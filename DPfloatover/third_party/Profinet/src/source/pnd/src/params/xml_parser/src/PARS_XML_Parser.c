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
/*  F i l e               &F: PARS_XML_Parser.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of XML DOM Parser                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3043
#define PND_MODULE_ID       3043


/*----------------------------------------------------------------------------------------------------*\
|   Includes                                                                                Includes   |
\*----------------------------------------------------------------------------------------------------*/
#include "PARSP_Utils.h"
#include "PARS_MCB.h"                              /* Management for Services/Applications/Filter */
#include "PARS_MemoryService.h"              /* Direct usage of memory Service only with mempools */
#include "PARS_FileSystem.h"                                                /* file system access */
#include "PARS_ConversionService.h"

#include "PARS_XML.h"
#include "PARS_XML_Internal.h"                                        /* Prototypes of XML Parser */

#include "pnd_trc.h"
#include "pnd_trc.h"
#include "pnd_xml_cfg.h"

/*----------------------------------------------------------------------------------------------------*\
|   Disable junk warnings                                                      Disable junk warnings   |
\*----------------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma warning (disable : 4127)    /* Conditional Expression is constant */
#endif

LSA_CHAR* xmlBuffer; //TODO
PARS_DWORD xmlBufferLength = 0; //TODO


/*----------------------------------------------------------------------------------------------------*\
|  static Globals                                                                     static Globals   |
\*----------------------------------------------------------------------------------------------------*/

#if PARS_USE_MEM_POOL
  extern PARS_MEM_POOL_HANDLE PARS_XML_g_pMemPoolAttributes;
  extern PARS_MEM_POOL_HANDLE PARS_XML_g_pMemPoolNodes;
  extern PARS_MEM_POOL_HANDLE PARS_XML_g_pMemPoolSheets;
#endif /* PARS_USE_MEM_POOL */

static PARS_RESULT DLLCALL PARS_XML_CreateNode      ( /*  in */ PARS_XML_Node           a_ndeNode,
                                                      /*  in */ PARS_DWORD              a_dwNavigationFlags,
                                                      /*  in */ PARS_DWORD              a_dwNodeType,
                                                      /* out */ PARS_XML_Node*          a_pndeValue);

static PARS_RESULT DLLCALL PARS_XML_ExpandNode        ( /*  in */ PARS_XML_Node a_ndeNode,
                                                        /*  in */ PARS_DWORD    a_dwActualDepth,
                                                        /*  in */ PARS_DWORD    a_dwExpandDepth);

static PARS_RESULT DLLCALL PARS_XML_DeleteNode        ( /*  in */ PARS_XML_Node a_ndeNode);

static PARS_RESULT DLLCALL PARS_XML_DeleteSheet       ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet);

/*----------------------------------------------------------------------------------------------------*\
|  Implementation                                                                     Implementation   |
\*----------------------------------------------------------------------------------------------------*/

/*--- PARS_XML_EnterSheet() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Enter exclusive access to a PARS_XML_Sheet                                                     |
|       Waits until access is granted                                                                  |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]     XML Sheet whose exclusive access is wanted                 |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_EnterSheet ( /*  in */ PARS_XML_Sheet        a_shtXMLSheet)
{
    PARS_XML_PT_internalSheet    shtWorkSheet    = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_EnterSheet");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);

    shtWorkSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if(shtWorkSheet->pvSyncAccessSheetHandle)
    {
        PARS_ENTER_CRITICAL_SECTION(shtWorkSheet->pvSyncAccessSheetHandle);
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_LeaveSheet() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Frees exclusive access to a XMLSheet                                                           |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]     XML Sheet whose exclusive access is freed                  |                                                                                                     |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_LeaveSheet ( /*  in */ PARS_XML_Sheet        a_shtXMLSheet)
{
    PARS_XML_PT_internalSheet    shtWorkSheet    = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_LeaveSheet");

    CHECK_NULL_PARAMETER(a_shtXMLSheet);

    shtWorkSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if(shtWorkSheet->pvSyncAccessSheetHandle)
    {
        PARS_LEAVE_CRITICAL_SECTION(shtWorkSheet->pvSyncAccessSheetHandle);
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_CreateSheet() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Create a XML Sheet ( including DOM-Tree or not )                                               |
|       takes a pointer to the XML-Text Document, and returns a Pointer to an XMLSheet Structure.      |
|       ONLY THE <?xml ... ?> is parsed.                                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_strXMLDocument        [  in ]     Pointer to XML Document                                    |
|       a_dwDepth               [  in ]     Depth of Expanding the Nodes (1= Only Root Level)          |
|       a_pshtXMLSheet          [ out ]     Pointer to XML Sheet ( including DOM-Tree )                |
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
static PARS_RESULT DLLCALL PARS_XML_CreateSheet( /*  in */ PARS_LPSTR      a_strXMLDocument,
                                                 /*  in */ PARS_BOOL       a_bFreeDocWhenDeleteSheet,
                                                 /*  in */ PARS_DWORD      a_dwDepth,
                                                 /* out */ PARS_XML_Sheet* a_pshtXMLSheet,
                                                 /* out */ PARS_LPDWORD    a_pdwParsedLen)

{
    PARS_XML_PT_internalSheet       shtWork         = PARS_NULL;
    XML_TEXT_POS                    posActual       = a_strXMLDocument;
    XML_TEXT_POS                    posDTD          = PARS_NULL;
    XML_TEXT_POS                    posStart        = PARS_NULL;
    PARS_BOOL                       bEndScan        = PARS_FALSE;
    PARS_LPSTR                      strBom          = PARS_NULL;
    PARS_DWORD                      dwBomLen        = 0;

    PARS_XML_PT_internalPI          piNext          = PARS_NULL;
    PARS_XML_PT_internalPI          piActual        = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_CreateSheet");
    CHECK_SHUTDOWN_MODE(XML);

    /* its allowed to specify no XMLDocument */
    /*CHECK_NULL_PARAMETER(a_strXMLDocument);*/

    CHECK_NULL_PARAMETER(a_pshtXMLSheet);
    *a_pshtXMLSheet = PARS_NULL;

    if(PARS_NULL != a_pdwParsedLen)
    {
        *a_pdwParsedLen = 0;
    }

    /* Handling of Unicode Byte Order Mark (BOM) in front of XML contents
       Byte sequences:
        EF BB BF        UTF-8       -> only this one will be accepted here
        FE FF           UTF-16/UCS-2, little endian
        FF FE           UTF-16/UCS-2, big endian
        FF FE 00 00     UTF-32/UCS-4, little endian.
        00 00 FE FF     UTF-32/UCS-4, big-endian.

     */
    if(a_strXMLDocument)
    {
        if ( ( (PARS_BYTE) a_strXMLDocument[0] == (PARS_BYTE) 0xFE) &&
             ( (PARS_BYTE) a_strXMLDocument[1] == (PARS_BYTE) 0xFF))
        {
            return MWE_INVALID_PARAMETER; /* only EF BB BF is allowed! */
        }

        if ( ( (PARS_BYTE) a_strXMLDocument[0] == (PARS_BYTE) 0xFF) &&
             ( (PARS_BYTE) a_strXMLDocument[1] == (PARS_BYTE) 0xFE))
        {
            return MWE_INVALID_PARAMETER; /* only EF BB BF is allowed! */
        }

        if ( ( (PARS_BYTE) a_strXMLDocument[0] == (PARS_BYTE) 0x00) &&
             ( (PARS_BYTE) a_strXMLDocument[1] == (PARS_BYTE) 0x00))
        {
            return MWE_INVALID_PARAMETER; /* only EF BB BF is allowed! */
        }

        if ( ( (PARS_BYTE) a_strXMLDocument[0] == (PARS_BYTE) 0xEF) &&
             ( (PARS_BYTE) a_strXMLDocument[1] == (PARS_BYTE) 0xBB) &&
             ( (PARS_BYTE) a_strXMLDocument[2] == (PARS_BYTE) 0xBF))
        {
                strBom = a_strXMLDocument;
                dwBomLen = 3;
                a_strXMLDocument += 3;
        }
    }
    BAIL_OUT_ON_FAIL_EX( PARS_XML_NewSheet((PARS_XML_Sheet*)&shtWork), PARS_DEBUG_LEVEL_INFO, "Create new sheet failed.\n");
    BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateSheetSync((PARS_XML_Sheet)shtWork), PARS_DEBUG_LEVEL_INFO, "Create new sheetsync failed.\n");
    shtWork->strXMLOrigXMLDoc                   = PARS_NULL;
    shtWork->XML_SheetOptions.bFreeOrigXMLDoc   = PARS_FALSE;
    if(a_strXMLDocument)
    {
        /* Fill out shtWork    */
        shtWork->strXMLOrigXMLDoc                   = a_strXMLDocument-dwBomLen;
        shtWork->XML_SheetOptions.bFreeOrigXMLDoc   = a_bFreeDocWhenDeleteSheet;
        shtWork->posXMLDoc                          = a_strXMLDocument;
        shtWork->posActualCursor                    = a_strXMLDocument;
        if(strBom)
        {
            PARS_strlcpy( shtWork->strBom,
                          strBom,
                          XML_BOMLEN,
                          dwBomLen);
        }
        shtWork->posXMLDocEnd   = a_strXMLDocument+PARS_strlen(a_strXMLDocument);

        /* ignore whitespace */

        posActual = PARS_XML_ScanNonWhiteSpace(shtWork->posActualCursor, START_OF_TAG);
        if ((posActual==PARS_NULL)||(posActual[0] != START_OF_TAG))
        {
            /* no < found at the begin of document */
            BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "XML-Parse: Error searching First Tag  ");
        }
        shtWork->posActualCursor=posActual;
        /* test if  <?xml at begin of document  (root tag is also allowed)*/
        /* prolog is not a must ( see http://www.w3.org/TR/2004/REC-xml-20040204/ Section 2.8 ) */
        if( 0 == PARS_strncmp(shtWork->posActualCursor, "<?xml",5 ))
        {
            shtWork->posActualCursor=shtWork->posActualCursor+4;
            posActual = PARS_XML_ParseAttribute( shtWork->posActualCursor,
                                                 XML_ATTR_VERSION,
                                                 shtWork->strVersion,
                                                 XML_VERSIONLEN);
            if( posActual== PARS_NULL )
            {
                PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: No Version Information, assuming 1.0.\n" );
            }
            posActual = PARS_XML_ParseAttribute( shtWork->posActualCursor,
                                                 XML_ATTR_ENCODING,
                                                 shtWork->strEncoding,
                                                 XML_ENCODELEN);
            if( posActual== PARS_NULL )
            {
                PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: No Encoding Information, assuming US-ASCII.\n" );
            }
            posActual = PARS_XML_ParseAttribute( shtWork->posActualCursor,
                                                 XML_ATTR_STANDALONE,
                                                 shtWork->strStandAlone,
                                                 5);
            if (PARS_strncmp(shtWork->strStandAlone,XML_ATTR_STANDALONE_VALUE_YES,PARS_strlen(XML_ATTR_STANDALONE_VALUE_YES))==0)
            {
                shtWork->XML_SheetOptions.bfStandAlone=1;
            }
            BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"Error parsing standalone attribute.\n");
            /* parse to end of XML Tag     */
            posActual = PARS_XML_ScanNextEncounter(shtWork->posActualCursor, "?>");
            shtWork->posActualCursor=posActual;
        }

        /* OK, The next must be the Root Tag, or DTD, or PI (Processing Instruction) */
        posActual = PARS_XML_ScanChar(shtWork->posActualCursor,START_OF_TAG,END_OF_TAG, PARS_TRUE);
        BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"No Root Tag found.\n");
        do
        {
            BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "End of tag expected 0.\n");
            bEndScan = PARS_TRUE;
            /* Check for PI */
            if (PARS_strncmp(posActual,"?",1)==0)
            {
                posActual++;
                posStart = posActual;
#if PARS_XML_VERBOSE_PARSING
                PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: PI before Root Node found.\n");
#endif /* PARS_XML_VERBOSE_PARSING */
                while (PARS_TRUE)
                {
                    posActual = PARS_XML_ScanChar(posActual, '?', 0, PARS_FALSE);
                    BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"'?' expected.\n");
                    if (PARS_strncmp(posActual,">",1)==0)
                    {
#if PARS_XML_VERBOSE_PARSING
                        PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: END OF PI found.\n");
#endif /* PARS_XML_VERBOSE_PARSING */
                        posActual+=1;
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_NewPI((PARS_XML_PI*)&piNext), PARS_DEBUG_LEVEL_INFO, " Create PI failed.\n");
                        if(!piActual)
                        {
                            shtWork->p_piRoot = piNext;
                        }
                        else
                        {
                            piActual->pNextPI = piNext;
                        }
                        piNext->posXMLPI= posStart;
                        piNext->dwPILen = posActual-posStart-2;  /*  ?> */
                        piActual = piNext;
                        break;
                    }
                }
                posActual = PARS_XML_ScanChar(posActual,START_OF_TAG,END_OF_TAG, PARS_TRUE);
                BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"Next Tag expected 1.\n");
                bEndScan = PARS_FALSE;
                continue;
            }
            /* Check for PI */
            if (PARS_strncmp(posActual,"!--",3)==0)
            {
                PARS_TRACE( PARS_DEBUG_LEVEL_WARNING, "XML-Parse: COMMENT before Root Node found.\n");
                while (PARS_TRUE)
                {
                    posActual = PARS_XML_ScanChar(posActual, '-', 0, PARS_FALSE);
                    BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"'-' expected.\n");
                    if (PARS_strncmp(posActual,"->",2)==0)
                    {
#if PARS_XML_VERBOSE_PARSING
                        PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: END OF COMMENT Found.\n");
#endif /* PARS_XML_VERBOSE_PARSING */
                        posActual+=2;
                        break;
                    }
                }
                posActual = PARS_XML_ScanChar(posActual,START_OF_TAG,END_OF_TAG, PARS_TRUE);
                BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"Next Tag expected 2.\n");
                bEndScan = PARS_FALSE;
                continue;
            }

            if (PARS_strncmp(posActual,"!DOCTYPE",8)==0)    /* is it a DTD     */
            {
                MCB->LookUpService("ConvSrvc",(PARS_LPVOID*)&PARS_XML_g_pConversion);
                if(PARS_XML_g_pConversion!=PARS_NULL)
                {
                    PARS_XML_g_pConversion->FindCorresponding( posActual,
                        PARS_ZERO_TERMINATED_STRING,
                        ">",
                        1,
                        &posDTD);
                }
                else
                {
                    posDTD=posActual;
                    do
                    {
                        posDTD = PARS_XML_ScanChar(posDTD, ']', 0, PARS_FALSE);
                        if (posDTD==PARS_NULL)
                        {
                            posActual=PARS_NULL; /* Error in DTD     */
                            break;
                        }
                    } while (posDTD[0]!=END_OF_TAG);
                }
                if (posActual != PARS_NULL)
                {
                    shtWork->dwDTD_DataLen=posDTD-posActual+2;
                    shtWork->posXMLDTD_Data=posActual-1;
                    posActual = posDTD+1;
                    posActual = PARS_XML_ScanChar(posActual,START_OF_TAG,END_OF_TAG, PARS_TRUE);
                    BAIL_OUT_ON_NULL_EX( posActual,PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR,"Next Tag expected 3.\n");
                }
                bEndScan  = PARS_FALSE;
            }
        } while (bEndScan==PARS_FALSE);
    }
    else
    {
        shtWork->posXMLDocEnd = PARS_NULL;
        PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: No <?xml Tag, assuming defaults.\n");
    }

    /* OK we have a Root Tag, lets rock!     */
    BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateNode( PARS_NULL,
                                              0,
                                              PARS_XML_NODE_TYPE_ELEMENT,
                                              (PARS_XML_Node*)&(shtWork->p_ndeRoot) ),
                         PARS_DEBUG_LEVEL_INFO,
                         "Create Root Node failed");

    shtWork->p_ndeRoot->p_shtXMLSheet   = shtWork;
    shtWork->p_ndeRoot->posXMLTagStart  = posActual;
    shtWork->p_ndeRoot->dwOriginIndent  = 0;
    if(posActual)
    {
        /* find and set root tag name */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_FindNextValue(&posActual,
                                                     '>',
                                                    &(shtWork->p_ndeRoot->posXMLTagName),
                                                    &(shtWork->p_ndeRoot->dwTagNameLen),
                                                     PARS_FALSE,
                                                     PARS_TRUE),
                             PARS_DEBUG_LEVEL_INFO,
                             "Find Root Tag Name failed");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_ExpandNode( (PARS_XML_Node) shtWork->p_ndeRoot, 1, a_dwDepth),
                             PARS_DEBUG_LEVEL_INFO,
                             " Expand nodes failed.\n");    /* Read The Attributes and Set Node Data,     */
                                                            /* but NOT Expand Root Node                   */
    }
    *a_pshtXMLSheet = (PARS_XML_Sheet) shtWork;
    if (a_pdwParsedLen!=PARS_NULL)
    {
        *a_pdwParsedLen = shtWork->p_ndeRoot->posXMLNextTag - shtWork->posXMLDoc;
    }
