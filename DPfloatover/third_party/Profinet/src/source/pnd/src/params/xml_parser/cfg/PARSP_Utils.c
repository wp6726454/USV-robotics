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
/*  F i l e               &F: PARSP_Utils.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  !!! PORTLAYER !!!                                                        */
/*  xml parser runtime library, use functionality from crt when possible     */
/*  This file has to by modified during porting to a new system.             */
/*                                                                           */
/*****************************************************************************/

/* ---  ------------------------------------------------------------------------------------------------------------- --- *\
|   Includes                                                                                                    Includes   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>
//#include <direct.h>
#include <math.h>
#include <stdarg.h>


#include "PARSP_Config.h"
#include "PARS_Types.h"
#include "PARSP_Utils.h"
#include "PARS_MCB.h"

#include "PARSP_File.h"

#include "pnd_sys.h"

/* ---  ------------------------------------------------------------------------------------------------------------- --- *\
|   Disable junk warnings                                                                          Disable junk warnings   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */
#ifdef WIN32
  #pragma warning (disable : 4706)
#endif /* WIN32 */

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|   Helper functions (always available)                                                               Helper functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/

/*----------------------------------------------------------------------------------------------------*\
|   System Storage Access                                                      System Storage Access   |
\*----------------------------------------------------------------------------------------------------*/

/*--- PARSP_OpenStorage() ------------------------------------------------------------------------ PARSP_OpenStorage() ---*/
/**
 *  @brief   Helper function for OpenStorage and load storage to open the destination file
 *
 *  @param   a_strStorage      [  in ]    Name of the storage
 *  @param   a_strFileName     [  in ]    Name of the file
 *  @param   a_strAccess       [  in ]    Access of the file: "wb", "rb"
 *  @param   a_pStorage        [ out ]    File handle to the storage file
 *
 *  @return  PARS_LPBYTE   Beginning of matching memory Block
 */
/*--- PARSP_OpenStorage() ------------------------------------------------------------------------ PARSP_OpenStorage() ---*/
//static PARS_RESULT     DLLCALL PARSP_OpenStorage( /*  in */ PARS_LPSTR        a_strStorage,
//                                                  /*  in */ PARS_LPSTR        a_strFileName,
//                                                  /*  in */ PARS_LPSTR        a_strAccess,
//                                                  /* out*/  PARS_LPVOID*      a_pStorage)
//{
//    PARS_LPSTR                  strSystemRoot           = PARS_NULL;
//    PARS_CHAR                   strLocalPath[PARS_MAX_URL_LEN];
//
//    PARS_DWORD                  nPathLen                = 0;
//RETURN_INIT(MWE_FAC_DEFAPP , "PARSP_OpenStorage");
//    MCB->GetSystemRoot(&strSystemRoot);
//    /* ignore leading '/' or '\\' */
//    if ( a_strFileName[0]=='/')
//    {
//        a_strFileName++;    /* Over read leading '/, \' */
//    }
//
//    nPathLen = PARS_strlen( strSystemRoot);
//    nPathLen += a_strStorage?PARS_strlen(a_strStorage)+1:0;  /* +1 = '/' */
//    nPathLen += PARS_strlen(a_strFileName) + 2;              /* +2 = '/' and '\0' */
//
//    PARS_strlcpy( strLocalPath, strSystemRoot, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//    if (a_strStorage)
//    {
//        PARS_strlcat(strLocalPath, a_strStorage, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//        PARS_strlcat(strLocalPath, "/", PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//
//    }
//    PARS_strlcat(strLocalPath, a_strFileName, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//
//    fopen_s( (FILE**) a_pStorage, strLocalPath, a_strAccess);
//    if((*a_pStorage) == PARS_NULL)
//    {
//        SET_ACTUAL_ERROR(MWE_NOT_FOUND);
//    }
//LANDING_ZONE;
//END_LANDING_ZONE;
//}

/*--- PARSP_StoreSystemData() ---------------------------------------------------------------- PARSP_StoreSystemData() ---*/
/**
 *  @brief   Stores a memory block to the system storage
 *
 *  @param   a_strStorage      [  in ]    Name of the storage
 *  @param   a_strFileName     [  in ]    Name of the file
 *  @param   a_pData           [  in ]    Data block to store
 *  @param   a_dwDataLen       [  in ]    Size of the block
 *
 *  @return  PARS_LPBYTE   Beginning of matching memory Block
 */
/*--- PARSP_StoreSystemData() ---------------------------------------------------------------- PARSP_StoreSystemData() ---*/
//PARS_RESULT     DLLCALL PARSP_StoreSystemData( /*  in */ PARS_LPSTR        a_strStorage,
//                                               /*  in */ PARS_LPSTR        a_strFileName,
//                                               /*  in */ PARS_LPVOID       a_pData,
//                                               /*  in */ PARS_DWORD        a_dwDataLen)
//{
//    PARS_LPVOID     httpFile                = PARS_NULL;
//RETURN_INIT(MWE_FAC_DEFAPP , "PARSP_StoreSystemData");
//    /* Saving Certificate and Key file */
//    BAIL_OUT_ON_FAIL_SILENT(PARSP_OpenStorage(a_strStorage,a_strFileName, "wb", &httpFile));
//    if(a_dwDataLen==PARS_ZERO_TERMINATED_STRING)
//    {
//        a_dwDataLen = PARS_strlen( (PARS_LPSTR)a_pData);
//    }
//    fwrite( a_pData, a_dwDataLen, 1, (FILE *) httpFile);
//    fclose( (FILE *) httpFile);
//    httpFile = PARS_NULL;
//
//LANDING_ZONE;
//END_LANDING_ZONE;
//}

/*--- PARSP_LoadSystemData() ------------------------------------------------------------------ PARSP_LoadSystemData() ---*/
/**
 *  @brief   Loads a memory block from the system storage
 *
 *  @param   a_strStorage      [  in ]    Name of the storage
 *  @param   a_strFileName     [  in ]    Name of the file
 *  @param   a_pMalloc         [  in ]    Function to fetch memory for the buffer
 *  @param   a_ppBuffer        [ out ]    Data block to store
 *  @param   a_pdwBuffersize   [ out ]    Size of the block
 *
 *  @return  PARS_LPBYTE   Beginning of matching memory Block
 */
/*--- PARSP_LoadSystemData() ------------------------------------------------------------------ PARSP_LoadSystemData() ---*/
//PARS_RESULT     DLLCALL PARSP_LoadSystemData( /*  in */ PARS_LPSTR        a_strStorage,
//                                              /*  in */ PARS_LPSTR        a_strFileName,
//                                              /*  in */ PARS_MALLOC       a_pMalloc,
//                                              /* out */ PARS_LPVOID*      a_ppBuffer,
//                                              /* out */ PARS_LPDWORD      a_pdwBuffersize)
//{
//    PARS_LPVOID     httpFile                = PARS_NULL;
//    PARS_DWORD      dwSize                  = 0;
//    PARS_LONG       nFilePos    = 0;
//RETURN_INIT(MWE_FAC_DEFAPP , "PARSP_LoadSystemData");
//    /* Saving Certificate and Key file */
//    BAIL_OUT_ON_FAIL_SILENT(PARSP_OpenStorage(a_strStorage,a_strFileName, "rb", &httpFile));
//    nFilePos = ftell((FILE *)httpFile);
//    if ( 0!=fseek((FILE *)httpFile, 0, SEEK_END ) )
//    {
//        BAIL_OUT_SILENT(MWE_UNEXPECTED);
//    }
//    dwSize=ftell((FILE *)httpFile);
//    if(fseek((FILE *)httpFile, nFilePos, SEEK_SET))
//    {
//        BAIL_OUT_SILENT(MWE_UNEXPECTED);
//    }
//#if PARS_MEMORY_TRACE
//     *a_ppBuffer = a_pMalloc(dwSize+1, __LINE__, __FILE__);
//#else /* PARS_MEMORY_TRACE */
//    *a_ppBuffer = a_pMalloc(dwSize+1);
//#endif /* PARS_MEMORY_TRACE */
//    CHECK_MEMORY(*a_ppBuffer);
//
//    fread(*a_ppBuffer,1,dwSize,(FILE *)httpFile);
//    fclose((FILE *)httpFile);
//    httpFile = PARS_NULL;
//    (*a_pdwBuffersize)=dwSize;
//LANDING_ZONE;
//END_LANDING_ZONE;
//}

// not used by XML Parser
///*--- PARSP_DeleteSystemData() -------------------------------------------------------------- PARSP_DeleteSystemData() ---*/
///**
// *  @brief   Deletes a file in the system storage
// *
// *  @param   a_strStorage      [  in ]    Name of the storage
// *  @param   a_strFileName     [  in ]    Name of the file
// *
// *  @return  PARS_LPBYTE   Beginning of matching memory Block
// */
///*--- PARSP_DeleteSystemData() -------------------------------------------------------------- PARSP_DeleteSystemData() ---*/
//PARS_RESULT     DLLCALL PARSP_DeleteSystemData          ( /*  in */ PARS_LPSTR        a_strStorage,
//                                                          /*  in */ PARS_LPSTR        a_strFileName )
//{
//    PARS_LPSTR      strSystemRoot           = PARS_NULL;
//    PARS_CHAR       strLocalPath[PARS_MAX_URL_LEN];
//    PARS_DWORD      nPathLen                = 0;
//    PARS_LONG       loReturn    = -1;
//    PARS_WCHAR      wstrFileSpec[PARS_FILES_MAX_PATH_LENGTH];
//RETURN_INIT(MWE_FAC_DEFAPP , "PARSP_DeleteSystemData");
//    MCB->GetSystemRoot(&strSystemRoot);
//    /* ignore leading '/' or '\\' */
//    if ( a_strFileName[0]=='/')
//    {
//        a_strFileName++;    /* Over read leading '/, \' */
//    }
//
//    nPathLen = PARS_strlen( strSystemRoot);
//    nPathLen += a_strStorage?PARS_strlen(a_strStorage)+1:0;  /* +1 = '/' */
//    nPathLen += PARS_strlen(a_strFileName) + 2;                        /* +2 = '/' and '\0' */
//
//    PARS_strlcpy( strLocalPath, strSystemRoot, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//    if (a_strStorage)
//    {
//        PARS_strlcat(strLocalPath, a_strStorage, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//        PARS_strlcat(strLocalPath, "/", PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//
//    }
//    PARS_strlcat(strLocalPath, a_strFileName, PARS_MAX_URL_LEN, PARS_MAX_URL_LEN);
//    loReturn = PARSP_MapUTF8ToUnicode(strLocalPath, wstrFileSpec);
//    if (loReturn==-1)
//    {
//        RETURN( MWE_UNEXPECTED);
//    }
//
//    loReturn = PARS_wrmdir( (const wchar_t *) wstrFileSpec);
//    if (loReturn==-1)
//    {
//        loReturn = PARS_wremove( (const wchar_t *) wstrFileSpec);
//    }
//    if (loReturn==-1)
//    {
//        RETURN( MWE_UNEXPECTED);
//    }
//    RETURN ( MWE_NO_ERROR );
//LANDING_ZONE;
//END_LANDING_ZONE;
//}

/*--- PARSP_MemSearch() ---------------------------------------------------------------------------- PARSP_MemSearch() ---*/
/**
 *  @brief   Searches for a memory block in another memory block.
 *           client.
 *
 *  @param   a_strMemoryBlock      [  in ]    Memory Block to search in
 *  @param   a_strPattern          [  in ]    Memory (Sub-) Block to search for
 *  @param   a_dwMemoryBlockLen    [  in ]    Size of Memory Block to search in
 *  @param   a_dwPatternLen        [  in ]    Size of Memory Block to search for
 *
 *  @return  PARS_LPBYTE   Beginning of matching memory Block
 */
