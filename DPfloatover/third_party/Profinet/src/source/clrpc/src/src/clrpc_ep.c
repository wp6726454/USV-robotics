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
/*  F i l e               &F: clrpc_ep.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the End Point Mapper                                          */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    12
#define LTRC_ACT_MODUL_ID  12

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


	/* compile only, when server-part is needed */
#if CLRPC_CFG_MAX_SERVERS


/*
use this IDL

typedef struct
{
                uuid_t      object;
                twr_p_t     tower;
    [string]    char        annotation[ept_max_annotation_size];
} ept_entry_t, *ept_entry_p_t;

[idempotent] void ept_lookup(
    [in]            handle_t            h,
    [in]            unsigned32          inquiry_type,
    [in]            uuid_p_t            object,
    [in]            rpc_if_id_p_t       interface_id,
    [in]            unsigned32          vers_option,
    [in, out]       ept_lookup_handle_t *entry_handle,
    [in]            unsigned32          max_ents,
    [out]           unsigned32          *num_ents,
    [out, length_is(*num_ents), size_is(max_ents)]
                    ept_entry_t         entries[],
    [out]           error_status_t      *status
);

[at-most-once] void ept_lookup_handle_free(
    [in]            handle_t            h,
    [in, out]       ept_lookup_handle_t *entry_handle,
    [out]           error_status_t      *status
);

*/

/*------------------------------------------------------------------------------
// the context-handle of the endpoint-mapper
//----------------------------------------------------------------------------*/

	/* ::server_reported */
#define CLRPC_CTX_NOT_REPORTED		0
/*#define CLRPC_CTX_SEARCH_OBJ_ID	1*/
#define CLRPC_CTX_REPORTED			2

typedef struct {
	CLRPC_HASH_ENTRY_TYPE		hashed; /* .uuid of this context, need for hash add/rm */
	CLRPC_ONESHOT_TYPE			timer; /* the needed oneshot timer for self-destruction */
	LSA_UINT8					server_reported[CLRPC_CFG_MAX_SERVERS];
} CLRPC_EPM_CTX_TYPE, CLRPC_LOCAL_MEM_ATTR  *CLRPC_EPM_CTX_PTR_TYPE;


/*------------------------------------------------------------------------------
// global data
//----------------------------------------------------------------------------*/

	/* EPM-Context */
CLRPC_HASH_TYPE		clrpc_ctx_hash;


/*------------------------------------------------------------------------------
//	clrpc_epm_ctx_free
//	work: free's a context-handle
//----------------------------------------------------------------------------*/
static  LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_ctx_free (
	CLRPC_EPM_CTX_PTR_TYPE  CLRPC_LOCAL_MEM_ATTR  *ctx
	)
{
	CLRPC_LOG_FCT("clrpc_epm_ctx_free")
	LSA_UINT16  rc;

	CLRPC_ASSERT (ctx);
	CLRPC_ASSERT (*ctx);

	CLRPC_EPM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
		, "ctx(%#x)", *ctx
		);

	clrpc_oneshot_stop (& (*ctx)->timer);

	clrpc_hash_remove (& clrpc_ctx_hash, & (*ctx)->hashed.uuid);

	CLRPC_FREE_LOCAL_MEM (& rc, *ctx);
	CLRPC_ASSERT(rc == LSA_RET_OK);
	*ctx = LSA_NULL;
}


/*------------------------------------------------------------------------------
//	clrpc_epm_ctx_oneshot_callback
//	work: timeout of ctx
//----------------------------------------------------------------------------*/
static  LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_ctx_oneshot_callback (
	CLRPC_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE user_ptr
	)
{
	CLRPC_LOG_FCT("clrpc_ctx_oneshot_callback")
	CLRPC_EPM_CTX_PTR_TYPE  ctx = (CLRPC_EPM_CTX_PTR_TYPE)user_ptr; /* typecast */

	CLRPC_EPM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
		, "ctx(%#x)", ctx
		);

	LSA_UNUSED_ARG(oneshot);

	clrpc_epm_ctx_free (& ctx);
	return;
}


