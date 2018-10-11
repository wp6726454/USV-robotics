#ifndef CM_ARCB_H
#define CM_ARCB_H

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
/*  F i l e               &F: cm_arcb.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  ARCB, the application relation control block                             */
/*                                                                           */
/*****************************************************************************/

/*
 * included by cm_usr.h
 */

#ifndef CM_CFG_EVERYTHING_VISIBLE
# error "something wrong with the includes"
#endif

/*------------------------------------------------------------------------------
// ARCB .. the Application Relation Control Block
//----------------------------------------------------------------------------*/

/*
//	SPH v7.20::Address mode flags				[see AP00612432]
//
//	Bit 0: IP issue
//			0: The IO controller issues IP Suite
//			1: The IO controller uses the found IP Suite
//  Bit 1: Search protocol
//			0: The IO controller searches with DCP
//			1: Reserved (DNS?)
//	Bit 2: don't care
//	Bit 3: Reserved (CM-upper interface only)
//
//	Bit 4: set IP Suite remanent
//			0: set IP Suite temporary
//			1: set IP Suite remanent
*/

enum cm_nare_properties_enum {
	CM_NARE_MODE_MASK				= 0x000B,

	CM_NARE_MODE_CONFIG_IP_FIXED	= 0x0000,
	CM_NARE_MODE_RESOLVE_DCP		= 0x0001,

	CM_NARE_MODE_CONFIG_IP_DNS		= 0x0002,
	CM_NARE_MODE_RESOLVE_DNS		= 0x0003,

	CM_NARE_MODE__DONT_CARE			= 0x0004,

	CM_NARE_MODE_USE_IP				= 0x0008,	/* only used by CM_OPC_CL_RECORD_READ_IMPLICIT */

	CM_NARE_SETIP_MASK				= 0x0010,
	CM_NARE_SETIP_TEMPORARY			= 0x0000,
	CM_NARE_SETIP_REMANENT			= 0x0010
};


typedef struct cm_arcb_nare_alias_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_nare_alias_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16		name_length;		/* req: length of name */
	CM_UPPER_MEM_U8_PTR_TYPE
					name;				/* req: not null-terminated alias-name of IO-Device */
} CM_ARCB_NARE_ALIAS_TYPE;

typedef CM_ARCB_NARE_ALIAS_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE;


typedef struct cm_arcb_nare_tag {
	LSA_UINT16		properties;			/* req: cm_nare_properties_enum */
	LSA_UINT16		device_name_length;	/* req: length of device_name */
	CM_UPPER_MEM_U8_PTR_TYPE
					device_name;		/* req: not null-terminated name of IO-Device */
	LSA_UINT32		device_ip_addr;		/* req: ip address if CM_NARE_MODE_CONFIG_IP_FIXED */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_nare_alias_tag)
					device_alias_list;	/* req: list of CM_CL_NARE_ALIAS_TYPE) */
} CM_ARCB_NARE_TYPE;

typedef CM_ARCB_NARE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_NARE_PTR_TYPE;


/*----------------------------------------------------------------------------*/

#define CM_PRM_PROP_RESERVED_MASK		0xFFFFFFFF  /* reserved (set to zero) */

typedef struct cm_arcb_prm_tag {
	LSA_UINT32		prm_properties;		/* req: see CM_PRM_PROP_xxx */
	LSA_UINT16		prm_server_name_length;
										/* req: length of prm_server_name */
	CM_UPPER_MEM_U8_PTR_TYPE
					prm_server_name;	/* req: not null-terminated name of prm-server */
	LSA_UINT16		prm_vendor_id;		/* req: identification of the PRM-server (object_uuid) */
	LSA_UINT16		prm_device_id;		/* req: -"- */
	LSA_UINT16		prm_instance;		/* req: -"- */
} CM_ARCB_PRM_TYPE;

