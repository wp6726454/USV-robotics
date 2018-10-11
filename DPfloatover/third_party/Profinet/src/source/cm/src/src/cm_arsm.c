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
/*  F i l e               &F: cm_arsm.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  AR state-machines                                                        */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  7
#define CM_MODULE_ID       7

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_count_up_rir (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_cr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_up_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_cr3_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_rir_cnt
);

static CM_ACP_LOWER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_arcq_acp_rqb_alloc (
	CM_AR_GRAPH_PTR_TYPE  argr
);

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_acp_rqb_free (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);


/*===========================================================================*/
/*=== CM-AR-COM INIT ========================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_init (
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_init")

	argr->ar_com.down = CM_AR_REASON_NONE;
	argr->ar_com.cycle_counter = 0;
	argr->ar_com.down_pnio_status = CM_PNIO_ERR_NONE;
	CM_MEMSET (& argr->ar_com.down_debug_data, 0, sizeof(argr->ar_com.down_debug_data));

	argr->ar_com.cr_is_up = LSA_FALSE;
	argr->ar_com.cr_is_rir = LSA_FALSE;

	argr->ar_com.user_apdu_status = CM_EVENT_APDU_STATUS_INITIAL_VALUE; /* see ACP_STATE_CONSUMER_DEFAULT */

	/* note: argr->ar.ar_type, etc. is not yet valid */
}


