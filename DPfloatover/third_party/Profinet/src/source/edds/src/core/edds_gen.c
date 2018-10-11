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
/*  F i l e               &F: edds_gen.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS General and Debug requests                  */
/*                                                                           */
/*                          - EddsGeneralRequest()                           */
/*                          - EddsDebugRequest()                             */
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
/*  24.11.03    JS    support for linespeeds of 1 and 10 Gbit.               */
/*  20.01.04    JS    Set Statistics values to 0 per default.                */
/*  05.04.04    JS    Using LSA_UNUSED_ARG                                   */
/*  18.06.04    JS    added EDD_SRV_GET_PARAMS request.                      */
/*                    Note: we currently dont check the new parameter PortID */
/*                          within LinkStatus requests, so we are compatible */
/*                          with current applications (may be added later!)  */
/*  12.08.04    JS    added EDD_SRV_SET_IP request.                          */
/*  17.08.04    JS    Added Debug-Request                                    */
/*  23.09.04    JS    Set Linkstatus to DOWN on error (instead of UNKNOWN)   */
/*  07.10.04    JS    added parametercheck for PortID with LinkStatus        */
/*  14.10.04    JS    added MC-address management                            */
/*  18.11.04    JS    added RefCnt                                           */
/*  30.11.04    JS    added EDD_SRV_GET_PORT_PARAMS                          */
/*                    added EDD_SRV_MULTICAST_FWD_CTRL                       */
/*  06.12.04    JS    removed EDDS_SRV_GET_MAC_ADDR                          */
/*                    added EDD_SRV_SET_LINE_DELAY                           */
/*  20.01.05    JS    added HardwareType to GET_PARAMS services.             */
/*                    added EDD_SRV_RX_OVERLOAD_IND_PROVIDE                  */
/*  28.01.05    JS    LTRC support                                           */
/*  06.04.05    JS    added RxTxTimeResolutionInNS                           */
/*  11.05.05    JS    changed LineDelay to LineDelayInTicks                  */
/*                    added EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT support      */
/*  18.05.05    JS    changed RxTxTimeDelayInNS to TimeDelayInNs             */
/*  08.06.05    JS    added rxdelay and txdelay to link indication           */
/*  22.06.05    JS    addition to HardwareType within EDD_SRV_GET_PARAMS     */
/*                    addition to EDD_SRV_GET_PORT_PARAMS                    */
/*  04.07.05    JS    added PortStatus                                       */
/*  08.08.05    JS    added IRTPortStatus                                    */
/*                    added EDD_SRV_SETUP_PHY request                        */
/*  19.08.05    JS    EDDS_SRV_SEND_PAUSE removed                            */
/*  26.09.05    JS    added RTClass2_PortStatus                              */
/*  04.10.05    JS    added LED-blink support                                */
/*  22.11.05    JS    added EDDS_HW_TYPE_FEATURE_NO_PARTIAL_DATA_SUPPORT     */
/*  12.01.06    JS    added RxMRP_Drop                                       */
/*  26.01.06    JS    added RxOverload statistics                            */
/*  14.02.06    JS    added EDDS_HW_TYPE_FEATURE_NO_MRRT_SUPPORT             */
/*                    added EDD_HW_TYPE_FEATURE_NO_CT_SUPPORT                */
/*  06.03.06    JS    Hardwaretype LLIF-define depended for MMRT and CT      */
/*                    added switch functions                                 */
/*  10.03.06    JS    added PRM services                                     */
/*  02.05.06    JS    added PortMAC addresses                                */
/*  19.05.06    JS    added EDD_SRV_SENDCLOCK_CHANGE support                 */
/*  31.05.06    JS    EDD_SRV_SETUP_PHY only allowed if no PRM configure     */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  30.06.06    JS    EDD_SRV_SRT_GET_PARAMS becomes a general request       */
/*  26.09.06    JS    added LinkSpeedModeConfigured and                      */
/*                    LinkSpeedModeCapability to EDD_SRV_GET_PORT_PARAMS     */
/*                    RTClass2_PortStatus set to EDD_RTCLASS2_NOT_SUPPORTED  */
/*  14.12.06    JS    added RsvIntervalOrange and RsvIntervalRed to          */
/*                    EDD_SRV_GET_PORT_PARAMS                                */
/*  17.01.07    JS    added "Autoneg" to ext link indication. optimizations. */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  19.02.07    JS    Fix: If only PortState changed a Indication RQB my be  */
/*                         lost within EDDS_RequestPHYInterrupt()            */
/*  04.03.07    JS    added parameter MACAddrPrio                            */
/*  23.07.07    JS    added parameter SyncId0_TopoOk                         */
/*                    added EDD_SRV_SET_SYNC_STATE                           */
/*                    added EDD_SRV_SET_REMOTE_PORT_STATE                    */
/*  07.11.07    JS    added parameter PortOnlyHasSyncTx                      */
/*  15.01.08    JS    service EDDS_SRV_SET_MRP_STATE not supported anymore   */
/*  14.04.08    JS    added parameter IsMDIX                                 */
/*  17.06.08    JS    added HardwareType flags:                              */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT    */
/*                    EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT     */
/*                    EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE             */
/*  28.11.08    JS    added HardwareType flags:                              */
/*                    EDD_HW_TYPE_FEATURE_NO_SYNC_SUPPORT                    */
/*  10.12.08    JS    added parameter "StatusExt" and "PhyStatus" to         */
/*                          EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE             */
/*  22.12.08    JS    MRRT_PortState removed                                 */
/*                    removed service EDDS_SRV_SWITCH_SET_PORT_REDUNDANT     */
/*                    removed service EDDS_SRV_SWITCH_SET_BRIDGE_MODE        */
/*                    removed service EDDS_SRV_SET_MRP_STATE                 */
/*  26.01.09    JS    removed parameter "DoCheck" (now always TRUE) from     */
/*                          EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE             */
/*  28.01.09    JS    added new services:                                    */
/*                                     EDD_SRV_GET_STATISTICS                */
/*                                     EDD_SRV_RESET_STATISTICS              */
/*                    removed service  EDDS_SRV_GET_STATS                    */
/*  30.01.09    JS    added MAUType and MediaType, AutonegCapAdvertised      */
/*  19.02.09    JS    removed PHYType from services                          */
/*                    added MediaType to EDD_RQB_GET_PORT_PARAMS_TYPE        */
/*  04.05.09    JS    EDD_SRV_SETUP_PHY no longer supported                  */
/*                    added EDDS_SRV_DEBUG_SETUP_PHY for testing.            */
/*  08.05.09    JS    Removed PhyTxDelay100MBit, PhyRxDelay100MBit,          */
/*                            MACTxDelay100MBit and MACRxDelay100MBit from   */
/*                            EDD_SRV_GET_PORT_PARAMS                        */
/*  02.10.09    JS    added EDD_HW_TYPE_FEATURE_NO_SYNC_MASTER_SUPPORT       */
/*  29.07.10    AM    removed parameter PortOnlyHasSyncTx                    */
/*  21.01.11    AM    added EDDS_RequestGetStatisticsAll                     */
#endif
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  8
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_GEN */

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
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_LLTimerCbf                       +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ProvTimerRQB in provider mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LL one shot timer callbackfuntion                         +*/
/*+               Called after the LL timer elapsed.                        +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_LLTIMER_REQUEST()                                 +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LLTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{
  EDDS_ASSERT_NULL_PTR(pIntRQB);

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup!     */
        pIntRQB->InUse = LSA_TRUE;
        EDDS_DO_LLTIMER_REQUEST(pIntRQB->pRQB);
    }

}

#ifdef EDDS_CFG_LED_BLINK_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_LEDBlinkTimerCbf                       +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ProvTimerRQB in provider mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LEB blink cyclic timer callbackfuntion                    +*/
/*+               Called after the Cycletimer elapsed.                      +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_LED_BLINK_TIMER_REQUEST()                         +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LEDBlinkTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{
  EDDS_ASSERT_NULL_PTR(pIntRQB);

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup!     */
        pIntRQB->InUse = LSA_TRUE;
        EDDS_DO_LED_BLINK_TIMER_REQUEST(pIntRQB->pRQB);
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_LEDBlinkCycleRequest                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LED Cycle Request function.                               +*/
/*+               This function is called after a LED cylcic timer-request  +*/
/*+               occured (triggered by EDDS_DO_LED_BLINK_TIMER_REQUEST)   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 10/11/2014 - checked within EDDS_TriggerRequest
//WARNING: be careful not to call this functino with pDDB = 0 (-> without EDDS_TriggerRequest)
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LEDBlinkCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    EDDS_LED_BLINK_PTR_TYPE                     pLED;
    LSA_BOOL                                    LEDOn;
    LSA_BOOL                                    SetLEDMode;
    LSA_BOOL                                    Restore;
    LSA_RESULT                                  Status = EDD_STS_OK;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_LEDBlinkCycleRequest(pDDB: 0x%X)",
                           pDDB);


    pLED = &pDDB->pGlob->LEDBLink;
    LEDOn = LSA_FALSE;

    if (( pDDB->pGlob->HWIsSetup ) && ( pLED->Status == EDDS_LED_BLINK_ACTIVE ))
    {
        SetLEDMode = LSA_FALSE;
        Restore    = LSA_FALSE;

        if ( pLED->RemainTotal )
        {
            pLED->RemainTotal--;

            if ( pLED->Remain <= 1 )
            {
                /* toggle LED */
                pLED->Remain = pLED->OnOffDurationIn500msTicks;
                if ( pLED->LEDOn )
                {
                    pLED->LEDOn = LSA_FALSE;
                    LEDOn = LSA_FALSE;
                }
                else
                {
                    pLED->LEDOn = LSA_TRUE;
                    LEDOn = LSA_TRUE;
                }

                SetLEDMode = LSA_TRUE;
            }
            else pLED->Remain--;
        }
        else  /* we stop if RemainTotal is 0 */
        {
            LSA_UINT16 RetVal;

            pLED->Status = EDDS_LED_BLINK_INACTIVE;
            SetLEDMode   = LSA_TRUE;
            Restore      = LSA_TRUE;

            EDDS_STOP_TIMER(&RetVal,
                            pDDB->pGlob->LEDBLink.Timer.TimerID);

            pDDB->pGlob->LEDBLink.Timer.InProgress = LSA_FALSE;

        }
        /* Set LEDs on all ports */
        if ( SetLEDMode )
        {

            LSA_UINT32 i;

            for (i=1; i<=pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
                if (Restore)
                {
                    EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE, "==> EDDS_LL_LED_RESTORE_MODE(pDDB: 0x%X, Port: %d)", pDDB, i);
                    Status = EDDS_LL_LED_RESTORE_MODE(pDDB,i);
                    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_LED_RESTORE_MODE(). Status: 0x%X",Status);
                }
                else
                {
                    EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE, "==> EDDS_LL_LED_SET_MODE(pDDB: 0x%X, Port: %d = %d)", pDDB, i,LEDOn);
                    Status = EDDS_LL_LED_SET_MODE(pDDB,i,LEDOn);
                    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_LED_SET_MODE(). Status: 0x%X",Status);
                }
            }
        }
    }

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_LEDBlinkCycleRequest()");

}
//lint +esym(613,pDDB)

#endif

#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagTimerCbf                            +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ProvTimerRQB in provider mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: LEB blink cyclic timer callbackfuntion                    +*/
/*+               Called after the Cycletimer elapsed.                      +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_LED_BLINK_TIMER_REQUEST()                         +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup!     */
        pIntRQB->InUse = LSA_TRUE;
        EDDS_DO_DIAG_TIMER_REQUEST(pIntRQB->pRQB);
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DiagCycleRequest                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Diag Cycle Request Timer function.                        +*/
/*+               This function is called after a Diag cycle timer-request  +*/
/*+               occured (triggered by EDDS_DO_DIAG_TIMER_REQUEST)         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_RESULT                                  Status;
    LSA_UINT32                                  DropCnt;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DiagCycleRequest(pDDB: 0x%X)",
                           pDDB);


    Status = EDD_STS_OK;

    if ( pDDB->pGlob->HWIsSetup ) /* if hardware not setup we do nothing */
    {
        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"==> EDDS_LL_SWITCH_DROP_CNT(pDDB: 0x%X)",pDDB);

        Status = EDDS_LL_SWITCH_DROP_CNT(pDDB,&DropCnt);

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"<== EDDS_LL_SWITCH_DROP_CNT(Status: 0x%X, DropCnt: %d)",
                            Status,
                            DropCnt);

        if ( Status == EDD_STS_OK)
        {
            EDDS_DiagFrameDropHandler(pDDB,DropCnt);
        }
        else
        {
            /* no LL error expected */
            EDDS_FatalError(EDDS_FATAL_ERR_LL,
                            EDDS_MODULE_ID,
                            __LINE__);
        }
    }


    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DiagCycleRequest()");

}
#endif


