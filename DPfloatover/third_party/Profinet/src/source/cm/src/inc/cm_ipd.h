#ifndef CM_IPD_H
#define CM_IPD_H

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
/*  F i l e               &F: cm_ipd.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - physical device things                            */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*=== used by CMPD and CMCL =================================================*/
/*===========================================================================*/

#define CM_ALARM_SEQUENCE_UNDEF		0xFFFF /* valid alarm_sequence: 0..0x07FF */

#define CM_ALARM_SEQUENCE_NR_INCREMENT(seq_nr) {\
		(seq_nr) += 1; \
		if ((seq_nr) > 0x07FF) { (seq_nr) = 0; } \
	}

#define CM_IS_PDEV_SUBSLOT_NR(subslot_nr)	(((subslot_nr) & 0xF000) == 0x8000)

#define CM_REDUNDANT_FRAME_ID_BASE(frame_id)		((0x0700 <= (frame_id) && (frame_id) <= 0x0FFF) ? ((frame_id) & 0x0FFE) : 0)

/*------------------------------------------------------------------------------
// for marshalling and parsing
//----------------------------------------------------------------------------*/

#define cm_put_assert(cond_) {\
	if (! (cond_)) { \
		cm_put_assert_line = (LSA_UINT16)(__LINE__);\
		goto cm_put_assert_failed;\
	}}

#define cm_get_assert_error_set(err_) {\
		cm_get_assert_line = (LSA_UINT16)(__LINE__);\
		cm_get_assert_error = err_;\
	}

#define cm_get_assert(cond_) {\
	if (! (cond_)) {\
		cm_get_assert_error_set (CM_PNIO_ERR_CODE_12(0/*!*/, CM_AR_REASON_PROT));\
		goto cm_get_assert_failed;\
	}}

#define cm_get_assert2(cond_, err_) {\
	if (! (cond_)) {\
		cm_get_assert_error_set (err_);\
		goto cm_get_assert_failed;\
	}}

#define cm_adjust(dp_, left_, sizeof_) {\
	dp_ += (sizeof_); \
	left_ -= (sizeof_); \
	}

/*----------------------------------------------------------------------------*/
/* assert size of fields */

#if CM_DEBUG  &&  defined(_MSC_VER)

# define CM_SIZE_ASSERT_8(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT8) == sizeof (((type_*)0)->field_))

# define CM_SIZE_ASSERT_16(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT16) == sizeof (((type_*)0)->field_))

# define CM_SIZE_ASSERT_32(type_, field_) \
	CM_ASSERT (sizeof(LSA_UINT32) == sizeof (((type_*)0)->field_))

#else

# define CM_SIZE_ASSERT_8(type_, field_)
# define CM_SIZE_ASSERT_16(type_, field_)
# define CM_SIZE_ASSERT_32(type_, field_)

#endif

/*----------------------------------------------------------------------------*/

#define cm_put_u8(dp_, type_, field_, u8_) \
	CM_SIZE_ASSERT_8 (type_, field_); \
	CM_PUT8_HTON ((dp_), CM_OFFS (type_, field_), (u8_))

#define cm_put_u16(dp_, type_, field_, u16_) \
	CM_SIZE_ASSERT_16 (type_, field_); \
	CM_PUT16_HTON ((dp_), CM_OFFS (type_, field_), (u16_))

#define cm_put_u32(dp_, type_, field_, u32_) \
	CM_SIZE_ASSERT_32 (type_, field_); \
	CM_PUT32_HTON ((dp_), CM_OFFS (type_, field_), (u32_))

#define cm_put_uuid(dp_, type_, field_, uuid_ptr_) \
	CM_PUT_UUID_HTON ((dp_), CM_OFFS (type_, field_), (uuid_ptr_))

#define cm_put_mac(dp_, type_, field_, mac_ptr_) \
	CM_MEMCPY ((dp_) + CM_OFFS (type_, field_), (mac_ptr_), 6)

#define cm_put_memset(dp_, type_, field_, u8_) {\
	LSA_UINT16  i;\
	for (i = 0; i < sizeof (((type_*)0)->field_); ++i) {\
		*((dp_) + CM_OFFS (type_, field_) + i) = u8_; \
	}}

/*----------------------------------------------------------------------------*/

#define cm_get_u16(u16_ptr_, dp_, type_, field_) \
	CM_SIZE_ASSERT_16 (type_, field_); \
	*(u16_ptr_) = CM_GET16_NTOH ((dp_), CM_OFFS (type_, field_))

#define cm_get_u32(u32_ptr_, dp_, type_, field_) \
	CM_SIZE_ASSERT_32 (type_, field_); \
	*(u32_ptr_) = CM_GET32_NTOH ((dp_), CM_OFFS (type_, field_))

#define cm_get_uuid(uuid_ptr_, dp_, type_, field_) \
	CM_GET_UUID_NTOH ((uuid_ptr_), (dp_), CM_OFFS (type_, field_))

#define cm_get_mac(mac_ptr_, dp_, type_, field_) \
	CM_MEMCPY ((mac_ptr_), (dp_) + CM_OFFS (type_, field_), 6)



