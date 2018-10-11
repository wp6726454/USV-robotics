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
/*  F i l e               &F: pnd_BgzdStore.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of BGZD info storage classes                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3101
#define PND_MODULE_ID       3101

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_trc.h"
#include "pnd_sys.h"

/* StdLib++ */
#include <string>
#include <vector>

using namespace std;

#include "pnd_ValueHelper.h"
#include "pnd_BgzdStore.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */


/////////////////////////////////////////////////////////////////////////////
// CBgzdInfoStore  class to holds the BGZD information
/////////////////////////////////////////////////////////////////////////////

CBgzdInfoStore::CBgzdInfoStore()
{
	// Initialize with BGZD ok
	m_subslotState = PND_BGZD_SUB_OK;
	m_userState    = PND_BGZD_USER_OK;
}

/*---------------------------------------------------------------------------*/
CBgzdInfoStore::~CBgzdInfoStore()
{
}

/*---------------------------------------------------------------------------*/
/* Accessors Write                                                           */
/*---------------------------------------------------------------------------*/

void CBgzdInfoStore::Reset()
{
	m_subslotState = PND_BGZD_SUB_OK;
	m_userState    = PND_BGZD_USER_OK;
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::UpdateBgzdInfo( CBgzdInfoStore& bgzdInfo )
{
	// Update (Merge) the own states to BGZD in arg
	bgzdInfo.m_subslotState |= m_subslotState;
	bgzdInfo.m_userState    |= m_userState;
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::UpdateFromSubslotState( LSA_UINT16 submodState )
{
	// set the submod state from CM info
	// Note: the IOXS BAD state is set seperate from states in consumer/provider frames
	m_subslotState = PND_BGZD_SUB_OK;

	if ( ( submodState & CM_SUB_STATE_IDENT_MASK ) == CM_SUB_STATE_IDENT_NO_SUBMODULE )
	{
		m_subslotState |= PND_BGZD_SUB_NOT_EXISTS;
	}

	if ( ( submodState & CM_SUB_STATE_IDENT_MASK ) == CM_SUB_STATE_IDENT_WRONG )
	{
		m_subslotState |= PND_BGZD_SUB_WRONG;
	}

	if ( ( submodState & CM_SUB_STATE_AR_INFO_MASK ) == CM_SUB_STATE_AR_INFO_APPL_READY_PENDING )
	{
		m_subslotState |= PND_BGZD_SUB_ARP;
	}

	LSA_UINT16 locked = submodState & CM_SUB_STATE_AR_INFO_MASK;

	if (( locked == CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED ) ||
		( locked == CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER   ) ||
		( locked == CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR   ))
	{
		m_subslotState |= PND_BGZD_SUB_LOCKED_BY;
	}

	if ( ( submodState & CM_SUB_STATE_DIAG_INFO_MASK ) == CM_SUB_STATE_DIAG_INFO_AVAILABLE )
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_ERR;
	}

	if ( ( submodState & CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK) == CM_SUB_STATE_MAINTENANCE_REQUIRED )
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MR;
	}

	if ( ( submodState & CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK ) == CM_SUB_STATE_MAINTENANCE_DEMANDED )
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MD;
	}

	if ( ( submodState & CM_SUB_STATE_QUALIFIED_INFO_MASK) == CM_SUB_STATE_QUALIFIED_INFO_AVAILABLE )
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_QUA;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::UpdateFromDiagnosisList( LSA_UINT16 cntError, LSA_UINT16 cntMR, LSA_UINT16 cntMD, LSA_UINT16 cntQual )
{
	// Update BGZD Diagnosis info for submod based on diagnosis in diagnosis list
	if ( cntError > 0 )  // Diagnosis Error ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_ERR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_ERR;
	}

	if ( cntMR > 0 )   // Diagnosis MR ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MR;
	}

	if ( cntMD > 0 )  // Diagnosis MD ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MD;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MD;
	}

	if ( cntQual > 0 ) // Diagnosis Qua ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_QUA;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_QUA;
	}

}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintStat )
{
	// Update BGZD Diagnosis info for submod based on Alarm info
	if ( hasDiag )  // Diagnosis Error ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_ERR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_ERR;
	}

	// set the maint info (see see CM_MAINTENANCE_STATUS_xxx for single bits)
	if ( maintStat & CM_MAINTENANCE_STATUS_REQUIRED ) // Diagnosis MR ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MR;
	}

	if ( maintStat & CM_MAINTENANCE_STATUS_DEMANDED ) // Diagnosis MD ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MD;
	}
	else
	{		
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MD;
	}

	if ( maintStat & CM_MAINTENANCE_STATUS_QUA_BITS ) // Diagnosis Qua ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_QUA;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_QUA;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::UpdateFromLedInfo( void* pLedInfo )
{
	CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE pInfo = (CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE)pLedInfo;
	PND_ASSERT( pInfo != 0 );

	// Update BGZD Diagnosis info for submod based on CM SV Led info
	// Note: only used on IOD Device BGZD updates (summary subslot states for LED triggering)
	if ( pInfo->led_info & CM_SV_DEVICE_LED_INFO_NO ) // Not plugged
	{
		m_subslotState |= PND_BGZD_SUB_NOT_EXISTS;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_NOT_EXISTS;
	}

	if ( pInfo->led_info & CM_SV_DEVICE_LED_INFO_WRONG )  // wrong
	{
		m_subslotState |= PND_BGZD_SUB_WRONG;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_WRONG;
	}

	if ( pInfo->led_info & CM_SV_DEVICE_LED_INFO_ARP )  // ARP
	{
		m_subslotState |= PND_BGZD_SUB_ARP;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_ARP;
	}

	if (( pInfo->led_info & CM_SV_DEVICE_LED_INFO_LOCKED_SO  ) ||  // Locked
		( pInfo->led_info & CM_SV_DEVICE_LED_INFO_LOCKED_IOC ) ||
		( pInfo->led_info & CM_SV_DEVICE_LED_INFO_LOCKED_IOS ))
	{
		m_subslotState |= PND_BGZD_SUB_LOCKED_BY;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_LOCKED_BY;
	}

	if (( pInfo->led_info & CM_SV_DEVICE_LED_INFO_DIAG  ) ||  // Error Diagnosis
		( pInfo->led_info & CM_SV_DEVICE_LED_INFO_MCONS ))
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_ERR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_ERR;
	}

	// set the maint info (see see CM_MAINTENANCE_STATUS_xxx for single bits)
	if ( pInfo->maint_info & CM_MAINTENANCE_STATUS_REQUIRED ) // Diagnosis MR ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MR;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MR;
	}

	if ( pInfo->maint_info & CM_MAINTENANCE_STATUS_DEMANDED ) // Diagnosis MD ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_MD;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_MD;
	}

	if ( pInfo->maint_info & CM_MAINTENANCE_STATUS_QUA_BITS ) // Diagnosis Qua ?
	{
		m_subslotState |= PND_BGZD_SUB_DIAG_QUA;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_DIAG_QUA;
	}

	// Update BGZD Diagnosis info for user based on CM SV Led info
	// Note: set the AR diag info ( all indata state of RT ARs)
	if ( pInfo->led_info & CM_SV_DEVICE_LED_INFO_RT_AR ) // No AR, or any AR not INDATA
	{
		// All indata
		m_userState &= ~PND_BGZD_IOD_AR_NOT_INDATA;
	}
	else
	{
		m_userState |= PND_BGZD_IOD_AR_NOT_INDATA;
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CBgzdInfoStore::SetNotExists( LSA_BOOL state )
{
	if ( state ) // not exists ?
	{
		m_subslotState |= PND_BGZD_SUB_NOT_EXISTS;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_NOT_EXISTS;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetWrong( LSA_BOOL state )
{
	if ( state ) // wrong ?
	{
		m_subslotState |= PND_BGZD_SUB_WRONG;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_WRONG;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetARP( LSA_BOOL state )
{
	if ( state ) // set ARP
	{
		m_subslotState |= PND_BGZD_SUB_ARP;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_ARP;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetLocked( LSA_BOOL state )
{
	if ( state ) // set Locked
	{
		m_subslotState |= PND_BGZD_SUB_LOCKED_BY;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_LOCKED_BY;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetBadIOXS( LSA_BOOL state )
{
	if ( state ) // BAD IOXS ?
	{
		m_subslotState |= PND_BGZD_SUB_BAD_IOXS;
	}
	else
	{
		m_subslotState &= ~PND_BGZD_SUB_BAD_IOXS;
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CBgzdInfoStore::SetNoLinkUp( LSA_BOOL state )
{
	if ( state ) // NO link up on all ports ?
	{
		m_userState |= PND_BGZD_PD_NO_LINK_UP;
	}
	else
	{
		m_userState &= ~PND_BGZD_PD_NO_LINK_UP;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetBadIP( LSA_BOOL state )
{
	if ( state ) // NO link up on all ports ?
	{
		m_userState |= PND_BGZD_OHA_IP_BAD;
	}
	else
	{
		m_userState &= ~PND_BGZD_OHA_IP_BAD;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetBadNoS( LSA_BOOL state )
{
	if ( state ) // NO link up on all ports ?
	{
		m_userState |= PND_BGZD_OHA_NOS_BAD;
	}
	else
	{
		m_userState &= ~PND_BGZD_OHA_NOS_BAD;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetArStateIOC( LSA_BOOL notIndata, LSA_BOOL passivated )
{
	if ( notIndata ) // AR is INDATA
	{
		m_userState |= PND_BGZD_IOC_AR_NOT_INDATA;
	}
	else
	{
		m_userState &= ~PND_BGZD_IOC_AR_NOT_INDATA;
	}

	if ( passivated ) // AR is passivated (like offline)
	{
		m_userState |= PND_BGZD_IOC_DEV_PASSIVATED;
	}
	else
	{
		m_userState &= ~PND_BGZD_IOC_DEV_PASSIVATED;
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::SetArStateIOM( LSA_BOOL notIndata, LSA_BOOL passivated )
{
	if ( notIndata ) // MC-AR is INDATA
	{
		m_userState |= PND_BGZD_IOM_AR_NOT_INDATA;
	}
	else
	{
		m_userState &= ~PND_BGZD_IOM_AR_NOT_INDATA;
	}

	if ( passivated ) // MC-AR is passivated (like offline)
	{
		m_userState |= PND_BGZD_IOM_DEV_PASSIVATED;
	}
	else
	{
		m_userState &= ~PND_BGZD_IOM_DEV_PASSIVATED;
	}
}

/*---------------------------------------------------------------------------*/
/* Accessors Read                                                            */
/*---------------------------------------------------------------------------*/

LSA_BOOL CBgzdInfoStore::IsNotPlugged()
{
	// Check if not plugged bit is set for submod
	LSA_BOOL result = (( m_subslotState & PND_BGZD_SUB_NOT_EXISTS ) == PND_BGZD_SUB_NOT_EXISTS ) ? LSA_TRUE : LSA_FALSE;

	return ( result );
}

/*---------------------------------------------------------------------------*/
PND_BGZD_STATE_TYPE CBgzdInfoStore::GetBgzdState()
{
	PND_BGZD_STATE_TYPE result = BGZD_OK;

	// Check the single states (sub, user) in order deactivate, not exists, not avaliable, faulty, OK
	if ( IsDeactivatedState()) // Deactivated ?
	{
		result = BGZD_DEACTIVATED;
	}
	else
	{
		if ( IsNotExistsState()) // Not exist ?
		{
			result = BGZD_NOT_EXIST;
		}
		else
		{
			if ( IsNotAvaliableState()) // Not avaliable
			{
				result = BGZD_NOT_AVAILABLE;
			}
			else
			{
				if ( IsFaultyState())
				{
					result = BGZD_FAULTY;
				}
				else
				{
					if (( m_subslotState & PND_BGZD_SUB_DIAG_MR ) == PND_BGZD_SUB_DIAG_MR )
					{
						result = BGZD_MAINTENACE_REQUIRED;
					}
					else
					{
						if (( m_subslotState & PND_BGZD_SUB_DIAG_MD ) == PND_BGZD_SUB_DIAG_MD )
						{
							result = BGZD_MAINTENACE_DEMANDED;
						}
						else
						{
							if (( m_userState & PND_BGZD_IOD_AR_NOT_INDATA ) == PND_BGZD_IOD_AR_NOT_INDATA )
							{
								result = BGZD_IOD_AR_FAULT;
							}
						}
					}
				}
			}
		}
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
string CBgzdInfoStore::GetBgzdSubslot()
{
	stringVector tmp;
	CValueHelper val;

	// Get BGZD as string for a subslot only
	// Note: Qualified diagnosis are not part of BGZD so ignore it
	if ( m_subslotState & PND_BGZD_SUB_NOT_EXISTS )
	{
		tmp.push_back( "Not Exist" );
	}

	if ( m_subslotState & PND_BGZD_SUB_WRONG )
	{
		tmp.push_back( "Wrong" );
	}

	if ( m_subslotState & PND_BGZD_SUB_ARP )
	{
		tmp.push_back( "ARP" );
	}

	if ( m_subslotState & PND_BGZD_SUB_LOCKED_BY )
	{
		tmp.push_back( "Locked" );
	}

	if ( m_subslotState & PND_BGZD_SUB_BAD_IOXS )
	{
		tmp.push_back( "Bad IOXS" );
	}

	if ( m_subslotState & PND_BGZD_SUB_DIAG_ERR )
	{
		tmp.push_back( "Diag" );
	}

	if ( m_subslotState & PND_BGZD_SUB_DIAG_MD )
	{
		tmp.push_back( "MD" );
	}

	if ( m_subslotState & PND_BGZD_SUB_DIAG_MR )
	{
		tmp.push_back( "MR" );
	}

	string result = val.GetJoinedString( tmp, " | " );

	return ( result );
}

/*---------------------------------------------------------------------------*/
string CBgzdInfoStore::GetBgzdUser()
{
	stringVector tmp;
	CValueHelper val;

	// Get BGZD as string for a user including summary subslot BGZD
	if ( m_userState & PND_BGZD_OHA_IP_BAD )
	{
		tmp.push_back( "Bad IP" );
	}

	if ( m_userState & PND_BGZD_OHA_NOS_BAD )
	{
		tmp.push_back( "Bad NoS" );
	}

	if ( m_userState & PND_BGZD_PD_NO_LINK_UP )
	{
		tmp.push_back( "No link up" );
	}

	if ( m_userState & PND_BGZD_IOD_AR_NOT_INDATA )
	{
		tmp.push_back( "IOD AR state error" );
	}

	if ( m_userState & PND_BGZD_IOC_DEV_PASSIVATED )
	{
		tmp.push_back( "IOC Device passivated" );
	}

	if ( m_userState & PND_BGZD_IOC_AR_NOT_INDATA )
	{
		tmp.push_back( "IOC AR not INDATA" );
	}

	if ( m_userState & PND_BGZD_IOM_DEV_PASSIVATED )
	{
		tmp.push_back( "IOM Device passivated" );
	}

	if ( m_userState & PND_BGZD_IOM_AR_NOT_INDATA )
	{
		tmp.push_back( "IOM AR not INDATA" );
	}

	string subStr = GetBgzdSubslot();

	if ( !subStr.empty()) // subslot errors ?
	{
		tmp.push_back( subStr );
	}

	string result = val.GetJoinedString( tmp, " | " );

	return ( result );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_BOOL CBgzdInfoStore::IsOkState()
{
	LSA_BOOL result = LSA_FALSE;

	if (( m_userState == 0 ) && ( m_subslotState == 0 )) // error set ?
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CBgzdInfoStore::IsDeactivatedState()
{
	// check if any device is passivated on IOC/IOM
	LSA_BOOL result    = LSA_FALSE;
	LSA_UINT16 devMask = PND_BGZD_IOC_DEV_PASSIVATED | PND_BGZD_IOM_DEV_PASSIVATED;

	if ( (m_userState & devMask) != 0 )
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CBgzdInfoStore::IsNotExistsState()
{
	LSA_BOOL result   = LSA_FALSE;
	LSA_UINT16 arMask = PND_BGZD_IOC_AR_NOT_INDATA | PND_BGZD_IOM_AR_NOT_INDATA;

	// check if any submod has not exists and 
	// if any AR not INDATA on IOC, IOM
	if (( (m_subslotState & PND_BGZD_SUB_NOT_EXISTS) == PND_BGZD_SUB_NOT_EXISTS ) ||
		( (m_userState & arMask) != 0 ))
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CBgzdInfoStore::IsNotAvaliableState()
{
	// check submod for any wrong  BAD IOXS , ARP,locked by
	LSA_BOOL result = LSA_FALSE;
	LSA_UINT16 subMask = PND_BGZD_SUB_WRONG | PND_BGZD_SUB_BAD_IOXS | PND_BGZD_SUB_ARP | PND_BGZD_SUB_LOCKED_BY;
	
	if ( (m_subslotState & subMask ) != 0 )
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CBgzdInfoStore::IsFaultyState()
{
	// check submod for any error diag
	LSA_BOOL result = LSA_FALSE;

	if ((m_subslotState & PND_BGZD_SUB_DIAG_ERR) == PND_BGZD_SUB_DIAG_ERR )
	{
		result = LSA_TRUE;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
void CBgzdInfoStore::dumpBgzd()
{
	PND_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT,
		"CBgzdInfoStore::dumpBgzd(): BGZD ok(%u) deactivated(%u) notExist(%u) not avaliable(%u) faulty(%u)",
		IsOkState(),
		IsDeactivatedState(),
		IsNotExistsState(),
		IsNotAvaliableState(),
		IsFaultyState());

	PND_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_CHAT,
		"CBgzdInfoStore::dumpBgzd(): BGZD submod, not exist(%u) wrong(%d) ARP(%u) locked(%u) IOXS bad(%u) Error(%u) MD(%u) MR(%u)",
			( m_subslotState & PND_BGZD_SUB_NOT_EXISTS ),
			( m_subslotState & PND_BGZD_SUB_WRONG ),
			( m_subslotState & PND_BGZD_SUB_ARP ),
			( m_subslotState & PND_BGZD_SUB_LOCKED_BY ),
			( m_subslotState & PND_BGZD_SUB_BAD_IOXS ),
			( m_subslotState & PND_BGZD_SUB_DIAG_ERR ),
			( m_subslotState & PND_BGZD_SUB_DIAG_MD ),
			( m_subslotState & PND_BGZD_SUB_DIAG_MR ));

	PND_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_CHAT,
		"CBgzdInfoStore::dumpBgzd(): BGZD user, bad IP(%u) bad NOS(%d) no link up(%u) IOD AR(%u) IOD Dev(%u) IOC AR(%u) IOM Dev(%u) IOM AR(%u)",
			( m_userState & PND_BGZD_OHA_IP_BAD ),
			( m_userState & PND_BGZD_OHA_NOS_BAD ),
			( m_userState & PND_BGZD_PD_NO_LINK_UP ),
			( m_userState & PND_BGZD_IOD_AR_NOT_INDATA ),
			( m_userState & PND_BGZD_IOC_DEV_PASSIVATED ),
			( m_userState & PND_BGZD_IOC_AR_NOT_INDATA ),
			( m_userState & PND_BGZD_IOM_DEV_PASSIVATED ),
			( m_userState & PND_BGZD_IOM_AR_NOT_INDATA ));
}

/////////////////////////////////////////////////////////////////////////////
// CBgzdLedStore  class to holds the BGZD LED states (for one PN-Board)
/////////////////////////////////////////////////////////////////////////////

CBgzdLedStore::CBgzdLedStore()
{
	m_boardNr    = 0;
	m_timerId    = PND_TIMER_ID_INVALID;
	m_isShutdown = LSA_FALSE;
	m_toggleOn   = LSA_TRUE;

	// Initialize the LED storage for SF, BF, MAINT LED
	m_LedSF.led_category = LED_CATEGORY_SF;
	m_LedSF.gpio_nr      = PND_GPIO_NR_UNDEFINED;
	m_LedSF.led_state    = LED_STATE_OFF;
	m_LedSF.p_led_fct    = 0;

	m_LedBF.led_category = LED_CATEGORY_BF;
	m_LedBF.gpio_nr      = PND_GPIO_NR_UNDEFINED;
	m_LedBF.led_state    = LED_STATE_OFF;
	m_LedBF.p_led_fct    = 0;

	m_LedMAINT.led_category = LED_CATEGORY_MAINT;
	m_LedMAINT.gpio_nr      = PND_GPIO_NR_UNDEFINED;
	m_LedMAINT.led_state    = LED_STATE_OFF;
	m_LedMAINT.p_led_fct    = 0;

	// Allocate timer for toggle trigger (blink mode)
	LSA_UINT16 retVal = pnd_timer_allocate( 
			&m_timerId, 
			LSA_TIMER_TYPE_ONE_SHOT, 
			LSA_TIME_BASE_100MS, 
			CBgzdLedStore::EventTimeout );
	PND_ASSERT( retVal == LSA_RET_OK );
}

/*---------------------------------------------------------------------------*/
CBgzdLedStore::~CBgzdLedStore()
{
	if ( m_timerId != PND_TIMER_ID_INVALID )  // Timer allocated ?
	{
		// Delete the timer
		LSA_UINT16 retVal = pnd_timer_free( m_timerId );
		PND_ASSERT( retVal == LSA_RET_OK);

		m_timerId = PND_TIMER_ID_INVALID;
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CBgzdLedStore::SetBoardNr( LSA_UINT16 boardNr )
{
	PND_ASSERT(( boardNr >= 0 ) && ( boardNr <= PSI_CFG_MAX_HD_CNT));

	m_boardNr = boardNr;
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::RegisterLedSF( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr )
{
	// Add HW function and GPIO number (LED number) for SF LED triggering
	m_LedSF.p_led_fct = pFunc;
	m_LedSF.gpio_nr   = gpioNr;
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::RegisterLedBF( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr )
{
	// Add HW function and GPIO number (LED number) for BF LED triggering
	m_LedBF.p_led_fct = pFunc;
	m_LedBF.gpio_nr   = gpioNr;
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::RegisterLedMAINT( PND_BGZD_LED_FCT pFunc, LSA_UINT16 gpioNr )
{
	// Add HW function and GPIO number (LED number) for MAINT LED triggering
	m_LedMAINT.p_led_fct = pFunc;
	m_LedMAINT.gpio_nr   = gpioNr;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CBgzdLedStore::Startup()
{
	if ( m_boardNr > 0 )  // LD PnBoard ?
	{
		LSA_USER_ID_TYPE userID;
		userID.void_ptr = this;

		// Trigger first LED update (starting with all OK)
		SetLED( m_LedSF,    LSA_FALSE ); 
		SetLED( m_LedBF,    LSA_FALSE ); 
		SetLED( m_LedMAINT, LSA_FALSE ); 

		// Start the Timer for blink mode (500ms)
		PND_ASSERT( m_timerId != PND_TIMER_ID_INVALID );

		LSA_UINT16 retVal = pnd_timer_start( m_timerId, userID, 5 );
		PND_ASSERT( retVal == LSA_RET_OK);
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::Shutdown()
{
	if ( m_boardNr > 0 )  // LD PnBoard ?
	{
		// Set all LEDs to OFF
		SetLED( m_LedSF,    LSA_FALSE );
		SetLED( m_LedBF,    LSA_FALSE );
		SetLED( m_LedMAINT, LSA_FALSE );

		if ( m_timerId != PND_TIMER_ID_INVALID )  // Timer allocated ?
		{
			// Stop the toggle timer
			LSA_UINT16 retVal = pnd_timer_stop( m_timerId );
			PND_ASSERT(( retVal == LSA_RET_OK ) || (retVal == LSA_RET_OK_TIMER_NOT_RUNNING ));
		}
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::UpdateLED()
{
	if ( m_boardNr > 0 )  // LD PnBoard ?
	{
		// Update the LED states for SF, BF, MAINT from summary state
		m_LedBF.led_state = getLedStateBF();
		m_LedSF.led_state = getLedStateSF();
		m_LedMAINT.led_state = getLedStateMAINT();

		// Update LEDs with possible new state
		SetLED( m_LedBF,    isLedOn( m_LedBF.led_state    ));
		SetLED( m_LedSF,    isLedOn( m_LedSF.led_state    ));
		SetLED( m_LedMAINT, isLedOn( m_LedMAINT.led_state ));
	}
}

/*---------------------------------------------------------------------------*/
void CBgzdLedStore::SetLED( PND_LED_STORE_TYPE& led, LSA_BOOL state )
{
	PND_ASSERT( m_boardNr != 0 );

	// Trigger the HW LED
	if (( led.p_led_fct != 0 ) &&                    // HW function registered ?
		( led.gpio_nr   != PND_GPIO_NR_UNDEFINED ))  // LED avaliable
	{
		(*led.p_led_fct)( m_boardNr, led.gpio_nr, state );
	}
	else
	{
		PND_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
			"CBgzdLedStore::SetLED(): Set LED(%u) to state(%u), board(%u)",
			led.led_category,
			state,
			m_boardNr);
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CBgzdLedStore::EventTimeout( LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id )
{
	CBgzdLedStore* pInfo = static_cast <CBgzdLedStore*>(user_id.void_ptr);
	PND_ASSERT( pInfo != 0 );
	PND_ASSERT( timer_id == pInfo->m_timerId );

	if ( !pInfo->m_isShutdown ) // Shutdown ?
	{
		// Update the LEDs, in case of Toggle (Blink mode)
		if ( pInfo->m_LedSF.led_state == LED_STATE_TOGGLE )
		{
			pInfo->SetLED( pInfo->m_LedSF, pInfo->m_toggleOn );
		}

		if ( pInfo->m_LedBF.led_state == LED_STATE_TOGGLE )
		{
			pInfo->SetLED( pInfo->m_LedBF, pInfo->m_toggleOn );
		}

		if ( pInfo->m_LedMAINT.led_state == LED_STATE_TOGGLE )
		{
			pInfo->SetLED( pInfo->m_LedMAINT, pInfo->m_toggleOn );
		}

		pInfo->m_toggleOn = (pInfo->m_toggleOn) ? LSA_FALSE : LSA_TRUE;

		// Restart the timer
		LSA_UINT16 retVal = pnd_timer_start( timer_id, user_id, 5 ); // 500 ms
		PND_ASSERT( retVal == LSA_RET_OK);
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

PND_LED_STATE_TYPE CBgzdLedStore::getLedStateBF()
{
	PND_LED_STATE_TYPE result = LED_STATE_OFF;

	// Set to ON if IP, NoS invalid, no Link is Up on PDev
	LSA_UINT16 onMask = 
		PND_BGZD_OHA_IP_BAD |
		PND_BGZD_OHA_NOS_BAD |
		PND_BGZD_PD_NO_LINK_UP |
		PND_BGZD_IOD_AR_NOT_INDATA;

	if ( ( m_userState & onMask ) != 0 )
	{
		result = LED_STATE_ON;
	}
	else
	{
		// Set to toggle if no RT AR is present, or any RT AR is not IN-DATA on IOD
		// or state not_exists is present on IOC/IOM (see IsNotExists())
		// Note: for LED state the DA ARs are not checked for IOD
		if ((( m_userState & PND_BGZD_IOD_AR_NOT_INDATA ) == PND_BGZD_IOD_AR_NOT_INDATA ) ||
			( IsNotExistsState()))
		{
			result = LED_STATE_TOGGLE;
		}
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
PND_LED_STATE_TYPE CBgzdLedStore::getLedStateSF()
{
	PND_LED_STATE_TYPE result = LED_STATE_OFF;

	// Set to ON if any submod has state not avaliable or any submod has state faulty
	if (( IsNotAvaliableState()) || (IsFaultyState()) )
	{
		result = LED_STATE_ON;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
PND_LED_STATE_TYPE CBgzdLedStore::getLedStateMAINT()
{
	PND_LED_STATE_TYPE result = LED_STATE_OFF;

	// Set to ON if any submod has a maintance demanded diag
	if (( m_subslotState & PND_BGZD_SUB_DIAG_MD ) == PND_BGZD_SUB_DIAG_MD )
	{
		result = LED_STATE_ON;
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CBgzdLedStore::isLedOn( PND_LED_STATE_TYPE& ledState )
{
	LSA_BOOL result = LSA_FALSE;

	switch ( ledState )
	{
	case LED_STATE_ON:
		{
			result = LSA_TRUE;
		}
		break;
	case LED_STATE_TOGGLE:
		{
			result = ( m_toggleOn ) ? LSA_TRUE : LSA_FALSE;
		}
		break;
	default:
		break;
	}

	return ( result );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
