#ifndef CLRPC_USR_H
#define CLRPC_USR_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_usr.h                               :F&  */
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
/*  Detailed documentation in ...\doc\LSA_CLRPC_LSA-Interface.doc       */
/*                                                                           */
/*****************************************************************************/

/*-----------------------------------------------------------------------------
// common types
//---------------------------------------------------------------------------*/

typedef LSA_UINT8 CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 CLRPC_COMMON_MEM_ATTR * CLRPC_COMMON_MEM_U8_PTR_TYPE;

/*-----------------------------------------------------------------------------
// forward declaration
//---------------------------------------------------------------------------*/

typedef struct clrpc_rqb_tag CLRPC_UPPER_RQB_ATTR * CLRPC_UPPER_RQB_PTR_TYPE;

/*------------------------------------------------------------------------------
//	OPCODES
//	CLRPC_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

#define CLRPC_OPC_OPEN_CHANNEL				0x01
#define CLRPC_OPC_CLOSE_CHANNEL				0x02
#define CLRPC_OPC_SET_INTERFACE_PARAM		0x03    /* set additional parameters for the interface of the channel */
#define CLRPC_OPC_CLEAR_INTERFACE_PARAM		0x04    /* undo the actions of SET_INTERFACE_PARAM */
#define CLRPC_OPC_TIMER						0x05

#define CLRPC_OPC_CL_BIND					0x10
#define CLRPC_OPC_CL_REBIND					0x11
#define CLRPC_OPC_CL_UNBIND					0x12
#define CLRPC_OPC_CL_CALL					0x13
#define CLRPC_OPC_CL_CANCEL					0x14
#define CLRPC_OPC_CL_QUERY_EPM_BIND			0x15	/* don't use CLRPC_ANY_ARGS_TYPE */
#define CLRPC_OPC_CL_QUERY_EPM_NEXT			0x16	/* don't use CLRPC_ANY_ARGS_TYPE */
#define CLRPC_OPC_CL_QUERY_EPM_BREAK		0x17	/* don't use CLRPC_ANY_ARGS_TYPE */
#define CLRPC_OPC_CL_QUERY_EPM_UNBIND		0x18	/* don't use CLRPC_ANY_ARGS_TYPE */

#define CLRPC_OPC_SV_REGISTER_IF_OBJ		0x20
#define CLRPC_OPC_SV_UNREGISTER_IF_OBJ		0x21
#define CLRPC_OPC_SV_reserved_22			0x22	/* was _OBJECT_REGISTER */
#define CLRPC_OPC_SV_reserved_23			0x23	/* was _OBJECT_UNREGISTER */
#define CLRPC_OPC_SV_CALL_IND_PROVIDE		0x24
#define CLRPC_OPC_SV_CALL_INDICATION		0x25
#define CLRPC_OPC_SV_CALL_RESPONSE			0x26

#define CLRPC_OPC_EPM_START					0x30
#define CLRPC_OPC_EPM_STOP					0x31

#define CLRPC_OPC_MAX						0x32


/*------------------------------------------------------------------------------
//	ERROR_CODES
//	CLRPC_RQB_TYPE::response
//----------------------------------------------------------------------------*/

#define CLRPC_OK						LSA_RSP_OK
#define CLRPC_OK_CANCELLED				(LSA_RSP_OK_OFFSET + 1)
#define CLRPC_OK_ACTIVE				    (LSA_RSP_OK_OFFSET + 2)     /* internal use only */    

#define CLRPC_ERR_RESOURCE				LSA_RSP_ERR_RESOURCE
#define CLRPC_ERR_PARAM					LSA_RSP_ERR_PARAM
#define CLRPC_ERR_SEQUENCE				LSA_RSP_ERR_SEQUENCE
#define CLRPC_ERR_SYS_PATH				LSA_RSP_ERR_SYS_PATH

