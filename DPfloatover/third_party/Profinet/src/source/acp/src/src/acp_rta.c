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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_rta.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the rta-protokol (see IEC: APMS / APMR)                       */
/*                                                                           */
/*****************************************************************************/
#define ACP_MODULE_ID      7
#define LTRC_ACT_MODUL_ID  7

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)


/*----------------------------------------------------------------------------**

see 61158-8 v2.3:

DLPDU	::=	DestinationAddress, SourceAddress, DLSDU b, DLPDU_Padding* c, FrameCheckSequence	
DLSDU	::=	[VLAN] b, LT, SAPDU ^ FIDAPDU

	note: sizeof(DLPDU) >= 64
	b) This field should exist for UDP-RTC-PDU and UDP-RTA-PDU and shall exist for RTC-PDU and RTA-PDU.
	c) The number of padding octets shall be in the range of 0..46 depending on the DLSDU size. The value shall be set to zero.
	
SAPDU		::=	UDP-RTA-PDU ^ ...
UDP-RTA-PDU	::=	IPHeader, UDPHeader, FrameID, RTA-PDU

FIDAPDU		::=	FrameID, RTC-PDU ^ RTA-PDU ^ ...
RTA-PDU		::=	DATA-RTA-PDU ^ ACK-RTA-PDU ^ NACK-RTA-PDU ^ ERR-RTA-PDU

DATA-RTA-PDU	::= Reference, PDUType(=1), FlagsSequence, VarPartLen(1 .. 1432), RTA-SDU
NACK-RTA-PDU	::= Reference, PDUType(=2), FlagsSequence, VarPartLen(=0)
ACK-RTA-PDU		::= Reference, PDUType(=3), FlagsSequence, VarPartLen(=0)
ERR-RTA-PDU		::= Reference, PDUType(=4), FlagsSequence, VarPartLen(=4), PNIOStatus, [VendorDeviceErrorInfo] b

	b) The number of octets of this substitution shall not exceed the DLPDU_Padding

	sizeof(DLPDU)(=64) - DA(=6) - SA(=6) - FCS(=4) - VLAN(=4) - LT(=2) - FrameID(=2)  == 40
	40 - sizeof(ERR-RTA-PDU)(=16) == 24 == max-sizeof(VendorDeviceErrorInfo)
	
Reference				::= DestinationAlarmEndpoint, SourceAlarmEndpoint
FlagsSequence			::=	AddFlags, SendSeqNum, AckSeqNum
VendorDeviceErrorInfo	::= VendorID, DeviceID, Data*

---

:: FIDAPDU ::

Data-RTA-PDU	::=
	SA, DA, VLAN, LT(=0x8892), FrameID, Reference, PDUType(=1)
	, AddFlags, SendSeqNum, AckSeqNum, VarPartLen(1..1432), RTA-SDU
	, DLPDU_Padding*(0..27)

Nack-RTA-PDU	::=
	SA, DA, VLAN, LT(=0x8892), FrameID, Reference, PDUType(=2)
	, AddFlags, SendSeqNum, AckSeqNum, VarPartLen(=0)
	, DLPDU_Padding*(28)

Ack-RTA-PDU		::=
	SA, DA, VLAN, LT(=0x8892), FrameID, Reference, PDUType(=3)
	, AddFlags, SendSeqNum, AckSeqNum, VarPartLen(=0)
	, DLPDU_Padding*(28)

Err-RTA-PDU		::=
	SA, DA, VLAN, LT(=0x8892), FrameID, Reference, PDUType(=4)
	, AddFlags, SendSeqNum, AckSeqNum, VarPartLen(=4), PNIOStatus
	, DLPDU_Padding*(24) ^ VendorDeviceErrorInfo-ACP

VendorDeviceErrorInfo-ACP	::= VendorID, DeviceID, PNIOVersion, ( ErrorTag, ErrorData )*

PNIOVersion					::=	PNIO_VERSION, PNIO_DISTRIBUTION, PNIO_INCREMENT, PNIO_INTEGRATION_COUNTER
							# e.g.  v5.1i2.3 => 0x05010203

**----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
//	related to EDD-NRT
//----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_alloc (
	ACP_LOWER_PTR  channel_lower
	)
{
	ACP_LOG_FCT("acp_nrt_recv_alloc")
	ACP_LOWER_RQB_PTR_TYPE	lower;
	ACP_LL_NRT_RECV_PTR_TYPE  edd_nrt;

	lower = acp_ll_alloc_rqb (channel_lower);
	if (is_null (lower)) {
		return LSA_NULL;
	}

	edd_nrt = ACP_LL_GET_PPARAM (lower, ACP_LL_NRT_RECV_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_nrt));

	edd_nrt->RequestID = 0; /* need for EDD_SRV_NRT_CANCEL */

	ACP_ALLOC_NRT_RECV_MEM (ACP_CAST_LOWER_MEM_PTR_PTR (& edd_nrt->pBuffer),
			EDD_FRAME_BUFFER_LENGTH, channel_lower->sysptr
		);
	if (is_null (edd_nrt->pBuffer)) {
		acp_ll_free_rqb (channel_lower, lower);
		return LSA_NULL;
	}

	ACP_LL_SET_SERVICE (lower, 	EDD_SRV_NRT_RECV);
	return lower;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_free (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	ACP_LOCAL_MEM_ATTR  *lower
	)
{
	ACP_LOG_FCT("acp_nrt_recv_free")
	LSA_UINT16  rc;
	ACP_LL_NRT_RECV_PTR_TYPE  edd_nrt;

	ACP_ASSERT (is_not_null (lower));
	ACP_ASSERT (is_not_null (*lower));

	edd_nrt = ACP_LL_GET_PPARAM (*lower, ACP_LL_NRT_RECV_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_nrt));

	ACP_FREE_NRT_RECV_MEM (& rc, edd_nrt->pBuffer, channel_lower->sysptr);
	ACP_ASSERT (rc == LSA_RET_OK);

	acp_ll_free_rqb (channel_lower, *lower);
}


