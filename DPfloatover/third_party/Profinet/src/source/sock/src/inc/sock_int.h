#ifndef SOCK_INT_H
#define SOCK_INT_H
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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_int.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*      internal used defines, types, functions                              */
/*                                                                           */
/*****************************************************************************/

#include "sock_inc.h"
#include "sock_low.h"


#if SOCK_CFG_SNMP_ON
#ifndef SOCK_CFG_SNMP_PORTNUMBER
#error "SOCK_CFG_SNMP_PORTNUMBER is not defined in sock_cfg.h"
#endif
#endif

/*===========================================================================*/

#ifndef SOCK_LOG_FCT
# define SOCK_LOG_FCT(fn_)  /* empty */
#endif


/*------------------------------------------------------------------------------
//	defines
//----------------------------------------------------------------------------*/

#define SOCK_RSP_OK_PROGRESS           (LSA_RSP_OK_OFFSET +  10)    /* (0x4C) Internal Error code to map EINPROGRESS */

#define sock_is_null(ptr_)			(LSA_HOST_PTR_ARE_EQUAL(SOCK_TEST_POINTER(ptr_), LSA_NULL))
#define sock_is_not_null(ptr_)		(! LSA_HOST_PTR_ARE_EQUAL(SOCK_TEST_POINTER(ptr_), LSA_NULL))
#define sock_are_equal(ptr1_, ptr2_)	(LSA_HOST_PTR_ARE_EQUAL(SOCK_TEST_POINTER(ptr1_), SOCK_TEST_POINTER(ptr2_)))

/***/

/*-----------------------------------------------------------------------------
// circularly linked list macros
// adapted from Microsoft Header SRVSTRUC.H and WDM.H
//---------------------------------------------------------------------------*/

typedef struct sock_list_entry_tag SOCK_LOCAL_MEM_ATTR *SOCK_LIST_ENTRY_PTR_TYPE;

typedef struct sock_list_entry_tag {
	SOCK_LIST_ENTRY_PTR_TYPE Flink; /* forward link */
	SOCK_LIST_ENTRY_PTR_TYPE Blink; /* backward link */
} SOCK_LIST_ENTRY_TYPE;

#define SockListInitialize(ListHead_) {\
	(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
	}

#define SockListIsEmpty(ListHead_) (\
	(ListHead_)->Flink == (ListHead_)\
	)

#define SockListRemoveEntry(Entry_) {\
	SOCK_LIST_ENTRY_PTR_TYPE EX_Entry_;\
	EX_Entry_ = (Entry_);\
	EX_Entry_->Blink->Flink = EX_Entry_->Flink;\
	EX_Entry_->Flink->Blink = EX_Entry_->Blink;\
	}

#define SockListRemoveHead(ListHead_, Entry_, Type_) {\
	(Entry_) = (Type_)((ListHead_)->Flink);\
	SockListRemoveEntry((ListHead_)->Flink);\
	}

#define SockListInsertHead(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_)->Flink;\
	(Entry_)->Blink = (ListHead_);\
	(ListHead_)->Flink->Blink = (Entry_);\
	(ListHead_)->Flink = (Entry_);\
	}

#define SockListInsertTail(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_);\
	(Entry_)->Blink = (ListHead_)->Blink;\
	(ListHead_)->Blink->Flink = (Entry_);\
	(ListHead_)->Blink = (Entry_);\
	}

#define SockListPeekHead(ListHead_) (\
	SockListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink\
	)

#define SockListFirst(ListHead_, Type_) (\
	(Type_)(SockListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
	)

#define SockListNext(ListHead_, Entry_, Type_) (\
	(Type_)((Entry_)->Flink == (ListHead_) ? LSA_NULL : (Entry_)->Flink)\
	)

/*------------------------------------------------------------------------------
// RQB-Queue
//----------------------------------------------------------------------------*/