enum 
{
	CLRPC_ERR_OPCODE = LSA_RSP_ERR_OFFSET,
							/* (0xc0/192) wrong opcode (server or client not configured?) */
	CLRPC_ERR_LOWER_LAYER,	/* (0xc1/193) an error occurred in the lower layer */
	CLRPC_ERR_ALLOC_LOCAL,	/* (0xc2/194) could not allocate local memory */
	CLRPC_ERR_ALLOC_LOWER,	/* (0xc3/195) could not allocate lower memory */
	CLRPC_ERR_REJECTED,		/* (0xc4/196) epm or server did reject the call, see "nca_status" */
	CLRPC_ERR_FAULTED,		/* (0xc5/197) server had a fault while executing the call, see "nca_status" */
	CLRPC_ERR_TIMEOUT,		/* (0xc6/198) epm or server did not respond within (rtx-max * rtx-timeout) */
	CLRPC_ERR_IN_ARGS,		/* (0xc7/199) broadcast or maybe "ndr_data" too large, see channel-details */
	CLRPC_ERR_OUT_ARGS,		/* (0xc8/200) server sent back more than "alloc_len" */
	CLRPC_ERR_DECODE,		/* (0xc9/201) result of epm-query/break could not be decoded */
	CLRPC_ERR_PNIO_OUT_ARGS	/* (0xca/202) out-args not "PN IO signature", too short or inconsistent */

	/*** attention: max. 62 error codes ***/
};

/*-----------------------------------------------------------------------------
// NCA status codes (incomplete list!)
// the additional information of the "nca_status" may help to
// track down a problem when CLRPC_ERR_xxx is not enough.
//---------------------------------------------------------------------------*/

/* DCE RPC NCA-status codes (see file ncastat.idl) */

#define CLRPC_NCA_COMM_FAILURE				0x1C010001 /* unable to get response from server */
#define CLRPC_NCA_OP_RANGE_ERR				0x1C010002 /* bad operation number */
#define CLRPC_NCA_UNKNOWN_IF				0x1C010003 /* unknown interface */
#define CLRPC_NCA_WRONG_BOOT_TIME			0x1C010006 /* client passed wrong boot time to server */
#define CLRPC_NCA_YOU_CRASHED				0x1C010009 /* a restartet server called back a client */
#define CLRPC_NCA_PROTO_ERROR               0x1C01000B /* someone messed up the protocol */
#define CLRPC_NCA_OUT_ARGS_TOO_BIG			0x1C010013 /* out-args are too big */
#define CLRPC_NCA_SERVER_TOO_BUSY			0x1C010014 /* server is too busy to handle call */

#define CLRPC_NCA_UNSPECIFIC_REJECT			0x1C000009
#define CLRPC_NCA_WHO_ARE_YOU_FAILED		0x1C00000B
#define CLRPC_NCA_MANAGER_NOT_ENTERED		0x1C00000C
#define CLRPC_NCA_FAULT_UNSPECIFIC			0x1C000012
#define CLRPC_NCA_FAULT_REMOTE_COMM_ERR		0x1C000013
#define CLRPC_NCA_CONTEXT_MISMATCH			0x1C00001A
#define CLRPC_NCA_FAULT_REMOTE_NO_MEMORY	0x1C00001B

/*-----------------------------------------------------------------------------
// DREP (data representation) type and codes
//---------------------------------------------------------------------------*/

typedef struct 
{
	LSA_UINT8 drep_char;
	LSA_UINT8 drep_int;
	LSA_UINT8 drep_float;
	LSA_UINT8 drep_reserverd;
} CLRPC_DREP_TYPE;

	/* ==> never change the values: see clrpc_int.h */

#define CLRPC_DREP_CHAR_ASCII	(0 << 0)
#define CLRPC_DREP_CHAR_EBCDIC	(1 << 0)

#define CLRPC_DREP_INT_BIG		(0 << 4)
#define CLRPC_DREP_INT_LITTLE	(1 << 4)

#define CLRPC_DREP_FLOAT_IEEE	(0 << 0)
#define CLRPC_DREP_FLOAT_VAX	(1 << 0)
#define CLRPC_DREP_FLOAT_CRAY	(2 << 0)
#define CLRPC_DREP_FLOAT_IBM	(3 << 0)

/*-----------------------------------------------------------------------------
// constants for call semantics
//---------------------------------------------------------------------------*/

enum clrpc_semantics_enum 
{
	CLRPC_SEMANTICS_AT_MOST_ONCE = 0, /* the default */
	CLRPC_SEMANTICS_IDEMPOTENT,
	CLRPC_SEMANTICS_MAYBE,
	CLRPC_SEMANTICS_BROADCAST,
	CLRPC_SEMANTICS_BROADCAST_MAYBE,
	CLRPC_SEMANTICS_MAX /* for range check */
};

