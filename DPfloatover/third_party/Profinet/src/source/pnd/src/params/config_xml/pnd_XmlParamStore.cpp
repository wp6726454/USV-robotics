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
/*  F i l e               &F: pnd_XmlParamStore.cpp                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PNIO XML storage class                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3201
#define PND_MODULE_ID       3201

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "cm_arcb_helper.h"
#include "pnioerrx.h"
#include "pniousrx.h"


/* StdLib++ */
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "pnd_ValueHelper.h"
#include "pnd_ParamStore.h"
#include "pnd_XmlParamStore.h"


/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

////////////////////////////////////////////////////////////////////////////////////
// CXmlParamStore  class to implement the param handling from XML-Files
////////////////////////////////////////////////////////////////////////////////////

CXmlParamStore::CXmlParamStore()
{
    for ( LSA_INT idx = 1; idx <= PSI_CFG_MAX_HD_CNT; idx++ )
    {
        m_pPnioStore[idx] = 0;
    }
}

/*----------------------------------------------------------------------------*/
CXmlParamStore::~CXmlParamStore()
{
    // Cleanup resources
    for ( LSA_INT idx = 1; idx <= PSI_CFG_MAX_HD_CNT; idx++ )
    {
        if ( m_pPnioStore[idx] != 0 )
        {
            delete ( m_pPnioStore[idx] );
        }

        m_pPnioStore[idx] = 0;
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::Create( LSA_UINT32 nrOfEntries, LSA_VOID* pParamsData, LSA_UINT32 paramLength, LSA_VOID* pRemaData, LSA_UINT32 remaLength, PND_XML_REMA_DATA_TYPE *remaData )
{
    LSA_UINT32 result = PNIO_OK;
    LSA_UINT8* null_char;

    // check if the xml file ends with null character
    null_char = (LSA_UINT8*)pParamsData+paramLength;
    if ( *null_char!= 0)
    {
        return PNIO_ERR_CORRUPTED_DATA;
    }
	
	if (pRemaData != LSA_NULL)
	{
		null_char = (LSA_UINT8*)pRemaData+remaLength;
		if ( *null_char!= 0)
		{
			return PNIO_ERR_INVALID_REMA;
		}
	}

    if (( nrOfEntries > 0 ) && ( nrOfEntries <= PSI_CFG_MAX_HD_CNT ))
    {
        // create the PNIO configuration from XML file
        for ( LSA_UINT16 hdNr = 1; hdNr <= nrOfEntries; hdNr++ )
        {
            m_pPnioStore[hdNr] = new CPnioXMLStore();
            XML_ASSERT( m_pPnioStore[hdNr] != 0 );

            result = m_pPnioStore[hdNr]->Create( hdNr,  pParamsData, pRemaData, remaData);

            if ( result != PNIO_OK )
			{
                break;
            }
        }
    }
    else
    {
        result = PNIO_ERR_PRM_INVALIDARG;
    }

    return ( result );
}

PARS_VOID (get_PARS_rema_record)(PARS_PND_REMA_RECORD_TYPE *pars_record, PND_REMA_RECORD_TYPE pnd_record)
{
	CValueHelper val;

	LSA_CHAR* strVal = (LSA_CHAR*)pnd_record.recordData;
	LSA_UINT32 dataLen = 0;
	LSA_UINT32 tempDataLen = 0;
	
	pars_record->recordLen = (LSA_CHAR*)xml_malloc(5);
	
	dataLen = strlen(strVal);

	tempDataLen = (dataLen >> 1) & 0x7FFFFFFF;

	pars_record->recordData = (LSA_CHAR*)xml_malloc(dataLen+1);

	PARS_memcpy((LSA_CHAR *)pars_record->recordLen, (LSA_CHAR*)(const_cast<char *>((val.UINT32ToString(tempDataLen)).c_str())), 5);
	PARS_memcpy((LSA_CHAR *)pars_record->recordData, pnd_record.recordData, dataLen+1);
}

LSA_VOID CXmlParamStore::UpdateRema(PND_XML_REMA_DATA_TYPE data, PNIO_UINT8** remaFileData, PNIO_UINT32* dataLength)
{
	PARS_PND_REMA_DATA_TYPE PARS_remaData;
	
	xml_memset( &PARS_remaData.ipSuiteRecord, 0, sizeof(PARS_remaData.ipSuiteRecord));
	xml_memset( &PARS_remaData.nosRecord, 0, sizeof(PARS_remaData.nosRecord));
	xml_memset( &PARS_remaData.mib2Record, 0, sizeof(PARS_remaData.mib2Record));

	if (data.ipSuiteRecord.recordData != LSA_NULL)
	{
		PARS_remaData.ipSuiteRecord.recordData = (PARS_LPSTR)data.ipSuiteRecord.recordData;
		get_PARS_rema_record(&PARS_remaData.ipSuiteRecord, data.ipSuiteRecord);
	}
	if (data.nosRecord.recordData != LSA_NULL)
	{
		PARS_remaData.nosRecord.recordData = (PARS_LPSTR)data.nosRecord.recordData;
		get_PARS_rema_record(&PARS_remaData.nosRecord, data.nosRecord);
	}
	if (data.mib2Record.recordData != LSA_NULL)
	{
		PARS_remaData.mib2Record.recordData = (PARS_LPSTR)data.mib2Record.recordData;
		get_PARS_rema_record(&PARS_remaData.mib2Record, data.mib2Record);
	}
	update_rema(PARS_remaData, (PARS_LPSTR**)remaFileData, dataLength);

    xml_free(PARS_remaData.ipSuiteRecord.recordData);
    xml_free(PARS_remaData.ipSuiteRecord.recordLen);
    xml_free(PARS_remaData.nosRecord.recordData);
    xml_free(PARS_remaData.nosRecord.recordLen);
    xml_free(PARS_remaData.mib2Record.recordData);
    xml_free(PARS_remaData.mib2Record.recordLen);
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::UpdatePnioIF( LSA_UINT16 hdNr, LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput )
{
    LSA_UINT32 result = PNIO_ERR_INVALID_CONFIG;
    // Update user settings based on calculated values from PSI
    if ( pOutput != 0 ) // Args OK ?
    {
        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( ifNr == 1 );

            m_pPnioStore[hdNr]->PrepareStore( ifNr, pOutput );

            result = PNIO_OK;
        }
    }

    return ( result );
}


/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::GetParamPnioIF( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PNIO_IF_PTR_TYPE p_params )
{
    // Read the PNIO IF setting for fixed IF-Nr=1
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( p_params != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            PND_XML_INTERFACE_PTR_TYPE ifInput  = m_pPnioStore[hdNr]->GetXMLInput( cpHandle->if_nr );

            p_params->edd_if_id             = ifInput->oha.oha_hd.edd_if_id;
            p_params->vendor_id             = ifInput->oha.oha_hd.vendor_id;
            p_params->device_id             = ifInput->oha.oha_hd.device_id;
            p_params->max_alarm_data_length = ifInput->ioc.ioc_hd.max_alarm_data_length;

            // parsing send_clock_factor from send clock data block, send_clock data starts by 6th byte
            p_params->send_clock            = (ifInput->pd.send_clock.record_data[6]<<8) + 
                                               ifInput->pd.send_clock.record_data[6+1];

            if ( ifInput->oha.oha_hd.check_ip_address )  // Check IP suite ?
            {
                // Enable the check options
                p_params->check_ip_enabled  = LSA_TRUE;
                p_params->check_interval    = 1; // cyclic every 5 sec
            }
            else
            {
                // Disable the check options
                p_params->check_ip_enabled  = LSA_FALSE;
                p_params->check_interval    = 0;
            }

            result = PNIO_OK;
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::GetParamOHA( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_OHA_HD_IF_PTR_TYPE p_params )
{
    // Read the PNIO OHA settings for fixed IF-Nr=1
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( p_params != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            *p_params = m_pPnioStore[hdNr]->GetXMLInputOHA( cpHandle->if_nr );
            result = PNIO_OK;
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::GetParamPD( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PD_HD_IF_PTR_TYPE  p_params )
{
    // Read the PNIO PD settings for fixed IF-Nr=1
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( p_params != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            *p_params = m_pPnioStore[hdNr]->GetXMLInputPD( cpHandle->if_nr );
            result = PNIO_OK;
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::GetParamIOC( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_IOC_HD_IF_PTR_TYPE p_params )
{
    // Read the PNIO IOC settings for fixed IF-Nr=1
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( p_params != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            *p_params = m_pPnioStore[hdNr]->GetXMLInputIOC( cpHandle->if_nr );
            result = PNIO_OK;
        }
    }

    return ( result );
}


/*----------------------------------------------------------------------------*/

LSA_UINT32 CXmlParamStore::CreatePrmRecordsPD( PND_HANDLE_PTR_TYPE cpHandle, PrmRecordVector* pRecList )
{
    // Read the PD records from IF, Port configuration
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( pRecList != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            // Note: if PDEV configuration is not central REMA must be integrated
            //       (the REMA stored records and the records from INI must be handled)
            LSA_BOOL state = m_pPnioStore[hdNr]->IsPdevConfigCentral( cpHandle->if_nr );
            XML_ASSERT( state );

            m_pPnioStore[hdNr]->GetPrmRecordsPD( cpHandle->if_nr, pRecList );

            result = PNIO_OK;
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::CreateDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList )
{
    // create device param and params for all subslots

    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( pDev != 0 ) && ( pSubList != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            result = m_pPnioStore[hdNr]->GetDeviceParam( cpHandle, devNr, pDev, pSubList );
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::CreateArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd )
{
    // Create ARCB for one device

    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( pAdd != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            m_pPnioStore[hdNr]->GetArcbAddCL( cpHandle, devNr, hwType, pAdd );

            result = PNIO_OK;
        }
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CXmlParamStore::CreateArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE pRead )
{
    // Create ARCB Read implicit for one device
    LSA_UINT32 result = PNIO_ERR_PRM_INVALIDARG;

    if (( cpHandle != 0 ) && ( pRead != 0 )) // Args OK ?
    {
        LSA_UINT16 hdNr = cpHandle->hd_nr;

        if (( hdNr <= PSI_CFG_MAX_HD_CNT ) && // HD Nr valid ?
            ( m_pPnioStore[hdNr] != 0 ))
        {
            // Read the PNIO setting for fixed IF-Nr=1
            XML_ASSERT( cpHandle->if_nr == 1 );

            m_pPnioStore[hdNr]->GetArcbReadImpl( cpHandle, devNr, pRead );

            result = PNIO_OK;
        }
    }

    return ( result );
}


/////////////////////////////////////////////////////////////////////////////
// CPnioXMLStore PNIO XML HD Config storage class 
/////////////////////////////////////////////////////////////////////////////

CPnioXMLStore::CPnioXMLStore()
{
    m_hdNr   = 0;
    m_nrOfIF = 0;
    m_XMLDoc = LSA_NULL;
    m_RootXMLElement = LSA_NULL;

    for (LSA_UINT16 i = 0; i<(PSI_CFG_MAX_HD_IF_CNT+1); i++)
    {
        m_InternalXMLData[i].pd.prm_record_list.clear();
        xml_memset( &m_InternalXMLData[i].oha, 0, sizeof(m_InternalXMLData[i].oha));
        xml_memset( &m_InternalXMLData[i].ioc, 0, sizeof(m_InternalXMLData[i].ioc));
        xml_memset( &m_InternalXMLData[i].pd.pd_hd, 0, sizeof(m_InternalXMLData[i].pd.pd_hd));
        xml_memset( &m_InternalXMLData[i].pd.ip_v4_suite, 0, sizeof(m_InternalXMLData[i].pd.ip_v4_suite));
        m_InternalXMLData[i].pd.ip_address_validation_local = 0;
        m_InternalXMLData[i].pd.name_of_station_validation = 0;
        xml_memset( &m_InternalXMLData[i].pd.name_of_station, 0, sizeof(m_InternalXMLData[i].pd.name_of_station));
        xml_memset( &m_InternalXMLData[i].pd.send_clock, 0, sizeof(m_InternalXMLData[i].pd.send_clock));

		xml_memset( &m_InternalRemaData.base.ip_v4_suite, 0, sizeof(m_InternalRemaData.base.ip_v4_suite));
		xml_memset( &m_InternalRemaData.base.name_of_station, 0, sizeof(m_InternalRemaData.base.name_of_station));
		xml_memset( &m_InternalRemaData.mib2, 0, sizeof(m_InternalRemaData.mib2));

		m_InternalRemaData.base.ip_address_validation_local = 0;
		m_InternalRemaData.base.name_of_station_validation = 0;

		xml_memset( &m_InternalRemaDataArr[i].base.ip_v4_suite, 0, sizeof(m_InternalRemaDataArr[i].base.ip_v4_suite));
		xml_memset( &m_InternalRemaDataArr[i].base.name_of_station, 0, sizeof(m_InternalRemaDataArr[i].base.name_of_station));
		m_InternalRemaDataArr[i].base.ip_address_validation_local = 0;
		m_InternalRemaDataArr[i].base.name_of_station_validation = 0;
        

        m_InternalXMLData[i].iod_list.clear();
		m_InternalXMLData[i].iod_list.resize(PSI_MAX_CL_DEVICE_NR + 1);
    }
}

/*----------------------------------------------------------------------------*/
CPnioXMLStore::~CPnioXMLStore()
{
    LSA_UINT16 j, k, l, m, count1, count2, count3, count4, iod_count;
    j = 0;
    k = 0;
    l = 0;
    m = 0;
    count1 = 0;
    count2 = 0;
    count3 = 0;
    count4 = 0;
    iod_count = 0;

    xml_free(m_InternalRemaData.base.ip_v4_suite.record_data);
    xml_free(m_InternalRemaData.base.name_of_station.record_data);
    xml_free(m_InternalRemaData.mib2.record_data);

	// Cleanup resources
    for ( LSA_UINT16 ifNr = 1; ifNr <= m_nrOfIF; ifNr++ )
    {
        xml_free(m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data);
        xml_free(m_InternalXMLData[ifNr].pd.name_of_station.record_data);
        xml_free(m_InternalXMLData[ifNr].pd.send_clock.record_data);

		xml_free(m_InternalRemaDataArr[ifNr].base.ip_v4_suite.record_data);
		xml_free(m_InternalRemaDataArr[ifNr].base.name_of_station.record_data);
	
        if (( &m_InternalXMLData[ifNr].pd.prm_record_list != 0 ) && 
            ( !m_InternalXMLData[ifNr].pd.prm_record_list.empty() ))
        {
            // cleanup the created PRM records
            count1 = (LSA_UINT16)m_InternalXMLData[ifNr].pd.prm_record_list.size();

            for ( LSA_UINT16 i = 0; i < count1; i++ )
            {
                PND_PARAM_PRM_RECORD_PTR_TYPE pRec = m_InternalXMLData[ifNr].pd.prm_record_list.at(i);

                if ( pRec != 0 )
                {
                    if ( pRec->record_data != 0 )
                    {
                        xml_free( pRec->record_data );
                    }

                    xml_free( pRec );
                }
            }

            // Cleanup the list (with empty values)
            m_InternalXMLData[ifNr].pd.prm_record_list.clear();
        }

        // Cleanup the device list
        if (( &m_InternalXMLData[ifNr].iod_list != 0 ) && 
            ( !m_InternalXMLData[ifNr].iod_list.empty() ))
        {

            iod_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.size();

            for ( LSA_UINT16 i = 0; i < iod_count; i++ )
            {
				if(m_InternalXMLData[ifNr].iod_list.at(i) == LSA_NULL)
				{
					continue;
				}

                xml_free(m_InternalXMLData[ifNr].iod_list.at(i)->name_of_station);

                // DFP IOCR Entries
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->ir_info_data.dfp_iocr_list != 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->ir_info_data.dfp_iocr_list.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->ir_info_data.dfp_iocr_list.size();
                    for (j = 0; j < count1; j++)
                    {
                        xml_free (m_InternalXMLData[ifNr].iod_list.at(i)->ir_info_data.dfp_iocr_list.at(j));
                    }
                m_InternalXMLData[ifNr].iod_list.at(i)->ir_info_data.dfp_iocr_list.clear();
                }
                // AR Record Data Entries
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list != 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list.size();
                    for (j = 0; j < count1; j++)
                    {
                        PND_XML_RECORD_PTR_TYPE pRec = m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list.at(j);

                        if ( pRec != 0 )
                        {
                            if ( pRec->record_data != 0 )
                            {
                                xml_free( pRec->record_data );
                            }

                            xml_free( pRec );
                        }            
                        //delete m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list.at(j);
                    }
                    m_InternalXMLData[ifNr].iod_list.at(i)->ar_record_list.clear();
                }
                // AliasName Entries
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list != 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.size();
                    for (j = 0; j < count1; j++)
                    {
                        if (m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.at(j)->alias_name != LSA_NULL)
                            xml_free (m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.at(j)->alias_name);
                        delete (m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.at(j));
                    }
                    m_InternalXMLData[ifNr].iod_list.at(i)->station_name_alias_list.clear();
                }
                // IO CR Data
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data!= 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.size();
                    for (j = 0; j < count1; j++)
                    {
                        if (( &m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list != 0 ) && 
                            ( !m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.empty() ))
                        {
                            count2 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.size();
                            for (k = 0; k < count2; k++)
                            {

                                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iodata_list != 0 ) && 
                                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iodata_list.empty() ))
                                {
                                    count3 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iodata_list.size();
                                    for (l=0; l<count3; l++)
                                    {
                                        xml_free (m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iodata_list.at(l));
                                    }
                                    m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iodata_list.clear();
                                }

                                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iocs_list != 0 ) && 
                                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iocs_list.empty() ))
                                {
                                    count3 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iocs_list.size();
                                    for (l=0; l<count3; l++)
                                    {
                                        xml_free (m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iocs_list.at(l));
                                    }
                                    m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k)->iocs_list.clear();
                                }
                                delete m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.at(k);
                            }
                            m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j)->api_list.clear();
                        }
                        delete m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.at(j);
                    }
                    m_InternalXMLData[ifNr].iod_list.at(i)->iocr_data.clear();
                }
        
                // Local SCF adaptation entries
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->local_scf_list != 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->local_scf_list.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->local_scf_list.size();
                    for (j = 0; j < count1; j++)
                    {
                        delete m_InternalXMLData[ifNr].iod_list.at(i)->local_scf_list.at(j);
                    }
                    m_InternalXMLData[ifNr].iod_list.at(i)->local_scf_list.clear();
                }
                // Expected Submodule Data
                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data != 0 ) && 
                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.empty() ))
                {
                    count1 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.size();
                    for (j = 0; j < count1; j++)
                    {
                        if (( &m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list != 0 ) && 
                            ( !m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.empty() ))
                        {
                            count2 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.size();
                            for (k = 0; k < count2; k++)
                            {
                                if (( &m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list != 0 ) && 
                                    ( !m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.empty() ))
                                {
                                    count3 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.size();
                                    for(l=0; l < count3; l++)
                                    {
                                        if (( &m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_submodule_descr_list != 0 ) && 
                                            ( !m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_submodule_descr_list.empty() ))                    
                                        {
                                            count4 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_submodule_descr_list.size();
                                            for (m=0; m < count4; m++)
                                            {
                                                delete m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_submodule_descr_list.at(m);
                                            }
                                            m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_submodule_descr_list.clear();
                                        }

                                        if (( &m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list != 0 ) && 
                                            ( !m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.empty() ))                    
                                        {
                                            count4 = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.size();
                                            for (m=0; m < count4; m++)
                                            {
                                                if(m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.at(m)->record.record_data != 0)
                                                    xml_free (m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.at(m)->record.record_data);
                                                delete m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.at(m);
                                            }
                                            m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l)->arcb_record_list.clear();
                                        }
                                        delete m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.at(l);
                                    }
                                    m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k)->arcb_submodule_list.clear();
                                }
                                delete m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.at(k);
                            }
                            m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j)->arcb_slot_list.clear();
                        }
                        delete m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.at(j);
                    }
                    m_InternalXMLData[ifNr].iod_list.at(i)->exp_submod_data.clear();
                } 
                delete m_InternalXMLData[ifNr].iod_list.at(i);  
            }
            // Cleanup the list
            m_InternalXMLData[ifNr].iod_list.clear();
        }
    }

}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPnioXMLStore::Create( LSA_UINT16 hdNr, LSA_VOID* pParamsData, LSA_VOID* pRemaData, PND_XML_REMA_DATA_TYPE* remaData )
{
    LSA_UINT16 result = PNIO_OK;

    if (( hdNr > 0 ) && ( hdNr <= PSI_CFG_MAX_HD_CNT )) // HD in range ?
    {
        m_hdNr = hdNr;

        initializeXMLParser();

        // get number of IOinterfaces
        m_nrOfIF = getNrOfIF();
        XML_ASSERT((m_nrOfIF > 0 ) && ( m_nrOfIF <= PSI_CFG_MAX_HD_IF_CNT));


        // Read the PNIO settings from XML file for each IF
        // Note: this settings are used for EDDx and PNIO User setup
        for ( LSA_UINT16 ifNr = 1; ifNr <= m_nrOfIF; ifNr++ )
        {
            result = readParametersFromXMLFiles(ifNr, (LSA_CHAR*) pParamsData, (LSA_CHAR*)pRemaData, remaData);
        }

    }
    else
    {
        result = PNIO_ERR_PRM_INVALIDARG;
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::initializeXMLParser()
{
    xml_stdint_parser_init();
}

/*----------------------------------------------------------------------------*/
PND_XML_INTERFACE_PTR_TYPE CPnioXMLStore::GetXMLInput( LSA_UINT16 ifNr )
{
    XML_ASSERT((ifNr > 0 ) && ( ifNr <= m_nrOfIF));

    PND_XML_INTERFACE_PTR_TYPE result = &m_InternalXMLData[ifNr];

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::PrepareStore( LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput )
{
    // Prepare the PNIO settings for PNIO users from calculated outputs
    // Note: 2.nd prepare for complete settings after LD open
    XML_ASSERT((ifNr > 0 ) && ( ifNr <= m_nrOfIF));
    XML_ASSERT( pOutput != 0 );

    // check if User port count matches with PD map count
    XML_ASSERT( m_InternalXMLData[ifNr].pd.pd_hd.map_count <= ( pOutput->nr_of_usr_ports +1) );

    // Add PNIO User settings from outputs 
    // Note: used by CM Create services
    prepareFromOutputIOC( ifNr, pOutput );
    prepareFromOutputOHA( ifNr, pOutput );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CPnioXMLStore::IsPdevConfigCentral( LSA_UINT16 ifNr )
{
    LSA_BOOL result;

    XML_ASSERT(( ifNr > 0 ) && ( ifNr <= PSI_CFG_MAX_HD_IF_CNT));

    if (m_InternalXMLData[ifNr].pd.pd_hd.pdev_validation == 0x0000)
    {
        result = LSA_TRUE;
    }
    else
    {
        result = LSA_FALSE;
    }

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::GetPrmRecordsPD( LSA_UINT16 ifNr, PrmRecordVector* pRecList )
{
    LSA_UINT16 i;
    LSA_UINT32 size;
    PND_PARAM_PRM_RECORD_PTR_TYPE pRecord;

    i                   = 0;
    size                = 0;
    pRecord             = LSA_NULL;

    XML_ASSERT(( ifNr > 0 ) && ( ifNr <= PSI_CFG_MAX_HD_IF_CNT));
    XML_ASSERT( pRecList != 0 );

    // read the records configured as PD startup section

    size = m_InternalXMLData[ifNr].pd.prm_record_list.size();

    for(i=0; i<size; i++)
    {
        PND_PARAM_PRM_RECORD_PTR_TYPE pEntry;
        pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
        XML_ASSERT( pEntry != 0 );

        pRecord = m_InternalXMLData[ifNr].pd.prm_record_list.at(i);
        pEntry->slot_nr             = pRecord->slot_nr;
        pEntry->subslot_nr          = pRecord->subslot_nr;
        pEntry->record_index        = pRecord->record_index;
        pEntry->record_data_length  = pRecord->record_data_length;

        pEntry->record_data = (LSA_UINT8*)xml_malloc( pEntry->record_data_length );
        XML_ASSERT( pEntry->record_data != 0 );
        xml_memcpy(pEntry->record_data, pRecord->record_data, pEntry->record_data_length);
        pRecList->push_back(pEntry);
    }

}

/*----------------------------------------------------------------------------*/
PND_PARAM_PD_HD_IF_TYPE CPnioXMLStore::GetXMLInputPD( LSA_UINT16 ifNr )
{
    XML_ASSERT((ifNr > 0 ) && ( ifNr <= m_nrOfIF));

    PND_PARAM_PD_HD_IF_TYPE result = m_InternalXMLData[ifNr].pd.pd_hd;

    return ( result );
}

/*----------------------------------------------------------------------------*/
PND_PARAM_IOC_HD_IF_TYPE CPnioXMLStore::GetXMLInputIOC( LSA_UINT16 ifNr )
{
    XML_ASSERT((ifNr > 0 ) && ( ifNr <= m_nrOfIF));

    PND_PARAM_IOC_HD_IF_TYPE result =m_InternalXMLData[ifNr].ioc.ioc_hd;

    return ( result );
}

/*----------------------------------------------------------------------------*/
PND_PARAM_OHA_HD_IF_TYPE CPnioXMLStore::GetXMLInputOHA( LSA_UINT16 ifNr )
{
    XML_ASSERT((ifNr > 0 ) && ( ifNr <= m_nrOfIF));

    PND_PARAM_OHA_HD_IF_TYPE result = m_InternalXMLData[ifNr].oha.oha_hd;

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPnioXMLStore::GetDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList  )
{
    LSA_UINT16    ifNr;
    LSA_UINT32    api_count, slot_count, subslot_count, subslot_descr_count;

    XML_ASSERT( pDev != 0 );
    XML_ASSERT( pSubList != 0 );
    XML_ASSERT( cpHandle != 0 );
    XML_ASSERT( cpHandle->hd_nr == m_hdNr );
    XML_ASSERT((cpHandle->if_nr > 0 ) && ( cpHandle->if_nr <= m_nrOfIF));

    api_count           = 0;
    slot_count          = 0;
    subslot_count       = 0;
    subslot_descr_count = 0;
    ifNr                = cpHandle->if_nr;

	// Check if devNr is in range
	if( devNr >= m_InternalXMLData[ifNr].iod_list.size() )
	{
		return PNIO_ERR_PRM_INVALIDARG;
	}

	// Check if the object exists.
	if( m_InternalXMLData[ifNr].iod_list.at(devNr) == LSA_NULL)
	{
		return PNIO_ERR_PRM_INVALIDARG;
	}

    // Prepare device setting
    pDev->is_activate  = m_InternalXMLData[ifNr].iod_list.at(devNr)->is_active;
    pDev->vendor_id    = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.vendor_id;
    pDev->device_id    = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.device_id;
    pDev->instance     = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.instance_id;
    pDev->device_laddr = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.device_laddr;
	pDev->device_number= m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.device_number;


    api_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.size();

    // Get the subslot configuration from expected config
    for ( LSA_UINT16 i = 0; i < api_count; i++)
    {
        slot_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.size();

        for ( LSA_UINT16 j = 0; j < slot_count; j++)
        {
            subslot_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.size();
            for ( LSA_UINT16 k = 0; k < subslot_count; k++ )
            {
                LSA_UINT8 submodule_type;
                submodule_type = 0;
                // Add found subslot to result list
                PND_PARAM_SUBSLOT_PTR_TYPE pSubmod = (PND_PARAM_SUBSLOT_PTR_TYPE)xml_malloc( sizeof( PND_PARAM_SUBSLOT_TYPE));
                XML_ASSERT( pSubmod != 0 );

                // Subslot attributes
                pSubmod->api                = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->api_nr;
                pSubmod->slot_nr            = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->slot_nr;
                pSubmod->slot_laddr         = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->laddr;
                pSubmod->slot_ident         = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->module_ident;
                pSubmod->subslot_nr         = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->subslot_nr;
                pSubmod->subslot_laddr      = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->laddr;
                pSubmod->subslot_ident      = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->sub_ident;
                pSubmod->subslot_properties = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->sub_properties;

                // set default values
                pSubmod->input_length       = 0;
                pSubmod->input_base_laddr   = PND_LADDR_INVALID;
                pSubmod->output_length      = 0;
                pSubmod->output_base_laddr  = PND_LADDR_INVALID;
                // overwrite default values
                subslot_descr_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.size();
                for( LSA_UINT16 m = 0; m < subslot_descr_count; m++)
                {
                    if (m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(m)->data_description == 1)
                    {
                        pSubmod->input_length     = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(m)->data_length;
                        pSubmod->input_base_laddr = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(m)->i_base_addr;
                    }
                    else
                    {
                        pSubmod->output_length     = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(m)->data_length;
                        pSubmod->output_base_laddr = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(m)->q_base_addr;            
                    }
                }

				// set other subslot attributes
				pSubmod->reduction_ratio = m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(0)->reduction_ratio; // iocr_data.at(0) -> input CR / for reading output CR, iocr_data.at(1)  needed. For PnDriver V1.1  Input and Output CR values will be the same.
				pSubmod->phase = m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(0)->phase;                     // iocr_data.at(0) -> input CR / for reading output CR, iocr_data.at(1)  needed. For PnDriver V1.1  Input and Output CR values will be the same.
				pSubmod->cycle_time = m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(0)->send_clock;           // iocr_data.at(0) -> input CR / for reading output CR, iocr_data.at(1)  needed. For PnDriver V1.1  Input and Output CR values will be the same.

                pSubList->push_back( pSubmod );
            } // Subslot
        } // Slot
    } // Api

    pDev->nr_of_subslot = (LSA_UINT16)pSubList->size();

	return PNIO_OK;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::GetArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd )
{
    LSA_UINT16    ifNr, nare_properties;

    XML_ASSERT( pAdd != 0 );
    XML_ASSERT( cpHandle != 0 );
    XML_ASSERT( cpHandle->hd_nr == m_hdNr );
    XML_ASSERT((cpHandle->if_nr > 0 ) && ( cpHandle->if_nr <= m_nrOfIF));

    nare_properties     = 0;
    ifNr                = cpHandle->if_nr;

    // derive nare_properties from ip_address_validation_remote

    if (m_InternalXMLData[ifNr].iod_list.at(devNr)->ip_address_validation_remote == 0xFFFF)
    {
        nare_properties = 1; // IPSuite configured on site 
    }
    if((m_InternalXMLData[ifNr].iod_list.at(devNr)->ip_address_validation_prop & 0x10) == 0x10)
    {
        nare_properties |= 0x10;  // set Bit 4 for "set IP suite permanent"
    }

    // Prepare the "PSI sysptr" used from CM to address the pools over output macros
    // Note: only the PSI addressing elements are set
    PSI_SYS_TYPE locSys;
    xml_memset( &locSys, 0, sizeof( PSI_SYS_TYPE ));

    // Prepare the HD/IF Keys for PSI addressing only
    locSys.hd_nr       = cpHandle->hd_nr;
    locSys.if_nr       = cpHandle->if_nr;
    locSys.pnio_if_nr  = cpHandle->pnio_if_nr;
    locSys.comp_id_edd = cpHandle->edd_comp_id;

    pAdd->device_nr                 = devNr;
    pAdd->planned_mode              = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.planned_mode;
    pAdd->run_mode                  = 0;

    // check StartupMode
    // if "Advanced" (1), CMIActivityTimeout for IODevices must be read from AR_COMMUNICATION_DATA block
    // if "Legacy" (0), CMIActivityTimeout for IODevices is read from the CONTROLLER_PROPERTIES block
    if ( m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.ar_properties & (1 << 30))
    {
        pAdd->cmi_activity_timeout100ms = m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.cmi_timeout100ms;
    }
    else
    {
        pAdd->cmi_activity_timeout100ms = m_InternalXMLData[ifNr].ioc.cmi_timeout100ms;
    }
    pAdd->pnio_status               = CM_PNIO_ERR_NONE;

    // Note: address_mode in SDB is LSA_UINT32 (first 5 Bits used) but CM interface defines with LSA_UINT16
    cm_arcb_set_nare( // Prepare the CM ARCB NARE part
        &locSys, 
        &pAdd->dev_addr, 
        nare_properties,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->name_of_station_length,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->name_of_station,
        xml_htonl( m_InternalXMLData[ifNr].iod_list.at(devNr)->ip_address ) ); 

    for (LSA_UINT16 i = 0; i < m_InternalXMLData[ifNr].iod_list.at(devNr)->station_name_alias_list.size(); i++)
    {
        // Note: cm_arcb creates the allocated memory for the alias
        cm_arcb_append_nare_alias( // Prepare the CM ARCB NARE alias list
            &locSys,
            &pAdd->dev_addr, 
            m_InternalXMLData[ifNr].iod_list.at(devNr)->station_name_alias_list.at(i)->name_length,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->station_name_alias_list.at(i)->alias_name);   
    }

    LSA_UINT32 maxRecSize = m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.max_record_size;

    // Check for > 64 K (if greater limit it)
    maxRecSize += (( maxRecSize + CM_RECORD_OFFSET)  < 0x10000 ) ? CM_RECORD_OFFSET : 0;

    cm_arcb_set_rpc( // Prepare the CM ARCB RPC settings
        &locSys,
        &pAdd->dev_rpc,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.vendor_id, 
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.device_id, 
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.instance_id,
        0, /*appl_timeout1sec*/
        maxRecSize,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.write_multiple_is_supported );

    CmListInitialize( &pAdd->ar_list );

    CM_UPPER_ARCB_ADD_PTR_TYPE pARCB = (CM_UPPER_ARCB_ADD_PTR_TYPE)createArcbExpectedConfig( cpHandle, devNr, &locSys );
    XML_ASSERT( pARCB != 0 );

    if ( !cm_arcb_calc_iocr_buffer_size( pARCB, LSA_TRUE, hwType ))
    {
        PND_PARAMS_TRACE_01( cpHandle->pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
            "CPnioIniStore::GetArcbAddCL: ARCB creation, some bug in the related list found, devNr(%d)",
            devNr );
    }

    CmListInsertTail( &pAdd->ar_list, &pARCB->link );

}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::GetArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE pRead )
{
    LSA_UINT16    ifNr;

    XML_ASSERT( pRead != 0 );
    XML_ASSERT( cpHandle != 0 );
    XML_ASSERT( cpHandle->hd_nr == m_hdNr );
    XML_ASSERT((cpHandle->if_nr > 0 ) && ( cpHandle->if_nr <= m_nrOfIF));

    ifNr                = cpHandle->if_nr;

    // Prepare the "PSI sysptr" used from CM to address the pools over output macros
    // Note: only the PSI addressing elements are set
    PSI_SYS_TYPE locSys;
    xml_memset( &locSys, 0, sizeof( PSI_SYS_TYPE ));

    // Prepare the HD/IF Keys for PSI addressing only
    locSys.hd_nr       = cpHandle->hd_nr;
    locSys.if_nr       = cpHandle->if_nr;
    locSys.pnio_if_nr  = cpHandle->pnio_if_nr;
    locSys.comp_id_edd = cpHandle->edd_comp_id;

    CM_UPPER_ARCB_NARE_PTR_TYPE pNARE = &pRead->dev_addr;
    CM_UPPER_ARCB_RPC_PTR_TYPE  pRPC  = &pRead->dev_rpc;

    cm_arcb_set_nare(
        &locSys, 
        pNARE, 
        CM_NARE_MODE_USE_IP,
        0,
        0,
        xml_htonl( m_InternalXMLData[ifNr].iod_list.at(devNr)->ip_address ) );

    cm_arcb_set_rpc( // Prepare the CM ARCB RPC settings
        &locSys, 
        pRPC,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.vendor_id, 
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.device_id, 
        m_InternalXMLData[ifNr].iod_list.at(devNr)->pniod_properties.instance_id,
        0, /*appl_timeout1sec*/
        0,
        LSA_FALSE );

}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getNrOfHDs()
{
    LSA_UINT16 HDNr = 0;
    xml_element element, IOinterface;

    element = LSA_NULL;
    IOinterface = LSA_NULL;

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1);
    if (element == LSA_NULL)
        return 0;
    // count number of IOinterfaces
    do
    {
        HDNr++;
        IOinterface = xml_locate_element_by_child_content(element, "ClassRID", "3", HDNr);
    }
    while (IOinterface != LSA_NULL);

    return(HDNr - 1);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getNrOfIF()
{
    return 1; // currently only 1 interface per HD is supported
}

/*----------------------------------------------------------------------------*/
xml_doc CPnioXMLStore::createXMLDoc(LSA_CHAR* pXMLBuffer)
{
    return xml_stdint_parse_from_buffer(pXMLBuffer);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::freeXMLDoc(xml_doc XMLDoc)
{
    xml_stdint_free_doc(XMLDoc);
}

/*----------------------------------------------------------------------------*/
xml_element CPnioXMLStore::getRootXMLElement(xml_doc XMLDoc)
{
    xml_element element, root_element;

    // Always return HWConfiguration (ClassRID = 1) as a root element

    // get root element of the XML document
    root_element = xml_stdint_get_root_element(XMLDoc);

    if (root_element == LSA_NULL)
    {
        return LSA_NULL;
    }

    // check if the root element of the XML document is HWConfiguration
    element = xml_locate_element_by_child_content(root_element, "ClassRID", "1", 1);
    if (element == LSA_NULL)
    {
        return root_element;    // HWConfiguration is root element 
    }
    else
    {
        return element;         // ExtendedFile is root element
    }
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::readParametersFromXMLFiles(LSA_UINT16 ifNr, LSA_CHAR* pXMLBuffer, LSA_CHAR* pRemaBuffer, PND_XML_REMA_DATA_TYPE* remaData)
{
	LSA_UINT16 result = PNIO_OK;
	
    if(pRemaBuffer != PNIO_NULL)
    {
        result = readRemaParametersFromXMLFile(pRemaBuffer, remaData);
    }
	
	if (result == PNIO_OK)
	{
		result = readPNIOParametersFromXMLFile(ifNr, pXMLBuffer);
	}
	
	return result;
}

LSA_UINT16 CPnioXMLStore::readRemaParametersFromXMLFile(LSA_CHAR* pRemaBuffer, PND_XML_REMA_DATA_TYPE* remaData)
{
	LSA_UINT16 result;
  
	m_XMLDoc = createXMLDoc(pRemaBuffer);

    if (m_XMLDoc == LSA_NULL)
    {
        return PNIO_ERR_INVALID_REMA;
    }

    m_RootXMLElement = getRootXMLElement(m_XMLDoc);

    if (m_RootXMLElement == LSA_NULL)
    {
        return PNIO_ERR_INVALID_REMA;
    }

	result = getRemaNetworkParams(remaData);

    freeXMLDoc(m_XMLDoc);

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::readPNIOParametersFromXMLFile(LSA_UINT16 ifNr, LSA_CHAR* pXMLBuffer)
{
    LSA_UINT16 result, i;

    result  = PNIO_ERR_INVALID_CONFIG;
    i       = 0;

    m_XMLDoc = createXMLDoc(pXMLBuffer);

    if (m_XMLDoc == LSA_NULL)
    {
        return PNIO_ERR_CORRUPTED_DATA;
    }

    m_RootXMLElement = getRootXMLElement(m_XMLDoc);

    if (m_RootXMLElement == LSA_NULL)
    {
        return PNIO_ERR_CORRUPTED_DATA;
    }

    /////////////////////
    // CentralIOsystem //
    /////////////////////
   result = getPDNetworkParams(ifNr);
    if(result == PNIO_OK)
    {
        result = getPDParams(ifNr);
        if(result == PNIO_OK)
        {
            result = getPDSubslotInfo(ifNr);
            if(result == PNIO_OK)
            {
                result = getIOCParams(ifNr);
                if (result == PNIO_OK)
                {
                    result = getOHAParams(ifNr);
                    if (result == PNIO_OK)
                    {
                        result = getPDDataRecords(ifNr);
                    }
                }
            }
        }
    }

    ///////////////////////
    // DecentralIOsystem //
    ///////////////////////
    if (result == PNIO_OK)
    {
        for (i = 1; i <= m_InternalXMLData[ifNr].ioc.ioc_hd.device_count; i++)
        {
            PND_XML_IOD_PTR_TYPE pDevice = new  pnd_xml_iod_tag;

            result = getDeviceNetworkParams(i, &pDevice->ip_address, &pDevice->name_of_station, &pDevice->name_of_station_length, 
                                    &pDevice->ip_address_validation_remote, &pDevice->ip_address_validation_prop, &pDevice->station_name_alias_list);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getDeactivatedConfig(i, &pDevice->is_active);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getIODProperties(i, &pDevice->pniod_properties);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getAlarmCRData(i, &pDevice->alarm_cr_data);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getARCommunicationData(i, &pDevice->ar_data);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getIOCRData(i, &pDevice->iocr_data);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getExpectedSubmoduleData(i, &pDevice->exp_submod_data);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getOtherSubmoduleData(i, &pDevice->exp_submod_data);
            if (result != PNIO_OK)
            {
                return result;
            }

            result = getLocalSCFAdaption(i, &pDevice->local_scf_list);  // optional, may return PNIO_ERR_INVALID_CONFIG

            result = getARRecordData(i, &pDevice->ar_record_list);      // optional, may return PNIO_ERR_INVALID_CONFIG

            result = PNIO_OK;

            m_InternalXMLData[ifNr].iod_list[pDevice->pniod_properties.device_number] = pDevice;
        }
    }
    // free the XML document
    freeXMLDoc(m_XMLDoc);

    return result;
}

LSA_UINT16 CPnioXMLStore::getPDNetworkParams(LSA_UINT16 ifNr)
{
	//LSA_UINT32 value;
    LSA_UINT16      result;
    xml_element     element, network_element, iointerface_element;

    result              = PNIO_ERR_CORRUPTED_DATA;
    element             = LSA_NULL;
    network_element     = LSA_NULL;
    iointerface_element = LSA_NULL;

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        iointerface_element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (iointerface_element != LSA_NULL)
        {
            element = xml_locate_element_by_child_content(iointerface_element, "AID", "11", 1); // DataRecordsConf
            if (element != LSA_NULL)
            {
                network_element = xml_stdint_get_element_by_name(element, "Value");
            }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }                        
        }
    }

    if (network_element != LSA_NULL) 
    {

        // IP_ADDRESS_VALIDATION_LOCAL
        element = xml_stdint_get_element_by_attribute(network_element, "Key", "4097");
        if (element != LSA_NULL) 
        {
            PND_XML_RECORD_TYPE record;
            result = getRecord(element, &record);
            if(result == PNIO_OK)
            {
                m_InternalXMLData[ifNr].pd.ip_address_validation_local = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);
                xml_free(record.record_data);
            }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }
        }
        else
        {
            return PNIO_ERR_CORRUPTED_DATA;
        }

        // NAMEOFSTATION_VALIDATION
        element = xml_stdint_get_element_by_attribute(network_element, "Key", "4100");
        if (element != LSA_NULL) 
        {
            PND_XML_RECORD_TYPE record;
            result = getRecord(element, &record);
            if(result == PNIO_OK)
            {
                m_InternalXMLData[ifNr].pd.name_of_station_validation = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);
                xml_free(record.record_data);
            }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }
        }
        else
        {
            return PNIO_ERR_CORRUPTED_DATA;
        }

        // IP_ADDRESS_VALIDATION_LOCAL must be 0 (use IPSuite) for PN-Driver
        if (m_InternalXMLData[ifNr].pd.ip_address_validation_local == 0)
        {
            // IPV4_SUITE
            element = xml_stdint_get_element_by_attribute(network_element, "Key", "4096");  // try 0x1000
            if (element == LSA_NULL)
            {
                element = xml_stdint_get_element_by_attribute(network_element, "Key", "12288"); // try 0x3000
            }
            if (element != LSA_NULL) 
            {
                result = getRecord(element, &m_InternalXMLData[ifNr].pd.ip_v4_suite);
                if(result != PNIO_OK)
                {
                    return PNIO_ERR_CORRUPTED_DATA;
                }
                else
                {
                    m_InternalXMLData[ifNr].pd.ip_v4_suite.record_index = 0x23000;  // replace record index with extended index
                }
            }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }
        }
	else if (m_InternalXMLData[ifNr].pd.ip_address_validation_local == 0xFFFF) //Address tailoring exists
	{
        if(m_InternalRemaData.base.ip_v4_suite.record_data_length > 0)
        {
            m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data = (LSA_UINT8*)xml_malloc(m_InternalRemaData.base.ip_v4_suite.record_data_length);
            xml_memcpy(m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data, m_InternalRemaData.base.ip_v4_suite.record_data, m_InternalRemaData.base.ip_v4_suite.record_data_length);
        }
		m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data_length = m_InternalRemaData.base.ip_v4_suite.record_data_length;
		m_InternalXMLData[ifNr].pd.ip_v4_suite.record_index = 0x23000;
	}
        else
        {
            return PNIO_ERR_CORRUPTED_DATA;
        }

        // NAMEOFSTATION_VALIDATION must be 0 (use NameOfStation) for PN-Driver
        if(m_InternalXMLData[ifNr].pd.name_of_station_validation == 0)
        {
            // NAME_OF_STATION
            element = xml_stdint_get_element_by_attribute(network_element, "Key", "4099");  // try 0x1003
            if (element == LSA_NULL)
            {
                element = xml_stdint_get_element_by_attribute(network_element, "Key", "12291"); // try 0x3003
            }
            if (element != LSA_NULL) 
            {
                result = getRecord(element, &m_InternalXMLData[ifNr].pd.name_of_station);
                if(result != PNIO_OK)
                {
                    return PNIO_ERR_CORRUPTED_DATA;
                }
                else
                {
                    m_InternalXMLData[ifNr].pd.name_of_station.record_index = 0x23003;  // replace record index with extended index
                }
	        }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }
        }
	else if (m_InternalXMLData[ifNr].pd.name_of_station_validation == 0xFFFF) //Address tailoring exists
	{
        if(m_InternalRemaData.base.name_of_station.record_data_length > 0)
        {
            m_InternalXMLData[ifNr].pd.name_of_station.record_data = (LSA_UINT8*)xml_malloc( m_InternalRemaData.base.name_of_station.record_data_length );
            xml_memcpy(m_InternalXMLData[ifNr].pd.name_of_station.record_data, m_InternalRemaData.base.name_of_station.record_data, m_InternalRemaData.base.name_of_station.record_data_length);
        }
		m_InternalXMLData[ifNr].pd.name_of_station.record_data_length = m_InternalRemaData.base.name_of_station.record_data_length;
		m_InternalXMLData[ifNr].pd.name_of_station.record_index = 0x23003;
	}
	else
	{
            return PNIO_ERR_CORRUPTED_DATA;
        }
    }

    return result;
}