/*===========================================================================*/
/*=== CM-AR-COM IOCR ADD ====================================================*/
/*===========================================================================*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_frames_add_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_frames_add_next")

	if (is_null(iocr)) /* init */
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* add.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else
		{
			CM_ASSERT (iocr->cm.cr_state == CM_AR_CR_PASSIVE);
			CM_ASSERT (iocr->cm.acp_id != ACP_INVALID_ACP_ID);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if (! iocr->cm.is_multicast)
		{
			if (iocr->cm.cr_state == CM_AR_CR_IDLE)
			{
				iocr->cm.cr_state = CM_AR_CR_PASSIVE;
				break; /* add this iocr */
			}
		}
	}

	/***/

	if( is_not_null(iocr) )
	{
		CM_ASSERT (iocr->cm.acp_id == ACP_INVALID_ACP_ID);
		cm_acp_frame_add (argr->ar_com.channel, iocr, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}

/*===========================================================================*/
/*=== CM-AR-COM ADD =========================================================*/
/*===========================================================================*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mprov_add_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_mprov_add_next")

	if (is_null(iocr)) /* init */
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* add.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else
		{
			CM_ASSERT (iocr->cm.acp_id != ACP_INVALID_ACP_ID);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if (iocr->cm.is_multicast  &&  ! iocr->cm.is_consumer)
		{
			if (cm_pdusr_send_clock_get(argr->ar_com.channel) == 0)
			{
				/* possible case for CM-SV. see comments in cm_sv_ar_sendclock_changed() */
				iocr->cm.cr_state = CM_AR_CR_ERROR;
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("check: das noch immer der fall ?")
#endif /* CM_MESSAGE */
				cm_ar_com_down (argr, CM_AR_REASON_PDEV);
			}
			else
			{
				iocr->cm.cr_state = CM_AR_CR_PASSIVE;
				break; /* add this m-provider */
			}
		}
	}

	/***/

	if( is_not_null(iocr) )
	{
		CM_ASSERT (iocr->cm.acp_id == ACP_INVALID_ACP_ID);
		cm_acp_frame_add (argr->ar_com.channel, iocr, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcons_add_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_mcons_add_next")

	if (is_null(iocr)) /* init */
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* add.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else
		{
			CM_ASSERT (iocr->cm.acp_id != ACP_INVALID_ACP_ID);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if (iocr->cm.is_multicast  &&  iocr->cm.is_consumer)
		{
			if (cm_pdusr_send_clock_get(argr->ar_com.channel) == 0)
			{
				/* possible case for CM-SV. see comments in cm_sv_ar_sendclock_changed() */
				iocr->cm.cr_state = CM_AR_CR_ERROR;
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("check: das noch immer der fall ?")
#endif /* CM_MESSAGE */
				cm_ar_com_down (argr, CM_AR_REASON_PDEV);
			}
			else
			{
				iocr->cm.cr_state = CM_AR_CR_PASSIVE;
				break; /* add this m-consumer */
			}
		}
	}

	/***/

	if (is_not_null(iocr))
	{
		CM_ASSERT (iocr->cm.acp_id == ACP_INVALID_ACP_ID);
		cm_acp_frame_add (argr->ar_com.channel, iocr, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_alarm_add_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_alarm_add_next")

	if( is_null(alarm_cr) )
	{
		alarm_cr = argr->alarm_cr;
	}
	else /* add.cnf(+/-) */
	{
		if (alarm_cr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down(alarm_cr->cm.backlink, CM_AR_REASON_ALARM);
		}
		else
		{
			CM_ASSERT (alarm_cr->cm.cr_state == CM_AR_CR_UP);
		}
		alarm_cr = LSA_NULL;
	}

	/***/

	if (is_not_null(alarm_cr))
	{
		CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);
		alarm_cr->cm.cr_state = CM_AR_CR_UP;
		cm_acp_alarm_add (argr->ar_com.channel, alarm_cr, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM ACTIVATE ====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_prov_activate_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_UINT16  acp_mode_initial
) {
	CM_LOG_FCT("cm_ar_com_prov_activate_next")

	if( is_null(iocr) )
	{
		cm_ar_graph_update_autostop_cons_id (argr);

		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* activate.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_FRAME, iocr->frame_id);
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else
		{
			CM_ASSERT (iocr->cm.cr_state == CM_AR_CR_UP);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {

		if( ! iocr->cm.is_consumer ) /* note: activate unicast- and multicast-provider */
		{
			if( iocr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				iocr->cm.cr_state = CM_AR_CR_UP;
				break; /* activate this provider */
			}
		}
	}

	/***/

	if( is_not_null(iocr) )
	{
		if( argr->ar_com.autostop_cons_id != ACP_INVALID_ACP_ID )
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_ACTIVE | ACP_PROVIDER_SET_AUTOSTOP | acp_mode_initial, acp);
		}
		else
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_ACTIVE | acp_mode_initial, acp);
		}
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_prov_activate_aux_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_UINT16  acp_mode_initial
) {
	CM_LOG_FCT("cm_ar_com_prov_activate_aux_next")

	if( is_null(iocr) )
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* activate.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_FRAME, iocr->frame_id);
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if( ! iocr->cm.is_consumer &&  ! iocr->cm.is_multicast)
		{
			if( iocr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				iocr->cm.cr_state = CM_AR_CR_UP;
				break; /* activate the aux of this provider */
			}
		}
	}

	/***/

	if( is_not_null(iocr) )
	{
		cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_ACTIVE_AUX | acp_mode_initial, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_activate_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
) {
	CM_LOG_FCT("cm_ar_com_cons_activate_next")

	if( is_null(io_cr) )
	{
		io_cr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* activate.cnf(+/-) */
	{
		if (io_cr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_FRAME, io_cr->frame_id);
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else
		{
			CM_ASSERT (io_cr->cm.cr_state == CM_AR_CR_WF_UP);
		}
		io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(io_cr)
		; io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if( io_cr->cm.is_consumer && ! io_cr->cm.is_multicast)
		{
			if( io_cr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				break; /* activate this consumer */
			}
		}
	}

	/***/

	if( is_not_null(io_cr) )
	{
		switch (argr->arcq.curr_opc)
		{
		case CM_ARCQ_OPC_CONS_ACTIVATE:
			io_cr->cm.cr_state = CM_AR_CR_WF_UP;
			io_cr->cm.is_rir = LSA_FALSE;
			cm_acp_frame_control(argr->ar_com.channel, io_cr, ACP_CONSUMER_SET_ACTIVE, acp);
			break;

		case CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK:
			io_cr->cm.cr_state = CM_AR_CR_WF_UP;
			io_cr->cm.is_rir = LSA_FALSE;
			cm_acp_frame_control(argr->ar_com.channel, io_cr, (ACP_CONSUMER_SET_ACTIVE | ACP_CONSUMER_SET_TO_UNKNOWN_STATE), acp);
			break;

		default:
			CM_FATAL();
			break;
		}
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM PROVIDER IOPARAM SET ========================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_prov_ioparamset_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_prov_ioparamset_next")

	if( is_null(iocr) )
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* control.cnf(+/-) */
	{
		if (iocr->cm.cr_state == CM_AR_CR_ERROR)
		{
			cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_FRAME, iocr->frame_id);
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {

		if( ! iocr->cm.is_consumer ) /* note: control unicast- and multicast-provider */
		{
			if( iocr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				break; /* control(ioparam-set) this provider */
			}
		}
	}

	/***/

	if( is_not_null(iocr) )
	{
		cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_IOPARAM, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM PROVIDER APDUSTATUS =========================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_prov_apdu_status_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_UINT16  acp_mode
) {
	CM_LOG_FCT("cm_ar_com_prov_apdu_status_next")

	if( is_null(io_cr) )
	{
		io_cr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* set-apdustatus.cnf(+) */
	{
		io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(io_cr)
		; io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if( ! io_cr->cm.is_consumer )
		{
			/* note: CM-SV and m-provider, m-provider carry the PI-state too. meaning is "there is a problem" (14.11.2007) */

			if( io_cr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				break; /* set apdu-status of this provider */
			}
		}
	}

	/***/

	if( is_not_null(io_cr) )
	{
		cm_acp_frame_control (argr->ar_com.channel, io_cr, /*SET-DATASTATUS*/acp_mode, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM CONSUMER RESET ==============================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_reset_next(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_cons_reset_next")

	if( is_null(io_cr) ) {

		argr->ar_com.do_reset = LSA_TRUE;

		io_cr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else { /* reset.cnf(+/-) */

		if( io_cr->cm.cr_state == CM_AR_CR_RESET )
		{
			io_cr->cm.cr_state = CM_AR_CR_WF_UP;
		}
		else if( io_cr->cm.cr_state == CM_AR_CR_ERROR ) /*AP01211797*/
		{
			cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_FRAME, io_cr->frame_id);
			cm_ar_com_down (argr, CM_AR_REASON_FRAME);
		}
		else if( io_cr->cm.cr_state != CM_AR_CR_DOWN )
		{
			CM_FATAL1 (io_cr->cm.cr_state);
		}

		io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(
		; is_not_null(io_cr)
		; io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {
		if( io_cr->cm.is_consumer && ! io_cr->cm.is_multicast )
		{
			if( io_cr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				if (io_cr->cm.cr_state == CM_AR_CR_WF_UP
				||	io_cr->cm.cr_state == CM_AR_CR_UP)
				{
					io_cr->cm.cr_state = CM_AR_CR_RESET;
					break; /* reset this consumer */
				}
			}
		}
	}

	/***/

	if( is_not_null(io_cr) )
	{
		switch (argr->arcq.curr_opc)
		{
		case CM_ARCQ_OPC_CONS_SET2UNK:
			cm_acp_frame_control(argr->ar_com.channel, io_cr, ACP_CONSUMER_SET_TO_UNKNOWN_STATE, acp);
			break;

		case CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS:
			cm_acp_frame_control(argr->ar_com.channel, io_cr, ACP_CONSUMER_SET_TO_UNKNOWN_STATE | ACP_CONSUMER_SET_AUTOSTOP_ENABLE, acp);
			break;

		default:
			CM_FATAL();
			break;
		}
	}
	else
	{
		argr->ar_com.cr_is_up = LSA_FALSE;

		argr->ar_com.do_reset = LSA_FALSE;

		cm_arcq_request_done (argr, acp); /* call back */

		(LSA_VOID) cm_ar_com_check (argr); /* then check if up */
	}
}


/*===========================================================================*/
/*=== CM-AR-COM PASSIVATE ===================================================*/
/*===========================================================================*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_frames_passivate_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_frames_passivate_next")

	if (is_null (iocr))
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* passivate.cnf(+) */
	{
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	/***/

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {

#ifdef CM_MESSAGE
# pragma CM_MESSAGE("warning: ARCQ, refactor cm_arsm3.c (mcons-statemachine) => auf ARCQ umstellen")
#endif /* CM_MESSAGE */
if (iocr->cm.is_multicast && iocr->cm.is_consumer) {
	continue;
}

		/* passivate unicast- and multicast-provider */
		if( iocr->cm.acp_id != ACP_INVALID_ACP_ID ) /* means add.cnf(+) */
		{
			if (iocr->cm.is_multicast)
			{
				argr->ar_com.mcr_busy_cnt -= 1;
			}

			if (iocr->cm.cr_state == CM_AR_CR_ERROR) /* means control (e.g. activate) went wrong */
			{
				/* not a special case, passive -> passive is ok for EDD */
			}

			iocr->cm.cr_state = CM_AR_CR_PASSIVE;
			break; /* passivate this iocr */
		}
	}

	/***/

	if (is_not_null (iocr))
	{
		if (iocr->cm.is_consumer)
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_CONSUMER_SET_PASSIVE, acp);
			/* note: without ACP_CONSUMER_SET_PROV_AUTOSTOP_DISABLE */
		}
		else
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_PASSIVE | ACP_PROVIDER_SET_AUTOSTOP/*rmv-autostop*/, acp);
		}
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_prov_passivate_aux_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_prov_passivate_aux_next")

	if (is_null (iocr))
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* passivate.cnf(+) */
	{
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	for(; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		if (! iocr->cm.is_consumer && ! iocr->cm.is_multicast)
		{
			if( iocr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				break; /* passivate the aux of this provider */
			}
		}
	}

	/***/

	if (is_not_null (iocr))
	{
		cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_PASSIVE_AUX, acp);
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM RESET-TO-FACTORY ============================================*/
/*===========================================================================*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_cm_frame_reset_to_factory_next (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_cm_frame_reset_to_factory_next")

	if (is_null (iocr))
	{
		iocr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}
	else /* control.cnf(+) */
	{
		iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	}

	/***/

	for(
		; is_not_null(iocr)
		; iocr = CmListNext(&argr->io_cr_list, &iocr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
	) {

		if( iocr->cm.acp_id != ACP_INVALID_ACP_ID ) /* means add.cnf(+) */
		{
			if (iocr->cm.cr_state != CM_AR_CR_PASSIVE) { /* only allowed with passive frames */
				CM_FATAL1(iocr->cm.cr_state);
			}
			break;
		}
	}

	/***/

	if (is_not_null (iocr))
	{
		if (iocr->cm.is_consumer)
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_CONSUMER_SET_PASSIVE | ACP_CONSUMER_RESET_TO_FACTORY, acp); /* EDDI needs this combination */
		}
		else
		{
			cm_acp_frame_control (argr->ar_com.channel, iocr, ACP_PROVIDER_SET_PASSIVE | ACP_PROVIDER_RESET_TO_FACTORY, acp);/* EDDI needs this combination */
		}
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM REMOVE ======================================================*/
/*===========================================================================*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_frames_remove_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_frames_remove_next")

	CM_AR_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u)  iocr-remove, state(%u)"
		, argr->device_nr, argr->ar_nr, argr->ar_com.state
		);

	if (is_not_null (acp))
	{
		/* mostly we don't need acp-rqb, give it back */
		cm_arcq_acp_rqb_free (argr, acp);
		acp = LSA_NULL;
	}

	switch (argr->ar_com.state)
	{
	case CM_AR_COM_IDLE: /* usecase CMCL and faulty Nare|Connect or after cm_ar_com_idle() */
	case CM_AR_COM_STOPPED_ALARM:

		if( argr->ar_com.mcr_busy_cnt != 0 )
		{
			argr->ar_com.state = CM_AR_COM_STOP_MCRS;
			cm_ar_com_mcr_stop (argr);
		}
		else
		{
			argr->ar_com.state = CM_AR_COM_STOP_FRAME;
			io_cr = LSA_NULL;
			goto stop_frame; /* "fall through" gives a misra-c warning */
		}
		break;

	case CM_AR_COM_STOP_MCRS:
		argr->ar_com.state = CM_AR_COM_STOP_FRAME;
		io_cr = LSA_NULL;
		goto stop_frame; /* "fall through" gives a misra-c warning */

	case CM_AR_COM_STOP_FRAME:
stop_frame:
		if( is_null(io_cr) )
		{
			CM_ASSERT(argr->ar_com.mcr_busy_cnt == 0);

			io_cr = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
		}
		else /* remove.cnf(+) */
		{
			CM_ASSERT (io_cr->cm.cr_state == CM_AR_CR_IDLE);
			io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE);
		}

		for(
			; is_not_null(io_cr)
			; io_cr = CmListNext(&argr->io_cr_list, &io_cr->link, CM_AR_GRAPH_IO_CR_PTR_TYPE)
		) {
			if( io_cr->cm.acp_id != ACP_INVALID_ACP_ID )
			{
				io_cr->cm.cr_state = CM_AR_CR_IDLE;
				break; /* remove this iocr */
			}
			else
			{
				io_cr->cm.cr_state = CM_AR_CR_IDLE;
			}
		}

		/***/

		acp = cm_arcq_acp_rqb_alloc (argr);

		if( is_not_null(io_cr) )
		{
			cm_acp_frame_remove (argr->ar_com.channel, io_cr, acp);
		}
		else
		{
			argr->ar_com.state = CM_AR_COM_IDLE;
			cm_arcq_request_done (argr, acp);
		}
		break;

	default:
		CM_AR_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) ar(%u) ar_com.state(%u)"
			, argr->device_nr, argr->ar_nr, argr->ar_com.state
			);
#if CM_CFG_MAX_SERVERS
		if( argr->ar_com.path_type == CM_PATH_TYPE_SERVER ) {
			CM_AR_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "cm.sv.cn_flags(0x%08x) cm.sv.ar_flags(0x%08x)"
				,  argr->cm.sv.cn_flags, argr->cm.sv.ar_flags
				);
		}
#endif
		CM_FATAL1(argr->ar_com.state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_alarm_remove_next(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	CM_LOG_FCT("cm_ar_com_alarm_remove_next")

	CM_ASSERT(argr->ar_com.state == CM_AR_COM_DOWN);

	if (is_null(alarm_cr))
	{
		alarm_cr = argr->alarm_cr;
	}
	else /* remove.cnf(+) */
	{
		CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);
		alarm_cr = LSA_NULL;
	}

	/***/

	if (is_not_null(alarm_cr))
	{
		if( alarm_cr->cm.cr_state == CM_AR_CR_ERROR )
		{
			alarm_cr->cm.cr_state = CM_AR_CR_IDLE;
			cm_arcq_request_done (argr, acp);
		}
		else
		{
			alarm_cr->cm.cr_state = CM_AR_CR_IDLE;
			cm_acp_alarm_remove (argr->ar_com.channel, alarm_cr, acp);
		}
	}
	else
	{
		cm_arcq_request_done (argr, acp);
	}

}


/*===========================================================================*/
/*=== START MCRs ============================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_start(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_mcr_start")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr;
	LSA_BOOL is_mprov = LSA_FALSE;

	CM_ASSERT(argr->ar_com.mcr_busy_cnt == 0);

	for( CmListEach(io_cr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		if( io_cr->cm.is_multicast ) {

			argr->ar_com.mcr_busy_cnt += 1;

			if( io_cr->cm.is_consumer ) {

#if CM_CFG_USE_NARE
				cm_arsm_mcons_start(argr->ar_com.channel, io_cr);
#else
				CM_FATAL(); /* should not happen */
#endif
			}
			else
			{
				is_mprov = LSA_TRUE;
			}
		}
	}

	if (is_mprov)
	{
		cm_arsm_mprov_start (argr);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM STOP MCRS ===================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_mcr_stop")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr;
	LSA_BOOL is_mprov = LSA_FALSE;

	CM_AR_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) stop mcr_busy_cnt(%u)"
		, argr->device_nr, argr->ar_nr, argr->ar_com.mcr_busy_cnt
		);

	/***/

	argr->ar_com.mcr_busy_cnt += 1; /* don't fire callback in cm_ar_com_mcr_stop_done() */

	for( CmListEach(io_cr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		if( io_cr->cm.is_multicast ) {

			if( io_cr->cm.is_consumer ) {

#if CM_CFG_USE_NARE
				cm_arsm_mcons_stop(argr->ar_com.channel, io_cr);
#else
				CM_FATAL(); /* should not happen */
#endif
			}
			else
			{
				is_mprov = LSA_TRUE;
			}
		}
	}

	if (is_mprov)
	{
		cm_arsm_mprov_stop (argr);
	}

	argr->ar_com.mcr_busy_cnt -= 1;

	cm_ar_com_mcr_stop_done(argr, LSA_NULL);
}

/*===========================================================================*/
/*=== CM-AR-COM STOP MCRS DONE ==============================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_stop_done(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr
) {
	CM_LOG_FCT("cm_ar_com_mcr_stop_done")

	CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) mcr_stop_done, frame_id(0x%x) mcr_busy_cnt(%u)"
		, argr->device_nr, argr->ar_nr
		, is_null(io_cr) ? 0x0 : io_cr->frame_id
		, argr->ar_com.mcr_busy_cnt
		);

	if( is_not_null(io_cr) )
	{
		CM_ASSERT(argr->ar_com.mcr_busy_cnt > 0);

		argr->ar_com.mcr_busy_cnt -= 1;
	}

	if( argr->ar_com.mcr_busy_cnt == 0 )
	{
		if( argr->ar_com.path_type == CM_PATH_TYPE_MULTICAST )
		{
#if CM_CFG_MAX_MULTICAST_CHANNELS
			cm_mc_ar_com_mcr_stop_done(argr->ar_com.channel, argr);
#else
			CM_FATAL();
#endif
		}
		else if( argr->ar_com.state == CM_AR_COM_STOP_MCRS )
		{
			cm_arcq_step_next (argr, LSA_NULL, LSA_NULL);
		}
		else
		{
			CM_FATAL(); /* a bug */
		}
	}
}

