#ifndef CM_PDU_H
#define CM_PDU_H

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
/*  F i l e               &F: cm_pdu.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile: PDU definitions                                     */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by cm_int.h
 */

/*----------------------------------------------------------------------------*/

#include "pnio_pck1_on.h" /* packing macros */

/*------------------------------------------------------------------------------
// Offset Macro for CM_GETxxx and CM_PUTxxx, see cm_lib.h
//----------------------------------------------------------------------------*/

#define CM_OFFS(type, field)  ((LSA_INT)( &((type*)0)->field ))

/*------------------------------------------------------------------------------
// PDU UUID
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_pdu_uuid_tag {
	LSA_UINT8  uuid[16];
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_pdu_uuid_tag CM_PDU_UUID_TYPE;

/*------------------------------------------------------------------------------
// PDU MAC
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_pdu_mac_tag {
	LSA_UINT8  mac[6];
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_pdu_mac_tag CM_PDU_MAC_TYPE;

/*------------------------------------------------------------------------------
// PDU Constants
//----------------------------------------------------------------------------*/

#define CM_RECORD_INDEX_WRITE_MULTIPLE  0xE040 /* WriteMultiple */
#define CM_RECORD_INDEX_RPC_TRIGGER     0xFBFF /* Trigger index for the RPC connection monitoring */


enum cm_block_type_enum {
	CM_BLOCK_TYPE_NONE				= 0x0000,

	CM_BLOCK_TYPE_ALARM_HIGH		= 0x0001, /* AlarmNotification High, USAGE: AlarmNotification-PDU */
	CM_BLOCK_TYPE_ALARM_LOW			= 0x0002, /* AlarmNotification Low, USAGE: AlarmNotification-PDU */
	CM_BLOCK_TYPE_ALARM_ACK_HIGH	= 0x8001, /* Alarm Ack High, USAGE: AlarmAck-PDU */
	CM_BLOCK_TYPE_ALARM_ACK_LOW		= 0x8002, /* Alarm Ack Low, USAGE: AlarmAck-PDU */

	CM_BLOCK_TYPE_WRITE_REQ			= 0x0008, /* WriteRecordReq, USAGE: RecordDataWrite */
	CM_BLOCK_TYPE_READ_REQ			= 0x0009, /* ReadRecordReq, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_WRITE_RSP			= 0x8008, /* WriteRecordRes, USAGE: RecordDataWrite */
	CM_BLOCK_TYPE_READ_RSP			= 0x8009, /* ReadRecordRes, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_DIAGNOSIS			= 0x0010, /* DiagnosisBlock, USAGE: RecordDataRead, AlarmNotification-PDU */

	CM_BLOCK_TYPE_EXP_IDENT			= 0x0012, /* ExpectedIdentificationDataBlock, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_REAL_IDENT		= 0x0013, /* RealIdentificationData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_SUBST_VALUE		= 0x0014, /* SubstituteValue, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_IODATA_INP		= 0x0015, /* RecordInputDataObjectElement, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_IODATA_OUT		= 0x0016, /* RecordOutputDataObjectElement, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_AR_DATA			= 0x0018, /* ARData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_LOG_DATA			= 0x0019, /* LogBookData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_API_DATA			= 0x001A, /* APIData, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_I_M_0				= 0x0020, /* I&M0, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_I_M_1				= 0x0021, /* I&M1, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_2				= 0x0022, /* I&M2, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_3				= 0x0023, /* I&M3, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_4				= 0x0024, /* I&M4, USAGE: RecordDataRead, RecordDataWrite */
	CM_BLOCK_TYPE_I_M_5				= 0x0025, /* I&M5, USAGE: RecordDataRead */

	CM_BLOCK_TYPE_I_M0FilterDataSub	= 0x0030, /* I&M0FilterDataSubmodule, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M0FilterDataMod	= 0x0031, /* I&M0FilterDataModule, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M0FilterDataDev	= 0x0032, /* I&M0FilterDataDevice, USAGE: RecordDataRead*/
	CM_BLOCK_TYPE_I_M_5_Data		= 0x0034, /* I&M5Data, USAGE: RecordDataRead*/

