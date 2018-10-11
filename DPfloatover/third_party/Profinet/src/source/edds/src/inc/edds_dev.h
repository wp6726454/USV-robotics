
#ifndef EDDS_DEV_H                    /* ----- reinclude-protection ----- */
#define EDDS_DEV_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_dev.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for EDDS                */
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
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  15.06.04    JS    removed ASRT functions                                 */
/*  24.06.04    JS    added udp-support                                      */
/*  03.08.04    JS    added Rx-Statistics                                    */
/*  04.10.04    JS    moved EDDS_MAX_PORT_CNT to edds_sys.h                  */
/*  07.10.04    JS    changed LSA_BITFIELD to LSA_UINT32                     */
/*  14.10.04    JS    added multicast MAC management                         */
/*  15.11.04    JS    added InterfaceID                                      */
/*  20.01.05    JS    added Rx Overload support.                             */
/*  11.05.05    JS    added PhyType                                          */
/*  22.06.05    JS    added CableDelayNs                                     */
/*  04.10.05    JS    added LED-blink support                                */
/*  26.01.06    JS    added RxOverload statistics                            */
/*  10.03.06    JS    added PrmIndReq queue                                  */
/*  02.05.06    JS    added PortMAC addresses                                */
/*  03.05.06    JS    added PDPortData records and changed some naming       */
/*  29.06.06    JS    added LesserCheckQuality                               */
/*  10.10.06    JS    added TXCOMPID_NRT_PRIO_3                              */
/*  17.01.07    JS    added Autoneg to Link-Stat                             */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
/*  23.07.07    JS    added Peer2PeerBounary and DCPBoundary records         */
/*  10.01.08    JS    added PDInterfaceMrpDataAdjust, PDPortMrpDataAdjust    */
/*  12.02.08    JS    added "IsWireless" parameter.                          */
/*  04.04.08    JS    added PDSetDefaultPortStates record                    */
/*  26.06.08    JS    added "volatile" to avoid possible problems with       */
/*                    compiler optimizations (reordering of code)            */
/*  10.12.08    JS    added PhyStatus                                        */
/*  22.12.08    JS    added EDDS_RQB_SWI_SET_PORT_REDUNDANT_TYPE. moved from */
/*                    edd_usr.h. No more used by API                         */
/*                    added EDDS_RQB_SWI_SET_BRIDGE_MODE_TYPE. moved from    */
/*                    edd_usr.h. No more used by API                         */
/*                    removed MRRT_PortState                                 */
/*  28.01.09    JS    added port/if specfic statistics per channel           */
/*  02.02.09    JS    removed PhyType, added AutonegCapAdvertised            */
/*                    removed LinkSpeedModeCapability                        */
/*  04.05.09    JS    moved some defines from edd_usr.h                      */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Because of the zero-copy - interface we must take care of alignment and   */
/* buffersize requirements from the ethernetcontroller within application.   */
/*---------------------------------------------------------------------------*/
/* The EDDS needs the bufferrequirements mentioned below.                    */
/* For Rx-Frames and SRT - Tx-Frames it needs DWORD-Aligned buffers because  */
/* it needs to access the framecontent with WORD and DWORD - accesses. To    */
/* prevent misaligned accesses the buffers have to be proper aligned.        */
/* For NRT - Tx- Buffers the EDDS does not access the buffer, so the edds has */
/* no special requirements.                                                  */
/*---------------------------------------------------------------------------*/

#define EDDS_RX_FRAME_BUFFER_ALIGN      0x03 /* 4 -Byte-aligned frame buffers*/
#define EDDS_TX_NRT_FRAME_BUFFER_ALIGN  0x00 /* no alignment needed          */
#define EDDS_TX_SRT_FRAME_BUFFER_ALIGN  0x03 /* 4 -Byte-aligned frame buffers*/

/*---------------------------------------------------------------------------*/
/* type/len-field types                                                      */
/*---------------------------------------------------------------------------*/

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_VLAN_TAG                           0x0081
#define EDDS_xRT_TAG                            0x9288
#define EDDS_LLDP_TAG                           0xCC88
#define EDDS_IP_TAG                             0x0008
#define EDDS_ARP_TAG                            0x0608
#define EDDS_RARP_TAG                           0x3580    /* reversed ARP */
#define EDDS_MRP_TAG                            0xE388
#else
#define EDDS_xRT_TAG                            0x8892
#define EDDS_LLDP_TAG                           0x88CC
#define EDDS_IP_TAG                             0x0800
#define EDDS_ARP_TAG                            0x0806
#define EDDS_RARP_TAG                           0x8035    /* reversed ARP */
#define EDDS_VLAN_TAG                           0x8100
#define EDDS_MRP_TAG                            0x88E3
#endif

/*---------------------------------------------------------------------------*/
/* xRToverUDP UDP-Port                                                       */
/*---------------------------------------------------------------------------*/

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_xRT_UDP_PORT                       0x9288
#else
#define EDDS_xRT_UDP_PORT                       0x8892
#endif


/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* General Ethernet Header:                                                  */
/*                                                                           */
/*+   6   6   2     2     2                                                 +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+ |DA |SA |VLAN|VLAN |Type |                                              +*/
/*+ |   |   |TYPE| ID  |LEN  |                                              +*/
/*+ |   |   |    |     |     |                                              +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+          \--------/                                                     +*/
/*+           optional                                                      +*/
/*---------------------------------------------------------------------------*/

/* for optimized access to framebuffer we use this defines       */
/* The framebuffer has to be DWORD-aligned                       */

#define EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN           14
#define EDDS_FRAME_HEADER_SIZE_WITH_VLAN              18

#define EDDS_FRAME_LEN_TYPE_WORD_OFFSET                6
#define EDDS_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET      8