/*------------------------------------------------------------------------------
// PN IO signature
// (see clrpc_pdu.h)
//----------------------------------------------------------------------------*/

#define CLRPC_PNIO_OFFSET (4 + 4 + 4+4+4) /* offset for "user octets" in ndr_data */

/*
 * NOTE: the coding of the 'pnio_status' is application-specific
 */

#define CLRPC_PNIO_OPNUM_CONNECT       0
#define CLRPC_PNIO_OPNUM_RELEASE       1
#define CLRPC_PNIO_OPNUM_READ          2
#define CLRPC_PNIO_OPNUM_WRITE         3
#define CLRPC_PNIO_OPNUM_CONTROL       4
#define CLRPC_PNIO_OPNUM_READ_IMPLICIT 5
#define CLRPC_PNIO_OPNUM_MAX           CLRPC_PNIO_OPNUM_READ_IMPLICIT /* the last opnum, see CLRPC_SV_REGISTER_IF_OBJ_TYPE */

/*------------------------------------------------------------------------------
// size and format string for pnio-annotation (see CLRPC_OPC_SV_REGISTER_IF_OBJ / CLRPC_OPC_EPM_START)
// intended usage: sprintf()
//
// RPCAnnotation ::=
//  DeviceType, Blank, OrderID, Blank, HWRevision, Blank,
//  SWRevisionPrefix, SWRevision, EndTerm
//----------------------------------------------------------------------------*/

#define CLRPC_MAX_ANNOTATION_SIZE (63+1) /* size is defined by DCE RPC, see ep.idl */

#define CLRPC_PNIO_ANNOTATION_FORMAT   "%-25.25s %-20.20s %5u %c%3u%3u%3u"

#define CLRPC_PNIO_ANNOTATION(sDeviceType, sOrderID, uHWRevision, cSWRevisionPrefix, uSWRevision1, uSWRevision2, uSWRevision3) \
	CLRPC_PNIO_ANNOTATION_FORMAT, sDeviceType, sOrderID, uHWRevision, cSWRevisionPrefix, uSWRevision1, uSWRevision2, uSWRevision3


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define CLRPC_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(CLRPC_UPPER_OUT_FCT_ATTR,
CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE)(
	CLRPC_UPPER_RQB_PTR_TYPE rb
);

typedef struct 
{
	LSA_HANDLE_TYPE		handle;				/* cnf: channel-handle of CLRPC */
	LSA_HANDLE_TYPE		handle_upper;		/* req: channel-handle of user, every confirmation of a request set this handle to rqb.handle */
	LSA_SYS_PATH_TYPE	sys_path;			/* req: sys_path of channel */
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE
						clrpc_request_upper_done_ptr;
											/* req: pointer to the callback-function */
} CLRPC_OPEN_CHANNEL_TYPE;

typedef CLRPC_OPEN_CHANNEL_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_OPEN_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

/* no paramters */

typedef LSA_VOID_PTR_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CLOSE_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SET_INTERFACE_PARAM
//---------------------------------------------------------------------------*/

typedef struct clrpc_set_interface_param_tag
{
	LSA_UINT32	trace_idx;		                /* req: for "LSA trace index", see CLRPC_CFG_TRACE_MODE */
                                                /* cnf: - */
	LSA_UINT8	mac_addr[6];		            /* req: needed by clrpc_uuid_create() or 0 if not known */
                                                /* cnf: - */
    LSA_UINT16  client_receive_resources;       /* req: number of receive resources for the client, 
                                                        0 ==> no socket will be opened for client functions */
                                                /* cnf: - */
} CLRPC_SET_INTERFACE_PARAM_TYPE;

typedef CLRPC_SET_INTERFACE_PARAM_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_SET_INTERFACE_PARAM_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CLEAR_INTERFACE_PARAM
//---------------------------------------------------------------------------*/

/* no paramters */

typedef LSA_VOID_PTR_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CLEAR_INTERFACE_PARAM_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_BIND
// OPCODE: CLRPC_OPC_CL_REBIND
//---------------------------------------------------------------------------*/

#define CLRPC_INVALID_CLIENT_ID  LSA_NULL

