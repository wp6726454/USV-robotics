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
/*  F i l e               &F: OHAEventHandler.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of OHA Event Handler class                                */
/*  (Listener and Handler for OHA Events)                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3402
#define PND_MODULE_ID       3402

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
#include "pnd_OHAEventHandler.h"
#include "pnd_OHAUser.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// COHAEventHandler
/////////////////////////////////////////////////////////////////////////////

COHAEventHandler::COHAEventHandler( PND_HANDLE_PTR_TYPE pHandle, COHAUser *pUser )
{
	PND_ASSERT( pHandle != 0 );
	PND_ASSERT( pUser != 0 );

	m_pndHandle  = *pHandle;
	m_pUser      = pUser;
	m_isStopping = LSA_FALSE;
    m_responseRQB= 0;
	// Register the event handler methodes dependend on Config
	initMap();
}

/*----------------------------------------------------------------------------*/
COHAEventHandler::~COHAEventHandler()
{
}

/////////////////////////////////////////////////////////////////////////////
// Event-Handling methodes
/////////////////////////////////////////////////////////////////////////////

void COHAEventHandler::ErrorEventHandling(OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );

	PND_OHA_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
		"COHAEventHandler::ErrorEventHandling(): Not handled event received, OPC(%u) rqb(%08x)", 
		OHA_RQB_GET_OPCODE( pRQB ),
		pRQB );

	PND_FATAL( "COHAEventHandler::ErrorEventHandling() Not handled OHA Event received" );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::DcpIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	CValueHelper valObj;

	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_DCP_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	OHA_UPPER_DCP_INDICATION_PTR_TYPE pDCP = &pRQB->args.DcpInd;

	PND_OHA_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::DcpIndication(): OHA DCP.IND rsp(%#x) DCP-event (%u) Type(%u) SubType(%u) Options(%#x) Count(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pDCP->Event,
		pDCP->Type,
		pDCP->SubType,
		pDCP->Options, 
		pDCP->Count );

	PND_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK );

	if (( pDCP->Options & OHA_CTRL_IP_SUITE ) == OHA_CTRL_IP_SUITE )
	{
		if( pDCP->Event == OHA_EVENT_QUERY_SET ) // OHA IP suite user query
		{
			OHA_RQB_SET_OPCODE( pRQB, OHA_OPC_DCP_RESPONSE );
			pDCP->CheckIpEnabled = LSA_TRUE;
			
			if( m_pUser->GetIpAddrValidationState() /* IPAddressValidation     == 0xFFFF */ )
			{
				pDCP->Event = OHA_MODE_SET;
			}
			else
			{
				if(    pDCP->pStationData->LocalIpAddr == LSA_NULL
				    && pDCP->pStationData->LocalSubnetMask == LSA_NULL
				    && pDCP->pStationData->DefaultRouterAddr == LSA_NULL )
				{
					pDCP->Event = OHA_MODE_NO_SET;
				}
				else
				{
					pDCP->Event = OHA_MODE_EMERGENCY_SET;
				}
			}
		}

		if( pDCP->pStationData != 0 )
		{
			PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
				"COHAEventHandler::DcpIndication():   IP(%#x), NetMask(%#x), NextGatway(%#x)",
				pDCP->pStationData->LocalIpAddr,
				pDCP->pStationData->LocalSubnetMask,
				pDCP->pStationData->DefaultRouterAddr );
		}
	}
	
	if( ( pDCP->Options & OHA_CTRL_STATION_NAME ) == OHA_CTRL_STATION_NAME )
	{
		if( pDCP->Event == OHA_EVENT_QUERY_SET ) // OHA IP suite user query
		{
			OHA_RQB_SET_OPCODE( pRQB, OHA_OPC_DCP_RESPONSE );
			pDCP->CheckIpEnabled = LSA_TRUE;

			if( m_pUser->GetNameOfStationValidationState() /* NameOfStationValidation == 0xFFFF */ )
			{
				pDCP->Event = OHA_MODE_SET;
			}
			else
			{
				if( pDCP->pStationData->pStationName == LSA_NULL )
				{
					pDCP->Event = OHA_MODE_NO_SET;
				}
				else
				{
					pDCP->Event = OHA_MODE_EMERGENCY_SET;
				}
			}
		}
	}

	// reprovide RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::DcpQueryResponse( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_DCP_RESPONSE );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	OHA_UPPER_DCP_INDICATION_PTR_TYPE pDCP = &pRQB->args.DcpInd;

	PND_OHA_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::DcpQueryResponse(): OHA Query Response DCP.IND rsp(%#x) DCP-event(%u) Type(%u) SubType(%u) Options(%#x) RemaOptions(%#x) Count(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pDCP->Event,
		pDCP->Type,
		pDCP->SubType,
		pDCP->Options,
		pDCP->RemaOptions,
		pDCP->Count );

	PND_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK );


	// reprovide RQB for new DCP indication
	OHA_RQB_SET_HANDLE( pRQB, handle );
	OHA_RQB_SET_OPCODE( pRQB, OHA_OPC_DCP_INDICATION );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::LldpIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	CValueHelper valObj;

	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_LLDP_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	OHA_UPPER_LLDP_INDICATION_PTR_TYPE pLLDP = &pRQB->args.LldpInd;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::LldpIndication(): OHA LLDP.IND rsp(%#x) Status(%#x) PortId(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pLLDP->Status,
		pLLDP->PortId );

	PND_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK );

	// reprovide RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::HelloIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	CValueHelper valObj;

	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_HELLO_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	PND_OHA_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
		"COHAEventHandler::HelloIndication(): rsp(%#x) pRQB(%#x)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB );

	// reprovide RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );
	pRQB->args.HelloInd.StationNameLen = OHA_MAX_STATION_NAME_LEN;

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::AddrInfoIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	CValueHelper valObj;

	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_ADDR_INFO_IND );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	OHA_UPPER_ADDR_INFO_PTR_TYPE pAddr = &pRQB->args.AddrInfo;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::AddrInfoIndication(): OHA ADDR INFO.IND rsp(%#x) IpConflict(%u) , CurIpIsEmergency(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pAddr->CurrentIPSuiteConflictDetected,
		pAddr->CurrentIPSuiteIsEmergency );

	PND_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK );
	// Update IF settings
	m_pUser->SetInterfaceData(
		pAddr->CurrentIPSuite.IpAddr,
		pAddr->CurrentIPSuite.NetMask,
		pAddr->CurrentIPSuite.Gateway,
		pAddr->CurrentIPSuiteConflictDetected,
		pAddr->NoS.pName,
		pAddr->NoS.Length,
		LSA_FALSE );

	// reprovide RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::PrmIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_PRM_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	OHA_UPPER_PRM_INDICATION_PTR_TYPE pPRM = &pRQB->args.PrmInd;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::PrmIndication(): OHA PRM.IND rsp(%#x) DiagCount(%u) Port(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pPRM->diag_cnt,
		pPRM->edd_port_id );

	PND_ASSERT( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK );

	// reprovide RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::RemaIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_REMA_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::RemaIndication(): OHA REMA.IND rsp(%#x) DataTag(%u) Datasize(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.RemaData.data_tag,
		pRQB->args.RemaData.data_length );

	if ( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK )
	{
		// Trigger reading REMA record for data type 
		switch( pRQB->args.RemaData.data_tag )
		{
		case OHA_PDEV_DATA:     // PDev records 
			break;
		case OHA_STATION_DATA:  // IP-Suite, NameOfStation
			{
				if(pRQB->args.RemaData.data_length != 0 )
				{
					m_pUser->RemaRead( pRQB->args.RemaData.index, pRQB->args.RemaData.data_length );
                    m_responseRQB = pRQB;
				}
			}
			break;
		case OHA_SNMP_DATA:     // SNMP settings
			{
				if(pRQB->args.RemaData.data_length != 0 && 
				   pRQB->args.RemaData.index       == OHA_RECORD_INDEX_MULTIPLE_SNMP)
				{
					m_pUser->RemaRead( pRQB->args.RemaData.index, pRQB->args.RemaData.data_length );
                    m_responseRQB = pRQB;
				}
			}
			break;
		default:
			{
				PND_FATAL( "COHAEventHandler::RemaIndication() unknown OHA REMA tag received");
			}
			break;
		}
	}

    if( !m_responseRQB )
    {
        // reprovide RQB
        OHA_RQB_SET_HANDLE( pRQB, handle );

        reprovide( pRQB );
    }
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::LldpConfigIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_LLDP_CONFIG_INDICATION );

	LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::LldpConfigIndication(): OHA LLDP-MIB.IND rsp(%#x) mode(%u) options(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.LldpConfig.Mode,
		pRQB->args.LldpConfig.Options );

	// reprovide Indication RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::LldpPortConfigIndication( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( OHA_RQB_GET_OPCODE( pRQB ) == OHA_OPC_LLDP_PORT_CONFIG_INDICATION );

	LSA_HANDLE_TYPE  handle   = m_pUser->GetPnioHandle();

	PND_OHA_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::LldpPortConfigIndication(): OHA LLDP-MIB-PORT.IND rsp(%#x) mode(%u) portID(%u) options(%u)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.LldpPortConfig.Mode,
		pRQB->args.LldpPortConfig.PortID,
		pRQB->args.LldpPortConfig.PortOptions );

	// reprovide Indication RQB
	OHA_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/////////////////////////////////////////////////////////////////////////////
