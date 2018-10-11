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
/*  F i l e               &F: cm_pd8.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MasterProjectAdaption for PDEV and CM-CL, common feature                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  880
#define CM_MODULE_ID       880

#include "cm_int.h"

/*----------------------------------------------------------------------------*/

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

#if CM_CFG_MAX_CLIENTS
/*callback*/ static LSA_VOID CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstanceTailorData_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	);
#endif

static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_send_clock_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_IF_PTR_TYPE  port_if,
	LSA_BOOL is_prm_server
	);
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_write_internal_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	);
#if CM_CFG_USE_MRP
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_mrp_interface_adjust_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port_if,
	LSA_BOOL is_prm_server
	);
#endif
#if CM_CFG_USE_GSY
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_pd_sync_pll_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_IF_PTR_TYPE  port_if
	);
#endif


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_masterproject (
	CM_CHANNEL_PTR_TYPE usr_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get/*_nn*/ (usr_channel, CM_PATH_TYPE_PD);

	if (is_not_null (pd_channel->usr.pd.reclist.mtd))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}



/*------------------------------------------------------------------------------
// cm_pd_list_record - functions
//	use upper mem, see 853634(use upper mem for records)
//
//	cm_pd_list_record_append()
//	cm_pd_list_record_lookup()
//	cm_pd_list_record_free()
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_list_record_append (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_LIST_ENTRY_PTR_TYPE		add_to_this_list,

	LSA_UINT32					api,
	LSA_UINT16					slot_nr,
	LSA_UINT16					subslot_nr,
	LSA_UINT32					record_index,
	LSA_UINT16					transfer_sequence,
	LSA_BOOL					mk_remanent,
	LSA_BOOL					is_local,
	LSA_UINT32					record_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE	record_data,

	CM_AR_GRAPH_RECORD_PTR_TYPE	CM_LOCAL_MEM_ATTR * rec_ptr_ptr /* out */
	)
{
	CM_LOG_FCT ("cm_pd_list_record_append")
	CM_AR_GRAPH_RECORD_PTR_TYPE  ptr;
	LSA_UINT32  alloc_size;

	*rec_ptr_ptr = LSA_NULL;

		/* argument check */
	alloc_size = sizeof (*ptr) + record_data_len;

	ptr = LSA_NULL;
	if (alloc_size <= 0xFFFF)
	{
		CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& ptr), cm_null_user_id, (LSA_UINT16)(alloc_size), channel->sysptr);
	}
	if (is_null (ptr))
	{
		CM_PD_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "no local-mem, size(%u)", alloc_size);
		return 255; /* no mem (for record) */
	}

	CmListInitialize (			& ptr->prm_link);

	ptr->api					= api;
	ptr->slot_nr				= slot_nr;
	ptr->subslot_nr				= subslot_nr;

	ptr->record_index			= record_index;
	ptr->transfer_sequence		= transfer_sequence;

	ptr->mk_remanent			= mk_remanent;
	ptr->is_local				= is_local;

	ptr->tailor_data_alloc_len  = 0;
	ptr->tailor_data_len		= CM_TAILOR_DATA_LEN_NOT_TAILORED;
	ptr->tailor_data			= LSA_NULL;

	ptr->record_data_len		= (LSA_UINT16)record_data_len;

	CM_MEMCPY (ptr->record_data, record_data, record_data_len); /* dynamic size, see typedef */

	/***/

	CM_PD_TRACE_05 (0, LSA_TRACE_LEVEL_CHAT
		, "append-record to submod(%u/%u/0x%x) rec_index(0x%x) rec_len(%u)"
		, api, slot_nr, subslot_nr, record_index, record_data_len
		);

	CmListInsertTail (add_to_this_list, &ptr->link); /* NOTE: not sorted! */

	/***/

	*rec_ptr_ptr = ptr;
	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/
static CM_AR_GRAPH_RECORD_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_list_record_lookup (
	CM_LIST_ENTRY_PTR_TYPE		from_this_list,

	LSA_UINT32					api,
	LSA_UINT16					slot_nr,
	LSA_UINT16					subslot_nr,
	LSA_UINT32					record_index
	)
{
	CM_AR_GRAPH_RECORD_PTR_TYPE  ptr;

	for (CmListForeach (ptr, from_this_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
	{
		if (api == ptr->api && slot_nr == ptr->slot_nr && subslot_nr == ptr->subslot_nr && record_index == ptr->record_index)
		{
			return ptr;
		}
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_list_record_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	)
{
	LSA_UINT16  rc;

	if (is_not_null (rec->tailor_data))
	{
		CM_FREE_UPPER_MEM (&rc, rec->tailor_data, channel->sysptr);
		CM_ASSERT (rc == LSA_RET_OK);
		rec->tailor_data = LSA_NULL;
		rec->tailor_data_alloc_len = 0;
	}

	CM_FREE_UPPER_MEM (&rc, rec, channel->sysptr);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG (rc);
}


/*------------------------------------------------------------------------------
//	PD-RECLIST
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_reclist_init")
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;

	CmListInitialize (& reclist->regular_list);
	CmListInitialize (& reclist->empty_list);
	CmListInitialize (& reclist->rtf_list);
	reclist->mtd = LSA_NULL;
	reclist->is_running_mpa = LSA_FALSE;
	reclist->mpa_rb = LSA_NULL;

	cm_pd_reclist_prm_begin (channel, LSA_TRUE);

	reclist->is_valid = LSA_TRUE; /*!*/
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_REGULAR);
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_EMPTY);
	cm_pd_reclist_clear (channel, CM_PD_RECLIST_RTF);
	cm_pd_mtd_free (& channel->usr.pd.reclist.mtd);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_prm_begin (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  clear_pll_data
	)
{
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;

	reclist->is_valid					= LSA_FALSE;
	reclist->has_error					= LSA_FALSE;
	reclist->faulty_record_index		= 0;

	reclist->prm_data.do_irt				= LSA_FALSE;
	reclist->prm_data.do_mrp_record			= LSA_FALSE;
	reclist->prm_data.scr_send_clock		= 0; /* see cm_cmpd_prm_write() and cm_pd_reclist_add_default_rec() */
	reclist->prm_data.scr_sendclock_prop	= 0; /* see where scr_send_clock is set */
	reclist->prm_data.ir_data_uuid			= cm_uuid_nil_uuid;

	if (clear_pll_data)
	{
			/* PDSyncPLL v1.1 */
		reclist->prm_data.pll_reduction_ratio	= 0;
		reclist->prm_data.pll_hw_delay			= 0;
		reclist->prm_data.pll_cntrl_interval	= 0;
		reclist->prm_data.pll_window			= 0;
		reclist->prm_data.pll_setwaitcount		= 0;
	}

	cm_pd_reclist_clear (channel, CM_PD_RECLIST_REGULAR); /* clear normal records */

	cm_pd_mtd_free (&channel->usr.pd.reclist.mtd);
}


/*----------------------------------------------------------------------------*/
static  CM_LIST_ENTRY_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_reclist_get (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 list_type
	)
{
	switch(list_type)
	{
	case CM_PD_RECLIST_REGULAR:
		return &channel->usr.pd.reclist.regular_list;

	case CM_PD_RECLIST_EMPTY:
		return &channel->usr.pd.reclist.empty_list;

	case CM_PD_RECLIST_RTF:
		return &channel->usr.pd.reclist.rtf_list;

	default:
		CM_FATAL1 (list_type);
		return LSA_NULL;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_clear (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type
	)
{
	CM_LOG_FCT("cm_pd_reclist_clear")
	CM_LIST_ENTRY_PTR_TYPE  reclist = cm_pd_reclist_get (channel, list_type);
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

	while (! CmListIsEmpty (reclist))
	{
		CmListRemoveHead (reclist, rec, CM_AR_GRAPH_RECORD_PTR_TYPE);

		cm_pd_list_record_free (channel, rec);
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_write (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_reclist_write")

	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;
	CM_LIST_ENTRY_PTR_TYPE reclist = cm_pd_reclist_get(channel, list_type);
	LSA_UINT16  transfer_sequence = 0;
	LSA_UINT32  api = 0; /* PDEV */

	CM_AR_GRAPH_RECORD_PTR_TYPE  rec;
	LSA_UINT16  rc;

	/* remove if exist, keep only latest record */
	rec = cm_pd_list_record_lookup (reclist, api, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index);
	if (is_not_null (rec))
	{
		CmListRemoveEntry (& rec->link);
		cm_pd_list_record_free (channel, rec);
	}

	rc = cm_pd_list_record_append (channel, reclist
		, api, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
		, prm_rdwr->record_index, transfer_sequence, prm_rdwr->mk_remanent, prm_rdwr->is_local, prm_rdwr->record_data_length, prm_rdwr->record_data
		, &rec
		);

	if (rc != 0)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "reclist-write: failed, record_index(%u), list_type(%u)", prm_rdwr->record_index, list_type
			);
		return CM_ERR_ALLOC_LOCAL;
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_tailor (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_reclist_tailor")

	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = channel->usr.pd.reclist.mtd;

	if (is_not_null(mtd))
	{
		CM_PD_PORT_IF_PTR_TYPE       port_if    = cm_pd_port_if__nn (channel);
		CM_LIST_ENTRY_PTR_TYPE       reclist    = cm_pd_reclist_get (channel, CM_PD_RECLIST_REGULAR);
		LSA_UINT32                   api        = 0; /* PDEV */
		LSA_UINT16                   slot_nr    = port_if->port.addr.slot_nr;
		LSA_UINT16                   subslot_nr = port_if->port.addr.subslot_nr;

		CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052;
		CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos;
		LSA_UINT16  ioc_nos_len, rsp;

		{
			CM_AR_GRAPH_RECORD_PTR_TYPE  rec;
			for (CmListForeach (rec, reclist, CM_AR_GRAPH_RECORD_PTR_TYPE))
			{	
				rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;
			}
		}

		if (! cm_oha_get_name_of_station (channel, &ioc_nos_len, &ioc_nos))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "Controller NoS missing");
			return CM_ERR_SEQUENCE;
		}

		{
			CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

			for (CmListForeach(rec, reclist, CM_AR_GRAPH_RECORD_PTR_TYPE))
			{
				if (rec->record_index == 0x802b/* PDPortDataCheck */)
				{
					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}

		{
			LSA_UINT16  pdirdata_td_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

			if (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)
			{
				CM_AR_GRAPH_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x802C/* PDIRData */);

				rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
				if (rsp != CM_OK)
				{
					return rsp;
				}
				pdirdata_td_len = rec->tailor_data_len;
			}

			if (mtd->properties & CM_PD_MTD_PROP_SYNCDATA)
			{
				CM_AR_GRAPH_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x802d/* PDSyncData */);
				if (pdirdata_td_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
				{
					CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dropping PDSyncData");
					rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
				}
				else if (mtd->properties & CM_PD_MTD_PROP_NOS)
				{
					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}

		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
		{
			rec_8052 = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x8052/* PDInterfaceMrpDataAdjust */);

			rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec_8052, LSA_NULL, ioc_nos, ioc_nos_len);
			if (rsp != CM_OK)
			{
				return rsp;
			}
		}
		else
		{
			rec_8052 = LSA_NULL;
		}

		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
		{
			CM_AR_GRAPH_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x8051/* PDInterfaceMrpDataCheck */);

			rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, rec_8052, ioc_nos, ioc_nos_len);
			if (rsp != CM_OK)
			{
				return rsp;
			}
		}

		{
			LSA_UINT16 index;

			for (index = 0; index < mtd->port_cnt; index++)
			{
				CM_PD_MTD_PORT_PTR_TYPE mtdp = & mtd->ports[index];

				if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
				{
					CM_AR_GRAPH_RECORD_PTR_TYPE rec = cm_pd_list_record_lookup (reclist
						, api, mtdp->local_slot_nr, mtdp->local_subslot_nr, 0x8053/* PDPortMrpDataAdjust */
						);

					rsp = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, rec_8052, ioc_nos, ioc_nos_len);
					if (rsp != CM_OK)
					{
						return rsp;
					}
				}
			}
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_reset_to_factory (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_reclist_reset_to_factory")

	CM_LIST_ENTRY_PTR_TYPE  rec_list = cm_pd_reclist_get(channel, CM_PD_RECLIST_RTF); /* ResetToFactory */
	CM_AR_GRAPH_RECORD_PTR_TYPE rec;

	for (CmListForeach(rec, rec_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
	{
		if (rec->record_index == 0x00010000/* SendClock */)
		{
			CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;
			reclist->prm_data.scr_send_clock = CM_GET16_NTOH(rec->record_data, 6); /* SendClock from cm_pd_write_send_clock_alloc() */
			reclist->prm_data.scr_sendclock_prop = CM_GET16_NTOH(rec->record_data, 8); /* SendClockProp from cm_pd_write_send_clock_alloc() */
			break;
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_add_ep_rec (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_reclist_add_ep_rec")

	CM_LIST_ENTRY_PTR_TYPE rec_list = cm_pd_reclist_get(channel, CM_PD_RECLIST_EMPTY);
	CM_AR_GRAPH_RECORD_PTR_TYPE rec;

	for (CmListForeach(rec, rec_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
	{
		if (rec->record_index == 0x00010000/* SendClock */)
		{
			CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;

			/* update the pre-allocated SendClock-record */

			CM_PUT16_HTON(rec->record_data, 6, reclist->prm_data.scr_send_clock); /* SendClock from CM_PD_RECLIST_REGULAR */
			CM_PUT16_HTON(rec->record_data, 8, reclist->prm_data.scr_sendclock_prop); /* SendClockProp from CM_PD_RECLIST_REGULAR */
			break;
		}
	}

	return CM_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_add_default_rec (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_BOOL  is_prm_server,
	LSA_BOOL  set_mrp_off
	)
{
	CM_LOG_FCT("cm_pd_reclist_add_default_rec")

	CM_PD_PORT_IF_PTR_TYPE port_if = cm_pd_port_if__nn (channel);
	CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_addr (channel, port_if->port.addr.slot_nr, port_if->port.addr.subslot_nr);
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;
	LSA_UINT16 rsp = CM_OK;

	if( reclist->prm_data.scr_send_clock == 0 ) /* no SendClock-record written, make default record */
	{
		CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_send_clock_alloc (channel, port_if, is_prm_server);

		if (is_null(rb))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to alloc send clock rb"
				);
			return CM_ERR_RESOURCE;
		}

		rsp = cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb);

		if (rsp == CM_OK)
		{
			/* set scr_send_clock */
			rsp = cm_cmpd_prm_write(pdprm, port, rb->args.pd.prm_rdwr->record_index, rb->args.pd.prm_rdwr->record_data_length, rb->args.pd.prm_rdwr->record_data);
		}

		if (rsp != CM_OK)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to append send clock record"
				);
		}

		cm_pd_write_internal_free (channel, rb);
	}

#if CM_CFG_USE_MRP
	if (rsp == CM_OK
	&&	(! reclist->prm_data.do_mrp_record/* if no MRP-record written */ &&  set_mrp_off/* AP00824058: set MRPRole := OFF */))
	{
		CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_mrp_interface_adjust_alloc (channel, & port_if->port, is_prm_server);

		if (is_null(rb))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to alloc mrp adjust rb"
				);
			return CM_ERR_RESOURCE;
		}

		rsp = cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb);

		if (rsp == CM_OK)
		{
			/* set do_mrp_record */
			rsp = cm_cmpd_prm_write (pdprm
				, port, rb->args.pd.prm_rdwr->record_index, rb->args.pd.prm_rdwr->record_data_length, rb->args.pd.prm_rdwr->record_data
				);
		}

		if (rsp != CM_OK)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to append mrp adjust record"
				);
		}

		cm_pd_write_internal_free (channel, rb);
	}
#else
	LSA_UNUSED_ARG (set_mrp_off);
#endif

#if CM_CFG_USE_GSY
	if (rsp == CM_OK
	&&  is_prm_server
	&&  reclist->prm_data.pll_reduction_ratio > 0
	&&  is_null (cm_pd_list_record_lookup (cm_pd_reclist_get(channel, CM_PD_RECLIST_REGULAR), 0, port_if->port.addr.slot_nr, port_if->port.addr.subslot_nr, 0x00011020/* PDSyncPLL */)) )
	{
		CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_pd_sync_pll_alloc (channel, port_if);
		if (is_null(rb))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to alloc sync pll rb"
				);
			return CM_ERR_RESOURCE;
		}

		rsp = cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb);

		if (rsp == CM_OK)
		{
			/* set pll_reduction_ratio */
			rsp = cm_cmpd_prm_write(pdprm
				, port, rb->args.pd.prm_rdwr->record_index, rb->args.pd.prm_rdwr->record_data_length, rb->args.pd.prm_rdwr->record_data
				);
		}

		if (rsp != CM_OK)
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "failed to append sync pll record"
				);
		}

		cm_pd_write_internal_free(channel, rb);

	}
