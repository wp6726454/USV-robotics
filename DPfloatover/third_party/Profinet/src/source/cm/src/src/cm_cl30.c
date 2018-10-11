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
/*  F i l e               &F: cm_cl30.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  client function for rpc / connect / release / control / lower_done       */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	630
#define CM_MODULE_ID		630

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_AR_ar_type(u16_)			cm_put_u16  (dp, CM_REQ_AR, ar_type, (u16_))
#define cm_put_REQ_AR_ar_uuid(uuidp_)		cm_put_uuid (dp, CM_REQ_AR, ar_uuid, (uuidp_))
#define cm_put_REQ_AR_session_key(u16_)		cm_put_u16  (dp, CM_REQ_AR, session_key, (u16_))
#define cm_put_REQ_AR_cmi_mac(mac_)			cm_put_mac  (dp, CM_REQ_AR, cmi_mac, (mac_))
#define cm_put_REQ_AR_cmi_obj_uuid(uuidp_)	cm_put_uuid (dp, CM_REQ_AR, cmi_obj_uuid, (uuidp_))
#define cm_put_REQ_AR_ar_properties(u32_)	cm_put_u32  (dp, CM_REQ_AR, ar_properties, (u32_))
#define cm_put_REQ_AR_cmi_activity_timeout100ms(u16_) \
											cm_put_u16  (dp, CM_REQ_AR, cmi_activity_timeout100ms, (u16_))
#define cm_put_REQ_AR_udp_cmi_port(u16_)	cm_put_u16  (dp, CM_REQ_AR, cmi_udp_port, (u16_))
#define cm_put_REQ_AR_cmi_name_length(u16_)	cm_put_u16  (dp, CM_REQ_AR, cmi_name_length, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_get_RSP_AR_ar_type(u16p_)		cm_get_u16  (u16p_,  dp, CM_RSP_AR, ar_type)
#define cm_get_RSP_AR_ar_uuid(uuidp_)		cm_get_uuid (uuidp_, dp, CM_RSP_AR, ar_uuid)
#define cm_get_RSP_AR_session_key(u16p_)	cm_get_u16  (u16p_,  dp, CM_RSP_AR, session_key)
#define cm_get_RSP_AR_cmr_mac(macp_)		cm_get_mac  (macp_,  dp, CM_RSP_AR, cmr_mac)
#define cm_get_RSP_AR_udp_cmr_port(u16p_)	cm_get_u16  (u16p_,  dp, CM_RSP_AR, cmr_udp_port)

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_IOCR1_iocr_type(u16_)		cm_put_u16  (dp, CM_REQ_IOCR1, iocr_type, (u16_))
#define cm_put_REQ_IOCR1_iocr_ref(u16_)			cm_put_u16  (dp, CM_REQ_IOCR1, iocr_ref, (u16_))
#define cm_put_REQ_IOCR1_ethertype(u16_)		cm_put_u16  (dp, CM_REQ_IOCR1, ethertype, (u16_))
#define cm_put_REQ_IOCR1_iocr_properties(u32_)	cm_put_u32  (dp, CM_REQ_IOCR1, iocr_properties, (u32_))
#define cm_put_REQ_IOCR1_data_length(u16_)		cm_put_u16  (dp, CM_REQ_IOCR1, data_length, (u16_))
#define cm_put_REQ_IOCR1_frame_id(u16_)			cm_put_u16  (dp, CM_REQ_IOCR1, frame_id, (u16_))
#define cm_put_REQ_IOCR1_send_clock(u16_)		cm_put_u16  (dp, CM_REQ_IOCR1, send_clock, (u16_))
#define cm_put_REQ_IOCR1_reduction_ratio(u16_)	cm_put_u16  (dp, CM_REQ_IOCR1, reduction_ratio, (u16_))
#define cm_put_REQ_IOCR1_phase(u16_)			cm_put_u16  (dp, CM_REQ_IOCR1, phase, (u16_))
#define cm_put_REQ_IOCR1_sequence(u16_)			cm_put_u16  (dp, CM_REQ_IOCR1, sequence, (u16_))
#define cm_put_REQ_IOCR1_frame_send_offset(u32_) \
												cm_put_u32  (dp, CM_REQ_IOCR1, frame_send_offset, (u32_))
#define cm_put_REQ_IOCR1_watchdog_factor(u16_)	cm_put_u16  (dp, CM_REQ_IOCR1, watchdog_factor_legacy, (u16_))
#define cm_put_REQ_IOCR1_data_hold_factor(u16_)	cm_put_u16  (dp, CM_REQ_IOCR1, data_hold_factor, (u16_))
#define cm_put_REQ_IOCR1_iocr_tag_header(u16_)	cm_put_u16  (dp, CM_REQ_IOCR1, iocr_tag_header, (u16_))
#define cm_put_REQ_IOCR1_iocr_multicast_mac(mac_) \
												cm_put_mac  (dp, CM_REQ_IOCR1, iocr_multicast_mac, (mac_))
#define cm_put_REQ_IOCR1_number_of_related_apis(u16_) \
												cm_put_u16  (dp, CM_REQ_IOCR1, number_of_related_apis, (u16_))

#define cm_put_REQ_IOCR2_api(u32_)				cm_put_u32  (dp, CM_REQ_IOCR2, api, (u32_))
#define cm_put_REQ_IOCR2_number_of_releated_iodata_objects(u16_) \
												cm_put_u16  (dp, CM_REQ_IOCR2, number_of_releated_iodata_objects, (u16_))


#define cm_put_REQ_IOCR3_slot_nr(u16_)			cm_put_u16  (dp, CM_REQ_IOCR3, slot_nr, (u16_))
#define cm_put_REQ_IOCR3_subslot_nr(u16_)		cm_put_u16  (dp, CM_REQ_IOCR3, subslot_nr, (u16_))
#define cm_put_REQ_IOCR3_frame_offset(u16_)		cm_put_u16  (dp, CM_REQ_IOCR3, frame_offset, (u16_))


#define cm_put_REQ_IOCR4_number_of_releated_iocs(u16_) \
												cm_put_u16  (dp, CM_REQ_IOCR4, number_of_releated_iocs, (u16_))


#define cm_put_REQ_IOCR5_slot_nr(u16_)			cm_put_u16  (dp, CM_REQ_IOCR5, slot_nr, (u16_))
#define cm_put_REQ_IOCR5_subslot_nr(u16_)		cm_put_u16  (dp, CM_REQ_IOCR5, subslot_nr, (u16_))
#define cm_put_REQ_IOCR5_frame_offset(u16_)		cm_put_u16  (dp, CM_REQ_IOCR5, frame_offset, (u16_))


/*----------------------------------------------------------------------------*/

#define cm_get_RSP_IOCR_iocr_type(u16p_)		cm_get_u16  (u16p_, dp, CM_RSP_IOCR, iocr_type)
#define cm_get_RSP_IOCR_iocr_ref(u16p_)			cm_get_u16  (u16p_, dp, CM_RSP_IOCR, iocr_ref)
#define cm_get_RSP_IOCR_frame_id(u16p_)			cm_get_u16  (u16p_, dp, CM_RSP_IOCR, frame_id)

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_MCR_iocr_ref(u16_)				cm_put_u16  (dp, CM_REQ_MCR, iocr_ref, (u16_))
#define cm_put_REQ_MCR_mcr_properties(u32_)			cm_put_u32  (dp, CM_REQ_MCR, mcr_properties, (u32_))
#define cm_put_REQ_MCR_mci_timeout_factor(u16_)		cm_put_u16  (dp, CM_REQ_MCR, mci_timeout_factor, (u16_))
#define cm_put_REQ_MCR_station_name_length(u16_)	cm_put_u16  (dp, CM_REQ_MCR, station_name_length, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_API_number_of_related_apis(u16_) \
												cm_put_u16  (dp, CM_REQ_API, number_of_related_apis, (u16_))
#define cm_put_REQ_MOD_api(u32_)				cm_put_u32  (dp, CM_REQ_MOD, api, (u32_))
#define cm_put_REQ_MOD_slot_nr(u16_)			cm_put_u16  (dp, CM_REQ_MOD, slot_nr, (u16_))
#define cm_put_REQ_MOD_mod_ident(u32_)			cm_put_u32  (dp, CM_REQ_MOD, mod_ident, (u32_))
#define cm_put_REQ_MOD_mod_properties(u16_)		cm_put_u16  (dp, CM_REQ_MOD, mod_properties, (u16_))
#define cm_put_REQ_MOD_number_of_submodules(u16_) \
												cm_put_u16  (dp, CM_REQ_MOD, number_of_submodules, (u16_))

#define cm_put_REQ_SUB_subslot_nr(u16_)			cm_put_u16  (dp, CM_REQ_SUB, subslot_nr, (u16_))
#define cm_put_REQ_SUB_sub_ident(u32_)			cm_put_u32  (dp, CM_REQ_SUB, sub_ident, (u32_))
#define cm_put_REQ_SUB_sub_properties(u16_)		cm_put_u16  (dp, CM_REQ_SUB, sub_properties, (u16_))

#define cm_put_REQ_DSC_data_description(u16_)	cm_put_u16  (dp, CM_REQ_DSC, data_description, (u16_))
#define cm_put_REQ_DSC_data_length(u16_)		cm_put_u16  (dp, CM_REQ_DSC, data_length, (u16_))
#define cm_put_REQ_DSC_iops_length(u8_)			cm_put_u8   (dp, CM_REQ_DSC, iops_length, (u8_))
#define cm_put_REQ_DSC_iocs_length(u8_)			cm_put_u8   (dp, CM_REQ_DSC, iocs_length, (u8_))

/*----------------------------------------------------------------------------*/

#define cm_get_RSP_DIFF_number_of_apis(u16p_) \
												cm_get_u16  (u16p_, dp, CM_RSP_DIFF, number_of_apis)
#define cm_get_RSP_API_api(u32p_)				cm_get_u32  (u32p_, dp, CM_RSP_API, api)
#define cm_get_RSP_API_number_of_modules(u16p_) \
												cm_get_u16  (u16p_, dp, CM_RSP_API, number_of_modules)

#define cm_get_RSP_MOD_slot_nr(u16p_)			cm_get_u16  (u16p_, dp, CM_RSP_MOD, slot_nr)
#define cm_get_RSP_MOD_mod_ident(u32p_)			cm_get_u32  (u32p_, dp, CM_RSP_MOD, mod_ident)
#define cm_get_RSP_MOD_mod_state(u16p_)			cm_get_u16  (u16p_, dp, CM_RSP_MOD, mod_state)
#define cm_get_RSP_MOD_number_of_submodules(u16p_) \
												cm_get_u16  (u16p_, dp, CM_RSP_MOD, number_of_submodules)

#define cm_get_RSP_SUB_subslot_nr(u16p_)		cm_get_u16  (u16p_, dp, CM_RSP_SUB, subslot_nr)
#define cm_get_RSP_SUB_sub_ident(u32p_)			cm_get_u32  (u32p_, dp, CM_RSP_SUB, sub_ident)
#define cm_get_RSP_SUB_sub_state(u16p_)			cm_get_u16  (u16p_, dp, CM_RSP_SUB, sub_state)

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_ALCR_alcr_type(u16_)				cm_put_u16  (dp, CM_REQ_ALCR, alcr_type, (u16_))
#define cm_put_REQ_ALCR_ethertype(u16_)				cm_put_u16  (dp, CM_REQ_ALCR, ethertype, (u16_))
#define cm_put_REQ_ALCR_alcr_properties(u32_)		cm_put_u32  (dp, CM_REQ_ALCR, alcr_properties, (u32_))
#define cm_put_REQ_ALCR_timeout100ms(u16_)			cm_put_u16  (dp, CM_REQ_ALCR, timeout100ms, (u16_))
#define cm_put_REQ_ALCR_retries(u16_)				cm_put_u16  (dp, CM_REQ_ALCR, retries, (u16_))
#define cm_put_REQ_ALCR_local_alarm_ref(u16_)		cm_put_u16  (dp, CM_REQ_ALCR, local_alarm_ref, (u16_))
#define cm_put_REQ_ALCR_max_data_length(u16_)		cm_put_u16  (dp, CM_REQ_ALCR, max_alarm_data_length, (u16_))
#define cm_put_REQ_ALCR_alcr_tag_header_high(u16_)	cm_put_u16  (dp, CM_REQ_ALCR, alcr_tag_header_high, (u16_))
#define cm_put_REQ_ALCR_alcr_tag_header_low(u16_)	cm_put_u16  (dp, CM_REQ_ALCR, alcr_tag_header_low, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_get_RSP_ALCR_alcr_type(u16p_)				cm_get_u16 (u16p_, dp, CM_RSP_ALCR, alcr_type)
#define cm_get_RSP_ALCR_local_alarm_ref(u16p_)			cm_get_u16 (u16p_, dp, CM_RSP_ALCR, local_alarm_ref)
#define cm_get_RSP_ALCR_max_alarm_data_length(u16p_)	cm_get_u16 (u16p_, dp, CM_RSP_ALCR, max_alarm_data_length)

/*----------------------------------------------------------------------------*/

#define cm_get_RSP_ARSERVER_cmr_name_length(u16p_)	cm_get_u16 (u16p_, dp, CM_RSP_ARSERVER, cmr_name_length)

/*----------------------------------------------------------------------------*/

#define cm_put_BLK_CTRL_reserved1(u16_)				cm_put_u16  (dp, CM_BLK_CTRL, reserved1, (u16_))
#define cm_put_BLK_CTRL_ar_uuid(uuidp_)				cm_put_uuid (dp, CM_BLK_CTRL, ar_uuid, (uuidp_))
#define cm_put_BLK_CTRL_session_key(u16_)			cm_put_u16  (dp, CM_BLK_CTRL, session_key, (u16_))
#define cm_put_BLK_CTRL_alarm_sequence_number(u16_)	cm_put_u16  (dp, CM_BLK_CTRL, reserved2_alarm_sequence_number, (u16_))
#define cm_put_BLK_CTRL_control_command(u16_)		cm_put_u16  (dp, CM_BLK_CTRL, control_command, (u16_))
#define cm_put_BLK_CTRL_control_properties(u16_)	cm_put_u16  (dp, CM_BLK_CTRL, control_properties, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_get_BLK_CTRL_ar_uuid(uuidp_)				cm_get_uuid (uuidp_, dp, CM_BLK_CTRL, ar_uuid)
#define cm_get_BLK_CTRL_session_key(u16p_)			cm_get_u16  (u16p_,  dp, CM_BLK_CTRL, session_key)
#define cm_get_BLK_CTRL_alarm_sequence_number(u16p_) \
													cm_get_u16  (u16p_,  dp, CM_BLK_CTRL, reserved2_alarm_sequence_number)
#define cm_get_BLK_CTRL_control_command(u16p_)		cm_get_u16  (u16p_,  dp, CM_BLK_CTRL, control_command)
#define cm_get_BLK_CTRL_control_properties(u16p_)	cm_get_u16  (u16p_,  dp, CM_BLK_CTRL, control_properties)

/*----------------------------------------------------------------------------*/

#define cm_put_BLK_REL_reserved1(u16_)				cm_put_u16  (dp, CM_BLK_REL, reserved1, (u16_))
#define cm_put_BLK_REL_ar_uuid(uuidp_)				cm_put_uuid (dp, CM_BLK_REL, ar_uuid, (uuidp_))
#define cm_put_BLK_REL_session_key(u16_)			cm_put_u16  (dp, CM_BLK_REL, session_key, (u16_))
#define cm_put_BLK_REL_reserved2(u16_)				cm_put_u16  (dp, CM_BLK_REL, reserved2_alarm_sequence_number, (u16_))
#define cm_put_BLK_REL_release_command(u16_)		cm_put_u16  (dp, CM_BLK_REL, control_command, (u16_))
#define cm_put_BLK_REL_release_properties(u16_)		cm_put_u16  (dp, CM_BLK_REL, control_properties, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_IR_padding1(u16_)		cm_put_u16  (dp, CM_REQ_IRINFO, padding1, (u16_))
#define cm_put_REQ_IR_ir_data_uuid(uuidp_)	cm_put_uuid (dp, CM_REQ_IRINFO, ir_data_uuid, (uuidp_))
#define cm_put_REQ_IR_padding2(u16_)		cm_put_u16  (dp, CM_REQ_IRINFO, padding2, (u16_))
#define cm_put_REQ_IR_number_of_iocrs(u16_)	cm_put_u16  (dp, CM_REQ_IRINFO, number_of_iocrs, (u16_))

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_IRISUB_iocr_reference(u16_)		cm_put_u16 (dp, CM_REQ_IRINFOSUB, iocr_reference, (u16_))
#define cm_put_REQ_IRISUB_subframe_offset(u16_)		cm_put_u16 (dp, CM_REQ_IRINFOSUB, subframe_offset, (u16_))
#define cm_put_REQ_IRISUB_subframe_data(u32_)		cm_put_u32 (dp, CM_REQ_IRINFOSUB, subframe_data, (u32_))

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_SRD_RedundancyDataHoldFactor(u16_)	cm_put_u16  (dp, CM_REQ_SRINFO, RedundancyDataHoldFactor, (u16_))
#define cm_put_REQ_SRD_SRProperties(u32_)				cm_put_u32  (dp, CM_REQ_SRINFO, SRProperties, (u32_))

