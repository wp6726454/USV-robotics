#ifndef CLRPC_PDU_H
#define CLRPC_PDU_H

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
/*  F i l e               &F: clrpc_pdu.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PDU things, (packed structs)                                             */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by clrpc_int.h
 */

/*
 * NOTE: with V4.2i2.2 the packing-defines were removed (see clrpc_check_sizes(), AP00920933)
 */

#ifndef CLRPC_PNIO_OFFSET
# error "wrong include order"
#endif

/*------------------------------------------------------------------------------
// PN IO Signature
//

	[ uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
	, version(x.x)
	] interface <interface-name> {

	[idempotent] void <function-name>(

		[in] handle_t binding_handle, // "local matter", will not be transmitted

		[out] unsigned long *pnio_status, // PN IO Status-Code

		[in] unsigned long args_max,

		[in, out] unsigned long *args_len,

		[in, out, size_is(args_max), length_is(*args_len)] byte args[],

		[out, comm_status, fault_status] error_status_t *err_status // "local matter"
			// Microsoft: via ACF-File
			// CLRPC: "response" and "nca_status" in the RQB.
	);

	}

//
//----------------------------------------------------------------------------*/

struct clrpc_pnio_in_args_tag { /* (a.k.a. in-params) from client to server */

	/* "binding_handle" is "local matter", not transmitted */

	LSA_UINT32 args_max; /* allocated size */

	LSA_UINT32 args_len; /* actual size of "args.octets[]" */

	struct {
		LSA_UINT32 maximum_count; /* same as "args_max" */
		LSA_UINT32 offset;        /* zero! */
		LSA_UINT32 actual_count;  /* same as "args_len" */
		/* LSA_UINT8 octets[0] -- filled with "args_len" octets */
	} args;

	/* "err_status" is "local matter", not transmitted */

};

typedef struct clrpc_pnio_in_args_tag CLRPC_PNIO_IN_ARGS_TYPE;
typedef struct clrpc_pnio_in_args_tag CLRPC_COMMON_MEM_ATTR *CLRPC_PNIO_IN_ARGS_PTR_TYPE;

#define CLRPC_PNIO_IN_ARGS_SIZE  CLRPC_PNIO_OFFSET

/*----------------------------------------------------------------------------*/

struct clrpc_pnio_out_args_tag { /* (a.k.a. out-params) from server to client */

	/* "binding_handle" is "local matter", not transmitted */

	LSA_UINT32 pnio_status;

	LSA_UINT32 args_len; /* actual size of "args.octets[]" */

	struct {
		LSA_UINT32 maximum_count; /* same as "args_max" */
		LSA_UINT32 offset;        /* zero! */
		LSA_UINT32 actual_count;  /* same as "args_len" */
		/* LSA_UINT8 octets[0] -- filled with "args_len" octets */
	} args;

	/* "err_status" is "local matter", not transmitted */

};

typedef struct clrpc_pnio_out_args_tag CLRPC_PNIO_OUT_ARGS_TYPE;
typedef struct clrpc_pnio_out_args_tag CLRPC_COMMON_MEM_ATTR *CLRPC_PNIO_OUT_ARGS_PTR_TYPE;

#define CLRPC_PNIO_OUT_ARGS_SIZE  CLRPC_PNIO_OFFSET

/*------------------------------------------------------------------------------
// DCE RPC types and constants
//----------------------------------------------------------------------------*/

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
 *  #define RPC_C_IP_LLC_SIZE            8 // LLC frame for SNAP protocol
 *  #define RPC_C_IP_HDR_SIZE           20 // Base IP header
 *  #define RPC_C_IP_OPTS_SIZE           0 // IP options length
 *  #define RPC_C_UDP_HDR_SIZE           8 // UDP header
 *
 *  #define RPC_C_IP_UDP_MAX_LOC_UNFRG_TPDU ( RPC_C_ETHER_MAX_DATA_SIZE - \
 *          (RPC_C_IP_LLC_SIZE + RPC_C_IP_HDR_SIZE + RPC_C_IP_OPTS_SIZE + RPC_C_UDP_HDR_SIZE) \
 *          )
 */

#define CLRPC_BUFFER_SIZE  1500 /* see CLRPC_MUST_RECV_FRAG_SIZE */

