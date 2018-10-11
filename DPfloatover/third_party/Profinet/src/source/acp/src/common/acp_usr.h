#ifndef ACP_USR_H
#define ACP_USR_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*------------------------------------------------------------------------------
//	OPCODES
//	ACP_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

#define ACP_OPC_INTERNAL_TIMER				 0  /* only internal usage */

#define ACP_OPC_OPEN_CHANNEL				 1
#define ACP_OPC_CLOSE_CHANNEL				 2
#define ACP_OPC_CHANNEL_INFO				 3

#define ACP_OPC_PROVIDER_ADD				 4
/* not used									 5 */
#define ACP_OPC_PROVIDER_REMOVE				 6
#define ACP_OPC_PROVIDER_CONTROL			 7
#define ACP_OPC_PROVIDER_CHANGE_PHASE		 8
#define ACP_OPC_PROVIDER_XCHANGE_BUF		 9

#define ACP_OPC_GLOBAL_PROVIDER_CONTROL		10

#define ACP_OPC_CONSUMER_ADD				11
/* not used									12 */
#define ACP_OPC_CONSUMER_REMOVE				13
#define ACP_OPC_CONSUMER_CONTROL			14
#define ACP_OPC_CONSUMER_XCHANGE_BUF		15

#define ACP_OPC_STATE_IND_RES_PROVIDE		16
#define ACP_OPC_STATE_INDICATION			17

#define ACP_OPC_ALARM_OPEN					18
#define ACP_OPC_UDP_ALARM_OPEN				19
#define ACP_OPC_ALARM_CLOSE					20
#define ACP_OPC_ALARM_SEND					21
#define ACP_OPC_ALARM_ACK_SEND				22
#define ACP_OPC_ALARM_IND_RES_PROVIDE		23
#define ACP_OPC_ALARM_INDICATION			24
#define ACP_OPC_ALARM_ACK_INDICATION		25

#define ACP_OPC_LINK_STATUS_IND_RES_PROVIDE	26
#define ACP_OPC_LINK_STATUS_INDICATION		27

#define ACP_OPC_GET_APDU_STATUS				28

#define ACP_OPC_MULTICAST					29

#define ACP_OPC_OVERLOAD_IND_RES_PROVIDE	30
#define ACP_OPC_OVERLOAD_INDICATION			31

#define ACP_OPC_NOP							32

#define ACP_OPCODE_MAX						33


/*------------------------------------------------------------------------------
//	ERROR_CODES
//	ACP_RQB_TYPE::response
//----------------------------------------------------------------------------*/

#define ACP_OK						LSA_RSP_OK               /* 0x01       */
#define ACP_OK_CANCEL				(LSA_RSP_OK_OFFSET + 1)  /* 0x41 /  65 */

#define ACP_ERR_RESOURCE			LSA_RSP_ERR_RESOURCE     /* 0x84 / 132 */
#define ACP_ERR_PARAM				LSA_RSP_ERR_PARAM        /* 0x86 / 134 */
#define ACP_ERR_SEQUENCE			LSA_RSP_ERR_SEQUENCE     /* 0x87 / 135 */
#define ACP_ERR_SYS_PATH			LSA_RSP_ERR_SYS_PATH     /* 0x91 / 145 */

#define ACP_ERR_LOWER_LAYER			(LSA_RSP_ERR_OFFSET + 0) /* 0xc0 / 192 */
#define ACP_ERR_ALLOC_LOCAL_MEM		(LSA_RSP_ERR_OFFSET + 1) /* 0xc1 / 193 */
#define ACP_ERR_ALLOC_LOWER_RQB		(LSA_RSP_ERR_OFFSET + 2) /* 0xc2 / 194 */
#define ACP_ERR_UNSUPPORTED			(LSA_RSP_ERR_OFFSET + 3) /* 0xc3 / 195 */
#define ACP_ERR_ASRT				(LSA_RSP_ERR_OFFSET + 4) /* 0xc4 / 196 */
#define ACP_ERR_ALLOC_LOWER_MEM		(LSA_RSP_ERR_OFFSET + 5) /* 0xc5 / 197 */
#define ACP_ERR_EDD_PARAM			(LSA_RSP_ERR_OFFSET + 6) /* 0xc6 / 198 */
#define ACP_ERR_EDD_RESOURCE		(LSA_RSP_ERR_OFFSET + 7) /* 0xc7 / 199 */


/*-----------------------------------------------------------------------------
// forward declarations / common types
//---------------------------------------------------------------------------*/

typedef struct acp_rqb_tag ACP_UPPER_RQB_ATTR  * ACP_UPPER_RQB_PTR_TYPE;

/**/

typedef LSA_VOID ACP_UPPER_MEM_ATTR  * ACP_UPPER_MEM_PTR_TYPE;

/**/

typedef LSA_UINT8 ACP_UPPER_MEM_ATTR  * ACP_UPPER_MEM_U8_PTR_TYPE;

/**/

#define ACP_MAC_ADDR_SIZE	6

typedef LSA_UINT8  ACP_MAC_ADDR_TYPE[ACP_MAC_ADDR_SIZE];

/**/

#define ACP_IP_ADDR_SIZE	4

typedef LSA_UINT8  ACP_IP_ADDR_TYPE[ACP_IP_ADDR_SIZE]; /* IPv4, [0]..MSB  / network-format */

/**/

typedef LSA_UINT8 ACP_LOWER_MEM_ATTR  * ACP_LOWER_MEM_U8_PTR_TYPE;

/**/

#define ACP_ERROR_INFO_DATA_SIZE	EDD_DEBUG_INFO_SIZE

typedef LSA_UINT8  ACP_ERROR_INFO_DATA_TYPE[ACP_ERROR_INFO_DATA_SIZE];

typedef ACP_ERROR_INFO_DATA_TYPE  ACP_UPPER_MEM_ATTR  * ACP_UPPER_ERROR_INFO_DATA_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define ACP_INVALID_HANDLE	0x00

typedef  LSA_VOID  LSA_FCT_PTR(ACP_UPPER_OUT_FCT_ATTR, ACP_UPPER_CALLBACK_FCT_PTR_TYPE)(
	ACP_UPPER_RQB_PTR_TYPE  rb
);

struct acp_open_channel_tag
{
	LSA_HANDLE_TYPE		handle;				/* cnf: the new open handle from acp, input for all other requests */
	LSA_HANDLE_TYPE		handle_upper;		/* every confirmation of an request set this handle to rqb.handle */
	LSA_SYS_PATH_TYPE	sys_path;			/* use this targed, used by system adaption */
	ACP_UPPER_CALLBACK_FCT_PTR_TYPE
						acp_request_upper_done_ptr;
											/* the callback-function */
};

typedef struct acp_open_channel_tag					ACP_OPEN_CHANNEL_TYPE;

typedef ACP_OPEN_CHANNEL_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_OPEN_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

/* no type used */


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CHANNEL_INFO
//---------------------------------------------------------------------------*/

/* ::hardware_type */

