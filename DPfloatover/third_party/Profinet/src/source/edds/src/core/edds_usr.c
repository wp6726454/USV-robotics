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
/*  F i l e               &F: edds_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS user-interface functions with RQBs          */
/*                                                                           */
/*                          - edds_system()                                  */
/*                          - edds_open_channel()                            */
/*                          - edds_request()                                 */
/*                          - edds_close_channel()                           */
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
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*  15.06.04    JS    removed ASRT functions                                 */
/*  17.08.04    JS    Added Debug-Request                                    */
/*  23.09.04    JS    Set Linkstatus to DOWN on error (instead of UNKNOWN)   */
/*  15.11.04    JS    Added InterfaceID                                      */
/*  14.01.05    JS    little changes to avoid compiler warnings              */
/*  20.01.05    JS    little support for Rx-Overload detection               */
/*  31.01.05    JS    LTRC support                                           */
/*  09.02.05    JS    added optional reentrance check                        */
/*  11.05.05    JS    added PhyType                                          */
/*  22.06.05    JS    added CableDelayNs                                     */
/*  15.08.05    JS    removed EDDS_SRV_ASRT                                  */
/*  01.10.05    JS    added LED-blink support                                */
/*  06.03.06    JS    added EDDS_SwitchRequest                                */
/*  17.03.06    JS    added MRP_PortState/MRRT_PortState                     */
/*  07.04.06    JS    only one indication queue for prm                      */
/*  02.05.06    JS    added PortMAC addresses                                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  29.06.06    JS    added  LesserCheckQuality flag                         */
/*  26.09.06    JS    added  LinkSpeedModeCapability                         */
/*  30.10.06    JS    added  check for MAC addresses <> 0. 0 not allowed.    */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
/*  18.10.07    JS    release all PRM-Indication RQBS present (not only one  */
/*                    per queue) on channel close.                           */
/*  03.12.07    JS    Module id changed from 0 to 10                         */
/*  15.01.08    JS    added MRP R-Port support                               */
/*  12.02.08    JS    added "IsWireless" parameter.                          */
/*  28.03.08    JS    pSys is no neccessary a pointer.. dont init with NULL  */
/*  04.03.08    JS    Bufix: if EDDS_CFG_REENTRANCE_CHECK is used access to  */
/*                    RQB was after it was already freed within              */
/*                    EDDS_EXIT_CHECK_REQUEST                                */
/*  23.07.08    JS    Bufix: read access to already invalid RQB within TRACE */
/*  10.12.08    JS    added PhyStatus                                        */
/*  22.12.08    JS    removed MRRT_PortState                                 */
/*  30.01.09    JS    added MAUType and MediaType                            */
/*  31.06.10    JS    added optional protection macros  (AP00952063)         */
/*  28.09.11    MA    moved EDDS_LL_GET_CAPS to EDDS_DeviceSetup             */
/*                    renamed miss typed EDDS_Etup*** functions              */
/*                    to EDDS_Setup***p                                      */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  10
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_USR */

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
/*                                 Macros                                    */
/*===========================================================================*/

