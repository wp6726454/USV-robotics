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
/*  F i l e               &F: pnd_IOCEventHandler.cpp                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of IOC Event Handler class                                */
/*  (Listener and Handler for CM CL Events)                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3203
#define PND_MODULE_ID       3203

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

#include "psi_int.h"

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
#include "pnd_IOCAr.h"
#include "pnd_IOCEventHandler.h"
#include "pnd_IOCDevice.h"
#include "pnd_IOCSubslot.h"
#include "pnd_IOCUser.h"

#include <iostream>
#include <string>
#include <sstream>



/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */
 
/////////////////////////////////////////////////////////////////////////////
// CIOCEventHandler
/////////////////////////////////////////////////////////////////////////////

CIOCEventHandler::CIOCEventHandler( PND_HANDLE_PTR_TYPE pHandle, CIOCUser *pUser )
{
    PND_ASSERT( pHandle != 0 );
    PND_ASSERT( pUser != 0 );

    m_pndHandle      = *pHandle;
    m_pUser          = pUser;
    m_pDevice        = 0;
    m_isUserStopping = LSA_FALSE;

    // Register the event handler methodes dependend on Config
    initUserMap();
}

/*----------------------------------------------------------------------------*/
CIOCEventHandler::CIOCEventHandler( PND_HANDLE_PTR_TYPE pHandle, CIOCDevice *pDevice )
{
    PND_ASSERT( pHandle != 0 );
    PND_ASSERT( pDevice != 0 );

    m_pndHandle      = *pHandle;
    m_pUser          = 0;
    m_pDevice        = pDevice;
    m_isUserStopping = LSA_FALSE;

    // Register the event handler methodes dependend on Config
    initDeviceMap();
}

/*----------------------------------------------------------------------------*/
CIOCEventHandler::~CIOCEventHandler()
{
}

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
} PND_IOC_BLOCKTYPE_TYPE, PND_IOC_BLOCKTYPE_PTR_TYPE;

/////////////////////////////////////////////////////////////////////////////
// Event-Handling methodes CM CL user
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCEventHandler::ErrorUserEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pUser != 0 );

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
        "ErrorEventHandling::ErrorEventHandling():CM CL user received a not handled event, OPC(%u) RQB(0x%08x)", 
        CM_RQB_GET_OPCODE( pRQB ),
        pRQB );

    PND_FATAL("CIOCEventHandler::ErrorUserEventHandling(): Not handled CM CL user event received" );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::OverloadIndication(CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pUser != 0 );
    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB ) == CM_OPC_OVERLOAD_INFO );

    LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

    PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::OverloadIndication():IOC Overload INFO.IND rsp(%#x) send counter/changed(%u/%u), recv counter/changed (%u/%u) pRQB(0x%08x)",
        CM_RQB_GET_RESPONSE( pRQB ),
        pRQB->args.channel.overload->send_overload_counter,
        pRQB->args.channel.overload->send_overload_changed,
        pRQB->args.channel.overload->recv_overload_counter,
        pRQB->args.channel.overload->recv_overload_changed,
        pRQB );

    // reprovide RQB
    CM_RQB_SET_HANDLE( pRQB, handle );

    reprovideUserInd( pRQB );
}

/////////////////////////////////////////////////////////////////////////////
// Event-Handling methodes CM CL device
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCEventHandler::ErrorDeviceEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
        "ErrorEventHandling::ErrorDeviceEventHandling():CM CL device received a not handled event, OPC(%u) RQB(0x%08x)", 
        CM_RQB_GET_OPCODE( pRQB ),
        pRQB );

    PND_FATAL( "CIOCEventHandler::ErrorDeviceEventHandling(): Not handled CM CL device event received" );
}


PNIO_ALARM_TYPE PND_ConvertAlarmTypeToHostLibType(LSA_UINT16 AlarmLeType)
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


