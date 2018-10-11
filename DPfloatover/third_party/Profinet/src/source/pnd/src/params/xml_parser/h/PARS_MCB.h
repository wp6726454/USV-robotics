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
/*  F i l e               &F: PARS_MCB.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the Management Control Block.                              */
/*                                                                           */
/*****************************************************************************/

#ifndef MCB_H_DEFINED
#define MCB_H_DEFINED

#include "PARSP_Config.h"                                                                            /* Compiler Switches */
#include "PARS_Types.h"                                                                           /* Standard Definitions */

PARS_BEGIN_EXTERN_C

/*------------------------------------------------------------------------------------------------------------------------*\
|   Definitions                                                                                              Definitions   |
\*------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------*/
/**
 *         Format of the Trace Filter
 *
 *         ITs a 32Bit Value:
 *
 *   /--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--\
 *   |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16||15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
 *   |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
 *   |-----7-----|-----6-----|-----5-----|-----4-----||-----3-----|-----2-----|-----1-----|-----0-----|
 *   | E| S| C| C| C| C| C| C| C| C| C| C| C| C| C| C|| F| F| F| F| F| F| T| T| T| T| T| T| T| T| T| T|
 *   \--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--||--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--/
 *
 *         E == 1 : Error Condition                                                (  1 Bit )
 *         S == 1 : Standard Error Code, or == 0 : Component Specific Error Code   (  1 Bit )
 *         C ^=   : The Error Code                                                 ( 14 Bit )
 *         F ^=   : Facility (Code Component)                                      (  6 Bit )
 *         T ^=   : Trace Level, see defines above                                 ( 10 Bit )
 *
 *
 */
/*------------------------------------------------------------------------------------------------------------------------*/
typedef PARS_DWORD   PARS_TRACE_FILTER;

#define PARS_DEBUG_LEVEL_PARANOID       1100
#define PARS_DEBUG_LEVEL_HINT           1000
#define PARS_DEBUG_LEVEL_ENTRY           950
#define PARS_DEBUG_LEVEL_EXIT            900
#define PARS_DEBUG_LEVEL_INFO            800
#define PARS_DEBUG_LEVEL_BAIL_OUT        700    /* All BAIL_OUTS will use this level */
#define PARS_DEBUG_LEVEL_RUN_INFOS       600
#define PARS_DEBUG_LEVEL_LESS_RUN_INFOS  550
#define PARS_DEBUG_LEVEL_WARNING         500
#define PARS_DEBUG_LEVEL_ERROR           300
#define PARS_DEBUG_LEVEL_FATAL_ERROR     100
#define PARS_DEBUG_LEVEL_CONSOLE_ONLY   2048
/* MMI: Module Management Interface */
#define MMI_VERSION                         0x00010001L
#define MCB_VERSION                         0x00010001L

#define PARS_MAX_UNLOAD_RETRIES             4                                                    /* including forced Try! */

#define HTTP_FILTER_TYPE_PREFILTER          1
#define HTTP_FILTER_TYPE_POSTFILTER         2
#define HTTP_FILTER_TYPE_MASK               3

#define HTTP_MODULE_TYPE_FILTER             4
#define HTTP_MODULE_TYPE_APPLICATION        8
#define HTTP_MODULE_TYPE_SERVICE            12

#define PARSP_INFINITE                        0xFFFFFFFF /* Infinite Timeout for Semaphores */
#define PARSP_ERROR                           0xFFFFFFFF

#define PARS_INFINITE                         0xFFFFFFFF        /* Infinite wait for WaitForSemaphore */