/*--- PARSP_MemSearch() ---------------------------------------------------------------------------- PARSP_MemSearch() ---*/
PARS_LPBYTE PARSP_MemSearch( /*  in */  PARS_LPBYTE    a_strMemoryBlock,
                             /*  in */  PARS_LPBYTE    a_strPattern,
                             /*  in */  PARS_DWORD     a_dwMemoryBlockLen,
                             /*  in */  PARS_DWORD     a_dwPatternLen)
{
    PARS_LPBYTE  cp      = (PARS_LPBYTE) a_strMemoryBlock;
    PARS_LPBYTE  s1      = PARS_NULL;
    PARS_LPBYTE  s2      = PARS_NULL;
    PARS_LPBYTE  tStr2   = (PARS_LPBYTE) a_strPattern;

    if ( !*tStr2 )
    {
        return((PARS_LPBYTE)a_strMemoryBlock);
    }

    while ((cp<a_dwMemoryBlockLen+a_strMemoryBlock))
    {
        s1 = cp;
        s2 = (PARS_LPBYTE) tStr2;

        while ( (s1<(a_strMemoryBlock+a_dwMemoryBlockLen)) && (s2<(a_strPattern+a_dwPatternLen)) && !(*s1-*s2) )
        {
            s1++; 
            s2++;
        }
                

        if (s2==((a_strPattern+a_dwPatternLen)))
                return(cp);

        cp++;
    };
    return(PARS_NULL);
}

/*--- PARSP_strlcat() -------------------------------------------------------------------------------- PARSP_strlcat() ---*/
/**
 *  @brief   Like strncat but it is safe with '\\0' at the end.
 *           (Analog to PARS_strstr)
 *
 *  @param   a_strDestination       [  in ]    String to add to ('\\0'- terminated)
 *  @param   a_strSource            [  in ]    String to add ('\\0'- terminated)
 *  @param   a_szDestinationLength  [  in ]    Maximum Buffer size of a_strDestination
 *  @param   a_szCopyLength         [  in ]    Maximum chars to copy, when no '\\0' is found in a_strSource
 *
 *  @return  PARS_DWORD length of the needed space
 */
/*--- PARSP_strlcat() -------------------------------------------------------------------------------- PARSP_strlcat() ---*/
PARS_DWORD  PARSP_strlcat ( /*  bi */ PARS_LPSTR         a_strDestination,
                            /*  in */ PARS_LPCSTR        a_strSource,
                            /*  in */ PARS_DWORD         a_szDestinationLength,
                            /*  in */ PARS_DWORD         a_szCopyLength)
{
    PARS_size_t szLength = PARS_strlen(a_strDestination);

    if(szLength>a_szDestinationLength)
    {
        return szLength;
    }
    if(szLength+a_szCopyLength>=a_szDestinationLength)
    {
        a_szCopyLength = a_szDestinationLength-szLength;
    }
    return szLength + PARS_strlcpy( a_strDestination + szLength,
                                    a_strSource,
                                    a_szDestinationLength-szLength,
                                    a_szCopyLength);
}

/*--- PARSP_strlcpy() -------------------------------------------------------------------------------- PARSP_strlcpy() ---*/
/**
 *  @brief   Like strncpy but it is safe with '\\0' at the end.
 *           (Analog to PARS_strstr)
 *
 *  @param   a_strDestination       [  in ]    String to add to ('\\0'- terminated)
 *  @param   a_strSource            [  in ]    String to add ('\\0'- terminated)
 *  @param   a_szDestinationLength  [  in ]    Maximum Buffer size of a_strDestination
 *  @param   a_szCopyLength         [  in ]    Maximum chars to copy, when no '\\0' is found in a_strSource
 *
 *  @return  PARS_DWORD amount of chars copied if a_szDestinationLength is sufficient or
 *           a_szDestinationLength + amount of chars not copied when destiantion buffer is too small.
 */
/*--- PARSP_strlcpy() -------------------------------------------------------------------------------- PARSP_strlcpy() ---*/
PARS_DWORD  PARSP_strlcpy ( /*  bi */ PARS_LPSTR         a_strDestination,
                            /*  in */ PARS_LPCSTR        a_strSource,
                            /*  in */ PARS_DWORD         a_szDestinationLength,
                            /*  in */ PARS_DWORD         a_szCopyLength)
{
    PARS_DWORD  szIndex     = 0;
    PARS_LPSTR  pchCursor   = PARS_NULL;

    pchCursor = a_strDestination;
    for ( szIndex = 0;
          szIndex < a_szCopyLength && szIndex < a_szDestinationLength && (*a_strSource) != '\0';
          ++pchCursor, ++a_strSource, ++szIndex)
    {
        *pchCursor = *a_strSource;
    }
    if(szIndex < a_szDestinationLength)
    {
        *pchCursor = '\0';
    }
    else
    {
        a_strDestination[a_szDestinationLength-1]='\0';/* Overwrite a_strRight char, if we stuck to the end of the buffer */
    }
    if( (szIndex == a_szCopyLength) ||
        (*a_strSource == '\0')      )
    {
        return szIndex; /* a_nCount of the copied chars */
    }
    else
    {
        /* a_szDestinationLength + remaining chars, that are NOT copied */
        return a_szDestinationLength + PARS_strnlen (a_strSource, (a_szCopyLength-szIndex));
    }
}

/*--- PARSP_strnstr() -------------------------------------------------------------------------------- PARSP_strnstr() ---*/
/**
 *  @brief   Searches a substring in a a_strToCheck with the given length
 *
 *  @param   a_strToCheck       [  in ]    String within the substring is to search
 *  @param   a_strSubString     [  in ]    The substring to search for ('\\0'- terminated)
 *  @param   a_dwMaxLen         [  in ]    Length of the String
 *
 *  @return  PARS_LPSTR position of the a_strLeft found, or PARS_NULL if not found.
 */
/*--- PARSP_strnstr() -------------------------------------------------------------------------------- PARSP_strnstr() ---*/
PARS_LPSTR PARSP_strnstr( /* out */ PARS_LPCSTR a_strToCheck,
                          /*  in */ PARS_LPCSTR a_strSubString,
                          /*  in */ PARS_DWORD  a_dwMaxLen)

{
    PARS_LPSTR  cp      = (PARS_LPSTR) a_strToCheck;
    PARS_LPSTR  s1      = PARS_NULL;
    PARS_LPSTR  s2      = PARS_NULL;
    PARS_LPSTR  tStr2   = (PARS_LPSTR) a_strSubString;

    if ( !*tStr2 )
    {
        return((PARS_LPSTR)a_strToCheck);
    }

    while (*cp)
    {
        s1 = cp;
        s2 = (PARS_LPSTR) tStr2;

        while ( *s1 && *s2 && !(*s1-*s2) )
        {
            s1++, s2++;
            if ((PARS_DWORD)((s1-(PARS_LPSTR) a_strToCheck))>a_dwMaxLen)
            {
                return (PARS_NULL); /* Max Length reached! */
            }
        }

        if (!*s2)
                return(cp);

        cp++;
        if ((PARS_DWORD)((cp-(PARS_LPSTR) a_strToCheck))>a_dwMaxLen)
        {
            return (PARS_NULL); /* Max Length reached! */
        }
    };
    return(PARS_NULL);
}

// not used by XML parser
///*--- PARSP_strinstr() ------------------------------------------------------------------------------ PARSP_strinstr() ---*/
///**
// *  @brief   Searches for the a_strLeft occurrence of a_strSubString within a_strToCheck. This
// *           is like the a_strString.h PARS_strnstr() function,
// *           but this one does its job Case-Insensitive!
// *
// *  @param   a_strToCheck       [  in ]    String within the substring is to search
// *  @param   a_strSubString     [  in ]    The substring to search for ('\\0'- terminated)
// *  @param   a_dwMaxLen         [  in ]    Length of the String
// *
// *  @return  PARS_LPSTR position of the a_strLeft found, or PARS_NULL if not found.
// */
///*--- PARSP_strinstr() ------------------------------------------------------------------------------ PARSP_strinstr() ---*/
//PARS_LPSTR PARSP_strinstr( /* out */  PARS_LPCSTR   a_strToCheck,
//                           /*  in */  PARS_LPCSTR   a_strSubString,
//                           /*  in */  PARS_DWORD    a_dwMaxLen)
//{
//    PARS_LPSTR  cp      = (PARS_LPSTR) a_strToCheck;
//    PARS_LPSTR  s1      = PARS_NULL;
//    PARS_LPSTR  s2      = PARS_NULL;
//    PARS_LPSTR  tStr2   = (PARS_LPSTR) a_strSubString;
//
//    if ( !*tStr2 )
//    {
//        return((PARS_LPSTR)a_strToCheck);
//    }
//
//    while (*cp)
//    {
//        s1 = cp;
//        s2 = (PARS_LPSTR) tStr2;
//
//        while ( (*s1) && (*s2) && !(PARS_toupper(*s1)-PARS_toupper(*s2)) )/* Map to Uppercase */
//        {
//            s1++, s2++;
//            if ((PARS_DWORD)((s1-(PARS_LPSTR) a_strToCheck))>a_dwMaxLen)
//            {
//                return (PARS_NULL); /* Max Length reached! */
//            }
//        }
//
//        if (!*s2)
//                return(cp);
//
//        cp++;
//        if ((PARS_DWORD)((cp-(PARS_LPSTR) a_strToCheck))>a_dwMaxLen)
//        {
//            return (PARS_NULL); /* Max Length reached! */
//        }
//    };
//    return(PARS_NULL);
//}
//
///*--- PARSP_stristr() -------------------------------------------------------------------------------- PARSP_stristr() ---*/
///**
// *  @brief   Searches for the a_strLeft occurrence of a_strSubString within a_strToCheck. This
// *           is like the a_strString.h PARS_strstr() function,
// *           but this one does its job Case-Insensitive!
// *
// *  @param   a_strToCheck       [  in ]    String within the substring is to search
// *  @param   a_strSubString     [  in ]    The substring to search for ('\\0'- terminated)
// *
// *  @return  PARS_LPSTR position of the a_strLeft found, or PARS_NULL if not found.
// */
///*--- PARSP_stristr() -------------------------------------------------------------------------------- PARSP_stristr() ---*/
//PARS_LPSTR PARSP_stristr( /* out */  PARS_LPCSTR   a_strToCheck,
//                          /*  in */  PARS_LPCSTR   a_strSubString)
//{
//    PARS_LPSTR  cp      = (PARS_LPSTR) a_strToCheck;
//    PARS_LPSTR  s1      = PARS_NULL;
//    PARS_LPSTR  s2      = PARS_NULL;
//    PARS_LPSTR  tStr2   = (PARS_LPSTR) a_strSubString;
//
//    if ( !*tStr2 )
//    {
//        return((PARS_LPSTR)a_strToCheck);
//    }
//
//    while (*cp)
//    {
//        s1 = cp;
//        s2 = (PARS_LPSTR) tStr2;
//
//        while ( (*s1) && (*s2) && !(PARS_toupper(*s1)-PARS_toupper(*s2)) )/* Map to Uppercase */
//                s1++, s2++;
//
//        if (!*s2)
//                return(cp);
//
//        cp++;
//    };
//    return(PARS_NULL);
//}

/*--- PARSP_strnchr() -------------------------------------------------------------------------------- PARSP_strnchr() ---*/
/**
 *  @brief   Searches for the a_strLeft occurrence of a_chCharacterToFind within a_strToCheck.
 *           This is like the a_strToCheck.h strchr() function but with Max Length.
 *
 *  @param   a_strToCheck           [  in ]    String within the substring is to search
 *  @param   a_chCharacterToFind    [  in ]    Character to search for
 *  @param   a_dwMaxLen             [  in ]    Length of the String
 *
 *  @return  PARS_LPSTR position of the a_strToCheck found, or PARS_NULL if not found.
 */
