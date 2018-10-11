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
/*  F i l e               &F: cm_cmn.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  common functions                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	4
#define CM_MODULE_ID        4

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*------------------------------------------------------------------------------
// Range
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_init (
	CM_RANGE_PTR_TYPE range
)
{
	CM_LOG_FCT ("cm_range_init")

	range->count = 0;
	range->arr = LSA_NULL;
}


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_undo_init (
	CM_RANGE_PTR_TYPE	range
)
{
	CM_LOG_FCT ("cm_range_undo_init")

	CM_ASSERT (is_not_null (range));

	if( range->count != 0 ) {

		CM_ASSERT(is_not_null(range->arr));

#if CM_DEBUG
		{
		LSA_UINT16  i;
		for (i = 0; i < ((range->count + 7) / 8); ++i) {
			if (range->arr[i] != 0) {
				CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
					, "range_id(%u) not free", i + range->base
					);
				CM_FATAL();
			}
		}
		}
#endif

		{
		LSA_UINT16  rc;
		CM_FREE_LOCAL_MEM (& rc, range->arr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG(rc);
		}
	}
	else {
		CM_ASSERT(is_null(range->arr));
	}
}

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_alloc (
	CM_RANGE_PTR_TYPE range,
	LSA_UINT16	base,
	LSA_UINT16	count
)
{
	CM_LOG_FCT ("cm_range_alloc")
	LSA_UINT32  alloc_size;
	LSA_UINT16  i;

	if( count != 0 ) {

		alloc_size = ((count + 7) / 8);

		if (alloc_size > 0xFFFF) { /* always false (see cast below) */
			CM_FATAL(); /* alloc-length is 16-bits */
			return LSA_FALSE; /* unreachable... */
		}

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& range->arr), CM_CAST_U16(alloc_size));

		if (is_null (range->arr)) {
			CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "!mem, base(%u) count(%u)", base, count
				);
			CM_ASSERT(range->count == 0);
			return LSA_FALSE;
		}

		range->base = base;
		range->count = count;

		for (i = 0; i < ((count + 7) / 8); ++i) {
			range->arr[i] = 0;
		}
	}
	else {
		CM_ASSERT(is_null(range->arr));
	}

	return LSA_TRUE;
}


#ifdef cm_range_idx_mask_from_id
# error "cm_range_idx_mask_from_id already defined"
#endif

#define cm_range_idx_mask_from_id(idx, mask, id) {\
	idx  = CM_CAST_U16( (id) / 8 ); \
	mask = CM_CAST_U8 ( 1 << ((id) % 8) ); \
	}

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_alloc_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16 CM_COMMON_MEM_ATTR * id_ptr
)
{
	CM_LOG_FCT ("cm_range_alloc_id")
	LSA_UINT16  i;
	LSA_UINT16  idx;
	LSA_UINT8  mask;

	CM_ASSERT (is_not_null (range));

	for (i = 0; i < range->count; ++i) {

		cm_range_idx_mask_from_id (idx, mask, i);

		if ((range->arr[idx] & mask) == 0) {
			range->arr[idx] |= mask;
			*id_ptr = CM_CAST_U16(range->base + i);
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}


LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_reserve_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16  id
)
{
	CM_LOG_FCT ("cm_range_reserve_id")
	LSA_UINT16  idx;
	LSA_UINT8  mask;

	CM_ASSERT (is_not_null (range));

	if (! (range->base <= id && id < range->base + range->count)) {
		CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP
			, "id(0x%x) out of range, base(0x%x) count(%u)"
			, id, range->base, range->count
			);
		return LSA_FALSE;
	}

	id = CM_CAST_U16(id - range->base);

	cm_range_idx_mask_from_id (idx, mask, id);

	if ((range->arr[idx] & mask) != 0) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "id(0x%x) already reserved", id
			);
		return LSA_FALSE;
	}

	range->arr[idx] |= mask;
	return LSA_TRUE;
}


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_free_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16			id
)
{
	CM_LOG_FCT ("cm_range_free_id")
	LSA_UINT16  idx;
	LSA_UINT8  mask;

	CM_ASSERT (is_not_null (range));

	if (! (range->base <= id && id < range->base + range->count)) {
		CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
			, "id(0x%x) out of range, base(%u) count(%u)", id, range->base, range->count
			);
		CM_FATAL(); /* a bug */
		return;
	}

	id = CM_CAST_U16(id - range->base);

	cm_range_idx_mask_from_id (idx, mask, id);

	CM_ASSERT ((range->arr[idx] & mask) != 0);

	range->arr[idx] &= ~mask;
}



