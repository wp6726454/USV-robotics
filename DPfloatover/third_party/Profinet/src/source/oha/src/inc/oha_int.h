
#ifndef OHA_INT_H                        /* ----- reinclude-protection ----- */
#define OHA_INT_H

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
/*  F i l e               &F: oha_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  OHA.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    JS    initial version.                                       */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*                                                                           */
/*  Check plausibility                                                       */
/*                                                                           */
/*****************************************************************************/

#ifdef OHA_CFG_USE_RQB_PRIORITY
# error "OHA_CFG_USE_RQB_PRIORITY is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_DEBUG_TRACE_COMPILE_LEVEL
# error "OHA_CFG_DEBUG_TRACE_COMPILE_LEVEL is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_DEBUG_RQB
# error "OHA_CFG_DEBUG_RQB is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_LOCAL_MEM_STATIC
# error "OHA_CFG_LOCAL_MEM_STATIC is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_LOCAL_MEM_DYNAMIC
# error "OHA_CFG_LOCAL_MEM_DYNAMIC is defined. Please remove the define from oha_cfg.h"
#endif

/*---------------------------------------------------------------------------*/
/* Settings for DCP-Server                                                   */
/*---------------------------------------------------------------------------*/

#ifdef OHA_CFG_NO_DCP_OPTION_PBC
# error "OHA_CFG_NO_DCP_OPTION_PBC obsolete. DCP-Server support of PBC-Database (OrangeBox) was removed. please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_NO_DCP_OPTION_PB
# error "OHA_CFG_NO_DCP_OPTION_PB obsolete. DCP-Server support of Profibus-Options was removed. please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_DCP_OPTION_PB
# error "OHA_CFG_DCP_OPTION_PB is defined. DCP-Server support of former Profibus-Options was removed. please remove the define from oha_cfg.h"
#endif

/*---------------------------------------------------------------------------*/
/* Settings for LLDP                                                         */
/*---------------------------------------------------------------------------*/

#ifdef OHA_LLDP_TX_INTERVAL
# error "OHA_LLDP_TX_INTERVAL is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_LLDP_SEND_OPTIONALS_4_TO_7
# error "OHA_LLDP_SEND_OPTIONALS_4_TO_7 is defined. Please remove the define from oha_cfg.h"
#endif

/* System Capabilities (LLDP-Type 7) */

#ifdef OHA_LLDP_SYSTEM_CAPABILITIES
# error "OHA_LLDP_SYSTEM_CAPABILITIES is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_LLDP_ENABLED_CAPABILITIES
# error "OHA_LLDP_ENABLED_CAPABILITIES is defined. Please remove the define from oha_cfg.h"
#endif

/*---------------------------------------------------------------------------*/
/* Settings for SNMP/SOCK                                                    */
/*---------------------------------------------------------------------------*/

#ifdef OHA_CFG_NO_MASTER_AGENT_EXISTING
# error "OHA_CFG_NO_MASTER_AGENT_EXISTING is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_CFG_NO_SOCK
# error "OHA_CFG_NO_SOCK is defined. Please remove the define from oha_cfg.h"
#endif

/*------------------------------------------------------------------------------
// stringify
//----------------------------------------------------------------------------*/

#define OHA_STRINGIFY_(n)  # n
#define OHA_STRINGIFY(n)   OHA_STRINGIFY_(n) /* make it a string */

/*------------------------------------------------------------------------------
// pragma message
// intended usage: #pragma OHA_MESSAGE(...)
//----------------------------------------------------------------------------*/

#ifdef _MSC_VER /* Microsoft, pragma-args are expanded */
# ifdef OHA_MESSAGE
#  error "who defined OHA_MESSAGE?"
# endif /* OHA_MESSAGE */
# ifdef OHA_MESSAGE_OFF /* when compiling the variants, we want to see the warnings */
#  undef OHA_MESSAGE
# else
#  define OHA_MESSAGE(str) message( __FILE__ "(" OHA_STRINGIFY(__LINE__) ") : " str)
# endif /* OHA_MESSAGE */
#else
# undef OHA_MESSAGE
#endif

/*----------------------------------------------------------------------------*/
/* pointer compare macros                                                     */
/*----------------------------------------------------------------------------*/

#define OHA_TEST_POINTER(ptr2_)     (ptr2_)

#define OHA_IS_NULL(ptr_)           (LSA_HOST_PTR_ARE_EQUAL(OHA_TEST_POINTER(ptr_), LSA_NULL))
#define OHA_IS_NOT_NULL(ptr_)       (! LSA_HOST_PTR_ARE_EQUAL(OHA_TEST_POINTER(ptr_), LSA_NULL))
#define OHA_ARE_EQUAL(ptr1_, ptr2_) (LSA_HOST_PTR_ARE_EQUAL(OHA_TEST_POINTER(ptr1_), OHA_TEST_POINTER(ptr2_)))

/*----------------------------------------------------------------------------*/
/* read, write MIB objects                                                    */
/*----------------------------------------------------------------------------*/

#define OHA_DB_SET_MIB_VALUE(_poid, _val, _len, _rema)					 {	\
									(_poid)->Value = (_val);				\
									(_poid)->Length = ((LSA_UINT8)(_len));  \
									(_poid)->Remanent = ((LSA_UINT8)(_rema)); } 

#define OHA_DB_SET_MIB_ACCESS(_poid, _access)					  {	\
									(_poid)->AccCtrl = (_access); } 

#define OHA_DB_SET_MIB_OBJECT(_poid, _val, _len, _access)				 {	\
									(_poid)->Value = (_val);				\
									(_poid)->Length = ((LSA_UINT8)(_len));	\
									(_poid)->AccCtrl = (_access); } 


#define OHA_DB_GET_MIB_VALUE(_poid, _val, _len, _rema)					 {	\
									(_val) = (_poid)->Value;				\
									(_len) = (_poid)->Length;				\
									(_rema) = (_poid)->Remanent;  } 

#define OHA_DB_GET_MIB_ACCESS(_poid, _access)					   {	\
									(_access) = (_poid)->AccCtrl; } 

#define OHA_DB_GET_MIB_OBJECT(_poid, _val, _len, _access)				 {	\
									(_val) = (_poid)->Value;				\
									(_len) = (_poid)->Length;				\
									(_access) = (_poid)->AccCtrl; } 

/*-----------------------------------------------------------------------------
// database helper functions
//---------------------------------------------------------------------------*/

#include "oha_db.h"

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/* ========================================================================= */

/* OHA module IDs used in C-Files */

#define OHA_MODULE_ID_OHA_USR              0x1
#define OHA_MODULE_ID_OHA_LIB              0x2
#define OHA_MODULE_ID_OHA_SYS              0x3
#define OHA_MODULE_ID_OHA_SOCK             0x4
#define OHA_MODULE_ID_OHA_EDD              0x5
#define OHA_MODULE_ID_OHA_DCPU             0x6
#define OHA_MODULE_ID_OHA_DCP              0x7
#define OHA_MODULE_ID_OHA_NARE             0x8
#define OHA_MODULE_ID_OHA_LLDP             0x9
#define OHA_MODULE_ID_OHA_DCP1             0xA
#define OHA_MODULE_ID_OHA_DCP2             0xB
#define OHA_MODULE_ID_OHA_DB               0xC
#define OHA_MODULE_ID_OHA_LLDU             0xD
#define OHA_MODULE_ID_OHA_AGNT             0xE
#define OHA_MODULE_ID_OHA_SOCU             0xF

#define OHA_MODULE_ID_OHA_CFG              0x10
#define OHA_MODULE_ID_OHA_CFG_DB           0x11
#define OHA_MODULE_ID_OHA_TOPO             0x12
#define OHA_MODULE_ID_OHA_SNMX             0x13
#define OHA_MODULE_ID_OHA_TIM              0x14

#define OHA_MODULE_ID_OHA_ASN1             0x20
#define OHA_MODULE_ID_OHA_SNMP             0x21
#define OHA_MODULE_ID_OHA_MIBS             0x22
#define OHA_MODULE_ID_OHA_DB_L             0x23

#define OHA_MODULE_ID_OHA_PRM              0x24
#define OHA_MODULE_ID_OHA_REMA             0x25

#define OHA_MODULE_ID_OHA_MRP              0x30
#define OHA_MODULE_ID_OHA_MRPR             0x31


/*===========================================================================*/
/*                             IEEE-802.3 defines                            */
/*===========================================================================*/

/*
 * see OSF sources IPNAF.H line 90
 *
 * The max # of data bytes that can go into a UDP packet body such that
 * the resulting IP packet can fit through any of the local network
 * interfaces without inducing IP fragmentation.
 *
 * NOTE WELL:  This value is derived from
 *
 *      (1) The size of the data section of data link packets.  For the
 *          time being, the data link is assumed to be ethernet.
 *
 *      (2) The size of the LLC frame.  RFC 1042, which specifies IP
 *          over 802 networks, calls for the use of the SNAP protocol.
 *          SNAP takes up 8 bytes of the ethernet frame's data section.
 *
 *      (3) The size of the UDP and IP headers, from RFCs 768 and 791.
 *
 *      (4) The length of the IP options part of the header.  Since we
 *          do not currently use any of the IP options, this value is
 *          0.   *** This constant must be modified if we ever make use
 *          of IP options in the future. ***
 *
 *
 *  #define RPC_C_ETHER_MAX_DATA_SIZE 1500
 *  #define RPC_C_IP_LLC_SIZE            8 - LLC frame for SNAP protocol
 *  #define RPC_C_IP_HDR_SIZE           20 - Base IP header
 *  #define RPC_C_IP_OPTS_SIZE           0 - IP options length
 *  #define RPC_C_UDP_HDR_SIZE           8 - UDP header
 *
 *  #define RPC_C_IP_UDP_MAX_LOC_UNFRG_TPDU ( RPC_C_ETHER_MAX_DATA_SIZE - \
 *          (RPC_C_IP_LLC_SIZE + RPC_C_IP_HDR_SIZE + RPC_C_IP_OPTS_SIZE + RPC_C_UDP_HDR_SIZE) \
 *          )
 */

#define OHA_UDP_BUFFER_SIZE      1500

/* Time To Live - handling for send operation:
The TTL-TLV is part of the TLV-chain, which is to supply in case of a LLDP-"Set" by the user.
Furthermore the send-interval, a separate parameter, is to provide by a "Set" too.
That means, that the user must take care of a "harmonizing" TTL-TLV and send-interval.
/4/ recommends default values:
For the send-interval:  5 seconds
For the TTL-value:    4 * send-interval (TTL is maximally 65535, see TTL-TLV) */

#define OHA_LLDP_TTL_FACTOR          4

#ifdef OHA_CFG_MAX_SENDER
#  error "OHA_CFG_MAX_SENDER is defined. Please remove the define from oha_cfg.h"
#endif

#ifdef OHA_LLDP_TX_INTERVAL
#  error "OHA_LLDP_TX_INTERVAL is defined. Please remove the define from oha_cfg.h"
#endif

#define OHA_LLDP_NOTIFICATION_INTERVAL         5

/*  TimeToLive = min(65535, (TxInterval * TxHoldMultiplier))
 *
 *               TxInterval: SYNTAX Integer32 (5..32768 seconds)
 *                           DEFVAL { 30 }
 *               TxHoldMultiplier: SYNTAX Integer32 (2..10 seconds)
 *               DEFVAL { 4 }
 */

#define OHA_LLDP_TX_INTERVAL_MIN             5
#define OHA_LLDP_TX_INTERVAL_MAX         32768

#define OHA_LLDP_TTL_FACTOR_MIN              2
#define OHA_LLDP_TTL_FACTOR_MAX             10

/* Additional hello delay by LinkUp in 10ms (20ms default):                 */
/* OHA does a HelloDelay(LinkUp) := "AddDelay" + HelloDelay from the record */

#ifndef OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP
#define OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP   2
#endif

/*===========================================================================*/
/*                   some internal configuration settings                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef OHA_IP_TYPE  OHA_LOCAL_MEM_ATTR *    OHA_LOCAL_MEM_IP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* This structure is  used for internal request initiated by timeout-handler.*/
/* InUse is set if the RQB is underway (from timeout to LLDP_system(). if    */
/* it arrives OHA_system() InUse is reset to LSA_FALSE. So only one RQB is   */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef struct oha_internal_request_tag
{
    LSA_BOOL                   InUse;          /* LSA_TRUE if in rqb-queue  */
    OHA_UPPER_RQB_PTR_TYPE     pRQB;
} OHA_INTERNAL_REQUEST_TYPE;

typedef volatile OHA_INTERNAL_REQUEST_TYPE  OHA_LOCAL_MEM_ATTR * OHA_INTERNAL_REQUEST_PTR_TYPE;


/* statistics to read/write from/to LLDP */

typedef struct oha_stats_info_tag
{
	LSA_UINT32				GlobOptions;
	LSA_UINT32				IfOptions[OHA_CFG_MAX_NICS];
	LSA_UINT32				PortOptions[OHA_CFG_MAX_NICS][OHA_CFG_MAX_PORTS];
} OHA_STATS_INFO_TYPE;

typedef OHA_STATS_INFO_TYPE OHA_LOCAL_MEM_ATTR * OHA_STATS_INFO_PTR_TYPE;

/* SNMP-GET port options (lldpStatistics, see LLDP.MIB): */
#define OHA_CTRL_TX_FRAMESTOTAL			 (LSA_UINT32) 0x00000001 /* lldpStatistics "lldpStatsTxPortFramesTotal" */
#define OHA_CTRL_RX_FRAMESDISCARDEDTOTAL (LSA_UINT32) 0x00000002 /* lldpStatistics "lldpStatsRxPortFramesDiscardedTotal" */
#define OHA_CTRL_RX_PORTFRAMESERRORS	 (LSA_UINT32) 0x00000004 /* lldpStatistics "lldpStatsRxPortFramesErrors" */
#define OHA_CTRL_RX_PORTFRAMESTOTAL		 (LSA_UINT32) 0x00000008 /* lldpStatistics "lldpStatsRxPortFramesTotal" */
#define OHA_CTRL_RX_PORTAGEOUTSTOTAL	 (LSA_UINT32) 0x00000010 /* lldpStatistics "lldpStatsRxPortAgeoutsTotal" */

/* a SNMP-SET (globals) 'TxInterval', 'ReinitDelay' and 'TxDelay' leads to a LLDP_SET_GLOBALS */
#define OHA_CTRL_LLDP_SET_GLOBALS (OHA_CTRL_TX_INTERVAL | OHA_CTRL_REINIT_DELAY | OHA_CTRL_TX_DELAY | OHA_CTRL_TX_HOLD_MULTIPLIER)

/* a SNMP-SET (ports) 'TxEnable's' leads to a LLDP_SET */
#define OHA_CTRL_LLDP_TX_PORTS (OHA_CTRL_LLDP_TLVS | OHA_CTRL_LLDP_MAN_ADDR | OHA_CTRL_PNO_DELAY | OHA_CTRL_PNO_PORT_STATUS | \
                                OHA_CTRL_PNO_ALIAS | OHA_CTRL_PNO_MRP | OHA_CTRL_PNO_PTCP | OHA_CTRL_DOT3_TLVS |              \
                                OHA_CTRL_DOT1_VLAN_ID | OHA_CTRL_DOT1_VLAN_NAME | OHA_CTRL_DOT1_PROTO_VLAN | OHA_CTRL_DOT1_PROTOCOL)

/* statis to read/write from/to MRP */

 /* AP01479100: Wrong indexing of 2 dimensional array */
typedef LSA_HANDLE_TYPE OHA_MRP_STATS_INFO_TYPE[OHA_CFG_MAX_NICS][EDD_CFG_MAX_MRP_INSTANCE_CNT];

typedef OHA_MRP_STATS_INFO_TYPE OHA_LOCAL_MEM_ATTR * OHA_MRP_STATS_INFO_PTR_TYPE;

/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

/* AP01539318: linker error using C++ compiler (fkt. ptr as parameter) */
typedef LSA_VOID LSA_FCT_PTR(OHA_LOCAL_FCT_ATTR, OHA_LOCAL_CALLBACK_FCT_PTR_TYPE)(OHA_INTERNAL_REQUEST_PTR_TYPE pIntRQB);

typedef struct oha_timer_info_tag
{
    LSA_BOOL                       InUse;
    LSA_UINT16                     TimerID;
    OHA_INTERNAL_REQUEST_TYPE      IntRQB;
    OHA_LOCAL_CALLBACK_FCT_PTR_TYPE Cbf;
} OHA_TIMER_INFO_TYPE;

typedef struct oha_timer_info_tag   OHA_LOCAL_MEM_ATTR *    OHA_TIMER_INFO_PTR_TYPE;

#define OHA_TIMER_ID_INVALID   0xFFFF

/*===========================================================================*/
/* REMA record management                                                    */
/*===========================================================================*/

typedef struct oha_rema_header_tag
{
    LSA_UINT32 index;   /* "T" */
    LSA_UINT32 api;          /* application process identifier */
    LSA_UINT16 slot_number;
    LSA_UINT16 subslot_number;
    LSA_UINT32 record_data_length;     /* user data in octets */
    LSA_UINT16 comp_id;
    LSA_UINT16 reserved;
} OHA_REMA_HEADER_TYPE;

typedef struct oha_rema_record_tag
{
    OHA_REMA_HEADER_TYPE record_header;
    LSA_UINT8  record_data[1];
} OHA_REMA_RECORD_TYPE;

/*===========================================================================*/
/*                   TOPO database                                           */
/*===========================================================================*/

typedef struct oha_peer_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_PEER_PTR_TYPE;

