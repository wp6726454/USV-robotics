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
/*  F i l e               &F: cm_pd2.c                                  :F&  */
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

#define LTRC_ACT_MODUL_ID  820
#define CM_MODULE_ID       820

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/
/*=== DEFINES AND FORWARDS ==================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple_next (
	CM_CHANNEL_PTR_TYPE  channel
);

static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_check_ownership (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

static  LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_record_index_classify_adapt ( /* TIA 1167600 */
	LSA_UINT32  cls,
	CM_PD_PORT_PTR_TYPE  port
);

/*===========================================================================*/
/*=== PORT FUNCTIONS ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_init (
	CM_PD_PORT_PTR_TYPE port
) {
	CM_LOG_FCT("cm_pd_port_init")

	port->addr.slot_nr = 0;
	port->addr.subslot_nr = 0;	/* format: 0x8ipp */
	port->addr.mod_ident = 0;
	port->addr.sub_ident = 0;

	port->diag.update_allowed = LSA_TRUE;

	CmListInitialize (& port->diag.diag_list);
	CmListInitialize (& port->diag.diag_list_server);
	CmListInitialize (& port->diag.diag_list_user);

	port->misc.sync_led_info           = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;
	port->misc.sync_led__is_rate_valid = LSA_FALSE;
	port->misc.sync_led__rcv_sync_prio = 0;
	port->misc.sync_led__local_prio    = 0;
	port->misc.sync_led__pll_state     = CM_PD_PLL_STATE_UNKNOWN;

	port->misc.topo_state = CM_PD_TOPO_STATE_UNKNOWN;
	port->misc.MRPRingPort = EDD_MRP_NO_RING_PORT;

	port->misc.link_status_ind = LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_undo_init (
	CM_PD_PORT_PTR_TYPE port
) {
	CM_LOG_FCT("cm_pd_port_undo_init")

	while (! CmListIsEmpty (& port->diag.diag_list))
	{
		CM_PD_DIAG_PTR_TYPE diag = CmListFirst (& port->diag.diag_list, CM_PD_DIAG_PTR_TYPE);
		cm_pd_diag_update_remove (& port->diag.diag_list, diag);
	}

	while (! CmListIsEmpty (& port->diag.diag_list_server))
	{
		CM_PD_DIAG_PTR_TYPE diag = CmListFirst (& port->diag.diag_list_server, CM_PD_DIAG_PTR_TYPE);
		cm_pd_diag_update_remove (& port->diag.diag_list_server, diag);
	}
	while (! CmListIsEmpty (& port->diag.diag_list_user))
	{
		CM_PD_DIAG_PTR_TYPE diag = CmListFirst (& port->diag.diag_list_user, CM_PD_DIAG_PTR_TYPE);
		cm_pd_diag_update_remove (& port->diag.diag_list_user, diag);
	}

	CM_ASSERT (is_null (port->misc.link_status_ind));
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_IF_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_if__nn (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	return & channel->usr.pd.port_if;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_id (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id
	)
{
	if (port_id <= channel->usr.pd.port_count)
	{
		CM_PD_PORT_PTR_TYPE  port;
		if (port_id == 0)
		{
			port = & channel->usr.pd.port_if.port;
		}
		else
		{
			port = & channel->usr.pd.port_array_ptr[port_id]; //MP [port_id - 1]
		}
		return port;
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_id__nn (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id
) {
	CM_LOG_FCT("cm_pd_port_from_id__nn")

	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id (channel, port_id);
	if (is_null (port))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "port(%u) out of range", port_id
			);
		CM_FATAL(); /* should not happen */
		return LSA_NULL; /* not reached */
	}
	return port;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_addr (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_LOG_FCT("cm_pd_port_from_addr")
	LSA_UINT16  i;

	for (i = 0; i <= channel->usr.pd.port_count; ++i)
	{
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id (channel, i);

		if (port->addr.slot_nr == slot_nr  &&  port->addr.subslot_nr == subslot_nr)
		{
			return port;
		}
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "addr-mapping not found, slot(%u) subslot(0x%x)"
		, slot_nr, subslot_nr
		);
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_port_is_mapped (
	CM_PD_PORT_PTR_TYPE  port
) {
	if (port->addr.subslot_nr != 0)
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_set_send_clock_allowed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  send_clock
) {
	if ( cm_edd_can_set_send_clock (channel) )
	{
		return LSA_TRUE;
	}
	else if ( channel->usr.pd.clock.send_clock == send_clock )
	{
		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}


/*===========================================================================*/
/*=== PD-PRM callback-functions "cm_cmpd_prm_xxx" ===========================*/
/*===========================================================================*/


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cmpd_prm_write (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_PD_PORT_PTR_TYPE  port,
	LSA_UINT32  record_index,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data
) {
	CM_LOG_FCT("cm_cmpd_prm_write")
	CM_PD_RECLIST_PTR_TYPE  reclist = & pdprm->channel->usr.pd.reclist;

	CM_ASSERT (is_not_null (port));

	if (is_null (record_data)) {
		return CM_ERR_PARAM; /* should not happen */
	}

	switch (record_index)
	{
	case 0x802C: /* PDIRData */
		if (port->port_id == 0)
		{
			reclist->prm_data.do_irt = LSA_TRUE; /* note: needed because "at least one port is an irt-port" is signalled delayed by link.ind */
			reclist->prm_data.ir_data_uuid = cm_uuid_nil_uuid;

			if (record_data_length > 0UL + 2+2+1+1 +1+1 +2+2 + (2+2+1+1 +1+1 +16)) /* more check is done by subcomponent */
			{
				LSA_UINT16 block_version_ir, block_version_global;
				/*	PDIRData[VersionLow == 1]  ::=
				//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, ...
				//
				//	PDIRGlobalData[VersionLow == 1 or VersionLow == 2]  ::=
				//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, ...
				*/
				block_version_ir		= CM_GET16_NTOH (record_data,  4);
				block_version_global	= CM_GET16_NTOH (record_data, 16);
				if (block_version_ir == 0x0101
				&& (block_version_global == 0x0101 || block_version_global == 0x0102))
				{
					CM_GET_UUID_NTOH (& reclist->prm_data.ir_data_uuid, record_data, 20);
				}
			}
			else
			{
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "record_data_length(%u)", record_data_length);
				return CM_ERR_PRM_DATA;
			}
		}
		break;

	case 0x00010000: /* SendClock */
		if (port->port_id != 0)
		{
			return CM_ERR_PRM_PORTID;
		}
		else
		{
			LSA_UINT16 block_type, block_length, block_version, send_clock, sendclock_prop,	reserved;

				/* SendClock ::= BlockHeader, SendClockFactor, SendClockProperties, Reserved, [Data*] */
			if( record_data_length < 0UL + 2+2+1+1 + 2+2+2 ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "record_data_length(%u)", record_data_length);
				return CM_ERR_PRM_DATA;
			}

			block_type		= CM_GET16_NTOH (record_data, 0);
			block_length	= CM_GET16_NTOH (record_data, 2);
			block_version	= CM_GET16_NTOH (record_data, 4);
			send_clock		= CM_GET16_NTOH (record_data, 6);
			sendclock_prop	= CM_GET16_NTOH (record_data, 8);
			reserved		= CM_GET16_NTOH (record_data, 10);

			if( 0UL + 2+2 + block_length != record_data_length ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_length(%u)", block_length);
				return CM_ERR_PRM_DATA;
			}
			if( block_type != CM_BLOCK_TYPE_SEND_CLOCK ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_type(0x%x)", block_type);
				return CM_ERR_PRM_BLOCK;
			}
			if( block_version != 0x0100 ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_version(0x%x)", block_version);
				return CM_ERR_PRM_BLOCK;
			}
			switch (sendclock_prop) {
			case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
			case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
			case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
			case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
				/* we don't check for cm_edd_can_set_send_clock() here or else the user has to know which EDD is used */
				/* see the "correction" in cm_pd_sendclock_prop_sc_is_fixed() */
				break;

			default:
				CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unknown sendclock_prop(0x%x)", sendclock_prop
					);
				return CM_ERR_PRM_BLOCK;
			}

			if( reserved != 0x0000 ) {
				CM_PD_TRACE_00(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "reserved not zero");
				return CM_ERR_PRM_BLOCK;
			}

			{
			LSA_BOOL is_power_of_two = LSA_FALSE; /* sanity, valid only if returns true */

			if( ! cm_edd_send_clock_range_ok(pdprm->channel, send_clock, &is_power_of_two) ) {

				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unsupported sendclock(%u)"
					, send_clock
					);
				return CM_ERR_PRM_BLOCK;
			}
			}

			if ( ! cm_pd_set_send_clock_allowed (pdprm->channel, send_clock) ) /* TIA 1152436 */
			{
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "setting sendclock(%u) not allowed"
					, send_clock
					);
				return CM_ERR_PRM_BLOCK;
			}

			/* note: may be written several times in one prm-phase (semantics of RPC idempotent-rerun) */

			reclist->prm_data.scr_send_clock     = send_clock;
			reclist->prm_data.scr_sendclock_prop = sendclock_prop;
		}
		break;

	case 0x00011020: /* PDSyncPLL */
		if (port->port_id == 0)
		{
			/* PDSyncPLL v1.1 ::= BlockHeader, ReductionRatio(u16), PLLHWDelay(u16), ControlInterval(u16), PLLWindow(u32), SetWaitCount(u16), Reserved(u16) */
			if (record_data_length == 0UL + 2+2+1+1 +2+2+2+4+2+2) /* more check is done by subcomponent */
			{
				reclist->prm_data.pll_reduction_ratio = CM_GET16_NTOH (record_data, 6);
				reclist->prm_data.pll_hw_delay        = CM_GET16_NTOH (record_data, 8);
				reclist->prm_data.pll_cntrl_interval  = CM_GET16_NTOH (record_data,10);
				reclist->prm_data.pll_window          = CM_GET32_NTOH (record_data,12);
				reclist->prm_data.pll_setwaitcount    = CM_GET16_NTOH (record_data,16);
			}
		}
		break;

	default:
		/* not scanned */
		break;
	}

#if CM_CFG_USE_MRP
	{
	LSA_UINT32  record_cls = cm_record_index_classify (0/*PDev*/, record_index, CM_VENDOR_ID_SIEMENS/*!*/);

	if ((record_cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_MRP)
	{
		reclist->prm_data.do_mrp_record = LSA_TRUE;
	}
	}
#endif

	return CM_OK;
}


/*===========================================================================*/
/*=== PD-SendClock ==========================================================*/
/*===========================================================================*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_get (
	CM_CHANNEL_PTR_TYPE  channel
) {
	return channel->usr.pd.clock.sendclock_prop;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_sc_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_pd_sendclock_prop_sc_is_fixed")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_PD);

	/*
	 * note: cm_cmpd_prm_write() does not check against cm_edd_can_set_send_clock(channel)
	 */

	if( cm_edd_can_set_send_clock(channel) ) { /* use SendClockProperties */

		switch (channel->usr.pd.clock.sendclock_prop) {
		case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
		case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "send_clock(%u) is fixed", channel->usr.pd.clock.send_clock
				);
			return LSA_TRUE;

		case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
		case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
		default:
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "send_clock(%u) is variable", channel->usr.pd.clock.send_clock
				);
			return LSA_FALSE;
		}
	}
	else { /* no matter what the SendClockProperties say, if edd cannot...*/

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "send_clock(%u) is fixed (because edd cannot set send-clock)", channel->usr.pd.clock.send_clock
			);
		return LSA_TRUE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_rr_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_LOG_FCT("cm_pd_sendclock_prop_rr_is_fixed")

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_PD);

	switch (channel->usr.pd.clock.sendclock_prop) {
	case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
	case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "reduction_ratio is fixed"
			);
		return LSA_TRUE;

	case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
	case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
	default:
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "reduction_ratio is variable"
			);
		return LSA_FALSE;
	}
}


