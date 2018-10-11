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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_glb.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-global functions                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  13.08.03    JS    ported to LSA_GLOB_P02.01.00.00_00.02.01.01            */
/*                    added err-structure.                                   */
/*  29.06.04    JS    lsa-err-structure may not start 32-bit aligned, so we  */
/*                    have to copy the content bytewise.                     */
/*  10.08.04    JS    use of EDDS_FREE_CHECK macro                           */
/*  28.01.05    JS    LTRC support                                           */
/*  05.07.05    JS    added EDDS_FindLinkAutoMode                            */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  26.09.06    JS    added  LinkSpeedModeCapability                         */
/*  17.01.07    JS    added  Autoneg parameter                               */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  21.02.07    JS    AUTO-Mode detection changed. CLOSED and DISABLED added */
/*  28.02.07    JS    AUTO-Mode detection sets speed/mode with CLOSED/DISAB. */
/*  10.12.08    JS    added PhyStatus                                        */
/*  30.01.09    JS    added MAUType and MediaType                            */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_GLB */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_FatalError                             +*/
/*+  Input/Output               EDDS_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16           ModuleID               +*/
/*+                             LSA_UINT16           Line                   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : EDDS-fatal-errorcode                            +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via EDDS_FATAL_ERROR macro.           +*/
/*+               This function does not return!                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(550,LSAError)
//JB 05/12/2014 system adaption
EDDS_FAR_FCT LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_FatalError(
    EDDS_FATAL_ERROR_TYPE  Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line)

{

    LSA_FATAL_ERROR_TYPE    LSAError;

    LSAError.lsa_component_id  = LSA_COMP_ID_EDDS;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */

    EDDS_CORE_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"Fatal Error. ModID: %d, Line: %d, Error: 0x%X",ModuleID,Line,Error);

    EDDS_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);

    /* should not return! */
    while (1) ;  //lint !e716 JB 11/11/2014 FatalError should never return

}
//lint +esym(550,LSAError)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestFinish                          +*/
/*+  Input/Output               EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                             EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             LSA_RESULT                  Status          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+  Status               : Status to set in RQB                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishs Request by calling call-back-function located in  +*/
/*+               HDB. if Cbf is LSA_NULL noting is done.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RequestFinish(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_RESULT                  Status)
{
	LSA_UINT32	TraceIdx = pHDB->pDDB->pGlob->TraceIdx;

	LSA_UNUSED_ARG(TraceIdx);
	
	EDDS_CORE_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestFinish(pHDB: 0x%X,pRQB: 0x%X, Status: 0x%X)",
                           pHDB,
                           pRQB,
                           Status);


    EDDS_RQB_SET_STATUS(pRQB, Status);
    EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle );

    EDDS_UPPER_TRACE_05(TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<< Request finished. pRQB: 0x%X, OP: 0x%X, SRV: 0x%X, Hdl: 0x%Xh, Rsp: 0x%X",
                        pRQB,
                        EDD_RQB_GET_OPCODE(pRQB),
                        EDD_RQB_GET_SERVICE(pRQB),
                        pHDB->Handle,
                        Status);

    EDDS_CallChannelCbf(pHDB->Cbf,pRQB,pHDB->pSys);

    EDDS_CORE_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    		"OUT:EDDS_RequestFinish()");

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CallCbf                                +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
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

//lint -esym(613,pRQB)
//JB 11/11/2014 pRQB checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful while using this function, make sure that pRQB is not a null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB)
{
    EDDS_CORE_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CallCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",
                           Cbf,
                           pRQB,
                           EDDS_RQB_GET_STATUS(pRQB));


    if (( EDDS_RQB_GET_STATUS(pRQB) != EDD_STS_OK ) &&
        ( EDDS_RQB_GET_STATUS(pRQB) != EDD_STS_OK_CANCEL ))
    {

        EDDS_UPPER_TRACE_04(0,LSA_TRACE_LEVEL_UNEXP,"RQB-Response error [OP: 0x%X, SRV: 0x%X, UHdl: 0x%X, Rsp: 0x%X]",
                            EDD_RQB_GET_OPCODE(pRQB),
                            EDD_RQB_GET_SERVICE(pRQB),
                            EDD_RQB_GET_HANDLE(pRQB),
                            EDD_RQB_GET_RESPONSE(pRQB));

    }

    if ( ! LSA_HOST_PTR_ARE_EQUAL(Cbf,LSA_NULL) )
    {
   		Cbf(pRQB);
    }

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CallCbf()");


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CallChannelCbf                         +*/
/*+  Input/Output               LSA_VOID LSA_FCT_PTR..      Cbf             +*/
/*+                             EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Cbf                  : Pointer to Callbackfunction                     +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calls Cbf with RQB and SysPtr.                            +*/
/*+               if Cbf is LSA_NULL Cbf is not called.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//JB 11/11/2014 pRQB checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful when using this function, make sure that pRQB is not a null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallChannelCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE            pSys)
{

    EDDS_CORE_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CallChannelCbf(Cbf: 0x%X,pRQB: 0x%X, RQB-Status: 0x%X)",
                           Cbf,
                           pRQB,
                           EDDS_RQB_GET_STATUS(pRQB));


    if (( EDDS_RQB_GET_STATUS(pRQB) != EDD_STS_OK ) &&
        ( EDDS_RQB_GET_STATUS(pRQB) != EDD_STS_OK_CANCEL ))
    {

        EDDS_UPPER_TRACE_04(0,LSA_TRACE_LEVEL_UNEXP,"RQB-Response error [OP: 0x%X, SRV: 0x%X, UHdl: 0x%X, Rsp: 0x%X]",
                            EDD_RQB_GET_OPCODE(pRQB),
                            EDD_RQB_GET_SERVICE(pRQB),
                            EDD_RQB_GET_HANDLE(pRQB),
                            EDD_RQB_GET_RESPONSE(pRQB));

    }

    if ( ! LSA_HOST_PTR_ARE_EQUAL(Cbf,LSA_NULL) )
    {
   		EDDS_REQUEST_UPPER_DONE(Cbf,pRQB,pSys);
    }

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CallChannelCbf()");


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_AllocInternalRQB                       +*/
/*+  Input/Output               ----                                        +*/
/*+  Result                :    EDD_UPPER_RQB_PTR_TYPE                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an upper RQB for internal use (with parameter)  +*/
/*+               pParam gets pointer to EDDS_RQB_DEV_INTERNAL_TYPE         +*/
/*+               Returns LSA_NULL if alloc fails                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDD_UPPER_RQB_PTR_TYPE EDDS_LOCAL_FCT_ATTR EDDS_AllocInternalRQB(LSA_VOID)
{

    EDD_UPPER_RQB_PTR_TYPE  pRQB;
    LSA_UINT16              FreeStat;


    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_AllocInternalRQB()");

    EDDS_ALLOC_UPPER_RQB_LOCAL((&pRQB),sizeof(EDD_RQB_TYPE));

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        EDDS_ALLOC_UPPER_MEM_LOCAL((&pRQB->pParam),sizeof(EDDS_RQB_DEV_INTERNAL_TYPE));
        if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
        {
            EDDS_FREE_UPPER_RQB_LOCAL(&FreeStat,pRQB);
            EDDS_FREE_CHECK(FreeStat);
            pRQB = LSA_NULL;
        }

    }


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_AllocInternalRQB(pRQB: 0x%X)",
                           pRQB);

    return(pRQB);


}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_FreeInternalRQB                        +*/
/*+  Input/Output               EDD_UPPER_RQB_PTR_TYPE  pRQB                +*/
/*+  Result                :    EDD_UPPER_RQB_PTR_TYPE                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees an upper RQB allocated with EDDS_AllocInternalRQB   +*/
/*+               Only frees if pointers <> LSA_NULL.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_FreeInternalRQB(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_UINT16              FreeStat;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_FreeInternalRQB(pRQB: 0x%X)",
                           pRQB);


    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL) )
    {
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL))
        {
            EDDS_FREE_UPPER_MEM_LOCAL(&FreeStat,pRQB->pParam);
            EDDS_FREE_CHECK(FreeStat);

        }

        EDDS_FREE_UPPER_RQB_LOCAL(&FreeStat,pRQB);
        EDDS_FREE_CHECK(FreeStat);

    }


    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_FreeInternalRQB()");


}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SetupLinkRQBParams                        +*/
/*+  Input/Output          :    see below                                   +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes internal RQB-Structure                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pInternalRQB)
//JB 11/11/2014 pInternalRQB is checked in previously called funtions
//WARNING: be careful when using this function, make sure not to use pInternalRQB as null ptr!
//AD_DISCUSS pInternalRQB checked in EDDS_CreateDDB
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SetupInternalRQBParams(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pInternalRQB,
    EDD_SERVICE                     Service,
    LSA_HANDLE_TYPE                 Handle,
    EDDS_HANDLE                     hDDB,
    LSA_UINT32                      ID,
    LSA_UINT32                      Param)

{
    pInternalRQB->InUse                 = LSA_FALSE;

    EDD_RQB_SET_OPCODE(pInternalRQB->pRQB,EDD_OPC_SYSTEM);
    EDD_RQB_SET_SERVICE(pInternalRQB->pRQB,Service);
    EDD_RQB_SET_HANDLE(pInternalRQB->pRQB,Handle);

    ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pInternalRQB->pRQB->pParam)->hDDB      = (EDDS_HANDLE) hDDB;
    ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pInternalRQB->pRQB->pParam)->ID         = ID;
    ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pInternalRQB->pRQB->pParam)->Param      = Param;
}
//lint +esym(613,pInternalRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_FindLinkAutoMode                       +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_BOOL : LSA_TRUE if changed              +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finds actual link AUTO mode within Linkstatus of ports    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Priority (shown with 10/100 Mbit only.) The best mode is the AUTO mode +*/
/*+                                                                         +*/
/*+              Status               Speed            Mode                 +*/
/*+   best       LINK_UP              100              Full                 +*/
/*+              LINK_UP              100              Half                 +*/
/*+              LINK_UP              10               Full                 +*/
/*+              LINK_UP              10               Half                 +*/
/*+     |        LINK_UP_CLOSED       100              Full                 +*/
/*+     |        LINK_UP_CLOSED       100              Half                 +*/
/*+     |        LINK_UP_CLOSED       10               Full                 +*/
/*+     |        LINK_UP_CLOSED       10               Half                 +*/
/*+     V        LINK_UP_DISABLED     100              Full                 +*/
/*+              LINK_UP_DISABLED     100              Half                 +*/
/*+              LINK_UP_DISABLED     10               Full                 +*/
/*+              LINK_UP_DISABLED     10               Half                 +*/
/*+   worst      LINK_DOWN            UNKNOWN          UNKNOWN              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checking withing EDDS_GetDDB
//WARNING: be careful when using this function, make sure that pDDB is not a null ptr!
//AD_DISCUSS pDDB is checked EDDS_DeviceSetup and EDDS_RequestPHYInterrupt
LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_FindLinkAutoMode(
    EDDS_LOCAL_DDB_PTR_TYPE  pDDB)

{
    LSA_UINT32          i;
    LSA_UINT8           Status;
    LSA_UINT8           Speed;
    LSA_UINT8           Mode;
    LSA_BOOL            Changed;
    LSA_BOOL            Update;

    Status  = EDD_LINK_DOWN;
    Mode    = EDD_LINK_UNKNOWN;
    Speed   = EDD_LINK_UNKNOWN;
    Changed = LSA_FALSE;

    for ( i=1; i<= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        Update = LSA_FALSE;

        if ( pDDB->pGlob->LinkStatus[i].Status == EDD_LINK_UP )
        {
            /* first port with UP */
            if ( Status != EDD_LINK_UP )
            {
                Status = EDD_LINK_UP;
                Speed  = EDD_LINK_UNKNOWN;
                Mode   = EDD_LINK_UNKNOWN;
            }
            Update = LSA_TRUE;
        }

        /* if not already one port UP and port is CLOSED */
        if (( Status != EDD_LINK_UP ) &&
            ( pDDB->pGlob->LinkStatus[i].Status == EDD_LINK_UP_CLOSED ))
        {
            /* first port with CLOSED */
            if ( Status != EDD_LINK_UP_CLOSED )
            {
                Status = EDD_LINK_UP_CLOSED;
                Speed  = EDD_LINK_UNKNOWN;
                Mode   = EDD_LINK_UNKNOWN;
            }

            #if 1
            Update = LSA_TRUE;
            #endif
        }

        /* if not already one port UP or CLOSED and port is DISABLED */
        if (( Status != EDD_LINK_UP ) &&
            ( Status != EDD_LINK_UP_CLOSED ) &&
            ( pDDB->pGlob->LinkStatus[i].Status == EDD_LINK_UP_DISABLED ))
        {
            /* first port with DISABLED. */
            if ( Status != EDD_LINK_UP_DISABLED )
            {
                Status = EDD_LINK_UP_DISABLED;
                Speed  = EDD_LINK_UNKNOWN;
                Mode   = EDD_LINK_UNKNOWN;
            }
            #if 1
            Update = LSA_TRUE;
            #endif
        }

        /* Update Mode/Speed. Maybe a better Mode/Speed */
        if ( Update )
        {
            /* same speed but better mode ? */
            if (( pDDB->pGlob->LinkStatus[i].Speed == Speed ) &&
                ( pDDB->pGlob->LinkStatus[i].Mode > Mode ))
            {
                Mode = pDDB->pGlob->LinkStatus[i].Mode;
            }
            else
            {
                /* better speed */
                if ( pDDB->pGlob->LinkStatus[i].Speed > Speed )
                {
                    Speed = pDDB->pGlob->LinkStatus[i].Speed;
                    Mode  = pDDB->pGlob->LinkStatus[i].Mode;
                }
            }
        }

    }

    if (( pDDB->pGlob->LinkStatus[0].Status != Status ) ||
        ( pDDB->pGlob->LinkStatus[0].Mode   != Mode   ) ||
        ( pDDB->pGlob->LinkStatus[0].Speed  != Speed  )) Changed = LSA_TRUE;

    /* set AUTO values */
    pDDB->pGlob->LinkStatus[0].Status = Status;
    pDDB->pGlob->LinkStatus[0].Mode   = Mode;
    pDDB->pGlob->LinkStatus[0].Speed  = Speed;

    /* not present with AUTO.. */
    pDDB->pGlob->LinkStatus[0].LineDelay    = 0;
    pDDB->pGlob->LinkStatus[0].CableDelayNs = 0;
    pDDB->pGlob->LinkStatus[0].MAUType      = EDD_MAUTYPE_UNKNOWN;
    pDDB->pGlob->LinkStatus[0].MediaType    = EDD_MEDIATYPE_UNKNOWN;
    pDDB->pGlob->LinkStatus[0].PortStatus   = EDD_PORT_PRESENT;
    pDDB->pGlob->LinkStatus[0].PhyStatus    = EDD_PHY_STATUS_UNKNOWN;
    pDDB->pGlob->LinkStatus[0].AutonegCapAdvertised    = 0;
    pDDB->pGlob->LinkStatus[0].Autoneg      = EDD_AUTONEG_UNKNOWN;

    return(Changed);
}
//lint +esym(613,pDDB)



/*****************************************************************************/
/*  end of file EDDS_GLB.C                                                   */
/*****************************************************************************/