/*===========================================================================*/
/*=== CM-AR-COM TIMEOUT =====================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
) {
	CM_LOG_FCT("cm_ar_com_timeout")
	CM_AR_GRAPH_PTR_TYPE argr;

	LSA_UNUSED_ARG(user_ptr);

	argr = CM_CONTAINING_STRUCT(oneshot, CM_AR_GRAPH_PTR_TYPE, ar_com.timer);

	CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "dev(%u) ar(%u) com timeout", argr->device_nr, argr->ar_nr);

	cm_ar_com_down_debug_data_fmt (argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_TIMER, ((CM_CAST_U32(argr->device_nr) << 16) | argr->ar_nr));
	cm_ar_com_down(argr, CM_AR_REASON_TIMER);
}


/*===========================================================================*/
/*=== CM-AR-COM::ready ======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_ready (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_ar_com_ready")

	if (argr->ar_com.state == CM_AR_COM_IDLE)
	{
		CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) ar(%u) AR ready", argr->device_nr, argr->ar_nr);

		argr->ar_com.state = CM_AR_COM_READY;

		(LSA_VOID) cm_ar_com_check (argr); /* any down in the meantime ? */
	}
}

/*===========================================================================*/
/*=== CM-AR-COM::check ======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_count (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_mcr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_mup_cnt
	)
{
	CM_LOG_FCT("cm_ar_com_mcr_count")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	LSA_UINT16  mup_cnt, mcr_cnt;

	mup_cnt = mcr_cnt = 0;

	for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		if (iocr->cm.cr_state == CM_AR_CR_ALLOCATED)
		{
			continue; /* ignore */
		}
		if (iocr->cm.is_multicast)
		{
			mcr_cnt += 1;
			if (iocr->cm.cr_state == CM_AR_CR_UP/*MPROV*/
				|| iocr->cm.cr_state == CM_AR_MCONS_UP)
			{
				mup_cnt += 1;
			}
		}
	}
	CM_AR_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u):  mcr(%u) mup(%u)"
		, argr->device_nr, argr->ar_nr, mcr_cnt, mup_cnt
		);

	*p_mup_cnt = mup_cnt;
	*p_mcr_cnt = mcr_cnt;
}