/*----------------------------------------------------------------------------*/

#define cm_put_REQ_ARV_APStructureIdentifier(u16_)		cm_put_u16  (dp, CM_REQ_ARVENDOR, APStructureIdentifier, (u16_))
#define cm_put_REQ_ARV_API(u32_)						cm_put_u32  (dp, CM_REQ_ARVENDOR, API, (u32_))
/*----------------------------------------------------------------------------*/

#define cm_get_RSP_ARV_APStructureIdentifier(u16p_)		cm_get_u16  (u16p_, dp, CM_RSP_ARVENDOR, APStructureIdentifier)
#define cm_get_RSP_ARV_API(u32p_)						cm_get_u32  (u32p_, dp, CM_RSP_ARVENDOR, API)

/*----------------------------------------------------------------------------*/

#define cm_cl_calc_padding_length(len_)		CM_CAST_U16((4 - ((len_) % 4)) % 4)

/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_bind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	);

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_to_addr_info (
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT16  error_code,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *lower_rsp,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *error_cause,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  *pnio_status,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  *nca_status
	)
{
	CM_LOG_FCT ("cm_cl_rpc_to_addr_info")
	CLRPC_UPPER_CL_CALL_PTR_TYPE  call = rpc->args.cl.call;

	*lower_rsp = CM_RPC_GET_RESPONSE (rpc);
	*pnio_status = CM_PNIO_ERR_NONE;
	*nca_status = 0;

	switch (*lower_rsp)
	{
	case CLRPC_OK:
		switch (call->pnio_status)
		{
		case CM_PNIO_ERR_NONE:
			*error_cause = CM_ERROR_CAUSE_NONE;
			*pnio_status = CM_PNIO_ERR_NONE; /* good */
			break;

		default:
			if (error_code == CM_PNIO_ERR_CODE_CONNECT
			&&  call->pnio_status == CM_PNIO_FAULTY_ConnectBlockReq (64/*RMPM*/, 4/*Out of AR Resources*/))
			{
				*error_cause = CM_ERROR_CAUSE_OUT_OF_RESOURCES;
			}
			else
			{
				*error_cause = CM_ERROR_CAUSE_PNIO_CONFIGURATION;
			}
			*pnio_status = call->pnio_status;
			break;
		}
		break;

	case CLRPC_OK_CANCELLED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_APPLICATION_TIMEOUT);
		break;

	case CLRPC_ERR_REJECTED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_REJECTED);
		*nca_status  = call->nca_status;
		break;

	case CLRPC_ERR_FAULTED:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_FAULTED);
		*nca_status  = call->nca_status;
		break;

	case CLRPC_ERR_TIMEOUT:
		*error_cause = CM_ERROR_CAUSE_RPC_TIMEOUT;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_TIMEOUT);
		break;

	case CLRPC_ERR_IN_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_IN_ARGS);
		break;

	case CLRPC_ERR_OUT_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_OUT_ARGS);
		break;

	case CLRPC_ERR_DECODE:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_DECODE);
		break;

	case CLRPC_ERR_PNIO_OUT_ARGS:
		*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
		*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_PNIO_OUT_ARGS);
		break;

	case CLRPC_ERR_OPCODE:
		CM_FATAL1(CM_RPC_GET_OPCODE(rpc));
		return;

	default:
		if (call->ndr_len < CLRPC_PNIO_OFFSET)
		{
			*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
			*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, CM_PNIO_RPC_CODE2_PNIO_OUT_ARGS);
		}
		else
		{
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check: is this a possible case? if yes, do better code2")
#endif /* CM_MESSAGE */
			*error_cause = CM_ERROR_CAUSE_RPC_ERROR;
			*pnio_status = CM_PNIO_FAULTY_RPC_CALL (error_code, 0xFF);
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_mod_state_check (
	LSA_UINT16  mod_state
	)
{
	CM_LOG_FCT ("cm_mod_state_check")

	switch (mod_state) {
	case CM_MOD_STATE_NO_MODULE:
	case CM_MOD_STATE_WRONG_MODULE:
	case CM_MOD_STATE_PROPER_MODULE:
	case CM_MOD_STATE_SUBSTITUTED_MODULE:
		return LSA_TRUE;

	default:
		CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP, "unknown mod_state(%u)", mod_state);
		return LSA_FALSE;
	}
}
/*----------------------------------------------------------------------------*/
static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_sub_state_check (
	LSA_UINT16  mod_state,
	LSA_UINT16  sub_state
	)
{
	CM_LOG_FCT ("cm_sub_state_check")

	if ((sub_state & CM_SUB_STATE_FORMAT_MASK) != CM_SUB_STATE_FORMAT_1) {
		goto wrong_format;
	}

	switch (sub_state & CM_SUB_STATE_ADD_INFO_MASK) {
	case CM_SUB_STATE_ADD_INFO_NONE:					break;
	case CM_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED:	break;
	default:
		/* CM_SUB_STATE_ADD_INFO_MASK: not checked (61158.v23) ??? */
		goto wrong_format;
	}

	/* CM_SUB_STATE_QUALIFIED_INFO_MASK: all values valid */

	/* CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK: all values valid */

	/* CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK: all values valid */

	/* CM_SUB_STATE_DIAG_INFO_MASK: all values valid */

	switch (sub_state & CM_SUB_STATE_AR_INFO_MASK) {
	case CM_SUB_STATE_AR_INFO_OWNED:					break;
	case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:		break;
	case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED:	break;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:		break;
	case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:		break;
	default:
		goto wrong_format;
	}

	switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
	case CM_SUB_STATE_IDENT_OK:							break;
	case CM_SUB_STATE_IDENT_SUBSTITUTE:					break;
	case CM_SUB_STATE_IDENT_WRONG:						break;
	case CM_SUB_STATE_IDENT_NO_SUBMODULE:				break;
	default:
		goto wrong_format;
	}

	/***/

	switch (mod_state) {
	case CM_MOD_STATE_NO_MODULE:
		switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
		case CM_SUB_STATE_IDENT_NO_SUBMODULE:			break;/*AP00350631*/

		case CM_SUB_STATE_IDENT_OK:
		case CM_SUB_STATE_IDENT_SUBSTITUTE:
		case CM_SUB_STATE_IDENT_WRONG:
		default:
			goto wrong_format;
		}
		break;

	case CM_MOD_STATE_WRONG_MODULE:
		switch (sub_state & CM_SUB_STATE_IDENT_MASK) {
		case CM_SUB_STATE_IDENT_WRONG:					break;/*AP00841926*/
		case CM_SUB_STATE_IDENT_NO_SUBMODULE:			break;/*AP00841926*/

		case CM_SUB_STATE_IDENT_OK:
		case CM_SUB_STATE_IDENT_SUBSTITUTE:
		default:
			goto wrong_format;
		}
		break;

	case CM_MOD_STATE_PROPER_MODULE:
	case CM_MOD_STATE_SUBSTITUTED_MODULE:
		break;

	default:
		goto wrong_format;
	}

	return LSA_TRUE;

wrong_format:
	CM_CL_TRACE_02 (0, LSA_TRACE_LEVEL_UNEXP
		, "sub_state(0x%x) with wrong format / used mod_state(0x%x)", sub_state, mod_state
		);
	return LSA_FALSE;
}
/*----------------------------------------------------------------------------*/
/* AP00219434 */
static LSA_UINT8  CM_LOCAL_FCT_ATTR
cm_cl_guess_dev_type (
	CM_RPC_MEM_U8_PTR_TYPE  dp,
	LSA_UINT32  left
	)
{
	CM_LOG_FCT ("cm_cl_guess_dev_type")
	LSA_UINT16  api_cnt;
	LSA_UINT16  cm_get_assert_line = 0;
	LSA_UINT16  cm_get_assert_error;

	cm_get_assert (left >= sizeof (CM_RSP_DIFF));

	cm_get_RSP_DIFF_number_of_apis (& api_cnt);

	cm_adjust (dp, left, sizeof (CM_RSP_DIFF));

	while (api_cnt > 0) {
		LSA_UINT32  api;
		LSA_UINT16  mod_cnt;

		cm_get_assert (left >= sizeof (CM_RSP_API));

		cm_get_RSP_API_api (& api);
		cm_get_RSP_API_number_of_modules (& mod_cnt);

		LSA_UNUSED_ARG (api);

		cm_adjust (dp, left, sizeof (CM_RSP_API));

		while (mod_cnt > 0) {
			LSA_UINT16  slot_nr, mod_state, sub_cnt;
			LSA_UINT32  mod_ident;

			cm_get_assert (left >= sizeof (CM_RSP_MOD));

			cm_get_RSP_MOD_slot_nr (& slot_nr);
			cm_get_RSP_MOD_mod_ident (& mod_ident);
			cm_get_RSP_MOD_mod_state (& mod_state);
			cm_get_RSP_MOD_number_of_submodules (& sub_cnt);

			LSA_UNUSED_ARG (slot_nr);
			LSA_UNUSED_ARG (mod_ident);
			LSA_UNUSED_ARG (mod_state);

			cm_adjust (dp, left, sizeof (CM_RSP_MOD));

			if (sub_cnt > 0) {
				LSA_UINT16  subslot_nr, sub_state;
				LSA_UINT32  sub_ident;

				cm_get_assert (left >= sizeof (CM_RSP_SUB));

				cm_get_RSP_SUB_subslot_nr (& subslot_nr);
				cm_get_RSP_SUB_sub_ident (& sub_ident);
				cm_get_RSP_SUB_sub_state (& sub_state);

				LSA_UNUSED_ARG (subslot_nr);
				LSA_UNUSED_ARG (sub_ident);

				cm_adjust (dp, left, sizeof (CM_RSP_SUB));

				switch (sub_state & CM_SUB_STATE_FORMAT_MASK) {
				case CM_SUB_STATE_FORMAT_0:
					CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_CHAT
						, "subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x) => CM_CL_DEV_TYPE_V2"
						, subslot_nr, sub_ident, sub_state
						);
					return CM_CL_DEV_TYPE_V2;

				case CM_SUB_STATE_FORMAT_1:
					CM_CL_TRACE_03 (0, LSA_TRACE_LEVEL_CHAT
						, "subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x) => CM_CL_DEV_TYPE_V3"
						, subslot_nr, sub_ident, sub_state
						);
					return CM_CL_DEV_TYPE_V3;

				default:
					CM_FATAL1 (sub_state);
					break;
				}
			}
			mod_cnt -= 1;
		}
		api_cnt -= 1;
	}
	return CM_CL_DEV_TYPE_UNKNOWN;