LANDING_ZONE;
    PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: Fatal: (No Root Tag) Error Parsing XML!.\n");
    if(shtWork)
    {
        PARS_XML_DeleteSheet((PARS_XML_Sheet)shtWork);
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteSheet() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       XMLDeleteSheet takes a pointer to a previously by XMLCreateSheeet()                            |
|       generated XML-Sheet and deletes all memory occupied by this Sheet.                             |
|       (Ergo: All Nodes, Attribute Tables etc.)                                                       |
|       Attention: Make sure that you have no pointers to Nodes etc. from this                         |
|       Sheet, they all will be invalid, after deletion.                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet          [  in ]       XML-Sheet to delete                                       |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_DeleteSheet ( /*  in */ PARS_XML_Sheet  a_shtXMLSheet)
{
    PARS_XML_PT_internalSheet    tmpSheet = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteSheet");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);

    tmpSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if (tmpSheet->p_piRoot!=PARS_NULL)
    {
        PARS_XML_DeleteAllPI((PARS_XML_PI)tmpSheet->p_piRoot);
        tmpSheet->p_piRoot=PARS_NULL;
    }

    if (tmpSheet->p_ndeRoot!=PARS_NULL)
    {
        PARS_XML_DeleteAllNodes( tmpSheet->p_ndeRoot );
        tmpSheet->p_ndeRoot=PARS_NULL;
    }

    BAIL_OUT_ON_FAIL_EX( PARS_XML_DeleteSheetSync(a_shtXMLSheet), PARS_DEBUG_LEVEL_INFO, "Delete sheet sync failed");

    if( ( tmpSheet->strXMLOrigXMLDoc != PARS_NULL  ) &&
        ( tmpSheet->XML_SheetOptions.bFreeOrigXMLDoc != PARS_FALSE ) )
    {

        if( PARS_XML_g_pMem != PARS_NULL )
        {
            PARS_XML_g_pMem->SMemFree(tmpSheet->strXMLOrigXMLDoc);
        }
        else
        {
            XML_SMemFree(tmpSheet->strXMLOrigXMLDoc);
        }
        tmpSheet->strXMLOrigXMLDoc = PARS_NULL;
    }
#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL )
    {
        PARS_XML_g_pMem->MemPoolFixFree(PARS_XML_g_pMemPoolSheets, tmpSheet);
    }
    else
    {
        XML_SMemFree(tmpSheet);
    }
#else /* PARS_USE_MEM_POOL */
    XML_SMemFree(tmpSheet);
#endif /* PARS_USE_MEM_POOL */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteAttribute() ---------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Deletes a specified attribute of the specified node                                            |
|       The attribute is searched in the attribute list of the node                                    |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode              [  in ]       node with attribute to delete                             |
|       a_strAttributeName     [  in ]       name of attribute to delete                               |
|       a_dwAttributeNameLen   [  in ]       length of the attribute name                              |
|       a_strAttributeValue    [  in ]       value of attribute to delete                              |
|       a_dwAttributeValueLen  [  in ]       length of the attribute value                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_MEMORY             memory allocation                                                      |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|        MWE_NOT_FOUND          attribute not found in attribute list of node                          |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_DeleteAttribute ( /*  in */ PARS_XML_Node   a_ndeNode,
                                                      /*  in */ PARS_LPSTR      a_strAttributeName,
                                                      /*  in */ PARS_DWORD      a_dwAttributeNameLen,
                                                      /*  in */ PARS_LPSTR      a_strAttributeValue,
                                                      /*  in */ PARS_DWORD      a_dwAttributeValueLen)
{
    PARS_XML_PT_internalAttributes  attrActual      = PARS_NULL;
    PARS_XML_PT_internalAttributes  attrPrev        = PARS_NULL;

    PARS_BOOL                       bNoNameSpace    = PARS_FALSE;
    PARS_LPSTR                      strName         = PARS_NULL;
    PARS_LPSTR                      strValue        = PARS_NULL;
    PARS_DWORD                      dwValueLen      = 0;
    PARS_LPSTR                      strTempName     = PARS_NULL;
    PARS_DWORD                      dwNameLen       = 0;
    PARS_BOOL                       fFound          = PARS_FALSE;
    PARS_XML_PT_internalNode        tmpNode         = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteAttribute");

    CHECK_NULL_PARAMETER(a_ndeNode);

    attrActual=(tmpNode->p_atrRoot);
    /* RETURN_ON_NULL(attrActual); */
    BAIL_OUT_ON_NULL_SILENT(attrActual, MWE_NOT_FOUND); /* em3702: if there is no attribute root no attribute will be found */

    if (PARS_strnchr(a_strAttributeName, ':', a_dwAttributeNameLen)==PARS_NULL)
    {
        /* No Name space required, ignore any namespace */
        bNoNameSpace=PARS_TRUE;
    }

    do
    {
        if(attrActual->szPrefName)
        {
            strName     = attrActual->szPrefName;
            dwNameLen   = attrActual->dwPrefNameLen;
        }
        else
        {
            strName     = attrActual->posXMLName;
            dwNameLen   = attrActual->dwNameLen;
        }
        if(attrActual->szPrefValue)
        {
            strValue    = attrActual->szPrefValue;
            dwValueLen  = attrActual->dwPrefValueLen;
        }
        else
        {
            strValue    = attrActual->posXMLValue;
            dwValueLen  = attrActual->dwValueLen;
        }

        if (PARS_FALSE != bNoNameSpace)
        {
            strTempName=PARS_strnchr(strName, ':' , dwNameLen);
            if (strTempName)
            {
                strTempName++;
                dwNameLen = dwNameLen - (strTempName - strName);
                strName = strTempName;
            }
        }
        if(PARS_XML_SAFE_strncmp(strName, a_strAttributeName, dwNameLen, a_dwAttributeNameLen)==0)
        {
            if (PARS_NULL!=strValue)
            {
                if (a_strAttributeValue==PARS_NULL)    /* Consumer is only interested in the existence */
                {                                      /* of the Attribute                             */
                    fFound = PARS_TRUE;
                }
                if( (a_strAttributeValue != PARS_NULL) && (PARS_XML_SAFE_strncmp(strValue, a_strAttributeValue, dwValueLen, a_dwAttributeValueLen)==0))
                {
                    fFound = PARS_TRUE;
                }
            }
            else
            {
                /* attribute has no value */
                fFound = PARS_TRUE;
            }
        }
        if( fFound==PARS_FALSE )
        {
            attrPrev = attrActual;
            attrActual=attrActual->pNextAttribute;
        }
    } while ((attrActual!=PARS_NULL)&&(!fFound));

    if(fFound)
    {
        if (PARS_NULL!=attrPrev)
        {
            attrPrev->pNextAttribute = attrActual->pNextAttribute;
        }
        else
        {
            tmpNode->p_atrRoot = attrActual->pNextAttribute;
        }
        /* delete attribute */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_DeleteAttr( attrActual), PARS_DEBUG_LEVEL_INFO, "Delete Attribute failed ");
    }
    else
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_NOT_FOUND,"Attribute not found");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteNode() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Deletes the specified node and all its children                                                |
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
static PARS_RESULT DLLCALL PARS_XML_DeleteNode ( /*  in */ PARS_XML_Node    a_ndeNode)
{
    PARS_XML_PT_internalNode tmpNode = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteNode");
    CHECK_NULL_PARAMETER(a_ndeNode);

    PARS_XML_RemoveNode(a_ndeNode); /* VF 120804: Unlink Node */

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if (tmpNode->p_ndeFirstChild!=PARS_NULL)               /* Delete All children */
    {
        PARS_XML_DeleteAllNodes( tmpNode->p_ndeFirstChild );
        tmpNode->p_ndeFirstChild=PARS_NULL;
    }
    if (tmpNode->p_atrRoot!=PARS_NULL)                 /* Delete Attributes of this node */
    {
        PARS_XML_DeleteAllAttr( tmpNode->p_atrRoot );
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
#if PARS_USE_MEM_POOL
    if( PARS_XML_g_pMem != PARS_NULL )
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

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_ExpandContent() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Expand the content of the specified node and create all its children till the specified depth  |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]       node whose content is to expand                          |
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
static PARS_RESULT DLLCALL PARS_XML_ExpandContent ( /*  in */ PARS_XML_Node   a_ndeNode,
                                                    /*  in */ PARS_DWORD      a_dwActualDepth,
                                                    /*  in */ PARS_DWORD      a_dwExpandDepth)
{
    XML_TEXT_POS                posActual               = PARS_NULL;
    PARS_XML_PT_internalNode    ndeChildNode            = PARS_NULL;
    PARS_BOOL                   bEndScan                = PARS_FALSE;
    PARS_INT                    nOverrideEndTag         = 0;
    PARS_XML_PT_internalNode    tmpNode                 = PARS_NULL;
    XML_TEXT_POS                posXMLTag               = 0;
    PARS_DWORD                  dwTagLen                = 0;
    PARS_DWORD                  dwRealContentOffset     = 0;
    XML_TEXT_POS                posXMLContentStart      = PARS_NULL;
    PARS_BOOL                   fChildNodeFound         = PARS_FALSE;
    PARS_BOOL                   fNoEndingTag            = PARS_FALSE;
    PARS_BOOL                   bFinish                 = PARS_FALSE;
    PARS_LPSTR                  strCursor               = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_ExpandContent");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    /* Scan Content of TAG     */
    if (tmpNode->szPrefContent)
    {
        tmpNode->posXMLContentStart = tmpNode->szPrefContent;
        tmpNode->dwContentLen       = tmpNode->dwPrefContentLen;
        /* no ending tag could be found because content was set explicit */
        fNoEndingTag = PARS_TRUE;
    }
    posActual = tmpNode->posXMLContentStart;
    do
    {
        posXMLContentStart = posActual;
        if ((!posActual)
        &&(fNoEndingTag))
        {
            bEndScan = PARS_TRUE;
        }
        else
        {
            BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "Syntax error during content scan.\n");
            posActual = PARS_XML_ScanChar(posActual, START_OF_TAG, '\0', PARS_FALSE);
            if ((!posActual)
            &&(fNoEndingTag))
            {
                /* create content node if additional content exists after a found child node */
                if (fChildNodeFound)
                {
                    dwRealContentOffset = PARS_strspn(posXMLContentStart, " \t\n\r\v\f");
                    if(posXMLContentStart[dwRealContentOffset] != '\0')
                    {
                        /* additional content found */
                        tmpNode->dwFlags |= PARS_XML_NODE_EXPANDED;
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateNode( (PARS_XML_Node) tmpNode,
                                                                  PARS_XML_INSERT_LAST_CHILD,
                                                                  PARS_XML_NODE_TYPE_TEXT,
                                                                  (PARS_XML_Node*)&ndeChildNode),
                                             PARS_DEBUG_LEVEL_INFO,
                                             " create node failed ");
                        {
                            ndeChildNode->posXMLContentStart    = posXMLContentStart;
                            ndeChildNode->dwContentLen          = PARS_strlen(posXMLContentStart);
                        }
                    }
                }
            }
            else
            {
                BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "Expecting next Tag.\n");
                BAIL_OUT_ON_FAIL_EX( PARS_XML_FindNextValue( &posActual,
                                                              END_OF_TAG,
                                                             &posXMLTag,
                                                             &dwTagLen,
                                                              PARS_FALSE,
                                                              PARS_TRUE),
                                     PARS_DEBUG_LEVEL_HINT,
                                     "Error searching end of tag.\n");
                BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "Error searching end of tag.\n");
                if (posActual[0]!=END_OF_TAG)
                {
                    nOverrideEndTag=0;
                }
                else
                {
                    nOverrideEndTag=1;
                    posActual++; /* Read over end tag     */
                }
                if (((PARS_LPSTR)posXMLTag)[0]=='/') /* Ending Tag     */
                {
                    /* create content node if additional content exists after a found child node */
                    if (fChildNodeFound)
                    {
                        dwRealContentOffset = PARS_strspn(posXMLContentStart, " \t\n\r\v\f");
                        if(posXMLContentStart[dwRealContentOffset] != '<')
                        {
                            /* additional content found */
                            tmpNode->dwFlags |= PARS_XML_NODE_EXPANDED;
                            BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateNode( (PARS_XML_Node) tmpNode,
                                                                      PARS_XML_INSERT_LAST_CHILD,
                                                                      PARS_XML_NODE_TYPE_TEXT,
                                                                      (PARS_XML_Node*)&ndeChildNode),
                                                 PARS_DEBUG_LEVEL_INFO,
                                                 " create node failed ");
                            ndeChildNode->posXMLContentStart    = posXMLContentStart;
                            ndeChildNode->dwContentLen          = (posXMLTag-2) - posXMLContentStart;
                        }
                    }
#if PARS_XML_VERBOSE_PARSING
                    /* PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT, "XML-Parse: Find Ending Tag: <%*.*s>.\n", dwTagLen, dwTagLen, posXMLTag); *//* AP 00193992 */
                    PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,"XML-Parse: Find Ending Tag: %d %d %d", dwTagLen, dwTagLen, posXMLTag);
#endif /* PARS_XML_VERBOSE_PARSING */
                    if( (dwTagLen >= tmpNode->dwTagNameLen) &&
                        (PARS_strncmp(posXMLTag+1, tmpNode->posXMLTagName, tmpNode->dwTagNameLen)==0) &&
                        (PARS_strchr("> \t\n\r", posXMLTag[tmpNode->dwTagNameLen+1])!=PARS_NULL)
                      )
                    {
                        /* our ending Tag     */
#if PARS_XML_VERBOSE_PARSING
                        /*PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "Its our Tag. Scanning Ends!\n");*/
                        PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "Its our Tag. Scanning Ends!");
#endif /* PARS_XML_VERBOSE_PARSING */
                        /* Its really our Ending Tag!     */
                        tmpNode->dwContentLen=(posActual-2-tmpNode->dwTagNameLen) - tmpNode->posXMLContentStart -1;
                        posActual += PARS_strspn(posActual, " \t\n\r");
                        if(*posActual=='>') posActual++;
                        tmpNode->posXMLNextTag=posActual;
                        bEndScan=PARS_TRUE;
                        /* check indent for closing tag */
                        strCursor = tmpNode->posXMLContentStart+tmpNode->dwContentLen-1;
                        tmpNode->dwOriginIndentClosingTag = 0;
                        bFinish = PARS_FALSE;
                        do
                        {
                            switch(*strCursor)
                            {
                            case ' ': tmpNode->dwOriginIndentClosingTag++;
                                break;
                            case '\t': tmpNode->dwOriginIndentClosingTag+=PARS_XML_TABSIZE;
                                break;
                            case '\r':  tmpNode->dwFlags |= PARS_XML_NODE_LF_CLOSING_TAG;
                                bFinish = PARS_TRUE;
                                break;
                            case '\n':  tmpNode->dwFlags |= PARS_XML_NODE_LF_CLOSING_TAG;
                                bFinish = PARS_TRUE;
                                break;
                            default:
                                bFinish = PARS_TRUE;
                                break;
                            }
                            strCursor--;
                        } while (bFinish == PARS_FALSE);
                        /* check indent for closing tag */
                    }
                    else
                    {
                        if ((a_dwActualDepth<a_dwExpandDepth) ||
                            (a_dwExpandDepth==0))
                        {
#if PARS_XML_VERBOSE_PARSING
                            /*PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "Ending Tag is unexpected!\n");*/
                            PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "Ending Tag is unexpected!");
#endif /* PARS_XML_VERBOSE_PARSING */
                            BAIL_OUT_3( PARS_DEBUG_LEVEL_INFO, MWE_FAIL, "Ending Tag is unexpected: <%*.*s>.\n", dwTagLen, dwTagLen, posXMLTag);
                        }
#if PARS_XML_VERBOSE_PARSING
                            /*PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "\n");*/
                            PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "  ");
#endif /* PARS_XML_VERBOSE_PARSING */
                    }
                }
                else
                {
                    /* create content node if additional content exists in front of the found child node */
                    dwRealContentOffset = PARS_strspn(posXMLContentStart, " \t\n\r\v\f");
                    if(posXMLContentStart[dwRealContentOffset] != '<')
                    {
                        /* additional content found */
                        tmpNode->dwFlags |= PARS_XML_NODE_EXPANDED;
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateNode( (PARS_XML_Node) tmpNode,
                                                                  PARS_XML_INSERT_LAST_CHILD,
                                                                  PARS_XML_NODE_TYPE_TEXT,
                                                                  (PARS_XML_Node*)&ndeChildNode),
                                             PARS_DEBUG_LEVEL_INFO,
                                             " create node failed ");
                        {
                            ndeChildNode->posXMLContentStart    = posXMLContentStart;
                            ndeChildNode->dwContentLen          = (posXMLTag-1) - posXMLContentStart;
                        }
                    }
                    fChildNodeFound = PARS_TRUE;
#if PARS_XML_VERBOSE_PARSING
                    /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT, "XML-Parse: Find Child Tag: <%*.*s>.", dwTagLen, dwTagLen, posXMLTag); *//* AP 00193992 */
                    PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT, "XML-Parse: Find Child Tag: %d %d %d.", dwTagLen, dwTagLen, posXMLTag);