#endif

	return rsp;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_alloc_records (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_LOG_FCT("cm_pd_reclist_alloc_records")
	CM_PD_PORT_IF_PTR_TYPE port_if = cm_pd_port_if__nn (channel);

	CM_UPPER_RQB_PTR_TYPE rb = cm_pd_write_send_clock_alloc (channel, port_if, LSA_FALSE); /* default send clock */
	if (is_null(rb))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to alloc send clock rb"
			);
		CM_FATAL();
		return;
	}

	if (cm_pd_reclist_write (channel, CM_PD_RECLIST_EMPTY, rb) != CM_OK) /* used for empty parameterization */
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to append send clock record"
			);
		CM_FATAL();
		return;
	}

	if (cm_pd_reclist_write (channel, CM_PD_RECLIST_RTF, rb) != CM_OK) /* used for reset to factory */
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "failed to append send clock record"
			);
		CM_FATAL();
		return;
	}

	cm_pd_write_internal_free (channel, rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_set_error (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  faulty_record_index
	)
{
	if (! channel->usr.pd.reclist.has_error)
	{
		channel->usr.pd.reclist.has_error           = LSA_TRUE;
		channel->usr.pd.reclist.faulty_record_index = faulty_record_index;
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_reclist_validate_and_prealloc_tailoring_records (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	CM_LOG_FCT("cm_pd_reclist_validate_and_prealloc_tailoring_records")
	CM_LIST_ENTRY_PTR_TYPE       reclist    = cm_pd_reclist_get(channel, CM_PD_RECLIST_REGULAR);
	LSA_UINT32                   api        = 0; /* PDEV */
	LSA_UINT16                   slot_nr    = channel->usr.pd.port_if.port.addr.slot_nr;
	LSA_UINT16                   subslot_nr = channel->usr.pd.port_if.port.addr.subslot_nr;
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec, rec_8052;

	for (CmListForeach (rec, reclist, CM_AR_GRAPH_RECORD_PTR_TYPE))
	{
		if (cm_pd_mpa_prm_parse_record (channel, mtd, rec) != CM_OK)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ParseError, record-index(0x%x)", rec->record_index);
			return rec->record_index;
		}
	}


	if (mtd->properties & CM_PD_MTD_PROP_NOS)
	{
		;
		/// check missing PDPortDataCheck
	}

	if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
	{
		rec = cm_pd_list_record_lookup (reclist, api, slot_nr, subslot_nr, 0x8052/* PDInterfaceMrpDataAdjust */);
		rec_8052 = rec;

		if (is_null (rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
			return 0x8052;
		}
	}
	else
	{
		rec_8052 = LSA_NULL;
	}

	if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
	{
		rec = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x8051/* PDInterfaceMrpDataCheck */);
		if (is_null(rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataCheck record not found");
			return 0x8051;
		}
		if (is_null (rec_8052))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
			return 0x8051;
		}
	}

	if ((mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA))
	{
		rec = cm_pd_list_record_lookup(reclist, api, slot_nr, subslot_nr, 0x802C/* PDIRData */);
		if (is_null(rec))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDIRData record not found");
			return 0x802C;
		}
	}

	{
		LSA_UINT16  index;

		for (index = 0; index < mtd->port_cnt; index++)
		{
			CM_PD_MTD_PORT_PTR_TYPE mtdp = & mtd->ports[index];

			if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
			{
				rec = cm_pd_list_record_lookup(reclist, api, mtdp->local_slot_nr, mtdp->local_subslot_nr, 0x8053/* PDPortMrpDataAdjust */);
				if (is_null(rec))
				{
					CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDPortMrpDataAdjust record not found, port-index(%u)", index);
					return 0x8053;
				}
				if (is_null(rec_8052))
				{
					CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDInterfaceMrpDataAdjust record not found");
					return 0x8053;
				}
			}
		}
	}

	/* prealloc tailor_data */
	for (CmListForeach (rec, reclist, CM_AR_GRAPH_RECORD_PTR_TYPE))
	{
		if (cm_pd_mpa_prm_prealloc_tailor_data (channel, rec) != CM_OK)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDev prealloc tailor data failed, record_index(0x%x)", rec->record_index);
			return rec->record_index;
		}
	}

	return 0; /* Ok */
}