/*----------------------------------------------------------------------------*/
ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_nrt_send_alloc (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT32  len
	)
{
	ACP_LOG_FCT("acp_nrt_send_alloc")
	ACP_LOWER_RQB_PTR_TYPE	lower;
	ACP_LL_NRT_SEND_PTR_TYPE  edd_nrt;

	if (len < EDD_NRT_MIN_SND_LEN) {
		return LSA_NULL;
	}

	lower = acp_ll_alloc_rqb (channel_lower);
	if (is_null (lower)) {
		return LSA_NULL;
	}

	edd_nrt = ACP_LL_GET_PPARAM (lower, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_nrt));

	edd_nrt->pBuffer	= LSA_NULL;
	edd_nrt->Length		= 0;
	edd_nrt->Priority	= EDD_NRT_SEND_PRIO_ASRT_LOW;
	edd_nrt->PortID		= EDD_PORT_ID_AUTO;
	edd_nrt->TxFrmGroup	= EDD_NRT_TX_GRP_ASRT;

	ACP_ALLOC_NRT_SEND_MEM (ACP_CAST_LOWER_MEM_PTR_PTR (& edd_nrt->pBuffer),
			len, channel_lower->sysptr
		);
	if (is_null (edd_nrt->pBuffer)) {
		acp_ll_free_rqb (channel_lower, lower);
		return LSA_NULL;
	}

	ACP_LL_SET_SERVICE (lower, EDD_SRV_NRT_SEND);
	return lower;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send_free (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	ACP_LOCAL_MEM_ATTR  *lower
	)
{
	ACP_LOG_FCT("acp_nrt_send_free")
	LSA_UINT16  rc;
	ACP_LL_NRT_SEND_PTR_TYPE  edd_nrt;

	ACP_ASSERT (is_not_null (lower));
	ACP_ASSERT (is_not_null (*lower));

	edd_nrt = ACP_LL_GET_PPARAM (*lower, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_nrt));

	ACP_FREE_NRT_SEND_MEM (& rc, edd_nrt->pBuffer, channel_lower->sysptr);
	ACP_ASSERT (rc == LSA_RET_OK);

	acp_ll_free_rqb (channel_lower, *lower);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	)
{
	ACP_LOG_FCT("acp_nrt_send")
	ACP_LL_NRT_SEND_PTR_TYPE  edd_send;
	LSA_INT  i;
	{
	ACP_RTA_SM_PTR  rtasm = (ACP_RTA_SM_PTR) user_id;

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_DATA_CNF
			|| rtasm->state ==  ACP_RTA_ST_ACK_CNF
			|| rtasm->state ==  ACP_RTA_ST_NACK_CNF
			|| rtasm->state ==  ACP_RTA_ST_ERROR_CNF
		);
	ACP_RTA_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rta: send:nrt(0x%x) aref(%u/%u) astate(%u) rxSeq(0x%x) txSeq(0x%x) retry(%u)"
		, nrt, rtasm->rta->alarm_ref, rtasm->prio
		, rtasm->state, rtasm->rx_seq_num, rtasm->tx_seq_num, rtasm->retry
		);
	}

		/* do padding(0) if Length < minimum PLPDU(64) */
	edd_send = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (edd_send->Length <= ACP_RTA_MAX_SEND_LEN);

	for (i = edd_send->Length; i < 64; ++i) {
		edd_send->pBuffer[i] = 0;
	}

	acp_ll_request (channel->lower, nrt, EDD_SRV_NRT_SEND, user_id);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send_done (
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	)
{
	ACP_LOG_FCT("acp_nrt_send_done")
	ACP_RTA_SM_PTR  rtasm;
	LSA_UINT16  rsp;
	ACP_CHANNEL_PTR  channel;
	ACP_LOCAL_ALARM_PTR  alarm;

	rtasm = (ACP_RTA_SM_PTR)user_id;

	ACP_ASSERT (is_not_null(rtasm->rta));
	ACP_ASSERT (is_not_null(rtasm->rta->channel));

	alarm = acp_alarm_from_rta (rtasm->rta);
	channel = alarm->channel;

	if (ACP_LL_GET_RESPONSE(nrt) == EDD_STS_ERR_TIMEOUT || ACP_LL_GET_RESPONSE(nrt) == EDD_STS_ERR_TX) {
		/* AP01013118 (EDDx/MRPD: ignore temporary blocked ports, upper protocol must re-send) */
		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "rta: send-done:nrt(0x%x) ignoring EDD-TX-error edd-rsp(%u)"
			, nrt, ACP_LL_GET_RESPONSE(nrt)
			);
		rsp = ACP_OK;
	}
	else {
		rsp = acp_rsp_from_edd_rsp (ACP_LL_GET_RESPONSE (nrt));
	}
	ACP_ASSERT (rsp != ACP_OK_CANCEL);

		/* send resource no more needed, take care of
		//	callback to acp_rta_sm_q_or_send_wakeup()
		//	within this function */
	acp_rta_sm_q_or_send_provide (channel, nrt);

	ACP_RTA_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rta: send-done:nrt(0x%x) aref(%u/%u) astate(%u) rxSeq(0x%x) txSeq(0x%x) retry(%u) .. rsp(%d)"
		, nrt, rtasm->rta->alarm_ref, rtasm->prio
		, rtasm->state, rtasm->rx_seq_num, rtasm->tx_seq_num, rtasm->retry
		, rsp
		);

		/* using state instead of packet type */
	switch (rtasm->state) {
	case ACP_RTA_ST_DATA_CNF:
		acp_rta_sm_send_data_done (channel, rtasm, rsp);
		break;

	case ACP_RTA_ST_ACK_CNF:
		acp_rta_sm_send_ack_done (channel, rtasm, rsp);
		break;

	case ACP_RTA_ST_NACK_CNF:
		acp_rta_sm_send_nack_done (channel, rtasm, rsp);
		break;

	case ACP_RTA_ST_ERROR_CNF:
		acp_rta_sm_send_error_done (channel, rtasm, rsp);
		break;

	default:
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_provide (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	acp_ll_request (channel_lower, nrt, EDD_SRV_NRT_RECV, channel_lower);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_indication (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_nrt_recv_indication")
	ACP_LL_NRT_RECV_PTR_TYPE  edd_recv;
	ACP_RTA_HEADER_TYPE  rta_hdr;
#if 0/*RTA-UDP*/
	ACP_IP_UDP_HEADER_TYPE ip_udp_hdr;
#endif
	ACP_RTA_PTR  rta;
	ACP_RTA_SM_PTR  rtasm;
	ACP_LOCAL_ALARM_PTR  alarm;

	if (ACP_LL_GET_RESPONSE (nrt) != EDD_STS_OK) {
		ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "rta: unhandled edd.rsp(%d)"
			, ACP_LL_GET_RESPONSE (nrt)
			);
		ACP_FATAL (ACP_LL_GET_RESPONSE (nrt));
		return;
	}

	edd_recv = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_RECV_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_recv)); /* Lower-RQB-Error */

		/* parsing, may pure ethernet or ip */
	switch (edd_recv->FrameFilter) {

#if 0/*RTA-UDP*/
	case EDD_NRT_FRAME_UDP_ASRT:
			/* parse and check ip-udp-header */
		if (! acp_ip_udp_header_parse (edd_recv->pBuffer + edd_recv->UserDataOffset, edd_recv->UserDataLength, & ip_udp_hdr)) {
			acp_nrt_recv_provide (channel_lower, nrt);
			return;
		}

			/* parse and check rta-header */
		if (! acp_rta_header_parse (edd_recv->pBuffer, edd_recv->UserDataOffset, edd_recv->UserDataLength, & rta_hdr)) {
			acp_nrt_recv_provide (channel_lower, nrt);
			return;
		}

ACP_FATAL (ACP_ERR_UNSUPPORTED);

		rta = LSA_NULL;
		break;
#endif

	case EDD_NRT_FRAME_ASRT:
			/* parse and check rta-header */
		{
		LSA_BOOL rc = acp_rta_header_parse (channel_lower
			, edd_recv->pBuffer, edd_recv->UserDataOffset, edd_recv->UserDataLength, & rta_hdr
			);
		if (! rc) {
			acp_nrt_recv_provide (channel_lower, nrt);
			return;
		}
		}

			/* find rtasm (via channel_lower == system.channel) */
		rta = acp_rta_find_from_hdr (channel_lower, & rta_hdr);
		if (is_null (rta)) {
			ACP_RTA_TRACE_03 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
				, "rta: not found for nic_id(%d) and aref(%u/%u)"
				, channel_lower->nic_id, rta_hdr.dst_alarm_ref, rta_hdr.prio
				);
			acp_nrt_recv_provide (channel_lower, nrt);
			return;
		}
		break;

	default:
		ACP_FATAL (edd_recv->FrameFilter);
		return;
	}

	if (rta->state != ACP_ALARM_ST_OPEN) {
			/* may happen, e.g. we are closing and recv the rta-err-pdu */
		ACP_RTA_TRACE_03 (channel_lower->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: aref(%u/%u) alarm.state(%d) not open -> ignore"
			, rta->alarm_ref, rta_hdr.prio, rta->state
			);
		acp_nrt_recv_provide (channel_lower, nrt);
		return;
	}
	ACP_ASSERT (rta->on_closing == 0);

	rtasm = acp_rta_sm_from_rta (rta, rta_hdr.prio);

	ACP_RTA_TRACE_08 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "rta: recv:nrt(0x%x) aref(%u/%u) astate(%u) rxSeq(0x%x), rta.type(%d) sendSeq(0x%x) ackSeq(0x%x)"
		, nrt, rtasm->rta->alarm_ref, rtasm->prio
		, rtasm->state, rtasm->rx_seq_num, rta_hdr.type
		, rta_hdr.send_seq_nr, rta_hdr.ack_seq_nr
		);

		/* states common to all rta.type */
	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:
		break;

	case ACP_RTA_ST_DATA_Q:
	case ACP_RTA_ST_DATA_CNF:
	case ACP_RTA_ST_ACK_Q:
	case ACP_RTA_ST_ACK_CNF:
	case ACP_RTA_ST_NACK_Q:
	case ACP_RTA_ST_NACK_CNF:
	case ACP_RTA_ST_ERROR_Q:
	case ACP_RTA_ST_ERROR_CNF:
		ACP_RTA_TRACE_03 (channel_lower->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: aref(%u/%u) astate(%u)  queueing lower"
			, rtasm->rta->alarm_ref, rtasm->prio, rtasm->state
			);
		acp_add_to_lower_rqb_list (& rtasm->lower_list, nrt);
		return;

	case ACP_RTA_ST_FREE:
	default:
		ACP_FATAL (rtasm->state);
		break;
	}

		/* check common to all rta.type */
	alarm = acp_alarm_from_rta (rtasm->rta);

	if (	alarm->dst_mac[0] != rta_hdr.src_mac[0]  ||  alarm->dst_mac[1] != rta_hdr.src_mac[1]
		||	alarm->dst_mac[2] != rta_hdr.src_mac[2]  ||  alarm->dst_mac[3] != rta_hdr.src_mac[3]
		||	alarm->dst_mac[4] != rta_hdr.src_mac[4]  ||  alarm->dst_mac[5] != rta_hdr.src_mac[5])
	{
		ACP_RTA_TRACE_08 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rta: aref(%u/%u) recv wrong src_mac(%02x-%02x-%02x-%02x-%02x-%02x)"
			, rtasm->rta->alarm_ref, rtasm->prio
			, rta_hdr.src_mac[0], rta_hdr.src_mac[1], rta_hdr.src_mac[2]
			, rta_hdr.src_mac[3], rta_hdr.src_mac[4], rta_hdr.src_mac[5]
			);
		ACP_RTA_TRACE_08 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rta: aref(%u/%u) want dst_mac(%02x-%02x-%02x-%02x-%02x-%02x)"
			, rtasm->rta->alarm_ref, rtasm->prio
			, alarm->dst_mac[0], alarm->dst_mac[1], alarm->dst_mac[2]
			, alarm->dst_mac[3], alarm->dst_mac[4], alarm->dst_mac[5]
			);
		acp_nrt_recv_provide (channel_lower, nrt);
		return;
	}

	if (alarm->dst_alarm_ref != rta_hdr.src_alarm_ref) {
		ACP_RTA_TRACE_04 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rta: aref(%u/%u) dst_alarm_ref(%d) != rta_hdr.src_alarm_ref(%d)"
			, rtasm->rta->alarm_ref, rtasm->prio
			, alarm->dst_alarm_ref, rta_hdr.src_alarm_ref
			);
		acp_nrt_recv_provide (channel_lower, nrt);
		return;
	}

		/* switch to recv_$type */
	switch (rta_hdr.type) {
	case ACP_RTA_PDU_TYPE_DATA:
		acp_rta_sm_recv_data (rta->channel, rtasm, nrt, & rta_hdr);
		break;

	case ACP_RTA_PDU_TYPE_NACK:
		acp_rta_sm_recv_nack (rta->channel, rtasm, nrt, & rta_hdr);
		break;

	case ACP_RTA_PDU_TYPE_ACK:
		acp_rta_sm_recv_ack (rta->channel, rtasm, nrt, & rta_hdr);
		break;

	case ACP_RTA_PDU_TYPE_ERROR:
		acp_rta_sm_recv_error (rta->channel, rtasm, nrt, & rta_hdr);
		break;

	default:
		acp_nrt_recv_provide (channel_lower, nrt);
		ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "rta: unknown rta.type(%d), ignore"
			, rta_hdr.type
			);
		break;
	}
}


/*------------------------------------------------------------------------------
//	related to RTA_SM
//----------------------------------------------------------------------------*/

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_ethernet_header_build (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  ethertype
	)
{
	ACP_LOG_FCT("acp_ethernet_header_build")
	LSA_UINT16  vlan_id;

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		vlan_id  = ACP_CAST_U16 (ACP_RTA_VLAN_PRIO_HIGH | (rtasm->rta->vlan_id & ACP_RTA_VLAN_ID_MSK));
	}
	else {
		vlan_id  = ACP_CAST_U16 (ACP_RTA_VLAN_PRIO_LOW | (rtasm->rta->vlan_id & ACP_RTA_VLAN_ID_MSK));
	}

		/* dst / src */
	acp_dp_set_mac (dp, rtasm->rta->dst_mac);
	acp_dp_set_mac (dp, channel->mac_addr);

		/* vlan */
	acp_dp_set_u16 (dp, ACP_ETHERTYPE_VLAN);
	acp_dp_set_u16 (dp, vlan_id);

		/* LT */
	ACP_ASSERT (ethertype == 0x8892 || ethertype == 0x0800);
	acp_dp_set_u16 (dp, ethertype);
}


LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_ip_header_checksum (
	EDD_UPPER_MEM_U8_PTR_TYPE dp
	)
{
	EDD_UPPER_MEM_U16_PTR_TYPE pBuf;
	LSA_UINT32  Sum;

	if (((LSA_UINT32)dp & 0x01) == 0x01) {
		ACP_FATAL (dp);
		return 1;
	}

	pBuf = (EDD_UPPER_MEM_U16_PTR_TYPE) dp;

	Sum = 0;
	Sum += *pBuf++;	/* Version, IHL, TOS */
	Sum += *pBuf++; /* TotalLength */
	Sum += *pBuf++; /* Identification */
	Sum += *pBuf++; /* Flags, Fragment Offs */
	Sum += *pBuf++; /* TTL, Protocl */

/*	Sum += *pBuf++;*/ /* Header Checksum */ /* asume 0 */
	pBuf++; /* Header Checksum */
			/* "for purposes of computing the checksum, the value of the checksum field is zero" */

	Sum += *pBuf++; /* SA */
	Sum += *pBuf++; /* SA */
	Sum += *pBuf++; /* DA */
	Sum += *pBuf++; /* SA */

	/* add carrys so we get ones-complement */
	Sum  = (Sum >> 16) + (Sum & 0xffff);
	Sum += (Sum >> 16);

	return((LSA_UINT16)~Sum);
}


#if 0/*RTA-UDP*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_ip_udp_header_build (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  udp_data_length
	)
{
	ACP_LOG_FCT("acp_ip_udp_header_build")
	EDD_UPPER_MEM_U8_PTR_TYPE  dp_start;
	EDD_UPPER_MEM_U8_PTR_TYPE  dp_checksum;
	LSA_UINT16  checksum;

	LSA_UNUSED_ARG (channel);

/* IPHeader */

	dp_start = dp;

		/* IP_VersionIHL(=0x45), IP_TypeOfService(=0), IP_TotalLength, */
	acp_dp_set_u8  (dp, 0x45);
	acp_dp_set_u8  (dp, 0x00);
	acp_dp_set_u16 (dp, ACP_CAST_U16(20/* ip header */ + 8/* udp header */ + udp_data_length));

		/* IP_Identification, IP_Flags_FragOffset, */
	acp_dp_set_u16 (dp, 0x8892); /* "to aid in assembling the fragments" */
									/* WinXP: looks like a sequence counter */
	acp_dp_set_u16 (dp, 0x4000); /* for UDP-RTA-PDU fragmentation shall not be used */

		/*  IP_TTL, IP_Protocol, IP_HeaderChecksum, */
	acp_dp_set_u8  (dp, 0x80); /* WinXP send this TTL */
	acp_dp_set_u8  (dp, 0x11); /* protokol == UDP */
	dp_checksum = dp;
	acp_dp_set_u16 (dp, 0x0000);

		/* IP_SrcIPAddress, IP_DstIPAddress, [IP_Options] */
	acp_dp_set_ip  (dp, rtasm->rta->src_ip);
	acp_dp_set_ip  (dp, rtasm->rta->dst_ip);

	checksum = acp_ip_header_checksum (dp_start);
	acp_dp_set_u16 (dp_checksum, checksum);

/* UDPHeader */

		/* UDP_SrcPort(=0x8892), UDP_DstPort(=0x8892), */
	acp_dp_set_u16 (dp, 0x8892);
	acp_dp_set_u16 (dp, 0x8892);

		/* UDP_DataLength, UDP_Checksum */
	acp_dp_set_u16 (dp, ACP_CAST_U16(8/* udp header*/ + udp_data_length));
	acp_dp_set_u16 (dp, 0x0000);
}
#endif


LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_header_build_2 (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  type,
	LSA_UINT16  var_part_len
	)
{
	ACP_LOG_FCT("acp_rta_header_build")
	LSA_UINT16  frame_id;
	LSA_UINT8  version_type;
	LSA_UINT8  add_flags;
	LSA_UINT16  send_seq_nr;
	LSA_UINT16  ack_seq_nr;

	LSA_UNUSED_ARG (channel);

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		frame_id = ACP_RTA_FRAME_ID_HIGH;
	}
	else {
		frame_id = ACP_RTA_FRAME_ID_LOW;
	}

	version_type = ACP_CAST_U8 (ACP_RTA_PDU_VERS_1 | (type & ACP_RTA_PDU_TYPE_MASK));

	switch (type) {
	case ACP_RTA_PDU_TYPE_DATA:
		add_flags = ACP_RTA_ADD_FLAGS_DATA;
		ACP_ASSERT (var_part_len > 0);
		send_seq_nr = rtasm->tx_seq_num;
		ack_seq_nr = rtasm->rx_seq_numO;
		break;

	case ACP_RTA_PDU_TYPE_NACK:
	case ACP_RTA_PDU_TYPE_ACK:
		add_flags = ACP_RTA_ADD_FLAGS_ELSE;
		ACP_ASSERT (var_part_len == 0);
		send_seq_nr = rtasm->tx_seq_numO;
		ack_seq_nr = rtasm->rx_seq_numO;
		break;

	case ACP_RTA_PDU_TYPE_ERROR:
		add_flags = ACP_RTA_ADD_FLAGS_ELSE;
		ACP_ASSERT (var_part_len == sizeof(LSA_UINT32)); /* for pnio_status */
		send_seq_nr = rtasm->tx_seq_numO;
		ack_seq_nr = rtasm->rx_seq_numO;
		break;

	default:
		ACP_FATAL (type);
		return;
	}

	ACP_RTA_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: build-hdr: rta.type(%d) dst.aref(%u/%u) src.aref(%u/%u) send_seq_nr(0x%04x) ack_seq_nr(0x%04x) var_part_len(%d)"
		, type
		, rtasm->rta->dst_alarm_ref
		, rtasm->prio
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, send_seq_nr
		, ack_seq_nr
		, var_part_len
		);

		/* FrameID */
	acp_dp_set_u16 (dp, frame_id);

		/* Reference.AlarmDstEndpoint, Reference.AlarmSrcEndpoint */
	acp_dp_set_u16 (dp, rtasm->rta->dst_alarm_ref);
	acp_dp_set_u16 (dp, rtasm->rta->alarm_ref);

		/* PDUType, AddFlags, SendSeqNum, */
	acp_dp_set_u8  (dp, version_type);
	acp_dp_set_u8  (dp, add_flags);
	acp_dp_set_u16 (dp, send_seq_nr);

		/* AckSeqNum, VarPartLen */
	acp_dp_set_u16 (dp, ack_seq_nr);
	acp_dp_set_u16 (dp, var_part_len);

	return;
}


LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_header_build (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  type,
	LSA_UINT16  var_part_len
	)
{
	ACP_LOG_FCT("acp_rta_header_build")
	LSA_UINT16  frame_id;
	LSA_UINT16  vlan_id;
	LSA_UINT8  version_type;
	LSA_UINT8  add_flags;
	LSA_UINT16  send_seq_nr;
	LSA_UINT16  ack_seq_nr;
	LSA_UINT16  padding_len;
	LSA_UINT16  i;

	LSA_UNUSED_ARG (channel);

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		frame_id = ACP_RTA_FRAME_ID_HIGH;
		vlan_id  = ACP_CAST_U16 (ACP_RTA_VLAN_PRIO_HIGH | (rtasm->rta->vlan_id & ACP_RTA_VLAN_ID_MSK));
	}
	else {
		frame_id = ACP_RTA_FRAME_ID_LOW;
		vlan_id  = ACP_CAST_U16 (ACP_RTA_VLAN_PRIO_LOW | (rtasm->rta->vlan_id & ACP_RTA_VLAN_ID_MSK));
	}

	version_type = ACP_CAST_U8 (ACP_RTA_PDU_VERS_1 | (type & ACP_RTA_PDU_TYPE_MASK));

	switch (type) {
	case ACP_RTA_PDU_TYPE_DATA:
		add_flags = ACP_RTA_ADD_FLAGS_DATA;
		ACP_ASSERT (var_part_len > 0);
		send_seq_nr = rtasm->tx_seq_num;
		ack_seq_nr = rtasm->rx_seq_numO;
		padding_len = 0; /* padding is after RTA-SDU, must done in acp_alarm_build_data() */
		break;

	case ACP_RTA_PDU_TYPE_NACK:
	case ACP_RTA_PDU_TYPE_ACK:
		add_flags = ACP_RTA_ADD_FLAGS_ELSE;
		ACP_ASSERT (var_part_len == 0);
		send_seq_nr = rtasm->tx_seq_numO;
		ack_seq_nr = rtasm->rx_seq_numO;
		padding_len = 32;
		break;

	case ACP_RTA_PDU_TYPE_ERROR:
		add_flags = ACP_RTA_ADD_FLAGS_ELSE;
		ACP_ASSERT (var_part_len == sizeof(LSA_UINT32)); /* for pnio_status */
		send_seq_nr = rtasm->tx_seq_numO;
		ack_seq_nr = rtasm->rx_seq_numO;
		padding_len = 28;
		break;

	default:
		ACP_FATAL (type);
		return;
	}

	ACP_RTA_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: build-hdr: rta.type(%d) dst.aref(%u/%u) src.aref(%u/%u) send_seq_nr(0x%04x) ack_seq_nr(0x%04x) var_part_len(%d)"
		, type
		, rtasm->rta->dst_alarm_ref
		, rtasm->prio
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, send_seq_nr
		, ack_seq_nr
		, var_part_len
		);

		/* dst / src */
	acp_dp_set_mac (dp, rtasm->rta->dst_mac);
	acp_dp_set_mac (dp, channel->mac_addr);

		/* vlan */
	acp_dp_set_u16 (dp, ACP_ETHERTYPE_VLAN);
	acp_dp_set_u16 (dp, vlan_id);

		/* LT(=0x8892) / frame-id */
	acp_dp_set_u16 (dp, ACP_ETHERTYPE_RT);
	acp_dp_set_u16 (dp, frame_id);

		/* rta */
	acp_dp_set_u16 (dp, rtasm->rta->dst_alarm_ref);
	acp_dp_set_u16 (dp, rtasm->rta->alarm_ref);
	acp_dp_set_u8  (dp, version_type);
	acp_dp_set_u8  (dp, add_flags);
	acp_dp_set_u16 (dp, send_seq_nr);
	acp_dp_set_u16 (dp, ack_seq_nr);
	acp_dp_set_u16 (dp, var_part_len);

		/* padding */
	for (i = 0; i < padding_len; ++i) {
		acp_dp_set_u8 (dp, 0);
	}

	return;
}


