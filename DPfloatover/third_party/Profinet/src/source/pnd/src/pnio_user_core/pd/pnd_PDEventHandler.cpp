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
/*  F i l e               &F: pnd_PDEventHandler.cpp                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PD Event Handler class                                 */
/*  (Listener and Handler for CM PD Events)                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3501
#define PND_MODULE_ID      3501

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

#include "cm_inc.h"

/* StdLib++ */
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_UserNode.h"
#include "pnd_ParamStore.h"
#include "pnd_PDUser.h"
#include "pnd_PDSubslot.h"
#include "pnd_PDEventHandler.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */
 
/*----------------------------------------------------------------------------*/
/* Type Definition */

typedef struct
{
    LSA_UINT16 blockType;
    LSA_UINT16 blockLen;

    LSA_UINT16 blockVer;
    LSA_UINT16 AlarmType;

    LSA_UINT32 Api;
                    
    LSA_UINT16 slot;
    LSA_UINT16 subslot;

    LSA_UINT32 ModIdent;

    LSA_UINT32 SubIdent;

    LSA_UINT16 AlarmSpecifier;
    LSA_UINT16 UserStructureIdentifier;
} PND_PD_BLOCKTYPE_TYPE, PND_PD_BLOCKTYPE_PTR_TYPE;

/////////////////////////////////////////////////////////////////////////////
// CPDEventHandler
/////////////////////////////////////////////////////////////////////////////

CPDEventHandler::CPDEventHandler( PND_HANDLE_PTR_TYPE pHandle, CPDUser *pUser )
{
	PND_ASSERT( pHandle != 0 );
	PND_ASSERT( pUser != 0 );

	m_pndHandle  = *pHandle;
	m_pUser      = pUser;
	m_isStopping = LSA_FALSE;

	// Register the event handler methodes dependend on Config
	initMap();
}

/*----------------------------------------------------------------------------*/
CPDEventHandler::~CPDEventHandler()
{
}

/*----------------------------------------------------------------------------*/
PNIO_ALARM_TYPE PND_PD_ConvertAlarmTypeToHostLibType(LSA_UINT16 AlarmLeType)
{
  PNIO_ALARM_TYPE ret;
  switch(AlarmLeType)
  {
  case CM_ALARM_TYPE_DIAGNOSIS:
       ret = PNIO_ALARM_DIAGNOSTIC;
       break;
  case CM_ALARM_TYPE_PROCESS:
       ret = PNIO_ALARM_PROCESS;
       break;
  case CM_ALARM_TYPE_PULL:
       ret = PNIO_ALARM_PULL;
       break;
  case CM_ALARM_TYPE_PLUG:
       ret = PNIO_ALARM_PLUG;
       break;
  case CM_ALARM_TYPE_STATUS:
       ret = PNIO_ALARM_STATUS;
       break;
  case CM_ALARM_TYPE_UPDATE:
       ret = PNIO_ALARM_UPDATE;
       break;
  case CM_ALARM_TYPE_MEDIA_REDUNDANCY:
       ret = PNIO_ALARM_REDUNDANCY;
       break;
  case CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR:
       ret = PNIO_ALARM_CONTROLLED_BY_SUPERVISOR;
       break;
  case CM_ALARM_TYPE_RELEASED:
       ret = PNIO_ALARM_RELEASED_BY_SUPERVISOR;
       break;
  case CM_ALARM_TYPE_PLUG_WRONG:
       ret = PNIO_ALARM_PLUG_WRONG;
       break;
  case CM_ALARM_TYPE_RETURN_OF_SUBMODULE:
       ret = PNIO_ALARM_RETURN_OF_SUBMODULE;
       break;
  case CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS:
       ret = PNIO_ALARM_DIAGNOSTIC_DISAPPEARS;
       break;
  case CM_ALARM_TYPE_MCR_MISMATCH:
       ret = PNIO_ALARM_MCR_MISMATCH;
       break;
  case CM_ALARM_TYPE_PORT_DATA_CHANGED:
       ret = PNIO_ALARM_PORT_DATA_CHANGED;
       break;
  case CM_ALARM_TYPE_SYNC_DATA_CHANGED:
       ret = PNIO_ALARM_SYNC_DATA_CHANGED;
       break;
  case CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM:
       ret = PNIO_ALARM_ISOCHRONE_MODE_PROBLEM;
       break;
  case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM:
       ret = PNIO_ALARM_NETWORK_COMPONENT_PROBLEM;
       break;
  case CM_ALARM_TYPE_TIME_DATA_CHANGED:
       ret = PNIO_ALARM_TIME_DATA_CHANGED;
       break;
  case CM_ALARM_TYPE_UPLOAD_AND_STORAGE:
       ret = PNIO_ALARM_UPLOAD_AND_STORAGE;
       break;
  case CM_ALARM_TYPE_PULL_MODULE:
       ret = PNIO_ALARM_PULL_MODULE;
       break;
  default:
       PND_IOC_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "CopyAlarmData unknown ALARM_LE_TYPE=0x%x", AlarmLeType);
      ret = (PNIO_ALARM_TYPE)AlarmLeType;
       break;
  }
  return ret;
}
/////////////////////////////////////////////////////////////////////////////
// Event-Handling methodes
/////////////////////////////////////////////////////////////////////////////

