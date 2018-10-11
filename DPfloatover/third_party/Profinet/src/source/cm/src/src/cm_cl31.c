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
/*  F i l e               &F: cm_cl31.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for rpc / prm / records                                  */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	631
#define CM_MODULE_ID		631

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS


static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_prm_record_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_prm_request_next (
	CM_AR_GRAPH_PTR_TYPE  argr
);
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_record_read_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_WRITE_seq_number(u16_)			cm_put_u16    (dp, CM_BLK_READ_WRITE, seq_number, (u16_))
#define cm_put_REQ_WRITE_ar_uuid(uuidp_)			cm_put_uuid   (dp, CM_BLK_READ_WRITE, ar_uuid, (uuidp_))
#define cm_put_REQ_WRITE_api(u32_)					cm_put_u32    (dp, CM_BLK_READ_WRITE, api, (u32_))
#define cm_put_REQ_WRITE_slot_nr(u16_)				cm_put_u16    (dp, CM_BLK_READ_WRITE, slot_nr, (u16_))
#define cm_put_REQ_WRITE_subslot_nr(u16_)			cm_put_u16    (dp, CM_BLK_READ_WRITE, subslot_nr, (u16_))
#define cm_put_REQ_WRITE_padding_1					cm_put_memset (dp, CM_BLK_READ_WRITE, padding_1, 0)
#define cm_put_REQ_WRITE_index(u16_)				cm_put_u16    (dp, CM_BLK_READ_WRITE, index, (u16_))
#define cm_put_REQ_WRITE_record_data_length(u32_)	cm_put_u32    (dp, CM_BLK_READ_WRITE, record_data_length, (u32_))
#define cm_put_REQ_WRITE_padding_2					cm_put_memset (dp, CM_BLK_READ_WRITE, u.req_wr.padding_2, 0)

/*----------------------------------------------------------------------------*/

#define cm_get_RSP_WRITE_seq_number(u16p_)			cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, seq_number)
#define cm_get_RSP_WRITE_ar_uuid(uuidp_)			cm_get_uuid (uuidp_, dp, CM_BLK_READ_WRITE, ar_uuid)
#define cm_get_RSP_WRITE_api(u32p_)					cm_get_u32  (u32p_,  dp, CM_BLK_READ_WRITE, api)
#define cm_get_RSP_WRITE_slot_nr(u16p_)				cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, slot_nr)
#define cm_get_RSP_WRITE_subslot_nr(u16p_)			cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, subslot_nr)
#define cm_get_RSP_WRITE_padding_1(u16p_)			cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, padding_1)
#define cm_get_RSP_WRITE_index(u16p_)				cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, index)
#define cm_get_RSP_WRITE_record_data_length(u32p_)	cm_get_u32  (u32p_,  dp, CM_BLK_READ_WRITE, record_data_length)
#define cm_get_RSP_WRITE_additional_value_1(u16p_)	cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, u.rsp_wr.additional_value_1)
#define cm_get_RSP_WRITE_additional_value_2(u16p_)	cm_get_u16  (u16p_,  dp, CM_BLK_READ_WRITE, u.rsp_wr.additional_value_2)
#define cm_get_RSP_WRITE_pnio_status(u32p_)			cm_get_u32  (u32p_,  dp, CM_BLK_READ_WRITE, u.rsp_wr.pnio_status)


/*----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
// cm_cl_prm - interface
//
//		cm_cl_prm_request ()
//		cm_cl_prm_abort ()
//		=>  cm_cl_prm_start_cnf
//		=>	cm_cl_prm_begin_ind
//		=>	cm_cl_prm_end_req
//		=>	cm_cl_prm_up_ind
//		=>	cm_cl_prm_idle_ind
//		=>	cm_cl_prm_down_ind
//
//	prm-read:
//		cm_cl_prm_read_start ()		// for CMITimeout-retrigger
//		cm_cl_prm_read_stop ()
//		=> cm_cl_prm_read_timeout ()
//
//	prm-ardy
//		cm_cl_prm_ardy_timeout_start ()		// for Remote_Application_Ready_Timeout // for Advanced-PreDataInd
//		cm_cl_prm_ardy_timeout_stop ()
//		=> cm_cl_prm_ardy_timeout ()
//----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_sub_state_must_prmed (
	LSA_UINT16  sub_state
	)
{
	CM_LOG_FCT ("cm_cl_sub_state_must_prmed")

	switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:					break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:			break;
	case CM_SUB_STATE_IDENT_WRONG:				return LSA_FALSE;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:		return LSA_FALSE;
	default: CM_FATAL ();						return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK) {
	case CM_SUB_STATE_AR_INFO_OWNED:				break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:	break; /* AP00221324 */
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED:return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:	return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:	return LSA_FALSE;
	default: CM_FATAL ();							return LSA_FALSE;
	}

	return LSA_TRUE;
}

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_sub_state_prmed_effect (
	LSA_UINT16  sub_state
	)
{
	CM_LOG_FCT ("cm_cl_sub_state_prmed_effect")

	switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:					break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:			break;
	case CM_SUB_STATE_IDENT_WRONG:				return LSA_FALSE;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:		return LSA_FALSE;
	default: CM_FATAL ();						return LSA_FALSE;
	}

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK) {
	case CM_SUB_STATE_AR_INFO_OWNED:				break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:	return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED:return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:	return LSA_FALSE;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:	return LSA_FALSE;
	default: CM_FATAL ();							return LSA_FALSE;
	}

	return LSA_TRUE;
}