typedef struct 
{
	LSA_VOID_PTR_TYPE	client_id; /* first in struct! */
	/***/
	LSA_UINT32			host_ip;  /* network byte order! see CLRPC_HTONL() */
	LSA_UINT16			udp_port; /* network byte order! see CLRPC_HTONS() */
	CLRPC_UUID_TYPE		if_uuid;
	LSA_UINT16			if_version_major;
	LSA_UINT16			if_version_minor;
	CLRPC_UUID_TYPE		object_uuid;
    /* AP01309324: timeout_msec deleted, CLRPC_RTX_TIMEOUT_MSEC will be used now */
} CLRPC_CL_BIND_TYPE;

typedef CLRPC_CL_BIND_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CL_BIND_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_UNBIND
//---------------------------------------------------------------------------*/

typedef struct 
{
	LSA_VOID_PTR_TYPE	client_id; /* first in struct! */
	/***/
} CLRPC_CL_UNBIND_TYPE;

typedef CLRPC_CL_UNBIND_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CL_UNBIND_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_CALL
//---------------------------------------------------------------------------*/

typedef struct 
{
	LSA_VOID_PTR_TYPE	client_id; /* first in struct! */
	/***/
	LSA_UINT16			opnum;             /* [IN] 0=first function in interface (IDL) */
	LSA_UINT16			semantics;         /* [IN] see CLRPC_SEMANTICS_XXX */
	LSA_UINT16			pnio_signature;    /* [IN] 1="PN IO signature call" 0=otherwise */
	LSA_UINT16			appl_timeout1sec;  /* [IN, OUT] the server must respond within this time */
	                                       /* 0 = default (300sec) */
	                                       /* NOTE: [OUT] because CLRPC converts this parameter from seconds into "pings" */
	/***/
	LSA_UINT32			nca_status;        /* [OUT] CLRPC_NCA_XXX */
	/***/
	LSA_UINT32			pnio_status;       /* [OUT] PN IO status code (opaque for CLRPC) */
	/***/
	CLRPC_DREP_TYPE		drep;              /* [OUT] see CLRPC_DREP_XXX */
	/***/
	LSA_UINT32			alloc_len;         /* [IN] allocated length of ndr_data */
	LSA_UINT32			ndr_len;           /* [IN, OUT] actual length of ndr_data */
	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data;  /* [IN, OUT] buffer with NDR-encoded data */
	                                       /* must be at least 4 byte aligned */
	/***/
} CLRPC_CL_CALL_TYPE;

typedef CLRPC_CL_CALL_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CL_CALL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_CANCEL
//---------------------------------------------------------------------------*/

typedef struct 
{
	LSA_VOID_PTR_TYPE	client_id; /* first in struct! */
	/***/
} CLRPC_CL_CANCEL_TYPE;

typedef CLRPC_CL_CANCEL_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CL_CANCEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_BIND
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_NEXT
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_BREAK
// OPCODE: CLRPC_OPC_CL_QUERY_EPM_UNBIND
//---------------------------------------------------------------------------*/

#define CLRPC_QUERY_EPM_NDR_DATA_SIZE 256 /* see clrpc_int.h, too */

enum clrpc_query_epm_state_enum 
{
	CLRPC_QUERY_EPM_BIND = 0,
	CLRPC_QUERY_EPM_LOOKUP_FIRST,
	CLRPC_QUERY_EPM_LOOKUP_NEXT,
	CLRPC_QUERY_EPM_LOOKUP_FREE,
	CLRPC_QUERY_EPM_UNBIND,
	CLRPC_QUERY_EPM_STOP
}; /* used internally only! */

