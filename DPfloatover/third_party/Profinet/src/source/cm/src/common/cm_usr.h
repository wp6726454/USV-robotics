#ifndef CM_USR_H
#define CM_USR_H

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
/*  F i l e               &F: cm_usr.h                                  :F&  */
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

#ifndef CM_CFG_EVERYTHING_VISIBLE
#define CM_CFG_EVERYTHING_VISIBLE 1 /* 1 = everything visible (AP00754346) */
#else
/* must not be defined outside of cm_usr.h (except for testing) */
#endif

/*-----------------------------------------------------------------------------
// common types
//---------------------------------------------------------------------------*/

typedef LSA_UINT8 CM_COMMON_MEM_ATTR * CM_COMMON_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 CM_UPPER_MEM_ATTR * CM_UPPER_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 CM_ACP_LOWER_MEM_ATTR * CM_ACP_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 CM_RPC_LOWER_MEM_ATTR * CM_RPC_MEM_U8_PTR_TYPE;

typedef struct cm_mac_tag {
	LSA_UINT8  mac[6];
} CM_MAC_TYPE;

typedef CM_MAC_TYPE CM_COMMON_MEM_ATTR *CM_MAC_PTR_TYPE;

typedef struct cm_reserved_interval_if_red_tag
{
    LSA_UINT32   RxEndNs;
    LSA_UINT32   RxLocalTransferEndNs;
    LSA_UINT32   TxEndNs;
    LSA_UINT32   TxLocalTransferStartNs;
    LSA_UINT32   TxLocalTransferEndNs;
} CM_RESERVED_INTERVAL_IF_RED_TYPE;

typedef CM_RESERVED_INTERVAL_IF_RED_TYPE CM_COMMON_MEM_ATTR *CM_RESERVED_INTERVAL_IF_RED_PTR_TYPE;

/*-----------------------------------------------------------------------------
// includes
//---------------------------------------------------------------------------*/

#include "cm_err.h" /* PNIO error codes and make-fetch-macros */

#include "cm_lib.h" /* conversion functions */

#include "cm_list.h" /* list processing macros */

#include "cm_arcb.h" /* ARCB .. the Application Relation Control Block */


/*------------------------------------------------------------------------------
//	OPCODES
//	CM_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

#define CM_OPC_TIMER  0 /* internally used only */

	/*** channel-specific opcodes ***/

#define CM_OPC_OPEN_CHANNEL   1
#define CM_OPC_CLOSE_CHANNEL  2
#define CM_OPC_CHANNEL_INFO   3

#define CM_OPC_OVERLOAD_INFO  4
#define CM_OPC_reserved5      5

#define CM_OPC_EVENT_IND_RES_PROVIDE  6
#define CM_OPC_ALARM_IND_RES_PROVIDE  7

#define CM_OPC_IND_RES_WITHDRAW       8

	/*** client-specific opcodes ***/

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE

		/* cl-channel specific */
#define CM_OPC_CL_CREATE   20
#define CM_OPC_CL_DELETE   21
#define CM_OPC_CL_CONTROL  22

		/* device/ar specific */
#define CM_OPC_CL_DEVICE_ADD      23
#define CM_OPC_CL_DEVICE_REMOVE   24
#define CM_OPC_CL_DEVICE_CONTROL  25

#define CM_OPC_CL_RECORD_READ        26
#define CM_OPC_CL_RECORD_READ_QUERY  27

#define CM_OPC_CL_RECORD_READ_IMPLICIT        28
#define CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY  29

#define CM_OPC_CL_RECORD_WRITE           30
#define CM_OPC_CL_RECORD_WRITE_MULTIPLE  31

#define CM_OPC_CL_ALARM_SEND      32
#define CM_OPC_CL_ALARM_ACK_SEND  33

#define CM_OPC_CL_LOGBOOK_READ  34

#define CM_OPC_CL_DEVICE_PROVIDE_ALARM  35
#define CM_OPC_CL_ALARM_IND             36
#define CM_OPC_CL_ALARM_RSP             37

#define CM_OPC_CL_DEVICE_PROVIDE_EVENT  40
#define CM_OPC_CL_AR_ABORT_IND          41
#define CM_OPC_CL_AR_ABORT_RSP          42
#define CM_OPC_CL_AR_PRE_DATA_IND       43
#define CM_OPC_CL_AR_PRE_DATA_RSP       44
#define CM_OPC_CL_AR_PRE_OFFLINE_IND    45
#define CM_OPC_CL_AR_PRE_OFFLINE_RSP    46
#define CM_OPC_CL_AR_IN_DATA_IND        47
#define CM_OPC_CL_AR_IN_DATA_RSP        48
#define CM_OPC_CL_AR_OFFLINE_IND        49
#define CM_OPC_CL_AR_OFFLINE_RSP        50

#define CM_OPC_CL_AR_DATA_STATUS_IND  51
#define CM_OPC_CL_AR_DATA_STATUS_RSP  52

#define CM_OPC_CL_PRM_UPDATE         53
#define CM_OPC_CL_AR_PRM_BEGIN_IND   54
#define CM_OPC_CL_AR_PRM_BEGIN_RSP   55
#define CM_OPC_CL_AR_PRM_INDATA_IND  56
#define CM_OPC_CL_AR_PRM_INDATA_RSP  57

		/* internally used only */
#define CM_INTERNAL_OPC_CL_ALARM_ACK_SEND  59

#endif /* CM_CFG_MAX_CLIENTS */

	/*** multicast-specific opcodes ***/

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

		/* mc-channel specific */
#define CM_OPC_MC_CREATE   60
#define CM_OPC_MC_DELETE   61
#define CM_OPC_MC_CONTROL  62

		/* device/ar commands */
#define CM_OPC_MC_DEVICE_ADD      63
#define CM_OPC_MC_DEVICE_REMOVE   64
#define CM_OPC_MC_DEVICE_CONTROL  65

		/* device/ar indications */
#define CM_OPC_MC_EVENT_AR_ADDR_INFO_IND  66
#define CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP  67
#define CM_OPC_MC_EVENT_AR_IN_DATA_IND    68
#define CM_OPC_MC_EVENT_AR_IN_DATA_RSP    69
#define CM_OPC_MC_EVENT_AR_OFFLINE_IND    70
#define CM_OPC_MC_EVENT_AR_OFFLINE_RSP    71
#define CM_OPC_MC_EVENT_APDU_STATUS_IND   72
#define CM_OPC_MC_EVENT_APDU_STATUS_RSP   73

		/* internally used only */
#define CM_INTERNAL_OPC_MC_EVENT_AR_ADDR_INFO_GOOD_IND  74

#endif /* CM_CFG_MAX_MULTICAST_CHANNELS */

	/*** server-specific opcodes ***/

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

#define CM_OPC_SV_CREATE   100
#define CM_OPC_SV_DELETE   101
#define CM_OPC_SV_CONTROL  102

#define CM_OPC_SV_DEVICE_ADD            103
#define CM_OPC_SV_DEVICE_REMOVE         104
#define CM_OPC_SV_DEVICE_CONTROL        105
#define CM_OPC_SV_DEVICE_LED_INFO       106
#define CM_OPC_SV_DEVICE_PROVIDE_EVENT  107
#define CM_OPC_SV_DEVICE_PROVIDE_ALARM  108
#define CM_OPC_SV_DEVICE_READ           109

#define CM_OPC_SV_reserved110  110

#define CM_OPC_SV_SUBMODULE_ADD     111
#define CM_OPC_SV_SUBMODULE_REMOVE  112

#define CM_OPC_SV_reserved113  113

#define CM_OPC_SV_DIAG_ADD     114
#define CM_OPC_SV_DIAG_REMOVE  115

#define CM_OPC_SV_DIAG_ADD2     116 /* V5.2i4.16, AP01467031 */
#define CM_OPC_SV_DIAG_REMOVE2  117 /* V5.2i4.16, AP01467031 */

#define CM_OPC_SV_ARSET_TRIGGER  118
#define CM_OPC_SV_ARSET_ABORT    119

#define CM_OPC_SV_AR_APPL_READY  120
#define CM_OPC_SV_AR_CONTROL     121
#define CM_OPC_SV_AR_ABORT       122
#define CM_OPC_SV_reserved123    123
#define CM_OPC_SV_reserved124    124
#define CM_OPC_SV_AR_RIR_ACK     125

#define CM_OPC_SV_AR_ALARM_SEND  126

#define CM_OPC_SV_AR_ALARM_IND  127
#define CM_OPC_SV_AR_ALARM_RSP  128
#define CM_OPC_SV_AR_ALARM_ACK  129

#define CM_OPC_SV_AR_CONNECT_IND  130
#define CM_OPC_SV_AR_CONNECT_RSP  131

#define CM_OPC_SV_AR_DISCONNECT_IND  132
#define CM_OPC_SV_AR_DISCONNECT_RSP  133

#define CM_OPC_SV_AR_OWNERSHIP_IND  134
#define CM_OPC_SV_AR_OWNERSHIP_RSP  135

#define CM_OPC_SV_AR_PRM_END_IND  136
#define CM_OPC_SV_AR_PRM_END_RSP  137

#define CM_OPC_SV_AR_IN_DATA_IND  138
#define CM_OPC_SV_AR_IN_DATA_RSP  139

#define CM_OPC_SV_AR_RIR_IND  140
#define CM_OPC_SV_AR_RIR_RSP  141

#define CM_OPC_SV_AR_READ_IND  142
#define CM_OPC_SV_AR_READ_RSP  143

#define CM_OPC_SV_AR_WRITE_IND  144
#define CM_OPC_SV_AR_WRITE_RSP  145

#define CM_OPC_SV_AR_DATA_STATUS_IND  146
#define CM_OPC_SV_AR_DATA_STATUS_RSP  147

#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE
#define CM_OPC_SV_AR_MCONS_IND  148
#define CM_OPC_SV_AR_MCONS_RSP  149
#define CM_OPC_SV_reserved150   150
#define CM_OPC_SV_reserved151   151
#define CM_OPC_SV_reserved152   152
#define CM_OPC_SV_reserved153   153
#define CM_OPC_SV_reserved154   154
#define CM_OPC_SV_reserved155   155
#endif

#define CM_OPC_SV_AR_ALARM_PDEV_GROUP    156 /* internally used only */
#define CM_OPC_SV_AR_ALARM_PLUG_GROUP    157 /* internally used only */
#define CM_OPC_SV_AR_ALARM_DIAG_GROUP    158 /* internally used only */
#define CM_OPC_SV_AR_ALARM_ACK_INTERNAL  159 /* internally used only */
#define CM_OPC_SV_AR_ALARM_RPL_INTERNAL  160 /* internally used only */

#define CM_OPC_SV_reserved161  161
#define CM_OPC_SV_reserved162  162
#define CM_OPC_SV_reserved163  163
#define CM_OPC_SV_reserved164  164

#define CM_OPC_SV_AR_PDEVPRM_IND    165
#define CM_OPC_SV_AR_PDEVPRM_RSP    166
#define CM_OPC_SV_AR_PDEVPRM_READ   167
#define CM_OPC_SV_AR_PDEVPRM_WRITE  168

#endif /* CM_CFG_MAX_SERVERS */

	/*** pdev-specific opcodes ***/

#define CM_OPC_PD_CREATE  200
#define CM_OPC_PD_DELETE  201

#define CM_OPC_PD_REMAP   202

#define CM_OPC_PD_LINK_STATUS_INFO  203
#define CM_OPC_PD_SYNC_LED_INFO     204

#define CM_OPC_PD_PRM_READ  210

#define CM_OPC_PD_PRM_BEGIN  211
#define CM_OPC_PD_PRM_WRITE  212
#define CM_OPC_PD_PRM_END    213

#define CM_OPC_PD_reserved214  214

#define CM_OPC_PD_EVENT_APPL_READY_IND  215
#define CM_OPC_PD_EVENT_APPL_READY_RSP  216

#define CM_OPC_PD_ALARM_IND  217
#define CM_OPC_PD_ALARM_RSP  218

#define CM_OPC_PD_reserved219  219

	/* note: OWNERSHIP_SERVER is synchronous */
#define CM_OPC_PD_PRM_READ_SERVER              231 /* internal, cm-sv to cm-pd */
#define CM_OPC_PD_PRM_WRITE_SERVER             232 /* internal, cm-sv to cm-pd */
#define CM_OPC_PD_PRM_END_SERVER               233 /* internal, cm-sv to cm-pd */
#define CM_OPC_PD_PRM_RELINQUISH_SERVER        234 /* internal, cm-sv to cm-pd */
#define CM_OPC_PD_PRM_READ_CLIENT              235 /* internal, cm-cl to cm-pd */
#define CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER  236 /* internal, cm-pd to cm-pd */

	/***/

#define CM_OPC_MAX  250


/*------------------------------------------------------------------------------
//	ERROR_CODES
//	CM_RQB_TYPE::response
//----------------------------------------------------------------------------*/

enum cm_response_enum {
	CM_OK                   = LSA_RSP_OK /* (usually 0x01 but configurable) */,
	CM_OK_PENDING           = (LSA_RSP_OK_OFFSET + 0) /* (0x40) internally used only! */,
	CM_OK_CANCELLED         = (LSA_RSP_OK_OFFSET + 1) /* (0x41) */,
	CM_OK_REPLACED          = (LSA_RSP_OK_OFFSET + 2) /* (0x42) */,
	CM_OK_ADAPTION_NEEDED   = (LSA_RSP_OK_OFFSET + 3) /* (0x43) */,
	CM_OK_RESET_TO_FACTORY  = (LSA_RSP_OK_OFFSET + 4) /* (0x44) */,

	CM_ERR_RESOURCE         = LSA_RSP_ERR_RESOURCE /* (0x84) */,
	CM_ERR_PARAM            = LSA_RSP_ERR_PARAM    /* (0x86) */,
	CM_ERR_SEQUENCE         = LSA_RSP_ERR_SEQUENCE /* (0x87) */,
	CM_ERR_SYS_PATH         = LSA_RSP_ERR_SYS_PATH /* (0x91) */,

	CM_ERR_ELEM = LSA_RSP_ERR_OFFSET, /* (0xc0) must check elem[].response */
	CM_ERR_ALLOC_UPPER,     /* (0xc1) could not allocate upper memory */
	CM_ERR_ALLOC_LOCAL,     /* (0xc2) could not allocate local memory */
	CM_ERR_ALLOC_LOWER,     /* (0xc3) could not allocate lower memory */
	CM_ERR_LOWER_LAYER,     /* (0xc4) an error occurred in the lower layer */
	CM_ERR_NOT_ACCESSIBLE,  /* (0xc5) internally used */
	CM_ERR_ABORT,           /* (0xc6) operation was abortet */
	CM_ERR_SESSION,         /* (0xc7) request belongs to an old session */
	CM_ERR_TIMEOUT,         /* (0xc8) RPC failed due to a timeout */
	CM_ERR_COMM,            /* (0xc9) RPC failed due to a comm-error */
	CM_ERR_BUSY,            /* (0xca) RPC peer signalled "busy" (should try again later) */
	CM_ERR_LOWER_NARE,      /* (0xcb) an error occurred in the lower layer NARE */
	CM_ERR_LOWER_RPC,       /* (0xcc) an error occurred in the lower layer CLRPC */
	CM_ERR_RESERVED_CD,     /* (0xcd) */
	CM_ERR_OWNED,           /* (0xce) interface-submodule cannot be removed because it is owned by an AR */
	CM_ERR_FORMAT,          /* (0xcf) wrong format of alarm data */
	CM_ERR_PRM_INDEX,       /* (0xd0) unknown index (PrmWrite and PrmRead) */
	CM_ERR_PRM_PORTID,      /* (0xd1) port-id does not match with index (PrmWrite and PrmRead) */
	CM_ERR_PRM_DATA,        /* (0xd2) data-length too short (PrmRead) or data-length not consistent with block-structure (PrmWrite) */
	CM_ERR_PRM_BLOCK,       /* (0xd3) wrong block-type/version or something wrong with the block-data (PrmWrite) */
	CM_ERR_PRM_CONSISTENCY, /* (0xd4) the parameterization is not consistent (PrmEnd) */
	CM_ERR_PRM_OTHER,       /* (0xd5) internally used */
	CM_ERR_PRM_VERSION,     /* (0xd6) internally used */
	CM_ERR_ARVENDORBLOCK,   /* (0xd7) could not decode ARVendorBlockReq (V5.2i3, AP01354956) */

	/*** attention: max. 62 error codes ***/

	CM_ERR_NOT_YET = 0xFE   /* lsa-components may use 0xc0 - 0xfe ... */
};


/*-----------------------------------------------------------------------------
// ChannelProperties, see PNIO-Spec
//---------------------------------------------------------------------------*/

#define CM_DIAG_CHANNEL_PROPERTIES_MAKE(type, acc, m_req, m_dem, spec, dir) ((LSA_UINT16) \
	( (((type)  & 0xFF) <<  0) /* mask 0x00FF */ \
	| (((acc)   & 0x01) <<  8) /* mask 0x0100 */ \
	| (((m_req) & 0x01) <<  9) /* mask 0x0200 */ \
	| (((m_dem) & 0x01) << 10) /* mask 0x0400 */ \
	| (((spec)  & 0x03) << 11) /* mask 0x1800 */ \
	| (((dir)   & 0x07) << 13) /* mask 0xE000 */ \
	))

#define CM_CHPR_TYPE_MASK  0x00FF /* Bit 0 - 7: ChannelProperties.Type */
#define CM_CHPR_TYPE_OTHER 0x0000 /*  other */
#define CM_CHPR_TYPE_1     0x0001 /*  0x01 1 Bit */
#define CM_CHPR_TYPE_2     0x0002 /*  0x02 2 Bit */
#define CM_CHPR_TYPE_4     0x0003 /*  0x03 4 Bit */
#define CM_CHPR_TYPE_8     0x0004 /*  0x04 8 Bit */
#define CM_CHPR_TYPE_16    0x0005 /*  0x05 16 Bit */
#define CM_CHPR_TYPE_32    0x0006 /*  0x06 32 Bit */
#define CM_CHPR_TYPE_64    0x0007 /*  0x07 64 Bit */
#define CM_CHPR_TYPE_RSVD  0x00FF /*  0x08 - 0xFF reserved */

#define CM_CHPR_ACCU_MASK  0x0100 /* Bit 8 : ChannelProperties.Accumulative */
#define CM_CHPR_ACCU_NO    0x0000
#define CM_CHPR_ACCU_YES   0x0100

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MREQ_MASK  0x0200 /* Bit 9 : ChannelProperties.MaintenanceRequired */
#define CM_CHPR_MREQ_NO    0x0000
#define CM_CHPR_MREQ_YES   0x0200

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MDEM_MASK  0x0400 /* Bit 10 : ChannelProperties.MaintenanceDemanded */
#define CM_CHPR_MDEM_NO    0x0000
#define CM_CHPR_MDEM_YES   0x0400

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MAINT_MASK (CM_CHPR_MREQ_MASK | CM_CHPR_MDEM_MASK)
#define CM_CHPR_MAINT_ERR  (CM_CHPR_MREQ_NO   | CM_CHPR_MDEM_NO)   /* Diagnosis */
#define CM_CHPR_MAINT_REQ  (CM_CHPR_MREQ_YES  | CM_CHPR_MDEM_NO)   /* MaintenanceRequired */
#define CM_CHPR_MAINT_DEM  (CM_CHPR_MREQ_NO   | CM_CHPR_MDEM_YES)  /* MaintenanceDemanded */
#define CM_CHPR_MAINT_QUA  (CM_CHPR_MREQ_YES  | CM_CHPR_MDEM_YES)  /* QualifiedDiagnosis */

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_SPEC_MASK  0x1800 /* Bit 11 - 12: ChannelProperties.Specifier */
#define CM_CHPR_SPEC_RSVD  0x0000 /*  0x00 reserved */
#define CM_CHPR_SPEC_APP   0x0800 /*  0x01 error appears */
#define CM_CHPR_SPEC_DIS   0x1000 /*  0x02 error disappears and error free */
#define CM_CHPR_SPEC_DBO   0x1800 /*  0x03 error disappears but other errors remain */