/*--- PARSP_strnchr() -------------------------------------------------------------------------------- PARSP_strnchr() ---*/
PARS_LPSTR PARSP_strnchr ( /*  in */ PARS_LPCSTR a_strToCheck,
                           /*  in */ PARS_INT    a_chCharacterToFind,
                           /*  in */ PARS_DWORD  a_dwMaxLen)
{
    PARS_LPSTR tString  = (PARS_LPSTR) a_strToCheck;

    if( a_dwMaxLen==PARS_ZERO_TERMINATED_STRING )
    {
        a_dwMaxLen = PARS_strlen(a_strToCheck);
    }
    while (((PARS_DWORD)(tString-a_strToCheck)<a_dwMaxLen)
        &&(*tString && *tString != (PARS_CHAR)a_chCharacterToFind))
            tString++;

    if (((PARS_DWORD)(tString-a_strToCheck)<a_dwMaxLen)
        &&(*tString == (PARS_CHAR)a_chCharacterToFind))
    {
            return((PARS_LPSTR)tString);
    }
    return(PARS_NULL);
}

/*--- PARSP_StrToDbl() ------------------------------------------------------------------------------ PARSP_StrToDbl() ---*/
/**
 *  @brief   Replacement for atof (with better checks)
 *
 *  @param   a_strIn        [  in ]    String to parse and convert to double.
 *  @param   a_dwInLen      [  in ]    Length of the String to parse, or 0 to ignore the length
 *  @param   a_pdblOut      [ out ]    The Double Value.
 *  @param   a_pdwparsedLen [ out ]    How many chars are parsed from the input a_strString
 *
 *  @return  PARS_RESULT:
 *                          MWE_NO_ERROR:   succeeded
 *                          MWE_RANGE:      *a_pdblOut!=0.0 for Overflow; *a_pdblOut=0.0 for Underflow
 *                          MWE_NOT_FOUND:  String does not contain a number
 *                          MWS_FALSE:      not all of a_strIn is processed
 */
/*--- PARSP_StrToDbl() ------------------------------------------------------------------------------ PARSP_StrToDbl() ---*/
PARS_RESULT PARSP_StrToDbl( /*  in */ PARS_LPSTR      a_strIn,
                            /*  in */ PARS_DWORD      a_dwInLen, /* can be 0 to ignore */
                            /* out */ PARS_LPDOUBLE   a_pdblOut,
                            /* out */ PARS_LPDWORD    a_pdwparsedLen)
{
    PARS_INT    nLocalErrno     = 0;
    PARS_LPSTR  strEndPointer   = PARS_NULL;
    PARS_CHAR   chSave          = '\0';
    if (a_strIn==PARS_NULL) return MWE_INVALID_PARAMETER;
    if (a_pdblOut==PARS_NULL) return MWE_INVALID_PARAMETER;

    if(a_dwInLen==PARS_ZERO_TERMINATED_STRING)
    {
        a_dwInLen = PARS_strlen(a_strIn);
    }
    chSave = a_strIn[a_dwInLen];
    a_strIn[a_dwInLen] = '\0';

    errno = 0;
    *a_pdblOut = strtod(a_strIn, &strEndPointer);
    a_strIn[a_dwInLen] = chSave;
    nLocalErrno = errno;
    if(a_pdwparsedLen!=PARS_NULL)
    {
        *a_pdwparsedLen = 0;
    }
    if (nLocalErrno == ERANGE)
    {
        return MWE_RANGE;
    }
    if(a_pdwparsedLen!=PARS_NULL)
    {
        *a_pdwparsedLen = (PARS_DWORD) (strEndPointer - a_strIn);
    }
    if(strEndPointer - a_strIn == 0)
    {
        return MWE_NOT_FOUND;
    }
    if( ( strEndPointer != PARS_NULL) &&
        ( ((PARS_DWORD)(strEndPointer-a_strIn) != a_dwInLen) &&
          (*strEndPointer != '\0' )
        )
      )
    {
        return  MWS_FALSE;
    }
    return MWE_NO_ERROR;
}

/*--- UTF8 functions                                                                                    UTF8 functions ---*/

/*----------------------------------------------------------------------------------------------------*\
|   Character Conversions                                                      Character Conversions   |
\*----------------------------------------------------------------------------------------------------*/
static const PARS_BYTE    PARSP_g_abyUTF8Mask[6] = { 0x00,           /* Mask for 1 Byte UTF 0x0xxxxxxx */
                                                     0xC0,           /* Mask for 2 Byte UTF 0x110xxxxx */
                                                     0xE0,           /* Mask for 3 Byte UTF 0x1110xxxx */
                                                     0xF0,           /* Mask for 4 Byte UTF 0x11110xxx */
                                                     0xF8,           /* Mask for 5 Byte UTF 0x111110xx */
                                                     0xFC            /* Mask for 6 Byte UTF 0x1111110x */
                                                   };

/*--- PARSP_WriteUTF8Char() -------------------------------------------------------------------- PARSP_WriteUTF8Char() ---*/
/**
 *  @brief   Converts the given unicode char to a UTF8 char
 *
 *           only internally used by PARSP_MapUTF8ToUnicode
 *
 *  @param   a_wstrUniCodeText      [ out ]    Position to write the character to
 *  @param   a_strUtf8Text          [  in ]    Position of the character to convert
 *
 *  @return  PARS_INT:      Amount of converted characters.
 */
/*--- PARSP_WriteUTF8Char() -------------------------------------------------------------------- PARSP_WriteUTF8Char() ---*/
static PARS_INT PARSP_WriteUTF8Char( /*  in */ PARS_LPSTR*         a_strUtf8Text,
                                     /* out */ PARS_LPWSTR*        a_wstrUniCodeText)
{
    PARS_DWORD   dwOctetCount            = 0;                  /* How many Octets belongs to the char */
    PARS_LPSTR   strCursor               = 0;
    PARS_WCHAR   wActualChar             = 0;
    PARS_DWORD   dwResultAscii           = 0;
    PARS_BYTE    byMask                  = 0;
    PARS_DWORD   dwIndex                 = 0;

    strCursor = *a_strUtf8Text;
    wActualChar = (PARS_BYTE) (strCursor[0]);
    strCursor++;
    /* Chars below 128 have no mapping and are single UTF-8 Octets */
    if ((wActualChar&0x80)==0)
    {
        /* its ascii below 128 */
        (**a_wstrUniCodeText) = (PARS_WCHAR) wActualChar;
        (*a_wstrUniCodeText)++; /* set to the next char, and optimize search for the end of string */
        (*a_strUtf8Text)++; /* eaten up one char from utf8 source */
        return 0;
    }
    if( (wActualChar == 0xC0) || (wActualChar == 0xC1) )
    {
        /* these are ILLEGAL, see http://de.wikipedia.org/wiki/Utf8!! */
        (**a_wstrUniCodeText) = 0;
        return -1;
    }
    /* First step retrieve raw utf8 Data */
    dwOctetCount = 7;  /* Maximum BYTES of UTF Char is 6, we need 1 more, cause we make a -1 in the next step. */
    do
    {
        dwOctetCount--;
        byMask = (PARS_BYTE) ((0xff >> (8-dwOctetCount)) << (8-dwOctetCount));
    } while ((dwOctetCount>1) && ((wActualChar & byMask) != PARSP_g_abyUTF8Mask[dwOctetCount-1]));
    if (dwOctetCount==1)
    {
        return -1;
    }
    /* Building AscII Char */
    /* First UTF need a special treatment */
    /* Mask out the char bits from the a_strLeft UTF char */
    dwResultAscii = dwResultAscii | ( (wActualChar&~PARSP_g_abyUTF8Mask[dwOctetCount-1]) << ((dwOctetCount-1)*6));
    for (dwIndex=1;dwIndex<dwOctetCount;dwIndex++)
    {
        wActualChar = (PARS_BYTE) (strCursor[0]);
        strCursor++;
        if ((wActualChar&0xC0) != 0x80)
        {
            (**a_wstrUniCodeText) = 0;
            return -1;
        }
        dwResultAscii = dwResultAscii | ((wActualChar&0x3F)<<((dwOctetCount-1-dwIndex)*6));
    }
    if(dwResultAscii > 0xFFFF)
    {
        /* em3702: FIX: if the result would be truncated, return an error */
        (**a_wstrUniCodeText) = 0;
        return -1;
    }
    (*a_wstrUniCodeText)[0] = (PARS_WCHAR) (dwResultAscii&0xFFFF);
    (*a_wstrUniCodeText)++; /* set to the next char, and optimize search for the end of string */

    *a_strUtf8Text += dwOctetCount; /* set the input Buffer to the next char */
    return 0;
}

/*--- PARSP_WriteUnicodeChar() -------------------------------------------------------------- PARSP_WriteUnicodeChar() ---*/
/**
 *  @brief   Converts the given UTF8 Char to a unicode char
 *
 *           only internally used by PARSP_MapUnicodeToUTF8
 *
 *  @param   a_wstrUniCodeText      [ out ]    Position to write the character to
 *  @param   a_strUtf8Text          [  in ]    Position of the character to convert
 *
 *  @return  PARS_INT:      Amount of converted characters.
 */
/*--- PARSP_WriteUnicodeChar() -------------------------------------------------------------- PARSP_WriteUnicodeChar() ---*/
//static PARS_INT PARSP_WriteUnicodeChar( /* out */ PARS_LPWSTR*        a_wstrUniCodeText,
//                                        /*  in */ PARS_LPSTR*         a_strUtf8Text )
//{
//    PARS_BYTE    byUTF8Octets   = 0;                     /* How many octets the UTF char has */
//    PARS_BYTE    wActualChar    = 0;                 /* Helper for building the actual octet */
//    PARS_CHAR    strUTFChar[7]  = "";/* the UTF 8 char (Max 6 PARS_BYTE), which is to build, */
//                                     /* with trailing '\0'                                   */
//    PARS_LPSTR   strCursor      = 0;   /* Helper Cursor walks through the generated UTF char */
//    PARS_DWORD   dwAscii        = 0;
//
//    dwAscii = *(a_wstrUniCodeText)[0];
//
//    /* Chars below 128 have no mapping and are single UTF-8 Octets */
//    if (dwAscii<128)
//    {
//        wActualChar = (PARS_BYTE) (dwAscii & 0xff);
//
//        (*a_strUtf8Text)[0] = (PARS_CHAR) wActualChar;
//        (*a_strUtf8Text)[1] = 0;
//        (*a_strUtf8Text)++; /* set to the next char, and optimize search for the end of string */
//        (*a_wstrUniCodeText)++;
//        return (0);  /* finished converting the actual char, no mapping below ASCII 128 */
//    }
//
//    /* Fill Up Utf  Char */
//    strCursor = &strUTFChar[6]; /* Map to a_strRight position, UTF char has a maximum of 6 Bytes */
//    *strCursor = '\0';
//    byUTF8Octets = 0;
//    do
//    {
//        strCursor--;                                                    /* go to next free octet */
//        *strCursor = (PARS_CHAR) ((dwAscii>>(byUTF8Octets*6))&0x3F);  /* Mask out the actual 6 Bits */
//        *strCursor = (PARS_CHAR) (*strCursor | 0x80);                   /* Add UTF Mark */
//        dwAscii = dwAscii & (~(0x3F<<(byUTF8Octets*6)));            /* zero the processed bits */
//        byUTF8Octets++;                                                 /* Count used Octets */
//    } while (dwAscii!=0);
//    /* Were ready, when all bits are processed */
//    *strCursor = (PARS_CHAR)  (*strCursor | PARSP_g_abyUTF8Mask[byUTF8Octets-1]);
//    (*a_wstrUniCodeText)++;
//
//    strcpy_s(*a_strUtf8Text, 6, strCursor);   /* Write the existing UTF char */
//    (*a_strUtf8Text) += byUTF8Octets;    /* set to the next char, and optimize search for the end of string */
//    return 0;
//}