/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
CmListInsertSorted (
	CM_LIST_ENTRY_PTR_TYPE  ListHead,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry,
	CmListCmpFunctionType  Compare_ListEntry_LT_NewEntry
	)
{
	CM_LIST_ENTRY_PTR_TYPE  ListEntry;

	for (CmListForeach (ListEntry, ListHead, CM_LIST_ENTRY_PTR_TYPE)) {

		if (! Compare_ListEntry_LT_NewEntry (ListEntry, NewEntry)) {
			break;
		}
	}

	if (is_null(ListEntry)) {
		CmListInsertTail (ListHead, NewEntry);
	}
	else {
		CmListInsertBefore (ListEntry, NewEntry);
	}
}


/*----------------------------------------------------------------------------*/


LSA_UINT16  CM_UPPER_IN_FCT_ATTR	/* ret: 0..error  else..valid format_1-sub_state */
cm_sub_state_to_format_1 (
	LSA_UINT16  sub_state			/* in.: sub_state in format_0(must convert) or format_1(do pass) */
	)
{
	CM_LOG_FCT ("cm_sub_state_to_format_1")
	LSA_UINT16  out;

		/* if format_1 */
	if ((sub_state & CM_SUB_STATE_FORMAT_MASK) == CM_SUB_STATE_FORMAT_1) {
		return sub_state;
	}

		/* else format_0 */
	switch (sub_state & CM_SUB_STATE_DETAIL_MASK) {
	case CM_SUB_STATE_DETAIL_NO_SUBMODULE:
		out = CM_SUB_STATE_FORMAT_1
				| CM_SUB_STATE_ADD_INFO_NONE
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_AR_INFO_OWNED
				| CM_SUB_STATE_IDENT_NO_SUBMODULE
			;
		break;

	case CM_SUB_STATE_DETAIL_WRONG_SUBMODULE:
		out = CM_SUB_STATE_FORMAT_1
				| CM_SUB_STATE_ADD_INFO_NONE
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_AR_INFO_OWNED
				| CM_SUB_STATE_IDENT_WRONG
			;
		break;

	case CM_SUB_STATE_DETAIL_LOCKED_IOCONTROLLER:
		out = CM_SUB_STATE_FORMAT_1
				| CM_SUB_STATE_ADD_INFO_NONE
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER
				| CM_SUB_STATE_IDENT_OK
			;
		break;

	case CM_SUB_STATE_DETAIL_APPL_READY_PENDING:
		out = CM_SUB_STATE_FORMAT_1
				| CM_SUB_STATE_ADD_INFO_NONE
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_AR_INFO_APPL_READY_PENDING
				| CM_SUB_STATE_IDENT_OK
			;
		break;

	case CM_SUB_STATE_DETAIL_SUBSTITUTED_SUBMODULE:
		out = CM_SUB_STATE_FORMAT_1
				| CM_SUB_STATE_ADD_INFO_NONE
				| CM_SUB_STATE_QUALIFIED_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED
				| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED
				| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE
				| CM_SUB_STATE_AR_INFO_OWNED
				| CM_SUB_STATE_IDENT_SUBSTITUTE
			;
		break;

	case CM_SUB_STATE_DETAIL_RESERVED_3:
	case CM_SUB_STATE_DETAIL_RESERVED_5:
	case CM_SUB_STATE_DETAIL_RESERVED_6:
	default:
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong sub_state(0x%x)", sub_state
			);
		out = 0; /* 0 .. not a valid format_1 */
		break;
	}

	return out;
}