cm_get_assert_failed:

	CM_CL_TRACE_01 (0, LSA_TRACE_LEVEL_UNEXP
		, "CM_CL_DEV_TYPE_UNKNWON or assertion in line(%u)"
		, cm_get_assert_line
		);
	LSA_UNUSED_ARG (cm_get_assert_line);
	LSA_UNUSED_ARG (cm_get_assert_error);

	return CM_CL_DEV_TYPE_UNKNOWN;
}
/*----------------------------------------------------------------------------*/
/* AP00219434 */
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_set_ist_to_proper (
	CM_AR_GRAPH_PTR_TYPE	argr
	)
{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;

	for (CmListForeach(ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {
		CM_AR_GRAPH_MODULE_PTR_TYPE  mod;

		for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE)) {
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

			mod->ist.mod_ident = mod->soll.mod_ident;

			for (CmListForeach(sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {
				sub->ist.sub_state = CM_SUB_STATE_GOOD; /* default */
				sub->ist.sub_ident = sub->soll.sub_ident;
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16 CM_LOCAL_FCT_ATTR
cm_cl_marshal_connect (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_MEM_U8_PTR_TYPE  data,
	LSA_UINT16  data_max,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_marshal_connect")
	CM_RPC_MEM_U8_PTR_TYPE  dp, dp_len;
	LSA_UINT16  len_cnt;
	LSA_UINT32  left;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioap;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;
	LSA_UINT16  cm_put_assert_line;

	LSA_UINT16  cmi_station_name_length;
	CM_UPPER_MEM_U8_PTR_TYPE  cmi_station_name;
	CM_MAC_TYPE  cmi_mac_addr;
	LSA_UINT16  cmi_max_alarm_data_length;

	CM_ASSERT (is_not_null (argr));

	if ( ! cm_oha_get_name_of_station (channel, & cmi_station_name_length, & cmi_station_name))
	{
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "name_of_station not valid, changed in meantime?");
		return 0;
	}

	cmi_mac_addr				= *((CM_MAC_PTR_TYPE)& channel->lower.info.acp.eth_mac_addr);
	cmi_max_alarm_data_length	= channel->acp.max_alarm_data_length; /* alcr->max_alarm_data_len will store response */

	dp = data;
	left = data_max;

	/* 	see cm_arcb.h#IODConnectReq */

	/* --- ARBlockReq --- */
	cm_put_assert (left >= sizeof (CM_BLK_HEADER));

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_AR_REQ);
	cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_REQ_AR) + cmi_station_name_length));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


	cm_put_assert (left >= sizeof (CM_REQ_AR));

	cm_put_REQ_AR_ar_type			(argr->ar.ar_type);
	cm_put_REQ_AR_ar_uuid			(& argr->ar.ar_uuid);
	cm_put_REQ_AR_session_key		(argr->cm.cl.connect_counter);
	cm_put_REQ_AR_cmi_mac			(& cmi_mac_addr);
	cm_put_REQ_AR_cmi_obj_uuid		(& argr->ar.cmi_obj_uuid);
	cm_put_REQ_AR_ar_properties		(argr->ar.ar_properties);
	cm_put_REQ_AR_cmi_activity_timeout100ms
									(argr->ar.cmi_timeout100ms);
	cm_put_REQ_AR_udp_cmi_port		(CM_NTOH16(argr->ar.cmi_udp_port));
	cm_put_REQ_AR_cmi_name_length	(cmi_station_name_length);

	cm_adjust (dp, left, sizeof (CM_REQ_AR));


	cm_put_assert (left >= cmi_station_name_length);
	CM_MEMCPY (dp, cmi_station_name, cmi_station_name_length);
	cm_adjust (dp, left, cmi_station_name_length);


	/* --- [IOCRBlockReq*] --- */
	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		dp_len = dp; /* keep for block length calculation */

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_IOCR_REQ);
		cm_put_BLOCK_HEADER_length	(0);
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


		/* IOCR 1 */
		cm_put_assert (left >= sizeof (CM_REQ_IOCR1));

		cm_put_REQ_IOCR1_iocr_type			(iocr->iocr_type);
		cm_put_REQ_IOCR1_iocr_ref			(iocr->iocr_ref);
		cm_put_REQ_IOCR1_ethertype			(iocr->ethertype);
		cm_put_REQ_IOCR1_iocr_properties	(iocr->iocr_properties);

		cm_put_REQ_IOCR1_data_length		(iocr->data_length);
		cm_put_REQ_IOCR1_frame_id			(iocr->frame_id);

		cm_put_REQ_IOCR1_send_clock			(iocr->send_clock);
		cm_put_REQ_IOCR1_reduction_ratio	(iocr->reduction_ratio);
		cm_put_REQ_IOCR1_phase				(iocr->phase);
		cm_put_REQ_IOCR1_sequence			(0); /* AP01104502 */
		cm_put_REQ_IOCR1_frame_send_offset	(iocr->frame_send_offset);

		cm_put_REQ_IOCR1_watchdog_factor	(iocr->data_hold_factor); /* AP01304706 */
		cm_put_REQ_IOCR1_data_hold_factor	(iocr->data_hold_factor);

		cm_put_REQ_IOCR1_iocr_tag_header	(CM_CAST_U16(0xC000 | 0)); /* prio 6 + vlan_id=0 (TIA 208236) */
		cm_put_REQ_IOCR1_iocr_multicast_mac	(& iocr->multicast_mac);

		CmListLength (& iocr->related_ap_list, & len_cnt);

		cm_put_REQ_IOCR1_number_of_related_apis (len_cnt);

		cm_adjust (dp, left, sizeof (CM_REQ_IOCR1));

		/* IOCR 2 */
		for (CmListForeach(ioap, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {

			cm_put_assert (left >= sizeof (CM_REQ_IOCR2));

			cm_put_REQ_IOCR2_api (ioap->api);

			CmListLength (& ioap->related_iodata_objects_list, & len_cnt);

			cm_put_REQ_IOCR2_number_of_releated_iodata_objects	(len_cnt);

			cm_adjust (dp, left, sizeof (CM_REQ_IOCR2));


			/* IOCR 3 */
			for (CmListForeach(iodata, & ioap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {

				cm_put_assert (left >= sizeof (CM_REQ_IOCR3));

				cm_put_REQ_IOCR3_slot_nr (iodata->slot_nr);
				cm_put_REQ_IOCR3_subslot_nr (iodata->subslot_nr);
				cm_put_REQ_IOCR3_frame_offset (iodata->frame_offset);

				cm_adjust (dp, left, sizeof (CM_REQ_IOCR3));
			}

			/* IOCR 4 */
			cm_put_assert (left >= sizeof (CM_REQ_IOCR4));

			CmListLength (& ioap->related_iocs_list, & len_cnt);

			cm_put_REQ_IOCR4_number_of_releated_iocs (len_cnt);

			cm_adjust (dp, left, sizeof (CM_REQ_IOCR4));


			/* IOCR 5 */
			for (CmListForeach(iocs, & ioap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE)) {

				cm_put_assert (left >= sizeof (CM_REQ_IOCR5));

				cm_put_REQ_IOCR5_slot_nr		(iocs->slot_nr);
				cm_put_REQ_IOCR5_subslot_nr		(iocs->subslot_nr);
				cm_put_REQ_IOCR5_frame_offset	(iocs->frame_offset);

				cm_adjust (dp, left, sizeof (CM_REQ_IOCR5));
			}
		}

		/* set block length */
		len_cnt = CM_CAST_U16(dp - dp_len - sizeof(CM_BLK_HEADER) + sizeof(LSA_UINT16));
		cm_put_u16 (dp_len, CM_BLK_HEADER, length, len_cnt);
	}
	}


	/* --- [MCRBlockReq*] --- */
	{
	CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

	for (CmListForeach (mcr, & argr->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE))
	{
		/* CM_BLK_HEADER */
		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		dp_len = dp; /* keep for block length calculation */

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_MCR_REQ);
		cm_put_BLOCK_HEADER_length	(0);
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

		/* CM_REQ_MCR */
		cm_put_assert (left >= sizeof (CM_REQ_MCR));

		cm_put_REQ_MCR_iocr_ref (mcr->iocr_ref);
		cm_put_REQ_MCR_mcr_properties (mcr->mcr_properties);
		cm_put_REQ_MCR_mci_timeout_factor (mcr->mci_timeout_100ms);
		cm_put_REQ_MCR_station_name_length (mcr->provider_name_length);

		cm_adjust (dp, left, sizeof (CM_REQ_MCR));

		cm_put_assert (left >= mcr->provider_name_length);
		CM_MEMCPY (dp, mcr->provider_name, mcr->provider_name_length);
		cm_adjust (dp, left, mcr->provider_name_length);

		/* set block length */
		len_cnt = CM_CAST_U16(dp - dp_len - sizeof(CM_BLK_HEADER) + sizeof(LSA_UINT16));
		cm_put_u16 (dp_len, CM_BLK_HEADER, length, len_cnt);
	}
	}


	/* --- [ExpectedSubmoduleBlockReq*] --- */
	{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_SLOT_NR_PTR_TYPE  unique;

	CM_RPC_MEM_U8_PTR_TYPE  dp_related;
	LSA_UINT16  nr_related;

	/* for all unique slot_nr */
	for (CmListForeach(unique, &argr->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE))
	{
		/* unique->slot_nr  must(shall lt. IEC)  static for this block */

		/* CM_BLK_HEADER */
		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		dp_len = dp; /* keep for block length calculation */

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_EXPSUBMOD_REQ);
		cm_put_BLOCK_HEADER_length	(0);
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

		/* CM_REQ_API */
		cm_put_assert (left >= sizeof (CM_REQ_API));

		dp_related = dp; /* keep for number_of_related_apis */
		nr_related = 0;

		cm_put_REQ_API_number_of_related_apis (nr_related);

		cm_adjust (dp, left, sizeof (CM_REQ_API));


			/* search all ap's which contains the unique->slot_nr */
		for (CmListForeach(ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {

			mod = cm_ar_graph_find_module (ap, unique->slot_nr);
			if (is_null (mod)) {
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "ap(%u) does not contain slot_nr(%u)"
					, ap->api
					, unique->slot_nr
					);
				continue;
			}
			/* must only contained once, see InsertSorted */
			CM_ASSERT (unique->slot_nr == mod->slot_nr);

			nr_related += 1;
			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ap(%u) contains slot_nr(%u), nr_related(%u)"
				, ap->api
				, unique->slot_nr
				, nr_related
				);

			/* CM_REQ_MOD */
			cm_put_assert (left >= sizeof (CM_REQ_MOD));

			CmListLength (& mod->submod_list, & len_cnt);

			cm_put_REQ_MOD_api					(ap->api);
			cm_put_REQ_MOD_slot_nr				(mod->slot_nr);
			cm_put_REQ_MOD_mod_ident			(mod->soll.mod_ident);
			cm_put_REQ_MOD_mod_properties		(mod->soll.mod_properties);
			cm_put_REQ_MOD_number_of_submodules	(len_cnt);

			mod->ist.mod_ident = mod->soll.mod_ident;

			cm_adjust (dp, left, sizeof (CM_REQ_MOD));

			/* CM_REQ_SUB */
			for (CmListForeach(sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {

				cm_put_assert (left >= sizeof (CM_REQ_SUB));

				cm_put_REQ_SUB_subslot_nr		(sub->subslot_nr);
				cm_put_REQ_SUB_sub_ident		(sub->soll.sub_ident);
				cm_put_REQ_SUB_sub_properties	(sub->soll.sub_properties);

				sub->ist.sub_state = CM_SUB_STATE_GOOD; /* default */
				sub->ist.sub_ident = sub->soll.sub_ident;

				cm_adjust (dp, left, sizeof (CM_REQ_SUB));

				/* CM_REQ_DSC - INPUT */
				switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					cm_put_assert (left >= sizeof (CM_REQ_DSC));

					cm_put_REQ_DSC_data_description (sub->descr[0].data_description);
					cm_put_REQ_DSC_data_length (sub->descr[0].data_length);
					cm_put_REQ_DSC_iops_length (sub->descr[0].iops_length);
					cm_put_REQ_DSC_iocs_length (sub->descr[0].iocs_length);

					cm_adjust (dp, left, sizeof (CM_REQ_DSC));
					break;

				default:
					/* no input descr */
					break;
				}

				/* CM_REQ_DSC - OUTPUT */
				switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_OUTPUT:
				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					cm_put_assert (left >= sizeof (CM_REQ_DSC));

					cm_put_REQ_DSC_data_description (sub->descr[1].data_description);
					cm_put_REQ_DSC_data_length (sub->descr[1].data_length);
					cm_put_REQ_DSC_iops_length (sub->descr[1].iops_length);
					cm_put_REQ_DSC_iocs_length (sub->descr[1].iocs_length);

					cm_adjust (dp, left, sizeof (CM_REQ_DSC));
					break;

				default:
					/* no output descr */
					break;
				}
			}
		}

		/* set number_of_related_apis */
		cm_put_u16 (dp_related, CM_REQ_API, number_of_related_apis, nr_related);

		/* set block length */
		len_cnt = CM_CAST_U16(dp - dp_len - sizeof(CM_BLK_HEADER) + sizeof(LSA_UINT16));
		cm_put_u16 (dp_len, CM_BLK_HEADER, length, len_cnt);
	}
	}


	/* --- [AlarmCRBlockReq] --- */
	{
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alcr = argr->alarm_cr;

	if (is_null (alcr)) {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "no alarm-cr");
		return 0;
	}

	cm_put_assert (left >= sizeof (CM_BLK_HEADER));

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_ALCR_REQ);
	cm_put_BLOCK_HEADER_length	(sizeof (CM_REQ_ALCR) + sizeof(LSA_UINT16));
	cm_put_BLOCK_HEADER_version	(0x0100);

	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

	/* CM_REQ_ALCR */
	cm_put_assert (left >= sizeof (CM_REQ_ALCR));

	cm_put_REQ_ALCR_alcr_type				(alcr->alcr_type);
	cm_put_REQ_ALCR_ethertype				(alcr->ethertype);
	cm_put_REQ_ALCR_alcr_properties			(alcr->alcr_properties);
	cm_put_REQ_ALCR_timeout100ms			(alcr->timeout100ms);
	cm_put_REQ_ALCR_retries					(alcr->retries);
	cm_put_REQ_ALCR_local_alarm_ref			(alcr->local_alarm_ref);
	cm_put_REQ_ALCR_max_data_length			(cmi_max_alarm_data_length); /* alcr->max_alarm_data_len will store response */
	cm_put_REQ_ALCR_alcr_tag_header_high	(CM_CAST_U16(0xC000 | 0)); /* prio 6 + vlan_id=0 (TIA 208236) */
	cm_put_REQ_ALCR_alcr_tag_header_low		(CM_CAST_U16(0xA000 | 0)); /* prio 5 + vlan_id=0 (TIA 208236) */

	cm_adjust (dp, left, sizeof (CM_REQ_ALCR));
	}


	/* --- [PrmServerBlock] --- */

	/* not yet */


	/* --- [ARRPCBlockReq] --- */

	/* not yet */


	/* --- [IRInfoBlock] --- */
	if (is_not_null (argr->ir_info))
	{
		LSA_UINT16 subCount;

		CmListLength (&(argr->ir_info->ir_info_sub_list), &subCount);

		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_IRINFOBLOCK);
		cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_REQ_IRINFO) + subCount * sizeof (CM_REQ_IRINFOSUB)));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

		cm_put_assert (left >= sizeof (CM_REQ_IRINFO));

		cm_put_REQ_IR_padding1			(0);
		cm_put_REQ_IR_ir_data_uuid		(& argr->ir_info->ir_data_uuid);
		cm_put_REQ_IR_padding2			(0);
		cm_put_REQ_IR_number_of_iocrs	(subCount);

		cm_adjust (dp, left, sizeof (CM_REQ_IRINFO));

		if (0 != subCount)
		{
			CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE irInfoSub;

			for (CmListForeach(irInfoSub, &argr->ir_info->ir_info_sub_list, CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE))
			{
				cm_put_assert (left >= sizeof (CM_REQ_IRINFOSUB));
				cm_put_REQ_IRISUB_iocr_reference	(irInfoSub->iocr_ref);
				cm_put_REQ_IRISUB_subframe_offset	(irInfoSub->subframe_offset);
				cm_put_REQ_IRISUB_subframe_data		(irInfoSub->subframe_data);
				cm_adjust (dp, left, sizeof (CM_REQ_IRINFOSUB));
			}
		}
	}


	/* --- [SRInfoBlock] --- */
	if (is_not_null (argr->sr_info))
	{
		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_SRINFOBLOCK);
		cm_put_BLOCK_HEADER_length	(CM_CAST_U16(sizeof(LSA_UINT16) + sizeof (CM_REQ_SRINFO)));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


		cm_put_assert (left >= sizeof (CM_REQ_SRINFO));

		cm_put_REQ_SRD_RedundancyDataHoldFactor (argr->sr_info->rdh_factor);
		cm_put_REQ_SRD_SRProperties				(argr->sr_info->sr_properties);

		cm_adjust (dp, left, sizeof (CM_REQ_SRINFO));
	}


	/* --- [ARVendorBlockReq*] --- */
	{
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

	/* BlockHeader, APStructureIdentifier, API, [Data*], [Padding* a] */

	for (CmListForeach(arvendor, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		dp_len = dp; /* keep for block length calculation */

		cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_ARVENDORBLOCK_REQ);
		cm_put_BLOCK_HEADER_length	(0);
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


		cm_put_assert (left >= sizeof(CM_REQ_ARVENDOR));

		cm_put_REQ_ARV_APStructureIdentifier (arvendor->ap_structure_identifier);
		cm_put_REQ_ARV_API (arvendor->api);

		cm_adjust (dp, left, sizeof(CM_REQ_ARVENDOR));


		cm_put_assert (left >= arvendor->req_data_len);

		CM_MEMCPY (dp, arvendor->data, arvendor->req_data_len);

		cm_adjust (dp, left, arvendor->req_data_len);

		/* a The number of padding octets shall be adapted to make the block Unsigned32 aligned. */
		{
			/* [Data*] starts aligned, so [Padding* a] depends only on data_len */
			LSA_INT  padding = cm_cl_calc_padding_length (arvendor->req_data_len);
			while (padding > 0)
			{
				cm_put_assert (left >= 1);
				CM_PUT8_HTON (dp, 0, 0);
				--padding;
				cm_adjust (dp, left, 1);
			}
		}

		/* set block length */
		len_cnt = CM_CAST_U16(dp - dp_len - sizeof(CM_BLK_HEADER) + sizeof(LSA_UINT16));
		cm_put_u16 (dp_len, CM_BLK_HEADER, length, len_cnt);

		/* expect an ARVendorBlockRes */
		arvendor->res_data_len = 0;
	}
	}


	/* --- [ARFSUBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
	{
		CM_AR_GRAPH_RECORD_PTR_TYPE ptr;
		for (CmListForeach(ptr, &argr->cm.cl.ar_connect_prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
		{
			if (ptr->record_index == 0xE050 /* ARFSUDataAdjust data */)
			{
				cm_put_assert (left >= sizeof (CM_BLK_HEADER));

				cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_ARFSUBLOCK);
				cm_put_BLOCK_HEADER_length	(CM_GET16_NTOH(ptr->record_data, 2));
				cm_put_BLOCK_HEADER_version	(0x0100);

				cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

				cm_put_assert (left >= ptr->record_data_len - sizeof (CM_BLK_HEADER));

				CM_MEMCPY (dp, ptr->record_data + sizeof (CM_BLK_HEADER), ptr->record_data_len - sizeof (CM_BLK_HEADER));

				cm_adjust (dp, left, ptr->record_data_len - sizeof (CM_BLK_HEADER));
			}
		}
	}

	/* returns written data */
	return CM_CAST_U16(dp - data);

cm_put_assert_failed:
	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_put_assert_line
		);
	LSA_UNUSED_ARG (cm_put_assert_line);
	return 0;
}


/*----------------------------------------------------------------------------*/
/*	returns:
 *	0								.. Ok
 *	! is_appl_ready:
 *		CM_AR_REASON_xxx			.. Not Ok
 *	is_appl_ready:
 *		CM_PNIO_ERR_CODE_12(ErrorCode1, ErrorCode2)	.. Not Ok, see "Values of ErrorCode1 and ErrorCode2 for ErrorDecode with the value PNIO"
 */