/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_init (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_CL_AR_PRM_PTR_TYPE  prm = & argr->cm.cl.prm;

	prm->state				= CM_CL_PRM_STATE_DOWN;
	prm->prm_seq			= CM_CL_PRMMODE_IDLE;
	prm->prm_seq_next		= CM_CL_PRMMODE_IDLE;
	prm->prm_seq_next2		= CM_CL_PRMMODE_IDLE;

	CmListInitialize		(& prm->prm_list);
	CmListInitialize		(& prm->prm_submod_list);

	prm->prm_keep_plug		= LSA_NULL;
	prm->prm_keep_upd		= LSA_NULL;

	prm->submod.api			= 0;
	prm->submod.slot_nr		= 0;
	prm->submod.subslot_nr	= 0;

	prm->read_state			= 0;
	prm->read_timeout		= 0;
	cm_oneshot_init	(& prm->read_oneshot, cm_cl_prm_read_timeout, argr);

	prm->ardy_timer			= 0;
	prm->ardy_timeout_msec	= 0;
	cm_oneshot_init	(& prm->ardy_oneshot, cm_cl_prm_ardy_timeout, argr);
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_prm_mode (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	return argr->cm.cl.prm.prm_seq;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_abort (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	if (argr->cm.cl.prm.state != CM_CL_PRM_STATE_DOWN)
	{
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_WF_DOWN;

		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

			/* check prm-down.ind */
		cm_cl_prm_trigger (argr, 0);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_request (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prm_mode,
	CM_UPPER_RQB_PTR_TYPE  rqb
	)
{
	CM_LOG_FCT("cm_cl_prm_request")

	CM_ASSERT (cm_cl_prm_request_can_queue (argr, prm_mode));

	switch (prm_mode)
	{
	case CM_CL_PRMMODE_CONNECT:
		CM_ASSERT (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE);
		CM_ASSERT (are_equal (argr, rqb));
		argr->cm.cl.prm.prm_keep_upd = rqb;
		break;

	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_RETURN_OF_SUB:
		argr->cm.cl.prm.prm_keep_plug = rqb;
		break;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		argr->cm.cl.prm.prm_keep_upd = rqb;
		break;

	case CM_CL_PRMMODE_IDLE:
	default:
		CM_FATAL();
		return;
	}

	if (argr->cm.cl.prm.prm_seq_next == CM_CL_PRMMODE_IDLE)
	{
		argr->cm.cl.prm.prm_seq_next = (LSA_UINT8)prm_mode;
	}
	else if (argr->cm.cl.prm.prm_seq_next2 == CM_CL_PRMMODE_IDLE)
	{
		argr->cm.cl.prm.prm_seq_next2 = (LSA_UINT8)prm_mode;
	}
	else
	{
		CM_FATAL();
		return;
	}

	if (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE)
	{
		cm_cl_prm_request_next (argr);
	}
	else
	{
		CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PrmUpdate: dev(%u) ar(%u) request queued, prmmode(%u) next-prmmode(%u) next2-prmmode(%u) rqb(0x%08x), keep_upd(0x%x) keep_plug(0x%x)"
			, argr->device_nr, argr->ar_nr, prm_mode, argr->cm.cl.prm.prm_seq_next, argr->cm.cl.prm.prm_seq_next2, rqb, argr->cm.cl.prm.prm_keep_upd, argr->cm.cl.prm.prm_keep_plug
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_prm_request_can_queue (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  prm_mode
	)
{
	CM_LOG_FCT("cm_cl_prm_request_can_queue")

	switch (prm_mode)
	{
	case CM_CL_PRMMODE_CONNECT:
		CM_ASSERT (cm_cl_prm_mode (argr) == CM_CL_PRMMODE_IDLE);
		if (is_not_null (argr->cm.cl.prm.prm_keep_plug)) {
			return LSA_FALSE;
		}
		if (is_not_null (argr->cm.cl.prm.prm_keep_upd)) {
			return LSA_FALSE;
		}
		break;

	case CM_CL_PRMMODE_PLUG:
		if (is_not_null (argr->cm.cl.prm.prm_keep_plug)) {
			return LSA_FALSE;
		}
		break;

	case CM_CL_PRMMODE_RETURN_OF_SUB:
		if (is_not_null (argr->cm.cl.prm.prm_keep_plug)) {
			return LSA_FALSE;
		}
		break;

	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		if (is_not_null (argr->cm.cl.prm.prm_keep_upd)) {
			return LSA_FALSE;
		}
		break;

	default:
		CM_FATAL();
		return LSA_FALSE;
	}
	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_prm_request_next (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_cl_prm_request_next")
	LSA_UINT16 prev_prmmode = cm_cl_prm_mode(argr);

	argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_IDLE;

	switch (prev_prmmode)
	{
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_RETURN_OF_SUB:
#ifdef CM_MESSAGE
		/* TODO: here: synchronize with ar-alarm-event() eg remove .prm_alarm */
#endif /* CM_MESSAGE */
		CM_ASSERT (is_not_null (argr->cm.cl.prm.prm_keep_plug));
		argr->cm.cl.prm.prm_keep_plug = LSA_NULL;
		break;

	default:
		break;
	}

	if (argr->cm.cl.prm.prm_seq_next != CM_CL_PRMMODE_IDLE)
	{
		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: dev(%u) ar(%u): check NEXT, do next-prmmode(%u), next2-prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.prm_seq_next, argr->cm.cl.prm.prm_seq_next2
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);

			/* start next prm */
		cm_cl_prm_trigger (argr, CM_CL_DO_PRM_START);
		return LSA_TRUE;
	}

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PrmUpdate: dev(%u) ar(%u): check NEXT, IDLE, prev-prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, prev_prmmode, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR	/* return (ListEntry->field < NewEntry->field); */
cm_list_cmp__prm_record__api_slot_subslot_index_transfer (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
#define cm_index_is_ar_related(idx_)			(0xe000 <= (idx_) && (idx_) <= 0xebff)
#define cm_index_is_normative(idx_)				((idx_) >= 0x8000 && !(0xb000 <= (idx_) && (idx_) <= 0xbfff))
#define cm_transfer_sequence_is_defined(ts_)	((ts_) != 0)

	CM_AR_GRAPH_RECORD_PTR_TYPE  existing  = (CM_AR_GRAPH_RECORD_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_RECORD_PTR_TYPE  new_entry = (CM_AR_GRAPH_RECORD_PTR_TYPE) NewEntry;

		/* calc real record ptr */
	existing  = CM_CONTAINING_STRUCT (existing,  CM_AR_GRAPH_RECORD_PTR_TYPE, prm_link);
	new_entry = CM_CONTAINING_STRUCT (new_entry, CM_AR_GRAPH_RECORD_PTR_TYPE, prm_link);

	/* Sorting Order ::= /61158??0 Version 2.3 Ed. 2 from 2013-01 -- updated per eMail/2013-06-25/ and AP01553245/
	// a	- AR related data records defined by IEC 61158-6-10 (index is between 0xE000 and 0xEBFF) shall be transferred first in ascending order according to their index.
	// b	- Within the IO Device, all APIs are processed in ascending order.
	// c	- Within each API, all slots carrying a module are processed in ascending order.
	// d	- Within each module, all subslots carrying a submodule are processed in ascending order.
	// e	- Within each submodule,
	// f		- data records defined by IEC 61158-6-10 (index is between 0x8000 and 0xAFFF) will always be transferred first in ascending order according to their index. #AP01553245
	// g		- manufacturer and profile specific data records (index is between 0x0000 and 0x7FFF resp. between 0xB000 and 0xBFFF) will follow afterwards				#AP01553245
	// h			- according to their attribute @TransferSequence as given in the device抯 GSD,
	// i			- or if the attribute @TransferSequence is not stated, in ascending order according to their index.
	//
	//	Data records having an index between 0xC000 and 0xDFFF or between 0xECFF and 0xFFFF are always read-only according to IEC 61158-6-10 and IEC 61784-2.
	//	Therefore they do not appear within the transfer sequence.
	//	=> interpretation: read-only index are transferred like IEC data records, see f
	//
	//	@TransferSequence is not stated ::= (see GSDML Version 2.31 ?Date June 2013)
	//		"If this attribute is set to 0 (which is the default) then the transfer sequence is in ascending order according to their index."
	//
	// ASOM h:
	//				The transmission sequence is 
	//				- defined by the attribute DataRecords璗ransferSequence if present; derived from the GSD attribute ..\ParameterRecordDataItem\@Transfer resp. ..\F_ParameterRecordData璉tem\@TransferSequence,
	//				  The content of the attribute is an array of OMS_UINT32 stored in big endian format. Each array element specifies the index of a particular data record to be transmitted.
	//				  The number of elements is given implicitly by the length of the OMS_BLOB / 4.
	// ASOM i:
	//				- otherwise in ascending order according to their index.
	//
	*/

	if (! cm_index_is_ar_related(existing->record_index) && cm_index_is_ar_related(new_entry->record_index))
	{
		return LSA_FALSE; /*a, new first */
	}
	else if (cm_index_is_ar_related(existing->record_index) && ! cm_index_is_ar_related(new_entry->record_index))
	{
		return LSA_TRUE; /*a, existing first */
	}
	else if (cm_index_is_ar_related(existing->record_index) && cm_index_is_ar_related(new_entry->record_index))
	{
		return existing->record_index <= new_entry->record_index; /*a*/
	}
	else
	{
		if (existing->api == new_entry->api)
		{
			if (existing->slot_nr == new_entry->slot_nr)
			{
				if (existing->subslot_nr == new_entry->subslot_nr) /*e*/
				{
					if (! cm_index_is_normative(existing->record_index) && cm_index_is_normative(new_entry->record_index))
					{
						return LSA_FALSE; /*f, new first */
					}
					else if (cm_index_is_normative(existing->record_index) && ! cm_index_is_normative(new_entry->record_index))
					{
						return LSA_TRUE; /*f, existing first */
					}
					else if (cm_index_is_normative(existing->record_index) && cm_index_is_normative(new_entry->record_index))
					{
						return existing->record_index <= new_entry->record_index; /*f*/
					}
					else
					{
						if (! cm_transfer_sequence_is_defined(existing->transfer_sequence) && cm_transfer_sequence_is_defined(new_entry->transfer_sequence))
						{
							return LSA_FALSE; /*h, new first */
						}
						else if (cm_transfer_sequence_is_defined(existing->transfer_sequence) && ! cm_transfer_sequence_is_defined(new_entry->transfer_sequence))
						{
							return LSA_TRUE; /*h, existing first */
						}
						else if (cm_transfer_sequence_is_defined(existing->transfer_sequence) && cm_transfer_sequence_is_defined(new_entry->transfer_sequence))
						{
							if (existing->transfer_sequence == new_entry->transfer_sequence) /* illegal case, not checked earlier */
							{
								/* see GSDML: The TransferSequence attribute shall be unique within a submodule.
								//	The first transferred ParameterRecordData object shall have this attribute set to "1",
								//	the following objects shall have this attribute incremented in direct sequence (without gaps). */
								return existing->record_index <= new_entry->record_index; /* within transfer-sequence order by record-index */
							}
							else
							{
								return existing->transfer_sequence <= new_entry->transfer_sequence; /*h*/
							}
						}
						else
						{
							return existing->record_index <= new_entry->record_index; /*i*/
						}
					}
				}
				else
				{
					return existing->subslot_nr <= new_entry->subslot_nr; /*d*/
				}
			}
			else
			{
				return existing->slot_nr <= new_entry->slot_nr; /*c*/
			}
		}
		else
		{
			return existing->api <= new_entry->api; /*b*/
		}
	}
#undef cm_index_is_ar_related
#undef cm_index_is_normative
#undef cm_transfer_sequence_is_defined
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_record_list_build (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  prm_seq
	)
{
	CM_LOG_FCT ("cm_cl_prm_record_list_build")
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_ASSERT (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_IDLE);

	argr->cm.cl.prm.submod.api = 0;
	argr->cm.cl.prm.submod.slot_nr = 0;
	argr->cm.cl.prm.submod.subslot_nr = 0;

		/* collect prm-records to prm_list */

	CM_ASSERT (CmListIsEmpty (& argr->cm.cl.prm.prm_list));

	CmListInitialize (& argr->cm.cl.prm.prm_submod_list);

	switch (prm_seq)
	{
	case CM_CL_PRMMODE_CONNECT:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_CONNECT;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) ar(%u) prm-connect"
			, argr->device_nr, argr->ar_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

			/* first: all ar-related prm-records */
		{
			CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

			for (CmListForeach (rec, & argr->cm.cl.ar_prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
			{
				CmListInsertSorted (& argr->cm.cl.prm.prm_list, & rec->prm_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
			}
		}

			/* second: all submodule-related prm-records */
		for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					/* prm_mode connect means after IODConnectRes.ModuleDiffBlock, so sub_state is set correctly */
					if (cm_cl_sub_state_must_prmed (sub->ist.sub_state))
					{
						CmListInsertTail (& argr->cm.cl.prm.prm_submod_list, & sub->cl.submod_link);
					}
				}
			}
		}

		break;

	case CM_CL_PRMMODE_PLUG:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_PLUG;

		{
			CM_UPPER_ALARM_PTR_TYPE  alarm = argr->cm.cl.prm.prm_keep_plug->args.channel.alarm;

			argr->cm.cl.prm.submod.api = alarm->api;
			argr->cm.cl.prm.submod.slot_nr = alarm->slot_nr;
			argr->cm.cl.prm.submod.subslot_nr = alarm->subslot_nr;

			cm_ar_graph_find_api_slot_subslot (argr
				, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
				, LSA_NULL, & mod, & sub
				);
			if (is_null (sub)) { CM_FATAL(); }

			/* moved from cm_cl_alarm_indication() */
			{
				/* PLUG means: submodule does fit */
			mod->ist.mod_ident = alarm->mod_ident;
			sub->ist.sub_ident = alarm->sub_ident;
			}
		}

		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) ar(%u) prm-plug-alarm subslot(%u/%u/0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_SUB;

		sub->ist.sub_state = CM_SUB_STATE_GOOD;
		if (sub->ist.sub_ident != sub->soll.sub_ident) /* AP00219532 */
		{
			CM_SUB_STATE_SET_IDENT (sub->ist.sub_state, CM_SUB_STATE_IDENT_SUBSTITUTE);
		}

		CmListInsertTail (& argr->cm.cl.prm.prm_submod_list, & sub->cl.submod_link);
		break;

	case CM_CL_PRMMODE_RETURN_OF_SUB:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_RETURN_OF_SUB;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) ar(%u) prm-return-of-submodule"
			, argr->device_nr, argr->ar_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND); /* fake this .ind with com-up.ind */
		break;

	case CM_CL_PRMMODE_AR:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_AR;

		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) ar(%u) prm-update ar"
			, argr->device_nr, argr->ar_nr
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

			/* all submodule-related prm-records */
			/* ! sync with cm_cl_rpc_prm_begin() */
		for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					sub->ist.sub_state = CM_SUB_STATE_GOOD;

					CmListInsertTail (& argr->cm.cl.prm.prm_submod_list, & sub->cl.submod_link);
				}
			}
		}
		break;

	case CM_CL_PRMMODE_SUBMODULE:
		argr->cm.cl.prm.prm_seq = CM_CL_PRMMODE_SUBMODULE;

		{
			CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update = argr->cm.cl.prm.prm_keep_upd->args.cl.prm_update;

			argr->cm.cl.prm.submod.api = prm_update->prm_submod.api;
			argr->cm.cl.prm.submod.slot_nr = prm_update->prm_submod.slot_nr;
			argr->cm.cl.prm.submod.subslot_nr = prm_update->prm_submod.subslot_nr;

			cm_ar_graph_find_api_slot_subslot (argr
				, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
				, LSA_NULL, LSA_NULL, & sub
				);
			if (is_null (sub)) { CM_FATAL(); }

			cm_ar_graph_free_list_local_mem (& sub->cl.prm_record_list);
			CmListAppend (& sub->cl.prm_record_list, & prm_update->cm_internal_list);
		}

		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PrmUpdate: START: dev(%u) ar(%u) prm-update subslot(%u/%u/0x%x) submodule.state(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
			, sub->ist.sub_state
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);
		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND);

		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UNKNOWN_CONNECT;

		sub->ist.sub_state = CM_SUB_STATE_GOOD;

		CmListInsertTail (& argr->cm.cl.prm.prm_submod_list, & sub->cl.submod_link);
		break;

	default:
		CM_FATAL();
		return;
	}


	{
		CM_LIST_ENTRY_PTR_TYPE  inner_sub;
		CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

		for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
		{
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

			for (CmListForeach (rec, & sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
			{
				if (rec->tailor_data_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
				{
					CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "PrmUpdate: dev(%u) ar(%u) skip tailored record-index(0x%x) for subslot(%u/%u/0x%x)"
						, argr->device_nr, argr->ar_nr, rec->record_index
						, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
						);
				}
				else
				{
					CmListInsertSorted (& argr->cm.cl.prm.prm_list, & rec->prm_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
				}
			}
		}

#if 0/*trace record-order for transfer_seq*/
		{
		CM_AR_GRAPH_RECORD_PTR_TYPE i_rec, rec;
		for (CmListForeach (i_rec, & argr->cm.cl.prm.prm_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
		{
			rec = CM_CONTAINING_STRUCT (i_rec, CM_AR_GRAPH_RECORD_PTR_TYPE, prm_link);
			CM_CL_TRACE_05 (0, LSA_TRACE_LEVEL_CHAT
				, "OrderOfRecord: api(%u) slot(0x%04x) subslot(0x%04x) index(0x%08x) transfer_seq(0x%04x)"
				, rec->api, rec->slot_nr, rec->subslot_nr, rec->record_index, rec->transfer_sequence
				);
		}
		}
#endif
	}

		/* callback needed by prm-update.req */
	cm_cl_prm_start_cnf (argr);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_record_list_update (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_record_list_update")

	switch (cm_cl_prm_mode (argr))
	{
	case CM_CL_PRMMODE_AR:
	case CM_CL_PRMMODE_SUBMODULE:
		{
		LSA_BOOL again = LSA_TRUE;

		while (again)
		{
			CM_LIST_ENTRY_PTR_TYPE  inner_sub;

			again = LSA_FALSE;

			for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

				if (! cm_cl_sub_state_must_prmed (sub->ist.sub_state))
				{
					CmListRemoveEntry (inner_sub);
					again = LSA_TRUE;
					break; /*for*/
				}
			}
		}
		}


		CmListInitialize (& argr->cm.cl.prm.prm_list);
		{
			CM_LIST_ENTRY_PTR_TYPE  inner_sub;
			CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

			for (CmListForeach (inner_sub, & argr->cm.cl.prm.prm_submod_list, CM_LIST_ENTRY_PTR_TYPE))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub = CM_CONTAINING_STRUCT (inner_sub, CM_AR_GRAPH_SUBMODULE_PTR_TYPE, cl.submod_link);

				for (CmListForeach (rec, & sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
				{
					/*AP01312556*/
					CmListInsertSorted (& argr->cm.cl.prm.prm_list, & rec->prm_link, cm_list_cmp__prm_record__api_slot_subslot_index_transfer);
				}
			}
		}
		return;

	case CM_CL_PRMMODE_CONNECT:
	case CM_CL_PRMMODE_PLUG:
	case CM_CL_PRMMODE_RETURN_OF_SUB:
	default:
		CM_FATAL();
		return;
	}
}


/*------------------------------------------------------------------------------
// the new prm-statemachine
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_trigger (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT32  done
	)
{
	CM_LOG_FCT("cm_cl_prm_trigger")

	CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PrmUpdate: dev(%u) ar(%u) prm-trigger(0x%x) prmmode(%u) arState(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr, done, cm_cl_prm_mode (argr), argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

		/* do done */
	switch (done)
	{
	case 0: /* trigger only */
		break;

	case CM_CL_DO_PRM_START: /* start */
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_PRM;
		if (! CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START)) { /* prm-plug & prm-upd started at the same time */
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);
		}
		break;

	case CM_CL_WF_PRM_BEGIN_RSP:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_BEGIN_RSP);
		break;

	case CM_CL_WF_RPC_PRM_WRITE_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF);
		break;

	case CM_CL_WF_PRM_APPL_RDY_IND:
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND); /* see prm-abort */
		break;

	case CM_CL_WF_RPC_RECORD_CNF:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);
		break;

	case CM_CL_WF_PRM_INDATA_RSP:
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP);
		break;

	default:
		CM_FATAL1(done);
		break;
	}

		/* common wait-for */
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_BEGIN_RSP))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf prm-begin.rsp", argr->device_nr, argr->ar_nr);
		return;
	}
	/* not common:  CM_CL_WF_PRM_INDATA_RSP => see cm_cl_ar_check_abort() */

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc prm-begin.cnf", argr->device_nr, argr->ar_nr);
		return;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc prm-end.cnf", argr->device_nr, argr->ar_nr);
		return;
	}
	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc prm-write.cnf", argr->device_nr, argr->ar_nr);
		return;
	}

		/* do next */
	switch (argr->cm.cl.prm.state)
	{
	case CM_CL_PRM_STATE_DOWN:
			/* nothing to do */
		break;

	case CM_CL_PRM_STATE_PRM:

			/* do start */
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_PRM_START))
		{
			if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc user-read/write.cnf", argr->device_nr, argr->ar_nr);
				return;
			}

			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_PRM_START);

			CM_ASSERT (argr->cm.cl.prm.prm_seq == CM_CL_PRMMODE_IDLE);

			switch (argr->cm.cl.prm.prm_seq_next)
			{
			case CM_CL_PRMMODE_CONNECT:	
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_CONNECT);
				break;

			case CM_CL_PRMMODE_AR:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_AR);
				break;

			case CM_CL_PRMMODE_SUBMODULE:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_SUBMODULE);
				break;

			case CM_CL_PRMMODE_PLUG:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_PLUG);
				break;

			case CM_CL_PRMMODE_RETURN_OF_SUB:
				cm_cl_prm_record_list_build (argr, CM_CL_PRMMODE_RETURN_OF_SUB);
				break;

			default:
				CM_FATAL();
				return;
			}

			argr->cm.cl.prm.prm_seq_next  = argr->cm.cl.prm.prm_seq_next2;
			argr->cm.cl.prm.prm_seq_next2 = CM_CL_PRMMODE_IDLE;

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE); /* wait flag for arsm() */

			argr->cm.cl.prm.state = CM_CL_PRM_STATE_PRM;

			cm_cl_prm_begin_ind (argr);
			cm_cl_prm_trigger (argr, 0);
			/* wait for prm-begin.rsp */
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN))
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_BEGIN);

			cm_cl_rpc_prm_begin (argr);
			return;
		}

			/* writing the next prm_record from dev->prm_list */
		if (! CmListIsEmpty (& argr->cm.cl.prm.prm_list))
		{
			LSA_UINT16  rc = cm_cl_prm_record_write (argr->ar_com.channel, argr);
			if (rc != CM_OK)
			{
				cm_cl_prm_abort (argr);
				return;
			}
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END))
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_DO_RPC_PRM_END);

			cm_cl_prm_end_req (argr);
			cm_cl_rpc_prm_end (argr);
			return;
		}

		argr->cm.cl.prm.state = CM_CL_PRM_STATE_UP;
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE);
		cm_cl_prm_up_ind (argr);

		cm_cl_prm_trigger (argr, 0);
		break;

	case CM_CL_PRM_STATE_UP:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_APPL_RDY_IND))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc prm-appl-rdy.ind", argr->device_nr, argr->ar_nr);

				/* start next record-read/write */
			cm_cl_record_read_write (argr->ar_com.channel, argr);
			return;
		}

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf prm-indata.rsp", argr->device_nr, argr->ar_nr);

				/* start next record-read/write */
			cm_cl_record_read_write (argr->ar_com.channel, argr);
			return;
		}

		if (cm_cl_prm_request_next (argr))
		{	/* next prm started */
			return;
		}

		cm_cl_prm_idle_ind (argr);

			/* start next record-read/write */
		cm_cl_record_read_write (argr->ar_com.channel, argr);
		break;

	case CM_CL_PRM_STATE_WF_DOWN:

		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_INDATA_RSP))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf prm-indata.rsp", argr->device_nr, argr->ar_nr);
			return;
		}
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
		{
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "PrmUpdate: dev(%u) ar(%u) wf rpc user-read/write.cnf", argr->device_nr, argr->ar_nr);
			return;
		}

		/* no more wf-flag */
		argr->cm.cl.prm.state = CM_CL_PRM_STATE_DOWN;
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_PRM_DONE); /* up -> prm_abort() -> down.ind */
		if (is_not_null (argr->cm.cl.prm.prm_keep_upd))
		{
			cm_callback (argr->ar_com.channel, CM_OK_CANCELLED, & argr->cm.cl.prm.prm_keep_upd);
		}
		cm_cl_prm_down_ind (argr);
		break;

	default:
		CM_FATAL1(argr->cm.cl.prm.state);
		return;
	}
}


