#ifndef CM_ICLAR_H
#define CM_ICLAR_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: cm_iclar.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - client ar-graph things                            */
/*                                                                           */
/*****************************************************************************/

typedef struct cm_cl_ar_schedule_info_tag
{
	CM_LIST_ENTRY_TYPE	link;			/* used for schedule ready_to_run_list */
	LSA_UINT16			in_schedule;	/* AR is in schedule-phase (CM_DST_WF_SCHEDULE...CM_DST_PRM) */
	LSA_UINT16			wait_ticks;		/* countdown for START: */
	LSA_UINT16			reschedule_ticks;
										/* calculated ticks for next cm_cl_schedule_add() */
										/* depends on (NSU|ASU) and on (CM_DST_WF_DOWN|CM_DST_WF_DOWN) */
} CM_CL_AR_SCHEDULE_INFO_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct cm_cl_argr_alarm_tag
{
	LSA_UINT16			send_sequence_nr;	/* next alarm is send with this seq_nr; see CM_ALARM_SEQUENCE_NR_INCREMENT */
	LSA_UINT16			recv_sequence_nr;	/* next received alarm must own this seq_nr; see CM_ALARM_SEQUENCE_UNDEF; see CM_AR_TYPE_SINGLE_SYSRED */
	CM_LIST_ENTRY_TYPE	alarm_list;			/* upper queue: list for alarms to send */
	CM_UPPER_RQB_PTR_TYPE
						ack;				/* current AlarmAck */
	CM_UPPER_RQB_PTR_TYPE
						early_alarm;		/* lower queue: keep upper alarm, recv in state WF_IN_DATA */
} CM_CL_AR_ALARM_TYPE;


/*----------------------------------------------------------------------------*/

typedef struct cm_cl_argr_prm_tag
{
	/* prm-state */
	LSA_UINT8				state;			/* record and prm machine; see CM_CL_PRM_STATE_xxx */
	LSA_UINT8				prm_seq;		/* running prm-sequence, see CM_CL_PRMMODE_xxx */
	LSA_UINT8				prm_seq_next;	/* next prm-sequence, see CM_CL_PRMMODE_xxx */
	LSA_UINT8				prm_seq_next2;	/* next next prm-sequence, see CM_CL_PRMMODE_xxx */

	/* using wf_flags from ar-graph */

	CM_LIST_ENTRY_TYPE		prm_list;		/* list for current prm-records */
	CM_LIST_ENTRY_TYPE		prm_submod_list;/* list with prm-able submoduls */

	CM_UPPER_RQB_PTR_TYPE	prm_keep_plug;	/* keep current/next PlugAlarm-RQB */
	CM_UPPER_RQB_PTR_TYPE	prm_keep_upd;	/* keep current/next PrmUpdate-RQB */

		/* VV: change upper 2 pointer to:
	CM_UPPER_RQB_PTR_TYPE	prm_keep_curr;	// keep current PlugAlarm/PrmUpdate-RQB (note: PrmUpd is 0 at end of prm_seq) //
	CM_UPPER_RQB_PTR_TYPE	prm_keep_next;	// keep next PlugAlarm/PrmUpdate-RQB */

	struct cm_cl_argr_prm_submod_tag {
	LSA_UINT32				api;			/* api */
	LSA_UINT16				slot_nr;		/* slot number */
	LSA_UINT16				subslot_nr;		/* subslot number */
	}						submod;

	/* prm-read: see part-6 CMInitiatorTriggerTimeoutFactor */
	LSA_UINT8				read_state;
	LSA_UINT32				read_timeout;
	CM_ONESHOT_TYPE			read_oneshot;

	/* prm-ardy: see part-6: RemoteApplicationReadyTimeout */
	LSA_UINT16				ardy_timer;		/* 0 or CM_CL_ARDY_TIMEOUT__xxx */
	LSA_UINT32				ardy_timeout_msec;	
	CM_ONESHOT_TYPE			ardy_oneshot;

} CM_CL_AR_PRM_TYPE;

typedef CM_CL_AR_PRM_TYPE CM_LOCAL_MEM_ATTR * CM_CL_AR_PRM_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct cm_cl_down_info_tag
{
	LSA_UINT16		module_id;
	LSA_UINT16		line_nr;
	LSA_UINT8		reason_code;
	LSA_UINT16		error_cause;
	LSA_UINT32		pnio_status;
	LSA_UINT32		data2;
	CM_MAC_TYPE		mac_addr;
} CM_CL_DOWN_INFO_TYPE;

typedef CM_CL_DOWN_INFO_TYPE CM_LOCAL_MEM_ATTR * CM_CL_DOWN_INFO_PTR_TYPE;

#define CM_CL_DINFO_SET(dinfo_, reason_code_, error_cause_, pnio_status_) { \
	(dinfo_)->module_id = CM_MODULE_ID; (dinfo_)->line_nr = (LSA_UINT16) __LINE__; \
	(dinfo_)->reason_code = reason_code_; (dinfo_)->error_cause = error_cause_; (dinfo_)->pnio_status = pnio_status_; \
	(dinfo_)->data2 = 0; \
	}