static LSA_UINT16 CM_LOCAL_FCT_ATTR
cm_cl_parse_connect_response (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_MEM_U8_PTR_TYPE  dp_start,
	LSA_UINT32  dp_len,
	LSA_BOOL  is_appl_ready
	)
{
	CM_LOG_FCT ("cm_cl_parse_connect_response")
	LSA_UINT16  block_type, block_length, block_version;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  cm_get_assert_line = 0;
	LSA_UINT16  cm_get_assert_error = 0;
	LSA_BOOL  block_ar_seen = LSA_FALSE;
	LSA_BOOL  block_iocr_seen = LSA_FALSE;
	LSA_BOOL  block_alcr_seen = LSA_FALSE;
	LSA_BOOL  block_moddiff_seen = LSA_FALSE;
	LSA_BOOL  block_arserver_seen = LSA_FALSE;

	dp = dp_start;
	left = dp_len;
	block_type = 0xCCCC;

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("TODO: change all returns like ModuleDiffBlock does !")
#endif /* CM_MESSAGE */

	while (left > 0)
	{
		cm_get_assert (left >= sizeof (CM_BLK_HEADER));

		cm_get_BLOCK_HEADER_type (& block_type);
		cm_get_BLOCK_HEADER_length (& block_length);
		cm_get_BLOCK_HEADER_version (& block_version);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "BLOCK: type(0x%04x) len(%u) version(0x%04x)"
			, block_type, block_length, block_version
			);

		cm_get_assert (block_length >= sizeof(block_version));
		cm_get_assert (left >= (block_length - sizeof(block_version)));

		if (is_appl_ready)
		{
			if (block_type != CM_BLOCK_TYPE_MOD_DIFF)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "is_appl_ready and wrong block_type(0x%x)", block_type);
				return CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/);
			}
		}
		else if (! block_ar_seen)
		{
			if (block_type != CM_BLOCK_TYPE_AR_RSP)
			{
				CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "is_connect_rsp and wrong block_type(0x%x)", block_type);
				return CM_AR_REASON_PROT;
			}
		}

		switch (block_type)
		{
		case CM_BLOCK_TYPE_AR_RSP: /* ARBlockRes */
			{
			LSA_UINT16  ar_type, session_key;
			CLRPC_UUID_TYPE  ar_uuid;
			CM_MAC_TYPE  cmr_mac;
			LSA_UINT16  cmr_udp_port;

			if (block_ar_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARBlockRes: twice");
				return CM_AR_REASON_PROT;
			}
			block_ar_seen = LSA_TRUE;

			cm_get_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_get_assert (left >= sizeof (CM_RSP_AR));

			cm_get_RSP_AR_ar_type (& ar_type);
			cm_get_RSP_AR_ar_uuid (& ar_uuid);
			cm_get_RSP_AR_session_key (& session_key);
			cm_get_RSP_AR_cmr_mac (& cmr_mac);
			cm_get_RSP_AR_udp_cmr_port(& cmr_udp_port);

			cm_adjust (dp, left, sizeof (CM_RSP_AR));

			CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARBlockRes: ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
				, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
				, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
				, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
				, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
				);
			CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARBlockRes: type(%u) session_key(%u) cmr_mac(%02x-%02x-%02x-%02x-%02x-%02x) cmr_udp(%u)"
				, ar_type
				, session_key
				, cmr_mac.mac[0], cmr_mac.mac[1], cmr_mac.mac[2]
				, cmr_mac.mac[3], cmr_mac.mac[4], cmr_mac.mac[5]
				, cmr_udp_port
				);

			if (! cm_uuid_equal (& ar_uuid, & argr->ar.ar_uuid)) {
				CM_CL_TRACE_11 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARBlockRes: dev.ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x) does not match"
					, argr->ar.ar_uuid.time_low, argr->ar.ar_uuid.time_mid, argr->ar.ar_uuid.time_hi_and_version
					, argr->ar.ar_uuid.clock_seq_hi_and_reserved, argr->ar.ar_uuid.clock_seq_low
					, argr->ar.ar_uuid.node[0], argr->ar.ar_uuid.node[1], argr->ar.ar_uuid.node[2]
					, argr->ar.ar_uuid.node[3], argr->ar.ar_uuid.node[4], argr->ar.ar_uuid.node[5]
					);
				return CM_AR_REASON_PROT;
			}

			if (session_key != argr->cm.cl.connect_counter) {
				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARBlockRes: session_key(%u) != connect_counter(%u)"
					, session_key, argr->cm.cl.connect_counter
					);
				return CM_AR_REASON_PROT;
			}

			if (! cm_ar_graph_update_io_ar (argr, cmr_mac.mac, CM_HTON16(cmr_udp_port))) {
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARBlockRes: cannot cm_ar_graph_update_io_ar");
				return CM_AR_REASON_PROT;
			}
			}
			break;

		case CM_BLOCK_TYPE_IOCR_RSP:	/* IOCRBlockRes */
			{
			LSA_UINT16  iocr_type, iocr_ref, frame_id;

			block_iocr_seen = LSA_TRUE;

			cm_get_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_get_assert (left >= sizeof (CM_RSP_IOCR));

			cm_get_RSP_IOCR_iocr_type (& iocr_type);
			cm_get_RSP_IOCR_iocr_ref (& iocr_ref);
			cm_get_RSP_IOCR_frame_id (& frame_id);

			cm_adjust (dp, left, sizeof (CM_RSP_IOCR));

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "IOCRBlockRes: type(%u) ref(%u) frame_id(%u)"
				, iocr_type, iocr_ref, frame_id
				);

			if (! cm_ar_graph_update_io_cr (argr, iocr_type, iocr_ref, frame_id)) {
				goto cm_get_assert_failed;
			}
			}
			break;

		case CM_BLOCK_TYPE_MOD_DIFF: /* ModuleDiffBlock */
			{
			/*/	prm_mode \ is_appl_rdy	 FALSE								 TRUE
			//	 PRMMODE_CONNECT:		IODConnectRes						IOCControlReq(aka ApplRdy 4 Connect)
			//							 (exp-sub)							 (exp-sub)
			//	 PRMMODE_PLUG:												IOCControlReq(aka ApplRdy 4 Plug)
			//																 (one-sub/plug)
			//	 PRMMODE_AR:			IODControlRes(aka PrmBegin.rsp)		IOCControlReq(aka ApplRdy 4 PrmUpdate)
			//							 (exp-sub)							 (exp-sub)
			//	 PRMMODE_SUBMOD:		IODControlRes(aka PrmBegin.rsp)		IOCControlReq(aka ApplRdy 4 PrmUpdate)
			//							 (one-sub/upd)						 (one-sub/upd)
			*/
			LSA_UINT16  api_cnt;
			LSA_UINT8  dev_type;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE  one_sub = LSA_NULL;

			if (block_moddiff_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ModuleDiffBlock: twice");
				return CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/);
			}
			block_moddiff_seen = LSA_TRUE;

			switch (cm_cl_prm_mode(argr))
			{
			case CM_CL_PRMMODE_IDLE: /* ! note: IDLE means Connect, because PrmWrite is not started yet */
			case CM_CL_PRMMODE_CONNECT:
				one_sub = LSA_NULL;
				break;

			case CM_CL_PRMMODE_PLUG:
				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & one_sub
					);
				cm_get_assert2 (is_not_null (one_sub), CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				break;

			case CM_CL_PRMMODE_AR:
				one_sub = LSA_NULL;
				break;

			case CM_CL_PRMMODE_SUBMODULE:
				cm_ar_graph_find_api_slot_subslot (argr
					, argr->cm.cl.prm.submod.api, argr->cm.cl.prm.submod.slot_nr, argr->cm.cl.prm.submod.subslot_nr
					, LSA_NULL, LSA_NULL, & one_sub
					);
				cm_get_assert2 (is_not_null (one_sub), CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				break;

			case CM_CL_PRMMODE_RETURN_OF_SUB:
			default:
				cm_get_assert_error_set (CM_PNIO_ERR_ModuleDiffBlock(0/*BlockType*/));
				goto cm_get_assert_failed;
			}

			/* please note ArtsRQ:
			**  AP00219434: allow all submodule_state within appl_rdy.ind
			**              // but not connect.rsp/NO => appl_rdy.ind/PROPER/SUBST
			**              // accept all ident, even when differ from connect.rsp
			**	AP00350631: auto-convert submodule_state to new format (FormatIndicator == 1)
			**	AP00841926: never generate CM_MOD_STATE_WRONG_MODULE internally & checks according to 61158-6
			**	AP00893748: PDU-Check per AP00841926 ist beim Plug-Alarm zu streng
			**				// e.g. allow a PLUG.alarm followed by a ModDiffBlock with NO_MODULE
			**	AP00895528: allow number_of_modules == 0 (legacy,  version <= v2i7.13)
			**	AP00962427: IEPB Link: Control request wird abgelehnt (legacy,  version <= v2i7.14)
			**				// number_of_module == 1: mod_state == NO_MODULE: number_of_submodule == 1: sub_state == NO_SUBMOD
			*/

			cm_get_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ModuleDiffBlock(2/*BlockVersionHigh*/));
			cm_get_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ModuleDiffBlock(3/*BlockVersionLow*/));

			dev_type = cm_cl_guess_dev_type (dp, left);
			if (argr->cm.cl.dev->dev_type == CM_CL_DEV_TYPE_UNKNOWN) {
				argr->cm.cl.dev->dev_type = dev_type;
			}
			if (argr->cm.cl.dev->dev_type != CM_CL_DEV_TYPE_UNKNOWN
				&&  dev_type != CM_CL_DEV_TYPE_UNKNOWN)
			{
				cm_get_assert2 (argr->cm.cl.dev->dev_type == dev_type, CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/));
			}

			cm_get_assert2 (left >= sizeof (CM_RSP_DIFF), CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

			cm_get_RSP_DIFF_number_of_apis (& api_cnt);

			cm_adjust (dp, left, sizeof (CM_RSP_DIFF));

			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ModuleDiffBlock: api_cnt(%u)"
				, api_cnt
				);

			if (is_not_null (one_sub))
			{
				cm_get_assert2 (api_cnt == 1, CM_PNIO_ERR_ModuleDiffBlock(4/*NumberOfAPIs*/));
			}
			else
			{
				cm_get_assert2 (api_cnt >= 1, CM_PNIO_ERR_ModuleDiffBlock(4/*NumberOfAPIs*/));
			}

			while (api_cnt > 0)
			{
				LSA_UINT32  api;
				LSA_UINT16  mod_cnt;
				CM_AR_GRAPH_AP_PTR_TYPE  ap;

				cm_get_assert2 (left >= sizeof (CM_RSP_API), CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

				cm_get_RSP_API_api (& api);
				cm_get_RSP_API_number_of_modules (& mod_cnt);

				cm_adjust (dp, left, sizeof (CM_RSP_API));

				CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
					, " API: api(%u) mod_cnt(%u)"
					, api, mod_cnt
					);

				ap = cm_ar_graph_find_ap (argr, api);
				cm_get_assert2 (is_not_null (ap), CM_PNIO_ERR_ModuleDiffBlock(5/*API*/));

				if (is_not_null (one_sub))
				{
					cm_get_assert2 (argr->cm.cl.prm.submod.api == api, CM_PNIO_ERR_ModuleDiffBlock(5/*API*/));
					cm_get_assert2 (mod_cnt <= 1, CM_PNIO_ERR_ModuleDiffBlock(6/*NumberOfModules*/));
				}

				/* AP00895528: allow number_of_moduls == 0 (legacy, version <= v2i7.13) */

				while (mod_cnt > 0)
				{
					LSA_UINT16  slot_nr, mod_state, number_of_submodules, sub_cnt;
					LSA_UINT32  mod_ident;
					LSA_BOOL  mod_state_ok;
					CM_AR_GRAPH_MODULE_PTR_TYPE  mod;

					cm_get_assert2 (left >= sizeof (CM_RSP_MOD), CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

					cm_get_RSP_MOD_slot_nr (& slot_nr);
					cm_get_RSP_MOD_mod_ident (& mod_ident);
					cm_get_RSP_MOD_mod_state (& mod_state);
					cm_get_RSP_MOD_number_of_submodules (& number_of_submodules);

					cm_adjust (dp, left, sizeof (CM_RSP_MOD));

					CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "  MOD: slot_nr(%u) mod_ident(0x%x) mod_state(%u) number_of_submodules(%u)"
						, slot_nr, mod_ident, mod_state, number_of_submodules
						);

					mod = cm_ar_graph_find_module (ap, slot_nr);
					cm_get_assert2 (is_not_null (mod), CM_PNIO_ERR_ModuleDiffBlock(7/*SlotNumber*/));

					if (is_not_null (one_sub))
					{
						cm_get_assert2 (argr->cm.cl.prm.submod.slot_nr == slot_nr, CM_PNIO_ERR_ModuleDiffBlock(7/*SlotNumber*/));
					}

					/* CM_PNIO_ERR_ModuleDiffBlock(8/ModuleIdentNumber/): not checked */

					mod_state_ok = cm_mod_state_check (mod_state);
					cm_get_assert2 (mod_state_ok, CM_PNIO_ERR_ModuleDiffBlock(9/*ModuleState*/));

					if (mod_state == CM_MOD_STATE_NO_MODULE)
					{
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						if (number_of_submodules == 0)
						{
							for (CmListForeach(sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
							{
								if (is_not_null (one_sub))
								{
									if (argr->cm.cl.prm.submod.subslot_nr == sub->subslot_nr)
									{
										sub->ist.sub_state = CM_SUB_STATE_PULLED;
									}
								}
								else
								{
									sub->ist.sub_state = CM_SUB_STATE_PULLED;
								}
							}
						}
						else /* legacy, version <= v2i7.14, see CM_OPC_SV_APPL_READY2(!), AP00962427 */
						{
							if (is_not_null (one_sub))
							{
								cm_get_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
							else
							{
								LSA_UINT16  exp_sub_count;
								CmListLength (& mod->submod_list, & exp_sub_count);
								cm_get_assert2 (number_of_submodules == exp_sub_count, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
						}
					}
					else if (mod_state == CM_MOD_STATE_WRONG_MODULE)
					{
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						if (number_of_submodules == 0)
						{
							/* legacy, version <= v2i7.13 */
							for (CmListForeach(sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
							{
								if (is_not_null (one_sub))
								{
									if (argr->cm.cl.prm.submod.subslot_nr == sub->subslot_nr)
									{
										sub->ist.sub_state = CM_SUB_STATE_WRONG;
									}
								}
								else
								{
									sub->ist.sub_state = CM_SUB_STATE_WRONG;
								}
							}
						}
						else
						{
							if (is_not_null (one_sub))
							{
								cm_get_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
							else
							{
								LSA_UINT16  exp_sub_count;
								CmListLength (& mod->submod_list, & exp_sub_count);
								cm_get_assert2 (number_of_submodules == exp_sub_count, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
							}
						}
					}
					else /* PROPER or SUBSTITUTED */
					{
						if (is_not_null (one_sub))
						{
							cm_get_assert2 (number_of_submodules == 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
						}
						else
						{
							cm_get_assert2 (number_of_submodules >= 1, CM_PNIO_ERR_ModuleDiffBlock(10/*NumberOfSubmodules*/));
						}
					}

					sub_cnt = number_of_submodules;
					while (sub_cnt > 0)
					{
						LSA_UINT16  subslot_nr, sub_state;
						LSA_UINT32  sub_ident;
						LSA_BOOL  sub_state_ok;
						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

						cm_get_assert2 (left >= sizeof (CM_RSP_SUB), CM_PNIO_ERR_ModuleDiffBlock(1/*BlockLength*/));

						cm_get_RSP_SUB_subslot_nr (& subslot_nr);
						cm_get_RSP_SUB_sub_ident (& sub_ident);
						cm_get_RSP_SUB_sub_state (& sub_state);

						cm_adjust (dp, left, sizeof (CM_RSP_SUB));

						CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "   SUB: subslot_nr(0x%x) sub_ident(0x%x) sub_state(0x%x)"
							, subslot_nr, sub_ident, sub_state
							);

						sub = cm_ar_graph_find_submodule (ap, mod, slot_nr, subslot_nr);
						cm_get_assert2 (is_not_null (sub), CM_PNIO_ERR_ModuleDiffBlock(11/*SubslotNumber*/));

						if (is_not_null (one_sub))
						{
							cm_get_assert2 (argr->cm.cl.prm.submod.subslot_nr == subslot_nr, CM_PNIO_ERR_ModuleDiffBlock(11/*SubslotNumber*/));
						}

						/* , CM_PNIO_ERR_ModuleDiffBlock(12/SubmoduleIdentNumber/): not checked */

						sub_state = cm_sub_state_to_format_1 (sub_state); /* to be compatible to CM V2 */
						sub_state_ok = cm_sub_state_check (mod_state, sub_state);
						cm_get_assert2 (sub_state_ok, CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/));

						if (is_appl_ready)
						{
							LSA_BOOL prm_old = cm_cl_sub_state_must_prmed (sub->ist.sub_state);
							LSA_BOOL prm_new = cm_cl_sub_state_prmed_effect (sub_state);

							if (! prm_old && prm_new)
							{
								CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
									, "ModuleDiffBlock: slot_nr(%u) subslot_nr(0x%x) sub_state_old(%u) sub_state_new(%u), submodule wasn't prm but ApplRdy told us it should prm"
									, slot_nr, subslot_nr, sub->ist.sub_state, sub_state
									);
								return (CM_PNIO_ERR_ModuleDiffBlock(13/*SubmoduleState*/));
							}
						}

						/***/

						sub->ist.sub_state = sub_state;
						sub->ist.sub_ident = sub_ident;

						sub_cnt -= 1;
					}

					/***/

					mod->ist.mod_ident = mod_ident;

					mod_cnt -= 1;
				}
				api_cnt -= 1;
			}
			}
			break;

		case CM_BLOCK_TYPE_ALCR_RSP: /* AlarmCRBlockRes */
			{
			LSA_UINT16  alcr_type, remote_alarm_ref, max_alarm_data_length;
			CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alcr;

			if (block_alcr_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "AlarmCRBlockRes: twice");
				return CM_AR_REASON_PROT;
			}
			block_alcr_seen = LSA_TRUE;

			cm_get_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_get_assert (left >= sizeof (CM_RSP_ALCR));

			cm_get_RSP_ALCR_alcr_type (& alcr_type);
			cm_get_RSP_ALCR_local_alarm_ref (& remote_alarm_ref);
			cm_get_RSP_ALCR_max_alarm_data_length (& max_alarm_data_length);

			cm_adjust (dp, left, sizeof (CM_RSP_ALCR));

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "AlarmCRBlockRes: type(%u) remote_alarm_ref(%u) alarm_data_len(%u)"
				, alcr_type, remote_alarm_ref, max_alarm_data_length
				);

			alcr = cm_ar_graph_find_alarm_cr (argr);
			cm_get_assert (is_not_null (alcr));

			alcr->remote_alarm_ref = remote_alarm_ref;
			alcr->max_alarm_data_len = max_alarm_data_length;

			cm_get_assert (alcr->remote_alarm_ref != ACP_INVALID_ALARM_REF);
			cm_get_assert (CM_ALARM_DATA_LENGTH_MIN <= alcr->max_alarm_data_len);
			cm_get_assert (alcr->max_alarm_data_len <= CM_ALARM_DATA_LENGTH_MAX);
#ifdef CM_MESSAGE
# pragma CM_MESSAGE("TODO: check and/or adapt with channel->acp.max_alarm_data_length")
#endif /* CM_MESSAGE */
			}
			break;

		case CM_BLOCK_TYPE_ARSERVERBLOCK:
			{
			LSA_UINT16  cmr_name_length, i, pad_length;

			if (block_arserver_seen)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARServerBlock: twice");
				return CM_AR_REASON_PROT;
			}
			block_arserver_seen = LSA_TRUE;

			if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) == 0)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "ARServerBlock: only valid with ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED"
					);
				return CM_AR_REASON_PROT;
			}

			cm_get_assert (block_version == 0x0100);	/* only version 1.0 */

			cm_get_assert (left >= sizeof (CM_RSP_ARSERVER));

			cm_get_RSP_ARSERVER_cmr_name_length (& cmr_name_length);

			cm_adjust (dp, left, sizeof (CM_RSP_ARSERVER));

			/* CM_RSP_ARSERVER::cmr_name starts aligned */
			pad_length = cm_cl_calc_padding_length (cmr_name_length);

			CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARServerBlock: cmr_name_len(%u) pad_len(%u)", cmr_name_length, pad_length
				);

			cm_get_assert (block_length == sizeof(block_version) + sizeof(cmr_name_length) + cmr_name_length + pad_length);

			{
				LSA_UINT16               device_name_length;
				CM_UPPER_MEM_U8_PTR_TYPE device_name;

				if (is_not_null(argr->cm.cl.mtd) && (argr->cm.cl.mtd->properties & CM_PD_MTD_PROP_NOS))
				{
					device_name_length = argr->cm.cl.dev->dev_addr.tailored_device_name_length;
					device_name = argr->cm.cl.dev->dev_addr.tailored_device_name;
				}
				else
				{
					device_name_length = argr->cm.cl.dev->dev_addr.device_name_length;
					device_name = argr->cm.cl.dev->dev_addr.device_name;
				}

				if (cmr_name_length != device_name_length)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "ARServerBlock: cmr_name_length(%u)  dev_name_length(%u)"
						, cmr_name_length, argr->cm.cl.dev->dev_addr.device_name_length
						);
					return CM_AR_REASON_PROT;
				}

				cm_get_assert (left >= cmr_name_length);

				for (i = 0; i < device_name_length; ++i)
				{
					if (*(dp + i) != *(device_name + i))
					{
						CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "ARServerBlock: CMResponderStationName[%u](%c) != expeceted(%c)"
							, i, *(dp + i), *(device_name + i)
						);
						return CM_AR_REASON_PROT;
					}
				}
			}

			cm_adjust (dp, left, cmr_name_length);

				/* [ Padding* a] */
			cm_get_assert (left >= pad_length);
			cm_adjust (dp, left, pad_length);
			}
			break;

		case CM_BLOCK_TYPE_ARVENDORBLOCK_RSP:
			{
			LSA_UINT16  ap_structure_identifier;
			LSA_UINT32  api;
			LSA_UINT16  data_and_padding_length;
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

			cm_get_assert2 ((block_length % 4) == 0, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			cm_get_assert2 (block_length + sizeof(block_type) + sizeof(block_length) <= 512/*see cm_ar_graph_connect_len()*/, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			cm_get_assert2 (block_length >= sizeof(block_version) + sizeof(ap_structure_identifier) + sizeof(api), CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			data_and_padding_length = CM_CAST_U16 (block_length - sizeof(block_version) - sizeof(ap_structure_identifier) - sizeof(api));

			cm_get_assert2 (block_version == 0x0100, CM_PNIO_ERR_ARVendorBlockRes(2/*BlockVersion*/));	/* only version 1.0 */

			cm_get_assert2 (left >= sizeof (CM_RSP_ARVENDOR), CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));

			cm_get_RSP_ARV_APStructureIdentifier (& ap_structure_identifier);
			cm_get_RSP_ARV_API (& api);

			cm_adjust (dp, left, sizeof (CM_RSP_ARVENDOR));

			CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ARVendorBlockRes: api(%u) ap_structure_identifier(%u) data_and_padding_length(%u)"
				, api, ap_structure_identifier, data_and_padding_length
				);

			arvendor = cm_ar_graph_find_arvendor (argr, api, ap_structure_identifier);
			cm_get_assert2 (is_not_null (arvendor), CM_PNIO_ERR_ARVendorBlockRes(4/*APStructureIdentifier*/));

			cm_get_assert2 (left >= data_and_padding_length, CM_PNIO_ERR_ARVendorBlockRes(1/*BlockLength*/));
			/* ignore [Data*], [Padding*] */
			cm_adjust (dp, left, data_and_padding_length);

			if (arvendor->res_data_len == 1)
			{
				CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ARVendorBlockRes: twice");
				return CM_PNIO_ERR_ARVendorBlockRes(0/*BlockType*/);
			}
			else
			{
				arvendor->res_data_len = 1; /* see check for all ARVendorBlockRes */
			}
			}
			break;

		case CM_BLOCK_TYPE_PRMSRV_RSP:
		default:
			CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "IODConnectRes: unknown block_type(0x%x)", block_type);
			return CM_AR_REASON_PROT;
		}
	}

	if (! is_appl_ready)
	{
		if (! (block_ar_seen && block_iocr_seen && block_alcr_seen))
		{
			CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "IODConnectRes: dev(%u) ar(%u): not all block_type seen, ar(%u) iocr(%u) alcr(%u)"
				, argr->device_nr, argr->ar_nr
				, block_ar_seen, block_iocr_seen, block_alcr_seen
				);
			return CM_AR_REASON_PROT;
		}

		if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
		{
			if (! block_arserver_seen)
			{
				CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "IODConnectRes: dev(%u) ar(%u): AdvancedStartup: not all block_type seen, arserver(%u)"
					, argr->device_nr, argr->ar_nr
					, block_arserver_seen
					);
				return CM_AR_REASON_PROT;
			}
		}

		if (! CmListIsEmpty (&argr->arvendor_list))
		{
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

			for (CmListForeach(arvendor, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
			{
				if (arvendor->res_data_len == 0)
				{
					CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "IODConnectRes: dev(%u) ar(%u): ARVendorBlockRes not found for api(%u) and ap_structure_identifier(%u)"
						, argr->device_nr, argr->ar_nr
						, arvendor->api, arvendor->ap_structure_identifier
						);
					return CM_PNIO_ERR_ARVendorBlockRes(0/*BlockType*/);
				}
			}
		}
	}

	return 0/*CM_AR_REASON_NONE*/;

