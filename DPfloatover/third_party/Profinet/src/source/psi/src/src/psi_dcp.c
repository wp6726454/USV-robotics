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
/*  F i l e               &F: psi_dcp.c                                 :F&  */
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
/*  channel-detail things for DCP                                            */
/*                                                                           */
/*****************************************************************************/
#define PSI_MODULE_ID      304
#define LTRC_ACT_MODUL_ID  304

#include "psi_int.h"

#if ( PSI_CFG_USE_DCP == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_pnstack.h"
#include "psi_ld.h"

#if ( PSI_CFG_USE_HIF_HD ) // Using HIF HD
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "dcp_inc.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== DCP ===================================================================*/
/*===========================================================================*/

static void psi_init_details_DCP( DCP_DETAIL_PTR_TYPE pDetail )
{
	/* DCP-Strategie: Werden Kanäle mit derselben LowerID gefunden, dann muss */
	/* die Allokierungsstrategie (AllocOnRequest) fuer lower RecvRQBs bei     */
	/* a l l e n upper Channels auf einem lower Channel gleich sein! */

	pDetail->AllocOnRequest = LSA_TRUE;							/* RQB allocation strategy: see above */
																/* AP01509782: new details: */
	pDetail->ServerHelloResources = PSI_DCP_HELLO_RESOURCES;	/* FSU channel: Number of receive RQBs in lower  	*/
																/* layer for concurrently incoming HELLO requests	*/
	pDetail->ClientResources = PSI_DCP_CLIENT_RESOURCES;		/* Client channel: Number of RQBs and timers for	*/
																/* concurrently outgoing client requests			*/
	pDetail->HelloEnable = LSA_TRUE;							/* Client channel: Use 10ms HELLO timer				*/
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_dcp_get_path_info(
	LSA_SYS_PTR_TYPE    *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE   *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   sys_path )
{
	DCP_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE    pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16 result        = LSA_RET_OK;

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_dcp_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	switch (path)
	{
	case PSI_PATH_IF_SYS_DCP_EDD:
		{
			psi_init_details_DCP( pDetail );

			pDetail->LowerId  = pDetailsIF->pnio_if_nr;
			pDetail->DevId    = DCP_DEV_FSU;
			pDetail->ClientCh = LSA_FALSE; /* FALSE: This is a server channel or DCP_DEV_FSU */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	case PSI_PATH_IF_SYS_NARE_DCP_EDD:
		{
			psi_init_details_DCP( pDetail );

			pDetail->LowerId  = pDetailsIF->pnio_if_nr;
			pDetail->DevId    = DCP_DEV_ETH;
			pDetail->ClientCh = LSA_TRUE; /* TRUE: Detail extension for client channel exists	*/

			rsp_mbx_id    = PSI_MBX_ID_NARE;
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_DCP_EDD: /* server */
		{
			psi_init_details_DCP( pDetail );

			pDetail->LowerId  = pDetailsIF->pnio_if_nr;
			pDetail->DevId    = DCP_DEV_ETH;
			pDetail->ClientCh = LSA_FALSE; /* FALSE: This is a server channel */

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_OHA;
#endif
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM);
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
		pSys->comp_id       = LSA_COMP_ID_DCP;
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
LSA_RESULT psi_dcp_release_path_info(
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_VOID_PTR_TYPE     detail_ptr )
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT( sys_ptr );
	PSI_ASSERT( detail_ptr );

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return ( LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_dcp_open_channel( LSA_SYS_PATH_TYPE sys_path )
{
	DCP_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE  user_id    = {0};
	PSI_SYS_TYPE      dummy_sys  = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	DCP_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
	PSI_ASSERT(pRQB);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.handle                     = PSI_INVALID_HANDLE; // no DCP_INVALID_HANDLE
	pRQB->args.channel.handle_upper               = (LSA_UINT8)path;
	pRQB->args.channel.sys_path                   = sys_path;
	pRQB->args.channel.dcp_request_upper_done_ptr = (DCP_UPPER_CALLBACK_FCT_PTR_TYPE)psi_dcp_channel_done;

	PSI_RQB_SET_HANDLE( pRQB, PSI_INVALID_HANDLE );
	PSI_RQB_SET_OPCODE( pRQB, DCP_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_DCP );

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_dcp_open_channel(): Open DCP channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		pRQB->args.channel.sys_path,
		path );

#if ( PSI_CFG_USE_HIF_HD )
	/* DCP is on HD side, so delegate open to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, pRQB );
#else
	/* HIF HD not configured, open can be done direct */
	psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_dcp_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	DCP_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE  user_id   = {0};
	PSI_SYS_TYPE      dummy_sys = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	DCP_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
	PSI_ASSERT(pRQB);

	PSI_RQB_SET_HANDLE( pRQB, handle);
	PSI_RQB_SET_OPCODE( pRQB, DCP_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_DCP );
	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_06( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_dcp_close_channel(): Close DCP channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x) handle(%u)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		sys_path,
		path,
		handle );

#if ( PSI_CFG_USE_HIF_HD )
	/* DCP is on HD side, so delegate close to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, pRQB );
#else
	/* HIF HD not configured, close can be done direct */
	psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_dcp_channel_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16        rc;
	LSA_UINT32        trace_idx;
	LSA_OPCODE_TYPE   opc;
	PSI_SYS_TYPE      dummy_sys = {0};

	DCP_UPPER_RQB_PTR_TYPE pRQB = (DCP_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( DCP_UPPER_RQB_RESPONSE_GET( pRQB ) == DCP_RSP_OK);


	opc = DCP_UPPER_RQB_OPCODE_GET( pRQB );

	switch ( opc )  /* Handle response for DCP Opcode */
	{
	case DCP_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE sys_path = pRQB->args.channel.sys_path;
			LSA_HANDLE_TYPE   handle   = pRQB->args.channel.handle;

			trace_idx = psi_get_trace_idx_for_sys_path( pRQB->args.channel.sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_dcp_channel_done(): Open DCP channel done, rsp(%#x), sys_path(%#x) handle(%u)",
				DCP_UPPER_RQB_RESPONSE_GET( pRQB ),
				pRQB->args.channel.sys_path,
				pRQB->args.channel.handle );

			DCP_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Register handle for the channel
			psi_ld_open_channels_done( sys_path, handle );
		}
		break;
	case DCP_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE sys_path = LSA_RQB_GET_USER_ID_UVAR16( pRQB );

			trace_idx = psi_get_trace_idx_for_sys_path( sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_dcp_channel_done(): Close DCP channel done, rsp(%u/%#x), sys_path(%#x)",
				DCP_UPPER_RQB_RESPONSE_GET( pRQB ),
				DCP_UPPER_RQB_RESPONSE_GET( pRQB ),
				sys_path );

			DCP_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Unregister handle for the channel
			psi_ld_close_channels_done( sys_path );
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_dcp_channel_done(): unexpected opcode (%u/%#x)",
				opc, opc );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
#else
# ifdef PSI_MESSAGE
#  pragma PSI_MESSAGE("***** not compiling PSI DCP support (PSI_CFG_USE_DCP=" PSI_STRINGIFY(PSI_CFG_USE_DCP) ")")
# endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
