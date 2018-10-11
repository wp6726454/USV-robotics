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
/*  F i l e               &F: clrpc_cl4.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CLRPC-Client, Part 4                                   */
/*                                                                           */
/*  cited OSF sources DCE 1.1 are available from http://www.opengroup.org    */
/*                                                                           */
/*    - Endpoint Mapper Query                                                */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    23
#define LTRC_ACT_MODUL_ID  23

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


#if CLRPC_CFG_MAX_CLIENTS /* compile only, if client-part is configured */

    
/*=============================================================================
//=== QUERY ENDPOINT-MAPPER ===================================================
//===========================================================================*/

/*
 * see OSF documentation
 *
 * Chapter 14 "Transfer Syntax"
 * Appendix L "Protocol Tower Encoding"
 * Appendix I "Protocol Identifiers"
 *
 * for "how to" marshal and de-marshal
 *

struct _twr_r {
	LSA_UINT32 tower_length;
	LSA_UINT8  tower_octet_string[1];
};

struct _ept_entry_t {
	CLRPC_UUID_TYPE object;
	LSA_UINT32 ref3; // embedded pointer
	struct _twr_r * twr; // deferred after the end!
	char annotation[64];
};

[idempotent]
void ept_lookup(
	[in]            handle_t            h,
	[in]            unsigned32          inquiry_type,
	[in]            uuid_p_t            object,
	[in]            rpc_if_id_p_t       interface_id,
	[in]            unsigned32          vers_option,
	[in, out]       ept_lookup_handle_t *entry_handle, ==> context handle!!!
	[in]            unsigned32          max_ents,
	[out]           unsigned32          *num_ents,
	[out, length_is(*num_ents), size_is(max_ents)]
					ept_entry_t         entries[],
	[out]           error_status_t      *status
);


void ept_lookup_handle_free(
    [in]            handle_t            h,
    [in, out]       ept_lookup_handle_t *entry_handle,
    [out]           error_status_t      *status
);

*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_bind(
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_bind")

	CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE qry = upper->args.cl.query_epm; /* shortcut */

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND);

	/***/

	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(qry));

	qry->state = CLRPC_QUERY_EPM_BIND;

	/***/

	/*
	 * user sets host, port and timeout
	 */

	qry->u.bind.if_uuid = clrpc_endpoint_mapper_uuid;

	qry->u.bind.if_version_major = 3;

	qry->u.bind.if_version_minor = 0;

	qry->u.bind.object_uuid = clrpc_uuid_nil_uuid;

	/***/

	if( CLRPC_PTR_IS_NULL(qry->ndr_data) ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "no ndr_data"
			);
		return CLRPC_ERR_OUT_ARGS;
	}

	if( ((LSA_UINT32)qry->ndr_data & 3) != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "ndr_data not long-aligned"
			);
		return CLRPC_ERR_OUT_ARGS;
	}

	if( qry->alloc_len < CLRPC_QUERY_EPM_NDR_DATA_SIZE ) {
		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "alloc_len(%u) < CLRPC_QUERY_EPM_NDR_DATA_SIZE(%u)"
			, qry->alloc_len, CLRPC_QUERY_EPM_NDR_DATA_SIZE
			);
		return CLRPC_ERR_OUT_ARGS;
	}

	/* continues like a "normal" bind() */

	return CLRPC_OK;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_bind_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_bind_done")

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BIND);

	/***/

	CLRPC_ASSERT(upper->args.cl.query_epm->state == CLRPC_QUERY_EPM_BIND);

	upper->args.cl.query_epm->state = CLRPC_QUERY_EPM_LOOKUP_FIRST;

	return response;
}

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_next(
	CLRPC_UPPER_RQB_PTR_TYPE upper /* sanity checked in caller */
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_next")

	CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE qry = upper->args.cl.query_epm; /* shortcut */
	LSA_UINT8 * ptr;

	CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "state(%u)", qry->state
	);

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_NEXT);

	/***/

	CLRPC_ASSERT(qry);

	CLRPC_ASSERT(qry->u.call.client_id);

	/***/

	if( qry->state == CLRPC_QUERY_EPM_LOOKUP_FIRST ) {

		qry->ctx_uuid = clrpc_uuid_nil_uuid;
	}

	CLRPC_ASSERT(
		qry->state == CLRPC_QUERY_EPM_LOOKUP_FIRST
		|| qry->state == CLRPC_QUERY_EPM_LOOKUP_NEXT
	);

	/***/

	qry->u.call.opnum = 2; /* ept_lookup() */

	qry->u.call.semantics = CLRPC_SEMANTICS_IDEMPOTENT;

	qry->u.call.pnio_signature = 0; /* not "PN IO signature call" */

	qry->u.call.appl_timeout1sec = 30; /* 30sec should be enough */

	/* see checks in clrpc_cl_query_epm_bind() */