#define CM_CHPR_DIR_MASK   0xE000 /* Bit 13 - 15: ChannelProperties.Direction */
#define CM_CHPR_DIR_OTHER  0x0000 /*  0x00 manufacturer specific */
#define CM_CHPR_DIR_IN     0x2000 /*  0x01 Input */
#define CM_CHPR_DIR_OUT    0x4000 /*  0x02 Output */
#define CM_CHPR_DIR_INOUT  0x6000 /*  0x03 Input/Output */
#define CM_CHPR_DIR_RSVD   0xE000 /*  0x04 - 0x07 reserved */

/*-----------------------------------------------------------------------------
// forward declaration
//---------------------------------------------------------------------------*/

typedef struct cm_rqb_tag CM_UPPER_RQB_ATTR * CM_UPPER_RQB_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE:		CM_OPC_ALARM_IND_RES_PROVIDE
// INDICATION:	CM_OPC_PD_ALARM_IND
// RESPONSE:	CM_OPC_PD_ALARM_RSP
//
// OPCODE:		CM_OPC_CL_DEVICE_PROVIDE_ALARM
// INDICATION:	CM_OPC_CL_ALARM_IND
// RESPONSE:	CM_OPC_CL_ALARM_RSP
// OPCODE:		CM_OPC_CL_ALARM_SEND
// OPCODE:		CM_OPC_CL_ALARM_ACK_SEND
//
// OPCODE:		CM_OPC_SV_AR_ALARM_SEND
// OPCODE:		CM_OPC_SV_DEVICE_PROVIDE_ALARM
// INDICATION:	CM_OPC_SV_AR_ALARM_IND
// RESPONSE:	CM_OPC_SV_AR_ALARM_RSP
// OPCODE:		CM_OPC_SV_AR_ALARM_ACK
//---------------------------------------------------------------------------*/

#define CM_ALARM_PRIORITY_LOW   ACP_ALARM_PRIORITY_LOW
#define CM_ALARM_PRIORITY_HIGH  ACP_ALARM_PRIORITY_HIGH

/*
0x0000 reserved
*/
#define CM_ALARM_TYPE_DIAGNOSIS                 0x0001
#define CM_ALARM_TYPE_PROCESS                   0x0002
#define CM_ALARM_TYPE_PULL                      0x0003
#define CM_ALARM_TYPE_PLUG                      0x0004
#define CM_ALARM_TYPE_STATUS                    0x0005
#define CM_ALARM_TYPE_UPDATE                    0x0006
#define CM_ALARM_TYPE_MEDIA_REDUNDANCY          0x0007
#define CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR  0x0008
#define CM_ALARM_TYPE_RELEASED                  0x0009 /* old: RELEASED_BY_SUPERVISOR */
#define CM_ALARM_TYPE_PLUG_WRONG                0x000A
#define CM_ALARM_TYPE_RETURN_OF_SUBMODULE       0x000B
#define CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS      0x000C
#define CM_ALARM_TYPE_MCR_MISMATCH              0x000D /* Multicast Communication Mismatch */
#define CM_ALARM_TYPE_PORT_DATA_CHANGED         0x000E
#define CM_ALARM_TYPE_SYNC_DATA_CHANGED         0x000F
#define CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM    0x0010
#define CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM 0x0011
#define CM_ALARM_TYPE_TIME_DATA_CHANGED         0x0012
#define CM_ALARM_TYPE_DFP_PROBLEM               0x0013
#define CM_ALARM_TYPE_MRPD_PROBLEM              0x0014
#define CM_ALARM_TYPE_reserved_0x0015           0x0015
#define CM_ALARM_TYPE_MULTIPLE_INTERFACE        0x0016 /* Multiple interface mismatch notification */
#define CM_ALARM_TYPE_reserved_0x0017           0x0017
#define CM_ALARM_TYPE_reserved_0x0018           0x0018
#define CM_ALARM_TYPE_reserved_0x0019           0x0019
#define CM_ALARM_TYPE_reserved_0x001A           0x001A
#define CM_ALARM_TYPE_reserved_0x001B           0x001B
#define CM_ALARM_TYPE_reserved_0x001C           0x001C
#define CM_ALARM_TYPE_reserved_0x001D           0x001D
#define CM_ALARM_TYPE_UPLOAD_AND_STORAGE        0x001E
#define CM_ALARM_TYPE_PULL_MODULE               0x001F /* see AP00517543 */
/*
0x0020 - 0x007F manufacturer specific
0x0080 - 0x00FF reserved for profiles
0x0100 - 0xFFFF reserved
*/

#define CM_ALARM_MANUFACTURER_SPECIFIC_GROUP \
			( (1UL << 0) /* using bit 0 for the whole range 0x0020 - 0x007F */ )

#define CM_ALARM_RESERVED_GROUP \
			( (1UL << CM_ALARM_TYPE_reserved_0x0015) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0017) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0018) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0019) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001A) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001B) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001C) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001D) \
			)

#define CM_ALARM_PLUG_GROUP \
			( (1UL << CM_ALARM_TYPE_PULL) \
			| (1UL << CM_ALARM_TYPE_PLUG) \
			| (1UL << CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR) \
			| (1UL << CM_ALARM_TYPE_RELEASED) \
			| (1UL << CM_ALARM_TYPE_PLUG_WRONG) \
			| (1UL << CM_ALARM_TYPE_RETURN_OF_SUBMODULE) \
			| (1UL << CM_ALARM_TYPE_PULL_MODULE) \
			)

#define CM_ALARM_DIAG_GROUP \
			( (1UL << CM_ALARM_TYPE_DIAGNOSIS) \
			| (1UL << CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS) \
			| (1UL << CM_ALARM_TYPE_MEDIA_REDUNDANCY) \
			| (1UL << CM_ALARM_TYPE_MCR_MISMATCH) \
			| (1UL << CM_ALARM_TYPE_PORT_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_SYNC_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_TIME_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_DFP_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_MRPD_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_MULTIPLE_INTERFACE) \
			)

#define CM_ALARM_DATA_LENGTH_MIN   200  /* min. size of the AlarmNotification-PDU, see part 6 */
#define CM_ALARM_DATA_LENGTH_MAX   1432 /* max. size of the AlarmNotification-PDU, see part 6 */

#define CM_ALARM_HEADER_LENGTH     (ACP_ALARM_USER_DATA_OFFSET - 2)

#define CM_ALARM_OFFSET_USI        (CM_ALARM_HEADER_LENGTH)
#define CM_ALARM_OFFSET_DATA       (CM_ALARM_HEADER_LENGTH + 2)
#define CM_ALARM_OFFSET_SPECIFIER  (CM_ALARM_HEADER_LENGTH - 2)

	/*
	 * see PNIO-Spec for the definition of AlarmNotification-PDU and AlarmItem
	 *
	 * the min. size of the AlarmNotification-PDU is CM_ALARM_DATA_LENGTH_MIN
	 * the max. size of the AlarmNotification-PDU is CM_ALARM_DATA_LENGTH_MAX
	 *
	 * CM_OPC_CL_ALARM_SEND and CM_OPC_SV_AR_ALARM_SEND
	 *
	 *   alarm_data ......... points to AlarmItem::DATA
	 *   alarm_data_length .. is the length of AlarmItem::DATA
	 *   alarm_tag .......... is the AlarmItem::UserStructureIdentifier (in host-byte-order)
	 *                        it is used only if alarm_data_length != 0, else "don't care".
	 *
	 * CM_OPC_CL_ALARM_IND and CM_OPC_SV_AR_ALARM_IND
	 *
	 *   alarm_data ......... points to AlarmNotification-PDU to allow accessing the "raw" data
	 *   alarm_data_length .. is the length of the AlarmNotification-PDU
	 *   alarm_tag .......... is the AlarmItem::UserStructureIdentifier (in host-byte-order)
	 *
	 *   alarm_data + CM_ALARM_OFFSET_USI ... points to the "raw" AlarmItem::UserStructureIdentifier
	 *   alarm_data + CM_ALARM_OFFSET_DATA .. points to the "raw" AlarmItem::DATA
	 *
	 *   alarm_tag, AlarmItem::UserStructureIdentifier and AlarmItem::DATA
	 *   are valid only if alarm_data_length >= CM_ALARM_OFFSET_DATA
	 */

#define CM_ALARM_USI_MAINTENANCE  0x8100

#define CM_MAINTENANCE_STATUS_REQUIRED  0x00000001
#define CM_MAINTENANCE_STATUS_DEMANDED  0x00000002
#define CM_MAINTENANCE_STATUS_RESERVED  0x00000004 /* reserved since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11 */
#define CM_MAINTENANCE_STATUS_QUA_BITS  0xFFFFFFF8 /* see QualifiedChannelQualifier */

typedef struct cm_alarm_tag {

	LSA_UINT16  device_nr;   /* device number */
	LSA_UINT16  ar_nr;       /* ar number */
	LSA_UINT16  session_key; /* session key, see CL_EVENT_AR_OFFLINE_IND or CM_OPC_SV_AR_CONNECT_IND */

	LSA_UINT16  alarm_priority; /* CM_ALARM_PRIORITY_LOW or CM_ALARM_PRIORITY_HIGH */

	LSA_UINT32  api;        /* application process identifier */
	LSA_UINT16  slot_nr;    /* slot number */
	LSA_UINT16  subslot_nr; /* subslot number */

	LSA_UINT16  alarm_type; /* see CM_ALARM_TYPE_... */

	LSA_UINT16  alarm_sequence;         /* see "AlarmSpecifier" bits 0-10 */
	LSA_BOOL    diag_channel_available; /* see "AlarmSpecifier" bit 11 */
	LSA_BOOL    diag_generic_available; /* see "AlarmSpecifier" bit 12 */
	LSA_BOOL    diag_submod_available;  /* see "AlarmSpecifier" bit 13 */
	LSA_UINT8   reserved;               /* see "AlarmSpecifier" bit 14 */
	LSA_BOOL    ar_diagnosis_state;     /* see "AlarmSpecifier" bit 15 */

	LSA_UINT32  mod_ident;
	LSA_UINT32  sub_ident;

	LSA_UINT16                 alarm_tag;         /* see comment on CM_ALARM_OFFSET_DATA above */
	LSA_UINT16                 alarm_data_length; /* see comment on CM_ALARM_OFFSET_DATA above */
	CM_COMMON_MEM_U8_PTR_TYPE  alarm_data;        /* see comment on CM_ALARM_OFFSET_DATA above */

	LSA_UINT32  cm_pnio_err; /* see the macro CM_PNIO_ERR_MAKE() in file cm_err.h */
			/* alarm.req: don't care */
			/* alarm.cnf: see CM_PNIO_ERR_MAKE() */
			/* alarm.ind: see CM_PNIO_ERR_MAKE() */
			/* alarm.rsp: don't care */

	LSA_UINT32  maintenance_status; /* see CM_MAINTENANCE_STATUS_xxx */
			/* alarm.req: don't care
			 * alarm.cnf: current status
			 * alarm.ind: from the maintenance extension (USI 0x8100)
			 *            BlockType := 0x0F00
			 *            BlockVersion := 1.0
			 *            BlockLength := 4
			 *            Value := U32 (Bitfield)
			 * alarm.rsp: don't change (same as in alarm.ind)
			 */

	union {

		struct {
			LSA_UINT32  nothing_so_far; /* do not evaluate */
		} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		struct {
			/* only valid with (alarm_type == CM_ALARM_TYPE_PLUG or CM_ALARM_TYPE_RELEASED) */
			CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  diff_list;
			LSA_UINT16  mod_diff_block_length;  /* AP00679788 */
			CM_UPPER_MEM_PTR_TYPE  mod_diff_block;
		} cl;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		struct {
			LSA_VOID_PTR_TYPE void_ptr; /* do not evaluate (used internally) */
		} sv;
#endif
	} u;

} CM_ALARM_TYPE;

typedef CM_ALARM_TYPE CM_UPPER_MEM_ATTR *CM_UPPER_ALARM_PTR_TYPE;

typedef CM_UPPER_ALARM_PTR_TYPE CM_UPPER_CL_ALARM_SEND_PTR_TYPE; /* just an alias */

typedef CM_UPPER_ALARM_PTR_TYPE CM_UPPER_CL_ALARM_ACK_SEND_PTR_TYPE; /* just an alias */

/*-----------------------------------------------------------------------------
// module- and submodule-states
//---------------------------------------------------------------------------*/

enum cm_mod_state_enum {
	CM_MOD_STATE_NO_MODULE           = 0,
	CM_MOD_STATE_WRONG_MODULE        = 1,
	CM_MOD_STATE_PROPER_MODULE       = 2,
	CM_MOD_STATE_SUBSTITUTED_MODULE  = 3,
	/***/
	CM_MOD_STATE_RESERVED
};

enum cm_sub_state_enum {

	/* PNIO V2.0 defines a new format for the submodule-state */

	CM_SUB_STATE_FORMAT_MASK                    = 0x8000,
	CM_SUB_STATE_FORMAT_0                       = 0x0000, /* note: only for backward-compatibility */
	CM_SUB_STATE_FORMAT_1                       = 0x8000,

	/* the format 0 definitions - DO NOT USE ANYMORE! */

	CM_SUB_STATE_DETAIL_MASK                    = 0x0007, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_NO_SUBMODULE            = 0x0000, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_WRONG_SUBMODULE         = 0x0001, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_LOCKED_IOCONTROLLER     = 0x0002, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_3              = 0x0003, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_APPL_READY_PENDING      = 0x0004, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_5              = 0x0005, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_6              = 0x0006, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_SUBSTITUTED_SUBMODULE   = 0x0007, /* DO NOT USE ANYMORE! */

	/* the format 1 definitions */

	CM_SUB_STATE_ADD_INFO_MASK                  = 0x0007, /* Bit 0 - 2: SubmoduleState.AddInfo */
	CM_SUB_STATE_ADD_INFO_NONE                  = 0x0000,
	CM_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED  = 0x0001,

	CM_SUB_STATE_QUALIFIED_INFO_MASK            = 0x0008, /* Bit 3: SubmoduleState.QualifiedInfo */
	CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE   = 0x0000,
	CM_SUB_STATE_QUALIFIED_INFO_AVAILABLE       = 0x0008,

	CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK      = 0x0010, /* Bit 4: SubmoduleState.MaintenanceRequired */
	CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED       = 0x0000,
	CM_SUB_STATE_MAINTENANCE_REQUIRED           = 0x0010,

	CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK      = 0x0020, /* Bit 5: SubmoduleState.MaintenanceDemanded */
	CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED       = 0x0000,
	CM_SUB_STATE_MAINTENANCE_DEMANDED           = 0x0020,

	CM_SUB_STATE_DIAG_INFO_MASK                 = 0x0040, /* Bit 6: SubmoduleState.DiagInfo */
	CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE        = 0x0000,
	CM_SUB_STATE_DIAG_INFO_AVAILABLE            = 0x0040,

	CM_SUB_STATE_AR_INFO_MASK                   = 0x0780, /* Bit 7 - 10: SubmoduleState.ARInfo */
	CM_SUB_STATE_AR_INFO_OWNED                  = 0x0000,
	CM_SUB_STATE_AR_INFO_APPL_READY_PENDING     = 0x0080,
	CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED  = 0x0100,
	CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER    = 0x0180,
	CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR    = 0x0200,

	CM_SUB_STATE_IDENT_MASK                     = 0x7800, /* Bit 11 - 14: SubmoduleState.IdentInfo */
	CM_SUB_STATE_IDENT_OK                       = 0x0000,
	CM_SUB_STATE_IDENT_SUBSTITUTE               = 0x0800,
	CM_SUB_STATE_IDENT_WRONG                    = 0x1000,
	CM_SUB_STATE_IDENT_NO_SUBMODULE             = 0x1800
};

#define CM_SUB_STATE_PULLED /* for initialization */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_NO_SUBMODULE \
	)

#define CM_SUB_STATE_GOOD /* what the client assumes if not told otherwise */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_OK \
	)

#define CM_SUB_STATE_WRONG /* what the client assumes on receiving a plug-wrong-alarm */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_WRONG \
	)

#define CM_SUB_STATE_IS_OK(ss)				(((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_OK)
#define CM_SUB_STATE_IS_SUBSTITUE(ss)		(((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_SUBSTITUTE)
#define CM_SUB_STATE_IS_WRONG(ss)			(((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_WRONG)
#define CM_SUB_STATE_IS_NO_SUBMODULE(ss)	(((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_NO_SUBMODULE)

#define CM_SUB_STATE_IS_OWNED(ss)			(((ss) & CM_SUB_STATE_AR_INFO_MASK) <= CM_SUB_STATE_AR_INFO_APPL_READY_PENDING)

#define CM_SUB_STATE_GET_AR_INFO(ss)			((ss) & CM_SUB_STATE_AR_INFO_MASK)
#define CM_SUB_STATE_SET_AR_INFO(ss, ar_info)	{ ss = (((ss) & ~CM_SUB_STATE_AR_INFO_MASK) | ((ar_info) & CM_SUB_STATE_AR_INFO_MASK)); }

#define CM_SUB_STATE_SET_IDENT(ss, ident)		{ ss = (((ss) & ~CM_SUB_STATE_IDENT_MASK) | ((ident) & CM_SUB_STATE_IDENT_MASK)); }

#define CM_SUB_STATE_SET_DIAG_INFO(ss, diag_info) {\
	ss = (((ss) & ~CM_SUB_STATE_DIAG_INFO_MASK) | ((diag_info) & CM_SUB_STATE_DIAG_INFO_MASK));\
	}

/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_CL_AR_OFFLINE_IND
// RESPONSE:	CM_OPC_CL_AR_OFFLINE_RSP
// INDICATION:	CM_OPC_SV_AR_DISCONNECT_IND
// RESPONSE:	CM_OPC_SV_AR_DISCONNECT_RSP
//---------------------------------------------------------------------------*/