/* if VLAN Tag is present */
#define EDDS_FRAME_VLAN_TYPE_WORD_OFFSET              EDDS_FRAME_LEN_TYPE_WORD_OFFSET
#define EDDS_FRAME_VLAN_ID_WORD_OFFSET                 7

/* Macro to get LEN_TYPE-Field of Ethernetframe. This may be the VLAN-Type!  */
/* pBuffer has to be at least 16Bit aligned and points to start of frame     */

#define EDDS_FRAME_GET_LEN_TYPE(pBuffer)(((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET])

/*---------------------------------------------------------------------------*/
/* Frameformat for xRT over Ethernet                                         */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ mandatory on send      */
/* 12/16 | 2 |TypeLenxRT                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 | 2 |FrameID                              |                         */
/* 16/20 |   |Data                                 |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define EDDS_FRAME_XRT_FRAME_ID_WORD_OFFSET            7     /* without VLAN */
#define EDDS_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET  9

/*---------------------------------------------------------------------------*/
/* Frameformat for xRT over UDP                                              */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ not inserted on send   */
/* 12/16 | 2 |IP-Type                              |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 |20 |IPHeader                             |  we only accept 20 Byte */
/* ------+---+-------------------------------------+-------------------------*/
/* 34/38 | 8 |UDP-Header                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 42/46 | 2 |FrameID                              |                         */
/* 44/48 |   |Data                                 |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* for IP/UPD-Header see edds_pck.txt/h */

/*---------------------------------------------------------------------------*/
/* The following defines are only used if EDDS_CFG_SEND_BLOCKED is defined   */
/*---------------------------------------------------------------------------*/

#ifndef EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_100
#define EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_100 0xFFFFFFFFl
#endif

#ifndef EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10
#define EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10  0xFFFFFFFFl
#endif


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/* Handle-management-structures                                              */
/* (this structure is asigned to every open channel. Each component (NRT,SRT */
/* IRT) can have individual parameters within management structure only used */
/* in component.                                                             */
/*===========================================================================*/

typedef struct _EDDS_HDB_TYPE     EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_HDB_PTR_TYPE;
typedef struct _EDDS_DDB_TYPE     EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DDB_PTR_TYPE;

/* foreward declaration of LLIF table structure pointer (@see edds_llif.h) */
typedef struct _EDDS_LL_TABLE_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE;

/* A EDDS_TRANSFER_BUFFER is an array of EDD_FRAME_BUFFER_LENGTH byte */
typedef LSA_UINT8 EDDS_TRANSFER_BUFFER[EDD_FRAME_BUFFER_LENGTH];
typedef EDDS_TRANSFER_BUFFER      EDDS_LOCAL_MEM_ATTR *     EDDS_TRANSFER_BUFFER_ARRAY;

typedef struct _EDDS_LINK_BASE_TYPE
{
    EDD_UPPER_RQB_PTR_TYPE          pBottom;
    EDD_UPPER_RQB_PTR_TYPE          pTop;
} EDDS_LINK_BASE_TYPE;

typedef struct _EDDS_LINK_BASE_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_LINK_BASE_TYPE_PTR;

typedef struct _EDDS_HDB_TYPE
{
    LSA_BOOL                            InUse;
    LSA_UINT32                          UsedComp;
    LSA_SYS_PATH_TYPE                   SysPath;
    LSA_SYS_PTR_TYPE                    pSys;
    EDDS_DETAIL_PTR_TYPE                pDetail;
    LSA_HANDLE_TYPE                     UpperHandle;
    LSA_HANDLE_TYPE                     Handle;
    LSA_VOID                            LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);

    LSA_BOOL                            InsertSrcMAC; /* EDDS inserts station MAC */
    /* address in send frame   */
    /* before sending.         */
    /* Link-Indication Requests*/
    EDDS_LINK_BASE_TYPE                LinkIndReq[EDDS_MAX_PORT_CNT+1];  /* index 0 is AUTO */
    EDD_UPPER_RQB_PTR_TYPE              RxOverloadIndReq; /* Rx-Overload-Ind Req.*/

    /* ResetValue for Statistics    */
    /* because Statistics can be    */
    /* Reset per channel and Port   */
    /* we must calculate the actual */
    /* depending on this values     */
    EDDS_STATISTICS_TYPE               StatsResetValues[EDDS_MAX_PORT_CNT+1];  /* index 0 is AUTO */


    EDDS_LOCAL_DDB_PTR_TYPE             pDDB;         /* link to devicemanagement*/
    #ifdef EDDS_CFG_NRT_INCLUDE
    EDDS_LOCAL_HDB_COMP_NRT_PTR_TYPE    pNRT;
    #else
    EDDS_LOCAL_MEM_PTR_TYPE             pNRT;
    #endif
} EDDS_HDB_TYPE;


/* defines for UsedComp (can be ored) */

#define EDDS_COMP_NRT   0x01
#define EDDS_COMP_CSRT  0x04    /* cyclic  SRT */
#define EDDS_COMP_IRT   0x08
#define EDDS_COMP_SWI   0x10
#define EDDS_COMP_PRM   0x20

typedef struct _EDDS_HDB_INFO_TYPE
{
    LSA_UINT32               MaxHandle;
    LSA_UINT32               UsedHandle;
    EDDS_LOCAL_HDB_PTR_TYPE pTabHDB;
} EDDS_HDB_INFO_TYPE;

/*===========================================================================*/
/* Device-description block (DDB)                                            */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* general devicemanagement (component independend)                          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_TX_INFO_TYPE
{
    LSA_UINT32      InUse;                      /* Transmit in progress      */
    LSA_UINT32      TxCompID;                   /* Component ID for this Tx  */
} EDDS_TX_INFO_TYPE;

typedef struct _EDDS_TX_INFO_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_TX_INFO_PTR_TYPE;

/* defines for TxCompID */