/*------------------------------------------------------------------------------
// AR-Graph, Record
//----------------------------------------------------------------------------*/
#  ifdef CM_MESSAGE
/* todo: rename to cm_list_record_tag */
#  endif /* CM_MESSAGE */

#define CM_TAILOR_DATA_LEN_NOT_TAILORED		0
#define CM_TAILOR_DATA_LEN_SKIP_RECORD		1

typedef struct cm_ar_graph_record_tag {
	CM_LIST_ENTRY_TYPE		link; /* first in struct!  link within  submodule // CM_PD_CHANNEL_TYPE::reclist */

	CM_LIST_ENTRY_TYPE			prm_link; /* link within prm.prm_list */
	LSA_UINT32					api;
	LSA_UINT16					slot_nr;
	LSA_UINT16					subslot_nr;

	LSA_UINT32					record_index;
	LSA_UINT16					transfer_sequence;

	LSA_BOOL					mk_remanent;
	LSA_BOOL					is_local;

	LSA_UINT16					tailor_data_alloc_len;
	LSA_UINT16					tailor_data_len;
	CM_COMMON_MEM_U8_PTR_TYPE	tailor_data;

	LSA_UINT16					record_data_len;
	LSA_UINT8					record_data[1];
	
	/*** nothing here, dynamic size ***/

} CM_COMMON_MEM_ATTR  * CM_AR_GRAPH_RECORD_PTR_TYPE;


/*===========================================================================*/
/*=== DIAG ==================================================================*/
/*===========================================================================*/

typedef struct cm_pd_diag_tag {
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag) link; /* first in struct */

	LSA_UINT16 channel_properties;
	LSA_UINT16 channel_error_type;
	LSA_UINT16 ext_channel_error_type;
	LSA_UINT32 ext_channel_add_value;

	LSA_UINT16 alarm_type;

} CM_PD_DIAG_TYPE;

typedef CM_PD_DIAG_TYPE CM_LOCAL_MEM_ATTR *CM_PD_DIAG_PTR_TYPE;

/*===========================================================================*/
/*=== ADDRESS-MAPPING =======================================================*/
/*===========================================================================*/

#define CM_PD_INVALID_PORT_ID   0xFFFF

enum cm_pd_occupant_enum {

	CM_PD_OCCUPANT_NOBODY = 0,
	CM_PD_OCCUPANT_USER   = 1,
	CM_PD_OCCUPANT_SV     = 2
};

typedef struct cm_pd_port_addr_tag {

	/* logical address */
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;	/* mapped, if != 0, see format 0x8ipp */

	LSA_UINT32 mod_ident;
	LSA_UINT32 sub_ident;

} CM_PD_PORT_ADDR_TYPE;

/*===========================================================================*/
/*=== PORT ==================================================================*/
/*===========================================================================*/

enum cm_pd_topo_state_enum {

	CM_PD_TOPO_STATE_UNKNOWN  = 1,
	CM_PD_TOPO_STATE_NOK      = 2,
	CM_PD_TOPO_STATE_OK       = 3
};

typedef struct cm_pd_port_tag {

	LSA_UINT16  port_id; /* key of this entry, given by create */

	CM_PD_PORT_ADDR_TYPE  addr; /* address mapping, ident_numbers */

	struct {

		LSA_BOOL  update_allowed; /* see cm_pd_diag_update_start / cm_pd_diag_update_done */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list; /* list of CM_PD_DIAG_TYPE */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list_server; /* list of CM_PD_DIAG_TYPE */

		CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diag_tag)  diag_list_user; /* list of CM_PD_DIAG_TYPE */

	} diag;

	struct {

		LSA_UINT16 sync_led_info; /* see cm_pd_sync_led_info_enum */
		LSA_BOOL   sync_led__is_rate_valid;
		LSA_UINT8  sync_led__rcv_sync_prio;
		LSA_UINT8  sync_led__local_prio;
		LSA_UINT8  sync_led__pll_state;

		LSA_UINT8  topo_state; /* state of topo, see cm_pd_topo_state_enum */

		LSA_UINT8  MRPRingPort; /* EDD_MRP_NO_RING_PORT etc. */
		/* interface: see EDDGetParams.MRPSupportedRole   port: see EDDGetPortParams.MRPRingPort */

		CM_UPPER_RQB_PTR_TYPE  link_status_ind; /* only one resource allowed per port_id, see CM_OPC_PD_LINK_STATUS_INFO */
	} misc;

} CM_PD_PORT_TYPE;

typedef CM_PD_PORT_TYPE CM_LOCAL_MEM_ATTR *CM_PD_PORT_PTR_TYPE;


