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
/*  F i l e               &F: PARS_Types.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  General xml parser types get defined here.                               */
/*  This file includes all types that are used throughout the xml parser but */
/*  are not relevant for the porting.                                        */
/*  ATTENTION!!! Don't use internal structures in your code, they will be    */
/*  unaccessible in binary releases and they are all subject to change       */
/*  without any warning or information!                                      */
/*                                                                           */
/*****************************************************************************/

#ifndef __XMLPARSERTYPES_DEFINED__
#define __XMLPARSERTYPES_DEFINED__

/* ---  ------------------------------------------------------------------------------------------------------------- --- *\
|   Includes                                                                                                    Includes   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */
#include "PARSP_Config.h"
#include "PARSP_Types.h"

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name General definitions                                                                                             */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** Identifier for an illegal pointer */
#define PARS_ILLEGAL_PTR                    (PARS_LPVOID) -1
/* --- LSA_TRUE/LSA_FALSE------------------------------------------------------------------------------------------LSA_TRUE/LSA_FALSE --- */
#ifndef PARS_TRUE_STRING
    #define PARS_TRUE_STRING                "true"  /**< String representation for the boolean PARS_TRUE    */
#endif /* PARS_TRUE_STRING */

#ifndef PARS_FALSE_STRING
    #define PARS_FALSE_STRING               "false" /**< String representation for the boolean PARS_FALSE   */
#endif /* PARS_FALSE_STRING */
/** Macro to avoid PARS_strlen() calls for constant strings */
#define PARS_STRLEN_FROM_CONST_STRING(x)    (sizeof(x)-1)
/** Length to use for telling that the string is zero terminated */
#define PARS_ZERO_TERMINATED_STRING         0xFFFFFFFFl
/**@}*/

/* --- PARS_PT_String----------------------------------------------------------------------------------PARS_PT_String --- */
/** The string structure. Much better than simple zero terminated strings   */
typedef struct  PARS_S_String
{
    PARS_UINT   nMagic       : 16;  /**< Magic sequence used for recognizing xml parser strings                           */
    PARS_UINT   nReserved    : 8;   /**< for future use                                                                   */
    PARS_UINT   chSave       : 8;   /**< Save Character when set to ANSI-C String                                         */
    PARS_DWORD  dwStringLen;        /**< Length of the string                                                             */
    PARS_LPSTR  strContent;         /**< Pointer to the string                                                            */
} PARS_T_String, *PARS_PT_String;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Handling of xml parser strings                                                                                     */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** The magic bit that identifies a PARS_PT_String */
#define PARS_MSTR_MAGIC                 0x7AFC
/** Access to the last byte of the string */
#define PARS_MSTR_LastPos(x)            (MSTR_Raw(x)+MSTR_Len(x))
typedef struct  PARS_internal_S_String
{
    PARS_UINT   nMagic       : 16;  /**< Magic sequence used for recognizing miniweb strings                              */
    PARS_UINT   nReserved    : 8;   /**< for future use                                                                   */
    PARS_UINT   chSave       : 8;   /**< Save Character when set to ANSI-C String                                         */
    PARS_DWORD  dwStringLen;        /**< Length of the string                                                             */
    PARS_LPSTR  strContent;         /**< Pointer to the string                                                            */
} PARS_internal_T_String, *PARS_internal_PT_String;
/** Initializer for an empty string, for internal use only */
#define PARS_MSTR_Init(x) ( ((PARS_internal_PT_String)(x))->nMagic     = PARS_MSTR_MAGIC,      \
                            ((PARS_internal_PT_String)(x))->nReserved  = 0,                    \
                            ((PARS_internal_PT_String)(x))->chSave     = '\0',                 \
                            ((PARS_internal_PT_String)(x))->strContent = PARS_NULL,            \
                            ((PARS_internal_PT_String)(x))->dwStringLen= 0)


/** Helper to get the string into a printable format */
#define MSTR_SPRINT(x)   MSTR_Len(x), MSTR_Len(x), MSTR_Raw(x)
/** Macro to decide if the given string is a PARS_PT_String or a normal PARS_LPSTR */
/* Magic test must be alignment safe */
#if PARS_LITTLE_ENDIAN
  #define PARS_MSTR_IS_C_STRING(x)      (  x?(((*(PARS_BYTE*)x)!=(PARS_MSTR_MAGIC & 0xFF))   || ((*(((PARS_BYTE*)x)+1))!=((PARS_MSTR_MAGIC & 0xFF00)>>8))):1 )
#else /* Big Endian */
  #define PARS_MSTR_IS_C_STRING(x)      (  x?(((*(PARS_BYTE*)x)!=((PARS_MSTR_MAGIC & 0xFF00)>>8))  || ((*(((PARS_BYTE*)x)+1))!= (PARS_MSTR_MAGIC & 0xFF))):1 )
