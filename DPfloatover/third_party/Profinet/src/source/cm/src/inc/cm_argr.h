#ifndef CM_ARGR_H
#define CM_ARGR_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_argr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the AR - Graph                                             */
/*                                                                           */
/*****************************************************************************/

/*------------------------------------------------------------------------------
// CM-AR-COM state machine
//----------------------------------------------------------------------------*/

enum cm_ar_com_state_enum {

	CM_AR_COM_INIT = 0,
	CM_AR_COM_IDLE = 1, /* has io_ar, waiting for start */

	/*
	 * "in-lined" state machine CM-AR-COM (start)
	 */

	/* removed */

	/*
	 * state machine CM-AR-COM (surveillance)
	 */

	CM_AR_COM_READY = 5, /* cr-s unknown or up */
	CM_AR_COM_DOWN  = 6,

	/*
	 * "in-lined" state machine CM-AR-COM (stop)
	 */

	CM_AR_COM_STOPPED_ALARM   = 7,
	CM_AR_COM_STOP_MCRS       = 9,
	CM_AR_COM_STOP_FRAME      = 11
	/* next state == IDLE */
};


/*------------------------------------------------------------------------------
// CM-AR-CR-XXX states
//----------------------------------------------------------------------------*/

enum cm_ar_cr_state_enum {

	CM_AR_CR_ALLOCATED =  0, /* has mem */

	/*** provider, consumer and alarms ***/

	/* prov:  IDLE, ERROR, PASSIVE, UP */
	/* cons:  IDLE, ERROR, PASSIVE, WF_UP, UP, DOWN */
	/* alarm: IDLE, ERROR, UP */

	/* note: transition PASSIVE -> ERROR is possible => use acp_id as trigger for remove */

	CM_AR_CR_IDLE      =  1, /* has frame-id or alarm-ref */
	CM_AR_CR_ERROR     =  3, /* error on add => skip when removing; error on control => cm_ar_com_down() */
	CM_AR_CR_PASSIVE   =  4, /* after adding */
	CM_AR_CR_RESET     =  6, /* consumer only: resetting to unknown-state */
	CM_AR_CR_WF_UP     =  7, /* consumer only: after activation and reset */
	CM_AR_CR_UP        =  8,
	CM_AR_CR_DOWN      =  9, /* consumer only: received a miss.ind */

	/*** multicast provider ***/

	/* mprov:  IDLE, ERROR, PASSIVE, UP */

	/*** multicast consumer ***/

	/* mcons:  future:   IDLE, ERROR, PASSIVE, ... */

	/* mcons:  current:  IDLE, WF_LINK_UP, NARE, ... */
	CM_AR_MCONS_WF_LINK_UP  = 41, /* wait for link-up */
	CM_AR_MCONS_NARE        = 42,
	CM_AR_MCONS_NARE_CANCEL = 43,
	CM_AR_MCONS_NARE_PAUSE  = 44,
	CM_AR_MCONS_ADD         = 45,
	CM_AR_MCONS_REGISTER    = 46,
	CM_AR_MCONS_ACT_RST     = 47, /* activate and reset */
	CM_AR_MCONS_WF_UP       = 48,
	CM_AR_MCONS_UP          = 49,
	CM_AR_MCONS_DOWN        = 50,
	CM_AR_MCONS_UNREGISTER  = 51,
	CM_AR_MCONS_REMOVE      = 52,

	/***/

	CM_AR_CR_STATE_LAST
};


/*------------------------------------------------------------------------------
// PDMasterTailorData, PDInstanceTailorData
//----------------------------------------------------------------------------*/

/*
//	PDMasterTailorData [ Index = 0x00017081, BlockType = 0x7081 ] ::=
//		BlockHeader, DeviceTailorProperties, DeviceGroup, NumberOfPortTailorInfos, [PortMasterTailorInfo*]
//
//	PortMasterTailorInfo ::=
//		LocalSlotNumber, LocalSubslotNumber, PortTailorProperties, PeerStationNumber, PeerSlotNumber, PeerSubslotNumber
*/

/* DeviceTailorProperties */
#define CM_PD_MTD_PROP_DEVICE_OPTIONAL	0x0001 /* DeviceMode(optional) // not valid for PDev */
#define CM_PD_MTD_PROP_NOS				0x0002 /* TailorNoS */
#define CM_PD_MTD_PROP_IP_SUITE			0x0004 /* TailorIPv4 */
#define CM_PD_MTD_PROP_SYNCDATA			0x0008 /* TailorPDSyncData */
#define CM_PD_MTD_PROP_MRP_DATA_ADJUST	0x0010 /* TailorPDInterfaceMrpDataAdjust */
#define CM_PD_MTD_PROP_MRP_DATA_CHECK	0x0020 /* TailorPDInterfaceMrpDataCheck */
#define CM_PD_MTD_PROP_IR_FRAME_DATA	0x0040 /* TailorPDIRFrameData */
#define CM_PD_MTD_PROP_RESERVED			0xFF80 /* reserved */
#define CM_PD_MTD_PROP_I_MRP_DATA_MASK	0x0030 /* internal mask: both mrp must set */
#define CM_PD_MTD_PROP_I_ANY_PEER_OPT	0x1000 /* internal bit: any peers is optional */
#define CM_PD_MTD_PROP_I_ANY_PEER_PROG	0x2000 /* internal bit: any peer is programmable */

