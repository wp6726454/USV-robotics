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
/*  F i l e               &F: sock_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  system interface                                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  2
#define SOCK_MODULE_ID     2

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/* controls whether the keep alive mechanism is used (affects TCP conn only!)*/
LSA_BOOL SOCK_KEEPALIVE_ACTIVE;

#ifdef SOCK_CLEAR_GLOBAL_DATA
SOCK_DATA       SOCK_LOCAL_MEM_ATTR sock_data = { 0 };
#else
SOCK_DATA       SOCK_LOCAL_MEM_ATTR sock_data;
#endif

/*===========================================================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
// initialization of global data
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_SYSTEM_IN_FCT_ATTR
sock_init(LSA_VOID)
{
	SOCK_LOG_FCT("sock_init")

	LSA_INT i;

	SOCK_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,
		"Component SOCK is initialized MAX_CONNECTIONS = %d", SOCK_CFG_MAX_CONNECTIONS);

	sock_data.error.error_code[0] = 0;
	sock_data.error.error_code[1] = 0;
	sock_data.error.error_code[2] = 0;
	sock_data.error.error_code[3] = 0;
	sock_data.error.error_data_length = 0;
	sock_data.error.error_data_ptr = LSA_NULL;
	sock_data.error.line = 0;
	sock_data.error.lsa_component_id = 0;
	sock_data.error.module_id = 0;

	sock_data.sock_channels_opened = 0;

	for(i=0; i<SOCK_CFG_MAX_CHANNELS; i++)
	{
		sock_data.sock_channels[i].state = SOCK_CHANNEL_CLOSED;
	}

	sock_data.sel_status = LSA_FALSE;
	sock_data.socket_count = 0;
	sock_data.active_sockets_last = -1; /* -1 = empty */

	for(i = 0; i <SOCK_MAX_SOCKETS; i++)
	{
		sock_data.sockets[i].state = SOCK_CN_STATE_FREE;
	}

	SOCK_KEEPALIVE_ACTIVE = LSA_TRUE;   /* default is using the "keepalive" mechanism */

#if SOCK_CFG_SNMP_ON

	sock_data.snmp_intern_auto = LSA_FALSE;
	sock_data.snmp_extern_count = 0;
	sock_data.snmp_dev = LSA_NULL;
	sock_data.snmp_recv_buf = LSA_NULL;
	sock_data.snmp_int_agent_return_buf = LSA_NULL;

	sock_data.snmp_data = LSA_NULL;
	sock_data.snmp_data_length = 0;
	sock_data.snmp_pending_count = 0;

	for(i=0; i<SOCK_MAX_SNMP_AGENTS; i++)
	{
		sock_data.snmp_agents_data[i].state = SOCK_SNMP_INACTIVE;
		sock_data.snmp_agents_data[i].snmp_agent_req  = LSA_NULL;
		sock_data.snmp_agents_data[i].hChannel = SOCK_INVALID_HANDLE;
	}