/*===========================================================================*/
/*=== iterator ==============================================================*/
/*===========================================================================*/

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_queue_iterator_next(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	CM_LOG_FCT("cm_pd_queue_iterator_next")
	CM_PD_ITER_PTR_TYPE iter = &channel->usr.pd.req.iter;

	/*
	 * NOTE: copy/paste/adapt from cm_sv_record_read_pdev_next()
	 */

	if( iter->pdev.state == CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	if( is_null(pd) ) { /* first call */

		pd = channel->usr.pd.req.current;

		if( iter->pdev.offs_tot != 0 ) {
			CM_FATAL(); /* see caller */
		}

		iter->pdev.offs_tot = 0;
	}
	else { /* confirmation (callback from cm-pd) */

		CM_ASSERT(are_equal(pd, channel->usr.pd.req.current));

		goto _confirmation;
	}

	/***/

	for( iter->port_id = 0; iter->port_id <= channel->usr.pd.port_count; ++iter->port_id ) {

		{ /* block for easier diffing */
			{ /* block for easier diffing */

				{
				CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, iter->port_id);

				if( ! cm_pd_port_is_mapped (port) ) { /* not mapped */
					continue;
				}

				iter->api = 0/*!*/;
				iter->slot_nr = port->addr.slot_nr;
				iter->subslot_nr = port->addr.subslot_nr;
				}

				for(;;) { /* for each block */

					CM_PD_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "pdev-iter: ap/sl/su(%u/%u/%u) state(%u) offs/len(%u/%u)"
						, iter->api, iter->slot_nr, iter->subslot_nr
						, iter->pdev.state, iter->pdev.offs_tot, iter->pdev.org.record_data_length
						);

					{
					LSA_UINT32 pnio_err = CM_PNIO_ERR_NONE; /* assume ok */
					LSA_BOOL   is_interface = LSA_FALSE;
					LSA_UINT16 index;

					switch( iter->pdev.state ) {

					/* PDevData (0xF831) */

					case CM_PD_ITER_PDEV_BlockHeaderPDev:
						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = iter->pdev.org.record_data;
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 > iter->pdev.org.record_data_length ) {

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_PD_PDEVDATA);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* length is written below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					case CM_PD_ITER_PDEV_IRData:
						is_interface = LSA_TRUE;
						index = 0x802C;
						break;

					case CM_PD_ITER_PDEV_PDSyncDataClock:
						is_interface = LSA_TRUE;
						index = 0x802D;
						break;

					case CM_PD_ITER_PDEV_PDSyncDataTime:
						is_interface = LSA_TRUE;
						index = 0x8031;
						break;

					case CM_PD_ITER_PDEV_PDIRSubframeData:
						is_interface = LSA_TRUE;
						index = 0x8020;
						break;

					/* Header for PDRealData and PDExpectedData */

					case CM_PD_ITER_PDEV_MultipleBlockHeaderReal:
					case CM_PD_ITER_PDEV_MultipleBlockHeaderExpected:
						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = iter->pdev.org.record_data;
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 + 4+2+2 > iter->pdev.org.record_data_length ) {

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_MULTIPLE_BLOCK_HEADER);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* patched below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						CM_PUT32_HTON(data, offs, iter->api);
						offs += 4;
						CM_PUT16_HTON(data, offs, iter->slot_nr);
						offs += 2;
						CM_PUT16_HTON(data, offs, iter->subslot_nr);
						offs += 2;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					/* PDRealData (0xF841) */

					case CM_PD_ITER_PDEV_PDPortDataReal:
						index = 0x802A;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataReal:
						is_interface = LSA_TRUE;
						index = 0x8050;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataReal:
						index = 0x8054;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataReal:
						index = 0x8060; /* see 0xF841-patch below! using 0x8060 here because of cm_record_index_classify() */
						break;
					case CM_PD_ITER_PDEV_PDInterfaceDataReal:
						is_interface = LSA_TRUE;
						index = 0x8080;
						break;
					case CM_PD_ITER_PDEV_PDPortStatistic:
						index = 0x8072;
						break;

					/* PDExpectedData (0xF842) */

					case CM_PD_ITER_PDEV_PDPortDataCheck:
						index = 0x802B;
						break;
					case CM_PD_ITER_PDEV_PDPortDataAdjust:
						index = 0x802F;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataAdjust:
						is_interface = LSA_TRUE;
						index = 0x8052;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataCheck:
						is_interface = LSA_TRUE;
						index = 0x8051;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataAdjust:
						index = 0x8053;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataAdjust:
						index = 0x8062;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataCheck:
						index = 0x8061;
						break;
					case CM_PD_ITER_PDEV_PDNCDataCheck:
						is_interface = LSA_TRUE;
						index = 0x8070;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceFSUDataAdjust:
						is_interface = LSA_TRUE;
						index = 0x8090;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceAdjust:
						is_interface = LSA_TRUE;
						index = 0x8071;
						break;

					/***/

					default:
						CM_FATAL();
						return 0;
					}

					/***/

					if( pnio_err == CM_PNIO_ERR_NONE ) {

						LSA_UINT32 cls;

						/* AP00431325, avoid reads that will fail anyway */

						if( (iter->subslot_nr & 0x00FF) == 0x0000 /* is an interface-submodule */ ) { /* see cm_pdsv_range_ok() */

							if( index == 0x8072 ) {

								/* PDPortStatistic is for interface too */
							}
							else if( ! is_interface ) {

								goto _next;
							}
						}
						else { /* is a port-submodule */

							if( is_interface ) {

								goto _next;
							}
						}

						/***/

						cls = cm_record_index_classify(0/*!*/, index, CM_VENDOR_ID_DONTCARE);

						if( index == 0x8060 ) { /* special handling for POF */

							index = 0xF841; /* don't include manufacturer-specific info in PDRealData */
						}

						/***/

						{
						CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

						prm_rdwr->slot_nr = iter->slot_nr;
						prm_rdwr->subslot_nr = iter->subslot_nr;

						prm_rdwr->record_index = index;

						prm_rdwr->record_data = iter->pdev.org.record_data + iter->pdev.offs_tot;
						prm_rdwr->record_data_length = iter->pdev.org.record_data_length - iter->pdev.offs_tot;

						prm_rdwr->is_local = LSA_TRUE;
						prm_rdwr->cls = cls;

						/***/

						if ((cls & CM_REC_IDX_READ) != CM_REC_IDX_READ) {

							goto _next; /* e.g. POF not configured */
						}
						else {

							LSA_UINT16 rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, pd, iter->port_id);

							if (rc != CM_OK_PENDING) {

								goto _confirmation;
							}
						}
						}
					}

					if( pnio_err != CM_PNIO_ERR_NONE ) { /* abort iteration */

						iter->pdev.state = CM_PD_ITER_PDEV_IDLE;
					}

					return pnio_err; /* CM_PNIO_ERR_NONE .. wait for confirmation */
					}

_confirmation: /* confirmation is back from cm-pd */

					if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

						CM_PD_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "reading index(0x%x) from sl/su(%u/%u) failed, pd-rsp(%u)"
							, pd->args.pd.prm_rdwr->record_index
							, pd->args.pd.prm_rdwr->slot_nr
							, pd->args.pd.prm_rdwr->subslot_nr
							, CM_RQB_GET_RESPONSE(pd)
							);

						if( CM_RQB_GET_RESPONSE(pd) == CM_ERR_PRM_DATA ) { /* data-length too short */

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* abort iteration */

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
						}
						else {

							/* ignored (e.g. index cannot be read from a port, ...) */
						}
					}
					else {

						iter->pdev.offs_tot += pd->args.pd.prm_rdwr->record_data_length;

						if( iter->pdev.offs_tot > iter->pdev.org.record_data_length ) {
							CM_FATAL(); /* a bug */
						}
					}

					/***/

