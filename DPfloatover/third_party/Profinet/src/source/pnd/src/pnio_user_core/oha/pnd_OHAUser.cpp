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
/*  F i l e               &F: OHAUser.cpp                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation for OHA User (IF specific)                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3403
#define PND_MODULE_ID       3403

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
// COHAUser OHA user class
/////////////////////////////////////////////////////////////////////////////

COHAUser::COHAUser() : CUserNode()
{
	m_pnioHandle    = OHA_INVALID_HANDLE;
	m_sysPath       = PSI_SYS_PATH_INVALID;
	m_pEventHandler = 0;
	m_responseRQB   = 0;

	m_nameOfStation = "";
	m_ipAddr        = "0.0.0.0";
	m_subnetMask    = "0.0.0.0";
	m_gatewayIP     = "0.0.0.0";

	pnd_memset( &m_ohaParam, 0, sizeof( PND_PARAM_OHA_HD_IF_TYPE ));
    pnd_memset( &m_StationType, 0, sizeof( m_StationType ));
}

/*----------------------------------------------------------------------------*/
COHAUser::COHAUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent ) : CUserNode( pHandle, pParent )
{
	m_pnioHandle    = OHA_INVALID_HANDLE;
	m_sysPath       = PSI_SYS_PATH_INVALID;
	m_pEventHandler = 0;
	m_responseRQB   = 0;

	m_nameOfStation = "";
	m_ipAddr        = "0.0.0.0";
	m_subnetMask    = "0.0.0.0";
	m_gatewayIP     = "0.0.0.0";

	pnd_memset( &m_ohaParam, 0, sizeof( PND_PARAM_OHA_HD_IF_TYPE ));
    pnd_memset( &m_StationType, 0, sizeof( m_StationType ));
}

/*----------------------------------------------------------------------------*/
COHAUser::~COHAUser()
{
	if ( m_pEventHandler != 0 )
	{
		delete( m_pEventHandler );
		m_pEventHandler = 0;
	}
}

