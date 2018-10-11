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
/*  F i l e               &F: cm_pd7.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice PrmRead                                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  870
#define CM_MODULE_ID       870

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
// CM-PD-PRM-SC  READ
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR		/* CM_ERR_xxx:sync-error  CM_OK_PENDING:wait-for-lower-cnf */
cm_pd_prm_sc_read (
	CM_PD_PRM_PTR_TYPE		pdprm,
	CM_UPPER_RQB_PTR_TYPE	read,
	LSA_UINT16				port_id
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_read")
	LSA_UINT16				response;

	LSA_UINT16				rc;
	LSA_UINT16				slot_nr;
	LSA_UINT16				subslot_nr;
	LSA_UINT32				record_index;
	LSA_UINT32				cls;
	LSA_UINT32				record_data_length;
	CM_UPPER_MEM_PTR_TYPE	record_data;
	LSA_UINT16				scid;

	{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = read->args.pd.prm_rdwr;

	slot_nr				= prm_rdwr->slot_nr;
	subslot_nr			= prm_rdwr->subslot_nr;
	record_index		= prm_rdwr->record_index;
	cls					= prm_rdwr->cls;
	record_data_length	= prm_rdwr->record_data_length;
	record_data			= prm_rdwr->record_data;
	}

	/* note: 937815: do not read_from_reclist, always read records from previously prm-ed subcomponent, see also PDApplRdy.ind */

	scid = (LSA_UINT16)(cls & CM_REC_IDX_CMPD_MASK);

	CM_PD_TRACE_05 (pdprm->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PD-read: index(0x%x) from sub_comp_id(0x%x), port_id(%u) slot(%u) subslot(0x%x)"
		, record_index, scid, port_id, slot_nr, subslot_nr
		);

	if (is_not_null (pdprm->read) || (pdprm->wait_for & CM_PD_PRM_WF_READ_CNF) != 0)
	{
		CM_FATAL();
	}

	pdprm->read = read;
	pdprm->wait_for |= CM_PD_PRM_WF_READ_CNF;

	response = CM_OK_PENDING;

	/* CM_PD_SCID_EDD */
	if (scid == CM_PD_SCID_EDD)
	{
		rc = cm_edd_prm_read (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		if (rc != CM_OK) {
			response = CM_ERR_PRM_DATA;
		}
	}

#if CM_CFG_USE_MRP
	/* CM_PD_SCID_MRP */
	else if (scid == CM_PD_SCID_MRP)
	{
		rc = cm_mrp_prm_read (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		if (rc != CM_OK) {
			response = CM_ERR_PRM_DATA;
		}
	}
#endif

#if CM_CFG_USE_GSY
	/* CM_PD_SCID_GSY_CLOCK */
	else if (scid == CM_PD_SCID_GSY_CLOCK)
	{
		rc = cm_gsy_prm_read (pdprm->channel, GSY_SYNCID_CLOCK
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		if (rc != CM_OK) {
			response = CM_ERR_PRM_DATA;
		}
	}

	/* CM_PD_SCID_GSY_TIME */
	else if (scid == CM_PD_SCID_GSY_TIME)
	{
		if (! cm_pd_gsy_syncid_exists (pdprm->channel, GSY_SYNCID_TIME, LSA_TRUE)) { /*AP00496114*/
			response = CM_ERR_PRM_INDEX;
		}
		else {
			rc = cm_gsy_prm_read (pdprm->channel, GSY_SYNCID_TIME
				, port_id, slot_nr, subslot_nr, record_index
				, record_data_length, record_data
				);
			if (rc != CM_OK) {
				response = CM_ERR_PRM_DATA;
			}
		}
	}
#endif

#if CM_CFG_USE_POF
	/* CM_PD_SCID_POF */
	else if (scid == CM_PD_SCID_POF)
	{
		rc = cm_pof_prm_read (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		if (rc != CM_OK) {
			response = CM_ERR_PRM_DATA;
		}
	}
#endif

	/* CM_PD_SCID_OHA */
	else if (scid == CM_PD_SCID_OHA)
	{
		rc = cm_oha_prm_read (pdprm->channel
			, port_id, slot_nr, subslot_nr, record_index
			, record_data_length, record_data
			);
		if (rc != CM_OK) {
			response = CM_ERR_PRM_DATA;
		}
	}

	/* CM_PD_SCID_BBUF */
	else if (scid == CM_PD_SCID_BBUF) /* see CM_OPC_SV_AR_PDEVPRM_READ */
	{
		if (pdprm->channel->usr.pd.reclist.is_valid) /* see cm_pd_reclist_prm_begin() */
		{
			response = CM_ERR_PRM_OTHER;
		}
		else
		{
			CM_AR_GRAPH_RECORD_PTR_TYPE rec = cm_pd_reclist_read(pdprm->channel, CM_PD_RECLIST_REGULAR, slot_nr, subslot_nr, record_index);

			if (is_null (rec)) /* not found */
			{
				record_data_length = 0;

				response = CM_OK;
			}
			else if( record_data_length < rec->record_data_len ) {

				response = CM_ERR_PRM_DATA;
			}
			else {

				record_data_length = rec->record_data_len;

				CM_MEMCPY(record_data, rec->record_data, record_data_length);

				response = CM_OK;
			}
		}
	}

	/* CM_PD_SCID_CMPD */
	else if (scid == CM_PD_SCID_CMPD)
	{
		if (record_index == 0xB081 /* CM: PDIOSystemInfo */)
		{
			response = cm_pd_prm_read_PDIOSystemInfo (pdprm->channel, port_id, & record_data_length, record_data);
		}
		else
		{
			response = CM_ERR_PRM_INDEX;
		}
	}

	else
	{
		response = CM_ERR_PRM_INDEX; /* unreachable */
	}

	if (response == CM_OK)
	{
		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PD-read: rqb(0x%x) OK: index(0x%x) cnf_data_length(%u)", read, record_index, record_data_length
			);

		CM_RQB_SET_RESPONSE (read, CM_OK); /* needed by iterator */
		read->args.pd.prm_rdwr->record_data_length = record_data_length; /* update */
		read->args.pd.prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;

		pdprm->read = LSA_NULL;
		pdprm->wait_for &= ~ CM_PD_PRM_WF_READ_CNF;
	}
	else if (response == CM_OK_PENDING)
	{
		/* must wait for cm_pd_prm_sc_read_done() */
	}
	else /* CM_ERR_PRM_DATA, CM_ERR_PRM_INDEX */
	{
		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PD-read: rqb(0x%x) error: rsp(%u) index(0x%x)", read, response, record_index
			);

		CM_RQB_SET_RESPONSE (read, response); /* needed by iterator */
		read->args.pd.prm_rdwr->record_data_length = 0; /* update */
		/* rsp mapping to PNIOStatus is done within cm_sv_pdev_read_done()
		//	with helper cm_pd_response_to_pnio_err() */

		pdprm->read = LSA_NULL;
		pdprm->wait_for &= ~ CM_PD_PRM_WF_READ_CNF;
	}
	return response;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	)
{
	CM_LOG_FCT("cm_pd_edd_prm_read_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	EDD_UPPER_PRM_READ_PTR_TYPE  param;
	LSA_UINT16  response;

	param = CM_EDD_GET_PPARAM(edd, EDD_UPPER_PRM_READ_PTR_TYPE);

	if (CM_EDD_GET_RESPONSE (edd) != EDD_STS_OK) {

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "lower-rsp(%u)", CM_EDD_GET_RESPONSE (edd)
			);

		response = cm_pd_sc_map_response (LSA_COMP_ID_EDD, CM_EDD_GET_RESPONSE (edd));
	}
	else {
		response = CM_OK;
	}

	cm_pd_prm_sc_read_done (pdprm, response, param->record_data_length);

	cm_edd_free_rqb (channel, edd);
}

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_MRP
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	)
{
	CM_LOG_FCT("cm_pd_mrp_prm_read_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16 response;

	switch (CM_MRP_GET_RESPONSE (mrp)) {
	case MRP_RSP_OK:
		response = CM_OK;
		break;

	case MRP_RSP_ERR_PRM_NOT_CONFIGURED:
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE /*AP00586614:no error trace if mrp is not configured*/
			, "lower-rsp(%u/not-configured)", CM_MRP_GET_RESPONSE (mrp)
			);
		response = CM_ERR_PRM_INDEX; /* see cm_sv43.c: if(req->is_read && (cls&CM_REC_IDX_READ)==0) pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); */
		break;

	default:
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "lower-rsp(%u)", CM_MRP_GET_RESPONSE (mrp)
			);
		response = cm_pd_sc_map_response (LSA_COMP_ID_MRP, CM_MRP_GET_RESPONSE (mrp));
		break;
	}

	cm_pd_prm_sc_read_done (pdprm, response, mrp->args.ctrl.prm_read.record_data_length);

	cm_mrp_free_rqb (channel, mrp);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	)
{
	CM_LOG_FCT("cm_pd_gsy_prm_read_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16 response;

	LSA_UNUSED_ARG (sync_id);

	if (CM_GSY_GET_RESPONSE (gsy) != GSY_RSP_OK) {

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "lower-rsp(%u)  sync_id(%u)", CM_GSY_GET_RESPONSE (gsy), sync_id
			);

		response = cm_pd_sc_map_response (LSA_COMP_ID_GSY, CM_GSY_GET_RESPONSE (gsy));
	}
	else {
		response = CM_OK;
	}

	cm_pd_prm_sc_read_done (pdprm, response, gsy->args.PrmRecord.RecordDataLen);

	cm_gsy_free_rqb (channel, gsy);
}
#endif