	CM_BLOCK_TYPE_AR_REQ			= 0x0101, /* ARBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_IOCR_REQ			= 0x0102, /* IOCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ALCR_REQ			= 0x0103, /* AlarmCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_EXPSUBMOD_REQ		= 0x0104, /* ExpectedSubmoduleBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_PRMSRV_REQ		= 0x0105, /* PrmServerBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_MCR_REQ			= 0x0106, /* MCRBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARRPCBLOCK_REQ	= 0x0107, /* ARRPCBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARVENDORBLOCK_REQ	= 0x0108, /* ArVendorBlockReq, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_IRINFOBLOCK		= 0x0109, /* IRInfoBlock, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_SRINFOBLOCK		= 0x010A, /* SRInfoBlock, USAGE: IODConnectReq */
	CM_BLOCK_TYPE_ARFSUBLOCK		= 0x010B, /* ARFSUBlock, USAGE: IODConnectReq */

	CM_BLOCK_TYPE_AR_RSP			= 0x8101, /* ARBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_IOCR_RSP			= 0x8102, /* IOCRBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ALCR_RSP			= 0x8103, /* AlarmCRBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_MOD_DIFF			= 0x8104, /* ModuleDiffBlock, USAGE: IODConnectRes, RecordDataRead,IOCControlReq,IOSControlReq */
	CM_BLOCK_TYPE_PRMSRV_RSP		= 0x8105, /* PrmServerBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARSERVERBLOCK		= 0x8106, /* ARServerBlock, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARRPCBLOCK_RSP	= 0x8107, /* ARRPCBlockRes, USAGE: IODConnectRes */
	CM_BLOCK_TYPE_ARVENDORBLOCK_RSP	= 0x8108, /* ArVendorBlockRes, USAGE: IODConnectRes */

	CM_BLOCK_TYPE_IODBLOCK_CONN_REQ	= 0x0110, /* IODBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IODControlReq(Prm End.req) */
	CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ	= 0x0111, /* IODBlockReq, shall only be used in conjunction with a plug alarm event, USAGE: IODControlReq(Prm End.req) */

	CM_BLOCK_TYPE_IODBLOCK_CONN_RSP	= 0x8110, /* IODBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IODControlRes(Prm End.res) */
	CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP	= 0x8111, /* IODBlockRes, shall only be used in conjunction with a plug alarm event, USAGE: IODControlRes(Prm End.res) */

	CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ	= 0x0112, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Application Ready.req) */
	CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ	= 0x0113, /* IOXBlockReq, shall only be used in conjunction with a plug alarm event, USAGE: IOCControlReq, IOSControlReq(Application Ready.req) */

	CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP	= 0x8112, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Application Ready.res) */
	CM_BLOCK_TYPE_IOXBLOCK_PLUG_RSP	= 0x8113, /* IOXBlockRes, shall only be used in conjunction with a plug alarm event, USAGE: IOCControlRes IOSControlRes(Application Ready.res) */

	CM_BLOCK_TYPE_RELBLOCK_REQ		= 0x0114, /* ReleaseBlockReq, USAGE: IODReleaseReq */
	CM_BLOCK_TYPE_RELBLOCK_RSP		= 0x8114, /* ReleaseBlockRes, USAGE: IODReleaseRes */

#if 0/*reserved for future use*/
	CM_BLOCK_TYPE_IOXBLOCK_R4COMP_REQ = 0x0116, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Ready for Companion.req) */
	CM_BLOCK_TYPE_IOXBLOCK_R4COMP_RSP = 0x8116, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Ready for Companion.res) */
