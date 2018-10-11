#ifndef CLRPC_ISV_H
#define CLRPC_ISV_H

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
/*  F i l e               &F: clrpc_isv.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile for the server part                                  */
/*                                                                           */
/*****************************************************************************/

	/* configure the server send window size: count of pkt (not kilo-byte) */
	/* we don't check client's fack-body => set to 1 */
#define CLRPC_CFG_SV_SEND_WINDOW_SIZE		1


	/* timeout for self-destruction
	//  wait for final ack: (final-timeout + clrpc_oneshot.msec_per_tick)
	//	client wait for fack: (rtx-timeout + clrpc_oneshot.msec_per_tick) * nr-of-retry
	//	client want do send min 3 ping: (ping-timeout + clrpc_oneshot.msec_per_tick) * nr-of-pings
	//
	//	msec_per_tick: use CLRPC_QUIT_TIMEOUT_MSEC / 2
	*/
#define CLRPC_CFG_SCALL_DIE_TIMEOUT_1	(CLRPC_FINAL_TIMEOUT_MSEC + (CLRPC_QUIT_TIMEOUT_MSEC / 2))
#define CLRPC_CFG_SCALL_DIE_TIMEOUT_2	((CLRPC_RTX_TIMEOUT_MSEC + (CLRPC_QUIT_TIMEOUT_MSEC / 2)) * CLRPC_MAX_RTX_RETRY)
#define CLRPC_CFG_SCALL_DIE_TIMEOUT_3	((CLRPC_PING_TIMEOUT_MSEC + (CLRPC_QUIT_TIMEOUT_MSEC / 2)) * 3/*PNIO*/)

#define CLRPC_TIMEOUT_MAX(w1, w2)		((w1) > (w2) ? (w1) : (w2))

#define CLRPC_CFG_SCALL_DIE_TIMEOUT_MSEC \
	CLRPC_TIMEOUT_MAX (CLRPC_CFG_SCALL_DIE_TIMEOUT_1, \
		CLRPC_TIMEOUT_MAX (CLRPC_CFG_SCALL_DIE_TIMEOUT_2, CLRPC_CFG_SCALL_DIE_TIMEOUT_3))



	/* internal returns */
#define CLRPC_PROVIDE_LOWER					(CLRPC_OK + 1)
#define CLRPC_QUEUE_AGAIN					(CLRPC_OK + 2)


/*------------------------------------------------------------------------------
// server-specific-part for the common channel
//----------------------------------------------------------------------------*/

typedef struct clrpc_sv_channel_tag {
/*	LSA_HANDLE_TYPE				handle_lower; now use common handle_lower */
	CLRPC_UPPER_RQB_PTR_TYPE	upper;
} CLRPC_SV_CHANNEL_TYPE;


/*------------------------------------------------------------------------------
// SERVER
//----------------------------------------------------------------------------*/

	/* SERVER::state */
enum {
	CLRPC_SST_FREE = 0,
	CLRPC_SST_IDLE,
	CLRPC_SST_READY
};


typedef struct clrpc_if_object_tag {
	CLRPC_HASH_ENTRY_TYPE		hashed_object; /* .uuid is the UUID */
	CLRPC_UUID_TYPE				object_uuid; /* the object uuid registered within this interface */
	LSA_UINT8					annotation[CLRPC_MAX_ANNOTATION_SIZE];
	LSA_VOID_PTR_TYPE			usr_id;

	/* count running calls within this uuid */
	LSA_UINT16					ar_count_max; /* opcode: connect/release/read/write/control */
	LSA_UINT16					ar_count_curr;
	LSA_UINT16					read_implicit_max; /* opcode: read-implicit */
	LSA_UINT16					read_implicit_curr;
} CLRPC_IF_OBJECT_TYPE;

typedef struct clrpc_if_object_tag CLRPC_LOCAL_MEM_ATTR * CLRPC_IF_OBJECT_PTR_TYPE;