#if 0/*RTA-UDP*/
/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_ethernet_header_parse (
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  left,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_header_parse")
	EDD_UPPER_MEM_U8_PTR_TYPE  dp_start;

	if (left < ACP_PDU_ETHERNET_HEADER_LENGTH) {
		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: len(%d) < ACP_PDU_RTA_HEADER_LENGTH(%d)"
			, left
			, ACP_PDU_RTA_HEADER_LENGTH
			);
		return LSA_FALSE;
	}

		/* only src_mac */
	dp_start = dp;
	acp_dp_get_mac (hdr->src_mac, dp);

	return LSA_TRUE;
}
#endif


/*------------------------------------------------------------------------------
//	check min size
//	check frame_id -> prio
//	check implemented version
//	check valid type
//----------------------------------------------------------------------------*/
LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_rta_header_parse (
	ACP_LOWER_PTR  channel_lower,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  offset,
	LSA_UINT16  left,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_header_parse")
	EDD_UPPER_MEM_U8_PTR_TYPE  dp_start;
	LSA_UINT16  frame_id;
	LSA_UINT8   version_type;

#if ACP_CFG_TRACE_MODE != 2 /* LSA index trace */
	LSA_UNUSED_ARG(channel_lower);
#endif

	if (left < ACP_PDU_RTA_HEADER_LENGTH) {
		ACP_RTA_TRACE_02 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: left(%d) < ACP_PDU_RTA_HEADER_LENGTH(%d)"
			, left, ACP_PDU_RTA_HEADER_LENGTH
			);
		return LSA_FALSE;
	}

		/* src_mac */
	dp_start = dp;
	dp = dp + 6;
	acp_dp_get_mac (hdr->src_mac, dp);
	dp = dp_start + offset;

		/* frame-id */
	acp_dp_get_u16 (frame_id, dp);				left -= sizeof(LSA_UINT16);

	switch (frame_id) {
	case ACP_RTA_FRAME_ID_HIGH:		hdr->prio = ACP_ALARM_PRIORITY_HIGH; break;
	case ACP_RTA_FRAME_ID_LOW:		hdr->prio = ACP_ALARM_PRIORITY_LOW; break;
	default:
		ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: frame_id(0x%04x) unknown to RTA-PDU"
			, frame_id
			);
		return LSA_FALSE;
	}

		/* rta */
	acp_dp_get_u16 (hdr->dst_alarm_ref, dp);	left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (hdr->src_alarm_ref, dp);	left -= sizeof(LSA_UINT16);
	acp_dp_get_u8  (version_type, dp);			left -= sizeof(LSA_UINT8);
	acp_dp_get_u8  (hdr->add_flags, dp);		left -= sizeof(LSA_UINT8);
	acp_dp_get_u16 (hdr->send_seq_nr, dp);		left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (hdr->ack_seq_nr, dp);		left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (hdr->var_part_len, dp);		left -= sizeof(LSA_UINT16);

	hdr->version = ACP_CAST_U8 (version_type & ACP_RTA_PDU_VERS_MASK);
	hdr->type = ACP_CAST_U8 (version_type & ACP_RTA_PDU_TYPE_MASK);
	hdr->pnio_status = ACP_PNIO_STATUS_OK;

		/* check implemented ... */
	if (hdr->version != ACP_RTA_PDU_VERS_1) {
		ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: RTA-PDU with unknown rta.version(%d)"
			, hdr->version
			);
		return LSA_FALSE;
	}

	if (hdr->var_part_len > left) {
		ACP_RTA_TRACE_02 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: var_part_len(%d) > left(%d)"
			, hdr->var_part_len, left
			);
		return LSA_FALSE;
	}

		/* check known type */
	switch (hdr->type) {
	case ACP_RTA_PDU_TYPE_DATA:
		if (! (1 <= hdr->var_part_len  &&  hdr->var_part_len <= 1432)) {
			ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rta: DATA-RTA-PDU with wrong VarPartLen(%u)"
				, hdr->var_part_len
				);
			return LSA_FALSE;
		}
		break;

	case ACP_RTA_PDU_TYPE_NACK:
	case ACP_RTA_PDU_TYPE_ACK:
		if (hdr->var_part_len !=  0) {
			ACP_RTA_TRACE_02 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rta: (ACK/NACK)-RTA-PDU rta.type(%u), with wrong VarPartLen(%u)"
				, hdr->type, hdr->var_part_len
				);
			return LSA_FALSE;
		}
		break;

	case ACP_RTA_PDU_TYPE_ERROR:
		if (hdr->var_part_len == sizeof(LSA_UINT32))
		{
			acp_dp_get_u32 (hdr->pnio_status, dp);	left -= sizeof(LSA_UINT32);

			if (left >= 24) /* for struct, assume vendor == 0x002a */
			{
				LSA_UINT32 vendor_device, pnio_version, dd1, dd2, dd3, dd4;
				acp_dp_get_u32 (vendor_device, dp);
				acp_dp_get_u32 (pnio_version, dp);
				acp_dp_get_u32 (dd1, dp);
				acp_dp_get_u32 (dd2, dp);
				acp_dp_get_u32 (dd3, dp);
				acp_dp_get_u32 (dd4, dp);
				ACP_RTA_TRACE_07 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
					, "rta: ERR_RTA_PDU: pnio_status(%08x) VendorDevice(0x%08x) pnio_version(0x%08x) DebugData(0x%08x-%08x-%08x-%08x)"
					, hdr->pnio_status, vendor_device, pnio_version, dd1, dd2, dd3, dd4
					);
			}
		}
		else {
			ACP_RTA_TRACE_03 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rta: ERR_RTA_PDU rta.type(%d)/var_part_len(%d) mismatch/left(%d)"
				, hdr->type, hdr->var_part_len, left
				);
			return LSA_FALSE;
		}
		break;

	default:
		ACP_RTA_TRACE_01 (channel_lower->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: unknown rta.type(%d)"
			, hdr->type
			);
		return LSA_FALSE;
	}

	ACP_RTA_TRACE_09 (channel_lower->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "rta: parse-hdr: rta.type(%d) dst.aref(%u/%u) src.aref(%u/%u) send_seq_nr(0x%04x) ack_seq_nr(0x%04x) var_part_len(%d) pnio_status(0x%08x)"
		, hdr->type, hdr->dst_alarm_ref, hdr->prio, hdr->src_alarm_ref, hdr->prio
		, hdr->send_seq_nr, hdr->ack_seq_nr, hdr->var_part_len, hdr->pnio_status
		);
	return LSA_TRUE;
}


#if 0/*RTA-UDP*/
/*------------------------------------------------------------------------------
//	check xx
//	check xx
//----------------------------------------------------------------------------*/
LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_ip_udp_header_parse (
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  left,
	ACP_IP_UDP_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_ip_udp_header_parse")
	EDD_UPPER_MEM_U8_PTR_TYPE  dp_start;
	LSA_UINT8   version_ihl, tos, ttl;
	LSA_UINT16  identification, flags_fragmentoffset, checksum;

	if (left < ACP_PDU_IP_UDP_HEADER_LENGTH) {
		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: left(%d) < ACP_PDU_IP_UDP_HEADER_LENGTH(%d)", left, ACP_PDU_IP_UDP_HEADER_LENGTH
			);
		return LSA_FALSE;
	}

	dp_start = dp;

		/* IP */
	acp_dp_get_u8  (version_ihl, dp);				left -= sizeof(LSA_UINT8);
	acp_dp_get_u8  (tos, dp);						left -= sizeof(LSA_UINT8);
	acp_dp_get_u16 (hdr->ip_length, dp);			left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (identification, dp);			left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (flags_fragmentoffset, dp);		left -= sizeof(LSA_UINT16);
	acp_dp_get_u8  (ttl, dp);						left -= sizeof(LSA_UINT8);
	acp_dp_get_u8  (hdr->ip_protocol, dp);			left -= sizeof(LSA_UINT8);
	acp_dp_get_u16 (checksum, dp);					left -= sizeof(LSA_UINT16);
	acp_dp_get_ip  (hdr->ip_addr_src, dp);			left -= sizeof(ACP_IP_ADDR_TYPE);
	acp_dp_get_ip  (hdr->ip_addr_dst, dp);			left -= sizeof(ACP_IP_ADDR_TYPE);

		/* check IP */
	if (version_ihl != 0x45) {
		ACP_RTA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: version_ihl(0x%x) != 0x45", version_ihl
			);
		return LSA_FALSE;
	}
	if (hdr->ip_protocol != 0x11) { /* protocol == UDP */
		ACP_RTA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: ip_protocol(0x%x) != 0x11", hdr->ip_protocol
			);
		return LSA_FALSE;
	}
/* more check */


		/* UDP */
	acp_dp_get_u16 (hdr->udp_port_src, dp);			left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (hdr->udp_port_dst, dp);			left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (hdr->udp_length, dp);			left -= sizeof(LSA_UINT16);
	acp_dp_get_u16 (checksum, dp);					left -= sizeof(LSA_UINT16);

		/* check UDP */
	if (hdr->udp_port_src != 0x8892) {
		ACP_RTA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: udp_port_src(0x%x) != 0x8892", hdr->udp_port_src
			);
		return LSA_FALSE;
	}
	if (hdr->udp_port_dst != 0x8892) {
		ACP_RTA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: udp_port_dst(0x%x) != 0x8892", hdr->udp_port_dst
			);
		return LSA_FALSE;
	}
