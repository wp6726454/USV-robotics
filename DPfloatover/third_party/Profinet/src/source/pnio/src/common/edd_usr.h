
#ifndef EDD_USR_H                        /* ----- reinclude-protection ----- */
#define EDD_USR_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: EDD (EthernetDeviceDriver)                :C&  */
/*                                                                           */
/*  F i l e               &F: edd_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface for EDD (Ethernet Device Driver)                          */
/*  Defines constants, types, macros and prototyping for EDD.                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDD_MESSAGE
/*  12.08.08    JS    initial version.                                       */
/*  26.03.09    JS    removed multiple defines of EDD_SRV_SETUP_PHY          */
/*                    EDD_SRV_GET_STATISTICS and EDD_SRV_RESET_STATISTICS    */
/*  07.04.09    JS    added EDD_HW_TYPE_USED_ERTEC_200P to EDD_SRV_GET_PARAMS*/
/*  16.04.09    AH    Renamed EDD_COMMON_PORT_CNT to EDD_CFG_MAX_PORT_CNT    */
/*  20.04.09    AH    Added: AutoStopConsumerID in                           */
/*                           EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE;             */
/*                           EDD_CONSUMER_ID_INVALID                         */
/*                           EDD_CONSUMER_ID_REMOVE_AS                       */
/*  20.04.09    JS    Removed: AutoStopConsumerID from                       */
/*                           EDD_RQB_CSRT_PROVIDER_ADD_TYPE                  */
/*  24.04.09    JS    Added: EDD_NRT_FRAME_LEN_HSR                           */
/*                    Added: EDD_XRT_PROPERTIES_INVALID and comment          */
/*                    Added: EDD_DATAOFFSET_INVALID and comment              */
/*                    Added: DataOffset and EDDProperties to                 */
/*                           EDD_SRV_SRT_PROVIDER_CONTROL service            */
/*                    Added: SrcMAC,SrcIP,DataOffset,Partial_DataOffset, and */
/*                           EDDProperties to EDD_SRV_SRT_PROVIDER_CONTROL   */
/*                           service.                                        */
/*                    Added several comments to Provider ADD and Consumer ADD*/
/*                    parameters.                                            */
/*  27.04.09    JS    Removed: EDD_SRV_KRISC32_TYPE, EDD_SRV_KRISC32_PUT     */
/*                    Added PTCP-Services and types                          */
/*                           EDD_SRV_PTCP_DELAY_CONTROL                      */
/*                           EDD_SRV_PTCP_SYNC_CONTROL                       */
/*                           EDD_SRV_PTCP_DIAG_IND_PROVIDE                   */
/*                    Removed:"EDDI_OLD_NRT_FILTER_CLASS"                    */
/*  28.04.09    JS    Added EDD_NRT_TX_GRP_IP_IGMP                           */
/*                    changed TxFrmGroup numbering                           */
/*                    Removed service: EDD_SRV_SET_OWN_SRC_MAC_ADDR          */
/*                    Removed "EDD_CSRT_PROV_EVENT_SENT". no longer supported*/
/*                    Removed "EDD_MAC_ADDR_GROUP_MRRT".no longer supported  */
/*  05.05.09    JS    Removed PhyTxDelay100MBit, PhyRxDelay100MBit,          */
/*                            MACTxDelay100MBit and MACRxDelay100MBit from   */
/*                            EDD_SRV_GET_PORT_PARAMS                        */
/*                    Removed define EDD_SRV_SETUP_PHY                       */
/*                    Added EDD support comments to services                 */
/*                    Added parameter FrameSendOffset to                     */
/*                          EDD_RQB_CSRT_PROVIDER_ADD_TYPE                   */
/*  07.05.09    JS    Added comment regarding EDDI_SRV_SETUP_PHY             */
/*  13.05.09    UL    Changed value for EDD_XRT_PROPERTIES_INVALID           */
/*                    Removed parameter Partial_DataOffset from              */
/*                            EDD_SRV_SRT_CONSUMER_CTRL                      */
/*                    Added interface for isochronous support                */
/*                    Changed services EDD_SRV_SRT_CONSUMER/PROVIDER_ADD:    */
/*                            IOParams is not a pointer anymore              */
/*  18.05.09    JS    Removed parameter SrcIP_idx from                       */
/*                            EDD_RQB_CSRT_PROVIDER_ADD_TYPE                 */
/*  23.06.09    UL    Descriptive comments added for EDD_AUTONEG_CAP_xx      */
/*                    Added parameter ClearOnMISS to                         */
/*                                    EDD_SRV_SRT_CONSUMER_ADD               */
/*  15.07.09    JS    Removed EDDP_ structure definition                     */
/*                    Added define EDD_CRNUMBER_INVALID                      */
/*                    Added define EDD_CRNUMBER_NOT_USED                     */
/*                    Added parameter CRNumber to                            */
/*                            EDD_SRV_SRT_PROVIDER_CTRL                      */
/*                            EDD_SRV_SRT_CONSUMER_CTRL                      */
/*  23.07.09    JS    Changed EDD_SRV_SRT_PROVIDER_CONTROL parameters:       */
/*                              removed "Activate"                           */
/*                              added "Mode"                                 */
/*                    Changed EDD_SRV_SRT_CONSUMER_CONTROL parameters:       */
/*                              removed "Activate","SetToUnknownState",      */
/*                                      "AcceptDataInvalid"                  */
/*                              added "Mode"                                 */
/*                    Added parameter "RtClass3_PDEVTopoState" to            */
/*                              service EDD_SRV_SET_REMOTE_PORT_STATE        */
/*  28.07.09          Removed define EDD_CONSUMER_ID_INVALID                 */
/*  05.08.09          Added Modes EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE */
/*                                EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE*/
/*                          to EDD_SRV_SRT_CONSUMER_CONTROL.                 */
/*                    Changed comments to AutoStopConsumerID                 */
/*  02.10.09          Added EDD_HW_TYPE_FEATURE_SYNC_MASTER                  */
/*                    Added defines EDD_CRNUMBER_UNDEFINED                   */
/*                                  EDD_DATAOFFSET_UNDEFINED                 */
/*                                  EDD_XRT_PROPERTIES_UNDEFINED             */
/*                                  EDD_FRAME_SEND_OFFSET_UNDEFINED          */
/*                                  EDD_PROV_ID_INVALID                      */
/*                                  EDD_CONS_ID_INVALID                      */
/*                    removed define  EDD_CRNUMBER_INVALID                   */
/*                    removed define  EDD_CSRT_ID_ALL                        */
/*                    changed meaning of #defines within PROVIDER_ADD and    */
/*                    CONSUMER_ADD!                                          */
/*  07.10.09          Changed EDD_HW_TYPE_FEATURE_SYNC_MSK to                */
/*                            EDD_HW_TYPE_FEATURE_SYNC_SLAVE_MSK             */
/*                            EDD_HW_TYPE_FEATURE_NO_SYNC_SUPPORT to         */
/*                            EDD_HW_TYPE_FEATURE_SYNC_SLAVE_NO_SUPPORT      */
/*                            EDD_HW_TYPE_FEATURE_SYNC_SUPPORT to            */
/*                            EDD_HW_TYPE_FEATURE_SYNC_SLAVE_SW_SUPPORT      */
/*                            EDD_HW_TYPE_FEATURE_SYNC_IN_HW_SUPPORT to      */
/*                            EDD_HW_TYPE_FEATURE_SYNC_SLAVE_HW_SUPPORT      */
/*                    Added   EDD_HW_TYPE_FEATURE_SYNC_DELAY_MSK             */
/*  05.11.09    JS    Removed Services EDD_SRV_SRT_UDP_PROVIDER_ADD          */
/*                                     EDD_SRV_SRT_UDP_CONSUMER_ADD          */
/*                                     EDD_SRV_SRT_AUX_PROVIDER_ADD          */
/*                                     EDD_SRV_SRT_AUX_CONSUMER_ADD          */
/*                    Changed/moved/added parameters and defines of Services */
/*                                     EDD_SRV_SRT_PROVIDER_ADD              */
/*                                     EDD_SRV_SRT_CONSUMER_ADD              */
/*                                     EDD_SRV_SRT_PROVIDER_CONTROL          */
/*                                     EDD_SRV_SRT_CONSUMER_CONTROL          */
/*                    Changed EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MAX to 255    */
/*                    Changed EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX to 255    */
/*                                                                           */
/*                    Added parameter EventStatus to Service                 */
/*                            EDD_SRV_SRT_IND_PROVIDE                        */
/*                          removed events:                                  */
/*                           EDD_CSRT_CONS_EVENT_WDT_EXPIRED                 */
/*                           EDD_CSRT_CONS_EVENT_REDT_EXPIRED                */
/*                           EDD_CSRT_CONS_EVENT_SUBFRAMESENDERMODE_API      */
/*                           EDD_CSRT_CONS_EVENT_SUBFRAMESENDERMODE_RX       */
/*                                                                           */
/*  05.11.09    JS    Removed parameter "PortOnlyHasSyncTx" from Services    */
/*                           EDD_SRV_LINK_STATUS_IND_PROVIDE                 */
/*                           EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT             */
/*                           EDD_SRV_GET_PORT_PARAMS                         */
/*                                                                           */
/*  05.11.09    JS    Added defines                                          */
/*                           EDD_PTCP_SYNC_SLAVE_MODE_UPDATE                 */
/*                           EDD_PTCP_SYNC_MASTER_MODE_UPDATE                */
/*                                                                           */
/*  23.11.09    UL    Changed defines:                                       */
/*                           EDD_PROV_BUFFER_PROP_IRTE_IRT_DEFAULT           */
/*                           EDD_PROV_BUFFER_PROP_IRTE_IRT_ASYNC             */
/*                           EDD_PROV_BUFFER_PROP_IRTE_IRT_STAND_ALONE       */
/*                    changed to                                             */
/*                           EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC              */
/*                           EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC             */
/*                    Changed defines:                                       */
/*                           EDD_CONS_BUFFER_PROP_IRTE_IRT_DEFAULT           */
/*                           EDD_CONS_BUFFER_PROP_IRTE_IRT_ASYNC             */
/*                           EDD_CONS_BUFFER_PROP_IRTE_IRT_STAND_ALONE       */
/*                    changed to                                             */
/*                           EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC              */
/*                           EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC             */
/*                                                                           */
/*  24.11.09    SFr   Added defines                                          */
/*                           EDD_PARTIAL_DATAOFFSET_UNDEFINED                */
/*                           EDD_PARTIAL_DATAOFFSET_INVALID                  */
/*                                                                           */
/*  01.12.09    JS    Added define                                           */
/*                           EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY             */
/*  08.12.09    VS    Changed description of parameter "PortCnt" in services */
/*                      EDD_SRV_SRT_PROVIDER_ADD                             */
/*                      EDD_SRV_SRT_CONSUMER_ADD                             */
/*                                                                           */
/*  21.12.09    VS    Update of description of parameter                     */
/*                      - ProviderCyclePhaseMaxByteCnt                       */
/*                      - ProviderCyclePhaseMaxCnt                           */
/*                      - ProviderCyclePhaseMaxByteCntHard                   */
/*                      - ProviderCyclePhaseMaxCntHard                       */
/*                    in service EDD_SRV_SRT_GET_PARAMS                      */
/*  12.01.10    JS    Some comments changed/added                            */
/*                    Added defines                                          */
/*                       EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED                */
/*                       EDD_CSRT_CONS_EVENT_MEDIA_REDUNDANCY_APPEARS        */
/*                       EDD_CSRT_CONS_EVENT_MEDIA_REDUNDANCY_DISAPPEARS     */
/*                    EDD_SRV_PROVIDER_ADD:                                  */
/*                       Removed "Port" structure.                           */
/*                       Added "FrameSendOffset"                             */
/*                    EDD_SRV_CONSUMER_ADD:                                  */
/*                       Removed "Port" structure.                           */
/*                                                                           */
/*  14.01.10     UL   API for isochronous support changed (Tk)               */
/*                                                                           */
/*  26.01.10     JS   Renamed Framefilter EDD_NRT_FRAME_PTCP to              */
/*                                        EDD_NRT_FRAME_PTCP_SYNC            */
/*                    Added   Framefilter EDD_NRT_FRAME_PTCP_ANNO            */
/*                                                                           */
/*               VS   Changed description of define                          */
/*                    EDD_PROV_CONTROL_MODE_AUX_PASSIVATE                    */
/*                                                                           */
/*  12.02.10     JS   Added   Framefilter EDD_NRT_FRAME_PTCP_DELAY           */
/*                    Added defines EDD_SRT_FRAMEID_PTCP_ANNO_START          */
/*                                  EDD_SRT_FRAMEID_PTCP_ANNO_END            */
/*               JS   Changed  EDD_RQB_GET_PORT_PARAMS_TYPE:                 */
/*                            Element RsvIntervalRed is now from new type    */
/*                            EDD_RESERVED_INTERVAL_RX_TX_TYPE_RED           */
/*                    Removed defines EDD_SRT_FRAMEID_DFP_START              */
/*                                    EDD_SRT_FRAMEID_DFP_STOP               */
/*  15.02.10     JS   Removed several bits from EDD_CSRT_DSTAT_LOCAL_MASK_ALL*/
/*                            DATA_VALID cannot be changed by user anymore!  */
/*                    Added   DATA_VALID to   EDD_CSRT_DSTAT_LOCAL_INIT_VALUE*/
/*                            DATA_VALID is always "1"                       */
/*                    Added some EDD_SRT_FRAMEID_IRT_xx defines              */
/*                    Changes to EDD_SRV_SRT_PROVIDER_CONTROL:               */
/*                            added EDD_PROV_CONTROL_MODE_ACTIVATE_AUX       */
/*                            added EDD_PROV_CONTROL_MODE_PASSIVATE_AUX      */
/*                            removed EDD_PROV_CONTROL_MODE_AUX_PASSIVATE    */
/*  26.02.10     JS   Removed parameter FrameSendOffset from service         */
/*                            EDD_SRV_SRT_PROVIDER_ADD                       */
/*                    Removed defines EDD_FRAME_SEND_OFFSET_UNDEFINED        */
/*                                    EDD_FRAME_SEND_OFFSET_INVALID          */
/*                    Added parameter CycleReductionRatio, CyclePhase and    */
/*                                    CyclePhaseSequence to service          */
/*                                    EDD_SRV_SRT_PROVIDER_CONTROL           */
/*                    Added define EDD_CYCLE_REDUCTION_RATIO_UNDEFINED       */
/*                                 EDD_CYCLE_PHASE_UNDEFINED                 */
/*                                 EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED        */
/*                    Renamed EDD_CSRT_CYCLE_PHASE_SEQ_LAST to               */
/*                            EDD_CYCLE_PHASE_SEQUENCE_LAST                  */
/*                    Added parameter CycleReductionRatio, CyclePhase to     */
/*                                    EDD_SRV_SRT_CONSUMER_CONTROL           */
/*  27.04.10     JS   Changed comment for EDD_PROV_CONTROL_MODE_ACTIVATE     */
/*  30.04.10     JS   Changed comment for EDD_CSRT_CONS_EVENT_ENTERING_RED   */
/*  12.05.10     JS   Corrected some comments                                */
/*  18.05.10     UL   API for isochronous support changed:                   */
/*                    EDD_APPLSYNC_CMD_START replaced by                     */
/*                    EDD_APPLSYNC_CMD_START_SYNC and _START_ASYNC           */
/*  21.06.10     JS   Removed properties defines:                            */
/*                      EDD_CSRT_CONS_PROP_REDUNDANT                         */
/*                      EDD_CSRT_PROV_PROP_REDUNDANT                         */
/*                                                                           */
/*                    Changed description for DstMAC with services:          */
/*                     EDD_SRV_SRT_PROVIDER_ADD, EDD_SRV_SRT_PROVIDER_CONTROL*/
/*                                                                           */
/*                    Consumer event defines removed:                        */
/*                      EDD_CSRT_CONS_EVENT_MEDIA_REDUNDANCY_APPEARS         */
/*                      EDD_CSRT_CONS_EVENT_MEDIA_REDUNDANCY_DISAPPEARS      */
/*                                                                           */
/*                    EDD_SRV_SRT_PROVIDER_ADD, EDD_SRV_SRT_CONSUMER_ADD,    */
/*                    EDD_SRV_SRT_PROVIDER_CONTROL,                          */
/*                    EDD_SRV_SRT_CONSUMER_CONTROL:                          */
/*                       AppGroup may be set at CONTROL. New define          */
/*                       EDD_APPGROUP_UNDEFINED added                        */
/*                                                                           */
/*                       Removed parameter CutThrough.                       */
/*                       Added parameter ForwardMode,CutThrough_OutDataStart */
/*                                                                           */
/*  22.06.10     VS   New FrameID ranges for IRT                             */
/*                       Removed defines:                                    */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_NORMAL_START   */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_NORMAL_STOP    */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_DFP_START      */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_DFP_STOP       */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_DFP_START          */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_DFP_STOP           */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_NORMAL_START       */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_NORMAL_STOP        */
/*                       New defines added:                                  */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_START          */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_STOP           */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_START              */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP               */
/*  05.07.10     JS   Changed comment of CutThrough_OutDataStart             */
/*                    Changed comment to AppGroup with Consumer (3 allowed)  */
/*                    New parameters "PreambleLength" and "TxFragmentation"  */
/*                    added to services                                      */
/*                      EDD_SRV_LINK_STATUS_IND_PROVIDE                      */
/*                      EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT                  */
/*                      EDD_SRV_GET_PORT_PARAMS                              */
/*                    Provider parameter "CyclePhase" will not be used with  */
/*                    non legacy RTC3 provider.                              */
/*                    FrameID range for IRTtop changed:                      */
/*                          EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_STOP           */
/*                          EDD_SRT_FRAMEID_IRT_REDUNDANT_START              */
/*                    Comment "EDD-ERTEC" changed to "EDDI"                  */
/*  13.07.10     JS   added parameter "Neighbour_TopoState" to service       */
/*                          EDD_SRV_SET_REMOTE_PORT_STATE                    */
/*                    renamed EDD_SRV_PRM_CHANNEL_ERROR_TYPE to              */
/*                            EDD_SRV_PRM_CHANNEL_ERROR_TYPE_NC_MISMATCH     */
/*                    renamed EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE to          */
/*                            EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_NC_FD_NR    */
/*                    added defines:                                         */
/*                            EDD_SRV_PRM_CHANNEL_ERROR_TYPE_MRPD_MISMATCH   */
/*                            EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_MRPD_DV     */
/*                            EDD_SRV_PRM_CHANNEL_ERROR_TYPE_DFP_MISMATCH    */
/*                            EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_DFP_FLE     */
/*  15.07.10     JS   comments corrected.                                    */
/*  16.07.10     JS   parameter PreambleLength changed to ShortPreamble      */
/*  19.07.10     JS   added EDD_UPPER_CALLBACK_FCT_PTR_TYPE                  */
/*  12.08.10     JS   added two bits to "HardwareType"                       */
/*                            EDD_HW_TYPE_FEATURE_PREAMBLE_SHORTENING        */
/*                            EDD_HW_TYPE_FEATURE_FRAGMENTATION              */
/*  18.08.10     JS   added parameter "TraceIdx" to EDD_RQB_GET_PARAMS_TYPE  */
/*  06.09.10     AH   added parameter "TraceIdx" to EDD_RQB_CSRT_PARAM_TYPE  */
/*  13.09.10     JS   changed comment to parameter DoSwFwd                   */
/*  20.09.10     JS   removed parameter from EDD_SRV_SENDCLOCK_CHANGE:       */
/*                      - ProviderCyclePhaseMaxByteCnt                       */
/*                      - ProviderCyclePhaseMaxCnt                           */
/*                    removed parameter from EDD_SRV_SENDCLOCK_CHANGE and    */
/*                                      from EDD_SRV_SRT_GET_PARAMS          */
/*                      - ProviderCyclePhaseMaxByteCntHard                   */
/*                      - ProviderCyclePhaseMaxCntHard                       */
/*                    changed meaning of ProviderCyclePhaseMaxByteCnt        */
/*                    renamed parameter of                                   */
/*                      EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE:                */
/*                      - TxDelay to PortTxDelay                             */
/*                      - RxDelay to PortRxDelay                             */
/*  21.09.10     JS   changed description/meaning of parameter of service    */
/*                      EDD_SRV_NRT_SET_DCP_FILTER                           */
/*                    changed/added MRP parameter:                           */
/*                       - removed MRP_PortState                             */
/*                      service EDD_SRV_GET_PARAMS:                          */
/*                       - added MRPDefaultRole                              */
/*                       - added MRPRoleCapability                           */
/*                      service EDD_SRV_GET_PORT_PARAMS:                     */
/*                       - added MRPRingPort                                 */
/*  04.11.10     JS   added SFPosition and SFOffset to services:             */
/*                       EDD_SRV_SRT_PROVIDER_ADD                            */
/*                       EDD_SRV_SRT_PROVIDER_CONTROL                        */
/*                       EDD_SRV_SRT_CONSUMER_ADD                            */
/*                       EDD_SRV_SRT_CONSUMER_CONTROL                        */
/*                    removed EDD_CSRT_PROV_PROP_RTCLASS_AUX                 */
/*                    added   EDD_CSRT_DFP_DATALEN_MIN                       */
/*                    added   EDD_CSRT_DFP_DATALEN_MAX                       */
/*  24.11.10     JS   corrected some comments                                */
/*  13.12.10     JS   added new frame filter:EDD_NRT_FRAME_LEN_SINEC_FWL     */
/*  17.01.11     JS   removed revisions from header                          */
/*                    added new service EDD_SRV_GET_STATISTICS_ALL           */
/*                    changed service EDD_SRV_GET_STATISTICS                 */
/*                    added MaxInterfaceCntOfAllEDD and MaxPortCntOfAllEDD   */
/*                    to service EDD_SRV_GET_PARAMS                          */
/*  02.03.11     JS   removed EDD_PARTIAL_DATAOFFSET_INVALID                 */
/*                    removed EDD_PARTIAL_DATAOFFSET_UNDEFINED               */
/*                    moved Partial_DataLen and Partial_DataOffset to new    */
/*                    location.                                              */
/*                    addded EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS to   */
/*                      EDD_SRV_SRT_PROVIDER_CONTROL                         */
/*                    added EDD_CSRT_PROV_PROP_MODE_MC_CR to                 */
/*                      EDD_SRV_SRT_PROVIDER_ADD                             */
/*                                                                           */
/*  31.03.11     UL   Service EDD_SRV_SRT_PROVIDER_ADD:                      */
/*                    added DataStatusOffset to IOPARAMS                     */
/*                    added EDD_CSRT_PROV_PROP_SYSRED                        */
/*                                                                           */
/*                    Service EDD_SRV_SRT_PROVIDER_CONTROL:                  */
/*                    only REDUNDANCY and STATION_FAILURE                    */
/*                    can be changed from nowon                              */
/*                    Parameters added: FrameID, DstMAC, DstIP               */
/*                                                                           */
/*                    Service EDD_SRV_SRT_PROVIDER_SET_STATE:                */
/*                    only STOP_RUN can be changed from nowon                */
/*                                                                           */
/*                    Service EDD_SRV_SRT_CONSUMER_ADD:                      */
/*                    added EDD_CSRT_CONS_PROP_SYSRED                        */
/*                                                                           */
/* 15.04.11      UL   Service EDD_SRV_SRT_PROVIDER_CONTROL:                  */
/*                    added EDD_PROV_CONTROL_MODE_REMOVE_ACW                 */
/*                    Service EDD_SRV_SRT_CONSUMER_CONTROL:                  */
/*                    added EDD_CONS_CONTROL_MODE_REMOVE_ACW                 */
/*                                                                           */
/* 27.04.11      UL   Service EDD_SRV_SRT_PROVIDER_ADD:                      */
/*                    removed parameter DstMac, DstIP                        */
/*                                                                           */
/*                    Service EDD_SRV_SRT_PROVIDER_CONTROL:                  */
/*                    only STATION_FAILURE can be changed from nowon         */
/*                                                                           */
/*                    Service EDD_SRV_SRT_CONSUMER_ADD:                      */
/*                    removed parameter SrcMac, SrcIP                        */
/*                                                                           */
/*                    Description of WIN logic removed.                      */
/*                                                                           */
/*  13.05.11     JS   removed service EDD_SRV_SRT_GET_PARAMS                 */
/*                    moved parameters from EDD_SRV_SRT_GET_PARAMS to        */
/*                    EDD_SRV_GET_PARAMS                                     */
/*  16.05.11     VS   removed parameter "ReservedIntervalEndMaxNs"           */
/*                    from service EDD_SRV_GET_PARAMS                        */
/*  19.05.11     VS   changed comment of parameter "PhaseMasks" in service   */
/*                    EDD_SRV_GET_PARAMS                                     */
/*  16.06.11     TB   EDD_NRT_SEND_PRIO_ORG replaced by                      */
/*                    EDD_NRT_SEND_PRIO_MGMT_LOW and                         */
/*                    EDD_NRT_SEND_PRIO_MGMT_HIGH                            */
/*  11.07.11     AH   Added Statistics counter for NRT Fragmentation         */
/*                                                                           */
/*  14.06.11     UL   Name of the events SYSTEM_REDUNDANCY_APPEARS/SYSTEM_   */
/*                    REDUNDANCY_DISAPPEARS changed to NO_PRIMARY_AR_EXISTS/ */
/*                    PRIMARY_AR_EXISTS                                      */
/*                                                                           */
/*  18.07.11     UL   Service EDD_SRV_SRT_CONSUMER_CONTROL:                  */
/*                     Modes EDD_CONS_CONTROL_MODE_SET_MAC / _SET_IP removed */
/*  01.08.11     UL   Removed LSA_RQB_LOCAL_ERROR_TYPE in RQB                */
/*                    added element DebugInfo to services:                   */
/*                      EDD_SRV_SRT_IND_PROVIDE,                             */
/*                      EDD_SRV_SRT_CONSUMER_CONROL,                         */
/*                      EDD_SRV_SRT_PROVIDER_CONTROL                         */
/*  23.09.11     UL   Service EDD_SRV_SET_CYCLE_COUNTER changed to           */
/*                    EDD_SRV_SET_SYNC_TIME                                  */
/*                    Changed EDD_RQB_SET_SYNC_TIME_TYPE:                    */
/*                      Removed CycleCtrLow, CycleCtrHigh                    */
/*                      Added PTCPSeconds, PTCPNanoSeconds                   */
/*                                                                           */
/*  20.09.11     JS   Added define EDD_IEEE_FRAME_LENGTH_WITH_CRC            */
/*                                                                           */
/*  14.10.11     AH   Changed type from LSA_UINT32 to LSA_USER_ID_TYPE for   */
/*                    UserID in struct EDD_APPL_SYNC_START_PARAM_TYPE        */
/*                                                                           */
/*  31.10.11     UL   Changes in API for iso-support:                        */
/*                      New ApplSync command EDD_APPLSYNC_CMD_CHECK_PARAMS   */
/*                      New out-parameter pXPLLOUT_CycleLen_ns               */
/*                                                                           */
/*  07.02.12     JS   Changes in API for multiple MRP instances (rings)      */
/*                      EDD_SRV_GET_PARAMS                                   */
/*                       New:                                                */
/*                       - MRPInstanceCount                                  */
/*                       - MRPDefaultRoleInstance0                           */
/*                       - MRPSupportedRole                                  */
/*                       - MRPSupportedMultipleRole                          */
/*                       Removed:                                            */
/*                       - MRPDefaultRole                                    */
/*                       - MRPRoleCapability                                 */
/*                      EDD_SRV_SWITCH_FLUSH_FILTERING_DB                    */
/*                       New:                                                */
/*                       - PortCnt                                           */
/*                       - PortID                                            */
/*                                                                           */
/*                                                                           */
/*  15.02.12     JS   Changes in API for reserved interval (ap01260742)      */
/*                      Added parameter "RsvIntervalRed" to service          */
/*                      EDD_SRV_GET_PARAMS                                   */
/*                    Updated some descriptions to match edd_usr.doc         */
/*                                                                           */
/*  21.02.12     VS   Changes in Service EDD_SRV_NRT_SET_DCP_FILTER          */
/*                    for DCPIdentify Buffer                                 */
/*                      Removed: Pointer "ppFilterTLV"                       */
/*                      Added: struct - FilterTLV[MaxPortCnt].TLVLen         */
/*                                    - FilterTLV[MaxPortCnt].pTLV           */
/* 22.06.12      UL   Changes in service EDD_SRV_LED_BLINK:                  */
/*                      Removed: OnOffDurationIn500msTicks,                  */
/*                               TotalBlinkDurationInSeconds                 */
/*                                                                           */
/*  08.03.12     AH   Changed varibale name from MRPInstanceCount to         */
/*                    MaxMRP_Instances                                       */
/*                                                                           */
/*                                                                           */
/*  16.07.12     UL   Changes in services EDD_SRV_SRT_PROVIDER_ADD/_CONTROL/ */
/*                    _CHANGE_PHASE                                          */
/*                      Element CyclePhaseSequence changed from LSA_UINT16   */
/*                      to LSA_UINT32                                        */
/*                    Changes in service EDD_SRV_SRT_CONSUMER_ADD            */
/*                      Removed element CyclePhaseSequence                   */
/*                                                                           */
/*  25.09.12     TP   Removed lock bits from HardwareType                    */
/*                    EDD_HW_TYPE_FEATURE_LOCK_REQ_XXX                       */
/*                    Removed buffer services                                */
/*                    EDD_SRV_SRT_PROVIDER_LOCK_CURRENT                      */
/*                    EDD_SRV_SRT_PROVIDER_LOCK_NEW                          */
/*                    EDD_SRV_SRT_PROVIDER_UNLOCK                            */
/*                    EDD_SRV_SRT_CONSUMER_LOCK                              */
/*                    EDD_SRV_SRT_CONSUMER_UNLOCK                            */
/*                    Some services moved to eddi_usr.h                      */
/*  29.01.13     AH   Removed PhaseMasks from struct EDD_RQB_GET_PARAMS_TYPE */
/*                    Removed EDD_CSRT_PHASE_MASKS_CNT                       */
/*                                                                           */
/*  25.06.13     TP   Changes in API                                         */
/*                      Replaced type for parameter "RsvIntervalRed" in      */
/*                      service EDD_SRV_GET_PARAM                            */
/*                    Removed EDD_RESERVED_INTERVAL_IF_TYPE_RED and          */
/*                      EDD_RESERVED_INTERVAL_IF_RX_TX_TYPE_RED. Replaced by */
/*                      EDD_RESERVED_INTERVAL_IF_RED_TYPE.                   */
/*                    Changes in service EDD_SRV_PRM_COMMIT                  */
/*                      Element EDD_RESERVED_INTERVAL_IF_RED_TYPE added.     */
/*  08.07.13     MH   Removed WatchdogFactor from                            */
/*                    EDD_RQB_CSRT_CONSUMER_ADD_TYPE                         */
/*                    Removed EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MIN           */
/*                    Removed EDD_SRT_CONSUMER_WATCHDOG_FACTOR_MAX           */
/*  24.09.14     TP   Added EDD_AUTONEG_CAP_NONE                             */
/*  06.02.15     TH   EDD_RQB_GET_PORT_PARAMS_TYPE                           */
/*                        added values                                       */
/*                              PhyStatus                                    */
#endif                        

