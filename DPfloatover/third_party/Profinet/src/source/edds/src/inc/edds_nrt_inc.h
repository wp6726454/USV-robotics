
#ifndef EDDS_NRT_INC_H                        /* ----- reinclude-protection ----- */
#define EDDS_NRT_INC_H

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
/*  F i l e               &F: edds_nrt_inc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
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
/*  23.10.03    JS    added support for DCP and LLDP Filters                 */
/*  11.02.04    JS    added support for multiple send priorities (3)         */
/*  24.02.04    JS    DCP Header format has changed (len/resp-delay)         */
/*  15.06.04    JS    ASRT over NRT always used                              */
/*  24.06.04    JS    moved some defines to edds_dev.h                       */
/*  28.06.04    JS    added parameter State within descriptor. removed inUse */
/*  02.07.04    JS    xRToverUDP support added.                              */
/*  04.10.04    JS    Increased number of DCP-Filters                        */
/*  06.07.05    JS    added multiport support                                */
/*  12.01.06    JS    added MRP NRT filter support                           */
/*  10.10.06    JS    added NRT_TX_PRIO_3 prio                               */
/*  22.11.06    JS    added new NRT-Filters: EDD_NRT_FRAME_IP_ICMP           */
/*                                           EDD_NRT_FRAME_IP_IGMP           */
/*                                           EDD_NRT_FRAME_IP_TCP            */
/*                                           EDD_NRT_FRAME_IP_UDP            */
/*                                           EDD_NRT_FRAME_IP_OTHER          */
/*                                           EDD_NRT_FRAME_ARP               */
/*                                           EDD_NRT_FRAME_RARP              */
/*  27.02.07    JS    added new NRT-Filter : NRT_FILTER_XRT_DCP_HELLO_IDX    */
/*                                           NRT_FILTER_XRT_UDP_DCP_HELLO_IDX*/
/*  07.03.07    JS    added DCP-HELLO Filter structures                      */
/*  22.01.08    JS    added new NRT-Filter : EDD_NRT_FRAME_LEN_STDBY         */
/*  07.05.09    JS    added new NRT-Filter : EDD_NRT_FRAME_LEN_HSR           */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDDS functions uses the following IDs                     */
/*                                                                           */
/*        EDDS:  0x00 .. 0x0F                                                */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*        EIB:   0x10 .. 0x1F                                                */
/*                                                                           */
/*        Each component uses the followin module IDs (defined in component) */
/*                                                                           */
/*        NRT:   0x80 .. 0x8F                                                */
/*        SRT:   0x90 .. 0x9F                                                */
/*        IRT:   0xA0 .. 0xAF                                                */
/*        SWI:   0xB0 .. 0xBF                                                */
/*                                                                           */
/* ========================================================================= */


#define EDDS_MODULE_ID_NRT_USR             0x80
#define EDDS_MODULE_ID_NRT_SND             0x81
#define EDDS_MODULE_ID_NRT_RCV             0x82