#endif

	CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ = 0x0117, /* IOXBlockReq, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlReq, IOSControlReq(Ready for RTClass3.req) */
	CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_RSP = 0x8117, /* IOXBlockRes, shall only be used in conjunction with connection establishment phase, USAGE: IOCControlRes IOSControlRes(Ready for RTClass3.res) */

	CM_BLOCK_TYPE_PRM_BEGIN_REQ       = 0x0118, /* IODBlockReq, USAGE: IODControlReq(Prm Begin.req) */
	CM_BLOCK_TYPE_PRM_BEGIN_RSP       = 0x8118, /* IODBlockRes, USAGE: IODControlRes(Prm Begin.rsp) */

	CM_BLOCK_TYPE_SUBMODULE_LIST_REQ  = 0x0119, /* SubmoduleListBlock, USAGE: IODControlReq(Prm Begin.req) */

	CM_BLOCK_TYPE_PD_PORT_DATA_CHECK        = 0x0200, /* PDPortDataCheck,  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_PDEVDATA               = 0x0201, /* PDevData,         USAGE: RecordDataRead */
	CM_BLOCK_TYPE_PD_PORT_DATA_ADJUST       = 0x0202, /* PDPortDataAdjust  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_SYNC_DATA              = 0x0203, /* PDSyncData,       USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_ISOCHRONOUS               = 0x0204, /* Isochr.ModeData,  USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_DATA                = 0x0205, /* PDIRData,         USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA         = 0x0206, /* PDIRGlobalData,   USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_FRAME_DATA          = 0x0207, /* PDIRFrameData,    USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA      = 0x0208, /* PDIRBeginEndData, USAGE: RecordDataWrite/Read */

	CM_BLOCK_TYPE_PD_ADJ_DOMAIN_BOUNDARY    = 0x0209, /* adjusting DomainBoundary, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_CHK_PEERS              = 0x020A, /* checking Peers, PDPortDataCheck */
	CM_BLOCK_TYPE_PD_CHK_LINE_DELAY         = 0x020B, /* checking LineDelayFactor, PDPortDataCheck */
	CM_BLOCK_TYPE_PD_CHK_MAUTYPE            = 0x020C, /* checking MAUType, PDPortDataCheck */

	CM_BLOCK_TYPE_PD_ADJ_MAUTYPE            = 0x020E, /* adjusting MAUType, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_PORT_DATA_REAL         = 0x020F, /* PDPortDataReal, USAGE: RecordDataRead */
	CM_BLOCK_TYPE_PD_ADJ_MULTICAST_BOUNDARY = 0x0210, /* adjusting MulticastBoundary, PDPortDataAdjust */

	CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ      = 0x0211, /* adjusting MRP_Role,      PDInterfaceMrpDataAdjust */
	CM_BLOCK_TYPE_PD_MRP_INTERFACE_CHECK	= 0x0213, /* PDInterfaceMrpDataCheck, USAGE: RecordDataWrite/Read */
	CM_BLOCK_TYPE_PD_MRP_PORT_ADJ			= 0x0214, /* PDPortMrpDataAdjust,     USAGE: RecordDataWrite/Read */

	CM_BLOCK_TYPE_PD_ADJ_LINK_STATE         = 0x021B, /* adjusting LinkState, PDPortDataAdjust */
	CM_BLOCK_TYPE_PD_CHK_LINK_STATE         = 0x021C, /* checking LinkState, PDPortDataCheck */

	CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH	= 0x0226, /* adjusting PreambleLength, PDPortDataAdjust */

	CM_BLOCK_TYPE_MULTIPLE_BLOCK_HEADER = 0x0400, /* Sub block for MultipleBlockHeader, RecordDataRead */

	CM_BLOCK_TYPE_READQUERY             = 0x0500, /* RecordDataReadQuery */

	CM_BLOCK_TYPE_FSHELLOBLOCK          = 0x0600, /* FSHelloBlock, PDInterfaceFSUDataAdjust */
	CM_BLOCK_TYPE_FSPARAMETERBLOCK      = 0x0601, /* FSParameterBlock, ARFSUDataAdjust */
	CM_BLOCK_TYPE_FSUDATAADJUST         = 0x0608, /* PDInterfaceFSUDataAdjust, RecordDataRead RecordDataWrite */
	CM_BLOCK_TYPE_ARFSUDATAADJUST       = 0x0609, /* ARFSUDataAdjust, RecordDataRead RecordDataWrite */

	CM_BLOCK_TYPE_MAINTENANCE           = 0x0F00, /* PROFInet IO - Maintenance Integration.ppt */

	CM_BLOCK_TYPE_PD_SYNC_PLL			= 0x7000, /* PDSyncPLL (Index 0x00011020) */
	CM_BLOCK_TYPE_SEND_CLOCK            = 0xF000, /* SendClock (Index 0x00010000) */

	CM_BLOCK_TYPE_RESERVED              = 0xFFFF
};

/*------------------------------------------------------------------------------
//----------------------------------------------------------------------------*/

/*
 * cm_ar_type_enum, etc. see cm_arcb.h
 */

/*
 * cm_iocr_type_enum, etc. see cm_arcb.h
 */

/*
 * cm_alcr_type_enum, etc. see cm_arcb.h
 */

