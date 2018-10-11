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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_lib.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA-global functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  2
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_LIB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FatalError                              +*/
/*+  Input/Output               OHA_FATAL_ERROR_TYPE Error                  +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : OHA-fatal-errorcode                             +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via OHA_FATAL_ERROR macro.            +*/
/*+               This function does not return!                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FatalError(
    OHA_FATAL_ERROR_TYPE   Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line)

{
    LSA_FATAL_ERROR_TYPE    LSAError;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : OHA_FatalError()");
    LSAError.lsa_component_id  = LSA_COMP_ID_OHA;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */

    OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL,"OUT: OHA_FatalError. ModID: %d, Line: %d, Error: %Xh",ModuleID,Line,Error);

    OHA_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CallCbf                                 +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
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
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_CallCbf(
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf,
    OHA_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE            pSys)
{
    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CallCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",
                          Cbf,pRQB,(OHA_RQB_GET_RESPONSE(pRQB)));

#ifdef OHA_CFG_TRACE_RQB_ERRORS
    switch ( OHA_RQB_GET_RESPONSE(pRQB))
    {
        case OHA_OK:
        case OHA_OK_CANCEL:
            break;
        default:
            OHA_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH,"RQB-Response error [opcode: 0x%X, Handle: 0x%X, Resp: 0x%X]",OHA_RQB_GET_OPCODE(pRQB),OHA_RQB_GET_HANDLE(pRQB),OHA_RQB_GET_RESPONSE(pRQB));
            break;
    }
#endif

    if ( ! LSA_HOST_PTR_ARE_EQUAL(Cbf,LSA_NULL) )
    {
        OHA_REQUEST_UPPER_DONE(Cbf, pRQB, pSys);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CallCbf()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RQBSetResponse                          +*/
/*+  Input                      OHA_UPPER_RQB_PTR_TYPE       pRQB           +*/
/*+                             LSA_UINT16                   Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets Response and if lsa_component_id is not set our      +*/
/*+               response in err-structure.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_RQBSetResponse(
    OHA_UPPER_RQB_PTR_TYPE    pRQB,
    LSA_UINT16                Response)
{
    OHA_RQB_SET_RESPONSE(pRQB, Response);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RQBSetResponse(pRQB: 0x%X, Response: 0x%X)",
                          pRQB,Response);

    if (( Response != OHA_OK ) &&
        ( Response != OHA_OK_CANCEL) &&
        /* RemaRead and BufferOverflow is not an error ! */
        (!(Response == OHA_ERR_BUFFER_OVERFLOW && OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_REMA_READ)) &&
        /* PrmRead and BufferOverflow is not an error ! */
        (!(Response == OHA_ERR_BUFFER_OVERFLOW && OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_READ))
       )
    {
		OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"OUT: OHA_RQBSetResponse. LSA_COMP_ID_OHA, Opcode: %d, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),Response);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RQBSetResponse()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserRequestFinish                       +*/
/*+  Input/Output               OHA_HDB_USER-PTR_TYPE       pHDB            +*/
/*+                             OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             LSA_UINT16                  Response        +*/
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
/*+               Fill the RQB-local-Err-Structure (err) with local OHA     +*/
/*+               error if lsa_componente_id is LSA_COMP_ID_UNUSED.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UserRequestFinish(
    OHA_HDB_USER_PTR_TYPE      pHDB,
    OHA_UPPER_RQB_PTR_TYPE     pRQB,
    LSA_UINT16                 Response)
{
    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UserRequestFinish(pHDB: 0x%X,pRQB: 0x%X,Response: 0x%X)",
                          pHDB,pRQB,Response);

    OHA_RQBSetResponse(pRQB,Response);
    OHA_RQB_SET_HANDLE(pRQB, pHDB->Params.HandleUpper );

    OHA_UPPER_TRACE_04(0, LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. Opcode: 0x%X, Handle: 0x%Xh,UpperHandle: 0x%X, Response: 0x%X",OHA_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,OHA_RQB_GET_HANDLE(pRQB),Response);

    OHA_CallCbf(pHDB->Params.Cbf,pRQB,pHDB->Params.pSys);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_UserRequestFinish()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetUserHDBFromHandle                    +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_USER_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA user -channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to valid User-HDB for handle.                +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_USER_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHA_GetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_USER_PTR_TYPE pHDB;
    LSA_UINT16        Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetUserHDBFromHandle(Handle: 0x%X)",
                          Handle);
    /* Idx = (LSA_UINT16) (Handle);  */ /* Get User-Idx */

    /* if ( Idx < OHA_GET_GLOB_PTR()->UserCnt )  */
    if ( OHA_HANDLE_IS_USER(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get User-Idx */

        if ( Idx < OHA_CFG_MAX_CHANNELS /*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HUSER_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid USER handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }

        /* Handle must be in use (<> free) */
        if (OHA_IS_NOT_NULL(pHDB) && pHDB->State == OHA_HANDLE_STATE_FREE )
        {
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid user handle (0x%X). Out of range.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_GetUserHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetEddHDBFromHandle                      +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_EDD_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA EDD-channel handle to get pointer to HDB for.         +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to EDD -HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_EDD_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetEddHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_EDD_PTR_TYPE pHDB;
    LSA_UINT16           Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetEddHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_EDD(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get EDD-Idx */

        if ( Idx < OHA_CFG_MAX_NICS/*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HEDD_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid EDD handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid EDD handle (0x%X). No EDD handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetEddHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetDcpHDBFromHandle                      +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_DCP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA DCP-channel handle to get pointer to HDB for.         +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to DCP -HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_DCP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetDcpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_DCP_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetDcpHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_DCP(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get DCP-Idx */

        if ( Idx < OHA_CFG_MAX_NICS/*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HDCP_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid DCP handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid DCP handle (0x%X). No DCP handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetDcpHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetNareHDBFromHandle                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_NARE_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA NARE-channel handle to get pointer to HDB for.        +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to NARE-HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef OHA_CFG_NO_NARE
OHA_HDB_NARE_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetNareHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_NARE_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetNareHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_NARE(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get NARE-Idx */

        if ( Idx < OHA_CFG_MAX_NICS/*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HNARE_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid NARE handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid NARE handle (0x%X). No NARE handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetNareHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetLldpHDBFromHandle                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_LLDP_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA LLDP-channel handle to get pointer to HDB for.        +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to LLDP-HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_LLDP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetLldpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_LLDP_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetLldpHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_LLDP(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get LLDP-Idx */

        if ( Idx < OHA_CFG_MAX_NICS/*OHA_GET_GLOB_PTR()->LowerCnt*/ )
        {
            pHDB = OHA_GET_HLLDP_PTR(Idx);
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid LLDP handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid LLDP handle (0x%X). No LLDP handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetLldpHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetSockHDBFromHandle                     +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA SOCK-channel handle to get pointer to HDB for.        +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to SOCK-HDB for Handle.                      +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_SOCK_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_SOCK_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetSockHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_SOCK(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get SOCK-Idx */

        if ( Idx == 0 ) /* one SOCK channel only */
        {
            pHDB = OHA_GET_HSOCK_PTR();
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SOCK handle (0x%X). No SOCK handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetSockHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHAGetSnmpxHDBFromHandle                    +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    OHA_HDB_SNMPX_PTR_TYPE                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA SNMPX-channel handle to get pointer to HDB for.       +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to SNMPX-HDB for Handle.                     +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef OHA_CFG_USE_SNMPX
OHA_HDB_SNMPX_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetSnmpxHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{
    OHA_HDB_SNMPX_PTR_TYPE pHDB;
    LSA_UINT16             Idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAGetSnmpxHDBFromHandle(Handle: 0x%X)",
                          Handle);
    if ( OHA_HANDLE_IS_SNMPX(Handle))
    {
        Idx = (LSA_UINT16) OHA_HANDLE_IDX(Handle);   /* Get SNMPX-Idx */

        if ( Idx == 0 ) /* one SOCK channel only */
        {
            pHDB = OHA_GET_HSNMPX_PTR();
        }
        else
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SNMPX handle (0x%X). Out of range.",Handle);
            pHDB = LSA_NULL;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Invalid SNMPX handle (0x%X). No SNMPX handle.",Handle);
        pHDB = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAGetSnmpxHDBFromHandle(pHDB: 0x%X)",
                          pHDB);
    return(pHDB);
}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CancelDcpControlUser                    +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel controls a Dcp-Server then delete    +*/
/*+               this entry.                                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_CancelDcpControlUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_DCP_PTR_TYPE        pHDBDcp  = OHA_GET_HDCP_PTR(0);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CancelDcpControlUser(pHDBUser: 0x%X)",
                          pHDBUser);
    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_NICS)
    {
        /* is a DcpSet controlled by the user ? */
        if (pHDBDcp && pHDBDcp->DcpState == OHA_HANDLE_STATE_OPEN && /* dcp-channel is opened ? */
            (LSA_HOST_PTR_ARE_EQUAL(pHDBUser,pHDBDcp->pHDBUserCtrl)) )
        {
            pHDBDcp->pHDBUserCtrl = LSA_NULL;              /* delete this entry */
        }
        i++;       /* indicate the next user-channel */
        pHDBDcp++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CancelDcpControlUser()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckDcpControlUser                     +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE        pHDBUser       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to USER-HDB                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If this User-Channel controls a Dcp-Server and a          +*/
/*+               OHA_OPC_DCP_RESPONSE is active -> returns LSA_TRUE        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckDcpControlUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_DCP_PTR_TYPE        pHDBDcp  = OHA_GET_HDCP_PTR(0);
    LSA_BOOL                    RetVal   = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckDcpControlUser(pHDBUser: 0x%X)",
                          pHDBUser);
    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_NICS)
    {
        /* is a DcpSet controlled by the user ? */
        if (OHA_IS_NOT_NULL(pHDBDcp) && pHDBDcp->DcpState == OHA_HANDLE_STATE_OPEN && /* dcp-channel is opened ? */
            (LSA_HOST_PTR_ARE_EQUAL(pHDBUser,pHDBDcp->pHDBUserCtrl)) )
        {
        }
        i++;       /* indicate the next user-channel */
        pHDBDcp++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_CheckDcpControlUser()");
    return RetVal;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpSetAllNics                           +*/
/*+  Input/Output          :    LSA_UINT8         Mode                      +*/
/*+                             LSA_UINT16        Options                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+ parameters:     Mode (OHA_DCP_LOCK_SET, OHA_DCP_UNLOCK_SET)             +*/
/*+                   LockSetStatus (bit-coded):                            +*/
/*+                   OHA_CHANGE_OF_STATION_ID,                             +*/
/*+                   OHA_CHANGE_OF_STATION_TYPE                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Station-Type and Station-Id are NIC-global - set the      +*/
/*+               LockStatus for all NICs.                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpSetAllNics(
    LSA_UINT8       Mode,
    LSA_UINT32      Options
)
{
    LSA_UINT16                  i        = 0;
    OHA_HDB_DCP_PTR_TYPE        pHDBDcp  = OHA_GET_HDCPR_PTR(0);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DcpSetAllNics(Mode: 0x%X, Options: 0x%X)",
                          Mode, Options);
    LSA_UNUSED_ARG(Options);
    /* look for used User-Channels */
    while ( i < OHA_CFG_MAX_NICS)
    {
        if (OHA_IS_NOT_NULL(pHDBDcp) && pHDBDcp->pDB) /* dcp-channel exists ? */
        {
            switch (Mode)
            {
                    /* case OHA_DCP_LOCK_SET:                      */
                    /*  pHDBDcp->pDB->LockSetStatus |= Options;    */  /* the specific bit is set */
                    /* break;                                      */
                    /* case OHA_DCP_UNLOCK_SET:                    */
                    /*  pHDBDcp->pDB->LockSetStatus &= (~Options); */ /* the specific bit is deleted */
                    /* break;                                      */
                    /* default:                                    */
                    /*  OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);     */
                    /* break;                                      */
            }
        }
        i++;       /* indicate the next user-channel */
        pHDBDcp++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DcpSetAllNics()");
    return;
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TOPOGetNicByInterface                   +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_UPPER_DB_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a Database-Interface-Entry of by NICId.   +*/
/*+  If none is found LSA_NULL is returned                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TOPOGetNicByInterface(
    LSA_UINT32 InterfaceID)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;
    LSA_UINT16            NicId = OHA_INVALID_NIC_ID;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TOPOGetNicByInterface(InterfaceID: 0x%X)",
                          InterfaceID);

    if (InterfaceID >= EDD_INTERFACE_ID_MIN && InterfaceID <= EDD_INTERFACE_ID_MAX) /* 1 ... 16, see EDD */
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
        {
            if (pDB->InterfaceID == InterfaceID)
            {
                Found = LSA_TRUE;
                NicId = pDB->NicId;
            }
            else
            {
                pDB++;
                i++;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_TOPOGetNicByInterface(NicId: 0x%X)", NicId);
    return(NicId);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_TOPOIsIfRegistered                      +*/
/*+  Input/Output          :    LSA_UINT32      InterfaceID                 +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  LSA_FALSE - interface is not registered by a TopoControl               +*/
/*+  LSA_TRUE  - interface registered, an EddGetPortParams is done          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_TOPOIsIfRegistered(
    LSA_UINT32 InterfaceID)
{
    LSA_UINT32            i;
    LSA_BOOL              Found = LSA_FALSE;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TOPOIsIfRegistered(InterfaceID: 0x%X)",
                          InterfaceID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
		LSA_UINT16 NicID = OHA_TOPOGetNicByInterface(InterfaceID);

		if (NicID != OHA_INVALID_NIC_ID)
		{
			OHA_ASSERT_NULL_PTR(pHDBUser);

			if ( (pHDBUser->State != OHA_HANDLE_STATE_FREE) && /* user channel opened ? */
				  OHA_IS_NOT_NULL(pHDBUser->Params.pDetail))
			{
				if ( pHDBUser->TopoState != OHA_TOPO_STATE_CLOSE )
				{
					if (NicID == pHDBUser->Params.pDetail->Params.User.NICId) /* has this NIC */
					{
						if (Found)  /* interface is registered */
						{
							OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_TOPOIsIfRegistered: Interface (ID: 0x%X) is registered twice", InterfaceID);
						}
						Found = LSA_TRUE;
						break;
					}
				}
			}
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_TOPOIsIfRegistered(Found: 0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsAnyLocalSetActive                     +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  LSA_FALSE - no local set (IP, NameOfStation) is active by any user     +*/
/*+  LSA_TRUE  - local set (IP, NameOfStation) is active by any user        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_IsAnyLocalSetActive(
    LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    LSA_BOOL              Found = LSA_FALSE;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsAnyLocalSetActive(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        OHA_UPPER_RQB_PTR_TYPE   pRQB = pHDBUser->SockUserReq.pBottom;
        if (OHA_IS_NOT_NULL(pRQB))
        {
			OHA_ASSERT_NULL_PTR(pHDBUser->Params.pDetail);

			/* AP01022057: set only the user with the same Nic */
			if(NicID == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				switch (OHA_RQB_GET_OPCODE(pRQB))
				{
					case OHA_OPC_PRM_COMMIT:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_PRM_COMMIT) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_SET_ADDRESS:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_SET_ADDRESS) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_SET_EMERGENCY:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_SET_EMERGENCY) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_DCP_RESPONSE:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_DCP_RESPONSE) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_DCP_WRITE_ELEMENT:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_DCP_WRITE_ELEMENT) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_PRM_WRITE:
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Local Set (OHA_OPC_PRM_WRITE) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					case OHA_OPC_DCP_CONTROL:  /* AP01335332. Lock IP */
						OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetActive: Lock IP (OHA_OPC_DCP_CONTROL) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
						Found = LSA_TRUE;
					break;
					default:
						OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_FATAL,"OHA_IsAnyLocalSetActive: Invalid Opcode (0x%X)", OHA_RQB_GET_OPCODE(pRQB));
						OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
					break;
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_IsAnyLocalSetActive(Found: 0x%X)", Found);
    return(Found);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsAnyLocalSetMib2Active                 +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  LSA_FALSE - no local set (SysName, SysLocation or SysContact) is       +*/
/*+  active by any user                                                     +*/
/*+  LSA_TRUE  - local set (SysXyz) is active by any user                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_IsAnyLocalSetMib2Active(
    LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    LSA_BOOL              Found = LSA_FALSE;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsAnyLocalSetMib2Active(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        OHA_UPPER_RQB_PTR_TYPE   pRQB = pHDBUser->Mib2UserReq.pBottom;
        if (OHA_IS_NOT_NULL(pRQB))
        {
            switch (OHA_RQB_GET_OPCODE(pRQB))
            {
                case OHA_OPC_PRM_COMMIT:
                    if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
                    {
                        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetMib2Active: Local Set (OHA_OPC_PRM_COMMIT) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
                        Found = LSA_TRUE;
                    }
                    break;
                case OHA_OPC_PRM_WRITE:
                    if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
                    {
                        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetMib2Active: Local Set (OHA_OPC_PRM_WRITE) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
                        Found = LSA_TRUE;
                    }
                    break;
                case OHA_OPC_LLDP_WRITE_OPTION:
                    if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
                    {
                        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalSetMib2Active: Local Set (OHA_OPC_LLDP_WRITE_OPTION) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
                        Found = LSA_TRUE;
                    }
                    break;
                default:
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_FATAL,"OHA_IsAnyLocalSetMib2Active: Invalid Opcode (0x%X)", OHA_RQB_GET_OPCODE(pRQB));
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_IsAnyLocalSetMib2Active(Found: 0x%X)", Found);
    return(Found);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsAnyLocalCtrlTcIpActive                +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  LSA_FALSE - no local control (TimeParams) is                           +*/
/*+  active by any user                                                     +*/
/*+  LSA_TRUE  - local control is active by any user                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_IsAnyLocalCtrlTcIpActive(
    LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    LSA_BOOL              Found = LSA_FALSE;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsAnyLocalCtrlTcIpActive(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        OHA_UPPER_RQB_PTR_TYPE   pRQB = pHDBUser->TcIpUserReq.pBottom;
        if (OHA_IS_NOT_NULL(pRQB))
        {
            switch (OHA_RQB_GET_OPCODE(pRQB))
            {
                case OHA_OPC_PRM_READ:
                case OHA_OPC_PRM_WRITE:
					if(pRQB->args.PrmData.record_index == OHA_RECORD_INDEX_TCP_KEEP_ALIVE)
					{
						if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
						{
							OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalCtrlTcIpActive: Local Set (OHA_OPC_PRM_READ/WRITE) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
							Found = LSA_TRUE;
						}
					}
                    break;
                case OHA_OPC_TCIP_CONTROL:
                    if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
                    {
                        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_ERROR,"OHA_IsAnyLocalCtrlTcIpActive: Local Set (OHA_OPC_READ/WRITE_TCIP_DATA) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
                        Found = LSA_TRUE;
                    }
                    break;
                default:
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_FATAL,"OHA_IsAnyLocalCtrlTcIpActive: Invalid Opcode (0x%X)", OHA_RQB_GET_OPCODE(pRQB));
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                    break;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_IsAnyLocalCtrlTcIpActive(Found: 0x%X)", Found);
    return(Found);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_IsAnyPrmCommitActive                    +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    OHA_HDB_USER_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  LSA_NULL     - no OHA_OPC_PRM_COMMIT is queued                         +*/
/*+  <> LSA_NULL  - one OHA_OPC_PRM_COMMIT is queued                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
OHA_HDB_USER_PTR_TYPE OHA_IsAnyPrmCommitActive(
    LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    OHA_HDB_USER_PTR_TYPE pHDBFound = LSA_NULL;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IsAnyPrmCommitActive(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        OHA_UPPER_RQB_PTR_TYPE pRQB = pHDBUser->TopoUserReq.pBottom;
        if (OHA_IS_NOT_NULL(pRQB))
        {
            switch (OHA_RQB_GET_OPCODE(pRQB))
            {
                case OHA_OPC_PRM_COMMIT:
                    if (NicID == pHDBUser->Params.pDetail->Params.User.NICId)
                    {
                        if (OHA_IS_NOT_NULL(pHDBFound))
                        {
                            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                        }
                        pHDBFound = pHDBUser;
                        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"OHA_IsAnyPrmCommitActive: Local REMA Set (OHA_OPC_PRM_COMMIT) is active by the user (0x%X) NicID (0x%X)", i+1, NicID);
                    }
                    break;
                default:
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"OHA_IsAnyPrmCommitActive: Queued Opcode (0x%X)", OHA_RQB_GET_OPCODE(pRQB));
                    break;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_IsAnyPrmCommitActive(pHDBFound: 0x%X)", pHDBFound);
    return(pHDBFound);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DoLocalSetAddress                       +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    -                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NicID                                                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:                                                           +*/
/*+  calls a queued local set address request                               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_DoLocalSetAddress(LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    OHA_UPPER_RQB_PTR_TYPE pRQB;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DoLocalSetAddress(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        if (OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))
        {
			OHA_ASSERT_NULL_PTR(pHDBUser->Params.pDetail);

			/* AP01022057: set only the user with the same Nic */
			if(NicID == pHDBUser->Params.pDetail->Params.User.NICId)
			{
				while(OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))
				{
					OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->SockUserReq.pBottom,
											 pHDBUser->SockUserReq.pTop,
											 pRQB);

					switch (OHA_RQB_GET_OPCODE(pRQB))
					{
						case OHA_OPC_SET_ADDRESS:
						case OHA_OPC_SET_EMERGENCY:
							OHA_RequestSetAddress(pRQB, pHDBUser);
						break;
						case OHA_OPC_DCP_RESPONSE:
							OHA_RequestDcpResponse(pRQB, pHDBUser);
						break;
						case OHA_OPC_DCP_WRITE_ELEMENT:
							OHA_RequestWriteDcpElement(pRQB, pHDBUser);
						break;
						case OHA_OPC_PRM_COMMIT:
							OHA_RequestPrmCommit(pRQB, pHDBUser);
						break;
						case OHA_OPC_PRM_WRITE:
							OHA_PrmWriteStation(pRQB, pHDBUser);
						break;
						case OHA_OPC_DCP_CONTROL: /* AP01335332. Lock IP */
						{
							LSA_UINT16 Response = OHA_DcpWriteUserControl(NicID, (LSA_HANDLE_TYPE)(OHA_HANDLE_IDX(pHDBUser->ThisHandle)), &pRQB->args.DcpControl);
							if(OHA_IS_NOT_NULL(pRQB->args.DcpControl.pAddrInfo))
							{
								(LSA_UINT16)OHA_WriteAddrInfo(NicID, pRQB->args.DcpControl.pAddrInfo);
							}
							OHA_UserRequestFinish(pHDBUser,pRQB,Response);
						}
						break;
						default:
							OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_FATAL,"OHA_DoLocalSetAddress: Invalid Opcode (0x%X)", OHA_RQB_GET_OPCODE(pRQB));
							OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
						break;
					}
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DoLocalSetAddress()");
}

/*------------------------------------------------------------------------------
// validate station-name
     *
     * syntax:
     *
     * 1 or more labels, separated by [.]
     * total length is 1-240
     * label-length is 1-63
     * labels consist of [a-z0-9-]
     * labels do not start with [-]
     * labels do not end with [-]
     * the first (leftmost) label does not have the form "port-xyz" with x, y, z = 0...9
     * the first (leftmost) label does not have the form "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
     * station-names do not have the form n.n.n.n, n = 0...999
     * labels do only start with 'xn-' if RFC 3490 is applied
     *
The following syntax is reserved by PNIO:
 i) remote SET (name_interpertation = LSA_FALSE): default (norm convention), no further interpretation
ii) local  SET (name_interpertation = LSA_TRUE):  station-name is valid if
    the last (rightmost) label match with "--ab-cd-ef-gh-ij-kl"  with a ...l consisting of [a-f0-9-]
//----------------------------------------------------------------------------*/
LSA_BOOL  OHA_LOCAL_FCT_ATTR  oha_is_valid_station_name(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name,
    LSA_BOOL allow_upper_case,
    LSA_BOOL name_interpretation
)
{
    LSA_UINT16 idx;
    LSA_UINT16 lbl;
    LSA_UINT16 nr_of_labels;
    LSA_BOOL looks_like_ipaddr;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_is_valid_station_name(length: 0x%X, name: 0x%X, allow_upper_case:  0x%X, name_interpretation:  0x%X)",
                          length, name, allow_upper_case, name_interpretation);
    if ( ! ( name
             && length >= 1
             && length <= 240
           ) )
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(invalid length, 1...240): LSA_FALSE");
        return LSA_FALSE;
    }

    nr_of_labels = 0;

    looks_like_ipaddr = LSA_TRUE;

    lbl = 0; /* start of label */

    for ( idx = 0; idx <= length; ++idx )
    {

        int/*!*/ ch;

        if ( idx < length )
        {
            ch = name[idx];
        }
        else
        {
            ch = '.'; /* fake a dot at the end */
        }

        /***/

        if ( ch == '.' )
        {

            LSA_UINT16 len = (LSA_UINT16)(idx - lbl);

            if ( len < 1 || len > 63 )
            {
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(label has either 0 or more than 63 chars): LSA_FALSE");
                return LSA_FALSE; /* label has either 0 or more than 63 chars */
            }

            OHA_ASSERT(idx > 0);

            if ( name[idx - 1] == '-' )
            {
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(label ends with '-'): LSA_FALSE");
                return LSA_FALSE; /* label ends with '-' */
            }

            if ( len > 3 )
            {

                looks_like_ipaddr = LSA_FALSE; /* label has more than 3 characters */

                if ( nr_of_labels == 0 )
                {
                    if ( len == 8 || len == 14 )
                    {
                        if (oha_is_valid_port_name(len, &name[lbl]))
                        {
                            /* the first (leftmost) label match with a port-name */
                            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(the first (leftmost) label match with a port-name): LSA_FALSE");
                            return LSA_FALSE;
                        }
                    }
                }
            }

            nr_of_labels++;

            lbl = (LSA_UINT16)(idx + 1); /* new start of label */

            if (name_interpretation)
            {
                /* last label matches with "--ab-cd-ef-gh-ij-kl" ? */
                if ((length > lbl) && ((length - lbl) == (2 + 17)))
                {
                    if (name[lbl] == '-' && name[lbl+1] == '-')
                    {
                        if (oha_is_valid_mac_string((LSA_UINT16)(length - lbl - 2), &name[lbl+2], allow_upper_case))
                        {
                            /* the last (rightmost) label match with a valid mac-string - o.k. */
                            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(the last (rightmost) label match with a mac-string: LSA_TRUE");
                            return LSA_TRUE;
                        }
                    }
                }
            }

            continue;
        }

        if ( ch >= '0' && ch <= '9' )
        {
            continue;
        }

        looks_like_ipaddr = LSA_FALSE; /* not only digits */

        if ( ch >= 'a' && ch <= 'z' )
        {
            continue;
        }

        /* v3.3i2: for compatibility to v2 */
        if ( allow_upper_case )
        {
            if ( ch >= 'A' && ch <= 'Z' )
            {
                continue;
            }
        }

        if ( idx == lbl )
        {
            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(label does not start with letter or digit): LSA_FALSE");
            return LSA_FALSE; /* label does not start with letter or digit */
        }

        if ( ch == '-' )
        {
            continue;
        }

        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(illegal char): LSA_FALSE");
        return LSA_FALSE; /* illegal char */
    }

    if ( looks_like_ipaddr && nr_of_labels == 4 )
    {

        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(looks like an ip-addr): LSA_FALSE");
        return LSA_FALSE; /* looks like an ip-addr */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_station_name(): LSA_TRUE");
    return LSA_TRUE; /* ok */
}


/*------------------------------------------------------------------------------
// validate port-name
     *
     * syntax:
     *
     * the first (leftmost) label match with "port-xyz" with x, y, z = 0...9
     * the first (leftmost) label match with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
     *
//----------------------------------------------------------------------------*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_is_valid_port_name(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name
	)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_is_valid_port_name(length: 0x%X, name: 0x%X)",
                          length, name);

    if (OHA_IS_NOT_NULL(name))
    {
        if ( (length == 8 || length == 14)
             && name[0] == 'p'
             && name[1] == 'o'
             && name[2] == 'r'
             && name[3] == 't'
             && name[4] == '-'
             && name[5] >= '0' && name[5] <= '9'
             && name[6] >= '0' && name[6] <= '9'
             && name[7] >= '0' && name[7] <= '9' )
        {

            if (length == 8)
            {               /* the first (leftmost) label match with "port-xyz" with x, y, z = 0...9 */
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_port_name(the first (leftmost) label match with 'port-xyz' with x, y, z = 0...9): LSA_TRUE");
                return LSA_TRUE;
            }

            if (length == 14
                && name[8] == '-'
                && name[9] >= '0' && name[9] <= '9'
                && name[10] >= '0' && name[10] <= '9'
                && name[11] >= '0' && name[11] <= '9'
                && name[12] >= '0' && name[12] <= '9'
                && name[13] >= '0' && name[13] <= '9'
               )
            {
                /* the first (leftmost) label match with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9 */
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_port_name(the first (leftmost) label match with 'port-xyz-abcde' with x, y, z, a, b, c, d, e = 0...9): LSA_TRUE");
                return LSA_TRUE;
            }
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_port_name(): LSA_FALSE");
    return LSA_FALSE; /* not ok */
}


/*------------------------------------------------------------------------------
// validate mac-string
     *
     * syntax:
     *
     * the last (rightmost) label match with "ab-cd-ef-gh-ij-kl" with a...l consists of [a-f,0-9]
     *
//----------------------------------------------------------------------------*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_is_valid_mac_string(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name,
    LSA_BOOL allow_upper_case)
{
    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_is_valid_mac_string(length: 0x%X, name: 0x%X, , allow_upper_case: 0x%X)",
                          length, name, allow_upper_case);

    if (length == 17 && OHA_IS_NOT_NULL(name))
    {
        if (!allow_upper_case)
        {
            if ((name[0]  >= 'A' && name[0]  <= 'F') ||
                (name[1]  >= 'A' && name[1]  <= 'F') ||
                (name[3]  >= 'A' && name[3]  <= 'F') ||
                (name[4]  >= 'A' && name[4]  <= 'F') ||
                (name[6]  >= 'A' && name[6]  <= 'F') ||
                (name[7]  >= 'A' && name[7]  <= 'F') ||
                (name[9]  >= 'A' && name[9]  <= 'F') ||
                (name[10] >= 'A' && name[10] <= 'F') ||
                (name[12] >= 'A' && name[12] <= 'F') ||
                (name[13] >= 'A' && name[13] <= 'F') ||
                (name[15] >= 'A' && name[15] <= 'F') ||
                (name[16] >= 'A' && name[16] <= 'F'))
            {
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_mac_string (no upper case letters allowed): LSA_FALSE");
                return LSA_FALSE;
            }
        }

        if (name[2] == '-' && name[5] == '-'
            && name[8] == '-' && name[11] == '-'
            && name[14] == '-'

            && ((name[0]  >= '0' && name[0] <= '9')  || ((name[0] |32) >= 'a' && (name[0] |32) <= 'f'))
            && ((name[1]  >= '0' && name[1] <= '9')  || ((name[1] |32) >= 'a' && (name[1] |32) <= 'f'))
            && ((name[3]  >= '0' && name[3] <= '9')  || ((name[3] |32) >= 'a' && (name[3] |32) <= 'f'))
            && ((name[4]  >= '0' && name[4] <= '9')  || ((name[4] |32) >= 'a' && (name[4] |32) <= 'f'))
            && ((name[6]  >= '0' && name[6] <= '9')  || ((name[6] |32) >= 'a' && (name[6] |32) <= 'f'))
            && ((name[7]  >= '0' && name[7] <= '9')  || ((name[7] |32) >= 'a' && (name[7] |32) <= 'f'))
            && ((name[9]  >= '0' && name[9] <= '9')  || ((name[9] |32) >= 'a' && (name[9] |32) <= 'f'))
            && ((name[10] >= '0' && name[10] <= '9') || ((name[10]|32) >= 'a' && (name[10]|32) <= 'f'))
            && ((name[12] >= '0' && name[12] <= '9') || ((name[12]|32) >= 'a' && (name[12]|32) <= 'f'))
            && ((name[13] >= '0' && name[13] <= '9') || ((name[13]|32) >= 'a' && (name[13]|32) <= 'f'))
            && ((name[15] >= '0' && name[15] <= '9') || ((name[15]|32) >= 'a' && (name[15]|32) <= 'f'))
            && ((name[16] >= '0' && name[16] <= '9') || ((name[16]|32) >= 'a' && (name[16]|32) <= 'f'))
           )
        {
            /* the last (rightmost) label match with "ab-cd-ef-gh-ij-kl" with a...l consists of [a-f,0-9] */
            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_mac_string(the last (rightmost) label match with 'ab-cd-ef-gh-ij-kl' with a...l consists of [a-f,0-9]): LSA_TRUE");
            return LSA_TRUE;
        }

        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_mac_string (not a mac string): LSA_FALSE");
        return LSA_FALSE; /* not ok */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_mac_string (no buffer or invalid length): LSA_FALSE");
    return LSA_FALSE; /* not ok */
}

/* make the staion name valid:
------------------------------
The field dest_name will be coded as data type OctetString with 1 to 240 octets.
The following syntax is applied:
-       1 or more labels, separated by [.]
-       total length is 1-240
-       label-length is 1-63, truncated if longer
-       labels consisting of [A-Z] are converted to [a-z]
-       labels not consisting of [A-Za-z0-9-] are converted to [x]
-       starting labels with [-] are converted to [x]
-       ending labels with [-] are converted to [x]
-       station-names with the form n.n.n.n, n = 0...999, are converted to [x](n).n.n.n
EXAMPLE 1: "device-1.vendor_1" is converted to "device-1.vendorx1"
EXAMPLE 2: "device-1.b鰃eholz" is converted to "device-1.bxgeholz"

-       if mac_address is not a null pointer then it holds the mac address
        which is added by a further label as a string
EXAMPLE 3: "device-1.press1" and mac_address is converted to "device-1.press1.00-0a-5e-02-9b-8d"

 -       if mac_prefix is True the mac address is added with the prefix "--"
EXAMPLE 4: "device-1.press1" and mac_address with mac_prefix=True is converted to "device-1.press1.--00-0a-5e-02-9b-8d"

-       labels with the form "port-xyz" with x, y, z = 0...9 are converted to "port[x]xyz"
-       labels with the form "port-xyz-abcde" with a,b,c,d,e, x, y, z = 0...9 are converted to "port[x]xyz-abcde"
EXAMPLE 5: "port-001" is converted to "portx001"
-------------------------------  */
/* Return Values:   */
/*   OHA_ERR_PARAM              invalid (input)length or no buffer  */
/*   OHA_ERR_FAILED             could not convert (MAC address,...) */
/*   OHA_ERR_BUFFER_OVERFLOW    output buffer too small             */
/*   OHA_ERR_DATA_INVALID       label without characters or string is a PortId like "port-001" */
LSA_UINT16  OHA_LOCAL_FCT_ATTR  oha_make_valid_station_name(
    LSA_UINT16                  src_name_len,   /* IN */
    OHA_COMMON_MEM_U8_PTR_TYPE  src_name,       /* IN */
    OHA_COMMON_MEM_U16_PTR_TYPE dest_name_len,  /* IN: length of buffer, OUT: length of data written */
    OHA_COMMON_MEM_U8_PTR_TYPE  dest_name,      /* IN, OUT */
    OHA_MAC_TYPE                mac_address,    /* IN */
    LSA_BOOL                    mac_prefix      /* IN */
)
{
    LSA_UINT16 RetVal;
    LSA_UINT16 mac_len_to_append;
    LSA_UINT8 MacStr[OHA_MAC2A_LEN];

    LSA_UINT16 src_idx;
    LSA_UINT16 dest_idx;
    LSA_UINT16 lbl;
    LSA_UINT16 nr_of_labels;
    LSA_BOOL looks_like_ipaddr;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_make_valid_station_name(src_name_len: 0x%X, src_name: 0x%X, dest_name_len: 0x%X, dest_name: 0x%X, mac_address: 0x%X)",
                          src_name_len, src_name, dest_name_len, dest_name, mac_address);

    /* "." + "mac-string" or nothing */
    if (OHA_IS_NULL(mac_address))
    {
        mac_len_to_append = 0;
    }
    else    /* if mac_prefix is True the mac address is added with the prefix "--" */
    {
        mac_len_to_append = (mac_prefix) ? (LSA_UINT16)(2 + OHA_MAC2A_LEN) : (LSA_UINT16)OHA_MAC2A_LEN;
    }

    if (OHA_IS_NOT_NULL(mac_address))
    {
        LSA_UINT16 WrittenCharacters = oha_mac2a(mac_address, MacStr);
        if (WrittenCharacters != (OHA_MAC2A_LEN-1))
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "Writing MAC string to buffer failed(RetVal: 0x%X)", WrittenCharacters);
            RetVal = OHA_ERR_FAILED;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
            return RetVal;
        }
    }

    /* special case: SrcLen = 0 is valid if only the MAC string is to write */
    if (src_name_len == 0 && OHA_IS_NOT_NULL(mac_address))
    {
        if (OHA_IS_NOT_NULL(dest_name) && *dest_name_len >= (OHA_MAC2A_LEN-1)) /* only "mac-string" to write */
        {
            OHA_MEMCPY(dest_name, MacStr, OHA_MAC2A_LEN-1);
            *dest_name_len = (OHA_MAC2A_LEN-1);
            RetVal = OHA_OK;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
            return RetVal;
        }
    }

    if (src_name_len > OHA_MAX_CHASSIS_STRLEN ||
        OHA_IS_NULL(src_name) || OHA_IS_NULL(dest_name) ||
        src_name_len == 0 || (*dest_name_len == 0))
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "No buffer or invalid length");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
        return RetVal;
    }

    /* cut string if more than 240 characters are to write */
    if (src_name_len + mac_len_to_append > 240)
    {
        src_name_len = (LSA_UINT16)(240 - mac_len_to_append);
        if (src_name[src_name_len-1] == '.')    /* last label would be an empty one */
        {
            src_name_len--;
        }
    }

    /* DestLen must be greater equal SrcLen within MAC string if to add */
    if ((*dest_name_len + mac_len_to_append) < src_name_len)
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "DestLen must be greater equal SrcLen within MAC string if to add");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
        return RetVal;
    }

    nr_of_labels = 0;
    looks_like_ipaddr = LSA_TRUE;

    lbl = 0; /* start of label */

    for ( src_idx = 0, dest_idx = 0; src_idx <= src_name_len; ++src_idx )
    {
        int/*!*/ ch;

        if ( src_idx < src_name_len )
        {
            ch = src_name[src_idx];
            if (dest_idx >= *dest_name_len)
            {
                RetVal = OHA_ERR_BUFFER_OVERFLOW;  /* dest buffer too small */
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_make_valid_station_name() failed. Buffer too small (dest_name_len: 0x%X)",dest_name_len);
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
                return RetVal;
            }
        }
        else
        {
            ch = '.'; /* fake a dot at the end */
        }

        /***/
        if ( ch == '.' )
        {
            LSA_UINT16 len = (LSA_UINT16)(src_idx - lbl);

            if ( len < 1 )
            {
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "OUT: oha_make_valid_station_name(label 0x%X has 0 chars)!", lbl);
                return OHA_ERR_DATA_INVALID; /* label has 0 chars */
            }

            if ( len > 63 ) /* label has more than 63 chars */
            {
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_make_valid_station_name(label 0x%X trucated, has more than 63 chars)!", lbl);

                dest_idx = (LSA_UINT16)(lbl + 63);
                if (dest_name[dest_idx - 1] == '-')
                {
                    dest_name[dest_idx - 1] = 'x';  /* last label ends with [-] */
                }
            }

            if ( src_idx < src_name_len )
            {
                dest_name[dest_idx] = (LSA_UINT8)ch;
            }

            OHA_ASSERT(src_idx > 0);
            OHA_ASSERT(dest_idx > 0);

            if (dest_name[dest_idx - 1] == '-')
            {
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "oha_make_valid_station_name: ending label 0x%X '-' converted to 'x'", lbl);
                dest_name[dest_idx - 1] = 'x';  /* last label ends with [-] */
            }

            if ( len > 3 )
            {
                looks_like_ipaddr = LSA_FALSE; /* label has more than 3 characters */

                if ( nr_of_labels == 0 )
                {
                    if ( len == 8 || len == 14 )
                    {
                        if (oha_is_valid_port_name(len, &src_name[lbl]))
                        {
                            /* the first (leftmost) label matches with a port-name */
							/* make valid: e.g. "port-001" is converted to "portx001" */
                            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(the first (leftmost) label 0x%X matching with a port-name is converted)", lbl);
                            /* return OHA_ERR_DATA_INVALID; */
							dest_name[4] = 'x';
                        }
                    }
                }
            }

            nr_of_labels++;

            if ( src_idx < src_name_len )
            {
                dest_idx++;
                lbl = dest_idx; /* new start of label */
            }

            continue;
        }

        if ( ch >= '0' && ch <= '9' )
        {
            dest_name[dest_idx] = (LSA_UINT8)ch;
            dest_idx++;
            continue;
        }

        looks_like_ipaddr = LSA_FALSE; /* not only digits */

        if ( ch >= 'a' && ch <= 'z' )
        {
            dest_name[dest_idx] = (LSA_UINT8)ch;
            dest_idx++;
            continue;
        }

        if ( ch >= 'A' && ch <= 'Z' )
        {
            dest_name[dest_idx] = (LSA_UINT8)(ch|32);   /* convert to lower */
            dest_idx++;
            continue;
        }

        if ( dest_idx == lbl )
        {
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "oha_make_valid_station_name: label does not start with letter or digit, ch(0x%X) is converted to 'x'", ch);
            dest_name[dest_idx] = 'x';
            dest_idx++;
            continue;
        }

        if ( ch == '-' )
        {
            dest_name[dest_idx] = (LSA_UINT8)ch;
            dest_idx++;
            continue;
        }

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_make_valid_station_name: illegal char(0x%X) is converted to 'x'", ch);
        dest_name[dest_idx] = 'x';
        dest_idx++;
        continue;
    }

	/* AP00925207 make IP address valid "n.n.n.n" (n = 0, ... 999;) => "[x](n).n.n.n" */
    if ( looks_like_ipaddr && nr_of_labels == 4 )
	{
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "oha_make_valid_station_name: name is an ip address, first ch(0x%X) is converted to 'x'", dest_name[0]);
		dest_name[0] = 'x';
	}

    RetVal = OHA_OK;

    /* append the MAC string */
    if (OHA_IS_NOT_NULL(mac_address))
    {
        if ((dest_idx + mac_len_to_append) >= *dest_name_len)
        {
            RetVal = OHA_ERR_BUFFER_OVERFLOW;  /* dest buffer too small */
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_make_valid_station_name() failed. Buffer too small (dest_name_len: 0x%X)",dest_name_len);
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
            return RetVal;
        }

        dest_name[dest_idx] = '.';
        dest_idx++;

        /* if mac_prefix is True the mac address is added with the prefix "--" */
        if (mac_prefix)
        {
            dest_name[dest_idx] = '-';
            dest_idx++;
            dest_name[dest_idx] = '-';
            dest_idx++;
        }

        OHA_MEMCPY(&dest_name[dest_idx], MacStr, OHA_MAC2A_LEN-1);
        dest_idx += (LSA_UINT16)(OHA_MAC2A_LEN-1);
    }

    /* write the length of written data */
    *dest_name_len = dest_idx;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_make_valid_station_name(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*------------------------------------------------------------------------------
// oha_namecmp
     *
     * syntax:
     *
     * Compares a portion of one string to a portion of another, without case sensitivity.
     * strnicmp performs a signed comparison of name1 to name2, for a maximum length of maxlen bytes,
     * starting with the first character in each string and continuing with subsequent characters until
     * the corresponding characters differ or until the end of the strings is reached.
     * The comparison is not case sensitive.
     * It returns a boolean based on the result of comparing s1 (or part of it) to s2 (or part of it).
     *
     * return value
     *
     * the same as name2    == LSA_TRUE
     * different from name2 == LSA_FALSE
//----------------------------------------------------------------------------*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_namecmp(
    OHA_COMMON_MEM_U8_PTR_TYPE name1,
    OHA_COMMON_MEM_U8_PTR_TYPE name2,
    LSA_UINT32 length,
    LSA_BOOL allow_upper_case
)
{
    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_namecmp(name1: 0x%X, name2: 0x%X, length: 0x%X, allow_upper_case: 0x%X)",
                          name1, name2, length, allow_upper_case);

    if (length == 0)
    {
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_namecmp(): LSA_TRUE");
        return LSA_TRUE;
    }

    OHA_ASSERT_NULL_PTR(name1);
    OHA_ASSERT_NULL_PTR(name2);

    do
    {
        if (allow_upper_case)
        {
            if (OHA_TOLOWER(*name1) != OHA_TOLOWER(*name2))
            {
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_namecmp(): LSA_FALSE");
                return LSA_FALSE;    /* not the same */
            }
        }
        else
        {
            if ((*name1) != (*name2))
            {
                OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_namecmp(): LSA_FALSE");
                return LSA_FALSE;    /* not the same */
            }
        }
        name2++;
        if (*name1++ == 0)
        {
            OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_namecmp(): LSA_FALSE");
            return LSA_FALSE;    /* not the same */
        }
    }
    while (--length != 0);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_namecmp(): LSA_TRUE");
    return LSA_TRUE; /* the same */
}