/*------------------------------------------------------------------------------
// internal opc record_read
//----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_read (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_read")
	CM_UPPER_CL_RECORD_READ_PTR_TYPE  read;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_INT  dp_left;

	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = upper->args.cl.read;

	CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) slot(%u) subslot(0x%x) index(0x%x) sequence(%u) length(%u) query_length(%u)"
		, argr->device_nr, argr->ar_nr
		, read->rec.slot_nr, read->rec.subslot_nr, read->rec.record_index
		, read->rec.sequence_nr, read->rec.data_length, read->rec.readquery_length
		);

	CM_ASSERT (CM_RECORD_OFFSET + read->rec.readquery_length <= read->rec.data_length);

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc)) {
		return CM_ERR_ALLOC_LOWER;
	}
	CM_ASSERT (is_not_null (dev->rpc_client_id));

		/* increment sequence_nr */
	argr->cm.cl.record_sequence_nr_rollback = argr->cm.cl.record_sequence_nr;
	read->rec.sequence_nr = argr->cm.cl.record_sequence_nr;
	CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_READ;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->alloc_len		= read->rec.data_length;
	rpc->args.cl.call->ndr_len			= CM_RECORD_OFFSET + read->rec.readquery_length;
	rpc->args.cl.call->ndr_data			= read->rec.data; /* zero copy */

		/* marshal args to PNIO-net-format */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;

	dp_left = CM_RECORD_OFFSET - CLRPC_PNIO_OFFSET;

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_READ_REQ);
	cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_BLK_READ_WRITE)));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_put_REQ_READ_seq_number (read->rec.sequence_nr);
	cm_put_REQ_READ_ar_uuid (& argr->ar.ar_uuid);
	cm_put_REQ_READ_api (read->rec.api);
	cm_put_REQ_READ_slot_nr (read->rec.slot_nr);
	cm_put_REQ_READ_subslot_nr (read->rec.subslot_nr);
	cm_put_REQ_READ_padding_1;
	cm_put_REQ_READ_index (read->rec.record_index);
	cm_put_REQ_READ_record_data_length (read->rec.data_length - CM_RECORD_OFFSET);

	cm_put_REQ_READ_padding_2;

#ifdef CM_DEBUG
	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
	CM_ASSERT (dp_left == 0);
	LSA_UNUSED_ARG (dp); /* lint */
#endif

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pnio_status_abort_ar (
	LSA_UINT32  pnio_status
	)
{
	LSA_UINT8  error_decode;

	if (pnio_status == CM_PNIO_ERR_NONE)
	{
		return LSA_FALSE;
	}

	/* 2010-07-26: An applicative error code is not a protocol error and therefore no AR-abortion. */
	error_decode = CM_PNIO_ERR_FETCH_DECODE(pnio_status);
	if (error_decode == CM_PNIO_ERR_DECODE_PNIORW)
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_read_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_read_done")
	CM_UPPER_CL_RECORD_READ_PTR_TYPE  read;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT16  block_type, block_length, block_version;
	LSA_INT  dp_left;
	CLRPC_UUID_TYPE  ar_uuid;
	LSA_UINT16  slot_nr, subslot_nr, index, seq_number;
	LSA_UINT32  api, record_data_length;
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	LSA_UINT16  padding_1;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UNUSED_ARG (channel);

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY
		);

	read = upper->args.cl.read;

	CM_ASSERT (are_equal (read->rec.data, rpc->args.cl.call->ndr_data)); /* zero copy */

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_READ
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	read->rec.cm_pnio_err = pnio_status;

	if (cm_pnio_status_abort_ar (pnio_status))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): cm_cl_rpc_reacord_read_done, ndr_len(%d) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr, argr->ar_nr
			, rpc->args.cl.call->ndr_len, lower_rsp, pnio_status, nca_status
			);
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_READ, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}
	else
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): cm_cl_rpc_record_read_done, ndr_len(%d) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr, argr->ar_nr
			, rpc->args.cl.call->ndr_len, lower_rsp, pnio_status, nca_status
			);
	}

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: refactor with pattern cm_get_assert(dp_left) / cm_get_xxx() / cm_get_adjust(dp_left)")
#endif /* CM_MESSAGE */

		/* enough data ? */
	if (rpc->args.cl.call->ndr_len < CM_RECORD_OFFSET)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ndr_len(%d) < CM_RECORD_OFFSET", rpc->args.cl.call->ndr_len
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_READ));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	cm_get_BLOCK_HEADER_type (& block_type);
	cm_get_BLOCK_HEADER_length (& block_length);
	cm_get_BLOCK_HEADER_version (& block_version);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "BLOCK:type(0x%04x) len(%d) version(0x%04x)"
		, block_type, block_length, block_version
		);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_get_RSP_READ_seq_number (& seq_number);
	cm_get_RSP_READ_ar_uuid (& ar_uuid);
	cm_get_RSP_READ_api (& api);
	cm_get_RSP_READ_slot_nr (& slot_nr);
	cm_get_RSP_READ_subslot_nr (& subslot_nr);
	cm_get_RSP_READ_padding_1 (& padding_1);
	cm_get_RSP_READ_index (& index);
	cm_get_RSP_READ_record_data_length (& record_data_length);

	cm_get_RSP_READ_additional_value_1 (& read->rec.ret_val_1);
	cm_get_RSP_READ_additional_value_2 (& read->rec.ret_val_2);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
	LSA_UNUSED_ARG (dp); /* lint */

	if (dp_left < 0
	||	block_type != CM_BLOCK_TYPE_READ_RSP
	||  block_version != 0x0100
	||	read->rec.api != api
	||	read->rec.slot_nr != slot_nr || read->rec.subslot_nr != subslot_nr
	||	read->rec.record_index != index
	||	read->rec.sequence_nr != seq_number
	||	record_data_length + CM_RECORD_OFFSET < CM_RECORD_OFFSET /* wrap around check */
	||	record_data_length + CM_RECORD_OFFSET > rpc->args.cl.call->ndr_len
	||	padding_1 != 0
	)
	{
		CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "parse error: dev(%u) ar(%u) slot(%d) subslot(%d) index(%d) sequence(%d) length(%d) rv1(%d) rv2(%d) ndr_len(%d) padding_1(%d)"
			, argr->device_nr, argr->ar_nr, slot_nr, subslot_nr, index
			, seq_number, record_data_length, read->rec.ret_val_1, read->rec.ret_val_2
			, rpc->args.cl.call->ndr_len
			, padding_1
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_READ));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

		/* bad pnio-response */
	if (read->rec.cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "pnio_status(0x%x) rsp.len(%d)",
				read->rec.cm_pnio_err, record_data_length
			);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("CHECK: wirklich + record_data_length?")
#endif /* CM_MESSAGE */
		read->rec.data_length = CM_RECORD_OFFSET + record_data_length;

		return CM_OK; /* CM_OK, this is an application error */
	}

	read->rec.data_length = CM_RECORD_OFFSET + record_data_length;

	return CM_OK;
}