typedef struct {
	SOCK_LIST_ENTRY_TYPE anchor;
} SOCK_QUEUE_TYPE, SOCK_LOCAL_MEM_ATTR *SOCK_QUEUE_PTR_TYPE;

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_queue_init(
	SOCK_QUEUE_PTR_TYPE q
);

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_queue_enqueue(
	SOCK_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
);

SOCK_UPPER_RQB_PTR_TYPE  SOCK_LOCAL_FCT_ATTR
sock_queue_dequeue(
	SOCK_QUEUE_PTR_TYPE q
);

LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_queue_empty(
	SOCK_QUEUE_PTR_TYPE q
);

LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_queue_not_empty(
	SOCK_QUEUE_PTR_TYPE q
);

LSA_VOID_PTR_TYPE SOCK_LOCAL_FCT_ATTR
sock_queue_peek(
	SOCK_QUEUE_PTR_TYPE q
);

LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_queue_unlink (
	SOCK_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
);



/*------------------------------------------------------------------------------
// types
//----------------------------------------------------------------------------*/

typedef struct sock_rqb_tag  SOCK_LOCAL_MEM_ATTR *  SOCK_LOCAL_RQB_PTR_TYPE;




#if SOCK_CFG_SNMP_ON

#define SOCK_MAX_EXTERNAL_SNMP_AGENTS 10
#define SOCK_MAX_SNMP_AGENTS		  (1+SOCK_MAX_EXTERNAL_SNMP_AGENTS)


typedef enum
{
	SOCK_SNMP_INACTIVE = 0,
	SOCK_SNMP_ACTIVE
} SNMP_STATE;


/*------------------------------------------------------------------------------
// SNMP agent data structure
//----------------------------------------------------------------------------*/

typedef struct snmp_agent_tag
{
	SNMP_STATE				state;           /* initially SOCK_SNMP_INACTIVE -->  SOCK_SNMP_ACTIVE on activation */
	SOCK_UPPER_RQB_PTR_TYPE snmp_agent_req;  /* resources of ext. agents. 1st index internal agent, not used */
	LSA_HANDLE_TYPE         hChannel;        /* cross reference to channel which activated agent */

} SNMP_AGENT_DATA, * SNMP_AGENT_DATA_PTR;

#endif


/*------------------------------------------------------------------------------
// LSA-channel states
//----------------------------------------------------------------------------*/

typedef enum
{
	SOCK_CHANNEL_CLOSED = 0,
	SOCK_CHANNEL_OPENED = 1,
	SOCK_CHANNEL_PENDING_CLOSE = 2
} SOCK_CHANNEL_STATE;


/*------------------------------------------------------------------------------
// management structure for a channel
//----------------------------------------------------------------------------*/

typedef struct sock_channel_tag
{
	SOCK_CHANNEL_STATE      state;			/* state of the channel */
	LSA_HANDLE_TYPE         own_handle;     /* own handle */
	LSA_HANDLE_TYPE         upper_handle;   /* handle of user */
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr;        /* requestblock pointer */

	LSA_VOID LSA_FCT_PTR(SOCK_UPPER_OUT_FCT_ATTR, req_done_ptr) (SOCK_UPPER_RQB_PTR_TYPE rqb); /* pointer on the callback function */

	LSA_SYS_PTR_TYPE        sys_ptr;        /* SYS pointer of the system */
	SOCK_DETAIL_PTR_TYPE    detail_ptr;     /* DETAIL-Pointer internal */

	/* storage for the ip suite informations: */
	SOCK_ADDR_INFO_TYPE     addr_info;		/* storage of the current setting in IP Suite */
	SOCK_UPPER_RQB_PTR_TYPE rqb_addr_ind;	/* Requestblock-Pointer to address indication rqb(s) */
	SOCK_UPPER_RQB_PTR_TYPE rqb_mib2_ind;	/* Requestblock-Pointer to Mib2Params indication rqb */
} SOCK_CHANNEL_TYPE;

typedef struct sock_channel_tag SOCK_LOCAL_MEM_ATTR * SOCK_CHANNEL_PTR_TYPE;