/*------------------------------------------------------------------------------
// work: alloc / free internal used cm-upper rqb
//----------------------------------------------------------------------------*/
CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_upper_alloc_rqb (
	CM_CHANNEL_PTR_TYPE channel
)
{
	CM_LOG_FCT ("cm_upper_alloc_rqb")
	CM_UPPER_RQB_PTR_TYPE  upper;

	CM_ALLOC_UPPER_RQB (& upper, cm_null_user_id, sizeof (*upper), channel->sysptr);

	if (is_null (upper)) {
		CM_AR_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!rqb-mem");
		return LSA_NULL;
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& upper->args.void_ptr),
			cm_null_user_id, sizeof (CM_ANY_ARGS_TYPE), channel->sysptr
		);
	if (is_null (upper->args.void_ptr)) {
		LSA_UINT16  rc;
		CM_AR_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!upper-mem");
		CM_FREE_UPPER_RQB (& rc, upper, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		return LSA_NULL;
	}

	return upper;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_upper_free_rqb (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  upper
)
{
	CM_LOG_FCT ("cm_upper_free_rqb")
	LSA_UINT16  rc;

	CM_FREE_UPPER_MEM (& rc, upper->args.void_ptr, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	CM_FREE_UPPER_RQB (& rc, upper, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);
}


/*===========================================================================*/
/*=== RECORD-INDEX CLASSIFICATION ===========================================*/
/*===========================================================================*/

#define CM_REC_IDX_NORM_READ_ONLY         (CM_REC_IDX_NORM | CM_REC_IDX_READ) /* normative indices are read-only */

#define CM_REC_IDX_USER_READ_WRITE        (CM_REC_IDX_USER | CM_REC_IDX_READ | CM_REC_IDX_WRITE) /* user decides if writable or not */
#define CM_REC_IDX_USER_READ_ONLY         (CM_REC_IDX_USER | CM_REC_IDX_READ)

#define CM_REC_IDX_PDEV_READ_WRITE(sc_)  (CM_REC_IDX_PDEV | CM_REC_IDX_READ | CM_REC_IDX_WRITE | (sc_)) /* pdev decides if writable or not */
#define CM_REC_IDX_PDEV_READ_ONLY(sc_)   (CM_REC_IDX_PDEV | CM_REC_IDX_READ | (sc_)) /* some pdev indices are read-only */
#define CM_REC_IDX_PDEV_WRITE_ONLY(sc_)  (CM_REC_IDX_PDEV | CM_REC_IDX_WRITE | (sc_)) /* some pdev indices are write-only */
#define CM_REC_IDX_PDEV_READ_LOCAL(sc_)  (CM_REC_IDX_PDEV | CM_REC_IDX_READ_LOCAL | (sc_)) /* some pdev indices are read-only locally */
#define CM_REC_IDX_PDEV_WRITE_LOCAL(sc_) (CM_REC_IDX_PDEV | CM_REC_IDX_WRITE_LOCAL | (sc_)) /* some pdev indices are write-only locally */

#define CM_REC_IDX_NONPRM_BUT_MKREMA      (CM_REC_IDX_NONPRM | CM_REC_IDX_MKREMA)

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_record_index_classify(
	LSA_UINT32 api,
	LSA_UINT32 index32,
	LSA_UINT16 vendor_id
) {
	CM_LOG_FCT("cm_record_index_classify")
	LSA_UINT32 cls;

	/***/

	cls = 0; /* reserved := neither readable nor writeable */

	/***/

	if( index32 > 0xFFFF ) { /* extended record index */

		/* these records are only allowed from "local" (user, not via rpc) */

		switch (index32) {

		/* Submodule specifics records (0x00010000 .. 0x0FFFFFFF) */
		case 0x00010000: /* SendClockRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
			/* note: assigned to EDD because it can be read back from EDD */
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x00010001: /* PDSetDefaultPortState (has effect only in first PrmSequence) */
			cls |= CM_REC_IDX_PDEV_WRITE_ONLY (CM_REC_IDX_CMPD_EDD);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0x00010002: /* PDIRApplicationData (AP00964333 and AP01317720) */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_PORT;
			break;

		case 0x00010100: /* DcpNameOfStationRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00010101: /* DcpIPSuiteRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00011000: /* SNMPRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0x00011001: /* SysNameRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0x00011002: /* SysContactRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0x00011003: /* SysLocationRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

#if CM_CFG_USE_GSY
		case 0x00011020: /* PDSyncPLL */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_CLOCK);
			cls |= CM_REC_IDX_INTERFACE;
			break;
#endif

		case 0x00017081: /* PDMasterTailorData */
			cls |= CM_REC_IDX_PDEV_WRITE_ONLY (CM_REC_IDX_CMPD_PD);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x00023000: /* IPSuiteRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00023002: /* DHCPParametersRecord (TIA 965833, V5.3i2.x) */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00023003: /* NameOfStationRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00023008: /* TCPKeepAliveRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0x00023100: /* PNIdentification */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x00023110: /* CommonBusParamsRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		case 0x00023111: /* AdditionalBusParamsRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

#if CM_CFG_USE_MRP
		case 0x80008052: /* PrivInterfaceMrpDataAdjust */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80008054: /* PrivInterfaceMrpDataReal (AP01491345) */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80009000: /* PrivInterfaceHSRDataAdjust */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80009001: /* PrivInterfaceSTBYDataAdjust */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80009002: /* PrivInterfaceHSRDataReal */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80009003: /* PrivInterfaceSTBYDataReal */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x80009004: /* PrivInterfaceARDDataReal */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			break;

		case 0x8000D000: /* PrivInterfaceHSRConfig */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;
#endif

		case 0xF8000000: /* MultiplePDEVRecord */
			cls |= CM_REC_IDX_PDEV_WRITE_ONLY (CM_REC_IDX_CMPD_PD);
			/* note: slot/subslot don't care */
			break;

		case 0xF8000001: /* MultipleStationRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM;
			break;

		case 0xF8000002: /* MultipleSNMPRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA; /* TIA 1205368: MKREMA added with V5.4i2.13 */
			break;

		case 0xF8000003: /* MultipleBusParamsRecord */
			cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
			cls |= CM_REC_IDX_INTERFACE;
			cls |= CM_REC_IDX_NONPRM_BUT_MKREMA;
			break;

		default:
			/* reserved */
			CM_AR_TRACE_01 (0,LSA_TRACE_LEVEL_NOTE, "RecordIndexClassify: unknown index32(0x%x)", index32);
			break;
		}
	}
	else { /* see PNIO-Spec */

		LSA_UINT32 index16 = CM_CAST_U16(index32);

		if( index16 < 0x8000 ) { /* user specific */

			cls |= CM_REC_IDX_0000;

			cls |= CM_REC_IDX_USER_READ_WRITE;
		}
		else if( index16 < 0xC000 ) { /* subslot specific */

			cls |= CM_REC_IDX_8000;

			if( index16 < 0xAFF0 ) {

				switch( index16 ) {

				case 0x8000: /* ExpectedIdentificationData */
				case 0x8001: /* RealIdentificationData */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0x800A: /* Diagnosis in channel coding */
				case 0x800B: /* Diagnosis in all codings */
				case 0x800C: /* Diagnosis, Maintenance, Qualified and Status */
				case 0x8010: /* Maintenance required in channel coding */
				case 0x8011: /* Maintenance demanded in channel coding */
				case 0x8012: /* Maintenance required in all codings */
				case 0x8013: /* Maintenance demanded in all codings */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					cls |= CM_REC_IDX_DIAG;
					break;

				case 0x801E: /* SubstituteValues */
					cls |= CM_REC_IDX_USER_READ_WRITE;
					break;

#if CM_CFG_RT_CLASS_3_SUPPORT_ON
				case 0x8020: /* PDIRSubframeData */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
					cls |= CM_REC_IDX_INTERFACE;
					break;
#endif

				case 0x8028: /* RecordInputDataObjectElement */
				case 0x8029: /* RecordOutputDataObjectElement */
					cls |= CM_REC_IDX_USER_READ_ONLY;
					break;

				case 0x802A: /* PDPortDataReal */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_OHA);
					cls |= CM_REC_IDX_PORT;
					break;

				case 0x802B: /* PDPortDataCheck */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
					cls |= CM_REC_IDX_PORT;
					break;

#if CM_CFG_RT_CLASS_3_SUPPORT_ON
				case 0x802C: /* PDIRData */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
					cls |= CM_REC_IDX_INTERFACE;
					break;
#endif

#if CM_CFG_RT_CLASS_3_SUPPORT_ON || CM_CFG_RT_CLASS_2_SUPPORT_ON /* AP00456188 (enable synchronized RTClass2 operation) */
#if CM_CFG_USE_GSY
				case 0x802D: /* Expected PDSyncData with SyncID value 0 */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_CLOCK);
					cls |= CM_REC_IDX_INTERFACE;
					break;
#endif
#endif

				case 0x802F: /* PDPortDataAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
					cls |= CM_REC_IDX_PORT;
					break;

#if CM_CFG_RT_CLASS_3_SUPPORT_ON
				case 0x8030: /* IsochronousModeData */
					cls |= CM_REC_IDX_USER_READ_WRITE;
					break;
#endif

#if CM_CFG_USE_GSY
				case 0x8031: /* Expected PDSyncData with SyncID value 1 */
				/*-- 0x8032 - 0x804F Expected PDSyncData with SyncID value 2..31 */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_TIME);
					cls |= CM_REC_IDX_INTERFACE;
					break;
#endif

#if CM_CFG_USE_MRP
				case 0x8050: /* PDInterfaceMrpDataReal */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_MRP);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x8051: /* PDInterfaceMrpDataCheck */
				case 0x8052: /* PDInterfaceMrpDataAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x8053: /* PDPortMrpDataAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_MRP);
					cls |= CM_REC_IDX_PORT;
					break;

				case 0x8054: /* PDPortMrpDataReal */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_MRP);
					cls |= CM_REC_IDX_PORT;
					break;
#else
					/*
					 * note: if not configured, must look like a "reserved" index (18.7.2006)
					 */
#endif

#if CM_CFG_USE_POF
				case 0x8060: /* PDPortFODataReal */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_POF);
					cls |= CM_REC_IDX_PORT;
					break;

				case 0x8061: /* PDPortFODataCheck */
				case 0x8062: /* PDPortFODataAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_POF);
					cls |= CM_REC_IDX_PORT;
					break;
#else
					/*
					 * note: if not configured, must look like a "reserved" index (18.7.2006)
					 */
