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
/*  F i l e               &F: cm_arsm1.c                                :F&  */
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
/*  - cons apdu-status changed                                               */
/*  - link status changed                                                    */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  71
#define CM_MODULE_ID       71

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== CM-AR-COM CONSUMER APDU-STATUS CHANGED ================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_changed (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_UINT32 state_event
) {
	CM_LOG_FCT ("cm_ar_com_cons_apdu_status_changed")

	/* see CM_EVENT_APDU_STATUS_INITIAL_VALUE */

	if (state_event & ACP_STATE_STOP) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STOP", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STOP;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_RUN;
	}
	else if (state_event & ACP_STATE_RUN) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) RUN", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_RUN;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STOP;
	}

	/***/

	if (state_event & ACP_STATE_STATION_FAILURE) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STATION_FAILURE", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STATION_PROBLEM;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STATION_OK;
	}
	else if (state_event & ACP_STATE_STATION_OK) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STATION_OK", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STATION_OK;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STATION_PROBLEM;
	}

	/***/

	if (state_event & ACP_STATE_BACKUP) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) BACKUP", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_BACKUP;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_PRIMARY;
	}
	else if (state_event & ACP_STATE_PRIMARY) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) PRIMARY", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_PRIMARY;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_BACKUP;
	}

	/***/

	if (state_event & ACP_STATE_SYSRED_PRIMARY_AR_EXISTS) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) SYSRED-OK", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_SYSRED_OK;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_SYSRED_PROBLEM;
	}
	else if (state_event & ACP_STATE_SYSRED_NO_PRIMARY_AR_EXISTS) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) SYSRED-PROBLEM", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_SYSRED_PROBLEM;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_SYSRED_OK;
	}

	/***/

	if( iocr->cm.is_multicast ) {

#if CM_CFG_USE_NARE
		cm_arsm_mcons_apdu_status_changed(argr->ar_com.channel, iocr);
#else
		CM_FATAL(); /* a bug */
#endif
	}
	else {

		cm_ar_com_cons_apdu_status_check_unicast (argr);
	}
}

/*===========================================================================*/
/*=== CM-AR-COM::cons_apdu_status_count =====================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_count (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *cr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *operate,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *station,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *primary,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *sysred,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mcr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *moperate,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mstation,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mprimary,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *msysred
	)
{
	CM_LOG_FCT ("cm_ar_com_cons_apdu_status_count")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	*cr_cnt = *operate = *station = *primary = *sysred = 0;
	*mcr_cnt = *moperate = *mstation = *mprimary = *msysred = 0;

	for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {
		if (iocr->cm.cr_state == CM_AR_CR_ALLOCATED) {
			continue; /* ignore */
		}
		if (! iocr->cm.is_consumer) {
			continue; /* only consumer */
		}
		if (iocr->cm.is_multicast) {
			*mcr_cnt += 1;
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_RUN) {
				*moperate += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_STATION_OK) {
				*mstation += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_PRIMARY) {
				*mprimary += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_SYSRED_OK) {
				*msysred += 1;
			}
		}
		else {
			*cr_cnt += 1;
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_RUN) {
				*operate += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_STATION_OK) {
				*station += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_PRIMARY) {
				*primary += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_SYSRED_OK) {
				*sysred += 1;
			}
		}
	}
	CM_AR_TRACE_12(argr->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"dev/ar(%u/%u)  cr(%u) op(%u) st(%u) pr(%u) srd(%u)   mcr(%u) mop(%u) mst(%u) mpr(%u) msrd(%u)",
		argr->device_nr, argr->ar_nr,
		*cr_cnt,  *operate, *station, *primary, *sysred,
		*mcr_cnt, *moperate, *mstation, *mprimary, *msysred
		);
}