/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_check(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_check")
	LSA_BOOL  does_down_callback = LSA_FALSE;

	CM_AR_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u), down(%u) ar.state(%u) cr.is_up(%u) cr.is_rir(%u)"
		, argr->device_nr, argr->ar_nr
		, argr->ar_com.down, argr->ar_com.state, argr->ar_com.cr_is_up, argr->ar_com.cr_is_rir
		);

	/* report only once */

	if( argr->ar_com.state == CM_AR_COM_READY ) {

		/* srt and asrt is set up */

		if( argr->ar_com.down ) {

			/* any down */

			argr->ar_com.state = CM_AR_COM_DOWN; /* no more indications */

			CM_AR_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) ar(%u) com DOWN"
				, argr->device_nr, argr->ar_nr
				);

			switch (argr->ar_com.path_type) {
#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				cm_cl_ar_com_down (argr);
				break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
			case CM_PATH_TYPE_MULTICAST:
				cm_mc_ar_com_down (argr->ar_com.channel, argr);
				break;
#endif
#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				cm_sv_ar_com_down (argr);
				break;
#endif
			default:
				CM_FATAL(); /* a bug */
				break;
			}

			does_down_callback = LSA_TRUE;
		}
		else if( argr->ar_com.do_reset ) {

			/* defer check-if-up because reset is still running */
		}
		else {
			LSA_BOOL  do_com_up, do_com_rir;
			LSA_UINT16  cr_cnt, up_cnt, cr3_cnt, rir_cnt;

			do_com_up = do_com_rir = LSA_FALSE;
			cm_ar_com_count_up_rir (argr, &cr_cnt, &up_cnt, &cr3_cnt, &rir_cnt);

			CM_AR_TRACE_08 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u): check cr.is_up(%u) & cr.is_rir(%u)  with  cr(%u)==up(%u) & cr3(%u)==rir(%u)"
				, argr->device_nr, argr->ar_nr
				, argr->ar_com.cr_is_up, argr->ar_com.cr_is_rir
				, cr_cnt, up_cnt, cr3_cnt, rir_cnt
				);

			if (! argr->ar_com.cr_is_up)
			{
				if (up_cnt == cr_cnt)
				{
					do_com_up = LSA_TRUE;
					argr->ar_com.cr_is_up = LSA_TRUE;
				}
			}
			if (argr->ar_com.cr_is_up && ! argr->ar_com.cr_is_rir)
			{
				if (cr3_cnt > 0 && cr3_cnt == rir_cnt)
				{
					do_com_rir = LSA_TRUE;
					argr->ar_com.cr_is_rir = LSA_TRUE;
				}
			}

			/***/

			if (do_com_rir)
			{
				CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u) ar(%u)  com runs with RiR"
					, argr->device_nr, argr->ar_nr
					);

				switch (argr->ar_com.path_type) {
#if CM_CFG_MAX_CLIENTS
				case CM_PATH_TYPE_CLIENT:
					cm_cl_ar_com_rtc3_is_running (argr);
					break;
#endif
#if CM_CFG_MAX_SERVERS
				case CM_PATH_TYPE_SERVER:
					cm_sv_ar_received_in_red_indication(argr);
					break;
#endif
				default:
					CM_FATAL(); /* a bug */
					break;
				}
			}

			if (do_com_up)
			{
				CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u) ar(%u)  com is UP", argr->device_nr, argr->ar_nr
					);

				switch (argr->ar_com.path_type) {
#if CM_CFG_MAX_CLIENTS
				case CM_PATH_TYPE_CLIENT:
					cm_cl_ar_com_up (argr);
					break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
				case CM_PATH_TYPE_MULTICAST:
					/* nothing to do (no unicast frames) */
					break;
#endif
#if CM_CFG_MAX_SERVERS
				case CM_PATH_TYPE_SERVER:
					cm_sv_ar_new_data_indication(argr);
					break;
#endif
				default:
					CM_FATAL(); /* a bug */
					break;
				}
			}

			/*
			 * note: on AR_IN_DATA_RSP the client and server
			 *       will call cm_ar_com_cons_apdu_status_check_unicast()
			 */
		}
	}
	return does_down_callback;
}

