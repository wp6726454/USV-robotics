#ifndef CM_ISVAR_H
#define CM_ISVAR_H

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
/*  F i l e               &F: cm_isvar.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - server ar-graph things                            */
/*                                                                           */
/*****************************************************************************/

#define CM_SV_AR_IS_RTCLASS3(argr)  ((argr)->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3)

#define CM_SV_AR_IS_ADVANCED(argr)  (((argr)->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)

/*---------------------------------------------------------------------------*/

/*** defines for argr->cm.sv.cn_flags ***/

#define CM_SV_AR_CNFL_WF_UNPARK           0x00000001
#define CM_SV_AR_CNFL_WF_SEND_CLOCK       0x00000002
#define CM_SV_AR_CNFL_WF_FRAMES_ADD       0x00000004
#define CM_SV_AR_CNFL_WF_JOIN_CONN        0x00000008
#define CM_SV_AR_CNFL_WF_CONN_RSP         0x00000010
#define CM_SV_AR_CNFL_WF_REBIND           0x00000020
#define CM_SV_AR_CNFL_WF_ALARM_ADD        0x00000040
/***/
#define CM_SV_AR_CNFL_EARLY_ABORT         0x00000080 /* on ar-connect.rsp(-) */
#define CM_SV_AR_CNFL_EXP_USER_REQ        0x00000100 /* expect user-requests after ar-connect.rsp and before ar-disconnect.rsp */
#define CM_SV_AR_CNFL_RETRIGGER_TIMER     0x00000200 /* retrigger the CMI timeout on prm-end, read, write (stop on rpc.ind, restart on rpc.rsp) */
#define CM_SV_AR_CNFL_WF_IPSUITE_LOCK     0x00000400 /* AP01271744 */
#define CM_SV_AR_CNFL_WF_PDEVPRM_RSP      0x00000800 /* waiting for pdevprm.rsp */
/***/
#define CM_SV_AR_CNFL_WF_JOIN_OSHP        0x00001000
#define CM_SV_AR_CNFL_WF_OWNERSHIP_RSP    0x00002000
#define CM_SV_AR_CNFL_WF_PROV_ACT         0x00004000 /* waiting for prov-activate or prov-activate-aux and prov-set-ioparams */
#define CM_SV_AR_CNFL_WF_CONS_ACT         0x00008000 /* waiting for consumer-activate.cnf */
/***/
#define CM_SV_AR_CNFL_WF_RECORD_RSP       0x00010000 /* waiting for record.rsp */
#define CM_SV_AR_CNFL_WF_PRMEND_RSP       0x00020000 /* waiting for prm-end.rsp */
#define CM_SV_AR_CNFL_WF_PD_PRM_END_CNF   0x00040000
#define CM_SV_AR_CNFL_WF_RIR_RSP          0x00080000
#define CM_SV_AR_CNFL_EX_RIR_ACK          0x00100000 /* expect received-in-red acknowledge (CM_OPC_SV_AR_RIR_ACK) */
#define CM_SV_AR_CNFL_WF_INDATA_RSP       0x00200000 /* waiting for in-data.rsp */
#define CM_SV_AR_CNFL_WF_DATA_STATUS_RSP  0x00400000 /* waiting for data-status.rsp */
/***/
#define CM_SV_AR_CNFL_IS_IPSUITE_LOCKED   0x00800000 /* AP01271744 */
#define CM_SV_AR_CNFL_DO_OWNERSHIP_DISC   0x01000000 /* see where used */
#define CM_SV_AR_CNFL_WF_JOIN_DISC        0x02000000
#define CM_SV_AR_CNFL_WF_FRAMES_PAS       0x04000000 /* waiting for frames-passivate.cnf */
#define CM_SV_AR_CNFL_WF_ALARM_RMV        0x08000000 /* waiting for alarm-remove.cnf */
#define CM_SV_AR_CNFL_WF_PDEV_RELINQUISH  0x10000000 /* waiting for pdev-relinquish.cnf */
#define CM_SV_AR_CNFL_WF_RPC_CANCEL_CNF   0x20000000 /* waiting for rpc-cancel.cnf */
#define CM_SV_AR_CNFL_WF_DISC_RSP         0x40000000 /* waiting for ar-disconnect.rsp */
#define CM_SV_AR_CNFL_WF_FRAMES_RMV       0x80000000 /* waiting for frames-remove.cnf */

