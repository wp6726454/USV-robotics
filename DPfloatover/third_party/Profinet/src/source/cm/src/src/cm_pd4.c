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
/*  F i l e               &F: cm_pd4.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  840
#define CM_MODULE_ID       840

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_map_check (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  map_count,
	CM_UPPER_PD_MAP_PTR_TYPE  map_array
) {
	CM_LOG_FCT("cm_pd_port_addr_map_check")
	LSA_UINT16  port_id;
	CM_CHANNEL_PTR_TYPE  edd_channel = cm_edd_channel_get (channel);
	LSA_UINT16  interface_id = (LSA_UINT16)((edd_channel->lower.info.edd.InterfaceID - 1) << 8);

	for (port_id = 0; port_id < map_count; ++port_id)
	{
		CM_UPPER_PD_MAP_PTR_TYPE  map = (map_array + port_id);

		/* format: 0x8ipp */

		if (! CM_IS_PDEV_SUBSLOT_NR(map->subslot_nr))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "map-check: port_id(%u) subslot_nr(0x%x) with wrong format"
				, port_id, map->subslot_nr
				);
			return CM_ERR_PARAM;
		}

		if ((map->subslot_nr & 0x0F00) != interface_id)
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "map-check: port_id(%u) subslot_nr(0x%x) does not match to edd.InterfaceID(0x%x)"
				, port_id, map->subslot_nr, interface_id
				);
			return CM_ERR_PARAM;
		}

		/* note: pp != port_id is ok (AP00774150) */

		{
		LSA_UINT16  port_id_2;

		for (port_id_2 = port_id + 1; port_id_2 < map_count; ++port_id_2)
		{
			CM_UPPER_PD_MAP_PTR_TYPE  map_2 = (map_array + port_id_2);

			if (map->slot_nr    == map_2->slot_nr
			&&	map->subslot_nr == map_2->subslot_nr)
			{
				CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "map-check: port_id(%u) slot(%u) subslot(0x%x) mapped also to port_id_2(%u)"
					, port_id, map->slot_nr, map->subslot_nr, port_id_2
					);
				return CM_ERR_PARAM;
			}
		}
		}
	}
	return CM_OK;
}


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_map (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  port_id,
	CM_UPPER_PD_MAP_PTR_TYPE  map
) {
	CM_LOG_FCT("cm_pd_port_addr_map")
	CM_PD_PORT_PTR_TYPE  port;

	CM_PD_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "addr-map: port_id(%u) map to slot(%u) subslot(0x%x)"
		, port_id, map->slot_nr, map->subslot_nr
		);

	port = cm_pd_port_from_id__nn (channel, port_id);

	CM_ASSERT (port->addr.subslot_nr == 0);
	CM_ASSERT (map->subslot_nr != 0);
	CM_ASSERT (port->port_id == port_id);

	port->addr.slot_nr		= map->slot_nr;
	port->addr.subslot_nr	= map->subslot_nr;

	port->addr.mod_ident	= map->mod_ident;
	port->addr.sub_ident	= map->sub_ident;
}


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_unmap (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  port_id
) {
	CM_LOG_FCT("cm_pd_port_addr_unmap")

	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, port_id);

	if (cm_pd_port_is_mapped (port))
	{
		port->addr.slot_nr		= 0;
		port->addr.subslot_nr	= 0;	/* format: 0x8ipp */
		port->addr.mod_ident	= 0;
		port->addr.sub_ident	= 0;
	}
	else
	{
		CM_ASSERT (port->addr.slot_nr		== 0);
		CM_ASSERT (port->addr.subslot_nr	== 0);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_PRM_READ
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_read (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_opc_prm_read")
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr;

	prm_rdwr = rb->args.pd.prm_rdwr;

	CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PD-read: rqb(0x%x) slot_nr(%u) subslot_nr(0x%x) index(0x%x)"
		, rb
		, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index
		);

	/***/

	{
	LSA_UINT16 rc = cm_pd_req_accept (channel, rb, CM_PD_READY);

	if (rc != CM_OK) {
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}
	}

	prm_rdwr->is_local = LSA_TRUE;
	prm_rdwr->cls = cm_record_index_classify(0/*!*/, prm_rdwr->record_index, CM_VENDOR_ID_SIEMENS/*!*/);

	cm_pd_req_queue_add (channel, rb);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_PRM_BEGIN
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_prepare(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_LOG_FCT("cm_pd_opc_prm_prepare")

	/* no params */

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		,"PD-prepare: rqb(0x%x)"
		, rb
		);

	/***/

	{
	LSA_UINT16 rc = cm_pd_req_accept (channel, rb, CM_PD_READY);

	if (rc != CM_OK) {
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}
	}

	cm_pd_req_queue_add (channel, rb);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_PRM_WRITE
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_write (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_opc_prm_write")
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PD-write: rqb(0x%x) record_index(0x%x) slot(%u) subslot(0x%x) record_data_length(%u)"
		, rb
		, prm_rdwr->record_index, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
		, prm_rdwr->record_data_length
		);

	/***/

	{
	LSA_UINT16 rc = cm_pd_req_accept (channel, rb, CM_PD_READY);

	if (rc != CM_OK) {
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}
	}

	prm_rdwr->is_local = LSA_TRUE;
	prm_rdwr->cls = cm_record_index_classify(0/*!*/, prm_rdwr->record_index, CM_VENDOR_ID_SIEMENS/*!*/);

	cm_pd_req_queue_add (channel, rb);
}