#endif /* PARS_XML_VERBOSE_PARSING */
                    /* A Child Tag     */
                    if ((a_dwActualDepth<a_dwExpandDepth) ||
                        (a_dwExpandDepth==0))
                    {
                        posActual-=nOverrideEndTag+dwTagLen;
                        if (posXMLTag[dwTagLen-1]=='/')
                        {
                            dwTagLen--;
                        }
                        /* Link Node */
                        tmpNode->dwFlags |= PARS_XML_NODE_EXPANDED;
                        BAIL_OUT_ON_FAIL_EX( PARS_XML_CreateNode( (PARS_XML_Node) tmpNode,
                                                                  PARS_XML_INSERT_LAST_CHILD,
                                                                  PARS_XML_NODE_TYPE_ELEMENT,
                                                                  (PARS_XML_Node*)&ndeChildNode),
                                             PARS_DEBUG_LEVEL_INFO,
                                             " create node failed ");
                        ndeChildNode->posXMLTagStart = posActual;
                        ndeChildNode->posXMLTagName  = posXMLTag;
                        ndeChildNode->dwTagNameLen   = dwTagLen;
                        /* Count the indents */
                        strCursor = posActual-2;
                        ndeChildNode->dwOriginIndent = 0;
                        bFinish = PARS_FALSE;
                        do
                        {
                            switch(*strCursor)
                            {
                            case ' ': ndeChildNode->dwOriginIndent++;
                                break;
                            case '\t': ndeChildNode->dwOriginIndent+=PARS_XML_TABSIZE;
                                break;
                            case '\r':  ndeChildNode->dwFlags |= PARS_XML_NODE_LF;
                                bFinish = PARS_TRUE;
                                break;
                            case '\n':  ndeChildNode->dwFlags |= PARS_XML_NODE_LF;
                                bFinish = PARS_TRUE;
                                break;
                            default:
                                bFinish = PARS_TRUE;
                                break;
                            }
                            strCursor--;
                        } while (bFinish == PARS_FALSE);

                        PARS_XML_ExpandNode( (PARS_XML_Node) ndeChildNode,
                                             a_dwActualDepth +1,
                                             a_dwExpandDepth);
                        posActual = PARS_XML_GetNextTextTag( (PARS_XML_Node)ndeChildNode );
                    }
                    else
                    {
                        posActual = PARS_XML_ScanChar(posActual,END_OF_TAG, 0, PARS_FALSE);
                    }
                }
            }
        }
    } while (bEndScan==PARS_FALSE);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_ExpandNode() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Parses the node and generates                                                                  |
|       - the attribute list of the Node                                                               |
|       - the pointers to Content of the node etc.                                                     |
|       - all Sub Nodes to the given level for this node.                                              |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose is to expand                                |
|       a_dwActualDepth         [  in ]         the actual depth of this node                          |
|       a_dwExanndDepth         [  in ]         the levels (absolute) to which is to expand            |
|                                               ( if the level of this node is 3, and                  |
|                                               a_dwDepth is 4 only one level more will                |
|                                               expanded )                                             |
|                                               0 for all Levels                                       |
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
static PARS_RESULT DLLCALL PARS_XML_ExpandNode ( /*  in */ PARS_XML_Node  a_ndeNode,
                                                 /*  in */ PARS_DWORD     a_dwActualDepth,
                                                 /*  in */ PARS_DWORD     a_dwExpandDepth)
{
    XML_TEXT_POS                        posActual       = PARS_NULL;
    XML_TEXT_POS                        posNext         = PARS_NULL;
    PARS_XML_PT_internalAttributes      attrActual      = PARS_NULL;
    PARS_XML_PT_internalAttributes      attrTemp        = PARS_NULL;
    PARS_BOOL                           b_isEmptyTag    = PARS_FALSE;
    PARS_XML_PT_internalNode            tmpNode         = PARS_NULL;
    XML_TEXT_POS                        posAttrName     = PARS_NULL;
    PARS_DWORD                          dwAttrNameLen   = 0;
    XML_TEXT_POS                        posAttrValue    = PARS_NULL;
    PARS_DWORD                          dwAttrValueLen  = 0;
    PARS_BOOL                           bFound          = PARS_FALSE;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_ExpandNode");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if ((a_dwActualDepth>a_dwExpandDepth) &&
        (a_dwExpandDepth!=0))
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_FAIL,"Level > Depth");
    }
#if PARS_XML_VERBOSE_PARSING
    /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT,
                  "XML-Parse: Expanding Node: <%*.*s>.\n",
                  tmpNode->dwTagNameLen,
                  tmpNode->dwTagNameLen,
                  tmpNode->posXMLTagName); *//* AP 00193992 */ 
    PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                  "XML-Parse: Expanding Node: %d %d %d.",
                  tmpNode->dwTagNameLen,
                  tmpNode->dwTagNameLen,
                  tmpNode->posXMLTagName);

#endif /* PARS_XML_VERBOSE_PARSING */

    /* Ignore Leading Tag     */
    posActual=tmpNode->posXMLTagStart;
    posActual+=tmpNode->dwTagNameLen;
    b_isEmptyTag=PARS_FALSE;
    /* handle <![CDATA[ block */
    if ((tmpNode->dwTagNameLen>=8)&&(PARS_strncmp(tmpNode->posXMLTagName,"![CDATA[",8)==0))
    {
#if PARS_XML_VERBOSE_PARSING
        /*PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: Specialtag: CDATA.\n"); */
        PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "XML-Parse: Specialtag: CDATA.");
#endif /* PARS_XML_VERBOSE_PARSING */
        tmpNode->dwNodeType = PARS_XML_NODE_TYPE_CDATA;
        /* Seek for End of CDATA Block */
        tmpNode->dwTagNameLen = 8;
        posActual = (tmpNode->posXMLTagName+8);
        tmpNode->posXMLContentStart=posActual;
        bFound=PARS_FALSE;
        do
        {
            posActual = PARS_XML_ScanChar(posActual, ']', 0, PARS_FALSE);
            BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "']' expected.\n");
            if (PARS_strncmp(posActual, "]>", 2)==0)
            {
                tmpNode->dwContentLen=(posActual -1) - (tmpNode->posXMLContentStart );
                tmpNode->posXMLNextTag=posActual+2;
                /* EndTag Found */
                bFound=PARS_TRUE;
            }
        } while (bFound==PARS_FALSE);
#if PARS_XML_VERBOSE_PARSING
        /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT,
                      "XML-Parse: Expanding Node: <%*.*s> completed.\n",
                      tmpNode->dwTagNameLen,
                      tmpNode->dwTagNameLen,
                      tmpNode->posXMLTagName); *//* AP 00193992 */
       PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                      "XML-Parse: Expanding Node: %d %d %d completed.",
                      tmpNode->dwTagNameLen,
                      tmpNode->dwTagNameLen,
                      tmpNode->posXMLTagName);
#endif /* PARS_XML_VERBOSE_PARSING */
        RETURN(MWE_NO_ERROR);
    }
    /* handle comment block */
    if ((tmpNode->dwTagNameLen>=3)&&(PARS_strncmp(tmpNode->posXMLTagName,"!--",3)==0))
    {
#if PARS_XML_VERBOSE_PARSING
        /* PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: COMMENT found... " ); */
        PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "XML-Parse: COMMENT found... " );
#endif /* PARS_XML_VERBOSE_PARSING */
        tmpNode->dwNodeType = PARS_XML_NODE_TYPE_COMMENT;
        tmpNode->dwTagNameLen = 3;
        posActual = (tmpNode->posXMLTagName+3);
        tmpNode->posXMLContentStart=posActual;
        bFound = PARS_FALSE;
        while (!bFound)
        {
            posActual = PARS_XML_ScanChar(posActual, '-', 0, PARS_FALSE);
            BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_INFO, MWE_SYNTAX_ERROR, "'-' expected.\n");
            if (PARS_strncmp(posActual,"->",2)==0)
            {
#if PARS_XML_VERBOSE_PARSING
                /* PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "END OF COMMENT Found.\n"); */
                PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "END OF COMMENT Found.");
#endif /* PARS_XML_VERBOSE_PARSING */
                tmpNode->dwContentLen=(posActual -1) - (tmpNode->posXMLContentStart );
                tmpNode->posXMLNextTag=posActual+2;
                bFound = PARS_TRUE;
            }
        }
        RETURN(MWE_NO_ERROR);
    }

    /* Check for Attributes     */
    if (tmpNode->p_atrRoot==PARS_NULL)        /* 1. Scan Attributes     */
    {
        do
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_FindNextAttribute( posActual,
                                                            &posNext,
                                                            &posAttrName,
                                                            &dwAttrNameLen,
                                                            &posAttrValue,
                                                            &dwAttrValueLen),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "Find next Attribute failed ");
            if (posNext)
            {
                posActual = posNext;
                BAIL_OUT_ON_FAIL_EX( PARS_XML_NewAttribute( &attrActual ),
                                     PARS_DEBUG_LEVEL_INFO,
                                     "Creation of node failed");

                attrActual->posXMLName  = posAttrName;
                attrActual->dwNameLen   = dwAttrNameLen;
                attrActual->posXMLValue = posAttrValue;
                attrActual->dwValueLen  = dwAttrValueLen;

                if (tmpNode->p_atrRoot!=PARS_NULL)
                {
                    attrTemp->pNextAttribute = attrActual;
                }
                else
                {
                    tmpNode->p_atrRoot=attrActual;
                }
                attrTemp = attrActual;
#if PARS_XML_VERBOSE_PARSING
                /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT,
                              "XML-Parse: Find Attribute Name: %*.*s,",
                              attrActual->dwNameLen,
                              attrActual->dwNameLen,
                              attrActual->posXMLName); */ /* AP 00193992 */
                PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                              "XML-Parse: Find Attribute Name: %d %d %d",
                              attrActual->dwNameLen,
                              attrActual->dwNameLen,
                              attrActual->posXMLName);
                /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT,
                              "Value: %*.*s.\n",
                              attrActual->dwValueLen,
                              attrActual->dwValueLen,
                              attrActual->posXMLValue);*/ /* AP 00193992 */
                PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                              "Value: %d %d %d",
                              attrActual->dwValueLen,
                              attrActual->dwValueLen,
                              attrActual->posXMLValue);
#endif /* PARS_XML_VERBOSE_PARSING */

            }
        } while (posNext);
        posActual = PARS_XML_ScanChar(posActual, END_OF_TAG, START_OF_TAG, PARS_FALSE);
    }
    else
    {
        posActual = PARS_XML_ScanChar(posActual, END_OF_TAG, 0, PARS_FALSE);
    }
    BAIL_OUT_ON_NULL_EX( posActual, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "End of tag expected.\n");
    BAIL_OUT_ON_NULL_EX( posActual-2, PARS_DEBUG_LEVEL_HINT, MWE_SYNTAX_ERROR, "End of tag expected.\n");
    if (*(posActual-2)=='/')
    {
#if PARS_XML_VERBOSE_PARSING
        /*PARS_TRACE( PARS_DEBUG_LEVEL_HINT, "XML-Parse: Its an empty Tag.\n");*/
        PND_PARAMS_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "XML-Parse: Its an empty Tag.");
#endif /* PARS_XML_VERBOSE_PARSING */
        b_isEmptyTag=PARS_TRUE;

        tmpNode->posXMLContentStart = PARS_NULL;
        tmpNode->dwContentLen       = 0;
        tmpNode->posXMLNextTag      = posActual;

        if( (tmpNode->szPrefTagName!=PARS_NULL) &&
            (tmpNode->szPrefTagName[tmpNode->dwPrefTagNameLen-1]=='/') )
        {
            tmpNode->dwPrefTagNameLen--;
        }
        if( (tmpNode->posXMLTagName!=PARS_NULL) &&
            (tmpNode->posXMLTagName[tmpNode->dwTagNameLen-1]=='/') )
        {
            tmpNode->dwTagNameLen--;
        }
    }
    if (b_isEmptyTag==PARS_FALSE)
    {
        tmpNode->posXMLContentStart=posActual;
        BAIL_OUT_ON_FAIL_EX( PARS_XML_ExpandContent( a_ndeNode,
                                                     a_dwActualDepth+1,
                                                     a_dwExpandDepth),
                             PARS_DEBUG_LEVEL_INFO,
                             "Expand Content failed.\n");
    }
#if PARS_XML_VERBOSE_PARSING
    /*PARS_TRACE_3( PARS_DEBUG_LEVEL_HINT,
                  "XML-Parse: Expanding Node: <%*.*s> completed.\n",
                  tmpNode->dwTagNameLen,
                  tmpNode->dwTagNameLen,
                  tmpNode->posXMLTagName);*//* AP 00193992 */
    PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                  "XML-Parse: Expanding Node: %d %d %d completed.",
                  tmpNode->dwTagNameLen,
                  tmpNode->dwTagNameLen,
                  tmpNode->posXMLTagName);
#endif /* PARS_XML_VERBOSE_PARSING */
LANDING_ZONE;
    if( tmpNode!=PARS_NULL )
    {
        /*PARS_TRACE_3( PARS_DEBUG_LEVEL_WARNING,
                      "XML-Parse: Expanding Node: <%*.*s> ERROR!\n",
                      tmpNode->dwTagNameLen,
                      tmpNode->dwTagNameLen,
                      tmpNode->posXMLTagName); *//* AP 00193992 */
        PND_PARAMS_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                      "XML-Parse: Expanding Node: %d %d %d ERROR!",
                      tmpNode->dwTagNameLen,
                      tmpNode->dwTagNameLen,
                      tmpNode->posXMLTagName);
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_GetSheet() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the PARS_XML_Sheet of the specified root node                                          |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtTheXMLSheet        [  in ]         the sheet with the requested root node                 |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_XML_Sheet      !PARS_NULL      Pointer to XMLSheet                                        |
|                           PARS_NULL       a_shtXMLSheet is PARS_NULL                                 |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_XML_Sheet DLLCALL PARS_XML_GetSheet ( /*  in */ PARS_XML_Node       a_ndeNode)
{
    PARS_XML_PT_internalNode tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_ON_NULL(tmpNode);
    return (PARS_XML_Sheet) tmpNode->p_shtXMLSheet;
}

/*--- PARS_XML_GetRootNode() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the root node of the specified PARS_XML_Sheet                                          |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtTheXMLSheet        [  in ]         the sheet with the requested root node                 |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_XML_Node          !PARS_NULL      Pointer to the root node                                |
|                        PARS_NULL       a_shtXMLSheet is PARS_NULL                                    |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_XML_Node DLLCALL PARS_XML_GetRootNode ( /*  in */ PARS_XML_Sheet    a_shtTheXMLSheet)
{
    PARS_XML_PT_internalSheet   tmpSheet = (PARS_XML_PT_internalSheet) a_shtTheXMLSheet;

    RETURN_ON_NULL(tmpSheet);
    return (PARS_XML_Node) tmpSheet->p_ndeRoot;
}

/*--- PARS_XML_GetChildCount() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the number of childnodes of the specified node                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         the node whose childcount is requested                 |
|       a_dwNodeType            [  in ]         the type of nodes which are counted                    |
|       a_pdwChildCount         [ out ]         pointer to the number of children                      |
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
static PARS_RESULT DLLCALL PARS_XML_GetChildCount ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                    /*  in */ PARS_DWORD           a_dwNodeType,
                                                    /* out */ PARS_DWORD*          a_pdwChildCount )
{
    PARS_XML_PT_internalNode tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetChildCount");

    CHECK_NULL_PARAMETER(a_ndeNode);

    if(a_dwNodeType == PARS_XML_NODE_TYPE_ALLNODES)
    {
        *a_pdwChildCount = tmpNode->dwChildCount;
    }
    else
    {
        *a_pdwChildCount = 0;
        PARS_XML_GetNode(a_ndeNode, PARS_XML_NAVIGATE_DOWN, a_dwNodeType, &a_ndeNode);
        while(a_ndeNode)
        {
            (*a_pdwChildCount)++;
            PARS_XML_GetNode(a_ndeNode, PARS_XML_NAVIGATE_NEXT, a_dwNodeType, &a_ndeNode);
        }
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SetTagName() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Set TagName to a specified node                                                                |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose name is to set                              |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the tag name should be       |
|                                               copied to allocated memory )                           |
|       a_strName               [  in ]         tag name to set                                        |
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
static PARS_RESULT DLLCALL PARS_XML_SetTagName ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                 /*  in */ PARS_DWORD           a_dwMemFlags,
                                                 /*  in */ PARS_LPSTR           a_strName)
{
    PARS_XML_PT_internalNode tmpNode         = PARS_NULL;
    PARS_DWORD               dwNameLen       = 0;
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SetTagName");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strName);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    /* delete old name */
    if(tmpNode->szPrefTagName)
    {
        XML_SMemFree(tmpNode->szPrefTagName);
    }

    /* PARS_XML_MEM_FLAG_MAKE_COPY */
    dwNameLen = PARS_strlen(a_strName);
    if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
    {
        tmpNode->szPrefTagName = (PARS_LPSTR) XML_SMemMalloc( dwNameLen+1 );
        CHECK_MEMORY(tmpNode->szPrefTagName);
        PARS_strlcpy( tmpNode->szPrefTagName,
                      a_strName,
                      dwNameLen+1,
                      dwNameLen);
        tmpNode->dwPrefTagNameLen    = PARS_strlen(a_strName);
    }
    else
    {
        tmpNode->szPrefTagName      = PARS_NULL;
        tmpNode->dwPrefTagNameLen   =  0;
        tmpNode->posXMLTagName      = a_strName;
        tmpNode->dwTagNameLen       = dwNameLen;
    }
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetTagName() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Return the TagName of a specified node                                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose name is to return                           |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the tag name should be       |
|                                               copied to allocated memory or not)                     |
|       a_pstrValue             [ out ]         pointer to the tagname of the node                     |
|       a_pstrValueLen          [ out ]         pointer to length of the tagname                       |
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
PARS_RESULT DLLCALL PARS_XML_GetTagName ( /*  in */ PARS_XML_Node        a_ndeNode,
                                          /*  in */ PARS_DWORD           a_dwMemFlags,
                                          /* out */ PARS_LPSTR*          a_pstrValue,
                                          /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_XML_PT_internalNode    tmpNode         = PARS_NULL;
    PARS_LPSTR                  strTagName      = 0;
    PARS_DWORD                  dwLength        = 0;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetTagName");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    /* what name is to use */
    if((tmpNode->szPrefTagName)
        &&(PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)))
    {
        *a_pstrValue = tmpNode->szPrefTagName;
        dwLength     = tmpNode->dwPrefTagNameLen;
    }
    else
    {
        *a_pstrValue = tmpNode->posXMLTagName;
        dwLength     = tmpNode->dwTagNameLen;
    }
    if( (*a_pstrValue)[dwLength-1]=='/' )
    {
        dwLength--;
    }
    /* PARS_XML_MEM_FLAG_COPY_ON_DEMAND */
    if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
      &&(!tmpNode->szPrefTagName))
    {
        /* make copy for internal use */
        tmpNode->szPrefTagName = (PARS_LPSTR) XML_SMemMalloc(tmpNode->dwTagNameLen+1);
        CHECK_MEMORY( tmpNode->szPrefTagName );
        PARS_strlcpy(tmpNode->szPrefTagName, tmpNode->posXMLTagName, tmpNode->dwTagNameLen+1, tmpNode->dwTagNameLen);
        tmpNode->dwPrefTagNameLen = tmpNode->dwTagNameLen;
        *a_pstrValue = tmpNode->szPrefTagName;
        dwLength     = tmpNode->dwTagNameLen;
    }
    BAIL_OUT_ON_FAIL_EX( PARS_XML_StripNameSpace( a_pstrValue, &dwLength), PARS_DEBUG_LEVEL_HINT, "Error stripping namespace.\n");

