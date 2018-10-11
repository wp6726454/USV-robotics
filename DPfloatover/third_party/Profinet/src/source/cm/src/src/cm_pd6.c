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
/*  F i l e               &F: cm_pd6.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice  PRM-statemachine and sub-component                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  860
#define CM_MODULE_ID       860

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_prepare (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_prepare_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_write (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_write_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done,
	LSA_UINT32				record_index
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_end (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_end_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit2 (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_indication (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_indication_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	);


/*----------------------------------------------------------------------------*/

/* trigger for PDevConfig				0x000000FF */
#define CM_PD_CONFIG__DO_BEGIN			0x00000100
#define CM_PD_CONFIG__WF_BEGIN			0x00000200
#define CM_PD_CONFIG__DO_SEND_CLOCK		0x00000400
#define CM_PD_CONFIG__WF_SEND_CLOCK		0x00000800
#define CM_PD_CONFIG__DO_WRITE			0x00001000
#define CM_PD_CONFIG__WF_WRITE			0x00002000
#define CM_PD_CONFIG__DO_END			0x00004000
#define CM_PD_CONFIG__WF_END			0x00008000
#define CM_PD_CONFIG__DO_COMMIT			0x00010000
#define CM_PD_CONFIG__WF_COMMIT			0x00020000
#define CM_PD_CONFIG__DO_FIRST_IND		0x00040000
#define CM_PD_CONFIG__WF_FIRST_IND		0x00080000

#define CM_PD_CONFIG__PDEV_PRM_ERROR	0x00100000	/* => ApplReady.ind(CM_ERR_PRM_CONSISTENCY) */
#define CM_PD_CONFIG__PDEV_NOT_READY	0x00200000	/* => ApplReady.ind(CM_OK_ADAPTION_NEEDED) */
#define CM_PD_CONFIG__RESET_TO_FACTORY	0x00400000	/* => ApplReady.ind(CM_OK_RESET_TO_FACTORY) */
#define CM_PD_CONFIG__NO_ARDY_IND		0x00800000	/* no ApplReady.ind */


/*------------------------------------------------------------------------------
// common
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_gsy_syncid_exists (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT8  sync_id,
	LSA_BOOL  do_elog
	)
{
	CM_LOG_FCT("cm_pd_gsy_syncid_exists")
	LSA_UINT8  nic_id = pd_channel->detailptr->nic_id;
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_channel_find_gsy (nic_id, sync_id); /* note: GSY_SYNCID_TIME will be removed with AP01504849 */

	CM_ASSERT (pd_channel->detailptr->path_type == CM_PATH_TYPE_PD);

	if (is_null (gsy_channel))
	{
		if (do_elog)
		{
			CM_PD_TRACE_02 (pd_channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "sync_id(%u) doesn't exist within nic_id(%u)", sync_id, nic_id
				);
		}
		return LSA_FALSE;
	}
	return LSA_TRUE;
}
#endif


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_sc_map_response (
	LSA_UINT16			lsa_comp_id,
	LSA_UINT16			sc_response
	)
{
	CM_LOG_FCT("cm_pd_sc_map_response")
	LSA_UINT16  rc = CM_OK;

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("todo: check CM_ERR_PRM_OTHER")
#endif /* CM_MESSAGE */

	switch (lsa_comp_id) {
	case LSA_COMP_ID_EDD:
		switch (sc_response) {
		case EDD_STS_OK:						rc = CM_OK; break;
		case EDD_STS_ERR_PRM_INDEX:				rc = CM_ERR_PRM_INDEX; break;
		case EDD_STS_ERR_PRM_DATA:				rc = CM_ERR_PRM_DATA; break;
		case EDD_STS_ERR_PRM_BLOCK:				rc = CM_ERR_PRM_BLOCK; break;
		case EDD_STS_ERR_PRM_CONSISTENCY:		rc = CM_ERR_PRM_CONSISTENCY; break;
		case EDD_STS_ERR_PRM_PORTID:			rc = CM_ERR_PRM_PORTID; break;
		case EDD_STS_ERR_PRM_VERSION_CONFLICT:	rc = CM_ERR_PRM_VERSION; break; /* see AP00510012 */
		default:								rc = CM_ERR_PRM_OTHER; break;
		}
		break;

#if CM_CFG_USE_MRP
	case LSA_COMP_ID_MRP:
		switch (sc_response) {
		case MRP_RSP_OK:						rc = CM_OK; break;
		case MRP_RSP_ERR_PRM_INDEX:				rc = CM_ERR_PRM_INDEX; break;
		case MRP_RSP_ERR_PRM_DATA:				rc = CM_ERR_PRM_DATA; break;
		case MRP_RSP_ERR_PRM_BLOCK:				rc = CM_ERR_PRM_BLOCK; break;
		case MRP_RSP_ERR_PRM_CONSISTENCY:		rc = CM_ERR_PRM_CONSISTENCY; break;
		case MRP_RSP_ERR_PRM_PORTID:			rc = CM_ERR_PRM_PORTID; break;
		case MRP_RSP_ERR_PRM_NOT_CONFIGURED:	rc = CM_ERR_PRM_INDEX; break; /*AP00586614:no error trace if mrp is not configured*/
		default:								rc = CM_ERR_PRM_OTHER; break;
		}
		break;
#endif

#if CM_CFG_USE_GSY
	case LSA_COMP_ID_GSY:
		switch (sc_response) {
		case GSY_RSP_OK:					rc = CM_OK; break;
		case GSY_RSP_ERR_PRM_INDEX:			rc = CM_ERR_PRM_INDEX; break;
		case GSY_RSP_ERR_PRM_DATA:			rc = CM_ERR_PRM_DATA; break;
		case GSY_RSP_ERR_PRM_BLOCK:			rc = CM_ERR_PRM_BLOCK; break;
		case GSY_RSP_ERR_PRM_CONSISTENCY:	rc = CM_ERR_PRM_CONSISTENCY; break;
		case GSY_RSP_ERR_PRM_PORTID:		rc = CM_ERR_PRM_PORTID; break;
		default:							rc = CM_ERR_PRM_OTHER; break;
		}
		break;
#endif

#if CM_CFG_USE_POF
	case LSA_COMP_ID_POF:
		switch (sc_response) {
		case POF_RSP_OK:					rc = CM_OK; break;
		case POF_RSP_ERR_PRM_INDEX:			rc = CM_ERR_PRM_INDEX; break;
		case POF_RSP_ERR_PRM_DATA:			rc = CM_ERR_PRM_DATA; break;
		case POF_RSP_ERR_PRM_BLOCK:			rc = CM_ERR_PRM_BLOCK; break;
		case POF_RSP_ERR_PRM_CONSISTENCY:	rc = CM_ERR_PRM_CONSISTENCY; break;
		case POF_RSP_ERR_PRM_PORTID:		rc = CM_ERR_PRM_PORTID; break;
		default:							rc = CM_ERR_PRM_OTHER; break;
		}
		break;
#endif

	case LSA_COMP_ID_OHA:
		switch (sc_response) {
		case OHA_OK:						rc = CM_OK; break;
		case OHA_ERR_PRM_INDEX:				rc = CM_ERR_PRM_INDEX; break;
		case OHA_ERR_PRM_DATA:				rc = CM_ERR_PRM_DATA; break;
		case OHA_ERR_PRM_BLOCK:				rc = CM_ERR_PRM_BLOCK; break;
		case OHA_ERR_PRM_CONSISTENCY:		rc = CM_ERR_PRM_CONSISTENCY; break;
		case OHA_ERR_PRM_PORTID:			rc = CM_ERR_PRM_PORTID; break;
		default:							rc = CM_ERR_PRM_OTHER; break;
		}
		break;

	default:
		CM_FATAL1 (lsa_comp_id);
		rc = CM_ERR_PARAM;
		break;
	}

	return rc;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_set_response (
	CM_PD_PRM_PTR_TYPE	pdprm,
	LSA_UINT16			lsa_comp_id,
	LSA_UINT16			sc_response
	)
{
	CM_LOG_FCT("cm_pd_prm_set_response")
	LSA_UINT16  rc;

	rc = cm_pd_sc_map_response (lsa_comp_id, sc_response);

	if (rc != CM_OK && pdprm->response == CM_OK) {

		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "state(%u)  lsa_comp_id(%u) set response(%u)"
			, pdprm->state, lsa_comp_id, rc
			);
		pdprm->response = rc;
	}
}


/*----------------------------------------------------------------------------*/
/*AP00377684*/
LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_response_to_pnio_err (		/* if CM_OK: ret CM_PNIO_ERR_NONE  else: see CM_PNIO_ERR_MAKE() */
	LSA_RESPONSE_TYPE  rsp,			/* see cm_response_enum */
	LSA_BOOL  is_read
	)
{
	LSA_UINT32 pnio_err;

	/* note: use CM_PNIO_ERR_CODE_READ as ErrorCode
	**       for CM_PNIO_ERR_CODE_WRITE use CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite()
	*/

	switch (rsp) {
	case CM_OK:
		pnio_err = CM_PNIO_ERR_NONE;
		break;

	case CM_ERR_PRM_INDEX: /* unknown index (PrmWrite and PrmRead) */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		break;

	case CM_ERR_PRM_PORTID: /* port-id does not match with index (PrmWrite and PrmRead) */
		/* was __InvalidIndex(1), changed because of AP00392849 */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		break;

	case CM_ERR_PRM_DATA: /* data-length too short (PrmRead) or data-length not consistent with block-structure (PrmWrite) */
		if (is_read) {
			pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(0); /* see part 5, "Read Partial Access" */
		}
		else {
			/* was __WriteLengthError(0), changed because of AP00392849 */
			/* was __BufferTooSmall(0), changed because of AP00633079 */
			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		}
		break;

	case CM_ERR_PRM_BLOCK: /* wrong block-type/version or something wrong with the block-data (PrmWrite) */
		/* was __VersionConflict(0), changed because of AP00392849 */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		break;

	case CM_ERR_PRM_CONSISTENCY: /* the parameterization is not consistent (PrmEnd) */
		/* should not occur with record-read/write */
		/* was __InvalidType(0), changed because of AP00633079 */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		break;

	case CM_ERR_PRM_OTHER:
		/* was __FeatureNotSupported(0), changed because of AP00392849 */
		pnio_err = CM_PNIO_FAULTY_Record__UnkownError(0);
		break;

	case CM_ERR_PRM_VERSION:
		/* AP00510012: Some IRData lead to a hardware problem */
		/* was __VersionConflict(0), changed because of AP00633079 */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0);
		break;

	case CM_ERR_NOT_ACCESSIBLE: /* mapping does not exist, this rsp is only set by CM */
		/* was __InvalidSlotSubslot(8), changed because of AP00392849 */
		pnio_err = CM_PNIO_FAULTY_Record__InvalidSlotSubslot(0); /* subslot not plugged */
		break;

	default:
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("BEAUTIFY: check if this is unreachable")
#endif /* CM_MESSAGE */
		/* was __ReadError(0) / __WriteError(0), changed because of AP00392849 */
		pnio_err = CM_PNIO_FAULTY_Record__UnkownError(0);
		break;
	}

	return pnio_err;
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_sc_id_from_sync_id (
	LSA_UINT8  sync_id,
	LSA_UINT16 CM_LOCAL_MEM_ATTR  *sc_id,
	LSA_UINT16 CM_LOCAL_MEM_ATTR  *sc_bf
) {
	CM_LOG_FCT("cm_pd_sc_id_from_sync_id")

	switch (sync_id) {

	case GSY_SYNCID_CLOCK:
		*sc_id = CM_PD_SCID_GSY_CLOCK;
		*sc_bf = CM_PD_SCBF_GSY_CLOCK;
		break;

	case GSY_SYNCID_TIME:
		*sc_id = CM_PD_SCID_GSY_TIME;
		*sc_bf = CM_PD_SCBF_GSY_TIME;
		break;

	default:
		CM_PD_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL
			, "unknown sync_id(%u)", sync_id
			);
		*sc_id = 0; /* initialize to avoid a warning */
		*sc_bf = 0; /* initialize to avoid a warning */
		CM_FATAL1 (sync_id);
		break;
	}
}
#endif

