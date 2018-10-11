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
/*  F i l e               &F: PARS_FileSystem.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definition of the filesystem service.                          */
/*                                                                           */
/*****************************************************************************/
#ifndef __FILESYSTEM_DEFINED__
#define __FILESYSTEM_DEFINED__

PARS_BEGIN_EXTERN_C

#include "PARSP_File.h"


#define PARS_FILE_MAX_GROUP_LEN     256                                          /* Max Len of security group in xml file */
#define PARS_FILE                   PARS_LPVOID                                                            /* File Handle */

#if 0
#define PARSP_SEEK_SET               0
#define PARSP_SEEK_CUR               1
#define PARSP_SEEK_END               2

/*------------------------------------------------------------------------------------------------------------------------*\
|   Data Structures                                                                                      Data Structures   |
\*------------------------------------------------------------------------------------------------------------------------*/

#define PARSP_A_NORMAL               0x00
#define PARSP_A_RDONLY               0x01
#define PARSP_A_HIDDEN               0x02
#define PARSP_A_SYSTEM               0x04
#define PARSP_A_SUBDIR               0x10
#define PARSP_A_ARCH                 0x20
#define PARSP_A_ROM                  0x40
#define PARSP_A_XMLFILESYS           0x80
#define PARSP_A_LINK                 0x100

#define PARSP_FILE_ERROR            -1
#endif /* 0 */

#define PARS_FILETIME_ACCESS        1
#define PARS_FILETIME_WRITE         2
#define PARS_FILETIME_CREATE        3

/* Access to file system function from the static part of the xml parser core */

PARS_RESULT DLLCALL PARS_FILESYS_FileOpen               ( /*  in */ PARS_LPSTR        a_strPath,
                                                          /*  in */ PARS_LPCSTR       a_strMode,
                                                          /*  in */ PARS_LPVOID       a_hUserIdent,
                                                          /* out */ PARS_FILE*        a_pFileHandle);

PARS_RESULT DLLCALL PARS_FILESYS_SecureFileOpen         ( /*  in */ PARS_LPSTR        a_strPath,
                                                          /*  in */ PARS_LPSTR        a_strModuleName,
                                                          /*  in */ PARS_LPVOID       a_pSignature,
                                                          /*  in */ PARS_LPCSTR       a_strMode,
                                                          /* out */ PARS_FILE*        a_pFileHandle);

PARS_RESULT DLLCALL PARS_FILESYS_LocalFileOpen          ( /*  in */ PARS_LPSTR        a_strPath,
                                                          /*  in */ PARS_LPCSTR       a_strMode,
                                                          /*  in */ PARS_LPVOID       a_hUserIdent,
                                                          /* out */ PARS_FILE*        a_pFileHandle);

PARS_DWORD DLLCALL PARS_FILESYS_fread                   ( /*  in */ PARS_LPVOID       a_pBuffer,
                                                          /*  in */ PARS_DWORD        a_dwSize,
                                                          /*  in */ PARS_DWORD        a_dwCount,
                                                          /*  in */ PARS_FILE         a_filHandle);

PARS_DWORD DLLCALL PARS_FILESYS_fwrite                          ( /*  in */ const PARS_LPVOID a_pBuffer,
                                                          /*  in */ PARS_DWORD        a_dwSize,
                                                          /*  in */ PARS_DWORD        a_dwCount,
                                                          /*  in */ PARS_FILE         a_filHandle);

PARS_LONG DLLCALL PARS_FILESYS_fclose                   ( /*  in */ PARS_FILE         a_filHandle);