/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_RECORD_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_reclist_get_next (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	LSA_UINT16 list_type
	)
{
	CM_LOG_FCT("cm_pd_reclist_get_next")
	CM_LIST_ENTRY_PTR_TYPE  list = cm_pd_reclist_get(channel, list_type);

	if (is_null (rec))
	{
		rec = CmListFirst (list, CM_AR_GRAPH_RECORD_PTR_TYPE);
	}
	else
	{
		rec = CmListNext (list, & rec->link, CM_AR_GRAPH_RECORD_PTR_TYPE);
	}

	while (is_not_null (rec))
	{
		if (rec->tailor_data_len == CM_TAILOR_DATA_LEN_SKIP_RECORD)
		{
			rec = CmListNext (list, & rec->link, CM_AR_GRAPH_RECORD_PTR_TYPE);
		}
		else
		{
			return rec;
		}
	}

	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_RECORD_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_reclist_read (
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			list_type,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	)
{
	CM_LIST_ENTRY_PTR_TYPE  reclist = cm_pd_reclist_get (channel, list_type);
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

	rec = cm_pd_list_record_lookup (reclist, 0/*PDEV*/, slot_nr, subslot_nr, record_index);

	return rec;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_delete ( /* TIA 1095777 */
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	)
{
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);

	if (is_null (port))
	{
		return CM_ERR_PRM_PORTID; /* not mapped */
	}
	else
	{
		CM_LIST_ENTRY_PTR_TYPE  reclist = cm_pd_reclist_get (channel, CM_PD_RECLIST_REGULAR);
		CM_AR_GRAPH_RECORD_PTR_TYPE  rec;

		rec = cm_pd_list_record_lookup (reclist, 0/*PDEV*/, slot_nr, subslot_nr, record_index);

		if (is_not_null (rec))
		{
			CmListRemoveEntry (& rec->link);
			cm_pd_list_record_free (channel, rec);

			return CM_OK;
		}
		else
		{
			return CM_ERR_PRM_INDEX; /* not found */
		}
	}
}


/*------------------------------------------------------------------------------
// _mpa_ MasterProjectAdaption
//----------------------------------------------------------------------------*/

static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDIRData (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDIRData")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;
	LSA_UINT32					adj, number_of_ports;
	LSA_UINT16					block_type, block_len, block_version, pdirdata_block_version;

	/* PDIRData :=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, 
	//				PDIRFrameData, PDIRBeginEndData 
	*/
	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2+2+1+1 + 1+1 + 2+2;
	cm_get_assert (left >= adj);
	cm_get_assert (wr_left >= left);

	block_type		= CM_GET16_NTOH (dp, 0);
	block_len		= CM_GET16_NTOH (dp, 2);
	pdirdata_block_version	= CM_GET16_NTOH (dp, 4);

	cm_get_assert (block_type == CM_BLOCK_TYPE_PD_IR_DATA);
	cm_get_assert (block_len  == rec->record_data_len - (2+2));
	cm_get_assert (pdirdata_block_version == 0x0100 || pdirdata_block_version == 0x0101);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_adjust (dp, left, adj);
	cm_adjust (wr_dp, wr_left, adj);

	{
		/*	PDIRGlobalData ( BlockVersionLow = 1) :=
		//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, 
		//		(MaxPortTxDelay, MaxPortRxDelay) 
		//	PDIRGlobalData (BlockVersionLow = 2) :=
		//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, 
		//		(MaxPortTxDelay, MaxPortRxDelay, MaxLineRxDelay, YellowTime)* 
		*/

		adj = 2+2+1+1 + 1+1 + 16 + 4 + 4; 
		cm_get_assert (left >= adj);

		block_type				= CM_GET16_NTOH (dp,  0);
		block_len				= CM_GET16_NTOH (dp,  2);
		block_version			= CM_GET16_NTOH (dp,  4);
		number_of_ports			= CM_GET32_NTOH (dp,  28);

		cm_get_assert (block_type == CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA);
		cm_get_assert (block_len <= left - (2+2));
		if(block_version == 0x0101)
		{
			adj = 2 + 2 + block_len; /* no tailoring */
		}
		else
		{
			cm_get_assert (block_version == 0x0102)		
		}
		cm_get_assert (1 <= number_of_ports && number_of_ports <= 0xFF);

		cm_get_assert (left >= adj);
		
		CM_MEMCPY (wr_dp, dp, adj);

		cm_adjust (dp, left, adj);
		cm_adjust (wr_dp, wr_left, adj);

		if (block_version == 0x0102) 
		{
			LSA_UINT16 p;

			adj = 4 + 4 + 4 + 4; 

			for (p = 0; p < number_of_ports; ++p)
			{
				CM_PD_MTD_PORT_PTR_TYPE	mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, p);

				cm_get_assert (left >= adj);

				CM_MEMCPY (wr_dp, dp, adj);

				if (is_not_null (mtdp)  &&  ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT))
				{
					CM_PUT32_HTON (wr_dp, 8, mtdp->real_max_line_rx_delay);
				}

				cm_adjust (dp, left, adj);
				cm_adjust (wr_dp, wr_left, adj);
			}
		}
	}

	{
		/* PDIRFrameData (BlockVersionLow = 0) :=
		//		BlockHeader, Padding, Padding
		//		,   (FrameSendOffset, DataLength, ReductionRatio, Phase, FrameID, EtherType, RxPort, FrameDetails, TxPortGroup, [Padding*] )*
		//
		// PDIRFrameData (BlockVersionLow = 1) :=
		//		BlockHeader, Padding, Padding, FrameDataProperties
		//		, [ (FrameSendOffset, DataLength, ReductionRatio, Phase, FrameID, EtherType, RxPort, FrameDetails, TxPortGroup, [Padding*] )* ]
		*/

		adj = 2+2; 
		cm_get_assert (left >= adj);

		block_type		= CM_GET16_NTOH (dp,  0);
		block_len		= CM_GET16_NTOH (dp,  2);

		cm_get_assert (block_type == CM_BLOCK_TYPE_PD_IR_FRAME_DATA);
		cm_get_assert (CM_CAST_U32(block_len + (2+2)) <= left);

		if (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)
		{
			CM_COMMON_MEM_U8_PTR_TYPE  blocklen_dp = wr_dp;
			LSA_UINT32  block_left;
			LSA_UINT32	frame_count = 0;

			adj += 1+1 + 1+1;
			cm_get_assert (left >= adj);

			block_version	= CM_GET16_NTOH (dp,  4);

			if (block_version == 0x0101)
			{
				adj += 4; /* FrameDataProperties */
			}
			else
			{
				cm_get_assert (block_version == 0x0100);
			}

			cm_get_assert (left >= adj);
			cm_get_assert (wr_left >= adj);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_adjust (dp, left, adj);
			cm_adjust (wr_dp, wr_left, adj);

			cm_get_assert (block_len >= adj - (2+2));
			block_left = block_len - (adj - (2+2));

			while (block_left > 0)
			{
				LSA_UINT8	number_of_txportgroups;
				LSA_UINT16	frame_id;
				/* FrameSendOffset(0), DataLength(4), ReductionRatio(6), Phase(8), 
				// FrameID(10), EtherType(12), RxPort(14), FrameDetails(15), NumberOfTxPortGroups(16)
				*/

				adj = 4 + 2 + 2 + 2 + 2 + 2 + 1 + 1 + 1;
				/* TxPortGroupArray [Padding*] */
				cm_get_assert (left >= adj);

				frame_id				= CM_GET16_NTOH (dp, 10);
				number_of_txportgroups	= CM_GET8_NTOH  (dp, 16);

				adj += number_of_txportgroups;
				adj += (4 - adj % 4) % 4;

				cm_get_assert (left >= adj);

				if (cm_pdusr_mpa_tailor_frame_id (channel, frame_id))
				{
					cm_adjust (dp, left, adj); /* skip frame_id */
				}
				else
				{
					CM_MEMCPY (wr_dp, dp, adj);

					cm_adjust (dp, left, adj);
					cm_adjust (wr_dp, wr_left, adj);
					frame_count++;
				}
				cm_get_assert (block_left >= adj);
				block_left -= adj;
			}

			if(frame_count == 0)
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dropping PDIRData");
				rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
				return CM_OK;
			}

			CM_PUT16_HTON (blocklen_dp, 2, CM_CAST_U16 (wr_dp - blocklen_dp - (2+2)));
		}
		else
		{
			adj += block_len;

			cm_get_assert (left >= adj);
			cm_get_assert (wr_left >= adj);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_adjust (dp, left, adj);
			cm_adjust (wr_dp, wr_left, adj);
		}
	}

	if (left > 0)
	{
		cm_get_assert (pdirdata_block_version == 0x0101); /* PDIRBeginEndData */
		{
			/* PDIRBeginEndData := BlockHeader, Padding, Padding, RedGuard, NumberOfPorts, (NumberOfAssignments, 
			//						(TXBeginEndAssignment, RXBeginEndAssignment)*, NumberOfPhases, 
			//						(TXPhaseAssignment, RXPhaseAssignment)*)*
			*/

			LSA_UINT16	p;
			CM_COMMON_MEM_U8_PTR_TYPE  blocklen_dp = wr_dp;


			/* BlockHeader, Padding, Padding, RedGuard, NumberOfPorts */

			adj = 2+2+1+1 + 1+1 + 2+2 + 4;

			cm_get_assert (left >= adj);
			cm_get_assert (wr_left >= adj);

			block_type		= CM_GET16_NTOH (dp,  0);
			block_len		= CM_GET16_NTOH (dp,  2);
			block_version	= CM_GET16_NTOH (dp,  4);
			number_of_ports	= CM_GET32_NTOH (dp,  12);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA);
			cm_get_assert (block_len == left - (2+2));
			cm_get_assert (block_version == 0x0100);

			CM_MEMCPY (wr_dp, dp, adj);

			cm_adjust (dp, left, adj);
			cm_adjust (wr_dp, wr_left, adj);

			for (p = 0; p < number_of_ports; ++p)
			{
				LSA_UINT32				number_of_assignments, number_of_phases;
				CM_PD_MTD_PORT_PTR_TYPE	mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, p);
				LSA_UINT16				real_station_nr, real_slot_nr, real_subslot_nr, real_properties;

				number_of_assignments	= CM_GET32_NTOH (dp,  0);

				adj = 4 + number_of_assignments * ((4 + 4 + 4) + (4 + 4 + 4) );

				cm_get_assert (wr_left >= adj);

				number_of_phases	= CM_GET32_NTOH (dp, adj);

				adj += 4 + number_of_phases * (2 + 2);

				cm_get_assert (wr_left >= adj);

				/* NumberOfAssignments, (TXBeginEndAssignment, RXBeginEndAssignment)*, NumberOfPhases, 
				//						(TXPhaseAssignment, RXPhaseAssignment)*
				*/

				/*	TXBeginEndAssignment := RedOrangePeriodBegin, OrangePeriodBegin, GreenPeriodBegin 
				//	RXBeginEndAssignment := RedOrangePeriodBegin, OrangePeriodBegin, GreenPeriodBegin
				//	TXPhaseAssignment := PhaseAssignment 
				//	RXPhaseAssignment := PhaseAssignment
				*/

				cm_pd_mtdp_get_peer (mtdp, &real_station_nr, &real_slot_nr, &real_subslot_nr, &real_properties);

				if (is_not_null (mtdp)
				&&  (mtdp->properties & CM_PD_MTD_PEER_PROP_RED_PORT)
				&&	(!CM_IS_PDEV_SUBSLOT_NR (real_subslot_nr)  ||  !(real_properties & CM_PD_MTD_PEER_PROP_I_RTC3)))
				{
					LSA_UINT32	wr_adj = 4 + ((4+4+4) + (4+4+4)) + 4 + (2+2);

					cm_get_assert (wr_left >= wr_adj);

					CM_PUT32_HTON (wr_dp,  0, 0x01);
					CM_PUT32_HTON (wr_dp,  4, 0x00);
					CM_PUT32_HTON (wr_dp,  8, 0x00);
					CM_PUT32_HTON (wr_dp, 12, 0x00);
					CM_PUT32_HTON (wr_dp, 16, 0x00);
					CM_PUT32_HTON (wr_dp, 20, 0x00);
					CM_PUT32_HTON (wr_dp, 24, 0x00);

					CM_PUT32_HTON (wr_dp, 28, 0x01);
					CM_PUT16_HTON (wr_dp, 32, 0x00);
					CM_PUT16_HTON (wr_dp, 34, 0x00);

					cm_adjust (wr_dp, wr_left, wr_adj);
				}
				else
				{
					CM_MEMCPY (wr_dp, dp, adj);

					cm_adjust (wr_dp, wr_left, adj);
				}

				cm_adjust (dp, left, adj);
			}

			CM_PUT16_HTON (blocklen_dp, 2, CM_CAST_U16 (wr_dp - blocklen_dp - (2+2)));
		}
	}

	cm_get_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16(wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16(rec->tailor_data_len - (2+2)));

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDIRData Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT8  CM_LOCAL_FCT_ATTR
cm_pd_mpa_put_port_name (
	CM_COMMON_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  properties,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  slot_nr
	)
{
	LSA_UINT8  port_nr = subslot_nr & 0x00FF;
	LSA_UINT8  p_len = 8;

	if (properties & CM_PD_MTD_PEER_PROP_PORT_CODING)
	{
		p_len = 14;
	}

	if (is_null (dp))
	{
		return p_len;
	}

	CM_PUT8_HTON (dp, 0, 'p');
	CM_PUT8_HTON (dp, 1, 'o');
	CM_PUT8_HTON (dp, 2, 'r');
	CM_PUT8_HTON (dp, 3, 't');
	CM_PUT8_HTON (dp, 4, '-');
	CM_PUT8_HTON (dp, 5, '0');	/* see CM_PORT_UINT / EDD_CFG_MAX_PORT_CNT */
	CM_PUT8_HTON (dp, 6, '0' + port_nr / 10);
	CM_PUT8_HTON (dp, 7, '0' + port_nr % 10);

	if (p_len == 14)
	{
		CM_PUT8_HTON (dp,  8, '-');
		CM_PUT8_HTON (dp,  9, '0' + CM_CAST_U8 (slot_nr / 10000));
		slot_nr %= 10000;
		CM_PUT8_HTON (dp, 10, '0' + CM_CAST_U8 (slot_nr / 1000));
		slot_nr %= 1000;
		CM_PUT8_HTON (dp, 11, '0' + CM_CAST_U8 (slot_nr / 100));
		slot_nr %= 100;
		CM_PUT8_HTON (dp, 12, '0' + CM_CAST_U8 (slot_nr / 10));
		slot_nr %= 10;
		CM_PUT8_HTON (dp, 13, '0' + CM_CAST_U8 (slot_nr));
	}

	return p_len;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDPortDataCheck (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_PD_MTD_PORT_PTR_TYPE  start_mtdp, /* this port */
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos,
	LSA_UINT16  ioc_nos_len /* 0 .. do not tailor ioc_nos */
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDPortDataCheck")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;
	LSA_UINT32					adj;
	LSA_UINT32					max_checklinedelay = 0;
	LSA_UINT16					real_station_nr, real_slot_nr, real_subslot_nr, properties;

	cm_pd_mtdp_get_peer (start_mtdp, &real_station_nr, &real_slot_nr, &real_subslot_nr, &properties);

	if (CM_IS_PDEV_SUBSLOT_NR (real_subslot_nr))
	{
		CM_PD_MTD_PORT_PTR_TYPE  real_mtdp = cm_pdusr_mpa_lookup_mtdp (channel
			, real_station_nr, real_slot_nr, real_subslot_nr, LSA_NULL
			);

		if (real_mtdp) 
		{
			CM_PD_MTD_PORT_PTR_TYPE  peer_real_mtdp = cm_pdusr_mpa_lookup_mtdp (channel
				, real_mtdp->peer_station_nr, real_mtdp->peer_slot_nr, real_mtdp->peer_subslot_nr, LSA_NULL
				);

			if (start_mtdp->check_linedelay != 0  &&  peer_real_mtdp && peer_real_mtdp->check_linedelay != 0)
			{
				max_checklinedelay = LSA_MAX (start_mtdp->check_linedelay, peer_real_mtdp->check_linedelay);
			}
		}
	}
	else
	{
		rec->tailor_data_len = CM_TAILOR_DATA_LEN_SKIP_RECORD;
		return CM_OK;
	}

	/*	PDPortDataCheck ::=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
	//		, { [CheckPeers], [CheckLineDelay], [CheckMAUType], [CheckLinkState], [CheckSyncDifference], [CheckMAUTypeDifference] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2+2+1+1 + 1+1 + 2+2;
	cm_get_assert (left >= adj);
	cm_get_assert (wr_left >= adj);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_adjust (dp, left, adj);
	cm_adjust (wr_dp, wr_left, adj);

	{
		/*	CheckPeers		::= BlockHeader, NumberOfPeers, (LengthPeerPortName, PeerPortName, LengthPeerStationName, PeerStationName)*, [Padding*]
		//	CheckLineDelay	::= BlockHeader, Padding, Padding, LineDelay
		//	CheckMAUType	::= BlockHeader, MAUType
		//	CheckLinkState	::= BlockHeader, LinkState
		//	CheckSyncDifference	::= BlockHeader, CheckSyncMode
		//	CheckMAUTypeDifference ::= BlockHeader, MAUTypeMode
		*/
		while (left > 0)
		{
			LSA_UINT16  block_type, block_length;

			adj = 2+2;
			cm_get_assert (left >= adj);
			cm_get_assert (wr_left >= adj);

			block_type			= CM_GET16_NTOH (dp, 0);
			block_length		= CM_GET16_NTOH (dp, 2);

			switch (block_type)
			{
			case CM_BLOCK_TYPE_PD_CHK_LINE_DELAY:
				adj += block_length;

				cm_get_assert (left >= adj)

				if (max_checklinedelay == 0) /* skip sub-block */
				{
					cm_adjust (dp, left, adj);
				}
				else
				{
					cm_get_assert (wr_left >= adj)

					CM_MEMCPY (wr_dp, dp, adj);
					CM_PUT32_HTON (wr_dp, 8, max_checklinedelay);
						
					cm_adjust (dp, left, adj);
					cm_adjust (wr_dp, wr_left, adj);
				}
				break;

			case CM_BLOCK_TYPE_PD_CHK_PEERS:
				adj += block_length;

				cm_get_assert (left >= adj)

				if (! CM_IS_PDEV_SUBSLOT_NR (real_subslot_nr)) /* skip sub-block */
				{
					cm_adjust (dp, left, adj);
				}
				else
				{
					CM_COMMON_MEM_U8_PTR_TYPE	cp_wr_dp = wr_dp;

					/* BlockHeader + NrOfPeers(1) */
					{
						cm_get_assert (wr_left >= 2+2+1+1 + 1);

						CM_MEMCPY (wr_dp, dp, 2+2+1+1 + 1);

						cm_adjust (dp, left, adj);
						cm_adjust (wr_dp, wr_left, 2+2+1+1 + 1);
					}

					/* LengthPeerPortName, PeerPortName */
					{
						LSA_UINT8  p_len = cm_pd_mpa_put_port_name (LSA_NULL
							, properties, real_subslot_nr, real_slot_nr
							);

						adj = 1 + p_len;

						cm_get_assert (wr_left >= adj);

						CM_PUT8_HTON (wr_dp, 0, p_len/*LengthPeerPortName*/);
						p_len = cm_pd_mpa_put_port_name (wr_dp+1
							, properties, real_subslot_nr, real_slot_nr
							);
						cm_get_assert (adj > p_len);

						cm_adjust (wr_dp, wr_left, adj);
					}
					
					/* LengthPeerStationName, PeerStationName */
					{
						CM_COMMON_MEM_U8_PTR_TYPE  iod_nos = LSA_NULL;
						CM_COMMON_MEM_U8_PTR_TYPE  p_len_dp;
						LSA_UINT16  iod_nos_len = 0;
						LSA_UINT16  p_len = 0;

						if (real_station_nr > 0)
						{
#if CM_CFG_MAX_CLIENTS
							cm_clusr_dev_get_name (channel, real_station_nr, & iod_nos_len, & iod_nos);
#endif
						}

						p_len_dp = wr_dp;
						
						cm_get_assert (wr_left >= 1);
						CM_PUT8_HTON (wr_dp, 0, 0);
						cm_adjust (wr_dp, wr_left, 1);

						if (iod_nos_len > 0)
						{
							cm_get_assert (wr_left >= iod_nos_len);
							CM_MEMCPY (wr_dp, iod_nos, iod_nos_len);
							cm_adjust (wr_dp, wr_left, iod_nos_len);
							p_len += iod_nos_len;

							if (ioc_nos_len > 0 && (mtd->properties & CM_PD_MTD_PROP_NOS))
							{
								cm_get_assert (wr_left >= 1);
								CM_PUT8_HTON (wr_dp, 0, '.');
								cm_adjust (wr_dp, wr_left, 1);
								p_len += 1;
							}
						}

						if (ioc_nos_len > 0 && (real_station_nr == 0 || (mtd->properties & CM_PD_MTD_PROP_NOS)))
						{
							cm_get_assert (wr_left >= ioc_nos_len);
							CM_MEMCPY (wr_dp, ioc_nos, ioc_nos_len);
							cm_adjust (wr_dp, wr_left, ioc_nos_len);
							p_len += ioc_nos_len;
						}

						cm_get_assert (p_len <= 240);
						CM_PUT8_HTON (p_len_dp, 0, CM_CAST_U8(p_len));
					}

					/* Padding */
					while (((wr_dp - cp_wr_dp) & 3) != 0)
					{
						cm_get_assert (wr_left >= 1);
						CM_PUT8_HTON (wr_dp, 0, 0);
						cm_adjust (wr_dp, wr_left, 1);
					}

					/* CheckPeers.BlockHeader.BlockLength */
					CM_PUT16_HTON (cp_wr_dp, 2, CM_CAST_U16(wr_dp - cp_wr_dp - (2+2)));
				}
				break;

			case CM_BLOCK_TYPE_PD_CHK_MAUTYPE:
			default:
				adj += block_length;

				cm_get_assert (left >= adj)
				cm_get_assert (wr_left >= adj)

				CM_MEMCPY (wr_dp, dp, adj);
						
				cm_adjust (dp, left, adj);
				cm_adjust (wr_dp, wr_left, adj);
				break;						
			}
		}

		cm_get_assert (left == 0);
	}

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16(wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16(rec->tailor_data_len - (2+2)));

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortDataCheck Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDPortDataAdjust (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MTD_PORT_PTR_TYPE  start_mtdp, /* this port */
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDPortDataAdjust")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;
	LSA_UINT32					adj;

	/* PDPortDataAdjust ::=
	//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
	//		, { [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState], [AdjustPeerToPeerBoundary]
	//		, [AdjustDCPBoundary], [AdjustPreambleLength] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	{
		LSA_UINT16 real_station_nr, real_slot_nr, real_subslot_nr, real_properties;

		cm_pd_mtdp_get_peer (start_mtdp, &real_station_nr, &real_slot_nr, &real_subslot_nr, &real_properties);

		if (CM_IS_PDEV_SUBSLOT_NR(real_subslot_nr) && (real_properties & CM_PD_MTD_PEER_PROP_I_RTC3) != 0)
		{
			return CM_OK;
		}
	}

	adj = 2+2+1+1 + 1+1 + 2+2;
	cm_get_assert (left >= adj);
	cm_get_assert (wr_left >= adj);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_adjust (dp, left, adj);
	cm_adjust (wr_dp, wr_left, adj);

	{
		/*	AdjustPreambleLength	::= BlockHeader, Padding, Padding, PreambleLength, AdjustProperties, [Padding*]
		//	other					::= BlockHeader, Data*
		*/

		while (left > 0)
		{
			LSA_UINT16  block_type, block_length;

			adj = 2+2;
			cm_get_assert (left >= adj);
			cm_get_assert (wr_left >= adj);

			block_type			= CM_GET16_NTOH (dp, 0);
			block_length		= CM_GET16_NTOH (dp, 2);

			switch (block_type)
			{
			case CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH:
				adj += block_length;

				cm_get_assert (left >= adj)
				cm_get_assert (wr_left >= adj)

				CM_MEMCPY (wr_dp, dp, adj);

				CM_PUT16_HTON (wr_dp, 2+2+1+1 + 1+1, 0); /* PreambleLength.Length: Seven octets Preamble shall be used */

				cm_adjust (dp, left, adj);
				cm_adjust (wr_dp, wr_left, adj);

				break;

			default:
				adj += block_length;

				cm_get_assert (left >= adj)
				cm_get_assert (wr_left >= adj)

				CM_MEMCPY (wr_dp, dp, adj);
						
				cm_adjust (dp, left, adj);
				cm_adjust (wr_dp, wr_left, adj);
				break;						
			}
		}
	}

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16(wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16(rec->tailor_data_len - (2+2)));

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDSyncData (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos,
	LSA_UINT16  ioc_nos_len
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDSyncData")

	CM_COMMON_MEM_U8_PTR_TYPE dp = rec->tailor_data;
	LSA_UINT32 offs = 0;

	LSA_UINT32 offs_ri_begin, offs_ri_end, offs_pllwindow, offs_syncsendfactor, offs_sendclock;
	LSA_UINT32 offs_timeout, offs_subdomainname_length, offs_subdomainname, offs_subdomainuuid;

	LSA_UINT16 block_length, domain_name_length_new;
	LSA_UINT8 domain_name_length;
	LSA_UINT8 subdomain_uuid[CM_MD5_DIGEST_LENGTH];

	CM_MD5_CTX_TYPE ctx;

	LSA_UNUSED_ARG(channel);

	/*
	PDSyncData :=
		BlockHeader, Padding, Padding, PTCP_SubdomainUUID, ReservedIntervalBegin, ReservedIntervalEnd, PLLWindow, SyncSendFactor,
		SendClockFactor, PTCPTimeoutFactor, PTCPTakeoverTimeoutFactor, PTCPMasterStartupTime, SyncProperties, PTCP_MasterPriority1,
		PTCP_MasterPriority2, PTCPLengthSubdomainName, PTCPSubdomainName, [Padding*]
	*/

	offs += 2+2+1+1; /* BlockHeader */
	offs += 1;       /* Padding */
	offs += 1;       /* Padding */
	offs_subdomainuuid  = offs;
	offs += 16;      /* PTCP_SubdomainUUID */
	offs_ri_begin       = offs;
	offs += 4;       /* ReservedIntervalBegin */
	offs_ri_end         = offs;
	offs += 4;       /* ReservedIntervalEnd */
	offs_pllwindow      = offs;
	offs += 4;       /* PLLWindow */
	offs_syncsendfactor = offs;
	offs += 4;       /* SyncSendFactor */
	offs_sendclock      = offs;
	offs += 2;       /* SendClockFactor */
	offs_timeout        = offs;
	offs += 2;       /* PTCPTimeoutFactor */
	offs += 2;       /* PTCPTakeoverTimeoutFactor */
	offs += 2;       /* PTCPMasterStartupTime */
	offs += 2;       /* SyncProperties */
	offs += 1;       /* PTCP_MasterPriority1 */
	offs += 1;       /* PTCP_MasterPriority2 */
	offs_subdomainname_length = offs;

	if (offs+1 > rec->record_data_len) goto RecLenFailed;

	domain_name_length = CM_GET8_NTOH(rec->record_data, offs_subdomainname_length);
	domain_name_length_new = domain_name_length+1+ioc_nos_len;

	if (domain_name_length_new > 240)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "Tailored subdomain name would exceed 240 characters"
			);
		return CM_ERR_PRM_BLOCK;
	}

	CM_MEMCPY(dp, rec->record_data, offs_subdomainname_length);

	CM_PUT8_HTON(dp, offs, CM_CAST_U8(domain_name_length_new)); /* PTCPLengthSubdomainName */
	offs += 1;
	offs_subdomainname = offs;

	if (offs+domain_name_length > rec->record_data_len) goto RecLenFailed;

	CM_MEMCPY(dp+offs, rec->record_data+offs, domain_name_length); /* PTCPSubdomainName */
	offs += domain_name_length;

	CM_PUT8_HTON(dp, offs, '.');
	offs += 1;

	CM_MEMCPY(dp+offs, ioc_nos, ioc_nos_len); /* NoS controller */
	offs += ioc_nos_len;

	/* PTCP_SubdomainUUID := MD5 { ReservedIntervalBegin, ReservedIntervalEnd, PLLWindow, SyncSendFactor, SendClockFactor, PTCPTimeoutFactor, PTCPSubdomainName } */

	cm_md5_init(&ctx);
	cm_md5_update(&ctx, rec->record_data + offs_ri_begin, 4);
	cm_md5_update(&ctx, rec->record_data + offs_ri_end, 4);
	cm_md5_update(&ctx, rec->record_data + offs_pllwindow, 4);
	cm_md5_update(&ctx, rec->record_data + offs_syncsendfactor, 4);
	cm_md5_update(&ctx, rec->record_data + offs_sendclock, 2);
	cm_md5_update(&ctx, rec->record_data + offs_timeout, 2);
	cm_md5_update(&ctx, dp + offs_subdomainname, domain_name_length + 1 + ioc_nos_len);
	cm_md5_final(subdomain_uuid, &ctx);

	CM_MEMCPY(dp + offs_subdomainuuid, subdomain_uuid, CM_MD5_DIGEST_LENGTH); /* PTCP_SubdomainUUID */

	while ((offs & 3) != 0)
	{
		CM_PUT8_HTON(dp, offs, 0); /* Padding */
		offs += 1;
	}

	block_length = CM_CAST_U16(offs - (2+2));
	CM_PUT16_HTON(dp, 2, block_length); /* BlockLength */

	CM_ASSERT (offs <= 0xFFFF);
	rec->tailor_data_len = (LSA_UINT16)offs;

	return CM_OK;

RecLenFailed:
	CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDSyncData, Length of Record failed");

	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static  CM_COMMON_MEM_U8_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052,
	LSA_UINT16  rec_block_version,
	LSA_UINT8  rec_mrp_instance
	)
{
	CM_LOG_FCT("cm_pd_rec_8052_lookup_tailor_mrp_domainuuid")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE  dp = rec_8052->tailor_data;
	LSA_INT32  left = rec_8052->tailor_data_len;

	LSA_INT32  adj;
	LSA_UINT16  block_type, block_length, block_version;
	LSA_UINT8   nr_of_entries, mrp_instance;

	adj = 2+2+1+1 + 1+1;
	cm_get_assert (left >= adj);

	block_type    = CM_GET16_NTOH (dp, 0);
	block_length  = CM_GET16_NTOH (dp, 2);
	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH  (dp, 7);

	cm_get_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
	cm_get_assert (2+2 + block_length == left);
	cm_get_assert (block_version == 0x0100 || block_version == 0x0101);
	cm_get_assert (block_version == rec_block_version);

	if (block_version == 0x0100)
	{
		cm_get_assert (nr_of_entries == 0);

		cm_adjust (dp, left, adj);

		cm_get_assert (left >= 16);
		return dp;
	}
	else
	{
		cm_get_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_adjust (dp, left, adj);

		while (left > 0)
		{
			adj = 2+2+1+1 + 1+1;
			cm_get_assert (left >= adj);

			block_type    = CM_GET16_NTOH (dp, 0);
			block_length  = CM_GET16_NTOH (dp, 2);
			block_version = CM_GET16_NTOH (dp, 4);
			mrp_instance  = CM_GET8_NTOH  (dp, 7);

			cm_get_assert (block_type == 0x0231/*MrpInstance-DataAdjustBlock*/);
			cm_get_assert (left >= 2+2 + block_length);
			cm_get_assert (block_version == 0x0100);
			cm_get_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);
		
			if (mrp_instance == rec_mrp_instance)
			{
				cm_adjust (dp, left, adj);

				cm_get_assert (left >= 16);
				return dp;
			}

			cm_adjust (dp, left, 2+2 + block_length);
		}

		return LSA_NULL;
	}

