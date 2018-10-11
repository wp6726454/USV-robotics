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
/*  F i l e               &F: edds_prm.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-PRM-functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  10.03.06    JS    initial version.                                       */
/*  07.04.06    JS    changed drop check from port specific to global.       */
/*  03.05.06    JS    added PDPortData records and changed some naming       */
/*  05.05.06    JS    added Versioncheck                                     */
/*  12.05.06    JS    added padding check. read of a notpresent record       */
/*                    returns noting (instead of a default record)           */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  27.09.06    JS    paramcheck of multicastboundary fixed.                 */
/*  11.12.06    JS    added PDSCF-Record (Read-only)                         */
/*  17.01.07    JS    added ext-link indication if Autoneg setting changed.  */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  22.02.07    JS    unknwon record index defaults to "index error" instead */
/*                    of an empty record                                     */
/*  04.03.07    JS    added parameter MACAddrPrio                            */
/*  23.07.07    JS    added records PeerToPeerBoundary and DCPBoundary but   */
/*                    without handling (only detect and accept records)      */
/*  10.01.08    JS    added PDInterfaceMrpDataAdjust,PDPortMrpDataAdjust     */
/*  04.04.08    JS    added PDSetDefaultPortStates record                    */
/*  12.06.08    JS    change: with EDDS_PRM_PDPORT_DATA_MAUTYPE_1000BASETHD  */
/*                            and  EDDS_PRM_PDPORT_DATA_MAUTYPE_1000BASETFD  */
/*                            check for PhyType EDDS_PHY_TYPE_BASETX instead */
/*                            of EDDS_PHY_TYPE_BASET.                        */
/*  23.06.08    JS    added support for MAUType 1000BASELX10                 */
/*  10.07.08    JS    bugfix within EDDS_EtPortState (portid compare)        */
/*  10.12.08    JS    added PhyStatus                                        */
/*  30.01.09    JS    added MAUType and MediaType. moved MAUType check to LL */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_PRM */

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
/*                                    defines                                */
/*===========================================================================*/

/* Note: EDDS_CFG_PRM_DONT_CHECK_PHYTYPE should not be set.   */
/*       prevents check for PHYTYPE within MAUTYPE.           */

//#ifndef EDDS_CFG_PRM_DONT_CHECK_PHYTYPE
//#define EDDS_PRM_CHECK_PHYTYPE
//#endif

/* Note: EDDS_CFG_PRM_DONT_CHECK_ONE_PORT_UP should not be set. */
/*       prevents check for at least one Port remains UP       */

#ifndef EDDS_CFG_PRM_DONT_CHECK_ONE_PORT_UP
#define EDDS_PRM_CHECK_ONE_PORT_UP
#endif


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CopyUpper2Local                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CopyUpper2Local(
    LSA_UINT8   EDD_UPPER_MEM_ATTR * pSrc,
    LSA_UINT8   EDDS_LOCAL_MEM_ATTR * pDst,
    LSA_UINT32  Length)
{
	EDDS_ASSERT_NULL_PTR(pSrc);
	EDDS_ASSERT_NULL_PTR(pDst);

    while ( Length-- )
    {
        *pDst++ = *pSrc++;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CopyLocal2Upper                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CopyLocal2Upper(
    LSA_UINT8   EDDS_LOCAL_MEM_ATTR * pSrc,
    LSA_UINT8   EDD_UPPER_MEM_ATTR * pDst,
    LSA_UINT32  Length)
{
	EDDS_ASSERT_NULL_PTR(pSrc);
	EDDS_ASSERT_NULL_PTR(pDst);

    while ( Length-- )
    {
        *pDst++ = *pSrc++;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmAlloc                               +*/
/*+  Input/Output          :    EDDS_PRM_PTR_TYPE     pPrm                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates memory within PRM structure. PRM structure      +*/
/*+               has to be zero on entry!                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pPrm)
//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmAlloc(
    EDDS_PRM_PTR_TYPE     pPrm)
{

    LSA_RESULT              Status;

    EDDS_PRM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmAlloc(pPrm: 0x%X)",
                           pPrm);

    Status = EDD_STS_OK;

    EDDS_ALLOC_UPPER_MEM_LOCAL(((EDD_UPPER_MEM_PTR_TYPE *)&pPrm->PortData.pPortIDModeArray),sizeof(EDD_RQB_PORTID_MODE_TYPE));

    if ( LSA_HOST_PTR_ARE_EQUAL(pPrm->PortData.pPortIDModeArray, LSA_NULL) )
    {
        Status = EDD_STS_ERR_RESOURCE;
    }

    EDDS_PRM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmAlloc(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pPrm)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmFree                                +*/
/*+  Input/Output          :    EDDS_PRM_PTR_TYPE     pPrm                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees memory within PRM structure. checks if something to +*/
/*+               free so it can always be called.                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pPrm)
//JB 11/11/2014 pPrm is freed (if not yet LSA_NULL)
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmFree(
    EDDS_PRM_PTR_TYPE     pPrm)
{

    LSA_UINT16              FreeStat;

    EDDS_PRM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmFree(pPrm: 0x%X)",
                           pPrm);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pPrm->PortData.pPortIDModeArray, LSA_NULL))
    {
        EDDS_FREE_UPPER_MEM_LOCAL(&FreeStat,pPrm->PortData.pPortIDModeArray);
        EDDS_FREE_CHECK(FreeStat);
        pPrm->PortData.pPortIDModeArray = LSA_NULL;
    }

    EDDS_PRM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmFree()");
}
//lint +esym(613,pPrm)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmInit                                +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes PRM structures. Structure has to be zero on   +*/
/*+               entry and memory has to be allocated within structures!   +*/
/*+               Called after DDB creation.                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pPrm,pDDB)
//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmInit(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT32             j;
    EDDS_PRM_PTR_TYPE      pPrm;

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmInit(pDDB: 0x%X)",
                           pDDB);

    pPrm = &pDDB->pGlob->Prm;
    pPrm->PrmState = EDDS_PRM_STATE_PREPARED;
    pPrm->FirstCommitExecuted = LSA_FALSE;

    pPrm->FrameDrop.CheckEnable          = LSA_FALSE;
    pPrm->FrameDrop.ErrorStatus          = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.ErrorStatusIndicated = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.RecordSet_A.Present  = LSA_FALSE;
    pPrm->FrameDrop.RecordSet_B.Present  = LSA_FALSE;
    pPrm->FrameDrop.DropCnt              = 0;

    pPrm->PDSCFDataCheck.Present = LSA_FALSE;

    for ( j=0; j<EDDS_FRAME_DROP_BUDGET_CNT; j++)
    {
        pPrm->FrameDrop.RecordSet_A.Budget[j].Enabled = LSA_FALSE;
        pPrm->FrameDrop.RecordSet_B.Budget[j].Enabled = LSA_FALSE;
    }

    pPrm->PRMUsed = LSA_FALSE;

    for ( j=0; j<EDDS_MAX_PORT_CNT; j++)
    {
        pPrm->DoIndicate[j] = LSA_FALSE;

        pPrm->PortData.RecordSet_A[j].Present = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].Present = LSA_FALSE;

        pPrm->PortData.RecordSet_A[j].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_A[j].PreambleLengthPresent    = LSA_FALSE;

        pPrm->PortData.RecordSet_B[j].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_B[j].PreambleLengthPresent    = LSA_FALSE;

        pPrm->MRPPort.Present_A[j]          = LSA_FALSE;
        pPrm->MRPPort.Present_B[j]          = LSA_FALSE;

        pPrm->DefPortStates.RecordSet_A[j].PortStatePresent  = LSA_FALSE;
        pPrm->DefPortStates.RecordSet_B[j].PortStatePresent  = LSA_FALSE;
    }


    pPrm->MRPPort.AtLeastTwoRecordsPresentB = LSA_FALSE;

    pPrm->MRPInterface.Present_A     = LSA_FALSE;
    pPrm->MRPInterface.Present_B     = LSA_FALSE;

    pPrm->DefPortStates.Present_A    = LSA_FALSE;
    pPrm->DefPortStates.Present_B    = LSA_FALSE;


    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmInit()");

}
//lint +esym(613,pPrm,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EtupPrmIndEntry                        +*/
/*+  Input/Output          :    EDD_SRV_PRM_IND_DATA_PTR_TYPE   pPrm        +*/
/*+                             LSA_UINT32                      ErrorStatus +*/
/*+                             LSA_BOOL                        Appear      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pPrm        : Pointer to Diag-entry within IND-RQB                     +*/
/*+  ErrorStatus : ErrorStatus to Setup diag entry for                      +*/
/*+                                                                         +*/
/*+                EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED                +*/
/*+                EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED                +*/
/*+                EDDS_DIAG_FRAME_DROP_ERROR                               +*/
/*+                                                                         +*/
/*+  Appear      : Diag appears (LSA_TRUE) or disappears (LSA_FALSE)        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup entry within indication rqb structure               +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pPrm)
//JB 11/11/2014 can not be a null pointer (checked where called)
//WARNING: be careful when using this function, make sure not to use pPrm as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SetupPrmIndEntry(
    EDD_SRV_PRM_IND_DATA_PTR_TYPE             pPrm,
    LSA_UINT32                                ErrorStatus,
    LSA_BOOL                                  Appear)
{

    switch ( ErrorStatus )
    {
        case EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED:
            pPrm->ChannelProperties  = EDD_SRV_PRM_CH_PROP_MAIN_REQUIRED;
            break;
        case EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED:
            pPrm->ChannelProperties  = EDD_SRV_PRM_CH_PROP_MAIN_DEMANDED;
            break;
        case EDDS_DIAG_FRAME_DROP_ERROR:
        default:
            pPrm->ChannelProperties  = 0;
            break;
    };

    pPrm->ChannelNumber        = EDD_SRV_PRM_CHANNEL_NUMBER;
    pPrm->ChannelProperties   |= (Appear ? EDD_SRV_PRM_CH_PROP_APPEARS : EDD_SRV_PRM_CH_PROP_DISAPPEARS);
    pPrm->ChannelErrorType     = EDD_SRV_PRM_CHANNEL_ERROR_TYPE_NC_MISMATCH;
    pPrm->ExtChannelErrorType  = EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_NC_FD_NR;
    pPrm->ExtChannelAddValue   = 0;

    return;
}
//lint +esym(613,pPrm)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagFrameDropUpdate                    +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT32                  DropCnt         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  DropCnt    : Dropped frames till last call                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called with DropCnt of frames.                            +*/
/*+               Updates current ErrorStatus depending on DropCnt.         +*/
/*+               Shall only be called if FrameDrop check enabled!          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropUpdate(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt)
{

    EDDS_PRM_NC_DATA_PTR_TYPE               pFrameDrop;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagFrameDropUpdate(pDDB: 0x%X, DropCnt: %d)",
                           pDDB,
                           DropCnt);

    pFrameDrop = &pDDB->pGlob->Prm.FrameDrop;

    pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_OK;

    /* Error > Demanded > Required */

    if ( DropCnt != 0 ) /*  usually we have no drops and so we have no errors .. */
    {
        if (pFrameDrop->RecordSet_A.Budget[EDDS_ERROR_BUDGET_IDX].Enabled &&
            DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_ERROR_BUDGET_IDX].Value)
        {
            pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_ERROR;
        }
        else
        {
            if (pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled &&
                DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value)
            {
                pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED;
            }
            else
            {
                if (pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled &&
                    DropCnt >= pFrameDrop->RecordSet_A.Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value)
                {
                    pFrameDrop->ErrorStatus = EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED;
                }
            }
        }
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagFrameDropUpdate()");
}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmSetBToNotPresent                    +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Inits record set B to "not present"                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked before function call
//WARNING: be careful when calling this function, make sure not to use pDDb as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmSetBToNotPresent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT32 i;
    EDDS_PRM_PTR_TYPE      pPrm;

    pPrm = &pDDB->pGlob->Prm;

    pPrm->FrameDrop.RecordSet_B.Present  = LSA_FALSE;

    for (i=0; i<EDDS_MAX_PORT_CNT; i++)
    {
        pPrm->PortData.RecordSet_B[i].Present = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].MAUTypePresent           = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].PortStatePresent         = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].MulticastBoundaryPresent = LSA_FALSE;
        pPrm->PortData.RecordSet_B[i].PreambleLengthPresent    = LSA_FALSE;

        pPrm->MRPPort.Present_B[i] = LSA_FALSE;
        pPrm->DefPortStates.RecordSet_B[i].PortStatePresent = LSA_FALSE;
    }

    pPrm->MRPPort.AtLeastTwoRecordsPresentB = LSA_FALSE;
    pPrm->MRPInterface.Present_B            = LSA_FALSE;

    pPrm->DefPortStates.Present_B    = LSA_FALSE;

}
//lint +esym(613,pDDB)