#if PARS_USE_SYNCHRONISATION
    #define PARS_ENTER_CRITICAL_SECTION(a)      if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->EnterCriticalSection(a)
    #define PARS_LEAVE_CRITICAL_SECTION(a)      if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->LeaveCriticalSection(a)
    #define PARS_CREATE_CRITICAL_SECTION(a)     if(PARS_g_pCoreContext!=PARS_NULL) { a=PARS_NULL;PARS_g_pCoreContext->CreateCriticalSection(&a);}
    #define PARS_CHECK_CRITICAL_SECTION(a)      if(PARS_g_pCoreContext!=PARS_NULL) { if(a==PARS_NULL)BAIL_OUT_0(PARS_DEBUG_LEVEL_ERROR, MWE_MEMORY_ERROR, "Error creating semaphore.\n");}
    #define PARS_DELETE_CRITICAL_SECTION(a)     if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->DeleteCriticalSection(a);a=PARS_NULL;
    #define PARS_DEFINE_CRITICAL_SECTION(a)     PARS_LPVOID a = PARS_NULL;
    #define PARS_DECLARE_CRITICAL_SECTION(a)    PARS_LPVOID a;
    #define PARS_EXTERN_CRITICAL_SECTION(a)     extern PARS_LPVOID a;
    #define PARS_CREATE_SEMAPHORE(a,b,c)        if(PARS_g_pCoreContext!=PARS_NULL) { c=PARS_NULL;PARS_g_pCoreContext->CreateSemaphore(a,b,&c);}
    #define PARS_RELEASE_SEMAPHORE(a,b)         if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->ReleaseSemaphore(a,b)
    #define PARS_WAIT_FOR_SEMAPHORE(a,b)        if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->WaitForSemaphore(a,b)
    #define PARS_DELETE_SEMAPHORE(a)            if(PARS_g_pCoreContext!=PARS_NULL) PARS_g_pCoreContext->DeleteSemaphore(a)

#else   /* PARS_USE_SYNCHRONISATION */
    #define PARS_ENTER_CRITICAL_SECTION(a)
    #define PARS_LEAVE_CRITICAL_SECTION(a)
    #define PARS_CREATE_CRITICAL_SECTION(a)
    #define PARS_CHECK_CRITICAL_SECTION(a)
    #define PARS_DELETE_CRITICAL_SECTION(a)
    #define PARS_CREATE_SEMAPHORE(a,b,c)        c=(PARS_LPVOID)MWE_NOT_IMPLEMENTED
    /* b; only to avoid compiler warning that there are unused variables */
    #define PARS_RELEASE_SEMAPHORE(a,b)         b;
    #define PARS_WAIT_FOR_SEMAPHORE(a,b)        b;
    #define PARS_DELETE_SEMAPHORE(a)
    #define PARS_DEFINE_CRITICAL_SECTION(a)
    #define PARS_DECLARE_CRITICAL_SECTION(a)
    #define PARS_EXTERN_CRITICAL_SECTION(a)
#endif  /* PARS_USE_SYNCHRONISATION */

typedef struct PARS_S_ECB *PARS_ECB;                 /* Forward reference for the Callback definition */

/*------------------------------------------------------------------------------------------------------------------------*\
|   MCB Call Wrapper                                                                                    MCB Call Wrapper   |
\*------------------------------------------------------------------------------------------------------------------------*/
/*--- PARS_MCB_TRY_LOOKUP_SERVICE() ---------------------------------------------------- PARS_MCB_TRY_LOOKUP_SERVICE() ---*/
/**
 *  @brief   Performs a LookUpService and sets the ACTUAL_ERROR to the result of LookUpService
 *
 *  @param   NAME       [  in ] Name of the service to look up
 *  @param   VTABLE     [  bi ] Service vTable to be assigned
 *
 *  @return  A PARS_RESULT: The result of LookUpService
 *
 *  @sideeffects The ACTUAL_ERROR gets set
 */
/*--- PARS_MCB_TRY_LOOKUP_SERVICE() ---------------------------------------------------- PARS_MCB_TRY_LOOKUP_SERVICE() ---*/
#define PARS_MCB_TRY_LOOKUP_SERVICE(NAME, VTABLE)   ACTUAL_ERROR=(VTABLE?MWE_NO_ERROR:MCB->LookUpService(NAME, (PARS_LPVOID*) &VTABLE))

/*--- PARS_MCB_FREE_SERVICE() ---------------------------------------------------------------- PARS_MCB_FREE_SERVICE() ---*/
/**
 *  @brief   Performs a FreeService and returns the result of FreeService
 *
 *  @param   NAME       [  in ] Name of the service to be freed
 *  @param   VTABLE     [  bi ] Service vTable to be freed
 *
 *  @return  A PARS_RESULT
 */
/*--- PARS_MCB_FREE_SERVICE() ---------------------------------------------------------------- PARS_MCB_FREE_SERVICE() ---*/
#define PARS_MCB_FREE_SERVICE(NAME, VTABLE)   MCB->FreeService(NAME, (PARS_LPVOID*) &VTABLE)