typedef struct oha_peer_tag
{
    LSA_UINT16                  LengthPeerChassisID;    /* max. 255 bytes OHA_MAX_CHASSIS_STRLEN */
    OHA_UPPER_MEM_U8_PTR_TYPE   pPeerChassisID;         /* PeerStationName (NameOfStation)    */
    LSA_UINT16                  LengthPeerPortID;       /* max. 255 bytes OHA_MAX_PORT_STRLEN */
    OHA_UPPER_MEM_U8_PTR_TYPE   pPeerPortID;            /* PeerPortName, e.g. "Port-001"      */
} OHA_PEER_TYPE;

typedef struct oha_adjust_tag OHA_COMMON_MEM_ATTR * OHA_ADJUST_PTR_TYPE;

typedef struct oha_adjust_tag
{
    LSA_UINT32          DomainBoundaryIngress;
    LSA_UINT32          DomainBoundaryEgress;
    LSA_BOOL            DomainBoundaryExisting;
    LSA_UINT32          MulticastBoundary;
    LSA_BOOL            MulticastBoundaryExisting;
    LSA_UINT32          PeerToPeerBoundary;
    LSA_BOOL            PeerToPeerBoundaryExisting;
} OHA_ADJUST_TYPE;


typedef struct oha_db_topo_tag OHA_COMMON_MEM_ATTR * OHA_DB_TOPO_PTR_TYPE;

typedef struct oha_db_topo_tag
{
    /* LSA_UINT16       PortId; */
    LSA_UINT16          MAUType;
    LSA_UINT16          NumberOfPeers;
    LSA_UINT16          LinkState;
    LSA_BOOL            DelayExisting;
    LSA_UINT32          LineDelayFactor; /* LineDelay x resolution */
    LSA_BOOL            LinkStateExisting;

    LSA_BOOL            DelayDifferenceExisting;
    LSA_UINT16          DelayMode;
    LSA_BOOL            MAUTypeDifferenceExisting;
    LSA_UINT16          MAUTypeMode;

    OHA_ADJUST_TYPE     AdjustTopoData;

    OHA_PEER_TYPE       PeerSender[LLDP_MAX_PORT_SENDER];     /* AP01531722: all sender */
} OHA_DB_TOPO_TYPE;

/* initial value(s) of the alarm specifier                  */
/* 0 ... the first diagnosis is always built                */
/* OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY ... the first diagnosis is built in case of an appearing event */
/* OHA_TOPO_ERROR_APPEARS ... the first diagnosis is built in case of a disappearing event */

#define OHA_ALARM_SPECIFIER_INITIAL_VALUE   OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY

typedef struct
{
    LSA_UINT16          LastSpecifierMau;
    LSA_UINT16          LastSpecifierDelay;
    LSA_UINT16          LastSpecifierLink;
    LSA_UINT16          LastSpecifierPeer_Chassis;
    LSA_UINT16          LastSpecifierPeer_Port;
    LSA_UINT16          LastSpecifierPeer_MRP_domain;
    LSA_UINT16          LastSpecifierPeer_None;
    LSA_UINT16          LastSpecifierPeer_MAU;
    LSA_UINT16          LastSpecifierPeer_CableDelay;
    LSA_UINT16          LastSpecifierPeer_RT_Class3;
    LSA_UINT16          LastSpecifierPeer_PTCP_status;
    LSA_UINT16          LastSpecifierPeer_PreambleLength;
    LSA_UINT16          LastSpecifierPeer_Fragmentation;
    LSA_UINT16          LastSpecifierInterface_Gateway;
    LSA_UINT16          LastSpecifierInterface_NoS;
    LSA_UINT16          LastSpecifierInterface_IP;
    LSA_UINT16          LastSpecifierInterface_NoD;
} OHA_TOPO_LAST_PRM_IND_STATES;


/* Last Topo Data Event: */

/*   diag entries (index-1 for DiagValid[], DiagAppears[] and DiagData)      */
/*   OHA_TOPO_MAX_IND_ENTRIES                                                */

#define OHA_DIAG_LINK               1            /* link status              */
#define OHA_DIAG_MAU                2            /* MAUType                  */
#define OHA_DIAG_DELAY              3            /* cable delay              */
#define OHA_DIAG_PEER_NONE          4            /* No Peer Detected         */
#define OHA_DIAG_PEER_CHASSIS       5            /* neighbour (ChassisID)    */
#define OHA_DIAG_PEER_PORT          6            /* neighbour (PortID)       */
#define OHA_DIAG_PEER_MRP_UUID      7            /* neighbour MRP_DomainUUID */
#define OHA_DIAG_PEER_MAU           8            /* neighbour MAUType        */
#define OHA_DIAG_PEER_CABLEDELAY    9            /* Peer CableDelay          */
#define OHA_DIAG_PEER_RT_CLASS3    10            /* Peer RT_Class3           */
#define OHA_DIAG_PEER_PTCP         11            /* Peer PTCP_Status         */
#define OHA_DIAG_PEER_PREAMBLE     12            /* Peer PreambleLength      */
#define OHA_DIAG_PEER_FRAGMENT     13            /* Peer Fragmentation       */

/*   diag entries for port-id = 0 (interface)      */
#define OHA_TOPO_MAX_INTERFACE_IND_ENTRIES            4

#define OHA_DIAG_INTERFACE_GATEWAY  1            /* Interface (GateWay)      */
#define OHA_DIAG_INTERFACE_NOS      2            /* Interface (NoS)          */
#define OHA_DIAG_INTERFACE_IP       3            /* Interface (IP)           */
#define OHA_DIAG_INTERFACE_NOD      4            /* Interface (NameOfDevice) */

typedef struct oha_last_prm_ind_data_tag OHA_COMMON_MEM_ATTR * OHA_LAST_PRM_IND_DATA_PTR_TYPE;

typedef struct oha_last_prm_ind_data_tag
{
    LSA_UINT16      PortId;
    /* diag valid or not */
    LSA_BOOL        DiagValid[OHA_TOPO_MAX_IND_ENTRIES];
    /* status valid if DiagValid = True */
    LSA_BOOL        DiagAppears[OHA_TOPO_MAX_IND_ENTRIES];

    /* ChannelNumber, ChannelProperties, ChannelErrorType, ExtChannelErrorType,... */
    OHA_PRM_DIAG_ENTRY_TYPE DiagData[OHA_TOPO_MAX_IND_ENTRIES];

} OHA_LAST_PRM_IND_DATA_TYPE;


#define OHA_SET_DIAG_ERROR_APPEARS(_prop) { \
_prop = OHA_TOPO_ERROR_APPEARS; _prop <<= 11; \
}
#define OHA_SET_DIAG_ERROR_DISAPPEARS_AND_OKAY(_prop) ( \
_prop = OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY; _prop <<= 11; \
)
#define OHA_SET_DIAG_ERROR_DISAPPEARS_OTHER_REMAIN(_prop) ( \
_prop = OHA_TOPO_ERROR_DISAPPEARS_OTHER_REMAIN; _prop <<= 11; \
)

/* Count of Delays in PNIO LLDP-TLV (LLDP_PNIO_DELAY) */
/* RxDelayLoc, RxDelayRem, TxDelayLoc, TxDelayRem, CableDelay */
/* The demanded buffersize is 5 x sizeof(LSA_UINT32). */
#define OHA_LLDP_DB_DELAY_ENTRIES                       5
#define OHA_LLDP_PNO_DELAY_SIZE                 (OHA_LLDP_DB_DELAY_ENTRIES * (sizeof(LSA_UINT32)))

#define OHA_LLDP_IDX_RXDELAY_LOC                        0
#define OHA_LLDP_IDX_RXDELAY_REM                        1
#define OHA_LLDP_IDX_TXDELAY_LOC                        2
#define OHA_LLDP_IDX_TXDELAY_REM                        3
#define OHA_LLDP_IDX_CABLEDELAY_LOC                     4

#define OHA_LLDP_OFFSET_RXDELAY_LOC         (OHA_LLDP_IDX_RXDELAY_LOC * (sizeof(LSA_UINT32)))
#define OHA_LLDP_OFFSET_RXDELAY_REM         (OHA_LLDP_IDX_RXDELAY_REM * (sizeof(LSA_UINT32)))
#define OHA_LLDP_OFFSET_TXDELAY_LOC         (OHA_LLDP_IDX_TXDELAY_LOC * (sizeof(LSA_UINT32)))
#define OHA_LLDP_OFFSET_TXDELAY_REM         (OHA_LLDP_IDX_TXDELAY_REM * (sizeof(LSA_UINT32)))
#define OHA_LLDP_OFFSET_CABLEDELAY_LOC      (OHA_LLDP_IDX_CABLEDELAY_LOC * (sizeof(LSA_UINT32)))

/* IRT PortStatus */

#define OHA_LLDP_PNO_PORT_STATUS_SIZE       (sizeof(LSA_UINT32))

/* MRP Status */

#define OHA_LLDP_PNO_PORT_STATUS_MRP_SIZE   (2 + sizeof(OHA_UUID_TYPE)) /* Reserved (RRT) + MRP_DomainUUID*/

/* LLDP_PNIO_CHASSIS_MAC_ADR */

#define OHA_LLDP_PNO_CHASSIS_MAC_ADR_SIZE       (sizeof(OHA_MAC_TYPE))

/* PtcpStatus: PTCP_MasterSourceAddress + PTCP_SubdomainUUID + IRDataUUID +       */
/*     LengthOfPeriod + LLDP_RedPeriodBegin, LLDP_OrangePeriodBegin, LLDP_GreenPeriodBegin */
#define OHA_LLDP_PNO_PTCP_STATUS_SIZE   (OHA_MAC_ADDR_SIZE + OHA_UUID_SIZE + OHA_UUID_SIZE + (4*sizeof(LSA_UINT32)))

/* MAC/PHY Configuration/Status - PMD autoneg advertised capability */

#define OHA_MAU_AUTONEG_CAP_OTHER       0x8000
#define OHA_MAU_AUTONEG_CAP_10BASET     0x4000
#define OHA_MAU_AUTONEG_CAP_10BASETFD   0x2000
#define OHA_MAU_AUTONEG_CAP_100BASET4   0x1000
#define OHA_MAU_AUTONEG_CAP_100BASETX   0x0800
#define OHA_MAU_AUTONEG_CAP_100BASETXFD 0x0400
#define OHA_MAU_AUTONEG_CAP_100BASET2   0x0200
#define OHA_MAU_AUTONEG_CAP_100BASET2FD 0x0100
#define OHA_MAU_AUTONEG_CAP_FDXPAUSE    0x0080
#define OHA_MAU_AUTONEG_CAP_FDXAPAUSE   0x0040
#define OHA_MAU_AUTONEG_CAP_FDXSPAUSE   0x0020
#define OHA_MAU_AUTONEG_CAP_FDXBPAUSE   0x0010
#define OHA_MAU_AUTONEG_CAP_1000BASEX   0x0008
#define OHA_MAU_AUTONEG_CAP_1000BASEXFD 0x0004
#define OHA_MAU_AUTONEG_CAP_1000BASET   0x0002
#define OHA_MAU_AUTONEG_CAP_1000BASETFD 0x0001

/*---------------------------------------------------------------------------*/
/* Overview of Handle and Requests handled by OHA:                           */
/*                                                                           */
/* We have four types of handles:                                            */
/*                                                                           */
/* - SOCK-Systemchannel (one for all NICs)                                   */
/* - DCP-Systemchannel (one for every NIC)                                   */
/* - EDD-Systemchannel (one for every NIC)                                   */
/* - LLDP-Systemchannel (one for every NIC)                                  */
/* - NARE-Systemchannel (one for every NIC)                                  */
/* - USER-Channels (multiple per NIC possible)                               */
/*                                                                           */
/* The Systemchannels have to be opened by Systemadaption before a user      */
/* can open a USER-Channel. User-Requests can only be done with USER-Channels*/
/*                                                                           */
/* All Requests are handled by the User-Channel. Each user Channel holds a   */
/* number of RCTRL-blocks (RequestControlBlocks) which hold all information  */
/* about the current state of the Request within OHA.                        */
/*                                                                           */
/* This User-RCTRL-Block will move through the USER, LLDP, DCP, EDD, SOCK    */
/* and OHA channels depending on the Actions to perform.                     */
/*                                                                           */
/* If a User-Request needs a DCP-Call the RCTRL-Blcok is queued within the   */
/* DCP-Handle for performing a DCP-Request.                                  */
/* The same handling is done with EDD, NARE, LLDP and SOCK-Requests.         */
/*                                                                           */
/* The RCTRL-Block managed as queue of RCTRL-Blocks. So we can use the       */
/* index into the queue for reference. For DCP and SOCK we use this index    */
/* as user_id within the lower RQB, so we find the proper RCTRL-Block on     */
/* confirmation (the requests are not first in first out! so we need to      */
/* find the proper RCTRL!)                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/


/*===========================================================================*/
/* Handle-management-structures                                              */
/*===========================================================================*/

/* general handle parameters */
typedef struct oha_OHA_HANDLE_PARAMS_TYPE
{
    LSA_SYS_PATH_TYPE                   SysPath;
    LSA_SYS_PTR_TYPE                    pSys;
    OHA_DETAIL_PTR_TYPE                 pDetail;
    LSA_HANDLE_TYPE                     HandleUpper;
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE     Cbf;
} OHA_HANDLE_PARAMS_TYPE;

/* States: */
#define OHA_HANDLE_STATE_FREE                          1
#define OHA_HANDLE_STATE_CLOSE_USER_RUNNING            2
#define OHA_HANDLE_STATE_CLOSED                        3
#define OHA_HANDLE_STATE_OPEN                          4

#define OHA_HANDLE_STATE_OPEN_EDD_RUNNING              10
#define OHA_HANDLE_STATE_CLOSE_EDD_RUNNING             11
#define OHA_HANDLE_STATE_GET_MAC_EDD_RUNNING           12  /* get MAC of EDD-Channel       */
#define OHA_HANDLE_STATE_GET_LINK_STATUS_EDD_RUNNING   13  /* get LiknStatus of EDD-Channel*/
#define OHA_HANDLE_STATE_SET_ADDRESS_EDD_RUNNING       14  /* set IP, NoS of interface     */
#define OHA_HANDLE_STATE_CANCEL_ERROR_EDD_RUNNING      15  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING       16  /* close because of open error  */

#define OHA_HANDLE_STATE_OPEN_DCP_RUNNING              20
#define OHA_HANDLE_STATE_CLOSE_DCP_RUNNING             21
#define OHA_HANDLE_STATE_CANCEL_ERROR_DCP_RUNNING      23  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_DCP_RUNNING       24  /* close because of open error  */

#define OHA_HANDLE_STATE_OPEN_LLDP_RUNNING             30
#define OHA_HANDLE_STATE_CLOSE_LLDP_RUNNING            31
#define OHA_HANDLE_STATE_STOP_LLDP_RUNNING             32  /* StopSend by CloseChannel runs*/
#define OHA_HANDLE_STATE_GET_CONFIG_LLDP_RUNNING       33  /* get Config of LLDP-Channel   */
#define OHA_HANDLE_STATE_SET_LLDP_RUNNING              34  /* Set of the local send data   */
#define OHA_HANDLE_STATE_CANCEL_ERROR_LLDP_RUNNING     35  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_LLDP_RUNNING      36  /* close because of open error  */

#define OHA_HANDLE_STATE_OPEN_SOCK_RUNNING             40
#define OHA_HANDLE_STATE_CLOSE_SOCK_RUNNING            41
#define OHA_HANDLE_STATE_SETMIB2_SOCK_RUNNING          42  /* Set MIB2 Parameters (SOCK)   */
#define OHA_HANDLE_STATE_OPEN_PORT_RUNNING             43  /* opens port 161 for SNMP      */
/* #define OHA_HANDLE_STATE_CLOSE_PORT_RUNNING         43  *//* close port 161 for SNMP    */
#define OHA_HANDLE_STATE_CANCEL_ERROR_SOCK_RUNNING     44  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_SOCK_RUNNING      45  /* close because of open error  */
#define OHA_HANDLE_STATE_SETIP_SOCK_RUNNING            46  /* Set of IP Address (SOCK)     */

#define OHA_HANDLE_STATE_OPEN_NARE_RUNNING             50
#define OHA_HANDLE_STATE_CLOSE_NARE_RUNNING            51
#define OHA_HANDLE_STATE_CANCEL_ERROR_NARE_RUNNING     52  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_NARE_RUNNING      53  /* close because of open error  */

#define OHA_HANDLE_STATE_OPEN_SNMPX_RUNNING            60
#define OHA_HANDLE_STATE_CLOSE_SNMPX_RUNNING           61
#define OHA_HANDLE_STATE_REGISTER_SNMPX_RUNNING        62  /* registers LLDP subagent      */
#define OHA_HANDLE_STATE_UNREGISTER_SNMPX_RUNNING      63  /* unregister LLDP subagent     */
#define OHA_HANDLE_STATE_CANCEL_ERROR_SNMPX_RUNNING    64  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_SNMPX_RUNNING     65  /* close because of open error  */

#define OHA_HANDLE_STATE_CLOSE_WAIT_TIMER              70  /* wait for timer while closing */

#define OHA_HANDLE_STATE_OPEN_MRP_RUNNING              80
#define OHA_HANDLE_STATE_CLOSE_MRP_RUNNING             81
#define OHA_HANDLE_STATE_CANCEL_ERROR_MRP_RUNNING      82  /* cancel because of open error */
#define OHA_HANDLE_STATE_CLOSE_ERROR_MRP_RUNNING       83  /* close because of open error  */

typedef struct oha_user_rqb_link_tag
{
    OHA_UPPER_RQB_PTR_TYPE          pBottom;
    OHA_UPPER_RQB_PTR_TYPE          pTop;
} OHA_USER_RQB_LINK_TYPE;

