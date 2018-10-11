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
/*  F i l e               &F: PARSP_File.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  !!! File from the portlayer. !!!                                         */
/*  Public prototypes of the file functions.                                 */
/*                                                                           */
/*****************************************************************************/

/* ---------------------------------------------------------------------------------------------------------------------- */
/* --- !!! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! - !! PORTLAYER !! -- !! PORTLAYER !! -- !! PORTLAYER !! --- */
/* ---------------------------------------------------------------------------------------------------------------------- */

#ifndef __PARSP_FILE_H_DEFINED__
#define __PARSP_FILE_H_DEFINED__

PARS_BEGIN_EXTERN_C

/*----------------------------------------------------------------------------------------------------*\
|   Our FILE Abstraction                                                        Our FILE Abstraction   |
\*----------------------------------------------------------------------------------------------------*/
#define PARS_RAW_FILE    PARS_LPVOID                           /* Map Memory Structure to File Handle */
#define cMAX_PRINTF_STRING_LENGTH 1024

#define PARS_EOF         -1

#define PARSP_SEEK_SET               0
#define PARSP_SEEK_CUR               1
#define PARSP_SEEK_END               2

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

/*----------------------------------------------------------------------------------------------------*\
|   Prototypes for File Wrapper                                          Prototypes for File Wrapper   |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT      PARSP_fopen                        ( /*  in */ const PARS_LPSTR    a_strFilename,
                                                      /*  in */ const PARS_LPSTR    a_strMode,
                                                      /* out */ PARS_RAW_FILE*      a_pFileHandle);

PARS_DWORD       PARSP_fread                        ( /* out */ PARS_LPVOID         a_pbuffer,
                                                      /*  in */ PARS_DWORD          a_szSize,
                                                      /*  in */ PARS_DWORD          a_szCount,
                                                      /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_LONG        PARSP_feof                         ( /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_LONG        PARSP_fflush                       ( /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_RESULT      PARSP_fseek                        ( /*  in */ PARS_RAW_FILE       a_fileHandle,
                                                      /*  in */ PARS_LONG           a_offset,
                                                      /*  in */ PARS_LONG           a_origin);

PARS_LONG        PARSP_ftell                        ( /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_LONG        PARSP_filelength                   ( /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_RESULT  PARSP_fclose                           ( /*  in */ PARS_RAW_FILE       a_fileHandle);

PARS_RESULT  PARSP_finit                            ( PARS_VOID );
PARS_RESULT  PARSP_fCleanUp                         ( PARS_VOID );

PARS_RESULT  DLLCALL PARSP_FindFirst                ( /*  in */ PARS_LPSTR          a_strFileSpec,
                                                      /* out */ PARS_PT_FindData    a_pFindData,
                                                      /* out */ PARS_LPVOID*        a_pFindHandle );

PARS_RESULT  DLLCALL  PARSP_FindNext                ( /*  in */ PARS_LPVOID         a_pFindHandle,
                                                      /* out */ PARS_PT_FindData    a_pFindData);

PARS_LONG    DLLCALL  PARSP_FindClose               ( /*  in */ PARS_LPVOID         a_pFindHandle);

PARS_DWORD   DLLCALL  PARSP_FreeDiskSpace           ( PARS_VOID );

PARS_RESULT  PARSP_Delete                           ( /*  in */ PARS_LPSTR          a_strFileName);

PARS_RESULT  PARSP_Rename                           ( /*  in */ PARS_LPSTR          a_strOldName,
                                                      /*  in */ PARS_LPSTR          a_strNewName);

PARS_RESULT   PARSP_MakeDir                         ( /*  in */ PARS_LPSTR          a_strFileSpec);

PARS_RESULT   PARSP_GetFileDate                     ( /*  in */ PARS_RAW_FILE       a_fileHandle,
                                                      /* out */ PARS_LONG*          a_ploTime);

#if 0
PARS_RESULT PARS_LocalFileOpen                      ( /*  in */ PARS_LPSTR        a_strPath,
                                                      /*  in */ PARS_DWORD        a_dwPathLen,
                                                      /*  in */ PARS_LPCSTR       a_strMode,
                                                      /*  in */ PARS_LPVOID       a_hUserIdent,
                                                      /* out */ PARS_RAW_FILE*    a_pFileHandle );
#endif /* 0 */

PARS_END_EXTERN_C

#endif /* __PARSP_FILE_H_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/