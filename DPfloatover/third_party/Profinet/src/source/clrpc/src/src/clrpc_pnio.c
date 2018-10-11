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
/*  F i l e               &F: clrpc_pnio.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PN IO signature and Context Handle things                                */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    40
#define LTRC_ACT_MODUL_ID  40

#include "clrpc_inc.h"
#include "clrpc_int.h" /* prototypes in clrpc_icl.h and clrpc_isv.h */

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*
 * PDUs moved to clrpc_pdu.h
 */

/*===========================================================================*/
#if CLRPC_CFG_MAX_CLIENTS
/*===========================================================================*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_init(
	CLRPC_CLIENT_PTR_TYPE  client
) {
	CLRPC_LOG_FCT("clrpc_cl_pnio_init")

	LSA_UNUSED_ARG(client);
	/* nothing by now */
}

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_undo_init(
	CLRPC_CLIENT_PTR_TYPE  client
) {
	CLRPC_LOG_FCT("clrpc_cl_pnio_undo_init")

	LSA_UNUSED_ARG(client);
	/* nothing by now */
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_marshal(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_cl_pnio_marshal")
	CLRPC_PNIO_IN_ARGS_PTR_TYPE sig; /* see packing-checks in clrpc_check_sizes() */

	LSA_UNUSED_ARG(client);

	CLRPC_ASSERT(upper);
	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CALL);
	CLRPC_ASSERT(upper->args.cl.call->pnio_signature);

	CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ndr_len(%u)", upper->args.cl.call->ndr_len
		);

	/***/

	if( upper->args.cl.call->ndr_len < CLRPC_PNIO_OFFSET ) {
		CLRPC_CL_TRACE_01 (client->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ndr_len(%u) too short"
			, upper->args.cl.call->ndr_len
			);
		return CLRPC_ERR_PARAM;
	}

	sig = (CLRPC_PNIO_IN_ARGS_PTR_TYPE)upper->args.cl.call->ndr_data; /* shortcut to "signature" */

	/* marshal the in-args */

	sig->args_max = upper->args.cl.call->alloc_len - CLRPC_PNIO_OFFSET;
	sig->args_len = upper->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	sig->args.maximum_count = sig->args_max; /* same as args_max! */
	sig->args.offset = 0;
	sig->args.actual_count = sig->args_len; /* same as args_len! */

	return CLRPC_OK;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_pnio_demarshal(
	CLRPC_CLIENT_PTR_TYPE client,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_cl_pnio_demarshal")
	CLRPC_PNIO_OUT_ARGS_PTR_TYPE sig; /* see packing-checks in clrpc_check_sizes() */

	LSA_UNUSED_ARG(client);

	CLRPC_ASSERT(upper);
	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_CALL);
	CLRPC_ASSERT(upper->args.cl.call->pnio_signature);

	CLRPC_CL_TRACE_01(client->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ndr_len(%u)", upper->args.cl.call->ndr_len
		);

	if( upper->args.cl.call->ndr_len < CLRPC_PNIO_OFFSET ) {
		CLRPC_CL_TRACE_01 (client->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ndr_len(%u) too short"
			, upper->args.cl.call->ndr_len
			);
		return CLRPC_ERR_PNIO_OUT_ARGS; /* too short */
	}

	/* enough data for signature */

	sig = (CLRPC_PNIO_OUT_ARGS_PTR_TYPE)upper->args.cl.call->ndr_data; /* shortcut to "signature" */

	/* swap if DREP different */

	if( CLRPC_DREP_INT_DIFFERS(upper->args.cl.call->drep.drep_int) ) {

		CLRPC_SWAP_U32(&sig->pnio_status);
		CLRPC_SWAP_U32(&sig->args_len);

		CLRPC_SWAP_U32(&sig->args.maximum_count);
		CLRPC_SWAP_U32(&sig->args.offset);
		CLRPC_SWAP_U32(&sig->args.actual_count);
	}

	/* consistency check */

	if( ! (
		CLRPC_PNIO_OFFSET + sig->args_len == upper->args.cl.call->ndr_len
		&& sig->args.offset == 0
		&& sig->args.actual_count == sig->args_len
		&& CLRPC_PNIO_OFFSET + sig->args.maximum_count == upper->args.cl.call->alloc_len /* server must return what we said */
	) ) {
		CLRPC_CL_TRACE_00 (client->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "inconsistent"
			);
		return CLRPC_ERR_PNIO_OUT_ARGS; /* inconsistent */
	}

	/* demarshal the out-args */

	upper->args.cl.call->pnio_status = sig->pnio_status;

	return CLRPC_OK;
}