#define ACP_HW_TYPE_USED_MSK				0x000000FF /* see EDD_HW_TYPE_USED_MSK */
#define ACP_HW_TYPE_USED_STANDARD_MAC		0x00000000 /* EDDS: standard MAC and software xRT support */
#define ACP_HW_TYPE_USED_ERTEC_400			0x00000001 /* EDDI: ERTEC-IP 400 hardware support */
#define ACP_HW_TYPE_USED_ERTEC_200			0x00000002 /* EDDI: ERTEC-IP 200 hardware support */
#define ACP_HW_TYPE_USED_SOC				0x00000003 /* EDDI: ERTEC-IP SoC hardware support */
#define ACP_HW_TYPE_USED_PNIP_200P			0x00000004 /* EDDP: PN-IP    200Plus hardware support */

#define ACP_HW_FEATURE_RESERVED_100_MSK						0x00000100 /* Bit 8: was LOCK Services allowed? */

#define ACP_HW_FEATURE_TIMESTAMP_MSK						0x00000200 /* Bit 9: Timestamps supported? */
#define ACP_HW_FEATURE_TIMESTAMP_SUPPORT					0x00000000 /* Yes */
#define ACP_HW_FEATURE_TIMESTAMP_NO_SUPPORT					0x00000200 /* No  */

#define ACP_HW_FEATURE_DELAY_MSK							0x00000400 /* Bit 10: Rx/Tx-Delays supported? */
#define ACP_HW_FEATURE_DELAY_SUPPORT						0x00000000 /* Yes */
#define ACP_HW_FEATURE_DELAY_NO_SUPPORT						0x00000400 /* No  */

#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS12_MSK				0x00000800 /* Bit 11: Partial Data supported for RTClass1/2? */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS12_SUPPORT			0x00000000 /* Yes */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS12_NO_SUPPORT		0x00000800 /* No  */

#define ACP_HW_FEATURE_RESERVED_12_MSK						0x00001000 /* Bit 12: reserved */

#define ACP_HW_FEATURE_CT_MSK								0x00002000 /* Bit 13: CT(Cut-Through) supported? */
#define ACP_HW_FEATURE_CT_SUPPORT							0x00000000 /* Yes */
#define ACP_HW_FEATURE_CT_NO_SUPPORT						0x00002000 /* No  */

#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS3_MSK				0x00004000 /* Bit 14: Partial Data supported for RTClass3 */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS3_SUPPORT			0x00000000 /* Yes */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS3_NO_SUPPORT		0x00004000 /* No  */

#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS_UDP_MSK			0x00008000 /* Bit 15: Partial Data supported for RTClass1/UDP */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS_UDP_SUPPORT		0x00000000 /* Yes */
#define ACP_HW_FEATURE_PARTIAL_DATA_CLASS_UDP_NO_SUPPORT	0x00008000 /* No  */

#define ACP_HW_FEATURE_AUTOPADDING_CLASS_UDP_MSK			0x00010000 /* Bit 16: Autopadding supported? */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS_UDP_SUPPORT		0x00000000 /* Yes */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS_UDP_NO_SUPPORT		0x00010000 /* No  */

#define ACP_HW_FEATURE_AUTOPADDING_CLASS12_MSK				0x00020000 /* Bit 17: Autopadding supported? */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS12_SUPPORT			0x00000000 /* Yes */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS12_NO_SUPPORT		0x00020000 /* No  */

#define ACP_HW_FEATURE_AUTOPADDING_CLASS3_MSK				0x00040000 /* Bit 18: Autopadding supported? */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS3_SUPPORT			0x00000000 /* Yes */
#define ACP_HW_FEATURE_AUTOPADDING_CLASS3_NO_SUPPORT		0x00040000 /* No  */

#define ACP_HW_FEATURE_APDUSTATUS_SEPERATE_MSK				0x00080000 /* Bit 19: is APDUStatus separated from frame? */
#define ACP_HW_FEATURE_APDUSTATUS_SEPERATE					0x00000000 /* Yes */
#define ACP_HW_FEATURE_APDUSTATUS_NOT_SEPERATE				0x00080000 /* No  */

#define ACP_HW_FEATURE_SYNC_SLAVE_MSK						0x00300000 /* Bits 20/21: Sync slave support */
#define ACP_HW_FEATURE_SYNC_SLAVE_NO						0x00000000
#define ACP_HW_FEATURE_SYNC_SLAVE_SW_SERVICES				0x00100000
#define ACP_HW_FEATURE_SYNC_SLAVE_HW_SERVICES				0x00200000

#define ACP_HW_FEATURE_SYNC_MASTER_MSK						0x00C00000 /* Bits 22/22: Sync master support */
#define ACP_HW_FEATURE_SYNC_MASTER_NO						0x00000000
#define ACP_HW_FEATURE_SYNC_MASTER_SW_SERVICES				0x00400000
#define ACP_HW_FEATURE_SYNC_MASTER_HW_SERVICES				0x00800000

#define ACP_HW_FEATURE_SYNC_DELAY_MSK						0x03000000 /* Bits 23/24: Sync delay support */
#define ACP_HW_FEATURE_SYNC_DELAY_NO						0x00000000
#define ACP_HW_FEATURE_SYNC_DELAY_SW_SERVICES				0x01000000
#define ACP_HW_FEATURE_SYNC_DELAY_HW_SERVICES				0x02000000

/* ::kram_info, only valid with EDDI */

struct acp_kram_info_tag
{
	LSA_UINT32		kram_base_addr;
	LSA_UINT32		offset_process_image_start;
	LSA_UINT32		offset_async_in_start;		/* only ERTEC 200 */
	LSA_UINT32		size_async_in;				/* only ERTEC 200 */
	LSA_UINT32		offset_async_out_start;		/* only ERTEC 200 */
	LSA_UINT32		size_async_out;				/* only ERTEC 200 */
	LSA_UINT32		offset_process_image_end;
};

typedef struct acp_kram_info_tag	ACP_KRAM_INFO_TYPE;


struct acp_channel_info_tag
{
	LSA_UINT32	hardware_type;				/* cnf: information about the used lower EDD */
	LSA_UINT16	cycle_base_factor;			/* cnf: the basic cycle factor; cycle_time = cycle_base_facto * 31.25us */

	LSA_UINT16	edd_io_datalen_min;			/* cnf: minimum add_provider/add_consumer.len */
	LSA_UINT16	edd_io_datalen_max;			/* cnf: maximum add_provider/add_consumer.len */

	LSA_UINT16	edd_io_udp_datalen_min;		/* cnf: minimum RT-over-UDP add_provider/add_consumer.len */
	LSA_UINT16	edd_io_udp_datalen_max;		/* cnf: maximum RT-over-UDP add_provider/add_consumer.len */

	LSA_UINT16	frame_id_base_class1_unicast;
					/* cnf: the first available unicast frame_id of the consumer range of the class 1 */
	LSA_UINT16	nr_of_consumer_class1_unicast;
					/* cnf: number of available unicast consumer of the class 1 */
	LSA_UINT16	frame_id_base_class1_multicast;
					/* cnf: the first available multicast frame_id of the consumer range of the class 1 */
	LSA_UINT16	nr_of_consumer_class1_multicast;
					/* cnf: number of available consumer multicast of the class 1 */

	LSA_UINT16	frame_id_base_class2_unicast;
					/* cnf: the first available unicast frame_id of the consumer range of the class 2 */
	LSA_UINT16	nr_of_consumer_class2_unicast;
					/* cnf: number of available unicast consumer of the class 2 */
	LSA_UINT16	frame_id_base_class2_multicast;
					/* cnf: the first available multicast frame_id of the consumer range of the class 2 */
	LSA_UINT16	nr_of_consumer_class2_multicast;
					/* cnf: number of available multicast consumer of the class 2 */