#define EDDS_TXCOMPID_NRT_PRIO_0 0       /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_1 1       /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_2 2       /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_NRT_PRIO_3 3       /* this tx-entry is from NRT            */
#define EDDS_TXCOMPID_SRT_Z_XRT  4       /* this tx-entry is from cyclic xRT     */
#define EDDS_TXCOMPID_SRT_Z_UDP  5       /* this tx-entry is from cyclic xRToUDP */

/* Multicast MAC-Management */
typedef struct _EDDS_MC_MAC_TYPE
{
    LSA_UINT32         Cnt;                   /* MC-MAC usage counter      */
    EDD_MAC_ADR_TYPE   MAC;                   /* MC-MAC if used            */
} EDDS_MC_MAC_TYPE;

typedef struct _EDDS_MC_MAC_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_MC_MAC_PTR_TYPE;

/* the number of supported MC depends on LLIF, so the LLIF can configure   */
/* the number of MC MACs managed by EDDS. If the EDDS resources filled up, */
/* the EDDS set a RESOURCE-Error. In addition the LLIF can set a RESOURCE  */
/* error too. So it dont cares if the the EDDS can manage more MC-MACs then */
/* the LLIF supports. Both can signal a RESOURCE error. But it makes no    */
/* sense to let the EDDS manage more MC then the LLIF supports. On the other*/
/* hand the EDDS may manage less MC then the LLIF supports (to save memory) */

#ifdef LLIF_CFG_MC_MAC_CNT
#define EDDS_MC_MAC_CNT LLIF_CFG_MC_MAC_CNT
#else
#define EDDS_MC_MAC_CNT 20  /* default if not defined by LLIF */
#endif

typedef struct _EDDS_MC_MAC_INFO_TYPE
{
    EDDS_MC_MAC_TYPE McMac[EDDS_MC_MAC_CNT];
} EDDS_MC_MAC_INFO_TYPE;

typedef struct _EDDS_MC_MAC_INFO_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_MC_MAC_INFO_PTR_TYPE;

#define EDDS_MC_NO_IDX 0xFFFFFFFF;


typedef struct _EDDS_HW_PARAM_STRUCT
{
    LSA_UINT32        InterfaceID;    /* Interface ID from DPB                   */
    EDDS_LL_CAPS_TYPE Caps;           /* Capabilities */
    EDD_MAC_ADR_TYPE  MACAddress;     /* Station MAC address.                    */
    EDD_MAC_ADR_TYPE  MACAddressPort[EDDS_MAX_PORT_CNT]; /* Port MAC addresses   */
    LSA_UINT8         PHYPower[EDDS_MAX_PORT_CNT];      /* Configured Power      */
    LSA_UINT8         LinkSpeedMode[EDDS_MAX_PORT_CNT]; /* Configured SpeedMode  */
    LSA_BOOL          IsWireless[EDDS_MAX_PORT_CNT];    /* Configured IsWireless */
    EDD_IP_ADR_TYPE   IPAddress;      /* Local IP address                        */
} EDDS_HW_PARAM_TYPE;

typedef EDDS_HW_PARAM_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_HW_PARAM_PTR_TYPE;

typedef struct _EDDS_STAT_TX_STRUCT
{
    LSA_UINT32    TxTimeout;
} EDDS_STAT_TX_TYPE;

typedef EDDS_STAT_TX_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_STAT_TX_TYPE_PTR;

typedef struct _EDDS_STAT_RX_STRUCT
{
    LSA_UINT32    RxXRToUDPIPWrongChecksum;
    LSA_UINT32    RxXRToUDPWrongHeader;
    LSA_UINT32    RxOverload;             /* rx-overload (rx-flooding) */
} EDDS_STAT_RX_TYPE;

typedef EDDS_STAT_RX_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_STAT_RX_TYPE_PTR;

typedef volatile struct _EDDS_TIMER_STRUCT
{
    LSA_BOOL       InUse;                /* LSA_TRUE: Timer in USE  */
    LSA_BOOL       InProgress;           /* LSA_TRUE: Timer running */
    LSA_TIMER_ID_TYPE TimerID;           /* LSA Timer ID            */
    LSA_UINT16     EDDSTimerID;         /* Timer ID within EDDS     */
} EDDS_TIMER_TYPE;

typedef EDDS_TIMER_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_TIMER_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* This structure is used for internal errors detected for debugging         */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_INTERNAL_ERROR_STRUCT
{
    LSA_UINT32    TxResource;           /* Tx-Resources error            */
    LSA_UINT32    RxOverloadCnt;        /* Number of RxOverloads occured */
} EDDS_INTERNAL_ERROR_TYPE;

typedef EDDS_INTERNAL_ERROR_TYPE EDDS_LOCAL_MEM_ATTR *     EDDS_INTERNAL_ERROR_TYPE_PTR;


/*---------------------------------------------------------------------------*/
/* actual LinkStatus/Speed and Mode of PHY                                   */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_LINK_STAT_STRUCT
{
    LSA_UINT8           Status;
    LSA_UINT8           Speed;
    LSA_UINT8           Mode;
    LSA_UINT8           Autoneg;        /* Autoneg configuration                   */
    /* EDD_AUTONEG_ON                          */
    /* EDD_AUTONEG_OFF                         */
    LSA_UINT16          MAUType;        /* MAU Type                                */
    LSA_UINT8           MediaType;      /* Media Type                              */
    LSA_UINT8           PortStatus;     /* Status of Port                          */
    LSA_UINT8           PhyStatus;      /* PHYStatus of Port                       */
    LSA_UINT16          PortState;      /* State  of Port                          */
    LSA_UINT32          LineDelay;      /* LineDelay set by EDD_SRV_SET_LINE_DELAY */
    LSA_UINT32          CableDelayNs;   /* CableDel. set by EDD_SRV_SET_LINE_DELAY */
    LSA_UINT32          AutonegCapAdvertised;      /* autoneg advertised capabil.  */
} EDDS_LINK_STAT_TYPE;