#ifndef EDDS_DONT_USE_MRP
#ifdef LLIF_CFG_SWITCH_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EtMRPMulticast                         +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT16                  Mode            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  Mode        : EDD_MULTICAST_FWD_ENABLE                                 +*/
/*+                EDD_MULTICAST_FWD_DISABLE                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Multicast FWD for MRP.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EtMRPMulticast(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_UINT16              Mode)

{

    LSA_RESULT                       Status;
    EDD_RQB_PORTID_MODE_TYPE         PortIDMode[EDD_CFG_MAX_PORT_CNT];
    LSA_UINT32                       i;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_EtMRPMulticast(pDDB: 0x%X, Mode: %d)",
                           pDDB,Mode);

    /* we set all Ports */
    for (i = 0; i < pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        PortIDMode[i].Mode    = Mode;
        PortIDMode[i].PortID  = (LSA_UINT16)(i+1);
    }

    if(EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd))
    {
        /* MRP_1 */

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(MRP1: pDDB: 0x%X,Mode: %d)",pDDB,Mode);

        Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(
                     pDDB,
                     PortIDMode,
                     (LSA_UINT16)pDDB->pGlob->HWParams.Caps.PortCnt,
                     EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                     EDD_MAC_ADDR_GROUP_MRP_1,
                     0);

        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(). Status: 0x%X",Status);


        if (Status != EDD_STS_OK)
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_EtMRPMulticast: EDDS_LL_SWITCH_MULTICAST_FWD_CTRL failed!");
        }

        /* MRP_2 */
        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(MRP2: pDDB: 0x%X,Mode: %d)",pDDB,Mode);

        Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(
                     pDDB,
                     PortIDMode,
                     (LSA_UINT16)pDDB->pGlob->HWParams.Caps.PortCnt,
                     EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                     EDD_MAC_ADDR_GROUP_MRP_2,
                     0);

        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(). Status: 0x%X",Status);

        if (Status != EDD_STS_OK)
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_EtMRPMulticast: EDDS_LL_SWITCH_MULTICAST_FWD_CTRL failed!");
        }
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_EtMRPMulticast()");
}
#endif
#endif