#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RxOverloadTimerCbf                      +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ProvTimerRQB in provider mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Rx Overload cyclic Timer callbackfuntion                  +*/
/*+               Called after the Cycletimer elapsed.                      +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_RX_OVERLOAD_TIMER_REQUEST()                       +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup!     */
        pIntRQB->InUse = LSA_TRUE;
        EDDS_DO_RX_OVERLOAD_TIMER_REQUEST(pIntRQB->pRQB);
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RxOverloadIndicateEvent                 +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: If a Rx Overload indication request is pending, we check  +*/
/*+               if the status has changed and if so we indciate the new   +*/
/*+               status.                                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadIndicateEvent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_RESULT                                  Status;
    LSA_HANDLE_TYPE                             Handle;
    EDDS_LOCAL_HDB_PTR_TYPE                     pHDB;
    EDD_UPPER_RX_OVERLOAD_IND_PROVIDE_PTR_TYPE  pParam;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RxOverloadIndicateEvent(pDDB: 0x%X)",
                           pDDB);

    pHDB   = LSA_NULL;
    Handle = 0;
    Status = EDD_STS_OK;

    /* loop over all handles owned by DDB */

    pDDB->pGlob->RxStats.RxOverload++;   /* number ob RxOverload events */

    while ( Status == EDD_STS_OK )
    {
        Status = EDDS_HandleGetHDB(Handle,&pHDB);

        if (( Status == EDD_STS_OK ) && ( pHDB->InUse ) && ( pHDB->pDDB == pDDB ))
        {
            if ( ! LSA_HOST_PTR_ARE_EQUAL( pHDB->RxOverloadIndReq, LSA_NULL))
            {
                pParam = (EDD_UPPER_RX_OVERLOAD_IND_PROVIDE_PTR_TYPE) pHDB->RxOverloadIndReq->pParam;

                /* check if we have a different state (should be). If so indicate yet */
                if ( pParam->Status != pDDB->pGlob->RxOverload.Status )
                {

                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                                        "EDDS_RxOverloadIndicateEvent: Indicating Rx-overload event. Status: 0x%X",
                                        pDDB->pGlob->RxOverload.Status);

                    pParam->Status = pDDB->pGlob->RxOverload.Status;
                    EDDS_RequestFinish(pHDB,pHDB->RxOverloadIndReq,EDD_STS_OK);
                    pHDB->RxOverloadIndReq = LSA_NULL;
                }
            }
        }

        Handle++;
    }

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RxOverloadIndicateEvent()");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RxOverloadCycleRequest                  +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Rx Overload Cycle Request Timer function.                 +*/
/*+               This function is called after a Rx overload timer-request +*/
/*+               occured (triggered by EDDS_DO_RX_OVERLOAD_TIMER_REQUEST)  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    EDDS_LOCAL_HDB_PTR_TYPE                     pHDB;


    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RxOverloadCycleRequest(pDDB: 0x%X)",
                           pDDB);


    pHDB = LSA_NULL;

    if ( pDDB->pGlob->HWIsSetup ) /* if hardware not setup we do nothing */
    {

        pDDB->pGlob->RxOverload.RxCnt = 0;  /* trigger RxCnt */

        /* If the receiver ist off, we check if we have to turn it ON again.       */

        if ( pDDB->pGlob->RxOverload.RxOFF )
        {
            if ( pDDB->pGlob->RxOverload.OFFCnt )           /* should be if RxOFF !*/
            {
                if ( ! ( --pDDB->pGlob->RxOverload.OFFCnt)) /* if now 0 we turn on */
                {
                    /* turn receive on again */

                    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                                          "EDDS_RxOverloadCycleRequest: Turning receiver ON after overload event! (pDDB: 0x%X)",
                                          pDDB);

                    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_RX_OVERLOAD_RX_ON(pDDB: 0x%X)",pDDB);

                    EDDS_LL_RX_OVERLOAD_RX_ON(pDDB);

                    EDDS_LOWER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_RX_OVERLOAD_RX_ON()");

                    pDDB->pGlob->RxOverload.RxOFF = LSA_FALSE;
                    pDDB->pGlob->RxOverload.ONCnt = EDDS_CFG_RX_OVERLOAD_RXON_MIN;
                }
            }
            else
            {
                EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                EDDS_MODULE_ID,
                                __LINE__);
            }
        }
        else /* receveiver is ON */
        {
            if ( pDDB->pGlob->RxOverload.ONCnt )
            {
                pDDB->pGlob->RxOverload.ONCnt--;
            }
        }

        /* The receiver has to remain on for  EDDS_CFG_RX_OVERLOAD_RXON_MIN cycles */
        /* before we signal a "NO_OVERLOAD" event to user.                        */
        /* EDDS_CFG_RX_OVERLOAD_RXON_MIN may be 0!                                */

        if (( ! pDDB->pGlob->RxOverload.RxOFF ) &&
            ( ! pDDB->pGlob->RxOverload.ONCnt ) &&
            ( pDDB->pGlob->RxOverload.Status == EDD_RX_OVERLOAD ))
        {
            pDDB->pGlob->RxOverload.Status   = EDD_NO_RX_OVERLOAD;
            pDDB->pGlob->RxOverload.OFFLevel = 0;
            /* we are on without new overload */
            EDDS_RxOverloadIndicateEvent(pDDB);
        }
    }


    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RxOverloadCycleRequest()");

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_EtupLinkRQBParams                             +*/
/*+  Input/Output   :    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE   pParam+*/
/*+                      EDDS_LINK_STAT_PTR_TYPE  pLinkStat                 +*/
/*+  Result         :    --                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup Link RQB Params with actual ones from DDB           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SetupLinkParams(
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam,
    EDDS_LINK_STAT_PTR_TYPE                     pLinkStat
)
{
  EDDS_ASSERT_NULL_PTR(pParam);
  EDDS_ASSERT_NULL_PTR(pLinkStat);

    pParam->Status                   = pLinkStat->Status;
    pParam->PhyStatus                = pLinkStat->PhyStatus;
    pParam->Speed                    = pLinkStat->Speed;
    pParam->Mode                     = pLinkStat->Mode;
    pParam->Autoneg                  = pLinkStat->Autoneg;
    pParam->PortStatus               = pLinkStat->PortStatus;
    pParam->IRTPortStatus            = EDD_IRT_NOT_SUPPORTED;
    pParam->RTClass2_PortStatus      = EDD_RTCLASS2_NOT_SUPPORTED;
    pParam->MAUType                  = pLinkStat->MAUType;
    pParam->MediaType                = pLinkStat->MediaType;
    pParam->LineDelayInTicksMeasured = pLinkStat->LineDelay;
    pParam->CableDelayInNsMeasured   = pLinkStat->CableDelayNs;
    pParam->PortTxDelay              = 0;
    pParam->PortRxDelay              = 0;
    pParam->PortState                = pLinkStat->PortState;
    pParam->SyncId0_TopoOk           = LSA_FALSE;
    pParam->ShortPreamble            = LSA_FALSE;
    pParam->TxFragmentation			 = LSA_FALSE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IndicateExtLinkChange                         +*/
/*+  Input          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB                   +*/
/*+                      LSA_UINT32                  PortID (0=auto,1..x)   +*/
/*+  Result         :    --                                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates a ext link change to all channels with          +*/
/*+               indication resource.                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 11/11/2014 check within EDDS_GetDDB (previously called)
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_IndicateExtLinkChange(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  PortID
)
{

    LSA_RESULT                              Status;
    LSA_HANDLE_TYPE                         Handle;
    EDDS_LOCAL_HDB_PTR_TYPE                 pHDB2;
    EDD_UPPER_RQB_PTR_TYPE                  pRQB2;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam;

    Handle = 0;
    Status = EDD_STS_OK;

    /* loop over all handles owned by DDB and check if we have to indicate a change */

    while ( Status == EDD_STS_OK )
    {
        Status = EDDS_HandleGetHDB(Handle,&pHDB2);

        if (( Status == EDD_STS_OK ) && ( pHDB2->InUse ) && ( pHDB2->pDDB == pDDB ))
        {
            /* only if we have a EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT pending */

            if (( ! LSA_HOST_PTR_ARE_EQUAL(pHDB2->LinkIndReq[PortID].pBottom, LSA_NULL)) &&
                ( EDD_RQB_GET_SERVICE(pHDB2->LinkIndReq[PortID].pBottom) == EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT ))
            {

                EDDS_RQB_REM_BLOCK_BOTTOM(pHDB2->LinkIndReq[PortID].pBottom,
                                          pHDB2->LinkIndReq[PortID].pTop,
                                          pRQB2);

                pParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB2->pParam;

                EDDS_SetupLinkParams(pParam,&pDDB->pGlob->LinkStatus[PortID]);
                EDDS_RequestFinish(pHDB2,pRQB2,EDD_STS_OK);
            }
        }
        Handle++;

    }

}
//lint +esym(613,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestPHYInterrupt                    +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the PHY-Interrupt request.          +*/
/*+                                                                         +*/
/*+               This function is called if a link-status change was       +*/
/*+               detected.                                                 +*/
/*+               We will determine the acutal Link-Status and store it     +*/
/*+               within device-management.                                 +*/
/*+                                                                         +*/
/*+               If Indication-resources are present, we indicate the      +*/
/*+               Status-change.                                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//JB 11/11/2014 always checked before calling this function
//WARNING: if you call this function, make sure not to use pDDB as null ptr!
//AD_DISCUSS what about the function EDDS_PrmCopySetB2A there is no check
//checks are in EDDS_InterruptRequest and in EDDS_SetupLinkRequest
//@fixme am2219 code refactoring unused return value
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPHYInterrupt(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    LSA_RESULT                                  Status;
    EDD_RQB_GET_LINK_STATUS_TYPE                LinkStatus;
    LSA_HANDLE_TYPE                             Handle;
    EDDS_LOCAL_HDB_PTR_TYPE                     pHDB;
    EDD_UPPER_RQB_PTR_TYPE                      pRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam;
    LSA_UINT32                                  i;
    LSA_UINT16                                  MAUType;
    LSA_UINT8                                   MediaType;
    LSA_UINT32                                  AutonegCapAdvertised;
    LSA_UINT8                                   LinkSpeedModeConfigured;
    LSA_UINT32                                  PortStatus;
    LSA_BOOL                                    LinkChanged,PortChanged;
    EDDS_LINK_STAT_PTR_TYPE                     pLinkStat;

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestPHYInterrupt(pDDB: 0x%X)",
                           pDDB);

    pHDB   = LSA_NULL;
    Status = EDD_STS_OK;

    /* --------------------------------------------------------------------------*/
    /* Loop all ports and get actual link states                                 */
    /* --------------------------------------------------------------------------*/

    for ( i=1; i<= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
    {

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,i);

        /* NOTE: LinkSpeedModeConfigured is not used here                          */
        /*       whenever LL_SET_LINK_STATE is called the actual phy setup is read */
        Status = EDDS_LL_GET_LINK_STATE(pDDB,i,&LinkStatus,&MAUType,&MediaType,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);

        EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(Port: %d, Mode: %d,Speed: %d, Stat: %d)",
                            i,
                            LinkStatus.Mode,
                            LinkStatus.Speed,
                            LinkStatus.Status);

        EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(PMAUType: %d, MediaType: %d,PortStatus: %d",
                            MAUType,
                            MediaType,
                            PortStatus);

        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(AutonegCap: 0x%X, Status : 0x%X",
                            AutonegCapAdvertised,
                            Status);

        if ( Status != EDD_STS_OK )
        {
            LinkStatus.Mode   = EDD_LINK_UNKNOWN;
            LinkStatus.Speed  = EDD_LINK_UNKNOWN;
            LinkStatus.Status = EDD_LINK_DOWN;  /* On error we signal link down */
            MAUType           = EDD_MAUTYPE_UNKNOWN;
            MediaType         = EDD_MEDIATYPE_UNKNOWN;
            PortStatus        = EDD_PORT_NOT_PRESENT;
            AutonegCapAdvertised     = 0;
        }

        /* Check if there are changes to the actual values. if so we */
        /* indicate to all handles which have indication resources   */

        LinkChanged = LSA_FALSE;
        PortChanged = LSA_FALSE;
        pLinkStat   = &pDDB->pGlob->LinkStatus[i];

        if (( LinkStatus.Status != pLinkStat->Status    ) ||
            ( LinkStatus.Mode   != pLinkStat->Mode      ) ||
            ( LinkStatus.Speed  != pLinkStat->Speed     )) LinkChanged = LSA_TRUE;

        if (( MAUType               != pLinkStat->MAUType       ) ||
            ( MediaType             != pLinkStat->MediaType     ) ||
            ( PortStatus            != pLinkStat->PortStatus    )) PortChanged = LSA_TRUE;

        if ( LinkChanged || PortChanged )
        {
            Handle = 0;
            Status = EDD_STS_OK;

            /* loop over all handles owned by DDB */
            while ( Status == EDD_STS_OK )
            {
                Status = EDDS_HandleGetHDB(Handle,&pHDB);

                if (( Status == EDD_STS_OK ) &&
                    ( pHDB->InUse ) &&
                    ( pHDB->pDDB == pDDB ) &&
                    ( ! LSA_HOST_PTR_ARE_EQUAL(pHDB->LinkIndReq[i].pBottom,LSA_NULL)))  /* Indication RQB present ? */
                {

                    /* PortChanged only indicated if EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT */
                    /* or LinkChanged too.                                               */

                    if ( ( LinkChanged ) ||
                         ( EDD_RQB_GET_SERVICE(pHDB->LinkIndReq[i].pBottom) == EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT))
                    {

                        EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[i].pBottom,
                                                  pHDB->LinkIndReq[i].pTop,
                                                  pRQB);

                        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) /* should be, already checked */
                        {

                            pParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

                            EDDS_SetupLinkParams(pParam,pLinkStat); /* copy actual parameters */

                            /* set changed ones.. */
                            pParam->Status      = LinkStatus.Status;
                            pParam->Mode        = LinkStatus.Mode;
                            pParam->Speed       = LinkStatus.Speed;
                            pParam->MAUType     = MAUType;
                            pParam->MediaType   = MediaType;
                            pParam->PortStatus  = (LSA_UINT8) PortStatus;

                            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
                        }
                    }
                }

        /* copy new status to actual status */
        pLinkStat->Status       = LinkStatus.Status;
        pLinkStat->Mode         = LinkStatus.Mode;
        pLinkStat->Speed        = LinkStatus.Speed;
        pLinkStat->MAUType      = MAUType;
        pLinkStat->MediaType    = MediaType;
        pLinkStat->PortStatus   = (LSA_UINT8)  PortStatus;
        pLinkStat->AutonegCapAdvertised     = AutonegCapAdvertised;

                Handle++;
            }
            Status = EDD_STS_OK;
        }

    } /* for */

    /*---------------------------------------------------------------------------*/
    /* init AUTO mode setting. This is the fastest mode present on any port      */
    /*---------------------------------------------------------------------------*/

    if ( EDDS_FindLinkAutoMode(pDDB) ) /* if AUTO link changed .. */
    {
        Handle      = 0;
        Status      = EDD_STS_OK;
        pLinkStat   = &pDDB->pGlob->LinkStatus[0];

        /* loop over all handles owned by pDDB */
        while ( Status == EDD_STS_OK )
        {
            Status = EDDS_HandleGetHDB(Handle,&pHDB);

            if (( Status == EDD_STS_OK ) && ( pHDB->InUse ) && ( pHDB->pDDB == pDDB ))
            {

                EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[0].pBottom,
                                          pHDB->LinkIndReq[0].pTop,
                                          pRQB);

                if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
                {
                    pParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

                    EDDS_SetupLinkParams(pParam,pLinkStat); /* copy actual parameters */
                    EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
                }
            }

            Handle++;

        } /* while */
    }

    /*---------------------------------------------------------------------------*/

    #ifdef EDDS_CFG_SEND_CYCLE_BASED

    /* Set maximum acyclic Send Byte count depending on actual AUTO mode */

    switch (pDDB->pGlob->LinkStatus[0].Speed)
    {
        case EDD_LINK_SPEED_100:
            pDDB->pGlob->TxMaxAcylic = EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_100;
            break;
        case EDD_LINK_SPEED_1000:
            pDDB->pGlob->TxMaxAcylic = EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_1000;
            break;
        case EDD_LINK_SPEED_10000:
            pDDB->pGlob->TxMaxAcylic = EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10000;
            break;
        default:
            pDDB->pGlob->TxMaxAcylic = EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10;
            break;
    }

    #endif

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestPHYInterrupt(), Status: 0x%X",
                           EDD_STS_OK);
    return EDD_STS_OK;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GetMcMACEntry                          +*/