typedef struct cm_pd_port_prm_data_tag {

	LSA_BOOL  do_irt;				/* index 0x802C: PDIRData */
	LSA_BOOL  do_mrp_record;		/* something is written to MRP */
	LSA_UINT16  scr_send_clock;		/* from SendClock-record */
	LSA_UINT16  scr_sendclock_prop;	/* from SendClock-record */
	
	CLRPC_UUID_TYPE  ir_data_uuid;	/* PDIRData.PDIRGlobalData.IRDataUUID */

	LSA_UINT16  pll_reduction_ratio;	/* PDSyncPLL.ReductionRatio,  0 if not written */
	LSA_UINT16  pll_hw_delay;			/* PDSyncPLL.PLLHWDelay */
	LSA_UINT16  pll_cntrl_interval;		/* PDSyncPLL.ControlInterval */
	LSA_UINT32	pll_window;				/* PDSyncPLL.PLLWindow */
	LSA_UINT16  pll_setwaitcount;		/* PDSyncPLL.SetWaitCount v1.1 */

} CM_PD_PORT_IF_PRM_DATA_TYPE;


typedef struct cm_pd_port_if_tag {

	CM_PD_PORT_TYPE  port; /* isa port */

	/* here interface specific things */

/* note: functionality moved to reclist
// VV: remove CM_PD_PORT_IF_TYPE
	CM_PD_PORT_IF_PRM_DATA_TYPE	 A;
	CM_PD_PORT_IF_PRM_DATA_TYPE	 B;
*/
} CM_PD_PORT_IF_TYPE;

typedef CM_PD_PORT_IF_TYPE CM_LOCAL_MEM_ATTR *CM_PD_PORT_IF_PTR_TYPE;

/*===========================================================================*/
/*=== PD-USER CHANNEL-TYPE ==================================================*/
/*===========================================================================*/

enum cm_sub_comp_id_enum
{
	CM_PD_SCID_EDD		= (CM_REC_IDX_CMPD_EDD),			/* 1 */
#if CM_CFG_USE_MRP
	CM_PD_SCID_MRP		= (CM_REC_IDX_CMPD_MRP),			/* 2 */
#endif
#if CM_CFG_USE_GSY
	CM_PD_SCID_GSY_CLOCK= (CM_REC_IDX_CMPD_GSY_CLOCK),		/* 3 */
	CM_PD_SCID_GSY_TIME	= (CM_REC_IDX_CMPD_GSY_TIME),		/* 4 */
#endif
#if CM_CFG_USE_POF
	CM_PD_SCID_POF		= (CM_REC_IDX_CMPD_POF),			/* 5 */
#endif
	CM_PD_SCID_OHA		= (CM_REC_IDX_CMPD_OHA),			/* 6 */
	CM_PD_SCID_BBUF		= (CM_REC_IDX_CMPD_BBUF),			/* 7 */
	CM_PD_SCID_CMPD		= (CM_REC_IDX_CMPD_PD),				/* 8 */

	CM_PD_SCID_MAX		= (CM_REC_IDX_CMPD_PD + 1)
};

enum cm_sub_comp_bf_enum
{
	CM_PD_SCBF_EDD		= (0x01 << (CM_PD_SCID_EDD)),		/* 0x002 */
#if CM_CFG_USE_MRP
	CM_PD_SCBF_MRP		= (0x01 << (CM_PD_SCID_MRP)),		/* 0x004 */
#endif
#if CM_CFG_USE_GSY
	CM_PD_SCBF_GSY_CLOCK= (0x01 << (CM_PD_SCID_GSY_CLOCK)),	/* 0x008 */
	CM_PD_SCBF_GSY_TIME	= (0x01 << (CM_PD_SCID_GSY_TIME)),	/* 0x010 */
#endif
#if CM_CFG_USE_POF
	CM_PD_SCBF_POF		= (0x01 << (CM_PD_SCID_POF)),		/* 0x020 */
#endif
	CM_PD_SCBF_OHA		= (0x01 << (CM_PD_SCID_OHA)),		/* 0x040 */
	CM_PD_SCBF_CMPD		= (0x01 << (CM_PD_SCID_CMPD)),		/* 0x100 */

	/* special */
	CM_PD_SCBF_EDD_LINK_STATUS		= 0x0800
};

enum cm_pd_prm_state_enum
{
	CM_PD_PRM_STATE_WF_PREPARE		= 1,
	CM_PD_PRM_STATE_WORKING			= 2,
	CM_PD_PRM_STATE_WF_WRITE		= 3,
	CM_PD_PRM_STATE_WF_END			= 5,
	CM_PD_PRM_STATE_WF_COMMIT		= 6,
	CM_PD_PRM_STATE_WF_SC_WORKING	= 7
};

enum cm_pd_wait_for_enum
{
	CM_PD_PRM_DO_PREPARE		= 0x0001,
	CM_PD_PRM_DO_WRITE			= 0x0002,
	CM_PD_PRM_DO_END			= 0x0004,
	CM_PD_PRM_DO_COMMIT			= 0x0008,
	CM_PD_PRM_DO_FIRST_IND		= 0x0010,

	CM_PD_PRM_WF_PREPARE_CNF	= 0x0100,
	CM_PD_PRM_WF_WRITE_CNF		= 0x0200,
	CM_PD_PRM_WF_END_CNF		= 0x0400,
	CM_PD_PRM_WF_COMMIT_CNF		= 0x0800,
	CM_PD_PRM_WF_FIRST_IND		= 0x1000,