/*----------------------------------------------------------------------------*/
void COHAUser::Create( LSA_VOID_PTR_TYPE pConfig )
{
	// Setup the internal data structure (from OHA params)
	PND_PARAM_OHA_HD_IF_PTR_TYPE pOHA = (PND_PARAM_OHA_HD_IF_PTR_TYPE)pConfig;
	PND_ASSERT( pOHA != 0 );

	m_ohaParam = *pOHA;

	// Prepare SYS path for IF specific CM-CL user
	PSI_SYSPATH_SET_PATH( m_sysPath, PSI_PATH_IF_APP_OHA );
	PSI_SYSPATH_SET_IF( m_sysPath, m_pndHandle.if_nr );
	PSI_SYSPATH_SET_HD( m_sysPath, m_pndHandle.hd_nr );

	m_pEventHandler = new COHAEventHandler( &m_pndHandle, this );
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE COHAUser::GetPnioHandle()
{
	return ( m_pnioHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::Startup( LSA_VOID* p_rb )
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStartup  = LSA_TRUE;
	m_isShutdown = LSA_FALSE;

	// Store the upper RQB, and Startup with open the PNIO channel for OHA
	m_responseRQB = pRQB;

	OpenChannel();
}

/*----------------------------------------------------------------------------*/
void COHAUser::Shutdown( LSA_VOID* p_rb )
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isShutdown = LSA_TRUE;
	m_isStartup  = LSA_FALSE;

	// Store the upper RQB, start shutdown OHA user till closing the PNIO channel
	m_responseRQB = pRQB;

	if ( m_pnioHandle != OHA_INVALID_HANDLE )  // channel open ?
	{
		// start shutdown user and till close the PNIO channel
		CloseChannel();
	}
	else
	{
		// signal shutdown complete
		shutdownDone( PNIO_OK );
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string COHAUser::GetKey()
{
	CValueHelper valObj;

	string result = valObj.UINT16ToString( m_pndHandle.pnio_if_nr );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT COHAUser::GetNumber()
{
	return ( m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
/*  OHA Request functions                                                     */
/*----------------------------------------------------------------------------*/

void COHAUser::OpenChannel()
{
	PND_ASSERT( m_pnioHandle == OHA_INVALID_HANDLE      );
	PND_ASSERT( m_sysPath    != PSI_SYS_PATH_INVALID );

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_OPEN_CHANNEL, m_pnioHandle, &m_pndHandle );
	PND_ASSERT(pRQB);

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::OpenChannel(): Open OHA user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
		m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

	LSA_SYS_PATH_TYPE path = PSI_SYSPATH_GET_PATH( m_sysPath );

	pRQB->args.channel.handle                     = OHA_INVALID_HANDLE;
	pRQB->args.channel.handle_upper               = (LSA_UINT8)path;
	pRQB->args.channel.sys_path                   = m_sysPath;
	pRQB->args.channel.oha_request_upper_done_ptr = (OHA_UPPER_CALLBACK_FCT_PTR_TYPE)pnd_pnio_user_request_lower_done;

	eps_user_open_channel_lower( pRQB, this, COHAUser::OpenChannelDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::CloseChannel()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_CLOSE_CHANNEL, m_pnioHandle, &m_pndHandle );
	PND_ASSERT(pRQB);

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::CloseChannel(): Close OHA user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
		m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

	// Stop listening OHA indications
	m_pEventHandler->StopListening();

	eps_user_close_channel_lower( pRQB, this, COHAUser::CloseChannelDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::RemaRead( LSA_UINT32 remaIdx, LSA_UINT32 dataSize )
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_REMA_READ, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );
	PND_ASSERT( dataSize <= 0xFFFF );

	pRQB->args.RemaData.data = (LSA_UINT8*)pnd_oha_alloc_upper_mem( (LSA_UINT16)dataSize, &m_pndHandle );

	pRQB->args.RemaData.index       = remaIdx;
	pRQB->args.RemaData.data_length = dataSize;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::RemaRead(): OHA REMA Read, DataIdx(%#x) ReadSize(%u) rqb(%08x)",
		remaIdx, dataSize, pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::RemaReadDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::DcpAccessInit()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	// Disable remote setting for IP, NoS, RTF
	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_DCP_CONTROL, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

	pRQB->args.DcpControl.Mode      = OHA_QUERY_WRITE;
	pRQB->args.DcpControl.Options   = OHA_CTRL_STATION_NAME | OHA_CTRL_IP_SUITE | OHA_CTRL_RESET_TO_FACTORY;
	pRQB->args.DcpControl.Type      = 0;
	pRQB->args.DcpControl.SubType   = 0;
	pRQB->args.DcpControl.pAddrInfo = 0;
	
	PND_OHA_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::DcpAccessInit(): OHA DCP Access Control, mode(%#x) options(%#x) Type(%#x) SubType(%#x) rqb(%08x)",
		pRQB->args.DcpControl.Mode, 
		pRQB->args.DcpControl.Options,
		pRQB->args.DcpControl.Type,
		pRQB->args.DcpControl.SubType,
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::DcpAccessInitDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetDcpRole()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_DCP_WRITE_ELEMENT, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

	pRQB->args.DcpElem.Type         = DCP_DBID_IDENT;
	pRQB->args.DcpElem.SubType      = DCP_DSID_IDROLE;
	pRQB->args.DcpElem.Remanent     = LSA_FALSE;
	pRQB->args.DcpElem.Length       = 1;   // One byte for Role
	pRQB->args.DcpElem.pVariable[0] = DCP_PN_CONTROLLER;
	pRQB->args.DcpElem.AccCtrl      = OHA_READ_ONLY;

	PND_OHA_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SetDcpRole(); OHA Set DCP Role, role(%#x) rqb(0x%08x)",
	    pRQB->args.DcpElem.pVariable[0],
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SetDcpRoleDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetDcpStationType()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );


	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_DCP_WRITE_ELEMENT, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

	pnd_sprintf( m_StationType,
		"PNDriver_%04x%04x",
		m_ohaParam.vendor_id,
		m_ohaParam.device_id );

	pRQB->args.DcpElem.Type         = OHA_DCP_OPTION_IDENT;
	pRQB->args.DcpElem.SubType      = DCP_DSID_IDTYPE;
	pRQB->args.DcpElem.Remanent     = LSA_FALSE;
    pnd_memcpy(pRQB->args.DcpElem.pVariable, m_StationType, PSI_STRLEN(m_StationType));
    pRQB->args.DcpElem.Length    = (LSA_UINT16)PSI_STRLEN(m_StationType);
	pRQB->args.DcpElem.AccCtrl      = OHA_READ_ONLY;

	PND_OHA_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SetDcpRole(); OHA Set DCP Station Type, rqb(0x%08x)",
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SetDcpStationTypeDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::LldpAccessInit( LSA_UINT16 portID )
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );
	PND_ASSERT(( portID > 0 ) && ( portID <= PSI_CFG_MAX_PORT_CNT ));

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_ACCESS_CONTROL, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

	pRQB->args.LldpAccess.PortId      = portID;
	pRQB->args.LldpAccess.AccessMode  = OHA_READ_ONLY;
	pRQB->args.LldpAccess.Options     = 0;
	pRQB->args.LldpAccess.PortOptions = 0xFFFFFFFF; // all options

	PND_OHA_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::LldpAccessInit(): OHA LLDP Access Init, portID(%u) mode(%u) options(%#x) port_opt(%#x) rqb(%08x)", 
		pRQB->args.LldpAccess.PortId,
		pRQB->args.LldpAccess.AccessMode,
		pRQB->args.LldpAccess.Options,
		pRQB->args.LldpAccess.PortOptions,
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::LldpAccessInitDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteSysName()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_WRITE_OPTION, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

    pRQB->args.LldpOption.PortId    = 0;
    pRQB->args.LldpOption.Type      = OHA_LLDP_TYPE_SYSTEM_NAME; 
    pRQB->args.LldpOption.SubType   = 0;
    pRQB->args.LldpOption.OUI       = 0;
    pRQB->args.LldpOption.Length    = 0;
	
	PND_OHA_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteSysName(): OHA SNMP Write sysName, PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) Length(%u) rqb(0x%08x)",
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SNMPWriteSysNameDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteSysDescr()
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );
    OHA_COMMON_MEM_U8_PTR_TYPE sys_descr = (OHA_COMMON_MEM_U8_PTR_TYPE)"Siemens AG, PN DRIVER, PN DRIVER, 6ES7195-3AA00-0YA0, HW: Version 0, FW: Version V1.0, S0000000";

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_WRITE_OPTION, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

    pRQB->args.LldpOption.PortId    = 0;
    pRQB->args.LldpOption.Type      = OHA_LLDP_TYPE_SYSTEM_DESCR; 
    pRQB->args.LldpOption.SubType   = 0;
    pRQB->args.LldpOption.OUI       = 0;
    pnd_memcpy(pRQB->args.LldpOption.pVariable, sys_descr, PSI_STRLEN(sys_descr));
    pRQB->args.LldpOption.Length    = (LSA_UINT16)PSI_STRLEN(sys_descr);
	
	PND_OHA_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteSysDescr(): OHA SNMP Write sysDescr, PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) StringLength(%u) rqb(0x%08x)",
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SNMPWriteSysDescrDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteIfDescr( LSA_UINT16 portID )
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );
    LSA_UINT8 if_descr[255];

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_LLDP_WRITE_OPTION, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );



    if( portID == 0)
    {
        PSI_SPRINTF(if_descr, "Siemens AG, PN DRIVER, PN DRIVER, 6ES7195-3AA00-0YA0, HW: Version 0, FW: Version %c%02u.%02u.%02u, S0000000, internal, X%u",PND_VERSION_REVISION_PREFIX, PND_VERSION_FUNCTIONAL_ENHANCEMENT, PND_VERSION_BUG_FIX, PND_VERSION_INTERNAL_CHANGE, m_pndHandle.pnio_if_nr);
    }
    else
    {
        PSI_SPRINTF(if_descr, "Siemens AG, PN DRIVER, PN DRIVER, 6ES7195-3AA00-0YA0, HW: Version 0, FW: Version %c%02u.%02u.%02u, S0000000, Ethernet Port X%u P%u", PND_VERSION_REVISION_PREFIX, PND_VERSION_FUNCTIONAL_ENHANCEMENT, PND_VERSION_BUG_FIX, PND_VERSION_INTERNAL_CHANGE, m_pndHandle.pnio_if_nr, portID);
    }

    pRQB->args.LldpOption.PortId    = portID;
    pRQB->args.LldpOption.Type      = OHA_LLDP_TYPE_PORT_DESCR; 
    pRQB->args.LldpOption.SubType   = 0;
    pRQB->args.LldpOption.OUI       = 0;
    pnd_memcpy(pRQB->args.LldpOption.pVariable, if_descr, PSI_STRLEN(if_descr));
    pRQB->args.LldpOption.Length    = (LSA_UINT16)PSI_STRLEN(if_descr);
	
	PND_OHA_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteIfDescr(): OHA SNMP Write ifDescr, PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) StringLength(%u) rqb(0x%08x)",
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SNMPWriteIfDescrDone );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetAddress( LSA_VOID* p_rb )
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	PND_RQB_PTR_TYPE pPndRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);
	PND_ASSERT( pPndRQB != 0);

	PNIO_IPv4 IpParam = pPndRQB->args.set_ip_and_nos.IpV4; 
	PNIO_NOS NoSParam = pPndRQB->args.set_ip_and_nos.NoS;
	PNIO_SET_IP_NOS_MODE_TYPE Mode = pPndRQB->args.set_ip_and_nos.mode;

	if( m_ohaParam.IPAddrValAllowed != LSA_TRUE )
	{
		if( (Mode & PNIO_SET_IP_MODE) == PNIO_SET_IP_MODE || (Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE )
		{
			pPndRQB->args.set_ip_and_nos.pnio_err = PNIO_ERR_SET_IP_NOS_NOT_ALLOWED;

			pnd_pnio_user_request_iobc_done( pPndRQB ); 
			return;
		}
	}
	if( m_ohaParam.NameOfStationValAllowed != LSA_TRUE)
	{
		if( (Mode & PNIO_SET_IP_MODE) == PNIO_SET_IP_MODE || (Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE )
		{
			pPndRQB->args.set_ip_and_nos.pnio_err = PNIO_ERR_SET_IP_NOS_NOT_ALLOWED;

			pnd_pnio_user_request_iobc_done( pPndRQB );
			return;
		}
	}

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_SET_ADDRESS, m_pnioHandle, &m_pndHandle );
	PND_ASSERT( pRQB != 0 );

	OHA_ADDRESS_TYPE* pAddressType = &pRQB->args.Address;

	pAddressType->AccessMode     = OHA_QUERY_WRITE;
    pAddressType->Options        = 0;
	pAddressType->Remanent       = 0;
	pAddressType->CheckIpEnabled = LSA_TRUE;
	pAddressType->pStationData->DeviceId  = m_ohaParam.device_id;
    pAddressType->pStationData->VendorId  = m_ohaParam.vendor_id;


    if((Mode & PNIO_SET_IP_MODE) == PNIO_SET_IP_MODE)
    {
        pAddressType->Options |= OHA_CTRL_IP_SUITE;

		// Remanent Storage Settings
		if( IpParam.Remanent )
		{
			pAddressType->Remanent |= OHA_CTRL_IP_SUITE;
		}

		pnd_memcpy( pAddressType->pStationData->LocalIpAddr, IpParam.IpAddress, OHA_IP_ADDR_SIZE );
		pnd_memcpy( pAddressType->pStationData->LocalSubnetMask, IpParam.NetMask, OHA_IP_ADDR_SIZE );
		pnd_memcpy( pAddressType->pStationData->DefaultRouterAddr, IpParam.Gateway, OHA_IP_ADDR_SIZE );
    }

    if((Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE)
    {
        pAddressType->Options |= OHA_CTRL_STATION_NAME;

		// Remanent Storage Settings
		if( NoSParam.Remanent )
		{
			pAddressType->Remanent |= OHA_CTRL_STATION_NAME;
		}

		pAddressType->pStationData->StationNameLen = NoSParam.Length;
								
		pAddressType->pStationData->pStationName = (LSA_UINT8*)pnd_oha_alloc_upper_mem( OHA_MAX_STATION_NAME_LEN, &m_pndHandle );
		PND_ASSERT( pAddressType->pStationData->pStationName != 0 );

		pnd_memcpy( pAddressType->pStationData->pStationName, NoSParam.Nos, OHA_MAX_STATION_NAME_LEN );
    }

    PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "COHAUser::SetAddress(); OHA Set Address, Mode(%d) Options(%#x) rqb(0x%08x)",
	    pPndRQB->args.set_ip_and_nos.mode,
        pRQB->args.Address.Options,
		pRQB );

	// store PND RQB in user ID for done handling
	LSA_RQB_SET_USER_ID_PTR( pRQB, pPndRQB );

	eps_user_request_lower( pRQB, this, COHAUser::SetAddressDone );
}