/*---------------------------------------------------------------------------*/

/*** defines for argr->ar_flags ***/

#define CM_SV_AR_FL_EXP_PRM_END       0x00000001 /* expect a prm-end.ind */
#define CM_SV_AR_FL_PRM_END_IND       0x00000002 /* prm-end.ind (valid rpc-control) seen and indicated to user */
#define CM_SV_AR_FL_PRM_END_RSP       0x00000004 /* got prm-end.rsp from user */
#define CM_SV_AR_FL_APPL_READY_ZCNT   0x00000008 /* set when --pr_appl_ready_cnt == 0 */
#define CM_SV_AR_FL_APPL_READY_REQ    0x00000010 /* rpc-control.REQ started */
#define CM_SV_AR_FL_APPL_READY_CNF    0x00000020 /* rpc-control.CNF seen */
#define CM_SV_AR_FL_NEW_DATA_REQ      0x00000040 /* set2unk.REQ started (new-data.ind requested) */
#define CM_SV_AR_FL_NEW_DATA_CNF      0x00000080 /* set2unk.CNF seen */
#define CM_SV_AR_FL_NEW_DATA_IND      0x00000100 /* new-data.ind seen */
#define CM_SV_AR_FL_CONNECT_PHASE     0x00000200 /* connect-phase: between rpc-connect.ind and rpc-applready.cnf */
#define CM_SV_AR_FL_PREDATA_PHASE     0x00000400 /* predata-phase: between rpc-applready.cnf and in-data.ind */
#define CM_SV_AR_FL_RECEIVED_IN_RED   0x00000800 /* received-in-red seen */
#define CM_SV_AR_FL_SM_ATTACHED       0x00001000 /* did cm_sv_ar_detach() */
#define CM_SV_AR_FL_PD_ATTACHED       0x00002000 /* did cm_pdusr_attach() */
#define CM_SV_AR_FL_QF_OWNERSHIP      0x00004000 /* did cm_sv_ownership_queue_for() */
#define CM_SV_AR_FL_DO_PULL_PLUG      0x00008000 /* see cm_sv_ar_submodule_state_action() */
#define CM_SV_AR_FL_PBE_PHASE         0x00010000 /* prm-begin-end (PBE) phase */
#define CM_SV_AR_FL_PBE_PDEV          0x00020000 /* prm-begin-end contains the pdev */
#define CM_SV_AR_FL_WF_PROV_PAS_AUX   0x00040000 /* waiting for aux-provider-passivate.cnf */
#define CM_SV_AR_FL_WF_RDY4RTC3_CNF   0x00080000 /* wait for rpc-ready4rtclass3.cnf */
#define CM_SV_AR_FL_DO_IN_DATA_IND    0x00100000 /* do in-data.ind */
#define CM_SV_AR_FL_DO_DIAG_ALARM     0x00200000
#define CM_SV_AR_FL_PBE_DO_OSHP_IND   0x00400000
#define CM_SV_AR_FL_PBE_DO_CTRL_RSP   0x00800000
#define CM_SV_AR_FL_PBE_PLG_REL       0x01000000 /* prm-begin-end overlaps with plug or release alarm */
#define CM_SV_AR_FL_PBE_DELAY         0x02000000 /* prm-begin before appl-ready.cnf */
#define CM_SV_AR_FL_DO_CONS_12UDP     0x04000000 /* consumer activation, legacy and advanced (AP01256809, AP01205330, RTC3: see note on green/red consumer) */
#define CM_SV_AR_FL_AR_PROBLEM        0x08000000 /* the AR has a problem with the send-clock, pdev-ownership, or IRDataUUID */
#define CM_SV_AR_FL_SET_MRP_OFF       0x10000000
#define CM_SV_AR_FL_ARP_PDEV          0x20000000 /* PDEV could not be committed or saw invalid PDEV records */
#define CM_SV_AR_FL_ARP_E000          0x40000000 /* all submodules are "a.r.p." because an 0xExxx Index had an error */
#define CM_SV_AR_FL_DOWN_PHASE        0x80000000 /* cm_sv_ar_com_down() was called */