#if CLRPC_DEBUG
	CLRPC_MEMSET(qry->ndr_data, 0xCC, qry->alloc_len); /* to find uninitialized things */
#endif

	qry->u.call.alloc_len = qry->alloc_len;

	ptr = qry->u.call.ndr_data = qry->ndr_data;

	CLRPC_ASSERT(qry->u.call.alloc_len >= CLRPC_QUERY_EPM_NDR_DATA_SIZE);
	CLRPC_ASSERT(CLRPC_PTR_IS_NOT_NULL(ptr));
	CLRPC_ASSERT(((LSA_UINT32)ptr & 3) == 0);

	/***/

	/* marshal the in-params */

	CLRPC_MARSHAL32(&ptr, 0); /* inquiry_type = all elements */

	CLRPC_MARSHAL32(&ptr, 1); /* ref 1 */

	CLRPC_MARSHAL_UUID(&ptr, &clrpc_uuid_nil_uuid); /* object uuid */

	CLRPC_MARSHAL32(&ptr, 2); /* ref 2 */

	CLRPC_MARSHAL_UUID(&ptr, &clrpc_uuid_nil_uuid); /* interface uuid */

	CLRPC_MARSHAL16(&ptr, 0); /* vers maj */

	CLRPC_MARSHAL16(&ptr, 0); /* vers min */

	CLRPC_MARSHAL32(&ptr, 1); /* vers_option = all versions */

	CLRPC_MARSHAL32(&ptr, 0); /* context zero */

	CLRPC_MARSHAL_UUID(&ptr, &qry->ctx_uuid); /* context uuid */

	CLRPC_MARSHAL32(&ptr, 1); /* max_ents */

	/***/

	qry->u.call.ndr_len = ptr - qry->u.call.ndr_data;

	CLRPC_ASSERT(qry->u.call.ndr_len <= qry->u.call.alloc_len);

	/* continues like a "normal" call() */
}

/*---------------------------------------------------------------------------*/


LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_next_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_next_done")
	CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE qry = upper->args.cl.query_epm; /* shortcut */
	LSA_UINT32 bytes_left;
	LSA_UINT8 * ptr;

	CLRPC_ASSERT(qry);

	CLRPC_CL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u), nca_status(%#x), ndr_len(%u), state(%u)"
		, response
		, qry->u.call.nca_status
		, qry->u.call.ndr_len
		, qry->state
	);

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_NEXT);
	CLRPC_ASSERT(upper->args.cl.call->pnio_signature == 0);

	CLRPC_ASSERT(qry->u.call.client_id);

	CLRPC_ASSERT(
		qry->state == CLRPC_QUERY_EPM_LOOKUP_FIRST
		|| qry->state == CLRPC_QUERY_EPM_LOOKUP_NEXT
	);

	/***/

	qry->epm_entry.valid = 0; /* assume invalid */

	qry->epm_entry.more = 0; /* assume no more entries */

	/***/

	if( response != CLRPC_OK ) {

		if( response == CLRPC_ERR_OUT_ARGS ) {
			/* not all out-args received, cannot decode! */
			CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
				, "out-args too big"
				);
			qry->epm_entry.more = 1; /* try again */
			return CLRPC_OK;
		}

		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "call failed, rsp(%u), nca_status(%#x)"
			, response
			, qry->u.call.nca_status
			);

		return response;
	}

	/***/

	ptr = qry->u.call.ndr_data;

	CLRPC_ASSERT(((LSA_UINT32)ptr & 3) == 0); /* buffer must be long-aligned (at least) */

	bytes_left = qry->u.call.ndr_len;

	if( bytes_left < (20 + 4 + 4+4+4 + 4) || (bytes_left & 3) != 0 ) {
too_few_ndrdata:
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "too few ndrdata (%u) or not 0 mod 4"
			, qry->u.call.ndr_len
			);
		return CLRPC_ERR_DECODE;
	}

	/* unmarshal the out-params */

	{
	LSA_UINT32 status = CLRPC_GET32_ALIGNED(ptr + bytes_left - 4); /* see if() above */

	if( CLRPC_DREP_INT_DIFFERS(qry->u.call.drep.drep_int) ) {
		CLRPC_SWAP_U32(&status);
	}

	bytes_left -= 4;

	if( status != 0 ) {
		if( status == CLRPC_EPT_NOT_REGISTERED ) { /* the end */

			/* note that the server released the context handle (null-uuid) */

last_entry:

			qry->state = CLRPC_QUERY_EPM_STOP;

			/* this was the last entry */
			CLRPC_ASSERT( ! qry->epm_entry.valid);
			CLRPC_ASSERT( ! qry->epm_entry.more);
			return CLRPC_OK; /* complete */
		}
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "status(%#x)"
			, status
			);
		return CLRPC_ERR_DECODE;
	}

	{
	LSA_UINT32 ctx_zero;
	CLRPC_UUID_TYPE ctx_uuid;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &ctx_zero, qry, too_few_ndrdata);

	CLRPC_UNMARSHAL_UUID(&ptr, &bytes_left, &ctx_uuid, qry, too_few_ndrdata);

	if( ctx_zero != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong ctx-handle"
			);
		return CLRPC_ERR_DECODE;
	}

#if 0 /*noisy*/
	CLRPC_CL_TRACE_11(0, LSA_TRACE_LEVEL_CHAT
		, "ctx-handle(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ctx_uuid.time_low, ctx_uuid.time_mid, ctx_uuid.time_hi_and_version
		, ctx_uuid.clock_seq_hi_and_reserved, ctx_uuid.clock_seq_low
		, ctx_uuid.node[0], ctx_uuid.node[1], ctx_uuid.node[2]
		, ctx_uuid.node[3], ctx_uuid.node[4], ctx_uuid.node[5]
		);
#endif

	if( qry->state == CLRPC_QUERY_EPM_LOOKUP_FIRST ) {

		CLRPC_ASSERT(clrpc_uuid_is_nil(&qry->ctx_uuid));

		qry->ctx_uuid = ctx_uuid; /* remember the context handle */

		qry->state = CLRPC_QUERY_EPM_LOOKUP_NEXT;
	}

	if( clrpc_uuid_is_nil(&ctx_uuid) ) {
		/*
		 * ever reached? see 'status' above
		 */
		goto last_entry;
	}
	else {
		qry->epm_entry.more = 1;
	}
	}
	}

	/***/

	{
	LSA_UINT32 num_ents;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &num_ents, qry, too_few_ndrdata);

	if( num_ents != 1 ) {
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong num_ents (%u)"
			, num_ents
			);
		return CLRPC_ERR_DECODE;
	}
	}

	{
	LSA_UINT32 max, ofs, cnt;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &max, qry, too_few_ndrdata); 
	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &ofs, qry, too_few_ndrdata);

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &cnt, qry, too_few_ndrdata);

	/*CLRPC_CL_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "unmarshal: max(%u) ofs(%u), cnt(%u)", max, ofs, cnt);*/

	if( ofs != 0 || cnt != 1 ) {
		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong entries-ofs/cnt (%u/%u)"
			, ofs
			, cnt
			);
		return CLRPC_ERR_DECODE;
	}
	}

	/* object */

	CLRPC_UNMARSHAL_UUID(&ptr, &bytes_left, &qry->epm_entry.object_uuid, qry, too_few_ndrdata);

	/* tower pointer reference (embedded pointer, deferred!) */

	{
	LSA_UINT32 ref3;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &ref3, qry, too_few_ndrdata);

	if( ref3 != 3 ) {
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong ref3 (%u)"
			, ref3
			);
		return CLRPC_ERR_DECODE;
	}
	}

	/* annotation */

	{
	LSA_UINT32 ofs, cnt;
	LSA_UINT8 * ann;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &ofs, qry, too_few_ndrdata);

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &cnt, qry, too_few_ndrdata);

	if( ofs != 0 || cnt > 64 ) {
		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong annotation ofs/cnt (%u/%u)"
			, ofs
			, cnt
			);
		return CLRPC_ERR_DECODE;
	}
	if( qry->u.call.drep.drep_char != CLRPC_DREP_CHAR_ASCII ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "annotation not ascii"
			);