enum cm_ar_reason_enum {
	/*
	 * see PNIO-Spec, chapter "Values of ErrorCode1 and ErrorCode2 for ErrorDecode with the value PNIO"
	 *   ErrorCode / ErrorDecode / ErrorCode1 / ErrorCode2
	 *   0xCF        0x81          0xFD         CM_AR_REASON_...
	 */
	CM_AR_REASON_NONE      =  0,
	/***/
	CM_AR_REASON_1         =  1, /* 0x01: sequence numbers do not match (no longer used in versions >= V3.9)*/
	CM_AR_REASON_2         =  2, /* 0x02: alarm instance closed (no longer used in versions >= V3.6) */
	CM_AR_REASON_MEM       =  3, /* 0x03: out of mem */
	CM_AR_REASON_FRAME     =  4, /* 0x04: add provider or consumer failed */
	CM_AR_REASON_MISS      =  5, /* 0x05: miss (consumer) */
	CM_AR_REASON_TIMER     =  6, /* 0x06: cmi timeout */
	CM_AR_REASON_ALARM     =  7, /* 0x07: alarm-open failed */
	CM_AR_REASON_ALSND     =  8, /* 0x08: alarm-send.cnf(-) */
	CM_AR_REASON_ALACK     =  9, /* 0x09: alarm-ack-send.cnf(-) */
	CM_AR_REASON_ALLEN     = 10, /* 0x0A: alarm-data too long */
	CM_AR_REASON_ASRT      = 11, /* 0x0B: alarm.ind(err) */
	CM_AR_REASON_RPC       = 12, /* 0x0C: rpc-client call.cnf(-) */
	CM_AR_REASON_ABORT     = 13, /* 0x0D: ar-abort.req */
	CM_AR_REASON_RERUN     = 14, /* 0x0E: rerun aborts existing */
	CM_AR_REASON_REL       = 15, /* 0x0F: got release.ind */
	CM_AR_REASON_PAS       = 16, /* 0x10: device passivated */
	CM_AR_REASON_RMV       = 17, /* 0x11: device / AR removed */
	CM_AR_REASON_PROT      = 18, /* 0x12: protocol violation */
	CM_AR_REASON_NARE      = 19, /* 0x13: NARE error */
	CM_AR_REASON_BIND      = 20, /* 0x14: RPC-Bind error */
	CM_AR_REASON_CONNECT   = 21, /* 0x15: RPC-Connect error */
	CM_AR_REASON_READ      = 22, /* 0x16: RPC-Read error */
	CM_AR_REASON_WRITE     = 23, /* 0x17: RPC-Write error */
	CM_AR_REASON_CONTROL   = 24, /* 0x18: RPC-Control error */
	CM_AR_REASON_25        = 25, /* 0x19: reserved (formerly: pull or plug in forbidden window) */
	CM_AR_REASON_26        = 26, /* 0x1A: reserved (formerly: AP removed) */
	CM_AR_REASON_LNK_DOWN  = 27, /* 0x1B: link "down", for local purpose only */
	CM_AR_REASON_MMAC      = 28, /* 0x1C: could not register multicast-mac */
	CM_AR_REASON_SYNC      = 29, /* 0x1D: not synchronized (cannot start companion-ar) */
	CM_AR_REASON_TOPO      = 30, /* 0x1E: wrong topology (cannot start companion-ar) */
	CM_AR_REASON_DCP_NAME  = 31, /* 0x1F: dcp, station-name changed */
	CM_AR_REASON_DCP_RESET = 32, /* 0x20: dcp, reset to factory or factory reset */
	CM_AR_REASON_33        = 33, /* 0x21: reserved (formerly: cannot start companion-ar) */
	CM_AR_REASON_IRDATA    = 34, /* 0x22: no irdata record yet */
	CM_AR_REASON_PDEV      = 35, /* 0x23: ownership of physical device */
	CM_AR_REASON_LNK_MODE  = 36, /* 0x24: link mode not full duplex */
	CM_AR_REASON_IPSUITE   = 37, /* 0x25: IP-Suite [of the IOC] changed by means of DCP_set(IPParameter) or local engineering */
	CM_AR_REASON_RDHT      = 38, /* 0x26: IOCARSR RDHT expired */
	CM_AR_REASON_PDEV_PRM  = 39, /* 0x27: IOCARSR PDev, parametrization impossible */
	CM_AR_REASON_ARDY      = 40, /* 0x28: Remote application timeout expired */
	/***/
	CM_AR_REASON_MAX       = 255/*!*/
};

typedef struct cm_ar_abort_offline_tag {

	LSA_UINT32 reason_code; /* see cm_ar_reason_enum */
	LSA_UINT32 reason_pnio_status; /* see documentation */

} CM_AR_ABORT_OFFLINE_TYPE;

typedef CM_AR_ABORT_OFFLINE_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_AR_ABORT_OFFLINE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// common types, used by CM-PD and CM-CL
//---------------------------------------------------------------------------*/

typedef struct cm_pd_diff_submodule_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag)
					link;			/* link to next entry */
	LSA_UINT16		subslot_nr;		/* ind: subslot_nr */
	LSA_UINT32		sub_ident;		/* ind: real submodule-ident number */
	LSA_UINT16		sub_state;		/* ind: see cm_sub_state_enum */
} CM_PD_DIFF_SUBMODULE_TYPE;

typedef struct cm_pd_diff_module_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag)
					link;			/* link to next entry */
	LSA_UINT16		slot_nr;		/* ind: slot_nr */
	LSA_UINT32		mod_ident;		/* ind: real module-ident number */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag)
					submodule_list;	/* list of type: CM_PD_DIFF_SUBMODULE_TYPE */
} CM_PD_DIFF_MODULE_TYPE;

typedef struct cm_pd_diff_ap_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)
					link;			/* link to next entry */
	LSA_UINT32		api;			/* ind: application process identifier */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag)
					module_list;	/* list of type: CM_PD_DIFF_MODULE_TYPE */
} CM_PD_DIFF_AP_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_PD_EVENT_APPL_READY_IND
// RESPONSE:   CM_OPC_PD_EVENT_APPL_READY_RSP
//---------------------------------------------------------------------------*/

typedef CM_PD_DIFF_SUBMODULE_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE;

typedef CM_PD_DIFF_MODULE_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_PD_DIFF_MODULE_PTR_TYPE;

typedef CM_PD_DIFF_AP_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_PD_DIFF_AP_PTR_TYPE;


typedef struct cm_pd_appl_ready_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  diff_list; /* list of type: CM_PD_DIFF_AP_TYPE */

	LSA_UINT32  faulty_record_index; /* ind: if CM_ERR_PRM_CONSISTENCY: first faulty record index */

	LSA_UINT16  mod_diff_block_length;  /* AP00679788 */
	CM_UPPER_MEM_PTR_TYPE  mod_diff_block;

	LSA_UINT16  diag_block_length;  /* AP01427283 */
	CM_UPPER_MEM_PTR_TYPE  diag_block;

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red; /* see EDD documentation, EDD_RESERVED_INTERVAL_IF_RED_TYPE (AP00964333 and AP01317720) */

} CM_PD_APPL_READY_TYPE;

typedef CM_PD_APPL_READY_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_PD_APPL_READY_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* needed by CMMC */
enum cm_addr_info_class_enum
{
	CM_ADDR_INFO_CLASS_OK,		/* device_ip, device_mac */
	CM_ADDR_INFO_CLASS_LINK,	/* link_speed not as in ar_properties */
	CM_ADDR_INFO_CLASS_COMPANION,/* first of a companion has diag in submodule, lower_rsp contains subslot_nr */
	CM_ADDR_INFO_CLASS_NARE,	/* lower_rsp: contains NARE_RSP_ERR_xxx */
								/* ev. device_ip, device_mac */
	CM_ADDR_INFO_CLASS_RPC,		/* pnio_status, ev. nca_status */
								/* ?lower_rsp: contains CLRPC_ERR_xxx?, */
	CM_ADDR_INFO_CLASS_MISS,	/* same as reason_code: CM_AR_REASON_MISS */
	CM_ADDR_INFO_CLASS_OTHER	/* reason_code: cm_ar_reason_enum */
};


/*------------------------------------------------------------------------------
// begin of CM-CL and CM-MC events
//----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_MC_EVENT_AR_ADDR_INFO_IND
// RESPONSE:   CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP
//---------------------------------------------------------------------------*/

enum cm_addr_info_phase_enum {
	CM_ADDR_INFO_PHASE_NARE,
	CM_ADDR_INFO_PHASE_CONNECT,
	CM_ADDR_INFO_PHASE_PLUG,
	CM_ADDR_INFO_PHASE_IN_DATA
};

typedef struct cm_mc_addr_info_tag {
	LSA_UINT8    info_class; /* see cm_addr_info_class_enum */
	LSA_UINT8    info_phase; /* see cm_addr_info_phase_enum */
	LSA_UINT16   reason_code; /* see cm_ar_reason_enum */
	LSA_UINT16   lower_rsp; /* see NARE_RSP_xxx, CLRPC_ERR_xxx, subslot_nr */
	LSA_UINT32   pnio_status; /* pnio error status, see PNIO-Spec */
	LSA_UINT32   nca_status; /* CLRPC nca error */
	CM_MAC_TYPE  device_mac; /* ethernet mac-addr of device or error-mac */
	LSA_UINT32   device_ip; /* ip-addr of device or error-ip */
	LSA_UINT32   device_subnet_mask; /* if ok: subnet-mask of device */
	LSA_UINT32   device_gateway_ip; /* if ok: ip-addr of the gateway used by the device */
} CM_MC_ADDR_INFO_TYPE;

typedef CM_MC_ADDR_INFO_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_MC_ADDR_INFO_PTR_TYPE;

#endif


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_CL_AR_ABORT_IND
// RESPONSE:   CM_OPC_CL_AR_ABORT_RSP
// INDICATION: CM_OPC_CL_AR_PRE_OFFLINE_IND
// RESPONSE:   CM_OPC_CL_AR_PRE_OFFLINE_RSP
// INDICATION: CM_OPC_CL_AR_OFFLINE_IND
// RESPONSE:   CM_OPC_CL_AR_OFFLINE_RSP
//---------------------------------------------------------------------------*/

#define CM_ERROR_CAUSE_NONE					0	/* No problem, everything OK */
#define CM_ERROR_CAUSE_IP_MULTIPLE			1	/* IP address exists multiple times */
#define CM_ERROR_CAUSE_NOS_MULTIPLE			2	/* NameOfStation exists multiple times */
#define CM_ERROR_CAUSE_LINK_DOWN			3	/* Link down (no MAC link, no IP suite) */
#define CM_ERROR_CAUSE_MAU_MISMATCH			4	/* MAUType mismatch */
#define CM_ERROR_CAUSE_RPC_ERROR			5	/* RPC protocol error */
#define CM_ERROR_CAUSE_PNIO_CONFIGURATION	6	/* PNIO configuration protocol error */
#define CM_ERROR_CAUSE_RPC_TIMEOUT			7	/* RPC application timeout */
#define CM_ERROR_CAUSE_IP_SET_ABORTED		8	/* IP address assignment to IO device aborte */
#define CM_ERROR_CAUSE_IOD_NOT_FOUND		9	/* IO device not found */
#define CM_ERROR_CAUSE_OUT_OF_RESOURCES		10	/* Out of AR resources */
#define CM_ERROR_CAUSE_MISS					11	/* WDT / DHT expired */
#define CM_ERROR_CAUSE_NOS_SET_ABORTED		12	/* Device with wrong NameOfStation found on topological position */
#define CM_ERROR_CAUSE_IOC_ABORTED			13	/* Controller local abort reason */
#define CM_ERROR_CAUSE_IOD_ABORTED			14	/* Device abort reason */
#define CM_ERROR_CAUSE_SRD_ABORTED			15	/* SystemRedundancy abort reason */
#define CM_ERROR_CAUSE_VENDORID_ABORTED		16	/* Device with wrong VendorId/DeviceId/Instance found on topological position */

#define CM_ERROR_CAUSE_MP_IN_PROGRESS		17	/* MasterProject: Machine- or AddressTailoring in progress */
#define CM_ERROR_CAUSE_MP_NOS_MISSING		18	/* MasterProject: NameOfStation adaption missing (PDev with invalid NoS) */
#define CM_ERROR_CAUSE_MP_IPv4_MISSING		19	/* MasterProject: IPv4Suite adaption missing (PDev with invalid IPv4Suite) */
#define CM_ERROR_CAUSE_MP_NOS_INVALID		20	/* MasterProject: tailored NameOfStation|PTCPSubdomainName|MRPDomainName is invalid */
#define CM_ERROR_CAUSE_MP_IPv4_INVALID		21	/* MasterProject: tailored IPv4Suite of IOD is invalid */
#define CM_ERROR_CAUSE_MP_TAILORING_MISSING	22	/* MasterProject: MachineTailoring missing */
#define CM_ERROR_CAUSE_MP_PDEV_ERROR		23	/* MasterProject: PDev configuration error */
#define CM_ERROR_CAUSE_MP_DEVICE_DISABLED	24	/* MasterProject: disabled Device is activated */
#define CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE	25	/* MasterProject: configuration incomplete (e.g. neighbour not exist because of device-remove) */

typedef struct cm_cl_error_cause_tag {

	LSA_UINT8		reason_code;			/* see cm_ar_reason_enum, see ERR-RTA-PDU, see ErrorCode2 for ErrorCode1 == 253 */
	LSA_UINT8		padding;

	LSA_UINT16		error_cause;			/* big-endian(!) contains error information, see CM_ERROR_CAUSE_xxx */
	LSA_UINT8		additional_info[10];	/* big-endian(!) contains error information, see "cm/doc/PNIO_CM_CL_Diagnosis_ErrorList.doc" */

} CM_CL_ERROR_CAUSE_TYPE;

typedef CM_CL_ERROR_CAUSE_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE;

typedef struct cm_cl_ar_abort_tag {

	CM_CL_ERROR_CAUSE_TYPE	error_cause;
	LSA_UINT32				device_ip;		/* known ip-address of device */

} CM_CL_AR_ABORT_TYPE;

typedef CM_CL_AR_ABORT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_AR_ABORT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_CL_AR_IN_DATA_IND
// RESPONSE:   CM_OPC_CL_AR_IN_DATA_RSP
// INDICATION: CM_OPC_CL_AR_PRE_DATA_IND
// RESPONSE:   CM_OPC_CL_AR_PRE_DATA_RSP
//---------------------------------------------------------------------------*/

typedef CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE;

typedef CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  CM_UPPER_CL_DIFF_MODULE_PTR_TYPE;

typedef CM_UPPER_PD_DIFF_AP_PTR_TYPE  CM_UPPER_CL_DIFF_AP_PTR_TYPE;

typedef struct cm_cl_ip_suite_tag {

	CM_MAC_TYPE  device_mac; /* ethernet mac-addr of device or error-mac */
	LSA_UINT32   device_ip; /* ip-addr of device or error-ip */
	LSA_UINT32   device_subnet_mask; /* subnet-mask of device */
	LSA_UINT32   device_gateway_ip; /* ip-addr of the gateway used by the device */

} CM_CL_IP_SUITE_TYPE;

typedef CM_CL_IP_SUITE_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_IP_SUITE_PTR_TYPE;


typedef struct cm_cl_ar_in_data_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  diff_list; /* list of type: CM_PD_DIFF_AP_TYPE */

	LSA_UINT16  mod_diff_block_length;  /* AP00679788 */
	CM_UPPER_MEM_PTR_TYPE  mod_diff_block;

	CM_CL_IP_SUITE_TYPE  ip_suite; /* values retrieved from name resolution */

} CM_CL_AR_IN_DATA_TYPE;

typedef CM_CL_AR_IN_DATA_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_AR_IN_DATA_PTR_TYPE;


#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_MC_EVENT_AR_IN_DATA_IND
// RESPONSE:   CM_OPC_MC_EVENT_AR_IN_DATA_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_mc_in_data_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  diff_list; /* list of type: CM_PD_DIFF_AP_TYPE */

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  good_list; /* list of type: CM_PD_DIFF_AP_TYPE */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_iocr_tag)  iocr_list; /* list of type: CM_ARCB_IOCR_TYPE */

	LSA_UINT16  mod_diff_block_length;  /* AP00679788 */
	CM_UPPER_MEM_PTR_TYPE  mod_diff_block;

} CM_MC_IN_DATA_TYPE;

typedef CM_MC_IN_DATA_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_MC_IN_DATA_PTR_TYPE;

#endif


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_CL_AR_PRM_BEGIN_IND
// RESPONSE:   CM_OPC_CL_AR_PRM_BEGIN_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_arcb_prm_submodule_tag
{
	LSA_UINT32		api;				/* api */
	LSA_UINT16		slot_nr;			/* slot number */
	LSA_UINT16		subslot_nr;			/* subslot number */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
					prm_record_list;	/* list of new prm-records for this submodule */

} CM_ARCB_PRM_SUBMODULE_TYPE;

typedef CM_ARCB_PRM_SUBMODULE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ARCB_PRM_SUBMODULE_PTR_TYPE;

typedef struct cm_cl_prm_begin_tag
{
	LSA_UINT16				prm_mode; /* see PrmUpdate.req */
	CM_ARCB_PRM_SUBMODULE_TYPE
							prm_submodule; /* valid with prm_mode = SUBMODULE */

} CM_CL_PRM_BEGIN_TYPE;

typedef CM_CL_PRM_BEGIN_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_PRM_BEGIN_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_CL_AR_PRM_INDATA_IND
// RESPONSE:   CM_OPC_CL_AR_PRM_INDATA_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_cl_prm_indata_tag
{
	LSA_UINT16				prm_mode; /* see PrmUpdate.req */
	CM_ARCB_PRM_SUBMODULE_TYPE
							prm_submodule; /* valid with prm_mode = SUBMODULE */

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)
							diff_list;

	LSA_UINT16				mod_diff_block_length;
	CM_UPPER_MEM_PTR_TYPE	mod_diff_block;

} CM_CL_PRM_INDATA_TYPE;

typedef CM_CL_PRM_INDATA_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_CL_PRM_INDATA_PTR_TYPE;


/*------------------------------------------------------------------------------
// end of CM-CL and CM-MC events
//----------------------------------------------------------------------------*/
#endif


/*-----------------------------------------------------------------------------
// begin of server events
//---------------------------------------------------------------------------*/
#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_SV_AR_CONNECT_IND
// RESPONSE:	CM_OPC_SV_AR_CONNECT_RSP
// INDICATION:	CM_OPC_SV_AR_DISCONNECT_IND (uses the same type as connect)
// RESPONSE:	CM_OPC_SV_AR_DISCONNECT_RSP (uses the same type as connect)
//---------------------------------------------------------------------------*/

#define CM_SV_SESSION_KEY_TO_AR_NR(session_key)  ((LSA_UINT16)((session_key) & 0xFF)) /* convert the session key to an AR number */

#define CM_SV_SESSION_KEY_TO_ARSET_NR(session_key)  ((LSA_UINT16)(((session_key) >> 8) & 0x03)) /* convert the session key to an AR-set number */

#define CM_SV_ARUUID_TO_SRARNR(aruuid)  ((LSA_UINT16)((aruuid).node[5] & 0x7)) /* extract the "SR-AR number" from ARUUID::zzzz */

#define CM_IO_BUFFER_SIZE_ALIGNED(x)          (((x) + 3) & ~3)
#define CM_IO_BUFFER_SIZE_ALIGNED_TRIPLED(x)  (CM_IO_BUFFER_SIZE_ALIGNED(x) * 3)

#define CM_SV_ARVENDORBLOCK_RES_MAXSIZE  (2+2+1+1 + 2+4 + 500) /* maximum size of the ARVendorBlockRes (FDIS 61784-2_V23_FINAL__TC2WG11_TO_PNO_CO__Rev15.doc) */