/*------------------------------------------------------------------------------------------------------------------------*\
|   File System Service Interface                                                          File System Service Interface   |
\*------------------------------------------------------------------------------------------------------------------------*/
typedef struct FileSysServiceInterface_
{

    /* Interface Handling */
    PARS_DWORD  dwStructSize;
    PARS_RESULT (DLLCALL *GetMMI)               ( /* out */ PARS_PT_MMI*       a_pMMI_IF);

    PARS_RESULT (DLLCALL* FileOpen)             ( /* in */ PARS_LPSTR          a_strPath,
                                                  /* in */ PARS_LPCSTR         a_strMode);

    PARS_RESULT (DLLCALL* SecureFileOpen)       ( /*  in */ PARS_LPSTR         a_strPath,
                                                  /*  in */ PARS_LPSTR         a_strModuleName,
                                                  /*  in */ PARS_LPVOID        a_pSignature,
                                                  /*  in */ PARS_LPCSTR        a_strMode,
                                                  /*  out */ PARS_FILE*        a_pFileHandle);

#if PARS_IS_STANDALONE
    PARS_RESULT (DLLCALL *LocalFileOpen)        ( /*  in */ PARS_LPSTR        a_strPath,
                                                  /*  in */ PARS_LPCSTR       a_strMode,
                                                  /*  in */ PARS_LPVOID       a_hUserIdent,
                                                  /* out */ PARS_FILE*        a_pFileHandle);
#endif /* PARS_IS_STANDALONE */

    PARS_DWORD  (DLLCALL* fread)                ( /*  in */ PARS_LPVOID        a_pBuffer,
                                                  /*  in */ PARS_DWORD         a_dwSize,
                                                  /*  in */ PARS_DWORD         a_dwCount,
                                                  /*  in */ PARS_FILE          a_filHandle);

    PARS_RESULT (DLLCALL* fseek)                ( /*  in */ PARS_FILE          a_filHandle,
                                                  /*  in */ PARS_LONG          a_loOffset,
                                                  /*  in */ PARS_LONG          a_loOrigin);

    PARS_DWORD  (DLLCALL* feof)                 ( /*  in */ PARS_FILE          a_filHandle);

    PARS_LONG   (DLLCALL* fflush)               ( /*  in */ PARS_FILE          a_fileHandle);

    PARS_DWORD  (DLLCALL* fprintf)              ( /*  in */ PARS_FILE          a_filHandle,
                                                  /*  in */ PARS_LPCSTR        a_lpszFormat,
                                                  /*  in */ ...);

    PARS_LONG   (DLLCALL* fclose)               ( /*  in */ PARS_FILE          a_filHandle);

   PARS_LONG   (DLLCALL* GetFileLength)        ( /*  in */ PARS_FILE          a_filHandle);

    PARS_LPSTR  (DLLCALL* GetFileRealm)         ( /*  in */ PARS_FILE          a_filHandle);

    PARS_LPSTR  (DLLCALL* GetRequestedAlgo)     ( /*  in */ PARS_FILE          a_filHandle );

    PARS_RESULT (DLLCALL* GetFileDate)          ( /*  in */ PARS_FILE          a_filHandle,
                                                  /*  in */ PARS_DWORD         a_dwWhichTime,
                                                  /* out */ PARS_LPLONG        a_ploTime);

    PARS_BOOL   (DLLCALL* IsXMLFileSys)         ( /*  in */ PARS_FILE          a_filHandle);

    PARS_RESULT (DLLCALL* GetFreeSpace)         ( /* out */ PARS_LPDWORD       a_pdwFreeSpace);

    PARS_RESULT (DLLCALL* GetDefaultFileName)   ( /*  in */ PARS_LPSTR         a_strPath,
                                                  /*  bi */ PARS_LPSTR         a_strDefFileName,
                                                  /*  in */ PARS_DWORD         a_dwDefFileNameLen);

    PARS_RESULT (DLLCALL* MapURLToLocalPath)    ( /* out */ PARS_LPSTR         a_strLocalPath,
                                                  /*  in */ PARS_LPSTR         a_strLocalLinkPath,
                                                  /*  in */ PARS_DWORD         a_dwLocalLinkPathLen,
                                                  /*  bi */ PARS_LPSTR         a_strRestPath,
                                                  /*  in */ PARS_DWORD         a_dwMaxLocalPathLen,
                                                  /*  in */ PARS_BOOL          a_bFirst);

    PARS_RESULT (DLLCALL* FindFirst)            ( /*  in */ PARS_LPSTR         a_strPath,
                                                  /*  in */ PARS_LPSTR         a_strSystemSection,
                                                  /*  in */ PARS_LPVOID        a_hUserIdent,
                                                  /* out */ PARS_T_FindData*   a_pFindData,
                                                  /* out */ PARS_LPVOID*       a_ppvFindHandle);

    PARS_RESULT (DLLCALL* FindNext)             ( /*  in */ PARS_LPVOID        a_pFindHandle,
                                                  /* out */ PARS_PT_FindData   a_pFindData);

    PARS_RESULT (DLLCALL* FindClose)            ( /*  in */ PARS_LPVOID        a_pFindHandle);

    PARS_LPSTR  (DLLCALL* GetLocalPath)         ( /*  in */ PARS_FILE          a_filHandle);

    PARS_RESULT (DLLCALL* DeleteFile)           ( /*  in */ PARS_LPSTR         a_strURL,
                                                  /*  in */ PARS_LPSTR         a_strStorage,
                                                  /*  in */ PARS_LPVOID        a_hUserIdent);

    PARS_RESULT (DLLCALL* RenameFile)           ( /*  in */ PARS_LPSTR         a_strOldURL,
                                                  /*  in */ PARS_LPSTR         a_strNewURL,
                                                  /*  in */ PARS_LPVOID        a_hUserIdent);

    PARS_RESULT (DLLCALL* HTTP_Copy)            ( /*  in */ PARS_LPSTR         a_strOldURL,
                                                  /*  in */ PARS_LPSTR         a_strNewURL,
                                                  /*  in */ PARS_LPVOID        a_hUserIdent);

    PARS_RESULT (DLLCALL* ConvertTimer)         ( /*  in */ PARS_LPSTR         a_strBuffer,
                                                  /*  in */ PARS_LONG          a_loTimeValue);

    PARS_BOOL   (DLLCALL* GetBrowsable)         ( /*  in */ PARS_FILE          a_filHandle);

    PARS_RESULT (DLLCALL* MKDir)                ( /*  in */ PARS_LPSTR         a_strPath,
                                                  /*  in */ PARS_LPVOID        a_hUserIdent);

	PARS_VOID (DLLCALL* xmlwrite)				( /*  in */ const PARS_LPVOID  a_pBuffer);
} FileSysServiceInterface, *PARS_FILESYS_PT_Interface;

/*------------------------------------------------------------------------------------------------------------------------*\
|   local functions                                                                                      local functions   |
\*------------------------------------------------------------------------------------------------------------------------*/

PARS_VOID BuildFileSysDescr                         ( /* out */ FileSysServiceInterface *a_pFileSys);

PARS_RESULT DLLCALL PARS_FILESYS_Initialize         ( /*  in */ PARS_LPSTR         a_strServiceName,
                                                      /*  in */ PARS_PT_MCB        a_pContext,
                                                      /*  in */ PARS_HDLLMODULE    a_hModuleHandle);

PARS_END_EXTERN_C

#endif  /* __FILESYSTEM_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/