_next:
					iter->pdev.state += 1; /* next */

					switch( iter->pdev.state ) {

					case CM_PD_ITER_PDEV_PDevDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_BlockHeaderPDev; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDRealDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderReal; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDExpectedDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderExpected; /* setup for next subslot */
						break;

					default:
						continue; /* read next index */
					}

					break; /* forever-loop */
				}
			}
		}
	}

	/***/

	iter->pdev.state = CM_PD_ITER_PDEV_IDLE;

	iter->pdev.org.record_data_length = iter->pdev.offs_tot; /* update */

	*channel->usr.pd.req.current->args.pd.prm_rdwr = iter->pdev.org; /* restore */

	/***/

	CM_ASSERT (are_equal (pd, channel->usr.pd.req.current));

	cm_pd_req_callback (channel, CM_OK, pd);

	return CM_PNIO_ERR_NONE; /* completed */
}

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_queue_iterator_first(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb,
	LSA_UINT16  first_state
) {
	CM_PD_ITER_PTR_TYPE iter = &channel->usr.pd.req.iter;
	LSA_UINT32 pnio_err;

	if( iter->pdev.state != CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	iter->pdev.state = first_state;

	iter->pdev.org = *rb->args.pd.prm_rdwr; /* save original request */

	iter->pdev.offs_tot = 0; /* sanity */

	pnio_err = cm_pd_queue_iterator_next(channel, LSA_NULL);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		LSA_UINT16 response;

		if( iter->pdev.state != CM_PD_ITER_PDEV_IDLE ) {
			CM_FATAL();
		}

		iter->pdev.org.record_data_length = 0; /* update */

		*channel->usr.pd.req.current->args.pd.prm_rdwr = iter->pdev.org; /* restore */

		/***/

		if( (pnio_err & 0xFFFFFF00) == CM_PNIO_FAULTY_Record__BufferTooSmall(0) ) {

			response = CM_ERR_PRM_DATA;
		}
		else {

			response = CM_ERR_PRM_OTHER;
		}

		/***/

		CM_ASSERT (are_equal (rb, channel->usr.pd.req.current));

		cm_pd_req_callback (channel, response, rb);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_prm_read_internal")
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;
	
	if (is_null (port))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
			);
		cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
		return;
	}

	if (prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT))
	{	
		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}
	}

	prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

	if ((prm_rdwr->cls & CM_REC_IDX_READ) != CM_REC_IDX_READ)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PD-read: index(0x%x) not readable, response(%u)"
			, prm_rdwr->record_index, CM_ERR_PRM_INDEX
			);
		cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		return;
	}

	{
		LSA_UINT16  rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, rb, port->port_id);

		if (rc != CM_OK_PENDING)
		{
			cm_pd_req_callback (channel, rc, rb);
			return;
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_prm_read_user")
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	if( channel->usr.pd.req.iter.pdev.state != CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	if (is_null (prm_rdwr->record_data))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-read: record_data == NULL");
		cm_pd_req_callback (channel, CM_ERR_PARAM, rb);
		return;
	}

	if ((prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT)) != 0)
	{
		if (is_null (port))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
				);
			cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
			return;
		}

		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}
	}

	if ((prm_rdwr->cls & CM_REC_IDX_READ) != CM_REC_IDX_READ)
	{
		if (! (prm_rdwr->is_local && (prm_rdwr->cls & CM_REC_IDX_READ_LOCAL) != 0))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "PD-read: index(0x%x) not readable, response(%u)"
				, prm_rdwr->record_index, CM_ERR_PRM_INDEX
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
			return;
		}
	}

	if (prm_rdwr->cls & CM_REC_IDX_DIAG)
	{
		if (prm_rdwr->cls & CM_REC_IDX_8000)
		{
			if (is_null (port))
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
					);
				cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
				return;
			}

			{
			CM_CHANNEL_PTR_TYPE sv_channel;

			CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_PD);

			sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);