#define CLRPC_MUST_RECV_FRAG_SIZE  1464 /* see DCE RPC Appendix K */

/*------------------------------------------------------------------------------
 * see OSF sources DG.H line 2010
 *
 * "RPC_C_DG_MAX_WINDOW_SIZE" is the size of the window we want to offer
 * (in kilobytes). The window size indicates the receiver's idea of
 * how much buffering is available in the network (including
 * IP/UDP). The window size is the receiver's advice on how much
 * *unacknowledged* data the sender should have outstanding. This is
 * changed dynamically by the receiver.
 *
 * NOTE: DCE RPC says 24, see OSF sources DG.H line 2027
 *
 * NOTE: it is not clear how this value is interpreted by a Microsoft server.
 *       even stranger: the same call is answered sometimes with packets
 *       of 1024 or 1392 UDP-payload although we always advertise "must recv frag size".
 *
 * NOTE: for the calculation of the window size see DGCALL.C line 459
 *       min. window of 10 frags for want-fack interval >= 4
 *
 */

#define CLRPC_MAX_WINDOW_SIZE  5 /* PNIO: 5KB ~ 3 frags ~ 4KB net-data */

#define CLRPC_MAX_NOFACK_SIZE  3 /* PNIO: allow 4KB net-data without fack */

/*------------------------------------------------------------------------------
// Endpoint-Mapper
//----------------------------------------------------------------------------*/

#define CLRPC_EPM_DEFAULT_PORT    CLRPC_HTONS(0x8894) /* see PNIO-Spec (note: 135 for DCE RPC, see ep.idl) */

#define CLRPC_EPT_NOT_REGISTERED  0x16c9a0d6 /* see rpcsts.idl */

#define CLRPC_EPM_ALLOC_LEN       216 /* see register.alloc_len (min: 216) */

#if CLRPC_QUERY_EPM_NDR_DATA_SIZE < CLRPC_EPM_ALLOC_LEN
# error "must adapt! see clrpc_usr.h"
#endif

extern CLRPC_UUID_TYPE clrpc_endpoint_mapper_uuid; /* defined in clrpc_uuid.c */

#if CLRPC_CFG_MAX_SERVERS
extern CLRPC_UUID_TYPE clrpc_endpoint_type_id; /* defined in clrpc_uuid.c */
#endif

/*------------------------------------------------------------------------------
// DCE RPC header
//----------------------------------------------------------------------------*/

struct clrpc_header_tag {
	LSA_UINT8 rpc_vers;        /* 00:01 RPC version (see below) */
	LSA_UINT8 ptype;           /* 01:01 packet type (see below) */
	LSA_UINT8 flags;           /* 02:01 flags (see CLRPC_PF_... below) */
	LSA_UINT8 flags2;          /* 03:01 more flag (see CLRPC_PF2_... below) */
	LSA_UINT8 drep[3];         /* 04:03 data type format of sender (see below) */
	LSA_UINT8 serial_hi;       /* 07:01 high byte of fragment serial number */
	CLRPC_UUID_TYPE object;    /* 08:16 object UID */
	CLRPC_UUID_TYPE if_id;     /* 24:16 interface UID */
	CLRPC_UUID_TYPE actuid;    /* 40:16 activity UID of caller */
	LSA_UINT32 server_boot;    /* 56:04 time server booted */
	LSA_UINT32 if_vers;        /* 60:04 version of interface */
	LSA_UINT32 seq;            /* 64:04 sequence number -- monotonically increasing */
	LSA_UINT16 opnum;          /* 68:02 operation number within the trait */
	LSA_UINT16 ihint;          /* 70:02 interface hint (which interface w/in server) */
	LSA_UINT16 ahint;          /* 72:02 activity hint */
	LSA_UINT16 len;            /* 74:02 length of body */
	LSA_UINT16 fragnum;        /* 76:02 fragment number */
	LSA_UINT8 auth_proto;      /* 78:01 authentication protocol */
	LSA_UINT8 serial_lo;       /* 79:01 low byte of fragment serial number */
};

typedef struct clrpc_header_tag CLRPC_HEADER_TYPE;
typedef struct clrpc_header_tag CLRPC_COMMON_MEM_ATTR *CLRPC_HEADER_PTR_TYPE;