cm_get_assert_failed:
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "IODConnectRes: parse response failed, assert_error(0x%x) sourcecode-line(%u) current-block_type(0x%x), current-parse-offset(%u)"
		, cm_get_assert_error, cm_get_assert_line, block_type, (LSA_UINT32)dp - (LSA_UINT32)dp_start
		);
	LSA_UNUSED_ARG (cm_get_assert_line);
	return cm_get_assert_error;
}


/*------------------------------------------------------------------------------
// bind an rpc-client-handle to the device->rpc_client_id
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_bind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_LOG_FCT ("cm_cl_rpc_bind")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_BIND_CNF);

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc)) {
		return CM_ERR_ALLOC_LOWER;
	}

	rpc->args.cl.bind->client_id		= CM_RPC_INVALID_CLIENT_ID; /* out */

	rpc->args.cl.bind->host_ip			= CM_HTON32(0x7F000001); /* anything, 127.0.0.1 */
	rpc->args.cl.bind->udp_port			= 0;

	cm_rpc_get_IPNIO_Device(
		& rpc->args.cl.bind->if_uuid,
		& rpc->args.cl.bind->if_version_major,
		& rpc->args.cl.bind->if_version_minor,
		LSA_NULL, LSA_NULL
		);

	cm_rpc_make_object_uuid (& rpc->args.cl.bind->object_uuid,
			dev->dev_rpc.vendor_id, dev->dev_rpc.device_id, dev->dev_rpc.instance
		);

	cm_rpc_request_lower (channel
		, rpc, CLRPC_OPC_CL_BIND, CM_CAST_U16(dev->device_nr | CM_CL_DEV_SUBSYS_DEV), 0
		);

	return CM_OK_PENDING;
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_bind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_rpc_bind_done")

	CM_ASSERT (is_not_null (rpc));
	CM_ASSERT (dev->wf & CM_CL_DEV__WF_BIND_CNF);

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u): wf(0x%x) rpc-rsp(%u)"
		, dev->device_nr, dev->wf, CM_RPC_GET_RESPONSE (rpc)
		);

	if (CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK)
	{
		dev->rpc_client_id = rpc->args.cl.bind->client_id;
		cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_BIND_CNF, CM_OK);
	}
	else
	{
		dev->rpc_client_id = CM_RPC_INVALID_CLIENT_ID;
		cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_BIND_CNF, CM_ERR_LOWER_LAYER);
	}
}


/*------------------------------------------------------------------------------
// rebind the device->rpc_client_id to the dev_addr
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_rebind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_rebind")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	CM_ASSERT (argr->cm.cl.state == CM_DST_REBIND);

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc))
	{
		CM_CL_DOWN_INFO_TYPE  dinfo;

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_BIND));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	rpc->args.cl.bind->client_id		= dev->rpc_client_id;

	rpc->args.cl.bind->host_ip			= argr->ar.host_ip;
	rpc->args.cl.bind->udp_port			= 0;

	cm_rpc_get_IPNIO_Device(
		& rpc->args.cl.bind->if_uuid,
		& rpc->args.cl.bind->if_version_major,
		& rpc->args.cl.bind->if_version_minor,
		LSA_NULL, LSA_NULL
		);

	cm_rpc_make_object_uuid (& rpc->args.cl.bind->object_uuid
		, dev->dev_rpc.vendor_id, dev->dev_rpc.device_id, dev->dev_rpc.instance
		);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF);

	cm_rpc_request_lower (channel, rpc, CLRPC_OPC_CL_REBIND, argr->device_nr, argr->ar_nr);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_rebind_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_rpc_rebind_done")

	CM_ASSERT (argr->cm.cl.state == CM_DST_REBIND || argr->cm.cl.state == CM_DST_WF_DOWN);

	CM_ASSERT (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF));
	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_REBIND_CNF);

	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) rsp(%u) bind(%08x)"
		, argr->device_nr
		, argr->ar_nr
		, CM_RPC_GET_RESPONSE (rpc)
		, rpc->args.cl.bind->client_id
		);

	if (CM_RPC_GET_RESPONSE (rpc) != CLRPC_OK)
	{
		/* docu: should never fail */
		CM_FATAL1(CM_RPC_GET_RESPONSE (rpc));
	}

	cm_cl_arsm (argr);
}


/*------------------------------------------------------------------------------
// rpc-unbind the rpc-client-handle
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_rpc_unbind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev
	)
{
	CM_LOG_FCT ("cm_cl_rpc_unbind")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) wf(0x%x) rpc_client_id(0x%x)"
		, dev->device_nr
		, dev->wf
		, dev->rpc_client_id
		);

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_UNBIND_CNF);

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc))
	{
		dev->rpc_client_id = CM_RPC_INVALID_CLIENT_ID; /* else recursion !!! */
/*
*/
		return CM_ERR_ALLOC_LOWER;
	}

	rpc->args.cl.unbind->client_id = dev->rpc_client_id;

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_UNBIND, CM_CAST_U16 (dev->device_nr | CM_CL_DEV_SUBSYS_DEV), 0
		);

	return CM_OK_PENDING;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_unbind_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_CL_DEVICE_PTR_TYPE  dev,
	LSA_UINT16  rpc_rsp
	)
{
	CM_LOG_FCT ("cm_cl_rpc_unbind_done")
	LSA_UINT16  cm_rsp;

	CM_ASSERT (dev->wf & CM_CL_DEV__WF_UNBIND_CNF);
/*
	dev->wf &= ~ CM_CL_DEV__WF_UNBIND_CNF;

	must handled in the forwarwed functions
*/

	if (rpc_rsp != CLRPC_OK) {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) rsp(%u) from rpc, set to invalid_client_id"
			, dev->device_nr, rpc_rsp
			);
		cm_rsp = CM_ERR_LOWER_LAYER;
	}
	else {
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) rpc_rsp(%u)"
			, dev->device_nr, rpc_rsp
			);
		cm_rsp = CM_OK;
	}

	dev->rpc_client_id = CM_RPC_INVALID_CLIENT_ID;

	cm_cl_device_add_rm_done (channel, dev, CM_CL_DEV__WF_UNBIND_CNF, cm_rsp);
}