cm_get_assert_failed:
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_03 (0, LSA_TRACE_LEVEL_UNEXP
		, "Record8052, lookup MRP_DomainUUID, mrp_instance(%u), error in line(%u): record-offset(%u)"
		, rec_mrp_instance, cm_get_assert_line, rec_8052->tailor_data_len - left
		);
	return LSA_NULL;

}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataCheck (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataCheck")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;

	LSA_UINT32					adj;
	LSA_UINT16					block_version;
	LSA_UINT8					nr_of_entries;

	/*	PDInterface-MrpDataCheck [BlockVersionLow = 0]  ::=
	//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Check
	//
	//	PDInterface-MrpDataCheck [BlockVersionLow = 1]  ::=
	//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataCheckBlock*
	//
	//	MrpInstance-DataCheckBlock [BlockType = 0x0233] ::=
	//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID, MRP_Check
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2+2+1+1 + 1+1;
	cm_get_assert (left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH  (dp, 7);

	cm_get_assert (block_version == 0x0100 || block_version == 0x0101);

	if (block_version == 0x0100)
	{
		cm_get_assert (nr_of_entries == 0);
	}
	else
	{
		cm_get_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_get_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_adjust (dp, left, adj);
		cm_adjust (wr_dp, wr_left, adj);
	}

	while (left > 0)
	{
		CM_COMMON_MEM_U8_PTR_TYPE	uuid;
		LSA_UINT8  mrp_instance;

		adj = 2+2+1+1 + 1+1 + 16 + 4;

		cm_get_assert (left >= adj);
		cm_get_assert (wr_left >= adj);

		mrp_instance = CM_GET8_NTOH (dp, 7);

		cm_get_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);

		CM_MEMCPY (wr_dp, dp, adj);

		uuid = cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (rec_8052, block_version, mrp_instance);
		cm_get_assert (is_not_null (uuid));

		CM_MEMCPY (wr_dp + 2+2+1+1 + 1+1, uuid, 16);

		cm_adjust (dp, left, adj);
		cm_adjust (wr_dp, wr_left, adj);
	}

	cm_get_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = rec->record_data_len;

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInterfaceMrpDataCheck Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDPortMrpDataAdjust (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDPortMrpDataAdjust")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;

	LSA_UINT32					adj;
	LSA_UINT16					block_version;
	LSA_UINT8					mrp_instance;
	CM_COMMON_MEM_U8_PTR_TYPE	uuid;

	/*	PDPortMrpDataAdjust [BlockVersionLow = 0]  ::=
	//		BlockHeader, Padding, Padding, MRP_DomainUUID
	//
	//	PDPortMrpDataAdjust [BlockVersionLow = 1]  ::=
	//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2+2+1+1 + 1+1;
	cm_get_assert (left >= adj);
	cm_get_assert (wr_left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	mrp_instance  = CM_GET8_NTOH  (dp, 7);

	cm_get_assert (block_version == 0x0100 || block_version == 0x0101);
	cm_get_assert (/*0 <= mrp_instance &&*/ mrp_instance <= 126);

	CM_MEMCPY (wr_dp, dp, adj);

	cm_adjust (wr_dp, wr_left, adj);


	adj = 16;
	cm_get_assert (wr_left >= adj);

	uuid = cm_pd_rec_8052_lookup_tailor_mrp_domainuuid (rec_8052, block_version, mrp_instance);
	cm_get_assert (is_not_null (uuid));

	CM_MEMCPY (wr_dp, uuid, adj);

	cm_adjust (wr_dp, wr_left, adj);
	LSA_UNUSED_ARG (wr_dp); /* lint */
	LSA_UNUSED_ARG (wr_left); /* lint */

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = rec->record_data_len;

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDPortMrpDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataAdjust (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos,
	LSA_UINT16  ioc_nos_len
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataAdjust")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE	dp		= rec->record_data;
	LSA_UINT32					left	= rec->record_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	wr_dp	= rec->tailor_data;
	LSA_UINT32					wr_left	= rec->tailor_data_alloc_len;
	LSA_UINT32					adj;
	LSA_UINT16					block_version;
	LSA_UINT8					nr_of_entries, name_length;

	/*	PDInterface-MrpDataAdjust [BlockVersionLow = 0] ::=
	//		BlockHeader, Padding, Padding
	//		, MRP_DomainUUID, MRP_Role, [Padding*] a, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
	//
	//	PDInterface-MrpDataAdjust [BlockVersionLow = 1] ::=
	//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataAdjustBlock*
	//
	//	MrpInstance-DataAdjustBlock [BlockType = 0x0231] ::=
	//		BlockHeader, Padding, MRP_Instance
	//		, MRP_DomainUUID, MRP_Role, [Padding*] a, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
	*/

	rec->tailor_data_len = CM_TAILOR_DATA_LEN_NOT_TAILORED;

	adj = 2+2+1+1 + 1+1;
	cm_get_assert (left >= adj);

	block_version = CM_GET16_NTOH (dp, 4);
	nr_of_entries = CM_GET8_NTOH  (dp, 7);

	cm_get_assert (block_version == 0x0100 || block_version == 0x0101);

	if (block_version == 0x0100)
	{
		cm_get_assert (nr_of_entries == 0);
		
		nr_of_entries += 1;
	}
	else
	{
		cm_get_assert (1 <= nr_of_entries  && nr_of_entries <= 127);

		cm_get_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_adjust (dp, left, adj);
		cm_adjust (wr_dp, wr_left, adj);
	}

	while (nr_of_entries > 0)
	{
		CM_COMMON_MEM_U8_PTR_TYPE	block_dp;
		CM_COMMON_MEM_U8_PTR_TYPE	wr_block_dp, wr_uuid_dp, wr_name_dp;
		LSA_UINT16  block_type;
		LSA_UINT32  block_left;

		block_dp = dp;
		wr_block_dp = wr_dp;

		adj = 2+2+1+1 + 1 + 1;
		wr_uuid_dp = wr_block_dp + adj;

		adj += 16 + 2 + 2 + 1;
		cm_get_assert (left >= adj);
		cm_get_assert (wr_left >= adj);

		block_type  = CM_GET16_NTOH (dp, 0);
		block_left  = 2+2 + CM_GET16_NTOH (dp, 2/* block_length */); 
		name_length = CM_GET8_NTOH  (dp, adj - 1);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_get_assert ((block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ    &&  block_version == 0x0100)
		||             (block_type == 0x0231/*MrpInstanceDataAdjustBlock*/  &&  block_version == 0x0101));
		cm_get_assert (block_left <= left);
		cm_get_assert (name_length > 0);
		cm_get_assert (name_length + 1 + ioc_nos_len <= 240);

		CM_PUT8_HTON (wr_dp, adj - 1, CM_CAST_U8 (name_length + 1 + ioc_nos_len));

		cm_adjust (dp, left, adj);
		block_left -= adj;
		cm_adjust (wr_dp, wr_left, adj);

		cm_get_assert (left >= name_length);
		cm_get_assert (block_left >= name_length);
		cm_get_assert (wr_left >= CM_CAST_U8 (name_length + 1 + ioc_nos_len));

		wr_name_dp = wr_dp;
		CM_MEMCPY    (wr_dp,                   dp, name_length);
		CM_PUT8_HTON (wr_dp, name_length,      '.');
		CM_MEMCPY    (wr_dp + name_length + 1, ioc_nos, ioc_nos_len);

		cm_adjust (dp, left, name_length);
		block_left -= name_length;
		cm_adjust (wr_dp, wr_left, name_length + 1 + ioc_nos_len);

		while (((dp - block_dp) & 0x3) != 0)
		{
			cm_get_assert (left >= 1);
			cm_get_assert (block_left >= 1);
			cm_adjust (dp, left, 1);
			block_left -= 1;
		}

		while (((wr_dp - wr_block_dp) & 0x3) != 0)
		{
			cm_get_assert (wr_left >= 1);
			CM_PUT8_HTON (wr_dp, 0, 0);
			cm_adjust (wr_dp, wr_left, 1);
		}

		/* { [(MrpManagerParams) ^ (MrpClientParams)] } */
		adj = block_left;
		cm_get_assert (wr_left >= adj);

		CM_MEMCPY (wr_dp, dp, adj);

		cm_adjust (dp, left, adj);
		cm_adjust (wr_dp, wr_left, adj);

		/* calc md5 of MRP_DomainName */
		{
		CM_MD5_CTX_TYPE ctx;
		LSA_UINT8  uuid[CM_MD5_DIGEST_LENGTH];

		cm_md5_init   (&ctx);
		cm_md5_update (&ctx, wr_name_dp, name_length + 1 + ioc_nos_len);
		cm_md5_final  (uuid, &ctx);

		CM_MEMCPY (wr_uuid_dp, uuid, CM_MD5_DIGEST_LENGTH);
		}

		/* BlockHeader.BlockLength */
		CM_PUT16_HTON (wr_block_dp, 2, CM_CAST_U16 (wr_dp - wr_block_dp - (2+2)));
	
		nr_of_entries -= 1;
	}

	cm_get_assert (left == 0);

	/* BlockHeader.BlockLength */
	rec->tailor_data_len = CM_CAST_U16(wr_dp - rec->tailor_data);
	CM_PUT16_HTON (rec->tailor_data, 2, CM_CAST_U16 (rec->tailor_data_len - (2+2)));

	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInterfaceMrpDataAdjust Tailor Error in line(%u), record-offset(%u)", cm_get_assert_line, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052,
	CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos,
	LSA_UINT16  ioc_nos_len
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_write_tailor_data")

	LSA_UINT16  rsp = CM_OK;

	switch (rec->record_index)
	{
	case 0x802b: /* PDPortDataCheck */
		{
			CM_PD_MTD_PORT_PTR_TYPE  start_mtdp = LSA_NULL;

			if ((mtd->properties & CM_PD_MTD_PROP_NOS)
			||	(mtd->properties & (CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG)))
			{
				start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);
			}
			if (is_not_null (start_mtdp))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortDataCheck (channel, mtd, start_mtdp, rec, ioc_nos, ioc_nos_len);
			}
		}
		break;

	case 0x802f: /* PDPortDataAdjust */
		{
			CM_PD_MTD_PORT_PTR_TYPE  start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			if (is_not_null (start_mtdp) && (start_mtdp->properties & CM_PD_MTD_PEER_PROP_RED_PORT))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortDataAdjust (channel, start_mtdp, rec);
			}
		}
		break;

	case 0x802d: /* PDSyncData */
		if ((mtd->properties & CM_PD_MTD_PROP_SYNCDATA) &&  (mtd->properties & CM_PD_MTD_PROP_NOS))
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDSyncData (channel, rec, ioc_nos, ioc_nos_len);
		}
		break;

	case 0x8051: /* PDInterfaceMrpDataCheck */
		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_CHECK)
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataCheck (channel, rec, rec_8052);
		}
		break;

	case 0x8052: /* PDInterfaceMrpDataAdjust */
		if (mtd->properties & CM_PD_MTD_PROP_MRP_DATA_ADJUST)
		{
			rsp = cm_pd_mpa_prm_write_tailor_data_PDInterfaceMrpDataAdjust (channel, rec, ioc_nos, ioc_nos_len);
		}
		break;

	case 0x8053: /* PDPortMrpDataAdjust */
		{
			CM_PD_MTD_PORT_PTR_TYPE  start_mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			if (is_not_null (start_mtdp) &&  (start_mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST))
			{
				rsp = cm_pd_mpa_prm_write_tailor_data_PDPortMrpDataAdjust (channel, rec, rec_8052);
			}
		}
		break;

	case 0x802C: /* PDIRData */
		/* bits are checked inside */
		rsp = cm_pd_mpa_prm_write_tailor_data_PDIRData (channel, mtd, rec);
		break;

	default:
		return CM_OK;
	}

	if (rec->tailor_data_len > rec->tailor_data_alloc_len)
	{
		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "tailor_data_len(%u) > tailor_data_alloc_len(%u), record_index(%u)"
			, rec->tailor_data_len, rec->tailor_data_alloc_len, rec->record_index
			);
		CM_FATAL();
	}

	if (rsp != CM_OK)
	{
		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "Failed to write tailor data, subslot(%u/%u/0x%x) record-index(0x%x)", rec->api, rec->slot_nr, rec->subslot_nr, rec->record_index
			);
	}

	return rsp;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_parse_record (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_parse_record")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_COMMON_MEM_U8_PTR_TYPE  dp = rec->record_data;
	LSA_UINT32  left = rec->record_data_len;
	LSA_UINT32  adj;

	/* see SPH Table 1 Data record compendium */

	switch (rec->record_index)
	{
	case 0x802b: /* PDPortDataCheck */
		{
			/*	PDPortDataCheck ::=	BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
			//		, { [CheckPeers], [CheckLineDelay], [CheckMAUType], [CheckLinkState], [CheckSyncDifference], [CheckMAUTypeDifference] }
			*/
			CM_PD_MTD_PORT_PTR_TYPE  mtdp = cm_pd_mtd_lookup_mtdp (mtd, rec->slot_nr, rec->subslot_nr);

			/* is_null() means an "any peer" */

			if (is_not_null (mtdp))
			{
				LSA_UINT16  block_type, block_length, block_version, pad, slot_nr, subslot_nr;

				adj = 2+2+1+1 + 1+1 + 2+2;
				cm_get_assert (left >=  adj);

				block_type			= CM_GET16_NTOH (dp,  0);
				block_length		= CM_GET16_NTOH (dp,  2);
				block_version		= CM_GET16_NTOH (dp,  4);
				pad					= CM_GET16_NTOH (dp,  6);
				slot_nr				= CM_GET16_NTOH (dp,  8);
				subslot_nr			= CM_GET16_NTOH (dp, 10);

				cm_adjust (dp, left, adj);

				cm_get_assert (block_type == CM_BLOCK_TYPE_PD_PORT_DATA_CHECK);
				cm_get_assert (block_length + 2+2  ==  rec->record_data_len);
				cm_get_assert (block_version == 0x0100);
				cm_get_assert (pad == 0);
				cm_get_assert (slot_nr == rec->slot_nr);
				cm_get_assert (subslot_nr == rec->subslot_nr);

				/*	CheckPeers		::= BlockHeader, NumberOfPeers, (LengthPeerPortName, PeerPortName, LengthPeerStationName, PeerStationName)*, [Padding*]
				//	CheckLineDelay	::= BlockHeader, Padding, Padding, LineDelay
				//	CheckMAUType	::= BlockHeader, MAUType
				//	CheckLinkState	::= BlockHeader, LinkState
				//	CheckSyncDifference	::= BlockHeader, CheckSyncMode
				//	CheckMAUTypeDifference ::= BlockHeader, MAUTypeMode
				*/
				while (left > 0)
				{
					LSA_UINT16  nr_of_peers;
					LSA_UINT32  linedelay;

					adj = 2+2;
					cm_get_assert (left >= adj);

					block_type			= CM_GET16_NTOH (dp, 0);
					block_length		= CM_GET16_NTOH (dp, 2);

					cm_adjust (dp, left, adj);

					cm_get_assert (left >= block_length);

					switch (block_type)
					{
					case CM_BLOCK_TYPE_PD_CHK_PEERS:
						cm_get_assert (block_length >= 1+1 + 1);

						block_version	= CM_GET16_NTOH (dp, 0);
						nr_of_peers		= CM_GET8_NTOH  (dp, 2);

						cm_get_assert (block_version == 0x0100);
						cm_get_assert (nr_of_peers == 1);
						break;

					case CM_BLOCK_TYPE_PD_CHK_LINE_DELAY:
						cm_get_assert (block_length == 1+1 + 1+1 + 4);

						block_version	= CM_GET16_NTOH (dp, 0);
						pad				= CM_GET16_NTOH (dp, 2);
						linedelay		= CM_GET32_NTOH (dp, 4);

						cm_get_assert (block_version == 0x0100);
						cm_get_assert (pad == 0);
						cm_get_assert (linedelay != 0);
						cm_get_assert ((linedelay & 0x80000000) == 0); /* LineDelay.FormatIndicator */

						mtdp->check_linedelay = linedelay;
						break;

					default:
						/* ANY, with version ANY */
						break;
					}

					cm_adjust (dp, left, block_length);
				}

				cm_get_assert (left == 0);

				mtdp->properties |= CM_PD_MTD_PEER_PROP_I_PARSED;
			}
		}
		break;

	case 0x802c: /* PDIRData */
		{
			LSA_UINT16  block_type, block_length, block_version, pad, slot_nr, subslot_nr;
			LSA_UINT32  max_bridge_delay, nr_of_ports;

			/*	PDIRData [VersionLow == 1]  ::=
			//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, PDIRFrameData, PDIRBeginEndData
			*/

			adj = 2+2+1+1 + 1+1 + 2+2;
			cm_get_assert (left >=  adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad  				= CM_GET16_NTOH (dp,  6);
			slot_nr				= CM_GET16_NTOH (dp,  8);
			subslot_nr			= CM_GET16_NTOH (dp, 10);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_IR_DATA);
			cm_get_assert (block_length + 2+2  ==  rec->record_data_len);
			cm_get_assert (block_version == 0x0101);
			cm_get_assert (pad == 0);
			cm_get_assert (slot_nr == rec->slot_nr);
			cm_get_assert (subslot_nr == rec->subslot_nr);

			while (left > 0)
			{
				adj = 2+2;
				cm_get_assert (left >= adj);

				block_type			= CM_GET16_NTOH (dp, 0);
				block_length		= CM_GET16_NTOH (dp, 2);

				cm_adjust (dp, left, adj);

				cm_get_assert (left >= block_length);

				switch (block_type)
				{
				case CM_BLOCK_TYPE_PD_IR_GLOBAL_DATA:
					/*	PDIRGlobalData with BlockVersionLow = 1  ::=
					//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, (MaxPortTxDelay, MaxPortRxDelay)*
					//
					//	PDIRGlobalData with BlockVersionLow = 2  ::=
					//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, NumberOfPorts, (MaxPortTxDelay, MaxPortRxDelay, MaxLineRxDelay, YellowTime)*
					*/
					adj = 1+1 + 1+1 + 16 + 4 + 4;
					cm_get_assert (block_length >= adj);

					block_version		= CM_GET16_NTOH (dp,  0);
					pad  				= CM_GET16_NTOH (dp,  2);
					max_bridge_delay	= CM_GET32_NTOH (dp, 20);
					nr_of_ports			= CM_GET32_NTOH (dp, 24);
				
					cm_get_assert (block_version == 0x0101 || block_version == 0x0102);
					cm_get_assert (pad == 0);
					cm_get_assert (1 <= max_bridge_delay && max_bridge_delay <= 0x0000FFFF);
					cm_get_assert (1 <= nr_of_ports && nr_of_ports <= 0x000000FF);

					mtd->pdirglobaldata_version	= block_version;
					mtd->max_bridge_delay		= max_bridge_delay;

					if (block_version == 0x0101)
					{
						cm_adjust (dp, left, block_length);

						/* calculate later as f(LineDelay), see cm_pd_mpa_tailor_pre_calc_tailor_data() */ 
					}
					else 
					{
						LSA_UINT16  i;
						LSA_UINT32  max_line_rx_delay;

						cm_adjust (dp, left, adj);

						adj = 4 + 4 + 4 + 4;
						for (i = 0; i < nr_of_ports; ++i)
						{
							CM_PD_MTD_PORT_PTR_TYPE  mtdp = cm_pd_mtd_lookup_mtdp_from_index (mtd, i);

							cm_get_assert (left >= adj);

							max_line_rx_delay = CM_GET32_NTOH (dp, 8);

							cm_adjust (dp, left, adj);
							cm_get_assert (/* 0x00000000 <= max_line_rx_delay  && */  max_line_rx_delay <= 0x3B9AC9FF);
							/* 0 .. Unknown, No delay, forward as soon as possible */

							if (is_not_null (mtdp))
							{
								mtdp->max_line_rx_delay = max_line_rx_delay;
							}
						}
					}
					break;

				case CM_BLOCK_TYPE_PD_IR_FRAME_DATA:
					/*	PDIRFrameData [VersionLow == 0]  ::=
					//		BlockHeader, Padding, Padding, (FrameSendOffset, ...
					//
					//	PDIRFrameData [VersionLow == 1]  ::=
					//		BlockHeader, Padding, Padding, FrameDataProperties, [ ...
					*/
					cm_get_assert (block_length >= 1+1 + 1+1 + 4);

					block_version		= CM_GET16_NTOH (dp,  0);
					pad  				= CM_GET16_NTOH (dp,  2);

					cm_get_assert (block_version == 0x0100  ||  block_version == 0x0101);
					cm_get_assert (pad == 0);

					cm_adjust (dp, left, block_length);
					break;

				case CM_BLOCK_TYPE_PD_IR_BEGIN_END_DATA:
					/*	PDIRBeginEndData [VersionLow == 0]  ::=
					//		BlockHeader, Padding, Padding, RedGuard, NumberOfPorts, (...
					*/
					cm_get_assert (block_length >= 1+1 + 1+1 + 2+2 + 4);

					block_version		= CM_GET16_NTOH (dp,  0);
					pad  				= CM_GET16_NTOH (dp,  2);

					cm_get_assert (block_version == 0x0100);

					cm_adjust (dp, left, block_length);
					break;

				default:
					/* ANY, with version ANY */
					cm_adjust (dp, left, block_length);
					break;
				}
			}
		}
		break;

	case 0x802d: /* PDSyncData */
		{
			LSA_UINT16  block_type, block_length, block_version, pad;

			/* PDSyncData [BlockVersionLow = 2] ::=
			//		BlockHeader, Padding, Padding, PTCP_SubdomainUUID, ...
			*/

			adj = 2+2+1+1 + 1+1 + 16;
			cm_get_assert (left >=  adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad  				= CM_GET16_NTOH (dp,  6);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_SYNC_DATA);
			cm_get_assert (block_length + 2+2 == rec->record_data_len);
			cm_get_assert (block_version == 0x102);
			cm_get_assert (pad == 0);
		}
		break;

	case 0x802f: /* PDPortDataAdjust */
		{
			LSA_UINT16  block_type, block_length, block_version, pad, slot_nr, subslot_nr;

			/* PDPortDataAdjust ::=
			//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber
			//		, { [AdjustDomainBoundary], [AdjustMulticastBoundary], [AdjustMAUType ^ AdjustLinkState], [AdjustPeerToPeerBoundary]
			//		, [AdjustDCPBoundary], [AdjustPreambleLength] }
			*/

			adj = 2+2+1+1 + 1+1 + 2+2;
			cm_get_assert (left >=  adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad					= CM_GET16_NTOH (dp,  6);
			slot_nr				= CM_GET16_NTOH (dp,  8);
			subslot_nr			= CM_GET16_NTOH (dp, 10);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_PORT_DATA_ADJUST);
			cm_get_assert (block_length + 2+2  ==  rec->record_data_len);
			cm_get_assert (block_version == 0x0100);
			cm_get_assert (pad == 0);
			cm_get_assert (slot_nr == rec->slot_nr);
			cm_get_assert (subslot_nr == rec->subslot_nr);

			while (left > 0)
			{
				adj = 2+2;
				cm_get_assert (left >= adj);

				block_type			= CM_GET16_NTOH (dp, 0);
				block_length		= CM_GET16_NTOH (dp, 2);

				cm_adjust (dp, left, adj);

				cm_get_assert (left >= block_length);

				switch (block_type)
				{
				case CM_BLOCK_TYPE_PD_ADJ_PREAMBLE_LENGTH:
					/*	AdjustPreambleLength	::= BlockHeader, Padding, Padding, PreambleLength, AdjustProperties, [Padding*] a
					*/
					cm_get_assert (block_length == 1+1 + 1+1 + 2 + 2);

					block_version	= CM_GET16_NTOH (dp, 0);

					cm_get_assert (block_version == 0x0100);
					break;

				default:
					/* ANY, with version ANY */
					break;
				}

				cm_adjust (dp, left, block_length);
			}
		}
		break;

	case 0x8051: /* PDInterfaceMrpDataCheck */
		{
			LSA_UINT16  block_type, block_length, block_version, pad, nr_of_entries;

			/* PDInterface-MrpDataCheck [BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Check
			//
			//	PDInterface-MrpDataCheck [BlockVersionLow = 1]  ::=
			//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataCheckBlock*
			*/

			adj = 2+2+1+1 + 1+1 + 16 + 4;
			cm_get_assert (left >= adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad  				= CM_GET8_NTOH  (dp,  6);
			nr_of_entries		= CM_GET8_NTOH  (dp,  7);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_CHECK);
			cm_get_assert (block_length + 2+2 == rec->record_data_len);
			cm_get_assert (block_version == 0x0100  ||  block_version == 0x0101);
			cm_get_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_get_assert (nr_of_entries == 0);
			}
			else
			{
				cm_get_assert (1 <= nr_of_entries && nr_of_entries <= 127);
			}
		}
		break;

	case 0x8052: /* PDInterfaceMrpDataAdjust */
		{
			LSA_UINT16  block_type, block_length, block_version, pad, nr_of_entries;

			/* PDInterface-MrpDataAdjust [BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Role, [Padding*] a
			//		, MRP_LengthDomainName, MRP_DomainName, [Padding*] a, { [(MrpManagerParams) ^ (MrpClientParams)] }
			//
			//	PDInterface-MrpDataAdjust [BlockVersionLow = 1] ::=
			//		BlockHeader, Padding, MRP_NumberOfEntries, MrpInstanceDataAdjustBlock*
			*/

			adj = 2+2+1+1 + 1+1 + 16 + 2 + 2 + 1;
			cm_get_assert (left >= adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad  				= CM_GET8_NTOH  (dp,  6);
			nr_of_entries		= CM_GET8_NTOH  (dp,  7);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
			cm_get_assert (block_length + 2+2 == rec->record_data_len);
			cm_get_assert (block_version == 0x0100  ||  block_version == 0x0101);
			cm_get_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_get_assert (nr_of_entries == 0);
			}
			else
			{
				cm_get_assert (1 <= nr_of_entries && nr_of_entries <= 127);
			}
		}
		break;

	case 0x8053: /* PDPortMrpDataAdjust */
		{
			LSA_UINT16  block_type, block_length, block_version, pad, instance;

			/* PDPortMrpDataAdjust with BlockVersionLow = 0] ::=
			//		BlockHeader, Padding, Padding, MRP_DomainUUID
			//
			//	PDPortMrpDataAdjust [BlockVersionLow = 1]  ::=
			//		BlockHeader, Padding, MRP_Instance, MRP_DomainUUID
			*/

			adj = 2+2+1+1 + 1+1 + 16;
			cm_get_assert (left >= adj);

			block_type			= CM_GET16_NTOH (dp,  0);
			block_length		= CM_GET16_NTOH (dp,  2);
			block_version		= CM_GET16_NTOH (dp,  4);
			pad  				= CM_GET8_NTOH  (dp,  6);
			instance			= CM_GET8_NTOH  (dp,  7);

			cm_adjust (dp, left, adj);

			cm_get_assert (block_type == CM_BLOCK_TYPE_PD_MRP_PORT_ADJ);
			cm_get_assert (block_length + 2+2 == rec->record_data_len);
			cm_get_assert (block_version == 0x0100  ||  block_version == 0x0101);
			cm_get_assert (pad == 0);
			if (block_version == 0x0100)
			{
				cm_get_assert (instance == 0);
			}
			else
			{
				cm_get_assert (/*0 <= instance &&*/ instance <= 126);
			}
		}
		break;

	default:
		break;
	}
	return CM_OK;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "Parse Error in line(%u): index(0x%x) slot(%u) subslot(0x%x), record-offset(%u)"
		, cm_get_assert_line, rec->record_index, rec->slot_nr, rec->subslot_nr, rec->record_data_len - left
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_prealloc_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	)
{
	CM_LOG_FCT("cm_pd_mpa_prm_prealloc_tailor_data")
	LSA_UINT32  alloc_length;

	switch(rec->record_index)
	{
	case 0x802b: /* PDPortDataCheck */
		alloc_length = rec->record_data_len + 240; /* NrOfPeers has to be exactly 1, no need to parse CheckPeers */
		break;

	case 0x802f: /* PDPortDataAdjust */
		alloc_length = rec->record_data_len;
		break;

	case 0x802d: /* PDSyncData */
		alloc_length = rec->record_data_len + 240; /* PTCPSubdomainName */
		break;

	case 0x8051: /* PDInterfaceMrpDataCheck */
		alloc_length = rec->record_data_len;
		break;

	case 0x8052: /* PDInterfaceMrpDataAdjust */
		if (rec->record_data_len > 2+2+1+1 + 1 + 1)
		{
			LSA_UINT16  block_version = CM_GET16_NTOH (rec->record_data, 4);
			LSA_UINT8   nr_of_entries = CM_GET8_NTOH  (rec->record_data, 7);

			if (block_version == 0x0100)
			{
				alloc_length = rec->record_data_len + 240; /* MRP_DomainName */
			}
			else if (block_version == 0x0101)
			{
				if (1 <= nr_of_entries  && nr_of_entries <= 127)
				{
					alloc_length = rec->record_data_len + nr_of_entries * 240; /* MRP_DomainName */
				}
				else
				{
					return CM_ERR_RESOURCE;
				}
			}
			else
			{
				return CM_ERR_RESOURCE;
			}
		}
		else
		{
			return CM_ERR_RESOURCE;
		}
		break;

	case 0x8053: /* PDPortMrpDataAdjust */
		alloc_length = rec->record_data_len;
		break;

	case 0x802C: /* PDIRData */
		alloc_length = rec->record_data_len; 
		break;

	default:
		alloc_length = 0;
		break;
	}

	if (alloc_length > 0)
	{
		LSA_UINT16  len_u16 = (LSA_UINT16)alloc_length;

		if (alloc_length > 0xFFFF)
		{
			return CM_ERR_RESOURCE;
		}
		if (is_not_null (channel))
		{
			/* usecase cmpd */
			CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR(&rec->tailor_data), cm_null_user_id, len_u16, channel->sysptr);
		}
		else
		{
			/* usecase cmcl */
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(&rec->tailor_data), len_u16);
		}
		if (is_null (rec->tailor_data))
		{
			return CM_ERR_RESOURCE;
		}
		rec->tailor_data_alloc_len = len_u16;
		rec->tailor_data_len       = CM_TAILOR_DATA_LEN_NOT_TAILORED;
	}
	else
	{
		rec->tailor_data           = LSA_NULL;
		rec->tailor_data_alloc_len = 0;
		rec->tailor_data_len       = CM_TAILOR_DATA_LEN_NOT_TAILORED;
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_write_internal_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_write_internal_free")
	LSA_UINT16  rc;

	CM_ASSERT (CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_WRITE || CM_RQB_GET_OPCODE (rb) == CM_OPC_PD_PRM_WRITE_SERVER);
	CM_ASSERT (CM_RQB_GET_HANDLE (rb) == CM_INVALID_HANDLE);

	CM_FREE_UPPER_MEM (& rc, rb->args.pd.prm_rdwr->record_data, channel->sysptr);
	CM_ASSERT (rc == CM_OK);

	cm_upper_free_rqb (channel, rb);
}