typedef CM_ARCB_PRM_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_PRM_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_pdu_version_tag {/* format: ((major << 8) | minor) */
	/* connect */
	LSA_UINT16		ar;					/* req: version of ARBlockReq */
	LSA_UINT16		io_cr;				/* req: version of IOCRBlockReq */
	LSA_UINT16		expsubmod;			/* req: version of ExpectedSubmoduleBlockReq */
	LSA_UINT16		alarm_cr;			/* req: version of AlarmCRBlockReq */
	LSA_UINT16		prm;				/* req: version of PrmServerBlock */
	LSA_UINT16		mcr;				/* req: version of MCRBlockReq */
	LSA_UINT16		arrpc;				/* req: version of ARRPCBlockReq */
	/***/
	LSA_UINT16		control;			/* req: version of IODControlReq */
	LSA_UINT16		read;				/* req: version of IODReadReq */
	LSA_UINT16		write;				/* req: version of IODWriteReq/IODWriteHeader */
} CM_ARCB_PDU_VERSION_TYPE;

typedef CM_ARCB_PDU_VERSION_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_PDU_VERSION_PTR_TYPE;


/*----------------------------------------------------------------------------*/

enum cm_alcr_type_enum {
	CM_ALCR_TYPE_RESERVED_0	= 0x0000,
	CM_ALCR_TYPE_ALARM		= 0x0001
};

enum cm_alcr_properties_enum {
	CM_ALCR_PROP_PRIO_MASK			= 0x00000001, /* Bit 0: AlarmCRProperties.Priority */
	CM_ALCR_PROP_PRIO_USER			= 0x00000000, /* priority given by the user */
	CM_ALCR_PROP_PRIO_ONLY_LOW		= 0x00000001, /* use only low priority */

	CM_ALCR_PROP_TRANSPORT_MASK		= 0x00000002, /* Bit 1: AlarmCRProperties.Transport */
	CM_ALCR_PROP_TRANSPORT_RTA		= 0x00000000, /* Alarm CR uses Data-RTA-PDU */
	CM_ALCR_PROP_TRANSPORT_UDP		= 0x00000002, /* Alarm CR uses UDP-RTA-PDU */

	CM_ALCR_PROP_UNCHECKED_2_23		= 0x00FFFFFC   /* Bit 2 - 23: reserved_1, set to zero but do not check */

#define \
	CM_ALCR_PROP_RESERVED_24_31		  0xFF000000   /* Bit 24 - 31: reserved_2, set to zero and test for zero */
};


typedef struct cm_arcb_alarm_cr_tag {
	LSA_UINT32		alcr_properties;		/* see cm_alcr_properties_enum */
	LSA_UINT16		timeout100ms;			/* >= 1 */
	LSA_UINT16		retries;				/* 3..15 */

	LSA_UINT16		alcr_tag_header_high;	/* high priority vlan_tag */
	LSA_UINT16		alcr_tag_header_low;	/* low priority vlan_tag */
} CM_ARCB_ALARM_CR_TYPE;

typedef CM_ARCB_ALARM_CR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_ALARM_CR_PTR_TYPE;


/*----------------------------------------------------------------------------*/

enum cm_mcr_properties_enum { /* a.k.a AddressResolutionProperties */
	CM_MCR_PROP_PROTOCOL_MASK			= 0x00000007,
	CM_MCR_PROP_PROTOCOL_DNS			= 0x00000001,
	CM_MCR_PROP_PROTOCOL_DCP			= 0x00000002,

	CM_MCR_PROP_UNCHECKED_3_7			= 0x000000F8, /* Bit 3 -  7: reserved_1, set to zero but do not check */

	CM_MCR_PROP_RESERVED_8_15			= 0x0000FF00  /* Bit 8 - 15: reserved_2, set to zero and test for zero */

#define \
	CM_MCR_PROP_FACTOR_1SEC_MASK          0xFFFF0000  /* 0..reserved, 0x01-0x64 mandatory, >= 0x65 optional */
};

#define CM_MCR_PROP_GET_FACTOR_1SEC(mcr_prop)  ((LSA_UINT16)((mcr_prop) >> 16))

#define CM_MCR_PROPERTIES_BUILD(protocol, factor) (\
	((protocol) & CM_MCR_PROP_PROTOCOL_MASK) \
	| (((LSA_UINT32)(LSA_UINT16)(factor)) << 16) \
	)