typedef struct cm_sv_ar_connect_iocr_tag {

	LSA_UINT16 iocr_ref;        /* ind: info, rsp: unchanged(!) */
	LSA_UINT16 frame_id;        /* ind: info */

	LSA_UINT16 iocr_type;       /* ind: see cm_iocr_type_enum */
	LSA_UINT32 iocr_properties; /* ind: see cm_iocr_properties_enum */

	LSA_USER_ID_TYPE iocr_user_id; /* ind: zero, rsp: user-defined value; see data_user_id and iocs_user_id in CM_OPC_SV_AR_OWNERSHIP_IND */

	LSA_UINT16 reduction_ratio; /* ind: info (V5.2i4, TIA 216515) */
	LSA_UINT16 cycle_phase;     /* ind: info, rsp: unchanged or user-defined for usecase AP00856570 */

	LSA_UINT16 io_buffer_size; /* IOCRBlockReq::DataLength or less if the HW can "partial data" and/or "autopadding" */

	/*
	 * note that the "io_buffer_size" may not be the same as the alloc-size.
	 * depending on the HW it has to be incremented by apdu_status_length and/or aligned and/or tripled.
	 *
	 * see CM_IO_BUFFER_SIZE_ALIGNED() and CM_IO_BUFFER_SIZE_ALIGNED_TRIPLED()
	 */

	LSA_UINT16 eddx_apdu_status_length; /* ind: 0 or 4 if EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE */
	LSA_UINT32 eddx_data_status_offset; /* ind: see EDD_CSRT_PROV_IO_PARAMS_TYPE::DataStatusOffset, EDD_DATAOFFSET_INVALID if the APDUStatus _is_ part of the io_buffer */
	LSA_UINT32 eddx_data_offset;        /* rsp: offset to the "io_buffer" with size "io_buffer_size" + adjustments, see EDD_CSRT_PROV_IO_PARAMS_TYPE::DataOffset and EDD_CSRT_CONS_IO_PARAMS_TYPE::DataOffset */

	LSA_UINT32 eddi_cw_offset;          /* ind: see EDD_CSRT_PROV_IO_PARAMS_TYPE::CWOffset */
	LSA_UINT32 eddi_apdu_status_offset; /* ind: see EDD_CSRT_CONS_IO_PARAMS_TYPE::APDUStatusOffset */
	LSA_UINT32 eddi_buffer_properties;  /* rsp: EDD_CSRT_PROV_IO_PARAMS_TYPE::BufferProperties and EDD_CSRT_CONS_IO_PARAMS_TYPE::BufferProperties */

	LSA_UINT16 eddx_cr_number;          /* rsp: see EDD_CSRT_PROV_IO_PARAMS_TYPE::CRNumber and EDD_CSRT_CONS_IO_PARAMS_TYPE::CRNumber */
	                                    /*      EDDS see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254 */
	LSA_UINT8  eddp_forward_mode;       /* rsp: see EDD_CSRT_CONS_IO_PARAMS_TYPE::ForwardMode */
	LSA_UINT32 eddp_ct_outdatastart;    /* rsp: see EDD_CSRT_CONS_IO_PARAMS_TYPE::CutThrough_OutDataStart */

} CM_SV_AR_CONNECT_IOCR_TYPE;

typedef CM_SV_AR_CONNECT_IOCR_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_CONNECT_IOCR_PTR_TYPE;


typedef struct cm_sv_ar_connect_tag {

	/* caller information */

	LSA_UINT32 host_ip; /* network byte order, ip-address of IOC */

	CM_COMMON_MEM_U8_PTR_TYPE cmi_station_name; /* zero-terminated string */

	CLRPC_UUID_TYPE cmi_obj_uuid; /* CMInitiatorObjectUUID */

	CLRPC_UUID_TYPE ar_uuid; /* ARUUID */

	/* AR information */

	LSA_UINT16 alarm_send_max_length; /* = AlarmCRBlockReq::MaxAlarmDataLength - CM_ALARM_OFFSET_DATA */

	LSA_UINT16 ar_type; /* see cm_ar_type_enum */

	LSA_UINT32 ar_properties; /* see cm_ar_properties_enum */

	LSA_BOOL is_iosar_with_device_access; /* true .. the AR is an CM_AR_TYPE_SUPERVISOR with CM_AR_PROP_DEVICE_ACCESS_YES */

	/* AR information for FSU (fast startup) */

	CM_COMMON_MEM_U8_PTR_TYPE ARFSUBlock_ptr; /* LSA_NULL or pointer to the ARFSUBlock (V5.0i5, AP01034170) */

	/* AR information for CM_AR_TYPE_SINGLE_SYSRED (system redundancy) */

	LSA_UINT32 sr_rdht_msec; /* redundancy data hold time, in milliseconds */

	LSA_UINT32 sr_properties; /* see CM_SR_PROP_INP_VALID_ON_BACKUP_AR, etc. */

	/* AR vendor-specific information for CM_AR_PROP_STARTUP_MODE_ADVANCED */

	LSA_UINT32 ARVendorBlock_cnt; /* 0 or number of ARVendorBlockReq/ARVendorBlockRes */

	LSA_UINT32 CM_UPPER_MEM_ATTR * ARVendorBlock_ptr; /* LSA_NULL or pointer to array of ARVendorBlockReq/ARVendorBlockRes, see documentation (V5.2i3, AP01354956) */

	/***/

	struct { /* only for CM_OPC_SV_AR_DISCONNECT_IND */

		LSA_UINT32 reason_code; /* see cm_ar_reason_enum */

		LSA_UINT32 reason_pnio_status; /* for AP00678183 only. if reason_code == CM_AR_REASON_ASRT this field contains the pnio_status from the asrt-error packet */

		LSA_BOOL was_in_data; /* LSA_TRUE if the AR was aborted after in-data */

	} disconnect;

	/* IOCR information */

	LSA_UINT16 send_clock_31250ns; /* note: all IOCRs have the same send-clock, 0 if nr_of_iocrs == 0, (V5.2i4, TIA 216515) */

	LSA_UINT16 nr_of_iocrs; /* number of array elements (0 or 2..n) */

	CM_SV_AR_CONNECT_IOCR_TYPE iocr[1];

	/* iocr[2..n] follow here */

} CM_SV_AR_CONNECT_TYPE;

typedef CM_SV_AR_CONNECT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_CONNECT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_SV_AR_OWNERSHIP_IND
// RESPONSE:	CM_OPC_SV_AR_OWNERSHIP_RSP
//---------------------------------------------------------------------------*/

#define CM_IO_BUFFER_INP_DATA_LENGTH(elem_ptr)  ((LSA_UINT16)( ((elem_ptr)->sub_properties & CM_SUB_PROP_REDUCE_INP_LEN_MASK) == CM_SUB_PROP_REDUCE_INP_LEN_YES ? 0 : (elem_ptr)->descr[0/*inp*/].data_length ))
#define CM_IO_BUFFER_OUT_DATA_LENGTH(elem_ptr)  ((LSA_UINT16)( ((elem_ptr)->sub_properties & CM_SUB_PROP_REDUCE_OUT_LEN_MASK) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ? 0 : (elem_ptr)->descr[1/*out*/].data_length ))

#define CM_IO_BUFFER_INP_IOPS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[0/*inp*/].iops_length ))
#define CM_IO_BUFFER_OUT_IOPS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[1/*out*/].iops_length ))

#define CM_IO_BUFFER_INP_IOCS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[0/*inp*/].iocs_length ))
#define CM_IO_BUFFER_OUT_IOCS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[1/*out*/].iocs_length ))


typedef struct cm_sv_ar_ownership_element_descr_tag {

	/*
	 * length of IOData in the "io_buffer" see CM_IO_BUFFER_INP_DATA_LENGTH() and CM_IO_BUFFER_OUT_DATA_LENGTH()
	 * length of IOPS   in the "io_buffer" see CM_IO_BUFFER_INP_IOPS_LENGTH() and CM_IO_BUFFER_OUT_IOPS_LENGTH()
	 * length of IOCS   in the "io_buffer" see CM_IO_BUFFER_INP_IOCS_LENGTH() and CM_IO_BUFFER_OUT_IOCS_LENGTH()
	 */

	LSA_UINT16 data_description; /* see cm_sub_data_description_enum, note that CM_SUB_DATA_DESCR_TYPE_MASK is consistent with CM_SUB_PROP_TYPE_MASK */

	LSA_USER_ID_TYPE data_user_id; /* the "io_buffer" containing IOData and IOPS, see CM_SV_AR_CONNECT_IOCR_TYPE::iocr_user_id */
	LSA_UINT16       data_offset;  /* offset of IOData */
	LSA_UINT16       data_length;  /* expected length of IOData */
	LSA_UINT8        iops_length;  /* expected length of IOPS (following IOData) */

	LSA_USER_ID_TYPE iocs_user_id; /* the "io_buffer" containing IOCS, see CM_SV_AR_CONNECT_IOCR_TYPE::iocr_user_id */
	LSA_UINT16       iocs_offset;  /* offset of IOCS */
	LSA_UINT8        iocs_length;  /* expected length of IOCS */

} CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE;

typedef CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_DESCR_PTR_TYPE;


typedef struct cm_sv_ar_ownership_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	/***/

	LSA_UINT32 mod_ident;      /* module ident number */
	LSA_UINT16 mod_properties; /* see cm_mod_properties_enum */

	LSA_UINT32 sub_ident;      /* submodule ident number */
	LSA_UINT16 sub_properties; /* see cm_sub_properties_enum */

	CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE descr[2]; /* index 0=input 1=output */

	/***/

	LSA_BOOL is_wrong; /* ind: see documentation (AP00881547), rsp: true if the real submodule is "not a substitute" */

	LSA_UINT16 owner_session_key; /* ind: the owner, rsp: 0 or unchanged */

} CM_SV_AR_OWNERSHIP_ELEMENT_TYPE;

typedef CM_SV_AR_OWNERSHIP_ELEMENT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE;


#define CM_SV_AR_CONTEXT_CONN  1 /* indication context is 'connect' */
#define CM_SV_AR_CONTEXT_DISC  2 /* indication context is 'disconnect' */
#define CM_SV_AR_CONTEXT_PLUG  3 /* indication context is 'plug' */

typedef struct cm_sv_ar_ownership_tag {

	LSA_UINT16 ar_context; /* see CM_SV_AR_CONTEXT_CONN, ... */

	LSA_BOOL set_mrp_off;
		/* ind: always LSA_FALSE
		 * rsp: if LSA_TRUE and no other MRP record was written then CM-PD generates an "MRP off" record (AP00824058)
		 * evaluated only if ar_context == CM_SV_AR_CONTEXT_CONN
		 */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_AR_OWNERSHIP_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_AR_OWNERSHIP_TYPE;

typedef CM_SV_AR_OWNERSHIP_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_SV_AR_PRM_END_IND
// RESPONSE:	CM_OPC_SV_AR_PRM_END_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_prm_end_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	/***/

	LSA_BOOL appl_ready_follows; /* ind: LSA_FALSE, rsp: see usecase below */
	LSA_BOOL appl_ready_pending; /* ind: LSA_TRUE,  rsp: see usecase below */

	/*
	 * Usecase "tell status now"
	 *   appl_ready_follows = LSA_FALSE;
	 *   appl_ready_pending = is-not-ready ? LSA_TRUE : LSA_FALSE;
	 *
	 * Usecase "tell status later with CM_OPC_SV_AR_APPL_READY"
	 *   appl_ready_follows = LSA_TRUE;
	 *   appl_ready_pending = LSA_TRUE; // don't care
	 */

} CM_SV_AR_PRM_END_ELEMENT_TYPE;

typedef CM_SV_AR_PRM_END_ELEMENT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_PRM_END_ELEMENT_PTR_TYPE;


typedef struct cm_sv_ar_prm_end_tag {

	LSA_UINT16 ar_context; /* see CM_SV_AR_CONTEXT_CONN, ... */

	LSA_UINT16 nr_of_elems; /* number of array elements (0..n) */

	CM_SV_AR_PRM_END_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_AR_PRM_END_TYPE;

typedef CM_SV_AR_PRM_END_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_PRM_END_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_SV_AR_IN_DATA_IND
// RESPONSE:	CM_OPC_SV_AR_IN_DATA_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_in_data_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

} CM_SV_AR_IN_DATA_ELEMENT_TYPE;

typedef CM_SV_AR_IN_DATA_ELEMENT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_IN_DATA_ELEMENT_PTR_TYPE;


typedef struct cm_sv_ar_in_data_tag {

	LSA_UINT16 nr_of_elems; /* number of array elements (0..n) */

	CM_SV_AR_IN_DATA_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_AR_IN_DATA_TYPE;

typedef CM_SV_AR_IN_DATA_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_IN_DATA_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION:	CM_OPC_SV_AR_RIR_IND
// RESPONSE:	CM_OPC_SV_AR_RIR_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_rir_element_tag {

	LSA_USER_ID_TYPE iocr_user_id; /* see CM_OPC_SV_AR_CONNECT_IND */

} CM_SV_AR_RIR_ELEMENT_TYPE;

typedef CM_SV_AR_RIR_ELEMENT_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_RIR_ELEMENT_PTR_TYPE;

typedef struct cm_sv_ar_rir_tag {

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red; /* see EDD documentation, EDD_RESERVED_INTERVAL_IF_RED_TYPE (AP00964333 and AP01317720) */

	LSA_UINT16 nr_of_elems; /* number of array elements (2) */

	CM_SV_AR_RIR_ELEMENT_TYPE elem[2];

} CM_SV_AR_RIR_TYPE;

typedef CM_SV_AR_RIR_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_RIR_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_SV_AR_DATA_STATUS_IND
// RESPONSE:   CM_OPC_SV_AR_DATA_STATUS_RSP
//---------------------------------------------------------------------------*/

#define CM_DATA_STATUS_INIT       (CM_DATA_STATUS_PRIMARY | CM_DATA_STATUS_RUN | CM_DATA_STATUS_OK) /* initial value */

#define CM_DATA_STATUS_PRIMARY    EDD_CSRT_DSTAT_BIT_STATE /* 0=backup 1=primary */
#define CM_DATA_STATUS_SYSRED_NO  EDD_CSRT_DSTAT_BIT_REDUNDANCY /* 0=primary-AR-present, 1=no-primary-AR-present */
#define CM_DATA_STATUS_BIT2       0x04 /* always reported as 0 */
#define CM_DATA_STATUS_BIT3       0x08 /* always reported as 0 */
#define CM_DATA_STATUS_RUN        EDD_CSRT_DSTAT_BIT_STOP_RUN /* 0=stop 1=run */
#define CM_DATA_STATUS_OK         EDD_CSRT_DSTAT_BIT_STATION_FAILURE /* 0=failure 1=ok */
#define CM_DATA_STATUS_BIT6       0x40 /* always reported as 0 */
#define CM_DATA_STATUS_BIT7       0x80 /* always reported as 0 */

typedef struct cm_sv_ar_data_status_tag {

	LSA_UINT8 curr; /* ind: current value, see CM_DATA_STATUS_PRIMARY, ... */
	LSA_UINT8 prev; /* ind: previous value */

} CM_SV_AR_DATA_STATUS_TYPE;

typedef CM_SV_AR_DATA_STATUS_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_DATA_STATUS_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_SV_AR_MCONS_IND
// RESPONSE:   CM_OPC_SV_AR_MCONS_RSP
//---------------------------------------------------------------------------*/
#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE

#define CM_SV_MCONS_EVENT_ADDR_INFO    1
#define CM_SV_MCONS_EVENT_IN_DATA      2
#define CM_SV_MCONS_EVENT_OFFLINE      3
#define CM_SV_MCONS_EVENT_DATA_STATUS  4

typedef struct cm_sv_ar_mcons_tag {

	LSA_UINT16 cr_ref; /* cr-reference */

	LSA_UINT16 mc_event; /* see CM_SV_MCONS_EVENT_ADDR_INFO, ... */

	union {

		struct {

			LSA_UINT16 info_class; /* CM_ADDR_INFO_CLASS_NARE, CM_ADDR_INFO_CLASS_MISS or CM_ADDR_INFO_CLASS_OK */
			LSA_UINT16 nare_rsp;   /* see NARE_RSP_xxx */

			CM_MAC_TYPE source_mac; /* ethernet mac-addr or error-mac */
			LSA_UINT32  source_ip;  /* ip-addr or error-ip */

		} addr_info;

		struct {

			LSA_UINT8 nothing_so_far;

		} in_data;

		struct {

			LSA_UINT8 nothing_so_far;

		} offline;

		struct {

			LSA_UINT8 curr; /* ind: current value, see CM_DATA_STATUS_PRIMARY, ... */
			LSA_UINT8 prev; /* ind: previous value */

		} data_status;

	} u;

} CM_SV_AR_MCONS_TYPE;

typedef CM_SV_AR_MCONS_TYPE  CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_MCONS_PTR_TYPE;

#endif

/*-----------------------------------------------------------------------------
// end of server events
//---------------------------------------------------------------------------*/
#endif


/*------------------------------------------------------------------------------
// OPCODE:     CM_OPC_CL_RECORD_READ
// OPCODE:     CM_OPC_CL_RECORD_READ_QUERY
// OPCODE:     CM_OPC_CL_RECORD_READ_IMPLICIT
// OPCODE:     CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY
// OPCODE:     CM_OPC_CL_RECORD_WRITE
// OPCODE:     CM_OPC_CL_RECORD_WRITE_MULTIPLE
// INDICATION: CM_OPC_SV_AR_READ_IND
// RESPONSE:   CM_OPC_SV_AR_READ_RSP
// INDICATION: CM_OPC_SV_AR_WRITE_IND
// RESPONSE:   CM_OPC_SV_AR_WRITE_RSP
//----------------------------------------------------------------------------*/

#define CM_RECORD_HEADER_SIZE  (64U)
#define CM_RECORD_OFFSET       (CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE)

#define CM_RECORD_READ_NOT_HANDLED(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_READ, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xA0, /* application, read error */ \
		(code2) \
		))

#define CM_RECORD_READ_NO_IODATA(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_READ, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xB0, /* invalid index, see AP00383155 */ \
		(code2) \
		))

#define CM_RECORD_WRITE_NOT_HANDLED(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_WRITE, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xA1, /* application, write error */ \
		(code2) \
		))

#define CM_RECORD_WRITE_BACKUP(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_WRITE, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xBA, /* access, backup (usecase: system redundancy) */ \
		(code2) \
		))


typedef struct cm_record_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_record_tag) link; /* only CM_OPC_CL_RECORD_WRITE_MULTIPLE */

	LSA_UINT32  api;           /* req: record addressing */
	LSA_UINT16  slot_nr;       /* req: -"- */
	LSA_UINT16  subslot_nr;    /* req: -"- */
	LSA_UINT16  record_index;  /* req: -"- */

	LSA_UINT16  sequence_nr;   /* req: sequencing (common for read and write!)*/

	LSA_UINT32  data_length;
		/*
		// rd-req: size of 'data' (including CM_RECORD_OFFSET)
		// rd-cnf: CM_RECORD_OFFSET + number of bytes read
		// wr-req: CM_RECORD_OFFSET + number of bytes to write
		// wr-cnf: CM_RECORD_OFFSET + number of bytes written
		*/

	CM_COMMON_MEM_U8_PTR_TYPE  data;
		/*
		// the CLRPC buffer, user-data starts at offset CM_RECORD_OFFSET
		// the headers are not hidden to "see" the alignment of the user-data
		//
		// rd-req: "empty" buffer
		// rd-cnf: read data stored at offset CM_RECORD_OFFSET
		// wr-req: data-to-write at offset CM_RECORD_OFFSET
		// wr-cnf: "unchanged"
		*/

	LSA_UINT32  cm_pnio_err;
		/*
		// cnf: see the macro CM_PNIO_ERR_MAKE() in file cm_err.h
		//
		// for indices not handled by the CM-user the error codes
		// defined by the following macros shall be returned:
		//
		// CM_RECORD_READ_NOT_HANDLED(code2)
		// CM_RECORD_WRITE_NOT_HANDLED(code2)
		//
		// the value of the 'code2' is user-specific
		 */

	LSA_UINT16  ret_val_1;  /* cnf: return value 1 */
	LSA_UINT16  ret_val_2;  /* cnf: return value 2 */

	LSA_UINT32  readquery_length;
		/*
		// rd-req: size of 'RecordDataReadQuery' (see PNIO-Spec) at offset CM_RECORD_OFFSET
		// rd-cnf: don't care
		// wr-req: don't evaluate
		// wr-cnf: don't care
		*/

	LSA_BOOL is_iosar_with_device_access; /* only for CM_OPC_SV_AR_WRITE_IND */
		/*
		// wr-ind: true .. the AR is an CM_AR_TYPE_SUPERVISOR with CM_AR_PROP_DEVICE_ACCESS_YES
		// else: don't evaluate
		*/

} CM_RECORD_TYPE;