/*----------------------------------------------------------------------------*/
static  LSA_INT32  CM_LOCAL_FCT_ATTR
cm_ar_graph_connect_len (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_ar_graph_connect_len")
	LSA_UINT16  cmi_station_name_length = 0;

	LSA_INT32  len_req;
	LSA_INT32  len_cnf;

	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioap;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;

	CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

	CM_AR_GRAPH_SLOT_NR_PTR_TYPE  unique;
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

	{	CM_UPPER_MEM_U8_PTR_TYPE  cmi_station_name;
		if ( ! cm_oha_get_name_of_station (channel, & cmi_station_name_length, & cmi_station_name))
		{
			return 0;
		}
	}

	/*----------------------------------------------------------------------------
	//	see cm_arcb.h#IODConnectReq
	*/

	/* --- ARBlockReq --- */
	len_req = CLRPC_PNIO_OFFSET;
	len_req += sizeof(CM_BLK_HEADER);
	len_req += sizeof (CM_REQ_AR);
	len_req += cmi_station_name_length;

	/* --- [IOCRBlockReq*] --- */
	for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_IOCR1);

		for (CmListForeach (ioap, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE))
		{
			len_req += sizeof (CM_REQ_IOCR2);

			for (CmListForeach (iodata, & ioap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE))
			{
				len_req += sizeof (CM_REQ_IOCR3);
			}

			len_req += sizeof (CM_REQ_IOCR4);

			for (CmListForeach (iocs, & ioap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE))
			{
				len_req += sizeof (CM_REQ_IOCR5);
			}
		}
	}

	/* --- [AlarmCRBlockReq] --- */
	len_req += sizeof (CM_BLK_HEADER);
	len_req += sizeof (CM_REQ_ALCR);

	/* --- [ExpectedSubmoduleBlockReq*] --- */
	/* for all unique slot_nr */
	for (CmListForeach(unique, &argr->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE))
	{
		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_API);

			/* search all ap's which contains the unique->slot_nr */
		for (CmListForeach(ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			mod = cm_ar_graph_find_module (ap, unique->slot_nr);
			if (is_null (mod)) {
				continue;
			}
			len_req += sizeof (CM_REQ_MOD);

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				len_req += sizeof (CM_REQ_SUB);

				switch (sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
				case CM_SUB_PROP_TYPE_OUTPUT:
					len_req += sizeof (CM_REQ_DSC);
					break;

				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					len_req += sizeof (CM_REQ_DSC);
					len_req += sizeof (CM_REQ_DSC);
					break;

				default:
					CM_FATAL ();
					break;
				}
			}
		}
	}

	/* --- [PrmServerBlock] --- // not yet */

	/* --- [MCRBlockReq*] --- */
	for (CmListForeach (mcr, & argr->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE))
	{
		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_MCR);
		len_req += mcr->provider_name_length;
	}

	/* --- [ARRPCBlockReq] --- // not possible */

	/* --- [IRInfoBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
		&& is_not_null (argr->ir_info))
	{
		LSA_UINT16 subCount = 0;

		CmListLength (&(argr->ir_info->ir_info_sub_list), &subCount);

		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_IRINFO);
		len_req += subCount * sizeof (CM_REQ_IRINFOSUB);
	}

	/* --- [SRInfoBlock] --- */
	if (is_not_null (argr->sr_info))
	{
		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_SRINFO);
	}

	/* --- [ARVendorBlockReq*] --- */
	for (CmListForeach (arvendor, & argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		len_req += sizeof (CM_BLK_HEADER);
		len_req += sizeof (CM_REQ_ARVENDOR);
		len_req += arvendor->req_data_len;
		len_req += cm_cl_calc_padding_length (arvendor->req_data_len);
	}

	/* --- [ARFSUBlock] --- */
	if ((argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0)
	{
		CM_AR_GRAPH_RECORD_PTR_TYPE ptr;
		for (CmListForeach(ptr, &argr->cm.cl.ar_connect_prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
		{
			len_req += ptr->record_data_len;
		}
	}

	/*----------------------------------------------------------------------------
	//	IODConnectRes ::=
	//		ARBlockRes,
	//			{ [IOCRBlockRes*], [AlarmCRBlockRes], [ModuleDiffBlock]
	//			, [ARRPCBlockRes], ARServerBlockRes,  [ARVendorBlockRes*]
	//			}
	*/
	len_cnf = CLRPC_PNIO_OFFSET;

	/* --- ARBlockRes --- */
	len_cnf += sizeof (CM_BLK_HEADER);
	len_cnf += sizeof (CM_RSP_AR);

	/* --- [IOCRBlockRes*] --- */
	for (CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		len_cnf += sizeof (CM_BLK_HEADER);
		len_cnf += sizeof (CM_RSP_IOCR);
	}

	/* --- [AlarmCRBlockRes] --- */
	len_cnf += sizeof (CM_BLK_HEADER);
	len_cnf += sizeof (CM_RSP_ALCR);

	/* --- [ModuleDiffBlock] --- */
	/* max-len .. for every submodule one entry */
	/* BlockHeader, NumberOfAPIs,
		(API, NumberOfModules,
			(SlotNumber, ModuleIdentNumber, ModuleState, NumberOfSubmodules,
				(SubslotNumber, SubmoduleIdentNumber, SubmoduleState)*)*)*


		note: cm-server assume optimum hierarchy size
	*/
	len_cnf += sizeof (CM_BLK_HEADER);
	len_cnf += sizeof (CM_RSP_DIFF);
	for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		len_cnf += sizeof (CM_RSP_API);

		for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			len_cnf += sizeof (CM_RSP_MOD);

			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				len_cnf += sizeof (CM_RSP_SUB);
			}
		}
	}

	/* --- [ARRPCBlockRes] --- // not possible */

	/* --- [ARServerBlockRes] --- */
	if (argr->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED)
	{
		len_cnf += sizeof (CM_BLK_HEADER);
		len_cnf += sizeof (CM_RSP_ARSERVER);
		len_cnf += 240; /* max StationNameLength */
	}

	/* --- [ARVendorBlockRes*] --- */
	/* see part2: "At least an ARVendorBlockRes size of 512 octets for each used ARVendorBlockReq at the connect request" */
	for (CmListForeach (arvendor, & argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		len_cnf += 512;
	}

	/*----------------------------------------------------------------------------*/
	if (len_req > len_cnf)
	{
		return len_req;
	}
	return len_cnf;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_connect (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_connect")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  marshalled_len;
	LSA_INT32  alloc_len;
	CM_CL_DEVICE_PTR_TYPE  dev;
	CM_CL_DOWN_INFO_TYPE  dinfo;

/* VV: use oha_channel.oha.nos.length */
	alloc_len = cm_ar_graph_connect_len (channel, argr);
	if (alloc_len >= 0xFFFF || alloc_len <= 0)
	{
		CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "alloc_len(%u) >= 0xFFFF", alloc_len);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	CM_COUNTER_INCREMENT (argr->cm.cl.connect_counter);

	dev = argr->cm.cl.dev;
	CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): rpc_client_id(%08x) alloc_len(%u)"
		, argr->device_nr, argr->ar_nr, dev->rpc_client_id, alloc_len
		);

	cm_rpc_alloc_call_rqb (channel, (LSA_UINT16)alloc_len, & rpc);
	if (is_null (rpc)) {
		CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!rpc");

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* marshal to PNIO-net-Format */
	marshalled_len = cm_cl_marshal_connect (channel
		, rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET
		, (LSA_UINT16) (alloc_len - CLRPC_PNIO_OFFSET)
		, argr
		);
	CM_ASSERT (CLRPC_PNIO_OFFSET + marshalled_len <= alloc_len);
	if (marshalled_len == 0)
	{
		cm_rpc_free_call_rqb (channel, & rpc);

		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONNECT));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}


		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONNECT;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->ndr_len			= CLRPC_PNIO_OFFSET + marshalled_len;
/*	rpc->args.cl.call->ndr_data[0]		= done with marshal */

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_connect_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_rpc_connect_done")
	LSA_UINT16  err12;
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	CM_CL_DOWN_INFO_TYPE  dinfo;
	CLRPC_UPPER_CL_CALL_PTR_TYPE  call = rpc->args.cl.call;

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_CONNECT_CNF);

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_CONNECT
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): rpc-connect-done, rsp(%u) nca_status(%u) ndr_len(%u) pnio_status(0x%x)"
		, argr->device_nr, argr->ar_nr
		, lower_rsp, nca_status, call->ndr_len, pnio_status
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): cm_cl_rpc_connect_done: pnio_status(0x%x)"
			, argr->device_nr, argr->ar_nr, pnio_status
			);
		CM_CL_DINFO_SET2  (&dinfo, CM_AR_REASON_CONNECT, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

#ifdef CM_MESSAGE
/* VV: should return (part of) pnio_status like ModuleDiffBlock does */
#endif /* CM_MESSAGE */

	err12 = cm_cl_parse_connect_response (argr, call->ndr_data + CLRPC_PNIO_OFFSET, call->ndr_len - CLRPC_PNIO_OFFSET, LSA_FALSE);
	if (err12 != 0)
	{
		if (CM_PNIO_ERR_FETCH_CODE_1 (err12) == 0) {
			pnio_status = CM_PNIO_FAULTY_BlockType (CM_PNIO_ERR_CODE_CONNECT); /* compatible */
		}
		else {
			pnio_status = CM_PNIO_FAULTY_IODConnectRes (err12);
		}
		CM_CL_DINFO_SET  (&dinfo, CM_AR_REASON_CONNECT, CM_ERROR_CAUSE_PNIO_CONFIGURATION, pnio_status);
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* trigger next step */
	cm_cl_arsm (argr);
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ar_appl_ready_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,

	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	CM_RPC_MEM_U8_PTR_TYPE  dp,
	LSA_INT32  left,

	LSA_UINT16  block_type,
	LSA_UINT16  control_properties,
	LSA_UINT16  alarm_sequence_number
	)
{
	CM_LOG_FCT ("cm_cl_ar_appl_ready_ind")
	LSA_UINT16  rc;

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_PRM: /* PrmEnd.cnf not seen yet */
	case CM_DST_WF_APPL_READY_IND:
		{
		LSA_UINT16  prmmode = cm_cl_prm_mode(argr);
		LSA_BOOL  ar_is_up = cm_ar_com_is_up(argr);
		LSA_BOOL  start_predata_timer = LSA_FALSE; /* 900548(RT AdvancedStartup does a needlessly PRE_INDATA.IND) / decision: also for Advanced-IRT */

		if (argr->cm.cl.applrdy_state == CM_CL_APPL_READY_STATE_UP
		||	prmmode == CM_CL_PRMMODE_IDLE)
		{
			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) ar(%u): ApplicationReady.ind: rerun in state(%u) prmmode(%u)"
				, argr->device_nr, argr->ar_nr, argr->cm.cl.state, prmmode
				);
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
			return;
		}

		if ((prmmode == CM_CL_PRMMODE_CONNECT    &&  block_type != CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ)
		||	(prmmode == CM_CL_PRMMODE_PLUG       &&  block_type != CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ)
		||	(prmmode == CM_CL_PRMMODE_AR         &&  block_type != CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ)
		||	(prmmode == CM_CL_PRMMODE_SUBMODULE  &&  block_type != CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ))
		{
			CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): ApplicationReady.ind: block_type(0x%x) and prmmode(%u) mismatch, state(%u) wf0(0x%x) wf1(0x%x)"
				, argr->device_nr, argr->ar_nr, block_type, prmmode
				, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
				);
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (0/*BlockType*/)));
			return;
		}

		CM_CL_TRACE_09 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): ApplicationReady.ind  state(%u) applrdy_state(%u) wf0(0x%x) wf1(0x%x) prmmode(%u) block_type(0x%x) ar_is_up(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state, argr->cm.cl.applrdy_state
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1], prmmode, block_type, ar_is_up
			);

		switch (cm_cl_prm_mode (argr))
		{
		case CM_CL_PRMMODE_CONNECT:
				/* check this: how long must we support V2 (means SubmoduleState.FormatIndicator == 0) ? */
				/* use this function also within cm_cl_marshal_connect() */
			if (argr->cm.cl.dev->dev_type == CM_CL_DEV_TYPE_V3)
			{
				cm_ar_graph_set_ist_to_proper (argr);
			}

			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT
			||	argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3
			||	argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				if ((control_properties & CM_CONTROL_PROP_READY_FOR_RTC3) != 0)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "dev(%u) ar(%u): ControlBlockProperties not 0", argr->device_nr, argr->ar_nr);
					cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (9/*ControlBlockProperties*/)));
					return;
				}
			}

			if (argr->cm.cl.sumode == CM_SUMODE_AdvRT)
			{
				if (! ar_is_up)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "dev(%u) ar(%u): ApplicationReady.ind: AR not up, start PreData-Timeout", argr->device_nr, argr->ar_nr
						);
					start_predata_timer = LSA_TRUE;
				}
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_AdvRTClass3)
			{
				/* our CMSV has implemented norm part 6 Figure A.5 ?Principle of the data evaluation during startup (Special case: Isochronous mode application)
				// this means ar_is_up is never true during ApplReady.ind */
				if (! ar_is_up)
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "dev(%u) ar(%u): ApplicationReady.ind: AR not up, start PreData-Timeout", argr->device_nr, argr->ar_nr
						);
						start_predata_timer = LSA_TRUE;
				}
				else
				{
					CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev(%u) ar(%u): ApplicationReady.ind: AR up, no PreData.ind nesessary", argr->device_nr, argr->ar_nr
						);
				}
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRT)
			{
				;
			}
			else if (argr->cm.cl.sumode == CM_SUMODE_LegacyRTClass3)
			{
				if ((control_properties & CM_CONTROL_PROP_READY_FOR_RTC3) == 0)
				{
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3);
					CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_DO_PRE_DATA_IND);
				}
			}
			break;

		case CM_CL_PRMMODE_PLUG:
			if (is_not_null (argr->cm.cl.prm_alarm) /* may happen in case of rerun */
			&&	argr->cm.cl.prm_alarm->args.channel.alarm->alarm_sequence != alarm_sequence_number)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u) ar(%u): ApplicationReady.ind: prm.alarm_seq_nr(%u) != appl_rdy.alarm_seq_nr(%u)"
					, argr->device_nr, argr->ar_nr
					, argr->cm.cl.prm_alarm->args.channel.alarm->alarm_sequence, alarm_sequence_number
					);
				cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockPlug (7/*alarm_sequence*/)));
				return;
			}
			break;

		case CM_CL_PRMMODE_AR:
			cm_ar_graph_set_ist_to_proper (argr); /* 1078181 */
			break;

		case CM_CL_PRMMODE_SUBMODULE:
			break;

		case CM_CL_PRMMODE_RETURN_OF_SUB:
		default:
			CM_FATAL();
			break;
		}

		rc = cm_cl_parse_connect_response (argr, dp, left, LSA_TRUE);
		if (rc != 0)
		{
			CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ar(%u): ApplicationReady.ind: cannot parse response, rc(%u) prmmode(%u)"
				, argr->device_nr, argr->ar_nr, rc, prmmode
				);
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (rc));
			return;
		}

		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);

		cm_cl_prm_ardy_timeout_stop (argr);
		argr->cm.cl.applrdy_state = CM_CL_APPL_READY_STATE_UP;
		if (start_predata_timer)
		{
			cm_cl_prm_ardy_timeout_start (argr, CM_CL_ARDY_TIMEOUT__PreDataInd);
		}
		}

		if (argr->cm.cl.state == CM_DST_PRM)
		{
			/* cm_cl_arsm(): too early, only keep ready state */
		}
		if (argr->cm.cl.state == CM_DST_WF_APPL_READY_IND)
		{
			cm_cl_arsm (argr);
		}
		break;

	case CM_DST_WF_NEW_DATA:
	case CM_DST_IN_DATA:
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) ar(%u): rerun in state(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		break;

	case CM_DST_PASSIVE:
	default:
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) ar(%u): wrong state(%u) wf0(0x%x) wf1(0x%x) or go passive in meantime, reject rpc"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		if (block_type == CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ) {
			cm_pnio_c_control_rsp (channel
				, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect(6/*SessionKey*/))
				);
		}
		else {
			cm_pnio_c_control_rsp (channel
				, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockPlug(6/*SessionKey*/))
				);
		}
		break;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_ar_ready_for_rtc3_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT16  block_type
	)
{
	CM_LOG_FCT ("cm_cl_ar_ready_for_rtc3_ind")
	LSA_UNUSED_ARG (block_type);

	switch (argr->cm.cl.usr_state)
	{
	case CM_CL_AR_INDATA:
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): rerun in state(%u)"
			, argr->device_nr, argr->ar_nr, argr->cm.cl.state
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		return;

	case CM_CL_AR_PASSIVE:
	case CM_CL_AR_WF_DOWN:
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): go passive in meantime, reject rpc; state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/)));
		return;

	case CM_CL_AR_WF_IN:
		break;

	default:
		CM_FATAL1 (argr->cm.cl.usr_state);
		return;
	}


	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u): ReadyForRTClass3: state(%u) applrdy_state(%u) wf0(0x%x) wf1(0x%x)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.cl.state, argr->cm.cl.applrdy_state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
		);

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3))
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): got ReadyForRTClass3 in state(%u): wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_READY_FOR_RTC3);

		cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		rpc = LSA_NULL;
	}
	else
	{
		/* in case of rerun */
		CM_ASSERT (is_not_null (rpc));
	}

		/* kick state-machine */
	switch (argr->cm.cl.state)
	{
	case CM_DST_WF_SCHEDULE: /* case: down -> re-schedule -> rtc3.ind  note: may occur because session_nr will increment later in cm_cl_rpc_connect() */
	case CM_DST_NARE:
	case CM_DST_REBIND:
	case CM_DST_CONNECT:
	case CM_DST_COM_START:
	case CM_DST_COM_ACTIVATE:
		if (is_not_null (rpc)) /* too late, wrong session */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_FAULTY_IOXControlRes (CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/)));
		}
		break;

	case CM_DST_PRM: /* case: plug.ind -> prm_write.req -> rtc3.ind & delayed PrmEnd.cnf*/
	case CM_DST_WF_APPL_READY_IND:
		if (is_not_null (rpc)) /* rerun */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		}
		/* do not call cm_cl_arsm () */
		break;

	case CM_DST_WF_NEW_DATA:
		if (is_not_null (rpc)) /* rerun */
		{
			cm_pnio_c_control_rsp (channel, argr, rpc, CM_PNIO_ERR_NONE);
		}
		/* trigger */
		cm_cl_arsm (argr);
		break;

	default:
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) ar(%u): state(%u) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.cl.state, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_FATAL1 (argr->cm.cl.state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_rpc_prepare_control_rsp (
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT16  block_type
	)
{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_INT  left;

		/* set rpc.control.rsp to done */
	dp = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.sv.call->ndr_len - CLRPC_PNIO_OFFSET;

	switch (block_type) {
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
		cm_put_BLOCK_HEADER_type (CM_BLOCK_TYPE_IOXBLOCK_CONN_RSP);
		break;
	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		cm_put_BLOCK_HEADER_type (CM_BLOCK_TYPE_IOXBLOCK_PLUG_RSP);
		break;
	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		cm_put_BLOCK_HEADER_type (CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_RSP);
		break;
	default:
		CM_ASSERT (0); /* checked previously */
		return;
	}

		/* marshal answer */
	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

	cm_put_BLK_CTRL_control_command (CM_CONTROL_COMMAND_DONE);
	cm_adjust (dp, left, sizeof (CM_BLK_CTRL));
	LSA_UNUSED_ARG (left); /* lint */

	rpc->args.sv.call->ndr_len = dp - rpc->args.sv.call->ndr_data;
}


/*------------------------------------------------------------------------------
// handle incoming rpc_call::pnio_c_control
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pnio_c_control_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_pnio_c_control_ind")
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  cm_get_assert_line = 0;
	LSA_UINT16  cm_get_assert_error = 0;

	LSA_UINT16  block_type, block_length, block_version;
	CLRPC_UUID_TYPE  ar_uuid;
	CM_AR_GRAPH_PTR_TYPE  argr = LSA_NULL;
	LSA_UINT16  session_key, alarm_sequence_number, control_command, control_properties;

	dp = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.sv.call->ndr_len - CLRPC_PNIO_OFFSET;

	CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "IOXControl.ind: rpc-rqb(0x%x) opnum(%u) ndr_len(%u)"
		, rpc, rpc->args.sv.call->opnum, rpc->args.sv.call->ndr_len
		);

		/* parse block-header */
	block_type = 0;
	cm_get_assert2 (left >= sizeof (CM_BLK_HEADER), CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));

	cm_get_BLOCK_HEADER_type (& block_type);
	cm_get_BLOCK_HEADER_length (& block_length);
	cm_get_BLOCK_HEADER_version (& block_version);

	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

	switch (block_type)
	{
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		cm_get_assert2 (block_length == 28, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		cm_get_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ControlBlockConnect (2/*BlockVersionHigh*/));
		cm_get_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ControlBlockConnect (3/*BlockVersionLow*/));

			/* parse control-block */
		cm_get_assert2 (left >= sizeof (CM_BLK_CTRL), CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		cm_get_assert2 (block_length == 28, CM_PNIO_ERR_ControlBlockConnect (1/*BlockLength*/));
		cm_get_assert2 ((block_version & 0xFF00) == 0x0100, CM_PNIO_ERR_ControlBlockPlug (2/*BlockVersionHigh*/));
		cm_get_assert2 ((block_version & 0x00FF) == 0x0000, CM_PNIO_ERR_ControlBlockPlug (3/*BlockVersionLow*/));

			/* parse control-block */
		cm_get_assert2 (left >= sizeof (CM_BLK_CTRL), CM_PNIO_ERR_ControlBlockPlug (1/*BlockLength*/));
		break;

	default:
		cm_get_assert_error_set (CM_PNIO_ERR_ControlBlockConnect (0/*BlockType*/)); /* BlockConnect */
		goto cm_get_assert_failed;
	}

	/* left is checked above */

	cm_get_BLK_CTRL_ar_uuid (& ar_uuid);
	cm_get_BLK_CTRL_session_key (& session_key);
	cm_get_BLK_CTRL_alarm_sequence_number (& alarm_sequence_number);
	cm_get_BLK_CTRL_control_command (& control_command);
	cm_get_BLK_CTRL_control_properties (& control_properties);

	cm_put_BLK_CTRL_control_command (0);	/* !!! set response */

	cm_adjust (dp, left, sizeof (CM_BLK_CTRL));

	CM_CL_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
		, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
		, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
		, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
		);
	CM_CL_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "session_key(%u) block_type(0x%x) ctrl_command(0x%x) ctrl_prop(0x%x) alarm_seq_nr(%u)"
		, session_key, block_type, control_command, control_properties, alarm_sequence_number
		);

	argr = cm_cl_ar_from_ar_uuid (channel, & ar_uuid, LSA_TRUE);

	switch (block_type)
	{
	case CM_BLOCK_TYPE_IOXBLOCK_CONN_REQ:
		{
		cm_get_assert2 (is_not_null (argr), CM_PNIO_ERR_ControlBlockConnect (5/*ARUUID*/));
		cm_get_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/));
		cm_get_assert2 (control_command == CM_CONTROL_COMMAND_APPLICATION_READY, CM_PNIO_ERR_ControlBlockConnect (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_appl_ready_ind (channel, argr
			, rpc, dp, left
			, block_type, control_properties, alarm_sequence_number
			);
		}
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_PLUG_REQ:
		{
		cm_get_assert2 (is_not_null (argr), CM_PNIO_ERR_ControlBlockPlug (5/*ARUUID*/));
		cm_get_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockPlug (6/*SessionKey*/));
		cm_get_assert2 (control_command == CM_CONTROL_COMMAND_APPLICATION_READY, CM_PNIO_ERR_ControlBlockPlug (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_appl_ready_ind (channel, argr
			, rpc, dp, left
			, block_type, control_properties, alarm_sequence_number
			);
		}
		break;

	case CM_BLOCK_TYPE_IOXBLOCK_R4RTC3_REQ:
		{
		cm_get_assert2 (is_not_null (argr), CM_PNIO_ERR_ControlBlockConnect (5/*ARUUID*/));
		cm_get_assert2 (session_key == argr->cm.cl.connect_counter, CM_PNIO_ERR_ControlBlockConnect (6/*SessionKey*/));
		cm_get_assert2 (control_command == CM_CONTROL_COMMAND_READY_FOR_RTC3, CM_PNIO_ERR_ControlBlockConnect (8/*ControlCommand*/));

		cm_rpc_prepare_control_rsp (rpc, block_type);

		cm_cl_ar_ready_for_rtc3_ind (channel, argr, rpc, block_type);
		}
		break;

	default:
		CM_ASSERT (0); /* checked previously */
		return;
	}

	return;

cm_get_assert_failed:
	{
	LSA_UINT32  pnio_err = CM_PNIO_FAULTY_IOXControlRes (cm_get_assert_error);
	CM_CL_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u) ar(%u)  assertion in line(%u): block_type(0x%x) pnio_err(0x%x) left(%u)"
		, is_null(argr) ? 0 : argr->device_nr, is_null(argr) ? 0 : argr->ar_nr
		, cm_get_assert_line
		, block_type, pnio_err, left
		);
	LSA_UNUSED_ARG (cm_get_assert_line);
	cm_pnio_c_control_rsp (channel, argr, rpc, pnio_err);
	}
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pnio_c_control_rsp (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc,
	LSA_UINT32  pnio_code
	)
{
	CM_LOG_FCT ("cm_pnio_c_control_rsp")

	LSA_UINT32  trace_idx = is_not_null (argr) ? argr->trace_idx : 0;

	CM_ASSERT (pnio_code == CM_PNIO_ERR_NONE || (pnio_code & 0xFFFFFF00) != 0);

		/* ok response */
	if (pnio_code == CM_PNIO_ERR_NONE) {
		rpc->args.sv.call->pnio_status = CM_PNIO_ERR_NONE;
		CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
	}

		/* error response */
	else {
		rpc->args.sv.call->pnio_status = pnio_code;

		LSA_UNUSED_ARG (trace_idx);
		CM_CL_TRACE_01 (trace_idx, LSA_TRACE_LEVEL_UNEXP, "pnio_status(0x%x)", rpc->args.sv.call->pnio_status);

		rpc->args.sv.call->ndr_len = CLRPC_PNIO_OFFSET;
		CM_RPC_SET_RESPONSE (rpc, CLRPC_OK);
		cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0);
	}
}