/*------------------------------------------------------------------------------
//	states of a socket
//----------------------------------------------------------------------------*/

typedef enum
{
	SOCK_CN_STATE_FREE = 0,				/* free socket struct */
	SOCK_CN_STATE_OPENED = 0x1,			/* state after SOCK_OPC_OPEN, to be used with SOCK_OPC_CONNECT_WITH_OPEN only */
	SOCK_CN_STATE_CONNECTING = 0x2,		/* connecting tcp client socket, waiting for server's accept */
	SOCK_CN_STATE_CONNECTED = 0x4,		/* tcp socket in connected state, udp socket in operation state */
	SOCK_CN_STATE_LISTENING = 0x8,		/* open tcp server socket, waiting for client's connect */
	SOCK_CN_STATE_DISCONNECTING = 0x10,	/* socket is about to close (user must send SOCK_OPC_CLOSE), resources are cancelled */
	SOCK_CN_STATE_DISABLED = 0x20		/* disabled (not active) socket, see sock_socket_close() also */
} SOCK_STATE;

#define SOCK_CN_STATE_ACTIVE_MASK   (SOCK_CN_STATE_CONNECTED \
									| SOCK_CN_STATE_CONNECTING \
									| SOCK_CN_STATE_LISTENING)

/*------------------------------------------------------------------------------
// management structure for a socket
//----------------------------------------------------------------------------*/

typedef struct sock_socket_data_tag
{
	LSA_UINT32 bytes_transferred;               /* for SOCK_READ_ALL/SOCK_SEND_ALL: store bytes read/sent */
	SOCK_QUEUE_TYPE queue;                      /* pending rqb's */
	SOCK_UPPER_RQB_PTR_TYPE curr;               /* current unqueued rqb "in work" */

} SOCK_SOCKET_DATA;


typedef struct sock_socket_tag {

	LSA_UINT16              dev_handle;             /* own handle */
	LSA_INT32               active_handle;          /* index into to sock_data.active_sockets[], see sock_socket_alloc() and sock_socket_free() */
	SOCK_STATE              state;                  /* state of socket */
	SOCK_CHANNEL_PTR_TYPE   channel;                /* pointer on channel structure */

	SOCKIF_FD               sock_fd;                /* socket descriptor */
	LSA_INT32               type;                   /* SOCKIF_DGRAM for UDP, SOCKIF_STREAM for TCP sockets */

	SockIF_AddrIn           loc_addr;               /* local port and IP-address */
	SockIF_AddrIn           rem_addr;               /* remote port and IP-address */

	LSA_UINT32              mcast_interface_id;     /* InterfaceID (1..16) if socket is assigned to multicasts or 0 if not */
	LSA_UINT32              mcast_address;          /* multicast group address the socket has joined (see SOCK_OPC_UDP_MULTICAST_OPEN) */

	LSA_UINT16              abort_resp;             /* reason for last connection clearing */

	SOCK_SOCKET_DATA		send;                       /* send data */
	SOCK_SOCKET_DATA		recv;                       /* receive data */

	SOCK_UPPER_RQB_PTR_TYPE exception_rb;           /* pending SOCK_OPC_EXCEPTION_NOTIFY request block */
	SOCK_UPPER_RQB_PTR_TYPE connect_rb;             /* pending connect rqb - only valid in state CONNECTING */
	SOCK_UPPER_RQB_PTR_TYPE open_close_rb;          /* pending SOCK_OPC_CLOSE or SOCK_OPC_UDP_MULTICAST_OPEN */

	struct {
		SOCK_QUEUE_TYPE queue;                        /* pending SOCK_OPC_ACCEPT_NOTIFY rqb's */
		SOCK_UPPER_RQB_PTR_TYPE curr;                 /* current unqueued accept rqb "in work" */
	} accept;

} SOCK_SOCKET;