typedef struct cm_arcb_mcr_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_mcr_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16		iocr_ref;			/* for reference to iocr */
	LSA_UINT32		mcr_properties;		/* AddressResolutionProperties, see cm_mcr_properties_enum */
	LSA_UINT16		mci_timeout_100ms;	/* 0 <= MCITimeoutFactor <= 100; base: 100ms   */
	LSA_UINT16		provider_name_length;
										/* req: length of ProviderStationName */
	CM_UPPER_MEM_U8_PTR_TYPE
					provider_name;		/* req: ProviderStationName (not null-terminated) */
} CM_ARCB_MCR_TYPE;

typedef CM_ARCB_MCR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_MCR_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_rpc_tag {
	LSA_UINT16		vendor_id;			/* req: part of object UUID identification of the remote */
	LSA_UINT16		device_id;			/* req: -"- */
	LSA_UINT16		instance;			/* req: -"- */

	LSA_UINT16		appl_timeout1sec;	/* only for CM_OPC_CL_RECORD_READ_IMPLICIT, otherwise don't care */
										/* req: remote application must respond within this timeout */
										/*		if 0: use create::rpc_remote_application_timeout1sec */

/* AP00784533 */
	LSA_UINT32		max_supported_record_length;
										/* gives the largest CM_OPC_CL_RECORD_WRITE::data_length */
										/* 0 means the default value "4152" (CM_RECORD_OFFSET + 4068) */
	LSA_BOOL		write_multiple_is_supported;
										/* LSA_TRUE: device supports write-multiple */

} CM_ARCB_RPC_TYPE;

typedef CM_ARCB_RPC_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_RPC_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_ir_info_sub_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ir_info_sub_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16		iocr_ref;
	LSA_UINT16		subframe_offset;
	LSA_UINT32		subframe_data;
} CM_ARCB_IR_INFO_SUB_TYPE;

typedef CM_ARCB_IR_INFO_SUB_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE;


typedef struct cm_arcb_ir_info_tag {
	CLRPC_UUID_TYPE	ir_data_uuid;		/* IRDataUUID */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ir_info_sub_tag)
					ir_info_sub_list;	/* list of: CM_ARCB_IR_INFO_SUB_TYPE */
} CM_ARCB_IR_INFO_TYPE;

typedef CM_ARCB_IR_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IR_INFO_PTR_TYPE;


/*----------------------------------------------------------------------------*/

#define CM_SR_PROP_INP_VALID_ON_BACKUP_AR  0x00000001
#define CM_SR_PROP_RESERVED_1_MASK         0x00000002
#define CM_SR_PROP_MODE                    0x00000004
#define CM_SR_PROP_RESERVED_2_MASK         0x0000FFF8
#define CM_SR_PROP_RESERVED_3_MASK         0xFFFF0000

typedef struct cm_arcb_sr_info_tag {

	LSA_UINT16		rdh_factor;
	LSA_UINT32		sr_properties;	/* see CM_SR_PROP_xxx */

} CM_ARCB_SR_INFO_TYPE;

typedef CM_ARCB_SR_INFO_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_ARCB_SR_INFO_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_arvendor_tag {
	LSA_UINT16					arvendor_data_len;	/* 0 or length of arvendor_data */
	CM_UPPER_MEM_U8_PTR_TYPE	arvendor_data;		/* LSA_NULL or ARVendorBlockReq 1..n  */
} CM_ARCB_ARVENDOR_TYPE;

typedef CM_ARCB_ARVENDOR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_ARVENDOR_PTR_TYPE;


/*----------------------------------------------------------------------------*/
/*	IODConnectReq ::=
//    ARBlockReq,
//      { [IOCRBlockReq*],  [AlarmCRBlockReq], [ExpectedSubmoduleBlockReq*]
//      , [PrmServerBlock], [MCRBlockReq*],    [ARRPCBlockReq]
//      , [IRInfoBlock],    [SRInfoBlock],     [ARVendorBlockReq*]
//      , [ARFSUBlock]
//      }
*/

enum cm_ar_type_enum {
	CM_AR_TYPE_SINGLE        = 0x0001, /* IOCARSingle */
	CM_AR_TYPE_SUPERVISOR    = 0x0006, /* IOSAR, The supervisor AR is a special form of the IOCARSingle */
	CM_AR_TYPE_SINGLE_RTC3   = 0x0010, /* IOCARSingle using RT_CLASS_3 */
	CM_AR_TYPE_SINGLE_SYSRED = 0x0020, /* IOCARSR, The SR AR is a special form of the IOCARSingle indicating system redundancy or configure in run usage */
	CM_AR_TYPE_RESERVED /* all other types are reserved for future use */
};