typedef struct _EDDS_LINK_STAT_STRUCT EDDS_LOCAL_MEM_ATTR *    EDDS_LINK_STAT_PTR_TYPE;

/* defines for Status       see EDD_SRV_GET_LINK_STATUS*/
/* defines for Speed        see EDD_SRV_GET_LINK_STATUS*/
/* defines for Mode         see EDD_SRV_GET_LINK_STATUS*/
/* defines for PortState    see edd_usr.h              */
/* defines for PHYStatus    see edd_usr.h              */
/* defines for LinkSpeedModeCapability  see edd_usr.h  */

/*---------------------------------------------------------------------------*/
/* actual Rx overload stats                                                  */
/*---------------------------------------------------------------------------*/
#if defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)

typedef struct _EDDS_RX_OVERLOAD_STRUCT
{
    LSA_UINT32          Status;      /* Rx-Overload status                  */
    /* EDD_NO_RX_OVERLOAD     (= ON)       */
    /* EDD_RX_OVERLOAD        (= OFF)      */
    LSA_BOOL            RxOFF;       /* Receiver ON/OFF                     */
    EDDS_TIMER_TYPE     Timer;       /* Rx-overload check Timer info        */
    LSA_UINT32          RxCnt;       /* RxCnt till last trigger             */
    LSA_UINT32          OFFCnt;      /* Number of remaining cycles Rx       */
    /* remains OFF (0 if ON)               */
    LSA_UINT32          OFFLevel;    /* RX_OFF level (0,1,2) for next OFF   */
    /* Set to 0 if we are ON for at least  */
    /* two check cycles or in startup      */
    LSA_UINT32          ONCnt;       /* Set to  EDDS_CFG_RX_OVERLOAD_RXON_MIN*/
    /* after turning ON after OFF. Counts  */
    /* down to 0.                          */
} EDDS_RX_OVERLOAD_TYPE;
#endif /* defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */
/*---------------------------------------------------------------------------*/
/* actual LED blink status                                                   */
/*---------------------------------------------------------------------------*/
typedef struct _EDDS_LED_BLINK_TYPE
{
    LSA_UINT8           Status;        /* Blink running?                      */
    LSA_BOOL            LEDOn;         /* LED on/off?                         */
    LSA_UINT32          RemainTotal;   /* Time till LED blink stopped (ticks) */
    LSA_UINT32          Remain;        /* Time till LED on/off changed (ticks)*/
    EDDS_TIMER_TYPE     Timer;         /* Rx-overload check Timer info        */
    LSA_UINT16          OnOffDurationIn500msTicks;   /* from RQB: range 1..10 */
} EDDS_LED_BLINK_TYPE;

typedef struct _EDDS_LED_BLINK_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LED_BLINK_PTR_TYPE;

/* Status */
#define EDDS_LED_BLINK_INACTIVE    0
#define EDDS_LED_BLINK_ACTIVE      1
/*---------------------------------------------------------------------------*/
/* PRM/Diagnosis (configured by PRM-services)                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* PDU-Format:                                                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*   BlockHeader                                                             */
/*---------------------------------------------------------------------------*/
/*    LSA_UINT16          BlockType                                     2    */
/*    LSA_UINT16          BlockLength        (without Type/Length)      2    */
/*    LSA_UINT8           BlockVersionHigh                              1    */
/*    LSA_UINT8           BlockVersionLow                               1    */
/*                                                                     ---   */
/*                                                                      6    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_HEADER_SIZE 6
#define EDDS_PRM_BLOCK_VERSION_HIGH                 0x01
#define EDDS_PRM_BLOCK_VERSION_LOW                  0x00

/*---------------------------------------------------------------------------*/
/*   PDNCDataCheck (= Frame dropped - no resource)                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          MaintenanceRequiredPowerBudget;               4    */
/*    LSA_UINT32          MaintenanceDemandedPowerBudget;               4    */
/*    LSA_UINT32          ErrorPowerBudget;                             4    */
/*                                                                    -----  */
/*                                                                     20    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDNC_DATA_CHECK        0x8070

#define EDDS_PRM_PDNC_DATA_CHECK_BLOCK_TYPE          0x0230
#define EDDS_PRM_PDNC_DATA_CHECK_BLOCK_SIZE         (EDDS_PRM_RECORD_HEADER_SIZE+14)

#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_VALUE_MSK    0x7FFFFFFF

#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_MSK    0x80000000
#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_ON     0x80000000
#define EDDS_PRM_PDNC_DATA_CHECK_BUDGET_CHECK_OFF    0x00000000

#define EDDS_PRM_PDNC_DROP_BUDGET_MIN                0x1
#define EDDS_PRM_PDNC_DROP_BUDGET_MAX                0x3E7

/*---------------------------------------------------------------------------*/
/* Record for ExtChannelErrorType "Frame dropped - no resource"              */
/* Note: will be set to 0 in startup -> default record                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_BUDGET_TYPE
{
    LSA_BOOL      Enabled;
    LSA_UINT32    Value;
} EDDS_PRM_BUDGET_TYPE;

#define EDDS_FRAME_DROP_BUDGET_CNT            3

typedef struct _EDDS_NC_DATA_CHECK_RECORD_SET_TYPE
{
    LSA_BOOL               Present;  /* record set present ? */
    EDDS_PRM_BUDGET_TYPE  Budget[EDDS_FRAME_DROP_BUDGET_CNT];
} EDDS_NC_DATA_CHECK_RECORD_SET_TYPE;

typedef struct _EDDS_NC_DATA_CHECK_RECORD_SET_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_NC_DATA_CHECK_RECORD_SET_PTR_TYPE;

/* do not change order ! */
#define EDDS_MAINTENANCE_REQUIRED_BUDGET_IDX  0
#define EDDS_MAINTENANCE_DEMANDED_BUDGET_IDX  1
#define EDDS_ERROR_BUDGET_IDX                 2