#define CLRPC_HEADER_VERSION	4
#define CLRPC_HEADER_SIZE		80

#define CLRPC_MUST_RECV_BODY_SIZE    (CLRPC_MUST_RECV_FRAG_SIZE - CLRPC_HEADER_SIZE)

/* packet types */

#define CLRPC_PT_REQUEST    0 /* client -> server */
#define CLRPC_PT_PING       1 /* client -> server */
#define CLRPC_PT_RESPONSE   2 /* server -> client */
#define CLRPC_PT_FAULT      3 /* server -> client */
#define CLRPC_PT_WORKING    4 /* server -> client */
#define CLRPC_PT_NOCALL     5 /* server -> client */
#define CLRPC_PT_REJECT     6 /* server -> client */
#define CLRPC_PT_ACK        7 /* client -> server */
#define CLRPC_PT_QUIT       8 /* client -> server */
#define CLRPC_PT_FACK       9 /* both directions  */
#define CLRPC_PT_QUACK      10 /* server -> client */
#define CLRPC_PT_MAX        10

#define CLRPC_PT_IS(ptype_, mask_) (((1 << (ptype_)) & (mask_)) != 0)

#define CLRPC_PT_IS_DATA(ptype_) \
	CLRPC_PT_IS(ptype_, \
		(1 << CLRPC_PT_REQUEST)  | \
		(1 << CLRPC_PT_RESPONSE) | \
		(1 << CLRPC_PT_FAULT) \
	)

#define CLRPC_PT_IS_CTOS(ptype_) \
	CLRPC_PT_IS(ptype_, \
		(1 << CLRPC_PT_REQUEST)  | \
		(1 << CLRPC_PT_PING)     | \
		(1 << CLRPC_PT_ACK)      | \
		(1 << CLRPC_PT_QUIT)     | \
		(1 << CLRPC_PT_FACK) \
	)

#define CLRPC_PT_IS_STOC(ptype_) \
	CLRPC_PT_IS(ptype_, \
		(1 << CLRPC_PT_RESPONSE) | \
		(1 << CLRPC_PT_FAULT)    | \
		(1 << CLRPC_PT_WORKING)  | \
		(1 << CLRPC_PT_NOCALL)   | \
		(1 << CLRPC_PT_REJECT)   | \
		(1 << CLRPC_PT_QUACK)    | \
		(1 << CLRPC_PT_FACK) \
	)

/* packet flags */

#define CLRPC_PF_FORWARDED   0x01 /* (client -> server) Packet was forwarded */
#define CLRPC_PF_LAST_FRAG   0x02 /* (both directions)  Packet is the last fragment */
#define CLRPC_PF_FRAG        0x04 /* (both directions)  Packet is a fragment */
#define CLRPC_PF_NO_FACK     0x08 /* (both directions)  Don't send a FACK for this FRAG */
#define CLRPC_PF_MAYBE       0x10 /* (client -> server) "maybe" request */
#define CLRPC_PF_IDEMPOTENT  0x20 /* (client -> server) "idempotent" request */
#define CLRPC_PF_BROADCAST   0x40 /* (client -> server) "broadcast" request */
#define CLRPC_PF_BLAST_OUTS  0x80 /* (client -> server) out's can be blasted */

/* packet flags2 */

#define CLRPC_PF2_FORWARDED_2     0x01 /* (client -> server) Packet is being forwarded in two pieces */
#define CLRPC_PF2_CANCEL_PENDING  0x02 /* (server -> client) Cancel was pending at call end */
#define CLRPC_PF2_RESERVED04      0x04
#define CLRPC_PF2_RESERVED08      0x08
#define CLRPC_PF2_RESERVED10      0x10
#define CLRPC_PF2_RESERVED20      0x20
#define CLRPC_PF2_RESERVED40      0x40
#define CLRPC_PF2_RESERVED80      0x80

/* ihint and ahint */

#define CLRPC_NO_HINT			((LSA_UINT16) 0xffff)

/*
 *  R P C _ I F _ V E R S _ M A J O R
 *  R P C _ I F _ V E R S _ M I N O R
 *
 * Macros to turn a 32 bit version number into its major and minor version
 * number components.
 */
