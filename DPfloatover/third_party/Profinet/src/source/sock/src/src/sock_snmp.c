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
/*  F i l e               &F: sock_snmp.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Productive part of the Socket-Interface                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 6
#define SOCK_MODULE_ID    6

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_SNMP_ON
/*****************************************************************************/


/*------------------------------------------------------------------------------
// activate internal SNMP agent
//----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_snmp_activate_int(
	SOCK_CHANNEL_PTR_TYPE p_cha,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_LOG_FCT("sock_snmp_activate_int")

	LSA_UINT16 snmp_port = (LSA_UINT16)(SOCK_CFG_SNMP_PORTNUMBER == 0 ? 161 : SOCK_CFG_SNMP_PORTNUMBER);
	SOCKIF_FD       fd;
	LSA_INT native_error;
	SockIF_AddrIn   *loc_addr_p;
	LSA_RESPONSE_TYPE response = SOCK_RSP_OK;
	SOCK_SOCKET_PTR soc;
	LSA_VOID* p_tmp;

	soc = LSA_NULL;

	if ((fd = SockIF_Socket(SOCKIF_DGRAM)) != SOCKIF_INVALID_FD)
	{
		soc = sock_socket_alloc(p_cha, fd, SOCKIF_DGRAM);
	}

	if ( sock_is_null(soc) )
	{
		SOCK_RQB_SET_RESPONSE(p_rqb, SOCK_RSP_ERR_RESOURCE);
		return SOCK_RSP_ERR_RESOURCE;
	}

	soc->loc_addr.sin_family = SOCKIF_AF_INET;
	soc->loc_addr.sin_port = SOCK_NTOHS( snmp_port );
	soc->loc_addr.sin_addr.S_un.S_addr = 0;

	loc_addr_p=&soc->loc_addr;

	response = SockIF_Bind(soc->sock_fd, loc_addr_p, &native_error);

	if (response != SOCK_RSP_OK)
	{
		SOCK_PROTOCOL_TRACE_03(0, LSA_TRACE_LEVEL_ERROR
			, "fd(0x%x) CLT BIND error(%d) response(%d)"
			, soc->sock_fd, native_error, response
			);

		sock_socket_free(soc);

		return response;
	}

	soc->rem_addr.sin_family = SOCKIF_AF_INET;

	soc->state = SOCK_CN_STATE_CONNECTED;

	SOCK_ALLOC_LOCAL_MEM(&p_tmp, SOCK_CFG_SNMP_BUFFER_LEN );
	sock_data.snmp_recv_buf = (SOCK_UPPER_MEM_PTR_TYPE)p_tmp;

	if( sock_is_null(sock_data.snmp_recv_buf) )
	{
		sock_socket_free(soc);

		return SOCK_RSP_ERR_RESOURCE;
	}

	SOCK_ALLOC_LOCAL_MEM(&p_tmp, SOCK_CFG_SNMP_BUFFER_LEN );
	sock_data.snmp_int_agent_return_buf = (SOCK_UPPER_MEM_PTR_TYPE)p_tmp;

	if( sock_is_null(sock_data.snmp_int_agent_return_buf) )
	{
		SOCK_FREE_LOCAL_MEM(&response, sock_data.snmp_recv_buf);

		sock_data.snmp_recv_buf = LSA_NULL;

		sock_socket_free(soc);

		return SOCK_RSP_ERR_RESOURCE;
	}

	sock_data.snmp_pending_count = 0; /* socket will be included in select, see sock_socket_execute_select() */

	/*
	 *   internal agent has always same handle (agents are global and have no channel or device relation )
	 */
	sock_data.snmp_agents_data[0].state = SOCK_SNMP_ACTIVE;
	sock_data.snmp_agents_data[0].hChannel = SOCK_RQB_GET_HANDLE(p_rqb);
	sock_data.snmp_agents_data[0].snmp_agent_req = LSA_NULL;

	sock_data.snmp_dev = soc;

	SOCK_PROTOCOL_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,
		"Listen-Sockets SNMP 0x%x for own Port %d with internal Connection Structure %d created ",
		soc->sock_fd, SOCK_HTONS(soc->loc_addr.sin_port), soc->dev_handle );

	return (SOCK_RSP_OK);
}