/*===========================================================================*/
/*=== CM-AR-COM::cons_apdu_status_check_unicast =============================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_check_unicast (
	CM_AR_GRAPH_PTR_TYPE  argr
)
{
	CM_LOG_FCT ("cm_ar_com_cons_apdu_status_check_unicast")
	LSA_UINT16  cr_cnt;
	LSA_UINT16  operate;
	LSA_UINT16  station;
	LSA_UINT16  primary;
	LSA_UINT16  sysred;
	LSA_UINT16  mcr_cnt;
	LSA_UINT16  moperate;
	LSA_UINT16  mstation;
	LSA_UINT16  mprimary;
	LSA_UINT16  msysred;

	/* AP00754080 (change of apdustatus not indicated to user)
	 *
	 * the formerly "return if not cm_ar_com_is_up()" here was the problem. the intention
	 * was to defer the checking until all-again before making the 'all-decision'.
	 *
	 * presumably, the bug krept in with the reset-to-unknown functionality.
	 * (is_up() is false between reset and all-again and the check on all-again was missing.)
	 *
	 * however, it is not necessary to wait for all-again because the reset-to-unknown does
	 * not affect the apdustatus bits (only the miss/again bits). since the apdustatus is
	 * indicated only after in-data.ind (= all-again) there is no problem on startup either.
	 */

	if ( cm_ar_com_is_down(argr) ) {
		CM_AR_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "already down"
			);
		return;
	}

	/***/

	cm_ar_com_cons_apdu_status_count (argr,
			& cr_cnt, & operate, & station, & primary, & sysred,
			& mcr_cnt, & moperate, & mstation, & mprimary, & msysred
		);

	if (cr_cnt > 0) {

		LSA_UINT16  new_apdu_status = 0; /* ind: see cm_event_apdu_status_enum */
		LSA_UINT16  apdu_delta;

		if (operate == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_RUN; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STOP; /* any */
		}

		if (station == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STATION_OK; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STATION_PROBLEM; /* any */
		}

		if (primary == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_PRIMARY; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_BACKUP; /* any */
		}

		if (sysred == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_SYSRED_OK; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_SYSRED_PROBLEM; /* any */
		}

		apdu_delta = CM_CAST_U16((~ argr->ar_com.user_apdu_status) & new_apdu_status);

		CM_AR_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) Datastatus-Check: old(0x%x) new(0x%x) diff(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->ar_com.user_apdu_status, new_apdu_status, apdu_delta
			);

		if (apdu_delta != 0) {

			switch (argr->ar_com.path_type) {
#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				if (cm_cl_ar_com_apdu_status_ind (argr, apdu_delta)) {
					argr->ar_com.user_apdu_status = new_apdu_status;
				}
				break;
#endif
#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				if (cm_sv_ar_data_status_indication (argr, new_apdu_status, argr->ar_com.user_apdu_status)) {
					argr->ar_com.user_apdu_status = new_apdu_status;
				}
				break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
			case CM_PATH_TYPE_MULTICAST:
				CM_AR_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "!path_type(%u)", argr->ar_com.path_type);
				CM_FATAL1 (argr->ar_com.path_type);
				return;
#endif
			default:
				CM_AR_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "!path_type(%u)", argr->ar_com.path_type);
				CM_FATAL1 (argr->ar_com.path_type);
				return;
			}
		}
	}
}

/*===========================================================================*/
/*=== CM-AR-COM LINK-STATUS CHANGED =========================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_link_status_changed(
	CM_AR_GRAPH_PTR_TYPE ar_graph,
	LSA_BOOL is_up,
	LSA_BOOL is_transient /* LSA_TRUE: the "down" is transient (some 100ms) */
) {
	CM_LOG_FCT("cm_ar_com_link_status_changed")

	CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u), link is_up(%u) is_transient(%u)"
		, ar_graph->device_nr, ar_graph->ar_nr, is_up, is_transient
		);

	if( is_up ) {

		CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr;
		LSA_BOOL is_mprov = LSA_FALSE;

		for( CmListEach(io_cr, &ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			if( io_cr->cm.is_multicast ) {

				if( io_cr->cm.is_consumer ) {

#if CM_CFG_USE_NARE
					cm_arsm_mcons_link_up (ar_graph->ar_com.channel, io_cr);
#else
					CM_FATAL(); /* should not happen */
#endif
				}
				else {

					is_mprov = LSA_TRUE;
				}
			}
		}

		if (is_mprov)
		{
			cm_arsm_mprov_link_up (ar_graph);
		}
	}
	else if( is_transient ) {

		/* don't call cm_ar_com_down() if the "down" is transient (MRP ring-reconfiguration) */
	}
	else {

		/* don't call cm_ar_com_down(CM_AR_REASON_LNK_DWN) if "down" (AP00831818) */
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
