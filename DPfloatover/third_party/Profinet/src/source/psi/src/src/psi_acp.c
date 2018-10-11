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
/*  F i l e               &F: psi_acp.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail things for ACP                                            */
/*                                                                           */
/*****************************************************************************/
#define PSI_MODULE_ID      301
#define LTRC_ACT_MODUL_ID  301

#include "psi_int.h"

#if ( PSI_CFG_USE_ACP == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_pnstack.h"
#include "psi_ld.h"

#if ( PSI_CFG_USE_HIF_HD ) // Using HIF HD
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "acp_inc.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== ACP ===================================================================*/
/*===========================================================================*/

LSA_RESULT psi_acp_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path )
{
	ACP_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE    pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16 result        = LSA_RET_OK;

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_acp_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	switch (path)
	{
	case PSI_PATH_IF_APP_IOC_CMCL_ACP:  /* IO Controller */
		{
			PSI_RT_DETAIL_PTR_TYPE pACP = &pDetailsIF->acp.cm_cl;

			pDetail->nic_id       = pDetailsIF->pnio_if_nr;
			pDetail->channel_type = ACP_CHANNEL_TYPE_USER;

			pDetail->type.user.frame_id_base_class1_unicast     = pACP->frame_id_base_class1_unicast;
			pDetail->type.user.nr_of_consumers_class1_unicast   = pACP->nr_of_consumers_class1_unicast;
			pDetail->type.user.frame_id_base_class1_multicast   = pACP->frame_id_base_class1_multicast;
			pDetail->type.user.nr_of_consumers_class1_multicast = pACP->nr_of_consumers_class1_multicast;

			pDetail->type.user.frame_id_base_class2_unicast     = pACP->frame_id_base_class2_unicast;
			pDetail->type.user.nr_of_consumers_class2_unicast   = pACP->nr_of_consumers_class2_unicast;
			pDetail->type.user.frame_id_base_class2_multicast   = pACP->frame_id_base_class2_multicast;
			pDetail->type.user.nr_of_consumers_class2_multicast = pACP->nr_of_consumers_class2_multicast;

			pDetail->type.user.nr_of_providers_class1_2 = pACP->nr_of_providers_class1_2;

			pDetail->type.user.nr_of_consumers_class3 = pACP->nr_of_consumers_class3;
			pDetail->type.user.nr_of_providers_class3 = pACP->nr_of_providers_class3;

			pDetail->type.user.alarm_ref_base  = pACP->alarm_ref_base;
			pDetail->type.user.alarm_nr_of_ref = pACP->alarm_nr_of_ref;

			pDetail->type.user.group_id = pACP->group_id;

			rsp_mbx_id    = PSI_MBX_ID_CM;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_APP_IOD_CMSV_ACP: /* IO Device */
		{
			PSI_RT_DETAIL_PTR_TYPE pACP = &pDetailsIF->acp.cm_sv;

			pDetail->nic_id       = pDetailsIF->pnio_if_nr;
			pDetail->channel_type = ACP_CHANNEL_TYPE_USER;

			pDetail->type.user.frame_id_base_class1_unicast     = pACP->frame_id_base_class1_unicast;
			pDetail->type.user.nr_of_consumers_class1_unicast   = pACP->nr_of_consumers_class1_unicast;
			pDetail->type.user.frame_id_base_class1_multicast   = pACP->frame_id_base_class1_multicast;
			pDetail->type.user.nr_of_consumers_class1_multicast = pACP->nr_of_consumers_class1_multicast;

			pDetail->type.user.frame_id_base_class2_unicast     = pACP->frame_id_base_class2_unicast;
			pDetail->type.user.nr_of_consumers_class2_unicast   = pACP->nr_of_consumers_class2_unicast;
			pDetail->type.user.frame_id_base_class2_multicast   = pACP->frame_id_base_class2_multicast;
			pDetail->type.user.nr_of_consumers_class2_multicast = pACP->nr_of_consumers_class2_multicast;

			pDetail->type.user.nr_of_providers_class1_2 = pACP->nr_of_providers_class1_2;

			pDetail->type.user.nr_of_consumers_class3 = pACP->nr_of_consumers_class3;
			pDetail->type.user.nr_of_providers_class3 = pACP->nr_of_providers_class3;

			pDetail->type.user.alarm_ref_base  = pACP->alarm_ref_base;
			pDetail->type.user.alarm_nr_of_ref = pACP->alarm_nr_of_ref;

			pDetail->type.user.group_id = pACP->group_id;

			rsp_mbx_id    = PSI_MBX_ID_CM;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_APP_IOM_CMMC_ACP:  /* IO Multicast */
		{
			PSI_RT_DETAIL_PTR_TYPE pACP = &pDetailsIF->acp.cm_mc;

			pDetail->channel_type = ACP_CHANNEL_TYPE_USER;
			pDetail->nic_id       = pDetailsIF->pnio_if_nr;

			pDetail->type.user.frame_id_base_class1_unicast     = pACP->frame_id_base_class1_unicast;
			pDetail->type.user.nr_of_consumers_class1_unicast   = pACP->nr_of_consumers_class1_unicast;
			pDetail->type.user.frame_id_base_class1_multicast   = pACP->frame_id_base_class1_multicast;
			pDetail->type.user.nr_of_consumers_class1_multicast = pACP->nr_of_consumers_class1_multicast;

			pDetail->type.user.frame_id_base_class2_unicast     = pACP->frame_id_base_class2_unicast;
			pDetail->type.user.nr_of_consumers_class2_unicast   = pACP->nr_of_consumers_class2_unicast;
			pDetail->type.user.frame_id_base_class2_multicast   = pACP->frame_id_base_class2_multicast;
			pDetail->type.user.nr_of_consumers_class2_multicast = pACP->nr_of_consumers_class2_multicast;

			pDetail->type.user.nr_of_providers_class1_2 = pACP->nr_of_providers_class1_2;

			pDetail->type.user.nr_of_consumers_class3 = pACP->nr_of_consumers_class3;
			pDetail->type.user.nr_of_providers_class3 = pACP->nr_of_providers_class3;

			pDetail->type.user.alarm_ref_base  = pACP->alarm_ref_base;
			pDetail->type.user.alarm_nr_of_ref = pACP->alarm_nr_of_ref;

			pDetail->type.user.group_id = pACP->group_id;

			rsp_mbx_id    = PSI_MBX_ID_CM;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_SYS_ACP_EDD: /* ACP */
		{
			pDetail->channel_type = ACP_CHANNEL_TYPE_SYSTEM;
			pDetail->nic_id       = pDetailsIF->pnio_if_nr;

			pDetail->type.system.max_edd_indication_res = 16; /* (LaM) Based on MaxControllerDevices, EDD_CFG_CSRT_MAX_INDICATIONS and the count of PROFINET phases (reduction ratios)  */

			pDetail->type.system.max_edd_nrt_recv_res = (pDetailsIF->cm.cl.device_count + (pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar) ) * 2; /* Count devices * 2 */
			pDetail->type.system.max_edd_nrt_send_res = (pDetailsIF->cm.cl.device_count + (pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar) ) * 2;

#if ( PSI_CFG_USE_HIF_HD )  // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_PSI;
#endif
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM);
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
		pSys->comp_id       = LSA_COMP_ID_ACP;
		pSys->comp_id_lower = comp_id_lower;
		pSys->comp_id_edd   = pDetailsIF->edd_comp_id;
		pSys->rsp_mbx_id    = rsp_mbx_id;            // MBX for responses
		pSys->hSysIF        = pDetailsIF->hSysIF;    // Sys handle for HW out functions
		pSys->hPoolNRT      = pDetailsIF->hPoolNRT;  // NRT pool handle for NRT mem
		pSys->check_arp     = LSA_FALSE;
		pSys->trace_idx     = pDetailsIF->trace_idx;

		*sys_ptr_ptr    = pSys;
		*detail_ptr_ptr = pDetail;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_acp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr )
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT( sys_ptr );
	PSI_ASSERT( detail_ptr );

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_ACP );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return ( LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_acp_open_channel( LSA_SYS_PATH_TYPE sys_path )
{
	ACP_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE user_id    = {0};
	PSI_SYS_TYPE      dummy_sys = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	ACP_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), 0 );
	PSI_ASSERT(pRQB);

	ACP_ALLOC_UPPER_MEM((LSA_VOID_PTR_TYPE*)&pRQB->args, user_id, sizeof(ACP_ARGS_TYPE), &dummy_sys);
	PSI_ASSERT(pRQB->args);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args->channel.open.handle                     = ACP_INVALID_HANDLE;
	pRQB->args->channel.open.handle_upper               = (LSA_UINT8)path;
	pRQB->args->channel.open.sys_path                   = sys_path;
	pRQB->args->channel.open.acp_request_upper_done_ptr = (ACP_UPPER_CALLBACK_FCT_PTR_TYPE)psi_acp_channel_done;

	PSI_RQB_SET_HANDLE( pRQB, ACP_INVALID_HANDLE   );
	PSI_RQB_SET_OPCODE( pRQB, ACP_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_ACP );

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_acp_open_channel(): Open ACP channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		pRQB->args->channel.open.sys_path,
		path );

#if ( PSI_CFG_USE_HIF_HD )
	/* ACP is on HD side, so delegate open to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, pRQB );
#else
	/* HIF HD not configured, open can be done direct */
	psi_request_start( PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_open_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_acp_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	ACP_UPPER_RQB_PTR_TYPE pRQB;

	LSA_USER_ID_TYPE  user_id   = {0};
	PSI_SYS_TYPE      dummy_sys = {0};

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	ACP_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
	PSI_ASSERT(pRQB);

	ACP_ALLOC_UPPER_MEM((ACP_UPPER_MEM_PTR_TYPE *)&pRQB->args, user_id, sizeof(ACP_ARGS_TYPE), &dummy_sys);
	PSI_ASSERT(pRQB->args);

	PSI_RQB_SET_HANDLE( pRQB, handle);
	PSI_RQB_SET_OPCODE( pRQB, ACP_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_ACP );

	// User ID is used for unregister handle over SYS-PATH information
	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // User ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_06( trace_idx, LSA_TRACE_LEVEL_NOTE,
		"psi_acp_close_channel(): Close ACP channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x) handle(%u)",
		pnio_if_nr,
		hd_nr,
		if_nr,
		sys_path,
		path,
		handle );

#if ( PSI_CFG_USE_HIF_HD )
	/* ACP is on HD side, so delegate close to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, pRQB );
#else
	/* HIF HD not configured, close can be done direct */
	psi_request_start( PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_close_channel, pRQB );
#endif
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_acp_channel_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16        rc;
	LSA_UINT32        trace_idx;
	LSA_OPCODE_TYPE   opc;

	PSI_SYS_TYPE      dummy_sys = {0};
	ACP_UPPER_RQB_PTR_TYPE pRQB = (ACP_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( ACP_RQB_GET_RESPONSE( pRQB ) == ACP_OK);

	opc = ACP_RQB_GET_OPCODE( pRQB );

	switch ( opc )  /* Handle response for ACP Opcode */
	{
	case ACP_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE sys_path = pRQB->args->channel.open.sys_path;
			LSA_HANDLE_TYPE   handle   = pRQB->args->channel.open.handle;

			trace_idx = psi_get_trace_idx_for_sys_path( pRQB->args->channel.open.sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_acp_channel_done(): Open ACP channel done, rsp(%#x), sys_path(%#x) handle(%u)",
				ACP_RQB_GET_RESPONSE( pRQB ),
				pRQB->args->channel.open.sys_path,
				pRQB->args->channel.open.handle );

			ACP_FREE_UPPER_MEM( &rc, pRQB->args, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK);

			ACP_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Register handle for the channel
			psi_ld_open_channels_done( sys_path, handle );
		}
		break;
	case ACP_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE sys_path = LSA_RQB_GET_USER_ID_UVAR16( pRQB );

			trace_idx = psi_get_trace_idx_for_sys_path( sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE,
				"psi_acp_channel_done(): Close ACP channel done, rsp(%u/%#x), sys_path(%#x)",
				ACP_RQB_GET_RESPONSE( pRQB ),
				ACP_RQB_GET_RESPONSE( pRQB ),
				sys_path );

			ACP_FREE_UPPER_MEM( &rc, pRQB->args, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK);

			ACP_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Unregister handle for the channel
			psi_ld_close_channels_done( sys_path );
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_acp_channel_done(): unexpected opcode (%u/%#x)",
				opc, opc );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
#else
# ifdef PSI_MESSAGE
#  pragma PSI_MESSAGE("***** not compiling PSI ACP support (PSI_CFG_USE_ACP=" PSI_STRINGIFY(PSI_CFG_USE_ACP) ")")
# endif
#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
