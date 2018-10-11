#ifndef CM_IMC_H
#define CM_IMC_H

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
/*  F i l e               &F: cm_imc.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - multicast things                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef CM_CFG_MAX_MC_DEVICES
#error "CM_CFG_MAX_MC_DEVICES not defined"
#endif

/*----------------------------------------------------------------------------*/

/* MDST .. Multicast-Device-STate */
#define CM_MST_PASSIVE		1
#define CM_MST_WF_IN		2
#define CM_MST_IN_DATA		3
#define CM_MST_WF_DOWN		4
#define CM_MST_WF_REMOVE	5

/* MWF .. Multicast-Wait-For */

#define CM_MWF__COM_START_REQ			0x00000001
#define CM_MWF__COM_START_CNF			0x00000002
#define CM_MWF__COM_STOP_REQ			0x00000004
#define CM_MWF__COM_STOP_CNF			0x00000008

#define CM_MWF__MCR_START_REQ			0x00000010
#define CM_MWF__MCR_STOP_REQ			0x00000020
#define CM_MWF__MCR_STOP_CNF			0x00000040
#define CM_MWF__COM_UP_DOWN_IND			0x00000080

#define CM_MWF__IN_DATA_RSP				0x00000100
#define CM_MWF__OFFLINE_IND				0x00000200
#define CM_MWF__OFFLINE_RSP				0x00000400
#define CM_MWF__ADDR_INFO_GOOD_REQ		0x00000800

#define CM_MWF__ADDR_INFO_BAD_RSP		0x00001000
#define CM_MWF__ADDR_INFO_GOOD_RSP		0x00002000
#define CM_MWF__APDU_STATUS_RSP			0x00004000
#define CM_MWF__res1					0x00008000

#define CM_MWF__DO_NOT_REMOVE			0x00010000 /* obsolet */
#define CM_MWF__DO_AUTOSTOP				0x00020000
#define CM_MWF__DEVICE_REMOVE_CNF		0x00040000

#define CM_MWF__IRT_ALLOWED				0x00100000
#define CM_MWF__IRT_ADDR_INFO_DONE		0x00200000
#define CM_MWF__DO_PD_DETACH			0x00400000
#define CM_MWF__SENDCLOCK_CHANGED		0x00800000

#define CM_MWF__IS_RESET_TO_FACTORY		0x01000000 /* keep until device_remove */


#define CM_MC_WF_IS_SET(wait_for, mwf_flag)	\
	(((wait_for) & (mwf_flag)) != 0)

#define CM_MC_WF_SET(wait_for, mwf_flag)	\
	{ CM_ASSERT(!CM_MC_WF_IS_SET(wait_for, mwf_flag)); wait_for |= mwf_flag; }

#define CM_MC_WF_RESET(wait_for, mwf_flag)	\
	{ CM_ASSERT( CM_MC_WF_IS_SET(wait_for, mwf_flag)); wait_for &= ~(mwf_flag); }

#define CM_MC_WF_CLEAR(wait_for, mwf_flag)	\
	{ CM_ASSERT( CM_MC_WF_IS_SET(wait_for, mwf_flag)); wait_for &= ~(mwf_flag); }

/*----------------------------------------------------------------------------*/

typedef struct cm_mc_device_link_tag {
	struct cm_mc_device_tag CM_COMMON_MEM_ATTR * Flink; /* forward link */
	struct cm_mc_device_tag CM_COMMON_MEM_ATTR * Blink; /* backward link */
} CM_MC_DEVICE_LINK_TYPE;