/*===========================================================================*/
/*=== CM-AR-COM::is_up ======================================================*/
/*===========================================================================*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_count_up_rir (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_cr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_up_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_cr3_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_rir_cnt
	)
{
	CM_LOG_FCT("cm_ar_com_count_up_rir")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	LSA_UINT16  cr_cnt, cr3_cnt;
	LSA_UINT16  up_cnt, rir_cnt;

	cr_cnt = up_cnt = 0;
	cr3_cnt = rir_cnt = 0;

	for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		if (iocr->cm.cr_state == CM_AR_CR_ALLOCATED) {
			continue; /* ignore */
		}
		if (iocr->cm.is_multicast) {
			continue;
		}
		if (! iocr->cm.is_consumer) {
			continue;
		}

		cr_cnt += 1;
		if (iocr->cm.cr_state == CM_AR_CR_UP)
		{
			up_cnt += 1;
		}

		if((iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3)
		{
			cr3_cnt += 1;
			if (iocr->cm.is_rir)
			{
				rir_cnt += 1;
			}
		}
	}

	*p_cr_cnt = cr_cnt;
	*p_up_cnt = up_cnt;
	*p_cr3_cnt = cr3_cnt;
	*p_rir_cnt = rir_cnt;
}

/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_up(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_is_up")

	if( argr->ar_com.state == CM_AR_COM_READY )
	{
		if (argr->ar_com.cr_is_up)
		{
			CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u) is up", argr->device_nr, argr->ar_nr
				);
			return LSA_TRUE;
		}
	}
	CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) is not up", argr->device_nr, argr->ar_nr
		);
	return LSA_FALSE;
}


/*===========================================================================*/
/*=== CM-AR-COM::is received in red =========================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_rir(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_is_rir")

	if( argr->ar_com.state == CM_AR_COM_READY )
	{
		if (argr->ar_com.cr_is_up  &&  argr->ar_com.cr_is_rir)
		{
			CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u) RiR is up", argr->device_nr, argr->ar_nr
				);
			return LSA_TRUE;
		}
	}

	CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) RiR is not up", argr->device_nr, argr->ar_nr
		);
	return LSA_FALSE;
}


/*===========================================================================*/
/*=== CM-AR-COM::is_down ====================================================*/
/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_down (
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_is_down")

	if( argr->ar_com.down ) {
		/* don't test state for CM_AR_COM_READY */
		CM_AR_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) yes, com is down with reason(%u)"
			, argr->device_nr, argr->ar_nr
			, argr->ar_com.down
			);
		return LSA_TRUE;
	}
	return LSA_FALSE;
}