/*------------------------------------------------------------------------------
// activate external SNMP agent
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_snmp_activate_ext(
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	int i;

	sock_data.snmp_extern_count++;

	/*
	 *  find free handle for new agent
	 */
	for( i=1; i<SOCK_MAX_SNMP_AGENTS; i++)  /* index 0 is internal agent */
	{
		if( sock_data.snmp_agents_data[i].state == SOCK_SNMP_INACTIVE )
		{
			break;
		}
	}

	if( i == SOCK_MAX_SNMP_AGENTS )
	{
		return SOCK_RSP_ERR_RESOURCE;  /* no more handle available */
	}

	p_rqb->args.snmp_act.snmp_handle = (LSA_UINT16)i;

	sock_data.snmp_agents_data[i].state  = SOCK_SNMP_ACTIVE;  /* allocate handle */
	sock_data.snmp_agents_data[i].hChannel = SOCK_RQB_GET_HANDLE(p_rqb);
	sock_data.snmp_agents_data[i].snmp_agent_req = LSA_NULL;

	/*
	 * resources can be provided over any opnened user channel now (snmp is global, no channel granularity)
	 */
	return SOCK_RSP_OK;
}


/*------------------------------------------------------------------------------
// activate SNMP agent
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_snmp_activate(
	SOCK_CHANNEL_PTR_TYPE p_cha,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCKIF_FD               fd;
	LSA_UINT16              response;

	fd = 0;
	response = SOCK_RSP_OK_ACTIVE;

	if ( (p_rqb->args.snmp_act.type == SOCK_INTERNER_SNMP_AGENT))   /* if SNMP type == internal agent */
	{
		if ( sock_data.snmp_agents_data[0].state == SOCK_SNMP_ACTIVE )   /* internal agent active */
		{
			return(SOCK_RSP_ERR_SEQUENCE);	/* internal agent requested but already started */
		}

		response = sock_snmp_activate_int(p_cha, p_rqb);

		if(response == SOCK_RSP_OK)
		{
			p_rqb->args.snmp_act.snmp_handle = 0;   /* internal agent handle is always 0 */

			sock_data.snmp_intern_auto = LSA_FALSE;   /* The internal agent is explicitly started. */
		}

		return response;
	}

	if (p_rqb->args.snmp_act.type == SOCK_EXTERNER_SNMP_AGENT)   /* external agent requested */
	{
		if ( sock_data.snmp_agents_data[0].state == SOCK_SNMP_INACTIVE )
		{
			response = sock_snmp_activate_int(p_cha, p_rqb);   /* activate the internal agent first   */

			if (response != SOCK_RSP_OK)
			{
				return response;
			}

			sock_data.snmp_intern_auto = LSA_TRUE;   /* The internal agent was automatically started. */
		}

		response = sock_snmp_activate_ext( p_rqb);
	}

	return response;
}


/*------------------------------------------------------------------------------
// deactivate SNMP agent
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_snmp_deactivate(
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	LSA_UINT16   response;
	SOCK_RQB_ARGS_SNMP_DEACTIVATE_PTR_TYPE deact = &p_rqb->args.snmp_deact;

	response = SOCK_RSP_OK_ACTIVE;

	if( deact->snmp_handle == 0 )  /* internal agent*/
	{
		if( sock_data.snmp_extern_count != 0 )
		{
			response = SOCK_RSP_ERR_SEQUENCE;
		}
		else
		{
			response = sock_snmp_deactivate_int();
		}
	}
	else							/* external agent*/
	{
		response = sock_snmp_deactivate_ext( p_rqb);

		if( response == SOCK_RSP_OK && sock_data.snmp_extern_count == 0 )
		{
			if (sock_data.snmp_intern_auto)
			{
				response = sock_snmp_deactivate_int();
			}
		}
	}

	return response;

}


/*------------------------------------------------------------------------------
// deactivate internal SNMP agent
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_snmp_deactivate_int(
	LSA_VOID
) {
	LSA_UINT16  retval;

	if (sock_data.snmp_dev->state == SOCK_CN_STATE_CONNECTED)
	{
		/*
		 * stop receive
		 */
		sock_data.snmp_intern_auto = LSA_FALSE;

		sock_data.snmp_agents_data[0].state = SOCK_SNMP_INACTIVE;
		sock_data.snmp_agents_data[0].hChannel = SOCK_INVALID_HANDLE;

		sock_socket_free(sock_data.snmp_dev);

		sock_data.snmp_dev = LSA_NULL;

		SOCK_FREE_LOCAL_MEM(&retval, sock_data.snmp_recv_buf );
		SOCK_FREE_LOCAL_MEM(&retval, sock_data.snmp_int_agent_return_buf );

		if( retval != LSA_RET_OK )
		{
			/*TODO: error treatment*/
		}
		sock_data.snmp_recv_buf = LSA_NULL;

		return(SOCK_RSP_OK);
	}
	else
	{
		return(SOCK_RSP_ERR_SEQUENCE);
	}
}