/* PortTailorProperties */
#define CM_PD_MTD_PEER_PROP_PEER_MASK			0x0003 /* PeerMode */
#define CM_PD_MTD_PEER_PROP_PEER_FIXED			0x0000 /* PeerMode Fixed peer to mandatory neighbor */
#define CM_PD_MTD_PEER_PROP_PEER_OPT			0x0001 /* PeerMode Fixed peer to optional neighbor */
#define CM_PD_MTD_PEER_PROP_PEER_PROG			0x0002 /* PeerMode Programmable peer */
#define CM_PD_MTD_PEER_PROP_PEER_ANY			0x0003 /* PeerMode Any peer */
#define CM_PD_MTD_PEER_PROP_PORT_CODING			0x0004 /* NameOfPortCoding */
#define CM_PD_MTD_PEER_PROP_RED_PORT			0x0008 /* TailorREDPort */
#define CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST		0x0010 /* TailorPDPortMrpDataAdjust */
#define CM_PD_MTD_PEER_PROP_RESERVED			0xFFE0 /* reserved */
#define CM_PD_MTD_PEER_PROP_I_PARSED			0x1000 /* internal bit: PDPortDataCheck parsed */
/* real_properties */
#define CM_PD_MTD_PEER_PROP_I_RTC3				0x2000 /* internal bit: Neighbour is an RTC3 device */

/* set_by_itd */
#define CM_PD_MTD_SET_IS_TAILORED				0x0001 /* PDInstanceTailorData was written */
#define CM_PD_MTD_SET_DEVICE_ENABLED			0x0002 /* enabled by .DeviceInstanceTailorData.StationNumber */


typedef struct cm_pd_port_prm_pmtd_tag
{
	LSA_UINT16  local_station_nr;		/* local StationNumber */
	LSA_UINT16  local_slot_nr;			/* PortMasterTailorData.LocalSlotNumber */
	LSA_UINT16  local_subslot_nr;		/* PortMasterTailorData.LocalSubslotNumber */

	LSA_UINT16  properties;				/* PortMasterTailorData.PortTailorProperties */

	LSA_UINT16  peer_station_nr;		/* PortMasterTailorData.PeerStationNumber  or  PDInstanceTailorData...PeerPort.StationNumber */
	LSA_UINT16  peer_slot_nr;			/* PortMasterTailorData.PeerSlotNumber     or  PDInstanceTailorData...PeerPort.SlotNumber */
	LSA_UINT16  peer_subslot_nr;		/* PortMasterTailorData.PeerSubslotNumber  or  PDInstanceTailorData...PeerPort.SubslotNumber */

	LSA_UINT32  check_linedelay;		/* parsed PDPortDataCheck.CheckLineDelay  0..reserved/unknown */
	LSA_UINT32  max_line_rx_delay;		/* parsed PDIRData.PDIRGlobalData.MaxLineRxDelay  0..unknown */

	LSA_UINT16  real_station_nr;		/* enabled/programmed neighbour StationNumber */
	LSA_UINT16  real_slot_nr;			/* enabled/programmed neighbour SlotNumber */
	LSA_UINT16  real_subslot_nr;		/* enabled/programmed neighbour SubSlotNumber */
	LSA_UINT16	real_properties;		/* enabled/programmed neighbour properties + real_properties */
	LSA_UINT32  real_max_line_rx_delay;	/* tailored MaxLineRXDelay  0..no neighbour or green neighbour */
} CM_PD_PORT_PRM_PMTD_TYPE;

typedef CM_PD_PORT_PRM_PMTD_TYPE CM_LOCAL_MEM_ATTR  * CM_PD_MTD_PORT_PTR_TYPE;

typedef struct cm_pd_master_tailor_data_tag
{
	LSA_UINT16				properties;				/* != 0 if PDMasterTailorData is written */
	LSA_UINT16				port_cnt;				/* count of PortMasterTailorData*/
	CM_PD_MTD_PORT_PTR_TYPE	ports;					/* PortMasterTailorData */
	LSA_UINT16				set_by_itd;				/* infos given by PDInstanceTailorData */
	LSA_UINT16				pdirglobaldata_version;	/* version of the PDIRGlobalData  0..unknown/not existing */
	LSA_UINT32				max_bridge_delay;		/* PDIRData.PDIRGlobalData.MaxBridgeDelay */
	LSA_UINT16				frame_id[2];			/* consumer/provider frame_id */
} CM_PD_MASTER_TAILOR_DATA_TYPE;

typedef CM_PD_MASTER_TAILOR_DATA_TYPE CM_LOCAL_MEM_ATTR * CM_PD_MASTER_TAILOR_DATA_PTR_TYPE;