/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_PRM_END
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_end (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_opc_prm_end")

	/* no params */

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		,"PD-prm-end: rqb(0x%x)"
		, rb
		);

	/***/

	{
	LSA_UINT16 rc = cm_pd_req_accept (channel, rb, CM_PD_READY);

	if (rc != CM_OK) {
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}
	}

	cm_pd_req_queue_add (channel, rb);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_ALARM_RSP
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_alarm_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_opc_alarm_rsp")
	CM_UPPER_ALARM_PTR_TYPE param;

	param = rb->args.channel.alarm;

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PD-alarm-rsp: rqb(0x%x) slot_nr(%u) subslot_nr(%u) alarm_type(%u) prio(%u)"
		, rb
		, param->slot_nr, param->subslot_nr, param->alarm_type, param->alarm_priority
		);
	LSA_UNUSED_ARG (param);

	/***/

	CM_ASSERT (channel->usr.pd.req.alarm_ind_count > 0);
	channel->usr.pd.req.alarm_ind_count -= 1;

	cm_pd_alarm_provide (channel, rb);
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_EVENT_APPL_READY_RSP
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_appl_ready_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_opc_appl_ready_rsp")
	CM_UPPER_EVENT_PTR_TYPE  upper_event;

	CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "PD-appl-ready-rsp: rqb(0x%x)", rb
		);

	upper_event = rb->args.channel.event;

	cm_pd_diff_list_free (channel, & upper_event->u.pd.appl_ready.diff_list);

	if (is_not_null (upper_event->u.pd.appl_ready.mod_diff_block))
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.pd.appl_ready.mod_diff_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
		upper_event->u.pd.appl_ready.mod_diff_block = LSA_NULL;
	}

	if (is_not_null (upper_event->u.pd.appl_ready.diag_block)) /* AP01427283 */
	{
		LSA_UINT16  rc;
		CM_FREE_UPPER_MEM (&rc, upper_event->u.pd.appl_ready.diag_block, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		LSA_UNUSED_ARG (rc);
		upper_event->u.pd.appl_ready.diag_block = LSA_NULL;
	}

	CM_RQB_SET_OPCODE (rb, CM_OPC_EVENT_IND_RES_PROVIDE);
	cm_event_provide (channel, rb);

	if (channel->usr.pd.almi.appl_ready_ind_pending != CM_ERR_NOT_YET) /* retrigger appl_ready_ind */
	{
		LSA_UINT16 rsp = channel->usr.pd.almi.appl_ready_ind_pending;

		channel->usr.pd.almi.appl_ready_ind_pending = CM_ERR_NOT_YET;

		cm_pd_callback_appl_ready(channel, rsp);
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_LINK_STATUS_INFO
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_link_status_info (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_opc_link_status_info")
	CM_PD_PORT_PTR_TYPE  port;

	if (cm_pd_req_accept (channel, rb, CM_PD_READY) != CM_OK)
	{
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	port = cm_pd_port_from_id (channel, rb->args.pd.link_status->port_id);
	if (is_null (port))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "LinkStatusInfo: port_id(%u) doesn't exist", rb->args.pd.link_status->port_id
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (is_not_null (port->misc.link_status_ind))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "LinkStatusInfo: port_id(%u) has already a indication resource, rb(%08x)"
			, rb->args.pd.link_status->port_id, port->misc.link_status_ind
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	port->misc.link_status_ind = rb;

	cm_pd_link_status_trigger_user (channel, port->port_id);
}


/*------------------------------------------------------------------------------
// common for CM-PD and CM-CL
//----------------------------------------------------------------------------*/

CM_UPPER_PD_DIFF_AP_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_ap_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT32  api
	)
{
	CM_LOG_FCT ("cm_pd_diff_ap_find_append")
	CM_UPPER_PD_DIFF_AP_PTR_TYPE  diff_ap;

	for (CmListForeach (diff_ap, head, CM_UPPER_PD_DIFF_AP_PTR_TYPE)) {
		if (diff_ap->api == api) {
			return diff_ap;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_ap)
		, cm_null_user_id, sizeof (*diff_ap), channel->sysptr
		);
	if (is_null (diff_ap)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_ap->api = api;
	CmListInitialize (& diff_ap->module_list);

	CmListInsertTail (head, & diff_ap->link);
	return diff_ap;
}


/*----------------------------------------------------------------------------*/
CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_module_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT16  slot_nr,
	LSA_UINT32  mod_ident
	)
{
	CM_LOG_FCT ("cm_pd_diff_ap_find_append")
	CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  diff_mod;

	for (CmListForeach (diff_mod, head, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE)) {
		if (diff_mod->slot_nr == slot_nr) {
			return diff_mod;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_mod)
		, cm_null_user_id, sizeof (*diff_mod), channel->sysptr
		);
	if (is_null (diff_mod)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_mod->slot_nr = slot_nr;
	diff_mod->mod_ident = mod_ident;

	CmListInitialize (& diff_mod->submodule_list);

	CmListInsertTail (head, & diff_mod->link);
	return diff_mod;
}

/*----------------------------------------------------------------------------*/
CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_submodule_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  sub_state,
	LSA_UINT32  sub_ident
	)
{
	CM_LOG_FCT ("cm_pd_diff_submodule_find_append")
	CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

	for (CmListForeach (diff_sub, head, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE)) {
		if (diff_sub->subslot_nr == subslot_nr) {
			return diff_sub;
		}
	}

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& diff_sub)
		, cm_null_user_id, sizeof (*diff_sub), channel->sysptr
		);
	if (is_null (diff_sub)) {
		CM_FATAL ();
		return LSA_NULL;
	}

	diff_sub->subslot_nr = subslot_nr;
	diff_sub->sub_state = sub_state;
	diff_sub->sub_ident = sub_ident;

	CmListInsertTail (head, & diff_sub->link);
	return diff_sub;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diff_list_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *head
	)
{
	CM_LOG_FCT ("cm_pd_diff_list_free")
	CM_UPPER_PD_DIFF_AP_PTR_TYPE  diff_ap;
	CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  diff_mod;
	CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  diff_sub;
	LSA_UINT16  rc;

	while (! CmListIsEmpty (head)) {
		CmListRemoveHead (head, diff_ap, CM_UPPER_PD_DIFF_AP_PTR_TYPE);

		while (! CmListIsEmpty (& diff_ap->module_list)) {
			CmListRemoveHead (& diff_ap->module_list, diff_mod, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE);

			while (! CmListIsEmpty (& diff_mod->submodule_list)) {
				CmListRemoveHead (& diff_mod->submodule_list, diff_sub, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE);

				CM_FREE_UPPER_MEM (& rc, diff_sub, channel->sysptr);
				CM_ASSERT (rc == LSA_RET_OK);
			}

			CM_FREE_UPPER_MEM (& rc, diff_mod, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_UPPER_MEM (& rc, diff_ap, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
	}
}


/*------------------------------------------------------------------------------
//	ModuleDiffBlock ::=
//		BlockHeader
//		, NumberOfAPIs, (API
//		, NumberOfModules, (SlotNumber, ModuleIdentNumber a, ModuleState b
//		, NumberOfSubmodules, [(SubslotNumber, SubmoduleIdentNumber a, SubmoduleState)*])*)*
//
//	a Real identification data
//	b If ModuleState=NO_MODULE then NumberOfSubmodules shall be zero. The subsequent
//	part shall be omitted. For all other ModuleState only the SubmoduleState shall be used to
//	decide whether the submodule shall be parametrized.
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_difflist_to_moddiffblock (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  ar_graph, /* may be LSA_NULL */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *diff_list,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mod_diff_block_length,
	CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR  *mod_diff_block
	)
{
	CM_LOG_FCT("cm_pd_difflist_to_moddiffblock")
	LSA_UINT16  state;
	CM_UPPER_MEM_U8_PTR_TYPE  dp = LSA_NULL; /* = LSA_NULL because of warning C4701: potentially uninitialized local variable 'dp' used */
	LSA_UINT16  cm_put_assert_line;
	LSA_UINT32  len = 0; /* = 0 because of warning C4701: potentially uninitialized local variable 'len' used */
	LSA_UINT32  left;
	LSA_UINT16  ap_count;

	*mod_diff_block_length = 0;
	*mod_diff_block = LSA_NULL;

	CmListLength (diff_list, & ap_count);
	if (ap_count == 0)
	{
		return;
	}

	for (state = 0; state <= 1; ++state)
	{
		LSA_BOOL  do_calc_len;

		CM_UPPER_PD_DIFF_AP_PTR_TYPE  ap;

		if (state == 0)
		{
			do_calc_len = LSA_TRUE;
			len = 0;
			left = 0;
		}
		else
		{
			do_calc_len = LSA_FALSE;

			if (len > 0xFFFF) { CM_FATAL(); }

			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& dp)
				, cm_null_user_id, (LSA_UINT16)len, channel->sysptr
				);
			if (is_null (dp)) {
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot alloc upper mem, len(%u)", len);
				CM_FATAL();
				return;
			}
			*mod_diff_block_length = (LSA_UINT16)len;
			*mod_diff_block = dp;

			left = len;
		}

		if (do_calc_len)
		{
			len += sizeof (CM_BLK_HEADER);
			len += sizeof (CM_RSP_DIFF);
		}
		else
		{
			cm_put_assert (left >= sizeof (CM_BLK_HEADER));

			/* BlockLength ::= This field shall contain the number of octets without counting the fields BlockType and BlockLength. */

			cm_put_u16 (dp, CM_BLK_HEADER, type, CM_BLOCK_TYPE_MOD_DIFF);
			cm_put_u16 (dp, CM_BLK_HEADER, length, CM_CAST_U16(len - 2/*BlockType*/ - 2/*BlockLength*/));
			cm_put_u16 (dp, CM_BLK_HEADER, version, 0x0100);

			cm_adjust  (dp, left, sizeof (CM_BLK_HEADER));


			cm_put_assert (left >= sizeof (CM_RSP_DIFF));

			cm_put_u16 (dp, CM_RSP_DIFF, number_of_apis, ap_count);

			cm_adjust  (dp, left, sizeof (CM_RSP_DIFF));
		}

		for (CmListForeach (ap, diff_list, CM_UPPER_PD_DIFF_AP_PTR_TYPE))
		{
			CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  mod;
			LSA_UINT16  mod_count;

			CmListLength (& ap->module_list, & mod_count);

			if (do_calc_len)
			{
				len += sizeof (CM_RSP_API);
			}
			else
			{
				cm_put_assert (left >= sizeof (CM_RSP_API));

				cm_put_u32 (dp, CM_RSP_API, api,               ap->api);
				cm_put_u16 (dp, CM_RSP_API, number_of_modules, mod_count);

				cm_adjust  (dp, left, sizeof (CM_RSP_API));
			}

			for (CmListForeach (mod, & ap->module_list, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE))
			{
				CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  sub;
				LSA_UINT16  sub_count;
				LSA_UINT16  mod_state;

				if (is_null (ar_graph)) /* PDEV */
				{
					mod_state = CM_MOD_STATE_PROPER_MODULE;
					CmListLength (& mod->submodule_list, & sub_count);
				}
				else
				{
#if CM_CFG_MAX_CLIENTS
					CM_AR_GRAPH_MODULE_PTR_TYPE  ag_mod;
					LSA_UINT16  no_sub_count, ag_sub_count;

					no_sub_count = 0;
					for (CmListForeach (sub, & mod->submodule_list, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE))
					{
						if ((sub->sub_state & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_NO_SUBMODULE)
						{
							no_sub_count += 1;
						}
					}

					cm_ar_graph_find_api_slot_subslot (ar_graph
						, ap->api, mod->slot_nr, 0, LSA_NULL, & ag_mod, LSA_NULL
						);
					CM_ASSERT (is_not_null (ag_mod));

					CmListLength (& ag_mod->submod_list, & ag_sub_count);

					if (ag_sub_count == no_sub_count) /* means: all submodule have IDENT_NO_SUBMODULE */
					{
						sub_count = 0;
						mod_state = CM_MOD_STATE_NO_MODULE;
					}
					else
					{
						CmListLength (& mod->submodule_list, & sub_count); /* must report all submodule */

						mod_state = CM_MOD_STATE_PROPER_MODULE;
						if (ag_mod->soll.mod_ident != mod->mod_ident)
						{
							mod_state = CM_MOD_STATE_SUBSTITUTED_MODULE;
						}
					}
#else
					mod_state = CM_MOD_STATE_PROPER_MODULE;
					CmListLength (& mod->submodule_list, & sub_count);
					CM_FATAL();
#endif
				}

				if (do_calc_len)
				{
					len += sizeof (CM_RSP_MOD);
				}
				else
				{
					cm_put_assert (left >= sizeof (CM_RSP_MOD));

					cm_put_u16 (dp, CM_RSP_MOD, slot_nr,				mod->slot_nr);
					cm_put_u32 (dp, CM_RSP_MOD, mod_ident,				mod->mod_ident);
					cm_put_u16 (dp, CM_RSP_MOD, mod_state,				mod_state);
					cm_put_u16 (dp, CM_RSP_MOD, number_of_submodules,	sub_count);

					cm_adjust  (dp, left, sizeof (CM_RSP_MOD));
				}

				if (sub_count > 0)
				{
					for (CmListForeach (sub, & mod->submodule_list, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE))
					{
						if (do_calc_len)
						{
							len += sizeof (CM_RSP_SUB);
						}
						else
						{
							cm_put_assert (left >= sizeof (CM_RSP_SUB));

							cm_put_u16 (dp, CM_RSP_SUB, subslot_nr, sub->subslot_nr);
							cm_put_u32 (dp, CM_RSP_SUB, sub_ident,  sub->sub_ident);
							cm_put_u16 (dp, CM_RSP_SUB, sub_state,  sub->sub_state);

							cm_adjust  (dp, left, sizeof (CM_RSP_SUB));
						}
					}
				}
			}
		}
	}
	return;

cm_put_assert_failed:
	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
		, "write of module_diff_block failed, line(%u), len(%u) left(%u)"
		, cm_put_assert_line, len, left
		);
	CM_FATAL1 (cm_put_assert_line);
}


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_for_appl_ready( /* AP01427283 */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *diag_block_length,
	CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR  *diag_block
) {
	CM_LOG_FCT("cm_pd_diag_appl_ready")
	CM_UPPER_MEM_PTR_TYPE mem = LSA_NULL;
	LSA_UINT32 len = 0;
	LSA_UINT32 offs = 0;

	/* compare to cm_pd_diag_move_to_user_diag() and cm_pd_prm_read_user() */

	{
	LSA_UINT16  port_id;

	for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id ) {

		CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn(channel, port_id);

		if( cm_pd_port_is_mapped(port) ) {

			LSA_UINT32 tmp = 0;
			CM_PD_DIAG_PTR_TYPE diag;

			for( CmListForeach(diag, & port->diag.diag_list, CM_PD_DIAG_PTR_TYPE) ) {

				tmp += 2+2+2 + 2+4; /* ExtChannelDiagnosisData */
			}

			if( tmp != 0 ) { /* port has diagnoses */

				len += 2+2+1+1 + 4+2+2 + 2+2+2 + tmp; /* block-header, diagnosis-header, and diagnoses */
			}
		}
	}
	}

	if( len != 0 ) {

		if( len > 0xFFFF ) { /* see cast below */

			CM_FATAL();
		}
		else {

			CM_ALLOC_UPPER_MEM(&mem, cm_null_user_id, (LSA_UINT16)len, channel->sysptr);

			if( is_null(mem) ) {

				CM_FATAL();
			}
			else {

				LSA_UINT16 port_id;

				for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id ) {

					CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn(channel, port_id);

					if( cm_pd_port_is_mapped (port) ) {

						LSA_BOOL ok = cm_pd_diag_marshal_8000(
							port->addr.slot_nr, port->addr.subslot_nr,
							(CM_COMMON_MEM_U8_PTR_TYPE)mem, len, &offs,
							&port->diag.diag_list,
							0x800C
							);

						if( ! ok ) {
							CM_FATAL(); /* a bug in length calculation */
						}
					}
				}
			}
		}
	}

	CM_ASSERT(offs == len);

	*diag_block_length = (LSA_UINT16)offs;
	*diag_block = mem;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