/*------------------------------------------------------------------------------
// oha_ipcmp
     *
     * syntax:
     *
     * Compares a portion of one ip suite to a portion of another.
     * An ip suite is consisting of 4 byte ip address, 4 byte ip mask and 4 byte gateway address.
     * A pointer with NULL means that an ip address is invalid = {0,0,0,0}.
     * The function returns a boolean based on the result of comparing ip1,ipmask1,gateway1 to
     * ip2, ipmask2, gateway2.
     *
     * return value
     *
     * the same as ip2  == LSA_TRUE
     * different from ip2 == LSA_FALSE
//----------------------------------------------------------------------------*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_ipcmp(
    OHA_COMMON_MEM_U8_PTR_TYPE ip1,
    OHA_COMMON_MEM_U8_PTR_TYPE ipmask1,
    OHA_COMMON_MEM_U8_PTR_TYPE gateway1,
    OHA_COMMON_MEM_U8_PTR_TYPE ip2,
    OHA_COMMON_MEM_U8_PTR_TYPE ipmask2,
    OHA_COMMON_MEM_U8_PTR_TYPE gateway2)
{
    LSA_BOOL    IpSuiteIsTheSame;
    LSA_UINT8   Ip1[]      = {0,0,0,0};
    LSA_UINT8   Ip2[]      = {0,0,0,0};
    LSA_UINT8   IpMask1[]  = {0,0,0,0};
    LSA_UINT8   IpMask2[]  = {0,0,0,0};
    LSA_UINT8   Gateway1[] = {0,0,0,0};
    LSA_UINT8   Gateway2[] = {0,0,0,0};
    OHA_COMMON_MEM_U8_PTR_TYPE pIp1      = (OHA_IS_NOT_NULL(ip1)) ? ip1 : Ip1;
    OHA_COMMON_MEM_U8_PTR_TYPE pIp2      = (OHA_IS_NOT_NULL(ip2)) ? ip2 : Ip2;
    OHA_COMMON_MEM_U8_PTR_TYPE pIpMask1  = (OHA_IS_NOT_NULL(ipmask1)) ? ipmask1 : IpMask1;
    OHA_COMMON_MEM_U8_PTR_TYPE pIpMask2  = (OHA_IS_NOT_NULL(ipmask2)) ? ipmask2 : IpMask2;
    OHA_COMMON_MEM_U8_PTR_TYPE pGateway1 = (OHA_IS_NOT_NULL(gateway1)) ? gateway1 : Gateway1;
    OHA_COMMON_MEM_U8_PTR_TYPE pGateway2 = (OHA_IS_NOT_NULL(gateway2)) ? gateway2 : Gateway2;

    OHA_FUNCTION_TRACE_06(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_ipcmp(ip1: 0x%X, ipmask1: 0x%X, gateway1: 0x%X, ip2: 0x%X, ipmask2: 0x%X, gateway2: 0x%X)",
                          ip1, ipmask1, gateway1, ip2, ipmask2, gateway2);

    /* compare ip1, ip2 */
    IpSuiteIsTheSame = OHA_MEMCMP(pIp1, pIp2, OHA_IP_ADDR_SIZE); /* ip address */

    if (IpSuiteIsTheSame)   /* ip mask */
    {
        IpSuiteIsTheSame = OHA_MEMCMP(pIpMask1, pIpMask2, OHA_IP_ADDR_SIZE);
    }
    if (IpSuiteIsTheSame) /* gateway */
    {
        IpSuiteIsTheSame = OHA_MEMCMP(pGateway1, pGateway2, OHA_IP_ADDR_SIZE);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_ipcmp(): 0x%X", IpSuiteIsTheSame);
    return IpSuiteIsTheSame; /* the same */
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeTopoPeers                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoPeers(OHA_DB_TOPO_PTR_TYPE pTopoDataBase)
{
    LSA_UINT16   i;
    LSA_UINT16   FreeStat = LSA_RET_OK;  /* free the buffer for var. */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeTopoPeers(pDbIf: 0x%X)", pTopoDataBase);

    if (! LSA_HOST_PTR_ARE_EQUAL(pTopoDataBase, LSA_NULL))
    {
        /* free allocated TOPO-data entries */
        for (i = 0; i < LLDP_MAX_PORT_SENDER; i++) /* over all ports */
        {
            OHA_UPPER_PEER_PTR_TYPE pPeer = &pTopoDataBase->PeerSender[i];

            if (! LSA_HOST_PTR_ARE_EQUAL(pPeer->pPeerChassisID, LSA_NULL))
            {
                OHA_FREE_LOCAL_MEM(&FreeStat, pPeer->pPeerChassisID);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
                pPeer->pPeerChassisID      = LSA_NULL;
                pPeer->LengthPeerChassisID = 0;
            }
            if (! LSA_HOST_PTR_ARE_EQUAL(pPeer->pPeerPortID, LSA_NULL))
            {
                OHA_FREE_LOCAL_MEM(&FreeStat, pPeer->pPeerPortID);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
                pPeer->pPeerPortID      = LSA_NULL;
                pPeer->LengthPeerPortID = 0;
            }
        }
        pTopoDataBase->NumberOfPeers = 0;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeTopoPeers()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeAllTopoDBs                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeAllTopoDBs(OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    LSA_UINT16   i;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeAllTopoDBs(pHDBUser: 0x%X)", pHDBUser);

    /* free allocated TOPO-data entries */
    for (i = 0; i < OHA_CFG_MAX_PORTS; i++)  /* over all ports */
    {
        OHA_FreeTopoDB(pHDBUser->pTopoDataBase[i]);
        pHDBUser->pTopoDataBase[i] = LSA_NULL;

        OHA_FreeTopoDB(pHDBUser->pPrmTopoBase[i]);
        pHDBUser->pPrmTopoBase[i] = LSA_NULL;
	}

    for (i = 0; i <= OHA_CFG_MAX_PORTS; i++)  /* interface (port 0) and all ports */
	{
        OHA_FreeTopoPortInd(pHDBUser, i);

        pHDBUser->TopoLastIndStati[i].LastSpecifierMau          = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierDelay        = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierLink         = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Chassis = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Port    = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_MRP_domain = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_None    = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_MAU     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_CableDelay = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_RT_Class3  = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_PTCP_status = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_PreambleLength = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Fragmentation = OHA_ALARM_SPECIFIER_INITIAL_VALUE;

        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_Gateway = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_NoS     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_IP      = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_NoD     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeAllTopoDBs()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeTopoLastInd                         +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA last Indication if allocated.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoPortInd(
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16   PortId)
{
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeTopoPortInd(pHDBUser: 0x%X, PortId: 0x%X)", pHDBUser, PortId);

    if (PortId <= OHA_CFG_MAX_PORTS)
    {
        /* Last local TOPO indication existing ? */
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pPortTopoInd[PortId], LSA_NULL))
        {
            LSA_UINT16 FreeStat = LSA_RET_OK;
            OHA_FREE_LOCAL_MEM(&FreeStat, pHDBUser->pPortTopoInd[PortId]);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);

            pHDBUser->pPortTopoInd[PortId] = LSA_NULL;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeTopoPortInd()");
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_AllocTopoPortInd                        +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocs OHA last port Indication structure.                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AllocTopoPortInd(
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16   PortId)
{
    LSA_UINT16      RetVal    = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_AllocTopoPortInd(pHDBUser: 0x%X, PortId: 0x%X)", pHDBUser, PortId);

    if (PortId <= OHA_CFG_MAX_PORTS)
    {
        if ( LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pPortTopoInd[PortId], LSA_NULL) )
        {
            /* alloc the db for topo */
            OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

            OHA_ALLOC_LOCAL_MEM(&pVarTemp, sizeof(OHA_LAST_PRM_IND_DATA_TYPE));  /* alloc the ind. data */
            if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_AllocTopoPortInd: Allocating TOPO-LastInd. failed.");
                RetVal = LSA_RET_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AllocTopoPortInd(RetVal: 0x%X)", RetVal);
                return (RetVal);
            }
            pHDBUser->pPortTopoInd[PortId] = (OHA_LAST_PRM_IND_DATA_PTR_TYPE)pVarTemp;
            OHA_MEMSET(pHDBUser->pPortTopoInd[PortId], 0, sizeof(OHA_LAST_PRM_IND_DATA_TYPE));
            pHDBUser->pPortTopoInd[PortId]->PortId = PortId;
        }
    }
    else
    {
        RetVal = OHA_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AllocTopoPortInd()");
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CopyTopoData                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copies the temporary topt data to the Topo database       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyTopoData(
    OHA_DB_TOPO_PTR_TYPE pDestTopoData,
    OHA_DB_TOPO_PTR_TYPE pSrcTopoData)
{
    LSA_UINT16      RetVal    = OHA_OK;
    OHA_UPPER_PEER_PTR_TYPE     pDestPeer;
    OHA_UPPER_PEER_PTR_TYPE     pSrcPeer;
    LSA_UINT16   i;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CopyTopoData(pDestTopoData: 0x%X, pSrcTopoData: 0x%X)", pDestTopoData, pSrcTopoData);

    OHA_ASSERT_NULL_PTR(pDestTopoData);
    OHA_ASSERT_NULL_PTR(pSrcTopoData);

    /* (i) the peers */
    pDestTopoData->NumberOfPeers = pSrcTopoData->NumberOfPeers;

    /* copy TOPO-peer entries */
    pDestPeer = pDestTopoData->PeerSender;
    pSrcPeer  = pSrcTopoData->PeerSender;

    for (i = 0; i < pSrcTopoData->NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++, pDestPeer++, pSrcPeer++)  /* over all peers */
    {
        /* copy TOPO-data entries */
        pDestPeer->LengthPeerChassisID    = pSrcPeer->LengthPeerChassisID;
        pDestPeer->LengthPeerPortID   = pSrcPeer->LengthPeerPortID;

        RetVal = oha_db_write_TopoPeer(&pDestPeer->pPeerChassisID, pSrcPeer->pPeerChassisID, pSrcPeer->LengthPeerChassisID);
        if (RetVal != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoData: Write PeerStationName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        RetVal = oha_db_write_TopoPeer(&pDestPeer->pPeerPortID, pSrcPeer->pPeerPortID, pSrcPeer->LengthPeerPortID);
        if (RetVal != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoData: Write PeerPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoData(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
    }

    /* (ii) the MAUType */
    pDestTopoData->MAUType = pSrcTopoData->MAUType;

    /* (iii) the LineDelayFactor */
    pDestTopoData->DelayExisting          = pSrcTopoData->DelayExisting;
    pDestTopoData->LineDelayFactor        = pSrcTopoData->LineDelayFactor;

    /* (iv) the LinkState */
    pDestTopoData->LinkStateExisting      = pSrcTopoData->LinkStateExisting;
    pDestTopoData->LinkState              = pSrcTopoData->LinkState;

    /* (v) the DelayDifference */
    pDestTopoData->DelayDifferenceExisting = pSrcTopoData->DelayDifferenceExisting;
    pDestTopoData->DelayMode              = pSrcTopoData->DelayMode;

    /* (vi) the MAUTypeDifference */
    pDestTopoData->MAUTypeDifferenceExisting = pSrcTopoData->MAUTypeDifferenceExisting;
    pDestTopoData->MAUTypeMode            = pSrcTopoData->MAUTypeMode;

    /* (vii) the DomainBoundary */
    pDestTopoData->AdjustTopoData.DomainBoundaryExisting = pSrcTopoData->AdjustTopoData.DomainBoundaryExisting;
    pDestTopoData->AdjustTopoData.DomainBoundaryIngress = pSrcTopoData->AdjustTopoData.DomainBoundaryIngress;
    pDestTopoData->AdjustTopoData.DomainBoundaryEgress  = pSrcTopoData->AdjustTopoData.DomainBoundaryEgress;

    /* (viii) the MulticastBoundary */
    pDestTopoData->AdjustTopoData.MulticastBoundaryExisting = pSrcTopoData->AdjustTopoData.MulticastBoundaryExisting;
    pDestTopoData->AdjustTopoData.MulticastBoundary     = pSrcTopoData->AdjustTopoData.MulticastBoundary;

    /* (ix) the PeerToPeerBoundary */
    pDestTopoData->AdjustTopoData.PeerToPeerBoundaryExisting = pSrcTopoData->AdjustTopoData.PeerToPeerBoundaryExisting;
    pDestTopoData->AdjustTopoData.PeerToPeerBoundary        = pSrcTopoData->AdjustTopoData.PeerToPeerBoundary;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoData()");
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CopyAdjustData                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copies the temporary adjust data to the GSY-HDB           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyAdjustData(
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy,
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16 PortId)
{
    LSA_UINT16              RetVal;
    OHA_DB_TOPO_PTR_TYPE    pTopoData;
    LSA_UINT16              NicID;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CopyTopoData(pHDBGsy: 0x%X, pHDBUser: 0x%X, PortId: 0x%X)", pHDBGsy, pHDBUser, PortId);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBGsy);

    NicID = pHDBUser->Params.pDetail->Params.User.NICId;
    /* look for the corresponding NicId */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyAdjustData(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

    pTopoData = pHDBUser->pPrmTopoBase[PortId-1];

    if (OHA_IS_NOT_NULL(pTopoData))
    {
        /* the DomainBoundary */
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryExisting = pTopoData->AdjustTopoData.DomainBoundaryExisting;
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryIngress = pTopoData->AdjustTopoData.DomainBoundaryIngress;
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryEgress  = pTopoData->AdjustTopoData.DomainBoundaryEgress;

        /* the MulticastBoundary */
        pHDBGsy->AdjustData[PortId-1].MulticastBoundaryExisting = pTopoData->AdjustTopoData.MulticastBoundaryExisting;
        pHDBGsy->AdjustData[PortId-1].MulticastBoundary     = pTopoData->AdjustTopoData.MulticastBoundary;

        /* the PeerToPeerBoundary */
        pHDBGsy->AdjustData[PortId-1].PeerToPeerBoundaryExisting = pTopoData->AdjustTopoData.PeerToPeerBoundaryExisting;
        pHDBGsy->AdjustData[PortId-1].PeerToPeerBoundary        = pTopoData->AdjustTopoData.PeerToPeerBoundary;
    }
    else    /* the topo db has been deleted */
    {
        /* the DomainBoundary */
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryExisting = 0;
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryIngress = 0;
        pHDBGsy->AdjustData[PortId-1].DomainBoundaryEgress  = 0;

        /* the MulticastBoundary */
        pHDBGsy->AdjustData[PortId-1].MulticastBoundaryExisting = 0;
        pHDBGsy->AdjustData[PortId-1].MulticastBoundary     = 0;

        /* the PeerToPeerBoundary */
        pHDBGsy->AdjustData[PortId-1].PeerToPeerBoundaryExisting = 0;
        pHDBGsy->AdjustData[PortId-1].PeerToPeerBoundary        = 0;
    }

    /* control port for sending LLDP- and delay-frames */
    RetVal = OHA_AdjustAdminStatus(NicID, PortId, pHDBGsy->AdjustData[PortId-1].PeerToPeerBoundary);

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicID), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyAdjustData()");
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_AdjustAdminStatus                       +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets the port state depending of adjusted data.           +*/
/*+                                                                         +*/
/*+  Coding of the field PeerToPeerBoundary:                                +*/
/*+                                                                         +*/
/*+   Bit Value       Meaning                                               +*/
/*+                                                                         +*/
/*+    0   1  The LLDP agent shall not send LLDP frames (egress filter).    +*/
/*+    0   0  The LLDP agent shall send LLDP frames for this port.          +*/
/*+                                                                         +*/
/*+    1   1  The PTCP ASE shall not send PTCP_DELAY frames (egress filter).+*/
/*+    1   0  The PTCP ASE shall send PTCP_DELAY frames for this port.      +*/
/*+                                                                         +*/
/*+   ...  0  Reserved                                                      +*/
/*+   31   0  Reserved                                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AdjustAdminStatus(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,
    LSA_UINT32 PeerToPeerBoundary)
{
    LSA_UINT16              RetVal;
    LSA_BOOL                Rema = LSA_FALSE;
	LSA_UINT32              Value;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_AdjustAdminStatus(NICId: 0x%X, PortId: 0x%X, PeerToPeerBoundary: 0x%X)", NICId, PortId, PeerToPeerBoundary);

    /* look for the corresponding NicId */
    if (PortId == 0 || PortId > OHA_CFG_MAX_PORTS)
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AdjustAdminStatus(RetVal: 0x%X)", RetVal);
        return (RetVal);
    }

	RetVal = OHA_DB_READ_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS, &Value, LSA_NULL, &Rema);
	OHA_ASSERT(RetVal == LSA_RET_OK);

    switch (PeerToPeerBoundary & OHA_PTP_SEND_LLDP_MASK)
    {
        case OHA_PTP_SEND_LLDP_ON:
			RetVal = OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS,
													  OHA_LLDP_SEND_RECV, sizeof(LSA_UINT32), Rema); /* default */
			OHA_ASSERT(RetVal == LSA_RET_OK);
            break;
        case OHA_PTP_SEND_LLDP_OFF:
			RetVal = OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(NICId, PortId, OHA_CTRL_ADMIN_STATUS,
													  OHA_LLDP_DISABLED, sizeof(LSA_UINT32), Rema);
			OHA_ASSERT(RetVal == LSA_RET_OK);
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OHA_AdjustAdminStatus: Invalid PeerToPeerBoundary (0x%X)", PeerToPeerBoundary);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AdjustAdminStatus()");
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UpdateFrameOffset                       +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Update the frame offsets in the GSY-HDB                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UpdateFrameOffset(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,
    LSA_BOOL OnlySyncForwarding)
{
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy = OHA_GSYGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateFrameOffset(NICId: 0x%X, PortId: 0x%X, OnlySyncForwarding: 0x%X)", NICId, PortId, OnlySyncForwarding);

    OHA_ASSERT_NULL_PTR(pHDBGsy);
    OHA_ASSERT(PortId && PortId <= OHA_CFG_MAX_PORTS);

    if (pHDBGsy->SyncDataCommit.Mode == OHA_SYNC_ADD)
    {
        OHA_HDB_EDD_PTR_TYPE    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);

        OHA_ASSERT_NULL_PTR(pHDBEdd);

        /* If only SyncForwarding is specified for an (IRT)Port nur SyncForwarding, LLDP send with */
        /* IRDATAUUID == 0, RTClass3-PortState==OFF and REDperiod = OFF will be done.              */
        if (OnlySyncForwarding)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OHA_UpdateFrameOffset (PortId: 0x%X): OnlySyncForwarding port, REDperiod = OFF (=OK)!", PortId);
            pHDBGsy->FrameOffsetValid[PortId-1].RedPeriodBegin = 0; /* invalid */
        }
        else
        {
            /* get the actual red period from EddGetPortParams  */
            /* Red Part of Reserved Interval (contains IRT_Frames only) */
            if (pHDBEdd->LinkStatus[PortId-1].RxIntervalRedBegin == 0 && pHDBEdd->LinkStatus[PortId-1].RxIntervalRedEnd == 0)
            {
                pHDBGsy->FrameOffsetValid[PortId-1].RedPeriodBegin = 0;
            }
            else
            {
                pHDBGsy->FrameOffsetValid[PortId-1].RedPeriodBegin = pHDBEdd->LinkStatus[PortId-1].RxIntervalRedBegin;
                pHDBGsy->FrameOffsetValid[PortId-1].RedPeriodBegin |= OHA_SYNC_PERIOD_VALID_MASK; /* set valid */
            }
        }

        if (pHDBEdd->LinkStatus[PortId-1].RxIntervalOrangeBegin == 0 && pHDBEdd->LinkStatus[PortId-1].RxIntervalOrangeEnd == 0)
        {
            pHDBGsy->FrameOffsetValid[PortId-1].OrangePeriodBegin = 0; /* invalid */
        }
        else
        {
            pHDBGsy->FrameOffsetValid[PortId-1].OrangePeriodBegin = pHDBEdd->LinkStatus[PortId-1].RxIntervalOrangeBegin;
            pHDBGsy->FrameOffsetValid[PortId-1].OrangePeriodBegin |= OHA_SYNC_PERIOD_VALID_MASK; /* set valid */
        }

        if (pHDBEdd->LinkStatus[PortId-1].RxIntervalRedEnd > pHDBEdd->LinkStatus[PortId-1].RxIntervalOrangeEnd)
        {
            pHDBGsy->FrameOffsetValid[PortId-1].GreenPeriodBegin = pHDBEdd->LinkStatus[PortId-1].RxIntervalRedEnd;
        }
        else
        {
            pHDBGsy->FrameOffsetValid[PortId-1].GreenPeriodBegin = pHDBEdd->LinkStatus[PortId-1].RxIntervalOrangeEnd;
        }
        pHDBGsy->FrameOffsetValid[PortId-1].GreenPeriodBegin |= OHA_SYNC_PERIOD_VALID_MASK; /* set valid */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateFrameOffset()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CopySyncData                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copies the temporary Sync data to the GSY-HDB             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopySyncData(
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy,
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    LSA_UINT16              RetVal    = OHA_OK;
    LSA_UINT16              NICId;
    LSA_UINT16              PortId;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;
    LSA_UINT16 PtcpTimeDelay = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CopySyncData(pHDBGsy: 0x%X, pHDBUser: 0x%X)", pHDBGsy, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBGsy);
    OHA_ASSERT_NULL_PTR(pHDBUser);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBLldp);
    OHA_ASSERT_NULL_PTR(pHDBLldp->pDB);

	/* copy the temp sync data */
	/* AP00845941 MAC-Address of the Sync-Master disappears for a short time in LLDP if the Controller is added again after a Remove. */
	/* Correction: The SyncMaster-MAC Address is not deleted if the new Sync Record is the same (if the PTCP_SubdomainUUID's are equal). */

	/* OHA_MEMCPY(&pHDBGsy->SyncDataCommit, &pHDBGsy->SyncDataWrite, sizeof(OHA_SYNC_DATA_TYPE));   */
	pHDBGsy->SyncDataCommit.Mode        = pHDBGsy->SyncDataWrite.Mode;
	pHDBGsy->SyncDataCommit.InterfaceID = pHDBGsy->SyncDataWrite.InterfaceID;
	pHDBGsy->SyncDataCommit.RecordIndex = pHDBGsy->SyncDataWrite.RecordIndex;

	/* PTCP_SubdomainUUID's are equal and not zero ? */
	if (!OHA_MEMCMP(&pHDBGsy->SyncDataWrite.SubdomainID,
					(OHA_COMMON_MEM_PTR_TYPE)OHA_ZERO_UUID, sizeof(OHA_UUID_TYPE)))
	{
		if(OHA_MEMCMP(&pHDBGsy->SyncDataWrite.SubdomainID,
						&pHDBGsy->SyncDataCommit.SubdomainID, sizeof(OHA_UUID_TYPE)))
		{
			OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OHA_CopySyncData(Mode: 0x%X, IfID: 0x%X): The PTCP_SubdomainUUID's are equal, therfore the SyncMaster-MAC Address is not deleted!",
									pHDBGsy->SyncDataWrite.Mode, pHDBGsy->SyncDataWrite.InterfaceID);
		}
		else	/* delete the SyncMaster-MAC Address */
		{
			OHA_MEMCPY(&pHDBGsy->SyncDataCommit.MasterSourceAddress,
						&pHDBGsy->SyncDataWrite.MasterSourceAddress, sizeof(OHA_MAC_TYPE));
		}
	}
	else	/* delete the SyncMaster-MAC Address */
	{
		OHA_MEMCPY(&pHDBGsy->SyncDataCommit.MasterSourceAddress,
					&pHDBGsy->SyncDataWrite.MasterSourceAddress, sizeof(OHA_MAC_TYPE));
	}

	OHA_MEMCPY(&pHDBGsy->SyncDataCommit.SubdomainID,
				&pHDBGsy->SyncDataWrite.SubdomainID, sizeof(OHA_UUID_TYPE));

	OHA_MEMCPY(&pHDBGsy->SyncDataCommit.IRDataID,
				&pHDBGsy->SyncDataWrite.IRDataID, sizeof(OHA_UUID_TYPE));

	pHDBGsy->SyncDataCommit.PTCPTimeoutFactor = pHDBGsy->SyncDataWrite.PTCPTimeoutFactor;
	pHDBGsy->SyncDataCommit.SyncSendFactor    = pHDBGsy->SyncDataWrite.SyncSendFactor;

    /* calculate the delay time for the PTCP diagnosis */
    (LSA_UINT16)OHA_CalculatePtcpTimeDelay(pHDBUser, &PtcpTimeDelay);

    for (PortId = 0; PortId < OHA_CFG_MAX_PORTS && PortId < pHDBLldp->pDB->PortCnt; PortId++)  /* over all ports */
    {
        OHA_UpdateFrameOffset(NICId, (LSA_UINT16)(PortId+1), LSA_FALSE);
		pHDBGsy->FrameOffsetValid[PortId].LengthOfPeriod  = pHDBLldp->pDB->LengthOfPeriod;
		pHDBGsy->FrameOffsetValid[PortId].LengthOfPeriod |= OHA_SYNC_PERIOD_VALID_MASK; /* set valid */

        /* store the delay time for the PTCP diagnosis */
        pHDBUser->PtcpDelayTime[PortId] = PtcpTimeDelay;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopySyncData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeTopoDB                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoDB(OHA_DB_TOPO_PTR_TYPE pTopoDataBase)
{
    LSA_UINT16   FreeStat = LSA_RET_OK;    /* free the buffer for var. */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_FreeTopoDB(pTopoDataBase: 0x%X)", pTopoDataBase);

    if (! LSA_HOST_PTR_ARE_EQUAL(pTopoDataBase, LSA_NULL))
    {
        OHA_FreeTopoPeers(pTopoDataBase);
        OHA_FREE_LOCAL_MEM(&FreeStat, pTopoDataBase);
        OHA_MEM_FREE_FAIL_CHECK(FreeStat);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_FreeTopoDB()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_InitTopo                                +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Database elements if allocated.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_InitTopo(OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    LSA_UINT16 i;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_InitTopo(pHDBUser: 0x%X)", pHDBUser);

    /* (re)init the Topo-DB */
    for (i = 0; i <= OHA_CFG_MAX_PORTS; i++)
    {
        pHDBUser->TopoLastIndStati[i].LastSpecifierMau      = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierDelay    = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierLink     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Chassis = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Port = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_MRP_domain = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_None = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_MAU  = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_CableDelay  = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_RT_Class3  = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_PTCP_status = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_PreambleLength = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierPeer_Fragmentation = OHA_ALARM_SPECIFIER_INITIAL_VALUE;

        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_Gateway = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_NoS     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_IP      = OHA_ALARM_SPECIFIER_INITIAL_VALUE;
        pHDBUser->TopoLastIndStati[i].LastSpecifierInterface_NoD     = OHA_ALARM_SPECIFIER_INITIAL_VALUE;

        /* not free the Ind's for the diag stati */
        /* Last local TOPO indication existing ? */
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDBUser->pPortTopoInd[i], LSA_NULL))
        {
            OHA_MEMSET(pHDBUser->pPortTopoInd[i], 0, sizeof(OHA_LAST_PRM_IND_DATA_TYPE));
            pHDBUser->pPortTopoInd[i]->PortId = i;
        }
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_InitTopo()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_Peers_PdPortDataExpected            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get peers from PdPortDataCheck record                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_Peers_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNextPeers,
    OHA_COMMON_MEM_U16_PTR_TYPE Port_Len,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPort,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis
)
{
    LSA_UINT8 PortLen, ChassisLen;

    LSA_UINT32      index = 0;
    LSA_BOOL        RetVal = LSA_FALSE;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_Peers_PdPortDataCheck(OHA_Get_Peers_PdPortDataExpected: 0x%X)", pRecData);

    if ( LSA_HOST_PTR_ARE_EQUAL(pRecData, LSA_NULL) )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_Peers_PdPortDataExpected: No pRecData!!!");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName    */
    index += 1;
    if (!PortLen)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_Peers_PdPortDataExpected: LengthPeerPortName failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }
    *ppPort  = pRecord;
    pRecord += PortLen;                 /* PeerPortName          */

    index += PortLen;
    OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
    index += 1;
    if (!ChassisLen)
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_Peers_PdPortDataExpected: LengthPeerStationName failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataExpected(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }
    *ppChassis = pRecord;
    pRecord += ChassisLen;              /* PeerStationName       */
    index += ChassisLen;
    LSA_UNUSED_ARG(index);

    RetVal = LSA_TRUE;  /* the peer record is ok */
    *Port_Len = PortLen;
    *Chassis_Len = ChassisLen;

    *ppNextPeers = pRecord;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataExpected(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_Get_Peers_PdPortDataReal                +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get peers from PdPortDataCheck record                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_Peers_PdPortDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNextPeers,
    OHA_COMMON_MEM_U16_PTR_TYPE Port_Len,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPort,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis
)
{
    LSA_UINT8 PortLen, ChassisLen;

    LSA_UINT32      index = 0;
    LSA_BOOL        RetVal = LSA_FALSE;
    OHA_UPPER_MEM_U8_PTR_TYPE  pRecord = pRecData;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_Get_Peers_PdPortDataReal(OHA_Get_Peers_PdPortDataReal: 0x%X)", pRecData);

    if ( LSA_HOST_PTR_ARE_EQUAL(pRecData, LSA_NULL) )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_Get_Peers_PdPortDataReal: No pRecData!!!");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataReal(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

	/* Coding of the field PeerStationName: */
	/* This field shall be coded according to 4.3.1.4.15. It shall contain the NameOfStation */
	/* part of the received LLDP_PortID or LLDP_ChassisID, if coded according to 4.3.1.4.15. */
	/* Otherwise the LengthPeerStationName shall be zero.                                    */
	/* See also Coding of the field PeerPortName. */

    OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName  */
    index += 1;
    if (!PortLen)
    {
		*ppPort = pRecord-1; /* points to length byte, so the pointer is valid */
    }
	else
	{
		*ppPort = pRecord;
	}
    pRecord += PortLen;                 /* PeerPortName        */
    index += PortLen;

    OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
    index += 1;
    if (!ChassisLen)
    {
		*ppChassis = pRecord-1; /* points to length byte, so the pointer is valid */
    }
	else
	{
		*ppChassis = pRecord;
	}
    pRecord += ChassisLen;                  /* PeerStationName     */
    index += ChassisLen;

    /* padding byte after PeerStationName */
    while ( (index & 3) != 0 )
    {
        pRecord += 1;                       /* padding byte        */
        index += 1;
    }

    RetVal = LSA_TRUE;  /* the peer record is ok */
    *Port_Len = PortLen;
    *Chassis_Len = ChassisLen;

    pRecord += 4;                           /* LineDelayFactor */
    index += 4;

    /* PeerMACAddress */
    pRecord += OHA_MAC_ADDR_SIZE;
    index += OHA_MAC_ADDR_SIZE;

    /* padding bytes after reading PeerMACAddress */
    while ( (index & 3) != 0 )
    {
        pRecord += 1;                       /* padding byte        */
        index += 1;
    }

    *ppNextPeers = pRecord;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_Get_Peers_PdPortDataReal(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckTopoPeers                          +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks peers of the record                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CheckTopoPeers(
    OHA_UPPER_MEM_U8_PTR_TYPE   pPeerData,
    LSA_UINT16                  NICId)
{
    LSA_UINT32                  i;
    LSA_UINT16                  RetVal = OHA_OK;
    LSA_UINT32                  index = 0;
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecord = pPeerData;
    LSA_UINT16                  NumberOfPeers;
    LSA_UINT16                  BlockType;
    LSA_UINT16                  BlockLength;
    LSA_UINT16                  BlockVersion;
    LSA_UINT32                  RecordLen;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckTopoPeers(pPeerData: 0x%X, NICId: 0x%X)", pPeerData, NICId);

    OHA_ASSERT_NULL_PTR(pPeerData);

    /* get the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);
    index += 2;

    if (BlockLength < OHA_TOPO_BLOCK_HEADER_PDU_SIZE ||
        BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION  ||
        BlockType != OHA_TOPO_CHECK_PEERS)
    {
        RetVal = OHA_ERR_PRM_DATA;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: BlockHeader failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    RecordLen = BlockLength + 2*sizeof(LSA_UINT16);

    index += 1;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U8 (NumberOfPeers, pRecord);    /* NumberOfPeers       */

    /* copy TOPO-peer entries */
    for (i = 0; i < NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++)  /* over all peers */
    {
        LSA_UINT8 PortLen, ChassisLen;
        OHA_COMMON_MEM_U8_PTR_TYPE PortNamePtr;
        OHA_COMMON_MEM_U8_PTR_TYPE ChassisNamePtr;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName  */
        if (!PortLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: LengthPeerPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        PortNamePtr = pRecord;
        /* check, if it's a RFC-name */
        if (!(oha_is_valid_port_name((LSA_UINT16)PortLen, PortNamePtr)))
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: PeerPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        pRecord += PortLen;                 /* PeerPortName        */
        index += PortLen;
        if (index>RecordLen) goto RecLenFailed;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
        if (!ChassisLen)
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: LengthPeerStationName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        ChassisNamePtr = pRecord;

        /* check, if it's a RFC-name */
        if (!(oha_is_valid_station_name((LSA_UINT16)ChassisLen, ChassisNamePtr, OHA_UPPER_CASES_ALLOWED(NICId), OHA_UPPER_CASES_ALLOWED(NICId))))
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: PeerStationName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        pRecord += ChassisLen;              /* PeerStationName     */
        index += ChassisLen;
        if (index>RecordLen) goto RecLenFailed;
    }

    /* padding byte after reading all peers */
    while ( (index & 3) != 0 )
    {
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        if (*pRecord != 0) /* check the paddings */
        {
            RetVal = OHA_ERR_PRM_DATA;
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"OHA_CheckTopoPeers() - wrong Padding !");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
        pRecord += 1;                       /* padding byte        */
    }

    LSA_UNUSED_ARG(pRecord);
    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (!(BlockLength == index - 2*sizeof(LSA_UINT16)))
    {
RecLenFailed:
        RetVal = OHA_ERR_PRM_BLOCK;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckTopoPeers: Length of PeerData failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoPeers(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckTopoMauType                        +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckTopoMauType(
    LSA_UINT16                  MauType,
    LSA_BOOL                    allow_non_pnio_mautypes)
{
    LSA_BOOL     RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckTopoMauType(MauType: 0x%X, allow_non_pnio_mautypes: 0x%X)", MauType, allow_non_pnio_mautypes);

    switch (MauType)    /* for PNIO */
    {
            /* PDPortDataCheck, PDPortDataReal */
        case OHA_MAU_TYPE_100BASETXFD:  /* 100 MBit Full-Duplex CU (Default) */
        case OHA_MAU_TYPE_100BASEFXFD:  /* 100 MBit Full-Duplex Fiber        */
        case OHA_MAU_TYPE_1000BASEXFD:  /*   1 GBit Full-Duplex unknown      */
        case OHA_MAU_TYPE_1000BASELXFD: /*   1 GBit Full-Duplex Fiber, long  */
        case OHA_MAU_TYPE_1000BASESXFD: /*   1 GBit Full-Duplex Fiber, short */
        case OHA_MAU_TYPE_1000BASETFD:  /*   1 GBit Full-Duplex UTP          */

        case OHA_MAU_TYPE_10GIGBASEFX:  /*  10 GBit Fiber                    */

        case OHA_MAU_TYPE_100BASELX10:  /* 100 MBit Full-Duplex Fiber, long  */
        case OHA_MAU_TYPE_1000BASELX10: /*   1 GBit Full-Duplex Fiber, long  */
        case OHA_MAU_TYPE_100BASEPXFD:  /* 100 MBit Full-Duplex POF-Fiber    */

            RetVal = LSA_TRUE;
            break;

            /* PDPortDataReal */
        case OHA_MAU_TYPE_10BASET:      /* UTP MAU, duplex mode unknown      */

        case OHA_MAU_TYPE_10BASETHD:    /* 10 Mbit/s, half duplex mode (CU)  */
        case OHA_MAU_TYPE_10BASETFD:    /* 10 Mbit/s, full duplex mode (CU)  */
        case OHA_MAU_TYPE_10BASEFLHD:   /* async fiber 10Mbit/s, half duplex */
        case OHA_MAU_TYPE_10BASEFLFD:   /* async fiber10Mbit/s, full duplex  */

        case OHA_MAU_TYPE_100BASETXHD:  /* 100 MBit Half-Duplex CU           */
        case OHA_MAU_TYPE_100BASEFXHD:  /* 100 MBit Half-Duplex Fiber        */
        case OHA_MAU_TYPE_1000BASEXHD:  /*   1 GBit Half-Duplex unknown      */
        case OHA_MAU_TYPE_1000BASELXHD: /*   1 GBit Half-Duplex Fiber, long  */
        case OHA_MAU_TYPE_1000BASESXHD: /*   1 GBit Half-Duplex Fiber, short */
        case OHA_MAU_TYPE_1000BASETHD:  /*   1 GBit Half-Duplex UTP          */

            if (allow_non_pnio_mautypes)
            {
                RetVal = LSA_TRUE;
            }
            else
            {
                RetVal = LSA_FALSE;
            }
            break;

        default:        /* LLDP types */
            /* if (MauType == 0 || MauType > LLDP_TLV_MAU_TYPE_10GIGBASELX4) */
        {
            RetVal = LSA_FALSE; /* no valid MAUType for PNIO */
        }
        break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckTopoMauType(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckDomainUUID                         +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks MRP DomainUUID                                     +*/
/*+                                                                         +*/
/*+  LSA_TRUE:  o.k.                                                        +*/
/*+  LSA_FALSE: not o.k.                                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckDomainUUID(
    OHA_UPPER_MEM_U8_PTR_TYPE                   pDomainUUID)
{
    LSA_BOOL  RetVal;
    LSA_UINT8 MrpUUID[OHA_UUID_SIZE];

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckDomainUUID(pDomainUUID: 0x%X)", pDomainUUID);

    /*         MRP_DomainUUID (Value):                      Meaning / Usage:                       */
    /* 0x00000000-0000-0000-0000-000000000000   reserved                                           */
    /* 0x00000000-0000-0000-0000-000000000001 -                                                    */
    /* 0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFE   Unique UUID for MRP redundancy domain (optional)   */
    /* 0xFFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF   Default UUID for MRP redundancy domain (mandatory) */

    OHA_MEMSET(&MrpUUID, 0, sizeof(OHA_UUID_TYPE));
    RetVal = OHA_MEMCMP(pDomainUUID, &MrpUUID, sizeof(OHA_UUID_TYPE));
    RetVal = (RetVal) ? LSA_FALSE : LSA_TRUE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckDomainUUID(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CheckMRP_PortStatus                     +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks MRP PortStatus                                     +*/
/*+                                                                         +*/
/*+  LSA_TRUE:  o.k.                                                        +*/
/*+  LSA_FALSE: not o.k.                                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef OHA_CFG_NO_MRP
LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckMRP_PortStatus(LSA_UINT32 MRP_PortStatus)
{
    LSA_BOOL  RetVal;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CheckMRP_PortStatus(MRP_PortStatus: 0x%X)", MRP_PortStatus);

    /* check the local MRP PortState */
    switch (MRP_PortStatus)
    {
        case MRP_OPERATION_OFF:
        case MRP_OPERATION_ON:
            RetVal = LSA_TRUE;
            break;
        default:
            RetVal = LSA_FALSE;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CheckMRP_PortStatus: MRP_PortStatus failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckMRP_PortStatus(RetVal: 0x%X)", RetVal);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CheckMRP_PortStatus(RetVal: 0x%X)", RetVal);
    return(RetVal);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetSpeedFromMauType                     +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetSpeedFromMauType(
    LSA_UINT16                  MauType)
{
    LSA_UINT16     Speed;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetSpeedFromMauType(MauType: 0x%X)", MauType);

    switch (MauType)    /* for PNIO */
    {
        case LLDP_TLV_MAU_TYPE_10BASE5: /* thick coax */
        case LLDP_TLV_MAU_TYPE_10BASE2:
        case LLDP_TLV_MAU_TYPE_10BASET:
        case LLDP_TLV_MAU_TYPE_10BASEFP:
        case LLDP_TLV_MAU_TYPE_10BASEFB:
        case LLDP_TLV_MAU_TYPE_10BASEFL:
        case LLDP_TLV_MAU_TYPE_10BROAD36:
        case LLDP_TLV_MAU_TYPE_10BASETHD:
        case LLDP_TLV_MAU_TYPE_10BASETFD:
        case LLDP_TLV_MAU_TYPE_10BASEFLHD:
        case LLDP_TLV_MAU_TYPE_10BASEFLFD:
            Speed = EDD_LINK_SPEED_10;
            break;

        case LLDP_TLV_MAU_TYPE_100BASET4:
        case LLDP_TLV_MAU_TYPE_100BASETXHD:
        case LLDP_TLV_MAU_TYPE_100BASETXFD:/* 100 MBit Full-Duplex  CU (Default) */
        case LLDP_TLV_MAU_TYPE_100BASEFXHD:
        case LLDP_TLV_MAU_TYPE_100BASEFXFD:
        case LLDP_TLV_MAU_TYPE_100BASET2HD:
        case LLDP_TLV_MAU_TYPE_100BASET2FD:
        case OHA_MAU_TYPE_100BASELX10:      /* 100 MBit Full-Duplex Fiber, long  */
        case OHA_MAU_TYPE_100BASEPXFD:      /* 100 MBit Full-Duplex Fiber        */
            Speed = EDD_LINK_SPEED_100;
            break;

        case LLDP_TLV_MAU_TYPE_1000BASEXHD:
        case LLDP_TLV_MAU_TYPE_1000BASEXFD: /*   1 GBit Full-Duplex unknown      */
        case LLDP_TLV_MAU_TYPE_1000BASELXHD:
        case LLDP_TLV_MAU_TYPE_1000BASELXFD:/*   1 GBit Full-Duplex Fiber, long  */
        case LLDP_TLV_MAU_TYPE_1000BASESXHD:
        case LLDP_TLV_MAU_TYPE_1000BASESXFD:/*   1 GBit Full-Duplex Fiber, short */
        case LLDP_TLV_MAU_TYPE_1000BASECXHD:
        case LLDP_TLV_MAU_TYPE_1000BASECXFD:
        case LLDP_TLV_MAU_TYPE_1000BASETHD:
        case LLDP_TLV_MAU_TYPE_1000BASETFD: /*   1 GBit Full-Duplex UTP          */
        case OHA_MAU_TYPE_1000BASELX10:     /*   1 GBit Full-Duplex Fiber, long  */
            Speed = EDD_LINK_SPEED_1000;
            break;

        case LLDP_TLV_MAU_TYPE_10GIGBASEX:  /*  10 GBit Fiber                    */
        case LLDP_TLV_MAU_TYPE_10GIGBASELX4:
        case LLDP_TLV_MAU_TYPE_10GIGBASER:
        case LLDP_TLV_MAU_TYPE_10GIGBASEER:
        case LLDP_TLV_MAU_TYPE_10GIGBASELR:
        case LLDP_TLV_MAU_TYPE_10GIGBASESR:
        case LLDP_TLV_MAU_TYPE_10GIGBASEW:
        case LLDP_TLV_MAU_TYPE_10GIGBASEEW:
        case LLDP_TLV_MAU_TYPE_10GIGBASELW:
        case LLDP_TLV_MAU_TYPE_10GIGBASESW:
            Speed = EDD_LINK_SPEED_10000;
            break;

        case LLDP_TLV_MAU_TYPE_AUI: /* no internal MAU, view from AUI */
        case LLDP_TLV_MAU_TYPE_FOIRL: /* FOIRL MAU */
            Speed = EDD_LINK_UNKNOWN;   /* Speed unknown */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromMauType: Speed unknown.");
            break;

        default:        /* other types */
            Speed = EDD_LINK_UNKNOWN;   /* no valid Speed for PNIO */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromMauType: MauType failed.");
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetSpeedFromMauType(Speed: 0x%X)", Speed);
    return(Speed);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetModeFromMauType                      +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetModeFromMauType(
    LSA_UINT16                  MauType)
{
    LSA_UINT16     Mode;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetModeFromMauType(MauType: 0x%X)", MauType);

    switch (MauType)    /* for PNIO */
    {
        case LLDP_TLV_MAU_TYPE_10BASETHD:
        case LLDP_TLV_MAU_TYPE_10BASEFLHD:

        case LLDP_TLV_MAU_TYPE_100BASETXHD:
        case LLDP_TLV_MAU_TYPE_100BASEFXHD:
        case LLDP_TLV_MAU_TYPE_100BASET2HD:
        case LLDP_TLV_MAU_TYPE_1000BASEXHD:
        case LLDP_TLV_MAU_TYPE_1000BASELXHD:
        case LLDP_TLV_MAU_TYPE_1000BASESXHD:
        case LLDP_TLV_MAU_TYPE_1000BASECXHD:
        case LLDP_TLV_MAU_TYPE_1000BASETHD:
            Mode = EDD_LINK_MODE_HALF;
            break;

        case LLDP_TLV_MAU_TYPE_10BASETFD:
        case LLDP_TLV_MAU_TYPE_10BASEFLFD:

        case LLDP_TLV_MAU_TYPE_100BASETXFD: /* 100 MBit Full-Duplex CU (Default) */
        case LLDP_TLV_MAU_TYPE_100BASEFXFD:
        case LLDP_TLV_MAU_TYPE_100BASET2FD:

        case LLDP_TLV_MAU_TYPE_1000BASEXFD: /*   1 GBit Full-Duplex unknown      */
        case LLDP_TLV_MAU_TYPE_1000BASELXFD:/*   1 GBit Full-Duplex Fiber, long  */
        case LLDP_TLV_MAU_TYPE_1000BASESXFD:/*   1 GBit Full-Duplex Fiber, short */
        case LLDP_TLV_MAU_TYPE_1000BASECXFD:
        case LLDP_TLV_MAU_TYPE_1000BASETFD: /*   1 GBit Full-Duplex UTP          */
        case OHA_MAU_TYPE_100BASELX10:      /* 100 MBit Full-Duplex Fiber, long  */
        case OHA_MAU_TYPE_100BASEPXFD:      /* 100 MBit Full-Duplex Fiber        */
        case OHA_MAU_TYPE_1000BASELX10:     /*   1 GBit Full-Duplex Fiber, long  */
            Mode = EDD_LINK_MODE_FULL;
            break;

        case LLDP_TLV_MAU_TYPE_10GIGBASEX:  /*  10 GBit Fiber                    */
        case LLDP_TLV_MAU_TYPE_10GIGBASELX4:
        case LLDP_TLV_MAU_TYPE_10GIGBASER:
        case LLDP_TLV_MAU_TYPE_10GIGBASEER:
        case LLDP_TLV_MAU_TYPE_10GIGBASELR:
        case LLDP_TLV_MAU_TYPE_10GIGBASESR:
        case LLDP_TLV_MAU_TYPE_10GIGBASEW:
        case LLDP_TLV_MAU_TYPE_10GIGBASEEW:
        case LLDP_TLV_MAU_TYPE_10GIGBASELW:
        case LLDP_TLV_MAU_TYPE_10GIGBASESW:

        case LLDP_TLV_MAU_TYPE_AUI:
        case LLDP_TLV_MAU_TYPE_10BASE5:
        case LLDP_TLV_MAU_TYPE_FOIRL:
        case LLDP_TLV_MAU_TYPE_10BASE2:
        case LLDP_TLV_MAU_TYPE_10BASET:   /* duplex mode unknown */
        case LLDP_TLV_MAU_TYPE_10BASEFP:
        case LLDP_TLV_MAU_TYPE_10BASEFB:
        case LLDP_TLV_MAU_TYPE_10BASEFL:  /* duplex mode unknown */
        case LLDP_TLV_MAU_TYPE_10BROAD36:

        case LLDP_TLV_MAU_TYPE_100BASET4:
            Mode = EDD_LINK_UNKNOWN;      /* duplex mode unknown */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromMauType: duplex mode unknown.");
            break;

        default:        /* other types */
            Mode = EDD_LINK_UNKNOWN;    /* no valid MauType for PNIO */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromMauType: MauType failed.");
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetModeFromMauType(Mode: 0x%X)", Mode);
    return(Mode);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetAdvertisedCapFromParams              +*/
/*+  Input/Output          :    AutonegCapAdvertised (EDD)                  +*/
/*+  Result                :    AutonegAdvertisedCap (RFC)                  +*/
/*+                                                                         +*/
/*+  RFC 3636          802.3 MAU MIB                September 2003          +*/
/*+                                                                         +*/
/*+  SYNTAX      BITS {                                                     +*/
/*+      bOther(0),        - other or unknown                               +*/
/*+      b10baseT(1),      - 10BASE-T  half duplex mode                     +*/
/*+      b10baseTFD(2),    - 10BASE-T  full duplex mode                     +*/
/*+      b100baseT4(3),    - 100BASE-T4                                     +*/
/*+      b100baseTX(4),    - 100BASE-TX half duplex mode                    +*/
/*+      b100baseTXFD(5),  - 100BASE-TX full duplex mode                    +*/
/*+      b100baseT2(6),    - 100BASE-T2 half duplex mode                    +*/
/*+      b100baseT2FD(7),  - 100BASE-T2 full duplex mode                    +*/
/*+      bFdxPause(8),     - PAUSE for full-duplex links                    +*/
/*+      bFdxAPause(9),    - Asymmetric PAUSE for full-duplex links         +*/
/*+      bFdxSPause(10),   - Symmetric PAUSE for full-duplex links          +*/
/*+      bFdxBPause(11),   - Asymmetric and Symmetric PAUSE for full-duplex links +*/
/*+      b1000baseX(12),   - 1000BASE-X, -LX, -SX, -CX half duplex mode     +*/
/*+      b1000baseXFD(13), - 1000BASE-X, -LX, -SX, -CX full duplex mode     +*/
/*+      b1000baseT(14),   - 1000BASE-T half duplex mode                    +*/
/*+      b1000baseTFD(15)  - 1000BASE-T full duplex mode                    +*/
/*+  }                                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: converts AutonegCapAdvertised (EDD) to a RFC3636 capability +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetAdvertisedCapFromParams(LSA_UINT32 AutonegCapAdvertised)
{
	LSA_UINT16 RFC_AutonegAdvertisedCap = 0;	/* bitfield (see rfc3636) */

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
    	                   "IN : OHA_GetAdvertisedCapFromParams(AutonegCapAdvertised: 0x%X)",
																AutonegCapAdvertised);

	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_OTHER)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_OTHER;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_10BASET)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_10BASET;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_10BASETFD)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_10BASETFD;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_100BASET4)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_100BASET4;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_100BASETX)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_100BASETX;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_100BASETXFD)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_100BASETXFD;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_100BASET2)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_100BASET2;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_100BASET2FD)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_100BASET2FD;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_FDXPAUSE)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_FDXPAUSE;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_FDXAPAUSE)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_FDXAPAUSE;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_FDXSPAUSE)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_FDXSPAUSE;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_FDXBPAUSE)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_FDXBPAUSE;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_1000BASEX)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_1000BASEX;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_1000BASEXFD)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_1000BASEXFD;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_1000BASET)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_1000BASET;
	}
	if(AutonegCapAdvertised & EDD_AUTONEG_CAP_1000BASETFD)
	{
		RFC_AutonegAdvertisedCap |= OHA_MAU_AUTONEG_CAP_1000BASETFD;
	}

	if (AutonegCapAdvertised & 0xffff0000)	/* other existing */
	{
		RFC_AutonegAdvertisedCap |= EDD_AUTONEG_CAP_OTHER;
	}

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetAdvertisedCapFromParams(RFC_AutonegAdvertisedCap: 0x%X)", RFC_AutonegAdvertisedCap);
	return(RFC_AutonegAdvertisedCap);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetLinkStatus                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: get the LinkState from the EDD PortStatus                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetLinkStatus(LSA_UINT16 PortStatus)
{
    LSA_UINT16 LinkStatus;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetLinkStatus(Status: 0x%X)", PortStatus);

    switch (PortStatus)
    {
        case EDD_LINK_UP:
        case EDD_LINK_UP_CLOSED:
        case EDD_LINK_UP_DISABLED:
            LinkStatus = OHA_PORT_LINK_STATE_UP;
            break;
        case EDD_LINK_DOWN:
            LinkStatus = OHA_PORT_LINK_STATE_DOWN;
            break;
        case EDD_LINK_UNKNOWN:
            LinkStatus = OHA_PORT_LINK_STATE_UNKNOWN;
            break;
        default:
            LinkStatus = OHA_PORT_LINK_STATE_UNKNOWN;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_GetLinkStatus: Invalid LinkStatus (0x%X)", LinkStatus);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetLinkStatus(PortState: 0x%X)", LinkStatus);
    return(LinkStatus);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetPortStateFromParams                  +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetLinkStatusChange(
    LSA_UINT16              PrevStatus,
    LSA_UINT16              ActualStatus)
{
    LSA_UINT16 Prev;
    LSA_UINT16 Actual;
    LSA_UINT16 LinkChange;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetLinkStatusChange(PrevStatus: 0x%X, ActualStatus: 0x%X)", PrevStatus, ActualStatus);

    switch (PrevStatus)
    {
        case EDD_LINK_UP:
        case EDD_LINK_UP_CLOSED:
        case EDD_LINK_UP_DISABLED:
            Prev = EDD_LINK_UP;
            break;
        case EDD_LINK_DOWN:
        case EDD_LINK_UNKNOWN:
            Prev = EDD_LINK_DOWN;
            break;
        default:
            Prev = EDD_LINK_UNKNOWN;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_GetLinkStatusChange: Invalid Previous Status (0x%X)", PrevStatus);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    switch (ActualStatus)
    {
        case EDD_LINK_UP:
        case EDD_LINK_UP_CLOSED:
        case EDD_LINK_UP_DISABLED:
            Actual = EDD_LINK_UP;
            break;
        case EDD_LINK_DOWN:
        case EDD_LINK_UNKNOWN:
            Actual = EDD_LINK_DOWN;
            break;
        default:
            Actual = EDD_LINK_UNKNOWN;
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_GetLinkStatusChange: Invalid ActualStatus (0x%X)", ActualStatus);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* no change of port state ? */
    if (Actual == Prev)
    {
        LinkChange = EDD_LINK_UNKNOWN;
    }
    else
    {
        LinkChange = Actual;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetLinkStatusChange(LinkChange: 0x%X)", LinkChange);
    return(LinkChange);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CopyTopoPeersToDB                       +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyTopoPeersToDB(
    OHA_DB_TOPO_PTR_TYPE        pTopoDB,
    OHA_UPPER_MEM_U8_PTR_TYPE   pPeerData)
{
    LSA_UINT32                  i;
    LSA_UINT16                  RetVal = OHA_OK;
    OHA_UPPER_PEER_PTR_TYPE     pDest;
    LSA_UINT32                  index = 0;
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecord = pPeerData;
    LSA_UINT16                  NumberOfPeers;
    LSA_UINT16                  BlockType;
    LSA_UINT16                  BlockLength;
    LSA_UINT16                  BlockVersion;
    LSA_UINT32                  RecordLen;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CopyTopoPeersToDB(pTopoDB: 0x%X, pPeerData: 0x%X)", pTopoDB, pPeerData);

    OHA_ASSERT_NULL_PTR(pPeerData);
    OHA_ASSERT_NULL_PTR(pTopoDB);

    pDest = pTopoDB->PeerSender;

    /* get the BlockHeader: */
    /*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */
    OHA_PDU_GET_U16 (BlockType, pRecord);
    index += 2;
    OHA_PDU_GET_U16 (BlockLength, pRecord);
    index += 2;
    OHA_PDU_GET_U16 (BlockVersion, pRecord);
    index += 2;

    if (BlockLength < OHA_TOPO_BLOCK_HEADER_PDU_SIZE ||
        BlockVersion != OHA_TOPO_DATA_BLOCK_VERSION  ||
        BlockType != OHA_TOPO_CHECK_PEERS)
    {
        RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: BlockHeader failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    RecordLen = BlockLength + 2*sizeof(LSA_UINT16);

    index += 1;
    if (index>RecordLen) goto RecLenFailed;
    OHA_PDU_GET_U8 (NumberOfPeers, pRecord);    /* NumberOfPeers       */

    /* copy TOPO-peer entries */
    for (i = 0; i < NumberOfPeers && i < LLDP_MAX_PORT_SENDER; i++, pDest++)  /* over all peers */
    {
        LSA_UINT8 PortLen, ChassisLen;
        OHA_COMMON_MEM_U8_PTR_TYPE PortNamePtr;
        OHA_COMMON_MEM_U8_PTR_TYPE ChassisNamePtr;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (PortLen, pRecord);  /* LengthPeerPortName  */
        if (!PortLen)
        {
            RetVal = OHA_ERR_PARAM;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: LengthPeerPortName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        PortNamePtr = pRecord;
        pRecord += PortLen;                 /* PeerPortName        */
        index += PortLen;
        if (index>RecordLen) goto RecLenFailed;

        index += 1;
        if (index>RecordLen) goto RecLenFailed;
        OHA_PDU_GET_U8 (ChassisLen, pRecord);/* LengthPeerStationName */
        if (!ChassisLen)
        {
            RetVal = OHA_ERR_PARAM;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: LengthPeerStationName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        ChassisNamePtr = pRecord;
        pRecord += ChassisLen;              /* PeerStationName       */
        index += ChassisLen;
        if (index>RecordLen) goto RecLenFailed;

        /* copy TOPO-data entries */
        RetVal = oha_db_write_TopoPeer(&pDest->pPeerChassisID, ChassisNamePtr, ChassisLen);
        if (RetVal == LSA_RET_OK)
        {
            pDest->LengthPeerChassisID    = ChassisLen;
            RetVal = oha_db_write_TopoPeer(&pDest->pPeerPortID, PortNamePtr, PortLen);
            if (RetVal == LSA_RET_OK)
            {
                pDest->LengthPeerPortID   = PortLen;
                pTopoDB->NumberOfPeers++;/* finally write the number of peers to the topo-db */
            }
            else
            {
                RetVal = OHA_ERR_RESOURCE;
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: Write PeerPortName failed.");
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }
        else
        {
            RetVal = OHA_ERR_RESOURCE;
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: Write PeerStationName failed.");
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }
    }

    /* padding byte after reading all peers */
    while ( (index & 3) != 0 )
    {
        pRecord += 1;                       /* padding byte        */
        index += 1;
        if (index>RecordLen) goto RecLenFailed;
    }

    LSA_UNUSED_ARG(pRecord);
    /* BlockLength means total_size - sizeof(BlockHeader.Type) - sizeof(BlockHeader.Length) */
    if (!(BlockLength == index - 2*sizeof(LSA_UINT16)))
    {
RecLenFailed:
        RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_CopyTopoPeersToDB: Length of PeerData failed.");
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CopyTopoPeersToDB(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Internal input function: oha_db_write_TopoPeer()                          */
/* writes a TOPO database-element, if exist: delete and alloc a new one      */
/*****************************************************************************/
LSA_UINT16 oha_db_write_TopoPeer(
    OHA_COMMON_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppElement,
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length)     /* length of variable in octets */
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_TopoPeer(ppElement: 0x%X, pVariable: 0x%X, Length: 0x%X)",
                          ppElement, pVariable, Length);
    if (! LSA_HOST_PTR_ARE_EQUAL((ppElement), LSA_NULL) )
    {
        OHA_ALLOC_LOCAL_MEM(&pVarTemp, Length);  /* alloc the new buffer for the variable */
        if (! LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )
        {
            if (! LSA_HOST_PTR_ARE_EQUAL((*ppElement), LSA_NULL) ) /* varialble exists */
            {
                LSA_UINT16   FreeStat = LSA_RET_OK;   /* free the "old" buffer for var. */
                OHA_FREE_LOCAL_MEM(&FreeStat, *ppElement);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
            }

            *ppElement = (OHA_COMMON_MEM_U8_PTR_TYPE)pVarTemp;  /* now supply the new var. */
            OHA_MEMCPY(*ppElement, pVariable, Length);
        }
        else    /* failed the allocation */
        {
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_db_write_TopoPeer(). Allocating memory failed.");
            RetVal = LSA_RET_ERR_RESOURCE;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_db_write_TopoPeer(). No such element.");
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_TopoPeer(RetVal: 0x%X)", RetVal);
    return RetVal;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetCountOfRealPeers                     +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetCountOfRealPeers(
    LSA_UINT16              NICId,
    LSA_UINT16              PortId)
{
    LSA_UINT16     RealPeers = 0;
    LSA_UINT16     SenderId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetCountOfRealPeers(NICId: 0x%X, PortId: 0x%X)", NICId, PortId);

    if (NICId != OHA_INVALID_NIC_ID &&
        PortId && PortId <= OHA_CFG_MAX_PORTS)
    {
        OHA_HDB_EDD_PTR_TYPE    pHDBEdd  = OHA_EDDGetHandleByNIC(NICId);
        OHA_HDB_LLDP_PTR_TYPE   pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

        if ((! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp,  LSA_NULL)) &&
            (! LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL)) &&
            (! LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pDB,  LSA_NULL)))
        {
            LSA_UINT16  LinkState = OHA_GetLinkStatus(pHDBEdd->LinkStatus[PortId-1].LinkStatus);

            /* if the Link is down -> return "no peers detected" */
            if (LinkState == OHA_PORT_LINK_STATE_UP)
            {
                for (SenderId = 0; SenderId < LLDP_MAX_PORT_SENDER; SenderId++)
                {
                    OHA_LLDP_SENDER_PTR_TYPE pRemoteSender = &pHDBLldp->pDB->LldpData[PortId-1].RemoteSender[SenderId];
                    if (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID ||
                        pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID)
                    {
                        RealPeers++;
                    }
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetCountOfRealPeers(RealPeers: 0x%X)", RealPeers);
    return(RealPeers);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetPortDelays                                 +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pLocDelays           : in: local delays from database for TLV to add   +*/
/*+  pRemDelays           : in: remote delays from database for TLV to add  +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: PNO                                             +*/
/*+                Subtype: 1                                               +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / PNO                                          */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*  PNO defined information string:                                          */
/*                                                                           */
/*    4 octets  4 octets  4 octets                                           */
/*  +---------+---------+---------+---------+------------+                   */
/*  | RxDelay | RxDelay | TxDelay | TxDelay | CableDelay |                   */
/*  | Local   | Remote  | Local   | Remote  | Local      |                   */
/*  |         |         |         |         |            |                   */
/*  +---------+---------+---------+---------+------------+                   */
/*                                                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortDelays(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE                        pLocDelays,
    OHA_UPPER_MEM_U8_PTR_TYPE                        pRemDelays)
{
    LSA_UINT16                  Response;
    LSA_UINT32                  VLength;
    OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = OHA_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetPortDelays(ppTLV: 0x%X, pTLength: 0x%X, pLocDelays: 0x%X, pRemDelays: 0x%X)", ppTLV, pTLength, pLocDelays, pRemDelays);

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pLocDelays,LSA_NULL))
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
        VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_DELAY_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = OHA_ERR_FAILED;
        }
        else
        {
            LSA_UINT32 Delay;

            OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,OHA_LLDP_ORG_OUI_PNO_1,
                                           OHA_LLDP_ORG_OUI_PNO_2,
                                           OHA_LLDP_ORG_OUI_PNO_3,
                                           OHA_LLDP_SUB_TYPE_PNO_DELAY);

            Delay = OHA_LLDP_GET_U32(pLocDelays+(OHA_LLDP_IDX_RXDELAY_LOC * 4/*sizeof(LSA_UINT32)*/));
            OHA_LLDP_SET_U32(pTLV, Delay);   /* RxDelayLoc    */
            if (LSA_HOST_PTR_ARE_EQUAL(pRemDelays,LSA_NULL))
            {
                OHA_LLDP_SET_U32(pTLV, 0);   /* unknown RxDelayRem */
            }
            else
            {   /* !!! remote is, what's received for "local" !!! */
                Delay = OHA_LLDP_GET_U32(pRemDelays+(OHA_LLDP_IDX_RXDELAY_LOC * 4/*sizeof(LSA_UINT32)*/));
                OHA_LLDP_SET_U32(pTLV, Delay); /* RxDelayRem    */
            }
            Delay = OHA_LLDP_GET_U32(pLocDelays+(OHA_LLDP_IDX_TXDELAY_LOC * 4/*sizeof(LSA_UINT32)*/));
            OHA_LLDP_SET_U32(pTLV, Delay);   /* TxDelayLoc    */
            if (LSA_HOST_PTR_ARE_EQUAL(pRemDelays,LSA_NULL))
            {
                OHA_LLDP_SET_U32(pTLV, 0);   /* unknown TxDelayRem */
            }
            else
            {   /* !!! remote is, what's received for "local" !!! */
                Delay = OHA_LLDP_GET_U32(pRemDelays+(OHA_LLDP_IDX_TXDELAY_LOC * 4/*sizeof(LSA_UINT32)*/));
                OHA_LLDP_SET_U32(pTLV, Delay); /* TxDelayRem    */
            }
            Delay = OHA_LLDP_GET_U32(pLocDelays+(OHA_LLDP_IDX_CABLEDELAY_LOC * 4/*sizeof(LSA_UINT32)*/));
            OHA_LLDP_SET_U32(pTLV, Delay);/* CableDelayLoc */

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetPortDelays(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpVSetChassisPortTTL                            +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV     +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE                  pTLength +*/
/*+                    LLDP_TLV_SET_INFO_TYPE  OHA_UPPER_MEM_ATTR *pTLVInfo +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : out:Pointer to TLV chain length                 +*/
/*+  pTLVInfo             : in: Pointer to TLVInfo structure with TLVs      +*/
/*+                             to put into *ppTLV.                         +*/
/*+                                                                         +*/
/*+  Result: LSA_RET_OK                                                     +*/
/*+          LSA_RET_ERR_PARAM                                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds the TLVs from pTLVInfo into the ppTLV.               +*/
/*+               These must be the first TLVs within a LLDP-PDU            +*/
/*+                                                                         +*/
/*+               ppTLV must have room for the TLVs added.                  +*/
/*+               The max length of all TLVs is LLDP_MAX_TLV_DATA_SIZE.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetChassisPortTTL(
    OHA_UPPER_MEM_PTR_TYPE  OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_SET_INFO_TYPE  OHA_UPPER_MEM_ATTR *pTLVInfo,
	LSA_UINT32                                 MultipleIfMode)
{

    LSA_UINT16       Response;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetChassisPortTTL(ppTLV: 0x%X, pTLength: 0x%X, pTLVInfo: 0x%X, MultipleIfMode: 0x%X)",
						  ppTLV, pTLength, pTLVInfo, MultipleIfMode);

    Response = LSA_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo,LSA_NULL)))
    {
        Response = LSA_RET_ERR_PARAM;
    }
    else
    {
        OHA_COMMON_MEM_U8_PTR_TYPE pTLVHelp;
        LSA_UINT32                  Len;
        LSA_UINT32                  TLen;

        if ( ( pTLVInfo->ChassisIDLen < LLDP_MIN_CHASSIS_ID_STRING_SIZE   )  ||
             ( pTLVInfo->ChassisIDLen > LLDP_MAX_CHASSIS_ID_STRING_SIZE   )  ||
             ( pTLVInfo->PortIDLen    < LLDP_MIN_PORT_ID_STRING_SIZE      )  ||
             ( pTLVInfo->PortIDLen    > LLDP_MAX_PORT_ID_STRING_SIZE      )  ||
             ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pChassisIDString,LSA_NULL))  ||
             ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pPortIDString,LSA_NULL   )) )
        {
            Response = LLDP_RSP_ERR_PARAM;
        }
        else
        {
            /* set TLV */
            pTLVHelp = (OHA_COMMON_MEM_U8_PTR_TYPE)(*ppTLV);
            TLen = 0;

            /* ------------------------------------------------------------------*/
            /* insert Chassis ID                                                 */
            /* ------------------------------------------------------------------*/

            Len = pTLVInfo->ChassisIDLen + LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE;

            /* TL-Fields */
            OHA_LLDP_SET_TL_FIELD(pTLVHelp,LLDP_TLV_TYPE_CHASSIS_ID,Len);

            /* V-Field */
            *pTLVHelp++ = pTLVInfo->ChassisIDSubtype;

            if (pTLVInfo->ChassisIDLen)
            {
                OHA_MEMCPY(pTLVHelp,
                           pTLVInfo->pChassisIDString,
                           pTLVInfo->ChassisIDLen);

                /* only send lower cases !!! */
                if (pTLVInfo->ChassisIDSubtype == LLDP_TLV_CHA_SUB_LOCALLY_ASSIGNED)
                {
					/* AP01512558: not by multiple interface mode */
					if(!(MultipleIfMode & OHA_NAME_OF_DEVICE_MASK))
					{
						/* convert upper cases to lower cases */
						OHA_STRNTOLOWER(pTLVHelp, pTLVInfo->ChassisIDLen);
					}
                }

                pTLVHelp += pTLVInfo->ChassisIDLen;
            }

            TLen += LLDP_TL_SIZE + Len;

            /* ------------------------------------------------------------------*/
            /* insert Port ID                                                    */
            /* ------------------------------------------------------------------*/

            Len = pTLVInfo->PortIDLen + LLDP_TLV_PORT_ID_SUBTYPE_SIZE;

            /* TL-Fields */
            OHA_LLDP_SET_TL_FIELD(pTLVHelp,LLDP_TLV_TYPE_PORT_ID,Len);

            /* V-Field */
            *pTLVHelp++ = pTLVInfo->PortIDSubtype;

            if (pTLVInfo->PortIDLen)
            {
                OHA_MEMCPY(pTLVHelp,
                           pTLVInfo->pPortIDString,
                           pTLVInfo->PortIDLen);

                /* only send lower cases !!! */
                if (pTLVInfo->PortIDSubtype == LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED)
                {
                    /* convert upper cases to lower cases */
                    OHA_STRNTOLOWER(pTLVHelp, pTLVInfo->PortIDLen);
                }

                pTLVHelp += pTLVInfo->PortIDLen;
            }

            TLen += LLDP_TL_SIZE + Len;

            /* ------------------------------------------------------------------*/
            /* insert TTL                                                        */
            /* ------------------------------------------------------------------*/

            Len = LLDP_TLV_TTL_SIZE;

            /* TL-Fields */
            OHA_LLDP_SET_TL_FIELD(pTLVHelp,LLDP_TLV_TYPE_TTL,Len);

            /* V-Field */
            *pTLVHelp++ = (LSA_UINT8) (pTLVInfo->TimeToLive >> 8 );
            *pTLVHelp++ = (LSA_UINT8) (pTLVInfo->TimeToLive & 0xFF);

            TLen += LLDP_TL_SIZE + Len;

            *ppTLV    = pTLVHelp;
            *pTLength = TLen;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetChassisPortTTL(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetPortStatus                                 +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+  pPortStatus       : in: local port status from database for TLV to add +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: PNO                                             +*/
/*+                Subtype: 2                                               +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / PNO                                          */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*  PNO defined information string:                                          */
/*                                                                           */
/*    2 octets         2 octets                                              */
/*  +------------+  +------------+                                           */
/*  | PortStatus.|  | PortStatus.|                                           */
/*  | RTClass2   |  | RTClass3   |                                           */
/*  +------------+  +------------+                                           */
/*                                                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortStatus(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE                        pPortStatus)
{
    LSA_UINT16                 Response;
    LSA_UINT32                 VLength;
    OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = OHA_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetPortStatus(ppTLV: 0x%X, pTLength: 0x%X, pPortStatus: 0x%X)", ppTLV, pTLength, pPortStatus);

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pPortStatus,LSA_NULL))
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
        VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_PORT_STATUS_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = OHA_ERR_FAILED;
        }
        else
        {
            LSA_UINT32 PortStatus;

            OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,OHA_LLDP_ORG_OUI_PNO_1,
                                           OHA_LLDP_ORG_OUI_PNO_2,
                                           OHA_LLDP_ORG_OUI_PNO_3,
                                           OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS);

            PortStatus = OHA_LLDP_GET_U32(pPortStatus);
            OHA_LLDP_SET_U32(pTLV, PortStatus);   /* PortStatus    */

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetPortStatus(Response: 0x%X)", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetPortStatusMrp                              +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+  pPortStatus       : in: local port status from database for TLV to add +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: PNO                                             +*/
/*+                Subtype: 4                                               +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / PNO                                          */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*  PNO defined information string:                                          */
/*                                                                           */
/*      16 octets            2 octets                                        */
/*  +----------------+  +-----------------+                                  */
/*  | MRP_DomainUUID |  | RRT_Status      |                                  */
/*  |                |  | (Reserved)      |                                  */
/*  +----------------+  +-----------------+                                  */
/*                                                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortStatusMrp(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE                        pDomainUUID,
    LSA_UINT16                                       MrpPortStatus
)
{
    LSA_UINT16                 Response;
    LSA_UINT32                 VLength;
    OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = OHA_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetPortStatusMrp(ppTLV: 0x%X, pTLength: 0x%X, MrpPortStatus: 0x%X)", ppTLV, pTLength, MrpPortStatus);

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL))
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
        VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = OHA_ERR_FAILED;
        }
        else
        {
            OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,OHA_LLDP_ORG_OUI_PNO_1,
                                           OHA_LLDP_ORG_OUI_PNO_2,
                                           OHA_LLDP_ORG_OUI_PNO_3,
                                           OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP);

            /* MRP_DomainUUID */
            OHA_MEMCPY(pTLV, pDomainUUID, sizeof(OHA_UUID_TYPE));
            pTLV += sizeof(OHA_UUID_TYPE);

            /* MRP_PortStatus */
            OHA_LLDP_SET_U16(pTLV, MrpPortStatus);   /* MRP_PortStatus    */

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetPortStatusMrp(Response: 0x%X)", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetPtcpStatus                                 +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+  pPtcpStatus       : in: local port status from database for TLV to add +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: PNO                                             +*/
/*+                Subtype: 6                                               +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / PNO                                          */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*  PNO defined information string:                                          */
/*                                                                           */
/*       6 octets            8 octets             8 octets                   */
/*  +----------------+  +-----------------+   +-----------------+            */
/*  | MasterSource-  |  | SubdomainUUID   |   | IRDataUUID      |            */
/*  | Address        |  |                 |   |                 |            */
/*  +----------------+  +-----------------+   +-----------------+            */
/*                                                                           */
/*       32 bits          32 bits          32 bits           32 bits         */
/*  +----------------+ +------------+  +---------------+  +--------------+   */
/*  | LengthOfPeriod | | RedPeriod- |  | OrangePeriod- |  | GreenPeriod- |   */
/*  |                | | Begin      |  |               |  |              |   */
/*  +----------------+ +------------+  +---------------+  +--------------+   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPtcpStatus(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   pMasterSourceAddress,
    OHA_UPPER_MEM_U8_PTR_TYPE   pSubdomainUUID,
    OHA_UPPER_MEM_U8_PTR_TYPE   pIRDataUUID,
    OHA_FRAME_OFFSET_PTR_TYPE pFrameOffset
)
{
    LSA_UINT16                 Response;
    LSA_UINT32                 VLength;
    OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetPtcpStatus(ppTLV: 0x%X, pTLength: 0x%X, pMasterSourceAddress: 0x%X, pSubdomainUUID: 0x%X, pIRDataUUID:  0x%X)",
                          ppTLV, pTLength, pMasterSourceAddress, pSubdomainUUID, pIRDataUUID);

    OHA_ASSERT_NULL_PTR(pMasterSourceAddress);
    OHA_ASSERT_NULL_PTR(pSubdomainUUID);
    OHA_ASSERT_NULL_PTR(pIRDataUUID);
    OHA_ASSERT_NULL_PTR(pFrameOffset);

    Response = OHA_OK;

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL))
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
        VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_PTCP_STATUS_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = OHA_ERR_FAILED;
        }
        else
        {
            OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,OHA_LLDP_ORG_OUI_PNO_1,
                                           OHA_LLDP_ORG_OUI_PNO_2,
                                           OHA_LLDP_ORG_OUI_PNO_3,
                                           OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS);

            /* MasterSourceAddress */
            OHA_MEMCPY(pTLV, pMasterSourceAddress, sizeof(OHA_MAC_TYPE));
            pTLV += sizeof(OHA_MAC_TYPE);

            /* PTCP_SubdomainUUID */
            OHA_MEMCPY(pTLV, pSubdomainUUID, sizeof(OHA_UUID_TYPE));
            pTLV += sizeof(OHA_UUID_TYPE);

            /* PTCP_IRDataUUID */
            OHA_MEMCPY(pTLV, pIRDataUUID, sizeof(OHA_UUID_TYPE));
            pTLV += sizeof(OHA_UUID_TYPE);

            /* LengthOfPeriod */
            OHA_LLDP_SET_U32(pTLV, pFrameOffset->LengthOfPeriod);

            /* LLDP_RedPeriodBegin */
            OHA_LLDP_SET_U32(pTLV, pFrameOffset->RedPeriodBegin);

            /* LLDP_OrangePeriodBegin */
            OHA_LLDP_SET_U32(pTLV, pFrameOffset->OrangePeriodBegin);

            /* LLDP_GreenPeriodBegin */
            OHA_LLDP_SET_U32(pTLV, pFrameOffset->GreenPeriodBegin);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetPtcpStatus(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetUnknownOptionals                           +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+                                                                         +*/
/*+              TLV-Types: 9 ... 126                                       +*/
/*+                    OUI: irrelevant                                      +*/
/*+                Subtype: <>0                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetUnknownOptionals(
    LSA_UINT16                                       NicId,
    LSA_UINT16                                       PortId,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                       pTLength)
{
    LSA_UINT16                 Response;
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    OHA_DB_IF_PTR_TYPE         pDbIf;

    Response = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetUnknownOptionals(ppTLV: 0x%X, pTLength: 0x%X, NicId: 0x%X, PortId: 0x%X)", ppTLV, pTLength, NicId, PortId);

    pDbIf = OHA_DB_GET_INTERFACE(NicId);

    /* check parameters */
    if ( PortId == 0 ||
         PortId > OHA_CFG_MAX_PORTS ||
         LSA_HOST_PTR_ARE_EQUAL(pDbIf,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetUnknownOptionals(Response: 0x%X)", Response);
        return(Response);
    }

    pAnchor = &pDbIf->LldpData[PortId-1].pUnknown_Optionals;

    if (OHA_IS_NOT_NULL(pAnchor))       /* get the elements */
    {
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
            if (pElement->Type > OHA_LLDP_TYPE_MGM_ADDR &&      /* the LLDP-types (9...126) */
                pElement->Type < OHA_LLDP_TYPE_ORG_SPECIFIC &&
                pElement->SubType &&
                pElement->Length  &&                            /* length > 0 */
                OHA_IS_NOT_NULL(pElement->pVariable))
            {
                OHA_UPPER_MEM_U8_PTR_TYPE   pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
                LSA_UINT32                  VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + pElement->Length;

                /* verify TLV fit into */
                if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
                {
                    Response = OHA_ERR_FAILED;
                    break;
                }
                else
                {
                    OHA_LLDP_SET_TL_FIELD(pTLV,pElement->Type,VLength);

                    OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,0,0,0,pElement->SubType);

                    OHA_PDU_SET_MEM(pTLV, pElement->pVariable, pElement->Length);   /* value */

                    *pTLength += LLDP_TL_SIZE + VLength;
                    *ppTLV     = pTLV;
                }
            }
            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetUnknownOptionals(Response: 0x%X)", Response);
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetPNOSpecifics                               +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+                                                                         +*/
/*+              TLV-Types: 127                                             +*/
/*+                    OUI: PNO specific                                    +*/
/*+                Subtype: <>0                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPNOSpecifics(
    LSA_UINT16                                      NicId,
    LSA_UINT16                                      PortId,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                      pTLength)
{
    LSA_UINT16                 Response;
    OHA_DB_IF_PTR_TYPE         pDbIf;
    OHA_COMMON_MEM_U8_PTR_TYPE pPortStatus = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE pDomainUUID = LSA_NULL;
    OHA_COMMON_MEM_U8_PTR_TYPE pPtcpStatus = LSA_NULL;
    LSA_UINT8                  SubType;
    LSA_UINT16                 Length;
    LSA_UINT16                 RetVal;
	LSA_UINT32                 TxEnable    = OHA_LLDP_TX_DISABLE;

    Response = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetPNOSpecifics(ppTLV: 0x%X, pTLength: 0x%X, NicId: 0x%X, PortId: 0x%X)", ppTLV, pTLength, NicId, PortId);

    pDbIf = OHA_DB_GET_INTERFACE(NicId);

    /* check parameters */
    if ( PortId == 0 ||
         PortId > OHA_CFG_MAX_PORTS ||
         LSA_HOST_PTR_ARE_EQUAL(pDbIf,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OhaLldpSetPNOSpecifics(Response: 0x%X)", Response);
        return(Response);
    }

	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_PNO_DELAY,
											 &TxEnable, LSA_NULL, LSA_NULL);

    /* PNIO: port delay info exists if supported (EDD_GET_PARAM-HwType)? */
    if (TxEnable == OHA_LLDP_TX_ENABLE &&
		OHA_EDD_TIMESTAMP_SUPPORTED(pDbIf->HardwareType))
    {
        OHA_UPPER_MEM_U8_PTR_TYPE   pLocDelays = LSA_NULL;
        OHA_UPPER_MEM_U8_PTR_TYPE   pRemDelays = LSA_NULL;

        SubType = OHA_LLDP_SUB_TYPE_PNO_DELAY;
        Length  = 0;

        /* first try to read from local database (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
        (LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                               &SubType,/*SubType*/
                                               OHA_LLDP_TYPE_ORG_OUI_PNO,
                                               &pLocDelays,/* pVariable */
                                               &Length /* Length */);

        if ((Length == OHA_LLDP_PNO_DELAY_SIZE) && (!LSA_HOST_PTR_ARE_EQUAL(pLocDelays, LSA_NULL)))
        {
            LSA_UINT16 SenderId = pDbIf->LldpData[PortId-1].LldpSenderId;

            if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* check it */
            {
                OHA_LLDP_SENDER_PTR_TYPE pDbSender = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1];

                if (pDbSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID ||
                    pDbSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID )
                {
                    SubType = OHA_LLDP_SUB_TYPE_PNO_DELAY;
                    /* try to read from remote database (RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay) */
                    (LSA_VOID)oha_db_read_rem_lldp_element(NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                                           &SubType,/*SubType*/
                                                           OHA_LLDP_TYPE_ORG_OUI_PNO,
                                                           &pRemDelays,/* pVariable */
                                                           &Length /* Length */);
                    if (!((Length == OHA_LLDP_PNO_DELAY_SIZE) && (!LSA_HOST_PTR_ARE_EQUAL(pRemDelays, LSA_NULL))))
                    {
                        pRemDelays = LSA_NULL;  /* no valid element (remote delays) read */
                    }
                }
            }
            RetVal = OhaLldpSetPortDelays(ppTLV, pTLength, pLocDelays, pRemDelays);
            if (RetVal != OHA_OK)
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                      "OhaLldpSetPNOSpecifics: OhaLldpSetPortDelays() failed. (RetVal: 0x%X)",RetVal);
            }
        }
    }

    /* PNIO: IRT port status, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_PNO_PORT_STATUS,
											 &TxEnable, LSA_NULL, LSA_NULL);

    if (TxEnable == OHA_LLDP_TX_ENABLE)
	{
		Length  = 0;
		SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS;

		/* read from LLDP-DB (PortStatus) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pPortStatus, &Length);

		/* set the port status for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pPortStatus, LSA_NULL)) && (Length == OHA_LLDP_PNO_PORT_STATUS_SIZE))
		{
			RetVal = OhaLldpSetPortStatus(ppTLV, pTLength, pPortStatus);
			if (RetVal != OHA_OK)
			{
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
									  "OhaLldpSetPNOSpecifics: OhaLldpSetPortStatus() failed. (RetVal: 0x%X)",RetVal);
			}
		}
	}

    /* PNIO: MRP port status, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_PNO_MRP,
											 &TxEnable, LSA_NULL, LSA_NULL);

    if (TxEnable == OHA_LLDP_TX_ENABLE)
	{
		Length  = 0;
		SubType = OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP;

		/* read from LLDP-DB (PortStatus) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pDomainUUID, &Length);

		/* set the MRP port status for the lldp send request */
		/* LLDP-TLV LLDP_PNIO_MRPPORTSTATUS: LLDP_PNIO_Header, LLDP_PNIO_SubType(0x04),    */
		/*                                   MRP_DomainUUID (16 byte), RRT_Status (2 byte) */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pDomainUUID, LSA_NULL)) && (Length == 2 + sizeof(OHA_UUID_TYPE)))
		{
			LSA_UINT16 PortStatusMrp = OHA_GET16_NTOH (pDomainUUID + sizeof(OHA_UUID_TYPE), 0);

			RetVal = OhaLldpSetPortStatusMrp(ppTLV, pTLength, pDomainUUID, PortStatusMrp);
			if (RetVal != OHA_OK)
			{
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
									  "OhaLldpSetPNOSpecifics: OhaLldpSetPortStatusMrp() failed. (RetVal: 0x%X)",RetVal);
			}
		}
	}

    {
        OHA_COMMON_MEM_U8_PTR_TYPE pChassisMacAdr = LSA_NULL;

        LSA_UINT16 ChassisMACLen = (LSA_UINT16)OHA_DB_READ_MAC_ADDR(NicId, &pChassisMacAdr);

        /* check, if it's an empty string */
        if ((ChassisMACLen == sizeof(OHA_MAC_TYPE)) && OHA_IS_NOT_NULL(pChassisMacAdr))
        {
            /* set the chassis MACAddr for the lldp send request */
            RetVal = OhaLldpSetChassisMacAdr(ppTLV, pTLength, pChassisMacAdr);
            if (RetVal != OHA_OK)
            {
                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                      "OhaLldpSetPNOSpecifics: OhaLldpSetChassisMacAdr() failed. (RetVal: 0x%X)",RetVal);
            }
        }
    }

    /* PNIO: PTCP status, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_PNO_PTCP,
											 &TxEnable, LSA_NULL, LSA_NULL);

    if (TxEnable == OHA_LLDP_TX_ENABLE)
	{
		Length  = 0;
		SubType = OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS;

		/* read from LLDP-DB (PortStatus) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_PNO,
											   &pPtcpStatus, &Length);

		/* set the PTCP status for the lldp send request */
		/* LLDP-TLV LLDP_PNIO_PTCPSTATUS: LLDP_PNIO_Header, LLDP_PNIO_SubType(0x04),                 */
		/*     PTCP_MasterSourceAddress (8 byte), PTCP_SubDomainUUID (16 byte), IRDataUUID (16 byte) */
		/*     LengthOfPeriod (32 bit), RedPeriodBegin (32 bit), OrangePeriodBegin (32 bit), GreenPeriodBegin (32 bit) */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pPtcpStatus, LSA_NULL)) && (Length == OHA_LLDP_PNO_PTCP_STATUS_SIZE))
		{
			OHA_FRAME_OFFSET_TYPE FrameOffset;
			OHA_COMMON_MEM_U8_PTR_TYPE pPtcpOffset = pPtcpStatus+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE+OHA_UUID_SIZE;

			FrameOffset.LengthOfPeriod    = OHA_LLDP_GET_U32(pPtcpOffset);
			FrameOffset.RedPeriodBegin    = OHA_LLDP_GET_U32(pPtcpOffset+sizeof(LSA_UINT32));
			FrameOffset.OrangePeriodBegin = OHA_LLDP_GET_U32(pPtcpOffset+2*sizeof(LSA_UINT32));
			FrameOffset.GreenPeriodBegin  = OHA_LLDP_GET_U32(pPtcpOffset+3*sizeof(LSA_UINT32));

			RetVal = OhaLldpSetPtcpStatus(ppTLV, pTLength, pPtcpStatus,
										  pPtcpStatus+OHA_MAC_ADDR_SIZE,
										  pPtcpStatus+OHA_MAC_ADDR_SIZE+OHA_UUID_SIZE,
										  &FrameOffset);
			if (RetVal != OHA_OK)
			{
				OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
									  "OhaLldpSetPNOSpecifics: OhaLldpSetPortStatusMrp() failed. (RetVal: 0x%X)",RetVal);
			}
		}
	}

    /* PNIO: others */
    {
        /* find further entries in the database */
        OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;  /* org. spec. others */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement;

        pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, 0, OHA_LLDP_TYPE_ORG_OUI_PNO);

        OHA_ASSERT_NULL_PTR(pAnchor);

        pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
			TxEnable = OHA_LLDP_TX_ENABLE;
			if(pElement->SubType == OHA_LLDP_SUB_TYPE_PNO_ALIAS)
			{
				(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_PNO_ALIAS,
														 &TxEnable, LSA_NULL, LSA_NULL);
			}
            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length &&
                pElement->SubType != OHA_LLDP_SUB_TYPE_PNO_DELAY &&         /* already set */
                pElement->SubType != OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS &&   /* already set */
                pElement->SubType != OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP && /* already set */
                pElement->SubType != OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR && /* already set */
                pElement->SubType != OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS)     /* already set */
            {
                LSA_UINT32                 VLength;
                OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

                pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
                VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + pElement->Length;

                /* verify TLV fit into */
                if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
                {
                    Response = OHA_ERR_FAILED;
                    break;
                }
                if(TxEnable == OHA_LLDP_TX_ENABLE)
                {
                    OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

                    OHA_PUT32_HTON (pTLV, 0, (pElement->OUI << 8));
                    (pTLV) += LLDP_TLV_ORG_SPECIFIC_OUI_SIZE;
                    OHA_PDU_SET_U8(pTLV, pElement->SubType);

                    OHA_PDU_SET_MEM(pTLV, pElement->pVariable, pElement->Length);

                    *pTLength += LLDP_TL_SIZE + VLength;
                    *ppTLV     = pTLV;
                }
            }
            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetPNOSpecifics(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetOrgDefOthers                               +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+                                                                         +*/
/*+              TLV-Types: 127                                             +*/
/*+                    OUI: organizationally specific                       +*/
/*+                Subtype: <>0                                             +*/
/*                                                                           */
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC                                                */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetOrgDefOthers(
    LSA_UINT16                                      NicId,
    LSA_UINT16                                      PortId,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                      pTLength)
{
    LSA_UINT16                 Response;
    OHA_DB_IF_PTR_TYPE         pDbIf;
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;  /* org. spec. others */

    Response = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetOrgDefOthers(ppTLV: 0x%X, pTLength: 0x%X, NicId: 0x%X, PortId: 0x%X)", ppTLV, pTLength, NicId, PortId);

    pDbIf = OHA_DB_GET_INTERFACE(NicId);

    /* check parameters */
    if ( PortId == 0 ||
         PortId > OHA_CFG_MAX_PORTS ||
         LSA_HOST_PTR_ARE_EQUAL(pDbIf,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OhaLldpSetOrgDefOthers(Response: 0x%X)", Response);
        return(Response);
    }

    /* find entries in the database */
    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, 0, 0);
    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
    {
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && pElement->SubType)
            {
                LSA_UINT32                 VLength;
                OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

                pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
                VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + pElement->Length;

                /* verify TLV fit into */
                if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
                {
                    Response = OHA_ERR_FAILED;
                    break;
                }
                else
                {
                    OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

                    OHA_PUT32_HTON (pTLV, 0, (pElement->OUI << 8));
                    (pTLV) += LLDP_TLV_ORG_SPECIFIC_OUI_SIZE;
                    OHA_PDU_SET_U8(pTLV, pElement->SubType);

                    OHA_PDU_SET_MEM(pTLV, pElement->pVariable, pElement->Length);

                    *pTLength += LLDP_TL_SIZE + VLength;
                    *ppTLV     = pTLV;
                }
            }
            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetOrgDefOthers(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetIEEE802_1Extensions                        +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+                                                                         +*/
/*+              TLV-Types: 127                                             +*/
/*+                    OUI: IEEE802.1 specific                              +*/
/*+                Subtype: <>0                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetIEEE802_1Extensions(
    LSA_UINT16                                      NicId,
    LSA_UINT16                                      PortId,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                      pTLength)
{
    LSA_UINT16                 Response;
    OHA_DB_IF_PTR_TYPE         pDbIf;
    LSA_UINT8                  SubType;
    LSA_UINT16                 Len;
    OHA_COMMON_MEM_U8_PTR_TYPE pVar;
    LSA_UINT16                 RetVal;
	LSA_UINT32                 TxEnable = OHA_LLDP_TX_DISABLE;

    Response = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetIEEE802_1Extensions(ppTLV: 0x%X, pTLength: 0x%X, NicId: 0x%X, PortId: 0x%X)", ppTLV, pTLength, NicId, PortId);

    pDbIf = OHA_DB_GET_INTERFACE(NicId);

    /* check parameters */
    if ( PortId == 0 ||
         PortId > OHA_CFG_MAX_PORTS ||
         LSA_HOST_PTR_ARE_EQUAL(pDbIf,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OhaLldpSetIEEE802_1Extensions(Response: 0x%X)", Response);
        return(Response);
    }

    /* IEEE 802.1: Port VLanID, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_DOT1_VLAN_ID,
											 &TxEnable, LSA_NULL, LSA_NULL);

	if(TxEnable == OHA_LLDP_TX_ENABLE)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID;

		/* read from LLDP-DB (PortVLanID) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1,
											   &pVar, &Len);

		/* set the VLanID for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == LLDP_TLV_IEEE_802_1_PORT_VLAN_IDENTIFIER_SIZE))
		{
			LLDP_PARAMS_PORT_VLAN_ID_TYPE TLVParams;
			OHA_PDU_GET_U16(TLVParams.PortVLANIdentifier, pVar);
			RetVal = lldp_TLVSetPortVLANID(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetPortVLANID: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    /* IEEE 802.1: Port & Protocol VLanID, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_DOT1_PROTO_VLAN,
											 &TxEnable, LSA_NULL, LSA_NULL);

	if(TxEnable == OHA_LLDP_TX_ENABLE)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_1_TYPE_PPVID;

		/* read from LLDP-DB (PPVLanID) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1,
											   &pVar, &Len);

		/* set the PPVLanID for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == LLDP_TLV_IEEE_802_1_PPVID_FLAGS_SIZE + LLDP_TLV_IEEE_802_1_PPVID_REFERENCE_NUMBER_SIZE ))
		{
			LLDP_PARAMS_PPVID_TYPE TLVParams;
			OHA_PDU_GET_U8(TLVParams.Flags, pVar);
			OHA_PDU_GET_U16(TLVParams.ReferenceNumber, pVar);
			RetVal = lldp_TLVSetPPVID(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetPPVID: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    /* IEEE 802.1: VLan Name, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_DOT1_VLAN_NAME,
											 &TxEnable, LSA_NULL, LSA_NULL);

	if(TxEnable == OHA_LLDP_TX_ENABLE)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME;

		/* read from LLDP-DB (VLan Name) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1,
											   &pVar, &Len);

		/* set the VLan Name for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len >= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE +
			 LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_NAME_LEN_SIZE +
			 LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN ) &&
			(Len <= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE +
			 LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_NAME_LEN_SIZE +
			 LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN ))
		{
			LLDP_PARAMS_VLAN_NAME_TYPE TLVParams;
			OHA_PDU_GET_U16(TLVParams.VLANID, pVar);
			OHA_PDU_GET_U8(TLVParams.VLANNameLength, pVar);
			if (Len == TLVParams.VLANNameLength + LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE +
				LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_NAME_LEN_SIZE)
			{
				OHA_PDU_GET_MEM(TLVParams.VLANName, pVar, Len - LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE -
								LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_NAME_LEN_SIZE);
				RetVal = lldp_TLVSetVLANName(ppTLV, pTLength, &TLVParams);
				if (RetVal != LLDP_RSP_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetVLANName: Invalid Response (0x%X)", RetVal);
				}
			}
			else
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"VLANNameLength (0x%X) does not match with size of element", TLVParams.VLANNameLength);
			}
		}
	}

    /* IEEE 802.1: Protocol Identity, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_DOT1_PROTOCOL,
											 &TxEnable, LSA_NULL, LSA_NULL);

	if(TxEnable == OHA_LLDP_TX_ENABLE)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY;

		/* read from LLDP-DB (Protocol Identity) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1,
											   &pVar, &Len);

		/* set the Protocol Identity for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len >= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_LENGTH_SIZE +
			 LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN ) &&
			(Len <= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_LENGTH_SIZE +
			 LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN ))
		{
			LLDP_PARAMS_PROTOCOL_IDENTITY_TYPE TLVParams;
			OHA_PDU_GET_U8(TLVParams.Length, pVar);
			if (Len == TLVParams.Length + LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_LENGTH_SIZE)
			{
				OHA_PDU_GET_MEM(TLVParams.Identity, pVar, Len - LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_LENGTH_SIZE);
				RetVal = lldp_TLVSetProtIdentity(ppTLV, pTLength, &TLVParams);
				if (RetVal != LLDP_RSP_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetProtIdentity: Invalid Response (0x%X)", RetVal);
				}
			}
			else
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"ProtocolIdentity Length (0x%X) does not match with size of element", TLVParams.Length);
			}
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetIEEE802_1Extensions(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetIEEE802_3Extensions                        +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+                                                                         +*/
/*+              TLV-Types: 127                                             +*/
/*+                    OUI: IEEE802.1 specific                              +*/
/*+                Subtype: <>0                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetIEEE802_3Extensions(
    LSA_UINT16                                      NicId,
    LSA_UINT16                                      PortId,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                      pTLength)
{
    LSA_UINT16                 Response;
    LSA_UINT16                 RetVal;
    OHA_DB_IF_PTR_TYPE         pDbIf;
    LSA_UINT8                  SubType = 0;
    LSA_UINT16                 Len     = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pVar    = LSA_NULL;
	LSA_UINT32                 TxEnable = OHA_DOT3_TLVS_TX_DISABLE;

    Response = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetIEEE802_3Extensions(ppTLV: 0x%X, pTLength: 0x%X, NicId: 0x%X, PortId: 0x%X)", ppTLV, pTLength, NicId, PortId);

    pDbIf = OHA_DB_GET_INTERFACE(NicId);

    /* check parameters */
    if ( PortId == 0 ||
         PortId > OHA_CFG_MAX_PORTS ||
         LSA_HOST_PTR_ARE_EQUAL(pDbIf,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OhaLldpSetIEEE802_3Extensions(Response: 0x%X)", Response);
        return(Response);
    }

    /* IEEE 802.3: MAC/PHY configuration status, if existing */
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId, PortId, OHA_CTRL_DOT3_TLVS,
											 &TxEnable, LSA_NULL, LSA_NULL);

	if(TxEnable & OHA_DOT3_MAC_PHY_CONFIG_STATUS_TX)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS;

		/* read from LLDP-DB (MAC/PHY configuration status) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
											   &pVar, &Len);

		/* set the MAC/PHY configuration status for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == (LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE +
					 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE +
					 LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE)) )
		{
			LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE TLVParams;
			OHA_PDU_GET_U8(TLVParams.AutonegSupportStatus, pVar);
			OHA_PDU_GET_U16(TLVParams.PMDAutonegAdvertisedCap, pVar);
			OHA_PDU_GET_U16(TLVParams.OperationalMAUType, pVar);

			RetVal = lldp_TLVSetMACPHYConfigStatus(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetMACPHYConfigStatus: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    /* IEEE 802.3: Power via MDI, if existing */
	if(TxEnable & OHA_DOT3_POWER_VIA_MDI_TX)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI;

		/* read from LLDP-DB (Power via MDI) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
											   &pVar, &Len);

		/* set the Power via MDI for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_SUPPORT_SIZE +
			 LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_PAIR_SIZE +
			 LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_CLASS_SIZE))
		{
			LLDP_PARAMS_POWER_VIA_MDI_TYPE TLVParams;
			OHA_PDU_GET_U8(TLVParams.MDIPowerSupport, pVar);
			OHA_PDU_GET_U8(TLVParams.PowerPair, pVar);
			OHA_PDU_GET_U8(TLVParams.PowerClass, pVar);
			RetVal = lldp_TLVSetPowerViaMDI(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetPowerViaMDI: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    /* IEEE 802.3: Link aggregation, if existing */
	if(TxEnable & OHA_DOT3_LINK_AGGREGATION_TX)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION;

		/* read from LLDP-DB (Link aggregation) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
											   &pVar, &Len);

		/* set the Link aggregation for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_STATUS_SIZE +
			 LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_PORT_ID_SIZE ))
		{
			LLDP_PARAMS_LINK_AGGREGATION_TYPE TLVParams;
			OHA_PDU_GET_U8(TLVParams.LinkAggregationStatus, pVar);
			OHA_PDU_GET_U32(TLVParams.AggregatedPortID, pVar);
			RetVal = lldp_TLVSetLinkAgg(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetLinkAgg: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    /* IEEE 802.3: Maximum frame size, if existing */
	if(TxEnable & OHA_DOT3_MAX_FRAME_SIZE_TX)
	{
		pVar    = LSA_NULL;
		Len     = 0;
		SubType = LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE;

		/* read from LLDP-DB (Maximum frame size) */
		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
											   &SubType, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3,
											   &pVar, &Len);

		/* set the Maximum frame size for the lldp send request */
		if ((!LSA_HOST_PTR_ARE_EQUAL(pVar, LSA_NULL)) &&
			(Len == LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_FRAME_SIZE_SIZE) )
		{
			LLDP_PARAMS_MAX_FRAME_SIZE_TYPE TLVParams;
			OHA_PDU_GET_U16(TLVParams.MaxFrameSize, pVar);
			RetVal = lldp_TLVSetMaxFrameSize(ppTLV, pTLength, &TLVParams);
			if (RetVal != LLDP_RSP_OK)
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"lldp_TLVSetMaxFrameSize: Invalid Response (0x%X)", RetVal);
			}
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetIEEE802_3Extensions(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OHA_LLDP_TLVValidateMgmAddrLen                       +*/
/*+  Input/Output      OHA_COMMON_MEM_U8_PTR_TYPE               pTLV        +*/
/*+                    LSA_UINT32                               TLength     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pTLV                 : in: Pointer to start of value-field of          +*/
/*+                             mgmaddr-tlv                                 +*/
/*+                             (points to mgmaddrstring length field)      +*/
/*+  TLength              : in: Total remaining length of TLV-chain         +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_DATA_INVALID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Validates the Management Address TLV for Length validness +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 LLDP_LOCAL_FCT_ATTR OHA_LLDP_TLVValidateMgmAddrLen(
    OHA_COMMON_MEM_U8_PTR_TYPE     pTLV,
    LSA_UINT32                     TLength)
{
    LSA_UINT16   Response;
    LSA_UINT32   MgmStrLen;
    LSA_UINT32   OIDStrLen;

    if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE )
    {
        MgmStrLen = pTLV[0];  /* read mgm addr string length */

        /* check for valid parameter */
        if (( MgmStrLen >= LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN ) &&
            ( MgmStrLen <= LLDP_TLV_MGM_ADDR_MAX_MGMT_ADDR_STRING_LEN ))
        {
            /* check against remaining length */
            if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE
                 + MgmStrLen
                 - LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN )
            {
                /* read OIDStrLen */
                OIDStrLen = pTLV[LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE
                                 +MgmStrLen
                                 +LLDP_TLV_MGM_ADDR_IFNUM_SUBTYPE_SIZE
                                 +LLDP_TLV_MGM_ADDR_IFNUM_SIZE];

                /* check for valid parameter */
                if (/*( OIDStrLen  >= LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN ) && *//* always true */
                    ( OIDStrLen  <= LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN ))
                {
                    /* check against remaining length */
                    if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE
                         + MgmStrLen
                         - LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN
                         + OIDStrLen
                         - LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN  )
                    {
                        Response = OHA_OK;
                    }
                    else
                    {
                        Response = OHA_ERR_DATA_INVALID;
                    }
                }
                else
                {
                    Response = OHA_ERR_PARAM;
                }
            }
            else
            {
                Response = OHA_ERR_DATA_INVALID;
            }
        }
        else
        {
            Response = OHA_ERR_PARAM;
        }
    }
    else
    {
        Response = OHA_ERR_DATA_INVALID;
    }

    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OHA_LldpGetMgmAddr                                   +*/
/*+  Input/Output      OHA_UPPER_MEM_U8_PTR_TYPE.           pValue          +*/
/*+                    LSA_UINT32                           Length          +*/
/*+                    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE  pTLVParams      +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pValue            Pointer to address of Value element.                 +*/
/*+  TLength           Length of "Value" from the TLV                       +*/
/*+  pMgmAddr          Pointer to TLV parameters for Management Address     +*/
/*+                                                                         +*/
/*+  Result: OHA_RSP_OK                                                     +*/
/*+          OHA_RSP_ERR_PARAM                                              +*/
/*+          OHA_RSP_ERR_TLV_MISMATCH                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets (read) Parameter Management Address from a TLV-      +*/
/*+               element beginning at "Value" and updates pTLVParams with  +*/
/*+               the TLV specific parameters.                              +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_MGM_ADDR                          +*/
/*                                                                           */
/* Value field of Management Address (LLDP_TLV_TYPE_MGM_ADDR):               */
/*                                                                           */
/*    1 octet       1 octet   1..31 octets    1 octet                        */
/*  +-------------+----------+-------------+------------+--~                 */
/*  |Mgmt addr    |Mgmt addr |Management   |Interface   |                    */
/*  |string length|subtype   |address      |numbering   |                    */
/*  |             |          |             |subtype     |                    */
/*  +-------------+----------|-------------+------------+--~                 */
/*                                                                           */
/*       4 octets    1 octet   0..128 octets                                 */
/*  ~--+-----------+---------+---------------+                               */
/*     |Interface  |OID      |OID identifier |                               */
/*     |number     |string   |address        |                               */
/*     |           |length   |               |                               */
/*  ~--+-----------+---------|---------------+                               */
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpGetMgmAddr(
    OHA_UPPER_MEM_U8_PTR_TYPE               pValue,
    LSA_UINT32                              VLength,
    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE     pMgmAddr)
{
    LSA_UINT16                  Response;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpGetMgmAddr(pValue: 0x%X, VLength: 0x%X, pMgmAddr: 0x%X)", pValue, VLength, pMgmAddr);

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(pValue,LSA_NULL) || VLength == 0 ||
         LSA_HOST_PTR_ARE_EQUAL(pMgmAddr,LSA_NULL) )
    {
        Response = LSA_RET_ERR_PARAM;
    }
    else
    {
        /* Validate Length fields */
        Response = OHA_LLDP_TLVValidateMgmAddrLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,VLength);

        if ( Response == OHA_OK)
        {
            pMgmAddr->MgmtAddrStringLen = *pValue++;
            pMgmAddr->MgmtAddrSubtype   = *pValue++;

            if ( pMgmAddr->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE)
            {
                OHA_MEMCPY( (OHA_COMMON_MEM_PTR_TYPE) pMgmAddr->MgmtAddress,
                            pValue,
                            pMgmAddr->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE);
            }

            pValue += pMgmAddr->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE;

            pMgmAddr->InterfaceNumberingSubtype = *pValue++;
            pMgmAddr->InterfaceNumber           = OHA_LLDP_GET_U32(pValue);
            pValue += 4;

            pMgmAddr->OIDStringLen = *pValue++;

            if ( pMgmAddr->OIDStringLen )
            {
                OHA_MEMCPY( (OHA_COMMON_MEM_PTR_TYPE) pMgmAddr->OIDIdentifier,
                            pValue,
                            pMgmAddr->OIDStringLen);
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpGetMgmAddr(Response: 0x%X)", Response);
    return(Response);
}


/*****************************************************************************/
/*+                                                                         +*/
/*+  Functionname      OHA_TopoPGetPeerMAC                                   +*/
/*+  Input/Output      LSA_UINT16.           NICId                          +*/
/*+                    LSA_UINT16            PortId                         +*/
/*+                    LSA_UINT16            SenderId                       +*/
/*+                                                                         +*/
/*+  Result: LSA_NULL:       no remote sender                               +*/
/*+          <> LSA_NULL:    Pointer to MAC address of remote sender        +*/
/*+                                                                         +*/
/*****************************************************************************/
OHA_COMMON_MEM_U8_PTR_TYPE OHA_TopoGetPeerMAC(LSA_UINT16    NICId,
                                              LSA_UINT16   PortId,
                                              LSA_UINT16   SenderId)
{
    OHA_HDB_LLDP_PTR_TYPE    pHDBLldp;
    OHA_LLDP_SENDER_PTR_TYPE pRemoteSender;
    OHA_COMMON_MEM_PTR_TYPE  MemPtr = LSA_NULL;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_TopoGetPeerMAC(NICId: 0x%X, PortId: 0x%X, SenderId: 0x%X)", NICId, PortId, SenderId);

    pHDBLldp  = OHA_LLDPGetHandleByNIC(NICId);
    if (LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL) ||
        LSA_HOST_PTR_ARE_EQUAL(pHDBLldp->pDB, LSA_NULL))
    {
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_TopoGetPeerMAC - no LLDP existing");
        return LSA_NULL;
    }

    if (PortId && (PortId <= OHA_CFG_MAX_PORTS))
    {
        pRemoteSender = &pHDBLldp->pDB->LldpData[PortId-1].RemoteSender[SenderId-1];
        if (OHA_IS_NOT_NULL(pRemoteSender))
        {
            /* get the sender-status of the port-senders */
            if (pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID ||
                pRemoteSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID)
            {
                MemPtr = &pRemoteSender->SenderMacAddr;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_TopoGetPeerMAC(RetVal: 0x%X)", MemPtr);
    return (OHA_COMMON_MEM_U8_PTR_TYPE)MemPtr;
}


/*****************************************************************************/
/*+                                                                         +*/
/*+  Functionname      oha_write_mrp_port_status                            +*/
/*+  Input/Output      LSA_UINT16.               NICId                      +*/
/*+                    LSA_UINT16                PortId                     +*/
/*+                    OHA_UPPER_MEM_U8_PTR_TYPE pDomainUUID                +*/
/*+                    LSA_UINT16                RRT_State                  +*/
/*+                                                                         +*/
/*+  Result: LSA_RET_OK:       write to LLDP DB successful                  +*/
/*+          <> LSA_RET_OK:    write to LLDP DB failed                      +*/
/*+                                                                         +*/
/*****************************************************************************/
LSA_UINT16 oha_write_mrp_port_status(
    LSA_UINT16     NICId,
    LSA_UINT16     PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE  pDomainUUID,
    LSA_UINT16     RRT_State)
{
    LSA_UINT16 RetVal;
    LSA_UINT32 SetLength = 0;
    LSA_UINT8  MRP_PortStatusTlv[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE];
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR pMRP_PortStatusTlv = MRP_PortStatusTlv;
    OHA_UPPER_MEM_U8_PTR_TYPE   pVal = pDomainUUID;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_write_mrp_port_status(NICId: 0x%X, PortId: 0x%X, pDomainUUID: 0x%X, RRT_State: 0x%X)", NICId, PortId, pDomainUUID, RRT_State);

    RetVal = OhaLldpSetPortStatusMrp(&pMRP_PortStatusTlv, &SetLength, pVal,
                                     RRT_State);
    if (RetVal != LLDP_RSP_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"lldp_TLVSetMACPHYConfigStatus: Invalid Response (0x%X)", RetVal);
        OHA_FATAL(OHA_FATAL_ERR_LLDP);
    }

    /* write updated element to LLDP-DB */
    RetVal = OHA_DB_WRITE_LLDP_ELEMENT(NICId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                       OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                       &MRP_PortStatusTlv[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE],
                                       OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE);
    if (RetVal != LSA_RET_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_EDDLinkIndDone: OHA_DB_WRITE_LLDP_ELEMENT(LLDP_PNIO_MRPPORTSTATUS) failed (RetVal: 0x%X).", RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_write_mrp_port_status(RetVal: 0x%X)", RetVal);
    return (RetVal);
}


/*****************************************************************************/
/*+                                                                         +*/
/*+  Functionname      oha_write_ptcp_status                                +*/
/*+  Input/Output      LSA_UINT16.               NICId                      +*/
/*+                    LSA_UINT16                PortId                     +*/
/*+                    OHA_UPPER_PTCP_DATA_PTR_TYPE pPtcpData               +*/
/*+                                                                         +*/
/*+  Result: LSA_RET_OK:       write to LLDP DB successful                  +*/
/*+          <> LSA_RET_OK:    write to LLDP DB failed                      +*/
/*+                                                                         +*/
/*****************************************************************************/
LSA_UINT16 oha_write_ptcp_status(
    LSA_UINT16     NICId,
    LSA_UINT16     PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE  pMasterSourceAddress,
    OHA_UPPER_MEM_U8_PTR_TYPE  pSubdomainUUID,
    OHA_UPPER_MEM_U8_PTR_TYPE  pIRDataUUID,
    OHA_FRAME_OFFSET_PTR_TYPE pFrameOffset)
{
    LSA_UINT16 RetVal;
    LSA_UINT32 SetLength = 0;
    LSA_UINT8  PTCP_StatusTlv[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_PTCP_STATUS_SIZE];
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR pPTCP_StatusTlv = PTCP_StatusTlv;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_write_ptcp_status(NICId: 0x%X, PortId: 0x%X, pMasterSourceAddress: 0x%X, pFrameOffset: 0x%X)",
                          NICId, PortId, pMasterSourceAddress, pFrameOffset);

    OHA_ASSERT_NULL_PTR(pMasterSourceAddress);
    OHA_ASSERT_NULL_PTR(pSubdomainUUID);
    OHA_ASSERT_NULL_PTR(pIRDataUUID);
    OHA_ASSERT_NULL_PTR(pFrameOffset);

    /* PtcpStatus: PTCP_MasterSourceAddress + PTCP_SubdomainID + IRDataID +     */
    /*     LengthOfPeriod + RedPeriodBegin, OrangePeriodBegin, GreenPeriodBegin */

    /* Coding of the field PTCPSubdomainID: This field shall be coded as data type UUID.  */
    /* The value NULL indicates no syncronisation within the Read Real Sync Data service. */

    RetVal = OhaLldpSetPtcpStatus(&pPTCP_StatusTlv, &SetLength,
                                  pMasterSourceAddress,
                                  pSubdomainUUID,
                                  pIRDataUUID,
                                  pFrameOffset);
    if (RetVal != LSA_RET_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"OhaLldpSetPtcpStatus: Invalid Response (0x%X)", RetVal);
        OHA_FATAL(OHA_FATAL_ERR_LLDP);
    }

    /* write updated element to LLDP-DB */
    RetVal = OHA_DB_WRITE_LLDP_ELEMENT(NICId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC,
                                       OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS, OHA_LLDP_TYPE_ORG_OUI_PNO,
                                       &PTCP_StatusTlv[LLDP_TL_SIZE + LLDP_TLV_ORG_SPECIFIC_MIN_SIZE],
                                       OHA_LLDP_PNO_PTCP_STATUS_SIZE);
    if (RetVal != LSA_RET_OK)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"oha_write_ptcp_status: OHA_DB_WRITE_LLDP_ELEMENT(LLDP_PNIO_PTCPSTATUS) failed (RetVal: 0x%X).", RetVal);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_write_ptcp_status(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_MRPGetHandleByNIC                       +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_MRP_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a MRP HDB with NICId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_GET_HEDD_PTR(0);
    OHA_HDB_MRP_PTR_TYPE  pHDBMrp  = OHA_GET_HMRP_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_MRPGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
    {
        if (( pHDBEdd->State == OHA_HANDLE_STATE_OPEN ) &&
            ( pHDBEdd->Params.pDetail->Params.Edd.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDBEdd++;
            pHDBMrp++;
            i++;
        }
    }

    if ( ! Found )
    {
        pHDBMrp = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_MRPGetHandleByNIC(pHDBMrp:  0x%X)", pHDBMrp);
    return(pHDBMrp);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_REMAGetHandleByNIC                      +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_REMA_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath (global or interface specific)      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a REMA HDB with NICId. If none is found   +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_REMA_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_REMAGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_GET_HREMA_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_REMAGetHandleByNIC(NICId: 0x%X)",
                          NICId);

	while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
	{
		if (pDB->NicId == NICId)
		{
			Found = LSA_TRUE;
		}
		else
		{
			pDB++;
			pHDBRema++;
			i++;
		}
	}

    if ( ! Found )
    {
        pHDBRema = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_REMAGetHandleByNIC(pHDBRema: 0x%X)", pHDBRema);
    return(pHDBRema);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GSYGetHandleByNIC                       +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_HDB_GSY_PTR_TYPE                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a GSY HDB with NICId. If none is found    +*/
/*+               LSA_NULL is returned                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_GSY_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_GSYGetHandleByNIC(
    LSA_UINT16 NICId)
{
    OHA_HDB_EDD_PTR_TYPE  pHDBEdd  = OHA_GET_HEDD_PTR(0);
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy  = OHA_GET_HGSY_PTR(0);
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GSYGetHandleByNIC(NICId: 0x%X)",
                          NICId);

    while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
    {
        if (( pHDBEdd->State == OHA_HANDLE_STATE_OPEN ) &&
            ( pHDBEdd->Params.pDetail->Params.Edd.NICId == NICId  ))
        {
            Found = LSA_TRUE;
        }
        else
        {
            pHDBEdd++;
            pHDBGsy++;
            i++;
        }
    }

    if ( ! Found )
    {
        pHDBGsy = LSA_NULL;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GSYGetHandleByNIC(pHDBGsy:  0x%X)", pHDBGsy);
    return(pHDBGsy);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      OhaLldpSetChassisMacAdr                              +*/
/*+  Input/Output      OHA_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV       +*/
/*+                    OHA_UPPER_MEM_U32_PTR_TYPE               pTLength    +*/
/*+                                                                         +*/
/*+  Result            LSA_UINT16                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV             : in: Pointer to address to put TLV element to.      +*/
/*+                      out:Pointer behind inserted TLV element.           +*/
/*+  pTLength          : in: Length of TLV chain before adding TLV          +*/
/*+                      out:Length of TLV chain after adding TLV           +*/
/*+  pPortStatus       : in: local port status from database for TLV to add +*/
/*+                                                                         +*/
/*+  Result: OHA_OK                                                         +*/
/*+          OHA_ERR_PARAM                                                  +*/
/*+          OHA_ERR_FAILED                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: PNO                                             +*/
/*+                Subtype: 5                                               +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/
/* LLDP TLV-Format:                                                          */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / PNO                                          */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | PNO      |  PNO defined      |                               */
/*  | 00-0E-CF| subtype  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*  PNO defined information string:                                          */
/*                                                                           */
/*      6 octets                                                             */
/*  +---------------------------+                                            */
/*  | LLDP_PNIO_CHASSIS_MAC_ADR |                                            */
/*  |                           |                                            */
/*  +---------------------------+                                            */
/*                                                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetChassisMacAdr(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE                        pChassisMacAdr
)
{
    LSA_UINT16                 Response;
    LSA_UINT32                 VLength;
    OHA_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = OHA_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaLldpSetChassisMacAdr(ppTLV: 0x%X, pTLength: 0x%X, pChassisMacAdr: 0x%X)", ppTLV, pTLength, pChassisMacAdr);

    /* check parameters */
    if ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL)    ||
         LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)   ||
         LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL))
    {
        Response = OHA_ERR_PARAM;
    }
    else
    {
        pTLV    = (OHA_UPPER_MEM_U8_PTR_TYPE)(*ppTLV);
        VLength = LLDP_TLV_ORG_SPECIFIC_MIN_SIZE + OHA_LLDP_PNO_CHASSIS_MAC_ADR_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = OHA_ERR_FAILED;
        }
        else
        {
            OHA_LLDP_SET_TL_FIELD(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,OHA_LLDP_ORG_OUI_PNO_1,
                                           OHA_LLDP_ORG_OUI_PNO_2,
                                           OHA_LLDP_ORG_OUI_PNO_3,
                                           OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR);

            /* LLDP_PNIO_CHASSISMACADR */
            OHA_MEMCPY(pTLV, pChassisMacAdr, sizeof(OHA_MAC_TYPE));
            pTLV += sizeof(OHA_MAC_TYPE);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaLldpSetChassisMacAdr(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbStoreLocalTTL                         +*/
/*+  Input                 :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_UINT16       TTl                        +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+  TimeToLive = min(65535, (TxInterval * TxHoldMultiplier))               +*/
/*+                                                                         +*/
/*+               TxInterval: SYNTAX Integer32 (5..32768 seconds)           +*/
/*+                           DEFVAL { 30 }                                 +*/
/*+                                                                         +*/
/*+               TxHoldMultiplier: SYNTAX Integer32 (2..10 seconds)        +*/
/*+               DEFVAL { 4 }                                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks and writes TimeToLive to the local LLDP DataBase   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbStoreLocalTTL(
    LSA_UINT16 NICId,
    LSA_UINT16 TxInterval,
    LSA_UINT16 TxHoldMultiplier
)
{
    LSA_UINT16              Response;
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp;

    Response = OHA_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbStoreLocalTTL(NICId: 0x%X, TxInterval: 0x%X, TxHoldMultiplier: 0x%X)", NICId, TxInterval, TxHoldMultiplier);

    pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);
    if ( OHA_IS_NULL(pHDBLldp) || OHA_IS_NULL(pHDBLldp->pDB))
    {
        Response = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_DbStoreLocalTTL failed. No LLDP-Channel or no database present (NIC-ID: 0x%X)", NICId);
    }
    else
    {
        LSA_UINT8  LldpTTL[LLDP_TLV_TTL_SIZE];
        LSA_UINT16 Status;
        OHA_UPPER_MEM_U8_PTR_TYPE pLldpTTL = LldpTTL;

        if (TxInterval < OHA_LLDP_TX_INTERVAL_MIN      || TxInterval > OHA_LLDP_TX_INTERVAL_MAX ||          /* TxInterval (5..32768 seconds)    */
            TxHoldMultiplier < OHA_LLDP_TTL_FACTOR_MIN || TxHoldMultiplier > OHA_LLDP_TTL_FACTOR_MAX)   /* TxHoldMultiplier (2..10 seconds) */
        {
            OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"TxInterval(0x%X) or TxHoldMultiplier(0x%X) out of range!", TxInterval, TxHoldMultiplier);
            Response = OHA_ERR_PARAM;
        }
        else
        {
            OHA_PUT16_HTON (pLldpTTL, 0, (LSA_UINT16)(TxInterval * TxHoldMultiplier));

            /* write to LLDP-DB (PortStatus) */
            Status = OHA_DB_WRITE_LLDP_ELEMENT(NICId, 0, OHA_LLDP_TYPE_TTL,
                                               0,/*SubType*/ 0,
                                               LldpTTL,/* pVariable */
                                               LLDP_TLV_TTL_SIZE /* Length */);
            if (Status != LSA_RET_OK)
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_DbStoreLocalTTL: OHA_DB_WRITE_LLDP_ELEMENT(TTL) failed (Status: 0x%X).", Status);
                Response = OHA_ERR_PARAM;
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DbStoreLocalTTL(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbStoreMgmAddress                       +*/
/*+  Input                 :    LSA_UINT16       NICId                      +*/
/*+                        :    OHA_IP_TYPE      Ipaddr                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+  TLV Management Address (Tpe: LLDP_TLV_TYPE_MGM_ADDR, Length: 9-167     +*/
/*+                                                                         +*/
/*+  Value:                                                                 +*/
/*+ +---------+---------+----------+---------+---------+-------+----------+ +*/
/*+ |Mgmt addr|Mgmt addr|Management|Interface|Interface|OID    |OID       | +*/
/*+ |string   |subtype  |address   |numbering|number   |string |identifier| +*/
/*+ |ength    |         |          |subtype  |         |length |          | +*/
/*+ +---------+---------+----------+---------+---------+-------+----------+ +*/
/*+ |1 octet  |1 octet  |1-31 oct. |1 octet  |4 octets |1 octet|0-128 oct.| +*/
/*+ +---------+---------+----------+---------+---------+-------+----------+ +*/
/*+           | Mgmt addr string   |                   | OID string       | +*/
/*+                                                                         +*/
/*+  Description: writes the Management Address to the local LLDP DataBase  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbStoreMgmAddress(
    LSA_UINT16 NICId,
    OHA_COMMON_MEM_U8_PTR_TYPE pIpaddr
)
{
    LSA_UINT16          Response = OHA_OK;
    OHA_DB_IF_PTR_TYPE  pDB;
    LSA_UINT8           LldpMgmAddr[LLDP_TLV_MGM_ADDR_MAX_SIZE];
    LSA_UINT16          Len = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbStoreMgmAddress(NICId: 0x%X, Ipaddr: 0x%X)",
                          NICId, pIpaddr);

    pDB = OHA_DB_GET_INTERFACE(NICId);
    if (OHA_IS_NULL(pDB))
    {
        Response = OHA_ERR_RESOURCE;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_DbStoreMgmAddress failed. No LLDP-Channel or no database present (NIC-ID: 0x%X)", NICId);
    }
    else if (OHA_IS_NULL(pIpaddr))  /* update Management Address (only InterfaceNumber) */
    {
        LSA_UINT16 RetVal;
        LSA_UINT8 SubType = 0; /* subtype of variable, suboption */
        OHA_COMMON_MEM_U8_PTR_TYPE pVariable = LSA_NULL; /* variable in octets */

        RetVal = OHA_DB_READ_LLDP_ELEMENT(NICId, 0,
                                          OHA_LLDP_TYPE_MGM_ADDR, &SubType, 0,
                                          &pVariable, &Len);
        if (RetVal == LSA_RET_OK &&
            (Len >= LLDP_TLV_MGM_ADDR_MIN_SIZE && Len <= LLDP_TLV_MGM_ADDR_MAX_SIZE))
        {
            OHA_MEMCPY(LldpMgmAddr, pVariable, Len);

            /* update the Interface Number */
            pVariable = LldpMgmAddr;

            pVariable += (1 + 1 + OHA_IP_ADDR_SIZE + 1);

            /* InterfaceNumber */
            OHA_PDU_SET_U32 (pVariable, pDB->InterfaceID);
        }
        else
        {
            Response = OHA_ERR_RESOURCE;
            /* Error on getting handle. Abort. */
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_DbStoreMgmAddress failed. No LLDP-Channel or no database present (NIC-ID: 0x%X)", NICId);
        }
    }
    else    /* write Management Address */
    {
        OHA_UPPER_MEM_U8_PTR_TYPE pLldpMgmAddr = LldpMgmAddr;

        LSA_UINT8 LocManAddrOID[32];
        LSA_UINT LocManAddrOID_Len = sizeof(LocManAddrOID);

        (LSA_VOID)oha_lldp_objid_to_asn1(LocManAddrOID, &LocManAddrOID_Len);

        /* MgmtAddrStringLen */
        OHA_PDU_SET_U8 (pLldpMgmAddr, (LSA_UINT16)(OHA_IP_ADDR_SIZE + LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE));

        /* MgmtAddrSubtype */
        OHA_PDU_SET_U8 (pLldpMgmAddr, OHA_ADDR_FAMILY_IPV4); /* IP version 4 */

        /* MgmtAddress */
        OHA_MEMCPY(pLldpMgmAddr, pIpaddr, OHA_IP_ADDR_SIZE);
        pLldpMgmAddr += OHA_IP_ADDR_SIZE;

        /* InterfaceNumberingSubtype */
        OHA_PDU_SET_U8 (pLldpMgmAddr, LLDP_TLV_MGMADDR_IFNUM_IFINDEX);

        /* InterfaceNumber */
        OHA_PDU_SET_U32 (pLldpMgmAddr, pDB->InterfaceID);

        Len = (LSA_UINT16)(LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE +
                           LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE     +
                           OHA_IP_ADDR_SIZE                             +
                           LLDP_TLV_MGM_ADDR_IFNUM_SUBTYPE_SIZE         +
                           LLDP_TLV_MGM_ADDR_IFNUM_SIZE                 +
                           LLDP_TLV_MGM_ADDR_OI_STRING_LENGTH_SIZE      +
                           LocManAddrOID_Len);

        /* OIDStringLen */
        OHA_PDU_SET_U8 (pLldpMgmAddr, (LSA_UINT8)(LocManAddrOID_Len));

        /* copy oid to pdu */
        OHA_MEMCPY(pLldpMgmAddr, LocManAddrOID, LocManAddrOID_Len);
    }

    if (Response == OHA_OK)
    {
        Response = OHA_DB_WRITE_LLDP_ELEMENT(NICId, 0, OHA_LLDP_TYPE_MGM_ADDR, 0, /* NicId, Type, SubType */
                                             0, LldpMgmAddr, Len);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DbStoreMgmAddress(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetSpeedFromSpeedMode                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetSpeedFromSpeedMode(
    LSA_UINT8                   SpeedMode)
{
    LSA_UINT16     Speed;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetSpeedFromSpeedMode(MauType: 0x%X)", SpeedMode);

    switch (SpeedMode)  /* for PNIO */
    {
        case EDD_LINK_10MBIT_HALF:
        case EDD_LINK_10MBIT_FULL:
            Speed = EDD_LINK_SPEED_10;
            break;

        case EDD_LINK_100MBIT_HALF:     /* 100 MBit */
        case EDD_LINK_100MBIT_FULL:
            Speed = EDD_LINK_SPEED_100;
            break;

        case EDD_LINK_1GBIT_HALF:
        case EDD_LINK_1GBIT_FULL: /*   1 GBit */
            Speed = EDD_LINK_SPEED_1000;
            break;

        case EDD_LINK_10GBIT_HALF:
        case EDD_LINK_10GBIT_FULL:
            Speed = EDD_LINK_SPEED_10000;
            break;

        case EDD_LINK_AUTONEG:
        case EDD_LINK_UNCHANGED:        /* only for EDD_SRV_SETUP_PHY ? */
            Speed = EDD_LINK_UNKNOWN;   /* Speed unknown */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromSpeedMode: Speed unknown.");
            break;

        default:        /* other types */
            Speed = EDD_LINK_UNKNOWN;   /* no valid Speed for PNIO */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetSpeedFromSpeedMode: MauType failed.");
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetSpeedFromSpeedMode(Speed: 0x%X)", Speed);
    return(Speed);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetModeFromSpeedMode                    +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks OHA Database elements                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetModeFromSpeedMode(
    LSA_UINT8                   SpeedMode)
{
    LSA_UINT16     Mode;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetModeFromSpeedMode(SpeedMode: 0x%X)", SpeedMode);

    switch (SpeedMode)  /* for PNIO */
    {
        case EDD_LINK_100MBIT_HALF:
        case EDD_LINK_10MBIT_HALF:
        case EDD_LINK_1GBIT_HALF:
        case EDD_LINK_10GBIT_HALF:
            Mode = EDD_LINK_MODE_HALF;
            break;

        case EDD_LINK_100MBIT_FULL:     /* Full-Duplex  */
        case EDD_LINK_10MBIT_FULL:
        case EDD_LINK_1GBIT_FULL:
        case EDD_LINK_10GBIT_FULL:
            Mode = EDD_LINK_MODE_FULL;
            break;

        case EDD_LINK_AUTONEG:
        case EDD_LINK_UNCHANGED:        /* only for EDD_SRV_SETUP_PHY ? */
            Mode = EDD_LINK_UNKNOWN;      /* duplex mode unknown */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetModeFromSpeedMode: duplex mode unknown.");
            break;

        default:        /* other types */
            Mode = EDD_LINK_UNKNOWN;    /* no valid MauType for PNIO */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_GetModeFromSpeedMode: MauType failed.");
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetModeFromSpeedMode(Mode: 0x%X)", Mode);
    return(Mode);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_CalculatePtcpTimeDelay                  +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates the delay for the PTCP diagnosis               +*/
/*+               the grid is 100ms                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_CalculatePtcpTimeDelay(
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    OHA_COMMON_MEM_U16_PTR_TYPE pPtcpDelay)
{
    LSA_UINT16              RetVal;
    LSA_UINT16              NICId;
    OHA_HDB_GSY_PTR_TYPE    pHDBGsy;

    OHA_ASSERT_NULL_PTR(pHDBUser);

	OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_CalculatePtcpTimeDelay(pPtcpDelay: 0x%X, pHDBUser: 0x%X)", pPtcpDelay, pHDBUser);

    OHA_ASSERT_NULL_PTR(pPtcpDelay);

    NICId = pHDBUser->Params.pDetail->Params.User.NICId;
    pHDBGsy  = OHA_GSYGetHandleByNIC(NICId);

    OHA_ASSERT_NULL_PTR(pHDBGsy);

    *pPtcpDelay = 0;

    /* calculate the delay (100ms grid) */
    if (pHDBGsy->SyncDataCommit.Mode == OHA_SYNC_ADD)
    {
        LSA_UINT16 PTCPTimeoutFactor;
        LSA_UINT32 SyncSendFactor;  /* the time base is 31,25 祍 */
        LSA_UINT32 Temp;

        RetVal = OHA_OK;
        PTCPTimeoutFactor = pHDBGsy->SyncDataCommit.PTCPTimeoutFactor;
        SyncSendFactor = pHDBGsy->SyncDataCommit.SyncSendFactor;

        if (PTCPTimeoutFactor == 0)
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CalculatePtcpTimeDelay(RetVal: 0x%X)", RetVal);
            return(RetVal);
        }

        /* PTCPTimeoutFactor    Meaning     */
        /* 0x00                 disabled    */
        /* 0x01 - 0x02          optional    */
        /* 0x03 default,        mandatory   */
        /* 0x04 - 0x0F          mandatory   */
        /* 0x010 - 0xFF         optional    */
        /* 0x100 - 0xFFFF       reserved    */

        /* SyncSendFactor           Meaning     MulticastSelection (Clock)  */
        /* 0x0000                   reserved    (32ms)                      */
        /* 0x0001 - 0x03FF          optional                                */
        /* 0x0400                   default     mandatory (32ms)            */
        /* 0x0401 - 0x18FFF         optional                                */
        /* 0x19000                  default     (3,2s)                      */
        /* 0x19001 - 0xF9FFF        optional                                */
        /* 0xFA000                  default     (32s)                       */
        /* 0x000F423F - 0xA4CB7FFF  optional                                */
        /* 0xA4CB8000               default     (24h)                       */
        /* 0xA4CB8001 - 0xFFFFFFFF  reserved    ParamError ?                */

        /* an "old" record: the SyncSendFactor is 0 (default) */
        if (SyncSendFactor == 0)
        {
            SyncSendFactor = 0x400;
        }

        /* 3 x PTCP_TimeoutFactor x PTCP_SyncSendFactor */
        /* Default: 3 * 3 * (0x400 * 31,25us) = 3 * 3 * 32ms = 288ms (ca.300ms) */
        /* Maximum: 3 * 16 * 32s = 1536s = 25,6min      */

        /* 3 x 31,25us ~ 0.1ms */
        Temp = PTCPTimeoutFactor * SyncSendFactor;
        Temp += 1023;   /* round up */
        Temp >>= 10;    /* division by 1024 -> in 100ms grid */
        *pPtcpDelay = (LSA_UINT16)Temp;
    }
    else
    {
        RetVal = OHA_ERR_PRM_DATA;
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_CalculatePtcpTimeDelay(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_AdminStatusToSendMode                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: converts PortControl-Mode to PortConfig adminstatus       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AdminStatusToSendMode(
    LSA_UINT32                  AdminStatus)
{
    LSA_UINT16     SendMode;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_AdminStatusToSendMode(AdminStatus: 0x%X)", AdminStatus);

    /* AdminStatus: (see LLDP.MIB: lldpPortConfigAdminStatus) */
    switch (AdminStatus)    /* for PNIO */
    {
        case OHA_LLDP_SEND_ONLY:        /* enables only send of lldp-data, txOnly(1) */
        case OHA_LLDP_SEND_RECV:        /* enables send and receive, txAndRx(3) */
										/* AP01201309: start the receiving of lldp data too */
        case OHA_LLDP_RECV_ONLY:          /* enables only receive of lldp-data, rxOnly(2) */
            SendMode = OHA_LLDP_SEND_ON;  /* enables the sending of lldp-data  */
            break;
        case OHA_LLDP_DISABLED:           /* disables lldp, disabled(4) */
            SendMode = OHA_LLDP_SEND_OFF;  /* disables the sending of lldp-data */
            break;
        default:        /* other types */
            SendMode = 0;   /* no valid SendMode for LLDP */
            OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL,"OHA_AdminStatusToSendMode: AdminStatus failed.");
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_AdminStatusToSendMode(SendMode: 0x%X)", SendMode);
    return(SendMode);
}

/*------------------------------------------------------------------------------
  validate ip-address, subnet-mask and gateway-ip
    *
    * checking rules see SPH IP Configuration
    *
  ----------------------------------------------------------------------------*/
LSA_BOOL oha_is_valid_ip_address (
	LSA_UINT32 ip_addr,        /* ip-address in host byte order */
	LSA_UINT32 subnet_mask,    /* subnet-mask in host byte order */
	LSA_UINT32 gateway,        /* ip-address of the gateway in host byte order */
	LSA_BOOL ip_interpretation /* LSA_TRUE ... ip_address 0.0.0.0 is an error too */
)
{

	OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
		"IN : oha_is_valid_ip_address(ip_addr: 0x%X, subnet_mask: 0x%X, gateway: 0x%X)",
		ip_addr, subnet_mask, gateway);

	if (ip_addr == 0) /* delete ip-suite */
	{
		if (subnet_mask != 0 || gateway != 0)
		{
			OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address(): ip-addr 0.0.0.0 but subnet-mask or gateway not 0.0.0.0!");
			return LSA_FALSE;
		}

		if (ip_interpretation)
		{
			OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address(): ip-address 0.0.0.0 not allowed!");
			return LSA_FALSE;
		}
	}
	else
	{
		/*
		// Excerpt from SPH IP Configuration V1.0i10, chapter 3.4.1 (TFS 947725)
		//
		// IP address is invalid if equals...
		// - the subnet address: the host part of the address is ??
		// - the directed broadcast address: the host part of the address is a series of consecutive binary ??
		// - the limited broadcast address (i.e. 255.255.255.255)
		// - the subnet part of the address is ??
		// - an address in the address range from 0.0.0.0 up to 0.255.255.255 (highest 8 bit of the address are 400 set to ?000 0000?
		// - a loopback address: an address in the address range from 127.0.0.0 up to 127.255.255.255 (highest 8 bit of the address are set to ?111 1111?
		// - a multicast address: an address in the address range from 224.0.0.0 up to 239.255.255.255 (highest 4 bit of the address are ?110?
		// - a reserved address: an address in the address range from 240.0.0.0 up to 255.255.255.255 (highest 4 bit of the address are ?111?
		//
		// Subnet mask
		// - consecutive binary ?? followed by a series of consecutive binary ??
		// - not   0.0.0.0 (/0)
		// - not 128.0.0.0 (/1)
		// - not 192.0.0.0 (/2)
		// - not 224.0.0.0 (/3)
		// - not 255.255.255.254 (/31)
		// - not 255.255.255.255 (/32)
		//
		// Gateway
		// - 0.0.0.0 or host's IP address means "no gateway"
		// - a valid IP address
		// - in the same subnet (if check is enabled)
		//
		// History:
		// AP00671263 ... all-1 host address is invalid (since V4.0i80)
		// AP00820668 ... all-0 host address is invalid (since V4.1)
		// AP01334273 ... net 0 is invalid (/mask and /8)
		// AP01417775 ... gateway must not be a broadcast address
		// AP01145327 ... gateway must be in the same subnet
		// AP01314551 ... OHA_CFG_NO_GATEWAY_CHECK to disable AP01145327
		// AP00749757 ... same as AP01314551 for V4.0
		// TFS 827678 ... mask 0 is invalid (/4, /5, /6, /7), obsoleted by TFS 947725
		*/

		switch (subnet_mask)
		{
			case 0x00000000: /* /0 .... invalid because IP-stacks interpret it as "derive mask from ip-address" and not "no subnetting" */
			case 0x80000000: /* /1 .... invalid because of SPH IP Configuration */
			case 0xC0000000: /* /2 .... invalid because of SPH IP Configuration */
			case 0xE0000000: /* /3 .... invalid because of SPH IP Configuration */
			case 0xFFFFFFFE: /* /31 ... invalid because of SPH IP Configuration */
			case 0xFFFFFFFF: /* /32 ... invalid because of SPH IP Configuration */
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (0x%X): not a valid subnet_mask!", subnet_mask);
				return LSA_FALSE;

			default:
				{
				LSA_UINT32 tmp = ~subnet_mask + 1; /* note: if x == 1111000... then ~x == 0000111... then ~x+1 == 0001000... which is 2^x */

				if ((tmp & (tmp - 1)) != 0) /* 2^x check see OpenBSD, param.h */
				{
					OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (0x%X): not a valid subnet_mask!", subnet_mask);
					return LSA_FALSE;
				}
				}
				break;
		}

		/***/

		{
		LSA_UINT32 host0_addr = (ip_addr & subnet_mask);
		LSA_UINT32 host1_addr = (ip_addr | ~subnet_mask);

		if (ip_addr == host0_addr
			|| ip_addr == host1_addr
			|| ip_addr == 0xFFFFFFFF /* 255.255.255.255 (limited broadcast) */
			|| (ip_addr & subnet_mask) == 0 /* net-address {0, <Host-number>} */
			|| (ip_addr & 0xFF000000) == 0  /* net-address 0.0.0.0/8 */
			|| (ip_addr & 0xFF000000) == 0x7F000000 /* 127.0.0.0/8 (loopback) */
			|| (ip_addr & 0xF0000000) == 0xE0000000 /* 224.0.0.0/4 (multicast) */
			|| (ip_addr & 0xF0000000) == 0xF0000000 /* 240.0.0.0/4 (reserved) */
			)
		{
			OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (ip: 0x%X, snm: 0x%X): is not valid!", ip_addr, subnet_mask);
			return LSA_FALSE;
		}
		}

		/***/

		if (gateway == 0 || gateway == ip_addr)
		{
			/* no gateway */
		}
		else
		{
			if (gateway == 0xFFFFFFFF /* 255.255.255.255 (limited broadcast) */
				|| (gateway & 0xFF000000) == 0          /* net-address 0.0.0.0/8 */
				|| (gateway & 0xFF000000) == 0x7F000000 /* 127.0.0.0/8 (loopback) */
				|| (gateway & 0xF0000000) == 0xE0000000 /* 224.0.0.0/4 (multicast) */
				|| (gateway & 0xF0000000) == 0xF0000000 /* 240.0.0.0/4 (reserved) */
				)
			{
				OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (gateway: 0x%X): not valid!", gateway);
				return LSA_FALSE;
			}

#ifndef OHA_CFG_NO_GATEWAY_CHECK /* AP01314551 */
			if ((gateway & subnet_mask) != (ip_addr & subnet_mask))
			{
				OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (ip: 0x%X, gateway: 0x%X, subnet: 0x%X): not in the same subnet!", ip_addr, gateway, subnet_mask);
				return LSA_FALSE;
			}
			else /* in same subnet */
			{
				LSA_UINT32 host0_addr = (gateway & subnet_mask);
				LSA_UINT32 host1_addr = (gateway | ~subnet_mask);

				if (gateway == host0_addr
					|| gateway == host1_addr)
				{
					OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_ip_address (gateway: 0x%X, host0: 0x%X, host1: 0x%X): ip_addr not valid!", gateway, host0_addr, host1_addr);
					return LSA_FALSE;
				}
			}
#endif
		}
	}

	OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_ip_address(): LSA_TRUE");
	return LSA_TRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OHA_DbGetTxInterval                         +*/
/*+  Input                 :    LSA_UINT16       NICId                      +*/
/*+  Result                :    LSA_UINT16       TTl                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+  TimeToLive = min(65535, (TxInterval * TxHoldMultiplier))               +*/
/*+                                                                         +*/
/*+               TxInterval: SYNTAX Integer32 (5..32768 seconds)           +*/
/*+                           DEFVAL { 30 }                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: checks and writes TimeToLive to the local LLDP DataBase   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbGetTxInterval(
    LSA_UINT16                  NICId,
    OHA_COMMON_MEM_U16_PTR_TYPE pTxInterval
)
{
    LSA_UINT16  Response;

    LSA_UINT8                  SubType = 0;
    LSA_UINT16                 Len     = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pVar    = LSA_NULL;
	LSA_UINT32                 TxHoldMultiplier = OHA_LLDP_TTL_FACTOR;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbGetTxInterval(NICId: 0x%X, pTxInterval: 0x%X)", NICId, pTxInterval);

    OHA_ASSERT_NULL_PTR(pTxInterval);

    *pTxInterval = 0;

	(LSA_VOID)OHA_DB_READ_LLDP_MIB_VALUE(OHA_CTRL_TX_HOLD_MULTIPLIER, &TxHoldMultiplier, LSA_NULL, LSA_NULL);

    /* read from LLDP-DB (TimeToLive) */
    (LSA_VOID)oha_db_read_loc_lldp_element(NICId, 0, OHA_LLDP_TYPE_TTL,
                                           &SubType, 0,
                                           &pVar, &Len);

    if (OHA_IS_NOT_NULL(pVar) && Len == LLDP_TLV_TTL_SIZE)
    {
        LSA_UINT16 TimeToLive = OHA_GET16_NTOH (pVar, 0);

        OHA_ASSERT(TimeToLive);

        *pTxInterval = (LSA_UINT16)(TimeToLive / TxHoldMultiplier);  /* TimeToLive = TxInterval * TxHoldMultiplier */

        /* check range */
        OHA_ASSERT(!((*pTxInterval) < OHA_LLDP_TX_INTERVAL_MIN || (*pTxInterval) > OHA_LLDP_TX_INTERVAL_MAX));  /* TxInterval (5..32768 seconds)    */
		Response = OHA_OK;
    }
    else
    {
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"OHA_DbGetTxInterval: oha_db_read_loc_lldp_element(TTL) failed.");
        Response = OHA_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DbGetTxInterval(Response: 0x%X)", Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+  Functionname          :    OhaGetConflictIp                            +*/
/*+  Input                 :    LSA_UINT16       NICId                      +*/
/*+                             OHA_COMMON_MEM_U8_PTR_TYPE *IpAddrPtrPtr    +*/
/*+                             OHA_COMMON_MEM_U8_PTR_TYPE *IpMaskPtrPtr    +*/
/*+                             OHA_COMMON_MEM_U8_PTR_TYPE *GatewayPtrPtr   +*/
/*+  Result                :    -                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the RealIpOhaGetConflictIp.                         +*/
/*      Conflict IP-Suite is the Emergency IP-Suite (if existing) otherwise  */
/*      Conflict IP-Suite is the Expected IP-Suite                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR  OhaGetConflictIp(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpAddrPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpMaskPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * GatewayPtrPtr,
    LSA_BOOL                      * pIpIsEmergency
)
{
    static OHA_IP_TYPE      DefaultAdr = {0,0,0,0};
    OHA_COMMON_MEM_U8_PTR_TYPE pIpSuite = LSA_NULL;
    LSA_BOOL IpIsEmergency = LSA_FALSE;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OhaGetConflictIp(NICId: 0x%X, IpAddrPtrPtr: 0x%X, pIsEmergency: 0x%X)",
                          NicId, IpAddrPtrPtr, pIpIsEmergency);

    (LSA_VOID)oha_db_read_emergency_ip_suite(NicId, &pIpSuite, LSA_NULL, LSA_NULL);
    if (OHA_IS_NOT_NULL(pIpSuite))
    {
        *IpAddrPtrPtr  = pIpSuite;
        *IpMaskPtrPtr  = pIpSuite + OHA_IP_ADDR_SIZE;
        *GatewayPtrPtr = pIpSuite + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
        IpIsEmergency = LSA_TRUE;
    }

    /* no emergency Ip, set the expected Ip */
    if (OHA_IS_NULL(pIpSuite))
    {
        (LSA_VOID)oha_db_read_expected_ip_suite(NicId, &pIpSuite, LSA_NULL, LSA_NULL);
        if (OHA_IS_NOT_NULL(pIpSuite))
        {
            *IpAddrPtrPtr  = pIpSuite;
            *IpMaskPtrPtr  = pIpSuite + OHA_IP_ADDR_SIZE;
            *GatewayPtrPtr = pIpSuite + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE;
        }
    }

    /* no emergency and no expected Ip existing, set {0,0,0,0} */
    if (OHA_IS_NULL(pIpSuite))
    {
        *IpAddrPtrPtr = *IpMaskPtrPtr = *GatewayPtrPtr = DefaultAdr;
    }

    if (OHA_IS_NOT_NULL(pIpIsEmergency))
    {
        *pIpIsEmergency = IpIsEmergency;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OhaGetConflictIp(IpAddrPtrPtr: 0x%X)", IpAddrPtrPtr);
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserSetAddrPending                      +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE      pHDB             +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to USER-HDB                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if User-Request SetAddress, DcpResponse is pending.+*/
/*+               If so we check for free User RCTRL-Blocks and if found    +*/
/*+               we start the handling for this RQB.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR  OHA_UserSetAddrPending (
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16 Status)
{
    OHA_UPPER_RQB_PTR_TYPE  pRQBUser;
    LSA_UINT16              Response;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "IN: OHA_UserSetAddrPending(pHDBUser: 0x%X)", pHDBUser);

    OHA_RQB_REM_BLOCK_BOTTOM(pHDBUser->SockUserReq.pBottom,
                             pHDBUser->SockUserReq.pTop,
                             pRQBUser);

    /* we dont expect LSA_NULL here */
    OHA_ASSERT_NULL_PTR(pRQBUser);

    /* we 'queue' not only one user request */
    if(OHA_IS_NOT_NULL(pHDBUser->SockUserReq.pBottom))	/* AP01335332: Lock IP */
	{
		OHA_ASSERT(OHA_RQB_GET_OPCODE(pHDBUser->SockUserReq.pBottom) == OHA_OPC_DCP_CONTROL);
	}

    switch (OHA_RQB_GET_OPCODE(pRQBUser))
    {
        case OHA_OPC_SET_ADDRESS:
        case OHA_OPC_SET_EMERGENCY:
        {
            /* if SetIp fails -> Status != OK */
            if (Status == OHA_OK)
			{
	            Response = OHA_DoSetAddressRequest(pRQBUser, pHDBUser); /* finish the SetAddress request */
	            OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
			}
			else
			{
                OHA_UserRequestFinish(pHDBUser,pRQBUser,Status);    /* confirm the user */
			}
        }
        break;

        case OHA_OPC_PRM_WRITE:
        {
            /* if SetIp fails -> Response != OK */
            Response = OHA_DoPrmWriteStation(pRQBUser, pHDBUser);   /* finish the PrmWriteStation request */
            OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
        }
        break;

        case OHA_OPC_DCP_RESPONSE:
            /* if SetIp fails -> Response != OK */
            (LSA_VOID)OHA_DoDcpResponseRequest(pRQBUser, pHDBUser); /* finish the DcpResponse request */
            break;

        case OHA_OPC_DCP_WRITE_ELEMENT:
            if (Status == OHA_OK)
            {
                Response = OHA_DoWriteAddressRequest(pRQBUser, pHDBUser);   /* finish the request */
                OHA_UserRequestFinish(pHDBUser,pRQBUser,Response);  /* confirm the user */
            }
            else
            {
                OHA_UserRequestFinish(pHDBUser,pRQBUser,Status);    /* confirm the user */
            }
            break;

        case OHA_OPC_PRM_COMMIT:
            if (Status != OHA_OK)
            {
                OHA_DoPrmPrepare(pHDBUser);

                /* finally the user-indication */
                OHA_UserRequestFinish(pHDBUser,pRQBUser,OHA_ERR_PRM_CONSISTENCY);

                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"SetIpAddress for Prm-Commit (Rtf) failed (Status: 0x%X)!", Status);
            }
            else
            {
                /* a ResetToFactory is done, now SetIp (0.0.0.0), SetName (""),... */
				(LSA_VOID)OHA_DoRtfRequest(pHDBUser->Params.pDetail->Params.User.NICId,
											pHDBUser->Params.pDetail->Params.User.CheckInterval);

                /* finally do the PrmCommit */
                OHA_PrmCommit(pRQBUser, pHDBUser);

                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"Handle Prm-Commit successful");
            }
            break;

        default:
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

	OHA_DoLocalSetAddress(pHDBUser->Params.pDetail->Params.User.NICId); /* AP01335332: Lock IP */

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UserSetAddrPending()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetCountOfInterfaces                    +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: counts OHA interfaces                                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetCountOfInterfaces( LSA_VOID )
{
    LSA_UINT16            IfCount = 0;
    OHA_HDB_EDD_PTR_TYPE  pHDB    = OHA_GET_HEDD_PTR(0);
    LSA_UINT32            i       = 0;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetCountOfInterfaces()");

    /* ----------------------------------------------------------------------*/
    /* Search for opened EDD handles                                         */
    /* ----------------------------------------------------------------------*/
    while (i < OHA_CFG_MAX_NICS)
    {
        if ( pHDB->State == OHA_HANDLE_STATE_OPEN )
        {
            IfCount++;
        }

		i++;
        pHDB++;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetCountOfInterfaces(IfCount: 0x%X)", IfCount);
    return(IfCount);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_GetUsedUserCnt                          +*/
/*+  Input/Output          :    LSA_UINT16      NicID                       +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: counts opened user channels of a NIC                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetUsedUserCnt(
    LSA_UINT16 NicID)
{
    LSA_UINT32            i;
    LSA_UINT16            UserCount = 0;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetUsedUserCnt(NicID: 0x%X)",
                          NicID);

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
		if (NicID != OHA_INVALID_NIC_ID)
		{
			OHA_ASSERT_NULL_PTR(pHDBUser);

			if ( (pHDBUser->State != OHA_HANDLE_STATE_FREE) && /* user channel opened ? */
				  OHA_IS_NOT_NULL(pHDBUser->Params.pDetail))
			{
				if (NicID == pHDBUser->Params.pDetail->Params.User.NICId) /* has this NIC */
				{
					UserCount++;
				}
			}
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetUsedUserCnt(UserCount: 0x%X)", UserCount);
    return(UserCount);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_USERIsAgentActive                       +*/
/*+  Input/Output          :    -                                           +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  InterfaceID                                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: counts opened user channels of a NIC                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_USER_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_USERIsAgentActive(LSA_HANDLE_TYPE Agent,
															   LSA_BOOL OHA_COMMON_MEM_ATTR*pIsRema)
{
    LSA_UINT32            i;
    LSA_UINT16            UserCount = 0;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);
	OHA_HDB_USER_PTR_TYPE pHDBUserRet = LSA_NULL;
	LSA_BOOL IsRema = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_USERIsAgentActive(Agent: 0x%X)", Agent);

	OHA_ASSERT(Agent == OHA_HANDLE_TYPE_LLDP || Agent == OHA_HANDLE_TYPE_MRP)

    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
		if(OHA_IS_NOT_NULL(pHDBUser->SnmpUserReq.pBottom))
		{
			OHA_UPPER_RQB_PTR_TYPE pUserRqb = pHDBUser->SnmpUserReq.pBottom;

			if(Agent == OHA_HANDLE_TYPE_LLDP)
			{
				if(OHA_RQB_GET_OPCODE(pUserRqb) == OHA_OPC_LLDP_CONTROL_OID)
				{
					UserCount++;
					pHDBUserRet = pHDBUser;
					IsRema      = LSA_TRUE;
				}
				if (OHA_RQB_GET_OPCODE(pUserRqb) == OHA_OPC_PRM_WRITE)
				{
					if(pUserRqb->args.PrmData.record_index == OHA_RECORD_INDEX_LLDP_INTERFACE_DATA ||
						pUserRqb->args.PrmData.record_index == OHA_RECORD_INDEX_LLDP_PORT_DATA     ||
						pUserRqb->args.PrmData.record_index == OHA_RECORD_INDEX_MULTIPLE_SNMP)
					{
						UserCount++;
						pHDBUserRet = pHDBUser;
						IsRema      = pUserRqb->args.PrmData.mk_rema;
					}
				}
			}
			else if(Agent == OHA_HANDLE_TYPE_MRP)
			{
				if(OHA_RQB_GET_OPCODE(pUserRqb) == OHA_OPC_MRP_CONTROL_OID)
				{
					UserCount++;
					pHDBUserRet = pHDBUser;
					IsRema      = LSA_TRUE;
				}
			}
		}
    }

	OHA_ASSERT(UserCount < 2);

	if(OHA_IS_NOT_NULL(pIsRema))
	{
		*pIsRema = IsRema;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_USERIsAgentActive(UserCount: 0x%X)", UserCount);
    return(pHDBUserRet);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKIsAgentActive                       +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Agent                   +*/
/*+  Result                :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Agent      : OHA_HANDLE_TYPE_LLDP, OHA_HANDLE_TYPE_MRP                 +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to an active Snmp Agent (LLDP or MRP).       +*/
/*+               If non is active, LSA_NULL is returned!                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_HDB_AGNT_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHA_SOCKIsAgentActive(LSA_HANDLE_TYPE Agent)
{
    OHA_HDB_SOCK_PTR_TYPE pHDBSock = OHA_SOCKGetHandleGlobal();
	OHA_HDB_AGNT_PTR_TYPE pSubAgnt = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "IN : OHA_SOCKIsAgentActive(pHDBSock: 0x%X)", pHDBSock);

    if (OHA_IS_NOT_NULL(pHDBSock))  /* not closed */
    {
		if (Agent == OHA_HANDLE_TYPE_LLDP)
		{
			pSubAgnt = &pHDBSock->LldpAgent;
			if(OHA_IS_NULL(pSubAgnt->SnmpRQBs.pBottom))
			{
				pSubAgnt = LSA_NULL;
			}
		}
		else if(Agent == OHA_HANDLE_TYPE_MRP)
		{
			pSubAgnt = &pHDBSock->MrpAgent;
			if(OHA_IS_NULL(pSubAgnt->SnmpRQBs.pBottom))
			{
				pSubAgnt = LSA_NULL;
			}
		}
		else
		{
			OHA_FATAL(OHA_FATAL_ERR_HANDLE_INVALID);
		}
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW,"No SOCK channel opened. No SOCK handle.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_SOCKIsAgentActive(pSubAgnt: 0x%X)",
                          pSubAgnt);
    return(pSubAgnt);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_SOCKFreeAgent                           +*/
/*+  Input/Output          :    OHA_HDB_AGNT_PTR_TYPE pAgent                +*/
/*+                        :    OHA_HDB_SOCK_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input:                                                                 +*/
/*+                                                                         +*/
/*+  Agent      : LLDP, MRP                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes resources of an active Snmp Agent (LLDP or MRP).  +*/
/*+               If not active, no removal                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_SOCKFreeAgent(OHA_HDB_AGNT_PTR_TYPE  pSubAgnt,
                           OHA_HDB_SOCK_PTR_TYPE  pHDBSock)
{
    OHA_ASSERT_NULL_PTR(pHDBSock);

    while (pSubAgnt->SnmpRQBs.pBottom)
    {
        OHA_SOCK_LOWER_RQB_PTR_TYPE pRxRQB;
        LSA_UINT16                  RetVal = LSA_RET_OK;

        OHA_RQB_REM_BLOCK_BOTTOM(pSubAgnt->SnmpRQBs.pBottom,
                                 pSubAgnt->SnmpRQBs.pTop,
                                 pRxRQB);

        OHA_SOCK_FREE_LOWER_MEM(&RetVal,pRxRQB->args.snmp_recv.buffer,pHDBSock->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);

        OHA_SOCK_FREE_LOWER_RQB(&RetVal,pRxRQB,pHDBSock->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
	}

    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DbGetGlobalIfMode                       +*/
/*+  Input/Output          :    -                                           +*/
/*+  Result                :    Global MultipleInterfaceMode                +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Gets MultipleInterfaceMode from a Database-Interface-Entry +*/
/*+ MultipleInterfaceMode fron any interface is returned if the bit         +*/
/*+ MultipleInterfaceMode.NameOfDevice is set.                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetGlobalIfMode(
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_0,
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_1)
{
    OHA_DB_IF_PTR_TYPE    pDB    = &oha_database.IfData[0];
    LSA_UINT32            i      = 0;
    LSA_UINT32            IfMode = 0;
	LSA_UINT16            CountOfMode_0 = 0;
	LSA_UINT16            CountOfMode_1 = 0;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DbGetGlobalIfMode()");

    while ( i < OHA_CFG_MAX_NICS )
    {
		if (pDB->MultipleIfModeExisting)
		{
			if (pDB->MultipleIfModeValid & OHA_NAME_OF_DEVICE_MASK)
			{
				IfMode = OHA_NAME_OF_DEVICE_MASK;
				CountOfMode_1++;
			}
			else
			{
				CountOfMode_0++;
			}
		}
        pDB++;
        i++;
    }

	if(OHA_IS_NOT_NULL(pCountOfMode_0))
	{
		*pCountOfMode_0 = CountOfMode_0;
	}
	if(OHA_IS_NOT_NULL(pCountOfMode_1))
	{
		*pCountOfMode_1 = CountOfMode_1;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DbGetGlobalIfMode(IfMode: 0x%X)", IfMode);
    return(IfMode);
}

/*****************************************************************************/
/*  end of file OHA_LIB.C                                                    */
/*****************************************************************************/

