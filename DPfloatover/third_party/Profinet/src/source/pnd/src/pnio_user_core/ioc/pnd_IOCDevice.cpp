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
/*  F i l e               &F: pnd_IOCDevice.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of the IOC Device class                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3202
#define PND_MODULE_ID       3202

/////////////////////////////////////////////////////////////////////////////
#include "eps_sys.h"

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

#include "cm_inc.h"
#include "cm_arcb_helper.h"

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
#include "pnd_IOCAr.h"
#include "pnd_IOCEventHandler.h"
#include "pnd_IOCDevice.h"
#include "pnd_IOCSubslot.h"
#include "pnd_IOCSlot.h"
#include "pnd_IOCUser.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CIOCDevice
/////////////////////////////////////////////////////////////////////////////

CIOCDevice::CIOCDevice( LSA_UINT16 devNr, PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent ) : CUserNode( pHandle, pParent )
{
    m_deviceNr = devNr;

    m_isAdded        = LSA_FALSE;
    m_isAllocateDone = LSA_FALSE;
    m_activate       = LSA_FALSE;
    m_initialStateActivate = LSA_FALSE;
    m_vendorID       = 0;
    m_deviceID       = 0;
    m_instance       = 0;
    m_laddr          = PND_LADDR_INVALID;
    m_eddHwType      = EDD_HW_TYPE_USED_STANDARD_MAC;
    m_pAR            = 0;
    m_pEventHandler  = 0;
    m_responseRQB    = 0;
    m_nrOfAlarmRes   = 0;
	m_DiagDeviceState.ErrorCause = 0;		//TODO
	m_DiagDeviceState.ReasonCode = 0;
	pnd_memset(m_DiagDeviceState.AdditionalInfo, 0, sizeof(m_DiagDeviceState.AdditionalInfo));
}

/*---------------------------------------------------------------------------*/
CIOCDevice::~CIOCDevice()
{
    if ( m_pEventHandler != 0 )
    {
        delete( m_pEventHandler );
        m_pEventHandler = 0;
    }

    if ( m_pAR != 0 )
    {
        delete ( m_pAR );
        m_pAR = 0;
    }

    // Free elements in API- and AR-Map
    deleteMap( m_subslotMap );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams, SubslotVector* pSubList )
{
    PND_PARAM_DEVICE_PTR_TYPE pDevParam = (PND_PARAM_DEVICE_PTR_TYPE)pParams;
    PND_ASSERT( pSubList  != 0 );
    PND_ASSERT( laddr_ptr != 0 );

    // setup internal device attributes and create the IOC subslots based on config 
    PND_ASSERT( m_pParentNode != 0 );
    PND_ASSERT(( m_pndHandle.hd_nr > 0 ) && ( m_pndHandle.hd_nr <= PSI_CFG_MAX_HD_CNT ));
    PND_ASSERT(( m_pndHandle.if_nr > 0 ) && ( m_pndHandle.if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

    m_vendorID = pDevParam->vendor_id;
    m_deviceID = pDevParam->device_id;
    m_instance = pDevParam->instance;
    m_laddr    = pDevParam->device_laddr;
    m_activate = pDevParam->is_activate;
    m_initialStateActivate = pDevParam->is_activate;

    LSA_UINT16 nrOfSub = pDevParam->nr_of_subslot;

    PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::Create(): Creating IOC device, devNr(%u),venID(%#x) devID(%#x) inst(%#x) LADDR(%u) nrOfSub(%u)", 
        m_deviceNr,
        m_vendorID,
        m_deviceID,
        m_instance,
        m_laddr,
        nrOfSub );

    // Create the event handler for all CM-CL device events (indications)
    m_pEventHandler = new CIOCEventHandler( &m_pndHandle, this );

    // Register device LADDR in addresslist if avaliable
    // Note: if any submod is device substitude the entry of device is overwrite with submod address
    if ( pDevParam->device_laddr != PND_LADDR_INVALID )
    {
        laddr_ptr->laddr_ref[pDevParam->device_laddr] = this;
        laddr_ptr->laddr_type[pDevParam->device_laddr] = IOCDEVICE;
		laddr_ptr->laddr_device_nr[pDevParam->device_laddr] = pDevParam->device_number;
		laddr_ptr->laddr_slot_nr[pDevParam->device_laddr] = 0;
		laddr_ptr->laddr_subslot_nr[pDevParam->device_laddr] = 0;
    }

    // Create the subslots from subslot list
    for ( LSA_UINT16 subIdx = 0; subIdx < pSubList->size(); subIdx++ )
    {
        PND_PARAM_SUBSLOT_PTR_TYPE pSubParam = pSubList->at( subIdx );

        // Create the subslot with keys and properties from INI and add it to Map
        CIOCSubslot* pSub = new CIOCSubslot( pSubParam->api, pSubParam->slot_nr, pSubParam->subslot_nr, &m_pndHandle, this );
        PND_ASSERT( pSub != 0 );

        // Register modul and submodul LADDR in addresslist if avaliable
        // Note: if any submod is modul substitude the entry is set to same entry
        if ( pSubParam->slot_laddr != PND_LADDR_INVALID )
        {
            if ( pSubParam->slot_laddr == pSubParam->subslot_laddr )  // submodul is modul substitude
            {
                laddr_ptr->laddr_ref[pSubParam->slot_laddr] = pSub;
                laddr_ptr->laddr_type[pSubParam->slot_laddr] = IOCSUBSLOT;
				laddr_ptr->laddr_device_nr[pSubParam->slot_laddr] = pDevParam->device_number;
				laddr_ptr->laddr_slot_nr[pSubParam->slot_laddr] = pSubParam->slot_nr;
				laddr_ptr->laddr_subslot_nr[pSubParam->slot_laddr] = pSubParam->subslot_nr;
            }
            else
            {
                PND_ASSERT( pSubParam->subslot_laddr != PND_LADDR_INVALID );

                LSA_UINT32  api     = pSubParam->api;
                LSA_UINT16  slot_nr  = pSubParam->slot_nr;

                // search if slot exists already
                CIOCSubslot *pSubObj = GetSubslot( api , slot_nr, 0 );

                if (!pSubObj)
                {
                    // if slot does not exist create a new one
                    // add pSubParam->subslot_laddr as unique number for AddNode...
                    CIOCSlot* pSlot = new CIOCSlot( pSubParam->api, pSubParam->slot_nr, pSubParam->subslot_laddr, &m_pndHandle, this );
                    PND_ASSERT( pSlot != 0 );
                    laddr_ptr->laddr_ref[pSubParam->slot_laddr] = pSlot;
                    laddr_ptr->laddr_type[pSubParam->slot_laddr] = IOCSLOT;
					laddr_ptr->laddr_device_nr[pSubParam->slot_laddr] = pDevParam->device_number;
					laddr_ptr->laddr_slot_nr[pSubParam->slot_laddr] = pSubParam->slot_nr;
					laddr_ptr->laddr_subslot_nr[pSubParam->slot_laddr] = pSubParam->subslot_nr;

                    pSlot->Create(pSubParam);
                    AddNode( m_subslotMap, pSlot );
                }
                else
                {
                    laddr_ptr->laddr_ref[pSubParam->slot_laddr] = pSubObj;
                    laddr_ptr->laddr_type[pSubParam->slot_laddr] = IOCSLOT;
					laddr_ptr->laddr_device_nr[pSubParam->slot_laddr] = pDevParam->device_number;
					laddr_ptr->laddr_slot_nr[pSubParam->slot_laddr] = pSubParam->slot_nr;
					laddr_ptr->laddr_subslot_nr[pSubParam->slot_laddr] = pSubParam->subslot_nr;
                }

                laddr_ptr->laddr_ref[pSubParam->subslot_laddr] = pSub;
                laddr_ptr->laddr_type[pSubParam->subslot_laddr] = IOCSUBSLOT;
				laddr_ptr->laddr_device_nr[pSubParam->subslot_laddr] = pDevParam->device_number;
				laddr_ptr->laddr_slot_nr[pSubParam->subslot_laddr] = pSubParam->slot_nr;
				laddr_ptr->laddr_subslot_nr[pSubParam->subslot_laddr] = pSubParam->subslot_nr;
            }
        }
        else
        {
            PND_ASSERT( pSubParam->subslot_laddr != PND_LADDR_INVALID );

            laddr_ptr->laddr_ref[pSubParam->subslot_laddr] = pSub;
            laddr_ptr->laddr_type[pSubParam->subslot_laddr] = IOCSUBSLOT;
			laddr_ptr->laddr_device_nr[pSubParam->subslot_laddr] = pDevParam->device_number;
			laddr_ptr->laddr_slot_nr[pSubParam->subslot_laddr] = pSubParam->slot_nr;
			laddr_ptr->laddr_subslot_nr[pSubParam->subslot_laddr] = pSubParam->subslot_nr;
        }

        pSub->Create( pSubParam );
        AddNode( m_subslotMap, pSub );
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

CIOCAr* CIOCDevice::GetAr()
{
    return ( m_pAR );
}

/*---------------------------------------------------------------------------*/
CIOCSubslot* CIOCDevice::GetSubslot( LSA_UINT32 apiNr, LSA_UINT16 slotNr, LSA_UINT16 subNr )
{
    string key = CIOCSubslot::GetKey( apiNr, slotNr, subNr );
    CIOCSubslot* pSub = dynamic_cast<CIOCSubslot*>( getSubNode( m_subslotMap,key ));

    return ( pSub );
}

/*---------------------------------------------------------------------------*/
LSA_INT CIOCDevice::GetSubslots( UserNodeVector &list )
{
    // Add all stored subslots from shadow real cfg tree
    getNodeList( m_subslotMap, list );

    return ( list.size() );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::SetEddHwType( LSA_UINT32 hwType )
{
    // Set the correct EDD HW type (used by CM ARCB functions for offset calculation)
    // Note: must be set before Add
    m_eddHwType = hwType;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::ResetSubslots( LSA_UINT16 state )
{
    UserNodeVector tmpList;

    LSA_INT count = getNodeList( m_subslotMap, tmpList );

    // Reset all real cfg tree subslots to submod state (i.E.: to OK)
    for ( LSA_INT idx = 0; idx < count; idx++ )
    {
        CIOCSubslot* subObj = dynamic_cast<CIOCSubslot*>( tmpList[idx] );

        if ( subObj != 0 )
        {
            // Reset BGZD and consumer to OK
            subObj->SetState( state );
        }
    }

    // Reset the BGZD Info states
    m_bgzdStore.Reset();
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CIOCDevice::GetDeviceState()
{
	return ( (LSA_BOOL) m_activate );
}

LSA_VOID CIOCDevice::SetInitialDeviceState()
{
    m_activate = m_initialStateActivate;
}

LSA_VOID CIOCDevice::GetDeviceDiagState(LSA_UINT16 *error_cause, LSA_UINT8 *reason_code, LSA_UINT8 *additional_info)
{
    PND_ASSERT( additional_info );

	*error_cause = m_DiagDeviceState.ErrorCause;
	*reason_code = m_DiagDeviceState.ReasonCode;
	pnd_memcpy(additional_info , m_DiagDeviceState.AdditionalInfo, sizeof(m_DiagDeviceState.AdditionalInfo));
}

LSA_VOID CIOCDevice::SetDeviceDiagState(LSA_UINT16 error_cause, LSA_UINT8 reason_code, LSA_UINT8 *additional_info)
{
	m_DiagDeviceState.ErrorCause = error_cause;
	m_DiagDeviceState.ReasonCode = reason_code;

	if(additional_info == LSA_NULL)
	{
		// This is a reset case
		pnd_memset(m_DiagDeviceState.AdditionalInfo, 0, sizeof(m_DiagDeviceState.AdditionalInfo));
	}
	else
	{
		pnd_memcpy(m_DiagDeviceState.AdditionalInfo, additional_info, sizeof(m_DiagDeviceState.AdditionalInfo));
	}
	
}
/*---------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::UpdateFromDiffList( LSA_VOID_PTR_TYPE diffList )
{
    CM_UPPER_CL_DIFF_AP_PTR_TYPE         pAp;
    CM_UPPER_CL_DIFF_MODULE_PTR_TYPE     pMod;
    CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE  pSub;

    CM_LIST_ENTRY_PTR_TYPE pList = (CM_LIST_ENTRY_PTR_TYPE)diffList;
    PND_ASSERT( pList != 0 );

    if ( !CmListIsEmpty (pList) ) // Empty list ?
    {
        PND_IOC_TRACE_00(  m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, 
            "CIOCDevice::UpdateFromDiffList(): DIFF-LIST" );

        for ( CmListForeach( pAp, pList, CM_UPPER_CL_DIFF_AP_PTR_TYPE ))
        {
            PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, 
                "CIOCDevice::UpdateFromDiffList():  api(%u)", pAp->api );

            for ( CmListForeach( pMod, &pAp->module_list, CM_UPPER_CL_DIFF_MODULE_PTR_TYPE ))
            {
                PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, 
                    "CIOCDevice::UpdateFromDiffList():    slot_nr(%u) mod_ident(%u/%#x)", 
                    pMod->slot_nr, pMod->mod_ident, pMod->mod_ident );

                for ( CmListForeach( pSub, &pMod->submodule_list, CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE ))
                {
                    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, 
                        "CIOCDevice::UpdateFromDiffList():      subslot_nr(%u/0x%04x) sub_ident(%u/%#x) sub_state(0x%04x)",
                        pSub->subslot_nr, pSub->subslot_nr,
                        pSub->sub_ident,  pSub->sub_ident,
                        pSub->sub_state );

                    // Update BGZD for subslot
                    CIOCSubslot *pSubObj = GetSubslot( pAp->api, pMod->slot_nr, pSub->subslot_nr );
                    PND_ASSERT( pSubObj != 0 );

                    pSubObj->SetState( pSub->sub_state );
                }
            }
        }
    }
    else
    {
        PND_IOC_TRACE_00( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, 
            "CIOCDevice::UpdateFromDiffList(): DIFF-LIST is empty" );
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::UpdateForPullModul( LSA_UINT32 api, LSA_UINT16 slot )
{
    UserNodeVector tmpList;

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
        "CIOCDevice::UpdateForPullModul(): Update diag states for modul, dev/api/sl(%#x/%#x/%#x)",
        GetNumber(), api, slot );

    // set all subslots with key api/slot to pulled
    // Note: All subsmods with faulty state are reset to pulled(see CM_SUB_STATE_PULLED)
    LSA_INT count = getNodeList( m_subslotMap, tmpList );

    for ( LSA_INT idx = 0; idx < count; idx++ )
    {
        CIOCSubslot* subObj = dynamic_cast<CIOCSubslot*>(tmpList[idx]);

        if (( subObj             !=  0   ) &&  // Exists ?
            ( subObj->GetApiNr()  == api ) &&  // API matches ?
            ( subObj->GetSlotNr() == slot ))   // Slot matches ?
        {
            subObj->SetState( CM_SUB_STATE_PULLED );
        }
    }

}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::UpdateFromDiagAlarm( LSA_VOID_PTR_TYPE alarmPtr )
{
    CM_UPPER_ALARM_PTR_TYPE pAlarm = (CM_UPPER_ALARM_PTR_TYPE)alarmPtr;
    PND_ASSERT( pAlarm != 0 );

    PND_IOC_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
        "CIOCDevice::UpdateFromDiagAlarm(): Update diag state from alarm, dev/api/sl/su(%u/%u/%u/%#x) haveDiag(%u) maint(%#x) alarm-type(%#x)",
        GetNumber(), pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr,
        pAlarm->diag_submod_available, pAlarm->maintenance_status,
        pAlarm->alarm_type );

    // Get the submod from real config and update the BGZD state for receiced diagnosis
    CIOCSubslot *pSub = GetSubslot( pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr );

    if ( pSub != 0 )
    {
        pSub->UpdateFromAlarm( pAlarm->diag_submod_available, pAlarm->maintenance_status );
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::UpdateForSubmodAlarm( LSA_VOID_PTR_TYPE alarmPtr )
{
    CM_UPPER_ALARM_PTR_TYPE pAlarm = (CM_UPPER_ALARM_PTR_TYPE)alarmPtr;
    PND_ASSERT( pAlarm != 0 );

    PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE_LOW,
        "CIOCDevice::UpdateForSubmodAlarm(): Set submod state dev/api/sl/su(%#x/%#x/%#x/%#x) for alarm type(%u/%#x)",
        m_deviceNr, pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr,
        pAlarm->alarm_type, pAlarm->alarm_type );

    // Get the submod from real config, and set the new BGZD state
    CIOCSubslot *pSub = GetSubslot( pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr );

    if ( pSub != 0 ) // submod exists ?
    {
        switch ( pAlarm->alarm_type )
        {
        case CM_ALARM_TYPE_PULL: // Pull submod
            {
                pSub->SetState( CM_SUB_STATE_PULLED );
            }
            break;
        case CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR: // Lock submod
            {
                pSub->SetState( (CM_SUB_STATE_GOOD | CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED));
            }
            break;
        case CM_ALARM_TYPE_PLUG_WRONG:  // Set to plugged wrong
            {
                pSub->SetState( CM_SUB_STATE_WRONG );
            }
            break;
        default:
            break;
        }
    }
    else
    {
        // Handle special usecase pull module over ALARM_PULL with subslot==0
        if (( pAlarm->alarm_type == CM_ALARM_TYPE_PULL ) &&  // PULL alarm
            ( pAlarm->subslot_nr == 0 ))                     // sublot 0
        {
            UpdateForPullModul( pAlarm->api, pAlarm->slot_nr );
        }
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::Startup( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pRQB != 0);
    PND_ASSERT( m_responseRQB == 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isStartup  = LSA_TRUE;
    m_isShutdown = LSA_FALSE;

    // Store the upper RQB, and Startup with allocate buffer (IODU request) for add device
    m_responseRQB = pRQB;

    AllocateBuffer();
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::Shutdown( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pRQB != 0);
    PND_ASSERT( m_responseRQB == 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isShutdown = LSA_TRUE;
    m_isStartup  = LSA_FALSE;

    // Store the upper RQB, start shutdown CM CL user till closing the PNIO channel
    m_responseRQB = pRQB;

    if ( m_isAdded ) // Device added (CM-CL)
    {
        // Remove device first
        Remove();
    }
    else
    {
        if ( m_isAllocateDone ) // PI Buffer allocated
        {
            // Free PI buffer first
            FreeBuffer();
        }
        else
        {
            // Trigger shutdown complete
            shutdownDone( PNIO_OK );
        }
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::ResetBgzd()
{
    // Reset own BGZD state
    m_bgzdStore.Reset();

    // Reset BGZD state of all submods
    UserNodeVector subList;

    // Set all submods to good, for new IND
    LSA_INT count = getNodeList( m_subslotMap, subList );

    for ( LSA_INT idx = 0; idx < count; idx++ )
    {
        CIOCSubslot *pSub = (CIOCSubslot *)subList[idx];
        PND_ASSERT( pSub != 0 );

        pSub->SetState( CM_SUB_STATE_GOOD );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::UpdateBgzd( CBgzdInfoStore& bgzdInfo )
{
    // Note: only merge the BGZD if device is added
    if ( m_isAdded ) // Device added (CM-CL)
    {
        // Merge the own state with BGZD from Args
        // Note: called by parent to set the summary state of all childs
        m_bgzdStore.UpdateBgzdInfo( bgzdInfo );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::StartUpdateBgzd()
{
    // Note: in case of shutdwon, BGZD update is not done
    if ( !m_isShutdown ) // Shutdown in progress
    {
        UserNodeVector objList;

        m_bgzdStore.Reset();

        // Update the own BGZD from BGZD of all IOC subslots (childs) as summary
        LSA_INT count = getNodeList( m_subslotMap, objList );

        for ( LSA_INT sub = 0; sub < count; sub++ )
        {
            CIOCSubslot *pSub = dynamic_cast<CIOCSubslot*>( objList[sub] );

            if ( pSub != 0 )
            {
                pSub->UpdateBgzd( m_bgzdStore );
            }
        }

        // Update the own BGZD (user part) from actual AR state
        if ( m_pAR != 0 )
        {
            updateArState( m_pAR->GetArState());
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::SetMode( PND_RQB_PTR_TYPE p_rb, LSA_BOOL activate )
{
    PND_RQB_PTR_TYPE pPndRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pPndRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pPndRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    if ( activate )
    {
        // note: mode change from Offline to Clear or Operate
        if ( m_activate ) // marked for activating
        {
            LSA_HANDLE_TYPE handle = GetPnioHandle();

            CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
            PND_ASSERT(pRQB);

            PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                "CIOCDevice::SetMode(): CM-CL Activate.REQ, nr(%u) pRQB(0x%08x)", 
                m_deviceNr, pRQB );

            pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_ACTIVATE;
            pRQB->args.cl.dev_control->device_nr   = m_deviceNr;

            LSA_RQB_SET_USER_ID_PTR( pRQB, pPndRQB );

            eps_user_request_lower( pRQB, this, CIOCDevice::SetModeDone );
        }
        else
        {
            PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                "CIOCDevice::SetMode(): IOC Device set to not activate, devNr(%u) hdNr(%u) ifNr(%u)",
                m_deviceNr, m_pndHandle.hd_nr, m_pndHandle.if_nr);

            pPndRQB->args.pnuc_internal.pnio_err  = PNIO_OK;

            pnd_pnio_user_request_local_done( pPndRQB );
        }
    }
    else
    {
        // note: mode change from Clear operate to OFFLINE
        if ( m_activate ) // marked as activating
        {
            LSA_HANDLE_TYPE handle = GetPnioHandle();

            CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
            PND_ASSERT(pRQB);

            PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                "CIOCDevice::SetMode(): CM-CL Passivate.REQ, nr(%u) pRQB(0x%08x)", 
                m_deviceNr, pRQB );

            pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_PASSIVATE;
            pRQB->args.cl.dev_control->device_nr   = m_deviceNr;

            LSA_RQB_SET_USER_ID_PTR( pRQB, pPndRQB );

            eps_user_request_lower( pRQB, this, CIOCDevice::SetModeDone );
        }
        else
        {
            PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                "CIOCDevice::SetMode(): IOC Device already passivated, devNr(%u) hdNr(%u) ifNr(%u)",
                m_deviceNr, m_pndHandle.hd_nr, m_pndHandle.if_nr );

            pPndRQB->args.pnuc_internal.pnio_err  = PNIO_OK;

            pnd_pnio_user_request_local_done( pPndRQB );
        }
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

string CIOCDevice::GetKey()
{
    CValueHelper valObj;

    string result = valObj.UINT16ToString( m_deviceNr );

    return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_UINT CIOCDevice::GetNumber()
{
    return ( (LSA_UINT)m_deviceNr );
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CIOCDevice::GetPnioHandle()
{
    PND_ASSERT( m_pParentNode != 0 );
    LSA_HANDLE_TYPE handle = m_pParentNode->GetPnioHandle();

    return ( handle );
}

LSA_UINT16 CIOCDevice::GetVendorID()
{
    return ( m_vendorID );
}

LSA_UINT16 CIOCDevice::GetDeviceID()
{
    return ( m_deviceID );
}

LSA_UINT16 CIOCDevice::GetInstanceID()
{
    return ( m_instance );
}

LSA_VOID CIOCDevice::SetStatusActivate()
{
    m_activate = 1;
}

LSA_VOID CIOCDevice::SetStatusPassivate()
{
    m_activate = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::AllocateBuffer()
{
    LSA_HANDLE_TYPE handle = GetPnioHandle();

    // Prepare CM RQB with created ARCB 
    // Note: used for IOCR config transfered to IODU and complete Adding
    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_ADD, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd = pRQB->args.cl.dev_add;
    PND_ASSERT( pAdd != 0 );

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::AllocateBuffer(): Start adding device, devNr(%d) EDD HW Type(%#x)", 
        m_deviceNr, m_eddHwType );

    // get the ARCB settings from param storage  (see CreateArcbAddCL)
    CParamStore *paramObj = CParamStore::GetInstance();
    PND_ASSERT( paramObj != 0 );

    LSA_UINT32 pnioRes = paramObj->CreateArcbAddCL( 
        &m_pndHandle,
        m_deviceNr,
        m_eddHwType,
        pAdd );

    if ( pnioRes == PNIO_OK ) // ARCB creation OK ?
    {
        UserNodeVector consumerSublist;
        UserNodeVector providerSublist;

        // create IODU RQB
        PND_RQB_PTR_TYPE pRQBIodu = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
        PND_ASSERT( pRQBIodu != 0 );

        LSA_RQB_SET_OPCODE( pRQBIodu, PND_OPC_PI_ALLOC );

        pRQBIodu->args.pi_alloc.pnd_handle = this->m_pndHandle;

        pRQBIodu->args.pi_alloc.device_nr = m_deviceNr;

        pRQBIodu->args.pi_alloc.pnio_err = PNIO_OK;

        PND_PI_ALLOC_PTR_TYPE pIODU = &(pRQBIodu->args.pi_alloc);

        // create AR, IODU PI settings and subslot list from ARCB configuration
        PND_ASSERT( m_pAR == 0 );
        m_pAR = createArFromARCB( pAdd, pIODU, providerSublist, consumerSublist );

        // create IODU address lists for provider (outputs) and consumer (inputs) based on subslot lists
        pIODU->provider_buffer.number_of_IO_addr = (LSA_UINT32)providerSublist.size();
        pIODU->provider_buffer.IO_addr_array = createAddressList( providerSublist, LSA_TRUE );

        // create IODU address mapping lists for consumer (inputs) based on subslot list
        pIODU->consumer_buffer.number_of_IO_addr = (LSA_UINT32)consumerSublist.size();
        pIODU->consumer_buffer.IO_addr_array = createAddressList( consumerSublist, LSA_FALSE );

        // save CM RQB in user ID
        // Note: used as session data for finish CM device adding
        LSA_RQB_SET_USER_ID_PTR( pRQBIodu, pRQB );

        // start IODU request for PI allocation
        pnd_pnio_user_request_iobc( pRQBIodu, this, CIOCDevice::AllocateBufferDone );
    }
    else
    {
        // stop startup with error
        startupDone( PNIO_ERR_INTERNAL );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::FreeBuffer()
{
    PND_ASSERT( m_pAR != 0 );

    // create IODU RQB
    PND_RQB_PTR_TYPE pRQBIodu = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
    PND_ASSERT( pRQBIodu != 0 );
    
    pRQBIodu->args.pi_free.consumer_buffer_handle = m_pAR->GetConsumerHandle();
    pRQBIodu->args.pi_free.provider_buffer_handle = m_pAR->GetProviderHandle();
    pRQBIodu->args.pi_free.pnio_err               = PNIO_OK;

    LSA_RQB_SET_OPCODE( pRQBIodu, PND_OPC_PI_FREE );

    pRQBIodu->args.pi_free.pnd_handle = this->m_pndHandle;

    // start IODU request for PI allocation
    pnd_pnio_user_request_iobc( pRQBIodu, this, CIOCDevice::FreeBufferDone );
}

/////////////////////////////////////////////////////////////////////////////
// Requests
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCDevice::Add( LSA_VOID* p_rb )
{
    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)p_rb;
    PND_ASSERT( pRQB );

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::Add(): Add device, devNr(%d) EDD HW Type(%#x) pRQB(0x%08x)", 
        m_deviceNr, m_eddHwType, pRQB );

    eps_user_request_lower( pRQB, this, CIOCDevice::AddDone );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::Remove()
{
    LSA_HANDLE_TYPE handle = GetPnioHandle();

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_REMOVE, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::Remove(): CM-CL DeviceRemove.REQ, nr(%u) pRQB(0x%08x)", 
        m_deviceNr, pRQB );

    pRQB->args.cl.dev_remove->device_nr = m_deviceNr;

    eps_user_request_lower( pRQB, this, CIOCDevice::RemoveDone );
}


/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::DeviceActivate( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT(p_rb != 0);

    CIOCUser* ParentIoc = CIOCDevice::getIoc();
    PND_IOC_MODE_TYPE ParentMode = ParentIoc->GetMode();

    if (ParentMode != IOC_OFFLINE)
    {
        PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;
        switch (pActivate->DevActMode)
        {
            case PNIO_DA_TRUE: 
                Activate(p_rb);
                break;
            case PNIO_DA_FALSE:
                Passivate(p_rb);
                break;
            default:
                PND_FATAL("Device Activateunknowen command");
                break;
        }
    }
    else
    {
        PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;
        switch (pActivate->DevActMode)
        {
            case PNIO_DA_TRUE: 
                SetStatusActivate();
                break;
            case PNIO_DA_FALSE:
                SetStatusPassivate();
                break;
            default:
                PND_FATAL("Device Activateunknowen command");
                break;
        }

        pnd_pnio_user_request_iobc_done( p_rb );
    }
}

/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) Request functions                                               */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::Activate(PND_RQB_PTR_TYPE p_rb)
{
    PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;

    LSA_HANDLE_TYPE handle = GetPnioHandle();

    PND_ASSERT(pActivate->DevActMode == PNIO_DA_TRUE);

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::Activate(): CM-CL DeviceActive.REQ, nr(%u) pRQB(0x%08x)", 
        m_deviceNr, pRQB );

    pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_ACTIVATE;
    pRQB->args.cl.dev_control->device_nr   = m_deviceNr;

    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCDevice::ActivateDone );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::Passivate(PND_RQB_PTR_TYPE p_rb)
{
    PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;
    LSA_HANDLE_TYPE handle = GetPnioHandle();

    PND_ASSERT(pActivate->DevActMode == PNIO_DA_FALSE);

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::Passivate(): CM-CL DevicePassivate.REQ, nr(%u) pRQB(0x%08x)", 
        m_deviceNr, pRQB );

    pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_PASSIVATE;
    pRQB->args.cl.dev_control->device_nr   = m_deviceNr;

    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCDevice::PassivateDone );
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCDevice::AddDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCDevice *pDevice        = static_cast<CIOCDevice*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pDevice != 0);
    PND_ASSERT( pRQB    != 0);

    CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd = pRQB->args.cl.dev_add;
    PND_ASSERT( pAdd != 0);
    PND_ASSERT( pAdd->device_nr == pDevice->m_deviceNr );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_DEVICE_ADD );

    PND_IOC_TRACE_05( pDevice->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::AddDone(): CM-CL DeviceADD.CNF, rsp(%u/%#x) nr(%u) pnio_status(%#x) pRQB(0x%08x)",
        CM_RQB_GET_RESPONSE(pRQB), CM_RQB_GET_RESPONSE(pRQB),
        pRQB->args.cl.dev_add->device_nr, 
        pAdd->pnio_status,
        pRQB );

    if ( CM_RQB_GET_RESPONSE(pRQB) == CM_OK )
    {
        pDevice->m_isAdded = LSA_TRUE;

        // Provide event resources
        PND_ASSERT( pDevice->m_pEventHandler != 0 );
        pDevice->m_pEventHandler->StartDeviceListening( pAdd->nr_of_event_resources, pAdd->nr_of_alarm_resources );

        // Save count of alarm, event resources for user alarm pool
        // Note: indicate with startup done in internal RQB
        pDevice->m_nrOfAlarmRes = pAdd->nr_of_event_resources + pAdd->nr_of_alarm_resources;

        // Initialize the submod state (for update all)
        pDevice->ResetSubslots( CM_SUB_STATE_GOOD );

        // Device is added with state passivated
        pDevice->m_pAR->SetArState( PND_IOC_AR_STATE_PASSIVE );

        // Finish startup with OK
        pDevice->startupDone( PNIO_OK );
    }
    else
    {
        // stop startup with error
        pDevice->startupDone( PNIO_ERR_INTERNAL );
    }

    // Free ARCB and CM RQB
    // Note: free of ARCB is done by ParamStore (creator of ARCB)
    CParamStore *paramObj = CParamStore::GetInstance();
    PND_ASSERT( paramObj != 0 );

    paramObj->DeleteArcb( &pDevice->m_pndHandle, pRQB->args.cl.dev_add );

    pnd_cm_free_rqb( pRQB, &pDevice->m_pndHandle );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::RemoveDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCDevice *pDevice        = static_cast<CIOCDevice*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pDevice != 0);
    PND_ASSERT( pRQB    != 0);
    PND_ASSERT( pRQB->args.cl.dev_remove->device_nr == pDevice->m_deviceNr );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_DEVICE_REMOVE );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pDevice->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::RemoveDone(): CM-CL DeviceRemove.CNF, rsp(%u/%#x) nr(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr, 
        pRQB );

    if ( rsp == CM_OK )
    {
        if ( pDevice->m_isShutdown ) // shutdown in progress ?
        {
            // continue with Free PI buffer (IODU request)
            pDevice->FreeBuffer();
        }
    }
    else
    {
        // stop shutdown with fatal
        PND_FATAL( "CIOCDevice::RemoveDone: unexpected CM CL response received" );
    }

    pnd_cm_free_rqb( pRQB, &pDevice->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) callback functions                                              */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::ActivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCDevice *pDevice        = static_cast<CIOCDevice*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pDevice != 0 );
    PND_ASSERT( pRQB    != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)                == CM_OPC_CL_DEVICE_CONTROL );
    PND_ASSERT( pRQB->args.cl.dev_control->device_nr   == pDevice->m_deviceNr );
    PND_ASSERT( pRQB->args.cl.dev_control->control_tag == CM_CL_DEVICE_CONTROL_ACTIVATE );

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

    LSA_RESPONSE_TYPE  rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pDevice->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::ActivateDone(): CM-CL DeviceActive.CNF, rsp(%u/%#x) nr(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr, 
        pRQB );

    if ( rsp == CM_OK )
    {
        // set BGZD to not passivated 
        // Note: necessary because it is possible, that indication is received late
        CIOCAr *pAr = pDevice->GetAr();
        PND_ASSERT ( pAr != 0 );

        if (( pAr->GetArState() == PND_IOC_AR_STATE_PASSIVE ) || // From Passive state
            ( pAr->GetArState() == PND_IOC_AR_STATE_UNKNOWN ))   // After Add device
        {
            // Initialize the submod and AR state (BGZD)
            pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
            pDevice->updateArState( PND_IOC_AR_STATE_START );
        }
        
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_OK;
        pDevice->SetStatusActivate();
    }
    else
    {
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_iobc_done( pPndRQB );

    pnd_cm_free_rqb( pRQB, &pDevice->m_pndHandle );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::PassivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCDevice *pDevice        = static_cast<CIOCDevice*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pDevice != 0 );
    PND_ASSERT( pRQB    != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)                == CM_OPC_CL_DEVICE_CONTROL );
    PND_ASSERT( pRQB->args.cl.dev_control->device_nr   == pDevice->m_deviceNr );
    PND_ASSERT( pRQB->args.cl.dev_control->control_tag == CM_CL_DEVICE_CONTROL_PASSIVATE );

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );
    
    LSA_RESPONSE_TYPE  rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pDevice->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::PassivateDone(): CM-CL DevicePassivate.CNF, rsp(%u/%#x) nr(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr, 
        pRQB );

    if ( rsp == CM_OK )
    {
        // Initialize the submod state (for update all)
        pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
        pDevice->SetStatusPassivate();

        // Set AR state to passivated (used by BGZD-check)
        // Note: necessary because it is possible, that no indication is provided by CM
        CIOCAr *pAr = pDevice->GetAr();
        PND_ASSERT ( pAr != 0 );

        pAr->SetArState( PND_IOC_AR_STATE_PASSIVE );

        pPndRQB->args.iob_dev_act.pnio_err = PNIO_OK;
    }
    else
    {
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_iobc_done( pPndRQB );

    pnd_cm_free_rqb( pRQB, &pDevice->m_pndHandle );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::SetModeDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCDevice *pDevice        = static_cast<CIOCDevice*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pDevice != 0 );
    PND_ASSERT( pRQB    != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)                == CM_OPC_CL_DEVICE_CONTROL );
    PND_ASSERT( pRQB->args.cl.dev_control->device_nr   == pDevice->m_deviceNr );

    LSA_UINT16 controlTag = pRQB->args.cl.dev_control->control_tag;

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );
    
    LSA_RESPONSE_TYPE  rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_05( pDevice->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::SetModeDone(): CM-CL DeviceControl.CNF, rsp(%u/%#x) nr(%u) tag(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr,
        controlTag,
        pRQB );

    if ( rsp == CM_OK )
    {
        CIOCAr *pAr = pDevice->GetAr();
        PND_ASSERT ( pAr != 0 );

        if ( controlTag == CM_CL_DEVICE_CONTROL_PASSIVATE )
        {
            // Initialize the submod state (for update all)
            pDevice->ResetSubslots( CM_SUB_STATE_GOOD );

            // Set AR state to passivated (used by BGZD-check)
            // Note: necessary because it is possible, that no indication is provided by CM
            pAr->SetArState( PND_IOC_AR_STATE_PASSIVE );
        }
        else
        {
            if (( pAr->GetArState() == PND_IOC_AR_STATE_PASSIVE ) || // From Passive state
                ( pAr->GetArState() == PND_IOC_AR_STATE_UNKNOWN ))   // After Add device
            {
                // Initialize the submod and AR state (BGZD)
                pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
                pDevice->updateArState( PND_IOC_AR_STATE_START );
            }
        }

        pPndRQB->args.iob_dev_act.pnio_err = PNIO_OK;
    }
    else
    {
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_local_done( pPndRQB );

    pnd_cm_free_rqb( pRQB, &pDevice->m_pndHandle );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID CIOCDevice::AllocateBufferDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCDevice *pDevice = static_cast<CIOCDevice*>(p_inst);
    PND_ASSERT( pDevice != 0 );

    PND_RQB_PTR_TYPE pRQBIodu = static_cast<PND_RQB_PTR_TYPE>(p_rb);
    PND_ASSERT( pRQBIodu != 0 );

    // restore CM RQB from user_id
    CM_UPPER_RQB_PTR_TYPE pRQBCm = (CM_UPPER_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQBIodu );
    PND_ASSERT( pRQBCm != 0 );

    LSA_UINT32 pnioErr = pRQBIodu->args.pi_alloc.pnio_err;

    if ( pnioErr == PNIO_OK ) // Alloc OK ?
    {
        PND_ASSERT( pDevice->m_pAR != 0 );

        pDevice->m_isAllocateDone = LSA_TRUE;

        // update IOCR EDD properties using ARCB
        pDevice->m_pAR->UpdateImageProperties( &(pRQBIodu->args.pi_alloc));

        // continue with CM adding device
        pDevice->Add( pRQBCm );
    }
    else
    {
        // Free ARCB and CM RQB
        // Note: free of ARCB is done by ParamStore (creator of ARCB)
        CParamStore *paramObj = CParamStore::GetInstance();
        PND_ASSERT( paramObj != 0 );

        paramObj->DeleteArcb( &pDevice->m_pndHandle, pRQBCm->args.cl.dev_add );

        pnd_cm_free_rqb( pRQBCm, &pDevice->m_pndHandle );

        // stop startup with error
        pDevice->startupDone( pnioErr );
    }

    // free IODU RQB
    if ( pRQBIodu->args.pi_alloc.consumer_buffer.IO_addr_array != 0 )
    {
        pnd_mem_free( pRQBIodu->args.pi_alloc.consumer_buffer.IO_addr_array );
        pRQBIodu->args.pi_alloc.consumer_buffer.IO_addr_array = 0;
    }

    if ( pRQBIodu->args.pi_alloc.provider_buffer.IO_addr_array != 0 )
    {
        pnd_mem_free( pRQBIodu->args.pi_alloc.provider_buffer.IO_addr_array );
        pRQBIodu->args.pi_alloc.provider_buffer.IO_addr_array = 0;
    }

    pnd_mem_free( pRQBIodu );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::FreeBufferDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCDevice *pDevice = static_cast<CIOCDevice*>(p_inst);
    PND_ASSERT( pDevice != 0 );

    PND_RQB_PTR_TYPE pRQBIodu = static_cast<PND_RQB_PTR_TYPE>(p_rb);
    PND_ASSERT( pRQBIodu != 0 );

    LSA_UINT32 pnioErr = pRQBIodu->args.pi_free.pnio_err;

    if ( pnioErr == PNIO_OK ) // Free OK ?
    {
        // Finish wiht OK
        pDevice->shutdownDone( PNIO_OK );
    }
    else
    {
        // stop shutdown with fatal
        PND_FATAL( "CIOCDevice::FreeBufferDone(): unexpected IODU response received" );
    }

    pnd_mem_free( pRQBIodu );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

CIOCUser* CIOCDevice::getIoc()
{
    CIOCUser* pIoc = dynamic_cast<CIOCUser*>( GetParent() );
    PND_ASSERT( pIoc != 0 );

    return ( pIoc );
}

LSA_VOID CIOCDevice::startupDone( LSA_UINT32 pnioErr )
{
    PND_RQB_PTR_TYPE pRQB = m_responseRQB;
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    // Signal startup complete
    m_isStarted   = LSA_TRUE;
    m_isStartup   = LSA_FALSE;
    m_responseRQB = 0;

    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::startupDone(): IOC Device startup complete, devNr(%u) hdNr(%u) ifNr(%u) pnio_res(%#x) alarmCount(%u)",
        m_deviceNr, m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr, m_nrOfAlarmRes );

    pRQB->args.pnuc_internal.pnio_err        = pnioErr;
    pRQB->args.pnuc_internal.nr_of_alarm_ind = m_nrOfAlarmRes;

    pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::shutdownDone( LSA_UINT32 pnioErr )
{
    PND_RQB_PTR_TYPE pRQB = m_responseRQB;
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isStarted   = LSA_FALSE;
    m_isStartup   = LSA_FALSE;
    m_isShutdown  = LSA_FALSE;
    m_responseRQB = 0;

    PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCDevice::shutdownDone(): IOC Device shutdown complete devNr(%u) hdNr(%u) ifNr(%u) pnio_res(%#x)",
        m_deviceNr, m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

    // Signal shutdown complete
    pRQB->args.pnuc_internal.pnio_err = pnioErr;

    pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCDevice::updateArState( PND_IOC_AR_STATE_TYPE state )
{
    if ( m_pAR != 0 )  // AR created ?
    {
        // update state of AR
        m_pAR->SetArState( state );
    }

    // Update IOD device BGZD
    switch ( state )
    {
    case PND_IOC_AR_STATE_OFFLINE:
    case PND_IOC_AR_STATE_PASSIVE:
        {
            // Notify OFFLINE reached
            // Note: used by scripting to dectect state change to OFFLINE
            LSA_BOOL notIndata  = ( state == PND_IOC_AR_STATE_OFFLINE ) ? LSA_TRUE : LSA_FALSE;
            LSA_BOOL passivated = ( state == PND_IOC_AR_STATE_PASSIVE ) ? LSA_TRUE : LSA_FALSE;

            m_bgzdStore.SetArStateIOC( notIndata, passivated );
        }
        break;
    case PND_IOC_AR_STATE_START:
    case PND_IOC_AR_STATE_PREDATA:
        {
            // Set AR BGZD to not INDATA
            m_bgzdStore.SetArStateIOC( LSA_TRUE, LSA_FALSE );
        }
        break;
    case PND_IOC_AR_STATE_INDATA: // INDATA
    default:  // Initial state
        {
            // Set AR BGZD to OK
            m_bgzdStore.SetArStateIOC( LSA_FALSE, LSA_FALSE );
        }
        break;
    }
}

/*----------------------------------------------------------------------------*/
CIOCAr* CIOCDevice::createArFromARCB( LSA_VOID* p_cm, LSA_VOID* p_iodu, UserNodeVector& prov, UserNodeVector& cons )
{
    LSA_UINT32 arCount;
    CM_UPPER_ARCB_ADD_PTR_TYPE pARCB;

    CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pADD = (CM_UPPER_CL_DEVICE_ADD_PTR_TYPE)p_cm;
    PND_ASSERT( pADD != 0 );

    PND_PI_ALLOC_PTR_TYPE pIODU = (PND_PI_ALLOC_PTR_TYPE)p_iodu;
    PND_ASSERT( pIODU != 0 );

    CIOCAr *pAR = 0;

    // Check supported ARs
    // Note: only one AR is supported for each device
    CmListLength (&pADD->ar_list, &arCount );
    PND_ASSERT( arCount == 1 );

    // Get first ARCB
    for ( CmListForeach( pARCB, &pADD->ar_list, CM_UPPER_ARCB_ADD_PTR_TYPE ))
    {
        // Create AR from ARCB configuration
        pAR = new CIOCAr( &m_pndHandle, pARCB );
        PND_ASSERT( pAR != 0 );

        CM_UPPER_ARCB_IOCR_PTR_TYPE pIOCR;

        // Create the IOD setting based on IOCR info
        for( CmListForeach( pIOCR, &pARCB->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE ))
        {
            // create an scan IOCR to set the PI properties for input, output in IODU request
            // Note: the subslot list for provider, consumer includes all IOData submods
            //       dataless only included if IOXS is not discarded
            LSA_BOOL isProvider = pAR->CreateIOCR( pIOCR, pIODU );

            CM_UPPER_ARCB_IOAPI_PTR_TYPE pAPI;

            // Update aplication offsets for each subslot (located in IOCR-API-List)
            // Note: information is used for IO-ADDR list in IODU request
            for ( CmListForeach( pAPI, &pIOCR->related_ioapi_list, CM_UPPER_ARCB_IOAPI_PTR_TYPE))
            {
                CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE pIODATA;

                // Add the IODATA, IOPS information
                for ( CmListForeach( pIODATA, &pAPI->related_iodata_objects_list, CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE))
                {
                    CIOCSubslot* pSubObj = GetSubslot( pAPI->api, pIODATA->slot_nr, pIODATA->subslot_nr );
                    PND_ASSERT( pSubObj != 0 );

                    // Update IODATA offset for submod, and add to subslot list
                    // Note: information is stored to complete IODU address package with IODATA and IOCS from other IOCR
                    if ( isProvider ) // Provider (output frame) ?
                    {
                        pSubObj->SetOutputOffset( pIODATA->application_offset );
                        prov.push_back( pSubObj );
                    }
                    else
                    {
                        pSubObj->SetInputOffset( pIODATA->application_offset );
                        cons.push_back( pSubObj );
                    }
                }

                CM_UPPER_ARCB_IOCS_PTR_TYPE pIOCS;

                // Add the IOCS information
                for (CmListForeach( pIOCS, &pAPI->related_iocs_list, CM_UPPER_ARCB_IOCS_PTR_TYPE))
                {
                    CIOCSubslot* pSubObj = GetSubslot( pAPI->api, pIOCS->slot_nr, pIOCS->subslot_nr );
                    PND_ASSERT( pSubObj != 0 );

                    // Update IOCS offset for submod
                    // Note: information is stored to complete IODU address package with IODATA and IOCS from other IOCR
                    if ( isProvider ) // Provider (output frame) ?
                    {
                        pSubObj->SetInputOffsetIOCS( pIOCS->application_offset );
                    }
                    else
                    {
                        pSubObj->SetOutputOffsetIOCS( pIOCS->application_offset );
                    }
                }
            }
        }
    }

    return ( pAR );
}

/*----------------------------------------------------------------------------*/
PND_IO_ADDR_PTR_TYPE CIOCDevice::createAddressList( UserNodeVector &sublist, LSA_BOOL isProvider )
{
    PND_IO_ADDR_PTR_TYPE pAddr = 0;

    if ( !sublist.empty() ) // empty subslot list ?
    {
        LSA_UINT32 count = sublist.size();

        pAddr = (PND_IO_ADDR_PTR_TYPE)pnd_mem_alloc( count * sizeof(PND_IO_ADDR_TYPE));
        PND_ASSERT( pAddr != 0 );

        // add address settings for each subslot in list
        for ( LSA_UINT16 idx = 0; idx < count; idx++ )
        {
            CIOCSubslot *pSub = dynamic_cast<CIOCSubslot*>( sublist[idx] );
            PND_ASSERT( pSub != 0 );

            if ( isProvider )
            {
                pAddr[idx] = pSub->GetAddressForOutput();
            }
            else
            {
                pAddr[idx] = pSub->GetAddressForInput();
            }
        }
    }

    return ( pAddr );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