#define CLRPC_IF_VERS_MAJOR(if_vers_) ((LSA_UINT16)((if_vers_) & 0xffff))
#define CLRPC_IF_VERS_MINOR(if_vers_) ((LSA_UINT16)((if_vers_) >> 16))

#define CLRPC_MAKE_IF_VERS(if_major_, if_minor_) \
	(LSA_UINT32)( \
		((if_major_) & 0xffff)  |  (((LSA_UINT32)(if_minor_)) << 16) \
	)


/*------------------------------------------------------------------------------
// DCE RPC data-representation
//----------------------------------------------------------------------------*/

#ifndef CLRPC_CFG_DREP_INT
# error "CLRPC_CFG_DREP_INT undefined in clrpc_cfg.h"
#endif
#ifndef CLRPC_CFG_DREP_CHAR
# error "CLRPC_CFG_DREP_CHAR undefined in clrpc_cfg.h"
#endif
#ifndef CLRPC_CFG_DREP_FLOAT
# error "CLRPC_CFG_DREP_FLOAT undefined in clrpc_cfg.h"
#endif

#define CLRPC_DREP_0		( (CLRPC_CFG_DREP_CHAR) | (CLRPC_CFG_DREP_INT) )
#define CLRPC_DREP_1		(CLRPC_CFG_DREP_FLOAT)
#define CLRPC_DREP_2		0 /* reserved, must be set to zero */

#define CLRPC_DREP_INT_DIFFERS(remote_drep_0) ((remote_drep_0 & 0xF0) != (CLRPC_DREP_0 & 0xF0))

/*------------------------------------------------------------------------------
// DCE RPC timeout constants
//----------------------------------------------------------------------------*/

/* DGCCALL.C line 1705
 * ==> inter_cancel_time ... 2sec
 *
 * DGCALL.C line 1937
 * ==> inter_quit_time ... 1sec
 *
 * DGCCALL.C line 2121
 * ==> max orphan time 3sec, thus 3 retransmissions
 * note: we use CLRPC_MAX_RTX_RETRY
 */

#define CLRPC_QUIT_TIMEOUT_MSEC  (1*1000) /* PNIO: 1sec */

/*------------------------------------------------------------------------------
 * DGCCALL.C line 2120
 * ==> delay sending ack
 */

#define CLRPC_FINAL_TIMEOUT_MSEC  (3*1000) /* at-most-once only */

/*------------------------------------------------------------------------------
 * DG.H line 875
 * backing off (*= 2) until max.
 * ==> initial ... 2sec
 * ==> max ....... 8sec, thus 3 retransmissions
 *
 * implementation says 1sec!?
 *
 * Microsoft clients give up after 6 retransmissions
 *
 */

#define CLRPC_RTX_TIMEOUT_MSEC  (2*1000) /* PNIO: 2sec */

#define CLRPC_MAX_RTX_RETRY  3 /* PNIO: 3 times */

/*------------------------------------------------------------------------------
 * must not ping too fast
 * Microsoft needs 400msec to answer the first epm-query
 */

#define CLRPC_PING_TIMEOUT_MSEC  (2*1000) /* PNIO: 2sec */

/*------------------------------------------------------------------------------
 * DGCCALL.C line 1797
 * ==> broadcast, wait 3sec for an answer
 * ==> AES Appendix K says 5sec
 */

#define CLRPC_BROADCAST_TIMEOUT_MSEC (3*1000)

/*------------------------------------------------------------------------------
 * lifetime of an endpoint-mapper context-handle (in millisec)
 */
#ifndef CLRPC_CFG_CTX_TIMEOUT_MSEC
# define CLRPC_CFG_CTX_TIMEOUT_MSEC			(30*1000)
#endif

/*------------------------------------------------------------------------------
// DCE RPC comparing counters
//----------------------------------------------------------------------------*/

/* OSF sources DG.H line 471ff
 *
 * Macros to help do A < B and A <= B compares on unsigned numbers in
 * general and specifically packet fragment and sequence numbers.  These
 * macros are useful when the numbers being compared are allowed to wrap
 * (e.g. we really want 0xfffffffe to be less than 0x00000002).
 *
 * To perform such a comparison, we count the fact that the number space
 * for a given data type / object is large enough that we can assert:
 * if the unsigned difference A - B is "negative", A is < B.  We define
 * "negative" as being a unsigned difference that when converted to
 * a signed type of the same precision yields a negative value.
 */