#ifdef LLIF_CFG_SWITCH_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EtPortState                            +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+                             LSA_UINT16                  PortState       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  PortID      : PortID to set State for  (1..x)                          +*/
/*+  PortState   : EDD_PORT_STATE_BLOCKING                                  +*/
/*+                EDD_PORT_STATE_FORWARDING                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set PortState of a PortId                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EtPortState(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_UINT16              PortID,
    LSA_UINT16              PortState)
{

    LSA_RESULT                       Status;
    EDD_RQB_SWI_SET_PORT_STATE_TYPE  PortStates;
    LSA_UINT32                       i;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_EtPortState(pDDB: 0x%X, PortID: %d,PortState: %d)",
                           pDDB,PortID,PortState);


    /* we setup all Ports. But only one is changed */
    for (i = 0; i < EDD_CFG_MAX_PORT_CNT; i++)
    {
        if ( (i+1) == PortID )
        {
            PortStates.PortIDState[i] = PortState;
        }
        else
        {
            PortStates.PortIDState[i] = EDD_PORT_STATE_UNCHANGED;
        }
    }

    if(EDDS_LL_AVAILABLE(pDDB,setSwitchPortState))
    {
        EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_SET_PORT_STATE(pDDB: 0x%X, PortID: %d, State: %d)",pDDB,PortID,PortState);

        Status = EDDS_LL_SWITCH_SET_PORT_STATE(
                     pDDB,
                     &PortStates);

        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_SET_PORT_STATE(). Status: 0x%X",Status);

        if (Status != EDD_STS_OK)
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_EtPortState: EDDS_LL_SWITCH_SET_PORT_STATE failed!");
        }
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_EtPortState()");
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmCopySetB2A                          +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Copy all record set from Set B to A. A will be activated  +*/
/*+                                                                         +*/
/*+               If record set is present, DoIndicate is set to TRUE       +*/
/*+               to trigger an indication even with no error pending.      +*/
/*+                                                                         +*/
/*+               PDNC-Record:                                              +*/
/*+               ErrorStatusIndicated will be set to "OK" so pending       +*/
/*+               errors will cause an indication.                          +*/
/*+                                                                         +*/
/*+               PDPort-Record:                                            +*/
/*+               Calls EDDS_LL_SWITCH_MULTICAST_FWD_CTRL for every port    +*/
/*+               Calls EDDS_LL_SET_LINK_STATE for changed ports            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(550,PortStateSet)  
//JB 11/11/2014 used when:  //#JB_TODO -> above
	//#ifndef EDDS_DONT_USE_MRP
    //#ifdef LLIF_CFG_SWITCH_SUPPORT
//lint -esym(613,pDDB)
//JB 11/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmCopySetB2A(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT32             i;
    EDDS_PRM_PTR_TYPE      pPrm;
    LSA_RESULT             Status;
    LSA_UINT8              LinkState;
    LSA_UINT8              PhyStatus;
    LSA_UINT8              Autoneg;
    #if !defined (EDDS_DONT_USE_MRP)
    LSA_BOOL               PortStateSet[EDDS_MAX_PORT_CNT];
    #endif

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmCopySetB2A(pDDB: 0x%X)",
                           pDDB);

    Status = EDD_STS_OK;
    pPrm   = &pDDB->pGlob->Prm;

    pPrm->PRMUsed = LSA_TRUE;  /* we used PRM */

    /* --------------------------------------------------------------------------*/
    /* PDNC-Record (on interface)                                                */
    /* --------------------------------------------------------------------------*/

    pPrm->FrameDrop.RecordSet_A = pPrm->FrameDrop.RecordSet_B;

    /* If the record set is present we have to indicate independend of ErrorStatus */
    /* we set a flag for next indication-check                                     */

    #if 0
    /* force indication if present . Note: index 0 = interface */
    pPrm->DoIndicate[0] = pPrm->FrameDrop.RecordSet_A.Present;
    #else
    /* force indication */
    pPrm->DoIndicate[0] = LSA_TRUE;
    #endif

    pPrm->FrameDrop.ErrorStatus          = EDDS_DIAG_FRAME_DROP_OK;
    pPrm->FrameDrop.ErrorStatusIndicated = EDDS_DIAG_FRAME_DROP_OK;

    /* Set global CheckEnable flag if at least one Budget ist enabled */
    if ( pPrm->FrameDrop.RecordSet_A.Present &&
         ( pPrm->FrameDrop.RecordSet_A.Budget[0].Enabled ||
           pPrm->FrameDrop.RecordSet_A.Budget[1].Enabled ||
           pPrm->FrameDrop.RecordSet_A.Budget[2].Enabled ))
    {
        pPrm->FrameDrop.CheckEnable = LSA_TRUE;
        /* actualize ErrorStatus depending on actual DropCnt */
        EDDS_DiagFrameDropUpdate(pDDB,pPrm->FrameDrop.DropCnt);
    }
    else
    {
        pPrm->FrameDrop.CheckEnable = LSA_FALSE;
    }

    /* --------------------------------------------------------------------------*/
    /* PDSetDefaultPortStates                                                    */
    /* Note: Can only be present on first COMMIT after startup !                 */
    /* --------------------------------------------------------------------------*/

    #if !defined (EDDS_DONT_USE_MRP)
    for (i = 0; i < EDDS_MAX_PORT_CNT; i++)
    {
        /* flag for MRP handling to know if PortState still setup */
        PortStateSet[i] = LSA_FALSE;
    }
    #endif

    #ifdef LLIF_CFG_SWITCH_SUPPORT
    if (pDDB->pGlob->Prm.DefPortStates.Present_B)
    {
        LSA_UINT16 Old_PortState;

        for (i = 0; i < pDDB->pGlob->HWParams.Caps.PortCnt; i++)
        {
            /* PortState present for Port? */
            if ( pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortStatePresent)
            {
                LSA_UINT16  PortState = EDD_PORT_STATE_FORWARDING;

                switch (pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortState )
                {
                    case EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE:
                        PortState = EDD_PORT_STATE_DISABLE;
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: PDSetDefaultPortState: Setting PortState to DISABLE (PortID: %d)!",i+1);
                        break;
                    case EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING:
                        PortState = EDD_PORT_STATE_BLOCKING;
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: PDSetDefaultPortState: Setting PortState to BLOCKING (PortID: %d)!",i+1);
                        break;
                    default:
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDDS_PrmCopySetB2A: Invalid PortState: %d",pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortState);
                        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                        EDDS_MODULE_ID,
                                        __LINE__);
                        break;
                }

                Old_PortState  = pDDB->pGlob->LinkStatus[i+1].PortState;

                pDDB->pGlob->LinkStatus[i+1].PortState = (LSA_UINT16) PortState;

                /* Set the PortState if changed */
                if (Old_PortState != pDDB->pGlob->LinkStatus[i+1].PortState)
                {
                    EDDS_EtPortState(pDDB,(LSA_UINT16) (i+1),PortState);
                    EDDS_IndicateExtLinkChange(pDDB,i+1);
                }

                #if !defined (EDDS_DONT_USE_MRP)
                PortStateSet[i] = LSA_TRUE; /* for later use with MRP! */
                #endif
            }

            /* copy set B to A */
            pDDB->pGlob->Prm.DefPortStates.RecordSet_A[i] = pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i];

        } /* for */
    }

    /* copy state of B to A */
    pDDB->pGlob->Prm.DefPortStates.Present_A = pDDB->pGlob->Prm.DefPortStates.Present_B;
    #endif

    /* --------------------------------------------------------------------------*/
    /* PDInterfaceMrpDataAdjust, PDPortMrpDataAdjust                             */
    /* Note: Only done on first COMMIT !                                         */
    /* --------------------------------------------------------------------------*/

    #ifndef EDDS_DONT_USE_MRP
    #ifdef LLIF_CFG_SWITCH_SUPPORT
    if ( ! pDDB->pGlob->Prm.FirstCommitExecuted ) /* only done on first COMMIT after startup! */
    {
        LSA_UINT16 PortState;
        LSA_BOOL   MrpDisabled;
        LSA_UINT16 Old_PortState;

        /* If MRP is used we set all none MRP-Ports to FORWARDING and all MRP    */
        /* Ports to BLOCKING.                                                    */

        if (0 != pDDB->SWI.MaxMRP_Instances ) /*MRPused? */
        {
            /* if only a Interface record is present MRP is disabled! */
            if ( ( pDDB->pGlob->Prm.MRPInterface.Present_B ) &&
                 (!pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB))
            {
                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
					                "EDDS_PrmCopySetB2A: Only Interface MRP-Record present. MRP is disabled!");
                MrpDisabled = LSA_TRUE;

                /* enable MRP-MC-forwarding (we have no MRP) */
                EDDS_EtMRPMulticast(pDDB,EDD_MULTICAST_FWD_ENABLE);
            }
            else
            {
                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
					                "EDDS_PrmCopySetB2A: Either none or sufficient MRP-records are present. MRP is NOT disabled!");
                MrpDisabled = LSA_FALSE;

                /* disable MRP-MC-forwarding (we have MRP) */
                EDDS_EtMRPMulticast(pDDB,EDD_MULTICAST_FWD_DISABLE);
            }


            /* set PortState for every Port */
            for (i = 0; i < pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {

                /* Three cases: */
                /* 1. no MRP-Record present                    : MRP enabled use DSB setup MRP ports  */
                /* 2. only interface record                    : MRP disabled                         */
                /* 3. at least 2 MRP-Port records/ 1 interface : MRP enabled with this ports          */

                PortState = EDD_PORT_STATE_FORWARDING;

                if ( ! MrpDisabled )
                {
                    if (pDDB->pGlob->Prm.MRPInterface.Present_B)
                    {
                        /* MRP used. Is this a R-Port? (= record present) */
                        if ( pDDB->pGlob->Prm.MRPPort.Present_B[i])
                        {
                            PortState = EDD_PORT_STATE_BLOCKING;
                        }
                    }
                    else
                    {
                        /* No MRP-Records (we already checked that no MRPPort records present two)*/
                        /* -> use startup defaults                                                */

                        if ( EDD_MRP_RING_PORT_DEFAULT == pDDB->SWI.MRPRingPort[i] )
                        {
                            PortState = EDD_PORT_STATE_BLOCKING;
                        }
                    }
                }

                if ( ! PortStateSet[i]) /* set only if not already set by PDSetDefaultPortState !*/
                {
                    Old_PortState = pDDB->pGlob->LinkStatus[i+1].PortState;

                    if ( PortState == EDD_PORT_STATE_BLOCKING )
                    {
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: MRP: Setting PortState to BLOCKING (PortID: %d)!",i+1);
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"EDDS_PrmCopySetB2A: MRP: Setting PortState to FORWARDING (PortID: %d)!",i+1);
                    }

                    pDDB->pGlob->LinkStatus[i+1].PortState = (LSA_UINT16) PortState;

                    /* Set the PortState if changed */
                    if (Old_PortState != pDDB->pGlob->LinkStatus[i+1].PortState)
                    {
                        EDDS_EtPortState(pDDB,(LSA_UINT16)(i+1),PortState);
                        EDDS_IndicateExtLinkChange(pDDB,i+1);
                    }
                }

                /* copy set B to A */
                pDDB->pGlob->Prm.MRPPort.Present_A[i] = pDDB->pGlob->Prm.MRPPort.Present_B[i];

            } /* for */

            /* copy set B to A */
            pDDB->pGlob->Prm.MRPInterface.Present_A = pDDB->pGlob->Prm.MRPInterface.Present_B;
        }
    }
    #endif
    #endif

    /* --------------------------------------------------------------------------*/
    /* PDPort Record                                                             */
    /* --------------------------------------------------------------------------*/

	//lint -esym(550,MACAddrLow)
	//lint -esym(438,MACAddrLow)
	//JB 11/11/2014 only if LLIF_CFG_SWITCH_SUPPORT is not defined
    for (i=0; i<pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        LSA_UINT32 MACAddrLow;

        /* copy set B to A. B maybe not "present" */
        pPrm->PortData.RecordSet_A[i] = pPrm->PortData.RecordSet_B[i];

        /* ----------------------------------------------------------------------*/
        /* If a record for a port was written we have to shoot an indication.    */
        /* We set a flag to do so..                                              */
        /* Note: The following has to be extended if more records supported!     */
        /* ----------------------------------------------------------------------*/

        #if 0
        /* force indication if present . Note: index 0 = interface */
        pPrm->DoIndicate[i+1] = pPrm->PortData.RecordSet_A[i].Present;
        #else
        /* force indication */
        pPrm->DoIndicate[i+1] = LSA_TRUE;
        #endif

        /* ----------------------------------------------------------------------*/
        /* MulticastBoundary                                                     */
        /* ----------------------------------------------------------------------*/

        pPrm->PortData.pPortIDModeArray->Mode    = EDD_MULTICAST_FWD_BITMASK ;
        pPrm->PortData.pPortIDModeArray->PortID  = (LSA_UINT16)(i+1);

        if (( pPrm->PortData.RecordSet_A[i].Present                  ) &&
            ( pPrm->PortData.RecordSet_A[i].MulticastBoundaryPresent ))
        {
            /* configure forward. notation is inverse to prm-record (1=forward) */
            MACAddrLow =  (~pPrm->PortData.RecordSet_A[i].MulticastBoundary);
        }
        else
        {
            /* default if no block present..  forward all */
            MACAddrLow =  0xFFFFFFFF;
        }

        #ifdef LLIF_CFG_SWITCH_SUPPORT
        if(EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd))
        {
            EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X,0x%X,0x%X)",pDDB,EDD_MAC_ADDR_GROUP_RT_2_QUER,MACAddrLow);

            Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,
                                                       pPrm->PortData.pPortIDModeArray,
                                                       1,
                                                       EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                                                       EDD_MAC_ADDR_GROUP_RT_2_QUER,
                                                       MACAddrLow);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(). Status: 0x%X",Status);

            if ( Status != EDD_STS_OK )
            {
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SWITCH_MULTICAST_FWD_CTRL failed (0x%X)!",Status);
            }

            Status = EDD_STS_OK;
        }
        #endif

        /* ----------------------------------------------------------------------*/
        /* DCPBoundary                                                           */
        /* ----------------------------------------------------------------------*/

        pPrm->PortData.pPortIDModeArray->Mode    = EDD_MULTICAST_FWD_BITMASK ;
        pPrm->PortData.pPortIDModeArray->PortID  = (LSA_UINT16)(i+1);

        if (( pPrm->PortData.RecordSet_A[i].Present                  ) &&
            ( pPrm->PortData.RecordSet_A[i].DCPBoundaryPresent       ))
        {
            /* configure forward. notation is inverse to prm-record (1=forward) */
            MACAddrLow =  (~pPrm->PortData.RecordSet_A[i].DCPBoundary);
        }
        else
        {
            /* default if no block present..  forward all */
            MACAddrLow =  0xFFFFFFFF;
        }

        #ifdef LLIF_CFG_SWITCH_SUPPORT
        if(EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd))
        {
            EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X,0x%X,0x%X)",pDDB,EDD_MAC_ADDR_GROUP_DCP,MACAddrLow);

            Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,
                                                       pPrm->PortData.pPortIDModeArray,
                                                       1,
                                                       EDD_MULTICAST_FWD_PRIO_UNCHANGED,
                                                       EDD_MAC_ADDR_GROUP_DCP,
                                                       MACAddrLow);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(). Status: 0x%X",Status);

            if ( Status != EDD_STS_OK )
            {
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SWITCH_MULTICAST_FWD_CTRL failed (0x%X)!",Status);
            }

            Status = EDD_STS_OK;
        }
        #endif

        /* ----------------------------------------------------------------------*/
        /* PortState and MAUType                                                 */
        /* ----------------------------------------------------------------------*/

        /* if no PortStatePresent and no MAUTypePresent -> Autoneg and PowerON */
        if (( ! pPrm->PortData.RecordSet_A[i].PortStatePresent ) &&
            ( ! pPrm->PortData.RecordSet_A[i].MAUTypePresent   ))
        {
            /* set linkstate only if changed */
            if (( pDDB->pGlob->HWParams.LinkSpeedMode[i] != EDD_LINK_AUTONEG ) ||
                ( pDDB->pGlob->HWParams.PHYPower[i]      != EDDS_PHY_POWER_ON ))
            {
                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    i+1,
                                    EDD_LINK_AUTONEG,
                                    EDDS_PHY_POWER_ON);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,i+1,EDD_LINK_AUTONEG,EDDS_PHY_POWER_ON);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    EDD_RQB_GET_LINK_STATUS_TYPE LinkStatus;
                    LSA_UINT16                   MAUType;
                    LSA_UINT32                   PortStatus, AutonegCapAdvertised;
                    LSA_UINT8                    LinkSpeedModeConfigured, MediaType;
                    
                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,i+1);
                    
                    /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. Typically this configuration is identically to the PDPortDataAdjust */
                    /*       parameterization, but the "AutoNeg"-configuration (=default) maybe mapped to a fixed setting if autoneg is not supported.    */
                    /*       Typically the case with POF ports because autoneg is not supported with POF. The actual autoneg to fixed mapping depends on  */
                    /*       phy adaption by LL.                                                                                                          */
                    /*       All other parameters from EDDS_LL_GET_LINK_STATE are not used here!                                                          */
                    Status = EDDS_LL_GET_LINK_STATE(pDDB, i+1, &LinkStatus,&MAUType,&MediaType,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);
                    
                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(LinkSpeedModeConfigured: 0x%X, Status : 0x%X",
                                        LinkSpeedModeConfigured, Status);
                    
                    if( EDD_STS_OK != Status)
                    {
                        EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                            "EDDS_RequestSetupPHY(), Status: 0x%X", Status);
                        
                        EDDS_FatalError(EDDS_FATAL_ERR_LL,
                                        EDDS_MODULE_ID,
                                        __LINE__);
                    }
                    
                    pDDB->pGlob->HWParams.LinkSpeedMode[i] = LinkSpeedModeConfigured;
                    pDDB->pGlob->HWParams.PHYPower[i]      = EDDS_PHY_POWER_ON;
                }
                else
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                }
            }

            Status = EDD_STS_OK;
        }

        /* if PortStatePresent only DOWN allowed. Already checked */
        if (( pPrm->PortData.RecordSet_B[i].PortStatePresent ) &&
            ( pPrm->PortData.RecordSet_B[i].PortState == EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN )) /* should be ! */
        {
            /* set linkstate only if changed */
            if ( pDDB->pGlob->HWParams.PHYPower[i] != EDDS_PHY_POWER_OFF )
            {
                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    i+1,
                                    EDD_LINK_UNCHANGED,
                                    EDDS_PHY_POWER_OFF);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,i+1,EDD_LINK_UNCHANGED,EDDS_PHY_POWER_OFF);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    pDDB->pGlob->HWParams.PHYPower[i]      = EDDS_PHY_POWER_OFF;
                }
                else
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                }
            }

            Status = EDD_STS_OK;
        }

        if ( pPrm->PortData.RecordSet_B[i].MAUTypePresent )
        {
            LinkState = pPrm->PortData.RecordSet_B[i].LinkStat;

            /* set linkstate only if changed */
            if (( pDDB->pGlob->HWParams.LinkSpeedMode[i] != LinkState ) ||
                ( pDDB->pGlob->HWParams.PHYPower[i]      != EDDS_PHY_POWER_ON ))
            {
                EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                    pDDB,
                                    i+1,
                                    LinkState,
                                    EDDS_PHY_POWER_ON);

                Status = EDDS_LL_SET_LINK_STATE(pDDB,i+1,LinkState,EDDS_PHY_POWER_ON);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    EDD_RQB_GET_LINK_STATUS_TYPE LinkStatus;
                    LSA_UINT16                   MAUType;
                    LSA_UINT32                   PortStatus, AutonegCapAdvertised;
                    LSA_UINT8                    LinkSpeedModeConfigured, MediaType;
                    
                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,i+1);
                    
                    /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. Typically this configuration is identically to the PDPortDataAdjust */
                    /*       parameterization, but the "AutoNeg"-configuration (=default) maybe mapped to a fixed setting if autoneg is not supported.    */
                    /*       Typically the case with POF ports because autoneg is not supported with POF. The actual autoneg to fixed mapping depends on  */
                    /*       phy adaption by LL.                                                                                                          */
                    /*       All other parameters from EDDS_LL_GET_LINK_STATE are not used here!                                                          */
                    Status = EDDS_LL_GET_LINK_STATE(pDDB, i+1, &LinkStatus,&MAUType,&MediaType,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);
                    
                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(LinkSpeedModeConfigured: 0x%X, Status : 0x%X",
                                        LinkSpeedModeConfigured, Status);
                    
                    if( EDD_STS_OK != Status)
                    {
                        EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_FATAL,
                                            "EDDS_RequestSetupPHY(), Status: 0x%X", Status);
                        
                        EDDS_FatalError(EDDS_FATAL_ERR_LL,
                                        EDDS_MODULE_ID,
                                        __LINE__);
                    }
                    
                    pDDB->pGlob->HWParams.LinkSpeedMode[i]  = LinkSpeedModeConfigured;
                    pDDB->pGlob->HWParams.PHYPower[i]       = EDDS_PHY_POWER_ON;
                }
                else
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmCopySetB2A: EDDS_LL_SET_LINK_STATE failed (0x%X)!",Status);
                }
            }
        }

        /* Setup PhyStatus */
        if ( pDDB->pGlob->HWParams.PHYPower[i] == EDDS_PHY_POWER_ON )
        {
            PhyStatus = EDD_PHY_STATUS_ENABLED;
        }
        else
        {
            PhyStatus = EDD_PHY_STATUS_DISABLED;
        }

        /* check if Autoneg setting or PhyStatus changed. if so store and do a ext    */
        /* link indication                                                            */
        /* Note: Autoneg and Status/Mode not matching! Autoneg is only the configured!*/
        /*       setting!                                                             */

        Autoneg = EDD_AUTONEG_OFF;
        if ( pDDB->pGlob->HWParams.LinkSpeedMode[i] == EDD_LINK_AUTONEG ) Autoneg = EDD_AUTONEG_ON;

        /* Note: it is possible that the PhyStatus is no DISABELD but the link is still */
        /*       up. This must be taken into account by application. PhyStatus is the   */
        /*       configured value! the actual linkstate maybe changed later.            */

        if ((pDDB->pGlob->LinkStatus[i+1].Autoneg != Autoneg     ) ||
            (pDDB->pGlob->LinkStatus[i+1].PhyStatus != PhyStatus ))/* change ? */
        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            if ((EDDS_PHY_POWER_OFF == pDDB->pGlob->HWParams.PHYPower[i]) &&
                (pDDB->pGlob->LinkStatus[i+1].PhyStatus != PhyStatus))
            {
                if(pDDB->pGlob->RQBInt_PHY.InUse == LSA_FALSE)
                {
                    /* simulate a phy interrupt (only when currently no RQB is en route), */
                    /* because some controller might not generate a phy interrupt on phy  */
                    /* power down */
                    /* NOTE: EDD_SRV_PRM_COMMIT and EDDS_DO_INT_REQUEST_PHY are both      */
                    /*       running in low context, so we don't have a problem here      */
                    EDDS_RequestPHYInterrupt(pDDB);
                }
            }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

            /* Store configured Autoneg setting */
            pDDB->pGlob->LinkStatus[i+1].Autoneg = Autoneg;
            pDDB->pGlob->LinkStatus[i+1].PhyStatus  = PhyStatus;
            EDDS_IndicateExtLinkChange(pDDB,i+1);
        }

    }	
	//lint +esym(438,MACAddrLow)
	//lint +esym(550,MACAddrLow)

    pPrm->FirstCommitExecuted = LSA_TRUE;  /* at least the first COMMIT is executed */

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmCopySetB2A()");

}
//lint +esym(613,pDDB)
//lint +esym(550,PortStateSet)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmIndicatePort                        +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  PortID     : PortID to indicate for (0=interface, 1.. EDDS_MAX_PORT_CNT)+*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called to check if a Diag has to be indicated for PortID  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use it with pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmIndicatePort(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT16                  PortID)
{

    LSA_RESULT                                Status;
    EDD_UPPER_RQB_PTR_TYPE                    pRQB;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE         pRQBParam;
    EDDS_PRM_PTR_TYPE                         pPrm;
    EDDS_LOCAL_HDB_PTR_TYPE                   pHDB;
    LSA_UINT32                                Cnt;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmIndicatePort(pDDB: 0x%X,PortID: %d)",
                           pDDB,
                           PortID);

    //Status     = EDD_STS_OK;
    pPrm       = &pDDB->pGlob->Prm;
    Cnt        = 0;

    Status = EDDS_HandleGetHDB(pPrm->Handle, &pHDB);

    if ( Status != EDD_STS_OK )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    pRQB = pPrm->PrmIndReq[PortID].pBottom;

    if ( ! ( LSA_HOST_PTR_ARE_EQUAL( pRQB, LSA_NULL) ))  /* Indication Resource present ! */
    {
        pRQBParam = (EDD_UPPER_PRM_INDICATION_PTR_TYPE) pRQB->pParam;

        switch (PortID)
        {
            case 0:  /* interface */
                /* --------------------------------------------------------------*/
                /* Check PDNC (frame drop)                                       */
                /* --------------------------------------------------------------*/

                /* enabled ? */
                if ( pPrm->FrameDrop.CheckEnable )
                {
                    if ( pPrm->FrameDrop.ErrorStatus != pPrm->FrameDrop.ErrorStatusIndicated )
                    {
                        if ( pPrm->FrameDrop.ErrorStatusIndicated != EDDS_DIAG_FRAME_DROP_OK )
                        {
                            /* disappear */
                            EDDS_SetupPrmIndEntry(&pRQBParam->diag[0],
                                                 pPrm->FrameDrop.ErrorStatusIndicated,
                                                 LSA_FALSE);
                            Cnt++;
                        }

                        if ( pPrm->FrameDrop.ErrorStatus != EDDS_DIAG_FRAME_DROP_OK )
                        {
                            /* appear */
                            EDDS_SetupPrmIndEntry(&pRQBParam->diag[Cnt],
                                                 pPrm->FrameDrop.ErrorStatus,
                                                 LSA_TRUE);
                            Cnt++;
                        }

                        pPrm->FrameDrop.ErrorStatusIndicated = pPrm->FrameDrop.ErrorStatus;
                    }
                }

                break;
            default: /* ports */
                /* --------------------------------------------------------------*/
                /* Check PDPort (nothing to indicate at all)                     */
                /* --------------------------------------------------------------*/
                break;
        }

        if (Cnt > EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES )
        {
            /* Cnt exceeds space within RQB! */
            EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                            EDDS_MODULE_ID,
                            __LINE__);
        }

        /* If DoIndicate is TRUE we have to indicate if Cnt = 0 */
        /* this is after a COMMIT with all ports a record was   */
        /* written.                                             */

        if (( Cnt > 0 ) || (pPrm->DoIndicate[PortID] ))
        {
            EDDS_RQB_REM_BLOCK_BOTTOM(pPrm->PrmIndReq[PortID].pBottom,
                                      pPrm->PrmIndReq[PortID].pTop,
                                      pRQB);

            if ( LSA_HOST_PTR_ARE_EQUAL( pRQB, LSA_NULL) )
            {
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                EDDS_MODULE_ID,
                                __LINE__);
            }

            pRQBParam->diag_cnt      = (LSA_UINT16) Cnt;
            pRQBParam->edd_port_id   = PortID;

            pPrm->DoIndicate[PortID] = LSA_FALSE;

            EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_PrmIndicatePort: Indicate for PortID %d. Entrys: %d",
                                  PortID,
                                  Cnt);

            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
        }
    }
    else
    {
        EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDS_PrmIndicatePort: No Indication resources present.");
    }

    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmIndicatePort()");
}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmIndicateAll                         +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called to check if a Diag has to be indicated for any Port+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use it with pDDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmIndicateAll(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_UINT16    i;

    for (i=0; i<=pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {
        EDDS_PrmIndicatePort(pDDB,i); /* Note: 0=Interface */
    }

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagFrameDropHandler                   +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_UINT32                  DropCnt         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  DropCnt    : Dropped frames till last call                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called with DropCnt of frames.                            +*/
/*+               Checks for current ErrorStatus and indicates if changed   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_ENTER_CHECK_REQUEST
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropHandler(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt)
{

	EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagFrameDropHandler(pDDB: 0x%X, DropCnt: %d)",
                           pDDB,
                           DropCnt);

    if ( LSA_FALSE != pDDB->pGlob->Prm.FrameDrop.CheckEnable )
    {
        EDDS_DiagFrameDropUpdate(pDDB,DropCnt);
        EDDS_PrmIndicatePort(pDDB,0); /* FrameDrop is on Interface */
    }

    pDDB->pGlob->Prm.FrameDrop.DropCnt = DropCnt;  /* always save actual drop cnt */


    EDDS_PRM_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagFrameDropHandler()");
}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmValidateConsistency                 +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_BOOL                    Local           +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Local      : LSA_TRUE : Local  parametration                           +*/
/*+               LSA_FALSE: Remote parametration                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_CONSISTENCY                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks is all record set Bs are consistent.               +*/
/*+                                                                         +*/
/*+               PDPortDataAdjust: At least one PortState has to be UP     +*/
/*+                                 with Autoneg or with Mautype 100Mbit/   +*/
/*+                                 1Gbit/10Gbit and fullduplex.            +*/
/*                                  if Local = LSA_FALSE                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmValidateConsistency(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_BOOL                    Local)
{

    LSA_RESULT                               Status;
    LSA_UINT32                               i;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE pRecordSetB;
    LSA_BOOL                                 Valid;
    #ifndef EDDS_DONT_USE_MRP		//@fixme am2219 remove in NEA6.0
    LSA_UINT32                               CntMrpPorts;
    LSA_BOOL                                 MrpDisabled = LSA_TRUE;
    #endif

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmValidateConsistency(pDDB: 0x%X)",
                           pDDB);


    Status = EDD_STS_OK;
    Valid  = LSA_FALSE;
    i      = 0;

    /* For PDPortData we have to check if at least one Port remains in UP State */

    #ifdef EDDS_PRM_CHECK_ONE_PORT_UP
    if ( ! Local )
    {
        while ((i< pDDB->pGlob->HWParams.Caps.PortCnt) && ( ! Valid ))
        {
            pRecordSetB = &pDDB->pGlob->Prm.PortData.RecordSet_B[i];

            if ( pRecordSetB->Present )
            {
                /* if PortState is present it is "DOWN" (OFF)   */
                /* If not we have a MAUType or Default which is */
                /* both UP                                      */

                if ( ! pRecordSetB->PortStatePresent ) /* != OFF */
                {
                    if ( pRecordSetB->MAUTypePresent ) /* fixed MAU-Type */
                    {
                        /* Valid only TRUE if 100MBit/1Gbit/10Gbit full */
                        if  (( pRecordSetB->LinkStat == EDD_LINK_100MBIT_FULL ) ||
                             ( pRecordSetB->LinkStat == EDD_LINK_1GBIT_FULL   ) ||
                             ( pRecordSetB->LinkStat == EDD_LINK_10GBIT_FULL  ))
                        {
                            Valid = LSA_TRUE;
                        }
                    }
                    else  /* no MAUType -> Autoneg */
                    {
                        Valid = LSA_TRUE;
                    }
                }
            }
            else
            {
                Valid = LSA_TRUE; /* use default -> Autoneg (=Up) */
            }

            i++;
        }

        if ( ! Valid )
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmValidateConsistency: PDPortDataAdjust. All ports DOWN, 10Mbit or halfduplex not allowed!");

            Status = EDD_STS_ERR_PRM_CONSISTENCY;
        }
    }
    #endif

    #ifndef EDDS_DONT_USE_MRP
    if ( Status == EDD_STS_OK)
    {

        /* --------------------------------------------------------------------------*/
        /* PDPortMrpDataAjdust/PDInterfaceMrpDataAdjust Record                       */
        /* --------------------------------------------------------------------------*/

        CntMrpPorts = 0;
        /* only 0 or two PDPortMrpDataAjdust are allowed */
        for (i = 0; i< pDDB->pGlob->HWParams.Caps.PortCnt; i++)
        {
            /* we only need the info the a write was done! */
            if ( pDDB->pGlob->Prm.MRPPort.Present_B[i] )
            {
                CntMrpPorts++;
            }
        }

        /* If no MRP was configured on EDDS startup we dont allow */
        /* MRP-Records to be written!                             */

#ifdef LLIF_CFG_SWITCH_SUPPORT
		MrpDisabled = LSA_FALSE; 
        if ( 0 == pDDB->SWI.MaxMRP_Instances )
        {
            if ( (CntMrpPorts) ||
                 (pDDB->pGlob->Prm.MRPInterface.Present_B))
            {
                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_PrmValidateConsistency: MRP record(s) present, but MRP not configured at EDDS!");
                Status = EDD_STS_ERR_PRM_CONSISTENCY;
            }
            MrpDisabled = LSA_TRUE;
        }
#else
        MrpDisabled = LSA_TRUE;
#endif

        /* three cases (we only check for record presence!):            */
        /* no PDPortMrpDataAdjust record  + PDInterfaceMrpDataAdjust    */
        /* 2  PDPortMrpDataAdjust records + PDInterfaceMrpDataAdjust    */
        /* no records at all.                                           */

        /* Note that MRP records are only allowed if MRP is configured  */
        /* within EDDS!                                                 */

        if ( Status == EDD_STS_OK ) //lint !e774 JB 11/11/2014 only if LLIF_CFG_SWITCH_SUPPORT is not defined
        {
            if(0 == CntMrpPorts)
            {
                if (pDDB->pGlob->Prm.MRPInterface.Present_B)
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
						                "EDDS_PrmValidateConsistency: Only Interface MRP-Record present. MRP disabled.");
                    MrpDisabled = LSA_TRUE;
                }
                else
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
						                "EDDS_PrmValidateConsistency: No MRP-Records present.");
                }

                pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB = LSA_FALSE;
            }
            else if( (CntMrpPorts >= 2) &&
                     (CntMrpPorts <= EDDS_MAX_PORT_CNT))
            {
                if (! pDDB->pGlob->Prm.MRPInterface.Present_B )
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmValidateConsistency: Interface MRP-Record missing!");
                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
                else
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
						                "EDDS_PrmValidateConsistency: Interface MRP-Record and at least 2 port MRP-Records present.");
					pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB = LSA_TRUE;
                }
            }
            else
            {
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_PrmValidateConsistency: Invalid default ring port count (%d)!",CntMrpPorts);
                Status = EDD_STS_ERR_PRM_CONSISTENCY;
            }
        } /* if */
    }

    if ( Status == EDD_STS_OK)
    {
        /* --------------------------------------------------------------------------*/
        /* PDSetDefaultPortStates                                                    */
        /* --------------------------------------------------------------------------*/

        /* Only ports not currently a R-Port with MRP are allowed to be set ! */
        if ( pDDB->pGlob->Prm.DefPortStates.Present_B && (! MrpDisabled)) //lint !e774 !e845 JB 11/11/2014 only if LLIF_CFG_SWITCH_SUPPORT is not defined
        {
            /* check for overlapping R-ports if MRP is enabled */
            for (i = 0; i< pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
                LSA_BOOL   RPort = LSA_FALSE;

                if ( pDDB->pGlob->Prm.MRPPort.AtLeastTwoRecordsPresentB ) /* R-Ports from PRM */
                {
                    if ( pDDB->pGlob->Prm.MRPPort.Present_B[i])
                    {
                        RPort = LSA_TRUE;
                    }
                }
                else /* R-Ports from Setup */
                {
#ifdef LLIF_CFG_SWITCH_SUPPORT
                    if (EDD_MRP_RING_PORT_DEFAULT == pDDB->SWI.MRPRingPort[i] )
                    {
                        RPort = LSA_TRUE;
                    }
#endif
                }

                /* check if this port is set with PDSetDefaultPortStates and is a Rport -> not allowed */
                if ( pDDB->pGlob->Prm.DefPortStates.RecordSet_B[i].PortStatePresent && RPort )
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmValidateConsistency: PDSetDefaultStates overlapping with MRP-R-Port! (PortId: %d)",i+1);
                    Status = EDD_STS_ERR_PRM_CONSISTENCY;
                }
            } /* for */
        } /* if */
    }
    #endif

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmValidateConsistency(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDNCDataCheckWrite                  +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDNCDataCheck record                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid record data and copy to record set B.    +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRecordData)
//JB 11/11/2014 checked where called
//WARNING: be careful while using this function, make sure not to use pRecordData as null ptr
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDNCDataCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData)
{

    LSA_RESULT                              Status;
    EDDS_PRM_NC_DATA_PTR_TYPE               pFrameDrop;
    EDDS_PRM_BUDGET_TYPE                    Budget[EDDS_FRAME_DROP_BUDGET_CNT];
    LSA_UINT32                              Value;
    LSA_UINT32                              i;

	for(i=0; i<EDDS_FRAME_DROP_BUDGET_CNT;++i)  
	{
		Budget[i].Enabled = LSA_FALSE;
        Budget[i].Value   = 0;
	}

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDNCDataCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    pFrameDrop = &pDDB->pGlob->Prm.FrameDrop;
    pFrameDrop->RecordSet_B.Present = LSA_FALSE; /* overwrite actual set if present */

    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /*   PDNCDataCheck (= Frame dropped - no resource)                           */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                            */
    /*    LSA_UINT8           Padding                                            */
    /*    LSA_UINT8           Padding                                            */
    /*    LSA_UINT32          MaintenanceRequiredPowerBudget;                    */
    /*    LSA_UINT32          MaintenanceDemandedPowerBudget;                    */
    /*    LSA_UINT32          ErrorPowerBudget;                                  */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE;

    /* check padding bytes */
    if ( *pRecordData++ != 0 ) Status = EDD_STS_ERR_PRM_BLOCK;
    if ( *pRecordData++ != 0 ) Status = EDD_STS_ERR_PRM_BLOCK;

    /* check for valid budgets                   */
    /* order: 0: required, 1: demanded, 2: error */

	for(i=0; (EDD_STS_OK==Status) && (EDDS_FRAME_DROP_BUDGET_CNT > i);++i) 
    {

        EDDS_GET_U32_INCR(pRecordData,Value);  /* budget */

        /* ON ? */
        if ( (Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_MSK) == EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON )
        {
            Value &= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;  /* Get Value */

            /* check range */
            if (( Value >= EDDS_PRM_PDNC_DROP_BUDGET_MIN ) &&
                ( Value <= EDDS_PRM_PDNC_DROP_BUDGET_MAX ))
            {
                Budget[i].Enabled = LSA_TRUE;
                Budget[i].Value   = Value;

                /* check if value of budget is: error > demanded > required  */
                switch ( i )
                {
                    case EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX: /* required budget */
                        break;

                    case EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX: /* demanded budget */

                        if (( Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) &&
                            ( Value <= Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value )) /* required enabled ? */
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Demanded Budget invalid (%d)", Value);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        break;

                    case EDDS_ERROR_BUDGET_IDX: /* Error budget */

                        if (( Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) &&
                            ( Value <= Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value )) /* required enabled ? */
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Error Budget invalid (%d)", Value);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        if (( Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled ) &&
                            (  Value <= Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value )) /* demanded enabled ? */
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDNCDataCheckWrite: Error Budget invalid (%d)", Value);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }

                        break;
                    default:
                        break;

                } /* switch */
            }
            else
            {
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_PrmPDNCDataCheckWrite: Budget Value invalid (%d)", Value);

                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
    }

    /* if OK copy to set B */

    if ( Status == EDD_STS_OK )
    {
        for (i=0; i<EDDS_FRAME_DROP_BUDGET_CNT;++i)
        {
            pFrameDrop->RecordSet_B.Budget[i] = Budget[i]; 
        }

        /* record set is present.. */
        pFrameDrop->RecordSet_B.Present = LSA_TRUE;
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDNCDataCheckWrite(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRecordData)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDPortDataAdjustWrite               +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             LSA_BOOL                    Local           +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+                             LSA_UINT32                  BlockLength     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  Local       : LSA_TRUE : Local  parametration                          +*/
/*+                LSA_FALSE: Remote parametration                          +*/
/*+  PortID      : PortID. 1.. EDDS_MAX_PORT_CNT                            +*/
/*+  pRecordData : Pointer to PDPortData    record                          +*/
/*+  BlockLength : BlockLength  (total length of block from Header)         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid record data and copy to record set B.    +*/
/*+                                                                         +*/
/*+               If Local is set all Speed/Mode settings allowed           +*/
/*+               If not, only 100/1000/1000 FullDuplex allowed.            +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+               On entry all settings marked as invalid!                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRecordData)
//JB 11/11/2014 checked where called
//WARNING: be careful while using this function, make sure not to use pRecordData 
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDPortDataAdjustWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_BOOL                    Local,
    LSA_UINT16                  PortID,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  BlockLength)
{

    LSA_RESULT                               Status;
    EDDS_PRM_PORT_DATA_PTR_TYPE              pPortData;
    LSA_UINT16                               SubBlockType;
    LSA_UINT16                               SubBlockLength;
    EDD_UPPER_MEM_U8_PTR_TYPE                pRecordDataHelp;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE pRecordSetB;
    //LSA_UINT32                               i;
    LSA_UINT8                                VersionHigh;
    LSA_UINT8                                VersionLow;
    LSA_UINT8                                Pad1;
    LSA_UINT8                                Pad2;
    LSA_UINT8                                Pad3;
    LSA_UINT8                                Pad4;
    LSA_UINT8                                LinkState;
    LSA_UINT32                               Position;

    EDDS_PRM_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDPortDataAdjustWrite(pDDB: 0x%X, PortID: %d, pRecordData: 0x%X, Length: 0x%X)",
                           pDDB,
                           PortID,
                           pRecordData,
                           BlockLength);

    Status        = EDD_STS_OK;
    //i             = 0;
    LinkState     = EDD_LINK_100MBIT_FULL;  /* default */
    Position      = 1;

    pPortData   = &pDDB->pGlob->Prm.PortData;
    pRecordSetB = &pPortData->RecordSet_B[PortID-1];

    /* overwrite actual set if present */
    pRecordSetB->Present = LSA_FALSE;
    pRecordSetB->MAUTypePresent           = LSA_FALSE;
    pRecordSetB->MulticastBoundaryPresent = LSA_FALSE;
    pRecordSetB->PortStatePresent         = LSA_FALSE;
    pRecordSetB->PreambleLengthPresent    = LSA_FALSE;

    /*---------------------------------------------------------------------------*/
    /*   PDPortDataAdjust                                                        */
    /*---------------------------------------------------------------------------*/
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          SlotNumber                                    2    */
    /*    LSA_UINT16          SubslotNumber                                 2    */
    /*    [AdjustDomainBoundary]                             skipped   by EDDS   */
    /*    [AdjustMulticastBoundary]                          evaluated by EDDS   */
    /*    [AdjustMauType ^ AdjustPortState]                  evaluated by EDDS   */
    /*                                                                           */
    /*    AdjustMulticastBoundary:                                               */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          MulticastBoundary                             4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*    AdjustMauType                                                          */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          MAUType                                       2    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*                                                                    -----  */
    /*                                                                     12    */
    /*                                                                           */
    /*    AdjustPortState                                                        */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT16          PortState                                     2    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*                                                                    -----  */
    /*                                                                     12    */
    /*                                                                           */
    /*    AdjustPeerToPeerBoundary                                               */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          PeerToPeerBoundary                            4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*                                                                           */
    /*    AdjustDCPBoundary                                                      */
    /*                                                                           */
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT32          DCPBoundary                                   4    */
    /*    LSA_UINT16          AdjustProperties                              2    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*                                                                    -----  */
    /*                                                                     16    */
    /*---------------------------------------------------------------------------*/

    /* Pointer to first Record behind SubslotNuber */
    pRecordData  += EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE;
    BlockLength  -= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE - 4;

    /* Note: maybe we have no sublocks! this is valid ! */

    Position = 1;  //lint !e838 JB 11/11/2014 see while loop below:

    while (( Status == EDD_STS_OK ) && ( BlockLength ))
    {

        if ( BlockLength >= EDDS_PRM_RECORD_HEADER_SIZE )
        {
            pRecordDataHelp = pRecordData;

            EDDS_GET_U16_INCR(pRecordDataHelp,SubBlockType);
            EDDS_GET_U16_INCR(pRecordDataHelp,SubBlockLength);

            VersionHigh = *pRecordDataHelp++;
            VersionLow  = *pRecordDataHelp++;

            SubBlockLength +=4;  /* add length of BlockType and Length */

            if ( ( SubBlockLength <= BlockLength                 ) &&
                 ( SubBlockLength >= EDDS_PRM_RECORD_HEADER_SIZE ) &&
                 ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH    ) &&
                 ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW     ))
            {

                /* pRecordDataHelp points behing blockheader */

                switch (SubBlockType)
                {
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE)
                        {
                            if ( Position <= 2 )
                            {
                                Position = 3;
                                pRecordSetB->MulticastBoundaryPresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U32_INCR(pRecordDataHelp,pRecordSetB->MulticastBoundary);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MulticastBoundaryAP);
                                Pad3 = *pRecordDataHelp++;
                                Pad4 = *pRecordDataHelp++;

                                if (( pRecordSetB->MulticastBoundaryAP != EDDS_PRM_PDPORT_DATA_MC_ADJUST_PROPERTIES ) ||
                                    ( Pad1                             != 0                                        ) ||
                                    ( Pad2                             != 0                                        ) ||
                                    ( Pad3                             != 0                                        ) ||
                                    ( Pad4                             != 0                                        ))
                                {
                                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: MulticastBoundary invalid (%d,%d)", pRecordSetB->MulticastBoundary,pRecordSetB->MulticastBoundaryAP);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust MC Boundary block length (%d)",
                                                SubBlockLength-4);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;

                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE)
                        {
                            if ( Position <= 3 )
                            {
                                Position = 4;
                                pRecordSetB->MAUTypePresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MAUType);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->MAUTypeAP);

                                if (( pRecordSetB->MAUTypeAP == EDDS_PRM_PDPORT_DATA_MAUTYPE_ADJUST_PROPERTIES ) &&
                                    ( Pad1                   == 0                                             ) &&
                                    ( Pad2                   == 0                                             ))
                                {

                                    /* check if MAUType supported by LLIF and translate MAUType into speed/mode */

                                    EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                                        "==> EDDS_LL_CHECK_MAUTYPE(pDDB: 0x%X, PortID: %d, MAUType: %d, Power: 0x%X)",
                                                        pDDB,
                                                        PortID,
                                                        pRecordSetB->MAUType,
                                                        EDDS_PHY_POWER_ON);

                                    Status = EDDS_LL_CHECK_MAUTYPE(pDDB,PortID,pRecordSetB->MAUType,EDDS_PHY_POWER_ON,&LinkState);

                                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_CHECK_MAUTYPE(). Status: 0x%X, LinkState: %d",
                                                        Status,
                                                        LinkState);

                                    if ( Status != EDD_STS_OK )
                                    {
                                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDPortDataAdjustWrite: MAUType not supported by LLIF (MAUType: %d)!",pRecordSetB->MAUType);
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                    else
                                    {
                                        /* save LinkState for later use */
                                        pRecordSetB->LinkStat = LinkState;

                                        /* check if this is a unvalid LinkState */
                                        /* must be >= 100Mbit and FullDuplex    */
                                        if (( LinkState != EDD_LINK_100MBIT_FULL ) &&
                                            ( LinkState != EDD_LINK_1GBIT_FULL   ) &&
                                            ( LinkState != EDD_LINK_10GBIT_FULL  ))
                                        {
                                            if ( ! Local ) /* if local we allow all.. */
                                            {
                                                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                                    "EDDS_PrmPDPortDataAdjustWrite: MAUType not fullduplex and/or <100MBit (Local: %d, MT: 0x%X)",Local,pRecordSetB->MAUType);
                                                Status = EDD_STS_ERR_PRM_BLOCK;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: MAUTypeAP or padding invalid (%d)",pRecordSetB->MAUTypeAP);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust MAUType block length (%d)",
                                                SubBlockLength-4);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;

                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE)
                        {
                            if ( Position <= 3 )
                            {
                                Position = 4;
                                pRecordSetB->PortStatePresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PortState);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PortStateAP);

                                if (( pRecordSetB->PortState   != EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN             ) ||
                                    ( pRecordSetB->PortStateAP != EDDS_PRM_PDPORT_DATA_PORTSTATE_ADJUST_PROPERTIES ) ||
                                    ( Pad1                     != 0                                               ) ||
                                    ( Pad2                     != 0                                               ))
                                {
                                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: PortState invalid (%d,%d)", pRecordSetB->PortState,pRecordSetB->PortStateAP);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                                else
                                {

                                    /* check if DOWN supported by LLIF */

                                    EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                                        "==> EDDS_LL_CHECK_MAUTYPE(pDDB: 0x%X, PortID: %d, MAUType: %d, Power: 0x%X)",
                                                        pDDB,
                                                        PortID,
                                                        EDD_MAUTYPE_UNKNOWN,
                                                        EDDS_PHY_POWER_OFF);

                                    Status = EDDS_LL_CHECK_MAUTYPE(pDDB,PortID,EDD_MAUTYPE_UNKNOWN,EDDS_PHY_POWER_OFF,&LinkState);

                                    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_CHECK_MAUTYPE(). Status: 0x%X",Status);

                                    if ( Status != EDD_STS_OK )
                                    {
                                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDPortDataAdjustWrite: POWER OFF not supported!");
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust Portstate block length (%d)",
                                                SubBlockLength-4);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        /* Must be first block if present */
                        if ( Position == 1 )
                        {
                            Position = 2;
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( Position <= 4 )
                        {
                            Position = 5;
                            /* tbd */
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( SubBlockLength == EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE)
                        {
                            if ( Position <= 5 )
                            {
                                Position = 6;
                                pRecordSetB->DCPBoundaryPresent = LSA_TRUE;

                                Pad1 = *pRecordDataHelp++;
                                Pad2 = *pRecordDataHelp++;
                                EDDS_GET_U32_INCR(pRecordDataHelp,pRecordSetB->DCPBoundary);
                                EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->DCPBoundaryAP);
                                Pad3 = *pRecordDataHelp++;
                                Pad4 = *pRecordDataHelp++;

                                if (( pRecordSetB->DCPBoundaryAP       != EDDS_PRM_PDPORT_DATA_DCP_ADJUST_PROPERTIES ) ||
                                    ( Pad1                             != 0                                         ) ||
                                    ( Pad2                             != 0                                         ) ||
                                    ( Pad3                             != 0                                         ) ||
                                    ( Pad4                             != 0                                         ))
                                {
                                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: DCPBounary invalid (%d,%d)", pRecordSetB->DCPBoundary,pRecordSetB->DCPBoundaryAP);

                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_PrmPDPortDataAdjustWrite: Duplicate block or invalid block combination");

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: Invalid adjust DCP Boundary block length (%d)",
                                                SubBlockLength-4);

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    case EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_TYPE:
                        /*-----------------------------------------------------------*/
                        if ( Position <= 6 )
                        {
                            Position = 7;
                            pRecordSetB->PreambleLengthPresent = LSA_TRUE;

                            Pad1 = *pRecordDataHelp++;
                            Pad2 = *pRecordDataHelp++;
                            EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PreambleLength);
                            EDDS_GET_U16_INCR(pRecordDataHelp,pRecordSetB->PreambleLengthAP);

                            if (( pRecordSetB->PreambleLength   != EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_LONG )       ||
                                ( pRecordSetB->PreambleLengthAP != EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_PROPERTIES ) ||
                                ( Pad1                          != 0 )   ||
                                ( Pad2                          != 0 ))
                            {
                                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_PrmPDPortDataAdjustWrite: PreambleLength invalid (%d,%d)", pRecordSetB->PreambleLength,pRecordSetB->PreambleLengthAP);

                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_PrmPDPortDataAdjustWrite: duplicate block or invalid block combination");

                            Status = EDD_STS_ERR_PRM_BLOCK;
                        }
                        break;
                        /*-----------------------------------------------------------*/
                    default:
                        /*-----------------------------------------------------------*/
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                            "EDDS_PrmPDPortDataAdjustWrite: Unknown Subblock (0x%X)",SubBlockType);

                        Status = EDD_STS_ERR_PRM_BLOCK;
                        break;
                }

                pRecordData += SubBlockLength;
                BlockLength -= SubBlockLength;
            }
            else
            {
                if ( ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH    ) &&
                     ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW     ))
                {
                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmPDPortDataAdjustWrite: Invalid Subblock length (%d,%d)",SubBlockType,SubBlockLength);

                    Status = EDD_STS_ERR_PRM_DATA;
                }
                else
                {
                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_PrmPDPortDataAdjustWrite: Invalid Subblock version (%d,%d)",VersionHigh,VersionLow);

                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
        }
        else
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_PrmPDPortDataAdjustWrite: Invalid record length");

            Status = EDD_STS_ERR_PRM_DATA;
        }
    }

    /* if OK set present else set all to invalid */
    /* tbd: reaction if entrys missing? */

    if ( Status == EDD_STS_OK )
    {
        pRecordSetB->Present = LSA_TRUE;  /* maybe no subblock present ! */
    }
    else
    {
        pRecordSetB->MAUTypePresent           = LSA_FALSE;
        pRecordSetB->MulticastBoundaryPresent = LSA_FALSE;
        pRecordSetB->PortStatePresent         = LSA_FALSE;
        pRecordSetB->PreambleLengthPresent    = LSA_FALSE;
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDPortDataAdjustWrite(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRecordData)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDSCFDataCheckWrite                 +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDNCDataCheck record                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid Sendclockfactor.                         +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRecordData,pDDB)
//JB 11/11/2014 pRecordData can not be a null pointer - checked where called; pDDB checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDSCFDataCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData)
{

    LSA_RESULT                              Status;
    #ifdef EDDS_CFG_SRT_INCLUDE
    LSA_UINT16                              SendClockFactor;
    #endif

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDSCFDataCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    Status = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /*   PDSCFDataCheck (= Sendclock factor check)                               */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                            */
    /*    LSA_UINT16          SendClockFactor                                    */
    /*    LSA_UINT8           Data[x]                                            */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE;

    #ifdef EDDS_CFG_SRT_INCLUDE
    if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
    {
		/* check for valid SendclockFactor           */
		EDDS_GET_U16_INCR(pRecordData,SendClockFactor);

		#ifndef EDDS_CFG_PRM_DONT_CHECK_SENDCLOCK
		if ( pDDB->pSRT->CSRT.Cfg.CycleBaseFactor != SendClockFactor )
		{
			EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_PrmPDSCFDataCheckWrite: Invalid SendClockFactor(%d,%d)!",
								pDDB->pSRT->CSRT.Cfg.CycleBaseFactor,
								SendClockFactor);

			Status = EDD_STS_ERR_PRM_BLOCK;
		}
		#endif
    }
    #endif

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDSCFDataCheckWrite(), Status: 0x%X",
                           Status);

    return(Status); //lint !e438 JB 11/11/2014 last value of pRecordDate actually used as SendClockFactor after EDDS_GET_U16_INCR
}
//lint +esym(613,pRecordData,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDPortStatisticRecordRead           +*/
/*+                                                                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_IDB_PTR_TYPE     pIDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                                                                         +*/
/*+  Result                :    EDD_RSP                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pIDB       : Pointer to InterfaceDescriptionBlock                      +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function reads the Port/Inteface-Statistic           +*/
/*+               and stores it to PrmRead RQB.                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
EDDS_STATIC EDD_RSP EDDS_LOCAL_FCT_ATTR EDDS_PrmPDPortStatisticRecordRead(
	EDD_UPPER_RQB_PTR_TYPE      pRQB,
	EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    EDD_RSP                         Response;
    EDD_RQB_GET_STATISTICS_TYPE		StatisticData;
    EDD_UPPER_MEM_U8_PTR_TYPE       pRecordData;
    EDD_UPPER_PRM_READ_PTR_TYPE     pPrmReadParam;
    LSA_UINT32						TraceIdx = pHDB->pDDB->pGlob->TraceIdx;

    LSA_UNUSED_ARG(TraceIdx);

    Response                            = EDD_STS_OK;
    pPrmReadParam                       = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam;  //lint !e613 JB 11/11/2014 should not be necessary (cast as reason?)
    pPrmReadParam->record_data_length   = 0;
    pRecordData                         = pPrmReadParam->record_data;

    EDDS_PRM_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    		"[H:%2X] IN: EDDS_PrmPDPortStatisticRecordRead(pRQB=0x%X), PortID=%d",
    		pHDB->Handle, pRQB, pPrmReadParam->edd_port_id);

    /* ------------------------------------ */
    /* check PortID                         */
    /* ------------------------------------ */
    // PortID=0     : interface
    // PortID=1..n  : Port
    if (pPrmReadParam->edd_port_id > pHDB->pDDB->pGlob->HWParams.Caps.PortCnt)
    {
        Response = EDD_STS_ERR_PRM_PORTID;
        EDDS_PRM_TRACE_03(TraceIdx,LSA_TRACE_LEVEL_ERROR, "[H:%2X] EDDS_PrmPDPortStatisticRecordRead(): ERROR -> PortID(%d) is not between 0..%d!",
            pHDB->Handle, pPrmReadParam->edd_port_id, pHDB->pDDB->pGlob->HWParams.Caps.PortCnt);
    }
    else
    {
        /* ------------------------------------ */
        /* get current StatisticData            */
        /* ------------------------------------ */
        // Port/Interface Statistics
    	StatisticData.RawUsed = LSA_FALSE;
    	StatisticData.Reset   = LSA_FALSE;
    	StatisticData.PortID = pPrmReadParam->edd_port_id;
       	Response = EDDS_RequestGetStatistics(&StatisticData,pHDB);

        /* ------------------------------------ */
        /* write StatisticData to RecordData    */
        /* ------------------------------------ */
        if (Response == EDD_STS_OK)
        {
            // BlockType
            EDDS_SET_U16_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_BLOCKTYPE);

            // BlockLength
            pPrmReadParam->record_data_length = EDDS_PRM_PDPORT_STATISTIC_SIZE;
            EDDS_SET_U16_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_SIZE-4);	// BlockLength + BlockType (4byte) not counted in BlockLength

            // BlockVersionHigh
            EDDS_SET_U8_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONHIGH);
            // BlockVersionLow
            EDDS_SET_U8_INCR(pRecordData, EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONLOW);

            // Padding
            EDDS_SET_U8_INCR(pRecordData, 0x00);
            // Padding
            EDDS_SET_U8_INCR(pRecordData, 0x00);

            /* ------------------------------------------------------------------------ */
            /* >>> StatisticData                                                        */
            /* ------------------------------------------------------------------------ */
            // ifInOctets
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.InOctets);
            // ifOutOctets
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.OutOctets);
            // ifInDiscards
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.InDiscards);
            // ifOutDiscards
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.OutDiscards);
            // ifInErrors
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.InErrors);
            // ifOutErrors
            EDDS_SET_U32_INCR(pRecordData, StatisticData.sMIB.OutErrors);

            // Padding: only for alignment (the block must be UINT32 aligned)
