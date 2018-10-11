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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cm.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail setting for CM                                            */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      303
#define LTRC_ACT_MODUL_ID  303

#include "psi_int.h"

#if ( PSI_CFG_USE_CM == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_pnstack.h"
#include "psi_ld.h"

#if ( PSI_CFG_USE_HIF_HD ) // Using HIF HD
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "cm_inc.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== CM ====================================================================*/
/*===========================================================================*/

LSA_RESULT psi_cm_get_path_info(
	LSA_SYS_PTR_TYPE        *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE       *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path )
{
	CM_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE   pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16 result        = LSA_RET_OK;

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_cm_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	switch( path )
	{
	case PSI_PATH_IF_APP_CMPD: /* physical device */
		{
			pDetail->path_type = CM_PATH_TYPE_PD;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.pd.epm.recv_resource_count = 16;
			pDetail->u.pd.epm.send_resource_count = 16;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_USER; // CMPD user
#endif
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_APP_IOC_CMCL_ACP: /* IO Controller */
		{
			pDetail->path_type = CM_PATH_TYPE_CLIENT;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.cl.rpc.alloc_len                          = pDetailsIF->cm.cl.max_record_length;    // max. record length
			pDetail->u.cl.rpc.call_resource_count                = pDetailsIF->cm.cl.device_count;         // all possible devices may call back (prm-end) in parallel
			pDetail->u.cl.rpc.call_resource_count                += pDetailsIF->cm.cl.max_read_implicit;   // for read-implicit (sync-log)

			pDetail->u.cl.rpc.recv_resource_count                = (pDetailsIF->cm.cl.device_count + pDetailsIF->cm.cl.max_read_implicit + 5);  //???ToBeLookedAt, f(turn-around-time, line-speed, ...) //(LaM)NOTE (pDetailsIF->cm.cl.device_count + ?)
			pDetail->u.cl.rpc.send_resource_count                = (LSA_UINT16)((pDetailsIF->cm.cl.device_count * 0.2) + 1);  //???ToBeLookedAt //(LaM)NOTE =8 (2/msec sollen versendet werden => 2 Resc im EDD + 2 im Clrpc + 2 auf dem Weg EDD / Clrpc + 2 auf dem Weg Clrpc / EDD)

			pDetail->u.cl.acp.state_ind_resource_count           = pDetailsIF->cm.cl.device_count;
			pDetail->u.cl.acp.alarm_high_prio_ind_resource_count = pDetailsIF->cm.cl.device_count;
			pDetail->u.cl.acp.alarm_low_prio_ind_resource_count  = pDetailsIF->cm.cl.device_count;
			pDetail->u.cl.acp.max_alarm_data_length              = pDetailsIF->cm.cl.max_alarm_data_length;

			pDetail->u.cl.max_connects_per_second = PSI_MAX_CONNECTS_PER_SECOND;
			pDetail->u.cl.min_reconnect_timeout   = 1;   // in seconds (default)
			pDetail->u.cl.schedule_timeout        = 200; // in milli-seconds, must match FSU requirements

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD; // CMCL user
#else
			rsp_mbx_id = PSI_MBX_ID_USER;    // CMCL user
#endif
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	case PSI_PATH_IF_APP_IOM_CMMC_ACP: /* IO Multicast */
		{
			pDetail->path_type = CM_PATH_TYPE_MULTICAST;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.mc.state_ind_resource_count = 10;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD; // CMMC user
#else
			rsp_mbx_id = PSI_MBX_ID_USER;   // CMMC user
#endif
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	case PSI_PATH_IF_APP_IOD_CMSV_ACP: /* IO Device */
		{
			pDetail->path_type = CM_PATH_TYPE_SERVER;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.sv.rpc.alloc_len                          = pDetailsIF->cm.sv.max_record_length;     /*use max*/ // CM_RECORD_OFFSET + 32*1024; // 4K mandatory record length
			pDetail->u.sv.rpc.call_resource_count                = (pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar);        // all devices may get connected to in parallel
			pDetail->u.sv.rpc.call_resource_count                += (pDetailsIF->cm.sv.device_count* pDetailsIF->cm.sv.max_read_implicit); // for read-implicit
			pDetail->u.sv.rpc.recv_resource_count                = 20; //???ToBeLookedAt, f(turn-around-time, line-speed, ...)
			pDetail->u.sv.rpc.send_resource_count                = 20; //???ToBeLookedAt

			pDetail->u.sv.acp.state_ind_resource_count           = 10;
			pDetail->u.sv.acp.alarm_high_prio_ind_resource_count = 20; //???ToBeLookedAt, no loss! alarms stay in ACP!
			pDetail->u.sv.acp.alarm_low_prio_ind_resource_count  = 20; //???ToBeLookedAt, no loss! alarms stay in ACP!
			pDetail->u.sv.acp.max_alarm_data_length              = pDetailsIF->cm.sv.max_alarm_data_length;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD; // CMMC user
#else
			rsp_mbx_id = PSI_MBX_ID_USER;   // CMSV user
#endif
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	case PSI_PATH_IF_SYS_CM_CLRPC:
		{
			// Note: from CM to CLRPC an IF specific channel is opened
			pDetail->path_type = CM_PATH_TYPE_RPC;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.rpc.recv_resource_count = 20; /* taken from clrpc path info, may a wild guess ... */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id    = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_CLRPC;
		}
		break;

#if ( PSI_CFG_USE_NARE == 1 )
	case PSI_PATH_IF_SYS_CM_NARE:
		{
			pDetail->path_type = CM_PATH_TYPE_NARE;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.nare.resolve_notify_resource_count = PSI_MAX_CONNECTS_PER_SECOND;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_NARE;
		}
		break;
#endif

	case PSI_PATH_IF_SYS_CM_EDD:
		{
			pDetail->path_type = CM_PATH_TYPE_EDD;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

#if ( PSI_CFG_USE_MRP == 1 )
	case PSI_PATH_IF_SYS_CM_MRP:
		{
			pDetail->path_type = CM_PATH_TYPE_MRP;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.mrp.nothing_so_far = 0; /* well... */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_MRP;
		}
		break;
#endif

#if ( PSI_CFG_USE_GSY == 1 )
	case PSI_PATH_IF_SYS_CM_GSY_CLOCK:
		{
			pDetail->path_type = CM_PATH_TYPE_GSY;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.gsy.nothing_so_far = 0; /* well... */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_GSY;
		}
		break;
#endif

	case PSI_PATH_IF_SYS_CM_OHA:
		{
			pDetail->path_type = CM_PATH_TYPE_OHA;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			// Note: must match with OHA details
			pDetail->u.oha.hello_resource_count = pDetailsIF->oha.hello_resource_count_dcp;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_OHA;
		}
		break;

#if ( PSI_CFG_USE_POF == 1 )
	case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail->path_type = CM_PATH_TYPE_POF;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.pof.nothing_so_far = 0; /* well... */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = LSA_COMP_ID_POF;
		}
		break;
#endif

	default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;
		}
		break;
	}

	if ( result == LSA_RET_OK )
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr         = hd_nr;
		pSys->if_nr         = if_nr;
		pSys->pnio_if_nr    = pDetailsIF->pnio_if_nr;
		pSys->comp_id       = LSA_COMP_ID_CM;
		pSys->comp_id_lower = comp_id_lower;
		pSys->comp_id_edd   = pDetailsIF->edd_comp_id;
		pSys->rsp_mbx_id    = rsp_mbx_id;            // MBX for responses
		pSys->hSysIF        = pDetailsIF->hSysIF;    // Sys handle for HW out functions
		pSys->hPoolNRT      = pDetailsIF->hPoolNRT;  // NRT pool handle for NRT mem
		pSys->check_arp     = LSA_FALSE;
		pSys->trace_idx     = pDetailsIF->trace_idx;

		*detail_ptr_ptr = pDetail;
		*sys_ptr_ptr    = pSys;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_cm_release_path_info(
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_VOID_PTR_TYPE     detail_ptr )
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT( sys_ptr );
	PSI_ASSERT( detail_ptr );

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_CM );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return ( LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_cm_open_channel( LSA_SYS_PATH_TYPE sys_path )
{
	CM_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE  user_id   = {0};
	PSI_SYS_TYPE      dummy_sys = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	CM_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
	PSI_ASSERT(pRQB);

	CM_ALLOC_UPPER_MEM(&pRQB->args.void_ptr, user_id, sizeof(CM_ANY_ARGS_TYPE), &dummy_sys);
	PSI_ASSERT(pRQB->args.void_ptr);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.open->handle                    = CM_INVALID_HANDLE;
	pRQB->args.channel.open->handle_upper              = (LSA_UINT8)path;
	pRQB->args.channel.open->sys_path                  = sys_path;
	pRQB->args.channel.open->cm_request_upper_done_ptr = (CM_UPPER_CALLBACK_FCT_PTR_TYPE)psi_cm_channel_done;

	PSI_RQB_SET_HANDLE( pRQB, CM_INVALID_HANDLE   );
	PSI_RQB_SET_OPCODE( pRQB, CM_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_CM );

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_cm_open_channel(): Open CM channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		pRQB->args.channel.open->sys_path,
		path );

#if ( PSI_CFG_USE_HIF_HD )
	/* CM is on HD side, so delegate open to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, pRQB );
#else
	/* HIF HD not configured, close can be done direct */
	psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_cm_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	CM_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE  user_id   = {0};
	PSI_SYS_TYPE      dummy_sys = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	CM_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
	PSI_ASSERT(pRQB);

	CM_ALLOC_UPPER_MEM(&pRQB->args.void_ptr, user_id, sizeof(CM_ANY_ARGS_TYPE), &dummy_sys);
	PSI_ASSERT(pRQB->args.void_ptr);

	PSI_RQB_SET_HANDLE( pRQB, handle);
	PSI_RQB_SET_OPCODE( pRQB, CM_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_CM );
	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_06( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_cm_close_channel(): Close CM channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x) handle(%u)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		sys_path,
		path,
		handle );

#if ( PSI_CFG_USE_HIF_HD )
	/* CM is on HD side, so delegate close to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, pRQB );
#else
	/* HIF HD not configured, close can be done direct */
	psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_cm_channel_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16        rc;
	LSA_UINT32        trace_idx;
	LSA_OPCODE_TYPE   opc;

	PSI_SYS_TYPE      dummy_sys = {0};
	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK);

	opc = CM_RQB_GET_OPCODE( pRQB );

	switch ( opc )  /* Handle response for CM Opcode */
	{
	case CM_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE sys_path = pRQB->args.channel.open->sys_path;
			LSA_HANDLE_TYPE   handle   = pRQB->args.channel.open->handle;

			trace_idx = psi_get_trace_idx_for_sys_path( pRQB->args.channel.open->sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_cm_channel_done(): Open CM channel done, rsp(%#x), sys_path(%#x) handle(%u)",
				ACP_RQB_GET_RESPONSE( pRQB ),
				pRQB->args.channel.open->sys_path,
				pRQB->args.channel.open->handle );

			CM_FREE_UPPER_MEM( &rc, pRQB->args.void_ptr, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK);

			CM_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Register handle for the channel
			psi_ld_open_channels_done( sys_path, handle );
		}
		break;
	case CM_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE sys_path = LSA_RQB_GET_USER_ID_UVAR16( pRQB );

			trace_idx = psi_get_trace_idx_for_sys_path( sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_cm_channel_done(): Close CM channel done, rsp(%u/%#x), sys_path(%#x)",
				CM_RQB_GET_RESPONSE( pRQB ),
				CM_RQB_GET_RESPONSE( pRQB ),
				sys_path );

			CM_FREE_UPPER_MEM( &rc, pRQB->args.void_ptr, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK);

			CM_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Unregister handle for the channel
			psi_ld_close_channels_done( sys_path );
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_cm_channel_done(): unexpected opcode (%u/%#x)",
				opc, opc );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
#else
# ifdef PSI_MESSAGE
#  pragma PSI_MESSAGE("***** not compiling PSI CM support (PSI_CFG_USE_CM=" PSI_STRINGIFY(PSI_CFG_USE_CM) ")")
# endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
