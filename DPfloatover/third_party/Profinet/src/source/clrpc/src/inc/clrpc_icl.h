#ifdef CLRPC_CLI_H
#error "included twice"
#endif
#define CLRPC_CLI_H

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
/*  F i l e               &F: clrpc_icl.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile, Client Part                                         */
/*                                                                           */
/*****************************************************************************/

/*-----------------------------------------------------------------------------
// client part of channel type
//---------------------------------------------------------------------------*/

enum clrpc_client_lower_state_enum {
	CLRPC_CL_LOWER_CLOSED = 0,
	CLRPC_CL_LOWER_OPENING,
	CLRPC_CL_LOWER_READY,
	CLRPC_CL_LOWER_CLOSING
};

typedef struct clrpc_cl_channel_tag {

	CLRPC_DISPATCH_TAG dispatch_tag; /* see CLRPC_LL_SET_USER_ID_PTR() and CLRPC_CONTAINING_STRUCT() */

	/*
	 * note: after writing half of the module it was
	 *       straightened out that CLRPC will run in
	 *       a single threaded environment only where
	 *       all events (upper, lower, timer) come in
	 *       through the same mail-box (message-queue).
	 *
	 *       the entered/again flags and the queues are
	 *       rudiments of a multi-threaded design.
	 *
	 *       however, we keep the original design (without
	 *       enter/exit!) because the gain would be little.
	 */

	LSA_UINT16  entered;
	LSA_UINT16  again;

	struct {
		CLRPC_QUEUE_TYPE bindQ;
		CLRPC_QUEUE_TYPE unbindQ;
	} upper;

	struct {
		LSA_UINT16  state; /* see clrpc_client_lower_state_enum */

		CLRPC_LOWER_RQB_PTR_TYPE  rqb; /* an RQB for udp-open/close */
		CLRPC_LOWER_RQB_PTR_TYPE  done; /* udp-open/close */

		LSA_INT32  sock_fd; /* the socket's "file-descriptor" */

		LSA_UINT16  udp_port; /* bound local port, host_ip is stored in channel */

		LSA_UINT32  provideCnt; /* number of provided receive resources */

	} lower;

	LSA_UINT32  instanceCnt; /* number of client instances */

	CLRPC_HASH_TYPE actLUT; /* activity lookup table */

	CLRPC_UUID_TYPE cas_uuid; /* "client-address-space" UUID, see WAY2 response */

} CLRPC_CL_CHANNEL_TYPE;

/*-----------------------------------------------------------------------------
// client type
//---------------------------------------------------------------------------*/

enum clrpc_client_state_enum {
	CLRPC_CL_NO_INSTANCE = 0,
	/* don't change the order */
	CLRPC_CL_IDLE,
	CLRPC_CL_SEND,
	CLRPC_CL_RECV,
	CLRPC_CL_ORPHAN,
	/* final must be the last state */
	CLRPC_CL_FINAL
};

typedef struct clrpc_client_tag {
	CLRPC_DISPATCH_TAG dispatch_tag; /* first in struct!!! */

	CLRPC_CHANNEL_PTR_TYPE  channel;   /* backlink */
	LSA_UINT32              trace_idx; /* for "LSA index trace" */

	LSA_UINT16  state; /* see clrpc_client_state_enum */

	LSA_UINT16  entered; /* see note in the channel-declaration! */
	LSA_UINT16  again;

	struct {
		LSA_UINT32  host_ip;
		LSA_UINT16  udp_port;

		CLRPC_UUID_TYPE  if_uuid;
		LSA_UINT16       if_version_major;
		LSA_UINT16       if_version_minor;

		CLRPC_UUID_TYPE  object_uuid;

		/***/

		CLRPC_HASH_ENTRY_TYPE act; /* .uuid is the activity-uuid */
		LSA_UINT32       actseq;

		/***/

		LSA_UINT16  hintsUpdated; /* reset on each call */
		LSA_UINT16  ihint;
		LSA_UINT16  ahint;

		/***/

		LSA_UINT16  bound; /* 0=not bound */
					/* reset before broadcast calls */
					/* reset after calls with errors */

		LSA_UINT32  serverHost; /* valid if bound != 0 */
		LSA_UINT16  serverPort; /* valid if bound != 0 */

		LSA_UINT32  serverBootTime;
	} bnd;

	CLRPC_QUEUE_TYPE callQ;

	CLRPC_QUEUE_TYPE cancelQ;

	CLRPC_QUEUE_TYPE convQ;

	CLRPC_QUEUE_TYPE recvQ;

	CLRPC_QUEUE_TYPE sendCnfQ;

	CLRPC_ONESHOT_TYPE oneshot;
	LSA_UINT16         timeoutFlag;

	struct {
		CLRPC_UPPER_RQB_PTR_TYPE  curr;
		LSA_UINT32  nca_status;
		LSA_UINT16  rtxCnt;
		LSA_UINT16  workingCnt;
	} call;

	struct { /* "transmit queue" -- which is not a real queue (for now) */
		LSA_UINT8   baseFlags; /* idempotent, maybe, broadcast, frag */
		LSA_UINT16  serial;
		LSA_UINT16  sendSize; /* set from details */
		LSA_UINT16  sndWndCurr;
		LSA_UINT16  sndWndMax;
		LSA_UINT32  sndIdx;
		LSA_UINT32  ackIdx;
	} txq;

	struct { /* "receive queue" -- which is not a real queue (for now) */
		LSA_UINT16  recvFragmented;
		LSA_UINT16  nextFragnum;
		LSA_UINT16  highSerial;
		LSA_UINT32  rcvIdx;
		LSA_UINT16  allReceived;
	} rxq;

} CLRPC_CLIENT_TYPE, CLRPC_LOCAL_MEM_ATTR *CLRPC_CLIENT_PTR_TYPE;

/*-----------------------------------------------------------------------------
// client internal functions
//---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_init_channel(
	CLRPC_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_undo_init_channel(
	CLRPC_CHANNEL_PTR_TYPE channel
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_request(
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_channel_lower_done (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_cl_client_lower_done (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE instance_ptr,
    CLRPC_LOWER_RQB_PTR_TYPE  lower
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_channel_event(
	CLRPC_CHANNEL_PTR_TYPE channel
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_rebind_delayed(
	CLRPC_CLIENT_PTR_TYPE client, /* sanity checked in caller */
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_unbind_delayed(
	CLRPC_CLIENT_PTR_TYPE client, /* sanity checked in caller */
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
);

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_udp_recv_done(
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_LOWER_RQB_PTR_TYPE rb
);

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_provide(
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_LOWER_RQB_PTR_TYPE lower
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_event(
	CLRPC_CLIENT_PTR_TYPE client
);

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_bind(
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_bind_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_next(
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_next_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_break(
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR clrpc_cl_query_epm_break_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_init(
	CLRPC_CLIENT_PTR_TYPE  client
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_undo_init(
	CLRPC_CLIENT_PTR_TYPE  client
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_marshal(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_demarshal(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_context_free(
	CLRPC_CLIENT_PTR_TYPE  client,
	CLRPC_UPPER_RQB_PTR_TYPE upper
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_timeout(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	CLRPC_CLIENT_PTR_TYPE client /* user_ptr */
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
