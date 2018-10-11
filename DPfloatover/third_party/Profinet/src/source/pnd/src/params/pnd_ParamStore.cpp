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
/*  F i l e               &F: pnd_ParamStore.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of Configuration storage class (HW and PNIO settings)     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3000
#define PND_MODULE_ID       3000

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"

#include "cm_arcb_helper.h"

#include "servusrx.h"

/* StdLib++ */
#include <vector>
#include <string>

using namespace std;

#include "pnd_ValueHelper.h"
#include "pnd_ParamStore.h"
#include "pnd_XmlParamStore.h" // param storage from XML input

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
//  Constants
/////////////////////////////////////////////////////////////////////////////

static const LSA_CHAR* const SYSTEM_DESCRIPTION_DELIMITER = ", ";
static const LSA_CHAR* const SYSTEM_DESCRIPTION_DOT_FOR_FW_VERSION = ".";
static const LSA_CHAR* const SYSTEM_DESCRIPTION_HW_VERSION_PREFIX = "HW: Version ";
static const LSA_CHAR* const SYSTEM_DESCRIPTION_FW_VERSION_PREFIX = "FW: Version ";
static const LSA_CHAR* const SYSTEM_DESCRIPTION_PRODUCT_SERIAL_NUMBER_PREFIX = "S";
static const LSA_UINT32 MAX_SYSTEM_DESCRIPTION_LENGTH = 255;
static const LSA_UINT32 MAX_IF_DESCRIPTION_LENGTH = 255;

/////////////////////////////////////////////////////////////////////////////
//  Variables
/////////////////////////////////////////////////////////////////////////////

CParamStore* CParamStore::m_instance = 0;  // The ParamStore Singleton Instance

/////////////////////////////////////////////////////////////////////////////
// CParamStore PnDriver storage class for HW and PNIO configuration
/////////////////////////////////////////////////////////////////////////////

CParamStore::CParamStore()
{
	// Init members
	m_created = LSA_FALSE;

	pnd_memset( &m_startup, 0, sizeof( m_startup ));
	pnd_memset( &m_imInput, 0, sizeof( m_imInput ));
	pnd_memset( &m_hdInput, 0, sizeof( m_hdInput ));
	pnd_memset( &m_hdOutput, 0, sizeof( m_hdOutput ));

	m_pParamHandlerXML = new CXmlParamStore();

	pnd_memset( &m_remaData.ipSuiteRecord, 0, sizeof(PND_REMA_RECORD_TYPE));
	m_remaData.ipSuiteRecord.recordLen = 0; m_remaData.ipSuiteRecord.recordType = 0;
	pnd_memset( &m_remaData.nosRecord, 0, sizeof(PND_REMA_RECORD_TYPE));
	m_remaData.nosRecord.recordLen = 0; m_remaData.nosRecord.recordType = 0;
	pnd_memset( &m_remaData.mib2Record, 0, sizeof(PND_REMA_RECORD_TYPE));
	m_remaData.mib2Record.recordLen = 0; m_remaData.mib2Record.recordType = 0;
    pnd_memset( &m_systemDescription, 0, sizeof(PNIO_SYSTEM_DESCR));
}

/*----------------------------------------------------------------------------*/
CParamStore::~CParamStore()
{
    xml_free(m_remaData.ipSuiteRecord.recordData);
    xml_free(m_remaData.nosRecord.recordData);
    xml_free(m_remaData.mib2Record.recordData);

	// Free resources
	if ( m_pParamHandlerXML != 0 )
	{
		delete ( m_pParamHandlerXML );
		m_pParamHandlerXML = 0;
	}
}

/*----------------------------------------------------------------------------*/
CParamStore* CParamStore::GetInstance()
{
	if ( CParamStore::m_instance == 0 ) // not created ?
	{
		m_instance = new CParamStore();
		PND_ASSERT( m_instance != 0 );
	}

	return ( m_instance );
}