	LSA_UINT16	nr_of_provider_class1_2;
					/* cnf: number of available provider for the class 1 and the class 2 */
	LSA_BOOL	consumer_class1_2_is_combined;
					/* cnf: intepret the values of nr_of_consumer_class1 and nr_of_consumer_class2 */
					/*   false..max(cons_class1) == nr_of_consumer_class1 && max(cons_class2) == nr_of_consumer_class2 */
					/*   true...max(cons_class1) + max(cons_class1) == nr_of_consumer_class1 */
					/*           and nr_of_consumer_class2 == nr_of_consumer_class1 */

	LSA_UINT16	nr_of_consumer_class3;		/* cnf: number of available consumer of the class 3 */

	LSA_UINT16	nr_of_provider_class3;		/* cnf: number of available provider of the class 3 */

	LSA_UINT16	eth_interface_id;			/* cnf: id (label) of the ethernet-connector */
	LSA_UINT16	eth_ports_max;				/* cnf: number of supported ports */
	ACP_MAC_ADDR_TYPE
				eth_mac_addr;				/* cnf: the ethernet-MAC-addresse of the used device */

	LSA_UINT16	udp_port;					/* cnf: the UDP-port for RT-over-UDP (network byte order) */
	ACP_IP_ADDR_TYPE
				ip_addr;					/* cnf: the IP-addresse of the used device (RT-over-UDP) */

	LSA_UINT16	alarm_ref_base;				/* cnf: the first available referenc-number of an alarm */
	LSA_UINT16	alarm_nr_of_ref;			/* cnf: number of available references */

	LSA_UINT32	provider_max_phase_byte;	/* cnf: 0..no limit; limit of provider_add */
	LSA_UINT32	provider_max_phase_count;	/* cnf: 0..no limit; limit of provider_add  */

	ACP_KRAM_INFO_TYPE
				kram_info;					/* cnf: KRAM info, only valid with EDDI */

};

typedef struct acp_channel_info_tag					ACP_CHANNEL_INFO_TYPE;

typedef ACP_CHANNEL_INFO_TYPE ACP_UPPER_MEM_ATTR *	ACP_CHANNEL_UPPER_INFO_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_PROVIDER_ADD
//---------------------------------------------------------------------------*/

/* all data specific to EDDx */
struct acp_prov_ioparam_tag
{
	LSA_UINT32			properties;			/* add/cntrl: see EDD_PROV_BUFFER_PROP_xxx */
	LSA_UINT32			data_offset;		/* add/cntrl: see EDD_DATAOFFSET_xxx */
	LSA_UINT16			cr_number;			/* add/cntrl: see EDD_CRNUMBER_xxx */

	LSA_UINT16			edd_prov_id;		/* add-cnf: the edd_id of for the new provider */
	LSA_UINT32			cw_offset;			/* add-cnf: the cw_offset for the new provider */
	LSA_UINT32			data_status_offset;	/* add-cnf: the offset to the data-status */

	LSA_UINT8			app_group;			/* add: 0=no-group(default), 1..2 */

	LSA_UINT8			sf_position;		/* add/cntrl: 1..127 Position; EDD_SFPOSITION_UNDEFINED -> will be set with _CONTROL */
	LSA_UINT32			sf_offset;			/* add/cntrl: 0..xx Offset; EDD_SFOFFSET_UNDEFINED -> will be set with _CONTROL */
};

typedef struct acp_prov_ioparam_tag		ACP_PROV_IOPARAM_TYPE;

/* ::frame_id */
#define ACP_INVALID_FRAME_ID				0xFFFF /* invalid frame_id */

/* ::prov_id */
#define ACP_INVALID_ACP_ID					0 /* invalid prov_id */

/*
** not yet :: DFP, FastForwardingMACAdr, DistributedSubFrameWatchDog, FullSubFrameStructure
*/

/* default mode after add */
#define ACP_PROVIDER_SET_DEFAULT		\
	(ACP_PROVIDER_SET_PASSIVE)

struct acp_provider_add_tag
{
	LSA_UINT16			prov_id;			/* cnf: the prov_id of ACP for the new provider */
	LSA_UINT16			properties;			/* see EDD_CSRT_PROV_PROP_xxx */

	LSA_UINT16			frame_id;			/* the frame_id of the sending frame */
	LSA_UINT16			length;				/* the length of the usable data */
	LSA_UINT16			reduction_ratio;	/* see the cyclic sending args */
	LSA_UINT16			phase;				/* -"- */
	LSA_UINT32			sequence;			/* -"- */
	LSA_UINT32			usr_id;				/* the id of the user for this provider, the user recv this usr_id within the indication */
	ACP_PROV_IOPARAM_TYPE
						ioparam;			/* used by EDDx, see there */
};

typedef struct acp_provider_add_tag					ACP_PROVIDER_ADD_TYPE;

typedef ACP_PROVIDER_ADD_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_PROVIDER_ADD_PTR_TYPE;

/*
//	ad) the cyclic sending args
//
//	send_clock:
//		basic cyclic sending timer, see ACP_CHANNEL_INFO_TYPE::cycle_base_factor
//
//	reduction_ratio:
//		(sending|checking) frame on every  "send_clock * reduction_ratio"  tick
//
//	phase:
//		there are  "1 .. reduction_ratio"  possible (sending|checking) ticks
//
//	sequence:
//		the order of frames withing one (sending|checking) tick
//		0 means insert at last position
*/


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_PROVIDER_REMOVE
//---------------------------------------------------------------------------*/

struct acp_provider_remove_tag
{
	LSA_UINT16		prov_id;	/* the id of ACP for the provider which must be removed */
};

typedef struct acp_provider_remove_tag					ACP_PROVIDER_REMOVE_TYPE;

typedef ACP_PROVIDER_REMOVE_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_PROVIDER_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_PROVIDER_CONTROL
//---------------------------------------------------------------------------*/

/* ::mode */
/* group Passive <-> Active */
#define ACP_PROVIDER_SET_PASSIVE			0x0001
#define ACP_PROVIDER_SET_ACTIVE				0x0002

/* legacy mode only: Passive-AUX <-> Active-AUX */
#define ACP_PROVIDER_SET_PASSIVE_AUX		0x0004
#define ACP_PROVIDER_SET_ACTIVE_AUX			0x0008

/* group Station_Failure <-> Station_Ok */
#define ACP_PROVIDER_SET_STATION_FAILURE	0x0100
#define ACP_PROVIDER_SET_STATION_OK			0x0200

/* evaluate parameter autostop_cons_id */
#define ACP_PROVIDER_SET_AUTOSTOP			0x0400

/* evaluate parameter ioparam */
#define ACP_PROVIDER_SET_IOPARAM			0x0800

/* only meaningful with set-passive: reason for passivating is a ResetToFactory, see EDD_PROV_CONTROL_MODE_REMOVE_ACW */
#define ACP_PROVIDER_RESET_TO_FACTORY		0x1000

/* a bit for the ACP-user, not used by ACP */
#define ACP_PROVIDER_SET_USER				0x8000