/*===========================================================================*/
/*=== CM-AR-COM::down =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down_debug_data_fmt (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  module_id,
	LSA_UINT32  line_nr,
	LSA_UINT8  reason,
	LSA_UINT32  data1
) {
	ACP_ERROR_INFO_DATA_TYPE  debug_data;

	debug_data[0] = 1; /* CM */
	debug_data[1] = reason;
	debug_data[2] = 0;
	debug_data[3] = 0;

	CM_PUT16_HTON (debug_data,  4, module_id);
	CM_PUT16_HTON (debug_data,  6, (LSA_UINT16) line_nr);
	CM_PUT32_HTON (debug_data,  8, data1);
	CM_PUT32_HTON (debug_data, 12, 0);

	cm_ar_com_down_debug_data_set (argr, & debug_data);
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down_debug_data_set (
	CM_AR_GRAPH_PTR_TYPE  argr,
	ACP_UPPER_ERROR_INFO_DATA_PTR_TYPE  debug_data
) {
	if (is_not_null (debug_data)  &&  argr->ar_com.down_debug_data[0] == 0/*no DebugIdent*/)
	{
		CM_MEMCPY (& argr->ar_com.down_debug_data, debug_data, sizeof(argr->ar_com.down_debug_data));
	}
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down2(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8 reason,
	LSA_UINT16 cycle_counter
) {
	CM_LOG_FCT("cm_ar_com_down2")

		/* keeps only first reason */
	if (argr->ar_com.down == CM_AR_REASON_NONE) {

		CM_ASSERT (reason != CM_AR_REASON_NONE);

		argr->ar_com.down = reason;
		argr->ar_com.cycle_counter = cycle_counter;

		CM_AR_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev(%u) ar(%u) !AR goes down! state(%u) reason(%u) cycle(%u) ---"
			, argr->device_nr, argr->ar_nr
			, argr->ar_com.state, argr->ar_com.down, argr->ar_com.cycle_counter
			);
	}
	else {
		CM_AR_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ignored reason(%u) .state(%u) .down(%u)"
			, reason, argr->ar_com.state, argr->ar_com.down
			);
	}

	(LSA_VOID) cm_ar_com_check (argr);
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8 reason
) {
	cm_ar_com_down2(argr, reason, 0);
}


/*===========================================================================*/
/*=== CM-AR-COM::idle =======================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_idle (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_ar_com_idle")

	if (argr->ar_com.state == CM_AR_COM_IDLE /* faulty Nare|Connect */
	||	argr->ar_com.state == CM_AR_COM_STOPPED_ALARM	/* default down case */)
	{
		CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "dev(%u) ar(%u) AR idle", argr->device_nr, argr->ar_nr);

		argr->ar_com.state = CM_AR_COM_IDLE;

		cm_ar_com_init (argr); /* re-init */
	}
	else {
		CM_ASSERT(0);
	}
}


/*===========================================================================*/
/*=== CM-AR-COM::timer_init =================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_init(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	cm_oneshot_init(&argr->ar_com.timer, cm_ar_com_timeout, argr->ar_com.channel);
}

/*===========================================================================*/
/*=== CM-AR-COM::timer_start ================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_trigger(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_timer_trigger")

	cm_oneshot_trigger(&argr->ar_com.timer, argr->ar.cmi_timeout100ms * 100);

	CM_AR_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "started, timeout(%u msec)"
		, argr->ar.cmi_timeout100ms * 100
		);
}

/*===========================================================================*/
/*=== CM-AR-COM::timer_stop =================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_ar_com_timer_stop")

	cm_oneshot_stop(&argr->ar_com.timer);

	CM_AR_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "stopped"
		);
}


/*------------------------------------------------------------------------------
// ARCQ ..  AR_COM-Command Queue
//----------------------------------------------------------------------------*/

/*
** cm_arcqlist_xxx  ..  helper for fixed size list
**
** see CM_ARCQ_LIST_PTR_TYPE for the list and CM_ARCQ_ENTRY_PTR_TYPE for one entry
*/

#define cm_arcqlist_next_index(idx)		((idx == CM_ARCQ_LIST_SIZE-1) ? 0 : idx+1)

#define cm_arcqlist_entry(acq,idx)		(& ((acq)->entry[idx]))

#define cm_arcqlist_current(acq)		cm_arcqlist_entry(acq,(acq)->curr)

#define cm_arcqlist_is_empty(acq)		(cm_arcqlist_current(acq)->opcode == CM_ARCQ_OPC_IDLE)

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcqlist_init (CM_ARCQ_LIST_PTR_TYPE acq)
{
	for (acq->curr = 0; acq->curr < CM_ARCQ_LIST_SIZE; ++acq->curr) {
		CM_ARCQ_ENTRY_PTR_TYPE acqe = cm_arcqlist_entry (acq, acq->curr);
		acqe->opcode = CM_ARCQ_OPC_IDLE;
	}
	acq->curr = 0;
	acq->next = 0;
}

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcqlist_insert (CM_ARCQ_LIST_PTR_TYPE acq, LSA_UINT8 opcode, LSA_UINT16 para1)
{
	CM_ARCQ_ENTRY_PTR_TYPE  acqe = cm_arcqlist_entry (acq, acq->next);

	if (acqe->opcode != CM_ARCQ_OPC_IDLE) {
		CM_FATAL(); /* wrong CM_ARCQ_LIST_SIZE */
	}

	CM_ASSERT(opcode != CM_ARCQ_OPC_IDLE);
	acqe->opcode = opcode;
	acqe->para1 = para1;
	acqe->is_startup = LSA_TRUE;

	acq->next = cm_arcqlist_next_index (acq->next);
}

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcqlist_remove (CM_ARCQ_LIST_PTR_TYPE acq)
{
	CM_ARCQ_ENTRY_PTR_TYPE  acqe = cm_arcqlist_entry (acq, acq->curr);

	acqe->opcode = CM_ARCQ_OPC_IDLE;

	acq->curr = cm_arcqlist_next_index (acq->curr);
}