/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* LLDP-Header                                                               */
/*                                                                           */
/*+   6   6   2                                                             +*/
/*+ +---+---+----+-----------------                                         +*/
/*+ |DA |SA |Type|                                                          +*/
/*+ |   |   |Len |  LDPDU                                                   +*/
/*+ |   |   |    |                                                          +*/
/*+ +---+---+----+-----------------                                         +*/
/*+                                                                         +*/
/*+ LLDP has no VLAN-Tag on send (but maybe on receive)                     +*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* DCP via xRT-Format                                                        */
/*                                                                           */
/*+  MC-header:                                                             +*/
/*+   6   6   2     2     2     2    1    1    4   2    2                   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+-----+---+---------...+   +*/
/*+ |DA |SA |VLAN|VLAN |Type |Frame|Serv|Serv|xid|resp |len|            |   +*/
/*+ |   |   |TYPE| ID  |LEN  | ID  | ID |type|   |delay|   | TLV-Fields |   +*/
/*+ |   |   |    |     |     |     |    |    |   |     |   |            |   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+-----+---+---------...+   +*/
/*+          \--------/                                  |  \-----v--../    +*/
/*+           optional                                   +--------+         +*/
/*+                                                                         +*/
/*+  UC-header:                                                             +*/
/*+   6   6   2     2     2     2    1    1    4   2    2                   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+-----+---+---------...+   +*/
/*+ |DA |SA |VLAN|VLAN |Type |Frame|Serv|Serv|xid|pad  |len|            |   +*/
/*+ |   |   |TYPE| ID  |LEN  | ID  | ID |type|   |     |   | TLV-Fields |   +*/
/*+ |   |   |    |     |     |     |    |    |   |     |   |            |   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+-----+---+---------...+   +*/
/*+          \--------/                                  |  \-----v--../    +*/
/*+           optional                                   +--------+         +*/
/*+                                                                         +*/
/*+  TLV-Field:                                                             +*/
/*+                                                                         +*/
/*+        2             2                                                  +*/
/*+ +--------------+-----------+--------------..+                           +*/
/*+ |      Type    |           |                |                           +*/
/*+ +---1---+--1---+   Length  |     Value      |                           +*/
/*+ | Option|SubOpt|           |                |                           +*/
/*+ +-------+------+-----------+--------------..+                           +*/
/*+                      |      \-------v----../                            +*/
/*+                      +--------------+                                   +*/
/*+                                                                         +*/
/*+  TLV-Res-Field:                                                         +*/
/*+                                                                         +*/
/*+        2             2          2                                       +*/
/*+ +--------------+-----------+---------+-------...+                       +*/
/*+ |      Type    |           |BlockInfo|          |                       +*/
/*+ +---1---+--1---+   Length  +--------------------+                       +*/
/*+ | Option|SubOpt|           |       Value        |                       +*/
/*+ +-------+------+-----------+---------+--------..+                       +*/
/*+                      |      \-------v-------.../                        +*/
/*+                      +--------------+                                   +*/
/*---------------------------------------------------------------------------*/

/* Offset values without VLAN-Tag: */
/* Important: All LEN-Fields have to be WORD-aligned. If not, access has to be */
/*            changed within coding!                                           */

#define EDDS_FRAME_DCP_FRAME_ID_OFFSET           14

#define EDDS_FRAME_DCP_SERVICE_ID_OFFSET         16
#define EDDS_FRAME_DCP_SERVICE_TYPE_OFFSET       17
#define EDDS_FRAME_DCP_XID_OFFSET                18
#define EDDS_FRAME_DCP_RESP_DELAY_OFFSET         22
#define EDDS_FRAME_DCP_LEN_OFFSET                24
#define EDDS_FRAME_DCP_TLV_OFFSET                26

#define EDDS_FRAME_DCP_MC_HEADER_SIZE            26   /* all fields till TLV */
#define EDDS_FRAME_DCP_UC_HEADER_SIZE            26   /* all fields till TLV */

/* defines for SERVICE_ID:   */

#define EDDS_DCP_SERVICE_ID_IDENTIFY_REQ         5

/* defines for SERVICE_TYPE: */

#define EDDS_DCP_SERVICE_TYPE_MC_REQUEST_MSK     0x3
#define EDDS_DCP_SERVICE_TYPE_MC_REQUEST         0x3

/* TLV offsets (from start of a TLV) */
#define EDDS_DCP_TLV_TYPE_OFFSET                 0
#define EDDS_DCP_TLV_LENGTH_OFFSET               2
#define EDDS_DCP_TLV_TLSIZE                      4
#define EDDS_DCP_TLV_RES_BLOCKINFO_SIZE          2

#define EDDS_DCP_TLV_VALUE_OFFSET                4
#define EDDS_DCP_TLV_VALUE_MAX_SIZE              240

#define EDDS_DCP_TLV_MAX_TLVSIZE                 EDDS_DCP_TLV_TLSIZE + EDDS_DCP_TLV_VALUE_MAX_SIZE


/* NameOfStation-TLV: */
#define EDDS_DCP_DEVICE_PROPERTIES_OPTION        2     /* option    */
#define EDDS_DCP_SUBOPTION_NAME_OF_STATION       2     /* suboption */