/*------------------------------------------------------------------------------
// BlockHeader ::=
//   BlockType, BlockLength, BlockVersionHigh, BlockVersionLow
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_BlockHeader_tag {
	LSA_UINT16  type;    /* see cm_block_type_enum */
	LSA_UINT16  length;  /* block-length without 'type' and 'length' */
	LSA_UINT16  version; /* high=major, low=minor */
	/* block body follows */
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_BlockHeader_tag CM_BLK_HEADER;

/*------------------------------------------------------------------------------
// ARBlockReq ::=
//   BlockHeader,
//   ARType, ARUUID, SessionKey,
//   CMInitiatorMacAdd, CMInitiatorObjectUUID,
//   ARProperties,
//   CMInitiatorActivityTimeout,
//   UDPCMInitiatorPort,
//   StationNameLength, CMInitiatorStationName
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ARBlockReq_tag {

	/* BlockHeader not here! */

	LSA_UINT16			ar_type;			/* see cm_ar_type_enum */
	CM_PDU_UUID_TYPE	ar_uuid;
	LSA_UINT16			session_key;

	CM_PDU_MAC_TYPE		cmi_mac;			/* CMInitiatorMacAdd */
	CM_PDU_UUID_TYPE	cmi_obj_uuid;		/* CMInitiatorObjectUUID */

	LSA_UINT32			ar_properties;		/* see cm_ar_properties_enum */

	LSA_UINT16			cmi_activity_timeout100ms;	/* CMInitiatorActivityTimeout */

	LSA_UINT16			cmi_udp_port;		/* InitiatorUDPRTPort */

	LSA_UINT16			cmi_name_length;	/* StationNameLength */

	/* LSA_UINT8 cmi_name[] follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ARBlockReq_tag CM_REQ_AR;

/*------------------------------------------------------------------------------
// ARBlockRes ::=
//   BlockHeader,
//   ARType, ARUUID,
//   SessionKey,
//   CMResponderMacAdd,
//   UDPCMResponderPort
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ARBlockRes_tag {

	/* BlockHeader not here! */

	LSA_UINT16			ar_type;
	CM_PDU_UUID_TYPE	ar_uuid;
	LSA_UINT16			session_key;

	CM_PDU_MAC_TYPE		cmr_mac;		/* CMResponderMacAdd */

	LSA_UINT16			cmr_udp_port;	/* ResponderUDPRTPort */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ARBlockRes_tag CM_RSP_AR;