/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArAlarmIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_ALARM_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_ALARM_PTR_TYPE pAlarm = pRQB->args.channel.alarm;
    PND_ASSERT( pAlarm != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pAlarm->device_nr );

    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArAlarmIndication():CM CL ALARM.IND, dev(%u) ar(%u) skey(%u) type(%u) RQB(0x%08x)",
        pAlarm->device_nr, 
        pAlarm->ar_nr, 
        pAlarm->session_key,
        pAlarm->alarm_type,
        pRQB );

    PND_IOC_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArAlarmIndication():    prio(%u) seq(%u) ap/sl/su(%u/%u/%#x) ident mod/sub(%#x/%#x)",
        pAlarm->alarm_priority,
        pAlarm->alarm_sequence,
        pAlarm->api, 
        pAlarm->slot_nr, 
        pAlarm->subslot_nr,
        pAlarm->mod_ident,
        pAlarm->sub_ident );

    PND_IOC_TRACE_08( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArAlarmIndication():    diag ch/gen/sub(%u/%u/%u) ar(%u) maintenance(%#x) tag(%#x) alarm_data_len(%u) data_len(%u)",
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
    case CM_ALARM_TYPE_PLUG:  // Alarms with DiffList, or GOOD state

    case CM_ALARM_TYPE_RELEASED:
    case CM_ALARM_TYPE_MCR_MISMATCH:
        {
            if (CmListIsEmpty (&pAlarm->u.cl.diff_list) ) // Empty list ?
            {
                // Set the submod to GOOD
                CIOCSubslot *pSub = m_pDevice->GetSubslot( pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr );

                if ( pSub != 0 )
                {
                    pSub->SetState( CM_SUB_STATE_GOOD );
                }
            }
            else
            {
                // Set new state for all submod from diff list entry
                // Note: in case of BGZD changed stated, the sublots triggers the updated
                m_pDevice->UpdateFromDiffList( &pAlarm->u.cl.diff_list );
            }
        }
        break;

    case CM_ALARM_TYPE_PULL_MODULE:  // Alarms for update single submod state property
        {
            // Set new state for all submod under pulled modul
            // Note: the BGZD changes are triggered by the subslots
            m_pDevice->UpdateForPullModul( pAlarm->api, pAlarm->slot_nr );
        }
        break;

    case CM_ALARM_TYPE_PULL:  // Alarms for update single submod state property
    case CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR:
    case CM_ALARM_TYPE_PLUG_WRONG:
        {
            // Set new single submod state and the summary states for device and client
            // Note: the BGZD changes are triggered by the subslots
            m_pDevice->UpdateForSubmodAlarm( pAlarm );
        }
        break;

    case CM_ALARM_TYPE_DIAGNOSIS:  // Alarms for update diag submod state property
    case CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS:
    case CM_ALARM_TYPE_MEDIA_REDUNDANCY:
    case CM_ALARM_TYPE_PORT_DATA_CHANGED:
    case CM_ALARM_TYPE_SYNC_DATA_CHANGED:
    case CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM:
    case CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM:
    case CM_ALARM_TYPE_TIME_DATA_CHANGED:
    case CM_ALARM_TYPE_DFP_PROBLEM:
    case CM_ALARM_TYPE_MRPD_PROBLEM:
    case CM_ALARM_TYPE_MULTIPLE_INTERFACE:
        {
            // Update single submod and the summary states for device and client
            // Note: the BGZD changes and diag updates are triggered by the subslots
            m_pDevice->UpdateFromDiagAlarm( pAlarm );
        }
        break;

    case CM_ALARM_TYPE_RETURN_OF_SUBMODULE: // Return of submod alarm received
        {
            // Reset an existing A.R.P state, if exist (changed for RQ-AP01054520)
            // Note: RoS Alarm implicit indicates end of A.R.P
            CIOCSubslot *pSub = m_pDevice->GetSubslot( pAlarm->api, pAlarm->slot_nr, pAlarm->subslot_nr );

            if ( pSub != 0 )
            {
                pSub->ResetARP();
            }
        }
        break;
    case CM_ALARM_TYPE_UPLOAD_AND_STORAGE:
    case CM_ALARM_TYPE_PROCESS:
        {
            //nothing to do, just pass up to user
        }
        break;

    default:
        // No special handling
        break;
    }

       

    // get alarm resource from CP
    CIOCUser *pUser = dynamic_cast<CIOCUser*>(m_pDevice->GetParent());
    PND_ASSERT( pUser != 0 );

    PND_RQB_PTR_TYPE p_Rqb = pUser->GetAlarmResource();

    PND_ALARM_PTR_TYPE pndAlarm = &p_Rqb->args.iob_alarm;
    
    if (!p_Rqb)
    {
        // If no PND_RQB_PTR_TYPE exists, Alarm will be acknowledge immediately.

        // prepare the Alarm-ACK RQB based on Alarm (PNIO Error default with OK)
        CM_UPPER_RQB_PTR_TYPE pAckRQB = getAlarmAckRqb( pAlarm, CM_PNIO_ERR_NONE );
        PND_ASSERT( pAckRQB != 0 );

        // Respond the Alarm indication followed by the Alarm-ACK REQ
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_ALARM_RSP);
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB,    this, CIOCEventHandler::EventDeviceReceived );
        eps_user_request_lower( pAckRQB, this, CIOCEventHandler::AlarmAckDone );
    }
    else
    {
        if(pAlarm) {

            pndAlarm->alarm.AlarmType        = PND_ConvertAlarmTypeToHostLibType(pAlarm->alarm_type);
            pndAlarm->alarm.AlarmPriority    = (PNIO_APRIO_TYPE)pAlarm->alarm_priority;
            pndAlarm->alarm.DeviceNum        = pAlarm->device_nr;
            pndAlarm->alarm.SlotNum          = pAlarm->slot_nr;
            pndAlarm->alarm.SubslotNum       = pAlarm->subslot_nr;

            // AlarmAinfo only relevant on PNIO_ALARM_DEV_FAILURE / PNIO_ALARM_DEV_RETURN
            pndAlarm->alarm.AlarmAinfo.Api   = pAlarm->api;

            pndAlarm->alarm.AlarmTinfo.ProfileType     = PNIO_AL_PROFILE_TYPE;
            pndAlarm->alarm.AlarmTinfo.AinfoType       = PNIO_AL_AINFO_TYPE;
            pndAlarm->alarm.AlarmTinfo.ControllerFlags = PNIO_AL_CTRL_FLAGS;
            pndAlarm->alarm.AlarmTinfo.DeviceFlag      = pAlarm->ar_diagnosis_state;


            pndAlarm->alarm.AlarmTinfo.PnioVendorIdent =  m_pDevice->GetVendorID();; // pLESrc->al_tinfo.pnio_vendor_ident;
            pndAlarm->alarm.AlarmTinfo.PnioDevIdent    =  m_pDevice->GetDeviceID(); // pLESrc->al_tinfo.pnio_dev_ident;
            pndAlarm->alarm.AlarmTinfo.PnioDevInstance =  m_pDevice->GetInstanceID();// pLESrc->al_tinfo.pnio_dev_instance;

            // set values for ainfo
            // DiagDs only for Diagnosisalarm-OB (OB 82) 
            pnd_memset (&(pndAlarm->alarm.DiagDs), 0, sizeof(pndAlarm->alarm.DiagDs));

            if ( pndAlarm->alarm.AlarmType == PNIO_ALARM_PLUG )
            {
                 CM_UPPER_ALARM_PTR_TYPE pAlarmPlug = pRQB->args.channel.alarm;
         
                 // Set the submod to GOOD
                 CIOCSubslot *pSub = m_pDevice->GetSubslot( pAlarmPlug->api, pAlarmPlug->slot_nr, pAlarmPlug->subslot_nr );

                if ( pSub != 0 )
                {
                     pSub->SetState( CM_SUB_STATE_GOOD );
                }

                // fill pnio compatible modul type, relevant for all plug/pull alarms
                if (pAlarmPlug->mod_ident == pSub->GetSlotIdent())
                {
                    pndAlarm->alarm.PnioCompatModtype             = PNIO_AL_REAL_EQUAL_EXPECT;

                }
                else
                {
                    pndAlarm->alarm.PnioCompatModtype             = PNIO_AL_REAL_NOT_EXPECT;

                }
            }
            
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

            PND_ASSERT(pAlarm->alarm_data_length >= PNIO_AOFF_USERSTRUCTUREIDENTIFIER);

            if ((pndAlarm->alarm.AlarmType != PNIO_ALARM_DEV_FAILURE) && (pndAlarm->alarm.AlarmType != PNIO_ALARM_DEV_RETURN))
            {
                if ( pAlarm->alarm_data_length >= PNIO_AOFF_USERSTRUCTUREIDENTIFIER )
                {
                    PND_IOC_BLOCKTYPE_TYPE block_access;

                    pnd_memset(&block_access,0,sizeof(PND_IOC_BLOCKTYPE_TYPE));

                    pnd_memcpy (&block_access, pAlarm->alarm_data, CM_ALARM_OFFSET_DATA);

                    pndAlarm->alarm.AlarmAinfo.BlockType    =  PSI_NTOH16( block_access.blockType );
                    pndAlarm->alarm.AlarmAinfo.BlockVersion =  PSI_NTOH16( block_access.blockVer );
                    
                    pndAlarm->alarm.AlarmAinfo.Api      =  PSI_NTOH32( block_access.Api );
                    pndAlarm->alarm.AlarmAinfo.ModIdent =  PSI_NTOH32( block_access.ModIdent );
                    pndAlarm->alarm.AlarmAinfo.SubIdent =  PSI_NTOH32( block_access.SubIdent );

                    PND_ASSERT(pAlarm->alarm_type  ==  PSI_NTOH16( block_access.AlarmType ));
                    PND_ASSERT(pAlarm->slot_nr     ==  PSI_NTOH16( block_access.slot ));
                    PND_ASSERT(pAlarm->subslot_nr  ==  PSI_NTOH16( block_access.subslot ));


                    pndAlarm->alarm.AlarmAinfo.AlarmSpecifier =   PSI_NTOH16( block_access.AlarmSpecifier );

                    if (pAlarm->alarm_data_length >= PNIO_ALARM_NORM_HEADER_OFFSETS_LEN) 
                    {
                        block_access.UserStructureIdentifier      =   PSI_NTOH16(  block_access.UserStructureIdentifier );
                        pndAlarm->alarm.AlarmAinfo.UserStrucIdent = block_access.UserStructureIdentifier;

                        pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen =
                            pAlarm->alarm_data_length - PNIO_ALARM_NORM_HEADER_OFFSETS_LEN;

                        if(pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen > sizeof(pndAlarm->alarm.AlarmAinfo.UAData))
                        {
                            pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen = sizeof(pndAlarm->alarm.AlarmAinfo.UAData);
                        }

                        memcpy(&pndAlarm->alarm.AlarmAinfo.UAData.UserAlarmData, pAlarm->alarm_data + PNIO_ALARM_NORM_HEADER_OFFSETS_LEN, pndAlarm->alarm.AlarmAinfo.UserAlarmDataLen);

                        switch(pndAlarm->alarm.AlarmAinfo.UserStrucIdent)
                        {
                        case 0x8000: // ChannelDiagnosis
                            {
                                pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelErrorType  = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelErrorType);
                                pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelNumber     = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelNumber);
                                pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelProperties = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.diag.ChannelProperties);
                                break;
                            }

                        case 0x8002: // ExtChannelDiagnosis
                            {
                                pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelErrorType    = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelErrorType);
                                pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelNumber       = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelNumber);
                                pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelProperties   = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ChannelProperties);
                                pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelErrorType = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelErrorType);
                                pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelAddValue  = PSI_NTOH32(pndAlarm->alarm.AlarmAinfo.UAData.ext_diag.ExtChannelAddValue);
                                break;
                            }

                        case 0x8100:    // Maintenance
                            {
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelErrorType  = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelErrorType);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelNumber     = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelNumber);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelProperties = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ChannelProperties);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.UserStrucIdent    = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.UserStrucIdent);

                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockLength = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockLength);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockType   = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.BlockHeader.BlockType);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.MaintenanceStatus       = PSI_NTOH32(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.maintInfo.MaintenanceStatus);
                                
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelErrorType = PSI_NTOH16(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelErrorType);
                                pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelAddValue  = PSI_NTOH32(pndAlarm->alarm.AlarmAinfo.UAData.mext_diag.ExtChannelAddValue);
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
        }

        // deleted in Function: LSA_VOID CIOCUser::AlarmReProvide( PND_RQB_PTR_TYPE p_rb ) 
        PND_ALARM_SESSION_PTR_TYPE SessionData = new PND_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
        pUser->AlarmIndication(p_Rqb);
    }
}