typedef struct _EDDS_PRM_NC_DATA_TYPE
{
    LSA_UINT32          DropCnt;                      /* drop count last interval */
    LSA_BOOL            CheckEnable;                  /* Check Enable             */
    LSA_UINT8           ErrorStatus;                  /* Error Status             */
    LSA_UINT8           ErrorStatusIndicated;         /* Error Status indicated   */
    EDDS_NC_DATA_CHECK_RECORD_SET_TYPE   RecordSet_A; /* Current RecordSet        */
    EDDS_NC_DATA_CHECK_RECORD_SET_TYPE   RecordSet_B; /* temporary RecordSet      */
} EDDS_PRM_NC_DATA_TYPE;

typedef struct _EDDS_PRM_NC_DATA_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_NC_DATA_PTR_TYPE;

/* CheckEnable (if record set A is present and something to check) */
/* ErrorStatus          (actual Error Status)                      */
/* ErrorStatusIndicated (actual Error Status indicated to user)    */
#define EDDS_DIAG_FRAME_DROP_OK                         0    /* dont change */
#define EDDS_DIAG_FRAME_DROP_ERROR                      1
#define EDDS_DIAG_FRAME_DROP_MAINTENANCE_DEMANDED       2
#define EDDS_DIAG_FRAME_DROP_MAINTENANCE_REQUIRED       3

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
/*    AdjustDomainBoundary:                                                  */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT32          DomainBoundary                                4    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*                                                                    -----  */
/*                                                                     16    */
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
/*    AdjustPreambleLength                                                   */
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          PreambleLength                                2    */
/*    LSA_UINT16          AdjustProperties                              2    */
/*                                                                    -----  */
/*                                                                     12    */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDPORT_DATA_ADJUST             0x802F

#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_TYPE               0x0202
#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MIN_SIZE          (EDDS_PRM_RECORD_HEADER_SIZE+6)

/* AdjustDomainBoundary  */
#define EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_TYPE   0x0209
#define EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_SIZE  (EDDS_PRM_RECORD_HEADER_SIZE+10)

/* AdjustMulticastBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_TYPE  0x0210
#define EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

#define EDDS_PRM_PDPORT_DATA_MC_BOUNDARY_BLOCK               1
#define EDDS_PRM_PDPORT_DATA_MC_BOUNDARY_NOT_BLOCK           0

#define EDDS_PRM_PDPORT_DATA_MC_ADJUST_PROPERTIES            0

/* AdjustMauType */
#define EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_TYPE      0x020E
#define EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_MAUTYPE_ADJUST_PROPERTIES       0

/* AdjustPortState */
#define EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_TYPE    0x021B
#define EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE   (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_PORTSTATE_UP                    1
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_DOWN                  2  /* only valid with adjust */
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_TESTING               3
#define EDDS_PRM_PDPORT_DATA_PORTSTATE_UNKNOWN               4

#define EDDS_PRM_PDPORT_DATA_PORTSTATE_ADJUST_PROPERTIES     0

/* AdjustPeerToPeerBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_TYPE  0x0224
#define EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

/* AdjustDCPBoundary */
#define EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_TYPE  0x0225
#define EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE (EDDS_PRM_RECORD_HEADER_SIZE+10)

#define EDDS_PRM_PDPORT_DATA_DCP_BOUNDARY_BLOCK               1
#define EDDS_PRM_PDPORT_DATA_DCP_BOUNDARY_NOT_BLOCK           0

#define EDDS_PRM_PDPORT_DATA_DCP_ADJUST_PROPERTIES            0

/* AdjustPreambleLength */
#define EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_TYPE   0x0226
#define EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_SIZE   (EDDS_PRM_RECORD_HEADER_SIZE+6)

#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_SHORT                1   // One octet Preamble shall be used
#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_LONG                 0   // Seven octets Preamble shall be used
#define EDDS_PRM_PDPORT_DATA_PREAMBLE_LENGTH_PROPERTIES           0

/* MAX Record Size */
/* Note: we add all block but MAUType and PORTState will not occure both! */

#define EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE          ((EDDS_PRM_RECORD_HEADER_SIZE+6) + \
EDDS_PRM_PDPORT_DATA_ADJUST_D_BOUNDARY_SBLOCK_SIZE    + \
EDDS_PRM_PDPORT_DATA_ADJUST_MC_BOUNDARY_SBLOCK_SIZE   + \
EDDS_PRM_PDPORT_DATA_ADJUST_MAUTYPE_SBLOCK_SIZE       + \
EDDS_PRM_PDPORT_DATA_ADJUST_PORTSTATE_SBLOCK_SIZE     + \
EDDS_PRM_PDPORT_DATA_ADJUST_P2P_BOUNDARY_SBLOCK_SIZE  + \
EDDS_PRM_PDPORT_DATA_ADJUST_DCP_BOUNDARY_SBLOCK_SIZE  + \
EDDS_PRM_PDPORT_DATA_ADJUST_PREAMBLE_LENGTH_SBLOCK_SIZE)

typedef struct _EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE
{
    LSA_BOOL               Present;                   /* record set present at all ? */
    /* (but maybe no subblock!)    */
    LSA_BOOL               MulticastBoundaryPresent;
    LSA_BOOL               MAUTypePresent;
    LSA_BOOL               PortStatePresent;
    LSA_BOOL               DCPBoundaryPresent;
    LSA_BOOL               PreambleLengthPresent;
    LSA_UINT8              LinkStat;                  /* SpeedMode form MAUType if present */
    LSA_UINT16             SlotNumber;
    LSA_UINT16             SubSlotNumber;
    LSA_UINT32             MulticastBoundary;
    LSA_UINT16             MulticastBoundaryAP;
    LSA_UINT16             MAUType;
    LSA_UINT16             MAUTypeAP;
    LSA_UINT16             PortState;
    LSA_UINT16             PortStateAP;
    LSA_UINT32             DCPBoundary;
    LSA_UINT16             DCPBoundaryAP;
    LSA_UINT16             PreambleLength;
    LSA_UINT16             PreambleLengthAP;
    LSA_UINT32             RecordLength;                                      /* record length if "Present" */
    LSA_UINT8              Record[EDDS_PRM_PDPORT_DATA_ADJUST_BLOCK_MAX_SIZE]; /* record if "Present" */
} EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE;

