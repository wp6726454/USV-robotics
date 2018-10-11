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
/*  F i l e               &F: cm_sys.c                                  :F&  */
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
#define LTRC_ACT_MODUL_ID  1
#define CM_MODULE_ID       1

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/

/*lint -e(708) Info 708: union initialization */
const LSA_USER_ID_TYPE cm_null_user_id = { 0 }; /* handy to have */
/*
  see ansi-c:
	A brace-enclosed initializer for a union object initializes the
	member that appears first in the declaration list of the union type.
  ad ms-vs2010:
	c99 feature "designator" doesn't work:
	const LSA_USER_ID_TYPE cm_null_user_id = { .uvar32 = 0 }; // compile error
*/

const CLRPC_UUID_TYPE cm_uuid_nil_uuid = { 0, 0, 0, 0, 0, { 0,0,0,0,0,0 } };


/*------------------------------------------------------------------------------
//	func: cm_init
//	work: initialises the module CM, must be called on startup
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_SYSTEM_IN_FCT_ATTR
cm_init (
	CM_INIT_PTR_TYPE init
) {
	CM_LOG_FCT ("cm_init")

	CM_UPPER_TRACE_02 (0, LSA_TRACE_LEVEL_NOTE
		, "oneshot_lsa_time_base(%u) oneshot_lsa_time_factor(%u)"
		, init->oneshot_lsa_time_base, init->oneshot_lsa_time_factor
		);

	/***/

	{
	LSA_INT must;

	/*
	 * LSA_FALSE must be 0 or many things will go wrong...
	 * and LSA_TRUE must not be LSA_FALSE...
	 */

#if 0 /* show #define of LSA_TRUE / LSA_FALSE */
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("LSA_FALSE is defined as '" CM_STRINGIFY(LSA_FALSE) "'")
#pragma CM_MESSAGE("LSA_TRUE is defined as '" CM_STRINGIFY(LSA_TRUE) "'")
#endif /* CM_MESSAGE */
#endif

	must = ( LSA_FALSE == 0 && LSA_TRUE != LSA_FALSE );

	if( ! must ) {
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL
			, "strange TRUE/FALSE"
			);
		CM_FATAL();
	}

	/*
	 * but: LSA_NULL needs not to be 0. some may set it to -1 or some other special value!
	 *
	 * don't: if( LSA_NULL == 0 ) ...
	 */

#if 0 /* show #define of LSA_NULL */
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("LSA_NULL is defined as '" CM_STRINGIFY(LSA_NULL) "'")
#endif /* CM_MESSAGE */
#endif

#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: test with LSA_NULL != 0")*/
/*currently no one dares to do this (because of lack of a clean c-lib)*/
#endif /* CM_MESSAGE */

	/*
	 * the RQB must start with the two pointers or many things will go wrong...
	 */

	must = (LSA_UINT32)&CM_RQB_GET_NEXT_RQB_PTR((CM_UPPER_RQB_PTR_TYPE)0) == 0
		&& (LSA_UINT32)&CM_RQB_GET_PREV_RQB_PTR((CM_UPPER_RQB_PTR_TYPE)0) == sizeof(LSA_VOID_PTR_TYPE)
		; /* must be always true */

	if( ! must ) {
		CM_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL
			, "strange RQB"
			);
		CM_FATAL();
	}
	}

	/*
	 * init all global vars
	 */

	cm_oneshot_sys_init(init->oneshot_lsa_time_base, init->oneshot_lsa_time_factor);

	/***/

	cm_channels_sys_init();

	/***/

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
//	func: cm_undo_init
//	work: de-initialises the module CM, must be call on shutdown
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_SYSTEM_IN_FCT_ATTR
cm_undo_init(
	LSA_VOID
) {
	CM_LOG_FCT ("cm_undo_init")

	/***/

	cm_channels_sys_undo_init();

	/***/

	cm_oneshot_sys_undo_init();

	/***/

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
//	func: cm_timeout
//	work:
//----------------------------------------------------------------------------*/
LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_timeout (
	LSA_TIMER_ID_TYPE  timer_id,
	LSA_USER_ID_TYPE  user_id
); /* implemented in cm_tim.c */


/*------------------------------------------------------------------------------
//	func: cm_fatal
//----------------------------------------------------------------------------*/

LSA_FATAL_ERROR_TYPE cm_fatal_var; /* for some debuggers that have problems with the stack-trace */

LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_fatal(
	LSA_UINT16 cm_module_id,
	LSA_INT line
) {
	cm_fatal1(cm_module_id, line, 0);
}


LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_fatal1(
	LSA_UINT16 cm_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
) {
	CM_LOG_FCT ("cm_fatal1")

	CM_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "module_id(%u) line(%u) ec_0(0x%x)"
		, cm_module_id, line, ec_0
		);

	cm_fatal_var.lsa_component_id = LSA_COMP_ID_CM;
	cm_fatal_var.module_id        = cm_module_id;
	cm_fatal_var.line             = (LSA_UINT16)line;

	cm_fatal_var.error_code[0]    = ec_0;
	cm_fatal_var.error_code[1]    = 0;
	cm_fatal_var.error_code[2]    = 0;
	cm_fatal_var.error_code[3]    = 0;

	cm_fatal_var.error_data_length = 0;
	cm_fatal_var.error_data_ptr    = LSA_NULL;

	CM_FATAL_ERROR(sizeof(cm_fatal_var), &cm_fatal_var);
	/* not reached */
}

/*------------------------------------------------------------------------------
//	func: UUID utility functions
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_SYSTEM_IN_FCT_ATTR
cm_uuid_equal (
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid1,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid2
) {
	/* note: same logic as clrpc_uuid_equal(), see AP01196938 (CM must not call CLRPC directly) */

	if ((uuid1->time_low == uuid2->time_low)
	&& (uuid1->time_mid == uuid2->time_mid)
	&& (uuid1->time_hi_and_version == uuid2->time_hi_and_version)
	&& (uuid1->clock_seq_hi_and_reserved == uuid2->clock_seq_hi_and_reserved)
	&& (uuid1->clock_seq_low == uuid2->clock_seq_low)
	&& uuid1->node[0] == uuid2->node[0]
	&& uuid1->node[1] == uuid2->node[1]
	&& uuid1->node[2] == uuid2->node[2]
	&& uuid1->node[3] == uuid2->node[3]
	&& uuid1->node[4] == uuid2->node[4]
	&& uuid1->node[5] == uuid2->node[5]
	) {
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/*---------------------------------------------------------------------------*/

LSA_BOOL  CM_SYSTEM_IN_FCT_ATTR
cm_uuid_is_nil (
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const uuid
) {
	return cm_uuid_equal (uuid, & cm_uuid_nil_uuid);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