LSA_VOID CIOCEventHandler::ArAlarmIndicationDone(CM_UPPER_RQB_PTR_TYPE p_rb )
{

    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    if (CM_RQB_GET_OPCODE( pRQB ) == CM_OPC_CL_ALARM_IND)
    {
        CM_UPPER_ALARM_PTR_TYPE pAlarm = pRQB->args.channel.alarm;
        PND_ASSERT( pAlarm != 0 );
        PND_ASSERT( m_pDevice->GetNumber() == pAlarm->device_nr );

        LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();

        CM_UPPER_RQB_PTR_TYPE pAckRQB = getAlarmAckRqb( pAlarm, CM_PNIO_ERR_NONE );
        PND_ASSERT( pAckRQB != 0 );

        // Respond the Alarm indication followed by the Alarm-ACK REQ
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_ALARM_RSP);
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB,    this, CIOCEventHandler::EventDeviceReceived );
        eps_user_request_lower( pAckRQB, this, CIOCEventHandler::AlarmAckDone );
    }
    else
    {
        

        switch (CM_RQB_GET_OPCODE( pRQB ))
        {
            case CM_OPC_CL_AR_IN_DATA_IND:
                {
                    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();

                    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_IN_DATA_RSP );
                    CM_RQB_SET_HANDLE( pRQB, handle );

                    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
                    break;
                }
            case CM_OPC_CL_AR_OFFLINE_IND:
                {
                    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();
                    
                    // Respond indication with correct opcode
                    // zum Beispiel
                    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_OFFLINE_RSP );
                    CM_RQB_SET_HANDLE( pRQB, handle );

                    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
                    break;
                }
            case CM_OPC_CL_AR_PRE_OFFLINE_IND:
                {
                    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();
                    
                    // Respond indication with correct opcode
                    // zum Beispiel
                    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRE_OFFLINE_RSP );
                    CM_RQB_SET_HANDLE( pRQB, handle );

                    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
                    break;
                }
            case CM_OPC_CL_AR_ABORT_IND:
                {
                    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();
                    
                    // Respond indication with correct opcode
                    // zum Beispiel
                    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_ABORT_RSP );
                    CM_RQB_SET_HANDLE( pRQB, handle );

                    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
                    break;
                }
            default:
                {
                    PND_FATAL("wrong opcode");
                    break;
                }
        }

    } // end else

}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArAbortIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_ABORT_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );
    
    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0 );

    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArAbortIndication():====> AR_ABORT.IND dev(%u) ar(%u) session_key(%u) reason_Code(%#x) RQB(0x%08x)",
        pEvent->device_nr, 
        pEvent->ar_nr, 
        pEvent->session_key,
        pEvent->u.cl.ar_abort.error_cause.reason_code,
        pRQB );

	// Set diagnose parameters
	m_pDevice->SetDeviceDiagState( 
		pEvent->u.cl.ar_abort.error_cause.error_cause,
		pEvent->u.cl.ar_abort.error_cause.reason_code,
		&pEvent->u.cl.ar_abort.error_cause.additional_info[0] );

    traceAbortReason( &pEvent->u.cl.ar_abort );

    // Check valid transistion, update AR- and submod state(reset all to GOOD )
    PND_ASSERT( arObj->GetArState() == PND_IOC_AR_STATE_START );

    traceAbortReason( &pEvent->u.cl.ar_abort );

    m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );

    // Note: In case of activated, CM CL starts automatic the AR connection
    // Only in case of User remove or passivated, the state is offline before passivated
    switch ( pEvent->u.cl.ar_abort.error_cause.reason_code )
    {
    case CM_AR_REASON_RMV: // IOC abort --> Device Remove ?
    case CM_AR_REASON_PAS: // IOC abort --> Device Passivate ?
        // Set to Offine --> change to possible passivated is done at Passivate.Cnf
        arObj->SetArState( PND_IOC_AR_STATE_OFFLINE );
        break;
    default:
        // Set to restart connection
        arObj->SetArState( PND_IOC_AR_STATE_START );
        break;
    }

        // prepare response indication
        // note: is reprovided after async PI requests finished
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_ABORT_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );

#if 0
    // get alarm resource from CP
    CIOCUser *pUser = dynamic_cast<CIOCUser*>(m_pDevice->GetParent());
    PND_ASSERT( pUser != 0 );
    PND_RQB_PTR_TYPE p_Rqb = pUser->GetAlarmResource();

    if (!p_Rqb)
    {
        // prepare response indication
        // note: is reprovided after async PI requests finished
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_ABORT_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }
    else
    {
        p_Rqb->args.iob_alarm.event_ind.event_opcode = CM_RQB_GET_OPCODE( pRQB );
        p_Rqb->args.iob_alarm.event_ind.device_nr = pEvent->device_nr;
        p_Rqb->args.iob_alarm.event_ind.ar_nr = pEvent->ar_nr;
        p_Rqb->args.iob_alarm.event_ind.session_key = pEvent->session_key;

        // p_Rqb->args.iob_alarm.alarm.AlarmType = 0;
        p_Rqb->args.iob_alarm.alarm.DeviceNum = pEvent->device_nr;

        PND_ALARM_SESSION_PTR_TYPE SessionData = new PND_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
        pUser->AlarmIndication(p_Rqb);
    }
#endif //0
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArPreOfflineIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_PRE_OFFLINE_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );
    
    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0 );

    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArPreOfflineIndication():====> AR_PRE_OFFLINE.IND dev(%u) ar(%u) session_key(%u) reasonCode(%#x) RQB(0x%08x)",
        pEvent->device_nr, 
        pEvent->ar_nr, 
        pEvent->session_key,
        pEvent->u.cl.ar_abort.error_cause.reason_code,
        pRQB );

	// Set diagnose parameters
	m_pDevice->SetDeviceDiagState( 
		pEvent->u.cl.ar_abort.error_cause.error_cause,
		pEvent->u.cl.ar_abort.error_cause.reason_code,
		&pEvent->u.cl.ar_abort.error_cause.additional_info[0] );

    traceAbortReason( &pEvent->u.cl.ar_abort );

    // Check valid transistion, update AR- and submod state(reset all to GOOD )
    PND_ASSERT( arObj->GetArState() == PND_IOC_AR_STATE_PREDATA );

    m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );

    // Note: In case of activated, CM CL starts automatic the AR connection
    // Only in case of User remove or passivated, the state is offline before passivated
    switch ( pEvent->u.cl.ar_abort.error_cause.reason_code )
    {
    case CM_AR_REASON_RMV: // IOC abort --> Device Remove ?
    case CM_AR_REASON_PAS: // IOC abort --> Device Passivate ?
        // Set to Offine --> change to possible passivated is done at Passivate.Cnf
        arObj->SetArState( PND_IOC_AR_STATE_OFFLINE );
        break;
    default:
        // Set to restart connection
        arObj->SetArState( PND_IOC_AR_STATE_START );
        break;
    }



        // prepare response indication
        // note: is reprovided after async PI requests finished
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRE_OFFLINE_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );

#if 0

    CIOCUser *pUser = dynamic_cast<CIOCUser*>(m_pDevice->GetParent());
    PND_ASSERT( pUser != 0 );

    PND_RQB_PTR_TYPE p_Rqb = pUser->GetAlarmResource();

    if (!p_Rqb)
    {
        // prepare response indication
        // note: is reprovided after async PI requests finished
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRE_OFFLINE_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }
    else
    {
        p_Rqb->args.iob_alarm.event_ind.event_opcode = CM_RQB_GET_OPCODE( pRQB );
        p_Rqb->args.iob_alarm.event_ind.device_nr = pEvent->device_nr;
        p_Rqb->args.iob_alarm.event_ind.ar_nr = pEvent->ar_nr;
        p_Rqb->args.iob_alarm.event_ind.session_key = pEvent->session_key;

        PND_ALARM_SESSION_PTR_TYPE SessionData = new PND_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
        pUser->AlarmIndication(p_Rqb);
    }