/*--- PARS_MCB_LOOKUP_SERVICE() ------------------------------------------------------------ PARS_MCB_LOOKUP_SERVICE() ---*/
/**
 *  @brief   Performs a LookUpService and bails out if it failed.
 *
 *  @param   NAME       [  in ] Name of the service to look up
 *  @param   VTABLE     [  bi ] Service vTable to be assigned
 *
 *  @sideeffects The ACTUAL_ERROR gets set
 */
/*--- PARS_MCB_LOOKUP_SERVICE() ------------------------------------------------------------ PARS_MCB_LOOKUP_SERVICE() ---*/
#define PARS_MCB_LOOKUP_SERVICE(NAME, VTABLE)   PARS_MCB_TRY_LOOKUP_SERVICE(NAME, VTABLE);if(MWE_FAILED(ACTUAL_ERROR))BAIL_OUT_1(PARS_DEBUG_LEVEL_WARNING, ACTUAL_ERROR, "Service '%s' not found.\n", NAME)

/*--- PARS_MCB_LOOKUP_SERVICE_EX() ------------------------------------------------------ PARS_MCB_LOOKUP_SERVICE_EX() ---*/
/**
 *  @brief   Performs a LookUpServiceDelayed and set sthe ACTUAL_ERROR to the result of LookUpServiceDelayed
 *
 *  @param   NAME       [  in ] Name of the service to look up
 *  @param   CALLBACK   [  in ] Callback to call when the service was found
 *  @param   COOKIE     [  in ] Cookie to pass to the callback function
 *
 *  @sideeffects The ACTUAL_ERROR gets set
 */
/*--- PARS_MCB_LOOKUP_SERVICE_EX() ------------------------------------------------------ PARS_MCB_LOOKUP_SERVICE_EX() ---*/
#define PARS_MCB_LOOKUP_SERVICE_EX(NAME, CALLBACK, COOKIE)  ACTUAL_ERROR=MCB->LookUpServiceDelayed(NAME, CALLBACK, COOKIE)

typedef struct PARS_S_ParamContext
{
    PARS_LPSTR  strQuery;
    PARS_LPSTR  strCursor;
} PARS_T_ParamContext, *PARS_PT_ParamContext;

/*------------------------------------------------------------------------------------------------------------------------*\
|   Callback Functions                                                                                Callback Functions   |
\*------------------------------------------------------------------------------------------------------------------------*/
typedef PARS_RESULT (DLLCALL *PARS_FP_STARTUPCOMPLETE)   (PARS_VOID);

typedef PARS_RESULT (DLLCALL *PARS_FP_SERVICE_REGISTERED)( /*  in */ PARS_LPSTR          a_strServiceName,
                                                           /*  in */ PARS_LPVOID         a_pServiceCalltable,
                                                           /*  in */ PARS_LPVOID         a_pCookie);

typedef PARS_RESULT (DLLCALL *PARS_CLOSECOMPONENT)      ( /*  in */ PARS_LONG           a_loRefCount,
                                                          /*  in */ PARS_BOOL           a_bForceShutdown);

typedef PARS_BOOL (DLLCALL *PARS_DEBUGOUTPUT)           ( /*  in */ PARS_LPCSTR         a_strServerText,
                                                          /*  in */ PARS_DWORD          a_dwServerTextLen,
                                                          /*  in */ PARS_LPVOID         a_pvCookie);

typedef PARS_RESULT (DLLCALL *PARS_FP_INIT_SERVICE)     ( /*  in */ PARS_LPSTR          a_strServiceName,
                                                          /*  in */ PARS_LPVOID         a_pContext,
                                                          /*  in */ PARS_HDLLMODULE     a_hModuleHandle);

typedef PARS_RESULT (DLLCALL *PARS_FP_INIT_FILTER)      ( /*  in */ PARS_LPSTR          a_strFilterName,
                                                          /*  in */ PARS_LPVOID         a_pContext,
                                                          /*  in */ PARS_HDLLMODULE     a_hModuleHandle);

typedef PARS_RESULT (DLLCALL *PARS_FP_INIT_APPLICATION) ( /*  in */ PARS_LPSTR          a_strApplicationName,
                                                          /*  in */ PARS_LPSTR          a_strApplicationPath,
                                                          /*  in */ PARS_LPVOID         a_pContext,
                                                          /*  in */ PARS_HDLLMODULE     a_hModuleHandle);