/* --------------------------------------------------------------------------*/
/* for SOCK-Systemchannel (only one SOCK-Channel possible)..                 */
/*                                                                           */
/* for handling all requests to SOCK. We have a queue which holds all        */
/* user-request blocks awaiting service. i.e. need a request to SOCK.        */
/* If a free SOCK-Request nlock is present pending user-requests are         */
/* removed and assotiated with a SOCK-Request block.                         */
/*                                                                           */
/* We have only a limited number of SOCK-Request blocks (configured). This   */
/* number is the number of simultaneous active SOCK-requests.                */
/*                                                                           */
/* --------------------------------------------------------------------------*/

/* receive-resources for SNMP      */
#define OHA_SNMP_NUM_OF_RECV            1

/* Agent-Request-States: */
#define OHA_SNMP_STATE_STOP             1  /* initial, after OpenChannel */
#define OHA_SNMP_STATE_WF_STOP          2  /* SNMP_STOP is active        */
#define OHA_SNMP_STATE_START            3  /* SNMP_START is done         */
#define OHA_SNMP_STATE_WF_START         4  /* SNMP_START is active       */
#define OHA_SNMP_STATE_WF_STATISTICS    5  /* get Statistics from LLDP   */
#define OHA_SNMP_STATE_WF_USER          6  /* user-query 'Set' or 'NoSet'*/

/* LowerLayer (SOCK) Request control block  */
#ifndef OHA_SOCK_RQB_PTR_TYPE
#define OHA_SOCK_RQB_PTR_TYPE          SOCK_UPPER_RQB_PTR_TYPE
#endif

typedef struct oha_sock_rqb_link_tag
{
    OHA_SOCK_RQB_PTR_TYPE              pBottom;
    OHA_SOCK_RQB_PTR_TYPE              pTop;
} OHA_SOCK_RQB_LINK_TYPE;

typedef struct oha_sock_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_SOCK_RQB_LINK_PTR_TYPE;

typedef struct oha_hdb_sock_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_SOCK_PTR_TYPE;

typedef struct oha_hdb_agnt_tag
{
    LSA_UINT16                       RequestState; /* request state (SNMP act/deact)*/
    LSA_UINT16                       SnmpHandle;   /* the used SNMP handle          */
    LSA_UINT32                       StatsPendCnt; /* pending LLDP/MRP GetStats     */
    OHA_SOCK_RQB_LINK_TYPE           SnmpRQBs;     /* Request-Control-blocks for    */
												   /* this Agent.                   */
    LSA_UINT32                       SnmpRQBCnt;   /* Number of pending SNMP RQBs   */
    LSA_UINT32                       RxPendingCnt; /* Number of pending SOCK RCVs   */
} OHA_HDB_AGNT_TYPE;

typedef struct oha_hdb_agnt_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_AGNT_PTR_TYPE;

typedef struct oha_hdb_sock_tag
{
    LSA_UINT8                        State;        /* State of handle (channel)     */
    LSA_HANDLE_TYPE                  ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE           Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE           pOCReq;       /* Open or Close Request         */
    LSA_UINT16                       OpenResponse; /* Response from Open if error   */
    LSA_UINT16                       UserCnt;      /* Number of User-Channels using */
    /* this SOCK-channel.            */
    LSA_HANDLE_TYPE                  SockHandle;   /* If InUse. the channel handle */
    LSA_UINT32                       UserRQBCnt;   /* Number of pending user RQBBs (ControlSock) */
    OHA_USER_RQB_LINK_TYPE           UserRQBs;     /* Queued User-RQB-Blocks       (ControlSock) */
    /* ------------------------------*/
	OHA_HDB_AGNT_TYPE                LldpAgent;    /* controls for the LLDP.MIB     */
	OHA_HDB_AGNT_TYPE                MrpAgent;     /* controls for the MRP.MIB      */
    /* ------------------------------*/
    LSA_UINT32                       SetIpRQBCnt;  /* Number of pending SetAddrInfo RQBs */
    OHA_SOCK_RQB_LINK_TYPE           SetIpRQBs;    /* RQB's for SetIpAddress        */
    /* LSA_INT32                      SockFD;    *//* lower-layer send sock         */
    LSA_UINT32                       Mib2RQBCnt;   /* Number of pending SOCK RQBs   */
    OHA_SOCK_LOWER_RQB_PTR_TYPE      pMib2IndRQB;  /* Mib2Params indication         */
} OHA_HDB_SOCK_TYPE;

/* --------------------------------------------------------------------------*/
/* for SNMPX-Systemchannel (only one SNMPX-Channel possible).                */
/*                                                                           */
/* for handling all requests to SNMPX. We have a queue which holds all       */
/* user-request blocks awaiting service. i.e. need a request to SNMPX.       */
/* If a free SNMPX-Request nlock is present pending user-requests are        */
/* removed and assotiated with a SNMPX-Request block.                        */
/*                                                                           */
/* We have only a limited number of SNMPX-Request blocks (configured). This  */
/* number is the number of simultaneous active SNMPX-requests.               */
/*                                                                           */
/* --------------------------------------------------------------------------*/
#ifdef OHA_CFG_USE_SNMPX

/* Agent-Request-States: */
#define OHA_SNMPX_STATE_STOP            1  /* initial, after OpenChannel */
#define OHA_SNMPX_STATE_WF_STOP         2  /* SNMPX_STOP is active       */
#define OHA_SNMPX_STATE_START           3  /* SNMPX_START is done        */
#define OHA_SNMPX_STATE_WF_START        4  /* SNMPX_START is active      */

/* LowerLayer (SNMPX) Request control block  */
#ifndef OHA_SNMPX_RQB_PTR_TYPE
#define OHA_SNMPX_RQB_PTR_TYPE          SNMPX_UPPER_RQB_PTR_TYPE
#endif

typedef struct oha_snmpx_rqb_link_tag
{
    OHA_SNMPX_RQB_PTR_TYPE              pBottom;
    OHA_SNMPX_RQB_PTR_TYPE              pTop;
} OHA_SNMPX_RQB_LINK_TYPE;

typedef struct oha_snmpx_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_SNMPX_RQB_LINK_PTR_TYPE;

typedef struct oha_hdb_snmpx_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_SNMPX_PTR_TYPE;

typedef struct oha_hdb_snmpx_tag
{
    LSA_UINT8                        SnmpxState;   /* State of handle               */
    LSA_HANDLE_TYPE                  ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE           Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE           pOCReq;       /* Open or Close Request         */
    LSA_UINT16                       OpenResponse; /* Response from Open if error   */
    LSA_UINT16                       UserCnt;      /* Number of User-Channels using */
    /* this SNMPX-channel.           */
    LSA_UINT16                       RequestState; /* request state (SNMP act/deact)*/
    LSA_HANDLE_TYPE                  SnmpxHandle;  /* If InUse. the SNMPX handle    */
    LSA_UINT32                       UserRQBCnt;   /* Number of pending user RQBBs  */
    OHA_USER_RQB_LINK_TYPE           UserRQBs;     /* Queued User-RQB-Blocks        */
    /* ------------------------------*/
    LSA_UINT32                       SnmpxRQBCnt;  /* Number of pending SNMPX RQBs  */
    OHA_SNMPX_RQB_LINK_TYPE          SnmpxRQBs;    /* Request-Control-blocks for    */
    /* SnmpxRequests (Pending)       */
    LSA_UINT32                       RxPendingCnt; /* Number of pending SNMPX RCVs  */
} OHA_HDB_SNMPX_TYPE;
#endif


/* --------------------------------------------------------------------------*/
/* For NARE Channel                                                          */
/*                                                                           */
/* For handling all NARE-Request from OHA to NARE                            */
/* --------------------------------------------------------------------------*/
#ifndef OHA_CFG_NO_NARE

/* LowerLayer (NARE) Request management type */
#ifndef OHA_NARE_RQB_PTR_TYPE
#define OHA_NARE_RQB_PTR_TYPE          NARE_UPPER_RQB_PTR_TYPE
#endif

typedef struct oha_nare_rqb_link_tag
{
    OHA_NARE_RQB_PTR_TYPE          pBottom;
    OHA_NARE_RQB_PTR_TYPE          pTop;
} OHA_NARE_RQB_LINK_TYPE;

typedef struct oha_nare_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_NARE_RQB_LINK_PTR_TYPE;

/* CheckAddress-Data (OhaOpcCheckAddress) */
typedef struct oha_check_info_tag
{
    LSA_UINT32               ThisIdx;     /* Index of this element in array  */
    /* = UserId                        */
    LSA_UINT8                State;       /* actual state                    */
    LSA_BOOL                 Underway;    /* LSA_TRUE if NareRqb in progress */
    LSA_UINT32               Interval;    /* SendInterval in seconds         */
    LSA_UINT32               NextSend;    /* Next Send Timer counter.        */
    OHA_NARE_RQB_PTR_TYPE    pCheckRQB;   /* Request-Control-block for       */
    /* NareRequest                     */
    OHA_NARE_RQB_PTR_TYPE    pCancelRQB;  /* Request-Control-block for       */
    /* cancelling NareRequest          */
    LSA_UINT32               StillUnderway; /* cannot send request, because  */
    /* last one still in progress.     */
    LSA_UINT16               LastResponse;/* last CHECK info to the user     */
    LSA_BOOL                 UseDelay;   /* if true an initial delay will be used (in) */
} OHA_CHECK_INFO_TYPE;

typedef struct oha_check_info_tag  OHA_LOCAL_MEM_ATTR *    OHA_CHECK_INFO_PTR_TYPE;

/* States of CheckAddress (cyclic) */
#define OHA_CHECK_STATE_STOP                 0 /* cyclic check incative or stopped */
#define OHA_CHECK_STATE_START                1 /* the cyclic check is running      */
#define OHA_CHECK_STATE_WITHDRAW             2 /* a withdraw of cyclic check is in progress*/

typedef struct oha_hdb_nare_tag OHA_LOCAL_MEM_ATTR *   OHA_HDB_NARE_PTR_TYPE;

typedef struct oha_hdb_nare_tag
{
    LSA_UINT16              State;        /* handle state                  */
    LSA_HANDLE_TYPE         ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE  Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE  pOCReq;       /* Open or Close Request running */
    LSA_UINT16              OpenResponse; /* Response from Open if error   */
    LSA_UINT16              UserCnt;      /* Number of User-Channels using */
    /* this NARE-channel.            */
    LSA_HANDLE_TYPE         NareHandle;   /* If Open the lower NARE handle */
    LSA_UINT32              UserRQBCnt[OHA_CFG_MAX_CHANNELS];/* Number of pending User-RQBs */
    OHA_USER_RQB_LINK_TYPE  UserRQBs[OHA_CFG_MAX_CHANNELS];  /* Pending UserRQ-Blocks       */
    /* ------------------------------*/
    LSA_UINT32              NareRQBCnt;   /* Number of pending NARE RQBs   */
    /* OHA_NARE_RQB_LINK_TYPE  NareRQBs;*//* Request-Control-blocks for    */
    /* NareRequests (Pending)        */
    /* for OHA_OPC_SET_ADDRESS and OHA_OPC_DCP_RESPONSE:   */
    OHA_CHECK_INFO_TYPE     IpCheck;      /* check info (Set IP)           */

    OHA_DB_IF_PTR_TYPE      pDB;          /* pointer to database-interface */
} OHA_HDB_NARE_TYPE;
#endif

/* we have OHA_MAX_NICS NARE-channels. */


/* --------------------------------------------------------------------------*/
/* For UserChannel                                                           */
/*                                                                           */
/* For handling all User-Requests to OHA. We have a queue which holds all    */
/* user-requests till resources are free for further processing (UserReq).   */
/*                                                                           */
/* Every UserChannel uses one assoziated DCP-, EDD-, NARE-, LLDP-channel     */
/* selected by NICId in CDB (detail-ptr). Every UserChannel uses one SOCK-   */
/* Channel (currently there is only one SOCK-Channel for all User-Channels,  */
/* because the SOCK-Channel is NIC independent)                              */
/* --------------------------------------------------------------------------*/

/* Topo-States: */
#define OHA_TOPO_STATE_CLOSE            1  /* channel is not registered  */
#define OHA_TOPO_STATE_OPEN             2  /* channel is registered      */
#define OHA_TOPO_STATE_STOP             3  /* no checking of topo data   */
#define OHA_TOPO_STATE_START            4  /* checking of topo data      */

/* Prm-States: */
#define OHA_PRM_STATE_CLOSE             0  /* channel is not registered  */
/* #define OHA_PRM_STATE_INIT           1*//* no (new) prm data written  */
#define OHA_PRM_STATE_WRITTEN           1  /* a PrmWrite is done         */
#define OHA_PRM_STATE_PREPARE           2  /* a PrmPrepare is done       */
#define OHA_PRM_STATE_END               3  /* a PrmEnd is done           */
#define OHA_PRM_STATE_COMMIT            4  /* a PrmCommit is done        */

typedef struct oha_hdb_user_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_USER_PTR_TYPE;

typedef struct oha_hdb_user_tag
{
    LSA_UINT16                     State;             /* handle state                  */
    LSA_HANDLE_TYPE                ThisHandle;        /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE         Params;            /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE         pOCReq;            /* Open or Close Request running */
    LSA_UINT16                     OpenResponse;      /* Response from Open if error   */

    LSA_UINT32                     UserReqPendCnt;    /* Number of User-Requests       */
    /* currently pending             */
    OHA_USER_RQB_LINK_TYPE         SnmpUserReq;       /* User Requests for SNMP        */
    LSA_UINT32                     StatsPendCnt;      /* Nr. of pending LLDP GetStats  */
    OHA_USER_RQB_LINK_TYPE         SockUserReq;       /* User Requests for SOCK        */
    OHA_USER_RQB_LINK_TYPE         Mib2UserReq;       /* MIBII Requests for SOCK       */
    OHA_USER_RQB_LINK_TYPE         TcIpUserReq;       /* TCIP Requests for SOCK        */
    LSA_UINT32                     DcpIndReqPending;  /* Number of pending ind. request*/
    OHA_USER_RQB_LINK_TYPE         DcpIndReq;         /* Indication Requests for DCP   */
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pLastDcpInd;  /* ind-resource by last DcpSet.Ind */

    LSA_UINT32                     LldpIndReqPending; /* Number of pending ind. request*/
    OHA_USER_RQB_LINK_TYPE         LldpIndReq;        /* Indication Requests for LLDP  */
    OHA_UPPER_LLDP_INDICATION_PTR_TYPE pLastLldpInd;  /* ind-resource by last Lldp.Ind */

    LSA_HANDLE_TYPE                EDDHandle;         /* If Open the lower EDD handle  */
    LSA_HANDLE_TYPE                SOCKHandle;        /* If Open the lower SOCK handle */
    LSA_HANDLE_TYPE                NAREHandle;        /* If Open the lower NARE handle */
    LSA_HANDLE_TYPE                DCPHandle;         /* If Open the lower DCP handle  */
    LSA_HANDLE_TYPE                LLDPHandle;        /* If Open the lower LLDP handle */
    OHA_MAC_TYPE                   MACAddr;           /* MAC Address of NIC            */
    /* One RCTRL for every port      */
    LSA_UINT32                     PortCnt;           /* Number of Ports               */
    LSA_UINT32                     PortMaxSender;     /* Number of Sender per Port (Rx)*/
    LSA_UINT32                     InStartCnt;        /* Number of active Ports (START)*/
    LSA_UINT32                     EddReqPendingCnt;  /* Number of pending EDD-requests*/
    LSA_UINT32                     NareReqPendingCnt; /* Number of pend. NARE-requests */
    LSA_UINT32                     SockReqPendingCnt; /* Number of pend. SOCK-requests */
    LSA_UINT32                     LldpReqPendingCnt; /* Number of pend. LLDP-requests */

    LSA_UINT16                     TopoState;         /* handle state (start, stop,..) */
    LSA_UINT16                     PrmState;          /* handle state (prepare, write, end) */
    LSA_UINT32                     TopoReqPendingCnt; /* Number of pend. TOPO-requests */
    OHA_USER_RQB_LINK_TYPE         TopoUserReq;       /* Indication Requests for TOPO  */

    LSA_UINT32                     TopoIndReqPending; /* Number of pending ind. request*/
    OHA_USER_RQB_LINK_TYPE         TopoIndReq[OHA_CFG_MAX_PORTS+1];   /* Indication Requests for TOPO  */
    OHA_LAST_PRM_IND_DATA_PTR_TYPE pPortTopoInd[OHA_CFG_MAX_PORTS+1]; /* last topo changes for this user */
    OHA_TOPO_LAST_PRM_IND_STATES   TopoLastIndStati[OHA_CFG_MAX_PORTS+1]; /* last specifiers and stati     */

    LSA_UINT16                     TopoRequestState;  /* request state (topo data)     */
    OHA_DB_TOPO_PTR_TYPE           pTopoDataBase[OHA_CFG_MAX_PORTS]; /* expected topo data */
    OHA_DB_TOPO_PTR_TYPE           pPrmTopoBase[OHA_CFG_MAX_PORTS]; /* buffered topo data */

    OHA_UPPER_RQB_PTR_TYPE         pAddressInd[OHA_CFG_MAX_NICS];   /* Indication Requests for ADDRESS */

    LSA_UINT16                     PendingPortId;/* PortId for EDD_SRV_GET_PORT_PARAMS */
    LSA_UINT32                     LldpIndPendingCnt; /* Number of pend. Ind-Req (LLDP)*/
    LSA_UINT32                     DcpIndPendingCnt;  /* Number of pend. Ind-Req (DCP) */

    LSA_UINT32                     CheckIndReqPending; /* Number of pending ind. requests OhaOpcCheckIndication */
    OHA_USER_RQB_LINK_TYPE         CheckIndReq;        /* CheckIndication Requests for CheckName and CheckIp  */

    LSA_UINT32                     HelloIndReqPending; /* Number of pending ind. requests OhaOpcHelloIndication */
    OHA_USER_RQB_LINK_TYPE         HelloIndReq;        /* HelloIndication Requests for Device Initiative  */

    LSA_UINT32                     RemaIndPending[OHA_CFG_MAX_NICS];  /* Number of pending REMA ind. requests */
    OHA_USER_RQB_LINK_TYPE         RemaInd[OHA_CFG_MAX_NICS];       /* RemaIndication Requests */

#ifndef OHA_CFG_NO_NARE            /* for OHA_OPC_RESOLVE_ADDRESS: */
    OHA_CHECK_INFO_TYPE            IpCheckInfo[OHA_CFG_MAX_NICS]; /* array of cyclic Check info (Resolve IP) */
    OHA_CHECK_INFO_TYPE            NameCheckInfo[OHA_CFG_MAX_NICS]; /* array of cyclic Check info (Resolve NameOfStation) */
#endif

    LSA_UINT16                     PtcpTimerID[OHA_CFG_MAX_PORTS]; /* Timer_ID for PTCP diagnosis   */
    LSA_UINT16                     PtcpDelayTime[OHA_CFG_MAX_PORTS]; /* delay for the PTCP diag.    */
    LSA_BOOL                       PtcpTimerInUse[OHA_CFG_MAX_PORTS]; /* False/True ... timer (in)active */

    LSA_UINT32                     LldpConfigIndPending; /* Number of pending If-config ind. request */
    OHA_USER_RQB_LINK_TYPE         LldpConfigInd;        /* Indication Requests for LLDP-Set (global) */

    LSA_UINT32                     LldpPortConfigIndPending; /* Number of pending port-config ind. request */
    OHA_USER_RQB_LINK_TYPE         LldpPortConfigInd;  /* Indication Requests for LLDP-Set (ports)  */
} OHA_HDB_USER_TYPE;


