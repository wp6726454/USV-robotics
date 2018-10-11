#ifndef ACP_INT_H
#define ACP_INT_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#include "acp_inc.h"
#include "acp_trc.h"


/*------------------------------------------------------------------------------
// stringify
//----------------------------------------------------------------------------*/

#define ACP_STRINGIFY_(n)  # n
#define ACP_STRINGIFY(n)   ACP_STRINGIFY_(n) /* make it a string */


/*------------------------------------------------------------------------------
// pragma message
// intended usage: #pragma ACP_MESSAGE(...)
//----------------------------------------------------------------------------*/

#ifdef _MSC_VER /* Microsoft, pragma-args are expanded */

# ifdef ACP_MESSAGE
#  error "who defined ACP_MESSAGE?"
# else
#  ifdef ACP_CFG_MESSAGE /* for developers only */
#    define ACP_MESSAGE(str) message( __FILE__ "(" ACP_STRINGIFY(__LINE__) ") : " str)
#  endif
# endif /* ACP_MESSAGE */

#endif


/*------------------------------------------------------------------------------
// logging
//----------------------------------------------------------------------------*/

#ifndef ACP_LOG_FCT
# define ACP_LOG_FCT(fn)  /* empty */
#endif


/*------------------------------------------------------------------------------
// douple-linked-list, copy from CM
//----------------------------------------------------------------------------*/

#define ACP_LIST_TYPED_ENTRY_TYPE(ptr_type_)	\
	struct ptr_type_ ## _entry_tag { \
		ptr_type_  Flink; \
		ptr_type_  Blink; \
	}

typedef struct ACP_LIST_ENTRY_PTR_TYPE_entry_tag ACP_COMMON_MEM_ATTR  * ACP_LIST_ENTRY_PTR_TYPE;

typedef ACP_LIST_TYPED_ENTRY_TYPE(ACP_LIST_ENTRY_PTR_TYPE)  ACP_LIST_ENTRY_TYPE;


#define AcpListInitialize(ListHead_) {\
	(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
	}

#define AcpListIsEmpty(ListHead_) (\
	LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Flink, (ListHead_))\
	)

#define AcpListIterFirst(ListHead_, Type_) (\
	(Type_)(AcpListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
	)

#define AcpListIterNext(ListHead_, Entry_, Type_) (\
	(Type_)(LSA_HOST_PTR_ARE_EQUAL((Entry_)->Flink, (ListHead_)) ? LSA_NULL : (Entry_)->Flink)\
	)

#define AcpListInsertTail(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_);\
	(Entry_)->Blink = (ListHead_)->Blink;\
	(ListHead_)->Blink->Flink = (Entry_);\
	(ListHead_)->Blink = (Entry_);\
	}

#define AcpListRemoveEntry(Entry_) {\
	ACP_LIST_ENTRY_PTR_TYPE EX_Entry_;\
	EX_Entry_ = (Entry_);\
	EX_Entry_->Blink->Flink = EX_Entry_->Flink;\
	EX_Entry_->Flink->Blink = EX_Entry_->Blink;\
	}

#define AcpListRemoveHead(ListHead_, Entry_, Type_) {\
	(Entry_) = (Type_)((ListHead_)->Flink);\
	AcpListRemoveEntry((ListHead_)->Flink);\
	}

#define AcpListForeach(Entry_, ListHead_, Type_) \
	Entry_ = AcpListIterFirst (ListHead_, Type_); \
	is_not_null (Entry_); \
	Entry_ = AcpListIterNext (ListHead_, (ACP_LIST_ENTRY_PTR_TYPE)(Entry_), Type_)


/*------------------------------------------------------------------------------
// oneshot-timer -- implemented in file acp_tim.c, copy from CM
//----------------------------------------------------------------------------*/

typedef struct acp_oneshot_tag ACP_LOCAL_MEM_ATTR * ACP_ONESHOT_PTR_TYPE;

typedef LSA_VOID LSA_FCT_PTR(ACP_LOCAL_FCT_ATTR,
	ACP_ONESHOT_CALLBACK)(
		ACP_ONESHOT_PTR_TYPE oneshot,
		LSA_VOID_PTR_TYPE user_ptr
	);

