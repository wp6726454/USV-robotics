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
/*  F i l e               &F: psi_edds.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Channel-detail-settings for EDDS.                                        */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      307
#define LTRC_ACT_MODUL_ID  307

#include "psi_int.h"

#if ( PSI_CFG_USE_EDDS == 1 )

#include "psi_pnstack.h"
#include "psi_hd.h"

#include "edds_inc.h"
#include "edds_iobuf_inc.h"
#include "edds_iobuf_int.h" // internal header

#ifdef EDDS_CFG_HW_INTEL // using INTEL LL
#include "intel_inc.h"
#endif

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== EDDS ==================================================================*/
/*===========================================================================*/

static EDDS_CDB_TYPE * psi_init_CDB_EDDS( 
	EDDS_CDB_TYPE      *cdb, 
	EDDS_CDB_NRT_TYPE  *pNRT, 
	LSA_VOID_PTR_TYPE   hDDB, 
	LSA_UINT32          filter )
{
	// *--------------------------------------
	// *  CDB Header
	// *--------------------------------------
	cdb->UsedComp.UseNRT  = LSA_FALSE;
	cdb->UsedComp.UseCSRT = LSA_FALSE;
	cdb->UsedComp.UsePRM  = LSA_FALSE;

	cdb->hDDB         = hDDB;
	cdb->InsertSrcMAC = LSA_FALSE; 
	cdb->pNRT         = LSA_NULL;
	cdb->pPRM         = LSA_NULL;

	// *--------------------------------------
	// *  NRT configuration
	// *--------------------------------------
	if( pNRT != LSA_NULL ) 
	{
		pNRT->FrameFilter         = filter;

		cdb->pNRT            = pNRT;
		cdb->UsedComp.UseNRT = LSA_TRUE;
	}

	return cdb; /* for convenience, see usage */
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_edds_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path )
{
	EDDS_DETAIL_PTR_TYPE         pDetail;
	PSI_SYS_PTR_TYPE             pSys;
	EDDS_UPPER_CDB_NRT_PTR_TYPE  pNRT;
	EDDS_UPPER_CDB_PRM_PTR_TYPE  pPRM;
	EDDS_HANDLE                  hDDB;

	LSA_UINT16 rsp_mbx_id = PSI_MBX_ID_MAX;
	LSA_BOOL   arp_state  = LSA_FALSE;

	LSA_UINT16 hd_nr  = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr  = PSI_SYSPATH_GET_IF( sys_path );
	LSA_UINT16 path   = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = psi_get_detail_store( hd_nr, if_nr );

	LSA_USER_ID_TYPE user_id = {0};
	LSA_UINT16       result  = LSA_RET_OK;

    PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_edds_get_path_info(sys_path=%#x): hd_nr(%u) if_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, if_nr, path, path );

    PSI_ASSERT( sys_ptr_ptr != LSA_NULL );
	PSI_ASSERT( pDetailsIF  != LSA_NULL );
	PSI_ASSERT( pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDS );

	hDDB = psi_hd_get_hDDB( hd_nr );
	PSI_ASSERT( hDDB );

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pDetail, user_id, sizeof(*pDetail), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pSys, user_id, sizeof(*pSys), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys);

	PSI_MEMSET( pDetail, 0, sizeof(*pDetail) );

	switch (path)
	{
	    case PSI_PATH_IF_SYS_MRP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_MRP );

			rsp_mbx_id = PSI_MBX_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_ARP );

            #if ( PSI_CFG_USE_HIF_HD == 1 )
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            #else
			rsp_mbx_id = PSI_MBX_ID_TCIP;
            #endif
			arp_state  = LSA_TRUE;      // mark this channel for ARP check at response
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ICMP:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_ICMP );

            #if ( PSI_CFG_USE_HIF_HD == 1 )
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            #else
			rsp_mbx_id = PSI_MBX_ID_TCIP;
            #endif
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_UDP:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			if ( pDetailsIF->tcip.multicast_support_on ) // MC support on ?
			{
				pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_IGMP );
			}
			else
			{
				pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP );
			}

            #if ( PSI_CFG_USE_HIF_HD == 1 )
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            #else
			rsp_mbx_id = PSI_MBX_ID_TCIP;
            #endif
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_TCP:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_TCP );

            #if ( PSI_CFG_USE_HIF_HD == 1 )
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            #else
			rsp_mbx_id = PSI_MBX_ID_TCIP;
            #endif
		}
		break;

	    case PSI_PATH_IF_SYS_DCP_EDD:
		{
			LSA_UINT32 filter = EDD_NRT_FRAME_DCP_HELLO /* | EDD_NRT_FRAME_UDP_DCP_HELLO */;

			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, filter );

			rsp_mbx_id = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{ // ** user channel to NARE (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, 0 );

			rsp_mbx_id = PSI_MBX_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{ // ** user channel to OHA (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, 0 );

            #if ( PSI_CFG_USE_HIF_HD == 1 )
			rsp_mbx_id = PSI_MBX_ID_HIF_HD;
            #else
			rsp_mbx_id = PSI_MBX_ID_OHA;
            #endif
		}
		break;

		// ** user channel to DCP, NRT data **
	    case PSI_PATH_IF_SYS_OHA_DCP_EDD:  /* whichever comes first! */
	    case PSI_PATH_IF_SYS_NARE_DCP_EDD: /* whichever comes first! */
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP );

			rsp_mbx_id = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			LSA_UINT32 filter = EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_PTCP_DELAY;

			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, filter );

			rsp_mbx_id = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{ 
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_ANNO );

			rsp_mbx_id = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{ // ** user channel to LLDP with LLDP receive filter (not supported yet)**

			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, EDD_NRT_FRAME_LLDP );

			rsp_mbx_id = PSI_MBX_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD: // ** user channel to ACP with ASRT and CSRT data)**
		{
			LSA_UINT32 filter = EDD_NRT_FRAME_ASRT | EDD_NRT_FRAME_UDP_ASRT;

			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, filter );

			pDetail->UsedComp.UseCSRT   = LSA_TRUE; 

			rsp_mbx_id = PSI_MBX_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{ // ** user channel to CM (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT);

			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pPRM, user_id, sizeof(*pPRM), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pPRM);

			pDetail = psi_init_CDB_EDDS( pDetail, pNRT, hDDB, 0 );

			pDetail->UsedComp.UsePRM               = LSA_TRUE;
			pDetail->pPRM                          = pPRM;

			/* Note: Must match with OHA allow_non_pnio_mautypes setting */
			pPRM->PortDataAdjustLesserCheckQuality = pDetailsIF->oha.allow_non_pnio_mautypes;

			rsp_mbx_id = PSI_MBX_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail = psi_init_CDB_EDDS( pDetail, LSA_NULL, hDDB, 0 );

			rsp_mbx_id = PSI_MBX_ID_POF;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE*)&pNRT, user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT( pNRT != LSA_NULL );

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LEN_TSYNC);
			
			pDetail->InsertSrcMAC = LSA_TRUE;

			rsp_mbx_id = PSI_MBX_ID_PSI; // PSI EDD user hub
		}
		break;	

	    default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
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
		pSys->comp_id       = LSA_COMP_ID_EDDS;
		pSys->comp_id_lower = LSA_COMP_ID_UNUSED;
		pSys->comp_id_edd   = pDetailsIF->edd_comp_id;
		pSys->rsp_mbx_id    = rsp_mbx_id;
		pSys->hSysIF        = pDetailsIF->hSysIF;    // Sys handle for HW out functions
		pSys->hPoolNRT      = pDetailsIF->hPoolNRT;  // NRT pool handle for NRT mem
		pSys->check_arp     = arp_state;
		pSys->trace_idx     = pDetailsIF->trace_idx;

		*detail_ptr_ptr = pDetail;
		*sys_ptr_ptr    = pSys;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_edds_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr )
{
	LSA_UINT16           rc;
	EDDS_DETAIL_PTR_TYPE pDetail;
	PSI_SYS_PTR_TYPE     pSys;

	PSI_ASSERT(detail_ptr);

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_EDDS );

	pDetail = (EDDS_DETAIL_PTR_TYPE)detail_ptr;

	if ( pDetail->pNRT != LSA_NULL )
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pNRT, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
		PSI_ASSERT( rc == LSA_RET_OK );
	}

	if ( pDetail->pPRM != LSA_NULL )
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pPRM, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
		PSI_ASSERT( rc == LSA_RET_OK );
	}

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM);
	PSI_ASSERT( rc == LSA_RET_OK );

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/* EDDS system calls for device open, setup, close                            */
/*----------------------------------------------------------------------------*/

