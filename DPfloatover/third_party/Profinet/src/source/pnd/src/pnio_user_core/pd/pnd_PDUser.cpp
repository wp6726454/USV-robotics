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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_PDUser.cpp                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation for PD (CM-PD) User                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3503
#define PND_MODULE_ID       3503

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

/* StdLib++ */
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_ValueHelper.h"
#include "pnd_UserNode.h"
#include "pnd_UserStore.h"

#include "pnd_ParamStore.h"
#include "pnd_PDEventHandler.h"
#include "pnd_PDSubslot.h"
#include "pnd_PDUser.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CPDUser (CM-PD) user class
/////////////////////////////////////////////////////////////////////////////

CPDUser::CPDUser(): CUserNode()
{
	m_pnioHandle = CM_INVALID_HANDLE;
	m_sysPath    = PSI_SYS_PATH_INVALID;

	m_pEventHandler  = 0;
	m_responseRQB    = 0;
	m_actPrmRecIdx   = 0;
	m_isCreated      = LSA_FALSE;
	m_syncState      = PND_SYNC_STATE_NOT_CONFIGURED;

	m_prmRecList.clear();

	pnd_memset( &m_pndHandle, 0, sizeof( PND_HANDLE_TYPE ));

    pnd_memset( &m_pdParam, 0, sizeof( PND_PARAM_PD_HD_IF_TYPE ));
}

/*----------------------------------------------------------------------------*/
CPDUser::CPDUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent) : CUserNode( pHandle, pParent )
{
	m_pnioHandle = CM_INVALID_HANDLE;
	m_sysPath    = PSI_SYS_PATH_INVALID;

	m_pEventHandler  = 0;
	m_responseRQB    = 0;
	m_actPrmRecIdx   = 0;
	m_isCreated      = LSA_FALSE;
	m_syncState      = PND_SYNC_STATE_NOT_CONFIGURED;

	m_prmRecList.clear();

    pnd_memset( &m_pdParam, 0, sizeof( PND_PARAM_PD_HD_IF_TYPE ));
}