typedef struct clrpc_server_tag
{
	CLRPC_DISPATCH_TAG	dispatch_tag;		/* first in struct!!! */
	CLRPC_CHANNEL_PTR_TYPE					/* backlink to channel, who has registered this if-obj-uuid pair */
						channel;			/*		note: the epm-if is always registered in the EPM-channel */
	LSA_UINT32			trace_idx;			/* for "LSA index trace" */
	LSA_UINT16			state;				/* internal state */
	LSA_UINT16			id;					/* my own id, see trace-message sid(%u) */
	CLRPC_UPPER_RQB_PTR_TYPE
						upper;				/* the current upper rqb */

	CLRPC_UUID_TYPE		if_uuid;			/* the uuid of the interface */
	LSA_UINT16			if_major;			/* major-part of the interface-version */
	LSA_UINT16			if_minor;			/* minor-part of the interface-version */
	LSA_UINT16			port;				/* the used port */
	LSA_UINT16			is_endpoint_mapper;	/* > 0: is endpoint-mapper, n means ref-counter */
	LSA_UINT32			alloc_len;			/* this count of bytes has to alloc for every call.ind */
	LSA_UINT16			opnum_max;			/* range-check for opnums */
	LSA_UINT16			pnio_signature;		/* 1..PN IO function signature 0..else */
	LSA_INT32			sock_fd;			/* lower-layer send sock */
	LSA_UINT32			server_boot;		/* boot time of server-instance, for checking */
	LSA_UINT16			scall_cnt;			/* count of alloc ServerCall entries */
	LSA_UINT16			scall_next_id;		/* the next scall->id */
	LSA_UINT16			do_unregister;		/* 1..unregister in progress, 2..wait for all scalls */

	CLRPC_IF_OBJECT_TYPE
						obj;				/* the object uuid registered within this interface */

	CLRPC_QUEUE_TYPE	provided_queue;		/* queue with resources for call.indications */
	CLRPC_QUEUE_TYPE	send_rqb_queue;		/* queue with send-resourses */
	CLRPC_QUEUE_TYPE	send_scall_queue;	/* queue with scall waiting for send-resources */
} CLRPC_SERVER_TYPE;

typedef struct clrpc_server_tag CLRPC_LOCAL_MEM_ATTR * CLRPC_SERVER_PTR_TYPE;


	/* global server data */
extern CLRPC_SERVER_TYPE	clrpc_servers[CLRPC_CFG_MAX_SERVERS];	/* defined in clrpc_sv.c */
extern CLRPC_HASH_TYPE		clrpc_sc_hash;							/* defined in clrpc_sv.c */
extern CLRPC_HASH_TYPE		clrpc_ctx_hash;							/* defined in clrpc_ep.c */


/*------------------------------------------------------------------------------
// SCALL - one ServerCall
//----------------------------------------------------------------------------*/

	/* SCALL::state */
	/* bitmask because of: if (state & (s1 | s2)) ... */
#define CLRPC_CST_FREE				0x00
#define CLRPC_CST_INIT				0x01
#define CLRPC_CST_RECV				0x02
#define CLRPC_CST_WF_WAY			0x04
#define CLRPC_CST_WF_ANSWER			0x08
#define CLRPC_CST_SEND				0x10
#define CLRPC_CST_WF_ACK			0x20
#define CLRPC_CST_ORPHAN			0x40

	/* SCALL::way_state */
#define CLRPC_WAY_ST_UNKNOWN		0
#define CLRPC_WAY_ST_RUNNING		1
#define CLRPC_WAY_ST_OK				2
#define CLRPC_WAY_ST_ERROR			3


typedef struct clrpc_scall_tag CLRPC_LOCAL_MEM_ATTR * CLRPC_SCALL_PTR_TYPE;