#if CM_CFG_MAX_SERVERS
			if( is_not_null(sv_channel) ) {
				if( ! cm_sv_pdev_is_plugged(sv_channel) ) {
					sv_channel = LSA_NULL;
				}
			}
/*
 * note:
 *	port->addr.occupant == CM_PD_OCCUMPAND_SV doesn't work, because it is set to _NOBODY
 *	within callback from prm_prepare / prm_end and relinquish
 */

#endif

			if( is_not_null(sv_channel) ) {

#if CM_CFG_MAX_SERVERS
				{
				LSA_UINT32 offs = 0;
				LSA_UINT32 pnio_err;

				pnio_err = cm_sv_pdev_diag_read(
					sv_channel,
					prm_rdwr->slot_nr,
					prm_rdwr->subslot_nr,
					CM_CAST_U16(prm_rdwr->record_index),
					prm_rdwr->record_data, prm_rdwr->record_data_length, &offs
					);

				if( pnio_err != CM_PNIO_ERR_NONE ) {
					CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
						"PD-read: cm_sv_pdev_diag_read returned pnio_err(%x)", pnio_err
						);
					prm_rdwr->cm_pnio_err = pnio_err;
					prm_rdwr->record_data_length = 0;
					cm_pd_req_callback (channel, CM_OK/*!*/, rb);
				}
				else {
					prm_rdwr->record_data_length = offs; /* update */
					cm_pd_req_callback (channel, CM_OK, rb);
				}
				}
#else
				CM_FATAL();
#endif
			}
			else {

				LSA_UINT32 offs = 0;
				LSA_BOOL ok;

				ok = cm_pd_diag_marshal_8000(
					prm_rdwr->slot_nr, prm_rdwr->subslot_nr,
					prm_rdwr->record_data, prm_rdwr->record_data_length, &offs,
					&port->diag.diag_list,
					CM_CAST_U16(prm_rdwr->record_index)
					);

				if( ! ok ) {
					cm_pd_req_callback (channel, CM_ERR_PRM_DATA, rb);
				}
				else {
					prm_rdwr->record_data_length = offs; /* update */
					cm_pd_req_callback (channel, CM_OK, rb);
				}
			}
			}
		}
		else if (prm_rdwr->cls & CM_REC_IDX_C000)
		{
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("PD-TODO: PrmRead 0xCxxxx for version XYZ")
#endif /* CM_MESSAGE */
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		}
		else if ((prm_rdwr->cls & CM_REC_IDX_F000) || (prm_rdwr->cls & CM_REC_IDX_F800))
		{
			CM_CHANNEL_PTR_TYPE sv_channel;

			CM_ASSERT(channel->detailptr->path_type == CM_PATH_TYPE_PD);

			sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);

#if CM_CFG_MAX_SERVERS
			if( is_not_null(sv_channel) ) {
				if( ! cm_sv_pdev_is_plugged(sv_channel) ) {
					sv_channel = LSA_NULL;
				}
			}