/*----------------------------------------------------------------------------*/
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_send_clock_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_IF_PTR_TYPE  port_if,
	LSA_BOOL is_prm_server
	)
{
	CM_LOG_FCT("cm_pd_write_send_clock_alloc")
	CM_UPPER_RQB_PTR_TYPE  rb;
	LSA_UINT16  record_data_length;
	CM_UPPER_MEM_PTR_TYPE  record_data;
	LSA_UINT16  send_clock;
	LSA_UINT16  sendclock_prop;


	rb = cm_upper_alloc_rqb (channel);
	if (is_null (rb))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	/* SendClock ::= BlockHeader, SendClockFactor, SendClockProperties, Reserved, [Data*] */

	record_data_length = 2+2+1+1 + 2+2+2;

	CM_ALLOC_UPPER_MEM (& record_data, cm_null_user_id, record_data_length, channel->sysptr);
	if (is_null (record_data))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	if( is_prm_server )
	{

		/* note: the SendClock-record does not come via RPC */
		CM_ASSERT(channel->usr.pd.clock.send_clock != 0);
		send_clock = channel->usr.pd.clock.send_clock; /* must be self-fulfilling, see cm_pdusr_attach() */
		sendclock_prop = cm_pd_sendclock_prop_get (channel); /* must keep what the startup-parameterization said */
	}
	else {

		/* TIA 612541 (V5.3i2.15) updates AP00914550 and AP01353701
		 *
		 * if the user does not write a SendClock-record, the following rules apply:
		 *
		 * EDDS: SC=FIX, RR=VAR
		 * - SC=FIX because the send-clock cannot be changed at runtime
		 * - RR=VAR because otherwise an AR with another send-clock cannot be established (e.g., PDEV has SC=32ms and the AR has SC=1ms and RR=32)
		 * - note that this contradicts AP00914550 (no automatic RR-adaption)
		 *
		 * EDDI/EDDP: SC=VAR, RR=FIX
		 * - SC=VAR because the send-clock can be changed at runtime
		 * - RR=FIX because automatic RR-adaption is not desired (AP00914550, V4.2i2.4)
		 *
		 */

		if( ! cm_edd_can_set_send_clock(channel) )
		{
			send_clock = channel->usr.pd.clock.send_clock; /* unchanged */
			sendclock_prop = CM_SENDCLOCK_PROP_SC_FIX_RR_VAR; /* TIA 612541 (was SC_FIX_RR_FIX) */
		}
		else {
			send_clock = 32; /* default to 1 ms */
			sendclock_prop = CM_SENDCLOCK_PROP_SC_VAR_RR_FIX;
		}
	}

	CM_PUT16_HTON (record_data, 0, CM_BLOCK_TYPE_SEND_CLOCK);
	CM_PUT16_HTON (record_data, 2, CM_CAST_U16(record_data_length - (2+2)));
	CM_PUT16_HTON (record_data, 4, 0x0100);
	CM_PUT16_HTON (record_data, 6, send_clock);
	CM_PUT16_HTON (record_data, 8, sendclock_prop);
	CM_PUT16_HTON (record_data,10, 0x0000);

	{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	prm_rdwr->slot_nr            = port_if->port.addr.slot_nr;
	prm_rdwr->subslot_nr         = port_if->port.addr.subslot_nr;
	prm_rdwr->record_index       = 0x00010000;
	prm_rdwr->record_data_length = record_data_length;
	prm_rdwr->record_data        = CM_CAST_COMMON_MEM_U8_PTR (record_data);

	CM_RQB_SET_HANDLE (rb, CM_INVALID_HANDLE);

	if( is_prm_server )
	{
		CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PRM_WRITE_SERVER);
		prm_rdwr->mk_remanent = LSA_TRUE;
	}
	else
	{
		CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PRM_WRITE);
		prm_rdwr->mk_remanent = LSA_FALSE;
	}

	prm_rdwr->is_local = LSA_TRUE;
	prm_rdwr->cls = cm_record_index_classify(0/*!*/, prm_rdwr->record_index, CM_VENDOR_ID_DONTCARE);
	}

	return rb;
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_pd_sync_pll_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_IF_PTR_TYPE  port_if
	)
{
	CM_LOG_FCT("cm_pd_write_pd_sync_pll_alloc")
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;
	CM_UPPER_RQB_PTR_TYPE  rb;
	LSA_UINT16  record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE  record_data;

	/***/

	rb = cm_upper_alloc_rqb (channel);
	if (is_null (rb))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	record_data_length = 2+2+1+1 + 2+2+2+4+2+2;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR(& record_data)
		, cm_null_user_id, record_data_length, channel->sysptr
		);
	if (is_null (record_data))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	CM_PUT16_HTON (record_data, 0, CM_BLOCK_TYPE_PD_SYNC_PLL);
	CM_PUT16_HTON (record_data, 2, CM_CAST_U16(record_data_length - (2+2)));
	CM_PUT16_HTON (record_data, 4, 0x0101); /* v1.1 */
	CM_PUT16_HTON (record_data, 6, reclist->prm_data.pll_reduction_ratio);
	CM_PUT16_HTON (record_data, 8, reclist->prm_data.pll_hw_delay);
	CM_PUT16_HTON (record_data,10, reclist->prm_data.pll_cntrl_interval);
	CM_PUT32_HTON (record_data,12, reclist->prm_data.pll_window);
	CM_PUT16_HTON (record_data,16, reclist->prm_data.pll_setwaitcount);
	CM_PUT16_HTON (record_data,18, 0/*reserved*/);

	{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	prm_rdwr->slot_nr            = port_if->port.addr.slot_nr;
	prm_rdwr->subslot_nr         = port_if->port.addr.subslot_nr;
	prm_rdwr->record_index       = 0x00011020; /* PDSyncPLL */
	prm_rdwr->record_data_length = record_data_length;
	prm_rdwr->record_data        = record_data;

	CM_RQB_SET_HANDLE (rb, CM_INVALID_HANDLE);
	CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PRM_WRITE_SERVER);

	prm_rdwr->mk_remanent = LSA_TRUE;
	prm_rdwr->is_local = LSA_TRUE;
	prm_rdwr->cls = cm_record_index_classify(0/*!*/, prm_rdwr->record_index, CM_VENDOR_ID_DONTCARE);
	}

	return rb;
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
static CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_write_mrp_interface_adjust_alloc (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port_if,
	LSA_BOOL is_prm_server
	)
{
	CM_LOG_FCT("cm_pd_write_mrp_interface_adjust_alloc")
	CM_UPPER_RQB_PTR_TYPE  rb;
	LSA_UINT16  record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE  record_data;
	CLRPC_UUID_TYPE  mrp_uuid;

	/***/

	rb = cm_upper_alloc_rqb (channel);
	if (is_null (rb))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	record_data_length = 2+2+1+1 +1+1 + 16 + 2 + 2/*pad*/ + 1+17 + 2/*pad*/;

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR(& record_data)
		, cm_null_user_id, record_data_length, channel->sysptr
		);
	if (is_null (record_data))
	{
		CM_FATAL(); /* no mem */
		return LSA_NULL;
	}

	/* PDInterfaceMrpDataAdjust ::=
	**	BlockHeader, Padding, Padding, MRP_DomainUUID, MRP_Role, [Padding*] a
	**	, MRP_LengthDomainName_u8, MRP_DomainName, [Padding*] a, ...
	**
	**  a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
	**
	**	MRP_Role             .. OFF ==0
	**	MRP_DomainName       .. default-mrpdomain
	**	MRP_LengthDomainName .. 17
	*/

	CM_MEMSET (& mrp_uuid, 0xFF, sizeof(mrp_uuid));

	CM_PUT16_HTON (record_data, 0, CM_BLOCK_TYPE_PD_MRP_INTERFACE_ADJ);
	CM_PUT16_HTON (record_data, 2, CM_CAST_U16(record_data_length - (2+2)));
	CM_PUT16_HTON (record_data, 4, 0x0100);
	CM_PUT16_HTON (record_data, 6, 0x0000);

	CM_PUT_UUID_HTON (record_data,  8,  & mrp_uuid);
	CM_PUT16_HTON    (record_data, 24,  0);
	CM_PUT16_HTON    (record_data, 26,  0);
	CM_PUT8_HTON     (record_data, 28, 17);
	CM_MEMCPY        (record_data+ 29, CM_CAST_LOCAL_MEM_PTR("default-mrpdomain"), 17);
	CM_PUT16_HTON    (record_data, 46,  0);

	{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	prm_rdwr->slot_nr            = port_if->addr.slot_nr;
	prm_rdwr->subslot_nr         = port_if->addr.subslot_nr;
	prm_rdwr->record_index       = 0x8052; /* PDInterfaceMrpDataAdjust */
	prm_rdwr->record_data_length = record_data_length;
	prm_rdwr->record_data        = record_data;

	CM_RQB_SET_HANDLE (rb, CM_INVALID_HANDLE);

	if( is_prm_server )
	{
		CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PRM_WRITE_SERVER);
		prm_rdwr->mk_remanent = LSA_TRUE;
	}
	else {
		CM_RQB_SET_OPCODE (rb, CM_OPC_PD_PRM_WRITE);
		prm_rdwr->mk_remanent = LSA_FALSE;
	}

	prm_rdwr->is_local = LSA_TRUE;
	prm_rdwr->cls = cm_record_index_classify(0/*!*/, prm_rdwr->record_index, CM_VENDOR_ID_DONTCARE);
	}

	return rb;
}
#endif