/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/

#define EDD_OPC_SYSTEM                             (LSA_OPCODE_TYPE)   0x0000
#define EDD_OPC_OPEN_CHANNEL                       (LSA_OPCODE_TYPE)   0x0001
#define EDD_OPC_CLOSE_CHANNEL                      (LSA_OPCODE_TYPE)   0x0002
#define EDD_OPC_REQUEST                            (LSA_OPCODE_TYPE)   0x0003
#define EDD_OPC_REQUEST_SRT_BUFFER                 (LSA_OPCODE_TYPE)   0x0004

/*---------------------------------------------------------------------------*/
/* EDD global user services (known/supported by all EDD variants)            */
/*                                                                           */
/* For additional EDD specific user services see eddx_usr.h                  */
/* For additional EDD specific sysstem services see eddx_sys.h               */
/*                                                                           */
/* Bits 08..15 used for decoding of service! Do not change!                  */
/*                                                                           */
/* Range 0x0000..0x0FFF reserved for Device-Services                         */
/*---------------------------------------------------------------------------*/

#define EDD_SRV_TYPE_MASK 0xFF00

/*=====  EDD_OPC_REQUEST: NRT-Services  (0x1000..0x10FF) ====*/
#define EDD_SRV_NRT_TYPE                                (EDD_SERVICE)   0x1000

#define EDD_SRV_NRT_RECV                                (EDD_SERVICE)   0x1000
#define EDD_SRV_NRT_SEND                                (EDD_SERVICE)   0x1001
#define EDD_SRV_NRT_CANCEL                              (EDD_SERVICE)   0x1002
/*      EDDI_SRV_NRT_RECV_MONITOR_START                                 0x1004*/
/*      EDDI_SRV_NRT_RECV_MONITOR_STOP                                  0x1005*/
#define EDD_SRV_NRT_SEND_TIMESTAMP                      (EDD_SERVICE)   0x1006
/*      EDDI_SRV_NRT_FRAMETRIGGER_SET                                   0x1007*/
/*      EDDI_SRV_NRT_FRAMETRIGGER_GET                                   0x1008*/

/*=====  EDD_OPC_REQUEST: CRT-Services (0x2001..0x20FF) ====*/
#define EDD_SRV_SRT_TYPE                                (EDD_SERVICE)   0x2000

#define EDD_SRV_SRT_IND_PROVIDE                         (EDD_SERVICE)   0x2001
#define EDD_SRV_SRT_PROVIDER_SET_STATE                  (EDD_SERVICE)   0x2002
#define EDD_SRV_SRT_PROVIDER_ADD                        (EDD_SERVICE)   0x2003
#define EDD_SRV_SRT_PROVIDER_CONTROL                    (EDD_SERVICE)   0x2004
#define EDD_SRV_SRT_PROVIDER_CHANGE_PHASE               (EDD_SERVICE)   0x2005
#define EDD_SRV_SRT_PROVIDER_REMOVE                     (EDD_SERVICE)   0x2006
#define EDD_SRV_SRT_PROVIDER_XCHANGE_BUF                (EDD_SERVICE)   0x2007
#define EDD_SRV_SRT_CONSUMER_ADD                        (EDD_SERVICE)   0x2008
#define EDD_SRV_SRT_CONSUMER_CONTROL                    (EDD_SERVICE)   0x2009
#define EDD_SRV_SRT_CONSUMER_REMOVE                     (EDD_SERVICE)   0x200a
#define EDD_SRV_SRT_CONSUMER_XCHANGE_BUF                (EDD_SERVICE)   0x200b
#define EDD_SRV_SRT_GET_APDU_STATUS                     (EDD_SERVICE)   0x200c

/*=====  EDD_OPC_REQUEST:  SWI-Services (0x2200..0x22FF)  ====*/
#define EDD_SRV_SWI_TYPE                                (EDD_SERVICE)   0x2200

#define EDD_SRV_SWITCH_GET_PARAMS                       (EDD_SERVICE)   0x2200
#define EDD_SRV_SWITCH_SET_PORT_STATE                   (EDD_SERVICE)   0x2201
#define EDD_SRV_SWITCH_GET_PORT_STATE                   (EDD_SERVICE)   0x2202
#define EDD_SRV_SWITCH_FLUSH_FILTERING_DB               (EDD_SERVICE)   0x2203
/*      EDDI_SRV_SWITCH_SET_PORT_MONITOR                                0x221b*/
/*      EDDI_SRV_SWITCH_GET_PORT_MONITOR                                0x221c*/
/*      EDDI_SRV_SWITCH_SET_MIRROR_PORT                                 0x221d*/
/*      EDDI_SRV_SWITCH_GET_MIRROR_PORT                                 0x221e*/
/*      EDDI_SRV_SWITCH_FLUSH_FILTERING_DB                              0x2225*/
/*      EDDI_SRV_SWITCH_SET_FDB_ENTRY                                   0x2205*/

/*=====  EDD_OPC_REQUEST:  GENERAL-Services (0x2300..0x23FF) ======*/
#define EDD_SRV_GENERAL_TYPE                            (EDD_SERVICE)   0x2300

#define EDD_SRV_GET_LINK_STATUS                         (EDD_SERVICE)   0x2301
#define EDD_SRV_LINK_STATUS_IND_PROVIDE                 (EDD_SERVICE)   0x2302
#define EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT             (EDD_SERVICE)   0x2303
#define EDD_SRV_MULTICAST                               (EDD_SERVICE)   0x2304
#define EDD_SRV_GET_PARAMS                              (EDD_SERVICE)   0x2305
#define EDD_SRV_SET_IP                                  (EDD_SERVICE)   0x2306
#define EDD_SRV_MULTICAST_FWD_CTRL                      (EDD_SERVICE)   0x2307
#define EDD_SRV_SET_LINE_DELAY                          (EDD_SERVICE)   0x2308
#define EDD_SRV_GET_PORT_PARAMS                         (EDD_SERVICE)   0x2309
#define EDD_SRV_RX_OVERLOAD_IND_PROVIDE                 (EDD_SERVICE)   0x230a
#define EDD_SRV_SENDCLOCK_CHANGE                        (EDD_SERVICE)   0x230b
/*      EDDI_SRV_SETUP_PHY                                              0x230C*/
#define EDD_SRV_LED_BLINK                               (EDD_SERVICE)   0x230d
#define EDD_SRV_SET_SYNC_TIME                           (EDD_SERVICE)   0x2310
#define EDD_SRV_SET_SYNC_STATE                          (EDD_SERVICE)   0x2315
#define EDD_SRV_SET_REMOTE_PORT_STATE                   (EDD_SERVICE)   0x2316
#define EDD_SRV_GET_STATISTICS                          (EDD_SERVICE)   0x2317
#define EDD_SRV_RESET_STATISTICS                        (EDD_SERVICE)   0x2318
#define EDD_SRV_GET_STATISTICS_ALL                      (EDD_SERVICE)   0x2319

/*=====   reserved (0x2400..0x24FF) used by EDDI      internal ======*/

/*=====   reserved (0x2500..0x25FF) used by EDDI      internal ======*/

/*=====  EDD_OPC_REQUEST:  PRM-Services (0x2700..0x27FF)  ====*/
#define EDD_SRV_PRM_TYPE                                (EDD_SERVICE)   0x2700

#define EDD_SRV_PRM_READ                                (EDD_SERVICE)   0x2700
#define EDD_SRV_PRM_WRITE                               (EDD_SERVICE)   0x2701
#define EDD_SRV_PRM_INDICATION                          (EDD_SERVICE)   0x2702
#define EDD_SRV_PRM_PREPARE                             (EDD_SERVICE)   0x2703
#define EDD_SRV_PRM_END                                 (EDD_SERVICE)   0x2704
#define EDD_SRV_PRM_COMMIT                              (EDD_SERVICE)   0x2705

/*=====  EDD_OPC_REQUEST: NRT-Filter services (0x2800..0x28FF) ====*/
#define EDD_SRV_NRT_FILTER_TYPE                         (EDD_SERVICE)   0x2800

#define EDD_SRV_NRT_SET_ARP_FILTER                      (EDD_SERVICE)   0x2800
#define EDD_SRV_NRT_SET_DCP_FILTER                      (EDD_SERVICE)   0x2801
#define EDD_SRV_NRT_SET_DCP_HELLO_FILTER                (EDD_SERVICE)   0x2802

/*=====   reserved (0x3000..0x3FFF) used by EDDI      internal ======*/
/*=====   reserved (0x4000..0x4FFF) used by EDDI      internal ======*/
/*=====   reserved (0x5000..0x5FFF) used by EDDI      internal ======*/

/*=====  EDD_OPC_REQUEST: SYNC-Services (0x6000..0x60FF)  ====*/
#define EDD_SRV_SYNC_TYPE                               (EDD_SERVICE)   0x6000

#define EDD_SRV_SYNC_RECV                               (EDD_SERVICE)   0x6000
#define EDD_SRV_SYNC_SEND                               (EDD_SERVICE)   0x6001
#define EDD_SRV_SYNC_CANCEL                             (EDD_SERVICE)   0x6002
#define EDD_SRV_SYNC_SET_RATE                           (EDD_SERVICE)   0x6003

/*=====  EDD_OPC_REQUEST: PTCP-Services (0x6100..0x61FF)  ====*/
#define EDD_SRV_PTCP_TYPE                               (EDD_SERVICE)   0x6100

#define EDD_SRV_PTCP_DELAY_CONTROL                      (EDD_SERVICE)   0x6100
#define EDD_SRV_PTCP_SYNC_CONTROL                       (EDD_SERVICE)   0x6101
#define EDD_SRV_PTCP_DIAG_IND_PROVIDE                   (EDD_SERVICE)   0x6102


/*=====   reserved (0x8000..0x8FFF) used by EDD dependend services ======*/

/*---------------------------------------------------------------------------*/
/*  Status within RQB (Response)                                             */
/*---------------------------------------------------------------------------*/

/* Type of EDD-Respones. */
#define EDD_RSP  LSA_RESPONSE_TYPE

/* LSA OK-Codes */
#define EDD_STS_OK                       (EDD_RSP)    LSA_RSP_OK
/* LSA ERR-Codes */
#define EDD_STS_ERR_RESOURCE             (EDD_RSP)    LSA_RSP_ERR_RESOURCE
#define EDD_STS_ERR_PARAM                (EDD_RSP)    LSA_RSP_ERR_PARAM
#define EDD_STS_ERR_SEQUENCE             (EDD_RSP)    LSA_RSP_ERR_SEQUENCE
#define EDD_STS_ERR_SYS_PATH             (EDD_RSP)    LSA_RSP_ERR_SYS_PATH
#define EDD_STS_ERR_TIMER_IS_RUNNING     (EDD_RSP)    LSA_RET_ERR_TIMER_IS_RUNNING

/* EDD OK-Code (MaxOffset = 0x3F) */
/* LSA_RSP_OK_OFFSET = 0x40       */

#define EDD_STS_OK_NO_DATA               (EDD_RSP)   (LSA_RSP_OK_OFFSET + 0x01)
#define EDD_STS_OK_PENDING               (EDD_RSP)   (LSA_RSP_OK_OFFSET + 0x02)
#define EDD_STS_OK_CANCEL                (EDD_RSP)   (LSA_RSP_OK_OFFSET + 0x03)

/* ERR-Codes from Offset 0x2F..0x3E reserved for EDD specific systemfunctions*/

/* EDD ERR-Codes (MaxOffset = 0x3E)*/
/* LSA_RSP_ERR_OFFSET = 0xC0       */

#define EDD_STS_ERR_SERVICE              (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x01)
#define EDD_STS_ERR_OPCODE               (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x02)
#define EDD_STS_ERR_TIMEOUT              (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x03)
#define EDD_STS_ERR_TX                   (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x04)
#define EDD_STS_ERR_RX                   (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x05)
#define EDD_STS_ERR_ALIGN                (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x06)
#define EDD_STS_ERR_HW                   (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x07)
#define EDD_STS_ERR_CHANNEL_USE          (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x08)
#define EDD_STS_ERR_LIMIT_REACHED        (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x09)
#define EDD_STS_ERR_PROTOCOL             (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0A)
#define EDD_STS_ERR_TIMEOUT_ACK          (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0B)
#define EDD_STS_ERR_ABORT                (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0C)
#define EDD_STS_ERR_PHASE_MASKED         (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0D)
#define EDD_STS_ERR_NO_LOCAL_IP          (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0E)
#define EDD_STS_ERR_NOT_SUPPORTED        (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x0F)
#define EDD_STS_ERR_NOT_IMPL             (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x10)
#define EDD_STS_ERR_EXCP                 (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x11)
#define EDD_STS_ERR_BLOCKED_EXCP         (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x12)
#define EDD_STS_ERR_MODE                 (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x13)
#define EDD_STS_ERR_NOT_ALLOWED          (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x14)
#define EDD_STS_ERR_PRM_INDEX            (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x15)
#define EDD_STS_ERR_PRM_DATA             (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x16)
#define EDD_STS_ERR_PRM_BLOCK            (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x17)
#define EDD_STS_ERR_PRM_CONSISTENCY      (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x18)
#define EDD_STS_ERR_PRM_PORTID           (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x19)
#define EDD_STS_ERR_PRM_VERSION_CONFLICT (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x1A)
#define EDD_STS_ERR_NO_TIMER             (EDD_RSP)   (LSA_RSP_ERR_OFFSET + 0x1B)

/* ERR-Codes from Offset 0x2F..0x3E reserved for EDD specific systemfunctions*/

/*===========================================================================*/
/*                              basic types for EDD                          */
/*===========================================================================*/

#define EDD_UPPER_MEM_PTR_TYPE               /* pointer to upper-memory */ \
    LSA_VOID                    EDD_UPPER_MEM_ATTR *

#define EDD_UPPER_MEM_U8_PTR_TYPE            /* pointer to upper-memory */ \
    LSA_UINT8                   EDD_UPPER_MEM_ATTR *

#define EDD_UPPER_MEM_U16_PTR_TYPE           /* pointer to upper-memory */ \
    LSA_UINT16                  EDD_UPPER_MEM_ATTR *

#define EDD_UPPER_MEM_U32_PTR_TYPE          /* pointer to upper-memory */ \
    LSA_UINT32                  EDD_UPPER_MEM_ATTR *

#define EDD_UPPER_MEM_U64_PTR_TYPE              /* pointer to device-memory */ \
    LSA_UINT64                  EDD_UPPER_MEM_ATTR *


#define EDD_COMMON_MEM_PTR_TYPE               /* pointer to common-memory */ \
    LSA_VOID                    EDD_COMMON_MEM_ATTR *

#define EDD_COMMON_MEM_U8_PTR_TYPE            /* pointer to common-memory */ \
    LSA_UINT8                   EDD_COMMON_MEM_ATTR *

#define EDD_COMMON_MEM_U16_PTR_TYPE            /* pointer to common-memory */ \
    LSA_UINT16                  EDD_COMMON_MEM_ATTR *

#define EDD_COMMON_MEM_U32_PTR_TYPE            /* pointer to common-memory */ \
    LSA_UINT32                  EDD_COMMON_MEM_ATTR *

#define EDD_COMMON_MEM_U64_PTR_TYPE            /* pointer to common-memory */ \
    LSA_UINT64                  EDD_COMMON_MEM_ATTR *

#define EDD_COMMON_MEM_BOOL_PTR_TYPE            /* pointer to common-memory */ \
    LSA_BOOL                    EDD_COMMON_MEM_ATTR *


#define EDD_SERVICE             LSA_UINT32

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* MAC/IP-Address                                                            */
/*---------------------------------------------------------------------------*/

#define EDD_MAC_ADDR_SIZE  6

typedef struct _EDD_MAC_ADR_TYPE
{
    LSA_UINT8       MacAdr[EDD_MAC_ADDR_SIZE];
} EDD_MAC_ADR_TYPE;


#define EDD_IP_ADDR_SIZE  4

typedef struct _EDD_IP_ADR_TYPE
{
    LSA_UINT8       IPAdr[EDD_IP_ADDR_SIZE];
} EDD_IP_ADR_TYPE;

/* ========================================================================= */
/*+  Frame:                                                                 +*/
/*+                                                                         +*/
/*+   56 Bits  8 Bits  6 Byte  6 Byte 2 Byte    46..1500Byte      4By       +*/
/*+  +-------+-------+-------+-------+--------+-----------------+-----+     +*/
/*+  |Preable| Synch |  DST  |  SRC  |LNG/Type|    LLC-Data     | FCS |     +*/
/*+  +-------+-------+-------+-------+--------+-----------------+-----+     +*/
/*+                  |<-------------- Ethernetframe ----------------->|     +*/
/*+                                                                         +*/
/*+  VLAN-Tag (optional inserted between Lng/Type and LLC-Data)             +*/
/*+  Mandatory for xRT-Frames, optional for NRT-Frames.                     +*/
/*+                                                                         +*/
/*+    2Byte    2Byte                                                       +*/
/*+  +--------+-------+                                                     +*/
/*+  |lng/type|tag-ctr|                                                     +*/
/*+  +--------+-------+                                                     +*/
/*+                                                                         +*/
/*+  LLC-Data for xRT-Frames:                                               +*/
/*+                                                                         +*/
/*+  for cSRT:                                                              +*/
/*+                                                                         +*/
/*+    2Byte                              2Byte     1Byte      1Byte        +*/
/*+  +-------+-----------------------+------------+---------+---------+     +*/
/*+  |FrameID|      cSRT-User-Data   |CycleCounter|DataStat |TransStat|     +*/
/*+  +-------+-----------------------+------------+---------+---------+     +*/
/*+                                                                         +*/
/*+  for aSRT:                                                              +*/
/*+                                                                         +*/
/*+    2Byte                                                                +*/
/*+  +-------+-----------------------+--------------------------------+     +*/
/*+  |FrameID|      aSRT-User-Data   |         aSRT-Epilog *)         |     +*/
/*+  +-------+-----------------------+--------------------------------+     +*/
/*+  *) not present yet                                                     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  If the frame is VLAN-tagged, the max-frame size for xRT-Frames will    +*/
/*+  not bei extended!                                                      +*/
/*+                                                                         +*/
/* ========================================================================= */

/* IEEE Frame length with long frame support */
#define EDD_IEEE_FRAME_LENGTH           1518                       /* max. length without CRC */
#define EDD_IEEE_FRAME_LENGTH_WITH_CRC  (EDD_IEEE_FRAME_LENGTH+4)  /* max. length with    CRC */

/*---------------------------------------------------------------------------*/
/* Because of the zero-copy - interface we must take care of                 */
/* buffersize requirements from the ethernetcontroller within application.   */
/*---------------------------------------------------------------------------*/
/* All receive - buffers must be at least EDD_FRAME_BUFFER_LENGTH size!      */
/* NOTE: This values must be >= maximum framesize received by any Hardware!  */
/*       We use 1536 here because the EDDI may receive this size.            */
/*---------------------------------------------------------------------------*/

#define EDD_FRAME_BUFFER_LENGTH      EDD_CFG_RX_FRAME_BUFFER_LENGTH

#define EDD_VLAN_TAG_SIZE            4

/*---------------------------------------------------------------------------*/
/* SRT-FrameID-Ranges                                                        */
/*---------------------------------------------------------------------------*/

/* PTCP FrameIDs (part 1) */
#define EDD_SRT_FRAMEID_PTCP_START_RANGE_0                0x0000
#define EDD_SRT_FRAMEID_PTCP_SYNC_START                   0x0000

#define EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3               0x0080
#define EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3_B             0x00A0

#define EDD_SRT_FRAMEID_PTCP_STOP_RANGE_0                 0x009F

/* IRT */
#define EDD_SRT_FRAMEID_IRT_START                         0x0100

#define EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_START           (EDD_SRT_FRAMEID_IRT_START)
#define EDD_SRT_FRAMEID_IRT_NON_REDUNDANT_STOP            0x06FF

#define EDD_SRT_FRAMEID_IRT_REDUNDANT_START               0x0700
#define EDD_SRT_FRAMEID_IRT_REDUNDANT_STOP                (EDD_SRT_FRAMEID_IRT_STOP)

#define EDD_SRT_FRAMEID_IRT_STOP                          0x0FFF

/* RESERVED                             */
#define EDD_SRT_FRAMEID_RES_START                         0x1000   /* reserved frame-ids */
#define EDD_SRT_FRAMEID_RES_STOP                          0x77FF

/* SRT                                  */
#define EDD_SRT_FRAMEID_CSRT_START1                       0x8000
#define EDD_SRT_FRAMEID_CSRT_STOP1                        0xBFFF
#define EDD_SRT_FRAMEID_CSRT_START2                       0xC000
#define EDD_SRT_FRAMEID_CSRT_STOP2                        0xFBFF

#define EDD_SRT_FRAMEID_ASRT_START_HIGH                   0xFC00
#define EDD_SRT_FRAMEID_ASRT_STOP_HIGH                    0xFCFF
#define EDD_SRT_FRAMEID_ASRT_PNIO_ALARM_HIGH              0xFC01

