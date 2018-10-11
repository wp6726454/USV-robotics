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
/*  F i l e               &F: psi_oha.c                                 :F&  */
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
/*  channel-detail things for OHA                                            */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      312
#define LTRC_ACT_MODUL_ID  312

#include "psi_int.h"

#if ( PSI_CFG_USE_OHA == 1 )
/*----------------------------------------------------------------------------*/

#include "psi_pnstack.h"
#include "psi_ld.h"

#include "oha_inc.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== OHA  ==================================================================*/
/*===========================================================================*/

LSA_RESULT psi_oha_get_path_info(
	LSA_SYS_PTR_TYPE            *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE           *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE           sys_path )
{
	OHA_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE    pSys;

	LSA_UINT16 rsp_mbx_id    = PSI_MBX_ID_MAX;
	LSA_UINT16 comp_id_lower = LSA_COMP_ID_UNUSED;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16 result        = LSA_RET_OK;

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_oha_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	switch (path)
	{
	case PSI_PATH_GLO_APP_OHA: // OHA User global
		{
			pDetail->PathType          = OHA_PATH_TYPE_USER;
			pDetail->Params.User.NICId = pDetailsIF->pnio_if_nr;

			/* 0: every dcp indication is sent to the user  */
			/* <> 0: bitcoded, see OHA_CTRL_IP_SUITE, OHA_CTRL_STATION_NAME,... */
			pDetail->Params.User.DcpIndicationFilter = 0;   // Turned off
			pDetail->Params.User.CheckInterval       = 0;   // Turned off
			pDetail->Params.User.CheckIpEnabled      = LSA_FALSE;   // Turned off
			pDetail->Params.User.RemaStationFormat   = OHA_REMA_STATION_FORMAT_ADVANCED;

			/* true: 10Mbps and half-duplex (for all speeds) are allowed */
			pDetail->Params.User.allow_non_pnio_mautypes = pDetailsIF->oha.allow_non_pnio_mautypes;

			/* false: Prm-Indications not enabled */
			pDetail->Params.User.CheckTopology = LSA_FALSE;
			
            #if ( PSI_CFG_USE_HIF == 1 ) // Using HIF LD
			rsp_mbx_id = PSI_MBX_ID_HIF_LD;
#else
			rsp_mbx_id = PSI_MBX_ID_USER; // OHA user
#endif
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_APP_OHA:  // OHA User IF specific
		{
			pDetail->PathType          = OHA_PATH_TYPE_USER;
			pDetail->Params.User.NICId = pDetailsIF->pnio_if_nr;

			/* 0: every dcp indication is sent to the user  */
			/* <> 0: bitcoded, see OHA_CTRL_IP_SUITE, OHA_CTRL_STATION_NAME,... */
			pDetail->Params.User.DcpIndicationFilter = 0;
			/* check IP changes by using OHA OHA_OPC_SET_ADDRESS service */
			pDetail->Params.User.CheckInterval       = pDetailsIF->oha.check_interval;
			pDetail->Params.User.CheckIpEnabled      = pDetailsIF->oha.check_ip_enabled;
			/* using new advanced REMA format */
			pDetail->Params.User.RemaStationFormat   = OHA_REMA_STATION_FORMAT_ADVANCED;

			/* true: 10Mbps and half-duplex (for all speeds) are allowed */
			pDetail->Params.User.allow_non_pnio_mautypes = pDetailsIF->oha.allow_non_pnio_mautypes;

			/* false: Prm-Indications not enabled */
			pDetail->Params.User.CheckTopology = LSA_FALSE;
			
            #if ( PSI_CFG_USE_HIF == 1 ) // Using HIF LD
			rsp_mbx_id = PSI_MBX_ID_HIF_LD;
#else
			rsp_mbx_id = PSI_MBX_ID_USER; // OHA IF user
#endif
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_EDD:
		{
			pDetail->PathType         = OHA_PATH_TYPE_EDD;
			pDetail->Params.Edd.NICId = pDetailsIF->pnio_if_nr;
			pDetail->Params.Edd.NoS_allow_upper_cases = pDetailsIF->oha.nos_allow_upper_cases;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_DCP_EDD:
		{
			pDetail->PathType                = OHA_PATH_TYPE_DCP;
			pDetail->Params.Dcp.NICId        = pDetailsIF->pnio_if_nr;
			pDetail->Params.Dcp.NumOfRecv    = pDetailsIF->oha.recv_resource_count_dcp;  /* receive-resources for DCP-Server */
			pDetail->Params.Dcp.NumOfRecvFSU = pDetailsIF->oha.hello_resource_count_dcp; /* receive-resources for FSU-Indication */

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_DCP;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{
			pDetail->PathType                         = OHA_PATH_TYPE_LLDP;
			pDetail->Params.Lldp.NICId                = pDetailsIF->pnio_if_nr;
			pDetail->Params.Lldp.NumOfRecv            = (pDetailsIF->nr_of_ports * 2); /* receive-resources for LLDP */

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_LLDP;
		}
		break;

	case PSI_PATH_GLO_SYS_OHA_SOCK:
		{
			pDetail->PathType               = OHA_PATH_TYPE_SOCK;
			pDetail->Params.Sock.NICId      = pDetailsIF->pnio_if_nr;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_SOCK;
		}
		break;

	case PSI_PATH_GLO_SYS_OHA_SNMPX:
		{
			pDetail->PathType               = OHA_PATH_TYPE_SNMPX;
			pDetail->Params.Nare.NICId      = pDetailsIF->pnio_if_nr;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_SNMPX;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_NARE:
		{
			pDetail->PathType          = OHA_PATH_TYPE_NARE;
			pDetail->Params.Nare.NICId = pDetailsIF->pnio_if_nr;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_NARE;
		}
		break;

	case PSI_PATH_IF_SYS_OHA_MRP:
		{
			pDetail->PathType         = OHA_PATH_TYPE_MRP;
			pDetail->Params.Mrp.NICId = pDetailsIF->pnio_if_nr;

			rsp_mbx_id    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_MRP;
		}
		break;

	case PSI_PATH_IF_SYS_CM_OHA:
		{
			pDetail->PathType          = OHA_PATH_TYPE_USER;
			pDetail->Params.User.NICId = pDetailsIF->pnio_if_nr;

			/* 0: every dcp indication is sent to the user  */
			/* <> 0: bitcoded, see OHA_CTRL_IP_SUITE, OHA_CTRL_STATION_NAME,... */
			pDetail->Params.User.DcpIndicationFilter = 0;

			/* check IP changes by writing IP records over CM PD */
			pDetail->Params.User.CheckInterval       = pDetailsIF->oha.check_interval;
			pDetail->Params.User.CheckIpEnabled      = pDetailsIF->oha.check_ip_enabled;

			pDetail->Params.User.RemaStationFormat   = 0;  // no REMA user channel

			/* true: 10Mbps and half-duplex (for all speeds) are allowed */
			pDetail->Params.User.allow_non_pnio_mautypes = pDetailsIF->oha.allow_non_pnio_mautypes;

			/* true: Prm-Indications enabled */
			pDetail->Params.User.CheckTopology = LSA_TRUE;

            #if ( PSI_CFG_USE_HIF_HD == 1 ) // Using HIF HD
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
#else
			rsp_mbx_id = PSI_MBX_ID_CM;
#endif
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM);
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
		pSys->comp_id       = LSA_COMP_ID_OHA;
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

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_oha_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr )
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT( sys_ptr );
	PSI_ASSERT( detail_ptr );

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_OHA );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_oha_open_channel( LSA_SYS_PATH_TYPE sys_path )
{
	OHA_UPPER_RQB_PTR_TYPE pRQB;

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	OHA_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.handle                     = OHA_INVALID_HANDLE;
	pRQB->args.channel.handle_upper               = (LSA_UINT8)path;
	pRQB->args.channel.sys_path                   = sys_path;
	pRQB->args.channel.oha_request_upper_done_ptr = (OHA_UPPER_CALLBACK_FCT_PTR_TYPE)psi_oha_channel_done;

	PSI_RQB_SET_HANDLE( pRQB, OHA_INVALID_HANDLE );
	PSI_RQB_SET_OPCODE( pRQB, OHA_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_OHA );

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, 
		"psi_oha_open_channel(): Open OHA channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x)", 
		pnio_if_nr, 
		hd_nr, 
		if_nr, 
		pRQB->args.channel.sys_path, 
		path );

	/* OHA is on LD side --> open can be done direct */
	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_oha_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	OHA_UPPER_RQB_PTR_TYPE pRQB;

	LSA_UINT16 path  = PSI_SYSPATH_GET_PATH( sys_path );
	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	LSA_UINT16 pnio_if_nr = psi_get_pnio_if_nr( hd_nr, if_nr );

	LSA_UINT32 trace_idx;

	PSI_ASSERT ( path != PSI_SYS_PATH_INVALID );

	OHA_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	PSI_RQB_SET_HANDLE( pRQB, handle);
	PSI_RQB_SET_OPCODE( pRQB, OHA_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_OHA );

	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_06( trace_idx, LSA_TRACE_LEVEL_NOTE, 
		"psi_oha_close_channel(): Close OHA channel PNIO-IF(%u) hd(%u) if(%u) sys_path(%#x) path(%#x) handle(%u)", 
		pnio_if_nr, 
		hd_nr, 
		if_nr, 
		sys_path, 
		path, 
		handle );

	/* OHA is on LD side --> close can be done direct */
	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, pRQB );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_oha_channel_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16        rc;
	LSA_UINT32        trace_idx;
	LSA_OPCODE_TYPE   opc;

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK);

	opc = OHA_RQB_GET_OPCODE( pRQB );

	switch ( opc )  /* Handle response for ACP Opcode */
	{
	case OHA_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE sys_path = pRQB->args.channel.sys_path;
			LSA_HANDLE_TYPE   handle   = pRQB->args.channel.handle;

			trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, 
				"psi_oha_channel_done(): Open OHA channel done, rsp(%#x), sys_path(%#x) handle(%u)",
				OHA_RQB_GET_RESPONSE( pRQB ), 
				pRQB->args.channel.sys_path, 
				pRQB->args.channel.handle );

			OHA_FREE_UPPER_RQB_LOCAL(&rc, pRQB );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Register handle for the channel
			psi_ld_open_channels_done( sys_path, handle ); 
		}
		break;

	case OHA_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE sys_path = LSA_RQB_GET_USER_ID_UVAR16( pRQB );

			trace_idx = psi_get_trace_idx_for_sys_path( sys_path );

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, 
				"psi_oha_channel_done(): Close OHA channel done, rsp(%u/%#x), sys_path(%#x)",
				OHA_RQB_GET_RESPONSE( pRQB ), 
				OHA_RQB_GET_RESPONSE( pRQB ), 
				sys_path );

			OHA_FREE_UPPER_RQB_LOCAL( &rc, pRQB );
			PSI_ASSERT( rc == LSA_RET_OK );

			// Unregister handle for the channel
			psi_ld_close_channels_done( sys_path ); 
		}
		break;

	default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_oha_channel_done(): unexpected opcode (%u/%#x)",
				opc, opc );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID psi_oha_init_db_if( LSA_UINT16 pnio_if_nr, PSI_HD_IF_OHA_INPUT_PTR_TYPE oha_input_ptr, LSA_UINT8*  chassis_name_ptr )
{
	LSA_UINT16 rc;
	LSA_UINT16 port_id;
    LSA_UINT8 temp[256];
    LSA_UINT8 extension[40];

	PSI_ASSERT( oha_input_ptr != LSA_NULL );
	PSI_ASSERT(( pnio_if_nr > 0) && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR) );

	// prepare the OHA DB for this interface based on INI Settings
	// note: NIC-ID has same value as EDD IF-ID,
	//       the IP and NoS and MIB2 specific data must be set by API after startup

    PSI_SPRINTF(extension, "%sX%d", ", internal, ", pnio_if_nr);
    if( (PSI_STRLEN(oha_input_ptr->if_descr) + PSI_STRLEN(extension)) < 256 )
    {
        PSI_STRCPY(temp, oha_input_ptr->if_descr);
        strcat((char*)temp, (const char*)extension);
    }
    else
    {
        *temp = 0;
    }

	rc = oha_init_interface( 
		pnio_if_nr,
		oha_input_ptr->vendor_id,
		oha_input_ptr->device_id,
		(LSA_UINT16)oha_input_ptr->instance_opt,
		(oha_input_ptr->instance_opt != 0xFFFFFFFF) ? LSA_TRUE : LSA_FALSE, 
		0, 
		LSA_NULL, 
		0,
		LSA_FALSE, 
		chassis_name_ptr, 
		(LSA_UINT16)PSI_STRLEN((const char *) chassis_name_ptr ),
		PSI_CFG_OHA_LLDP_SYSTEM_CAPABILITIES,   /* SysCapSupported */
		PSI_CFG_OHA_LLDP_ENABLED_CAPABILITIES,  /* SysCapEnabled   */
		oha_input_ptr->sys_descr,   /* SysDescr */
		(LSA_UINT16)PSI_STRLEN(oha_input_ptr->sys_descr),
		temp,   /* IfDescr  */
		(LSA_UINT16)PSI_STRLEN(temp)
		);

	PSI_ASSERT(rc == OHA_OK);

	for (port_id = 1; port_id <= OHA_CFG_MAX_PORTS; ++port_id)
	{
		LSA_UINT8  port_name[33];

		PSI_SPRINTF (port_name, "port-%03d", port_id);
        PSI_SPRINTF(extension, "%sX%d P%d", ", Ethernet Port, ", pnio_if_nr, port_id);
        if( (PSI_STRLEN(oha_input_ptr->if_descr) + PSI_STRLEN(extension)) < 256 )
        {
            PSI_STRCPY(temp, oha_input_ptr->if_descr);
            strcat((char*)temp, (const char*)extension);
        }
        else
        {
            *temp = 0;
        }

		rc = oha_init_port(pnio_if_nr, port_id,
				port_name,
				(LSA_UINT16)PSI_STRLEN((const char *)port_name),
				temp,
				(LSA_UINT16)PSI_STRLEN(temp)
		);
		if (rc != OHA_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"psi_oha_init_db_if: oha_init_port failed (Status: 0x%X).", rc);
		}
	PSI_ASSERT(rc == OHA_OK);
}
}

/*----------------------------------------------------------------------------*/

LSA_VOID psi_oha_undo_init_db_if( LSA_UINT16  pnio_if_nr )
{
	LSA_UINT16 ret;

	ret = oha_undo_init_interface(pnio_if_nr);
	PSI_ASSERT(ret == LSA_RET_OK);
}

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI OHA support (PSI_CFG_USE_OHA=" PSI_STRINGIFY(PSI_CFG_USE_OHA) ")")
#endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