skip_entry:

		/* user must/should skip this entry ! */
		CLRPC_ASSERT( ! qry->epm_entry.valid);
		CLRPC_ASSERT( qry->epm_entry.more);
		return CLRPC_OK;
	}

	CLRPC_UNMARSHAL_BUF(&ptr, &bytes_left, &ann, cnt, too_few_ndrdata);

	if( ann[cnt - 1] != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "annotation not zero-terminated"
			);
		return CLRPC_ERR_DECODE;
	}

	CLRPC_MEMCPY(qry->epm_entry.annotation, ann, cnt);
	}

	CLRPC_ALIGN(&bytes_left, &ptr, LSA_UINT32);

	{
	LSA_UINT32 twrlen, max;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &twrlen, qry, too_few_ndrdata);

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &max , qry, too_few_ndrdata);

	if( twrlen != max ) {
		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong tower len/max (%u/%u)"
			, twrlen
			, max
			);
		return CLRPC_ERR_DECODE;
	}
	}

	{
	LSA_UINT16 floor_max, floor_idx;

	/* note: floor-encoding is in little-endian!!! */

	CLRPC_UNMARSHAL16_LITTLE(&ptr, &bytes_left, &floor_max, too_few_ndrdata);

	if( floor_max != 5 ) {
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "not 5 floors (%u)"
			, floor_max
			);
		goto skip_entry;
	}

	for( floor_idx = 0; floor_idx < floor_max; ++floor_idx ) {
		LSA_UINT16 lhs_cnt, rhs_cnt;
		LSA_UINT8 *lhs_dat, *rhs_dat;

		/* each floor is has format: left-hand-side count, data ; right-hand-side count, data */

		CLRPC_UNMARSHAL16_LITTLE(&ptr, &bytes_left, &lhs_cnt, too_few_ndrdata);

		CLRPC_UNMARSHAL_BUF(&ptr, &bytes_left, &lhs_dat, lhs_cnt, too_few_ndrdata);

		CLRPC_UNMARSHAL16_LITTLE(&ptr, &bytes_left, &rhs_cnt, too_few_ndrdata);

		CLRPC_UNMARSHAL_BUF(&ptr, &bytes_left, &rhs_dat, rhs_cnt, too_few_ndrdata);

		/*CLRPC_CL_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "floor(%u), lhs_cnt(%u), rhs_cnt(%u)", floor_idx, lhs_cnt, rhs_cnt);*/

		switch( floor_idx ) {
		case 0: /* interface-id */
			if( lhs_cnt != 19 || lhs_dat[0] != 0x0d || rhs_cnt != 2 ) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled encoding"
					, floor_idx
					);
				goto skip_entry;
			}

			CLRPC_GET_UUID_LITTLE(&qry->epm_entry.if_uuid, lhs_dat+1);

			qry->epm_entry.if_vers_major = CLRPC_GET16_LITTLE(lhs_dat+17);

			qry->epm_entry.if_vers_minor = CLRPC_GET16_LITTLE(rhs_dat+0);

			break;

		case 1: /* data-rep */
			if( lhs_cnt != 19 || lhs_dat[0] != 0x0d || rhs_cnt != 2 ) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled encoding"
					, floor_idx
					);
				goto skip_entry;
			}

			/* expected syntax-id 8a885d04-1ceb-11c9-9fe8-08002b104860, version 2*/
			if( ! clrpc_uuid_equal((CLRPC_UUID_PTR_TYPE)(lhs_dat+1), (CLRPC_UUID_PTR_TYPE)"\x04\x5d\x88\x8a\xeb\x1c\xc9\x11\x9f\xe8\x08\x00\x2b\x10\x48\x60")
				|| CLRPC_GET16_LITTLE(lhs_dat+17) != 2
			) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled syntax"
					, floor_idx
					);
				goto skip_entry;
			}
			break;

		case 2: /* protocol-id */
			if( lhs_cnt != 1 || lhs_dat[0] != 0x0a /* connectionless */ ) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled encoding"
					, floor_idx
					);
				goto skip_entry;
			}
			break;

		case 3: /* port */
			if( lhs_cnt != 1 || lhs_dat[0] != 8 /* UDP */ || rhs_cnt != 2 ) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled encoding"
					, floor_idx
					);
				goto skip_entry;
			}
			CLRPC_MEMCPY(&qry->epm_entry.udp_port, rhs_dat, sizeof(qry->epm_entry.udp_port)); /* copy! big endian! */
			break;

		case 4: /* host */
			if( lhs_cnt != 1 || lhs_dat[0] != 9 /* IP */ || rhs_cnt != 4 ) {
				CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "floor(%u), unhandled encoding"
					, floor_idx
					);
				goto skip_entry;
			}
			CLRPC_MEMCPY(&qry->epm_entry.host_ip, rhs_dat, sizeof(qry->epm_entry.host_ip)); /* copy! big endian! */
			break;
		}
	}
	}

	CLRPC_ALIGN(&bytes_left, &ptr, LSA_UINT32);

	if( bytes_left != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "decode-error"
			);
		return CLRPC_ERR_DECODE;
	}

	/***/

	qry->epm_entry.valid = 1; /* the entry is valid */

	return CLRPC_OK;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_break(
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_break")
	CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE qry = upper->args.cl.query_epm; /* shortcut */
	LSA_UINT8 * ptr;

	CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "state(%u)", qry->state
		);

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BREAK);

	/***/

	CLRPC_ASSERT(qry);

	CLRPC_ASSERT(qry->u.call.client_id);

	/***/

	qry->epm_entry.valid = 0;
	qry->epm_entry.more = 0;

	/***/

	if( qry->state == CLRPC_QUERY_EPM_STOP ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "already in stop-state (is ok!)"
			);
		/* this is not an error */
		return CLRPC_OK; /* complete */
	}

	if( qry->state != CLRPC_QUERY_EPM_LOOKUP_NEXT ) {
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "not in next-state, state(%u)"
			, qry->state
			);
		return CLRPC_ERR_SEQUENCE;
	}

	qry->state = CLRPC_QUERY_EPM_LOOKUP_FREE;

	/***/

	qry->u.call.opnum = 4; /* ept_lookup_handle_free() */

	qry->u.call.semantics = CLRPC_SEMANTICS_AT_MOST_ONCE;

	qry->u.call.pnio_signature = 0; /* not "PN IO signature call" */

	/***/