#define CLRPC_FRAGNUM_IS_LT(a_, b_)		(((LSA_INT16) ((a_) - (b_))) < 0)
#define CLRPC_FRAGNUM_IS_LTE(a_, b_)	(((LSA_INT16) ((a_) - (b_))) <= 0)

#define CLRPC_SERIAL_IS_LT(a_, b_)		(((LSA_INT16) ((a_) - (b_))) < 0)
#define CLRPC_SERIAL_IS_LTE(a_, b_)		(((LSA_INT16) ((a_) - (b_))) <= 0)

#define CLRPC_CMP16(a_, cmp_, b_)	(((LSA_INT16) ((a_) - (b_))) cmp_ 0)
#define CLRPC_CMP32(a_, cmp_, b_)	(((LSA_INT32) ((a_) - (b_))) cmp_ 0)

/*------------------------------------------------------------------------------
// DCE RPC, fack body
//----------------------------------------------------------------------------*/

struct clrpc_fack_body_tag {
	LSA_UINT8 vers;             /* Fack packet body version */
	LSA_UINT8 pad1;
	LSA_UINT16 window_size;     /* Sender's receive window size (in kilobytes) */
	LSA_UINT32 max_tsdu;        /* See above */
	LSA_UINT32 max_frag_size;   /* See above */
	LSA_UINT16 serial_num;      /* serial number of packet that induced this fack */
	LSA_UINT16 selack_len;      /* number of elements in the selack array    */
	LSA_UINT32 selack[1];       /* variable number of 32 bit selective ack bit masks */
};

typedef struct clrpc_fack_body_tag CLRPC_FACK_BODY_TYPE;
typedef struct clrpc_fack_body_tag CLRPC_COMMON_MEM_ATTR *CLRPC_FACK_BODY_PTR_TYPE;

#define CLRPC_FACK_BODY_VERS  1
#define CLRPC_FACK_BODY_SIZE  (1+1+2+4+4+2+2+4)

/*------------------------------------------------------------------------------
// DCE RPC, error body
// for FAULT and REJECT
//----------------------------------------------------------------------------*/

struct clrpc_error_body_tag {
	LSA_UINT32 nca_status; /* see CLRPC_NCA_XXX in clrpc_usr.h */
};

typedef struct clrpc_error_body_tag CLRPC_ERROR_BODY_TYPE;
typedef struct clrpc_error_body_tag CLRPC_COMMON_MEM_ATTR *CLRPC_ERROR_BODY_PTR_TYPE;

#define CLRPC_ERROR_BODY_SIZE  4

/*------------------------------------------------------------------------------
// DCE RPC, quit body
//----------------------------------------------------------------------------*/

struct clrpc_quit_body_tag {
	LSA_UINT32  vers;       /* quit body format version */
	LSA_UINT32  cancel_id;  /* id of a cancel-request event */
	/* end of version 0 format */
};

typedef struct clrpc_quit_body_tag CLRPC_QUIT_BODY_TYPE;
typedef struct clrpc_quit_body_tag CLRPC_COMMON_MEM_ATTR * CLRPC_QUIT_BODY_PTR_TYPE;

#define CLRPC_QUIT_BODY_VERS  0
#define CLRPC_QUIT_BODY_SIZE  (4+4)

/*------------------------------------------------------------------------------
// DCE RPC, quack body
//----------------------------------------------------------------------------*/

struct clrpc_quack_body_tag {
	LSA_UINT32  vers;       /* cancel-request body format version */
	LSA_UINT32  cancel_id;  /* id of a cancel-request event being ack'd */
	LSA_UINT8   server_is_accepting;
	/* end of version 0 format */
	LSA_UINT8   clrpc_padding[3]; /* AP00920933 */
};

typedef struct clrpc_quack_body_tag CLRPC_QUACK_BODY_TYPE;
typedef struct clrpc_quack_body_tag CLRPC_COMMON_MEM_ATTR *CLRPC_QUACK_BODY_PTR_TYPE;