/*----------------------------------------------------------------------------*/
#if CM_CFG_USE_POF
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	)
{
	CM_LOG_FCT("cm_pd_pof_prm_read_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16 response;

	if (CM_POF_GET_RESPONSE (pof) != POF_RSP_OK) {

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "lower-rsp(%u)", CM_POF_GET_RESPONSE (pof)
			);

		response = cm_pd_sc_map_response (LSA_COMP_ID_POF, CM_POF_GET_RESPONSE (pof));
	}
	else {
		response = CM_OK;
	}

	cm_pd_prm_sc_read_done (pdprm, response, pof->args.PrmRead.record_data_length);

	cm_pof_free_rqb (channel, pof);
}
#endif

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	)
{
	CM_LOG_FCT("cm_pd_oha_prm_read_done")
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	LSA_UINT16 response;

	if (CM_OHA_GET_RESPONSE (oha) != OHA_OK) {

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "lower-rsp(%u)", CM_OHA_GET_RESPONSE (oha)
			);

		response = cm_pd_sc_map_response (LSA_COMP_ID_OHA, CM_OHA_GET_RESPONSE (oha));
	}
	else {

		response = CM_OK;
	}

	cm_pd_prm_sc_read_done (pdprm, response, oha->args.PrmData.record_data_length);

	cm_oha_free_rqb (channel, oha);
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_read_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				response,
	LSA_UINT32				record_data_length
	)
{
	CM_LOG_FCT("cm_pd_prm_sc_read_done")
	CM_UPPER_RQB_PTR_TYPE  read = pdprm->read;

	if (is_null (read) || (pdprm->wait_for & CM_PD_PRM_WF_READ_CNF) == 0)
	{
		CM_FATAL();
	}

	/***/

	if( response == CM_OK )
	{
		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "PD-read: rqb(0x%x) OK: index(0x%x) cnf_data_length(%u)"
			, read, read->args.pd.prm_rdwr->record_index, record_data_length
			);

		read->args.pd.prm_rdwr->record_data_length = record_data_length; /* update */
		read->args.pd.prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	}
	else /* CM_ERR_PRM_xxx */
	{
		CM_PD_TRACE_03 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "PD-read: rqb(0x%x) error: rsp(%u) index(0x%x)"
			, read, response, read->args.pd.prm_rdwr->record_index
			);

		read->args.pd.prm_rdwr->record_data_length = 0; /* update */
		/* rsp mapping to PNIOStatus is done within cm_sv_pdev_read_done()
		//	with helper cm_pd_response_to_pnio_err() */
	}

	pdprm->read = LSA_NULL;
	pdprm->wait_for &= ~ CM_PD_PRM_WF_READ_CNF;

	cm_pd_req_callback (pdprm->channel, response, read);
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