typedef struct sock_socket_tag SOCK_LOCAL_MEM_ATTR * SOCK_SOCKET_PTR;

typedef struct sock_socket_data_tag SOCK_LOCAL_MEM_ATTR * SOCK_SOCKET_DATA_PTR;

#define SOCK_MAX_SOCKETS (SOCK_CFG_MAX_CONNECTIONS+1)


/*------------------------------------------------------------------------------
// socket component data
//----------------------------------------------------------------------------*/

typedef struct sock_data_tag
{
	struct lsa_fatal_error_tag   error;

	LSA_INT32               socket_count;

	SOCK_CHANNEL_TYPE       sock_channels[SOCK_CFG_MAX_CHANNELS];
	LSA_UINT8				sock_channels_opened;

	SOCK_SOCKET             sockets[SOCK_MAX_SOCKETS];
	SOCK_SOCKET_PTR         active_sockets[SOCK_MAX_SOCKETS]; /* pointers to all opened sockets in sockets[], see sock_socket_execute_select() */
	LSA_INT32				active_sockets_last; /* (!) -1 = empty array, contains index of last entry if populated */

	LSA_BOOL                sel_status;

#if SOCK_CFG_SNMP_ON

	LSA_BOOL                snmp_intern_auto;   /* = TRUE if internal SNMP-agent was activated automatically */
	LSA_UINT16              snmp_extern_count;
	SOCK_SOCKET_PTR         snmp_dev;           /* references snmp socket "device" */
	SOCK_UPPER_MEM_PTR_TYPE snmp_recv_buf;      /* receive buffer for snmp data */
	SOCK_UPPER_MEM_PTR_TYPE	snmp_int_agent_return_buf; /* return buffer for internal agent parse */
	SNMP_AGENT_DATA         snmp_agents_data[SOCK_MAX_SNMP_AGENTS];
	LSA_UINT8				snmp_pending_count; /* 0 (no snmp request is "in work") or 1 (snmp request pending) */

	/* snmp data */
	LSA_VOID*               snmp_data; /* snmp data passed to agents */
	LSA_UINT32				snmp_data_length; /* length of snmp data */
	SockIF_AddrIn			snmp_rem_addr; /* snmp request's source address */
#endif

	/* mib2 sysgroup data mirror */
	LSA_UINT8				sysName_buf[SOCK_MAX_MIBII_IFSTRING_LENGTH+1];  /* local copy for SOCK_OPC_MIB2_SYSGROUP_DATA_IND */
	SOCK_NAME_TYPE			sysName; /* sock_init sets pName to sysName_buf */

	LSA_UINT8			    sysDescr_buf[SOCK_MAX_MIBII_IFSTRING_LENGTH+1];
	SOCK_NAME_TYPE		    sysDescr;

	LSA_UINT8			    sysContact_buf[SOCK_MAX_MIBII_IFSTRING_LENGTH+1];
	SOCK_NAME_TYPE			sysContact;

	LSA_UINT8			    sysLocation_buf[SOCK_MAX_MIBII_IFSTRING_LENGTH+1];
	SOCK_NAME_TYPE			sysLocation;

	/* SOCK_OPC_ADDR_INFO_IND: local storage for current ip suite settings of all interfaces */
	SOCK_ADDR_INFO_TYPE		addr_info[SOCK_MAX_INTERFACE_ID]; /* index = InterfaceID - 1 */
	LSA_UINT8				addr_info_buf[SOCK_MAX_INTERFACE_ID][SOCK_NAME_OF_STATION_LENGTH_MAX];

	SOCK_RQB_TYPE           select_trigger_rqb; /* local rqb for TCIP_TRIGGER_SELECT() */

	LSA_BOOL                do_tcp_disconnect_check; /* flag set by disconnect check timer, eval see sock_socket_execute_select() */
	LSA_TIMER_ID_TYPE       tcp_disconnect_check_timer_id; /* disconnect check timer */

	SOCK_UPPER_RQB_PTR_TYPE close_channel_rqb; /* store close channel rqb while waiting for disconnect timer, see sock_close_channel()  */
} SOCK_DATA;