#endif

	/*
	 * mib2 sysgroup data mirror
	 */
	sock_data.sysName_buf[0] = 0;
	sock_data.sysName.pName = sock_data.sysName_buf;
	sock_data.sysName.Length = 0;
	sock_data.sysName.Remanent = LSA_FALSE;

	sock_data.sysDescr_buf[0] = 0;
	sock_data.sysDescr.pName = sock_data.sysDescr_buf;
	sock_data.sysDescr.Length = 0;
	sock_data.sysDescr.Remanent = LSA_FALSE;

	sock_data.sysContact_buf[0] = 0;
	sock_data.sysContact.pName  = sock_data.sysContact_buf;
	sock_data.sysContact.Length = 0;
	sock_data.sysContact.Remanent = LSA_FALSE;

	sock_data.sysLocation_buf[0] = 0;
	sock_data.sysLocation.pName  = sock_data.sysLocation_buf;
	sock_data.sysLocation.Length = 0;
	sock_data.sysLocation.Remanent = LSA_FALSE;

	/*
	 * address info mirror
	 */
	SOCK_MEMSET(sock_data.addr_info_buf, 0, sizeof(sock_data.addr_info_buf));
	for(i=1; i<=SOCK_MAX_INTERFACE_ID; i++)
	{
		SOCK_ADDR_INFO_PTR_TYPE ai = &sock_data.addr_info[i-1];

		ai->InterfaceID = i; /* 1..MAX_INTERFACE_ID */

		ai->NoS.pName = sock_data.addr_info_buf[i-1];
		ai->NoS.Length = 0;
		ai->NoS.Remanent = LSA_FALSE;

		ai->CurrentIPSuite.IpAddr.s_addr = 0;
		ai->CurrentIPSuite.Gateway.s_addr = 0;
		ai->CurrentIPSuite.NetMask.s_addr = 0;

		ai->ExpectedIPSuite.IpAddr.s_addr = 0;
		ai->ExpectedIPSuite.Gateway.s_addr = 0;
		ai->ExpectedIPSuite.NetMask.s_addr = 0;

		ai->EmergencyIPSuite.IpAddr.s_addr = 0;
		ai->EmergencyIPSuite.Gateway.s_addr = 0;
		ai->EmergencyIPSuite.NetMask.s_addr = 0;

		ai->CurrentIPSuiteIsEmergency = LSA_FALSE;
		ai->CurrentIPSuiteConflictDetected = LSA_FALSE;
	}

	/*
	 *  (!) for internal use: posted by TCIP_TRIGGER_SELECT() only (!)
	 */
	SOCK_RQB_SET_OPCODE( &sock_data.select_trigger_rqb, SOCK_OPC_SELECT_TRIGGER );
	sock_data.select_trigger_rqb.args.select_trigger.inUse = 0;


	/* flag and timer for periodic tcp connection check */
	sock_data.do_tcp_disconnect_check = LSA_FALSE;
	{
	LSA_UINT16 ret;
	
	SOCK_ALLOC_TIMER(&ret, &sock_data.tcp_disconnect_check_timer_id, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_1S);

	if (ret != LSA_RET_OK)
	{
		SOCK_FATAL();
	}
	}

	sock_data.close_channel_rqb = LSA_NULL;

	SockIF_Init();

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
// undo initialization of global data
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_SYSTEM_IN_FCT_ATTR
sock_undo_init(LSA_VOID)
{
	LSA_INT i;
	LSA_UINT16 ret_val;

	for(i=0; i<SOCK_CFG_MAX_CHANNELS; i++) /* sanity */
	{
		SOCK_ASSERT(sock_data.sock_channels[i].state == SOCK_CHANNEL_CLOSED);
	}

	/***/

	SOCK_FREE_TIMER(&ret_val, sock_data.tcp_disconnect_check_timer_id);

	if (ret_val != LSA_RET_OK)
	{
		SOCK_FATAL();
	}
	
	SOCK_MEMSET(&sock_data, 0, sizeof(sock_data) );

	return LSA_RET_OK;
}