static LSA_VOID psi_edds_setup_request_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	PSI_EDDS_SETUP_PTR_TYPE  p_setup;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_ASSERT( pRQB != 0 );

	// Read the session data for changing the CBF and context switch to PSI task
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_setup->session.req_done_func != LSA_NULL );

	psi_system_request_done( PSI_MBX_ID_PSI, p_setup->session.req_done_func, pRQB );
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_close_request_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	PSI_EDDS_SESSION_PTR_TYPE p_session;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_ASSERT( pRQB != 0 );

	// Read the session data for changing the CBF and context switch to PSI task
	p_session = (PSI_EDDS_SESSION_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_session != LSA_NULL );
	PSI_ASSERT( p_session->req_done_func != LSA_NULL );

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
		"psi_edds_close_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_session->hd_nr, pRQB );

	psi_system_request_done( PSI_MBX_ID_PSI, p_session->req_done_func, pRQB );
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_open_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16               retVal;
	PSI_EDDS_SETUP_PTR_TYPE  p_setup;

	EDD_UPPER_RQB_PTR_TYPE      pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	EDDS_UPPER_DDB_INI_PTR_TYPE pOpen;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK        == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM    == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_OPEN == EDD_RQB_GET_SERVICE( pRQB ));

	pOpen = (EDDS_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	PSI_ASSERT( pOpen != 0 );

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_setup != LSA_NULL );

	// Register hDDB for this board
	*p_setup->session.hDDB_ptr = pOpen->hDDB;

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// continue wiht initializing NRT subcomponent
	psi_edds_init_nrt( p_setup );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_open( PSI_EDDS_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	EDDS_UPPER_DDB_INI_PTR_TYPE pOpen;
	EDDS_UPPER_DPB_PTR_TYPE     pDPB;

	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_setup->session.hDDB_ptr != LSA_NULL );

	pDPB = &p_setup->iniDPB;
	PSI_ASSERT( pDPB != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_DDB_INI_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	pOpen = (EDDS_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_OPEN );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_edds_device_open_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	pOpen->pDPB  = pDPB;
	pOpen->Cbf   = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_setup_request_done;
	pOpen->hDDB  = LSA_NULL;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_init_nrt_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16              retVal;
	PSI_EDDS_SETUP_PTR_TYPE p_setup;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK                == EDD_RQB_GET_RESPONSE( pRQB ) );
	PSI_ASSERT( EDD_OPC_SYSTEM            == EDD_RQB_GET_OPCODE( pRQB ) );
	PSI_ASSERT( EDDS_SRV_DEV_COMP_NRT_INI == EDD_RQB_GET_SERVICE( pRQB ) );

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_setup != LSA_NULL );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// continue with initializing SRT subcomponent
	psi_edds_init_srt( p_setup );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_init_nrt( PSI_EDDS_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE          pRQB;
	EDDS_UPPER_CMP_NRT_INI_PTR_TYPE pNRTIni; // NRT initialisation parameter block

	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_setup->session.hDDB_ptr != LSA_NULL );

	pNRTIni = &p_setup->iniNRT; // Use Setupblock as PPARAM
	PSI_ASSERT( pNRTIni != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_COMP_NRT_INI );
	EDD_RQB_SET_PPARAM( pRQB, pNRTIni );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_edds_init_nrt_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	// Add CBF and actual hDDB 
	pNRTIni->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_setup_request_done;
	pNRTIni->hDDB = *p_setup->session.hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_init_srt_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                       retVal;
	LSA_UINT16                       cnsCnt;
	LSA_UINT16                       provCnt;
	PSI_EDDS_SETUP_PTR_TYPE          p_setup;
	PSI_HD_STORE_PTR_TYPE            p_hd;
	EDDS_UPPER_CMP_SRT_INI_PTR_TYPE  pSRTIni;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK                == EDD_RQB_GET_RESPONSE( pRQB ) );
	PSI_ASSERT( EDD_OPC_SYSTEM            == EDD_RQB_GET_OPCODE( pRQB ) );
	PSI_ASSERT( EDDS_SRV_DEV_COMP_SRT_INI == EDD_RQB_GET_SERVICE( pRQB ) );

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_setup != LSA_NULL );

	// Register the EDDS IO Buffer handle and save in HD struct (used for PI alloc/free)
	pSRTIni = (EDDS_UPPER_CMP_SRT_INI_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	PSI_ASSERT( pSRTIni != LSA_NULL );

	p_hd = psi_hd_get_hd_store( p_setup->session.hd_nr );
	PSI_ASSERT( p_hd != LSA_NULL );

    if // Gigabit-Support enabled?
       ( p_setup->session.b_gigabit_support )
    {
        p_hd->hd_hw.edd.edds.io_buffer_handle = LSA_NULL;
    }
    else // Gigabit-Support not enabled
    {
	    p_hd->hd_hw.edd.edds.io_buffer_handle = EDDS_IOBuffer_Init( pSRTIni->CSRT.hIOBufferManagement, 1, &cnsCnt, &provCnt );
	    PSI_ASSERT( p_hd->hd_hw.edd.edds.io_buffer_handle != LSA_NULL );
	    PSI_ASSERT( cnsCnt  > 1 );
	    PSI_ASSERT( provCnt > 1 );
    }

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// Enable the PNIO IRs
	PSI_HD_ENABLE_IR( p_setup->session.hd_sys_handle );

	// continue with device setup
	psi_edds_device_setup( p_setup );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_init_srt( PSI_EDDS_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE          pRQB;
	EDDS_UPPER_CMP_SRT_INI_PTR_TYPE pSRTIni;

	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_setup->session.hDDB_ptr != LSA_NULL );

	pSRTIni = &p_setup->iniSRT;  // Use setupblock as pParam
	PSI_ASSERT( pSRTIni != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_COMP_SRT_INI );
	EDD_RQB_SET_PPARAM( pRQB, pSRTIni );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_edds_init_srt_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	// Add CBF and actual hDDB 
	pSRTIni->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_setup_request_done;
	pSRTIni->hDDB = *p_setup->session.hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_setup_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16               retVal;
	PSI_EDDS_SETUP_PTR_TYPE  p_setup;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK         == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_SETUP == EDD_RQB_GET_SERVICE( pRQB ));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_setup != LSA_NULL );

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// Device open sequence finished --> notify done
	psi_hd_edd_open_done( p_setup->session.hd_nr, p_setup );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_setup( PSI_EDDS_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDS_UPPER_DSB_PTR_TYPE       pDSB;
 	EDDS_UPPER_DDB_SETUP_PTR_TYPE pDDB;

	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_setup->session.hDDB_ptr != LSA_NULL );

	pDSB = &p_setup->iniDSB;  // Use setupblock as pParam
	PSI_ASSERT( pDSB != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_DDB_SETUP_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_SETUP );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_edds_device_setup_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	pDDB       = (EDDS_UPPER_DDB_SETUP_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pDDB->pDSB = pDSB;
	pDDB->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_setup_request_done;
	pDDB->hDDB = *p_setup->session.hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_shutdown_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                retVal;
	PSI_EDDS_SESSION_PTR_TYPE p_session;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK            == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM        == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_SHUTDOWN == EDD_RQB_GET_SERVICE( pRQB ));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_session != LSA_NULL );

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	/* Disable the IR */
	PSI_HD_DISABLE_IR( p_session->hd_sys_handle );

	// continue with release NRT
	psi_edds_release_nrt( p_session );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_shutdown( PSI_EDDS_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE           pRQB;
	EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE pShutdown;

	PSI_ASSERT( p_session != LSA_NULL );
	PSI_ASSERT( p_session->hDDB_ptr != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_DDB_SHUTDOWN_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_SHUTDOWN );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register session data for next request in done
	p_session->req_done_func = psi_edds_device_shutdown_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pShutdown       = (EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pShutdown->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_close_request_done;
	pShutdown->hDDB = *p_session->hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_release_nrt_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                retVal;
	PSI_EDDS_SESSION_PTR_TYPE p_session;
	PSI_HD_STORE_PTR_TYPE     p_hd;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK                == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM            == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_COMP_NRT_REL == EDD_RQB_GET_SERVICE( pRQB ));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_session != LSA_NULL );

	// Unregister the EDDS IO Buffer handle (used for PI alloc/free)
	p_hd = psi_hd_get_hd_store( p_session->hd_nr );
	PSI_ASSERT( p_hd != LSA_NULL );

    if ( p_hd->hd_hw.edd.edds.io_buffer_handle != LSA_NULL )
    {
	    EDDS_IOBuffer_Deinit( p_hd->hd_hw.edd.edds.io_buffer_handle );
	    p_hd->hd_hw.edd.edds.io_buffer_handle = LSA_NULL;
    }

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// continue with release SRT
	psi_edds_release_srt( p_session );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_release_nrt( PSI_EDDS_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE          pRQB;
	EDDS_UPPER_CMP_NRT_REL_PTR_TYPE pNRTRelease; 

	PSI_ASSERT( p_session != LSA_NULL );
	PSI_ASSERT( p_session->hDDB_ptr != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_CMP_NRT_REL_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_COMP_NRT_REL );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register session data for next request in done
	p_session->req_done_func = psi_edds_release_nrt_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pNRTRelease       = (EDDS_UPPER_CMP_NRT_REL_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pNRTRelease->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_close_request_done;
	pNRTRelease->hDDB = *p_session->hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_release_srt_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                retVal;
	PSI_EDDS_SESSION_PTR_TYPE p_session;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK                == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM            == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_COMP_SRT_REL == EDD_RQB_GET_SERVICE( pRQB ));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_session != LSA_NULL );

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// continue with device close
	psi_edds_device_close( p_session );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_release_srt( PSI_EDDS_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE          pRQB;
	EDDS_UPPER_CMP_SRT_REL_PTR_TYPE pSRTRelease; // SRT release parameter block

	PSI_ASSERT( p_session != LSA_NULL );
	PSI_ASSERT( p_session->hDDB_ptr != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_CMP_SRT_REL_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_COMP_SRT_REL );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register session data for next request in done
	p_session->req_done_func = psi_edds_release_srt_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pSRTRelease       = (EDDS_UPPER_CMP_SRT_REL_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pSRTRelease->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_close_request_done;
	pSRTRelease->hDDB = *p_session->hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_close_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                retVal;
	PSI_EDDS_SESSION_PTR_TYPE p_session;

	EDD_UPPER_RQB_PTR_TYPE pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( EDD_STS_OK         == EDD_RQB_GET_RESPONSE( pRQB ));
	PSI_ASSERT( EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE( pRQB ));
	PSI_ASSERT( EDDS_SRV_DEV_CLOSE == EDD_RQB_GET_SERVICE( pRQB ));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE) PSI_RQB_GET_USER_ID_PTR( pRQB );
	PSI_ASSERT( p_session != LSA_NULL );

	// Reset the hDDB 
	*p_session->hDDB_ptr = LSA_NULL;

	EDDS_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT( retVal == LSA_RET_OK );

	EDDS_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT( retVal == EDD_STS_OK );

	// Device close sequence finished --> notify done
	psi_hd_edd_close_done( p_session->hd_nr, p_session );
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_close( PSI_EDDS_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	EDDS_UPPER_DDB_REL_PTR_TYPE pClose;

	PSI_ASSERT( p_session != LSA_NULL );
	PSI_ASSERT( p_session->hDDB_ptr != LSA_NULL );

	EDDS_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
	PSI_ASSERT( pRQB != LSA_NULL );

	EDDS_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDS_RQB_DDB_REL_TYPE) );
	PSI_ASSERT( pRQB->pParam != LSA_NULL );

	// set opcode and service
	EDD_RQB_SET_OPCODE( pRQB, EDD_OPC_SYSTEM );
	EDD_RQB_SET_SERVICE( pRQB, EDDS_SRV_DEV_CLOSE );

	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_EDDS );

	// Register session data for next request in done
	p_session->req_done_func = psi_edds_device_close_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pClose       = (EDDS_UPPER_DDB_REL_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pClose->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE) psi_edds_close_request_done;
	pClose->hDDB = *p_session->hDDB_ptr;

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_prepare_setup_data(
	PSI_EDDS_SETUP_PTR_TYPE   p_setup, 
	PSI_HD_INPUT_PTR_TYPE     p_inp, 
	PSI_HD_STORE_PTR_TYPE     p_hd )
{
	LSA_UINT16                 port_idx;
	PSI_HD_IF_INPUT_PTR_TYPE   if_ptr;
	PSI_HD_EDDS_PTR_TYPE       edd_ptr;
	PSI_HD_HW_PTR_TYPE         hd_hw_ptr;
	PSI_DETAIL_STORE_PTR_TYPE  pDetails;

	PSI_ASSERT( p_setup != LSA_NULL );
	PSI_ASSERT( p_inp   != LSA_NULL );
	PSI_ASSERT( p_hd    != LSA_NULL );
	PSI_ASSERT( p_inp->nr_of_if <= PSI_CFG_MAX_HD_IF_CNT );

	if_ptr    = &p_inp->pnio_if[0]; // Only one IF
	hd_hw_ptr = &p_hd->hd_hw;
	edd_ptr   = &p_hd->hd_hw.edd.edds;

	// nLLIF parameter (hw-dependent)
	p_setup->iniDPB.pLLFunctionTable  = hd_hw_ptr->edd.edds.ll_function_table;
	p_setup->iniDPB.pLLManagementData = hd_hw_ptr->edd.edds.ll_handle;
	p_setup->iniDPB.pLLParameter      = hd_hw_ptr->edd.edds.ll_parameter;

    // preset Gigabit-Support
    p_setup->session.b_gigabit_support = LSA_FALSE; // = PSI default configuration

    if ( p_inp->stdmac.use_setting ) // Usage of StdMAC inputs?
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
			"psi_edds_prepare_setup_data(): Gigabit-Support input (%u)",
		    p_inp->stdmac.gigabit_support );

        if ( p_inp->stdmac.gigabit_support ) // Gigabit-Support is requested?
        {
            #if defined (EDDS_CFG_HW_INTEL) // using INTEL LL
            p_setup->session.b_gigabit_support = LSA_TRUE;
            #elif !defined (EDDS_CFG_HW_INTEL)
		    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_edds_prepare_setup_data(): invalid params, Gigabit-Support only allowed with compiler-switch EDDS_CFG_HW_INTEL" );
		    PSI_FATAL( 0 );
            #endif //EDDS_CFG_HW_INTEL
        }
    }

    #if defined (EDDS_CFG_HW_INTEL) // using INTEL LL
    if ( p_setup->session.b_gigabit_support ) // Gigabit support is requested?
    {
	    // set disable 1000 MBit support to FALSE
	    ((INTEL_LL_PARAMETER_TYPE *)(p_setup->iniDPB.pLLParameter))->Disable1000MBitSupport = LSA_FALSE;
    }
    else // Gigabit-Support is not requested
    {
	    // set disable 1000 MBit support to TRUE
	    ((INTEL_LL_PARAMETER_TYPE *)(p_setup->iniDPB.pLLParameter))->Disable1000MBitSupport = LSA_TRUE;
    }
    #endif //EDDS_CFG_HW_INTEL

	// ---- setup DPB ---------------
    p_setup->iniDPB.hSysDev             = (EDDS_SYS_HANDLE)p_hd->hd_sys_handle; // system device handle
	p_setup->iniDPB.InterfaceID         = if_ptr->edd_if_id;                    // The InterfaceID is a number (1...16) that clearly identifies the device.
	p_setup->iniDPB.TraceIdx            = if_ptr->trace_idx;                    // Trace index used for this interface
    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    #if defined (EDDS_CFG_HW_INTEL)
    if ( p_setup->session.b_gigabit_support ) // Gigabit support is requested?
    {
        p_setup->iniDPB.RxBufferCnt     = 2*1489;                               // 1GBit, 1ms scheduler cycle (wirespeed with minimal packets)
        p_setup->iniDPB.TxBufferCnt     = 2*1489;                               // 1GBit, 1ms scheduler cycle (wirespeed with minimal packets)
    }
    else // 100 MBit
    {
        p_setup->iniDPB.RxBufferCnt     = 2*149;                                // 100MBit, 1ms scheduler cycle
        p_setup->iniDPB.TxBufferCnt     = 2*149;                                // 100MBit, 1ms scheduler cycle
    }
    #elif !defined (EDDS_CFG_HW_INTEL)
    p_setup->iniDPB.RxBufferCnt         = 32*2*149;                             // 100MBit, 32ms scheduler cycle
    p_setup->iniDPB.TxBufferCnt         = 32*2*149;                             // 100MBit, 32ms scheduler cycle
    #endif //EDDS_CFG_HW_INTEL
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	p_setup->iniDPB.RxBufferCnt         = 8;                                    // Number of receive buffers that are to be set up for the device. The receive buffer has a size of RxBufferSize.
	p_setup->iniDPB.TxBufferCnt         = 8;                                    // The value specifies how many send requests (buffers) can be transfered "at the same time" to the Ethernet controller. TxBufferCnt should be > (max cSRT + max NRT + max aSRT) per cycle.
	p_setup->iniDPB.RxBufferSize        = EDD_FRAME_BUFFER_LENGTH;              // Size of the receive buffer ( must not change that!!! )
	p_setup->iniDPB.RxBufferOffset      = 0;                                    // Offset in the receive buffer starting with which the receive frame is to be stored.
	p_setup->iniDPB.TxToutCnt           = 0;                                    // Timeout for Transmit in seconds. Timeout monitoring is 1 second.
    #if defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD)
    p_setup->iniDPB.RxIsrMaxFrameCnt    = 100;
    #endif //EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    p_setup->iniDPB.ZeroCopyInterface   = LSA_TRUE;                             // preset: use EDDS-Zero-Copy-Interface
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
    if ( p_inp->stdmac.use_setting ) // Usage of StdMAC inputs?
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
			"psi_edds_prepare_setup_data(): NRT-Copy-Interface input (%u)",
		    p_inp->stdmac.nrt_copy_interface );

        if ( p_inp->stdmac.nrt_copy_interface ) // NRT Copy Interface is requested?
        {
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            hd_hw_ptr->b_edd_nrt_copy_if_on   = LSA_TRUE;                       // use EDDS-Copy-Interface
            p_setup->iniDPB.ZeroCopyInterface = LSA_FALSE;                      // use EDDS-Copy-Interface
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_edds_prepare_setup_data(): invalid params, NRT-Copy-Interface only allowed with compiler-switch EDDS_CFG_CYCLIC_SCHEDULING_MODE (=NEA)" );
		    PSI_FATAL( 0 );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
        }
    }

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    #if defined (EDDS_CFG_HW_INTEL) // using INTEL LL
    p_setup->iniDPB.schedulerCycle      = (LSA_UINT16)1;                        // scheduler cycle = 1ms
    #elif !defined (EDDS_CFG_HW_INTEL)
    p_setup->iniDPB.schedulerCycle      = (LSA_UINT16)32;                       // scheduler cycle = 32ms
    #endif //EDDS_CFG_HW_INTEL
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

    // interfaces and ports
    p_setup->iniDPB.MaxInterfaceCntOfAllEDD = p_hd->nr_of_all_if;               // Number of all interfaces of all EDDs.    
    p_setup->iniDPB.MaxPortCntOfAllEDD      = p_hd->nr_of_all_ports;            // Number of all ports of all EDDs.

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
                        "psi_edds_prepare_setup_data(): MaxIFCount: %u, MaxPortCount: %u",
                        p_setup->iniDPB.MaxInterfaceCntOfAllEDD,
                        p_setup->iniDPB.MaxPortCntOfAllEDD);

    // MRP settings
    #ifdef LLIF_CFG_SWITCH_SUPPORT
    #ifndef EDDS_DONT_USE_MRP
    #ifdef PSI_MESSAGE
    #pragma PSI_MESSAGE("TODO complete setup MRP and MPR port settings, if known!")
    #endif
	{
		if (if_ptr->mrp.use_setting) // Usage of MRP inputs?
		{
		    p_setup->iniDPB.SWI.MaxMRP_Instances         = if_ptr->mrp.max_instances;
		    p_setup->iniDPB.SWI.MRPSupportedRole         = if_ptr->mrp.supported_role;
		    p_setup->iniDPB.SWI.MRPSupportedMultipleRole = if_ptr->mrp.supported_multiple_role;
		    p_setup->iniDPB.SWI.MRPDefaultRoleInstance0  = if_ptr->mrp.default_role_instance0;
        }
        else
		{
            // default = MRP Off
		    p_setup->iniDPB.SWI.MaxMRP_Instances         = 0;
		    p_setup->iniDPB.SWI.MRPSupportedRole         = EDD_MRP_ROLE_NONE;
		    p_setup->iniDPB.SWI.MRPSupportedMultipleRole = EDD_MRP_ROLE_NONE;
		    p_setup->iniDPB.SWI.MRPDefaultRoleInstance0  = EDD_MRP_ROLE_NONE;
        }

		for( port_idx = 0; port_idx < EDDS_MAX_PORT_CNT; port_idx++ )
		{
			p_setup->iniDPB.SWI.MRPRingPort[port_idx] = EDD_MRP_NO_RING_PORT;
		}
	}
    #endif //EDDS_DONT_USE_MRP
    #endif //LLIF_CFG_SWITCH_SUPPORT

	///////////////////////////////////////////////////////////////////////////
	// setup DSB
	p_setup->iniDSB.LinkSpeedMode = EDD_LINK_UNCHANGED;

	// setup IF MAC
	PSI_MEMCPY( p_setup->iniDSB.MACAddress[0].MacAdr,
		        hd_hw_ptr->if_mac[0],
		        EDD_MAC_ADDR_SIZE );

	// setup PORT MAC
	for( port_idx = 0 ; port_idx < p_inp->nr_of_ports ; port_idx++ )
	{
	    PSI_MEMCPY( p_setup->iniDSB.MACAddress[port_idx+1].MacAdr,
			        hd_hw_ptr->port_mac[port_idx],
			        EDD_MAC_ADDR_SIZE );
	}

	///////////////////////////////////////////////////////////////////////////
	// setup NRT subcomponent
	// number of receive buffers
	p_setup->iniNRT.RxBufferCntARPIP     = 32;	// for NRT frames: ICMP, IGMP TCP UDP IP_OTHER ARP RARP
	p_setup->iniNRT.RxBufferCntOther     = 16;	// for NRT frames: DCP, TSYNC, STDBY, HSR, SINEC_FWL, OTHER, LLDP, MRP, OTHER
	p_setup->iniNRT.RxBufferCntASRTAlarm = 128;	// for NRT frames: ASRT, UDP_ASRT
	p_setup->iniNRT.RxBufferCntASRTOther = 64;	// for NRT frames: DCP, UDP_DCP, PTCP

	p_setup->iniNRT.RxFilterUDP_Broadcast = if_ptr->nrt.RxFilterUDP_Broadcast;
	p_setup->iniNRT.RxFilterUDP_Unicast   = if_ptr->nrt.RxFilterUDP_Unicast;

	// ---- setup SRT subcomponent
	pDetails = psi_get_detail_store( (LSA_UINT16)p_inp->hd_id, (LSA_UINT16)if_ptr->if_id );
	PSI_ASSERT( pDetails != LSA_NULL );

    if //Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32,64,128,256,512,1024 (=1ms,2ms,4ms,8ms,16ms,32ms Cylce-Time)
       (   (p_inp->send_clock_factor != (LSA_UINT16)32)
	    && (p_inp->send_clock_factor != (LSA_UINT16)64)
	    && (p_inp->send_clock_factor != (LSA_UINT16)128)
	    && (p_inp->send_clock_factor != (LSA_UINT16)256)
	    && (p_inp->send_clock_factor != (LSA_UINT16)512)
	    && (p_inp->send_clock_factor != (LSA_UINT16)1024))
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
		    "psi_edds_prepare_setup_data(): invalid params, startup send_clock_factor (%u)",
		    p_inp->send_clock_factor );
		PSI_FATAL( 0 );
    }

    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
		"psi_edds_prepare_setup_data(): send_clock_factor input (%u) EDDS SchedulerCycleMs (%u)",
		p_inp->send_clock_factor, p_setup->iniDPB.schedulerCycle );
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
		"psi_edds_prepare_setup_data(): send_clock_factor input (%u)",
		p_inp->send_clock_factor );
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

    p_setup->iniSRT.CSRT.CycleBaseFactor             = p_inp->send_clock_factor;
	p_setup->iniSRT.CSRT.ProviderCycleDropCnt        = 10;
	p_setup->iniSRT.CSRT.ProviderCyclePhaseMaxCnt1ms = 5;

	p_setup->iniSRT.CSRT.ConsumerCntClass1 =  // All unicast/multicast class1
		pDetails->acp.cm_cl.nr_of_consumers_class1_unicast + 
		pDetails->acp.cm_sv.nr_of_consumers_class1_unicast +
		pDetails->acp.cm_mc.nr_of_consumers_class1_unicast + 
		pDetails->acp.cm_cl.nr_of_consumers_class1_multicast + 
		pDetails->acp.cm_sv.nr_of_consumers_class1_multicast +
		pDetails->acp.cm_mc.nr_of_consumers_class1_multicast;

	p_setup->iniSRT.CSRT.ConsumerCntClass2 =   // All unicast/multicast class2
		pDetails->acp.cm_cl.nr_of_consumers_class2_unicast + 
		pDetails->acp.cm_sv.nr_of_consumers_class2_unicast + 
		pDetails->acp.cm_mc.nr_of_consumers_class2_unicast +
		pDetails->acp.cm_cl.nr_of_consumers_class2_multicast + 
		pDetails->acp.cm_sv.nr_of_consumers_class2_multicast +
		pDetails->acp.cm_mc.nr_of_consumers_class2_multicast;

	p_setup->iniSRT.CSRT.ProviderCnt =   
		p_setup->iniSRT.CSRT.ConsumerCntClass1 + 
		p_setup->iniSRT.CSRT.ConsumerCntClass2;

	p_setup->iniSRT.CSRT.ConsumerFrameIDBaseClass1 = 0xF800 - 
		( pDetails->acp.cm_cl.nr_of_consumers_class1_unicast + 
		  pDetails->acp.cm_sv.nr_of_consumers_class1_unicast +
		  pDetails->acp.cm_mc.nr_of_consumers_class1_unicast );

	p_setup->iniSRT.CSRT.ConsumerFrameIDBaseClass2 = 0xBC00 - 
		( pDetails->acp.cm_cl.nr_of_consumers_class2_unicast + 
		  pDetails->acp.cm_sv.nr_of_consumers_class2_unicast + 
	      pDetails->acp.cm_mc.nr_of_consumers_class2_unicast );

    if // Gigabit-Support enabled?
       ( p_setup->session.b_gigabit_support )
    {
        // set counters to minimum
	    p_setup->iniSRT.CSRT.ConsumerCntClass1 = 1; // All unicast/multicast class1
	    p_setup->iniSRT.CSRT.ConsumerCntClass2 = 0; // All unicast/multicast class2
	    p_setup->iniSRT.CSRT.ProviderCnt       = 1;
    }
}

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling pcIOX EDDS support (PSI_CFG_USE_EDDS=" PSI_STRINGIFY(PSI_CFG_USE_EDDS) ")")
#endif
#endif // PSI_CFG_USE_EDDS

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