typedef CM_RECORD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CM_RECORD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// INDICATION: CM_OPC_CL_AR_DATA_STATUS_IND
// RESPONSE:   CM_OPC_CL_AR_DATA_STATUS_RSP
// INDICATION: CM_OPC_MC_EVENT_APDU_STATUS_IND
// RESPONSE:   CM_OPC_MC_EVENT_APDU_STATUS_RSP
//---------------------------------------------------------------------------*/

enum cm_event_apdu_status_enum {
	CM_EVENT_APDU_STATUS_STOP            = 0x0001,
	CM_EVENT_APDU_STATUS_RUN             = 0x0002,
	CM_EVENT_APDU_STATUS_STATION_OK      = 0x0004,
	CM_EVENT_APDU_STATUS_STATION_PROBLEM = 0x0008,
	CM_EVENT_APDU_STATUS_PRIMARY         = 0x0010,
	CM_EVENT_APDU_STATUS_BACKUP          = 0x0020,
	CM_EVENT_APDU_STATUS_SYSRED_OK       = 0x0040,	/* the sysred "ar-set" has a primary ar */
	CM_EVENT_APDU_STATUS_SYSRED_PROBLEM  = 0x0080,
	/***/
	CM_EVENT_APDU_STATUS_INITIAL_VALUE        = (CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_PRIMARY | CM_EVENT_APDU_STATUS_SYSRED_OK),
	CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE = (CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_BACKUP  | CM_EVENT_APDU_STATUS_SYSRED_OK)
};


/*-----------------------------------------------------------------------------
// OPCODE:     CM_OPC_EVENT_IND_RES_PROVIDE
// INDICATION: CM_OPC_PD_EVENT_*_IND
// RESPONSE:   CM_OPC_PD_EVENT_*_RSP
//
// OPCODE:     CM_OPC_CL_DEVICE_PROVIDE_EVENT
// INDICATION: CM_OPC_CL_AR_*_IND
// RESPONSE:   CM_OPC_CL_AR_*_RSP
//
// OPCODE:     CM_OPC_SV_DEVICE_PROVIDE_EVENT
// INDICATION: CM_OPC_SV_AR_*_IND
// RESPONSE:   CM_OPC_SV_AR_*_RSP
//---------------------------------------------------------------------------*/

typedef struct cm_event_tag {

	LSA_UINT16 device_nr;   /* device number */
	LSA_UINT16 ar_nr;       /* ar number */
	LSA_UINT16 session_key; /* session key, see CM_OPC_CL_AR_OFFLINE_IND or CM_OPC_SV_AR_CONNECT_IND */

	union { /* pdev or client or server or multicast */

		union {
				/* CM_OPC_PD_EVENT_APPL_READY_IND / _RSP */
			CM_PD_APPL_READY_TYPE appl_ready;

		} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		union {
				/* CM_OPC_CL_AR_ABORT_IND / _RSP */
				/* CM_OPC_CL_AR_PRE_OFFLINE_IND / _RSP */
				/* CM_OPC_CL_AR_OFFLINE_IND / _RSP */
			CM_CL_AR_ABORT_TYPE  ar_abort;

				/* CM_OPC_CL_AR_PRE_DATA_IND / _RSP */
				/* CM_OPC_CL_AR_IN_DATA_IND / _RSP */
			CM_CL_AR_IN_DATA_TYPE in_data;

				/* CM_OPC_CL_AR_PRM_BEGIN_IND / _RSP */
			CM_CL_PRM_BEGIN_TYPE prm_begin;

				/* CM_OPC_CL_AR_PRM_INDATA_IND / _RSP */
			CM_CL_PRM_INDATA_TYPE prm_indata;

				/* CM_OPC_CL_AR_DATA_STATUS_IND / _RSP */
			LSA_UINT16 data_status; /* ind: see cm_event_apdu_status_enum */

		} cl;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		union {
				/* CM_OPC_SV_AR_CONNECT_IND / _RSP */
			CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect;

				/* CM_OPC_SV_AR_DISCONNECT_IND / _RSP */
			CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_disconnect; /* note: same type as ar_connect */

				/* CM_OPC_SV_AR_OWNERSHIP_IND / _RSP */
			CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership;

				/* CM_OPC_SV_AR_PDEVPRM_IND / _RSP */
			LSA_UINT32 ar_pdevprm; /* no arguments (placeholder) */

				/* CM_OPC_SV_AR_PRM_END_IND / _RSP */
			CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end;

				/* CM_OPC_SV_AR_IN_DATA_IND / _RSP */
			CM_UPPER_SV_AR_IN_DATA_PTR_TYPE ar_in_data;

				/* CM_OPC_SV_AR_RIR_IND / _ RSP */
			CM_SV_AR_RIR_TYPE ar_rir;

				/* CM_OPC_SV_AR_READ_IND / _RSP */
				/* CM_OPC_SV_AR_WRITE_IND / _RSP */
			CM_RECORD_TYPE ar_rec; /* see definition */

				/* CM_OPC_SV_AR_DATA_STATUS_IND / _RSP */
			CM_SV_AR_DATA_STATUS_TYPE ar_data_status;

#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE
				/* CM_OPC_SV_AR_MCONS_IND / _RSP */
			CM_SV_AR_MCONS_TYPE ar_mcons;
#endif
		} sv;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
		union {
				/* CM_OPC_MC_EVENT_AR_ADDR_INFO_IND */
			CM_MC_ADDR_INFO_TYPE  addr_info;

				/* CM_OPC_MC_EVENT_AR_IN_DATA_IND / _RSP */
			CM_MC_IN_DATA_TYPE in_data;

				/* CM_OPC_MC_EVENT_AR_OFFLINE_IND / _RSP */
			CM_AR_ABORT_OFFLINE_TYPE  ar_offline;

				/* CM_OPC_MC_EVENT_APDU_STATUS_IND / _RSP */
			LSA_UINT16  apdu_status; /* ind: see cm_event_apdu_status_enum */

		} mc;
#endif

	} u;

} CM_EVENT_TYPE;

typedef CM_EVENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_EVENT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define CM_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(CM_UPPER_OUT_FCT_ATTR,
CM_UPPER_CALLBACK_FCT_PTR_TYPE)(
	CM_UPPER_RQB_PTR_TYPE rb
);

typedef struct cm_open_channel_tag {
	LSA_HANDLE_TYPE		handle;						/* cnf: channel-handle of CM */
	LSA_HANDLE_TYPE		handle_upper;				/* req: channel-handle of user, every confirmation of an request set this handle to rqb.handle */
	LSA_SYS_PATH_TYPE	sys_path;					/* req: sys_path of channel */
	CM_UPPER_CALLBACK_FCT_PTR_TYPE
						cm_request_upper_done_ptr;	/* req: pointer to the callback-function */
} CM_OPEN_CHANNEL_TYPE;

typedef CM_OPEN_CHANNEL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_OPEN_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

/* no parameters */
typedef LSA_VOID_PTR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CLOSE_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CHANNEL_INFO
//---------------------------------------------------------------------------*/

typedef struct cm_channel_info_tag {

	LSA_UINT32		hardware_type;		/* see ACP_HW_TYPE_USED_MSK etc. */

	LSA_UINT16		interface_id;
	LSA_UINT16		port_cnt;
	CM_MAC_TYPE		mac_addr;

	LSA_UINT32		process_image_base_addr;
	LSA_UINT32		process_image_start_offset;
	LSA_UINT32		process_image_end_offset;

} CM_CHANNEL_INFO_TYPE;

typedef CM_CHANNEL_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CHANNEL_INFO_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_OVERLOAD_INFO
//---------------------------------------------------------------------------*/

typedef struct cm_overload_info_tag {

	LSA_UINT32 send_overload_counter;
	LSA_UINT32 recv_overload_counter;

	LSA_BOOL   send_overload_changed; /* for convenience */
	LSA_BOOL   recv_overload_changed; /* for convenience */

} CM_OVERLOAD_INFO_TYPE;

typedef CM_OVERLOAD_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_OVERLOAD_INFO_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_IND_RES_WITHDRAW
//---------------------------------------------------------------------------*/

typedef struct cm_ind_res_withdraw_tag {

	LSA_UINT16 events;
	LSA_UINT16 alarms_high;
	LSA_UINT16 alarms_low;

} CM_IND_RES_WITHDRAW_TYPE;

typedef CM_IND_RES_WITHDRAW_TYPE CM_UPPER_MEM_ATTR *CM_UPPER_IND_RES_WITHDRAW_PTR_TYPE;


/*-----------------------------------------------------------------------------
// begin of physical device opcodes
//---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_CREATE
//---------------------------------------------------------------------------*/

typedef struct cm_pd_oem_data_tag { /* TIA 1099470 (V5.4i1.23) */

	/* see PNIO-Spec: */
	/* - DCP, DevicePropertiesOption/SuboptionOEMDeviceID, OEMDeviceIDBlockRes */
	/* - RecordDataRead, IMData, I&M5 (Index 0xAFF5) */

	LSA_UINT16 vendor_id;
	LSA_UINT16 device_id;
	LSA_UINT8  IM_Annotation[64];		/* UTF-8, filled with blanks */
	LSA_UINT8  IM_OrderID[64];			/* UTF-8, filled with blanks */
	LSA_UINT8  IM_Serial_Number[16];	/* VisibleString, codes 0x20 - 0x7E, filled with blanks */
	LSA_UINT16 IM_Hardware_Revision;	/* (0..0xFFFF)*/
	LSA_UINT8  SWRevisionPrefix;						/* VisibleString[1], one of ('V', 'R', 'P', 'U', 'T') */
	LSA_UINT8  IM_SWRevision_Functional_Enhancement;	/* (0..0xFF)*/
	LSA_UINT8  IM_SWRevision_Bug_Fix;					/* (0..0xFF)*/
	LSA_UINT8  IM_SWRevision_Internal_Change;			/* (0..0xFF)*/

} CM_PD_OEM_DATA_TYPE;

typedef CM_PD_OEM_DATA_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_OEM_DATA_PTR_TYPE;


typedef struct cm_pd_map_tag {

	LSA_UINT16  slot_nr;     /* slot number */
	LSA_UINT16  subslot_nr;  /* subslot number (0x8ipp) */

	LSA_UINT32  mod_ident;   /* module ident number */
	LSA_UINT32  sub_ident;   /* submodule ident number */

} CM_PD_MAP_TYPE;

typedef CM_PD_MAP_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_MAP_PTR_TYPE;


typedef struct cm_pd_create_tag {

	LSA_UINT16 max_alarm_data_length; /* req: sizeof AlarmNotification-PDU; see CM_ALARM_DATA_LENGTH _MIN / _MAX */

	LSA_UINT8 annotation[63+1]; /* req: EPM: zero-terminated string, see CLRPC_PNIO_ANNOTATION_FORMAT */

	CM_UPPER_OEM_DATA_PTR_TYPE  oem_data; /* req: LSA_NULL or pointer to OEMDeviceID and I&M5Data */

	LSA_UINT16  map_count;	/* 2..n   max(n) == 1 + EDD_CFG_MAX_PORT_CNT */

	CM_PD_MAP_TYPE  map[1];
		/* index 0: interface    index 1..n: edd_port_id n */
		/* NOTE: the cm-user must alloc additional memory for the ports, see also EDD_CFG_MAX_PORT_CNT */

	/*** nothing here, dynamic size ***/

} CM_PD_CREATE_TYPE;

typedef CM_PD_CREATE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_CREATE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_DELETE
//---------------------------------------------------------------------------*/

/* no parameters */
typedef LSA_VOID_PTR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_DELETE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_LINK_STATUS_INFO
//---------------------------------------------------------------------------*/

#define CM_PORT_ID_AUTO  EDD_PORT_ID_AUTO

#define CM_LINK_STATUS_UNKNOWN      EDD_LINK_UNKNOWN
#define CM_LINK_STATUS_UP           EDD_LINK_UP
#define CM_LINK_STATUS_DOWN         EDD_LINK_DOWN
#define CM_LINK_STATUS_UP_CLOSED    EDD_LINK_UP_CLOSED   /* same as "down" for normal communication */
#define CM_LINK_STATUS_UP_DISABLED  EDD_LINK_UP_DISABLED /* same as "down" for normal communication */

#define CM_LINK_SPEED_UNKNOWN  EDD_LINK_UNKNOWN
#define CM_LINK_SPEED_10_M     EDD_LINK_SPEED_10
#define CM_LINK_SPEED_100_M    EDD_LINK_SPEED_100
#define CM_LINK_SPEED_1_G      EDD_LINK_SPEED_1000
#define CM_LINK_SPEED_10_G     EDD_LINK_SPEED_10000

#define CM_LINK_MODE_UNKNOWN      EDD_LINK_UNKNOWN
#define CM_LINK_MODE_HALF_DUPLEX  EDD_LINK_MODE_HALF
#define CM_LINK_MODE_FULL_DUPLEX  EDD_LINK_MODE_FULL

#define CM_LINK_AUTONEG_UNKNOWN   EDD_AUTONEG_UNKNOWN
#define CM_LINK_AUTONEG_ON        EDD_AUTONEG_ON
#define CM_LINK_AUTONEG_OFF       EDD_AUTONEG_OFF

typedef struct cm_link_status_info_tag {

	LSA_UINT16 interface_id; /* out(!), see channel-info */
	LSA_UINT16 port_id;      /* CM_PORT_ID_AUTO or 1..n (see channel-info) */
	LSA_UINT8  status;       /* see CM_LINK_STATUS_UNKNOWN, etc; out: never "unknown" */
	LSA_UINT8  speed;        /* see CM_LINK_SPEED_UNKNOWN, etc; out: valid if "up" */
	LSA_UINT8  mode;         /* see CM_LINK_MODE_UNKNOWN, etc; out: valid if "up" */
	LSA_UINT8  autoneg;      /* see CM_LINK_AUTONEG_UNKNOWN, etc; out: valid if "up" */

} CM_PD_LINK_STATUS_INFO_TYPE;

typedef CM_PD_LINK_STATUS_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_LINK_STATUS_INFO_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_REMAP
//---------------------------------------------------------------------------*/

typedef struct cm_pd_remap_tag {

	LSA_UINT16  map_count;	/* same as in cm_pd_create_tag::map_count */

	CM_PD_MAP_TYPE  map[1];
		/* index 0: interface    index 1..n: edd_port_id n */
		/* NOTE: the cm-user must alloc additional memory for the ports, see also EDD_CFG_MAX_PORT_CNT */

	/*** nothing here, dynamic size ***/

} CM_PD_REMAP_TYPE;

typedef CM_PD_REMAP_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_REMAP_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_PRM_BEGIN
//---------------------------------------------------------------------------*/

typedef LSA_VOID CM_UPPER_MEM_ATTR * CM_UPPER_PD_PRM_BEGIN_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_PRM_READ
// OPCODE: CM_OPC_PD_PRM_WRITE
// OPCODE: CM_OPC_PD_PRM_READ_SERVER
// OPCODE: CM_OPC_PD_PRM_WRITE_SERVER
// OPCODE: CM_OPC_PD_PRM_READ_CLIENT
//---------------------------------------------------------------------------*/

/* SendClock-record ::= definition see cm/doc/PNIO_and_extended_record_indices.doc */

enum cm_sendclock_prop_enum {
	CM_SENDCLOCK_PROP_SC_VAR_RR_VAR	= 0x0000, /* send_clock: variable  reduction_ratio: variable */
	CM_SENDCLOCK_PROP_SC_VAR_RR_FIX	= 0x0001, /* send_clock: variable  reduction_ratio: fixed    */
	CM_SENDCLOCK_PROP_SC_FIX_RR_VAR	= 0x0002, /* send_clock: fixed     reduction_ratio: variable */
	CM_SENDCLOCK_PROP_SC_FIX_RR_FIX	= 0x0003  /* send_clock: fixed     reduction_ratio: fixed    */
};


/*----------------------------------------------------------------------------*/

typedef struct cm_pd_prm_rdwr_tag {

	LSA_UINT16  slot_nr;
	LSA_UINT16  subslot_nr;
	LSA_UINT32  record_index;  /* see PNIO-Spec */
	LSA_UINT32  record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE  record_data;

	LSA_UINT32  cm_pnio_err;   /* cnf: see the macro CM_PNIO_ERR_MAKE() in file cm_err.h */
	LSA_UINT16  ret_val_1;     /* cnf: return value 1, always 0 */
	LSA_UINT16  ret_val_2;     /* cnf: return value 2, always 0 */

	LSA_BOOL    mk_remanent;
		/* the user has to specifiy this flag for the opcodes (AP00738255, CM V4.1i2.1)
		 * - CM_OPC_PD_PRM_WRITE
		 * - CM_OPC_SV_AR_PDEVPRM_WRITE
		 *
		 *   LSA_FALSE ... the record will _not_ be passed on to remanence
		 *   LSA_TRUE .... the record will be passed on to remanence
		 *
		 * for CM_OPC_PD_PRM_READ the flag is don't care
		 */

	LSA_BOOL    is_local;
		/* this flag is set internally (users need not set it) for the opcodes
		 * - CM_OPC_PD_PRM_READ
		 * - CM_OPC_PD_PRM_WRITE
		 *
		 * the user has to specifiy this flag for the opcode (AP00585108, CM V4.0i70.4)
		 * - CM_OPC_SV_AR_PDEVPRM_WRITE
		 *
		 * note: semantics of the flag have changed with CM V4.1i2.1 (AP00738255)
		 *
		 *   LSA_FALSE ... the record will be checked with normative rules
		 *   LSA_TRUE .... the record will be checked with 'local' rules
		 */

	LSA_UINT32  cls; /* see CM_REC_IDX_MASK, etc. */
		/* this field is used internally only (users need not set it)
		 * - CM_OPC_PD_PRM_READ_SERVER
		 * - CM_OPC_PD_PRM_WRITE_SERVER
		 * - CM_OPC_PD_PRM_READ_CLIENT
		 */

} CM_PD_PRM_RDWR_TYPE;

typedef CM_PD_PRM_RDWR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_PRM_RDWR_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_PRM_END
// OPCODE: CM_OPC_PD_PRM_END_SERVER
//---------------------------------------------------------------------------*/

typedef struct cm_pd_prm_end_tag {

	LSA_BOOL  set_mrp_off;  /* req: if LSA_TRUE and no other MRP record was written then CM-PD generates an "MRP off" record (AP00824058) */

	LSA_BOOL  is_master_project; /* cnf: if LSA_TRUE then record PDMasterTailorData was written within the prm-sequence */

	CLRPC_UUID_TYPE ir_data_uuid; /* req: only for CM_OPC_PD_PRM_END_SERVER, usage see cm_pd_prm_end_user() */
	LSA_BOOL pdev_problem;        /* req: only for CM_OPC_PD_PRM_END_SERVER, usage see cm_pd_prm_end_user() (V5.3i3.19, TIA 1103366) */

} CM_PD_PRM_END_TYPE;

typedef CM_PD_PRM_END_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_PRM_END_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_PRM_RELINQUISH_SERVER
//---------------------------------------------------------------------------*/

typedef LSA_VOID CM_UPPER_MEM_ATTR * CM_UPPER_PD_PRM_RELINQUISH_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_PD_SYNC_LED_INFO
//---------------------------------------------------------------------------*/

enum cm_pd_sync_led_info_enum {
	CM_PD_SYNC_LED_INFO_NOT_CONFIGURED = 0, /* meaning see text below */
	CM_PD_SYNC_LED_INFO_NOT_SYNC       = 1, /* meaning see text below */
	CM_PD_SYNC_LED_INFO_SYNC           = 2, /* meaning see text below */
	CM_PD_SYNC_LED_INFO_MAX /* for range-checking */
};