/*------------------------------------------------------------------------------
//	clrpc_epm_ctx_new
//	work: creates a new context-handle
//----------------------------------------------------------------------------*/
static  CLRPC_EPM_CTX_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_ctx_new (
	LSA_VOID
	)
{
	CLRPC_LOG_FCT("clrpc_epm_ctx_new")
	CLRPC_EPM_CTX_PTR_TYPE  ctx;
	CLRPC_UUID_TYPE  ctxuid;
	LSA_UINT16  i;
	CLRPC_CHANNEL_PTR_TYPE  sys_channel = clrpc_system_channel();

	CLRPC_ALLOC_LOCAL_MEM (CLRPC_CAST_LOCAL_MEM_PTR_PTR(& ctx), sizeof(CLRPC_EPM_CTX_TYPE));
	if (CLRPC_PTR_IS_NULL (ctx)) {
		CLRPC_EPM_TRACE_00 (0, LSA_TRACE_LEVEL_UNEXP
			, "no local mem"
			);
		return LSA_NULL;
	}

	clrpc_uuid_create (sys_channel, & ctxuid);
	clrpc_hash_entry_setup (& ctx->hashed, & ctxuid, ctx);

	clrpc_oneshot_init (& ctx->timer, clrpc_epm_ctx_oneshot_callback, ctx);

	for (i = 0; i < CLRPC_CFG_MAX_SERVERS; ++i)
	{
		ctx->server_reported[i] = CLRPC_CTX_NOT_REPORTED;
	}

	clrpc_hash_add (& clrpc_ctx_hash, & ctx->hashed);

	clrpc_oneshot_trigger (& ctx->timer, CLRPC_CFG_CTX_TIMEOUT_MSEC);

	CLRPC_EPM_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
		, "new ctx(%#x)"
		, ctx
		);
	return ctx;
}


/*------------------------------------------------------------------------------
//	clrpc_epm_ctx_fetch
//	work: search a context-handle
//----------------------------------------------------------------------------*/
static  CLRPC_EPM_CTX_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_ctx_fetch (
	CLRPC_UUID_PTR_TYPE  uuid
	)
{
	CLRPC_LOG_FCT("clrpc_epm_ctx_fetch")
	CLRPC_EPM_CTX_PTR_TYPE  ctx = LSA_NULL;

	ctx = (CLRPC_EPM_CTX_PTR_TYPE)clrpc_hash_lookup (& clrpc_ctx_hash, uuid, CLRPC_NO_HINT);

		/* retrigger timout */
	if (CLRPC_PTR_IS_NOT_NULL (ctx))
	{
		clrpc_oneshot_trigger (& ctx->timer, CLRPC_CFG_CTX_TIMEOUT_MSEC);
	}

	CLRPC_EPM_TRACE_12 (0, LSA_TRACE_LEVEL_CHAT
		, "ctx(%08x) uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ctx
		, uuid->time_low, uuid->time_mid, uuid->time_hi_and_version
		, uuid->clock_seq_hi_and_reserved, uuid->clock_seq_low
		, uuid->node[0], uuid->node[1], uuid->node[2]
		, uuid->node[3], uuid->node[4], uuid->node[5]
		);

	return ctx;
}