/*+  Input/Output               EDDS_MC_MAC_INFO_PTR_TYPE            pMCInfo+*/
/*+                        :    EDD_UPPER_MEM_U8_PTR_TYPE            pMCAddr+*/
/*+                             LSA_UIN32                           *pIdx   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pMCInfo              : Pointer to MC Management                        +*/
/*+  pMCAddr              : pointer to MC-MAC address (6 Bytes)             +*/
/*+  pIdx                 : Returned Index within MC-Info                   +*/
/*+                         if result = LSA_TRUE :Index of used MC entry    +*/
/*+                         if result = LSA_FALSE:Index of free MC entry    +*/
/*+                                               or EDDS_MC_NO_IDX         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs MC-management for pMCAddr. if one is found        +*/
/*+               result is set to LSA_TRUE and pIdx gets the index.        +*/
/*+               If not not found (LSA_FALSE), but a free empty entry is   +*/
/*+               present pIdx gets the idx. If no free or used entry then  +*/
/*+               pIdx gets EDDS_MC_NO_IDX                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIdx)
//JB 11/11/2014 pIdx will be initialized within this function -> no danger of null ptr
//AD_DISCUSS 20/11/2014 what about if pIdx point to null
EDDS_STATIC LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_GetMcMACEntry(
    EDDS_MC_MAC_INFO_PTR_TYPE             pMCInfo,
    EDD_UPPER_MEM_U8_PTR_TYPE             pMCAddr,
    LSA_UINT32                           *pIdx)
{

    EDDS_MC_MAC_PTR_TYPE pMC;
    LSA_BOOL            UsedFound;
    LSA_BOOL            FreeFound;
    LSA_UINT32          i;

  EDDS_ASSERT_NULL_PTR(pMCInfo);
  EDDS_ASSERT_NULL_PTR(pMCAddr);

    UsedFound = LSA_FALSE;
    FreeFound = LSA_FALSE;
    i         = 0;
    pMC       = &pMCInfo->McMac[0];
    *pIdx     = EDDS_MC_NO_IDX; /* none */

    while (( i< EDDS_MC_MAC_CNT ) && ( ! UsedFound ))
    {
        if (pMC->Cnt) /* Entry used */
        {
            if ((pMC->MAC.MacAdr[5] == pMCAddr[5]) &&
                (pMC->MAC.MacAdr[4] == pMCAddr[4]) &&
                (pMC->MAC.MacAdr[3] == pMCAddr[3]) &&
                (pMC->MAC.MacAdr[2] == pMCAddr[2]) &&
                (pMC->MAC.MacAdr[1] == pMCAddr[1]) &&
                (pMC->MAC.MacAdr[0] == pMCAddr[0]))
            {
                UsedFound   = LSA_TRUE;
                *pIdx       = i;
            }
        }
        else
        {
            if ( ! FreeFound )
            {
                FreeFound   = LSA_TRUE;
                *pIdx       = i;
            }
        }

        i++;
        pMC++;
    }

    return(UsedFound);

}
//lint +esym(613,pIdx)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestMC                              +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_MULTICAST               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_MULTICAST_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+     LSA_UINT8               Mode            Mode                        +*/
/*+                                             EDDS_MUTLICAST_ENABLE       +*/
/*+                                             EDD_MULTICAST_DISABLE       +*/
/*+                                             EDD_MULTICAST_DISABLE_ALL   +*/
/*+                                                                         +*/
/*+     EDD_MAC_ADR_TYPE        MACAddr         6-Byte MC address if        +*/
/*+                                             Mode <> DISABLE_ALL         +*/
/*+  RQB-will not be changed.                                               +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_MULTICAST request.      +*/
/*+                                                                         +*/
/*+               Depending on Mode the spezified MACAddr will be enabled   +*/
/*+               or disabled.                                              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: MACAddr is no multicast address or     +*/
/*+                                  MACAddr not found (on disable)         +*/
/*+                                  pParam is LSA_NULL                     +*/
/*+                                  Mode unknown                           +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_RESOURCE: No free entry for Multicastaddress  +*/
/*+                                                                         +*/
/*+               The EDDS checks if a MC Address already enabled. If so    +*/
/*+               the EDDS only increments a "used" counter. So the LLIF    +*/
/*+               dont gets a ENABLE for the same MC-Address again.         +*/
/*+                                                                         +*/
/*+               On disable, this counter is decremented and the MC        +*/
/*+               address is only disabled within LLIF if the counter       +*/
/*+               reaches 0 (not in use anymore)                            +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestMC(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDD_UPPER_MULTICAST_PTR_TYPE        pMCParam;
    LSA_UINT32                          Idx;
    EDDS_MC_MAC_INFO_PTR_TYPE           pMCInfo;
    LSA_UINT32                          i;
    EDDS_LOCAL_DDB_PTR_TYPE     		pDDB	= pHDB->pDDB;


    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestMC(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pMCParam = (EDD_UPPER_MULTICAST_PTR_TYPE) pRQB->pParam;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pMCParam, LSA_NULL) )
    {
        pMCInfo = pDDB->pGlob->pMCInfo;
        pMCParam->RefCnt = 0;

        switch (pMCParam->Mode)
        {
                /* ------------------------------------------------------------*/
                /* enable one MC-MACs                                          */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_ENABLE:
                /* check if MC already used */
                if ( EDDS_GetMcMACEntry(
                         pMCInfo,
                         &pMCParam->MACAddr.MacAdr[0],
                         &Idx))
                {
                    /* already active. only increment used counter */
                    pMCInfo->McMac[Idx].Cnt++;
                    pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;
                }
                else
                {
                    /* free idx found ? */
                    if ( Idx < EDDS_MC_MAC_CNT)
                    {

                        EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                            "==> EDDS_LL_MC_ENABLE(pDDB: 0x%X, MAC: %2X-%2X-%2X....)",
                                            pDDB,
                                            pMCParam->MACAddr.MacAdr[0],
                                            pMCParam->MACAddr.MacAdr[1],
                                            pMCParam->MACAddr.MacAdr[2]);

                        Status = EDDS_LL_MC_ENABLE(pDDB,&pMCParam->MACAddr);

                        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_MC_ENABLE(). Status: 0x%X",Status);


                        if ( Status == EDD_STS_OK )
                        {
                            pMCInfo->McMac[Idx].Cnt = 1;
                            for (i=0; i<EDD_MAC_ADDR_SIZE; i++) pMCInfo->McMac[Idx].MAC.MacAdr[i] = pMCParam->MACAddr.MacAdr[i];
                            pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;
                        }
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestMC(): No more free multicast resources!");
                        Status = EDD_STS_ERR_RESOURCE;
                    }
                }
                break;

                /* ------------------------------------------------------------*/
                /* disable one MC-MACs                                         */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_DISABLE:
                /* check if MC is enabled (if not we do nothing) */
                if ( EDDS_GetMcMACEntry(
                         pMCInfo,
                         &pMCParam->MACAddr.MacAdr[0],
                         &Idx))
                {
                    /* Found one. last one ? */
                    if ( pMCInfo->McMac[Idx].Cnt == 1 )
                    {

                        EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                            "==> EDDS_LL_MC_DISABLE(pDDB: 0x%X, MAC: %2X-%2X-%2X....)",
                                            pDDB,
                                            pMCParam->MACAddr.MacAdr[0],
                                            pMCParam->MACAddr.MacAdr[1],
                                            pMCParam->MACAddr.MacAdr[2]);

                        Status = EDDS_LL_MC_DISABLE(pDDB,LSA_FALSE,&pMCParam->MACAddr);

                        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_MC_DISABLE(). Status: 0x%X",Status);

                        if (Status == EDD_STS_OK)
                        {
                            pMCInfo->McMac[Idx].Cnt = 0;
                            for (i=0; i<EDD_MAC_ADDR_SIZE; i++) pMCInfo->McMac[Idx].MAC.MacAdr[i] = 0;
                        }
                    }
                    else
                    {
                        /* only decrement usage counter */
                        pMCInfo->McMac[Idx].Cnt--;
                    }

                    pMCParam->RefCnt = pMCInfo->McMac[Idx].Cnt;

                }
                break;

                /* ------------------------------------------------------------*/
                /* disable all MC-MACs                                         */
                /* ------------------------------------------------------------*/
            case EDD_MULTICAST_DISABLE_ALL:


                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> EDDS_LL_MC_DISABLE(pDDB: 0x%X, all)",
                                    pDDB);

                Status = EDDS_LL_MC_DISABLE(pDDB,LSA_TRUE,LSA_NULL);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_MC_DISABLE(). Status: 0x%X",Status);

                if ( Status == EDD_STS_OK )
                {
                    EDDS_MEMSET_LOCAL(pMCInfo,0,sizeof(EDDS_MC_MAC_INFO_TYPE));
                    pMCParam->RefCnt = 0;
                }
                break;
            default:
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestMC: Invalid mode (0x%X)!",pMCParam->Mode);
                Status = EDD_STS_ERR_PARAM;
                break;
        }
    }
    else
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestMC: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }


    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestMC(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestMCFwdCtrl                       +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_MULTICAST_FWD_CTRL      +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_MULTICAST_FWD_CTRL_TYPE                       +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_MULTICAST_FWD_CTRL req. +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestMCFwdCtrl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE   pMCParam;
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB		= pHDB->pDDB;


    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestMCFwdCtrl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pMCParam = (EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE) pRQB->pParam;

    if ( LSA_HOST_PTR_ARE_EQUAL( pMCParam, LSA_NULL) )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestMCFwdCtrl: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        #ifdef LLIF_CFG_SWITCH_SUPPORT
      if(EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd))
      {
      EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB: 0x%X)",pDDB);

      Status = EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(
             pDDB,
             pMCParam->pPortIDModeArray,
             pMCParam->PortIDModeCnt,
             pMCParam->MACAddrPrio,
             pMCParam->MACAddrGroup,
             pMCParam->MACAddrLow);

      EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(). Status: 0x%X",Status);
      }
        #else
        /* EDDS don´t support forwarding. we do nothing at all */
        LSA_UNUSED_ARG(pDDB);
        #endif

    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestMCFwdCtrl(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB,pRQB)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSetLineDelay                    +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SET_LINE_DELAY          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_SET_LINE_DELAY_TYPE                           +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SET_LINE_DELAY request  +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetLineDelay(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_SET_LINE_DELAY_PTR_TYPE       pLineDelay;
    LSA_UINT32                              Idx;
    LSA_BOOL                                Changed;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSetLineDelay(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pLineDelay = (EDD_UPPER_SET_LINE_DELAY_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pLineDelay, LSA_NULL) ) ||
         ( (pLineDelay->PortID == 0) || (pLineDelay->PortID > pDDB->pGlob->HWParams.Caps.PortCnt))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetLineDelay: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Changed = LSA_FALSE;
        Idx     = pLineDelay->PortID;

        if (( pDDB->pGlob->LinkStatus[Idx].LineDelay    != pLineDelay->LineDelayInTicksMeasured ) ||
            ( pDDB->pGlob->LinkStatus[Idx].CableDelayNs != pLineDelay->CableDelayInNsMeasured   ))
        {
            Changed = LSA_TRUE;
        }

        pDDB->pGlob->LinkStatus[Idx].LineDelay    = pLineDelay->LineDelayInTicksMeasured;
        pDDB->pGlob->LinkStatus[Idx].CableDelayNs = pLineDelay->CableDelayInNsMeasured;

        if (Changed) EDDS_IndicateExtLinkChange(pDDB,Idx);

        Status = EDD_STS_OK;  /* no error */
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSetLineDelay(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetPortParams                   +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_PORT_PARAMS         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_GET_PORT_PARAMS_TYPE                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_PORT_PARAMS request +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM: Invalid PortID.                        +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetPortParams(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE      pPortParams;
    LSA_UINT32                              Idx;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetPortParams(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pPortParams = (EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPortParams, LSA_NULL) ) ||
         ( (pPortParams->PortID == 0) || (pPortParams->PortID > pDDB->pGlob->HWParams.Caps.PortCnt))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetPortParams: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Idx = pPortParams->PortID;

        /* EDDS does not support this values yet. using 0 */
        pPortParams->LineDelayInTicksMeasured  = pDDB->pGlob->LinkStatus[Idx].LineDelay;
        pPortParams->CableDelayInNsMeasured    = pDDB->pGlob->LinkStatus[Idx].CableDelayNs;
        pPortParams->MACAddr                   = pDDB->pGlob->HWParams.MACAddressPort[Idx-1];
        pPortParams->PortStatus                = pDDB->pGlob->LinkStatus[Idx].PortStatus;
#ifdef LLIF_CFG_SWITCH_SUPPORT
  #ifndef EDDS_DONT_USE_MRP
      pPortParams->MRPRingPort		   = pDDB->SWI.MRPRingPort[Idx-1];
  #else
        pPortParams->MRPRingPort			   = EDD_MRP_NO_RING_PORT;
  #endif
#else
        pPortParams->MRPRingPort			   = EDD_MRP_NO_RING_PORT;
#endif
        pPortParams->MediaType                 = pDDB->pGlob->LinkStatus[Idx].MediaType;
        pPortParams->IRTPortStatus             = EDD_IRT_NOT_SUPPORTED;
        pPortParams->RTClass2_PortStatus       = EDD_RTCLASS2_NOT_SUPPORTED;
        pPortParams->LinkSpeedModeConfigured   = pDDB->pGlob->HWParams.LinkSpeedMode[Idx-1];
        pPortParams->AutonegCapAdvertised      = pDDB->pGlob->LinkStatus[Idx].AutonegCapAdvertised;
        pPortParams->RsvIntervalOrange.Rx.BeginNs     = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Rx.EndNs       = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Tx.BeginNs     = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalOrange.Tx.EndNs       = 0; /* EDDS dont support Orange-Phase (RT-Class2) */
        pPortParams->RsvIntervalRed.Rx.BeginNs        = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.EndNs          = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.EndLocalNs     = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Rx.LastLocalFrameLen  = 0; /* EDDS dont support Red -Phase (RT-Class3) */
        pPortParams->RsvIntervalRed.Tx.BeginNs        = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.EndNs          = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.EndLocalNs     = 0; /* EDDS dont support Red  -Phase (RT-Class3)  */
        pPortParams->RsvIntervalRed.Tx.LastLocalFrameLen  = 0; /* EDDS dont support Red -Phase (RT-Class3) */
        pPortParams->IsWireless                       = pDDB->pGlob->HWParams.IsWireless[Idx-1];
        pPortParams->IsMDIX                           = LSA_FALSE;
        pPortParams->ShortPreamble						= LSA_FALSE;	/* not supported */
        pPortParams->TxFragmentation					= LSA_FALSE;	/* not supported */
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetPortParams(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLinkIndProvide                  +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_LINK_STATUS_IND_PROVIDE.+*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE                  +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam:                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function puts a MAC-Link-Status-indication resource  +*/
/*+               to the EDDS. This RQB remains within EDDS till it detects +*/
/*+               a change in Link-Status or the Channel will be closed.    +*/
/*+                                                                         +*/
/*+               If a link-status changed is detected, the RQB will be     +*/
/*+               used for indication of the event and pParam will be filled+*/
/*+               with the current link-status                              +*/
/*+                                                                         +*/
/*+               If the channel is closed, the RQB will be canceled.       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLinkIndProvide(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    LSA_BOOL                                   QueueRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pRQBParam;
    LSA_UINT32                                 Idx;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLinkIndProvide(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK;
    QueueRQB = LSA_TRUE;

    pRQBParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    /* PortID has to be EDD_PORT_ID_AUTO or 1..EDDS_MAX_PORT_CNT to be valid */

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) ) ||
         ( (pRQBParam->PortID != EDD_PORT_ID_AUTO) && ( (pRQBParam->PortID == 0) || (pRQBParam->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestLinkIndProvide: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Idx = pRQBParam->PortID;

        if (( pRQBParam->Status != pDDB->pGlob->LinkStatus[Idx].Status    ) ||
            ( pRQBParam->Mode   != pDDB->pGlob->LinkStatus[Idx].Mode      ) ||
            ( pRQBParam->Speed  != pDDB->pGlob->LinkStatus[Idx].Speed     ))
        {

            EDDS_SetupLinkParams(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);

            QueueRQB = LSA_FALSE;

        }

        if ( QueueRQB )
        {
            /* Queue the request within handle-management.  */

            EDDS_RQB_PUT_BLOCK_TOP( pHDB->LinkIndReq[Idx].pBottom,
                                    pHDB->LinkIndReq[Idx].pTop,
                                    pRQB);
        }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLinkIndProvide(), Status: 0x%X",
                           Status);


    //pDDB = LSA_NULL; /* not used yet. to prevent compiler warning */

  return(Status);

}
//lint +esym(613,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLinkIndProvideExt               +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_LINK_STATUS_IND_PROVIDE.+*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE                  +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam:                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function puts a MAC-Link-Status-indication resource  +*/
/*+               to the EDDS. This RQB remains within EDDS till it detects +*/
/*+               a change in Link-Status or the Channel will be closed.    +*/
/*+                                                                         +*/
/*+               If a link-status changed is detected, the RQB will be     +*/
/*+               used for indication of the event and pParam will be filled+*/
/*+               with the current link-status                              +*/
/*+                                                                         +*/
/*+               If the channel is closed, the RQB will be canceled.       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLinkIndProvideExt(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    LSA_BOOL                                   QueueRQB;
    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pRQBParam;
    LSA_UINT32                                 Idx;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLinkIndProvideExt(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK;
    QueueRQB = LSA_TRUE;

    pRQBParam = (EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    /* PortID has to be 1..EDDS_MAX_PORT_CNT to be valid */

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) ) ||
         ( ( (pRQBParam->PortID == 0) || (pRQBParam->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestLinkIndProvideExt: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Idx = pRQBParam->PortID;

        /* note: maybe memcmp(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
         *       --> LinkStatus should be changed from EDDS_LINK_STAT_TYPE
         *           to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE ???
         */
        if (( pRQBParam->Status  != pDDB->pGlob->LinkStatus[Idx].Status    ) ||
            ( pRQBParam->Mode    != pDDB->pGlob->LinkStatus[Idx].Mode      ) ||
            ( pRQBParam->Speed   != pDDB->pGlob->LinkStatus[Idx].Speed     ) ||
            ( pRQBParam->PortRxDelay != 0                                 ) ||
            ( pRQBParam->PortTxDelay != 0                                 ) ||
            ( pRQBParam->PortStatus != pDDB->pGlob->LinkStatus[Idx].PortStatus   ) ||
            ( pRQBParam->PortState  != pDDB->pGlob->LinkStatus[Idx].PortState    ) ||
            ( pRQBParam->PhyStatus  != pDDB->pGlob->LinkStatus[Idx].PhyStatus    ) ||
            ( pRQBParam->IRTPortStatus != EDD_IRT_NOT_SUPPORTED                  ) ||
            ( pRQBParam->SyncId0_TopoOk != LSA_FALSE                             ) ||
            ( pRQBParam->RTClass2_PortStatus != EDD_RTCLASS2_NOT_SUPPORTED       ) ||
            ( pRQBParam->MAUType    != pDDB->pGlob->LinkStatus[Idx].MAUType      ) ||
            ( pRQBParam->MediaType  != pDDB->pGlob->LinkStatus[Idx].MediaType    ) ||
            ( pRQBParam->CableDelayInNsMeasured    != pDDB->pGlob->LinkStatus[Idx].CableDelayNs ) ||
            ( pRQBParam->LineDelayInTicksMeasured  != pDDB->pGlob->LinkStatus[Idx].LineDelay    )
#if 0
            ||
            ( pRQBParam->LineDelayInTicksMeasured  != pDDB->pGlob->LinkStatus[Idx].Autoneg) ||
            ( pRQBParam->ShortPreamble  != pDDB->pGlob->LinkStatus[Idx].ShortPreamble) ||
            ( pRQBParam->TxFragmentation  != pDDB->pGlob->LinkStatus[Idx].TxFragmentation)
#endif
           )
        {

            EDDS_SetupLinkParams(pRQBParam,&pDDB->pGlob->LinkStatus[Idx]);
            EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);

            QueueRQB = LSA_FALSE;

        }

        if ( QueueRQB )
        {

            /* Queue the request within handle-management.  */

            EDDS_RQB_PUT_BLOCK_TOP( pHDB->LinkIndReq[Idx].pBottom,
                                    pHDB->LinkIndReq[Idx].pTop,
                                    pRQB);
        }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLinkIndProvideExt(), Status: 0x%X",
                           Status);

    //pDDB = LSA_NULL; /* not used yet. to prevent compiler warning */

  return(Status);

}
//lint +esym(613,pHDB,pRQB)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetLinkState                    +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_LINK_STATUS         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_LINK_STATUS_PTR_TYPE                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pParam:     filled with link    +*/
/*+                                                     status.             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_LINK_STATUS request.+*/
/*+               I fills the pParam structure with the actual Link status  +*/
/*+               which is stored within Managementstructure.               +*/
/*+               Note that this status is not get from the PHY here. The   +*/
/*+               real PHY-Link-Status is only determined on PHY-Interrupt! +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB,pRQB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB/pRQB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetLinkState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStatus;
    LSA_UINT32                          Idx;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetLinkStatus(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pLinkStatus = (EDD_UPPER_GET_LINK_STATUS_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pLinkStatus, LSA_NULL) ) ||
         ( (pLinkStatus->PortID != EDD_PORT_ID_AUTO) && ( (pLinkStatus->PortID == 0) || (pLinkStatus->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStats: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        Idx = pLinkStatus->PortID;

        pLinkStatus->Mode   = pDDB->pGlob->LinkStatus[Idx].Mode;
        pLinkStatus->Speed  = pDDB->pGlob->LinkStatus[Idx].Speed;
        pLinkStatus->Status = pDDB->pGlob->LinkStatus[Idx].Status;
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetLinkStatus(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetStatistics                   +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_STATISTICS          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_STATISTICS_PTR_TYPE                     +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_STATISTICS_PTR_TYPE    pParam: filled with statistics +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_STATISTICS request. +*/
/*+               It fills the pParam structure with the actual statistics  +*/
/*+                                                                         +*/
/*+               The statistics will be per channel and IF/Port.           +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS 20/11/2014  pHDB is on the stack of EDDS_Request
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatistics(
  EDD_UPPER_GET_STATISTICS_PTR_TYPE const pStats,
    EDDS_LOCAL_HDB_PTR_TYPE const    pHDB)
{

    LSA_RESULT                       		Status;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;


    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetStatistics(pStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pStats,
                           pDDB,
                           pHDB);

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pStats, LSA_NULL) ) ||
         ( pStats->PortID > pDDB->pGlob->HWParams.Caps.PortCnt)
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStatistics: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        EDDS_MEMSET_LOCAL(&pDDB->pGlob->TempStats,0,sizeof(pDDB->pGlob->TempStats));

        /* get hardwaredependend statistics */
        EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_STATS(pDDB: 0x%X, PortID: 0x%X)",pDDB,pStats->PortID);

        /* Note: PortID 0 = interface */
        Status = EDDS_LL_GET_STATS(pDDB,pStats->PortID,&pDDB->pGlob->TempStats);

        EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_STATS(). Status: 0x%X",Status);

        if ( Status == EDD_STS_OK )
        {
            EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset = &pHDB->StatsResetValues[pStats->PortID];
            EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal = &pDDB->pGlob->TempStats;

            /* calculate the statistics depending on reset-values for this Port/IF */
            /* Note: Overflow will not be handled!                                 */

            pStats->sMIB.InDiscards        = pStatsLocal->InDiscards      - pStatsReset->InDiscards;
            pStats->sMIB.InErrors          = pStatsLocal->InErrors        - pStatsReset->InErrors;
            pStats->sMIB.InNUcastPkts      = pStatsLocal->InNUcastPkts    - pStatsReset->InNUcastPkts;
            pStats->sMIB.InOctets          = pStatsLocal->InOctets        - pStatsReset->InOctets;
            pStats->sMIB.InUcastPkts       = pStatsLocal->InUcastPkts     - pStatsReset->InUcastPkts;
            pStats->sMIB.InUnknownProtos   = pStatsLocal->InUnknownProtos - pStatsReset->InUnknownProtos;
            pStats->sMIB.OutDiscards       = pStatsLocal->OutDiscards     - pStatsReset->OutDiscards;
            pStats->sMIB.OutErrors         = pStatsLocal->OutErrors       - pStatsReset->OutErrors;
            pStats->sMIB.OutNUcastPkts     = pStatsLocal->OutNUcastPkts   - pStatsReset->OutNUcastPkts;
            pStats->sMIB.OutOctets         = pStatsLocal->OutOctets       - pStatsReset->OutOctets;
            pStats->sMIB.OutQLen           = pStatsLocal->OutQLen         - pStatsReset->OutQLen;
            pStats->sMIB.OutUcastPkts      = pStatsLocal->OutUcastPkts    - pStatsReset->OutUcastPkts;

            /* if we should reset we store the actual values as Reset-Values */
            if ( pStats->Reset )
            {
                pStatsReset->InDiscards        = pStatsLocal->InDiscards;
                pStatsReset->InErrors          = pStatsLocal->InErrors;
                pStatsReset->InNUcastPkts      = pStatsLocal->InNUcastPkts;
                pStatsReset->InOctets          = pStatsLocal->InOctets;
                pStatsReset->InUcastPkts       = pStatsLocal->InUcastPkts;
                pStatsReset->InUnknownProtos   = pStatsLocal->InUnknownProtos;
                pStatsReset->OutDiscards       = pStatsLocal->OutDiscards;
                pStatsReset->OutErrors         = pStatsLocal->OutErrors;
                pStatsReset->OutNUcastPkts     = pStatsLocal->OutNUcastPkts;
                pStatsReset->OutOctets         = pStatsLocal->OutOctets;
                pStatsReset->OutQLen           = pStatsLocal->OutQLen;
                pStatsReset->OutUcastPkts      = pStatsLocal->OutUcastPkts;
            }
        }

    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetStatistics(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ResetStatistics                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                             LSA_UINT16                  PortID          +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  PortID     : PortID:  0 = interface                                    +*/
/*+                        1..x PortID                                      +*/
/*+                        EDD_STATISTICS_RESET_ALL : all                   +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function reset the statistics of the IF and/or Port. +*/
/*+               This is done by setting the "resetvalue" to the actual    +*/
/*+               values (because the reset is done per if/port and per     +*/
/*+               handle!)                                                  +*/
/*+                                                                         +*/
/*+               Note: PortID must be valid! not checked                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_ResetStatistics(
    EDDS_LOCAL_HDB_PTR_TYPE     const pHDB,
    LSA_UINT16                  const PortID)
{

    LSA_RESULT                          Status;
    LSA_UINT32                          i;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB		= pHDB->pDDB;

    Status = EDD_STS_OK;

    EDDS_CORE_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :EDDS_ResetStatistics()");

    /* loop over all ports and IF */
    i = 0;
    while (( i<=pDDB->pGlob->HWParams.Caps.PortCnt ) && (Status == EDD_STS_OK))
    {
        /* should we reset this port/IF ? */
        if (( i == PortID) || ( EDD_STATISTICS_RESET_ALL == PortID))
        {

            EDDS_MEMSET_LOCAL(&pDDB->pGlob->TempStats,0,sizeof(pDDB->pGlob->TempStats));

            /* get hardwaredependend statistics */
            EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_STATS(pDDB: 0x%X, PortID: 0x%X)",pDDB,i);

            /* Note: PortID 0 = interface */
            Status = EDDS_LL_GET_STATS(pDDB,i,&pDDB->pGlob->TempStats);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_STATS(). Status: 0x%X",Status);

            if ( Status == EDD_STS_OK )
            {
                EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset = &pHDB->StatsResetValues[i];
                EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal = &pDDB->pGlob->TempStats;

                pStatsReset->InDiscards        = pStatsLocal->InDiscards;
                pStatsReset->InErrors          = pStatsLocal->InErrors;
                pStatsReset->InNUcastPkts      = pStatsLocal->InNUcastPkts;
                pStatsReset->InOctets          = pStatsLocal->InOctets;
                pStatsReset->InUcastPkts       = pStatsLocal->InUcastPkts;
                pStatsReset->InUnknownProtos   = pStatsLocal->InUnknownProtos;
                pStatsReset->OutDiscards       = pStatsLocal->OutDiscards;
                pStatsReset->OutErrors         = pStatsLocal->OutErrors;
                pStatsReset->OutNUcastPkts     = pStatsLocal->OutNUcastPkts;
                pStatsReset->OutOctets         = pStatsLocal->OutOctets;
                pStatsReset->OutQLen           = pStatsLocal->OutQLen;
                pStatsReset->OutUcastPkts      = pStatsLocal->OutUcastPkts;
            }
        }
        i++;
    }

    return(Status);

}
//lint +esym(613,pHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetStatisticsAll                +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_STATISTICS_ALL      +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE                 +*/
/*+                                                                         +*/
/*+    RQB-header:                                                          +*/
/*+    LSA_OPCODE_TYPE         opcode:     unchanged                        +*/
/*+    LSA_HANDLE_TYPE         Handle:     unchanged                        +*/
/*+    LSA_USER_ID_TYPE        UserId:     unchanged                        +*/
/*+    LSA_COMP_ID_TYPE        CompId:     unchanged                        +*/
/*+    EDD_SERVICE             Service:    unchanged                        +*/
/*+    LSA_RESULT              Status:     unchanged                        +*/
/*+    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE pParam: filled with statistics +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_GET_STATISTICS_ALL      +*/
/*+               request.                                                  +*/
/*+               It fills the pParam structure with the actual statistics  +*/
/*+                                                                         +*/
/*+               The statistics will be for all IF/Ports.                  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatisticsAll(
  EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE const pStats,
    EDDS_LOCAL_HDB_PTR_TYPE const     pHDB)
{

    LSA_RESULT                              Status;
    LSA_UINT32                              PortID;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetStatisticsAll(pStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pStats,
                           pDDB,
                           pHDB);

    if ( LSA_HOST_PTR_ARE_EQUAL( pStats, LSA_NULL))
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetStatisticsAll: pStats is null!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
    Status = EDD_STS_OK;
    for(PortID=0;PortID <= pDDB->pGlob->HWParams.Caps.PortCnt; ++PortID)
    {
      EDDS_MEMSET_LOCAL(&pDDB->pGlob->TempStats,0,sizeof(pDDB->pGlob->TempStats));

      /* get hardwaredependend statistics */
      EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_STATS(pDDB: 0x%X, PortID: 0x%X)",pDDB,PortID);

      /* Note: PortID 0 = interface */
      Status = EDDS_LL_GET_STATS(pDDB,PortID,&pDDB->pGlob->TempStats);

      EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_STATS(). Status: 0x%X",Status);

      if ( Status == EDD_STS_OK )
      {
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsReset = &pHDB->StatsResetValues[PortID];
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStatsLocal = &pDDB->pGlob->TempStats;

        /* calculate the statistics depending on reset-values for this Port/IF */
        /* Note: Overflow will not be handled!                                 */

        pStats->sMIB[PortID].InDiscards        = pStatsLocal->InDiscards      - pStatsReset->InDiscards;
        pStats->sMIB[PortID].InErrors          = pStatsLocal->InErrors        - pStatsReset->InErrors;
        pStats->sMIB[PortID].InNUcastPkts      = pStatsLocal->InNUcastPkts    - pStatsReset->InNUcastPkts;
        pStats->sMIB[PortID].InOctets          = pStatsLocal->InOctets        - pStatsReset->InOctets;
        pStats->sMIB[PortID].InUcastPkts       = pStatsLocal->InUcastPkts     - pStatsReset->InUcastPkts;
        pStats->sMIB[PortID].InUnknownProtos   = pStatsLocal->InUnknownProtos - pStatsReset->InUnknownProtos;
        pStats->sMIB[PortID].OutDiscards       = pStatsLocal->OutDiscards     - pStatsReset->OutDiscards;
        pStats->sMIB[PortID].OutErrors         = pStatsLocal->OutErrors       - pStatsReset->OutErrors;
        pStats->sMIB[PortID].OutNUcastPkts     = pStatsLocal->OutNUcastPkts   - pStatsReset->OutNUcastPkts;
        pStats->sMIB[PortID].OutOctets         = pStatsLocal->OutOctets       - pStatsReset->OutOctets;
        pStats->sMIB[PortID].OutQLen           = pStatsLocal->OutQLen         - pStatsReset->OutQLen;
        pStats->sMIB[PortID].OutUcastPkts      = pStatsLocal->OutUcastPkts    - pStatsReset->OutUcastPkts;

        /* if we should reset we store the actual values as Reset-Values */
        if ( pStats->Reset )
        {
          pStatsReset->InDiscards        = pStatsLocal->InDiscards;
          pStatsReset->InErrors          = pStatsLocal->InErrors;
          pStatsReset->InNUcastPkts      = pStatsLocal->InNUcastPkts;
          pStatsReset->InOctets          = pStatsLocal->InOctets;
          pStatsReset->InUcastPkts       = pStatsLocal->InUcastPkts;
          pStatsReset->InUnknownProtos   = pStatsLocal->InUnknownProtos;
          pStatsReset->OutDiscards       = pStatsLocal->OutDiscards;
          pStatsReset->OutErrors         = pStatsLocal->OutErrors;
          pStatsReset->OutNUcastPkts     = pStatsLocal->OutNUcastPkts;
          pStatsReset->OutOctets         = pStatsLocal->OutOctets;
          pStatsReset->OutQLen           = pStatsLocal->OutQLen;
          pStatsReset->OutUcastPkts      = pStatsLocal->OutUcastPkts;
        }
      }
    }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetStatisticsAll(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestResetStatistics                 +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_RESET_STATISTICS        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_RESET_STATISTICS_PTR_TYPE                   +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_RESET_STATISTICS_PTR_TYPE   pParam: filled with statistics+*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_RESET_STATISTICS request+*/
/*+               It will reset the statistics of the IF and/or Port. This  +*/
/*+               is done by setting the "resetvalue" to the actual values  +*/
/*+               (because the reset is done per if/port and per handle!)   +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 10/11/2014 pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
//AD_DISCUSS pHDB checked within EDDS_Request
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestResetStatistics(
  EDD_UPPER_RESET_STATISTICS_PTR_TYPE    const pResStats,
    EDDS_LOCAL_HDB_PTR_TYPE     const pHDB)
{

    LSA_RESULT                            	Status;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestResetStatistics(pResStats: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pResStats,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pResStats, LSA_NULL) ) ||
         (( pResStats->PortID > pDDB->pGlob->HWParams.Caps.PortCnt) && ( pResStats->PortID != EDD_STATISTICS_RESET_ALL ))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestResetStatistics: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        Status = EDDS_ResetStatistics(pHDB,pResStats->PortID);
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestResetStatistics(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSetIP                           +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SET_IP                  +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_SET_IP_PTR_TYPE                             +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_SET_IP_PTR_TYPE pParam:     Local IP Address to use       +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SET_IP request          +*/
/*+               It stores the spezified local IP address for use with     +*/
/*+               xRToverUDP.                                               +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetIP(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    EDD_UPPER_SET_IP_PTR_TYPE       pIP;
    EDD_IP_ADR_TYPE                 IPAddress = {{0,0,0,0}};
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSetIP(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pIP = (EDD_UPPER_SET_IP_PTR_TYPE) pRQB->pParam;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pIP, LSA_NULL) )
    {
        IPAddress = pIP->LocalIP;

        /* only one access, so we dont conflict with receive-isr, accessing this */
        /* IP-Address                                                            */

        pDDB->pGlob->HWParams.IPAddress = IPAddress;
    }
    else
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetIP: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSetIP(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestGetParams                       +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_GET_PARAMS              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_GET_PARAMS_PTR_TYPE                         +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_GET_PARAMS_PTR_TYPE pParam:     filled with parameters    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDDS_SRV_GET_PARAMSS request    +*/
/*+               It fills the pParam structure with all parameters of the  +*/
/*+               Ethernetcontroller.                                       +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetParams(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{
    LSA_UINT						i;
    LSA_RESULT                      Status;
    EDD_UPPER_GET_PARAMS_PTR_TYPE   pParams;
    EDDS_LOCAL_DDB_PTR_TYPE			pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestGetParams(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pParams = (EDD_UPPER_GET_PARAMS_PTR_TYPE) pRQB->pParam;

  if ( ! LSA_HOST_PTR_ARE_EQUAL(pParams, LSA_NULL) )
  {
    pParams->InterfaceID						= pDDB->pGlob->HWParams.InterfaceID;
    //lint -save -e835
    //JB 11/11/2014 we use #define's that are initialized with zero on purpose //#JB_TODO re-structure - do not user -save!
    pParams->HardwareType						= EDD_HW_TYPE_USED_STANDARD_MAC |
        EDD_HW_TYPE_FEATURE_NO_TIMESTAMP_SUPPORT |
        EDD_HW_TYPE_FEATURE_NO_DELAY_SUPPORT |
        EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_12_SUPPORT |
#ifdef LLIF_CFG_SWITCH_CT_SUPPORT
                EDD_HW_TYPE_FEATURE_CT_SUPPORT |
#else
        EDD_HW_TYPE_FEATURE_NO_CT_SUPPORT |
#endif
        EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_3_SUPPORT |
        EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_1_UDP_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE |
                EDD_HW_TYPE_FEATURE_SYNC_SLAVE_NO_SUPPORT  |
                EDD_HW_TYPE_FEATURE_SYNC_MASTER_NO_SUPPORT |
                EDD_HW_TYPE_FEATURE_SYNC_DELAY_NO_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT |
                EDD_HW_TYPE_FEATURE_NO_FRAGMENTATION_SUPPORT;
    //lint -restore

    pParams->TraceIdx							= pDDB->pGlob->TraceIdx;
    pParams->TimeResolutionInNs					= 0 ; /* not supported */
    pParams->MACAddr							= pDDB->pGlob->HWParams.MACAddress;

      for (i=0; i<EDD_IP_ADDR_SIZE; i++) pParams->LocalIP.IPAdr[i] = pDDB->pGlob->HWParams.IPAddress.IPAdr[i];

    pParams->MaxPortCnt							= (LSA_UINT16) pDDB->pGlob->HWParams.Caps.PortCnt;
    pParams->MaxPortCntOfAllEDD					= pDDB->pGlob->MaxPortCntOfAllEDD;
    pParams->MaxInterfaceCntOfAllEDD			= pDDB->pGlob->MaxInterfaceCntOfAllEDD;
#if defined(LLIF_CFG_SWITCH_SUPPORT) && !defined(EDDS_DONT_USE_MRP)
      pParams->MaxMRP_Instances         = pDDB->SWI.MaxMRP_Instances;
      pParams->MRPDefaultRoleInstance0  = pDDB->SWI.MRPDefaultRoleInstance0;
      pParams->MRPSupportedRole         = pDDB->SWI.MRPSupportedRole;
      pParams->MRPSupportedMultipleRole = pDDB->SWI.MRPSupportedMultipleRole;

#else
      pParams->MaxMRP_Instances         = 0;
      pParams->MRPDefaultRoleInstance0  = EDD_MRP_ROLE_NONE;
      pParams->MRPSupportedRole         = 0;
      pParams->MRPSupportedMultipleRole = 0;
#endif
#ifdef EDDS_CFG_SRT_INCLUDE
    pParams->CycleBaseFactor					= pDDB->pSRT->CSRT.Cfg.CycleBaseFactor;
    pParams->ConsumerFrameIDBaseClass3			= 0;
    pParams->ConsumerCntClass3					= 0;
    /**@note ConsumerFrameIDBaseClass1 vs. ConsumerFrameIDBaseClass2 */
    pParams->ConsumerFrameIDBaseClass2			= pDDB->pSRT->CSRT.Cons.ConsumerFrameIDBase1;
    pParams->ConsumerCntClass2					= pDDB->pSRT->CSRT.Cfg.ConsumerCntClass2;
    pParams->ConsumerFrameIDBaseClass1			= pDDB->pSRT->CSRT.Cons.ConsumerFrameIDBase2;
    pParams->ConsumerCntClass1					= pDDB->pSRT->CSRT.Cfg.ConsumerCntClass1;
    pParams->ConsumerCntClass12Combined			= LSA_FALSE;
    pParams->ProviderCnt						= pDDB->pSRT->CSRT.Cfg.ProviderCnt;
    pParams->ProviderCntClass3					= 0;
    pParams->ProviderGroupCnt					= EDD_CFG_CSRT_MAX_PROVIDER_GROUP;
    pParams->DataLenMin							= EDD_CSRT_DATALEN_MIN;
    pParams->DataLenMax							= EDD_CSRT_DATALEN_MAX;
    pParams->DataLenMinUDP						= EDD_CSRT_UDP_DATALEN_MIN;
    pParams->DataLenMaxUDP						= EDD_CSRT_UDP_DATALEN_MAX;

    pParams->ProviderCyclePhaseMaxByteCnt		= (32 > pParams->CycleBaseFactor) ? 32 : pDDB->pSRT->CSRT.Cfg.CycleBaseFactor;
    pParams->ProviderCyclePhaseMaxByteCnt *= 6250;
    pParams->ProviderCyclePhaseMaxByteCnt /= 32;

    pParams->ProviderCyclePhaseMaxCnt			= (pDDB->pSRT->CSRT.Cfg.ProviderCyclePhaseMaxCnt1ms*pParams->CycleBaseFactor)/32;

#else
    pParams->CycleBaseFactor				 	  = 32;	/* fixed */
    pParams->ConsumerFrameIDBaseClass3		 	  = 0;
    pParams->ConsumerCntClass3				 	  = 0;
    /**@note ConsumerFrameIDBaseClass1 vs. ConsumerFrameIDBaseClass2 */
    pParams->ConsumerFrameIDBaseClass2			  = 0;
    pParams->ConsumerCntClass2					  = 0;
    pParams->ConsumerFrameIDBaseClass1			  = 0;
    pParams->ConsumerCntClass1					  = 0;
    pParams->ConsumerCntClass12Combined			  = LSA_FALSE;
    pParams->ProviderCnt						  = 0;
    pParams->ProviderCntClass3					  = 0;
    pParams->ProviderGroupCnt					  = 0;
    pParams->DataLenMin							  = 0;
    pParams->DataLenMax							  = 0;
    pParams->DataLenMinUDP						  = 0;
    pParams->DataLenMaxUDP						  = 0;
    pParams->ProviderCyclePhaseMaxByteCnt		  = 0;
    pParams->ProviderCyclePhaseMaxCnt			  = 0;

#endif /* xxx */
    /* KRAM not present. Set to 0 */
    pParams->KRAMInfo.KRAMBaseAddr                 = 0;
    pParams->KRAMInfo.offset_ProcessImageStart     = 0;
    pParams->KRAMInfo.offset_async_in_start        = 0;
    pParams->KRAMInfo.size_async_in                = 0;
    pParams->KRAMInfo.offset_async_out_start       = 0;
    pParams->KRAMInfo.size_async_out               = 0;
    pParams->KRAMInfo.offset_ProcessImageEnd       = 0;

    pParams->RsvIntervalRed.RxEndNs                = 0;
    pParams->RsvIntervalRed.RxLocalTransferEndNs   = 0;
    pParams->RsvIntervalRed.TxEndNs                = 0;
    pParams->RsvIntervalRed.TxLocalTransferEndNs   = 0;
    pParams->RsvIntervalRed.TxLocalTransferStartNs = 0;
  }
  else
  {
    EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestGetParams: RQB->pParam is NULL!");
    Status = EDD_STS_ERR_PARAM;
  }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestGetParams(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestRxOverloadIndProvide            +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_RX_OVERLOAD_IND_PROVIDE +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE                  +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE  pParam:                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function puts a RX-Overload indication resource      +*/
/*+               to the EDDS. This RQB remains within EDDS till it detects +*/
/*+               a change RX Overload change or the Channel will be closed.+*/
/*+                                                                         +*/
/*+               If a Rx Overload change is detected, the RQB will be      +*/
/*+               used for indication of the event and pParam will be filled+*/
/*+               with the current state. On entry the actual state is      +*/
/*+               compared to the input from pParams. If they differ the    +*/
/*+               request will be finised with the actual state.            +*/
/*+                                                                         +*/
/*+               If the channel is closed, the RQB will be canceled.       +*/
/*+                                                                         +*/
/*+               Only one request per channel is allowed at a time. Further+*/
/*+               requests will get EDD_STS_ERR_SEQUENCE.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestRxOverloadIndProvide(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 	Status;
    EDD_UPPER_RX_OVERLOAD_IND_PROVIDE_PTR_TYPE	pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE						pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestRxOverloadIndProvide(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status   = EDD_STS_OK;

    pRQBParam = (EDD_UPPER_RX_OVERLOAD_IND_PROVIDE_PTR_TYPE) pRQB->pParam;

    if ( LSA_HOST_PTR_ARE_EQUAL( pRQBParam, LSA_NULL) )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestRxOverloadIndProvide: RQB->pParam is NULL!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        if ( LSA_HOST_PTR_ARE_EQUAL( pHDB->RxOverloadIndReq, LSA_NULL))
        {
            /* check if we have a different state. If so indicate yet */
#if !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
            if ( EDD_NO_RX_OVERLOAD != pRQBParam->Status )
#else  /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
            if ( pRQBParam->Status != pDDB->pGlob->RxOverload.Status )
#endif /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
            {

//                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
//                                    "EDDS_RxOverloadIndicateEvent: Indicating Rx-overload event. Status: 0x%X",
//#if !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
//                                    EDD_NO_RX_OVERLOAD);
//                pRQBParam->Status = EDD_NO_RX_OVERLOAD;
//#else  /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
//                pDDB->pGlob->RxOverload.Status);
//                pRQBParam->Status = pDDB->pGlob->RxOverload.Status;
//#endif /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */

#if !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                                    "EDDS_RxOverloadIndicateEvent: Indicating Rx-overload event. Status: 0x%X",
                  EDD_NO_RX_OVERLOAD);
        pRQBParam->Status = EDD_NO_RX_OVERLOAD;
#else /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                                    "EDDS_RxOverloadIndicateEvent: Indicating Rx-overload event. Status: 0x%X",
                  pDDB->pGlob->RxOverload.Status);
        pRQBParam->Status = pDDB->pGlob->RxOverload.Status;