#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
    if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
    {
        /* make copy and return this */
        strTagName = (PARS_LPSTR) XML_SMemMalloc(dwLength+1);
        CHECK_MEMORY(strTagName);
        PARS_strlcpy(strTagName, *a_pstrValue, dwLength+1, dwLength);
        *a_pstrValue = strTagName;
    }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */

    if (PARS_NULL!=a_pdwLength)
    {
        *a_pdwLength = dwLength;
    }
    else if (PARS_NULL!=tmpNode->szPrefTagName)
    {
         *a_pstrValue    = PARS_NULL;
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER," Invalid Flag and Parameter combination, Value Length must not be PARS_NULL");
    }

LANDING_ZONE;
    if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
      &&( tmpNode && tmpNode->szPrefTagName))
    {
      XML_SMemFree(tmpNode->szPrefTagName);
      tmpNode->szPrefTagName = PARS_NULL;
    }

#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
    if ((PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
      && strTagName)
    {
      XML_SMemFree(strTagName);
      strTagName = PARS_NULL;
    }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
    if (PARS_NULL!=a_pdwLength)
    {
        *a_pdwLength = 0;
    }
    if (PARS_NULL!=a_pstrValue)
    {
        *a_pstrValue = PARS_NULL;
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_GetTagnameNamespace() -----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the TagNameNamespace of a specified node                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose tagnamenamespace is to return               |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the namespace should be      |
|                                               copied to allocated memory or not)                     |
|       a_pstrValue             [ out ]         pointer to the tagnamenamespace of the node            |
|       a_pstrValueLen          [ out ]         pointer to length of the tagnamenamespace              |
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
static PARS_RESULT DLLCALL PARS_XML_GetTagnameNamespace ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                          /*  in */ PARS_DWORD           a_dwMemFlags,
                                                          /* out */ PARS_LPSTR*          a_pstrValue,
                                                          /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_XML_PT_internalNode    tmpNode             = PARS_NULL;
    PARS_LPSTR                  strActual           = PARS_NULL;
    PARS_LPSTR                  strTagName          = PARS_NULL;
    PARS_DWORD                  dwTagNameLen        = 0;
    PARS_DWORD                  dwLength            = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "GetTagnameNamespace");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    /* test invalid flags */
    if(PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
    {
        *a_pstrValue    = PARS_NULL;
        dwLength   = 0;
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER,"Invalid Flag PARS_XML_MEM_FLAG_COPY_ON_DEMAND");
    }

    /* what name is to use */
    if((tmpNode->szPrefTagName)
        &&(PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)))
    {
        strTagName = tmpNode->szPrefTagName;
        dwTagNameLen = tmpNode->dwPrefTagNameLen;
    }
    else
    {
        strTagName = tmpNode->posXMLTagName;
        dwTagNameLen = tmpNode->dwTagNameLen;
    }

    strActual=PARS_strnchr(strTagName, ':',dwTagNameLen);
    if (strActual==PARS_NULL)
    {
        /* No Name space */
        *a_pstrValue    = PARS_NULL;
        dwLength   = 0;
    }
    else
    {
        dwLength=strActual-strTagName;
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
        if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
        {
            *a_pstrValue = (PARS_LPSTR)XML_SMemMalloc(dwLength+1);
            CHECK_MEMORY(*a_pstrValue);
            PARS_memcpy(*a_pstrValue, strTagName, dwLength);
            (*a_pstrValue)[dwLength] = '\0';
        }
        else
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
        {
            *a_pstrValue = strTagName;
        }
    }
    if (PARS_NULL!=a_pdwLength)
    {
        *a_pdwLength = dwLength;
    }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
    else if (PARS_XML_MEM_FLAG_GIVE_A_COPY != (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
    {
        *a_pstrValue    = PARS_NULL;
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER," Invalid Flag and Parameter combination, Value Length must not be PARS_NULL");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SetContent() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Sets the content of a specified node                                                           |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose tagnamenamespace is to return               |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the content should be        |
|                                               copied to allocated memory or not)                     |
|       a_strValue              [  in ]         content string to set to node                          |
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
static PARS_RESULT DLLCALL PARS_XML_SetContent( /*  in */ PARS_XML_Node        a_ndeNode,
                                                /*  in */ PARS_DWORD           a_dwMemFlags,
                                                /*  in */ PARS_LPSTR           a_strValue,
                                                /*  in */ PARS_DWORD           a_dwValueLen)
{
    PARS_LPSTR                  strContent      = PARS_NULL;
    PARS_XML_PT_internalNode    tmpNode         = PARS_NULL;
    PARS_DWORD                  dwIndex         = 0;
    PARS_DWORD                  dwWriteIndex    = 0;
    PARS_DWORD                  dwExpand        = 0;
    PARS_DWORD                  dwRawChars      = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SetContent");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    /* Handle basic escaping of &,< here */
    for(dwIndex=0;dwIndex<a_dwValueLen;dwIndex++)
    {
        if(a_strValue[dwIndex]=='\0') break;
        switch(a_strValue[dwIndex])
        {
        case '&': dwExpand+=5; break;
        case '<' :dwExpand+=4; break;
        default: dwRawChars++; break;
        }
    }
    if( dwExpand > 0)
    {
        /* We have to Expand -> make a copy */
        a_dwMemFlags |= PARS_XML_MEM_FLAG_MAKE_COPY;
    }
    /* PARS_XML_MEM_FLAG_MAKE_COPY */
    if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
    {
        if(a_dwValueLen == PARS_ZERO_TERMINATED_STRING)
        {
            a_dwValueLen = PARS_strlen(a_strValue);
        }
        strContent = (PARS_LPSTR) XML_SMemMalloc( dwRawChars+dwExpand+1 );
        CHECK_MEMORY(strContent);
        dwWriteIndex = 0;
        for(dwIndex=0;dwIndex<a_dwValueLen;dwIndex++)
        {
            switch(a_strValue[dwIndex])
            {
            case '&' : PARS_memcpy(strContent+dwWriteIndex, "&amp;", 5); dwWriteIndex+=5; break;
            case '<' : PARS_memcpy(strContent+dwWriteIndex, "&lt;", 4); dwWriteIndex+=4; break;
            default: strContent[dwWriteIndex++]=a_strValue[dwIndex]; break;
            }
        }
        strContent[dwWriteIndex++]='\0';
    }
    else
    {
        if(a_dwValueLen != PARS_ZERO_TERMINATED_STRING)
        {
            BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_UNEXPECTED, "Called without PARS_XML_MEM_FLAG_MAKE_COPY potentially no zero terminated string.\n");
        }
        strContent = a_strValue;
    }

    /* delete old content */
    if(tmpNode->szPrefContent)
    {
        XML_SMemFree(tmpNode->szPrefContent);
    }
    tmpNode->dwFlags &=~PARS_XML_NODE_EXPANDED;
    tmpNode->szPrefContent        = strContent;
    tmpNode->dwPrefContentLen     = PARS_strlen(strContent);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetContent() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the content of a specified node  (if there was a write access to any child of the node |
|       the content must be recalculated                                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose content is requested                        |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the content should be        |
|                                               copied to allocated memory or not)                     |
|       a_pstrValue             [ out ]         pointer to content of the node                         |
|       a_pdwValue              [ out ]         pointer to length of content of the node               |
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
PARS_RESULT DLLCALL PARS_XML_GetContent ( /*  in */ PARS_XML_Node        a_ndeNode,
                                          /*  in */ PARS_DWORD           a_dwMemFlags,
                                          /* out */ PARS_LPSTR*          a_pstrValue,
                                          /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_XML_PT_internalNode    ndeChildNode    = PARS_NULL;
    PARS_XML_PT_internalNode    tmpNode         = PARS_NULL;
    PARS_LPSTR                  strTempBuf      = PARS_NULL;
    PARS_DWORD                  dwLength        = 0;
    PARS_XMLAsyncStruct         stXMLAsyncStruct;
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetContent");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    stXMLAsyncStruct.cbXMLAsync = PARS_NULL;
    stXMLAsyncStruct.pCookie    = PARS_NULL;
    stXMLAsyncStruct.dwC14NForm = XML_C14N_NONE;
    stXMLAsyncStruct.dwIndent   = PARS_XML_DEFAULT_INDENT;

    if( PARS_XML_MEM_FLAG_FORCE_RENEW == (a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_RENEW) )
    {
        strTempBuf = ( PARS_LPSTR) XML_SMemMalloc( PARS_XML_MAX_TEMP_BUFFER_SIZE );
        CHECK_MEMORY(strTempBuf);

        BAIL_OUT_ON_FAIL_EX( PARS_XML_GetNode( a_ndeNode,
                                               PARS_XML_NAVIGATE_DOWN,
                                               PARS_XML_NODE_TYPE_ALLNODES,
                                               (PARS_XML_Node*) &ndeChildNode),
                             PARS_DEBUG_LEVEL_INFO,
                             " GetNode failed.\n");
        BAIL_OUT_ON_FAIL_EX( PARS_XML_PackXMLNodes( ndeChildNode,
                                                    0,
                                                    strTempBuf,
                                                   &dwLength,
                                                    PARS_XML_MAX_TEMP_BUFFER_SIZE,
                                                   &stXMLAsyncStruct,
                                                   PARS_XML_USE_NEIGHBOURS),
                             PARS_DEBUG_LEVEL_INFO,
                             " Pack new content failed.\n");
        /* Doing a relocate to a smaller buffer */
        *a_pstrValue = (PARS_LPSTR) XML_SMemMalloc(dwLength+1);
        CHECK_MEMORY(*a_pstrValue);
        PARS_memcpy(*a_pstrValue, strTempBuf, dwLength);
        (*a_pstrValue)[dwLength] = '\0';
        XML_SMemFree(strTempBuf);
        strTempBuf = PARS_NULL;

        if(a_pdwLength)
        {
            *a_pdwLength = dwLength;
        }
    }
    else
    {
        tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

        /* what Content is to use */
        if((tmpNode->szPrefContent)
            &&(PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)))
        {
            *a_pstrValue = tmpNode->szPrefContent;
            dwLength     = tmpNode->dwPrefContentLen;
        }
        else
        {
            *a_pstrValue = tmpNode->posXMLContentStart;
            dwLength     = tmpNode->dwContentLen;
        }

        /* PARS_XML_MEM_FLAG_COPY_ON_DEMAND */
        if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
          &&(!tmpNode->szPrefContent))
        {
            /* make copy for internal use */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( tmpNode->posXMLContentStart,
                                                          tmpNode->dwContentLen,
                                                         &(tmpNode->szPrefContent),
                                                         &(tmpNode->dwPrefContentLen)),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "Mem Error.\n");
            *a_pstrValue = tmpNode->szPrefContent;
            dwLength = tmpNode->dwPrefContentLen;
        }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
        if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
        {
            /* make copy to return this */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrValue,
                                                          dwLength,
                                                          a_pstrValue,
                                                         &dwLength),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");
        }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */

        if (PARS_NULL!=a_pdwLength)
        {
            *a_pdwLength = dwLength;
        }
        else if (PARS_NULL!=tmpNode->szPrefContent)
        {
            *a_pstrValue    = PARS_NULL;
            BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER," Invalid Flag and Parameter combination, Value Length must not be PARS_NULL");
        }
    }