/*------------------------------------------------------------------------------
//	clrpc_epm_server_unregister
//	work: detach the server-object_hash from ctx-iterators
//	see : AP00820599
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_unregister_server (
	CLRPC_SERVER_PTR_TYPE  server
	)
{
	CLRPC_LOG_FCT("clrpc_epm_unregister_server")
	CLRPC_EPM_CTX_PTR_TYPE  ctx;
	CLRPC_HASH_ITERATOR_TYPE  iter;

	clrpc_hash_iter_setup (& iter);
	clrpc_hash_iter_attach (& iter, & clrpc_ctx_hash);
	clrpc_hash_iter_reset (& iter);

	for (ctx = (CLRPC_EPM_CTX_PTR_TYPE)clrpc_hash_iter_next (& iter);
		CLRPC_PTR_IS_NOT_NULL (ctx);
		ctx = (CLRPC_EPM_CTX_PTR_TYPE)clrpc_hash_iter_next (& iter))
	{
		if (server->is_endpoint_mapper)
		{
			clrpc_epm_ctx_free (& ctx);
		}
		else
		{
			ctx->server_reported[server->id] = CLRPC_CTX_REPORTED;
		}
	}
	clrpc_hash_iter_detach (& iter);
}


/*------------------------------------------------------------------------------
//	clrpc_epm_set_empty_data
//	work: write nil-response-data for requests lookup/lookup_handle_free
//		to call->ndrdata, adjust call->ndr_len to correct size
//----------------------------------------------------------------------------*/
static  LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_set_empty_data (
	CLRPC_UPPER_SV_CALL_PTR_TYPE  call,
	LSA_UINT32  save_max_ents,
	LSA_UINT32  status
	)
{
	CLRPC_LOG_FCT("clrpc_epm_set_empty_data")
	CLRPC_UPPER_MEM_U8_PTR_TYPE  dp;		/* data_ptr */

	dp = call->ndr_data;

	CLRPC_EPM_TRACE_02 (0, LSA_TRACE_LEVEL_CHAT
		, "max_ents(%u) status(%u)"
		, save_max_ents, status
		);

		/* nil-lookup response */
	if (save_max_ents > 0)
	{
		CLRPC_EPM_DP_U32 (dp, 0);				/* entry_handle */
		CLRPC_EPM_DP_UUID (dp, & clrpc_uuid_nil_uuid);
		CLRPC_EPM_DP_U32 (dp, 0);				/* num_ents */

		CLRPC_EPM_DP_U32 (dp, save_max_ents);	/* entries max */
		CLRPC_EPM_DP_U32 (dp, 0);				/* entries offset */
		CLRPC_EPM_DP_U32 (dp, 0);				/* entries cnt */

			/* align status */
		dp = (CLRPC_UPPER_MEM_U8_PTR_TYPE)(((LSA_UINT32)dp + 3) & (~ 0x3));
		CLRPC_EPM_DP_U32 (dp, status);			/* status */
	}

		/* nil-lookup_handle_free response */
	else
	{
		CLRPC_EPM_DP_U32 (dp, 0);				/* entry_handle */
		CLRPC_EPM_DP_UUID (dp, & clrpc_uuid_nil_uuid);

			/* align status */
		dp = (CLRPC_UPPER_MEM_U8_PTR_TYPE)(((LSA_UINT32)dp + 3) & (~ 0x3));
		CLRPC_EPM_DP_U32 (dp, status);			/* status */
	}

		/* adjust body len */
	call->ndr_len = dp - call->ndr_data;
}


/*------------------------------------------------------------------------------
//	find the endpoint-mapper server with a state
//----------------------------------------------------------------------------*/
static  CLRPC_SERVER_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_server_lookup (
	LSA_UINT16  state
	)
{
	CLRPC_LOG_FCT("clrpc_epm_server_lookup")
	LSA_UINT16  id;
	CLRPC_SERVER_PTR_TYPE  server;
	
	CLRPC_ASSERT (state == CLRPC_SST_IDLE || state == CLRPC_SST_READY);

	for (id = 0; id < CLRPC_CFG_MAX_SERVERS; ++id)
	{
		server = & clrpc_servers[id];

		if (server->state != state) {
			continue;
		}
		if (server->is_endpoint_mapper == 0) {
			continue;
		}

		return server; /* epm with state */
	}
	CLRPC_EPM_TRACE_00 (0, LSA_TRACE_LEVEL_CHAT, "epm-server not found");
	return LSA_NULL;
}

/*------------------------------------------------------------------------------
//	clrpcget_str_len_
//	work: length of string, included the \0 
//----------------------------------------------------------------------------*/
LSA_UINT32 CLRPC_LOCAL_FCT_ATTR clrpc_get_strlen(
    CLRPC_COMMON_MEM_U8_PTR_TYPE    str, 
    LSA_UINT32                      max_count)
{
    LSA_UINT32 i;
    
    for (i=0; i<max_count; i++)
    {
        if (*str =='\0')
        {
            break;
        }
        else
        {
            str++;
        }
    }
    return(i);
}


