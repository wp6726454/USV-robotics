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
/*  F i l e               &F: clrpc_sys.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the system LSA interface                                      */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    1
#define LTRC_ACT_MODUL_ID  1

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*----------------------------------------------------------------------------*/

LSA_USER_ID_TYPE	clrpc_null_user_id; /* see clrpc_init(), do not initialize here! */


/*-Local------------------------------------------------------------------------
//	func: clrpc_check_sizes
//	work: check if PDU types contain unexpected paddings
//----------------------------------------------------------------------------*/

static LSA_BOOL  CLRPC_LOCAL_FCT_ATTR
clrpc_check_sizes(
	LSA_VOID
) {

	if( sizeof(CLRPC_UUID_TYPE) != CLRPC_UUID_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_PNIO_IN_ARGS_TYPE) != CLRPC_PNIO_IN_ARGS_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_PNIO_OUT_ARGS_TYPE) != CLRPC_PNIO_OUT_ARGS_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_HEADER_TYPE) != CLRPC_HEADER_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_FACK_BODY_TYPE) != CLRPC_FACK_BODY_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_ERROR_BODY_TYPE) != CLRPC_ERROR_BODY_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_QUIT_BODY_TYPE) != CLRPC_QUIT_BODY_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_QUACK_BODY_TYPE) != CLRPC_QUACK_BODY_SIZE_PADDED ) { /* not: CLRPC_QUACK_BODY_SIZE_ONWIRE */
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_NDR_CONTEXT_HANDLE_TYPE) != CLRPC_NDR_CONTEXT_HANDLE_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_WAY_REQ_TYPE) != CLRPC_WAY_REQ_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_WAY_RSP_TYPE) != CLRPC_WAY_RSP_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_WAY2_RSP_TYPE) != CLRPC_WAY2_RSP_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_EPM_LOOKUP_TYPE) != CLRPC_EPM_LOOKUP_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	if( sizeof(CLRPC_EPM_LOOKUP_FREE_TYPE) != CLRPC_EPM_LOOKUP_FREE_SIZE ) {
		return LSA_FALSE; /* unexpected */
	}

	return LSA_TRUE; /* ok */
}


/*-Export-----------------------------------------------------------------------
//	func: clrpc_init
//	work: initialises the module CLRPC, must be called on startup
//----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_init (
	CLRPC_INIT_PTR_TYPE init
	)
{
	CLRPC_LOG_FCT("clrpc_init")

    LSA_UINT16 ret_val;
    LSA_UINT64 tmp_clkseq;
	LSA_UINT16 i;
#if CLRPC_CFG_DREP_INT == CLRPC_DREP_INT_BIG
	LSA_UINT8 b;
#elif CLRPC_CFG_DREP_INT == CLRPC_DREP_INT_LITTLE
	LSA_UINT8 b;
#else
	#error "check CLRPC_CFG_DREP_INT"
#endif

	/** RQB has to start with two pointers */
	if( ! (
		(LSA_UINT32)&CLRPC_RQB_GET_NEXT_RQB_PTR((CLRPC_UPPER_RQB_PTR_TYPE)0) == 0
		&& (LSA_UINT32)&CLRPC_RQB_GET_PREV_RQB_PTR((CLRPC_UPPER_RQB_PTR_TYPE)0) == (LSA_UINT32)sizeof(LSA_VOID_PTR_TYPE)
	   ) ) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "RQB does not start with two pointers"
			);
		CLRPC_FATAL1(0); /* RQB does not start with the two pointers */
	}

	/** check internal data representation */
    i = 0x1234;
#if CLRPC_CFG_DREP_INT == CLRPC_DREP_INT_BIG
    b = 0x12;
#elif CLRPC_CFG_DREP_INT == CLRPC_DREP_INT_LITTLE
    b = 0x34;
#else
#error "check CLRPC_CFG_DREP_INT"
#endif
    if( *(LSA_UINT8*)&i != b ) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "CLRPC_CFG_DREP_INT is wrong"
			);
		CLRPC_FATAL1(0); /* CLRPC_CFG_DREP_INT is wrong */
	}

	/** check sizes of internal type-definitions */
    if( ! clrpc_check_sizes() ) 
    {
		CLRPC_UPPER_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL
			, "clrpc_check_sizes() failed"
			);
		CLRPC_FATAL1(0); /* clrpc_check_sizes() failed */
	}

    /* initialize global clrpc_data */
    CLRPC_GET_100NS_COUNTER(&ret_val, &tmp_clkseq);
    if (ret_val != LSA_RET_OK)
    {
        CLRPC_FATAL1(ret_val);
    }
    clrpc_data.clkseq = (LSA_UINT16)(tmp_clkseq & CLRPC_CLOCK_SEQ_MASK);
    clrpc_data.counter_adjust = 0;
    clrpc_data.last_counter = 0;
    clrpc_data.last_boot_time = 0;
    clrpc_data.channels_ptr = &(clrpc_channels);
#ifdef CLRPC_CFG_MAX_SERVERS
    clrpc_data.servers_ptr = &clrpc_servers;
    clrpc_data.sc_hash_ptr = &clrpc_sc_hash;
    clrpc_data.ctx_hash_ptr = &clrpc_ctx_hash;
#endif
    clrpc_data.oneshot_ptr = &clrpc_oneshot;

	/** initialize channel data */
    for (i = 0; i < CLRPC_CFG_MAX_CHANNELS; ++i) 
    {
		clrpc_channels[i].state = CLRPC_ST_FREE;
	}

#if CLRPC_CFG_MAX_SERVERS
	/** initialize server data */
	{
	LSA_UINT16 rc = clrpc_sv_init ();

        if (rc != LSA_RET_OK) 
        {
		return rc;
	}
	}