/*---------------------------------------------------------------------------*/
/* defines for SNAP-Type Frames                                              */
/*                                                                           */
/*+   6   6   2     1     1     1        5                                  +*/
/*+ +---+---+----+-----+-----+-----+-------------+-------                   +*/
/*+ |DA |SA |LEN |DSAP |SSAP |CTRL |   SNAP ID   | Data                     +*/
/*+ |   |   |    |     |     |     +--3---+---2--+                          +*/
/*+ |   |   |    |     |     |     | OUI  | Type |                          +*/
/*+ +---+---+----+-----+-----+-----+------+------+-------                   +*/
/*+                                                                         +*/
/*+        DSAP    SSAP    CTRL   OUI                    Type               +*/
/*+ DCP:    AAh    AAh      3h    8 0 6 (Siemens)        01FDh              +*/
/*+ Time    AAh    AAh      3h    8 0 6 (Siemens)        0100h              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+ Note that optionally a VLAN-Tag may be present after LEN!!              +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/

#define EDDS_FRAME_SNAP_HEADER_LEN      8   /* SNAP-Header */

/* Values without VLAN */

#define EDDS_FRAME_DSAP_OFFSET          14
#define EDDS_FRAME_SSAP_OFFSET          15
#define EDDS_FRAME_CTRL_OFFSET          16
#define EDDS_FRAME_OUI_OFFSET           17
#define EDDS_FRAME_TYPE_OFFSET          20

#define EDDS_FRAME_DSAP                 0xAA
#define EDDS_FRAME_SSAP                 0xAA
#define EDDS_FRAME_DSAP_DEF             0xFD
#define EDDS_FRAME_SSAP_DEF             0xFD
#define EDDS_FRAME_CTRL                 3

#define EDDS_FRAME_OUI_SIEMENS_1        8
#define EDDS_FRAME_OUI_SIEMENS_2        0
#define EDDS_FRAME_OUI_SIEMENS_3        6

#define EDDS_FRAME_OUI_HIRSCHM_1        0
#define EDDS_FRAME_OUI_HIRSCHM_2        0x80
#define EDDS_FRAME_OUI_HIRSCHM_3        0x63

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_FRAME_STDBY_TYPE           0xFB01
#define EDDS_FRAME_DCP_TYPE             0xFD01
#define EDDS_FRAME_TSYNC_TYPE           0x0001
#define EDDS_FRAME_HSR_TYPE             0x0008
#define EDDS_FRAME_SINEC_FWL_TYPE       0x1001
#else
#define EDDS_FRAME_STDBY_TYPE           0x01FB
#define EDDS_FRAME_DCP_TYPE             0x01FD
#define EDDS_FRAME_TSYNC_TYPE           0x0100
#define EDDS_FRAME_HSR_TYPE             0x0800
#define EDDS_FRAME_SINEC_FWL_TYPE       0x0110
#endif
#define EDDS_FRAME_INVALID_TYPE         0xFFFF

/* for optimized access to framebuffer we use this defines       */
/* The framebuffer has to be DWORD-aligned                       */

#define EDDS_FRAME_SNAP_HEADER_LEN_NO_VLAN              (14+EDDS_FRAME_SNAP_HEADER_LEN)
#define EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_NO_VLAN    7  /* for WORD access  */
#define EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_NO_VLAN    4  /* for DWORD access */
#define EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_NO_VLAN         10 /* for WORD access */

#define EDDS_FRAME_SNAP_HEADER_LEN_VLAN                 (14+EDD_VLAN_TAG_SIZE+EDDS_FRAME_SNAP_HEADER_LEN)
#define EDDS_FRAME_SNAP_DSAP_SSAP_WORD_OFFSET_VLAN       9  /* for WORD access  */
#define EDDS_FRAME_SNAP_CTRL_OUI_DWORD_OFFSET_VLAN       5  /* for DWORD access */
#define EDDS_FRAME_SNAP_TYPE_WORD_OFFSET_VLAN            12 /* for WORD access */

#ifndef EDDS_CFG_BIG_ENDIAN
#define EDDS_FRAME_SNAP_DSAP_SSAP_FILTER ((EDDS_FRAME_SSAP << 8) | EDDS_FRAME_DSAP)
#define EDDS_FRAME_SNAP_DSAP_SSAP_DEF_FILTER ((EDDS_FRAME_SSAP_DEF << 8) | EDDS_FRAME_DSAP_DEF)

#define EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS (( EDDS_FRAME_OUI_SIEMENS_3 << 24 ) | \
( EDDS_FRAME_OUI_SIEMENS_2 << 16 ) | \
( EDDS_FRAME_OUI_SIEMENS_1 << 8 ) | \
( EDDS_FRAME_CTRL ))