typedef struct clrpc_scall_tag {
		/* LINKING - service: must first part, for send_scall_queue */
	CLRPC_LIST_ENTRY_TYPE		linkage;

		/* COMMON - part */
	LSA_VOID_PTR_TYPE			self;		/* for sanity check */
	CLRPC_SERVER_PTR_TYPE		server;		/* link to server/interface_uuid */
	LSA_UINT32					trace_idx;	/* for "LSA index trace" */
	LSA_UINT16					id;			/* my own id, see trace-message cid(%u) */
	LSA_UINT16					state;		/* interner state */
	LSA_UINT16					state_pre;	/* previous interner state */
	CLRPC_HASH_ENTRY_TYPE		hashed_act;	/* .uuid is the activity UUID */
	LSA_UINT32					seq;		/* sequence number */
	LSA_UINT16					ahint;		/* hash of actuid */
	LSA_UINT16					busy;		/* bool: something is running, see clrpc_cl_dispatch() */

	CLRPC_UPPER_RQB_PTR_TYPE	upper;		/* reserved-upper-rqb or answer-rqb */
	CLRPC_UPPER_SV_CALL_PTR_TYPE
								up_call;	/* shortcut to upper->data->sv.call */
	LSA_UINT8					upper_wf;	/* 1..wait for upper.rsp */
	LSA_UINT8					upper_drop;	/* 1..upper.rsp should drop, not send */

		/* TIMEOUT - part */
	LSA_UINT32					oneshot_queue;	/* queue for (one) timeout */
	CLRPC_ONESHOT_TYPE			oneshot;	/* the oneshot-timer */

		/* RECV - part */
	struct {
		CLRPC_QUEUE_TYPE		queue;		/* queue for lower recv */
		LSA_UINT32				host;		/* recv from host:port */
		LSA_UINT16				port;

		LSA_UINT16				fragnum;
		LSA_UINT32				cancel_id;	/* last recv cancel_id */

	} recv;

		/* SEND - part */
	struct {
		CLRPC_QUEUE_TYPE		answer_queue;	/* queue for upper answer */
		CLRPC_QUEUE_TYPE		send_queue;		/* queue for lower send resource */

		LSA_UINT16				in_queue;		/* 1 if this scall in the send-queue, 0 else  */
		LSA_UINT16				serial;			/* serial to send in next pkt */
		LSA_UINT16				ptype;			/* ptype to send */
		LSA_UINT16				next_state;		/* next state */
		LSA_UINT16				again;			/* have to send again 0|1 */

			/* frag data */
		LSA_UINT8				frag_flags;
		LSA_UINT16				frag_ack_fragnum;	/* last ack fragment */
		LSA_UINT16				frag_window_size;
		LSA_UINT16				frag_curr_fragnum;
		LSA_UINT16				frag_last_fragnum;

			/* fack data */
		LSA_UINT16				fack_serial;

			/* reject or fault data */
		LSA_UINT32				reject_fault_error;

			/* cancel quack */
		LSA_UINT32				cancel_id;

	} send;

		/* WAY - part */
	struct {
		LSA_UINT16				state;			/* state of way-callback */
		LSA_UINT16				timeout_cnt;	/* count of timeout for retransmit req */
		CLRPC_HASH_ENTRY_TYPE	hashed_act;		/* .uuid is the activity-uuid of way-cb */
		CLRPC_UUID_TYPE			cas_uuid;		/* the cas_uuid from the way2.rsp */
	} way;

} CLRPC_SCALL_TYPE;


/*------------------------------------------------------------------------------
// PKT - helper for the rpc-pkt-header
//----------------------------------------------------------------------------*/

#define CLRPC_PKT_GET_FROM_SEND(lower_, sock_send_, pkt_send_) \
	sock_send_ = & (lower_)->args.data; CLRPC_ASSERT (sock_send_); \
	pkt_send_ = (CLRPC_HEADER_PTR_TYPE)(sock_send_)->buffer_ptr;

#define CLRPC_PKT_GET_FROM_RECV(lower_, sock_recv_, pkt_recv_) \
	sock_recv_ = & (lower_)->args.data; CLRPC_ASSERT (sock_recv_); \
	pkt_recv_ = (CLRPC_HEADER_PTR_TYPE)(sock_recv_)->buffer_ptr;

/* clrpc_pkt_init(pkt_, ptype_) */

/* clrpc_pkt_is_sane(pkt_, recv_len_) */

#define CLRPC_PKT_SET_IF_ID(pkt_, uuid_) \
	CLRPC_MEMCPY (& (pkt_)->if_id, uuid_, sizeof((pkt_)->if_id))

#define CLRPC_PKT_SET_SERIAL(pkt_, serial_) \
	(pkt_)->serial_hi = (LSA_UINT8)((serial_) >> 8);\
	(pkt_)->serial_lo = (LSA_UINT8)(serial_)

#define CLRPC_PKT_GET_SERIAL(pkt_) \
	(LSA_UINT16)(((pkt_)->serial_hi << 8) + (pkt_)->serial_lo)

#define CLRPC_PKT_SET_ACTUID(pkt_, actuid_) \
	CLRPC_MEMCPY (& (pkt_)->actuid, actuid_, sizeof((pkt_)->actuid))

#define CLRPC_PKT_GET_BODY(pkt_, TYPE_) \
	((TYPE_)((pkt_) + 1))

#define CLRPC_UDP_SEND_INIT(udp_) { \
	(udp_)->sock_fd = CLRPC_LL_INVALID_SOCKET; \
	(udp_)->rem_addr.sin_family = SOCK_AF_INET; \
	(udp_)->rem_addr.sin_addr.S_un.S_addr = 0; \
	(udp_)->rem_addr.sin_port = 0; \
	(udp_)->data_length = 0; \
	}

#define CLRPC_UDP_IP(udp_) \
	((udp_)->rem_addr.sin_addr.S_un.S_addr)

#define CLRPC_UDP_PORT(udp_) \
	((udp_)->rem_addr.sin_port)