#endif //0
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArPreDataIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    CValueHelper valObj;

    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_PRE_DATA_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );
    
    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    PND_IOC_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArPreDataIndication():====> PRE DATA.IND dev(%u) ar(%u) session_key(%u) RQB(0x%08x)",
        pEvent->device_nr, 
        pEvent->ar_nr, 
        pEvent->session_key,
        pRQB );

    // Update device tree from diff list
    // Note: the BGZD updates are triggered by subslots
    m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
    m_pDevice->UpdateFromDiffList( &pEvent->u.cl.in_data.diff_list);

    // Check valid transistion, update AR-State
    PND_ASSERT( arObj->GetArState() == PND_IOC_AR_STATE_START);
    arObj->SetArState( PND_IOC_AR_STATE_PREDATA );

    // Respond indication
    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRE_DATA_RSP );
    CM_RQB_SET_HANDLE( pRQB, handle );

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArInDataIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    CValueHelper valObj;

    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_IN_DATA_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );

    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    PND_IOC_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArInDataIndication():====> AR_IN_DATA.IND dev(%u) ar(%u) session_key(%u) RQB(0x%08x)",
        pEvent->device_nr, 
        pEvent->ar_nr, 
        pEvent->session_key,
        pRQB );

	// Reset diagnose parameters
	m_pDevice->SetDeviceDiagState( CM_AR_REASON_NONE, CM_ERROR_CAUSE_NONE, LSA_NULL);

    // Update device tree from diff list
    // Note: the BGZD changed are triggered by the submods
    m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
    m_pDevice->UpdateFromDiffList( &pEvent->u.cl.in_data.diff_list );

    // Check valid transistion, update AR-State
    PND_ASSERT(( arObj->GetArState() == PND_IOC_AR_STATE_START ) || (arObj->GetArState() == PND_IOC_AR_STATE_PREDATA));
    arObj->SetArState( PND_IOC_AR_STATE_INDATA );
    arObj->SetSessionKey( pEvent->session_key );

    CIOCUser *pUser = dynamic_cast<CIOCUser*>(m_pDevice->GetParent());
    PND_ASSERT( pUser != 0 );

    PND_RQB_PTR_TYPE p_Rqb = pUser->GetAlarmResource();

    if (!p_Rqb)
    {
        // prepare response indication
        // note: is reprovided after async PI requests finished
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_IN_DATA_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }
    else
    {
        p_Rqb->args.iob_alarm.event_ind.event_opcode = CM_RQB_GET_OPCODE( pRQB );
        p_Rqb->args.iob_alarm.event_ind.device_nr = pEvent->device_nr;
        p_Rqb->args.iob_alarm.event_ind.ar_nr = pEvent->ar_nr;
        p_Rqb->args.iob_alarm.event_ind.session_key = pEvent->session_key;

        p_Rqb->args.iob_alarm.alarm.AlarmType = PNIO_ALARM_DEV_RETURN;
        p_Rqb->args.iob_alarm.alarm.DeviceNum = pEvent->device_nr;

        p_Rqb->args.iob_alarm.alarm.SlotNum    = 0;
        p_Rqb->args.iob_alarm.alarm.SubslotNum = 0;


        PND_ALARM_SESSION_PTR_TYPE SessionData = new PND_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
        pUser->AlarmIndication(p_Rqb);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArOfflineIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_OFFLINE_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );

    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    PND_IOC_TRACE_05( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArOfflineIndication():====> AR_OFFLINE.IND dev(%u) ar(%u) session_key(%u) reason_code(%#x) RQB(0x%08x)",
        pEvent->device_nr, 
        pEvent->ar_nr, 
        pEvent->session_key,
        pEvent->u.cl.ar_abort.error_cause.reason_code,
        pRQB );

	// Set diagnose parameters
	m_pDevice->SetDeviceDiagState( 
		pEvent->u.cl.ar_abort.error_cause.error_cause,
		pEvent->u.cl.ar_abort.error_cause.reason_code,
		&pEvent->u.cl.ar_abort.error_cause.additional_info[0] );

    traceAbortReason( &pEvent->u.cl.ar_abort );
    // Check valid transistion, update AR- and submod state
    PND_ASSERT (arObj->GetArState() == PND_IOC_AR_STATE_INDATA);
    m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
    arObj->SetSessionKey( pEvent->session_key );

    // Note: In case of activated, CM CL starts automatic the AR connection
    // Only in case of User remove or passivated, the state is offline before passivated
    switch ( pEvent->u.cl.ar_abort.error_cause.reason_code )
    {
    case CM_AR_REASON_RMV: // IOC abort --> Device Remove ?
    case CM_AR_REASON_PAS: // IOC abort --> Device Passivate ?
        // Set to Offine --> change to possible passivated is done at Passivate.Cnf
        arObj->SetArState( PND_IOC_AR_STATE_OFFLINE );
        break;
    default:
        // Set to restart connection
        arObj->SetArState( PND_IOC_AR_STATE_START );
        break;
    }

    CIOCUser *pUser = dynamic_cast<CIOCUser*>(m_pDevice->GetParent());
    PND_ASSERT( pUser != 0 );

    PND_RQB_PTR_TYPE p_Rqb = pUser->GetAlarmResource();

    if (!p_Rqb)
    {
        // Respond indication and update GUI8
        CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_OFFLINE_RSP );
        CM_RQB_SET_HANDLE( pRQB, handle );

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }
    else
    {
        p_Rqb->args.iob_alarm.event_ind.event_opcode = CM_RQB_GET_OPCODE( pRQB );
        p_Rqb->args.iob_alarm.event_ind.device_nr   = pEvent->device_nr;
        p_Rqb->args.iob_alarm.event_ind.ar_nr       = pEvent->ar_nr;
        p_Rqb->args.iob_alarm.event_ind.session_key = pEvent->session_key;

        p_Rqb->args.iob_alarm.alarm.AlarmType = PNIO_ALARM_DEV_FAILURE;
        p_Rqb->args.iob_alarm.alarm.DeviceNum = pEvent->device_nr;

        p_Rqb->args.iob_alarm.alarm.SlotNum    = 0;
        p_Rqb->args.iob_alarm.alarm.SubslotNum = 0;

        PND_ALARM_SESSION_PTR_TYPE SessionData = new PND_ALARM_SESSION_TYPE;
            
        SessionData->pCmRqb     = pRQB;
        SessionData->pevHandler = this;
        
        p_Rqb->args.iob_alarm.SessionData = SessionData;

        // else pass AlarmRQB up to PNIO User Core
        pUser->AlarmIndication(p_Rqb);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArPrmBeginIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_PRM_BEGIN_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );

    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    if( pEvent->u.cl.prm_begin.prm_mode == CM_CL_PRMMODE_AR)
    {
        PND_IOC_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "CIOCEventHandler::ArPrmBeginIndication():===> AR-PRM-BEGIN.IND for AR, dev(%u) ar(%u) session_key(%u) RQB(0x%08x)",
            pEvent->device_nr,
            pEvent->ar_nr,
            pEvent->session_key,
            pRQB );
    }
    else
    {
        PND_IOC_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "===> AR-PRM-BEGIN.IND for SUBMODULE, dev(%u) ar(%u) session_key(%u) api/slot/sub(%u/%u/%#x) RQB(0x%08x)",
            pEvent->device_nr,
            pEvent->ar_nr,
            pEvent->session_key,
            pEvent->u.cl.prm_begin.prm_submodule.api,
            pEvent->u.cl.prm_begin.prm_submodule.slot_nr, 
            pEvent->u.cl.prm_begin.prm_submodule.subslot_nr,
            pRQB );
    }

    // Respond indication
    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRM_BEGIN_RSP);
    CM_RQB_SET_HANDLE( pRQB, handle );

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArPrmInDataIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_PRM_INDATA_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );

    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    if( pEvent->u.cl.prm_begin.prm_mode == CM_CL_PRMMODE_AR )
    {
        PND_IOC_TRACE_04( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "CIOCEventHandler::ArPrmInDataIndication():===> AR-PRM-INDATA.IND for AR, dev(%u) ar(%u) session_key(%u) RQB(0x%08x) ",
            pEvent->device_nr,
            pEvent->ar_nr,
            pEvent->session_key,
            pRQB );

        // Update device tree and AR state from diff list
        // Note: the BGZD changes are triggered by the submods
        m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
        m_pDevice->UpdateFromDiffList( &pEvent->u.cl.prm_indata.diff_list );
    }
    else
    {
        PND_IOC_TRACE_07( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "===> AR-PRM-INDATA.IND for SUBMODULE, dev(%u) ar(%u) session_key(%u) api/slot/sub(%u/%u/%#x) RQB(0x%08x)",
            pEvent->device_nr,
            pEvent->ar_nr,
            pEvent->session_key,
            pEvent->u.cl.prm_indata.prm_submodule.api,
            pEvent->u.cl.prm_indata.prm_submodule.slot_nr, 
            pEvent->u.cl.prm_indata.prm_submodule.subslot_nr,
            pRQB );

        if ( CmListIsEmpty (&pEvent->u.cl.prm_indata.diff_list) ) // Empty list ?
        {
            // Set the submod to GOOD
            m_pDevice->ResetSubslots( CM_SUB_STATE_GOOD );
        }
        else
        {
            // Set new state for all submod from diff list entry (this submod)
            // Note: the BGZD changes are triggered by the submods
            m_pDevice->UpdateFromDiffList( &pEvent->u.cl.prm_indata.diff_list );
        }
    }

    // Respond indication
    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_PRM_INDATA_RSP );
    CM_RQB_SET_HANDLE( pRQB, handle );

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::ArDataStatusIndication( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_ASSERT( CM_RQB_GET_OPCODE( pRQB )   == CM_OPC_CL_AR_DATA_STATUS_IND );
    PND_ASSERT( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK );

    CM_UPPER_EVENT_PTR_TYPE pEvent = pRQB->args.channel.event;
    PND_ASSERT( pEvent != 0 );
    PND_ASSERT( m_pDevice->GetNumber() == pEvent->device_nr );

    CIOCAr *arObj = m_pDevice->GetAr();
    PND_ASSERT( arObj != 0);

    PND_IOC_TRACE_06( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::ArDataStatusIndication():===> AR_DATA_STATUS.IND dev/ar(%u/%u) session_key(%u) data_status(%#u/%#x) RQB(0x%08x)",
        pEvent->device_nr,
        pEvent->ar_nr,
        pEvent->session_key,
        pEvent->u.cl.data_status, pEvent->u.cl.data_status,
        pRQB );

    // update DS state
    arObj->SetDataStatus( pEvent->u.cl.data_status );

    // Respond indication
    CM_RQB_SET_OPCODE( pRQB, CM_OPC_CL_AR_DATA_STATUS_RSP );
    CM_RQB_SET_HANDLE( pRQB, handle );

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
}

/////////////////////////////////////////////////////////////////////////////
// Requests
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCEventHandler::StartUserListening()
{
    PND_ASSERT( m_pUser != 0 );

    LSA_HANDLE_TYPE  handle  = m_pUser->GetPnioHandle();

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::StartUserListening(): Providing CM CL user event resources, PNIO-IFNr(%u)",
        m_pndHandle.pnio_if_nr );

    m_isUserStopping = LSA_FALSE;

    // Provide overload event resource
    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_OVERLOAD_INFO, handle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    pRQB->args.channel.overload->send_overload_counter = 0;
    pRQB->args.channel.overload->recv_overload_counter = 0;

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventUserReceived );

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::StartUserListening(): Providing IOC User resources done, PNIO-IFNr(%u)", 
        m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::StopUserListening()
{
    // Note: i.E.: is called if closing the channel is requested
    //       to prevent reproviding user IND RQBs after Close
    m_isUserStopping = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::StartDeviceListening( LSA_UINT16 nrOfEvents, LSA_UINT16 nrOfAlarms )
{
    PND_ASSERT( m_pDevice != 0 );

    LSA_HANDLE_TYPE    handle = m_pDevice->GetPnioHandle();

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::StartDeviceListening():Providing CM CL device resources, dev(%u) events(%u) alarms high/low(%u)", 
        m_pDevice->GetNumber(), 
        nrOfEvents,
        nrOfAlarms );

    PND_ASSERT( nrOfEvents > 0 );
    PND_ASSERT( nrOfAlarms > 0 );

    // Provide event resources
    for ( int lvi = 0; lvi < nrOfEvents; lvi++ )
    {
        CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_PROVIDE_EVENT, handle, &m_pndHandle, 0 );
        PND_ASSERT(pRQB);

        pRQB->args.channel.event->device_nr = (LSA_UINT16)m_pDevice->GetNumber();

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }

    // Provide alarm high/low resources
    for ( int lvi = 0; lvi < nrOfAlarms; lvi++ )
    {
        CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DEVICE_PROVIDE_ALARM, handle, &m_pndHandle, 0 );
        PND_ASSERT(pRQB);

        pRQB->args.channel.alarm->device_nr      = (LSA_UINT16)m_pDevice->GetNumber();
        pRQB->args.channel.alarm->ar_nr          = 0; // don't care
        pRQB->args.channel.alarm->alarm_priority = 0; // don't care

        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
    }

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::StartUserListening():Providing IOC device resources done, PNIO-IFNr(%u)", 
        m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::HandleEvent( LSA_OPCODE_TYPE opcode, CM_UPPER_RQB_PTR_TYPE pRQB )
{
    // Delegate to the registered common Event Handler method
    PND_ASSERT( pRQB != 0);
    PND_ASSERT(( opcode > 0 ) && ( opcode < CM_OPC_MAX ));

    (this->*m_handler[opcode])( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::RespondIndication( LSA_VOID*p_rb )
{
    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)p_rb;
    PND_ASSERT( pRQB != 0);

    // Respond indication after async complete request
    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
        "CIOCEventHandler::RespondIndication():CM CL RQB responded, opc(%u/%#x) pRQB(0x%08x)", 
        CM_RQB_GET_OPCODE( pRQB ),
        CM_RQB_GET_OPCODE( pRQB ),
        pRQB );

    eps_user_request_lower( pRQB, this, CIOCEventHandler::EventDeviceReceived );
}

