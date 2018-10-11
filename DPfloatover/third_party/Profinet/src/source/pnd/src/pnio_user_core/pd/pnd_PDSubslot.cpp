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
/*  F i l e               &F: PDSubslot.cpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PDEV Subslot class                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3502
#define PND_MODULE_ID      3502

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
// CPDSubslot PD subslot storage class to hold PDEV subslot specific data
/////////////////////////////////////////////////////////////////////////////

CPDSubslot::CPDSubslot( LSA_UINT16 portID, PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent ) : CUserNode( pHandle, pParent )
{
	m_portID = portID;

	// subslot information
	m_slotNr       = 0;
	m_subslotNr    = 0;
	m_slotIdent    = 0;
	m_subslotIdent = 0;
	m_laddr        = PND_LADDR_INVALID;

	// Sync Information
	m_syncState    = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;
	m_isRateValid  = LSA_FALSE;
	m_rcvSyncPrio  = 0;
	m_localPrio    = 0;
	m_pllState     = CM_PD_PLL_STATE_UNKNOWN;

	// MAU-Type information
	m_mauStatus = CM_LINK_STATUS_UNKNOWN;
	m_mauSpeed  = CM_LINK_SPEED_UNKNOWN;
	m_mauMode   = CM_LINK_MODE_UNKNOWN;
}

/*----------------------------------------------------------------------------*/
CPDSubslot::~CPDSubslot()
{
	// cleanup 
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::Create( LSA_UINT16 slotNr, LSA_UINT16 subslotNr, LSA_UINT32 modIdent, LSA_UINT32 subIdent, LSA_UINT16 laddr )
{
	PND_PD_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::Create(): Creating PDEV subslot, Port-ID(%u) slot(%#x) sub(%#x) modIdent(%#x) subIdent(%#x) laddr(%#x)", 
		m_portID, slotNr, subslotNr, modIdent, subIdent, laddr );

	m_slotNr       = slotNr;
	m_subslotNr    = subslotNr;
	m_slotIdent    = modIdent;
	m_subslotIdent = subIdent;
	m_laddr        = laddr;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CPDSubslot::GetKey()
{
	string retStr = CPDSubslot::GetKey( m_portID );
	return ( retStr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CPDSubslot::GetNumber()
{
	return( m_portID );
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CPDSubslot::GetPnioHandle()
{
	PND_ASSERT( m_pParentNode != 0 );
	LSA_HANDLE_TYPE handle = m_pParentNode->GetPnioHandle();

	return ( handle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CPDSubslot::GetKey( LSA_UINT16 portID )
{
	CValueHelper valObj;

	string result = valObj.UINT16ToString( portID );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPDSubslot::GetSlotNr()
{
	return( m_slotNr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPDSubslot::GetSubslotNr()
{
	return( m_subslotNr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPDSubslot::GetSubslotLogAddr()
{
	return( m_laddr );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPDSubslot::GetSlotIdent()
{
	return( m_slotIdent );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPDSubslot::GetSubslotIdent()
{
	return( m_subslotIdent );
}

/*----------------------------------------------------------------------------*/
PND_SYNC_STATE_TYPE CPDSubslot::GetSyncState()
{
	PND_SYNC_STATE_TYPE result = PND_SYNC_STATE_NOT_CONFIGURED;

	switch ( m_syncState ) 
	{
	case CM_PD_SYNC_LED_INFO_NOT_SYNC:
		{
			result = PND_SYNC_STATE_NOT_SYNC;
		}
		break;
	case CM_PD_SYNC_LED_INFO_SYNC:
		{
			result = PND_SYNC_STATE_SYNC;
		}
		break;
	default:
		{
			result = PND_SYNC_STATE_NOT_CONFIGURED;
		}
		break;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void CPDSubslot::Map( LSA_UINT16 slotNr, LSA_UINT16 subslotNr, LSA_UINT32 modIdent, LSA_UINT32 subIdent )
{
	// Add PNIO address mapping
	m_slotNr       = slotNr;
	m_subslotNr    = subslotNr;
	m_slotIdent    = modIdent;
	m_subslotIdent = subIdent;
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::Unmap()
{
	// Remove PNIO address mapping 
	m_slotNr       = 0;
	m_subslotNr    = 0;
	m_slotIdent    = 0;
	m_subslotIdent = 0;
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::SetMauState( LSA_UINT8 status, LSA_UINT8 speed, LSA_UINT8 mode )
{
	PND_ASSERT(
		status == CM_LINK_STATUS_UP
		|| status == CM_LINK_STATUS_DOWN
		|| status == CM_LINK_STATUS_UP_CLOSED
		|| status == CM_LINK_STATUS_UP_DISABLED
	);

	PND_ASSERT(
		speed == CM_LINK_SPEED_UNKNOWN
		|| speed == CM_LINK_SPEED_10_M
		|| speed == CM_LINK_SPEED_100_M
		|| speed == CM_LINK_SPEED_1_G
		|| speed == CM_LINK_SPEED_10_G
	);

	PND_ASSERT(
		mode == CM_LINK_MODE_UNKNOWN
		|| mode == CM_LINK_MODE_HALF_DUPLEX
		|| mode == CM_LINK_MODE_FULL_DUPLEX
	);

	if ( status != CM_LINK_STATUS_DOWN ) // link up ?
	{
		m_mauStatus = status;
		m_mauSpeed  = speed;
		m_mauMode   = mode;
	}
	else
	{
		// Note out params speed and mode only valid if link is up (see cm_usr.h)
		m_mauStatus = status;
		m_mauSpeed  = CM_LINK_SPEED_UNKNOWN;
		m_mauMode   = CM_LINK_MODE_UNKNOWN;
	}

	PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::SetMauState(): Update MAU states, portID(%u) State(%#x) Speed(%#x) Mode(%#x)",
		m_portID,
		m_mauStatus,
		m_mauSpeed,
		m_mauMode );
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::SetSyncState( LSA_UINT16 syncLedInfo, LSA_BOOL isRateValid, LSA_UINT8 rcvSyncPrio, LSA_UINT8 localPrio, LSA_UINT8 pllState )
{
	m_syncState    = syncLedInfo;
	m_isRateValid  = isRateValid;
	m_rcvSyncPrio  = rcvSyncPrio;
	m_localPrio    = localPrio;
	m_pllState     = pllState;
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintenanceStatus )
{
	// Update the BGZD State from Alarm info, and trigger upper update
	// note: if A.R.P is set, this information belongs to the previous PDev-configuration
	m_bgzdStore.UpdateFromAlarm( hasDiag, maintenanceStatus );
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::SetState( LSA_UINT16 state )
{
	// Reset and update BGZD from new submod state
	m_bgzdStore.UpdateFromSubslotState( state );
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::SetARP()
{
	// Set ARP in BGZD state
	m_bgzdStore.SetARP( LSA_TRUE );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL CPDSubslot::IsMapped()
{
	// mapped to a number 0x8xxx ?
	LSA_BOOL result = ( m_subslotNr != 0 ) ? LSA_TRUE : LSA_FALSE;
	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CPDSubslot::IsLinkUp()
{
	LSA_BOOL result = LSA_FALSE;

	if (( m_mauStatus != CM_LINK_STATUS_UNKNOWN ) &&
		( m_mauStatus != CM_LINK_STATUS_DOWN    ))
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
LSA_VOID CPDSubslot::RecordRead( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT( p_rb != 0 );

	ReadRecord( p_rb );
}
/*----------------------------------------------------------------------------*/
LSA_VOID CPDSubslot::RecordWrite( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT( p_rb != 0 );

    WriteRecord( p_rb );
}



/*----------------------------------------------------------------------------*/
/* PD (CM-PD) Request functions                                               */
/*----------------------------------------------------------------------------*/

void CPDSubslot::ReadRecord( PND_RQB_PTR_TYPE p_rb ) //( LSA_UINT32 recIdx, LSA_UINT16 dataSize )
{
	LSA_HANDLE_TYPE handle = GetPnioHandle();

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_PRM_READ, handle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_RECORD_READ_WRITE_PTR_TYPE pRead = &p_rb->args.rec_read;

	PND_PD_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::ReadRecord(): PD PRM READ record, slot(%#x) sub(%#x) portID(%u) recIdx(%#x) dataSize(%#u) rqb(%08x)", 
		m_slotNr, m_subslotNr, m_portID, pRead->RecordIndex, pRead->Length, pRQB );

	pRQB->args.pd.prm_rdwr->slot_nr            = m_slotNr;
	pRQB->args.pd.prm_rdwr->subslot_nr         = m_subslotNr;
	pRQB->args.pd.prm_rdwr->record_index       = pRead->RecordIndex;
	pRQB->args.pd.prm_rdwr->record_data_length = pRead->Length;

	if ( pRead->Length > 0 ) // Read with buffer ?
	{
		pRQB->args.pd.prm_rdwr->record_data = (LSA_UINT8*)pnd_cm_alloc_upper_mem( (LSA_UINT16)pRead->Length, &m_pndHandle );
		PND_ASSERT( pRQB->args.pd.prm_rdwr->record_data != 0);
	}

	// store PND RQB in user ID for done handling
    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

	eps_user_request_lower( pRQB, this, CPDSubslot::ReadRecordDone );
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::WriteRecord( PND_RQB_PTR_TYPE p_rb )
{
	LSA_HANDLE_TYPE handle = GetPnioHandle();

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_PRM_WRITE, handle, &m_pndHandle, 0 );
	PND_ASSERT(pRQB);

	PND_RECORD_READ_WRITE_PTR_TYPE pWrite = &p_rb->args.rec_write;

	PND_PD_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::WriteRecord(): PD PRM WRITE record, slot(%#x) sub(%#x) portID(%u) recIdx(%#x) dataSize(%#u) rqb(%08x)", 
		m_slotNr, m_subslotNr, m_portID, pWrite->RecordIndex, pWrite->Length, pRQB );

	pRQB->args.pd.prm_rdwr->slot_nr            = m_slotNr;
	pRQB->args.pd.prm_rdwr->subslot_nr         = m_subslotNr;
	pRQB->args.pd.prm_rdwr->record_index       = pWrite->RecordIndex;
	pRQB->args.pd.prm_rdwr->record_data_length = pWrite->Length;
	pRQB->args.pd.prm_rdwr->record_data        = 0;

	if ( pWrite->Length > 0 ) // Write with buffer ?
	{
		PND_ASSERT( pWrite->pBuffer != 0 );

		pRQB->args.pd.prm_rdwr->record_data = (LSA_UINT8*)pnd_cm_alloc_upper_mem( (LSA_UINT16)pWrite->Length, &m_pndHandle );
		PND_ASSERT( pRQB->args.pd.prm_rdwr->record_data != 0);

		pnd_memcpy( pRQB->args.pd.prm_rdwr->record_data, pWrite->pBuffer, pWrite->Length );
	}

	// store PND RQB in user ID for done handling
    LSA_RQB_SET_USER_ID_PTR( pRQB, p_rb );

	eps_user_request_lower( pRQB, this, CPDSubslot::WriteRecordDone );
}

/*----------------------------------------------------------------------------*/
/* PD (CM-PD) callback functions                                              */
/*----------------------------------------------------------------------------*/

void CPDSubslot::ReadRecordDone( void* p_inst, void* p_rb )
{
	CPDSubslot *pSub           = static_cast<CPDSubslot*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pSub != 0);
	PND_ASSERT( pRQB != 0);

	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_PD_PRM_READ );

	LSA_UINT32 pnioErr = pRQB->args.pd.prm_rdwr->cm_pnio_err;
	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );

	// resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

	PND_PD_TRACE_08( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::ReadRecordDone():PD PRM Read done, rsp(%#x) PNIO-ERR(%#x) slot(%#x) sub(%#x) portID(%u) recIdx(%#x) len(%#x) rqb(%08x)", 
		CM_RQB_GET_RESPONSE(pRQB),
		pnioErr,
		pSub->m_slotNr,
		pSub->m_subslotNr,
		pSub->m_portID,
		pRQB->args.pd.prm_rdwr->record_index,
		pRQB->args.pd.prm_rdwr->record_data_length,
		pRQB );

	// Copy CM PNIO state fields to PND RQB
	pPndRQB->args.rec_read.err.AddValue1 = pRQB->args.pd.prm_rdwr->ret_val_1;
    pPndRQB->args.rec_read.err.AddValue1 = pRQB->args.pd.prm_rdwr->ret_val_2;
    pPndRQB->args.rec_read.err.ErrCode   = CM_PNIO_ERR_FETCH_CODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrDecode = CM_PNIO_ERR_FETCH_DECODE( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode1  = CM_PNIO_ERR_FETCH_CODE_1( pnioErr );
    pPndRQB->args.rec_read.err.ErrCode2  = CM_PNIO_ERR_FETCH_CODE_2( pnioErr );

	if (( rsp == CM_OK ) && ( pnioErr == CM_PNIO_ERR_NONE ))
    {
		pPndRQB->args.rec_read.RecordIndex = pRQB->args.pd.prm_rdwr->record_index;

        PND_ASSERT( pPndRQB->args.rec_read.Length >= pRQB->args.pd.prm_rdwr->record_data_length);

		pnd_memcpy( pPndRQB->args.rec_read.pBuffer, pRQB->args.pd.prm_rdwr->record_data, pRQB->args.pd.prm_rdwr->record_data_length);

		pPndRQB->args.rec_read.Length = pRQB->args.pd.prm_rdwr->record_data_length;

        pPndRQB->args.rec_read.pnio_err = PNIO_OK;

        pnd_pnio_user_request_iobc_done( pPndRQB );
    }
    else
    {
        // complete record reading request from IOBC with error
        pPndRQB->args.rec_read.Length   = 0;  // No data avaliable
        pPndRQB->args.rec_read.pnio_err = PNIO_ERR_INTERNAL;

        pnd_pnio_user_request_iobc_done( pPndRQB );
    }

	if ( pRQB->args.pd.prm_rdwr->record_data != 0 )
	{
		pnd_cm_free_upper_mem( pRQB->args.pd.prm_rdwr->record_data, &pSub->m_pndHandle );

		pRQB->args.pd.prm_rdwr->record_data = 0;
	}

	pnd_cm_free_rqb( pRQB, &pSub->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void CPDSubslot::WriteRecordDone( void* p_inst, void* p_rb )
{
	CPDSubslot *pSub           = static_cast<CPDSubslot*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pSub != 0);
	PND_ASSERT( pRQB != 0);

	PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_PD_PRM_WRITE );

	LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE( pRQB );
	LSA_UINT32 recIdx     = pRQB->args.pd.prm_rdwr->record_index;
	LSA_UINT32 pnioErr    = pRQB->args.pd.prm_rdwr->cm_pnio_err;
	 
	// resstore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

	PND_PD_TRACE_07( pSub->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDSubslot::WriteRecordDone(): PD PRM Write done, rsp(%#x) PNIO-ERR(%#x) portID(%u) slot(%#x) sub(%#x) recIdx(%#x) rqb(%08x)", 
		rsp,
		pnioErr,
		pSub->m_portID,
		pSub->m_slotNr,
		pSub->m_subslotNr,
		recIdx,
		pRQB );

	// Copy CM PNIO state fields to PND RQB
	pPndRQB->args.rec_write.err.AddValue1 = pRQB->args.pd.prm_rdwr->ret_val_1;
    pPndRQB->args.rec_write.err.AddValue2 = pRQB->args.pd.prm_rdwr->ret_val_2;
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

	if ( pRQB->args.pd.prm_rdwr->record_data != 0 )
	{
		pnd_cm_free_upper_mem( pRQB->args.pd.prm_rdwr->record_data, &pSub->m_pndHandle );

		pRQB->args.pd.prm_rdwr->record_data = 0;
	}

	pnd_cm_free_rqb( pRQB, &pSub->m_pndHandle );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
