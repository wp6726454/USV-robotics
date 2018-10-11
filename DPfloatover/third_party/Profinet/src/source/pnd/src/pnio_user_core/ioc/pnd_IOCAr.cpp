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
/*  F i l e               &F: pnd_IOCAr.cpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of controller AR class                                    */
/*                                                                           */
/*****************************************************************************/


#define LTRC_ACT_MODUL_ID   3206
#define PND_MODULE_ID       3206

/////////////////////////////////////////////////////////////////////////////

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
#include <iostream>
#include <sstream>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_ValueHelper.h"
#include "pnd_UserNode.h"
#include "pnd_UserStore.h"

#include "pnd_ParamStore.h"
#include "pnd_IOCAr.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CIOCAr
/////////////////////////////////////////////////////////////////////////////

CIOCAr::CIOCAr( PND_HANDLE_PTR_TYPE pHandle, LSA_VOID* arcb_ptr )
{
	PND_ASSERT( pHandle != 0 );
	PND_ASSERT ( arcb_ptr != 0 );

	CM_UPPER_ARCB_ADD_PTR_TYPE pARCB = (CM_UPPER_ARCB_ADD_PTR_TYPE)arcb_ptr;

	m_arNr       = pARCB->ar_nr;
	m_pndHandle  = *pHandle;
	m_properties = pARCB->ar_properties;
	m_type       = pARCB->ar_type;

	m_arState         = PND_IOC_AR_STATE_UNKNOWN;
	m_sessionKey      = 0;
	m_UUID            = clrpc_uuid_nil_uuid;

	// Start DS state initial (see also CM_EVENT_APDU_STATUS_INITIAL_VALUE)
	// Note: primary/backup and sysred Ok/problem only used with SRD
	m_arDS.is_run        = LSA_TRUE;
	m_arDS.is_station_ok = LSA_TRUE;
	m_arDS.is_primary    = LSA_TRUE;
	m_arDS.is_sysred_ok  = LSA_TRUE;

	pnd_memset( &m_consumer, 0, sizeof(PND_PNUC_IOCR_TYPE));
	pnd_memset( &m_provider, 0, sizeof(PND_PNUC_IOCR_TYPE));

	PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE_LOW,
		"CIOCAr::CIOCAr(): Creating IOC AR, arNr(%u), arPorp(0x%08x), arType(%#x) arState(%u) sesKey(%u)",
		m_arNr, m_properties, m_type, m_arState, m_sessionKey );
}