#endif /* #if PARS_LITTLE_ENDIAN */
/** Casting macro to make a string from a zero terminated PARS_LPSTR */
#define MSTR_C(x)      (PARS_MSTR_IS_C_STRING(x)?((PARS_PT_String)x):((PARS_PT_String)PARS_NULL))
/** Casting macro to make a string from a static zero terminated PARS_LPSTR */
#define MSTR_STATIC_C(x) ((PARS_LPSTR)x)

/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Constants for data representation (numeric variable types)                                                      */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#define PARS_VT_EMPTY                   0x0000  /**< Empty type...                                                        */
#define PARS_VT_I2                      0x0001  /**< Two byte signed integer                                              */
#define PARS_VT_I4                      0x0002  /**< Four byte signed integer                                             */
#define PARS_VT_R4                      0x0003  /**< Four byte floating point                                             */
#define PARS_VT_R8                      0x0004  /**< Eight byte floating point                                            */
#define PARS_VT_CY                      0x0005  /**< Not supported                                                        */
#define PARS_VT_BSTR                    0x0006  /**< Not supported                                                        */
#define PARS_VT_ERROR                   0x0007  /**< PARS_RESULT                                                          */
#define PARS_VT_BOOL                    0x0008  /**< Boolean                                                              */
#define PARS_VT_VARIANT                 0x0009  /**< used only by OPC - XML                                               */
#define PARS_VT_DECIMAL                 0x000A  /**< used by OPC-XML decimal is mapped to double, cause this is the largest data type xml parser has */
#define PARS_VT_I1                      0x000B  /**< One byte signed integer                                              */
#define PARS_VT_UI1                     0x000C  /**< One byte unsigned integer                                            */
#define PARS_VT_UI2                     0x000D  /**< Two byte unsigned integer                                            */
#define PARS_VT_UI4                     0x000E  /**< Four byte unsigned integer                                           */
#define PARS_VT_INT                     0x000F  /**< Signed integer                                                       */
#define PARS_VT_UINT                    0x0010  /**< Unsigned integer                                                     */
#define PARS_VT_HRESULT                 0x0011  /**< Error Result code                                                    */
#define PARS_VT_LPSTR                   0x0012  /**< Standard Zero terminated string                                      */
#define PARS_VT_PURE_LPSTR              0x0013  /**< This type can only be converted to/from LPSTR                        */
#define PARS_VT_LPWSTR                  0x0014  /**< Not supported                                                        */
#define PARS_VT_DATE                    0x0015  /**< is not convertible to R8                                             */
#define PARS_VT_UNLOCALIZED_DATE        0x0016  /**< at conversion to/from String: we hav no location info                */
#define PARS_VT_STREAM                  0x0017  /**< Stream, can be converted to String                                   */
#define PARS_VT_BASE64_STRING           0x0018  /**< BLOB as MBase64 encoded string                                       */
#define PARS_VT_BINARY_STRING           0x0019  /**< BLOB in binary representation                                        */
#define PARS_VT_UNKNOWN                 0x001A  /**< Datatype not (yet) known.                                            */
#define PARS_VT_GUID                    0x001B  /**< Globally unique identifier in struct form.                           */
#define PARS_VT_IMAGE                   0x001C  /**< a Image ( used by odbc to store images in a DB )                     */
#define PARS_VT_COMPLEX_TYPE            0x001D  /**< XML schema data type, see documentation                              */
#define PARS_VT_FORMATEDSTR             0x001E  /**< sprintf, sscanf format string, see documentation                     */
#define PARS_VT_ARRAY                   0x001F  /**< Array with elements                                                  */
#define PARS_VT_VALUE                   0x0020  /**< only allowed in an array structure                                   */
#define PARS_VT_OBJECT                  0x0021  /**< only allowed in an array structure                                   */
#define PARS_VT_XML_NODE                0x0022  /**< XML Dom Node, can be converted to String -> TagName                  */
#define PARS_VT_ENDARRAY                0x0023  /**< End Element of an C-like Value Array                                 */
#if PARS_DEFINES_64_BIT_DATATYPES
#  define PARS_VT_I8                    0x0024  /**< Eight byte signed integer                                            */
#  define PARS_VT_UI8                   0x0025  /**< Eight byte unsigned integer                                          */
#  define PARS_VT_START_COMPLEX_TYPES   0x0026  /**< Lowest ID of a complex data type                                     */
#else /* PARS_DEFINES_64_BIT_DATATYPES */
#  define PARS_VT_START_COMPLEX_TYPES   0x0024  /**< Lowest ID of a complex data type                                     */
#endif /* PARS_DEFINES_64_BIT_DATATYPES */