void COHAUser::WriteParameter( LSA_VOID* p_rb )
{
	PND_ASSERT( m_pnioHandle != OHA_INVALID_HANDLE );

	PND_RQB_PTR_TYPE pPndRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);
	PND_ASSERT( pPndRQB != 0);

    CParamStore* pParamStore = CParamStore::GetInstance();
    PND_ASSERT( pParamStore != 0 );

    switch(pPndRQB->args.prm_write.record_index)
    {
        case 0xF8000002: /* MultipleSNMPRecord */ 
            PND_REMA_RECORD_TYPE *pMultipleSNMPRecord;

            pMultipleSNMPRecord = (PND_REMA_RECORD_TYPE*)pParamStore->GetParamMultipleSNMPRecord();
            
            if(pMultipleSNMPRecord != LSA_NULL)
            {
                OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)pnd_oha_alloc_rqb( OHA_OPC_PRM_WRITE, m_pnioHandle, &m_pndHandle );
	            PND_ASSERT( pRQB != 0 );

                OHA_PRM_DATA_TYPE* pPrmData = &pRQB->args.PrmData;

                pPrmData->subslot_number     = pPndRQB->args.prm_write.subslot_number;
                pPrmData->slot_number        = pPndRQB->args.prm_write.slot_number;
                pPrmData->record_index       = pPndRQB->args.prm_write.record_index;
                pPrmData->mk_rema            = pPndRQB->args.prm_write.mk_rema;
                pPrmData->edd_port_id        = pPndRQB->args.prm_write.edd_port_id;
                pPrmData->record_data_length = pMultipleSNMPRecord->recordLen/2;
                
                CValueHelper val;
                val.CharArrToHexArr(pPrmData->record_data, pMultipleSNMPRecord->recordData, pMultipleSNMPRecord->recordLen);

                PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                    "COHAUser::WriteParameter(); OHA Write Parameter, record index(%#x) mk_rema(%d) rqb(0x%08x)",
	                pRQB->args.PrmData.record_index,
                    pRQB->args.PrmData.mk_rema,
		            pRQB );

                eps_user_request_lower( pRQB, this, COHAUser::WriteParameterDone );
            }
            break;
        default:
            PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE_HIGH,
                "COHAUser::WriteParameter(); Wrong record index, record index(%#x) mk_rema(%d) rqb(0x%08x)",
	            pPndRQB->args.prm_write.record_index,
                pPndRQB->args.prm_write.mk_rema,
		        pPndRQB );
            break;
    }

    pnd_mem_free(pPndRQB);
}
/*----------------------------------------------------------------------------*/
/*  OHA callback functions                                                      */
/*----------------------------------------------------------------------------*/

