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
/*  F i l e               &F: PARS_MemoryService.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface definition of the memory service.                              */
/*                                                                           */
/*****************************************************************************/

#ifndef __MEMORY_SERVICE_DEFINED__
#define __MEMORY_SERVICE_DEFINED__

/* alignment macro */
#define PARS_MEM_MIN_ALIGN(a) (a<1?1:a)
#define PARS_MEM_ALIGN(size, a)   \
 (((PARS_DWORD)(size) + (PARS_DWORD)(PARS_MEM_MIN_ALIGN(a)-1)) & ~((PARS_DWORD)(PARS_MEM_MIN_ALIGN(a)-1)))

PARS_BEGIN_EXTERN_C

#if PARS_MEMORY_TRACE
#define PARS_MEM_TRACE_ARGS_MAC        , __LINE__,__FILE__
 #define PARS_MEM_TRACE_ARGS            , a_nLineNumber, a_strFileName
 #define PARS_MEM_TRACE_ARGS_UNUSED     PARS_UNREFERENCED_PARAMETER(a_nLineNumber);PARS_UNREFERENCED_PARAMETER(a_strFileName);
#else
 #define PARS_MEM_TRACE_ARGS_DEC
 #define PARS_MEM_TRACE_ARGS_MAC
 #define PARS_MEM_TRACE_ARGS
 #define PARS_MEM_TRACE_ARGS_UNUSED
#endif

#define MemMalloc(x)                    DbgMemMalloc(x  PARS_MEM_TRACE_ARGS_MAC)
#define MemFree(x)                      DbgMemFree(x  PARS_MEM_TRACE_ARGS_MAC)
#define NewString(x)                    DbgNewString(x  PARS_MEM_TRACE_ARGS_MAC)
#define AddMemBlock(a,b,c,d)            DbgAddMemBlock(a,b,c,d  PARS_MEM_TRACE_ARGS_MAC)
#define SAddMemBlock(a,b,c,d)           DbgSAddMemBlock(a,b,c,d  PARS_MEM_TRACE_ARGS_MAC)

typedef  PARS_LPVOID                    PARS_MEM_POOL_HANDLE;

#define MemPoolFixConstruct(a,b,c,d)    DbgMemPoolFixConstruct(a,b,c,d  PARS_MEM_TRACE_ARGS_MAC)
#define MemPoolFixDestruct(a)           DbgMemPoolFixDestruct(a  PARS_MEM_TRACE_ARGS_MAC)
#define MemPoolFixMalloc(a)             DbgMemPoolFixMalloc(a  PARS_MEM_TRACE_ARGS_MAC)
#define MemPoolFixFree(a,b)             DbgMemPoolFixFree(a,b  PARS_MEM_TRACE_ARGS_MAC)

#define SMemMalloc(x)                   DbgSMemMalloc((x)  PARS_MEM_TRACE_ARGS_MAC)
#define SMemFree(x)                     DbgSMemFree(x  PARS_MEM_TRACE_ARGS_MAC)
#define SNewString(x)                   DbgSNewString(x  PARS_MEM_TRACE_ARGS_MAC)
#define SNewStringFromBuffer(x,y)       DbgSNewStringFromBuffer(x, y  PARS_MEM_TRACE_ARGS_MAC)
#define SRealloc(a,b)                   DbgSReAlloc(a,b  PARS_MEM_TRACE_ARGS_MAC)

/* For direct caller like OpcUA */
#if PARS_USE_MEM_POOL
    PARS_LPVOID DLLCALL PARS_MEM_PoolMalloc( /*  in */ PARS_DWORD a_dwSize PARS_MEM_TRACE_ARGS_DEC );
    PARS_VOID   DLLCALL PARS_MEM_PoolFree( /*  in */  PARS_LPVOID a_pvBlock PARS_MEM_TRACE_ARGS_DEC );
    PARS_LPVOID DLLCALL PARS_MEM_PoolRealloc( /*  bi */ PARS_LPVOID     a_pvBlock, /*  bi */ PARS_DWORD      a_dwNewLen PARS_MEM_TRACE_ARGS_DEC);

    #define Direct_SMemMalloc(x)              PARS_MEM_PoolMalloc(x  PARS_MEM_TRACE_ARGS_MAC)
    #define Direct_SMemFree(x)                PARS_MEM_PoolFree(x  PARS_MEM_TRACE_ARGS_MAC)
    #define Direct_SReAlloc(x,y)              PARS_MEM_PoolRealloc(x, y PARS_MEM_TRACE_ARGS_MAC)
#else /* PARS_USE_MEM_POOL */
    PARS_LPVOID DLLCALL PARS_MEM_Malloc ( /*  in */ PARS_DWORD  a_dwSize PARS_MEM_TRACE_ARGS_DEC);
    PARS_LPVOID DLLCALL PARS_MEM_Realloc( /*  in */ PARS_LPVOID a_ptrPointer,
                                          /*  in */ PARS_DWORD  a_dwSize PARS_MEM_TRACE_ARGS_DEC);
    PARS_VOID   DLLCALL PARS_MEM_Free   ( /*  in */ PARS_LPVOID a_ptrPointer PARS_MEM_TRACE_ARGS_DEC);

    #define Direct_SMemMalloc(x)              PARS_MEM_Malloc(x  PARS_MEM_TRACE_ARGS_MAC)
    #define Direct_SMemFree(x)                PARS_MEM_Free(x  PARS_MEM_TRACE_ARGS_MAC)
    #define Direct_SReAlloc(x,y)              PARS_MEM_Realloc(x, y PARS_MEM_TRACE_ARGS_MAC)