/*
 * the meaning of "sync_led_info" depends on the "edd_port_id"
 *
 * edd_port_id == 0:
 *
 *   info is requested about clock-synchronization (SyncID 0)
 *   CM_PD_SYNC_LED_INFO_NOT_CONFIGURED ... synchronizaion is not configured (parameterization does not contain the PDSyncData record)
 *   CM_PD_SYNC_LED_INFO_NOT_SYNC ......... synchronizing but not yet synchronized
 *   CM_PD_SYNC_LED_INFO_SYNC ............. the clock (from which the send-clock is derived) is synchronized
 *
 *   the field "is_rate_valid" contains additional information for the state CM_PD_SYNC_LED_INFO_NOT_SYNC
 *   LSA_FALSE ... means that no sync frames are being received
 *   LSA_TRUE .... means that sync frames are being received and the rate was determined (and the station will be synchronized soon)
 *
 *   the fields "rcv_sync_prio" and "local_prio" contain additional information for the state CM_PD_SYNC_LED_INFO_SYNC
 *   "rcv_sync_prio" ... MasterPriority1 from SyncFrame (0 at active master)
 *   "local_prio" ...... MasterPriority1 from SyncRecord (0 at slave) with ActiveBit set at active Master after Startup
 *   "pll_state" ....... State of the synchronization via hardware-signal PLL_Ext_In
 *   more information can be found in the GSY documentation (opcode GSY_OPC_DIAG_INDICATION_PROVIDE).
 *
 * edd_port_id != 0:
 *
 *   info is requested about IRT-operation of the specified port
 *   CM_PD_SYNC_LED_INFO_NOT_CONFIGURED ... IRT-operation is not configured (parameterization does not contain the
 *                                          IRData record or this port is not referenced by the IRData record)
 *   CM_PD_SYNC_LED_INFO_NOT_SYNC ......... IRT-operation is configured but the station is either not yet
 *                                          synchronized or the port has an RTClass3 mismatch diagnosis
 *   CM_PD_SYNC_LED_INFO_SYNC ............. the local RTClass3_PortStatus == RTCLASS3_RUN
 *
 *   the other fields are "don't evaluate".
 *
 *   IRT-operation needs some negotiation. See the PNIO-Spec "RTC3PSM state table" for the details.
 *   RTCLASS3_RUN == (synchronized && IRT-TX enabled && IRT-RX enabled)
 */

#define CM_PD_PLL_STATE_UNKNOWN  0 /* initial pll_state */

typedef struct cm_pd_sync_led_info_tag {

	LSA_UINT16 edd_port_id;     /* req/---: 0=interface, 1=port 1, etc... */

	LSA_UINT16 sync_led_info;   /* req/cnf: see cm_pd_sync_led_info_enum */

	LSA_BOOL  is_rate_valid;    /* req/cnf: see text above */
	LSA_UINT8 rcv_sync_prio;    /* req/cnf: see text above */
	LSA_UINT8 local_prio;       /* req/cnf: see text above */
	LSA_UINT8 pll_state;		/* req/cnf: see CM_PD_PLL_STATE_UNKNOWN and gsy_usr.h::GSY_PLL_STATE_SYNC_OFF, etc */
} CM_PD_SYNC_LED_INFO_TYPE;

typedef CM_PD_SYNC_LED_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_PD_SYNC_LED_INFO_PTR_TYPE;


/*-----------------------------------------------------------------------------
// end of physical device opcodes
//---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
// begin of client opcodes
//---------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_CREATE
//---------------------------------------------------------------------------*/

typedef struct cm_cl_create_tag {

	LSA_UINT16		max_devices;		/* req: max count of devices; 1..CM_CFG_MAX_CL_DEVICES */

	LSA_UINT8		annotation[63+1];	/* req: EPM: zero-terminated string, see CLRPC_PNIO_ANNOTATION_FORMAT */
	LSA_UINT16		vendor_id;			/* req: EPM: for object uuid of IO controller */
	LSA_UINT16		device_id;			/* req: EPM: for object uuid of IO controller */
	LSA_UINT16		instance;			/* req: EPM: for object uuid of IO controller */

	LSA_UINT16		rpc_remote_application_timeout1sec; /* req: application must respond within this number of seconds or call will be cancelled, 0=default */

	LSA_UINT32		alarm_reject;		/* req: bit set: CM acks the alarm internally with a PN IO error code */
										/*      bit 0 .. "manufacturer specific" Alarm */
										/*      bit 1 - 31 .. (1UL << CM_ALARM_TYPE_xxx) */
										/*      see CM_ALARM_MANUFACTURER_SPECIFIC_GROUP and CM_ALARM_RESERVED_GROUP */

	LSA_UINT32		alarm_priority;		/* req: bit set: alarm send with high priority */
										/*      bit 0 .. "manufacturer specific" Alarm */
										/*      bit 1 - 31 .. (1UL << CM_ALARM_TYPE_xxx) */
										/*      see CM_ALARM_MANUFACTURER_SPECIFIC_GROUP */

	LSA_UINT8		sr_ar_number;		/* req: 0 .. reserved, no CM_AR_TYPE_SINGLE_SYSRED allowed */
										/*		1 - 4 .. number of SR-AR */

	LSA_UINT16		initial_data_status;
										/* req: the initial global provider datastatus, see cm_cl_control_apdu_status_enum */

	LSA_BOOL		IOXS_Required;		/* req: corresponds to the MDD key "IOXS_Required" (V5.4i2.1x, TIA 1219981) */
										/*      LSA_TRUE ... SubmoduleProperties.DiscardIOXS is not supported */
										/*      LSA_FALSE .. SubmoduleProperties.DiscardIOXS is supported */

} CM_CL_CREATE_TYPE;

typedef CM_CL_CREATE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_CREATE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_DELETE
//---------------------------------------------------------------------------*/

/* no parameters */

typedef LSA_VOID CM_UPPER_MEM_ATTR * CM_UPPER_CL_DELETE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_CONTROL
//---------------------------------------------------------------------------*/

enum cm_cl_control_enum {
	CM_CL_CONTROL_PASSIVATE = 0,
	CM_CL_CONTROL_ACTIVATE,
	CM_CL_CONTROL_APDU_STATUS,
	CM_CL_CONTROL_NOS_ASSIGNMENT_ON,
	CM_CL_CONTROL_NOS_ASSIGNMENT_OFF
};

enum cm_cl_control_apdu_status_enum {
	CM_CL_CONTROL_APDU_STATUS_STOP	= 0x0001,
	CM_CL_CONTROL_APDU_STATUS_RUN	= 0x0002
};

/*
 * default apdu_tag after create:
 *	(CM_CL_CONTROL_APDU_STATUS_RUN)
 *
 * default nos_assignment after create:
 *	CM_CL_CONTROL_NOS_ASSIGNMENT_ON
 */

typedef struct cm_cl_control_tag {

	LSA_UINT16		control_tag;		/* req: see cm_cl_control_enum */

	LSA_UINT16		apdu_status;		/* req: see cm_cl_control_apdu_status_enum */

} CM_CL_CONTROL_TYPE;

typedef CM_CL_CONTROL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_DEVICE_ADD
//---------------------------------------------------------------------------*/

enum cm_cl_planned_mode_enum { /* see engineering tool "DeviceMode" */
	CM_CL_PMODE_SCAN_CYCLE_10MS_MASK	= 0x00FF, /* scan-cycle for (fsu-)devices: 10..200 * 10ms */

	CM_CL_PMODE_USE_GATEWAY_MASK		= 0x0100, /* see optional ARRPCBlockReq */
	CM_CL_PMODE_USE_GATEWAY_NO			= 0x0000,
	CM_CL_PMODE_USE_GATEWAY_YES			= 0x0100,

	CM_CL_PMODE_IS_FSU_MASK				= 0x0200, /* see "Establishing of an AR (fast startup procedure)" aka FSU */
	CM_CL_PMODE_IS_FSU_NO				= 0x0000,
	CM_CL_PMODE_IS_FSU_YES				= 0x0200,

	CM_CL_PMODE_IS_ACCELERATED_MASK		= 0x0400, /* see "Establishing of an AR (accelerated procedure)" aka ASU */
	CM_CL_PMODE_IS_ACCELERATED_NO		= 0x0000,
	CM_CL_PMODE_IS_ACCELERATED_YES		= 0x0400,

	CM_CL_PMODE_OPTIMIZED_IP_CHECK_MASK	= 0x0800,
	CM_CL_PMODE_OPTIMIZED_IP_CHECK_NO	= 0x0000, /* check uniqueness of ip-addr (if known) */
	CM_CL_PMODE_OPTIMIZED_IP_CHECK_YES	= 0x0800, /* check uniqueness of ip-addr only with set-ip-addr */

	CM_CL_PMODE_CHECK_DEVICE_ID_MASK	= 0x1000, /* AP01148513 */
	CM_CL_PMODE_CHECK_DEVICE_ID_NO		= 0x0000, /* legacy, do not check xxx_id, set NoS if device-NoS is empty */
	CM_CL_PMODE_CHECK_DEVICE_ID_YES		= 0x1000, /* check vendor_id, device_id and instance_id prior to set NoS */

	CM_CL_PMODE_NOS_OVERWRITE_MASK		= 0x2000, /* AP01569103 */
	CM_CL_PMODE_NOS_OVERWRITE_NO		= 0x0000, /* legacy, set NoS if device-NoS is empty */
	CM_CL_PMODE_NOS_OVERWRITE_YES		= 0x2000  /* set NoS of device found at topological position */

#define CM_CL_PMODE_ZERO_MASK		  0xFFFFC000  /* these bits must be 0 */
};

enum cm_cl_run_mode_enum {

	CM_CL_RMODE_DISABLE_DATA_STATUS_IND	= 0x0040

#define CM_CL_RMODE_ZERO_MASK		  0xFFFFFFBF  /* these bits must be 0 */
};


typedef struct cm_cl_device_add_tag {

	LSA_UINT16		device_nr;			/* local identification of this IO-Device; 1..CM_CFG_MAX_CL_DEVICES */

	LSA_UINT32		planned_mode;		/* see cm_cl_planned_mode_enum */
	LSA_UINT32		run_mode;			/* see cm_cl_run_mode_enum */

	LSA_UINT16		cmi_activity_timeout100ms;
										/* CM Initiator activity timeout; base: 100ms */

	CM_ARCB_NARE_TYPE
					dev_addr;			/* address info of this device */

	CM_ARCB_RPC_TYPE
					dev_rpc;			/* various RPC info */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_add_tag)
					ar_list;			/* list of ARs, identified by ar_nr */

	LSA_UINT32		pnio_status;		/* cnf: result of local PDU-check */

	LSA_UINT16		nr_of_event_resources;
										/* cnf: the cm-user must provide this many CM_OPC_CL_DEVICE_PROVIDE_EVENT resources */
	LSA_UINT16		nr_of_alarm_resources;
										/* cnf: the cm-user must provide this many CM_OPC_CL_DEVICE_PROVIDE_ALARM resources */

	LSA_BOOL		is_master_project;	/* cnf: true if the record PDMasterTailorData is given to the interface-submodule, the IOD is part of a master-project */

	LSA_UINT32		device_ip;			/* cnf: used ip address */

} CM_CL_DEVICE_ADD_TYPE;


typedef CM_CL_DEVICE_ADD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_DEVICE_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_DEVICE_REMOVE
//---------------------------------------------------------------------------*/

typedef struct cm_cl_device_remove_tag {

	LSA_UINT16		device_nr;			/* req: local identification of this IOD */

} CM_CL_DEVICE_REMOVE_TYPE;

typedef CM_CL_DEVICE_REMOVE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_DEVICE_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_DEVICE_CONTROL
//---------------------------------------------------------------------------*/

#define CM_CL_DEVICE_CONTROL_PASSIVATE		1
#define CM_CL_DEVICE_CONTROL_ACTIVATE		2
#define CM_CL_DEVICE_CONTROL_APDU_STATUS	3
#define CM_CL_DEVICE_CONTROL_ABORT			4

enum cm_cl_ar_control_enum { /* values can be or-ed together! */
#if 0/*see SYSRED and eddi_BufferRequest_xxx*/
	CM_CL_AR_CONTROL_PRIMARY         = 0x0001,
	CM_CL_AR_CONTROL_BACKUP          = 0x0002,
	CM_CL_AR_CONTROL_RUN             = 0x0004,
	CM_CL_AR_CONTROL_STOP            = 0x0008,
#endif
	CM_CL_AR_CONTROL_STATION_OK      = 0x0010,
	CM_CL_AR_CONTROL_STATION_PROBLEM = 0x0020
};

typedef struct cm_cl_device_control_tag {

	LSA_UINT16		device_nr;			/* req: local identification of this IOD */

	LSA_UINT16		control_tag;		/* req: see CM_CL_DEVICE_CONTROL_xxx */

	LSA_UINT16		apdu_status;		/* req: see cm_cl_ar_control_enum */

} CM_CL_DEVICE_CONTROL_TYPE;

typedef CM_CL_DEVICE_CONTROL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_DEVICE_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_RECORD_READ
// OPCODE: CM_OPC_CL_RECORD_READ_QUERY
//---------------------------------------------------------------------------*/

typedef struct cm_cl_record_read_tag {

	LSA_UINT16		device_nr;			/* req: local identification of this IOD */
	LSA_UINT16		ar_nr;				/* req: -"- */
	LSA_UINT16		session_key;

	CM_RECORD_TYPE	rec; /* see definition */

} CM_CL_RECORD_READ_TYPE;

typedef CM_CL_RECORD_READ_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_RECORD_READ_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_RECORD_WRITE
//---------------------------------------------------------------------------*/

typedef struct cm_cl_record_write_tag {

	LSA_UINT16		device_nr;			/* req: local identification of this IOD */
	LSA_UINT16		ar_nr;				/* req: -"- */
	LSA_UINT16		session_key;

	CM_RECORD_TYPE	rec; /* see definition */

} CM_CL_RECORD_WRITE_TYPE;

typedef CM_CL_RECORD_WRITE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_RECORD_WRITE_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_RECORD_READ_IMPLICIT
// OPCODE: CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY
//----------------------------------------------------------------------------*/

typedef struct cm_cl_record_read_implicit_tag {

	CM_ARCB_NARE_TYPE
					dev_addr;			/* address information */
	CM_ARCB_RPC_TYPE
					dev_rpc;			/* object uuid information */
	CLRPC_UUID_TYPE	target_uuid;		/* requestes ar uuid or nil */
	CM_RECORD_TYPE	rec;				/* see definition */

	LSA_UINT16		lower_response;		/* cnf: if CM_ERR_LOWER_NARE, mirror NARE response */
										/* cnf: if CM_ERR_LOWER_RPC, mirror CLRPC response */

} CM_CL_RECORD_READ_IMPLICIT_TYPE;

typedef CM_CL_RECORD_READ_IMPLICIT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_RECORD_WRITE_MULTIPLE
//---------------------------------------------------------------------------*/

/*
	zero-copy interface: calculate rec_list[idx].data / .data_length:
	idx in range (1 .. CmListLength(rec_list))

	| |      |                                               |
	| |      |              |                 |              |
	|C| E040 | idx1 | data1 | idx2 | data2 |P | idx3 | data3 |
	+-+----.-+------+-----.-+------+-----.-+.-+------+-----.-+
	       1              2                 3
	C .. CLRPC_PNIO_OFFSET
	P .. Padding: The number of padding octets (value=0) shall be 0,1,2,3 to have 32 bit alignment to the next IODWriteHeader.

	rqb.data_length = CM_RECORD_OFFSET

	for idx in 1 .. CmListLength {

		padding = rqb.data_length % 4
		padding = (4 - padding) % 4

		rqb.data_length += padding + CM_RECORD_HEADER_SIZE + user[idx].data_length
	}

	rqb.data = alloc (rqb.data_length)

	data_offs = CM_RECORD_HEADER_SIZE

	for idx in 1 .. CmListLength {

		padding = data_offs % 4
		padding = (4 - padding) % 4

		data_offs += padding

		rec_list[idx].data_length   = CM_RECORD_OFFSET + user[idx].data_length
		rec_list[idx].data          = rqb.data + data_offs

		memcpy (rec_list[idx].data + CM_RECORD_OFFSET, user[idx].data, user[idx].data_length)

		data_offs += CM_RECORD_HEADER_SIZE + user[idx].data_length
	}

	assert (rqb.data_length == CLRPC_PNIO_OFFSET + data_offs)

*/


typedef struct cm_cl_record_write_multiple_tag {

	LSA_UINT16		device_nr;			/* req: local identification of this IOD */
	LSA_UINT16		ar_nr;				/* req: -"- */
	LSA_UINT16		session_key;

		/*	.req
		//	if data_length == 0:
		//		CM internally allocs clrpc-mem and copies rec_list.data
		//		each rec_list[idx].data_length == sizeof(rec_list[idx].data)
		//	else:
		//		// zero-copy interface: see pseudo-code above
		//		CM uses data for clrpc-mem,
		//		rec_list.data must point within data+len
		*/
	LSA_UINT32		data_length;
	CM_COMMON_MEM_U8_PTR_TYPE
					data;

	CM_LIST_TYPED_ENTRY_TYPE(cm_record_tag)
					rec_list;			/* of type CM_RECORD_TYPE */

	LSA_UINT32		cm_pnio_err;		/* result of the rpc, rec_list::cm_pnio_err only vaild if ok */

} CM_CL_RECORD_WRITE_MULTIPLE_TYPE;

typedef CM_CL_RECORD_WRITE_MULTIPLE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_RECORD_WRITE_MULTIPLE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_LOGBOOK_READ
//---------------------------------------------------------------------------*/

enum cm_cl_logbook_lbentry_type_enum {
	CM_CL_LBENTRY_TYPE_ADD_ERR		=  1,
	CM_CL_LBENTRY_TYPE_ADD_OK		=  2,
	CM_CL_LBENTRY_TYPE_LINK_ERR		=  3,
	CM_CL_LBENTRY_TYPE_NARE_ERR		=  4,
	CM_CL_LBENTRY_TYPE_NARE_OK		=  5,
	CM_CL_LBENTRY_TYPE_CONNECT_ERR	=  6,
	CM_CL_LBENTRY_TYPE_PRM_ERR		=  7,
	CM_CL_LBENTRY_TYPE_DOWN_ERR		=  8,
	CM_CL_LBENTRY_TYPE_IN_DATA		=  9,
	CM_CL_LBENTRY_TYPE_OFFLINE		= 10,

	CM_CL_LBENTRY_TYPE_MAX			= 11
};

typedef struct cm_cl_logbook_entry_tag {
	LSA_UINT64		time_stamp;
	LSA_UINT16		session_key;
	LSA_UINT16		lbentry_type;		/* cm_cl_logbook_lbentry_type_enum */
	LSA_UINT32		data_1;
	LSA_UINT32		data_2;
} CM_CL_LOGBOOK_ENTRY_TYPE;

typedef CM_CL_LOGBOOK_ENTRY_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_LOGBOOK_ENTRY_PTR_TYPE;

typedef struct cm_cl_logbook_read_tag {

	LSA_UINT16		device_nr;
	LSA_UINT16		ar_nr;

	LSA_UINT64		curr_time_stamp;

	LSA_UINT16		entry_count;
	CM_UPPER_CL_LOGBOOK_ENTRY_PTR_TYPE
					entry;				/* entry[entry_count] */

} CM_CL_LOGBOOK_READ_TYPE;

typedef CM_CL_LOGBOOK_READ_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_CL_PRM_UPDATE
//---------------------------------------------------------------------------*/

	/* prm_mode */
#define CM_CL_PRMMODE_AR			0x0001	/* prm all submodule of the ar */
#define CM_CL_PRMMODE_SUBMODULE		0x0002	/* prm a single submodule */