#define PARS_VT_RESERVED                0x8000  /**< Reserved                                                             */
#define PARS_VT_ILLEGAL                 0xFFFF  /**< Illegal datatype (fail safe value)                                   */
#define PARS_VT_ILLEGALMASKED           0x0FFF  /**<                                                                      */
#define PARS_VT_TYPEMASK                0x00FF  /**<                                                                      */

#if PARS_OM_SUPPORT_COMPLEX_TYPES
#  define PARS_MAX_TYPES                  PARS_VT_TYPEMASK                  /**< limits our type count                    */
#else /* PARS_OM_SUPPORT_COMPLEX_TYPES */
#  define PARS_MAX_TYPES                  (PARS_VT_START_COMPLEX_TYPES + 1) /**< limits our type count                    */
#endif /* PARS_OM_SUPPORT_COMPLEX_TYPES */
#define PARS_CONVERTABLE_TO_DOUBLE      PARS_VT_LPSTR       /**< all types below that one are convertible to a DOUBLE     */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Stream Data structures                                                                                             */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/** Stream structure */
typedef struct  PARS_internal_S_Stream
{
    PARS_DWORD              dwStreamSize;               /**< Current end of stream                                        */
    PARS_DWORD              dwActualPosition;           /**< Current Read or Write Pointer                                */
    PARS_DWORD              dwRawStreamSize;            /**< Size of allocated Buffer                                     */
    PARS_WORD               wStreamFlags;               /**< Must stream be freed                                         */
    PARS_CHAR               chSave;                     /**< Temporary character storage when written a temp zero char    */
    PARS_LPSTR              strStream;                  /**< Stream Data                                                  */
} PARS_internal_T_Stream, *PARS_internal_PT_Stream;

#ifdef _DEBUG
    typedef PARS_internal_T_Stream   PARS_T_Stream, *PARS_PT_Stream;
#else  /* _DEBUG */
    typedef struct _PARS_S_Stream
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_internal_T_Stream)];
    } PARS_T_Stream, *PARS_PT_Stream;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Current position of the stream */
typedef struct  PARS_S_StreamPos
{
    PARS_DWORD              dwStreamSize;               /**< Current end of stream                                        */
    PARS_DWORD              dwActualPosition;           /**< Current Read or Write Pointer                                */
} PARS_T_StreamPos, *PARS_PT_StreamPos;
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name XML Definitions                                                                                                 */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#define        XML_VERSIONLEN           10              /**< Max Len of Version string                                    */
#define        XML_ENCODELEN            20              /**< Max Len of Encoding Value                                    */
#define        XML_STANDALONELEN         4              /**< Max Len of Encoding Value                                    */
#define        XML_BOMLEN                8              /**< Max Len of Byte Order Mask                                   */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name XML Data structures                                                                                             */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
typedef PARS_LPSTR         XML_TEXT_POS;                /**< represents a Text Position in an XML Document                */
typedef PARS_LPSTR         XML_DOC;                     /**< represents a an XML Document                                 */

/* ---------------------------------------------------------------------------------------------------------------------- */
/**
 * The T_XML_Sheet represents the syntax of a complete XML document.
 *
 * @warning The XML Parser will not safe any portions of the given XML document, but only
 *          handles pointers into this document. So the XML Sheet is invalid when deleting
 *          or moving the underlying XML Document.
 * */
typedef struct PARS_XML_S_internalSheet
{
    /** A couple of flags */
    struct S_XMLSheetOptions
    {
        PARS_INT        bfStandAlone:1;                 /**< Weather there are nested documents                           */
        PARS_INT        bFreeOrigXMLDoc:1;              /**< Delete Doc Pointer at DeleteSheet()                          */
    } XML_SheetOptions;
    /** XML Version of this document (normally 1.0)                                                                       */
    PARS_CHAR           strVersion[XML_VERSIONLEN];
    /** The encoding of the document (something like "US-ASCII")                                                          */
    PARS_CHAR           strEncoding[XML_ENCODELEN];
    /** Indicates if doc is standalone                                                                                    */
    PARS_CHAR           strStandAlone[XML_STANDALONELEN];
    /** Byte Order Mask of the document                                                                                   */
    PARS_CHAR           strBom[XML_BOMLEN];
    /** Pointer to the beginning of the underlying XML document                                                           */
    XML_TEXT_POS        posXMLDoc;
    /** Pointer after the last char of the XML document (surely access to the content of this pointer is illegal, he?)    */
    XML_TEXT_POS        posXMLDocEnd;
    /** A pointer to the currently parsed text position which is a working pointer and shall not be used (i.e. it is
     *  forbidden to do that                                                                                              */
    XML_TEXT_POS        posActualCursor;
    /** Sync access to sheet                                                                                              */
    PARS_LPVOID         pvSyncAccessSheetHandle;
    /** Pointer to the first processing instruction                                                                       */
    struct PARS_XML_S_internalPI*     p_piRoot;
    /** Pointer to the root node of this document (represents the root-tag in the XML document)                           */
    struct PARS_XML_S_internalNode*   p_ndeRoot;
    /** Contains the DTD if available                                                                                     */
    XML_TEXT_POS        posXMLDTD_Data;
    /** Len of the DTD                                                                                                    */
    PARS_DWORD          dwDTD_DataLen;
    /** Preferred DTD                                                                                                     */
    PARS_LPSTR          szPrefDTD_Data;
    /** Len of Preferred DTD                                                                                              */
    PARS_DWORD          dwPrefDTD_DataLen;
    /** Doc Pointer                                                                                                       */
    PARS_LPSTR          strXMLOrigXMLDoc;
} PARS_XML_T_internalSheet, *PARS_XML_PT_internalSheet;