/*===========================================================================*/
#endif
/*===========================================================================*/
#if CLRPC_CFG_MAX_SERVERS
/*===========================================================================*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_init (
	LSA_VOID
) {
	/* nothing by now */
}

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_undo_init (
	LSA_VOID
) {
	/* nothing by now */
}

/*---------------------------------------------------------------------------*/

/*
	Error: 0, see clrpc_sc_do_recv()
	OK:    else
*/
LSA_UINT32 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_fetch_alloc_len (
	CLRPC_HEADER_PTR_TYPE  pkt
	)
{
	CLRPC_LOG_FCT("clrpc_sv_pnio_fetch_alloc_len")
	CLRPC_PNIO_IN_ARGS_PTR_TYPE  sig;
	LSA_UINT32  alloc_size;
	LSA_UINT32  args_len, a_offset, a_actual_count, a_maximum_count;

	CLRPC_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "pkt.len(%u)", pkt->len
		);

		/* enough data for signature */
	if (pkt->len < CLRPC_PNIO_OFFSET ) {
		CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "pkt.len(%u) too short"
			, pkt->len
			);
		return 0;
	}

		/* must first fragment */
	if ((pkt->flags & CLRPC_PF_FRAG)  &&  pkt->fragnum != 0) {
		CLRPC_SV_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
			, "fragnum(%u) != 0, drop!"
			, pkt->fragnum
			);
		return 0;
	}

	sig = (CLRPC_PNIO_IN_ARGS_PTR_TYPE)(pkt + 1); /* shortcut to "signature" */

	alloc_size = sig->args_max;

	args_len = sig->args_len;
	a_offset = sig->args.offset;
	a_actual_count = sig->args.actual_count;
	a_maximum_count = sig->args.maximum_count;

		/* swap if DREP different */
	if (CLRPC_DREP_INT_DIFFERS (pkt->drep[0])) {

		CLRPC_SWAP_U32 (& alloc_size);

		CLRPC_SWAP_U32 (& args_len);
		CLRPC_SWAP_U32 (& a_offset);
		CLRPC_SWAP_U32 (& a_actual_count);
		CLRPC_SWAP_U32 (& a_maximum_count);

	}

		/* consistency check */
	if (! (a_offset == 0 && a_actual_count == args_len && a_maximum_count == alloc_size)) {
		CLRPC_SV_TRACE_05 (0, LSA_TRACE_LEVEL_UNEXP
			, "inconsistent a_offset(%u) act_count(%u) args_len(%u) max_count(%u) alloc_size(%u)"
			, a_offset, a_actual_count, args_len, a_maximum_count, alloc_size
			);
		return 0;
	}


	alloc_size += CLRPC_PNIO_OFFSET;

	CLRPC_SV_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "alloc_size(%u)", alloc_size
		);

	return alloc_size;
}

/*---------------------------------------------------------------------------*/