/*------------------------------------------------------------------------------
// internal opc record write
//----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_write")
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE  write;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_INT  dp_left;

	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)

	write = upper->args.cl.write;

	CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) slot(%d) subslot(%d) index(%d) sequence(%d) length(%d)"
		, argr->device_nr, argr->ar_nr
		, write->rec.slot_nr, write->rec.subslot_nr, write->rec.record_index
		, write->rec.sequence_nr, write->rec.data_length
		);

	if (write->rec.data_length < CM_RECORD_OFFSET) {
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "rec.data_len(%d) < CM_REC_OFFSET(%d)",
				write->rec.data_length, CM_RECORD_OFFSET
			);
		return CM_ERR_PARAM;
	}

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc)) {
		return CM_ERR_ALLOC_LOWER;
	}

		/* increment sequence_nr */
	argr->cm.cl.record_sequence_nr_rollback = argr->cm.cl.record_sequence_nr;
	write->rec.sequence_nr = argr->cm.cl.record_sequence_nr;
	CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_WRITE;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->alloc_len		= write->rec.data_length;
	rpc->args.cl.call->ndr_len			= write->rec.data_length;
	rpc->args.cl.call->ndr_data			= write->rec.data; /* zero copy */

		/* marshal args to PNIO-net-format */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = CM_RECORD_OFFSET;

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_WRITE_REQ);
	cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_BLK_READ_WRITE)));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_put_REQ_WRITE_seq_number (write->rec.sequence_nr);
	cm_put_REQ_WRITE_ar_uuid (& argr->ar.ar_uuid);
	cm_put_REQ_WRITE_api (write->rec.api);
	cm_put_REQ_WRITE_slot_nr (write->rec.slot_nr);
	cm_put_REQ_WRITE_subslot_nr (write->rec.subslot_nr);
	cm_put_REQ_WRITE_padding_1;
	cm_put_REQ_WRITE_index (write->rec.record_index);
	cm_put_REQ_WRITE_record_data_length (write->rec.data_length - CM_RECORD_OFFSET);
	cm_put_REQ_WRITE_padding_2;

#ifdef CM_DEBUG
	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
#endif
	LSA_UNUSED_ARG (dp); /* lint */
	LSA_UNUSED_ARG (dp_left); /* lint */

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_write_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_write_done")
	CM_UPPER_CL_RECORD_WRITE_PTR_TYPE  write;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT16  block_type, block_length, block_version;
	LSA_INT  dp_left;
	CLRPC_UUID_TYPE  ar_uuid;
	LSA_UINT16  slot_nr, subslot_nr, index, seq_number;
	LSA_UINT32  api, record_data_length;
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	LSA_UINT16  padding_1;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UNUSED_ARG (channel);

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE);

	write = upper->args.cl.write;

	CM_ASSERT (are_equal (write->rec.data, rpc->args.cl.call->ndr_data)); /* zero copy */

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_WRITE
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
		, argr->device_nr, argr->ar_nr
		, lower_rsp, pnio_status, nca_status
		);

	write->rec.cm_pnio_err = pnio_status;

	if (cm_pnio_status_abort_ar (pnio_status))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): cm_cl_rpc_record_write_done, ndr_len(%d) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr, argr->ar_nr
			, rpc->args.cl.call->ndr_len, lower_rsp, pnio_status, nca_status
			);
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_WRITE, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

		/* enough data ? */
	if (rpc->args.cl.call->ndr_len < CM_RECORD_OFFSET)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ndr_len(%d) < CM_RECORD_OFFSET", rpc->args.cl.call->ndr_len);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	cm_get_BLOCK_HEADER_type (& block_type);
	cm_get_BLOCK_HEADER_length (& block_length);
	cm_get_BLOCK_HEADER_version (& block_version);

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "BLOCK:type(0x%04x) len(%d) version(0x%04x)"
		, block_type, block_length, block_version
		);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_get_RSP_WRITE_seq_number (& seq_number);
	cm_get_RSP_WRITE_ar_uuid (& ar_uuid);
	cm_get_RSP_WRITE_api (& api);
	cm_get_RSP_WRITE_slot_nr (& slot_nr);
	cm_get_RSP_WRITE_subslot_nr (& subslot_nr);
	cm_get_RSP_WRITE_padding_1 (& padding_1);
	cm_get_RSP_WRITE_index (& index);
	cm_get_RSP_WRITE_record_data_length (& record_data_length);

	cm_get_RSP_WRITE_additional_value_1 (& write->rec.ret_val_1);
	cm_get_RSP_WRITE_additional_value_2 (& write->rec.ret_val_2);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));
	LSA_UNUSED_ARG (dp); /* lint */

		/* check */
	if (dp_left < 0
	||	block_type != CM_BLOCK_TYPE_WRITE_RSP
	||	block_version != 0x0100
	||	write->rec.api != api
	||	write->rec.slot_nr != slot_nr || write->rec.subslot_nr != subslot_nr
	||	write->rec.record_index != index
	||	write->rec.sequence_nr != seq_number
	||	record_data_length + CM_RECORD_OFFSET < CM_RECORD_OFFSET /* wrap around */
	||	record_data_length + CM_RECORD_OFFSET > write->rec.data_length
	||	padding_1 != 0
	)
	{
		CM_CL_TRACE_12 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "parse error: dev(%u) ar(%u) api(%d) slot(%d) subslot(%d) index(%d) sequence(%d) length(%d) rv1(%d) rv2(%d) rec.len(%d) padding_1(%d)"
			, argr->device_nr, argr->ar_nr
			, api, slot_nr, subslot_nr, index
			, seq_number, record_data_length, write->rec.ret_val_1, write->rec.ret_val_2
			, write->rec.data_length, padding_1
			);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

		/* bad pnio-response */
	if (write->rec.cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "len(%d)  rsp.len(%d)  pnio_status(0x%x)",
				write->rec.data_length, record_data_length, write->rec.cm_pnio_err
			);
		write->rec.data_length = CM_RECORD_OFFSET + record_data_length;

		return CM_OK; /* CM_OK, this is an application error ! */
	}

		/* cnf written data-len */
	write->rec.data_length = CM_RECORD_OFFSET + record_data_length;

	return CM_OK;
}


/*------------------------------------------------------------------------------
// internal opc write multiple
//----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_write_multiple (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_write_multiple")
	CM_UPPER_CL_RECORD_WRITE_MULTIPLE_PTR_TYPE  wrm;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT  dp_left;
	LSA_UINT16  list_cnt;
	CM_UPPER_CM_RECORD_PTR_TYPE  rec;
	CM_COMMON_MEM_U8_PTR_TYPE  ndr_data;
	LSA_UINT32  padding, alloc_len;
	LSA_UINT16  cm_put_assert_line = 0;

	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE_MULTIPLE)
	CM_ASSERT (is_not_null (dev->rpc_client_id));

	wrm = upper->args.cl.write_multiple;

	CmListLength (& wrm->rec_list, & list_cnt);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) data_length(%u) data(0x%x)  rec_list_cnt(%d)"
		, argr->device_nr, argr->ar_nr, wrm->data_length, wrm->data, list_cnt
		);

	if (wrm->data_length == 0) {
		CM_ASSERT (is_null (wrm->data));

			/* calculate needed alloc-len */
		alloc_len = CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE;
		for (CmListForeach (rec, & wrm->rec_list, CM_UPPER_CM_RECORD_PTR_TYPE)) {

			padding = alloc_len % 4;
			padding = (4 - padding) % 4;

			alloc_len += padding + CM_RECORD_HEADER_SIZE + rec->data_length;
		}

		if (alloc_len > 0xFFFF) {
			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "too much records or too much data, list_cnt(%u) alloc_len(%u)"
				, list_cnt, alloc_len
				);
			return CM_ERR_ALLOC_LOWER;
		}

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& ndr_data), cm_null_user_id, CM_CAST_U16(alloc_len), channel->sysptr);
		if (is_null (ndr_data)) {
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "no mem for ndr_data, alloc_len(%u)", alloc_len
				);
			return CM_ERR_ALLOC_LOWER;
		}
		wrm->data = ndr_data; /* keep ndr_data for memory-free */
	}
	else {
		CM_ASSERT (is_not_null (wrm->data));

CM_FATAL(); /* not implemented, must implement with CM_OPC_RECORD_WRITE_MULTIPLE */
		return CM_ERR_PARAM;
	}

		/* needed by alignment, see [Padding]* within IODWriteMultipleReq  */
	if ((((LSA_UINT32)ndr_data) & 0x03) != 0) {
		CM_FATAL1 ((LSA_UINT32)ndr_data);
		return CM_ERR_RESOURCE;
	}

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc)) {
		return CM_ERR_ALLOC_LOWER;
	}

/*
IODWriteMultipleReq ::=
	IODWriteHeader<index=0xE040>,
		(IODWriteHeader, RecordDataWrite, [Padding]*)*

	The number of padding octets (value=0) shall be 0,1,2,3
	to have 32 bit alignment to the next IODWriteHeader.
*/

		/* save sequence_nr */
	argr->cm.cl.record_sequence_nr_rollback = argr->cm.cl.record_sequence_nr;

		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_WRITE;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->alloc_len		= alloc_len;
	rpc->args.cl.call->ndr_len			= alloc_len;
	rpc->args.cl.call->ndr_data			= ndr_data; /* zero copy */

		/* marshal args to PNIO-net-format */
	dp = ndr_data + CLRPC_PNIO_OFFSET;
	dp_left = alloc_len - CLRPC_PNIO_OFFSET;

	cm_put_assert (dp_left >= sizeof (CM_BLK_HEADER));

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_WRITE_REQ);
	cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_BLK_READ_WRITE)));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_put_assert (dp_left >= sizeof (CM_BLK_READ_WRITE));

	cm_put_REQ_WRITE_seq_number (argr->cm.cl.record_sequence_nr);
	cm_put_REQ_WRITE_ar_uuid (& argr->ar.ar_uuid);
	cm_put_REQ_WRITE_api (0xFFFFFFFF); /* don't care */
	cm_put_REQ_WRITE_slot_nr (0xFFFF); /* don't care */
	cm_put_REQ_WRITE_subslot_nr (0xFFFF); /* don't care */
	cm_put_REQ_WRITE_padding_1;
	cm_put_REQ_WRITE_index (0xE040);
	cm_put_REQ_WRITE_record_data_length (alloc_len - CM_RECORD_OFFSET);
	cm_put_REQ_WRITE_padding_2;

	CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));

	padding = 0;
	for (CmListForeach (rec, & wrm->rec_list, CM_UPPER_CM_RECORD_PTR_TYPE)) {

			/* calc padding for current record */
		padding = (dp - ndr_data) % 4;
		padding = (padding == 0) ? 0 : 4 - padding;

		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "offs(%u) padding(%u) api(%u) slot(%u) subslot(%u) index(0x%x) data_len(%u)"
			, (dp - ndr_data), padding
			, rec->api, rec->slot_nr, rec->subslot_nr, rec->record_index, rec->data_length
			);

			/* padding for the next record */
		while (padding > 0) {
			cm_put_assert (dp_left >= 1);
			CM_PUT8_HTON (dp, 0, 0);
			cm_adjust (dp, dp_left, 1);

			padding -= 1;
		}

			/* calc padding for current record
		padding = rec->data_length % 4;
		padding = (padding == 0) ? 0 : 4 - padding;*/

			/* marshal embedded write-header */
		rec->sequence_nr = argr->cm.cl.record_sequence_nr;
		CM_COUNTER_INCREMENT (argr->cm.cl.record_sequence_nr);

		cm_put_assert (dp_left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_WRITE_REQ);
		cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_BLK_READ_WRITE)));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

		cm_put_assert (dp_left >= sizeof (CM_BLK_READ_WRITE));

		cm_put_REQ_WRITE_seq_number (rec->sequence_nr);
		cm_put_REQ_WRITE_ar_uuid (& argr->ar.ar_uuid);
		cm_put_REQ_WRITE_api (rec->api);
		cm_put_REQ_WRITE_slot_nr (rec->slot_nr);
		cm_put_REQ_WRITE_subslot_nr (rec->subslot_nr);
		cm_put_REQ_WRITE_padding_1;
		cm_put_REQ_WRITE_index (rec->record_index);
		cm_put_REQ_WRITE_record_data_length (rec->data_length);
		cm_put_REQ_WRITE_padding_2;

		cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));

			/* marshal embedded write-data */
		cm_put_assert (dp_left >= rec->data_length);

		CM_MEMCPY (dp, rec->data, rec->data_length);

		cm_adjust (dp, dp_left, rec->data_length);
	}


	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return CM_OK;