/*------------------------------------------------------------------------------
// CM-PD-PRM
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_init (
	CM_CHANNEL_PTR_TYPE	channel,
	CM_PD_PRM_PTR_TYPE	pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_init")

	/* check */
	{
	LSA_UINT32 max_ports = sizeof(pdprm->wait_first_ind[0]) * 8 - 1;

	if (channel->usr.pd.port_count > max_ports) {
		CM_FATAL1 (channel->usr.pd.port_count);
		/* must extend sizeof(wait_first_ind) for more ports as bit-mask */
	}
	}

	/* init */
	pdprm->channel = channel;
	pdprm->trace_idx = channel->trace_idx;

	pdprm->state = CM_PD_PRM_STATE_WORKING;
	pdprm->is_parameterized = LSA_FALSE;
	pdprm->is_reset_to_factory = LSA_FALSE;

	pdprm->occupant = CM_PD_OCCUPANT_NOBODY;
	pdprm->last_appl_ready_ind = CM_ERR_PRM_CONSISTENCY;

	pdprm->device_nr = 0;
	pdprm->ar_nr = 0;

	pdprm->wait_cnf = 0;
	pdprm->wait_cnf_next = 0;

	pdprm->wait_for = 0;
	pdprm->wait_sc = 0;
	pdprm->wait_sc_ind = 0;

	pdprm->response = CM_OK;

	pdprm->arp = 0;	/* 0..Ok */

	/*
	pdprm->wait_first_ind[0] = not used
	*/
	pdprm->wait_first_ind[CM_PD_SCID_EDD] = 0;
#if CM_CFG_USE_MRP
	pdprm->wait_first_ind[CM_PD_SCID_MRP] = 0;
#endif
#if CM_CFG_USE_GSY
	pdprm->wait_first_ind[CM_PD_SCID_GSY_CLOCK] = 0;
	pdprm->wait_first_ind[CM_PD_SCID_GSY_TIME] = 0;
#endif
#if CM_CFG_USE_POF
	pdprm->wait_first_ind[CM_PD_SCID_POF] = 0;
#endif
	pdprm->wait_first_ind[CM_PD_SCID_OHA] = 0;
	pdprm->wait_first_ind[CM_PD_SCID_CMPD] = 0; /* sanity */

	pdprm->end_counter = 0;

	pdprm->pd_config_rqb = cm_upper_alloc_rqb (channel);
	if (is_null (pdprm->pd_config_rqb))
	{
		CM_FATAL();
	}
	CM_RQB_SET_OPCODE (pdprm->pd_config_rqb, 0);


	/* keep upper requests */
	pdprm->write = LSA_NULL;
	pdprm->write_nonprm = LSA_NULL;
	pdprm->end = LSA_NULL;
	pdprm->relinquish = LSA_NULL;

	pdprm->read = LSA_NULL;

	/***/

	CM_ASSERT (CM_PD_SCID_EDD == 1);
	CM_ASSERT ((1 << CM_PD_SCID_EDD) == CM_PD_SCBF_EDD);
#if CM_CFG_USE_MRP
	CM_ASSERT ((1 << CM_PD_SCID_MRP) == CM_PD_SCBF_MRP);
#endif
#if CM_CFG_USE_GSY
	CM_ASSERT ((1 << CM_PD_SCID_GSY_CLOCK) == CM_PD_SCBF_GSY_CLOCK);
	CM_ASSERT ((1 << CM_PD_SCID_GSY_TIME) == CM_PD_SCBF_GSY_TIME);
#endif
#if CM_CFG_USE_POF
	CM_ASSERT ((1 << CM_PD_SCID_POF) == CM_PD_SCBF_POF);
#endif
	CM_ASSERT ((1 << CM_PD_SCID_OHA) == CM_PD_SCBF_OHA);
	CM_ASSERT (CM_PD_SCID_MAX == 9)
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_undo_init (
	CM_PD_PRM_PTR_TYPE	pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_undo_init")

	cm_upper_free_rqb (pdprm->channel, pdprm->pd_config_rqb);
	pdprm->pd_config_rqb = LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_owner_get (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	return pdprm->occupant;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_owner_set (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT8				new_occupant,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
	)
{
	CM_LOG_FCT("cm_pd_prm_owner_set")

	CM_PD_TRACE_02 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "new_occupant(%u) ar_nr(%u)", new_occupant, ar_nr
		);

		/* assign new owner */
	switch (new_occupant) {
	case CM_PD_OCCUPANT_NOBODY:
		pdprm->occupant = CM_PD_OCCUPANT_NOBODY;
		pdprm->device_nr = 0;
		pdprm->ar_nr = 0;
		break;

	case CM_PD_OCCUPANT_USER:
	case CM_PD_OCCUPANT_SV:
		pdprm->occupant = new_occupant;
		pdprm->device_nr = device_nr;
		pdprm->ar_nr = ar_nr;

		pdprm->arp = 0;
		CM_MEMSET (pdprm->wait_first_ind, 0, sizeof(pdprm->wait_first_ind));
		break;

	default:
		CM_FATAL1 (new_occupant);
		return;
	}
}


/*----------------------------------------------------------------------------*/
/* sync */ LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_ownership (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT8				new_occupant,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
	)
{
	CM_LOG_FCT("cm_pd_prm_ownership")

	CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "PD-ownership: state(%u) wait_for(0x%x)  new_occupant(%u)  ar_nr(%u)"
		, pdprm->state, pdprm->wait_for
		, new_occupant, ar_nr
		);

	switch (pdprm->state) {
	case CM_PD_PRM_STATE_WF_PREPARE:
		CM_ASSERT (pdprm->occupant == CM_PD_OCCUPANT_NOBODY);
		cm_pd_prm_owner_set (pdprm, new_occupant, device_nr, ar_nr);
		return CM_OK;

	case CM_PD_PRM_STATE_WORKING:
		/* assign new owner */
		CM_ASSERT (pdprm->occupant == CM_PD_OCCUPANT_NOBODY);
		cm_pd_prm_owner_set (pdprm, new_occupant, device_nr, ar_nr);
		return CM_OK;

	case CM_PD_PRM_STATE_WF_WRITE:
		return CM_ERR_SEQUENCE;

	case CM_PD_PRM_STATE_WF_END:
		return CM_ERR_SEQUENCE;

	case CM_PD_PRM_STATE_WF_COMMIT:
		return CM_ERR_SEQUENCE;

	case CM_PD_PRM_STATE_WF_SC_WORKING:
		return CM_ERR_SEQUENCE;

	default:
		CM_FATAL1 (pdprm->state);
		return CM_ERR_PARAM;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_nonprm (
	CM_PD_PRM_PTR_TYPE		pdprm,
	CM_UPPER_RQB_PTR_TYPE	write
	)
{
	CM_LOG_FCT("cm_pd_prm_write_nonprm")

	CM_ASSERT (is_null (pdprm->write_nonprm));
	pdprm->write_nonprm = write;

	cm_pd_prm_sc_write (pdprm);
}


/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  PREPARE
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_prepare (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_prepare")

	CM_ASSERT ( !(pdprm->wait_for & CM_PD_PRM_WF_PREPARE_CNF));
	CM_ASSERT (pdprm->wait_sc == 0);

	pdprm->state = CM_PD_PRM_STATE_WF_PREPARE;

	pdprm->wait_for |= CM_PD_PRM_WF_PREPARE_CNF;
	pdprm->response = CM_OK;

	pdprm->arp = 0;
	/* keep faulty_record_index */
	CM_MEMSET (pdprm->wait_first_ind, 0, sizeof(pdprm->wait_first_ind));

	/* CMPD */

	/* CM_PD_SCID_EDD */
	cm_edd_prm_prepare (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_EDD;

#if CM_CFG_USE_MRP
	/* CM_PD_SCID_MRP */
	cm_mrp_prm_prepare (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_MRP;
#endif

#if CM_CFG_USE_GSY
	/* CM_PD_SCID_GSY_CLOCK */
	cm_gsy_prm_prepare (pdprm->channel, GSY_SYNCID_CLOCK);
	pdprm->wait_sc |= CM_PD_SCBF_GSY_CLOCK;

	/* CM_PD_SCID_GSY_TIME */
	if (cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_FALSE)) {

		cm_gsy_prm_prepare (pdprm->channel, GSY_SYNCID_TIME);
		pdprm->wait_sc |= CM_PD_SCBF_GSY_TIME;
	}
#endif

#if CM_CFG_USE_POF
	/* CM_PD_SCID_POF */
	cm_pof_prm_prepare (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_POF;
#endif

	/* CM_PD_SCID_OHA */
	cm_oha_prm_prepare (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_OHA;

	CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_PREPARE: wait_for(0x%x) wait_sc(0x%x) response(%u)"
		, pdprm->wait_for, pdprm->wait_sc, pdprm->response
		);

	if (pdprm->wait_sc == 0) {
		cm_pd_prm_sc_prepare_done (pdprm, 0/*!*/);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_prepare_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_EDD, CM_EDD_GET_RESPONSE (edd));

	cm_pd_prm_sc_prepare_done (pdprm, CM_PD_SCBF_EDD);
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_prepare_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_MRP, CM_MRP_GET_RESPONSE (mrp));

	cm_pd_prm_sc_prepare_done (pdprm, CM_PD_SCBF_MRP);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_prepare_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16  sc_id, sc_bf;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_GSY, CM_GSY_GET_RESPONSE (gsy));

	cm_pd_sc_id_from_sync_id (sync_id, & sc_id, & sc_bf);

	cm_pd_prm_sc_prepare_done (pdprm, sc_bf);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_prepare_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_POF, CM_POF_GET_RESPONSE (pof));

	cm_pd_prm_sc_prepare_done (pdprm, CM_PD_SCBF_POF);
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_prepare_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_OHA, CM_OHA_GET_RESPONSE (oha));

	cm_pd_prm_sc_prepare_done (pdprm, CM_PD_SCBF_OHA);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_prepare_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_prepare_done")

	CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_PREPARE_DONE: wait_for(0x%x) wait_sc(0x%x) response(%u) wait_sc_done(0x%x)"
		, pdprm->wait_for, pdprm->wait_sc, pdprm->response, wait_sc_done
		);
	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_PREPARE_CNF);

	CM_ASSERT ((pdprm->wait_sc & wait_sc_done) == wait_sc_done);
	pdprm->wait_sc &= ~ wait_sc_done;

	if (pdprm->response != CM_OK)
	{
		cm_pd_prm_set_arp (pdprm, pdprm->response, 0, wait_sc_done);
	}

	if (pdprm->wait_sc != 0)
	{
		return; /* wait.. */
	}

	pdprm->wait_for &= ~ CM_PD_PRM_WF_PREPARE_CNF;