#endif

	/** init all global vars */

	CLRPC_MEMSET (& clrpc_null_user_id, 0, sizeof (clrpc_null_user_id));

	clrpc_uuid_sys_init();

	clrpc_oneshot_sys_init(init->oneshot_lsa_time_base, init->oneshot_lsa_time_factor);

	/***/

#if CLRPC_DEBUG 

#ifdef CLRPC_MESSAGE
#pragma CLRPC_MESSAGE("attention: TEST-CASES configured!")
#endif

	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_RQB_TYPE )=(%u)"  
		, sizeof(CLRPC_RQB_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_ANY_ARGS_TYPE )=(%u)"  
		, sizeof(CLRPC_ANY_ARGS_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_LOWER_RQB_TYPE )=(%u)"  
		, sizeof(CLRPC_LOWER_RQB_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_INIT_TYPE )=(%u)"  
		, sizeof(CLRPC_INIT_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_DETAIL_TYPE )=(%u)"  
		, sizeof(CLRPC_DETAIL_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_CHANNEL_TYPE )=(%u)"  
		, sizeof(CLRPC_CHANNEL_TYPE)  
		);
#if CLRPC_CFG_MAX_CLIENTS
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_CL_CHANNEL_TYPE )=(%u)"  
		, sizeof(CLRPC_CL_CHANNEL_TYPE)  
		);
#endif
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_SV_CHANNEL_TYPE )=(%u)"  
		, sizeof(CLRPC_SV_CHANNEL_TYPE)  
		);
#if CLRPC_CFG_MAX_CLIENTS
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_CLIENT_TYPE )=(%u)"  
		, sizeof(CLRPC_CLIENT_TYPE)  
		);
#endif
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_SERVER_TYPE )=(%u)"  
		, sizeof(CLRPC_SERVER_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_SCALL_TYPE )=(%u)"  
		, sizeof(CLRPC_SCALL_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_HASH_ENTRY_TYPE )=(%u)"  
		, sizeof(CLRPC_HASH_ENTRY_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_HASH_TYPE )=(%u)"  
		, sizeof(CLRPC_HASH_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_HASH_ITERATOR_TYPE )=(%u)"  
		, sizeof(CLRPC_HASH_ITERATOR_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_PNIO_IN_ARGS_TYPE )=(%u)"  
		, sizeof(CLRPC_PNIO_IN_ARGS_TYPE)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_PNIO_OUT_ARGS_TYPE )=(%u)"  
		, sizeof(CLRPC_PNIO_OUT_ARGS_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( CLRPC_UUID_TYPE )=(%u)"  
		, sizeof(CLRPC_UUID_TYPE)  
		);
        
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( clrpc_channels )=(%u)"  
		, sizeof(clrpc_channels)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( clrpc_servers )=(%u)"  
		, sizeof(clrpc_servers)  
		);
	CLRPC_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_CHAT  
		, "sizeof( clrpc_sc_hash )=(%u)"  
		, sizeof(clrpc_sc_hash)  
		);
#endif

	return LSA_RET_OK;
}


/*-Export-----------------------------------------------------------------------
//	func: clrpc_undo_init
//	work: de-initialises the module CLRPC, must be called on shutdown
//----------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_undo_init(
    LSA_VOID
	)
{
	LSA_UINT16 i;

	clrpc_oneshot_sys_undo_init();

#if CLRPC_CFG_MAX_SERVERS
	clrpc_sv_undo_init ();
#endif

	for (i = 0; i < CLRPC_CFG_MAX_CHANNELS; ++i) {
		CLRPC_ASSERT(clrpc_channels[i].state == CLRPC_ST_FREE);
	}

    return LSA_RET_OK;
}


///*-Export-----------------------------------------------------------------------
////	func: clrpc_timeout
////	work:
////----------------------------------------------------------------------------*/
//LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR  clrpc_timeout(
//    LSA_UINT16        timer_id,
//    LSA_USER_ID_TYPE  user_id
//); /* implemented in clrpc_cmn.c */
//

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_get_sock_handle (
    LSA_HANDLE_TYPE  clrpc_handle
)
{
	CLRPC_LOG_FCT ("clrpc_get_sock_handle")
	CLRPC_CHANNEL_PTR_TYPE channel;

	CLRPC_ASSERT (clrpc_handle < CLRPC_CFG_MAX_CHANNELS);
	channel = &clrpc_channels[clrpc_handle];

	return channel->handle_lower;
}

/*------------------------------------------------------------------------------
//	func: clrpc_fatal1
//----------------------------------------------------------------------------*/

LSA_FATAL_ERROR_TYPE clrpc_fatal_var; /* for some debuggers that have problems with the stack-trace */

LSA_VOID  CLRPC_SYSTEM_IN_FCT_ATTR
clrpc_fatal1(
	LSA_UINT16 clrpc_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
) {
	CLRPC_LOG_FCT("clrpc_fatal1")
    
	clrpc_fatal_var.lsa_component_id = LSA_COMP_ID_CLRPC;
	clrpc_fatal_var.module_id        = clrpc_module_id;
	clrpc_fatal_var.line             = (LSA_UINT16)line;

	clrpc_fatal_var.error_code[0]    = ec_0;
	clrpc_fatal_var.error_code[1]    = 0;
	clrpc_fatal_var.error_code[2]    = 0;
	clrpc_fatal_var.error_code[3]    = 0;

	clrpc_fatal_var.error_data_length = 0;
	clrpc_fatal_var.error_data_ptr    = LSA_NULL;
    
	CLRPC_CL_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "fatal_error in module(%u) line(%u) error_code0(%u)"
		, clrpc_module_id, line, ec_0
		);

	CLRPC_FATAL_ERROR(sizeof(clrpc_fatal_var), &clrpc_fatal_var);
	/* not reached */
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