#ifdef _DEBUG
    typedef PARS_XML_T_internalSheet   PARS_XML_T_Sheet, *PARS_XML_PT_Sheet, *PARS_XML_Sheet;
#else  /* _DEBUG */
    typedef struct _PARS_XML_Sheet
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_XML_T_internalSheet)];
    } PARS_XML_T_Sheet, *PARS_XML_PT_Sheet, *PARS_XML_Sheet;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
/** The PARS_XML_T_internalNode is the syntax representation of a XML-tag                                                 */
typedef struct PARS_XML_S_internalNode
{
    XML_TEXT_POS            posXMLTagName;              /**< Pointer to Tag Name                                          */
    PARS_LPSTR              szPrefTagName;              /**< Pointer to Preferred Tag Name                                */
    PARS_DWORD              dwPrefTagNameLen;           /**< Length of Preferred Tag Name                                 */

    struct PARS_XML_T_internalAttributes*   p_atrRoot;  /**< List of Attributes                                           */

    XML_TEXT_POS            posXMLTagStart;             /**< Where this Tag start in the Text                             */
    XML_TEXT_POS            posXMLNextTag;              /**< Pointer after the closing Tag                                */

    PARS_DWORD              dwNodeType;                 /**< type of the node                                             */
    PARS_DWORD              dwFlags;                    /**< flags of internal state                                      */
    PARS_DWORD              dwChildCount;               /**< Count of the existing children                               */

    struct PARS_XML_S_internalNode* p_ndeParent;        /**< Pointer to Parent Node                                       */
    struct PARS_XML_S_internalNode* p_ndeFirstChild;    /**< Pointer to first Child Node                                  */
    struct PARS_XML_S_internalNode* p_ndeLastChild;     /**< Pointer to last Child Node                                   */
    struct PARS_XML_S_internalNode* p_ndePrevNeighbour; /**< Pointer to previous Neighbor                                 */
    struct PARS_XML_S_internalNode* p_ndeNextNeighbour; /**< Pointer to next Neighbor                                     */

    struct PARS_XML_S_internalSheet*      p_shtXMLSheet;/**< Pointer to enclosing XML Sheet                               */

    XML_TEXT_POS            posXMLContentStart;         /**< Pointer to the Beginning                                     */
                                                        /**< of the TAG-Content! in XML Doc                               */
    PARS_DWORD              dwContentLen;               /**< Length of the Content                                        */
    PARS_LPSTR              szPrefContent;              /**< Pointer to Preferred Content                                 */
    PARS_DWORD              dwPrefContentLen;           /**< Length of Preferred Content                                  */

    PARS_DWORD              dwTagNameLen;               /**< Length of Tag Name                                           */

    PARS_DWORD              dwOriginIndent;             /**< Indent count for this node                                   */
    PARS_DWORD              dwOriginIndentClosingTag;   /**< Indent count for the closing tag of this node                */
} PARS_XML_T_internalNode, *PARS_XML_PT_internalNode;

#ifdef _DEBUG
    typedef PARS_XML_T_internalNode   PARS_XML_T_Node, *PARS_XML_PT_Node, *PARS_XML_Node;
#else  /* _DEBUG */
    typedef struct _PARS_XML_Node
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_XML_T_internalNode)];
    } PARS_XML_T_Node, *PARS_XML_PT_Node, *PARS_XML_Node;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
/**
 * The PARS_XML_T_internalAttributes is a simple list of Node - Attributes. A node attribute is a name-value pair. The
 * name and value are always strings.
 * */
typedef    struct PARS_XML_T_internalAttributes
{
    XML_TEXT_POS    posXMLName;                         /**< pointer to attribute name                                    */
    PARS_DWORD      dwNameLen;                          /**< len of attribute name                                        */
    XML_TEXT_POS    posXMLValue;                        /**< pointer to attribute value                                   */
    PARS_DWORD      dwValueLen;                         /**< len of attribute value                                       */

    PARS_LPSTR      szPrefName;                         /**< preferred name of the attribute (string)                     */
    PARS_DWORD      dwPrefNameLen;                      /**< len of preferred name of attribute                           */
    PARS_LPSTR      szPrefValue;                        /**< preferred value of attribute                                 */
    PARS_DWORD      dwPrefValueLen;                     /**< len of preferred value of attribute                          */

    PARS_BOOL       bAlreadySorted;                     /**< Flag used for sorting attribute lists for C14N               */
    struct PARS_XML_T_internalAttributes*    pNextAttribute;/**< pointer to next attribute in list                        */
} PARS_XML_T_internalAttributes, *PARS_XML_PT_internalAttributes;