/* more check */

	return LSA_TRUE;
}
#endif


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_init (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_RTA_PTR  rta,
	LSA_UINT8  prio,
	LSA_UINT16  retry_count
	)
{
	ACP_LOG_FCT("acp_rta_sm_init")

	LSA_UNUSED_ARG (channel);

	rtasm->rta				= rta;
	rtasm->prio				= prio;
	rtasm->tx_seq_num		= ACP_RTA_SQN_START_SND; /* First seq_-num_ber to send   */
	rtasm->tx_seq_numO		= ACP_RTA_SQN_START_ACK; /* First Ack-seq_-No to send   */
	rtasm->rx_seq_num		= ACP_RTA_SQN_START_SND; /* Awaited send-seq-num       */
	rtasm->rx_seq_numO		= ACP_RTA_SQN_START_ACK; /* Awaited Ack-seq_-num        */
	rtasm->pnio_status_ec2	= 0;
	rtasm->upper			= LSA_NULL;
	rtasm->lower_list		= LSA_NULL;
	rtasm->wf_ack			= 0;
	rtasm->wf_timeout		= 0;
	rtasm->retry			= 0; /*retry_count;*/LSA_UNUSED_ARG(retry_count);
	acp_oneshot_init (
	& rtasm->oneshot_timer,	  acp_rta_sm_timeout, rtasm);
	rtasm->state			= ACP_RTA_ST_OPEN;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_stop (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	)
{
	ACP_LOG_FCT("acp_rta_sm_stop")
	ACP_RTA_PTR  rta;

	rta = rtasm->rta;

	ACP_RTA_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) on_closing(%d) wf_ack(%d)"
		, rta->alarm_ref
		, rtasm->prio
		, rta->on_closing
		, rtasm->wf_ack
		);

	if (rtasm->wf_ack) {
		acp_oneshot_stop (& rtasm->oneshot_timer);
		rtasm->wf_ack = 0;
	}
	ACP_ASSERT (acp_oneshot_is_running (& rtasm->oneshot_timer) == 0);

	rtasm->wf_timeout = 0;

	{
	ACP_UPPER_RQB_PTR_TYPE  rb;

	while (acp_get_from_rqb_list (& rta->send_queue[rtasm->prio], & rb)) {
		acp_callback_async (channel, ACP_OK_CANCEL, & rb);
	}
	}

	if ((rtasm->upper)) {
		ACP_ASSERT ( /* AP00294144 */
				ACP_RQB_GET_OPCODE (rtasm->upper) == ACP_OPC_ALARM_INDICATION
			||	ACP_RQB_GET_OPCODE (rtasm->upper) == ACP_OPC_ALARM_ACK_INDICATION
			);
		ACP_RQB_SET_OPCODE (rtasm->upper, ACP_OPC_ALARM_IND_RES_PROVIDE);
		acp_op_alarm_provide (channel, rtasm->upper);
		rtasm->upper = LSA_NULL;
	}

		/* free lowerQ, see acp_nrt_recv_indication() */
	{
	ACP_LOWER_RQB_PTR_TYPE  nrt;

	while (acp_get_from_lower_rqb_list (& rtasm->lower_list, & nrt)) {
		acp_nrt_recv_provide (channel->lower, nrt);
	}
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_data")
	ACP_LOCAL_ALARM_PTR  alarm;
	ACP_LL_NRT_SEND_PTR_TYPE  edd_send;
	LSA_UINT16  alarm_length;
	LSA_UINT16  alarm_offset;
	LSA_UINT16  rta_offset;

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_DATA_Q);
	ACP_ASSERT (rtasm->rta->on_closing == 0);

	alarm = acp_alarm_from_rta (rtasm->rta);

	edd_send = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_send));

	edd_send->Length		= 0;
	edd_send->Priority		= EDD_NRT_SEND_PRIO_ASRT_LOW;
	edd_send->PortID		= EDD_PORT_ID_AUTO;
	edd_send->TxFrmGroup	= EDD_NRT_TX_GRP_ASRT;

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		edd_send->Priority	= EDD_NRT_SEND_PRIO_ASRT_HIGH;
	}

	rta_offset = ACP_PDU_ETHERNET_HEADER_LENGTH;
	alarm_offset = ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_RTA_HEADER_LENGTH;
	if (alarm->is_udp) {
		rta_offset += ACP_PDU_IP_UDP_HEADER_LENGTH;
		alarm_offset += ACP_PDU_IP_UDP_HEADER_LENGTH;
	}
	ACP_ASSERT (alarm_offset < ACP_RTA_MAX_SEND_LEN);

	alarm_length = acp_alarm_build_data (channel, alarm, rtasm->prio,
			edd_send->pBuffer + alarm_offset,
			ACP_CAST_U16 (ACP_RTA_MAX_SEND_LEN - alarm_offset)
		);

	if (alarm_length > 0) {
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + rta_offset,
				ACP_RTA_PDU_TYPE_DATA, alarm_length
			);

		if (alarm->is_udp) {
#if 0/*RTA-UDP*/
			acp_ip_udp_header_build (channel, rtasm,
					edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
					ACP_CAST_U16 (alarm_length + ACP_PDU_RTA_HEADER_LENGTH)
				);
			acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_IP);
#else
			ACP_FATAL (ACP_ERR_UNSUPPORTED);
#endif
		}
		else {
			acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_RT);
		}

		edd_send->Length = alarm_offset + alarm_length;

		rtasm->state = ACP_RTA_ST_DATA_CNF;
		acp_nrt_send (channel, nrt, rtasm);
	}
	else {
		acp_rta_sm_q_or_send_provide (channel, nrt);
		acp_rta_sm_send_data_done (channel, rtasm, ACP_ERR_RESOURCE);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_data_done")

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_DATA_CNF);

	if (rtasm->rta->on_closing) {
		rtasm->wf_ack = 0;
		ACP_ASSERT (is_null (rtasm->rta->send_queue[rtasm->prio])); /* see acp_rta_sm_stop */
	}
	else {
		if (rsp == ACP_OK) {
			rtasm->wf_ack = 1;
			/* AP00507075: infinitely sending Alarm-PDU
			** rtasm->retry = rtasm->rta->retry_count; */
			acp_oneshot_trigger (& rtasm->oneshot_timer, rtasm->rta->timeout_ms);
		}
		else {
			rtasm->wf_ack = 0;
			/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, ACP_PNIO_STATUS_OK);
		}
	}

	rtasm->state = ACP_RTA_ST_OPEN;
	acp_rta_sm_state_enter (channel, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_ack")
	ACP_LOCAL_ALARM_PTR  alarm;
	ACP_LL_NRT_SEND_PTR_TYPE  edd_send;

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_ACK_Q);
	ACP_ASSERT (rtasm->rta->on_closing == 0);

	alarm = acp_alarm_from_rta (rtasm->rta);

	edd_send = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_send));

	edd_send->Length		= ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_RTA_HEADER_LENGTH;
	edd_send->Priority		= EDD_NRT_SEND_PRIO_ASRT_LOW;
	edd_send->PortID		= EDD_PORT_ID_AUTO;
	edd_send->TxFrmGroup	= EDD_NRT_TX_GRP_ASRT;

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		edd_send->Priority	= EDD_NRT_SEND_PRIO_ASRT_HIGH;
	}

	if (rtasm->rta->is_udp) {
#if 0/*RTA_UDP*/
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_IP_UDP_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_ACK, 0
			);
		acp_ip_udp_header_build (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				0 + ACP_PDU_RTA_HEADER_LENGTH
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_IP);

		edd_send->Length += ACP_PDU_IP_UDP_HEADER_LENGTH;
#else
		ACP_FATAL (ACP_ERR_UNSUPPORTED);
#endif
	}
	else {
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_ACK, 0
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_RT);
	}

	rtasm->state = ACP_RTA_ST_ACK_CNF;
	acp_nrt_send (channel, nrt, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_ack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_ack_done")

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_ACK_CNF);

		/* indicate to acp-user */
	if (is_not_null (rtasm->upper)) {
		if (rsp != ACP_OK) {
				/* on error always ACP_OPC_ALARM_INDICATION */
			rsp = ACP_ERR_ASRT;
			ACP_RQB_SET_OPCODE (rtasm->upper, ACP_OPC_ALARM_INDICATION);
		}
		/* callback */ acp_rta_indication (channel, rtasm->rta, rtasm->prio, rsp, & rtasm->upper);
	}
	ACP_ASSERT (is_null (rtasm->upper));

	rtasm->state = ACP_RTA_ST_OPEN;
	acp_rta_sm_state_enter (channel, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_nack")
	ACP_LOCAL_ALARM_PTR  alarm;
	ACP_LL_NRT_SEND_PTR_TYPE  edd_send;

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_NACK_Q);

	alarm = acp_alarm_from_rta (rtasm->rta);

	edd_send = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_send));

	edd_send->Length		= ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_RTA_HEADER_LENGTH;
	edd_send->Priority		= EDD_NRT_SEND_PRIO_ASRT_LOW;
	edd_send->PortID		= EDD_PORT_ID_AUTO;
	edd_send->TxFrmGroup	= EDD_NRT_TX_GRP_ASRT;

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		edd_send->Priority	= EDD_NRT_SEND_PRIO_ASRT_HIGH;
	}

	if (rtasm->rta->is_udp) {
#if 0/*RTA-UDP*/
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_IP_UDP_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_NACK, 0
			);
		acp_ip_udp_header_build (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				0 + ACP_PDU_RTA_HEADER_LENGTH
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_IP);

		edd_send->Length += ACP_PDU_IP_UDP_HEADER_LENGTH;
#else
		ACP_FATAL (ACP_ERR_UNSUPPORTED);
#endif
	}
	else {
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_NACK, 0
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_RT);
	}

	rtasm->state = ACP_RTA_ST_NACK_CNF;
	acp_nrt_send (channel, nrt, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_nack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_ack_done")

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_NACK_CNF);
	LSA_UNUSED_ARG (rsp);

	rtasm->state = ACP_RTA_ST_OPEN;
	acp_rta_sm_state_enter (channel, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_error")
	ACP_LL_NRT_SEND_PTR_TYPE  edd_send;
	EDD_UPPER_MEM_U8_PTR_TYPE  dp;
	LSA_UINT32  pnio_status;
	LSA_UINT32  pnio_version = 0;

	ACP_RTA_PTR  rta = rtasm->rta;

	ACP_ASSERT (rtasm->state == ACP_RTA_ST_ERROR_Q);

		/* new SM: on_closing sends only the low prio error_pdu */
	if (rta->on_closing  &&  rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		acp_rta_sm_q_or_send_provide (channel, nrt);

		rtasm->state = ACP_RTA_ST_ERROR_CNF;
		acp_rta_sm_send_error_done (channel, rtasm, ACP_OK);
		return;
	}

	edd_send = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_SEND_PTR_TYPE);
	ACP_ASSERT (is_not_null (edd_send));

	edd_send->Length		= ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_RTA_HEADER_LENGTH;
	edd_send->Priority		= EDD_NRT_SEND_PRIO_ASRT_LOW;
	edd_send->PortID		= EDD_PORT_ID_AUTO;
	edd_send->TxFrmGroup	= EDD_NRT_TX_GRP_ASRT;

	if (rtasm->prio == ACP_ALARM_PRIORITY_HIGH) {
		edd_send->Priority	= EDD_NRT_SEND_PRIO_ASRT_HIGH;
	}

	edd_send->Length += sizeof(LSA_UINT32); /*pnio_status*/
	dp = edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_RTA_HEADER_LENGTH;

	if (rta->is_udp) {
#if 0/*RTA-UDP*/
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH + ACP_PDU_IP_UDP_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_ERROR, sizeof(LSA_UINT32)/* pnio_status */
			);
		acp_ip_udp_header_build (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				sizeof(LSA_UINT32) + ACP_PDU_RTA_HEADER_LENGTH
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_IP);

		edd_send->Length += ACP_PDU_IP_UDP_HEADER_LENGTH;
		dp += ACP_PDU_IP_UDP_HEADER_LENGTH;