void COHAUser::OpenChannelDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB)   == OHA_OPC_OPEN_CHANNEL);

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::OpenChannelDone(): Open OHA user channel done, rsp(%#x) PNIO-IFNr(%u) sys_path(%#x) handle(%u) rqb(%08x)", 
		rsp,
		pUser->m_pndHandle.pnio_if_nr, 
		pUser->m_sysPath,
		OHA_RQB_GET_HANDLE( pRQB ),
		pRQB );

	if ( rsp == OHA_OK )
	{
		pUser->m_pnioHandle = pRQB->args.channel.handle;

		// Provide event resources
		pUser->m_pEventHandler->StartListening();

		// continue with DCP Access Init (disable remote DCP settings
		pUser->DcpAccessInit();
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::CloseChannelDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB)   == OHA_OPC_CLOSE_CHANNEL);

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_04( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::CloseChannelDone(): Close OHA user channel done, rsp(%#x) PNIO-IFNr(%u) sys_path(%#x) rqb(%08x)", 
		rsp,
		pUser->m_pndHandle.pnio_if_nr, 
		pUser->m_sysPath,
		pRQB );

	if ( rsp == OHA_OK )
	{
		pUser->m_pnioHandle = OHA_INVALID_HANDLE;

		if ( pUser->m_isShutdown ) // shutdown in progress ?
		{
			// signal shutdown complete
			pUser->shutdownDone( PNIO_OK );
		}
	}
	else
	{
		// stop shutdown with fatal
		PND_FATAL( "COHAUser::CloseChannelDone(): unexpected OHA response received" );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::RemaReadDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_REMA_READ);

	PND_OHA_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::RemaReadDone(): OHA REMA READ done, rsp(%#x) dataTag(%u) dataSize(%u/%#x) rqb(%08x)", 
		OHA_RQB_GET_RESPONSE(pRQB),
		pRQB->args.RemaData.data_tag, 
		pRQB->args.RemaData.data_length,
		pRQB->args.RemaData.data_length,
		pRQB );

	if ( OHA_RQB_GET_RESPONSE( pRQB ) == OHA_ERR_BUFFER_OVERFLOW ) // READ buffer to small ?
	{
		// Retry READ with bigger buffer
		pUser->RemaRead( pRQB->args.RemaData.index, pRQB->args.RemaData.data_length );
	}
	else
	{
		// Store record data remanent for PNIO interface with
		// keys Types OHA_PDEV_DATA      PDev records 
		//            OHA_STATION_DATA   NoS, IP-Suite records
		//            OHA_SNMP_DATA      SNMP record (OHA_RECORD_INDEX_MULTIPLE_SNMP)

		PND_RQB_PTR_TYPE pPndRQB = pUser->GetRemaResource();

		if( !pPndRQB )
		{
			PND_OHA_TRACE_00( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE, "COHAUser::RemaReadDone(): No resource has been provided.");

            pUser->m_pEventHandler->RemaIndicationDone();
		}
		else
		{
            // UpdateRema function call
			LSA_RQB_SET_OPCODE( pPndRQB, PND_OPC_REMA_IND );
			LSA_RQB_SET_HANDLE( pPndRQB, 0 );

		    pPndRQB->args.rema_data.data_length = pRQB->args.RemaData.data_length;
		    pPndRQB->args.rema_data.data_tag    = pRQB->args.RemaData.data_tag;
		    pPndRQB->args.rema_data.index       = pRQB->args.RemaData.index;
		    pPndRQB->args.rema_data.pnio_err    = PNIO_OK;

			// "pPndRQB->args.rema_data.data" will be freed at iobc
		    pPndRQB->args.rema_data.data = (PNIO_UINT8*)pnd_mem_alloc( pPndRQB->args.rema_data.data_length );
		    pnd_memcpy(pPndRQB->args.rema_data.data, pRQB->args.RemaData.data, pRQB->args.RemaData.data_length);

			// direct call to pnd_pnio_user_core_request
		    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (LSA_VOID_PTR_TYPE)pPndRQB);

		    pnd_pnio_user_request_iobc_done( pPndRQB );
	    }
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::DcpAccessInitDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_DCP_CONTROL );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_06( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::DcpAccessInitDone(): OHA DCP Access Control done, rsp(%#x) mode(%#x) options(%#x) Type(%#x) SubType(%#x) rqb(%08x)",
		rsp,
		pRQB->args.DcpControl.Mode, 
		pRQB->args.DcpControl.Options,
		pRQB->args.DcpControl.Type,
		pRQB->args.DcpControl.SubType,
		pRQB );

	if ( rsp == OHA_OK )
	{
	    // write sysDescr in the MIB database
	    pUser->SetDcpRole();
    }
    else
    {
	    // stop startup with error
	    pUser->startupDone( PNIO_ERR_INTERNAL );        
    }

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetDcpRoleDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_DCP_WRITE_ELEMENT );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SetDcpRoleDone(); OHA Set DCP Role done , rsp(%#x) role(%#x) rqb(%08x)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.DcpElem.pVariable[0],
		pRQB );

	if ( rsp == OHA_OK )
	{
        // write station type in the MIB database
		pUser->SetDcpStationType();
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetDcpStationTypeDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_DCP_WRITE_ELEMENT );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SetDcpStationTypeDone(); OHA Set DCP Station Type done , rsp(%#x) rqb(%08x)",
		OHA_RQB_GET_RESPONSE( pRQB ),
		pRQB );

	if ( rsp == OHA_OK )
	{
		// Continue with set the LLDP MIB access for each port
		LSA_UINT16 nrOfPorts = pUser->m_ohaParam.nr_of_usr_ports;
		PND_ASSERT( nrOfPorts <= PSI_CFG_MAX_PORT_CNT );

		// start with first port
		pUser->LldpAccessInit( 1 );
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::LldpAccessInitDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_ACCESS_CONTROL);

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_06( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::LldpAccessInitDone(): OHA LLDP Access Init done, rsp(%#x) portID(%u) mode(%u) options(%#x) port_opt(%#x) rqb(%08x)", 
		OHA_RQB_GET_RESPONSE(pRQB),
		pRQB->args.LldpAccess.PortId,
		pRQB->args.LldpAccess.AccessMode,
		pRQB->args.LldpAccess.Options,
		pRQB->args.LldpAccess.PortOptions,
		pRQB );

	if ( rsp == OHA_OK )
	{
		// Continue with set the LLDP MIB access for each port
		LSA_UINT16 nrOfPorts = pUser->m_ohaParam.nr_of_usr_ports;
		PND_ASSERT( nrOfPorts <= PSI_CFG_MAX_PORT_CNT );
	
		if ( pRQB->args.LldpAccess.PortId < nrOfPorts )
		{
			// continue with next port
			pUser->LldpAccessInit( ( pRQB->args.LldpAccess.PortId +1) );
		}
		else
		{
            pUser->startupDone( PNIO_OK );
		}
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteSysNameDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_WRITE_OPTION );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_07( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteSysNameDone(): OHA SNMP Write sysName done, rsp(%#x) PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) StringLength(%u) rqb(0x%08x)",
		OHA_RQB_GET_RESPONSE(pRQB),
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );

	if ( rsp == OHA_OK )
	{
	    // write sysDescr in the MIB database
	    pUser->SNMPWriteSysDescr();
    }
    else
    {
	    // stop startup with error
	    pUser->startupDone( PNIO_ERR_INTERNAL );        
    }

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteSysDescrDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_WRITE_OPTION );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_07( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteSysDescrDone(): OHA SNMP Write sysDescr done, rsp(%#x) PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) StringLength(%u) rqb(0x%08x)",
		OHA_RQB_GET_RESPONSE(pRQB),
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );

	if ( rsp == OHA_OK )
	{
	    // write ifDescr in the MIB database
		LSA_UINT16 nrOfPorts = pUser->m_ohaParam.nr_of_usr_ports;
		PND_ASSERT( nrOfPorts <= PSI_CFG_MAX_PORT_CNT );

		// start with the first port
	    pUser->SNMPWriteIfDescr(0);
    }
    else
    {
	    // stop startup with error
	    pUser->startupDone( PNIO_ERR_INTERNAL );        
    }

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SNMPWriteIfDescrDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT(OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_LLDP_WRITE_OPTION);

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

	PND_OHA_TRACE_07( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::SNMPWriteIfDescrDone(): OHA SNMP Write ifDescr done, rsp(%#x) PortId(%u) Type(%u) SubType(%u) OUI(0x%08x) StringLength(%u) rqb(0x%08x)",
		OHA_RQB_GET_RESPONSE(pRQB),
		pRQB->args.LldpOption.PortId,
		pRQB->args.LldpOption.Type,
		pRQB->args.LldpOption.SubType,
        pRQB->args.LldpOption.OUI,
        pRQB->args.LldpOption.Length,
		pRQB );


	if ( rsp == OHA_OK )
	{
		// Continue with set the LLDP MIB access for each port
		LSA_UINT16 nrOfPorts = pUser->m_ohaParam.nr_of_usr_ports;
		PND_ASSERT( nrOfPorts <= PSI_CFG_MAX_PORT_CNT );
	
		if ( pRQB->args.LldpOption.PortId < nrOfPorts )
		{
			// continue with next port
			pUser->SNMPWriteIfDescr( ( pRQB->args.LldpOption.PortId + 1) );
		}
		else
		{
			// finish startup with ok
			pUser->startupDone( PNIO_OK );
		}
	}
	else
	{
		// stop startup with error
		pUser->startupDone( PNIO_ERR_INTERNAL );
	}

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetAddressDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_SET_ADDRESS );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

    PND_OHA_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "COHAUser::SetAddressDone(): OHA SET ADDRESS done, rsp(%#x) accessMode(%#x) options(%#x) userLockStatus(%#x) rqb(0x%08x)", 
		rsp,
		pRQB->args.Address.AccessMode, 
		pRQB->args.Address.Options,
		pRQB->args.Address.UserLockStatus,
		pRQB );

	// restore PND RQB from user ID for done handling
    PND_RQB_PTR_TYPE pPndRQB = (PND_RQB_PTR_TYPE)LSA_RQB_GET_USER_ID_PTR( pRQB );
    PND_ASSERT( pPndRQB != 0 );

	PND_SET_IP_AND_NOS_TYPE *pIpAndNosType = &pPndRQB->args.set_ip_and_nos;
    OHA_ADDRESS_TYPE* pAddressType = &pRQB->args.Address;


	if( rsp == CM_OK )
	{
		pIpAndNosType->pnio_err = PNIO_OK;
	}
	else
	{
		if( rsp == CM_ERR_PARAM )
		{
            if( (pAddressType->Options & OHA_CTRL_IP_SUITE) == OHA_CTRL_IP_SUITE )
			{
                pIpAndNosType->pnio_err = PNIO_ERR_PRM_IP;
			}
            
            if( (pAddressType->Options & OHA_CTRL_STATION_NAME) == OHA_CTRL_STATION_NAME )
			{
                pIpAndNosType->pnio_err = PNIO_ERR_PRM_NOS;
			}
		}
	}

	// pStationData+pStationName are also freed here
	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );

	pnd_pnio_user_request_iobc_done( pPndRQB );
}