/* get pDDB from hDDB */
#define EDDSGETDDB(hDDB) ((EDDS_LOCAL_DDB_PTR_TYPE) (hDDB))

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_InterruptRequest                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_INTERRUPT          +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDDS_UPPER_DEV_INTERNAL_PTR_TYPE +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DEV_INTERNAL_TYPE                            +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     ID       : spezifies the interrupt reason.                          +*/
/*+                EDDS_ISR_TX                                              +*/
/*+                EDDS_ISR_RX_NRT                                          +*/
/*+                EDDS_ISR_RX_cSRT                                         +*/
/*+                EDDS_ISR_RX_aSRT                                         +*/
/*+     Param    : not used.                                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from isr (edds intern) for further low  +*/
/*+               prio handling of interrupt.                               +*/
/*+                                                                         +*/
/*+               NRT: finish all ready (sended) frames.                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_InterruptRequest(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE    pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_BOOL                            Ready;
    LSA_UINT32                          Cnt;
    LSA_BOOL                            StopTimeout;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_InterruptRequest(pRQB: 0x%X)",
                           pRQB);

    #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                        EDDS_MODULE_ID,
                        __LINE__);
    }
    #endif

    pRQBParam   = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;

    /* use fast version getting pDDB */

    pDDB = EDDSGETDDB(pRQBParam->hDDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
    {

        switch ( pRQBParam->ID)
        {

                /*-------------------------------------------------------------*/
                /* A transmit interrupt occured. handle ready transmits and    */
                /* start pending transmit requests.                            */
                /* This is handled in HIGH priority context                    */
                /*-------------------------------------------------------------*/
            case EDDS_ISR_TX:

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
                EDDS_ENTER_HIGH();
#endif

                pDDB->pGlob->RQBInt_Tx.InUse = LSA_FALSE;

                /* The Timeouttimer is only stopped on first call to   */
                /* EDDS_TransmitSts. On further calls it remains at it is*/
                /* because it maybe is already restarted from the call */
                /* to function EDDS_TransmitTrigger and should not be  */
                /* stopped again.                                      */

                StopTimeout = LSA_TRUE;
                Cnt         = 0;
                Ready       = LSA_FALSE;

                /* Note that it is possible that we wont leave this    */
                /* loop as long as someone has something to send and   */
                /* we are slower than the packet which is send, so the */
                /* packet is ready when we enter EDDS_TransmitSts again. */
                /* Because of this we break if all tx-buffers          */
                /* handled and a new int-tx-request is pending.        */

                while ((! Ready ) &&
                       ( EDDS_TransmitSts(pDDB,StopTimeout) == EDD_STS_OK ))
                {

                    Cnt++;

                    /* TransmitTimeout restarted if something was send.*/
                    StopTimeout = LSA_FALSE;

                    /* Break if handled all tx-resources and a new     */
                    /* Tx-request is pending.                          */

                    if (( Cnt >= pDDB->pGlob->TxBuffCnt ) &&
                        ( pDDB->pGlob->RQBInt_Tx.InUse ))
                    {
                        Ready = LSA_TRUE;
                    }
                }

                #ifndef EDDS_CFG_SEND_CYCLE_BASED
                EDDS_TransmitTrigger(pDDB);
                #endif

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
                EDDS_EXIT_HIGH();
#endif
                break;

                /*-------------------------------------------------------------*/
                /* PHY-Interrupt (link/duplex/speed)-change occured.           */
                /*-------------------------------------------------------------*/
            case EDDS_ISR_PHY:
                pDDB->pGlob->RQBInt_PHY.InUse = LSA_FALSE;
                /* all what have to be done is done in this function */
                /* we dont need the actual link state here.          */
                EDDS_RequestPHYInterrupt(pDDB);
                break;

                #ifdef EDDS_CFG_NRT_INCLUDE
                /*-------------------------------------------------------------*/
                /* A receive  interrupt for NRT occured. handle received frames*/
                /*-------------------------------------------------------------*/
            case EDDS_ISR_RX_NRT:
                pDDB->pGlob->RQBInt_Rx_NRT.InUse = LSA_FALSE;

                EDDS_NRTRxRequestInterrupt(pDDB);
                break;
                #endif

                /*-------------------------------------------------------------*/
                /* A receive  interrupt for cyclic SRT occured (not used yet)  */
                /*-------------------------------------------------------------*/
            case EDDS_ISR_RX_cSRT:
                pDDB->pGlob->RQBInt_Rx_cSRT.InUse = LSA_FALSE;
                break;


            default:
                break;
        } /* switch */
    } /* if */

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_InterruptRequest()");

}
//lint +esym(613,pRQB)

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TimeoutRequest                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_TIMEOUT            +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDDS_UPPER_DEV_INTERNAL_PTR_TYPE +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DEV_INTERNAL_TYPE                            +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     ID       : spezifies the timeout reason.                            +*/
/*+                EDDS_TIMEOUT_TX                                          +*/
/*+     Param    : not used.                                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from isr (edds intern) for further low  +*/
/*+               prio handling of interrupt.                               +*/
/*+                                                                         +*/
/*+               NRT: finish all ready (sended) frames.                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_TimeoutRequest(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE    pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TimeoutRequest(pRQB: 0x%X)",
                           pRQB);

    #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                        EDDS_MODULE_ID,
                        __LINE__);
    }
    #endif

    pRQBParam   = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;  //lint !e613 this comment should no be nesessary! #JB_LINTERROR

    /* use fast version getting pDDB */

    pDDB = EDDSGETDDB(pRQBParam->hDDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
    {

        switch ( pRQBParam->ID)
        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                /*-------------------------------------------------------------*/
                /* A transmit timeout occured.                                 */
                /* This is handled in HIGH priority context                    */
                /*-------------------------------------------------------------*/
            case EDDS_TIMEOUT_TX:

                EDDS_TransmitTimeout(pDDB);

                break;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            case EDDS_TIMEOUT_LL:

            	EDDS_LLTimeoutCallback(pDDB);

            	break;

            default:
                break;
        } /* switch */
    } /* if */


    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TimeoutRequest()");


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_TriggerRequest                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_TRIGGER            +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDDS_UPPER_DEV_INTERNAL_PTR_TYPE +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DEV_INTERNAL_TYPE                            +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     ID       : spezifies the timeout reason.                            +*/
/*+                EDDS_TRIGGER_PROV_INDICATION                             +*/
/*+                EDDS_TRIGGER_PROVIDER_CYCLE                              +*/
/*+                EDDS_TRIGGER_CONSUMER_CYCLE                              +*/
/*+                EDDS_TRIGGER_NRT_CANCEL_PART2                            +*/
/*+                EDDS_TRIGGER_RX_OVER_CYCLE                               +*/
/*+                EDDS_TRIGGER_RX_OVER_EVENT                               +*/
/*+     Param    : not used.                                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from EDDS for further handling of events +*/
/*+                                                                         +*/
/*+               * Start Provider Cycle                                    +*/
/*+               * Start Consumer Check Cycle                              +*/
/*+               * Start Indication of Provider events.                    +*/
/*+               * cyclic rx overload timer                                +*/
/*+               * Rx overload event                                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_TriggerRequest(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    #ifdef EDDS_CFG_NRT_INCLUDE
    LSA_RESULT                          Status;
    EDDS_LOCAL_HDB_PTR_TYPE             pHDB;
    #endif
    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE    pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_TriggerRequest(pRQB: 0x%X)",
                           pRQB);


    #ifdef EDDS_CFG_DO_INTERNAL_FATAL_CHECK
    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                        EDDS_MODULE_ID,
                        __LINE__);
    }
    #endif

    pRQBParam   = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;  //lint !e613 this comment should not be necessary #JB_LINTERROR

    /* use fast version getting pDDB */

    pDDB = EDDSGETDDB(pRQBParam->hDDB);

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
    {

        switch ( pRQBParam->ID)
        {

                #ifdef EDDS_CFG_SRT_INCLUDE
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                /*-------------------------------------------------------------*/
                /* Trigger start of new provider cycle                         */
                /* -> triggered by EDDS_DO_PROVIDER_TIMER_REQUEST              */
                /* This is handled in HIGH priority context                    */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_PROVIDER_CYCLE:
                if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
                {

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
					EDDS_ENTER_HIGH();
#endif
					EDDS_CSRTProviderCycleRequest(pDDB);

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
					EDDS_EXIT_HIGH();
#endif
                }
                break;
                /*-------------------------------------------------------------*/
                /* Provider indications are present and should be handled..    */
                /* -> triggered by EDDS_DO_PROVIDER_IND_REQUEST                */
                /* This is handled in LOW or MIDDLE priority context           */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_PROV_INDICATION:

#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_ENTER_MIDDLE();
#endif
                EDDS_RTProviderCheckIndRequest(pDDB);

#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_EXIT_MIDDLE();
#endif
                break;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                /*-------------------------------------------------------------*/
                /* Consumer indications are present and should be handled..    */
                /* -> triggered by EDDS_DO_CONSUMER_IND_REQUEST                */
                /* This is handled in LOW or MIDDLE priority context           */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_CONS_INDICATION:

#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_ENTER_MIDDLE();
#endif
                EDDS_RTConsumerCheckIndRequest(pDDB);

#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_EXIT_MIDDLE();
#endif
                break;

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                /*-------------------------------------------------------------*/
                /* Start Consumer check..                                      */
                /* -> triggered by EDDS_DO_CONSUMER_TIMER_REQUEST              */
                /* This is handled in LOW or MIDDLE priority context           */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_CONSUMER_CYCLE:
#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_ENTER_MIDDLE();
#endif
                EDDS_CSRTConsumerCycleRequest(pDDB);

#ifdef EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT
                EDDS_EXIT_MIDDLE();
#endif
                break;

                #endif
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                #ifdef EDDS_CFG_NRT_INCLUDE
                /*-------------------------------------------------------------*/
                /* NRT-CANCEL Part2. This is triggered from Part1 of CANCEL    */
                /* -> triggered by EDDS_DO_NRT_CANCEL_PART2                    */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_NRT_CANCEL_PART2:
                Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB); /* get handle */  //lint !e613 why? #JB_LINTERROR

                if ( Status != EDD_STS_OK)
                {
                    EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                }

                EDDS_NRTCancelPart2(pHDB);
                break;

                #endif

                #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
                /*-------------------------------------------------------------*/
                /* Rx overload cylic timer event                               */
                /* -> triggered by EDDS_DO_RX_OVERLOAD_TIMER_REQUEST           */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_RX_OVER_CYCLE:
                pDDB->pGlob->RQBInt_Rx_OverTimer.InUse = LSA_FALSE;
                EDDS_RxOverloadCycleRequest(pDDB);
                break;

                /*-------------------------------------------------------------*/
                /* Rx overload event (overload).                               */
                /* -> triggered by EDDS_DO_RX_OVERLOAD_REQUEST                 */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_RX_OVER_EVENT:
                pDDB->pGlob->RQBInt_Rx_OverEvent.InUse = LSA_FALSE;
                EDDS_RxOverloadIndicateEvent(pDDB);
                break;
                #endif

                #ifdef EDDS_CFG_LED_BLINK_SUPPORT
                /*-------------------------------------------------------------*/
                /* LED Blink timer (500ms)                                     */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_LED_BLINK_CYCLE:
                pDDB->pGlob->RQBInt_LED_BlinkTimer.InUse = LSA_FALSE;
                EDDS_LEDBlinkCycleRequest(pDDB);
                break;
                #endif

                #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
                /*-------------------------------------------------------------*/
                /* Diag cycle timer (1s)                                        */
                /*-------------------------------------------------------------*/
            case EDDS_TRIGGER_DIAG_CYCLE:
                pDDB->pGlob->RQBInt_DiagTimer.InUse = LSA_FALSE;
                EDDS_DiagCycleRequest(pDDB);
                break;
                #endif

            default:
                break;
        } /* switch */
    } /* if */

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_TriggerRequest()");


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SetupLinkRequest                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_SETUP_LINK         +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     EDDS_UPPER_SETUP_LINK_PTR_TYPE  +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_SETUP_LINK_TYPE                              +*/
/*+                                                                         +*/
/*+     LinkSpeedMode    : LinkSpeedMode to set PHY to                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets PHY to the spezified LinkSpeedMode. Note that        +*/
/*+               this will override the settings form device-setup.        +*/
/*+               This function has no effect if device is not setup.       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SetupLinkRequest(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                          Status;
    EDDS_UPPER_SETUP_LINK_PTR_TYPE      pRQBParam;
    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;
    LSA_UINT8                           Autoneg;
    LSA_UINT8                           PhyStatus;


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_SetupLinkRequest(pRQB: 0x%X)",
                           pRQB);


    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )  //lint !e613 why? #JB_LINTERROR
    {
        EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    pRQBParam   = (EDDS_UPPER_SETUP_LINK_PTR_TYPE)pRQB->pParam;  //lint !e613 why? #JB_LINTERROR

    Status = EDDS_GetDDB(pRQBParam->hDDB, &pDDB);

    /* check if hDDB already exists. if not the request will be dropped */

    if (Status == EDD_STS_OK)
    {

        if  ( (pRQBParam->PortID > 0) && (pRQBParam->PortID <= pDDB->pGlob->HWParams.Caps.PortCnt))
        {

            EDDS_LOWER_TRACE_03(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_SET_LINK_STATE(pDDB: 0x%X, LSP: 0x%X, Power: 0x%X)",
                                pDDB,
                                pRQBParam->LinkSpeedMode,
                                pRQBParam->PHYPower);

            Status = EDDS_LL_SET_LINK_STATE(pDDB,
                                            pRQBParam->PortID,
                                            pRQBParam->LinkSpeedMode,
                                            pRQBParam->PHYPower);

            EDDS_LOWER_TRACE_01(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SET_LINK_STATE(). Status: 0x%X",Status);

            if ( Status == EDD_STS_OK )
            {
                EDDS_HW_PARAM_PTR_TYPE       pHWParams;
                EDDS_LINK_STAT_PTR_TYPE      pLinkStatus;
                EDD_RQB_GET_LINK_STATUS_TYPE LinkStatus;
                LSA_UINT16                   MAUType;
                LSA_UINT32                   PortStatus, AutonegCapAdvertised;
                LSA_UINT8                    LinkSpeedModeConfigured, MediaType;

                EDDS_LOWER_TRACE_02(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,pRQBParam->PortID);

                /* NOTE: LinkSpeedModeConfigured is the actual phy configuration. A "AutoNeg"-configuration (=default) maybe mapped to a fixed */
                /*       setting if autoneg is not supported. Typically the case with POF ports because autoneg is not supported with POF.     */
                /*       The actual autoneg to fixed mapping depends on phy adaption by LL.                                                    */
                /*       All other parameters from EDDS_LL_GET_LINK_STATE are not used here!                                                   */
                Status = EDDS_LL_GET_LINK_STATE(pDDB, pRQBParam->PortID, &LinkStatus,&MAUType,&MediaType,&PortStatus,&AutonegCapAdvertised,&LinkSpeedModeConfigured);

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

                pHWParams   = &pDDB->pGlob->HWParams;
                pLinkStatus = &pDDB->pGlob->LinkStatus[pRQBParam->PortID];

                pHWParams->LinkSpeedMode[pRQBParam->PortID-1] = LinkSpeedModeConfigured;
                pHWParams->PHYPower[pRQBParam->PortID-1]      = pRQBParam->PHYPower;

                /* Setup PhyStatus Variable for Link */
                if ( EDDS_PHY_POWER_ON == pHWParams->PHYPower[pRQBParam->PortID-1] )
                {
                    PhyStatus = EDD_PHY_STATUS_ENABLED;
                }
                else
                {
                    PhyStatus = EDD_PHY_STATUS_DISABLED;
                }

                Autoneg = EDD_AUTONEG_OFF;
                if ( LinkSpeedModeConfigured == EDD_LINK_AUTONEG ) Autoneg = EDD_AUTONEG_ON;

                /* autoneg or phy status changed? */
                if ((pLinkStatus->Autoneg   != Autoneg    ) ||
                    (pLinkStatus->PhyStatus != PhyStatus))
                {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    if ((EDDS_PHY_POWER_OFF == pHWParams->PHYPower[pRQBParam->PortID-1]) &&
                        (pLinkStatus->PhyStatus != PhyStatus))
                    {
                        if(pDDB->pGlob->RQBInt_PHY.InUse == LSA_FALSE)
                        {
                            /* simulate a phy interrupt (only when currently no RQB is en route), */
                            /* because some controller might not generate a phy interrupt on phy  */
                            /* power down */
                            /* NOTE: EDDS_SRV_DEV_SETUP_LINK and EDDS_DO_INT_REQUEST_PHY are both */
                            /*       running in low context, so we don't have a problem here      */
                            EDDS_RequestPHYInterrupt(pDDB);
                        }
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

                    /* Store configured Autoneg setting */
                    pLinkStatus->Autoneg   = Autoneg;
                    pLinkStatus->PhyStatus = PhyStatus;
                    EDDS_IndicateExtLinkChange(pDDB,pRQBParam->PortID);
                }
            }
        }
        else
        {
            Status = EDD_STS_ERR_PARAM;
        }
    }

    EDDS_RQB_SET_STATUS(pRQB, Status);
    EDDS_CallCbf(pRQBParam->Cbf,pRQB);

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_SetupLinkRequest()");

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceOpen                             +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_OPEN               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_INI_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDDB     : Returned Device-Handle                                   +*/
/*+     pDDB     : Pointer to device-specific initialization parameters     +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         EDD_STS_ERR_NO_TIMER            +*/
/*+                                         EDD_STS_ERR_HW                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a device. This request is the first request*/
/*+               needed for initialization of the device. It returns an    +*/
/*+               device-handle which have to be used in further calls.     +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pDDB)
//checked within EDDS_ENTER_CHECK_REQUEST
//pDDB gets set within this function
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceOpen(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_INI_PTR_TYPE     pRqbDDB;
    EDDS_UPPER_DPB_PTR_TYPE         pDPB;   /* Pointer to device parameter*/
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    LSA_BOOL                        DDBInit;
    LSA_BOOL                        LLTimerInit;
#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    LSA_BOOL                        RxTimerInit;
#endif
    LSA_BOOL                        LEDTimerInit;
#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
    LSA_BOOL                        DiagTimerInit;
#endif
    LSA_BOOL						SWIError;
    LSA_UINT16                      RetVal;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_BOOL                        TimerInit;
#endif

#ifdef LLIF_CFG_SWITCH_SUPPORT
#ifndef EDDS_DONT_USE_MRP
	LSA_UINT32                    PortIndex;
    LSA_UINT32                    MRPRedundantPortCnt = 0;
	LSA_UINT8                     RingPort;
#endif
#endif

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceOpen(pRQB: 0x%X)",
                           pRQB);

    DDBInit       = LSA_FALSE;
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	TimerInit     = LSA_FALSE;
#endif

    LLTimerInit   = LSA_FALSE;
#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    RxTimerInit   = LSA_FALSE;
#endif
    LEDTimerInit  = LSA_FALSE;
#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
    DiagTimerInit = LSA_FALSE;
#endif
    SWIError 	  = LSA_FALSE;
    Status        = EDD_STS_OK;
    pDDB          = LSA_NULL;  /* avoids compiler warning */

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceOpen: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        pRqbDDB = (EDDS_UPPER_DDB_INI_PTR_TYPE)pRQB->pParam;

        pDPB = pRqbDDB->pDPB;      /* pointer to hardwarespecific parameters */

#ifdef LLIF_CFG_SWITCH_SUPPORT   /* check SWI Parameter */
#ifndef EDDS_DONT_USE_MRP
    //check for valid mrp instance count (0 .. EDD_CFG_MAX_MRP_INSTANCE_CNT)
	if( EDD_CFG_MAX_MRP_INSTANCE_CNT >= pDPB->SWI.MaxMRP_Instances )
	{
		// MRP is supported?
		if( 0 != pDPB->SWI.MaxMRP_Instances )
		{
			for (PortIndex = 0; PortIndex < EDDS_MAX_PORT_CNT; PortIndex++)
			{
				RingPort = pDPB->SWI.MRPRingPort[PortIndex];   // EDD_MRP_RING_PORT, EDD_MRP_NO_RING_PORT or EDD_MRP_RING_PORT_DEFAULT

				// count number of default ring ports
				if(EDD_MRP_RING_PORT_DEFAULT == RingPort)
				{
					MRPRedundantPortCnt++;
				}
				else
				{
					// check for invalid ring port type
					if( (EDD_MRP_RING_PORT    != RingPort) &&
						(EDD_MRP_NO_RING_PORT != RingPort) )
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPRingPort at index %d.", PortIndex);
						SWIError = LSA_TRUE;
					}
				}
			} //end of loop

			// check default ring port count
			if( !SWIError )
			{
				// check for even default ring port count
				if( 0 != (MRPRedundantPortCnt & 1) )
				{
					EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Uneven default ring port count.");
					SWIError = LSA_TRUE;
				}
			}

			// check for valid MRPSupportedRole
			if( !SWIError )
			{
				if( pDPB->SWI.MRPSupportedRole & (~(EDD_MRP_ROLE_CAP_MANAGER | EDD_MRP_ROLE_CAP_CLIENT | EDD_MRP_ROLE_CAP_AUTOMANAGER)) )
				{
					EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPSupportedRole.");
					SWIError = LSA_TRUE;
				}
			}

			// default role must be part of supported roles
			// NOTE: dont check MRPSupportedMultipleRole, this is done by MRP
			if( !SWIError )
			{
				LSA_UINT32 MRPSupportedRoleCap = 0;

				switch( pDPB->SWI.MRPDefaultRoleInstance0 )
				{
					case EDD_MRP_ROLE_MANAGER:
					{
						MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_MANAGER;
						break;
					}
					case EDD_MRP_ROLE_CLIENT:
					{
						MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_CLIENT;
						break;
					}
					case EDD_MRP_ROLE_AUTOMANAGER:
					{
						MRPSupportedRoleCap = EDD_MRP_ROLE_CAP_AUTOMANAGER;
						break;
					}
					case EDD_MRP_ROLE_NONE:
                    {
                        MRPSupportedRoleCap = 0;
                        break;
                    }
					    
					default:
					{
						EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MRPDefaultRoleInstance0.");
						SWIError = LSA_TRUE;
						break;
					}
				}

				if ( !SWIError )
				{
					if(    !(pDPB->SWI.MRPSupportedRole & MRPSupportedRoleCap) 
					    &&  (MRPSupportedRoleCap != 0))
					{
						EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: MRPSupportedRole does not match MRPDefaultRoleInstance0.");
						SWIError = LSA_TRUE;
					}
				}
			} // default role must be part of supported roles
		}
	}
	else
	{
		EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Illegal value for MaxMRP_Instances.");
		SWIError = LSA_TRUE;
	}