/* --------------------------------------------------------------------------*/
/* For LLDP Channel                                                          */
/*                                                                           */
/* For handling all LLDP-Request from OHA to LLDP                            */
/* --------------------------------------------------------------------------*/

/* Lldp-Request-States: */
#define OHA_LLDP_STATE_CLOSE            1  /* initial, after OpenChannel */
#define OHA_LLDP_STATE_STOP             2  /* no sending of data         */
#define OHA_LLDP_STATE_WF_STOP          3  /* LLDP_STOP is active        */
#define OHA_LLDP_STATE_START            4  /* sending of data is active  */
#define OHA_LLDP_STATE_WF_START         5  /* LLDP_START is active       */
#define OHA_LLDP_STATE_RE_START         6  /* LLDP_START again (umproj.) */


/* LowerLayer (LLDP) Request management type */
#ifndef OHA_LLDP_RQB_PTR_TYPE
#define OHA_LLDP_RQB_PTR_TYPE          LLDP_UPPER_RQB_PTR_TYPE
#endif

typedef struct oha_lldp_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_LLDP_RQB_LINK_PTR_TYPE;

typedef struct oha_lldp_rqb_link_tag
{
    OHA_LLDP_RQB_PTR_TYPE          pBottom;
    OHA_LLDP_RQB_PTR_TYPE          pTop;
} OHA_LLDP_RQB_LINK_TYPE;

typedef struct oha_hdb_lldp_tag OHA_LOCAL_MEM_ATTR *   OHA_HDB_LLDP_PTR_TYPE;

typedef struct oha_hdb_lldp_tag
{
    LSA_UINT16                 State;        /* handle state                  */
    LSA_HANDLE_TYPE            ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE     Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE     pOCReq;       /* Open or Close Request running */
    LSA_UINT16                 OpenResponse; /* Response from Open if error   */
    LSA_UINT16                 UserCnt;      /* Number of User-Channels using */
    /* this LLDP-channel.            */
    LSA_UINT16                 RequestState; /* request state (sending data)  */
    LSA_HANDLE_TYPE            LldpHandle;   /* If Open the lower LLDP handle */
    LSA_UINT32                 UserRQBCnt;   /* Number of pending User-RQBs   */
    OHA_USER_RQB_LINK_TYPE     UserRQBs;     /* Pending UserRQ-Blocks         */
    /* ------------------------------*/
    OHA_DB_IF_PTR_TYPE         pDB;          /* pointer to database-interface */
    LSA_UINT16                 PendingPortId;/* PortId for LLDP_SET           */
    LSA_UINT16                 StartingPortId;/* PortId for LLDP_START        */

    LSA_UINT32                 RxPendingCnt; /* Number of pending LLDP RQBs   */
    OHA_LLDP_RQB_LINK_TYPE     LldpRQBs[OHA_CFG_MAX_PORTS+1]; /* Request-Control-blocks for    */
    /* LLdpRequests (Pending) 0 for LLDP_PORT_ID_ALL! */
    LSA_BOOL                   StartStopActive[OHA_CFG_MAX_PORTS+1]; /* LldpStart or LldpStop active */
    /* neighbour-infos               */
    OHA_LLDP_INDICATION_TYPE   LastInd;      /* last remote changes           */

    LSA_UINT16                 Set_PendingCnt[OHA_CFG_MAX_PORTS]; /* Number of pend. LLDP-Set RQBs */
    LSA_BOOL                   Set_IsActual[OHA_CFG_MAX_PORTS]; /* LldpSet is mirrored to the DB or not */

    OHA_HDB_USER_PTR_TYPE      pHDBUserQueryIf; /* query user for global snmp-set */
    LSA_UINT16                 QueryIfPendCnt; /* outstanding OHA_OPC_LLDP_CONFIG_RESPONSE */
    OHA_HDB_USER_PTR_TYPE      pHDBUserQueryPort; /* query user for port snmp-set */
    LSA_UINT16                 QueryPortPendCnt; /* outstanding OHA_OPC_LLDP_PORT_CONFIG_RESPONSE */
} OHA_HDB_LLDP_TYPE;


/* --------------------------------------------------------------------------*/
/* For EDD Channel                                                           */
/*                                                                           */
/* For handling all EDD-Request from OHA to EDD                              */
/* --------------------------------------------------------------------------*/

typedef struct oha_hdb_edd_link_status_tag
{
    /* LSA_UINT16       PortID; */
    LSA_UINT8           Autoneg;
    LSA_UINT8           PortStatus; /* EDD_PORT_PRESENT, EDD_PORT_NOT_PRESENT */
    LSA_UINT16          LinkStatus;
    LSA_UINT8           Speed;
    LSA_UINT8           Mode;
    LSA_UINT16          MAUType;        /* from ExtLinkStatus */
    LSA_UINT16          MediaType;      /* from ExtLinkStatus */
    LSA_UINT32          LineDelay;
    LSA_UINT32          CableDelay;     /* in ns   */
    LSA_BOOL            DelayExisting;
    LSA_UINT8           IRTPortStatus;  /* Class 3 */
    LSA_UINT8           SRTPortStatus;  /* Class 2 */
    OHA_MAC_TYPE        PortMACAddr;
    LSA_UINT8           SpeedModeConfigured;
    LSA_UINT32          SpeedModeCapability;
    LSA_UINT32          RxIntervalRedBegin; /* Red Part of Reserved Interval */
    LSA_UINT32          RxIntervalRedEnd;   /* (contains IRT_Frames only)    */
    LSA_UINT32          RxIntervalOrangeBegin; /* Orange Part of Reserved Interval */
    LSA_UINT32          RxIntervalOrangeEnd;
    LSA_BOOL            IsWireless;
    LSA_BOOL            ShortPreamble;
    LSA_BOOL            TxFragmentation;
    LSA_UINT8           RTC2_PortStatus;    /* for info record */
    LSA_UINT8           RTC3_PortStatus;    /* for info record */
    LSA_UINT32          RxDelay;            /* for info record */
    LSA_UINT32          TxDelay;            /* for info record */
    LSA_UINT8           PortState;          /* for info record */
    LSA_UINT8           MRP_PortState;      /* for info record */
    LSA_UINT8           SyncTopoOk;         /* for info record */
	LSA_UINT8           PhyStatus;		/* for dynamic ifDescr string */
} OHA_HDB_EDD_LINK_STATUS_TYPE;

/* LengthOfPeriod:    Duration of a cycle in nanoseconds, multiply of 31250 nanoseconds */
/* RedPeriodBegin:    FrameOffset of first IRT-Frame over all ports in ns */
/* OrangePeriodBegin: FrameOffset of first ISRT/IRT-Frame over all ports in ns */
/* GreenPeriodBegin:  FrameOffset + FrameForwardingTime of last ISRT/IRT-Frame over all ports in ns */

typedef struct oha_frame_offset_tag   OHA_LOCAL_MEM_ATTR * OHA_FRAME_OFFSET_PTR_TYPE;

typedef struct oha_frame_offset_tag
{
    LSA_UINT32      LengthOfPeriod; /* SendClockFactor x 31.25us */
    LSA_UINT32      RedPeriodBegin;
    LSA_UINT32      OrangePeriodBegin;
    LSA_UINT32      GreenPeriodBegin;
} OHA_FRAME_OFFSET_TYPE;

/* LowerLayer (EDD) Request management type */
#ifndef OHA_EDD_RQB_PTR_TYPE
#define OHA_EDD_RQB_PTR_TYPE          EDD_UPPER_RQB_PTR_TYPE
#endif

typedef struct oha_edd_rqb_link_tag
{
    OHA_EDD_RQB_PTR_TYPE          pBottom;
    OHA_EDD_RQB_PTR_TYPE          pTop;
} OHA_EDD_RQB_LINK_TYPE;


typedef struct oha_edd_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_EDD_RQB_LINK_PTR_TYPE;

typedef struct oha_hdb_edd_tag OHA_LOCAL_MEM_ATTR *   OHA_HDB_EDD_PTR_TYPE;

typedef struct oha_hdb_edd_tag
{
    LSA_UINT16                       State;           /* handle state                  */
    LSA_HANDLE_TYPE                  ThisHandle;      /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE           Params;          /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE           pOCReq;          /* Open or Close Request running */
    LSA_UINT16                       OpenResponse;    /* Response from Open if error   */
    /* occurred while opening.       */
    LSA_UINT16                       UserCnt;         /* Number of User-Channels using */
    /* this EDD-channel.             */
    LSA_HANDLE_TYPE                  EDDHandle;       /* If Open the lower EDD handle. */
    LSA_UINT32                       UserRQBCnt;      /* Number of pending RCtrlBl.    */
    OHA_USER_RQB_LINK_TYPE           UserRQBs;        /* Pending UserCtrl-Blocks       */
    /* OHA_MAC_TYPE                  MACAddr;       *//* MAC Address of NIC            */
    /* LSA_UINT32                    PortCnt;       *//* Number of Ports               */
    LSA_UINT32                       EddRQBCnt;       /* Number of pending EDD RQBs    */
    OHA_EDD_RQB_LINK_TYPE            EddRQBs;         /* Request-Control-blocks for    */
    /* EDD-Requests (Pending)        */
    LSA_UINT16                       LinkIndPendingCnt;/* Number of LinkInd-RQBs pend. */
    OHA_HDB_EDD_LINK_STATUS_TYPE     LinkStatus[OHA_CFG_MAX_PORTS];

    SOCK_RQB_ARGS_SET_ADDR_INFO_TYPE SetAddrInfo;  /* requested adressparams        */
    LSA_UINT16                       SetMib2_PendingCnt[OHA_CFG_MAX_PORTS]; /* Number of pend. SetMIB2.Req */
    LSA_BOOL                         SetMib2_IsActual[OHA_CFG_MAX_PORTS];   /* SetMib2Params is updated */

    OHA_EDD_RQB_LINK_TYPE            LowerDcpFilterRQBs; /* Request-Control-blocks for   */
                                     /* EDD-Requests EDD_SRV_NRT_SET_DCP_FILTER (Pending) */
    LSA_UINT32                       SetFilter_PendingCnt; /* Number of pending DCP SET-FILTER-RQBs */
	LSA_BOOL                         SetFilter_IsActual; /* DCP-Set (Filter) is updated or not */

    OHA_HDB_USER_PTR_TYPE            pHDBUserPrm;     /* user-control-channel for EDD_SRV_SET_REMOTE_STATE  */
    EDD_RQB_SET_REMOTE_PORT_STATE_TYPE LastRemotePortState[OHA_CFG_MAX_PORTS];/* for EDD_SRV_SET_REMOTE_STATE */

    LSA_UINT16                       Set_PendingCnt[OHA_CFG_MAX_PORTS]; /* Number of pend. EDD-Set RQBs */
    LSA_BOOL                         Set_IsActual[OHA_CFG_MAX_PORTS];   /* EddSetRemotePortState updated */

	LSA_UINT32                       Mib2IfIndex[OHA_CFG_MAX_PORTS+1];  /* 0 ... interface */
    LSA_UINT16                       PendingPortId;   /* PortId for SOCK_OPC_GET_MIBII_PARAMS */
} OHA_HDB_EDD_TYPE;


/* --------------------------------------------------------------------------*/
/* For MRP (Upper-)Channel                                                   */
/*                                                                           */
/* For handling all MRP-Requests from MRP to OHA                             */
/* --------------------------------------------------------------------------*/

/* LowerLayer (MRP) Request management type */
#ifndef OHA_MRP_RQB_PTR_TYPE
#define OHA_MRP_RQB_PTR_TYPE          MRP_UPPER_RQB_PTR_TYPE
#endif

#ifndef OHA_CFG_NO_MRP
typedef struct oha_mrp_rqb_link_tag
{
    OHA_MRP_RQB_PTR_TYPE          pBottom;
    OHA_MRP_RQB_PTR_TYPE          pTop;
} OHA_MRP_RQB_LINK_TYPE;

typedef struct oha_mrp_rqb_link_tag  OHA_LOCAL_MEM_ATTR * OHA_MRP_RQB_LINK_PTR_TYPE;
#endif

typedef struct oha_hdb_mrp_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_MRP_PTR_TYPE;

typedef struct oha_hdb_mrp_tag
{
    LSA_UINT16              State;        /* handle state                  */
    LSA_HANDLE_TYPE         ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE  Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE  pOCReq;       /* Open or Close Request running */
										  /* this NARE-channel.            */
    LSA_UINT16              OpenResponse; /* Response from Open if error   */
										  /* occurred while opening.       */
    LSA_UINT16              UserCnt;      /* Number of User-Channels using */
										  /* this EDD-channel.             */
    LSA_HANDLE_TYPE         MrpHandle;    /* If Open the lower NARE handle */
    LSA_UINT32              MrpRQBCnt;    /* Number of pending MRP RQBs    */
#ifndef OHA_CFG_NO_MRP
    OHA_MRP_RQB_LINK_TYPE   MrpRQBs;      /* Request-Control-blocks for MRP*/
#endif
    LSA_UINT32              RxPendingCnt; /* Number of pending MRP RCV's   */
#ifndef OHA_CFG_NO_MRP
	MRP_OPC_RINGSTATE_INDICATION_TYPE MrpStateInd[EDD_CFG_MAX_MRP_INSTANCE_CNT];
	MRP_GET_STATUS_TYPE     MrpGetStatus[EDD_CFG_MAX_MRP_INSTANCE_CNT];
	MRP_SET_STATUS_TYPE     MrpSetStatus[EDD_CFG_MAX_MRP_INSTANCE_CNT];
#endif
} OHA_HDB_MRP_TYPE;


/* --------------------------------------------------------------------------*/
/* For REMA (Upper-)Channel                                                  */
/*                                                                           */
/* For handling all REMA-Requests from ESD to OHA                            */
/* --------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
// REMA-STORE
//----------------------------------------------------------------------------*/
typedef struct oha_rema_prm_record_tag
{
    OHA_LIST_ENTRY_TYPE  link; /* link to next entry */

    OHA_PRM_DATA_TYPE  rec;

} OHA_PRM_RECORD_TYPE;

typedef OHA_PRM_RECORD_TYPE OHA_UPPER_MEM_ATTR * OHA_PRM_RECORD_PTR_TYPE;


typedef struct oha_hdb_rema_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_REMA_PTR_TYPE;