typedef struct cm_mc_device_tag {
	CM_MC_DEVICE_LINK_TYPE	link;
	CM_CHANNEL_PTR_TYPE		channel; /* backlink to channel */

	LSA_UINT16				device_nr; /* key of this entry, given by add */
	LSA_UINT8				ar_nr; /* key of this entry, given by add */
	LSA_UINT8				state; /* state of device, see enum CM_MC_DEV_STATE_TYPE */

	LSA_UINT32				wait_for; /* flag wf cm-user action / abort actions / lower cnf */

	CM_AR_GRAPH_PTR_TYPE	ar_graph;

	CM_UPPER_RQB_PTR_TYPE	upper_add_rm; /* current upper rqb for: CM_OPC_MC_DEVICE_ADD, CM_OPC_MC_DEVICE_REMOVE */
	CM_UPPER_RQB_PTR_TYPE	upper_ctrl; /* current upper rqb for: CM_CL_DEVICE_CONTROL_ACTIVATE / CM_CL_DEVICE_CONTROL_PASSIVATE */
	CM_UPPER_RQB_PTR_TYPE	upper_apdu; /* current upper rqb for: CM_MC_DEVICE_CONTROL_APDU_STATUS */

	struct cm_mc_dev_addr_info_tag {
		LSA_UINT8				info_class; /* see cm_addr_info_class_enum */
		LSA_UINT8				info_phase; /* see cm_addr_info_phase_enum */
		LSA_UINT16				reason_code; /* see cm_addr_info_reason_code_enum */
		LSA_UINT16				lower_rsp; /* see NARE_RSP_xxx, CLRPC_ERR_xxx */
		LSA_UINT32				pnio_status; /* see part 6 */
		/* mac and ip is stored in ar_graph.ar */
		LSA_UINT32				subnet_mask;
		LSA_UINT32				gateway_ip;
	}						addr_info;

} CM_MC_DEVICE_TYPE;

typedef CM_MC_DEVICE_TYPE CM_LOCAL_MEM_ATTR * CM_MC_DEVICE_PTR_TYPE;


/*----------------------------------------------------------------------------*/

/* channel-wait-for */
#define CM_MC_CWF__RESET_TO_FACTORY_CNF		0x0001

typedef struct cm_mc_channel_tag {
	CM_UPPER_RQB_PTR_TYPE	upper;			/* current upper create / delete rqb */
	CM_UPPER_RQB_PTR_TYPE	upper_apdu;		/* current clear / operate rqb */

		/* anchor of devices */
	CM_MC_DEVICE_LINK_TYPE  CM_LOCAL_MEM_ATTR
							* dev;			/* dynamic dev_list, see CM_OPC_MC_CREATE */
	LSA_UINT16				dev_cnt;		/* CM_OPC_MC_CREATE::max_devices */

		/* pd */
	LSA_UINT16				pd_send_clock;	/* 0..pending, >0..set */
	LSA_UINT16				wait_for;		/* channel-wait-for */

} CM_MC_CHANNEL_TYPE;


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_channel_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mc_channel_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_request (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_create (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_delete (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MC_DEVICE_PTR_TYPE  dev
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_control_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MC_DEVICE_PTR_TYPE  dev
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MC_DEVICE_PTR_TYPE  dev
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_addr_info_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_in_data_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_offline_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_apdu_status_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mc_acp_global_provider_control (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  apdu_tag
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_acp_global_provider_control_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*---------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_link_status_changed(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_up,
	LSA_BOOL is_transient /* LSA_TRUE: the "down" is transient (some 100ms) */
);

/*--ar_com-statemachine-------------------------------------------------------*/
/* callback-functions */

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_init (
	CM_AR_GRAPH_PTR_TYPE  argr
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_undo_init (
	CM_AR_GRAPH_PTR_TYPE argr
	);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  arcq_para1
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_start_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_reset_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mc_mcons_nare_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE io_cr,
	LSA_UINT16 nare_rsp,
	CM_MAC_PTR_TYPE source_mac,
	LSA_UINT32 source_ip,
	LSA_UINT32 subnet_mask,
	LSA_UINT32 gateway_ip
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_mcr_up(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_mcr_down(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT16  old_state,
	LSA_UINT8 reason
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_mcr_apdu_status_check(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_down (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_mcr_stop_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_stop_frames_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_PTR_TYPE argr
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_apdu_status_control_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mc_ar_com_apdu_status_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  apdu_delta
);

/*----------------------------------------------------------------------------*/

CM_MC_DEVICE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_mc_dev_from_device_ar (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_sm (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MC_DEVICE_PTR_TYPE  dev
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_device_down (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MC_DEVICE_PTR_TYPE  dev,
	LSA_UINT8   reason_code,
	LSA_UINT16  lower_rsp
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mc_ar_need_local_irt (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_sendclock_changed(
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  send_clock
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mc_pdev_changed(
	CM_CHANNEL_PTR_TYPE  channel
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mc_pdev_set_down_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mc_pdev_set_down_check (
	CM_CHANNEL_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_IMC_H */