LANDING_ZONE;
    if( strTempBuf != PARS_NULL )
    {
        XML_SMemFree(strTempBuf);
        strTempBuf = PARS_NULL;
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_IsFirstChild() ------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns PARS_TRUE if the the specified node is the first child of its parent                   |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node whose content is requested                        |
|       a_dwNodeType            [  in ]         node types which should be considered                  |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_TRUE                    if the node is the first child                                    |
|       PARS_FALSE                   otherwise                                                         |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_BOOL DLLCALL PARS_XML_IsFirstChild ( /*  in */ PARS_XML_Node a_ndeNode,
                                                 /*  in */ PARS_DWORD    a_dwNodeType)
{
    PARS_XML_PT_internalNode    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if (tmpNode==PARS_NULL)
    {
        return PARS_FALSE;
    }
    if (tmpNode->p_ndeParent==PARS_NULL)
    {
        return PARS_TRUE;    /* Root Element is the first Child of the world!     */
    }
    PARS_XML_GetNode( (PARS_XML_Node) tmpNode->p_ndeParent,
                      PARS_XML_NAVIGATE_DOWN,
                      a_dwNodeType,
                      (PARS_XML_Node*) &tmpNode );
    if (tmpNode == (PARS_XML_PT_internalNode) a_ndeNode)
    {
        return PARS_TRUE; /* children from parent points to us, so we are the first child!     */
    }
    return PARS_FALSE;
}

/*--- PARS_XML_SetDTD() ------------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Sets the Document Type Definition for the specified XML-Sheet                                  |
|       the content must be recalculated                                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtSheet              [  in ]         sheet where the DTD should be set                      |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the DTD should be            |
|                                               copied to allocated memory or not)                     |
|       a_strValue              [  in ]         the DTD string to set                                  |
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
static PARS_RESULT DLLCALL PARS_XML_SetDTD ( /*  in */ PARS_XML_Sheet        a_shtSheet,
                                             /*  in */ PARS_DWORD            a_dwMemFlags,
                                             /*  in */ PARS_LPSTR            a_strValue)
{
    PARS_LPSTR                  strDTD      = PARS_NULL;
    PARS_XML_PT_internalSheet   tmpSheet    = PARS_NULL;
    PARS_DWORD                  dwDTDLen    = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SetDTD");
    CHECK_NULL_PARAMETER(a_shtSheet);
    CHECK_NULL_PARAMETER(a_strValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpSheet = (PARS_XML_PT_internalSheet) a_shtSheet;

    dwDTDLen = PARS_strlen(a_strValue)+1;
    /* PARS_XML_MEM_FLAG_MAKE_COPY */
    if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
    {
        strDTD = (PARS_LPSTR) XML_SMemMalloc( dwDTDLen+1 );
        CHECK_MEMORY(strDTD);
        PARS_strlcpy( strDTD,
                      a_strValue,
                      dwDTDLen+1,
                      dwDTDLen);
    }
    else
    {
        strDTD = a_strValue;
    }

    /* delete old content */
    if( tmpSheet->szPrefDTD_Data )
    {
        XML_SMemFree( tmpSheet->szPrefDTD_Data );
    }

    tmpSheet->szPrefDTD_Data        = strDTD;
    tmpSheet->dwPrefDTD_DataLen     = dwDTDLen;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetDTD() ------------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the Document Type Definition of the specified XML-SHeet                                |
|       the content must be recalculated                                                               |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtSheet              [  in ]         sheet where the DTD should be set                      |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the DTD should be            |
|                                               copied to allocated memory or not)                     |
|       a_pstrValue             [  in ]         pointer to the DTD                                     |
|       a_pdwValueLen           [  in ]         pointer to the length of the DTD                       |
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
static PARS_RESULT DLLCALL PARS_XML_GetDTD ( /*  in */ PARS_XML_Sheet        a_shtSheet,
                                             /*  in */ PARS_DWORD            a_dwMemFlags,
                                             /* out */ PARS_LPSTR*           a_pstrValue,
                                             /* out */ PARS_LPDWORD          a_pdwLength)
{

    PARS_XML_PT_internalSheet   tmpSheet    = PARS_NULL;
    PARS_DWORD                  dwLength    = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetDTD");
    CHECK_NULL_PARAMETER(a_shtSheet);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpSheet = (PARS_XML_PT_internalSheet) a_shtSheet;

    /* what DTD is to use */
    if((tmpSheet->szPrefDTD_Data)
        &&(PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)))
    {
        *a_pstrValue = tmpSheet->szPrefDTD_Data;
        dwLength     = tmpSheet->dwPrefDTD_DataLen;
    }
    else
    {
        *a_pstrValue = tmpSheet->posXMLDTD_Data;
        dwLength     = tmpSheet->dwDTD_DataLen;
    }

    /* PARS_XML_MEM_FLAG_COPY_ON_DEMAND */
    if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
      &&(!tmpSheet->szPrefDTD_Data))
    {
        /* make copy for internal use */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( tmpSheet->posXMLDTD_Data,
                                                      tmpSheet->dwDTD_DataLen,
                                                     &(tmpSheet->szPrefDTD_Data),
                                                     &(tmpSheet->dwPrefDTD_DataLen)),
                             PARS_DEBUG_LEVEL_INFO,
                             "Mem Error");
        *a_pstrValue = tmpSheet->szPrefDTD_Data;
        dwLength = tmpSheet->dwPrefDTD_DataLen;
    }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
    if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
    {
        /* make copy to return this */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrValue,
                                                      dwLength,
                                                      a_pstrValue,
                                                     &dwLength),
                             PARS_DEBUG_LEVEL_INFO,
                             "MemError");
    }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */

    if (PARS_NULL!=a_pdwLength)
    {
        *a_pdwLength = dwLength;
    }
    else if (PARS_NULL!=tmpSheet->szPrefDTD_Data)
    {
        *a_pstrValue    = PARS_NULL;
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER," Invalid Flag and Parameter combination, Value Length must not be PARS_NULL");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetNodeTextAsync() --------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the specified XML-Node to the given buffer and calls the callback every time the         |
|       buffer is full and at the end                                                                  |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         attribute to pack to buffer                            |
|       a_strXMLText            [  bi ]         in: buffer to add packed attribute                     |
|                                               out: buffer with packed attribute                      |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_cbfGetXMLText         [  in ]         callback function to deliver packed XML-Sheet          |
|       a_pCookie              [  in ]         cookie of the caller                                    |
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
static PARS_RESULT DLLCALL PARS_XML_GetNodeTextAsync( /*  in */ PARS_XML_Node               a_ndeNode,
                                                      /*  bi */ PARS_LPSTR                  a_strXMLText,
                                                      /*  bi */ PARS_DWORD*                 a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD                  a_dwC14NForm,
                                                      /*  in */ PARS_DWORD                  a_dwIndent,
                                                      /*  in */ PARS_XML_GET_XML_TEXT_CB    a_cbfGetXMLText,
                                                      /*  in */ PARS_LPVOID                 a_pCookie)
{
    PARS_DWORD                  dwXMLTextLen        = 0;
    PARS_DWORD                  dwXMLTextBufferSize = 0;
    PARS_XMLAsyncStruct         stXMLAsyncStruct;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetNodeTextAsync");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strXMLText);
    CHECK_NULL_PARAMETER(a_pdwXMLTextLen);
    CHECK_FUNCTION_POINTER(a_cbfGetXMLText);
    CHECK_SHUTDOWN_MODE(XML);

    stXMLAsyncStruct.cbXMLAsync = a_cbfGetXMLText;
    stXMLAsyncStruct.pCookie    = a_pCookie;
    stXMLAsyncStruct.dwC14NForm = a_dwC14NForm;
    stXMLAsyncStruct.dwIndent   = a_dwIndent;
    stXMLAsyncStruct.dwBehavior = PARS_XML_INHERIT_XMLNS;

    if( a_dwC14NForm == XML_C14N_NONE )
    {
        stXMLAsyncStruct.strNewLine = "\r\n";
    }
    else
    {
        stXMLAsyncStruct.strNewLine = "\n";
    }
    dwXMLTextBufferSize = *a_pdwXMLTextLen;

    SET_ACTUAL_ERROR(PARS_XML_PackXMLNodes( (PARS_XML_PT_internalNode) a_ndeNode,
                                            0,
                                            a_strXMLText,
                                            &dwXMLTextLen,
                                            dwXMLTextBufferSize,
                                            &stXMLAsyncStruct,
                                             PARS_XML_IGNORE_NEIGHBORS) );

    if ((*a_pdwXMLTextLen)
        &&(PARS_NULL!=a_cbfGetXMLText))
    {
        SET_ACTUAL_ERROR(a_cbfGetXMLText(a_strXMLText, dwXMLTextLen, PARS_TRUE, a_pCookie));
        *a_pdwXMLTextLen =  0;
    }

    LANDING_ZONE;
    END_LANDING_ZONE;
}