cm_put_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL
		, "cm_put_assert_failed in line(%u), programming error"
		, cm_put_assert_line
		);
	LSA_UNUSED_ARG (cm_put_assert_line);
	CM_FATAL();
	return CM_ERR_PARAM;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_record_write_multi_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_rpc_record_write_multi_done")
	CM_UPPER_CL_RECORD_WRITE_MULTIPLE_PTR_TYPE  wrm;
	CM_UPPER_CM_RECORD_PTR_TYPE  rec;
	LSA_UINT16  rec_cnt;
	CM_RPC_MEM_U8_PTR_TYPE  dp_start, dp;
	LSA_UINT32  dp_left;
	LSA_UINT16  block_type, block_length, block_version;
	CLRPC_UUID_TYPE  ar_uuid;
	LSA_UINT16  slot_nr, subslot_nr, index, seq_number, padding_1, ret_val_1, ret_val_2;
	LSA_UINT32  api, record_data_length;
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	LSA_UINT32  pnio_status_2;
	LSA_UINT16  cm_get_assert_line = 0;
	LSA_UINT16  cm_get_assert_error = 0;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UNUSED_ARG(channel);

	CM_ASSERT (is_not_null (upper));
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE_MULTIPLE);

	wrm = upper->args.cl.write_multiple;

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_WRITE
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) rsp(%d) pnio_status(0x%x) nca_status(0x%x)"
		, argr->device_nr, argr->ar_nr
		, lower_rsp, pnio_status, nca_status
		);

		/* cnf: pnio_status of header */
	wrm->cm_pnio_err = pnio_status;

	if (cm_pnio_status_abort_ar (wrm->cm_pnio_err))
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u) lower_rsp(%u) != OK, ndr_len(%u) pnio_status(0x%x) nca_status(0x%x)"
			, argr->device_nr, argr->ar_nr
			, lower_rsp, rpc->args.cl.call->ndr_len, wrm->cm_pnio_err, nca_status
			);
		CM_CL_DINFO_SET2 (&dinfo, CM_AR_REASON_WRITE, error_cause, wrm->cm_pnio_err, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

		/* enough data: CLRPC and E040-header */
	if (rpc->args.cl.call->ndr_len < CM_RECORD_OFFSET)
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): ndr_len(%u) < CM_RECORD_OFFSET(%u), pnio_status(0x%x)"
			, argr->device_nr, argr->ar_nr
			, rpc->args.cl.call->ndr_len, CM_RECORD_OFFSET, wrm->cm_pnio_err
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

	dp_start = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	dp = dp_start;
	dp_left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

		/* parse E040-write-header */
	cm_get_assert (dp_left >= sizeof (CM_BLK_HEADER));

	cm_get_BLOCK_HEADER_type (& block_type);
	cm_get_BLOCK_HEADER_length (& block_length);
	cm_get_BLOCK_HEADER_version (& block_version);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

	cm_get_assert (dp_left >= sizeof (CM_BLK_READ_WRITE));

	cm_get_RSP_WRITE_seq_number (& seq_number);
	cm_get_RSP_WRITE_ar_uuid (& ar_uuid);
	cm_get_RSP_WRITE_api (& api);
	cm_get_RSP_WRITE_slot_nr (& slot_nr);
	cm_get_RSP_WRITE_subslot_nr (& subslot_nr);
	cm_get_RSP_WRITE_padding_1 (& padding_1);
	cm_get_RSP_WRITE_index (& index);
	cm_get_RSP_WRITE_record_data_length (& record_data_length);
	cm_get_RSP_WRITE_additional_value_1 (& ret_val_1);
	cm_get_RSP_WRITE_additional_value_2 (& ret_val_2);
	cm_get_RSP_WRITE_pnio_status (& pnio_status_2);

	cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));

		/* check E040-header */
	if (! cm_uuid_equal (& ar_uuid, & argr->ar.ar_uuid)
	|| block_type != CM_BLOCK_TYPE_WRITE_RSP || block_version != 0x0100
	|| index != 0xE040
	|| seq_number != argr->cm.cl.record_sequence_nr_rollback
	|| record_data_length + CM_RECORD_OFFSET < CM_RECORD_OFFSET /* wrap around */
	|| padding_1 != 0
/*	|| pnio_status != pnio_status_2 	// may differ and this is Ok, see cm_cl_rpc_to_addr_info() */
	)
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "parse error: dev(%u) ar(%u) index(0x%x) seq_number(%u/%u) rec_data_len(%u)"
			, argr->device_nr, argr->ar_nr
			, index
			, seq_number, argr->cm.cl.record_sequence_nr_rollback
			, record_data_length
			);
		argr->cm.cl.record_sequence_nr = argr->cm.cl.record_sequence_nr_rollback;
		CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) not found"
			, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
			, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
			, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
			, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

		/* cnf: written data-len */
	wrm->data_length = CM_RECORD_OFFSET + record_data_length;
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("@check: docu says: count of written bytes, PNO says 0 within response?")
#endif /* CM_MESSAGE */

		/* bad pnio-response */
	if (wrm->cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		for (CmListForeach (rec, & wrm->rec_list, CM_UPPER_CM_RECORD_PTR_TYPE))
		{
			rec->cm_pnio_err = wrm->cm_pnio_err;
		}
		if (cm_pnio_status_abort_ar (wrm->cm_pnio_err))
		{
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): faulty_block, pnio_status(0x%x)"
				, argr->device_nr, argr->ar_nr, pnio_status
				);
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
			cm_cl_arsm_down (argr, &dinfo);
			return CM_ERR_LOWER_LAYER;
		}
		return CM_OK;	/* CM_OK, this is an application error ! */
	}

		/* parsing E040-body */
	CmListLength (& wrm->rec_list, & rec_cnt);

		/* enough data: CLRPC + E040-header + rec_cnt embedded header */
	if (rpc->args.cl.call->ndr_len < 0UL + CM_RECORD_OFFSET + rec_cnt * CM_RECORD_HEADER_SIZE)
	{
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): ndr_len(%u) < CM_RECORD_OFFSET(%u) + rec_cnt(%u) * CM_RECORD_HEADER_SIZE(%u)"
			, argr->device_nr, argr->ar_nr
			, rpc->args.cl.call->ndr_len, CM_RECORD_OFFSET, rec_cnt, CM_RECORD_HEADER_SIZE
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}

	rec = CmListFirst (& wrm->rec_list, CM_UPPER_CM_RECORD_PTR_TYPE);

	while (is_not_null (rec)  &&  dp_left > 0) {

		cm_get_assert (dp_left >= sizeof (CM_BLK_HEADER));

		cm_get_BLOCK_HEADER_type (& block_type);
		cm_get_BLOCK_HEADER_length (& block_length);
		cm_get_BLOCK_HEADER_version (& block_version);

		cm_adjust (dp, dp_left, sizeof (CM_BLK_HEADER));

		if (block_type != CM_BLOCK_TYPE_WRITE_RSP
		||	block_length != sizeof(block_version) + sizeof (CM_BLK_READ_WRITE)
		||	block_version != 0x0100)
		{
			CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): block_type(0x%x) block_length(%u) block_version(0x%x)"
				, argr->device_nr, argr->ar_nr
				, block_type, block_length, block_version
				);
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
			cm_cl_arsm_down (argr, &dinfo);
			return CM_ERR_LOWER_LAYER;
		}

		cm_get_assert (dp_left >= sizeof (CM_BLK_READ_WRITE));

		cm_get_RSP_WRITE_seq_number (& seq_number);
		cm_get_RSP_WRITE_ar_uuid (& ar_uuid);
		cm_get_RSP_WRITE_api (& api);
		cm_get_RSP_WRITE_slot_nr (& slot_nr);
		cm_get_RSP_WRITE_subslot_nr (& subslot_nr);
		cm_get_RSP_WRITE_padding_1 (& padding_1);
		cm_get_RSP_WRITE_index (& index);
		cm_get_RSP_WRITE_record_data_length (& record_data_length);
		cm_get_RSP_WRITE_additional_value_1 (& ret_val_1);
		cm_get_RSP_WRITE_additional_value_2 (& ret_val_2);
		cm_get_RSP_WRITE_pnio_status (& pnio_status_2);

		cm_adjust (dp, dp_left, sizeof (CM_BLK_READ_WRITE));

			/* check dev-specific */
		if (! cm_uuid_equal (& argr->ar.ar_uuid, & ar_uuid))
		{
			CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) not found"
				, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
				, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
				, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
				, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
				);

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): unexpeced uuid(%08x)"
				, argr->device_nr, argr->ar_nr, & ar_uuid
				);
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
			cm_cl_arsm_down (argr, &dinfo);
			return CM_ERR_LOWER_LAYER;
		}

		if (rec->sequence_nr != seq_number
		||	rec->api != api
		||	rec->slot_nr != slot_nr
		||	rec->subslot_nr != subslot_nr
		||	rec->record_index != index
		||	rec->data_length < record_data_length)
		{
			CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): sequence_nr(%u != %u) api(%u != %u) slot(%u != %u)"
				, argr->device_nr, argr->ar_nr
				, rec->sequence_nr, seq_number
				, rec->api, api
				, rec->slot_nr, slot_nr
				);
			CM_CL_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): subslot(%u != %u) index(%u != %u) data_length(%u < %u)"
				, argr->device_nr, argr->ar_nr
				, rec->subslot_nr, subslot_nr
				, rec->record_index, index
				, rec->data_length, record_data_length
				);
			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
			cm_cl_arsm_down (argr, &dinfo);
			return CM_ERR_LOWER_LAYER;
		}

		rec->data_length = CM_RECORD_OFFSET + record_data_length;
		rec->ret_val_1 = ret_val_1;
		rec->ret_val_2 = ret_val_2;
		rec->cm_pnio_err = pnio_status_2;

		rec = CmListNext (& wrm->rec_list, & rec->link, CM_UPPER_CM_RECORD_PTR_TYPE);
	}

		/* some record is missing */
	if (is_not_null (rec)  ||  dp_left > 0)
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): is_not_null(rec)(%u)  or  dp_left(%u) > 0"
			, argr->device_nr, argr->ar_nr
			, is_not_null (rec) ? 1 : 0, dp_left
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
		cm_cl_arsm_down (argr, &dinfo);
		return CM_ERR_LOWER_LAYER;
	}


	return CM_OK;

cm_get_assert_failed:

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "unexpected parsing error: dev(%u) ar(%u)  offset(%u). assertion in line(%u)"
		, argr->device_nr, argr->ar_nr
		, ((LSA_UINT32)dp - (LSA_UINT32)dp_start)
		, cm_get_assert_line
		);
	LSA_UNUSED_ARG (cm_get_assert_error);
	
	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_WRITE));
	CM_CL_DINFO_SET_lnr (&dinfo, cm_get_assert_line);
	cm_cl_arsm_down (argr, &dinfo);
	return CM_ERR_LOWER_LAYER;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID CM_LOCAL_FCT_ATTR