enum cm_ar_properties_enum {
	CM_AR_PROP_STATE_MASK						= 0x00000007,  /* Bit 0 - 2: ARProperties.State */
	CM_AR_PROP_STATE_ACTIVE						= 0x00000001,

	CM_AR_PROP_SUPERVISOR_TAKEOVER_ALLOWED		= 0x00000008,  /* Bit 3: ARProperties.SupervisorTakeoverAllowed */

	CM_AR_PROP_PRM_SERVER_CMINITIATOR			= 0x00000010,  /* Bit 4: ARProperties.ParametrizationServer (1 = CMInitiator, 0 = external) */

	CM_AR_PROP_UNCHECKED_5_7					= 0x000000E0,  /* Bit 5 - 7: reserved_1, set to zero but do not check */

	CM_AR_PROP_DEVICE_ACCESS					= 0x00000100,  /* Bit 8: ARProperties.DeviceAccess (1 = yes, 0 = no) */

	CM_AR_PROP_COMPANION_AR_MASK				= 0x00000600,  /* Bit 9-10: ARProperties.CompanionAR */
	CM_AR_PROP_COMPANION_AR_SINGLE				= 0x00000000,  /* single AR */

	CM_AR_PROP_ACKNOWLEDGE_COMPANION_AR			= 0x00000800,  /* Bit 11: Companion AR with acknowledge (1 = with, 0 = without) */

	CM_AR_PROP_UNCHECKED_12_23					= 0x00FFF000,  /* Bit 12 - 23: reserved_2, set to zero but do not check */

	CM_AR_PROP_RESERVED_24_29					= 0x3F000000,  /* Bit 24 - 29: reserved_3, set to zero and test for zero */

	CM_AR_PROP_STARTUP_MODE_ADVANCED			= 0x40000000   /* Bit 30: ARProperties.StartupMode (1 = advanced, 0 = legacy) */

#define \
	CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED		  0x80000000   /* Bit 31: The AlarmType(=Pull module) shall signal pulling of module. */
};


typedef struct cm_arcb_add_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_add_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

		/* ARBlockReq */
	LSA_UINT16		ar_nr;				/* local identification of this AR; 1..255 */
	LSA_UINT16		ar_type;			/* see cm_ar_type_enum */
	CLRPC_UUID_TYPE	ar_uuid;			/* reg: AR UUID reference */
										/* cnf: modified AR UUID reference if CM_AR_TYPE_SINGLE_SYSRED */
	LSA_UINT32		ar_properties;		/* see cm_ar_properties_enum */

		/* IOCRBlockReq */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_tag)
					iocr_list;			/* req: list of CM_ARCB_IOCR_TYPE */
										/* cnf: filled frame_id */

		/* IOCR local sendclock-adaption */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_local_tag)
					iocr_local_list;	/* optional list of CM_ARCB_IOCR_LOCAL_TYPE */

		/* AlarmCRBlockReq */
	CM_UPPER_ARCB_ALARM_CR_PTR_TYPE
					alarm_cr;			/* max_alarm_data_length see Create-Client */

		/* ExpectedSubmoduleBlockReq: expected api/modules/submodules/prm_records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ap_tag)
					ap_list;			/* list of CM_ARCB_AP_TYPE */

		/* PrmServerBlock */
	CM_UPPER_ARCB_PRM_PTR_TYPE
					prm;				/* LSA_NULL or an PrmServerBlock */

		/* MCRBlockReq */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_mcr_tag)
					mcr_list;			/* list of CM_ARCB_MCR_TYPE */

		/* IRInfoBlock */
	CM_UPPER_ARCB_IR_INFO_PTR_TYPE
					ir_info;			/* LSA_NULL or an IRInfoBlock */

		/* SRInfoBlock */
	CM_UPPER_ARCB_SR_INFO_PTR_TYPE
					sr_info;			/* LSA_NULL or an SRInfoBlock, for CM_AR_TYPE_SINGLE_SYSRED */

		/* ARVendorBlockReq */
	CM_ARCB_ARVENDOR_TYPE
					arvendor_blob;		/* ARVendorBlockReq 1..n */

		/* ar-related prm_records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
					ar_prm_record_list;	/* list of CM_ARCB_RECORD_TYPE */
										/* this records will only prm-ed in connect-phase, index should ar-related */

} CM_ARCB_ADD_TYPE;