struct acp_provider_control_tag
{
	LSA_UINT16			prov_id;			/* the id of ACP for the provider which must be controlled */
	LSA_UINT16			mode;				/* the mode specifies what has to change */
	LSA_UINT16			autostop_cons_id;	/* if mode.ACP_PROVIDER_SET_AUTOSTOP: */
											/* if autostop_cons_id == ACP_INVALID_ACP_ID: remove the autostop-feature */
											/* else: if the autostop_cons_id(acp_id) got MISS, autostop this provider (means passivate) */
	LSA_UINT16			frame_id;			/* the frame_id of the sending frame */
	ACP_MAC_ADDR_TYPE	dst_mac;			/* the ethernet-address of the consumer / maybe the gateway-addr with RT-over-UDP */
	ACP_IP_ADDR_TYPE	dst_ip;				/* RT-over-UDP: the ip-address of the consumer  else: don't care*/
	LSA_UINT16			reduction_ratio;	/* if ACP_PROVIDER_SET_ACTIVE_xxx: see the cyclic sending args */
	LSA_UINT16			phase;				/* if ACP_PROVIDER_SET_ACTIVE_xxx: -"- */
	LSA_UINT32			sequence;			/* if ACP_PROVIDER_SET_ACTIVE_xxx: -"- */
	ACP_PROV_IOPARAM_TYPE		ioparam;	/* if ACP_PROVIDER_SET_IOPARAM: used by EDDx, see there */
	ACP_ERROR_INFO_DATA_TYPE	debug_data;	/* valid if rsp != OK, see ACP_OPC_ALARM_CLOSE */
};

typedef struct acp_provider_control_tag					ACP_PROVIDER_CONTROL_TYPE;

typedef ACP_PROVIDER_CONTROL_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_PROVIDER_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_PROVIDER_CHANGE_PHASE
//---------------------------------------------------------------------------*/

struct acp_provider_change_phase_tag
{
	LSA_UINT16		prov_id;	/* the id of ACP for the provider which phase should be moved */
	LSA_UINT16		phase;		/* the new phase of the provider, see _add */
	LSA_UINT32		sequence;	/* the new sequence within the phase of the provider, see _add */
};

typedef struct acp_provider_change_phase_tag				ACP_PROVIDER_CHANGE_PHASE_TYPE;

typedef ACP_PROVIDER_CHANGE_PHASE_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_PROVIDER_CHANGE_PHASE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_PROVIDER_XCHANGE_BUF
//---------------------------------------------------------------------------*/

/* currently only valid with EDDI */

struct acp_provider_xchange_buf_tag
{
	LSA_UINT32		new_offset;	/* new offset */
	LSA_UINT32		old_offset;	/* the old offset, for validation purpose */
	LSA_UINT32		data_len;	/* the length of the usable data, must same as within provider_add */
	LSA_UINT16		prov_id;	/* the id of ACP for the provider which KRAM-offset should be moved */
};

typedef struct acp_provider_xchange_buf_tag					ACP_PROVIDER_XCHANGE_BUF_TYPE;

typedef ACP_PROVIDER_XCHANGE_BUF_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_PROVIDER_XCHANGE_BUF_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_GLOBAL_PROVIDER_CONTROL
//---------------------------------------------------------------------------*/

/*	::mode */

/* group name Stop <-> Run */
#define ACP_GLOBAL_PROVIDER_SET_STOP			0x0001
#define ACP_GLOBAL_PROVIDER_SET_RUN				0x0002

/* a bit for the ACP-user, not used by ACP */
#define ACP_GLOBAL_PROVIDER_SET_USER			0x8000


#define ACP_GLOBAL_PROVIDER_SET_DEFAULT		\
	(ACP_GLOBAL_PROVIDER_SET_RUN)

struct acp_global_provider_control_tag
{
	LSA_UINT16			mode;				/* 'or' the needed mode options */
};

typedef struct acp_global_provider_control_tag					ACP_GLOBAL_PROVIDER_CONTROL_TYPE;

typedef ACP_GLOBAL_PROVIDER_CONTROL_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_GLOBAL_PROVIDER_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CONSUMER_ADD
//---------------------------------------------------------------------------*/

/* all data specific to EDDx */
struct acp_cons_ioparam_tag					/* add/cntrl means, can set within add or later with control */
{
	LSA_UINT32			properties;			/* add/cntrl: see EDD_CONS_BUFFER_PROP_xxx */
	LSA_UINT32			data_offset;		/* add/cntrl: see EDD_DATAOFFSET_xxx */
	LSA_UINT16			cr_number;			/* add/cntrl: see EDD_CRNUMBER_xxx */
	LSA_UINT8			forward_mode;		/* add/cntrl: defines forwarding of I/O-Data to memory, see EDD_FORWARDMODE_xxx */
    LSA_UINT32			ct_outdatastart;	/* add/cntrl: used if forward_mode is set to EDD_FORWARDMODE_SF; Range: 0..0x7FF */

/* nobody needs
//	LSA_UINT16			edd_cons_id;		// add-cnf: the consumer-handle from the unterlaying edd */
	LSA_UINT32			apdu_status_offset;	/* add-cnf: offset to the APDU_Status */

	LSA_UINT8			sf_position;		/* add/cntrl: 1..127 Position; EDD_SFPOSITION_UNDEFINED -> will be set with _CONTROL */
	LSA_UINT32			sf_offset;			/* add/cntrl: 0..xx Offset; EDD_SFOFFSET_UNDEFINED -> will be set with _CONTROL */

	LSA_UINT8			app_group;			/* add: 0..no, 1, 2 */
	LSA_BOOL			clear_on_miss;		/* add: on Consumer MISS or on passivation of the consumer the next buffer will return cleared (all 0) data */
};

typedef struct acp_cons_ioparam_tag		ACP_CONS_IOPARAM_TYPE;


/* ACP_INVALID_FRAME_ID: invalid frame_id */
/* ACP_INVALID_ACP_ID: invalid cons_id */
/* ACP_PROP_*: properties */

/* default mode after add */
#define ACP_CONSUMER_SET_DEFAULT			ACP_CONSUMER_SET_PASSIVE

struct acp_consumer_add_tag
{
	LSA_UINT16			cons_id;			/* cnf: the id of ACP for the new consumer */
	LSA_UINT16			properties;			/* see EDD_CSRT_CONS_PROP_xxx */

	LSA_UINT16			frame_id;			/* the frame_id of the receiving frame */
	LSA_UINT16			length;				/* the length of the usable data */
	LSA_UINT16			partial_length;		/* partial data length: 0=no, 1..data_length-1 */
	LSA_UINT16			partial_offset;		/* partial data length: 0=no, 1..data_length-partial_length */
	LSA_UINT16			reduction_ratio;	/* see the cyclic sending args */
	LSA_UINT16			phase;				/* -"- */
	/* LSA_UINT16 		watchdog_factor ... removed with V5.3i1.2 (AP01448175) */
	LSA_UINT16			data_hold_factor;	/* Factor to check for DataHold */
	LSA_UINT32			usr_id;				/* the id of the user for this consumer, the user recv this usr_id within the indication */
	ACP_CONS_IOPARAM_TYPE
						ioparam;			/* used by EDDx, see there */
};

typedef struct acp_consumer_add_tag					ACP_CONSUMER_ADD_TYPE;

typedef ACP_CONSUMER_ADD_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_CONSUMER_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CONSUMER_REMOVE
//---------------------------------------------------------------------------*/