#define EDDS_FRAME_SNAP_CTRL_OUI_FILTER_HIRSCHMANN (( EDDS_FRAME_OUI_HIRSCHM_3 << 24 ) | \
( EDDS_FRAME_OUI_HIRSCHM_2 << 16 ) | \
( EDDS_FRAME_OUI_HIRSCHM_1 << 8 ) | \
( EDDS_FRAME_CTRL ))

#else
#define EDDS_FRAME_SNAP_DSAP_SSAP_FILTER ((EDDS_FRAME_SSAP ) | EDDS_FRAME_DSAP << 8)
#define EDDS_FRAME_SNAP_DSAP_SSAP_DEF_FILTER ((EDDS_FRAME_SSAP_DEF ) | EDDS_FRAME_DSAP_DEF << 8)

#define EDDS_FRAME_SNAP_CTRL_OUI_FILTER_SIEMENS (( EDDS_FRAME_OUI_SIEMENS_3     ) | \
( EDDS_FRAME_OUI_SIEMENS_2 << 8 ) | \
( EDDS_FRAME_OUI_SIEMENS_1 << 16 ) | \
( EDDS_FRAME_CTRL         << 24 ))

#define EDDS_FRAME_SNAP_CTRL_OUI_FILTER_HIRSCHMANN (( EDDS_FRAME_OUI_HIRSCHM_3     ) | \
( EDDS_FRAME_OUI_HIRSCHM_2 << 8 ) | \
( EDDS_FRAME_OUI_HIRSCHM_1 << 16 ) | \
( EDDS_FRAME_CTRL         << 24 ))

#endif

#define EDDS_NRT_FRAME_BUFFER_MIN_LENGTH 64       /* min-size for sw-padding */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/* link-structure */

typedef struct _EDDS_NRT_LINK_BASE_TYPE
{
    EDD_UPPER_RQB_PTR_TYPE          pBottom;
    EDD_UPPER_RQB_PTR_TYPE          pTop;
} EDDS_NRT_LINK_BASE_TYPE;

typedef struct _EDDS_NRT_LINK_BASE_TYPE  EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_LINK_BASE_TYPE_PTR;

/* Tx-Prioritys supported */

#define EDDS_NRT_TX_PRIO_0        0  /* prio 0: normal prio. eg. TCP    */
#define EDDS_NRT_TX_PRIO_1        1  /* prio 1: used for ASRT-Low-Prio  */
#define EDDS_NRT_TX_PRIO_2        2  /* prio 2: used for ASRT-High-Prio */
#define EDDS_NRT_TX_PRIO_3        3  /* prio 3: used for ORG -Prio      */

#define EDDS_NRT_TX_PRIO_CNT (EDDS_NRT_TX_PRIO_3+1)

/* The following index is for the FilterInfo array access. */

#define EDDS_NRT_FILTER_ARP_IDX                0
#define EDDS_NRT_FILTER_LEN_DCP_IDX            1
#define EDDS_NRT_FILTER_LEN_TSYNC_IDX          2
#define EDDS_NRT_FILTER_LEN_OTHER_IDX          3
#define EDDS_NRT_FILTER_OTHER_IDX              4
#define EDDS_NRT_FILTER_XRT_DCP_IDX            5
#define EDDS_NRT_FILTER_OTHER_LLDP_IDX         6
#define EDDS_NRT_FILTER_XRT_ASRT_IDX           7
#define EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX       8
#define EDDS_NRT_FILTER_XRT_UDP_DCP_IDX        9
#define EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX       10
#define EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX       11
#define EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX      12
#define EDDS_NRT_FILTER_OTHER_MRP_IDX          13
#define EDDS_NRT_FILTER_IP_ICMP_IDX            14
#define EDDS_NRT_FILTER_IP_IGMP_IDX            15
#define EDDS_NRT_FILTER_IP_TCP_IDX             16
#define EDDS_NRT_FILTER_IP_UDP_IDX             17
#define EDDS_NRT_FILTER_IP_OTHER_IDX           18
#define EDDS_NRT_FILTER_RARP_IDX               19
#define EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX      20
#define EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX  21
#define EDDS_NRT_FILTER_LEN_STDBY_IDX          22
#define EDDS_NRT_FILTER_LEN_HSR_IDX            23
#define EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX      24

#define EDDS_NRT_MAX_FILTER_IDX                24      /* we have currently 25 filters     */

#define EDDS_NRT_FILTER_NOT_USED     0xff