/*--- PARS_XML_GetXMLTextAsync() --------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the specified XML-Sheet to the given buffer and calls the callback everytime the         |
|       buffer is full and at the end                                                                  |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         attribute to pack to buffer                            |
|       a_strXMLText            [  bi ]         in: buffer to add packed attribute                     |
|                                               out: buffer with packed attribute                      |
|       a_pdwXMLTextLen         [  bi ]         length of XMLText                                      |
|       a_cbfGetXMLText         [  in ]         callback function to deliver packed XML-Sheet          |
|       a_pCookie              [  in ]         cookie of the caller                                    |
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
static PARS_RESULT DLLCALL PARS_XML_GetXMLTextAsync ( /*  in */ PARS_XML_Sheet              a_shtXMLSheet,
                                                      /*  bi */ PARS_LPSTR                  a_strXMLText,
                                                      /*  bi */ PARS_DWORD*                 a_pdwXMLTextLen,
                                                      /*  in */ PARS_DWORD                  a_dwC14NForm,
                                                      /*  in */ PARS_DWORD                  a_dwIndent,
                                                      /*  in */ PARS_XML_GET_XML_TEXT_CB    a_cbfGetXMLText,
                                                      /*  in */ PARS_LPVOID                 a_pCookie)
{
    PARS_XML_PT_internalNode    ndeNode             = PARS_NULL;
    PARS_XML_PT_internalSheet   shtSheet            = PARS_NULL;
    PARS_DWORD                  dwXMLTextLen        = 0;
    PARS_DWORD                  dwXMLTextBufferSize = 0;
    PARS_XMLAsyncStruct         stXMLAsyncStruct;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetXMLTextAsync");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strXMLText);
    CHECK_NULL_PARAMETER(a_pdwXMLTextLen);
    CHECK_FUNCTION_POINTER(a_cbfGetXMLText);
    CHECK_SHUTDOWN_MODE(XML);

    stXMLAsyncStruct.cbXMLAsync = a_cbfGetXMLText;
    stXMLAsyncStruct.pCookie    = a_pCookie;
    stXMLAsyncStruct.dwC14NForm = a_dwC14NForm;
    stXMLAsyncStruct.dwIndent   = a_dwIndent;
    stXMLAsyncStruct.dwBehavior = PARS_XML_BEHAVIOR_NONE;

    if( a_dwC14NForm == XML_C14N_NONE )
    {
        stXMLAsyncStruct.strNewLine = "\r\n";
    }
    else
    {
        stXMLAsyncStruct.strNewLine = "\n";
    }
    dwXMLTextBufferSize = *a_pdwXMLTextLen;
    shtSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;
    /* test if XML file is unchanged and return buffer */

    if( a_dwC14NForm == XML_C14N_NONE )
    {
        /* all canonical forms have no BOM, <?xml Header or Doc Type */
        PARS_XML_PackXMLSheetHeader(a_shtXMLSheet, a_strXMLText, &dwXMLTextLen, dwXMLTextBufferSize, &stXMLAsyncStruct);
    }
    PARS_XML_PackXMLSheetPI(a_shtXMLSheet, a_strXMLText, &dwXMLTextLen, dwXMLTextBufferSize, &stXMLAsyncStruct);

    ndeNode = (PARS_XML_PT_internalNode) shtSheet->p_ndeRoot;
    SET_ACTUAL_ERROR(PARS_XML_PackXMLNodes( ndeNode,
                                            0,
                                            a_strXMLText,
                                           &dwXMLTextLen,
                                            dwXMLTextBufferSize,
                                           &stXMLAsyncStruct,
                                            PARS_XML_USE_NEIGHBOURS) );

    if ( ( *a_pdwXMLTextLen ) &&
         ( PARS_NULL != a_cbfGetXMLText ) )
    {
        SET_ACTUAL_ERROR(a_cbfGetXMLText(a_strXMLText, dwXMLTextLen, PARS_TRUE, a_pCookie));
        *a_pdwXMLTextLen =  0;
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetXMLText() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Packs the specified XML-Sheet to the given buffer                                              |
|       -> Error if the buffer is to small                                                             |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         attribute to pack to buffer                            |
|       a_strXMLText            [  bi ]         in: buffer to add packed attribute                     |
|                                               out: buffer with packed attribute                      |
|       a_pdwXMLTextLen         [  bi ]         in: size of buffer                                     |
|                                               out: length of XMLText                                 |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|       MWE_FAIL                buffer to small                                                        |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_GetXMLText ( /*  in */ PARS_XML_Sheet     a_shtXMLSheet,
                                                 /*  in */ PARS_DWORD         a_dwC14NForm,
                                                 /*  in */ PARS_DWORD         a_dwIndent,
                                                 /*  bi */ PARS_LPSTR         a_strXMLText,
                                                 /*  bi */ PARS_DWORD*        a_pdwXMLTextLen)
{
    PARS_XML_PT_internalNode    ndeNode             = PARS_NULL;
    PARS_XML_PT_internalSheet   shtSheet            = PARS_NULL;
    PARS_DWORD                  dwXMLTextLen        = 0;
    PARS_DWORD                  dwXMLTextBufferSize = 0;
    PARS_XMLAsyncStruct         stXMLAsyncStruct;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetXMLText");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strXMLText);
    CHECK_NULL_PARAMETER(a_pdwXMLTextLen);
    CHECK_SHUTDOWN_MODE(XML);

    dwXMLTextBufferSize = *a_pdwXMLTextLen;

    shtSheet = (PARS_XML_PT_internalSheet) a_shtXMLSheet;
    /* test if xml file is unchanged and return buffer */

    stXMLAsyncStruct.cbXMLAsync = PARS_NULL;
    stXMLAsyncStruct.pCookie    = PARS_NULL;
    stXMLAsyncStruct.dwC14NForm = a_dwC14NForm;
    stXMLAsyncStruct.dwIndent   = a_dwIndent;
    stXMLAsyncStruct.dwBehavior = PARS_XML_BEHAVIOR_NONE;

    if( a_dwC14NForm == XML_C14N_NONE )
    {
        stXMLAsyncStruct.strNewLine = "\r\n";
    }
    else
    {
        stXMLAsyncStruct.strNewLine = "\n";
    }

    PARS_XML_PackXMLSheetHeader(a_shtXMLSheet, a_strXMLText, &dwXMLTextLen, dwXMLTextBufferSize, PARS_NULL);
    /* recodes the previously coded header line, attributes can be changed via SetXMLSheetAttribute */
    PARS_XML_PackXMLSheetPI(a_shtXMLSheet, a_strXMLText, &dwXMLTextLen, dwXMLTextBufferSize, PARS_NULL);
    /* copies the additional PI Lines (omitting the first line) */

    ndeNode = (PARS_XML_PT_internalNode) shtSheet->p_ndeRoot;
    PARS_XML_PackXMLNodes( ndeNode,
                           0,
                           a_strXMLText,
                          &dwXMLTextLen,
                           dwXMLTextBufferSize,
                          &stXMLAsyncStruct,
                           PARS_XML_USE_NEIGHBOURS );

    *a_pdwXMLTextLen =  dwXMLTextLen;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_AddAttribute() ------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Creates an attribute and adds the attribute to the specified node                              |
|       Sets the name and value to the attribute                                                       |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node which will get the new attribute                  |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the name and value of the    |
|                                               attribute should be copied to allocated memory or not) |
|       a_strName               [  in ]         name string of the attribute                           |
|       a_strValue              [  in ]         value string of the attribute                          |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_AddAttribute ( /*  in */ PARS_XML_Node        a_ndeNode,
                                            /*  in */ PARS_DWORD           a_dwMemFlags,
                                            /*  in */ PARS_LPSTR           a_strName,
                                            /*  in */ PARS_DWORD           a_dwNameLen,
                                            /*  in */ PARS_LPSTR           a_strValue,
                                            /*  in */ PARS_DWORD           a_dwValueLen)
{
    PARS_XML_PT_internalAttributes  attrActual      = PARS_NULL;
    PARS_XML_PT_internalAttributes  attrNew         = PARS_NULL;
    PARS_LPSTR                      strName         = PARS_NULL;
    PARS_LPSTR                      strValue        = PARS_NULL;
    PARS_XML_PT_internalNode        tmpNode         = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_AddAttribute");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strName);
    CHECK_NULL_PARAMETER(a_strValue);
    CHECK_SHUTDOWN_MODE(XML);

    if(a_dwValueLen==PARS_ZERO_TERMINATED_STRING)
    {
        a_dwValueLen = PARS_strlen(a_strValue);
    }
    if(a_dwNameLen==PARS_ZERO_TERMINATED_STRING)
    {
        a_dwNameLen = PARS_strlen(a_strName);
    }
    /* PARS_XML_MEM_FLAG_MAKE_COPY */
    if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
    {
        strName = (PARS_LPSTR) XML_SMemMalloc(a_dwNameLen+1);
        CHECK_MEMORY(strName);
        PARS_strlcpy(strName,a_strName, a_dwNameLen+1, a_dwNameLen);

        strValue = (PARS_LPSTR) XML_SMemMalloc(a_dwValueLen+1);
        CHECK_MEMORY(strValue);
        PARS_strlcpy(strValue,a_strValue, a_dwValueLen+1, a_dwValueLen);
    }
    else
    {
        strName     = a_strName;
        strValue    = a_strValue;
    }

    BAIL_OUT_ON_FAIL_EX( PARS_XML_NewAttribute( &attrNew), PARS_DEBUG_LEVEL_INFO, "create new attribute failed");

    attrNew->szPrefName = strName;
    attrNew->dwPrefNameLen = a_dwNameLen;

    attrNew->szPrefValue = strValue;
    attrNew->dwPrefValueLen = a_dwValueLen;

    attrActual=(tmpNode->p_atrRoot);

    if(attrActual)
    {   /* get last attribute */
        while(attrActual->pNextAttribute)
        {
            attrActual = attrActual->pNextAttribute;
        };
        attrActual->pNextAttribute = attrNew;
    }
    else
    {
        tmpNode->p_atrRoot = attrNew;
    }

LANDING_ZONE;
  if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
  {
    if (strName)
    {
      XML_SMemFree(strName);
      strName = PARS_NULL;
    }

    if (strValue)
    {
      XML_SMemFree(strValue);
      strValue = PARS_NULL;
    }
  }
END_LANDING_ZONE;
}

/*--- PARS_XML_QueryAttributeByName() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for an attribute of the specified node with the appropriate name                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node which will get the new attribute                  |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the value of the             |
|                                               attribute should be copied to allocated memory or not) |
|       a_strAttributeName      [  in ]         name of the attribute to search                        |
|       a_strAttributeName      [  in ]         length name of the attribute to search                 |
|       a_pstrValue             [ out ]         value of the found attribute                           |
|       a_pdwLength             [ out ]         length of the value of the found attribute             |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_QueryAttributeByName ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                    /*  in */ PARS_DWORD           a_dwMemFlags,
                                                    /*  in */ PARS_LPSTR           a_strAttributeName,
                                                    /*  in */ PARS_DWORD           a_dwAttributeNameLen,
                                                    /*  in */ PARS_BOOL            a_bIgnoreNamespaces,
                                                    /* out */ PARS_LPSTR*          a_pstrValue,
                                                    /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_XML_PT_internalAttributes  attrActual      = PARS_NULL;
    PARS_BOOL                       bNoNameSpace    = PARS_FALSE;
    PARS_LPSTR                      strName         = PARS_NULL;
    PARS_LPSTR                      strTempName     = PARS_NULL;
    PARS_DWORD                      dwLen           = 0;
    PARS_BOOL                       fFound          = PARS_FALSE;
    PARS_DWORD                      dwLength        = 0;
    PARS_XML_PT_internalNode        tmpNode         = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_QueryAttributeByName");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strAttributeName);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);
    if(a_dwAttributeNameLen==PARS_ZERO_TERMINATED_STRING)
    {
        a_dwAttributeNameLen = PARS_strlen(a_strAttributeName);
    }
    attrActual=(tmpNode->p_atrRoot);
    BAIL_OUT_ON_NULL_SILENT(attrActual, MWS_FALSE); /* em3702: if there is no attribute root no attribute will be found */

    BAIL_OUT_ON_NULL_EX( (PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)),
                         PARS_DEBUG_LEVEL_INFO,
                         MWE_INVALID_PARAMETER,
                         "Invalid Flag specified.\n");

    if(a_bIgnoreNamespaces==PARS_FALSE)
    {
        if (PARS_strnchr(a_strAttributeName, ':', a_dwAttributeNameLen)==PARS_NULL)
        {
            /* No Name space required, ignore any namespace */
            bNoNameSpace=PARS_TRUE;
        }
    }

    do
    {
        if(attrActual->szPrefName)
        {
            strName = attrActual->szPrefName;
            dwLen   = attrActual->dwPrefNameLen;
        }
        else
        {
            strName = attrActual->posXMLName;
            dwLen   = attrActual->dwNameLen;
        }
        BAIL_OUT_ON_NULL_EX( strName, PARS_DEBUG_LEVEL_INFO, MWE_UNEXPECTED, "No Tag name found.\n");
        if( bNoNameSpace != PARS_FALSE )
        {
            strTempName=PARS_strnchr(strName, ':' , dwLen);
            if (strTempName)
            {
                strTempName++;
                dwLen = dwLen - (strTempName - strName);
                strName = strTempName;
            }
        }
        if(PARS_XML_SAFE_strncmp(strName, a_strAttributeName, dwLen, a_dwAttributeNameLen)==0)
        {
            fFound = PARS_TRUE;
        }
        else
        {
            attrActual=attrActual->pNextAttribute;
        }
    } while ((attrActual!=PARS_NULL)&&(!fFound));

    if(fFound)
    {
        if(attrActual->szPrefValue)
        {
            *a_pstrValue    = attrActual->szPrefValue;
            dwLength    = attrActual->dwPrefValueLen;
        }
        else
        {
            *a_pstrValue    = attrActual->posXMLValue;
            dwLength    = attrActual->dwValueLen;
        }
        if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
        &&(!attrActual->szPrefValue))
        {
            /* make copy for internal use */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( attrActual->posXMLValue,
                                                          attrActual->dwValueLen,
                                                         &(attrActual->szPrefValue),
                                                         &(attrActual->dwPrefValueLen)),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "Mem Error");
            *a_pstrValue = attrActual->szPrefValue;
            dwLength = attrActual->dwPrefValueLen;
        }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
        if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
        {
            /* make copy to return this */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrValue,
                                                          dwLength,
                                                          a_pstrValue,
                                                         &dwLength),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");
            if (PARS_NULL!=a_pdwLength)
            {
                *a_pdwLength = dwLength;
            }
            RETURN(MWE_NO_ERROR);
        }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
        if (PARS_NULL!=a_pdwLength)
        {
            *a_pdwLength = dwLength;
        }
        else if (PARS_NULL!=attrActual->szPrefName)
        {
             *a_pstrValue    = PARS_NULL;
             BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER," Invalid Flag and Parameter combination, Value Length must not be PARS_NULL.\n");
        }
    }
    else
    {
        *a_pstrValue    = PARS_NULL;
        if (PARS_NULL!=a_pdwLength)
        {
            *a_pdwLength = dwLength;
        }
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_QueryAttributeByValue() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for an attribute of the specified node with the appropriate value                       |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node which will get the new attribute                  |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the name of the              |
|                                               attribute should be copied to allocated memory or not) |
|       a_strAttributeValue     [  in ]         value of the attribute to search                       |
|       a_strAttributeValueLen  [  in ]         length of value of the attribute to search             |
|       a_pstrName              [ out ]         name of the found attribute                            |
|       a_pdwLength             [ out ]         length of the name of the found attribute              |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_QueryAttributeByValue ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                            /*  in */ PARS_DWORD           a_dwMemFlags,
                                                            /*  in */ PARS_LPSTR           a_strAttributeValue,
                                                            /*  in */ PARS_DWORD           a_dwAttributeValueLen,
                                                            /* out */ PARS_LPSTR*          a_pstrName,
                                                            /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_XML_PT_internalAttributes  attrActual      = PARS_NULL;
    PARS_LPSTR                      strValue        = PARS_NULL;
    PARS_DWORD                      dwLen           = 0;
    PARS_BOOL                       fFound          = PARS_FALSE;
    PARS_DWORD                      dwLength        = 0;
    PARS_XML_PT_internalNode        tmpNode         = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_QueryAttributeByValue");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_strAttributeValue);
    CHECK_NULL_PARAMETER(a_pstrName);
    CHECK_SHUTDOWN_MODE(XML);

    attrActual=(tmpNode->p_atrRoot);
    BAIL_OUT_ON_NULL_SILENT(attrActual, MWS_FALSE); /* em3702: if there is no attribute root no attribute will be found */

    BAIL_OUT_ON_NULL_EX( (PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)),
                         PARS_DEBUG_LEVEL_INFO,
                         MWE_INVALID_PARAMETER,
                         " Invalid Flag specified ");

    do
    {
        if(attrActual->szPrefValue)
        {
            strValue = attrActual->szPrefValue;
            dwLen    = attrActual->dwPrefValueLen;
        }
        else
        {
            strValue = attrActual->posXMLValue;
            dwLen    = attrActual->dwValueLen;
        }
        if(PARS_XML_SAFE_strncmp(strValue, a_strAttributeValue, dwLen, a_dwAttributeValueLen)==0)
        {
            fFound = PARS_TRUE;
        }
        else
        {
            attrActual=attrActual->pNextAttribute;
        }
    } while ((attrActual!=PARS_NULL)&&(!fFound));

    if(fFound)
    {
        if(attrActual->szPrefName)
        {
            *a_pstrName     = attrActual->szPrefName;
            dwLength        = attrActual->dwPrefNameLen;
        }
        else
        {
            *a_pstrName     = attrActual->posXMLName;
            dwLength        = attrActual->dwNameLen;
        }
        if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
        &&(!attrActual->szPrefValue))
        {
            /* make copy for internal use */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( attrActual->posXMLName,
                                                          attrActual->dwNameLen,
                                                         &(attrActual->szPrefName),
                                                         &(attrActual->dwPrefNameLen)),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "Mem Error");
            *a_pstrName = attrActual->szPrefName;
            dwLength = attrActual->dwPrefNameLen;
        }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
        if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
        {
            /* make copy to return this */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrName,
                                                          dwLength,
                                                          a_pstrName,
                                                         &dwLength),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");
        }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
        if (PARS_NULL!=a_pdwLength)
        {
            *a_pdwLength = dwLength;
        }
        else if (PARS_NULL!=attrActual->szPrefValue)
        {
             *a_pstrName    = PARS_NULL;
             BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER,"Invalid Flag and Parameter combination, Name Length must not be PARS_NULL");
        }
    }
    else
    {
        *a_pstrName    = PARS_NULL;
        if (PARS_NULL!=a_pdwLength)
        {
            *a_pdwLength     = 0;
        }
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetNamespaceUri() ---------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for the Uri of specified namespace at the given node or all its parent nodes            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node which will get the new attribute                  |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the namespaceuri             |
|                                               should be copied to allocated memory or not)           |
|                                               only PARS_XML_MEM_FLAG_MAKE_COPY here                  |
|       a_strNamespace          [  in ]         value of the attribute to search                       |
|       a_dwNamespaceLen        [  in ]         length of value of the attribute to search             |
|       a_pstrValue             [ out ]         namespaceuri of the specified namespace                |
|       a_pdwLength             [ out ]         length of the name namespaceuri                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_GetNamespaceUri ( /*  in */ PARS_XML_Node        a_ndeNode,
                                               /*  in */ PARS_DWORD           a_dwMemFlags,
                                               /*  in */ PARS_LPSTR           a_strNamespace,
                                               /*  in */ PARS_DWORD           a_dwNamespaceLen,
                                               /* out */ PARS_LPSTR*          a_pstrValue,
                                               /* out */ PARS_LPDWORD         a_pdwLength)
{
    PARS_LPSTR                  strBuff     = PARS_NULL;
    PARS_XML_PT_internalNode    tmpNode     = (PARS_XML_PT_internalNode) a_ndeNode;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetNamespaceUri");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_SHUTDOWN_MODE(XML);

    if(!a_dwNamespaceLen && a_strNamespace)
    {
        a_dwNamespaceLen = PARS_strlen(a_strNamespace);
    }
    /* +7 is needed space for "xmlns:" and '\0' */
    strBuff    = (PARS_LPSTR)XML_SMemMalloc(a_dwNamespaceLen + 7);
    CHECK_MEMORY(strBuff);

    PARS_strlcpy( strBuff,
                  "xmlns",
                  a_dwNamespaceLen + 7,
                  a_dwNamespaceLen + 7);
    if (a_dwNamespaceLen)
    {
        strBuff[5] = ':';
        PARS_memcpy((strBuff+6), a_strNamespace, a_dwNamespaceLen);
        strBuff[a_dwNamespaceLen+6] = '\0';
    }
    if (a_pstrValue)
    {
        *a_pstrValue = PARS_NULL;
    }
    /* Check for Name space */
    while ((a_pstrValue==PARS_NULL || *a_pstrValue==PARS_NULL) &&
           (tmpNode!=PARS_NULL))
    {
        BAIL_OUT_ON_FAIL_EX( PARS_XML_QueryAttributeByName( ((PARS_XML_Node)tmpNode),
                                                            a_dwMemFlags,
                                                            strBuff,
                                                            PARS_ZERO_TERMINATED_STRING,
                                                            PARS_TRUE,
                                                            a_pstrValue,
                                                            a_pdwLength),
                             PARS_DEBUG_LEVEL_INFO,
                             "Error searching for Namespace Attribute.\n");
        tmpNode = tmpNode->p_ndeParent;
    }
TRANSPARENT_LANDING_ZONE;
    if (strBuff!=PARS_NULL)
    {
        XML_SMemFree(strBuff);
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_FindNodeByTagname() -------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for a node with appropriate tagname; start at specified node                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node to start with search                              |
|       a_dwNavigationFlags     [  in ]         specifies the direction to search for the appropriate  |
|                                               node                                                   |
|       a_strTagName            [  in ]         tagname of the node to find                            |
|       a_dwNamespaceLen        [  in ]         length of tagname to find                              |
|       a_pndeValue             [ out ]         found node                                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_FindNodeByTagname ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                        /*  in */ PARS_DWORD           a_dwNavigationFlags,
                                                        /*  in */ PARS_LPSTR           a_strTagName,
                                                        /*  in */ PARS_DWORD           a_dwTagNameLen,
                                                        /* out */ PARS_XML_Node*       a_pndeValue)
{
    PARS_XML_PT_internalNode    tmpNode         = PARS_NULL;
    PARS_BOOL                   fNoNameSpace    = PARS_FALSE;
    PARS_BOOL                   fFound          = PARS_FALSE;
    XML_TEXT_POS                posXMLTag       = PARS_NULL;
    PARS_DWORD                  dwTagLen        = 0;
    PARS_LPSTR                  strTagName      = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_FindNodeByTagname");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_ZERO_PARAMETER(a_dwNavigationFlags);
    CHECK_NULL_PARAMETER(a_strTagName);
    CHECK_NULL_PARAMETER(a_pndeValue);
    *a_pndeValue = PARS_NULL;
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    if (PARS_strnchr(a_strTagName, ':', a_dwTagNameLen)==PARS_NULL)
    {
        /* No Name space required, ignore any namespace */
        fNoNameSpace=PARS_TRUE;
    }

    PARS_XML_GetNode( (PARS_XML_Node) tmpNode,
                      a_dwNavigationFlags,
                      PARS_XML_NODE_TYPE_ALLNODES,
                      (PARS_XML_Node*) &tmpNode );

    /* set navigation flag for next get node */
    if((PARS_XML_NAVIGATE_DOWN == (a_dwNavigationFlags & PARS_XML_NAVIGATE_DOWN))
    ||(PARS_XML_NAVIGATE_FIRST == (a_dwNavigationFlags & PARS_XML_NAVIGATE_FIRST)))
    {
        a_dwNavigationFlags = PARS_XML_NAVIGATE_NEXT|(a_dwNavigationFlags&~PARS_XML_DIRECTIONS);
    }
    else if (PARS_XML_NAVIGATE_LAST ==(a_dwNavigationFlags & PARS_XML_NAVIGATE_LAST))
    {
        a_dwNavigationFlags = PARS_XML_NAVIGATE_PREV|(a_dwNavigationFlags&~PARS_XML_DIRECTIONS);
    }

    while((tmpNode)&&(!fFound))
    {
        if(tmpNode)
        {
            if(tmpNode->szPrefTagName)
            {
                strTagName = tmpNode->szPrefTagName;
                dwTagLen   = tmpNode->dwPrefTagNameLen;
            }
            else
            {
                strTagName = tmpNode->posXMLTagName;
                dwTagLen = tmpNode->dwTagNameLen;
            }
            if (strTagName)
            {
                if (fNoNameSpace!=PARS_FALSE)
                {
                    posXMLTag=PARS_strnchr(strTagName, ':', dwTagLen);
                    if (posXMLTag==PARS_NULL)
                    {
                        posXMLTag= strTagName;
                    }
                    else
                    {
                        posXMLTag++;
                        dwTagLen = dwTagLen - (posXMLTag-strTagName);
                    }
                }
                else
                {
                    posXMLTag = strTagName;
                }
                if(a_dwNavigationFlags & PARS_XML_IGNORE_CASE)
                {
                    if(PARS_XML_SAFE_strnicmp(posXMLTag, a_strTagName, dwTagLen, a_dwTagNameLen)==0)
                    {
                        fFound = PARS_TRUE;
                    }
                }
                else
                {
                    if(PARS_XML_SAFE_strncmp(posXMLTag, a_strTagName, dwTagLen, a_dwTagNameLen)==0)
                    {
                        fFound = PARS_TRUE;
                    }
                }
            }
            if (!fFound)
            {
                if((PARS_XML_NAVIGATE_ROOT ==(a_dwNavigationFlags & PARS_XML_NAVIGATE_ROOT))
                ||(PARS_XML_NAVIGATE_UP == (a_dwNavigationFlags & PARS_XML_NAVIGATE_UP)))
                {
                    /* no next search allowed */
                    tmpNode = PARS_NULL;
                }
                else
                {
                    PARS_XML_GetNode( (PARS_XML_Node) tmpNode,
                                      a_dwNavigationFlags,
                                      PARS_XML_NODE_TYPE_ALLNODES,
                                      (PARS_XML_Node*)&tmpNode);
                }
            }
        }
    }
    *a_pndeValue = (PARS_XML_Node) tmpNode;
#if PARS_XML_VERBOSE_PARSING
    if (fFound == PARS_FALSE)
    {
        /*PARS_TRACE_1( PARS_DEBUG_LEVEL_HINT, "TagName : <%s> NOT FOUND.\n", a_strTagName);*/
        PND_PARAMS_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "TagName : %d NOT FOUND.", a_strTagName);
    }
#endif /* PARS_XML_VERBOSE_PARSING */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_FindNodeByAttribute() -----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for a node with appropriate attribute name and attribute value; start at specified      |
|       node. If no attribute value is specified, only attribute name is compared.                     |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node to start with search                              |
|       a_dwNavigationFlags     [  in ]         specifies the direction to search for the appropriate  |
|                                               node                                                   |
|       a_strAttributeName      [  in ]         attribute name to search                               |
|       a_dwAttributeNameLen    [  in ]         length of attribute name                               |
|       a_strAttributeValue     [  in ]         attribute value to search                              |
|       a_dwAttributeValueLen   [  in ]         length of attribute value                              |
|       a_pndeValue             [ out ]         found node                                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_FindNodeByAttribute ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                          /*  in */ PARS_DWORD           a_dwNavigationFlags,
                                                          /*  in */ PARS_LPSTR           a_strAttributeName,
                                                          /*  in */ PARS_DWORD           a_dwAttributeNameLen,
                                                          /*  in */ PARS_LPSTR           a_strAttributeValue,
                                                          /*  in */ PARS_DWORD           a_dwAttributeValueLen,
                                                          /* out */ PARS_XML_Node*       a_pndeValue)
{
    PARS_XML_PT_internalNode    tmpNode             = PARS_NULL;
    PARS_BOOL                   fFound              = PARS_FALSE;
    PARS_LPSTR                  strAttributeValue   = PARS_NULL;
    PARS_DWORD                  dwAttributeValueLen = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_FindNodeByAttribute");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_ZERO_PARAMETER(a_dwNavigationFlags);
    CHECK_NULL_PARAMETER(a_strAttributeName);
    CHECK_NULL_PARAMETER(a_pndeValue);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    PARS_XML_GetNode( (PARS_XML_Node) tmpNode,
                      a_dwNavigationFlags,
                      PARS_XML_NODE_TYPE_ALLNODES,
                      (PARS_XML_Node*) &tmpNode );

   /* set navigation flag for next get node */
    if((PARS_XML_NAVIGATE_DOWN ==(a_dwNavigationFlags&PARS_XML_NAVIGATE_DOWN))
    ||(PARS_XML_NAVIGATE_FIRST == (a_dwNavigationFlags&PARS_XML_NAVIGATE_FIRST)))
    {
        a_dwNavigationFlags = PARS_XML_NAVIGATE_NEXT|(a_dwNavigationFlags&~PARS_XML_DIRECTIONS);
    }
    else if (PARS_XML_NAVIGATE_LAST ==(a_dwNavigationFlags & PARS_XML_NAVIGATE_LAST))
    {
        a_dwNavigationFlags = PARS_XML_NAVIGATE_PREV|(a_dwNavigationFlags&~PARS_XML_DIRECTIONS);
    }

    while((tmpNode)&&(!fFound))
    {
        if(tmpNode)
        {
            BAIL_OUT_ON_FAIL_EX( PARS_XML_QueryAttributeByName( (PARS_XML_Node) tmpNode,
                                                                PARS_XML_MEM_FLAG_NO_FLAG,
                                                                a_strAttributeName,
                                                                a_dwAttributeNameLen,
                                                                PARS_TRUE,
                                                               &strAttributeValue,
                                                               &dwAttributeValueLen),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "Query Attribute Error");
            if(strAttributeValue!=PARS_NULL)
            {
                if (a_strAttributeValue==PARS_NULL)    /* Consumer is only interested in the existence */
                {                                      /* of the Attribute                             */
                    fFound = PARS_TRUE;
                }
                else
                {
                    if(a_dwNavigationFlags & PARS_XML_IGNORE_CASE)
                    {
                        if (PARS_XML_SAFE_strnicmp(strAttributeValue, a_strAttributeValue, dwAttributeValueLen, a_dwAttributeValueLen)==0)
                        {
                            fFound = PARS_TRUE;
                        }
                    }
                    else
                    {
                        if (PARS_XML_SAFE_strncmp(strAttributeValue, a_strAttributeValue, dwAttributeValueLen, a_dwAttributeValueLen)==0)
                        {
                            fFound = PARS_TRUE;
                        }
                    }
                }
            }
            if(!fFound)
            {
                if((PARS_XML_NAVIGATE_ROOT ==(a_dwNavigationFlags & PARS_XML_NAVIGATE_ROOT))
                ||(PARS_XML_NAVIGATE_UP == (a_dwNavigationFlags & PARS_XML_NAVIGATE_UP)))
                {
                    /* no next search allowed */
                    tmpNode = PARS_NULL;
                }
                else
                {
                    PARS_XML_GetNode( (PARS_XML_Node) tmpNode,
                                      a_dwNavigationFlags,
                                      PARS_XML_NODE_TYPE_ALLNODES,
                                      (PARS_XML_Node*) &tmpNode );
                }
            }
        }
    }
    *a_pndeValue = (PARS_XML_Node) tmpNode;

#if PARS_XML_VERBOSE_PARSING
    if (fFound == PARS_FALSE)
    {
        /*PARS_TRACE_1( PARS_DEBUG_LEVEL_HINT, "AttributeName : <%s> NOT FOUND\r\n", a_strAttributeName);*/
        PND_PARAMS_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "AttributeName : %d NOT FOUND", a_strAttributeName);
    }
