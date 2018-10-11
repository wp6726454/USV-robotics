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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_glb.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE-global functions                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_NARE_GLB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* NARE headerfiles */
#include "nare_int.h"            /* internal header */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREFatalError                              +*/
/*+  Input/Output               NARE_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : NARE-fatal-errorcode                            +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via NARE_FATAL_ERROR macro.           +*/
/*+               This function does not return!                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREFatalError(
    NARE_FATAL_ERROR_TYPE  Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line)
{
    LSA_FATAL_ERROR_TYPE   LSAError;

    LSAError.lsa_component_id  = LSA_COMP_ID_NARE;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */
    NARE_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL,"Fatal Error. ModID: %d, Line: %d, Error: %Xh",ModuleID,Line,Error);
    NARE_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);

    /* should not return! */
    while (1) ;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARERQBSetError                             +*/
/*+  Input                      NARE_UPPER_RQB_PTR_TYPE      pRQB           +*/
/*+                             LSA_UINT16                   LSACompID      +*/
/*+                             LSA_OPCODE_TYPE              Opcode         +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets local NARE LL-Error in RQB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARERQBSetError(
    NARE_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16                LSACompID,
    LSA_OPCODE_TYPE           Opcode,
    LSA_UINT16                Response)
{
	LSA_UNUSED_ARG(pRQB);
	LSA_UNUSED_ARG(LSACompID);
	LSA_UNUSED_ARG(Opcode);
	LSA_UNUSED_ARG(Response);
return;
#if 0
	NARE_LOCAL_ERROR_CODE_TYPE ErrorCode;
    /* check if structure fit into array */
    if ( sizeof(NARE_LOCAL_ERROR_CODE_TYPE) <= sizeof(pRQB->err.error_code) )
    {   /* because the error_code-array may not be propably aligned we */
        /* have to use a temporary structure and copy the content byte */
        /* by byte into the array! We can not cast!                    */
        ErrorCode.opcode   = Opcode;
        ErrorCode.response = Response;
        NARE_MEMCOPY_COMMON(
            (NARE_COMMON_MEM_PTR_TYPE)pRQB->err.error_code,
            (NARE_COMMON_MEM_PTR_TYPE)&ErrorCode,
            sizeof(NARE_LOCAL_ERROR_CODE_TYPE));
        pRQB->err.lsa_component_id = LSACompID;
        pRQB->err.error_code_id    = 0; /* unused */
    }
#endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NARECallCbf                                 +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Cbf                  : Pointer to Callbackfunction                     +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calls Cbf with RQB.                                       +*/
/*+               if Cbf is LSA_NULL Cbf is not called.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NARECallCbf(
    NareUpperCbf                 Cbf,
    NARE_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE             pSys)
{
    NARE_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,"IN : NARECallCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",Cbf,pRQB,(NARE_RQB_GET_RESPONSE(pRQB)));
#if NARE_CFG_TRACE_RQB_ERRORS
    switch ( NARE_RQB_GET_RESPONSE(pRQB))
    {
        case NARE_RSP_OK:
        case NARE_RSP_OK_CANCEL:
            break;

        default:
            NARE_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH,"RQB-Response error [opcode: 0x%X, Handle: 0x%X, Resp: 0x%X]",NARE_RQB_GET_OPCODE(pRQB),NARE_RQB_GET_HANDLE(pRQB),NARE_RQB_GET_RESPONSE(pRQB));
            break;
    }
#endif
    if (!LSA_HOST_PTR_ARE_EQUAL(Cbf,LSA_NULL))
    {   /* AP00999037: call macro instead of: Cbf(pRQB); */
        NARE_REQUEST_UPPER_DONE(Cbf,pRQB,pSys);
    }
    NARE_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"OUT: NARECallCbf()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREUserRequestFinish                       +*/