#endif /* !defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */

                EDDS_RequestFinish(pHDB,pRQB,EDD_STS_OK);
            }
            else
            {
                /* store RQB for later indication */
                pHDB->RxOverloadIndReq = pRQB;
            }
        }
        else
        {
            Status = EDD_STS_ERR_SEQUENCE;
        }
    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestRxOverloadIndProvide(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSetupPHY                        +*/
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
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEBUG_SETUP_PHY        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_SETUP_PHY_TYPE                               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDDS_UPPER_SETUP_PHY_PTR_TYPE pParam:  points to PHY parameters     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SETUP_PHY request.      +*/
/*+               We setup the PHY with the given parameters.               +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetupPHY(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDDS_UPPER_SETUP_PHY_PTR_TYPE       pPHYParams;
    LSA_UINT8                           Autoneg;
    LSA_UINT8                           PhyStatus;
    EDDS_LOCAL_DDB_PTR_TYPE				pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSetupPHY(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pPHYParams = (EDDS_UPPER_SETUP_PHY_PTR_TYPE) pRQB->pParam;

    if ( ( LSA_HOST_PTR_ARE_EQUAL( pPHYParams, LSA_NULL) ) ||
         ( (pPHYParams->PortID == 0) || (pPHYParams->PortID > pDDB->pGlob->HWParams.Caps.PortCnt))
       )
    {
        EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetupPHY: RQB parameter error!");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* request only allowed if Port not already configured by PRM ! */
        if ( ! pDDB->pGlob->Prm.PRMUsed )
        {
            EDDS_LOWER_TRACE_04(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, PortID: %d, LSP: 0x%X, Power: 0x%X)",
                                pDDB,
                                pPHYParams->PortID,
                                pPHYParams->LinkSpeedMode,
                                pPHYParams->PHYPower);

            Status = EDDS_LL_SET_LINK_STATE(pDDB,pPHYParams->PortID,pPHYParams->LinkSpeedMode,pPHYParams->PHYPower);

            if ( Status == EDD_STS_OK )
            {
                EDD_RQB_GET_LINK_STATUS_TYPE LinkStatus;
                LSA_UINT16                   MAUType;
                LSA_UINT32                   PortStatus, AutonegCapAdvertised;
                LSA_UINT8                    LinkSpeedModeConfigured, MediaType;

                EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,pPHYParams->PortID);

                /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. A "AutoNeg"-configuration (=default) maybe mapped to a fixed */
                /*       setting if autoneg is not supported. Typically the case with POF ports because autoneg is not supported with POF.     */
                /*       The actual autoneg to fixed mapping depends on phy adaption by LL.                                                    */
                /*       All other parameters from EDDS_LL_GET_LINK_STATE are not used here!                                                   */
                Status = EDDS_LL_GET_LINK_STATE(pDDB, pPHYParams->PortID, &LinkStatus,&MAUType,&MediaType,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);

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

                pDDB->pGlob->HWParams.LinkSpeedMode[pPHYParams->PortID-1] = LinkSpeedModeConfigured;
                pDDB->pGlob->HWParams.PHYPower[pPHYParams->PortID-1]      = pPHYParams->PHYPower;

                /* Setup PhyStatus */
                if ( pDDB->pGlob->HWParams.PHYPower[pPHYParams->PortID-1] == EDDS_PHY_POWER_ON )
                {
                    PhyStatus = EDD_PHY_STATUS_ENABLED;
                }
                else
                {
                    PhyStatus = EDD_PHY_STATUS_DISABLED;
                }

                Autoneg = EDD_AUTONEG_OFF;
                if ( LinkSpeedModeConfigured == EDD_LINK_AUTONEG ) Autoneg = EDD_AUTONEG_ON;

                if ((pDDB->pGlob->LinkStatus[pPHYParams->PortID].Autoneg != Autoneg    ) || /* change ? */
                    (pDDB->pGlob->LinkStatus[pPHYParams->PortID].PhyStatus != PhyStatus))
                {
                    /* Store configured Autoneg setting */
                    pDDB->pGlob->LinkStatus[pPHYParams->PortID].Autoneg = Autoneg;
                    pDDB->pGlob->LinkStatus[pPHYParams->PortID].PhyStatus = PhyStatus;
                    EDDS_IndicateExtLinkChange(pDDB,pPHYParams->PortID);
                }
            }
        }
        else
        {
            EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSetupPHY: Sequence error!");
            Status = EDD_STS_ERR_SEQUENCE;
        }

    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSetupPHY(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)