typedef struct oha_hdb_rema_tag
{
    LSA_UINT32          this_idx;     /* Index of this element in array  */

    LSA_UINT16          rema_state; /* see prm-state */

    OHA_LIST_ENTRY_TYPE set_b; /* PDEV records of type: OHA_PRM_RECORD_TYPE */

    /* note: we keep the record set A in memory. this is not a must. */
    OHA_LIST_ENTRY_TYPE set_a; /* PDEV records of type: OHA_PRM_RECORD_TYPE */

    /* we store the last indicated PDEV rema data and indicate only the changes. */
    OHA_REMA_DATA_TYPE  PDEV_Last_Ind;

    /* we store the last indicated rema station data and indicate the changes. */
    OHA_REMA_DATA_TYPE  Station_Last_Ind;

    /* we store the last indicated rema NameOfStation data and indicate the changes. */
    OHA_REMA_DATA_TYPE  NameOfStation_Last_Ind;

    /* we store the last indicated rema IPSuite data and indicate the changes. */
    OHA_REMA_DATA_TYPE  IPSuite_Last_Ind;

    /* we store the last indicated rema DHCP data and indicate the changes. */
    OHA_REMA_DATA_TYPE  DHCPData_Last_Ind;

    /* we store the last indicated rema SNMP data and indicate the changes. */
    OHA_REMA_DATA_TYPE  Snmp_Last_Ind;

    /* we store the last indicated rema BUS data and indicate the changes. */
    OHA_REMA_DATA_TYPE  Bus_Last_Ind;

    /* LSA_TRUE: the user has locked the rema-indication */
    LSA_BOOL            Ind_Locked[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: the user got the PDEV rema-indication */
    LSA_BOOL            PDEV_Ind_Done[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: the user got the Station rema-indication */
    LSA_BOOL            Station_Ind_Done[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: he user got the NameOfStation rema-indication */
    LSA_BOOL            NameOfStation_Ind_Done[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: the user got the IPSuite rema-indication */
    LSA_BOOL            IPSuite_Ind_Done[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: the user got the DHCP rema-indication */
    LSA_BOOL            DHCPData_Ind_Done[OHA_CFG_MAX_CHANNELS];

    /* LSA_TRUE: the user got the SNMP rema-indication */
    LSA_BOOL            Snmp_Ind_Done[OHA_CFG_MAX_CHANNELS];
	
    /* LSA_TRUE: the user got the BUS rema-indication */
    LSA_BOOL            Bus_Ind_Done[OHA_CFG_MAX_CHANNELS];
} OHA_HDB_REMA_TYPE;

/* --------------------------------------------------------------------------*/
/* For "GSY" (Upper-)Channel                                                 */
/*                                                                           */
/* For handling all "GSY"-Requests from CM to OHA                            */
/* --------------------------------------------------------------------------*/

#define OHA_ZERO_IP                 "\0\0\0\0"
#define OHA_ZERO_IP_SUITE           "\0\0\0\0\0\0\0\0\0\0\0\0"
#define OHA_ZERO_MAC                "\0\0\0\0\0\0"
#define OHA_ZERO_UUID               "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

#define OHA_SYNC_PERIOD_VALID_MASK          0x80000000L

/* Mode: */
#define OHA_SYNC_ADD                 1  /* Add SyncData         */
#define OHA_SYNC_REMOVE              2  /* Remove SyncData      */

typedef struct oha_sync_data_tag OHA_LOCAL_MEM_ATTR * OHA_SYNC_DATA_PTR_TYPE;

typedef struct oha_sync_data_tag
{
    LSA_UINT16      Mode;                /* write/remove TLV, see above   */
    LSA_UINT16      InterfaceID;
    LSA_UINT32      RecordIndex;         /* RTA or RTC (PdSyncData)       */
    OHA_UUID_TYPE   SubdomainID;         /* := 0 if no sync record exists */
    OHA_UUID_TYPE   IRDataID;            /* := 0 if no sync record exists */
    OHA_MAC_TYPE    MasterSourceAddress; /* := 0 if not sync              */
    LSA_UINT16      PTCPTimeoutFactor;
    LSA_UINT32      SyncSendFactor;      /* The time base is 31,25 µs     */
} OHA_SYNC_DATA_TYPE;

typedef struct oha_hdb_gsy_tag  OHA_LOCAL_MEM_ATTR *   OHA_HDB_GSY_PTR_TYPE;

typedef struct oha_hdb_gsy_tag
{
    OHA_ADJUST_TYPE     AdjustData[OHA_CFG_MAX_PORTS];
    OHA_SYNC_DATA_TYPE  SyncDataWrite; /* written by PrmWrite (temp)    */
    OHA_SYNC_DATA_TYPE  SyncDataCommit;/* set valid by PrmCommit (valid)*/
    /* red, oramge, green (phase)    */
    OHA_FRAME_OFFSET_TYPE FrameOffsetValid[OHA_CFG_MAX_PORTS]; /* valid */
} OHA_HDB_GSY_TYPE;

/* --------------------------------------------------------------------------*/
/* For DCP Channel                                                           */
/*                                                                           */
/* For handling all DCP-Request from OHA to DCP                              */
/* --------------------------------------------------------------------------*/

/* LowerLayer (DCP) Request management type */
#ifndef OHA_DCP_RQB_PTR_TYPE
#define OHA_DCP_RQB_PTR_TYPE          DCP_UPPER_RQB_PTR_TYPE
#endif

/* Dcp-States for frame filter request : */
#define OHA_DCP_STATE_FILTER_STOP           1  /* no filtering of frames      */
#define OHA_DCP_STATE_FILTER_WF_STOP        2  /* (RE)SET_FILTER is active    */
#define OHA_DCP_STATE_FILTER_START          3  /* frame-filter is active      */
#define OHA_DCP_STATE_FILTER_WF_START       4  /* SET_FILTER is active        */

/* Query states for a remote DCP Set: */
#define OHA_DCP_STATE_QUERY_STOP       1  /* initial, after OpenDcpChannel       */
#define OHA_DCP_STATE_QUERY_SCAN       2  /* a scan of the frame is active       */
#define OHA_DCP_STATE_QUERY_USER       3  /* Query user is active by a DcpControl(QueryWrite) */
#define OHA_DCP_STATE_QUERY_IP         4  /* QueryIp is on the ethernet active   */
#define OHA_DCP_STATE_QUERY_NAME       5  /* QueryName is on the ethernet active */
#define OHA_DCP_STATE_QUERY_BOTH       6  /* both queries are on the ethernet    */

/* Dcp-States for Hello request: */
#define OHA_DCP_STATE_HELLO_WRITTEN    1  /* a DbWriteDeviceInitiative is done */
#define OHA_DCP_STATE_HELLO_IS_TO_SEND 2  /* send a Hello.Req by (next) LinkUp */
#define OHA_DCP_STATE_HELLO_SENT       3  /* the Hello.Req is sent             */

/* Fast StartUp */

typedef struct oha_hello_data_tag OHA_LOCAL_MEM_ATTR * OHA_HELLO_DATA_PTR_TYPE;

typedef struct oha_hello_data_tag
{
    LSA_UINT32  HelloMode;     /* 0 = OFF, 1 = Send DCP_Hello.req on; LinkUp, 2 = Send after HelloDelay  */
    LSA_UINT32  HelloInterval; /* Wait this time after the first DCP_Hello.req before conveying a 2nd DCP_Hello.req */
    LSA_UINT32  HelloRetry;    /* Number of retransmission of the Hello.req */
    LSA_UINT32  HelloDelay;    /* Wait this time after the first LinkUp.ind before conveying a DCP_Hello.req  */
} OHA_HELLO_DATA_TYPE;

typedef struct oha_dcp_rqb_link_tag  OHA_LOCAL_MEM_ATTR *    OHA_DCP_RQB_LINK_PTR_TYPE;

typedef struct oha_dcp_rqb_link_tag
{
    OHA_DCP_RQB_PTR_TYPE          pBottom;
    OHA_DCP_RQB_PTR_TYPE          pTop;
} OHA_DCP_RQB_LINK_TYPE;


typedef struct oha_hdb_dcp_tag OHA_LOCAL_MEM_ATTR *   OHA_HDB_DCP_PTR_TYPE;

typedef struct oha_hdb_dcp_tag
{
    LSA_UINT16                  DcpState;     /* handle state                  */
    LSA_HANDLE_TYPE             ThisHandle;   /* this handle                   */
    OHA_HANDLE_PARAMS_TYPE      Params;       /* general handle parameters     */
    OHA_UPPER_RQB_PTR_TYPE      pOCReq;       /* Open or Close Request running */
    LSA_UINT16                  OpenResponse; /* Response from Open if error   */
    LSA_UINT16                  UserCnt;      /* Number of User-Channels using */
    /* this DCP-channel.             */
    LSA_UINT16                  RequestState; /* request state (sending data)  */
    LSA_HANDLE_TYPE             DcpHandle;    /* If Open the lower DCP handle. */
    LSA_UINT32                  UserRQBCnt;   /* Number of pending User-RQBs   */
    OHA_USER_RQB_LINK_TYPE      UserRQBs;     /* Pending UserRQ-Blocks         */
    /* ------------------------------*/
    LSA_UINT32                  RxPendingCnt; /* Number of pending DCP Receive-RQBs */
    LSA_UINT32                  TxPendingCnt; /* Number of pending DCP Send-RQBs    */
    OHA_DB_IF_PTR_TYPE          pDB;          /* pointer to database-interface */
    OHA_HDB_USER_PTR_TYPE       pHDBUserCtrl; /* user-control-channel-LDB      */
    LSA_UINT16                  QueryState;   /* request state (user query)    */
    OHA_DCP_LOWER_RQB_PTR_TYPE  pLowerDcpSetRqb; /* DcpSet to respond          */
    OHA_UPPER_RQB_PTR_TYPE      pUpperRespRqb;/* USER-ResponseRQB(Rsp running) */
    OHA_DCP_INDICATION_TYPE     CheckSetInd;  /* options of a DcpSet           */
    OHA_DCP_INDICATION_TYPE     LastSet;      /* last changes of a DcpSet      */

    LSA_UINT16                  HelloState;   /* request state (Hello.Req)     */
    OHA_HELLO_DATA_TYPE         FSU_DataWrite; /* FSU written by PrmWrite (temp)*/
    OHA_HELLO_DATA_TYPE         FSU_DataCommit;/* set valid by PrmCommit (valid)*/

} OHA_HDB_DCP_TYPE;

/* --------------------------------------------------------------------------*/
/* global OHA-management (handle independend)                                */
/* --------------------------------------------------------------------------*/

/* one timer for cyclic ResolveAddress and                                   */
/* further user timer for delaying PTCP diagnosis                            */
#define OHA_CFG_MAX_TIMER (1 + 1 + OHA_CFG_MAX_CHANNELS * OHA_CFG_MAX_PORTS)


/* main-managment of OHA */
typedef struct oha_glob_tag
{
    OHA_TIMER_INFO_TYPE   Timer[OHA_CFG_MAX_TIMER];    /* Timermanagement     */
    LSA_UINT16            CyclicTimerID;               /* Timer_ID of cylcic  */
    /* 1s timer.           */
    LSA_UINT32            CyclicTimerCnt;              /* TimerCounter        */
    LSA_UINT32            CyclicTimerUserCnt;          /* number of users of  */
    /* the cyclic timer    */
    LSA_UINT16            UsedUserCnt;     /* currently used user-channels    */
    /* currently used Lower-channels   */
    LSA_UINT16            UsedLowerCnt;    /* one per NIC => NicCount         */
    /* N-If: global interface NIC (for building the lldpLocChassisID) */
    LSA_UINT16            IfNicID;
} OHA_GLOB_TYPE;

typedef struct oha_glob_tag  OHA_LOCAL_MEM_ATTR *   OHA_GLOB_PTR_TYPE;


/* --------------------------------------------------------------------------*/
/* global OHA-Data                                                           */
/* --------------------------------------------------------------------------*/

typedef struct oha_data_tag
{
#if 0
    /* main-managment of OHA */
    LSA_UINT16            UsedUserCnt;     /* currently used user-channels    */
    /* currently used Lower-channels   */
    LSA_UINT16            UsedLowerCnt;    /* one per NIC => NicCount         */
#endif
    OHA_GLOB_TYPE         Glob;                         /* Global Mgm.        */

    OHA_HDB_GSY_TYPE      HGsy[OHA_CFG_MAX_NICS];       /* GSY -Handle Mgm    */
    OHA_HDB_MRP_TYPE      HMrp[OHA_CFG_MAX_NICS];       /* MRP -Handle Mgm    */
    OHA_HDB_REMA_TYPE     HRema[OHA_CFG_MAX_NICS];      /* REMA-Handle Mgm    */
    OHA_HDB_USER_TYPE     HUser[OHA_CFG_MAX_CHANNELS];  /* USER-Handle Mgm    */
                                           /* we have only one SOCK channel!  */
    OHA_HDB_SOCK_TYPE     HSock;                        /* SOCK-Handle Mgm    */

#ifdef OHA_CFG_USE_SNMPX                   /* we have only one SNMPX channel! */
    OHA_HDB_SNMPX_TYPE    HSnmpx;                       /* SNMPX-Handle Mgm   */
#endif
    OHA_HDB_EDD_TYPE      HEdd[OHA_CFG_MAX_NICS];       /* EDD -Handle Mgm    */
    OHA_HDB_DCP_TYPE      HDcp[OHA_CFG_MAX_NICS];       /* DCP -Handle Mgm    */
#ifndef OHA_CFG_NO_NARE
    OHA_HDB_NARE_TYPE     HNare[OHA_CFG_MAX_NICS];      /* NARE-Handle Mgm    */
#endif
    OHA_HDB_LLDP_TYPE     HLldp[OHA_CFG_MAX_NICS];      /* LLDP-Handle Mgm    */
} OHA_DATA_TYPE;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

extern OHA_DATA_TYPE    OHA_Data;               /* global OHA-ROOT-Structure */

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

#define OHA_GET_GLOB_PTR()    (&(OHA_Data.Glob))
#define OHA_GET_HUSER_PTR(_x) (&(OHA_Data.HUser[_x]))
#define OHA_GET_HSOCK_PTR(  ) (&(OHA_Data.HSock))
#define OHA_GET_HSNMPX_PTR()  (&(OHA_Data.HSnmpx[0]))
#define OHA_GET_HLLDP_PTR(_x) (&(OHA_Data.HLldp[_x]))
#define OHA_GET_HNARE_PTR(_x) (&(OHA_Data.HNare[_x]))
#define OHA_GET_HDCP_PTR(_x)  (&(OHA_Data.HDcp[_x]))
#define OHA_GET_HEDD_PTR(_x)  (&(OHA_Data.HEdd[_x]))
#define OHA_GET_HMRP_PTR(_x)  (&(OHA_Data.HMrp[_x]))
#define OHA_GET_HGSY_PTR(_x)  (&(OHA_Data.HGsy[_x]))

#define OHA_GET_HREMA_PTR(_x) (&(OHA_Data.HRema[_x]))

/*===========================================================================*/
/* Handle-handling                                                           */
/*===========================================================================*/

/* Coding of Handle-Variable:                                        */
/* we have six types of handle. A SOCK, DCP, EDD, NARE, LLDP, MRP and a USER-Handle. */
/* 128 User-Channels (0x00 ... 0x7F), 8 * 16 Lower-Channels */


#define OHA_HANDLE_TYPE_MSK   0xF0     /* Bit 4..7 selects the Type of lower handle */
#define OHA_HANDLE_VALUE_MSK  0x0F     /* Bit 0..3 the lower handle value itself    */

#define OHA_HANDLE_TYPE_USER  0x00
#define OHA_HANDLE_TYPE_NARE  0x80
#define OHA_HANDLE_TYPE_DCP   0x90
#define OHA_HANDLE_TYPE_LLDP  0xA0
#define OHA_HANDLE_TYPE_SOCK  0xB0
#define OHA_HANDLE_TYPE_SNMPX 0xC0
#define OHA_HANDLE_TYPE_EDD   0xD0
#define OHA_HANDLE_TYPE_MRP   0xE0
#define OHA_HANDLE_TYPE_SYS   0xF0

#define OHA_HANDLE_GET_TYPE(Handle)  (((Handle)<0x80) ? OHA_HANDLE_TYPE_USER : ((Handle) & OHA_HANDLE_TYPE_MSK))

#define OHA_MAX_HANDLE_IDX_LOWER     16   /* 4 bits */
#define OHA_MAX_HANDLE_IDX_UPPER     128  /* 7 bits */

#define OHA_HANDLE_IDX(Handle)       (((Handle)<0x80) ? (Handle) : ((Handle)&OHA_HANDLE_VALUE_MSK))

#define OHA_HANDLE_IS_USER(Handle)   ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_USER) ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_SOCK(Handle)   ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_SOCK) ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_SNMPX(Handle)  ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_SNMPX) ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_DCP(Handle)    ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_DCP)  ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_EDD(Handle)    ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_EDD)  ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_NARE(Handle)   ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_NARE) ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_LLDP(Handle)   ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_LLDP) ? LSA_TRUE:LSA_FALSE)
#define OHA_HANDLE_IS_MRP(Handle)    ((OHA_HANDLE_GET_TYPE(Handle) == OHA_HANDLE_TYPE_MRP)  ? LSA_TRUE:LSA_FALSE)

#define OHA_EDD_TIMESTAMP_SUPPORTED(_hw_ytpe) ((_hw_ytpe & EDD_HW_TYPE_FEATURE_DELAY_MSK) == EDD_HW_TYPE_FEATURE_TIMESTAMP_SUPPORT)
#define OHA_EDD_FRAGMENTATION_SUPPORTED(_hw_ytpe) ((_hw_ytpe & EDD_HW_TYPE_FEATURE_FRAGMENTATION_MSK) == EDD_HW_TYPE_FEATURE_FRAGMENTATION_SUPPORT)
#define OHA_EDD_PREAMBLE_SHORTENING_SUPPORTED(_hw_ytpe) ((_hw_ytpe & EDD_HW_TYPE_FEATURE_PREAMBLE_SHORTENING_MSK) == EDD_HW_TYPE_FEATURE_PREAMBLE_SHORTENING_SUPPORT)

/*===========================================================================*/
/*                           Access to network bytes                         */
/*===========================================================================*/


/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  OHA_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define OHA_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block)            \
{                                                               \
    OHA_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);                 \
    OHA_RQB_SET_PREV_RQB_PTR(_Block, _Top);                     \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        OHA_RQB_SET_NEXT_RQB_PTR(_Top, _Block);                 \
        _Top        = _Block;                                   \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:  OHA_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define OHA_RQB_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)         \
{                                                               \
    OHA_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);                  \
    OHA_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);                 \
                                                                \
    if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
    {                                                           \
        _Bottom = _Block;                                       \
        _Top    = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        OHA_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);              \
        _Bottom         = _Block;                               \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   OHA_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block)            \
{                                                               \
    _Block = _Top;                                              \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = OHA_RQB_GET_PREV_RQB_PTR(_Top);                  \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            OHA_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);           \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   OHA_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)         \
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = OHA_RQB_GET_NEXT_RQB_PTR((_Bottom));          \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            OHA_RQB_SET_PREV_RQB_PTR((_Bottom), LSA_NULL);      \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   OHA_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)       \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
                                                                \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
              (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
            _Block = OHA_RQB_GET_NEXT_RQB_PTR(_Block);          \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                OHA_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block); \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    OHA_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block); \
                }                                               \
                else                                            \
                {                                               \
                    OHA_RQB_SET_NEXT_RQB_PTR(OHA_RQB_GET_PREV_RQB_PTR(_Block),OHA_RQB_GET_NEXT_RQB_PTR(_Block)); \
                    OHA_RQB_SET_PREV_RQB_PTR(OHA_RQB_GET_NEXT_RQB_PTR(_Block),OHA_RQB_GET_PREV_RQB_PTR(_Block)); \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   OHA_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)    \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
                                                                    \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = OHA_RQB_GET_NEXT_RQB_PTR(_Block);              \
                                                                    \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}                                                                   \

