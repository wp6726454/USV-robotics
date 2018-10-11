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
/*  F i l e               &F: pnd_ValueHelper.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation PnDriver value helping class (conversion, value checking) */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3104
#define PND_MODULE_ID       3104

/////////////////////////////////////////////////////////////////////////////

/* EPS-framework */
#include "pnd_int.h"
#include "pnd_sys.h"
#include "clrpc_inc.h"           // for UUID conversion

/* StdLib++*/
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "pnd_ValueHelper.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */


/////////////////////////////////////////////////////////////////////////////
// CValueHelper: Checks Value for Range, Format and conversion

CValueHelper::CValueHelper()
{
	m_lastError = "";
}

/*----------------------------------------------------------------------------*/
CValueHelper::~CValueHelper()
{
}

/*----------------------------------------------------------------------------*/
string CValueHelper::GetError()
{
	return ( ( m_lastError + '\n') );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsMinValid( string value, LSA_INT minValue )
{
	LSA_INT  converted;
	LSA_BOOL result = LSA_TRUE;

	m_lastError = "";

	if ((!String2Int( value, &converted )) ||  // converted ?
		( converted < minValue ))                   // in range ?
	{
		ostringstream resStr;
		resStr << "Value(" << value << ") is invalid, must be at least " << minValue << "!";
		m_lastError = resStr.str();

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsRangeValid( string value, LSA_INT minValue, LSA_INT maxValue )
{
	LSA_INT  converted;
	LSA_BOOL result = LSA_TRUE;

	m_lastError = "";

	if ((!String2Int( value, &converted )) ||        // converted ?
		( converted < minValue) || ( converted > maxValue ))   // in range ?
	{
		ostringstream resStr;
		resStr << "Value(" << value << ") is invalid, must be between " << minValue << " and " << maxValue << "!";
		m_lastError = resStr.str();

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsSizeValid( string value, LSA_INT maxSize )
{
	PND_ASSERT ( maxSize > 0 );

	LSA_BOOL result = LSA_TRUE;
	m_lastError = "";

	if ( (LSA_INT)value.size() > maxSize )
	{
		ostringstream resStr;
		resStr << "FieldSize(" << value.size() << ") is too long, must be less than " << maxSize << "!";
		m_lastError = resStr.str();

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsMACValid( string value )
{
	LSA_INT  mac[6];
	LSA_INT  nr;
	LSA_BOOL result = LSA_TRUE;

	m_lastError = "";

	LSA_CHAR *pBuf = (LSA_CHAR*)value.c_str();

	if (( value.size() != 17 ) ||                                                                                  // valid size ?
		( sscanf(pBuf,"%2x-%2x-%2x-%2x-%2x-%2x%n", &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5], &nr) != 6 ) || // valid format ?
		( nr != 17 ))                                                                                              // all converted ?
	{
		m_lastError = "Value(" +  value + ") for MAC-Address is not valid!";

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsUUIDValid( string value )
{
	CLRPC_UUID_TYPE tmpUUID;
	LSA_BOOL        result = LSA_TRUE;

	m_lastError = "";

	if (( value.size() != 36 ) ||                                                      // invalid size ?
		( clrpc_uuid_from_string( &tmpUUID, (LSA_UINT8*)value.c_str() ) != CLRPC_OK )) // invalid format ?
	{
		m_lastError = "Value(" + value + ") for UUID is not valid!";

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsIPValid( string value )
{
	LSA_UINT32 ipValue;

	LSA_BOOL result = LSA_TRUE;
	m_lastError = "";

	if (( value.empty()) ||               // Empty ?
		(!String2IP(value, &ipValue)) ||  // Invalid IP ?
		( ipValue == 0 ))                 // IP 0.0.0.0 ?
	{
		m_lastError = "Value(" +  value + ") for IP-Address is not valid!";

		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsIPValidNullAllowed( string value )
{
	LSA_UINT32 ipValue;
	LSA_BOOL   result = LSA_TRUE;

	m_lastError = "";

	if (( value.empty()) ||             // Empty ?
		(!String2IP(value, &ipValue)))  // Invalid IP ?
	{
		m_lastError = "Value(" +  value + ") for IP-Address is not valid!";

		result = LSA_FALSE;
	}
	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsBoardIdValid( string value )
{
	LSA_BOOL result = LSA_TRUE;
	m_lastError = "";

	PSI_HD_ID_TYPE tmpID;

	// Try to convert the string value from a known CP ID format
	if ( !String2BoardID( value, &tmpID )) // convert ok ? 
	{
		m_lastError = "Value(" + value + ") is not valid\n" +
			"Supported formats:\n" +
			"Board PCI location [<Bus Number>,<Device Number>,<Function Number>]\n" +
			"Board MAC [xx-xx-xx-xx-xx-xx]\n";
		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::IsNotEmptyMAC( LSA_UINT8 *pValue )
{
	LSA_BOOL result = LSA_TRUE;

	PND_ASSERT( pValue != 0 );

	m_lastError = "";

	// Check if MAC is not empty (MAC check for MAC addresses delivered from Driver)
	if (( pValue[0] == 0 ) &&  // empty MAC = 00-00-00-00-00-00
		( pValue[1] == 0 ) && 
		( pValue[2] == 0 ) &&
		( pValue[3] == 0 ) && 
		( pValue[4] == 0 ) &&
		( pValue[5] == 0 ))
	{
		m_lastError = "Value has empty MAC, value(00-00-00-00-00-00)";
		result = LSA_FALSE;
	}

	if (( pValue[0] == 0xFF ) &&  // not set MAC = FF-FF-FF-FF-FF-FF
		( pValue[1] == 0xFF ) && 
		( pValue[2] == 0xFF ) &&
		( pValue[3] == 0xFF ) && 
		( pValue[4] == 0xFF ) &&
		( pValue[5] == 0xFF ))
	{
		m_lastError = "Value has invalid (not set) MAC, value(FF-FF-FF-FF-FF-FF)";
		result = LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL CValueHelper::String2Int( string value, LSA_INT *pIntVal )
{
	LSA_INT convVal, nr;

	LSA_BOOL result = LSA_FALSE;
	LSA_INT strSize = value.size();

	if (( strSize > 0 ) &&                                          // empty string ?
		( sscanf( value.c_str(), "%i%n", &convVal, &nr ) == 1 ) &&  // converted ?
		( nr == strSize ))                                          // All chars handled ? 
	{
		*pIntVal = convVal;
		result = LSA_TRUE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2UINT32( string value, LSA_UINT32 *pVal )
{
	LSA_INT convVal = 0;
	LSA_BOOL result = LSA_FALSE;
    LSA_INT maxValue = 0xFFFFFFFF;

	if (( value[0] != '-' ) &&
		( String2Int( value, &convVal )))
	{
		result = ( convVal <= maxValue ) ? LSA_TRUE : LSA_FALSE;
		*pVal  = (LSA_UINT32)convVal;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2UINT16( string value, LSA_UINT16 *pVal )
{
	LSA_INT convVal = 0;
	LSA_BOOL result = LSA_FALSE;

	if (( value[0] != '-' ) &&
		( String2Int( value, &convVal )))
	{
		result = ( convVal <= 0xFFFF ) ? LSA_TRUE : LSA_FALSE;
		*pVal  = (LSA_UINT16)convVal;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2UINT8( string value, LSA_UINT8 *pVal )
{
	LSA_INT convVal = 0;
	LSA_BOOL result = LSA_FALSE;

	if (( value[0] != '-' ) && 
		( String2Int( value, &convVal )))
	{
		result = ( convVal <= 0xFF ) ? LSA_TRUE : LSA_FALSE;
		*pVal  = (LSA_UINT8)convVal;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2BOOL( string value, LSA_BOOL *pVal )
{
	LSA_INT convVal = 0;
	LSA_BOOL result = LSA_FALSE;

	if ( String2Int( value, &convVal ))
	{
		result = (( convVal == 0 ) || ( convVal == 1 )) ? LSA_TRUE : LSA_FALSE;
		*pVal  = ( convVal == 1 ) ? LSA_TRUE : LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2Location( string cpID, PSI_PCI_LOCATION_PTR_TYPE pLocId )
{
	LSA_INT busNr, devNr, funcNr, n;

	LSA_BOOL result = LSA_FALSE;

	if ((!cpID.empty()) &&                                                       // Format size ?
		(sscanf( cpID.c_str(), "%i,%i,%i%n", &busNr,&devNr,&funcNr, &n) == 3) && // all set ?
		(n == (LSA_INT)cpID.size()))                                             // all converted ?
	{
		pLocId->bus_nr      = (LSA_UINT16)busNr;
		pLocId->device_nr   = (LSA_UINT16)devNr;
		pLocId->function_nr = (LSA_UINT16)funcNr;

		result = LSA_TRUE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2MAC( string macStr, LSA_UINT8 *pMac )
{
	LSA_BOOL result = LSA_FALSE;

	PND_ASSERT ( pMac != 0 );

	if ( IsMACValid ( macStr ))  //  // MAC format valid ?
	{
		LSA_INT mac0, mac1, mac2, mac3, mac4, mac5;

		LSA_CHAR* pBuf = (LSA_CHAR*)macStr.c_str();

		if ( sscanf( pBuf, "%2x-%2x-%2x-%2x-%2x-%2x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5) == 6 ) 
		{
			pMac[0] = (LSA_UINT8)(mac0 & 0xFF);
			pMac[1] = (LSA_UINT8)(mac1 & 0xFF);
			pMac[2] = (LSA_UINT8)(mac2 & 0xFF);
			pMac[3] = (LSA_UINT8)(mac3 & 0xFF);
			pMac[4] = (LSA_UINT8)(mac4 & 0xFF);
			pMac[5] = (LSA_UINT8)(mac5 & 0xFF);

			result = LSA_TRUE;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2IP( string ipStr, LSA_UINT32* pIpVal )
{
	LSA_BOOL result = LSA_FALSE;

	PND_ASSERT ( pIpVal != 0 );

	if ( !ipStr.empty()) // Format size ?
	{
		LSA_UINT ip1, ip2, ip3, ip4, n;

		LSA_CHAR* pBuf = (LSA_CHAR*)ipStr.c_str();

		if (( sscanf( pBuf, "%u.%u.%u.%u%n", &ip1, &ip2, &ip3, &ip4, &n) == 4 ) && // Format ok ?
			( n == strlen( pBuf )))                                                // Complete ?
		{
			if( (ip1 <= 255) && (ip2 <= 255) && (ip3 <= 255) && (ip4 <= 255 )) // in range
			{
				LSA_UINT32 x;

				x   = ip1;
				x <<= 8;
				x  |= ip2;
				x <<= 8;
				x  |= ip3;
				x <<= 8;
				x  |= ip4;

				*pIpVal = x;
				result = LSA_TRUE;
			}
		}
	}

	return( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2UUID( string uuidStr, LSA_VOID* pUUID )
{
	LSA_BOOL result = LSA_FALSE;

	PND_ASSERT ( pUUID != 0 );

	if ((!uuidStr.empty())  &&     // Format size ?
		( uuidStr.size() == 36 ))
	{
		// try to convert it to a CLRPC UUID
		LSA_UINT16 ret = clrpc_uuid_from_string( (CLRPC_UUID_PTR_TYPE)pUUID, (LSA_UINT8*)uuidStr.c_str() );

		result = ( ret == CLRPC_OK ) ? LSA_TRUE : LSA_FALSE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2BoardID( string value, PSI_HD_ID_PTR_TYPE pID )
{
	PSI_PCI_LOCATION_TYPE pciLoc;
	PSI_MAC_TYPE          mac;

	LSA_BOOL result = LSA_FALSE;

	PND_ASSERT ( pID != 0 );

	// Initialize the possible keys
	pID->hd_selection = PSI_HD_SELECT_WITH_UNKNOWN;

	pnd_memset( &pID->hd_mac,      0, sizeof( pID->hd_mac) );
	pnd_memset( &pID->hd_location, 0, sizeof( pID->hd_location) );

	// try to convert in from known formatsin order
	if ( String2Location( value, &pciLoc )) // valid PCI location ?
	{
		pID->hd_selection            = PSI_HD_SELECT_WITH_LOCATION;
		pID->hd_location.bus_nr      = pciLoc.bus_nr;
		pID->hd_location.device_nr   = pciLoc.device_nr;
		pID->hd_location.function_nr = pciLoc.function_nr;
		result = LSA_TRUE;
	}
	else
	{
		if ( String2MAC( value, mac )) // valid MAC ?
		{
			pID->hd_selection = PSI_HD_SELECT_WITH_MAC;
			pnd_memcpy( &pID->hd_mac, &mac, sizeof( pID->hd_mac ) );
			result = LSA_TRUE;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2EddCompID( string value, LSA_UINT16* pEddCompID )
{
	LSA_BOOL result     = LSA_FALSE;
	LSA_INT  eddIniType = -1;

	*pEddCompID = 0;  // set to invalid EDDx component ID

	if ( String2Int( value, &eddIniType ))  // Valid Int string
	{
		switch ( eddIniType )  // EDD Type value from INI File
		{
		case 1: // EDDI
			{
				*pEddCompID = LSA_COMP_ID_EDDI;
				result = LSA_TRUE;
			}
			break;
		case 2: // EDDP
			{
				*pEddCompID = LSA_COMP_ID_EDDP;
				result = LSA_TRUE;
			}
			break;
		case 3: // EDDS
			{
				*pEddCompID = LSA_COMP_ID_EDDS;
				result = LSA_TRUE;
			}
			break;
		default:  // unknown
			{
				*pEddCompID = 0;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2PortMediaType( string value, PSI_PORT_MEDIA_TYPE *pMediaType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pMediaType = PSI_MEDIA_TYPE_UNKNOWN;  // set to default unknown

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // PSI Media type value from INI File
		{
		case 1: // copper
			{
				*pMediaType = PSI_MEDIA_TYPE_COPPER;
				result = LSA_TRUE;
			}
			break;
		case 2: // fibre
			{
				*pMediaType = PSI_MEDIA_TYPE_FIBER;
				result = LSA_TRUE;
			}
			break;
		case 3: // wireless
			{
				*pMediaType = PSI_MEDIA_TYPE_WIRELESS;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pMediaType = PSI_MEDIA_TYPE_UNKNOWN;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2RingPortType( string value, PSI_RINGPORT_TYPE *pRingPortType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pRingPortType = PSI_NO_RING_PORT;  // set default to none

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // Ringport value from INI File
		{
		case 0: // no ring port
			{
				*pRingPortType = PSI_NO_RING_PORT;
				result = LSA_TRUE;
			}
			break;
		case 1: // possible ring port but no default
			{
				*pRingPortType = PSI_RING_PORT;
				result = LSA_TRUE;
			}
			break;
		case 2: // default ring port
			{
				*pRingPortType = PSI_RING_PORT_DEFAULT;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pRingPortType = PSI_NO_RING_PORT;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2MrpRoleType( string value, PSI_MRP_ROLE_TYPE *pRoleType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pRoleType = PSI_MRP_ROLE_NONE;  // set default to none

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // MRP role value from INI File
		{
		case 0: // MRP role OFF
			{
				*pRoleType = PSI_MRP_ROLE_NONE;
				result = LSA_TRUE;
			}
			break;
		case 1: // MRP role client
			{
				*pRoleType = PSI_MRP_ROLE_CLIENT;
				result = LSA_TRUE;
			}
			break;
		case 2: // MRP role manager
			{
				*pRoleType = PSI_MRP_ROLE_MANAGER;
				result = LSA_TRUE;
			}
			break;
		case 3: // MRP role auto manager
			{
				*pRoleType = PSI_MRP_ROLE_AUTO_MANAGER;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pRoleType = PSI_MRP_ROLE_NONE;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2LdRunsType( string value, PSI_LD_RUNS_ON_TYPE *pLdRunsType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pLdRunsType = PSI_LD_RUNS_ON_UNKNOWN;  // set default unknown

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // LD runs on value from INI File
		{
		case 1: // LD runs on App level (i.e pcIOX)
			{
				*pLdRunsType = PSI_LD_RUNS_ON_LIGHT;
				result = LSA_TRUE;
			}
			break;
		case 2: // LD runs on advanced level (i.e high performance system)
			{
				*pLdRunsType = PSI_LD_RUNS_ON_ADVANCED;
				result = LSA_TRUE;
			}
			break;
		case 3: // LD runs on basic level (i.e as FW system on Pn board)
			{
				*pLdRunsType = PSI_LD_RUNS_ON_BASIC;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pLdRunsType = PSI_LD_RUNS_ON_UNKNOWN;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2EddType( string value, LSA_UINT16 *pEddType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pEddType = LSA_COMP_ID_UNKNOWN;  // set default unknown

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // EDD type value from INI File
		{
		case 1: // EDDI
			{
				*pEddType = LSA_COMP_ID_EDDI;
				result = LSA_TRUE;
			}
			break;
		case 2: // EDDP
			{
				*pEddType = LSA_COMP_ID_EDDP;
				result = LSA_TRUE;
			}
			break;
		case 3: // EDDS
			{
				*pEddType = LSA_COMP_ID_EDDS;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pEddType = LSA_COMP_ID_UNKNOWN;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2AsicType( string value, LSA_UINT16 *pAsicType )
{
	LSA_BOOL result = LSA_FALSE;
	LSA_INT iniType = -1;

	*pAsicType = PSI_ASIC_TYPE_UNKNOWN;  // set default unknown

	if ( String2Int( value, &iniType ))  // Valid Int string
	{
		switch ( iniType )  // EDD type value from INI File
		{
		case 0: // StdMAC (don't care)
			{
				*pAsicType = PSI_ASIC_TYPE_UNKNOWN;  // set default unknown
				result = LSA_TRUE;
			}
			break;

		case 1: // ERTEC400
			{
				*pAsicType = PSI_ASIC_TYPE_ERTEC400;
				result = LSA_TRUE;
			}
			break;
		case 2: // ERTEC200
			{
				*pAsicType = PSI_ASIC_TYPE_ERTEC200;
				result = LSA_TRUE;
			}
			break;
		case 3: // SOC
			{
				*pAsicType = PSI_ASIC_TYPE_SOC1;
				result = LSA_TRUE;
			}
			break;
		case 4: // ERTEC200+
			{
				*pAsicType = PSI_ASIC_TYPE_ERTEC200P;
				result = LSA_TRUE;
			}
			break;
		default:  // invalid
			{
				*pAsicType = PSI_ASIC_TYPE_UNKNOWN;
				result = LSA_FALSE;
			}
			break;
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL CValueHelper::String2ImSoftwareVersion( string value, PSI_IM_SW_VERSION_PTR_TYPE pImID )
{
	LSA_INT funcNr, bugFixNr, changeNr, n;
	LSA_CHAR prefix;

	LSA_BOOL result = LSA_FALSE;

	PND_ASSERT( pImID != 0 );

	if ((!value.empty()) &&                                                                        // Format size ?
		(sscanf( value.c_str(), "%c%i.%i.%i%n", &prefix, &funcNr,&bugFixNr,&changeNr, &n) == 4) && // all set ?
		(n == (LSA_INT)value.size()))                                                              // all converted ?
	{
		pImID->revision_prefix        = (LSA_UINT8)prefix;
		pImID->functional_enhancement = (LSA_UINT8)funcNr;
		pImID->bug_fix                = (LSA_UINT8)bugFixNr;
		pImID->internal_change        = (LSA_UINT8)changeNr;

		result = LSA_TRUE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CValueHelper::UINT32ToString( LSA_UINT32 val )
{
	ostringstream resStr;
	resStr << val;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::UINT16ToString( LSA_UINT16 val )
{
	ostringstream resStr;
	resStr << val;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::UINT8ToString( LSA_UINT8 val )
{
	LSA_UINT16 outVal = val;

	ostringstream resStr;
	resStr << outVal;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::INT32ToString( LSA_INT32 val )
{
	ostringstream resStr;
	resStr << val;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::INT16ToString( LSA_INT16 val )
{
	ostringstream resStr;
	resStr << val;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::INT8ToString( LSA_INT8 val )
{
	LSA_INT16 outVal = val;

	ostringstream resStr;
	resStr << outVal;

	return ( resStr.str() );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::BOOLToString( LSA_BOOL val )
{
	string result = "invalid";
	
	switch ( val )
	{
	case LSA_FALSE:
		result = "false";
		break;
	case LSA_TRUE:
		result = "true";
		break;
	default:
		break;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CValueHelper::MAC2String( LSA_UINT8* pMac )
{
	LSA_CHAR tmpBuf[20];

	PND_ASSERT( pMac != 0 );

	sprintf( tmpBuf, "%02x-%02x-%02x-%02x-%02x-%02x",
		pMac[0],
		pMac[1],
		pMac[2],
		pMac[3],
		pMac[4],
		pMac[5] );

	string result = tmpBuf;

	return ( result );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::Buffer2String( LSA_UINT8* pBuffer, LSA_UINT16 bufferSize )
{
	string result = "";

	if ( bufferSize > 0 )
	{
		PND_ASSERT( pBuffer != 0 );

		string tmp( (const char*)pBuffer, bufferSize );

		result = tmp;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::IP2String( LSA_UINT32 ip )
{
	LSA_UINT16 ipVal[4];
	ipVal[0] = ((ip >> 24) & 0xFF);
	ipVal[1] = ((ip >> 16) & 0xFF);
	ipVal[2] = ((ip >> 8)  & 0xFF);
	ipVal[3] = ( ip & 0xFF);

	ostringstream resStr;
	resStr << ipVal[0] << "." << ipVal[1] << "." << ipVal[2] << "."  << ipVal[3];

	string result = resStr.str();

	return ( result );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::IP2String( LSA_UINT8* pIP )
{
	PND_ASSERT( pIP != 0 );

	ostringstream resStr;
	resStr << (LSA_UINT16)pIP[0] << "." << (LSA_UINT16)pIP[1] << "." << (LSA_UINT16)pIP[2] << "."  << (LSA_UINT16)pIP[3];

	string result = resStr.str();

	return( result );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CValueHelper::PndHandle2SysPtrPSI( PND_HANDLE_PTR_TYPE cpHandle, LSA_VOID* psiSysPtr )
{
	PND_ASSERT( cpHandle != 0 );
	PND_ASSERT( psiSysPtr != 0 );

	PSI_SYS_PTR_TYPE pSYS = (PSI_SYS_PTR_TYPE)psiSysPtr;

	// Prepare the HD/IF Keys for PSI addressing only
	pSYS->hd_nr       = cpHandle->hd_nr;
	pSYS->if_nr       = cpHandle->if_nr;
	pSYS->pnio_if_nr  = cpHandle->pnio_if_nr;
	pSYS->comp_id_edd = cpHandle->edd_comp_id;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_BOOL CValueHelper::SplitObjectID( string objID, LSA_UINT16 *pInst, LSA_UINT16 *pDevId, LSA_UINT16 *pVenId )
{
	LSA_INT inst, devId, venId, n;
	LSA_BOOL result = LSA_FALSE;

	if ((objID.size() == 14) &&                                                    // Format size ?
		(sscanf( objID.c_str(), "%4x.%4x.%4x%n", &inst,&devId,&venId, &n) == 3) && // all set ?
		(n == 14))                                                                 // all converted ?
	{
		*pInst  = (LSA_UINT16)inst;
		*pDevId = (LSA_UINT16)devId;
		*pVenId = (LSA_UINT16)venId;
		result = LSA_TRUE;
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
string CValueHelper::GetLeftSubString( string srcStr, char seperator )
{
	string result = "";

	if ( !srcStr.empty())
	{
		LSA_INT foundIdx = srcStr.find( seperator );

		if ( foundIdx != -1 )
		{
			result = srcStr.substr( 0, foundIdx ); 
		}
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
#if 0

int CValueHelper::GetByteCount( string argVal )
{
	LSA_INT result = 0;
	LSA_CHAR *sep = " \t,-"; // Possible whitespaces between byte strings

	LSA_CHAR *pBuf = (LSA_CHAR*)argVal.c_str();
	LSA_CHAR *tok  = strtok(pBuf, sep);

	while ( tok )
	{
		result++;
		tok = strtok( 0, sep );
	}

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 CValueHelper::GetBytes( string argVal, LSA_UINT8 **pMem )
{
	LSA_UINT16 size = 0;

	if ( !argVal.empty())
	{
		// Get size of mem block
		size = (LSA_UINT16)GetByteCount( argVal );

		if ( size > 0 )
		{
			LSA_INT idx = 0;
			LSA_UINT8* pAlloc = (LSA_UINT8*)pnd_mem_alloc( size );
			PND_ASSERT( pAlloc != 0 );

			*pMem = pAlloc;

			// Split String to byte values
			LSA_CHAR *sep = " \t,;-";

			LSA_CHAR *pBuf = (LSA_CHAR*)argVal.c_str();
			LSA_CHAR *tok  = strtok (pBuf, sep);

			while ( tok ) 
			{
				LSA_INT memVal;

				if ( !String2Int( tok, &memVal) ) // converting ok ?
				{
					pnd_mem_free( pAlloc );
					*pMem = 0;
					size  = 0;
					break;
				}

				pAlloc[idx] = (LSA_UINT8)memVal;
				idx += 1;
				tok = strtok (0, sep);
			}
		}
	}

	return ( size );
}

#endif //0 
/*----------------------------------------------------------------------------*/
string CValueHelper::GetJoinedString( stringVector& srcList, string seperator )
{
	LSA_INT count = srcList.size();

	string result = (count > 0 ) ? srcList[0] : "";

	for ( LSA_INT idx = 1; idx < count; idx++ )
	{
		result += seperator;
		result += srcList[idx];
	}

	return ( result );
}

PNIO_UINT8* CValueHelper::HexArrToCharArr(PNIO_UINT8* src, PNIO_UINT32 dataLength)
{
  PNIO_UINT8* dst = (PNIO_UINT8*)pnd_mem_alloc(2*dataLength + 1);

  for (PNIO_UINT32 ind = 0; ind<2*dataLength; ind = ind+2)
  {
	  char char1, char2;
	  char1 = (src[ind/2]>>4) & 0x0F;
	  char2 = (src[ind/2])    & 0x0F;

	  if(char1 > 9)
	  {
		  dst[ind] = char1-10 + 'A';
	  }
	  else
	  {
		  dst[ind] = char1 + '0';
	  }

	  if(char2 > 9)
	  {
		  dst[ind+1] = char2-10 + 'A';
	  }
	  else
	  {
		  dst[ind+1] = char2 + '0';
	  }

  }
  dst[2*dataLength] = LSA_NULL;
  return dst;
}

LSA_VOID CValueHelper::CharArrToHexArr( PNIO_UINT8* dst, PNIO_UINT8* src, PNIO_UINT32 dataLength)
{

  for (PNIO_UINT32 ind = 0; ind<dataLength; ind = ind+2)
  {
	  PNIO_UINT8 char1, char2;
	  
      char1 = src[ind];
      char2 = src[ind+1];
      
      if(char1 >= 'A' && char1 <= 'F')
      {
          char1 = ((char1 - 'A' + 10)<<4) & 0xF0;  
      }
      else if(char1 >= 'a' && char1 <= 'f')
      {
          char1 = ((char1 - 'a' + 10)<<4) & 0xF0;  
      }
      else if(char1 >= '0' && char1 <= '9')
      {
          char1 = ((char1 - '0')<<4) & 0xF0;  
      }

      if(char2 >= 'A' && char2 <= 'F')
      {
          char2 = (char2 - 'A' + 10) & 0x0F;  
      }
      else if(char2 >= 'a' && char2 <= 'f')
      {
          char2 = ((char2 - 'a' + 10)<<4) & 0xF0;  
      }
      else if(char2 >= '0' && char2 <= '9')
      {
          char2 = (char2 - '0') & 0x0F;  
      }

      dst[ind/2] = char1 + char2;
  }

  dst[dataLength/2] = LSA_NULL;
}

LSA_UINT32 CValueHelper::GetDigitCount(LSA_UINT32 number)
{
    LSA_UINT32 digitCount = 1;

    while(true)
    {
        number = number/10;
        if(number == 0)
        {
            break;
        }
        digitCount++;
    }

    return digitCount;    
}
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