#endif

				case 0x8070: /* PDNCDataCheck */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x8071: /* PDInterfaceAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x8072: /* PDPortStatistic */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_EDD);
					cls |= CM_REC_IDX_INTERFACE;
					cls |= CM_REC_IDX_PORT;
					break;

				case 0x8080: /* PDInterfaceDataReal */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_OHA);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x8090: /* PDInterfaceFSUDataAdjust */
					cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_OHA);
					cls |= CM_REC_IDX_INTERFACE;
					break;

				case 0x80A0: /* PROFIenergy Record_0  TIA 1094125 (obsoleting AP00906596, AP00893198) */
					cls |= CM_REC_IDX_USER_READ_WRITE;
					break;

				default:
					/* reserved */
					break;
				}
			}
			else if( index16 < 0xB000 ) {

				/* 0xAFF0 .. 0xAFFF = I&M 0-15 */

				switch( index16 ) {

				case 0xAFF0: /* I&M0 */
					cls |= CM_REC_IDX_USER_READ_ONLY;
					break;

				case 0xAFF1: /* I&M1 */
				case 0xAFF2: /* I&M2 */
				case 0xAFF3: /* I&M3 */
				case 0xAFF4: /* I&M4 */
					cls |= CM_REC_IDX_USER_READ_WRITE;
					cls |= CM_REC_IDX_IM1234;
					break;

				case 0xAFF5: /* I&M5 (V5.4i1.2x, TIA 1092255) */
					cls |= CM_REC_IDX_NORM_READ_ONLY; /* see cm_sv_record_read_normative() */
					break;

				default:
					/* reserved (because no definition, see too AP01467022) */
					break;
				}
			}
			else { /* 0xB000 .. 0xBFFF = Profiles */

				LSA_BOOL is_simatic = LSA_FALSE;

				if( api == 0 ) {

#if CM_CFG_SIMATIC_B000
					is_simatic = LSA_TRUE;
#endif

					if( vendor_id == CM_VENDOR_ID_SIEMENS ) { /* AP00579173: enable always if vendor is "Siemens" */
						is_simatic = LSA_TRUE;
					}
				}

				if( is_simatic ) { /* special case, see 25.conclave */

					cls |= CM_REC_IDX_SIMATIC;
					cls |= CM_REC_IDX_NONPRM; /* these special records are not(!) affected by the A-B-buffer-model */

					switch( index16 ) {

#if CM_CFG_USE_GSY
					case 0xB000: /* GSY: Sync-Log, SyncID 0 */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_CLOCK);
						cls |= CM_REC_IDX_INTERFACE;
						break;
#endif

#if CM_CFG_USE_GSY
					case 0xB001: /* GSY: Sync-Log, SyncID 1 */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_TIME);
						cls |= CM_REC_IDX_INTERFACE;
						break;
#endif

					case 0xB050: /* EDD: Ext_PLL control, SyncID 0 */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
						cls |= CM_REC_IDX_INTERFACE;
						break;

#if CM_CFG_USE_GSY
					case 0xB051: /* GSY: Ext_PLL control, for SyncID 1 */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_GSY_TIME);
						cls |= CM_REC_IDX_INTERFACE;
						break;