typedef PARS_RESULT (DLLCALL *PARS_FP_INIT_WEBSERVICE)  ( /*  in */ PARS_LPVOID         a_pWsInfo,
                                                          /*  in */ PARS_LPVOID         a_pContext,
                                                          /*  in */ PARS_HDLLMODULE     a_hModuleHandle);

#if PARS_USES_FILTER
typedef PARS_RESULT (DLLCALL *PARS_FILTERPROC)          ( /*  in */ PARS_ECB            a_pECB,
                                                          /*  in */ PARS_DWORD          a_dwFlags,
                                                          /*  bi */ PARS_PT_Stream      a_strmIn,
                                                          /*  bi */ PARS_PT_Stream      a_strmOut);
#endif /* PARS_USES_FILTER */

typedef PARS_RESULT (DLLCALL *PARS_IO_COMPLETION_CB)    ( /*  in */ PARS_ECB            a_pECB,
                                                          /*  in */ PARS_LPVOID         a_pContext,
                                                          /*  in */ PARS_RESULT         a_resServerError);

typedef PARS_RESULT (DLLCALL *PARS_ONNEWREQUEST)        ( /*  in */ PARS_ECB            a_pECB);

#if PARS_DEBUG_OUT
typedef PARS_RESULT (DLLCALL *PARS_TRACE_FP)            ( /*  in */ PARS_TRACE_FILTER   a_tfLevel,
                                                          /*  in */ PARS_LPCSTR         a_strMessage,
                                                          /*  in */ ...);

typedef PARS_RESULT (DLLCALL *PARS_SET_CONDITION_FP)    ( /*  in */ PARS_TRACE_FILTER*  a_aFacilityList,
                                                          /*  in */ PARS_DWORD          a_dwFacilityCount,
                                                          /*  in */ PARS_DWORD          a_dwTraceLevel);

typedef PARS_RESULT (DLLCALL *PARS_GET_CONDITION_FP)    ( /*  in */ PARS_TRACE_FILTER** a_paFacilityList,
                                                          /*  in */ PARS_LPDWORD        a_dwFacilityCount,
                                                          /*  in */ PARS_LPDWORD        a_dwTraceLevel);

PARS_VOID PARS_MCB_SetDebugOutput                       ( /*  in */ PARS_DEBUGOUTPUT    a_fpDebugOutputCB,
                                                          /*  in */ PARS_LPVOID         a_pvCookie);

#define    HTTP_SETDEBUGOUTPUT(x,y)         PARS_MCB_SetDebugOutput(x,y)
    extern PARS_DEBUGOUTPUT  PARS_g_fpDebugOutputCB; /* Callback Function for Debug output */
    extern PARS_LPVOID  PARS_g_pvCookie; /* Cookie for Debug Output */
#else /* PARS_DEBUG_OUT */
    #define    HTTP_SETDEBUGOUTPUT(x,y)
    #define    HTTP_DEBUGOUTPUT(x)
#endif /* PARS_DEBUG_OUT */

typedef PARS_RESULT (DLLCALL *PARS_FP_SHUTDOWN_INFORM)  (  /* in */ PARS_LPVOID         a_pCookie);

/* Use this to initialize MCB in your Lib Initialization Routine! */
#define ASSIGN_MCB(x)  if (!x) return MWE_INVALID_PARAMETER; else PARS_g_pCoreContext=x;

PARS_RESULT PARS_MCB_CleanUp                             ( PARS_VOID );

/*------------------------------------------------------------------------------------------------------------------------*\
|   Structures                                                                                                Structures   |
\*------------------------------------------------------------------------------------------------------------------------*/
typedef struct   PARS_S_MODULE_INFO
{
    /* Members for all module Types */
    PARS_LONG                loRefCnt;
    PARS_CLOSECOMPONENT      fpCloseComponent;

    PARS_DWORD               dwFlags;
    PARS_LPVOID              hLib;

    union   U_CallEntry
    {
        /* Service Members */
        PARS_LPVOID              pServiceDescription;        /* Set when module is Service */
#if PARS_USES_FILTER
        /* Filter Members */
        PARS_FILTERPROC          fpFilter;
#endif /* PARS_USES_FILTER */
        /* Application Members */
        PARS_ONNEWREQUEST        fpOnNewRequest;
        /* Generic Function Pointer */
        PARS_FPVOID              fpCallEntry;
    } CallEntry;

    PARS_FP_INIT_SERVICE  pInit;
} PARS_T_MODULE_INFO, *PARS_PT_MODULE_INFO;