/*
//	PDInstanceTailorData [ Index = 0xB080, BlockType = 0x7080 ] ::=
//		BlockHeader, Padding, Padding, DeviceInstanceTailorData, [ PortInstanceTailorData ]
//
//	DeviceInstanceTailorData ::=	# enable optional devices
//		DeviceGroup, NumberOfDevices, (StationNumber, Padding, Padding)*
//
//	PortInstanceTailorData ::		# peer programable ports, use enabled or mandatory devices
//		Padding, Padding, NumberOfInterconnections, (PeerPort1, PeerPort2)*
//
//	PeerPort1 ::=
//		StationNumber, SlotNumber, SubslotNumber, Padding, Padding
//
//	PeerPort2 ::=
//		StationNumber, SlotNumber, SubslotNumber, Padding, Padding
*/

typedef struct cm_pd_port_instance_peer_tag
{
	/* PeerPort1 */
	LSA_UINT16		device_nr_1;
	LSA_UINT16		slot_nr_1;
	LSA_UINT16		subslot_nr_1;
	/* PeerPort2 */
	LSA_UINT16		device_nr_2;
	LSA_UINT16		slot_nr_2;
	LSA_UINT16		subslot_nr_2;
} CM_PD_PORT_INSTANCE_PEER_TYPE;

typedef CM_PD_PORT_INSTANCE_PEER_TYPE CM_LOCAL_MEM_ATTR * CM_PD_PORT_INSTANCE_PEER_PTR_TYPE;

typedef struct cm_pd_instance_tailor_data_tag
{
	LSA_UINT16							nr_of_devices;		/* NumberOfDevices */
	LSA_UINT16							*devices;			/* enable these devices */
	LSA_UINT16							nr_of_peers;		/* NumberOfInterconnections */
	CM_PD_PORT_INSTANCE_PEER_PTR_TYPE	peers;				/* connect these pdev-ports */
} CM_PD_INSTANCE_TAILOR_DATA_TYPE;

typedef CM_PD_INSTANCE_TAILOR_DATA_TYPE CM_LOCAL_MEM_ATTR * CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE;


/*------------------------------------------------------------------------------
// CM-CL-AR things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

# include "cm_iclar.h"

#endif


/*------------------------------------------------------------------------------
// CM-SV-AR things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS

# include "cm_isvar.h"

#endif


/*------------------------------------------------------------------------------
// ARCQ - fixed size list
//
//	e.g: ProvAct, ConsAct, ProvSetDS, FramePas, AlarmRem
//----------------------------------------------------------------------------*/

#define CM_ARCQ_LIST_SIZE  5

struct cm_arcq_entry_tag {
	LSA_UINT8   opcode;
	LSA_BOOL    is_startup;
	LSA_UINT16  para1;
};

typedef struct cm_arcq_entry_tag CM_COMMON_MEM_ATTR * CM_ARCQ_ENTRY_PTR_TYPE;

typedef struct cm_arcq_list_tag {
	LSA_UINT8  curr;
	LSA_UINT8  next;
	struct cm_arcq_entry_tag  entry[CM_ARCQ_LIST_SIZE];
} CM_ARCQ_LIST_TYPE;

typedef struct cm_arcq_list_tag CM_COMMON_MEM_ATTR * CM_ARCQ_LIST_PTR_TYPE;


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_alarm_cr_tag CM_COMMON_MEM_ATTR * CM_AR_GRAPH_ALARM_CR_PTR_TYPE; /* forward */

typedef CM_ARCB_IR_INFO_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_IR_INFO_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IR_INFO_SUB_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_SR_INFO_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_SR_INFO_PTR_TYPE; /* only other mem-attrib */


/*------------------------------------------------------------------------------
// AR-Graph, Head
//----------------------------------------------------------------------------*/

typedef struct cm_argr_ar_tag {
	LSA_UINT32			host_ip;			/* network byte order, client: IP of IOD; server: IP of IOC */

	LSA_UINT16			ar_type;			/* see cm_ar_type_enum */
	CLRPC_UUID_TYPE		ar_uuid;			/* ARUUID */

	CM_MAC_TYPE			eth_mac;			/* client: CMResponderMacAdd; server: CMInitiatorMacAdd */
	CLRPC_UUID_TYPE		cmi_obj_uuid;		/* CMIObjectUUID */
	LSA_UINT32			ar_properties;		/* see cm_ar_properties_enum */
	LSA_UINT16			cmi_timeout100ms;	/* CMIActivityTimeout */
	LSA_UINT16			cmi_udp_port;		/* network byte order, InitiatorUDPRTPort */
	LSA_UINT16			cmr_udp_port;		/* network byte order, ResponderUDPRTPort */

	LSA_UINT8			cmi_station_name_length; /* length of cmi_station_name */
	CM_COMMON_MEM_U8_PTR_TYPE
						cmi_station_name;	/* not null-terminated! */
} CM_AR_GRAPH_AR_TYPE;