#endif

			if( is_not_null(sv_channel) ) {

#if CM_CFG_MAX_SERVERS
				{
				LSA_UINT32 offs = 0;
				LSA_UINT32 pnio_err;

				pnio_err = cm_sv_pdev_diag_read(
					sv_channel,
					prm_rdwr->slot_nr, prm_rdwr->subslot_nr,
					CM_CAST_U16(prm_rdwr->record_index),
					prm_rdwr->record_data, prm_rdwr->record_data_length, &offs
					);

				if( pnio_err != CM_PNIO_ERR_NONE ) {
					CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
						"PD-read: cm_sv_pdev_diag_read returned pnio_err(%x)", pnio_err
						);
					prm_rdwr->cm_pnio_err = pnio_err;
					prm_rdwr->record_data_length = 0;
					cm_pd_req_callback (channel, CM_OK/*!*/, rb);
				}
				else {
					prm_rdwr->record_data_length = offs; /* update */
					cm_pd_req_callback (channel, CM_OK, rb);
				}
				}
#else
				CM_FATAL();
#endif
			}
			else {

				LSA_UINT32 offs = 0;
				LSA_BOOL ok = LSA_TRUE;
				LSA_UINT16 port_id;

				/*AP00383492*/
				for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id )
				{
					CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, port_id);

					if( cm_pd_port_is_mapped (port) ) /* is mapped */
					{
						ok = cm_pd_diag_marshal_8000 (port->addr.slot_nr, port->addr.subslot_nr
							, prm_rdwr->record_data, prm_rdwr->record_data_length
							, &offs, &port->diag.diag_list, CM_CAST_U16(prm_rdwr->record_index)
							);

						if( ! ok ) {
							break;
						}
					}
				}

				if( ! ok ) {
					cm_pd_req_callback (channel, CM_ERR_PRM_DATA, rb);
				}
				else {
					prm_rdwr->record_data_length = offs; /* update */
					cm_pd_req_callback (channel, CM_OK, rb);
				}
			}
		}
		else /* CM_REC_IDX_E000 */
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: index(0x%x) unknown to CMPD", prm_rdwr->record_index
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		}
	}
	else if (prm_rdwr->cls & CM_REC_IDX_PDEV)
	{
		if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_NONE)
		{
			CM_FATAL(); /* see see cm_record_index_classify() */
		}

		/*
		 * PD-records are read from ports
		 * with the exception of F8xx records
		 */

		switch(prm_rdwr->record_index) {

		case 0xF831: /* PDevData */
			cm_pd_queue_iterator_first(channel, rb, CM_PD_ITER_PDEV_BlockHeaderPDev);
			return;

		case 0xF841: /* PDRealData */
			cm_pd_queue_iterator_first(channel, rb, CM_PD_ITER_PDEV_MultipleBlockHeaderReal);
			return;

		case 0xF842: /* PDExpectedData */
			cm_pd_queue_iterator_first(channel, rb, CM_PD_ITER_PDEV_MultipleBlockHeaderExpected);
			return;

		default:
			/* another pdev-record */
			break;
		}

		if (is_null (port))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
				);
			cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
			return;
		}

		prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

		if ((prm_rdwr->cls & CM_REC_IDX_READ) != CM_REC_IDX_READ)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "PD-read: index(0x%x) not readable, response(%u) "
				, prm_rdwr->record_index, CM_ERR_PRM_INDEX
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
			return;
		}

		{
			LSA_UINT16  rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, rb, port->port_id);

			if (rc != CM_OK_PENDING)
			{
				cm_pd_req_callback (channel, rc, rb);
				return;
			}
		}
	}
	else
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-read: index(0x%x) unknown to CMPD", prm_rdwr->record_index
			);
		cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_begin_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_prm_begin_user")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	switch (cm_pd_prm_owner_get (pdprm))
	{
	case CM_PD_OCCUPANT_NOBODY:
		break;

	case CM_PD_OCCUPANT_USER:
	case CM_PD_OCCUPANT_SV:
	default:
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-begin failed, PDev already owned by (%u)", cm_pd_prm_owner_get (pdprm)
			);
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (CmListIsEmpty(& channel->res.eventQ))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-begin failed, no indication resources");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (! channel->usr.pd.reclist.is_valid)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-begin failed, reclist not valid");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (cm_pd_prm_ownership (pdprm, CM_PD_OCCUPANT_USER, 0, 0xFFFF/*special ar_nr*/) != CM_OK)
	{
		CM_FATAL();
	}

	cm_pd_reclist_prm_begin (channel, LSA_TRUE);

	cm_pd_req_callback (channel, CM_OK, rb);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_prm_write_user")
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	LSA_UINT16  rsp;

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	rsp = cm_pd_prm_write_check_ownership (channel, rb);

	if (rsp == CM_OK)
	{
		if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_BBUF) /* TIA 1095777 */
		{
			/* note: prm_rdwr->record_data may be LSA_NULL */

			rsp = cm_pd_reclist_delete (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index);
		}
		else
		{
			rsp = cm_pd_prm_write_internal (channel, rb);
		}

		if (rsp == CM_OK_PENDING)
		{
			return;
		}

		if (rsp != CM_OK  &&  (prm_rdwr->cls & CM_REC_IDX_NONPRM) == 0)
		{
			cm_pd_reclist_set_error (channel, prm_rdwr->record_index);
		}
	}

	cm_pd_req_callback (channel, rsp, rb);
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_check_ownership (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_prm_write_check_ownership")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	if ((prm_rdwr->cls & CM_REC_IDX_NONPRM) == 0) /* prm-records need ownership */
	{
		if (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_WRITE_SERVER)
		{
			if (cm_pd_prm_owner_get (pdprm) != CM_PD_OCCUPANT_SV)
			{
				return CM_ERR_NOT_ACCESSIBLE/*!*/;
			}
		}
		else /* CM_OPC_PD_PRM_WRITE */
		{
			if (cm_pd_prm_owner_get (pdprm) != CM_PD_OCCUPANT_USER)
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PD-write failed, occupant(%u) is invalid, record_index(0x%x), only NONPRM index allowed"
					, cm_pd_prm_owner_get (pdprm), prm_rdwr->record_index
					);
				return CM_ERR_SEQUENCE;
			}
		}

		if (channel->usr.pd.reclist.is_valid)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, reclist_valid incorrect, need PD-begin first"
				);
			return CM_ERR_SEQUENCE;
		}
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_record_index_classify_adapt ( /* TIA 1167600 */
	LSA_UINT32  cls,
	CM_PD_PORT_PTR_TYPE  port
	)
{
	CM_LOG_FCT("cm_pd_record_index_classify_adapt")

	/* see cm_record_index_classify() */

	switch (cls & CM_REC_IDX_CMPD_MASK)
	{
#if CM_CFG_USE_MRP
	case CM_REC_IDX_CMPD_MRP:
		if (port->misc.MRPRingPort == EDD_MRP_NO_RING_PORT)
		{
			cls &= ~(CM_REC_IDX_WRITE | CM_REC_IDX_WRITE_LOCAL | CM_REC_IDX_READ | CM_REC_IDX_READ_LOCAL);
		}
		return cls;	
#endif

	default:
		LSA_UNUSED_ARG (port);
		return cls;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_prm_write_internal")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	if (is_null (prm_rdwr->record_data))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-write failed, record_data == NULL"
			);
		return CM_ERR_PARAM;
	}

	if (is_null (port))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-write failed, port not mapped, slot(%u) subslot(0x%x)"
			, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
			);
		return CM_ERR_NOT_ACCESSIBLE/*!*/;
	}

	prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

	if (prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT))
	{	
		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) not writeable to interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_PORTID;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) not writeable to port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_PORTID;
		}
	}

	if ((prm_rdwr->cls & CM_REC_IDX_WRITE) != CM_REC_IDX_WRITE)
	{
		if (! (prm_rdwr->is_local && (prm_rdwr->cls & CM_REC_IDX_WRITE_LOCAL) != 0))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, wrong record_index(0x%x), cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_INDEX;
		}
	}

	if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_NONE)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-write failed, wrong record_index(0x%x), unknown subcomponend_id, cls(0x%x)"
			, prm_rdwr->record_index, prm_rdwr->cls
			);
		return CM_ERR_PRM_INDEX;
	}

	if (prm_rdwr->cls & CM_REC_IDX_NONPRM) /* write non prm directly */
	{
		if (prm_rdwr->record_index == 0xB080/*PDInstanceTailorData*/)
		{
#if CM_CFG_MAX_CLIENTS
			cm_pd_prm_write_PDInstanceTailorData (channel, rb);
#else
			return CM_ERR_PRM_INDEX;
#endif
		}
		else
		{
			cm_pd_prm_write_nonprm (pdprm, rb);
		}
		return CM_OK_PENDING;
	}

	if (channel->usr.pd.reclist.is_valid)
	{
		return CM_ERR_SEQUENCE; /* unreachable, see cm_pd_prm_write_check_ownership */
	}

	if (prm_rdwr->record_index == 0x00017081/*PDMasterTailorData*/)
	{
		if (is_not_null (channel->usr.pd.reclist.mtd)) /* mtd already created, keep only latest record, see cm_pd_reclist_write() */
		{
			cm_pd_mtd_free (& channel->usr.pd.reclist.mtd);
		}

		channel->usr.pd.reclist.mtd = cm_pd_mtd_parse (channel, 0/*PDev*/, prm_rdwr->record_data_length, prm_rdwr->record_data);
			
		if (is_null (channel->usr.pd.reclist.mtd))
		{
			return CM_ERR_PRM_DATA;
		}
		/* no return, keep all records, because of remanence */
	}

		/* sniff some data */
	if (cm_cmpd_prm_write (pdprm, port, prm_rdwr->record_index, prm_rdwr->record_data_length, prm_rdwr->record_data) != CM_OK)
	{
		return CM_ERR_PRM_DATA;
	}

	if (cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb) != CM_OK)
	{
		return CM_ERR_PRM_DATA;
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_nonprm_done (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
	) 
{
	cm_pd_req_callback (pdprm->channel, CM_RQB_GET_RESPONSE(rb), rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_prm_write_multiple")
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	if (is_null (prm_rdwr->record_data))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-write failed, record_data == NULL");
		cm_pd_req_callback (channel, CM_ERR_PARAM, rb);
		return;
	}


	/* startup multiple write */
	if (mwr->is_running) {
		CM_FATAL1 (mwr->data_offs);
	}

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;

	mwr->is_running			= LSA_TRUE;
	mwr->all_records_done	= LSA_FALSE;
	mwr->mk_remanent		= prm_rdwr->mk_remanent;
	mwr->is_local			= prm_rdwr->is_local;
	mwr->data				= prm_rdwr->record_data;
	mwr->data_offs			= 0;
	mwr->data_length		= prm_rdwr->record_data_length;
	mwr->rsp				= CM_OK;

	mwr->internal_rqb		= cm_upper_alloc_rqb (channel);

	if (is_null (mwr->internal_rqb))
	{
		mwr->rsp = CM_ERR_ALLOC_UPPER;

		mwr->is_running = LSA_FALSE;
		mwr->all_records_done = LSA_TRUE;
		cm_pd_req_callback (channel, mwr->rsp, rb);
		return;
	}

	CM_RQB_SET_OPCODE (mwr->internal_rqb, CM_OPC_PD_PRM_WRITE);

	cm_pd_prm_write_multiple_next (channel);
}

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_multi_write_parse (
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr
	)
{
	CM_LOG_FCT("cm_pd_multi_write_parse")
#define cm_pd_mwr_left(mwr_)  ((mwr_)->data_length - (mwr_)->data_offs)

	if (cm_pd_mwr_left(mwr) == 0)
	{
		CM_PD_TRACE_01 (0, LSA_TRACE_LEVEL_CHAT
			, "CMPD-MultiWrite: parsing data_length(%u) is finish", mwr->data_length
			);
		mwr->all_records_done = LSA_TRUE;
		return;
	}


	if (cm_pd_mwr_left(mwr) < 4+4+2+2+4+2+2)
	{
		CM_PD_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
			, "CMPD-MultiWrite: left(%u) is less then RemaWriteHeader(%u)"
			, cm_pd_mwr_left(mwr), 4+4+2+2+4+2+2
			);
		mwr->rsp = CM_ERR_PRM_DATA;
		mwr->all_records_done = LSA_TRUE;
		return;
	}

	{
	CM_UPPER_RQB_PTR_TYPE  rb = mwr->internal_rqb;
	CM_UPPER_MEM_U8_PTR_TYPE  dp = mwr->data + mwr->data_offs;
	LSA_UINT32  index, api, length;
	LSA_UINT16  slot, subslot;
	LSA_UINT32  padding;

	/*
	//	RemaWriteMultiple ::=
	//		(RemaWriteHeader, RemaRecordData, [Padding*] a)*
	//
	//		a) the number of padding octets shall be adapted to make the block Unsigned32 aligned.
	//
	//	RemaWriteHeader ::=
	//		RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16)
	//		, RemaRecordDataLength(u32), RemaCompID(u16), Reserved(u16)
	*/

	index		= CM_GET32_NTOH (dp, 0);
	api			= CM_GET32_NTOH (dp, 4);
	slot		= CM_GET16_NTOH (dp, 4+4);
	subslot		= CM_GET16_NTOH (dp, 4+4+2);
	length		= CM_GET32_NTOH (dp, 4+4+2+2);
/*
//	lsa_comp_id	= CM_GET16_NTOH (dp, 4+4+2+2+4);		// reserved with AP00916017
//	reserved	= CM_GET16_NTOH (dp, 4+4+2+2+4+2);		// reserved: set 0, do not check
*/
	mwr->data_offs += 4+4+2+2+4+2+2;

	dp = mwr->data + mwr->data_offs;

	if (length > cm_pd_mwr_left(mwr))
	{
		CM_PD_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
			, "CMPD-MultiWrite: not enough data for next record, current-offset(%u) wanted-length(%u) left(%u)"
			, mwr->data_offs, length, cm_pd_mwr_left(mwr)
			);
		mwr->rsp = CM_ERR_PRM_DATA;
		mwr->all_records_done = LSA_TRUE;
		return;
	}

	LSA_UNUSED_ARG							( api ); /* api for PDEV allways 0 */
	rb->args.pd.prm_rdwr->slot_nr			= slot;
	rb->args.pd.prm_rdwr->subslot_nr		= subslot;
	rb->args.pd.prm_rdwr->record_index		= index;
	rb->args.pd.prm_rdwr->record_data_length= length;
	rb->args.pd.prm_rdwr->record_data		= dp;

	rb->args.pd.prm_rdwr->mk_remanent		= mwr->mk_remanent;
	rb->args.pd.prm_rdwr->is_local			= mwr->is_local;

	rb->args.pd.prm_rdwr->cls				= cm_record_index_classify(0/*!*/, index, CM_VENDOR_ID_SIEMENS/*!*/);

	mwr->data_offs += length;

	padding = (4 - (mwr->data_offs % 4)) % 4;

	if (padding <= cm_pd_mwr_left(mwr))
	{
		mwr->data_offs += padding;
	}
	}