typedef struct cm_cl_prm_update_tag
{
	LSA_UINT16		device_nr;	/* local identification of this IO-Device */
	LSA_UINT16		ar_nr;		/* local identification of this AR */

	LSA_UINT16		prm_mode;	/* which submodules/records will be written, see CM_CL_PRMMODE_xxx  */

	CM_ARCB_PRM_SUBMODULE_TYPE
					prm_submod;	/* valid if mode = submod: new prm-records for this submodule */

	CM_LIST_ENTRY_TYPE			/* !!! CM-internal only, do not use !!! */
					cm_internal_list;
} CM_CL_PRM_UPDATE_TYPE;

typedef CM_CL_PRM_UPDATE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_CL_PRM_UPDATE_PTR_TYPE;

/*-----------------------------------------------------------------------------
// end of client opcodes
//---------------------------------------------------------------------------*/
#endif

/*-----------------------------------------------------------------------------
// begin of server opcodes
//---------------------------------------------------------------------------*/
#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_CREATE
//---------------------------------------------------------------------------*/

typedef struct cm_sv_create_tag {

	LSA_UINT16 rpc_remote_application_timeout1sec;
		/* application must respond within this number of seconds */
		/* or call will be cancelled, 0=default */

	LSA_UINT32 alarm_priority;
		/* bit set: alarm is sent with high priority */
		/* bit 0 ........ "manufacturer specific" alarm */
		/* bit 1 - 31 ... (1 << CM_ALARM_TYPE_xxx) */
		/* see CM_ALARM_MANUFACTURER_SPECIFIC_GROUP */

	LSA_BOOL can_RecordDataReadQuery;
		/* LSA_FALSE ... reject a record-read with a RecordDataReadQuery-block */
		/* LSA_TRUE .... accept a record-read with a RecordDataReadQuery-block */
		/*               and the cm-user will handle it correctly */

	LSA_BOOL can_ARFSUDataAdjust;
		/* LSA_FALSE ... the ARFSUDataAdjust record is handled internally */
		/* LSA_TRUE .... the ARFSUDataAdjust record will be indicated to the cm-user */
		/*               and the cm-user will handle it correctly */
		/* Meaning for advanced mode ARs with ARFSUBlock: The AR is accepted only if the flag is LSA_TRUE (AP01034170) */

	LSA_BOOL enable_AlarmResponder;
		/* LSA_FALSE ... all received alarms will be acknowledged internally with CM_PNIO_ERR_AlarmTypeNotSupported (AP01035514) */
		/* LSA_TRUE .... all received alarms will be indicated to the cm-user (who will acknowledge them) */

	LSA_BOOL enable_DataStatusIndication;
		/* LSA_FALSE ... CM_OPC_SV_AR_DATA_STATUS_IND is not indicated to the cm-user */
		/* LSA_TRUE .... CM_OPC_SV_AR_DATA_STATUS_IND is indicated to the cm-user */

	LSA_BOOL IOXS_Required; /* corresponds to the GSDML key "IOXS_Required" (V5.4i2.1x, TIA 1219981) */
		/* LSA_FALSE ... SubmoduleProperties.DiscardIOXS is supported and the cm-user will handle it correctly */
		/* LSA_TRUE .... SubmoduleProperties.DiscardIOXS is not supported */

} CM_SV_CREATE_TYPE;

typedef CM_SV_CREATE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_CREATE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DELETE
//---------------------------------------------------------------------------*/

/* no parameters */
typedef LSA_VOID CM_UPPER_MEM_ATTR * CM_UPPER_SV_DELETE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_CONTROL
//---------------------------------------------------------------------------*/

#define CM_SV_CONTROL_CMD_DATA_STATUS_STOP  0x0001 /* cmd-bit 0 */
#define CM_SV_CONTROL_CMD_DATA_STATUS_RUN   0x0002 /* cmd-bit 1 */

/* the global data-status after CM_OPC_SV_CREATE is "run" (ACP_GLOBAL_PROVIDER_SET_DEFAULT) */

typedef struct cm_sv_control_tag {

	LSA_UINT16 cmd; /* see CM_SV_CONTROL_CMD_DATA_STATUS_STOP, ... */

} CM_SV_CONTROL_TYPE;

typedef CM_SV_CONTROL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DEVICE_ADD
//---------------------------------------------------------------------------*/

typedef struct cm_sv_device_add_tag {

	LSA_UINT16 device_nr; /* device number, 1..CM_CFG_MAX_SV_DEVICES */

	/***/

	LSA_UINT16 vendor_id; /* vendor ID (assigned by PROFIBUS International) */
	LSA_UINT16 device_id; /* device ID (vendor specific) */
	LSA_UINT16 instance;  /* instance number (device specific) */

	LSA_UINT8 annotation[63+1]; /* zero-terminated string, see CLRPC_PNIO_ANNOTATION_FORMAT */

	/***/

	LSA_UINT16 max_ars_IOC; /* 0..9, "IO controller AR" (AP00774491, AP01379821) */
	LSA_UINT16 max_ars_IOS; /* 0..9, "IO supervisor AR" (AP01034486, AP01379821) */
	LSA_UINT16 max_ars_DAC; /* 0..n, "IOSAR with device access" (AP00774491) */
		/*
		 * "IOC" means ARs with IOCRs and IOCRProperties.RTClass == 1/2/3/UDP and ARType == "IOCAR..."
		 * "IOS" means ARs with IOCRs and IOCRProperties.RTClass == 1/2/3/UDP and ARType == "IOSAR"
		 * "DAC" means ARs without IOCRs and ARType == "IOSAR" and ARProperties.DeviceAccess == 1
		 *
		 * max_ars_IOC + max_ars_IOS + max_ars_DAC >= 1
		 * max_ars_IOC + max_ars_IOS + max_ars_DAC <= CM_CFG_MAX_SV_ARS (see cm_cfg.h)
		 *
		 * the "implicit AR" exists always (thus the name).
		 */

	LSA_UINT16 max_slot_nr;        /* 0..0x7FFF */
	LSA_UINT16 max_subslot_nr;     /* 0..0x7FFF */
	LSA_UINT16 max_nr_of_subslots; /* 0..n (AP00774491) */
		/*
		 * the submodules of an AR (expected config) are checked against these bounds
		 * if slot_nr > max_slot_nr then the AR is rejected
		 * if subslot_nr > max_subslot_nr then the AR is rejected
		 * if number of submodules > max_nr_of_subslots then the AR is rejected
		 * note that there is no "max_api"
		 * memory usage is thus bounded by O(max_nr_of_subslots * (max_ars_IOC + max_ars_IOS))
		 */

	LSA_UINT16 alarm_send_max_length; /* size of the biggest alarm sent (V5.2i1, AP01232054) */
		/*
		 * usecase: the user specifies the size of the biggest alarm he will ever send (CM_OPC_SV_AR_ALARM_SEND)
		 * a value less than 172 (= CM_ALARM_DATA_LENGTH_MIN - CM_ALARM_OFFSET_DATA) will be rounded up
		 *
		 * req: 0..1404 (= CM_ALARM_DATA_LENGTH_MAX - CM_ALARM_OFFSET_DATA)
		 * cnf: effective value (different from .req in case of rounding up)
		 *
		 * if alarm_send_max_length > CM_SV_AR_CONNECT_TYPE::alarm_send_max_length then the AR is rejected
		 */

	LSA_BOOL contains_pdev;
		/*
		 * LSA_FALSE ... 0x8ipp submodules cannot be added.
		 * LSA_TRUE .... 0x8ipp submodules can be added (this device is the pdev-device, AP00995373).
		 */

	LSA_BOOL central_pdev;
		/*
		 * LSA_FALSE ... the PDEV can be owned and parameterized by an AR (if the device "contains_pdev").
		 * LSA_TRUE .... the PDEV cannot be owned by an AR and is parameterized via CM-PD only (AP01066848).
		 */

	LSA_BOOL parking_mode;
		/*
		 * for FSU (fast startup) devices only (AP00654135, AP00975630)
		 * the use-case is described in the documentation
		 *
		 * LSA_FALSE ... normal mode
		 * LSA_TRUE .... parking mode
		 */

	LSA_UINT16 write_multiple_parallelism;
		/*
		 * number of parallel CM_OPC_SV_AR_WRITE_IND per AR (AP00786836, CM V4.2i2.1)
		 * this parameter is effective for "real time ARs" only (not for "device access ARs")
		 * 0 ... reserved
		 * 1 ... one write-indication at a time (compatibility mode)
		 *       the cm-user sees no difference between a write-single and a write-multiple
		 * n ... at most n write-indications at a time
		 *       the cm-user must be able to handle the parallel indications
		 *       the write-indications can be responded out-of-order
		 */

	LSA_BOOL  SRD_enabled;
	LSA_BOOL  CIR_enabled;
		/*
		 * the use-cases are described in the documentation
		 *
		 * SRD_enabled
		 *   LSA_FALSE ... normal mode
		 *   LSA_TRUE .... functionality "System Redundancy" enabled
		 *
		 * CIR_enabled
		 *   LSA_FALSE ... normal mode
		 *   LSA_TRUE .... functionality "Configuration in Run" enabled
		 *   CIR can be enabled only if SRD is enabled
		 */

	LSA_BOOL PDevPrmInd_enabled; /* TIA 1095777 (V5.4i2.21) */
		/*
		 * the use-case is described in the documentation
		 * see CM_OPC_SV_AR_PDEVPRM_IND/_RSP
		 */

	LSA_UINT16 min_device_interval_31250ns; /* e.g., 32 -> 32*31250ns = 1ms (AP01451871) */
		/*
		 * 0 ... no limitation
		 * n ... see GSDML/MinDeviceInterval
		 *
		 * if AR::send_clock * AR::reduction_ratio < min_device_interval then the AR is rejected
		 */

	LSA_UINT16 nr_of_event_resources;
		/*
		 * req: don't care
		 * cnf: the cm-user must provide this many CM_OPC_SV_DEVICE_PROVIDE_EVENT resources
		 */

	LSA_UINT16 nr_of_alarm_resources;
		/*
		 * req: don't care
		 * cnf: the cm-user must provide this many CM_OPC_SV_DEVICE_PROVIDE_ALARM resources
		 */

} CM_SV_DEVICE_ADD_TYPE;

typedef CM_SV_DEVICE_ADD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DEVICE_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DEVICE_REMOVE
//---------------------------------------------------------------------------*/

typedef struct cm_sv_device_remove_tag {

	LSA_UINT16 device_nr; /* device number */

} CM_SV_DEVICE_REMOVE_TYPE;

typedef CM_SV_DEVICE_REMOVE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DEVICE_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DEVICE_CONTROL
//---------------------------------------------------------------------------*/

#define CM_SV_DEVICE_CONTROL_CMD_PASSIVATE  0
#define CM_SV_DEVICE_CONTROL_CMD_ACTIVATE   1

typedef struct cm_sv_device_control_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 cmd; /* see CM_SV_DEVICE_CONTROL_CMD_PASSIVATE, ... */

} CM_SV_DEVICE_CONTROL_TYPE;

typedef CM_SV_DEVICE_CONTROL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DEVICE_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DEVICE_LED_INFO
//---------------------------------------------------------------------------*/

#define CM_SV_DEVICE_LED_INFO_OWNED       0x0001 /* expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_OWNED */
#define CM_SV_DEVICE_LED_INFO_ARP         0x0002 /* expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_APPL_READY_PENDING */
#define CM_SV_DEVICE_LED_INFO_LOCKED_SO   0x0004 /* expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED */
#define CM_SV_DEVICE_LED_INFO_LOCKED_IOC  0x0008 /* expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER */
#define CM_SV_DEVICE_LED_INFO_LOCKED_IOS  0x0010 /* expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR */

#define CM_SV_DEVICE_LED_INFO_OK          0x0100 /* expected-config: at least one submodule is CM_SUB_STATE_IDENT_OK */
#define CM_SV_DEVICE_LED_INFO_SUBST       0x0200 /* expected-config: at least one submodule is CM_SUB_STATE_IDENT_SUBSTITUTE */
#define CM_SV_DEVICE_LED_INFO_WRONG       0x0400 /* expected-config: at least one submodule is CM_SUB_STATE_IDENT_WRONG */
#define CM_SV_DEVICE_LED_INFO_NO          0x0800 /* expected-config: at least one submodule is CM_SUB_STATE_IDENT_NO_SUBMODULE */

#define CM_SV_DEVICE_LED_INFO_DIAG        0x1000 /* real-config: at least one submodule has diagnosis information */

#define CM_SV_DEVICE_LED_INFO_MCONS       0x2000 /* m-consumer: at least one m-consumer has diagnosis information */

#define CM_SV_DEVICE_LED_INFO_RT_AR       0x4000 /* one or more RT-ARs exist, and all are in-data */

typedef struct cm_sv_device_led_info_tag {

	LSA_UINT16 device_nr; /* device number */

	/***/

	LSA_UINT32 led_info;
		/*
		 * bit set = see CM_SV_DEVICE_LED_INFO_OWNED, ...
		 */

	LSA_UINT32 maint_info;
		/*
		 * bit set = real-config: at least one submodule has maintenance or qualified information
		 *
		 * bit  0 = Maintenance-Required, see CM_MAINTENANCE_STATUS_REQUIRED
		 * bit  1 = Maintenance-Demanded, see CM_MAINTENANCE_STATUS_DEMANDED
		 * bit  2 = Reserved (since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11)
		 * bit  3 = Qualifier_3
		 * ...
		 * bit 31 = Qualifier_31
		 *
		 * NOTE: see PNIO-Spec "Values for MaintenanceStatus"
		 */

} CM_SV_DEVICE_LED_INFO_TYPE;

typedef CM_SV_DEVICE_LED_INFO_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DEVICE_READ
//----------------------------------------------------------------------------*/

typedef struct cm_sv_device_read_tag {

	LSA_UINT16					device_nr; /* device number */

	LSA_UINT32					data_length; /* req: size of 'data' (including CM_RECORD_OFFSET), cnf: CM_RECORD_OFFSET +number of bytes read */
	CM_COMMON_MEM_U8_PTR_TYPE	data;

	LSA_UINT32					api;
	LSA_UINT16					slot_nr;
	LSA_UINT16					subslot_nr;
	LSA_UINT32					record_index;  /* see PNIO-Spec */
	CLRPC_UUID_TYPE				target_ar_uuid;

	LSA_UINT32					pnio_status; /* cnf: pnio error status, see PNIO-Spec */

} CM_SV_DEVICE_READ_TYPE;

typedef CM_SV_DEVICE_READ_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DEVICE_READ_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_SUBMODULE_ADD
//---------------------------------------------------------------------------*/

#define CM_SV_IM0_BITS_NOTHING     0x00 /* this submodule does not contain I&M data */
#define CM_SV_IM0_BITS_SUBMODULE   0x01 /* this submodule contains I&M0 data, see I&M0FilterData (record index F840) */
#define CM_SV_IM0_BITS_MODULE      0x02 /* modifier: this submodule stands for the module */
#define CM_SV_IM0_BITS_DEVICE      0x04 /* modifier: this submodule stands for the device */
#define CM_SV_IM0_BITS_IM5         0x08 /* modifier: this submodule contains I&M5 data */
#define CM_SV_IM0_BITS_RESERVED    0x70
#define CM_SV_IM0_BITS_PROFIENERGY 0x80 /* note: not I&M but a solution for TIA 1094125 */

typedef struct cm_sv_submodule_add_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number, range: see "add device"  */
	LSA_UINT16 subslot_nr; /* subslot number, range: see "add device" plus 0x8000-0x8FFF */

	LSA_UINT32 mod_ident; /* module ident number */
	LSA_UINT32 sub_ident; /* submodule ident number */
	LSA_UINT8  im0_bits;  /* see CM_SV_IM0_BITS_NOTHING, ... */

	LSA_UINT16 response; /* out */

} CM_SV_SUBMODULE_ADD_ELEMENT_TYPE;

typedef CM_SV_SUBMODULE_ADD_ELEMENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_PTR_TYPE;


typedef struct cm_sv_submodule_add_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_SUBMODULE_ADD_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_SUBMODULE_ADD_TYPE;

typedef CM_SV_SUBMODULE_ADD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_SUBMODULE_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_SUBMODULE_REMOVE
//---------------------------------------------------------------------------*/

typedef struct cm_sv_submodule_remove_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	LSA_UINT16 response; /* out */

} CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE;

typedef CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_PTR_TYPE;


typedef struct cm_sv_submodule_remove_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_SUBMODULE_REMOVE_TYPE;

typedef CM_SV_SUBMODULE_REMOVE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_SUBMODULE_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DIAG_ADD
// OPCODE: CM_OPC_SV_DIAG_ADD2
// OPCODE: CM_OPC_SV_DIAG_REMOVE2 (uses the same type as CM_OPC_SV_DIAG_ADD2)
//---------------------------------------------------------------------------*/

#define CM_SV_DIAG_TYPE_CHANNEL      0 /* ChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_EXT_CHANNEL  1 /* ExtChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_QUALIFIED    2 /* QualifiedChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_GENERIC      3 /* ManufacturerSpecificDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_MAX          4 /* internally used */

typedef struct cm_sv_diag_add_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	LSA_UINT32 diag_tag;  /* diagnosis tag (!= 0) */
	LSA_UINT16 diag_type; /* CM_SV_DIAG_TYPE_CHANNEL, ... */

	LSA_UINT16 channel_number;     /* ChannelNumber, 0..0x7FFF and 0x8000 */
	LSA_UINT16 channel_properties; /* ChannelProperties, see CM_CHPR_MAINT_MASK, ... */

	union { /* specific for diag-type, see PNIO-Spec */

		struct { /* CM_SV_DIAG_TYPE_CHANNEL */
			LSA_UINT16 channel_error_type; /* ChannelErrorType */
		} ch;

		struct { /* CM_SV_DIAG_TYPE_EXT_CHANNEL */
			LSA_UINT16 channel_error_type;     /* ChannelErrorType */
			LSA_UINT16 ext_channel_error_type; /* ExtChannelErrorType */
			LSA_UINT32 ext_channel_add_value;  /* ExtChannelAddValue */
		} ext;

		struct { /* CM_SV_DIAG_TYPE_QUALIFIED */
			LSA_UINT16 channel_error_type;          /* ChannelErrorType */
			LSA_UINT16 ext_channel_error_type;      /* ExtChannelErrorType */
			LSA_UINT32 ext_channel_add_value;       /* ExtChannelAddValue */
			LSA_UINT32 qualified_channel_qualifier; /* QualifiedChannelQualifier */
		} qua;

		struct { /* CM_SV_DIAG_TYPE_GENERIC */
			LSA_UINT16                info_tag;    /* UserStructureIdentifier for info_data */
			LSA_UINT16                info_length; /* size of info_data */
			CM_COMMON_MEM_U8_PTR_TYPE info_data;   /* additional info; CM makes a copy */
		} gen;
	} u;

	LSA_UINT16 response; /* out */

} CM_SV_DIAG_ADD_ELEMENT_TYPE;

typedef CM_SV_DIAG_ADD_ELEMENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE;


typedef struct cm_sv_diag_add_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_DIAG_ADD_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_DIAG_ADD_TYPE;

typedef CM_SV_DIAG_ADD_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DIAG_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_DIAG_REMOVE
//---------------------------------------------------------------------------*/

typedef struct cm_sv_diag_remove_element_tag {

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	LSA_UINT32 diag_tag; /* diagnosis tag (see diag-add), 0 = remove all */

	LSA_UINT16 response; /* out */

} CM_SV_DIAG_REMOVE_ELEMENT_TYPE;

typedef CM_SV_DIAG_REMOVE_ELEMENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DIAG_REMOVE_ELEMENT_PTR_TYPE;