#define EDD_SRT_FRAMEID_ASRT_START_LOW                    0xFE00
#define EDD_SRT_FRAMEID_ASRT_STOP_LOW                     0xFEFB
#define EDD_SRT_FRAMEID_ASRT_PNIO_ALARM_LOW               0xFE01

#define EDD_SRT_FRAMEID_CSRT_START              EDD_SRT_FRAMEID_IRT_START
#define EDD_SRT_FRAMEID_CSRT_STOP               EDD_SRT_FRAMEID_CSRT_STOP2
#define EDD_SRT_FRAMEID_CSRT_CNT_MAX            (EDD_SRT_FRAMEID_CSRT_STOP-EDD_SRT_FRAMEID_CSRT_START+1)

/* DCP with xRT-Frame format FrameIDs   */
#define EDD_SRT_FRAMEID_ASRT_DCP_HELLO                    0xFEFC
#define EDD_SRT_FRAMEID_ASRT_DCP_UNICAST                  0xFEFD
#define EDD_SRT_FRAMEID_ASRT_DCP_MC_REQ                   0xFEFE
#define EDD_SRT_FRAMEID_ASRT_DCP_MC_RSP                   0xFEFF

/* PTCP FrameIDs (part 2)               */
#define EDD_SRT_FRAMEID_PTCP_START_RANGE_1                0xFF00

#define EDD_SRT_FRAMEID_PTCP_ANNO_START                   0xFF00
#define EDD_SRT_FRAMEID_PTCP_ANNO_END                     0xFF1F

#define EDD_SRT_FRAMEID_PTCP_DELAY_START                  0xFF40
#define EDD_SRT_FRAMEID_PTCP_DELAY_END                    0xFF43

#define EDD_SRT_FRAMEID_PTCP_STOP_RANGE_1                 0xFF5F

#define EDD_MRRT_FRAMEID                                  0xFF60


/*---------------------------------------------------------------------------*/
/* some IP Setting used with xRToverUDP                                      */
/*---------------------------------------------------------------------------*/

#define EDD_DEFAULT_TTL  64         /* TTL (time to live) field of IP-Header */

/*---------------------------------------------------------------------------*/
/* DebugInfo for services EDD_SRV_SRT_IND_PROVIDE,                           */
/* EDD_SRV_SRT_CONSUMER_CONROL, EDD_SRV_SRT_PROVIDER_CONTROL                 */
/*---------------------------------------------------------------------------*/
#define EDD_DEBUG_INFO_SIZE                     16      /* 16 bytes debug info */

#define EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDI    0x03    /* EDDI: IND_PROVIDE */
#define EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDP    0x05    /* EDDP: IND_PROVIDE */
#define EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDS    0x07    /* EDDS: IND_PROVIDE */

#define EDD_DEBUGINFO_CONTROL_IDENT_EDDI        0x02    /* EDDI: PROVIDER_/CONSUMER_CONTROL */
#define EDD_DEBUGINFO_CONTROL_IDENT_EDDP        0x04    /* EDDP: PROVIDER_/CONSUMER_CONTROL */
#define EDD_DEBUGINFO_CONTROL_IDENT_EDDS        0x06    /* EDDS: PROVIDER_/CONSUMER_CONTROL */

/*===========================================================================*/
/*                        RQB (Request Block)                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* RQB- Header for Services                                                  */
/* (EDD_RQB_HEADER see edd_cfg.h)                                            */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_TYPE            EDD_UPPER_RQB_ATTR *    EDD_UPPER_RQB_PTR_TYPE;

typedef struct _EDD_RQB_TYPE
{
    EDD_RQB_HEADER
    EDD_SERVICE                     Service;
    EDD_UPPER_MEM_PTR_TYPE          pParam;
    LSA_VOID                       *internal_context;     /* EDD internal use*/
    LSA_UINT32                      internal_context_1;   /* EDD internal use*/
    LSA_UINT32                      internal_context_Prio;/* EDD internal use*/
    EDD_RQB_TRAILER
} EDD_RQB_TYPE;


/*---------------------------------------------------------------------------*/
/* RQB- service-specific paramteter blocks, pParam points to.                */
/*      Opcode and Service specify the Paramblock                            */
/*                                                                           */
/*   RQB-Header                                                              */
/*  +------------+                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  +------------+ service-       e.g. EDD_RQB_GET_LINK_STATUS_TYPE          */
/*  |   pParam   | ------------->+-----------+                               */
/*  +------------+ specific      |           |                               */
/*                               | +------------+                            */
/*                               | |            |                            */
/*                               | | +-------------+                         */
/*                               | | |             |                         */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*===========================================================================*/

typedef LSA_VOID LSA_FCT_PTR( EDD_UPPER_OUT_FCT_ATTR, EDD_UPPER_CALLBACK_FCT_PTR_TYPE)(EDD_UPPER_RQB_PTR_TYPE  pRQB);

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_OPEN_CHANNEL     Service: no code                         */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_OPEN_CHANNEL_TYPE
{
    LSA_HANDLE_TYPE                   Handle;             /* returned EDD-Handle */
    LSA_HANDLE_TYPE                   HandleUpper;
    LSA_SYS_PATH_TYPE                 SysPath;
    EDD_UPPER_CALLBACK_FCT_PTR_TYPE   Cbf;
} EDD_RQB_OPEN_CHANNEL_TYPE;

typedef struct _EDD_RQB_OPEN_CHANNEL_TYPE   EDD_UPPER_MEM_ATTR *    EDD_UPPER_OPEN_CHANNEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_CLOSE_CHANNEL    Service: no code                         */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_GET_LINK_STATUS                 */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_GET_LINK_STATUS_TYPE
{
    LSA_UINT16          PortID;
    LSA_UINT8           Status;
    LSA_UINT8           Speed;
    LSA_UINT8           Mode;
} EDD_RQB_GET_LINK_STATUS_TYPE;

/* PortID             :   Port to get status for                        (in) */
/*                        range: 1..FFFF or EDD_PORT_ID_AUTO                 */

#define EDD_LINK_UNKNOWN        0          /* unknown link status/speed/mode */

/* Status             :   Status of link                                (out)*/

#define EDD_LINK_UP             1
#define EDD_LINK_DOWN           2
#define EDD_LINK_UP_CLOSED      3
#define EDD_LINK_UP_DISABLED    4

/* Speed              :   Speed if link is up                           (out)*/

#define EDD_LINK_SPEED_10       1       /* 10  MBit/s          */
#define EDD_LINK_SPEED_100      2       /* 100 MBit/s          */
#define EDD_LINK_SPEED_1000     3       /* 1   GBit/s          */
#define EDD_LINK_SPEED_10000    4       /* 10  GBit/s          */

/* Duplexity          :   Duplexity  if link is up                      (out)*/

#define EDD_LINK_MODE_HALF      1
#define EDD_LINK_MODE_FULL      2


typedef struct _EDD_RQB_GET_LINK_STATUS_TYPE    EDD_UPPER_MEM_ATTR *    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_LINK_STATUS_IND_PROVIDE         */
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT     */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE
{
  LSA_UINT16          PortID;
  LSA_UINT8           Autoneg;
  LSA_UINT8           Status;
  LSA_UINT8           PhyStatus;
  LSA_UINT8           Speed;
  LSA_UINT8           Mode;
  LSA_UINT8           PortStatus;
  LSA_UINT8           IRTPortStatus;
  LSA_UINT8           RTClass2_PortStatus;
  LSA_UINT16          PortState;
  LSA_UINT16          MAUType;
  LSA_UINT8           MediaType;
  LSA_BOOL            ShortPreamble;
  LSA_BOOL            TxFragmentation;
  LSA_UINT32          LineDelayInTicksMeasured;
  LSA_UINT32          CableDelayInNsMeasured;
  LSA_UINT32          PortTxDelay;
  LSA_UINT32          PortRxDelay;
  LSA_BOOL            SyncId0_TopoOk;
} EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE;

typedef struct _EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE    EDD_UPPER_MEM_ATTR *    EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE;


/* PortID             :   Port to get status for                        (in) */
/*                        range: 1..FFFF or EDD_PORT_ID_AUTO                 */
/* Autoneg            :   Current negotiation configuration       (in *)(out)*/
/* Status             :   Status of link                            (in)(out)*/
/* PhyStatus          :   PHY Status                              (in *)(out)*/
/* Speed              :   Speed if link is up                       (in)(out)*/
/* Mode               :   Mode  if link is up                       (in)(out)*/
/* PortStatus         :   Status of Port                          (in *)(out)*/
/*                        EDD_PORT_PRESENT                                   */
/*                        EDD_PORT_NOT_PRESENT (only if Link down)           */
/* IRTPortStatus      :   IRT Status of Port                      (in *)(out)*/
/*                        EDD_IRT_NOT_SUPPORTED                              */
/*                        EDD_IRT_PORT_INACTIVE                              */
/*                        EDD_IRT_PORT_ACTIVE_TX_UP                          */
/*                        EDD_IRT_PORT_ACTIVE_TX_RX                          */
/* RTClass2_PortStatus:   RT Class2 Port Status                   (in *)(out)*/
/*                        EDD_RTCLASS2_NOT_SUPPORTED                         */
/*                        EDD_RTCLASS2_INACTIVE                              */
/*                        EDD_RTCLASS2_ACTIVE                                */
/* PortState          :   Forwarding PortState                    (in *)(out)*/
/* PhyType            :   Type of PHY. See EDD_SRV_GET_PORT_PARAMS(in *)(out)*/
/* MAUType            :   MAU Type                                (in *)(out)*/
/* MediaType          :   Media Type                              (in *)(out)*/
/* ShortPreamble      :   ShortPreamble active for sending        (in *)(out)*/
/*                        LSA_TRUE:  Preamble length: 1 Byte                 */
/*                        LSA_FALSE: Preamble length: 7 Bytes                */
/* TxFragmentation    :   Tx frame fragmentation activated?       (in *)(out)*/
/*                        LSA_TRUE:  activated                               */
/*                        LSA_FALSE: not activated                           */
/* LineDelayInTicksMeasured    :   LineDelay messured             (in *)(out)*/
/* CableDelayInNsMeasured      :   CableDelay in ns.              (in *)(out)*/
/* PortTxDelay        :   Send Delay of MAC+PHY in ns             (in *)(out)*/
/* PortRxDelay        :   Receive Delay of MAC+PHY in ns          (in *)(out)*/
/* SyncId0_TopoOk     :   State of the neighbourhood check for    (in *)(out)*/
/*                        SyncID0                                            */

/* *): "in" only with EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT !                  */

#define EDD_DELAY_UNKNOWN   0   /* if RX/TX-Delay is unknown or not supported*/

/* Status/Speed/Mode, see EDD_SRV_GET_LINK_STATUS */

/* Autoneg (configured Autoneg) */
#define EDD_AUTONEG_UNKNOWN            0   /* unknown  negotiation configuration */
#define EDD_AUTONEG_ON                 1   /* active   negotiation configuration */
#define EDD_AUTONEG_OFF                2   /* inactive negotiation configuration */

/* PortStatus */
#define EDD_PORT_PRESENT               0   /* Port is present                    */
#define EDD_PORT_NOT_PRESENT           1   /* Port is not present.               */

/* IRTPortStatus */
#define EDD_IRT_NOT_SUPPORTED          0   /* IRT not supported (no IRT parameter)                   */
#define EDD_IRT_PORT_INACTIVE          1   /* IRT supported but inactive                             */
#define EDD_IRT_PORT_ACTIVE_TX_UP      2   /* IRT with active TX only (while opening IRT-Connection) */
#define EDD_IRT_PORT_ACTIVE_TX_RX      5   /* IRT with active both TX and RX                         */

/* RTClass2_PortStatus  */
#define EDD_RTCLASS2_NOT_SUPPORTED     0
#define EDD_RTCLASS2_ACTIVE            1   /*  */
#define EDD_RTCLASS2_INACTIVE          2   /*  */

/* PortState */
#define EDD_PORT_STATE_DISABLE                        0x0001
#define EDD_PORT_STATE_BLOCKING                       0x0002
#define EDD_PORT_STATE_LISTENING                      0x0003
#define EDD_PORT_STATE_LEARNING                       0x0004
#define EDD_PORT_STATE_FORWARDING                     0x0005
#define EDD_PORT_STATE_UNCHANGED                      0x0006
#define EDD_PORT_STATE_FORWARDING_WITHOUT_LEARNING    0x0007

/* MAUType */
#define EDD_MAUTYPE_UNKNOWN                    0       /* unknown  MAUType */

/* MediaType */
#define EDD_MEDIATYPE_UNKNOWN                  0       /* unknown  MediaType */
#define EDD_MEDIATYPE_COPPER_CABLE             1
#define EDD_MEDIATYPE_FIBER_OPTIC_CABLE        2
#define EDD_MEDIATYPE_RADIO_COMMUNICATION      3

/* PhyStatus */
#define EDD_PHY_STATUS_UNKNOWN                 0
#define EDD_PHY_STATUS_DISABLED                1
#define EDD_PHY_STATUS_ENABLED                 2

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_MULTICAST                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_MULTICAST_TYPE
{
    LSA_UINT8                          Mode;
    EDD_MAC_ADR_TYPE                   MACAddr;
    LSA_UINT32                         RefCnt;
} EDD_RQB_MULTICAST_TYPE;

typedef struct _EDD_RQB_MULTICAST_TYPE      EDD_UPPER_MEM_ATTR *    EDD_UPPER_MULTICAST_PTR_TYPE;

/* Mode               :   Mode of function                              (in) */

#define EDD_MULTICAST_ENABLE        0
#define EDD_MULTICAST_DISABLE       1
#define EDD_MULTICAST_DISABLE_ALL   2

/* MACAddr            :   MAC Address to enable/disable                 (in) */
/* RefCnt             :   MAC Address reference counter                (out) */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_MULTICAST_FWD_CTRL              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PORTID_MODE_TYPE
{
    LSA_UINT16                         PortID;
    LSA_UINT16                         Mode;
} EDD_RQB_PORTID_MODE_TYPE;

typedef struct _EDD_RQB_PORTID_MODE_TYPE    EDD_UPPER_MEM_ATTR * EDD_UPPER_PORTID_MODE_PTR_TYPE;

/* PortID             :   Port to set forwarding for.                   (in) */
/*                        range: 1..FFFF                                     */
/* Mode               :   Mode of function                              (in) */

#define EDD_MULTICAST_FWD_ENABLE            0
#define EDD_MULTICAST_FWD_DISABLE           1
#define EDD_MULTICAST_FWD_BITMASK           2
#define EDD_MULTICAST_FWD_BITMASK_PLUS_BL   3 /* only for internal use by EDDI      */
#define EDD_MULTICAST_FWD_DISABLE_PLUS_BL   4 /* only for internal use by EDDI      */

typedef struct _EDD_RQB_MULTICAST_FWD_CTRL_TYPE
{
    EDD_UPPER_PORTID_MODE_PTR_TYPE  pPortIDModeArray;
    LSA_UINT16                      PortIDModeCnt;
    LSA_UINT16                      MACAddrGroup;
    LSA_UINT16                      MACAddrPrio;
    LSA_UINT32                      MACAddrLow;
} EDD_RQB_MULTICAST_FWD_CTRL_TYPE;

typedef struct _EDD_RQB_MULTICAST_FWD_CTRL_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_MULTICAST_FWD_CTRL_PTR_TYPE;

/* pPortIDModeArray   :   Pointer to array with Port ids/modes          (in) */
/* PortIDModeCnt      :   Number of arrayelements within pPortIDModeArr.(in) */
/* MACAddrGroup       :   MAC Address group                             (in) */

#define EDD_MAC_ADDR_GROUP_PTCP      0        /* NOTE: only for internal use!*/
#define EDD_MAC_ADDR_GROUP_PTCP_ALL  1        /* NOTE: only for internal use!*/
#define EDD_MAC_ADDR_GROUP_RT_2_QUER 2
#define EDD_MAC_ADDR_GROUP_MRP_1     3
#define EDD_MAC_ADDR_GROUP_MRP_2     4
#define EDD_MAC_ADDR_GROUP_HSR_1     6
#define EDD_MAC_ADDR_GROUP_HSR_2     7
#define EDD_MAC_ADDR_GROUP_STBY_1    8
#define EDD_MAC_ADDR_GROUP_STBY_2    9
#define EDD_MAC_ADDR_GROUP_DCP      10        /* NOTE: only for internal use!*/


/* MACAddrPrio        :   Prio for MAC Address                          (in) */
/*                        only allowed with HSR/STBY Groups                  */

#define EDD_MULTICAST_FWD_PRIO_UNCHANGED    0 /* Prio won't be changed       */
#define EDD_MULTICAST_FWD_PRIO_NORMAL       1 /* Prio Normal                 */
#define EDD_MULTICAST_FWD_PRIO_HIGH         2 /* Prio High                   */
#define EDD_MULTICAST_FWD_PRIO_ORG          3 /* Prio ORG                    */

/* MACAddrLow         :   MAC Address low bits. Depends on Group        (in) */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_GET_PARAMS                      */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_KRAM_INFO_TYPE
{
    LSA_UINT32            KRAMBaseAddr;
    LSA_UINT32            offset_ProcessImageStart;
    LSA_UINT32            offset_async_in_start;   /* only ERTEC 200 */
    LSA_UINT32            size_async_in;           /* only ERTEC 200 */
    LSA_UINT32            offset_async_out_start;  /* only ERTEC 200 */
    LSA_UINT32            size_async_out;          /* only ERTEC 200 */
    LSA_UINT32            offset_ProcessImageEnd;
} EDD_KRAM_INFO_TYPE;

typedef struct _EDD_KRAM_INFO_TYPE  EDD_UPPER_MEM_ATTR * EDD_KRAM_INFO_PTR_TYPE;

typedef struct _EDD_RESERVED_INTERVAL_IF_RED_TYPE
{               
    LSA_UINT32   RxEndNs;
    LSA_UINT32   RxLocalTransferEndNs;
    LSA_UINT32   TxEndNs;
    LSA_UINT32   TxLocalTransferStartNs;
    LSA_UINT32   TxLocalTransferEndNs;
} EDD_RESERVED_INTERVAL_IF_RED_TYPE;

/* RxEndNs                  : Maximum end time of all red Rx ?intervals (periods) of the interface      (out)*/
/*                            (all ports and all phases) in ns. This value is determined from the             */
/*                            PDIRBeginEndData. If no PDIRData, or no red Rx periods are present,             */
/*                            this value is set to 0                                                          */
/* RxLocalTransferEndNs     : This value contains the time in ns when the I/O data of the last locally   (out)*/
/*                            received red frame (consumer) of the interface is transferred into              */
/*                            I/O-memory and can be accessed by the application. This value is                */
/*                            calculated from PDIRData Parameter. This value is calculated from all           */
/*                            ports and phases of the interface to find the maximum value. If no              */
/*                            PDIRData, or no local received red (RTC3) frames are present, this value        */
/*                            is set to 0.                                                                    */
/* TxEndNs                  : Maximum end time of all red Tx - intervals (periods) of the interface      (out)*/
/*                            (all ports and all phases) in ns. This value is determined from the             */
/*                            PDIRBeginEndData. If no PDIRData or no red Tx periods are present, this         */
/*                            value is set to 0.                                                              */
/* TxLocalTransferStartNs   : This value contains the time in ns when the I/O data of the first locally  (out)*/
/*                            send red frame (provider) of the interface will be transferred for sending.     */
/*                            Till this time the I/O data may be updated by the application to be used for    */
/*                            sending within the current cycle. This value is calculated from all ports       */
/*                            and phases of the interface to find the maximum value. If no PDIRData, or       */
/*                            no local send red (RTC3) frames are present, this value is set to 0.            */
/* TxLocalTransferEndNs     : This value contains the maximum transfer end time (worst case) in ns       (out)*/
/*                            when the I/O data of all local transmitted red frames of the interface          */
/*                            (provider) are transfered for sending. This value is calculated from all        */
/*                            ports and phases to find the worst case. If no PDIRData, or no local            */
/*                            transmitted red (RTC3) frames are present, this value is set to 0.              */


typedef struct _EDD_RQB_GET_PARAMS_TYPE
{
    LSA_UINT32                           InterfaceID;
    LSA_UINT32                           HardwareType;
    LSA_UINT32                           TraceIdx;
    LSA_UINT32                           TimeResolutionInNs;
    EDD_MAC_ADR_TYPE                     MACAddr;
    EDD_IP_ADR_TYPE                      LocalIP;
    LSA_UINT16                           MaxPortCnt;
    LSA_UINT16                           MaxPortCntOfAllEDD;
    LSA_UINT16                           MaxInterfaceCntOfAllEDD;
    LSA_UINT8                            MaxMRP_Instances;
    LSA_UINT8                            MRPDefaultRoleInstance0;
    LSA_UINT8                            MRPSupportedRole;
    LSA_UINT8                            MRPSupportedMultipleRole;
    LSA_UINT16                           CycleBaseFactor;
    LSA_UINT16                           ConsumerFrameIDBaseClass3;
    LSA_UINT16                           ConsumerCntClass3;
    LSA_UINT16                           ConsumerFrameIDBaseClass2;
    LSA_UINT16                           ConsumerCntClass2;
    LSA_UINT16                           ConsumerFrameIDBaseClass1;
    LSA_UINT16                           ConsumerCntClass1;
    LSA_BOOL                             ConsumerCntClass12Combined;
    LSA_UINT16                           ProviderCnt;
    LSA_UINT16                           ProviderCntClass3;
    LSA_UINT16                           ProviderGroupCnt;
    LSA_UINT16                           DataLenMin;
    LSA_UINT16                           DataLenMax;
    LSA_UINT16                           DataLenMinUDP;
    LSA_UINT16                           DataLenMaxUDP;
    LSA_UINT32                           ProviderCyclePhaseMaxByteCnt;          /* 0: no limit */
    LSA_UINT32                           ProviderCyclePhaseMaxCnt;              /* 0: no limit */
    EDD_KRAM_INFO_TYPE                   KRAMInfo;
    EDD_RESERVED_INTERVAL_IF_RED_TYPE    RsvIntervalRed;
} EDD_RQB_GET_PARAMS_TYPE;

typedef struct _EDD_RQB_GET_PARAMS_TYPE      EDD_UPPER_MEM_ATTR *    EDD_UPPER_GET_PARAMS_PTR_TYPE;

/* InterfaceID        : Interface ID from DPB                                               (out)*/

#define EDD_INTERFACE_ID_MIN  1
#define EDD_INTERFACE_ID_MAX  16

/* HardwareType       : Type of Ethernethardware                                            (out)*/
/*                        Bit  0...7. EDD-Hardware used                                          */
/*                        Bit  8      EDD-Feature: reserved                                      */
/*                        Bit  9      EDD-Feature: Timestamp supported                           */
/*                        Bit  10     EDD-Feature: Delays supported                              */
/*                        Bit  11     EDD-Feature: Consumer Class 1,2 partial data supported     */
/*                        Bit  12     EDD-Feature: reserved                                      */
/*                        Bit  13     EDD-Feature: CT (Cut-Through) supported                    */
/*                        Bit  14     EDD-Feature: Consumer Class 3 partial data supported       */
/*                        Bit  15     EDD-Feature: Consumer Class 1 UDP partial data supported   */
/*                        Bit  16     EDD-Feature: Provider Class 1 UDP autopadding  supported   */
/*                        Bit  17     EDD-Feature: Provider Class 1/2   autopadding  supported   */
/*                        Bit  18     EDD-Feature: Provider Class 3     autopadding  supported   */
/*                        Bit  19     EDD-Feature: APDUStatus is separated from Frame  Flag      */
/*                        Bit  20..21 EDD-Feature: SyncSupport                                   */
/*                        Bit  22..23 EDD-Feature: Sync Master support                           */
/*                        Bit  24..25 EDD-Feature: Sync Delay support                            */
/*                        Bit  26     EDD-Feature: PreambleShortening support                    */
/*                        Bit  27     EDD-Feature: Fragmentation support                         */
/*                        Bit  28.31. reserved (0)                                               */

#define EDD_HW_TYPE_USED_MSK                                    0x000000FF   /* Bit 0..7: Mask for used hardware   */
#define EDD_HW_TYPE_USED_STANDARD_MAC                           0x00000000   /* No special RT hardware support.    */
                                                                             /* EDD uses a standard MAC and        */
                                                                             /* software xRT support               */
#define EDD_HW_TYPE_USED_ERTEC_400                              0x00000001   /* ERTEC 400 hardware support.        */
#define EDD_HW_TYPE_USED_ERTEC_200                              0x00000002   /* ERTEC 200 hardware support.        */
#define EDD_HW_TYPE_USED_SOC                                    0x00000003   /* SoC hardware support.              */
#define EDD_HW_TYPE_USED_ERTEC_200P                             0x00000004   /* ERTEC 200 Plus hardware support    */

/* Feature: TimeStamps supported (see NRT-Send/Recv) :   */
#define EDD_HW_TYPE_FEATURE_TIMESTAMP_MSK                       0x00000200   /* Bit 9: Timestamps supported ?      */
#define EDD_HW_TYPE_FEATURE_TIMESTAMP_SUPPORT                   0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_TIMESTAMP_SUPPORT                0x00000200   /* No                                 */

/* Feature: RX/TX-Delays supported (see Link Indication) */
#define EDD_HW_TYPE_FEATURE_DELAY_MSK                           0x00000400   /* Bit 10: Rx/Tx-Delays supported?    */
#define EDD_HW_TYPE_FEATURE_DELAY_SUPPORT                       0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_DELAY_SUPPORT                    0x00000400   /* No                                 */

/* Feature: Consumer partial data supported  for CLASS 1/2            */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_MSK           0x00000800   /* Bit 11: Partial Data supported?    */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_12_SUPPORT       0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_12_SUPPORT    0x00000800   /* No                                 */

/* Feature: CT (Cut-Through) supported              */
#define EDD_HW_TYPE_FEATURE_CT_MSK                              0x00002000   /* Bit 13: CT supported ?             */
#define EDD_HW_TYPE_FEATURE_CT_SUPPORT                          0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_CT_SUPPORT                       0x00002000   /* No                                 */

/* Feature: Consumer partial data supported for CLASS 3           */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_MSK            0x00004000   /* Bit 14: Partial Data supported?    */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_3_SUPPORT        0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_3_SUPPORT     0x00004000   /* No                                 */