struct acp_consumer_remove_tag
{
	LSA_UINT16			cons_id;			/* the id of ACP for the consumer which must be removed */
};

typedef struct acp_consumer_remove_tag					ACP_CONSUMER_REMOVE_TYPE;

typedef ACP_CONSUMER_REMOVE_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_CONSUMER_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CONSUMER_CONTROL
//---------------------------------------------------------------------------*/

/*	::mode */
/* group Passive <-> Active */
#define ACP_CONSUMER_SET_PASSIVE				0x0001
#define ACP_CONSUMER_SET_ACTIVE					0x0002

/* retrigger consumer indications AGAIN / MISS */
#define ACP_CONSUMER_SET_TO_UNKNOWN_STATE		0x0004

/* enable or disable provider-autostop, see provider_control::autostop_cons_id */
#define ACP_CONSUMER_SET_AUTOSTOP_ENABLE		0x0100
#define ACP_CONSUMER_SET_AUTOSTOP_DISABLE		0x0200

/* only meaningful with set-passive: reason for passivating is a ResetToFactory, see EDD_CONS_CONTROL_MODE_REMOVE_ACW */
#define ACP_CONSUMER_RESET_TO_FACTORY			0x1000

/* a bit for the ACP-user, not used by ACP */
#define ACP_CONSUMER_SET_USER					0x8000

struct acp_consumer_control_tag
{
	LSA_UINT16			cons_id;			/* the id of ACP for the consumer which must be controlled */
	LSA_UINT16			mode;				/* ::mode */
	LSA_UINT16			reduction_ratio;	/* if ACP_CONSUMER_SET_ACTIVE: see the cyclic sending args */
	LSA_UINT16			phase;				/* if ACP_CONSUMER_SET_ACTIVE: -"- */
	ACP_MAC_ADDR_TYPE	src_mac;			/* if ACP_CONSUMER_SET_ACTIVE: if RTClass123: the ethernet-addresse of the provider / maybe the gateway-addr with RT-over-UDP */
	ACP_IP_ADDR_TYPE	src_ip;				/* if ACP_CONSUMER_SET_ACTIVE: if RT-over-UDP: the ip-address of the provider */
	ACP_CONS_IOPARAM_TYPE		ioparam;	/* if ACP_CONSUMER_SET_ACTIVE: used by EDDx, see there */
	ACP_ERROR_INFO_DATA_TYPE	debug_data;	/* valid if rsp != OK, see ACP_OPC_ALARM_CLOSE */
};

typedef struct acp_consumer_control_tag					ACP_CONSUMER_CONTROL_TYPE;

typedef ACP_CONSUMER_CONTROL_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_CONSUMER_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_CONSUMER_XCHANGE_BUF
//---------------------------------------------------------------------------*/

/* currently only valid with EDDI */

struct acp_consumer_xchange_buf_tag
{
	LSA_UINT32			new_offset;			/* new offset */
	LSA_UINT32			old_offset;			/* the old offset, for validation purpose */
	LSA_UINT32			data_len;			/* the length of the usable data, must same as within consumer_add */
	LSA_UINT16			cons_id;			/* the id of ACP for the consumer which KRAM-offset should be moved */
	LSA_BOOL			data_copy;			/* true ..copy data + apdu-status to new kram-buffer */
											/* false..user must wait for new receiving frame to obtail valid data */
	LSA_UINT32			apdu_status_offset;	/* cnf: used by EDDI, offset to the APDU_Status */
};

typedef struct acp_consumer_xchange_buf_tag					ACP_CONSUMER_XCHANGE_BUF_TYPE;

typedef ACP_CONSUMER_XCHANGE_BUF_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_CONSUMER_XCHANGE_BUF_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: ACP_OPC_STATE_IND_RES_PROVIDE
// INDICATION: ACP_OPC_STATE_INDICATION
//---------------------------------------------------------------------------*/

/* ::state_event */
/* Consumer-Indication */
/* group Miss <-> Again */
#define ACP_STATE_MISS					EDD_CSRT_CONS_EVENT_MISS
#define ACP_STATE_MISS_NOT_STOPPED		EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED
#define ACP_STATE_AGAIN					EDD_CSRT_CONS_EVENT_AGAIN

/* group Run <-> Stop */
#define ACP_STATE_RUN					EDD_CSRT_CONS_EVENT_RUN
#define ACP_STATE_STOP					EDD_CSRT_CONS_EVENT_STOP

/* group Station_Ok <-> Station_Failure */
#define ACP_STATE_STATION_OK			EDD_CSRT_CONS_EVENT_STATION_OK
#define ACP_STATE_STATION_FAILURE		EDD_CSRT_CONS_EVENT_STATION_FAILURE

/* group Primary <-> Backup */
#define ACP_STATE_PRIMARY				EDD_CSRT_CONS_EVENT_PRIMARY
#define ACP_STATE_BACKUP				EDD_CSRT_CONS_EVENT_BACKUP

/* group consumer is "Received in Red" (note: ACP_STATE_RED_LEAVING removed with V5.3i1.2 (AP01448175)) */
#define ACP_STATE_RED_ENTERING				EDD_CSRT_CONS_EVENT_ENTERING_RED

/* group system redundancy */
#define ACP_STATE_SYSRED_NO_PRIMARY_AR_EXISTS		EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS
#define ACP_STATE_SYSRED_PRIMARY_AR_EXISTS			EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS

/* initial consumer state (see EDD_USR.doc, there is no #define) */
#define ACP_STATE_CONSUMER_DEFAULT \
	( ACP_STATE_MISS_NOT_STOPPED | ACP_STATE_RUN \
	| ACP_STATE_STATION_OK       | ACP_STATE_PRIMARY \
	| ACP_STATE_SYSRED_PRIMARY_AR_EXISTS \
	)

#define ACP_STATE_CONSUMER_SYSRED_DEFAULT \
	( ACP_STATE_MISS_NOT_STOPPED | ACP_STATE_RUN \
	| ACP_STATE_STATION_OK       | ACP_STATE_BACKUP \
	| ACP_STATE_SYSRED_PRIMARY_AR_EXISTS \
	)
/* if SYSRED => PRIMRAY exists ?  defined in EDD_USR.DOC	B0.05	2011-08-05 !!! */

/* initial provider state */
#define ACP_STATE_PROVIDER_DEFAULT \
	(0x00000000)

struct acp_state_indication_tag
{
	LSA_UINT32			usr_id;				/* the id from the user, given with provider_add or consumer_add */
	LSA_UINT32			state_event;		/* the indications ACP_STATE_*  (or'ed) */
	LSA_UINT32			total_state;		/* the total state of all signaled events */
	LSA_UINT16			acp_id;				/* the unique id from ACP */
	LSA_UINT16			cycle_count;		/* valid only with MISS, the last received cycle counter */
	ACP_ERROR_INFO_DATA_TYPE	debug_data;	/* valid only with MISS, see ACP_OPC_ALARM_CLOSE */
};

typedef struct acp_state_indication_tag					ACP_STATE_INDICATION_TYPE;

typedef ACP_STATE_INDICATION_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_STATE_INDICATION_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_GET_APDU_STATUS
//----------------------------------------------------------------------------*/

