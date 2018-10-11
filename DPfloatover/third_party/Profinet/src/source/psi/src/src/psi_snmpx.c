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
/*  F i l e               &F: psi_snmpx.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*  channel-detail things for SNMPX                                          */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      314
#define LTRC_ACT_MODUL_ID  314

#include "psi_int.h"

#if ( PSI_CFG_USE_SNMPX == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_pnstack.h"
#include "psi_ld.h"

#include "snmpx_inc.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== SNMPX==================================================================*/
/*===========================================================================*/

LSA_RESULT psi_snmpx_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE           *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path )
{
	SNMPX_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE      pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16 result        = LSA_RET_OK;

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_snmpx_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	switch (path)
	{
	case PSI_PATH_GLO_APP_SNMPX:  // SNMP User
		{
			pDetail->PathType                       = SNMPX_PATH_TYPE_MANAGER;
			pDetail->Params.Manager.NumOfRetries    = 2;
			pDetail->Params.Manager.TimeoutInterval = 2;	/* timeout: 2 sec */

#if ( PSI_CFG_USE_HIF )  // Using HIF LD
			rsp_mbx_id = PSI_MBX_ID_HIF_LD;
#else
			rsp_mbx_id = PSI_MBX_ID_USER; // SNMPX user
#endif
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_GLO_SYS_OHA_SNMPX:
		{
			pDetail->PathType             = SNMPX_PATH_TYPE_AGENT;
			pDetail->Params.Agent.Nothing = 0;

			rsp_mbx_id    = PSI_MBX_ID_OHA;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_GLO_SYS_SNMPX_SOCK:
		{
			/* receive-resources for SOCK (SNMP-Port) for the master agent */
			pDetail->PathType              = SNMPX_PATH_TYPE_SOCK;
			pDetail->Params.Sock.NumOfRecv = 2;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_SOCK;
		}
		break;

	default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM);
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
		pSys->comp_id       = LSA_COMP_ID_SNMPX;
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
LSA_RESULT psi_snmpx_release_path_info(
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_VOID_PTR_TYPE   detail_ptr )
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT( sys_ptr );
	PSI_ASSERT( detail_ptr );

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_SNMPX, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return ( LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_snmpx_open_channel( LSA_SYS_PATH_TYPE sys_path )
{
	SNMPX_UPPER_RQB_PTR_TYPE pRQB;

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	SNMPX_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.Channel.handle                       = SNMPX_INVALID_HANDLE;
	pRQB->args.Channel.handle_upper                 = (LSA_UINT8)path;
	pRQB->args.Channel.sys_path                     = sys_path;
	pRQB->args.Channel.snmpx_request_upper_done_ptr = (SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE)psi_snmpx_channel_done;

	PSI_RQB_SET_HANDLE( pRQB, SNMPX_INVALID_HANDLE );
	PSI_RQB_SET_OPCODE( pRQB, SNMPX_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_SNMPX );

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, 
		"psi_snmpx_open_channel(): Open SNMPX channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x)", 
		pnio_if_nr, 
		hd_nr, 
		if_nr, 
		pRQB->args.Channel.sys_path, 
		path );

	/* SNMPX is on LD side --> open can be done direct */
	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_open_channel, pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_snmpx_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	SNMPX_UPPER_RQB_PTR_TYPE pRQB;

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	SNMPX_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	PSI_RQB_SET_HANDLE( pRQB, handle );
	PSI_RQB_SET_OPCODE( pRQB, SNMPX_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_SNMPX );

	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_06( trace_idx, LSA_TRACE_LEVEL_NOTE, 
		"psi_snmpx_close_channel(): Close SNMPX channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x) handle(%u)", 
		pnio_if_nr, 
		hd_nr, 
		if_nr, 
		sys_path, 
		path, 
		handle );

	/* SNMPX is on LD side --> close can be done direct */
	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_close_channel, pRQB );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_snmpx_channel_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16        rc;
	LSA_UINT32        trace_idx;
	LSA_OPCODE_TYPE   opc;

	SNMPX_UPPER_RQB_PTR_TYPE pRQB = (SNMPX_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( SNMPX_RQB_GET_RESPONSE( pRQB ) == SNMPX_RSP_OK);

	opc = SNMPX_RQB_GET_OPCODE( pRQB );

	switch ( opc )  /* Handle response for ACP Opcode */
	{
	case SNMPX_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE sys_path = pRQB->args.Channel.sys_path;
			LSA_HANDLE_TYPE   handle   = pRQB->args.Channel.handle;

			trace_idx = psi_get_trace_idx_for_sys_path( pRQB->args.Channel.sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, 
				"psi_snmpx_channel_done(): Open SNMPX channel done, rsp(%#x), sys_path(%#x) handle(%u)",
				SNMPX_RQB_GET_RESPONSE( pRQB ), 
				pRQB->args.Channel.sys_path, 
				pRQB->args.Channel.handle );

			SNMPX_FREE_UPPER_RQB_LOCAL(&rc, pRQB );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Register handle for the channel
			psi_ld_open_channels_done( sys_path, handle ); 
		}
		break;
	case SNMPX_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE sys_path = LSA_RQB_GET_USER_ID_UVAR16( pRQB );

			trace_idx = psi_get_trace_idx_for_sys_path( sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, 
				"psi_snmpx_channel_done(): Close SNMPX channel done, rsp(%u/%#x), sys_path(%#x)",
				SNMPX_RQB_GET_RESPONSE( pRQB ), 
				SNMPX_RQB_GET_RESPONSE( pRQB ), 
				sys_path );

			SNMPX_FREE_UPPER_RQB_LOCAL(&rc, pRQB );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Unregister handle for the channel
			psi_ld_close_channels_done( sys_path ); 
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_snmpx_channel_done(): unexpected opcode (%u/%#x)",
				opc, opc );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
#else
# ifdef PSI_MESSAGE
#  pragma PSI_MESSAGE("***** not compiling PSI SNMPX  support (PSI_CFG_USE_SNMPX=" PSI_STRINGIFY(PSI_CFG_USE_SNMPX) ")")
# endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