#define EDDS_IS_BC_MAC(SrcMAC)      \
        ( (SrcMAC[0] & SrcMAC[1] & SrcMAC[2] & SrcMAC[3] & SrcMAC[4] & SrcMAC[5]) == 0xFF )

#define EDDS_IS_MC_MAC(SrcMAC)  ( (SrcMAC[0] & 0x01) )

#define EDDS_UDP_FILTER_DST_PORT_DHCP_CLIENT            68
#define EDDS_UDP_FILTER_DST_PORT_NTP_SNTP               123
#define EDDS_UDP_FILTER_DST_PORT_SNMP                   161
#define EDDS_UDP_FILTER_DST_PORT_SNMPTRAP               162
#define EDDS_UDP_FILTER_DST_PORT_PNIO_EPM               0x8894
#define EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_BEGIN   0xC000
#define EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_END     0xFFFF

/* The following are defines for the second-stage frame buffers */
/* we currently have 4 second-stage buffers                     */

#define EDDS_NRT_BUFFER_ASRT_ALARM_IDX     0
#define EDDS_NRT_BUFFER_IP_IDX             1
#define EDDS_NRT_BUFFER_ASRT_OTHER_IDX     2
#define EDDS_NRT_BUFFER_OTHER_IDX          3
#define EDDS_NRT_BUFFER_MAX_IDX            4

#define EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_ALARM   4
#define EDDS_NRT_RX_DSCR_MAX_LOOKUP_ARPIP        3
#define EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_OTHER   3
#define EDDS_NRT_RX_DSCR_MAX_LOOKUP_OTHER        3
#define EDDS_NRT_RX_DSCR_MAX_LOOKUP              ( EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_ALARM + EDDS_NRT_RX_DSCR_MAX_LOOKUP_ARPIP + EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_OTHER + EDDS_NRT_RX_DSCR_MAX_LOOKUP_OTHER )

/*===========================================================================*/
/* NRT handle (channel) management-structures                                */
/*===========================================================================*/

typedef struct _EDDS_HDB_COMP_NRT_TYPE
{
    LSA_BOOL            CancelInProgress;   /* Cancel for this channel in progr. */
    /* set if tx-cancel is underway.     */
    LSA_UINT32          RxQueueIdx;     /* Receive queue idx used by this channel*/
    /* NRT_FILTER_xx_IDX etc.                */
    LSA_UINT32          RxQueueReqCnt;  /* Number of Rx-Requests in              */
    /* Queue.(used in RQB confirmation )     */
    /* dont checked for wrap-around!         */
    LSA_UINT32          TxQueueReqCnt;  /* Number of transmits pending but not   */
    /* not in progress.                      */
    LSA_UINT32          TxInProgCnt;    /* Number of transmits in progress.      */
    EDDS_NRT_LINK_BASE_TYPE  TxCancel;       /* Tx to be canceled queue               */
    EDDS_INTERNAL_REQUEST_TYPE    CancelRQB;        /* Internal CANCEL-RQB       */
    EDD_UPPER_RQB_PTR_TYPE        pUserCancelRQB;   /* User     CANCEL-RQB ptr   */

} EDDS_HDB_COMP_NRT_TYPE;

typedef struct _EDDS_HDB_COMP_NRT_TYPE    EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_HDB_COMP_NRT_PTR_TYPE;

/*===========================================================================*/
/* NRT Device management-structures                                          */
/*===========================================================================*/

/* statistics */