typedef CM_ARCB_ADD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_ADD_PTR_TYPE;


/*------------------------------------------------------------------------------
// IOCRBlockReq (iodata / iocs / ioap / iocr)
//----------------------------------------------------------------------------*/

typedef struct cm_arcb_iodata_object_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iodata_object_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16		slot_nr;			/* related info */
	LSA_UINT16		subslot_nr;
	LSA_UINT16		frame_offset;
	LSA_UINT16		application_offset;
} CM_ARCB_IODATA_OBJECT_TYPE;

typedef CM_ARCB_IODATA_OBJECT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_iocs_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocs_tag)
					link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16		slot_nr;			/* related info */
	LSA_UINT16		subslot_nr;
	LSA_UINT16		frame_offset;
	LSA_UINT16		application_offset;
} CM_ARCB_IOCS_TYPE;

typedef CM_ARCB_IOCS_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IOCS_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_ioapi_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag)
						link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT32			api;				/* application process identifier */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iodata_object_tag)
						related_iodata_objects_list;
											/* list of: CM_ARCB_IODATA_OBJECT_TYPE */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocs_tag)
						related_iocs_list;	/* list of: CM_ARCB_IOCS_TYPE */
} CM_ARCB_IOAPI_TYPE;

typedef CM_ARCB_IOAPI_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IOAPI_PTR_TYPE;

/*----------------------------------------------------------------------------*/

enum cm_iocr_type_enum {
	CM_IOCR_TYPE_RESERVED_0			= 0x0000,
	CM_IOCR_TYPE_INPUT				= 0x0001,
	CM_IOCR_TYPE_OUTPUT				= 0x0002,
	CM_IOCR_TYPE_MULTICAST_PROVIDER	= 0x0003,
	CM_IOCR_TYPE_MULTICAST_CONSUMER	= 0x0004
};

enum cm_iocr_properties_enum {
	CM_IOCR_PROP_RT_CLASS_MASK			= 0x0000000F,  /* Bit 0 - 3: RTClass */
	CM_IOCR_PROP_RT_CLASS_1				= 0x00000001,  /*	RT_CLASS_1   / Shall be supported for legacy IO devices by IO controller and IO supervisor. It should not be generated by an IO device. */
	CM_IOCR_PROP_RT_CLASS_2				= 0x00000002,  /*	RT_CLASS_2   / Data-RTC-PDU */
	CM_IOCR_PROP_RT_CLASS_3				= 0x00000003,  /*	RT_CLASS_3   / Data-RTC-PDU */
	CM_IOCR_PROP_RT_CLASS_1_UDP			= 0x00000004,  /*	RT_CLASS_UDP / UDP-RTC-PDU */

	CM_IOCR_PROP_RESERVED_4_12			= 0x00001FF0,  /* Bit 4 - 12: reserved_1, set to zero and test for zero */

	CM_IOCR_PROP_UNCHECKED_13_23		= 0x00FFE000   /* Bit 13 - 23: reserved_2, set to zero but do not check */

#define \
	CM_IOCR_PROP_RESERVED_24_31			  0xFF000000   /* Bit 24 - 31: reserved_3, set to zero and test for zero */
};