#endif /* PARS_XML_VERBOSE_PARSING */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetRealContent() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Removes all comments and leading and trailing whitespaces                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_strLine               [  bi ]         string where to get real content                       |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT  DLLCALL PARS_XML_GetRealContent ( /*  bi */ PARS_LPSTR   a_strLine )
{
    PARS_LPSTR       strActPos           = a_strLine;
    PARS_LPSTR       strLastPos          = strActPos;
    PARS_BOOL        fFindNext           = PARS_TRUE;
    PARS_LPSTR       szTemp              = PARS_NULL;
    PARS_LPSTR       szToFree            = PARS_NULL;

    PARS_LONG        nFindPos            = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetRealContent");
    CHECK_NULL_PARAMETER(a_strLine);
    if( PARS_XML_MAX_TEMP_BUFFER_SIZE < (PARS_DWORD) (PARS_strlen(strActPos)+1))
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_WARNING, MWE_FAIL,"Internal Temp Buffer to small");
    }

    szTemp = (PARS_LPSTR) XML_SMemMalloc(PARS_XML_MAX_TEMP_BUFFER_SIZE);
    CHECK_MEMORY(szTemp);
    szTemp[0]='\0';
    szToFree = szTemp;
    /* remove comments */
    do
    {
        strActPos  = PARS_strstr(strActPos , "<!--");
        if (strActPos  == PARS_NULL)
        {
            fFindNext = PARS_FALSE;
        }
        else
        {
            PARS_strlcat( szTemp,
                          strLastPos,
                          PARS_XML_MAX_TEMP_BUFFER_SIZE,
                          strActPos-strLastPos);
            PARS_strlcat( szTemp,
                          " ",
                          PARS_XML_MAX_TEMP_BUFFER_SIZE,
                          1);
            strActPos = PARS_strstr(strActPos , "-->");
            if (strActPos==PARS_NULL)
            {
                BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_SYNTAX_ERROR,"Comment is not complete");
            }
            strLastPos=strActPos+3;
        }
    }while (fFindNext);

    if ( strLastPos != a_strLine )
    {
        /* comment found */
        PARS_strlcat( szTemp,
                      strLastPos,
                      PARS_XML_MAX_TEMP_BUFFER_SIZE,
                      PARS_XML_MAX_TEMP_BUFFER_SIZE);
    }
    else
    {
        PARS_strlcpy( szTemp,
                      a_strLine,
                      PARS_XML_MAX_TEMP_BUFFER_SIZE,
                      PARS_XML_MAX_TEMP_BUFFER_SIZE);
    }

    /* trim left */
    nFindPos = PARS_strspn (szTemp , " \t\n\r\v\f");
    if (0!=nFindPos)
    {
        szTemp += nFindPos;
    }

    /* trimright */
    PARS_strrev (szTemp);
    nFindPos = PARS_strspn (szTemp , " \t\n\r\v\f");
    if (0!=nFindPos)
    {
        szTemp += nFindPos;
    }
    PARS_strrev (szTemp);

    PARS_strlcpy( a_strLine,
                  szTemp,
                  PARS_XML_MAX_TEMP_BUFFER_SIZE,
                  PARS_XML_MAX_TEMP_BUFFER_SIZE);
TRANSPARENT_LANDING_ZONE;
    if( szToFree != PARS_NULL )
    {
        XML_SMemFree( szToFree );
        szToFree = PARS_NULL;
    }
END_LANDING_ZONE;
}

/*--- PARS_XML_MoveNode() ----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Move node to specified position                                                                |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node to start with search                              |
|       a_dwInsertionFlags      [  in ]         specifies the direction to insert the node from the    |
|                                               destination node                                       |
|       a_ndeDestinationNode    [  in ]         node to link specified node to                         |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation                                                      |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_MoveNode ( /*  in */ PARS_XML_Node        a_ndeNode,
                                               /*  in */ PARS_DWORD           a_dwInsertionFlags,
                                               /*  in */ PARS_XML_Node        a_ndeDestinationNode)
{
    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_MoveNode");
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_NULL_PARAMETER(a_ndeDestinationNode);
    CHECK_ZERO_PARAMETER(a_dwInsertionFlags);
    CHECK_SHUTDOWN_MODE(XML);

    BAIL_OUT_ON_FAIL_EX( PARS_XML_RemoveNode(a_ndeNode),
                         PARS_DEBUG_LEVEL_INFO,
                         "Remove node from Tree failed ");

    BAIL_OUT_ON_FAIL_EX( PARS_XML_InsertNode(a_ndeNode, a_ndeDestinationNode, a_dwInsertionFlags),
                         PARS_DEBUG_LEVEL_INFO,
                         "Insert moved node in tree failed");

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_CreateNode() --------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       creates a xml node and links it into the xml-tree at the given position                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  bi ]       node to link the new node to                             |
|       a_dwNavigationFlags     [  in ]       direction to link the new node                           |
|       a_dwNodeType            [  in ]       type of the node to create                               |
|       a_pndeValue             [ out ]       new created node                                         |
|                                                                                                      |
|    Return Value:                                                                                     |
|        MWE_NO_ERROR           ok.                                                                    |
|        MWE_INVALID_PARAMETER  invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_CreateNode ( /*  in */ PARS_XML_Node        a_ndeNode,
                                                 /*  in */ PARS_DWORD           a_dwInsertionFlags,
                                                 /*  in */ PARS_DWORD           a_dwNodeType,
                                                 /* out */ PARS_XML_Node*       a_pndeValue)
{
    PARS_XML_PT_internalNode    ndeNewNode  = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_CreateNode");
    CHECK_SHUTDOWN_MODE(XML);

    *a_pndeValue = (PARS_XML_Node) PARS_NULL;

    BAIL_OUT_ON_FAIL_EX( PARS_XML_NewNode((PARS_XML_Node*)&ndeNewNode),
                         PARS_DEBUG_LEVEL_INFO,
                         "Creation of node failed");

    ndeNewNode->dwNodeType = a_dwNodeType;

    BAIL_OUT_ON_FAIL_EX( PARS_XML_InsertNode( (PARS_XML_Node)ndeNewNode,
                                              a_ndeNode,
                                              a_dwInsertionFlags),
                         PARS_DEBUG_LEVEL_INFO,
                         "Insert Created Node in Tree failed ");

    *a_pndeValue = (PARS_XML_Node) ndeNewNode;

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetNode() -----------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Search for a node from specified start node; Only nodes of the specified type are considered   |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_ndeNode               [  in ]         node to start with search                              |
|       a_dwNavigationFlags     [  in ]         specifies the direction to search for the appropriate  |
|                                               node                                                   |
|       a_dwNodeType            [  in ]         type of node to find                                   |
|       a_pndeValue             [ out ]         found node                                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_MEMORY              memory allocation error                                                |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_GetNode  ( /*  in */ PARS_XML_Node  a_ndeNode,
                                        /*  in */ PARS_DWORD     a_dwNavigationFlags,
                                        /*  in */ PARS_DWORD     a_dwNodeType,
                                        /* out */ PARS_XML_Node* a_pndeValue)
{
    PARS_XML_PT_internalNode    tmpNode = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetNode");
    CHECK_NULL_PARAMETER(a_pndeValue);
    *a_pndeValue = PARS_NULL;
    CHECK_NULL_PARAMETER(a_ndeNode);
    CHECK_ZERO_PARAMETER(a_dwNavigationFlags);
    CHECK_SHUTDOWN_MODE(XML);

    tmpNode = (PARS_XML_PT_internalNode) a_ndeNode;

    do
    {
        switch(a_dwNavigationFlags&PARS_XML_DIRECTIONS)
        {
        case PARS_XML_NAVIGATE_UP :     if (tmpNode != (PARS_XML_PT_internalNode) a_ndeNode)
                                        {
                                            /* second try is not allowed */
                                            *a_pndeValue = PARS_NULL;
                                        }
                                        else
                                        {
                                            *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndeParent;
                                        }
                                        break;
        case PARS_XML_NAVIGATE_DOWN :     *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndeFirstChild;
                                        /* set direction for second try ( if necessary ) */
                                        a_dwNavigationFlags = PARS_XML_NAVIGATE_NEXT|(a_dwNavigationFlags&~PARS_XML_DIRECTIONS);
                                        break;
        case PARS_XML_NAVIGATE_PREV :     *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndePrevNeighbour;
                                        break;
        case PARS_XML_NAVIGATE_NEXT :   *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndeNextNeighbour;
                                        break;
        case PARS_XML_NAVIGATE_FIRST :  if(tmpNode->p_ndeParent)
                                        {
                                            *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndeParent->p_ndeFirstChild;
                                        }
                                        break;
        case PARS_XML_NAVIGATE_LAST :   if(tmpNode->p_ndeParent)
                                        {
                                            *a_pndeValue = (PARS_XML_Node) tmpNode->p_ndeParent->p_ndeLastChild;
                                        }
                                        break;
        case PARS_XML_NAVIGATE_ROOT :   *a_pndeValue = (PARS_XML_Node) tmpNode->p_shtXMLSheet->p_ndeRoot;
                                        if ((a_dwNodeType & PARS_XML_NODE_TYPE_ELEMENT)!= PARS_XML_NODE_TYPE_ELEMENT)
                                        {   /* root node must be type element */
                                            *a_pndeValue = PARS_NULL;
                                        }
                                        break;
        default                     :   *a_pndeValue = PARS_NULL;
                                        break;
        }
        tmpNode  = (PARS_XML_PT_internalNode)*a_pndeValue;
    } while ((tmpNode)&&(0==(tmpNode->dwNodeType & a_dwNodeType)));

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SetXMLSheetBom() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Set the Byte Order Mask of the XML-File                                                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         XMLSheet to set the BOM                                |
|       a_strValue              [  in ]         BOM to set                                             |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_SetXMLSheetBom ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                              /*  in */ PARS_LPSTR          a_strValue)
{
    PARS_XML_PT_internalSheet    shtWork         = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SetXMLSheetBom");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strValue);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    PARS_strlcpy(shtWork->strBom, a_strValue, XML_BOMLEN, XML_BOMLEN-1);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetXMLSheetBom() ----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the Byte Order Mask of the XML-File                                                    |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         XMLSheet to return the BOM                             |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the BOM                      |
|                                               should be copied to allocated memory or not)           |
|       a_pstrValue             [ out ]         BOM to return                                          |
|       a_pdwLength             [ out ]         Length of BOM                                          |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_GetXMLSheetBom ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                              /*  in */ PARS_DWORD          a_dwMemFlags,
                                              /* out */ PARS_LPSTR*         a_pstrValue,
                                              /* out */ PARS_LPDWORD        a_pdwLength)
{
    PARS_XML_PT_internalSheet   shtWork     = PARS_NULL;
    PARS_DWORD                  dwLength    = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetXMLSheetBOM");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    *a_pstrValue = shtWork->strBom;
    if(*a_pstrValue)
    {
        dwLength= PARS_strlen(*a_pstrValue);
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
        if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
        {
            /* make copy to return this */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrValue,
                                                          dwLength,
                                                          a_pstrValue,
                                                         &dwLength),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");
        }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */
    }
    if(a_pdwLength)
    {
        *a_pdwLength = dwLength;
    }
LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_SetXMLSheetAttribute() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Set the value of a XML-Sheet Attribute                                                         |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         actual XMLSheet                                        |
|       a_strAttributeName      [  in ]         name of the attribute to set value to                  |
|       a_pstrValue             [  in ]         value to set                                           |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_SetXMLSheetAttribute ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                    /*  in */ PARS_LPSTR          a_strAttributeName,
                                                    /*  in */ PARS_LPSTR          a_strValue)
{
    PARS_XML_PT_internalSheet   shtWork = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_SetXMLSheetAttribute");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strAttributeName);
    CHECK_NULL_PARAMETER(a_strValue);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if (!(PARS_strcmp(XML_ATTR_VERSION, a_strAttributeName)))
    {
        /* set version */
        PARS_strlcpy(shtWork->strVersion, a_strValue, XML_VERSIONLEN, XML_VERSIONLEN-1);
    }
    else if(!(PARS_strcmp(XML_ATTR_ENCODING, a_strAttributeName)))
    {
        /* set encoding */
        PARS_strlcpy(shtWork->strEncoding, a_strValue, XML_ENCODELEN, XML_ENCODELEN-1);
    }
    else if(!(PARS_strcmp(XML_ATTR_STANDALONE, a_strAttributeName)))
    {
        /* set standalone */
        PARS_strlcpy(shtWork->strStandAlone, a_strValue, XML_STANDALONELEN, XML_STANDALONELEN-1);

        if (PARS_strncmp(a_strValue,XML_ATTR_STANDALONE_VALUE_YES,PARS_strlen(XML_ATTR_STANDALONE_VALUE_YES))==0)
        {
            shtWork->XML_SheetOptions.bfStandAlone=1;
        }
        else
        {
            shtWork->XML_SheetOptions.bfStandAlone=0;
        }
    }
    else
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER,"AttributeName not allowed");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetXMLSheetAttribute() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the value of a XML-Sheet Attribute                                                     |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         actual XMLSheet                                        |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the attribute value          |
|                                               should be copied to allocated memory or not)           |
|       a_strAttributeName      [  in ]         name of the attribute to get value from                |
|       a_pstrValue             [ out ]         value of the attribute                                 |
|       a_pdwLength             [ out ]         length of value                                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_GetXMLSheetAttribute ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                    /*  in */ PARS_DWORD          a_dwMemFlags,
                                                    /*  in */ PARS_LPSTR          a_strAttributeName,
                                                    /* out */ PARS_LPSTR*         a_pstrValue,
                                                    /* out */ PARS_LPDWORD        a_pdwLength)
{
    PARS_XML_PT_internalSheet   shtWork = PARS_NULL;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetXMLSheetAttribute");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strAttributeName);
    CHECK_NULL_PARAMETER(a_pstrValue);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if (!(PARS_strcmp(XML_ATTR_VERSION, a_strAttributeName)))
    {
        /* get version */
        *a_pstrValue = shtWork->strVersion;
        *a_pdwLength = PARS_strlen(*a_pstrValue);
    }
    else if(!(PARS_strcmp(XML_ATTR_ENCODING, a_strAttributeName)))
    {
        /* get encoding */
        *a_pstrValue = shtWork->strEncoding;
        *a_pdwLength = PARS_strlen(*a_pstrValue);
    }
    else if(!(PARS_strcmp(XML_ATTR_STANDALONE, a_strAttributeName)))
    {
        /* get standalone */
        *a_pstrValue = shtWork->strStandAlone;
        *a_pdwLength = PARS_strlen(*a_pstrValue);
    }
    else
    {
        BAIL_OUT_0( PARS_DEBUG_LEVEL_INFO, MWE_INVALID_PARAMETER,"AttributeName not allowed");
    }
#ifdef PARS_XML_MEM_FLAG_GIVE_A_COPY
    if(PARS_XML_MEM_FLAG_GIVE_A_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_GIVE_A_COPY))
    {
        /* make copy to return this */
        BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString(*a_pstrValue,
                                                     *a_pdwLength,
                                                      a_pstrValue,
                                                      a_pdwLength),
                             PARS_DEBUG_LEVEL_INFO,
                             "MemError");
    }