typedef struct cm_argr_prm_tag {
	LSA_UINT32			prm_properties;
	CLRPC_UUID_TYPE		prm_obj_uuid;		/* Parameterisation_Object_UUID */

	LSA_UINT8			prm_server_name_length; /* length of prm_server_name */
	CM_COMMON_MEM_U8_PTR_TYPE
						prm_server_name;	/* not null-terminated! */
} CM_AR_GRAPH_PRM_TYPE;


typedef struct cm_ar_graph_tag {
	CM_LIST_ENTRY_TYPE  link;

	LSA_UINT32  trace_idx; /* for "LSA index trace" */

	LSA_UINT16	device_nr;
	LSA_UINT16	ar_nr;
	LSA_UINT16  session_key;

	/***/

	struct cm_ar_com_tag
	{
		CM_CHANNEL_PTR_TYPE  channel; /* see alloc */
		LSA_UINT8  path_type; /* see channel->detailptr->path_type */

		LSA_UINT16 state; /* see cm_ar_com_state_enum */

		LSA_UINT16 ar_send_clock;
		LSA_BOOL   is_power_of_two;
		LSA_BOOL   has_udp; /* TRUE if ar contains iocrs or alarmcrs with upd-transport (AP00583478) */
		LSA_UINT16 autostop_cons_id; /* ACP_INVALID_ACP_ID or cons_id for ACP_PROVIDER_SET_AUTOSTOP */

		/*
		 * variables for CM-AR-COM
		 */

		LSA_UINT8   down; /* see cm_ar_reason_enum (see ACP_OPC_ALARM_CLOSE::pnio_status_reason) */
		LSA_UINT16  cycle_counter;
		LSA_UINT32  down_pnio_status; /* for AP00678183, see cm_acp_alarm_indication() */
		ACP_ERROR_INFO_DATA_TYPE  down_debug_data; /* see VendorDeviceErrorInfo */

		LSA_BOOL cr_is_up;
		LSA_BOOL cr_is_rir;

		/***/

		LSA_UINT16 user_apdu_status; /* data-status indicated to user */

		/***/

		CM_ONESHOT_TYPE timer; /* see CM-AR-COM-TIMER */

		LSA_UINT16 mcr_busy_cnt; /* number of "busy" MCRs */

		LSA_BOOL do_reset; /* TRUE .. reset running (defer 'up' until after cnf) */
	} ar_com;

	struct cm_arcq_tag
	{
		CM_ARCQ_LIST_TYPE  list; /* list of arcq-opcodes, see cm_arcq_request() */
		LSA_UINT8  entered; /* reentrance check */
		LSA_UINT8  curr_opc;
		CM_ACP_LOWER_RQB_PTR_TYPE  acp_rqb; /* pre-allocated ACP-RQB */
	} arcq;

	/***/

	union cm_cl_sv_mc_tag {

		struct {
			LSA_UINT32  nothing_so_far; /* do not evaluate, AP00565264 */
		} pd;

#if CM_CFG_MAX_CLIENTS
		CM_CL_ARGR_TYPE cl;
#endif

#if CM_CFG_MAX_SERVERS
		CM_SV_ARGR_TYPE sv;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		LSA_UINT16 mc;
#endif
	} cm;

	/* IODConnectReq */

	CM_AR_GRAPH_AR_TYPE				ar;			/* ARBlockReq */

	CM_LIST_ENTRY_TYPE				io_cr_list;	/* [IOCRBlockReq*] */
												/* list of: CM_AR_GRAPH_IO_CR_PTR_TYPE */

	CM_AR_GRAPH_ALARM_CR_PTR_TYPE	alarm_cr;	/* [AlarmCRBlockReq] */

	CM_LIST_ENTRY_TYPE				ap_list;	/* [ExpectedSubmoduleBlockReq*] */
												/* list of: CM_AR_GRAPH_AP_PTR_TYPE / CM_AR_GRAPH_MODULE_PTR_TYPE / CM_AR_GRAPH_SUBMODULE_PTR_TYPE */

	/* prm-server not yet						// [PrmServerBlock] */

	CM_LIST_ENTRY_TYPE				mcr_list;	/* [MCRBlockReq*] */

	CM_AR_GRAPH_IR_INFO_PTR_TYPE	ir_info;	/* [IRInfoBlock] */

	CM_AR_GRAPH_SR_INFO_PTR_TYPE	sr_info;	/* [SRInfoBlock] */

	CM_LIST_ENTRY_TYPE				arvendor_list;	/* [ARVendorBlockReq*] */
												/* list of: CM_AR_GRAPH_ARVENDOR_PTR_TYPE */
} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, IOCR (iodata, iocs, ioapi, iocr)
//----------------------------------------------------------------------------*/

/* general send order: CM-CL, then CM-SV, then CM-MC (see AP00542516) extended by AP01379821 (sequence 16->32 bits) */
/* CL: 0x1000dddd */
/* SV: 0x20ddaaaa */
/* MC: 0x3000dddd */
#define CM_AR_IOCR_SEQUENCE_CL_BASE		0x10000000
#define CM_AR_IOCR_SEQUENCE_CL_COUNT	0x0000ffff
#define CM_AR_IOCR_SEQUENCE_SV_BASE		0x20000000
#define CM_AR_IOCR_SEQUENCE_SV_COUNT	0x01000000 /* 24 bits */
#define CM_AR_IOCR_SEQUENCE_MC_BASE		0x30000000
#define CM_AR_IOCR_SEQUENCE_MC_COUNT	0x0000ffff