#ifdef _DEBUG
    typedef PARS_XML_T_internalAttributes   PARS_XML_T_Attributes, *PARS_XML_PT_Attributes, *PARS_XML_Attributes;
#else  /* _DEBUG */
    typedef struct _PARS_XML_Attributes
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_XML_T_internalAttributes)];
    } PARS_XML_T_Attributes, *PARS_XML_PT_Attributes, *PARS_XML_Attributes;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Processing instruction structure                                                                                      */
typedef    struct PARS_XML_S_internalPI
{
    XML_TEXT_POS                    posXMLPI;       /**< pointer to processing instruction                                */
    PARS_DWORD                      dwPILen;        /**< len of processing instruction                                    */

    PARS_LPSTR                      szPrefPI;       /**< preferred processing instruction                                 */
    PARS_DWORD                      dwPrefPILen;    /**< len of preferred processing instruction                          */

    struct PARS_XML_S_internalPI*   pNextPI;        /**< pointer to next processing instruction                           */
} PARS_XML_T_internalPI, *PARS_XML_PT_internalPI;

#ifdef _DEBUG
    typedef PARS_XML_T_internalPI   PARS_XML_T_PI, *PARS_XML_PT_PI, *PARS_XML_PI;
#else  /* _DEBUG */
    typedef struct _PARS_XML_PI
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_XML_T_internalPI)];
    } PARS_XML_T_PI, *PARS_XML_PT_PI, *PARS_XML_PI;
#endif /* _DEBUG */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name XPATH Data structures                                                                                           */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
#if PARS_XML_ENABLE_XPATH
/* --- PARS_XML_PT_NodeSet------------------------------------------------------------------------PARS_XML_PT_NodeSet --- */
/* TODO Frank */
  typedef struct PARS_XML_S_internalNodeSet
  {
    PARS_DWORD  dwNodeCount;
    PARS_DWORD  dwMaxNodeCount;
    PARS_BOOL   bIsStringArray;
    union
    {
        PARS_XML_Node*   ndeArray;
        PARS_LPSTR*      pstrAttributes;
    } Content;
  } PARS_XML_T_internalNodeSet, *PARS_XML_PT_internalNodeSet;

  #ifdef _DEBUG
    typedef PARS_XML_T_internalNodeSet   PARS_XML_T_NodeSet, *PARS_XML_PT_NodeSet, *PARS_XML_NodeSet;
  #else  /* _DEBUG */
    typedef struct _PARS_XML_NodeSet
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_XML_T_internalNodeSet)];
    } PARS_XML_T_NodeSet, *PARS_XML_PT_NodeSet, *PARS_XML_NodeSet;
  #endif /* _DEBUG */
#endif /* PARS_XML_ENABLE_XPATH */
/**@}*/

/* ---------------------------------------------------------------------------------------------------------------------- */
/** @name Common data structures                                                                                          */
/* ---------------------------------------------------------------------------------------------------------------------- */
/**@{*/
/* ---------------------------------------------------------------------------------------------------------------------- */
/** Hidden representaion of the ConnId */
#ifdef _DEBUG
    typedef struct PARS_S_CONNID*   PARS_HCONN;
#else  /* _DEBUG */
    typedef PARS_LPVOID             PARS_HCONN;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Our simple time structure                                                                                             */
typedef struct _PARS_Time_t
{
    PARS_LONG        loTime;        /**< The time as long                       */
    PARS_DWORD       dwMilliSec;    /**< The milli seconds                      */
} PARS_TIME_T, *PARS_PT_TIME;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Our used-in-almost-every-module value struct */
typedef struct _PARS_internal_S_Value
{
    PARS_LPVOID                 RawValue;               /**< Pointer to the value or the value itself (when Scalar is set)*/
    PARS_DWORD                  dwRawValueLength;       /**< Length of the value                                          */
    PARS_DWORD                  dwDataType;             /**< DataType of the value                                        */
    PARS_UINT                   bIsScalar           :1; /**< Memory behavior etc.                                         */
    PARS_UINT                   bIsValid            :1; /**< Set to LSA_TRUE when a Raw Value is set                          */
    PARS_UINT                   bFreeExternalMem    :1; /**< Set to LSA_TRUE when a Raw Value is set                          */
} PARS_internal_T_Value;

#ifdef _DEBUG
    typedef PARS_internal_T_Value PARS_T_Value, *PARS_PT_Value;