/*------------------------------------------------------------------------------
// IOCRBlockReq ::=
//   BlockHeader,
//   IOCRType, IOCRReference, LT, IOCRProperties,
//   DataLength, FrameID,
//   SendClock, ReductionRatio, Phase, Sequence, FrameSendOffset,
//   WatchdogFactorLegacy, DataHoldFactor,
//   IOCRTagHeader, IOCRMulticastMACAdd,
//   NumberOfRelatedAPIs,
//   ( API,
//     NumberOfRelatedIODataObjects,
//     ( Slotnumber, SubslotNumber, IODataObjectFrameOffset
//     )*,
//     NumberOfRelatedIOCS,
//     ( Slotnumber, SubslotNumber, IOCSFrameOffset
//     )*
//   )*
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockReqPart1_tag {

	/* BlockHeader not here! */

	LSA_UINT16		iocr_type;			/* see cm_iocr_type_enum */
	LSA_UINT16		iocr_ref;
	LSA_UINT16		ethertype;
	LSA_UINT32		iocr_properties;	/* see cm_iocr_properties_enum */

	LSA_UINT16		data_length;		/* >= edd.min_frame_size && <= edd.max_frame_size */
	LSA_UINT16		frame_id;

	LSA_UINT16		send_clock;
	LSA_UINT16		reduction_ratio;
	LSA_UINT16		phase;
	LSA_UINT16		sequence;
	LSA_UINT32		frame_send_offset;	/* timebase 1ns */

	LSA_UINT16		watchdog_factor_legacy; /* legacy, must be the same as data_hold_factor (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12) */
	LSA_UINT16		data_hold_factor;

	LSA_UINT16		iocr_tag_header;
	CM_PDU_MAC_TYPE	iocr_multicast_mac;

	LSA_UINT16		number_of_related_apis;

	/* part 2 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockReqPart2_tag {

	LSA_UINT32		api;
	LSA_UINT16		number_of_releated_iodata_objects;

	/* part 3 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockReqPart3_tag { /* related io data object */

	LSA_UINT16		slot_nr;
	LSA_UINT16		subslot_nr;
	LSA_UINT16		frame_offset; /* IODataObjectFrameOffset */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockReqPart4_tag {

	LSA_UINT16		number_of_releated_iocs;

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockReqPart5_tag { /* related iocs */

	LSA_UINT16		slot_nr;
	LSA_UINT16		subslot_nr;
	LSA_UINT16		frame_offset; /* IOCSFrameOffset */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_IOCRBlockReqPart1_tag CM_REQ_IOCR1;
typedef struct cm_IOCRBlockReqPart2_tag CM_REQ_IOCR2;
typedef struct cm_IOCRBlockReqPart3_tag CM_REQ_IOCR3;
typedef struct cm_IOCRBlockReqPart4_tag CM_REQ_IOCR4;
typedef struct cm_IOCRBlockReqPart5_tag CM_REQ_IOCR5;

/*------------------------------------------------------------------------------
// IOCRBlockRes ::=
//   BlockHeader, IOCRType, IOCRReference, FrameID
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IOCRBlockRes_tag {

	/* BlockHeader not here! */

	LSA_UINT16		iocr_type;	/* see cm_iocr_type_enum */
	LSA_UINT16		iocr_ref;
	LSA_UINT16		frame_id;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_IOCRBlockRes_tag CM_RSP_IOCR;

/*------------------------------------------------------------------------------
// MCRBlockReq ::=
//   BlockHeader,
//   IOCRReference, AddressResolutionProperties,
//   MCITimeoutFactor, StationNameLength, ProviderStationName, [Padding]*
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_MCRBlockReq_tag {

	/* BlockHeader not here! */

	LSA_UINT16		iocr_ref;
	LSA_UINT32		mcr_properties; /* AddressResolutionProperties */
	LSA_UINT16		mci_timeout_factor;
	LSA_UINT16		station_name_length;

	/* LSA_UINT8 station_name[] follows */

	/* Padding follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_MCRBlockReq_tag CM_REQ_MCR;


/*------------------------------------------------------------------------------
// IRInfoBlock ::=
//   BlockHeader, Padding* a, IRDataUUID, Padding* a,
//     NumberOfIOCRs b, [(FrameID, SubframeOffset, SubframeData)*], [Padding*] c
//
// a The number of Padding octets shall be 2.
// b This field shall be coded as zero if no DFP is used.
// c The number of padding octets shall be adapted to make the block Unsigned32 aligned.
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IRInfoBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT16			padding1; /* 2 bytes */
	CM_PDU_UUID_TYPE	ir_data_uuid;

	LSA_UINT16			padding2; /* 2 bytes */
	LSA_UINT16			number_of_iocrs; /* This field shall be coded as zero if no DFP is used */
} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IRInfoSubframe_tag {

	LSA_UINT16		iocr_reference;
	LSA_UINT16		subframe_offset;
	LSA_UINT32		subframe_data;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_IRInfoBlock_tag CM_REQ_IRINFO;

typedef struct cm_IRInfoSubframe_tag CM_REQ_IRINFOSUB;


/*------------------------------------------------------------------------------
// SRInfoBlock ::=
//   BlockHeader, RedundancyDataHoldFactor, SRProperties
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_SRInfoBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT16		RedundancyDataHoldFactor;
	LSA_UINT32		SRProperties;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_SRInfoBlock_tag CM_REQ_SRINFO;


/*------------------------------------------------------------------------------
// ARVendorBlockReq ==  ARVendorBlockRes ::=
//   BlockHeader,
//   APStructureIdentifier, API, [Data*], [Padding*]
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ARVendor_tag {

	/* BlockHeader not here! */

	LSA_UINT16		APStructureIdentifier;
	LSA_UINT32		API;

	/* LSA_UINT8	Data[] follows */

	/* Padding follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ARVendor_tag CM_REQ_ARVENDOR;

typedef struct cm_ARVendor_tag CM_RSP_ARVENDOR;


/*------------------------------------------------------------------------------
// ARServerBlock ::=
//	BlockHeader, StationNameLength, CMResponderStationName, [Padding* a]
//
// a The number of Padding octets shall ensure Unsigned32 alignment.
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ARServerBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT16			cmr_name_length;	/* StationNameLength */

	/* LSA_UINT8		cmr_name[] follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ARServerBlock_tag CM_RSP_ARSERVER;


/*------------------------------------------------------------------------------
// ExpectedSubmoduleBlockReq ::=
//   BlockHeader,
//   NumberOfRelatedAPIs,
//   ( API, SlotNumber, ModuleIdentNumber, ModuleProperties,
//     NumberOfSubmodules,
//     ( SubslotNumber, SubmoduleIdentNumber, SubmoduleProperties,
//       <one or two, depending on the properties>
//       ( DataDescription, SubmoduleDataLength, LengthIOPS, LengthIOCS
//       )*
//     )*
//   )*
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ExpectedSubmoduleBlockReqPart1_tag { /* related APIs */

	/* BlockHeader not here! */

	LSA_UINT16		number_of_related_apis;

	/* part 2 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ExpectedSubmoduleBlockReqPart2_tag { /* module */

	LSA_UINT32		api;

	LSA_UINT16		slot_nr;
	LSA_UINT32		mod_ident;
	LSA_UINT16		mod_properties; /* see cm_mod_properties_enum */

	LSA_UINT16		number_of_submodules;

	/* part 3 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ExpectedSubmoduleBlockReqPart3_tag { /* submodule */

	LSA_UINT16		subslot_nr;
	LSA_UINT32		sub_ident;
	LSA_UINT16		sub_properties; /* see cm_sub_properties_enum */

	/* part 4 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ExpectedSubmoduleBlockReqPart4_tag { /* data description */

	LSA_UINT16		data_description;
	LSA_UINT16		data_length; /* SubmoduleDataLength */

	LSA_UINT8		iops_length;
	LSA_UINT8		iocs_length;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ExpectedSubmoduleBlockReqPart1_tag CM_REQ_API;
typedef struct cm_ExpectedSubmoduleBlockReqPart2_tag CM_REQ_MOD;
typedef struct cm_ExpectedSubmoduleBlockReqPart3_tag CM_REQ_SUB;
typedef struct cm_ExpectedSubmoduleBlockReqPart4_tag CM_REQ_DSC;

/*------------------------------------------------------------------------------
// ModuleDiffBlock ::=
//   BlockHeader,
//   NumberOfAPIs,
//   ( API,
//     NumberOfModules
//     ( SlotNumber, ModuleIdentNumber, ModuleState
//       NumberOfSubmodules,
//       ( SubslotNumber, SubmoduleIdentNumber, SubmoduleState
//       )*
//     )*
//   )*
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ModuleDiffBlockPart1_tag {

	/* BlockHeader not here! */

	LSA_UINT16		number_of_apis;

	/* part 2 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ModuleDiffBlockPart2_tag {

	LSA_UINT32		api;
	LSA_UINT16		number_of_modules;

	/* part 3 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ModuleDiffBlockPart3_tag { /* module */

	LSA_UINT16		slot_nr;
	LSA_UINT32		mod_ident;
	LSA_UINT16		mod_state;

	LSA_UINT16		number_of_submodules;

	/* part 4 follows */

} PNIO_PACKED_ATTRIBUTE_POST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_ModuleDiffBlockPart4_tag { /* submodule */

	LSA_UINT16		subslot_nr;
	LSA_UINT32		sub_ident;
	LSA_UINT16		sub_state;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ModuleDiffBlockPart1_tag CM_RSP_DIFF;
typedef struct cm_ModuleDiffBlockPart2_tag CM_RSP_API;
typedef struct cm_ModuleDiffBlockPart3_tag CM_RSP_MOD;
typedef struct cm_ModuleDiffBlockPart4_tag CM_RSP_SUB;

/*------------------------------------------------------------------------------
// AlarmCRBlockReq ::=
//   BlockHeader,
//   AlarmCRType, LT, AlarmCRProperties,
//   RTATimeoutFactor, RTARetries,
//   LocalAlarmReference, MaxAlarmDataLength,
//   AlarmCRTagHeaderHigh, AlarmCRTagHeaderLow
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_AlarmCRBlockReq_tag {

	/* BlockHeader not here! */

	LSA_UINT16		alcr_type;
	LSA_UINT16		ethertype;
	LSA_UINT32		alcr_properties;
	LSA_UINT16		timeout100ms;
	LSA_UINT16		retries;

	LSA_UINT16		local_alarm_ref;
	LSA_UINT16		max_alarm_data_length;

	LSA_UINT16		alcr_tag_header_high;	/* VLAN */
	LSA_UINT16		alcr_tag_header_low;	/* VLAN */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_AlarmCRBlockReq_tag CM_REQ_ALCR;

/*------------------------------------------------------------------------------
// AlarmCRBlockRes ::=
//   BlockHeader,
//   AlarmCRType,
//   LocalAlarmReference,
//   MaxAlarmDataLength
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_AlarmCRBlockRes_tag {

	/* BlockHeader not here! */

	LSA_UINT16		alcr_type;
	LSA_UINT16		local_alarm_ref; /* local: view from sender of PDU */
	LSA_UINT16		max_alarm_data_length;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_AlarmCRBlockRes_tag CM_RSP_ALCR;

/*------------------------------------------------------------------------------
// PrmServerBlock ::=
//   BlockHeader,
//   ParameterServerObjectUUID, ParameterServerProperties,
//   CMInitiatorActivityTimeout,
//   StationNameLength, ParameterServerStationName
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_PrmServerBlock_tag {

	/* BlockHeader not here! */

	CM_PDU_UUID_TYPE	prm_server_obj_uuid;
	LSA_UINT32			prm_server_properties;
	LSA_UINT16			cmi_activity_timeout100ms;

	LSA_UINT16			prm_server_name_length;

	/* LSA_UINT8 prm_server_name[] follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_PrmServerBlock_tag CM_REQ_PRMSRV;


/*------------------------------------------------------------------------------
// ControlBlockConnect ::=
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ControlBlockPlug ::=
//   BlockHeader
//   reserved (2 octets), ARUUID, SessionKey,
//   AlarmSequenceNumber, ControlCommand, ControlBlockProperties
//
// ControlBlockRFC ::= ... Ready for Companion
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ControlBlockRTC ::= ... Ready for RTClass3
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//
// ReleaseBlock ::=
//   BlockHeader,
//   reserved (2 octets), ARUUID, SessionKey,
//   reserved (2 octets), ControlCommand, ControlBlockProperties
//----------------------------------------------------------------------------*/

#define CM_CONTROL_COMMAND_PRM_END             0x0001
#define CM_CONTROL_COMMAND_APPLICATION_READY   0x0002
#define CM_CONTROL_COMMAND_RELEASE             0x0004
#define CM_CONTROL_COMMAND_DONE                0x0008
#if 0/*reserved for future use*/
#define CM_CONTROL_COMMAND_READY_FOR_COMPANION 0x0010
#endif
#define CM_CONTROL_COMMAND_READY_FOR_RTC3      0x0020
#define CM_CONTROL_COMMAND_PRM_BEGIN		   0x0040

#if 0/*reserved for future use*/
#define CM_CONTROL_PROP_READY_FOR_RFC          0x0001 /* used with CM_CONTROL_COMMAND_APPLICATION_READY */
#endif
#define CM_CONTROL_PROP_READY_FOR_RTC3         0x0002 /* used with CM_CONTROL_COMMAND_APPLICATION_READY */


PNIO_PACKED_ATTRIBUTE_PRE struct cm_ControlBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT16			reserved1;
	CM_PDU_UUID_TYPE	ar_uuid;
	LSA_UINT16			session_key;
	LSA_UINT16			reserved2_alarm_sequence_number;
	LSA_UINT16			control_command; /* see CM_CONTROL_COMMAND_PRM_END, etc. above */
	LSA_UINT16			control_properties;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_ControlBlock_tag CM_BLK_CTRL;
typedef struct cm_ControlBlock_tag CM_BLK_REL;

/*------------------------------------------------------------------------------
// SubmoduleListBlock ::=
//   BlockHeader, NumberOfEntries, (API, SlotNumber, SubslotNumber)*
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_SubmoduleListBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT16			number_of_entries;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_SubmoduleListBlock_tag CM_BLK_SUBMODULE_LIST;

PNIO_PACKED_ATTRIBUTE_PRE struct cm_SubmoduleListEntryBlock_tag {

	/* BlockHeader not here! */

	LSA_UINT32	api;
	LSA_UINT16	slot_nr;
	LSA_UINT16	subslot_nr;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_SubmoduleListEntryBlock_tag CM_BLK_SUBMODULE_LIST_ENTRY;

/*------------------------------------------------------------------------------
// IODReadReq ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   [TargetARUUID], RWPadding*
//
// IODReadRes ::=
//   IODReadHeader, RecordDataRead
//
// IODReadHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   AdditionalValue1, AdditionalValue2, RWPadding*
//
//
// IODWriteReq ::=
//   IODWriteHeader, RecordDataWrite
//
// IODWriteMultipleReq ::=
//   IODWriteHeader(Index=0xE040),
//   ( IODWriteHeader, RecordDataWrite, Padding* )*
//
// IODWriteHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   RWPadding*
//
// IODWriteRes ::=
//   IODWriteResHeader
//
// IODWriteMultipleRes ::=
//   IODWriteResHeader(Index=0xE040),
//   ( IODWriteResHeader )*
//
// IODWriteResHeader ::=
//   BlockHeader, SeqNumber, ARUUID,
//   API, SlotNumber, SubslotNumber,
//   Padding*, Index, RecordDataLength,
//   AdditionalValue1, AdditionalValue2,
//   PNIOStatus, RWPadding*
//
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_IODReadWrite_tag {

	/* BlockHeader not here! */

	LSA_UINT16			seq_number;
	CM_PDU_UUID_TYPE	ar_uuid;
	LSA_UINT32			api;
	LSA_UINT16			slot_nr;
	LSA_UINT16			subslot_nr;
	LSA_UINT16			padding_1;
	LSA_UINT16			index;
	LSA_UINT32			record_data_length;

	PNIO_PACKED_ATTRIBUTE_PRE union { /* request and response differ in the following fields */

		PNIO_PACKED_ATTRIBUTE_PRE struct {
			LSA_UINT8			padding_2[24]; /* so that the header-size is 64 */
		} PNIO_PACKED_ATTRIBUTE_POST req_rd_explicit;

		PNIO_PACKED_ATTRIBUTE_PRE struct {
			CM_PDU_UUID_TYPE	target_ar_uuid;
			LSA_UINT8			padding_2[8]; /* so that the header-size is 64 */
		} PNIO_PACKED_ATTRIBUTE_POST req_rd_implicit;

		PNIO_PACKED_ATTRIBUTE_PRE struct {
			LSA_UINT8			padding_2[24]; /* so that the header-size is 64 */
		} PNIO_PACKED_ATTRIBUTE_POST req_wr;

		PNIO_PACKED_ATTRIBUTE_PRE struct {
			LSA_UINT16			additional_value_1;
			LSA_UINT16			additional_value_2;
			LSA_UINT8			padding_2[20]; /* so that the header-size is 64 */
		} PNIO_PACKED_ATTRIBUTE_POST rsp_rd_wr; /* read and write */

		PNIO_PACKED_ATTRIBUTE_PRE struct {
			LSA_UINT16			additional_value_1;
			LSA_UINT16			additional_value_2;
			LSA_UINT32			pnio_status;
			LSA_UINT8			padding_2[16]; /* so that the header-size is 64 */
		} PNIO_PACKED_ATTRIBUTE_POST rsp_wr;

	} PNIO_PACKED_ATTRIBUTE_POST u;

	/* LSA_UINT8 record_data[] follows */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_IODReadWrite_tag CM_BLK_READ_WRITE;

#define CM_IOD_READ_WRITE_HEADER_SIZE_ASSERT() { \
	CM_ASSERT(CM_RECORD_HEADER_SIZE == 64); \
	CM_ASSERT(sizeof(CM_BLK_HEADER) + sizeof(CM_BLK_READ_WRITE) == CM_RECORD_HEADER_SIZE); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u ) == 24); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u.req_rd_explicit ) == 24); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u.req_rd_implicit ) == 24); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u.req_wr ) == 24); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u.rsp_rd_wr ) == 24); \
	CM_ASSERT(sizeof( ((CM_BLK_READ_WRITE*)0)->u.rsp_wr ) == 24); \
	}

/*------------------------------------------------------------------------------
// LogEntry ::=
//   LocalTimeStamp, ARUUID, PNIOStatus, EntryDetail
//----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct cm_LogEntry_tag {

	LSA_UINT32			local_time_stamp_high;
	LSA_UINT32			local_time_stamp_low;

	CM_PDU_UUID_TYPE	ar_uuid;

	LSA_UINT32			pnio_status;
	LSA_UINT32			entry_detail;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct cm_LogEntry_tag CM_PDU_LOG_ENTRY;

/*----------------------------------------------------------------------------*/

#include "pnio_pck_off.h" /* unpacking macros */

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_PDU_H */