/*----------------------------------------------------------------------------*/
void CParamStore::DeleteInstance()
{
	if ( CParamStore::m_instance != 0 )
	{
		// Delete the singleton instance
		delete ( m_instance );
		m_instance = 0;
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::Create( LSA_UINT32 nrOfEntries, LSA_VOID* pLocationData, LSA_VOID* pConfigData, LSA_UINT32 length, LSA_VOID* pRemaData, LSA_UINT32 remaLength, PNIO_SYSTEM_DESCR* pSysDescr )
{
	LSA_UINT32 result = PNIO_OK;

	// Note: each CP entry is mapped to one HD with one IF
	if (( nrOfEntries == 0 ) 		|| // check possible CP entries
	    ( nrOfEntries > PSI_CFG_MAX_HD_CNT) ||
	    ( pLocationData == LSA_NULL )       || // check parameter data
	    ( pConfigData   == LSA_NULL ))
	{
		result = PNIO_ERR_PRM_INVALIDARG;
	}
    else
    {
        result = IsSystemDescriptionValid(pSysDescr);
        if(result == PNIO_OK)
        {
            result = IsIfDescriptionValid(pSysDescr);
            if(result == PNIO_OK)
            {
		        // create and check PNIO configuration from user
		        // Note: must be done before prepare, because some data are needed from PNIO params
		        result = m_pParamHandlerXML->Create( nrOfEntries, pConfigData, length, pRemaData, remaLength, (PND_XML_REMA_DATA_TYPE*)&m_remaData );

		        if ( result == PNIO_OK || result == PNIO_WARN_NO_DECENTRALIOSYSTEM )
		        {
			        // prepare the PND handle mapping and LD/HD for startup
			        prepareParamStartup( nrOfEntries, pLocationData, pSysDescr );
			        m_created = LSA_TRUE;
		        }
            }
	    }
    }

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::GetParamStartup( PND_PARAM_STARTUP_PTR_TYPE p_params )
{
	LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

	if (( p_params != 0 ) &&  // Param ok ?
	    ( m_created     ))    // created ?
	{
		*p_params = m_startup;
		result = PNIO_OK;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::GetParamHifLD( PND_PARAM_HIF_LD_PTR_TYPE p_params )
{
	LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

	if (( p_params != 0 ) &&  // Param ok ?
	    ( m_created     ))    // created ?
	{
		// Note: one HD has only one IF from CP startup
		p_params->hd_count = m_startup.nr_of_cp;

		for ( LSA_UINT32 idx = 0; idx < p_params->hd_count; idx++ )
		{
			p_params->hd_param[idx].hd_id            = m_startup.pnd_handle[idx].hd_nr;
			p_params->hd_param[idx].nr_of_if         = 1;
			p_params->hd_param[idx].pnio_if[0].if_id = m_startup.pnd_handle[idx].if_nr;
		}

		result = PNIO_OK;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::GetParamPsiLD( PND_PARAM_PSI_LD_PTR_TYPE p_params )
{
	LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

	if (( p_params != 0 ) &&  // Param ok ?
	    ( m_created     ))    // created ?
	{
		// Note: one HD has only one IF from CP startup
		p_params->im_param = m_imInput;

		p_params->hd_count = m_startup.nr_of_cp;

		for ( LSA_UINT32 idx = 0; idx < p_params->hd_count; idx++ )
		{
			p_params->hd_param[idx] = m_hdInput[idx+1];
		}

		result = PNIO_OK;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::SetParamPsiHD( LSA_UINT16 hdNr, PSI_HD_OUTPUT_PTR_TYPE p_output )
{
	LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

	PND_ASSERT(( hdNr > 0 ) && ( hdNr <= PSI_CFG_MAX_HD_CNT ));

	if (( p_output != 0 ) &&  // Param ok ?
	    ( m_created     ))    // created ?
	{
		// update HD/PNIO-IF params for IF=1 (calculated from PSI)
		m_hdOutput[hdNr] = *p_output;

		result = m_pParamHandlerXML->UpdatePnioIF( hdNr, 1, &m_hdOutput[hdNr].pnio_if[0] );
	}

	return ( result );
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_UINT32 CParamStore::GetParamOHA( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_OHA_HD_IF_PTR_TYPE p_params )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->GetParamOHA( cpHandle, p_params );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::GetParamPD( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PD_HD_IF_PTR_TYPE p_params )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->GetParamPD( cpHandle, p_params );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::GetParamIOC( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_IOC_HD_IF_PTR_TYPE p_params )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->GetParamIOC( cpHandle, p_params );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::CreateDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->CreateDeviceParam( cpHandle, devNr, pDev, pSubList );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::CreatePrmRecordsPD( PND_HANDLE_PTR_TYPE cpHandle, PrmRecordVector* pRecList )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->CreatePrmRecordsPD( cpHandle, pRecList );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::CreateAnnotation( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT8 *pEntry )
{
	LSA_UINT32 result = PNIO_ERR_PRM_DEV_ANNOTATION;

	if (( cpHandle != 0 ) &&  // Param ok ?
	    ( pEntry   != 0 ) &&
	    ( m_created     ))    // created ?
	{
		// RPCAnnotation ::=    --> Format see CLRPC_PNIO_ANNOTATION_FORMAT
		//  DeviceType(25s), Blank, OrderID(20s), Blank, HWRevision(5u), Blank,
		//  SWRevisionPrefix(1c), SWRevision(3u3u3u), EndTerm(=0)
		pnd_sprintf( pEntry, CLRPC_PNIO_ANNOTATION_FORMAT,
			m_systemDescription.IM_DeviceType,                          // DeviceType
			m_systemDescription.IM_OrderId,                             // OrderID
			m_systemDescription.IM_HwRevision,                          // HW revision
			m_systemDescription.IM_SwVersion.revision_prefix,           // SWRevisionPrefix
			m_systemDescription.IM_SwVersion.functional_enhancement,    // SWRevision 
			m_systemDescription.IM_SwVersion.bug_fix,
			m_systemDescription.IM_SwVersion.internal_change );

		result = PNIO_OK;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::CreateArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->CreateArcbAddCL( cpHandle, devNr, hwType, pAdd );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CParamStore::CreateArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE pRead )
{
	LSA_UINT32 result;

	PND_ASSERT( m_pParamHandlerXML != 0 );

	// delegate to implementation class
	result = m_pParamHandlerXML->CreateArcbReadImpl( cpHandle, devNr, pRead );

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CParamStore::DeleteArcb( PND_HANDLE_PTR_TYPE cpHandle, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd )
{
	if ( pAdd != 0 )
	{
		CValueHelper valObj;

		// cleanup the created ARCB
		PND_ASSERT( cpHandle != 0 );

		// Prepare the "PSI sysptr" used from CM to address the pools over output macros
		// Note: only the PSI addressing elements are set
		PSI_SYS_TYPE locSys;
		pnd_memset( &locSys, 0, sizeof( PSI_SYS_TYPE ));

		valObj.PndHandle2SysPtrPSI( cpHandle, &locSys );

		while (! CmListIsEmpty (&pAdd->ar_list)) 
		{
			CM_UPPER_ARCB_ADD_PTR_TYPE  arcb;

			CmListRemoveHead( &pAdd->ar_list, arcb, CM_UPPER_ARCB_ADD_PTR_TYPE );
			cm_arcb_free( &locSys, arcb, LSA_TRUE, LSA_TRUE, LSA_TRUE);
		}

		cm_arcb_free_nare_alias( &locSys, &pAdd->dev_addr );

	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CParamStore::DeleteArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE pRead )
{
	if ( pRead != 0 )
	{
		// cleanup the created ARCB for read implicit
		CValueHelper valObj;
		LSA_UINT16   rc;

		PND_ASSERT( cpHandle != 0 );

		// Prepare the "PSI sysptr" used from CM macros to address the pools over output macros
		// Note: only the PSI addressing elements are set
		PSI_SYS_TYPE locSys;
		pnd_memset( &locSys, 0, sizeof( PSI_SYS_TYPE ));

		valObj.PndHandle2SysPtrPSI( cpHandle, &locSys );

		// cleanup the created ARCB for read implicit
		if ( pRead->rec.data != 0 )  // record data allocated ?
		{
			CM_FREE_UPPER_MEM(&rc, pRead->rec.data, &locSys );
			PND_ASSERT( rc == LSA_RET_OK );
		}

		if ( pRead->dev_addr.device_name != 0 )  // device name allocated
		{
			CM_FREE_UPPER_MEM(&rc,pRead->dev_addr.device_name, &locSys );
			PND_ASSERT( rc == LSA_RET_OK );
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CParamStore::DeletePrmRecordsPD( PrmRecordVector* pRecList )
{
	if (( pRecList != 0 ) &&
		( !pRecList->empty() ))
	{
		// cleanup the created PRM records
		LSA_INT count = pRecList->size();

		for ( LSA_INT lvi = 0; lvi < count; lvi++ )
		{
			PND_PARAM_PRM_RECORD_PTR_TYPE pRec = pRecList->at(lvi);

			if ( pRec != 0 )
			{
				if ( pRec->record_data != 0 )
				{
					pnd_mem_free( pRec->record_data );
				}

				pnd_mem_free( pRec );
			}
		}

		// Cleanup the list (with empty values)
		pRecList->clear();
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CParamStore::DeleteDeviceParam( SubslotVector* pSubList )
{
	if (( pSubList != 0 ) &&
	    ( !pSubList->empty() ))
	{
		// cleanup the created PRM records
		LSA_INT count = pSubList->size();

		for ( LSA_INT lvi = 0; lvi < count; lvi++ )
		{
			PND_PARAM_SUBSLOT_PTR_TYPE pSub = pSubList->at(lvi);

			if ( pSub != 0 )
			{
				pnd_mem_free( pSub );
			}
		}

		// Cleanup the list (with empty values)
		pSubList->clear();
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CParamStore::prepareParamStartup( LSA_UINT32 nrOfEntries, LSA_VOID* pLocationData, PNIO_SYSTEM_DESCR* pSysDescr )
{
	// important: PNIO params must be avaliable for setup the startup information
	// note: mapped to PNIP(PCI-Location is set) or StdMAC(MAC-Location = set)
	PNIO_CP_ID_PTR_TYPE pLoc = (PNIO_CP_ID_PTR_TYPE)pLocationData;
    LSA_UINT32 result;

	PND_ASSERT(nrOfEntries==1);

	m_startup.nr_of_cp = (LSA_UINT16)nrOfEntries;

    // Set system description
    m_systemDescription = *pSysDescr;

	// Setup common I&M configuration data
	m_imInput.hw_revision           = 0;
	m_imInput.profile_id            = 0;
	m_imInput.profile_specific_type = 0x0001;
	m_imInput.revision_counter      = 0;
	m_imInput.supported             = 0;
	m_imInput.vendor_id_high        = 0;
	m_imInput.vendor_id_low         = 0x2a;
	m_imInput.version               = 0x0101;

	m_imInput.sw_version.revision_prefix        = PND_VERSION_REVISION_PREFIX;
	m_imInput.sw_version.functional_enhancement = PND_VERSION_FUNCTIONAL_ENHANCEMENT;
	m_imInput.sw_version.bug_fix                = PND_VERSION_BUG_FIX;
	m_imInput.sw_version.internal_change        = PND_VERSION_INTERNAL_CHANGE;

	// important: limit size to "20+1"
	pnd_sprintf( m_imInput.order_id, "%s", PND_MLFB);

	// important: limit size to "25+1"
	pnd_sprintf( m_imInput.device_type, "%s", "PN Driver" );


	for ( LSA_UINT16 idx = 0; idx < m_startup.nr_of_cp; idx++ ) 
	{		
		PND_ASSERT( m_pParamHandlerXML != 0 );

		PSI_HD_INPUT_PTR_TYPE pHD = &m_hdInput[idx+1];
		pnd_memset( &pHD->hd_location, 0, sizeof( PSI_HD_ID_TYPE ));

		// prepare the PND handle for this CP (without EDDx setting
		m_startup.pnd_handle[idx].cp_nr = idx;
		m_startup.pnd_handle[idx].hd_nr = idx +1;  // 1..N
		m_startup.pnd_handle[idx].if_nr = 1;       // fixed, one HD has only one IF

		PND_PARAM_PNIO_IF_TYPE pnioParam;
		result = m_pParamHandlerXML->GetParamPnioIF( &m_startup.pnd_handle[idx], &pnioParam );
		PND_ASSERT( result == PNIO_OK );

		m_startup.pnd_handle[idx].pnio_if_nr = pnioParam.edd_if_id;

		// prepare the HW settings based on location
		switch ( pLoc[idx].CpSelection )
		{
			case PNIO_CP_SELECT_WITH_PCI_LOCATION: // PCI location
			{
				m_startup.pnd_handle[idx].edd_comp_id = LSA_COMP_ID_EDDS;

				// Set PCI location
				pHD->hd_location.hd_selection            = PSI_HD_SELECT_WITH_LOCATION;
				pHD->hd_location.hd_location.bus_nr      = pLoc[idx].CpPciLocation.BusNr;
				pHD->hd_location.hd_location.device_nr   = pLoc[idx].CpPciLocation.DeviceNr;
				pHD->hd_location.hd_location.function_nr = pLoc[idx].CpPciLocation.FunctionNr;

				// HD common settings
				pHD->hd_id                    = idx +1;
				pHD->runs_on_level_basic      = LSA_FALSE;
				pHD->nr_of_ports              = 1;
				pHD->nr_of_if                 = 1;
				pHD->edd_type                 = LSA_COMP_ID_EDDS;
				pHD->asic_type                = PSI_ASIC_TYPE_UNKNOWN;
				pHD->rev_nr                   = 0;
				pHD->allow_none_pnio_mautypes = LSA_TRUE;
				pHD->send_clock_factor 	      = pnioParam.send_clock;

				// StdMAC settings (only for EDDS)
				pHD->stdmac.use_setting     = LSA_TRUE;
				pHD->stdmac.gigabit_support = LSA_FALSE;

				// HW port settings
				pHD->hw_port[0].if_nr             = 1;
				pHD->hw_port[0].user_nr           = 1;
				pHD->hw_port[0].mrp_ringport_type = PSI_NO_RING_PORT;

				// PNIO IF settings common
				pHD->pnio_if[0].if_id     = 1;
				pHD->pnio_if[0].edd_if_id = m_startup.pnd_handle[idx].pnio_if_nr;

				// PNIO IF settings IOC
				pHD->pnio_if[0].ioc.use_setting           = LSA_TRUE;
				pHD->pnio_if[0].ioc.nr_of_rt_devices      = PSI_MAX_CL_DEVICES;
				pHD->pnio_if[0].ioc.nr_of_qv_provider     = 0;
				pHD->pnio_if[0].ioc.nr_of_qv_consumer     = 0;
				pHD->pnio_if[0].ioc.nr_of_irt_devices     = 0;
				pHD->pnio_if[0].ioc.logical_addr_space    = 8192;
				pHD->pnio_if[0].ioc.nr_of_submod          = 2048;
				pHD->pnio_if[0].ioc.max_iocr_data_size    = 1440;
				pHD->pnio_if[0].ioc.max_mcr_data_size     = 256;
				pHD->pnio_if[0].ioc.max_record_length     = PSI_MAX_CL_RECORD_SIZE;
				pHD->pnio_if[0].ioc.max_read_implicit     = 1;
				pHD->pnio_if[0].ioc.max_alarm_data_length = pnioParam.max_alarm_data_length;

				// PNIO IF settings OHA
				pHD->pnio_if[0].oha.use_setting       = LSA_TRUE;
				pHD->pnio_if[0].oha.vendor_id         = pnioParam.vendor_id;
				pHD->pnio_if[0].oha.device_id         = pnioParam.device_id;
                pHD->pnio_if[0].oha.check_ip_enabled  = pnioParam.check_ip_enabled;
				pHD->pnio_if[0].oha.check_interval    = pnioParam.check_interval;
                result = GetSysDescr(pHD->pnio_if[0].oha.sys_descr);
                PND_ASSERT( result == PNIO_OK );
                result = GetIfDescr(pHD->pnio_if[0].oha.if_descr);
                PND_ASSERT( result == PNIO_OK );
                
				// PNIO IF settings IP
				pHD->pnio_if[0].ip.use_setting = LSA_TRUE;
                pHD->pnio_if[0].ip.nr_of_send  = 32;
				pHD->pnio_if[0].ip.nr_of_arp   = 32;
				pHD->pnio_if[0].ip.nr_of_icmp  = 32;
				pHD->pnio_if[0].ip.nr_of_udp   = 100;
				pHD->pnio_if[0].ip.nr_of_tcp   = 100;
				pHD->pnio_if[0].ip.multicast_support_on = LSA_FALSE;

				// PNIO IF settings MRP
				pHD->pnio_if[0].mrp.use_setting             = LSA_FALSE;
				pHD->pnio_if[0].mrp.max_instances           = 0;
				pHD->pnio_if[0].mrp.supported_role          = 0;
				pHD->pnio_if[0].mrp.default_role_instance0  = (PSI_MRP_ROLE_TYPE)EDD_MRP_ROLE_NONE;
				pHD->pnio_if[0].mrp.supported_multiple_role = 0;

				// PNIO IF settings NRT
				pHD->pnio_if[0].nrt.RxFilterUDP_Broadcast = LSA_TRUE;
				pHD->pnio_if[0].nrt.RxFilterUDP_Unicast   = LSA_TRUE;
			}
			break;
			case PNIO_CP_SELECT_WITH_MAC_ADDRESS:      // MAC location (StdMAC HD)
			{
				m_startup.pnd_handle[idx].edd_comp_id = LSA_COMP_ID_EDDS;

				// Set MAC location
				pHD->hd_location.hd_selection            = PSI_HD_SELECT_WITH_MAC;
				pHD->hd_location.hd_location.bus_nr      = pLoc[idx].CpPciLocation.BusNr;
				pHD->hd_location.hd_location.device_nr   = pLoc[idx].CpPciLocation.DeviceNr;
				pHD->hd_location.hd_location.function_nr = pLoc[idx].CpPciLocation.FunctionNr;

				pHD->hd_location.hd_mac[0] = pLoc[idx].CpMacAddr[0];
				pHD->hd_location.hd_mac[1] = pLoc[idx].CpMacAddr[1];
				pHD->hd_location.hd_mac[2] = pLoc[idx].CpMacAddr[2];
				pHD->hd_location.hd_mac[3] = pLoc[idx].CpMacAddr[3];
				pHD->hd_location.hd_mac[4] = pLoc[idx].CpMacAddr[4];
				pHD->hd_location.hd_mac[5] = pLoc[idx].CpMacAddr[5];

				// HD common settings
				pHD->hd_id                    = idx +1;
				pHD->runs_on_level_basic      = LSA_FALSE;
				pHD->nr_of_ports              = 1;
				pHD->nr_of_if                 = 1;
				pHD->edd_type                 = LSA_COMP_ID_EDDS;
				pHD->asic_type                = PSI_ASIC_TYPE_UNKNOWN;
				pHD->rev_nr                   = 0;
				pHD->allow_none_pnio_mautypes = LSA_TRUE;
				pHD->send_clock_factor 	      = pnioParam.send_clock;

				// StdMAC settings (only for EDDS)
				pHD->stdmac.use_setting     = LSA_TRUE;
				pHD->stdmac.gigabit_support = LSA_FALSE;

				// HW port settings
				pHD->hw_port[0].if_nr             = 1;
				pHD->hw_port[0].user_nr           = 1;
				pHD->hw_port[0].mrp_ringport_type = PSI_NO_RING_PORT;

				// PNIO IF settings common
				pHD->pnio_if[0].if_id     = 1;
				pHD->pnio_if[0].edd_if_id = m_startup.pnd_handle[idx].pnio_if_nr;

				// PNIO IF settings IOC
				pHD->pnio_if[0].ioc.use_setting           = LSA_TRUE;
				pHD->pnio_if[0].ioc.nr_of_rt_devices      = PSI_MAX_CL_DEVICES;
				pHD->pnio_if[0].ioc.nr_of_irt_devices     = 0;
				pHD->pnio_if[0].ioc.nr_of_qv_provider     = 0;
				pHD->pnio_if[0].ioc.nr_of_qv_consumer     = 0;
				pHD->pnio_if[0].ioc.logical_addr_space    = 8192;
				pHD->pnio_if[0].ioc.nr_of_submod          = 2048;
				pHD->pnio_if[0].ioc.max_iocr_data_size    = 1440;
				pHD->pnio_if[0].ioc.max_mcr_data_size     = 256;
				pHD->pnio_if[0].ioc.max_record_length     = PSI_MAX_CL_RECORD_SIZE;
				pHD->pnio_if[0].ioc.max_read_implicit     = 1;
				pHD->pnio_if[0].ioc.max_alarm_data_length = pnioParam.max_alarm_data_length;

				// PNIO IF settings OHA
				pHD->pnio_if[0].oha.use_setting       = LSA_TRUE;
				pHD->pnio_if[0].oha.vendor_id         = pnioParam.vendor_id;
				pHD->pnio_if[0].oha.device_id         = pnioParam.device_id;
				pHD->pnio_if[0].oha.check_ip_enabled  = pnioParam.check_ip_enabled;
				pHD->pnio_if[0].oha.check_interval    = pnioParam.check_interval;
                result = GetSysDescr(pHD->pnio_if[0].oha.sys_descr);
                PND_ASSERT( result == PNIO_OK );
                result = GetIfDescr(pHD->pnio_if[0].oha.if_descr);
                PND_ASSERT( result == PNIO_OK );

				// PNIO IF settings IP
				pHD->pnio_if[0].ip.use_setting = LSA_TRUE;
                		pHD->pnio_if[0].ip.nr_of_send  = 32;
				pHD->pnio_if[0].ip.nr_of_arp   = 32;
				pHD->pnio_if[0].ip.nr_of_icmp  = 32;
				pHD->pnio_if[0].ip.nr_of_udp   = 100;
				pHD->pnio_if[0].ip.nr_of_tcp   = 100;
				pHD->pnio_if[0].ip.multicast_support_on = LSA_FALSE;

				// PNIO IF settings MRP
				pHD->pnio_if[0].mrp.use_setting             = LSA_FALSE;
				pHD->pnio_if[0].mrp.max_instances           = 0;
				pHD->pnio_if[0].mrp.supported_role          = 0;
				pHD->pnio_if[0].mrp.default_role_instance0  = (PSI_MRP_ROLE_TYPE)EDD_MRP_ROLE_NONE;
				pHD->pnio_if[0].mrp.supported_multiple_role = 0;

				// PNIO IF settings NRT
				pHD->pnio_if[0].nrt.RxFilterUDP_Broadcast = LSA_TRUE;
				pHD->pnio_if[0].nrt.RxFilterUDP_Unicast   = LSA_TRUE;
			}
			break;
			default:
			{				
				PND_FATAL("CParamStore::prepareParamStartup(): Invalid HD selection");
			}
			break;
		}
	}
}

LSA_VOID CParamStore::SetRemaData(PNIO_UINT32 index, PNIO_UINT8* data)
{
	switch(index)
	{
		case 0x23000:	// TODO: --> define
            xml_free(m_remaData.ipSuiteRecord.recordData);
			m_remaData.ipSuiteRecord.recordData = data;
			break;
		case 0x23003:	// TODO: --> define
            xml_free(m_remaData.nosRecord.recordData);
			m_remaData.nosRecord.recordData = data;
			break;
	    case 0xF8000002: // TODO: --> define
            xml_free(m_remaData.mib2Record.recordData);
			m_remaData.mib2Record.recordData = data;
			break;
        default:
            break;
	};
}

/*----------------------------------------------------------------------------*/
LSA_VOID CParamStore::UpdateRema(PNIO_UINT8* data, PNIO_UINT32 dataLength, PNIO_UINT32 index, PNIO_UINT8** remaData, PNIO_UINT32* bufferLength)
{
	CValueHelper val;

	PNIO_UINT8* recData = LSA_NULL;
	
	recData = val.HexArrToCharArr(data, dataLength);
	SetRemaData(index, recData);
	
    m_remaData.mib2Record.recordLen = 2*dataLength;

	m_pParamHandlerXML->UpdateRema(m_remaData, remaData, bufferLength); //TODO m_remaData Only needed for update rema, will be extracted from Create methods
}

LSA_VOID_PTR_TYPE CParamStore::GetParamMultipleSNMPRecord()
{
    if(m_remaData.mib2Record.recordData == LSA_NULL)
    {
        return LSA_NULL;
    }

    return (LSA_VOID_PTR_TYPE)&m_remaData.mib2Record;
}

LSA_UINT32 CParamStore::IsSystemDescriptionValid(PNIO_SYSTEM_DESCR* pSysDescr)
{
    CValueHelper val;
    if(pSysDescr == LSA_NULL)
    {
        return PNIO_ERR_PRM_POINTER;
    }

    LSA_UINT32 delimiterLength = pnd_strlen(SYSTEM_DESCRIPTION_DELIMITER);
    LSA_UINT32 dotLength = pnd_strlen(SYSTEM_DESCRIPTION_DOT_FOR_FW_VERSION);
    LSA_UINT32 sysDescrLength = pnd_strlen((LSA_CHAR*)pSysDescr->Vendor) + delimiterLength
                                +pnd_strlen((LSA_CHAR*)pSysDescr->ProductFamily) + delimiterLength
                                +pnd_strlen((LSA_CHAR*)pSysDescr->IM_DeviceType) + delimiterLength
                                +pnd_strlen((LSA_CHAR*)pSysDescr->IM_OrderId) + delimiterLength
                                +pnd_strlen((LSA_CHAR*)SYSTEM_DESCRIPTION_HW_VERSION_PREFIX) + val.GetDigitCount(pSysDescr->IM_HwRevision) + delimiterLength
                                +pnd_strlen((LSA_CHAR*)SYSTEM_DESCRIPTION_FW_VERSION_PREFIX) + 1 
                                +val.GetDigitCount(pSysDescr->IM_SwVersion.functional_enhancement) + dotLength + val.GetDigitCount(pSysDescr->IM_SwVersion.bug_fix) + dotLength 
                                +val.GetDigitCount(pSysDescr->IM_SwVersion.internal_change) + delimiterLength                                                      
                                +pnd_strlen((LSA_CHAR*)SYSTEM_DESCRIPTION_PRODUCT_SERIAL_NUMBER_PREFIX) + pnd_strlen((LSA_CHAR*)pSysDescr->ProductSerialNr);

    if(sysDescrLength > MAX_SYSTEM_DESCRIPTION_LENGTH)
    {
        return PNIO_ERR_PRM_INVALIDARG;
    }

    return PNIO_OK;

}

LSA_UINT32 CParamStore::IsIfDescriptionValid(PNIO_SYSTEM_DESCR* pSysDescr)
{
    if(pSysDescr == LSA_NULL)
    {
        return PNIO_ERR_PRM_POINTER;
    }

    LSA_UINT32 ifDescrLength = pnd_strlen((LSA_CHAR*)pSysDescr->Vendor) + pnd_strlen((LSA_CHAR*)SYSTEM_DESCRIPTION_DELIMITER)
                               + pnd_strlen((LSA_CHAR*)pSysDescr->ProductFamily) + 24;

    if(ifDescrLength > MAX_IF_DESCRIPTION_LENGTH)
    {
        return PNIO_ERR_PRM_INVALIDARG;
    }

    return PNIO_OK;
}


LSA_UINT32 CParamStore::GetSysDescr(LSA_UINT8* pSysDescr)
{
    LSA_UINT32 result;

    result = IsSystemDescriptionValid(&m_systemDescription);

    if(result == PNIO_OK)
    {
        pnd_sprintf(pSysDescr, "%s%s%s%s%s%s%s%s%s%d%s%s%c%d%s%d%s%d%s%s%s", 
            m_systemDescription.Vendor, SYSTEM_DESCRIPTION_DELIMITER,
            m_systemDescription.ProductFamily, SYSTEM_DESCRIPTION_DELIMITER,
            m_systemDescription.IM_DeviceType, SYSTEM_DESCRIPTION_DELIMITER,
            m_systemDescription.IM_OrderId, SYSTEM_DESCRIPTION_DELIMITER,
            SYSTEM_DESCRIPTION_HW_VERSION_PREFIX, m_systemDescription.IM_HwRevision, SYSTEM_DESCRIPTION_DELIMITER,
            SYSTEM_DESCRIPTION_FW_VERSION_PREFIX, m_systemDescription.IM_SwVersion.revision_prefix,
            m_systemDescription.IM_SwVersion.functional_enhancement, SYSTEM_DESCRIPTION_DOT_FOR_FW_VERSION,
            m_systemDescription.IM_SwVersion.bug_fix, SYSTEM_DESCRIPTION_DOT_FOR_FW_VERSION,
            m_systemDescription.IM_SwVersion.internal_change, SYSTEM_DESCRIPTION_DELIMITER,
            SYSTEM_DESCRIPTION_PRODUCT_SERIAL_NUMBER_PREFIX, m_systemDescription.ProductSerialNr);
    }

    return result;
}

LSA_UINT32 CParamStore::GetIfDescr(LSA_UINT8* pIfDescr)
{
    LSA_UINT32 result;

    result = IsIfDescriptionValid(&m_systemDescription);

    if(result == PNIO_OK)
    {
        pnd_sprintf(pIfDescr, "%s%s%s", m_systemDescription.Vendor, SYSTEM_DESCRIPTION_DELIMITER, m_systemDescription.ProductFamily);
    }

    return result;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