#else
		ACP_FATAL (ACP_ERR_UNSUPPORTED);
#endif
	}
	else {
		acp_rta_header_build_2 (channel, rtasm,
				edd_send->pBuffer + ACP_PDU_ETHERNET_HEADER_LENGTH,
				ACP_RTA_PDU_TYPE_ERROR, sizeof(LSA_UINT32)
			);
		acp_ethernet_header_build (channel, rtasm, edd_send->pBuffer, ACP_ETHERTYPE_RT);
	}

	pnio_status = ACP_PNIO_STATUS_REASON_TEMPLATE | rtasm->pnio_status_ec2;
	acp_dp_set_u32 (dp, pnio_status);

	if (! rta->is_udp)
	{
		ACP_UPPER_RQB_PTR_TYPE  rb = channel->upper;

		if (is_not_null (rb) &&	ACP_RQB_GET_OPCODE(rb) == ACP_OPC_ALARM_CLOSE  /* sanity */
		&&	rb->args->alarm.close.error_info.pnio_version != 0)
		{
			LSA_UINT16  len = sizeof(rb->args->alarm.close.error_info.debug_data);
			EDD_UPPER_MEM_U8_PTR_TYPE  dd;
			

			pnio_version = rb->args->alarm.close.error_info.pnio_version;
			
			acp_dp_set_u16 (dp, rb->args->alarm.close.error_info.vendor_id);
			acp_dp_set_u16 (dp, rb->args->alarm.close.error_info.device_id);
			acp_dp_set_u32 (dp, rb->args->alarm.close.error_info.pnio_version);
			edd_send->Length += 2+2+4;
			
			dd = dp;
			ACP_MEMCPY (dp, rb->args->alarm.close.error_info.debug_data, len);
			dp += len;
			edd_send->Length += len;

			{
			LSA_UINT32 dd1, dd2, dd3, dd4;
			acp_dp_get_u32 (dd1, dd);
			acp_dp_get_u32 (dd2, dd);
			acp_dp_get_u32 (dd3, dd);
			acp_dp_get_u32 (dd4, dd);
			ACP_RTA_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
				, "rta: alarm-close: aref(%u/%u)  pnio_status(0x%x)  pnio_version(0x%08x)  DebugData(0x%08x-%08x-%08x-%08x)"
				, rta->alarm_ref, rtasm->prio, pnio_status, pnio_version, dd1, dd2, dd3, dd4
				);	
			}
		}
	}

	if (pnio_version == 0)
	{
		ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rta: alarm-close: aref(%u/%u)  pnio_status(0x%x)"
			, rta->alarm_ref, rtasm->prio, pnio_status
			);	
	}

	rtasm->state = ACP_RTA_ST_ERROR_CNF;
	acp_nrt_send (channel, nrt, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_error_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	)
{
	ACP_LOG_FCT("acp_rta_sm_send_error_done")
	ACP_RTA_PTR  rta;
	ACP_ASSERT (rtasm->state == ACP_RTA_ST_ERROR_CNF);

	LSA_UNUSED_ARG (rsp); /* ignore send errors */

	rta = rtasm->rta;

	ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) on_closing(%d)"
		, rta->alarm_ref, rtasm->prio, rta->on_closing
		);

	if (rta->on_closing) {
		rtasm->state = ACP_RTA_ST_FREE;

		if (rta->low.state == ACP_RTA_ST_FREE && rta->high.state == ACP_RTA_ST_FREE) {
			/* callback */ acp_rta_close_done (channel, rta, ACP_OK);
			return;
		}

		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: aref(%u/%u) must wait for next error_done"
			, rta->alarm_ref
			, rtasm->prio
			);
	}
	else {
		rtasm->state = ACP_RTA_ST_OPEN;
		acp_rta_sm_state_enter (channel, rtasm);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_sm_recv_data")
	ACP_UPPER_RQB_PTR_TYPE  upper;
	ACP_LOCAL_ALARM_PTR  alarm;
	LSA_UINT16  rc;

	channel = channel; rtasm = rtasm; nrt = nrt;

	alarm = acp_alarm_from_rta (rtasm->rta);
	ACP_RTA_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) astate(%u) wf_ack(%d)"
		, alarm->alarm_ref
		, rtasm->prio
		, rtasm->state
		, rtasm->wf_ack
		);
	ACP_ASSERT (alarm->on_closing == 0);

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:

			/* check sequence_numbers */
		if (hdr->send_seq_nr != rtasm->rx_seq_num) {

			acp_nrt_recv_provide (channel->lower, nrt);

			if (hdr->send_seq_nr == rtasm->rx_seq_numO) {
				ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "rta: sm.rx_seq_num(%d) != recv.send_seq_nr(%d) => send ACK (rerun)"
					, rtasm->rx_seq_num
					, hdr->send_seq_nr
					);
				rtasm->state = ACP_RTA_ST_ACK_Q;
				acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
			}
			else {
				ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "rta: sm.rx_seq_num(%d) != recv.send_seq_nr(%d) => send NACK"
					, rtasm->rx_seq_num
					, hdr->send_seq_nr
					);
				rtasm->state = ACP_RTA_ST_NACK_Q;
				acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
			}
			return;
		}

			/* use data.ind as T-ACK */
		if (rtasm->wf_ack) {

				/* expected this seq-nr */
			if (hdr->ack_seq_nr == rtasm->tx_seq_num) {
				acp_oneshot_stop (& rtasm->oneshot_timer);
				rtasm->wf_ack = 0;

				/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_OK, ACP_PNIO_STATUS_OK);

				ACP_RTA_INCR_SEQ_NUM (rtasm->tx_seq_num, rtasm->tx_seq_numO);
			}

				/* old seq-nr */
			else if (hdr->ack_seq_nr == rtasm->tx_seq_numO) {
				/* sender doesn't seen my data yet, keep wf_ack */
			}

				/* not expected */
			else {
#if 0 /* 07-12-2005/APMS: DATA ignores unknown seq-nr */
				/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, ACP_PNIO_STATUS_OK);
#endif
			}
		}

			/* need acp_indication resource */
		acp_get_from_rqb_list (& channel->alarm_provide[hdr->prio], & upper);

		if (is_null (upper)) {
			/* do not queue lower-recv,
			//	when there are no free upper-resources, send no ack,
			//	sender must re-send */
			ACP_RTA_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rta: no upper resources, ignore lower.ind, aref(%u/%u) astate(%u) wf_ack(%u)"
				, alarm->alarm_ref, rtasm->prio, rtasm->state, rtasm->wf_ack
				);
			acp_nrt_recv_provide (channel->lower, nrt);

			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

		ACP_ASSERT (hdr->send_seq_nr == rtasm->rx_seq_num);
		{
		ACP_LL_NRT_RECV_PTR_TYPE  edd_recv;

		edd_recv = ACP_LL_GET_PPARAM (nrt, ACP_LL_NRT_RECV_PTR_TYPE);
		ACP_ASSERT (is_not_null (edd_recv));

			/* prepare acp_alarm.ind */
		rc = acp_alarm_indication_set_data (alarm, upper,
				edd_recv->pBuffer + edd_recv->UserDataOffset + ACP_PDU_RTA_HEADER_LENGTH,
				hdr->var_part_len, hdr->prio
			);
		acp_nrt_recv_provide (channel->lower, nrt);

		if (rc != ACP_OK) {
				/* parsing troubles */
			ACP_RQB_SET_OPCODE (upper, ACP_OPC_ALARM_INDICATION);
			/* callback */ acp_rta_indication (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, & upper);

			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

			/* keep upper and send ack,
			//	upper will send to acp_user on acp_rta_sm_send_ack_done */
		rtasm->upper = upper;

		ACP_RTA_INCR_SEQ_NUM (rtasm->rx_seq_num, rtasm->rx_seq_numO);

		rtasm->state = ACP_RTA_ST_ACK_Q;
		acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
		}
		break;

	default:
		/* see acp_nrt_recv_indication() */
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_sm_recv_nack")

	ACP_ASSERT (rtasm->rta->on_closing == 0);

	ACP_RTA_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u)  wf_ack(%d) hdr.ack_seq_nr(0x%04x) tx_seq_num(0x%04x)"
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, rtasm->wf_ack
		, hdr->ack_seq_nr
		, rtasm->tx_seq_num
		);

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:

		acp_nrt_recv_provide (channel->lower, nrt);

#if 1 /* 07-12-2005/APMS/APMR: NAK will be ignored in any case */
		;
#else
			/* do not expect a ack */
		if (! rtasm->wf_ack) {
/*?APMS#13:missing?*/
			return;
		}

			/* known seq-nr */
/*?APMS#23:	if (hdr->ack_seq_nr == rtasm->tx_seq_num  ||  hdr->ack_seq_nr == rtasm->tx_seq_numO) */
		if (hdr->ack_seq_nr != rtasm->tx_seq_num) {
			return;
		}

		acp_oneshot_stop (& rtasm->oneshot_timer);
		rtasm->wf_ack = 0;
		/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, ACP_PNIO_STATUS_OK);
#endif
		/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
		acp_rta_sm_state_enter (channel, rtasm);
		break;

	default:
		/* see acp_nrt_recv_indication() */
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_sm_recv_ack")

	ACP_RTA_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u)  wf_ack(%d) hdr.ack_seq_nr(0x%04x) tx_seq_num(0x%04x)"
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, rtasm->wf_ack
		, hdr->ack_seq_nr
		, rtasm->tx_seq_num
		);
	ACP_ASSERT (rtasm->rta->on_closing == 0);

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:

		acp_nrt_recv_provide (channel->lower, nrt);