CM_ASSERT (is_null (pdprm->relinquish));

	CM_ASSERT (pdprm->wait_sc == 0);
	CM_ASSERT (pdprm->wait_sc_ind == 0);

	cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_BEGIN);	
}

/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  WRITE
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_write (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_write")

	CM_PD_PORT_PTR_TYPE		port;

	LSA_UINT16				port_id;
	LSA_UINT16				slot_nr;
	LSA_UINT16				subslot_nr;
	LSA_UINT32				record_index;
	LSA_UINT32				record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE	record_data;
	LSA_UINT16				scid;
	LSA_BOOL				is_local;
	LSA_BOOL				mk_remanent;
	LSA_UINT32				record_cls;

	CM_ASSERT (is_not_null (pdprm->write_nonprm) ||  is_not_null (pdprm->write));

	pdprm->response = CM_OK;

	if (pdprm->wait_for & CM_PD_PRM_WF_WRITE_CNF)
	{
		return;
	}

	if (is_not_null (pdprm->write))
	{
		CM_AR_GRAPH_RECORD_PTR_TYPE  rec = pdprm->write;

		CM_ASSERT (pdprm->wait_sc == 0);

		pdprm->state = CM_PD_PRM_STATE_WF_WRITE;
		pdprm->wait_for |= CM_PD_PRM_IS_INTERNAL_WRITE;

		slot_nr				= rec->slot_nr;
		subslot_nr			= rec->subslot_nr;
		record_index		= rec->record_index;
		record_data_length	= rec->record_data_len;
		record_data			= rec->record_data;
		is_local			= rec->is_local;
		mk_remanent			= rec->mk_remanent;
		record_cls			= cm_record_index_classify (0/*PDev*/, rec->record_index, CM_VENDOR_ID_SIEMENS/*!*/);

		if (rec->tailor_data_len > 0)
		{
			record_data_length	= rec->tailor_data_len;
			record_data			= rec->tailor_data;
		}
	}
	else if (is_not_null (pdprm->write_nonprm))
	{
		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pdprm->write_nonprm->args.pd.prm_rdwr;

		slot_nr				= prm_rdwr->slot_nr;
		subslot_nr			= prm_rdwr->subslot_nr;
		record_index		= prm_rdwr->record_index;
		record_data_length	= prm_rdwr->record_data_length;
		record_data			= prm_rdwr->record_data;
		is_local			= prm_rdwr->is_local;
		mk_remanent			= prm_rdwr->mk_remanent;
		record_cls			= prm_rdwr->cls;

		CM_ASSERT (record_cls & CM_REC_IDX_NONPRM);
	}
	else
	{
		CM_FATAL();
		return;
	}

	pdprm->wait_for |= CM_PD_PRM_WF_WRITE_CNF;

	port = cm_pd_port_from_addr (pdprm->channel, slot_nr, subslot_nr);
	if (is_null (port)) {
		CM_FATAL ();
		return;
	}

	port_id	= port->port_id;

	scid = (LSA_UINT16)(record_cls & CM_REC_IDX_CMPD_MASK);
	if (scid == 0) {
		CM_FATAL1 (record_cls);
	}

	/* CMPD */

	/* CM_PD_SCID_EDD */
	if (
		scid == CM_PD_SCID_EDD
		|| record_index == 0x802D /* PDSyncData (SyncID 0) goes to GSY, EDD and OHA */
		|| record_index == 0x8052 /* PDInterfaceMrpDataAdjust */
		|| record_index == 0x8053 /* PDPortMrpDataAdjust */
	) {
		cm_edd_prm_write (pdprm->channel
			, is_local
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		pdprm->wait_sc |= CM_PD_SCBF_EDD;
	}

#if CM_CFG_USE_MRP
	/* CM_PD_SCID_MRP */
	if (scid == CM_PD_SCID_MRP) {
		cm_mrp_prm_write (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		pdprm->wait_sc |= CM_PD_SCBF_MRP;
	}
#endif

#if CM_CFG_USE_GSY
	/* CM_PD_SCID_GSY_CLOCK */
	if (
		scid == CM_PD_SCID_GSY_CLOCK
		|| record_index == 0x802F /* PDPortDataAdjust goes to EDD, GSY and OHA */
		|| record_index == 0x00010000 /* SendClock goes to EDD and GSY */
	) {
		cm_gsy_prm_write (pdprm->channel, GSY_SYNCID_CLOCK
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		pdprm->wait_sc |= CM_PD_SCBF_GSY_CLOCK;
	}

	/* CM_PD_SCID_GSY_TIME */
	if (scid == CM_PD_SCID_GSY_TIME) {
		if (! cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_TRUE)) { /*AP00496114*/
			pdprm->response = CM_ERR_PRM_INDEX;
		}
		else {
			cm_gsy_prm_write (pdprm->channel, GSY_SYNCID_TIME
				, port_id, slot_nr, subslot_nr, record_index
				, record_data_length, record_data
				);
			pdprm->wait_sc |= CM_PD_SCBF_GSY_TIME;
		}
	}
#endif

#if CM_CFG_USE_POF
	/* CM_PD_SCID_POF */
	if (scid == CM_PD_SCID_POF) {
		cm_pof_prm_write (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		pdprm->wait_sc |= CM_PD_SCBF_POF;
	}
#endif

	if (scid == CM_PD_SCID_CMPD)
	{
		if (record_index == 0x00017081/* PDMasterTailorData */)
		{
			/* => rema */
		}
		else {
			pdprm->response = CM_ERR_PRM_INDEX;
		}
	}

	/* CM_PD_SCID_OHA */
	{
	LSA_BOOL  needed_by_oha;
	LSA_BOOL  mk_rema;

	if (
		scid == CM_PD_SCID_OHA
		|| record_index == 0x802C /* PDIRData goes to EDD and OHA */
		|| record_index == 0x802D /* PDSyncData (SyncID 0) goes to GSY, EDD and OHA */
		|| record_index == 0x802F /* PDPortDataAdjust goes to EDD, GSY and OHA */
	) {
		needed_by_oha = LSA_TRUE;
	}
	else {
		needed_by_oha = LSA_FALSE;
	}

	if (record_cls & CM_REC_IDX_NONPRM)
	{
		mk_rema = LSA_FALSE; /* AP00863475 / non-prm indices must not be stored remanently */

		if (record_cls & CM_REC_IDX_MKREMA)
		{
			mk_rema = LSA_TRUE; /* see AP01265195 / IPSuite and NOS is stored remanently */
		}
	}
	else
	{
		mk_rema = mk_remanent;
	}

	if (needed_by_oha || mk_rema)
	{
		cm_oha_prm_write (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index, mk_rema
			, record_data_length, record_data
			);
		pdprm->wait_sc |= CM_PD_SCBF_OHA;
	}
	}

	if (pdprm->wait_sc == 0) {
		cm_pd_prm_sc_write_done (pdprm, 0/*!*/, 0);
	}
	else {
		CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_WRITE: record_index(0x%x) wait_for(0x%x) wait_sc(0x%x)   port(%u)"
			, record_index, pdprm->wait_for, pdprm->wait_sc, port_id
			);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cmpd_sendclock_changed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
) {
	CM_LOG_FCT("cm_cmpd_sendclock_changed")
	CM_PD_PRM_PTR_TYPE pdprm = &channel->usr.pd.pdprm;

	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_SEND_CLOCK)
	{
		if (send_clock == 0) /* send-clock could not be set */
		{
			cm_pd_prm_set_arp (pdprm, CM_ERR_LOWER_LAYER, 0, CM_PD_SCBF_CMPD);
		}
		cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_SEND_CLOCK);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_write_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	EDD_UPPER_PRM_WRITE_PTR_TYPE  param = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_WRITE_PTR_TYPE);

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_EDD, CM_EDD_GET_RESPONSE (edd));

	cm_pd_prm_sc_write_done (pdprm, CM_PD_SCBF_EDD, param->record_index);
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_write_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_MRP, CM_MRP_GET_RESPONSE (mrp));

	cm_pd_prm_sc_write_done (pdprm, CM_PD_SCBF_MRP, mrp->args.ctrl.prm_write.record_index);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_write_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16  sc_id, sc_bf;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_GSY, CM_GSY_GET_RESPONSE (gsy));

	cm_pd_sc_id_from_sync_id (sync_id, & sc_id, & sc_bf);

	cm_pd_prm_sc_write_done (pdprm, sc_bf, gsy->args.PrmRecord.RecordIndex);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_write_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_POF, CM_POF_GET_RESPONSE (pof));

	cm_pd_prm_sc_write_done (pdprm, CM_PD_SCBF_POF, pof->args.PrmWrite.record_index);
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_write_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_OHA, CM_OHA_GET_RESPONSE (oha));

	cm_pd_prm_sc_write_done (pdprm, CM_PD_SCBF_OHA, oha->args.PrmData.record_index);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_set_arp (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				rsp,
	LSA_UINT32				record_index,
	LSA_UINT16				wait_scbf_done
	)
{
	CM_LOG_FCT("cm_pd_prm_set_arp")

	LSA_UNUSED_ARG (rsp);
	LSA_UNUSED_ARG (record_index);
	LSA_UNUSED_ARG (wait_scbf_done);

	if (pdprm->arp == 0)
	{
		pdprm->arp = 1; /* 1.. PDEV has A.R.P. */
/*		cm_pd_reclist_set_error (pdprm->channel, record_index);	TIA#829122 */

		CM_PD_TRACE_05 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PRM_SC: SET-ARP : wait_cnf(0x%x); wait_for(0x%x) got rsp(%u) with record_index(0x%x) from wait_sc_done(0x%x)"
			, pdprm->wait_cnf, pdprm->wait_for, pdprm->response, record_index, wait_scbf_done
			);
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_write_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done,
	LSA_UINT32				record_index
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_write_done")

	CM_PD_TRACE_06 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_WRITE_DONE: record_index(0x%x) wait_for(0x%x) wait_sc(0x%x) response(%u) arp(0x%x) wait_sc_done(0x%x)"
		, record_index, pdprm->wait_for, pdprm->wait_sc, pdprm->response, pdprm->arp, wait_sc_done
		);

	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_WRITE_CNF);
	CM_ASSERT ((pdprm->wait_sc & wait_sc_done) == wait_sc_done);
	pdprm->wait_sc &= ~ wait_sc_done;

	if (pdprm->wait_for & CM_PD_PRM_IS_INTERNAL_WRITE) /* prm state machine */
	{
		CM_ASSERT (is_not_null (pdprm->write));

		if (pdprm->response != CM_OK)
		{
			cm_pd_prm_set_arp (pdprm, pdprm->response, record_index, wait_sc_done);
		}

		if (pdprm->wait_sc != 0) {
			/* must wait */
			return;
		}

		pdprm->wait_for &= ~ (CM_PD_PRM_WF_WRITE_CNF | CM_PD_PRM_IS_INTERNAL_WRITE);

		cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_WRITE);

		if (is_not_null (pdprm->write_nonprm))
		{
			cm_pd_prm_sc_write(pdprm);
		}
	}
	else /* usr write non prm */
	{
		CM_UPPER_RQB_PTR_TYPE  nonprm = pdprm->write_nonprm;
		pdprm->write_nonprm = LSA_NULL;

		pdprm->wait_for &= ~ CM_PD_PRM_WF_WRITE_CNF;

		CM_RQB_SET_RESPONSE (nonprm, pdprm->response);

		cm_pd_prm_write_nonprm_done (pdprm, nonprm);

		if (is_not_null (pdprm->write))
		{
			cm_pd_prm_sc_write(pdprm);
		}
	}
}