/* Feature: Consumer partial data supported for CLASS 1 UDP           */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_MSK        0x00008000   /* Bit 15: Partial Data supported?    */
#define EDD_HW_TYPE_FEATURE_PARTIAL_DATA_CLASS_1_UDP_SUPPORT    0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_PARTIAL_DATA_CLASS_1_UDP_SUPPORT 0x00008000   /* No                                 */

/* Feature: Provider autopadding supported for CLASS 1 UDP           */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_MSK         0x00010000   /* Bit 16: Autopadding supported?     */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_1_UDP_SUPPORT     0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_1_UDP_SUPPORT  0x00010000   /* No                                 */

/* Feature: Provider autopadding supported for CLASS 1/2             */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_MSK            0x00020000   /* Bit 17: Autopadding supported?     */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_12_SUPPORT        0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT     0x00020000   /* No                                 */

/* Feature: Provider autopadding supported for CLASS 3               */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_MSK             0x00040000   /* Bit 18: Autopadding supported?     */
#define EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT         0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_3_SUPPORT      0x00040000   /* No                                 */

/* Feature: APDUStatus is separated from Frame  Flag is set during edd_device_open!   */
#define EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE_MSK             0x00080000   /* Bit 19: is APDUStatus separated from frame? */
#define EDD_HW_TYPE_FEATURE_APDUSTATUS_SEPARATE                 0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE              0x00080000   /* No                                 */

/* Feature: SYNC slave support within EDD */
#define EDD_HW_TYPE_FEATURE_SYNC_SLAVE_MSK                      0x00300000   /* Bits 20/21: Sync-Support           */
#define EDD_HW_TYPE_FEATURE_SYNC_SLAVE_NO_SUPPORT               0x00000000   /* no SYNC-Support                    */
                                                                             /* -> no SYNC-Services supported      */
                                                                             /* -> no KRISC-Services supported     */
#define EDD_HW_TYPE_FEATURE_SYNC_SLAVE_SW_SUPPORT               0x00100000   /* Sync supported with services       */
                                                                             /* -> all SYNC-Services supported     */
                                                                             /* -> KRISC services not supported    */
#define EDD_HW_TYPE_FEATURE_SYNC_SLAVE_HW_SUPPORT               0x00200000   /* Sync supported in hardware         */
                                                                             /* -> no SYNC_SEND/RECV/CANCEL        */
                                                                             /* -> KRISC slave services supported  */
/* Feature: SYNC master support within EDD */
#define EDD_HW_TYPE_FEATURE_SYNC_MASTER_MSK                     0x00C00000   /* Bit 22/23: Sync master support     */
#define EDD_HW_TYPE_FEATURE_SYNC_MASTER_NO_SUPPORT              0x00000000   /* No SYNC_SEND and no KRISC services */
#define EDD_HW_TYPE_FEATURE_SYNC_MASTER_SW_SUPPORT              0x00400000   /* All SYNC services supported        */
#define EDD_HW_TYPE_FEATURE_SYNC_MASTER_HW_SUPPORT              0x00800000   /* KRISC master services supported    */

/* Feature: SYNC delay support within EDD */
#define EDD_HW_TYPE_FEATURE_SYNC_DELAY_MSK                      0x03000000   /* Bit 24/25: Sync delay support      */
#define EDD_HW_TYPE_FEATURE_SYNC_DELAY_NO_SUPPORT               0x00000000   /* No SEND_TIMESTAMP,no KRISC-Services*/
#define EDD_HW_TYPE_FEATURE_SYNC_DELAY_SW_SUPPORT               0x01000000   /* SEND_TIMESTAMP service supported   */
#define EDD_HW_TYPE_FEATURE_SYNC_DELAY_HW_SUPPORT               0x02000000   /* KRISC delay services supported     */

/* Feature: PreambleShortening support           */
#define EDD_HW_TYPE_FEATURE_PREAMBLE_SHORTENING_MSK             0x04000000   /* Bit 26: Preambleshortening support?*/
#define EDD_HW_TYPE_FEATURE_PREAMBLE_SHORTENING_SUPPORT         0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_PREAMBLE_SHORTENING_SUPPORT      0x04000000   /* No                                 */

/* Feature: Fragmentation support                */
#define EDD_HW_TYPE_FEATURE_FRAGMENTATION_MSK                   0x08000000   /* Bit 27: Fragmentation supported?   */
#define EDD_HW_TYPE_FEATURE_FRAGMENTATION_SUPPORT               0x00000000   /* Yes                                */
#define EDD_HW_TYPE_FEATURE_NO_FRAGMENTATION_SUPPORT            0x08000000   /* No                                 */

/* TraceIdx           :   Trace index useable with LSA-Trace (indexed traces)               (out)*/
/*                        0   : no special index on unknown                                      */
/*                              (because only one interface used or id not present)              */
/*                        1..x: trace index. Typically the index references the EDD interface,   */
/*                              which shall be unique for all EDD interfaces. The TraceIdx will  */
/*                              be configured by systemadaption on EDD startup.                  */
/* TimeResolutionInNS         : NRT send/recv Timestamp timebase                            (out)*/
/*                              0 if timestamp not supported.                                    */
/* MACAddr                    : Station MAC address                        (out)*/
/* LocalIP                    : Local IP Address set by EDD_SRV_SET_IP     (out)*/
/*                              0.0.0.0 if not set or invalid                   */
/* MaxPortCnt                 : Number of supported Ports                  (out)*/
/* MaxInterfaceCntOfAllEDD    : Maximum number of interfaces of all EDDs   (out)*/
/*                              within system.                                  */
/*                              Configured by systemadaption on startup         */
/*                              Max: EDD_CFG_MAX_INTERFACE_CNT                  */
/* MaxPortCntOfAllEDD         : Maximum number of ports of all EDDs        (out)*/
/*                              within system.                                  */
/*                              Configured by systemadaption on startup         */
/*                              Max: EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE         */
/* MaxMRP_Instances           : Number of MRP instances supported for this (out)*/
/*                              interface. This parameter is configured         */
/*                              within EDD startup.                             */
/*                              0: none. MRP not supported.                     */
/*                              1.. EDD_CFG_MAX_MRP_INSTANCE_CNT                */
/* MRPDefaultRoleInstance0    : If MRP is supported but no MRP             (out)*/
/*                              parameterization is present it is assumed       */
/*                              that only one MRP instance is present.          */
/*                              In this case this parameter defines the MRP     */
/*                              role for MRP instance 0.                        */
/*                              The parameter is configured within EDD on       */
/*                              startup and only forwarded to the user (MRP)    */

#define EDD_MRP_ROLE_NONE                0
#define EDD_MRP_ROLE_CLIENT              1
#define EDD_MRP_ROLE_MANAGER             2
#define EDD_MRP_ROLE_AUTOMANAGER         3

/* MRPSupportedRole           : This parameter defines the possible MRP    (out)*/
/*                              roles if only one MRP instance is present       */
/*                              within parameterization (PRM).                  */
/*                              The parameter is configured within EDD on       */
/*                              startup and only forwarded to the user (MRP)    */
/*                              Values can be ORed!                             */
/* MRPSupportedMultipleRole   : This parameter defines the possible MRP    (out)*/
/*                              roles if multiple  MRP instance are present     */
/*                              within parameterization (PRM).                  */
/*                              The parameter is configured within EDD on       */
/*                              startup and only forwarded to the user (MRP)    */
/*                              Values can be ORed!                             */

#define EDD_MRP_ROLE_CAP_CLIENT          0x01
#define EDD_MRP_ROLE_CAP_MANAGER         0x02
#define EDD_MRP_ROLE_CAP_AUTOMANAGER     0x04


/* Cyclical parameters--------------------------------------------------------- */
/* CycleBaseFactor            : Base CycleTime = CycleBaseFactor * 31.25us (out)*/
/*                              Note: Set to 32 if cyclical not supported.      */
/* ConsumerFrameIDBaseClass3  : FrameID base for xRT Class 3 frames (IRT)  (out)*/
/*                              (0= not supported)                              */
/* ConsumerCntClass3          : Number of xRT Class 3 Consumer (IRT)       (out)*/
/*                              (0= not supported)                              */
/* ConsumerFrameIDBaseClass2  : FrameID base for xRT Class 2 frames (SRT)  (out)*/
/*                              (0= not supported)                              */
/* ConsumerCntClass2          : Number of xRT Class 2 Consumer (SRT)       (out)*/
/*                              (0= not supported)                              */
/* ConsumerFrameIDBaseClass1  : FrameID base for xRT Class 1 frames        (out)*/
/*                              (SRT and xRToverUDP)                            */
/*                              (0= not supported)                              */
/* ConsumerCntClass1          : Number of xRT Class 1 Consumer             (out)*/
/*                              (0= not supported)                              */
/* ConsumerCntClass12Combined                                              (out)*/
/*                              LSA_FALSE:  ConsumerCntClass1 and Class2        */
/*                                          defines the ConsumerCnt per class.  */
/*                                          No dependences.                     */
/*                              LSA_TRUE :  ConsumerCntClass1 and Class2        */
/*                                          have the same value and defines     */
/*                                          the maximum Consumers for both      */
/*                                          classes.                            */
/* ConsumerCntClass1          : Number of xRT Class 1 Consumer             (out)*/
/* ProviderCnt                : Number of Provider (xRT and xRToverUDP)    (out)*/
/*                            : Class1 and Class2                               */
/* ProviderCntClass3          : Number of Provider (IRT). Class 3          (out)*/
/* ProviderGroupCnt           : Number of supported provider groups        (out)*/
/* DataLenMin                 : Min DataLen of I/O-area. Not Frame length! (out)*/
/*                              of xRT-Frames                                   */
/*                              equal to EDD_CSRT_DATALEN_MIN                   */
/* DataLenMax                 : Max DataLen of I/O-area. Not Frame length! (out)*/
/*                              of xRT-Frames                                   */
/*                              equal to EDD_CSRT_DATALEN_MAX                   */
/* DataLenMinUDP              : Min DataLen of I/O-area. Not Frame length! (out)*/
/*                              of xRToverUDP-Frames.                           */
/*                              equal to EDD_CSRT_UDP_DATALEN_MIN               */
/* DataLenMaxUDP              : Max DataLen of I/O-area. Not Frame length! (out)*/
/*                              of xRToverUDP-Frames.                           */
/*                              equal to EDD_CSRT_UDP_DATALEN_MAX               */
/* ProviderCyclePhaseMaxByteCnt     : Maximum number of frame data bytes of all */
/*                                    providers that are permitted in a cycle.  */
/*                                    Is not checked at PROVIDER activate. (out)*/
/*                                      0: no limit                             */
/* ProviderCyclePhaseMaxCnt         : Maximum number of all providers that are  */
/*                                    permitted in a cycle.                     */
/*                                    Is checked at PROVIDER activate (out)     */
/*                                      0: no limit                             */
/* KRAMInfo                   : Only used with ERTEC. KRAM usage.          (out)*/
/* RsvIntervalRed             : Structure with parameters regarding        (out)*/
/*                              reserved red interval.                          */


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SET_IP                          */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SET_IP_TYPE
{
    EDD_IP_ADR_TYPE                 LocalIP;
} EDD_RQB_SET_IP_TYPE;

typedef struct _EDD_RQB_SET_IP_TYPE      EDD_UPPER_MEM_ATTR *    EDD_UPPER_SET_IP_PTR_TYPE;

/* LocalIP            :   Local IP-Address used with xRT over UDP       (in) */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SET_LINE_DELAY                  */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SET_LINE_DELAY_TYPE
{
    LSA_UINT16                      PortID;
    LSA_UINT32                      LineDelayInTicksMeasured;
    LSA_UINT32                      CableDelayInNsMeasured;
} EDD_RQB_SET_LINE_DELAY_TYPE;

typedef struct _EDD_RQB_SET_LINE_DELAY_TYPE  EDD_UPPER_MEM_ATTR  * EDD_UPPER_SET_LINE_DELAY_PTR_TYPE;

/* PortID                     :   Port to set forwarding for.           (in) */
/*                                range: 1..FFFF                             */
/* LineDelayInTicksMeasured   :   Linedelay in ticks.                   (in) */
/*                                Depends on TimeResolutionInNs              */
/* CableDelayInNsMeasured     :   CableDelay in ns.                     (in) */


/* Note:                                                                     */
/* Changing LineDelayInTicksMeasured and CableDelayInNsMeasured will force   */
/* a extended link indication (see EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT)      */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_GET_PORT_PARAMS                 */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RESERVED_INTERVAL_TYPE
{
    LSA_UINT32   BeginNs;
    LSA_UINT32   EndNs;
} EDD_RESERVED_INTERVAL_TYPE;

typedef struct _EDD_RESERVED_INTERVAL_TYPE  EDD_UPPER_MEM_ATTR * EDD_RESERVED_INTERVAL_PTR_TYPE;

typedef struct _EDD_RESERVED_INTERVAL_RX_TX_TYPE
{
    EDD_RESERVED_INTERVAL_TYPE  Rx;
    EDD_RESERVED_INTERVAL_TYPE  Tx;
} EDD_RESERVED_INTERVAL_RX_TX_TYPE;

typedef struct _EDD_RESERVED_INTERVAL_TYPE_RED
{
    LSA_UINT32   BeginNs;
    LSA_UINT32   EndNs;
    LSA_UINT32   EndLocalNs;
    LSA_UINT32   LastLocalFrameLen;
} EDD_RESERVED_INTERVAL_TYPE_RED;

typedef struct _EDD_RESERVED_INTERVAL_TYPE_RED  EDD_UPPER_MEM_ATTR * EDD_RESERVED_INTERVAL_RED_PTR_TYPE;

typedef struct _EDD_RESERVED_INTERVAL_RX_TX_TYPE_RED
{
    EDD_RESERVED_INTERVAL_TYPE_RED  Rx;
    EDD_RESERVED_INTERVAL_TYPE_RED  Tx;
} EDD_RESERVED_INTERVAL_RX_TX_TYPE_RED;

typedef struct _EDD_RQB_GET_PORT_PARAMS_TYPE
{
    LSA_UINT16                         PortID;
    LSA_UINT8                          MediaType;
    EDD_MAC_ADR_TYPE                   MACAddr;
    LSA_UINT8                          PortStatus;
    LSA_UINT8                          PhyStatus;
    LSA_UINT8                          IRTPortStatus;
    LSA_UINT32                         LineDelayInTicksMeasured;
    LSA_UINT32                         CableDelayInNsMeasured;
    LSA_UINT8                          RTClass2_PortStatus;
    LSA_UINT8                          MRPRingPort;
    LSA_UINT8                          LinkSpeedModeConfigured;
    LSA_UINT32                         AutonegCapAdvertised;
    EDD_RESERVED_INTERVAL_RX_TX_TYPE_RED RsvIntervalRed;
    EDD_RESERVED_INTERVAL_RX_TX_TYPE   RsvIntervalOrange;
    LSA_BOOL                           IsWireless;
    LSA_BOOL                           IsMDIX;
    LSA_BOOL                           ShortPreamble;
    LSA_BOOL                           TxFragmentation;
} EDD_RQB_GET_PORT_PARAMS_TYPE;

typedef struct _EDD_RQB_GET_PORT_PARAMS_TYPE  EDD_UPPER_MEM_ATTR  * EDD_UPPER_GET_PORT_PARAMS_PTR_TYPE;

/* PortID                     :   Port to set forwarding for.           (in) */
/* MediaType                  :   Media Type                            (out)*/
/* MACAddr                    :   Port MAC address from DSB. 0= not used(out)*/
/* PortStatus                 :   Status of Port                        (out)*/
/*                                 EDD_PORT_PRESENT                          */
/*                                 EDD_PORT_NOT_PRESENT (only if Link down)  */
/* PhyStatus                  :   Status of the PHY                     (out)*/
/*                                 EDD_PHY_STATUS_ENABLED                    */
/*                                 EDD_PHY_STATUS_DISABLED                   */
/*                                 EDD_PHY_STATUS_UNKNOWN                    */
/* IRTPortStatus              :   IRT Status of Port                    (out)*/
/*                                 EDD_IRT_NOT_SUPPORTED                     */
/*                                 EDD_IRT_PORT_INACTIVE                     */
/*                                 EDD_IRT_PORT_ACTIVE_TX_UP                 */
/*                                 EDD_IRT_PORT_ACTIVE_TX_DOWN               */
/*                                 EDD_IRT_PORT_ACTIVE_RX                    */
/*                                 EDD_IRT_PORT_ACTIVE_TX_RX                 */
/* LineDelayInTicksMeasured   :   Linedelay in ticks                    (out)*/
/*                                Depends on TimeResolutionInNs              */
/* CableDelayInNsMeasured     :   CableDelay in ns.                     (out)*/
/* RTClass2_PortStatus        :   RT Class2 Port Status                 (out)*/
/*                                  EDD_RTCLASS2_NOT_SUPPORTED               */
/*                                  EDD_RTCLASS2_INACTIVE                    */
/*                                  EDD_RTCLASS2_ACTIVE                      */
/* MRPRingPort                :   Defines if this port is a ring port   (out)*/
/*                                used with MRP and if this port is a        */
/*                                default ring port on startup               */

#define EDD_MRP_NO_RING_PORT                 0x00 /* no ring port            */
#define EDD_MRP_RING_PORT                    0x01 /* ring port but no default*/
#define EDD_MRP_RING_PORT_DEFAULT            0x02 /* default ring port       */


/* LinkSpeedModeConfigured    :   Configured LinkSpeedMode              (out)*/
/*                                                                           */

#define EDD_LINK_AUTONEG        0
#define EDD_LINK_100MBIT_HALF   1
#define EDD_LINK_100MBIT_FULL   2
#define EDD_LINK_10MBIT_HALF    3
#define EDD_LINK_10MBIT_FULL    4
#define EDD_LINK_1GBIT_HALF     5
#define EDD_LINK_1GBIT_FULL     6
#define EDD_LINK_10GBIT_HALF    7
#define EDD_LINK_10GBIT_FULL    8
#define EDD_LINK_UNCHANGED      9   /* for edd internal use only ! */

/* AutonegCapAdvertised       :   Autonegotiation advertised capabilit. (out)*/
/*                                Can be ORed!! refers to RFC 3636           */
/*                                0 if no Autoneg supported                  */

#define EDD_AUTONEG_CAP_NONE               0x00000000 //no Autoneg supported
#define EDD_AUTONEG_CAP_OTHER              0x00000001
#define EDD_AUTONEG_CAP_10BASET            0x00000002 //10MBit over RJ45-Conn큦 (2*twistedpair),  half duplex
#define EDD_AUTONEG_CAP_10BASETFD          0x00000004 //10MBit over RJ45-Conn큦 (2*twistedpair),  full duplex
#define EDD_AUTONEG_CAP_100BASET4          0x00000008 //100MBit over RJ45-Conn큦 (4*twistedpair, CAT3),  half duplex
#define EDD_AUTONEG_CAP_100BASETX          0x00000010 //100MBit over RJ45-Conn큦 (2*twistedpair, MLT3-Coding, CAT5),  half duplex
#define EDD_AUTONEG_CAP_100BASETXFD        0x00000020 //100MBit over RJ45-Conn큦 (2*twistedpair, MLT3-Coding, CAT5),  full duplex
#define EDD_AUTONEG_CAP_100BASET2          0x00000040 //100MBit over RJ45-Conn큦 (2*twistedpair, CAT3),  half duplex
#define EDD_AUTONEG_CAP_100BASET2FD        0x00000080 //100MBit over RJ45-Conn큦 (2*twistedpair, CAT3), full duplex
#define EDD_AUTONEG_CAP_FDXPAUSE           0x00000100 //Pause for full duplex links (symmetry unknown)
#define EDD_AUTONEG_CAP_FDXAPAUSE          0x00000200 //Asymmetric pause for full duplex links
#define EDD_AUTONEG_CAP_FDXSPAUSE          0x00000400 //Symmetric pause for full duplex links
#define EDD_AUTONEG_CAP_FDXBPAUSE          0x00000800 //Asymmetric and symmetric pause for full duplex links
#define EDD_AUTONEG_CAP_1000BASEX          0x00001000 //1GBit (-LX, -CX over RJ45, -SX) half duplex
#define EDD_AUTONEG_CAP_1000BASEXFD        0x00002000 //1GBit (-LX, -CX over RJ45, -SX) full duplex
#define EDD_AUTONEG_CAP_1000BASET          0x00004000 //1GBit over RJ45-Conn큦 (4*twistedpair, CAT5e), half duplex
#define EDD_AUTONEG_CAP_1000BASETFD        0x00008000 //1GBit over RJ45-Conn큦 (4*twistedpair, CAT6), full duplex

/* RsvIntervalRed             : Red Part of Reserved Interval                */
/*                              (contains IRT_Frames only)                   */
/*                              Interval is derived from currently projected */
/*                              IR_DATA_RECORD.                              */
/* RsvIntervalRed.Tx.BeginNs  : Start of the "red" tx interval (period) (out)*/
/*                              in ns for this port.                         */
/* RsvIntervalRed.Tx.EndNs    : End of the "red" tx interval (period)   (out)*/
/*                              in ns for this port.                         */
/* RsvIntervalRed.Tx.EndLocalNs:FrameSendOffset of the last local       (out)*/
/*                              IRT-Frame send at this port                  */
/* RsvIntervalRed.Tx.LastLocalFrameLen : Length of the last local       (out)*/
/*                              IRT-Frame sent at this port                  */
/* RsvIntervalRed.Rx.BeginNs  : Start of the "red" rx interval (period) (out)*/
/*                              in ns for this port.                         */
/* RsvIntervalRed.Rx.EndNs    : End of the "red" rx interval (period)   (out)*/
/*                              in ns for this port.                         */
/*                              BeginNs == 0 and EndNs == 0 means:           */
/*                              No Red Interval was projected.               */
/* RsvIntervalRed.Rx.EndLocalNs:FrameSendOffset of the last local       (out)*/
/*                              IRT-Frame received at this port              */
/* RsvIntervalRed.Rx.LastLocalFrameLen : Length of the last local       (out)*/
/*                              IRT-Frame received at this port              */
/* RsvIntervalOrange          : Orange Part of Reserved Interval             */
/*                              (contains both ISRT and IRT_Frames           */
/*                              Interval is derived from currently projected */
/*                              IR_DATA_RECORD and SYNC_RECORD               */
/*                              BeginNs == 0 and EndNs == 0 means:           */
/*                              No Orange Interval was projected.            */
/* RsvIntervalOrange.Tx.BeginNs:Start of the combined "red" and "orange"(out)*/
/*                              period in ns for this port                   */
/* RsvIntervalOrange.Tx.EndNs : End of the combined "red" and "orange"  (out)*/
/*                              period in ns for this port                   */
/*                              BeginNs == 0 and EndNs == 0 means:           */
/*                              No Orange Interval was projected.            */
/* RsvIntervalOrange.Rx.BeginNs: no Orange Part exist                   (out)*/
/* RsvIntervalOrange.Rx.EndNs  : no Orange Part exist                   (out)*/
/* IsWireless                  : Is this a wireless port? (config from DSB)  */
/*                                 LSA_TRUE:  This is a wireless port        */
/*                                 LSA_FALSE: This is no wireless port       */
/* IsMDIX                      : Does the PHY cross rx/tx wires if link has  */
/*                               no autoneg?                                 */
/*                                 LSA_TRUE:  yes                            */
/*                                 LSA_FALSE: no                             */
/* ShortPreamble               : ShortPreamble active for sending       (out)*/
/*                                  LSA_TRUE:  Preamble length: 1 Byte       */
/*                                  LSA_FALSE: Preamble length: 7 Bytes      */
/* TxFragmentation             : Tx frame fragmentation activated?      (out)*/
/*                                 LSA_TRUE:  activated                      */
/*                                 LSA_FALSE: not activated                  */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_RX_OVERLOAD_IND_PROVIDE         */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_RX_OVERLOAD_IND_PROVIDE_TYPE
{
    LSA_UINT32          Status;
} EDD_RQB_RX_OVERLOAD_IND_PROVIDE_TYPE;

typedef struct _EDD_RQB_RX_OVERLOAD_IND_PROVIDE_TYPE  EDD_UPPER_MEM_ATTR  * EDD_UPPER_RX_OVERLOAD_IND_PROVIDE_PTR_TYPE;

/* Status             :   Overload Status                            (in/out)*/

#define EDD_NO_RX_OVERLOAD    0           /* no overload.                    */
#define EDD_RX_OVERLOAD       1           /* overload occured. Rx stopped    */


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SENDCLOCK_CHANGE                */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [d]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SENDCLOCK_CHANGE_TYPE
{
    LSA_UINT16      CycleBaseFactor;
} EDD_RQB_SENDCLOCK_CHANGE_TYPE;

/* description of parameters see EDD_SRV_GET_PARAMS */

typedef struct _EDD_RQB_SENDCLOCK_CHANGE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SENDCLOCK_CHANGE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_LED_BLINK                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_LED_BLINK_TYPE
{
    LSA_UINT32    dummy;     /* don큧 care */
} EDD_RQB_LED_BLINK_TYPE;

typedef struct _EDD_RQB_LED_BLINK_TYPE    EDD_UPPER_MEM_ATTR *    EDD_UPPER_LED_BLINK_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SET_SYNC_TIME               */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SET_SYNC_TIME_TYPE
{
    LSA_UINT32      ReferenceTimeStampInTicks; /* ticks         */
    LSA_UINT32      PTCPSeconds;
    LSA_UINT32      PTCPNanoSeconds;
    LSA_INT32       RCFInterval;
} EDD_RQB_SET_SYNC_TIME_TYPE;

typedef struct _EDD_RQB_SET_SYNC_TIME_TYPE    EDD_UPPER_MEM_ATTR *  EDD_UPPER_SET_SYNC_TIME_PTR_TYPE;