/*------------------------------------------------------------------------------
// free channel structure
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_channel_free(
	SOCK_CHANNEL_PTR_TYPE channel
) {
	channel->own_handle = 0;
	channel->upper_handle = 0;
	channel->rqb_ptr = LSA_NULL;
	channel->req_done_ptr = LSA_NULL;
	channel->detail_ptr = LSA_NULL;
	return;
}


/*------------------------------------------------------------------------------
// get channel from handle
//----------------------------------------------------------------------------*/
SOCK_CHANNEL_PTR_TYPE SOCK_LOCAL_FCT_ATTR
sock_channel_from_handle(
	LSA_HANDLE_TYPE handle
) {
	SOCK_LOG_FCT("sock_channel_from_handle")

	if( handle < SOCK_CFG_MAX_CHANNELS )
	{
		/* see sock_open_channel() */
		return &sock_data.sock_channels[handle];
	}

	SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
		, "handle(%u) out of range"
		, handle
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// timer callback (see sock_cfg.c)
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_SYSTEM_IN_FCT_ATTR
sock_timeout(
  LSA_TIMER_ID_TYPE timer_id, /* timer-id */
	LSA_USER_ID_TYPE user_id /* */
)
{
	SOCK_LOG_FCT("sock_timeout")

	LSA_UNUSED_ARG(user_id);

	if (timer_id == sock_data.tcp_disconnect_check_timer_id)
	{
		SOCK_EXCHANGE_TYPE rqb_in_use;
		SOCK_UPPER_RQB_PTR_TYPE tr = &sock_data.select_trigger_rqb;
	
		sock_data.do_tcp_disconnect_check = LSA_TRUE; /* check all tcp sockets, see sock_socket_exectute_select() */

		rqb_in_use = SOCK_INTERLOCKED_EXCHANGE(&(tr->args.select_trigger.inUse), 1);

		if (rqb_in_use == 0) /* see sock_usr_close_channel() */
		{
			SOCK_REQUEST_LOCAL(tr);
		}
	}
	else
	{
		SOCK_FATAL();
	}
}


/*------------------------------------------------------------------------------
// fatal error function, no return allowed
//----------------------------------------------------------------------------*/
LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_fatal(
	LSA_UINT16 sock_module_id,
	LSA_INT line
) {
	sock_fatal1(sock_module_id, line, 0);
}


LSA_VOID  SOCK_LOCAL_FCT_ATTR
sock_fatal1(
	LSA_UINT16 sock_module_id,
	LSA_INT line,
	LSA_UINT32 ec_0
) {
	SOCK_LOG_FCT("sock_fatal1")

	SOCK_UPPER_TRACE_03(0, LSA_TRACE_LEVEL_FATAL
		, "module_id(%u) line(%u) ec_0(0x%x)"
		, sock_module_id, line, ec_0
		);

	sock_data.error.lsa_component_id = LSA_COMP_ID_SOCK;
	sock_data.error.module_id        = sock_module_id;
	sock_data.error.line             = (LSA_UINT16)line;

	sock_data.error.error_code[0]    = ec_0;
	sock_data.error.error_code[1]    = 0;
	sock_data.error.error_code[2]    = 0;
	sock_data.error.error_code[3]    = 0;

	sock_data.error.error_data_length = 0;
	sock_data.error.error_data_ptr    = LSA_NULL;

	SOCK_FATAL_ERROR(sizeof(sock_data.error), &sock_data.error);
	/* not reached */
}


/*------------------------------------------------------------------------------
// fatal error function, no return allowed
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
socks_fatal_error(
	LSA_UINT16 module,
	LSA_UINT16 line,
	LSA_UINT16 error_data_length,
	LSA_VOID_PTR_TYPE error_data_ptr,
	LSA_UINT32 error_code0,
	LSA_UINT32 error_code1,
	LSA_UINT32 error_code2,
	LSA_UINT32 error_code3
) {
	SOCK_LOG_FCT("socks_fatal_error")
	SOCK_SYSTEM_TRACE_08(0, LSA_TRACE_LEVEL_FATAL
		, "Fatal error has occurred in SOCK: module=0x%x, line=%d, error_data_length=%d, error_data_ptr=0x%x, code0=0x%x, code1=0x%x, code2=0x%x, code3=0x%x"
		, module, line, error_data_length, error_data_ptr, error_code0, error_code1, error_code2, error_code3
		);


	sock_data.error.lsa_component_id = LSA_COMP_ID_SOCK;
	sock_data.error.module_id = (LSA_UINT16)module;
	sock_data.error.line = (LSA_UINT16)line;
	sock_data.error.error_data_length = error_data_length;
	sock_data.error.error_data_ptr = error_data_ptr;
	sock_data.error.error_code[0] = error_code0;
	sock_data.error.error_code[1] = error_code1;
	sock_data.error.error_code[2] = error_code2;
	sock_data.error.error_code[3] = error_code3;

	SOCK_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &sock_data.error);
	return;
}

/*------------------------------------------------------------------------------
// SNMP callback
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_SYSTEM_IN_FCT_ATTR
sock_mib2_sysgroup_written(
	char *pSysName,    /* != LSA_NULL if written */
	char *pSysContact, /* != LSA_NULL if written */
	char *pSysLocation /* != LSA_NULL if written */
) {
	LSA_UINT16 len;

	/*
	 *  copy to local storage
	 */
	if( sock_is_not_null(pSysName) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysName);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysName.pName, pSysName, len );
			sock_data.sysName.Length = len;
			sock_data.sysName.Remanent = LSA_TRUE;
		}
	}

	if( sock_is_not_null(pSysContact) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysContact);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysContact.pName, pSysContact, len );
			sock_data.sysContact.Length = len;
			sock_data.sysContact.Remanent = LSA_TRUE;
		}
	}

	if( sock_is_not_null(pSysLocation) )
	{
		len = (LSA_UINT16)SOCK_STRLEN(pSysLocation);
		if( len <= SOCK_MAX_MIBII_IFSTRING_LENGTH )
		{
			SOCK_MEMCPY(sock_data.sysLocation.pName, pSysLocation, len );
			sock_data.sysLocation.Length = len;
			sock_data.sysLocation.Remanent = LSA_TRUE;
		}
	}

	/*
	 *  return rqb if available
	 */
	sock_mib2_check_sys_changes();
}

