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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_glb.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP-global functions                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
/*  29.06.04    JS    handling of err-structure changed. alignment !         */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  2
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_GLB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_FatalError                             +*/
/*+  Input/Output               LLDP_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : LLDP-fatal-errorcode                            +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via LLDP_FATAL_ERROR macro.           +*/
/*+               This function does not return!                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_FatalError(
    LLDP_FATAL_ERROR_TYPE  Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line)

{
    LSA_FATAL_ERROR_TYPE    LSAError;

    LSAError.lsa_component_id  = LSA_COMP_ID_LLDP;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */

    LLDP_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"Fatal Error. ModID: %d, Line: %d, Error: %Xh",ModuleID,Line,Error);
    LLDP_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
    /* should not return! */
    while (1) ;

}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_CallCbf                                +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
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

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_CallCbf(
    LLDP_HDB_PTR_TYPE            pHDB,
     LldpUpperCbf                Cbf, /* AP01539304 needs a typedef */
    LLDP_UPPER_RQB_PTR_TYPE      pRQB)
{
    LSA_UINT16                   PathStat;

    LLDP_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_CallCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",Cbf,pRQB,(LLDP_RQB_GET_RESPONSE(pRQB)));
    switch ( LLDP_RQB_GET_RESPONSE(pRQB))
    {
        case LLDP_RSP_OK:
        case LLDP_RSP_OK_CANCEL:
            break;
        default:
            LLDP_UPPER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,"RQB-Response error [opcode: 0x%X, Handle: 0x%X, Resp: 0x%X]",LLDP_RQB_GET_OPCODE(pRQB),LLDP_RQB_GET_HANDLE(pRQB),LLDP_RQB_GET_RESPONSE(pRQB));
            break;
    }
    if (!LSA_HOST_PTR_ARE_EQUAL(Cbf,LSA_NULL) )
    {   /* AP00999034: call macro instead of: Cbf(pRQB); */
        /* AP01252535: release the HDB here and before calling the CBF! */
        LLDP_UPPER_RQB_PTR_TYPE  temppRQB = pRQB;
        LSA_SYS_PTR_TYPE         SysPtr   = pHDB->Params.pSys;
        LLDP_DETAIL_PTR_TYPE     DetailPtr= pHDB->Params.pDetail;

        if(pHDB->ReleaseSysPtr == LSA_TRUE)
        {	/* release the HDB prior calling the CBF */
            LLDP_UserReleaseHDB(pHDB);
            /* call the callback function */
            LLDP_REQUEST_UPPER_DONE(Cbf,temppRQB, SysPtr);

            LLDP_SYSTEM_TRACE_00(0 ,LSA_TRACE_LEVEL_NOTE,"-->: Calling LLDP_RELEASE_PATH_INFO.");
            /* release PATH-Info !                          */
            LLDP_RELEASE_PATH_INFO(&PathStat, SysPtr, DetailPtr);
            LLDP_SYSTEM_TRACE_01(0 ,LSA_TRACE_LEVEL_NOTE,"<--: LLDP_RELEASE_PATH_INFO (Rsp: 0x%X).",PathStat);
            if (PathStat != LSA_RET_OK ) 
            {
                LLDP_FATAL(LLDP_FATAL_ERR_RELEASE_PATH_INFO);
            } 
            pHDB->ReleaseSysPtr = LSA_FALSE;
        }
        else
        {
            LLDP_REQUEST_UPPER_DONE(Cbf,temppRQB, SysPtr);
        }
    }
    LLDP_FUNCTION_TRACE_00(0 ,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_CallCbf()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserRequestFinish                      +*/
/*+  Input/Output               LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+                             LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                             LLDP_RSP                    Response        +*/
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
/*+               Fill the RQB-local-Err-Structure (err) with local LLDP    +*/
/*+               error if lsa_componente_id is LSA_COMP_ID_UNUSED.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_UserRequestFinish(
    LLDP_HDB_PTR_TYPE           pHDB,
    LLDP_UPPER_RQB_PTR_TYPE     pRQB,
    LLDP_RSP                    Response)
{
    LLDP_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserRequestFinish(pHDB: 0x%X,pRQB: 0x%X,Response: 0x%X)",pHDB,pRQB,Response);

    LLDP_RQBSetResponse(pRQB,Response);
    LLDP_RQB_SET_HANDLE(pRQB, pHDB->Params.HandleUpper );

    LLDP_UPPER_TRACE_04(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: Request finished. Opcode: 0x%X, Handle: 0x%Xh,UpperHandle: 0x%X, Response: 0x%X",LLDP_RQB_GET_OPCODE(pRQB),pHDB->ThisHandle,LLDP_RQB_GET_HANDLE(pRQB),Response);
    LLDP_CallCbf(pHDB,pHDB->Params.Cbf,pRQB);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserRequestFinish()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_GetUserHDBFromHandle                   +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE     Handle                  +*/
/*+  Result                :    LLDP_HDB_PTR_TYPE                           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : LLDP user -channel handle to get pointer to HDB for.      +*/
/*+                                                                         +*/
/*+  Result     : Pointer to HDB or LSA_NULL if Handle invalid              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to valid User-HDB for handle.                +*/
/*+               Handle has to be a User-Channel-Handle!!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_HDB_PTR_TYPE  LLDP_LOCAL_FCT_ATTR LLDP_GetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle)
{

    LLDP_HDB_PTR_TYPE pHDB;
    LSA_UINT16        Idx;

    Idx = (LSA_UINT16) (Handle);   /* Get User-Idx */

    if ( Idx < LLDP_MGM_GET_HUSER_PTR()->HandleCnt )
    {
        pHDB = &(LLDP_MGM_GET_HUSER_PTR()->HDB[Idx]);

        /* Handle must be in use (<> free) */
        if ( pHDB->State == LLDP_HANDLE_STATE_FREE )
        {
            pHDB = LSA_NULL;
        }
    }
    else
    {
        LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"Invalid user handle (0x%X). Out of range.",Handle);
        pHDB = LSA_NULL;
    }
    return(pHDB);
}


/*****************************************************************************/
/*  end of file LLDP_GLB.C                                                    */
/*****************************************************************************/