#undef cm_pd_mwr_left
}

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple_next (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_prm_write_multiple_next")
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;

	cm_pd_multi_write_parse (mwr);

	if (mwr->all_records_done)
	{
		cm_upper_free_rqb (channel, mwr->internal_rqb);

		if (mwr->rsp != CM_OK)
		{
			cm_pd_reclist_set_error (channel, 0xF8000000/*MultiplePDev*/);
		}

		mwr->is_running = LSA_FALSE;
		cm_pd_req_callback (channel, mwr->rsp, channel->usr.pd.req.current);
	}
	else
	{
		/* call single prm_write with internal rqb */
		cm_pd_prm_write_user (channel, mwr->internal_rqb);
	}
}

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_RESPONSE_TYPE  response
	)
{
	CM_LOG_FCT("cm_pd_prm_write_multiple_done")
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;

	if (response != CM_OK  &&  mwr->rsp == CM_OK)
	{
		mwr->rsp = response;
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "WriteMultiple, got first bad response(%u)", response
			);
	}

	/* one of the single write is done, check if the multiple block is done, else cnf write_multiple */

	cm_pd_prm_write_multiple_next (channel);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_end_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_prm_end_user")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_BOOL is_prm_server = LSA_FALSE;

	if (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_END_SERVER)
	{
		CM_UPPER_RQB_PTR_TYPE tmp = cm_pd_req_queue_remove_current (channel); /* see cm_pd_req_callback() */
		CM_ASSERT (are_equal (rb, tmp));
		LSA_UNUSED_ARG (tmp);
		
		is_prm_server = LSA_TRUE;

		if (cm_pd_prm_owner_get (pdprm) != CM_PD_OCCUPANT_SV)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-end error, owner(%u) is invalid", cm_pd_prm_owner_get (pdprm));
			cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
			return;
		}
	}
	if (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_END  &&  cm_pd_prm_owner_get (pdprm) != CM_PD_OCCUPANT_USER)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-end error, owner(%u) is invalid", cm_pd_prm_owner_get (pdprm));
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}
	if (channel->usr.pd.reclist.is_valid)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-end error, reclist_valid incorrect");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (is_not_null (channel->usr.pd.reclist.mtd))
	{
		LSA_UINT32 rec_index = cm_pd_reclist_validate_and_prealloc_tailoring_records (channel, channel->usr.pd.reclist.mtd);
		if (rec_index != 0)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PD-end failed, record with index(0x%x) failed", rec_index);
			cm_pd_reclist_set_error (channel, rec_index);
		}
	}

	if (cm_pd_reclist_add_default_rec (channel, pdprm, is_prm_server, rb->args.pd.prm_end->set_mrp_off) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}
	if (cm_pd_reclist_add_ep_rec (channel) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}

	if (is_prm_server)
	{
		/* if RQB::IRDataUUID == NIL ignore, else do not commit if not equal to PDIRGlobalData::IRDataUUID */

		CM_UPPER_PD_PRM_END_PTR_TYPE prm_end = rb->args.pd.prm_end;

		if (!cm_uuid_is_nil (&prm_end->ir_data_uuid))
		{
			CM_PD_RECLIST_PTR_TYPE  reclist = &pdprm->channel->usr.pd.reclist;

			if (!cm_uuid_equal (&prm_end->ir_data_uuid, &reclist->prm_data.ir_data_uuid))
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PD-end failed, IRDataUUID mismatch");
				cm_pd_reclist_set_error (channel, 0);
			}
		}

		if( prm_end->pdev_problem ) /* TIA 1103368 */
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PD-end failed, CM-SV pdev-problem");
			cm_pd_reclist_set_error (channel, 0);
		}
	}

	cm_pd_alarm_user_enable (pdprm->channel, LSA_FALSE);

	channel->usr.pd.reclist.is_valid = LSA_TRUE;

	rb->args.pd.prm_end->is_master_project = cm_pdusr_is_masterproject(channel);

	if (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_END)
	{
		cm_pd_req_callback (pdprm->channel, CM_OK, rb);

		cm_pd_config_start (channel, CM_PD_CONFIG_PRM_USR);
	}
	else if (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_END_SERVER)
	{
		CM_ASSERT (is_null (pdprm->end));
		pdprm->end = rb;

		cm_pd_config_start (channel, CM_PD_CONFIG_PRM_SV);
	}
	else
	{
		CM_FATAL();
	}
}