/*--- PARSP_MapUnicodeToUTF8() -------------------------------------------------------------- PARSP_MapUnicodeToUTF8() ---*/
/**
 *  @brief   Converts the given Unicode a_strString to a utf8 a_strString
 *
 *  @param   a_wstrUnicodeText  [  in ]    Text a_strString to be converted.
 *  @param   a_strUTF8CodeText  [ out ]    Converted text a_strString.
 *
 *  @return  PARS_INT:      Amount of converted characters.
 */
/*--- PARSP_MapUnicodeToUTF8() -------------------------------------------------------------- PARSP_MapUnicodeToUTF8() ---*/
//PARS_INT PARSP_MapUnicodeToUTF8( /*  in */ PARS_LPWSTR          a_wstrUnicodeText,
//                                 /* out */ PARS_LPSTR           a_strUTF8CodeText)
//{
//    PARS_INT    iRetVal = 0;
//    *a_strUTF8CodeText='\0'; /* RQ Gunnar 18.11.05 */
//
//    do
//    {
//        iRetVal =  PARSP_WriteUnicodeChar(&a_wstrUnicodeText, &a_strUTF8CodeText);
//        if (0 != iRetVal)
//        {
//            return iRetVal;
//        }
//    } while ((*(a_wstrUnicodeText-1))!=0);
//    return 0;
//}

/*--- PARSP_MapUTF8ToUnicode() -------------------------------------------------------------- PARSP_MapUTF8ToUnicode() ---*/
/**
 *  @brief   Converts the given UTF8 a_strString to a Unicode a_strString
 *
 *  @param   a_strUtf8Text      [  in ]    Text a_strString to be converted.
 *  @param   a_wstrUniCodeText  [ out ]    Converted text a_strString.
 *
 *  @return  PARS_INT:      Amount of converted characters.
 */
/*--- PARSP_MapUTF8ToUnicode() -------------------------------------------------------------- PARSP_MapUTF8ToUnicode() ---*/
PARS_INT PARSP_MapUTF8ToUnicode( /*  in */ PARS_LPSTR          a_strUtf8Text,
                                 /* out */ PARS_LPWSTR         a_wstrUniCodeText)
{
    PARS_INT    iRetVal  = 0;
    do
    {
        iRetVal =  PARSP_WriteUTF8Char( &a_strUtf8Text, &a_wstrUniCodeText);
        if (0 != iRetVal)
        {
            return iRetVal;
        }
    } while ((*(a_strUtf8Text-1))!='\0');
    return 0;
}

/*--- Several helper functions -------------------------------------------------------------- Several helper functions ---*/

/*--- PARSP_Get_HTTP_Code() -------------------------------------------------------------------- PARSP_Get_HTTP_Code() ---*/
/**
 *  @brief   Extracts the HTTP error code from the given xml parser error code.
 *
 *           (Only the Code, not the sub code will be extracted)
 *
 *  @param   a_dwErrorCode      [  in ]    The xml parser error code
 *
 *  @return  PARS_INT:      The corresponding HTTP Error code
 */
/*--- PARSP_Get_HTTP_Code() -------------------------------------------------------------------- PARSP_Get_HTTP_Code() ---*/
PARS_INT    PARSP_Get_HTTP_Code( /*  in */ PARS_DWORD a_dwErrorCode)
{
    PARS_WORD  wTemp  = (PARS_WORD) ( (a_dwErrorCode & 0x3fff0000l)>>16 );
    if(wTemp==0)
    {
        return 0;
    }
    if (MWE_SUCCEEDED(a_dwErrorCode))
    {
        return ((wTemp - 0x101)/0x100)+100;
    }
    else
    {
        return ((wTemp - 0x101)/0x30)+300;
    }
}

/*--- PARSP_ForceThreadRotation() -------------------------------------------------------- PARSP_ForceThreadRotation() ---*/
/**
 *  @brief   Forces a thread change, will be called during long lasting operation to prevent
 *           other threads from starving in cooperative multitasking systems.
 *
 *  @return  PARS_VOID
 */
/*--- PARSP_ForceThreadRotation() -------------------------------------------------------- PARSP_ForceThreadRotation() ---*/
PARS_VOID   PARSP_ForceThreadRotation( PARS_VOID )
{
}

/*--- PARSP_DebugBreak() -------------------------------------------------------------------------- PARSP_DebugBreak() ---*/
/**
 *  @brief   Forces the CPU to stop in debug mode: Inserts a Hard breakpoint when _DEBUG is set.
 *           Will be used by the ASSERT macros.
 *           Break only if Tracelevel is PARS_DEBUG_LEVEL_WARNING or more.
 *
 *  @param   a_dwTraceLevel      [  in ]    Tracelevel to decide if we have to break or not
 *
 *  @return  PARS_VOID
 */
/*--- PARSP_DebugBreak() -------------------------------------------------------------------------- PARSP_DebugBreak() ---*/
PARS_VOID PARSP_DebugBreak( /*  in */ PARS_DWORD    a_dwTraceLevel )
{
#ifdef _DEBUG
    if( a_dwTraceLevel <= PARS_DEBUG_LEVEL_WARNING )
    {
/*        __asm int 3; */
    }
#else /* _DEBUG */
    if( a_dwTraceLevel <= PARS_DEBUG_LEVEL_WARNING )
    {
        PARS_PORT_TRACE(PARS_DEBUG_LEVEL_FATAL_ERROR, "################ DebugBreak hit! ################\n");
    }
#endif
}

/*--- ---------------------------------------------------------------------------------------------------------------- ---*\
|   Own implementation of CRT functions                                            Own implementation of CRT functions     |
\*--- ---------------------------------------------------------------------------------------------------------------- ---*/

/*--- memory functions from C-runtime ------------------------------------------------ memory functions from C-runtime ---*/

#if PARS_OWN_MEMSET
/*--- PARSP_memset() ---------------------------------------------------------------------------------- PARSP_memset() ---*/
/**
 *  @brief   Initializes the given BYTE buffer with the given initial value.
 *
 *  @param   a_pvDst            [ out ]    Buffer which is to be initialized
 *  @param   a_nValue           [  in ]    Initialization value.
 *  @param   a_nCount           [  in ]    Count of bytes to initialize.
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_memset() ---------------------------------------------------------------------------------- PARSP_memset() ---*/
PARS_LPVOID PARSP_memset( /* out */ PARS_LPVOID  a_pvDst,
                          /*  in */ PARS_INT     a_nValue,
                          /*  in */ PARS_size_t  a_nCount)
{
    PARS_VOID *start = a_pvDst;

    while (a_nCount--) {
            *(PARS_LPSTR)a_pvDst = (PARS_CHAR)a_nValue;
            a_pvDst = (PARS_LPSTR) a_pvDst + 1;
    }

    return(start);
}
#endif /* PARS_OWN_MEMSET */

#if PARS_OWN_MEMCPY
/*--- PARSP_memcpy() ---------------------------------------------------------------------------------- PARSP_memcpy() ---*/
/**
 *  @brief   Copies the source buffer to the destination buffer both with the size of a_nCount bytes.
 *
 *  @param   a_pvDst            [ out ]    Destination buffer
 *  @param   a_pvSrc            [  in ]    Source buffer.
 *  @param   a_nCount           [  in ]    Count of bytes to be copied.
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_memcpy() ---------------------------------------------------------------------------------- PARSP_memcpy() ---*/
PARS_LPVOID PARSP_memcpy( /* out */ PARS_LPVOID         a_pvDst,
                          /*  in */ const PARS_LPVOID   a_pvSrc,
                          /*  in */ PARS_size_t         a_nCount)
{
    PARS_LPVOID ret = a_pvDst;
    PARS_LPVOID tSrc = a_pvSrc;
    /*
     * copy from lower addresses to higher addresses
     */
    while (a_nCount--) {
            *(PARS_LPSTR)a_pvDst = *(PARS_LPSTR)tSrc;
            a_pvDst = (PARS_LPSTR)a_pvDst + 1;
            tSrc = (PARS_LPSTR)tSrc + 1;
    }
    return(ret);
}
#endif /* PARS_OWN_MEMCPY */

#if PARS_OWN_MEMMOVE
/*--- PARSP_memmove() -------------------------------------------------------------------------------- PARSP_memmove() ---*/
/**
 *  @brief   Copies the source buffer to the destination buffer both with the size of a_nCount bytes. Use this method
 *           instead of memcpy when a_pvDst and a_pvSrc are overlapping (memcpy has undefined behavior then).
 *
 *  @param   a_pvDst            [ out ]    Destination buffer
 *  @param   a_pvSrc            [  in ]    Source buffer.
 *  @param   a_nCount           [  in ]    Count of bytes to be copied.
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_memmove() -------------------------------------------------------------------------------- PARSP_memmove() ---*/
#error no own implementation for PARSP_memmove
PARS_INT  PARSP_memmove ( /* out */ PARS_LPVOID         a_pvDst,
                          /*  in */ const PARS_LPVOID   a_pvSrc,
                          /*  in */ PARS_size_t         a_nCount)
{
    return 0;
}
#endif /* PARS_OWN_MEMMOVE */

#if PARS_OWN_MEMCMP
/*--- PARSP_memcmp() ---------------------------------------------------------------------------------- PARSP_memcmp() ---*/
/**
 *  @brief   Compares the two buffers byte by byte, a_nCount bytes long
 *
 *  @param   a_pvDst            [ out ]    Destination buffer
 *  @param   a_pvSrc            [  in ]    Source buffer.
 *  @param   a_nCount           [  in ]    Count of bytes to be copied.
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_memcmp() ---------------------------------------------------------------------------------- PARSP_memcmp() ---*/
PARS_INT PARSP_memcmp( /* out */ const PARS_VOID*    a_pvDst,
                       /*  in */ const PARS_VOID*    a_pvSrc,
                       /*  in */ PARS_size_t         a_nCount)
{
    if (!a_nCount)
            return(0);
    while ( --a_nCount && *(char *)a_pvDst == *(char *)a_pvSrc )
    {
        a_pvDst = (char *)a_pvDst + 1;
        a_pvSrc = (char *)a_pvSrc + 1;
    }
    return( *((unsigned char *)a_pvDst) - *((unsigned char *)a_pvSrc) );
}
#endif /* PARS_OWN_MEMCMP */

/*--- String functions from C-runtime ------------------------------------------------ String functions from C-runtime ---*/

#if PARS_OWN_STRNLEN
/*--- PARSP_strnlen() -------------------------------------------------------------------------------- PARSP_strnlen() ---*/
/**
 *  @brief   Gives the length of a C-String also checking the a_dwMaxLen
 *
 *  @param   a_strToCheck       [  in ]    String which length is to calculate
 *  @param   a_dwMaxLen         [  in ]    Maximum length of the a_strString
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_strnlen() -------------------------------------------------------------------------------- PARSP_strnlen() ---*/
PARS_DWORD PARSP_strnlen( /*  in */   PARS_LPCSTR     a_strToCheck,
                          /*  in */   PARS_DWORD      a_dwMaxLen)
{
    PARS_DWORD      dwIndex = 0;
    if (PARS_NULL == a_strToCheck)
    {
        return 0;
    }

    for (dwIndex = 0; dwIndex < a_dwMaxLen && *a_strToCheck; dwIndex++, a_strToCheck++)
        ;
    return dwIndex;
}
#endif /* PARS_OWN_STRNLEN */