#if 0 /*testcase*/
#ifdef ACP_MESSAGE
# pragma ACP_MESSAGE("error: testcase enabled: do not recv any ACK-PDU => DATA-PDU should do implizit Ack")
#endif /* ACP_MESSAGE */
		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "rta: aref(%u/%u) testcase ignore ACK-PDU"
			, rtasm->rta->alarm_ref, rtasm->prio
			);
		return;
#endif

			/* do not expect an ack */
		if (! rtasm->wf_ack) {
			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

			/* expected this seq-nr */
		if (hdr->ack_seq_nr == rtasm->tx_seq_num) {
			acp_oneshot_stop (& rtasm->oneshot_timer);
			rtasm->wf_ack = 0;
			/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_OK, ACP_PNIO_STATUS_OK);

			ACP_RTA_INCR_SEQ_NUM (rtasm->tx_seq_num, rtasm->tx_seq_numO);

			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

			/* old seq-nr */
		if (hdr->ack_seq_nr == rtasm->tx_seq_numO) {

			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

			/* not expected */
		ACP_RTA_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rta: aref(%u/%u)  hdr.ack_seq_nr(0x%04x) != tx_seq_num(0x%04x)"
			, rtasm->rta->alarm_ref
			, rtasm->prio
			, hdr->ack_seq_nr
			, rtasm->tx_seq_num
			);

#if 0 /* 07-12-2005/APMS: ACK ignored unknown seq-nr */
		acp_oneshot_stop (& rtasm->oneshot_timer);
		rtasm->wf_ack = 0;
		/* callback */ acp_rta_send_done (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, ACP_PNIO_STATUS_OK);
#endif

		/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
		acp_rta_sm_state_enter (channel, rtasm);
		break;

	default:
		/* see acp_nrt_recv_indication() */
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_sm_recv_error")
	ACP_LOCAL_ALARM_PTR  alarm;
	ACP_UPPER_RQB_PTR_TYPE  upper;

	alarm = acp_alarm_from_rta (rtasm->rta);
	ACP_RTA_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) astate(%u) pnio_status(%08x)"
		, alarm->alarm_ref
		, rtasm->prio
		, rtasm->state
		, hdr->pnio_status
		);
	ACP_ASSERT (rtasm->rta->on_closing == 0);

	/* rtasm->wf_ack dont care */

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:

		acp_nrt_recv_provide (channel->lower, nrt);

			/* need acp_indication resource */
		acp_get_from_rqb_list (& channel->alarm_provide[hdr->prio], & upper);
		if (is_null (upper)) {

			/*
			 * ignore no upper.ind-resource troubles
			 *	 no more packets will be arrived, next send should timeout ...
			 */

			ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "rta: no upper resources, must ignore RTA-ERR-PDU, aref(%u/%u) pnio_status(%x)"
				, alarm->alarm_ref, rtasm->prio, hdr->pnio_status
				);
			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
			return;
		}

			/* prepare acp_alarm.ind */
		upper->args->alarm.data.alarm_ref	= alarm->alarm_ref;
		upper->args->alarm.data.usr_id		= alarm->usr_id;
		upper->args->alarm.data.priority	= rtasm->prio;
		upper->args->alarm.data.pnio_status	= hdr->pnio_status;

		ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "recv RTA-ERROR.ind: aref(%u/%u) pnio_status(0x%08x)"
			, alarm->alarm_ref, rtasm->prio, hdr->pnio_status
			);

		ACP_RQB_SET_OPCODE (upper, ACP_OPC_ALARM_INDICATION);
		/* callback */ acp_rta_indication (channel, rtasm->rta, rtasm->prio, ACP_ERR_ASRT, & upper);

		/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
		acp_rta_sm_state_enter (channel, rtasm);
		break;

	default:
		/* see acp_nrt_recv_indication() */
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_timeout (
	ACP_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE  user_ptr
	)
{
	ACP_LOG_FCT("acp_rta_sm_timeout")
	ACP_RTA_SM_PTR  rtasm;
	ACP_CHANNEL_PTR  channel;
	LSA_UNUSED_ARG (oneshot);

	rtasm = (ACP_RTA_SM_PTR) user_ptr;

	ACP_ASSERT (are_equal (& rtasm->oneshot_timer, oneshot));

	channel = rtasm->rta->channel;
	ACP_RTA_TRACE_07 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) alarm.state(%d) astate(%u) retry(%d) wf_ack(%d) on_closing(%d)"
		, rtasm->rta->alarm_ref, rtasm->prio, rtasm->rta->state, rtasm->state
		, rtasm->retry, rtasm->wf_ack, rtasm->rta->on_closing
		);

	ACP_ASSERT (rtasm->rta->on_closing == 0);

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:
		if (rtasm->wf_ack) {

			if (rtasm->rta->state != ACP_ALARM_ST_OPEN) { /* AP00299415 */

				ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
					, "rta: timeout: aref(%u/%u)  recv a rta-err.pdu in the meantime, retry(%u) set to 0"
					, rtasm->rta->alarm_ref, rtasm->prio, rtasm->retry
					);
				rtasm->retry = 0;
			}

				/* re-send */
			if (rtasm->retry > 0) {

				ACP_RTA_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "rta: timeout:aref(%u/%u)  send data with retry(%u) txSeq(0x%x) rxSeq(0x%x)"
					, rtasm->rta->alarm_ref, rtasm->prio
					, rtasm->retry, rtasm->tx_seq_num, rtasm->rx_seq_num
					);

				rtasm->retry -= 1;

				rtasm->state = ACP_RTA_ST_DATA_Q;
				acp_rta_sm_q_or_send (rtasm->rta->channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
			}

				/* or send.cnf(-) */
			else {
				ACP_ASSERT (rtasm->retry == 0);

				acp_oneshot_stop (& rtasm->oneshot_timer);
				rtasm->wf_ack = 0;

				ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "rta: timeout: aref(%u/%u)  alarm-send.cnf(-)"
					, rtasm->rta->alarm_ref, rtasm->prio
					);

				/* callback */ acp_rta_send_done (channel,
						rtasm->rta, rtasm->prio, ACP_ERR_ASRT, ACP_PNIO_STATUS_OK
					);

				/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
				acp_rta_sm_state_enter (channel, rtasm);
			}
		}

		else {
			/* T-ACK recv in meantime */
			/* rtasm->state = ACP_RTA_ST_OPEN; no-op */
			acp_rta_sm_state_enter (channel, rtasm);
		}
		break;

	case ACP_RTA_ST_DATA_Q:
	case ACP_RTA_ST_DATA_CNF:
	case ACP_RTA_ST_ACK_Q:
	case ACP_RTA_ST_ACK_CNF:
	case ACP_RTA_ST_NACK_Q:
	case ACP_RTA_ST_NACK_CNF:
	case ACP_RTA_ST_ERROR_Q:
	case ACP_RTA_ST_ERROR_CNF:
		ACP_RTA_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: wf enter state ACP_RTA_ST_OPEN"
			);
		rtasm->wf_timeout = 1;
		break;

	case ACP_RTA_ST_FREE:
	default:
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_state_enter (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	)
{
	ACP_LOG_FCT("acp_rta_sm_state_enter")

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:

			/* if a close.req is pending */
		if (rtasm->rta->on_closing) {
			ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rta: aref(%u/%u) next state: ACP_RTA_ST_ERROR_Q"
				, rtasm->rta->alarm_ref
				, rtasm->prio
				);
			rtasm->state = ACP_RTA_ST_ERROR_Q;
			acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
			return;
		}

			/* if a timeout is pending */
		if (rtasm->wf_timeout) {
			ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rta: aref(%u/%u) next state: acp_rta_sm_timeout"
				, rtasm->rta->alarm_ref
				, rtasm->prio
				);
			rtasm->wf_timeout = 0;
			acp_rta_sm_timeout (& rtasm->oneshot_timer, rtasm);
			return;
		}

			/* if a rta-err.pdu was received or a rta_send.cnf(-) has occured, AP00299415 */
		if (rtasm->rta->state != ACP_ALARM_ST_OPEN) {
			ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "rta: aref(%u/%u) expect close.req: alarm.state(%u) != ST_OPEN"
				, rtasm->rta->alarm_ref
				, rtasm->prio
				, rtasm->rta->state
				);
			/* a close.req are expected */
			return;
		}

			/* if a recv.ind is pending */
		if (is_not_null (rtasm->lower_list)) {
			ACP_LOWER_RQB_PTR_TYPE  nrt;

			ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "rta: aref(%u/%u) next state: acp_nrt_recv_indication"
				, rtasm->rta->alarm_ref
				, rtasm->prio
				);
			acp_get_from_lower_rqb_list (& rtasm->lower_list, & nrt);
			acp_nrt_recv_indication (channel->lower, nrt); /* simulate recv from lower ! */

#ifdef ACP_MESSAGE
# pragma ACP_MESSAGE ("check: are there troubles with re-sort?")
/* nicht so gut, was ist wenn wieder eingehaengt wird, dann erfolgt ein Umreihung */
#endif /* ACP_MESSAGE */
			return;
		}

			/* if a upper.req is pending */
		if (is_not_null (rtasm->rta->send_queue[rtasm->prio])) {
			if (rtasm->wf_ack) {
				ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "rta: aref(%u/%u) wf_ack"
					, rtasm->rta->alarm_ref
					, rtasm->prio
					);
				return;
			}
			ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rta: aref(%u/%u) next state: ACP_RTA_ST_DATA_Q"
				, rtasm->rta->alarm_ref, rtasm->prio
				);
			acp_rta_send (channel, rtasm->rta, rtasm->prio);
			return;
		}

			/* nothing to do */
		ACP_RTA_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: aref(%u/%u) no event waiting"
			, rtasm->rta->alarm_ref, rtasm->prio
			);
		break;

	default:
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_close (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	)
{
	ACP_LOG_FCT("acp_rta_sm_close")

	ACP_ASSERT (rtasm->rta->on_closing);

	ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: close aref(%u/%u) astate(%u)"
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, rtasm->state
		);

	rtasm->pnio_status_ec2 = rtasm->rta->close_reason;

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:
		rtasm->state = ACP_RTA_ST_ERROR_Q;
		acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
		break;

	case ACP_RTA_ST_DATA_Q:
	case ACP_RTA_ST_ACK_Q:
	case ACP_RTA_ST_NACK_Q:
		rtasm->state = ACP_RTA_ST_ERROR_Q; /* redirect _wakeup */
		break;

	case ACP_RTA_ST_DATA_CNF:
	case ACP_RTA_ST_ACK_CNF:
	case ACP_RTA_ST_NACK_CNF:
		/* see on_closing */
		break;

	default:
		ACP_FATAL (rtasm->state);
		break;
	}
}