#endif

					case 0xB060: /* EDD: Trace-Unit control */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
						cls |= CM_REC_IDX_PORT;
						break;

					case 0xB061: /* EDD: Trace-Unit data */
						cls |= CM_REC_IDX_PDEV_READ_WRITE (CM_REC_IDX_CMPD_EDD);
						cls |= CM_REC_IDX_INTERFACE;
						break;

					case 0xB070: /* OHA: PDInterfaceAdditionalInfo */
						cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_OHA);
						cls |= CM_REC_IDX_INTERFACE;
						break;

					case 0xB071: /* OHA: SnmpControlRecord */
						cls |= CM_REC_IDX_PDEV_WRITE_LOCAL (CM_REC_IDX_CMPD_OHA);
						cls |= CM_REC_IDX_INTERFACE;
						break;

					case 0xB080: /* CM: PDInstanceTailorData */
						cls |= CM_REC_IDX_PDEV_WRITE_LOCAL (CM_REC_IDX_CMPD_PD);
						cls |= CM_REC_IDX_INTERFACE;
						break;

					case 0xB081: /* CM: PDIOSystemInfo */
						cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_PD);
						cls |= CM_REC_IDX_INTERFACE;
						break;

					default:
						/* reserved */
						break;
					}
				}
				else {

					cls |= CM_REC_IDX_USER_READ_WRITE;
				}
			}
		}
		else if( index16 < 0xE000 ) { /* slot specific (read only! see AP01556954) */

			cls |= CM_REC_IDX_C000;

			if( index16 < 0xD000 ) {

				switch( index16 ) {

				case 0xC000: /* ExpectedIdentificationData */
				case 0xC001: /* RealIdentificationData */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0xC00A: /* Diagnosis in channel coding */
				case 0xC00B: /* Diagnosis in all codings */
				case 0xC00C: /* Diagnosis, Maintenance, Qualified and Status */
				case 0xC010: /* Maintenance required in channel coding */
				case 0xC011: /* Maintenance demanded in channel coding */
				case 0xC012: /* Maintenance required in all codings */
				case 0xC013: /* Maintenance demanded in all codings */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					cls |= CM_REC_IDX_DIAG;
					break;

				default:
					/* reserved */
					break;
				}
			}
			else {

				/* 0xD000 .. 0xDFFF = Profiles */

				cls |= CM_REC_IDX_USER_READ_ONLY; /* AP01556954 */
			}
		}
		else if( index16 < 0xF000 ) { /* ar specific (read only with exceptions! see AP01556954) */

			cls |= CM_REC_IDX_E000;

			if( index16 < 0xEC00 ) {

				switch( index16 ) {

				case 0xE000: /* ExpectedIdentificationData */
				case 0xE001: /* RealIdentificationData */
				case 0xE002: /* ModuleDiffBlock */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0xE00A: /* Diagnosis in channel coding */
				case 0xE00B: /* Diagnosis in all codings */
				case 0xE00C: /* Diagnosis, Maintenance, Qualified and Status */
				case 0xE010: /* Maintenance required in channel coding */
				case 0xE011: /* Maintenance demanded in channel coding */
				case 0xE012: /* Maintenance required in all codings */
				case 0xE013: /* Maintenance demanded in all codings */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					cls |= CM_REC_IDX_DIAG;
					break;

				case 0xE040: /* WriteMultiple */
					/* write-multiple is handled differently, see cm_sv_rpc_record_dispatch() */
					break;

				case 0xE050: /* ARFSUDataAdjust data */
					cls |= CM_REC_IDX_USER_READ_WRITE; /* write goes to user, read is handled internally */
					break;

				default:
					/* reserved */
					break;
				}
			}
			else {

				/* 0xEC00 .. 0xEFFF = Profiles */

				cls |= CM_REC_IDX_USER_READ_ONLY; /* AP01556954 */
			}
		}
		else if( index16 < 0xF800 ) { /* api specific (read only! see AP01556954) */

			cls |= CM_REC_IDX_F000;

			if( index16 < 0xF400 ) {

				switch( index16 ) {

				case 0xF000: /* RealIdentificationData */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0xF00A: /* Diagnosis in channel coding */
				case 0xF00B: /* Diagnosis in all codings */
				case 0xF00C: /* Diagnosis, Maintenance, Qualified and Status */
				case 0xF010: /* Maintenance required in channel coding */
				case 0xF011: /* Maintenance demanded in channel coding */
				case 0xF012: /* Maintenance required in all codings */
				case 0xF013: /* Maintenance demanded in all codings */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					cls |= CM_REC_IDX_DIAG;
					break;

				case 0xF020: /* ARData for one API */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				default:
					/* reserved */
					break;
				}
			}
			else {

				/* 0xF400 .. 0xF7FF = Profiles */

				cls |= CM_REC_IDX_USER_READ_ONLY; /* AP01556954 */
			}
		}
		else { /* device specific (read only! see AP01556954) */

			cls |= CM_REC_IDX_F800;

			if( index16 < 0xFC00 ) {

				switch( index16 ) {

				case 0xF80C: /* Diagnosis, Maintenance, Qualified and Status */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					cls |= CM_REC_IDX_DIAG;
					break;

				case 0xF820: /* ARData for one device */
				case 0xF821: /* APIData */
				case 0xF830: /* LogBookData (LogData in PNIO-Spec V2.2) */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0xF831: /* PDevData */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_PD);
					/* note: will be re-classified to normative in the future */
					break;

				case 0xF840: /* I&M0FilterData */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;

				case 0xF841: /* PDRealData */
				case 0xF842: /* PDExpectedData */
					cls |= CM_REC_IDX_PDEV_READ_ONLY (CM_REC_IDX_CMPD_PD);
					/* note: will be re-classified to normative in the future */
					break;

				case 0xFBFF: /* Trigger index for the RPC connection monitoring */
					cls |= CM_REC_IDX_NORM_READ_ONLY;
					break;
				
				default:
					/* reserved */
					break;
				}
			}
			else {

				/* 0xFC00 .. 0xFFFF = Profiles */

				cls |= CM_REC_IDX_USER_READ_ONLY; /* AP01556954 */
			}
		}
	}

	/***/

	CM_AR_TRACE_02(0,LSA_TRACE_LEVEL_CHAT, "index(0x%08x) classification(0x%08x)", index32, cls);

	return cls;
}