#endif /* PARS_USE_MEM_POOL */

/*----------------------------------------------------------------------------------------------------*\
|   Typedefs MEM MALLOC POOLS                                              Typedefs MEM MALLOC POOLS   |
\*----------------------------------------------------------------------------------------------------*/

typedef struct PARS_S_MEM_MALLOC_POOLS_INIT_ENTRY
{
    PARS_DWORD  dwElementSize;
    PARS_DWORD  dwElementNumber;
}PARS_T_MEM_MALLOC_POOLS_INIT_ENTRY;

/*----------------------------------------------------------------------------------------------------*\
|   Service Interface                                                              Service Interface   |
\*----------------------------------------------------------------------------------------------------*/
typedef struct MemServiceInterface_
{
    PARS_LPVOID  (DLLCALL *DbgMemMalloc)            ( /*  in */ PARS_DWORD              a_dwSize
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID  (DLLCALL *DbgReAlloc)              ( /*  bi */ PARS_LPVOID             a_ppbyDst,
                                                      /*  bi */ PARS_DWORD              a_dwNewLen
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_VOID    (DLLCALL *DbgMemFree)              ( /*  in */ PARS_LPVOID             a_pMemoryBlock
                                                      PARS_MEM_TRACE_ARGS_DEC);
    PARS_LPSTR   (DLLCALL *DbgNewString)            ( /*  in */ PARS_LPCSTR             a_strToCopy
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID  (DLLCALL *DbgAddMemBlock)          ( /*  bi */ PARS_LPVOID*            a_ppbyDst,
                                                      /*  bi */ PARS_LPDWORD            a_pdwDstLen,
                                                      /*  in */ PARS_LPVOID             a_pbySrc,
                                                      /*  in */ PARS_DWORD              a_dwSrcLen
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID  (DLLCALL *DbgSAddMemBlock)         ( /*  bi */ PARS_LPVOID*            a_ppbyDst,
                                                      /*  bi */ PARS_LPDWORD            a_pdwDstLen,
                                                      /*  in */ PARS_LPVOID             a_pbySrc,
                                                      /*  in */ PARS_DWORD              a_dwSrcLen
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID  (DLLCALL *DbgSMemMalloc)           ( /*  in */ PARS_DWORD              a_dwSize
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_VOID    (DLLCALL *DbgSMemFree)             ( /*  in */ PARS_LPVOID             a_pMemoryBlock
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPSTR   (DLLCALL *DbgSNewString)           ( /*  in */ PARS_LPCSTR             a_strToCopy
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPSTR   (DLLCALL *DbgSNewStringFromBuffer) ( /*  in */ PARS_LPCSTR             a_strToCopy,
                                                      /*  in */ PARS_DWORD              a_dwStrLen
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID  (DLLCALL *DbgSReAlloc)             ( /*  bi */ PARS_LPVOID             a_ppbyDst,
                                                      /*  bi */ PARS_DWORD              a_dwNewLen
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_RESULT (DLLCALL *DbgMemPoolFixConstruct)   ( /*  in */ PARS_LPSTR              a_strPoolName,
                                                      /*  in */ PARS_DWORD              a_dwNumberOfElements,
                                                      /*  in */ PARS_DWORD              a_dwSizeOfElement,
                                                      /* out */ PARS_MEM_POOL_HANDLE*   a_phPool
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_RESULT (DLLCALL *DbgMemPoolFixDestruct)    ( /*  in */ PARS_MEM_POOL_HANDLE    a_hPool
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_LPVOID (DLLCALL *DbgMemPoolFixMalloc)      ( /*  in */ PARS_MEM_POOL_HANDLE    a_hPool
                                                      PARS_MEM_TRACE_ARGS_DEC);

    PARS_RESULT (DLLCALL *DbgMemPoolFixFree)        ( /*  in */ PARS_MEM_POOL_HANDLE    a_hPool,
                                                      /*  in */ PARS_LPVOID             a_pvBlock
                                                      PARS_MEM_TRACE_ARGS_DEC);

} MemServiceInterface, PARS_MEM_T_Service, *PARS_MEM_PT_Service;

/*----------------------------------------------------------------------------------------------------*\
|   Management Functions                                                        Management Functions   |
\*----------------------------------------------------------------------------------------------------*/

PARS_RESULT PARS_MEM_InitializeService                   ( /*  in */ PARS_LPSTR               a_strServiceName,
                                                          /*  in */ PARS_PT_MCB              a_pContext,
                                                          /*  in */ PARS_HDLLMODULE          a_hModuleHandle);

PARS_RESULT DLLCALL PARS_MEM_CleanupService              ( /*  in */ PARS_LONG                a_loRefCount,
                                                          /*  in */ PARS_BOOL                a_bForceShutdown);


PARS_VOID PARS_BuildMemDescr                             ( /*  in */ MemServiceInterface**   a_ppMem);

PARS_END_EXTERN_C

#endif  /* __MEMORY_SERVICE_DEFINED__ */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/