typedef struct _EDDS_NRT_STAT_STRUCT
{
    /* number of discarded frames in second stage buffers */
    LSA_UINT32    RxSilentDiscarded[EDDS_NRT_BUFFER_MAX_IDX];
    LSA_UINT32    RxARPIP_Drop;         /* number of ARPIP-Frames dropped (no rqb)       */
    LSA_UINT32    RxLenDCP_Drop;        /* number of DCP-LEN   -Frames dropped (no rqb)  */
    LSA_UINT32    RxLenTSYNC_Drop;      /* number of TSYNC-LEN -Frames dropped (no rqb)  */
    LSA_UINT32    RxLenOther_Drop;      /* number of Type-Other -Frames dropped (no rqb) */
    LSA_UINT32    RxXRTDCP_Drop;        /* number of DCP-XRT   -Frames dropped (no rqb)  */
    LSA_UINT32    RxLLDP_Drop;          /* number of LLDP-Other -Frames dropped (no rqb) */
    LSA_UINT32    RxOther_Drop;         /* number of Other-Frames dropped (no rqb )      */
    LSA_UINT32    RxASRTAlarm_Drop;     /* number of ASRT-Frames dropped (no rqb )       */
    LSA_UINT32    RxUDP_ASRTAlarm_Drop; /* number of UDP ASRT-Frames dropped (no rqb )   */
    LSA_UINT32    RxUDP_XRTDCP_Drop;    /* number of UDP DCP XRT-Frames dropped (no rqb )*/
    LSA_UINT32    RxPTP_Drop;           /* number of PTP -Frames dropped (no rqb )       */
    LSA_UINT32    RxMRP_Drop;           /* number of MRP -Frames dropped (no rqb )       */
    LSA_UINT32    RxDCPFilterDrop;      /* number of DCP-Other-Frames filter-drop.       */
    LSA_UINT32    RxDCPFilterInvalidFrame;  /* invalid DCP-IDENTIFY-Frame (length)       */
    LSA_UINT32    RxARPFilterDrop;      /* number of ARP-Frames filter-drop.             */
} EDDS_NRT_STAT_TYPE;


typedef struct _EDDS_NRT_RX_FILTER_MGM_TYPE
{
    LSA_BOOL                        InUse;              /* TRUE if in use       */
    LSA_UINT32                      FrameFilter;        /* FrameFilter          */
    struct _EDDS_HDB_TYPE  EDDS_LOCAL_MEM_ATTR * pHDB;  /* HDB-Pointer          */
} EDDS_NRT_RX_FILTER_MGM_TYPE;


typedef struct _EDDS_NRT_RX_FILTER_MGM_TYPE  EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_RX_FILTER_MGM_TYPE_PTR;


typedef struct _EDDS_NRT_RX_DSCR_TYPE
{
    LSA_UINT32                      State;          /* State of descriptor       */
    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer;        /* recv-buffer               */
    LSA_UINT32                      Length;         /* Framelength in buffer.    */
    /* if InUse is TRUE          */
    LSA_UINT32                      PortID;         /* PortID                    */
} EDDS_NRT_RX_DSCR_TYPE;

/* State */
#define EDDS_NRT_DSCR_STATE_FREE       0   /* if not free it is in use           */
#define EDDS_NRT_DSCR_STATE_IN_USE     1   /* general in use. unspezified        */

/* in some cases we can differ the content because it was already analysed in    */
/* ISR. So we set the state with a value spezifing the content.                  */

/* for ASRT-Frames */
#define EDDS_NRT_DSCR_STATE_IN_USE_XRT       1
#define EDDS_NRT_DSCR_STATE_IN_USE_UDP       2

/* for OTHER-Frames */
#define EDDS_NRT_DSCR_STATE_IN_USE_OTHER_LEN 1
#define EDDS_NRT_DSCR_STATE_IN_USE_OTHER_TYPE 2

typedef struct _EDDS_NRT_RX_DSCR_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_DSCR_TYPE_PTR;


typedef struct _EDDS_NRT_RX_DSCR_MGM_TYPE
{
    LSA_UINT32                  RxDscrCnt;     /* Number of Receive-Dscr    */
    LSA_UINT32                  RxFillCnt;     /* next Rx-Descr to fill  .  */
    /* 0..RxBuffCnt-1            */
    LSA_UINT32                  RxCheckCnt;    /* next Rx-Descr to check..  */
    /* 0..RxBuffCnt-1            */
    EDDS_NRT_DSCR_TYPE_PTR      pRxDscrStart;  /* Pointer to first dscr     */
    /* (array of RxDscrCnt entry)*/
    /* will be allocated.        */
    LSA_UINT32                  RxDscrLookUpCnt;
    LSA_UINT32                  RxDscrLookUpCntMax;
} EDDS_NRT_RX_DSCR_MGM_TYPE;

typedef struct _EDDS_NRT_RX_DSCR_MGM_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_DSCR_MGM_TYPE_PTR;