/*=============================================================================
 * function name:   OHA_RQB_SEARCH_QUEUE_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      OHA_UPPER_RQB_PTR_TYPE         pRQB 
 *                  LSA_HANDLE_TYPE                Handle
 * return value:    LSA_VOID
 *
 * pRQB             points to start of RQB-queue (maybe NULL) 
 * Handle           Handle to search in RQBs
 *
 * Description:     On return pRQB points to NULL if no entry was found, else 
 *                  to the RQB with the spezified Handle. On entry pRQB may
 *                  already be NULL.
 *===========================================================================*/

#define OHA_RQB_SEARCH_QUEUE_HANDLE(pRQB,Handle)                        \
    {                                                                   \
        while   ((! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&          \
                ( OHA_RQB_GET_HANDLE(pRQB) != Handle ))                 \
        {                                                               \
            pRQB = OHA_RQB_GET_NEXT_RQB_PTR(pRQB);                      \
        }                                                               \
    }                                                                   \


/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  OHA_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define OHA_PUT_BLOCK_TOP(_Bottom, _Top, _Block)                \
{                                                               \
                                                                \
    _Block->pNext = LSA_NULL;                                   \
    _Block->pPrev = _Top;                                       \
                                                                \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
    {                                                           \
        _Top    = _Block;                                       \
        _Bottom = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Top->pNext = _Block;                                   \
        _Top        = _Block;                                   \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:  OHA_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define OHA_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)             \
{                                                               \
    _Block->pNext = _Bottom;                                    \
    _Block->pPrev = LSA_NULL;                                   \
                                                                \
    if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
    {                                                           \
        _Bottom = _Block;                                       \
        _Top    = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Bottom->pPrev  = _Block;                               \
        _Bottom         = _Block;                               \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   OHA_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_REM_BLOCK_TOP(_Bottom, _Top, _Block)                \
{                                                               \
    _Block = _Top;                                              \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = _Top->pPrev;                                     \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            _Top->pNext = LSA_NULL;                             \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   OHA_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)             \
{                                                               \
    _Block = _Bottom;                                           \
                                                                \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = _Bottom->pNext;                               \
                                                                \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            _Bottom->pPrev = LSA_NULL;                          \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   OHA_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_REM_BLOCK(_Bottom, _Top, _Handle, _Block)           \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
                                                                \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
              (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
            _Block = _Block->pNext;                             \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                OHA_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);    \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    OHA_REM_BLOCK_TOP(_Bottom, _Top, _Block);   \
                }                                               \
                else                                            \
                {                                               \
                    _Block->pPrev->pNext = _Block->pNext;       \
                    _Block->pNext->pPrev = _Block->pPrev;       \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   OHA_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define OHA_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)        \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
                                                                    \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = (_Block)->pNext;                               \
                                                                    \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}                                                                   \


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* oha_snmp.c */

LSA_EXTERN LSA_UINT16 oha_lldp_objid_to_asn1(
    LSA_UINT8* data, 
    LSA_UINT* datalength);

LSA_EXTERN LSA_UINT16 oha_asn1_to_objid(
    LSA_UINT8 * data,
    LSA_UINT    datalength,
    OHA_OID *   objid,
    LSA_UINT *  objidlength);

LSA_EXTERN LSA_UINT16 oha_snmp_count_if_ports(LSA_VOID);

LSA_EXTERN LSA_UINT oha_snmp_get_if_id(LSA_UINT lldpPortNumber, LSA_UINT *port_id);

LSA_EXTERN OHA_DB_IF_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_GetIf(
    LSA_UINT16 IfIndex);

/* OHA_db.c */

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeDataBase(LSA_VOID);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_init_defvals(OHA_DB_IF_PTR_TYPE pDB);

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR  oha_init_stats_info(OHA_OID mib_node);

LSA_EXTERN LSA_UINT16 oha_update_supported_database(
    LSA_UINT16               NicId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetConfig(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetAddress(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestAddressIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AddressIndicateEvent(
    LSA_UINT16              NICId,
    OHA_HDB_USER_PTR_TYPE   pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_WriteAddrInfo(
    LSA_UINT16                              NICId,
    OHA_UPPER_ADDR_INFO_PTR_TYPE            pAddress);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_AddressIndicateEventAllUser(
    LSA_UINT16              NICId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSetAddress(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoSetAddressRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoWriteAddressRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoRtfRequest(
   LSA_UINT16               NICId,
   LSA_UINT16               CheckInterval);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSetIdent(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoSetIdentRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT8 oha_db_read_set_access(
    LSA_UINT16  NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType);

LSA_EXTERN  LSA_UINT8 oha_db_read_get_access(
    LSA_UINT16  NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType);

LSA_EXTERN LSA_BOOL oha_db_read_query_set_access(
    LSA_UINT16 NicId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType);

LSA_EXTERN LSA_BOOL oha_db_compare_remanence(
    LSA_UINT16 NicId,
    LSA_UINT8  Type,
    LSA_UINT8  SubType,
    LSA_UINT16 Status);

LSA_EXTERN LSA_UINT16 oha_db_write_remanence(
    LSA_UINT16 NicId,
    LSA_UINT8  Type,
    LSA_UINT8  SubType,
    LSA_BOOL   RemaStatus);

LSA_EXTERN LSA_UINT16 OHA_DbWriteUserControl(
    LSA_UINT16  NicId,
    LSA_UINT16  UserHandle,
    LSA_UINT8   Type,       /* type of variable */
    LSA_UINT8   SubType,    /* type of variable */
    LSA_UINT8   UserControl); /* user control for variable */

LSA_EXTERN LSA_BOOL oha_db_read_user_lock(
    LSA_UINT16 NicId,
    LSA_UINT16  UserHandle,
    LSA_UINT8   Type,
    LSA_UINT8   SubType);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_temp_ip_address(
    LSA_UINT16                     NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                        AddrLen);

LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_ip_address(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,
    LSA_BOOL                      * pRemanent );

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_expected_ip_suite(
    LSA_UINT16                     NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                        AddrLen);

LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_expected_ip_suite(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,
    LSA_BOOL                      * pRemanent );

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_expected_ip_suite(
    LSA_UINT16                    NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_emergency_ip_suite(
    LSA_UINT16                     NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr,
    LSA_INT                        AddrLen);

LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_emergency_ip_suite(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pAccCtrl,
    LSA_BOOL                      * pRemanent );

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_emergency_ip_suite(
    LSA_UINT16                    NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_set_real_ip_suite(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE  pIpAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubnetAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE  pRouterAddr);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_real_ip_suite(
    LSA_UINT16                    NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_read_ip_conflict_status(
    LSA_UINT16                  NicId,
    LSA_BOOL *                  ConflictStatus);

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_profibus_params(
    LSA_UINT16                    NicId);

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_write_ip_conflict_status(
    LSA_UINT16                  NicId,
    LSA_BOOL                    ConflictStatus);

LSA_EXTERN LSA_VOID OHA_SYSTEM_IN_FCT_ATTR oha_db_write_factory_reset_status(
    LSA_UINT16                  NicId,
    LSA_BOOL                    ConflictStatus);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_temp_station_name(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* NamePtr,
    LSA_UINT16                    NameLen);

LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  oha_db_read_temp_station_name(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * NamePtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE          pAccCtrl,
    LSA_BOOL                          * pRemanent );

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_temp_station_name(
    LSA_UINT16                  NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_delete_station_name(
    LSA_UINT16                    NicId);

LSA_EXTERN LSA_UINT16 oha_db_update_variable(
    OHA_DB_ELEM_PTR_TYPE        pElement,
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,
    LSA_UINT16                  Length);

LSA_EXTERN LSA_UINT16 oha_db_write_variable(
    OHA_COMMON_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppElement,
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,
    LSA_UINT16                  Length);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UnlockDbUser(
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_temp_ip_address(
    LSA_UINT16                  NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_db_store_rema_ip_address(
    LSA_UINT16                     NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    IpSuitePtr);

LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rema_ip_address(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpSuitePtrPtr);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rema_ip_address(
    LSA_UINT16                  NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_update_rema_ip_address(
    LSA_UINT16                  NicId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_snmp_variable (
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) * pAnchor,
    OHA_COMMON_MEM_U8_PTR_TYPE    VarPtr,
    LSA_UINT16                    VarLen,
    LSA_UINT8                     VarType,
	LSA_BOOL                      IsRema);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_snmp_variable (
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) * pAnchor,
    OHA_COMMON_MEM_U8_PTR_TYPE  * pVarPtr,
    OHA_COMMON_MEM_U16_PTR_TYPE   pVarLen,
    OHA_COMMON_MEM_U8_PTR_TYPE    pVarType,
	LSA_BOOL                    * pIsRema);

/* OHA_db_l.c */

LSA_EXTERN LSA_BOOL OHA_LldpIsMib2Object(LSA_UINT8 Type);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadLldpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN OHA_DB_LLDP_ELEM_PTR_TYPE oha_create_lldp_DbElement(
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length);

LSA_EXTERN LSA_UINT16 oha_update_lldp_DbElement(
    OHA_DB_LLDP_ELEM_PTR_TYPE   pElement,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length);

LSA_EXTERN LSA_VOID oha_delete_lldp_DbElement (
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pElement,
    OHA_LOCAL_MEM_U16_PTR_TYPE pCountOfDeletedElem);

LSA_EXTERN OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * oha_get_loc_lldp_db_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,
    LSA_UINT8                   SubType,
    LSA_UINT32                  OUI);        /* OUI, if type=127 */

LSA_EXTERN LSA_UINT16 oha_db_lldp_write_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,
    LSA_UINT8                   SubType,
    LSA_UINT32                  OUI,
    LSA_BOOL                   RemaStatus);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_lldp_read_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,
    LSA_UINT32                  OUI,
    LSA_BOOL                  * RemaStatus);

LSA_EXTERN LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR oha_db_lldp_compare_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,
    LSA_UINT32                  OUI,
    LSA_BOOL					RemaStatus);

LSA_EXTERN OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * oha_get_rem_lldp_db_element(
    LSA_UINT16  NicId,
    LSA_UINT16  PortId,
    LSA_UINT16  SenderId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType,
    LSA_UINT32  OUI);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   Type,        /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  SubType,     /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_loc_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,        /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);     /* length of variable in octets */

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_loc_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI);       /* OUI, if subtype=127 */

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI);       /* OUI, if subtype=127 */

LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_pno_others(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId);

LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_orgspec_others(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId);

LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_unknown_optionals(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId);

LSA_EXTERN LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR oha_db_count_rem_lldp_elements(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT32                  OUI);       /* OUI, if subtype=127 */

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_chassis_name(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,      /* index + 1 to RemoteSender[] - for SNMP */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,      /* subtype of ChassisID, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppChassis,    /* ChassisID in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pChassisLen);  /* length of ChassisID in octets */

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_port_name(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,   /* index + 1 to RemoteSender[] - for SNMP */
	OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,   /* subtype of PortID, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppName,    /* NameOfPort in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLen);      /* length of NameOfPort in octets */

LSA_EXTERN LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR  oha_port_names_valid(
    LSA_UINT16  NicId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadLldpOptions(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestWriteLldpOption(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpAccess(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpConfigIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpConfigResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpPortConfigIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpPortConfigResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

#define oha_create_snmp_DbElement	oha_create_lldp_DbElement
#define oha_update_snmp_DbElement	oha_update_lldp_DbElement
#define oha_delete_snmp_DbElement   oha_delete_lldp_DbElement

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_value(
    LSA_UINT32                  Options,
    OHA_COMMON_MEM_U32_PTR_TYPE *ppValue,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_if_value(
    LSA_UINT16                  NicId,
    LSA_UINT32                  IfOptions,
    OHA_COMMON_MEM_U8_PTR_TYPE  *ppValue,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_port_value(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,
    OHA_COMMON_MEM_U32_PTR_TYPE *ppValue,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_temp_lldp_mib_values(
    LSA_UINT32                   Options,
	OHA_LLDP_IF_PARAM_TYPE OHA_COMMON_MEM_ATTR *pConfig);

LSA_EXTERN LSA_UINT32 OHA_GetGlobQueryOptions(LSA_VOID);

LSA_EXTERN LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_GetPortQueryOptions(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId);

LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_temp_lldp_mib_port_values(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  Options,
	OHA_LLDP_PORT_PARAM_TYPE OHA_COMMON_MEM_ATTR *pConfig);

/* OHA_sys.c */

/* LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserCheckHDB(LSA_VOID);  */

#define oha_db_read_loc_lldp_element OHA_DB_READ_LLDP_ELEMENT

/* OHA_lib.c */

#define OHA_MAC2A_LEN               18
#define OHA_IP2A_LEN                16

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR oha_namecmp(
    OHA_COMMON_MEM_U8_PTR_TYPE name1,
    OHA_COMMON_MEM_U8_PTR_TYPE name2,
    LSA_UINT32 length,
	LSA_BOOL allow_upper_case);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR oha_ipcmp(
    OHA_COMMON_MEM_U8_PTR_TYPE ip1,
    OHA_COMMON_MEM_U8_PTR_TYPE ipmask1,
    OHA_COMMON_MEM_U8_PTR_TYPE gateway1,
    OHA_COMMON_MEM_U8_PTR_TYPE ip2,
    OHA_COMMON_MEM_U8_PTR_TYPE ipmask2,
    OHA_COMMON_MEM_U8_PTR_TYPE gateway2);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_CallCbf(
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE Cbf,
    OHA_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE            sys_ptr);

LSA_EXTERN LSA_VOID OHA_RQBSetResponse(
    OHA_UPPER_RQB_PTR_TYPE    pRQB,
    LSA_UINT16                Response);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UserRequestFinish(
    OHA_HDB_USER_PTR_TYPE      pHDB,
    OHA_UPPER_RQB_PTR_TYPE     pRQB,
    LSA_UINT16                 Status);

LSA_EXTERN OHA_HDB_USER_PTR_TYPE OHA_IsAnyPrmCommitActive(
    LSA_UINT16 NicID);

LSA_EXTERN OHA_HDB_USER_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHA_GetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN OHA_HDB_DCP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetDcpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN OHA_HDB_EDD_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetEddHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN OHA_HDB_SOCK_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN OHA_HDB_AGNT_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHA_SOCKIsAgentActive(LSA_HANDLE_TYPE Agent);

LSA_EXTERN LSA_VOID OHA_SOCKFreeAgent(OHA_HDB_AGNT_PTR_TYPE  pSubAgnt,
    OHA_HDB_SOCK_PTR_TYPE  pHDBSock);

#ifdef OHA_CFG_USE_SNMPX
LSA_EXTERN OHA_HDB_SNMPX_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetSnmpxHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);
#endif

#ifndef OHA_CFG_NO_NARE
LSA_EXTERN OHA_HDB_NARE_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetNareHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelCyclicNare(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);
#endif

LSA_EXTERN OHA_HDB_LLDP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetLldpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_CancelDcpControlUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckDcpControlUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TOPOGetNicByInterface(
    LSA_UINT32 InterfaceID);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_TOPOIsIfRegistered(
    LSA_UINT32 InterfaceID);

LSA_EXTERN LSA_BOOL OHA_IsAnyLocalSetActive(
    LSA_UINT16 NicID);

LSA_EXTERN LSA_BOOL OHA_IsAnyLocalSetMib2Active(
    LSA_UINT16 NicID);

LSA_EXTERN LSA_BOOL OHA_IsAnyLocalCtrlTcIpActive(
    LSA_UINT16 NicID);

LSA_EXTERN LSA_VOID OHA_DoLocalSetAddress(
    LSA_UINT16 NicID);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoDB(
    OHA_DB_TOPO_PTR_TYPE pTopoDataBase);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_InitTopo(
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeAllTopoDBs(
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoPeers(
    OHA_DB_TOPO_PTR_TYPE pTopoDataBase);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyTopoData(
    OHA_DB_TOPO_PTR_TYPE pDestTopoData, OHA_DB_TOPO_PTR_TYPE pSrcTopoData);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyAdjustData(
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy,
    OHA_HDB_USER_PTR_TYPE pHDBUser, LSA_UINT16 PortId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AdjustAdminStatus(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,
    LSA_UINT32 PeerToPeerBoundary);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UpdateFrameOffset(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,
    LSA_BOOL OnlySyncForwarding);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopySyncData(
    OHA_HDB_GSY_PTR_TYPE  pHDBGsy,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_EXTERN LSA_UINT16 oha_db_write_TopoPeer(
    OHA_COMMON_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppElement,
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,
    LSA_UINT16                  Length);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CopyTopoPeersToDB(
    OHA_DB_TOPO_PTR_TYPE        pTopoDB,
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CheckTopoPeers(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT16                  NicID);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckDomainUUID(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckMRP_PortStatus(
    LSA_UINT32 MRP_PortStatus);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckTopoMauType(
    LSA_UINT16                  MauType,
    LSA_BOOL                    allow_non_pnio_mautypes);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetCountOfRealPeers(
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetCountOfInterfaces(LSA_VOID);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetUsedUserCnt(
    LSA_UINT16                  NicID);

LSA_EXTERN OHA_HDB_USER_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_USERIsAgentActive(LSA_HANDLE_TYPE Agent,
																		  LSA_BOOL OHA_COMMON_MEM_ATTR*pIsRema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AllocTopoPortInd(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId);

LSA_EXTERN LSA_UINT16 OHA_GetSpeedFromMauType(
    LSA_UINT16              MAUType);

LSA_EXTERN LSA_UINT16 OHA_GetModeFromMauType(
    LSA_UINT16              MAUType);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetSpeedFromSpeedMode(
    LSA_UINT8                   SpeedMode);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetModeFromSpeedMode(
    LSA_UINT8                   SpeedMode);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetAdvertisedCapFromParams(
	LSA_UINT32				AutonegCapAdvertised);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetLinkStatus(
    LSA_UINT16              PortStatus);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GetLinkStatusChange(
    LSA_UINT16              PrevStatus,
    LSA_UINT16              ActualStatus);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortDelays(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   pLocDelays,
    OHA_UPPER_MEM_U8_PTR_TYPE   pRemDelays);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortStatus(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   pPortStatus);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetChassisMacAdr(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   ChassisMacAdr);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPortStatusMrp(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   pDomainUUID,
    LSA_UINT16                  MrpPortStatus);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPtcpStatus(
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR  *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    OHA_UPPER_MEM_U8_PTR_TYPE   pMasterSourceAddress,
    OHA_UPPER_MEM_U8_PTR_TYPE   pSubdomainUUID,
    OHA_UPPER_MEM_U8_PTR_TYPE   pIRDataUUID,
    OHA_FRAME_OFFSET_PTR_TYPE pFrameOffset);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetChassisPortTTL(
    OHA_UPPER_MEM_PTR_TYPE  OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_SET_INFO_TYPE  OHA_UPPER_MEM_ATTR *pTLVInfo,
	LSA_UINT32                                 MultipleIfMode);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpGetMgmAddr(
    OHA_UPPER_MEM_U8_PTR_TYPE   pValue,
    LSA_UINT32                  VLength,
    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE pMgmAddr);

LSA_EXTERN OHA_COMMON_MEM_U8_PTR_TYPE OHA_TopoGetPeerMAC(
    LSA_UINT16  NICId,
    LSA_UINT16  PortId,
    LSA_UINT16  SenderId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetUnknownOptionals(
    LSA_UINT16                              NicId,
    LSA_UINT16                              PortId,
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE              pTLength);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetPNOSpecifics(
    LSA_UINT16                              NicId,
    LSA_UINT16                              PortId,
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE              pTLength);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetIEEE802_1Extensions(
    LSA_UINT16                              NicId,
    LSA_UINT16                              PortId,
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE              pTLength);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetIEEE802_3Extensions(
    LSA_UINT16                              NicId,
    LSA_UINT16                              PortId,
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE              pTLength);


LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpSetOrgDefOthers(
    LSA_UINT16                              NicId,
    LSA_UINT16                              PortId,
    OHA_UPPER_MEM_PTR_TYPE OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE              pTLength);

LSA_EXTERN LSA_UINT16 oha_write_mrp_port_status(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE   pDomainUUID,
    LSA_UINT16                  RRT_State);

LSA_EXTERN LSA_UINT16 oha_write_ptcp_status(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE   pMasterSourceAddress,
    OHA_UPPER_MEM_U8_PTR_TYPE   pSubdomainUUID,
    OHA_UPPER_MEM_U8_PTR_TYPE   pIRDataUUID,
    OHA_FRAME_OFFSET_PTR_TYPE pFrameOffset);


LSA_EXTERN OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN OHA_HDB_REMA_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_REMAGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN OHA_HDB_GSY_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_GSYGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbStoreLocalTTL(
    LSA_UINT16 NICId,
    LSA_UINT16 TxInterval,
    LSA_UINT16 TxHoldMultiplier);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbGetTxInterval(
    LSA_UINT16 NICId,
    OHA_COMMON_MEM_U16_PTR_TYPE pTxInterval);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DbStoreMgmAddress(
    LSA_UINT16 NICId,
    OHA_COMMON_MEM_U8_PTR_TYPE pIpaddr);

LSA_EXTERN LSA_UINT16 OHA_CalculatePtcpTimeDelay(
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    OHA_COMMON_MEM_U16_PTR_TYPE pPtcpDelay);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_AdminStatusToSendMode(
    LSA_UINT32                  AdminStatus);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR  OhaGetConflictIp(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpAddrPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpMaskPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * GatewayPtrPtr,
    LSA_BOOL                      * pIpIsEmergency);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR  OHA_UserSetAddrPending (
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16 Status);

LSA_EXTERN LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetGlobalIfMode(
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_0,
    OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfMode_1);

/* OHA_usr.c */

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_USERCloseChannelDone(
    OHA_HDB_USER_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_USERChannelDone(
    OHA_HDB_USER_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserGetFreeHDB(
    OHA_HDB_USER_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBUser);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UserReleaseHDB(
    OHA_HDB_USER_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UserChannelStateMachine(
    OHA_HDB_USER_PTR_TYPE  pHDB,
    LSA_UINT16             Response);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDbControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestDcpResponse(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPortControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPtcpWriteData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestGetNeighbour(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestControlSnmpOid(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestWriteDcpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadDcpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

/* OHA_topo.c */

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateEvent(
    OHA_LAST_PRM_IND_DATA_PTR_TYPE          ThisInd,
    OHA_HDB_USER_PTR_TYPE                   pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateEmptyEvent(
    LSA_UINT16                              PortId,
    OHA_HDB_USER_PTR_TYPE                   pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrm(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT16                  PortId,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrmAdjust(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT16                  PortId,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPrmSync(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddIRData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddIfAdjustData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddPnIdentData(
    OHA_UPPER_RQB_PTR_TYPE      pRQB,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestAddFastStartUp(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    LSA_UINT32                  RecordIndex,
    OHA_HDB_USER_PTR_TYPE       pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicatePort(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL                CheckPeers,
    LSA_BOOL                CheckLinkStatus);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TopoIndicateInterface(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL                BuildEmptyEvent);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeTopoPortInd(
    OHA_HDB_USER_PTR_TYPE pHDBUser,
    LSA_UINT16   PortId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Chassis(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Ports(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_MRP_domain(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_None(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_PTCP_Status(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_MAU_Type(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_RT_Class3(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_CableDelay(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare,
    LSA_UINT32          *   pExtCHannelAddValue);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_PreambleLength(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoPeer_Fragmentation(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoDelay(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoSpeed(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoMode(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_Gateway(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_NoS(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_NoD(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoInterface_IP(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_PrepareTopoIndication(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_UINT16              ChannelErrorType,
    LSA_UINT16              ExtCHannelErrorType,
    LSA_UINT16              ChannelSpecifier,
    LSA_UINT32              ExtCHannelAddValue,
    LSA_UINT16              DiagIndex);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TOPORequestHandler(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              NICId,
    LSA_UINT16              Response);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadMrpPrm(
    OHA_UPPER_RQB_PTR_TYPE  pRQB,
    LSA_UINT16              NICId,
    LSA_UINT16              record_index);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateEventAllUsers(
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortId);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_mrp_port_status(
    OHA_HDB_MRP_PTR_TYPE        pHDBMrp,
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortID);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_ptcp_status(
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortID,
    LSA_BOOL                    OnlySyncForwarding);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR oha_update_port_status(
    LSA_UINT16                  NicID,
    LSA_UINT16                  PortID,
    LSA_BOOL                    OnlySyncForwarding);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_IsWirelessPort(
    LSA_UINT16                  NICId,
    LSA_UINT16                  PortID);

LSA_EXTERN LSA_VOID oha_update_sync_status(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TOPOUserCheckPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoRemotePortState(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId);

/* OHA_edd.c */

LSA_EXTERN LSA_VOID   OHA_LOCAL_FCT_ATTR OHA_EDDChannelDone(
    OHA_HDB_EDD_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

/* LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDCloseChannel(    */
/*  OHA_HDB_EDD_PTR_TYPE        pHDB);                              */
LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDCloseChannel(
    LSA_HANDLE_TYPE       EDDHandle,
    LSA_SYS_PTR_TYPE      pSys);
LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDCloseChannelDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

/* LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDOpenChannel(     */
/*  OHA_HDB_EDD_PTR_TYPE        pHDB);                              */
LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);
LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDOpenChannelDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetParams(
    OHA_HDB_EDD_PTR_TYPE        pHDB,
	LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDGetParamsDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDReleaseHDB(
    OHA_HDB_EDD_PTR_TYPE pHDB);

LSA_EXTERN OHA_HDB_EDD_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_EDDGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_EDDCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetFreeHDB(
    OHA_HDB_EDD_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBEdd);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDLinkIndProvide(
    OHA_HDB_EDD_PTR_TYPE     pHDB,
    LSA_UINT16               PortCnt);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDLinkIndDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE      pRQB,
    OHA_HDB_EDD_PTR_TYPE            pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EddIndicateEventAllUsers(
    OHA_HDB_EDD_PTR_TYPE        pHDBEdd,
    LSA_UINT16                  PortId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDGetPortParams(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDGetPortParamsDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDControlSignal(
    LSA_UINT16  NicId,
    LSA_UINT8 Frequency);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDControlSignalDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_EDDIsAtLeastOnePortUp(
    LSA_UINT16                  NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetRemotePortState(
    LSA_UINT16                  NicId,
    EDD_RQB_SET_REMOTE_PORT_STATE_TYPE RemotePortState);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetRemotePortStateDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetArpFilter(
    LSA_UINT16                  NICId,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDDSetIp(
    LSA_UINT16                  NICId,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetArpFilterDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDDSetIpDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_EDD_SetFilter(
    LSA_UINT16	                NICId,
    LSA_UINT8                   Mode,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_EDD_SetFilterDone(
    OHA_EDD_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_EDD_PTR_TYPE        pHDB);

/* OHA_dcp.c */

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPOpenChannelDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_DCPChannelDone(
    OHA_HDB_DCP_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPCancelReceive(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPCancelReceiveDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB);

LSA_EXTERN OHA_HDB_DCP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_DCPGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_DCPCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPGetFreeHDB(
    OHA_HDB_DCP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBDcp);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPReleaseHDB(
    OHA_HDB_DCP_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPRecvDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPSendDone(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPRecvProvide(
    OHA_HDB_DCP_PTR_TYPE     pHDB,
    LSA_UINT32               Cnt,
    LSA_UINT8                ServiceId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_dcp_Get(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_dcp_Identify(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_dcp_Set(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR oha_dcp_Hello (
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE    pEvent);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateCheckIpEvent(
	LSA_UINT16                  NicId);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEventAllUsers(
    OHA_HDB_DCP_PTR_TYPE         pHDBDcp,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE    pEvent);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpIndicateEventAllOtherUsers(
    OHA_UPPER_RQB_PTR_TYPE       pRQB,
    LSA_UINT16                   NicId);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpHelloEventAllUsers(
    OHA_UPPER_HELLO_PTR_TYPE    pEvent,
    LSA_UINT16                  NicId);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR oha_dcp_free_rsp_rqb(
    OHA_HDB_DCP_PTR_TYPE        pHDB,
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR oha_dcp_free_rcv_rqb(
    OHA_HDB_DCP_PTR_TYPE        pHDB,
    OHA_DCP_LOWER_RQB_PTR_TYPE  pRQB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DcpSetAllNics(
    LSA_UINT8                   Mode,
    LSA_UINT32                  Options);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_DCPRequestHandler(
    OHA_HDB_DCP_PTR_TYPE        pHDBDcp,
    OHA_UPPER_RQB_PTR_TYPE      pRQBUser,
    LSA_UINT16                  Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DCPUpdateFilter(
    LSA_UINT16 NICId,
    LSA_UINT32 ControlStatus,
    LSA_UINT16 Orginator);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpWriteAccessControl(
    LSA_UINT16 NicId,
    LSA_UINT32 options,
    LSA_UINT8 control);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpGetReason(LSA_UINT8 reason);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpWriteUserControl(
	LSA_UINT16      NicId,
    LSA_HANDLE_TYPE UserHandle,
    OHA_UPPER_DCP_CONTROL_PTR_TYPE control);

LSA_UINT16 oha_dcp_ParseSet(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd);

LSA_UINT16 oha_dcp_QuerySet(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_QUERY_ELEM_PTR_TYPE      pQueryElem);

LSA_EXTERN OHA_EDD_LOWER_MEM_PTR_TYPE oha_dcp_alloc_FilterTable(
    LSA_UINT16                   NicId,
    LSA_UINT8                    Mode,
    OHA_COMMON_MEM_U16_PTR_TYPE  Entries);

LSA_EXTERN LSA_VOID oha_dcp_free_FilterTable(
    LSA_UINT16                   NicId,
	OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE pFilterSet);

LSA_EXTERN LSA_VOID oha_dcp_confirm_remote_set(
    OHA_HDB_DCP_PTR_TYPE                pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd);

LSA_EXTERN LSA_INT  oha_dcp_HelloReqSet(
    OHA_HDB_DCP_PTR_TYPE    pHDB,
    DCP_USR_RSP_DATA_PTR    DataPtr,
    LSA_INT                 DataLen,
    LSA_INT             *   OptCntPtr);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestHelloIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT8 oha_dcp_HelloOptTcp(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_HELLO_PTR_TYPE pInd);

LSA_EXTERN LSA_UINT8 oha_dcp_HelloOptIdent(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_HELLO_PTR_TYPE pInd);

LSA_EXTERN LSA_UINT8 oha_dcp_HelloOptDevInit(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_COMMON_MEM_U16_PTR_TYPE DevInit);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoDcpResponseRequest(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoDcpSetAddr(
    OHA_HDB_DCP_PTR_TYPE    pHDBDcp);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_FinishDcpResponse(
    OHA_HDB_DCP_PTR_TYPE     pHDBDcp,
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               Response);

LSA_EXTERN LSA_BOOL  OHA_LOCAL_FCT_ATTR  oha_is_valid_dhcp_option(
    LSA_UINT16                  Length,
    OHA_COMMON_MEM_U8_PTR_TYPE  pOption);

/* OHA_lldu.c */

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPUserCheckPending(
    OHA_HDB_LLDP_PTR_TYPE   pHDBLldp,
    LSA_UINT16              Response);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpStartSend(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPUserCheckPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelLldpUser(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

/* OHA_lldp.c */

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_LLDPChannelDone(
    OHA_HDB_LLDP_PTR_TYPE      pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPOpenChannelDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPCloseChannel(
    LSA_HANDLE_TYPE       LLDPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPCloseChannelDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPRecvProvide(
    OHA_HDB_LLDP_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPRecvDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetConfig(
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPGetConfigDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSetGlobals(
    LSA_UINT16              NicId,
    LSA_UINT16              Orginator,
    LSA_UINT32				TxInterval,
	LSA_UINT32				ReInitDelay,
    LSA_UINT32				TxDelay,
    LSA_UINT32				TxHoldMultiplier);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetGlobalsDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_LLDPCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetFreeHDB(
    OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBEdd);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPReleaseHDB(
    OHA_HDB_LLDP_PTR_TYPE pHDB);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSet(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPRequestHandler(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    OHA_UPPER_RQB_PTR_TYPE       pRQBUser,
    LSA_UINT16                   Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStart(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   AdminStatus,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStartAll(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPStartDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPStop(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   AdminStatus,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPStopDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPGetStats(
    OHA_HDB_LLDP_PTR_TYPE        pHDB,
    LSA_UINT16                   PortId,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPGetStatsDone(
    OHA_LLDP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_LLDP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateGlobals(
    LSA_UINT16                   NicID,
	LSA_UINT32                   SetIfOptions,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPSetMibOptions(
	LSA_UINT32                   IfSetOptions,
	LSA_BOOL					 IsRema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPSetTempIfOptions(
    LSA_UINT16                  NicId,
	LSA_UINT32                  IfSetOptions);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicateEvent(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    OHA_HDB_USER_PTR_TYPE        pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicateConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicateEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT16                   PortId);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicateConfigEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LldpIndicatePortConfigEventAllUsers(
    OHA_HDB_LLDP_PTR_TYPE        pHDBLldp,
    LSA_UINT16                   PortId,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateSend(
    LSA_UINT16 NICId,
    LSA_UINT32 ControlStatus,
    LSA_UINT16 Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateSendGlobal(LSA_VOID);

OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByIf(
    LSA_UINT16 IfId);

OHA_HDB_LLDP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_LLDPGetHandleByIndex(
    LSA_UINT16 Index);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadLldpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FreeRemoteSender(
    OHA_LLDP_SENDER_PTR_TYPE    pRemoteSender,
    OHA_LOCAL_MEM_U16_PTR_TYPE  pCountOfDeletedElem);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadQueryElements(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OhaLldpGetParams(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   OhaSenderStatus,
    OHA_UPPER_MEM_PTR_TYPE      OHA_UPPER_MEM_ATTR *ppTLV,
    OHA_UPPER_MEM_U32_PTR_TYPE  pTLength,
    LSA_BOOL *                  pRemoteChanges);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPDoStats(
    LSA_UINT16                  Orginator,
	LSA_UINT8                   MessageType,
	LSA_BOOL                    IsRema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPPrepareStats(
    LSA_UINT16                   Orginator,
	OHA_COMMON_MEM_U16_PTR_TYPE  pCountOfQueries,
	LSA_BOOL                     MakeRema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LLDPUpdateStats(
    OHA_DB_IF_PTR_TYPE          pDB,
    LSA_UINT16                  PORTId,
    LSA_UINT16                  SenderId,
    LSA_UINT16                  Event);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpQueryConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    LSA_UINT32                   Options,
	LSA_BOOL                     MakeRema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpQueryPortConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
	LSA_UINT16					 PortId,
    LSA_UINT32                   Options);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_LldpIndicatePortConfigEvent(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
	LSA_UINT16					 PortId,
    LSA_UINT32                   Options,
    LSA_UINT16                   Orginator);

/* OHA_mrp.c */

#ifndef OHA_CFG_NO_MRP
LSA_EXTERN OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByIf(
    LSA_UINT32 IfId);

LSA_EXTERN OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_MRPGetHandleByIndex(
    LSA_UINT32 Index);

LSA_EXTERN OHA_HDB_MRP_PTR_TYPE  OHA_LOCAL_FCT_ATTR OHAGetMrpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_MRPCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPGetFreeHDB(
    OHA_HDB_MRP_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBMrp);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPReleaseHDB(
    OHA_HDB_MRP_PTR_TYPE pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPOpenChannel(
    LSA_HANDLE_TYPE       OHAHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPOpenChannelDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPCloseChannel(
    LSA_HANDLE_TYPE       MRPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPCloseChannelDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPRecvProvide(
    OHA_HDB_MRP_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPRecvDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPGetStatus(
    OHA_HDB_MRP_PTR_TYPE        pHDB,
	LSA_UINT16                  Orginator,
    LSA_UINT16                  Instance);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPGetStatusDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPSetStatus(
    OHA_HDB_MRP_PTR_TYPE       pHDB,
    LSA_UINT16                  Orginator,
    LSA_UINT16                  Instance);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_MRPSetStatusDone(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_MrpWriteData(
    OHA_MRP_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_MRP_PTR_TYPE        pHDB);

/* OHA_mrpr.c */

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_MRPCountStats(
    LSA_UINT16                   Orginator,
	LSA_UINT8                   MessageType);

LSA_EXTERN LSA_UINT32 oha_snmp_get_mrp_statistics(LSA_VOID);

LSA_EXTERN LSA_VOID oha_snmp_delete_mrp_statistics(LSA_VOID);

#endif

/* OHA_sock.c */

LSA_EXTERN OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleByIf(
    LSA_UINT32 IfId);

LSA_EXTERN OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SOCKGetHandleGlobal(
    LSA_VOID);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKGetFreeHDB(
    OHA_HDB_SOCK_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBSock);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKReleaseHDB(
    OHA_HDB_SOCK_PTR_TYPE pHDB);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_SOCKCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_SOCKChannelDone(
    OHA_HDB_SOCK_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKOpenChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKOpenChannelDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKCloseChannel(
    LSA_HANDLE_TYPE       SOCKHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKCloseChannelDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKRecvProvide(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode,
    LSA_UINT32               Cnt);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKRecvDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKParseDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRxRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPSockCheckPending(
    OHA_HDB_SOCK_PTR_TYPE   pHDBSock,
	OHA_OID                 MibNode,
    LSA_UINT16              Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKOpenPort(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKOpenPortDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2Status(
    LSA_UINT16              NicID,
    LSA_UINT16              PortID,
    LSA_UINT16              Orginator);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKUpdateMib2Params(
    LSA_UINT16              NicID,
    LSA_UINT16              Orginator,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysName,
    LSA_UINT16              SysNameLen,
	LSA_BOOL                SysNameRema,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysContact,
    LSA_UINT16              SysContactLen,
    LSA_BOOL                SysContactRema,
    OHA_UPPER_MEM_U8_PTR_TYPE pSysLocation,
    LSA_UINT16              SysLocationLen,
    LSA_BOOL                SysLocationRema);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2StatusDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2Params(
    LSA_UINT16              NicID,
    LSA_UINT16              Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetMib2ParamsDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKMib2ParamsInd(
    LSA_UINT16              NicID,
    LSA_UINT16              Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKMib2ParamsIndDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKClosePort(
    OHA_HDB_SOCK_PTR_TYPE    pHDB,
	OHA_OID                  MibNode);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKClosePortDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

/* LSA_EXTERN int oha_snmp_agt_parse(u_char*, unsigned, u_char*, unsigned);  */
LSA_EXTERN LSA_INT oha_snmp_agt_parse(
    OHA_OID mib_node, /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    LSA_UINT8*, 
    LSA_UINT, 
    LSA_UINT8*, 
    LSA_UINT, 
    LSA_BOOL*);

LSA_EXTERN LSA_INT oha_snmp_create_ident(LSA_UINT8 *in, LSA_UINT8 *out, LSA_UINT length, LSA_UINT32 errindex);

LSA_EXTERN LSA_UINT8 oha_snmp_msg_parse(
    LSA_UINT8 * data,
    LSA_UINT    length); 

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKRequestHandler(
    OHA_HDB_SOCK_PTR_TYPE       pHDBSock,
    OHA_OID                     mib_node,
    LSA_UINT16                  Response);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestSockControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestControlTcipData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestControlTcipDataDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKUserSetIpPending(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetIpAddress(
    LSA_UINT16                      NicID,
    LSA_UINT16                      Orginator,
    OHA_COMMON_MEM_U8_PTR_TYPE      pIpAddr,
    OHA_COMMON_MEM_U8_PTR_TYPE      pNetMask,
    OHA_COMMON_MEM_U8_PTR_TYPE      pGateWay,
    LSA_BOOL                        CurrentIpSuiteIsEmergency);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKControlTcipData(
    LSA_UINT16                      NicID,
    LSA_UINT16                      Orginator,
	LSA_OPCODE_TYPE                 Opcode,
    LSA_UINT16                      KeepAliveTime);

LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKSetNameAddress(
    LSA_UINT16                      NicID);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKSetIpAddressDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKStartUp(
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKShutDown(
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelSockUser(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_CheckSockUser(
    OHA_HDB_USER_PTR_TYPE         pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SOCKGetMib2Status(
    LSA_UINT16                  NicID,
	LSA_UINT16                  PortID,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKGetMib2StatusDone(
    OHA_SOCK_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SOCK_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_InitAddrInfo(
    LSA_UINT16                  NicID);

/* OHA_SNMX.c */

#ifdef OHA_CFG_USE_SNMPX

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_RequestSnmpxControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN OHA_HDB_SNMPX_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_SNMPXGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXGetFreeHDB(
    OHA_HDB_SNMPX_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBSnmpx);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXReleaseHDB(
    OHA_HDB_SNMPX_PTR_TYPE pHDB);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_SNMPXCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_SNMPXChannelDone(
    OHA_HDB_SNMPX_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXOpenChannel(
    LSA_HANDLE_TYPE       SNMPXHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXOpenChannelDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXCloseChannel(
    LSA_HANDLE_TYPE       SNMPXHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXCloseChannelDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXRecvProvide(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB,
    LSA_UINT32               Cnt);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXOidProvideDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXRecvDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXSendDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXRegisterAgent(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXRegisterAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_SNMPXUnregisterAgent(
    OHA_HDB_SNMPX_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXUnregisterAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXRequestHandler(
    OHA_HDB_SNMPX_PTR_TYPE          pHDBSnmpx,
    OHA_UPPER_RQB_PTR_TYPE          pRQBUser,
    LSA_UINT16                      Response);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_SNMPXControlAgent(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_SNMPX_PTR_TYPE   pHDBSnmpx);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SNMPXControlAgentDone(
    OHA_SNMPX_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_SNMPX_PTR_TYPE        pHDB);

#endif

/* OHA_NARE.C */

#ifndef OHA_CFG_NO_NARE

LSA_EXTERN OHA_HDB_NARE_PTR_TYPE OHA_LOCAL_FCT_ATTR OHA_NAREGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_NARECheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREGetFreeHDB(
    OHA_HDB_NARE_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBNare);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREReleaseHDB(
    OHA_HDB_NARE_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_NAREChannelDone(
    OHA_HDB_NARE_PTR_TYPE       pHDB,
    LSA_UINT16                 Response);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREOpenChannelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECloseChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARECloseChannelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARETestIp(
    LSA_UINT16                   NicID,
    OHA_COMMON_MEM_U8_PTR_TYPE   IpAddrPtr,
    LSA_UINT                     AddrLen,
    LSA_UINT16                   Orginator,
    LSA_UINT32                   RequestID);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestCyclicIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARECheckIpDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN OHA_NARE_LOWER_RQB_PTR_TYPE OHA_NARETestIpCancel(
    OHA_HDB_NARE_PTR_TYPE       pHDBNare,
    LSA_UINT16                  Orginator,
    LSA_UINT32                  RequestID);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NARETestIpCancelDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_FreeCyclicNareRqbs(
    OHA_HDB_USER_PTR_TYPE        pHDBUser);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREHelloSend(
    LSA_UINT16                  NICId,
    LSA_UINT32                  HelloRetry,
    LSA_UINT32                  HelloInterval,
    LSA_UINT32                  HelloDelay);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NAREHelloStop(
    LSA_UINT16                  NICId,
    LSA_UINT16                  Orginator);

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_NAREHelloDone(
    OHA_NARE_LOWER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_NARE_PTR_TYPE        pHDB);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DoHello(
    LSA_UINT16                  NICId,
    LSA_UINT16                  AdditionalDelay);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_NARECheckAddress(
    LSA_UINT16                  NicID,
    LSA_UINT32                  CheckOptions,
    LSA_UINT32                  Options,
    LSA_UINT16                  CheckInterval);

LSA_EXTERN LSA_BOOL OHA_LOCAL_FCT_ATTR OHA_NARECheckAddressCancel(
    LSA_UINT16                  NicID,
    LSA_UINT32                  CheckOptions,
    LSA_UINT16                  Orginator);
#endif

/* OHA_PRM.C */

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmPrepare(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_DoPrmPrepare(
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmWrite(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWritePDEV(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteBUS(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteSNMPControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteLLDP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmControlTCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmWriteDHCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoPrmWriteStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_DoPrmWriteSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmEnd(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmCommit(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_PrmCommit(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmRead(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPrmIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdPortData(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_BOOL                 IsWireless);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdPortDataAdjust(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdSyncData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_PdIRData(
    OHA_HDB_USER_PTR_TYPE    pHDBUser,
    OHA_UPPER_RQB_PTR_TYPE   pRQB);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Check_FastStartUp(
    OHA_UPPER_RQB_PTR_TYPE   pRQB);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadRealPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadRealInterfacePrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadPDInterfacePrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadPNIdentPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadFastStartUpPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadExpectedPrm(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestReadAdditionalInfo(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadStation(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_PrmReadDHCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadSNMP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadBus(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_RequestPrmReadTCP(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_PrmReadLLDPIfData(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16               NicId);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Put_PrmLLDPPortData(
    LSA_UINT16                  NICId,
	LSA_UINT16                  PortId,
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    OHA_COMMON_MEM_U32_PTR_TYPE pRecordLen,
	LSA_UINT32                  record_index,
    LSA_BOOL                    only_rema);

LSA_EXTERN LSA_UINT16 OHA_Put_PrmLLDPInterfaceData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
	LSA_UINT32	record_index,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CompareTopoMAU(
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId,
    LSA_BOOL            *   pCompare);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_GetExt_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pLinkStateExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pLinkState,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayDifferenceExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pDelayMode,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pMAUTypeDifferenceExisting,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUTypeMode);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_GetExt_PdPortDataAdjusted(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDomainBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pDomainBoundaryIngress,
    OHA_COMMON_MEM_U32_PTR_TYPE pDomainBoundaryEgress,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pMulticastBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pMulticastBoundary,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pPeerToPeerBoundaryExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pPeerToPeerBoundary);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdIRData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIRDataUUID);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_NameOfStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_SNMPSysData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysName,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_IpSuiteData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

LSA_EXTERN LSA_UINT16 OHA_GetMultipleStationData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetNameOfStationData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetIPSuiteData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetDHCPData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetMultipleSNMPData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetMultipleBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetCommonBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

LSA_EXTERN LSA_UINT16 OHA_GetAdditionBusData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen,
    LSA_BOOL only_rema);

/* OHA_REMA.C */

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_BOOL OHA_IsRemaResponsePending(LSA_UINT32 InterfaceId);

LSA_EXTERN LSA_UINT16  OHA_UPPER_IN_FCT_ATTR oha_rema_prm_prepare(
    LSA_UINT16 NicID,
    LSA_BOOL is_commit);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_prm_write(
    LSA_UINT16              NicID,
    OHA_UPPER_RQB_PTR_TYPE  rqb);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_prm_end(
    LSA_UINT16              NICId);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_prm_commit(
    LSA_UINT16             NicID,
    LSA_HANDLE_TYPE        ChannelHandle,
    LSA_BOOL               do_rtf);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_station_update(
    LSA_UINT16             NicID);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_dhcp_update(
    LSA_UINT16             NicID);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_snmp_update(
    LSA_UINT16             NicID);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_bus_update(
    LSA_UINT16             NicID);

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_rema_init(LSA_VOID); /* AP01039458 */

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_PDEV_undo_init(
    LSA_UINT16  NicID);

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_station_undo_init(
    LSA_UINT16  NicID);

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_snmp_undo_init(
    LSA_UINT16  NicID);

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_bus_undo_init(
    LSA_UINT16  NicID);

LSA_EXTERN LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_rema_prm_free_recordset_a(
    LSA_UINT16  NICId);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaRead(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaWrite(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB);

/* oha_tim.c */

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_TimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    OHA_LOCAL_CALLBACK_FCT_PTR_TYPE Cbf,   /* AP01539318 */
    OHA_LOCAL_MEM_U16_PTR_TYPE     pOHATimerID);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_TimerFree(LSA_UINT16 OHATimerID);
LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_TimerMgmSetup(LSA_VOID);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerStop(
    LSA_UINT16 OHATimerID);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerStart(
    LSA_UINT16 OHATimerID,
    LSA_UINT16 Time);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerRQBFree(
    LSA_UINT16 OHATimerID);

LSA_EXTERN LSA_UINT16  OHA_LOCAL_FCT_ATTR OHA_TimerRQBCheckFree(
    LSA_UINT16 OHATimerID);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_TimerCB(
    OHA_INTERNAL_REQUEST_PTR_TYPE   pIntRQB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestTimer(
    OHA_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestCyclicTimer(
    OHA_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestCyclicCheck(
    OHA_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestPtcpTimer(
    OHA_UPPER_RQB_PTR_TYPE  pRQB,
    OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              PortId);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelPtcpTimer(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

LSA_EXTERN LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_CancelCyclicTimer(
    OHA_HDB_USER_PTR_TYPE        pHDBUser,
    OHA_LOCAL_MEM_BOOL_PTR_TYPE  pFinish);

/* OHA_MIBS.C */

LSA_EXTERN OHA_STATS_INFO_PTR_TYPE oha_snmp_get_stats_info(LSA_VOID);

LSA_EXTERN LSA_UINT32 oha_snmp_get_lldp_statistics(LSA_VOID);

LSA_EXTERN LSA_VOID oha_snmp_delete_lldp_statistics(LSA_VOID);

LSA_EXTERN LSA_UINT32 oha_snmp_get_lldp_config_options(LSA_VOID);

LSA_EXTERN LSA_UINT32 oha_snmp_get_lldp_port_config_options(LSA_UINT16 if_index, LSA_UINT16 port_index);

LSA_EXTERN LSA_UINT32 oha_snmp_set_lldp_port_config_options(LSA_UINT16 nic_id, LSA_UINT16 port_id, LSA_UINT32 options);

LSA_EXTERN LSA_VOID oha_snmp_set_lldp_config_options(LSA_UINT32 option);

/* OHA_SOCU.C */

LSA_EXTERN LSA_VOID OHA_LOCAL_FCT_ATTR OHA_SOCKUserSetMib2ParamsPending(
	OHA_HDB_USER_PTR_TYPE   pHDBUser,
    LSA_UINT16              Response);

/*===========================================================================*/
/*                                   macros                                  */
/*===========================================================================*/

#define OHA_FATAL(Error) \
{                                                    \
		OHA_FatalError( Error,                       \
			    	 	OHA_MODULE_ID,               \
				     	__LINE__);                   \
}				     	

#ifndef OHA_ALLOC_FAIL_FATAL
#define OHA_ALLOC_FAIL_FATAL(pPtr) \
{                                                         \
	if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
	{                                                     \
		OHA_FATAL(OHA_FATAL_ERR_NULL_PTR);                \
	}                                                     \
}
#endif
  
#ifdef OHA_CFG_FREE_ERROR_IS_FATAL
#define OHA_MEM_FREE_FAIL_CHECK(RetVal) \
{                                                         \
	if( RetVal != LSA_RET_OK )                            \
	{                                                     \
		OHA_FATAL(OHA_FATAL_ERR_MEM_FREE);                \
	}                                                     \
}
#else
#define OHA_MEM_FREE_FAIL_CHECK(pPtr) {}
#endif

/*===========================================================================*/
/*                                  prototypes                               */
/*===========================================================================*/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined OHA_UPPER_IN_FCT_ATTR
#error "OHA_UPPER_IN_FCT_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_UPPER_OUT_FCT_ATTR
#error "OHA_UPPER_OUT_FCT_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_SYSTEM_IN_FCT_ATTR
#error "OHA_SYSTEM_IN_FCT_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_SYSTEM_OUT_FCT_ATTR
#error "OHA_SYSTEM_OUT_FCT_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_LOCAL_FCT_ATTR
#error "OHA_LOCAL_FCT_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_UPPER_RQB_ATTR
#error "OHA_UPPER_RQB_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_UPPER_MEM_ATTR
#error "OHA_UPPER_MEM_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_SYSTEM_MEM_ATTR
#error "OHA_SYSTEM_MEM_ATTR not defined in file oha_cfg.h"
#endif

#if !defined OHA_LOCAL_MEM_ATTR
#error "OHA_LOCAL_MEM_ATTR not defined in file oha_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif



/*****************************************************************************/
/*  end of file OHA_INT.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_INT_H */