/*----------------------------------------------------------------------------*/
CIOCAr::~CIOCAr()
{
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CIOCAr::CreateIOCR( LSA_VOID* iocr_ptr, LSA_VOID* iodu_ptr )
{
	CM_UPPER_ARCB_IOCR_PTR_TYPE pIOCR = (CM_UPPER_ARCB_IOCR_PTR_TYPE)iocr_ptr;
	PND_ASSERT( pIOCR != 0 );

	PND_PI_ALLOC_PTR_TYPE pIODU = (PND_PI_ALLOC_PTR_TYPE)iodu_ptr;
	PND_ASSERT( pIODU != 0 );

	LSA_BOOL isRtc3 = LSA_FALSE;

	// check CR-Type, seen from controller
	if (( pIOCR->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK ) == CM_IOCR_PROP_RT_CLASS_3 )
	{
		isRtc3 = LSA_TRUE;
	}

	// Note: the group is the ACP group the provType depends on AR-Type
	//       the settings are only relevant for providers
	LSA_UINT8 provType   = ( IsSRD() ) ? EDD_PROVADD_TYPE_SYSRED : EDD_PROVADD_TYPE_DEFAULT;
	LSA_BOOL  isProvider = LSA_FALSE;

	switch ( pIOCR->iocr_type ) // IOCR type (seen from controller)
	{
	case CM_IOCR_TYPE_INPUT:   // UC consumer
		{
			PND_ASSERT( m_consumer.arcb_iocr_ptr == 0 );

			// prepare for provider IOCR
			pIODU->consumer_buffer.buffer_handle       = 0;
			pIODU->consumer_buffer.is_provider         = isProvider;
			pIODU->consumer_buffer.partial_length      = pIOCR->partial_length;
			pIODU->consumer_buffer.apdu_status_length  = pIOCR->apdu_status_length;

			// Prepare internal IOCR properties
			m_consumer.iocr_ref_nr = pIOCR->iocr_ref;
			m_consumer.iodu_handle = 0;
			m_consumer.m_piProperties.is_provider        = isProvider;
			m_consumer.m_piProperties.is_rtc3            = isRtc3;
			m_consumer.m_piProperties.partial_length     = pIOCR->partial_length;
			m_consumer.m_piProperties.apdu_status_length = pIOCR->apdu_status_length;
			m_consumer.m_piProperties.group_id           = PND_IOC_GROUP_ID;
			m_consumer.m_piProperties.prov_type          = provType;
			m_consumer.m_piProperties.frame_id           = pIOCR->frame_id;

			if ( pIOCR->subframe_data != 0 )  // Subframe ?
			{ 
				pIODU->consumer_buffer.is_dfp = LSA_TRUE;

				m_consumer.m_piProperties.is_dfp          = LSA_TRUE;
				m_consumer.m_piProperties.subframe_offset = pIOCR->subframe_offset;
				m_consumer.m_piProperties.subframe_data   = pIOCR->subframe_data;
			}
			else
			{
				pIODU->consumer_buffer.is_dfp = LSA_FALSE;

				m_consumer.m_piProperties.is_dfp          = LSA_FALSE;
				m_consumer.m_piProperties.subframe_offset = EDD_SFOFFSET_INVALID;
				m_consumer.m_piProperties.subframe_data   = 0;
			}

			// Save Ref for update PI properties from IODU
			m_consumer.arcb_iocr_ptr = pIOCR;
		}
		break;
	case CM_IOCR_TYPE_OUTPUT:  // UC provider
		{
			PND_ASSERT( m_provider.arcb_iocr_ptr == 0 );

			isProvider = LSA_TRUE;

			// prepare for provider IOCR
			//pIODU->provider_buffer.init_value          = 0; //BaA delete
			pIODU->provider_buffer.buffer_handle       = 0;
			pIODU->provider_buffer.is_provider         = isProvider;
			pIODU->provider_buffer.partial_length      = pIOCR->partial_length;
			//pIODU->provider_buffer.partial_offset      = 0;  // Fixed setting //BaA delete
			pIODU->provider_buffer.apdu_status_length  = pIOCR->apdu_status_length;

			// Prepare internal IOCR properties
			m_provider.iocr_ref_nr = pIOCR->iocr_ref;
			m_provider.iodu_handle = 0;
			m_provider.m_piProperties.is_provider        = isProvider;
			m_provider.m_piProperties.is_rtc3            = isRtc3;
			m_provider.m_piProperties.partial_length     = pIOCR->partial_length;
			m_provider.m_piProperties.apdu_status_length = pIOCR->apdu_status_length;
			m_provider.m_piProperties.group_id           = PND_IOC_GROUP_ID;
			m_provider.m_piProperties.prov_type          = provType;
			m_provider.m_piProperties.frame_id           = pIOCR->frame_id;

			if ( pIOCR->subframe_data != 0 )  // Subframe ?
			{ 
				pIODU->provider_buffer.is_dfp = LSA_TRUE;

				m_provider.m_piProperties.is_dfp          = LSA_TRUE;
				m_provider.m_piProperties.subframe_offset = pIOCR->subframe_offset;
				m_provider.m_piProperties.subframe_data   = pIOCR->subframe_data;
			}
			else
			{
				pIODU->provider_buffer.is_dfp = LSA_FALSE;

				m_provider.m_piProperties.is_dfp          = LSA_FALSE;
				m_provider.m_piProperties.subframe_offset = EDD_SFOFFSET_INVALID;
				m_provider.m_piProperties.subframe_data   = 0;
			}
			// Save Ref for update PI properties from IODU
			m_provider.arcb_iocr_ptr = pIOCR;
		}
		break;
	case CM_IOCR_TYPE_MULTICAST_PROVIDER:
	case CM_IOCR_TYPE_MULTICAST_CONSUMER:
	default:
		{
			// Not expected IOCR type
			PND_FATAL( "CIOCAr::CreateIOCR not expected IOCR type configured" );
		}
		break;
	}

	PND_IOC_TRACE_11( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CIOCAr::CreateIOCR(): refNr=(%u) type(%u) isProv(%u) isRTC3(%u) prop(%#x) dataLen=(%u) parOff(%u) parLen(%u) apduLen(%u) sfOff(%u) sfData(%u)", 
		pIOCR->iocr_ref, 
		pIOCR->iocr_type,
		isProvider,
		isRtc3,
		pIOCR->iocr_properties, 
		pIOCR->data_length, 
		pIOCR->partial_offset, 
		pIOCR->partial_length, 
		pIOCR->apdu_status_length,
		pIOCR->subframe_offset,
		pIOCR->subframe_data );

	return ( isProvider );
}

/////////////////////////////////////////////////////////////////////////////
// Accessors
/////////////////////////////////////////////////////////////////////////////

LSA_VOID* CIOCAr::GetProviderHandle()
{
	return( m_provider.iodu_handle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID* CIOCAr::GetConsumerHandle()
{
	return( m_consumer.iodu_handle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCAr::UpdateImageProperties( LSA_VOID_PTR_TYPE pProp )
{
	// Update EDD properties from PI ALLOC
	// Note: used for CM request in ARCB
	PND_PI_ALLOC_PTR_TYPE pIODU = (PND_PI_ALLOC_PTR_TYPE)pProp;
	PND_ASSERT( pIODU != 0 );

	// Update EDD properties for consumer (INPUT CR) from IODU result
	// Note: after set IOCR ref from ARCB is removed (part of outside ARCB)
	CM_UPPER_ARCB_IOCR_PTR_TYPE pIOCR = (CM_UPPER_ARCB_IOCR_PTR_TYPE)m_consumer.arcb_iocr_ptr;
	PND_ASSERT( pIOCR != 0 );

	PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CIOCAr::UpdateImageEddProperties(): PI consumer, refNr=(%u) edd_data_offset(%#x) edd_properties(%#x) edd_cr_number(%#x) edd_forward_mode(%#x) edd_ct_outdatastart(%#x)", 
		m_consumer.iocr_ref_nr,
		pIODU->consumer_buffer.edd_data_offset,
		pIODU->consumer_buffer.edd_properties,
		(LSA_UINT16)pIODU->consumer_buffer.edd_cr_number,
		(LSA_UINT8)pIODU->consumer_buffer.edd_forward_mode,
		pIODU->consumer_buffer.edd_ct_outdatastart );

	cm_arcb_iocr_set_edd( 
		pIOCR,
		pIODU->consumer_buffer.edd_data_offset,
		pIODU->consumer_buffer.edd_properties,
		(LSA_UINT16)pIODU->consumer_buffer.edd_cr_number,
		(LSA_UINT8)pIODU->consumer_buffer.edd_forward_mode,
		pIODU->consumer_buffer.edd_ct_outdatastart );

	m_consumer.iodu_handle   = pIODU->consumer_buffer.buffer_handle;
	m_consumer.arcb_iocr_ptr = 0;

	// Update EDD properties for provider (OUTPUT CR) from IODU result
	// Note: after set IOCR ref from ARCB is removed (part of outside ARCB)
	pIOCR = (CM_UPPER_ARCB_IOCR_PTR_TYPE)m_provider.arcb_iocr_ptr;
	PND_ASSERT( pIOCR != 0 );

	PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CIOCAr::UpdateImageEddProperties(): PI provider, refNr=(%u) edd_data_offset(%#x) edd_properties(%#x) edd_cr_number(%#x) edd_forward_mode(%#x) edd_ct_outdatastart(%#x)", 
		m_provider.iocr_ref_nr,
		pIODU->provider_buffer.edd_data_offset,
		pIODU->provider_buffer.edd_properties,
		(LSA_UINT16)pIODU->provider_buffer.edd_cr_number,
		(LSA_UINT8)pIODU->provider_buffer.edd_forward_mode,
		pIODU->provider_buffer.edd_ct_outdatastart );

	cm_arcb_iocr_set_edd( 
		pIOCR,
		pIODU->provider_buffer.edd_data_offset,
		pIODU->provider_buffer.edd_properties,
		(LSA_UINT16)pIODU->provider_buffer.edd_cr_number,
		(LSA_UINT8)pIODU->provider_buffer.edd_forward_mode,
		pIODU->provider_buffer.edd_ct_outdatastart );

	m_provider.iodu_handle   = pIODU->provider_buffer.buffer_handle;
	m_provider.arcb_iocr_ptr = 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_UINT16 CIOCAr::GetSessionKey()
{
	return ( m_sessionKey );
}

/*----------------------------------------------------------------------------*/
PND_IOC_AR_STATE_TYPE CIOCAr::GetArState()
{
	return ( m_arState );
}


/*----------------------------------------------------------------------------*/
PND_AR_DS_TYPE CIOCAr::GetArDataState()
{
	return ( m_arDS );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCAr::GetArType()
{
	return ( m_type );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CIOCAr::GetArNumber()
{
	return ( m_arNr );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCAr::SetSessionKey( LSA_UINT16 sessionKey )
{
	m_sessionKey = sessionKey;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCAr::SetArState( PND_IOC_AR_STATE_TYPE state )
{
	m_arState = state;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCAr::SetUUID( LSA_UINT8* p_uuid )
{
	PND_ASSERT( p_uuid != 0 );

	m_UUID = *((CLRPC_UUID_PTR_TYPE)p_uuid);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCAr::SetDataStatus( LSA_UINT16 cmDataStatus )
{
	// Note: only the differences are signaled by CM-CL
	if (( cmDataStatus & CM_EVENT_APDU_STATUS_RUN) == CM_EVENT_APDU_STATUS_RUN )
	{
		m_arDS.is_run = LSA_TRUE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_STOP) == CM_EVENT_APDU_STATUS_STOP )
	{
		m_arDS.is_run = LSA_FALSE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_STATION_OK) == CM_EVENT_APDU_STATUS_STATION_OK )
	{
		m_arDS.is_station_ok = LSA_TRUE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_STATION_PROBLEM) == CM_EVENT_APDU_STATUS_STATION_PROBLEM )
	{
		m_arDS.is_station_ok = LSA_FALSE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_PRIMARY) == CM_EVENT_APDU_STATUS_PRIMARY )
	{
		m_arDS.is_primary = LSA_TRUE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_BACKUP) == CM_EVENT_APDU_STATUS_BACKUP )
	{
		m_arDS.is_primary = LSA_FALSE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_SYSRED_OK) == CM_EVENT_APDU_STATUS_SYSRED_OK )
	{
		m_arDS.is_sysred_ok = LSA_TRUE;
	}

	if (( cmDataStatus & CM_EVENT_APDU_STATUS_SYSRED_PROBLEM) == CM_EVENT_APDU_STATUS_SYSRED_PROBLEM )
	{
		m_arDS.is_sysred_ok = LSA_FALSE;
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL CIOCAr::IsSRD()
{
	LSA_BOOL result = ( m_type == CM_AR_TYPE_SINGLE_SYSRED ) ? LSA_TRUE : LSA_FALSE;

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CIOCAr::IsINDATA()
{
	LSA_BOOL result = ( m_arState == PND_IOC_AR_STATE_INDATA ) ? LSA_TRUE : LSA_FALSE;

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CIOCAr::IsPassivate()
{
	LSA_BOOL result = ( m_arState == PND_IOC_AR_STATE_PASSIVE ) ? LSA_TRUE : LSA_FALSE;

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CIOCAr::IsRecordReadAllowed()
{
	LSA_BOOL result = LSA_FALSE;

	if (( m_arState == PND_IOC_AR_STATE_PREDATA ) ||
		( m_arState == PND_IOC_AR_STATE_INDATA ))
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