/* Data-Status Bits                                                          */
/*                                                                           */
/* Bit| Name           | Values      | Handling                              */
/* ---+----------------+-------------+---------------------------------------*/
/*  0 | State          | 0: BACKUP   | handled by application                */
/*    |                | 1: PRIMARY  |                                       */
/* ---+----------------+-------------+---------------------------------------*/
/*  1 | Redundancy     | 0           | discard frame if not 0                */
/*                                   | @todo: better text                    */
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
/*  7 | reserved       | 0           | discard frame if not 0                */
/* ---+----------------+-------------+---------------------------------------*/

#define ACP_DATASTATUS_PRIMARY			0x01 /* see EDD_CSRT_DSTAT_BIT_STATE */
#define ACP_DATASTATUS_REDUNDANCY		0x02 /* see EDD_CSRT_DSTAT_BIT_REDUNDANCY */
#define ACP_DATASTATUS_DATA_VALID		0x04 /* see EDD_CSRT_DSTAT_BIT_DATA_VALID */
#define ACP_DATASTATUS_BIT_3			0x08 /* see EDD_CSRT_DSTAT_BIT_3 */
#define ACP_DATASTATUS_RUN				0x10 /* see EDD_CSRT_DSTAT_BIT_STOP_RUN */
#define ACP_DATASTATUS_STATION_OK		0x20 /* see EDD_CSRT_DSTAT_BIT_STATION_FAILURE */
#define ACP_DATASTATUS_BIT_6			0x40 /* see EDD_CSRT_DSTAT_BIT_6 */
#define ACP_DATASTATUS_BIT_7			0x80 /* see EDD_CSRT_DSTAT_IGNORE */

struct acp_get_apdu_status_tag
{
	LSA_UINT16	acp_id;				/* req: the unique id from ACP */

	LSA_UINT16	cycle_count;		/* cnf: CycleCounter. 31.25us based */
	LSA_UINT8	data_status;		/* cnf: DataStatus */
	LSA_UINT8	transfer_status;	/* cnf: TransferStatus */
	LSA_BOOL	present;		    /* cnf: LSA_TRUE ... cycle_count,data_status,transfer_status is valid */
									/*      LSA_FALSE .. not valid, maybe no consumer received yet */
};

typedef struct acp_get_apdu_status_tag					ACP_GET_APDU_STATUS_TYPE;

typedef ACP_GET_APDU_STATUS_TYPE  ACP_UPPER_MEM_ATTR *	ACP_UPPER_GET_APDU_STATUS_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_ALARM_OPEN
// OPCODE: ACP_OPC_UDP_ALARM_OPEN
//----------------------------------------------------------------------------*/

/* alarm::alarm_ref, alarm::dst_alarm_ref */
#define ACP_INVALID_ALARM_REF	0xFFFF

/* alarm::properties */
#define ACP_AL_PROP_USE_GATEWAY_MASK	0x0001
#define ACP_AL_PROP_USE_GATEWAY_NO		0x0000
#define ACP_AL_PROP_USE_GATEWAY_YES		0x0001

#define ACP_AL_PROP_RESERVED			0xFFFE

struct acp_alarm_open_tag
{
	LSA_UINT16			alarm_ref;		/* IN,KEY the local alarm-relation */
	LSA_UINT16			dst_alarm_ref;	/* the alarm-reference of the alarm-destination */
	LSA_UINT16			properties;		/* see alarm::properties */
	ACP_MAC_ADDR_TYPE	dst_mac;		/* the ethernet-address of the alarm-destination */
	ACP_IP_ADDR_TYPE	dst_ip;			/* RTA-over-UDP: the ip-address of the alarm-destination  else: don't care */
	ACP_IP_ADDR_TYPE	src_ip;			/* RTA-over-UPD: the ip-address of the alarm-source  else: dont't care */
	LSA_UINT32			usr_id;			/* a usr_id, inserted to alarm_indication related to alarm_ref */
	LSA_UINT16			vlan_id;		/* the ethernet-frame is taged with this VLANID */
	LSA_UINT8			timeout100ms;	/* timeout for the needed transport-ack */
	LSA_UINT8			retry_count;	/* how many send repetitions on missing transport-ack */
};

typedef struct acp_alarm_open_tag					ACP_ALARM_OPEN_TYPE;

typedef ACP_ALARM_OPEN_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_ALARM_OPEN_PTR_TYPE;

/* alarm_ref:	(alarm_ref_base <= alarm_ref < alarm_ref_base + alarm_nr_of_ref) */
/* dst_mac:		maybe the gateway-addr with RT-over-UDP */


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_ALARM_CLOSE
//----------------------------------------------------------------------------*/

/* normative:
**	VendorDeviceErrorInfo			::= VendorID, DeviceID, Data*
**
** our define:
**	Vendor(Siemens)DeviceErrorInfo	::= VendorID_u16(0x002a), DeviceID_u16, PNIOVersion_u32, DebugIdent_u8, DebugData_u8 x 15
**
**  sizeof(VendorDeviceErrorInfo)			<= 24
**  sizeof(Vendor(Siemens)DeviceErrorInfo)	== 24
**
*/

#if ACP_ERROR_INFO_DATA_SIZE != 16
# error "ACP != EDD"
#endif

typedef struct acp_error_info_tag
{
	LSA_UINT16  vendor_id;		/* the VendorID */
	LSA_UINT16  device_id;		/* the DeviceID */
	LSA_UINT32  pnio_version;	/* the version of the PNIO stack, used by VendorDeviceErrorInfo */
								/*	must be 0 if VendorDeviceErrorInfo shall not used within ERR-RTA-PDU */
	ACP_ERROR_INFO_DATA_TYPE  debug_data;
								/* format: DebugIdent, DebugData */
								/* must filled by caller */
} ACP_ERROR_INFO_TYPE;


#define ACP_PNIO_STATUS_REASON_MIN	2

typedef struct acp_alarm_close_tag
{
	LSA_UINT16			alarm_ref;			/* the handle to the alarm-relation */
	LSA_UINT8			pnio_status_reason;
	ACP_ERROR_INFO_TYPE	error_info;			/* optional block VendorDeviceErrorInfo */
	
} ACP_ALARM_CLOSE_TYPE;

typedef ACP_ALARM_CLOSE_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_ALARM_CLOSE_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_ALARM_SEND
// OPCODE: ACP_OPC_ALARM_SEND_ACK
// OPCODE: ACP_OPC_ALARM_IND_RES_PROVIDE
// INDICATION: ACP_OPC_ALARM_INDICATION
// INDICATION: ACP_OPC_ALARM_ACK_INDICATION
//----------------------------------------------------------------------------*/

#define ACP_ALARM_PRIORITY_LOW			0
#define ACP_ALARM_PRIORITY_HIGH			1

/* informative: existing alarm_type within IEC 61158-6
	0x0000	reserved
	0x0001	Diagnosis appears
	0x0002	Process
	0x0003	Pull
	0x0004	Plug
	0x0005	Status
	0x0006	Update
	0x0007	Redundancy
	0x0008	Controlled by supervisor
	0x0009	Released
	0x000A	Plug Wrong Submodule
	0x000B	Return of Submodule
	0x000C	Diagnosis disappears
	0x000D	Multicast Communication Mismatch
	0x000E	Port Data Change Notification
	0x000F	Sync Data Changed Notification
	0x0010	Isochronous Mode Problem Notification
	0x0011	Network component problem notification
	0x0012	Time data changed notification
	0x0013	Dynamic Frame Packing problem notification
	0x0014	MRPD problem notification
	0x0015 - 0x001D	Reserved
	0x001E	Upload and retrieval notification
	0x001F	Pull module b
	0x0020 - 0x007F	Manufacturer specific
	0x0080 - 0x00FF	Reserved for profiles
	0x0100 - 0xFFFF	Reserved
*/