/*  CycleBeginTimeStamp  :  CycleBeginTimeStamp marks the point in (local) time         */
/*                          to which the CycleCounter relates.                          */
/*  PTCPSeconds          :  Seconds of PTCP time                                        */
/*  PTCPNanoSeconds      :  NanoSeconds of PTCP time                                    */
/*  RCFInterval          :  Current RFCInterval wich represents the current             */
/*                           *R*ate *C*ompensation *F*actor                             */
/*                             (==RCF = f(Masterquartz) / f(Slavequartz)).              */
/*                           Needed by EDD to limit errors caused by                    */
/*                           reaction time.                                             */
/*                           Relation between RCFInterval and RCF:                      */
/*                           RCFInterval = 1 / (RCF - 1)                                */
/*                                                                                      */
/*                           RCFInterval <  0 :Slavequartz is faster than Masterquartz  */
/*                           RCFInterval >  0 :Slavequartz is slower than Masterquartz  */
/*                           RCFInterval == 0 :Slavequartz is as fast as  Masterquartz  */
/*                                             --> EDD will do no correction of reaction*/
/*                                                 time                                 */
/*                                             Use this as DEFAULT value if the current */
/*                                             RCFInterval  is unknown.                 */


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SET_SYNC_STATE                  */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [d]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SET_SYNC_STATE_TYPE
{
    LSA_UINT32      SyncState;
} EDD_RQB_SET_SYNC_STATE_TYPE;

typedef struct _EDD_RQB_SET_SYNC_STATE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SET_SYNC_STATE_PTR_TYPE;

/* SyncState          :   State of synchronisation for SyncID 0         (in) */

#define EDD_SYNC_STATE_BAD   0
#define EDD_SYNC_STATE_GOOD  1

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SET_REMOTE_PORT_STATE           */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [d]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SET_REMOTE_PORT_STATE_TYPE
{
  LSA_UINT16     PortID;
  LSA_UINT32     SyncId0_TopoState;
  LSA_UINT32     RtClass2_TopoState;
  LSA_UINT32     RtClass3_TopoState;
  LSA_UINT32     RtClass3_PDEVTopoState;
  LSA_UINT32     RtClass3_RemoteState;
  LSA_UINT32     Neighbour_TopoState;

} EDD_RQB_SET_REMOTE_PORT_STATE_TYPE;

typedef struct _EDD_RQB_SET_REMOTE_PORT_STATE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SET_REMOTE_PORT_STATE_PTR_TYPE;

#define EDD_SET_REMOTE_PORT_STATE_IGNORE          0xFFFFFFFF

#define EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH   0
#define EDD_SET_REMOTE_PORT_STATE_TOPO_OK         1

#define EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF   0
#define EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX    1
#define EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX  2

/* Service to deliver information about the remote port state to the EDD.    */
/* This Service shall be called if one of the following values had           */
/* changed on remote side.                                                   */
/* Irrelevant parameters must be filled with                                 */
/* EDD_SET_REMOTE_PORT_STATE_IGNORE.                                         */
/* This services triggers several state machines concerning sync frame       */
/* forwarding and IRT forwarding.                                            */
/*                                                                           */
/* PortID             :   Port to set the remote state for.             (in) */
/*                        range: 1..FFFF                                     */
/*                                                                           */
/* SyncId0_TopoState:     State of neighbourhood check for SyncID0      (in) */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_OK        */
/*                                                                           */
/*                        Note: Used for SYNC-Forwarding.                    */
/*                              Neighbour SubdomainUUID, MasterMAC ok        */
/*                                                                           */
/* RtClass2_TopoState:    State of neighbourhood check for RtClass2     (in) */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_OK        */
/*                                                                           */
/*                        Note: RTC2PSM Variable: NEIGHBOR and               */
/*                                                SYNC(Diff_Fault)           */
/*                                                                           */
/* RtClass3_TopoState     State of topology neighbourhood check for RTClass3 */
/*                        (=ChassisID,PortID,MAUType,CableDelay,LineDelay)   */
/*                                                                           */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_OK        */
/*                                                                           */
/*                        Note: RTC3PSM Variable: NEIGHBOR and               */
/*                                                SYNC(Diff_Fault)           */
/*                                                                           */
/* RtClass3_PDEVTopoState State of physical device topology neighbourhood    */
/*                        check for RtClass3                           (in)  */
/*                        (=IRDataUUID, PTCP_SubdomainUUID)                  */
/*                                                                           */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_OK        */
/*                                                                           */
/*                        Note: RTC3PSM Variable: ENGINEERING                */
/*                                                                           */
/* RtClass3_RemoteState   RtClass3 activity of remote port              (in) */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF  */
/*                                  EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX   */
/*                                  EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX */
/*                                                                           */
/*                        Note: RTC3PSM Variable: REMOTEPORTSTATE            */
/*                                                                           */
/* Neighbour_TopoState    State of neighbourhood check needed for       (in) */
/*                        short preamble, fragmentation, yellowTime setup.   */
/*                        (=ChassisID,PortID,MAUType,IRDataUUID)             */
/*                                  EDD_SET_REMOTE_PORT_STATE_IGNORE         */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_MISMATCH  */
/*                                  EDD_SET_REMOTE_PORT_STATE_TOPO_OK        */
/*                                                                           */
/*                                                                           */
/*                                                                           */

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= NRT-INTERFACE ===================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_SEND                        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_SEND_TIMESTAMP              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_SEND                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef enum _EDD_NRT_TX_STATUS_TYPE
{
  NRT_TX_OK,
  NRT_TX_ERR_LEN_OR_TAG,
  NRT_TX_ERR_NO_MEM,
  NRT_TX_ERR_SRC_MAC_FDB

} EDD_NRT_TX_STATUS_TYPE;

typedef struct _EDD_RQB_NRT_SEND_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE            pBuffer;
    LSA_UINT32                           Length;
    LSA_UINT32                           Priority;
    LSA_UINT16                           PortID;
    LSA_UINT16                           FrameID;                /* 1) */
    LSA_BOOL                             TxTimePresent;          /* 1) */
    LSA_UINT32                           TxTime;
    LSA_UINT32                           LineDelayInTicksHardware;
    LSA_UINT32                           TxFrmGroup;
    //IRTE ONLY!!
    EDD_NRT_TX_STATUS_TYPE               Status;
} EDD_RQB_NRT_SEND_TYPE;

/* 1) only used with EDD_SRV_SYNC_SEND */

typedef struct _EDD_RQB_NRT_SEND_TYPE       EDD_UPPER_MEM_ATTR *    EDD_UPPER_NRT_SEND_PTR_TYPE;

/* pBuffer            :   Pointer to buffer to send                     (in) */
/* Length             :   Length of buffer to send in bytes             (in) */

#define EDD_NRT_MAX_SND_LEN         EDD_IEEE_FRAME_LENGTH
                                        /* maximum Frame length without CRC  */
                                        /* (includes vlan-tagged long frames)*/
#define EDD_NRT_MIN_SND_LEN         14  /* at least dst/src-mac and type/len */

/* Priority           :   Priority of handling                          (in) */
/*                        NOTE: EDDP uses the new entry <TxFrmGroup>         */

#define EDD_NRT_SEND_PRIO_0         0
#define EDD_NRT_SEND_PRIO_1         1
#define EDD_NRT_SEND_PRIO_2         2
#define EDD_NRT_SEND_PRIO_3         3
#define EDD_NRT_SEND_PRIO_4         4
#define EDD_NRT_SEND_PRIO_5         5
#define EDD_NRT_SEND_PRIO_6         6
#define EDD_NRT_SEND_PRIO_7         7
#define EDD_NRT_SEND_PRIO_ASRT_LOW  8
#define EDD_NRT_SEND_PRIO_ASRT_HIGH 9
#define EDD_NRT_SEND_PRIO_MGMT_LOW  10  //LLDP
#define EDD_NRT_SEND_PRIO_MGMT_HIGH 11  //GSY, MRP

/* PortID             :   Port to use for sending the frame             (in) */
/*                        Only needed if there are multiple ports            */
/*                        present for sending a frame.                       */
/*                        range: 1..FFFF or EDD_PORT_ID_AUTO                 */

#define EDD_PORT_ID_AUTO        0x0000  /* let EDD select a PORT             */
#define EDD_PORT_ID_LOCAL       0xFFFF  /* local Switch Port ERTEC only      */

/* FrameID            :   FrameID of Sync-Frame to be send              (in) */
/*                        only used with EDD_SRV_SYNC_SEND!                  */
/* TxTimePresent      :   Timestamp (TxTime) present                    (out)*/
/*                        only used with EDD_SRV_SYNC_SEND!                  */
/* TxTime             :   Timestamp.                                    (out)*/
/*                        Only used with EDD_SRV_NRT_SEND_TIMESTAMP and      */
/*                        EDD_SRV_SYNC_SEND!                                 */
/*                        based on TimeResolutionInNS                        */
/* LineDelayInTicksHardware    :   LineDelay set within Hardware        (out)*/
/*                        Only used with EDD_SRV_NRT_SEND_TIMESTAMP and      */
/*                        EDD_SRV_SYNC_SEND!                                 */
/*                        Only set with hardware adding a LineDelay to SYNC  */
/*                        frames on sending (e.g. ERTEC 400). else 0.        */
/*                        based on TimeResolutionInNS                        */

/* TxFrmGroup         :   Selects TX-queue indirectly for TX-handling   (in) */
/*                        NOTE: EDDP uses this new entry only. Queue-TX-     */
/*                              scheduling depends on Frame Classification,  */
/*                              so timepoint of transmitting can be different*/
/*                              to putting sequence of the Tx-RQBs!          */

#define EDD_NRT_TX_GRP_DEFAULT          0   /* Queue: DEFAULT=0              */
                                            /* default TX                    */

#define EDD_NRT_TX_GRP_IP_ICMP          1   /* Queue: IP_MISC=3              */
                                            /* IP: protocol 1=ICMP           */
#define EDD_NRT_TX_GRP_IP_IGMP          2   /* Queue: IP_MISC=3              */
                                            /* IP: protocol 2=IGMP           */
#define EDD_NRT_TX_GRP_IP_TCP           3   /* Queue: IP_TCP=1               */
                                            /* IP: protocol 6=TCP            */
#define EDD_NRT_TX_GRP_IP_UDP           4   /* Queue: IP_UDP=2               */
                                            /* IP: protocol 17=UDP           */
#define EDD_NRT_TX_GRP_ARP              5   /* Queue: IP_MISC=3              */
                                            /* AddressResolutionProtocol     */
#define EDD_NRT_TX_GRP_DCP              6   /* Queue: DCP=4                  */
                                            /* DeviceControlProtocol or      */
                                            /* xRToverDUP with FrmIDs: DCP   */
#define EDD_NRT_TX_GRP_ASRT             7   /* Queue: ALARM=5                */
                                            /* xRT/xRToverDUP with FrmID:ASRT*/
#define EDD_NRT_TX_GRP_MRP              8   /* Queue: MRP=6                  */
                                            /* MediaRedundancyProtocol       */
#define EDD_NRT_TX_GRP_LEN_TSYNC        9   /* Queue: DEFAULT=0              */
                                            /* LEN types with clocktime sync */
#define EDD_NRT_TX_GRP_LEN_STDBY        10  /* Queue: MRP=6                  */
                                            /* LEN types with STDBY SNAP     */
#define EDD_NRT_TX_GRP_LEN_HSR          11  /* Queue: MRP=6                  */
                                            /* LEN types with HSR SNAP       */

#define EDD_NRT_TX_GRP_LLDP             12  /* Queue: LLDP=7                 */
                                            /* LinkLayerDiscoveryProtocol    */
#define EDD_NRT_TX_GRP_PTCP             13  /* Queue: SYNC=8                 */
                                            /* PrecisionTransparentClockProt.*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_RECV                        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_RECV                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef enum _EDD_NRT_RX_STATUS_TYPE
{
  NRT_RX_OK,
  NRT_RX_ERR_BUF_LEN,
  NRT_RX_ERR_BUF_ADR,
  NRT_RX_ERR_NO_MEM

} EDD_NRT_RX_STATUS_TYPE;

typedef struct _EDD_RQB_NRT_RECV_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE        pBuffer;
    LSA_UINT32                       RequestID;
    LSA_UINT16                       IOCount;
    LSA_UINT16                       UserDataLength;
    LSA_UINT16                       UserDataOffset;
    LSA_UINT16                       PortID;
    LSA_UINT32                       FrameFilter;
    LSA_UINT32                       FrameFilterInfo;
    LSA_UINT32                       RequestCnt;
    LSA_UINT32                       RxTime;
    LSA_BOOL                         doSwFwd[EDD_CFG_MAX_PORT_CNT];     /* 1) */
    //IRTE ONLY!!
    LSA_BOOL                         Tag;
    EDD_NRT_RX_STATUS_TYPE           Status;

    LSA_BOOL                         TE_Event; /* ERTEC 200: TriggerEvent 0 : no trigger of the FrameTriggerUnit occurred -- 1 : Trigger occurred*/
    LSA_BOOL                         SE_Event; /* ERTEC 200 SelectorEvent 0 : */
} EDD_RQB_NRT_RECV_TYPE;

/* 1) only used with EDD_SRV_SYNC_RECV */

typedef struct _EDD_RQB_NRT_RECV_TYPE       EDD_UPPER_MEM_ATTR *    EDD_UPPER_NRT_RECV_PTR_TYPE;

/* pBuffer            :   Pointer to buffer to put received data to     (in) */
/*                        must match receive-buffer requirements!!           */
/* RequestID          :   Optional Request-ID used for canceling of     (in) */
/*                        requests.                                          */
/* IOCount            :   Number of bytes received                      (out)*/
/* UserDataLength     :   Length of UserData starting behind type/length(out)*/
/*                        field. (may contain padding data)                  */
/* UserDateOffset     :   Offset of start of UserData. Offset depends   (out)*/
/*                        on VLAN-tag present or not.                        */
/* PortID             :   PortID the frame was received from            (out)*/
/*                        range: 1..FFFF                                     */
/*                        1: if only one port present.                       */
/* FrameFilter        :   FrameFilter applied on receive                (out)*/
/*                        (only one!)                                        */

#define EDD_NRT_FRAME_ARP           0x00000001L  /*  ARP                             */
#define EDD_NRT_FRAME_LEN_DCP       0x00000002L  /*  DCP with SNAP-Header            */
#define EDD_NRT_FRAME_LEN_TSYNC     0x00000004L  /*  TSYNC with SNAP-Header          */
#define EDD_NRT_FRAME_LEN_OTHER     0x00000008L  /*  LEN-Type Frames not DCP/TSYNC   */
#define EDD_NRT_FRAME_OTHER         0x00000010L  /*  Type-Frames not IP/DCP/LLDP/xRT */
#define EDD_NRT_FRAME_DCP           0x00000020L  /*  DCP-Frames with xRT-Header      */
#define EDD_NRT_FRAME_LLDP          0x00000040L  /*  LLDP-Frames                     */
#define EDD_NRT_FRAME_ASRT          0x00000080L  /*  ASRT-Frames                     */
#define EDD_NRT_FRAME_UDP_ASRT      0x00000100L  /*  xRToverUDP ASRT type frames     */
#define EDD_NRT_FRAME_UDP_DCP       0x00000200L  /*  xRToverUDP DCP type frames      */
#define EDD_NRT_FRAME_PTCP_SYNC     0x00000400L  /*  xRT PTCP frames Sync/FU Frames  */
#define EDD_NRT_FRAME_MRP           0x00000800L  /*  MRP frames                      */
#define EDD_NRT_FRAME_IP_ICMP       0x00001000L  /*  Internet Control Message Prot.  */
#define EDD_NRT_FRAME_IP_IGMP       0x00002000L  /*  Internet Group Management Prot. */
#define EDD_NRT_FRAME_IP_TCP        0x00004000L  /*  Transmission Control Protocol   */
#define EDD_NRT_FRAME_IP_UDP        0x00008000L  /*  User Datagram Protocol          */
#define EDD_NRT_FRAME_IP_OTHER      0x00010000L  /*  all other IP protocols          */
#define EDD_NRT_FRAME_RARP          0x00020000L  /*  reverse-ARP                     */
#define EDD_NRT_FRAME_DCP_HELLO     0x00040000L  /*  DCP-HELLO Frame with xRT-Header */
#define EDD_NRT_FRAME_UDP_DCP_HELLO 0x00080000L  /*  DCP-HELLO Frame with UDP-Header */
#define EDD_NRT_FRAME_LEN_STDBY     0x00100000L  /*  MRP-Standby SNAP Frame          */
#define EDD_NRT_FRAME_LEN_HSR       0x00200000L  /*  HSR SNAP Frame                  */
#define EDD_NRT_FRAME_PTCP_ANNO     0x00400000L  /*  xRT PTCP frames Announce Frames */
#define EDD_NRT_FRAME_PTCP_DELAY    0x00800000L  /*  xRT PTCP frames Delay Frames    */
#define EDD_NRT_FRAME_LEN_SINEC_FWL 0x01000000L  /*  SINEC FW loader                 */

/* FrameFilterInfo    :   Additional Info depending on FrameFilter.     (out)*/
/* RequestCnt         :   Pending number of recv-requests in EDD        (out)*/
/* RxTime             :   Timestamp of received frame.                  (out)*/
/*                        based on TimeResolutionInNS                        */
/*                        (only if timestamps supported)                     */
/* doSwFwd            :   LSA_TRUE if user shall forward the frame to   (out)*/
/*                        this port if possible. index 0: PortID 1 etc.      */
/*                        only valid with PTCP frames!                       */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_CANCEL                      */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_CANCEL                     */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_NRT_CANCEL_TYPE
{
    LSA_UINT8                           Mode;
    LSA_UINT32                          RequestID;
} EDD_RQB_NRT_CANCEL_TYPE;

typedef struct _EDD_RQB_NRT_CANCEL_TYPE     EDD_UPPER_MEM_ATTR *    EDD_UPPER_NRT_CANCEL_PTR_TYPE;

/* Mode               :   Mode of Cancel.                               (in) */
/* RequestID          :   Only used with EDD_NRT_CANCEL_MODE_RX_BY_ID   (in) */
/*                        Specifies the Request-ID of the Requests to        */
/*                        to cancel.                                         */

#define  EDD_NRT_CANCEL_MODE_ALL      0   /* cancel all requests                   */
#define  EDD_NRT_CANCEL_MODE_RX_ALL   1   /* cancel all rx-requests                */
#define  EDD_NRT_CANCEL_MODE_TX_ALL   2   /* cancel all tx-requests                */
#define  EDD_NRT_CANCEL_MODE_RX_BY_ID 3   /* cancel all rx-requests with RequestID */

#define  EDD_NRT_CANCEL_MODE_MAX      3   /* Max Cancel Mode. internal use         */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_SET_DCP_FILTER              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_NRT_SET_DCP_FILTER_TLV_TYPE
{
    LSA_UINT32                              TLVLen;
    EDD_UPPER_MEM_PTR_TYPE                  pTLV;
} EDD_RQB_NRT_SET_DCP_FILTER_TLV_TYPE;

typedef struct _EDD_RQB_NRT_SET_DCP_FILTER_TYPE
{
    LSA_UINT8                               Mode;
    LSA_UINT16                              FilterTLVCnt;
    EDD_RQB_NRT_SET_DCP_FILTER_TLV_TYPE     FilterTLV[EDD_CFG_MAX_PORT_CNT+1];
} EDD_RQB_NRT_SET_DCP_FILTER_TYPE;

typedef struct _EDD_RQB_NRT_SET_DCP_FILTER_TYPE     EDD_UPPER_MEM_ATTR *    EDD_UPPER_NRT_SET_DCP_FILTER_PTR_TYPE;

/* Mode               :   Mode of Request                               (in) */
/*                          EDD_DCP_FILTER_ON                                */
/*                              Deactivates all filter TLVs                  */
/*                                                                           */
/*                          EDD_DCP_FILTER_OFF                               */
/*                              Sets the filter TLVs defined by FilterTLVCnt */
/*			                    and FilterTLV. All previously set filter     */
/*                              TLVs are automatically deactivated           */

#define EDD_DCP_FILTER_ON   1
#define EDD_DCP_FILTER_OFF  0

/* FilterTLVCnt       :   Only used with Mode = EDD_DCP_FILTER_ON:           */
/*                        Specifies how many FilterTLV entrys are present    */
/*                        within FilterTLV array and shall be evaluated      */
/*                        by EDD.                                            */
/*                        Possible values: 1...MaxPortCnt+1                  */

/* FilterTLV[]        :   Only used with Mode = EDD_DCP_FILTER_ON:           */
/*                        Array with FilterTLVCnt Filter TLV entrys to be    */
/*                        evaluated. All other entrys are not evaluated      */
/*                        and don't care.                                    */

/*          TLVLen    :   Length of the TLV referenced by pTLV.              */
/*                        Possible values: 1..259.                           */
/*                        Note: The maximum length is limited to 259 because */
/*                        the maximum length of the alias name (value) is 255*/
/*                        and type + length = 4                              */
/*                          Type                      = 2   Bytes            */
/*                          Length                    = 2   Bytes            */
/*                          max name of station       = 240 Bytes            */
/*                          name of port              = 14  Bytes            */
/*                          seperator '.'             = 1   Bytes            */
/*                         _________________________________                 */
/*                                                    = 259 Bytes            */

/*          pTLV      :   Pointer to filter TLV according to the DCP         */
/*                        specification (16-bit type, 16-bit length,         */
/*                        <length> bytes value). Filtering in the EDDP is    */
/*                        performed as explained in section                  */
/*                        "DCP IDENTIFY Filter in PROFINET IO".              */
/*                        Notes:                                             */
/*                          - The "length-field" within the TLV shall be     */
/*                            equal to (TLVLen-4).                           */
/*                          - After confirmation the memory referenced by    */
/*                            pTLV is not used by EDD anymore! (the EDD      */
/*                            copies the TLV to internal structures)         */

#define EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN  259 /* TypeHi + TypeLo + LenHi + LenLo + Max Alias Name */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_SET_DCP_HELLO_FILTER        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE
{
    LSA_UINT8                                    Mode;
    LSA_UINT32                                   UserID;
    EDD_UPPER_MEM_U8_PTR_TYPE                    pNameOfStation;
    LSA_UINT32                                   NameOfStationLen;
} EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE;

typedef struct _EDD_RQB_NRT_SET_DCP_HELLO_FILTER_TYPE     EDD_UPPER_MEM_ATTR *    EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE;

#define EDD_DCP_MAX_DCP_HELLO_FILTER 32

/* Mode               :   Mode of Request                               (in) */

#define EDD_DCP_HELLO_FILTER_OFF_ALL  0
#define EDD_DCP_HELLO_FILTER_OFF      1
#define EDD_DCP_HELLO_FILTER_ON       2

/* UserID             :   ID of Filter to activate/deactivate           (in) */
/* pNameOfStation     :   Pointer to NameOfStation to Filter.           (in) */
/*                        (only if EDD_DCP_HELLO_FILTER_ON)                  */
/* NameOfStationLen   :   Length in bytes of NameOfStation              (in) */
/*                        Range: 1..240                                      */

#define EDD_DCP_MAX_DCP_HELLO_NAME_OF_STATION 240

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_NRT_SET_ARP_FILTER              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

#define EDD_MAX_ARP_FILTER  1       //1...8 supported by IRTE-IP

typedef struct _EDD_RQB_NRT_SET_ARP_FILTER_TYPE
{
    LSA_UINT8                                    Mode;
    LSA_UINT8                                    FilterCnt;
    EDD_IP_ADR_TYPE                              IPAddr[EDD_MAX_ARP_FILTER];
} EDD_RQB_NRT_SET_ARP_FILTER_TYPE;

typedef  struct  _EDD_RQB_NRT_SET_ARP_FILTER_TYPE  EDD_UPPER_MEM_ATTR  *  EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE;

#define EDD_ARP_FILTER_OFF      0
#define EDD_ARP_FILTER_ON       1

/* Mode               :   EDD_ARP_FILTER_OFF: Deactivates ARP-Filter    (in) */
/*                        EDD_ARP_FILTER_ON:  Activates the ARP-Filter       */
/*                                            with specified IP-addresses    */
/* FilterCnt          :   Number of Filters (IP-Addresses) within       (in) */
/*                        IPAddr array.                                      */
/*                        Range: 1..EDD_MAX_ARP_FILTERS (=1)                 */
/*                        Only used with Mode "EDD_ARP_FILTER_ON"            */
/* IPAddr             :   Array with IP addresses used as ARP-Filter.   (in) */
/*                        The number of entries used depends on "FilterCnt". */
/*                        Only used with Mode "EDD_ARP_FILTER_ON"            */

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= SRT-INTERFACE ===================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_IND_PROVIDE                 */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

#ifndef EDD_CFG_CSRT_MAX_INDICATIONS
#define EDD_CFG_CSRT_MAX_INDICATIONS        32  /* default if not defined */
#endif

/* Data-field (filled by EDD) */
typedef struct _EDD_RQB_CSRT_IND_DATA_TYPE
{
    LSA_UINT32                           UserID;
    LSA_UINT32                           Event;
    LSA_UINT32                           EventStatus;
    LSA_UINT16                           CycleCnt;
    LSA_UINT8                            DebugInfo[4]; //Byte 12..15 of DebugInfo
} EDD_RQB_CSRT_IND_DATA_TYPE;

typedef struct _EDD_RQB_CSRT_IND_DATA_TYPE      EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_IND_DATA_PTR_TYPE;

/* UserID:        UserID from Consumer or Provider - add                    */
/* Event:         Event, depending on Consumer or Provider                  */
/*                Signals changed events                                    */
/*                Events can be ORed.                                       */
/* EventStatus:   Current Status for all Events.                            */
/*                Events can be ORed                                        */
/*                Note:  EDD_CSRT_PROV_EVENT_DROP will not be set here      */
/* CycleCounter:  CycleCounter of last received frame                       */
/*                Only used with Consumer indication                        */
/*                (set to 0 if never received a Consumer)                   */
/* DebugInfo:     Detailed info if at least one MISS event is present. See  */
/*                edd_usr.doc for further explanations.                     */

/* Provider Event:                 */
#define EDD_CSRT_PROV_EVENT_DROP	                      0x00000002  //Is signaled if it was not possible to send all providers within a cycle (overload),
                                                                      //or a new cycle is to start before the last one was completed.
                                                                      //This indication is global (independent of the provider); that is, not assigned to any provider.

/* Consumer Event and EventStatus: */
#define EDD_CSRT_CONS_EVENT_MISS	                      0x00000004  //Is signaled if a frame was not received during several consecutive cycles, or if it is faulty,
                                                                      //(transfer status <> 0, or CycleCounter invalid), or if DataStatus is  "invalid"
                                                                      //The Consumer is stopped. (see also EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED)