#define CM_CL_DINFO_SET2(dinfo_, reason_code_, error_cause_, pnio_status_, data2_) { \
	(dinfo_)->module_id = CM_MODULE_ID; (dinfo_)->line_nr = (LSA_UINT16) __LINE__; \
	(dinfo_)->reason_code = reason_code_; (dinfo_)->error_cause = error_cause_; (dinfo_)->pnio_status = pnio_status_; \
	(dinfo_)->data2 = data2_; \
	}

#define CM_CL_DINFO_SETm(dinfo_, reason_code_, error_cause_, pnio_status_, mac_addr_) { \
	(dinfo_)->module_id = CM_MODULE_ID; (dinfo_)->line_nr = (LSA_UINT16) __LINE__; \
	(dinfo_)->reason_code = reason_code_; (dinfo_)->error_cause = error_cause_; (dinfo_)->pnio_status = pnio_status_; \
	(dinfo_)->data2 = 0; \
	CM_MEMCPY ((dinfo_)->mac_addr.mac, mac_addr_, 6); \
	}

#define CM_CL_DINFO_SET_lnr(dinfo_, line_nr_) { \
	(dinfo_)->line_nr = (LSA_UINT16) line_nr_; \
	}

/*----------------------------------------------------------------------------*/
/* AR StartUp Type */

#define CM_SUMODE_UNDEFINED			0

#define CM_SUMODE_IsLegacy			0x01
#define CM_SUMODE_IsAdvanced		0x02
#define CM_SUMODE_IsRT				0x10
#define CM_SUMODE_IsRTClass3		0x20

#define CM_SUMODE_LegacyRT			(CM_SUMODE_IsLegacy | CM_SUMODE_IsRT)
#define CM_SUMODE_LegacyRTClass3	(CM_SUMODE_IsLegacy | CM_SUMODE_IsRTClass3)

#define CM_SUMODE_AdvRT				(CM_SUMODE_IsAdvanced | CM_SUMODE_IsRT)
#define CM_SUMODE_AdvRTClass3		(CM_SUMODE_IsAdvanced | CM_SUMODE_IsRTClass3)

/*----------------------------------------------------------------------------*/
typedef struct cm_cl_argr_tag
{
	struct cm_client_device_tag CM_COMMON_MEM_ATTR
						*dev;				/* backlink; 1 dev for n argr */

	CM_CL_AR_ALARM_TYPE	alarm[2];			/* index: CM_ALARM_PRIORITY_LOW CM_ALARM_PRIORITY_HIGH */

	LSA_UINT8			alarm_enabled_state;/* forward alarm.ind and apdu.ind to user; see CM_CL_ALARM_ENABLED_xxx */

	LSA_BOOL			station_problem;	/* provider in "station problem" */

	/*---*/

	LSA_UINT8			usr_state;			/* state of AR, users view, see CM_CL_AR_xxx */
	LSA_UINT8			state;				/* state of AR, see CM_DST_xxx */

	CM_CL_AR_PRM_TYPE	prm;				/* prm-statemachine, see cm_cl_prm_xxx() */

	LSA_UINT8			sumode;				/* mode of startup, see CM_SUMODE_xxx */

	LSA_UINT8			applrdy_state;		/* application ready; see CM_CL_APPL_READY_STATE_xxx */

	LSA_UINT8			in_down;			/* cm_cl_arsm_down() recursion prevention */

	LSA_UINT32			wait_for[2];		/* wf cm-user action / abort actions / lower cnf */
											/* see cm_cl_wait_for_enum */

	LSA_UINT16			connect_counter;	/* count: every rpc_connect.req */

	LSA_UINT16			record_sequence_nr;	/* sequence_nr for records */
	LSA_UINT16			record_sequence_nr_rollback;
											/* sequence_nr for records, not confirmed yet */
	CM_CL_AR_SCHEDULE_INFO_TYPE
						schedule;			/* needed by cm_cl_schedule_xxx() */

	struct cm_cl_argr_ev_tag
	{
/*		LSA_BOOL				current_fixed;	AP01301749 */
		CM_CL_IP_SUITE_TYPE		ip_suite;
		CM_CL_DOWN_INFO_TYPE	current;
		CM_CL_DOWN_INFO_TYPE	previous;
	}					ev;					/* see CM_OPC_CL_IN_DATA_IND, CM_OPC_CL_AR_OFFLINE_IND */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
						ar_connect_prm_record_list;	/* this list of records will only prm-ed in the connect, index should ar-related */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)
						ar_prm_record_list;	/* this list of records will only prm-ed in connect-phase, index should ar-related */

	CM_UPPER_RQB_PTR_TYPE
						prm_alarm;			/* lower queue: keep plug-alarm */
	CM_LIST_ENTRY_TYPE
						record_list;		/* upper queue: list for read/write records */

	CM_LIST_ENTRY_TYPE	unique_slot_nr_list;/* if used: list with slot_nr used within ap_list, cm_ar_graph_build_unique_slot_nr */
											/* of type: CM_AR_GRAPH_SLOT_NR_PTR_TYPE */
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE
						mtd;				/* parsed PDMasterTailorData */
} CM_CL_ARGR_TYPE;


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ICLAR_H */