#if PARS_OWN_STRLEN
/*--- PARSP_strnlen() -------------------------------------------------------------------------------- PARSP_strnlen() ---*/
/**
 *  @brief   Gives the length of a C-String
 *
 *  @param   a_strToCheck       [  in ]    String which length is to calculate
 *
 *  @return  PARS_LPVOID:       a_pvDst
 */
/*--- PARSP_strnlen() -------------------------------------------------------------------------------- PARSP_strnlen() ---*/
PARS_DWORD PARSP_strlen (/*  in */ PARS_LPCSTR  a_strToCheck)
{
    PARS_LPSTR    eos = (PARS_LPSTR) a_strToCheck;
    while( *eos++ ) ;
    return ( (PARS_size_t)(eos - a_strToCheck - 1) );
}
#endif /* PARS_OWN_STRLEN */

#if PARS_OWN_STRSTR
/*--- PARSP_strstr() ---------------------------------------------------------------------------------- PARSP_strstr() ---*/
/**
 *  @brief   Searches for the substring within the given check a_strString, returns pointer to the
 *
 *           a_strLeft occurrence of the sub a_strString, or PARS_NULL if substring is not part of the check a_strString.
 *
 *  @param   a_strToCheck       [  in ]    String in which is to search
 *  @param   a_strSubString     [  in ]    The a_strString which is to search for
 *
 *  @return  PARS_LPSTR:       a_strLeft occurrence of the sub a_strString or PARS_NMULL if not found.
 */
/*--- PARSP_strstr() ---------------------------------------------------------------------------------- PARSP_strstr() ---*/
PARS_LPSTR PARSP_strstr( /*  out */ PARS_LPCSTR a_strToCheck,
                         /*   in */ PARS_LPCSTR a_strSubString)

{
    PARS_LPSTR  cp      = (PARS_LPSTR) a_strToCheck;
    PARS_LPSTR  s1      = PARS_NULL;
    PARS_LPSTR  s2      = PARS_NULL;
    PARS_LPSTR  tStr2   = (PARS_LPSTR) a_strSubString;

    if ( !*tStr2 )
    {
        return((PARS_LPSTR)a_strToCheck);
    }

    while (*cp)
    {
        s1 = cp;
        s2 = (PARS_LPSTR) tStr2;

        while ( *s1 && *s2 && !(*s1-*s2) )
        {
            s1++; 
            s2++;
        }
                

        if (!*s2)
                return(cp);

        cp++;
    };
    return(PARS_NULL);
}
#endif /* PARS_OWN_STRSTR */

#if PARS_OWN_STRCAT
/*--- PARSP_strcat() ---------------------------------------------------------------------------------- PARSP_strcat() ---*/
/**
 *  @brief   Adds the second a_strString to the end of the a_strLeft a_strString, without buffer checking
 *
 *  @param   a_strSource    [  bi ]    String to append to
 *  @param   a_strToAdd     [  in ]    String to add
 *
 *  @return  PARS_LPSTR:       a_strSource
 */
/*--- PARSP_strcat() ---------------------------------------------------------------------------------- PARSP_strcat() ---*/
PARS_LPSTR  PARSP_strcat( /*  in */ PARS_LPSTR          a_strSource,
                          /*  in */ PARS_LPCSTR         a_strToAdd)
{
    char * cp = a_strSource;

    while( *cp )
            cp++;                   /* find end of a_strRight */

    while( *cp++ = *a_strToAdd++ ) ;       /* Copy a_strLeft to end of a_strRight */

    return( a_strSource );                  /* return a_strRight */
}
#endif /* PARS_OWN_STRCAT */

#if PARS_OWN_STRCPY
/*--- PARSP_strcpy() ---------------------------------------------------------------------------------- PARSP_strcpy() ---*/
/**
 *  @brief   Copies the a_strString a_strToAdd into the spot specified by
 *           a_strSource; assumes enough room.
 *
 *  @param   a_strSource    [  bi ]    String to copy to
 *  @param   a_strToAdd     [  in ]    String to copy
 *
 *  @return  PARS_LPSTR:       a_strSource
 */
/*--- PARSP_strcpy() ---------------------------------------------------------------------------------- PARSP_strcpy() ---*/
PARS_LPSTR  PARSP_strcpy( /*  in */ PARS_LPSTR          a_strSource,
                          /*  in */ PARS_LPCSTR         a_strToAdd)
{
    char * cp = a_strSource;

    while( *cp++ = *a_strToAdd++ )
            ;               /* Copy a_strLeft over a_strRight */
    return( a_strSource );
}
#endif /* PARS_OWN_STRCPY */

#if PARS_OWN_STRNICMP
/*--- PARSP_strnicmp() ------------------------------------------------------------------------------ PARSP_strnicmp() ---*/
/**
 *  @brief   Compares two strings for lexical order, ignoring case.  The comparison stops
 *           after: (1) a difference between the strings is found, (2) the end
 *           of the strings is reached, or (3) a_nMaxLen characters have been
 *           compared.
 *
 *  @param   a_strLeft      [  bi ]    String to compare
 *  @param   a_strRight     [  bi ]    String to compare
 *  @param   a_nMaxLen      [  in ]    Maximum length to compare
 *
 *  @return  PARS_INT:       <0 if Left < Right
 *                           0 if Left == Right
 *                           >0 if Left > Right
 */
/*--- PARSP_strnicmp() ------------------------------------------------------------------------------ PARSP_strnicmp() ---*/
PARS_INT PARSP_strnicmp ( /*  in */ PARS_LPCSTR a_strLeft,
                          /*  in */ PARS_LPCSTR a_strRight,
                          /*  in */ PARS_LONG   a_nMaxLen)
{
    PARS_INT f,l;
    PARS_LPSTR  tFirst = (PARS_LPSTR)a_strLeft;
    PARS_LPSTR  tLast = (PARS_LPSTR) a_strRight;

    if ( a_nMaxLen )
    {
        do {
            if ( ((f = (PARS_BYTE)(*(tFirst++))) >= 'A') &&
                 (f <= 'Z') )
                f -= 'A' - 'a';

            if ( ((l = (PARS_BYTE)(*(tLast++))) >= 'A') &&
                 (l <= 'Z') )
                l -= 'A' - 'a';

        } while ( --a_nMaxLen && f && (f == l) );

        return( f - l );
    }

    return( 0 );
}
#endif /* PARS_OWN_STRNICMP */

#if PARS_OWN_STRNCMP
/*--- PARSP_strncmp() -------------------------------------------------------------------------------- PARSP_strncmp() ---*/
/**
 *  @brief   Compares two strings for lexical order.  The comparison stops
 *           after: (1) a difference between the strings is found, (2) the end
 *           of the strings is reached, or (3) a_nMaxLen characters have been
 *           compared.
 *
 *  @param   a_strLeft      [  bi ]    String to compare
 *  @param   a_strRight     [  bi ]    String to compare
 *  @param   a_nMaxLen      [  in ]    Maximum length to compare
 *
 *  @return  PARS_INT:       <0 if Left < Right
 *                           0 if Left == Right
 *                           >0 if Left > Right
 */
/*--- PARSP_strncmp() -------------------------------------------------------------------------------- PARSP_strncmp() ---*/
PARS_INT  PARSP_strncmp( /*  in */ PARS_LPCSTR    a_strLeft,
                         /*  in */ PARS_LPCSTR    a_strRight,
                         /*  in */ PARS_DWORD     a_nMaxLen)
{
    size_t x = 0;

    if (!a_nMaxLen)
    {
        return 0;
    }

    /*
     * This explicit guard needed to deal correctly with boundary
     * cases: strings shorter than 4 bytes and strings longer than
     * UINT_MAX-4 bytes .
     */
    if( a_nMaxLen >= 4 )
    {
        /* unroll by four */
        for (; x < a_nMaxLen-4; x+=4)
        {
            a_strLeft+=4;
            a_strRight +=4;

            if (*(a_strLeft-4) == 0 || *(a_strLeft-4) != *(a_strRight-4))
            {
                return(*(unsigned char *)(a_strLeft-4) - *(unsigned char *)(a_strRight-4));
            }

            if (*(a_strLeft-3) == 0 || *(a_strLeft-3) != *(a_strRight-3))
            {
                return(*(unsigned char *)(a_strLeft-3) - *(unsigned char *)(a_strRight-3));
            }

            if (*(a_strLeft-2) == 0 || *(a_strLeft-2) != *(a_strRight-2))
            {
                return(*(unsigned char *)(a_strLeft-2) - *(unsigned char *)(a_strRight-2));
            }

            if (*(a_strLeft-1) == 0 || *(a_strLeft-1) != *(a_strRight-1))
            {
                return(*(unsigned char *)(a_strLeft-1) - *(unsigned char *)(a_strRight-1));
            }
        }
    }

    /* residual loop */
    for (; x < a_nMaxLen; x++)
    {
        if (*a_strLeft == 0 || *a_strLeft != *a_strRight)
        {
            return(*(unsigned char *)a_strLeft - *(unsigned char *)a_strRight);
        }
        a_strLeft+=1;
        a_strRight+=1;
    }
    return 0;
}
#endif /* PARS_OWN_STRNCMP */

#if PARS_OWN_STRICMP
/*--- PARSP_stricmp() -------------------------------------------------------------------------------- PARSP_stricmp() ---*/
/**
 *  @brief   Compares two strings for lexical order, ignoring case.  The comparison stops
 *           after: (1) a difference between the strings is found, (2) the end
 *           of the strings is reached
 *
 *  @param   a_strLeft      [  bi ]    String to compare
 *  @param   a_strRight     [  bi ]    String to compare
 *
 *  @return  PARS_INT:       <0 if Left < Right
 *                           0 if Left == Right
 *                           >0 if Left > Right
 */
/*--- PARSP_stricmp() -------------------------------------------------------------------------------- PARSP_stricmp() ---*/
PARS_INT  PARSP_stricmp( /*  in */ PARS_LPCSTR         a_strLeft,
                         /*  in */ PARS_LPCSTR         a_strRight)
{
    PARS_INT f,l;
    PARS_LPSTR  tSrc = (PARS_LPSTR) a_strRight;
    PARS_LPSTR  tDst = (PARS_LPSTR) a_strLeft;

    do {
        if ( ((f = (PARS_BYTE)(*(tDst++))) >= 'A') && (f <= 'Z') )
            f -= ('A' - 'a');

        if ( ((l = (PARS_BYTE)(*(tSrc++))) >= 'A') && (l <= 'Z') )
            l -= ('A' - 'a');
    } while ( f && (f == l) );

    return(f - l);
}
#endif /* PARS_OWN_STRICMP */

#if PARS_OWN_STRCMP
/*--- PARSP_strcmp() ---------------------------------------------------------------------------------- PARSP_strcmp() ---*/
/**
 *  @brief   Compares two strings for lexical order.  The comparison stops
 *           after: (1) a difference between the strings is found, (2) the end
 *           of the strings is reached
 *
 *  @param   a_strLeft      [  bi ]    String to compare
 *  @param   a_strRight     [  bi ]    String to compare
 *
 *  @return  PARS_INT:       <0 if Left < Right
 *                           0 if Left == Right
 *                           >0 if Left > Right
 */
/*--- PARSP_strcmp() ---------------------------------------------------------------------------------- PARSP_strcmp() ---*/
PARS_INT  PARSP_strcmp( /*  in */ PARS_LPCSTR    a_strLeft,
                        /*  in */ PARS_LPCSTR    a_strRight)
{
    int ret = 0 ;

    while( ! (ret = *(unsigned char *)a_strLeft - *(unsigned char *)a_strRight) && *a_strRight)
            ++a_strLeft, ++a_strRight;

    if ( ret < 0 )
            ret = -1 ;
    else if ( ret > 0 )
            ret = 1 ;
    return( ret );
}
#endif /* PARS_OWN_STRCMP */