#if 0
            EDDS_SET_U8_INCR(pRecordData, 0x00);
#endif
            /* ------------------------------------------------------------------------ */
            /* <<< StatisticData                                                        */
            /* ------------------------------------------------------------------------ */
        }
    }

    EDDS_PRM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
    		"[H:--] OUT:EDDS_PrmPDPortStatisticRecordRead(): Response=0x%X", Response);

    return (Response);
}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmPDSetDefPortStatesCheckWrite        +*/
/*+  Input/Output          :    EDDS_UPPER_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData     +*/
/*+                             LSA_UINT32                  BlockLength     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB        : Pointer to DeviceDescriptionBlock                        +*/
/*+  pRecordData : Pointer to PDSetDefaultPortStates record                 +*/
/*+  BlockLength : BlockLength from header                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for valid SetDefaultPortStates                     +*/
/*+                                                                         +*/
/*+               Note: Blockheader not checked! has bo be valid!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRecordData,pDDB)
//JB 11/11/2014 pRecordData can not be a null pointer - checked where called; pDDB checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmPDSetDefPortStatesCheckWrite(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  BlockLength)
{

    LSA_RESULT                              Status;
    EDDS_PRM_SET_DEF_PORTSTATES_PTR_TYPE    pDefPortStates;
    LSA_UINT8                               PortId;    /* User PortId 1..x */
    LSA_UINT8                               PortState;
    //LSA_UINT8                               Pad1,Pad2;
    LSA_UINT32                              i;

    EDDS_PRM_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmPDSetDefPortStatesCheckWrite(pDDB: 0x%X, pRecordData: 0x%X)",
                           pDDB,
                           pRecordData);

    Status         = EDD_STS_OK;
    pDefPortStates = &pDDB->pGlob->Prm.DefPortStates;

    pDefPortStates->Present_B = LSA_FALSE; /* not present */

    /* Set all entrys to not present*/
    for (i=0; i<EDDS_MAX_PORT_CNT; i++)
    {
        pDefPortStates->RecordSet_B[i].PortStatePresent = LSA_FALSE;
    }

    /*---------------------------------------------------------------------------*/
    /*   PDSetDefaultPortStates                                                  */
    /*---------------------------------------------------------------------------*/
    /*    BlockHeader                                                       6    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    LSA_UINT8           Padding                                       1    */
    /*    (                                                                      */
    /*      LSA_UINT8           PortId                                      1    */
    /*      LSA_UINT8           PortState                                   1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*      LSA_UINT8           Padding                                     1    */
    /*    )*                                                                     */
    /*                                                                           */
    /* PortID: 1..255 (a valid PortId)                                           */
    /* each PortId must be present Port and only appears once within record!     */
    /* Multiple PortId settings may be present within record.                    */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/

    /* Pointer behind BlockHeader and padding */
    pRecordData += EDDS_PRM_RECORD_HEADER_SIZE+2;

    BlockLength -= (EDDS_PRM_RECORD_HEADER_SIZE - 4) +2 ; /* remaining length behind paddings */

    while (BlockLength >= 4 ) /* PortId, PortState, Pad1, Pad2 = 4 Bytes */ 
    {
        PortId    = *pRecordData++;
        PortState = *pRecordData++;
        //Pad1 = *pRecordData++;
		//Pad2 = *pRecordData++;
		pRecordData +=2;
        BlockLength -= 4;

        /* check for valid PortId */
        if (( PortId > 0 ) && ( PortId <= pDDB->pGlob->HWParams.Caps.PortCnt))
        {
            /* check for duplicate entry */
            if ( ! pDefPortStates->RecordSet_B[PortId-1].PortStatePresent )
            {
                /* check for valid PortState */
                if (( PortState == EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE ) ||
                    ( PortState == EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING))
                {
                    /* all ok. store values.. */
                    pDefPortStates->RecordSet_B[PortId-1].PortStatePresent = LSA_TRUE;
                    pDefPortStates->RecordSet_B[PortId-1].PortState        = PortState;

                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_PrmPDSetDefPortStatesCheckWrite: PortID: %d, PortState: %d",PortId,PortState);
                }
                else
                {
                    /* Invalid PortState! */
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmPDSetDefPortStatesCheckWrite, Invalid PortState: 0x%X",PortState);
                    Status = EDD_STS_ERR_PRM_BLOCK;
                }
            }
            else
            {
                /* PortState for PortID already set within record! */
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Multiple PortId: 0x%X",PortId);
                Status = EDD_STS_ERR_PRM_BLOCK;
            }
        }
        else
        {
            /* invalid PortId */
            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Invalid PortId: 0x%X",PortId);
            Status = EDD_STS_ERR_PRM_BLOCK;
        }
    } /* while */

    /* if ok check if Blocklength matches (no bytes left) */
    if ((Status == EDD_STS_OK ) && ( BlockLength != 0 ))
    {
        /* invalid BlockLength (dont matches content) */
        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR, "EDDS_PrmCheckPDSetDefaultPortStates, Invalid remaining BlockLength (not 0): 0x%X",BlockLength);
        Status = EDD_STS_ERR_PRM_BLOCK;
    }

    /*---------------------------------------------------------------------------*/
    /* on error clear content. if ok set WRITE_DONE.                             */
    /*---------------------------------------------------------------------------*/
    if ( Status != EDD_STS_OK )
    {
        for (i=0; i<EDDS_MAX_PORT_CNT; i++)
        {
            pDefPortStates->RecordSet_B[i].PortStatePresent = LSA_FALSE;
        }
    }
    else
    {
        pDefPortStates->Present_B = LSA_TRUE; /* present */
    }

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmPDSetDefPortStatesCheckWrite(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRecordData,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmRead                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_READ                +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_READ_PTR_TYPE                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_PORTID                                    +*/
/*+               EDD_STS_ERR_PRM_INDEX                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_READ request        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmRead(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                               	Status;
    EDD_UPPER_PRM_READ_PTR_TYPE              	pPrmRead;
    EDDS_PRM_PTR_TYPE                        	pPrm;
    EDD_UPPER_MEM_U8_PTR_TYPE                	pRecordData;
    EDDS_NC_DATA_CHECK_RECORD_SET_PTR_TYPE   	pNCDataRecordSet;
    EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE 	pPORTDataRecordSet;
    LSA_UINT32                               	Budget;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmRead(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK;
    pPrmRead = (EDD_UPPER_PRM_READ_PTR_TYPE) pRQB->pParam; //lint !e613 JB 11/11/2014 should not be necessary (cast as reason?)

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPrmRead, LSA_NULL) ) ||
         ((pPrmRead->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt)) ||
         ( LSA_HOST_PTR_ARE_EQUAL( pPrmRead->record_data, LSA_NULL) )
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pPrm = &pDDB->pGlob->Prm;

        switch (pPrmRead->record_index)
        {

			/* ---------------------------------------- */
			/* PDPortStatistic                          */
			/* ---------------------------------------- */
			case EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX:
			{
				EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE, "[H:%2X]     PrmRecord: PDPortStatistic", pHDB->Handle);
                if ( pPrmRead->record_data_length >= EDDS_PRM_PDPORT_STATISTIC_SIZE )
                {
                	Status = EDDS_PrmPDPortStatisticRecordRead (pRQB, pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RequestPrmRead: Invalid record_data_length (%d)", pPrmRead->record_data_length);

                    Status = EDD_STS_ERR_PRM_DATA;
                }
			}
			break;

                /* --------------------------------------------------------------*/
                /* PDNCDataCheck                                                 */
                /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK:
                if (pPrmRead->edd_port_id == 0)
                {
                    pNCDataRecordSet = &pPrm->FrameDrop.RecordSet_A;

                    if (pNCDataRecordSet->Present )
                    {
                        if ( pPrmRead->record_data_length >= EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE )
                        {
                            pPrmRead->record_data_length = EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE;

                            pRecordData = pPrmRead->record_data;

                            /*--------------------------------------------------------*/
                            /*   BlockHeader                                          */
                            /*--------------------------------------------------------*/
                            /*    LSA_UINT16          BlockType;                      */
                            /*    LSA_UINT16          BlockLength;                    */
                            /*    LSA_UINT8           BlockVersionHigh;               */
                            /*    LSA_UINT8           BlockVersionLow;                */
                            /*--------------------------------------------------------*/

                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE);
                            EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE - 4);
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
                            *pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

                            /*--------------------------------------------------------*/
                            /*   PDNCDataCheck (= Frame dropped - no resource)        */
                            /*--------------------------------------------------------*/
                            /*    LSA_UINT8           Padding                         */
                            /*    LSA_UINT8           Padding                         */
                            /*    LSA_UINT32          MaintenanceRequiredPowerBudget; */
                            /*    LSA_UINT32          MaintenanceDemandedPowerBudget; */
                            /*    LSA_UINT32          ErrorPowerBudget;               */
                            /* -------------------------------------------------------*/
                            *pRecordData++ = 0; /* padding */
                            *pRecordData++ = 0; /* padding */

                            Budget = pNCDataRecordSet->Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            Budget = pNCDataRecordSet->Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            Budget = pNCDataRecordSet->Budget[EDDS_ERROR_BUDGET_IDX].Value & EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK;
                            if ( pNCDataRecordSet->Budget[EDDS_ERROR_BUDGET_IDX].Enabled ) Budget |= EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON;
                            EDDS_SET_U32_INCR(pRecordData,Budget);

                            /* -------------------------------------------------------*/
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmRead: Invalid record_data_length (%d)", pPrmRead->record_data_length);

                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDNC: Invalid port_id (<>0)!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
                break;
                /* --------------------------------------------------------------*/
                /* PDPortDataAdjust                                              */
                /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST:
                if ( pPrmRead->edd_port_id != 0 ) /* 0 not allowed ! */
                {
                    pPORTDataRecordSet = &pPrm->PortData.RecordSet_A[pPrmRead->edd_port_id-1];

                    /* record present? */
                    if (pPORTDataRecordSet->Present )
                    {
						#if 1
						/* copy record */
						if ( pPrmRead->record_data_length >= pPORTDataRecordSet->RecordLength )
						{
							EDDS_CopyLocal2Upper(pPORTDataRecordSet->Record,
												 pPrmRead->record_data,
												 pPORTDataRecordSet->RecordLength);

							pPrmRead->record_data_length = pPORTDataRecordSet->RecordLength;

						}
						else
						{
							EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
												"EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d", pPrmRead->record_data_length,pPORTDataRecordSet->RecordLength);

							Status = EDD_STS_ERR_PRM_DATA;
						}
						#else
						/* calculate total length needed */

						Len = EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE;
						/* Note: MAUType and PortState not both present ! */
						if  ( pPORTDataRecordSet->MulticastBoundaryPresent)
							Len += EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE;
						if  ( pPORTDataRecordSet->MAUTypePresent)
							Len += EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE;
						if ( pPORTDataRecordSet->PortStatePresent)
							Len += EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE;

						if ( pPrmRead->record_data_length >= Len )
						{

							pPrmRead->record_data_length = Len;

							pRecordData = pPrmRead->record_data;

							/*--------------------------------------------------------*/
							/*   BlockHeader                                          */
							/*--------------------------------------------------------*/
							/*    LSA_UINT16          BlockType;                      */
							/*    LSA_UINT16          BlockLength;                    */
							/*    LSA_UINT8           BlockVersionHigh;               */
							/*    LSA_UINT8           BlockVersionLow;                */
							/*--------------------------------------------------------*/

							EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE);
							EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(Len - 4)));
							*pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
							*pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

							/*--------------------------------------------------------*/
							/*    LSA_UINT8           Padding                         */
							/*    LSA_UINT8           Padding                         */
							/*    LSA_UINT16          SlotNumber                      */
							/*    LSA_UINT16          SubslotNumber                   */
							/*--------------------------------------------------------*/

							*pRecordData++ = 0; /* padding */
							*pRecordData++ = 0; /* padding */
							EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->SlotNumber);
							EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->SubSlotNumber);

							/*----------------------------------------------------*/
							/*    AdjustMulticastBoundary:                        */
							/*----------------------------------------------------*/
							/*    BlockHeader                                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT32          MulticastBoundary           */
							/*    LSA_UINT16          AdjustProperties            */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT8           Padding                     */
							/*----------------------------------------------------*/

							if (pPORTDataRecordSet->MulticastBoundaryPresent)
							{
								EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE);
								EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE - 4)));
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

								*pRecordData++ = 0; /* padding */
								*pRecordData++ = 0; /* padding */
								EDDS_SET_U32_INCR(pRecordData,pPORTDataRecordSet->MulticastBoundary);
								EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MulticastBoundaryAP);
								*pRecordData++ = 0; /* padding */
								*pRecordData++ = 0; /* padding */
							}

							/*----------------------------------------------------*/
							/*    AdjustMauType                                   */
							/*----------------------------------------------------*/
							/*    BlockHeader                                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT16          MAUType                     */
							/*    LSA_UINT16          AdjustProperties            */
							/*----------------------------------------------------*/

							if (pPORTDataRecordSet->MAUTypePresent)
							{
								EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE);
								EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE - 4)));
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

								*pRecordData++ = 0; /* padding */
								*pRecordData++ = 0; /* padding */
								EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MAUType);
								EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->MAUTypeAP);
							}

							/*----------------------------------------------------*/
							/*    AdjustPortState                                 */
							/*----------------------------------------------------*/
							/*    BlockHeader                                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT8           Padding                     */
							/*    LSA_UINT16          PortState                   */
							/*    LSA_UINT16          AdjustProperties            */
							/*----------------------------------------------------*/

							if (pPORTDataRecordSet->PortStatePresent)
							{
								EDDS_SET_U16_INCR(pRecordData,EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE);
								EDDS_SET_U16_INCR(pRecordData,((LSA_UINT16)(EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE - 4)));
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_HIGH; /* version high */
								*pRecordData++ = EDDS_PRM_BLOCK_VERSION_LOW; /* version low  */

								*pRecordData++ = 0; /* padding */
								*pRecordData++ = 0; /* padding */
								EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->PortState);
								EDDS_SET_U16_INCR(pRecordData,pPORTDataRecordSet->PortStateAP);
							}
						}
						else
						{
							EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
												"EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d", pPrmRead->record_data_length,Len);

							Status = EDD_STS_ERR_PRM_DATA;
						}
						#endif

                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDPORT: Invalid port_id (=0)!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
                break;

                /* --------------------------------------------------------------*/
                /* PDSCFDataCheck                                                */
                /* --------------------------------------------------------------*/
            case EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK:
                if ( 0 == pPrmRead->edd_port_id) /* only 0 allowed ! */
                {
                    /* record present? */
                    if (pPrm->PDSCFDataCheck.Present )
                    {
						/* copy record */
						if ( pPrmRead->record_data_length >= pPrm->PDSCFDataCheck.RecordLength )
						{
							EDDS_CopyLocal2Upper(pPrm->PDSCFDataCheck.Record,
												 pPrmRead->record_data,
												 pPrm->PDSCFDataCheck.RecordLength);

							pPrmRead->record_data_length = pPrm->PDSCFDataCheck.RecordLength;

						}
						else
						{
							EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
												"EDDS_RequestPrmRead: Invalid record_data_length (%d). Needed: %d", pPrmRead->record_data_length,pPrm->PDSCFDataCheck.RecordLength);

							Status = EDD_STS_ERR_PRM_DATA;
						}
                    }
                    else
                    {
                        pPrmRead->record_data_length = 0; /* no record present */
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmRead: PDSCFDataCheck: only port 0 allowed!");
                    Status = EDD_STS_ERR_PRM_PORTID;
                }
            	break;
			/* ---------------------------------------- */
			/* User specific records                    */
			/* These records are not used by EDDS;      */
            /* return OK!                               */
			/* ---------------------------------------- */
			case EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX:
			case EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX:
			case EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX:
			{
				pPrmRead->record_data_length = 0;
				EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_RequestPrmRead(): PrmRecord(0x%X) is not used by EDDS",
					pPrmRead->record_index);
			}
                break;



                /* --------------------------------------------------------------*/
                /* unknown/unsupported                                           */
                /* --------------------------------------------------------------*/
            default:
                #ifndef EDDS_CFG_PRM_READ_UNKNOWN_INDEX_ALLOWED
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RequestPrmRead: record_index (%d)", pPrmRead->record_index);

                Status = EDD_STS_ERR_PRM_INDEX;
                #else
                /* no error. we return an empty record instead */
                pPrmRead->record_data_length = 0; /* no record present */
                #endif
                break;
        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmRead(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmPrepare                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_PREPARE             +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam not used                                                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_PREPARE request     +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmPrepare(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;
    EDDS_PrmSetBToNotPresent(pDDB);

    Status = EDD_STS_OK;

    LSA_UNUSED_ARG(pRQB);
    LSA_UNUSED_ARG(pHDB);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmEnd                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_END                 +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_PRM_END_TYPE                                  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_CONSISTENCY                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_END request         +*/
/*+                                                                         +*/
/*+               END is only allowed after PREPARE or WRITE                +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmEnd(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_PRM_END_PTR_TYPE              pPrmEnd;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmEnd(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pPrmEnd = (EDD_UPPER_PRM_END_PTR_TYPE) pRQB->pParam; //lint !e613 JB 11/11/2014 should not be necessary (cast as reason?)

    if (  LSA_HOST_PTR_ARE_EQUAL( pPrmEnd, LSA_NULL) )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmEnd: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if (( pDDB->pGlob->Prm.PrmState == EDDS_PRM_STATE_PREPARED ) ||
            ( pDDB->pGlob->Prm.PrmState == EDDS_PRM_STATE_WRITE   ))
        {
            Status = EDDS_PrmValidateConsistency(pDDB,pPrmEnd->Local);
            pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_END; /* wait for commit */
        }
        else
        {
            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RequestPrmEnd: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);

            Status = EDD_STS_ERR_SEQUENCE;
        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmEnd(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint -esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmCommit                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_COMMIT              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam not used                                                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_COMMIT request      +*/
/*+               We copy record set B to record set A                      +*/
/*+               For all present records we have to indicate the current   +*/
/*+               ErrorStatus. If no error present we have to indicate an   +*/
/*+               empty message for ports some record was written. So all   +*/
/*+               written ports get a flag (doIndicate) to force a indicat. +*/
/*+                                                                         +*/
/*+               COMMIT is only allowed after an END. COMMIT shall only    +*/
/*+               be called if all WRITE and END were successful! CM does.  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmCommit(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                    Status;
	EDD_UPPER_PRM_COMMIT_PTR_TYPE pPrmCommit;
    EDDS_LOCAL_DDB_PTR_TYPE		  pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmCommit(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);



    Status = EDD_STS_OK;
	pPrmCommit = (EDD_UPPER_PRM_COMMIT_PTR_TYPE) pRQB->pParam;

	if ( LSA_HOST_PTR_ARE_EQUAL( pPrmCommit, LSA_NULL) )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmCommit: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        switch ( pDDB->pGlob->Prm.PrmState )
        {
            case EDDS_PRM_STATE_END:
                /* OK. copy record set B to record set A and activate it */
                EDDS_PrmCopySetB2A(pDDB);
                EDDS_PrmSetBToNotPresent(pDDB);
                pDDB->pGlob->Prm.PrmState = EDDS_PRM_STATE_PREPARED;

			    pPrmCommit->RsvIntervalRed.RxEndNs                = 0;
			    pPrmCommit->RsvIntervalRed.RxLocalTransferEndNs   = 0;
			    pPrmCommit->RsvIntervalRed.TxEndNs                = 0;
			    pPrmCommit->RsvIntervalRed.TxLocalTransferEndNs   = 0;
			    pPrmCommit->RsvIntervalRed.TxLocalTransferStartNs = 0;
                break;

            case EDDS_PRM_STATE_PREPARED:
            case EDDS_PRM_STATE_WRITE:
            default:

                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RequestPrmCommit: Wrong state (%d)", pDDB->pGlob->Prm.PrmState);

                Status = EDD_STS_ERR_SEQUENCE;
                break;
		}
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmCommit(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmWrite                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_WRITE               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_WRITE_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PRM_INDEX                                     +*/
/*+               EDD_STS_ERR_PRM_DATA                                      +*/
/*+               EDD_STS_ERR_PRM_BLOCK                                     +*/
/*+               EDD_STS_ERR_PRM_PORTID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_WRITE request       +*/
/*+                                                                         +*/
/*+               WRITE is only allowed after PREPARE or WRITE              +*/
/*+                                                                         +*/
/*+               Note: A PRM error will not cause a state-change!          +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//lint -esym(438,pRecordData)
//JB 11/11/2014 checked within EDDS_Request - pRecordData -> ++ on purpose!
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmWrite(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_PRM_WRITE_PTR_TYPE            pPrmWrite;
    EDDS_PRM_PTR_TYPE                       pPrm;
    EDD_UPPER_MEM_U8_PTR_TYPE               pRecordData;
    LSA_UINT16                              BlockType;
    LSA_UINT16                              BlockLength;
    LSA_UINT8                               VersionHigh;
    LSA_UINT8                               VersionLow;
    LSA_UINT8                               Pad1;
    LSA_UINT8                               Pad2;
    LSA_UINT16                              Slot;
    LSA_UINT16                              SubSlot;
    LSA_BOOL                                Local;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmWrite(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status    = EDD_STS_OK;
    pPrmWrite = (EDD_UPPER_PRM_WRITE_PTR_TYPE) pRQB->pParam;
    pPrm      = &pDDB->pGlob->Prm;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPrmWrite, LSA_NULL) ) ||
         ((pPrmWrite->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt  ) ) ||
         ( LSA_HOST_PTR_ARE_EQUAL( pPrmWrite->record_data, LSA_NULL) )
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if (( pPrm->PrmState == EDDS_PRM_STATE_PREPARED ) ||
            ( pPrm->PrmState == EDDS_PRM_STATE_WRITE   ))
        {
            switch (pPrmWrite->record_index)
            {
                    /* --------------------------------------------------------------*/
                    /* PDNCDataCheck                                                 */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK:
                    if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                    {
                        if ( pPrmWrite->record_data_length == EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE )
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            //@fixme am219 implement EDDS_GET_U08_INCR
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            if ( BlockLength == ( EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE - 4) )
                            {
                                if (( BlockType   == EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE    ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH            ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW             ))
                                {
                                    /* check record set and fill RecordSetB if OK */
                                    Status = EDDS_PrmPDNCDataCheckWrite(pDDB,pPrmWrite->record_data);
                                }
                                else
                                {
                                    EDDS_UPPER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDNC: BlockHeader wrong version (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDNC: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);
                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDNC: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDNC: Invalid port_id (<>0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDPortDataAdjust                                              */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST:
                    if ( pPrmWrite->edd_port_id != 0 ) /* 0 not allowed ! */
                    {
                        if (( pPrmWrite->record_data_length >= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE ) &&
                            ( pPrmWrite->record_data_length <= EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE ))
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            /* --------------------------------*/
                            /*    LSA_UINT8      Padding       */
                            /*    LSA_UINT8      Padding       */
                            /*    LSA_UINT16     SlotNumber    */
                            /*    LSA_UINT16     SubslotNumber */
                            /* --------------------------------*/

                            Pad1 = *pRecordData++;
                            Pad2 = *pRecordData++;
                            EDDS_GET_U16_INCR(pRecordData,Slot);
                            EDDS_GET_U16_INCR(pRecordData,SubSlot);

                            if ( BlockLength == (pPrmWrite->record_data_length -4) )
                            {

                                if (( BlockType   == EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE ) &&
                                    ( Pad1        == 0                                      ) &&
                                    ( Pad2        == 0                                      ) &&
                                    ( Slot        == pPrmWrite->slot_number                 ) &&
                                    ( SubSlot     == pPrmWrite->subslot_number              ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH            ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW             ))
                                {

                                    /* store slot/subslot for use with prmread */
                                    pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].SlotNumber    = Slot;
                                    pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].SubSlotNumber = SubSlot;

                                    Local = pPrmWrite->Local;
                                    /* If LesserCheckQuality we verify the same way as Local.. so set Local */
                                    if ( pDDB->pGlob->Prm.PortData.LesserCheckQuality ) Local = LSA_TRUE;

                                    /* check record set and fill RecordSetB if OK */
                                    Status = EDDS_PrmPDPortDataAdjustWrite(pDDB,Local,pPrmWrite->edd_port_id,pPrmWrite->record_data,BlockLength);

                                    /* copy record to SetB if OK */
                                    if ( Status == EDD_STS_OK )
                                    {
                                        EDDS_CopyUpper2Local(pPrmWrite->record_data,
                                                             pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].Record,
                                                             pPrmWrite->record_data_length);

                                        pDDB->pGlob->Prm.PortData.RecordSet_B[pPrmWrite->edd_port_id-1].RecordLength = pPrmWrite->record_data_length;

                                    }
                                }
                                else
                                {
                                    EDDS_UPPER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDPORT: BlockHeader wrong (0x%X,%d,%d,%d)",BlockType,Slot,SubSlot,VersionHigh);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }

                            }
                            else
                            {
                                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDPORT: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);
                                Status = EDD_STS_ERR_PRM_DATA;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDPORT: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDPORT: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;

                    /* --------------------------------------------------------------*/
                    /* PDSCFDataCheck                                                */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK:
                    if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                    {
                        if ( (EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE <= pPrmWrite->record_data_length)&&(EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE >= pPrmWrite->record_data_length) )
                        {
                            pRecordData = pPrmWrite->record_data;
                            /* --------------------------------*/
                            /* Record Header                   */
                            /* --------------------------------*/
                            EDDS_GET_U16_INCR(pRecordData,BlockType);
                            EDDS_GET_U16_INCR(pRecordData,BlockLength);
                            VersionHigh = *pRecordData++;
                            VersionLow  = *pRecordData++;

                            if ( BlockLength == (pPrmWrite->record_data_length -4) )
                            {
                                if (( BlockType   == EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_TYPE   ) &&
                                    ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH            ) &&
                                    ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW             ))
                                {
                                    /* check record set */
                                    Status = EDDS_PrmPDSCFDataCheckWrite(pDDB,pPrmWrite->record_data);
                                    /* copy record for read back if OK */
                                    if ( Status == EDD_STS_OK )
                                    {
                                        EDDS_CopyUpper2Local(pPrmWrite->record_data,
                                                             pDDB->pGlob->Prm.PDSCFDataCheck.Record,
                                                             pPrmWrite->record_data_length);

                                        pDDB->pGlob->Prm.PDSCFDataCheck.RecordLength = pPrmWrite->record_data_length;
                                        pDDB->pGlob->Prm.PDSCFDataCheck.Present = LSA_TRUE;
                                    }
                                }
                                else
                                {
                                    EDDS_UPPER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDSCF: BlockHeader wrong version (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDSCF: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);
                                Status = EDD_STS_ERR_PRM_BLOCK;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RequestPrmWrite: PDSCF: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                            Status = EDD_STS_ERR_PRM_DATA;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDNC: Invalid port_id (<>0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDInterfaceMrpDataAdjust                                      */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDINTERFACE_MRP_DATA_ADJUST:
                    /* we only need the info the a write was done! */
                    pDDB->pGlob->Prm.MRPInterface.Present_B = LSA_TRUE;
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDPortMrpDataAdjust                                           */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDPORT_MRP_DATA_ADJUST:
                    /* we only need the info the a write was done! */
                    if ( pPrmWrite->edd_port_id != 0 ) /* only 0 allowed */
                    {
                        pDDB->pGlob->Prm.MRPPort.Present_B[pPrmWrite->edd_port_id-1] = LSA_TRUE;
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDPORT: Invalid port_id (=0)!");
                        Status = EDD_STS_ERR_PRM_PORTID;
                    }
                    break;
                    /* --------------------------------------------------------------*/
                    /* PDSetDefaultPortStates                                        */
                    /* --------------------------------------------------------------*/
                case EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES:
                    if ( ! pPrm->FirstCommitExecuted )
                    {
                        /* we only need the info the a write was done! */
                        if ( pPrmWrite->edd_port_id == 0 ) /* only 0 allowed */
                        {
                            if ( pPrmWrite->record_data_length >= EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_MIN_SIZE )
                            {
                                pRecordData = pPrmWrite->record_data;
                                /* --------------------------------*/
                                /* Record Header + padding         */
                                /* --------------------------------*/
                                EDDS_GET_U16_INCR(pRecordData,BlockType);
                                EDDS_GET_U16_INCR(pRecordData,BlockLength);
                                VersionHigh = *pRecordData++;
                                VersionLow  = *pRecordData++;
                                Pad1 = *pRecordData++;
                                Pad2 = *pRecordData++;

                                if ( BlockLength == (pPrmWrite->record_data_length -4) )
                                {
                                    if (( BlockType   == EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_TYPE   ) &&
                                        ( VersionHigh == EDDS_PRM_BLOCK_VERSION_HIGH                ) &&
                                        ( VersionLow  == EDDS_PRM_BLOCK_VERSION_LOW                 ) &&
                                        ( Pad1        == 0                                          ) &&
                                        ( Pad2        == 0                                          ))
                                    {
                                        /* check record set */
                                        Status = EDDS_PrmPDSetDefPortStatesCheckWrite(pDDB,pPrmWrite->record_data,BlockLength);
                                    }
                                    else
                                    {
                                        EDDS_UPPER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                            "EDDS_RequestPrmWrite: PDSetDefaultPortStats: BlockHeader wrong (0x%X,%d,%d)",BlockType,VersionHigh,VersionLow);
                                        Status = EDD_STS_ERR_PRM_BLOCK;
                                    }
                                }
                                else
                                {
                                    EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_RequestPrmWrite: PDSetDefaultPortStats: BlockHeader wrong length (0x%X,%d)",BlockType,BlockLength);
                                    Status = EDD_STS_ERR_PRM_BLOCK;
                                }
                            }
                            else
                            {
                                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                                    "EDDS_RequestPrmWrite: PDSetDefaultPortStats: record_data_length wrong (%d)", pPrmWrite->record_data_length);
                                Status = EDD_STS_ERR_PRM_DATA;
                            }
                        }
                        else
                        {
                            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmWrite: PDSetDefaultPortStats: Invalid port_id (=0)!");
                            Status = EDD_STS_ERR_PRM_PORTID;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestPrmWrite: PDSetDefaultPortStats: Not allowed because first COMMIT already done!");
                        Status = EDD_STS_ERR_PRM_INDEX;
                    }
                    break;

				/* ---------------------------------------- */
				/* User specific records                    */
				/* These records are not used by EDDS;      */
                /* return OK!                               */
				/* ---------------------------------------- */
				case EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX:
				case EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX:
				case EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX:
				{
					EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_PRMRequestPrmWrite(): PrmRecord(0x%X) is not used by EDDS",
							pPrmWrite->record_index);
				}
				break;


                    /* --------------------------------------------------------------*/
                    /* unknown/unsupported                                           */
                    /* --------------------------------------------------------------*/
                default:
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RequestPrmWrite: record_index (%d)", pPrmWrite->record_index);

                    Status = EDD_STS_ERR_PRM_INDEX;
                    break;
            }

            /* at least one (good or bad) write occured */
            pPrm->PrmState = EDDS_PRM_STATE_WRITE;
        }
        else
        {
            Status = EDD_STS_ERR_SEQUENCE;
        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmWrite(), Status: 0x%X",
                           Status);

    return(Status);  //lint !e438 JB 03/12/2014 fix NEA6.0, at the moment we increment one time too much (on purpose -> streaming)
}
//lint +esym(613,pRQB,pHDB)
//lint +esym(438,pRecordData)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPrmIndication                   +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_INDICATION          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_PRM_WRITE_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_CANCEL                                         +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_PRM_INDICATION request  +*/
/*+                                                                         +*/
/*+               PortID 0 = Interface                                      +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmIndication(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                        	Status;
    EDD_UPPER_PRM_INDICATION_PTR_TYPE 	pRQBParam;
    LSA_UINT16                        	PortID;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPrmIndication(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRQBParam = (EDD_UPPER_PRM_INDICATION_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) )||
         ((pRQBParam->edd_port_id > pDDB->pGlob->HWParams.Caps.PortCnt)) )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestPrmIndication: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        PortID = pRQBParam->edd_port_id;

        EDDS_RQB_PUT_BLOCK_TOP( pDDB->pGlob->Prm.PrmIndReq[PortID].pBottom,
                                pDDB->pGlob->Prm.PrmIndReq[PortID].pTop,
                                pRQB);

        pRQBParam->diag_cnt = 0; /* init with 0 */

        /* check if something to indicate */
        EDDS_PrmIndicatePort(pDDB,PortID);

    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPrmIndication(), Status: 0x%X",
                           Status);


    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_PrmRequest                             +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_PRM_READ                +*/
/*+                                         EDD_SRV_PRM_PREPARE             +*/
/*+                                         EDD_SRV_PRM_WRITE               +*/
/*+                                         EDD_SRV_PRM_INDICATION          +*/
/*+                                         EDD_SRV_PRM_END                 +*/
/*+                                         EDD_SRV_PRM_COMMIT              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all EDDS General (Component         +*/
/*+               independent) requests.                                    +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 checked within EDDS_Request
//WARNING: be careful when using this function, make sure not to use pRQB, pHDB as null ptr's!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    LSA_BOOL                        Indicate;
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB		= pHDB->pDDB;

    EDDS_PRM_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_PrmRequest(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Indicate = LSA_TRUE;

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_PRM_READ:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_READ (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmRead(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_PREPARE:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_PREPARE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmPrepare(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_WRITE:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_WRITE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmWrite(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_INDICATION:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_INDICATION (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmIndication(pRQB,pHDB);
            if ( Status == EDD_STS_OK ) Indicate = LSA_FALSE;
            break;
        case EDD_SRV_PRM_END:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_END (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmEnd(pRQB,pHDB);
            break;
        case EDD_SRV_PRM_COMMIT:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_PRM_COMMIT (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestPrmCommit(pRQB,pHDB);
            EDDS_RequestFinish(pHDB,pRQB,Status);
            Indicate = LSA_FALSE;
            /* Indicate all neccessary. Has to be done after COMMIT confirmation! */
            EDDS_PrmIndicateAll(pDDB);
            break;
        default:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_PrmRequest: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }


    if (Indicate) EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_PRM_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_PrmRequest(), Status: 0x%X",
                           Status);

}
//lint +esym(613,pRQB,pHDB)


/*****************************************************************************/
/*  end of file EDDS_PRM.C                                                   */
/*****************************************************************************/