#if CLRPC_DEBUG
	CLRPC_MEMSET(qry->ndr_data, 0xCC, qry->alloc_len); /* to find uninitialized things */
#endif

	qry->u.call.alloc_len = qry->alloc_len;

	ptr = qry->u.call.ndr_data = qry->ndr_data;

	CLRPC_ASSERT(((LSA_UINT32)ptr & 3) == 0); /* buffer must be long-aligned (at least) */

	/***/

	/* marshal the in-params */

	CLRPC_MARSHAL32(&ptr, 0); /* context zero */

	CLRPC_ASSERT( ! clrpc_uuid_is_nil(&qry->ctx_uuid));

	CLRPC_MARSHAL_UUID(&ptr, &qry->ctx_uuid); /* context uuid */

	/***/

	qry->u.call.ndr_len = ptr - qry->u.call.ndr_data;

	CLRPC_ASSERT(qry->u.call.ndr_len <= qry->u.call.alloc_len);

	/* continues like a "normal" call() */
	return CLRPC_OK_CANCELLED; /* tricky return-code, see caller */
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_cl_query_epm_break_done(
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE upper
) {
	CLRPC_LOG_FCT("clrpc_cl_query_epm_break_done")
	CLRPC_UPPER_CL_QUERY_EPM_PTR_TYPE qry = upper->args.cl.query_epm; /* shortcut */
	LSA_UINT32 bytes_left;
	LSA_UINT8 * ptr;

	CLRPC_ASSERT(qry);

	CLRPC_CL_TRACE_04(0, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u), nca_status(%#x), ndr_len(%u), state(%u)"
		, response
		, qry->u.call.nca_status
		, qry->u.call.ndr_len
		, qry->state
		);

	CLRPC_ASSERT(CLRPC_RQB_GET_OPCODE(upper) == CLRPC_OPC_CL_QUERY_EPM_BREAK);
	CLRPC_ASSERT(upper->args.cl.call->pnio_signature == 0);

	CLRPC_ASSERT(qry->u.call.client_id);

	CLRPC_ASSERT(qry->state == CLRPC_QUERY_EPM_LOOKUP_FREE);

	/***/

	if( response != CLRPC_OK ) {
		CLRPC_CL_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "call failed, rsp(%u), nca_status(%#x)"
			, response
			, qry->u.call.nca_status
			);
		return response;
	}

	/***/

	ptr = qry->u.call.ndr_data;

	CLRPC_ASSERT(((LSA_UINT32)ptr & 3) == 0); /* buffer must be long-aligned (at least) */

	bytes_left = qry->u.call.ndr_len;

	if( bytes_left != (20 + 4) || (bytes_left & 3) != 0 ) {
too_few_ndrdata:
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "too few ndrdata (%u) or not 0 mod 4"
			, qry->u.call.ndr_len
			);
		return CLRPC_ERR_DECODE;
	}

	/* unmarshal the out-params */

	{
	LSA_UINT32 ctx_zero;
	CLRPC_UUID_TYPE ctx_uuid;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &ctx_zero, qry, too_few_ndrdata);

	CLRPC_UNMARSHAL_UUID(&ptr, &bytes_left, &ctx_uuid, qry, too_few_ndrdata);

	if( ctx_zero != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong ctx-handle-zero"
			);
		return CLRPC_ERR_DECODE;
	}