/* -------------------------------------------------------------------------*/
/* for DCP-Filtering of xRT-DCP-Frames (Identify)                           */
/* -------------------------------------------------------------------------*/
typedef struct _EDDS_NRT_DCP_FILTER_MGM_TYPE
{
	LSA_BOOL                    InUse;          /* is filter used           */
	LSA_HANDLE_TYPE             Handle;         /* handle of channel        */
	EDD_UPPER_MEM_U8_PTR_TYPE   pDCP_TLVFilter; /* if <> LSA_NULL:          */
	LSA_UINT8                   DCP_TLVFilter[EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN];
    /* Filter for xRT-DCP-Frames*/
    /* (for EDD_NRT_FRAME_DCP)  */
    LSA_UINT16                  Type;           /* Type of TLV-Field        */
    /* Network-Format!          */
    LSA_UINT16                  ValueLength;    /* Length of Value-Field    */
    /* Network-Format!          */
} EDDS_NRT_DCP_FILTER_MGM_TYPE;

#define EDDS_NRT_DCP_FILTER_CNT  (EDDS_MAX_PORT_CNT+1)

/* -------------------------------------------------------------------------*/
/* for DCP-Filtering of xRT-DCP-Frames (Hello)                              */
/* -------------------------------------------------------------------------*/
typedef struct _EDDS_NRT_DCP_HELLO_FILTER_MGM_TYPE
{
    LSA_BOOL                    InUse;          /* Filter in Use?           */
    LSA_HANDLE_TYPE             Handle;         /* Handle of channel        */
    LSA_UINT32                  UserID;         /* UserID if in use         */
    LSA_UINT8                   StationName[EDD_DCP_MAX_DCP_HELLO_NAME_OF_STATION];
    LSA_UINT32                  StationNameLen; /* Length of StationName    */
} EDDS_NRT_DCP_HELLO_FILTER_MGM_TYPE;

typedef struct _EDDS_NRT_DCP_HELLO_FILTER_MGM_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_DCP_HELLO_FILTER_MGM_PTR;

typedef struct _EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_TYPE
{
    EDDS_NRT_DCP_HELLO_FILTER_MGM_TYPE   Filter[EDD_DCP_MAX_DCP_HELLO_FILTER];
} EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_TYPE;

typedef struct _EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_PTR;

/* -------------------------------------------------------------------------*/
/* for ARP-Filtering                                                        */
/* -------------------------------------------------------------------------*/
typedef struct _EDDS_ARP_FRAME_FILTER_TYPE
{
    LSA_UINT8                   FilterCnt;
    EDD_IP_ADR_TYPE             IPAddr[EDD_MAX_ARP_FILTER];
} EDDS_ARP_FRAME_FILTER_TYPE;

typedef struct _EDDS_ARP_FRAME_FILTER_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_ARP_FRAME_FILTER_PTR;

typedef struct _EDDS_NRT_ARP_FILTER_MGM_TYPE
{
    LSA_BOOL                    InUse;          /* Filters in Use?          */
    LSA_HANDLE_TYPE             Handle;         /* Handle of channel        */
    EDDS_ARP_FRAME_FILTER_TYPE  ARPFilter;      /* Arp filter               */
} EDDS_NRT_ARP_FILTER_MGM_TYPE;

typedef struct _EDDS_NRT_ARP_FILTER_MGM_TYPE    EDDS_LOCAL_MEM_ATTR *   EDDS_NRT_ARP_FILTER_MGM_PTR;

/* -------------------------------------------------------------------------*/
/* managing tx-requests                                                     */
/* -------------------------------------------------------------------------*/

typedef struct _EDDS_NRT_TX_MGM_TYPE
{
    EDDS_NRT_LINK_BASE_TYPE     Req;            /* Tx-Requestqueue          */
    LSA_UINT32                  InProgCnt;      /* Number of Tx in progress */
    /* Rx IP-Request queues     */
    LSA_UINT32                  InProgCntMax;   /* Max Number of Tx         */
    /* allowed to be in progress*/
    /* (constant, init on start)*/
} EDDS_NRT_TX_MGM_TYPE;