LSA_UINT32 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_demarshal (
	CLRPC_SCALL_PTR_TYPE scall,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_sv_pnio_demarshal")
	CLRPC_PNIO_IN_ARGS_PTR_TYPE sig; /* see packing-checks in clrpc_check_sizes() */

	LSA_UNUSED_ARG(scall);

	CLRPC_ASSERT(upper);
	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_SV_CALL_INDICATION);

	CLRPC_SV_TRACE_01(scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ndr_len(%u)", upper->args.sv.call->ndr_len
		);

	if( upper->args.sv.call->ndr_len < CLRPC_PNIO_OFFSET ) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ndr_len(%u) too short"
			, upper->args.sv.call->ndr_len
			);
		return CLRPC_NCA_PROTO_ERROR; /* too short,  AP00651106: CLRPC_NCA_PROTO_ERROR instead of CLRPC_NCA_UNSPECIFIC_REJECT */
	}

	/* enough data for signature */

	sig = (CLRPC_PNIO_IN_ARGS_PTR_TYPE)upper->args.sv.call->ndr_data; /* shortcut to "signature" */

	/* swap if DREP different */

	if( CLRPC_DREP_INT_DIFFERS(upper->args.sv.call->drep.drep_int) ) {

		CLRPC_SWAP_U32(&sig->args_max);
		CLRPC_SWAP_U32(&sig->args_len);

		CLRPC_SWAP_U32(&sig->args.maximum_count);
		CLRPC_SWAP_U32(&sig->args.offset);
		CLRPC_SWAP_U32(&sig->args.actual_count);
	}

	/* consistency check */

	if( ! (
		CLRPC_PNIO_OFFSET + sig->args_len == upper->args.sv.call->ndr_len
		&& sig->args.offset == 0
		&& sig->args.actual_count == sig->args_len
		&& sig->args.maximum_count == sig->args_max
	) ) {
		CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "inconsistent"
			);
		return CLRPC_NCA_PROTO_ERROR; /* inconsistent,  AP00651106: CLRPC_NCA_PROTO_ERROR instead of CLRPC_NCA_UNSPECIFIC_REJECT */
	}

	/* demarshal the in-args */

	if( upper->args.sv.call->alloc_len > CLRPC_PNIO_OFFSET + sig->args_max ) {
		/* report min(local-alloc-len, client-buffer-size) */
		upper->args.sv.call->alloc_len = CLRPC_PNIO_OFFSET + sig->args_max;
	}

	/***/

	upper->args.sv.call->pnio_status = CLRPC_PNIO_STATUS_INVALID; /* init out-arg, may be helpful for debugging */

	return 0; /* nca_status */
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_sv_pnio_marshal (
	CLRPC_SCALL_PTR_TYPE scall,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_sv_pnio_marshal")
	CLRPC_PNIO_OUT_ARGS_PTR_TYPE sig; /* see packing-checks in clrpc_check_sizes() */

	LSA_UNUSED_ARG(scall);

	CLRPC_ASSERT(upper);
	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_SV_CALL_RESPONSE);

	CLRPC_SV_TRACE_01(scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ndr_len(%u)", upper->args.sv.call->ndr_len
		);

	/***/

	if( upper->args.sv.call->ndr_len < CLRPC_PNIO_OFFSET ) {
		CLRPC_SV_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ndr_len(%u) too short"
			, upper->args.sv.call->ndr_len
			);
		return 1; /* error */
	}

	sig = (CLRPC_PNIO_OUT_ARGS_PTR_TYPE)upper->args.sv.call->ndr_data; /* shortcut to "signature" */
	if (CLRPC_PTR_IS_NULL (sig)) {
		CLRPC_SV_TRACE_00 (scall->trace_idx, LSA_TRACE_LEVEL_FATAL, "ndr_data == LSA_NULL");
		return 1;
	}

	/* marshal the out-args */

	sig->pnio_status = upper->args.sv.call->pnio_status;

	sig->args_len = upper->args.sv.call->ndr_len - CLRPC_PNIO_OFFSET;

	sig->args.maximum_count = sig->args.maximum_count; /* don't change, cannot use alloc_len, see above */
	sig->args.offset = 0;
	sig->args.actual_count = sig->args_len; /* same as args_len! */

	return 0; /* ok */
}

/*===========================================================================*/
#endif
/*===========================================================================*/

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