typedef struct 
{
	union 
    { /* first in struct! */
		CLRPC_CL_BIND_TYPE bind;
		CLRPC_CL_CALL_TYPE call;
		CLRPC_CL_UNBIND_TYPE unbind;
	} u;

	/*** a user-supplied buffer for the query ***/

	LSA_UINT32 alloc_len; /* actual length of ndr_data, must be at least CLRPC_QUERY_EPM_NDR_DATA_SIZE bytes in size */

	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data;  /* [IN, OUT] buffer with NDR-encoded data */
	                                       /* must be at least 4 byte aligned */

	/*** the result ***/

	struct 
    {
		LSA_UINT16 valid; /* != 0 if the entry is valid */
		LSA_UINT16 more;  /* != 0 if there are more entries */
		/***/
		CLRPC_UUID_TYPE object_uuid;
		CLRPC_UUID_TYPE if_uuid;
		LSA_UINT16 if_vers_major;
		LSA_UINT16 if_vers_minor;
		LSA_UINT32 host_ip;  /* network byte order! see CLRPC_NTOHL() */
		LSA_UINT16 udp_port; /* network byte order! see CLRPC_NTOHS() */

		LSA_UINT8 annotation[CLRPC_MAX_ANNOTATION_SIZE];
	} epm_entry; /* OUT: the result */

	/*** the rest is for "internal" use only ***/

	LSA_UINT16 state; /* see clrpc_query_epm_state_enum */

	CLRPC_UUID_TYPE ctx_uuid; /* context-handle UUID */

} CLRPC_CL_QUERY_EPM_TYPE;

typedef CLRPC_CL_QUERY_EPM_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_REGISTER_IF_OBJ
//---------------------------------------------------------------------------*/

#define CLRPC_INVALID_SERVER_ID			0xFFFF

typedef struct clrpc_sv_register_if_obj_tag 
{
	LSA_UINT16			server_id;				/* out: valid server_id if response is ok */
	CLRPC_UUID_TYPE		if_uuid;				/* in.: uuid of the interface */
	LSA_UINT16			if_version_major;		/* in.: version of the interface, major part */
	LSA_UINT16			if_version_minor;		/* in.: version of the interface, minor part */
	LSA_UINT16			udp_port;				/* in.: >0..use this udp-port, 0..sock select a free one */
												/* out: the selected udp-port number */
	LSA_UINT32			alloc_len;				/* in.: alloc alloc_len bytes for recv-data, reject call if more data arrived */
	LSA_UINT16			opnum_max;				/* in.: for opnum check, 0 <= opnum <= opnum_max */
	LSA_UINT16			pnio_signature;			/* in.: 1="all functions have PN IO signature" 0=otherwise */
	LSA_UINT16			recv_resource_count;	/* in.: provide recv_count recv resources to sock */
	LSA_UINT16			send_resource_count;	/* in.: all scalls use send_count common send resources */

/* if_obj_type */
	CLRPC_UUID_TYPE		object_uuid;		/* in.: register this object-uuid */
	LSA_VOID_PTR_TYPE	usr_id;				/* in.: with this user-id, see CLRPC_OPC_SV_CALL_INDICATION */
	LSA_UINT8			annotation[CLRPC_MAX_ANNOTATION_SIZE];
											/* in.: the endpoint-mapper shows this ann for the registered object-uuid */
} CLRPC_SV_REGISTER_IF_OBJ_TYPE;

typedef CLRPC_SV_REGISTER_IF_OBJ_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_UNREGISTER_IF_OBJ
//---------------------------------------------------------------------------*/

typedef struct clrpc_sv_unregister_if_obj_tag 
{
	LSA_UINT16		server_id;
} CLRPC_SV_UNREGISTER_IF_OBJ_TYPE;

typedef CLRPC_SV_UNREGISTER_IF_OBJ_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_SV_UNREGISTER_IF_OBJ_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_SV_CALL_IND_PROVIDE
// INDICATION: CLRPC_OPC_SV_CALL_INDICATION
// OPCODE: CLRPC_OPC_SV_CALL_RESPONSE
//---------------------------------------------------------------------------*/

#define CLRPC_PNIO_STATUS_INVALID  0xFFFFFFFF