#if PARS_OWN_STRRCHR
/*--- PARSP_strrchr() -------------------------------------------------------------------------------- PARSP_strrchr() ---*/
/**
 *  @brief   Finds the last occurrence of a_nCharacter in a_strString.  The terminating
 *           null character is used as part of the search.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_nCharacter       [  in ]    Character to search for
 *
 *  @return  PARS_LPSTR position of the a_strString found, or PARS_NULL if not found.
 */
/*--- PARSP_strrchr() -------------------------------------------------------------------------------- PARSP_strrchr() ---*/
PARS_LPSTR  PARSP_strrchr( /*  in */ PARS_LPCSTR         a_strString,
                           /*  in */ PARS_INT            a_nCharacter)
{
    char *start = (char *)a_strString;

    while (*a_strString++)                       /* find end of a_strString */
            ;
                                            /* search towards front */
    while (--a_strString != start && *a_strString != (char)a_nCharacter)
            ;

    if (*a_strString == (char)a_nCharacter)                /* char found ? */
            return( (char *)a_strString );

    return(NULL);
}
#endif /* PARS_OWN_STRRCHR */

#if PARS_OWN_STRCHR
/*--- PARSP_strchr() ---------------------------------------------------------------------------------- PARSP_strchr() ---*/
/**
 *  @brief   Finds the first occurrence of a_nCharacter in a_strString.  The terminating
 *           null character is used as part of the search.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_nCharacter       [  in ]    Character to search for
 *
 *  @return  PARS_LPSTR position of the a_strString found, or PARS_NULL if not found.
 */
/*--- PARSP_strchr() ---------------------------------------------------------------------------------- PARSP_strchr() ---*/
PARS_LPSTR  PARSP_strchr( /*  in */ PARS_LPCSTR         a_strString,
                          /*  in */ PARS_INT            a_nCharacter)
{
    PARS_LPSTR tString  = (PARS_LPSTR) a_strString;
    while (*tString && *tString != (PARS_CHAR)a_nCharacter)
            tString++;

    if (*tString == (PARS_CHAR)a_nCharacter)
    {
            return((PARS_LPSTR)tString);
    }
    return(PARS_NULL);
}
#endif /* PARS_OWN_STRCHR */

#if PARS_OWN_STRTOK
/*--- PARSP_strtok() ---------------------------------------------------------------------------------- PARSP_strtok() ---*/
/**
 *  @brief   Find the next token in a a_strString, using the specified locale passed in.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_strControl       [  in ]    separator character a_strString
 *
 *  @return  PARS_LPSTR     position of the a_strString found, or PARS_NULL if not found.
 */
/*--- PARSP_strtok() ---------------------------------------------------------------------------------- PARSP_strtok() ---*/
PARS_LPSTR  PARSP_strtok( /*  in */ PARS_LPSTR          a_strString,
                          /*  in */ PARS_LPCSTR         a_strControl)
{
    PARS_LPSTR     str;
    PARS_LPCSTR    ctrl = a_strControl;
    unsigned char map[32];
    PARS_INT count;
    static char *nextoken;

    /* Clear a_strControl map */
    for (count = 0; count < 32; count++)
            map[count] = 0;

    /* Set bits in delimiter table */
    do {
            map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);

    /* Initialize a_strToCheck. If a_strString is PARS_NULL, set a_strToCheck to the saved
     * pointer (i.e., continue breaking tokens out of the a_strString
     * from the a_strRight strtok call) */
    if (a_strString)
            str = a_strString;
    else
            str = nextoken;

    /* Find beginning of token (skip over leading delimiters). Note that
     * there is no token iff this loop sets a_strToCheck to point to the terminal
     * null (*a_strToCheck == '\0') */
    while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
            str++;

    a_strString = str;

    /* Find the end of the token. If it is not the end of the a_strString,
     * put a null there. */
    for ( ; *str ; str++ )
            if ( map[*str >> 3] & (1 << (*str & 7)) ) {
                    *str++ = '\0';
                    break;
            }

    /* Update nextoken (or the corresponding field in the per-thread data
     * structure */
    nextoken = str;

    /* Determine if a token has been found. */
    if ( a_strString == str )
            return PARS_NULL;
    else
            return a_strString;
}
#endif /* PARS_OWN_STRTOK */

#if PARS_OWN_STRPBRK
/*--- PARSP_strpbrk() -------------------------------------------------------------------------------- PARSP_strpbrk() ---*/
/**
 *  @brief   Scan strings for characters in specified character sets.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_strControl       [  in ]    separator character a_strString
 *
 *  @return  PARS_LPSTR     first occurrence of any character from a_strControl in a_strString,
 *                          or a NULL pointer if the two a_strString arguments have no characters in common.
 */
/*--- PARSP_strpbrk() -------------------------------------------------------------------------------- PARSP_strpbrk() ---*/
PARS_LPSTR  PARSP_strpbrk( /*  in */ PARS_LPCSTR         a_strString,
                           /*  in */ PARS_LPCSTR         a_strControl)
{
    PARS_LPSTR str  = (PARS_LPSTR) a_strString;
    PARS_LPSTR ctrl = (PARS_LPSTR) a_strControl;

    PARS_BYTE map[32];
    PARS_INT count;

    /* Clear out bit map */
    for (count=0; count<32; count++)
            map[count] = 0;

    /* Set bits in a_strControl map */
    while (*ctrl)
    {
            map[*ctrl >> 3] |= (1 << (*ctrl & 7));
            ctrl++;
    }

    /* 1st char in a_strControl map stops search */
    while (*str)
    {
            if (map[*str >> 3] & (1 << (*str & 7)))
                    return((PARS_LPSTR)str);
            str++;
    }
    return(PARS_NULL);
}
#endif /* PARS_OWN_STRPBRK */

#if PARS_OWN_STRCSPN
/*--- PARSP_strcspn() -------------------------------------------------------------------------------- PARSP_strcspn() ---*/
/**
 *  @brief   Scan strings for characters in specified character sets.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_strControl       [  in ]    separator character a_strString
 *
 *  @return  PARS_DWORD     the length of the initial segment of a_strString that consists entirely of
 *                          characters not in a_strControl. If a_strString begins with a character that is
 *                          in a_strControl, the function returns 0. No return value is reserved to indicate an error.
 */
/*--- PARSP_strcspn() -------------------------------------------------------------------------------- PARSP_strcspn() ---*/
PARS_INT    PARSP_strcspn( /*  in */ PARS_LPCSTR         a_strString,
                           /*  in */ PARS_LPCSTR         a_strControl)
{
    PARS_LPSTR str  = (PARS_LPSTR) a_strString;
    PARS_LPSTR ctrl = (PARS_LPSTR) a_strControl;

    PARS_BYTE map[32];
    PARS_INT count;

    /* Clear out bit map */
    for (count=0; count<32; count++)
            map[count] = 0;

    /* Set bits in a_strControl map */
    while (*ctrl)
    {
            map[*ctrl >> 3] |= (1 << (*ctrl & 7));
            ctrl++;
    }

    /* 1st char in a_strControl map stops search */
    count=0;
    map[0] |= 1;    /* null chars not considered */
    while (!(map[*str >> 3] & (1 << (*str & 7))))
    {
            count++;
            str++;
    }
    return(count);
}
#endif /* PARS_OWN_STRCSPN */

#if PARS_OWN_STRSPN
/*--- PARSP_strspn() ---------------------------------------------------------------------------------- PARSP_strspn() ---*/
/**
 *  @brief   Scan strings for characters in specified character sets.
 *
 *  @param   a_strString        [  in ]    String to search within
 *  @param   a_strControl       [  in ]    separator character a_strString
 *
 *  @return  PARS_DWORD     the length of the initial segment of a_strString that consists entirely of
 *                          characters contained in a_strControl. If a_strString begins with a character that is
 *                          not in a_strControl, the function returns 0. No return value is reserved to indicate an error.
 */
/*--- PARSP_strspn() ---------------------------------------------------------------------------------- PARSP_strspn() ---*/
PARS_INT  PARSP_strspn                              ( /*  in */ PARS_LPCSTR         a_strString,
                                                      /*  in */ PARS_LPCSTR         a_strControl)
{
    PARS_LPSTR str  = (PARS_LPSTR) a_strString;
    PARS_LPSTR ctrl = (PARS_LPSTR) a_strControl;

    PARS_CHAR map[32];
    PARS_INT count;

    /* Clear out bit map */
    for (count=0; count<32; count++)
            map[count] = 0;

    /* Set bits in a_strControl map */
    while (*ctrl)
    {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
        ctrl++;
    }

    /* 1st char NOT in a_strControl map stops search */
    if (*str)
    {
        count=0;
        while (map[*str >> 3] & (1 << (*str & 7)))
        {
            count++;
            str++;
        }
        return(count);
    }
    return(0);
}
#endif /* PARS_OWN_STRSPN */

#if PARS_OWN_STRREV
/*--- PARSP_strrev() ---------------------------------------------------------------------------------- PARSP_strrev() ---*/
/**
 *  @brief   Reverse characters of a a_strString.
 *
 *  @param   a_strString        [  in ]    String to reverse.
 *
 *  @return  PARS_LPSTR     a_strString
 */
/*--- PARSP_strrev() ---------------------------------------------------------------------------------- PARSP_strrev() ---*/
PARS_LPSTR  PARSP_strrev( /*  in */ PARS_LPSTR          a_strString)
{
    PARS_LPSTR start = a_strString;
    PARS_LPSTR left = a_strString;
    PARS_CHAR ch;

    while (*a_strString++)                 /* find end of a_strString */
            ;
    a_strString -= 2;

    while (left < a_strString)
    {
        ch = *left;
        *left++ = *a_strString;
        *a_strString-- = ch;
    }
    return(start);
}
#endif /* PARS_OWN_STRREV */

#if PARS_OWN_TOLOWER
/*--- PARSP_tolower() -------------------------------------------------------------------------------- PARSP_tolower() ---*/
/**
 *  @brief   Convert character to lower case.
 *
 *  @param   a_chCharacter        [  in ]    Character to convert lowercase.
 *
 *  @return  PARS_CHAR     Character in lowercase
 */
/*--- PARSP_tolower() -------------------------------------------------------------------------------- PARSP_tolower() ---*/
PARS_CHAR   PARSP_tolower( /*  in */ PARS_CHAR           a_chCharacter)
{
    if(a_chCharacter>='A' && a_chCharacter<='Z')
    {
        return a_chCharacter-'A'+'a';
    }
    return a_chCharacter;
}
#endif /* PARS_OWN_TOLOWER */

#if PARS_OWN_TOUPPER
/*--- PARSP_toupper() -------------------------------------------------------------------------------- PARSP_toupper() ---*/
/**
 *  @brief   Convert character to upper case.
 *
 *  @param   a_chCharacter        [  in ]    Character to convert uppercase.
 *
 *  @return  PARS_CHAR     Character in uppercase
 */
/*--- PARSP_toupper() -------------------------------------------------------------------------------- PARSP_toupper() ---*/
PARS_CHAR   PARSP_toupper( /*  in */ PARS_CHAR           a_chCharacter)
{
    if(a_chCharacter>='a' && a_chCharacter<='z')
    {
        return a_chCharacter-'a'+'A';
    }
    return a_chCharacter;
}
#endif /* PARS_OWN_TOUPPER */

#if PARS_OWN_STRLWR
/*--- PARSP_strlwr() ---------------------------------------------------------------------------------- PARSP_strlwr() ---*/
/**
 *  @brief   Convert a string to lower case.
 *
 *  @param   a_strString        [  in ]    Character to convert lowercase.
 *
 *  @return  PARS_LPSTR     string in lowercase (same as a_strString)
 */