/*------------------------------------------------------------------------------
// release an AR
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_release (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_release")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;
	LSA_UINT16  ndr_len;
	LSA_UINT16  cm_put_assert_line = 0;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	ndr_len = CLRPC_PNIO_OFFSET + sizeof (CM_BLK_HEADER) + sizeof (CM_BLK_REL);

	CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) ar(%u): cm_cl_rpc_release, rpc_client_id(%08x) opnum(%u) ndr_len(%u)"
		, argr->device_nr
		, argr->ar_nr
		, dev->rpc_client_id
		, CLRPC_PNIO_OPNUM_RELEASE
		, ndr_len
		);

	cm_rpc_alloc_call_rqb (channel, ndr_len, & rpc);
	if (is_null (rpc))
	{
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

		/* marshal to PNIO-Net-Format */
	{
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	LSA_UINT16  release_command, release_properties;

	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = ndr_len - CLRPC_PNIO_OFFSET;

	cm_put_assert (left >= sizeof (CM_BLK_HEADER));

	cm_put_BLOCK_HEADER_type	(CM_BLOCK_TYPE_RELBLOCK_REQ);
	cm_put_BLOCK_HEADER_length	(sizeof(LSA_UINT16) + sizeof (CM_BLK_REL));
	cm_put_BLOCK_HEADER_version	(0x0100); /* fixed V1.0 because not in CM_ARCB_PDU_VERSION_TYPE */

	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


	cm_put_assert (left >= sizeof (CM_BLK_REL));

	release_command = CM_CONTROL_COMMAND_RELEASE;
	release_properties = 0;

	cm_put_BLK_REL_reserved1 (0);
	cm_put_BLK_REL_ar_uuid (& argr->ar.ar_uuid);
	cm_put_BLK_REL_session_key (argr->cm.cl.connect_counter);
	cm_put_BLK_REL_reserved2 (0);
	cm_put_BLK_REL_release_command (release_command);
	cm_put_BLK_REL_release_properties (release_properties);

	cm_adjust (dp, left, sizeof (CM_BLK_REL));
	CM_ASSERT (left == 0);
	LSA_UNUSED_ARG (dp); /* lint */
	LSA_UNUSED_ARG (left); /* lint */
	}

		/* fill rpc-call */
	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_RELEASE;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->ndr_len			= ndr_len;
/*	rpc->args.cl.call->ndr_data[0]		= see marshal */

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return;

cm_put_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_put_assert_line
		);
	LSA_UNUSED_ARG (cm_put_assert_line);

	cm_rpc_free_call_rqb (channel, & rpc);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_MEM));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_release_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
)
{
	CM_LOG_FCT ("cm_cl_rpc_release_done")
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_RELEASE_CNF);

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_CONTROL
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): cm_cl_rpc_release_done, pnio_status(0x%x)"
			, argr->device_nr, argr->ar_nr, pnio_status
			);
		CM_ASSERT (cm_ar_com_is_down (argr));
	}
	else
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) ar(%u): cm_cl_rpc_release_done."
			, argr->device_nr, argr->ar_nr
			);

			/* parse response */
		/* ignore anything, want to go down ... */
		CM_ASSERT (cm_ar_com_is_down (argr));
	}


		/* already down, see ar_passivate() */
	cm_cl_arsm (argr);
}


/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_control (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_BOOL  plug,
	LSA_UINT16  alarm_seq_nr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_control")
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc = LSA_NULL;
	LSA_UINT16  ndr_len;
	LSA_UINT16  cm_put_assert_line = 0;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UINT16  block_type;
	LSA_UINT16  control_cmd = CM_CONTROL_COMMAND_PRM_END;
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;

	ndr_len = CLRPC_PNIO_OFFSET + sizeof (CM_BLK_HEADER) + sizeof (CM_BLK_CTRL);

	cm_rpc_alloc_call_rqb (channel, ndr_len, & rpc);
	if (is_null (rpc))
	{
		CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): no mem, do down, plug(%u) alarm_seq_nr(%u)"
			, argr->device_nr, argr->ar_nr, plug, alarm_seq_nr
			);
		CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
		cm_cl_arsm_down (argr, &dinfo);
		return;
	}

	rpc->args.cl.call->client_id		= dev->rpc_client_id;
	rpc->args.cl.call->opnum			= CLRPC_PNIO_OPNUM_CONTROL;
	rpc->args.cl.call->semantics		= CLRPC_SEMANTICS_IDEMPOTENT;
	rpc->args.cl.call->pnio_signature	= 1; /* see cm_rpc_get_IPNIO_Device() */
	rpc->args.cl.call->appl_timeout1sec	= channel->usr.cl.appl_timeout1sec;

	rpc->args.cl.call->ndr_len			= ndr_len;
	rpc->args.cl.call->ndr_data[0]		= 0;

		/* ? two functions */
	if (plug) {
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) PrmEnd(Plug).req rpc_client_id(%08x) opnum(%u) control_cmd(%u) ndr_len(%u)"
			, argr->device_nr, argr->ar_nr
			, dev->rpc_client_id, CLRPC_PNIO_OPNUM_CONTROL, control_cmd, ndr_len
			);

		block_type = CM_BLOCK_TYPE_IODBLOCK_PLUG_REQ;
	}
	else {
		CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) PrmEnd(Connect).req rpc_client_id(%08x) opnum(%u) control_cmd(%u) ndr_len(%u)"
			, argr->device_nr, argr->ar_nr
			, dev->rpc_client_id, CLRPC_PNIO_OPNUM_CONTROL, control_cmd, ndr_len
			);

		block_type = CM_BLOCK_TYPE_IODBLOCK_CONN_REQ;
		alarm_seq_nr = 0;
	}

		/* marshal */
	{
		CM_RPC_MEM_U8_PTR_TYPE  dp;
		LSA_UINT32  left;

		dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
		left = ndr_len - CLRPC_PNIO_OFFSET;

		cm_put_assert (left >= sizeof (CM_BLK_HEADER));

		cm_put_BLOCK_HEADER_type	(block_type);
		cm_put_BLOCK_HEADER_length	(sizeof(LSA_UINT16) + sizeof (CM_BLK_CTRL));
		cm_put_BLOCK_HEADER_version	(0x0100);

		cm_adjust (dp, left, sizeof (CM_BLK_HEADER));


		cm_put_assert (left >= sizeof (CM_BLK_CTRL));

		cm_put_BLK_CTRL_reserved1 (0);
		cm_put_BLK_CTRL_ar_uuid (& argr->ar.ar_uuid);
		cm_put_BLK_CTRL_session_key (argr->cm.cl.connect_counter);
		cm_put_BLK_CTRL_alarm_sequence_number (alarm_seq_nr);
		cm_put_BLK_CTRL_control_command (control_cmd);
		cm_put_BLK_CTRL_control_properties (0);	/* fixed data */

		cm_adjust (dp, left, sizeof (CM_BLK_CTRL));
		CM_ASSERT (left == 0);
		LSA_UNUSED_ARG (dp); /* lint */
		LSA_UNUSED_ARG (left); /* lint */
	}

	CM_ASSERT (rpc->args.cl.call->ndr_len <= rpc->args.cl.call->alloc_len);

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CALL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
	return;

cm_put_assert_failed:

	CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "buffer too small, assertion failed in line(%u)"
		, cm_put_assert_line
		);
	LSA_UNUSED_ARG (cm_put_assert_line);

	cm_rpc_free_call_rqb (channel, & rpc);
	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_MEM, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*----------------------------------------------------------------------------*/