#define EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED              0x00000008  //Is signaled if a frame was not received during several consecutive cycles, or if it is faulty,
                                                                      //(transfer status <> 0, or CycleCounter invalid), or if DataStatus is  "invalid" but
                                                                      //The Consumer is NOT stopped. (see also EDD_CSRT_CONS_EVENT_MISS)

#define EDD_CSRT_CONS_EVENT_AGAIN	                      0x00000010  //Is signaled if a valid frame with the DataStatus "Valid" was received for the first time after
                                                                      //Start or after a MISS.

#define EDD_CSRT_CONS_EVENT_RUN	                          0x00000020  //Is signaled if the Providerstate changes in the DataStatus from the "STOP" mode to the "RUN" mode.
#define EDD_CSRT_CONS_EVENT_STOP	                      0x00000040  //Is signaled if the Providerstate changes in the DataStatus from the "RUN" mode to the "STOP" mode.

#define EDD_CSRT_CONS_EVENT_STATION_OK	                  0x00000080  //Is signaled if the StationProblemIndicator changes in the Data Status from the "Failure" state
                                                                      //to the "OK" state.
#define EDD_CSRT_CONS_EVENT_STATION_FAILURE	              0x00000100  //Is signaled if the StationProblemIndicator changes in the Data Status from the "OK" state
                                                                      //to the "Failure" state.

#define EDD_CSRT_CONS_EVENT_PRIMARY	                      0x00000200  //Is signaled if State changes in the Data Status from the BACKUP state to the PRIMARY state.
#define EDD_CSRT_CONS_EVENT_BACKUP	                      0x00000400  //Is signaled if State changes in the Data Status from the PRIMARY state to the BACKUP state.

#define EDD_CSRT_CONS_EVENT_ENTERING_RED                  0x00000800  //This consumerframe has been received in the red phase for the first time at all, or when it
                                                                      //has been received in the orange or green phase before.
#define EDD_CSRT_CONS_EVENT_LEAVING_RED                   0x00001000  //This consumerframe has been received another than the red phase for the first time
                                                                      //(when it has been received in the red phase before).

#define EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS          0x00002000  //System Redundancy appears    (Bit 1 of the data-status changed from "0" to "1".)
#define EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS             0x00004000  //System Redundancy disappears (Bit 1 of the data-status changed from "1" to "0".)

#define EDD_CSRT_CONS_EVENT_NEGATION                      0x80000000  //!!!used internally only!!! Shall never appear at the user-if.

typedef struct _EDD_RQB_CSRT_IND_PROVIDE_TYPE
{
    LSA_UINT16                           Count;
    EDD_RQB_CSRT_IND_DATA_TYPE           Data[EDD_CFG_CSRT_MAX_INDICATIONS];
    LSA_UINT8                            DebugInfo[EDD_DEBUG_INFO_SIZE-4];
} EDD_RQB_CSRT_IND_PROVIDE_TYPE;

typedef struct _EDD_RQB_CSRT_IND_PROVIDE_TYPE   EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_IND_PROVIDE_PTR_TYPE;

/* Count: number of entrys filled by edd within data-array              (out)*/
/* Data:  filled with events by EDD                                     (out)*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_SET_STATE          */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE
{
    LSA_UINT16                          GroupID;
    LSA_UINT8                           Status;
    LSA_UINT8                           Mask;
} EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE;

typedef struct _EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE    EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE;

/* GroupID            : Provider Group to change global Provider Status (in) */
/* Status/Mask        : Bits of DataStatus to be modified               (in) */

/* Status : New Bit-value           */
/* Mask   : Which Bit to set        */

/* values for Status and Mask. Can be ORed                        */
/* Note: Not all are valid. see EDD_CSRT_DSTAT_GLOBAL_MASK_ALL !  */

/*---------------------------------------------------------------------------*/
/* Data-Status Bits                                                          */
/*                                                                           */
/* Bit| Name           | Values      | Handling                              */
/* ---+----------------+-------------+---------------------------------------*/
/*  0 | State          | 0: BACKUP   | handled by application                */
/*    |                | 1: PRIMARY  |                                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  1 | Redundancy     | 0: REDUNDANT| Default: One primary AR of a given    */
/*    |                |             | system redundancy AR-set is present   */
/*    |                | 1           | None primary AR of a given...         */
/* ---+----------------+-------------+---------------------------------------*/
/*  2 | DataValid      | 0: Invalid  | discard frame if 0                    */
/*    |                | 1: Valid    |                                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  3 | reserved       | 0           | value dont care                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  4 | ProviderState  | 0: STOP     | handled by application                */
/*    |                | 1: RUN      |                                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  5 | StationFailure | 0: Failure  | handled by application                */
/*    |                | 1: OK       |                                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  6 | reserved       | 0           | discard frame if not 0                */
/* ---+----------------+-------------+---------------------------------------*/
/*  7 | Ignore         | 0           | discard DataStatus if not 0           */
/* ---+----------------+-------------+---------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define EDD_CSRT_DSTAT_BIT_STATE            0x01    /* DataStatus.State                    */
#define EDD_CSRT_DSTAT_BIT_REDUNDANCY       0x02    /* DataStatus.Redundancy               */
#define EDD_CSRT_DSTAT_BIT_DATA_VALID       0x04    /* DataStatus.DataValid                */
#define EDD_CSRT_DSTAT_BIT_3                0x08    /* DataStatus.Reserved_1               */
#define EDD_CSRT_DSTAT_BIT_STOP_RUN         0x10    /* DataStatus.ProviderState            */
#define EDD_CSRT_DSTAT_BIT_STATION_FAILURE  0x20    /* DataStatus.StationProblemIndicator  */
#define EDD_CSRT_DSTAT_BIT_6                0x40    /* DataStatus.Reserved_2               */
#define EDD_CSRT_DSTAT_BIT_IGNORE           0x80    /* DataStatus.Ignore                   */

/* STAION_FAILURE    0: FAILURE 1: OK      */
/* STOP_RUN          0: STOP    1: RUN     */

/* InitValue for global DataStatus Bits    */
#define EDD_CSRT_DSTAT_GLOBAL_INIT_VALUE  ( EDD_CSRT_DSTAT_BIT_STOP_RUN )

/* Bits which can be set global            */
#define EDD_CSRT_DSTAT_GLOBAL_MASK_ALL    ( EDD_CSRT_DSTAT_BIT_STOP_RUN )


#define EDD_CSRT_DSTAT_WIN_MASK           ( EDD_CSRT_DSTAT_BIT_STATE        |  \
                                            EDD_CSRT_DSTAT_BIT_REDUNDANCY       |  \
                                            EDD_CSRT_DSTAT_BIT_DATA_VALID   |  \
                                            EDD_CSRT_DSTAT_BIT_3            |  \
                                            EDD_CSRT_DSTAT_BIT_STATION_FAILURE  |  \
                                            EDD_CSRT_DSTAT_BIT_6            |  \
                                            EDD_CSRT_DSTAT_BIT_IGNORE              \
                                           )

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_ADD                */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_CSRT_PROV_IO_PARAMS_TYPE
{
    LSA_UINT32                          BufferProperties;             /* in  */
    LSA_UINT16                          CRNumber;                     /* in  */
    LSA_UINT8                           AppGroup;                     /* in  */
    LSA_UINT8                           SFPosition;                   /* in  */
    LSA_UINT32                          CWOffset;                     /* out */
    LSA_UINT32                          SFOffset;                     /* in  */
    LSA_UINT32                          DataStatusOffset;             /* out */
    LSA_UINT32                          DataOffset;                   /* in  */
} EDD_CSRT_PROV_IO_PARAMS_TYPE;


/* BufferProperties   :   Buffer properties:                                 */
/*                                                                           */
/*                        EDD_PROV_BUFFER_PROP_IRTE:                         */
/*                                                                           */
/*                         EDDI:                                             */
/*                          EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC: (default)   */
/*                          --> Mandatory for SRT-Provider/Consumer:         */
/*                          IRT-DataBuffer belongs to a asynchronous Image   */
/*                          --> DataBuffer may be read or written            */
/*                              *consistently* by using Consistency-Buffer   */
/*                              (Locking-Functions)                          */
/*                          --> DataBuffer should *not* be transfered via    */
/*                              DMA to/from HostRam.                         */
/*                              (Otherwise consistency will be lost !)       */
/*                                                                           */
/*                          EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC:              */
/*                          IRT-DataBuffer belongs to a synchronized Image   */
/*                          --> DataBuffer only may be read or written       */
/*                              *consistently* between StartOp-Interrupt     */
/*                              and InDone/OutDone-Signal                    */
/*                          --> DataBuffer is enabled to be transfered via   */
/*                              DMA to/from HostRam                          */
/*                                                                           */
/*                          EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID            */
/*                          --> no valid properties (causes an EDD PARAM err)*/
/*                                                                           */
/*                          EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED          */
/*                          --> the BufferProperties will be set with        */
/*                              EDD_SRV_SRT_PROVIDER_CONTROL later or no     */
/*                              properties to be set.                        */
/*                                                                           */
/*                         EDDP: not used                                    */
/*                         EDDS: not used                                    */

#define EDD_PROV_BUFFER_PROP_IRTE_MASK                          0xF
#define EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC                     0x0
#define EDD_PROV_BUFFER_PROP_IRTE_IMG_SYNC                      0x1
#define EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID                   0xE
#define EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED                 0xF

/* CRNumber           :   EDDP: IO-CR this provider belongs to.              */
/*                          ERTEC200P:                                       */
/*                              EDD_CRNUMBER_NOT_USED : CRNumber is not used.*/
/*                                                      Not allowed!         */
/*                              1..27                 : CRNumber             */
/*                              EDD_CRNUMBER_UNDEFINED: CRNumber will be set */
/*                                                      on PROVIDER_CONTROL  */
/*                        EDDI: not used                                     */
/*                        EDDS: not used                                     */

#define EDD_CRNUMBER_NOT_USED             0
#define EDD_CRNUMBER_UNDEFINED            0xFFFF

/* AppGroup           :   EDDP: Application-group this provider belongs to   */
/*                          0: no application group (default)                */
/*                          1: Group 1                                       */
/*                          2: Group 2                                       */
/*                          EDD_APPGROUP_UNDEFINED: AppGroup will be set     */
/*                                                  on PROVIDER_CONTROL      */
/*                        EDDI: not used                                     */
/*                        EDDS: not used                                     */
/*                                                                           */

#define EDD_APPGROUP_UNDEFINED              0xFF


/* SFPosition         :   Only if EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP is set:  */
/*                                                                           */
/*                        Subframeposition.                                  */
/*                           1..127                  : Position              */
/*                           EDD_SFPOSITION_UNDEFINED: Position will be set  */
/*                                                     on PROVIDER_CONTROL   */

#define EDD_SFPOSITION_UNDEFINED             0xFF
#define EDD_SFPOSITION_INVALID               0

/* CWOffset           :   EDDI: returned by EDDI only for buffer handling    */
/*                        EDDS: not used                                     */
/*                        EDDP: not used                                     */
/*                                                                           */
/* SFOffset           :   Only if EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP is set:  */
/*                                                                           */
/*                        Offset of the datagramm data (DG-C_SDU) in the     */
/*                        packframe relativ to the beginning of the packframe*/
/*                        C_SDU.                                             */
/*                                                                           */
/*                           0..xx                   : Offset                */
/*                           EDD_SFOFFSET_UNDEFINED  : Offset will be set    */
/*                                                     on PROVIDER_CONTROL   */

#define EDD_SFOFFSET_UNDEFINED              0xFFFFFFFFL
#define EDD_SFOFFSET_INVALID                0xFDFDFDFDL

/* DataOffset         :   EDDP,EDDI: Dataoffset of I/O-Buffer to start of    */
/*                                   I/O-Data RAM. Special meaning with EDDI */
/*                                   and packframes (DFP). see spec.         */
/*                                                                           */
/*                          If set to EDD_DATAOFFSET_UNDEFINED the DataOffset*/
/*                          will be set on EDD_SRV_SRT_PROVIDER_CONTROL or   */
/*                          is not used.                                     */
/*                                                                           */
/*                        EDDS: not used                                     */
/*                                                                           */
/* DataStatusOffset   :   EDDI / EDD_CSRT_PROV_PROP_FIXED_DS:                */
/*                        EDD_DATAOFFSET_INVALID                             */
/*                                                                           */
/*                        EDDI w. SOC:                                       */
/*                        Offset of the DataStatus of this provider relative */
/*                        to the beginning of KRAM.                          */
/*                                                                           */
/*                        EDDI w. ERTEC200 or ERTEC400:                      */
/*                        EDD_DATAOFFSET_UNDEFINED                           */
/*                                                                           */
/*                        EDDP:                                              */
/*                        Offset of the DataStatus of this provider relative */
/*                        to the beginning of PNIPRAM.                       */
/*                                                                           */
/*                        EDDS:                                              */
/*                        t.b.d.                                             */

#define EDD_DATAOFFSET_UNDEFINED            0xFFFFFFFFL
#define EDD_DATAOFFSET_INVALID              0xFDFDFDFDL



typedef struct _EDD_RQB_CSRT_PROVIDER_ADD_TYPE
{
    LSA_UINT16                           FrameID;
    LSA_UINT16                           Properties;
    LSA_UINT16                           DataLen;
    LSA_UINT16                           CycleReductionRatio;
    LSA_UINT16                           CyclePhase;
    LSA_UINT32                           CyclePhaseSequence;
    LSA_UINT32                           UserID;
    LSA_UINT16                           GroupID;
    LSA_UINT16                           ProviderID;                  /* out */
    EDD_CSRT_PROV_IO_PARAMS_TYPE         IOParams;
} EDD_RQB_CSRT_PROVIDER_ADD_TYPE;


typedef struct _EDD_RQB_CSRT_PROVIDER_ADD_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE;

/* FrameID            :   valid FrameID. Range depends on SRT-Init-parameters*/
/*                          or EDD_FRAME_ID_UNDEFINED                        */

#define EDD_FRAME_ID_UNDEFINED  0xFFFF

/* Properties         :   Properties of Provider                             */
/*                        RTCLASS (only one):                                */
/*                          EDD_CSRT_PROV_PROP_RTCLASS_1  : RTClass1         */
/*                          EDD_CSRT_PROV_PROP_RTCLASS_2  : RTClass2         */
/*                          EDD_CSRT_PROV_PROP_RTCLASS_3  : RTClass3         */
/*                          EDD_CSRT_PROV_PROP_RTCLASS_UDP: RTClassUDP       */
/*                                                                           */
/*                        EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP:                 */
/*                            This is a (Dynamic Frame Packing) provider with*/
/*                            a packed frame structure. Only valid with      */
/*                            EDD_CSRT_PROV_PROP_RTCLASS_3.                  */
/*                                                                           */
/*                                                                           */
/*                        EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY  [EDDI,EDDP]   */
/*                             Provider uses 2-CR startup                    */
/*                             The AUX-provider must be switched off with    */
/*                             PROVIDER_CONTROL-Service.                     */
/*                                                                           */
/*                        EDD_CSRT_PROV_PROP_MODE_MC_CR                  ]   */
/*                            This Provider is used with a "Multicast CR"    */
/*                            and special handling for the MAC-DA (DstMAC)   */
/*                            validation. If the flag is not set the         */
/*                            provider uses Unicast.                         */
/*                            This mode is not allowed together with         */
/*                            EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY or         */
/*                            EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP              */
/*                                                                           */
/*                        EDD_CSRT_PROV_PROP_SYSRED                          */
/*                            A system redundancy provider                   */
/*                                                                           */

#define EDD_CSRT_PROV_PROP_RTCLASS_MASK      0x000F
#define EDD_CSRT_PROV_PROP_RTCLASS_1         0x0001
#define EDD_CSRT_PROV_PROP_RTCLASS_2         0x0002
#define EDD_CSRT_PROV_PROP_RTCLASS_3         0x0003
#define EDD_CSRT_PROV_PROP_RTCLASS_UDP       0x0004

#define EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP    0x0010
#define EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY  0x0020
#define EDD_CSRT_PROV_PROP_MODE_MC_CR        0x0040
#define EDD_CSRT_PROV_PROP_SYSRED            0x0080

/* DataLen            :   DataLen of I/O-area of provider. Not Frame length! */
/*                        Valid range depends on properties!                 */
/*                        EDD_CSRT_PROV_PROP_RTCLASS_1:                      */
/*                        EDD_CSRT_PROV_PROP_RTCLASS_2:                      */
/*                        EDD_CSRT_PROV_PROP_RTCLASS_3 (no DFP)              */
/*                            Min: EDD_CSRT_DATALEN_MIN                      */
/*                            Max: EDD_CSRT_DATALEN_MAX                      */
/*                        EDD_CSRT_PROV_PROP_RTCLASS_UDP                     */
/*                            Min: EDD_CSRT_UDP_DATALEN_MIN                  */
/*                            Max: EDD_CSRT_UDP_DATALEN_MAX                  */
/*                        EDD_CSRT_PROV_PROP_RTCLASS_3 (DFP) datagrams       */
/*                            Min: EDD_CSRT_DFP_DATALEN_MIN                  */
/*                            Max: EDD_CSRT_DFP_DATALEN_MAX                  */

#define EDD_CSRT_DFP_DATALEN_MIN            1
#define EDD_CSRT_DFP_DATALEN_MAX            255

#define EDD_CSRT_DATALEN_MIN                40    /* Provider: Only if EDD   */
                                                  /* does not support        */
                                                  /* Autopadding of CSRT-Data*/
#define EDD_CSRT_DATALEN_MAX                1490

#define EDD_CSRT_UDP_DATALEN_MIN            12    /* Provider: Only if EDD   */
                                                  /* does not support        */
                                                  /* Autopadding of CSRT-Data*/
#define EDD_CSRT_UDP_DATALEN_MAX            1462

/* CycleReductionRatio:   Ratio to specify cycletime.                        */
/*                        Valid range depends on service!                    */
/*                        EDD_SRV_SRT_PROVIDER_ADD:                          */
/*                            Min  : EDD_CSRT_CYLCE_REDUCTION_RATIO_MIN      */
/*                            Max  : EDD_CSRT_CYLCE_REDUCTION_RATIO_MAX      */
/*                        only valid: 1,2,4,8,16,32,64,128,256,512           */
/*                        EDD_SRV_SRT_UDP_PROVIDER_ADD:                      */
/*                            Min  : EDD_CSRT_UDP_CYLCE_REDUCTION_RATIO_MIN  */
/*                            Max  : EDD_CSRT_UDP_CYLCE_REDUCTION_RATIO_MAX  */
/*                        only valid: 1,2,4,8,.,256,512,1024,2048,..,16384   */
/*                                                                           */
/*                        If set to EDD_CYCLE_REDUCTION_RATIO_UNDEFINED the  */
/*                        CycleReductionRatio must be set on                 */
/*                        EDD_SRV_SRT_PROVIDER_CONTROL.                      */

#define EDD_CYCLE_REDUCTION_RATIO_UNDEFINED       0

#define EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN        1           /* 2^0         */
#define EDD_CSRT_CYCLE_REDUCTION_RATIO_MAX        512         /* 2^9         */

#define EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MIN   1           /* 2^0         */
#define EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX   16          /* 2^4         */

#define EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN    1           /* 2^0         */
#define EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX    16384       /* 2^14        */

/* CyclePhase         :   Phase to put provider to.                          */
/*                        Min  : 1                                           */
/*                        Max  : CycleReductionRatio                         */
/*                                                                           */
/*                        If set to EDD_CYCLE_PHASE_UNDEFINED the CyclePhase */
/*                        must be set on EDD_SRV_SRT_PROVIDER_CONTROL or     */
/*                        EDD_SRV_SRT_PROVIDER_CHANGE_PHASE                  */
/*                                                                           */
/*                        With a none legacy RTCLASS_3 provider              */
/*                        (EDD_CSRT_PROV_PROP_RTCLASS_3 and not              */
/*                        EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) the           */
/*                        CyclePhase is determined by EDD internally from    */
/*                        PDIRData and CyclePhase is not evaluated!          */

#define EDD_CYCLE_PHASE_UNDEFINED                 0

/* CyclePhaseSequence :   Position within sendlist for this phase.           */
/*                        Currently not supported by edd.                    */
/*                        0   : put to last postion.                         */
/*                        else: put to this postion.                         */
/*                                                                           */
/*                        If set to EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED the   */
/*                        CyclePhaseSequence must be set on                  */
/*                        EDD_SRV_SRT_PROVIDER_CONTROL or                    */
/*                        EDD_SRV_SRT_PROVIDER_CHANGE_PHASE                  */
/*                                                                           */
/*                        With RTCLASS_3 provider this parameter is not used */
/*                                                                           */

#define EDD_CYCLE_PHASE_SEQUENCE_LAST                0
#define EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED           0xFFFFFFFF

/* VLANID             :   VLAN-ID put into sendframe. (0..4095)              */
/* UserID             :   UserID used for status-indications                 */
/* GroupID            :   GroupID used for setting global provider status    */
/* ProviderID         :   ProviderID returned by EDD                         */
/* IOParams           :   additional IO-Parameters.   see above              */

/* Providertypes for eddx_ProviderBufferRequest_Add:                         */
/* They CANNOT be ORed!                                                      */
/*                        EDD_PROVADD_TYPE_DEFAULT                           */
/*                            A normal provider(no DFP, no system redundancy)*/
/*                        EDD_PROVADD_TYPE_SYSRED                            */
/*                            A system redundancy provider.                  */
/*                                                                           */

#define EDD_PROVADD_TYPE_DEFAULT                    0x01
#define EDD_PROVADD_TYPE_SYSRED                     0x02

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_CONTROL            */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE
{
    LSA_UINT16                          ProviderID;
    LSA_UINT16                          Mode;
    LSA_UINT16                          FrameID;
    EDD_MAC_ADR_TYPE                    DstMAC;
    EDD_IP_ADR_TYPE                     DstIP;
    LSA_UINT16                          AutoStopConsumerID;
    LSA_UINT8                           Status;
    LSA_UINT8                           Mask;
    LSA_UINT16                          CycleReductionRatio;
    LSA_UINT16                          CyclePhase;
    LSA_UINT32                          CyclePhaseSequence;
    EDD_CSRT_PROV_IO_PARAMS_TYPE        IOParams;
    LSA_UINT8                           DebugInfo[EDD_DEBUG_INFO_SIZE];
} EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE;

typedef struct _EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE;

/* ProviderID         :   Provider to control                           (in) */

#define EDD_PROV_ID_INVALID        0xFFFE

/* Mode               :   Specifies the mode of provider control        (in) */
/*                                                                           */
/*                        Multiple bits may be set (restrictions see below)  */
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS:        */
/*                                                                           */
/*                         Only parameters still set to "UNDEFINED" are      */
/*                         evaluated.Every  UNDEFINED parameter gets the     */
/*                         value (may still be UNDEFINED).                   */
/*                         This mode shall not be set together with other    */
/*                         mode bits!                                        */
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_ACTIVATE:                    */
/*                        EDD_PROV_CONTROL_MODE_PASSIVATE:                   */
/*                                                                           */
/*                         Activate or passivate the provider. No effect if  */
/*                         the provider is already in the requested state.   */
/*                         If EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY was        */
/*                         selected at EDD_SRV_SRT_PROVIDER_ADD:             */
/*                         - MODE_PASSIVATE always refers to both providers  */
/*                         - RTC3-Provider can be activated only if the      */
/*                           AUX-Provider is already active,                 */
/*                         else ERR_SEQUENCE                                 */
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_ACTIVATE_AUX:                */
/*                        EDD_PROV_CONTROL_MODE_PASSIVATE_AUX:               */
/*                                                                           */
/*                         Activate or passivate the AUX-provider. Only      */
/*                         allowed if EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY    */
/*                         was selected at EDD_SRV_SRT_PROVIDER_ADD. Will    */
/*                         effect the RTC1 frame of the provider.            */
/*                         Only one Mode shall be set at once!               */
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_STATUS:                      */
/*                                                                           */
/*                         Evaluate parameters "Status" and "Mask". see below*/
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_AUTOSTOP:                    */
/*                                                                           */
/*                         Evalute parameter "AutoStopConsumerID". see below.*/
/*                         Note: Provider Autstop will not effect the AUX    */
/*                               frame if EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY*/
/*                                                                           */
/*                        EDD_PROV_CONTROL_MODE_REMOVE_ACW                   */
/*                         Remove ACW when being passivated (RTF support)    */
/*                                                                           */


#define EDD_PROV_CONTROL_MODE_ACTIVATE      0x01
#define EDD_PROV_CONTROL_MODE_PASSIVATE     0x02

#define EDD_PROV_CONTROL_MODE_ACTIVATE_AUX  0x04
#define EDD_PROV_CONTROL_MODE_PASSIVATE_AUX 0x08

#define EDD_PROV_CONTROL_MODE_STATUS        0x10
#define EDD_PROV_CONTROL_MODE_AUTOSTOP      0x20

#define EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS  0x40
#define EDD_PROV_CONTROL_MODE_REMOVE_ACW            0x8000


/* FrameID            :   valid FrameID. Range depends on SRT-Init-parameters*/
/* DstMAC             :   Destination MAC address to use                     */
/*                        Notes:                                             */
/*                        * For DstMAC validation see edd user doc.          */
/*                        * if the DstMAC is not present it shall be set to  */
/*                          00:00:00:00:00:00                                */
/* DstIP              :   Destination IP  address to use                     */
/*                        Only used with RTCLASS_UDP provider                */
/* AutoStopConsumerID :   ConsumerID of the consumer which will be used for  */
/*                        autostopping the provider if the consumers datahold*/
/*                        timer expires.                                     */
/*                        Only evaluated if EDD_PROV_CONTROL_MODE_AUTOSTOP   */
/*                        Values:                                            */
/*                        0..0xFFF0                 : Consumer ID            */
/*                        EDD_CONSUMER_ID_REMOVE_AS : Remove AutoStop        */
/*                                                                           */
/*                        Default on Provider ADD: no Autostop.              */
/*                                                                           */
/*                        Note: The Provider autostop must be activated      */
/*                              with EDD_SRV_SRT_CONSUMER_CONTROL! This can  */
/*                              be done before or after adding a provider!   */