/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  END
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_end (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_end")
	LSA_BOOL  is_local;

	CM_ASSERT (! (pdprm->wait_for & CM_PD_PRM_WF_END_CNF));
	CM_ASSERT (pdprm->wait_sc == 0);

	pdprm->state = CM_PD_PRM_STATE_WF_END;

	pdprm->wait_for |= CM_PD_PRM_WF_END_CNF;
	pdprm->response = CM_OK;

	is_local = (pdprm->wait_cnf & CM_PD_CONFIG_PRM_USR) != 0 ? LSA_TRUE : LSA_FALSE;

	pdprm->end_counter += 1;
	pdprm->channel->usr.pd.almi.user_sequence_nr = 0; /* reset, see AP00632258 */

	CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_END: state(%u) wait_for(0x%x) arp(0x%x) end_counter(%u)"
		, pdprm->state, pdprm->wait_for, pdprm->arp, pdprm->end_counter
		);


	/* CMPD */

	/* CM_PD_SCID_EDD */
	cm_edd_prm_end (pdprm->channel, is_local);
	pdprm->wait_sc |= CM_PD_SCBF_EDD;

#if CM_CFG_USE_MRP
	/* CM_PD_SCID_MRP */
	cm_mrp_prm_end (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_MRP;
#endif

#if CM_CFG_USE_GSY
	/* CM_PD_SCID_GSY_CLOCK */
	cm_gsy_prm_end (pdprm->channel, GSY_SYNCID_CLOCK);
	pdprm->wait_sc |= CM_PD_SCBF_GSY_CLOCK;

	/* CM_PD_SCID_GSY_TIME */
	if (cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_FALSE)) {
		cm_gsy_prm_end (pdprm->channel, GSY_SYNCID_TIME);
		pdprm->wait_sc |= CM_PD_SCBF_GSY_TIME;
	}
#endif

#if CM_CFG_USE_POF
	/* CM_PD_SCID_POF */
	cm_pof_prm_end (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_POF;
#endif

	/* CM_PD_SCID_OHA */
	cm_oha_prm_end (pdprm->channel);
	pdprm->wait_sc |= CM_PD_SCBF_OHA;

	if (pdprm->wait_sc == 0)
	{
		cm_pd_prm_sc_end_done (pdprm, 0/*!*/);
	}
	else
	{
		CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_END: sc_end-waiting wait_for(0x%x) wait_sc(0x%x) arp(0x%x) end_counter(%u)"
			, pdprm->wait_for, pdprm->wait_sc, pdprm->arp, pdprm->end_counter
			);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_end_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_EDD, CM_EDD_GET_RESPONSE (edd));

	cm_pd_prm_sc_end_done (pdprm, CM_PD_SCBF_EDD);
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_end_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_MRP, CM_MRP_GET_RESPONSE (mrp));

	cm_pd_prm_sc_end_done (pdprm, CM_PD_SCBF_MRP);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_end_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16  sc_id, sc_bf;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_GSY, CM_GSY_GET_RESPONSE (gsy));

	cm_pd_sc_id_from_sync_id (sync_id, & sc_id, & sc_bf);

	cm_pd_prm_sc_end_done (pdprm, sc_bf);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_end_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_POF, CM_POF_GET_RESPONSE (pof));

	cm_pd_prm_sc_end_done (pdprm, CM_PD_SCBF_POF);
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_end_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	cm_pd_prm_set_response (pdprm, LSA_COMP_ID_OHA, CM_OHA_GET_RESPONSE (oha));

	cm_pd_prm_sc_end_done (pdprm, CM_PD_SCBF_OHA);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prepare_appl_ready (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_RESPONSE_TYPE  rsp,
	CM_UPPER_RQB_PTR_TYPE  appl_ready_ind
	)
{
	CM_ASSERT (is_not_null (appl_ready_ind));

	CM_ASSERT (CM_RQB_GET_OPCODE (appl_ready_ind) == CM_OPC_PD_EVENT_APPL_READY_IND);

	{
		CM_UPPER_EVENT_PTR_TYPE  upper_event = appl_ready_ind->args.channel.event;
		LSA_UINT16  port_id;

		upper_event->device_nr   = 0; /* user: don't evaluate */
		upper_event->ar_nr       = 0; /* user: don't evaluate */
		upper_event->session_key = pdprm->end_counter;

		upper_event->u.pd.appl_ready.faulty_record_index = pdprm->channel->usr.pd.reclist.faulty_record_index;

		CmListInitialize (& upper_event->u.pd.appl_ready.diff_list);

		for (port_id = 0; port_id <= pdprm->channel->usr.pd.port_count; ++port_id)
		{
			CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (pdprm->channel, port_id);
			LSA_UINT16  sub_state;
			LSA_BOOL  diag_status;
			LSA_UINT32  maint_status;

			if (! cm_pd_port_is_mapped (port)) { /* not mapped */
				continue;
			}

			cm_pd_diag_maint_qual (port, & diag_status, & maint_status);

			if (rsp == CM_OK  ||  rsp == CM_OK_ADAPTION_NEEDED)
			{
				sub_state = CM_SUB_STATE_GOOD;
			}
			else
			{
				sub_state = (CM_SUB_STATE_GOOD &  ~ CM_SUB_STATE_AR_INFO_MASK);
				sub_state |= CM_SUB_STATE_AR_INFO_APPL_READY_PENDING;
			}

			if (diag_status) {
				sub_state |= CM_SUB_STATE_DIAG_INFO_AVAILABLE;
			}
			if ((maint_status & CM_MAINTENANCE_STATUS_REQUIRED) != 0) {
				sub_state |= CM_SUB_STATE_MAINTENANCE_REQUIRED;
			}
			if ((maint_status & CM_MAINTENANCE_STATUS_DEMANDED) != 0) {
				sub_state |= CM_SUB_STATE_MAINTENANCE_DEMANDED;
			}
			if ((maint_status & CM_MAINTENANCE_STATUS_QUA_BITS) != 0) {
				sub_state |= CM_SUB_STATE_QUALIFIED_INFO_AVAILABLE;
			}

			if (sub_state != CM_SUB_STATE_GOOD) {
				CM_UPPER_PD_DIFF_AP_PTR_TYPE  diff_ap;
				CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  diff_mod;
				CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  diff_sub;

				diff_ap = cm_pd_diff_ap_find_append (pdprm->channel
					, & upper_event->u.pd.appl_ready.diff_list
					, 0 /*api*/
					);

				diff_mod = cm_pd_diff_module_find_append (pdprm->channel
					, & diff_ap->module_list
					, port->addr.slot_nr, port->addr.mod_ident
					);

				diff_sub = cm_pd_diff_submodule_find_append (pdprm->channel
					, & diff_mod->submodule_list
					, port->addr.subslot_nr, sub_state, port->addr.sub_ident
					);
				LSA_UNUSED_ARG (diff_sub);
			}
		}

		cm_pd_difflist_to_moddiffblock (pdprm->channel, LSA_NULL
			, & upper_event->u.pd.appl_ready.diff_list
			, & upper_event->u.pd.appl_ready.mod_diff_block_length
			, & upper_event->u.pd.appl_ready.mod_diff_block
			);

		if (rsp != CM_OK) { /* AP01427283 */

			upper_event->u.pd.appl_ready.diag_block_length = 0;
			upper_event->u.pd.appl_ready.diag_block = LSA_NULL;
		}
		else {

			cm_pd_diag_for_appl_ready (pdprm->channel
				, & upper_event->u.pd.appl_ready.diag_block_length
				, & upper_event->u.pd.appl_ready.diag_block
				);

			cm_pd_diag_move_to_user_diag (pdprm->channel);
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: move cm_pd_diag_move_to_user_diag() to caller cm_pd_prm_sc_indication_done() because 'move' is a side-effect")
#endif /* CM_MESSAGE */
		}

		upper_event->u.pd.appl_ready.rsv_interval_red = pdprm->channel->usr.pd.rsv_interval_red; /* AP00964333 and AP01317720 */
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_end_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_end_done")

	CM_PD_TRACE_05 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_END_DONE: wait_for(0x%x) wait_sc(0x%x) response(%u) arp(0x%x) wait_sc_done(0x%x)"
		, pdprm->wait_for, pdprm->wait_sc, pdprm->response, pdprm->arp, wait_sc_done
		);
	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_END_CNF);

	CM_ASSERT ((pdprm->wait_sc & wait_sc_done) == wait_sc_done);
	pdprm->wait_sc &= ~ wait_sc_done;

	if (pdprm->response != CM_OK)
	{
			/* set interface to arp */
		cm_pd_prm_set_arp (pdprm, pdprm->response, 0, wait_sc_done);
	}

	if (pdprm->wait_sc != 0)
	{
			/* wait.. */
		return;
	}

	pdprm->wait_for &= ~ CM_PD_PRM_WF_END_CNF;

	cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_END);
}