typedef struct acp_oneshot_tag {
	ACP_LIST_ENTRY_TYPE dlink; /* must be first in struct */
	ACP_ONESHOT_CALLBACK callback;
	LSA_VOID_PTR_TYPE user_ptr;
	LSA_UINT32 ticks;
	LSA_UINT16 running;
} ACP_ONESHOT_TYPE;

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_oneshot_sys_init (
	LSA_UINT16  time_base,
	LSA_UINT16  time_factor
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_oneshot_sys_undo_init (
	LSA_VOID
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_oneshot_init(
	ACP_ONESHOT_PTR_TYPE oneshot,
	ACP_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_oneshot_trigger(
	ACP_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
);

LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_oneshot_stop(
	ACP_ONESHOT_PTR_TYPE oneshot
);

LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_oneshot_is_running(
	ACP_ONESHOT_PTR_TYPE oneshot
);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_timeout_synchronized (
	ACP_UPPER_RQB_PTR_TYPE  rb
	);


/*------------------------------------------------------------------------------
// ACP_FS_		FrameState_
//	0x00ff		-- reserved, the SM adds here the event
//	0x0f00		consumer
//	0xf000		provider
//----------------------------------------------------------------------------*/

#define ACP_FS_MASK_CONSUMER			0x0f00
#define ACP_FS_MASK_PROVIDER			0xf000

#define ACP_FS_NO						0x0000

#define ACP_FS_CONS_PASSIVE				0x0100
#define ACP_FS_CONS_ACTIVE				0x0300


#define ACP_FS_PROV_PASSIVE				0x1000
#define ACP_FS_PROV_ACTIVE				0x3000


/*-----------------------------------------------------------------------------
// ACP_FO_		FrameOptions
//---------------------------------------------------------------------------*/

enum acp_frame_options_enum {
	/* mask: 0x03 => cons: 0x01  prov: 0x02 */
	/* mask: 0x30 => srt:  0x00  udp:  0x10  sysred: 0x20 */
	ACP_FO_CONSUMER_SRT			= 0x0001,
	ACP_FO_PROVIDER_SRT			= 0x0002,
	ACP_FO_CONSUMER_SYSRED		= 0x0021,
	ACP_FO_PROVIDER_SYSRED		= 0x0022
};

#define acp_is_consumer(frame_ptr)		(((frame_ptr)->options & 0x03) == 0x01)
#define acp_is_provider(frame_ptr)		(((frame_ptr)->options & 0x03) == 0x02)

#define acp_is_sysred(frame_ptr)		(((frame_ptr)->options & 0x30) == 0x20)

/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct acp_channel_tag  ACP_LOCAL_MEM_ATTR  * ACP_CHANNEL_PTR;

typedef struct acp_local_alarm_tag  ACP_LOCAL_MEM_ATTR  * ACP_RTA_PTR;

/* typedef ACP_LIST_TYPED_ENTRY_TYPE(ACP_RTA_SM_PTR)  ACP_RTA_SM_ENTRY_TYPE; not yet */


#define ACP_IANA_PNIO_UDP_UNICAST_PORT	0x8892  /* see PNIO 61158-6 :: IANA_PNIO_UDP_UNICAST_PORT */


/*------------------------------------------------------------------------------
// data for the lower layer
//----------------------------------------------------------------------------*/

typedef struct acp_link_status_tag {
	ACP_LIST_ENTRY_TYPE  link;

	LSA_UINT16	port_id;		/* unique-key: port-id 1..n or ACP_PORT_ID_AUTO */

	LSA_UINT8	status;			/* current Link-Status */
	LSA_UINT8	speed;			/* current Link-Status speed */
	LSA_UINT8	mode;			/* current Link-Status mode */

} ACP_LINK_STATUS_TYPE;

typedef ACP_LINK_STATUS_TYPE ACP_LOCAL_MEM_ATTR * ACP_LINK_STATUS_PTR;


typedef struct acp_lower_tag {
	LSA_UINT16			nic_id;
	LSA_UINT16			count_of_open_channel;
	LSA_HANDLE_TYPE		handle_lower;
	LSA_UINT16			trace_idx;		/* needed for indexed trace */

	LSA_SYS_PTR_TYPE	sysptr;			/* need for alloc/free lower_layer */

		/* link-status */
	ACP_LIST_ENTRY_TYPE	lower_link_list;/* current Link-Status, list-of(ACP_LOWER_LINK_TYPE) */

		/* overload */
	LSA_BOOL			overload_edd_running;
	ACP_OVERLOAD_TYPE	overload;

		/* queue or send, resources for rta */
	struct acp_rta_qos_tag {
	ACP_LIST_ENTRY_TYPE	nrt_send_list;	/* available sending resources */
	ACP_LIST_ENTRY_TYPE	data_list;		/* alarms waiting for data-sending resource */
	ACP_LIST_ENTRY_TYPE	ack_list;		/* alarms waiting for ack-sending resource */
	} q_or_send;

} ACP_LOWER_TYPE;

typedef ACP_LOWER_TYPE ACP_LOCAL_MEM_ATTR * ACP_LOWER_PTR;


/*------------------------------------------------------------------------------
// local data for one frame (consumer or provider)
//----------------------------------------------------------------------------*/

typedef struct acp_local_frame_tag {
	ACP_CHANNEL_PTR		channel;		/* backlink to channel */
	LSA_UINT16			state;			/* state for statemachine */							/* @todo: VV: Loeschkanditat, see ACP_FS_xxx */
	LSA_UINT16			options;		/* typ (SRT/UDP), see acp_frame_options_enum */
	LSA_UINT16			acp_id;			/* the id used by ACP */
	LSA_UINT16			edd_id;			/* the id used by EDD */
	LSA_UINT32			usr_id;			/* the id used by the user */
	LSA_UINT32			st_signaled;	/* state - signaled to the user, see mode */
	LSA_UINT32			st_internal;	/* state - received from lower */
	LSA_UINT16			cycle_counter;	/* last known cycle_counter, only valid with MISS */
} ACP_LOCAL_FRAME;

typedef ACP_LOCAL_FRAME ACP_LOCAL_MEM_ATTR * ACP_LOCAL_FRAME_PTR;


/*------------------------------------------------------------------------------
// local data for one alarm
//----------------------------------------------------------------------------*/

/* ALARM-State */
#define ACP_ALARM_ST_FREE				0x00
#define ACP_ALARM_ST_WF_OPEN			0x01
#define ACP_ALARM_ST_OPEN				0x02
#define ACP_ALARM_ST_WF_FREE			0x04
#define ACP_ALARM_ST_ERROR				0x08

/* ALARM-Reference */
#define ACP_ALARM_REF_NOT_USED			0xFFFF


/* handles RTA-protocol within ACP */

/* RTA-SM-State */
#define ACP_RTA_ST_FREE			0
#define ACP_RTA_ST_OPEN			1

#define ACP_RTA_ST_ERROR_Q		10
#define ACP_RTA_ST_ERROR_CNF	11
#define ACP_RTA_ST_DATA_Q		20
#define ACP_RTA_ST_DATA_CNF		21
#define ACP_RTA_ST_ACK_Q		30
#define ACP_RTA_ST_ACK_CNF		31
#define ACP_RTA_ST_NACK_Q		40
#define ACP_RTA_ST_NACK_CNF		41

/* start sequencen numbers */
#define ACP_RTA_SQN_START_SND  0xFFFF
#define ACP_RTA_SQN_START_ACK  0xFFFE

#define ACP_RTA_INCR_SEQ_NUM(SeqNum, SeqNumO)	{\
		SeqNumO = (SeqNum);\
		SeqNum  = ACP_CAST_U16 (((SeqNum) + 1) & 0x7FFF);\
	}

	/*	ErrorCode	0xCF	RTA error
	//	ErrorDecode	0x81	PNIO
	//	ErrorCode1	0xFD	Used by RTA for protocol error
	//	ErrorCode2	0x..	see ACP_ALARM_CLOSE
	*/
#define ACP_PNIO_STATUS_REASON_TEMPLATE	0xCF81FD00

typedef struct acp_rta_sm_tag {

	ACP_LIST_ENTRY_TYPE  link;		/* for sendQ */
	ACP_RTA_PTR		rta;			/* backlink */
	LSA_UINT16		state;			/* State of Connection */

	LSA_UINT8		prio;
	LSA_UINT8		pnio_status_ec2;/* pnio_status to send / only part ErrorCode2 */

	LSA_UINT8		wf_ack;			/* send pending, wf_ack.ind */
	LSA_UINT8		wf_timeout;		/* acp_rta_sm_state_enter() must handle timeout */

	LSA_UINT16		tx_seq_num;		/* TxSeqNum : Actual SendSeqNumber      */
	LSA_UINT16		tx_seq_numO;	/* TxSeqNumO: Old    SendSeqNumber      */

	LSA_UINT16		rx_seq_num;		/* RxSeqNum : Next Rx SendSeqNum awaited*/
	LSA_UINT16		rx_seq_numO;	/* RxSeqNumO: Old  Rx SendSeqNum (ACK)  */

	ACP_UPPER_RQB_PTR_TYPE	upper;		/* keep a upper-acp_alarm.ind */
	ACP_LOWER_RQB_PTR_TYPE	lower_list;	/* keep lower-nrt-recv.ind */

	ACP_ONESHOT_TYPE		oneshot_timer;	/* need by re-send, acp_rta_send() */
	LSA_UINT16				retry;			/* retry counter, see rta->retry_count */

} ACP_RTA_SM_TYPE;

typedef struct acp_rta_sm_tag ACP_LOCAL_MEM_ATTR * ACP_RTA_SM_PTR;


typedef struct acp_local_alarm_tag {
	/* ACP-ALARM-Part */
	ACP_CHANNEL_PTR			channel;
	LSA_UINT32				usr_id;
	LSA_UINT16				alarm_ref;
	LSA_UINT8				state;
	LSA_BOOL				is_udp;

	ACP_UPPER_RQB_PTR_TYPE	send_queue[2]; /* low & high prio */

	/* ACP-RTA-Part */
/*	LSA_UINT8				state;			used from ACP-ALARM-Part */
/*	LSA_BOOL				is_udp;			used from ACP-ALARM-Part */
/*	LSA_UINT16				alarm_ref;		used from ACP-ALARM-Part */
	LSA_UINT16				dst_alarm_ref;
	ACP_MAC_ADDR_TYPE		dst_mac;
	ACP_IP_ADDR_TYPE		dst_ip;
	ACP_IP_ADDR_TYPE		src_ip;
	LSA_UINT8				on_closing;
	LSA_UINT8				close_reason;
	LSA_UINT16				vlan_id;
	LSA_UINT16				timeout_ms;
	LSA_UINT16				retry_count;

	ACP_RTA_SM_TYPE			low; /* low prio */
	ACP_RTA_SM_TYPE			high; /* high prio */
} ACP_LOCAL_ALARM_TYPE;


typedef ACP_LOCAL_ALARM_TYPE  ACP_LOCAL_MEM_ATTR * ACP_LOCAL_ALARM_PTR;


#define acp_rta_from_alarm(alarm) \
	((ACP_RTA_PTR)(alarm))

#define acp_alarm_from_rta(rta) \
	((ACP_LOCAL_ALARM_PTR)(rta))

#define acp_rta_sm_from_rta(rta, prio) \
	(((prio) == ACP_ALARM_PRIORITY_LOW) ? (& (rta)->low) : (& (rta)->high))

#define acp_rta_sm_from_alarm(alarm, prio) \
	acp_rta_sm_from_rta(acp_rta_from_alarm(alarm), (prio))


/* see PNIO 61158-6 */

#define ACP_ETHERTYPE_VLAN				0x8100
#define ACP_ETHERTYPE_IP				0x0800
#define ACP_ETHERTYPE_RT				0x8892	/* EDD-Version does swap on EDD_CFG_BIG_ENDIAN */

#define ACP_RTA_MIN_SEND_LEN			32
#define ACP_RTA_MAX_SEND_LEN			EDD_IEEE_FRAME_LENGTH

#define ACP_RTA_FRAME_ID_HIGH			EDD_SRT_FRAMEID_ASRT_PNIO_ALARM_HIGH  /* 0xFC01 */
#define ACP_RTA_FRAME_ID_LOW			EDD_SRT_FRAMEID_ASRT_PNIO_ALARM_LOW   /* 0xFE01 */

#define ACP_RTA_VLAN_ID_MSK				0x0FFF

#define ACP_RTA_VLAN_PRIO_MSK			0xE000
#define ACP_RTA_VLAN_PRIO_HIGH			(6 << 13)
#define ACP_RTA_VLAN_PRIO_LOW			(5 << 13)

#define ACP_RTA_PDU_VERS_MASK			0xF0
#define ACP_RTA_PDU_VERS_1				0x10

#define ACP_RTA_PDU_TYPE_MASK			0x0F
#define ACP_RTA_PDU_TYPE_DATA			0x01
#define ACP_RTA_PDU_TYPE_NACK			0x02
#define ACP_RTA_PDU_TYPE_ACK			0x03
#define ACP_RTA_PDU_TYPE_ERROR			0x04

#define ACP_RTA_ADD_FLAGS_DATA			0x11
#define ACP_RTA_ADD_FLAGS_ELSE			0x01


/*
EtherHeader		::=
	DA, SA, [VLAN], LT

IPHeader		::=
	IP_VersionIHL(=0x45), IP_TypeOfService(=0), IP_TotalLength, IP_Identification,
	IP_Flags_FragOffset, IP_TTL, IP_Protocol, IP_HeaderChecksum,
	IP_SrcIPAddress, IP_DstIPAddress, [IP_Options]

UDPHeader		::=
	UDP_SrcPort(=0x8892), UDP_DstPort(=0x8892), UDP_DataLength, UDP_Checksum

*/
#define ACP_PDU_ETHERNET_HEADER_LENGTH	(6+6+2+2+2)
										/* dst.mac + src.mac + vlan.type + vlan.id + LT */

#define ACP_PDU_IP_UDP_HEADER_LENGTH	(1+1+2+2+2+1+1+2+4+4 + 2+2+2+2)
										/* VersionIHL .. DstIPAddress, SrcPort .. UdpChecksum */

/*
Data-RTA-PDU	::=
	[VLAN], LT(=0x8892),				// not contained in this define
	FrameID, Reference, PDUType (=1),
	AddFlags, SendSeqNum, AckSeqNum, VarPartLen (1..1432),
	RTA-SDU, RTAPadding (0..31)
*/
#define ACP_PDU_RTA_HEADER_LENGTH		(2+2+2+1+1+2+2+2)


typedef struct acp_rta_header_tag {
	ACP_MAC_ADDR_TYPE	src_mac;
	LSA_UINT16			prio; /* <=> frame_id */
	LSA_UINT16			dst_alarm_ref;
	LSA_UINT16			src_alarm_ref;
	LSA_UINT8			version;
	LSA_UINT8			type;
	LSA_UINT8			add_flags;
	LSA_UINT16			send_seq_nr;
	LSA_UINT16			ack_seq_nr;
	LSA_UINT16			var_part_len;
	LSA_UINT32			pnio_status;
} ACP_RTA_HEADER_TYPE;

typedef ACP_RTA_HEADER_TYPE  ACP_LOCAL_MEM_ATTR * ACP_RTA_HEADER_PTR;


typedef struct acp_ip_udp_header_tag {
	/* IP */
	LSA_UINT16			ip_length;
	LSA_UINT8			ip_protocol;
	ACP_IP_ADDR_TYPE	ip_addr_src;
	ACP_IP_ADDR_TYPE	ip_addr_dst;
	/* UDP */
	LSA_UINT16			udp_port_src;
	LSA_UINT16			udp_port_dst;
	LSA_UINT16			udp_length;
} ACP_IP_UDP_HEADER_TYPE;

typedef ACP_IP_UDP_HEADER_TYPE  ACP_LOCAL_MEM_ATTR * ACP_IP_UDP_HEADER_PTR;


/*------------------------------------------------------------------------------
//	read data from PDU / write data to PDU
//----------------------------------------------------------------------------*/

#define acp_dp_set_u8(dp_, u8_)		\
		{ACP_PUT8_HTON ((dp_), 0, (u8_)); (dp_) += 1;}

#define acp_dp_get_u8(u8_, dp_)		\
		{u8_ = ACP_GET8_NTOH ((dp_), 0); (dp_) += 1;}

#define acp_dp_set_u16(dp_, u16_)		\
		{ACP_PUT16_HTON ((dp_), 0, (u16_)); (dp_) += 2;}

#define acp_dp_get_u16(u16_, dp_)		\
		{u16_ = ACP_GET16_NTOH ((dp_), 0); (dp_) += 2;}

#define acp_dp_set_u32(dp_, u32_)		\
		{ACP_PUT32_HTON (dp_, 0, u32_); (dp_) += 4;}

#define acp_dp_get_u32(u32_, dp_)		\
		{u32_ = ACP_GET32_NTOH ((dp_), 0); (dp_) += 4;}

#define acp_dp_set_mac(dp_, mac_) \
		{ACP_MEMCPY (dp_, mac_, ACP_MAC_ADDR_SIZE); (dp_) += ACP_MAC_ADDR_SIZE;}

#define acp_dp_get_mac(mac_, dp_) \
		{ACP_MEMCPY (mac_, dp_, ACP_MAC_ADDR_SIZE); (dp_) += ACP_MAC_ADDR_SIZE;}

#define acp_dp_set_ip(dp_, ip_) \
		{ACP_MEMCPY (dp_, ip_, ACP_IP_ADDR_SIZE); (dp_) += ACP_IP_ADDR_SIZE;}

#define acp_dp_get_ip(ip_, dp_) \
		{ACP_MEMCPY (ip_, dp_, ACP_IP_ADDR_SIZE); (dp_) += ACP_IP_ADDR_SIZE;}


/*------------------------------------------------------------------------------
// channel table
//----------------------------------------------------------------------------*/

#define ACP_CHANNEL_STATE_FREE		0
#define ACP_CHANNEL_STATE_RESERVED	1
#define ACP_CHANNEL_STATE_READY		2

typedef struct acp_channel_tag {
	LSA_HANDLE_TYPE			my_handle;		/* my one handle <=> index in table + 1 */
	LSA_UINT16				state;			/* internal state */

	LSA_SYS_PTR_TYPE		sysptr;			/* current not used, lsa specifically, must match with lower->sysptr */
	ACP_DETAIL_PTR_TYPE		detailptr;		/* pointer to channel-details */

	LSA_UINT16				trace_idx;		/* needed for indexed trace */
	ACP_MAC_ADDR_TYPE		mac_addr;		/* local mac-address */
	LSA_UINT16				max_eth_ports;	/* max counts of ports within this mac-addr, 1..n */
	ACP_IP_ADDR_TYPE		ip_addr;		/* local ip-address */

	ACP_UPPER_CALLBACK_FCT_PTR_TYPE
							callback;		/* the callback-function to the user */
	LSA_HANDLE_TYPE			handle_upper;	/* the handle of the user */
	ACP_LOWER_PTR			lower;			/* pointer to lower-structur */

	ACP_UPPER_RQB_PTR_TYPE	upper;			/* current working upper request block */
	ACP_UPPER_RQB_PTR_TYPE	upper_list;		/* a queue with upper request blocks, waiting for serve */
	LSA_BOOL				upper_is_busy;	/* request-loop is running */

	LSA_UINT16				max_group_id;	/* max group_id of lower layer, my_handle - 1 must < max_group_id */
	LSA_UINT16				max_frames;		/* count_cons + count_prov + 1 */
	ACP_LOCAL_FRAME_PTR		frames;			/* a array of frames, used by this channel !dynamisches array! */
	ACP_UPPER_RQB_PTR_TYPE	provide;		/* all provided RQBs for state-indications */
	ACP_LIST_ENTRY_TYPE		link_status_list;/* all provided RQBs for link_status-indications */
	ACP_UPPER_RQB_PTR_TYPE	overload_ind;	/* one overload-indication allowed */
	LSA_UINT16				do_event_prov_drop;
											/* true: must do the state-indication PROV_DROP */

	LSA_UINT16				max_alarms;		/* alarms[0 .. detailptr->alarm_nr_of_ref-1] */
	ACP_LOCAL_ALARM_PTR		alarms;			/* an array of alarm used by this channel */
	ACP_UPPER_RQB_PTR_TYPE	alarm_provide[2];
											/* upper alarm-queue, prio low & high */
	ACP_LOWER_RQB_PTR_TYPE	alarm_lower[2];	/* lower alarm-queue, prio low & high */

} ACP_CHANNEL;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

#if (ACP_MODULE_ID == 1)

ACP_CHANNEL						acp_channels[ACP_CFG_MAX_CHANNELS];
ACP_LOWER_TYPE					acp_lowers[ACP_CFG_MAX_CHANNELS];
LSA_USER_ID_TYPE				acp_null_usr_id;

#else

LSA_EXTERN ACP_CHANNEL			acp_channels[ACP_CFG_MAX_CHANNELS];
LSA_EXTERN ACP_LOWER_TYPE		acp_lowers[ACP_CFG_MAX_CHANNELS];
LSA_EXTERN LSA_USER_ID_TYPE		acp_null_usr_id;

#endif


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define is_null(ptr)			(LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define is_not_null(ptr)		(! LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define are_equal(ptr1,ptr2)	(LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

#define is_null_ip(acp_ip_addr_) (\
		(acp_ip_addr_)[0] == 0 && (acp_ip_addr_)[1] == 0 \
	&&	(acp_ip_addr_)[2] == 0 && (acp_ip_addr_)[3] == 0 \
	)

#define ACP_FATAL(error_code_0)	{\
	acp_fatal_error (ACP_MODULE_ID, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL\
		);\
	}


/*------------------------------------------------------------------------------
// helper for casting and warning-level
//----------------------------------------------------------------------------*/

#if ACP_CFG_CAST_WARN

#	define ACP_CAST_U8(val_)				(val_)
#	define ACP_CAST_U16(val_)				(val_)

#	define ACP_CAST_UPPER_RQB_PTR_PTR(val_)	(val_)
#	define ACP_CAST_UPPER_MEM_PTR_PTR(val_)	(val_)

#	define ACP_CAST_LOCAL_MEM_PTR_PTR(val_)	(val_)

#	define ACP_CAST_LOWER_RQB_PTR_PTR(val_)	(val_)
#	define ACP_CAST_LOWER_MEM_PTR_PTR(val_)	(val_)

#else

#	define ACP_CAST_U8(val_)				((LSA_UINT8)(val_))
#	define ACP_CAST_U16(val_)				((LSA_UINT16)(val_))

#	define ACP_CAST_UPPER_RQB_PTR_PTR(val_)	((ACP_UPPER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR *)(val_))
#	define ACP_CAST_UPPER_MEM_PTR_PTR(val_)	((ACP_UPPER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR *)(val_))

#	define ACP_CAST_LOCAL_MEM_PTR_PTR(val_)	((ACP_LOCAL_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR *)(val_))

#	define ACP_CAST_LOWER_RQB_PTR_PTR(val_)	((ACP_LOWER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR *)(val_))
#	define ACP_CAST_LOWER_MEM_PTR_PTR(val_)	((ACP_LOWER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR *)(val_))

#endif


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*-acp_al.c-------------------------------------------------------------------*/

LSA_UINT16 ACP_LOCAL_FCT_ATTR
acp_alarm_build_data (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_ALARM_PTR  alarm,
	LSA_UINT8  prio,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  dp_size
	);
LSA_INT16  ACP_LOCAL_FCT_ATTR
acp_alarm_indication_set_data (
	ACP_LOCAL_ALARM_PTR  alarm,
	ACP_UPPER_RQB_PTR_TYPE  upper,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  dp_len,
	LSA_UINT16  prio
	);

ACP_LOCAL_ALARM_PTR  ACP_LOCAL_FCT_ATTR
acp_alarm_find (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT16  alarm_ref,
	LSA_BOOL  do_elog
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_alarm_count_open (
	ACP_CHANNEL_PTR  channel
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_alarm_open (
	ACP_CHANNEL_PTR  channel,
	LSA_BOOL  is_udp_alarm
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_alarm_close (
	ACP_CHANNEL_PTR  channel
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_alarm_send (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_alarm_ack_send (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_alarm_provide (
	ACP_CHANNEL_PTR  channel,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);


/*-acp_rta.c-----------------------------------------------------------------*/


/* rta */
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_open (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  alarm_ref,
	LSA_UINT16  dst_alarm_ref,
	ACP_MAC_ADDR_TYPE  dst_mac,
	ACP_IP_ADDR_TYPE  dst_ip,
	ACP_IP_ADDR_TYPE  src_ip,
	LSA_UINT16  vlan_id,
	LSA_UINT16  timeout10ms,
	LSA_UINT16  retry_count
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_open_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  rsp
	);

LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_close (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_close_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  rsp
	);

ACP_RTA_PTR  ACP_LOCAL_FCT_ATTR
acp_rta_find_from_hdr (
	ACP_LOWER_PTR  channel_lower,
	ACP_RTA_HEADER_PTR  hdr
	);

LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_send_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio,
	LSA_UINT16  rsp,
	LSA_UINT32  pnio_status
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_indication (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_PTR  rta,
	LSA_UINT16  prio,
	LSA_UINT16  rsp,
	ACP_UPPER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR  * upper
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_header_build (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  type,
	LSA_UINT16  var_part_len
	);
LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_rta_header_parse (
	ACP_LOWER_PTR  channel_lower,
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  offset,
	LSA_UINT16  left,
	ACP_RTA_HEADER_PTR  hdr
	);

LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_ip_udp_header_parse (
	EDD_UPPER_MEM_U8_PTR_TYPE  dp,
	LSA_UINT16  left,
	ACP_IP_UDP_HEADER_PTR  hdr
	);

/* rta_sm_q_or_send */
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send_provide (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_q_or_send_wakeup (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);

/* nrt */
ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_alloc (
	ACP_LOWER_PTR  channel_lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_free (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	ACP_LOCAL_MEM_ATTR  *lower
	);
ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_nrt_send_alloc (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT32  len
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send_free (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	ACP_LOCAL_MEM_ATTR  *lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_send_done (
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	LSA_VOID_PTR_TYPE  user_id
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_provide (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_nrt_recv_indication (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);


/* rta_sm */
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_state_enter (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm
	);

LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);


LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_recv_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt,
	ACP_RTA_HEADER_PTR  hdr
	);

LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_data_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_ack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_ack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_nack (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_nack_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_error (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	ACP_LOWER_RQB_PTR_TYPE	nrt
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_send_error_done (
	ACP_CHANNEL_PTR  channel,
	ACP_RTA_SM_PTR  rtasm,
	LSA_UINT16  rsp
	);

LSA_VOID ACP_LOCAL_FCT_ATTR
acp_rta_sm_timeout (
	ACP_ONESHOT_PTR_TYPE  oneshot,
	LSA_VOID_PTR_TYPE  user_ptr
	);


/*-acp_low.c------------------------------------------------------------------*/

ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_FCT_ATTR
acp_ll_alloc_rqb (
	ACP_LOWER_PTR			channel_lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_ll_free_rqb (
	ACP_LOWER_PTR			channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
	);
LSA_VOID ACP_LOCAL_FCT_ATTR
acp_ll_request (
	ACP_LOWER_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  edd,
	EDD_SERVICE  service,
	LSA_VOID_PTR_TYPE  user_id
	);
ACP_LINK_STATUS_PTR  ACP_LOCAL_FCT_ATTR
acp_link_status_find (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT16  port_id
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_link_status_alloc (
	ACP_LOWER_PTR  channel_lower,
	ACP_UPPER_RQB_PTR_TYPE  upper
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_link_status_compare_set (
	ACP_LOWER_PTR  channel_lower,
	ACP_LINK_STATUS_PTR  acp_link,
	ACP_UPPER_RQB_PTR_TYPE  upper
	);


/*-acp_opc.c------------------------------------------------------------------*/

LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_add_to_rqb_list (
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_UPPER_RQB_PTR_TYPE  rqb
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_add_to_lower_rqb_list (
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_LOWER_RQB_PTR_TYPE  rqb
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_get_from_rqb_list (
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_get_from_lower_rqb_list (
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb_list,
	ACP_LOWER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR  * rqb
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_free_resource (
	ACP_CHANNEL_PTR channel
	);

ACP_LOCAL_FRAME_PTR  ACP_LOCAL_FCT_ATTR
acp_frame_new (
	ACP_CHANNEL_PTR channel,
	LSA_UINT16 frametyp
	);
ACP_LOCAL_FRAME_PTR  ACP_LOCAL_FCT_ATTR
acp_frame_find (
	ACP_CHANNEL_PTR channel,
	LSA_UINT16 acp_id
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_frame_count (
	ACP_CHANNEL_PTR channel
	);

LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_rsp_from_edd_rsp (
	LSA_UINT16  edd_rsp
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_state_provide (
	ACP_CHANNEL_PTR channel
	);
LSA_UINT16  ACP_LOCAL_FCT_ATTR
acp_check_frame_indication (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	LSA_UINT8 ACP_LOCAL_MEM_ATTR  *debug_info
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_link_status_provide (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_link_status_provide_done (
	ACP_LOWER_PTR  channel_lower,
    ACP_LOWER_RQB_PTR_TYPE  edd
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_channel_info (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_channel_info_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_add (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_add_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_control (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_xbuf (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_xbuf_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_global_provider_control (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_global_provider_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_remove (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_remove_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_add (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_add_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_control (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_control_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_xbuf (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_xbuf_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_remove (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_consumer_remove_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_change_phase (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_provider_change_phase_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_get_apdu_status (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_get_apdu_status_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOCAL_FRAME_PTR  frame,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_check_link_status_indication (
	ACP_CHANNEL_PTR  channel
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_multicast (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_multicast_done (
	ACP_CHANNEL_PTR  channel,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);


LSA_BOOL  ACP_LOCAL_FCT_ATTR
acp_edd_overload_provide (
	ACP_LOWER_PTR  channel_lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_overload_provide_done (
	ACP_LOWER_PTR  channel_lower,
	ACP_LOWER_RQB_PTR_TYPE  lower
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_op_overload_provide (
	ACP_CHANNEL_PTR  channel
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_overload_check (
	ACP_LOWER_PTR  channel_lower,
	LSA_UINT32  add_rx,
	LSA_UINT32  add_tx
);

/*-acp_sys.c------------------------------------------------------------------*/

/*-acp_tim.c------------------------------------------------------------------*/

	/* see above */


/*-acp_usr.c------------------------------------------------------------------*/

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback (
	ACP_CHANNEL_PTR channel,
	LSA_UINT32 result
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback_async (
	ACP_CHANNEL_PTR channel,
	LSA_UINT32 result,
	ACP_UPPER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR * rb
	);
LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_callback_and_free_channel (
	ACP_CHANNEL_PTR  channel,
	LSA_UINT32  result,
	ACP_UPPER_RQB_PTR_TYPE  rb
	);

ACP_CHANNEL_PTR  ACP_LOCAL_FCT_ATTR
acp_channel_from_handle (
	LSA_HANDLE_TYPE  channel_handle
	);

LSA_VOID  ACP_LOCAL_FCT_ATTR
acp_run_request (
	ACP_CHANNEL_PTR channel
	);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_INT_H */