/*--- PARSP_strlwr() ---------------------------------------------------------------------------------- PARSP_strlwr() ---*/
PARS_LPSTR  PARSP_strlwr( /*  in */ PARS_LPSTR           a_strString)
{
    PARS_LPSTR    tStr = a_strString;
    PARS_LPSTR    act;

    do {

        act = tStr++;

        if ( ((*act) >= 'A') && ( (*act) <= 'Z') )
            (*act) -= ('A' - 'a');

    } while ((*tStr) != 0);

    return a_strString;
}
#endif /* PARS_OWN_STRLWR */

#if PARS_OWN_STRUPR
/*--- PARSP_strupr() ---------------------------------------------------------------------------------- PARSP_strupr() ---*/
/**
 *  @brief   Convert a string to lower case.
 *
 *  @param   a_strString        [  in ]    Character to convert lowercase.
 *
 *  @return  PARS_LPSTR     string in lowercase (same as a_strString)
 */
/*--- PARSP_strupr() ---------------------------------------------------------------------------------- PARSP_strupr() ---*/
PARS_LPSTR  PARSP_strupr( /*  in */ PARS_LPSTR  a_strString)
{
    PARS_LPSTR    tStr = a_strString;
    PARS_LPSTR act;

    do {

        act = tStr++;

        if ( ((*act) >= 'a') && ( (*act) <= 'z') )
            /* (*act) += ('a' - 'A'); em3702: FIX: instead of ADDING the difference we have to SUBSTRACT it because in the ASCII format, the UPPERCASE letters are BEFORE the lowercase ones! */
            (*act) -= ('a' - 'A');

    } while ((*tStr) != 0);

    return a_strString;
}
#endif /* PARS_OWN_STRUPR */

/*--- Floating Point checks                                                                      Floating Point checks ---*/
#if PARS_OWN_ISNAN
PARS_INT PARSP_isNan( /*  in */ PARS_DOUBLE a_dblToCheck)
{
    return (PARS_INT)isnan(a_dblToCheck);
}
#endif /* PARS_OWN_ISNAN */

#if PARS_OWN_ISFINITE
PARS_INT    PARSP_isfinite( /*  in */ PARS_DOUBLE   a_dblToCheck)
{
    return (PARS_INT)isfinite(a_dblToCheck);
}
#endif /* PARS_OWN_ISFINITE */

/*--- printf derivate                                                                                  printf derivate ---*/

#if PARS_OWN_SPRINTF
/*--- PARSP_sprintf() ------------------------------------------------------------------------------ PARSP_sprintf() ---*/
/**
 *  @brief   prints to a string.
 *
 *  @param   a_strBuffer        [  in ]    Buffer to print to
 *  @param   a_strFormat        [  in ]    Format string to print
 *  @param   ...                [  in ]    Additional parameters
 *
 *  @return  PARS_LPSTR     string in lowercase (same as a_strString)
 */
/*--- PARSP_sprintf() ------------------------------------------------------------------------------ PARSP_sprintf() ---*/
#error no own implementation for PARSP_sprintf
PARS_LPSTR  PARSP_sprintf                           ( /*  in */ PARS_LPSTR          a_strBuffer,
                                                      /*  in */ PARS_LPCSTR         a_strCommand,
                                                      /*  in */ PARS_va_list        a_Parameter)
{
    return PARS_NULL;
}
#endif /* PARS_OWN_SPRINTF */

///*--- PARSP_snprintf() ------------------------------------------------------------------------------ PARSP_snprintf() ---*/
///**
// *  @brief   prints to a string delimited in size by a_nBuffersize.
// *
// *  @param   a_strBuffer        [  in ]    Buffer to print to
// *  @param   a_nBufferSize      [  in ]    Size of the buffer
// *  @param   a_strFormat        [  in ]    Format string to print
// *  @param   ...                [  in ]    Additional parameters
// *
// *  @return  PARS_LPSTR     string in lowercase (same as a_strString)
// */
///*--- PARSP_snprintf() ------------------------------------------------------------------------------ PARSP_snprintf() ---*/
//PARS_RESULT PARSP_snprintf                              ( /*  in */ PARS_LPSTR      a_strBuffer,
//                                                          /*  in */ PARS_INT        a_nBufferSize,
//                                                          /*  in */ PARS_LPCSTR     a_strFormat,
//                                                          /*  in */ ...)
//{
//    PARS_INT        nResult = 0;
//    PARS_va_list    vaList;
//
//    a_strBuffer[0]='\0';
//    PARS_va_start ( vaList, a_strFormat );
//    nResult = PARS_vsnprintf( a_strBuffer, a_nBufferSize, a_strFormat, vaList);
//    PARS_va_end( vaList );
//    /* some vsnprintf functions are not writing a terminating '\0'              */
//    a_strBuffer[a_nBufferSize-1] = '\0';
//
//    if ( ( nResult < 0 )            ||
//         ( nResult > a_nBufferSize) )
//    {
//        return MWE_MEMORY_ERROR;
//    }
//    return MWE_NO_ERROR;
//}

#if PARS_OWN_VSNPRINTF
/*--- PARSP_vsnprintf() ----------------------------------------------------------------------------- PARSP_vsnprintf() ---*/
/**
 *  @brief   prints to a string, with parameters given as a va list.
 *           The destination string delimited in size by a_nBuffersize.
 *
 *  @param   a_strBuffer        [  in ]    Buffer to print to
 *  @param   a_nBufferSize      [  in ]    Size of the buffer
 *  @param   a_strCommand       [  in ]    Format string to print
 *  @param   a_Parameter        [  in ]    Additional parameters
 *
 *  @return  PARS_LPSTR     string in lowercase (same as a_strString)
 */
/*--- PARSP_vsnprintf() ---------------------------------------------------------------------------- PARSP_vsnprintf() ---*/
#error no own implementation for PARSP_vsnprintf
PARS_INT  PARSP_vsnprintf                           ( /*  in */ PARS_LPSTR          a_strBuffer,
                                                      /*  in */ PARS_INT            a_nBufferSize,
                                                      /*  in */ PARS_LPCSTR         a_strCommand,
                                                      /*  in */ PARS_va_list        a_Parameter);
{
    return 0;
}
#endif /* PARS_OWN_VSNPRINTF */

#if PARS_OWN_SSCANF /*  PARSP_sscanf ------------------------------------------------------------------- PARSP_sscanf --- */
    #define PARS_sscanf PARSP_sscanf
    PARS_LPSTR  PARSP_sscanf                            ( /*  in */ PARS_LPCSTR         a_strBuffer,
                                                          /*  in */ PARS_LPCSTR         a_strFormat,
                                                          /*  in */ ...)
#else /* PARS_OWN_SSCANF */
    #define PARS_sscanf sscanf
#endif /* PARS_OWN_SSCANF */

/*--- String to number functions                                                            String to number functions ---*/

#if PARS_OWN_ATOI
/*--- PARSP_atoi() -------------------------------------------------------------------------------------- PARSP_atoi() ---*/
/**
 *  @brief   Convert a string to an integer
 *
 *  @param   a_strBuffer        [  in ]    Buffer containing the string
 *
 *  @return  PARS_INT     the converted number
 */
/*--- PARSP_atoi() -------------------------------------------------------------------------------------- PARSP_atoi() ---*/
PARS_INT    PARSP_atoi( /*  in */ PARS_LPCSTR         a_strBuffer)
{
    return (PARS_INT) PARSP_atol(a_strBuffer);
}
#endif /* PARS_OWN_ATOI */

#if PARS_OWN_ATOL
/*--- PARSP_atol() -------------------------------------------------------------------------------------- PARSP_atol() ---*/
/**
 *  @brief   Convert a string to a long integer
 *
 *  @param   a_strBuffer        [  in ]    Buffer containing the string
 *
 *  @return  PARS_INT     the converted number
 */