/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getRemaNetworkParams(PND_XML_REMA_DATA_TYPE* remaData)
{
	CValueHelper val;

	LSA_UINT16      result;
    xml_element     element, network_element, iointerface_element;

    result              = PNIO_ERR_INVALID_REMA;
    element             = LSA_NULL;
    network_element     = LSA_NULL;
    iointerface_element = LSA_NULL;

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        iointerface_element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (iointerface_element != LSA_NULL)
        {
            element = xml_locate_element_by_child_content(iointerface_element, "AID", "11", 1); // DataRecordsConf
            if (element != LSA_NULL)
            {
                network_element = xml_stdint_get_element_by_name(element, "Value");
				if (network_element != LSA_NULL) 
				{
					// IPV4_SUITE
					element = xml_stdint_get_element_by_attribute(network_element, "Key", "23000");
					if (element != LSA_NULL)
					{
						result = getRecord(element, &m_InternalRemaData.base.ip_v4_suite);				
						if (result == PNIO_OK)
						{
						remaData->ipSuiteRecord.recordData = val.HexArrToCharArr(m_InternalRemaData.base.ip_v4_suite.record_data,				m_InternalRemaData.base.ip_v4_suite.record_data_length);
						}
						else
						{
							return PNIO_ERR_INVALID_REMA;
						}
					}
					// NAME_OF_STATION
					element = xml_stdint_get_element_by_attribute(network_element, "Key", "23003");
					if (element != LSA_NULL) 
					{
						result = getRecord(element, &m_InternalRemaData.base.name_of_station);
						if (result == PNIO_OK)
						{
						remaData->nosRecord.recordData = val.HexArrToCharArr(m_InternalRemaData.base.name_of_station.record_data, m_InternalRemaData.base.name_of_station.record_data_length);
						}
						else
						{
							return PNIO_ERR_INVALID_REMA;
						}
					}
					// MIB2 DATA
					element = xml_stdint_get_element_by_attribute(network_element, "Key", "F8000002");
					if (element != LSA_NULL) 
					{
						result = getRecord(element, &m_InternalRemaData.mib2);
						if (result == PNIO_OK)
						{
						remaData->mib2Record.recordData = val.HexArrToCharArr(m_InternalRemaData.mib2.record_data, m_InternalRemaData.mib2.record_data_length);
                        remaData->mib2Record.recordLen  = 2*m_InternalRemaData.mib2.record_data_length;
						}
						else
						{
							return PNIO_ERR_INVALID_REMA;
						}
					}
				}
				else
				{
					return PNIO_OK; 
				}                        
			}
		}
    }
    return result; 
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getPDParams(LSA_UINT16 ifNr)
{
    LSA_UINT16 result;
    xml_element element, records_conf_element;

    result                = PNIO_ERR_CORRUPTED_DATA;
    element               = LSA_NULL;
    records_conf_element  = LSA_NULL;

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (element != LSA_NULL)
        {
            element = xml_locate_element_by_child_content(element, "AID", "11", 1); // DataRecordsConf
            if (element != LSA_NULL)
            {
                records_conf_element = xml_stdint_get_element_by_name(element, "Value");
                if (records_conf_element != LSA_NULL) 
                {
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_validation = 0; // default value - PDEV central (parameters in project)

                    // SEND_CLOCK
                    element = xml_stdint_get_element_by_attribute(records_conf_element, "Key", "65536");
                    if (element != LSA_NULL) 
                    {
                        result = getRecord(element, &m_InternalXMLData[ifNr].pd.send_clock);
                        if(result != PNIO_OK)
                        {
                            return PNIO_ERR_CORRUPTED_DATA;
                        }
                    }
                    else
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }

                    // PNIdentification
                    element = xml_stdint_get_element_by_attribute(records_conf_element, "Key", "143616");
                    if (element != LSA_NULL) 
                    {
                        PND_XML_RECORD_TYPE record;
                        result = getRecord(element, &record);
                        if(result == PNIO_OK)
                        {
                            // Vendor ID
                            m_InternalXMLData[ifNr].ioc.ioc_hd.vendor_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);
                            m_InternalXMLData[ifNr].pd.pd_hd.vendor_id = m_InternalXMLData[ifNr].ioc.ioc_hd.vendor_id;

                            // Device ID
                            m_InternalXMLData[ifNr].ioc.ioc_hd.device_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);
                            m_InternalXMLData[ifNr].pd.pd_hd.device_id = m_InternalXMLData[ifNr].ioc.ioc_hd.device_id;

                            // Instance ID
                            m_InternalXMLData[ifNr].ioc.ioc_hd.instance = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+4);
                            m_InternalXMLData[ifNr].pd.pd_hd.instance = m_InternalXMLData[ifNr].ioc.ioc_hd.instance;

                            xml_free(record.record_data);
                        }    
                    }
                    else
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }
                }
            }
        }
    }

    m_InternalXMLData[ifNr].pd.pd_hd.map_count = getPortCount() + 1;

    return result;
}