/*----------------------------------------------------------------------------*/
CPDUser::~CPDUser()
{
	if ( m_pEventHandler != 0 )
	{
		delete( m_pEventHandler );
		m_pEventHandler = 0;
	}

	// Free the stored elements in subslot-MAP
	deleteMap( m_subslotMap );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams )
{
	PND_PARAM_PD_HD_IF_PTR_TYPE pPD = (PND_PARAM_PD_HD_IF_PTR_TYPE)pParams;
	PND_ASSERT( pPD != 0 );
	PND_ASSERT( laddr_ptr != 0 );

	// setup internal attributes and create the PDev subslot based on config 
	PND_ASSERT( m_pParentNode != 0 );
	PND_ASSERT(( m_pndHandle.hd_nr > 0 ) && ( m_pndHandle.hd_nr <= PSI_CFG_MAX_HD_CNT ));
	PND_ASSERT(( m_pndHandle.if_nr > 0 ) && ( m_pndHandle.if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

    m_pdParam = *pPD;

	// Prepare SYS path for IF specific CM-PD user
	PSI_SYSPATH_SET_PATH( m_sysPath, PSI_PATH_IF_APP_CMPD );
	PSI_SYSPATH_SET_IF( m_sysPath, m_pndHandle.if_nr );
	PSI_SYSPATH_SET_HD( m_sysPath, m_pndHandle.hd_nr );

	// create the event handler for CM PD events
	m_pEventHandler = new CPDEventHandler( &m_pndHandle, this );
	PND_ASSERT( m_pEventHandler != 0 );

	// create the PD subslots
	for ( LSA_UINT16 portID = 0; portID < pPD->map_count; portID++ )
	{
		PND_PARAM_PD_SUB_ENTRY_PTRY_TYPE pMap = &pPD->pdev_sub[portID];

		// Create the subslots and add it to map
		// Note: information is used later for PD Create service
		CPDSubslot *pSub = new CPDSubslot( portID, &m_pndHandle, this );
		PND_ASSERT( pSub != 0 );

		pSub->Create( 
			pMap->slot_nr,
			pMap->subslot_nr,
			pMap->mod_ident,
			pMap->sub_ident,
			pMap->laddr );

		AddNode( m_subslotMap, pSub );

		// Register LADDR in addresslist if avaliable
		if ( pMap->laddr != PND_LADDR_INVALID )
		{
			laddr_ptr->laddr_ref[pMap->laddr] = pSub;

			if( portID == 0 )
			{
			    laddr_ptr->laddr_type[pMap->laddr] = PDSUBSLOTINTERFACE;
			}
			else
			{
				laddr_ptr->laddr_type[pMap->laddr] = PDSUBSLOTPORT;
			}

			laddr_ptr->laddr_device_nr[pMap->laddr]  = 0;
			laddr_ptr->laddr_slot_nr[pMap->laddr]    = pMap->slot_nr;
			laddr_ptr->laddr_subslot_nr[pMap->laddr] = pMap->subslot_nr;
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CPDUser::GetPnioHandle()
{
	return ( m_pnioHandle );
}

/*----------------------------------------------------------------------------*/
CPDSubslot* CPDUser::GetMappedSubslot( LSA_UINT16 subslotNr )
{
	UserNodeVector subList;
	CPDSubslot *pMapped = 0;

	LSA_INT count = getNodeList( m_subslotMap, subList );

	for ( LSA_INT idx = 0; idx < count; idx++ )
	{
		CPDSubslot *pSub = dynamic_cast<CPDSubslot*>(subList[idx]);

		if (( pSub != 0 ) && ( subslotNr == pSub->GetSubslotNr()))
		{
			pMapped = pSub;
			break;
		}
	}

	return ( pMapped );
}

/*----------------------------------------------------------------------------*/
CPDSubslot* CPDUser::GetSubslot( LSA_UINT16 portID )
{
	string key = CPDSubslot::GetKey( portID );
	CPDSubslot* pSub = dynamic_cast<CPDSubslot*>( getSubNode( m_subslotMap, key));

	return ( pSub );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CPDUser::UpdateFromDiffList( LSA_VOID* pDiffList )
{
	CM_LIST_ENTRY_PTR_TYPE diffList = (CM_LIST_ENTRY_PTR_TYPE)pDiffList;

	CM_UPPER_PD_DIFF_AP_PTR_TYPE        pAp;
	CM_UPPER_PD_DIFF_MODULE_PTR_TYPE    pMod;
	CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE pSub;

	PND_ASSERT( diffList != 0 );

	if ( !CmListIsEmpty (diffList) ) // Empty list ?
	{
		PND_PD_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CPDUser::UpdateFromDiffList(): ---- DIFF list, PNIO-IFNr(%u) -----------", 
			m_pndHandle.pnio_if_nr );

		for ( CmListForeach( pAp, diffList, CM_UPPER_PD_DIFF_AP_PTR_TYPE ))
		{
			for ( CmListForeach( pMod, &pAp->module_list, CM_UPPER_PD_DIFF_MODULE_PTR_TYPE ))
			{
				PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
					"CPDUser::UpdateFromDiffList():  api(%#x) slot_nr(%#x) ident(%u/%#x)", 
					pAp->api, pMod->slot_nr,  pMod->mod_ident, pMod->mod_ident );

				for ( CmListForeach( pSub, &pMod->submodule_list, CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE ))
				{
					PND_PD_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
						"CPDUser::UpdateFromDiffList():    subslot_nr(%u/%#x) sub_ident(%u/%#x) sub_state(%u/%#x)",
						pSub->subslot_nr, pSub->subslot_nr,
						pSub->sub_ident, pSub->sub_ident,
						pSub->sub_state, pSub->sub_state );

					// Update BGZD for PDEV subslot
					CPDSubslot *pSubObj = GetMappedSubslot( pSub->subslot_nr );
					PND_ASSERT( pSubObj != 0 );

					pSubObj->SetState( pSub->sub_state );
				}
			}
		}
	}
	else
	{
		PND_PD_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CPDUser::UpdateFromDiffList(): ---- DIFF list is empty, PNIO-IFNr(%u) -----------", 
			m_pndHandle.pnio_if_nr );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::ResetBgzdState()
{
	// Reset the summary and the subslot BGZD info to OK for new first update
	UserNodeVector subList;

	m_bgzdStore.UpdateFromSubslotState( PND_BGZD_SUB_OK );

	LSA_INT count = getNodeList( m_subslotMap, subList );

	for ( LSA_INT idx = 0; idx < count; idx++ )
	{
		CPDSubslot *pSub = dynamic_cast<CPDSubslot*>(subList[idx]);
		PND_ASSERT( pSub != 0 );

		pSub->SetState( PND_BGZD_SUB_OK );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::SetARP()
{
	UserNodeVector subList;

	LSA_INT count = getNodeList( m_subslotMap, subList );

	for ( LSA_INT idx = 0; idx < count; idx++ )
	{
		CPDSubslot *pSub = dynamic_cast<CPDSubslot*>(subList[idx]);
		PND_ASSERT( pSub != 0 );

		pSub->SetARP();
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CPDUser::PrmStartup()
{
	// Start with prepare 
	// Note: if no records stored in list (default PRM), PRM END is done after prepare
	PND_ASSERT( m_prmRecList.empty());

	// get the PRM records from param storagee  (siehe CreatePrmRecordsPD)
	CParamStore *paramObj = CParamStore::GetInstance();
	PND_ASSERT( paramObj != 0 );

	paramObj->CreatePrmRecordsPD( &m_pndHandle, &m_prmRecList );

	// Start with prepare 
	// Note: if no records stored in list (default PRM), PRM END is done after prepare
	PrmBegin();

}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmFinished( LSA_UINT32 pnioErr )
{
	// PRM sequence finished, check if an upper event is necessary 
	// note: i.e: in case of automatic startup
	if ( m_isStartup ) // automatic startup ?
	{
		// get the PRM records from param storagee  (siehe CreatePrmRecordsPD)
		CParamStore *paramObj = CParamStore::GetInstance();
		PND_ASSERT( paramObj != 0 );

		paramObj->DeletePrmRecordsPD( &m_prmRecList );

		// Signal startup complete
		startupDone( pnioErr );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::CheckLinkState()
{
	// check Linkup state of interface (portID = 0)
	// Note: if any Link is up, the auto portID=0 is also up

	CPDSubslot *pSub = GetSubslot( 0 );

	if ( pSub != 0 )  // IF sub avaliable ?
	{
		LSA_BOOL allLinkDown = ( pSub->IsLinkUp() ) ? LSA_FALSE : LSA_TRUE;

		m_bgzdStore.SetNoLinkUp( allLinkDown );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::Startup( LSA_VOID* p_rb )
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStartup  = LSA_TRUE;
	m_isShutdown = LSA_FALSE;

	// Store the upper RQB, and Startup with open the PNIO channel for CM PD
	m_responseRQB = pRQB;

	OpenChannel();
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::Shutdown( LSA_VOID* p_rb )
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isShutdown = LSA_TRUE;
	m_isStartup  = LSA_FALSE;

	// Store the upper RQB, start shutdown CM PD user till closing the PNIO channel
	m_responseRQB = pRQB;

	if ( m_pnioHandle != CM_INVALID_HANDLE )  // CM channel open ?
	{
		// start shutdown CM PD user till closing the PNIO channel
		if ( m_isCreated ) // CM PD created ?
		{
			DeleteCmPd();
		}
		else
		{
			CloseChannel();
		}
	}
	else
	{
		// Trigger shutdown complete
		shutdownDone( PNIO_OK );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::StartUpdateBgzd()
{
	// Note: in case of shutdwon, BGZD update is not done
	if ( !m_isShutdown ) // Shutdown in progress
	{
		UserNodeVector subList;
		LSA_UINT16 nrOfLinkUp = 0;

		m_bgzdStore.Reset();

		// Update the own BGZD from BGZD of all childs as summary
		LSA_INT count = getNodeList( m_subslotMap, subList );

		for ( LSA_INT idx = 0; idx < count; idx++ )
		{
			CPDSubslot *pSub = dynamic_cast<CPDSubslot*>( subList[idx] );

			if ( pSub != 0 )
			{
				pSub->UpdateBgzd( m_bgzdStore );
				nrOfLinkUp += pSub->IsLinkUp() ? 1 : 0;
			}
		}

		LSA_BOOL linkState = ( nrOfLinkUp == 0 ) ? LSA_TRUE : LSA_FALSE;
		m_bgzdStore.SetNoLinkUp( linkState );
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CPDUser::GetKey()
{
	CValueHelper valObj;

	string result = valObj.UINT16ToString( m_pndHandle.pnio_if_nr );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CPDUser::GetNumber()
{
	return ( m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
/* PD (CM-PD) Request functions                                               */
/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::OpenChannel()
{
	PND_ASSERT( m_pnioHandle == CM_INVALID_HANDLE      );
	PND_ASSERT( m_sysPath    != PSI_SYS_PATH_INVALID );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_OPEN_CHANNEL, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::OpenChannel(): Open PD user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
		m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

	LSA_SYS_PATH_TYPE path = PSI_SYSPATH_GET_PATH( m_sysPath );

	pRQB->args.channel.open->handle                    = CM_INVALID_HANDLE;
	pRQB->args.channel.open->handle_upper              = (LSA_UINT8)path;
	pRQB->args.channel.open->sys_path                  = m_sysPath;
	pRQB->args.channel.open->cm_request_upper_done_ptr = (CM_UPPER_CALLBACK_FCT_PTR_TYPE)pnd_pnio_user_request_lower_done;

	eps_user_open_channel_lower( pRQB, this, CPDUser::OpenChannelDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::CloseChannel()
{
	PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CLOSE_CHANNEL, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::CloseChannel(): Close PD user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
		m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

	// Stop listening CM indications
	m_pEventHandler->StopListening();

	eps_user_close_channel_lower( pRQB, this, CPDUser::CloseChannelDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::CreateCmPd()
{
	UserNodeVector subList;
	PND_PARAM_PSI_LD_TYPE psiParams;

	PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

	LSA_UINT16 count     = (LSA_UINT16)getNodeList( m_subslotMap, subList );
	LSA_UINT16 more_size = count * sizeof(CM_PD_MAP_TYPE);

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_CREATE, m_pnioHandle, &m_pndHandle, more_size );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::CreateCmPd(): PD CREATE, mapCount(%u) rqb(%08x)", 
		count, pRQB );

	CParamStore* pParam = CParamStore::GetInstance();
	PND_ASSERT( pParam != 0 );

	pParam->CreateAnnotation( &m_pndHandle, pRQB->args.pd.create->annotation );

	pRQB->args.pd.create->max_alarm_data_length = CM_ALARM_DATA_LENGTH_MIN; /* enough? */
	
	// OEM Identification Data
	pRQB->args.pd.create->oem_data = (CM_UPPER_OEM_DATA_PTR_TYPE) pnd_mem_alloc( sizeof(CM_PD_OEM_DATA_TYPE) );

	//Get PSI Params
    pParam->GetParamPsiLD( &psiParams );

	PSI_IM_INPUT_PTR_TYPE pPsiInput = &psiParams.im_param;
	
	pRQB->args.pd.create->oem_data->vendor_id                            = (PNIO_UINT16) ((pPsiInput->vendor_id_high<<8) & 0xFF00) 
		                                                                 + (PNIO_UINT16) ((pPsiInput->vendor_id_low)     & 0x00FF);
	pRQB->args.pd.create->oem_data->device_id                            = 0x08;
	pRQB->args.pd.create->oem_data->IM_Hardware_Revision                 = pPsiInput->hw_revision;
	pRQB->args.pd.create->oem_data->SWRevisionPrefix                     = pPsiInput->sw_version.revision_prefix;
	pRQB->args.pd.create->oem_data->IM_SWRevision_Functional_Enhancement = pPsiInput->sw_version.functional_enhancement;
	pRQB->args.pd.create->oem_data->IM_SWRevision_Bug_Fix                = pPsiInput->sw_version.bug_fix;
	pRQB->args.pd.create->oem_data->IM_SWRevision_Internal_Change        = pPsiInput->sw_version.internal_change;
	
	pnd_memset( pRQB->args.pd.create->oem_data->IM_Serial_Number, 
		        (PNIO_UINT8)(0x20)/* Filling with blanks */,
				sizeof(pRQB->args.pd.create->oem_data->IM_Serial_Number) );

	pnd_memcpy( pRQB->args.pd.create->oem_data->IM_Annotation, 
		        pPsiInput->device_type, 
		        sizeof(pPsiInput->device_type) );

	pnd_memcpy( pRQB->args.pd.create->oem_data->IM_OrderID, 
		        pPsiInput->order_id, 
		        sizeof(pPsiInput->order_id) );
	
	pRQB->args.pd.create->map_count = (LSA_UINT16)count;

	for ( LSA_UINT16 portID = 0; portID < count; portID++ )
	{
		CM_UPPER_PD_MAP_PTR_TYPE pMap = &pRQB->args.pd.create->map[portID];

		CPDSubslot* pSub = dynamic_cast<CPDSubslot*>( subList[portID] );
		PND_ASSERT( pSub != 0 );

		pMap->slot_nr    = pSub->GetSlotNr();
		pMap->subslot_nr = pSub->GetSubslotNr();
		pMap->mod_ident  = pSub->GetSlotIdent();
		pMap->sub_ident  = pSub->GetSubslotIdent();
	}

	eps_user_request_lower( pRQB, this, CPDUser::CreateCmPdDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::DeleteCmPd()
{
	PND_ASSERT( m_pnioHandle !=  CM_INVALID_HANDLE );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_DELETE, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::DeleteCmPd(): PD DELETE, rqb(%08x) -----------", pRQB );

	eps_user_request_lower( pRQB, this, CPDUser::DeleteCmPdDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmBegin()
{
	PND_ASSERT( m_pnioHandle !=  CM_INVALID_HANDLE );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_PRM_BEGIN, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::PrmBegin(): PD PRM BEGIN, rqb(%08x) -----------", pRQB );

	eps_user_request_lower( pRQB, this, CPDUser::PrmBeginDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmWrite( PND_PARAM_PRM_RECORD_PTR_TYPE pEntry )
{
	PND_ASSERT( m_pnioHandle !=  CM_INVALID_HANDLE );
	PND_ASSERT( pEntry != 0 );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_PRM_WRITE, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::PrmWrite(): PD PRM WRITE, idx(%u) slot(%#x) sub(%#x) recIdx(%#x) length(%u) rqb(%08x)",
		m_actPrmRecIdx,
		pEntry->slot_nr,
		pEntry->subslot_nr,
		pEntry->record_index,
		pEntry->record_data_length,
		pRQB );

	pRQB->args.pd.prm_rdwr->slot_nr            = pEntry->slot_nr;
	pRQB->args.pd.prm_rdwr->subslot_nr         = pEntry->subslot_nr;
	pRQB->args.pd.prm_rdwr->record_index       = pEntry->record_index;
	pRQB->args.pd.prm_rdwr->record_data_length = pEntry->record_data_length;
	pRQB->args.pd.prm_rdwr->record_data        = pEntry->record_data;

	pRQB->args.pd.prm_rdwr->mk_remanent = LSA_FALSE;

	eps_user_request_lower( pRQB, this, CPDUser::PrmWriteDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmEnd()
{
	PND_ASSERT( m_pnioHandle !=  CM_INVALID_HANDLE );

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_PRM_END, m_pnioHandle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_PD_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::PrmEnd(): PD PRM End, rqb(%08x)", pRQB );

	pRQB->args.pd.prm_end->set_mrp_off = LSA_FALSE;

	eps_user_request_lower( pRQB, this, CPDUser::PrmEndDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::InterfaceOpen( PND_RQB_PTR_TYPE p_rb )
{
	// send RSP RQB to IOBC
   p_rb->args.iob_open.nr_of_alarm_res = (LSA_UINT16) m_subslotMap.size();
   p_rb->args.iob_open.pnio_err = PNIO_OK;

   LSA_RQB_SET_RESPONSE(p_rb, LSA_RET_OK);


   pnd_pnio_user_request_iobc_done( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::InterfaceClose( PND_RQB_PTR_TYPE p_rb )
{
	PND_RQB_PTR_TYPE pAlarmRqb = 0;

	pAlarmRqb = GetAlarmResource();

	while (pAlarmRqb != 0)
	{
		LSA_RQB_SET_RESPONSE(pAlarmRqb, PNIO_OK_CANCEL);
      
		pnd_pnio_user_request_iobc_done( pAlarmRqb );

		pAlarmRqb = GetAlarmResource();
	}

	p_rb->args.iob_open.pnio_err = PNIO_OK;
	LSA_RQB_SET_RESPONSE(p_rb, LSA_RET_OK);

	pnd_pnio_user_request_iobc_done( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::AlarmProvide( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(p_rb);

   SetAlarmResource(p_rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::AlarmReProvide( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT(p_rb);

    PND_PD_ALARM_SESSION_PTR_TYPE SessionData = (PND_PD_ALARM_SESSION_PTR_TYPE)p_rb->args.iob_alarm.SessionData;

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)SessionData->pCmRqb;

    CPDEventHandler *pThis = static_cast<CPDEventHandler*>(SessionData->pevHandler);

    SessionData->pCmRqb = 0;
    SessionData->pevHandler = 0;

    // Sessiondata is allocated by EventHandler
    delete  SessionData;

    p_rb->args.iob_alarm.SessionData = 0;

    SetAlarmResource(p_rb);

    // xxx RQB aus Map lesen key = 
    // value ist die Session structure
    // pEventHandler->AArAlarmIndicationDone( pRQB );

    pThis->AlarmIndicationDone(pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::AlarmIndication( LSA_VOID * p_rb )
{
   PND_ASSERT(p_rb);
   
   //return Alarm RQB Back to PND User Core
   pnd_pnio_user_request_iobc_done( p_rb );
}
/*----------------------------------------------------------------------------*/
/* PD (CM-PD) callback functions                                                */
/*----------------------------------------------------------------------------*/

LSA_VOID CPDUser::OpenChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_OPEN_CHANNEL);

	PND_PD_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::OpenChannelDone(): Open PD user channel done, rsp(%#x) PNIO-IFNr(%u) sys_path(%#x) handle(%u) rqb(%08x)", 
		rsp,
		pUser->m_pndHandle.pnio_if_nr, 
		pUser->m_sysPath,
		CM_RQB_GET_HANDLE( pRQB ),
		pRQB );

	pUser->m_pnioHandle = pRQB->args.channel.open->handle;
	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

	if ( rsp == CM_OK )
	{
		if ( pUser->m_isStartup ) // automatic startup ?
		{
			// Continue with CM PD creation
			pUser->CreateCmPd();
		}
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::CloseChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_CLOSE_CHANNEL);

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );
    PND_ASSERT( rsp == CM_OK );

	PND_PD_TRACE_04( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::CloseChannelDone(): Close PD user channel done, rsp(%#x) PNIO-IFNr(%u) sys_path(%#x) rqb(%08x)", 
		rsp,
		pUser->m_pndHandle.pnio_if_nr, 
		pUser->m_sysPath,
		pRQB );

	pUser->m_pnioHandle = CM_INVALID_HANDLE;
	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

	if ( pUser->m_isShutdown ) // shutdown in progress ?
	{
		LSA_UINT32 pnioRes = (rsp == CM_OK) ? PNIO_OK : PNIO_ERR_INTERNAL;
		// check shutdown complete
		pUser->shutdownDone( pnioRes );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::CreateCmPdDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_PD_CREATE);

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

	PND_PD_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::CreateCmPdDone(): PD CREATE done, rsp(%#x) PNIO-IFNr(%u) rqb(%08x)", 
		CM_RQB_GET_RESPONSE(pRQB),
		pUser->m_pndHandle.pnio_if_nr, 
		pRQB );

	// free oem_data
	pnd_mem_free( pRQB->args.pd.create->oem_data );

	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

	if ( rsp == CM_OK )
	{
		pUser->m_isCreated = LSA_TRUE;

		// Provide the event RQBs ( alarms, event, Link, sync )
		pUser->m_pEventHandler->StartListening( 1, (LSA_UINT16)pUser->m_subslotMap.size());

		if ( pUser->m_isStartup ) // automatic startup ?
		{
			// Init PDEV with PRM records
			pUser->PrmStartup();
		}
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::DeleteCmPdDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_PD_DELETE);

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);
    PND_ASSERT( rsp == CM_OK );

	PND_PD_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::DeleteCmPdDone(): PD DELETE done, rsp(%#x) PNIO-IFNr(%u) rqb(%08x)", 
		CM_RQB_GET_RESPONSE(pRQB),
		pUser->m_pndHandle.pnio_if_nr, 
		pRQB );

	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

	if ( rsp == CM_OK )
	{
		pUser->m_isCreated = LSA_FALSE;
	}

	if ( pUser->m_isShutdown ) // shutdown ?
	{
		PND_ASSERT( rsp == CM_OK );

		// Close the channel
		pUser->CloseChannel();
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmBeginDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0 );
	PND_ASSERT( pRQB  != 0 );
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_PD_PRM_BEGIN);

	if ( CM_RQB_GET_RESPONSE(pRQB) == CM_OK )
	{
		PND_PD_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CPDUser::PrmBeginDone(): PD PRM BEGIN done, rsp(%#x) rqb(%08x)", 
			CM_RQB_GET_RESPONSE(pRQB),
			pRQB );

		pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

		if ( !pUser->m_prmRecList.empty() )  // PRM records to write ?
		{
			// Write first PRM record
			pUser->m_actPrmRecIdx = 0;

			PND_PARAM_PRM_RECORD_PTR_TYPE pEntry = pUser->m_prmRecList[pUser->m_actPrmRecIdx];
			pUser->PrmWrite( pEntry );
		}
		else
		{
			// Finish with PRM END
			pUser->PrmEnd();
		}
	}
	else
	{
		PND_PD_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CPDUser::PrmBeginDone(): PD PRM BEGIN failed, rsp(%#x) rqb(%08x)", 
			CM_RQB_GET_RESPONSE(pRQB),
			pRQB );

		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmWriteDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_PD_PRM_WRITE);

	if (( CM_RQB_GET_RESPONSE(pRQB)           == CM_OK            ) &&  // Write OK ?
		( pRQB->args.pd.prm_rdwr->cm_pnio_err == CM_PNIO_ERR_NONE ))
	{
		PND_PD_TRACE_04( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CPDUser::PrmWriteDone(): PD PRM WRITE done, idx(%u) rsp(%#x) recIdx(%#x) rqb(%08x)",
			pUser->m_actPrmRecIdx,
			CM_RQB_GET_RESPONSE(pRQB),
			pRQB->args.pd.prm_rdwr->record_index,
			pRQB );

		pUser->m_actPrmRecIdx++;

		if ( pUser->m_actPrmRecIdx < pUser->m_prmRecList.size() )  // PRM records to write ?
		{
			// Write next PRM record
			PND_PARAM_PRM_RECORD_PTR_TYPE pEntry = pUser->m_prmRecList[pUser->m_actPrmRecIdx];

			pUser->PrmWrite( pEntry );
		}
		else
		{
			// Finish with PRM END
			pUser->PrmEnd();
		}
	}
	else
	{
		PND_PD_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
			"CPDUser::PrmWriteDone(): PD PRM WRITE failed, idx(%u) rsp(%#x) PNIO-ERR(%#x) recIdx(%#x) rqb(%08x)", 
			pUser->m_actPrmRecIdx,
			CM_RQB_GET_RESPONSE(pRQB),
			pRQB->args.pd.prm_rdwr->cm_pnio_err,
			pRQB->args.pd.prm_rdwr->record_index,
			pRQB );

		pUser->PrmFinished( PNIO_ERR_INTERNAL );

		// finish the current PRM sequence
		// Note: necessary on CM PD for later shutdown
		pUser->PrmEnd();
	}

	// Note: record data stored from PRM Record list, not freed here
	pRQB->args.pd.prm_rdwr->record_data = 0;
	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::PrmEndDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
	CPDUser *pUser             = static_cast<CPDUser*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_PD_PRM_END);

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

	PND_PD_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::PrmEndDone(): PD PRM END done, rsp(%u/%#x) rqb(%08x)", 
		rsp, rsp,
		pRQB );

	if ( rsp != CM_OK ) // Wait on ApplReady Event to finish ?
	{
		// Set A.R.P for this PRM sequence
		// Note: in case of not OK response, CM does not sent an ApplReady Event
		pUser->SetARP();

		// Signal PRM sequence finished
		pUser->PrmFinished( PNIO_ERR_INTERNAL );
	}

	pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CPDUser::startupDone( LSA_UINT32 pnioErr )
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	// Signal startup complete
	m_isStarted   = LSA_TRUE;
	m_isStartup   = LSA_FALSE;
	m_responseRQB = 0;

	PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::startupDone(): PD User startup complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
		m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

	pRQB->args.pnuc_internal.pnio_err = pnioErr;

	pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::shutdownDone( LSA_UINT32 pnioErr )
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStarted   = LSA_FALSE;
	m_isStartup   = LSA_FALSE;
	m_isShutdown  = LSA_FALSE;
	m_responseRQB = 0;

	PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::shutdownDone(): PD User shutdown complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
		m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

	// Signal shutdown complete
	pRQB->args.pnuc_internal.pnio_err = pnioErr;

	pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDUser::SetAlarmResource(PND_RQB_PTR_TYPE AlarmRqb)
{
   PND_ASSERT(AlarmRqb != 0);

   m_alarmResources.push_back(AlarmRqb);
}


/*----------------------------------------------------------------------------*/
PND_RQB_PTR_TYPE CPDUser::GetAlarmResource()
{
   PND_RQB_PTR_TYPE pUser = 0;

   if (!m_alarmResources.empty())
   {
      pUser = *m_alarmResources.begin();

      m_alarmResources.erase(m_alarmResources.begin());
   }

   return pUser;
}

LSA_UINT16 CPDUser::GetVendorId()
{
    return m_pdParam.vendor_id;
}

LSA_UINT16 CPDUser::GetDeviceId()
{
    return m_pdParam.device_id;
}

LSA_UINT16 CPDUser::GetInstanceId()
{
    return m_pdParam.instance;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