#endif	/* EDDS_DONT_USE_MRP */
#endif /* LLIF_CFG_SWITCH_SUPPORT */
        /* Check for valid parameters, more checks done in EDDS_LL_OPEN */

    if (( pDPB->InterfaceID < EDD_INTERFACE_ID_MIN  )        ||
            ( pDPB->InterfaceID > EDD_INTERFACE_ID_MAX  )        ||
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            ( pDPB->RxBufferOffset > pDPB->RxBufferSize )        ||
            ( pDPB->TxBufferCnt    == 0                        ) ||  //unsigned char can not be less than 0 (former <=)
            ( pDPB->RxBufferOffset & EDDS_RX_FRAME_BUFFER_ALIGN ) ||
            ( pDPB->RxBufferSize - pDPB->RxBufferOffset <  EDD_FRAME_BUFFER_LENGTH ) ||
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            ( pDPB->MaxInterfaceCntOfAllEDD > EDD_CFG_MAX_INTERFACE_CNT) ||
            ( pDPB->MaxPortCntOfAllEDD > EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE) ||
            ( LSA_NULL == pDPB->pLLFunctionTable )||
            ( LSA_NULL == pDPB->pLLManagementData )||
            (LSA_TRUE == SWIError)  //lint !e731 JB 12/11/2014 on purpose
        )  //lint !e774 !e845 JB 12/11/2014 see #ifdef
        {
            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Invalid RQB parameter!");
            Status = EDD_STS_ERR_PARAM;
        }

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    if((2*149) > pDPB->RxBufferCnt )
        {
            pDPB->RxBufferCnt = (2*149);    //@fixme am2219 honor scheduler trigger time
        }

        /* @fixme do we need this TX Buffer-Count? Calculate it from number of providers + NRT bandwidth ! */
    if((2*149) > pDPB->TxBufferCnt )
        {
            pDPB->TxBufferCnt = (2*149);    //@fixme am2219 honor scheduler trigger time
        }

    /* ********************************************************************* */
    /* scheduler cycle: check for 1,2,4,8,16,32 ... value                    */
    /* ********************************************************************* */
    if( !(pDPB->schedulerCycle && !(pDPB->schedulerCycle & (pDPB->schedulerCycle-1))))
    {
        Status = EDD_STS_ERR_PARAM;
        EDDS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "EDDS_DeviceOpen: ERROR -> invalid SchedulerCycle(%d)!",
            pDPB->schedulerCycle);
    }

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

        #if defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD)
    if (0 == pDPB->RxIsrMaxFrameCnt)
        {
            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: RxIsrMaxFrameCnt can not be 0!");
            Status = EDD_STS_ERR_PARAM;
        }
        #endif

    if (Status == EDD_STS_OK)
        {
            Status  = EDDS_CreateDDB(&pDDB,pDPB);
        }

    if (Status == EDD_STS_OK)
        {
			DDBInit = LSA_TRUE;

            pDDB->pGlob->pLLFunctionTable  = (EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE) pDPB->pLLFunctionTable;
            pDDB->pGlob->pLLManagementData = pDPB->pLLManagementData;

            if(	!EDDS_LL_AVAILABLE(pDDB,open)
            		|| !EDDS_LL_AVAILABLE(pDDB,setup)
            		|| !EDDS_LL_AVAILABLE(pDDB,shutdown)
            		|| !EDDS_LL_AVAILABLE(pDDB,close)
            		|| !EDDS_LL_AVAILABLE(pDDB,enqueueSendBuffer)
            		|| !EDDS_LL_AVAILABLE(pDDB,getNextFinishedSendBuffer)
#if 0 /* optional function */
#ifdef EDDS_CFG_SEND_BLOCKED
            		|| !EDDS_LL_AVAILABLE(pDDB,triggerSend)
#endif
#endif
            		|| !EDDS_LL_AVAILABLE(pDDB,reinitTX)
            		|| !EDDS_LL_AVAILABLE(pDDB,getNextReceivedBuffer)
            		|| !EDDS_LL_AVAILABLE(pDDB,provideReceiveBuffer)
            		|| !EDDS_LL_AVAILABLE(pDDB,getLinkStateSetup)
            		|| !EDDS_LL_AVAILABLE(pDDB,getLinkState)
            		|| !EDDS_LL_AVAILABLE(pDDB,checkMAU)
            		|| !EDDS_LL_AVAILABLE(pDDB,getStatistics)
            		|| !EDDS_LL_AVAILABLE(pDDB,getCapabilities)
            		|| !EDDS_LL_AVAILABLE(pDDB,getMACAddr)
            		|| !EDDS_LL_AVAILABLE(pDDB,enableMC)
            		|| !EDDS_LL_AVAILABLE(pDDB,disableMC)
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    || !EDDS_LL_AVAILABLE(pDDB,recurringTask)
#else /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            		|| !EDDS_LL_AVAILABLE(pDDB,getPendingInterrupt)
            		|| !EDDS_LL_AVAILABLE(pDDB,interruptRX)
            		|| !EDDS_LL_AVAILABLE(pDDB,interruptTX)
            		|| !EDDS_LL_AVAILABLE(pDDB,interruptPhy)
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
#if 0 /* optional function */
#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
            		|| !EDDS_LL_AVAILABLE(pDDB,interrupt)
#endif
#endif
            		|| !EDDS_LL_AVAILABLE(pDDB,setLinkState)
            		|| !EDDS_LL_AVAILABLE(pDDB,backupPortLEDMode)
            		|| !EDDS_LL_AVAILABLE(pDDB,restorePortLEDMode)
            		|| !EDDS_LL_AVAILABLE(pDDB,setPortLED)
#if 0 /* optional functions */
#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
            		|| !EDDS_LL_AVAILABLE(pDDB,enableRX)
            		|| !EDDS_LL_AVAILABLE(pDDB,disableRX)
#endif
#ifdef LLIF_CFG_SWITCH_SUPPORT
            		|| !EDDS_LL_AVAILABLE(pDDB,setSwitchPortState)
            		|| !EDDS_LL_AVAILABLE(pDDB,controlSwitchMulticastFwd)
            		|| !EDDS_LL_AVAILABLE(pDDB,flushSwitchFilteringDB)
            		|| !EDDS_LL_AVAILABLE(pDDB,setSwitchPortRedundant)
            		|| !EDDS_LL_AVAILABLE(pDDB,setSwitchBridgeMode)
#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
            		|| !EDDS_LL_AVAILABLE(pDDB,getSwitchDroppedFrames)
#endif /* LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT */
#endif /* LLIF_CFG_SWITCH_SUPPORT */
#endif /* 0 */
#if 0 /* optional function */
#ifdef LLIF_CFG_USE_LL_ARP_FILTER
            		|| !EDDS_LL_AVAILABLE(pDDB,setArpFilter)
#endif /* LLIF_CFG_USE_LL_ARP_FILTER */
#endif
            		/* EDDS_LL_AVAILABLE(pDDB,timeout) is not necessary, because this function is not mandatory! */
            )
            {
				EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
						"EDDS_DeviceOpen: mandatory function pLLFunctionTable is missing!"
				);
				Status = EDD_STS_ERR_PARAM;
            }else
            {
				/* returnvalue: DDB handle */

				pRqbDDB->hDDB   = pDDB->hDDB;

				pDDB->hSysDev = pDPB->hSysDev;   // save system handle of device
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
				pDDB->RxIsrMaxFrameCnt = pDPB->RxIsrMaxFrameCnt;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

				pDDB->pGlob->TraceIdx = pDPB->TraceIdx;

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                pDDB->pGlob->schedulerCycle = pDPB->schedulerCycle;
#endif
				pDDB->pGlob->HWParams.InterfaceID    = pDPB->InterfaceID;
				pDDB->pGlob->HWParams.Caps.PortCnt   = EDDS_MAX_PORT_CNT;

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
				pDDB->pGlob->RxBufferSize   = pDPB->RxBufferSize;
				pDDB->pGlob->RxBufferOffset = pDPB->RxBufferOffset;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

				pDDB->pGlob->MaxInterfaceCntOfAllEDD	= pDPB->MaxInterfaceCntOfAllEDD;
				pDDB->pGlob->MaxPortCntOfAllEDD			= pDPB->MaxPortCntOfAllEDD;
	#ifdef LLIF_CFG_SWITCH_SUPPORT
				pDDB->SWI								= pDPB->SWI;
	#endif
				/*---------------------------------------------------------------*/
				/* Init DIAG structures                                          */
				/*---------------------------------------------------------------*/

				EDDS_PrmInit(pDDB);

				/*---------------------------------------------------------------*/
				/* allocate Tx-Timer if configured                               */
				/*---------------------------------------------------------------*/
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

				pDDB->pGlob->TxToutCnt = pDPB->TxToutCnt; /* save timeout-count  */

				if ( pDPB->TxToutCnt )
				{

					EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
										 "--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
										 LSA_TIMER_TYPE_ONE_SHOT,
										 LSA_TIME_BASE_1S);

					EDDS_ALLOC_TIMER(&RetVal,
									 (LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *)&pDDB->pGlob->TxTimer.TimerID,
									 LSA_TIMER_TYPE_ONE_SHOT,
									 LSA_TIME_BASE_1S);

					if ( RetVal == LSA_RET_OK)
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
											 "<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
											 pDDB->pGlob->TxTimer.TimerID);

						TimerInit     = LSA_TRUE;

						EDDS_TimerSet(pDDB->pGlob->TxTimer.TimerID,
									  &pDDB->pGlob->RQBInt_Tx_Tout,
									  LSA_NULL,
									  &pDDB->pGlob->TxTimer.EDDSTimerID);

						pDDB->pGlob->TxTimer.InProgress = LSA_FALSE;
						pDDB->pGlob->TxTimer.InUse      = LSA_TRUE;

					}
					else
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot alloc timer (0x%X)!",RetVal);
						Status = EDD_STS_ERR_NO_TIMER;
					}


				}
				else pDDB->pGlob->TxTimer.InUse = LSA_FALSE;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
				/*---------------------------------------------------------------*/
				/* allocate LL-Timer if configured                               */
				/*---------------------------------------------------------------*/

				if ( EDDS_LL_AVAILABLE(pDDB,timeout) )
				{

					EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
										 "--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
										 LSA_TIMER_TYPE_ONE_SHOT,
										 LSA_TIME_BASE_10MS);

					EDDS_ALLOC_TIMER(&RetVal,
									 (LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *)&pDDB->pGlob->LLTimer.TimerID,
									 LSA_TIMER_TYPE_ONE_SHOT,
									 LSA_TIME_BASE_10MS);

					if ( RetVal == LSA_RET_OK)
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
											 "<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
											 pDDB->pGlob->LLTimer.TimerID);

						LLTimerInit   = LSA_TRUE;

						EDDS_TimerSet(pDDB->pGlob->LLTimer.TimerID,
									  &pDDB->pGlob->RQBInt_LL_Tout,
									  EDDS_LLTimerCbf,
									  &pDDB->pGlob->LLTimer.EDDSTimerID);

						pDDB->pGlob->LLTimer.InProgress = LSA_FALSE;
						pDDB->pGlob->LLTimer.InUse      = LSA_TRUE;
					}
					else
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot alloc timer (0x%X)!",RetVal);
						Status = EDD_STS_ERR_NO_TIMER;
					}


				}
				else pDDB->pGlob->LLTimer.InUse = LSA_FALSE;

				/*---------------------------------------------------------------*/
				/* allocate Rx Overload Timer if configured.                     */
				/*---------------------------------------------------------------*/

				#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE

				if(EDDS_LL_AVAILABLE(pDDB,disableRX))
				{

					EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
										 "--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
										 LSA_TIMER_TYPE_CYCLIC,
										 LSA_TIME_BASE_100MS);

					EDDS_ALLOC_TIMER(&RetVal,
									 (LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *)&pDDB->pGlob->RxOverload.Timer.TimerID,
									 LSA_TIMER_TYPE_CYCLIC,
									 LSA_TIME_BASE_100MS);

					if ( RetVal == LSA_RET_OK)
					{

						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
											 "<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
											 pDDB->pGlob->RxOverload.Timer.TimerID);

						RxTimerInit = LSA_TRUE;

						EDDS_TimerSet(pDDB->pGlob->RxOverload.Timer.TimerID,
									  &pDDB->pGlob->RQBInt_Rx_OverTimer,
									  EDDS_RxOverloadTimerCbf,
									  &pDDB->pGlob->RxOverload.Timer.EDDSTimerID);

						pDDB->pGlob->RxOverload.Timer.InProgress = LSA_FALSE;
						pDDB->pGlob->RxOverload.Timer.InUse      = LSA_TRUE;

					}
					else
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot alloc timer (0x%X)!",RetVal);
						Status = EDD_STS_ERR_NO_TIMER;
					}
				}
				#endif

				/*---------------------------------------------------------------*/
				/* allocate LED Blink Timer if configured                        */
				/*---------------------------------------------------------------*/

				#ifdef EDDS_CFG_LED_BLINK_SUPPORT

				EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
									 "--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
									 LSA_TIMER_TYPE_CYCLIC,
									 LSA_TIME_BASE_100MS);

				EDDS_ALLOC_TIMER(&RetVal,
								 (LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *)&pDDB->pGlob->LEDBLink.Timer.TimerID,
								 LSA_TIMER_TYPE_CYCLIC,
								 LSA_TIME_BASE_100MS);

				if ( RetVal == LSA_RET_OK)
				{

					EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
										 "<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
										 pDDB->pGlob->LEDBLink.Timer.TimerID);

					LEDTimerInit = LSA_TRUE;

					EDDS_TimerSet(pDDB->pGlob->LEDBLink.Timer.TimerID,
								  &pDDB->pGlob->RQBInt_LED_BlinkTimer,
								  EDDS_LEDBlinkTimerCbf,
								  &pDDB->pGlob->LEDBLink.Timer.EDDSTimerID);

					pDDB->pGlob->LEDBLink.Timer.InProgress = LSA_FALSE;
					pDDB->pGlob->LEDBLink.Timer.InUse      = LSA_TRUE;

				}
				else
				{
					EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
					EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot alloc timer (0x%X)!",RetVal);
					Status = EDD_STS_ERR_NO_TIMER;
				}
				#endif

				/*---------------------------------------------------------------*/
				/* allocate DIAG-Timer if configured                             */
				/*---------------------------------------------------------------*/

				#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT

				if(EDDS_LL_AVAILABLE(pDDB,getSwitchDroppedFrames))
				{
					EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
										 "--> Calling EDDS_ALLOC_TIMER (TimerType: 0x%X, TimerBase: 0x%X).",
										 LSA_TIMER_TYPE_CYCLIC,
										 LSA_TIME_BASE_1S);

					EDDS_ALLOC_TIMER(&RetVal,
									 (LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *)&pDDB->pGlob->Prm.Timer.TimerID,
									 LSA_TIMER_TYPE_CYCLIC,
									 LSA_TIME_BASE_1S);

					if ( RetVal == LSA_RET_OK)
					{

						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
											 "<-- Calling EDDS_ALLOC_TIMER success. TimerID: %d.",
											 pDDB->pGlob->Prm.Timer.TimerID);

						DiagTimerInit = LSA_TRUE;

						EDDS_TimerSet(pDDB->pGlob->Prm.Timer.TimerID,
									  &pDDB->pGlob->RQBInt_DiagTimer,
									  EDDS_DiagTimerCbf,
									  &pDDB->pGlob->Prm.Timer.EDDSTimerID);

						pDDB->pGlob->Prm.Timer.InProgress = LSA_FALSE;
						pDDB->pGlob->Prm.Timer.InUse      = LSA_TRUE;

					}
					else
					{
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"<-- Calling EDDS_ALLOC_TIMER failed (Response: 0x%X).",RetVal);
						EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot alloc timer (0x%X)!",RetVal);
						Status = EDD_STS_ERR_NO_TIMER;
					}
				}
				#endif

				/*---------------------------------------------------------------*/
				/* The following is very ethernethardware dependend              */
				/*---------------------------------------------------------------*/

				if ( Status == EDD_STS_OK )
				{

					EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_OPEN(pDDB: 0x%X,pDPB: 0x%X)",pDDB,pDPB);

					Status = EDDS_LL_OPEN(pDDB,pDDB->hDDB,pDPB,pDDB->pGlob->TraceIdx);

					EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_OPEN(). Status: 0x%X",Status);

				}
			}
		}
        else
        {
            EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_DeviceOpen: Cannot create DDB (0x%X)!",Status);
        }

        /*-----------------------------------------------------------------------*/
        /* If an error occured we close the DDB (must be present here)           */
        /*-----------------------------------------------------------------------*/

        if (( Status != EDD_STS_OK ) && ( DDBInit))
        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            if ( TimerInit )
            {
                EDDS_TimerFree(pDDB->pGlob->TxTimer.EDDSTimerID);
                pDDB->pGlob->TxTimer.InUse      = LSA_FALSE;
            }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

            if ( LLTimerInit )
            {
                EDDS_TimerFree(pDDB->pGlob->LLTimer.EDDSTimerID);
                pDDB->pGlob->LLTimer.InUse      = LSA_FALSE;
            }

            #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
            if ( RxTimerInit )
            {
                EDDS_TimerFree(pDDB->pGlob->RxOverload.Timer.EDDSTimerID);
                pDDB->pGlob->RxOverload.Timer.InUse = LSA_FALSE;
            }
            #endif

            #ifdef EDDS_CFG_LED_BLINK_SUPPORT
            if ( LEDTimerInit )
            {
                EDDS_TimerFree(pDDB->pGlob->LEDBLink.Timer.EDDSTimerID);
                pDDB->pGlob->LEDBLink.Timer.InUse = LSA_FALSE;
            }
            #endif

            #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
            if ( DiagTimerInit )
            {
                EDDS_TimerFree(pDDB->pGlob->Prm.Timer.EDDSTimerID);
                pDDB->pGlob->Prm.Timer.InUse = LSA_FALSE;
            }
            #endif

            EDDS_CloseDDB(pRqbDDB->hDDB);
        }

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_OPEN finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceOpen()");


}
//lint +esym(613,pRQB,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceSetup                            +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     (not used)                      +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Component-ID (not used)         +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_SETUP              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_SETUP_TYPE                               +*/
/*+                                                                         +*/
/*+     hDDB     : Device-Handle                                            +*/
/*+     pDSB     : Pointer to device-specific setup parameters              +*/
/*+                (input - variables !)                                    +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         EDD_STS_ERR_HW                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to setup a device. This request has to be called  +*/
/*+               after ab open_device-call and all components are init.    +*/
/*+               This call setup the ethernet hardware.                    +*/
/*+               because this may cause interrupts, interrupts             +*/
/*+               have to be enabled before calling.                        +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceSetup(EDD_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_SETUP_PTR_TYPE   pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_UPPER_DSB_PTR_TYPE         pDSB;   /* Pointer to device setup-parameter*/
    EDD_RQB_GET_LINK_STATUS_TYPE    LinkStatus;
    LSA_UINT32                      i;
    LSA_UINT16                      MAUType;
    LSA_UINT8                       MediaType;
    LSA_UINT32                      PortStatus;
    LSA_UINT8                       PhyStatus;
    LSA_UINT32                      AutonegCapAdvertised;
    LSA_UINT8                       LinkSpeedModeConfigured;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceSetup(pRQB: 0x%X)",
                           pRQB);

    Status = EDD_STS_OK;

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceSetup: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        pRqbDDB = (EDDS_UPPER_DDB_SETUP_PTR_TYPE)pRQB->pParam;

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        if ( Status == EDD_STS_OK )
        {
        	Status = EDDS_LL_GET_CAPS(pDDB, &(pDDB->pGlob->HWParams.Caps));
        }

        if ( Status == EDD_STS_OK )
        	{
				pDSB = pRqbDDB->pDSB;      /* pointer to hardwarespezific parameters */

				/* Check if already setup or not all components initialized. if so   */
				/* we signal a sequence-error                                        */

				if (( pDDB->pGlob->HWIsSetup )
					#ifdef EDDS_CFG_NRT_INCLUDE
					|| ( LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL) )
					#endif
					#ifdef EDDS_CFG_SRT_INCLUDE
				||((pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
					&& ( LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) )
					)
					#endif
				   )
				{
					Status = EDD_STS_ERR_SEQUENCE;
				}
				else
				{
                    #if defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
				    LSA_USER_ID_TYPE Dummy;
				    LSA_UINT16       RetVal;
					Dummy.uvar32  = 0;
					RetVal        = 0;

					/*---------------------------------------------------------------*/
					/* start Rx Overload Timer if configured.                        */
					/*---------------------------------------------------------------*/

					pDDB->pGlob->RxOverload.Status   = EDD_NO_RX_OVERLOAD;
					pDDB->pGlob->RxOverload.RxCnt    = 0;
					pDDB->pGlob->RxOverload.RxOFF    = LSA_FALSE;
					pDDB->pGlob->RxOverload.OFFCnt   = 0;
					pDDB->pGlob->RxOverload.OFFLevel = 0;
					pDDB->pGlob->RxOverload.ONCnt    = 0;/* on startup we are ok */

					if(EDDS_LL_AVAILABLE(pDDB,disableRX))
					{

						Dummy.uvar32 = pDDB->pGlob->RxOverload.Timer.EDDSTimerID;

						EDDS_START_TIMER(&RetVal,
										 pDDB->pGlob->RxOverload.Timer.TimerID,
										 Dummy,
										 EDDS_CFG_RX_OVERLOAD_CHECK_FACTOR);

						if ( RetVal != LSA_RET_ERR_PARAM )
						{
							pDDB->pGlob->RxOverload.Timer.InProgress = LSA_TRUE;
						}
						else
						{
							EDDS_FatalError(EDDS_FATAL_ERR_TIMER_START,
											EDDS_MODULE_ID,
											__LINE__);
						}
					}
					#endif /* defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
					/*---------------------------------------------------------------*/
					/* Init LED Blink management.                                    */
					/*---------------------------------------------------------------*/

					pDDB->pGlob->LEDBLink.Status         = EDDS_LED_BLINK_INACTIVE;

					/*---------------------------------------------------------------*/
					/* start DIAG Timer                                              */
					/*---------------------------------------------------------------*/

					#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
					if(EDDS_LL_AVAILABLE(pDDB,getSwitchDroppedFrames))
					{
	                    LSA_USER_ID_TYPE Dummy;
	                    LSA_UINT16       RetVal;

						Dummy.uvar32 = pDDB->pGlob->Prm.Timer.EDDSTimerID;

						EDDS_START_TIMER(&RetVal,
										 pDDB->pGlob->Prm.Timer.TimerID,
										 Dummy,
										 1);   /* one second timer */

						if ( RetVal != LSA_RET_ERR_PARAM )
						{
							pDDB->pGlob->Prm.Timer.InProgress = LSA_TRUE;
						}
						else
						{
							EDDS_FatalError(EDDS_FATAL_ERR_TIMER_START,
											EDDS_MODULE_ID,
											__LINE__);
						}
					}
					#endif

					/*---------------------------------------------------------------*/
					/* This parameters will be copied to DDB because they may be needed */
					/* when we have to (re)setup the ethernetcontroller.                */
					/*---------------------------------------------------------------*/

					pDDB->pGlob->DSBParams = *pDSB; /* save DSB in management */

					EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SETUP(pDDB: 0x%X,pDSB: 0x%X)",pDDB,pDSB);

					Status = EDDS_LL_SETUP(pDDB,pDSB);

					EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SETUP(). Status: 0x%X",Status);

					if ( Status == EDD_STS_OK )
					{

						EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_MAC_ADDR(pDDB: 0x%X (Interface))",pDDB);

						EDDS_LL_GET_MAC_ADDR(pDDB,0,&pDDB->pGlob->HWParams.MACAddress);

						EDDS_LOWER_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_MAC_ADDR()");

						/* MAC Address shall not be 0! */
						if ( EDDS_MACADDR_IS_0(pDDB->pGlob->HWParams.MACAddress))
						{
							EDDS_FatalError( EDDS_FATAL_ERR_LL,
											 EDDS_MODULE_ID,
											 __LINE__);
						}

						/* If LLIF is configured with dynamic port cnt we get the actual */
						/* PortCnt (if not we use EDDS_MAX_PORT_CNT, see device open)    */

						EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_PORT_CNT(pDDB: 0x%X (Interface))",pDDB);

						EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_PORT_CNT(PortCnt: %d)",pDDB->pGlob->HWParams.Caps.PortCnt);

						/* Check for Valid PortCnt */
						if (( pDDB->pGlob->HWParams.Caps.PortCnt < 1  ) ||
							( pDDB->pGlob->HWParams.Caps.PortCnt > EDDS_MAX_PORT_CNT ))
						{
							EDDS_FatalError( EDDS_FATAL_ERR_LL,
											 EDDS_MODULE_ID,
											 __LINE__);
						}

						pDDB->pGlob->HWIsSetup = LSA_TRUE;

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
						/* this should be moved to EDDS_DeviceOpen;
						 * this could be done, if EDDS_LL_OPEN returns the capabilities
						 * (-->EDDS_LL_GET_CAPS() will be removed from LLIF API)
						 */
					    /*if ( Status == EDD_STS_OK )*/
					    {
					        /* allocate sMIB structure for local interface and all ports;
					         * this will be an array.
					         */
					        EDDS_ALLOC_LOCAL_FAST_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pDDB->pGlob->sMIB, (pDDB->pGlob->HWParams.Caps.PortCnt+1) * sizeof(*(pDDB->pGlob->sMIB)));

					        if ( LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
					        {
					            Status = EDD_STS_ERR_RESOURCE;
					        }
					        else EDDS_MEMSET_LOCAL(pDDB->pGlob->sMIB,0,sizeof(*(pDDB->pGlob->sMIB)));
					    }
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

						for ( i=1; i<= pDDB->pGlob->HWParams.Caps.PortCnt; i++)
						{
							/* get Port MAC Adresses */
							EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE_SETUP(pDDB: 0x%X, Port: %d)",pDDB,i);

							EDDS_LL_GET_LINK_STATE_SETUP(pDDB,i,
														 &pDDB->pGlob->HWParams.LinkSpeedMode[i-1],
														 &pDDB->pGlob->HWParams.PHYPower[i-1],
														 &pDDB->pGlob->HWParams.IsWireless[i-1]);  //@fixme: param check missing

							EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE_SETUP(LinkSpeedMode: %d, Power: %d)",
												pDDB->pGlob->HWParams.LinkSpeedMode[i-1],
											pDDB->pGlob->HWParams.PHYPower[i-1]);

							/* get Port MAC Adresses */
							EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_MAC_ADDR(pDDB: 0x%X, Port: %d)",pDDB,i);

							EDDS_LL_GET_MAC_ADDR(pDDB,i,&pDDB->pGlob->HWParams.MACAddressPort[i-1]);

							EDDS_LOWER_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_MAC_ADDR()");

							/* MAC Address shall not be 0! */
							if ( EDDS_MACADDR_IS_0(pDDB->pGlob->HWParams.MACAddressPort[i-1]))
							{
								EDDS_FatalError( EDDS_FATAL_ERR_LL,
												 EDDS_MODULE_ID,
												 __LINE__);
							}

							/* Setup PhyStatus Variable for Link */
							if ( pDDB->pGlob->HWParams.PHYPower[i-1] == EDDS_PHY_POWER_ON )
							{
								PhyStatus = EDD_PHY_STATUS_ENABLED;
							}
							else
							{
								PhyStatus = EDD_PHY_STATUS_DISABLED;
							}

							/* Get actual Link-Status for the first time */

							EDDS_LOWER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_GET_LINK_STATE(pDDB: 0x%X, Port: %d)",pDDB,i);

							/* NOTE: LinkSpeedModeConfigured is not used from this call to LL_GET_LINK_STATE */
							/*       LinkSpeedMode already determined by EDDS_LL_GET_LINK_STATE_SETUP        */
							if ( EDDS_LL_GET_LINK_STATE(pDDB,i,&LinkStatus,&MAUType,&MediaType,&PortStatus,
														&AutonegCapAdvertised,&LinkSpeedModeConfigured) != EDD_STS_OK )
							{
								EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"<== EDDS_LL_GET_LINK_STATE(Port: %d). Status <> EDD_STS_OK",i);

								LinkStatus.Mode   = EDD_LINK_UNKNOWN;
								LinkStatus.Speed  = EDD_LINK_UNKNOWN;
								LinkStatus.Status = EDD_LINK_DOWN;   /* On error we signal link down */
								MAUType           = EDD_MAUTYPE_UNKNOWN;
								MediaType         = EDD_MEDIATYPE_UNKNOWN;
								PortStatus        = EDD_PORT_NOT_PRESENT;
								AutonegCapAdvertised     = 0;   /* default: No Capability */
							}
							else
							{
								EDDS_LOWER_TRACE_04(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(Port: %d, Mode: %d,Speed: %d, Status: %d)",
													i,
													LinkStatus.Mode,
													LinkStatus.Speed,
													LinkStatus.Status);

								EDDS_LOWER_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(PMAUType: %d, MediaType: %d,PortStatus: %d",
													MAUType,
													MediaType,
													PortStatus);

								EDDS_LOWER_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_GET_LINK_STATE(AutonegCap: 0x%X, LinkSpeedModeConfigured: 0x%X, Status : 0x%X",
													AutonegCapAdvertised,
													LinkSpeedModeConfigured,
													Status);
							}

							/* copy status to actual status */

							pDDB->pGlob->LinkStatus[i].Status  = LinkStatus.Status;
							pDDB->pGlob->LinkStatus[i].Mode    = LinkStatus.Mode;
							pDDB->pGlob->LinkStatus[i].Speed   = LinkStatus.Speed;

							/* Setup Autoneg setting */
							if ( pDDB->pGlob->HWParams.LinkSpeedMode[i-1] == EDD_LINK_AUTONEG )
							{
								pDDB->pGlob->LinkStatus[i].Autoneg = EDD_AUTONEG_ON;
							}
							else
							{
								pDDB->pGlob->LinkStatus[i].Autoneg = EDD_AUTONEG_OFF;
							}

							pDDB->pGlob->LinkStatus[i].MAUType        = MAUType;
							pDDB->pGlob->LinkStatus[i].MediaType      = MediaType;
							pDDB->pGlob->LinkStatus[i].PortStatus     = (LSA_UINT8) PortStatus;
							pDDB->pGlob->LinkStatus[i].PortState      = (LSA_UINT16) EDD_PORT_STATE_FORWARDING;
							pDDB->pGlob->LinkStatus[i].PhyStatus      = (LSA_UINT8) PhyStatus;
							pDDB->pGlob->LinkStatus[i].CableDelayNs   = 0;
							pDDB->pGlob->LinkStatus[i].LineDelay      = 0;
							pDDB->pGlob->LinkStatus[i].AutonegCapAdvertised    = AutonegCapAdvertised;

						} /* for */

						#ifndef EDDS_DONT_USE_MRP
						#ifdef LLIF_CFG_SWITCH_SUPPORT
						/* If MRP is used set the PortState of R-Ports to ON     */
						/* Note that the LLIF must set the Portstate to BLOCKING */
						/* for R-Ports in startup!                               */
						if ( 0 != pDDB->SWI.MaxMRP_Instances )
						{
							LSA_INT idx;
							for(idx=0;idx < EDDS_MAX_PORT_CNT; ++idx)
							{
								if(EDD_MRP_RING_PORT_DEFAULT == pDDB->SWI.MRPRingPort[idx])
								{
								  // NOTE: index 0 = Auto
									pDDB->pGlob->LinkStatus[idx+1].PortState = (LSA_UINT16) EDD_PORT_STATE_BLOCKING;
								}
							}
						}
						#endif
						#endif
						/* init AUTO mode setting. This is the fastest mode present on any port */
						EDDS_FindLinkAutoMode(pDDB);  //lint !e534 JB 12/11/2014 ret val has no meaning (here)

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

					}
					else
					{
						#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
						if(EDDS_LL_AVAILABLE(pDDB,disableRX))
						{
							/* on error stop timer */
							LSA_UINT16 RetVal;

							EDDS_STOP_TIMER(&RetVal,
											pDDB->pGlob->RxOverload.Timer.TimerID);

							pDDB->pGlob->RxOverload.Timer.InProgress = LSA_FALSE;
						}
						#endif
						#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
						if(EDDS_LL_AVAILABLE(pDDB,getSwitchDroppedFrames))
						{
							/* on error stop timer */
							LSA_UINT16 RetVal;

							EDDS_STOP_TIMER(&RetVal,
											pDDB->pGlob->Prm.Timer.TimerID);

							pDDB->pGlob->Prm.Timer.InProgress = LSA_FALSE;
						}
						#endif
					}
				}
        	}

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_SETUP finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceSetup()");

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceShutdown                         +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     (not used)                      +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Component-ID (not used)         +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_SHUTDOWN           +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_SETUP_TYPE                               +*/
/*+                                                                         +*/
/*+     hDDB     : Device-Handle                                            +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to shutdown device. This request has to be called +*/
/*+               before closing a device. This function stops the ethernet +*/
/*+               hardware. after this no more interrupts occur. A new      +*/
/*+               DeviceSetup is required to continue.                      +*/
/*+                                                                         +*/
/*+               The request is finished by calling the spezified callback +*/
/*+               function. If NULL no callback-function is called and      +*/
/*+               the status is set on function return.                     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DeviceShutdown(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceShutdown(pRQB: 0x%X)",
                           pRQB);

    Status    = EDD_STS_OK;

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceShutdown: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        pRqbDDB = (EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE)pRQB->pParam;

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        if ( Status == EDD_STS_OK )
        {

            if (  EDDS_HandleSearchDDB(pDDB) || /* does handle use DDB ?    */
                  ( ! pDDB->pGlob->HWIsSetup ))   /* and hw is setup     ?    */
                Status = EDD_STS_ERR_SEQUENCE;  /* yes-> close handle first */
            else
            {

                /*---------------------------------------------------------------*/
                /* The following is very ethernethardware dependend              */
                /* We shutdown the device.                                       */
                /* We set HWIsSetup before to avoid further sends to be done     */
                /* while shutting down.                                          */
                /*---------------------------------------------------------------*/

                pDDB->pGlob->HWIsSetup = LSA_FALSE;

                EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_SHUTDOWN(pDDB: 0x%X)",pDDB);

                Status = EDDS_LL_SHUTDOWN(pDDB);

                EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_SHUTDOWN(). Status: 0x%X",Status);

                if ( Status != EDD_STS_OK )  /* dont expecting failure here */
                {
                    EDDS_FatalError( EDDS_FATAL_ERR_LL,
                                     EDDS_MODULE_ID,
                                     __LINE__);
                }

                if ( pDDB->pGlob->LLTimer.InProgress )
                {
                    LSA_UINT16 RetVal;

                    EDDS_STOP_TIMER(&RetVal,
                                    pDDB->pGlob->LLTimer.TimerID);

                    pDDB->pGlob->LLTimer.InProgress = LSA_FALSE;
                }

                #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
                /* Stop rx-overload check timer */
				if ( pDDB->pGlob->RxOverload.Timer.InProgress )
				{
					LSA_UINT16 RetVal;

					EDDS_STOP_TIMER(&RetVal,
									pDDB->pGlob->RxOverload.Timer.TimerID);

					pDDB->pGlob->RxOverload.Timer.InProgress = LSA_FALSE;
				}
                #endif

                #ifdef EDDS_CFG_LED_BLINK_SUPPORT
                /* Stop LEB Blink timer */
                if ( pDDB->pGlob->LEDBLink.Timer.InProgress )
                {
                    LSA_UINT16 RetVal;

                    EDDS_STOP_TIMER(&RetVal,
                                    pDDB->pGlob->LEDBLink.Timer.TimerID);

                    pDDB->pGlob->LEDBLink.Timer.InProgress = LSA_FALSE;
                }
                #endif

                #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
                /* Stop Diag timer */
                if ( pDDB->pGlob->Prm.Timer.InProgress )
                {
                    LSA_UINT16 RetVal;

                    EDDS_STOP_TIMER(&RetVal,
                                    pDDB->pGlob->Prm.Timer.TimerID);

                    pDDB->pGlob->Prm.Timer.InProgress = LSA_FALSE;
                }
                #endif

                /* now no more interrupts. */

                /*---------------------------------------------------------------*/
                /* Because it is possible that there are some internal tx        */
                /* requests pending we have to shutdown the tx-handling.         */
                /*---------------------------------------------------------------*/

                EDDS_TransmitShutdown(pDDB);

            }

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            /* this should be moved to EDDS_DeviceClose;
             * this could be done, if the allocation of the sMIB array will be done in EDDS_LL_OPEN.
             * \see commend above in EDDS_LL_OPEN
             */
            {
                LSA_UINT16                      FreeStat;
                /* free sMIB structure for local interface and all ports
                 */
                EDDS_FREE_LOCAL_FAST_MEM(&FreeStat,pDDB->pGlob->sMIB);
                EDDS_FREE_CHECK(FreeStat);
                pDDB->pGlob->sMIB = LSA_NULL;
            }
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
        }

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_SHUTDOWN finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceShutdown()");

}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DeviceClose                            +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_CLOSE              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_DDB_REL_TYPE                                 +*/
/*+                                                                         +*/
/*+     hDevice  : Valid Device-Handle from device-open                     +*/
/*+     Cbf      : Call-back-function. Called when Device-open is finished. +*/
/*+                LSA_NULL if not used.                                    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:                                    +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDDS_STS_ERR_REF                +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDD_STS_ERR_TIMEOUT             +*/
/*+                                         or others                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to close device. This request finishes all        +*/
/*+               device-handling by terminating all pending requests and   +*/
/*+               shut down the device.                                     +*/
/*+                                                                         +*/
/*+               If handles still open, we return EDD_STS_ERR_SEQUENCE.    +*/
/*+                                                                         +*/
/*+               When closing the device, the systemadaption have to make  +*/
/*+               sure, that no more interrupts come in for this device.    +*/
/*+               (i.e. with this DDB)                                      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_TIMEOUT is set if a timeout on hardwareaccess +*/
/*+               occured, so the hardware couldnt shut down. but the       +*/
/*+               device managment was closed.                              +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//checked within EDDS_ENTER_CHECK_REQUEST
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DeviceClose(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_DDB_REL_PTR_TYPE     pRqbDDB;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_DeviceClose(pRQB: 0x%X)",
                           pRQB);

    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_DeviceClose: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbDDB = (EDDS_UPPER_DDB_REL_PTR_TYPE)pRQB->pParam;

        /* Here we have to check if any handle uses this device */
        /* if so we cant close the device.                      */

        Status = EDDS_GetDDB(pRqbDDB->hDDB, &pDDB);

        if ( Status == EDD_STS_OK )
        {
            /*----------------------------------------------------------*/
            /* check if a RQB is in use (e.g. pending in the RQB-queue) */
            /* this must not occur. If so we signal an sequence-error.  */
            /* the systemadaption must make sure, that no request from  */
            /* isr are pending when closing the device!!                */
            /* (e.g disable interrupts. clear queue. then close device) */
            /*----------------------------------------------------------*/

            if (
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                ( pDDB->pGlob->RQBInt_Tx.InUse )      ||
                ( pDDB->pGlob->RQBInt_Tx_Tout.InUse ) ||
                ( pDDB->pGlob->RQBInt_Rx_NRT.InUse )  ||
                ( pDDB->pGlob->RQBInt_Rx_cSRT.InUse ) ||
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
                ( pDDB->pGlob->RQBInt_Rx_OverTimer.InUse ) ||
                ( pDDB->pGlob->RQBInt_Rx_OverEvent.InUse ) ||
                #endif
                #ifdef EDDS_CFG_LED_BLINK_SUPPORT
                ( pDDB->pGlob->RQBInt_LED_BlinkTimer.InUse ) ||
                #endif
                #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
                ( pDDB->pGlob->RQBInt_DiagTimer.InUse ) ||
                #endif
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                ( pDDB->pGlob->RQBInt_PHY.InUse ) ||
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                ( pDDB->pGlob->RQBInt_LL_Tout.InUse )
                )
            {
                Status = EDD_STS_ERR_SEQUENCE;
            }
        }

        if ( Status == EDD_STS_OK )
        {
            if (( EDDS_HandleSearchDDB(pDDB) ) || /* does handle use DDB ? or */
                ( pDDB->pGlob->HWIsSetup   ))   /* HW-not shutdown yet ?    */
            {
                Status = EDD_STS_ERR_SEQUENCE;  /* yes-> close handle first */
            }
            else
            {

                if (( LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL) ) &&
                    ( LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) ))
                {

                    /*---------------------------------------------------------------*/
                    /* The following is very ethernethardware dependend              */
                    /* We shutdown the device before removing the DDB-Info.          */
                    /* (It may be possible, that an request with an old DDB is queued*/
                    /* to the driver. This is checked on entry to RQB-handling.      */
                    /*---------------------------------------------------------------*/

                    EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_CLOSE(pDDB: 0x%X)",pDDB);

                    Status = EDDS_LL_CLOSE(pDDB);

                    EDDS_LOWER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_CLOSE(). Status: 0x%X",Status);

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    /*---------------------------------------------------------------*/
                    /* Stop and free tx-timer if used (even on error)                */
                    /* (Stop can be called if timer dont running, so no special check*/
                    /* needed)                                                       */
                    /*---------------------------------------------------------------*/

                    if ( pDDB->pGlob->TxTimer.InUse )
                    {
                        pDDB->pGlob->TxTimer.InUse      = LSA_FALSE;
                        EDDS_TimerFree(pDDB->pGlob->TxTimer.EDDSTimerID);
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

                    if ( pDDB->pGlob->LLTimer.InUse )
                    {
                        pDDB->pGlob->LLTimer.InUse      = LSA_FALSE;
                        EDDS_TimerFree(pDDB->pGlob->LLTimer.EDDSTimerID);
                    }

                    #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
                    if ( pDDB->pGlob->RxOverload.Timer.InUse )
                    {
                        pDDB->pGlob->RxOverload.Timer.InUse = LSA_FALSE;
                        EDDS_TimerFree(pDDB->pGlob->RxOverload.Timer.EDDSTimerID);
                    }
                    #endif

                    #ifdef EDDS_CFG_LED_BLINK_SUPPORT
                    if ( pDDB->pGlob->LEDBLink.Timer.InUse )
                    {
                        pDDB->pGlob->LEDBLink.Timer.InUse = LSA_FALSE;
                        EDDS_TimerFree(pDDB->pGlob->LEDBLink.Timer.EDDSTimerID);
                    }
                    #endif

                    #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
                    if ( pDDB->pGlob->Prm.Timer.InUse )
                    {
                        pDDB->pGlob->Prm.Timer.InUse = LSA_FALSE;
                        EDDS_TimerFree(pDDB->pGlob->Prm.Timer.EDDSTimerID);
                    }
                    #endif

                    /* hDDB was already checked for validness..                      */

                    EDDS_CloseDDB(pRqbDDB->hDDB);

                }
                else Status = EDD_STS_ERR_SEQUENCE; /* components not closed */

            }
        }

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_CLOSE finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbDDB->Cbf,pRQB);
    }


    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_DeviceClose()");

}
//lint +esym(613,pRQB)

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_system                                 +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:     Request-dependend               +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_xxxx               +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_DEV_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values in Requestblock                                      +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     unchanged                       +*/
/*+     EDD_SERVICE             Service:    unchanged                       +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         others                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles devicerequest which will be used in systemadaption+*/
/*+               This functions have a RQB-Parameter structure.            +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//check pRQB
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_system(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    #ifdef EDDS_CFG_REENTRANCE_CHECK
    LSA_UINT32     ReentrPrio = EDDS_RENTR_PRIO_NONE;
    #endif

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_system(pRQB: 0x%X)",
                           pRQB);

    EDDS_ASSERT_NULL_PTR(pRQB);
    EDDS_ENTER_CHECK_REQUEST(pRQB,&ReentrPrio);

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM )
    {

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_system: Invalid opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                /*-----------------------------------------------------------------------*/
                /* Interrupt-request from edds_interrupt.                                */
                /*-----------------------------------------------------------------------*/
            case EDDS_SRV_DEV_INTERRUPT:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_INTERRUPT (pRQB: 0x%X)",pRQB);
                EDDS_InterruptRequest(pRQB);
                EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_INTERRUPT finished.");
                break;
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

                /*-----------------------------------------------------------------------*/
                /* Timeout-request from edds_timeout                                     */
                /*-----------------------------------------------------------------------*/
            case EDDS_SRV_DEV_TIMEOUT:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_TIMEOUT (pRQB: 0x%X)",pRQB);
                EDDS_TimeoutRequest(pRQB);
                EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_TIMEOUT finished.");
                break;

                /*-----------------------------------------------------------------------*/
                /* Trigger-request from edds                                             */
                /*-----------------------------------------------------------------------*/
            case EDDS_SRV_DEV_TRIGGER:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_TRIGGER (pRQB: 0x%X)",pRQB);
                EDDS_TriggerRequest(pRQB);
                EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_TRIGGER finished.");
                break;

                /*-----------------------------------------------------------------------*/
                /* Device open/close/setup/shutdown etc                                  */
                /*-----------------------------------------------------------------------*/

            case EDDS_SRV_DEV_OPEN:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_OPEN (pRQB: 0x%X)",pRQB);
                EDDS_DeviceOpen(pRQB);
                break;

            case EDDS_SRV_DEV_SETUP:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_SETUP (pRQB: 0x%X)",pRQB);
                EDDS_DeviceSetup(pRQB);
                break;

            case EDDS_SRV_DEV_SHUTDOWN:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_SHUTDOWN (pRQB: 0x%X)",pRQB);
                EDDS_DeviceShutdown(pRQB);
                break;

            case EDDS_SRV_DEV_CLOSE:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_CLOSE (pRQB: 0x%X)",pRQB);
                EDDS_DeviceClose(pRQB);
                break;

                /*-----------------------------------------------------------------------*/
                /* Setup link.                                                           */
                /*-----------------------------------------------------------------------*/

            case EDDS_SRV_DEV_SETUP_LINK:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_SETUP_LINK (pRQB: 0x%X)",pRQB);
                EDDS_SetupLinkRequest(pRQB);
                break;

                /*-----------------------------------------------------------------------*/
                /* NRT-Component                                                         */
                /*-----------------------------------------------------------------------*/

                #ifdef EDDS_CFG_NRT_INCLUDE
            case EDDS_SRV_DEV_COMP_NRT_INI:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_COMP_NRT_INI (pRQB: 0x%X)",pRQB);
                EDDS_NRTInitComponent(pRQB);
                break;
            case EDDS_SRV_DEV_COMP_NRT_REL:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_COMP_NRT_REL (pRQB: 0x%X)",pRQB);
                EDDS_NRTRelComponent(pRQB);
                break;
                #endif

                /*-----------------------------------------------------------------------*/
                /* SRT-Component                                                         */
                /*-----------------------------------------------------------------------*/

                #ifdef EDDS_CFG_SRT_INCLUDE
            case EDDS_SRV_DEV_COMP_SRT_INI:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_COMP_SRT_INI (pRQB: 0x%X)",pRQB);
                EDDS_RTInitComponent(pRQB);
                break;
            case EDDS_SRV_DEV_COMP_SRT_REL:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"--> Request: EDDS_SRV_DEV_COMP_SRT_REL (pRQB: 0x%X)",pRQB);
                EDDS_RTRelComponent(pRQB);
                break;
                #endif

                /*-----------------------------------------------------------------------*/
                /* SWI-Component                                                         */
                /*-----------------------------------------------------------------------*/

            default:
                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_system: Invalid service (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

                EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_SERVICE);
                EDDS_RQB_ERROR(pRQB);
                break;

        } /* switch */
    } /* else */


    EDDS_EXIT_CHECK_REQUEST(ReentrPrio);

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_system()");


}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_open_channel                           +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_OPEN_CHANNEL            +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDD_RQB_OPEN_CHANNEL_TYPE                             +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         Handle:         channel-handle of EDDS (ret) +*/
/*+     LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user      +*/
/*+     LSA_SYS_PATH_TYPE       SysPath:        system-path of channel      +*/
/*+     LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)    +*/
/*+                                         (EDD_UPPER_RQB_PTR_TYPE pRQB)   +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_OPEN_CHANNEL            +*/
/*+     LSA_HANDLE_TYPE         Handle:     returned channel of user        +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_RESOURCE            +*/
/*+                                         EDD_STS_ERR_SYS_PATH            +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDDS_STS_ERR_LOWER_LAYER        +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel of a device.                    +*/
/*+                                                                         +*/
/*+               Within this request the output - macro EDDS_GET_PATH_INFO +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+               (e.g. the device-handle).                                 +*/
/*+                                                                         +*/
/*+               After a successful open a handle is channel-handle is     +*/
/*+               returned and channel-dependend requests can be used.      +*/
/*+                                                                         +*/
/*+               HandleUpper:                                              +*/
/*+                                                                         +*/
/*+               This handle will be returned in the RQB in all further    +*/
/*+               request-confirmations.                                    +*/
/*+                                                                         +*/
/*+               cbf:                                                      +*/
/*+                                                                         +*/
/*+               The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-funktion will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+               SysPath:                                                  +*/
/*+                                                                         +*/
/*+               The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via EDDS_GET_PATH_INFO.                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pRQBChannel,pDetail,pDDB)
//pRQBChannel,pDDB and pDetail are set within this function - future called are checked using Status
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID EDD_UPPER_IN_FCT_ATTR  edds_open_channel(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_HDB_PTR_TYPE         pHDB;
    LSA_SYS_PTR_TYPE                pSys = {0};
    EDDS_DETAIL_PTR_TYPE            pDetail;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE pRQBChannel;
    LSA_UINT16                      PathStat;
    LSA_BOOL                        HandleInit;
    LSA_BOOL                        SysPathInit;
    LSA_UINT16                      RelStat;
    LSA_BOOL						do_release_path_info = LSA_FALSE;
    #ifdef EDDS_CFG_REENTRANCE_CHECK
    LSA_UINT32                      ReentrPrio = EDDS_RENTR_PRIO_NONE;
    #endif


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_open_channel(pRQB: 0x%X)",
                           pRQB);

	//@fixme am2219 check pRQB != null - NEA6.0
    EDDS_ENTER_CHECK_REQUEST(pRQB,&ReentrPrio);

    HandleInit  = LSA_FALSE;
    SysPathInit = LSA_FALSE;
    Status      = EDD_STS_OK;
    pRQBChannel = LSA_NULL;
    pDetail     = LSA_NULL;  /* access prevents compiler warning */
    pDDB        = LSA_NULL;  /* access prevents compiler warning */

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_OPEN_CHANNEL )
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,">>> edds_open_channel: Invalid RQB-Opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        Status = EDD_STS_ERR_OPCODE;
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_OPC_OPEN_CHANNEL (Handle: 0x%X)",EDD_RQB_GET_HANDLE(pRQB));

        if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
        {

            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"edds_open_channel: pParam is NULL");

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
        else
        {

            pRQBChannel = (EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)pRQB->pParam;

            if ( LSA_HOST_PTR_ARE_EQUAL(pRQBChannel->Cbf, LSA_NULL) )
            {

                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"edds_open_channel: Cbf is NULL");

                EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
                Status = EDD_STS_ERR_PARAM;
                EDDS_RQB_ERROR(pRQB);
            }
        }
    }


    /*---------------------------------------------------------------------------*/
    /* If parameters are valid, we have a call-back-function now.                */
    /*---------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {

        /*-----------------------------------------------------------------------*/
        /* Get a Handle. On error abort                                          */
        /*-----------------------------------------------------------------------*/

        Status = EDDS_HandleNew(&pHDB);

        if (Status == EDD_STS_OK)
        {
            HandleInit = LSA_TRUE;

            /* ini HDB (Handle-Discription-Block) */

            pHDB->InUse         = LSA_TRUE;
            pHDB->UsedComp      = 0;
            pHDB->SysPath       = pRQBChannel->SysPath;
            pHDB->UpperHandle   = pRQBChannel->HandleUpper;
            //pHDB->Handle        = Handle;		// am2219: this should be not necessary (see EDDS_HandleNew() ).
            pHDB->Cbf           = pRQBChannel->Cbf;
            pHDB->pDDB          = LSA_NULL;

            /* default */
            pHDB->pNRT          = LSA_NULL;
        }
        else
        {
            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: Cannot get free EDDS handle");
        }

        /*-----------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                               */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {

            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_GET_PATH_INFO.");

            EDDS_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQBChannel->SysPath);
            if (PathStat != LSA_RET_OK)
            {

                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"<-- EDDS_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: EDDS_GET_PATH_INFO failed");

                Status = EDD_STS_ERR_SYS_PATH;
            }
            else
            {

                EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);

                SysPathInit         = LSA_TRUE;

                pHDB->pSys          = pSys;
                pHDB->pDetail       = pDetail;

                if (LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL))
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: EDDS_GET_PATH_INFO() pDetail is NULL");

                    Status = EDD_STS_ERR_PARAM;
                }

            }

        }

        /*-----------------------------------------------------------------------*/
        /* Get pointer to DDB      ..                                            */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            Status = EDDS_GetDDB(pDetail->hDDB, &pDDB);

            if (Status != EDD_STS_OK)
            {

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: Invalid hDBB (0x%X) within CDB",pDetail->hDDB);

                Status = EDD_STS_ERR_PARAM;
            }
            else
            {
                if (pDDB->pGlob->HWIsSetup ) /* check if Device is setup.. */
                {
                    pHDB->pDDB          = pDDB;
                    pHDB->InsertSrcMAC  = pDetail->InsertSrcMAC;
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: EDDS not SETUP yet!");

                    Status = EDD_STS_ERR_SEQUENCE;
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   PRM Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
            if (pDetail->UsedComp.UsePRM)
            {
                /* only one channel supported per device */
                if ( pDDB->pGlob->Prm.HandleCnt == 0 )
                {
                    /* check if component is initialized */
                    if  ( !LSA_HOST_PTR_ARE_EQUAL(pDetail->pPRM, LSA_NULL) )
                    {
                        pHDB->UsedComp |= EDDS_COMP_PRM;
                        pDDB->pGlob->Prm.HandleCnt = 1;
                        pDDB->pGlob->Prm.Handle    = pHDB->Handle;
                        pDDB->pGlob->Prm.PortData.LesserCheckQuality = pDetail->pPRM->PortDataAdjustLesserCheckQuality;
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: pPRM is NULL!");
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                else
                {
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init   NRT Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
            if (pDetail->UsedComp.UseNRT)
            {
                #ifdef EDDS_CFG_NRT_INCLUDE

                /* check if component is initialized */

                if  (( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL)) &&
                     ( !LSA_HOST_PTR_ARE_EQUAL(pDetail->pNRT, LSA_NULL)) )
                {

                    /* open channel for NRT */

                    Status = EDDS_NRTOpenChannel( pHDB,
                                                  pDetail->pNRT);

                    if ( Status == EDD_STS_OK )
                    {
                        pHDB->UsedComp |= EDDS_COMP_NRT;
                    }

                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: NRT component not initialized yet!");
                    Status = EDD_STS_ERR_SEQUENCE;
                }
                #else
                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: NRT not supported!");
                Status = EDD_STS_ERR_PARAM;
                #endif

            }
        }

        /*-----------------------------------------------------------------------*/
        /* Init  cSRT Component.                                                 */
        /*-----------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {
        	if(	(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
        			&& (pDetail->UsedComp.UseCSRT)
        	)
            {
                #ifdef EDDS_CFG_SRT_INCLUDE
                if( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT, LSA_NULL) )
                {
                    /* open channel for cSRT */

                    Status = EDDS_RTOpenChannelCyclic( pHDB );

                    if ( Status == EDD_STS_OK )
                    {
                        pHDB->UsedComp |= EDDS_COMP_CSRT;
                    }
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: SRT component not initialized yet!");
                    Status = EDD_STS_ERR_SEQUENCE;
                }
                #else
                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"edds_open_channel: SRT not supported!");
                Status = EDD_STS_ERR_PARAM;
                #endif
            }
        }


        /*-----------------------------------------------------------------------*/
        /* On error we have to undo several things..                             */
        /* (without statuscheck, because we cant do anything .)                  */
        /*-----------------------------------------------------------------------*/

        if ( Status != EDD_STS_OK )
        {
            if ( HandleInit )
            {
                if ( SysPathInit )
                {
                    if ( pHDB->UsedComp & EDDS_COMP_PRM )
                    {
                        pDDB->pGlob->Prm.HandleCnt = 0;
                        pHDB->UsedComp &= ~EDDS_COMP_PRM;
                    }

                    #ifdef EDDS_CFG_NRT_INCLUDE
                    if ( pHDB->UsedComp & EDDS_COMP_NRT )
                    {
                        EDDS_NRTCloseChannel(pHDB);  //lint !e534 JB 12/11/2014 see code comment above
                        pHDB->UsedComp &= ~EDDS_COMP_NRT;
                    }
                    #endif
                    #ifdef EDDS_CFG_SRT_INCLUDE
                    if(	(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
                    		&& ( pHDB->UsedComp & EDDS_COMP_CSRT )
                    )
                    {
                        EDDS_RTCloseChannelCyclic(pHDB); //lint !e534 JB 12/11/2014 see code comment above
                        pHDB->UsedComp &= ~EDDS_COMP_CSRT;
                    }
                    #endif
                    do_release_path_info = LSA_TRUE;
                }

                EDDS_HandleRel(pHDB->Handle); /* free handle */  //lint !e534 JB 12/11/2014 see code comment above
            }

        }

        pRQBChannel->Handle = pHDB->Handle;  /* set EDDS Handle */
        EDD_RQB_SET_HANDLE(pRQB, pRQBChannel->HandleUpper);
        EDDS_RQB_SET_STATUS(pRQB, Status);

        EDDS_CallChannelCbf(pRQBChannel->Cbf,pRQB,pSys);

        if(LSA_TRUE == do_release_path_info) //lint !e731 JB 12/11/2014 on purpose
        {
            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_RELEASE_PATH_INFO.");

            EDDS_RELEASE_PATH_INFO(&RelStat,pSys,pDetail);

            EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_RELEASE_PATH_INFO (Rsp: 0x%X).",RelStat);
        }

    }


    EDDS_EXIT_CHECK_REQUEST(ReentrPrio);

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_open_channel()");

}
//lint +esym(613,pRQB,pRQBChannel,pDetail,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_close_channel                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_CLOSE_CHANNEL           +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     LSA_NULL                        +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_CLOSE_CHANNEL           +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an channel.                                        +*/
/*+                                                                         +*/
/*+               Calls every components Close-Channel function. Every      +*/
/*+               component checks if it is used for this handle and if     +*/
/*+               so it trys to close the channel. if it fails the component+*/
/*+               returns an error-status.                                  +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_close_channel(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                          Status;
    LSA_UINT16                          Status2;
    EDDS_LOCAL_HDB_PTR_TYPE             pHDB;
    LSA_HANDLE_TYPE                     Handle;
    LSA_SYS_PTR_TYPE                    pSys;
    EDDS_DETAIL_PTR_TYPE                pDetail;
    LSA_HANDLE_TYPE                     UpperHandle;
    LSA_VOID                            LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE);
    EDD_UPPER_RQB_PTR_TYPE              pRQBDummy;
    LSA_UINT32                          i;
    LSA_BOOL							do_release_path_info = LSA_FALSE;
    #ifdef EDDS_CFG_REENTRANCE_CHECK
    LSA_UINT32                          ReentrPrio = EDDS_RENTR_PRIO_NONE;
    #endif


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_close_channel(pRQB: 0x%X)",
                           pRQB);

	//@fixme am2219 check pRQB != null - NEA6.0
    EDDS_ENTER_CHECK_REQUEST(pRQB,&ReentrPrio);

    Status = EDD_STS_OK;

    Handle = 0;         /* to prevent compiler bug on optimize */

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/

    if ( EDD_RQB_GET_OPCODE(pRQB) != EDD_OPC_CLOSE_CHANNEL )
    {

        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,">>> edds_close_channel: Invalid RQB-Opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
        //Status = EDD_STS_ERR_OPCODE;
        EDDS_RQB_ERROR(pRQB);
        return;
    }
    else
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_OPC_CLOSE_CHANNEL (Handle: 0x%X)",EDD_RQB_GET_HANDLE(pRQB));

        Handle = EDD_RQB_GET_HANDLE(pRQB);
        Status = EDDS_HandleGetHDB(Handle, &pHDB);

        /* does handle exceeds EDDS_CFG_MAX_CHANNELS */
        if ( Status != EDD_STS_OK )
        {

            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_close_channel: Invalid Handle (0x%X)",Handle);

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
        /* channel in use? */
        else if ( !pHDB->InUse )
        {
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_close_channel: Handle not in use (0x%X)",Handle);

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
            Status = EDD_STS_ERR_PARAM;
            EDDS_RQB_ERROR(pRQB);
        }
    }


    if (Status == EDD_STS_OK)
    {
        UpperHandle  = pHDB->UpperHandle;   /* save this because we will free HDB*/
        Cbf  = pHDB->Cbf;   /* save this because we will free the HDB later on   */

        /*-----------------------------------------------------------------------*/
        /* Close Channel for NRT                                                 */
        /*-----------------------------------------------------------------------*/

        #ifdef EDDS_CFG_NRT_INCLUDE
        //if (Status == EDD_STS_OK)  //no need to check this again
        {

            if ( pHDB->UsedComp & EDDS_COMP_NRT )
            {
                /* because the close-sequence may be interrupted by a higher */
                /* prio recv/send-request we set this channel to inactive    */
                /* before closing, so send/recv-requests will be regarded and*/
                /* dont conflict with an close in progress. if the close     */
                /* fails we set the channel back to active. note that the    */
                /* user should not call recv/send - requests while closing   */
                /* so this is only for secure.                               */

                pHDB->UsedComp &= ~EDDS_COMP_NRT;
                Status = EDDS_NRTCloseChannel(pHDB);
                if  (Status != EDD_STS_OK ) pHDB->UsedComp |= EDDS_COMP_NRT;
            }
        }
        #endif


        /*-----------------------------------------------------------------------*/
        /* Close Channel for cSRT                                                */
        /*-----------------------------------------------------------------------*/

        #ifdef EDDS_CFG_SRT_INCLUDE
        if(	(pHDB->pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
        		&& (Status == EDD_STS_OK))
        {
            if ( pHDB->UsedComp & EDDS_COMP_CSRT )
            {
                /* we dont need to set the channel to inactive temporary, because*/
                /* we have no higher-prio user request which may intercept this..*/

                Status = EDDS_RTCloseChannelCyclic(pHDB);
                if  (Status == EDD_STS_OK ) pHDB->UsedComp &= ~EDDS_COMP_CSRT;
            }

        }
        #endif

        /*-----------------------------------------------------------------------*/
        /* Release all Link-Indication RQBs with CANCEL.                         */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            for ( i=0; i<=pHDB->pDDB->pGlob->HWParams.Caps.PortCnt; i++)
            {
                do
                {
                    EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->LinkIndReq[i].pBottom,
                                              pHDB->LinkIndReq[i].pTop,
                                              pRQBDummy);

                    if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
                    {
                        EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
                    }
                }
                while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL));
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Release Rx Overload indication RQB with CANCEL                        */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            if ( ! LSA_HOST_PTR_ARE_EQUAL( pHDB->RxOverloadIndReq, LSA_NULL))
            {
                EDDS_RequestFinish(pHDB,pHDB->RxOverloadIndReq,EDD_STS_OK_CANCEL);
                pHDB->RxOverloadIndReq = LSA_NULL;
            }
        }

        /*-----------------------------------------------------------------------*/
        /* Release all Prm-Indication RQBs with CANCEL.                          */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            if ( pHDB->UsedComp & EDDS_COMP_PRM )
            {
                for ( i=0; i<=pHDB->pDDB->pGlob->HWParams.Caps.PortCnt; i++)
                {
                    do
                    {
                        EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->pDDB->pGlob->Prm.PrmIndReq[i].pBottom,
                                                  pHDB->pDDB->pGlob->Prm.PrmIndReq[i].pTop,
                                                  pRQBDummy);

                        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
                        {
                            EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
                        }
                    }
                    while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL));
                }

                pHDB->UsedComp &= ~EDDS_COMP_PRM;
                pHDB->pDDB->pGlob->Prm.HandleCnt = 0;
            }
        }

        pSys         = pHDB->pSys;          /* save this because we will free the HDB*/
        pDetail      = pHDB->pDetail;       /* save this because we will free the HDB*/

        /*-----------------------------------------------------------------------*/
        /* If all channels successfully closed, we release the handle and call   */
        /* the LSA release path info-function.                                   */
        /*-----------------------------------------------------------------------*/

        if (Status == EDD_STS_OK)
        {
            Status = EDDS_HandleRel(Handle); /* free HDB. dont use pHDB anymore! */

            if (Status == EDD_STS_OK)
            {
                /* Call LSA-Release Path Info. This is done only on success */
                /* NOTE: If this fails we have already closed all things and*/
                /*       cant do anything so we ignore this error or fatal? */
            	do_release_path_info = LSA_TRUE;
            }

        }

        /*-----------------------------------------------------------------------*/
        /* Finish the request. Note: dont use EDDS_RequestFinish() here, because */
        /* pHDB may be released and invalid!!                                    */
        /*-----------------------------------------------------------------------*/

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDD_RQB_SET_HANDLE(pRQB, UpperHandle );

        EDDS_CallChannelCbf(Cbf,pRQB,pSys);
        if(LSA_TRUE == do_release_path_info)  //lint !e731 JB 12/11/2014 on purpose
        {
            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"--> Calling EDDS_RELEASE_PATH_INFO.");

            EDDS_RELEASE_PATH_INFO(&Status2,pSys,pDetail);

            EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- EDDS_RELEASE_PATH_INFO (Rsp: 0x%X).",Status2);

            if (Status2 != LSA_RET_OK )
                EDDS_FatalError( EDDS_FATAL_ERR_RELEASE_PATH_INFO,
                                 EDDS_MODULE_ID,
                                 __LINE__);
        }
    }


    EDDS_EXIT_CHECK_REQUEST(ReentrPrio);

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_close_channel(RQB-Status: 0x%X)",
                           Status);


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_Request                                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_NRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SRT_xxx - Services                     +*/
/*+                         EDDS_SRV_IRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SWI_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given back on open_channel-request.        +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//check pRQB/pHQB
EDDS_STATIC LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_Request(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                  Status;
    EDD_SERVICE                 Service;
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB;


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_Request(pRQB: 0x%X)",
                           pRQB);

    /*---------------------------------------------------------------------------*/
    /* Check for valid parameters. Here we cant use the call-back-function       */
    /* on error.                                                                 */
    /*---------------------------------------------------------------------------*/
	EDDS_ASSERT_NULL_PTR(pRQB);

    Status = EDDS_HandleGetHDB(EDD_RQB_GET_HANDLE(pRQB), &pHDB); /* get handle */

    /* does handle exceeds EDDS_CFG_MAX_CHANNELS */
    if ( Status != EDD_STS_OK )
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid handle (0x%X)",EDD_RQB_GET_HANDLE(pRQB));

        Status = EDD_STS_ERR_PARAM;
        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_RQB_ERROR(pRQB);
    }
    /* channel in use? */
    else if ( !pHDB->InUse )
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_close_channel: Handle not in use (0x%X)",EDD_RQB_GET_HANDLE(pRQB));

        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        Status = EDD_STS_ERR_PARAM;
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        Service = EDD_RQB_GET_SERVICE(pRQB);

        Status  = EDD_STS_OK;

        switch (Service & EDD_SRV_TYPE_MASK)
        {
                /*------------------------------------------------------------*/
                /* NRT-Request                                                */
                /*------------------------------------------------------------*/
            case EDD_SRV_NRT_TYPE:
            case EDD_SRV_NRT_FILTER_TYPE:
            {
                #ifdef EDDS_CFG_NRT_INCLUDE
                /* check if channel configured for this services */
                if ( pHDB->UsedComp & EDDS_COMP_NRT )
                {
                    EDDS_NRTRequest(pRQB,pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
                #else
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid service (0x%X)",Service);
                Status = EDD_STS_ERR_SERVICE;
                #endif
            }
            break;

            /*------------------------------------------------------------*/
            /* SRT-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_SRT_TYPE:
            {
                #ifdef EDDS_CFG_SRT_INCLUDE
                /* check if channel configured for this services */
            	if(	(pHDB->pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
            			&& ( pHDB->UsedComp & EDDS_COMP_CSRT ))
                {
                    EDDS_RTRequestCyclic(pRQB,pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }

                #else
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid service (0x%X)",Service);
                Status = EDD_STS_ERR_SERVICE;
                #endif
            }
            break;

            /*------------------------------------------------------------*/
            /* SWI-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_SWI_TYPE:
            {
                #ifdef LLIF_CFG_SWITCH_SUPPORT
                EDDS_SwitchRequest(pRQB,pHDB);
                #else
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Not supported (0x%X)",Service);
                Status = EDD_STS_ERR_NOT_SUPPORTED;
                #endif
            }
            break;

            /*------------------------------------------------------------*/
            /* General requests                                           */
            /*------------------------------------------------------------*/
            case EDD_SRV_GENERAL_TYPE:
            {
                EDDS_GeneralRequest(pRQB,pHDB);
            }
            break;

            /*------------------------------------------------------------*/
            /* PRM-Request                                                */
            /*------------------------------------------------------------*/
            case EDD_SRV_PRM_TYPE:
            {
                /* check if channel configured for this services */
                if ( pHDB->UsedComp & EDDS_COMP_PRM )
                {
                    EDDS_PrmRequest(pRQB,pHDB);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_Request: Invalid request for this channel");
                    Status = EDD_STS_ERR_CHANNEL_USE;
                }
            }
            break;

            /*------------------------------------------------------------*/
            /* Debug Requests                                             */
            /*------------------------------------------------------------*/
            case EDDS_SRV_DEBUG_TYPE:
            {
                EDDS_DebugRequest(pRQB,pHDB);
            }
            break;

            /*------------------------------------------------------------*/
            /* Unknown/unsupported Service                                */
            /*------------------------------------------------------------*/
            default:
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_Request: Invalid service (0x%X)",Service);
                Status = EDD_STS_ERR_SERVICE;
            }
            break;
        }

        if ( Status != EDD_STS_OK )
        {
            EDDS_RequestFinish(pHDB,pRQB,Status);
        }

    }

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_Request(RQB-Status: 0x%X)",
                           Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_request                                +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+                                         EDD_OPC_REQUEST_SRT_BUFFER      +*/
/*+     LSA_HANDLE_TYPE         Handle:     valid channel-handle            +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    not used                        +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+     valid services: all EDDS_SRV_NRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SRT_xxx - Services                     +*/
/*+                         EDDS_SRV_IRT_xxx - Services                     +*/
/*+                         EDDS_SRV_SWI_xxx - Services                     +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST                 +*/
/*+                                         EDD_OPC_REQUEST_SRT_BUFFER      +*/
/*+     LSA_HANDLE_TYPE         Handle:     upper-handle from open_channel  +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:     not changed                     +*/
/*+     EDD_SERVICE             Service:    not changed                     +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_OPCODE              +*/
/*+                                         EDD_STS_ERR_SERVICE             +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+                                         EDD_STS_ERR_CHANNEL_USE         +*/
/*+                                         :                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given back on open_channel-request.        +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro EDDS_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_OPCODE                                        +*/
/*+               EDD_STS_ERR_SERVICE                                       +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//WARNING be careful when using this function, make sure not to use pRQB as null ptr
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_request(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    #ifdef EDDS_CFG_REENTRANCE_CHECK
    LSA_UINT32                  ReentrPrio = EDDS_RENTR_PRIO_NONE;
    #endif

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_request(pRQB: 0x%X)",
                           pRQB);

	//@fixme am2219 check pRQB != null - NEA6.0
    EDDS_ENTER_CHECK_REQUEST(pRQB,&ReentrPrio);

    switch ( EDD_RQB_GET_OPCODE(pRQB))
    {

            #ifdef EDDS_CFG_SRT_INCLUDE
        case EDD_OPC_REQUEST_SRT_BUFFER:
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_request: Unsupported opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));
            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_NOT_SUPPORTED);
            EDDS_RQB_ERROR(pRQB);
            break;
            #endif
        case EDD_OPC_REQUEST:
            EDDS_Request(pRQB);
            break;
        default:
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"edds_request: Invalid opcode (0x%X)",EDD_RQB_GET_OPCODE(pRQB));

            EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_OPCODE);
            EDDS_RQB_ERROR(pRQB);
            break;

    } /* switch */

    EDDS_EXIT_CHECK_REQUEST(ReentrPrio);

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_request()");

}
//lint -esym(613,pRQB)

/*****************************************************************************/
/*  end of file EDDS_USR.C                                                   */
/*****************************************************************************/