LSA_BOOL CPnioXMLStore::isDataNull(PND_XML_RECORD_TYPE record)
{
	return (record.record_data == 0x00000000);
}
/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getPDDataRecords(LSA_UINT16 ifNr)
{
    LSA_UINT32          string_length, key_value;
    LSA_UINT16          slot_nr, subslot_nr, result;
    xml_element         element, element1, records_conf_element;
    LSA_CHAR            element_content[11];
    LSA_UINT32          content_length;
    LSA_UINT8           status;
    LSA_CHAR            key_string[11];
    PND_PARAM_PRM_RECORD_PTR_TYPE pEntry;

    slot_nr             = 0;
    subslot_nr          = 0;
    result              = PNIO_ERR_CORRUPTED_DATA;
    content_length      = 0;
    string_length       = 0;
    key_value           = 0;
    element             = LSA_NULL;
    element1            = LSA_NULL;
    records_conf_element= LSA_NULL;
    status              = LSA_FALSE;
    pEntry              = LSA_NULL;

    memset (element_content, 0, 11);
    memset (key_string, 0, 11);

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        slot_nr = 1; // set slot number to default value 1 - PND can only have one slot

        // find the right Interface
        element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (element != LSA_NULL)
        {
            // get subslot nr
            element1  = xml_stdint_get_element_by_attribute(element, "AID", "2");
            content_length = xml_stdint_get_element_content(element1, element_content);
            XML_ASSERT(content_length<=10); // uint32 max 10 digits
            subslot_nr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
            if(status != LSA_TRUE)
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }
            // get data records for this interface
            element1 = xml_locate_element_by_child_content(element, "AID", "11", 1); // DataRecordsConf
            records_conf_element = xml_stdint_get_element_by_name(element1, "Value");
            if (records_conf_element != LSA_NULL) 
            {
                // IPV4Suite
                pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
                XML_ASSERT( pEntry != 0 );
                // get slot number
                pEntry->slot_nr = slot_nr;
                // get subslot nr
                pEntry->subslot_nr = subslot_nr;
                // get record	
                if (m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data_length > 0)	
                {
                    pEntry->record_index       = m_InternalXMLData[ifNr].pd.ip_v4_suite.record_index;
                    pEntry->record_data_length = m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data_length;
                    pEntry->record_data = (LSA_UINT8*)xml_malloc( pEntry->record_data_length );
                    XML_ASSERT( pEntry->record_data != 0 );
                    xml_memcpy(pEntry->record_data, m_InternalXMLData[ifNr].pd.ip_v4_suite.record_data, pEntry->record_data_length);
                    m_InternalXMLData[ifNr].pd.prm_record_list.push_back (pEntry);
                }
                else
                {
                    xml_free(pEntry);
                }
                // NameOfStation
                pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
                XML_ASSERT( pEntry != 0 );
                // get slot number
                pEntry->slot_nr = slot_nr;
                // get subslot nr
                pEntry->subslot_nr = subslot_nr;
                // get record
                if (m_InternalXMLData[ifNr].pd.name_of_station.record_data_length > 0)
                {
                    pEntry->record_index       = m_InternalXMLData[ifNr].pd.name_of_station.record_index;
                    pEntry->record_data_length = m_InternalXMLData[ifNr].pd.name_of_station.record_data_length;
                    pEntry->record_data = (LSA_UINT8*)xml_malloc( pEntry->record_data_length );
                    XML_ASSERT( pEntry->record_data != 0 );
                    xml_memcpy(pEntry->record_data, m_InternalXMLData[ifNr].pd.name_of_station.record_data, pEntry->record_data_length);
                    m_InternalXMLData[ifNr].pd.prm_record_list.push_back (pEntry);
                }
                else
                {
                    xml_free(pEntry);
                }
                // SendClock
                pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
                XML_ASSERT( pEntry != 0 );
                // get slot number
                pEntry->slot_nr = slot_nr;
                // get subslot nr
                pEntry->subslot_nr = subslot_nr;
                // get record

                pEntry->record_index       = m_InternalXMLData[ifNr].pd.send_clock.record_index;
                pEntry->record_data_length = m_InternalXMLData[ifNr].pd.send_clock.record_data_length;
                pEntry->record_data = (LSA_UINT8*)xml_malloc( pEntry->record_data_length );
                XML_ASSERT( pEntry->record_data != 0 );
                xml_memcpy(pEntry->record_data, m_InternalXMLData[ifNr].pd.send_clock.record_data, pEntry->record_data_length);
                m_InternalXMLData[ifNr].pd.prm_record_list.push_back (pEntry);
                // read all records from interface
                element1 = xml_stdint_get_next_element(records_conf_element, LSA_NULL);
                while (element1 != LSA_NULL)
                {
                    // check block type
                    string_length = xml_stdint_get_attribute_value(element1, "Key", key_string, sizeof(key_string));
                    key_value = xml_string_to_u32(key_string, &status, LSA_FALSE);
                    if(status != LSA_TRUE)
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }                                        
                    if((key_value != 0x10000)                  // if not SEND_CLOCK
                        &&(key_value != 0x1101)                // if not COMM_FUNCT_INTERFACE
                        &&(key_value != 0x3015)                // if not PN_IDENTIFICATION
                        &&(!((0x1000 <= key_value) && (key_value <= 0x1007)))   // if not other network parameters
                        &&(!((0x3000 <= key_value) && (key_value <= 0x3009)))   // if not other network parameters
                        &&(!((0x3010 <= key_value) && (key_value <= 0x3014))))  // if not other network parameters
                    {
                        // read the record
                        PND_XML_RECORD_TYPE record;
                        result = getRecord(element1, &record);
                        if(result == PNIO_OK)
                        {
                            pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
                            XML_ASSERT( pEntry != 0 );
                            // get slot number
                            pEntry->slot_nr = slot_nr;
                            // get subslot nr
                            pEntry->subslot_nr = subslot_nr;
                            // get record
                            pEntry->record_index       = record.record_index;
                            pEntry->record_data_length = record.record_data_length;
                            pEntry->record_data        = record.record_data;
                            m_InternalXMLData[ifNr].pd.prm_record_list.push_back (pEntry);
                        }
                    }
                    element1 = xml_stdint_get_next_element(LSA_NULL, element1);
                }
            }
            // get records from ports
            element = xml_stdint_get_next_element(LSA_NULL, element);
            while (element != LSA_NULL)
            {
                // if this is a port
                element1  = xml_locate_element_by_content(element, "ClassRID", "4");
                if(element1 != LSA_NULL)
                {

                    // get subslot nr
                    element1  = xml_stdint_get_element_by_attribute(element, "AID", "2");
                    content_length = xml_stdint_get_element_content(element1, element_content);
                    XML_ASSERT(content_length<=10); // uint32 max 10 digits
                    subslot_nr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                    if(status != LSA_TRUE)
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }
                    // get data records for this port
                    element1 = xml_locate_element_by_child_content(element, "AID", "11", 1); // DataRecordsConf
                    records_conf_element = xml_stdint_get_element_by_name(element1, "Value");
                    if (records_conf_element != LSA_NULL) 
                    {
                        // read all records from port
                        element1 = xml_stdint_get_next_element(records_conf_element, LSA_NULL);
                        while (element1 != LSA_NULL)
                        {
                            // read the record
                            PND_XML_RECORD_TYPE record;
                            result = getRecord(element1, &record);
                            if(result == PNIO_OK)
                            {
                                pEntry = (PND_PARAM_PRM_RECORD_PTR_TYPE)xml_malloc( sizeof(PND_PARAM_PRM_RECORD_TYPE));
                                XML_ASSERT( pEntry != 0 );
                                // get slot number
                                pEntry->slot_nr = slot_nr;
                                // get subslot nr
                                pEntry->subslot_nr = subslot_nr;
                                // get record
                                pEntry->record_index       = record.record_index;
                                pEntry->record_data_length = record.record_data_length;
                                pEntry->record_data        = record.record_data;
                                m_InternalXMLData[ifNr].pd.prm_record_list.push_back (pEntry);
                            }
                            element1 = xml_stdint_get_next_element(LSA_NULL, element1);
                        }
                    }
                }
                // if this is an interface, stop looking for ports
                element1  = xml_locate_element_by_content(element, "ClassRID", "3");
                if(element1 != LSA_NULL)
                {
                    break;
                }
                //get next element
                element = xml_stdint_get_next_element(LSA_NULL, element);
            }

            result = PNIO_OK;
        }
    }

    return  result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getPDSubslotInfo(LSA_UINT16 ifNr)
{
    LSA_UINT16          port_nr, slot_nr, result;
    xml_element         element, element1;
    LSA_CHAR            element_content[11];
    LSA_UINT32          content_length;
    LSA_UINT8           status;

    port_nr             = 0;
    slot_nr             = 0;
    result              = PNIO_ERR_CORRUPTED_DATA;
    content_length      = 0;
    element             = LSA_NULL;
    element1            = LSA_NULL;
    status              = LSA_FALSE;

    memset (element_content, 0, 11);

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        // get slot nr

        //element1  = xml_stdint_get_element_by_attribute(element, "AID", "1");
        //content_length = xml_stdint_get_element_content(element1, element_content);
        //XML_ASSERT(content_length<=10); // uint32 max 10 digits
        //slot_nr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);

		slot_nr = 1; // set slot number to default value 1 - PND can only have one slot

        // find the right Interface
        element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (element != LSA_NULL)
        {        
            // get interface info

            // get slot nr
            m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].slot_nr = slot_nr;
            // get subslot nr
            element1  = xml_stdint_get_element_by_attribute(element, "AID", "2");
            content_length = xml_stdint_get_element_content(element1, element_content);
            XML_ASSERT(content_length<=10); // uint32 max 10 digits
            m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].subslot_nr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
            // get module ident
            m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].mod_ident = 0;
            // get submodule ident
            m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].sub_ident = 0;
            // get LADDR
            memset (element_content, 0, 10);
            element1 = xml_locate_element_by_child_content(element, "AID", "10", 1); // LADDR
            element1 = xml_stdint_get_element_by_name(element1, "Value");
            content_length = xml_stdint_get_element_content(element1, element_content);
            XML_ASSERT(content_length<=10); // uint32 max 10 digits
            m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].laddr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);

            //get next element
            element = xml_stdint_get_next_element(LSA_NULL, element);
            while (element != LSA_NULL)
            {
                // if this is a port
                element1  = xml_locate_element_by_content(element, "ClassRID", "4");
                if(element1 != LSA_NULL)
                {
                    // get port info
                    port_nr++;

                    // get slot nr
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].slot_nr = slot_nr;
                    // get subslot nr
                    memset (element_content, 0, 10);
                    element1  = xml_stdint_get_element_by_attribute(element, "AID", "2");
                    content_length = xml_stdint_get_element_content(element1, element_content);
                    XML_ASSERT(content_length<=10); // uint32 max 10 digits
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].subslot_nr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                    // get module ident
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].mod_ident = 0;
                    // get submodule ident
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].sub_ident = 0;
                    // get LADDR
                    memset (element_content, 0, 10);
                    element1 = xml_locate_element_by_child_content(element, "AID", "10", 1); // LADDR
                    element1 = xml_stdint_get_element_by_name(element1, "Value");
                    content_length = xml_stdint_get_element_content(element1, element_content);
                    XML_ASSERT(content_length<=10); // uint32 max 10 digits
                    m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[port_nr].laddr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                }
                // if this is an interface
                element1  = xml_locate_element_by_content(element, "ClassRID", "3");
                if(element1 != LSA_NULL)
                {
                    break;
                }
                //get next element
                element = xml_stdint_get_next_element(LSA_NULL, element);
            }

            result = PNIO_OK;
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getIOCParams(LSA_UINT16 ifNr)
{
    LSA_UINT16    result;  
    xml_element   element;

    result      = PNIO_ERR_CORRUPTED_DATA;
    element     = LSA_NULL;


    element = getDecentralIOSystem();

    if (element != LSA_NULL) 
    {  

        m_InternalXMLData[ifNr].ioc.ioc_hd.device_count           = getDeviceCount();

        // find CONTROLLER_PROPERTIES
        element = xml_locate_element_by_child_content(element, "AID", "15", 1); // IOsysParamConfig
        if (element != LSA_NULL) 
        {  
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // CONTROLLER_PROPERTIES
                element = xml_stdint_get_element_by_attribute(element, "Key", "12352");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // Vendor ID
                        //m_InternalXMLData[ifNr].ioc.ioc_hd.vendor_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);

                        // Device ID
                        //m_InternalXMLData[ifNr].ioc.ioc_hd.device_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);

                        // Instance ID
                        //m_InternalXMLData[ifNr].ioc.ioc_hd.instance = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+4);

                        // CMIActivityTimeout
                        m_InternalXMLData[ifNr].ioc.cmi_timeout100ms = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+6);

                        // RPC Remote Application Timeout
                        m_InternalXMLData[ifNr].ioc.appl_timeout1sec = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+8);

                        xml_free(record.record_data);
                    }

                }
            }
        }
    }


    // Default values (not from XML file)
    m_InternalXMLData[ifNr].ioc.ioc_hd.sr_ar_nr               = 0;
    m_InternalXMLData[ifNr].ioc.ioc_hd.max_alarm_data_length  = 200;

    // this value will be set later, when received from the PNIO stack
    m_InternalXMLData[ifNr].ioc.ioc_hd.max_devices            = 0;

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getOHAParams(LSA_UINT16 ifNr)
{
    LSA_UINT16    result, if_nr;  
    xml_element   element;

    result      = PNIO_ERR_CORRUPTED_DATA;
    if_nr       = 0;
    element     = LSA_NULL;


    element = getDecentralIOSystem();

    if (element != LSA_NULL) 
    {  
        // find CONTROLLER_PROPERTIES
        element = xml_locate_element_by_child_content(element, "AID", "15", 1); // IOsysParamConfig
        if (element != LSA_NULL) 
        {  
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // CONTROLLER_PROPERTIES
                element = xml_stdint_get_element_by_attribute(element, "Key", "12352");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // Vendor ID
                        m_InternalXMLData[ifNr].oha.oha_hd.vendor_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);

                        // Device ID
                        m_InternalXMLData[ifNr].oha.oha_hd.device_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }


    // According to PROFINET Standard: SubslotNumber = 0xL000 + 0x0I00 + 0x00PP, where I = interface number
    if_nr = (m_InternalXMLData[ifNr].pd.pd_hd.pdev_sub[0].subslot_nr & 0x7FFF) >> 8;
    m_InternalXMLData[ifNr].oha.oha_hd.edd_if_id        = (LSA_UINT8)(if_nr + 1);

    // Default values (not from XML file)
    m_InternalXMLData[ifNr].oha.oha_hd.check_ip_address = 1; // IO Controller

    // this value will be overriden later, when received from the PNIO stack
    m_InternalXMLData[ifNr].oha.oha_hd.nr_of_usr_ports  = m_InternalXMLData[ifNr].pd.pd_hd.map_count - 1;

	// IPAddressValidation and NameOfStationValidation data 
	m_InternalXMLData[ifNr].oha.oha_hd.IPAddrValAllowed = (LSA_BOOL) (m_InternalXMLData[ifNr].pd.ip_address_validation_local == 0xFFFF);
	m_InternalXMLData[ifNr].oha.oha_hd.NameOfStationValAllowed = (LSA_BOOL) (m_InternalXMLData[ifNr].pd.name_of_station_validation == 0xFFFF);

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getDeviceNetworkParams(LSA_UINT16 devNr, LSA_UINT32* ip_address, LSA_UINT8** name_of_station, 
                                                LSA_UINT16* name_of_station_length, LSA_UINT16* ip_address_validation_remote, 
                                                LSA_UINT16* ip_address_validation_prop, StationNameAliasVector* station_name_alias_list)
{
    LSA_UINT16        result, name_length;  
    xml_element       element, network_element;
    LSA_UINT32        content_length;
    LSA_CHAR*         element_str;
    LSA_UINT8*        record_data;
    LSA_UINT8*        station_name;
    LSA_UINT8         status;

    result            = PNIO_ERR_CORRUPTED_DATA;
    element           = LSA_NULL;
    network_element   = LSA_NULL;
    element_str       = LSA_NULL;
    content_length    = 0;
    record_data       = LSA_NULL;
    station_name      = LSA_NULL;
    name_length       = 0;
    status            = 0;

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {
        // NetworkParameters
        element = xml_locate_element_by_child_content(element, "ClassRID", "11", 1);
        if (element != LSA_NULL) 
        {      
            // NetworkParamConfig
            element = xml_locate_element_by_child_content(element, "AID", "12", 1);
            if(element != LSA_NULL)
            {
                network_element = xml_stdint_get_element_by_name(element, "Value");
                if (network_element != LSA_NULL) 
                {
                    // IPV4_SUITE
                    element = xml_stdint_get_element_by_attribute(network_element, "Key", "4096");
                    if (element != LSA_NULL) 
                    {
                        PND_XML_RECORD_TYPE record;
                        result = getRecord(element, &record);
                        if(result == PNIO_OK)
                        {
                            *ip_address = getUINT32FromRecord(record.record_data, XML_BLOCK_HEADER);   // IPAddress
                            xml_free(record.record_data);
                        }
                        else
                        {
                            return PNIO_ERR_CORRUPTED_DATA;
                        }
                    }

                    // IP_ADDRESS_VALIDATION_REMOTE
                    element = xml_stdint_get_element_by_attribute(network_element, "Key", "4103");
                    if (element != LSA_NULL) 
                    {
                        PND_XML_RECORD_TYPE record;
                        result = getRecord(element, &record);
                        if(result == PNIO_OK)
                        {
                            *ip_address_validation_remote = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);
                            *ip_address_validation_prop = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);
                            xml_free(record.record_data);
                        }
                        else
                        {
                            return PNIO_ERR_CORRUPTED_DATA;
                        }
                    }

                    // STATION_NAME_ALIAS
                    element = xml_stdint_get_element_by_attribute(network_element, "Key", "4102");
                    if (element != LSA_NULL) 
                    {
                        PND_XML_RECORD_TYPE record;
                        LSA_UINT16 block_length, pointer, count;

                        result = getRecord(element, &record);
                        if(result == PNIO_OK)
                        {
                            pointer = XML_BLOCK_HEADER;
                            count = getUINT16FromRecord(record.record_data, pointer);
                            pointer = pointer + 2;
                            for (LSA_UINT16 i = 0; i < count; i++)
                            {
                                PND_XML_STATION_NAME_ALIAS_PTR_TYPE station_name_alias = new pnd_xml_station_name_alias_tag;
                                block_length                    = getUINT16FromRecord(record.record_data, pointer);
                                name_length                     = getUINT8FromRecord(record.record_data, pointer+2);

				                if( name_length != 0 )
				                {
					                station_name = (LSA_UINT8*)xml_malloc(name_length);
					                xml_memcpy(station_name, record.record_data+pointer+3, name_length);
				                }
				                else
				                {
					                station_name = LSA_NULL;
				                }
                                

                                station_name_alias->name_length = (LSA_UINT8)name_length;
                                station_name_alias->alias_name  = station_name;
                                pointer                         = pointer + block_length;
                                station_name_alias_list->push_back(station_name_alias);
                            }
                            xml_free(record.record_data);
                        }
                        else
                        {
                            return PNIO_ERR_CORRUPTED_DATA;
                        }
                    }

                    // NAME_OF_STATION
                    element                   = xml_stdint_get_element_by_attribute(network_element, "Key", "4099");
                    if (element != LSA_NULL) 
                    {
                        PND_XML_RECORD_TYPE record;
                        result = getRecord(element, &record);
                        if(result == PNIO_OK)
                        {
                            name_length = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);   // NameLength
                            station_name              = (LSA_UINT8*)xml_malloc(name_length);
                            xml_memcpy(station_name, record.record_data+12, name_length);
                            *name_of_station_length   = name_length;
                            *name_of_station          = station_name;
                            xml_free(record.record_data);
                        }
                        else
                        {
                            return PNIO_ERR_CORRUPTED_DATA;
                        }
                    }
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getDeactivatedConfig(LSA_UINT16 devNr, LSA_UINT8* is_active)
{
    LSA_UINT16      result;  
    xml_element     element;
    LSA_CHAR        element_content[6];
    LSA_UINT32      content_length;

    result          = PNIO_ERR_CORRUPTED_DATA;
    element         = LSA_NULL;
    content_length  = 0;

    memset (element_content, 0, 6);

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {
        // DeactivatedConfig
        element = xml_locate_element_by_child_content(element, "AID", "4", 1);
        if (element != LSA_NULL) 
        {      
            element = xml_stdint_get_element_by_name(element, "Value");
            content_length = xml_stdint_get_element_content(element, element_content);
            XML_ASSERT(content_length<=5); // "false" or "true"

            if(xml_strncmp(element_content, "false", 5) == 0)
            {
                *is_active = 1;
            }
            else
            {
                *is_active = 0;
            }
            result = PNIO_OK;
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getIODProperties(LSA_UINT16 devNr, PND_XML_PNIOD_PROPERTIES_PTR_TYPE pniod_properties)
{
    LSA_UINT16      result;  
    LSA_UINT32      value32;
    xml_element     element, device_element;
    LSA_CHAR        element_content[11];
    LSA_UINT32      content_length;
    LSA_UINT8       status;

    result          = PNIO_ERR_CORRUPTED_DATA;
    value32         = 0;
    element         = LSA_NULL;
    device_element  = LSA_NULL;
    content_length  = 0;
    status          = LSA_FALSE;

    memset (element_content, 0, 11);

    device_element = getDevice(devNr);

    if(device_element != LSA_NULL)
    {
		// Device Number (Station Number)
		element = xml_stdint_get_element_by_name(device_element, "Key");
		if(element != LSA_NULL)
		{
			content_length = xml_stdint_get_element_content(element, element_content);
			XML_ASSERT(content_length<=10); // uint32 max 10 digits
			pniod_properties->device_number = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);

			memset (element_content, 0, 11); // clearing buffer 
		}

        // LADDR
        element = xml_locate_element_by_child_content(device_element, "AID", "10", 1); // LADDR
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            content_length = xml_stdint_get_element_content(element, element_content);
            XML_ASSERT(content_length<=5); // uint16 max 10 digits
            pniod_properties->device_laddr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
        }

        // find PNIOD_PROPERTIES
        element = xml_locate_element_by_child_content(device_element, "AID", "13", 1); // IODevParamConfig
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // PNIOD_PROPERTIES
                element = xml_stdint_get_element_by_attribute(element, "Key", "12384");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // Vendor ID
                        pniod_properties->vendor_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);

                        // Device ID
                        pniod_properties->device_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);

                        // Instance ID
                        pniod_properties->instance_id = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+4);

                        // MaxRecordSize
                        pniod_properties->max_record_size = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+6);

                        // MultipleWriteSupported
                        value32 = getUINT32FromRecord(record.record_data, XML_BLOCK_HEADER+8);
                        pniod_properties->write_multiple_is_supported = (LSA_UINT8)(value32 & 0x01);

                        // DeviceMode
                        pniod_properties->planned_mode =  getUINT32FromRecord(record.record_data, XML_BLOCK_HEADER+12);

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getAlarmCRData(LSA_UINT16 devNr, PND_XML_ALARM_CR_DATA_PTR_TYPE alarm_cr_data)
{
    LSA_UINT16      result;  
    xml_element     element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    element         = LSA_NULL;

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {

        // find ALARMCR_DATA
        element = xml_locate_element_by_child_content(element, "AID", "13", 1);
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // ALARMCR_DATA
                element = xml_stdint_get_element_by_attribute(element, "Key", "12551");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // AlarmCRProperties
                        alarm_cr_data->alcr_properties = getUINT32FromRecord(record.record_data, XML_BLOCK_HEADER+8);

                        // RTATimeoutFactor
                        alarm_cr_data->timeout100ms = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+12);

                        // RTARetries
                        alarm_cr_data->retries = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+14);

                        // AlarmCRTagHeaderHigh
                        alarm_cr_data->alcr_tag_header_high = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+16);

                        // AlarmCRTagHeaderLow
                        alarm_cr_data->alcr_tag_header_low = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+18);

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getARCommunicationData(LSA_UINT16 devNr, PND_XML_AR_DATA_PTR_TYPE ar_comm_data)
{
    LSA_UINT16      result;  
    xml_element     element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    element         = LSA_NULL;

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {

        // find AR_COMMUNICATION_DATA
        element = xml_locate_element_by_child_content(element, "AID", "13", 1);
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // AR_COMMUNICATION_DATA
                element = xml_stdint_get_element_by_attribute(element, "Key", "12544");
                if (element != LSA_NULL) 
                {

                    // AR_Number
                    ar_comm_data->ar_nr = 1; // Default value

                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // ARType
                        ar_comm_data->ar_type = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+2);

                        // AR_UUID
                        getUUID(record.record_data, XML_BLOCK_HEADER+4, &ar_comm_data->ar_uuid);

                        // ARProperties
                        ar_comm_data->ar_properties = getUINT32FromRecord(record.record_data, XML_BLOCK_HEADER+20);

                        // check StartupMode
                        // if "Advanced" (1), get CMIActivityTimeout for IODs from this block
                        if ( ar_comm_data->ar_properties & (1 << 30))
                        {
                            // CMIActivityTimeout
                            ar_comm_data->cmi_timeout100ms = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER+24);
                        }

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getIOCRData(LSA_UINT16 devNr, IOCRVector* iocr_data)
{

    LSA_UINT16      result, pointer;
    xml_element     element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    pointer         = 0;
    element         = LSA_NULL;

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {

        // find IOCR_DATA
        element = xml_locate_element_by_child_content(element, "AID", "13", 1);
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // IOCR_DATA
                element = xml_stdint_get_element_by_attribute(element, "Key", "12546");
                if (element != LSA_NULL) 
                {

                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if (result == PNIO_OK)
                    {
                        // Number of CRs
                        LSA_UINT16 nr_of_cr = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);

                        pointer = XML_BLOCK_HEADER + 2;

                        for ( LSA_UINT16 i = 0; i < nr_of_cr; i++)
                        {
                            PND_XML_IOCR_DATA_PTR_TYPE pIOCR = new pnd_xml_iocr_data_tag;

                            // IOCRType
                            pIOCR->iocr_type = getUINT16FromRecord(record.record_data, pointer+4);

                            // IOCRReference
                            pIOCR->iocr_ref = getUINT16FromRecord(record.record_data, pointer+6);

                            // IOCRProperties
                            pIOCR->iocr_properties = getUINT32FromRecord(record.record_data, pointer+12);

                            // DataLength
                            pIOCR->data_length = getUINT16FromRecord(record.record_data, pointer+16);

                            // FrameID
                            pIOCR->frame_id = getUINT16FromRecord(record.record_data, pointer+18);

                            // SendClockFactor
                            pIOCR->send_clock = getUINT16FromRecord(record.record_data, pointer+20);

                            // ReductionRatio
                            pIOCR->reduction_ratio = getUINT16FromRecord(record.record_data, pointer+22);

                            // Phase
                            pIOCR->phase = getUINT16FromRecord(record.record_data, pointer+24);

                            // Sequence
                            pIOCR->sequence = getUINT16FromRecord(record.record_data, pointer+26);

                            // FrameSendOffset
                            pIOCR->frame_send_offset = getUINT32FromRecord(record.record_data, pointer+28);

                            // WatchdogFactor
                            pIOCR->watchdog_factor = getUINT16FromRecord(record.record_data, pointer+32);

                            // DataHoldFactor
                            pIOCR->data_hold_factor = getUINT16FromRecord(record.record_data, pointer+34);

                            // Number of APIs
                            LSA_UINT16 nr_of_api = getUINT16FromRecord(record.record_data, pointer+58);

                            pointer = pointer+60;

                            for ( LSA_UINT16 j = 0; j < nr_of_api; j++)
                            {
                                PND_XML_API_LIST_PTR_TYPE pAPI = new pnd_xml_api_list_tag;

                                //API
                                pAPI->api_nr = getUINT32FromRecord(record.record_data, pointer);

                                pointer = pointer + 4;

                                // NumberOfRelatedIODataObjects
                                LSA_UINT16 nr_of_iodata = getUINT16FromRecord(record.record_data, pointer);

                                pointer = pointer + 2;

                                for (LSA_UINT16 k = 0; k < nr_of_iodata; k++)
                                {
                                    PND_XML_IOCR_IO_LIST_PTR_TYPE pIODataObject;
                                    pIODataObject = (PND_XML_IOCR_IO_LIST_PTR_TYPE)xml_malloc( sizeof(PND_XML_IOCR_IO_LIST_TYPE));
                                    XML_ASSERT( pIODataObject != 0 );
                                    // SlotNumber
                                    pIODataObject->slot_nr = getUINT16FromRecord(record.record_data, pointer);
                                    // Subslotnumber
                                    pIODataObject->subslot_nr = getUINT16FromRecord(record.record_data, pointer+2);
                                    // FrameOffset
                                    pIODataObject->frame_offset = getUINT16FromRecord(record.record_data, pointer+4);

                                    pointer = pointer + 8;

                                    pAPI->iodata_list.push_back(pIODataObject);

                                }

                                pointer = pointer + 2;

                                // NumberOfRelatedIOCS
                                LSA_UINT16 nr_of_iocs = getUINT16FromRecord(record.record_data, pointer);
                                pointer = pointer + 2;

                                for (LSA_UINT16 l = 0; l < nr_of_iocs; l++)
                                {
                                    PND_XML_IOCR_IO_LIST_PTR_TYPE pIOCS;
                                    pIOCS = (PND_XML_IOCR_IO_LIST_PTR_TYPE)xml_malloc( sizeof(PND_XML_IOCR_IO_LIST_TYPE));
                                    XML_ASSERT( pIOCS != 0 );
                                    // SlotNumber
                                    pIOCS->slot_nr = getUINT16FromRecord(record.record_data, pointer);
                                    // Subslotnumber
                                    pIOCS->subslot_nr = getUINT16FromRecord(record.record_data, pointer+2);
                                    // FrameOffset
                                    pIOCS->frame_offset = getUINT16FromRecord(record.record_data, pointer+4);

                                    pointer = pointer + 8;

                                    pAPI->iocs_list.push_back(pIOCS);
                                }

                                pIOCR->api_list.push_back( pAPI );

                                pointer = pointer + 2;

                            }
 
                            iocr_data->push_back( pIOCR );
                        }

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }

    return result;

}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getExpectedSubmoduleData(LSA_UINT16 devNr, ArcbAPIVector* exp_submod_data)
{
    LSA_UINT16      result, pointer; 
    xml_element     element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    pointer         = 0;
    element         = LSA_NULL;

    element = getDevice(devNr);

    if(element != LSA_NULL)
    {

        // find EXPECTED_SUBMODULE_DATA
        element = xml_locate_element_by_child_content(element, "AID", "13", 1);
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // EXPECTED_SUBMODULE_DATA
                element = xml_stdint_get_element_by_attribute(element, "Key", "12545");
                if (element != LSA_NULL) 
                {

                    PND_XML_RECORD_TYPE record;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        // Number of APIs
                        LSA_UINT16 nr_of_api = getUINT16FromRecord(record.record_data, XML_BLOCK_HEADER);

                        pointer = XML_BLOCK_HEADER + 2;

                        for(LSA_UINT16 i = 0; i < nr_of_api; i++)
                        {
                            PND_XML_ARCB_API_PTR_TYPE pArcbAPI = new pnd_xml_arcb_api_tag;

                            //API
                            pArcbAPI->api_nr = getUINT32FromRecord(record.record_data, pointer);

                            // Number of slot blocks
                            LSA_UINT16 nr_of_slots = getUINT16FromRecord(record.record_data, pointer+6);

                            pointer = pointer+8;

                            for(LSA_UINT16 j = 0; j < nr_of_slots; j++)
                            {
                                PND_XML_ARCB_SLOT_PTR_TYPE pSlot = new pnd_xml_arcb_slot_tag;
                
                                // SlotNumber
                                pSlot->slot_nr = getUINT16FromRecord(record.record_data, pointer+4);
                                // ModuleIdentNumber
                                pSlot->module_ident = getUINT32FromRecord(record.record_data, pointer+8);
                                // ModuleProperties
                                pSlot->module_properties = getUINT16FromRecord(record.record_data, pointer+12);
                                // Number of Submodule Descriptions
                                LSA_UINT16 nr_of_subslots = getUINT16FromRecord(record.record_data, pointer+18);

                                pointer = pointer+20;

                                for(LSA_UINT16 k = 0; k < nr_of_subslots; k++)
                                {
                                    PND_XML_ARCB_SUBMODULE_PTR_TYPE pSubSlot = new pnd_xml_arcb_submodule_tag;
                                    LSA_UINT8 submodule_type;
                                    submodule_type = 0;

                                    // SubslotNumber
                                    pSubSlot->subslot_nr = getUINT16FromRecord(record.record_data, pointer);
                                    // SubmoduleIdentNumber
                                    pSubSlot->sub_ident = getUINT32FromRecord(record.record_data, pointer+4);
                                    // Submodule Properties
                                    pSubSlot->sub_properties = getUINT16FromRecord(record.record_data, pointer+8);

                                    pointer = pointer + 12;

                                    PND_XML_ARCB_SUBMODULE_DESCR_PTR_TYPE pSubModDescription1 = new pnd_xml_arcb_submodule_descr_tag;
                                    PND_XML_ARCB_SUBMODULE_DESCR_PTR_TYPE pSubModDescription2 = new pnd_xml_arcb_submodule_descr_tag;
                                    pSubModDescription1->data_description = getUINT16FromRecord(record.record_data, pointer);
                                    pSubModDescription1->data_length = getUINT16FromRecord(record.record_data, pointer+2);
                                    pSubModDescription1->iops_length = getUINT16FromRecord(record.record_data, pointer+4);
                                    pSubModDescription1->iocs_length = getUINT16FromRecord(record.record_data, pointer+6);

                                    pSubSlot->arcb_submodule_descr_list.push_back(pSubModDescription1);

                                    pointer = pointer + 12;

                                    submodule_type = pSubSlot->sub_properties & 0x03; // evaluate only the first two bits
                                    switch(submodule_type)
                                    {
                                        case 0:
                                        case 1:
                                        case 2:
                                            delete pSubModDescription2;
                                            break;

                                        case 3:
                                            pSubModDescription2->data_description = getUINT16FromRecord(record.record_data, pointer);
                                            pSubModDescription2->data_length = getUINT16FromRecord(record.record_data, pointer+2);
                                            pSubModDescription2->iops_length = getUINT16FromRecord(record.record_data, pointer+4);
                                            pSubModDescription2->iocs_length = getUINT16FromRecord(record.record_data, pointer+6);
                                            pSubSlot->arcb_submodule_descr_list.push_back(pSubModDescription2);
                                            pointer = pointer + 12;
                                            break;
                                        default:
                                            break;
                                    }
                                    pSlot->arcb_submodule_list.push_back(pSubSlot);
                                }
                                pArcbAPI->arcb_slot_list.push_back(pSlot);
                            }
                            exp_submod_data->push_back(pArcbAPI);
                        }

                        xml_free(record.record_data);
                    }
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getOtherSubmoduleData(LSA_UINT16 devNr, ArcbAPIVector* exp_submod_data)
{
    LSA_UINT16      result, slot_nr, slot_nr1, subslot_nr, subslot_nr1;
    LSA_UINT32      value32, content_length, string_length, dts_length;
    LSA_UINT8       status;
    xml_element     element, device_element, module_element, submodule_element, records_conf_element;
    LSA_CHAR        element_content[11];
    LSA_CHAR        length[6];

    result                = PNIO_ERR_CORRUPTED_DATA;
    slot_nr               = 0;
    slot_nr1              = 0;
    subslot_nr            = 0;
    subslot_nr1           = 0;
    value32               = 0;
    content_length        = 0;
    string_length         = 0;
    dts_length            = 0;
    status                = LSA_NULL;
    element               = LSA_NULL;
    device_element        = LSA_NULL;
    module_element        = LSA_NULL;
    submodule_element     = LSA_NULL;
    records_conf_element  = LSA_NULL;

    memset (element_content, 0, 11);
    memset (length, 0, 5);

    device_element = getDevice(devNr);

    LSA_UINT16 nr_of_api = (LSA_UINT16)exp_submod_data->size();

    for(LSA_UINT16 i = 0; i < nr_of_api; i++)    // APIs
    {

        LSA_UINT16 nr_of_slots = (LSA_UINT16)exp_submod_data->at(i)->arcb_slot_list.size();

        for(LSA_UINT16 j = 0; j < nr_of_slots; j++)  // Slots
        {

            slot_nr = exp_submod_data->at(i)->arcb_slot_list.at(j)->slot_nr;

            // locate module with this slot number
            LSA_UINT16 module_idx = 0;
            do
            {
                module_idx++;
                module_element = xml_locate_element_by_child_content(device_element, "ClassRID", "7", module_idx);
                if (module_element != LSA_NULL)
                {
                    // get slot nr
                    element  = xml_stdint_get_element_by_attribute(module_element, "AID", "1");
                    memset (element_content, 0, 11);
                    content_length = xml_stdint_get_element_content(element, element_content);
                    XML_ASSERT(content_length<=5); // uint16 max 5 digits
                    slot_nr1 = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                    if(slot_nr == slot_nr1)
                    {
                        break;
                    }
                }
            }
            while (module_element != LSA_NULL);
            
            if (module_element == LSA_NULL) // module not found
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }

            // get LADDR for this module
            // ModuleProxy
            element = xml_locate_element_by_child_content(module_element, "ClassRID", "8", 1);
            if(element == LSA_NULL)
            {
                // Submodule
                element = xml_locate_element_by_child_content(module_element, "ClassRID", "9", 1);
                if(element == LSA_NULL)
                {
                    // IOSubmodule
                    element = xml_locate_element_by_child_content(module_element, "ClassRID", "10", 1);
                    if(element == LSA_NULL)
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }        
                }
            }

            // LADDR
            element = xml_locate_element_by_child_content(element, "AID", "10", 1); // LADDR
            if (element != LSA_NULL) 
            {
                element = xml_stdint_get_element_by_name(element, "Value");
                memset (element_content, 0, 11);
                content_length = xml_stdint_get_element_content(element, element_content);
                XML_ASSERT(content_length<=5); // uint16 max 10 digits
                exp_submod_data->at(i)->arcb_slot_list.at(j)->laddr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
            }
            else
            {
                return PNIO_ERR_CORRUPTED_DATA;
            }

            LSA_UINT16 nr_of_subslots = (LSA_UINT16)exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.size();

            for(LSA_UINT16 k = 0; k < nr_of_subslots; k++)   // Subslots
            {

                subslot_nr = exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->subslot_nr;
                // locate submodule with this subslot number        
                LSA_UINT16 submodule_idx = 1;
                LSA_UINT16 iosubmodule_idx = 1;

                do
                {  
                    // try Submodule
                    submodule_element = xml_locate_element_by_child_content(module_element, "ClassRID", "9", submodule_idx);  // Submodule
                    if(submodule_element != LSA_NULL)
                    {
                        submodule_idx++;
                    }
                    else
                    {
                        // try IOSubmodule
                        submodule_element = xml_locate_element_by_child_content(module_element, "ClassRID", "10", iosubmodule_idx);  // IOSubmodule
                        if(submodule_element != LSA_NULL)
                        {
                            iosubmodule_idx++;
                        }
                    }

                    if (submodule_element != LSA_NULL)
                    {
                        // get subslot nr
                        element  = xml_stdint_get_element_by_attribute(submodule_element, "AID", "2");
                        memset (element_content, 0, 11);
                        content_length = xml_stdint_get_element_content(element, element_content);
                        XML_ASSERT(content_length<=5); // uint16 max 5 digits
                        subslot_nr1 = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                        if(subslot_nr == subslot_nr1)
                        {
                            break;
                        }
                    }
                }
                while (submodule_element != LSA_NULL);

                if (submodule_element == LSA_NULL) // submodule not found
                {
                    return PNIO_ERR_CORRUPTED_DATA;
                }

                // get LADDR for this submodule
                element = xml_locate_element_by_child_content(submodule_element, "AID", "10", 1); // LADDR
                if (element != LSA_NULL) 
                {      
                  element = xml_stdint_get_element_by_name(element, "Value");
                  memset (element_content, 0, 11);
                  content_length = xml_stdint_get_element_content(element, element_content);
                  XML_ASSERT(content_length<=5); // uint16 max 10 digits
                  exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->laddr = (LSA_UINT16)xml_string_to_u32(element_content, &status, LSA_FALSE);
                }
                else
                {
                    return PNIO_ERR_CORRUPTED_DATA;
                }

                // get IOmapping for this submodule 

                // if IOSubmodule (ClassRID=10)
                LSA_UINT16 nr_of_submodule_descr = (LSA_UINT16)exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.size();
                element  = xml_locate_element_by_content(submodule_element, "ClassRID", "10");
                if(element != LSA_NULL)
                {
                    // get IOmapping for this IOsubmodule
                    element = xml_locate_element_by_child_content(submodule_element, "AID", "5", 1); // IOmapping
                    if (element == LSA_NULL)
                    {
                        return PNIO_ERR_CORRUPTED_DATA;
                    }
                    element = xml_stdint_get_element_by_name(element, "Value");
                    // read all records from IOmapping block
                    element = xml_stdint_get_next_element(element, LSA_NULL);
                    while (element != LSA_NULL)
                    {
                        // check block type
                        memset (element_content, 0, 11);
                        string_length = xml_stdint_get_attribute_value(element, "AID", element_content, sizeof(element_content));
                        value32 = xml_string_to_u32(element_content, &status, LSA_FALSE);

                        // get the Ibase address
                        if (value32 == 6)  //Ibase (input base address)
                        {
                            for (LSA_UINT16 l = 0; l < nr_of_submodule_descr; l++)
                            {
                                memset (element_content, 0, 11);
                                content_length = xml_stdint_get_element_content(element, element_content);
                                XML_ASSERT(content_length<=10); // uint32 max 10 digits
                                exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(l)->i_base_addr = xml_string_to_u32(element_content, &status, LSA_FALSE);
                            }
                        }
                        // get the Qbase address
                        if (value32 == 8)  // Qbase (output base address)
                        {
                            for (LSA_UINT16 l = 0; l < nr_of_submodule_descr; l++)
                            {
                                memset (element_content, 0, 11);
                                content_length = xml_stdint_get_element_content(element, element_content);
                                XML_ASSERT(content_length<=10); // uint32 max 10 digits
                                exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(l)->q_base_addr = xml_string_to_u32(element_content, &status, LSA_FALSE);
                            }
                        }

                        element = xml_stdint_get_next_element(LSA_NULL, element);
                    }
                }
                // if Submodule
                else
                {
                    // set a default value PND_LADDR_INVALID (0xFFFF)
                    for (LSA_UINT16 l = 0; l < nr_of_submodule_descr; l++)
                    {
                        exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(l)->i_base_addr = PND_LADDR_INVALID;
                        exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_submodule_descr_list.at(l)->q_base_addr = PND_LADDR_INVALID;
                    }
                }


                // get data records for this submodule
                element = xml_locate_element_by_child_content(submodule_element, "AID", "11", 1); // DataRecordsConf
                if(element != LSA_NULL)
                {
                    records_conf_element = xml_stdint_get_element_by_name(element, "Value");
                    if (records_conf_element != LSA_NULL) 
                    {
                        // read all records from submodule
                        element = xml_stdint_get_next_element(records_conf_element, LSA_NULL);
                        while (element != LSA_NULL)
                        {
                            PND_XML_ARCB_RECORD_PTR_TYPE pArcbRecord = new pnd_xml_arcb_record_tag;
                            pArcbRecord->transfer_sequence = 0; // default value
                            result = getRecord(element, &pArcbRecord->record);
                            if (result == PNIO_OK)
                            {
                                exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_record_list.push_back(pArcbRecord);
                            }
                            else
                            {
                                delete pArcbRecord;
                            }

                            element = xml_stdint_get_next_element(LSA_NULL, element);
                        }
                    }
                }

                // get data transfer sequence for this submodule
                element = xml_locate_element_by_child_content(submodule_element, "AID", "14", 1); // DataRecordsTransferSequence
                if(element != LSA_NULL)
                {
                    element = xml_stdint_get_element_by_name(element, "Value");
                    // get the blob
                    LSA_CHAR* blob_str = (LSA_CHAR*)xml_stdint_get_element_content2(element, &content_length);
                    if(content_length > 0)
                    {
                        LSA_UINT8* blob = (LSA_UINT8*)xml_malloc(content_length);
                        status = xml_hex_string_to_byte_array(blob_str, blob, (content_length/2));
                        // read blob length
                        string_length = xml_stdint_get_attribute_value(element, "Length", length, sizeof(length));
                        dts_length = xml_string_to_u32(length, &status, LSA_FALSE);
                        dts_length = dts_length/4;
                        for (LSA_UINT16 l = 0; l < dts_length; l++)
                        {
                            // get record index
                            value32 = getUINT32FromRecord(blob, l*4);
                            // find this record in the record list
                            LSA_UINT16 nr_of_records = (LSA_UINT16)exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_record_list.size();
                            for (LSA_UINT16 m = 0; m < nr_of_records; m++)
                            {
                                if (value32 == exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_record_list.at(m)->record.record_index)
                                {
                                    exp_submod_data->at(i)->arcb_slot_list.at(j)->arcb_submodule_list.at(k)->arcb_record_list.at(m)->transfer_sequence = l+1; // overwrite the default value 0
                                }
                            }
                        }
                        xml_free(blob);
                    }
                    xml_free(blob_str);
                }
                else
                {
                    return PNIO_ERR_CORRUPTED_DATA;
                }

            } // Subslots
        } // Slots
    } // APIs

    return PNIO_OK;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getLocalSCFAdaption(LSA_UINT16 devNr, LocalSCFAdaptionVector* local_scf_adaption)
{
    LSA_UINT16      result;  
    xml_element     element, device_element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    element         = LSA_NULL;
    device_element  = LSA_NULL;

    device_element = getDevice(devNr);

    if(device_element != LSA_NULL)
    {
        // find LOCAL_SCF_ADAPTION
        element = xml_locate_element_by_child_content(device_element, "AID", "13", 1); // IODevParamConfig
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // LOCAL_SCF_ADAPTION
                element = xml_stdint_get_element_by_attribute(element, "Key", "12548");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    LSA_UINT16 count, pointer, block_length;
                    result = getRecord(element, &record);
                    if(result == PNIO_OK)
                    {
                        pointer = XML_BLOCK_HEADER;
                        count  = getUINT16FromRecord(record.record_data, pointer);
                        pointer = pointer + 2;
                        for (LSA_UINT16 i = 0; i < count; i++)
                        {
                            PND_XML_LOCAL_SCF_ADAPTION_PTR_TYPE local_scf_list = new pnd_xml_local_scf_adaption_tag;
                            block_length = getUINT16FromRecord(record.record_data, pointer);
                            // IOCRReference
                            local_scf_list->iocr_ref = getUINT16FromRecord(record.record_data, pointer+2);
                            // LocalSendClockFactor
                            local_scf_list->send_clock_factor = getUINT16FromRecord(record.record_data, pointer+4);
                            // LocalReductionRatio
                            local_scf_list->reduction_ratio = getUINT16FromRecord(record.record_data, pointer+6);
                            // LocalPhase
                            local_scf_list->phase = getUINT16FromRecord(record.record_data, pointer+8);
                            // LocalFrameSendOffset
                            local_scf_list->frame_send_offset = getUINT32FromRecord(record.record_data, pointer+12);
                            // LocalDataHoldFactor
                            local_scf_list->data_hold_factor = getUINT16FromRecord(record.record_data, pointer+18);

                            pointer = pointer + block_length;
                            local_scf_adaption->push_back(local_scf_list);
                        }

                        xml_free(record.record_data);
                    }
                    
                }
            }
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getARRecordData(LSA_UINT16 devNr, RecordVector* ar_record_list)
{
    LSA_UINT16      result;  
    xml_element     element;

    result          = PNIO_ERR_CORRUPTED_DATA;
    element         = LSA_NULL;


    element = getDevice(devNr);

    if(element != LSA_NULL)
    {

        // find AR_RECORD_DATA
        element = xml_locate_element_by_child_content(element, "AID", "13", 1);
        if (element != LSA_NULL) 
        {
            element = xml_stdint_get_element_by_name(element, "Value");
            if (element != LSA_NULL) 
            {
                // AR_RECORD_DATA
                element = xml_stdint_get_element_by_attribute(element, "Key", "12549");
                if (element != LSA_NULL) 
                {
                    PND_XML_RECORD_TYPE record;
                    LSA_UINT16 pointer, block_length, count;

                    result = getRecord(element, &record);

                    if(result == PNIO_OK)
                    {
                        pointer = XML_BLOCK_HEADER;
                        count = getUINT16FromRecord(record.record_data, pointer);
                        pointer = pointer + 2;
                        for (LSA_UINT16 i = 0; i < count; i++)
                        {
                            PND_XML_RECORD_PTR_TYPE ar_record = (PND_XML_RECORD_PTR_TYPE) xml_malloc( sizeof(PND_XML_RECORD_TYPE));
                            block_length = getUINT16FromRecord(record.record_data, pointer);
                            ar_record->record_data_length = getUINT16FromRecord(record.record_data, pointer+2);
                            ar_record->record_index = getUINT16FromRecord(record.record_data, pointer+4);
                            ar_record->record_data = (LSA_UINT8*)xml_malloc(ar_record->record_data_length);
                            xml_memcpy (ar_record->record_data, record.record_data+pointer+8, ar_record->record_data_length);
                            pointer = pointer + block_length;
                            ar_record_list->push_back(ar_record);
                        }
                        xml_free(record.record_data);
                    }
                }
            }
        }
    }
    return result;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::prepareFromOutputIOC( LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput )
{
    XML_ASSERT( pOutput != 0 );

    // Update settings from Outputs received from LD OPEN
    m_InternalXMLData[ifNr].ioc.ioc_hd.max_devices = pOutput->ioc_max_devices;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPnioXMLStore::prepareFromOutputOHA( LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput )
{
    XML_ASSERT( pOutput != 0 );

    // Update settings from Outputs received from LD OPEN
    m_InternalXMLData[ifNr].oha.oha_hd.nr_of_usr_ports = pOutput->nr_of_usr_ports;
}

/*----------------------------------------------------------------------------*/
LSA_VOID* CPnioXMLStore::createArcbExpectedConfig( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_VOID* psiSysPtr )
{
    CM_UPPER_ARCB_ADD_PTR_TYPE  pARCB = 0;
    CLRPC_UUID_PTR_TYPE         pIrUUID = 0;
    LSA_UINT16                  ifNr;

    ifNr                          = cpHandle->if_nr;

    // Add AR properties
    LSA_UINT32 dfp_iocr_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.size();
    if(dfp_iocr_count > 0)
    {
        pIrUUID = &m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.ir_data_uuid;
    }

    pARCB = cm_arcb_new_io_ar( // create AR ARCB setting
        psiSysPtr,
        devNr,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.ar_nr,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.ar_type,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.ar_uuid,
        m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_data.ar_properties,
        pIrUUID );

    XML_ASSERT( pARCB != 0 );

    // Add AR Alarm properties
    CM_UPPER_ARCB_ALARM_CR_PTR_TYPE pALCR = cm_arcb_append_alarmcr( 
            psiSysPtr,
            pARCB,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->alarm_cr_data.alcr_properties,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->alarm_cr_data.timeout100ms,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->alarm_cr_data.retries,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->alarm_cr_data.alcr_tag_header_high,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->alarm_cr_data.alcr_tag_header_low);

    XML_ASSERT( pALCR != 0 );

    // Add AR PRM records properties
    LSA_UINT16 record_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.size();
    for ( LSA_UINT16 recNr = 0; recNr < record_count; recNr ++)
    {
        LSA_UINT8* record_data = (LSA_UINT8*)xml_malloc( m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_data_length);
        XML_ASSERT( record_data != 0 );
        xml_memcpy(record_data, 
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_data, 
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_data_length);

        CM_UPPER_ARCB_RECORD_PTR_TYPE pREC = cm_arcb_append_prm_record( 
            psiSysPtr,
            pARCB,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_index,
            (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_data_length,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->ar_record_list.at(recNr)->record_data,
            LSA_TRUE );

        XML_ASSERT( pREC != 0 );

        if ( record_data != 0 )
        {
            xml_free( record_data );
        }
    }

    // Add IOCR properties
    LSA_UINT16 iocr_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.size();

    for ( LSA_UINT16 iocrIdx = 0; iocrIdx < iocr_count; iocrIdx ++)
    {
        CM_MAC_TYPE mcMAC;

        mcMAC.mac[0] = 0;
        mcMAC.mac[1] = 0;
        mcMAC.mac[2] = 0;
        mcMAC.mac[3] = 0;
        mcMAC.mac[4] = 0;
        mcMAC.mac[5] = 0;

        LSA_UINT32 iocrProp = m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->iocr_properties;
        LSA_UINT16 dataLen  = m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->data_length;

        // Note: PI buffer (KRAM, IOCC,...) is assigned later at process image alloc
        CM_UPPER_ARCB_IOCR_PTR_TYPE pIOCR = cm_arcb_append_iocr( 
            psiSysPtr,
            pARCB,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->iocr_type,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->iocr_ref,
            iocrProp,
            getDatalenIOCR( dataLen, iocrProp ),
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->frame_id,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->send_clock,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->reduction_ratio,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->phase,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->frame_send_offset,
            m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->data_hold_factor,
            0xC000, // fixed value
            mcMAC );

        XML_ASSERT( pIOCR != 0 );


        // Add local sendclock adaption properties
        LSA_UINT16 scf_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.size();
        for (LSA_UINT16 scfIdx = 0; scfIdx < scf_count; scfIdx ++)
        {
            if (m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->iocr_ref == m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->iocr_ref)
            {
                CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE pADPT = cm_arcb_append_iocr_local( 
                psiSysPtr,
                pARCB,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->iocr_ref,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->send_clock_factor,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->reduction_ratio,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->phase,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->data_hold_factor,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->local_scf_list.at(scfIdx)->frame_send_offset);

                XML_ASSERT( pADPT != 0 );
            }
        }

        // Add subframe properties
        LSA_UINT16 dfp_iocr_entry_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.size();
        for (LSA_UINT16 dfpiocrIdx = 0; dfpiocrIdx < dfp_iocr_entry_count; dfpiocrIdx ++)
        {
            if (m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.at(dfpiocrIdx)->iocr_ref == m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->iocr_ref)
            {
                CM_UPPER_ARCB_IR_INFO_SUB_PTR_TYPE pSUB = cm_arcb_append_iocr_ir_info_sub(
                psiSysPtr,
                pARCB,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.at(dfpiocrIdx)->iocr_ref,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.at(dfpiocrIdx)->subframe_offset,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->ir_info_data.dfp_iocr_list.at(dfpiocrIdx)->subframe_data);

                XML_ASSERT( pSUB != 0 );
            }
        }

        LSA_UINT16 api_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.size();

        for ( LSA_UINT16 apiIdx = 0; apiIdx < api_count; apiIdx++)
        {
            // Add IODATA properties
            LSA_UINT16 iodata_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iodata_list.size();
            for ( LSA_UINT16 iodataIdx = 0; iodataIdx < iodata_count; iodataIdx++)
            {
                CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE pIODATA = cm_arcb_append_iodata_objects( 
                    psiSysPtr,
                    pIOCR,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->api_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iodata_list.at(iodataIdx)->slot_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iodata_list.at(iodataIdx)->subslot_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iodata_list.at(iodataIdx)->frame_offset);

                XML_ASSERT( pIODATA != 0 );        
            }

            // Add IOCS properties
            LSA_UINT16 iocs_count = (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iocs_list.size();
            for ( LSA_UINT16 iocsIdx = 0; iocsIdx < iocs_count; iocsIdx++)
            {
                CM_UPPER_ARCB_IOCS_PTR_TYPE pIOCS = cm_arcb_append_iocs( 
                    psiSysPtr,
                    pIOCR,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->api_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iocs_list.at(iocsIdx)->slot_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iocs_list.at(iocsIdx)->subslot_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->iocr_data.at(iocrIdx)->api_list.at(apiIdx)->iocs_list.at(iocsIdx)->frame_offset);

                    XML_ASSERT( pIOCS != 0 );
            }
        }
    }

    /** **/

    // Add submod properties (expected config tree)

    LSA_UINT16 api_count =  (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.size();

    for ( LSA_UINT16 apiIdx = 0; apiIdx < api_count; apiIdx++)
    {
        LSA_UINT16 mod_count =  (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.size();    
        for ( LSA_UINT16 modIdx = 0; modIdx < mod_count; modIdx++)
        {
            CM_UPPER_ARCB_MODULE_PTR_TYPE pMOD = cm_arcb_append_module( 
                psiSysPtr,
                pARCB,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->api_nr,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->slot_nr,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->module_properties,
                m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->module_ident);

            XML_ASSERT( pMOD != 0 );

            LSA_UINT16 submod_count =  (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.size();
            for ( LSA_UINT16 submodIdx = 0; submodIdx < submod_count; submodIdx ++ )
            {


                // set default values
                LSA_UINT16  sub_input_data_descr  = 0;
                LSA_UINT16  sub_input_data_len    = 0;
                LSA_UINT16  sub_input_iocs_len    = 0;
                LSA_UINT16  sub_input_iops_len    = 0;
                LSA_UINT16  sub_output_data_descr = 0;
                LSA_UINT16  sub_output_data_len   = 0;
                LSA_UINT16  sub_output_iocs_len   = 0;
                LSA_UINT16  sub_output_iops_len   = 0;

                // overwrite default values
                LSA_UINT32 subslot_descr_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.size();
                for( LSA_UINT16 m = 0; m < subslot_descr_count; m ++ )
                {
                    if (m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->data_description == 1)
                    {
                        sub_input_data_descr  = 1;
                        sub_input_data_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->data_length;
                        sub_input_iocs_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->iocs_length;
                        sub_input_iops_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->iops_length;
                    }
                    else
                    {
                        sub_output_data_descr  = 2;
                        sub_output_data_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->data_length;
                        sub_output_iocs_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->iocs_length;
                        sub_output_iops_len    = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_submodule_descr_list.at(m)->iops_length;           
                    }
                }

                CM_UPPER_ARCB_SUBMODULE_PTR_TYPE pSUB;

                pSUB = cm_arcb_append_submodule( 
                    psiSysPtr, 
                    pMOD,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->subslot_nr,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->sub_properties,
                    m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->sub_ident,

                    sub_input_data_descr,
                    sub_input_data_len,
                    (LSA_UINT8)sub_input_iocs_len,
                    (LSA_UINT8)sub_input_iops_len,

                    sub_output_data_descr,
                    sub_output_data_len,
                    (LSA_UINT8)sub_output_iocs_len,
                    (LSA_UINT8)sub_output_iops_len);

                XML_ASSERT( pSUB != 0 );


                // add the PRM records for subslot
                LSA_UINT32  arcb_record_count = m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.size();				

                for ( LSA_UINT16 recNr = 0; recNr < arcb_record_count; recNr ++ )
                {

                    LSA_UINT8* record_data = (LSA_UINT8*)xml_malloc( 
                            m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->record.record_data_length);
                    XML_ASSERT( record_data != 0 );
                    xml_memcpy(record_data, 
                            m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->record.record_data,
                            m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->record.record_data_length);

                    CM_UPPER_ARCB_RECORD_PTR_TYPE pREC = cm_arcb_append_record( 
                        psiSysPtr,
                        pSUB,
                        m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->record.record_index,
                        m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->transfer_sequence,
                        (LSA_UINT16)m_InternalXMLData[ifNr].iod_list.at(devNr)->exp_submod_data.at(apiIdx)->arcb_slot_list.at(modIdx)->arcb_submodule_list.at(submodIdx)->arcb_record_list.at(recNr)->record.record_data_length,
                        record_data,
                        LSA_TRUE );

                    XML_ASSERT( pREC != 0 );

                    if ( record_data != 0 )
                    {
                        xml_free( record_data );
                    }
                }

            } // Submodules
        } // Modules
    } // APIs

    return ( pARCB );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getDatalenIOCR( LSA_UINT16 iocrDatalen, LSA_UINT32 iocrProp )
{
    LSA_UINT16 dataLen = iocrDatalen;

    // override the minsize if the configured one is too small for IOCR with type
    switch ( iocrProp & CM_IOCR_PROP_RT_CLASS_MASK )
    {
        case CM_IOCR_PROP_RT_CLASS_1:
        case CM_IOCR_PROP_RT_CLASS_2:
            {
                if ( iocrDatalen < CM_PNIO_C_SDU_MIN_LENGTH_RT )
                {
                    PND_PARAMS_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                        "ioc_get_iocr_datalen(): data_length(%u) for RT_CLASS_1/2 too small, enlarge to CM_PNIO_C_SDU_MIN_LENGTH_RT(%u)",
                        iocrDatalen, CM_PNIO_C_SDU_MIN_LENGTH_RT );

                    dataLen = CM_PNIO_C_SDU_MIN_LENGTH_RT;
                }
            }
            break;

        case CM_IOCR_PROP_RT_CLASS_1_UDP:
            {
                if ( iocrDatalen < CM_PNIO_C_SDU_MIN_LENGTH_UDP )
                {
                    PND_PARAMS_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                        "ioc_get_iocr_datalen(): data_length(%u) for RT_CLASS_1_UDP too small, enlarge to CM_PNIO_C_SDU_MIN_LENGTH_UDP(%u)",
                        iocrDatalen, CM_PNIO_C_SDU_MIN_LENGTH_UDP );

                    dataLen = CM_PNIO_C_SDU_MIN_LENGTH_UDP;
                }
            }
            break;

        default:
            break;
    }

    return ( dataLen );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getRecord(xml_element element, PND_XML_RECORD_PTR_TYPE record)
{
    LSA_UINT16  result;
    LSA_UINT32  string_length, content_length, record_length, record_index;
    LSA_UINT8   status;
    LSA_CHAR*   record_data_str;
    LSA_UINT8*  record_data;
    LSA_CHAR    length[6];
    LSA_CHAR    key[11];

    result          = PNIO_ERR_CORRUPTED_DATA;
    status          = LSA_FALSE;
    record_length   = 0;
    content_length  = 0;
    string_length   = 0;
    record_data_str = LSA_NULL;
    record_data     = LSA_NULL;


    XML_ASSERT(element);

    // read record length
    string_length = xml_stdint_get_attribute_value(element, "Length", length, sizeof(length));
    record_length = xml_string_to_u32(length, &status, LSA_FALSE);

    // read index
    string_length = xml_stdint_get_attribute_value(element, "Key", key, sizeof(key));
    record_index  = xml_string_to_u32(key, &status, LSA_FALSE);

    if(status != LSA_FALSE)
    {
        // get a pointer to the allocated memory with the record content
        record_data_str = (LSA_CHAR*)xml_stdint_get_element_content2(element, &content_length);
		if (content_length > 0)
		{
            // compare record length in xml file with the the length of the allocated memory
            if (content_length == (record_length*2))
            {
                record_data = (LSA_UINT8*)xml_malloc(content_length/2);
                status = xml_hex_string_to_byte_array(record_data_str, record_data, (content_length/2));

                record->record_data         = record_data;
                record->record_data_length  = record_length;
                record->record_index        = record_index;
                result = PNIO_OK;
            }
		}

		xml_free(record_data_str);
    }

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPnioXMLStore::getValueFromRecord(LSA_UINT8* record_data, LSA_UINT16 start, LSA_UINT16 length)
{
    LSA_CHAR*   buffer;
    LSA_UINT16  i;
    LSA_UINT32  v;

    buffer    = LSA_NULL;
    i         = 0;
    v     = 0;

    XML_ASSERT(length<=4); // 32-bit value can not have more then 4 bytes

    for (i=0; i<length; i++)
    {
        v = v + (record_data[start+i]<<((length - i -1)*8));
    }
  

    return v;
}

/*----------------------------------------------------------------------------*/
LSA_UINT8 CPnioXMLStore::getUINT8FromRecord(LSA_UINT8* record_data, LSA_UINT16 start)
{
    LSA_UINT32 value;
    value = 0;

    value = getValueFromRecord(record_data, start, 1);

    return (LSA_UINT8)value;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CPnioXMLStore::getUINT16FromRecord(LSA_UINT8* record_data, LSA_UINT16 start)
{
    LSA_UINT32 value;
    value = 0;

    value = getValueFromRecord(record_data, start, 2);

    return (LSA_UINT16)value;
}

/*----------------------------------------------------------------------------*/
LSA_UINT32 CPnioXMLStore::getUINT32FromRecord(LSA_UINT8* record_data, LSA_UINT16 start)
{
    LSA_UINT32 value;
    value = 0;

    value = getValueFromRecord(record_data, start, 4);

    return value;
}

/*----------------------------------------------------------------------------*/
xml_element  CPnioXMLStore::getDecentralIOSystem()
{
    LSA_UINT32      content_length;
    xml_element     element;
    LSA_UINT8*      cio_system_id;

    content_length  = 0;
    element         = LSA_NULL;
    cio_system_id   = LSA_NULL;

    // get the Link
    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (element != LSA_NULL)
        {
            element = xml_locate_element_by_child_content(element, "AID", "16", 1); // Link
            if (element != LSA_NULL)
            {
                // locate Link element
                element = xml_stdint_get_element_by_name(element, "TargetRID");
                // get the Link as character string
                cio_system_id = xml_stdint_get_element_content2(element, &content_length);
            }
        }
    }

    // find the corresponding DecentralIOSystem
    element = xml_locate_element_by_child_content(m_RootXMLElement, "RID", (LSA_CHAR*)cio_system_id, 1); // decentralIOsystem

    xml_free(cio_system_id);

    return element;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getDeviceCount()
{
    LSA_UINT16    device_count;
    xml_element   element, decentr_ios;

    device_count  = 0;
    element       = LSA_NULL;
    decentr_ios   = LSA_NULL;


    decentr_ios   = getDecentralIOSystem();

    XML_ASSERT(decentr_ios);

    // count number of IODevices
    do
    {
        device_count++;
        element = xml_locate_element_by_child_content(decentr_ios, "ClassRID", "6", device_count);
    }
    while (element != LSA_NULL);

    return(device_count - 1);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CPnioXMLStore::getPortCount()
{
    LSA_UINT16          port_count;
    xml_element         element, element1;

    port_count          = 0;
    element             = LSA_NULL;
    element1            = LSA_NULL;

    element = xml_locate_element_by_child_content(m_RootXMLElement, "ClassRID", "2", 1); // CPU
    if (element != LSA_NULL)
    {
        // find the right Interface
        element = xml_locate_element_by_child_content(element, "ClassRID", "3", m_hdNr); // IOinterface
        if (element != LSA_NULL)
        {        
            //get next element
            element = xml_stdint_get_next_element(LSA_NULL, element);
            while (element != LSA_NULL)
            {
                // if this is a port
                element1  = xml_locate_element_by_content(element, "ClassRID", "4");
                if(element1 != LSA_NULL)
                {
                    port_count++;
                }
                // if this is an interface
                element1  = xml_locate_element_by_content(element, "ClassRID", "3");
                if(element1 != LSA_NULL)
                {
                    break;
                }
                //get next element
                element = xml_stdint_get_next_element(LSA_NULL, element);
            }
        }
    }

    return port_count;
}

/*----------------------------------------------------------------------------*/
xml_element  CPnioXMLStore::getDevice(LSA_UINT16 DevNr)
{
    xml_element element;

    element = getDecentralIOSystem();
    element = xml_locate_element_by_child_content(element, "ClassRID", "6", DevNr);

    return element;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPnioXMLStore::getUUID(LSA_UINT8* record_data, LSA_UINT16 start_byte, CLRPC_UUID_PTR_TYPE uuid)
{

    uuid->time_low = getUINT32FromRecord(record_data, start_byte);
    uuid->time_mid = getUINT16FromRecord(record_data, start_byte+4);
    uuid->time_hi_and_version = getUINT16FromRecord(record_data, start_byte+6);
    uuid->clock_seq_hi_and_reserved = getUINT8FromRecord(record_data, start_byte+8);
    uuid->clock_seq_low = getUINT8FromRecord(record_data, start_byte+9);

    for (LSA_UINT16 i = 0; i < 6; i++)
    {
        uuid->node[i] = getUINT8FromRecord(record_data, start_byte+10+i);
    }
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