/*------------------------------------------------------------------------------
//	related to Pattern:
//		Resource	::= ACP_LOWER_RQB_PTR_TYPE  nrt	// EDD  - sendresource
//		Item		::= ACP_RTA_SM_PTR  rtasm		// aSRT - statemachine
//----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send_wakeup (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_q_or_send_wakeup")

	ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: send-wakeup: aref(%u/%u) astate(%u)"
		, rtasm->rta->alarm_ref
		, rtasm->prio
		, rtasm->state
		);

	switch (rtasm->state) {
	case ACP_RTA_ST_DATA_Q:
		acp_rta_sm_send_data (channel, rtasm, nrt);
		return;

	case ACP_RTA_ST_ERROR_Q:
		acp_rta_sm_send_error (channel, rtasm, nrt);
		return;

	case ACP_RTA_ST_ACK_Q:
		acp_rta_sm_send_ack (channel, rtasm, nrt);
		return;

	case ACP_RTA_ST_NACK_Q:
		acp_rta_sm_send_nack (channel, rtasm, nrt);
		return;

	default:
		ACP_FATAL (rtasm->state);
		return;
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send_check (
	ACP_CHANNEL_PTR  channel
	)
{
	ACP_LOG_FCT("acp_rta_sm_q_or_send_check")
	ACP_RTA_SM_PTR  rtasm;
	ACP_LOWER_RQB_PTR_TYPE	nrt;
	ACP_LIST_ENTRY_PTR_TYPE  list;

	if (AcpListIsEmpty (& channel->lower->q_or_send.nrt_send_list)) {
		ACP_RTA_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_WARN
			, "rta: send_check: no nrt_send_resource"
			);
		return;
	}

	if (! AcpListIsEmpty (& channel->lower->q_or_send.ack_list)) {
		list = & channel->lower->q_or_send.ack_list;
	}
	else if (! AcpListIsEmpty (& channel->lower->q_or_send.data_list)) {
		list = & channel->lower->q_or_send.data_list;
	}
	else {
		ACP_RTA_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "rta: send_check: no wf RTA_SM"
			);
		return; /* both rta_sm lists are empty */
	}

	AcpListRemoveHead (& channel->lower->q_or_send.nrt_send_list, nrt, ACP_LOWER_RQB_PTR_TYPE);
	AcpListRemoveHead (list, rtasm, ACP_RTA_SM_PTR);

	ACP_ASSERT (is_not_null (nrt));
	ACP_ASSERT (is_not_null (rtasm));

	acp_rta_sm_q_or_send_wakeup (channel, rtasm, nrt);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send_provide (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	)
{
	ACP_LOG_FCT("acp_rta_sm_q_or_send_provide")

	ACP_ASSERT (is_not_null (nrt));
	ACP_ASSERT (is_not_null (nrt->pParam));

	AcpListInsertTail (& channel->lower->q_or_send.nrt_send_list, (ACP_LIST_ENTRY_PTR_TYPE)nrt);

	acp_rta_sm_q_or_send_check (channel);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	)
{
	ACP_LOG_FCT("acp_rta_sm_q_or_send")
	ACP_LIST_ENTRY_PTR_TYPE  list;

	switch (rtasm->state) {
	case ACP_RTA_ST_DATA_Q:
	case ACP_RTA_ST_ERROR_Q:
		list = & channel->lower->q_or_send.data_list;
		break;

	case ACP_RTA_ST_ACK_Q:
	case ACP_RTA_ST_NACK_Q:
		list = & channel->lower->q_or_send.ack_list;
		break;

	default:
		ACP_FATAL (rtasm->state);
		return;
	}

	AcpListInsertTail (list, & rtasm->link);

	acp_rta_sm_q_or_send_check (channel);
}


/*------------------------------------------------------------------------------
//	related to RTA
//----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_open (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  alarm_ref,
	LSA_UINT16  dst_alarm_ref,
	ACP_MAC_ADDR_TYPE  dst_mac,
	ACP_IP_ADDR_TYPE  dst_ip,
	ACP_IP_ADDR_TYPE  src_ip,
	LSA_UINT16  vlan_id,
	LSA_UINT16  timeout100ms,
	LSA_UINT16  retry_count
	)
{
	ACP_LOG_FCT("acp_rta_open")
	LSA_UINT32  tmp;

	tmp = timeout100ms * 100;
	if (tmp > 0xFFFF) {
		ACP_RTA_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "rta: timeout100ms(%d) * 100 > 0xFFFF"
			, timeout100ms
			);
		/* callback */ acp_rta_open_done (channel, rta, ACP_ERR_PARAM);
		return;
	}

	rta->alarm_ref				= alarm_ref;
	rta->dst_alarm_ref			= dst_alarm_ref;
	ACP_MEMCPY (rta->dst_mac,	  dst_mac, ACP_MAC_ADDR_SIZE);
	ACP_MEMCPY (rta->dst_ip,	  dst_ip, ACP_IP_ADDR_SIZE);
	ACP_MEMCPY (rta->src_ip,	  src_ip, ACP_IP_ADDR_SIZE);
	rta->vlan_id				= vlan_id;
	rta->timeout_ms				= ACP_CAST_U16 (timeout100ms * 100);
	rta->retry_count			= retry_count;
	rta->on_closing				= 0;
	rta->close_reason			= 0;

	acp_rta_sm_init (channel, & rta->low, rta, ACP_ALARM_PRIORITY_LOW, retry_count);

	acp_rta_sm_init (channel, & rta->high, rta, ACP_ALARM_PRIORITY_HIGH, retry_count);

	/* callback */ acp_rta_open_done (channel, rta, ACP_OK);
	return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_close (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta
	)
{
	ACP_LOG_FCT("acp_rta_close")
	ACP_RTA_SM_PTR  rtasm;

	ACP_ASSERT (rta->state == ACP_ALARM_ST_WF_FREE);

	rta->on_closing = 1;

	/* first stop, then close (send error-pdu) */

	rtasm = acp_rta_sm_from_rta (rta, ACP_ALARM_PRIORITY_HIGH);
	acp_rta_sm_stop (channel, rtasm);

	rtasm = acp_rta_sm_from_rta (rta, ACP_ALARM_PRIORITY_LOW);
	acp_rta_sm_stop (channel, rtasm);

	rtasm = acp_rta_sm_from_rta (rta, ACP_ALARM_PRIORITY_HIGH);
	acp_rta_sm_close (channel, rtasm);

	rtasm = acp_rta_sm_from_rta (rta, ACP_ALARM_PRIORITY_LOW);
	acp_rta_sm_close (channel, rtasm);
}


/*----------------------------------------------------------------------------*/
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio
	)
{
	ACP_LOG_FCT("acp_rta_send")
	ACP_RTA_SM_PTR  rtasm;

	rtasm = acp_rta_sm_from_rta (rta, prio);

	ACP_ASSERT (rta->state == ACP_ALARM_ST_OPEN);
	ACP_ASSERT (is_not_null (rta->send_queue[prio]));

/* more cond ... */

	ACP_RTA_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rta: aref(%u/%u) astate(%u)"
		, rta->alarm_ref
		, rtasm->prio
		, rtasm->state
		);

	switch (rtasm->state) {
	case ACP_RTA_ST_OPEN:
		ACP_ASSERT (rtasm->wf_ack == 0);
		rtasm->retry = rtasm->rta->retry_count; /* AP00507075 */
		rtasm->state = ACP_RTA_ST_DATA_Q;
		acp_rta_sm_q_or_send (channel, rtasm); /* ==> acp_rta_sm_q_or_send_wakeup() */
		return;

	case ACP_RTA_ST_DATA_Q:
	case ACP_RTA_ST_DATA_CNF:
	case ACP_RTA_ST_ACK_Q:
	case ACP_RTA_ST_ACK_CNF:
	case ACP_RTA_ST_NACK_Q:
	case ACP_RTA_ST_NACK_CNF:
	case ACP_RTA_ST_ERROR_Q:
	case ACP_RTA_ST_ERROR_CNF:
		/* see: acp_rta_sm_state_enter() */
		return;

	default:
		ACP_FATAL (rtasm->state);
		return;
	}
}


/*----------------------------------------------------------------------------*/
ACP_RTA_PTR  ACP_LOCAL_FCT_ATTR
acp_rta_find_from_hdr (
	ACP_LOWER_PTR  channel_lower,
	ACP_RTA_HEADER_PTR  hdr
	)
{
	ACP_LOG_FCT("acp_rta_find_from_hdr")
	LSA_UINT16  i;
	ACP_LOCAL_ALARM_PTR  alarm;

	for (i = 0; i < ACP_CFG_MAX_CHANNELS; ++i)
	{
		ACP_CHANNEL_PTR  channel = & acp_channels[i];
		
		if (channel->state != ACP_CHANNEL_STATE_READY) {
			continue;
		}
		if (channel->detailptr->channel_type != ACP_CHANNEL_TYPE_USER) {
			continue;
		}
		if (channel->detailptr->nic_id != channel_lower->nic_id) {
			continue;
		}
		ACP_ASSERT (are_equal (channel->lower, channel_lower));

		alarm = acp_alarm_find (channel, hdr->dst_alarm_ref, LSA_FALSE);
		if (is_null (alarm)) {
			continue;
		}

		if (alarm->dst_mac[0] != hdr->src_mac[0]  ||  alarm->dst_mac[1] != hdr->src_mac[1]
		||	alarm->dst_mac[2] != hdr->src_mac[2]  ||  alarm->dst_mac[3] != hdr->src_mac[3]
		||	alarm->dst_mac[4] != hdr->src_mac[4]  ||  alarm->dst_mac[5] != hdr->src_mac[5])
		{
			continue;
		} /* ? cmp hdr->src_alarm_ref too */

			/* found */
		return acp_rta_from_alarm (alarm);
	}

	ACP_RTA_TRACE_02 (channel_lower->trace_idx, LSA_TRACE_LEVEL_WARN
		, "rta: channel.lower.nic_id(%d) aref(%u) not found"
		, channel_lower->nic_id
		, hdr->dst_alarm_ref
		);
	return LSA_NULL;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