typedef CM_ARCB_IOCS_TYPE CM_AR_GRAPH_IOCS_TYPE; /* the same */
typedef CM_ARCB_IOCS_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_IOCS_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IODATA_OBJECT_TYPE CM_AR_GRAPH_IODATA_OBJECT_TYPE; /* the same */
typedef CM_ARCB_IODATA_OBJECT_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE; /* only other mem-attrib */

typedef CM_ARCB_IOAPI_TYPE CM_AR_GRAPH_IOAPI_TYPE; /* the same */
typedef CM_ARCB_IOAPI_TYPE CM_LOCAL_MEM_ATTR * CM_AR_GRAPH_IOAPI_PTR_TYPE; /* only other mem-attrib */

typedef struct cm_ar_graph_io_cr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	struct {
		CM_AR_GRAPH_PTR_TYPE  backlink; /* backlink to the ar_graph */

		LSA_BOOL  is_consumer;  /* true if iocr_type is a consumer (iocr_type has view of controller!) */
		LSA_BOOL  is_multicast; /* true if iocr_type is a m-provider or m-consumer */

		LSA_UINT8  cr_state; /* see cm_ar_cr_state_enum, state machine CM-AR-CR-XXX */
		LSA_BOOL   do_stop;  /* stop-command for m-provider and m-consumer */
		LSA_BOOL   is_rir;   /* for RTClass3: is reveived in red */

		/* buffer handling */

		LSA_UINT16  acp_id;

		LSA_UINT32  edd_properties;      /* for EDDI */
		LSA_UINT32  edd_data_offset;     /* for EDDI, EDDP */
		LSA_UINT32  edd_ds_offset;       /* for EDDx, DataStatusOffset */
		LSA_UINT16  edd_cr_number;       /* for EDDP, EDDS */
		LSA_UINT8   edd_forward_mode;    /* for EDDP */
		LSA_UINT32  edd_ct_outdatastart; /* for EDDP */

		LSA_UINT16  apdu_status_length; /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  partial_length;     /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  partial_offset;     /* see cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  subframe_data;      /* see cm_ar_graph_check_ir_info_block(), cm_ar_graph_check_iocrs_against_expsubmods() */
		LSA_UINT16  subframe_offset;    /* see cm_ar_graph_check_ir_info_block(), cm_ar_graph_check_iocrs_against_expsubmods() */

		LSA_UINT32  edd_cw_offset; /* pointer to ControlWord, only provider (see AP00618087)*/
		LSA_UINT32  edd_as_offset; /* pointer to APDUStatus, only consumer (see AP00630092) */

		/* local adaption */

		struct {
			LSA_UINT16	send_clock; /* 0..no adaption set, see cm_cl_ar_graph_set_iocr_adaption(), see cm_sv_ar_connect_indication_continued() */
			LSA_UINT16	reduction_ratio;
			LSA_UINT16	phase;
			LSA_UINT32	sequence; /* AP01379821 (16->32 bits) */
			LSA_UINT16  data_hold_factor;
		} adpt;

		LSA_UINT16 cons_data_status; /* cons: see cm_..._ind() prov: 0 */

#if CM_CFG_USE_NARE
		CM_ONESHOT_TYPE mc_timer; /* timer, m-consumer only */
		LSA_UINT16 mc_flags;      /* see cm_cr_mc_flags_enum,  m-consumer only */
		CM_MAC_TYPE mc_addr;      /* resolved mac-addr */
#endif

#if CM_CFG_MAX_SERVERS
		struct {
			LSA_USER_ID_TYPE iocr_user_id;
#if CM_CFG_SV_AR_HAS_MCONS
			LSA_UINT16 data_status_ind; /* data_status indicated to the user */
#endif
		} sv;
#endif

	} cm;

	/***/

	LSA_UINT16			iocr_type;		/* see cm_iocr_type_enum */
	LSA_UINT16			iocr_ref;		/* IOCRReference */
	LSA_UINT16			ethertype;		/* LT */
	LSA_UINT32			iocr_properties;/* see cm_iocr_properties_enum */

	LSA_UINT16			data_length;
	LSA_UINT16			frame_id;

	LSA_UINT16			send_clock;
	LSA_UINT16			reduction_ratio;
	LSA_UINT16			phase;
	LSA_UINT32			frame_send_offset;

	LSA_UINT16			data_hold_factor;

	CM_MAC_TYPE			multicast_mac;

	/***/

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag) related_ap_list; /* list of CM_AR_GRAPH_IOAPI_TYPE */

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_IO_CR_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, ALARM-CR
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_alarm_cr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	struct {
		CM_AR_GRAPH_PTR_TYPE  backlink; /* backlink to the ar_graph */

		LSA_UINT16  cr_state; /* see cm_ar_cr_state_enum, state machine CM-AR-CR-XXX */
	} cm;

	/***/

	LSA_UINT16			alcr_type;       /* see cm_alcr_type_enum */
	LSA_UINT32			alcr_properties; /* see cm_alcr_properties_enum */

	LSA_UINT8			timeout100ms;
	LSA_UINT8			retries;

	LSA_UINT16			local_alarm_ref;

	LSA_UINT16			max_alarm_data_len; /* see acp-info */

	LSA_UINT16			ethertype; /* LT */

	/* rsp */
	LSA_UINT16			remote_alarm_ref;

} CM_AR_GRAPH_ALARM_TYPE;