#define ACP_SET_ALARM_SPECIFIER(\
	alarm_specifier_ptr, sequence_nr, diag_channel, diag_generic, diag_submod, ar_diag_state\
) {\
	*(alarm_specifier_ptr) = (LSA_UINT16)( \
			((((ar_diag_state) ? 0x1 : 0x0)  &   0x01) << 15) | \
			((((diag_submod)   ? 0x1 : 0x0)  &   0x01) << 13) | \
			((((diag_generic)  ? 0x1 : 0x0)  &   0x01) << 12) | \
			((((diag_channel)  ? 0x1 : 0x0)  &   0x01) << 11) | \
			(((sequence_nr)                  & 0x07FF) <<  0)   \
		);\
}

#define ACP_GET_ALARM_SPECIFIER(\
	sequence_nr_ptr, diag_channel_ptr, diag_generic_ptr, diag_submod_ptr, ar_diag_state_ptr, alarm_specifier\
) {\
	*(ar_diag_state_ptr) =   (LSA_BOOL)((((alarm_specifier) >> 15) &   0x01) ? LSA_TRUE : LSA_FALSE); \
	*(diag_submod_ptr)   =   (LSA_BOOL)((((alarm_specifier) >> 13) &   0x01) ? LSA_TRUE : LSA_FALSE); \
	*(diag_generic_ptr)  =   (LSA_BOOL)((((alarm_specifier) >> 12) &   0x01) ? LSA_TRUE : LSA_FALSE); \
	*(diag_channel_ptr)  =   (LSA_BOOL)((((alarm_specifier) >> 11) &   0x01) ? LSA_TRUE : LSA_FALSE); \
	*(sequence_nr_ptr)   = (LSA_UINT16)( ((alarm_specifier) >>  0) & 0x07FF); \
}

#define ACP_PNIO_STATUS_OK				0x0

#define ACP_ALARM_USER_DATA_OFFSET		28	/* see 61158-6: AlarmNotification-PDU.AlarmItem.Data */

struct acp_alarm_data_tag
{
	LSA_UINT16		alarm_ref;			/* req/ind: see alarm_open */
	LSA_UINT16		priority;			/* req/ind: see ACP_ALARM_PRIORITY_xxx */
	LSA_UINT32		usr_id;				/* ind: see ACP_OPC_ALARM_OPEN */

	LSA_UINT32		api;				/* req/ind: application process identifier */
	LSA_UINT16		alarm_type;			/* req/ind: see ACP_ALARM_TYPE_xxx */
	LSA_UINT16		slot_nr;			/* reg/ind: */
	LSA_UINT16		subslot_nr;			/* reg/ind: */
	LSA_UINT16		alarm_specifier;	/* req/ind: see ACP_SET_ALARM_SPECIFIER */

		/* for ALARM_ACK */
	LSA_UINT32		pnio_status;		/* reg: pnio status */
										/* ind: ACP_PNIO_STATUS_OK or received data */
		/* for ALARM */
	LSA_UINT32		module_ident;		/* req/ind: */
	LSA_UINT32		submodule_ident;	/* reg/ind: */

	LSA_UINT16		user_tag;			/* req: a user specific tag; identify the user_data structure */
	LSA_UINT16		user_max_length;	/* req: length of user_data; must >= ACP_ALARM_USER_DATA_OFFSET + user_length */
	LSA_UINT16		user_length;		/* req: 0..no user_tag, >0 user_tag and user_data will send within this alarm-pdu */
										/* ind: 0     ..no user_tag */
										/*      0xFFFF..more user_data than user_max_length */
										/*      >0    ..user_tag and user_data are valid */
	ACP_UPPER_MEM_U8_PTR_TYPE
					user_data;			/* req: user_data; range 0..user_length-1 */
										/* ind: user_data; range ACP_ALARM_USER_DATA_OFFSET..user_length-1 */
										/*      part of alarm-pdu; range 0..ACP_ALARM_USER_DATA_OFFSET-1 */
};

typedef struct acp_alarm_data_tag					ACP_ALARM_DATA_TYPE;

typedef ACP_ALARM_DATA_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_ALARM_DATA_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_LINK_STATUS_IND_RES_PROVIDE
// INDICATION: ACP_OPC_LINK_STATUS_INDICATION
//----------------------------------------------------------------------------*/

#define ACP_PORT_ID_AUTO			0 /* see EDD_PORT_ID_AUTO */

#define ACP_LINK_STATUS_UNKNOWN		0 /* see EDD_LINK_UNKNOWN */
#define ACP_LINK_STATUS_UP			1 /* see EDD_LINK_UP */
#define ACP_LINK_STATUS_DOWN		2 /* see EDD_LINK_DOWN */
#define ACP_LINK_STATUS_UP_CLOSED	3 /* see EDD_LINK_UP_CLOSED */
#define ACP_LINK_STATUS_UP_DISABLED	4 /* see EDD_LINK_UP_DISABLED */

#define ACP_LINK_SPEED_UNKNOWN		0 /* see EDD_LINK_UNKNOWN */
#define ACP_LINK_SPEED_10_M			1 /* see EDD_LINK_SPEED_10 */
#define ACP_LINK_SPEED_100_M		2 /* see EDD_LINK_SPEED_100 */
#define ACP_LINK_SPEED_1_G			3 /* see EDD_LINK_SPEED_1000 */
#define ACP_LINK_SPEED_10_G			4 /* see EDD_LINK_SPEED_10000 */

#define ACP_LINK_MODE_UNKNOWN		0 /* see EDD_LINK_UNKNOWN */
#define ACP_LINK_MODE_HALF_DUPLEX	1 /* see EDD_LINK_MODE_HALF */
#define ACP_LINK_MODE_FULL_DUPLEX	2 /* see EDD_LINK_MODE_FULL */

struct acp_link_status_indication_tag
{
	LSA_UINT16		port_id;		/* req: 1-0xFF..use this port; ACP_PORT_ID_AUTO..ACP select the needed Port */
									/* ind: unchanged */
	LSA_UINT8		status;			/* reg: set to your known state */
									/* ind: see ACP_LINK_STATUS_xxx */
	LSA_UINT8		speed;			/* req: set to your known speed */
									/* ind: if ACP_LINK_STATUS_UP: see ACP_LINK_SPEED_xxx else _UNKNOWN */
	LSA_UINT8		mode;			/* req: set to your known mode */
									/* ind: if ACP_LINK_STATUS_UP: see ACP_LINK_MODE_xxx else _UNKNOWN */
};

typedef struct acp_link_status_indication_tag					ACP_LINK_STATUS_INDICATION_TYPE;

typedef ACP_LINK_STATUS_INDICATION_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_LINK_STATUS_INDICATION_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_OVERLOAD_IND_RES_PROVIDE
// INDICATION: ACP_OPC_OVERLOAD_INDICATION
//----------------------------------------------------------------------------*/