#define CLRPC_QUACK_BODY_VERS  0
#define CLRPC_QUACK_BODY_SIZE_ONWIRE  (4+4+1 + 0) /* size on wire (see OSF sources DG.H line 651) */
#define CLRPC_QUACK_BODY_SIZE_PADDED  (4+4+1 + 3) /* padded */

/*------------------------------------------------------------------------------
// DCE RPC, context handle
// Network representation of a NIDL context handle.
//----------------------------------------------------------------------------*/

struct clrpc_context_handle_tag {
	LSA_UINT32       context_handle_attributes; /* zero! */
	CLRPC_UUID_TYPE  context_handle_uuid;
};

typedef struct clrpc_context_handle_tag CLRPC_NDR_CONTEXT_HANDLE_TYPE;

#define CLRPC_NDR_CONTEXT_HANDLE_SIZE  (4+16)

/*------------------------------------------------------------------------------
// Conversation Manager
//----------------------------------------------------------------------------*/

extern CLRPC_UUID_TYPE clrpc_conversation_manager_uuid; /* defined in clrpc_uuid.c */

extern CLRPC_UUID_TYPE clrpc_convc_uuid; /* defined in clrpc_uuid.c */

struct clrpc_way_req_tag {
	CLRPC_UUID_TYPE actuid;
	LSA_UINT32 boot_time;
};

typedef struct clrpc_way_req_tag CLRPC_WAY_REQ_TYPE;
typedef struct clrpc_way_req_tag CLRPC_COMMON_MEM_ATTR *CLRPC_WAY_REQ_PTR_TYPE;

#define CLRPC_WAY_REQ_SIZE  (16+4)

struct clrpc_way_rsp_tag {
	LSA_UINT32 actseq;
	LSA_UINT32 ncastat;
};

typedef struct clrpc_way_rsp_tag CLRPC_WAY_RSP_TYPE;
typedef struct clrpc_way_rsp_tag CLRPC_COMMON_MEM_ATTR *CLRPC_WAY_RSP_PTR_TYPE;

#define CLRPC_WAY_RSP_SIZE  (4+4)

struct clrpc_way2_rsp_tag {
	LSA_UINT32 actseq;
	CLRPC_UUID_TYPE casuid;
	LSA_UINT32 ncastat;
};

typedef struct clrpc_way2_rsp_tag CLRPC_WAY2_RSP_TYPE;
typedef struct clrpc_way2_rsp_tag CLRPC_COMMON_MEM_ATTR *CLRPC_WAY2_RSP_PTR_TYPE;

#define CLRPC_WAY2_RSP_SIZE  (4+16+4)

/*------------------------------------------------------------------------------
// Endpoint Mapper
//----------------------------------------------------------------------------*/

struct clrpc_epm_lookup_tag {
	LSA_UINT32			inquiry_type;

	LSA_UINT32			object_ref;
	CLRPC_UUID_TYPE		object;

	LSA_UINT32			if_id_ref;
	CLRPC_UUID_TYPE		if_id_uuid;
	LSA_UINT16			if_id_vers_major;
	LSA_UINT16			if_id_vers_minor;

	LSA_UINT32			vers_option;

	LSA_UINT32			entry_handle_ref;
	CLRPC_UUID_TYPE		entry_handle;

	LSA_UINT32			max_ents;
};

typedef struct clrpc_epm_lookup_tag CLRPC_EPM_LOOKUP_TYPE;
typedef struct clrpc_epm_lookup_tag CLRPC_COMMON_MEM_ATTR *CLRPC_EPM_LOOKUP_PTR_TYPE;

#define CLRPC_EPM_LOOKUP_SIZE  (4 + 4+16 + 4+16+2+2 + 4 + 4+16 + 4)

struct clrpc_epm_lookup_free_tag {
	LSA_UINT32			entry_handle_ref;
	CLRPC_UUID_TYPE		entry_handle;
};

typedef struct clrpc_epm_lookup_free_tag CLRPC_EPM_LOOKUP_FREE_TYPE;
typedef struct clrpc_epm_lookup_free_tag CLRPC_COMMON_MEM_ATTR *CLRPC_EPM_LOOKUP_FREE_PTR_TYPE;

#define CLRPC_EPM_LOOKUP_FREE_SIZE  (4+16)

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_PDU_H */