#ifdef EDDS_CFG_LED_BLINK_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestLEDBlink                        +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SETUP_PHY               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_SETUP_PHY_TYPE                               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+                                                                         +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     unchanged                       +*/
/*+     LSA_HANDLE_TYPE         Handle:     unchanged                       +*/
/*+     LSA_USER_ID_TYPE        UserId:     unchanged                       +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     unchanged                       +*/
/*+     EDDS_UPPER_SETUP_PHY_PTR_TYPE pParam:  points to PHY parameters     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_LED_BLINK request.      +*/
/*+               We setup blink all LEDs and then switch them back to      +*/
/*+               the original mode.                                        +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestLEDBlink(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                          Status;
    EDDS_LED_BLINK_PTR_TYPE             pLED;
    LSA_UINT32                          i;
    LSA_USER_ID_TYPE                    Dummy;
    LSA_UINT16                          RetVal;
    EDDS_LOCAL_DDB_PTR_TYPE     		pDDB		= pHDB->pDDB;
    LSA_UINT16                const     OnOffDurationIn500msTicks = 1;
    LSA_UINT16                const     TotalBlinkDurationInSeconds = 3;

    LSA_UNUSED_ARG(pRQB);

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestLEDBlink(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pLED       = &pDDB->pGlob->LEDBLink;

    {
        pLED->OnOffDurationIn500msTicks   = OnOffDurationIn500msTicks;

        /* we will start or retrigger LED Blink            */
        /* if we already running, we only set the new time */

        if ( pLED->Status == EDDS_LED_BLINK_INACTIVE )
        {

            pLED->Status = EDDS_LED_BLINK_ACTIVE;
            pLED->RemainTotal = (TotalBlinkDurationInSeconds * 2)-1; /* in 500ms ticks */
            pLED->Remain      = pLED->OnOffDurationIn500msTicks;                 /* in 500ms ticks */
            pLED->LEDOn       = LSA_TRUE; /* start with LED on */

            /* turn LEDs on all ports */

            for (i=1; i<=pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
              LSA_BOOL LEDOn = LSA_TRUE;

                EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                    "==> EDDS_LL_LED_BACKUP_MODE(pDDB: 0x%X, Port: %d)",
                                    pDDB,
                                    i);

                Status = EDDS_LL_LED_BACKUP_MODE(pDDB,i);

                EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_LED_BACKUP_MODE(). Status: 0x%X",Status);

                if(EDD_STS_OK == Status)
                {

          EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                    "==> EDDS_LL_LED_SET_MODE(pDDB: 0x%X, Port: %d = %d)",
                    pDDB,
                    i,
                    LEDOn);

          Status = EDDS_LL_LED_SET_MODE(pDDB,i,LEDOn);

          EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_LED_SET_MODE(). Status: 0x%X",Status);
                }

                if ( Status != EDD_STS_OK )
                {
                    /* we ignore errors and continue */
                    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                          "EDDS_LL_LED_SET_MODE. failed (Status: 0x%X)",
                                          Status );

                    Status = EDD_STS_OK;
                }
            }

            /*---------------------------------------------------------------*/
            /* start LED Blink Timer                                         */
            /*---------------------------------------------------------------*/

            Dummy.uvar32 = pDDB->pGlob->LEDBLink.Timer.EDDSTimerID;

            EDDS_START_TIMER(&RetVal,
                             pDDB->pGlob->LEDBLink.Timer.TimerID,
                             Dummy,
                             5);  /* 500ms timer */

            if ( RetVal != LSA_RET_ERR_PARAM )
            {
                pDDB->pGlob->LEDBLink.Timer.InProgress = LSA_TRUE;
            }
            else
            {
                EDDS_FatalError(EDDS_FATAL_ERR_TIMER_START,
                                EDDS_MODULE_ID,
                                2);
            }

        }
        else /* active */
        {
            /* if we got a smaller period we may reset Remain to the new one */
            if ( pLED->Remain > pLED->OnOffDurationIn500msTicks  )
            {
                pLED->Remain = pLED->OnOffDurationIn500msTicks;
            }

            pLED->RemainTotal = (TotalBlinkDurationInSeconds * 2)-1;/* in 500ms ticks */
        }

    }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestLEDBlink(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GeneralRequest                         +*/
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
/*+     EDD_SERVICE             Service:    EDDS_SRV_GET_STATS              +*/
/*+                                         EDD_SRV_GET_LINK_STATUS         +*/
/*+                                         EDD_SRV_MULTICAST               +*/
/*+                                         EDD_SRV_PRM_READ                +*/
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
//JB 10/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_GeneralRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    LSA_BOOL                        Indicate;
    LSA_UINT32						TraceIdx = pHDB->pDDB->pGlob->TraceIdx;

    LSA_UNUSED_ARG(TraceIdx);

    EDDS_CORE_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_GeneralRequest(pRQB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pHDB);

    Indicate = LSA_TRUE;

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_GET_LINK_STATUS:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_LINK_STATUS (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestGetLinkState(pRQB,pHDB);
            break;
        case EDD_SRV_LINK_STATUS_IND_PROVIDE:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LINK_STATUS_IND_PROVIDE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestLinkIndProvide(pRQB,pHDB);
            if ( Status == EDD_STS_OK ) Indicate = LSA_FALSE;
            break;
        case EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestLinkIndProvideExt(pRQB,pHDB);
            if ( Status == EDD_STS_OK ) Indicate = LSA_FALSE;
            break;
        case EDD_SRV_MULTICAST:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_MULTICAST (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestMC(pRQB,pHDB);
            break;
        case EDD_SRV_GET_PARAMS:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_PARAMS (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestGetParams(pRQB,pHDB);
            break;
        case EDD_SRV_SET_IP:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_IP (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestSetIP(pRQB,pHDB);
            break;
        case EDD_SRV_SET_LINE_DELAY:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_LINE_DELAY (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestSetLineDelay(pRQB,pHDB);
            break;
        case EDD_SRV_GET_PORT_PARAMS:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_PORT_PARAMS (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestGetPortParams(pRQB,pHDB);
            break;
        case EDD_SRV_MULTICAST_FWD_CTRL:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_MULTICAST_FWD_CTRL (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestMCFwdCtrl(pRQB,pHDB);
            break;
        case EDD_SRV_RX_OVERLOAD_IND_PROVIDE:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_RX_OVERLOAD_IND_PROVIDE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestRxOverloadIndProvide(pRQB,pHDB);
            if ( Status == EDD_STS_OK ) Indicate = LSA_FALSE;
            break;
            #if 0
        case EDD_SRV_SETUP_PHY:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SETUP_PHY (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestSetupPHY(pRQB,pHDB);
            break;
            #endif

        case EDD_SRV_SENDCLOCK_CHANGE:
            #ifdef EDDS_CFG_SRT_INCLUDE
        EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SENDCLOCK_CHANGE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
        Status = EDDS_RTSendclockChange(pRQB,pHDB);
      #else
        if((32 == ((EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam)->CycleBaseFactor) || (0 == ((EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE) pRQB->pParam)->CycleBaseFactor))
        {
          Status = EDD_STS_OK;
        }else
        {
          Status = EDD_STS_ERR_PARAM;
        }
            #endif
      break;
            #ifdef EDDS_CFG_LED_BLINK_SUPPORT
        case EDD_SRV_LED_BLINK:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_LED_BLINK (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestLEDBlink(pRQB,pHDB);
            break;
            #endif
        case EDD_SRV_SET_SYNC_STATE:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_SYNC_STATE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            /* no action. just accept */
            Status = EDD_STS_OK;
            break;
        case EDD_SRV_SET_REMOTE_PORT_STATE:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SET_REMOTE_PORT_STATE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            /* no action. just accept */
            Status = EDD_STS_OK;
            break;
        case EDD_SRV_GET_STATISTICS:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_STATISTICS (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestGetStatistics((EDD_UPPER_GET_STATISTICS_PTR_TYPE) pRQB->pParam,pHDB);
            break;
        case EDD_SRV_RESET_STATISTICS:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_RESET_STATISTICS (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestResetStatistics((EDD_UPPER_RESET_STATISTICS_PTR_TYPE) pRQB->pParam,pHDB);
            break;
        case EDD_SRV_GET_STATISTICS_ALL:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_GET_STATISTICS_ALL (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestGetStatisticsAll((EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE)pRQB->pParam,pHDB);
            break;
        default:
            EDDS_UPPER_TRACE_02(TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_GeneralRequest: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }


    if (Indicate) EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_CORE_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_GeneralRequest(), Status: 0x%X",
                           Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DebugRequest                           +*/
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
/*+     EDD_SERVICE             Service:    EDDS_SRV_GET_DEBUG_STATS        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all EDDS Debug  (Component          +*/
/*+               independent) requests.                                    +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 11/11/2014 pRQB is checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRQB or pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DebugRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                         		Status;
    EDDS_UPPER_GET_DEBUG_STATS_PTR_TYPE		pParams;
    EDDS_LOCAL_DDB_PTR_TYPE					pDDB		= pHDB->pDDB;

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DebugRequest(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
            case EDDS_SRV_GET_DEBUG_STATS:

                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDDS_SRV_GET_DEBUG_STATS (Handle: 0x%X)",EDD_RQB_GET_HANDLE(pRQB));

                pParams = (EDDS_UPPER_GET_DEBUG_STATS_PTR_TYPE) pRQB->pParam;

                EDDS_MEMSET_UPPER(pParams,0,sizeof(EDDS_RQB_GET_DEBUG_STATS_TYPE));

                pParams->Glob.RxXRToUDPIPWrongChecksum = pDDB->pGlob->RxStats.RxXRToUDPIPWrongChecksum;
                pParams->Glob.RxXRToUDPWrongHeader     = pDDB->pGlob->RxStats.RxXRToUDPWrongHeader;
                pParams->Glob.TxResources              = pDDB->pGlob->Errors.TxResource;
                pParams->Glob.RxOverloadCnt            = pDDB->pGlob->Errors.RxOverloadCnt;

                #ifdef EDDS_CFG_NRT_INCLUDE
                pParams->NRT.RxARPIPSD                 = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_IP_IDX];
                pParams->NRT.RxASRTOtherSD             = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_ASRT_OTHER_IDX];
                pParams->NRT.RxASRTAlarmSD             = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_ASRT_ALARM_IDX];
                pParams->NRT.RxOtherSD                 = pDDB->pNRT->Stats.RxSilentDiscarded[EDDS_NRT_BUFFER_OTHER_IDX];

                pParams->NRT.RxARPIP_Drop              = pDDB->pNRT->Stats.RxARPIP_Drop;
                pParams->NRT.RxASRTAlarm_Drop          = pDDB->pNRT->Stats.RxASRTAlarm_Drop;
                pParams->NRT.RxLenDCP_Drop             = pDDB->pNRT->Stats.RxLenDCP_Drop;
                pParams->NRT.RxLenOther_Drop           = pDDB->pNRT->Stats.RxLenOther_Drop;
                pParams->NRT.RxXRTDCP_Drop             = pDDB->pNRT->Stats.RxXRTDCP_Drop;
                pParams->NRT.RxLLDP_Drop               = pDDB->pNRT->Stats.RxLLDP_Drop;
                pParams->NRT.RxPTP_Drop                = pDDB->pNRT->Stats.RxPTP_Drop;
                pParams->NRT.RxMRP_Drop                = pDDB->pNRT->Stats.RxMRP_Drop;
                pParams->NRT.RxLenTSYNC_Drop           = pDDB->pNRT->Stats.RxLenTSYNC_Drop;
                pParams->NRT.RxOther_Drop              = pDDB->pNRT->Stats.RxOther_Drop;
                pParams->NRT.RxUDP_XRTDCP_Drop         = pDDB->pNRT->Stats.RxUDP_XRTDCP_Drop;
                pParams->NRT.RxUDP_ASRTAlarm_Drop      = pDDB->pNRT->Stats.RxUDP_ASRTAlarm_Drop;

                pParams->NRT.RxDCPFilterDrop           = pDDB->pNRT->Stats.RxDCPFilterDrop;
                pParams->NRT.RxDCPFilterInvalidFrame   = pDDB->pNRT->Stats.RxDCPFilterInvalidFrame;

                #endif

                #ifdef EDDS_CFG_SRT_INCLUDE
            pParams->SRT.ConsCycleCounterDropCnt   = pDDB->pSRT->CSRT.Stats.ConsCycleCounterDropCnt;
            pParams->SRT.ConsCycleJitterDrop       = pDDB->pSRT->CSRT.Stats.ConsCycleJitterDrop;
            pParams->SRT.ConsCycleOverloadCnt      = pDDB->pSRT->CSRT.Stats.ConsCycleOverloadCnt;
            pParams->SRT.ConsRxCycleCounterOld     = pDDB->pSRT->CSRT.Stats.ConsRxCycleCounterOld;
            pParams->SRT.ConsRxWrongUDPLenOrIP     = pDDB->pSRT->CSRT.Stats.ConsRxWrongUDPLenOrIP;
            pParams->SRT.ConsRxWrongXRTLenOrMAC    = pDDB->pSRT->CSRT.Stats.ConsRxWrongXRTLenOrMAC;
            pParams->SRT.NoMoreKnotElements        = pDDB->pSRT->CSRT.Stats.NoMoreKnotElements;
            pParams->SRT.ProvCycleCounterDropCnt   = pDDB->pSRT->CSRT.Stats.ProvCycleCounterDropCnt;
            pParams->SRT.ProvCycleOverloadCnt      = pDDB->pSRT->CSRT.Stats.ProvCycleOverloadCnt;
                #endif
                Status = EDD_STS_OK;
                break;

            case EDDS_SRV_DEBUG_SETUP_PHY:
                EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDDS_SRV_DEBUG_SETUP_PHY (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
                Status = EDDS_RequestSetupPHY(pRQB,pHDB);
                break;

            default:
                EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_DebugRequest: Invalid Service (Service: 0x%X)",EDD_RQB_GET_SERVICE(pRQB));
                Status = EDD_STS_ERR_SERVICE;
                break;
        }
    }
    else
    {
        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> Request: Service: 0x%X, RQB->pParam is NULL",EDD_RQB_GET_SERVICE(pRQB));
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DebugRequest(), Status: 0x%X",
                           Status);

}
//lint +esym(613,pRQB,pHDB)

#ifdef LLIF_CFG_SWITCH_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSwitchSetPortState              +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_SET_PORT_STATE   +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE                 +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles the EDD_SRV_SWITCH_SET_PORT_STATE rq+*/
/*+                                                                         +*/
/*+               We call the LLIF function to change the PortStates and    +*/
/*+               on success we indicate a link-change for all Ports/handles+*/
/*+               provided a ext-link indication resource.                  +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSwitchSetPortState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                  Status;
    EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE       pPortStates;
    LSA_UINT32                                  Idx;
    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB			= pHDB->pDDB;


    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSwitchSetPortState(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    if(EDDS_LL_AVAILABLE(pDDB,setSwitchPortState))
     {
    Status = EDD_STS_OK;

    pPortStates = (EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE) pRQB->pParam;

    if ( LSA_HOST_PTR_ARE_EQUAL( pPortStates, LSA_NULL) )
    {
      EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDDS_RequestSwitchSetPortState: RQB->pParam is NULL!");
      Status = EDD_STS_ERR_PARAM;
    }
    else
    {

      EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_SET_PORT_STATE(pDDB: 0x%X, pPortStates: 0x%X)",pDDB,pPortStates);

      Status = EDDS_LL_SWITCH_SET_PORT_STATE(
             pDDB,
             pPortStates);

      EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_SET_PORT_STATE(). Status: 0x%X",Status);


      Idx = 1;

      /*------------------------------------------------------------------------*/
      /* For every Port with changed PortState look for handles requiring a     */
      /* ext-link indication                                                    */
      /*------------------------------------------------------------------------*/

      /* loop over all ports */
      while (( Idx <= pDDB->pGlob->HWParams.Caps.PortCnt ) && ( Status == EDD_STS_OK ))
      {
        /* only if PortState changed */
        if ( pPortStates->PortIDState[Idx-1] != EDD_PORT_STATE_UNCHANGED)
        {
          /* PortState differs ? */
          if ( pDDB->pGlob->LinkStatus[Idx].PortState != pPortStates->PortIDState[Idx-1] )
          {
            pDDB->pGlob->LinkStatus[Idx].PortState = pPortStates->PortIDState[Idx-1]; /* save actual portstate */
            EDDS_IndicateExtLinkChange(pDDB,Idx);
          }
        }

        Idx++; /* next port */
      }
    }
     }else
     {
    Status = EDD_STS_ERR_NOT_SUPPORTED;
     }

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RequestSwitchSetPortState(), Status: 0x%X",
                           Status);

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RequestSwitchFlushFDB                  +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_FLUSH_FILTERING_DB*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:                                     +*/
/*+                                                                         +*/
/*+  pParam points to NULL (not used)                                       +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles EDD_SRV_SWITCH_FLUSH_FILTERING_DB   +*/
/*+                                                                         +*/
/*+               Note: Opcode/Service not checked!                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSwitchFlushFDB(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                 Status;
    EDDS_LOCAL_DDB_PTR_TYPE     			   pDDB	= pHDB->pDDB;
    EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE  pFlushFilterDB;
    LSA_UNUSED_ARG(pRQB);

    EDDS_CORE_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RequestSwitchFlushFDB(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);
    if(EDDS_LL_AVAILABLE(pDDB,flushSwitchFilteringDB))
     {
    LSA_UINT16 PortIdx;
    LSA_UINT16 Port;

    pFlushFilterDB = (EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE) pRQB->pParam;

    /* check PortCnt */
    if ((pFlushFilterDB->PortCnt < 1) || (pFlushFilterDB->PortCnt > EDD_CFG_MAX_PORT_CNT))
    {
      EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortCnt (%u)",pFlushFilterDB->PortCnt);
      return EDD_STS_ERR_PARAM;
    }

    /* check port status for every port in PortID array */
    /* range of PortCnt: 1..EDD_CFG_MAX_PORT_CNT        */
    for (PortIdx = 0 ; PortIdx < pFlushFilterDB->PortCnt ; PortIdx++)
    {
      /* PortID[] value range 1..EDD_CFG_MAX_PORT_CNT */
      Port = pFlushFilterDB->PortID[PortIdx];

      if ((Port < 1) ||
        (Port > pDDB->pGlob->HWParams.Caps.PortCnt))
      {
        EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortID (%u)",pFlushFilterDB->PortID[PortIdx]);
        return EDD_STS_ERR_PARAM;
      }

      if (EDD_PORT_STATE_BLOCKING != pDDB->pGlob->LinkStatus[Port].PortState &&
          EDD_PORT_STATE_DISABLE  != pDDB->pGlob->LinkStatus[Port].PortState)
      {
        EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,"EDDS_RequestSwitchFlushFDB(), invalid PortState for port (%u)",Port);
        return EDD_STS_ERR_PARAM;
      }
    }

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB: 0x%X)",pDDB);

    Status = EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB, pFlushFilterDB);

    EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SWITCH_FLUSH_FILTERING_DB(). Status: 0x%X",Status);

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                 "OUT:EDDS_RequestSwitchFlushFDB(), Status: 0x%X",
                 Status);
  }else
  {
    Status = EDD_STS_ERR_NOT_SUPPORTED;
  }

    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SwitchRequest                          +*/
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
/*+     EDD_SERVICE             Service:    EDD_SRV_SWITCH_SET_PORT_STATE   +*/
/*+                                         EDD_SRV_SWITCH_FLUSH_FILTERING_DB*/
/*+                                         EDDS_SRV_SWITCH_SET_PORT_REDUNDANT*/
/*+                                         EDDS_SRV_SWITCH_SET_BRIDGE_MODE +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles EDDS switch services.               +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_SwitchRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB			= pHDB->pDDB;

    LSA_UNUSED_ARG(pDDB);

    EDDS_CORE_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_SwitchRequest(pRQB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pHDB);

    switch (EDD_RQB_GET_SERVICE(pRQB))
    {
        case EDD_SRV_SWITCH_SET_PORT_STATE:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SWITCH_SET_PORT_STATE (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestSwitchSetPortState(pRQB,pHDB);
            break;
        case EDD_SRV_SWITCH_FLUSH_FILTERING_DB:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_SWITCH_FLUSH_FILTERING_DB (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
            Status = EDDS_RequestSwitchFlushFDB(pRQB,pHDB);
            break;
        default:
            EDDS_UPPER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>> EDDS_RequestFinish: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_SERVICE;
            break;
    }


    EDDS_RequestFinish(pHDB,pRQB,Status);

    EDDS_CORE_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_SwitchRequest(), Status: 0x%X",
                           Status);

}
#endif


/*****************************************************************************/
/*  end of file EDDS_GEN.C                                                   */
/*****************************************************************************/