/*------------------------------------------------------------------------------
//	clrpc_epm_callback
//	work: implementes 'ep.idl'
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_callback (
	CLRPC_SCALL_PTR_TYPE  scall,
	LSA_UINT16  response,
	CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR  *rb_ptr)
{
	CLRPC_LOG_FCT("clrpc_epm_callback")
	CLRPC_UPPER_RQB_PTR_TYPE  rb;
	CLRPC_UPPER_SV_CALL_PTR_TYPE  call;
	CLRPC_EPM_LOOKUP_PTR_TYPE  look;
	CLRPC_EPM_LOOKUP_FREE_PTR_TYPE  look_free;
	CLRPC_SERVER_PTR_TYPE  server;
	LSA_UINT32  save_max_ents, ann_count;
	LSA_UINT32  local_ip_addr;
	CLRPC_EPM_CTX_PTR_TYPE  ctx;
	LSA_UINT16  i;
	CLRPC_UPPER_MEM_U8_PTR_TYPE  dp;
	CLRPC_UUID_TYPE obj_uuid;
	LSA_UINT8  annotation[CLRPC_MAX_ANNOTATION_SIZE];

	CLRPC_ASSERT (rb_ptr);
	CLRPC_ASSERT (*rb_ptr);

	rb = *rb_ptr;
	*rb_ptr = LSA_NULL;

	CLRPC_EPM_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "response(%u) opc(%u)"
		, response, CLRPC_RQB_GET_OPCODE (rb)
		);

	CLRPC_ASSERT (response == CLRPC_OK);
	LSA_UNUSED_ARG (response);

	CLRPC_ASSERT (scall->server->is_endpoint_mapper > 0);

	local_ip_addr = 0; /* AP01017769: must return 0.0.0.0 for every epm-lookup */

	switch (CLRPC_RQB_GET_OPCODE (rb)) {
	case CLRPC_OPC_SV_CALL_INDICATION:
		CLRPC_ASSERT (CLRPC_PTR_IS_NOT_NULL (rb->args.sv.call));
		call = rb->args.sv.call;

		CLRPC_EPM_TRACE_07 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "epm-sid(%u) from.ip(%08x) from.port(%u) opnum(%u) sem(%u) alloc_len(%u) ndr_len(%u)"
			, call->server_id
			, CLRPC_NTOHL(call->host_ip), CLRPC_NTOHS(call->udp_port)
			, call->opnum, call->semantics, call->alloc_len, call->ndr_len
			);

			/* here must work the ep.idl */
		CLRPC_RQB_SET_RESPONSE (rb, CLRPC_OK);
        switch (call->opnum) {
/*----------------------------------------------------------------------------*/
		case 2: /* ept_lookup */
			if (sizeof(*look) != call->ndr_len)
			{
				CLRPC_EPM_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ndr_len(%u) != sizeof(args(%u))", call->ndr_len, sizeof (*look)
					);
				CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED);
				break; /*switch */
			}
			CLRPC_RQB_SET_RESPONSE (rb, CLRPC_OK);
			look = (CLRPC_EPM_LOOKUP_PTR_TYPE)call->ndr_data;

			if (CLRPC_DREP_INT_DIFFERS(call->drep.drep_int))
			{
				CLRPC_SWAP_U32(&look->inquiry_type);
				CLRPC_SWAP_U32(&look->object_ref);
				CLRPC_SWAP_UUID(&look->object);
				CLRPC_SWAP_U32(&look->if_id_ref);
				CLRPC_SWAP_UUID(&look->if_id_uuid);
				CLRPC_SWAP_U16(&look->if_id_vers_major);
				CLRPC_SWAP_U16(&look->if_id_vers_minor);
				CLRPC_SWAP_U32(&look->vers_option);
				CLRPC_SWAP_U32(&look->entry_handle_ref);
				CLRPC_SWAP_UUID(&look->entry_handle);
				CLRPC_SWAP_U32(&look->max_ents);
			}

			save_max_ents = look->max_ents;

			if (look->inquiry_type != 0)
			{
				CLRPC_EPM_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "inquiry_type(%u) != 0, only searching all elements supported"
					, look->inquiry_type
					);
				CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED);
				break; /*switch */
			}
			if (look->max_ents < 1)
			{
				CLRPC_EPM_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "max_ents(%u) < 1", look->max_ents
					);
				CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED);
				break; /*switch */
			}

			CLRPC_EPM_TRACE_07 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "inq_type(%u) obj(%u:%u..) interface(%u:%u..:%u:%u)"
				, look->inquiry_type, look->object_ref, look->object.time_low
				, look->if_id_ref
				, look->if_id_uuid.time_low, look->if_id_vers_major, look->if_id_vers_minor
				);
			CLRPC_EPM_TRACE_06 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "  option(%u) entry_handle(%u:%u.%u..%u) max_ents(%u)"
				, look->vers_option
				, look->entry_handle_ref
				, look->entry_handle.time_low, look->entry_handle.time_mid, look->entry_handle.node[5]
				, look->max_ents
				);

				/* create a new lookup_context of entry handle */
			if (clrpc_uuid_is_nil (& look->entry_handle))
			{
				ctx = clrpc_epm_ctx_new ();
			}
				/* or fetch an existing lookup_context */
			else
			{
				ctx = clrpc_epm_ctx_fetch (& look->entry_handle);
			}
			if (CLRPC_PTR_IS_NULL (ctx))
			{
				clrpc_epm_set_empty_data (call, save_max_ents, CLRPC_EPT_NOT_REGISTERED);
				break; /* case */
			}

				/* searching next server-entry for writing back */
			server = LSA_NULL;
			CLRPC_MEMCPY (& obj_uuid, & clrpc_uuid_nil_uuid, sizeof (obj_uuid));
			for (i = 0; i < CLRPC_CFG_MAX_SERVERS; ++i)
			{
/*				CLRPC_IF_OBJECT_PTR_TYPE  if_obj;*/

				if (ctx->server_reported[i] == CLRPC_CTX_REPORTED)
				{
					continue; /* for */
				}

				server = & clrpc_servers[i];

				if (! (server->state == CLRPC_SST_READY))
				{
					ctx->server_reported[i] = CLRPC_CTX_REPORTED;
					server = LSA_NULL;
					continue; /* for */
				}

#if 0 /* 2006-11-22: also deliver the annotation of the epm */
				if (server->is_endpoint_mapper > 0)
				{
					ctx->server_reported[i] = CLRPC_CTX_REPORTED;
					server = LSA_NULL;
					continue; /* for */
				}
#endif

				if (ctx->server_reported[i] == CLRPC_CTX_NOT_REPORTED)
				{
					ctx->server_reported[i] = CLRPC_CTX_REPORTED;

					obj_uuid = server->obj.object_uuid;
					CLRPC_MEMCPY (annotation, server->obj.annotation, sizeof (annotation));
					break; /* for */
				}
			}

				/* no (more) server for response, clear ctx and response */
			if (CLRPC_PTR_IS_NULL (server))
			{
				clrpc_epm_ctx_free (& ctx);
				clrpc_epm_set_empty_data (call, save_max_ents, CLRPC_EPT_NOT_REGISTERED);
				CLRPC_RQB_SET_RESPONSE (rb, CLRPC_OK);
				break; /* case */
			}

				/* write data to rqb */
			dp = call->ndr_data;
			CLRPC_EPM_DP_U32 (dp, 0);				/* entry_handle fix 0 */
			CLRPC_EPM_DP_UUID (dp, & ctx->hashed.uuid); /* entry_handle uuid */

			CLRPC_EPM_DP_U32 (dp, 1);				/* num_ents */

			CLRPC_EPM_DP_U32 (dp, save_max_ents);	/* entries max */
			CLRPC_EPM_DP_U32 (dp, 0);				/* entries offset */
			CLRPC_EPM_DP_U32 (dp, 1);				/* entries cnt */

			CLRPC_EPM_DP_UUID (dp, & obj_uuid);		/* entry.object */
			CLRPC_EPM_DP_U32 (dp, 3);				/* tower pointer ref, 3 see in-para */

			CLRPC_EPM_DP_U32 (dp, 0);				/* annotation offset */
			ann_count = clrpc_get_strlen(annotation, CLRPC_MAX_ANNOTATION_SIZE);  // len of string, 
            ann_count++;                            // \0 included
            if (ann_count > CLRPC_MAX_ANNOTATION_SIZE)
            {
                CLRPC_FATAL1(ann_count);
            }
            CLRPC_EPM_DP_U32 (dp, ann_count);		/* annotation cnt, incl. zero */
			CLRPC_MEMCPY(dp, annotation, ann_count);  /* annotation, incl. zero */
			dp += ann_count;

				/* tower data */
			dp = (CLRPC_UPPER_MEM_U8_PTR_TYPE)(((LSA_UINT32)dp + 3) & (~ 0x3));				/* align status */
			CLRPC_EPM_DP_U32 (dp, 75)				/* tower len */
			CLRPC_EPM_DP_U32 (dp, 75)				/* tower len */

				/* tower_octet_string[] in little endian! */
			CLRPC_EPM_DP_U16_LITTLE (dp, 5);		/* floor count */

				/* floor 1   rpc-interface */
			CLRPC_EPM_DP_U16_LITTLE (dp, 19);			/* lhs len */
			CLRPC_EPM_DP_U8 (dp, 0x0D);					/* const for uuid */
			CLRPC_EPM_DP_UUID_LITTLE (dp, & server->if_uuid);
			CLRPC_EPM_DP_U16_LITTLE (dp, server->if_major);

			CLRPC_EPM_DP_U16_LITTLE (dp, 2);				/* rhs len */
			CLRPC_EPM_DP_U16_LITTLE (dp, server->if_minor);	/* if_minor */

				/* floor 2 data representation id */
			CLRPC_EPM_DP_U16_LITTLE (dp, 19);			/* lhs len */
			CLRPC_EPM_DP_U8 (dp, 0x0D);
			CLRPC_EPM_DP_UUID_LITTLE (dp, & clrpc_endpoint_type_id);
			CLRPC_EPM_DP_U16_LITTLE (dp, 2);

			CLRPC_EPM_DP_U16_LITTLE (dp, 2);			/* rhs len */
			CLRPC_EPM_DP_U16_LITTLE (dp, 0);

				/* floor 3 protocol identifier */
			CLRPC_EPM_DP_U16_LITTLE (dp, 1);			/* lhs len */
			CLRPC_EPM_DP_U8 (dp, 0x0A);					/* udp-version major 4 */

			CLRPC_EPM_DP_U16_LITTLE (dp, 2);			/* rhs len */
			CLRPC_EPM_DP_U16_LITTLE (dp, 0);			/* udp-version minor 0 */

				/* floor 4 port address */
			CLRPC_EPM_DP_U16_LITTLE (dp, 1);			/* lhs len */
			CLRPC_EPM_DP_U8 (dp, 0x08);					/* udp port */

			CLRPC_EPM_DP_U16_LITTLE (dp, 2);			/* rhs len */
			CLRPC_MEMCPY(dp, &server->port, 2);			/* port number in big endian, copy! */
			dp += 2;

				/* floor 5 host address */
			CLRPC_EPM_DP_U16_LITTLE (dp, 1);			/* lhs len */
			CLRPC_EPM_DP_U8 (dp, 0x09);					/* udp port */

			CLRPC_EPM_DP_U16_LITTLE (dp, 4);			/* rhs len */

			CLRPC_MEMCPY(dp, &local_ip_addr, 4); /* ip address in big endian, copy! */
			dp += 4;

				/* adjust to natural alignment */
			dp = (CLRPC_UPPER_MEM_U8_PTR_TYPE)(((LSA_UINT32)dp + 3) & (~ 0x3));
			CLRPC_EPM_DP_U32 (dp, 0);				/* status */

				/* adjust body len */
			call->ndr_len = dp - call->ndr_data;
			CLRPC_ASSERT (call->alloc_len >= call->ndr_len);
			break;