#define CM_SV_AR_FL_PRM_MASK \
	( CM_SV_AR_FL_EXP_PRM_END \
	| CM_SV_AR_FL_PRM_END_IND \
	| CM_SV_AR_FL_PRM_END_RSP \
	| CM_SV_AR_FL_APPL_READY_REQ \
	| CM_SV_AR_FL_APPL_READY_CNF \
	)

#define CM_SV_AR_FL_NEW_MASK \
	( CM_SV_AR_FL_NEW_DATA_REQ \
	| CM_SV_AR_FL_NEW_DATA_CNF \
	| CM_SV_AR_FL_NEW_DATA_IND \
	)

/*---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_pi_tag { /* problem-indicator state-machine */

	LSA_BOOL is_enabled; /* true: try keeping actual equal target value */
	LSA_BOOL pi_target;  /* PI-state of the ar */
	LSA_BOOL pi_actual;  /* PI-state of the iocrs */
	LSA_BOOL pi_pending; /* PI-control pending */

} CM_SV_AR_PI_TYPE;

/*---------------------------------------------------------------------------*/

struct cm_sv_iterator_tag; /* forward, see cm_isv.h */

typedef struct cm_sv_ar_rec_tag { /* record state-machine */

	LSA_UINT16 seq_number_exp; /* expected */
	LSA_UINT16 seq_number_pre; /* previous */

	LSA_UINT32 multi_offs; /* != 0 if is-write-multiple */
	LSA_UINT32 multi_pending_cnt; /* AP00786836 */

	/*CM_SV_ITERATOR_TYPE*/struct cm_sv_iterator_tag CM_COMMON_MEM_ATTR *iter_ptr; /* an iterator for the F8xx records */

	LSA_UINT16 ardata_refcnt; /* see cm_sv_record_marshal_ARData_next() */

	LSA_UINT16 ARFSUDataAdjust_len;
	CM_LOCAL_MEM_PTR_TYPE ARFSUDataAdjust_ptr; /* AP00511230: keep a copy for read */

	LSA_UINT64 pdev_crc64; /* see usage (AP01238541) */

	LSA_BOOL is_rerun; /* introduced for "pdev_crc64 and rerun" (AP01290734) */

} CM_SV_AR_REC_TYPE;

/*---------------------------------------------------------------------------*/

/*** defines for almi[prio].flags ***/

#define CM_SV_AR_ALMI_IDLE         0x00 /* idle, all bits 0 */
#define CM_SV_AR_ALMI_ALARM_REQ    0x01 /* alarm.REQ made (not idle) */
#define CM_SV_AR_ALMI_ALARM_CNF    0x02 /* alarm.CNF seen */
#define CM_SV_AR_ALMI_ALARM_ACK    0x04 /* alarm-ack.IND seen */
#define CM_SV_AR_ALMI_DO_SET2UNK   0x08 /* do set2unk after alarm-ack.IND (plug, release or return-of-submodule alarm) */
#define CM_SV_AR_ALMI_IS_PLG_REL   0x10 /* modifier of DO_SET2UNK: plug or release alarm (expecting prm-sequence) */
#define CM_SV_AR_ALMI_RTC3_ACTION  0x20 /* rtc3-action running */
#define CM_SV_AR_ALMI_NWDT_ACTION  0x40 /* new-data-action running */
#define CM_SV_AR_ALMI_DO_ARDY      0x80 /* call cm_sv_rpc_appl_ready_request() on alarm-ack.ind */