	CM_PD_PRM_WF_READ_CNF		= 0x2000,

	CM_PD_PRM_IS_INTERNAL_WRITE	= 0x4000,
	CM_PD_PRM_IS_WRITE_SENDCLOCK= 0x8000,

	CM_PD_PRM_
};

typedef struct cm_pd_prm_tag
{
	CM_CHANNEL_PTR_TYPE	channel;	/* backlink */
	LSA_UINT32			trace_idx;
	LSA_UINT8			state;		/* see cm_pd_prm_state_enum */
	LSA_BOOL			is_parameterized; /* true if at least once parameterized successfully */
	LSA_BOOL			is_reset_to_factory; /* is parameterized because of ResetToFactory */

	LSA_UINT8			occupant;	/* current owner, see cm_pd_occupant_enum */
	LSA_RESPONSE_TYPE	last_appl_ready_ind; /* response of last indication */

	LSA_UINT16			device_nr;	/* set by plug / map / pull / unmap */
	LSA_UINT16			ar_nr;		/* set by ownership / transfer / relinquish */

	LSA_UINT32			wait_cnf;	/* wait for current cm_pd_config_start() */
	LSA_UINT16			wait_cnf_next;

	LSA_UINT16			wait_for;	/* wait for current working command */
	LSA_UINT16			wait_sc;	/* wait for subcomponents with above command */
	LSA_UINT16			wait_sc_ind;/* wait for subcomponents first indication */

	LSA_RESPONSE_TYPE	response;	/* summary of subcomponents response, mapped to cm-err */

	LSA_UINT16			end_counter;/* current prm-end-counter, see ... */
	LSA_UINT16			arp;		/* application ready pending, if sc_write.cnf(-) or sc_end.cnf(-) */
	CM_PORT_UINT		wait_first_ind[CM_PD_SCID_MAX];
									/* wait for first indication of subcomponent and port */
									/* every written port must fire one indication */
									/* bit = ((CM_PORT_UINT)1 << port_id) */

	CM_UPPER_RQB_PTR_TYPE		pd_config_rqb;	/* internal, used by cm_pd_config_xxx() */
	
	CM_AR_GRAPH_RECORD_PTR_TYPE	write;
	CM_UPPER_RQB_PTR_TYPE		write_nonprm;	/* keep upper requests */
	
	CM_UPPER_RQB_PTR_TYPE		end;
	CM_UPPER_RQB_PTR_TYPE		relinquish;	/* or prm_prepare.req */

	CM_UPPER_RQB_PTR_TYPE	read;

} CM_PD_PRM_TYPE;

typedef CM_PD_PRM_TYPE CM_COMMON_MEM_ATTR  * CM_PD_PRM_PTR_TYPE;

typedef struct cm_pd_reclist_tag
{
	LSA_BOOL										is_valid;            /* LSA_TRUE: if not in prm sequence   LSA_FALSE: between PrmBegin and PrmEnd */
	LSA_BOOL										has_error;           /* LSA_TRUE: if error occured during user prm sequence */
	LSA_UINT32										faulty_record_index; /* first faulty record index */
	CM_PD_PORT_IF_PRM_DATA_TYPE						prm_data;
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	regular_list;        /* normal records */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	empty_list;          /* empty parameterization */
	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag)	rtf_list;            /* ResetToFactory */

	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE				mtd;				/* parsed PDMasterTailorData */
/* must not keep
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE				itd;				// parsed PDInstanceTailorData */
	LSA_BOOL										is_running_mpa;		/* LSA_TRUE: a PDInstanceTailorData is 'running' */
	CM_UPPER_RQB_PTR_TYPE							mpa_rb;				/* if is_running_mpa stored PDInstanceTailorData RQB, else LSA_NULL */
} CM_PD_RECLIST_TYPE;

typedef CM_PD_RECLIST_TYPE CM_COMMON_MEM_ATTR  * CM_PD_RECLIST_PTR_TYPE;


enum cm_pd_iter_pdev_enum {

	CM_PD_ITER_PDEV_IDLE = 0,

	/* PDevData (0xF831) */
	CM_PD_ITER_PDEV_BlockHeaderPDev,
	CM_PD_ITER_PDEV_IRData,
	CM_PD_ITER_PDEV_PDSyncDataClock,
	CM_PD_ITER_PDEV_PDSyncDataTime,
	CM_PD_ITER_PDEV_PDIRSubframeData,
	CM_PD_ITER_PDEV_PDevDataComplete,

	/* PDRealData (0xF841) */
	CM_PD_ITER_PDEV_MultipleBlockHeaderReal,
	CM_PD_ITER_PDEV_PDPortDataReal,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataReal,
	CM_PD_ITER_PDEV_PDPortMrpDataReal,
	CM_PD_ITER_PDEV_PDPortFODataReal,
	CM_PD_ITER_PDEV_PDInterfaceDataReal,
	CM_PD_ITER_PDEV_PDPortStatistic,
	CM_PD_ITER_PDEV_PDRealDataComplete,