/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  COMMIT
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_commit")

	CM_ASSERT (! (pdprm->wait_for & CM_PD_PRM_WF_COMMIT_CNF));
	CM_ASSERT (! (pdprm->wait_for & CM_PD_PRM_WF_FIRST_IND));
	CM_ASSERT (pdprm->wait_sc == 0);
	CM_ASSERT (pdprm->wait_sc_ind == 0);
	CM_ASSERT (pdprm->arp == 0);

	pdprm->state = CM_PD_PRM_STATE_WF_COMMIT;

	pdprm->wait_for |= CM_PD_PRM_WF_COMMIT_CNF;
	pdprm->wait_for |= CM_PD_PRM_WF_FIRST_IND;

	CM_MEMSET (pdprm->wait_first_ind, 0, sizeof(pdprm->wait_first_ind));

	pdprm->is_parameterized = LSA_TRUE;

	pdprm->response = CM_OK;

	{
		LSA_UINT16  port_id;

		for (port_id = 0; port_id <= pdprm->channel->usr.pd.port_count; ++port_id)
		{
			CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (pdprm->channel, port_id);

			cm_pd_diag_clear (pdprm->channel, port);
		}
	}

#if CM_CFG_MAX_SERVERS
	cm_sv_pdev_diag_changed (pdprm->channel, CM_PD_INVALID_PORT_ID);
#endif

	/* prepare wait bits, see "ignore old ind" in e.g. cm_pd_oha_prm_indication_done() */
	{
		pdprm->wait_sc |= CM_PD_SCBF_EDD;

		pdprm->wait_first_ind[CM_PD_SCID_EDD] = cm_edd_prm_provide_mask (pdprm->channel);
		CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_EDD] != 0);

		pdprm->wait_sc_ind |= CM_PD_SCBF_EDD;
	}
	{
		pdprm->wait_sc |= CM_PD_SCBF_OHA;

		pdprm->wait_first_ind[CM_PD_SCID_OHA] = cm_oha_prm_provide_mask (pdprm->channel);
		CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_OHA] != 0);

		pdprm->wait_sc_ind |= CM_PD_SCBF_OHA;
	}
#if CM_CFG_USE_MRP
	{
		pdprm->wait_sc |= CM_PD_SCBF_MRP;

		pdprm->wait_first_ind[CM_PD_SCID_MRP] = cm_mrp_prm_provide_mask (pdprm->channel);
		CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_MRP] != 0);

		pdprm->wait_sc_ind |= CM_PD_SCBF_MRP;
	}
#endif
#if CM_CFG_USE_GSY
	{
		pdprm->wait_sc |= CM_PD_SCBF_GSY_CLOCK;

		pdprm->wait_first_ind[CM_PD_SCID_GSY_CLOCK] = cm_gsy_prm_provide_mask (pdprm->channel);
		CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_GSY_CLOCK] != 0);

		pdprm->wait_sc_ind |= CM_PD_SCBF_GSY_CLOCK;
	}
	if (cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_FALSE))
	{
		{
			pdprm->wait_sc |= CM_PD_SCBF_GSY_TIME;

			pdprm->wait_first_ind[CM_PD_SCID_GSY_TIME] = cm_gsy_prm_provide_mask (pdprm->channel);
			CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_GSY_TIME] != 0);

			pdprm->wait_sc_ind |= CM_PD_SCBF_GSY_TIME;
		}
	}
#endif
#if CM_CFG_USE_POF
	{
		pdprm->wait_sc |= CM_PD_SCBF_POF;

		pdprm->wait_first_ind[CM_PD_SCID_POF] = cm_pof_prm_provide_mask (pdprm->channel);
		CM_ASSERT (pdprm->wait_first_ind[CM_PD_SCID_POF] != 0);

		pdprm->wait_sc_ind |= CM_PD_SCBF_POF;
	}
#endif

	/* CM_PD_SCID_EDD */
	cm_edd_prm_commit (pdprm->channel);
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit2 (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_commit2")
	LSA_BOOL  is_local;

	CM_ASSERT ((pdprm->wait_for & CM_PD_PRM_WF_COMMIT_CNF));
	CM_ASSERT ((pdprm->wait_for & CM_PD_PRM_WF_FIRST_IND));

	CM_ASSERT (! (pdprm->wait_sc & CM_PD_SCBF_EDD));

	is_local = (pdprm->wait_cnf & CM_PD_CONFIG_PRM_USR) != 0 ? LSA_TRUE : LSA_FALSE;

	CM_PD_TRACE_02 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "continue-commit, end_counter(%u), is_local(%u)", pdprm->end_counter, is_local
		);

	/* FSU: OHA is the second */
	/* note: usecase TIA 833256 utilizes the "is_local" flag, see OHA */
	cm_oha_prm_commit (pdprm->channel, is_local);

#if CM_CFG_USE_MRP
	cm_mrp_prm_commit (pdprm->channel);
#endif

#if CM_CFG_USE_GSY
	cm_gsy_prm_commit (pdprm->channel, GSY_SYNCID_CLOCK);

	if (cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_FALSE))
	{
		cm_gsy_prm_commit (pdprm->channel, GSY_SYNCID_TIME);
	}
#endif

#if CM_CFG_USE_POF
	cm_pof_prm_commit (pdprm->channel);
#endif

	if (pdprm->wait_sc == 0)
	{
		cm_pd_prm_sc_commit_done (pdprm, 0/*trigger*/);
	}
	else
	{
		CM_PD_TRACE_02 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_COMMIT: sc_commit-waiting wait_for(0x%x) wait_sc(0x%x)"
			, pdprm->wait_for, pdprm->wait_sc
			);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_commit_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (CM_EDD_GET_RESPONSE (edd) != EDD_STS_OK) {
		CM_FATAL1 (CM_EDD_GET_RESPONSE (edd));
	}

	/***/

	{
	CM_RESERVED_INTERVAL_IF_RED_PTR_TYPE rsv = &channel->usr.pd.rsv_interval_red;
	EDD_UPPER_PRM_COMMIT_PTR_TYPE params = CM_EDD_GET_PPARAM (edd, EDD_UPPER_PRM_COMMIT_PTR_TYPE);

	rsv->RxEndNs                = params->RsvIntervalRed.RxEndNs;
	rsv->RxLocalTransferEndNs   = params->RsvIntervalRed.RxLocalTransferEndNs;
	rsv->TxEndNs                = params->RsvIntervalRed.TxEndNs;
	rsv->TxLocalTransferStartNs = params->RsvIntervalRed.TxLocalTransferStartNs;
	rsv->TxLocalTransferEndNs   = params->RsvIntervalRed.TxLocalTransferEndNs;
	}

	/***/

/*
//	not:	cm_pd_prm_sc_commit_done (pdprm, CM_PD_SCBF_EDD);
//
//	because of serialisation: first edd, then the others
*/

	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_COMMIT_CNF);

	CM_ASSERT ((pdprm->wait_sc & CM_PD_SCBF_EDD) == CM_PD_SCBF_EDD);
	pdprm->wait_sc &= ~ CM_PD_SCBF_EDD;

	CM_ASSERT (pdprm->wait_sc != 0);
	cm_pd_prm_sc_commit2 (pdprm);
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_commit_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (CM_MRP_GET_RESPONSE (mrp) != MRP_RSP_OK) {
		CM_FATAL1 (CM_MRP_GET_RESPONSE (mrp));
	}

	cm_pd_prm_sc_commit_done (pdprm, CM_PD_SCBF_MRP);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_commit_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16  sc_id, sc_bf;

	if (CM_GSY_GET_RESPONSE (gsy) != GSY_RSP_OK) {
		CM_FATAL1 (CM_GSY_GET_RESPONSE (gsy));
	}

	cm_pd_sc_id_from_sync_id (sync_id, & sc_id, & sc_bf);

	cm_pd_prm_sc_commit_done (pdprm, sc_bf);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_commit_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (CM_POF_GET_RESPONSE (pof) != POF_RSP_OK) {
		CM_FATAL1 (CM_POF_GET_RESPONSE (pof));
	}

	cm_pd_prm_sc_commit_done (pdprm, CM_PD_SCBF_POF);
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_commit_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (CM_OHA_GET_RESPONSE (oha) != OHA_OK) {
		CM_FATAL1 (CM_OHA_GET_RESPONSE (oha));
	}

	cm_pd_prm_sc_commit_done (pdprm, CM_PD_SCBF_OHA);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_commit_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_done
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_commit_done")

	CM_PD_TRACE_05 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_COMMIT_DONE: wait_for(0x%x) wait_sc(0x%x) wait_sc_ind(0x%x) response(%u) wait_sc_done(0x%x)"
		, pdprm->wait_for, pdprm->wait_sc, pdprm->wait_sc_ind, pdprm->response, wait_sc_done
		);
	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_COMMIT_CNF);

	CM_ASSERT ((pdprm->wait_sc & wait_sc_done) == wait_sc_done);
	pdprm->wait_sc &= ~ wait_sc_done;

	if (pdprm->wait_sc != 0) {
		/* wait.. */
		return;
	}

	pdprm->wait_for &= ~ CM_PD_PRM_WF_COMMIT_CNF;

	if (pdprm->response != CM_OK)
	{
		CM_FATAL(); /* see TODO: prealloc rqb for xxx_  _prepare _end _commit */
	}

	cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_COMMIT);

	/***/

	pdprm->channel->usr.pd.sync.tell_oha = LSA_TRUE; /* fake a delta (OHA resets state on PrmCommit) */
	cm_pd_oha_write_ptcp_trigger (pdprm->channel, LSA_NULL);
}


/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  INDICATION
//----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_indication (
	CM_PD_PRM_PTR_TYPE		pdprm
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_indication")

	CM_ASSERT ((pdprm->wait_for & CM_PD_PRM_WF_FIRST_IND));	/* set with commit.req */
	CM_ASSERT (pdprm->wait_sc == 0);

	pdprm->state = CM_PD_PRM_STATE_WF_SC_WORKING;

	/* CM_PD_SCID_EDD */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_EDD) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf edd.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_EDD]
			);
	}

#if CM_CFG_USE_MRP
	/* CM_PD_SCID_MRP */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_MRP) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf mrp.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_MRP]
			);
	}
#endif

#if CM_CFG_USE_GSY
	/* CM_PD_SCID_GSY_CLOCK */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_GSY_CLOCK) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf gsy-clock.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_GSY_CLOCK]
			);
	}

	/* CM_PD_SCID_GSY_TIME */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_GSY_TIME) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf gsy-time.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_GSY_TIME]
			);
	}
#endif

#if CM_CFG_USE_POF
	/* CM_PD_SCID_POF */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_POF) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf pof.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_POF]
			);
	}