typedef struct _EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE;

typedef struct _EDDS_PRM_PORT_DATA_TYPE
{
    LSA_BOOL                                LesserCheckQuality;              /* Flag form CDB.           */
    EDD_UPPER_PORTID_MODE_PTR_TYPE          pPortIDModeArray;                /* allocated on startup     */
    /* (one entry only!)        */
    EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE  RecordSet_A[EDDS_MAX_PORT_CNT];   /* Current RecordSet        */
    EDDS_PORT_DATA_ADJUST_RECORD_SET_TYPE  RecordSet_B[EDDS_MAX_PORT_CNT];   /* temporary RecordSet      */
} EDDS_PRM_PORT_DATA_TYPE;

typedef struct _EDDS_PRM_PORT_DATA_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PORT_DATA_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/*   PDSCFDataCheck (= SendClock factor check)                               */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT16          SendClockFactor                               2    */
/*    LSA_UINT8           Data[x]                                       x    */
/*                                                                    -----  */
/*                                                                      8+x  */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDSCF_DATA_CHECK        0x10000

#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_TYPE          0xF000
#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+2)
#define EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE	  (EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MIN_SIZE+4) /* x == 4 */

typedef struct _EDDS_PRM_PDSCF_DATA_CHECK
{
    LSA_BOOL               Present;                   							/* record set present at all ? */
    LSA_UINT32             RecordLength;                                        /* record length if "Present" */
    LSA_UINT8              Record[EDDS_PRM_PDSCF_DATA_CHECK_BLOCK_MAX_SIZE];  /* record if "Present" */
} EDDS_PRM_PDSCF_DATA_CHECK;


/*---------------------------------------------------------------------------*/
/*   PDPortMrpDataAdjust                                                     */
/*   Note: We only support WRITE ! and only check for Record to be written   */
/*   (no validation!)                                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_MRP_DATA_TYPE
{
    LSA_BOOL                             Present_A[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             Present_B[EDDS_MAX_PORT_CNT];
    LSA_BOOL                             AtLeastTwoRecordsPresentB;      /* TRUE if at least 2 records present within */
    /* Record set B on END. helper var  */
} EDDS_PRM_MRP_PORT_DATA_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDInterfaceMrpDataAdjust                                                */
/*   Note: We only support WRITE ! and only check for Record to be written   */
/*   (no validation!)                                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_MRP_INTERFACE_DATA_TYPE
{
    LSA_BOOL                             Present_A;
    LSA_BOOL                             Present_B;
} EDDS_PRM_MRP_INTERFACE_DATA_TYPE;


/*---------------------------------------------------------------------------*/
/*   PDPortStatistic                                                         */
/*---------------------------------------------------------------------------*/
#define EDDS_PRM_PDPORT_STATISTIC_RECORD_INDEX                      0x8072
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKTYPE                         0x0251
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONHIGH                  (EDDS_PRM_BLOCK_VERSION_HIGH)
#define EDDS_PRM_PDPORT_STATISTIC_BLOCKVERSIONLOW                   (EDDS_PRM_BLOCK_VERSION_LOW)
#define EDDS_PRM_PDPORT_STATISTIC_SIZE								(EDDS_PRM_RECORD_HEADER_SIZE+26)


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

#define EDDS_PRM_RECORD_INDEX_PDSET_DEF_PORTSTATES    0x10001

#define EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_TYPE          0xF001
#define EDDS_PRM_PDSET_DEF_PORTSTATES_BLOCK_MIN_SIZE     (EDDS_PRM_RECORD_HEADER_SIZE+2)

typedef struct _EDDS_PRM_RECORD_SET_PORT_STATES_TYPE
{
    LSA_BOOL                             PortStatePresent;  /* is PortState present? */
    LSA_UINT8                            PortState;
} EDDS_PRM_RECORD_SET_PORT_STATES_TYPE;

/* PortState : */
#define EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_DISABLE  0x01
#define EDDS_PRM_RECORD_PD_SET_DEF_PORTSTATES_BLOCKING 0x02

typedef struct _EDDS_PRM_SET_DEF_PORTSTATES_TYPE
{
    LSA_BOOL                             Present_A;
    LSA_BOOL                             Present_B;
    EDDS_PRM_RECORD_SET_PORT_STATES_TYPE RecordSet_A[EDDS_MAX_PORT_CNT];
    EDDS_PRM_RECORD_SET_PORT_STATES_TYPE RecordSet_B[EDDS_MAX_PORT_CNT];
} EDDS_PRM_SET_DEF_PORTSTATES_TYPE;