/* typedef ... CM_AR_GRAPH_ALARM_CR_PTR_TYPE see above */

/*------------------------------------------------------------------------------
// AR-Graph, AP
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_ap_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	/***/

	LSA_UINT32				api; /* application process identifier */

	CM_LIST_ENTRY_TYPE		mod_list;

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_AP_PTR_TYPE*/ struct cm_sv_ap_tag CM_LOCAL_MEM_ATTR * real_ap; /* pointer to the real config, see cm_sv_attach() */
	} sv;
#endif

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_AP_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Module
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_module_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	CM_AR_GRAPH_AP_PTR_TYPE  ap; /* parent */

	/***/

	LSA_UINT16				slot_nr;		/* Slot_Number */

	struct cm_arg_module_soll_tag {
		LSA_UINT32			mod_ident;
		LSA_UINT16			mod_properties;	/* see cm_mod_properties_enum */
	} soll;

#if CM_CFG_MAX_CLIENTS /* note: CM-SV uses sv.real_sl->mod_ident */
	struct cm_arg_module_ist_tag {
		LSA_UINT32			mod_ident;
	} ist;
#endif

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_SLOT_PTR_TYPE*/ struct cm_sv_slot_tag CM_LOCAL_MEM_ATTR * real_sl; /* pointer to the real config, see cm_sv_attach() */
	} sv;
#endif

	CM_LIST_ENTRY_TYPE		submod_list;

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_MODULE_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Submodule
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_submodule_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	CM_AR_GRAPH_MODULE_PTR_TYPE  mod; /* parent */

	/***/

	LSA_UINT16			subslot_nr;		/* ascending order */

	struct cm_arg_sub_soll_tag {
		LSA_UINT32		sub_ident;
		LSA_UINT16		sub_properties;	/* see cm_sub_properties_enum */
	} soll;

#if CM_CFG_MAX_CLIENTS /* note: CM-SV uses sv.real_su->sub_ident */
	struct cm_arg_sub_ist_tag {
		LSA_UINT32		sub_ident;
		LSA_UINT16		sub_state;		/* see cm_sub_state_enum, CM-SV: see comment in "struct sv" below */
	} ist;
#endif

	/* submodule data description */
	struct cm_argr_submodule_descr_tag {
		LSA_UINT16		data_description;	/* Data Description */
		LSA_UINT16		data_length;		/* Data Length, without IOPS */

		LSA_UINT8		iops_length;		/* Length IOPS 1..n */
		LSA_UINT8		iocs_length;		/* Length IOCS 1..m */

	} descr[2];								/* index: 0..input 1..output */

	LSA_BOOL	inp_via_mcr;	/* is a m-provider-submodule (inputs are written to m-provider cr) */
	LSA_BOOL	out_via_mcr;	/* is a m-consumer-submodule (outputs are read from m-consumer cr) */

#if CM_CFG_MAX_CLIENTS
	struct {
		CM_LIST_ENTRY_TYPE	submod_link;		/* link within prm.prm_submod_list */
		CM_LIST_ENTRY_TYPE	prm_record_list;	/* list for submodule-prm-records */
	} cl;
#endif

#if CM_CFG_MAX_SERVERS
	struct {
		/*CM_SV_SUBSLOT_PTR_TYPE*/ struct cm_sv_subslot_tag CM_LOCAL_MEM_ATTR * real_su; /* pointer to the real config, see cm_sv_attach() */
		LSA_UINT16 sm_flags; /* see CM_SV_AR_SMFL_TNA, ... */
		LSA_UINT16 st_told;  /* submodule state that was sent to CM-CL */
	} sv;
#endif

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_SUBMODULE_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Multicast CR
//----------------------------------------------------------------------------*/
typedef struct cm_ar_graph_mcr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT16		iocr_ref;
	LSA_UINT32		mcr_properties;	/* see cm_mcr_properties_enum */
	LSA_UINT16		mci_timeout_100ms;
	LSA_UINT8		provider_name_length;
	CM_UPPER_MEM_U8_PTR_TYPE
					provider_name;

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_MCR_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, ARVendorBlock
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_arvendor_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT32		api;
	LSA_UINT16		ap_structure_identifier;
	LSA_UINT16		req_data_len;
	LSA_UINT16		res_data_len;

	LSA_UINT8		data[1];
	/*** nothing here, dynamic size ***/

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_ARVENDOR_PTR_TYPE;