/*------------------------------------------------------------------------------
//  ARCQ
//
//  cm_arcq_request()					adds a arcq-opcode to the queue and calls
//    cm_arcq_start()					which trys to start the next opcode (if no other is running)
//      cm_arcq_step_next()				iterates the current opcode, does one call to
//        cm_arcq_$OPCODE_next()		to start a single iteration. This function calls typically
//          lower_req()
//
//  lower_cnf()							has to call
//    cm_arcq_step_next()				for the next iteration. cm_arcq_step_next() calls
//      cm_arcq_$OPCODE_next()			again for (a) confirm of the previous step and (b) start the next iteration
//         lower_req()					if there is a next step
//         cm_arcq_request_done()		if all steps are done
//
//  cm_arcq_request_done()				calls the callback
//    cm_$CHANNEL_request_done			which signals the finished opcode to the $CHANNEL and then calls
//    cm_arcq_start()					which trys to start next opcode (if any)
//
//
//  cm_$CHANNEL_request_done()			may call
//    cm_arcq_chain_next_request()		to chain a sequence of opcodes
//
//
//	NOTE:
//		cm_$CHANNEL_request_done() is always called asynchronously, see "is_startup"
//
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arcq_init (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_arcq_init")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp_rqb; /* pre-allocated ACP-RQB */

	cm_acp_alloc_rqb (argr->ar_com.channel, & acp_rqb);
	if (is_null (acp_rqb))
	{
		return LSA_FALSE;
	}

	cm_arcqlist_init (& argr->arcq.list);

	argr->arcq.entered	= 0;
	argr->arcq.curr_opc	= CM_ARCQ_OPC_IDLE;
	argr->arcq.acp_rqb	= acp_rqb;

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_undo_init (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{

	CM_ASSERT (cm_arcqlist_is_empty(&argr->arcq.list));
	CM_ASSERT (argr->arcq.curr_opc == CM_ARCQ_OPC_IDLE	/* cm-cl */
		|| argr->arcq.curr_opc == CM_ARCQ_OPC_CALLBACK	/* cm-sv */
		);
	CM_ASSERT (is_not_null (argr->arcq.acp_rqb));
	CM_ASSERT(argr->arcq.entered == 0);

	cm_acp_free_rqb (argr->ar_com.channel, & argr->arcq.acp_rqb);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_request (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode, /* see cm_arcq_opcode_enum */
	LSA_UINT16  para1
	)
{
	CM_LOG_FCT("cm_arcq_request")

	CM_AR_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) arcq queue opcode(%u)"
		, argr->device_nr, argr->ar_nr, arcq_opcode
		);

	CM_ASSERT (arcq_opcode != CM_ARCQ_OPC_IDLE);

	cm_arcqlist_insert (&argr->arcq.list, arcq_opcode, para1);

	/* start next if possible */

	if (argr->arcq.curr_opc == CM_ARCQ_OPC_IDLE)
	{
		cm_arcq_step_next (argr, LSA_NULL, LSA_NULL);
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_chain_next_request ( /* only allowed to call from within cm_$CHANNEL_request_done - callback */
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  para1
	)
{
	CM_LOG_FCT("cm_arcq_chain_next_request")

	CM_AR_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) arcq chain_next_request opcode(%u)"
		, argr->device_nr, argr->ar_nr, arcq_opcode
		);
	CM_ASSERT (argr->arcq.curr_opc == CM_ARCQ_OPC_CALLBACK);
	CM_ASSERT (arcq_opcode != CM_ARCQ_OPC_IDLE);


	cm_arcqlist_insert (&argr->arcq.list, arcq_opcode, para1);
}

/*----------------------------------------------------------------------------*/

static CM_ACP_LOWER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_arcq_acp_rqb_alloc (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_arcq_acp_rqb_alloc")
	CM_ACP_LOWER_RQB_PTR_TYPE  acp;

	if (is_not_null (argr->arcq.acp_rqb))
	{
		acp = argr->arcq.acp_rqb;
		argr->arcq.acp_rqb = LSA_NULL;
	}
	else
	{
		acp = LSA_NULL; /* compiler warning */
		CM_FATAL(); /* what's wrong with queueing ? */
	}
	return acp;
}