/*------------------------------------------------------------------------------
//	PDMasterTailorData
//----------------------------------------------------------------------------*/

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  local_device_nr,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data
	)
{
	CM_LOG_FCT("cm_pd_mtd_parse")
	LSA_UNUSED_ARG(channel);

#if CM_CFG_MAX_CLIENTS
	{
		/* see PDMasterTailorData */

		LSA_UINT16  block_type, block_length, block_version, tailor_properties, dev_group, port_cnt;
		LSA_UINT16  rc;
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd;
		CM_PD_MTD_PORT_PTR_TYPE  ports;
		LSA_UINT16  count_of_fixed;

		if (record_data_length <  0UL + 2+2+1+1 + 2+2+2) /* size without any ports */
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDMasterTailorData: record_data_length(%u)", record_data_length);
			return LSA_NULL;
		}

		block_type			= CM_GET16_NTOH (record_data,  0);
		block_length		= CM_GET16_NTOH (record_data,  2);
		block_version		= CM_GET16_NTOH (record_data,  4);
		tailor_properties	= CM_GET16_NTOH (record_data,  6);
		dev_group			= CM_GET16_NTOH (record_data,  8);
		port_cnt			= CM_GET16_NTOH (record_data, 10);

		if (record_data_length <  0UL + 2+2+1+1 + 2+2+2 + (port_cnt * (2+2+2+2+2+2))) /* size including ports */
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PDMasterTailorData: record_data_length(%u)", record_data_length);
			return LSA_NULL;
		}

		if (block_length + 2+2 != (LSA_INT)record_data_length
		||  block_type != 0x7081 /*BlockType(PDMasterTailorData)*/
		||  block_version != 0x0100
		||  dev_group != 0)
		{
			CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDMasterTailorData: block_length(%u) block_type(0x%x) block_version(0x%x) dev_group(0x%x)"
				, block_length, block_type, block_version, dev_group
				);
			return LSA_NULL;
		}

		if ((tailor_properties & CM_PD_MTD_PROP_RESERVED) != 0)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDMasterTailorData: reserved tailor_properties(0x%x) set", tailor_properties
				);
			return LSA_NULL;
		}

		/* both flags are required CM_PD_MTD_PROP_MRP_DATA_ADJUST + CM_PD_MTD_PROP_MRP_DATA_CHECK */
		if ((tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) != 0 && (tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) != CM_PD_MTD_PROP_I_MRP_DATA_MASK)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PDMasterTailorData: mrp data config invalid, tailor_properties (0x%x)", tailor_properties
				);
			return LSA_NULL;
		}

		count_of_fixed = 0;

		if (port_cnt > 0)
		{
			LSA_UINT16  mtdp_index;
			LSA_UINT16  offset         = 2+2+1+1 + 2+2+2;

			LSA_BOOL    mtdp_valid = LSA_TRUE;

			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(&ports), sizeof(*ports) * port_cnt);
			if (is_null(ports))
			{
				CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "no local-mem, size(%u)", sizeof(*ports) * port_cnt);
				return LSA_NULL;
			}

			for (mtdp_index = 0; mtdp_index < port_cnt; mtdp_index++)
			{
				CM_PD_MTD_PORT_PTR_TYPE mtdp = &ports[mtdp_index];

				mtdp->local_station_nr	= local_device_nr; /* backlink */
				mtdp->local_slot_nr		= CM_GET16_NTOH(record_data, offset+ 0);
				mtdp->local_subslot_nr	= CM_GET16_NTOH(record_data, offset+ 2);
				mtdp->properties		= CM_GET16_NTOH(record_data, offset+ 4);
				mtdp->peer_station_nr	= CM_GET16_NTOH(record_data, offset+ 6);
				mtdp->peer_slot_nr		= CM_GET16_NTOH(record_data, offset+ 8);
				mtdp->peer_subslot_nr	= CM_GET16_NTOH(record_data, offset+10);

				mtdp->check_linedelay		= 0;
				mtdp->max_line_rx_delay		= 0;

				mtdp->real_station_nr			= 0;
				mtdp->real_slot_nr				= 0;
				mtdp->real_subslot_nr			= 0;
				mtdp->real_properties			= 0;
				mtdp->real_max_line_rx_delay	= 0;

				if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_FIXED
				||  (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
				{
					if (mtdp->peer_station_nr > CM_CFG_MAX_CL_DEVICES)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_station_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

					if (mtdp->peer_slot_nr > 0x7FFF)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

					if (! CM_IS_PDEV_SUBSLOT_NR (mtdp->peer_subslot_nr))
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

					count_of_fixed += 1;
					if (tailor_properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL  &&  count_of_fixed > 2)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: for optional devices at most 2 fixed/optional peers allowed, index(0x%x) properties(0x%x)"
							, mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

				}

				if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
				||  (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_ANY)
				{
					if (mtdp->peer_station_nr != 0xFFFF)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_station_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

					if (mtdp->peer_slot_nr != 0xFFFF)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}

					if (mtdp->peer_subslot_nr != 0xFFFF)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: peer_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}
				}

				if (mtdp->local_slot_nr > 0x7FFF)
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: local_slot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (! CM_IS_PDEV_SUBSLOT_NR (mtdp->local_subslot_nr))
				{
					CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "PortMasterTailorData: local_subslot_nr invalid, index(0x%x) properties(0x%x)", mtdp_index, mtdp->properties
						);
					mtdp_valid = LSA_FALSE;
				}

				if (mtdp->properties & CM_PD_MTD_PEER_PROP_MRP_DATA_ADJUST)
				{
					if ((tailor_properties & CM_PD_MTD_PROP_I_MRP_DATA_MASK) == 0)
					{
						CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "PortMasterTailorData: tailor properties flag TailorPDInterfaceMrpDataAdjust is required, index(0x%x) properties(0x%x)"
							, mtdp_index, mtdp->properties
							);
						mtdp_valid = LSA_FALSE;
					}
				}

				if (! mtdp_valid)
				{
					CM_FREE_LOCAL_MEM(&rc, ports);
					CM_ASSERT (rc == LSA_RET_OK);
					return LSA_NULL;
				}

				if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
				{
					tailor_properties |= CM_PD_MTD_PROP_I_ANY_PEER_OPT;
				}
				if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG)
				{
					tailor_properties |= CM_PD_MTD_PROP_I_ANY_PEER_PROG;
				}

				offset += 2+2+2+2+2+2;
			}
		}
		else
		{
			ports = LSA_NULL;
		}

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(&mtd), sizeof(*mtd));
		if (is_null (mtd))
		{
			if (is_not_null (ports))
			{
				CM_FREE_LOCAL_MEM (&rc, ports);
			}
			return LSA_NULL;
		}

		mtd->properties = tailor_properties;
		mtd->port_cnt = port_cnt;
		mtd->ports = ports;
		mtd->set_by_itd = 0;
		mtd->pdirglobaldata_version = 0;
		mtd->max_bridge_delay = 0;
		mtd->frame_id[0] = 0;
		mtd->frame_id[1] = 0;

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PDMasterTailorData: successfully parsed: tailor_properties(0x%x)", tailor_properties
			);
		return mtd;
	}
#else
	LSA_UNUSED_ARG(local_device_nr);
	LSA_UNUSED_ARG(record_data_length);
	LSA_UNUSED_ARG(record_data);

	return LSA_NULL; /* usecase: IO Device */
#endif
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_lookup_mtdp (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
	)
{
	LSA_INT i;
	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[i];

		if (mtdp->local_slot_nr == slot_nr && mtdp->local_subslot_nr == subslot_nr)
		{
			return mtdp;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_lookup_mtdp_from_index (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	LSA_UINT16  index /* 0 .. (port_cnt - 1) */
	)
{
	LSA_INT i;
	LSA_UINT16  subslot_nr = 0x8000 + ((index + 1) & 0xFF);

	/* SPHi13: 4.4.7.5 Modular switches */

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[i];

		if ((mtdp->local_subslot_nr & 0xF0FF) == subslot_nr)
		{
			return mtdp;
		}
	}
	return LSA_NULL;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_mtd_neighbour_exists (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	CM_LOG_FCT("cm_pd_mtd_neighbour_exists")
	LSA_INT  i;

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE mtdp = &mtd->ports[i];

		{
			LSA_UINT16  peer_station_nr, peer_slot_nr, peer_subslot_nr, peer_properties;

			cm_pd_mtdp_get_peer (mtdp, &peer_station_nr, &peer_slot_nr, &peer_subslot_nr, &peer_properties);

			if (CM_IS_PDEV_SUBSLOT_NR (peer_subslot_nr))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  peer_mtd = cm_pdusr_mpa_lookup_mtd (usr_channel, peer_station_nr);

				if (is_null (peer_mtd))
				{
					CM_PD_TRACE_02 (usr_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "PreCheck-MTD: dev(%u) needs not (yet) existing neighbour-dev(%u)", mtdp->local_station_nr, peer_station_nr
						);
					return LSA_FALSE;
				}
			}
		}
	}

	return LSA_TRUE;
}