/*----------------------------------------------------------------------------*/
/* callback */
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_done (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg,
	LSA_UINT16  rsp
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & pd_channel->usr.pd.pdprm;

	switch (start_cfg)
	{
	case CM_PD_CONFIG_PRM_USR:
		/* finish ownership of pdprm */
		if (cm_pd_prm_owner_get (& pd_channel->usr.pd.pdprm) == CM_PD_OCCUPANT_USER)
		{
			cm_pd_prm_owner_set (& pd_channel->usr.pd.pdprm, CM_PD_OCCUPANT_NOBODY, 0, 0);
		}
		cm_pd_callback_appl_ready (pd_channel, rsp);
		break;

	case CM_PD_CONFIG_NOS_IP:
	case CM_PD_CONFIG_RESET_TO_FACTORY:
	case CM_PD_CONFIG_CL_DEVICE_ADD:
	case CM_PD_CONFIG_MACHINE_TAILOR:	
		cm_pd_callback_appl_ready (pd_channel, rsp);
		break;

	case CM_PD_CONFIG_PRM_SV:
		/* first: confirm SV */
		{
		CM_UPPER_RQB_PTR_TYPE  end = pdprm->end;
		CM_ASSERT(is_not_null(end));
		pdprm->end = LSA_NULL;

		cm_pd_req_callback (pd_channel, rsp, end);
		}

		/* second: inform user */
		cm_pd_callback_appl_ready (pd_channel, rsp);
		break;

	default:
		CM_FATAL();
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_callback_appl_ready (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_RESPONSE_TYPE rsp
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & pd_channel->usr.pd.pdprm;
	CM_UPPER_RQB_PTR_TYPE  appl_ready_ind = cm_event_get (pd_channel);

	if (is_null (appl_ready_ind))
	{
		pd_channel->usr.pd.almi.appl_ready_ind_pending = rsp; /* see cm_pd_opc_appl_ready_rsp */
		return;
	}

	CM_RQB_SET_OPCODE (appl_ready_ind, CM_OPC_PD_EVENT_APPL_READY_IND);
	cm_pd_prepare_appl_ready (pdprm, rsp, appl_ready_ind);

	cm_pd_req_callback (pdprm->channel, rsp, appl_ready_ind);

	if (rsp == CM_OK  ||  rsp == CM_OK_ADAPTION_NEEDED)
	{
		cm_pd_alarm_user_enable (pdprm->channel, LSA_TRUE);
	}
	else
	{
		cm_pd_alarm_user_enable (pdprm->channel, LSA_FALSE);
	}
}


/*===========================================================================*/
/*=== CM-SV PART ============================================================*/
/*===========================================================================*/
#if CM_CFG_MAX_SERVERS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_relinquish_server (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_LOG_FCT("cm_pd_prm_relinquish_server")

	channel->usr.pd.reclist.is_valid = LSA_TRUE;

	channel->usr.pd.almi.server_allowed = LSA_FALSE;
	channel->usr.pd.almi.server_max_alarm_data_length = 0; /* sanity */

	/* finish ownership of pdprm */
	cm_pd_prm_owner_set (& channel->usr.pd.pdprm, CM_PD_OCCUPANT_NOBODY, 0, 0);

	cm_pd_req_callback (channel, CM_OK, rb);
}

#endif


/*------------------------------------------------------------------------------
// marshal EXTENDED CHANNEL DIAGNOSIS
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_diag_marshal_8000(
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_COMMON_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 CM_COMMON_MEM_ATTR *offs_ptr,
	CM_LIST_ENTRY_PTR_TYPE ext_phys_diag_list,
	LSA_UINT16 index
) {
	CM_LOG_FCT("cm_pd_diag_marshal_8000")
	LSA_UINT32 offs;
	LSA_UINT32 ma = 4; /* always use block-version 1.1 */

	/*
	 * NOTE: copy/paste/adapt from cm_sv_record_marshal_diag()
	 *       what seems to be written in a strange way eases diff-ing!
	 */

	/***/

	offs = *offs_ptr;

	{ /* ap */
	LSA_UINT32 api = 0; /* see PNIO-Spec */
		{ /* slot */
			{ /* subslot */

				/*
				 * EXTENDED CHANNEL DIAGNOSIS
				 */

				{
				CM_ASSERT(is_not_null(ext_phys_diag_list));

				if( (/*iter->subslot->*/subslot_nr & 0xF000) == 0x8000 /* 0x8ipp is a pdev-submodule */ ) {

					if( CmListIsEmpty(ext_phys_diag_list) ) {

						ext_phys_diag_list = LSA_NULL;
					}
				}
				else {
					CM_FATAL(); /* a bug */
				}

				/***/

				if(
					is_not_null(ext_phys_diag_list)
				) {

					LSA_UINT32 offs_block;
					LSA_UINT32 offs_chpr_sum;

					/*
					 * ExtChannelDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber(0x8000), ChannelProperties a), UserStructureIdentifier(0x8002),
					 *   ExtChannelDiagnosisData *
					 *
					 * a) The field ChannelProperties.Type shall be set to zero.
					 *    The field ChannelProperties.Direction shall be set to zero.
					 *    The field ChannelProperties.Specifier shall be set to one if at least
					 *    one ChannelProperties.Specifier of the ChannelDiagnosisData is set to one.
					 */

					if( offs + 2+2+1+1 + ma+2+2 + 2+2+2 > size ) {
						CM_PD_TRACE_00 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "buffer too small");
						return LSA_FALSE;
					}

					offs_block = offs;

					CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
					offs += 2;
					CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
					offs += 2;

					if( ma != 0 ) { /* always true. for easier diff-ing against cm_sv_record_marshal_diag() */

						CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/);
						offs += 1+1;

						CM_PUT32_HTON(data, offs, /*iter->ap->*/api);
						offs += 4;
					}
					else {

						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_ASSERT(/*iter->ap->*/api == 0);
					}

					CM_PUT16_HTON(data, offs, /*iter->slot->*/slot_nr);
					offs += 2;
					CM_PUT16_HTON(data, offs, /*iter->subslot->*/subslot_nr);
					offs += 2;

					CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
					offs += 2;
					offs_chpr_sum = offs; /* ChannelProperties are written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x8002/*ExtChannelDiagnosis*/);
					offs += 2;

					/***/

					{
					LSA_UINT32 diag_cnt;
					LSA_UINT16 chpr_sum;

					diag_cnt = 0;
					chpr_sum = CM_CHPR_SPEC_DIS;

					/*
					 * ExtChannelDiagnosisData ::=
					 *   ChannelNumber, ChannelProperties, ChannelErrorType,
					 *   ExtChannelErrorType, ExtChannelAddValue
					 */

					if( is_not_null(ext_phys_diag_list) ) {

						CM_PD_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, ext_phys_diag_list, CM_PD_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + 2+4 > size ) {
									CM_PD_TRACE_00 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "buffer too small");
									return LSA_FALSE;
								}

								CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type);
								offs += 2;

								CM_PUT16_HTON(data, offs, diag->ext_channel_error_type);
								offs += 2;
								CM_PUT32_HTON(data, offs, diag->ext_channel_add_value);
								offs += 4;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					/* finally */

					CM_PUT16_HTON(data, offs_chpr_sum, chpr_sum);

					/***/

					if( diag_cnt != 0 ) {

						LSA_UINT32 blklen = offs - (offs_block + (2+2));

						if( blklen > 0xFFFF ) {
							CM_PD_TRACE_01 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "cannot store blklen(%u)", blklen);
#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: need to limit on add-diag!")*/
#endif /* CM_MESSAGE */
							return LSA_FALSE;
						}

						CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
					}
					else { /* rollback */

						offs = offs_block;
					}
					}
				}
				}
			}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	*offs_ptr = offs; /* update */

	return LSA_TRUE; /* ok */
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
