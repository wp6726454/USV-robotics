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
/*  F i l e               &F: PARS_Stream.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definition of the stream service.                              */
/*                                                                           */
/*****************************************************************************/

#ifndef __PARS_Stream_h_Defined
#define __PARS_Stream_h_Defined

PARS_BEGIN_EXTERN_C

/*----------------------------------------------------------------------------------------------------*\
|   Type definitions                                                                Type definitions   |
\*----------------------------------------------------------------------------------------------------*/

#define PARS_STRM_FLAG_NO_FLAG          0x0000
#define PARS_STRM_FLAG_FREE_STREAM      0x0001
#define PARS_STRM_FLAG_READ_ONLY        0x0002
#define PARS_STRM_FLAG_C_STRING_MODE    0x0004
#define PARS_STRM_FLAG_TOUCHED          0x0008
/*----------------------------------------------------------------------------------------------------*\
|   Service Interface                                                              Service Interface   |
\*----------------------------------------------------------------------------------------------------*/
typedef struct PARS_STREAM_S_ServiceTable
{
    /* Following is the Service vTable Header, every Service must have
       this functions / Members */
    PARS_DWORD       dwStructSize;
    PARS_RESULT     (DLLCALL *GetMMI)             ( /* out */ PARS_PT_MMI*        a_pMMI_IF);

    PARS_RESULT     (DLLCALL *Init)               ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *ExpandStream)       ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_DWORD          a_dwBufferSize);

    PARS_RESULT     (DLLCALL *FreeStream)         ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *Compact)            ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *Attach)             ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_LPSTR          a_strBufferToAttach,
                                                    /*  in */ PARS_DWORD          a_dwBufferSize,
                                                    /*  in */ PARS_BOOL           a_bFreeAttachment);

    PARS_RESULT     (DLLCALL *Detach)             ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_LPSTR*         a_strDetachBuffer,
                                                    /*  in */ PARS_LPDWORD        a_pdwNewBufferSize,
                                                    /* out */ PARS_LPBOOL         a_pbFreeBuffer);

    PARS_RESULT     (DLLCALL *Write)              ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_LPSTR          a_strBufferToWrite,
                                                    /*  in */ PARS_DWORD          a_dwBufferSize);

    PARS_RESULT     (DLLCALL *Print)              ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_LPSTR          a_strFormat,
                                                    ...);

    PARS_RESULT     (DLLCALL *Read)               ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /* out */ PARS_LPSTR*         a_pstrReadPointer,
                                                    /*  in */ PARS_LPDWORD        a_pdwAmountToRead);

    PARS_RESULT     (DLLCALL *ReadDWord)          ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_LPDWORD        a_pdwOutput);

    PARS_RESULT     (DLLCALL *fread)              ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /* out */ PARS_LPSTR          a_strfreadPointer,
                                                    /*  in */ PARS_LPDWORD        a_pdwAmountTofread);

    PARS_RESULT     (DLLCALL *GetPosition)        ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /* out */ PARS_PT_StreamPos   a_pPostition);

    PARS_RESULT     (DLLCALL *RevertToPosition)   ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_PT_StreamPos   a_pPostition);

    PARS_DWORD      (DLLCALL *GetLength)          ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *SetLength)          ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_DWORD          a_dwNewLen);

    PARS_DWORD      (DLLCALL *GetFreeBufferSpace) ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *Clone)              ( /*  in */ PARS_PT_Stream      a_pInStream,
                                                    /* out */ PARS_PT_Stream      a_pOutStream);

    PARS_RESULT     (DLLCALL *Copy)               ( /*  in */ PARS_PT_Stream      a_pInStream,
                                                    /* out */ PARS_PT_Stream      a_pOutStream);
    PARS_RESULT     (DLLCALL *DetachStream)       ( /*  in */ PARS_PT_Stream      a_pInStream,
                                                    /* out */ PARS_PT_Stream      a_pOutStream);

    PARS_BOOL       (DLLCALL *IsEmpty)            ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *Append)             ( /* out */ PARS_PT_Stream      a_pOutStream,
                                                    /*  in */ PARS_PT_Stream      a_pInStream,
                                                    /*  in */ PARS_BOOL           a_bWriteToActualPosition);

    PARS_RESULT     (DLLCALL *AppendString)       ( /* out */ PARS_PT_Stream      a_pOutStream,
                                                    /*  in */ PARS_LPSTR          a_strToAppend,
                                                    /*  in */ PARS_DWORD          a_dwAppendLen);

    PARS_RESULT     (DLLCALL *MakeCString)        ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *RestoreFromCString) ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_RESULT     (DLLCALL *Reset)              ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_LPSTR      (DLLCALL *StringBuffer)       ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_LPSTR      (DLLCALL *StringFromActPos)   ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_DWORD      (DLLCALL *StrLenFromActPos)   ( /*  bi */ PARS_PT_Stream      a_pStream);

    PARS_LPSTR      (DLLCALL *NextWritePosition)  ( /*  in */ PARS_PT_Stream      a_pStream);

    PARS_BOOL       (DLLCALL *IsTouched)          ( /*  in */ PARS_PT_Stream      a_pStream );

    PARS_RESULT     (DLLCALL *WipeTouch)          ( /*  in */ PARS_PT_Stream      a_pStream );

    PARS_RESULT     (DLLCALL *GetStringFromPos)   ( /*  in */ PARS_PT_Stream      a_pStream,
                                                    /*  in */ PARS_PT_StreamPos   a_pPostition,
                                                    /* out */ PARS_LPSTR*         a_pstrPosition );

#if PARS_LINKAGE_FILESYS!=PARS_CF_NONE
    PARS_RESULT     (DLLCALL *PersistToFile)      ( /*  in */ PARS_LPVOID         a_hFile,
                                                    /*  in */ PARS_PT_Stream      a_strmToPersist);

    PARS_RESULT     (DLLCALL *LoadStreamFromFile) ( /*  in */ PARS_LPVOID         a_hFile,
                                                    /*  in */ PARS_PT_Stream      a_strmToPersist);
#endif /* PARS_LINKAGE_FILESYS!=PARS_CF_NONE */

} PARS_STREAM_T_ServiceTable, *PARS_STREAM_PT_ServiceTable;

/*----------------------------------------------------------------------------------------------------*\
|   Prototypes                                                                            Prototypes   |
\*----------------------------------------------------------------------------------------------------*/
PARS_RESULT DLLCALL PARS_InitializeStreamService    ( /*  in */ PARS_LPSTR        a_strServiceName,
                                                      /*  in */ PARS_PT_MCB       a_pContext,
                                                      /*  in */ PARS_HDLLMODULE   a_hModuleHandle);

PARS_END_EXTERN_C

#endif /* __PARS_Stream_h_Defined */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/