/*------------------------------------------------------------------------------
// InterNiche callbacks
//----------------------------------------------------------------------------*/

#if SOCK_CFG_INTERNICHE

/*-----------------------------------------------
// return pending SOCK_OPC_UDP_MULTICAST_OPEN
//----------------------------------------------*/
#if SOCK_CFG_ENABLE_MULTICASTS
LSA_VOID TCIP_UPPER_OUT_FCT_ATTR 
TCIP_SRV_MULTICAST_DONE(
	LSA_RESPONSE_TYPE result,
	LSA_INT32 dev_handle,
	LSA_UINT8 mode
){
	LSA_RESPONSE_TYPE response;
	SOCK_SOCKET_PTR soc = sock_socket_from_handle(dev_handle);

	SOCK_ASSERT(sock_is_not_null(soc));
	SOCK_ASSERT(sock_is_not_null(soc->channel));
	SOCK_ASSERT(sock_is_not_null(soc->open_close_rb));

	if (mode == TCIP_SRV_MULTICAST_ENABLE) /* return pending open rb */
	{
		switch(result)
		{
		case TCIP_OK:
			{
			SOCK_RQB_ARGS_UDP_OPEN_MULTICAST_PTR_TYPE open_multicast = &soc->open_close_rb->args.udp_open_multicast;

			soc->state = SOCK_CN_STATE_CONNECTED; /* was disabled until now */

			soc->mcast_interface_id = open_multicast->interface_id;
			soc->mcast_address = open_multicast->group_addr.s_addr;

			open_multicast->sock_fd = dev_handle;
			}
			response = SOCK_RSP_OK;
			break;

		case TCIP_ERR_PARAM:
			response = SOCK_RSP_ERR_PARAM;
			break;

		case TCIP_ERR_RESOURCE:
			response = SOCK_RSP_ERR_RESOURCE;
			break;

		default:
			response = SOCK_RSP_ERR_LOC_NET;
		}
	
		sock_usr_callback(soc->channel, response, &soc->open_close_rb);
	
		if( response != SOCK_RSP_OK )
		{
			sock_socket_free(soc); /* close socket if multicast MAC address was not successfully written */
		}
	}
	else /* TCIP_SRV_MULTICAST_DISABLE: return pending close rb */
	{
		soc->mcast_interface_id = 0; /* mcast resource free now, see sock_socket_close() */
		
		sock_socket_close(soc); /* return pending rb */
	}
}
#endif

LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_TRIGGER_SELECT(
	LSA_VOID
)
{
	SOCK_EXCHANGE_TYPE rqb_in_use;

	SOCK_UPPER_RQB_PTR_TYPE tr = &sock_data.select_trigger_rqb;

	rqb_in_use = SOCK_INTERLOCKED_EXCHANGE(&(tr->args.select_trigger.inUse), 1);

	if( rqb_in_use == 0 )
	{
		/* send local trigger-RQB in order not to recurse into interniche stack */
		SOCK_REQUEST_LOCAL(tr);
	}
}

LSA_VOID TCIP_UPPER_OUT_FCT_ATTR
TCIP_MIB2_SYSGROUP_DATA_WRITTEN(
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysName,
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysContact,
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysLocation
) {
	sock_mib2_sysgroup_written(ptr_sysName, ptr_sysContact, ptr_sysLocation);
}

#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