/*------------------------------------------------------------------------------
// deactivate external SNMP agent
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_snmp_deactivate_ext(
	SOCK_UPPER_RQB_PTR_TYPE rb
) {
	SOCK_UPPER_RQB_PTR_TYPE rb_ret;
	SOCK_RQB_ARGS_SNMP_DEACTIVATE_PTR_TYPE deact = &rb->args.snmp_deact;
	SNMP_AGENT_DATA_PTR pAgent;

	pAgent = &sock_data.snmp_agents_data[deact->snmp_handle];

	/* return SOCK_OPC_SNMP_RECEIVE rqb's to caller */

	rb_ret = sock_snmp_recv_get(pAgent);

	while ( sock_is_not_null(rb_ret) )
	{
		SOCK_CHANNEL_PTR_TYPE channel = sock_channel_from_handle(pAgent->hChannel);
		SOCK_ASSERT(sock_is_not_null(channel));

		sock_usr_callback(channel, SOCK_RSP_OK_WITHDRAW, &rb_ret);

		rb_ret = sock_snmp_recv_get(pAgent);
	}

	pAgent->state = SOCK_SNMP_INACTIVE;
	pAgent->hChannel = SOCK_INVALID_HANDLE;

	sock_data.snmp_extern_count--;

	return (SOCK_RSP_OK);
}


/*------------------------------------------------------------------------------
// put SOCK_OPC_SNMP_RECEIVE rqb into agent's queue
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_snmp_recv_put(
	SNMP_AGENT_DATA_PTR p_AgentData,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_RQB_SET_OPCODE(p_rqb, SOCK_OPC_SNMP_RECEIVE_PROVIDE); /* change from rsp -> provide (see cancel) */

	SOCK_RQB_SET_NEXT_RQB_PTR(p_rqb, p_AgentData->snmp_agent_req );

	p_AgentData->snmp_agent_req = p_rqb;
}


/*------------------------------------------------------------------------------
// remove SOCK_OPC_SNMP_RECEIVE rqb from agent's queue
//----------------------------------------------------------------------------*/
SOCK_UPPER_RQB_PTR_TYPE SOCK_LOCAL_FCT_ATTR
sock_snmp_recv_get(
	SNMP_AGENT_DATA_PTR p_AgentData
) {
	SOCK_UPPER_RQB_PTR_TYPE p_ret_rb = p_AgentData->snmp_agent_req;

	if( sock_is_not_null(p_ret_rb) )
	{
		p_AgentData->snmp_agent_req = SOCK_RQB_GET_NEXT_RQB_PTR(p_ret_rb);
	}

	return p_ret_rb;
}


/*------------------------------------------------------------------------------
// searches for next active agent starting at p_index
//----------------------------------------------------------------------------*/
SNMP_AGENT_DATA_PTR SOCK_LOCAL_FCT_ATTR
sock_snmp_find_next_active_snmp_agent(
	LSA_UINT16 p_index
) {
	int i;

	for(i = p_index; i < SOCK_MAX_SNMP_AGENTS; i++ )
	{
		if( SOCK_SNMP_ACTIVE == sock_data.snmp_agents_data[i].state )
		{
			break;
		}
	}

	if( i == SOCK_MAX_SNMP_AGENTS )
	{
		return LSA_NULL;
	}

	return &sock_data.snmp_agents_data[i];
}