typedef struct _EDDS_PRM_SET_DEF_PORTSTATES_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_SET_DEF_PORTSTATES_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct _EDDS_PRM_TYPE
{
    LSA_UINT32                HandleCnt;           /* Number ob Handles(only 1)*/
    LSA_HANDLE_TYPE           Handle;              /* Channel handle           */
    LSA_UINT32                PrmState;            /* Actual State             */
    EDDS_TIMER_TYPE           Timer;               /* cylic One second timer   */
    EDDS_LINK_BASE_TYPE      PrmIndReq[EDDS_MAX_PORT_CNT+1];  /* Indication queues. 0 = Interface   */
    LSA_BOOL                  DoIndicate[EDDS_MAX_PORT_CNT+1]; /* Indicate for port. 0 = Interface  */
    LSA_BOOL                  PRMUsed;             /* PRM  was at least once   */
    /* written.                 */
    LSA_BOOL                  FirstCommitExecuted; /* TRUE after first COMMIT  */
    /* after startup was execut.*/
    EDDS_PRM_NC_DATA_TYPE     FrameDrop;           /* PDNC Record              */
    EDDS_PRM_PORT_DATA_TYPE   PortData;            /* PDPort Record            */
    EDDS_PRM_MRP_PORT_DATA_TYPE      MRPPort;      /* PDPortMrpDataAdjust      */
    EDDS_PRM_MRP_INTERFACE_DATA_TYPE MRPInterface; /* PDInterfaceMrpDataAdjust */
    EDDS_PRM_SET_DEF_PORTSTATES_TYPE DefPortStates;/* PDSetDefaultPortStates   */
    EDDS_PRM_PDSCF_DATA_CHECK        PDSCFDataCheck;/* PDSCFDataCheck          */
} EDDS_PRM_TYPE;

typedef struct _EDDS_PRM_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_PRM_PTR_TYPE;

/* PrmState */
#define EDDS_PRM_STATE_PREPARED    0  /* PREPARED for new param sequence  */
#define EDDS_PRM_STATE_WRITE       1  /* at least one WRITE after PREPARED*/
#define EDDS_PRM_STATE_END         2  /* END arrived (await commit)       */

/*---------------------------------------------------------------------------*/
/*   PDInterfaceMrpDataAdjust , PDPortMrpDataAdjust                          */
/*---------------------------------------------------------------------------*/

#define EDDS_PRM_RECORD_INDEX_PDINTERFACE_MRP_DATA_ADJUST             0x8052
#define EDDS_PRM_RECORD_INDEX_PDPORT_MRP_DATA_ADJUST                  0x8053

/* ---------------------------------------------------------------------------- */
/*   User specific records                                                      */
/*   The following records are not used by EDDP but they must be known          */
/* ---------------------------------------------------------------------------- */
#define EDDS_PRM_PDCONTROL_PLL_RECORD_INDEX                     0xB050
#define EDDS_PRM_PDTRACE_UNIT_CONTROL_RECORD_INDEX              0xB060
#define EDDS_PRM_PDTRACE_UNIT_DATA_RECORD_INDEX                 0xB061

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DEVICE_GLOB_TYPE
{
    /* --- Setup --------------- */
    EDDS_DSB_TYPE                   DSBParams;  /* DSB save.                 */
    /* -- Low-Level-Ethernet---- */
    /* depends on hardware       */
    EDDS_LOCAL_EDDS_LL_TABLE_PTR_TYPE	pLLFunctionTable;   /* Low-Level-function table     */
    EDDS_HANDLE							pLLManagementData;   /* Low-Level-Management data     */
    LSA_UINT32						TraceIdx;
    LSA_BOOL                        HWIsSetup;  /* LSA_TRUE if device_setup  */
    /* was successful.           */
    EDDS_INTERNAL_ERROR_TYPE        Errors;     /* Internal Errors for       */
    /* Debugging                 */
    EDDS_HW_PARAM_TYPE              HWParams;   /* HW-Parameters             */
    /* actual Link-Status        */
    EDDS_LINK_STAT_TYPE            LinkStatus[EDDS_MAX_PORT_CNT+1];  /* 0 = Auto */
#if defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
    EDDS_RX_OVERLOAD_TYPE           RxOverload; /* Rx-Overload Status        */
#endif /* defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */

    EDDS_LED_BLINK_TYPE             LEDBLink;   /* LED blink Status          */
    EDDS_PRM_TYPE                   Prm;        /* Prm/Diagnosis             */
    /* --- Rx-Buffer Info ------ */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT32                      RxBufferSize;       /* Size of Rx-Buffer */
    LSA_UINT32                      RxBufferOffset;     /* Start of Frame in */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    /* buffer            */
    /* (user-parameter   */
    /*  from DPB !)      */
    /* --- Tx handling --------- */
    LSA_UINT32                      TxBuffCnt;  /* Number of Tx-Resouces get */
    /* from low-level functions  */
    LSA_UINT32                      TxFreeIdx;  /* Idx to pTxInfo for next   */
    /* frame to be send.         */
    LSA_UINT32                      TxCheckIdx; /* Idx to pTxInfo for next   */
    /* frame finished sending    */
    LSA_UINT32                      TxInProgCnt;/* Number of Tx in progress. */
    LSA_UINT32                      TxAcycInProgCnt; /* Number of Acyclic Tx */
    /* in progress.         */
    #ifdef EDDS_CFG_SEND_BLOCKED
    LSA_UINT32                      TxMaxAcylic;/* Max number of Bytes       */
    /* per Cycle for actual speed*/
    /* for NRT/aSRT              */
    #endif
    EDDS_TX_INFO_PTR_TYPE           pTxInfo;    /* Infos about current tx in */
    /* progress. Array with TxCnt*/
    /* entrys                    */
    EDDS_MC_MAC_INFO_PTR_TYPE       pMCInfo;    /* Multicast MAC management  */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT8                       TxToutCnt;  /* number of s for Tx-Timeout*/
    /* (from DPB). 0 if none     */
    EDDS_TIMER_TYPE                 TxTimer;    /* Tx-Timer info             */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    EDDS_TIMER_TYPE					LLTimer;	/* LL-Timer info             */
    /* --hw-independend statistic*/
    EDDS_STAT_TX_TYPE               TxStats;    /* statistics for Tranmit    */
    EDDS_STAT_RX_TYPE               RxStats;    /* statistics for Receive    */
    EDDS_STATISTICS_TYPE            TempStats;  /* temporary storage for LL  */
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDD_GET_STATISTICS_MIB_TYPE*    sMIB;       /* MIB statistics array (for interface and all ports */
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    /* statistics when read      */
    /* -- RQBs for int-requests- */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Tx;      /* Tx-transmission ready */
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Tx_Tout; /* Tx-timeout            */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_LL_Tout; /* LL Timeout/DelayedFunction */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Rx_NRT;  /* NRT-Frame received    */
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Rx_cSRT; /* zycl. SRT Frame recei */
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_PHY;     /* PHY-Interrupt         */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Rx_OverTimer;  /* Rx overload     */
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_Rx_OverEvent;  /* Rx overload     */
    #endif
    #ifdef EDDS_CFG_LED_BLINK_SUPPORT
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_LED_BlinkTimer;/* LED Blink Timer */
    #endif
    #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
    EDDS_INTERNAL_REQUEST_TYPE      RQBInt_DiagTimer;     /* Diag Timer      */
    #endif
    /* ------------------------- */

    /* MaxPortCntOfAllEDD :    Number of all interfaces of all EDDs              */
    /*                         max: EDD_CFG_MAX_INTERFACE_CNT                    */
    LSA_UINT16                          MaxPortCntOfAllEDD;

    /* MaxInterfaceCntOfAllEDD Number of all ports of all EDDs                   */
    /*                         max: EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE           */
    LSA_UINT16                          MaxInterfaceCntOfAllEDD;
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    /* pointer to tx transfer memory (only valid if ZeroCopyInterface is FALSE */
    EDDS_TRANSFER_BUFFER_ARRAY  pTxTransferMem;

    /* traditional zero copy interface is used in EDDS, if this property is set TRUE */
    LSA_BOOL ZeroCopyInterface;
        
    /* time between scheduler calling in ms -> default: 1 */
    LSA_UINT16 schedulerCycle;
    
#endif
} EDDS_DEVICE_GLOB_TYPE;