#if 0 /*noisy*/
	CLRPC_CL_TRACE_11(0, LSA_TRACE_LEVEL_CHAT
		, "ctx-handle(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ctx_uuid.time_low, ctx_uuid.time_mid, ctx_uuid.time_hi_and_version
		, ctx_uuid.clock_seq_hi_and_reserved, ctx_uuid.clock_seq_low
		, ctx_uuid.node[0], ctx_uuid.node[1], ctx_uuid.node[2]
		, ctx_uuid.node[3], ctx_uuid.node[4], ctx_uuid.node[5]
		);
#endif

#if CLRPC_DEBUG
	if( ! clrpc_uuid_is_nil(&ctx_uuid) ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong ctx-handle-uuid (not null)"
			);
		/* ignore! */
	}
#endif
	}

	{
	LSA_UINT32 status;

	CLRPC_UNMARSHAL32(&ptr, &bytes_left, &status, qry, too_few_ndrdata);

	if( status != 0 ) {
		CLRPC_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong status (%#x)"
			, status
			);
		/* ignore! */
	}
	}

	if( bytes_left != 0 ) {
		CLRPC_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "decode-error"
			);
		/* ignore! */
	}

	qry->state = CLRPC_QUERY_EPM_STOP;

	return CLRPC_OK; /* complete */
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#else

#ifdef CLRPC_MESSAGE
#pragma CLRPC_MESSAGE("!!! not compiling client")
#endif /* CLRPC_MESSAGE */

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