typedef struct 
{
	LSA_UINT16					server_id;	/* in.: used server */
	LSA_VOID_PTR_TYPE			scall_id;	/* in.: internal handle to the scall */
											/* out: DO NOT CHANGE */
	LSA_VOID_PTR_TYPE			usr_id;		/* in.: see CLRPC_OPC_SV_REGISTER_IF_OBJ */
	LSA_UINT32					host_ip;	/* in.: network byte order! see CLRPC_NTOHL() */
	LSA_UINT16					udp_port;	/* in.: network byte order! see CLRPC_NTOHS() */
	LSA_UINT16					opnum;		/* in.: the opnum of the call */
	LSA_UINT16					semantics;	/* in.: see CLRPC_SEMANTICS_* */
											/* out: DO NOT CHANGE */
	CLRPC_DREP_TYPE				drep;		/* in.: see CLRPC_DREP_* */
	CLRPC_UUID_TYPE				object_id;	/* in.: see CLRPC_OPC_SV_REGISTER_IF_OBJ */
	LSA_UINT32					alloc_len;	/* in.: ndr_data points to a buffer with this size, see CLRPC_OPC_SV_REGISTER_IF_OBJ */
											/* out: DO NOT CHANGE */
	LSA_UINT32					ndr_len;	/* in.: received bytes, ndr_data[0..ndr_len-1] */
											/* out: send ndr_len bytes as response, ndr_data[0..ndr_len-1] */
	CLRPC_UPPER_MEM_U8_PTR_TYPE	ndr_data;	/* in.: buffer with recv data */
											/* out: DO NOT CHANGE THE POINTER */
											/* out: data to send */
	                                        /* NOTE: buffer is at least 4 byte aligned */
	/* if interface registered with "PN IO signature" */
	LSA_UINT32					pnio_status;/* out: PN IO status code (opaque for CLRPC) */
} CLRPC_SV_CALL_TYPE;

typedef CLRPC_SV_CALL_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_SV_CALL_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_EPM_START
//----------------------------------------------------------------------------*/

typedef struct 
{
	LSA_UINT16		server_id;				/* out: valid epm-server_id if response is ok */
	LSA_UINT16		udp_port;				/* out: the used udp-port number for the endpoint-mapper */

	LSA_UINT16		recv_resource_count;	/* in.: provide recv_count recv resources to sock */
	LSA_UINT16		send_resource_count;	/* in.: all scalls use send_count common send resources */

	LSA_UINT8		annotation[CLRPC_MAX_ANNOTATION_SIZE];
											/* in.: the endpoint-mapper shows this ann for the epm */
} CLRPC_EPM_START_TYPE;

typedef CLRPC_EPM_START_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_EPM_START_PTR_TYPE;


/*------------------------------------------------------------------------------
// OPCODE: CLRPC_OPC_EPM_STOP
//----------------------------------------------------------------------------*/

typedef CLRPC_SV_UNREGISTER_IF_OBJ_TYPE CLRPC_EPM_STOP_TYPE;

typedef CLRPC_EPM_STOP_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_EPM_STOP_PTR_TYPE;


/*------------------------------------------------------------------------------
// CLRPC_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

#define CLRPC_RQB_SET_ARGS(rqb_,val_)		{(rqb_)->args.void_ptr = (val_);}
#define CLRPC_RQB_GET_ARGS(rqb_)			((rqb_)->args.void_ptr)

typedef struct clrpc_rqb_tag 
{
	CLRPC_RQB_HEADER /* see clrpc_cfg.h */

	union 
    { /* a union of pointers for type-safety */

		union 
        {
			CLRPC_UPPER_OPEN_CHANNEL_PTR_TYPE	        open;
			CLRPC_UPPER_CLOSE_CHANNEL_PTR_TYPE	        close;
			CLRPC_UPPER_SET_INTERFACE_PARAM_PTR_TYPE	set_interface_param;
			CLRPC_UPPER_CLEAR_INTERFACE_PARAM_PTR_TYPE	clear_interface_param;
		} channel;

		union 
        {
			CLRPC_UPPER_CL_BIND_PTR_TYPE		bind;
			CLRPC_UPPER_CL_UNBIND_PTR_TYPE		unbind;
			CLRPC_UPPER_CL_CALL_PTR_TYPE		call;
			CLRPC_UPPER_CL_CANCEL_PTR_TYPE		cancel;
			CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE	query_epm;
		} cl; /* don't forget to update 'ANY_ARGS' */

		union 
        {
			CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE		reg_if_obj;
			CLRPC_UPPER_SV_UNREGISTER_IF_OBJ_PTR_TYPE	unreg_if_obj;
			CLRPC_UPPER_SV_CALL_PTR_TYPE				call;
		} sv; /* don't forget to update 'ANY_ARGS' */

		union 
        {
			CLRPC_UPPER_EPM_START_PTR_TYPE	start;
			CLRPC_UPPER_EPM_STOP_PTR_TYPE	stop;
		} epm; /* don't forget to update 'ANY_ARGS' */

		LSA_VOID CLRPC_UPPER_MEM_ATTR * void_ptr; /* for set/get macro */

	} args; /* var: arguments corresponding to the opcode */

	CLRPC_RQB_TRAILER /* see clrpc_cfg.h */
} CLRPC_RQB_TYPE;