#define EDD_CONSUMER_ID_REMOVE_AS           0xFFFF

/* Status/Mask        :   Bits of DataStatus to be modified             (in) */
/*                        Only evaluated if EDD_PROV_CONTROL_MODE_STATUS     */
/*                                                                           */
/*                        Status: New Bit-value                              */
/*                        Mask:   Which Bit to set                           */
/*                                                                           */
/*                        Values for Status and Mask. Can be ORed            */
/*                        Defines see above. here just for reference.        */
/*                                                                           */
/*                        Note: STOP_RUN and STATION_FAILURE can be set per  */
/*                              provider or global (see above). So if we     */
/*                              have different settings we have to decide..  */
/*                              For STOP_RUN       :  STOP wins.             */
/*                              For STATION_FAILURE:  FAILURE wins           */
/*                              See above for calculation of winner.         */

/* InitValue for local  DataStatus Bits: PRIMARY, DATA_VALID, RUN, OK  */
#define EDD_CSRT_DSTAT_LOCAL_INIT_VALUE    ( EDD_CSRT_DSTAT_BIT_STATE         | \
                                             EDD_CSRT_DSTAT_BIT_DATA_VALID    | \
                                             EDD_CSRT_DSTAT_BIT_STOP_RUN      | \
                                             EDD_CSRT_DSTAT_BIT_STATION_FAILURE \
                                           )

/* Mask of Bits which can be changed by PROVIDER_CONTROL service */
/* Note: DATA_VALID cannot be changed!                           */

#define EDD_CSRT_DSTAT_LOCAL_MASK_ALL     ( EDD_CSRT_DSTAT_BIT_STATION_FAILURE )

/* CycleReductionRatio:   Ratio to specify cycletime.                        */
/*                        see EDD_SRV_SRT_PROVIDER_ADD                       */
/*                                                                           */
/*                        Only evaluated if not already specified at         */
/*                        EDD_SRV_SRT_PROVIDER_ADD and the provider is       */
/*                        activated for the first time.                      */
/*                                                                           */
/* CyclePhase         :   Phase to put provider to.                          */
/*                        see EDD_SRV_SRT_PROVIDER_ADD                       */
/*                                                                           */
/*                        Only evaluated if not already specified at         */
/*                        EDD_SRV_SRT_PROVIDER_ADD and the provider is       */
/*                        activated for the first time.                      */
/*                                                                           */
/* CyclePhaseSequence :   Position within sendlist for this phase.           */
/*                        see EDD_SRV_SRT_PROVIDER_ADD                       */
/*                                                                           */
/*                        Only evaluated if not already specified at         */
/*                        EDD_SRV_SRT_PROVIDER_ADD and the provider is       */
/*                        activated for the first time.                      */
/*                                                                           */
/* IOParams           :   see EDD_SRV_SRT_PROVIDER_ADD service               */
/*                        some of the parameters can be set here             */
/*                                                                           */
/* DebugInfo          :   Detailed info if response is negative. See         */
/*                        edd_usr.doc for further explanations.              */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_REMOVE             */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE
{
    LSA_UINT16                           ProviderID;
} EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE;

typedef struct _EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE;

/* ProviderID         :   Provider to remove                            (in) */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_CHANGE_PHASE       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE
{
    LSA_UINT16                           ProviderID;                  /* in */
    LSA_UINT16                           CyclePhase;                  /* in */
    LSA_UINT32                           CyclePhaseSequence;          /* in */
} EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE;

typedef struct _EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE;

/* ProviderID         :   Provider to control                           (in) */
/* CyclePhase         :   Phase to put provider to.                          */
/*                        Min  : 1                                           */
/*                        Max  : CycleReductionRatio of Provider             */
/* CyclePhaseSequence :   Position within sendlist for this phase.           */
/*                        Currently not supported by edd.                    */
/*                        0   : put to last postion.                         */
/*                        else: put to this postion.                         */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_PROVIDER_XCHANGE_BUF        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_PROVIDER_XCHANGE_BUF_TYPE
{
    LSA_UINT16                           ProviderID;                    /* in */
    LSA_UINT32                           NewOffset;                     /* in */
    LSA_UINT32                           OldOffset;                     /* in */
    LSA_UINT32                           DataLen;                       /* in */
} EDD_RQB_CSRT_PROVIDER_XCHANGE_BUF_TYPE;

typedef struct _EDD_RQB_CSRT_PROVIDER_XCHANGE_BUF_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE;

/* ProviderID         :   related Provider                                   */
/* NewOffset          :   new Offset of the Buffer (as defined in            */
/*                        ProviderAdd)                                       */
/* OldOffset          :   old Offset of the Buffer (as defined in            */
/*                        ProviderAdd)                                       */
/* DataLen            :   Size of the Buffer       (must be the same as      */
/*                        defined in ProviderAdd)                            */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_CONSUMER_ADD                */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_CSRT_CONS_IO_PARAMS_TYPE
{
    LSA_UINT32                          BufferProperties;             /*  in */

    LSA_UINT16                          CRNumber;                     /*  in */
    LSA_UINT8                           AppGroup;                     /*  in */
    LSA_UINT8                           SFPosition;                   /*  in */
    LSA_UINT8                           ForwardMode;                  /*  in */
    LSA_UINT32                          CutThrough_OutDataStart;      /*  in */

    LSA_UINT32                          SFOffset;                     /*  in */
    LSA_UINT32                          DataOffset;                   /*  in */
    LSA_UINT32                          APDUStatusOffset;             /* out */

    LSA_BOOL                            ClearOnMISS;                  /*  in */

} EDD_CSRT_CONS_IO_PARAMS_TYPE;


/* BufferProperties   :   Buffer properties:                                 */
/*                                                                           */
/*                        EDD_CONS_BUFFER_PROP_IRTE:                         */
/*                                                                           */
/*                         EDDI:                                             */
/*                          EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC:             */
/*                          --> Mandatory for SRT-Provider/Consumer:         */
/*                          IRT-DataBuffer belongs to a asynchronous Image   */
/*                          --> DataBuffer may be read or written            */
/*                              *consistently* by using Consistency-Buffer   */
/*                              (Locking-Functions)                          */
/*                          --> DataBuffer should *not* be transfered via    */
/*                              DMA to/from HostRam.                         */
/*                              (Otherwise consistency will be lost !)       */
/*                                                                           */
/*                          EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC:              */
/*                          IRT-DataBuffer belongs to a synchronized Image   */
/*                          --> DataBuffer only may be read or written       */
/*                              *consistently* between StartOp-Interrupt     */
/*                              and InDone/OutDone-Signal                    */
/*                          --> DataBuffer is enabled to be transfered via   */
/*                              DMA to/from HostRam                          */
/*                                                                           */
/*                          EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID            */
/*                          --> no valid properties (causes an EDD PARAM err)*/
/*                                                                           */
/*                          EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED          */
/*                          --> the BufferProperties will be set with        */
/*                              EDD_SRV_SRT_CONSUMER_CONTROL later or no     */
/*                              properties to be set.                        */
/*                                                                           */
/*                         EDDP: not used                                    */
/*                         EDDS: not used                                    */

#define EDD_CONS_BUFFER_PROP_IRTE_MASK                          0xF
#define EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC                     0x0
#define EDD_CONS_BUFFER_PROP_IRTE_IMG_SYNC                      0x1
#define EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID                   0xE
#define EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED                 0xF

/* CRNumber           :   EDDP: IO-CR this provider belongs to.              */
/*                          ERTEC200P:                                       */
/*                              EDD_CRNUMBER_NOT_USED : CRNumber is not used.*/
/*                              1..27                 : CRNumber             */
/*                              EDD_CRNUMBER_UNDEFINED: CRNumber will be set */
/*                                                      on CONSUMER_CONTROL  */
/*                        EDDI: not used                                     */
/*                        EDDS: not used                                     */
/*                                                                           */
/* AppGroup           :   EDDP: Application-group this consumer belongs to   */
/*                          0: no application group (default)                */
/*                          1: Group 1                                       */
/*                          2: Group 2                                       */
/*                          3: Group 3                                       */
/*                          EDD_APPGROUP_UNDEFINED: AppGroup will be set     */
/*                                                  on CONSUMER_CONTROL      */
/*                        EDDI: not used                                     */
/*                        EDDS: not used                                     */
/*                                                                           */
/* SFPosition         :   Only if EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP is set:  */
/*                                                                           */
/*                        Subframeposition.                                  */
/*                           1..127                  : Position              */
/*                           EDD_SFPOSITION_UNDEFINED: Position will be set  */
/*                                                     on CONSUMER_CONTROL   */
/*                                                                           */
/* ForwardMode        :   Defines forwarding of I/O-Data to memory           */
/*                    :   EDDP: EDD_FORWARDMODE_SF : Normal reception        */
/*                                                   (store-and-forward mode)*/
/*                              EDD_FORWARDMODE_CT : This consumer shall be  */
/*                                                   received in CutThrough  */
/*                                                   Mode                    */
/*                              EDD_FORWARDMODE_UNDEFINED: Mode will be set  */
/*                                                        on CONSUMER_CONTROL*/
/*                                                                           */
/*                              Note: Only one Consumer can use CutThrough!  */
/*                        EDDI: Has no function for EDDI/EDDS, but due to    */
/*                              compatibility with EDDP FORWARDMODE_SF and   */
/*                              FORWARDMODE_UNDEFINED are allowed!           */
/*                        EDDS: not used                                     */
/*                                                                           */

#define EDD_FORWARDMODE_SF          0     /* StoreAndForward forwarding      */
#define EDD_FORWARDMODE_CT          1     /* CutThrough forwarding           */
#define EDD_FORWARDMODE_UNDEFINED   0xFF

/* CutThrough_OutDataStart:                                                  */
/*                    :   EDDP: only evaluated if ForwardMode set to         */
/*                              EDD_FORWARDMODE_CT (may also be set on       */
/*                              CONSUMER_CONTROL)                            */
/*                                                                           */
/*                              Defines the value written into register      */
/*                              CPM_OutDataStart of PN-IP before activating  */
/*                              the consumer. Range: 0..0x7FF                */
/*                                                                           */
/*                        EDDI: not used                                     */
/*                        EDDS: not used                                     */
/*                                                                           */
/* SFOffset           :   Only if EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP is set:  */
/*                                                                           */
/*                        Offset of the datagramm data (DG-C_SDU) in the     */
/*                        packframe relativ to the beginning of the packframe*/
/*                        C_SDU.                                             */
/*                                                                           */
/*                           0..xx                   : Offset                */
/*                           EDD_SFOFFSET_UNDEFINED  : Offset will be set    */
/*                                                     on CONSUMER_CONTROL   */
/*                                                                           */
/* DataOffset         :   EDDP,EDDI: Dataoffset of I/O-Buffer to start of    */
/*                                   I/O-Data RAM                            */
/*                                                                           */
/*                          If set to EDD_DATAOFFSET_UNDEFINED the DataOffset*/
/*                          will be set on EDD_SRV_SRT_PROVIDER_CONTROL or   */
/*                          is not used.                                     */
/*                                                                           */
/*                        EDDS: not used                                     */
/*                                                                           */
/* APDUStatusOffset   :   EDDI: Returns Offset of APDUStatus                 */
/*                        EDDP: Returns Offset of APDUStatus                 */
/*                        EDDS: not used                                     */
/*                                                                           */
/* ClearOnMISS        :   EDDS,EDDP:                                         */
/*                                                                           */
/*                        On Consumer MISS or on passivation of the consumer */
/*                        the next buffer will return cleared (all 0) data.  */
/*                        The support on this feature is I/O HW-dependend    */
/*                        For details see doc.                               */
/*                                                                           */
/*                        e.g. On ERTEC200P it is only supported if the      */
/*                             PERIF is used for I/O-Buffer handling.        */
/*                             (CRNumber is valid)                           */
/*                                                                           */
/*                        EDDI: not used                                     */



typedef struct _EDD_RQB_CSRT_CONSUMER_ADD_TYPE
{
    LSA_UINT16                           FrameID;
    LSA_UINT16                           Properties;
    LSA_UINT16                           DataLen;
    LSA_UINT16                           Partial_DataLen;             /*  in */
    LSA_UINT16                           Partial_DataOffset;          /*  in */
    LSA_UINT16                           CycleReductionRatio;
    LSA_UINT16                           CyclePhase;
    LSA_UINT16                           DataHoldFactor;
    LSA_UINT32                           UserID;
    LSA_UINT16                           ConsumerID;                  /* out */
    EDD_CSRT_CONS_IO_PARAMS_TYPE         IOParams;
} EDD_RQB_CSRT_CONSUMER_ADD_TYPE;

typedef struct _EDD_RQB_CSRT_CONSUMER_ADD_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE;

/* FrameID            :   valid FrameID. Range depends on SRT-Init-parameters*/
/* Properties         :   Properties of Consumer                             */
/*                        RTCLASS (only one):                                */
/*                          EDD_CSRT_CONS_PROP_RTCLASS_1  : RTClass1         */
/*                          EDD_CSRT_CONS_PROP_RTCLASS_2  : RTClass2         */
/*                          EDD_CSRT_CONS_PROP_RTCLASS_3  : RTClass3         */
/*                          EDD_CSRT_CONS_PROP_RTCLASS_UDP: RTClassUDP       */
/*                          EDD_CSRT_CONS_PROP_RTCLASS_AUX: EDDI special.    */
/*                                                                           */
/*                        EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP:                 */
/*                            This is a (Dynamic Frame Packing) provider with*/
/*                            a packed frame structure.                      */
/*                                                                           */
/*                        EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY [EDDI,EDDP]    */
/*                             Consumer uses 2-CR startup.                   */
/*                                                                           */
/*                        EDD_CSRT_CONS_PROP_SYSRED                          */
/*                             This consumer is a system redundancy consumer.*/

#define EDD_CSRT_CONS_PROP_RTCLASS_MASK      0x000F
#define EDD_CSRT_CONS_PROP_RTCLASS_1         0x0001
#define EDD_CSRT_CONS_PROP_RTCLASS_2         0x0002
#define EDD_CSRT_CONS_PROP_RTCLASS_3         0x0003
#define EDD_CSRT_CONS_PROP_RTCLASS_UDP       0x0004
#define EDD_CSRT_CONS_PROP_RTCLASS_AUX       0x0005

#define EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP    0x0010
#define EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY  0x0020
#define EDD_CSRT_CONS_PROP_SYSRED            0x0080

/* DataLen            :   DataLen of I/O-area of consumer. Not Frame length! */
/*                        Valid range depends on properties!                 */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_1:                      */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_2:                      */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_3 (no DFP)              */
/*                            Min: EDD_CSRT_DATALEN_MIN                      */
/*                            Max: EDD_CSRT_DATALEN_MAX                      */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_UDP                     */
/*                            Min: EDD_CSRT_UDP_DATALEN_MIN                  */
/*                            Max: EDD_CSRT_UDP_DATALEN_MAX                  */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_3 (DFP)                 */
/*                            Min: EDD_CSRT_DFP_DATALEN_MIN                  */
/*                            Max: EDD_CSRT_DFP_DATALEN_MAX                  */
/*                                                                           */
/* Partial_DataOffset :   EDDP,EDDI: PartialData access. dataoffset.         */
/*                          ( see "HardwareType" of EDD_SRV_GET_PARAMS if    */
/*                            partial access supported! )                    */
/*                          0..(DataLen - 1)                                 */
/*                                                                           */
/*                        EDDS: not used                                     */
/*                                                                           */
/* Partial_DataLen    :   EDDP, EDDI: PartialData access. Datalen.           */
/*                          ( see "HardwareType" of EDD_SRV_GET_PARAMS if    */
/*                            partial access supported! )                    */
/*                          0          : no partial access                   */
/*                          1..(DataLen - Partial_DataOffset)                */
/*                                                                           */
/*                        EDDS: not used                                     */
/*                                                                           */
/* CycleReductionRatio:   Ratio to spezify cycletime.                        */
/*                        Valid range depends on service!                    */
/*                        EDD_SRV_SRT_CONSUMER_ADD:                          */
/*                            Min  : EDD_CSRT_CYCLE_REDUCTION_RATIO_MIN      */
/*                            Max  : EDD_CSRT_CYCLE_REDUCTION_RATIO_MAX      */
/*                        only valid: 1,2,4,8,16,32,64,128,256,512           */
/*                        EDD_SRV_SRT_UDP_CONSUMER_ADD:                      */
/*                            Min  : EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MIN  */
/*                            Max  : EDD_CSRT_UDP_CYCLE_REDUCTION_RATIO_MAX  */
/*                        only valid: 1,2,4,8,.,256,512,1024,2048,..,16384   */
/*                                                                           */
/*                        If set to EDD_CYCLE_REDUCTION_RATIO_UNDEFINED the  */
/*                        CycleReductionRatio must be set on                 */
/*                        EDD_SRV_SRT_CONSUMER_CONTROL.                      */
/*                                                                           */
/* CyclePhase         :   Phase to check consumer                            */
/*                        Min  : 1                                           */
/*                        Max  : CycleReductionRatio                         */
/*                                                                           */
/*                        If set to EDD_CYCLE_PHASE_UNDEFINED the CylcePhase */
/*                        must be set on EDD_SRV_SRT_CONSUMER_CONTROL.       */
/*                                                                           */
/* DataHoldFactor     :   Factor to check for DataHold. Timebase is          */
/*                        "CycleBaseFactor * CycleReductionRatio * 31,25us"  */
/*                        Range: 3..255                                      */

#define EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MIN     3
#define EDD_SRT_CONSUMER_DATAHOLD_FACTOR_MAX   255

/* UserID             :   UserID used for status-indications                 */
/* ConsumerID         :   ConsumerID returned by EDD                         */
/* IOParams           :   additional IO-Parameters (see above)               */
/* Port               :   additional Port-Parameters. Only evaluated with    */
/*                        EDD_CSRT_CONS_PROP_RTCLASS_3 Consumer!             */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_CONSUMER_CONTROL            */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE
{
    LSA_UINT16                           ConsumerID;
    LSA_UINT16                           Mode;
    EDD_MAC_ADR_TYPE                     SrcMAC;
    EDD_IP_ADR_TYPE                      SrcIP;
    LSA_UINT16                           CycleReductionRatio;
    LSA_UINT16                           CyclePhase;
    EDD_CSRT_CONS_IO_PARAMS_TYPE         IOParams;
    LSA_UINT8                            DebugInfo[EDD_DEBUG_INFO_SIZE];

} EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE;

typedef struct _EDD_RQB_CSRT_CONSUMER_CONTROL_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE;

/* ConsumerID         :   Consumer to control                           (in) */

#define EDD_CONS_ID_INVALID        0xFFFE

/* Mode               :   Specifies the mode of consumer control        (in) */
/*                                                                           */
/*                        Multiple bits may be set (restrictions see below)  */
/*                                                                           */
/*                        EDD_CONS_CONTROL_MODE_ACTIVATE:                    */
/*                        EDD_CONS_CONTROL_MODE_PASSIVATE:                   */
/*                                                                           */
/*                         Activate or passivate the Consumer. No effect if  */
/*                         the consumer is already in the requested state.   */
/*                         Only one Mode shall be set at once!               */
/*                                                                           */
/*                        EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN:              */
/*                                                                           */
/*                         Set the actual consumer state to UNKNOWN for the  */
/*                         MISS/AGAIN-indication. This will cause an AGAIN   */
/*                         or MISS depending on a frame received or not      */
/*                         after setting the state to UNKNOWN.               */
/*                                                                           */
/*                         Not evaluated if Consumer is/set to PASSIVE       */
/*                                                                           */
/*                         Note: Does only effect the MISS/AGAIN indication! */
/*                         Note: The default-state after activating a passive*/
/*                               consumer with SetToUnknownState = FALSE is  */
/*                               the MISS-state.                             */
/*                                                                           */
/*                        EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE         */
/*                        EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE        */
/*                                                                           */
/*                         Enables or disables the autostop feature of       */
/*                         all providers associated with this consumer       */
/*                         (see EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE           */
/*                         parameter "AutoStopConsumerID").                  */
/*                                                                           */
/*                         Providers added later will use the actual state!  */
/*                         The setting can be changed while a provider is    */
/*                         active and inactive.                              */
/*                                                                           */
/*                         Only one Mode shall be set at once!               */
/*                                                                           */
/*                        EDD_CONS_CONTROL_MODE_REMOVE_ACW                   */
/*                         Remove ACW when being passivated (RTF support)    */
/*                                                                           */

#define EDD_CONS_CONTROL_MODE_ACTIVATE                 0x01
#define EDD_CONS_CONTROL_MODE_PASSIVATE                0x02

#define EDD_CONS_CONTROL_MODE_SET_TO_UNKNOWN           0x04

#define EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE     0x20
#define EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_DISABLE    0x40
#define EDD_CONS_CONTROL_MODE_REMOVE_ACW               0x8000


/* SrcMAC             :   Source  MAC address to check for.                  */
/*                        see EDD_SRV_SRT_CONSUMER_ADD                       */
/*                        Only valid if activating a formaly passive Consumer*/
/*                        Only valid with non RTCLASS_UDP consumer           */
/*                        Note: Not supported by EDDS                        */
/* SrcIP              :   Source  IP address. IP address of received frame   */
/*                        to be checked.                                     */
/*                        see EDD_SRV_SRT_CONSUMER_ADD                       */
/*                        Only valid if activating a formaly passive Consumer*/
/*                        Only valid with RTCLASS_UDP consumer               */
/*                        Note: Not suppored by EDDS                         */
/* CycleReductionRatio:   Ratio to specify cycletime.                        */
/*                        see EDD_SRV_SRT_CONSUMER_ADD                       */
/*                                                                           */
/*                        Only evaluated if not already specified at         */
/*                        EDD_SRV_SRT_CONSUMER_ADD and the provider is       */
/*                        activated for the first time.                      */
/*                                                                           */
/* CyclePhase         :   Phase to put provider to.                          */
/*                        see EDD_SRV_SRT_CONSUMER_ADD                       */
/*                                                                           */
/*                        Only evaluated if not already specified at         */
/*                        EDD_SRV_SRT_CONSUMER_ADD and the provider is       */
/*                        activated for the first time.                      */
/*                                                                           */
/* IOParams           :   see EDD_SRV_SRT_CONSUMER_ADD service               */
/*                        some of the parameters can be set here             */
/*                                                                           */
/* DebugInfo          :   Detailed info if response is negative. See         */
/*                        edd_usr.doc for further explanations.              */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_CONSUMER_REMOVE             */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE
{
    LSA_UINT16                           ConsumerID;
} EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE;

typedef struct _EDD_RQB_CSRT_CONSUMER_REMOVE_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE;

/* ConsumerID         :   Consumer to remove                            (in) */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_CONSUMER_XCHANGE_BUF        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_CSRT_CONSUMER_XCHANGE_BUF_TYPE
{
    LSA_UINT16                            ConsumerID;                    /* in */
    LSA_UINT32                            NewOffset;                     /* in */
    LSA_UINT32                            OldOffset;                     /* in */
    LSA_UINT32                            DataLen;                       /* in */
    LSA_BOOL                              CopyUserData;                  /* in */
    LSA_UINT32                            APDUStatusOffset;              /* out */
} EDD_RQB_CSRT_CONSUMER_XCHANGE_BUF_TYPE;

typedef struct _EDD_RQB_CSRT_CONSUMER_XCHANGE_BUF_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_CONSUMER_XCHANGE_BUF_PTR_TYPE;

/* ProviderID         :   related Provider                                   */
/* NewOffset          :   new Offset of the Buffer (as defined in            */
/*                        ProviderAdd)                                       */
/* OldOffset          :   old Offset of the Buffer (as defined in            */
/*                        ProviderAdd)                                       */
/* DataLen            :   Size of the Buffer       (must be the same as      */
/*                        defined in ProviderAdd)                            */
/* CopyUserData       :   Copy user Data to new buffer?                      */
/* APDUStatusOffset   :   dont used. (compatibility with EDDI)               */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SRT_GET_APDU_STATUS             */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_CSRT_APDU_STATUS
{
    LSA_UINT16          CycleCnt;       /* CycleCounter. 31.25us based       */
    LSA_UINT8           DataStatus;     /* DataStatus                        */
    LSA_UINT8           TransferStatus; /* TransferStatus                    */
} EDD_CSRT_APDU_STATUS_TYPE;

typedef struct _EDD_CSRT_APDU_STATUS  EDD_UPPER_MEM_ATTR * EDD_CSRT_APDU_STATUS_PTR_TYPE;

typedef struct _EDD_RQB_CSRT_GET_APDU_STATUS
{
    LSA_UINT8                   Type;       /* Consumer or Provider          */
    LSA_UINT16                  ID;         /* Consumer or Provider - ID     */
    LSA_BOOL                    Present;    /* APDUStatus present?           */
    EDD_CSRT_APDU_STATUS_TYPE   APDUStatus; /* actual APDU-Status if Present */
} EDD_RQB_CSRT_GET_APDU_STATUS_TYPE;

typedef struct _EDD_RQB_CSRT_GET_APDU_STATUS  EDD_UPPER_MEM_ATTR * EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE;

/* Type               :   Type of ID. Provider or Consumer              (in) */

#define EDD_CSRT_TYPE_PROVIDER  0
#define EDD_CSRT_TYPE_CONSUMER  1

/* ID                 :   Provider or Consumer ID                       (in) */
/* Present            :   LSA_TRUE:  APDU-Status present. APDUStatus    (out)*/
/*                    :              contains valid values                   */
/*                    :   LSA_FALSE: ADPU-Status not present.                */
/*                    :              (maybe no consumer received yet)        */
/* APDUStatus         :   APDU-Status for this Provider or Consumer     (out)*/

/*===========================================================================*/
/*===========================================================================*/

#define EDD_STATISTICS_RESET_ALL (LSA_UINT16)0xFFFF