/*----------------------------------------------------------------------------*/
		case 4: /* ept_lookup_handle_free */
			if (sizeof(*look_free) != call->ndr_len) {
				CLRPC_EPM_TRACE_02 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ndr_len(%u) != sizeof(look_free(%u))"
					, call->ndr_len, sizeof (*look_free)
					);
				CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED); /* not err_rejected, ms will repeat forever */
				break;
			}
			CLRPC_RQB_SET_RESPONSE (rb, CLRPC_OK);
			look_free = (CLRPC_EPM_LOOKUP_FREE_PTR_TYPE)call->ndr_data;

			if( CLRPC_DREP_INT_DIFFERS(call->drep.drep_int) ) {

				CLRPC_SWAP_U32(&look_free->entry_handle_ref);
				CLRPC_SWAP_UUID(&look_free->entry_handle);
			}

			CLRPC_EPM_TRACE_12 (scall->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "entry_handle.uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) .ref(%u)"
				, look_free->entry_handle.time_low, look_free->entry_handle.time_mid, look_free->entry_handle.time_hi_and_version
				, look_free->entry_handle.clock_seq_hi_and_reserved, look_free->entry_handle.clock_seq_low
				, look_free->entry_handle.node[0], look_free->entry_handle.node[1], look_free->entry_handle.node[2]
				, look_free->entry_handle.node[3], look_free->entry_handle.node[4], look_free->entry_handle.node[5]
				, look_free->entry_handle_ref
				);

				/* lookup and free context-handle */
			ctx = clrpc_epm_ctx_fetch (& look_free->entry_handle);
			if (CLRPC_PTR_IS_NOT_NULL (ctx)) {
				clrpc_epm_ctx_free (& ctx);
			}
			CLRPC_ASSERT (CLRPC_PTR_IS_NULL (ctx));
				/* set result data, status == OK == 0 */
			clrpc_epm_set_empty_data (call, 0, 0);
			break;