#endif

	/* CM_PD_SCID_OHA */
	if (pdprm->wait_sc_ind & CM_PD_SCBF_OHA) {
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC_IND: wf oha.first.ind: portmask(0x%x)"
			, pdprm->wait_first_ind[CM_PD_SCID_OHA]
			);
	}

	if (pdprm->wait_sc_ind == 0) {
		cm_pd_prm_sc_indication_done (pdprm, 0/*!*/);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	EDD_UPPER_PRM_INDICATION_PTR_TYPE ind
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_indication_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_PD_PORT_PTR_TYPE  port;
	LSA_UINT16  diag_cnt;

	if( channel->state != CM_PD_READY ) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PRM_SC: edd-indication: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}
	if (! channel->usr.pd.pdprm.is_parameterized)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PRM_SC: edd-indication: ignored, pdev not parameterized");
		return;
	}

	port = cm_pd_port_from_id__nn (channel, ind->edd_port_id);
	diag_cnt = ind->diag_cnt;

		/* ignore old ind */
	if (pdprm->state == CM_PD_PRM_STATE_WF_COMMIT  &&  (pdprm->wait_sc & CM_PD_SCBF_EDD) != 0) {

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: edd-indication: ignore: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x)"
			, port->port_id, diag_cnt, pdprm->state, pdprm->wait_sc_ind
			);
	}

		/* else update diag */
	else {
		LSA_UINT16  i;

		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: edd-indication: update: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x) wait_first_ind(0x%x)"
			, port->port_id, diag_cnt
			, pdprm->state, pdprm->wait_sc_ind, pdprm->wait_first_ind[CM_PD_SCID_EDD]
			);

		cm_pd_diag_update_start (channel, port);
		for (i = 0; i < diag_cnt; ++i)
		{
			cm_pd_diag_update (channel, port
				, ind->diag[i].ChannelProperties, ind->diag[i].ChannelErrorType
				, ind->diag[i].ExtChannelErrorType, ind->diag[i].ExtChannelAddValue
				);
		}
		cm_pd_diag_update_done (channel, port);

		if ((pdprm->wait_sc_ind & CM_PD_SCBF_EDD) /*AP00346893 pdprm->wait_first_ind[CM_PD_SCID_EDD] != 0*/)
		{
			CM_PORT_UINT msk = 1;
			msk <<= port->port_id;

			CM_ASSERT ((pdprm->wait_sc & CM_PD_SCBF_EDD) == 0);
			CM_ASSERT ((pdprm->wait_sc_ind & CM_PD_SCBF_EDD) != 0);

			pdprm->wait_first_ind[CM_PD_SCID_EDD] &= ~msk;

			if (pdprm->wait_first_ind[CM_PD_SCID_EDD] == 0)
			{
				cm_pd_prm_sc_indication_done (pdprm, CM_PD_SCBF_EDD);
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	MRP_PRM_INDICATION_PTR_TYPE	ind
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_indication_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_PD_PORT_PTR_TYPE  port;
	LSA_UINT16  diag_cnt;

	if( channel->state != CM_PD_READY ) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PRM_SC: mrp-indication: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}
	if (! channel->usr.pd.pdprm.is_parameterized)
	{
		CM_PD_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PRM_SC: mrp-indication: ignored, pdev not parameterized");
		return;
	}

	port = cm_pd_port_from_id__nn (channel, ind->edd_port_id);
	diag_cnt = ind->diag_cnt;

	if( ind->edd_port_id != 0/*interface*/  &&  diag_cnt != 0) {
		/* for ports 1..n: only a mrp_first.ind in order of a mrp_write.req are allowed */
		CM_FATAL(); /* not as documented */
	}

		/* ignore old ind */
	if (pdprm->state == CM_PD_PRM_STATE_WF_COMMIT  &&  (pdprm->wait_sc & CM_PD_SCBF_MRP) != 0) {

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: mrp-indication: ignore: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x)"
			, port->port_id, diag_cnt, pdprm->state, pdprm->wait_sc_ind
			);
	}

		/* else update diag */
	else {
		LSA_UINT16  i;

		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: mrp-indication: update: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x) wait_first_ind(0x%x)"
			, port->port_id, diag_cnt
			, pdprm->state, pdprm->wait_sc_ind, pdprm->wait_first_ind[CM_PD_SCID_MRP]
			);

		cm_pd_diag_update_start (channel, port);
		for (i = 0; i < diag_cnt; ++i) {

			cm_pd_diag_update (channel, port
				, ind->diag[i].ChannelProperties, ind->diag[i].ChannelErrorType
				, ind->diag[i].ExtChannelErrorType, ind->diag[i].ExtChannelAddValue
				);
		}
		cm_pd_diag_update_done (channel, port);

		if ((pdprm->wait_sc_ind & CM_PD_SCBF_MRP) /*AP00346893 pdprm->wait_first_ind[CM_PD_SCID_MRP] != 0*/)
		{
			CM_PORT_UINT msk = 1;
			msk <<= port->port_id;

			CM_ASSERT ((pdprm->wait_sc & CM_PD_SCBF_MRP) == 0);
			CM_ASSERT ((pdprm->wait_sc_ind & CM_PD_SCBF_MRP) != 0);

			pdprm->wait_first_ind[CM_PD_SCID_MRP] &= ~msk;

			if (pdprm->wait_first_ind[CM_PD_SCID_MRP] == 0) {
				cm_pd_prm_sc_indication_done (pdprm, CM_PD_SCBF_MRP);
			}
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_indication_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  sync_id,
	GSY_RQB_PRM_INDICATION_PROVIDE_TYPE GSY_UPPER_MEM_ATTR  *ind
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_indication_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16  sc_id, sc_done;
	CM_PD_PORT_PTR_TYPE  port;
	LSA_UINT16  diag_cnt;

	if( channel->state != CM_PD_READY ) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PRM_SC: gsy-indication: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}
	if (! channel->usr.pd.pdprm.is_parameterized)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PRM_SC: gsy-indication: ignored, pdev not parameterized");
		return;
	}

	cm_pd_sc_id_from_sync_id (sync_id, & sc_id, & sc_done);

	CM_ASSERT (sc_done == CM_PD_SCBF_GSY_CLOCK  ||  sc_done == CM_PD_SCBF_GSY_TIME);

	if( ind->EddPortId != 0/*interface*/ ) {
		CM_FATAL(); /* not as documented */
	}

	port = cm_pd_port_from_id__nn (channel, ind->EddPortId);
	diag_cnt = ind->DiagCnt;

		/* ignore old ind */
	if (pdprm->state == CM_PD_PRM_STATE_WF_COMMIT  &&  (pdprm->wait_sc & sc_done) != 0) {

		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: gsy-indication: ignore: port(%u) cnt(%u) sync_id(%u)   state(%u) wait_sc(0x%x)"
			, port->port_id, diag_cnt, sync_id
			, pdprm->state, pdprm->wait_sc
			);
	}

		/* else update diag */
	else {
		LSA_UINT16  i;

		CM_PD_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: gsy-indication: update: port(%u) cnt(%u) sync_id(%u)  state(%u) wait_sc_ind(0x%x) wait_first_ind(0x%x)"
			, port->port_id, diag_cnt, sync_id
			, pdprm->state, pdprm->wait_sc_ind, pdprm->wait_first_ind[sc_id]
			);

		cm_pd_diag_update_start (channel, port);
		for (i = 0; i < diag_cnt; ++i) {

			cm_pd_diag_update (channel, port
				, ind->pDiag[i].ChannelProperties, ind->pDiag[i].ChannelErrorType
				, ind->pDiag[i].ExtChannelErrorType, ind->pDiag[i].ExtChannelAddValue
				);
		}
		cm_pd_diag_update_done (channel, port);

		if ((pdprm->wait_sc_ind & sc_done) /*AP00346893 pdprm->wait_first_ind[CM_PD_SCID_GSY] != 0*/)
		{
			CM_PORT_UINT msk = 1;
			msk <<= port->port_id;

			CM_ASSERT ((pdprm->wait_sc & sc_done) == 0);
			CM_ASSERT ((pdprm->wait_sc_ind & sc_done) != 0);

			pdprm->wait_first_ind[sc_id] &= ~msk;

			if (pdprm->wait_first_ind[sc_id] == 0) {
				cm_pd_prm_sc_indication_done (pdprm, sc_done);
			}
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	POF_UPPER_PRM_INDICATION_PTR_TYPE ind
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_indication_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_PD_PORT_PTR_TYPE  port;
	LSA_UINT16  diag_cnt;

	if( channel->state != CM_PD_READY ) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PRM_SC: pof-indication: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}
	if (! channel->usr.pd.pdprm.is_parameterized)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PRM_SC: pof-indication: ignored, pdev not parameterized");
		return;
	}

	if( ind->edd_port_id == 0 ) {
		CM_FATAL(); /* not as documented */
	}

	port = cm_pd_port_from_id__nn (channel, ind->edd_port_id);
	diag_cnt = ind->diag_cnt;

		/* ignore old ind */
	if (pdprm->state == CM_PD_PRM_STATE_WF_COMMIT  &&  (pdprm->wait_sc & CM_PD_SCBF_POF) != 0) {

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: pof-indication: ignore: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x)"
			, port->port_id, diag_cnt, pdprm->state, pdprm->wait_sc
			);
	}

		/* else update diag */
	else {
		LSA_UINT16  i;

		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: pof-indication: update: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x) wait_first_ind(0x%x)"
			, port->port_id, diag_cnt
			, pdprm->state, pdprm->wait_sc_ind, pdprm->wait_first_ind[CM_PD_SCID_POF]
			);

		cm_pd_diag_update_start (channel, port);
		for (i = 0; i < diag_cnt; ++i) {

			cm_pd_diag_update (channel, port
				, ind->diag[i].ChannelProperties, ind->diag[i].ChannelErrorType
				, ind->diag[i].ExtChannelErrorType, ind->diag[i].ExtChannelAddValue
				);
		}
		cm_pd_diag_update_done (channel, port);

		if ((pdprm->wait_sc_ind & CM_PD_SCBF_POF) /*AP00346893 pdprm->wait_first_ind[CM_PD_SCID_POF] != 0*/)
		{
			CM_PORT_UINT msk = 1;
			msk <<= port->port_id;

			CM_ASSERT ((pdprm->wait_sc & CM_PD_SCBF_POF) == 0);
			CM_ASSERT ((pdprm->wait_sc_ind & CM_PD_SCBF_POF) != 0);

			pdprm->wait_first_ind[CM_PD_SCID_POF] &= ~msk;

			if (pdprm->wait_first_ind[CM_PD_SCID_POF] == 0) {
				cm_pd_prm_sc_indication_done (pdprm, CM_PD_SCBF_POF);
			}
		}
	}
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_indication_done (
	CM_CHANNEL_PTR_TYPE					channel,
	OHA_UPPER_PRM_INDICATION_PTR_TYPE	ind
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_indication_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_PD_PORT_PTR_TYPE  port;
	LSA_UINT16  diag_cnt;

	if( channel->state != CM_PD_READY ) {
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PRM_SC: oha-indication: ignored, state(%u) not pd-ready (assumption: after pd-delete)"
			, channel->state
			);
		return;
	}
	if (! channel->usr.pd.pdprm.is_parameterized)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "PRM_SC: oha-indication: ignored, pdev not parameterized");
		return;
	}

	port = cm_pd_port_from_id__nn (channel, ind->edd_port_id);
	diag_cnt = ind->diag_cnt;

		/* ignore old ind */
	if (pdprm->state == CM_PD_PRM_STATE_WF_COMMIT  &&  (pdprm->wait_sc & CM_PD_SCBF_OHA) != 0) {

		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: oha-indication: ignore: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x)"
			, port->port_id, diag_cnt, pdprm->state, pdprm->wait_sc
			);
	}

		/* else update diag */
	else {
		LSA_UINT16  i;

		CM_PD_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: oha-indication: update: port(%u) cnt(%u)   state(%u) wait_sc_ind(0x%x) wait_first_ind(0x%x)"
			, port->port_id, diag_cnt
			, pdprm->state, pdprm->wait_sc_ind, pdprm->wait_first_ind[CM_PD_SCID_OHA]
			);

		cm_pd_diag_update_start (channel, port);
		for (i = 0; i < diag_cnt; ++i) {

			cm_pd_diag_update (channel, port
				, ind->diag_ptr[i].ChannelProperties, ind->diag_ptr[i].ChannelErrorType
				, ind->diag_ptr[i].ExtChannelErrorType, ind->diag_ptr[i].ExtChannelAddValue
				);
		}
		if (port->port_id != 0) { /* always true for now (V3.12i25) */
			cm_pd_set_topo_state(channel, port);
		}
		cm_pd_diag_update_done (channel, port);

		if ((pdprm->wait_sc_ind & CM_PD_SCBF_OHA) /*AP00346893 pdprm->wait_first_ind[CM_PD_SCID_OHA] != 0*/)
		{
			CM_PORT_UINT msk = 1;
			msk <<= port->port_id;

			CM_ASSERT ((pdprm->wait_sc & CM_PD_SCBF_OHA) == 0);
			CM_ASSERT ((pdprm->wait_sc_ind & CM_PD_SCBF_OHA) != 0);

			pdprm->wait_first_ind[CM_PD_SCID_OHA] &= ~msk;

			if (pdprm->wait_first_ind[CM_PD_SCID_OHA] == 0) {
				cm_pd_prm_sc_indication_done (pdprm, CM_PD_SCBF_OHA);
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_indication_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				wait_sc_ind_done
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_indication_done")

	CM_PD_TRACE_04 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PRM_SC_IND_DONE: wait_for(0x%x) wait_sc(0x%x) wait_sc_ind(0x%x)  wait_sc_ind_done(0x%x)"
		, pdprm->wait_for, pdprm->wait_sc, pdprm->wait_sc_ind, wait_sc_ind_done
		);
	CM_ASSERT (pdprm->wait_for & CM_PD_PRM_WF_FIRST_IND);

	CM_ASSERT ((pdprm->wait_sc_ind & wait_sc_ind_done) == wait_sc_ind_done);
	pdprm->wait_sc_ind &= ~ wait_sc_ind_done;


	if (wait_sc_ind_done == CM_PD_SCBF_EDD)
	{
		if (cm_edd_get_link_status (pdprm->channel))
		{
			pdprm->wait_sc_ind |= CM_PD_SCBF_EDD_LINK_STATUS;
		}
		else
		{
			CM_PD_TRACE_00 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "PRM_SC_IND_DONE: ignore 'no-mem' error from cm_edd_get_link_status"
				);
		}
	}

	if (pdprm->wait_sc != 0  ||  pdprm->wait_sc_ind != 0) {
		/* wait.. */
		return;
	}

	pdprm->wait_for &= ~ CM_PD_PRM_WF_FIRST_IND;

	pdprm->state = CM_PD_PRM_STATE_WORKING;

	cm_pd_config_trigger (pdprm, CM_PD_CONFIG__WF_FIRST_IND);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_link_status_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_link_status_done")
	EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  edd_link_status;

	if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK)
	{
		CM_FATAL1 (CM_EDD_GET_RESPONSE(edd));
		return;
	}

	{
	CM_CHANNEL_PTR_TYPE  edd_channel = cm_edd_channel_get (channel);

	edd_link_status = CM_EDD_GET_PPARAM(edd, EDD_UPPER_GET_LINK_STATUS_PTR_TYPE);

	CM_ASSERT (is_not_null (edd_link_status));
	CM_ASSERT(edd_link_status->PortID == EDD_PORT_ID_AUTO);

	cm_edd_link_data_changed (edd_channel, edd_link_status->PortID
		, edd_link_status->Status, edd_link_status->Speed, edd_link_status->Mode
		, 0xFF
		, EDD_AUTONEG_UNKNOWN
		);
	}

	cm_edd_free_rqb (channel, edd);

		/* trigger prm-statemachine */
	cm_pd_prm_sc_indication_done (&channel->usr.pd.pdprm, CM_PD_SCBF_EDD_LINK_STATUS);
}