typedef struct _EDDS_DEVICE_GLOB_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE;

#ifdef LLIF_CFG_SWITCH_SUPPORT
typedef EDDS_DPB_SWI_TYPE EDDS_DDB_SWI_TYPE;
#endif
/*---------------------------------------------------------------------------*/
/* main devicemanagement                                                     */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DDB_TYPE
{
    EDDS_LOCAL_DDB_PTR_TYPE         pNext;      /* Pointer to next device    */
    EDDS_LOCAL_DDB_PTR_TYPE         pPrev;      /* Pointer to previous device*/
    EDDS_HANDLE                     hDDB;       /* Device-handle (currently  */
    EDDS_SYS_HANDLE                 hSysDev;    /* system handle of device   */
    /* pointer) to EDDS_DEVICE_TYP*/
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;      /* Global Device management  */

    #ifdef EDDS_CFG_NRT_INCLUDE
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRT;      /* NRT-specific management   */
    #else
    EDDS_LOCAL_MEM_PTR_TYPE         pNRT;       /* dummy */
    #endif

    #ifdef EDDS_CFG_SRT_INCLUDE
    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE pSRT;      /* SRT-specific management   */
    #else
    EDDS_LOCAL_MEM_PTR_TYPE         pSRT;       /* dummy */
    #endif

#ifdef LLIF_CFG_SWITCH_SUPPORT
    EDDS_DDB_SWI_TYPE				 SWI;		/* switch specific management */
#endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT32                       RxIsrMaxFrameCnt; /* specifies the maximum amount of frames to be */
                                                       /* received before aborting the receive handler */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
} EDDS_DDB_TYPE;


/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

/* Max EDDS Timers needed.                                                   */
/* 1. Tx-Timeout                                                             */
/* 2. Consumer cycle Timer                                                   */
/* 3. Provider cylce Timer                                                   */
/* 4. Rx-Overlocad cycle Timer (if configured)                               */
/* 5. LED-Blink Timer          (if configured)                               */
/* 6. Diag Timer               (if configured)                               */

#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
#define EDDS_OVERLOAD_TIMER_CNT   1
#else
#define EDDS_OVERLOAD_TIMER_CNT   0
#endif

#ifdef EDDS_CFG_LED_BLINK_SUPPORT
#define EDDS_LED_TIMER_CNT        1
#else
#define EDDS_LED_TIMER_CNT        0
#endif

#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
#define EDDS_DROP_TIMER_CNT       1
#else
#define EDDS_DROP_TIMER_CNT       0
#endif

#define EDDS_CFG_MAX_TIMER ((4+EDDS_OVERLOAD_TIMER_CNT+EDDS_LED_TIMER_CNT+EDDS_DROP_TIMER_CNT) * EDDS_CFG_MAX_DEVICES ) /* number of Timers for all */

typedef volatile struct _EDDS_TIMER_INFO_TYPE
{
    LSA_BOOL                        InUse;
    LSA_TIMER_ID_TYPE               TimerID;
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB;
    LSA_VOID                        LSA_FCT_PTR(EDDS_LOCAL_FCT_ATTR, Cbf)
    (EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);
} EDDS_TIMER_INFO_TYPE;

typedef volatile struct _EDDS_TIMER_INFO_TYPE  EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_TIMER_INFO_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* basetype of devicemanagement.                                             */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DDB_INFO_TYPE
{
    EDDS_LOCAL_DDB_PTR_TYPE pBotton;
    EDDS_LOCAL_DDB_PTR_TYPE pTop;

    LSA_UINT32             CntDDB;
} EDDS_DDB_INFO_TYPE;

typedef struct _EDDS_DDB_INFO_TYPE    EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DDB_INFO_PTR_TYPE;


typedef struct _EDDS_INFO_TYPE
{
    EDDS_DDB_INFO_TYPE              DDBInfo;
    EDDS_HDB_INFO_TYPE              HDBInfo;
    EDDS_LOCAL_TIMER_INFO_PTR_TYPE  pTIMInfo;   /* array of TimerInfos */
} EDDS_INFO_TYPE;



/*****************************************************************************/
/*  end of file edds_dev.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_DEV_H */