/*------------------------------------------------------------------------------
// pass snmp reuest data block to an snmp agent (internal or "1st" external agent)
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_snmp_dispatch_request(
	LSA_VOID* p_snmp_data,
	LSA_UINT32 p_snmp_data_length
) {
	SOCK_LOG_FCT("sock_snmp_dispatch_request")

	if( sock_data.snmp_extern_count > 0 )
	{
		/*
		 *   extern snmp agent is present - pass snmp data block to first active agent
		 */
		SNMP_AGENT_DATA_PTR pAgent = sock_snmp_find_next_active_snmp_agent(1);

		if( sock_is_not_null(pAgent) )
		{
			sock_snmp_dispatch_extern(pAgent, p_snmp_data, p_snmp_data_length, &sock_data.snmp_dev->rem_addr );
		}
		else
		{
			/*
			 *  there must be an active external agent if snmp_extern_count > 0
			 */
			SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "no active snmp agent found although snmp_extern_count>0 ");
			socks_fatal_error(SOCK_MODULE_ID, SOCK__LINE__, 0, LSA_NULL, 0, 0, 0, 0);
		}
	}
	else   /* pass data to internal agent */
	{
#if SOCK_CFG_INTERNICHE
		{
		/*
		 *  initiate statistics update before dispatching to internal agent
		 */
		LSA_BOOL pending = tcip_mib2_statistics_update();

		if( pending )  /* store snmp data pointer and remote address (see TCIP_MIB2_STATISTICS_UPDATE_DONE) */
		{
			sock_data.snmp_data = p_snmp_data;
			sock_data.snmp_data_length = p_snmp_data_length;
			sock_data.snmp_rem_addr = sock_data.snmp_dev->rem_addr;

			return;
		}
		}
#endif
		sock_snmp_dispatch_intern(p_snmp_data, p_snmp_data_length, &sock_data.snmp_dev->rem_addr );
	}
}


/*------------------------------------------------------------------------------
// pass snmp_data to internal snmp agent and send returned data back to remote address
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_snmp_dispatch_intern(
	LSA_VOID* p_snmp_data,
	LSA_UINT32 p_snmp_data_length,
	SockIF_AddrIn* p_rem_addr
) {
	SOCK_LOG_FCT("sock_snmp_dispatch_intern")

	LSA_UINT16 ret_val;

	SOCK_SNMP_AGT_PARSE((LSA_UINT8*)p_snmp_data, (LSA_UINT)p_snmp_data_length, (LSA_UINT8*)sock_data.snmp_int_agent_return_buf, SOCK_CFG_SNMP_BUFFER_LEN, &ret_val);

	if( ret_val != 0 )
	{
		/* internal snmp agent has returned data - write snmp data to remote address */

		SOCK_RQB_ARGS_DATA_TYPE send;
		
		send.sock_fd = sock_data.snmp_dev->sock_fd;
		send.buffer_length = SOCK_CFG_SNMP_BUFFER_LEN;
		send.buffer_ptr = sock_data.snmp_int_agent_return_buf;
		send.data_length = ret_val;
		send.flags = 0;
		send.offset = 0;
		send.rem_addr = *p_rem_addr;
		
		sock_udp_send(sock_data.snmp_dev, &send); /* ignore return value */

		//SockIF_SendTo(sock_data.snmp_dev->sock_fd,(char *)sock_data.snmp_int_agent_return_buf, ret_val, p_rem_addr, &bytes_written, &native_error);		
	}
	else
	{
		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "internal snmp agemt did not return data - discard received snmp data!  ");
	}

	/* allow reading next snmp request */

	sock_data.snmp_pending_count = 0;
}


/*------------------------------------------------------------------------------
// pass snmp data to external snmp agent
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_snmp_dispatch_extern(
	SNMP_AGENT_DATA_PTR p_AgentData,
	LSA_VOID* p_snmp_data,
	LSA_UINT32 p_snmp_data_length,
	SockIF_AddrIn* p_rem_addr
) {
	SOCK_LOG_FCT("sock_snmp_dispatch_extern")

	/* get snmp receive rqb and pass data */

	SOCK_UPPER_RQB_PTR_TYPE snmp_recv_rb = sock_snmp_recv_get(p_AgentData);

	if( sock_is_null(snmp_recv_rb) )
	{
		/* there is no receive request rqb available for 1st external agent -> snmp request is discarded */

		SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "no resource available for 1st external snmp agent! - discard received snmp data!  ");
	}
	else
	{
		SOCK_RQB_ARGS_SNMP_RECEIVE_PTR_TYPE recv = & snmp_recv_rb->args.snmp_recv;

		if( recv->buffer_size < p_snmp_data_length )
		{
			/* buffer provided by external agent too small -> snmp request is discarded */

			SOCK_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "buffer provided by agent too small (%d), need (%d); discard received snmp data!", recv->buffer_size, p_snmp_data_length );
		}
		else
		{
			SOCK_CHANNEL_PTR_TYPE channel = sock_channel_from_handle(p_AgentData->hChannel);
			SOCK_ASSERT(sock_is_not_null(channel));

			SOCK_MEMCPY(recv->buffer, p_snmp_data, p_snmp_data_length );
			recv->rem_addr = *p_rem_addr;
			recv->data_length = (LSA_UINT16)p_snmp_data_length;
			recv->snmp_done = LSA_FALSE;/* reset done flag */

			SOCK_RQB_SET_OPCODE(snmp_recv_rb, SOCK_OPC_SNMP_RECEIVE_IND);
			sock_usr_callback(channel, SOCK_RSP_OK, &snmp_recv_rb);

			return; /* allow treatment of discard below */
		}
	}

	/* snmp request was discarded --> allow reading next snmp request */

	sock_data.snmp_pending_count = 0;


}