/*---------------------------------------------------------------------------*/
/* NRT- Device Managament structure                                          */
/*                                                                           */
/* For Transmit:                                                             */
/*                                                                           */
/*  We support multiple priorites for sending. So we have a request queue    */
/*  for each priority. TxReq holds all requests pending but not serviced     */
/*  yet. TxInProg holds all requests which are in progress (e.g. put to      */
/*  send). When a request will be sended, it is unqueued from TxReq and      */
/*  queued in TxInProg. After transmit is finished the request is unqueued   */
/*  from TxInProg and inidicated to the user.                                */
/*                                                                           */
/* For Receive:                                                              */
/*                                                                           */
/*  We currently have several filters. For each we have a receive queue.     */
/*  Every filter can only be used by one handle (channel) but a channel can  */
/*  use more then one filter (can be ored). So a channel can use all filters.*/
/*  In this case we actually have only one receive-queue (but for alle types */
/*  of frames). To manage this, we have the FilterInfo array which holds     */
/*  for each filter (frametype) the corresponding handle (channel). The      */
/*  used receive queue within the pRxReq array is stored in the handle-mgnt. */
/*  (RxQueueIdx).                                                            */
/*  So when a new request for a handle arrives we look into FilterInfo for   */
/*  the handle to got the idx of the queue. Every handle has one queue but   */
/*  can be present multiple times within FilterInfo.                         */
/*                                                                           */
/*                                                                           */
/*  Example: handle 0 uses Filter 0 (IP) and 2 (others)                      */
/*           handle 1 uses Filter 1                                          */
/*                                                                           */
/*  FilterInfo and RxQueueIdx (in Handle-management) looks like this:        */
/*                                                                           */
/*  InUse   Handle    RxQueueIdx (in NRT-Handel-management)                  */
/*  LSA_TRUE  0          0                                                   */
/*  LSA_TRUE  1          1                                                   */
/*  LSA_TRUE  0          0                                                   */
/*                                                                           */
/*  We only need two Rx-queues. Handle 0 uses Queue 0 and handle 1 uses      */
/*  queue 1. Queue 2 is not used. Note that handle 0 uses the same queue     */
/*  for both filter!                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  Rxdescriptors:                                                           */
/*                                                                           */
/*  For NRT we use an separate receive-buffer/descriptor queue beside the    */
/*  one used by the ethernetcontroller. This is done to be able to remove    */
/*  the received frame from the controller as fast as possible, so we dont   */
/*  drop high priority frames (SRT). NRT frames may be dropped.              */
/*                                                                           */
/*  Wen a frame arrives we queue it within the RxDscrStart array and will    */
/*  handle it later. If no entry is free we drop the frame in int-handler.   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DDB_COMP_NRT_TYPE
{
    LSA_UINT32                     HandleCnt;   /* Number of handles using  */
    /* NRT.                     */
    /* Tx management for mult.  */
    /* priorities.              */
    EDDS_NRT_TX_MGM_TYPE           Tx[EDDS_NRT_TX_PRIO_CNT];
    EDDS_NRT_LINK_BASE_TYPE        TxInProg;    /* Tx in progress queue     */
    /* Rx IP-Request queues     */
    EDDS_NRT_LINK_BASE_TYPE        RxReq[EDDS_NRT_MAX_FILTER_IDX+1];
    /* Filter infos             */
    EDDS_NRT_RX_FILTER_MGM_TYPE         FilterInfo[EDDS_NRT_MAX_FILTER_IDX+1];

    /* maximum rx dscr look up count */
    LSA_UINT32                     RxDscrMaxLookUpCnt;
    LSA_UINT32                     RxQueueLookUpCnt;
    EDDS_NRT_RX_DSCR_MGM_TYPE      RxDscrMgm[EDDS_NRT_BUFFER_MAX_IDX]; /* alarm, ip, alarm other, other */

    EDDS_NRT_DCP_FILTER_MGM_TYPE   DCPFilter[EDDS_NRT_DCP_FILTER_CNT];
    /* Optional DCP-Filters for */
    /* (for EDD_NRT_FRAME_DCP)  */
    EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_PTR   pDCPHelloFilter;
    /* DCP-Hello Filters for          */
    /* (for EDD_NRT_FRAME_DCP_HELLO)  */

    EDDS_NRT_ARP_FILTER_MGM_TYPE        ARPFilter;   /* Filtering of ARP frames  */
    /* (for EDD_NRT_FRAME_ARP)  */

    EDDS_NRT_STAT_TYPE                  Stats;       /* some statistics          */

    LSA_BOOL                       RxFilterUDP_Unicast;
    LSA_BOOL                       RxFilterUDP_Broadcast;

} EDDS_DDB_COMP_NRT_TYPE;

typedef struct _EDDS_DDB_COMP_NRT_TYPE    EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/



/*****************************************************************************/
/*  end of file nrt_inc.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NRT_INC_H */