typedef struct cm_sv_diag_remove_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_DIAG_REMOVE_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_DIAG_REMOVE_TYPE;

typedef CM_SV_DIAG_REMOVE_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_DIAG_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_AR_APPL_READY
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_appl_ready_element_tag {

	LSA_UINT16 session_key; /* see connect.ind */

	LSA_UINT32 api;        /* application process identifier */
	LSA_UINT16 slot_nr;    /* slot number */
	LSA_UINT16 subslot_nr; /* subslot number */

	LSA_BOOL appl_ready_pending; /* is-not-ready ? LSA_TRUE : LSA_FALSE */

	LSA_UINT16 response; /* out */

} CM_SV_AR_APPL_READY_ELEMENT_TYPE;

typedef CM_SV_AR_APPL_READY_ELEMENT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_APPL_READY_ELEMENT_PTR_TYPE;


typedef struct cm_sv_ar_appl_ready_tag {

	LSA_UINT16 device_nr;   /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_AR_APPL_READY_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_AR_APPL_READY_TYPE;

typedef CM_SV_AR_APPL_READY_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_APPL_READY_PTR_TYPE;

/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_AR_ABORT
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_abort_tag {

	LSA_UINT16 device_nr;   /* device number */
	LSA_UINT16 session_key; /* see connect.ind */

} CM_SV_AR_ABORT_TYPE;

typedef CM_SV_AR_ABORT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_ABORT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_AR_RIR_ACK
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_rir_ack_tag {

	LSA_UINT16 device_nr;   /* device number */
	LSA_UINT16 session_key; /* see connect.ind */

} CM_SV_AR_RIR_ACK_TYPE;

typedef CM_SV_AR_RIR_ACK_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_RIR_ACK_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_AR_CONTROL
//---------------------------------------------------------------------------*/

#define CM_SV_AR_CONTROL_CMD_xxx   0 /* currently no commands supported */
#define CM_SV_AR_CONTROL_CMD_MASK  0

typedef struct cm_sv_ar_control_tag {

	LSA_UINT16 device_nr;   /* device number */
	LSA_UINT16 session_key; /* see connect.ind */

	LSA_UINT16 cmd; /* see CM_SV_AR_CONTROL_CMD_xxx, ... */

} CM_SV_AR_CONTROL_TYPE;

typedef CM_SV_AR_CONTROL_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_AR_PDEVPRM_READ
// OPCODE: CM_OPC_SV_AR_PDEVPRM_WRITE
//---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_pdevprm_rdwr_tag {

	LSA_UINT16 device_nr;   /* device number */
	LSA_UINT16 session_key; /* see connect.ind */

	CM_PD_PRM_RDWR_TYPE rec; /* see definition, implicit: api == 0 */

	/* write: don't forget to specify the "rec.mk_remanent" flag accordingly (AP00738255) */
	/* write: don't forget to specify the "rec.is_local" flag accordingly (AP00585108) */

	/* "rec.cm_pnio_err", "rec.ret_val_1", and "rec.ret_val_2" always return 0 (do not evaluate them) */

} CM_SV_AR_PDEVPRM_RDWR_TYPE;

typedef CM_SV_AR_PDEVPRM_RDWR_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_AR_PDEVPRM_RDWR_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_ARSET_TRIGGER
//---------------------------------------------------------------------------*/

typedef struct cm_sv_arset_trigger {

	LSA_UINT16 device_nr; /* device number */

	/* arset_nr := CM_SV_SESSION_KEY_TO_ARSET_NR(session_key) */

	LSA_UINT16 session_key; /* session key, see CM_OPC_SV_AR_CONNECT_IND */

	LSA_BOOL is_primary; /* LSA_TRUE ... "session_key" is the new primary AR, LSA_FALSE ... "session_key" no longer is the primary AR */

} CM_SV_ARSET_TRIGGER_TYPE;

typedef CM_SV_ARSET_TRIGGER_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_ARSET_TRIGGER_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_SV_ARSET_ABORT
//---------------------------------------------------------------------------*/

typedef struct cm_sv_arset_abort {

	LSA_UINT16 device_nr; /* device number */
	LSA_UINT16 arset_nr;  /* AR-set number (see CM_SV_SESSION_KEY_TO_ARSET_NR() too) */

} CM_SV_ARSET_ABORT_TYPE;

typedef CM_SV_ARSET_ABORT_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_SV_ARSET_ABORT_PTR_TYPE;


/*-----------------------------------------------------------------------------
// end of server opcodes
//---------------------------------------------------------------------------*/
#endif

/*-----------------------------------------------------------------------------
// begin of multicast opcodes
//---------------------------------------------------------------------------*/
#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_CREATE
//---------------------------------------------------------------------------*/

typedef struct cm_mc_create_tag {

	LSA_UINT16		max_devices;			/* req: max count of devices; 1 .. CM_CFG_MAX_MC_DEVICES */

} CM_MC_CREATE_TYPE;

typedef CM_MC_CREATE_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_MC_CREATE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_DELETE
//---------------------------------------------------------------------------*/

/* no parameters */

typedef LSA_VOID CM_UPPER_MEM_ATTR *  CM_UPPER_MC_DELETE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_CONTROL
//---------------------------------------------------------------------------*/

typedef CM_CL_CONTROL_TYPE  CM_MC_CONTROL_TYPE;

typedef CM_MC_CONTROL_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_MC_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_DEVICE_ADD
//---------------------------------------------------------------------------*/

typedef CM_CL_DEVICE_ADD_TYPE  CM_MC_DEVICE_ADD_TYPE;

typedef CM_MC_DEVICE_ADD_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_MC_DEVICE_ADD_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_DEVICE_REMOVE
//---------------------------------------------------------------------------*/

typedef CM_CL_DEVICE_REMOVE_TYPE  CM_MC_DEVICE_REMOVE_TYPE;

typedef CM_MC_DEVICE_REMOVE_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_MC_DEVICE_REMOVE_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CM_OPC_MC_DEVICE_CONTROL
//---------------------------------------------------------------------------*/

/* use CM_CL_DEVICE_CONTROL_PASSIVATE and CM_CL_DEVICE_CONTROL_ACTIVATE */

typedef CM_CL_DEVICE_CONTROL_TYPE  CM_MC_DEVICE_CONTROL_TYPE;

typedef CM_MC_DEVICE_CONTROL_TYPE CM_UPPER_MEM_ATTR *  CM_UPPER_MC_DEVICE_CONTROL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// end of multicast opcodes
//---------------------------------------------------------------------------*/
#endif


/*------------------------------------------------------------------------------
// ANY_ARGS, a UNION of all types
//----------------------------------------------------------------------------*/

typedef union cm_any_args_tag { /* may be useful: one for all... */

	union {
		CM_OPEN_CHANNEL_TYPE  open;
		/*CM_CLOSE_CHANNEL_TYPE	close; no parameters, no type! */
		CM_CHANNEL_INFO_TYPE  info;
		CM_OVERLOAD_INFO_TYPE  overload;
		CM_EVENT_TYPE  event;
		CM_ALARM_TYPE  alarm;
		CM_IND_RES_WITHDRAW_TYPE  withdraw;
	} channel;

	union {
		CM_PD_CREATE_TYPE  create;
		/*CM_PD_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_PD_REMAP_TYPE     remap;
		/***/
		CM_PD_LINK_STATUS_INFO_TYPE  link_status;
		CM_PD_SYNC_LED_INFO_TYPE     sync_led_info;
		/***/
		/*CM_PD_PRM_BEGIN_TYPE       prm_begin; no parameters */
		CM_PD_PRM_RDWR_TYPE          prm_rdwr;
		CM_PD_PRM_END_TYPE           prm_end;
		/*CM_PD_PRM_RELINQUISH_TYPE  prm_relinquish; no parameters */
	} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_CL_CREATE_TYPE  create;
		/*CM_CL_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_CL_CONTROL_TYPE  control;

		CM_CL_DEVICE_ADD_TYPE      dev_add;
		CM_CL_DEVICE_REMOVE_TYPE   dev_remove;
		CM_CL_DEVICE_CONTROL_TYPE  dev_control;

		CM_ALARM_TYPE  alarm_send;
		CM_ALARM_TYPE  alarm_ind;
		CM_ALARM_TYPE  alarm_ack_send;

		CM_CL_RECORD_READ_TYPE            read;
		CM_CL_RECORD_WRITE_TYPE           write;
		CM_CL_RECORD_WRITE_MULTIPLE_TYPE  write_multiple;

		CM_CL_LOGBOOK_READ_TYPE           logbook_read;
		CM_CL_RECORD_READ_IMPLICIT_TYPE   read_implicit;

		CM_CL_PRM_UPDATE_TYPE  prm_update;
	} cl;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_MC_CREATE_TYPE  create;
		/*CM_MC_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_MC_CONTROL_TYPE  control;
		/***/
		CM_MC_DEVICE_ADD_TYPE      dev_add;
		CM_MC_DEVICE_REMOVE_TYPE   dev_remove;
		CM_MC_DEVICE_CONTROL_TYPE  dev_control;
	} mc;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_SV_CREATE_TYPE   create;
		/*CM_SV_DELETE_TYPE d_lete; no parameters, no type! */
		CM_SV_CONTROL_TYPE  control;
		/***/
		CM_SV_DEVICE_ADD_TYPE      dev_add;
		CM_SV_DEVICE_REMOVE_TYPE   dev_remove;
		CM_SV_DEVICE_CONTROL_TYPE  dev_control;
		CM_SV_DEVICE_LED_INFO_TYPE dev_led_info;
		CM_EVENT_TYPE              dev_provide_event;
		CM_ALARM_TYPE              dev_provide_alarm; /* ALMR */
		CM_SV_DEVICE_READ_TYPE     dev_read;
		/***/
		CM_SV_SUBMODULE_ADD_TYPE    submod_add;
		CM_SV_SUBMODULE_REMOVE_TYPE submod_remove;
		CM_SV_DIAG_ADD_TYPE         diag_add;
		CM_SV_DIAG_REMOVE_TYPE      diag_remove;
		/***/
		CM_SV_ARSET_TRIGGER_TYPE arset_trigger;
		CM_SV_ARSET_ABORT_TYPE   arset_abort;
		/***/
		CM_EVENT_TYPE            ar_event;
		CM_SV_AR_ABORT_TYPE      ar_abort;
		CM_SV_AR_RIR_ACK_TYPE    ar_rir_ack;
		CM_SV_AR_APPL_READY_TYPE ar_appl_ready;
		CM_SV_AR_CONTROL_TYPE    ar_control;
		/***/
		CM_ALARM_TYPE            ar_alarm_send; /* ALMI */
		/***/
		CM_ALARM_TYPE            ar_alarm_ind; /* ALMR */
		CM_ALARM_TYPE            ar_alarm_ack; /* ALMR */
		/***/
		CM_SV_AR_PDEVPRM_RDWR_TYPE ar_pdevprm_rdwr;
	} sv;
#endif

} CM_ANY_ARGS_TYPE;

typedef CM_ANY_ARGS_TYPE CM_UPPER_MEM_ATTR * CM_UPPER_ANY_ARGS_PTR_TYPE;


/*------------------------------------------------------------------------------
// CM_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

#define CM_RQB_SET_ARGS(rqb_, v_)  {(rqb_)->args.void_ptr = (v_);}
#define CM_RQB_GET_ARGS(rqb_)      ((rqb_)->args.void_ptr)

typedef struct cm_rqb_tag {
	CM_RQB_HEADER  /* first in struct, see cm_cfg.h */

	union { /* a union of pointers for type-safety */

		union {
			CM_UPPER_OPEN_CHANNEL_PTR_TYPE   open;
			CM_UPPER_CLOSE_CHANNEL_PTR_TYPE  close;
			/***/
			CM_UPPER_CHANNEL_INFO_PTR_TYPE   info;
			CM_UPPER_OVERLOAD_INFO_PTR_TYPE  overload;
			/***/
			CM_UPPER_ALARM_PTR_TYPE  alarm;
			CM_UPPER_EVENT_PTR_TYPE  event;
			CM_UPPER_IND_RES_WITHDRAW_PTR_TYPE  withdraw;
		} channel;

		union {
			CM_UPPER_PD_CREATE_PTR_TYPE  create;
			CM_UPPER_PD_DELETE_PTR_TYPE  d_lete; /* sic! delete is a C++ keyword */
			CM_UPPER_PD_REMAP_PTR_TYPE   remap;
			/***/
			CM_UPPER_PD_LINK_STATUS_INFO_PTR_TYPE link_status;
			CM_UPPER_PD_SYNC_LED_INFO_PTR_TYPE    sync_led_info;
			/***/
			CM_UPPER_PD_PRM_BEGIN_PTR_TYPE       prm_begin;
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE        prm_rdwr;
			CM_UPPER_PD_PRM_END_PTR_TYPE         prm_end;
			CM_UPPER_PD_PRM_RELINQUISH_PTR_TYPE  prm_relinquish;
		} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_CL_CREATE_PTR_TYPE   create;
			CM_UPPER_CL_DELETE_PTR_TYPE   d_lete; /* sic! delete is a C++ keyword */
			CM_UPPER_CL_CONTROL_PTR_TYPE  control;
			/***/
			CM_UPPER_CL_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_CL_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_CL_DEVICE_CONTROL_PTR_TYPE  dev_control;
			/***/
			CM_UPPER_CL_ALARM_ACK_SEND_PTR_TYPE  alarm_ack_send;
			CM_UPPER_CL_ALARM_SEND_PTR_TYPE      alarm_send;
			/***/
			CM_UPPER_CL_RECORD_READ_PTR_TYPE            read;
			CM_UPPER_CL_RECORD_WRITE_PTR_TYPE           write;
			CM_UPPER_CL_RECORD_WRITE_MULTIPLE_PTR_TYPE  write_multiple;
			/***/
			CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE           logbook_read;
			/***/
			CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE   read_implicit;
			/***/
			CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update;
		} cl;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_MC_CREATE_PTR_TYPE   create;
			CM_UPPER_MC_DELETE_PTR_TYPE   d_lete; /* sic! delete is a C++ keyword */
			CM_UPPER_MC_CONTROL_PTR_TYPE  control;
			/***/
			CM_UPPER_MC_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_MC_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_MC_DEVICE_CONTROL_PTR_TYPE  dev_control;
		} mc;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_SV_CREATE_PTR_TYPE  create;
			CM_UPPER_SV_DELETE_PTR_TYPE  d_lete; /* sic! delete is a C++ keyword */
			CM_UPPER_SV_CONTROL_PTR_TYPE control;
			/***/
			CM_UPPER_SV_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_SV_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_SV_DEVICE_CONTROL_PTR_TYPE  dev_control;
			CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE dev_led_info;
			CM_UPPER_EVENT_PTR_TYPE              dev_provide_event;
			CM_UPPER_ALARM_PTR_TYPE              dev_provide_alarm; /* ALMR */
			CM_UPPER_SV_DEVICE_READ_PTR_TYPE     dev_read;
			/***/
			CM_UPPER_SV_SUBMODULE_ADD_PTR_TYPE    submod_add;
			CM_UPPER_SV_SUBMODULE_REMOVE_PTR_TYPE submod_remove;
			CM_UPPER_SV_DIAG_ADD_PTR_TYPE         diag_add;
			CM_UPPER_SV_DIAG_REMOVE_PTR_TYPE      diag_remove;
			/***/
			CM_UPPER_SV_ARSET_TRIGGER_PTR_TYPE arset_trigger;
			CM_UPPER_SV_ARSET_ABORT_PTR_TYPE   arset_abort;
			/***/
			CM_UPPER_EVENT_PTR_TYPE            ar_event;
			CM_UPPER_SV_AR_ABORT_PTR_TYPE      ar_abort;
			CM_UPPER_SV_AR_RIR_ACK_PTR_TYPE    ar_rir_ack;
			CM_UPPER_SV_AR_APPL_READY_PTR_TYPE ar_appl_ready;
			CM_UPPER_SV_AR_CONTROL_PTR_TYPE    ar_control;
			/***/
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_send; /* ALMI */
			/***/
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_ind; /* ALMR */
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_ack; /* ALMR */
			/***/
			CM_UPPER_SV_AR_PDEVPRM_RDWR_PTR_TYPE ar_pdevprm_rdwr;
		} sv;
#endif

		LSA_VOID CM_UPPER_MEM_ATTR * void_ptr;  /* see CM_RQB_SET_ARGS/CM_RQB_GET_ARGS */

	} args;  /* arguments corresponding to the opcode */

	CM_RQB_TRAILER /* last in struct, see cm_cfg.h */
} CM_RQB_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  cm_open_channel
 *
 * function:       open a communication channel
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  CM_UPPER_IN_FCT_ATTR
cm_open_channel(
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  cm_close_channel
 *
 * function:       close a communication channel
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  CM_UPPER_IN_FCT_ATTR
cm_close_channel(
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  cm_request
 *
 * function:       send a request to a communication channel
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  CM_UPPER_IN_FCT_ATTR
cm_request(
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  cm_upper_rqb
 *
 * function:       asynchronous allocation of an upper-RQB
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used */
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_upper_rqb(
    LSA_USER_ID_TYPE          user_id,
    LSA_UINT16                length,
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  cm_upper_mem
 *
 * function:       asynchronous allocation of an upper-memory
 *
 * NOT USED
 *
 *===========================================================================*/
#if 0 /* not used */
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR  cm_upper_mem(
    LSA_USER_ID_TYPE          user_id,
    LSA_UINT16                length,
    CM_UPPER_MEM_PTR_TYPE  upper_mem_ptr
);
#endif


/*====  out functions  =====*/

/*=============================================================================
 * function name:  CM_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     CM_UPPER_CALLBACK_FCT_PTR_TYPE
 *                                        cm_request_upper_done_ptr:
 *                     pointer to cm_request_upper_done_ptr, given by
 *                     cm_open_channel()
 *
 *                 CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE       sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef CM_REQUEST_UPPER_DONE
LSA_VOID  CM_UPPER_OUT_FCT_ATTR  CM_REQUEST_UPPER_DONE(
    CM_UPPER_CALLBACK_FCT_PTR_TYPE  cm_request_upper_done_ptr,
    CM_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                sys_ptr
);
#endif

/*=============================================================================
 * function name:  CM_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper-RQB
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_ALLOC_UPPER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ALLOC_UPPER_RQB(
    CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT16                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_FREE_UPPER_RQB
 *
 * function:       free an upper-RQB
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_FREE_UPPER_RQB
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_FREE_UPPER_RQB(
    LSA_UINT16                 CM_LOCAL_MEM_ATTR   *  ret_val_ptr,
    CM_UPPER_RQB_PTR_TYPE                             upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_ALLOC_UPPER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_ALLOC_UPPER_MEM(
    CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR   *  upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT16                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 *===========================================================================*/
#ifndef CM_FREE_UPPER_MEM
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_FREE_UPPER_MEM(
    LSA_UINT16             CM_LOCAL_MEM_ATTR   *  ret_val_ptr,
    CM_UPPER_MEM_PTR_TYPE                         upper_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  CM_UPPER_RQB_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 * maybe this should go into cm_sys.h, but there is no upper-rqb-type...
 *
 *===========================================================================*/
#ifndef CM_UPPER_RQB_ERROR
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_UPPER_RQB_ERROR(
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  CM_REQUEST_LOCAL
 *
 * function:       send a request to "self"
 *
 * documentation:  LSA_CM_Detailspec.doc
 *
 * NOTE: maybe this should go into cm_sys.h, but there is no upper-rqb-type...
 *
 *===========================================================================*/

#ifndef CM_REQUEST_LOCAL
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR
CM_REQUEST_LOCAL(
    CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_USR_H */