/*--- PARSP_atol() -------------------------------------------------------------------------------------- PARSP_atol() ---*/
PARS_LONG PARSP_atol( /*  in */ PARS_LPCSTR a_strBuffer)
{
    PARS_INT c;              /* current char */
    PARS_LONG total;         /* current total */
    PARS_INT sign;           /* if '-', then negative, otherwise positive */
    PARS_LPSTR tNptr    = (PARS_LPSTR) a_strBuffer;

    /* skip whitespace */
    while ( PARS_isspace((PARS_INT)(PARS_BYTE)*tNptr) )
        ++tNptr;

    c = (PARS_INT)(PARS_BYTE)*tNptr++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (PARS_INT)(PARS_BYTE)*tNptr++;    /* skip sign */

    total = 0;

    while( PARS_isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (PARS_INT)(PARS_BYTE)*tNptr++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}
#endif /* PARS_OWN_ATOL */

/* --- -------------------------------------------------------------------------------------------------------------- --- *\
|   Helper functions for Conversion                                                      Helper functions for Conversion   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */

#if PARS_OWN_ITOA || PARS_OWN_LTOA || PARS_OWN_ULTOA
/*--- PARSP_xtoa() -------------------------------------------------------------------------------------- PARSP_xtoa() ---*/
/**
 *  @brief   Convert a number to a string
 *
 *  @param   a_nValue        [  in ]    The number to convert
 *  @param   a_strBuffer     [  in ]    Buffer to write the result to
 *  @param   a_nRadix        [  in ]    Base of the conversion (normally 10)
 *  @param   a_nIsNeg        [  in ]    Is the value negative
 *
 *  @return  PARS_INT     the converted number
 */
/*--- PARSP_xtoa() -------------------------------------------------------------------------------------- PARSP_xtoa() ---*/
static PARS_VOID PARSP_xtoa( PARS_DWORD   a_nValue,
                             PARS_LPSTR   a_strBuffer,
                             unsigned     a_nRadix,
                             PARS_INT     a_nIsNeg)
{
    PARS_LPSTR   p;                /* pointer to traverse string */
    PARS_LPSTR   firstdig;         /* pointer to a_strLeft digit */
    PARS_CHAR    temp;              /* temp char */
    unsigned digval;        /* value of digit */

    p = a_strBuffer;

    if (a_nIsNeg) {
        /* negative, so output '-' and negate */
        *p++ = '-';
        a_nValue = (PARS_DWORD)(-(PARS_LONG)a_nValue);
    }

    firstdig = p;           /* save pointer to a_strLeft digit */

    do {
        digval = (unsigned) (a_nValue % a_nRadix);
        a_nValue /= a_nRadix;       /* get next digit */

        /* convert to ascii and store */
        if (digval > 9)
            *p++ = (PARS_CHAR) (digval - 10 + 'a');  /* a letter */
        else
            *p++ = (PARS_CHAR) (digval + '0');       /* a digit */
    } while (a_nValue > 0);

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

    *p-- = '\0';            /* terminate string; p points to a_strRight digit */

    do {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;   /* swap *p and *firstdig */
        --p;
        ++firstdig;         /* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */
}
#endif /*  PARS_OWN_ITOA || PARS_OWN_LTOA || PARS_OWN_ULTOA  */

#if PARS_OWN_ITOA
/*--- PARSP_itoa() -------------------------------------------------------------------------------------- PARSP_itoa() ---*/
/**
 *  @brief   Convert an integer to a string, using PARSP_xtoa.
 *
 *  @param   a_nValue        [  in ]    The integer to convert
 *  @param   a_strBuffer     [  in ]    Buffer to write the result to
 *  @param   a_nRadix        [  in ]    Base of the conversion (normally 10)
 *
 *  @return  PARS_INT     the converted number
 */
/*--- PARSP_itoa() -------------------------------------------------------------------------------------- PARSP_itoa() ---*/
PARS_LPSTR  PARSP_itoa( /*  in */ PARS_INT            a_nValue,
                        /*  in */ PARS_LPSTR          a_strBuffer,
                        /*  in */ PARS_INT            a_nRadix)
{
    if (a_nRadix == 10 && a_nValue < 0)
        PARSP_xtoa((PARS_DWORD)a_nValue, a_strBuffer, a_nRadix, 1);
    else
        PARSP_xtoa((PARS_DWORD)(PARS_DWORD)a_nValue, a_strBuffer, a_nRadix, 0);
    return a_strBuffer;
}
#endif /* PARS_OWN_ITOA */

#if PARS_OWN_LTOA
/*--- PARSP_ltoa() -------------------------------------------------------------------------------------- PARSP_ltoa() ---*/
/**
 *  @brief   Convert a long integer to a string, using PARSP_xtoa.
 *
 *  @param   a_nValue        [  in ]    The integer to convert
 *  @param   a_strBuffer     [  in ]    Buffer to write the result to
 *  @param   a_nRadix        [  in ]    Base of the conversion (normally 10)
 *
 *  @return  PARS_LPSTR     the converted number as string
 */
/*--- PARSP_ltoa() -------------------------------------------------------------------------------------- PARSP_ltoa() ---*/
PARS_LPSTR  PARSP_ltoa( /*  in */ PARS_INT            a_nValue,
                        /*  in */ PARS_LPSTR          a_strBuffer,
                        /*  in */ PARS_INT            a_nRadix)
{
    PARSP_xtoa((PARS_DWORD)a_nValue, a_strBuffer, a_nRadix, (a_nRadix == 10 && a_nValue < 0));
    return a_strBuffer;
}
#endif /* PARS_OWN_LTOA */

#if PARS_OWN_ULTOA
/*--- PARSP_ultoa() ------------------------------------------------------------------------------------ PARSP_ultoa() ---*/
/**
 *  @brief   Convert an unsigned long integer to a string, using PARSP_xtoa.
 *
 *  @param   a_nValue        [  in ]    The integer to convert
 *  @param   a_strBuffer     [  in ]    Buffer to write the result to
 *  @param   a_nRadix        [  in ]    Base of the conversion (normally 10)
 *
 *  @return  PARS_INT     the converted number
 */
/*--- PARSP_ultoa() ------------------------------------------------------------------------------------ PARSP_ultoa() ---*/
PARS_LPSTR  PARSP_ultoa( /*  in */ PARS_DWORD          a_nValue,
                         /*  in */ PARS_LPSTR          a_strBuffer,
                         /*  in */ PARS_INT            a_nRadix)
{
    PARSP_xtoa(a_nValue, a_strBuffer, a_nRadix, 0);
    return a_strBuffer;
}
#endif /* PARS_OWN_ULTOA */

#if PARS_OWN_WRMDIR
PARS_INT  PARSP_wrmdir ( /*  in */ const wchar_t *dirname)
{
   PARS_CHAR path[ADN_FS_TMP_FILENAME_LENGTH];
   size_t count = wcsrtombs(path, &dirname, ADN_FS_TMP_FILENAME_LENGTH, NULL);
   if(count >= 0)
   {
      path[count] = '\0';
      return rmdir(path);
   }else
   {
      return (PARS_INT)count;
   }
}
#endif /* PARS_OWN_WRMDIR */

#if PARS_OWN_WREMOVE
PARS_INT  PARSP_wremove ( /*  in */ const wchar_t *name)
{
   PARS_CHAR path[ADN_FS_TMP_FILENAME_LENGTH];
   size_t count = wcsrtombs(path, &name, ADN_FS_TMP_FILENAME_LENGTH, NULL);
   if(count >= 0)
   {
      path[count] = '\0';
      return remove(path);
   }else
   {
      return (PARS_INT)count;
   }
}
#endif /* PARS_OWN_WREMOVE */

#if PARS_OWN_TZSET
void  PARSP_tzset (void)
{
   tzset();
}
#endif /* PARS_OWN_TZSET */

#if PARS_OWN_GET_TIMEZONE
PARS_INT  PARSP_get_timezone ( /* out */ PARS_INT* seconds)
{
   errno = 0;
   *seconds = 0;
   return errno;
}
#endif /* PARS_OWN_GET_TIMEZONE */

#if PARS_OWN_GET_DSTBIAS
PARS_INT  PARSP_get_dstbias ( /* out */ PARS_INT* seconds)
{
   errno = 0;
   *seconds = 0;
   return errno;
}
#endif /* PARS_OWN_GET_DSTBIAS */

/* --- -------------------------------------------------------------------------------------------------------------- --- *\
|   PARS String Functions                                                                          PARS String Functions   |
\* --- -------------------------------------------------------------------------------------------------------------- --- */

/*--- MSTR_Raw() -------------------------------------------------------------------- MSTR_Raw() ---*/
/**
 *  @brief   Returns the internal pointer to the string content, this must not be '\\0' terminated.
 *
 *  @param   a_mstrSource       [  in ]   String from which to get the raw string
 *
 *  @return  PARS_LPSTR     The Raw string
 */
/*--- MSTR_Raw() -------------------------------------------------------------------- MSTR_Raw() ---*/
PARS_LPSTR DLLCALL MSTR_Raw( /*  in */ PARS_LPSTR  a_mstrSource)
{
    if(a_mstrSource==PARS_NULL)
    {
        return PARS_NULL;
    }
    if( PARS_MSTR_IS_C_STRING(a_mstrSource))
    {
        return (PARS_LPSTR)a_mstrSource;
    }
    else
    {
        return (PARS_LPSTR)a_mstrSource;
    }
}

/*--- MSTR_Len() -------------------------------------------------------------------------------- MSTR_Len() ---*/
/**
 *  @brief   Returns the length of a String
 *
 *  @param   a_mstrSource    [  in ]   String to evaluate
 from the a_mstrSource
 *
 *  @return  PARS_RESULT
 */
/*--- MSTR_Len() -------------------------------------------------------------------------------- MSTR_Len() ---*/
PARS_DWORD DLLCALL MSTR_Len( /*  in */ PARS_LPSTR  a_mstrSource)
{
    if(a_mstrSource == PARS_NULL)
    {
        return 0;
    }
    if( PARS_MSTR_IS_C_STRING(a_mstrSource))
    {
        return PARS_strlen((PARS_LPSTR)a_mstrSource);
    }
    else
    {
        return PARS_strlen(a_mstrSource);
    }
}

/*--- PARS_MSTR_MaxLen() -------------------------------------------------------------------------- PARS_MSTR_MaxLen() ---*/
/**
 *  @brief   Returns the length of a String, but not more than the given a_dwMaxLen. (cf. strnlen() )
 *
 *  @param   a_mstrSource    [  in ]   String to evaluate from the a_mstrSource
 *  @param   a_dwMaxLen      [  in ]   Maximum allowed length of the string
 *
 *  @return  PARS_RESULT
 */
/*--- PARS_MSTR_MaxLen() -------------------------------------------------------------------------- PARS_MSTR_MaxLen() ---*/
PARS_DWORD DLLCALL PARS_MSTR_MaxLen( /*  in */ PARS_LPSTR		a_mstrSource,
                                     /*  in */ PARS_DWORD       a_dwMaxLen)
{
    PARS_DWORD  dwLen =  0;
    if(a_mstrSource == PARS_NULL)
    {
        return 0;
    }
    if( PARS_MSTR_IS_C_STRING(a_mstrSource))
    {
        dwLen = PARS_strlen((PARS_LPSTR)a_mstrSource);
    }
    else
    {
        dwLen = PARS_strlen(a_mstrSource);
    }
    if( (a_dwMaxLen>0)&& (dwLen>a_dwMaxLen))
    {
        dwLen = a_dwMaxLen;
    }
    return dwLen;
}

/*--- PARS_Round() -------------------------------------------------------------------------------------- PARS_Round() ---*/
/**
 *  @brief   Implementation of rounding for doubles
 *
 *  @param   a_dSrc        [  in ]    Value to be rounded
 *  @param   a_dwDecimals  [  in ]    Decimal places of the destination value
 *  @param   a_dwStrategy  [  in ]    Rounding strategy to use (PARS_ROUND_STRATEGY_...)
 *  @param   a_pdDst       [ out ]    The rounded value
 *
 *  @return
 */
/*--- PARS_Round() -------------------------------------------------------------------------------------- PARS_Round() ---*/
PARS_RESULT DLLCALL PARS_Round( /*  in */   PARS_DOUBLE     a_dSrc,
                                /*  in */   PARS_DWORD      a_dwDecimals,
                                /*  in */   PARS_DWORD      a_dwStrategy,
                                /* out */   PARS_LPDOUBLE   a_pdDst)
{
    switch(a_dwStrategy)
    {
        case PARS_ROUND_STRATEGY_DEFAULT:
        {
            PARS_DOUBLE dTemp   = pow(10.0, (PARS_DOUBLE) a_dwDecimals);
            PARS_DOUBLE dResult = floor(a_dSrc*dTemp + 0.5);
            dResult /= dTemp;
            *a_pdDst = dResult;
            return(MWE_NO_ERROR);
        }
        /*
         * TODO: Add your custom rounding methods here...
         */
        default:
        {
            PARS_PORT_TRACE(PARS_DEBUG_LEVEL_ERROR, "Unknown rounding strategy!\n");
            return(MWE_NOT_IMPLEMENTED);
        }

    }
}

/*--- *PARS_SafeMalloc() ------------------------------------------------------ *PARS_SafeMalloc() ---*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        Memory Management                                                                             |
|                                                                                                      |
|    Description:                                                                                      |
|            PARS_SafeMalloc() will do the same as malloc() does. additonally it produces a            |
|            DebugOutput with the size of the Memory block, the address, and the Linenumber            |
|            and FileName from the Caller.                                                             |
|                                                                                                      |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_size              [  in ]   Size of the requested memory block                              |
|        LineNumber          [  in ]   LineNumber of the Caller                                        |
|        FileName            [  in ]   The File of the Caller                                          |
|                                                                                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_VOID :   !=PARS_NULL The Memory Block, PARS_NULL: Out of Memory                          |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*--- *PARS_SafeMalloc() -------------------------------------------------------*PARS_SafeMalloc() ---*/
PARS_LPVOID DLLCALL PARS_SafeMalloc( /*  in */ PARS_DWORD     a_dwSize )
{
    return pnd_mem_alloc (a_dwSize);
}

/*--- PARS_SafeFree() ------------------------------------------------------------ PARS_SafeFree() ---*\
|                                                                                                      |
|    Group / Interface:                                                                                |
|        Memory Management                                                                             |
|                                                                                                      |
|    Description:                                                                                      |
|        PARS_SafeFree() will do the same as free() does. additonally it produces a                    |
|        DebugOutput with the address of the freed block, and the Linenumber                           |
|        and FileName from the Caller.                                                                 |
|                                                                                                      |
|        In the Header file is an #define PARS_SafeFree() so you can use PARS_SafeFree like            |
|        free(), without additional parameters.                                                        |
|                                                                                                      |
|    Parameters:                                                                                       |
|        a_ptrPointer        [  in ]   Pointer to the memory block to clear                            |
|        LineNumber          [  in ]   LineNumber of the Caller                                        |
|        FileName            [  in ]   The File of the Caller                                          |
|                                                                                                      |
|                                                                                                      |
|    Return Value:                                                                                     |
|        PARS_VOID :                                                                                   |
|                                                                                                      |
|    Side Effects:                                                                                     |
|        none                                                                                          |
|                                                                                                      |
\*--- PARS_SafeFree() ------------------------------------------------------------ PARS_SafeFree() ---*/
PARS_VOID DLLCALL PARS_SafeFree( /*  in */ PARS_LPVOID a_ptrPointer )
{
    if (a_ptrPointer != PARS_NULL)
    {
	    pnd_mem_free (a_ptrPointer);
    }
    return;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/