/////////////////////////////////////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////////////////////////////////////

LSA_VOID CIOCEventHandler::EventUserReceived(LSA_VOID*p_inst, LSA_VOID*p_rb)
{
    CIOCEventHandler *eventObj = static_cast<CIOCEventHandler*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( eventObj           != 0 );
    PND_ASSERT( pRQB               != 0 );
    PND_ASSERT( eventObj->m_pUser  != 0 );

    if ( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK_CANCELLED )
    {
        // IND resource is canceled -- stop requesting in case of shutdown
        PND_ASSERT( eventObj->m_pUser->IsShutdown() );

        PND_IOC_TRACE_01( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
            "CIOCEventHandler::EventReceived():CM CL RQB cancelled, pRQB(0x%08x)", 
            pRQB );

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
LSA_VOID CIOCEventHandler::EventDeviceReceived(LSA_VOID*p_inst, LSA_VOID*p_rb)
{
    CIOCEventHandler *eventObj = static_cast<CIOCEventHandler*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( eventObj            != 0 );
    PND_ASSERT( pRQB                != 0 );
    PND_ASSERT( eventObj->m_pDevice != 0 );

    if ( CM_RQB_GET_RESPONSE( pRQB ) == CM_OK_CANCELLED )
    {
        // IND resource is canceled -- stop requesting in case of removing (shutdown)
        PND_ASSERT( eventObj->m_pDevice->IsShutdown() );

        PND_IOC_TRACE_01( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
            "CIOCEventHandler::EventReceived():CM CL RQB cancelled, pRQB(0x%08x)", 
            pRQB );

        pnd_cm_free_rqb( pRQB, &eventObj->m_pndHandle );
    }
    else
    {
        // Handle the event --> use event function map for handle the event
        LSA_OPCODE_TYPE opcode = CM_RQB_GET_OPCODE( pRQB );

        // Prepare response and call event handler for Opcode
        LSA_HANDLE_TYPE handle = eventObj->m_pDevice->GetPnioHandle();

        // Prepare response and call event handler for Opcode
        CM_RQB_SET_HANDLE( pRQB, handle );
        eventObj->HandleEvent( opcode, pRQB );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::AlarmAckDone( LSA_VOID*p_inst, LSA_VOID*p_rb )
{
    CIOCEventHandler *eventObj = static_cast<CIOCEventHandler*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( eventObj            != 0 );
    PND_ASSERT( pRQB                != 0 );
    PND_ASSERT( eventObj->m_pDevice != 0 );

    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_ALARM_ACK_SEND );

    CM_UPPER_ALARM_PTR_TYPE pAlarm = pRQB->args.channel.alarm;
    PND_ASSERT( pAlarm != 0 );

    PND_IOC_TRACE_07( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::AlarmAckDone():CM-CL ALARM-ACK.RSP, rsp(%u/%#x) dev(%u) ar(%u) skey(%u) alarm-type(%u/%#x)",
        CM_RQB_GET_RESPONSE(pRQB),
        CM_RQB_GET_RESPONSE(pRQB),
        pAlarm->device_nr, 
        pAlarm->ar_nr, 
        pAlarm->session_key, 
        pAlarm->alarm_type,
        pAlarm->alarm_type );

    PND_IOC_TRACE_06( eventObj->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCEventHandler::AlarmAckDone():  prio(%u) seq(%u) ap/sl/su(%u/%u/%u) pnio(%#x)",
        pAlarm->alarm_priority, 
        pAlarm->alarm_sequence, 
        pAlarm->api, 
        pAlarm->slot_nr,
        pAlarm->subslot_nr, 
        pAlarm->cm_pnio_err );

    pnd_cm_free_rqb( pRQB, &eventObj->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCEventHandler::initUserMap()
{
    // Initialize the Event-Handler list
    for ( int lvi = 0; lvi < CM_OPC_MAX; lvi++ )
    {
        m_handler[lvi] = &CIOCEventHandler::ErrorUserEventHandling;
    }

    // Set the CM user event methodes corresponding to CM Event numbers
    m_handler[CM_OPC_OVERLOAD_INFO] = &CIOCEventHandler::OverloadIndication;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::initDeviceMap()
{
    // Initialize the Event-Handler list
    for ( int lvi = 0; lvi < CM_OPC_MAX; lvi++ )
    {
        m_handler[lvi] = &CIOCEventHandler::ErrorDeviceEventHandling;
    }

    // Set the CM device event methodes corresponding to CM Event numbers
    m_handler[CM_OPC_CL_ALARM_IND]          = &CIOCEventHandler::ArAlarmIndication;
    m_handler[CM_OPC_CL_AR_ABORT_IND]       = &CIOCEventHandler::ArAbortIndication;
    m_handler[CM_OPC_CL_AR_PRE_DATA_IND]    = &CIOCEventHandler::ArPreDataIndication;
    m_handler[CM_OPC_CL_AR_PRE_OFFLINE_IND] = &CIOCEventHandler::ArPreOfflineIndication;
    m_handler[CM_OPC_CL_AR_IN_DATA_IND]     = &CIOCEventHandler::ArInDataIndication;
    m_handler[CM_OPC_CL_AR_OFFLINE_IND]     = &CIOCEventHandler::ArOfflineIndication;
    m_handler[CM_OPC_CL_AR_PRM_BEGIN_IND]   = &CIOCEventHandler::ArPrmBeginIndication;
    m_handler[CM_OPC_CL_AR_PRM_INDATA_IND]  = &CIOCEventHandler::ArPrmInDataIndication;
    m_handler[CM_OPC_CL_AR_DATA_STATUS_IND] = &CIOCEventHandler::ArDataStatusIndication;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::reprovideUserInd( CM_UPPER_RQB_PTR_TYPE pRQB )
{
    if ( m_isUserStopping )  // Closing CM CL channel in progress ?
    {
        // free RQB
        pnd_cm_free_rqb( pRQB, &m_pndHandle );
    }
    else
    {
        // Reprovide RQB for next indication
        eps_user_request_lower( pRQB, this, CIOCEventHandler::EventUserReceived );
    }
}

/*----------------------------------------------------------------------------*/
CM_UPPER_RQB_PTR_TYPE CIOCEventHandler::getAlarmAckRqb( CM_UPPER_ALARM_PTR_TYPE pAlarm, LSA_UINT32 pnioErr )
{
    CM_UPPER_RQB_PTR_TYPE pRQB;

    PND_ASSERT( pAlarm != 0 );

    LSA_HANDLE_TYPE  handle = m_pDevice->GetPnioHandle();

    // prepare Alarm ACK
    pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_ALARM_ACK_SEND, handle, &m_pndHandle, 0);
    PND_ASSERT( pRQB != 0 );

    pRQB->args.cl.alarm_ack_send->device_nr              = pAlarm->device_nr;
    pRQB->args.cl.alarm_ack_send->ar_nr                  = pAlarm->ar_nr;
    pRQB->args.cl.alarm_ack_send->session_key            = pAlarm->session_key;
    pRQB->args.cl.alarm_ack_send->api                    = pAlarm->api;
    pRQB->args.cl.alarm_ack_send->alarm_priority         = pAlarm->alarm_priority;
    pRQB->args.cl.alarm_ack_send->alarm_type             = pAlarm->alarm_type;
    pRQB->args.cl.alarm_ack_send->slot_nr                = pAlarm->slot_nr;
    pRQB->args.cl.alarm_ack_send->subslot_nr             = pAlarm->subslot_nr;
    pRQB->args.cl.alarm_ack_send->alarm_sequence         = pAlarm->alarm_sequence;
    pRQB->args.cl.alarm_ack_send->diag_channel_available = pAlarm->diag_channel_available;
    pRQB->args.cl.alarm_ack_send->diag_generic_available = pAlarm->diag_generic_available;
    pRQB->args.cl.alarm_ack_send->diag_submod_available  = pAlarm->diag_submod_available;
    pRQB->args.cl.alarm_ack_send->ar_diagnosis_state     = pAlarm->ar_diagnosis_state;
    pRQB->args.cl.alarm_ack_send->reserved               = pAlarm->reserved;
    pRQB->args.cl.alarm_ack_send->cm_pnio_err            = pnioErr;

    return ( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCEventHandler::traceAbortReason( CM_UPPER_CL_AR_ABORT_PTR_TYPE pAbort )
{
    PND_ASSERT( pAbort != 0 );

    string causeStr = "unknown";

    PND_ASSERT( pAbort != 0 );

    LSA_UINT8* pData;      // the PDU buffer for read/write
    LSA_UINT32 dataSize;   // PDU buffer size

    LSA_UINT16 error_cause;
    LSA_UINT32 pnio_status;


    // Extract PNIO status, and following info based on error cause
    pData       = pAbort->error_cause.additional_info;
    dataSize    = sizeof( pAbort->error_cause.additional_info );
    error_cause = pAbort->error_cause.error_cause;
    PND_SWAP_U16(&error_cause);

    LSA_UINT8 reason_code = pAbort->error_cause.reason_code;

    PNIO_UINT8 ErrCode;
    PNIO_UINT8 ErrDecode;
    PNIO_UINT8 ErrCode1;
    PNIO_UINT8 ErrCode2;

    pnd_memcpy( &ErrCode, &pData[0], 1 );
    pnd_memcpy( &ErrDecode, &pData[1], 1 );
    pnd_memcpy( &ErrCode1, &pData[2], 1 );
    pnd_memcpy( &ErrCode2, &pData[3], 1 );

    pnd_memcpy( &pnio_status, pData, sizeof(LSA_UINT32) );
    PND_SWAP_U32( &pnio_status );

    // note: see Clearcase-VOB/cm/doc/PNIO_CM_CL_Diagnosis_ErrorList.doc for this mapping
    switch( error_cause )
    {
    case CM_ERROR_CAUSE_NONE:
                PND_IOC_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE,"No problem, everything OK");
        break;

    case CM_ERROR_CAUSE_IP_MULTIPLE:
        {
            LSA_UINT8 mac[6];
            pnd_memcpy( &mac, &pData[sizeof(LSA_UINT32)], sizeof(mac));

            PND_IOC_TRACE_12( 0, LSA_TRACE_LEVEL_NOTE,"EC%u: IP address exists multiple times; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), mac_addr( %02x-%02x-%02x-%02x-%02x-%02x )",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] ); 
        }
        break;

    case CM_ERROR_CAUSE_NOS_MULTIPLE:
        {
            LSA_UINT8 mac[6];
            pnd_memcpy( &mac, &pData[sizeof(LSA_UINT32)], sizeof(mac));

            PND_IOC_TRACE_12( 0, LSA_TRACE_LEVEL_NOTE,"EC%u: NameOfStation exists multiple times; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), mac_addr( %02x-%02x-%02x-%02x-%02x-%02x )",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] ); 
        }
        break;

    case CM_ERROR_CAUSE_LINK_DOWN:
        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE,"EC%u: Link down; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }
        break;

    case CM_ERROR_CAUSE_MAU_MISMATCH:
        {
            PND_IOC_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MAUType mismatch; reason(%u), pnio_status(0x%x)",
                error_cause, reason_code, pnio_status );
        }
        break;

    case CM_ERROR_CAUSE_RPC_ERROR:
        {

            LSA_UINT32 nca_status = 0;
            pnd_memcpy( &nca_status, &pData, sizeof(LSA_UINT32) );

            PND_IOC_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: RPC protocoll error; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), nca_status(0x%x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, nca_status );
        }
        break;

    case CM_ERROR_CAUSE_PNIO_CONFIGURATION:
        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: PNIO configuration protocol error; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }
        break;

    case CM_ERROR_CAUSE_RPC_TIMEOUT:
        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: RPC application timeout; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }
        break;

    case CM_ERROR_CAUSE_IP_SET_ABORTED:
        {
            LSA_UINT8 mac[6];
            pnd_memcpy( &mac, &pData[sizeof(LSA_UINT32)], sizeof(mac));

            PND_IOC_TRACE_12( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: IP address assignment to IO device aborted; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), mac_addr( %02x-%02x-%02x-%02x-%02x-%02x )",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]  );
        }
        break;

    case CM_ERROR_CAUSE_IOD_NOT_FOUND:

        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: IO device not found; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }

        break;

    case CM_ERROR_CAUSE_OUT_OF_RESOURCES:

        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: Out of AR resources; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }
        break;

    case CM_ERROR_CAUSE_MISS:
        {
            LSA_UINT32 cycle_counter = 0;
            pnd_memcpy( &cycle_counter, &pData, sizeof(LSA_UINT16) );

            PND_IOC_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE," EC%u: WDT / DHT expired; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), cycle_counter(0x%x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, cycle_counter );
        }
        break;

    case CM_ERROR_CAUSE_NOS_SET_ABORTED:

        {

            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: Device with wrong NameOfStation found on topological position; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_IOC_ABORTED:

        {
            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: Controller local abort reason; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );
        }

        break;

    case CM_ERROR_CAUSE_IOD_ABORTED:

        {
            LSA_UINT32 remote_pnio_status = 0;
            pnd_memcpy( &remote_pnio_status, &pData, sizeof(LSA_UINT32) );

            PND_IOC_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: Device abort reason; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x), remote_pnio_status(0x%x)",
                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2, remote_pnio_status );

        }

        break;

    case CM_ERROR_CAUSE_SRD_ABORTED:

        {

            PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: SystemRedundancy abort reason; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

                error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_VENDORID_ABORTED:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: Device with wrong VendorID/DeviceID/Instance found on topological position; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_IN_PROGRESS:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, Machine- or AddressTailoring in progress; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_NOS_MISSING:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, NameOfStation adaption missing (PDev with invalid NoS); reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_IPv4_MISSING:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, IPv4Suite adaption missing (Pdev with invalid IPv4Suite); reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_NOS_INVALID:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject,tailored NameOfStation of IOD is invalid; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_IPv4_INVALID:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, tailored IPv4Suite of IOD is invalid; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_TAILORING_MISSING:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, MachineTailoring missing; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_PDEV_ERROR:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, PDev configuration error; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

    case CM_ERROR_CAUSE_MP_DEVICE_DISABLED:

        {

        PND_IOC_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "EC%u: MasterProject, disabled Device is activated; reason(%u), ErrCode(%02x), ErrDecode(%02x), ErrCode1(%02x), ErrCode2(%02x)",

        error_cause, reason_code, ErrCode, ErrDecode, ErrCode1, ErrCode2 );

        }

        break;

 


    default:
        //PND_ASSERT(0); //TODO Print: Unknown CM_ERROR_CAUSE
        break;
    }


}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