typedef struct PARS_S_MMI
{
    PARS_DWORD       dwStructSize;

    PARS_RESULT (DLLCALL *MMI_GetMMIVersion)            ( /* out */ PARS_DWORD*                 a_dwMMIVersion);
    PARS_RESULT (DLLCALL *MMI_GetIFVersion)             ( /* out */ PARS_DWORD*                 a_dwIFVersion);

} PARS_T_MMI, *PARS_PT_MMI;

PARS_RESULT DLLCALL PARS_MCB_GetMMIVersion              ( /* out */ PARS_DWORD*                 a_dwMMIVersion);
PARS_RESULT DLLCALL PARS_MCB_GetIFVersion               ( /* out */ PARS_DWORD*                 a_dwMMIVersion);
PARS_RESULT DLLCALL PARS_MCB_GetMMI                     ( /* out */ PARS_PT_MMI*                a_pMMI_IF);

PARS_RESULT DLLCALL PARS_CORE_FindErrorNode             ( /*  in */ PARS_WORD                   a_wErrorCode,
                                                          /*  in */ PARS_WORD                   a_wSubCode,
                                                          /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /* out */ PARS_XML_Node*              a_pndeErrorNode,
                                                          /* out */ PARS_LPBOOL                 a_pbDefaultUsed,
                                                          /* out */ PARS_LPBOOL                 a_pbDefaultErrorUsed,
                                                          /* out */ PARS_LPSTR*                 a_pstrErrorCode,
                                                          /* out */ PARS_LPDWORD                a_pdwErrorCodeLength);

PARS_XML_Node DLLCALL PARS_CORE_GetConfigBase           ( /*  in */ PARS_PT_String              a_mstrApplicationName);

PARS_XML_Node DLLCALL PARS_CORE_GetSystemSection        ( /*  in */ PARS_LPSTR                  a_strSectionName);

PARS_XML_Node DLLCALL PARS_CORE_GetRootNode             ( PARS_VOID );

PARS_XML_Sheet DLLCALL PARS_CORE_GetConfigSheet         ( PARS_VOID );

PARS_RESULT DLLCALL   PARS_CORE_GetConfigDoc            ( /* out */PARS_LPSTR*                  a_pstrXMLOrigXMLDoc );

PARS_RESULT DLLCALL PARS_CORE_CommitConfigXML           ( PARS_VOID );

PARS_RESULT DLLCALL PARS_CORE_GetString                 ( /*  in */ PARS_LPSTR                  a_strName,
                                                          /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /* out */ PARS_LPSTR*                 a_pstrValue,
                                                          /* out */ PARS_LPDWORD                a_pdwValueLen);

PARS_RESULT DLLCALL PARS_CORE_SetLanguage               ( /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /*  in */ PARS_DWORD                  a_dwLanguageLen);

PARS_RESULT DLLCALL PARS_CORE_GetLanguage               ( /* out */ PARS_LPSTR*                 a_pstrLanguage);

/*------------------------------------------------------------------------------------------------------------------------*\
|   Context Interface vTable                                                                    Context Interface vTable   |
\*------------------------------------------------------------------------------------------------------------------------*/