/*------------------------------------------------------------------------------
// AR-Graph, Unique Slot_nr
//----------------------------------------------------------------------------*/

typedef struct cm_ar_graph_slot_nr_tag {
	CM_LIST_ENTRY_TYPE	link; /* first in struct! */

	LSA_UINT16			slot_nr;
} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_SLOT_NR_PTR_TYPE;


/*------------------------------------------------------------------------------
// utilities
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_port_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_station_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_alias_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_station_name_copy (
	CM_COMMON_MEM_U8_PTR_TYPE to_station_name,
	CM_COMMON_MEM_U8_PTR_TYPE from_station_name,
	LSA_UINT16  station_name_length
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_iosar_with_device_access(
	CM_AR_GRAPH_PTR_TYPE ar_graph
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_frame_id_within_normative_range (
	LSA_BOOL				is_consumer,
	LSA_BOOL				is_multicast,
	LSA_UINT32				rt_class,
	LSA_UINT16				frame_id
);

/*------------------------------------------------------------------------------
// build the ar-graph
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_alloc(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_free (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_free_list_local_mem(
	CM_LIST_ENTRY_PTR_TYPE	list
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32				host_ip, /* network byte order */

	LSA_UINT16				ar_type,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const ar_uuid,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmi_mac,

	CLRPC_UUID_PTR_TYPE		cmi_obj_uuid,

	LSA_UINT32				ar_properties,
	LSA_UINT16				cmi_timeout100ms,
	LSA_UINT16				cmi_udp_port, /* network byte order */

	LSA_UINT16/*!*/			cmi_station_name_length,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmi_station_name
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmr_mac,

	LSA_UINT16				cmr_udp_port /* network byte order */
);
#endif