#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtd_reset (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	LSA_INT i;

	mtd->set_by_itd = CM_PD_MTD_SET_IS_TAILORED;

	for (i = 0; i < mtd->port_cnt; ++i)
	{
		CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[i];
		if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG)
		{
			mtdp->peer_station_nr = 0xFFFF;
			mtdp->peer_slot_nr = 0xFFFF;
			mtdp->peer_subslot_nr = 0xFFFF;
		}
		mtdp->real_station_nr = 0;
		mtdp->real_slot_nr = 0;
		mtdp->real_subslot_nr = 0;
		mtdp->real_properties = 0;
		mtdp->real_max_line_rx_delay = 0;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtd_set (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	LSA_UINT16  set
	)
{
	if (set == CM_PD_MTD_SET_DEVICE_ENABLED)
	{
		mtd->set_by_itd |=  CM_PD_MTD_SET_DEVICE_ENABLED;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtd_peer (
	CM_PD_MTD_PORT_PTR_TYPE  mtdp_1,
	CM_PD_MTD_PORT_PTR_TYPE  mtdp_2
	)
{
	/* note: do not implizit enable station_nr */

	mtdp_1->peer_station_nr = mtdp_2->local_station_nr;
	mtdp_1->peer_slot_nr    = mtdp_2->local_slot_nr;
	mtdp_1->peer_subslot_nr = mtdp_2->local_subslot_nr;

	mtdp_2->peer_station_nr = mtdp_1->local_station_nr;
	mtdp_2->peer_slot_nr    = mtdp_1->local_slot_nr;
	mtdp_2->peer_subslot_nr = mtdp_1->local_subslot_nr;
}

#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtdp_get_peer (
	CM_PD_MTD_PORT_PTR_TYPE  mtdp,
	LSA_UINT16				 *peer_station_nr,
	LSA_UINT16				 *peer_slot_nr,
	LSA_UINT16				 *peer_subslot_nr,
	LSA_UINT16				 *properties
)
{
	if (is_null(mtdp))
	{
		*peer_station_nr	= 0xFFFF;
		*peer_slot_nr		= 0xFFFF;
		*peer_subslot_nr	= 0xFFFF;
		*properties			= 0;
		return;
	}

	switch (mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK)
	{
	case CM_PD_MTD_PEER_PROP_PEER_FIXED:
		*peer_station_nr	= mtdp->peer_station_nr;
		*peer_slot_nr		= mtdp->peer_slot_nr;
		*peer_subslot_nr	= mtdp->peer_subslot_nr;
		*properties			= mtdp->properties;
		break;

	case CM_PD_MTD_PEER_PROP_PEER_OPT:
	case CM_PD_MTD_PEER_PROP_PEER_PROG:
		*peer_station_nr	= mtdp->real_station_nr;
		*peer_slot_nr		= mtdp->real_slot_nr;
		*peer_subslot_nr	= mtdp->real_subslot_nr;
		*properties			= mtdp->real_properties;
		break;

	case CM_PD_MTD_PEER_PROP_PEER_ANY:
	default:
		*peer_station_nr	= 0xFFFF;
		*peer_slot_nr		= 0xFFFF;
		*peer_subslot_nr	= 0xFFFF;
		*properties			= mtdp->properties;
		break;
	}
}

#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_lookup_neighbour_peer (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	CM_PD_MTD_PORT_PTR_TYPE  start_mtdp,

	CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_MEM_ATTR  *peer_mtdp,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  *tailored_maxlinerxdelay
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  start_mtd = cm_pdusr_mpa_lookup_mtd (usr_channel, start_mtdp->local_station_nr);

	switch (start_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK)
	{
	case CM_PD_MTD_PEER_PROP_PEER_ANY:
		return LSA_FALSE; /* adaption not necessary, no neighbour */

	case CM_PD_MTD_PEER_PROP_PEER_FIXED:
		*tailored_maxlinerxdelay = start_mtdp->max_line_rx_delay;
		*peer_mtdp = start_mtdp;
		return LSA_TRUE; /* adaption not necessary, this is the neighbour */

	case CM_PD_MTD_PEER_PROP_PEER_PROG:
		if (! (start_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
		{
			return LSA_FALSE; /* adaption missing */
		}
		if (! CM_IS_PDEV_SUBSLOT_NR (start_mtdp->peer_subslot_nr))
		{
			return LSA_FALSE; /* adaption missing */
		}
		*tailored_maxlinerxdelay = start_mtdp->max_line_rx_delay;
		*peer_mtdp = start_mtdp;
		return LSA_TRUE; /* adaption given, this is the neighbour */
				
	case CM_PD_MTD_PEER_PROP_PEER_OPT:
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  neighbour_mtd = cm_pdusr_mpa_lookup_mtd (usr_channel, start_mtdp->peer_station_nr);

			if (is_null (neighbour_mtd))
			{
				return LSA_FALSE; /* adaption missing, device not added */
			}
			if (! (neighbour_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
			{
				return LSA_FALSE; /* adaption missing */
			}
			if (neighbour_mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED)
			{
				*tailored_maxlinerxdelay = start_mtdp->max_line_rx_delay;
				*peer_mtdp = start_mtdp;
				return LSA_TRUE; /* adaption given, this is the neighbour */
			}
			/* adaption give, disabled */
			{
				CM_PD_MTD_PORT_PTR_TYPE  backlink_mtdp = cm_pd_mtd_lookup_mtdp (neighbour_mtd, start_mtdp->peer_slot_nr, start_mtdp->peer_subslot_nr);

				if (is_not_null (backlink_mtdp))
				{
					/* search opposite of backlink_mtdp */
					LSA_INT i;
					for (i = 0; i < neighbour_mtd->port_cnt; ++i)
					{
						CM_PD_MTD_PORT_PTR_TYPE  opposite_mtdp = & neighbour_mtd->ports[i];

						if (are_equal (backlink_mtdp, opposite_mtdp))
						{
							continue;
						}

						/* note: optional devices allows only "2 fixed neighbours" (mandatory or optional) */
						if ((opposite_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_FIXED
						||	(opposite_mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
						{
							CM_PD_MTD_PORT_PTR_TYPE  found_mtdp;
							if (cm_pdusr_mpa_lookup_neighbour_peer (usr_channel, opposite_mtdp, & found_mtdp, tailored_maxlinerxdelay))
							{
								if (*tailored_maxlinerxdelay > 0)
								{
									*tailored_maxlinerxdelay += neighbour_mtd->max_bridge_delay + opposite_mtdp->max_line_rx_delay;
								}
								*peer_mtdp = found_mtdp; /* adaption given, this is the neighbour */
								return LSA_TRUE;
							}
							else
							{
								return LSA_FALSE; /* adaption given, no neighbour */
							}
						}
					}
					return LSA_FALSE; /* adaption given, no neighbour */
				}
				return LSA_FALSE; /* adaption given, PDMasterTailorData config error */
			}
		}

	default:
		CM_FATAL();
		return LSA_FALSE;
	}
}

#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtd_free (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE * mtd_ptr_ptr
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = *mtd_ptr_ptr;
	LSA_UINT16 rc;

	if (is_not_null (mtd))
	{
		if (is_not_null (mtd->ports))
		{
			CM_FREE_LOCAL_MEM (&rc, mtd->ports);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_LOCAL_MEM (&rc, mtd);
		CM_ASSERT (rc == LSA_RET_OK);

		*mtd_ptr_ptr = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_is_device_added (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  device_nr
	)
{
	if (device_nr == 0) /* controller is always added */
	{
		return LSA_TRUE;
	}

#if CM_CFG_MAX_CLIENTS
	{
		CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_CLIENT);

		if (is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE  dev = cm_cl_dev_from_device_nr (cl_channel, device_nr);
			if (is_not_null (dev))
			{
				return LSA_TRUE;
			}
		}
	}
#else
	LSA_UNUSED_ARG (usr_channel);
#endif
	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_lookup_mtd (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  device_nr
	)
{
	if (device_nr == 0)
	{
		CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_PD);
		return pd_channel->usr.pd.reclist.mtd;
	}
	else
	{
#if CM_CFG_MAX_CLIENTS
		CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_CLIENT);

		if (is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE  dev = cm_cl_dev_from_device_nr (cl_channel, device_nr);
			if (is_not_null (dev))
			{
				CM_AR_GRAPH_PTR_TYPE  argr;
				cm_cl_dev_fetch_single__nn (dev, &argr);
				return argr->cm.cl.mtd;
			}
		}
		return LSA_NULL;
#else
		return LSA_NULL;
#endif
	}
}


/*----------------------------------------------------------------------------*/
CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_lookup_mtdp (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  device_nr,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE CM_LOCAL_MEM_ATTR  *mtd_
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (usr_channel, device_nr);
	CM_PD_MTD_PORT_PTR_TYPE  mtdp;

	if (is_null (mtd))
	{
		mtdp = LSA_NULL;
	}
	else
	{
		mtdp = cm_pd_mtd_lookup_mtdp (mtd, slot_nr, subslot_nr);
		if (is_null (mtdp))
		{
			mtd = LSA_NULL;
		}
	}
	if (is_not_null (mtd_))
	{
		*mtd_ = mtd;
	}
	return mtdp;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_tailor_frame_id (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  rtc3_frame_id
	)
{
#if CM_CFG_MAX_CLIENTS
	{
		CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (usr_channel, CM_PATH_TYPE_CLIENT);

		if (is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE  dev;

			if (CM_REDUNDANT_FRAME_ID_BASE (rtc3_frame_id) != 0)
			{
				rtc3_frame_id = CM_REDUNDANT_FRAME_ID_BASE (rtc3_frame_id);
			}

			for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);
				if (is_not_null (mtd))
				{
					if (mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL
					&&	mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED
					&&	! (mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED))
					{
						if (mtd->frame_id[0] == rtc3_frame_id  ||  mtd->frame_id[1] == rtc3_frame_id)
						{
							return LSA_TRUE;
						}
					}
				}
			}
		}
	}
#else
	LSA_UNUSED_ARG (usr_channel);
	LSA_UNUSED_ARG (rtc3_frame_id);
#endif

	return LSA_FALSE;
}


/*------------------------------------------------------------------------------
//	read record: PDIOSystemInfo
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_PDIOSystemInfo (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  edd_port_id,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  *record_data_length,
	CM_UPPER_MEM_PTR_TYPE  record_data
	)
{
	CM_LOG_FCT("cm_pd_prm_read_PDIOSystemInfo")
	LSA_UINT16  cm_put_assert_line;
	CM_COMMON_MEM_U8_PTR_TYPE  dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;
	LSA_UINT32  left = *record_data_length;
	LSA_UINT16  device_nr, device_nr_max;
	LSA_UINT32  adj;

	if (edd_port_id != 0/*interface*/)
	{
		return CM_ERR_PRM_INDEX;
	}

	device_nr_max = 0;
#if CM_CFG_MAX_CLIENTS
	{
		CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);
		if (is_not_null (cl_channel))
		{
			device_nr_max = CM_CFG_MAX_CL_DEVICES;
		}
	}
#endif


	/*	PDIOSystemInfo ::= BlockHeader, IOSystemProperties, NumberOfDevices, (StationNumber, DeviceProperties)*
	//	IOSystemProperties
	//		Bit 0: HasOptionalRTC3Devices
	//	DeviceProperties
	//		Bit 0: IsPartOfAddressTailoring
	//		Bit 1: IsOptional
	//		Bit 2: HasFixedPortToOptionalNeighbor
	//		Bit 3: HasProgrammablePeer
	*/

	{
		LSA_UINT16  number_of_devices = 0;
		LSA_UINT16	iosystem_properties = 0;

		adj = 2+2+1+1 + 2 + 2;
		cm_put_assert (left >= adj);

		CM_PUT16_HTON (dp,  0, 0x7082 /* BlockType: PDIOSystemInfo */);
		CM_PUT16_HTON (dp,  2, 0xCCCC);
		CM_PUT16_HTON (dp,  4, 0x0100);

		CM_PUT16_HTON (dp,  6, 0xCCCC);
		CM_PUT16_HTON (dp,  8, 0xCCCC);

		cm_adjust (dp, left, adj);

		for (device_nr = 0; device_nr <= device_nr_max; ++device_nr)
		{
			if (cm_pdusr_mpa_is_device_added (channel, device_nr))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE mtd = cm_pdusr_mpa_lookup_mtd (channel, device_nr);
				LSA_UINT16  device_properties = 0;

				if (is_not_null (mtd))
				{
					device_properties |=  (mtd->properties & CM_PD_MTD_PROP_NOS)				? 0x01 : 0;
					device_properties |=  (mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL)	? 0x02 : 0;
					device_properties |=  (mtd->properties & CM_PD_MTD_PROP_I_ANY_PEER_OPT)		? 0x04 : 0;
					device_properties |=  (mtd->properties & CM_PD_MTD_PROP_I_ANY_PEER_PROG)	? 0x08 : 0;

					iosystem_properties |= ((device_nr == 0) && (mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA)) ? 0x0001 : 0;
				}

				number_of_devices += 1;

				cm_put_assert (left > 2+2);

				CM_PUT16_HTON (dp,  0, device_nr);
				CM_PUT16_HTON (dp,  2, device_properties);

				cm_adjust (dp, left, 2+2);
			}
		}

		dp = (CM_COMMON_MEM_U8_PTR_TYPE)record_data;

		*record_data_length -= left;
		CM_PUT16_HTON (dp,  2, CM_CAST_U16(*record_data_length) - (2+2));
		CM_PUT16_HTON (dp,  6, iosystem_properties);
		CM_PUT16_HTON (dp,  8, number_of_devices);
	}

	return CM_OK;

cm_put_assert_failed:
	LSA_UNUSED_ARG(cm_put_assert_line);
	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDIOSystemInfo: failed, buffer to small, line(%u), given-data_length(%u)"
		, cm_put_assert_line, record_data_length
		);
	return CM_ERR_PRM_DATA;
}


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*------------------------------------------------------------------------------
//	PDInstanceTailorData
//----------------------------------------------------------------------------*/

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_itd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data,
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  *_itd
	)
{
	CM_LOG_FCT("cm_pd_itd_parse")
	CM_UPPER_MEM_U8_PTR_TYPE  dp = record_data;
	LSA_UINT32  left = record_data_length;
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  itd = LSA_NULL;

	/* see PDInstanceTailorData */

	LSA_UINT16  nr_of_devices, nr_of_peers;

	{
		LSA_UINT16  block_type, block_length, block_version, pad1, pad2, device_group;

		cm_get_assert2 (left >=  2+2+1+1+1+1 + 2+2,									CM_PNIO_FAULTY_PDInstanceTailor(0));

		block_type			= CM_GET16_NTOH (dp,  0);
		block_length		= CM_GET16_NTOH (dp,  2);
		block_version		= CM_GET16_NTOH (dp,  4);
		pad1				= CM_GET8_NTOH  (dp,  6);
		pad2				= CM_GET8_NTOH  (dp,  7);

		device_group		= CM_GET16_NTOH (dp,  8);
		nr_of_devices		= CM_GET16_NTOH (dp, 10);

		cm_adjust (dp, left, 2+2+1+1+1+1 + 2+2);

		cm_get_assert2 (block_type == 0x7080,										CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (CM_CAST_U32(block_length + 2+2)  ==  record_data_length,	CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (block_version == 0x0100,									CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (pad1 == 0,													CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (pad2 == 0,													CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (device_group == 0,											CM_PNIO_FAULTY_PDInstanceTailor(1));
		cm_get_assert2 (CM_CAST_U32(nr_of_devices * (2+1+1)) <= left,				CM_PNIO_FAULTY_PDInstanceTailor(0));
	}

	{
		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(& itd), sizeof(*itd));
		cm_get_assert2 (is_not_null (itd),											CM_PNIO_FAULTY_PDInstanceTailor(0));

		itd->nr_of_devices = nr_of_devices;
		itd->devices = LSA_NULL;
		itd->nr_of_peers = 0;
		itd->peers = LSA_NULL;

		if (nr_of_devices > 0)
		{
			cm_get_assert2 (nr_of_devices * sizeof(LSA_UINT16) < 0xFFFF,			CM_PNIO_FAULTY_PDInstanceTailor(0));
			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(& itd->devices), nr_of_devices * sizeof(LSA_UINT16));
			cm_get_assert2 (is_not_null (itd->devices),								CM_PNIO_FAULTY_PDInstanceTailor(0));
		}
	}

	{
		LSA_UINT16  i;

		for (i = 0; i < nr_of_devices; ++i)
		{
			LSA_UINT16  device_nr, pad1, pad2;

			cm_get_assert2 (left >= 2+1+1,											CM_PNIO_FAULTY_PDInstanceTailor(0));

			device_nr = CM_GET16_NTOH (dp, 0);
			pad1      = CM_GET8_NTOH  (dp, 2);
			pad2      = CM_GET8_NTOH  (dp, 3);

			cm_adjust (dp, left, 2+1+1);

			cm_get_assert2 (/*0 <= device_nr &&*/ device_nr <= CM_CFG_MAX_CL_DEVICES,	CM_PNIO_FAULTY_PDInstanceTailor(2)); 
			cm_get_assert2 (pad1 == 0,													CM_PNIO_FAULTY_PDInstanceTailor(0));
			cm_get_assert2 (pad2 == 0,													CM_PNIO_FAULTY_PDInstanceTailor(0));
		
			itd->devices[i] = device_nr;
		}
	}

	if (left > 0)
	{
		LSA_UINT16  pad1, pad2;

		cm_get_assert2 (left >= 1+1+2,													CM_PNIO_FAULTY_PDInstanceTailor(0));

		pad1        = CM_GET8_NTOH  (dp, 0);
		pad2        = CM_GET8_NTOH  (dp, 1);
		nr_of_peers = CM_GET16_NTOH (dp, 2);

		cm_adjust (dp, left, 1+1+2);

		cm_get_assert2 (pad1 == 0,														CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (pad2 == 0,														CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (nr_of_peers != 0,												CM_PNIO_FAULTY_PDInstanceTailor(0));
		cm_get_assert2 (CM_CAST_U32(nr_of_peers * ((2+2+2+1+1) +(2+2+2+1+1))) <= left,	CM_PNIO_FAULTY_PDInstanceTailor(0));

		itd->nr_of_peers = nr_of_peers;

		if (nr_of_peers > 0)
		{
			LSA_UINT16  i;

			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR(& itd->peers), nr_of_peers * sizeof(CM_PD_PORT_INSTANCE_PEER_TYPE));
			cm_get_assert2 (is_not_null (itd->peers),										CM_PNIO_FAULTY_PDInstanceTailor(0));

			for (i = 0; i < nr_of_peers; ++i)
			{
				LSA_UINT16  device_nr_1, slot_nr_1, subslot_nr_1, device_nr_2, slot_nr_2, subslot_nr_2;
				LSA_UINT16  pad1_1, pad2_1, pad1_2, pad2_2;

				cm_get_assert2 (left >= ((2+2+2+1+1) +(2+2+2+1+1)),							CM_PNIO_FAULTY_PDInstanceTailor(0));

				device_nr_1  = CM_GET16_NTOH (dp, 0);
				slot_nr_1    = CM_GET16_NTOH (dp, 2);
				subslot_nr_1 = CM_GET16_NTOH (dp, 4);
				pad1_1       = CM_GET8_NTOH  (dp, 6);
				pad2_1       = CM_GET8_NTOH  (dp, 7);

				device_nr_2  = CM_GET16_NTOH (dp,  8);
				slot_nr_2    = CM_GET16_NTOH (dp, 10);
				subslot_nr_2 = CM_GET16_NTOH (dp, 12);
				pad1_2       = CM_GET8_NTOH  (dp, 14);
				pad2_2       = CM_GET8_NTOH  (dp, 15);

				cm_adjust (dp, left, ((2+2+2+1+1) +(2+2+2+1+1)));

				cm_get_assert2 (/*0 <= device_nr_1 &&*/ device_nr_1 <= CM_CFG_MAX_CL_DEVICES,	CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (/*0 <= slot_nr_1 &&*/ slot_nr_1 <= 0x7FFF,						CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (CM_IS_PDEV_SUBSLOT_NR(subslot_nr_1),						CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (pad1_1 == 0,												CM_PNIO_FAULTY_PDInstanceTailor(0));
				cm_get_assert2 (pad2_1 == 0,												CM_PNIO_FAULTY_PDInstanceTailor(0));
		
				cm_get_assert2 (/*0 <= device_nr_2 &&*/ device_nr_2 <= CM_CFG_MAX_CL_DEVICES,	CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (/*0 <= slot_nr_2 &&*/ slot_nr_2 <= 0x7FFF,						CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (CM_IS_PDEV_SUBSLOT_NR(subslot_nr_2),						CM_PNIO_FAULTY_PDInstanceTailor(3));
				cm_get_assert2 (pad1_2 == 0,												CM_PNIO_FAULTY_PDInstanceTailor(0));
				cm_get_assert2 (pad2_2 == 0,												CM_PNIO_FAULTY_PDInstanceTailor(0));

				cm_get_assert2 (device_nr_1 != device_nr_2,									CM_PNIO_FAULTY_PDInstanceTailor(3));

				itd->peers[i].device_nr_1  = device_nr_1;
				itd->peers[i].slot_nr_1    = slot_nr_1;
				itd->peers[i].subslot_nr_1 = subslot_nr_1;
				itd->peers[i].device_nr_2  = device_nr_2;
				itd->peers[i].slot_nr_2    = slot_nr_2;
				itd->peers[i].subslot_nr_2 = subslot_nr_2;
			}
		}
	}

	cm_get_assert2 (left == 0,																CM_PNIO_FAULTY_PDInstanceTailor(0));

	*_itd = itd;
	return CM_PNIO_ERR_NONE;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);

	CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInstanceTailortData: parsing failed, line(%u), record-offset(%u) ErrorCode(0x%x)"
		, cm_get_assert_line, (dp - record_data), cm_get_assert_error
		);
	cm_pd_itd_free (& itd);
	*_itd = LSA_NULL;
	return cm_get_assert_error;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_itd_free (
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE CM_LOCAL_MEM_ATTR  *itd
	)
{
	if (is_not_null (*itd))
	{
		LSA_UINT16  rc;

		if (is_not_null ((*itd)->devices))
		{
			CM_FREE_LOCAL_MEM (&rc, (*itd)->devices);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		if (is_not_null ((*itd)->peers))
		{
			CM_FREE_LOCAL_MEM (&rc, (*itd)->peers);
			CM_ASSERT (rc == LSA_RET_OK);
		}

		CM_FREE_LOCAL_MEM (&rc, (*itd));
		CM_ASSERT (rc == LSA_RET_OK);

		*itd = LSA_NULL;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_itd_parsed_data_has_error (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  itd
	)
{
	CM_LOG_FCT("cm_pd_itd_parsed_data_has_error")
	LSA_UINT16  cm_get_assert_line;
	LSA_UINT32  cm_get_assert_error;
	LSA_INT  i;
	CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0 /* PDev */);

	cm_get_assert2 (is_not_null (cl_channel),	CM_PNIO_FAULTY_PDInstanceTailor(2)/* no device found */);

	cm_get_assert2 (is_not_null (pd_mtd),	CM_PNIO_FAULTY_Record__AccessDenied(2) /* PDMasterTailorData not available */);

	/* itd => dev */
	for (i = 0; i < itd->nr_of_devices; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, itd->devices[i]);

		cm_get_assert2 (is_not_null (mtd), CM_PNIO_FAULTY_PDInstanceTailor(2)/* device/mtd not found */);
		cm_get_assert2 ((mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL), CM_PNIO_FAULTY_PDInstanceTailor(2)/* enabled device not optional */);
	}

	for (i = 0; i < itd->nr_of_peers; ++i)
	{
		CM_PD_PORT_INSTANCE_PEER_PTR_TYPE  itdp = & itd->peers[i];
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd_1, mtd_2;
		CM_PD_MTD_PORT_PTR_TYPE  mtdp_1 = cm_pdusr_mpa_lookup_mtdp (cl_channel, itdp->device_nr_1, itdp->slot_nr_1, itdp->subslot_nr_1, & mtd_1);
		CM_PD_MTD_PORT_PTR_TYPE  mtdp_2 = cm_pdusr_mpa_lookup_mtdp (cl_channel, itdp->device_nr_2, itdp->slot_nr_2, itdp->subslot_nr_2, & mtd_2);

		cm_get_assert2 (is_not_null (mtdp_1), CM_PNIO_FAULTY_PDInstanceTailor(3)/* device/slot/subslot not found */);
		cm_get_assert2 (is_not_null (mtdp_2), CM_PNIO_FAULTY_PDInstanceTailor(3)/* device/slot/subslot not found */);

		cm_get_assert2 ((mtdp_1->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
			, CM_PNIO_FAULTY_PDInstanceTailor(3)/* peer-property not programable */
			)
		cm_get_assert2 ((mtdp_2->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_PROG
			, CM_PNIO_FAULTY_PDInstanceTailor(3)/* peer-property not programable */
			)

		{
			LSA_INT  p;
			LSA_BOOL found_1, found_2;

			found_1 = (mtd_1->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) ? LSA_FALSE : LSA_TRUE/* mandatory */;
			found_2 = (mtd_2->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) ? LSA_FALSE : LSA_TRUE/* mandatory */;

			for (p = 0; p < itd->nr_of_devices; ++p)
			{
				if (itd->devices[p] == itdp->device_nr_1)
				{
					found_1 = LSA_TRUE;
				}
				if (itd->devices[p] == itdp->device_nr_2)
				{
					found_2 = LSA_TRUE;
				}
			}
			cm_get_assert2 (found_1, CM_PNIO_FAULTY_PDInstanceTailor(3)/* programed device_1 disabled */);
			cm_get_assert2 (found_2, CM_PNIO_FAULTY_PDInstanceTailor(3)/* programed device_2 disabled */);
		}
	}

	/* pdev */
	{
		LSA_INT p;
		for (p = 0; p < pd_mtd->port_cnt; ++p)
		{
			CM_PD_MTD_PORT_PTR_TYPE  mtdp = & pd_mtd->ports[p];

			if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  peer_mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, mtdp->peer_station_nr);

				cm_get_assert2 (is_not_null (peer_mtd), CM_PNIO_FAULTY_PDInstanceTailor(2)/* optional peer-device/mtd not found */);
			}
		}
	}

	/* dev */
	{
		CM_CL_DEVICE_PTR_TYPE  dev;

		for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (is_not_null (mtd))
			{
				LSA_INT p;
				for (p = 0; p < mtd->port_cnt; ++p)
				{
					CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[p];

					if ((mtdp->properties & CM_PD_MTD_PEER_PROP_PEER_MASK) == CM_PD_MTD_PEER_PROP_PEER_OPT)
					{
						CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  peer_mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, mtdp->peer_station_nr);

						cm_get_assert2 (is_not_null (peer_mtd), CM_PNIO_FAULTY_PDInstanceTailor(2)/* optional peer-device/mtd not found */);
					}
				}
			}
		}
	}

	return CM_PNIO_ERR_NONE;

cm_get_assert_failed:
	LSA_UNUSED_ARG (channel);
	LSA_UNUSED_ARG (cm_get_assert_line);

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "PDInstanceTailortData: checking failed, line(%u), ErrorCode2(0x%x)", cm_get_assert_line, cm_get_assert_error
		);
	return cm_get_assert_error;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_mpa_tailor_abort_cmcl_and_set_running (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0/*PDev*/);
	CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);
	LSA_UINT16  abort_properties = CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG;
	LSA_BOOL  abort_all = LSA_FALSE;

	/* part 1: check if any PDev or dev is concerned */
	{
		LSA_BOOL  abort = LSA_FALSE;

		if (is_not_null (pd_mtd))
		{
			if ((pd_mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA) != 0)
			{
				abort_all = LSA_TRUE;
				abort = LSA_TRUE;
			}
			else if ((pd_mtd->properties & abort_properties) != 0)
			{
				abort = LSA_TRUE;
			}
		}

		if (!abort && is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE  dev;
			for (dev = cm_cl_dev_first (cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);
				if (is_not_null (mtd) && ((mtd->properties & abort_properties) != 0)) 
				{
					abort = LSA_TRUE;
					break;
				}
			}
		}

		if (!abort)
		{
			return CM_PNIO_FAULTY_PDInstanceTailor(0); /* no mtd abort-reason exists */;
		}
	}

	/* part 2: set PDInstanceTailorData running to prevent restart of devices */
	{
		channel->usr.pd.reclist.is_running_mpa = LSA_TRUE;
	}

	/* part 3: reset and abort */
	{
		if (abort_all || (is_not_null (pd_mtd)  &&  (pd_mtd->properties & abort_properties) != 0))
		{
			cm_pd_mtd_reset (pd_mtd);
		}
		if (is_not_null (cl_channel))
		{
			CM_CL_DEVICE_PTR_TYPE  dev;
			for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);
				
				if (abort_all)
				{
					if (is_not_null (mtd))
					{
						cm_pd_mtd_reset (mtd);
					}
					cm_cl_dev_abort (dev, CM_AR_REASON_PDEV);
				}
				else if (is_not_null (mtd)  &&  (mtd->properties & abort_properties) != 0)
				{
					cm_pd_mtd_reset (mtd);
					cm_cl_dev_abort (dev, CM_AR_REASON_PDEV);
				}
			}
		}
	}

	return CM_PNIO_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mpa_tailor_instance_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  itd
	)
{
	CM_LOG_FCT("cm_pd_mpa_tailor_instance_tailor_data")
	LSA_INT  i;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0/*PDev*/);

	cm_pd_mtd_set (pd_mtd, CM_PD_MTD_SET_DEVICE_ENABLED);

	for (i = 0; i < itd->nr_of_devices; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (channel, itd->devices[i]);

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PDInstanceTailorData: enable dev(%u)", itd->devices[i]
			);
		cm_pd_mtd_set (mtd, CM_PD_MTD_SET_DEVICE_ENABLED);
	}

	for (i = 0; i < itd->nr_of_peers; ++i)
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd_1;
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd_2;
		CM_PD_PORT_INSTANCE_PEER_PTR_TYPE  itdp = & itd->peers[i];
		CM_PD_MTD_PORT_PTR_TYPE  mtdp_1 = cm_pdusr_mpa_lookup_mtdp (channel, itdp->device_nr_1, itdp->slot_nr_1, itdp->subslot_nr_1, & mtd_1);
		CM_PD_MTD_PORT_PTR_TYPE  mtdp_2 = cm_pdusr_mpa_lookup_mtdp (channel, itdp->device_nr_2, itdp->slot_nr_2, itdp->subslot_nr_2, & mtd_2);

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PDInstanceTailorData: peer dev(%u) subslot(0x%x)  to  dev(%u) subslot(0x%x)"
			, itdp->device_nr_1, itdp->subslot_nr_1, itdp->device_nr_2, itdp->subslot_nr_2
			);

		cm_pd_mtd_peer (mtdp_1, mtdp_2);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mpa_tailor_pre_calc_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0/*PDev*/);
	CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);
	LSA_UINT16  i;

	/* pre-calc max_line_rx_delay for PDIRGlobalData [BlockVersionLow = 1] */
	if (is_not_null (pd_mtd))
	{
		if (pd_mtd->pdirglobaldata_version == 0x0101)
		{
			for (i = 0; i < pd_mtd->port_cnt; ++i)
			{
				CM_PD_MTD_PORT_PTR_TYPE  mtdp = & pd_mtd->ports[i];

				mtdp->max_line_rx_delay = 1000 + mtdp->check_linedelay - 50;
			}
		}
	}
	if (is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE  dev;

		for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (is_not_null (mtd)  &&  (mtd->pdirglobaldata_version == 0x0101))
			{
				for (i = 0; i < mtd->port_cnt; ++i)
				{
					CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[i];
	
					mtdp->max_line_rx_delay = 1000 + mtdp->check_linedelay - 50;
				}
			}
		}
	}

	if (is_not_null (pd_mtd))
	{
		for (i = 0; i < pd_mtd->port_cnt; ++i)
		{
			CM_PD_MTD_PORT_PTR_TYPE  neighbour_mtdp;
			CM_PD_MTD_PORT_PTR_TYPE  mtdp = & pd_mtd->ports[i];

			mtdp->real_max_line_rx_delay = 0;
			if (cm_pdusr_mpa_lookup_neighbour_peer (channel, mtdp, &neighbour_mtdp, &mtdp->real_max_line_rx_delay))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  neighbour_mtd = cm_pdusr_mpa_lookup_mtd (channel, neighbour_mtdp->peer_station_nr);

				mtdp->real_station_nr = neighbour_mtdp->peer_station_nr;
				mtdp->real_slot_nr = neighbour_mtdp->peer_slot_nr;
				mtdp->real_subslot_nr = neighbour_mtdp->peer_subslot_nr;
				mtdp->real_properties = neighbour_mtdp->properties;

				if (is_not_null (neighbour_mtd)  &&  neighbour_mtd->pdirglobaldata_version > 0)
				{
					mtdp->real_properties |= CM_PD_MTD_PEER_PROP_I_RTC3;
				}
			}
		}
	}

	if (is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE  dev;

		for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

			if (is_not_null (mtd)
			&&  ((mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL) == 0  ||  mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED))
			{
				for (i = 0; i < mtd->port_cnt; ++i)
				{
					CM_PD_MTD_PORT_PTR_TYPE  neighbour_mtdp;
					CM_PD_MTD_PORT_PTR_TYPE  mtdp = & mtd->ports[i];

					mtdp->real_max_line_rx_delay = 0;
					if (cm_pdusr_mpa_lookup_neighbour_peer (channel, mtdp, &neighbour_mtdp, &mtdp->real_max_line_rx_delay))
					{
						CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  neighbour_mtd = cm_pdusr_mpa_lookup_mtd (channel, neighbour_mtdp->peer_station_nr);

						mtdp->real_station_nr = neighbour_mtdp->peer_station_nr;
						mtdp->real_slot_nr = neighbour_mtdp->peer_slot_nr;
						mtdp->real_subslot_nr = neighbour_mtdp->peer_subslot_nr;
						mtdp->real_properties = neighbour_mtdp->properties;

						if (is_not_null (neighbour_mtd)  &&  neighbour_mtd->pdirglobaldata_version > 0)
						{
							mtdp->real_properties |= CM_PD_MTD_PEER_PROP_I_RTC3;
						}
					}
				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstanceTailorData (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_LOG_FCT("cm_pd_prm_write_PDInstanceTailorData")
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  itd;

	/*	parse record PDInstaceTailorData
	//	check parsed info, e.g. device_nr exists
	//	note: no cnf(-) allowed after this step
	//	set schedule-abort-reason
	//		abort all IODs targed by PDInstanceTailorData
	//		reset IODs (and PDev) to PDInstanceTailorData unknown
	//		set new PDInstanceTailorData
	//		pre-calc some information needed by cm_pd_mpa_prm_write_tailor_data()
	//		prm-trigger to PDev if targed by PDInstanceTailorData
	//	clear schedule-abort-reason
	//	PDInstaceTailorData.cnf(+)
	*/

	/* additional parameter check */
	{
		CM_PD_PORT_IF_PTR_TYPE port_if = cm_pd_port_if__nn (channel);

		if (port_if->port.addr.slot_nr != prm_rdwr->slot_nr  ||  port_if->port.addr.subslot_nr != prm_rdwr->subslot_nr)
		{
			prm_rdwr->cm_pnio_err = CM_PNIO_FAULTY_Record__InvalidSlotSubslot(0);
			cm_pd_prm_write_PDInstanceTailorData_done (channel, rb);
			return;
		}

		/* note: PDev.mtd must not exist with MachineTailoring and single-deployable */

		if (reclist->is_running_mpa)
		{
			prm_rdwr->cm_pnio_err = CM_PNIO_FAULTY_PDInstanceTailor(0);
			cm_pd_prm_write_PDInstanceTailorData_done (channel, rb);
			return;
		}
	}

	prm_rdwr->cm_pnio_err = cm_pd_itd_parse (channel, prm_rdwr->record_data_length, prm_rdwr->record_data, & itd);
	if (prm_rdwr->cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		cm_pd_prm_write_PDInstanceTailorData_done (channel, rb);
		return;
	}

	prm_rdwr->cm_pnio_err = cm_pd_itd_parsed_data_has_error (channel, itd);
	if (prm_rdwr->cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		cm_pd_itd_free (& itd);
		cm_pd_prm_write_PDInstanceTailorData_done (channel, rb);
		return;
	}

	prm_rdwr->cm_pnio_err = cm_pd_mpa_tailor_abort_cmcl_and_set_running (channel);
	if (prm_rdwr->cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		cm_pd_itd_free (& itd);
		cm_pd_prm_write_PDInstanceTailorData_done (channel, rb);
		return;
	}

	CM_ASSERT (reclist->is_running_mpa); /* no more cnf(-) occur */

	cm_pd_mpa_tailor_instance_tailor_data (channel, itd);

	cm_pd_itd_free (& itd);

	cm_pd_mpa_tailor_pre_calc_tailor_data (channel);

	channel->usr.pd.reclist.mpa_rb = rb; /* store for callback */

	cm_pd_prm_write_PDInstance_TailorData_trigger (channel);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstance_TailorData_trigger (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_LOG_FCT("cm_pd_prm_write_PDInstanceTailorData_trigger_config")

	if (channel->usr.pd.reclist.is_running_mpa)
	{
		CM_CHANNEL_PTR_TYPE  cl_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_CLIENT);

		if (is_not_null (cl_channel))
		{
			CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (channel, 0/*PDev*/);
			LSA_UINT16  abort_properties = CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG;
			LSA_BOOL  abort_all = LSA_FALSE;
			CM_CL_DEVICE_PTR_TYPE  dev;

			if (is_not_null (pd_mtd)  &&  ((pd_mtd->properties & CM_PD_MTD_PROP_IR_FRAME_DATA) != 0))
			{
				abort_all = LSA_TRUE;
			}

			for (dev = cm_cl_dev_first(cl_channel); is_not_null (dev); dev = cm_cl_dev_next (cl_channel, dev))
			{
				CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd = cm_pdusr_mpa_lookup_mtd (cl_channel, dev->device_nr);

				if (abort_all  ||  (is_not_null (mtd)  &&  (mtd->properties & abort_properties) != 0))
				{
					CM_AR_GRAPH_PTR_TYPE argr;

					cm_cl_dev_fetch_single__nn (dev, &argr);
					if (CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_DO_FRAMES_PASSIVATE) ||
						CM_CL_WF_IS_SET(argr->cm.cl.wait_for, CM_CL_WF_FRAMES_PASSIVATE_DONE))
					{
						CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev (%u) not yet passivated", dev->device_nr);
						return; 
					}
				}
			}
		}

		/* trigger a new PDev-parameterization */
		cm_pd_config_start (channel, CM_PD_CONFIG_MACHINE_TAILOR);
		
		cm_pd_prm_write_PDInstanceTailorData_done (channel, channel->usr.pd.reclist.mpa_rb);

		channel->usr.pd.reclist.mpa_rb = LSA_NULL;
	}
}

/*----------------------------------------------------------------------------*/
/*callback*/ 
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstanceTailorData_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_RECLIST_PTR_TYPE  reclist = & channel->usr.pd.reclist;

	if (reclist->is_running_mpa)
	{
		reclist->is_running_mpa = LSA_FALSE;
	}

	if (rb->args.pd.prm_rdwr->cm_pnio_err != CM_PNIO_ERR_NONE)
	{
		/* PDInstanceTailorData failed, keep pnio_err */
		cm_pd_req_callback (channel, CM_OK, rb);
	}
	else
	{
		cm_pd_req_callback (channel, CM_OK, rb);
	}
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