static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_control_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT ("cm_cl_rpc_control_done")
	LSA_UINT16  lower_rsp, error_cause;
	LSA_UINT32  pnio_status, nca_status;
	LSA_UINT16  cm_get_assert_line = 0;
	LSA_UINT16  cm_get_assert_error = 0;
	LSA_BOOL  is_prm_begin = LSA_FALSE;
	CM_CL_DOWN_INFO_TYPE  dinfo;

	LSA_UINT16  block_type, block_length, block_version;
	CM_RPC_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  left;
	CLRPC_UUID_TYPE  ar_uuid;
	LSA_UINT16  session_key, control_cmd, control_properties;

	cm_cl_rpc_to_addr_info (rpc, CM_PNIO_ERR_CODE_CONTROL
		, & lower_rsp, & error_cause, & pnio_status, & nca_status
		);

	CM_CL_TRACE_06 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) rsp(%u) pnio_status(0x%x) nca_status(0x%x) ndr_len(%u)"
		, argr->device_nr, argr->ar_nr
		, lower_rsp, pnio_status, nca_status, rpc->args.cl.call->ndr_len
		);

	if (pnio_status != CM_PNIO_ERR_NONE)
	{
		CM_CL_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): cm_cl_rpc_control_done: pnio_status(0x%x) wf0(0x%x) wf1(0x%x)"
			, argr->device_nr, argr->ar_nr, pnio_status
			, argr->cm.cl.wait_for[0], argr->cm.cl.wait_for[1]
			);
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF); /* one of this two is set .. */
		CM_CL_WF_CLEAR_IF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

		CM_CL_DINFO_SET2  (&dinfo, CM_AR_REASON_CONTROL, error_cause, pnio_status, nca_status);
		cm_cl_arsm_down (argr, &dinfo);

		cm_cl_prm_trigger (argr, 0);
		return;
	}


	/* parse answer */
	dp = rpc->args.cl.call->ndr_data + CLRPC_PNIO_OFFSET;
	left = rpc->args.cl.call->ndr_len - CLRPC_PNIO_OFFSET;

	cm_get_assert (left >= sizeof (CM_BLK_HEADER));

	cm_get_BLOCK_HEADER_type (& block_type);
	cm_get_BLOCK_HEADER_length (& block_length);
	cm_get_BLOCK_HEADER_version (& block_version);

	cm_adjust (dp, left, sizeof (CM_BLK_HEADER));

	cm_get_assert (block_length == sizeof(LSA_UINT16) + sizeof (CM_BLK_CTRL));
	cm_get_assert (block_version == 0x0100); /* only version 1.0 */

	if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_END_CNF);

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_CONNECT:
			cm_get_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_CONN_RSP);
			break;

		case CM_CL_PRMMODE_PLUG:
			cm_get_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_PLUG_RSP);
			break;

		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
			cm_get_assert (block_type == CM_BLOCK_TYPE_IODBLOCK_CONN_RSP);
			break;

		case CM_CL_PRMMODE_RETURN_OF_SUB:
		default:
			cm_get_assert_error_set (0/*pgm error*/);
			goto cm_get_assert_failed;
		}
	}
	else if (CM_CL_WF_IS_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF))
	{
		CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_PRM_BEGIN_CNF);

		is_prm_begin = LSA_TRUE;

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
			cm_get_assert (block_type == CM_BLOCK_TYPE_PRM_BEGIN_RSP);
			break;

		case CM_CL_PRMMODE_PLUG:
		case CM_CL_PRMMODE_CONNECT:
		case CM_CL_PRMMODE_RETURN_OF_SUB:
		default:
			cm_get_assert_error_set (0/*pgm error*/);
			goto cm_get_assert_failed;
		}
	}
	else
	{
		cm_get_assert_error_set (0/*pgm error*/);
		goto cm_get_assert_failed;
	}


	cm_get_assert (left >= sizeof (CM_BLK_CTRL));

	cm_get_BLK_CTRL_ar_uuid (& ar_uuid);
	cm_get_BLK_CTRL_session_key (& session_key);
	cm_get_BLK_CTRL_control_command (& control_cmd);
	cm_get_BLK_CTRL_control_properties (& control_properties);

	cm_adjust (dp, left, sizeof (CM_BLK_CTRL));

	cm_get_assert (cm_uuid_equal (& ar_uuid, & argr->ar.ar_uuid));
	cm_get_assert (session_key == argr->cm.cl.connect_counter);
	cm_get_assert (control_cmd == CM_CONTROL_COMMAND_DONE);
	LSA_UNUSED_ARG (control_properties); /* shall set and not be tested */

	CM_CL_TRACE_12 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "control_cmd(%x) ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, control_cmd
		, ar_uuid.time_low, ar_uuid.time_mid, ar_uuid.time_hi_and_version
		, ar_uuid.clock_seq_hi_and_reserved, ar_uuid.clock_seq_low
		, ar_uuid.node[0], ar_uuid.node[1], ar_uuid.node[2]
		, ar_uuid.node[3], ar_uuid.node[4], ar_uuid.node[5]
		);

	if (is_prm_begin)
	{
		LSA_UINT16  rc = 1;

		switch (cm_cl_prm_mode(argr))
		{
		case CM_CL_PRMMODE_AR:
		case CM_CL_PRMMODE_SUBMODULE:
			/*optional ModuleDiffBlock*/
			rc = cm_cl_parse_connect_response (argr, dp, left, LSA_TRUE/*only ModuleDiffBlock*/);
			if (rc != 0)
			{
				CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev(%u) ar(%u): PrmBegin.Rsp: cannot parse response ModuleDiffBlock, rc(%u) prmmode(%u)"
					, argr->device_nr, argr->ar_nr, rc, cm_cl_prm_mode(argr)
					);
				cm_get_assert (rc == 0);
			}
			/* update prm-record list */
			cm_cl_prm_record_list_update (argr);
			break;

		case CM_CL_PRMMODE_PLUG:
		case CM_CL_PRMMODE_CONNECT:
		case CM_CL_PRMMODE_RETURN_OF_SUB:
		default:
			cm_get_assert_error_set (0/*pgm error*/);
			goto cm_get_assert_failed;
		}
	}

		/* trigger state-machine */
	cm_cl_prm_trigger (argr, 0/*trigger*/);

	return;

cm_get_assert_failed:

	CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
		, "dev(%u) ar(%u), assertion failed in line(%u), ar goes down"
		, argr->device_nr, argr->ar_nr
		, cm_get_assert_line
		);
	LSA_UNUSED_ARG (cm_get_assert_error);

	CM_CL_DINFO_SET (&dinfo, CM_AR_REASON_PROT, CM_ERROR_CAUSE_IOC_ABORTED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_CONTROL));
	CM_CL_DINFO_SET_lnr (&dinfo, cm_get_assert_line);
	cm_cl_arsm_down (argr, &dinfo);
	return;
}


/*------------------------------------------------------------------------------
// cancel any call
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_cancel (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_LOG_FCT ("cm_cl_rpc_cancel")
	CM_CHANNEL_PTR_TYPE  channel = argr->ar_com.channel;
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc;

	cm_rpc_alloc_rqb (channel, & rpc);
	if (is_null (rpc))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) ar(%u): no rpc-rqb-mem for rpc-cancel, ignore this, must timeout calls"
			, argr->device_nr, argr->ar_nr
			);
		return;
	}

	rpc->args.cl.cancel->client_id = argr->cm.cl.dev->rpc_client_id;

	CM_CL_WF_SET (argr->cm.cl.wait_for, CM_CL_WF_RPC_CANCEL_CNF);

	cm_rpc_request_lower (channel, rpc
		, CLRPC_OPC_CL_CANCEL, CM_CAST_U16(argr->device_nr | CM_CL_DEV_SUBSYS_DEV), argr->ar_nr
		);
}

/*----------------------------------------------------------------------------*/

static  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_cancel_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
	)
{
	CM_LOG_FCT("cm_cl_rpc_cancel_done")

	cm_rpc_free_rqb (argr->ar_com.channel, &rpc);

	CM_CL_WF_CLEAR (argr->cm.cl.wait_for, CM_CL_WF_RPC_CANCEL_CNF);
	cm_cl_arsm (argr);
}


/*------------------------------------------------------------------------------
// client rpc lower done
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_cl_rpc_lower_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_RPC_LOWER_RQB_PTR_TYPE  rpc
)
{
	CM_LOG_FCT("cm_cl_rpc_lower_done")
	CM_CL_DEVICE_PTR_TYPE  dev;
	CM_AR_GRAPH_PTR_TYPE  argr;

	if (CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK || CM_RPC_GET_RESPONSE (rpc) == CLRPC_OK_CANCELLED) {
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rqb(%08x) opc(%u) rsp(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			);
	}
	else {
		CM_CL_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rqb(%08x) opc(%u) rsp(%u)"
			, rpc, CM_RPC_GET_OPCODE (rpc), CM_RPC_GET_RESPONSE (rpc)
			);
	}

	switch (CM_RPC_GET_OPCODE (rpc))
	{
	case CLRPC_OPC_SV_REGISTER_IF_OBJ:
		cm_cl_rpc_register_if_obj_done (channel, & rpc);
		break;

	case CLRPC_OPC_SV_UNREGISTER_IF_OBJ:
		cm_cl_rpc_unregister_done (channel, rpc);
		break;

	case CLRPC_OPC_SV_CALL_IND_PROVIDE:
		CM_ASSERT (CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK_CANCELLED); /* what else? */
		CM_ASSERT (channel->state == CM_CL_UNREGISTER);
		CM_CL_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "call-ind-res cancelled, freeing"
			);
		cm_rpc_free_rqb(channel, &rpc);
		break;

	case CLRPC_OPC_SV_CALL_INDICATION:
		CM_ASSERT (CM_RPC_GET_RESPONSE(rpc) == CLRPC_OK);

		/* sanity-check the call-semantics */

		if( rpc->args.sv.call->semantics != CLRPC_SEMANTICS_IDEMPOTENT ) {
			/* caller uses wrong IDL file? */
			if( rpc->args.sv.call->semantics != CLRPC_SEMANTICS_AT_MOST_ONCE ) {
				CM_CL_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "rpc-call.ind: opcum(%u) semantics(%u) not idempotent"
					, rpc->args.sv.call->opnum
					, rpc->args.sv.call->semantics
					);
				CM_RPC_SET_RESPONSE (rpc, CLRPC_ERR_FAULTED); /* not CLRPC_ERR_REJECTED, MS-RPC will retry endlessly! */
				cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
				break;
			}
			else {
				CM_CL_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "rpc-call.ind: semantics(%u) not idempotent, at-most-once is ok too"
					, rpc->args.sv.call->semantics
					);
			}
		}

			/* .ind from client to server calls */
		switch (rpc->args.sv.call->opnum) {
		case CLRPC_PNIO_OPNUM_CONTROL:
			/* RTF or MPT running: AR already down or gone, call is late (similar to device-remove) */
			cm_pnio_c_control_ind (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_READ_IMPLICIT:
			/* RTF or MPT running: read does not interfere with RTF or MPT */
			cm_pnio_c_read_implicit_ind (channel, rpc);
			break;

		case CLRPC_PNIO_OPNUM_CONNECT:
		case CLRPC_PNIO_OPNUM_RELEASE:
		case CLRPC_PNIO_OPNUM_READ:
		case CLRPC_PNIO_OPNUM_WRITE:
		default:
			CM_CL_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "opnum(%u) not handled, rejecting", rpc->args.sv.call->opnum);
			CM_RPC_SET_RESPONSE (rpc, CLRPC_ERR_FAULTED); /* not CLRPC_ERR_REJECTED, MS-RPC will retry endlessly! */
			cm_rpc_request_lower (channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, 0, 0); /* is-a-response: no need to set user_id */
			break;
		}
		break;

	case CLRPC_OPC_CL_BIND:
		{
		LSA_UINT16  device_nr = CM_CL_GET_DEVICE_NR (CM_RPC_GET_USER_ID_DEV (rpc));

		switch (CM_CL_GET_SUBSYS (CM_RPC_GET_USER_ID_DEV (rpc))) {
		case CM_CL_DEV_SUBSYS_DEV:
			dev = cm_cl_dev_from_device_nr (channel, device_nr);
			if (is_null (dev)) {
				CM_RPC_LOWER_RQB_ERROR (rpc);
				break;
			}
			cm_cl_rpc_bind_done (channel, dev, rpc);
			cm_rpc_free_rqb (channel, & rpc);
			break;

#if CM_CFG_MAX_CL_IMPLICIT_READ
		case CM_CL_DEV_SUBSYS_READ_IMPL:
			{
			CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

			ri = cm_cl_ri_from_device_nr__nn (channel, device_nr);
			cm_cl_ri_rpc_bind_done (channel, ri, rpc);
			}
			break;
#endif

		default:
			CM_FATAL1 (CM_RPC_GET_USER_ID_DEV (rpc));
			break;
		}
		}
		break;

	case CLRPC_OPC_CL_REBIND:
		argr = cm_cl_ar_from_device_ar (channel,
				CM_RPC_GET_USER_ID_DEV (rpc), CM_RPC_GET_USER_ID_AR (rpc)
			);
		if (is_null (argr)) {
			CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
			break;
		}
		cm_cl_rpc_rebind_done (argr, rpc);
		cm_rpc_free_rqb (channel, & rpc);
		break;

	case CLRPC_OPC_CL_UNBIND:
		{
		LSA_UINT16  device_nr = CM_CL_GET_DEVICE_NR (CM_RPC_GET_USER_ID_DEV (rpc));

		switch (CM_CL_GET_SUBSYS (CM_RPC_GET_USER_ID_DEV (rpc))) {
		case CM_CL_DEV_SUBSYS_DEV:
			dev = cm_cl_dev_from_device_nr (channel, device_nr);
			if (is_null (dev)) {
				CM_RPC_LOWER_RQB_ERROR (rpc);
				break;
			}
			cm_cl_rpc_unbind_done (channel, dev, CM_RPC_GET_RESPONSE (rpc));
			cm_rpc_free_rqb (channel, & rpc);
			break;

#if CM_CFG_MAX_CL_IMPLICIT_READ
		case CM_CL_DEV_SUBSYS_READ_IMPL:
			{
			CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

			ri = cm_cl_ri_from_device_nr__nn (channel, device_nr);
			cm_cl_ri_rpc_unbind_done (channel, ri, rpc);
			}
			break;
#endif

		default:
			CM_FATAL1 (CM_RPC_GET_USER_ID_DEV (rpc));
			break;
		}
		}
		break;

	case CLRPC_OPC_CL_CALL:
		{
		LSA_UINT16  device_nr = CM_CL_GET_DEVICE_NR (CM_RPC_GET_USER_ID_DEV (rpc));

		switch (CM_CL_GET_SUBSYS (CM_RPC_GET_USER_ID_DEV (rpc))) {
		case CM_CL_DEV_SUBSYS_DEV:

			argr = cm_cl_ar_from_device_ar (channel, device_nr, CM_RPC_GET_USER_ID_AR (rpc));
			if (is_null (argr)) {
				CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
				break;
			}

				/* .cnf from server to client calls, see .IDL of PNIO */
			switch (rpc->args.cl.call->opnum) {
			case CLRPC_PNIO_OPNUM_CONNECT:
				cm_cl_rpc_connect_done (argr, rpc);
				cm_rpc_free_call_rqb (channel, & rpc);
				break;

			case CLRPC_PNIO_OPNUM_RELEASE:
				cm_cl_rpc_release_done (argr, rpc);
				cm_rpc_free_call_rqb (channel, & rpc);
				break;

			case CLRPC_PNIO_OPNUM_READ:
				cm_cl_record_read_done (channel, argr, rpc);
				cm_rpc_free_rqb (channel, & rpc);
				break;

			case CLRPC_PNIO_OPNUM_WRITE:
				cm_cl_record_write_done (channel, argr, rpc);
				cm_rpc_free_rqb (channel, & rpc);
				break;

			case CLRPC_PNIO_OPNUM_CONTROL:
				cm_cl_rpc_control_done (argr, rpc);
				cm_rpc_free_call_rqb (channel, & rpc);
				break;

			default:
				CM_RPC_LOWER_RQB_ERROR (rpc); /* a bug in CLRPC */
				break;
			}
			break;

#if CM_CFG_MAX_CL_IMPLICIT_READ
		case CM_CL_DEV_SUBSYS_READ_IMPL:
			{
			CM_CL_READ_IMPLICIT_PTR_TYPE  ri;

			CM_ASSERT (rpc->args.cl.call->opnum == CLRPC_PNIO_OPNUM_READ_IMPLICIT);

			ri = cm_cl_ri_from_device_nr__nn (channel, device_nr);
			cm_cl_ri_rpc_call_done (channel, ri, rpc);
			}
			break;
#endif

		default:
			CM_FATAL1 (CM_RPC_GET_USER_ID_DEV (rpc));
			break;
		}
		}
		break;

	case CLRPC_OPC_CL_CANCEL:
		argr = cm_cl_ar_from_device_ar (channel,
				CM_RPC_GET_USER_ID_DEV (rpc), CM_RPC_GET_USER_ID_AR (rpc)
			);
		if (is_null (argr)) {
			CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
			break;
		}
		cm_cl_rpc_cancel_done (argr, rpc);
		break;

	default:
		CM_RPC_LOWER_RQB_ERROR (rpc); /* should not happen */
		break;
	}
}


/*---------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