/*====== EDD_GET_STATISTICS_IRTE_RAW_TYPE ======*/
typedef struct _EDD_GET_STATISTICS_IRTE_RAW_TYPE
{
    LSA_UINT32      RxGoodByte;   
    LSA_UINT32      RxTotalByte;    
    LSA_UINT32      TxTotalByte;    
    LSA_UINT32      TxFrame;        
    LSA_UINT32      RxUniFrame;          
    LSA_UINT32      RxUnUniFrame;       
    LSA_UINT32      RxMulFrame;          
    LSA_UINT32      RxUnMulFrame;        
    LSA_UINT32      RxBroadMulFrame;        
    LSA_UINT32      TxUniFrame;          
    LSA_UINT32      TxMulFrame;          
    LSA_UINT32      TxBroadFrame;       
    LSA_UINT32      Size_0_63_Frame;      
    LSA_UINT32      Size_64_Frame;        
    LSA_UINT32      Size_65_127_Frame;    
    LSA_UINT32      Size_128_255_Frame;   
    LSA_UINT32      Size_256_511_Frame;   
    LSA_UINT32      Size_512_1023_Frame;  
    LSA_UINT32      Size_1024_1536_Frame;  
    LSA_UINT32      RxGoodFrame;    
    LSA_UINT32      RxTotalFrame;   
    LSA_UINT32      RxSize_0_63_ErrCRC_Frame;
    LSA_UINT32      RxOverflowFrame;     
    LSA_UINT32      RxOversizeFrame;     
    LSA_UINT32      RxErrCrcAlignFrame;    
    LSA_UINT32      RxFcwDbDropFrame;      
    LSA_UINT32      TxCollisionFrame;    
    LSA_UINT32      HolBndCros;     

    //NRT Fragmentation
    LSA_UINT32      SentFragmentedFrames;    
    LSA_UINT32      SentNonFragmentedFrames;    
    LSA_UINT32      ReceivedFragmentedFrames;        
    LSA_UINT32      ReceivedNonFragmentedFrames;          
    LSA_UINT32      DiscardedTXFrames;       
    LSA_UINT32      DiscardedRXFrames;          
    LSA_UINT32      DiscardedRXFragments;   
} EDD_GET_STATISTICS_IRTE_RAW_TYPE;

typedef struct _EDD_GET_STATISTICS_IRTE_RAW_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_GET_STATISTICS_IRTE_RAW_PTR_TYPE;


/*====== EDD_GET_STATISTICS_RAW_TYPE ======*/
typedef union _EDD_GET_STATISTICS_RAW_TYPE
{
    EDD_GET_STATISTICS_IRTE_RAW_TYPE irte;
} EDD_GET_STATISTICS_RAW_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_GET_STATISTICS                  */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_GET_STATISTICS_MIB_TYPE
{
    LSA_UINT32      InOctets;
    LSA_UINT32      InUcastPkts;
    LSA_UINT32      InNUcastPkts;
    LSA_UINT32      InDiscards;
    LSA_UINT32      InErrors;
    LSA_UINT32      InUnknownProtos;
    LSA_UINT32      OutOctets;
    LSA_UINT32      OutUcastPkts;
    LSA_UINT32      OutNUcastPkts;
    LSA_UINT32      OutDiscards;
    LSA_UINT32      OutErrors;
    LSA_UINT32      OutQLen;
} EDD_GET_STATISTICS_MIB_TYPE;

typedef struct _EDD_GET_STATISTICS_MIB_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_GET_STATISTICS_MIB_PTR_TYPE;

typedef struct _EDD_RQB_GET_STATISTICS_TYPE
{
    LSA_UINT16      PortID;
    LSA_BOOL        RawUsed;
    LSA_BOOL        Reset;
    EDD_GET_STATISTICS_RAW_TYPE sRaw;
    EDD_GET_STATISTICS_MIB_TYPE sMIB;
} EDD_RQB_GET_STATISTICS_TYPE;

typedef struct _EDD_RQB_GET_STATISTICS_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_GET_STATISTICS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_RESET_STATISTICS                */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_RESET_STATISTICS_TYPE
{
  LSA_UINT16     PortID;

} EDD_RQB_RESET_STATISTICS_TYPE;

typedef struct _EDD_RQB_RESET_STATISTICS_TYPE   EDD_UPPER_MEM_ATTR * EDD_UPPER_RESET_STATISTICS_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_GET_STATISTICS_ALL              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_GET_STATISTICS_ALL_TYPE
{
    LSA_BOOL                    Reset;
    EDD_GET_STATISTICS_MIB_TYPE sMIB[EDD_CFG_MAX_PORT_CNT+1];
} EDD_RQB_GET_STATISTICS_ALL_TYPE;

typedef struct _EDD_RQB_GET_STATISTICS_ALL_TYPE  EDD_UPPER_MEM_ATTR * EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE;

/*===========================================================================*/
/*======================= SYNC  INTERFACE  ==================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_RECV                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

/* see EDD_SRV_NRT_RECV */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_SEND                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

/* see EDD_SRV_NRT_SEND */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_CANCEL                     */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

/* see EDD_SRV_NRT_CANCEL */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SYNC_SET_RATE                   */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SYNC_SET_RATE_TYPE
{
    LSA_UINT32    SyncID;
    LSA_BOOL      RateValid;
    LSA_INT32     RateIntervalNs;
} EDD_RQB_SYNC_SET_RATE_TYPE;

typedef struct _EDD_RQB_SYNC_SET_RATE_TYPE     EDD_UPPER_MEM_ATTR *    EDD_UPPER_SYNC_SET_RATE_PTR_TYPE;

/* SyncID             :   SyncID to set rate for (0..31)                (in) */
/* RateValid          :   is rate valid?                                (in) */
/* RateIntervalNs     :   Rateinterval if rate is valid                 (in) */
/*                        0  : no rate                                       */
/*                        >0 : the local timestamp is incremented by 1 per   */
/*                             RateIntervalNs                                */
/*                        <0 : the local timestamp is decremented by 1 per   */
/*                             RateIntervalNs                                */

/*===========================================================================*/
/*========================= SWITCH-INTERFACE ================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SWITCH_GET_PARAMS               */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/
/* Note: This service is not used by PNIO Stack and only supported by EDDI!  */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SWI_GET_PARAMS_TYPE
{
  LSA_UINT32  VLANSizeTable;
  LSA_UINT32  MACSizeTable;
  LSA_UINT16  LowWaterPollTime;
  LSA_UINT16  LowWaterPollTimeBase;

} EDD_RQB_SWI_GET_PARAMS_TYPE;

typedef struct _EDD_RQB_SWI_GET_PARAMS_TYPE    EDD_UPPER_MEM_ATTR *    EDD_UPPER_SWI_GET_PARAMS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SWITCH_SET_PORT_STATE           */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SWI_SET_PORT_STATE_TYPE
{
    LSA_UINT16  PortIDState [EDD_CFG_MAX_PORT_CNT];
} EDD_RQB_SWI_SET_PORT_STATE_TYPE;

typedef struct _EDD_RQB_SWI_SET_PORT_STATE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SWITCH_GET_PORT_STATE           */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/
/* Note: This service is not used by PNIO Stack and only supported by EDDI!  */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SWI_GET_PORT_STATE_TYPE
{
  LSA_UINT16  PortIDState [EDD_CFG_MAX_PORT_CNT];
} EDD_RQB_SWI_GET_PORT_STATE_TYPE;

typedef struct _EDD_RQB_SWI_GET_PORT_STATE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SWI_GET_PORT_STATE_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SWITCH_FLUSH_FILTERING_DB       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_SWI_FLUSH_FILTERING_DB_TYPE
{
    LSA_UINT16  PortCnt;
    LSA_UINT16  PortID[EDD_CFG_MAX_PORT_CNT];  
} EDD_RQB_SWI_FLUSH_FILTERING_DB_TYPE;

typedef struct _EDD_RQB_SWI_FLUSH_FILTERING_DB_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE;

/* PortCnt            :   Number of port entries within PortID[] to be  (in) */
/*                        flushed.                                           */
/*                        Range: 1..EDD_CFG_MAX_PORT_CNT                     */
/* PortID[]           :   Array with PortIDs to be flushed. Only the    (in) */
/*                        array entries 0..PortCnt are used/evaluated        */
/*                        Value of each entry: 1.. MaxPortCnt                */  

/* Note: If a EDD/hardware does not support clearing the FDB table for       */
/*       spezific PortIDs, the edd/hardware shall clear the whole FDB table  */
/*       instead. For EDD support see edd_usr.doc                            */

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= PRM-INTERFACE ===================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_READ                        */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PRM_READ_TYPE
{
    LSA_UINT16                  slot_number;
    LSA_UINT16                  subslot_number;
    LSA_UINT16                  edd_port_id;
    LSA_UINT32                  record_index;
    LSA_UINT32                  record_data_length;
    EDD_UPPER_MEM_U8_PTR_TYPE   record_data;
} EDD_RQB_PRM_READ_TYPE;

typedef struct _EDD_RQB_PRM_READ_TYPE   EDD_UPPER_MEM_ATTR *   EDD_UPPER_PRM_READ_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_WRITE                       */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PRM_WRITE_TYPE
{
    LSA_BOOL                    Local;
    LSA_UINT16                  slot_number;
    LSA_UINT16                  subslot_number;
    LSA_UINT16                  edd_port_id;
    LSA_UINT32                  record_index;
    LSA_UINT32                  record_data_length;
    EDD_UPPER_MEM_U8_PTR_TYPE   record_data;
} EDD_RQB_PRM_WRITE_TYPE;

typedef struct _EDD_RQB_PRM_WRITE_TYPE   EDD_UPPER_MEM_ATTR *   EDD_UPPER_PRM_WRITE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*  Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_PREPARE                    */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PRM_PREPARE_TYPE
{
    LSA_UINT32  dummy;
} EDD_RQB_PRM_PREPARE_TYPE;

typedef struct _EDD_RQB_PRM_PREPARE_TYPE   EDD_UPPER_MEM_ATTR *    EDD_UPPER_PRM_PREPARE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_END                         */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PRM_END_TYPE
{
    LSA_BOOL    Local;
} EDD_RQB_PRM_END_TYPE;

typedef struct _EDD_RQB_PRM_END_TYPE   EDD_UPPER_MEM_ATTR *    EDD_UPPER_PRM_END_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*  Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_COMMIT                     */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PRM_COMMIT_TYPE
{
    EDD_RESERVED_INTERVAL_IF_RED_TYPE RsvIntervalRed; /* see EDD_RESERVED_INTERVAL_IF_RED_TYPE */
} EDD_RQB_PRM_COMMIT_TYPE;

typedef struct _EDD_RQB_PRM_COMMIT_TYPE   EDD_UPPER_MEM_ATTR *    EDD_UPPER_PRM_COMMIT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PRM_INDICATION                  */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [x] EDDS [x]                                        */
/*---------------------------------------------------------------------------*/

#define EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES     28
/* 2    : PDNC FrameDrop on interface                                               */
/* 8    : only EDDP -> LATE-Error diagnosis for InBound pack groups on interface    */
/* 18   : only EDDP -> MRT diagnosis for MRPD consumers on interface                */

typedef struct _EDD_SRV_PRM_IND_DATA_TYPE
{
  LSA_UINT16   ChannelNumber;       /* 0x8000 */
  LSA_UINT16   ChannelProperties;   /* ChannelProperties.Type_Bit0_7 := 0,                                       */
                                    /* ChannelProperties.Accumulative_Bit8 := 0,                                 */
                                    /* Channelproperties.MaintenanceRequired_Bit9  := Component specific,        */
                                    /* Channelproperties.MaintenanceDemanded_Bit10 := Component specific,        */
                                    /* ChannelProperties.Specifier_Bit11_12        := Appearing(1) oder Disappearing(2) */
                                    /* ChannelProperties.Direction_Bit13_15        := 0                                 */
  LSA_UINT16   ChannelErrorType;    /* see below */
  LSA_UINT16   ExtChannelErrorType; /* see below */
  LSA_UINT32   ExtChannelAddValue;  /* depends on error  */

} EDD_SRV_PRM_IND_DATA_TYPE;

typedef struct _EDD_SRV_PRM_IND_DATA_TYPE EDD_UPPER_MEM_ATTR * EDD_SRV_PRM_IND_DATA_PTR_TYPE;

typedef struct _EDD_RQB_PRM_INDICATION_TYPE
{
  LSA_UINT16                  edd_port_id;           /* OUT : edd_port_id (0=interface)            */
  LSA_UINT16                  diag_cnt;              /* OUT : 0..EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES */
  EDD_SRV_PRM_IND_DATA_TYPE   diag[EDD_SRV_PRM_INDICATION_DIAG_MAX_ENTRIES];

} EDD_RQB_PRM_INDICATION_TYPE;

typedef struct _EDD_RQB_PRM_INDICATION_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_PRM_INDICATION_PTR_TYPE;


/*** ChannelNumber ***/
#define EDD_SRV_PRM_CHANNEL_NUMBER          0x8000

/*** ChannelProperties ***/
#define EDD_SRV_PRM_CH_PROP_NONE            0x0000

/* Bit  9: MaintenanceRequired */
#define EDD_SRV_PRM_CH_PROP_MAIN_REQUIRED   0x0200

/* Bit 10: MaintenanceDemanded */
#define EDD_SRV_PRM_CH_PROP_MAIN_DEMANDED   0x0400

/* Bit 11 - 12: ChannelProperties.Specifier */
#define EDD_SRV_PRM_CH_PROP_APPEARS         0x0800
#define EDD_SRV_PRM_CH_PROP_DISAPPEARS      0x1000

/*** ChannelErrorType ***/
#define EDD_SRV_PRM_CHANNEL_ERROR_TYPE_NC_MISMATCH    0x8008  /* Network component function mismatch                */
#define EDD_SRV_PRM_CHANNEL_ERROR_TYPE_DFP_MISMATCH   0x800A  /* Dynamic frame packing function mismatch            */
#define EDD_SRV_PRM_CHANNEL_ERROR_TYPE_MRPD_MISMATCH  0x800B  /* Media redundancy with planned duplication mismatch */

/* ExtChannelError for EDD_SRV_PRM_CHANNEL_ERROR_TYPE_NC_MISMATCH      */
#define EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_NC_FD_NR   0x8000  /* Frame dropped - no resource */

/* ExtChannelError for    EDD_SRV_PRM_CHANNEL_ERROR_TYPE_DFP_MISMATCH  */
#define EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_DFP_FLE    0x8000  /* DFP Frame late error. FrameID added to value!     */

/* ExtChannelError for    EDD_SRV_PRM_CHANNEL_ERROR_TYPE_MRPD_MISMATCH */
#define EDD_SRV_PRM_EXT_CHANNEL_ERROR_TYPE_MRPD_DV    0x8000  /* MRPD duplication void. FrameID added to value!    */


/*** ExtChannelAddValue  ***/
#define EDD_SRV_PRM_EXT_CHANNEL_ADD_NONE              0x0000  /* No info */


/*===========================================================================*/
/*======================= PTCP - INTERFACE ==================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PTCP_DELAY_CONTROL              */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [-] EDDP [x] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PTCP_DELAY_CONTROL_TYPE
{
    LSA_UINT8           ReqActive;
    LSA_UINT8           RspActive;
    EDD_MAC_ADR_TYPE    PortMacAddr;
    LSA_UINT16          PortID;
    LSA_UINT32          RxPortDelay;
    LSA_UINT32          TxPortDelay;
} EDD_RQB_PTCP_DELAY_CONTROL_TYPE;

typedef struct _EDD_RQB_PTCP_DELAY_CONTROL_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_PTCP_DELAY_CONTROL_PTR_TYPE;

/* ReqActive          : Set delay requestor inactive if 0 else active   (in) */
/* RspActive          : Set delay responder inactive if 0 else active   (in) */
#define	EDD_PTCP_DELAY_INACTIVE                0
#define	EDD_PTCP_DELAY_ACTIVE                  1
/* PortMacAddr        : PortMAC address of the requestor                (in) */
/* PortID             : Portnumber (1 or 2)                             (in) */
/* RxPortDelay        : Receive port delay local in nanoseconds         (in) */
/* TxPortDelay        : Send port delay local in nanoseconds            (in) */


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PTCP_SYNC_CONTROL               */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [-] EDDP [x] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

#define	EDD_PTCP_TLV_MAX_LEN                   52

typedef struct _EDD_RQB_PTCP_SYNC_CONTROL_TYPE
{
    LSA_UINT8       SyncId;
    LSA_UINT8       SlaveMode;
    LSA_UINT8       MasterMode;
    LSA_UINT8       LengthTLV;
    LSA_UINT32      SyncSendFactor;
    LSA_UINT16      TimeoutFactor;
    LSA_UINT16      TakeoverFactor;
    LSA_UINT32      PLLWindow;
    LSA_UINT8       TLV [EDD_PTCP_TLV_MAX_LEN];
} EDD_RQB_PTCP_SYNC_CONTROL_TYPE;

typedef struct _EDD_RQB_PTCP_SYNC_CONTROL_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_PTCP_SYNC_CONTROL_PTR_TYPE;

/* SyncId             : Synchronization ID: 0 = Clock, 1 = Time         (in) */
#define	EDD_PTCP_SYNC_ID_CLOCK                 0
#define	EDD_PTCP_SYNC_ID_TIME                  1

/* SlaveMode          : Mode of slave operation:                        (in) */
#define	EDD_PTCP_SYNC_SLAVE_MODE_STOP          0
#define	EDD_PTCP_SYNC_SLAVE_MODE_START         1
#define	EDD_PTCP_SYNC_SLAVE_MODE_UPDATE        2

/* MasterMode         : Mode of master operation:                       (in) */
#define	EDD_PTCP_SYNC_MASTER_MODE_OFF          0
#define	EDD_PTCP_SYNC_MASTER_MODE_START        1
#define	EDD_PTCP_SYNC_MASTER_MODE_UPDATE       2
#define	EDD_PTCP_SYNC_MASTER_MODE_ACTIVE       3
#define	EDD_PTCP_SYNC_MASTER_MODE_SLEEP        4

/* LengthTLV          : Length of SyncFrame TLV <= EDD_PTCP_TLV_MAX_LEN (in) */
/* SyncSendFactor     : Master send interval in milliseconds            (in) */
/* TimeoutFactor      : Slave timeout as multiple of send interval      (in) */
/* TakeoverFactor     : Takeover timeout as multiple of send interval   (in) */
/* PLLWindow          : Max. allowed deviation from master time in ns   (in) */
/* TLV[]              : SyncFrame TLVs                                  (in) */


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_PTCP_DIAG_IND_PROVIDE           */
/*---------------------------------------------------------------------------*/
/* Support:EDDI [-] EDDP [x] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDD_RQB_PTCP_DIAG_IND_PROVIDE_TYPE
{
    LSA_UINT8           SyncId;
    LSA_UINT8           DiagSrc;
    LSA_UINT8           SlaveState;
    LSA_UINT8           RcvSyncPrio;
    EDD_MAC_ADR_TYPE    MasterMacAddr;
    LSA_UINT16          SequenceId;
    LSA_INT32           RateInterval;
} EDD_RQB_PTCP_DIAG_IND_PROVIDE_TYPE;

typedef struct _EDD_RQB_PTCP_DIAG_IND_PROVIDE_TYPE EDD_UPPER_MEM_ATTR * EDD_UPPER_PTCP_DIAG_IND_PROVIDE_PTR_TYPE;

/* SyncId             : Synchronization ID: 0 = Clock, 1 = Time        (out) */
/* DiagSrc            : Cause of diagnosis indication                  (out) */
/* SlaveState         : State of sync slave                            (out) */
/* RcvSyncPrio        : Priority1 of sync master frame                 (out) */
/* MasterMacAddr      : Interface MAC address of sync master           (out) */
/* SequenceId         : Id of the last received sync frame(never cause)(out) */
/* RateInterval       : Rate to the master in ns                       (out) */
/*                      Cause of indication only on change from or to zero   */

/*---------------------------------------------------------------------------*/
/* Synchronous interface for isochronous-mode support                        */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* Support:EDDI [x] EDDP [-] EDDS [-]                                        */
/*---------------------------------------------------------------------------*/
#define EDD_APPLSYNC_CMD_START_SYNC     1            //start sync coupling (with parameters Sync==SyncOK)
#define EDD_APPLSYNC_CMD_START_ASYNC    2            //start async coupling (not synchronized Sync != SyncOK)
#define EDD_APPLSYNC_CMD_RESTART        3            //restart coupling (after freerun)
#define EDD_APPLSYNC_CMD_CHECK          4            //check tracks
#define EDD_APPLSYNC_CMD_FREERUN        5            //set coupling to freerunning (after coupled)
#define EDD_APPLSYNC_CMD_STOP           6            //stop coupling
#define EDD_APPLSYNC_CMD_CHECK_PARAMS   7            //Check parameters prior to START

#define EDD_APPLSYNC_TRACK_MASK_EARLY       0x01
#define EDD_APPLSYNC_TRACK_MASK_INTIME      0x02
#define EDD_APPLSYNC_TRACK_MASK_LATE_I      0x04
#define EDD_APPLSYNC_TRACK_MASK_LATE_O      0x08
#define EDD_APPLSYNC_TRACK_MASK_LATE        (EDD_APPLSYNC_TRACK_MASK_LATE_I | EDD_APPLSYNC_TRACK_MASK_LATE_O)
#define EDD_APPLSYNC_TRACK_MASK_ALL         0x0F

#define EDD_APPLSYNC_INT_NEWCYCLE         0x01
#define EDD_APPLSYNC_INT_TM               0x02
#define EDD_APPLSYNC_INT_TA               0x04
#define EDD_APPLSYNC_INT_ALL              0x07

typedef struct _EDD_APPL_SYNC_START_PARAM_TYPE
{   
    LSA_UINT8           ApplNr;                 //IN: IRTE:   irrelevant
                                                //    PNIP:   Nr. of ApplTimer. Allowed Values: 1?
    LSA_UINT8           If;                     //IN: IRTE:   irrelevant
                                                //    PNIP:   Interface the ApplicationTimer shall be coupled to
    LSA_UINT8           IntSrc ;                //IN: Interrupt-controller (see EDDI_APPLSYNC_INTSRC_xxx/EDDP_APPLSYNC_INTSRC_xxx)
    void              * pIRTE_PNIP;             //IN: IRTE:   virtual address of the IRTE-registers (as the user "sees" the IRTE-registers).
                                                //    PNIP:   virtual address of the PNIP-registers (as the user "sees" the PNIP-registers).
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, pCBF) (LSA_USER_ID_TYPE UserID, LSA_BOOL bOBLoss); //Pointer to the function that is called in case of TM-Int.
    LSA_USER_ID_TYPE    UserID;                 //IN: UserID passed when calling pCBF
    LSA_UINT16          CACF;                   //IN: Controller application cycle Factor. Valid Range: 1..16
    LSA_UINT16          ReductionRatio;         //IN: Reduction ratio. Valid range: 1,2,4,8,16,32,64,128,256 The valid range depends on wether Class2- or Class3-communication is running.
    LSA_UINT32          Tm_ns;                  //IN: Tm in ns. Valid range: 0..(CycleLen*ReductionRatio - Tk_ns)
    LSA_UINT32          Te_ns;                  //IN: Te in ns. Valid range: 0..(CycleLen*ReductionRatio - Tk_ns)
    union
    {
        struct
        {
            LSA_UINT32  Tki_ns;                 //IN: Tk inputs in ns. Valid range: 0..(CycleLen*ReductionRatio - Tm_ns)
            LSA_UINT32  Tko_ns;                 //IN: Tk outputs in ns. Valid range: 0..(CycleLen*ReductionRatio - Tm_ns)
        }SeparateTracks;

    LSA_UINT32          Tk_ns;                  //IN: Tk in ns. Valid range: 0..(CycleLen*ReductionRatio - Tm_ns)
    }Tk;
    LSA_UINT16       * pHandle;                 //OUT: Returns a handle for all further accesses to this instance.
    LSA_UINT32       * pXPLLOUT_CycleLen_ns;    //OUT: New Cyclelenght of XPLLOUT   
} EDD_APPL_SYNC_START_PARAM_TYPE;

typedef struct _EDD_APPL_SYNC_CHECK_PARAM_TYPE
{
    LSA_UINT16    Handle;
} EDD_APPL_SYNC_CHECK_PARAM_TYPE;

typedef struct _EDD_APPL_SYNC_FREERUN_PARAM_TYPE
{
    LSA_UINT16        Handle;
} EDD_APPL_SYNC_FREERUN_PARAM_TYPE;

typedef struct _EDD_APPL_SYNC_RESTART_PARAM_TYPE
{
    LSA_UINT16        Handle;
} EDD_APPL_SYNC_RESTART_PARAM_TYPE;

typedef struct _EDD_APPL_SYNC_STOP_PARAM_TYPE
{
    LSA_UINT16        Handle;
} EDD_APPL_SYNC_STOP_PARAM_TYPE;

typedef struct _EDD_APPL_SYNC_PARAM_TYPE
{
    LSA_UINT8 Cmd;
    union
    {
        EDD_APPL_SYNC_START_PARAM_TYPE    Start;
        EDD_APPL_SYNC_RESTART_PARAM_TYPE  Restart;
        EDD_APPL_SYNC_CHECK_PARAM_TYPE    Check;
        EDD_APPL_SYNC_FREERUN_PARAM_TYPE  FreeRun;
        EDD_APPL_SYNC_STOP_PARAM_TYPE     Stop;
    } Param;
} EDD_APPL_SYNC_PARAM_TYPE;

/*===========================================================================*/
/*======================= I2C - INTERFACE ===================================*/
/*===========================================================================*/
#define EDD_I2C_MAX_DATA_SIZE   300UL   //maximum data size for I2C transfers

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*==== set and get parameter of the edd-part of the rqb ===*/

#define EDD_RQB_SET_SERVICE(_pRQB, _Value) \
    ( (_pRQB)->Service = (_Value) )

#define EDD_RQB_GET_SERVICE(_pRQB) \
    ( (_pRQB)->Service )

#define EDD_RQB_SET_PPARAM(_pRQB, _Value)   \
    ( (_pRQB)->pParam = (EDD_UPPER_MEM_PTR_TYPE)(_Value) )

#define EDD_RQB_GET_PPARAM(_pRQB) \
    ( (_pRQB)->pParam  )

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*****************************************************************************/
/*                                                                           */
/* Output-functions are in edd_out.h                                         */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*  end of file edd_usr.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDD_USR_H */