/*------------------------------------------------------------------------------
// UNION of all types
//----------------------------------------------------------------------------*/

typedef union clrpc_any_args_tag 
{ /* may be useful: one for all... */

	union 
    {
		CLRPC_OPEN_CHANNEL_TYPE			open;
		/* close: no parameters */
		CLRPC_SET_INTERFACE_PARAM_TYPE	set_interface_param;
		/* clear_interface_param: no parameters */
	} channel;

	union 
    {
		CLRPC_CL_BIND_TYPE				bind;
		CLRPC_CL_UNBIND_TYPE			unbind;
		CLRPC_CL_CALL_TYPE				call;
		CLRPC_CL_CANCEL_TYPE			cancel;
		CLRPC_CL_QUERY_EPM_TYPE			query_epm;
	} cl;

	union 
    {
		CLRPC_SV_REGISTER_IF_OBJ_TYPE	reg_if_obj;
		CLRPC_SV_UNREGISTER_IF_OBJ_TYPE	unreg_if_obj;
		CLRPC_SV_CALL_TYPE				call;
	} sv;

	union 
    {
		CLRPC_EPM_START_TYPE			start;
		CLRPC_EPM_STOP_TYPE				stop;
	} epm;

} CLRPC_ANY_ARGS_TYPE;

typedef CLRPC_ANY_ARGS_TYPE CLRPC_UPPER_MEM_ATTR * CLRPC_UPPER_ANY_ARGS_PTR_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  clrpc_open_channel
 *
 * function:       open a communication channel
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR  clrpc_open_channel(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  clrpc_close_channel
 *
 * function:       close a communication channel
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR  clrpc_close_channel(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  clrpc_request
 *
 * function:       send a request to a communication channel
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR  clrpc_request(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  clrpc_upper_rqb
 *
 * function:       asynchronous allocation of an upper-RQB
 *
 * documentation:  none, not used
 *
 *===========================================================================*/
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_upper_rqb(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT16                length,
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  clrpc_upper_mem
 *
 * function:       asynchronous allocation of an upper-memory
 *
 * documentation:  none, not used
 *
 *===========================================================================*/
LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_upper_mem(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT16                length,
	CLRPC_UPPER_MEM_PTR_TYPE  upper_mem_ptr
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  CLRPC_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE 
 *                                           clrpc_request_upper_done_ptr:
 *                     pointer to clrpc_request_upper_done_ptr, given by
 *                     clrpc_open_channel()
 *
 *                 CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE          sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef CLRPC_REQUEST_UPPER_DONE
LSA_VOID  CLRPC_UPPER_OUT_FCT_ATTR  CLRPC_REQUEST_UPPER_DONE(
    CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE  clrpc_request_upper_done_ptr,
    CLRPC_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                   sys_ptr 
);
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper-RQB
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_ALLOC_UPPER_RQB
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_UPPER_RQB(
	CLRPC_UPPER_RQB_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_UPPER_RQB
 *
 * function:       free an upper-RQB
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_FREE_UPPER_RQB
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_UPPER_RQB(
	LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_ALLOC_UPPER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_ALLOC_UPPER_MEM(
	CLRPC_UPPER_MEM_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_FREE_UPPER_MEM
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_FREE_UPPER_MEM(
	LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	CLRPC_UPPER_MEM_PTR_TYPE                           upper_mem_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  CLRPC_UPPER_RQB_ERROR
 *
 * function:       notify a fatal-error
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/
#ifndef CLRPC_UPPER_RQB_ERROR
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_UPPER_RQB_ERROR(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  CLRPC_REQUEST_LOCAL
 *
 * function:       send a request to "self"
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef CLRPC_REQUEST_LOCAL
LSA_VOID  CLRPC_SYSTEM_OUT_FCT_ATTR  CLRPC_REQUEST_LOCAL(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_USR_H */