/*----------------------------------------------------------------------------*/
		default:
			CLRPC_EPM_TRACE_01 (scall->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "unsupported opnum(%u)", call->opnum
				);
			CLRPC_RQB_SET_RESPONSE (rb, CLRPC_ERR_FAULTED); /* not err_rejected, ms will repeat forever */
			break;
		}

			/* respond (answer) on every case, for rqb provide balance */
		CLRPC_RQB_SET_OPCODE (rb, CLRPC_OPC_SV_CALL_RESPONSE);
		clrpc_sv_op_call_response (scall->server->channel, rb);
		break;

/*----------------------------------------------------------------------------*/
	default:
		CLRPC_FATAL1 (CLRPC_RQB_GET_OPCODE (rb)); /* "unsupported opcode" */
		break;
	}
}


/*------------------------------------------------------------------------------
//	clrpc_epm_provide
//	work: provide call.ind-resources for the endpoint-mapper
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_epm_provide (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_SERVER_PTR_TYPE  server,
	LSA_UINT16  ind_provide_cnt
	)
{
	CLRPC_LOG_FCT("clrpc_epm_provide")
	CLRPC_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  i;

	CLRPC_ASSERT (server->is_endpoint_mapper > 0);

	for (i = 0; i < ind_provide_cnt; ++i)
	{
		CLRPC_ALLOC_UPPER_RQB (CLRPC_CAST_UPPER_RQB_PTR_PTR (& upper)
			, clrpc_null_user_id, sizeof(CLRPC_RQB_TYPE), channel->sysptr
			);
		if (CLRPC_PTR_IS_NULL (upper)) {
			CLRPC_FATAL1(0); /* alloc-error not handled */
		}

		CLRPC_ALLOC_UPPER_MEM (CLRPC_CAST_UPPER_MEM_PTR_PTR (& (upper)->args.sv.call)
			, clrpc_null_user_id, sizeof(CLRPC_SV_CALL_TYPE), channel->sysptr
			);
		if (CLRPC_PTR_IS_NULL (upper->args.sv.call)) {
			CLRPC_FATAL1(0); /* alloc-error not handled */
		}

		CLRPC_RQB_SET_HANDLE (upper, channel->handle_upper);
		CLRPC_RQB_SET_OPCODE (upper, CLRPC_OPC_SV_CALL_IND_PROVIDE);