typedef struct cm_sv_ar_almi_tag { /* alarm initiator */

	LSA_UINT16 sequence_number;

	LSA_UINT8 flags; /* see CM_SV_AR_ALMI_IDLE, ... */

	LSA_BOOL is_enabled;

	LSA_BOOL set2unk_enabled; /* only for "plug-prio" */

	LSA_BOOL do_rtc3_action; /* do ready-for-rtc3 rpc or prov-activate, only for "plug-prio" */

	LSA_BOOL do_new_data; /* do new-data.req/cnf/ind sequence */

	CM_LIST_ENTRY_TYPE queue;

	CM_UPPER_RQB_PTR_TYPE curr;

	/***/

	CM_ACP_LOWER_RQB_PTR_TYPE acp_rqb;

	CM_COMMON_MEM_U8_PTR_TYPE acp_mem; /* note: same type as CM_ALARM_TYPE::alarm_data */

	CM_UPPER_RQB_PTR_TYPE rpl_rqb; /* replacement RQB (see alarm cancellation and system redundancy, AP01280175) */

} CM_SV_AR_ALMI_TYPE;

typedef CM_SV_AR_ALMI_TYPE CM_LOCAL_MEM_ATTR *CM_SV_AR_ALMI_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct cm_sv_ar_almr_tag { /* alarm responder */

	CM_UPPER_RQB_PTR_TYPE delay; /* delay alarm until IN_DATA or ack-send.CNF */

	LSA_UINT16 sequence_number; /* expected */

	LSA_BOOL wf_alm_rsp; /* != LSA_NULL if waiting for alarm.RSP (after alarm.IND) */

	LSA_BOOL wf_ack_req; /* != LSA_NULL if waiting for alarm-ack.REQ (after alarm.RSP) */

	CM_UPPER_RQB_PTR_TYPE curr_ack; /* the alarm-ack.REQ (i.e., waiting for ack-send.CNF) */

	/***/

	CM_ACP_LOWER_RQB_PTR_TYPE acp_rqb;

	CM_UPPER_RQB_PTR_TYPE ack_rqb; /* for internal alarm-acks */

} CM_SV_AR_ALMR_TYPE;