// Requests
/////////////////////////////////////////////////////////////////////////////

void COHAEventHandler::StartListening()
{
	PND_ASSERT( m_pUser != 0 );

	LSA_HANDLE_TYPE  handle   = m_pUser->GetPnioHandle();

	PND_OHA_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"Providing OHA event resources, PNIO-IFNr(%u)", m_pndHandle.pnio_if_nr );

	m_isStopping = LSA_FALSE;

	// Provide LLDP event resources
	for ( int lvi = 0; lvi < 5; lvi++ )
	{
		OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_INDICATION, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	// Provide DCP event resources
	for ( int lvi = 0; lvi < 5; lvi++ )
	{
		OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_DCP_INDICATION, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	// Provide HELLO event resources
	for ( int lvi = 0; lvi < 2; lvi++ )
	{
		OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_HELLO_INDICATION, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	// Provide PRM event resources (2 for IF and each port)
	for ( int port = 0; port <= OHA_CFG_MAX_PORTS; port++ )
	{
		for ( int nr = 0; nr < 2; nr++ )
		{
			OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_PRM_INDICATION, handle, &m_pndHandle );
			PND_ASSERT(pRQB);

			pRQB->args.PrmInd.edd_port_id = (LSA_UINT16)(port);

			eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
		}
	}

	// Provide ADDR info event resources
	for ( int lvi = 0; lvi < 1; lvi++ )
	{
		OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_ADDR_INFO_IND, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	// Provide REMA event resources
	for ( int lvi = 0; lvi < 1; lvi++ )
	{
		OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_REMA_INDICATION, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		// Note: set for Notify only, the data is read over record with data_length
		pRQB->args.RemaData.index       = 0;
		pRQB->args.RemaData.data_length = 0; // want needed length
		pRQB->args.RemaData.data        = LSA_NULL;
		pRQB->args.RemaData.data_tag    = 0xFF;

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	// Provide LLDP Config event resources for IF
	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_CONFIG_INDICATION, handle, &m_pndHandle );
	PND_ASSERT(pRQB);

	eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );

	// Provide LLDP Config event resources for each port)
	for ( int port = 1; port <= OHA_CFG_MAX_PORTS; port++ )
	{
		pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_PORT_CONFIG_INDICATION, handle, &m_pndHandle );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}

	PND_OHA_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::StartListening(): Providing OHA resources done, PNIO-IFNr(%u)", 
		m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::StopListening()
{
	// Note: i.E.: is called if closing the channel is requested
	//       to prevent reproviding IND RQBs after Close
	m_isStopping = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::HandleEvent( LSA_OPCODE_TYPE opcode, OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	// Delegate to the registered common Event Handler method
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( ( opcode > 0 ) && (opcode < PND_OHA_OPC_MAX ));

	(this->*m_handler[opcode])( pRQB );
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////////////////////////////////////

void COHAEventHandler::EventReceived(void* p_inst, void* p_rb)
{
	COHAEventHandler *eventObj  = static_cast<COHAEventHandler*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( eventObj           != 0 );
	PND_ASSERT( pRQB               != 0 );
	PND_ASSERT( eventObj->m_pUser  != 0 );

	if ( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_OK_CANCEL )
	{
		// IND resource is canceled -- stop requesting in case of shutdown
		PND_ASSERT( eventObj->m_pUser->IsShutdown() );

		PND_OHA_TRACE_02( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
			"COHAEventHandler::EventReceived(): OHA RQB cancelled, PNIO-IF(%u) pRQB(0x%08x)", 
			eventObj->m_pndHandle.pnio_if_nr, pRQB );

		pnd_oha_free_rqb( pRQB, &eventObj->m_pndHandle );
	}
	else
	{
		// Handle the event --> use event function map for handle the event
		LSA_OPCODE_TYPE opcode = OHA_RQB_GET_OPCODE( pRQB );

		// Prepare response and call event handler for Opcode
		LSA_HANDLE_TYPE handle = eventObj->m_pUser->GetPnioHandle();

		// Prepare response and call event handler for Opcode
		OHA_RQB_SET_HANDLE( pRQB, handle );
		eventObj->HandleEvent( opcode, pRQB );
	}
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::RemaIndicationDone()
{
	LSA_HANDLE_TYPE  handle   = m_pUser->GetPnioHandle();

    PND_OHA_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAEventHandler::RemaIndicationDone(), PNIO-IFNr(%u)", m_pndHandle.pnio_if_nr );

	// reprovide RQB
	OHA_RQB_SET_HANDLE( m_responseRQB, handle );

	reprovide( m_responseRQB );

    m_responseRQB = 0;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void COHAEventHandler::initMap()
{
	// Initialize the Event-Handler list
	for ( int lvi = 0; lvi < PND_OHA_OPC_MAX; lvi++ )
	{
		m_handler[lvi] = &COHAEventHandler::ErrorEventHandling;
	}

	// Set the OHA Event methodes corresponding to OHA event numbers
	m_handler[OHA_OPC_DCP_INDICATION]              = &COHAEventHandler::DcpIndication;
	m_handler[OHA_OPC_DCP_RESPONSE]                = &COHAEventHandler::DcpQueryResponse;
	m_handler[OHA_OPC_LLDP_INDICATION]             = &COHAEventHandler::LldpIndication;
	m_handler[OHA_OPC_HELLO_INDICATION]            = &COHAEventHandler::HelloIndication;
	m_handler[OHA_OPC_ADDR_INFO_IND]               = &COHAEventHandler::AddrInfoIndication;
	m_handler[OHA_OPC_PRM_INDICATION]              = &COHAEventHandler::PrmIndication;
	m_handler[OHA_OPC_REMA_INDICATION]             = &COHAEventHandler::RemaIndication;
	m_handler[OHA_OPC_LLDP_CONFIG_INDICATION]      = &COHAEventHandler::LldpConfigIndication;
	m_handler[OHA_OPC_LLDP_PORT_CONFIG_INDICATION] = &COHAEventHandler::LldpPortConfigIndication;
}

/*----------------------------------------------------------------------------*/
void COHAEventHandler::reprovide( OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	if ( m_isStopping )  // Closing OHA channel in progress ?
	{
		// free RQB
		pnd_oha_free_rqb( pRQB, &m_pndHandle );
	}
	else
	{
		// Reprovide RQB for next indication
		eps_user_request_lower( pRQB, this, COHAEventHandler::EventReceived );
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