cm_cl_prm_record_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_prm_record_free")
	LSA_UINT16  rc;
	CM_UPPER_CM_RECORD_PTR_TYPE  cm_rec;

	switch (CM_RQB_GET_OPCODE (upper)) {
	case CM_OPC_CL_RECORD_WRITE:
		if (is_not_null(upper->args.cl.write->rec.data)) { /* may be null (AP01457314) */
			CM_FREE_UPPER_MEM (& rc, upper->args.cl.write->rec.data, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			upper->args.cl.write->rec.data = LSA_NULL;
		}
		break;

	case CM_OPC_CL_RECORD_WRITE_MULTIPLE:
		if (is_not_null(upper->args.cl.write_multiple->data)) { /* may be null (AP01457314) */
			CM_FREE_UPPER_MEM (& rc, upper->args.cl.write_multiple->data, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			upper->args.cl.write_multiple->data = LSA_NULL;
		}

		while (! CmListIsEmpty (& upper->args.cl.write_multiple->rec_list)) {
			CmListRemoveHead (& upper->args.cl.write_multiple->rec_list, cm_rec, CM_UPPER_CM_RECORD_PTR_TYPE);
			CM_FREE_UPPER_MEM (& rc, cm_rec, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);

		}
		break;

	default:
		CM_FATAL1 (CM_RQB_GET_OPCODE (upper));
		break;
	}
	cm_upper_free_rqb (channel, upper);
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_prm_record_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_record_write")
	CM_UPPER_RQB_PTR_TYPE  cm;
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec;
	LSA_UINT16  list_cnt;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	CM_ASSERT (argr->cm.cl.prm.state == CM_CL_PRM_STATE_PRM);
	CM_ASSERT (! CmListIsEmpty (& argr->cm.cl.prm.prm_list));

#define CM_CL_PRM_RECORD_LEN(rec_) (rec_->tailor_data_len > 0 ? rec_->tailor_data_len : rec_->record_data_len)
#define CM_CL_PRM_RECORD_DATA(rec_) (rec_->tailor_data_len > 0 ? rec_->tailor_data : rec_->record_data)

	CmListLength (& argr->cm.cl.prm.prm_list, & list_cnt);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): list_cnt(%u) write_multi_is_supported(%u)  max_supported_record_length(%u)"
		, argr->device_nr, argr->ar_nr
		, list_cnt, dev->dev_rpc.write_multiple_is_supported, dev->dev_rpc.max_supported_record_length
		);

	cm = cm_upper_alloc_rqb (channel);
	if (is_null (cm))
	{
		return CM_ERR_ALLOC_UPPER;
	}
	CM_RQB_SET_HANDLE (cm, channel->my_handle);

		/* fetch first prm-record */
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: test write-multiple with AP00299791")
#endif /* CM_MESSAGE */
	rec = CmListFirst (& argr->cm.cl.prm.prm_list, CM_AR_GRAPH_RECORD_PTR_TYPE);
	CM_ASSERT (is_not_null (rec));

		/* calc real record ptr */
	rec = CM_CONTAINING_STRUCT (rec, CM_AR_GRAPH_RECORD_PTR_TYPE, prm_link);

		/* multi-write allowed */
	if (list_cnt > 1  &&  dev->dev_rpc.write_multiple_is_supported)
	{
		CM_UPPER_CL_RECORD_WRITE_MULTIPLE_PTR_TYPE  wrm;
		CM_UPPER_CM_RECORD_PTR_TYPE  cm_rec;
		LSA_UINT32  curr_len;
		LSA_UINT16  next_padding;
		LSA_UINT16  rc;

		CM_RQB_SET_OPCODE (cm, CM_OPC_CL_RECORD_WRITE_MULTIPLE);
		wrm = cm->args.cl.write_multiple;

		wrm->device_nr = argr->device_nr;
		wrm->ar_nr = argr->ar_nr;
		wrm->session_key = argr->session_key;
		wrm->data_length = 0;	/* let CM alloc ndr_data */
		wrm->data = LSA_NULL;
		CmListInitialize (& wrm->rec_list);
		wrm->cm_pnio_err = CM_PNIO_ERR_NONE;

		curr_len = CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE /* == CM_RECORD_OFFSET */;
		next_padding = 0;

		while (is_not_null (rec)
		&& curr_len + next_padding + CM_RECORD_HEADER_SIZE + CM_CL_PRM_RECORD_LEN(rec)
			<= dev->dev_rpc.max_supported_record_length)
		{
			curr_len += next_padding + CM_RECORD_HEADER_SIZE + CM_CL_PRM_RECORD_LEN(rec);

			next_padding = CM_CAST_U16(curr_len % 4);
			next_padding = CM_CAST_U16((4 - next_padding) % 4);

				/* append an new cm_record to the upper
				// note: cm_record.data is pointing to the prm_record */

			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& cm_rec)
				, cm_null_user_id, sizeof (*cm_rec), channel->sysptr
				);
			if (is_null (cm_rec))
			{
				cm_cl_prm_record_free (channel, cm);
				return CM_ERR_ALLOC_UPPER;
			}

			if (rec->record_index > 0xFFFF)
			{
				CM_FATAL1(rec->record_index); /* see norm, 0 <= record_index <= 0xFFFF */
			}

				/* use rec-pointer, remove from prm_list */
			CmListRemoveEntry (& rec->prm_link);

			cm_rec->api				= rec->api;
			cm_rec->slot_nr			= rec->slot_nr;
			cm_rec->subslot_nr		= rec->subslot_nr;
			cm_rec->record_index	= (LSA_UINT16)rec->record_index;
			cm_rec->data_length		= CM_CL_PRM_RECORD_LEN(rec);
			cm_rec->data			= CM_CL_PRM_RECORD_DATA(rec);

			CmListInsertTail (& wrm->rec_list, & cm_rec->link)

			CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u): multi prm-record, submodule(%u/%u/0x%x) index(0x%x) data_len(%u)"
				, argr->device_nr, argr->ar_nr
				, rec->api, rec->slot_nr, rec->subslot_nr
				, rec->record_index, CM_CL_PRM_RECORD_LEN(rec)
				);

				/* fetch next prm-record */
			if (! CmListIsEmpty (& argr->cm.cl.prm.prm_list))
			{
				rec = CmListFirst (& argr->cm.cl.prm.prm_list, CM_AR_GRAPH_RECORD_PTR_TYPE);
				CM_ASSERT (is_not_null (rec));
					/* calc real record ptr */
				rec = CM_CONTAINING_STRUCT (rec, CM_AR_GRAPH_RECORD_PTR_TYPE, prm_link);
			}
			else
			{
				rec = LSA_NULL;
			}
		}

		if (! CmListIsEmpty (& wrm->rec_list))
		{
			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF);
			rc = cm_cl_rpc_record_write_multiple (channel, argr, cm);
			if (rc != CM_OK)
			{
				CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF); /* nothing to wait for (AP01457314) */
				cm_cl_prm_record_free (channel, cm);
				return rc;
			}

				/* wf rpc_record_write_done */
				/* keep upper in record_list */
			CmListInsertHead (& argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)cm);
			return CM_OK;
		}
		else
		{
			/* fall-down: go to single-write */
		}
	}

		/* only single-write allowed */
	{
		CM_UPPER_CL_RECORD_WRITE_PTR_TYPE  write;
		LSA_UINT16  rc;

		CM_RQB_SET_OPCODE (cm, CM_OPC_CL_RECORD_WRITE);
		write = cm->args.cl.write;

		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& write->rec.data)
			, cm_null_user_id, CM_CAST_U16 (CM_CL_PRM_RECORD_LEN(rec) + CM_RECORD_OFFSET), channel->sysptr
			);
		if (is_null (write->rec.data)) {
			cm_upper_free_rqb (channel, cm);
			return CM_ERR_ALLOC_UPPER;
		}

		if (rec->record_index > 0xFFFF)
		{
			CM_FATAL1(rec->record_index); /* see norm, 0 <= record_index <= 0xFFFF */
		}

			/* use rec-pointer, remove from prm_list */
		CmListRemoveEntry (& rec->prm_link);

		write->device_nr		= argr->device_nr;
		write->ar_nr			= argr->ar_nr;
		write->rec.api			= rec->api;
		write->rec.slot_nr		= rec->slot_nr;
		write->rec.subslot_nr	= rec->subslot_nr;
		write->rec.record_index	= (LSA_UINT16)rec->record_index;
		write->rec.sequence_nr	= 0;
		write->rec.ret_val_1	= 0;
		write->rec.ret_val_2	= 0;

		write->rec.data_length = CM_RECORD_OFFSET + CM_CL_PRM_RECORD_LEN(rec);
		if (CM_CL_PRM_RECORD_LEN(rec) > 0)
		{
			CM_MEMCPY (write->rec.data + CM_RECORD_OFFSET, CM_CL_PRM_RECORD_DATA(rec), CM_CL_PRM_RECORD_LEN(rec));
		}

		CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): single prm-record, submodule(%u/%u/0x%x) index(0x%x) data_len(%u)"
			, argr->device_nr, argr->ar_nr
			, rec->api, rec->slot_nr, rec->subslot_nr
			, rec->record_index, CM_CL_PRM_RECORD_LEN(rec)
			);

		CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF);
		rc = cm_cl_rpc_record_write (channel, argr, cm);
		if (rc != CM_OK)
		{
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF); /* nothing to wait for (AP01457314) */
			cm_cl_prm_record_free (channel, cm);
			return rc;
		}

			/* wf rpc_record_write_done */
			/* keep upper in record_list */
		CmListInsertHead (& argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)cm);
		return CM_OK;
	}
#undef CM_CL_PRM_RECORD_LEN
#undef CM_CL_PRM_RECORD_DATA
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_record_read_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_record_read_write")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  rc;
	LSA_UINT16  prm_cnt = 0, rec_cnt = 0;

	CmListLength (& argr->cm.cl.record_list, & rec_cnt);
	CmListLength (& argr->cm.cl.prm.prm_list, & prm_cnt);

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u)  prm_state(%u)  wf1(0x%x)  prm_cnt(%u) rec_cnt(%u)"
		, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.state, argr->cm.cl.wait_for[1], prm_cnt, rec_cnt
		);

	switch (argr->cm.cl.prm.state) {
	case CM_CL_PRM_STATE_DOWN:
	case CM_CL_PRM_STATE_WF_DOWN:
		/* no action */
		break;

	case CM_CL_PRM_STATE_PRM:
		/* no next action */
		break;

	case CM_CL_PRM_STATE_UP:
		if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
		{
			if (rec_cnt > 0)
			{
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "CL-RecordRW: dev(%u) ar(%u) prm_state(UP) wf rpc user-read/write.cnf"
					, argr->device_nr, argr->ar_nr
					);
			}
			return;
		}
		/* start next user-record */
		while (! CmListIsEmpty (& argr->cm.cl.record_list))
		{
			upper = CmListFirst (& argr->cm.cl.record_list, CM_UPPER_RQB_PTR_TYPE);

			CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);

			cm_cl_prm_read_retrigger (argr); /* simulate prm_read_stop() */

			if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
			||	CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY)
			{
				rc = cm_cl_rpc_record_read (channel, argr, upper);
			}
			else if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)
			{
				rc = cm_cl_rpc_record_write (channel, argr, upper);
			}
			else
			{
				CM_FATAL1 (CM_RQB_GET_OPCODE (upper));
				return;
			}
			if (rc == CM_OK)
			{
				return;
			}
				/* error callback */
			CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF);
			CmListRemoveHead (& argr->cm.cl.record_list, upper, CM_UPPER_RQB_PTR_TYPE);
			cm_cl_ar_callback (argr, rc, upper);
		}
		/* CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) ar(%u): no (more) user-record", argr->device_nr, argr->ar_nr); */
		return;

	default:
		CM_FATAL1 (argr->cm.cl.prm.state);
		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_record_read_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_record_read_done")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  rc;

	if (CmListIsEmpty (& argr->cm.cl.record_list)) {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!no record waiting");
		return;
	}

	CmListRemoveHead (& argr->cm.cl.record_list, upper, CM_UPPER_RQB_PTR_TYPE);
	CM_ASSERT (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ
		|| CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_READ_QUERY
		);

	rc = cm_cl_rpc_record_read_done (channel, argr, rpc, upper);

	cm_cl_ar_callback (argr, rc, upper);

		/* start next */
	cm_cl_prm_trigger (argr, CM_CL_WF_RPC_RECORD_CNF);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_record_write_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_record_write_done")
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT16  rc;
	LSA_BOOL  was_prm;

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RECORD_CNF))
	{
		was_prm = LSA_FALSE;
	}
	else if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_WRITE_CNF))
	{
		was_prm = LSA_TRUE;
	}
	else
	{
		CM_FATAL();
		return;
	}

	if (CmListIsEmpty (& argr->cm.cl.record_list))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!no record waiting");
		return;
	}

	CmListRemoveHead (& argr->cm.cl.record_list, upper, CM_UPPER_RQB_PTR_TYPE);

	if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE)
	{
		rc = cm_cl_rpc_record_write_done (channel, argr, rpc, upper);
	}
	else if (CM_RQB_GET_OPCODE (upper) == CM_OPC_CL_RECORD_WRITE_MULTIPLE)
	{
		rc = cm_cl_rpc_record_write_multi_done (channel, argr, rpc, upper);
	}
	else
	{
		CM_FATAL1 (CM_RQB_GET_OPCODE (upper));
		return;
	}

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev.prm_state(%d) rc(%d)"
		, argr->cm.cl.prm.state
		, rc
		);

	if (was_prm)
	{
			/* free internal upper */
		cm_cl_prm_record_free (channel, upper);

		cm_cl_prm_trigger (argr, CM_CL_WF_RPC_PRM_WRITE_CNF);
	}
	else
	{
			/* normal record-write cnf */
		cm_callback (channel, rc, & upper);

		cm_cl_prm_trigger (argr, CM_CL_WF_RPC_RECORD_CNF);
	}
}



/*------------------------------------------------------------------------------
//	PRM-Read - retrigger CMSV-CMITimeout
//----------------------------------------------------------------------------*/