typedef CM_SV_AR_ALMR_TYPE CM_LOCAL_MEM_ATTR *CM_SV_AR_ALMR_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct cm_sv_argr_tag {

	/*CM_SV_DEVICE_PTR_TYPE*/ struct cm_sv_device_tag CM_LOCAL_MEM_ATTR *dev;

	/*CM_SV_ARSET_PTR_TYPE*/ struct cm_sv_arset_tag CM_LOCAL_MEM_ATTR *arset; /* the AR-set if CM_SV_AR_IS_SYSRED(), else LSA_NULL */

	/***/

	LSA_UINT16 session_key_connect; /* the value from the connect request */

	LSA_UINT32 cn_flags; /* see CM_SV_AR_CNFL_WF_UNPARK, ... */

	LSA_UINT32 ar_flags; /* see CM_SV_AR_FL_EXP_PRM_END, ... */

	LSA_UINT32 pr_appl_ready_cnt; /* incr on prm-end.rsp, decr on appl-ready.req, zero then RPC */

	CM_UPPER_ALARM_PTR_TYPE plug_group_alarm;

	LSA_UINT16 nr_of_submodules;

	LSA_UINT16 nr_of_pdev_submodules; /* != 0 only if CM_SV_DEV_IS_PDEV_DEVICE() and cm_pdsv_mapping_ok(), see cm_sv_subslot_range_ok() */

	LSA_UINT32 provider_sequence; /* AP00619993 (stable sequence of provider frames), AP01379821 (16->32 bits) */

	LSA_BOOL sysred_IOCARSR_backup; /* true if got an AlarmAck with "IOCARSR backup" on any alarm-prio (TIA 209760) */

	/***/

	CM_RPC_LOWER_RQB_PTR_TYPE curr_rpc_ind; /* current rpc indication, here until event-response */

	CM_UPPER_RQB_PTR_TYPE     curr_ar_ctrl; /* current ar-control */

	CM_UPPER_RQB_PTR_TYPE     curr_ar_pdevprm; /* current ar-pdevprm read or write */

	CM_UPPER_RQB_PTR_TYPE     curr_arset_trigger; /* only for CM_AR_TYPE_SINGLE_SYSRED */

	/***/

	CM_SV_AR_PI_TYPE pi;

	CM_SV_AR_REC_TYPE rec;

	CM_SV_AR_ALMI_TYPE almi[2]; /* index = priority, 0=low 1=high */

	CM_SV_AR_ALMR_TYPE almr[2]; /* index = priority, 0=low 1=high */

	/***/

	CM_OHA_LOWER_RQB_PTR_TYPE lock_rqb; /* for lock/unlock ip-suite (AP01271744) */

	CM_RPC_LOWER_RQB_PTR_TYPE rebind_rqb; /* for rebind and cancel (AP00640250) */

	CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect_mem; /* see cm_sv_ar_connect_alloc() */

	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership_mem; /* see cm_sv_ar_ownership_alloc() */

	CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end_mem; /* see cm_sv_ar_prm_end_alloc(), note: cannot reuse ar_ownership_mem */

	CM_UPPER_SV_AR_IN_DATA_PTR_TYPE ar_in_data_mem; /* see cm_sv_ar_in_data_alloc(), note: cannot reuse ar_prm_end_mem */

	CM_UPPER_RQB_PTR_TYPE pdevprm_rdwr_rqb; /* for pdev-read or pdev-write (TIA 1095777) */

	CM_UPPER_RQB_PTR_TYPE pdev_end_rqb; /* for pdev-prmend (AP00640250) */

	CM_UPPER_RQB_PTR_TYPE pdev_rel_rqb; /* for pdev-relinquish (AP00640250) */

	CM_RPC_LOWER_RQB_PTR_TYPE rdy4rtc3_rqb; /* for "ready for RTClass3" callback (AP00640250) */

	CM_RPC_LOWER_RQB_PTR_TYPE applrdy_rqb; /* for "application ready" callback (AP00640250) */

} CM_SV_ARGR_TYPE;

/*----------------------------------------------------------------------------*/

/*** defines for ar_sub->sv.sm_flags ***/

#define CM_SV_AR_SMFL_TNA          0x0001 /* takeover not allowed */
#define CM_SV_AR_SMFL_DIRTY        0x0002 /* submodule state needs to be updated */
#define CM_SV_AR_SMFL_OSHP         0x0004 /* do an ownership.ind if plugged */
#define CM_SV_AR_SMFL_WRONG        0x0008 /* result from ownership.rsp (submodule is wrong) */
#define CM_SV_AR_SMFL_WF_PRMEND    0x0010 /* waiting for ar-prm-end.rsp (a ref-counter) */
#define CM_SV_AR_SMFL_EX_APLRDY    0x0020 /* expect ar-appl-ready.req */
#define CM_SV_AR_SMFL_IS_PRMED     0x0040 /* parameterized (i.e., alarms are enabled) */
#define CM_SV_AR_SMFL_IS_ARP       0x0080 /* modifier of IS_PRMED: application-ready pending (parameterization is faulty) */
#define CM_SV_AR_SMFL_EX_NEWDTA    0x0100 /* modifier of IS_PRMED: expect new-data */
#define CM_SV_AR_SMFL_PBE          0x0200 /* the submodule is part of a prm-begin-end sequence */
#define CM_SV_AR_SMFL_PBE_PLG_REL  0x0400 /* prm-begin-end overlaps with plug or release alarm */

/* see CM_AR_GRAPH_SUBMODULE_PTR_TYPE in cm_argr.h */

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ISVAR_H */