/*------------------------------------------------------------------------------
// treat snmp agents' re-provide
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_snmp_receive_rsp(
	SNMP_AGENT_DATA_PTR p_AgentData,
	SOCK_UPPER_RQB_PTR_TYPE p_rqb
) {
	SOCK_LOG_FCT("sock_snmp_receive_rsp")

	SOCK_RQB_ARGS_SNMP_RECEIVE_PTR_TYPE rcv = & p_rqb->args.snmp_recv;

	sock_snmp_recv_put(p_AgentData, p_rqb);

	if( rcv->snmp_done )
	{
		/* snmp request is finished - pass back to remote address */
		SOCK_RQB_ARGS_DATA_TYPE send;

		send.buffer_length = rcv->buffer_size;
		send.buffer_ptr = rcv->buffer;
		send.data_length = rcv->data_length;
		send.flags = 0;
		send.offset = 0;
		send.rem_addr = rcv->rem_addr;
		send.sock_fd = sock_data.snmp_dev->sock_fd;

		sock_udp_send(sock_data.snmp_dev, &send);
		
		//SockIF_SendTo(sock_data.snmp_dev->sock_fd,(char *) rcv->buffer, rcv->data_length, &rcv->rem_addr, &bytes_written, &native_error );

		sock_data.snmp_pending_count = 0; /* snmp reuest is finished - make ready to get next request */

		/* ignore write errors, see  */
	}
	else
	{
		SNMP_AGENT_DATA_PTR pNextAgent;

		LSA_UINT16 next_snmp_handle = (LSA_UINT16)(SOCK_RQB_GET_SNMP_HANDLE(p_rqb) + 1);

		pNextAgent = sock_snmp_find_next_active_snmp_agent( next_snmp_handle );

		if( sock_is_not_null(pNextAgent) )
		{
			/*
			 *  dispatch to "next" external agent.
			 */
			sock_snmp_dispatch_extern(pNextAgent, rcv->buffer, rcv->data_length, &rcv->rem_addr);
		}
		else
		{
			/*
			 *   "last" snmp agent did not return a valid snmp answer-> dispatch snmp request to internal agent
			 */
#if SOCK_CFG_INTERNICHE
			/*
			 *  initiate statistics update before dispatching to internal agent
			 */
			{
			LSA_BOOL pending = tcip_mib2_statistics_update();

			if( pending )  /* store snmp data pointer and remote address (see TCIP_MIB2_STATISTICS_UPDATE_DONE) */
			{
				sock_data.snmp_data = rcv->buffer;
				sock_data.snmp_data_length = rcv->data_length;
				sock_data.snmp_rem_addr = rcv->rem_addr;

				return;
			}
			}
#endif
			sock_snmp_dispatch_intern(rcv->buffer, rcv->data_length, &rcv->rem_addr );
		}
	}
}


#endif


/*------------------------------------------------------------------------------
// pass stored snmp data pointer to internal snmp agent
//----------------------------------------------------------------------------*/
#if SOCK_CFG_INTERNICHE

LSA_VOID TCIP_UPPER_OUT_FCT_ATTR
TCIP_MIB2_STATISTICS_UPDATE_DONE(
	LSA_VOID
)
{
	sock_snmp_dispatch_intern( sock_data.snmp_data, sock_data.snmp_data_length, &sock_data.snmp_rem_addr);
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