	/* PDExpectedData (0xF842) */
	CM_PD_ITER_PDEV_MultipleBlockHeaderExpected,
	CM_PD_ITER_PDEV_PDPortDataCheck,
	CM_PD_ITER_PDEV_PDPortDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceMrpDataCheck,
	CM_PD_ITER_PDEV_PDPortMrpDataAdjust,
	CM_PD_ITER_PDEV_PDPortFODataAdjust,
	CM_PD_ITER_PDEV_PDPortFODataCheck,
	CM_PD_ITER_PDEV_PDNCDataCheck,
	CM_PD_ITER_PDEV_PDInterfaceFSUDataAdjust,
	CM_PD_ITER_PDEV_PDInterfaceAdjust,
	CM_PD_ITER_PDEV_PDExpectedDataComplete,

	/* ARData */
	CM_PD_ITER_PDEV_ARData_Setup,
	CM_PD_ITER_PDEV_ARData_GetAPDUStatus,
	CM_PD_ITER_PDEV_ARData_GetAPDUStatus_Done
};

typedef struct cm_pd_iter_tag {

	struct { /* similar to CM_SV_ITERATOR_TYPE::pdev */

		LSA_UINT16 state; /* see cm_pd_iter_pdev_enum */

		CM_PD_PRM_RDWR_TYPE org; /* original request */

		LSA_UINT32 offs_tot;
		LSA_UINT32 offs_blk;

	} pdev;

	LSA_UINT16 port_id;

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;

} CM_PD_ITER_TYPE;

typedef CM_PD_ITER_TYPE CM_COMMON_MEM_ATTR  * CM_PD_ITER_PTR_TYPE;

typedef struct cm_pd_multi_write_tag
{
	LSA_BOOL  is_running;
	LSA_BOOL  all_records_done;
	LSA_BOOL  mk_remanent;
	LSA_BOOL  is_local;
	CM_UPPER_RQB_PTR_TYPE  internal_rqb;
	CM_UPPER_MEM_U8_PTR_TYPE  data;
	LSA_UINT32  data_length;
	LSA_UINT32  data_offs;
	LSA_RESPONSE_TYPE  rsp; /* summary of subcomponents response, mapped to cm-err */

} CM_PD_MULTI_WRITE_TYPE;

typedef CM_PD_MULTI_WRITE_TYPE CM_COMMON_MEM_ATTR  * CM_PD_MULTI_WRITE_PTR_TYPE;

typedef struct cm_pd_rtf_tag
{
	LSA_UINT32	wf_sm;		/* ResetToFactory, NoS or IPSuite changed - wait_for - statemachine, see CM_PD_RTF_WF__xxx */
} CM_PD_RTF_TYPE;

typedef CM_PD_RTF_TYPE CM_COMMON_MEM_ATTR  * CM_PD_RTF_PTR_TYPE;