#if CLRPC_DEBUG
		CLRPC_RQB_SET_NEXT_RQB_PTR (upper, LSA_NULL); /* sanity */
		CLRPC_RQB_SET_PREV_RQB_PTR (upper, LSA_NULL); /* sanity */
#endif
		upper->args.sv.call->server_id = server->id;
		upper->args.sv.call->ndr_data = LSA_NULL;
		upper->args.sv.call->alloc_len = 0;

		clrpc_sv_provide_enqueue (server, & upper);
	}
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_EPM_START
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_op_epm_start (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	)
{
	CLRPC_LOG_FCT("clrpc_op_epm_start")
	CLRPC_SERVER_PTR_TYPE  epm_server;
	LSA_UINT16  rc;

		/* more param-check is done in clrpc_sv_op_register_internal() */

	if (! clrpc_system_channel_is_ready ())
	{
		CLRPC_EPM_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "system-channel not ready yet"
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}
    
    if (!clrpc_mac_addr_is_valid(channel->mac_addr))
    {
		CLRPC_SV_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "only allowed after CLRPC_OPC_SET_INTERFACE_PARAM with mac_addr"
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
    }


		/* is there already an endpoint-mapper ? */
    epm_server = clrpc_epm_server_lookup (CLRPC_SST_READY);

		/* register the n-th endpoint-mapper */
	if (CLRPC_PTR_IS_NOT_NULL (epm_server))
	{
		CLRPC_ASSERT (epm_server->is_endpoint_mapper >= 1);

			/* increment ref-counter */
		epm_server->is_endpoint_mapper += 1;

		CLRPC_EPM_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "register to epm_server(%u): new reference counter: is_endpoint_mapper(%u)"
			, epm_server->id, epm_server->is_endpoint_mapper
			);
		rb->args.epm.start->server_id = epm_server->id;
		rb->args.epm.start->udp_port  = epm_server->port;
		clrpc_callback (channel, CLRPC_OK, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

		/* !CAST! keep original upper channel */
	CLRPC_RQB_SET_PREV_RQB_PTR (rb, (CLRPC_UPPER_RQB_PTR_TYPE)((LSA_VOID *)channel));

		/* init next epm_start*/
	CLRPC_RQB_SET_NEXT_RQB_PTR (rb, LSA_NULL); /* will keep the next epm_start */
	
		/* two or more epm-start may given at the 'same' time, see AP01017769 */
	epm_server = clrpc_epm_server_lookup (CLRPC_SST_IDLE);
	if (CLRPC_PTR_IS_NOT_NULL (epm_server))
	{
		CLRPC_UPPER_RQB_PTR_TYPE  next = epm_server->upper;
        if (CLRPC_PTR_IS_NULL (next)) 
        {
			CLRPC_FATAL1(epm_server->state); /* a bug */
		}
        while (CLRPC_PTR_IS_NOT_NULL (CLRPC_RQB_GET_NEXT_RQB_PTR(next))) 
        {
			next = CLRPC_RQB_GET_NEXT_RQB_PTR(next);
		}
        CLRPC_RQB_SET_NEXT_RQB_PTR (next, rb); /* handle later */
		return;		
	}

		/* register the first endpoint-mapper */
	rc = clrpc_sv_op_register_internal (rb);
	if (rc != CLRPC_OK)
	{
		clrpc_callback (channel, rc, & rb, CLRPC_MODULE_ID, __LINE__);
	}
}


/*------------------------------------------------------------------------------
//	CLRPC_OPC_EPM_STOP
//----------------------------------------------------------------------------*/
LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_op_epm_stop (
	CLRPC_CHANNEL_PTR_TYPE		channel,
	CLRPC_UPPER_RQB_PTR_TYPE	rb
	)
{
	CLRPC_LOG_FCT("clrpc_op_epm_stop")
	LSA_UINT16  rc;
	CLRPC_SERVER_PTR_TYPE  epm_server;

	CLRPC_ASSERT (channel->detailptr->path_type == CLRPC_PATH_TYPE_SYSTEM
		|| channel->detailptr->path_type == CLRPC_PATH_TYPE_USER
		);

		/* param-check is done in clrpc_sv_op_unregister_internal() */

		/* is there already a endpoint-mapper ? */
	epm_server = clrpc_epm_server_lookup (CLRPC_SST_READY);
	if (CLRPC_PTR_IS_NULL (epm_server)) {
		CLRPC_EPM_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "epm-server doesn't exist"
			);
		clrpc_callback (channel, CLRPC_ERR_SEQUENCE, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

		/* if more than one epm_started: decrement ref-counter */
	if (epm_server->is_endpoint_mapper > 1) {

		epm_server->is_endpoint_mapper -= 1;
		CLRPC_EPM_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "unregister to epm_server(%u): new reference counter: is_endpoint_mapper(%u)"
			, epm_server->id, epm_server->is_endpoint_mapper
			);

		clrpc_callback (channel, CLRPC_OK, & rb, CLRPC_MODULE_ID, __LINE__);
		return;
	}

	rc = clrpc_sv_op_unregister_internal (channel, rb);
	if (rc != CLRPC_OK) {
		clrpc_callback (channel, rc, & rb, CLRPC_MODULE_ID, __LINE__);
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* CLRPC_CFG_MAX_SERVERS */