/*---------------------------------------------------------------------------------------
 *
 * from part 6, C_SDU
 *
 * a) In case of Data-RTC-PDU the number of padding octets
 *    shall be in the range of 0..40 depending on the DataItem size.
 *
 *    In case of UDP-RTC-PDU the number of padding octets
 *    shall be in the range of 0..12 depending on the DataItem size.
 *
 * b) In case of RT class 3 transportation the number of padding octets
 *    is given by the engineering system.
 *
 * The maximum C_SDU size shall be not larger than 1440 octets.
 *
 * --------------------------------------------------------------------------------------
 *
 * CM V4.1i1:
 *  RT class 3 frames shall not be "short frames"
 *  CM_PNIO_C_SDU_MIN_LENGTH_IRT removed (value was 0)
 *
 *         DST + SRC + VLAN + LT + IP + UDP + FRAMEID + C_SDU + APDUSTATUS + FCS
 * RTC3:    6  +  6         +  2            +    2    +   40  +    2+1+1   +  4  = 64
 * RTC12:   6  +  6  +   4  +  2            +    2    +   40  +    2+1+1   +  4  = 68
 * RToUDP:  6  +  6  +   4  +  2 + 20 +  8  +    2    +   12  +    2+1+1   +  4  = 68
 *
 * An ethernet frame should have a minimum length of 64 bytes (legacy of 10MB ethernet).
 * If a switch removes the VLAN tag, the frame should still be at least 64 bytes long.
 */

#define CM_PNIO_C_SDU_MIN_LENGTH_UDP	12
#define CM_PNIO_C_SDU_MIN_LENGTH_RT		40
#define CM_PNIO_C_SDU_MAX_LENGTH		1440


typedef struct cm_arcb_iocr_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_tag)
						link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16			iocr_type;			/* IO CR Type, see cm_iocr_type_enum */
	LSA_UINT16			iocr_ref;			/* for reference */
	LSA_UINT32			iocr_properties;	/* see cm_iocr_properties_enum */

	LSA_UINT16			frame_id;			/* req: class-1/2: not used; class-3: use this frame_id */
											/* cnf: class-1/2: assigned frame_id, valid for consumer only */
	LSA_UINT16			data_length;		/* see CM_PNIO_C_SDU_MIN_LENGTH_RT, etc. */

	LSA_UINT16			send_clock;			/* The time base is 31,25 microseconds. The value range is from 1 to 128. It is mandatory to support the value 32. */
	LSA_UINT16			reduction_ratio;	/* reduction ratio; mandatory:1,2,4,8,16,32,128  mandatory-udp:256,512,1024,2048,8196,16384  optional:others < 16385 */
	LSA_UINT16			phase;				/* 1 <= phase <= reduction_ratio */
	/* LSA_UINT16		sequence ... removed with V5.3i1 (AP01104502); note: had no effect since AP00542516 and AP00619993 (stable sequence of provider frames) */
	LSA_UINT32			frame_send_offset;	/* time base:1 nanosec;  mandatory:0xFFFFFFFF(best effort);  optional:0..0x003D08FF */

	/* LSA_UINT16		watchdog_factor ... removed with V5.3i1 (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12); note: value had to be the same as data_hold_factor */
	LSA_UINT16			data_hold_factor;	/* 0x0001,0x0002 optional, 0x0003-0x00FF mandatory, 0x0100-0x1E00 optional, rest reserved */

	LSA_UINT16			iocr_tag_header;	/* vlan_tag ::= (prio,cfi(=0),vlan_id) */
	CM_MAC_TYPE			iocr_multicast_mac;	/* multicast MAC address */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ioapi_tag)
						related_ioapi_list;	/* list of: CM_ARCB_IOAPI_TYPE */

		/* information for buffer access (req) */

	LSA_UINT32			edd_properties;		/* for EDDI */
	LSA_UINT32			edd_data_offset;	/* for EDDx */
	LSA_UINT16			edd_cr_number;		/* for EDDP and EDDS (see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254) */
	LSA_UINT8			edd_forward_mode;	/* for EDDP */
	LSA_UINT32			edd_ct_outdatastart;/* for EDDP */

	LSA_UINT16			apdu_status_length;	/* see cm_arcb_calc_iocr_buffer_size() */
	LSA_UINT16			partial_length;		/* see cm_arcb_calc_iocr_buffer_size() */
	LSA_UINT16			partial_offset;		/* see cm_arcb_calc_iocr_buffer_size() */
	LSA_UINT16			subframe_data;		/* see cm_arcb_calc_iocr_buffer_size() */
	LSA_UINT16			subframe_offset;	/* see cm_arcb_calc_iocr_buffer_size() */

		/* information for buffer access (cnf) */

	LSA_UINT32			edd_apdustatus_offset;	/* cnf: the EDD::ApduStatusOffset, valid if consumer */
	LSA_UINT32			edd_cw_offset;			/* cnf: the EDD::CWOffset, valid if provider */
	LSA_UINT32			edd_datastatus_offset;	/* cnf: the EDD::DataStatusOffset, valid if provider */

} CM_ARCB_IOCR_TYPE;