typedef struct PARS_S_MCB
{
    /* Following is the Service vTable Header, every Service must have
       this functions / Members */
    PARS_DWORD       dwStructSize;
    PARS_RESULT (DLLCALL *GetMMI)                       ( /* out */ PARS_PT_MMI*                a_pMMI_IF);

#if PARS_CHECK_STACK_SIZE
    PARS_DWORD       dwStackStartpos;
    PARS_DWORD       dwMaxStackpos;

    PARS_VOID   (DLLCALL *CheckStack)                   ( PARS_VOID );
#endif /* PARS_CHECK_STACK_SIZE */

    PARS_RESULT (DLLCALL* PreRegisterService)           ( /*  in */ PARS_LPSTR                  a_strServiceName,
                                                          /*  in */ PARS_FP_INIT_SERVICE        a_Initializer);

    PARS_RESULT (DLLCALL* InitPreRegisteredService)     ( /*  in */ PARS_LPSTR                  a_strServiceName );

    PARS_RESULT (DLLCALL* InitApplication)              ( /*  in */ PARS_LPSTR                  a_strApplicationName,
                                                          /*  in */ PARS_LPSTR                  a_strApplicationPath,
                                                          /*  in */ PARS_FP_INIT_APPLICATION    a_fpInitFunction,
                                                          /*  in */ PARS_HDLLMODULE             a_hModulHandle);

    PARS_RESULT (DLLCALL* InitWebService)               ( /*  in */ PARS_LPVOID                 a_pWsInfo,
                                                          /*  in */ PARS_FP_INIT_WEBSERVICE     a_fpInitFunction,
                                                          /*  in */ PARS_HDLLMODULE             a_hModuleHandle);

#if PARS_USES_FILTER
    PARS_RESULT (DLLCALL* InitFilter)                   ( /*  in */ PARS_LPSTR                  a_strFilterName,
                                                          /*  in */ PARS_FP_INIT_FILTER         a_fpInitFunction,
                                                          /*  in */ PARS_HDLLMODULE             a_hModulHandle);
#endif /* PARS_USES_FILTER */

    /* Service vTable Header ends here, Service relevant Stuff follows */
    PARS_RESULT (DLLCALL *RegisterApplication)          ( /*  in */ PARS_LPSTR                  a_strUrl,
                                                          /*  in */ PARS_ONNEWREQUEST           a_fpOnNewRequest,
                                                          /*  in */ PARS_CLOSECOMPONENT         a_fpCloseComponent,
                                                          /*  in */ PARS_HDLLMODULE             a_hModuleHandle);

    PARS_RESULT (DLLCALL *RegisterService)              ( /*  in */ PARS_LPSTR                  a_strServiceName,
                                                          /*  in */ PARS_LPVOID                 a_pServiceDescriptor,
                                                          /*  in */ PARS_CLOSECOMPONENT         a_fpCloseComponent,
                                                          /*  in */ PARS_HDLLMODULE             a_hModuleHandle);

    PARS_RESULT (DLLCALL *FreeService)                  ( /*  in */ PARS_LPSTR                  a_strServiceName,
                                                          /*  bi */ PARS_LPVOID*                a_ppService);

    PARS_RESULT (DLLCALL *UnregisterService)            ( /*  in */ PARS_LPSTR                  a_strServiceName);

#if PARS_USES_FILTER
    PARS_RESULT (DLLCALL *HTTP_RegisterFilter)          ( /*  in */ PARS_LPSTR                  a_strFilterAppName,
                                                          /*  in */ PARS_DWORD                  a_dwFlags,
                                                          /*  in */ PARS_FILTERPROC             a_fpFilter,
                                                          /*  in */ PARS_CLOSECOMPONENT         a_fpCloseComponent,
                                                          /*  in */ PARS_HDLLMODULE             a_hModuleHandle);

    PARS_RESULT (DLLCALL *HTTP_CallFilter)              ( /*  in */ PARS_ECB                    a_pECB,
                                                          /*  in */ PARS_DWORD                  a_dwFlags,
                                                          /*  in */ PARS_LPBYTE                 a_lpbyInBuffer,
                                                          /*  bi */ PARS_LPDWORD                a_lpdwInBytes,
                                                          /* out */ PARS_LPBYTE*                a_lppbyOutBuffer,
                                                          /*  bi */ PARS_LPDWORD                a_lpdwOutBytes,
                                                          /* out */ PARS_LPBOOL                 a_pbFreeOutBuffer,
                                                          /*  in */ PARS_BOOL                   a_fLastFilter);
#endif /* PARS_USES_FILTER */
    PARS_RESULT (DLLCALL *LookUpService)                ( /*  in */ PARS_LPSTR                  a_strServiceName,
                                                          /*  bi */ PARS_LPVOID*                a_ppService);

    PARS_RESULT (DLLCALL *LookUpServiceDelayed)         ( /*  in */ PARS_LPSTR                  a_strServiceName,
                                                          /*  in */ PARS_FP_SERVICE_REGISTERED  a_pInformServiceRegistered,
                                                          /*  in */ PARS_LPVOID                 a_pCookie);

    PARS_RESULT (DLLCALL *GetAppCallEntry)              ( /*  in */ PARS_HCONN                  a_ConnId,
                                                          /* out */ PARS_FPVOID*                a_pfpCallEntry,
                                                          /*  bi */ PARS_LPVOID*                a_ppvHandle);

    PARS_RESULT (DLLCALL *FindErrorNode)                ( /*  in */ PARS_WORD                   a_wErrorCode,
                                                          /*  in */ PARS_WORD                   a_wSubCode,
                                                          /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /* out */ PARS_XML_Node*              a_pndeErrorNode,
                                                          /* out */ PARS_LPBOOL                 a_pbDefaultUsed,
                                                          /* out */ PARS_LPBOOL                 a_pbDefaultErrorUsed,
                                                          /* out */ PARS_LPSTR*                 a_pstrErrorCode,
                                                          /* out */ PARS_LPDWORD                a_pdwErrorCodeLength);
#if PARS_SUPPORT_WEBCFG
    PARS_XML_Node (DLLCALL *GetConfigBase)              ( /*  in */ PARS_PT_String              a_mstrApplicationName);

    PARS_XML_Node (DLLCALL *GetSystemSection)           ( /*  in */ PARS_LPSTR                  a_strSectionName);

    PARS_XML_Node (DLLCALL *GetRootNode)                ( PARS_VOID );

    PARS_XML_Sheet(DLLCALL *GetConfigSheet)             ( PARS_VOID );

    PARS_RESULT (DLLCALL *GetConfigDoc)                 ( /* out */PARS_LPSTR*                  a_pstrXMLOrigXMLDoc );

    PARS_RESULT (DLLCALL *GetString )                   ( /*  in */ PARS_LPSTR                  a_strName,
                                                          /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /* out */ PARS_LPSTR*                 a_pstrValue,
                                                          /* out */ PARS_LPDWORD                a_pdwValueLen);

    PARS_RESULT (DLLCALL *CommitConfigXML)              ( PARS_VOID );
#endif /* PARS_SUPPORT_WEBCFG */

    PARS_RESULT (DLLCALL *SetLanguage)                  ( /*  in */ PARS_LPSTR                  a_strLanguage,
                                                          /*  in */ PARS_DWORD                  a_dwLanguageLen);

    PARS_RESULT (DLLCALL *GetLanguage)                  ( /* out */ PARS_LPSTR*                 a_pstrLanguage);

    PARS_RESULT (DLLCALL *GetSystemRoot)                ( /* out */ PARS_LPSTR*                 a_pstrSystemRoot);

    PARS_RESULT (DLLCALL *GetWWWRoot)                   ( /* out */ PARS_LPSTR*                 a_pstrWWWRoot);

    PARS_RESULT (DLLCALL *SetSystemRoot)                ( /*  in */ PARS_LPSTR                  a_strSystemRoot);

    PARS_RESULT (DLLCALL *SetWWWRoot)                   ( /*  in */ PARS_LPSTR                  a_pstrWWWRoot);

#if PARS_DEBUG_OUT
    PARS_RESULT (DLLCALL *Trace)                        ( /*  in */ PARS_TRACE_FILTER           a_tfLevel,
                                                          /*  in */ PARS_LPCSTR                 a_strMessage,
                                                          /*  in */ ...);

    PARS_RESULT (DLLCALL *GetCBForConsoleTrace)         ( /* out */ PARS_DEBUGOUTPUT*           a_pfpConsoleTrace,
                                                          /* out */ PARS_LPVOID*                a_ppvCookie);

    PARS_RESULT (DLLCALL *TraceBuffer)                  ( /*  in */ PARS_TRACE_FILTER           a_tfLevel,
                                                          /*  in */ PARS_LPCSTR                 a_strMessage,
                                                          /*  in */ PARS_DWORD                  a_dwMessageLen);

    PARS_RESULT (DLLCALL *SetTraceCondition)            ( /*  in */ PARS_TRACE_FILTER*          a_aFacilityList,
                                                          /*  in */ PARS_DWORD                  a_dwFacilityCount,
                                                          /*  in */ PARS_DWORD                  a_dwTraceLevel);

    PARS_RESULT (DLLCALL *GetTraceCondition)            ( /* out */ PARS_TRACE_FILTER**         a_paFacilityList,
                                                          /* out */ PARS_LPDWORD                a_pFacilityCount,
                                                          /* out */ PARS_LPDWORD                a_dwTraceLevel);

    PARS_RESULT (DLLCALL *HookTrace)                    ( /*  in */ PARS_TRACE_FP               a_pTraceFunction,
                                                          /*  in */ PARS_SET_CONDITION_FP       a_fpSetTraceLevelFunction,
                                                          /*  in */ PARS_GET_CONDITION_FP       a_fpGetTraceLevelFunction);

    PARS_RESULT (DLLCALL *UnHookTrace)                  ( PARS_VOID );

#endif /* PARS_DEBUG_OUT */
    PARS_RESULT (DLLCALL *RegisterEarlyShutdownInform)  ( /*  in */ PARS_FP_SHUTDOWN_INFORM     a_pEarlyInformCB,
                                                          /*  in */ PARS_LPVOID                 a_pvCookie);
#if PARS_MTHREAD
    PARS_DWORD  (DLLCALL *GetCurrentThreadId)           ( PARS_VOID);

    PARS_VOID   (DLLCALL *ExitThread)                   ( /*  in */ PARS_DWORD                  a_dwExitCode);

    PARS_LPVOID (DLLCALL *CreateThread)                 ( /*  in */ PARS_DWORD                   a_dwStackSize,
                                                          /*  in */ PARSP_LPTHREAD_START_ROUTINE a_lpStartAddress,
                                                          /*  in */ PARS_LPVOID                  a_lpParameter,
                                                          /*  in */ PARS_DWORD                   a_dwCreationFlags,
                                                          /*  in */ PARS_DWORD                   a_dwCreationPriority,
                                                          /*  in */ PARS_DWORD                   a_dwRunningPriority,
                                                          /* out */ PARS_LPDWORD                 a_lpThreadId);

    PARS_VOID   (DLLCALL *CloseHandle)                  ( /*  in */ PARS_LPVOID                  a_WhichHandle);
#endif /* PARS_MTHREAD */

    PARS_RESULT (DLLCALL *CreateCriticalSection)        ( /* out */ PARS_LPVOID*                 a_phCriticalSection );

    PARS_RESULT (DLLCALL *DeleteCriticalSection)        ( /*  in */ PARS_LPVOID                  a_pHandle);

    PARS_RESULT (DLLCALL *EnterCriticalSection)         ( /*  in */ PARS_LPVOID                  a_pHandle);

    PARS_RESULT (DLLCALL *LeaveCriticalSection)         ( /*  in */ PARS_LPVOID                  a_pHandle);

    PARS_RESULT (DLLCALL *CreateSemaphore)              ( /*  in */ PARS_LONG                    a_lInitialCount,
                                                          /*  in */ PARS_LONG                    a_lMaximumCount,
                                                          /* out */ PARS_LPVOID*                 a_phSemaphore);

    PARS_RESULT (DLLCALL *ReleaseSemaphore)             ( /*  in */ PARS_LPVOID                  a_WhichSemaphore,
                                                          /*  in */ PARS_DWORD                   a_dwReleaseCount);

    PARS_RESULT (DLLCALL *WaitForSemaphore)             ( /*  in */ PARS_LPVOID                  a_WhichSemaphore,
                                                          /*  in */ PARS_DWORD                   a_dwMilliseconds);

    PARS_RESULT (DLLCALL *DeleteSemaphore)              ( /*  in */ PARS_LPVOID                  a_WhichSemaphore);

    PARS_RESULT (DLLCALL *MCB_GetMMIVersion)            ( /* out */ PARS_DWORD*                  a_dwMMIVersion);

    PARS_RESULT (DLLCALL *StartupComplete)              ( /*  in */ PARS_VOID );

    PARS_RESULT (DLLCALL *RegisterCompletionCB)         ( /*  in */ PARS_FP_STARTUPCOMPLETE      a_cbComplete);

} PARS_T_MCB, *PARS_PT_MCB;

/*------------------------------------------------------------------------------------------------------------------------*\
|   The one and only MCB definition                                                                                 MCB    |
\*------------------------------------------------------------------------------------------------------------------------*/
extern  PARS_PT_MCB         PARS_g_pCoreContext;
#define MCB                 PARS_g_pCoreContext

#ifdef STATIC_BINDING
  #define DEFINE_MODULE_MCB
#else
  #define DEFINE_MODULE_MCB    PARS_PT_MCB   PARS_g_pCoreContext = PARS_NULL;
#endif

PARS_END_EXTERN_C

#endif /* MCB_H_DEFINED */
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