typedef struct cm_pd_channel_tag {
	/*
	 * the states of the state machine CM-PD
	 * are defined in cm_int.h "in-line" with
	 * the states of CM-CH
	 */

	struct {
		CM_LIST_ENTRY_TYPE  queue;
		CM_UPPER_RQB_PTR_TYPE  current;
		CM_PD_ITER_TYPE  iter;
		LSA_BOOL   is_busy;
		LSA_BOOL   delete_is_in_queue;
		LSA_UINT16  alarm_ind_count; /* CM_OPC_PD_ALARM_IND */

		CM_PD_MULTI_WRITE_TYPE  multi_write;
	} req;

	/***/

	LSA_UINT16			port_count;
	CM_PD_PORT_IF_TYPE	port_if;		/* port_id == 0 */
	CM_PD_PORT_PTR_TYPE	port_array_ptr;	/* pointer to array of ports (interface is port_id = 0) */ /* 1 <= port_id <= port_count */

	struct {
		LSA_UINT16  send_clock;     /* current SendClockFactor */
		LSA_UINT16  ar_cnt;         /* number of ARs using this send-clock */
		LSA_BOOL    is_pending;     /* LSA_TRUE: wait for set-send-clock.CNF */
		LSA_UINT16  sendclock_prop; /* see cm_sendclock_prop_enum */
	} clock;

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red;

	CM_PD_PRM_TYPE  pdprm;

	CM_PD_RECLIST_TYPE  reclist;

	struct {
		LSA_BOOL	server_allowed;
		LSA_BOOL	server_wf_alarm_rsp; /*AP00635800*/
		LSA_UINT16  server_max_alarm_data_length;

		LSA_BOOL	user_allowed;
		LSA_UINT16	user_sequence_nr;

		LSA_UINT16 appl_ready_ind_pending; /* appl ready indication waiting for resources */
	} almi;

	struct {
		CM_LIST_ENTRY_TYPE led_info_list;

		LSA_BOOL tell_oha; /* LSA_TRUE .. must tell OHA the new sync state */
		LSA_BOOL oha_ptcp_write_pending;

	} sync;

	struct {
		LSA_UINT16	server_id;			/* server-id of registered clrpc-endpoint-mapper */

		LSA_UINT16  im5_record_data_length; /* see I&M5 */
		CM_UPPER_MEM_U8_PTR_TYPE  im5_record_data;
	} rpc;

	CM_PD_RTF_TYPE	rtf; /* ResetToFactory, NoS or IPSuite changed */

} CM_PD_CHANNEL_TYPE;


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_ready (
	CM_CHANNEL_PTR_TYPE usr_channel
);

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS
LSA_UINT8  CM_LOCAL_FCT_ATTR
cm_pdusr_is_irt_local_allowed (
	CM_CHANNEL_PTR_TYPE usr_channel
);
#endif

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_req_accept (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rqb,
	LSA_UINT16  state
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_req_queue_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_req_queue_remove_current (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_req_callback (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  response,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_create(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_delete(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_delete(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_remap (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_remap (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_rpc_epm_start_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16 rpc_response
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_rpc_epm_stop_done (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_set_ident_done(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 oha_response
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_get_port_params_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_parameterized (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 CM_LOCAL_MEM_ATTR *send_clock,
	LSA_BOOL CM_LOCAL_MEM_ATTR *is_reduction_ratio_fixed
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_is_send_clock_fixed ( /* returns the send_clock if "fixed", 0 otherwise */
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_BOOL CM_LOCAL_MEM_ATTR *is_reduction_ratio_fixed
);

CLRPC_UUID_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_get_ir_data_uuid (
	CM_CHANNEL_PTR_TYPE  usr_channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_attach (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_UINT16 send_clock
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdusr_detach (
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_pdev_stable ( /* true if user is allowed to start up */
	CM_CHANNEL_PTR_TYPE usr_channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_rtf_is_running(
	CM_CHANNEL_PTR_TYPE sv_channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_get_im5_record (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT32  max_im5_rec_data_length,
	LSA_UINT16 * written_length,
	CM_COMMON_MEM_U8_PTR_TYPE  im5_rec_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_init (
	CM_PD_PORT_PTR_TYPE port
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_undo_init (
	CM_PD_PORT_PTR_TYPE port
);

CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_id (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id
);

CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_id__nn (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id
);

CM_PD_PORT_IF_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_if__nn (
	CM_CHANNEL_PTR_TYPE channel
);

CM_PD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_port_from_addr (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_port_is_mapped (
	CM_PD_PORT_PTR_TYPE  port
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_map_check (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  map_count,
	CM_UPPER_PD_MAP_PTR_TYPE  map_array
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_map (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  port_id,
	CM_UPPER_PD_MAP_PTR_TYPE  map
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_port_addr_unmap (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  port_id
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_link_status_changed (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT8  status,
	LSA_UINT8  prev_status
);


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_link_status_trigger_user (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  port_id
);

/*----------------------------------------------------------------------------*/

#define CM_PD_RTF_WF__START_DONE			0x0001
#define CM_PD_RTF_WF__IS_RTF				0x0002
#define CM_PD_RTF_WF__IS_NOS_IP				0x0004

#define CM_PD_RTF_WF__SV_DOWN				0x0010
#define CM_PD_RTF_WF__CL_DOWN				0x0020
#define CM_PD_RTF_WF__MC_DOWN				0x0040

#define CM_PD_RTF_WF__DO_DELAYED_RTF		0x0100

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_rtf_nos_ip_changed (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_rtf_nos_ip_changed_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  wf_done
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_reset_to_factory (
	CM_CHANNEL_PTR_TYPE usr_channel
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR		/* CM_OK | CM_OK_PENDING | CM_ERR_xxx */
cm_pd_send_clock_set (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock,
	LSA_UINT16  sendclock_prop
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_set_send_clock_done (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 edd_response,
	LSA_UINT16 send_clock
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_get (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_sc_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_sendclock_prop_rr_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
);


LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pdusr_send_clock_get (
	CM_CHANNEL_PTR_TYPE usr_channel
);


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_read(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_internal(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_queue_iterator_next(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE pd
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_read_PDIOSystemInfo (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  edd_port_id,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  *record_data_length,
	CM_UPPER_MEM_PTR_TYPE  record_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_prepare(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_begin_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_write(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_nonprm (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstanceTailorData (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_PDInstance_TailorData_trigger (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_nonprm_done (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_prm_end(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_end_user(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_write_multiple_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_RESPONSE_TYPE  response
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_appl_ready_rsp(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_alarm_rsp(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_sync_led_info(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_opc_link_status_info (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_gsy_syncid_exists (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT8  sync_id,
	LSA_BOOL  do_elog
	);
#endif

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_sc_map_response (
	LSA_UINT16			lsa_comp_id,
	LSA_UINT16			sc_response
	);

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_response_to_pnio_err (		/* if CM_OK: ret CM_PNIO_ERR_NONE  else: see CM_PNIO_ERR_MAKE() */
	LSA_RESPONSE_TYPE  rsp,			/* see cm_response_enum */
	LSA_BOOL  is_read
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_init (
	CM_CHANNEL_PTR_TYPE		channel,
	CM_PD_PRM_PTR_TYPE		pdprm
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_undo_init (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_owner_get (
	CM_PD_PRM_PTR_TYPE		pdprm
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_owner_set (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT8				new_occupant,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
	);

/* sync */ LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_prm_ownership (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT8				new_occupant,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR		/* CM_ERR_xxx:sync-error  CM_OK_PENDING:wait-for-lower-cnf */
cm_pd_prm_sc_read (
	CM_PD_PRM_PTR_TYPE		pdprm,
	CM_UPPER_RQB_PTR_TYPE	read,
	LSA_UINT16				port_id
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_sc_read_done (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				response,
	LSA_UINT32				record_data_length
	);

/**/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_set_arp (
	CM_PD_PRM_PTR_TYPE		pdprm,
	LSA_UINT16				rsp,
	LSA_UINT32				record_index,
	LSA_UINT16				wait_scbf_done
	);

/*----------------------------------------------------------------------------*/

CM_UPPER_PD_DIFF_AP_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_ap_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT32  api
);

CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_module_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT16  slot_nr,
	LSA_UINT32  mod_ident
);

CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diff_submodule_find_append (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) CM_LOCAL_MEM_ATTR  *head,
	LSA_UINT16  subslot_nr,
	LSA_UINT16  sub_state,
	LSA_UINT32  sub_ident
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diff_list_free (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *head
);

/* --- */

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_difflist_to_moddiffblock (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  ar_graph, /* may be LSA_NULL */
	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) CM_LOCAL_MEM_ATTR  *diff_list,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mod_diff_block_length,
	CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR  *mod_diff_block
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_for_appl_ready( /* AP01427283 */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *diag_block_length,
	CM_UPPER_MEM_PTR_TYPE  CM_LOCAL_MEM_ATTR  *diag_block
);


/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdsv_mapping_ok (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_ownership (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_alarm_control (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_BOOL enable,
	LSA_BOOL mark_all_as_sent, /* evaluated if enable == false */
	LSA_UINT16 max_alarm_data_length /* evaluated if enable == true */
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_alarm_rsp (
	CM_CHANNEL_PTR_TYPE sv_channel,
	CM_UPPER_RQB_PTR_TYPE alarm
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdsv_diag_maint_qual (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	LSA_UINT32 CM_LOCAL_MEM_ATTR *maint_status_ptr
);

CM_LIST_ENTRY_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdsv_diag_read (
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdsv_request(
	CM_CHANNEL_PTR_TYPE sv_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_relinquish_server (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdcl_request(
	CM_CHANNEL_PTR_TYPE cl_channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdcl_pdev_is_ready_for_schedule (
	CM_CHANNEL_PTR_TYPE  cl_channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pdcl_master_tailor_device_added (
	CM_CHANNEL_PTR_TYPE  cl_channel
);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_almi_init (
	CM_CHANNEL_PTR_TYPE	channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_provide_event (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_alarm_user_enable (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  user_enable
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_write_ptcp_trigger(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CHANNEL_PTR_TYPE gsy_channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_write_ptcp_data_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16 oha_response
);

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_sync_state_changed(
	CM_CHANNEL_PTR_TYPE channel,
	CM_CHANNEL_PTR_TYPE gsy_channel /* current values in ->usr.gsy.xxx */
);
#endif

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_irt_port_status_changed(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 port_id,
	LSA_UINT8 irt_port_status
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value
);


CM_PD_DIAG_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_add (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,

	LSA_UINT16  channel_properties,
	LSA_UINT16  channel_error_type,
	LSA_UINT16  ext_channel_error_type,
	LSA_UINT32  ext_channel_add_value,
	LSA_UINT16  alarm_type
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_remove (
	CM_LIST_ENTRY_PTR_TYPE  diag_list,
	CM_PD_DIAG_PTR_TYPE  diag_rm
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_update_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_clear (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_maint_qual (
	CM_PD_PORT_PTR_TYPE  port,
	LSA_BOOL CM_LOCAL_MEM_ATTR  *diag_status_ptr,
	LSA_UINT32 CM_LOCAL_MEM_ATTR  *maint_status_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_move_to_user_diag (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_diag_move_to_server_diag (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_diag_marshal_8000(
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_COMMON_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 CM_COMMON_MEM_ATTR *offs_ptr,
	CM_LIST_ENTRY_PTR_TYPE ext_phys_diag_list,
	LSA_UINT16 index
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_set_topo_state (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port
);


/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cmpd_prm_write (
	CM_PD_PRM_PTR_TYPE  pdprm,
	CM_PD_PORT_PTR_TYPE  port,
	LSA_UINT32  record_index,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cmpd_sendclock_changed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
);


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	EDD_UPPER_PRM_INDICATION_PTR_TYPE ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_edd_prm_link_status_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_EDD_LOWER_RQB_PTR_TYPE	edd
	);



/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_MRP

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	MRP_PRM_INDICATION_PTR_TYPE	ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mrp_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_MRP_LOWER_RQB_PTR_TYPE	mrp
	);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	GSY_RQB_PRM_INDICATION_PROVIDE_TYPE GSY_UPPER_MEM_ATTR  *ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_gsy_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	LSA_UINT8					sync_id,
	CM_GSY_LOWER_RQB_PTR_TYPE	gsy
	);

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_USE_POF

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	POF_UPPER_PRM_INDICATION_PTR_TYPE ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_pof_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_POF_LOWER_RQB_PTR_TYPE	pof
	);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_prepare_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_write_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_end_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_commit_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_indication_done (
	CM_CHANNEL_PTR_TYPE			channel,
	OHA_UPPER_PRM_INDICATION_PTR_TYPE  ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_oha_prm_read_done (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	oha
	);

/*----------------------------------------------------------------------------*/
/* PDMasterTailorData */

#define CM_PD_CONFIG_START				0x00FF
#define CM_PD_CONFIG_PRM_USR			0x0001
#define CM_PD_CONFIG_PRM_SV				0x0002
#define CM_PD_CONFIG_NOS_IP				0x0004
#define CM_PD_CONFIG_RESET_TO_FACTORY	0x0008
#define CM_PD_CONFIG_CL_DEVICE_ADD		0x0010
#define CM_PD_CONFIG_MACHINE_TAILOR		0x0020

/* starts a PDev parameterization */
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_start (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_trigger (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT32  wf_done
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_callback (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_UINT16  rsp,
	CM_UPPER_RQB_PTR_TYPE  rb
	);

/* callback */
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_config_done (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg,
	LSA_UINT16  rsp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prepare_appl_ready (
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_RESPONSE_TYPE  rsp,
	CM_UPPER_RQB_PTR_TYPE  appl_ready_ind
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_callback_appl_ready (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_RESPONSE_TYPE rsp
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_prm_mp_tailoring (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_is_masterproject (
	CM_CHANNEL_PTR_TYPE  usr_channel
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_init (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_prm_begin (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  clear_pll_data
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_alloc_records (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_clear (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_write (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  list_type,
	CM_UPPER_RQB_PTR_TYPE  rb
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_tailor (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_add_default_rec (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PRM_PTR_TYPE  pdprm,
	LSA_BOOL  is_prm_server,
	LSA_BOOL  set_mrp_off
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_reset_to_factory (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_add_ep_rec (
	CM_CHANNEL_PTR_TYPE  channel
	);

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_reclist_validate_and_prealloc_tailoring_records (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_reclist_set_error (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32 faulty_record_index
);

#define CM_PD_RECLIST_REGULAR	0	/* records for the usual or tailored parameterization */
#define CM_PD_RECLIST_EMPTY		1	/* records for the empty parameterization */
#define CM_PD_RECLIST_RTF		2	/* records for ResetToFactory parameterization*/

CM_AR_GRAPH_RECORD_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_reclist_get_next (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	LSA_UINT16  list_type
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_parse_record (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_prealloc_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec
	);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_mpa_prm_write_tailor_data (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec,
	CM_AR_GRAPH_RECORD_PTR_TYPE  rec_8052,
	CM_COMMON_MEM_U8_PTR_TYPE  nos_cl,
	LSA_UINT16  nos_cl_len
	);

LSA_UINT8  CM_LOCAL_FCT_ATTR
cm_pd_mpa_put_port_name (
	CM_COMMON_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  properties, /* 0 or CM_PD_MTD_PEER_PROP_PORT_CODING */
	LSA_UINT16  subslot_nr,
	LSA_UINT16  slot_nr
	);

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  device_nr, /* backlink */
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data
	);

CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_lookup_mtdp (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
	);

CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_mtd_lookup_mtdp_from_index (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	LSA_UINT16  index /* 0 .. (port_cnt - 1) */
	);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pd_mtd_neighbour_exists (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_pd_mtd_free (
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_MEM_ATTR  *mtd
	);

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_pd_itd_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  record_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  record_data,
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE  *_itd
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_itd_free (
	CM_PD_INSTANCE_TAILOR_DATA_PTR_TYPE CM_LOCAL_MEM_ATTR  *itd
	);

CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_lookup_mtd (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16 device_nr
	);

CM_PD_MTD_PORT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_lookup_mtdp (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16 device_nr,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  CM_LOCAL_MEM_ATTR  *mtd_
	);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_pdusr_mpa_tailor_frame_id (
	CM_CHANNEL_PTR_TYPE  usr_channel,
	LSA_UINT16  rtc3_frame_id
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_mtdp_get_peer (
	CM_PD_MTD_PORT_PTR_TYPE  mtdp,
	LSA_UINT16				 *peer_station_nr,
	LSA_UINT16				 *peer_slot_nr,
	LSA_UINT16				 *peer_subslot_nr,
	LSA_UINT16				 *properties
	);

/*----------------------------------------------------------------------------*/

CM_AR_GRAPH_RECORD_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pd_reclist_read (
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			list_type,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pd_reclist_delete ( /* TIA 1095777 */
	CM_CHANNEL_PTR_TYPE	channel,
	LSA_UINT16			slot_nr,
	LSA_UINT16			subslot_nr,
	LSA_UINT32			record_index
	);

/*----------------------------------------------------------------------------*/

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
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pd_list_record_free (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_AR_GRAPH_RECORD_PTR_TYPE	rec
	);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_IPD_H */