#endif /* PARS_XML_MEM_FLAG_GIVE_A_COPY */

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_AddXMLSheetPI() -----------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Creates an Processing Instruction and links it to the actual sheet                             |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         actual XMLSheet                                        |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the PI                       |
|                                               should be copied to allocated memory or not)           |
|       a_strPI                 [  in ]         PI to add                                              |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_AddXMLSheetPI ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                    /*  in */ PARS_DWORD          a_dwMemFlags,
                                                    /*  in */ PARS_LPSTR          a_strPI)
{
    PARS_XML_PT_internalSheet   shtWork         = PARS_NULL;
    PARS_LPSTR                  strPI           = PARS_NULL;
    PARS_XML_PT_internalPI      piActual        = PARS_NULL;
    PARS_XML_PT_internalPI      piNext          = PARS_NULL;
    PARS_XML_PT_internalPI      piNew           = PARS_NULL;
    PARS_DWORD                  dwPILen         = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_AddXMLSheetPI");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strPI);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;
    dwPILen = PARS_strlen(a_strPI);
    /* PARS_XML_MEM_FLAG_MAKE_COPY */
    if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
    {
        strPI = (PARS_LPSTR) XML_SMemMalloc(dwPILen+1);
        CHECK_MEMORY(strPI);
        PARS_strlcpy( strPI,a_strPI, dwPILen+1, dwPILen);
    }
    else
    {
        strPI       = a_strPI;
    }

    BAIL_OUT_ON_FAIL_EX( PARS_XML_NewPI((PARS_XML_PI*)&(piNew)), PARS_DEBUG_LEVEL_INFO, "Create new PI failed");

    if(PARS_NULL==(shtWork->p_piRoot))
    {
        shtWork->p_piRoot = piNew;
    }
    else
    {
        piNext = shtWork->p_piRoot;
        while(PARS_NULL!=piNext)
        {
            piActual    = piNext;
            piNext      = piActual->pNextPI;
        }
        piActual->pNextPI = piNew;
    }
    piNew->szPrefPI = strPI;
    piNew->dwPrefPILen = PARS_strlen(strPI);

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetXMLSheetAttribute() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the value of a XML-Sheet Attribute                                                     |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         actual XMLSheet                                        |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the attribute value          |
|                                               should be copied to allocated memory or not)           |
|       a_strAttributeName      [  in ]         name of the attribute to get value from                |
|       a_pstrValue             [ out ]         value of the attribute                                 |
|       a_pdwLength             [ out ]         length of value                                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_GetXMLSheetPI ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                    /*  in */ PARS_DWORD          a_dwMemFlags,
                                                    /*  in */ PARS_LPSTR          a_strPITarget,
                                                    /*  in */ PARS_DWORD          a_dwPITargetLen,
                                                    /* out */ PARS_LPSTR*         a_pstrValue,
                                                    /* out */ PARS_LPDWORD        a_pdwLength)
{
    PARS_XML_PT_internalSheet   shtWork         = PARS_NULL;
    PARS_LPSTR                  strPI           = PARS_NULL;
    PARS_XML_PT_internalPI      piActual        = PARS_NULL;
    PARS_DWORD                  dwPILen         = 0;
    PARS_BOOL                   fFound          = PARS_FALSE;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetXMLSheetPI");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strPITarget);
    CHECK_ZERO_PARAMETER(a_dwPITargetLen);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if(PARS_ZERO_TERMINATED_STRING== a_dwPITargetLen)
    {
        a_dwPITargetLen = PARS_strlen(a_strPITarget);
    }

    piActual = shtWork->p_piRoot;
    while((PARS_NULL!=piActual)&&(!fFound))
    {
        if((piActual->szPrefPI)
            &&(PARS_XML_MEM_FLAG_FORCE_ORIGINAL !=(a_dwMemFlags & PARS_XML_MEM_FLAG_FORCE_ORIGINAL)))
        {
            strPI = piActual->szPrefPI;
            dwPILen = piActual->dwPrefPILen;
        }
        else
        {
            strPI = piActual->posXMLPI;
            dwPILen = piActual->dwPILen;
        }
        if((a_dwPITargetLen<=dwPILen)
            &&(PARS_strncmp(a_strPITarget, strPI, a_dwPITargetLen)==0))
        {
            fFound = PARS_TRUE;
        }
        else
        {
/*            piPrev = piActual;    */  /* VF Never used */
            piActual = piActual->pNextPI;
        }
    }

    if (fFound)
    {
        /* PARS_XML_MEM_FLAG_COPY_ON_DEMAND */
        if((PARS_XML_MEM_FLAG_COPY_ON_DEMAND == (a_dwMemFlags & PARS_XML_MEM_FLAG_COPY_ON_DEMAND))
        &&(!piActual->szPrefPI))
        {
            /* make copy for internal use */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( strPI,
                                                          dwPILen,
                                                         &(piActual->szPrefPI),
                                                         &(piActual->dwPrefPILen)),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");

            *a_pstrValue = piActual->szPrefPI;
        }
        else
        {
            *a_pstrValue = strPI;
            *a_pdwLength = dwPILen;
        }

        /* PARS_XML_MEM_FLAG_MAKE_COPY */
        if (PARS_XML_MEM_FLAG_MAKE_COPY == (a_dwMemFlags & PARS_XML_MEM_FLAG_MAKE_COPY))
        {
            /* make copy for internal use */
            BAIL_OUT_ON_FAIL_EX( PARS_XML_MakeCopyString( strPI,
                                                          dwPILen,
                                                          a_pstrValue,
                                                         &dwPILen),
                                 PARS_DEBUG_LEVEL_INFO,
                                 "MemError");

        }
        if(a_pdwLength)
        {
            *a_pdwLength = dwPILen;
        }
   }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_DeleteXMLSheetPI() --------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       XML Service Interface                                                                          |
|                                                                                                      |
|    Description:                                                                                      |
|       Creates an Processing Instruction and links it to the actual sheet                             |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLSheet           [  in ]         actual XMLSheet                                        |
|       a_dwMemFlags            [  in ]         memory flag (specifies if the PI                       |
|                                               should be copied to allocated memory or not)           |
|       a_strPI                 [  in ]         PI to add                                              |
|                                                                                                      |
|    Return Value:                                                                                     |
|       MWE_NO_ERROR            ok.                                                                    |
|       MWE_INVALID_PARAMETER   invalid parameter                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
static PARS_RESULT DLLCALL PARS_XML_DeleteXMLSheetPI ( /*  in */ PARS_XML_Sheet      a_shtXMLSheet,
                                                       /*  in */ PARS_LPSTR          a_strPITarget,
                                                       /*  in */ PARS_DWORD          a_dwPITargetLen)
{
    PARS_XML_PT_internalSheet   shtWork     = PARS_NULL;
    PARS_LPSTR                  strPI       = PARS_NULL;
    PARS_XML_PT_internalPI      piActual    = PARS_NULL;
    PARS_XML_PT_internalPI      piPrev      = PARS_NULL;
    PARS_BOOL                   fFound      = PARS_FALSE;
    PARS_DWORD                  dwPILen     = 0;

    RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_DeleteXMLSheetPI");
    CHECK_NULL_PARAMETER(a_shtXMLSheet);
    CHECK_NULL_PARAMETER(a_strPITarget);
    CHECK_SHUTDOWN_MODE(XML);

    shtWork = (PARS_XML_PT_internalSheet) a_shtXMLSheet;

    if(PARS_ZERO_TERMINATED_STRING== a_dwPITargetLen)
    {
        a_dwPITargetLen = PARS_strlen(a_strPITarget);
    }

    BAIL_OUT_ON_NULL_EX( shtWork->p_piRoot, PARS_DEBUG_LEVEL_INFO, MWE_SYNTAX_ERROR, "PI not found");

    piActual = shtWork->p_piRoot;
    while((PARS_NULL!=piActual)&&(!fFound))
    {
        if(piActual->szPrefPI)
        {
            strPI = piActual->szPrefPI;
            dwPILen = piActual->dwPrefPILen;
        }
        else
        {
            strPI = piActual->posXMLPI;
            dwPILen = piActual->dwPILen;
        }
        if ((a_dwPITargetLen<=dwPILen)
             &&(PARS_strncmp(a_strPITarget, strPI, a_dwPITargetLen)==0))
        {
            fFound = PARS_TRUE;
        }
        else
        {
            piPrev = piActual;
            piActual = piActual->pNextPI;
        }
    }
    if(fFound)
    {
        if (piPrev)
        {
            piPrev->pNextPI = piActual->pNextPI;
        }
        else
        {
            shtWork->p_piRoot = piActual->pNextPI;
        }
        BAIL_OUT_ON_FAIL_EX( PARS_XML_DeletePI((PARS_XML_PI) piActual), PARS_DEBUG_LEVEL_INFO, "Delete found PI failed");
    }

LANDING_ZONE;
END_LANDING_ZONE;
}

/*--- PARS_XML_GetSheetDoc() -------------------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal helper function XML Service                                                           |
|                                                                                                      |
|    Description:                                                                                      |
|       Returns the text document used to create the sheet, if there is one                            |
|                                                                                                      |
|    Parameters:                                                                                       |
|       a_shtXMLDocument        [  in ]   Sheet to use                                                 |
|       a_pstrSheetDocument     [ out ]   Pointer to the text used.                                    |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_RESULT                                                                                    |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_XML_GetSheetDoc( /*  in */ PARS_XML_Sheet      a_shtXMLDocument,
                                          /* out */ PARS_LPSTR*         a_pstrSheetDocument)
{
RETURN_INIT (MWE_FAC_XML_SERVICE, "PARS_XML_GetSheetDoc");
    CHECK_NULL_PARAMETER(a_shtXMLDocument);
    CHECK_NULL_PARAMETER(a_pstrSheetDocument);
    (*a_pstrSheetDocument) = ((PARS_XML_PT_internalSheet)a_shtXMLDocument)->strXMLOrigXMLDoc;
    if((*a_pstrSheetDocument)==PARS_NULL)
    {
        SET_ACTUAL_ERROR(MWE_NOT_FOUND);
    }
LANDING_ZONE;
END_LANDING_ZONE;
}


/*--- PARS_XML_BuildXMLServiceDescr() ----------------------------------------------------------------*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|       internal helper function XML Service                                                           |
|                                                                                                      |
|    Description:                                                                                      |
|       Create XMLService interface table                                                              |
|                                                                                                      |
|    Parameters:                                                                                       |
|       pXML                    [ out ]         pointer to xmlservice interface                        |
|                                                                                                      |
|    Return Value:                                                                                     |
|       PARS_VOID                                                                                      |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*----------------------------------------------------------------------------------------------------*/
PARS_LONG   (FileClose)               ( /*  in */ PARS_FILE          a_fileHandle)
{
	return fclose((FILE*)a_fileHandle);
}

PARS_LONG   (FileFlush)               ( /*  in */ PARS_FILE          a_fileHandle)
{
	return fflush((FILE*)a_fileHandle);
}

PARS_VOID  (XmlWrite)	( /*  in */ const PARS_LPVOID  a_pBuffer)
{
	sprintf(xmlBuffer+xmlBufferLength, (char*)a_pBuffer);
	xmlBufferLength = strlen(xmlBuffer);
}

PARS_VOID PARS_AllocateXmlBuffer()
{
	xmlBufferLength = 0;
	xmlBuffer = (LSA_CHAR*)xml_malloc(1024);
}

LSA_CHAR* GetXmlBuffer()
{
	return xmlBuffer;
}

PARS_DWORD GetXmlBufferLength()
{
	return xmlBufferLength;
}

PARS_VOID PARS_XML_BuildXMLServiceDescr ( /*  in */ XMLServiceInterface *a_pXML)
{
    a_pXML->dwStructSize               = sizeof(struct XMLServiceInterface_);
    a_pXML->GetMMI                     = PARS_NULL;

    a_pXML->CreateSheet                = PARS_XML_CreateSheet;
    a_pXML->GetSheetDoc                = PARS_XML_GetSheetDoc;

    a_pXML->DeleteSheet                = PARS_XML_DeleteSheet;
    a_pXML->GetSheet                   = PARS_XML_GetSheet;
    a_pXML->ExpandNode                 = PARS_XML_ExpandNode;
    a_pXML->DeleteNode                 = PARS_XML_DeleteNode;
    a_pXML->DeleteAttribute            = PARS_XML_DeleteAttribute;

    a_pXML->GetChildCount              = PARS_XML_GetChildCount;
    a_pXML->GetTagName                 = PARS_XML_GetTagName;
    a_pXML->GetContent                 = PARS_XML_GetContent;
    a_pXML->IsFirstChild               = PARS_XML_IsFirstChild;
    a_pXML->GetDTD                     = PARS_XML_GetDTD;
    a_pXML->GetRootNode                = PARS_XML_GetRootNode;
    a_pXML->QueryAttributeByName       = PARS_XML_QueryAttributeByName;
    a_pXML->QueryAttributeByValue      = PARS_XML_QueryAttributeByValue;
    a_pXML->FindNodeByTagname          = PARS_XML_FindNodeByTagname;
    a_pXML->FindNodeByAttribute        = PARS_XML_FindNodeByAttribute;
    a_pXML->GetTagnameNamespace        = PARS_XML_GetTagnameNamespace;
    a_pXML->GetNamespaceUri            = PARS_XML_GetNamespaceUri;

    a_pXML->ExpandContent              = PARS_XML_ExpandContent;
    a_pXML->CreateNode                 = PARS_XML_CreateNode;
	a_pXML->Generate_Rema_XML_Buffer   = PARS_Generate_Rema_XML_Buffer;
    a_pXML->SetTagName                 = PARS_XML_SetTagName;
    a_pXML->SetContent                 = PARS_XML_SetContent;
    a_pXML->AddAttribute               = PARS_XML_AddAttribute;
    a_pXML->SetDTD                     = PARS_XML_SetDTD;
    a_pXML->GetXMLText                 = PARS_XML_GetXMLText;
    a_pXML->GetXMLTextAsync            = PARS_XML_GetXMLTextAsync;

    a_pXML->GetNodeTextAsync           = PARS_XML_GetNodeTextAsync;

    a_pXML->GetRealContent             = PARS_XML_GetRealContent;

    a_pXML->GetNode                    = PARS_XML_GetNode;
    a_pXML->MoveNode                   = PARS_XML_MoveNode;

    a_pXML->SetXMLSheetBom             = PARS_XML_SetXMLSheetBom;
    a_pXML->GetXMLSheetBom             = PARS_XML_GetXMLSheetBom;

    a_pXML->SetXMLSheetAttribute       = PARS_XML_SetXMLSheetAttribute;
    a_pXML->GetXMLSheetAttribute       = PARS_XML_GetXMLSheetAttribute;

    a_pXML->AddXMLSheetPI              = PARS_XML_AddXMLSheetPI;
    a_pXML->DeleteXMLSheetPI           = PARS_XML_DeleteXMLSheetPI;
    a_pXML->GetXMLSheetPI              = PARS_XML_GetXMLSheetPI;
    a_pXML->EnterSheet                 = PARS_XML_EnterSheet;
    a_pXML->LeaveSheet                 = PARS_XML_LeaveSheet;

    a_pXML->EscapeContent              = PARS_XML_EscapeContent;

#if PARS_XML_ENABLE_XPATH
    a_pXML->XPath                      = PARS_XML_XPath;
    a_pXML->XPathString                = PARS_XML_XPathString;
    a_pXML->TestXPathPreparser         = PARS_XML_TestXPathPreparser;
#endif /* PARS_XML_ENABLE_XPATH */

#if PARS_XML_ENABLE_SXPARSER
    a_pXML->SXFeedParser               = PARS_XML_SXFeedParser;
    a_pXML->SXGetNextFrag              = PARS_XML_SXGetNextFrag;
    a_pXML->SXQuitParser               = PARS_XML_SXQuitParser;
#endif /* PARS_XML_ENABLE_SXPARSER    */

#if PARS_XML_ENABLE_XMLWRITER
    a_pXML->InitWriteCtxt              = PARS_XML_W_CtxtInit;
    a_pXML->DeleteWriteCtxt            = PARS_XML_W_CtxtDelete;
    a_pXML->WriteTagStart              = PARS_XML_W_WriteTagStart;
    a_pXML->WriteTagEnd                = PARS_XML_W_WriteTagEnd;
    a_pXML->WriteTagValue              = PARS_XML_W_WriteTagValue;
    a_pXML->WriteAttribute             = PARS_XML_W_WriteAttribute;
    a_pXML->WriteComment               = PARS_XML_W_WriteComment;
#endif /* PARS_XML_ENABLE_XMLWRITER */

	PARS_XML_g_pFile->fclose   		= FileClose;
	PARS_XML_g_pFile->fflush   		= FileFlush;
	PARS_XML_g_pFile->xmlwrite		= XmlWrite;

	PARS_XML_g_pMem->DbgSMemMalloc = PARS_SafeMalloc;
	PARS_XML_g_pMem->DbgSMemFree   = PARS_SafeFree;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