LSA_VOID  CPDEventHandler::ErrorEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );

	PND_PD_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
		"CPDEventHandler::ErrorEventHandling(): CM PD user received a not handled event, OPC(%u) RQB(0x%08x)", 
		CM_RQB_GET_OPCODE( pRQB ),
		pRQB );

	PND_FATAL( "CPDEventHandler::ErrorEventHandling(): Not handled CM PD event received" );
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::AlarmIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_PD_ALARM_IND );
	PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK);

	LSA_HANDLE_TYPE  handle   = m_pUser->GetPnioHandle();

	CM_UPPER_ALARM_PTR_TYPE pAlarm = pRQB->args.channel.alarm;
	PND_ASSERT( pAlarm != 0 );

	PND_PD_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::AlarmIndication(): PD ALARM.IND, dev(%u) ar(%u) skey(%u) type(%u) RQB(0x%08x)",
		pAlarm->device_nr, 
		pAlarm->ar_nr, 
		pAlarm->session_key,
		pAlarm->alarm_type,
		pRQB );

	PND_PD_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::AlarmIndication():    prio(%u) seq(%u) ap/sl/su(%u/%u/%#x) ident mod/sub(%#x/%#x)",
		pAlarm->alarm_priority,
		pAlarm->alarm_sequence,
		pAlarm->api, 
		pAlarm->slot_nr, 
		pAlarm->subslot_nr,
		pAlarm->mod_ident,
		pAlarm->sub_ident );

	PND_PD_TRACE_08( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::AlarmIndication():    diag ch/gen/sub(%u/%u/%u) ar(%u) maintenance(%#x) tag(%#x)  alarm_data_len(%u) data_len(%u)",
		pAlarm->diag_channel_available,
		pAlarm->diag_generic_available,
		pAlarm->diag_submod_available,
		pAlarm->ar_diagnosis_state,
		pAlarm->maintenance_status,
		pAlarm->alarm_tag, 
		pAlarm->alarm_data_length,
		(pAlarm->alarm_data_length - CM_ALARM_OFFSET_DATA));

	switch( pAlarm->alarm_type ) // Update information based on alarm type
	{
		case CM_ALARM_TYPE_PORT_DATA_CHANGED:
		case CM_ALARM_TYPE_SYNC_DATA_CHANGED:
		case CM_ALARM_TYPE_MEDIA_REDUNDANCY:
		case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM:
		case CM_ALARM_TYPE_TIME_DATA_CHANGED:
		case CM_ALARM_TYPE_DFP_PROBLEM:
		case CM_ALARM_TYPE_MRPD_PROBLEM:
		case CM_ALARM_TYPE_MULTIPLE_INTERFACE:
			{
				// Update submod state (BGZF info) based on Alarm information
				CPDSubslot* pSub = m_pUser->GetMappedSubslot( pAlarm->subslot_nr );
				PND_ASSERT( pSub != 0 );

				pSub->UpdateFromAlarm( pAlarm->diag_submod_available, pAlarm->maintenance_status ); 
			}
			break;
		default:
			break;
	}

	// get alarm resource from CP
    PND_RQB_PTR_TYPE p_Rqb = m_pUser->GetAlarmResource();

    PND_ALARM_PTR_TYPE pndAlarm = &p_Rqb->args.iob_alarm;

	if (!p_Rqb)
    {
	// always send AlarmRsp
	// Note: no alarm ACK necessary
	CM_RQB_SET_OPCODE( pRQB, CM_OPC_PD_ALARM_RSP);
	CM_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}
    else
    {

		if(pndAlarm) {

			pndAlarm->alarm.AlarmType = PND_PD_ConvertAlarmTypeToHostLibType(pAlarm->alarm_type);
            pndAlarm->alarm.AlarmPriority = (PNIO_APRIO_TYPE) pAlarm->alarm_priority;  // CM_ALARM_PRIORITY_LOW
			pndAlarm->alarm.SlotNum = pAlarm->slot_nr;
            pndAlarm->alarm.SubslotNum = pAlarm->subslot_nr;

			// set following fields to zero
            pndAlarm->alarm.CpuAlarmPriority = 0;
            pndAlarm->alarm.DiagDs[0]      = 0;
            pndAlarm->alarm.PrAlarmInfo[0] = 0;
            pndAlarm->alarm.DiagDs[1]      = 0;
            pndAlarm->alarm.PrAlarmInfo[1] = 0;
            pndAlarm->alarm.DiagDs[2]      = 0;
            pndAlarm->alarm.PrAlarmInfo[2] = 0;
            pndAlarm->alarm.DiagDs[3]      = 0;
            pndAlarm->alarm.PrAlarmInfo[3] = 0;

			// Alarm "T" Type
            pndAlarm->alarm.PnioCompatModtype          = 0;
            pndAlarm->alarm.AlarmTinfo.CompatDevGeoaddr= 0;
            pndAlarm->alarm.AlarmTinfo.ProfileType     = PNIO_AL_PROFILE_TYPE;
            pndAlarm->alarm.AlarmTinfo.AinfoType       = PNIO_AL_AINFO_TYPE;
            pndAlarm->alarm.AlarmTinfo.ControllerFlags = PNIO_AL_CTRL_FLAGS_PD;
            pndAlarm->alarm.AlarmTinfo.DeviceFlag      = pAlarm->ar_diagnosis_state;

			pndAlarm->alarm.AlarmTinfo.PnioVendorIdent = m_pUser->GetVendorId();
            pndAlarm->alarm.AlarmTinfo.PnioDevIdent    = m_pUser->GetDeviceId();
            pndAlarm->alarm.AlarmTinfo.PnioDevInstance = m_pUser->GetInstanceId();

			// Alarm "A" Type
            pndAlarm->alarm.AlarmAinfo.BlockType      = 0;
            pndAlarm->alarm.AlarmAinfo.BlockVersion   = 0;
            pndAlarm->alarm.AlarmAinfo.Api            = 0;
            pndAlarm->alarm.AlarmAinfo.AlarmSpecifier = 0;
            pndAlarm->alarm.AlarmAinfo.ModIdent       = 0;
            pndAlarm->alarm.AlarmAinfo.SubIdent       = 0;
            pndAlarm->alarm.AlarmAinfo.UserStrucIdent = 0;
            pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen = 0;

			PND_ASSERT(pAlarm->alarm_data_length >= PNIO_AOFF_USERSTRUCTUREIDENTIFIER);

            if ( pAlarm->alarm_data_length >= PNIO_AOFF_USERSTRUCTUREIDENTIFIER )
            {
                PND_PD_BLOCKTYPE_TYPE block_access;

                pnd_memset(&block_access,0,sizeof(PND_PD_BLOCKTYPE_TYPE));

                pnd_memcpy (&block_access, pAlarm->alarm_data, CM_ALARM_OFFSET_DATA);

                pndAlarm->alarm.AlarmAinfo.BlockType    =  pnd_hton16( block_access.blockType );
                pndAlarm->alarm.AlarmAinfo.BlockVersion =  pnd_hton16( block_access.blockVer );
                    
                pndAlarm->alarm.AlarmAinfo.Api      =  pnd_hton32( block_access.Api );
                pndAlarm->alarm.AlarmAinfo.ModIdent =  pnd_hton32( block_access.ModIdent );
                pndAlarm->alarm.AlarmAinfo.SubIdent =  pnd_hton32( block_access.SubIdent );

                PND_ASSERT(pAlarm->alarm_type  ==  pnd_hton16( block_access.AlarmType ));
                PND_ASSERT(pAlarm->slot_nr     ==  pnd_hton16( block_access.slot ));
                PND_ASSERT(pAlarm->subslot_nr  ==  pnd_hton16( block_access.subslot ));


                pndAlarm->alarm.AlarmAinfo.AlarmSpecifier =   pnd_hton16( block_access.AlarmSpecifier );

                if (pAlarm->alarm_data_length >= PNIO_ALARM_NORM_HEADER_OFFSETS_LEN) 
                {
                    block_access.UserStructureIdentifier      =   pnd_hton16(  block_access.UserStructureIdentifier );
                    pndAlarm->alarm.AlarmAinfo.UserStrucIdent = block_access.UserStructureIdentifier;

                    pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen =
                        pAlarm->alarm_data_length - PNIO_ALARM_NORM_HEADER_OFFSETS_LEN;

                    if(pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen > sizeof(pndAlarm->alarm.AlarmAinfo.UAData))
                    {
                        pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen = sizeof(pndAlarm->alarm.AlarmAinfo.UAData);
                    }

                    pnd_memcpy(&pndAlarm->alarm.AlarmAinfo.UAData.UserAlarmData, pAlarm->alarm_data + PNIO_ALARM_NORM_HEADER_OFFSETS_LEN, pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen);

                    switch(pndAlarm->alarm.AlarmAinfo.UserStrucIdent)
                    {
                    case 0x8000: // ChannelDiagnosis
                        {
                            pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelErrorType  = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelErrorType);
                            pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelNumber     = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelNumber);
                            pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelProperties = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelProperties);
                            break;
                        }

                    case 0x8002: // ExtChannelDiagnosis
                        {
                            pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelErrorType    = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelErrorType);
                            pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelNumber       = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelNumber);
                            pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelProperties   = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelProperties);
                            pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelErrorType = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelErrorType);
                            pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelAddValue  = pnd_hton32(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelAddValue);
                            break;
                        }

                    case 0x8100:    // Maintenance
                        {
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelErrorType  = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelErrorType);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelNumber     = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelNumber);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelProperties = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelProperties);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.UserStrucIdent    = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.UserStrucIdent);

                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockLength = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockLength);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockType   = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockType);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.MaintenanceStatus       = pnd_hton32(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.MaintenanceStatus);
                            
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelErrorType = pnd_hton16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelErrorType);
                            pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelAddValue  = pnd_hton32(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelAddValue);
                            break;
                        }
                            

                    default: 
                        {
                            break;
                        }

                    }// end switch

                }
                else 
                {
					// no Alarm Data
                    pndAlarm->alarm.AlarmAinfo.UserStrucIdent =  0;
                    pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen = 0;
                }
            }
           
		}

		// deleted in Function: LSA_VOID CIOCUser::AlarmReProvide( PND_RQB_PTR_TYPE p_rb ) 
        PND_PD_ALARM_SESSION_PTR_TYPE SessionData = new PND_PD_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
		m_pUser->AlarmIndication(p_Rqb);
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::ApplReadyIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( CM_RQB_GET_OPCODE( pRQB ) == CM_OPC_PD_EVENT_APPL_READY_IND );

	CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
	LSA_RESPONSE_TYPE response     = CM_RQB_GET_RESPONSE( pRQB );

	LSA_HANDLE_TYPE handle = m_pUser->GetPnioHandle();
	LSA_UINT32      pnioErr = PNIO_OK;

	switch ( response ) 
	{
	case CM_ERR_PRM_CONSISTENCY:
		{
			PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
				"CPDEventHandler::ApplReadyIndication(): PD APPL_READY.IND(ERR_PRM_CONSISTENCY) dev(%u) ar(%u) skey(%u) RQB(0x%08x)",
				pEvent->device_nr, 
				pEvent->ar_nr, 
				pEvent->session_key,
				pRQB );

			m_pUser->ResetBgzdState();
			m_pUser->SetARP();

			pnioErr = PNIO_ERR_INTERNAL;
		}
		break;

	case CM_OK:
		{
			PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
				"CPDEventHandler::ApplReadyIndication(): PD APPL_READY.IND(OK) dev(%u) ar(%u) skey(%u) RQB(0x%08x)",
				pEvent->device_nr, 
				pEvent->ar_nr, 
				pEvent->session_key,
				pRQB );

			m_pUser->ResetBgzdState();
		}
		break;

	case CM_OK_ADAPTION_NEEDED:
		{
			PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
				"CPDEventHandler::ApplReadyIndication(): PD APPL_READY.IND(OK_ADAPTATION_NEEDED) dev(%u) ar(%u) skey(%u) RQB(0x%08x)",
				pEvent->device_nr, 
				pEvent->ar_nr, 
				pEvent->session_key,
				pRQB );

			m_pUser->ResetBgzdState();

		}
		break;

    case CM_OK_RESET_TO_FACTORY:
        {
			PND_PD_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
				"CPDEventHandler::ApplReadyIndication(): PD APPL_READY.IND(OK_RESET_TO_FACTORY) dev(%u) ar(%u) skey(%u) RQB(0x%08x)",
				pEvent->device_nr, 
				pEvent->ar_nr, 
				pEvent->session_key,
				pRQB );

            m_pUser->ResetBgzdState();
        }
        break;

	default:
		{
			PND_FATAL( "CPDEventHandler::ApplReadyIndication(): unexpected PD APPL_READY response");
		}
		break;
	}

	// Update PD submod states from DIFF list
	m_pUser->UpdateFromDiffList( &pEvent->u.pd.appl_ready.diff_list );

	// always send RSP
	// note: CM uses the RQB for next indication
	CM_RQB_SET_OPCODE( pRQB, CM_OPC_PD_EVENT_APPL_READY_RSP);
	CM_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );

	// Signal PRM sequence finished
	// Note: in case of startup the PD is now ready
	m_pUser->PrmFinished( pnioErr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::SyncLedIndication(CM_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( CM_RQB_GET_OPCODE( pRQB ) == CM_OPC_PD_SYNC_LED_INFO );

	LSA_HANDLE_TYPE handle = m_pUser->GetPnioHandle();

	PND_PD_TRACE_08( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::SyncLedIndication(): PD SYNC LED-INFO.IND rsp(%#x) portID(%u) sync_led_info(%u) rate_valid(%u) rcv_sync_prio(%#x) local_prio(%#x) pll_state(%#x) RQB(0x%08x)",
		CM_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.pd.sync_led_info->edd_port_id,
		pRQB->args.pd.sync_led_info->sync_led_info,
		pRQB->args.pd.sync_led_info->is_rate_valid,
		pRQB->args.pd.sync_led_info->rcv_sync_prio,
		pRQB->args.pd.sync_led_info->local_prio,
		pRQB->args.pd.sync_led_info->pll_state,
		pRQB );

	PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

	// Update Sync led info for mapped PDEV subslot
	CPDSubslot* pSub = m_pUser->GetSubslot( pRQB->args.pd.sync_led_info->edd_port_id );
	PND_ASSERT( pSub != 0 );

	pSub->SetSyncState( 
		pRQB->args.pd.sync_led_info->sync_led_info,
		pRQB->args.pd.sync_led_info->is_rate_valid,
		pRQB->args.pd.sync_led_info->rcv_sync_prio,
		pRQB->args.pd.sync_led_info->local_prio,
		pRQB->args.pd.sync_led_info->pll_state );

	// reprovide RQB
	CM_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::LinkStatusIndication(CM_UPPER_RQB_PTR_TYPE pRQB )
{
	PND_ASSERT( m_pUser != 0 );
	PND_ASSERT( CM_RQB_GET_OPCODE( pRQB ) == CM_OPC_PD_LINK_STATUS_INFO );

	LSA_HANDLE_TYPE handle = m_pUser->GetPnioHandle();

	PND_PD_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::LinkStatusIndication(): PD LINK_STATUS.IND rsp(%#x) portID(%u) status(%u) speed(%u) mode(%u) if_id(%u) RQB(0x%08x)",
		CM_RQB_GET_RESPONSE( pRQB ),
		pRQB->args.pd.link_status->port_id,
		pRQB->args.pd.link_status->status,
		pRQB->args.pd.link_status->speed,
		pRQB->args.pd.link_status->mode,
		pRQB->args.pd.link_status->interface_id,
		pRQB );

	PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );
	PND_ASSERT( pRQB->args.pd.link_status->status != CM_LINK_STATUS_UNKNOWN ); // Never "unknown"

	if ( pRQB->args.pd.link_status->status == CM_LINK_STATUS_UP )
	{
		PND_ASSERT ( pRQB->args.pd.link_status->speed != CM_LINK_SPEED_UNKNOWN );
		PND_ASSERT ( pRQB->args.pd.link_status->mode  != CM_LINK_MODE_UNKNOWN  );
	}

	// Update MAUType info for PDEV subslot
	CPDSubslot* pSub = m_pUser->GetSubslot( pRQB->args.pd.link_status->port_id );
	PND_ASSERT( pSub != 0 );

	pSub->SetMauState( 
		pRQB->args.pd.link_status->status,
		pRQB->args.pd.link_status->speed,
		pRQB->args.pd.link_status->mode );

	// Trigger all link down check on PD User (to update the BGZD state)
	m_pUser->CheckLinkState();

	// reprovide RQB
	CM_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}


/////////////////////////////////////////////////////////////////////////////
// Requests
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CPDEventHandler::StartListening( LSA_UINT16 nrOfEvents, LSA_UINT16 nrOfSub )
{
	PND_ASSERT( m_pUser!= 0 );

	LSA_HANDLE_TYPE handle = m_pUser->GetPnioHandle();

	PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CPDEventHandler::StartListening(): Providing PD resources, PNIO-IFNr(%u) events(%u) submods(%u)", 
		m_pndHandle.pnio_if_nr, 
		nrOfEvents,
		nrOfSub );

	m_isStopping = LSA_FALSE;

	// Provide normal event resources (for ApplReady event only)
	for ( LSA_UINT16 lvi = 0; lvi < nrOfEvents; lvi++ )
	{
		CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_EVENT_IND_RES_PROVIDE, handle, &m_pndHandle, 0 );
		PND_ASSERT(pRQB);

		eps_user_request_lower( pRQB, this, CPDEventHandler::EventReceived );
	}

	// Provide alarm, sync and link info resources for each port and IF submod
	for ( LSA_UINT16 portID = 0; portID < nrOfSub; portID++ )
	{
		CM_UPPER_RQB_PTR_TYPE pRQB;

		// Alarms
		pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_ALARM_IND_RES_PROVIDE, handle, &m_pndHandle, 0 );
		PND_ASSERT( pRQB );

		pRQB->args.channel.alarm->alarm_priority = CM_ALARM_PRIORITY_LOW;

		eps_user_request_lower( pRQB, this, CPDEventHandler::EventReceived );

		// Sync Led info
		pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_SYNC_LED_INFO, handle, &m_pndHandle, 0 );
		PND_ASSERT(pRQB);

		pRQB->args.pd.sync_led_info->edd_port_id   = portID;
		pRQB->args.pd.sync_led_info->sync_led_info = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;
		pRQB->args.pd.sync_led_info->is_rate_valid = LSA_FALSE;
		pRQB->args.pd.sync_led_info->rcv_sync_prio = 0;
		pRQB->args.pd.sync_led_info->local_prio    = 0;
		pRQB->args.pd.sync_led_info->pll_state     = 0;


		eps_user_request_lower( pRQB, this, CPDEventHandler::EventReceived );

		// Link state
		pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_PD_LINK_STATUS_INFO, handle, &m_pndHandle, 0 );
		PND_ASSERT(pRQB);

		pRQB->args.pd.link_status->port_id      = portID;
		pRQB->args.pd.link_status->status       = CM_LINK_STATUS_UNKNOWN;
		pRQB->args.pd.link_status->speed        = CM_LINK_SPEED_UNKNOWN;
		pRQB->args.pd.link_status->mode         = CM_LINK_MODE_UNKNOWN;
		pRQB->args.pd.link_status->interface_id = 0xFFFF;

		eps_user_request_lower( pRQB, this, CPDEventHandler::EventReceived );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::StopListening()
{
	// Note: i.E.: is called if closing the channel is requested
	//       to prevent reproviding IND RQBs after Close
	m_isStopping = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::HandleEvent( LSA_OPCODE_TYPE opcode, CM_UPPER_RQB_PTR_TYPE pRQB )
{
	// Delegate to the registered common Event Handler method
	PND_ASSERT( pRQB != 0);
	PND_ASSERT(( opcode > 0 ) && ( opcode < CM_OPC_MAX ));

	(this->*m_handler[opcode])( pRQB );
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////////////////////////////////////

LSA_VOID  CPDEventHandler::EventReceived(LSA_VOID * p_inst, LSA_VOID * p_rb)
{
	CPDEventHandler *eventObj  = static_cast<CPDEventHandler*>(p_inst);
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( eventObj           != 0 );
	PND_ASSERT( pRQB               != 0 );
	PND_ASSERT( eventObj->m_pUser  != 0 );

	if ( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK_CANCELLED )
	{
		// IND resource is canceled -- stop requesting in case of shutdown
		PND_ASSERT( eventObj->m_pUser->IsShutdown() );

		PND_PD_TRACE_02( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
			"CPDEventHandler::EventReceived(): PD RQB cancelled, PNIO-IF(%u) pRQB(0x%08x)", 
			eventObj->m_pndHandle.pnio_if_nr, pRQB );

		pnd_cm_free_rqb( pRQB, &eventObj->m_pndHandle );
	}
	else
	{
		// Handle the event --> use event function map for handle the event
		LSA_OPCODE_TYPE opcode = CM_RQB_GET_OPCODE( pRQB );

		// Prepare response and call event handler for Opcode
		LSA_HANDLE_TYPE handle = eventObj->m_pUser->GetPnioHandle();

		// Prepare response and call event handler for Opcode
		CM_RQB_SET_HANDLE( pRQB, handle );
		eventObj->HandleEvent( opcode, pRQB );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CPDEventHandler::AlarmIndicationDone( LSA_VOID*p_rb )
{
	CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

	LSA_HANDLE_TYPE  handle   = m_pUser->GetPnioHandle();

	CM_RQB_SET_OPCODE( pRQB, CM_OPC_PD_ALARM_RSP);
	CM_RQB_SET_HANDLE( pRQB, handle );

	reprovide( pRQB );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID  CPDEventHandler::initMap()
{
	// Initialize the Event-Handler list
	for ( int lvi = 0; lvi < CM_OPC_MAX; lvi++ )
	{
		m_handler[lvi] = &CPDEventHandler::ErrorEventHandling;
	}

	// Set the CM Event methodes corresponding to CM Event numbers
	m_handler[CM_OPC_PD_ALARM_IND]            = &CPDEventHandler::AlarmIndication;
	m_handler[CM_OPC_PD_EVENT_APPL_READY_IND] = &CPDEventHandler::ApplReadyIndication;
	m_handler[CM_OPC_PD_SYNC_LED_INFO]        = &CPDEventHandler::SyncLedIndication;
	m_handler[CM_OPC_PD_LINK_STATUS_INFO]     = &CPDEventHandler::LinkStatusIndication;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  CPDEventHandler::reprovide( CM_UPPER_RQB_PTR_TYPE pRQB )
{
	if ( m_isStopping )  // Closing CM PD channel in progress ?
	{
		// free RQB
		pnd_cm_free_rqb( pRQB, &m_pndHandle );
	}
	else
	{
		// Reprovide RQB for next indication
		eps_user_request_lower( pRQB, this, CPDEventHandler::EventReceived );
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
