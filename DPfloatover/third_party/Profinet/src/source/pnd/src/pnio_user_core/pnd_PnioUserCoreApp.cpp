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
/*  F i l e               &F: pnd_pnio_user_core.cpp                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver PNIO user application class                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3002
#define PND_MODULE_ID       3002

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
#include <iostream>
#include <sstream>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_UserNode.h"
#include "pnd_UserStore.h"
#include "pnd_IOCUser.h"
#include "pnd_ParamStore.h"
#include "pnd_PnioUserCoreApp.h"
#include "pnd_PDUser.h"
#include "pnd_IOCDevice.h"
#include "pnd_IOCSlot.h"
#include "pnd_IOCSubslot.h"
#include "pnd_OHAUser.h"

PND_FILE_SYSTEM_EXTENSION(PNDRIVER_MODULE_ID)

/////////////////////////////////////////////////////////////////////////////
//  Variables
/////////////////////////////////////////////////////////////////////////////

CPnioUserCoreApp* CPnioUserCoreApp::m_instance = 0;  // The App Singleton Instance

/////////////////////////////////////////////////////////////////////////////
// CParamStore PnDriver storage class for HW and PNIO configuration
/////////////////////////////////////////////////////////////////////////////

CPnioUserCoreApp::CPnioUserCoreApp() : CUserNode()
{
    // Init members
    for ( LSA_INT idx = 0; idx < PSI_CFG_MAX_HD_CNT; idx++ )
    {
        m_userStore[idx] = 0;
        pnd_memset( &m_pndPnioHandle[idx],   0, sizeof( PND_HANDLE_TYPE ));
        pnd_memset( &m_logAddrList[idx], 0, sizeof( PND_PNUC_LADDR_TYPE ));
    }

    m_requestList.clear();
}

/*----------------------------------------------------------------------------*/
CPnioUserCoreApp::~CPnioUserCoreApp()
{
    // Init members
    for ( LSA_INT idx = 0; idx < PSI_CFG_MAX_HD_CNT; idx++ )
    {
        if ( m_userStore[idx] != 0 )
        {
            delete ( m_userStore[idx] );
        }

        m_userStore[idx] = 0;
        pnd_memset( &m_pndPnioHandle[idx],   0, sizeof( PND_HANDLE_TYPE ));
        pnd_memset( &m_logAddrList[idx], 0, sizeof( PND_PNUC_LADDR_TYPE ));
    }

    m_requestList.clear();
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CPnioUserCoreApp::IsCreated()
{
    LSA_BOOL result = (CPnioUserCoreApp::m_instance == 0) ? LSA_FALSE : LSA_TRUE;

    return ( result );
}

/*----------------------------------------------------------------------------*/
CPnioUserCoreApp* CPnioUserCoreApp::GetInstance()
{
    if ( CPnioUserCoreApp::m_instance == 0 ) // not created ?
    {
        m_instance = new CPnioUserCoreApp();
        PND_ASSERT( m_instance != 0 );
    }

    return ( m_instance );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::DeleteInstance()
{
    if ( CPnioUserCoreApp::m_instance != 0 )
    {
        // Delete the singleton instance
        delete ( m_instance );

        CPnioUserCoreApp::m_instance = 0;
    }
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::ControllerOpen( PND_RQB_PTR_TYPE pRQB )
{
    PND_IOB_CORE_OPEN_PTR_TYPE pOpen = &pRQB->args.iob_open;

    LSA_UINT16 cpIdx = pOpen->pnd_handle.cp_nr;

    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get IOC User for PND Handle and call the open Reequest
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

    CIOCUser* pIOC = dynamic_cast<CIOCUser*>( pStore->GetUser( IOC_USER ));
    PND_ASSERT( pIOC != 0 );

    // Delegate to IOC user instance for this IF
    pIOC->ControllerOpen( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::ControllerClose( PND_RQB_PTR_TYPE pRQB )
{
    PND_IOB_CORE_CLOSE_PTR_TYPE pClose = &pRQB->args.iob_close;

    LSA_UINT16 cpIdx = pClose->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get IOC User for PND Handle and call the close request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

    CIOCUser* pIOC = dynamic_cast<CIOCUser*>( pStore->GetUser( IOC_USER ));
    PND_ASSERT( pIOC != 0 );

    // Delegate to IOC user instance for this IF
    pIOC->ControllerClose( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::AlarmProvide( PND_RQB_PTR_TYPE pRQB )
{
    PND_ALARM_PTR_TYPE pAlarm = &pRQB->args.iob_alarm;

    LSA_UINT16 cpIdx = pAlarm->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get IOC User for PND Handle and call the AlarmProvide request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

	if(pAlarm->ind_ref == PNIO_REF_IFC)
	{
		CPDUser* pPD = dynamic_cast<CPDUser*>( pStore->GetUser( PD_USER ));
		PND_ASSERT( pPD != 0 );

		// Delegate to PD user instance for this IF

		pPD->AlarmProvide( pRQB );
	}
	else
	{
		CIOCUser* pIOC = dynamic_cast<CIOCUser*>( pStore->GetUser( IOC_USER ));
		PND_ASSERT( pIOC != 0 );

		// Delegate to IOC user instance for this IF
		pIOC->AlarmProvide( pRQB );
	}
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::AlarmReProvide( PND_RQB_PTR_TYPE pRQB )
{
    PND_ALARM_PTR_TYPE pAlarm = &pRQB->args.iob_alarm;

    LSA_UINT16 cpIdx = pAlarm->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get IOC & PD User for PND Handle and call AlarmReProvide request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

	if(pAlarm->ind_ref == PNIO_REF_IFC)
	{
		CPDUser* pPD = dynamic_cast<CPDUser*>( pStore->GetUser( PD_USER ));
		PND_ASSERT( pPD != 0 );

		// Delegate to PD user instance for this IF

		pPD->AlarmReProvide( pRQB );
	}
	else
	{
		CIOCUser* pIOC = dynamic_cast<CIOCUser*>( pStore->GetUser( IOC_USER ));
		PND_ASSERT( pIOC != 0 );

		// Delegate to IOC user instance for this IF

		pIOC->AlarmReProvide( pRQB );
	}
}

    // overrideable PND request Methodes
/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::RecordRead( PND_RQB_PTR_TYPE p_rb )
{
    PND_RECORD_READ_WRITE_PTR_TYPE pRead = &p_rb->args.rec_read;

    LSA_UINT16 cpIdx = pRead->pnd_handle.cp_nr;
    LSA_UINT32 logLaddr = pRead->Addr.u.Addr;

    if ( m_logAddrList[cpIdx].laddr_ref[logLaddr] == 0 ) 
    {
        // Invalid logical address --> use default impl for failure response
        CUserNode::RecordRead( p_rb );
    }
    else
    {
        // execute node specific implementation
        CUserNode* pUser = static_cast<CUserNode*>( m_logAddrList[cpIdx].laddr_ref[logLaddr] );

        pUser->RecordRead( p_rb );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::RecordWrite( PND_RQB_PTR_TYPE p_rb )
{
    PND_RECORD_READ_WRITE_PTR_TYPE pWrite = &p_rb->args.rec_write;

    LSA_UINT16 cpIdx = pWrite->pnd_handle.cp_nr;
    LSA_UINT32 logLaddr = pWrite->Addr.u.Addr;

    if ( m_logAddrList[cpIdx].laddr_ref[logLaddr] == 0 ) 
    {
        // Invalid logical address --> use default impl for failure response
        CUserNode::RecordWrite( p_rb );
    }
    else
    {
        // execute node specific implementation
        CUserNode* pUser = static_cast<CUserNode*>( m_logAddrList[cpIdx].laddr_ref[logLaddr] );

        pUser->RecordWrite( p_rb );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::DeviceActivate( PND_RQB_PTR_TYPE p_rb )
{
    PND_DEV_ACTIVATE_PTR_TYPE pActivate = &p_rb->args.iob_dev_act;

    LSA_UINT16 cpIdx = pActivate->pnd_handle.cp_nr;
    LSA_UINT32 logLaddr = pActivate->Addr.u.Addr;

    if ( m_logAddrList[cpIdx].laddr_ref[logLaddr] == 0 ) 
    {
        // Invalid logical address --> use default impl for failure response
        CUserNode::DeviceActivate( p_rb );
    }
    else
    {
        // execute node specific implementation
        CUserNode* pUser = static_cast<CUserNode*>( m_logAddrList[cpIdx].laddr_ref[logLaddr] );

        pUser->DeviceActivate( p_rb );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::DiagnosticRequest( PND_RQB_PTR_TYPE pRQB )
{
	PND_DIAG_REQ_PTR_TYPE pDiagnosticReq = &pRQB->args.diag_req;

	PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, ">>DiagnosticRequest,  DiagService: %d", pDiagnosticReq->DiagReq->DiagService );

	switch(pDiagnosticReq->DiagReq->DiagService)
	{
	    case PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST:
		    {
			    GetDiagConfigSubmoduleList( pRQB );
		    }
            break;
	    case PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO:
		    {
			    GetDiagConfigNameAddrInfoData( pRQB );
		    }
            break;
        case PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC:
		    {
			    GetDiagDeviceDiagnostic( pRQB );
		    }
            break;
        case PNIO_CTRL_DIAG_RESERVED:
	    case PNIO_CTRL_DIAG_CONFIG_IOROUTER_PRESENT:
	    case PNIO_CTRL_DIAG_CONFIG_OUTPUT_SLICE_LIST:
	    case PNIO_CTRL_DIAG_DEVICE_STATE:
		    {
			    // PNDriver doesn't support this service!
			    pDiagnosticReq->pnio_err = PNIO_ERR_PRM_INVALIDARG;
		    }
            break;
	    default:
		    {
			    PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "Diagnostic Request Ref: %d", pDiagnosticReq->DiagReq->ReqRef );
			    break;
		    }

	}

	PND_PNUC_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "<<DiagnosticRequest" );

	pnd_pnio_user_request_iobc_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::GetDiagConfigSubmoduleList( PND_RQB_PTR_TYPE pRQB )
{
	PND_DIAG_REQ_PTR_TYPE pDiagnosticReq = &pRQB->args.diag_req;
	LSA_UINT16 cpIdx = pDiagnosticReq->pnd_handle.cp_nr;
	LSA_UINT8  *pBuffer;
	LSA_UINT32 bufferLen;
	LSA_UINT32 logAddrCount;
	LSA_UINT32 subslotCount = 0;
	
	// getting IOCSUBSLOT count
	for(logAddrCount = 0; logAddrCount < PND_LADDR_MAX; logAddrCount++)
	{
		if(m_logAddrList[cpIdx].laddr_type[logAddrCount] == IOCSUBSLOT)
			subslotCount++;
	}
	
	bufferLen = subslotCount * sizeof(PNIO_CTRL_DIAG_CONFIG_SUBMODULE);
	pBuffer   = (PNIO_UINT8 *) pnd_mem_alloc( bufferLen );
	PND_ASSERT( pBuffer );

	pDiagnosticReq->pDiagDataBuffer   = pBuffer;
	pDiagnosticReq->DiagDataBufferLen = bufferLen;

	for(logAddrCount = 0; logAddrCount < PND_LADDR_MAX; logAddrCount++) 
	{
		if(m_logAddrList[cpIdx].laddr_type[logAddrCount] == IOCSUBSLOT)
		{
			// execute node specific implementation
			CUserNode* pUser = static_cast<CUserNode*>( m_logAddrList[cpIdx].laddr_ref[logAddrCount] );

			AddSubmoduleConfigToBuffer( pUser, pBuffer );

			pBuffer += (LSA_UINT8)sizeof(PNIO_CTRL_DIAG_CONFIG_SUBMODULE);
		}
	}

}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::GetDiagDeviceDiagnostic( PND_RQB_PTR_TYPE pRQB )
{
	PND_DIAG_REQ_PTR_TYPE pDiagnosticReq = &pRQB->args.diag_req;
	LSA_UINT16 cpIdx = pDiagnosticReq->pnd_handle.cp_nr;
	LSA_UINT32 logLaddr = pDiagnosticReq->DiagReq->u.Addr.u.Addr;

	LSA_VOID  *pBuffer;
	LSA_UINT32 bufferLen;

	bufferLen = sizeof(PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA);
	pBuffer   = pnd_mem_alloc( bufferLen );
	PND_ASSERT( pBuffer );

	pDiagnosticReq->pDiagDataBuffer   = (PNIO_UINT8 *) pBuffer;
	pDiagnosticReq->DiagDataBufferLen = bufferLen;

	if ( m_logAddrList[cpIdx].laddr_type[logLaddr] != IOCSUBSLOT &&
		 m_logAddrList[cpIdx].laddr_type[logLaddr] != IOCSLOT ) 
	{
		// parameter address is wrong
		// send PNIO error response for invalid LADDR as default  RQB to IOBC
		pDiagnosticReq->pnio_err = PNIO_ERR_PRM_ADD;
	}
	else
	{
		// execute node specific implementation
		CUserNode* pUser = static_cast<CUserNode*>( m_logAddrList[cpIdx].laddr_ref[logLaddr] );

		AddDiagDeviceDiagnosticToBuffer( pUser, pBuffer );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::GetDiagConfigNameAddrInfoData( PND_RQB_PTR_TYPE pRQB )
{
	PND_DIAG_REQ_PTR_TYPE pDiagnosticReq = &pRQB->args.diag_req;

	LSA_VOID  *pBuffer;
	LSA_UINT32 bufferLen;
	
	bufferLen = sizeof(PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA);
	pBuffer   = pnd_mem_alloc( bufferLen );
	PND_ASSERT( pBuffer );

	pDiagnosticReq->pDiagDataBuffer   = (PNIO_UINT8*) pBuffer;
	pDiagnosticReq->DiagDataBufferLen = bufferLen;

	AddConfNameAddrInfoToBuffer(pRQB, pBuffer);

}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::AddSubmoduleConfigToBuffer( CUserNode *pUser, LSA_UINT8 *pBuffer )
{
	CIOCSubslot* pSubslot = static_cast<CIOCSubslot*>( pUser );

	PNIO_CTRL_DIAG_CONFIG_SUBMODULE  *pConfSubmod;
	PND_IO_ADDR_TYPE  inputAddress = pSubslot->GetAddressForInput();
	PND_IO_ADDR_TYPE  outputAddress = pSubslot->GetAddressForOutput();

	pConfSubmod = (PNIO_CTRL_DIAG_CONFIG_SUBMODULE *)pBuffer;

	pConfSubmod->Address.AddrType   = PNIO_ADDR_LOG;        // PNIO_ADDR_LOG - PNIO_ADDR_GEO
	pConfSubmod->Address.IODataType = PNIO_IO_OUT;          // PNIO_IO_IN - PNIO_IO_OUT
	pConfSubmod->Address.u.Addr     = pSubslot->GetSubslotLogAddr();  // LADDR
	//pConfSubmod->Address.u.Reserved[5] = 0;

	pConfSubmod->DataLength      = inputAddress.length_IO_data != 0 ? inputAddress.length_IO_data : outputAddress.length_IO_data;
	pConfSubmod->DataType        = PNIO_DATA_RT;            // PNIO_DATA_RT - PNIO_DATA_IRT
	pConfSubmod->ComType         = PNIO_COM_UNICAST;		// PNIO_COM_DIRECT_DATA_EX - PNIO_COM_UNICAST 
	pConfSubmod->Api             = pSubslot->GetApiNr();

	pConfSubmod->ReductionFactor = pSubslot->GetReductionRatio();     // ReductionFactor
	pConfSubmod->Phase           = pSubslot->GetPhase();              // Phase
	pConfSubmod->CycleTime       = (LSA_UINT32)(pSubslot->GetCycleTime() * 3125)/100;   // TimeBase: 31.25 us, SendClock = SendClockFactor * TimeBase
	pConfSubmod->StatNo          = pSubslot->GetDeviceNr();

	pConfSubmod->Reserved1[0]    = 0;
	pConfSubmod->Reserved1[1]    = 0;

	pConfSubmod->Slot            = pSubslot->GetSlotNr();
	pConfSubmod->Subslot         = pSubslot->GetSubslotNr();

	pConfSubmod->Reserved2[0]    = 0;
	pConfSubmod->Reserved2[1]    = 0;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::AddDiagDeviceDiagnosticToBuffer( CUserNode *pUser, LSA_VOID *pBuffer )
{
	PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *pDevState = (PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *) pBuffer;
	CIOCDevice* pParentDevice;

	CIOCSlot *pSlot = dynamic_cast<CIOCSlot*>( pUser );
	CIOCSubslot *pSubslot = dynamic_cast<CIOCSubslot*>( pUser );

	if(pSlot)
	{
		pParentDevice = pSlot->getDevice();
	}
	else // pSubslot
	{
		pParentDevice = pSubslot->getDevice();
	}

	pDevState->Mode = (PNIO_DEV_ACT_TYPE) pParentDevice->GetDeviceState();

	pParentDevice->GetDeviceDiagState(
		&pDevState->ErrorCause, 
		&pDevState->ReasonCode, 
		pDevState->AdditionalInfo);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::AddConfNameAddrInfoToBuffer( PND_RQB_PTR_TYPE pRQB, LSA_VOID *pBuffer )
{
	PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA DiagConfNameAddrInfo;
	LSA_VOID *pDiagConfBuf = pBuffer;
    PND_ASSERT( pDiagConfBuf );

	LSA_UINT16 cpIdx = pRQB->args.diag_req.pnd_handle.cp_nr;
	PND_ASSERT( m_userStore[cpIdx] != 0 );

	// Get OHAUser to get the StationType and Interface Data
	CUserStore *pStore =  dynamic_cast<CUserStore*>( m_userStore[cpIdx] );
	PND_ASSERT( pStore != 0 );

	COHAUser* pOHAUser = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
	PND_ASSERT( pOHAUser != 0 );

	pOHAUser->GetStationType( DiagConfNameAddrInfo.TypeOfStation );

	pOHAUser->GetInterfaceData( &DiagConfNameAddrInfo.ip_addr,
		                        &DiagConfNameAddrInfo.ip_mask,
								&DiagConfNameAddrInfo.default_router,
								 DiagConfNameAddrInfo.name );

	pnd_memcpy(pDiagConfBuf, &DiagConfNameAddrInfo, sizeof(PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA));
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::SetIpAndNos( PND_RQB_PTR_TYPE p_rb )
{
	LSA_UINT16 cpIdx = p_rb->args.set_ip_and_nos.pnd_handle.cp_nr;
	PND_ASSERT( m_userStore[cpIdx] != 0 );

	// Get OHAUser to get the type_of_station
	CUserStore *pStore =  dynamic_cast<CUserStore*>( m_userStore[cpIdx] );
	PND_ASSERT( pStore != 0 );

	COHAUser* pOHAUser = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
	PND_ASSERT( pOHAUser != 0 );

	pOHAUser->SetAddress( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::RemaRead( PND_RQB_PTR_TYPE p_rb )
{
	UpdateRema( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::UpdateRema( PND_RQB_PTR_TYPE p_rb )
{
	CParamStore *paramObj = CParamStore::GetInstance();
    PND_ASSERT( paramObj != 0 );
	
	PND_REMA_DATA_TYPE *pRemaData = &p_rb->args.rema_data;

	paramObj->UpdateRema( pRemaData->data, pRemaData->data_length, pRemaData->index, &pRemaData->remaXMLbuffer, &pRemaData->remaXMLbufferLength); //returns rema data as &pRemaData->remaXMLbuffer
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::SetMode( PND_RQB_PTR_TYPE p_rb )
{
    PND_SET_MODE_PTR_TYPE pSetMode = &p_rb->args.iob_set_mode;

    LSA_UINT16 cpIdx = pSetMode->pnd_handle.cp_nr;

    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get IOC User for PND Handle and call the SetMode request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

    CIOCUser* pIOC = dynamic_cast<CIOCUser*>( pStore->GetUser( IOC_USER ));
    PND_ASSERT( pIOC != 0 );

    // Delegate to IOC user instance for this IF
    pIOC->SetMode( p_rb );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::InterfaceOpen( PND_RQB_PTR_TYPE pRQB )
{
    PND_IOB_CORE_OPEN_PTR_TYPE pOpen = &pRQB->args.iob_open;

    LSA_UINT16 cpIdx = pOpen->pnd_handle.cp_nr;

    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get PD User for PND Handle and call the open Reequest
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

    CPDUser* pPD = dynamic_cast<CPDUser*>( pStore->GetUser( PD_USER ) );
    PND_ASSERT( pPD != 0 );

    // Delegate to PD user instance for this IF
    pPD->InterfaceOpen( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::InterfaceClose( PND_RQB_PTR_TYPE pRQB )
{
    PND_IOB_CORE_CLOSE_PTR_TYPE pClose = &pRQB->args.iob_close;

    LSA_UINT16 cpIdx = pClose->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get PD User for PND Handle and call the close request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

    COHAUser* pOHA = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
    PND_ASSERT( pOHA != 0 );

    // Delegate to OHA user instance for this IF
    pOHA->RemaReleaseResources();

    CPDUser* pPD = dynamic_cast<CPDUser*>( pStore->GetUser( PD_USER ) );
    PND_ASSERT( pPD != 0 );

    // Delegate to PD user instance for this IF
    pPD->InterfaceClose( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::RemaProvide( PND_RQB_PTR_TYPE pRQB )
{
	PND_REMA_DATA_PTR_TYPE pRema = &pRQB->args.rema_data;

    LSA_UINT16 cpIdx = pRema->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get OHA User for PND Handle and call the RemaProvide request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

	COHAUser* pOHA = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
	PND_ASSERT( pOHA != 0 );

	// Delegate to OHA user instance for this IF
	pOHA->RemaProvide( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::RemaReProvide( PND_RQB_PTR_TYPE pRQB )
{
	PND_REMA_DATA_PTR_TYPE pRema = &pRQB->args.rema_data;

    LSA_UINT16 cpIdx = pRema->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get OHA User for PND Handle and call the RemaProvide request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

	COHAUser* pOHA = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
	PND_ASSERT( pOHA != 0 );

	// Delegate to OHA user instance for this IF
	pOHA->RemaReProvide( pRQB );
}

/*----------------------------------------------------------------------------*/
void CPnioUserCoreApp::ParamWrite( PND_RQB_PTR_TYPE pRQB )
{
	PND_PRM_WRITE_DATA_TYPE* pPrmWrite = &pRQB->args.prm_write;

    LSA_UINT16 cpIdx = pPrmWrite->pnd_handle.cp_nr;
    
    PND_ASSERT( m_userStore[cpIdx] != 0 );

    // Get OHA User for PND Handle and call the RemaProvide request
    CUserStore *pStore =  dynamic_cast<CUserStore*>(  m_userStore[cpIdx]);
    PND_ASSERT( pStore != 0 );

	COHAUser* pOHA = dynamic_cast<COHAUser*>( pStore->GetUser( OHA_USER ));
	PND_ASSERT( pOHA != 0 );

	// Delegate to OHA user instance for this IF
	pOHA->WriteParameter( pRQB );
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CPnioUserCoreApp::GetKey()
{
    return ( "0" );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CPnioUserCoreApp::GetNumber()
{
    return ( 0 );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::Startup( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)p_rb;
    PND_ASSERT( p_rb != 0 );

    PND_STARTUP_PTR_TYPE pStart = &pRQB->args.startup;
    PND_ASSERT( pStart->cbf != 0 );

    PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
        "CPnioUserCoreApp::Startup(): nr of CP(%d)", 
        pStart->startup_param.nr_of_cp );

    // check params 
    if ( ( pStart->startup_param.nr_of_cp > 0 ) &&
         ( pStart->startup_param.nr_of_cp <= PSI_CFG_MAX_HD_CNT ))
    {
        CUserNode *pStore;

        m_requestList.clear();

        m_isStartup = LSA_TRUE;

        // Create for each PND-Handle (HD/IF) an user storage
        for ( LSA_UINT16 idx = 0; idx < pStart->startup_param.nr_of_cp; idx++ )
        {
            m_pndPnioHandle[idx] = pStart->startup_param.pnd_handle[idx];

            CUserStore* pUserStore = new CUserStore( &m_pndPnioHandle[idx], &m_logAddrList[idx], this );
            PND_ASSERT( pUserStore != 0 );

            m_userStore[idx] = pUserStore;

            // Add PNIO IF entry to request list
            m_requestList.push_back ( pUserStore );
        }

        PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
            "CPnioUserCoreApp::Startup(): Starting, PNIO IF, count(%u)", 
            m_requestList.size() );

        pStore = getNextUserStore();
        PND_ASSERT( pStore != 0 );

        // create PND RQB for local Startup
        PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
        PND_ASSERT( pRQBLocal != 0 );

        LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

        pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
        pRQBLocal->args.pnuc_internal.data_ptr = p_rb;

        // Register CBF info in RQB for local request done
        PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
        PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CPnioUserCoreApp::StartupDone );

        // Trigger first startup and wait for complete
        // Note: received in StartupDone
        pStore->Startup( pRQBLocal );
    }
    else
    {
        // set error and finish request
        pStart->pnio_err = PNIO_ERR_PRM_INVALIDARG;

        PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_ERROR,
            "CPnioUserCoreApp::Startup(): invalid args, nr of CP(%d)", 
            pStart->startup_param.nr_of_cp );

        // Finsish request by calling CBF
        pStart->cbf( pRQB );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::Shutdown( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)p_rb;
    PND_ASSERT( p_rb != 0 );

    PND_SHUTDOWN_PTR_TYPE pShutdown = &pRQB->args.shutdown;
    PND_ASSERT( pShutdown->cbf != 0 );

    if (( m_isStarted ) &&   // Startup done ?
        ( !m_isShutdown ))    // Shutdown trigger already in progress ?
    {
        m_isShutdown = LSA_TRUE;

        // Shutdown each created PNIO IF
        // Note: shutdown the IF with closing alluser channels is done in reverse order
        m_requestList.clear();

        for ( LSA_INT idx = PSI_CFG_MAX_HD_CNT-1; idx >= 0; idx-- )
        {
            if ( m_userStore[idx] != 0 ) // created ?
            {
                m_requestList.push_back( m_userStore[idx] );
            }
        }

        PND_PNUC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
            "CUserStore::Shutdown(): Starting, PNIO IF, count(%u)", 
            m_requestList.size() );

        CUserNode* pStore = getNextUserStore();

        if ( pStore != 0 ) // IF to shutdown avaliable ?
        {
            // create PND RQB for local Startup
            PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
            PND_ASSERT( pRQBLocal != 0 );

            LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

            pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
            pRQBLocal->args.pnuc_internal.data_ptr = p_rb;

            // Register CBF info in RQB for local request done
            PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
            PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CPnioUserCoreApp::ShutdownDone );

            // Trigger first shutdown and wait for complete
            // Note: received in ShutdownDone
            pStore->Shutdown( pRQBLocal );
        }
        else
        {
            // No IFs to shutdown 
            PND_PNUC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,
                "CPnioUserCoreApp::Shutdown(): Shutdown, IF complete" );

            m_isShutdown = LSA_FALSE;
            m_isStarted  = LSA_FALSE;

            // Finsish request by calling CBF
            pShutdown->cbf( pRQB );
        }
    }
    else
    {
        // set error and finish request
        pShutdown->pnio_err = PNIO_ERR_SEQUENCE;

        PND_PNUC_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR,
            "CPnioUserCoreApp::Shutdown(): invalid state, started(%u) shutdown in progress(%u)",
            m_isStarted, m_isShutdown );

        // Finsish request by calling CBF
        pShutdown->cbf( pRQB );
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CPnioUserCoreApp::GetLogAddrList( void* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)p_rb;
    PND_ASSERT( p_rb != 0 );

    PND_IOB_CORE_OPEN_PTR_TYPE pOpen = &pRQB->args.iob_open;

    if(pOpen->laddr_list_ptr != PNIO_NULL)
    {
        pnd_memcpy(pOpen->laddr_list_ptr, m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_type, sizeof(m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_type));
    }
    if(pOpen->dev_number_list_ptr != PNIO_NULL)
    {
        pnd_memcpy(pOpen->dev_number_list_ptr, m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_device_nr, sizeof(m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_device_nr));
    }
    if(pOpen->slot_number_list_ptr != PNIO_NULL)
    {
        pnd_memcpy(pOpen->slot_number_list_ptr, m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_slot_nr, sizeof(m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_slot_nr));
    }
    if(pOpen->subslot_number_list_ptr != PNIO_NULL)
    {
        pnd_memcpy(pOpen->subslot_number_list_ptr, m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_subslot_nr, sizeof(m_logAddrList[pOpen->pnd_handle.cp_nr].laddr_subslot_nr));
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CPnioUserCoreApp::StartupDone( void* p_inst, void* p_rb )
{
    CPnioUserCoreApp *pApp = static_cast<CPnioUserCoreApp*>(p_inst);
    PND_RQB_PTR_TYPE pRQB  = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pApp != 0);
    PND_ASSERT( pRQB != 0);

    CUserNode *pUser = pApp->getNextUserStore();

    if ( pRQB->args.pnuc_internal.pnio_err != PNIO_OK )  // internal error detected
    {
        PND_PNUC_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE,
            "CUserStore::StartupDone(): >>>> Starting, PNIO users failed" );

        pApp->m_isStartup = LSA_FALSE;
        pApp->m_isStarted = LSA_TRUE;

        // Restore upper RQB and set PNIO result
        PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
        PND_ASSERT( pRspRQB != 0 );
        
        pRspRQB->args.startup.pnio_err = pRQB->args.pnuc_internal.pnio_err;

        // Delete local RQB
        pnd_mem_free( pRQB );

        // Finsish request by calling CBF
        pRspRQB->args.startup.cbf( pRspRQB );
    }
    else
    {
        if ( pUser != 0 ) // user store avaliable ?
        {
            // Trigger next startup and wait for complete
            // Note: received in StartupDone
            pUser->Startup( p_rb );
        }
        else
        {
            PND_PNUC_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE,
                "CUserStore::StartupDone(): >>>> Starting, PNIO users complete" );

            pApp->m_isStartup = LSA_FALSE;
            pApp->m_isStarted = LSA_TRUE;

            // Restore upper RQB and set PNIO result
            PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
            PND_ASSERT( pRspRQB != 0 );
        
            pRspRQB->args.startup.pnio_err = pRQB->args.pnuc_internal.pnio_err;

            // Delete local RQB
            pnd_mem_free( pRQB );

            // Finsish request by calling CBF
            pRspRQB->args.startup.cbf( pRspRQB );
        }
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioUserCoreApp::ShutdownDone( void* p_inst, void* p_rb )
{
    CPnioUserCoreApp *pApp = static_cast<CPnioUserCoreApp*>(p_inst);
    PND_RQB_PTR_TYPE pRQB  = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pApp != 0);
    PND_ASSERT( pRQB != 0);

    CUserNode *pStore = pApp->getNextUserStore();

    if ( pStore != 0 ) // user avaliable ?
    {
        // Trigger next startup and wait for complete
        // Note: reusing received RQB for next shutdown request
        pStore->Shutdown( p_rb );
    }
    else
    {
        PND_PNUC_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE,
            "CUserStore::CPnioUserCoreApp(): >>>> shutdown, PNIO IF complete" );

        pApp->m_isStarted  = LSA_FALSE;
        pApp->m_isShutdown = LSA_FALSE;

        // Restore upper RQB and set PNIO result
        PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
        PND_ASSERT( pRspRQB != 0 );
        
        pRspRQB->args.shutdown.pnio_err = pRQB->args.pnuc_internal.pnio_err;

        // Delete local RQB
        pnd_mem_free( pRQB );

        // Finsish request by calling CBF
        pRspRQB->args.shutdown.cbf( pRspRQB );
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CUserNode* CPnioUserCoreApp::getNextUserStore()
{
    CUserNode* pUser = 0;

    if ( !m_requestList.empty()) // Empty list ?
    {
        // Return the first node, and remove it from top of list
        pUser = m_requestList[0];

        m_requestList.erase( m_requestList.begin());
    }

    return ( pUser );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