extern SOCK_DATA SOCK_LOCAL_MEM_ATTR sock_data;

extern LSA_BOOL SOCK_KEEPALIVE_ACTIVE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_usr_callback( SOCK_CHANNEL_PTR_TYPE channel, LSA_UINT16 response, SOCK_UPPER_RQB_PTR_TYPE SOCK_LOCAL_MEM_ATTR *upper_ptr );
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_usr_open_channel(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_get_any_mac_addr(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_set_addr_info(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_BOOL SOCK_LOCAL_FCT_ATTR sock_usr_compare_ip_suite(SOCK_ADDR_INFO_PTR_TYPE p_ind, SOCK_ADDR_INFO_PTR_TYPE p_curr);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_usr_check_addr_info_changes(SOCK_ADDR_INFO_PTR_TYPE p_ai_mirror);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_usr_copy_addr_info(SOCK_ADDR_INFO_PTR_TYPE p_dest, SOCK_ADDR_INFO_PTR_TYPE p_src);

LSA_UINT16 SOCK_LOCAL_FCT_ATTR  sock_usr_addr_info_ind(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE p_rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR  sock_usr_except_notify(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR  sock_usr_accept_notify(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);

LSA_UINT16 SOCK_LOCAL_FCT_ATTR  sock_usr_close(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR  sock_usr_GetHostByName(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_usr_udp_connect(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);

#if SOCK_CFG_SNMP_ON

LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_snmp_activate(SOCK_CHANNEL_PTR_TYPE p_cha, SOCK_UPPER_RQB_PTR_TYPE p_rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_snmp_deactivate(SOCK_UPPER_RQB_PTR_TYPE p_rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_snmp_receive_provide(SOCK_UPPER_RQB_PTR_TYPE p_rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_usr_snmp_receive_rsp(SOCK_UPPER_RQB_PTR_TYPE p_rqb);

#endif

LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_mib2_set_system_params(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_mib2_set_if_params(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_mib2_set_sysgroup_data_ind(SOCK_CHANNEL_PTR_TYPE cha, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_mib2_get_ifindex(SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_VOID   SOCK_LOCAL_FCT_ATTR sock_mib2_check_sys_changes(LSA_VOID);

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_connect(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_connect_with_open(SOCK_SOCKET_PTR soc, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_VOID   SOCK_LOCAL_FCT_ATTR sock_tcp_connect_client_socket(SOCK_SOCKET_PTR soc);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_connect_cancel(SOCK_CHANNEL_PTR_TYPE channel);
LSA_VOID   SOCK_LOCAL_FCT_ATTR sock_tcp_accept(SOCK_SOCKET_PTR listening_soc);
LSA_VOID   SOCK_LOCAL_FCT_ATTR sock_tcp_enable_accept(SOCK_SOCKET_PTR dev);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_listen(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_open(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_receive(SOCK_SOCKET_PTR soc, SOCK_RQB_ARGS_DATA_PTR_TYPE data);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_tcp_send(SOCK_SOCKET_PTR soc, SOCK_RQB_ARGS_DATA_PTR_TYPE data);

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_open(SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_open_multicast (SOCK_CHANNEL_PTR_TYPE channel, SOCK_UPPER_RQB_PTR_TYPE rqb);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_read(LSA_INT32* bytes_read,SOCK_SOCKET_PTR soc,LSA_VOID* buffer,LSA_UINT32 length,LSA_BOOL peek,SockIF_AddrIn* address);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_receive(SOCK_SOCKET_PTR soc, SOCK_RQB_ARGS_DATA_PTR_TYPE data);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_send( SOCK_SOCKET_PTR soc, SOCK_RQB_ARGS_DATA_PTR_TYPE data);
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_udp_connect(SOCK_SOCKET_PTR soc, SOCK_RQB_ARGS_UDP_CONN_PTR_TYPE udp_connect);

#if SOCK_CFG_SNMP_ON

LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_snmp_activate( SOCK_CHANNEL_PTR_TYPE cha, SOCK_UPPER_RQB_PTR_TYPE p_rqb );
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_snmp_deactivate( SOCK_UPPER_RQB_PTR_TYPE p_rqb );

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_exception( SOCK_SOCKET_PTR dev);

LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_snmp_deactivate_int( LSA_VOID);
LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_snmp_deactivate_ext( SOCK_UPPER_RQB_PTR_TYPE rb);

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_recv_put( SNMP_AGENT_DATA_PTR p_AgentData, SOCK_UPPER_RQB_PTR_TYPE p_rqb);
SOCK_UPPER_RQB_PTR_TYPE SOCK_LOCAL_FCT_ATTR sock_snmp_recv_get(SNMP_AGENT_DATA_PTR p_AgentData);

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_dispatch_request(LSA_VOID* p_snmp_data, LSA_UINT32 p_snmp_data_length);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_dispatch_intern( LSA_VOID* p_snmp_data, LSA_UINT32 p_snmp_data_length, SockIF_AddrIn* p_rem_addr );
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_dispatch_extern( SNMP_AGENT_DATA_PTR p_AgentData, LSA_VOID* p_snmp_data, LSA_UINT32 p_snmp_data_length, SockIF_AddrIn* p_rem_addr );

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_snmp_receive_rsp(SNMP_AGENT_DATA_PTR p_AgentData, SOCK_UPPER_RQB_PTR_TYPE p_rqb);

SNMP_AGENT_DATA_PTR SOCK_LOCAL_FCT_ATTR sock_snmp_find_next_active_snmp_agent(LSA_UINT16 p_index );

#endif

/*------------------------------------------------------------------------------
//	socket structure functions
//----------------------------------------------------------------------------*/

SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR sock_socket_alloc(SOCK_CHANNEL_PTR_TYPE channel, SOCKIF_FD fd, LSA_INT32 type);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_free(SOCK_SOCKET_PTR soc);

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_return_all_resources(SOCK_SOCKET_PTR dev, LSA_UINT16 abort_resp);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_return_send_resources(SOCK_SOCKET_PTR soc, LSA_UINT16 response);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_return_connect_resource(SOCK_SOCKET_PTR soc, LSA_UINT16 response);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_return_exception_resource(SOCK_SOCKET_PTR soc, LSA_UINT16 response);
LSA_INT  SOCK_LOCAL_FCT_ATTR sock_socket_return_receive_resources(SOCK_SOCKET_PTR soc, LSA_UINT16 response);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_return_accept_notify_resources(SOCK_SOCKET_PTR soc, LSA_UINT16 response);

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR sock_socket_set_options(SOCK_CHANNEL_PTR_TYPE channel, SOCKIF_FD fd, LSA_INT32 type);

SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR sock_socket_from_handle(LSA_UINT32 handle);
SOCK_SOCKET_PTR SOCK_LOCAL_FCT_ATTR sock_socket_from_channel(SOCK_CHANNEL_PTR_TYPE channel, LSA_UINT32 handle);

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_receive(SOCK_SOCKET_PTR soc);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_send(SOCK_SOCKET_PTR soc);

LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_do_select(SOCK_UPPER_RQB_PTR_TYPE rqb_ptr);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_socket_execute_select(LSA_UINT32 timeout);

LSA_VOID sock_socket_close(SOCK_SOCKET_PTR soc);

/*------------------------------------------------------------------------------
//	channel functions
//----------------------------------------------------------------------------*/

SOCK_CHANNEL_PTR_TYPE SOCK_LOCAL_FCT_ATTR sock_channel_from_handle(LSA_HANDLE_TYPE handle);
LSA_VOID SOCK_LOCAL_FCT_ATTR sock_channel_free(SOCK_CHANNEL_PTR_TYPE channel);

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of SOCK_INT_H */