typedef CM_ARCB_IOCR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IOCR_PTR_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_arcb_iocr_local_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_local_tag)
						link;				/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16			iocr_ref;			/* for reference to iocr */

	LSA_UINT16			send_clock;			/* The time base is 31,25 microseconds. The value range is from 1 to 128. It is mandatory to support the value 32. */
	LSA_UINT16			reduction_ratio;	/* reduction ratio; mandatory:1,2,4,8,16,32,128  mandatory-udp:256,512,1024,2048,8196,16384  optional:others < 16385 */
	LSA_UINT16			phase;				/* 1 <= phase <= reduction_ratio */
	/* LSA_UINT16		sequence ... removed with V5.3i1, see CM_ARCB_IOCR_TYPE */
	/* LSA_UINT16		watchdog_factor ... removed with V5.3i1, see CM_ARCB_IOCR_TYPE */
	LSA_UINT16			data_hold_factor;	/* 0..use data_hold_factor from refered iocr, else..use as defined */
	LSA_UINT32			frame_send_offset;	/* time base:1 nanosec;  mandatory:0xFFFFFFFF(best effort);  optional:0..0x003D08FF */
} CM_ARCB_IOCR_LOCAL_TYPE;

typedef CM_ARCB_IOCR_LOCAL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE;


/*------------------------------------------------------------------------------
// Record / Submodule / Module / AP
//----------------------------------------------------------------------------*/

typedef struct cm_arcb_record_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
								link;		/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT32					record_index;
	LSA_UINT16					transfer_sequence; /* valid for manufacture specific index only (index < 0x8000); 0..undefined seqence; >0..use this sequence */
	LSA_UINT16					record_data_len;
	CM_UPPER_MEM_U8_PTR_TYPE	record_data;
} CM_ARCB_RECORD_TYPE;

typedef CM_ARCB_RECORD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_RECORD_PTR_TYPE;


/*----------------------------------------------------------------------------*/

enum cm_sub_properties_enum {
	CM_SUB_PROP_TYPE_MASK			= 0x0003,  /* Bit 0 - 1: SubmoduleProperties.Type */
	CM_SUB_PROP_TYPE_NO_DATA		= 0x0000,
	CM_SUB_PROP_TYPE_INPUT			= 0x0001,
	CM_SUB_PROP_TYPE_OUTPUT			= 0x0002,
	CM_SUB_PROP_TYPE_INPUT_OUTPUT	= 0x0003,

	CM_SUB_PROP_SHARED_INP_MASK		= 0x0004,  /* Bit 2: SubmoduleProperties.SharedInput */
	CM_SUB_PROP_SHARED_INP_NO		= 0x0000,
	CM_SUB_PROP_SHARED_INP_YES		= 0x0004,

	CM_SUB_PROP_REDUCE_INP_LEN_MASK = 0x0008,  /* Bit 3: SubmoduleProperties.ReduceInputSubmoduleDataLength */
	CM_SUB_PROP_REDUCE_INP_LEN_NO   = 0x0000,  /* Use expected input SubmoduleDataLength for I-CR */
	CM_SUB_PROP_REDUCE_INP_LEN_YES  = 0x0008,  /* Reduce input SubmoduleDataLength to zero for I-CR */

	CM_SUB_PROP_REDUCE_OUT_LEN_MASK = 0x0010,  /* Bit 4: SubmoduleProperties.ReduceOutputSubmoduleDataLength */
	CM_SUB_PROP_REDUCE_OUT_LEN_NO   = 0x0000,  /* Use expected output SubmoduleDatalength for O-CR */
	CM_SUB_PROP_REDUCE_OUT_LEN_YES  = 0x0010,  /* Reduce output SubmoduleDatalength to zero for O-CR */