/* clrpc_pkt_get_semantics(pkt_) */

/* clrpc_pkt_set_fack_body (pkt_, window_size_, serial_num_) */

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_init (
	CLRPC_HEADER_PTR_TYPE  hdr,
	LSA_UINT16  ptype,
	CLRPC_UUID_PTR_TYPE  if_id,
	LSA_UINT32  if_vers,
	LSA_UINT16  opnum,
	CLRPC_UUID_PTR_TYPE  obj_uuid,
	CLRPC_UUID_PTR_TYPE  actuid,
	LSA_UINT32  seq
	);
LSA_UINT8  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_get_semantics (
	CLRPC_HEADER_PTR_TYPE  pkt
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pkt_send_or_queue (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_SCALL_PTR_TYPE  scall
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pkt_send (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  send
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_set_fack_body (
	CLRPC_HEADER_PTR_TYPE  pkt_send,
	LSA_UINT16  window_size,
	LSA_UINT16  serial_num
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_pkt_set_error_body (
	CLRPC_HEADER_PTR_TYPE  pkt_send,
	LSA_UINT32  error_code
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_reject (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT32  error_code
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_nocall (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT16  last_known_fragnum
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_send_nocall_fack_body (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  recv,
	LSA_UINT16  last_known_fragnum,
	LSA_UINT16  window_size,
	LSA_UINT16  serial_num
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_provide_enqueue (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UPPER_RQB_PTR_TYPE  *upper
	);
CLRPC_UPPER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_provide_dequeue (
	CLRPC_SERVER_PTR_TYPE  server
	);

CLRPC_IF_OBJECT_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_if_obj_lookup (
	CLRPC_SERVER_PTR_TYPE  server,
	CLRPC_UUID_PTR_TYPE  object_uuid
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_event_send_done (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_ll_recv_provide (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  CLRPC_LOWER_IN_FCT_ATTR
clrpc_sv_scall_free (
	CLRPC_SERVER_PTR_TYPE  server,
    CLRPC_SCALL_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR * scall
	);

/*----------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_init (
	LSA_VOID
);
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_undo_init (
	LSA_VOID
);
LSA_UINT32 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_fetch_alloc_len (
	CLRPC_HEADER_PTR_TYPE  pkt
);
LSA_UINT32 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_demarshal (
	CLRPC_SCALL_PTR_TYPE scall,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);
LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_marshal (
	CLRPC_SCALL_PTR_TYPE scall,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_init (
	LSA_VOID
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_undo_init(
    LSA_VOID
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_init_channel (
	CLRPC_CHANNEL_PTR_TYPE channel
	);
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_undo_init_channel (
	CLRPC_CHANNEL_PTR_TYPE channel
	);
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_internal (
	CLRPC_UPPER_RQB_PTR_TYPE	rb_register /* CLRPC_OPC_SV_REGISTER_IF_OBJ or CLRPC_OPC_EPM_START */
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_if_obj (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	);
LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_unregister_internal (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_unregister_if_obj (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_register_object (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_unregister_object (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_call_ind_provide (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sv_op_call_response (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_UPPER_RQB_PTR_TYPE  rb
	);

/*----------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_oneshot_callback (
	CLRPC_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE user_ptr
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_opnum_count_decr (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_UPPER_RQB_PTR_TYPE  upper
	);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_if_obj_opnum_count_incr (
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj,
	LSA_UINT16  has_pnio_signature,
	LSA_UINT16  opnum
	);
    
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_if_obj_opnum_count_decr (
	CLRPC_IF_OBJECT_PTR_TYPE  if_obj,
	LSA_UINT16  has_pnio_signature,
	LSA_UINT16  opnum
	);


LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_wf_ack (
	CLRPC_SCALL_PTR_TYPE  scall
	);
LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sc_do_orphan (
	CLRPC_SCALL_PTR_TYPE  scall
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_sc_dispatch (
	CLRPC_SCALL_PTR_TYPE  scall,
	CLRPC_UPPER_RQB_PTR_TYPE  upper,
	CLRPC_LOWER_RQB_PTR_TYPE  lower,
	LSA_UINT32  oneshot,
	CLRPC_LOWER_RQB_PTR_TYPE  send
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_callback (
	CLRPC_SCALL_PTR_TYPE  scall,
	LSA_UINT16  response,
	CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR  *rb_ptr
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_provide (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_SERVER_PTR_TYPE  server,
	LSA_UINT16  ind_provide_cnt
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_unregister_server (
	CLRPC_SERVER_PTR_TYPE  server
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_op_epm_start (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	);
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_op_epm_stop (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_ISV_H */