#else  /* _DEBUG */
    typedef struct _PARS_S_Value
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_internal_T_Value)];
    } PARS_T_Value, *PARS_PT_Value;
#endif /* _DEBUG */

/* ---------------------------------------------------------------------------------------------------------------------- */
#define PARS_ARRAY_ELEMENT_COUNT(array)(array==PARS_NULL?0:(PARS_NULL==((PARS_OMUTILS_PT_Array)array)->RawArray?0:(((PARS_OMUTILS_PT_Array)array)->dwUpperBound-((PARS_OMUTILS_PT_Array)array)->dwLowerBound+1)))

/** Structure dealing with array values */
typedef struct _PARS_internal_S_Array
{
    PARS_PT_Value               RawArray;               /**< Array of RawValues (with type described in dwDataType)       */
    PARS_DWORD                  dwLowerBound;           /**< Lower bound of array                                         */
    PARS_DWORD                  dwUpperBound;           /**< Upper bound of array                                         */
    PARS_DWORD                  dwElementSize;          /**< Size of an Array element                                     */
    PARS_DWORD                  dwDataType;             /**< DataType of the value                                        */
} PARS_internal_T_Array;

#ifdef _DEBUG
    typedef PARS_internal_T_Array   PARS_T_Array, *PARS_PT_Array;
#else  /* _DEBUG */
    typedef struct _PARS_S_Array
    {
        PARS_DWORD   dwSpare[PARS_DWORD_SIZEOF(PARS_internal_T_Array)];
    } PARS_T_Array, *PARS_PT_Array;
#endif /* _DEBUG */

#define PARS_GET_VALUE_FROM_ARRAY(Array, ElementSize, Index)     (PARS_PT_Value)(((PARS_LPBYTE)Array)+ElementSize*Index)

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Property struct. This struct contains an additional property. See aProperties in the transport struct. */
typedef struct PARS_S_Property
{
    PARS_T_Value                valProperty;            /**< Value of the Property                                        */
    PARS_DWORD                  dwPropertyType;         /**< Which Property, (See OPC Spec for a type map), the           */
    PARS_RESULT                 resResultID;            /**< A Property dependent Result ID                               */
    PARS_TIME_T                 tTimeStamp;
} PARS_T_Property, *PARS_PT_Property;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Array of properties */
typedef struct PARS_S_Properties
{
    PARS_BOOL                   bReturnPropertyValues;  /**< Must Property values be returned                             */
    PARS_DWORD                  dwCount_Properties;     /**< Property count                                               */
    PARS_PT_Property            parr_Property;          /**< Array of Property                                            */
} PARS_T_Properties, *PARS_PT_Properties;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Structure carrying the read data information for handling post requests                                               */
typedef struct S_ReadDataStruct
{
    PARS_LPBYTE      pBuffer;       /**< Buffer providing the data              */
    PARS_DWORD       dwBufferLen;   /**< Length of the data                     */
} PARS_T_ReadDataStruct, *PARS_PT_ReadDataStruct;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Our a-bit-more-elaborate time structure                                                                               */
typedef struct PARS_S_SYS_TM
{
    PARS_INT    woSec;              /**< seconds after the minute - [0,59]                          */
    PARS_INT    woMin;              /**< minutes after the hour - [0,59]                            */
    PARS_INT    woHour;             /**< hours since midnight - [0,23]                              */
    PARS_INT    woDay;              /**< day of the month - [1,31]                                  */
    PARS_INT    woMon;              /**< months since January - [0,11]                              */
    PARS_INT    woYear;             /**< years since 1900                                           */
    PARS_INT    woWDay;             /**< days since Sunday - [0,6]                                  */
    PARS_INT    woYDay;             /**< days since January 1 - [0,365]                             */
    PARS_INT    woIsDst;            /**< daylight savings time flag                                 */
    PARS_INT    dwMSec;             /**< MilliSeconds                                               */
} PARS_T_SYS_TM, *PARS_PT_SYS_TM;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** Find data for file system stuff                                                                                       */
typedef struct PARS_S_FindData
{
    PARS_DWORD       dwAttrib;                          /**< Attributes of the file                 */
    PARS_LONG        time_create;                       /**< -1 for FAT file systems                */
    PARS_LONG        time_access;                       /**< -1 for FAT file systems                */
    PARS_LONG        time_write;                        /**< time of last write access to the file  */
    PARS_DWORD       dwSize;                            /**< size of the file                       */
    PARS_CHAR        name[PARS_FILES_MAX_PATH_LENGTH];  /**< name of the file                       */
} PARS_T_FindData, *PARS_PT_FindData;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** GUID structure                                                                                                        */
typedef struct PARS_S_GUID
{
    PARS_ULONG   Data1;                 /**< First data block of the GUID           */
    PARS_USHORT  Data2;                 /**< Second data block of the GUID          */
    PARS_USHORT  Data3;                 /**< Third data block of the GUID           */
    PARS_UCHAR   Data4[8];              /**< Fourth data block of the GUID          */
} PARS_T_GUID, *PARS_PT_GUID;