	CM_SUB_PROP_DISCARD_IOXS_MASK   = 0x0020,  /* Bit 5: SubmoduleProperties.DiscardIOXS */
	CM_SUB_PROP_DISCARD_IOXS_NO     = 0x0000,  /* Use IOXS-length */
	CM_SUB_PROP_DISCARD_IOXS_YES    = 0x0020,  /* Treat IOXS-Length as 0 */

	CM_SUB_PROP_UNCHECKED_6_7       = 0x00C0,  /* Bit 6 - 7: reserved_1, set to zero but do not check */

	CM_SUB_PROP_RESERVED_8_15       = 0xFF00   /* Bit 8 - 15: reserved_2, set to zero and test for zero */
};

enum cm_sub_data_description_enum {
	CM_SUB_DATA_DESCR_TYPE_MASK			= 0x0003,  /* Bit 0 - 1: DataDescription.Type */
	CM_SUB_DATA_DESCR_TYPE_RESERVED_0	= 0x0000,
	CM_SUB_DATA_DESCR_TYPE_INPUT		= 0x0001,
	CM_SUB_DATA_DESCR_TYPE_OUTPUT		= 0x0002,
	CM_SUB_DATA_DESCR_TYPE_RESERVED_3	= 0x0003

#define \
	CM_SUB_DATA_DESCR_RESERVED_MASK		  0xFFFC  /* Bit 2-15: DataDescription.reserved (set to zero) */
};

#define CM_SUB_DATA_LENGTH_MAX		(CM_PNIO_C_SDU_MAX_LENGTH - 1) /* The range shall be between 0 and 1439 */


typedef struct cm_arcb_submodule_descr_tag {
	LSA_UINT16		data_description;	/* see cm_sub_data_description_enum */
	LSA_UINT16		data_length;		/* SubmoduleDataLength, see CM_SUB_DATA_LENGTH_MAX */

	LSA_UINT8		iops_length;		/* length of IOPS 1..n */
	LSA_UINT8		iocs_length;		/* length of IOCS 1..m */

} CM_ARCB_SUBMODULE_DESCR_TYPE;


typedef struct cm_arcb_submodule_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_submodule_tag)
						link;			/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16			subslot_nr;		/* ascending order */
	LSA_UINT16			sub_properties;	/* see cm_sub_properties_enum */
	LSA_UINT32			sub_ident;		/* submodule ident number */

	/* submodule data description */
	CM_ARCB_SUBMODULE_DESCR_TYPE
						descr[2];		/* index: 0..input 1..output */

	union {
		struct {
			LSA_UINT32  nothing_so_far; /* do not evaluate, AP00565264 */
		} pd;
		struct {
			CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
						prm_record_list; /* list of prm-records for this submodule, must be in needed order */
		} cl; /* cm-client only */
	} u;
} CM_ARCB_SUBMODULE_TYPE;

typedef CM_ARCB_SUBMODULE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_SUBMODULE_PTR_TYPE;


/*---------------------------------------------------------------------------*/

enum cm_mod_properties_enum {
	CM_MOD_PROP_UNCHECKED_0_7 = 0x00FF,  /* Bit 0 -  7: reserved_1, set to zero but do not check */

	CM_MOD_PROP_RESERVED_8_15 = 0xFF00   /* Bit 8 - 15: reserved_2, set to zero and test for zero */
};


typedef struct cm_arcb_module_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_module_tag)
						link;			/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT16			slot_nr;		/* slot number */
	LSA_UINT16			mod_properties;	/* module properties */
	LSA_UINT32			mod_ident;		/* module ident number */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_submodule_tag)
						submodule_list;	/* list of submodules of type CM_ARCB_SUBMODULE_TYPE */
} CM_ARCB_MODULE_TYPE;

typedef CM_ARCB_MODULE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_MODULE_PTR_TYPE;


/*---------------------------------------------------------------------------*/

typedef struct cm_arcb_ap_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_ap_tag)
						link;			/* link to next entry, see list-macros in cm_list.h */

	LSA_UINT32  api;  /* application process identifier */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_module_tag)
						module_list;	/* list of modules of type CM_ARCB_MODULE_TYPE */
} CM_ARCB_AP_TYPE;

typedef CM_ARCB_AP_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_AP_PTR_TYPE;


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ARCB_H */