#if CM_CFG_MAX_SERVERS
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_io_ar(
	CM_AR_GRAPH_PTR_TYPE argr
);
#endif

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_autostop_cons_id (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ir_info (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CLRPC_UUID_PTR_TYPE		ir_data_uuid,
	LSA_UINT16				alloc_more /* for CM-SV, else 0 */
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ir_info_sub (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16				iocr_ref,
	LSA_UINT16				subframe_offset,
	LSA_UINT32				subframe_data
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_reduction_ratio_range_ok (
	LSA_UINT16  reduction_ratio,
	LSA_BOOL  is_power_of_two,
	LSA_UINT32  rt_class
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_io_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,

	LSA_UINT16  iocr_type,
	LSA_UINT16  iocr_ref,
	LSA_UINT16  ethertype,
	LSA_UINT32  iocr_properties,

	LSA_UINT16  data_length,
	LSA_UINT16  frame_id,

	LSA_UINT16  send_clock,
	LSA_UINT16  reduction_ratio,
	LSA_UINT16  phase,
	LSA_UINT32  frame_send_offset,

	LSA_UINT16  watchdog_factor_legacy, /* CM-CL/CM-MC: pass in data_hold_factor, CM-SV: pass in value from PDU (AP01304706) */
	LSA_UINT16  data_hold_factor,

	LSA_UINT16  iocr_tag_header, /* only for provider, don't care for consumer */

	CM_COMMON_MEM_U8_PTR_TYPE  mac_addr,

	CM_AR_GRAPH_IO_CR_PTR_TYPE CM_LOCAL_MEM_ATTR  *iocr_ptr_ptr /* out */
);

#if CM_CFG_MAX_CLIENTS  ||  CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_set_iocr_edd_data (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ptr,

	LSA_UINT32		edd_data_offset,
	LSA_UINT32		edd_properties,
	LSA_UINT16		edd_cr_number,
	LSA_UINT8		edd_forward_mode,
	LSA_UINT32		edd_ct_outdatastar,

	LSA_UINT16		partial_length,
	LSA_UINT16		partial_offset,
	LSA_UINT16		apdu_status_length,
	LSA_UINT16		subframe_data,
	LSA_UINT16		subframe_offset
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_set_iocr_adaption (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,

	LSA_UINT16		iocr_ref,
	LSA_UINT16		send_clock,
	LSA_UINT16		reduction_ratio,
	LSA_UINT16		phase,
	LSA_UINT32		frame_send_offset,
	LSA_UINT16		data_hold_factor
);

#endif

#if CM_CFG_MAX_CLIENTS
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_io_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16			iocr_type,
	LSA_UINT16			iocr_ref,
	LSA_UINT16			frame_id
);
#endif

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_ioap (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,

	LSA_UINT32		api,

	CM_AR_GRAPH_IOAPI_PTR_TYPE CM_LOCAL_MEM_ATTR *ioapi_ptr_ptr /* out */
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_iodata_objects (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE CM_LOCAL_MEM_ATTR *iodata_ptr_ptr
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_iocs (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IOCS_PTR_TYPE CM_LOCAL_MEM_ATTR *iocs_ptr_ptr
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_alarm_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16				alcr_type,
	LSA_UINT16				ethertype,
	LSA_UINT32				alcr_properties,

	LSA_UINT16				timeout100ms,
	LSA_UINT16				retries,

	LSA_UINT16				alarm_ref,
	LSA_UINT16				max_alarm_data_length,

	LSA_UINT16				alcr_tag_header_high,
	LSA_UINT16				alcr_tag_header_low,

	CM_AR_GRAPH_ALARM_CR_PTR_TYPE CM_LOCAL_MEM_ATTR *alcr_ptr_ptr /* out */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ap_module (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32	api,

	LSA_UINT16	slot_nr,
	LSA_UINT32	mod_ident,
	LSA_UINT16	mod_properties,

	LSA_UINT16	CM_LOCAL_MEM_ATTR *exp_slot_nr_ptr, /* LSA_NULL or expected slot_nr */

	CM_AR_GRAPH_AP_PTR_TYPE CM_LOCAL_MEM_ATTR *ap_ptr_ptr, /* out */
	CM_AR_GRAPH_MODULE_PTR_TYPE CM_LOCAL_MEM_ATTR *mod_ptr_ptr /* out */
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_submodule (
	LSA_UINT32		ar_properties,
	CM_AR_GRAPH_MODULE_PTR_TYPE  module,

	LSA_UINT16		subslot_nr,
	LSA_UINT32		sub_ident,
	LSA_UINT16		sub_properties,
	LSA_BOOL		IOXS_Required,

	CM_AR_GRAPH_SUBMODULE_PTR_TYPE CM_LOCAL_MEM_ATTR *sub_ptr_ptr /* out */
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_set_data_descr (
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  submod,

	LSA_UINT16		direction, /* 0..input, 1..output, 2..use-description */

	LSA_UINT16		data_description,
	LSA_UINT16		data_length,
	LSA_UINT8		iops_length,
	LSA_UINT8		iocs_length
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_mcr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16		iocr_ref,
	LSA_UINT32		mcr_properties,	/* see cm_mcr_properties_enum */
	LSA_UINT16		mci_timeout_100ms,
	LSA_UINT16/*!*/	provider_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE					provider_name,
	CM_AR_GRAPH_MCR_PTR_TYPE CM_LOCAL_MEM_ATTR	*mcr_ptr_ptr	/* out */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_sr_info (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16	rdh_factor,
	LSA_UINT32	sr_properties,
	LSA_UINT16	alloc_more /* for CM-SV, else 0 */
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_arvendor (
	CM_AR_GRAPH_PTR_TYPE  argr,

	LSA_UINT32					api,
	LSA_UINT16					ap_structure_identifier,
	LSA_UINT16					req_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE	req_data,
	LSA_UINT16					res_alloc_len
);

CM_AR_GRAPH_ARVENDOR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_arvendor (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT32  api,
	LSA_UINT16  ap_structure_identifier
);

/*----------------------------------------------------------------------------*/

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_consistency(
	CM_AR_GRAPH_PTR_TYPE ar_graph
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_iocr_against_mcr (
	CM_AR_GRAPH_PTR_TYPE ar_graph
);
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_mcr_against_iocr (
	CM_AR_GRAPH_PTR_TYPE ar_graph
);

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_check_send_clock_adaption (
	CM_AR_GRAPH_PTR_TYPE ar_graph,
	LSA_UINT16 local_clock
);

#endif


/*------------------------------------------------------------------------------
// find within the ar_graph
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_IO_CR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_io_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  io_cr_ref
);

CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ir_info_sub (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
);

CM_AR_GRAPH_IOAPI_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ioapi (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_UINT32  api
);

CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_iodata (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

CM_AR_GRAPH_IOCS_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_iocs (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

CM_AR_GRAPH_ALARM_CR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_alarm_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
);

CM_AR_GRAPH_AP_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ap (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT32  api
);

CM_AR_GRAPH_MODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_module (
	CM_AR_GRAPH_AP_PTR_TYPE  ar_ap,
	LSA_UINT16  slot_nr
);

CM_AR_GRAPH_SUBMODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_submodule (
	CM_AR_GRAPH_AP_PTR_TYPE  ar_ap,
	CM_AR_GRAPH_MODULE_PTR_TYPE  ar_mod, /* in: mod or slot_nr must specified */
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_api_slot_subslot (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_AR_GRAPH_AP_PTR_TYPE  CM_LOCAL_MEM_ATTR * ap_pp,
	CM_AR_GRAPH_MODULE_PTR_TYPE  CM_LOCAL_MEM_ATTR * mod_pp,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  CM_LOCAL_MEM_ATTR * sub_pp
);

CM_AR_GRAPH_MCR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_mcr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
);

/*------------------------------------------------------------------------------
// ar-graph helper
//----------------------------------------------------------------------------*/

#if CM_DEBUG

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_dump (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
);

#endif


/*------------------------------------------------------------------------------
// arcb helper
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcb_iocr_list_from_argr_iocr_list (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_ENTRY_PTR_TYPE  cb_iocr_list,
	CM_LIST_ENTRY_PTR_TYPE  ar_iocr_list
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcb_iocr_list_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_ENTRY_PTR_TYPE  cb_iocr_list
	);

#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ARGR_H */
