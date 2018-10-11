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
/*  F i l e               &F: pnd_IOCSubslot.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of controller Subslot class (shadow real cfg on IOC)      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3204
#define PND_MODULE_ID       3204

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
#include "pnd_IOCAr.h"
#include "pnd_IOCEventHandler.h"
#include "pnd_IOCDevice.h"
#include "pnd_IOCSubslot.h"
#include "pnd_IOCUser.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CIOCSubslot
/////////////////////////////////////////////////////////////////////////////

CIOCSubslot::CIOCSubslot( LSA_UINT32 api, LSA_UINT16 slot, LSA_UINT16 subSlot, 
                          PND_HANDLE_PTR_TYPE pHandle, CUserNode* parent) : CUserNode( pHandle, parent )
{
    // Initialize key values
    m_api       = api;
    m_slotNr    = slot;
    m_subslotNr = subSlot;

    // basic subslot attributes
    m_slotIdent    = 0;
    m_subslotIdent = 0;

    m_slotLaddr    = PND_LADDR_INVALID;
    m_subslotLaddr = PND_LADDR_INVALID;

    // IO subslot attributes
    m_subslotProp     = 0;
    m_isDiscardIOXS   = LSA_FALSE;
    m_isReducedInput  = LSA_FALSE;
    m_isReducedOutput = LSA_FALSE;

    m_input.length_IO_data = 0;
    m_input.offset_IO_data = PND_IO_OFFSET_INVALID;
    m_input.offset_IOCS    = PND_IO_OFFSET_INVALID;
    m_input.IO_base_addr   = PND_LADDR_INVALID;

    m_output.length_IO_data = 0;
    m_output.offset_IO_data = PND_IO_OFFSET_INVALID;
    m_output.offset_IOCS    = PND_IO_OFFSET_INVALID;
    m_output.IO_base_addr   = PND_LADDR_INVALID;
	
	// IO-CR parameters
	m_reductionRatio = 0;
	m_phase          = 0;
	m_cycleTime      = 0;
}

/*----------------------------------------------------------------------------*/
CIOCSubslot::~CIOCSubslot()
{
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::Create( LSA_VOID* pData )
{
    PND_PARAM_SUBSLOT_PTR_TYPE pSUB = (PND_PARAM_SUBSLOT_PTR_TYPE)pData;
    PND_ASSERT( pSUB != 0 );

    PND_IOC_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::Create(): API/Slot/Subslot(%#x/%#x/%#x) modLADDR(%u) subLADDR(%u) modIdent(%#x) subIdent(%#x)",
        m_api, 
        m_slotNr, 
        m_subslotNr, 
        pSUB->slot_laddr,
        pSUB->subslot_laddr,
        pSUB->slot_ident, 
        pSUB->subslot_ident );

    PND_IOC_TRACE_08( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::Create(): API/Slot/Subslot(%#x/%#x/%#x) subProp(%#x) inpLen(%u) iBaseAddr(%u) outLen(%u) qBaseAddr(%u)",
        m_api, 
        m_slotNr, 
        m_subslotNr, 
        pSUB->subslot_properties,
        pSUB->input_length,
        pSUB->input_base_laddr,
        pSUB->output_length,
        pSUB->output_base_laddr );

    m_slotLaddr    = pSUB->slot_laddr;
    m_subslotLaddr = pSUB->subslot_laddr;
    m_slotIdent    = pSUB->slot_ident;
    m_subslotIdent = pSUB->subslot_ident;
    m_subslotProp  = pSUB->subslot_properties;

    // set IO properties
    // note: offsets are set from ARCB after CM aplication offset calculation
    m_input.IO_base_addr   = pSUB->input_base_laddr;
    m_input.length_IO_data = pSUB->input_length;

    m_output.IO_base_addr   = pSUB->output_base_laddr;
    m_output.length_IO_data = pSUB->output_length;

	// set IO-CR Parameters
	m_reductionRatio = pSUB->reduction_ratio;
	m_phase          = pSUB->phase;
	m_cycleTime      = pSUB->cycle_time;

    // set the reduced bits (IOXS, IODATA reduce)
    setIOProperties();
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CIOCSubslot::GetKey()
{
    string retStr = CIOCSubslot::GetKey( m_api,m_slotNr,m_subslotNr) ;
    return ( retStr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CIOCSubslot::GetNumber()
{
    return( m_subslotNr );
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CIOCSubslot::GetPnioHandle()
{
    PND_ASSERT( m_pParentNode != 0 );
    LSA_HANDLE_TYPE handle = m_pParentNode->GetPnioHandle();

    return ( handle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CIOCSubslot::GetKey( LSA_UINT32 api, LSA_UINT16 slot, LSA_UINT16 subSlot )
{
    CValueHelper valObj;

    // Format <API>/<SLOT>/<SUBSLOT>
    string result = 
        valObj.UINT32ToString( api )  + "/" + 
        valObj.UINT16ToString( slot ) + "/" + 
        valObj.UINT16ToString( subSlot );
    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetApiNr()
{
    return( m_api );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCSubslot::GetSlotNr()
{
    return( m_slotNr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCSubslot::GetSubslotNr()
{
    LSA_UINT16 nr = (LSA_UINT16)GetNumber();
    return ( nr  );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCSubslot::GetSlotLogAddr()
{
    return ( m_slotLaddr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCSubslot::GetSubslotLogAddr()
{
    return ( m_subslotLaddr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetSlotIdent()
{
    return ( m_slotIdent );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetSubslotIdent()
{
    return ( m_subslotIdent );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetReductionRatio()
{
	return ( m_reductionRatio );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetPhase()
{
	return ( m_phase );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CIOCSubslot::GetCycleTime()
{
	return ( m_cycleTime );
}

/*----------------------------------------------------------------------------*/
PND_IO_ADDR_TYPE CIOCSubslot::GetAddressForInput()
{
    return ( m_input );
}

/*----------------------------------------------------------------------------*/
PND_IO_ADDR_TYPE CIOCSubslot::GetAddressForOutput()
{
    return ( m_output );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCSubslot::SetState( LSA_UINT16 state )
{
    // Reset and update BGZD from new submod state
    m_bgzdStore.UpdateFromSubslotState( state );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::SetInputOffset( LSA_UINT16 offset )
{
    m_input.offset_IO_data = offset;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::SetInputOffsetIOCS( LSA_UINT16 offset )
{
    m_input.offset_IOCS = offset;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::SetOutputOffset( LSA_UINT16 offset )
{
    m_output.offset_IO_data = offset;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::SetOutputOffsetIOCS( LSA_UINT16 offset )
{
    m_output.offset_IOCS = offset;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCSubslot::UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintenanceStatus )
{
    // Update the BGZD State from Alarm info, and trigger upper update
    m_bgzdStore.UpdateFromAlarm( hasDiag, maintenanceStatus );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::ResetARP()
{
    // Remove the A.R.P state from BGZD
    // Note: RoS Alarm implicit indicates end of A.R.P
    m_bgzdStore.SetARP( LSA_FALSE );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCSubslot::RecordRead( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT( p_rb != 0 );

    // Read the record over AR if AR exist with correct state otherwise over implicit read
    // Note: Possible the AR read failed, because CM has a different state

    CIOCAr *pAR = getAr();

    if (( pAR != 0 ) && ( pAR->IsRecordReadAllowed())) // can read over AR ?
    {
        // Try reading record over AR connection
        ReadRecordAR( p_rb );
    }
    else
    {
        // Try reading record implict
        ReadRecordImplicit( p_rb );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::RecordWrite( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT( p_rb != 0 );

    CIOCAr *pAR = getAr();

    if (( pAR != 0 )) // ar availabel ?
    {
        WriteRecord( p_rb );
    }

}

/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) Request functions                                               */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCSubslot::ReadRecordAR( PND_RQB_PTR_TYPE p_rb )
{
    LSA_HANDLE_TYPE handle = GetPnioHandle();

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_RECORD_READ, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_RECORD_READ_WRITE_PTR_TYPE pRead = &p_rb->args.rec_read;

    // Get the AR specific settings
    CIOCAr *pAR = getAr();

    LSA_UINT16 devNr  = GetDeviceNr();
    LSA_UINT16 arNr   = ( pAR != 0 ) ? pAR->GetArNumber() : 0;
    LSA_UINT16 sesKey = ( pAR != 0 ) ? pAR->GetSessionKey() : 0;

    PND_IOC_TRACE_09( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordAR(): IOC READ record, dev(%u) ar(%u) sesKey(%u) api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u) RQB(0x%08x)", 
        devNr, 
        arNr, 
        sesKey,
        m_api, 
        m_slotNr, 
        m_subslotNr, 
        pRead->RecordIndex, 
        pRead->Length, 
        pRQB );

    // Prepare the CM RQB 
    pRQB->args.cl.read->device_nr   = devNr;
    pRQB->args.cl.read->ar_nr       = arNr;
    pRQB->args.cl.read->session_key = sesKey;

    pRQB->args.cl.read->rec.api              = m_api;
    pRQB->args.cl.read->rec.slot_nr          = m_slotNr;
    pRQB->args.cl.read->rec.subslot_nr       = m_subslotNr;
    pRQB->args.cl.read->rec.record_index     = (LSA_UINT16)pRead->RecordIndex;
    pRQB->args.cl.read->rec.readquery_length = 0;
    pRQB->args.cl.read->rec.data_length      = CM_RECORD_OFFSET + pRead->Length;

    pRQB->args.cl.read->rec.data = (LSA_UINT8*)pnd_cm_alloc_upper_mem( (LSA_UINT16)pRQB->args.cl.read->rec.data_length, &m_pndHandle );
    PND_ASSERT( pRQB->args.cl.read->rec.data != 0 );

    // store PND RQB in user ID for done handling
    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCSubslot::ReadRecordARDone );
}


/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::ReadRecordImplicit( PND_RQB_PTR_TYPE p_rb )
{
    LSA_HANDLE_TYPE handle = GetPnioHandle();

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_RECORD_READ_IMPLICIT, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_RECORD_READ_WRITE_PTR_TYPE pRead = &p_rb->args.rec_read;

    LSA_UINT16 devNr = GetDeviceNr();

    PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordImplicit(): IOC READ record implicit, api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u) RQB(0x%08x)", 
        m_api, 
        m_slotNr, 
        m_subslotNr, 
        pRead->RecordIndex, 
        pRead->Length, 
        pRQB );

    // Create the ARCB settings for READ implicit
    CParamStore *pParam = CParamStore::GetInstance();
    PND_ASSERT( pParam != 0 );

    LSA_UINT32 pnioRes = pParam->CreateArcbReadImpl( &m_pndHandle, devNr, pRQB->args.cl.read_implicit );
    PND_ASSERT( pnioRes == PNIO_OK );

    pRead->pnio_err = pnioRes;

    pRQB->args.cl.read_implicit->target_uuid = clrpc_uuid_nil_uuid;  // not used

    pRQB->args.cl.read_implicit->rec.api              = m_api;
    pRQB->args.cl.read_implicit->rec.slot_nr          = m_slotNr;
    pRQB->args.cl.read_implicit->rec.subslot_nr       = m_subslotNr;
    pRQB->args.cl.read_implicit->rec.record_index     = (LSA_UINT16)pRead->RecordIndex;
    pRQB->args.cl.read_implicit->rec.readquery_length = 0;
    pRQB->args.cl.read_implicit->rec.data_length      = CM_RECORD_OFFSET + pRead->Length; 

    pRQB->args.cl.read_implicit->rec.data = (LSA_UINT8*)pnd_cm_alloc_upper_mem( (LSA_UINT16)pRQB->args.cl.read_implicit->rec.data_length, &m_pndHandle );
    PND_ASSERT( pRQB->args.cl.read_implicit->rec.data != 0 );

    // store PND RQB in user ID for done handling
    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCSubslot::ReadRecordImplicitDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::WriteRecord(PND_RQB_PTR_TYPE p_rb)
{
    LSA_HANDLE_TYPE  handle = GetPnioHandle();

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_RECORD_WRITE, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_RECORD_READ_WRITE_PTR_TYPE pWrite = &p_rb->args.rec_write;

    // Get the AR specific settings
    CIOCAr *pAR = getAr();

    LSA_UINT16 devNr  = GetDeviceNr();
    LSA_UINT16 arNr   = ( pAR != 0 ) ? pAR->GetArNumber() : 0;
    LSA_UINT16 sesKey = ( pAR != 0 ) ? pAR->GetSessionKey() : 0;

    PND_IOC_TRACE_09( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::WriteRecord(): IOC WRITE record, dev(%u) ar(%u) sesKey(%u) api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u) RQB(0x%08x)", 
        devNr, 
        arNr, 
        sesKey,
        m_api, 
        m_slotNr, 
        m_subslotNr, 
        pWrite->RecordIndex, 
        pWrite->Length, 
        pRQB );

    pRQB->args.cl.write->device_nr        = devNr;
    pRQB->args.cl.write->ar_nr            = arNr;
    pRQB->args.cl.write->session_key      = sesKey;

    pRQB->args.cl.write->rec.api          = m_api;
    pRQB->args.cl.write->rec.slot_nr      = m_slotNr;
    pRQB->args.cl.write->rec.subslot_nr   = m_subslotNr;
    pRQB->args.cl.write->rec.record_index = (LSA_UINT16)pWrite->RecordIndex;
    pRQB->args.cl.write->rec.data         = 0;
    pRQB->args.cl.write->rec.data_length  = CM_RECORD_OFFSET + pWrite->Length;

    if ( pWrite->Length > 0 )
    {
        PND_ASSERT( pWrite->pBuffer != 0 );

        LSA_UINT32 allocSize = CM_RECORD_OFFSET + pWrite->Length;  // Including header

        pRQB->args.cl.write->rec.data_length = allocSize;

        pRQB->args.cl.write->rec.data = (LSA_UINT8*)pnd_cm_alloc_upper_mem( (LSA_UINT16)allocSize, &m_pndHandle );
        PND_ASSERT( pRQB->args.cl.write->rec.data != 0 );

        // Set the records from user behind the CM header
        pnd_memcpy( pRQB->args.cl.write->rec.data + CM_RECORD_OFFSET, pWrite->pBuffer, pWrite->Length );
    }

    // store PND RQB in user ID for done handling
    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCSubslot::WriteRecordDone );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::DeviceActivate( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT(p_rb != 0);

    CIOCDevice* ParentDevice = CIOCSubslot::getDevice();
    CIOCUser* ParentIoc = ParentDevice->getIoc();
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
                ParentDevice->SetStatusActivate();
                break;
            case PNIO_DA_FALSE:
                ParentDevice->SetStatusPassivate();
                break;
            default:
                PND_FATAL("Device Activateunknowen command");
                break;
        }

        pnd_pnio_user_request_iobc_done( p_rb );
    }
}
/*---------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::Activate(PND_RQB_PTR_TYPE p_rb)
{
    PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;

    LSA_HANDLE_TYPE handle = GetPnioHandle();
    CIOCDevice* ParentDevice = CIOCSubslot::getDevice();
    LSA_UINT16  ParentDeviceNr = (LSA_UINT16)ParentDevice->GetNumber();

    PND_ASSERT(pActivate->DevActMode == PNIO_DA_TRUE);

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::Activate(): CM-CL DeviceActive.REQ, nr(%u) pRQB(0x%08x)", 
        ParentDeviceNr, pRQB );

    pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_ACTIVATE;
    pRQB->args.cl.dev_control->device_nr   = ParentDeviceNr;

    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCSubslot::ActivateDone );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::Passivate(PND_RQB_PTR_TYPE p_rb)
{
    PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;
    LSA_HANDLE_TYPE handle = GetPnioHandle();
    CIOCDevice* ParentDevice = CIOCSubslot::getDevice();
    LSA_UINT16  ParentDeviceNr = (LSA_UINT16)ParentDevice->GetNumber();

    PND_ASSERT(pActivate->DevActMode == PNIO_DA_FALSE);

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_CONTROL, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::Passivate(): CM-CL DevicePassivate.REQ, nr(%u) pRQB(0x%08x)", 
        ParentDeviceNr, pRQB );

    pRQB->args.cl.dev_control->control_tag = CM_CL_DEVICE_CONTROL_PASSIVATE;
    pRQB->args.cl.dev_control->device_nr   = ParentDeviceNr;

    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

    eps_user_request_lower( pRQB, this, CIOCSubslot::PassivateDone );
}


/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) callback functions                                              */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCSubslot::ReadRecordARDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCSubslot *pSub          = static_cast<CIOCSubslot*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pSub != 0);
    PND_ASSERT( pRQB != 0);

    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_RECORD_READ );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );
    // LSA_UINT32 recIdx     = pRQB->args.cl.read->rec.record_index;
    LSA_UINT32 pnioErr    = pRQB->args.cl.read->rec.cm_pnio_err;

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

    PND_IOC_TRACE_06( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordARDone(): IOC Record Read done, rsp(%#x) pnio_stat(%#x) dev(%u) ar(%u) sesKey(%u) RQB(0x%08x)", 
        rsp,
        pnioErr,
        pRQB->args.cl.read->device_nr,
        pRQB->args.cl.read->ar_nr,
        pRQB->args.cl.read->session_key,
        pRQB );

    PND_IOC_TRACE_06( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordARDone():    api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u/%x)", 
        pRQB->args.cl.read->rec.api,
        pRQB->args.cl.read->rec.slot_nr,
        pRQB->args.cl.read->rec.subslot_nr,
        pRQB->args.cl.read->rec.record_index,
        pRQB->args.cl.read->rec.data_length,
        pRQB->args.cl.read->rec.data_length );


    // Copy CM PNIO state fields to PND RQB
    pPndRQB->args.rec_read.err.AddValue1 = pRQB->args.cl.read_implicit->rec.ret_val_1;
    pPndRQB->args.rec_read.err.AddValue2 = pRQB->args.cl.read_implicit->rec.ret_val_2;
    pPndRQB->args.rec_read.err.ErrCode   = CM_PNIO_ERR_FETCH_CODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrDecode = CM_PNIO_ERR_FETCH_DECODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode1  = CM_PNIO_ERR_FETCH_CODE_1( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode2  = CM_PNIO_ERR_FETCH_CODE_2( pnioErr );


    if (( rsp == CM_OK ) && ( pnioErr == CM_PNIO_ERR_NONE )) // AR Read OK ?
    {
        pPndRQB->args.rec_read.RecordIndex = pRQB->args.cl.read->rec.record_index;

        PND_ASSERT(pPndRQB->args.rec_read.Length >= (pRQB->args.cl.read->rec.data_length - CM_RECORD_OFFSET));

        pnd_memcpy( pPndRQB->args.rec_read.pBuffer, &pRQB->args.cl.read->rec.data[CM_RECORD_OFFSET], pRQB->args.cl.read->rec.data_length - CM_RECORD_OFFSET);

		pPndRQB->args.rec_read.Length = pRQB->args.cl.read->rec.data_length - CM_RECORD_OFFSET;

        pPndRQB->args.rec_read.pnio_err = PNIO_OK;

        pnd_pnio_user_request_iobc_done( pPndRQB );
    }
    else
    {
        // Continue with try Read implict
        pSub->ReadRecordImplicit( pPndRQB );
    }

    if ( pRQB->args.cl.read->rec.data != 0 )
    {
        pnd_cm_free_upper_mem( pRQB->args.cl.read->rec.data, &pSub->m_pndHandle );

        pRQB->args.cl.read->rec.data = 0;
    }

    pnd_cm_free_rqb( pRQB, &pSub->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::ReadRecordImplicitDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCSubslot *pSub          = static_cast<CIOCSubslot*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pSub != 0);
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_RECORD_READ_IMPLICIT );

    // Store the record for upper update
    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );
    LSA_UINT32 pnioErr    = pRQB->args.cl.read_implicit->rec.cm_pnio_err;

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

    PND_IOC_TRACE_04( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordImplicitDone(): IOC Record Read Implicit done, rsp(%#x) lowerRsp(%#x) pnioErr(%#x) RQB(0x%08x)", 
        rsp,
        pRQB->args.cl.read_implicit->lower_response,
        pnioErr,
        pRQB );

    PND_IOC_TRACE_06( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ReadRecordImplicitDone():    api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u/%#x)", 
        pRQB->args.cl.read_implicit->rec.api,
        pRQB->args.cl.read_implicit->rec.slot_nr,
        pRQB->args.cl.read_implicit->rec.subslot_nr,
        pRQB->args.cl.read_implicit->rec.record_index,
        pRQB->args.cl.read_implicit->rec.data_length,
        pRQB->args.cl.read_implicit->rec.data_length );

    // Copy CM PNIO state fields to PND RQB
    pPndRQB->args.rec_read.err.AddValue1 = pRQB->args.cl.read_implicit->rec.ret_val_1;
    pPndRQB->args.rec_read.err.AddValue2 = pRQB->args.cl.read_implicit->rec.ret_val_2;
    pPndRQB->args.rec_read.err.ErrCode   = CM_PNIO_ERR_FETCH_CODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrDecode = CM_PNIO_ERR_FETCH_DECODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode1  = CM_PNIO_ERR_FETCH_CODE_1( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode2  = CM_PNIO_ERR_FETCH_CODE_2( pnioErr );

    if (( rsp == CM_OK ) && ( pnioErr == CM_PNIO_ERR_NONE )) // Read OK ?
    {
        PND_ASSERT( pPndRQB->args.rec_read.Length >= (pRQB->args.cl.read_implicit->rec.data_length - CM_RECORD_OFFSET));

        pnd_memcpy( pPndRQB->args.rec_read.pBuffer, pRQB->args.cl.read_implicit->rec.data + CM_RECORD_OFFSET, pRQB->args.cl.read_implicit->rec.data_length - CM_RECORD_OFFSET);

		pPndRQB->args.rec_read.Length = pRQB->args.cl.read_implicit->rec.data_length - CM_RECORD_OFFSET;

        pPndRQB->args.rec_read.pnio_err = PNIO_OK;

        pnd_pnio_user_request_iobc_done( pPndRQB );
    }
    else
    {
        // complete record reading request from IOBC with error
        pPndRQB->args.rec_read.Length   = 0;         // No data avaliable
        pPndRQB->args.rec_read.pnio_err = PNIO_ERR_INTERNAL;

        pnd_pnio_user_request_iobc_done( pPndRQB );
    }

    if ( pRQB->args.cl.read_implicit->rec.data != 0 )
    {
        pnd_cm_free_upper_mem( pRQB->args.cl.read_implicit->rec.data, &pSub->m_pndHandle );

        pRQB->args.cl.read_implicit->rec.data = 0;
    }

    // Delete the ARCB settings for READ implicit
    CParamStore *pParam = CParamStore::GetInstance();
    PND_ASSERT( pParam != 0 );

    pParam->DeleteArcbReadImpl( &pSub->m_pndHandle, pRQB->args.cl.read_implicit );

    pnd_cm_free_rqb( pRQB, &pSub->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::WriteRecordDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCSubslot *pSub          = static_cast<CIOCSubslot*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pSub != 0);
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_RECORD_WRITE );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );
    LSA_UINT32 pnioErr    = pRQB->args.cl.write->rec.cm_pnio_err;

        // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

    PND_IOC_TRACE_06( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::WriteRecordDone(): IOC Record Write done, rsp(%#x) pnioErr(%#x) dev(%u) ar(%u) sesKey(%u) RQB(0x%08x)", 
        rsp,
        pnioErr,
        pRQB->args.cl.write->device_nr,
        pRQB->args.cl.write->ar_nr,
        pRQB->args.cl.write->session_key,
        pRQB );

    PND_IOC_TRACE_05( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::WriteRecordDone():    api(%#x) slot(%#x) sub(%#x) recIdx(%#x) dataSize(%u)", 
        pRQB->args.cl.write->rec.api,
        pRQB->args.cl.write->rec.slot_nr,
        pRQB->args.cl.write->rec.subslot_nr,
        pRQB->args.cl.write->rec.record_index,
        pRQB->args.cl.write->rec.data_length );

        // Copy CM PNIO state fields to PND RQB
    pPndRQB->args.rec_write.err.AddValue1 = pRQB->args.cl.read_implicit->rec.ret_val_1;
    pPndRQB->args.rec_write.err.AddValue2 = pRQB->args.cl.read_implicit->rec.ret_val_2;
    pPndRQB->args.rec_write.err.ErrCode   = CM_PNIO_ERR_FETCH_CODE( pnioErr );
    pPndRQB->args.rec_write.err.ErrDecode = CM_PNIO_ERR_FETCH_DECODE( pnioErr );
    pPndRQB->args.rec_write.err.ErrCode1  = CM_PNIO_ERR_FETCH_CODE_1( pnioErr );
    pPndRQB->args.rec_write.err.ErrCode2  = CM_PNIO_ERR_FETCH_CODE_2( pnioErr );

    if (( rsp == CM_OK ) && ( pnioErr == CM_PNIO_ERR_NONE ))
    {
        pPndRQB->args.rec_write.pnio_err = PNIO_OK;
    }
    else
    {
        pPndRQB->args.rec_write.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_iobc_done( pPndRQB );


    if ( pRQB->args.cl.write->rec.data != 0 )
    {
        pnd_cm_free_upper_mem( pRQB->args.cl.write->rec.data, &pSub->m_pndHandle );

        pRQB->args.cl.write->rec.data = 0;
    }

    pnd_cm_free_rqb( pRQB, &pSub->m_pndHandle );
}


/*---------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::ActivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCSubslot *pSubSlot        = static_cast<CIOCSubslot*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB   = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    CIOCDevice* ParentDevice =  pSubSlot->getDevice();
    LSA_UINT16  ParentDeviceNr = (LSA_UINT16)ParentDevice->GetNumber();

    PND_ASSERT( pSubSlot != 0 );
    PND_ASSERT( pRQB    != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)                == CM_OPC_CL_DEVICE_CONTROL );
    PND_ASSERT( pRQB->args.cl.dev_control->device_nr   == ParentDeviceNr );
    PND_ASSERT( pRQB->args.cl.dev_control->control_tag == CM_CL_DEVICE_CONTROL_ACTIVATE );

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

    LSA_RESPONSE_TYPE  rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pSubSlot->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::ActivateDone(): CM-CL DeviceActive.CNF, rsp(%u/%#x) nr(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr, 
        pRQB );

    if ( rsp == CM_OK )
    {
        // set BGZD to not passivated 
        // Note: necessary because it is possible, that indication is received late
        CIOCAr *pAr = ParentDevice->GetAr();
        PND_ASSERT ( pAr != 0 );

        if (( pAr->GetArState() == PND_IOC_AR_STATE_PASSIVE ) || // From Passive state
            ( pAr->GetArState() == PND_IOC_AR_STATE_UNKNOWN ))   // After Add device
        {
            // Initialize the submod and AR state (BGZD)
            ParentDevice->ResetSubslots( CM_SUB_STATE_GOOD );
            ParentDevice->updateArState( PND_IOC_AR_STATE_START );
        }
        
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_OK;
        ParentDevice->SetStatusActivate();
    }
    else
    {
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_iobc_done( pPndRQB );

    pnd_cm_free_rqb( pRQB, &pSubSlot->m_pndHandle );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::PassivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCSubslot *pSubSlot        = static_cast<CIOCSubslot*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB   = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    CIOCDevice* ParentDevice =  pSubSlot->getDevice();
    LSA_UINT16  ParentDeviceNr = (LSA_UINT16)ParentDevice->GetNumber();

    PND_ASSERT( pSubSlot != 0 );
    PND_ASSERT( pRQB    != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)                == CM_OPC_CL_DEVICE_CONTROL );
    PND_ASSERT( pRQB->args.cl.dev_control->device_nr   == ParentDeviceNr );
    PND_ASSERT( pRQB->args.cl.dev_control->control_tag == CM_CL_DEVICE_CONTROL_PASSIVATE );

    // resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );
    
    LSA_RESPONSE_TYPE  rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pSubSlot->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCSubslot::PassivateDone(): CM-CL DevicePassivate.CNF, rsp(%u/%#x) nr(%u) pRQB(0x%08x)",
        rsp, rsp,
        pRQB->args.cl.dev_remove->device_nr, 
        pRQB );

    if ( rsp == CM_OK )
    {
        // Initialize the submod state (for update all)
        ParentDevice->ResetSubslots( CM_SUB_STATE_GOOD );
        ParentDevice->SetStatusPassivate();

        // Set AR state to passivated (used by BGZD-check)
        // Note: necessary because it is possible, that no indication is provided by CM
        CIOCAr *pAr = pSubSlot->getAr();
        PND_ASSERT ( pAr != 0 );

        pAr->SetArState( PND_IOC_AR_STATE_PASSIVE );

        pPndRQB->args.iob_dev_act.pnio_err = PNIO_OK;
    }
    else
    {
        pPndRQB->args.iob_dev_act.pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_pnio_user_request_iobc_done( pPndRQB );

    pnd_cm_free_rqb( pRQB, &pSubSlot->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CIOCDevice* CIOCSubslot::getDevice()
{
    CIOCDevice* pDevice = dynamic_cast<CIOCDevice*>( GetParent() );
    PND_ASSERT( pDevice != 0 );

    return ( pDevice );
}

/*----------------------------------------------------------------------------*/
CIOCAr* CIOCSubslot::getAr()
{
    CIOCDevice* pDevice = dynamic_cast<CIOCDevice*>( GetParent() );
    PND_ASSERT( pDevice != 0 );

    CIOCAr* pAR = pDevice->GetAr();

    return ( pAR );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCSubslot::GetDeviceNr()
{
    CIOCDevice* pDevice = dynamic_cast<CIOCDevice*>( GetParent() );
    PND_ASSERT( pDevice != 0 );

    LSA_UINT16 devNr = (LSA_UINT16)pDevice->GetNumber();

    return ( devNr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCSubslot::setIOProperties()
{
    switch ( m_subslotProp & CM_SUB_PROP_TYPE_MASK ) // Analyse submod type
    {
    case CM_SUB_PROP_TYPE_NO_DATA: // Dataless
        break;
    case CM_SUB_PROP_TYPE_INPUT:   // Input
        {
            if (( m_subslotProp & CM_SUB_PROP_REDUCE_INP_LEN_MASK ) == CM_SUB_PROP_REDUCE_INP_LEN_YES ) // reduced ?
            {
                m_isReducedInput = LSA_TRUE;
            }
        }
        break;
    case CM_SUB_PROP_TYPE_OUTPUT:   // Output
        {
            // Set the properties based on reduced bits
            if (( m_subslotProp & CM_SUB_PROP_REDUCE_OUT_LEN_MASK ) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ) // reduced ?
            {
                m_isReducedOutput = LSA_TRUE;
            }
        }
        break;
    case CM_SUB_PROP_TYPE_INPUT_OUTPUT:   // Inout
        {
            if (( m_subslotProp & CM_SUB_PROP_REDUCE_INP_LEN_MASK ) == CM_SUB_PROP_REDUCE_INP_LEN_YES ) // reduced ?
            {
                m_isReducedInput = LSA_TRUE;
            }

            if (( m_subslotProp & CM_SUB_PROP_REDUCE_OUT_LEN_MASK ) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ) // reduced ?
            {
                m_isReducedOutput = LSA_TRUE;
            }
        }
        break;
    default:
        {
            PND_FATAL( "CIOCSubslot::setIOProperties(): unexpected submod type value" );
        }
        break;
    }

    // Check IOXS discard setting
    if (( m_subslotProp & CM_SUB_PROP_DISCARD_IOXS_MASK ) == CM_SUB_PROP_DISCARD_IOXS_YES ) // reduced ?
    {
        // configure with discard IOXS
        m_isDiscardIOXS = LSA_TRUE;
    }
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