/*+  Input/Output               NARE_HDB_USER_PTR_TYPE      pHDB            +*/
/*+                             NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             NARE_RSP                    Response        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+  Response             : Response to set in RQB                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishs Request by calling call-back-function located in  +*/
/*+               HDB. if Cbf is LSA_NULL noting is done.                   +*/
/*+                                                                         +*/
/*+               Fill the RQB-local-Err-Structure (err) with local NARE    +*/
/*+               error if lsa_componente_id is LSA_COMP_ID_UNUSED.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREUserRequestFinish(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_UPPER_RQB_PTR_TYPE     pRQB,
    NARE_RSP                    Response)
{
    NARE_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : NAREUserRequestFinish(pHDB: 0x%X,pRQB: 0x%X,Response: 0x%X)",pHDB,pRQB,Response);
    NARE_RQB_SET_RESPONSE(pRQB,Response);
    NARE_RQB_SET_HANDLE(pRQB, pHDB->Params.HandleUpper );
    NARE_UPPER_TRACE_04(pHDB->TraceIdx, LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. pRQB: 0x%X, 0x%X, Handle: 0x%Xh,Response: 0x%X",pRQB,NARE_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,Response);
    if (NARE_RQB_GET_OPCODE(pRQB) == NARE_OPC_RESOLVE_CONFIG)
    {
            NARE_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"result of finish RESULT_CONFIG setequal %d Alias Used %d extended mode %d ",
                                                                        pRQB->args.ResConf.StationNameSetEqual,pRQB->args.ResConf.AliasUsed, pRQB->args.ResConf.ExtendedMode);
        if ((pRQB->args.ResConf.AllowStationNameOverwrite) && (Response == NARE_RSP_OK)) pRQB->args.ResConf.StationNameSetEqual = LSA_TRUE;
    }
    NARECallCbf(pHDB->Params.Cbf,pRQB,pHDB->Params.pSys);
    NARE_FUNCTION_TRACE_00(pHDB->TraceIdx, LSA_TRACE_LEVEL_CHAT,"OUT: NAREUserRequestFinish()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREGetUserHDBFromHandle                    +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    NARE_HDB_USER_PTR_TYPE                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE user -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to User-HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_HDB_USER_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    NARE_HDB_USER_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    if ( NARE_HANDLE_IS_USER(Handle))
    {
        Idx = (LSA_UINT16) NARE_HANDLE_IDX_USER(Handle);   /* Get User-Idx */
        if ( Idx < NARE_MGM_GET_HUSER_PTR()->HandleCnt )
        {
            pHDB = &(NARE_MGM_GET_HUSER_PTR()->HDB[Idx]);
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid user handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid user handle (0x%X). No user handle.",Handle);
        pHDB = LSA_NULL;
    }
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREGetDcpHDBFromHandle                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    NARE_HDB_DCP_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE dcp -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to DCP -HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_HDB_DCP_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetDcpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    NARE_HDB_DCP_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    if ( NARE_HANDLE_IS_DCP(Handle))
    {
        Idx = (LSA_UINT16) NARE_HANDLE_IDX_DCP(Handle);   /* Get DCP-Idx */
        if ( Idx < NARE_MGM_GET_HDCP_PTR()->HandleCnt )
        {
            pHDB = &(NARE_MGM_GET_HDCP_PTR()->HDB[Idx]);
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid DCP handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid DCP handle (0x%X). No DCP handle.",Handle);
        pHDB = LSA_NULL;
    }
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREGetEddHDBFromHandle                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    NARE_HDB_EDD_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE edd -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to EDD -HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_HDB_EDD_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetEddHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    NARE_HDB_EDD_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    if ( NARE_HANDLE_IS_EDD(Handle))
    {
        Idx = (LSA_UINT16) NARE_HANDLE_IDX_EDD(Handle);   /* Get EDD-Idx */
        if ( Idx < NARE_MGM_GET_HEDD_PTR()->HandleCnt )
        {
            pHDB = &(NARE_MGM_GET_HEDD_PTR()->HDB[Idx]);
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid EDD handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid EDD handle (0x%X). No EDD handle.",Handle);
        pHDB = LSA_NULL;
    }
    return(pHDB);
}

#ifndef NARE_CFG_NO_SOCK
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREGetSockHDBFromHandle                    +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    NARE_HDB_SOCK_PTR_TYPE                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : NARE SOCK -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to SOCK-HDB for Handle.                      +*/
/*+               Handle has to be a SOCK-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

NARE_HDB_SOCK_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{

    NARE_HDB_SOCK_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    if ( NARE_HANDLE_IS_SOCK(Handle))
    {
        Idx = (LSA_UINT16) NARE_HANDLE_IDX_SOCK(Handle);   /* Get User-Idx */

        if ( Idx < NARE_MGM_GET_HSOCK_PTR()->HandleCnt )
        {
            pHDB = &(NARE_MGM_GET_HSOCK_PTR()->HDB[Idx]);
        }
        else
        {
            NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        NARE_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). No SOCK handle.",Handle);
        pHDB = LSA_NULL;
    }
    return(pHDB);
}
#endif

/*****************************************************************************/
/*  end of file NARE_GLB.C                                                    */
/*****************************************************************************/