/* --------------------------------------------------------------------------------------------------- */
/** A Byte String is used in node IDs for opaque data*/
typedef struct PARS_S_ByteString
{
    PARS_LONG   loLength;               /**< The length of the byte string          */
    PARS_LPBYTE pbyData;                /**< The actual data of the byte string     */
} PARS_T_ByteString, *PARS_PT_ByteString;

/* ---------------------------------------------------------------------------------------------------------------------- */
/** The QualifiedName structure. Composed from an index and a name                                                        */
typedef struct PARS_S_QualifiedName
{
    PARS_DWORD      dwNamespaceIndex;   /**< The index in the namespace             */
    PARS_PT_String  mstrName;           /**< The name used for the qualified name   */
} PARS_T_QualifiedName, *PARS_PT_QualifiedName;

/* --------------------------------------------------------------------------------------------------- */
/** Enumeration for the different ID types */
typedef enum PARS_S_E_IdType {
    PARS_E_IdType_Numeric = 0x00,    /**< Indicates a numeric ID                        */
    PARS_E_IdType_String  = 0x01,    /**< Indicates a String ID                         */
    PARS_E_IdType_Guid    = 0x02,    /**< Indicates a GUID ID                           */
    PARS_E_IdType_Opaque  = 0x03,    /**< Indicates an opaque ID                        */
    PARS_E_IdType_Unknown = 0x04,    /**< Indicates an unknown identifier (no support in any related function) */
    PARS_E_IdType_Invalid = 0xFF     /**< Indicates an invalid ID type                  */
} PARS_E_IdType;

/* --------------------------------------------------------------------------------------------------- */
/** @name Predefined namespace indices
 * @{*/
/* --------------------------------------------------------------------------------------------------- */
/** Namespace index for OPC UA                                                                      */
#define PARS_AM_NSI_OPCUA                           0x00000000
/** String for the OPC UA namespace                                                                 */
#define PARS_AM_NSI_OPCUA_TEXT                      "http://opcfoundation.org/UA/"
/** Namespace index for local server                                                                */
#define PARS_AM_NSI_LOCALSERVER                     0x00000001
/** String for the local server namespace                                                           */
#define PARS_AM_NSI_LOCALSERVER_TEXT                "LocalServer"
/** Lowest possible user namespace index                                                            */
#define PARS_AM_NSI_USER_START                      0x00000002
/** Indicates that a provider should be asked for any namespace                                     */
#define PARS_AM_NSI_ANY                             PARS_AM_MAX_NAMESPACES
/** Indicates that no namespace index is used. Possible when a reference gets identified by NodeId  */
#define PARS_AM_NSI_NONE                            0xFFFFFFFF
/** highest possible namespace index the address modeler will handle for persistence                */
#define PARS_AM_NSI_ADDRESSMODELLER                 0xFFFFFFFE
/**@}*/

typedef struct PARS_S_Identifier
{
    PARS_DWORD          dwPreKey;           /**< Additional separating info for the data, might be '0'  */
    PARS_E_IdType       eIdType;            /**< Type of the Identifier                                 */
    union{
        PARS_DWORD          number;
        PARS_PT_String      string;
        PARS_PT_ByteString  blob;
        PARS_PT_GUID        guid;
        PARS_LPVOID         any;
    }data;
} PARS_T_Identifier, *PARS_PT_Identifier;
#define PARS_INIT_T_IDENTIFIER {PARS_AM_NSI_NONE, PARS_E_IdType_Invalid, {0}}
#define PARS_SET_OPCUA_ID(Identifier, Numeric) {(Identifier)->data.number = Numeric;                \
                                                (Identifier)->eIdType     = PARS_E_IdType_Numeric;  \
                                                (Identifier)->dwPreKey    = PARS_AM_NSI_OPCUA;}

/* ---------------------------------------------------------------------------------------------------------------------- */
/** The LocalizedText structure is used to store a text in a certain language */
/* --------------------------------------------------------------------------------------------------- */
/** The local id enumeration - which MUST fit to PARS_AM_g_LocalizedIdArray entries*/
typedef enum PARS_internal_E_LocalId
{
    PARS_E_InvalidLanguage  = 0x0000,
    PARS_E_English          = 0x0001
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 1
    ,
    PARS_E_German           = 0x0002
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 2
    ,
    PARS_E_French           = 0x0003
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 3
    ,
    PARS_E_Italian          = 0x0004
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 4
    ,
    PARS_E_Spanish          = 0x0005
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 5
    ,
    PARS_E_Russian          = 0x0006
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 6
    ,
    PARS_E_Turkish          = 0x0007
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 7
    ,
    PARS_E_Chinese          = 0x0008
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 8
    ,
    PARS_E_Taiwanese        = 0x0009
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 9
    ,
    PARS_E_Japanese         = 0x000A
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 10
    ,
    PARS_E_Arabic           = 0x000B
#endif
#if PARS_AM_MAX_SUPPORTED_LANGUAGES > 11
#pragma error "To many languages defined"
#endif
} PARS_E_LocalId;