#define CM_CL_PRM_READ_NONE		0
#define CM_CL_PRM_READ_RUNNING	1


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_start (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_start")
	LSA_UINT32  read_timeout;

	/*
	//	see	CMInitiatorTriggerTimeoutFactor = CMInitiatorActivityTimeoutFactor DIV 3
	//	and Table CMInitiatorTriggerTimeoutFactor
	//	| 33 .. 333 | With a time base of 100 ms |
	//
	*/
	read_timeout = argr->ar.cmi_timeout100ms * 100; /* msec */
	read_timeout /= 3;
	if (! (33*100 <= read_timeout && read_timeout <= 333*100))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u) ar(%u): prm-read: resize read_timeout(%u) to 3300 msec"
			, argr->device_nr, argr->ar_nr, read_timeout
			);
		read_timeout = 33 * 100; /* msec */
	}
	argr->cm.cl.prm.read_timeout	= read_timeout; /* msec */

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): prm-read: starting"
		, argr->device_nr, argr->ar_nr
		);

	CM_ASSERT (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE);
	argr->cm.cl.prm.read_state = CM_CL_PRM_READ_RUNNING;

	cm_cl_prm_read_retrigger (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_retrigger (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_retrigger")

	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_RUNNING)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u) ar(%u): prm-read: retrigger read-record with timeout(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.read_timeout
			);
		cm_oneshot_trigger (& argr->cm.cl.prm.read_oneshot, argr->cm.cl.prm.read_timeout);
	}
	else
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): prm-read: retrigger stopped in meantime"
			, argr->device_nr, argr->ar_nr
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_timeout (
	CM_ONESHOT_PTR_TYPE  timeout,
	LSA_VOID_PTR_TYPE  void_argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_timeout")
	CM_AR_GRAPH_PTR_TYPE  argr = (CM_AR_GRAPH_PTR_TYPE)void_argr;

	LSA_UNUSED_ARG (timeout);

	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): prm-read: timeout stopped in meantime"
			, argr->device_nr, argr->ar_nr
			);
		return; /* timeout to late */
	}

	cm_cl_prm_read_rpc_trigger_req (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_rpc_trigger_req (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_rpc_trigger_req")
	CM_CHANNEL_PTR_TYPE  channel = argr->cm.cl.dev->channel;
	CM_UPPER_RQB_PTR_TYPE  upper;
	LSA_UINT32  alloc_len;
	CM_COMMON_MEM_U8_PTR_TYPE  ndr_data;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_ASSERT (is_not_null (channel));

		/* build the read-record */
	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) ar(%u): prm-read: start rpc-read of rpc-trigger record"
		, argr->device_nr, argr->ar_nr
		);

	upper = cm_upper_alloc_rqb (channel);
	if (is_null (upper))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u): prm-read: no cm_upper_alloc_rqb()"
			, argr->device_nr, argr->ar_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	alloc_len = CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE + 0/*2009-12-03: this is a record with no body*/;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& ndr_data), cm_null_user_id, CM_CAST_U16(alloc_len), channel->sysptr);
	if (is_null (ndr_data))
	{
		cm_upper_free_rqb (channel, upper);
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u): prm-read: no mem for ndr_data, alloc_len(%u)"
			, argr->device_nr, argr->ar_nr, alloc_len
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	CM_RQB_SET_OPCODE (upper,			CM_OPC_CL_RECORD_READ);

	upper->args.cl.read->device_nr		= argr->device_nr;
	upper->args.cl.read->ar_nr			= argr->ar_nr;
	upper->args.cl.read->session_key	= argr->session_key;

	upper->args.cl.read->rec.api				= 0;
	upper->args.cl.read->rec.slot_nr			= 0;
	upper->args.cl.read->rec.subslot_nr			= 0;
	upper->args.cl.read->rec.record_index		= CM_RECORD_INDEX_RPC_TRIGGER;
	upper->args.cl.read->rec.sequence_nr		= 0;
	upper->args.cl.read->rec.data_length		= alloc_len;
	upper->args.cl.read->rec.data				= ndr_data;
	upper->args.cl.read->rec.cm_pnio_err		= 0;
	upper->args.cl.read->rec.ret_val_1			= 0;
	upper->args.cl.read->rec.ret_val_2			= 0;
	upper->args.cl.read->rec.readquery_length	= 0;
	upper->args.cl.read->rec.is_iosar_with_device_access = 0xCC; /* don't evaluate */

		/* all prepared, now let trigger read-write-statemachine */
	CmListInsertTail (& argr->cm.cl.record_list, (CM_LIST_ENTRY_PTR_TYPE)upper);
	cm_cl_prm_trigger (argr, 0);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_rpc_trigger_cnf (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_UPPER_RQB_PTR_TYPE  upper
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_record_cnf")
	LSA_UINT16  rc;

	CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): prm-read: rpc-trigger record done"
		, argr->device_nr, argr->ar_nr
		);

	CM_FREE_UPPER_MEM (&rc, upper->args.cl.read->rec.data, argr->ar_com.channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);

	cm_upper_free_rqb (argr->ar_com.channel, upper);

		/* retrigger next timeout */
	cm_cl_prm_read_retrigger (argr);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_read_stop (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_read_stop")

	if (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_RUNNING)
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u) ar(%u): prm-read: stopping"
			, argr->device_nr, argr->ar_nr
			);

		argr->cm.cl.prm.read_state = CM_CL_PRM_READ_NONE;
		cm_oneshot_stop (& argr->cm.cl.prm.read_oneshot);
	}

	CM_ASSERT (argr->cm.cl.prm.read_state == CM_CL_PRM_READ_NONE);
}


/*------------------------------------------------------------------------------
//	ApplReadyTimer, used for two timeouts
//	__RemoteApplReady		.. between PrmEnd and ApplRdy
//	__PreDataInd			.. between ApplRdy and PreData.ind
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_ardy_timeout_start (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  which_one
	)
{
	CM_LOG_FCT ("cm_cl_prm_ardy_timeout_start")

	switch (which_one)
	{
	case CM_CL_ARDY_TIMEOUT__RemoteApplReady:
		/*
		//	Remote_Application_Ready_Timeout (Value <= 300 sec)  see part-2
		//	  The parameter Remote_Application_Ready_Timeout represents the deadline between
		//	  IODControlRes (ControlCommand.PrmEnd) and IOXControlReq (ControlCommand.ApplicationReady).
		*/
		CM_ASSERT (argr->cm.cl.applrdy_state != CM_CL_APPL_READY_STATE_UP);

		argr->cm.cl.prm.ardy_timer = which_one;
		argr->cm.cl.prm.ardy_timeout_msec = 300 * 1000;

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): ardy-timer: starting remote_ardy_timeout(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.ardy_timeout_msec
			);

		cm_oneshot_trigger (& argr->cm.cl.prm.ardy_oneshot, argr->cm.cl.prm.ardy_timeout_msec);
		break;

	case CM_CL_ARDY_TIMEOUT__PreDataInd:
		CM_ASSERT (argr->cm.cl.applrdy_state == CM_CL_APPL_READY_STATE_UP);

		argr->cm.cl.prm.ardy_timer = which_one;
		argr->cm.cl.prm.ardy_timeout_msec = 3000;

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): ardy-timer: starting, predata_timeout(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.ardy_timeout_msec
			);

		cm_oneshot_trigger (& argr->cm.cl.prm.ardy_oneshot, argr->cm.cl.prm.ardy_timeout_msec);
		break;

	default:
		CM_FATAL();
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_ardy_timeout (
	CM_ONESHOT_PTR_TYPE  timeout,
	LSA_VOID_PTR_TYPE  void_argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_ardy_timeout")
	CM_AR_GRAPH_PTR_TYPE  argr = (CM_AR_GRAPH_PTR_TYPE)void_argr;
	LSA_UINT16  ardy_timer = argr->cm.cl.prm.ardy_timer;

	LSA_UNUSED_ARG (timeout);

	argr->cm.cl.prm.ardy_timer = 0;

	switch (ardy_timer)
	{
	case CM_CL_ARDY_TIMEOUT__RemoteApplReady:
		{
			CM_CL_DOWN_INFO_TYPE  dinfo;

			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u) ar(%u): ardy-timer: remote_application_ready_timeout"
				, argr->device_nr, argr->ar_nr
				);

			CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_ARDY, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ARDY));
			cm_cl_arsm_down (argr, &dinfo);
		}
		break;

	case CM_CL_ARDY_TIMEOUT__PreDataInd:
		{
			LSA_BOOL is_up = cm_ar_com_is_up (argr);
			LSA_BOOL is_down = cm_ar_com_is_down (argr);

			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u): ardy-timer: if !is_up(%u) && !is_down(%u) do PreData.ind"
				, argr->device_nr, argr->ar_nr, is_up, is_down
				);

			if (! is_up  &&  ! is_down)
			{
				CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
				cm_cl_arsm (argr);
			}
			else
			{
				/* too late, but not state switch yet, e.g. CM_CL_PRMMODE_PLUG */
			}
		}
		break;

	default:
		CM_FATAL1 (argr->cm.cl.prm.ardy_timer);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_prm_ardy_timeout_stop (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_prm_ardy_timeout_stop")

	argr->cm.cl.prm.ardy_timer = 0;

	if (cm_oneshot_is_running (& argr->cm.cl.prm.ardy_oneshot))
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u): ardy-timer: stopped, remaining ticks(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.prm.ardy_oneshot.ticks
			);

		cm_oneshot_stop (& argr->cm.cl.prm.ardy_oneshot);
	}
}


/*------------------------------------------------------------------------------
//	UUID_IO_ControllerInterface :: rpc implicit read
//----------------------------------------------------------------------------*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: read-implicit (make own file, etc)")
#endif /* CM_MESSAGE */

typedef struct cm_cl_record_params_tag {

	/* rpc opnum things */

	LSA_BOOL is_read;
	LSA_BOOL is_read_implicit;

	/* parsed things */

	LSA_UINT16 seq_number;

	CLRPC_UUID_TYPE ar_uuid;

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;
	LSA_UINT16 padding1;
	LSA_UINT16 index;

	LSA_UINT32 record_data_length;

	CLRPC_UPPER_MEM_U8_PTR_TYPE target_ar_uuid_ptr;

} CM_CL_RECORD_PARAMS_TYPE;

typedef CM_CL_RECORD_PARAMS_TYPE CM_COMMON_MEM_ATTR *CM_CL_RECORD_PARAMS_PTR_TYPE;