struct acp_overload_tag
{
	LSA_UINT32		rx_counter;
	LSA_UINT32		tx_counter;
};

typedef struct acp_overload_tag					ACP_OVERLOAD_TYPE;

typedef ACP_OVERLOAD_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_OVERLOAD_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_MULTICAST
//----------------------------------------------------------------------------*/

#define ACP_MULTICAST_ENABLE		0 /* see EDD_MULTICAST_ENABLE */
#define ACP_MULTICAST_DISABLE		1 /* see EDD_MULTICAST_DISABLE */

struct acp_multicast_tag
{
	LSA_UINT8			mode;
	ACP_MAC_ADDR_TYPE	mac_addr;
};

typedef struct acp_multicast_tag				ACP_MULTICAST_TYPE;

typedef ACP_MULTICAST_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_MULTICAST_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: ACP_OPC_NOP
//----------------------------------------------------------------------------*/

struct acp_nop_tag
{
	LSA_UINT32			user_1;
	LSA_UINT32			user_2;
	LSA_UINT32			user_3;
	LSA_UINT32			user_4;
};

typedef struct acp_nop_tag					ACP_NOP_TYPE;

typedef ACP_NOP_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_NOP_PTR_TYPE;


/*------------------------------------------------------------------------------
// UNION of all types
//----------------------------------------------------------------------------*/

union acp_args_tag
{
	union {
		ACP_OPEN_CHANNEL_TYPE  open;
		ACP_CHANNEL_INFO_TYPE  info;
		ACP_LINK_STATUS_INDICATION_TYPE  link;
		ACP_GET_APDU_STATUS_TYPE  apdu_status;
		ACP_MULTICAST_TYPE  multicast;
		ACP_OVERLOAD_TYPE  overload;
		ACP_NOP_TYPE  nop;
	} channel;

	union {
		ACP_CONSUMER_ADD_TYPE  add;
		ACP_CONSUMER_REMOVE_TYPE  remove;
		ACP_CONSUMER_CONTROL_TYPE  control;
		ACP_CONSUMER_XCHANGE_BUF_TYPE  xbuf;
	} cons;

	union {
		ACP_PROVIDER_ADD_TYPE  add;
		ACP_PROVIDER_REMOVE_TYPE  remove;
		ACP_PROVIDER_CONTROL_TYPE  control;
		ACP_PROVIDER_CHANGE_PHASE_TYPE  change_phase;
		ACP_PROVIDER_XCHANGE_BUF_TYPE  xbuf;
	} prov;

	union {
		ACP_GLOBAL_PROVIDER_CONTROL_TYPE  control;
	} global_prov;

	union {
		ACP_STATE_INDICATION_TYPE  ind;
	} state;

	union {
		ACP_ALARM_OPEN_TYPE  open;
		ACP_ALARM_CLOSE_TYPE  close;
		ACP_ALARM_DATA_TYPE  data;
	} alarm;
};

typedef union acp_args_tag					ACP_ARGS_TYPE;

typedef ACP_ARGS_TYPE ACP_UPPER_MEM_ATTR *	ACP_UPPER_ARGS_PTR_TYPE;


/*------------------------------------------------------------------------------
// ACP_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

struct acp_rqb_tag
{
	ACP_RQB_HEADER						/* see acp_cfg.h */

		/* ACP needed data */
	ACP_UPPER_ARGS_PTR_TYPE	 args;		/* var: arguments corresponding to the opcode */

	ACP_RQB_TRAILER						/* see acp_cfg.h */
};

typedef struct acp_rqb_tag			ACP_RQB_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define ACP_RQB_SET_ARGS(rb,v)			{ (rb)->args = (v); }
#define ACP_RQB_GET_ARGS(rb)			((rb)->args)


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// access to consumer-provider-buffer
//	AP01403259: removed because of new EDDS buffer interface
//----------------------------------------------------------------------------*/


/*====  in functions  =====*/

/*=============================================================================
 * function name:  acp_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (ACP_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       callback-function
 *
 * RQB-return values via callback-function:
 *     LSA_UINT16         response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       possibly:
 *                                       *_RSP_ERR_PARAM
 *                                       *_RSP_ERR_LOWER_LAYER
 *                                       or others
 *     RQB-args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of prefix
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  ACP_UPPER_IN_FCT_ATTR  acp_open_channel(
    ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  acp_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of acp
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  ACP_UPPER_IN_FCT_ATTR  acp_close_channel(
    ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  acp_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of acp
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  possibly:
 *                                  *_RSP_OK_EOM
 *                                  *_RSP_ERR_RESOURCE
 *                                  *_RSP_ERR_PARAM
 *                                  *_RSP_ERR_LOC_ABORT
 *                                  *_RSP_ERR_REM_ABORT
 *                                  *_RSP_ERR_LOC_NET
 *                                  *_RSP_ERR_REM_NET
 *                                  *_RSP_ERR_PROTOCOL
 *                                  *_RSP_ERR_TIMEOUT
 *                                  *_RSP_ERR_DEV
 *                                  *_RSP_ERR_ROUTE
 *                                  *_RSP_ERR_OWN_ADDR
 *                                  *_RSP_ERR_LOWER_LAYER
 *                                  *_RSP_ERR_WITHDRAW
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  ACP_UPPER_IN_FCT_ATTR  acp_request(
    ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  acp_upper_rqb
 *
 * function:       asynchronous allocation of an upper-RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of RQB
 *                 ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* LSA-function, not used */
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_upper_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_UPPER_RQB_PTR_TYPE     upper_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  acp_upper_mem
 *
 * function:       asynchronous allocation of an upper-memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of acp
 *                 LSA_UINT16                 length:         length of memory
 *                 ACP_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if 0 /* LSA-function, not used */
LSA_VOID  ACP_SYSTEM_IN_FCT_ATTR  acp_upper_mem(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    ACP_UPPER_MEM_PTR_TYPE     upper_mem_ptr
);
#endif

/*====  out functions  =====*/

/*=============================================================================
 * function name:  ACP_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper-RQB
 *
 * parameters:     ACP_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to upper-RQB-pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 acp_upper_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of acp
 *                 LSA_UINT32                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_UPPER_RQB
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_UPPER_RQB(
    ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_UPPER_RQB
 *
 * function:       free an upper-RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                                                      wrong system-pointer
 *                 ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_UPPER_RQB
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_UPPER_RQB(
    LSA_UINT16                 ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * parameters:     ACP_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 acp_upper_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of acp
 *                 LSA_UINT32                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_ALLOC_UPPER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_ALLOC_UPPER_MEM(
    ACP_UPPER_MEM_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT32                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif

/*=============================================================================
 * function name:  ACP_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-memory or
 *                                                      wrong system-pointer
 *                 ACP_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_FREE_UPPER_MEM
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR  ACP_FREE_UPPER_MEM(
    LSA_UINT16                 ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_UPPER_MEM_PTR_TYPE                           upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                 sys_ptr
);
#endif


/*=============================================================================
 * function name:  ACP_REQUEST_LOCAL
 *
 * function:       send a request to "self"
 *
 * parameters:     ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef ACP_REQUEST_LOCAL
LSA_VOID  ACP_SYSTEM_OUT_FCT_ATTR
ACP_REQUEST_LOCAL (
    ACP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_USR_H */