/*----------------------------------------------------------------------------*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_acp_rqb_free (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
	)
{
	if (is_null (argr->arcq.acp_rqb))
	{
		argr->arcq.acp_rqb = acp;
	}
	else
	{
		CM_ASSERT (is_null(argr->arcq.acp_rqb)); /* what's wrong with queueing ? */
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_free_ar_graph (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT("cm_arcq_free_ar_graph")

	if (argr->arcq.entered > 0)
	{
		argr->arcq.curr_opc = CM_ARCQ_OPC_FREE_AR_GRAPH;
	}
	else
	{
		cm_ar_graph_free (argr);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_step_next (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_VOID_PTR_TYPE  cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
	)
{
	CM_LOG_FCT("cm_arcq_step_next")
	CM_ARCQ_ENTRY_PTR_TYPE  arcq = cm_arcqlist_current (&argr->arcq.list);

	argr->arcq.entered += 1;

	if (argr->arcq.entered == 2)
	{
#ifdef CM_MESSAGE
		/* TODO: check: should keep and restore later ? see sync caller. */
#endif /* CM_MESSAGE */
		CM_ASSERT (is_null(cr));
		CM_ASSERT (is_null(acp));
		return; /* do one loop later */
	}
	else if (argr->arcq.entered > 2)
	{
		CM_FATAL(); /* cann't keep: should never happen */
		return;
	}

	if (is_not_null (cr)) /* null == start signal */
	{
		if (is_not_null (arcq))
		{
			arcq->is_startup = LSA_FALSE;
		}
	}

	/***/

	do
	{
		if (argr->arcq.curr_opc == CM_ARCQ_OPC_IDLE)
		{
			if (is_null (arcq) || arcq->opcode == CM_ARCQ_OPC_IDLE)
			{
				CM_AR_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "dev(%u) ar(%u) arcq is empty, nothing to do"
					, argr->device_nr, argr->ar_nr
					);
			}
			else
			{
				CM_AR_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "dev(%u) ar(%u) arcq starts opcode(%u) para1(0x%x) acp(%08x) down(%u)"
					, argr->device_nr, argr->ar_nr
					, arcq->opcode, arcq->para1
					, acp, argr->ar_com.down
					);

				argr->arcq.curr_opc = arcq->opcode;
				cr = LSA_NULL;

				/* pre-processing */
#if 0/*current none*/
				{
					switch (argr->arcq.curr_opc)
					{
					default:
						break;
					}
				}
#endif
			}
		}
		else
		{
			CM_ASSERT (is_not_null (arcq));

			CM_AR_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) ar(%u) arcq stepping opcode(%u) para1(0x%x) last-cr(%08x) down(%u)"
				, argr->device_nr, argr->ar_nr
				, arcq->opcode, arcq->para1
				, cr, argr->ar_com.down
				);
		}

		if (is_null (acp)) {
			acp = cm_arcq_acp_rqb_alloc (argr);
		}

		switch (argr->arcq.curr_opc)
		{
		case CM_ARCQ_OPC_IDLE:
			/* no more arcq, free acp */
			cm_arcq_acp_rqb_free (argr, acp);
			break;

		case CM_ARCQ_OPC_FRAMES_ADD:
			cm_ar_com_frames_add_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_FRAMES_PASSIVATE:
			cm_ar_com_frames_passivate_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_FRAMES_RTF:
			cm_ar_cm_frame_reset_to_factory_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_FRAMES_REMOVE:
			cm_ar_com_frames_remove_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_MPROV_ADD:
			cm_ar_com_mprov_add_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_PROV_ACTIVATE:
		case CM_ARCQ_OPC_MPROV_ACTIVATE:
			cm_ar_com_prov_activate_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp, arcq->para1);
			break;

		case CM_ARCQ_OPC_PROV_IOPARAMS_SET:
			cm_ar_com_prov_ioparamset_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_PROV_ACTIVATE_AUX:
			cm_ar_com_prov_activate_aux_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp, arcq->para1);
			break;

		case CM_ARCQ_OPC_PROV_DATASTATUS_SET:
			cm_ar_com_prov_apdu_status_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp, arcq->para1);
			break;

		case CM_ARCQ_OPC_PROV_PASSIVATE_AUX:
			cm_ar_com_prov_passivate_aux_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		/***/

		case CM_ARCQ_OPC_MCONS_ADD:
			cm_ar_com_mcons_add_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_CONS_ACTIVATE:
		case CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK:
			cm_ar_com_cons_activate_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_CONS_SET2UNK:
		case CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS:
			cm_ar_com_cons_reset_next (argr, (CM_AR_GRAPH_IO_CR_PTR_TYPE)cr, acp);
			break;

		/***/

		case CM_ARCQ_OPC_ALARM_ADD:
			cm_ar_com_alarm_add_next (argr, (CM_AR_GRAPH_ALARM_CR_PTR_TYPE)cr, acp);
			break;

		case CM_ARCQ_OPC_ALARM_REMOVE:
			cm_ar_com_alarm_remove_next (argr, (CM_AR_GRAPH_ALARM_CR_PTR_TYPE)cr, acp);
			break;

		/***/

		default:
			CM_FATAL1 (arcq->opcode); /* not supported opcode */
			return;
		}

		argr->arcq.entered -= 1;

		if (argr->arcq.curr_opc == CM_ARCQ_OPC_FREE_AR_GRAPH)
		{
			argr->arcq.curr_opc = CM_ARCQ_OPC_IDLE;
			cm_ar_graph_free (argr);
			return;
		}

		if (argr->arcq.entered)
		{
			arcq = cm_arcqlist_current (&argr->arcq.list);
			cr = LSA_NULL;
			acp = LSA_NULL;
		}
	} while (argr->arcq.entered);
}

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
	)
{
	CM_LOG_FCT("cm_arcq_request_done")
	LSA_UINT8  arcq_opcode;
	LSA_UINT16  arcq_para1;

	{
	CM_ARCQ_ENTRY_PTR_TYPE  arcq = cm_arcqlist_current (&argr->arcq.list);

	if (arcq->is_startup)
	{
		arcq->is_startup = LSA_FALSE;

		/* Is called directly from cm_arcq_request ().
		** Must therefore be terminated with an asynchronous call, using acp_nop (). */
		cm_acp_nop (argr->ar_com.channel, (LSA_VOID_PTR_TYPE)(LSA_UINT32)cm_arcq_request_done, (LSA_UINT32)argr, (LSA_UINT32)acp);
		return;
	}

	arcq_opcode = arcq->opcode;
	arcq_para1  = arcq->para1;

	cm_arcqlist_remove (&argr->arcq.list);
	}

	if (arcq_opcode != argr->arcq.curr_opc) {
		CM_FATAL(); /* a bug */
	}

	argr->arcq.curr_opc = CM_ARCQ_OPC_IDLE;

	CM_AR_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) ar(%u) arcq has done opcode(%u) para1(0x%x) acp(%08x) down(%u)"
		, argr->device_nr, argr->ar_nr
		, arcq_opcode, arcq_para1
		, acp, argr->ar_com.down
		);

	if (is_not_null (acp))
	{
		cm_arcq_acp_rqb_free (argr, acp);
	}
	else
	{
		/* see cm_ar_com_mcr_stop_done() */
	}

	/* pre-processing */
	{
		switch (arcq_opcode)
		{
		/* internal StateMachine here: can we do it better ? */
		case CM_ARCQ_OPC_ALARM_REMOVE:
			CM_ASSERT(argr->ar_com.state == CM_AR_COM_DOWN);
			argr->ar_com.state = CM_AR_COM_STOPPED_ALARM;
			break;

		default:
			break;
		}
	}

	/* callback */
	{
		argr->arcq.curr_opc = CM_ARCQ_OPC_CALLBACK;

		switch (argr->ar_com.path_type)
		{
#if CM_CFG_MAX_CLIENTS
		case CM_PATH_TYPE_CLIENT:
			cm_cl_arcq_request_done (argr, arcq_opcode, arcq_para1);
			break;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		case CM_PATH_TYPE_MULTICAST:
			cm_mc_arcq_request_done (argr, arcq_opcode, arcq_para1);
			break;
#endif

#if CM_CFG_MAX_SERVERS
		case CM_PATH_TYPE_SERVER:
			argr = cm_sv_arcq_request_done (argr, arcq_opcode, arcq_para1); /* returns LSA_NULL if ar-graph is freed */
			break;
#endif

		default:
			CM_FATAL(); /* a bug */
			break;
		}

		if (is_not_null (argr))
		{
			argr->arcq.curr_opc = CM_ARCQ_OPC_IDLE;
		}
	}

	/* post-processing */
	{
		/* current none */
	}

	/* start next if possible */
	if (is_not_null (argr))
	{
		/* if (argr->arcq.curr_opc == CM_ARCQ_OPC_IDLE) ... always true */
			cm_arcq_step_next (argr, LSA_NULL, LSA_NULL);
	}
	else
	{
		/* argr is gone. see case CM_ARCQ_OPC_COM_STOP_FRAMES above */
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