/*----------------------------------------------------------------------------*/
static LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_cl_record_header_parse(
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_CL_RECORD_PARAMS_PTR_TYPE req
) {
	CM_LOG_FCT ("cm_cl_record_header_parse")
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_len;

	/***/

	CM_IOD_READ_WRITE_HEADER_SIZE_ASSERT();

	CM_ASSERT(CLRPC_PNIO_OFFSET + sizeof(CM_BLK_HEADER) + sizeof(CM_BLK_READ_WRITE) == CM_RECORD_OFFSET);

	/***/

	CM_ASSERT(rpc->args.sv.call->ndr_len >= CLRPC_PNIO_OFFSET); /* see caller */

	data_ptr = rpc->args.sv.call->ndr_data;
	data_len = rpc->args.sv.call->ndr_len;

	if( req->is_read ) {

		if( data_len != CM_RECORD_OFFSET ) {
			CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "read, wrong data_len(%u)"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__RMPM_ArgsLengthInvalid(); /* read, wrong length */
		}
	}
	else { /* write */

		CM_ASSERT( ! req->is_read_implicit); /* impossible */

		if( data_len < CM_RECORD_OFFSET ) {
			CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "write, data_len(%u) too short"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__RMPM_ArgsLengthInvalid(); /* write, too few data */
		}
	}

	data_ptr += CLRPC_PNIO_OFFSET;
	data_len -= CLRPC_PNIO_OFFSET;

	/***/

	{
	LSA_UINT16 block_type;
	LSA_UINT16 block_length;
	LSA_UINT16 block_version;

	block_type    = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_HEADER, type));
	block_length  = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_HEADER, length));
	block_version = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_HEADER, version));

	data_ptr += sizeof(CM_BLK_HEADER);
	data_len -= sizeof(CM_BLK_HEADER);

	/***/

	if( req->is_read ) {

		if( block_type != CM_BLOCK_TYPE_READ_REQ ) {
			CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "read, wrong block_type(%u)"
				, block_type
				);
			return CM_PNIO_FAULTY_Record__Block(0); /* read, wrong block-type */
		}
	}
	else { /* write */

		if( block_type != CM_BLOCK_TYPE_WRITE_REQ ) {
			CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "write, wrong block_type(%u)"
				, block_type
				);
			return CM_PNIO_FAULTY_Record__Block(0); /* write, wrong block-type */
		}
	}

	if( block_length != (1+1)/*version*/ + sizeof(CM_BLK_READ_WRITE) ) {
		CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "wrong block_length(%u)"
				, block_length
				);
		return CM_PNIO_FAULTY_Record__Block(1); /* wrong block-length*/
	}

	if( (block_version & 0xFF00) != 0x0100/*V1.0*/ ) {
		CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "wrong block_version-high(%u)", block_version);
		return CM_PNIO_FAULTY_Record__Block(2); /* wrong block-version-high */
	}
	if( (block_version & 0x00FF) != 0x0000/*V1.0*/ ) {
		CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "wrong block_version-low(%u)", block_version);
		return CM_PNIO_FAULTY_Record__Block(3); /* wrong block-version-low */
	}
	}

	/***/

	req->seq_number  = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, seq_number));

	CM_GET_UUID_NTOH(&req->ar_uuid, data_ptr, CM_OFFS(CM_BLK_READ_WRITE, ar_uuid));

	req->api         = CM_GET32_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, api));
	req->slot_nr     = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, slot_nr));
	req->subslot_nr  = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, subslot_nr));

	req->padding1    = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, padding_1));
	req->index       = CM_GET16_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, index));

	req->record_data_length = CM_GET32_NTOH(data_ptr, CM_OFFS(CM_BLK_READ_WRITE, record_data_length));

	req->target_ar_uuid_ptr = data_ptr + CM_OFFS(CM_BLK_READ_WRITE, u.req_rd_implicit.target_ar_uuid);

	data_ptr += sizeof(CM_BLK_READ_WRITE);
	data_len -= sizeof(CM_BLK_READ_WRITE);

	LSA_UNUSED_ARG (data_ptr); /* lint */
	LSA_UNUSED_ARG (data_len); /* lint */

	/***/

	if( req->padding1 != 0 ) {

		/* NOTE: 'index' maybe be expanded to 32 bit in the future */
		/* to allow this, we have to check the padding (the HighWord) for zero */

		CM_CL_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
				, "padding_1 not zero"
				);
		return CM_PNIO_FAULTY_Record__InvalidRange(9); /* padding_1 not zero */
	}

	/***/

	req->record_data_length += CM_RECORD_OFFSET; /* include the rpc- and record-header, see cm_sv_record_event_indicate() */

	if( req->record_data_length < CM_RECORD_OFFSET ) { /* wrap around at 4GB */

		CM_CL_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "record-length(%u) too big"
				, req->record_data_length - CM_RECORD_OFFSET
				);
		return CM_PNIO_FAULTY_Record__RMPM_ArgsLengthInvalid(); /* record-length inconsistent with ndr-length */
	}

	if( req->is_read ) {

		if( req->record_data_length > rpc->args.sv.call->alloc_len ) {

			/* we must accept this record-read, but we need to shorten the length indicated to the user */

			CM_CL_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH /* not LEVEL_UNEXP */
				, "read, %u + length(%u) > alloc_len(%u), shortening the request"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->alloc_len
				);

			req->record_data_length = rpc->args.sv.call->alloc_len;
		}
	}
	else { /* write */

		if( req->record_data_length != rpc->args.sv.call->ndr_len ) {

			CM_CL_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP
				, "write, %u + length(%u) != ndr_len(%u), inconsistency"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->ndr_len
				);
			return CM_PNIO_FAULTY_Record__RMPM_ArgsLengthInvalid(); /* write, record-length inconsistent with ndr-length */
		}
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_record_header_update(
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
) {
	CM_LOG_FCT ("cm_cl_record_header_update")

	/***/

	CM_ASSERT(CM_RECORD_OFFSET == CLRPC_PNIO_OFFSET + sizeof(CM_BLK_HEADER) + sizeof(CM_BLK_READ_WRITE));

	/***/

	if( is_read ) {
		CM_PUT16_HTON(header_ptr, CM_OFFS(CM_BLK_HEADER, type), CM_BLOCK_TYPE_READ_RSP);
	}
	else {
		CM_PUT16_HTON(header_ptr, CM_OFFS(CM_BLK_HEADER, type), CM_BLOCK_TYPE_WRITE_RSP);
	}

	CM_PUT16_HTON(header_ptr, CM_OFFS(CM_BLK_HEADER, length), 1+1/*version*/ + sizeof(CM_BLK_READ_WRITE));

	CM_PUT16_HTON(header_ptr, CM_OFFS(CM_BLK_HEADER, version), 0x0100/*V1.0*/);

	header_ptr += sizeof(CM_BLK_HEADER);

	/*
	 * update the length and additional-values, the rest remains unchanged
	 */

	CM_PUT32_HTON(
		header_ptr
		, CM_OFFS(CM_BLK_READ_WRITE, record_data_length)
		, ndr_length - CM_RECORD_OFFSET
	);

	CM_PUT16_HTON(
		header_ptr
		, CM_OFFS(CM_BLK_READ_WRITE, u.rsp_rd_wr.additional_value_1)
		, ret_val_1
	);

	CM_PUT16_HTON(
		header_ptr
		, CM_OFFS(CM_BLK_READ_WRITE, u.rsp_rd_wr.additional_value_2)
		, ret_val_2
	);

	/*
	 * write-rsp: update the pnio-status
	 */

	if( ! is_read ) {

		CM_PUT32_HTON(
			header_ptr
			, CM_OFFS(CM_BLK_READ_WRITE, u.rsp_wr.pnio_status)
			, pnio_err
		);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_read_impl_ind_search (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	LSA_UINT16  i;
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR  *rind;

	/* rpc == LSA_NULL: search new one
	// rpc != LSA_NULL: search existing one
	*/
	for (i = 0; i < CM_CFG_MAX_CL_IMPLICIT_READ_IND; ++i)
	{
		rind = & channel->usr.cl.ipnio_controller__read_implicit[i];
		if (are_equal (*rind, rpc))
		{
			return CM_CAST_U16(i+1);
		}
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static CM_RPC_LOWER_RQB_PTR_TYPE CM_LOCAL_MEM_ATTR * CM_LOCAL_FCT_ATTR
cm_cl_read_impl_ind_from_idx (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  idx
	)
{
	if (1 <= idx && idx <= CM_CFG_MAX_CL_IMPLICIT_READ_IND)
	{
		return & channel->usr.cl.ipnio_controller__read_implicit[idx - 1];
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pnio_c_read_implicit_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_pnio_c_read_implicit_ind")
	LSA_UINT16  ridx;
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR  *rind;
	CM_CL_RECORD_PARAMS_TYPE  req;
	LSA_UINT32  pnio_err;
	LSA_UINT32  cls;
	LSA_UINT32  cls_allowed;

	/* param check */

	req.is_read = LSA_TRUE;
	req.is_read_implicit = LSA_TRUE;

	pnio_err = cm_cl_record_header_parse (rpc, &req);
	if (pnio_err != CM_PNIO_ERR_NONE)
	{
		rpc->args.sv.call->ndr_len = CLRPC_PNIO_OFFSET;
		rpc->args.sv.call->pnio_status = pnio_err;

		CM_RPC_SET_RESPONSE(rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
		return;
	}

	if (channel->state != CM_CL_READY)
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "channel.state(%u) != Ready, ignore read-implicit"
			, channel->state
			);
		pnio_err = CM_PNIO_FAULTY_Record__ResourceBusy(0);
		goto complete;
	}

	ridx = cm_cl_read_impl_ind_search (channel, LSA_NULL);
	if (ridx == 0)
	{
		CM_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "another read-implicit is still running"
			);
		pnio_err = CM_PNIO_FAULTY_Record__ResourceBusy(0);
		goto complete;
	}


	cls = cm_record_index_classify (req.api, req.index, channel->usr.cl.vendor_id);

	cls_allowed = CM_REC_IDX_SIMATIC;

	#if CM_CFG_CL_ALLOW_DIAG_AND_PDEV	/*AP00539136*/
		cls_allowed |= (CM_REC_IDX_DIAG | CM_REC_IDX_PDEV);
	#endif

	if ((cls & cls_allowed) != 0  &&  (cls & CM_REC_IDX_READ) != 0)
	{
		CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_PD);
		CM_PD_PORT_PTR_TYPE  port;

		if ((cls & CM_REC_IDX_8000) != 0)
		{
			switch (req.index) {
			case 0xB000: /* GSY: Sync-Log, RTA SyncID 0 */
			case 0xB001: /* GSY: Sync-Log, RTA SyncID 1 */
			case 0xB050: /* EDD: Ext_PLL control (for RTA+RTC SyncID 0) */
			case 0xB051: /* GSY: Ext_PLL control (for RTA SyncID 1) */
			case 0xB061: /* EDD: Trace-Unit data */
			case 0xB070: /* OHA: PDInterfaceAdditionalInfo */
				port = cm_pd_port_from_id (pd_channel, 0/*interface*/);
				break;

			case 0xB060: /* EDD: Trace-Unit control */
			default:
				port = cm_pd_port_from_addr (pd_channel, req.slot_nr, req.subslot_nr);
				break;
			}
		}
		else
		{
			port = cm_pd_port_from_id (pd_channel, 0/*interface*/);
		}

		if( is_null(port) )
		{
			pnio_err = CM_PNIO_FAULTY_Record__FeatureNotSupported(0);
			goto complete;
		}
		else if( port->addr.subslot_nr == 0 ) /* not (yet) mapped */
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "interface not (yet) mapped, slot(%u) subslot(0x%x)"
				, req.slot_nr, req.subslot_nr
				);
			pnio_err = CM_PNIO_FAULTY_Record__FeatureNotSupported(0); /* not telling InvalidSlotSubslot because params are ignored... */
			goto complete;
		}
		else
		{
			req.api = 0;
			req.slot_nr = port->addr.slot_nr;
			req.subslot_nr = port->addr.subslot_nr;
		}
	}
	else
	{
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unknown index(0x%x) cls(0x%x) cls_allowed(0x%x)"
			, req.index, cls, cls_allowed
			);
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		goto complete;
	}

	CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rpc-read-implicit.ind: rpc(0x%08x) opnum(%d) ndr_len(%d)  index(0x%x) slot(%u) subslot(0x%x)"
		, rpc, rpc->args.sv.call->opnum, rpc->args.sv.call->ndr_len
		, req.index, req.slot_nr, req.subslot_nr
		);

	{
	CM_UPPER_RQB_PTR_TYPE  pd;

	pd = cm_upper_alloc_rqb(channel);
	if (is_null(pd))
	{
		CM_CL_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "no mem"
			);
		pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(255); /* no mem */
		goto complete;
	}

	/***/

	{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

	prm_rdwr->slot_nr = req.slot_nr;
	prm_rdwr->subslot_nr = req.subslot_nr;
	prm_rdwr->record_index = req.index;
	prm_rdwr->record_data_length = req.record_data_length - CM_RECORD_OFFSET;
	prm_rdwr->record_data = rpc->args.sv.call->ndr_data + CM_RECORD_OFFSET;

	prm_rdwr->is_local = LSA_FALSE;
	prm_rdwr->cls = cls;
	}


	rind = cm_cl_read_impl_ind_from_idx (channel, ridx);
	CM_ASSERT (is_null (*rind));
	*rind = rpc; /* keep */

	cm_pdcl_request(channel, CM_OPC_PD_PRM_READ_CLIENT, ridx, 0, pd);
	}

	return; /* pending */

	/***/

complete:

	CM_ASSERT (pnio_err != CM_PNIO_ERR_NONE);

	rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET;
	rpc->args.sv.call->pnio_status = pnio_err;

	cm_cl_record_header_update (rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET
		, LSA_TRUE/*read*/
		, rpc->args.sv.call->ndr_len
		, pnio_err, 0, 0
		);

	CM_RPC_SET_RESPONSE(rpc, CLRPC_OK);
	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pnio_c_read_implicit_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  pd
	)
{
	CM_LOG_FCT ("cm_pnio_c_read_implicit_rsp")
	CM_RPC_LOWER_RQB_PTR_TYPE  CM_LOCAL_MEM_ATTR  *rind;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT32  pnio_err;

	CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_PD);

	channel = cm_peer_channel_get(channel, CM_PATH_TYPE_CLIENT);
	if (is_null(channel)) {
		CM_FATAL();
		return;
	}

	rind = cm_cl_read_impl_ind_from_idx (channel, CM_EDD_GET_USER_ID_DEV(pd));
	if (is_null (rind) || is_null (*rind))
	{
		CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "cannot find ridx(0x%x)", CM_EDD_GET_USER_ID_DEV(pd)
			);
		CM_FATAL();
		return;
	}

	rpc = *rind;
	*rind = LSA_NULL;

	if( CM_RQB_GET_RESPONSE(pd) != CM_OK )
	{
		rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET;

		pnio_err = cm_pd_response_to_pnio_err (CM_RQB_GET_RESPONSE (pd), LSA_TRUE);
	}
	else
	{
		rpc->args.sv.call->ndr_len = CM_RECORD_OFFSET + pd->args.pd.prm_rdwr->record_data_length;

		pnio_err = CM_PNIO_ERR_NONE;
	}

	cm_cl_record_header_update (rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET
		, LSA_TRUE/*read*/
		, rpc->args.sv.call->ndr_len
		, pnio_err, 0, 0
		);

	rpc->args.sv.call->pnio_status = pnio_err;

	CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */

	cm_upper_free_rqb(channel, pd);
}

/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