/*===========================================================================*/
/*=== RECORD-MARSHAL-DIAG TEST-IF-YES =======================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_record_marshal_diag_yes(
	LSA_UINT16 index,
	LSA_BOOL is_generic, /* false: channel or ext-channel lists, true: generic-list */
	LSA_UINT16 prop
) {
	/*
	 * [8CEF]00A: Diagnosis in channel coding
	 * [8CEF]00B: Diagnosis in all codings
	 * [8CEF]00C: Diagnosis, Maintenance, Qualified and Status
	 * [8CEF]010: Maintenance required in channel coding
	 * [8CEF]011: Maintenance demanded in channel coding
	 * [8CEF]012: Maintenance required in all codings
	 * [8CEF]013: Maintenance demanded in all codings
	 *      F80C: Diagnosis, Maintenance, Qualified and Status for one device
	 */

	switch( (index & 0xFF) ) {

	case 0x0A:
		/* Diagnosis in channel coding ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, and QualifiedChannelDiagnosis
		 *     with ChannelProperties.Maintenance(=diagnosis) and ChannelProperties.Specifier(=appear)
		 */
		if( is_generic ) {
			return LSA_FALSE;
		}
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	case 0x0B:
		/* Diagnosis in all codings ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, QualifiedChannelDiagnosis, and ManufacturerSpecificDiagnosis
		 *     with ChannelProperties.Maintenance(=diagnosis) and ChannelProperties.Specifier(=appear)
		 */
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	case 0x0C:
		/* Diagnosis, Maintenance, Qualified and Status ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, QualifiedChannelDiagnosis, ManufacturerSpecificDiagnosis
		 *     with ChannelProperties.Maintenance(=any) and ChannelProperties.Specifier(=appear),
		 *   and ManufacturerSpecificDiagnosis
		 *     with ChannelProperties.Maintenance(=diagnosis) and ChannelProperties.Specifier(=disappear)
		 */
		if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
			return LSA_TRUE;
		}
		if( is_generic ) {
			if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR) ) {
				if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_DIS ) {
					return LSA_TRUE;
				}
			}
		}
		return LSA_FALSE;

	case 0x10:
		/* Maintenance required in channel coding ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, and QualifiedChannelDiagnosis
		 *     with ChannelProperties.Maintenance(=MaintenanceRequired) and ChannelProperties.Specifier(=appear)
		 */
		if( is_generic ) {
			return LSA_FALSE;
		}
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_REQ) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	case 0x11:
		/* Maintenance demanded in channel coding ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, and QualifiedChannelDiagnosis
		 *     with ChannelProperties.Maintenance(=MaintenanceDemanded) and ChannelProperties.Specifier(=appear)
		 */
		if( is_generic ) {
			return LSA_FALSE;
		}
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_DEM) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	case 0x12:
		/* Maintenance required in all codings ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, QualifiedChannelDiagnosis, and ManufacturerSpecificDiagnosis
		 *     with ChannelProperties.Maintenance(=MaintenanceRequired) and ChannelProperties.Specifier(=appear)
		 */
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_REQ) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	case 0x13:
		/* Maintenance demanded in all codings ::=
		 *   ChannelDiagnosisData, ExtChannelDiagnosisData, QualifiedChannelDiagnosis, and ManufacturerSpecificDiagnosis
		 *     with ChannelProperties.Maintenance(=MaintenanceDemanded) and ChannelProperties.Specifier(=appear)
		 */
		if( ((prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_DEM) ) {
			if( (prop & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {
				return LSA_TRUE;
			}
		}
		return LSA_FALSE;

	default:
		CM_FATAL();
		return LSA_FALSE; /* unreachable */
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