/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdcl_pdev_is_ready_for_schedule (
	CM_CHANNEL_PTR_TYPE  cl_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(cl_channel, CM_PATH_TYPE_PD);

	if (pd_channel->usr.pd.pdprm.last_appl_ready_ind != CM_OK)
	{
		return LSA_FALSE;
	}
	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdcl_master_tailor_device_added (
	CM_CHANNEL_PTR_TYPE  cl_channel
	)
{
	CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(cl_channel, CM_PATH_TYPE_PD);

	/*
	//	intended usage: 
	//
	//	if gerade-geadded-device-gehört-zu-einen-masterproject:
	//		cm_pdcl_master_tailor_device_added ();
	*/

	cm_pd_config_start (pd_channel, CM_PD_CONFIG_CL_DEVICE_ADD);
}

/*----------------------------------------------------------------------------*/
static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_pdev_is_ready_for_tailoring ( /* note: the pdev of a non series machine is always ready */
	CM_PD_PRM_PTR_TYPE  pdprm
	)
{
	CM_LOG_FCT("cm_pd_pdev_is_ready_for_tailoring")
	CM_CHANNEL_PTR_TYPE pd_channel = pdprm->channel;
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (pd_channel, 0/*PDev*/);

	if (is_not_null (pd_mtd))
	{
			/* temporary: RtfNosIpChanged is not ready yet */
		if (pd_channel->usr.pd.rtf.wf_sm != 0)
		{
			return LSA_FALSE;
		}

			/* temporary: PDInstanceTailorData running */
		if (pd_channel->usr.pd.reclist.is_running_mpa)
		{
			return LSA_FALSE;
		}

		if (pd_mtd->properties & CM_PD_MTD_PROP_IP_SUITE)
		{
			LSA_UINT32  ip_addr, subnet, gateway;

			if (!cm_oha_get_ip_suite (pd_channel, &ip_addr, &subnet, &gateway))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & CM_PD_MTD_PROP_NOS)
		{
			LSA_UINT16  cmi_station_name_length;
			CM_UPPER_MEM_U8_PTR_TYPE  cmi_station_name;

			if (! cm_oha_get_name_of_station(pd_channel, &cmi_station_name_length, &cmi_station_name))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & (CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			if (! (pd_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
			{
				return LSA_FALSE;
			}
		}

		if (pd_mtd->properties & (CM_PD_MTD_PROP_NOS | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			if (! cm_pd_mtd_neighbour_exists (pd_channel, pd_mtd))
			{
				return LSA_FALSE;
			}
		}
	}
	return LSA_TRUE;
}


/*------------------------------------------------------------------------------
// CM-PD-CONFIG
//
//	see serienmaschine.doc => PDev-Config-StateMachine
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_start (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg
	)
{
	CM_LOG_FCT("cm_pd_config_start")
	CM_PD_PRM_PTR_TYPE  pdprm = & pd_channel->usr.pd.pdprm;

	if (start_cfg & (CM_PD_CONFIG_PRM_USR | CM_PD_CONFIG_PRM_SV))
	{
		/* PDev configuration changed */
	}
	else if (start_cfg & CM_PD_CONFIG_RESET_TO_FACTORY)
	{
		CM_ASSERT (pdprm->is_reset_to_factory);
		/* change PDev configuration */
	}
	else if (start_cfg & CM_PD_CONFIG_NOS_IP)
	{
		if (cm_pdusr_is_masterproject(pd_channel))
		{
			/* PDev configuration may change */
		}
		else
		{
			return; /* nothing to do .. */
		}
	}
	else if (start_cfg & CM_PD_CONFIG_CL_DEVICE_ADD)
	{
		if (cm_pdusr_is_masterproject(pd_channel)  &&  pdprm->last_appl_ready_ind != CM_OK)
		{
			/* PDev configuration may change */
		}
		else
		{
			return; /* nothing to do .. */
		}
	}
	else if (start_cfg & CM_PD_CONFIG_MACHINE_TAILOR)
	{
		if (cm_pdusr_is_masterproject(pd_channel))
		{
			/* PDev configuration may change */
		}
		else
		{
			return; /* nothing to do .. */
		}
	}
	else
	{
		CM_FATAL1 (start_cfg);
		return;
	}

	if (pdprm->wait_cnf != 0)
	{
		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: PD_CONFIG request-queue; wait_cnf(0x%x);  wait_cnf_next(0x%x) + start_cfg(0x%x)", pdprm->wait_cnf, pdprm->wait_cnf_next, start_cfg
			);
		pdprm->wait_cnf_next |= start_cfg;
	}
	else
	{
		pdprm->wait_cnf = start_cfg;

		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: PD_CONFIG request; wait_cnf(0x%x) ", pdprm->wait_cnf
			);
		CM_RQB_SET_HANDLE (pdprm->pd_config_rqb, CM_INVALID_HANDLE);
		CM_RQB_SET_OPCODE (pdprm->pd_config_rqb, CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER);
		cm_pd_req_queue_add (pdprm->channel, pdprm->pd_config_rqb); /* => cm_pd_config_trigger() */
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_trigger (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT32  wf_done
	)
{
	CM_LOG_FCT("cm_pd_config_trigger")

	if (wf_done == CM_PD_CONFIG_START)
	{
		LSA_BOOL  reclist_is_valid		= pdprm->channel->usr.pd.reclist.is_valid;
		LSA_BOOL  is_reset_to_factory	= pdprm->is_reset_to_factory;
		LSA_BOOL  reclist_prm_error		= pdprm->channel->usr.pd.reclist.has_error;
		LSA_BOOL  pdev_is_ready			= cm_pd_pdev_is_ready_for_tailoring (pdprm);

		if (! reclist_is_valid)
		{
			pdprm->wait_cnf |= CM_PD_CONFIG__NO_ARDY_IND; /* PRM_USR or PRM_SV end is pending */
		}
		else
		{
			if (pdprm->wait_cnf & (CM_PD_CONFIG_PRM_USR | CM_PD_CONFIG_PRM_SV))
			{
				pdprm->is_reset_to_factory	= LSA_FALSE; /* may be set within cm_pd_rtf_nos_ip_changed() */
				is_reset_to_factory			= LSA_FALSE;
			}

			pdprm->wait_cnf |= CM_PD_CONFIG__DO_BEGIN | CM_PD_CONFIG__DO_SEND_CLOCK | CM_PD_CONFIG__DO_WRITE;
			pdprm->wait_cnf |= CM_PD_CONFIG__DO_END   | CM_PD_CONFIG__DO_COMMIT     | CM_PD_CONFIG__DO_FIRST_IND;

			if (is_reset_to_factory)
			{
				pdprm->wait_cnf |= CM_PD_CONFIG__RESET_TO_FACTORY;
				cm_pd_reclist_reset_to_factory (pdprm->channel);
			}
			else if (reclist_prm_error)
			{
				pdprm->wait_cnf |= CM_PD_CONFIG__PDEV_PRM_ERROR;
			}
			else if (! pdev_is_ready)
			{
				pdprm->wait_cnf |= CM_PD_CONFIG__PDEV_NOT_READY;
			}
			else
			{
				if (cm_pd_reclist_tailor (pdprm->channel) != CM_OK)
				{
					pdprm->wait_cnf |= CM_PD_CONFIG__PDEV_PRM_ERROR; /* TIA#829122 */
				}
			}
		}

		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PRM_SC: PD_CONFIG start; wait_cnf(0x%x) ", pdprm->wait_cnf
			);
	}
	else if (wf_done != 0)
	{
		CM_PD_TRACE_02 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC: PD_CONFIG trigger-done-wait_cnf(0x%x); wait_cnf(0x%x)", wf_done, pdprm->wait_cnf
			);
		pdprm->wait_cnf &= ~ wf_done;

		if (wf_done == CM_PD_CONFIG__WF_WRITE)
		{
			pdprm->wait_cnf |= CM_PD_CONFIG__DO_WRITE;
		}
		else if (wf_done == CM_PD_CONFIG__WF_END)
		{
			if (pdprm->arp)
			{
				if (pdprm->wait_cnf & CM_PD_CONFIG__PDEV_PRM_ERROR)
				{
					CM_FATAL(); /* CM_ERR_PRM_CONSISTENCY with empty parameterization, maybe an engineering problem with SendClockFactor */
				}
				else
				{
					/* restart with empty parameterization */
					pdprm->arp = 0;

					pdprm->wait_cnf |= CM_PD_CONFIG__PDEV_PRM_ERROR;
					pdprm->wait_cnf |= CM_PD_CONFIG__DO_BEGIN | CM_PD_CONFIG__DO_SEND_CLOCK | CM_PD_CONFIG__DO_WRITE;
					pdprm->wait_cnf |= CM_PD_CONFIG__DO_END   | CM_PD_CONFIG__DO_COMMIT     | CM_PD_CONFIG__DO_FIRST_IND;
				}
			}
		}
	}
	else
	{
		CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "PRM_SC: PD_CONFIG trigger; wait_cnf(0x%x)", pdprm->wait_cnf
			);
	}

	/***/

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_BEGIN)
	{
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_BEGIN;

		pdprm->write = LSA_NULL;

		pdprm->wait_cnf |= CM_PD_CONFIG__WF_BEGIN;
		cm_pd_prm_sc_prepare (pdprm);
	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_BEGIN)
	{
		return;
	}

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_SEND_CLOCK)
	{
		LSA_UINT16  rc;
		
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_SEND_CLOCK;
		
		rc = cm_pd_send_clock_set (pdprm->channel, pdprm->channel->usr.pd.reclist.prm_data.scr_send_clock, pdprm->channel->usr.pd.reclist.prm_data.scr_sendclock_prop);

		switch (rc)
		{
		case CM_OK:	/* send_clock is set */
			break;

		case CM_OK_PENDING:
			pdprm->wait_cnf |= CM_PD_CONFIG__WF_SEND_CLOCK;
			break;

		default:
			cm_pd_prm_set_arp (pdprm, rc, 0, CM_PD_SCBF_CMPD); /* thus CM-PD will never commit this reclist */
			break;
		}

	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_SEND_CLOCK)
	{
		return;
	}

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_WRITE)
	{
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_WRITE;

		if ((pdprm->wait_cnf & (CM_PD_CONFIG__PDEV_NOT_READY | CM_PD_CONFIG__PDEV_PRM_ERROR)) != 0)
		{
			pdprm->write = cm_pd_reclist_get_next (pdprm->channel, pdprm->write, CM_PD_RECLIST_EMPTY);
		}
		else if ((pdprm->wait_cnf & (CM_PD_CONFIG__RESET_TO_FACTORY)) != 0)
		{
			pdprm->write = cm_pd_reclist_get_next (pdprm->channel, pdprm->write, CM_PD_RECLIST_RTF);
		}
		else
		{
			pdprm->write = cm_pd_reclist_get_next (pdprm->channel, pdprm->write, CM_PD_RECLIST_REGULAR/*normal prm, may tailored*/);
		}
		if (is_not_null (pdprm->write))
		{
			pdprm->wait_cnf |= CM_PD_CONFIG__WF_WRITE;
			cm_pd_prm_sc_write (pdprm);
		}
	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_WRITE)
	{
		return;
	}

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_END)
	{
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_END;

		pdprm->wait_cnf |= CM_PD_CONFIG__WF_END;
		cm_pd_prm_sc_end (pdprm);
	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_END)
	{
		return;
	}

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_COMMIT)
	{
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_COMMIT;

		pdprm->wait_cnf |= CM_PD_CONFIG__WF_COMMIT;
		cm_pd_prm_sc_commit (pdprm);
	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_COMMIT)
	{
		return;
	}

	if (pdprm->wait_cnf & CM_PD_CONFIG__DO_FIRST_IND)
	{
		pdprm->wait_cnf &= ~ CM_PD_CONFIG__DO_FIRST_IND;

		pdprm->wait_cnf |= CM_PD_CONFIG__WF_FIRST_IND;
		cm_pd_prm_sc_indication (pdprm);
	}
	if (pdprm->wait_cnf & CM_PD_CONFIG__WF_FIRST_IND)
	{
		return;
	}

	/* done */
	if (pdprm->wait_cnf & CM_PD_CONFIG__PDEV_NOT_READY)
	{
		pdprm->wait_cnf &= ~CM_PD_CONFIG__PDEV_NOT_READY;
		pdprm->last_appl_ready_ind = CM_OK_ADAPTION_NEEDED;
	}
	else if (pdprm->wait_cnf & CM_PD_CONFIG__PDEV_PRM_ERROR)
	{
		pdprm->wait_cnf &= ~CM_PD_CONFIG__PDEV_PRM_ERROR;
		pdprm->last_appl_ready_ind = CM_ERR_PRM_CONSISTENCY;
	}
	else if (pdprm->wait_cnf & CM_PD_CONFIG__RESET_TO_FACTORY)
	{
		pdprm->wait_cnf &= ~CM_PD_CONFIG__RESET_TO_FACTORY;
		pdprm->last_appl_ready_ind = CM_OK_RESET_TO_FACTORY;
	}
	else if (pdprm->wait_cnf & CM_PD_CONFIG__NO_ARDY_IND)
	{
		pdprm->wait_cnf &= ~CM_PD_CONFIG__NO_ARDY_IND;
		pdprm->last_appl_ready_ind = CM_OK_CANCELLED;
	}
	else
	{
		pdprm->last_appl_ready_ind = CM_OK;
	}

	CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "PRM_SC: PD_CONFIG request-done; wait_cnf(0x%x) with cm-rsp(%u); next-wait_cnf(0x%x)", pdprm->wait_cnf, pdprm->last_appl_ready_ind, pdprm->wait_cnf_next
		);

	/* callback CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER  =>  cm_pd_config_callback() */
	cm_pd_req_callback (pdprm->channel, pdprm->last_appl_ready_ind, pdprm->pd_config_rqb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_callback (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT16  rsp,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_ASSERT (are_equal (rb, pdprm->pd_config_rqb));
	LSA_UNUSED_ARG (rb);

	switch (pdprm->wait_cnf)
	{
	case CM_PD_CONFIG_PRM_USR:
		cm_pd_config_done (pdprm->channel, CM_PD_CONFIG_PRM_USR, rsp);
		break;

	case CM_PD_CONFIG_PRM_SV:
		cm_pd_config_done (pdprm->channel, CM_PD_CONFIG_PRM_SV, rsp);
		break;

	case CM_PD_CONFIG_NOS_IP:
	case CM_PD_CONFIG_CL_DEVICE_ADD:
	case CM_PD_CONFIG_RESET_TO_FACTORY:
	case CM_PD_CONFIG_MACHINE_TAILOR:
		if (rsp != CM_OK_CANCELLED) /* see CM_PD_CONFIG__NO_ARDY_IND */
		{
			cm_pd_config_done (pdprm->channel, (LSA_UINT16)pdprm->wait_cnf, rsp);
		}
		break;

	default:
		CM_FATAL1 (pdprm->wait_cnf);
		return;
	}

		/* all done */	
	pdprm->wait_cnf = 0;


		/* restart */
	if (pdprm->wait_cnf_next != 0)
	{
		LSA_UINT16 next = pdprm->wait_cnf_next;
		pdprm->wait_cnf_next = 0;

		if (next & CM_PD_CONFIG_PRM_SV) /* pdprm->end needs a confirmation */
		{
			cm_pd_config_start (pdprm->channel, CM_PD_CONFIG_PRM_SV);
		}
		else if (next & CM_PD_CONFIG_PRM_USR) /* USR takes precedence over RTF */
		{
			cm_pd_config_start (pdprm->channel, CM_PD_CONFIG_PRM_USR);
		}
		else if (next & CM_PD_CONFIG_RESET_TO_FACTORY)
		{
			cm_pd_config_start (pdprm->channel, CM_PD_CONFIG_RESET_TO_FACTORY);
		}
		else if (next & (CM_PD_CONFIG_NOS_IP | CM_PD_CONFIG_CL_DEVICE_ADD)) /* address-tailoring */
		{
			cm_pd_config_start (pdprm->channel, CM_PD_CONFIG_NOS_IP);
		}
		else if (next & CM_PD_CONFIG_MACHINE_TAILOR) /* 1156766 */
		{
			cm_pd_config_start (pdprm->channel, CM_PD_CONFIG_MACHINE_TAILOR);
		}
		else
		{
			CM_FATAL1 (next);
		}
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