/*----------------------------------------------------------------------------*/
void COHAUser::WriteParameterDone( void* p_inst, void* p_rb)
{
	COHAUser *pUser             = static_cast<COHAUser*>(p_inst);
	OHA_UPPER_RQB_PTR_TYPE pRQB = static_cast<OHA_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pUser != 0);
	PND_ASSERT( pRQB   != 0);
	PND_ASSERT( OHA_RQB_GET_OPCODE(pRQB) == OHA_OPC_PRM_WRITE );

	LSA_RESPONSE_TYPE rsp = OHA_RQB_GET_RESPONSE(pRQB);

    PND_OHA_TRACE_07( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "COHAUser::WriteParameterDone(): OHA PRM WRITE done, rsp(%#x) edd_port_id(%d) mk_rema(%d) record_index(%#x) slot_number(%#x) subslot_number(%#x) rqb(0x%08x)", 
		rsp,
		pRQB->args.PrmData.edd_port_id, 
		pRQB->args.PrmData.mk_rema,
		pRQB->args.PrmData.record_index,
        pRQB->args.PrmData.slot_number,
        pRQB->args.PrmData.subslot_number,
		pRQB );

	pnd_oha_free_rqb( pRQB, &pUser->m_pndHandle );
}
/*----------------------------------------------------------------------------*/
LSA_VOID COHAUser::RemaProvide( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(p_rb);

   SetRemaResource(p_rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID COHAUser::RemaReProvide( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(p_rb);

   SetRemaResource(p_rb);

   m_pEventHandler->RemaIndicationDone();
}

/*----------------------------------------------------------------------------*/
LSA_VOID COHAUser::RemaReleaseResources()
{
	PND_RQB_PTR_TYPE pRemaRqb = 0;

	while ((pRemaRqb = GetRemaResource()) != 0)
	{
		LSA_RQB_SET_RESPONSE(pRemaRqb, PNIO_OK_CANCEL);
      
		pnd_pnio_user_request_iobc_done( pRemaRqb );
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID COHAUser::GetStationType(LSA_UINT8* pBuffer)
{
    PND_ASSERT(pBuffer);
	pnd_memcpy(pBuffer, m_StationType, sizeof(m_StationType));
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL COHAUser::GetIpAddrValidationState()
{
	return m_ohaParam.IPAddrValAllowed;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL COHAUser::GetNameOfStationValidationState()
{
	return m_ohaParam.NameOfStationValAllowed;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
LSA_VOID COHAUser::GetInterfaceData( LSA_UINT32* pIp, LSA_UINT32* pMask, LSA_UINT32* pGwIp, LSA_UINT8* pName )
{
	CValueHelper valObj;
	PNIO_UINT16 strLength;

    PND_ASSERT( pName );

    valObj.String2IP( m_ipAddr, pIp );
	valObj.String2IP( m_subnetMask, pMask );
	valObj.String2IP( m_gatewayIP, pGwIp );

	strLength = (LSA_UINT16) pnd_strlen( m_nameOfStation.c_str() );
	if( valObj.IsSizeValid( m_nameOfStation, 256 ) )
	{
		pnd_memcpy(pName, (LSA_VOID *)m_nameOfStation.c_str(), strLength ) ;		
	}
	pName[strLength] = LSA_NULL; // Terminating character
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void COHAUser::SetInterfaceData( LSA_UINT8* pIp, LSA_BOOL ipDup, LSA_UINT8* pName, LSA_UINT16 size, LSA_BOOL nosDup )
{
	CValueHelper valObj;

	// Init Bad states with duplicate states
	LSA_BOOL nosBad = nosDup;
	LSA_BOOL ipBad  = ipDup;

	// Note: updated if a "OHA ADDR INFO.IND" is received
	m_ipAddr        = valObj.IP2String( pIp );

	if ( m_ipAddr.compare( "0.0.0.0") == 0 ) // Not Valid IP
	{
		ipBad = LSA_TRUE;
	}

	if ( size == 0 ) // Empty name ?
	{
		// Mark NoS Bad in BGZD
		nosBad = LSA_TRUE;
		m_nameOfStation = "";
	}
	else
	{
		PND_ASSERT( pName != 0 );

		m_nameOfStation = valObj.Buffer2String( pName, size );
	}

	// Update the BGZD states for IP, NoS
	m_bgzdStore.SetBadIP( ipBad );
	m_bgzdStore.SetBadNoS( nosBad );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void COHAUser::SetInterfaceData( LSA_UINT8* pIp, LSA_UINT8* pMask, LSA_UINT8* pGwIp, LSA_BOOL ipDup, LSA_UINT8* pName, LSA_UINT16 size, LSA_BOOL nosDup )
{
	CValueHelper valObj;

	// Init Bad states with duplicate states
	LSA_BOOL nosBad = nosDup;
	LSA_BOOL ipBad  = ipDup;

	// Note: updated if a "OHA ADDR INFO.IND" is received
	m_ipAddr        = valObj.IP2String( pIp );
	m_subnetMask    = valObj.IP2String( pMask );
	m_gatewayIP     = valObj.IP2String( pGwIp );

	if ( m_ipAddr.compare( "0.0.0.0") == 0 ) // Not Valid IP
	{
		ipBad = LSA_TRUE;
	}

	if ( size == 0 ) // Empty name ?
	{
		// Mark NoS Bad in BGZD
		nosBad = LSA_TRUE;
		m_nameOfStation = "";
	}
	else
	{
		PND_ASSERT( pName != 0 );

		m_nameOfStation = valObj.Buffer2String( pName, size );
	}

	// Update the BGZD states for IP, NoS
	m_bgzdStore.SetBadIP( ipBad );
	m_bgzdStore.SetBadNoS( nosBad );
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
 
void COHAUser::startupDone( LSA_UINT32 pnioErr )
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	// Signal startup complete
	m_isStarted   = LSA_TRUE;
	m_isStartup   = LSA_FALSE;
	m_responseRQB = 0;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDUser::startupDone(): PD User startup complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
		m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

	pRQB->args.pnuc_internal.pnio_err = pnioErr;

	pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAUser::shutdownDone( LSA_UINT32 pnioErr )
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStarted   = LSA_FALSE;
	m_isStartup   = LSA_FALSE;
	m_isShutdown  = LSA_FALSE;
	m_responseRQB = 0;

	PND_OHA_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"COHAUser::shutdownDone() OHA User shutdown complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
		m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

	// Signal shutdown complete
	pRQB->args.pnuc_internal.pnio_err = pnioErr;

	pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
void COHAUser::SetRemaResource(PND_RQB_PTR_TYPE pUser)
{
   PND_ASSERT(pUser != 0);

   m_remaResources.push_back(pUser);
}


/*----------------------------------------------------------------------------*/
PND_RQB_PTR_TYPE COHAUser::GetRemaResource()
{
   PND_RQB_PTR_TYPE pUser = 0;

   if (!m_remaResources.empty())
   {
      pUser = *m_remaResources.begin();

      m_remaResources.erase(m_remaResources.begin());
   }

   return pUser;
}
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