typedef struct PARS_S_LocalizedText
{
    PARS_E_LocalId  enuLocaleId;                                   /**< numeric Locale for the text (e.g. "de")           */
    PARS_PT_String  mstrText;                                      /**< Translated string in the given locale             */
} PARS_T_LocalizedText, *PARS_PT_LocalizedText;
/**@}*/

/**
 * @def     PARS_SwapDouble
 * @brief
 * */

/*--- PARS_SwapDouble() ---------------------------------------------------------------------------- PARS_SwapDouble() ---*/
/**
 * @def     PARS_SwapDouble
 *
 * @brief   If the system has a mixed double endianess (strange, but such shall exist), This method brings xSrc into
 *          the right order
 *
 * @param   xDst    Memory to take the swapped value
 * @param   xSrc    Pointer providing the value to be swapped
 * @param   xCount  size of xSrc
 */
/*--- PARS_SwapDouble() ---------------------------------------------------------------------------- PARS_SwapDouble() ---*/
#if PARS_DOUBLE_MIXED_ENDIAN
    #define PARS_SwapDouble(xDst, xSrc, xCount) \
    { \
        PARS_ULONG ii = 0; \
        PARS_ULONG jj = 1; \
        PARS_ULONG* dst = (PARS_ULONG*)xDst; \
        PARS_ULONG* src = (PARS_ULONG*)xSrc; \
        \
        for (; ii < 2; ii++, jj--) \
        { \
            PARS_SwapBytes(&dst[ii], &src[jj], sizeof(PARS_ULONG)); \
        } \
    }
#else /* PARS_DOUBLE_MIXED_ENDIAN */
    #define PARS_SwapDouble(xDst, xSrc, xCount) \
    { \
        PARS_SwapBytes(xDst, xSrc, xCount); \
    }
#endif /* PARS_DOUBLE_MIXED_ENDIAN */

/*--- PARS_SwapBytes() ------------------------------------------------------------------------------ PARS_SwapBytes() ---*/
/**
 * @def     PARS_SwapBytes
 *
 * @brief   On LITTLE_ENDIAN machines, this macro does nothing. On BIG_ENDIAN machines, this macro swaps the byte order
 *          of xSrc and copies the result into xDst.
 *
 * @param   xDst    Memory to take the swapped value
 * @param   xSrc    Pointer providing the value to be swapped
 * @param   xCount  size of xSrc
 */
/*--- PARS_SwapBytes() ------------------------------------------------------------------------------ PARS_SwapBytes() ---*/
#if PARS_LITTLE_ENDIAN
    #define PARS_SwapBytes(xDst, xSrc, xCount) \
    { \
        PARS_memcpy(xDst, xSrc, xCount); \
    }
#else /* PARS_LITTLE_ENDIAN */
    #define PARS_SwapBytes(xDst, xSrc, xCount) \
    { \
        PARS_ULONG ii = 0; \
        PARS_ULONG jj = xCount-1; \
        PARS_BYTE* dst = (PARS_BYTE*)xDst; \
        PARS_BYTE* src = (PARS_BYTE*)xSrc; \
        \
        for (; ii < xCount; ii++, jj--) \
        { \
            dst[ii] = src[jj]; \
        } \
    }
#endif /* PARS_LITTLE_ENDIAN */

#if 0
#define PARS_AUTH_RANDOM_LEN 16
#pragma pack (push, 1)
/* --- PARS_AUTHD_PT_nonceStruct------------------------------------------------------------PARS_AUTHD_PT_nonceStruct --- */
typedef struct PARS_AUTHD_S_nonceStruct
{
    PARS_LPVOID     pvNonceUserData;
    PARS_TIME_T     tiTime;
    PARS_CHAR       random[PARS_AUTH_RANDOM_LEN];
} PARS_AUTHD_T_nonceStruct, *PARS_AUTHD_PT_nonceStruct;
/* --- PARS_AUTHD_PT_HexNonce------------------------------------------------------------------PARS_AUTHD_PT_HexNonce --- */
typedef struct PARS_AUTHD_S_HexNonce
{
  char h[5+2*sizeof(PARS_AUTHD_T_nonceStruct)] ;
} PARS_AUTHD_T_HexNonce, *PARS_AUTHD_PT_HexNonce;
#pragma pack (pop)
#endif /* 0 */

#endif /* __XMLPARSERTYPES_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

