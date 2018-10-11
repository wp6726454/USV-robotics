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
/*  F i l e               &F: pnd_IOCUser.cpp                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation for IOC (CM-CL) User                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3205
#define PND_MODULE_ID       3205

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
#include "pnd_IOCEventHandler.h"
#include "pnd_IOCDevice.h"
#include "pnd_IOCSubslot.h"
#include "pnd_IOCUser.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CIOCUser (CM-CL) user class
/////////////////////////////////////////////////////////////////////////////

CIOCUser::CIOCUser() : CUserNode()
{
    m_isCreated     = LSA_FALSE;
    m_pnioHandle    = CM_INVALID_HANDLE;
    m_sysPath       = PSI_SYS_PATH_INVALID;
    m_pEventHandler = 0;
    m_responseRQB   = 0;
    m_nrOfAlarmRes  = 0;
   m_mode          = IOC_INITIAL;
   m_modeRQB       = 0;

    m_requestList.clear();

    pnd_memset( &m_iocParam, 0, sizeof( PND_PARAM_IOC_HD_IF_TYPE ));
}

/*----------------------------------------------------------------------------*/
CIOCUser::CIOCUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent ) : CUserNode( pHandle, pParent )
{
    m_isCreated     = LSA_FALSE;
    m_pnioHandle    = CM_INVALID_HANDLE;
    m_sysPath       = PSI_SYS_PATH_INVALID;
    m_pEventHandler = 0;
    m_responseRQB   = 0;
   m_nrOfAlarmRes  = 0;
   m_mode          = IOC_INITIAL;
   m_modeRQB       = 0;

    m_requestList.clear();

    pnd_memset( &m_iocParam, 0, sizeof( PND_PARAM_IOC_HD_IF_TYPE ));
}

/*----------------------------------------------------------------------------*/
CIOCUser::~CIOCUser()
{
    if ( m_pEventHandler != 0 )
    {
        delete( m_pEventHandler );
        m_pEventHandler = 0;
    }

    // Free the stored elements in  device MAP
    deleteMap( m_deviceMap );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams )
{
    PND_PARAM_IOC_HD_IF_PTR_TYPE pIOC = (PND_PARAM_IOC_HD_IF_PTR_TYPE)pParams;
    PND_ASSERT( pIOC != 0 );
    PND_ASSERT( laddr_ptr != 0 );

	LSA_UINT32 result;

    m_iocParam = *pIOC;

    // setup internal attributes and create the IOD devices and subslots based on config 
    PND_ASSERT( m_pParentNode != 0 );
    PND_ASSERT(( m_pndHandle.hd_nr > 0 ) && ( m_pndHandle.hd_nr <= PSI_CFG_MAX_HD_CNT ));
    PND_ASSERT(( m_pndHandle.if_nr > 0 ) && ( m_pndHandle.if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

    // Prepare SYS path for IF specific CM-PD user
    PSI_SYSPATH_SET_PATH( m_sysPath, PSI_PATH_IF_APP_IOC_CMCL_ACP );
    PSI_SYSPATH_SET_IF( m_sysPath, m_pndHandle.if_nr );
    PSI_SYSPATH_SET_HD( m_sysPath, m_pndHandle.hd_nr );

    // Create the event handler for all CM-CL events (indications)
    m_pEventHandler = new CIOCEventHandler( &m_pndHandle, this );
    PND_ASSERT( m_pEventHandler != 0 );

    CParamStore* pParam = CParamStore::GetInstance();
    PND_ASSERT( pParam != 0 );

    // create all devices and the including subslots
    // note: registering LADDR of device, slot, subslots in mapping list is done in device
	// Terminate "for" loop when all devices are already found.
    for ( LSA_UINT16 devIdx = 1, devCount = 1; (devIdx < PSI_MAX_CL_DEVICE_NR+1) && (devCount <= m_iocParam.device_count); devIdx++ )
	{
        SubslotVector         subList;
        PND_PARAM_DEVICE_TYPE devParam;

        pnd_memset( &devParam, 0, sizeof( PND_PARAM_DEVICE_TYPE ));

        result = pParam->CreateDeviceParam( &m_pndHandle, devIdx, &devParam, &subList );

		// There is no data at devIdx index
		if(result == PNIO_ERR_PRM_INVALIDARG)
		{
			continue;
		}
        PND_ASSERT( result == PNIO_OK );

		devCount++;

        CIOCDevice* pDevice = new CIOCDevice( devIdx, &m_pndHandle, this );
        PND_ASSERT( pDevice != 0 );

        pDevice->Create( laddr_ptr, &devParam, &subList );

        AddNode( m_deviceMap, pDevice );

        // cleanup subslot params
        pParam->DeleteDeviceParam( &subList );
    }
}


/*----------------------------------------------------------------------------*/
CIOCDevice* CIOCUser::GetDevice( LSA_UINT16 deviceNr )
{
    CIOCDevice* pDevice = dynamic_cast<CIOCDevice*>(getSubNode(m_deviceMap,deviceNr));
    PND_ASSERT( pDevice != 0 );

    return ( pDevice );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::Startup( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pRQB != 0);
    PND_ASSERT( m_responseRQB == 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isStartup  = LSA_TRUE;
    m_isShutdown = LSA_FALSE;

    // Store the upper RQB, and Startup with open the PNIO channel for CM CL
    m_responseRQB = pRQB;

    OpenChannel();
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::Shutdown( LSA_VOID* p_rb )
{
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pRQB != 0);
    PND_ASSERT( m_responseRQB == 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isShutdown = LSA_TRUE;
    m_isStartup  = LSA_FALSE;

    // Store the upper RQB, start shutdown CM CL user till closing the PNIO channel
    m_responseRQB = pRQB;

    if ( m_pnioHandle != CM_INVALID_HANDLE )
    {
        m_requestList.clear();
        LSA_INT count = getNodeList( m_deviceMap, m_requestList );

        PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "CIOCUser::Shutdown(): starting shutdown devices, count(%u)", 
            count );

        CIOCDevice* pDevice = getNextDevice();

        if ( pDevice != 0 ) // device avaliable ?
        {
            // create PND RQB for local Shutdown
            PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
            PND_ASSERT( pRQBLocal != 0 );

            LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

            pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
            pRQBLocal->args.pnuc_internal.data_ptr = m_responseRQB;

            // Register CBF info in RQB for local request done
            PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
            PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CIOCUser::ShutdownDone );

            // Trigger first shutdown and wait for complete
            // Note: received in ShutdownDone
            pDevice->Shutdown( pRQBLocal );
        }
        else
        {
            // start shutdown CM CL user till closing the PNIO channel
            if ( m_isCreated ) // CM CL created ?
            {
                // start shutdown with delete CM-CL
                DeleteCmCl();
            }
            else
            {
                // close the CM CL channel
                CloseChannel();
            }
        }
    }
    else
    {
        // Trigger shutdown complete
        shutdownDone( PNIO_OK );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::StartUpdateBgzd()
{
    // Note: in case of shutdwon, BGZD update is not done
    if ( !m_isShutdown ) // Shutdown in progress
    {
        UserNodeVector devList;

        m_bgzdStore.Reset();

        // Update the own BGZD from BGZD of all childs (IOC devices) as summary
        LSA_INT count = getNodeList( m_deviceMap, devList );

        for ( LSA_INT idx = 0; idx < count; idx++ )
        {
            CIOCDevice* pDev = dynamic_cast<CIOCDevice*>(devList[idx]);

            if ( pDev != 0 )
            {
                pDev->UpdateBgzd( m_bgzdStore );
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CIOCUser::GetKey()
{
    CValueHelper valObj;

    string result = valObj.UINT16ToString( m_pndHandle.pnio_if_nr );

    return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CIOCUser::GetNumber()
{
    return ( m_pndHandle.pnio_if_nr );
}

/*----------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CIOCUser::GetPnioHandle()
{
    return ( m_pnioHandle );
}

/*----------------------------------------------------------------------------*/
/* IOC (PND upper) Request functions                                          */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCUser::ControllerOpen( PND_RQB_PTR_TYPE p_rb )
{ 
   PND_ASSERT(m_mode == IOC_CONFIGURED);

   // send RSP RQB to IOBC
   p_rb->args.iob_open.nr_of_alarm_res = m_nrOfAlarmRes;
   p_rb->args.iob_open.pnio_err = PNIO_OK;

   LSA_RQB_SET_RESPONSE(p_rb, LSA_RET_OK);

   m_mode = IOC_OFFLINE;

   pnd_pnio_user_request_iobc_done( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::ControllerClose( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(m_mode == IOC_OFFLINE);

   PND_RQB_PTR_TYPE pAlarmRqb = 0;

   pAlarmRqb = GetAlarmResource();

   while (pAlarmRqb != 0)
   {
      LSA_RQB_SET_RESPONSE(pAlarmRqb, PNIO_OK_CANCEL);
      
      pnd_pnio_user_request_iobc_done( pAlarmRqb );

      pAlarmRqb = GetAlarmResource();
   }

   p_rb->args.iob_open.pnio_err = PNIO_OK;
   LSA_RQB_SET_RESPONSE(p_rb, LSA_RET_OK);

   m_mode = IOC_CONFIGURED;
   
   pnd_pnio_user_request_iobc_done( p_rb );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::AlarmProvide( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(p_rb);

   SetAlarmResource(p_rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::AlarmReProvide( PND_RQB_PTR_TYPE p_rb )
{
    PND_ASSERT(p_rb);

    PND_ALARM_SESSION_PTR_TYPE SessionData = (PND_ALARM_SESSION_PTR_TYPE)p_rb->args.iob_alarm.SessionData;

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)SessionData->pCmRqb;

    CIOCEventHandler *pThis = static_cast<CIOCEventHandler*>(SessionData->pevHandler);

    SessionData->pCmRqb = 0;
    SessionData->pevHandler = 0;

    // Sessiondata is allocated by EventHandler
    delete  SessionData;

    p_rb->args.iob_alarm.SessionData = 0;

    SetAlarmResource(p_rb);

    // xxx RQB aus Map lesen key = 
    // value ist die Session structure
    // pEventHandler->AArAlarmIndicationDone( pRQB );

    pThis->ArAlarmIndicationDone(pRQB);
}


/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::AlarmIndication( LSA_VOID * p_rb )
{
   PND_ASSERT(p_rb);
   
   //return Alarm RQB Back to PND User Core
   pnd_pnio_user_request_iobc_done( p_rb );
}


PND_IOC_MODE_TYPE CIOCUser::GetMode( LSA_VOID )
{
    return m_mode;
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetMode( PND_RQB_PTR_TYPE p_rb )
{
   PND_ASSERT(p_rb);
   PND_SET_MODE_PTR_TYPE pModeRQB = (PND_SET_MODE_PTR_TYPE)&p_rb->args.iob_set_mode;

   m_modeRQB = p_rb;

   switch ( m_mode )  // actual controller state
   {
   case IOC_OFFLINE: 
      {
         if ( pModeRQB->mode != PNIO_MODE_OFFLINE )
         {
            // Get all devices for activate
            m_requestList.clear();
            LSA_INT count = getNodeList( m_deviceMap, m_requestList );

            PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
               "CIOCUser::SetMode(): starting SetMode devices, mode(%u), count(%u)", 
               pModeRQB->mode, count );

            CIOCDevice* pDevice = getNextDevice();

            if ( pDevice != 0 ) // device avaliable ?
            {
               // create PND RQB for local SetMode
               PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
               PND_ASSERT( pRQBLocal != 0 );

               LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

               pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;

               // Register CBF info in RQB for local request done
               PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
               PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CIOCUser::SetModeDone );

               // Change device activation state to initial when offline -> operate or clear
               pDevice->SetInitialDeviceState();

               // Trigger first setmode (activate) and wait for complete
               // Note: received in SetModeDone
               pDevice->SetMode( pRQBLocal, LSA_TRUE );
            }
            else
            {
               if ( pModeRQB->mode == PNIO_MODE_OPERATE )
               {
                  // set APDU to run, complete request at SetRunDone
                  SetRun();
               }
            }
         }
      }
      break;
   case IOC_CLEAR:
      {
         if ( pModeRQB->mode == PNIO_MODE_OFFLINE )
         {
            // Set APDU stop first
            // Note: passivating device done after CM request (see SetStopDone())
            SetStop();
         }
         else
         {
            if ( pModeRQB->mode == PNIO_MODE_OPERATE )
            {
               // set APDU to run, complete request at SetRunDone
               SetRun();
            }
            else
            {
               // Already in state, respond Request
               pModeRQB->pnio_err = PNIO_OK;

               //return Alarm RQB Back to PND User Core
               pnd_pnio_user_request_iobc_done( p_rb );
            }
         }
      }
      break;

   case IOC_OPERATE: 
      {
         if ( pModeRQB->mode == PNIO_MODE_OFFLINE )
         {
            // Set APDU stop first
            // Note: passivating device done after CM request (see SetStopDone())
            SetStop();
         }
         else
         {
            if ( pModeRQB->mode == PNIO_MODE_CLEAR )
            {
               // set APDU to run, complete request at SetStopDone
               // note: devices still activated
               SetStop();
            }
            else
            {
               // Already in state, respond Request
               pModeRQB->pnio_err = PNIO_OK;

               //return Alarm RQB Back to PND User Core
               pnd_pnio_user_request_iobc_done( p_rb );
            }
         }
      }
      break;
   default:
      {
         PND_FATAL( "invalid internal mode" );
      }
      break;
   }

   // Set new internal controller mode for next requests
   if ( pModeRQB->mode == PNIO_MODE_OFFLINE ) 
   {
      m_mode = IOC_OFFLINE;
   }
   else
   {
      m_mode =  ( pModeRQB->mode == PNIO_MODE_CLEAR ) ? IOC_CLEAR : IOC_OPERATE;
   }
}

/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) Request functions                                              */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCUser::OpenChannel()
{
    PND_ASSERT( m_pnioHandle == CM_INVALID_HANDLE      );
    PND_ASSERT( m_sysPath    != PSI_SYS_PATH_INVALID );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_OPEN_CHANNEL, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::OpenChannel(): Open IOC user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
        m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

    LSA_SYS_PATH_TYPE path = PSI_SYSPATH_GET_PATH( m_sysPath );

    pRQB->args.channel.open->handle                    = CM_INVALID_HANDLE;
    pRQB->args.channel.open->handle_upper              = (LSA_UINT8)path;
    pRQB->args.channel.open->sys_path                  = m_sysPath;
    pRQB->args.channel.open->cm_request_upper_done_ptr = (CM_UPPER_CALLBACK_FCT_PTR_TYPE)pnd_pnio_user_request_lower_done;

    eps_user_open_channel_lower( pRQB, this, CIOCUser::OpenChannelDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::CloseChannel()
{
    PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CLOSE_CHANNEL, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::CloseChannel(): Close IOC user channel, PNIO-IFNr(%u) sys_path(%#x) rqb(%08x) -----------", 
        m_pndHandle.pnio_if_nr, m_sysPath, pRQB );

    // Stop listening CM user indications
    // Note: on this level all device indications must be freed
    m_pEventHandler->StopUserListening();

    eps_user_close_channel_lower( pRQB, this, CIOCUser::CloseChannelDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::GetChannelInfo()
{
    PND_ASSERT( m_pnioHandle !=  CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CHANNEL_INFO, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::GetChannelInfo(): CM CL GetChannelInfo.REQ, rqb(%08x) -----------",  pRQB );

    eps_user_request_lower( pRQB, this, CIOCUser::GetChannelInfoDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::CreateCmCl()
{
    PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_CREATE, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::CreateCmCl(): CM CL CREATE, rqb(%08x)", 
        pRQB );

    CParamStore* pParam = CParamStore::GetInstance();
    PND_ASSERT( pParam != 0 );

    pParam->CreateAnnotation( &m_pndHandle, pRQB->args.cl.create->annotation );

    // Set the cm client create tag from Input data
    pRQB->args.cl.create->max_devices = m_iocParam.max_devices;
    pRQB->args.cl.create->vendor_id   = m_iocParam.vendor_id;
    pRQB->args.cl.create->device_id   = m_iocParam.device_id;
    pRQB->args.cl.create->instance    = m_iocParam.instance;

    pRQB->args.cl.create->rpc_remote_application_timeout1sec = 10;

    // Start with stop --> run is set with mode service after startup
    // note: must set all reserved within the range 0x11-0x1F */
    pRQB->args.cl.create->alarm_reject        = CM_ALARM_RESERVED_GROUP;
    pRQB->args.cl.create->alarm_priority      = 0;
    pRQB->args.cl.create->sr_ar_number        = (LSA_UINT8)m_iocParam.sr_ar_nr;
    pRQB->args.cl.create->initial_data_status = CM_CL_CONTROL_APDU_STATUS_STOP;

    eps_user_request_lower( pRQB, this, CIOCUser::CreateCmClDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::DeleteCmCl()
{
    PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_DELETE, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    PND_IOC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::DeleteCmCl(): CM CL DELETE, rqb(%08x) -----------", pRQB );

    eps_user_request_lower( pRQB, this, CIOCUser::DeleteCmClDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetRun()
{
    PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_CONTROL, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    pRQB->args.cl.control->control_tag = CM_CL_CONTROL_APDU_STATUS;
    pRQB->args.cl.control->apdu_status = CM_CL_CONTROL_APDU_STATUS_RUN;

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::SetRun(): CM CL CONTROL Run, apdu-status(%x), rqb(%08x) -----------", 
        pRQB->args.cl.control->apdu_status,
        pRQB );

    eps_user_request_lower( pRQB, this, CIOCUser::SetRunDone );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetStop()
{
    PND_ASSERT( m_pnioHandle != CM_INVALID_HANDLE );

    CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)pnd_cm_alloc_rqb( CM_OPC_CL_CONTROL, m_pnioHandle, &m_pndHandle, 0 );
    PND_ASSERT(pRQB);

    pRQB->args.cl.control->control_tag = CM_CL_CONTROL_APDU_STATUS;
    pRQB->args.cl.control->apdu_status = CM_CL_CONTROL_APDU_STATUS_STOP;

    PND_IOC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::SetStop(): CM CL CONTROL Stop, apdu-status(%x), rqb(%08x) -----------", 
        pRQB->args.cl.control->apdu_status,
        pRQB );

    eps_user_request_lower( pRQB, this, CIOCUser::SetStopDone );
}

/*----------------------------------------------------------------------------*/
/* IOC internal callback functions                                            */
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCUser::StartupDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCUser        *pIOC = static_cast<CIOCUser*>( p_inst );
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>( p_rb );

    PND_ASSERT( pIOC != 0);
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    if ( pRQB->args.pnuc_internal.pnio_err != PNIO_OK )  // internal error detected
    {
        PND_PNUC_TRACE_00( pIOC->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
        "CUserStore::StartupDone(): >>>> Starting, PNIO IOC user failed" );

        pIOC->m_isStartup = LSA_FALSE;
        pIOC->m_isStarted = LSA_TRUE;

        // Restore upper RQB and set PNIO result
        PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
        PND_ASSERT( pRspRQB != 0 );
        
        pRspRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;

        // Delete local RQB
        pnd_mem_free( pRQB );

        pIOC->startupDone( PNIO_ERR_INTERNAL );
    }
    else
    {
    pIOC->m_nrOfAlarmRes += pRQB->args.pnuc_internal.nr_of_alarm_ind;

    CIOCDevice *pDevice = pIOC->getNextDevice();

    if ( pDevice != 0 ) // device avaliable ?
    {
        // Trigger next device startup and wait for complete
        // Note: reusing received RQB for next startup
        pRQB->args.pnuc_internal.nr_of_alarm_ind = 0;
        pDevice->Startup( pRQB );
    }
    else
    {
            pnd_mem_free( pRQB );

        // Trigger startup complete
        pIOC->startupDone( PNIO_OK );
    }
}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::ShutdownDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
    CIOCUser        *pIOC = static_cast<CIOCUser*>( p_inst );
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>( p_rb );

    PND_ASSERT( pIOC != 0);
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    CIOCDevice *pDevice = pIOC->getNextDevice();

    if ( pDevice != 0 ) // device avaliable ?
    {
        // Trigger next shutdown and wait for complete
        // Note: reusing received RQB for next shutdown
        pDevice->Shutdown( pRQB );
    }
    else
    {
        PND_IOC_TRACE_00( pIOC->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
            "CIOCUser::ShutdownDone(): >>>> Stopping, devices complete" );

        // set PNIO result
        pIOC->m_responseRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;

        // Delete local RQB
        pnd_mem_free( pRQB );

        // start shutdown CM CL user till closing the PNIO channel
        // Note: also done in case of INI changes 
        if ( pIOC->m_isCreated ) // CM CL created ?
        {
            // start shutdown with delete CM-CL
            pIOC->DeleteCmCl();
        }
        else
        {
            // only close the CM CL channel
            pIOC->CloseChannel();
        }
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetModeDone( LSA_VOID* p_inst, LSA_VOID* p_rb )
{
   CIOCUser        *pIOC = static_cast<CIOCUser*>( p_inst );
   PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>( p_rb );

   PND_ASSERT( pIOC != 0);
   PND_ASSERT( pRQB != 0);
   PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

   if ( pRQB->args.pnuc_internal.pnio_err != PNIO_OK )
   {
      // set PNIO result
      pIOC->m_modeRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;
   }

   CIOCDevice *pDevice = pIOC->getNextDevice();

   if ( pDevice != 0 ) // device for request avaliable ?
   {
      LSA_BOOL activate = ( pIOC->m_mode != IOC_OFFLINE ); 
      // Trigger next set mode request and wait for complete
      // Note: reusing received RQB for next shutdown
      pRQB->args.pnuc_internal.pnio_err = PNIO_OK;

      // Change device activation state to initial when offline -> operate or clear
      if(activate == LSA_TRUE)
      {
        pDevice->SetInitialDeviceState();
      }

      pDevice->SetMode( pRQB, activate ); 
   }
   else
   {
      PND_IOC_TRACE_00( pIOC->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
         "CIOCUser::SetModeDone(): >>>> SetMode, devices complete" );

      // Delete local RQB
      pnd_mem_free( pRQB );

      if ( pIOC->m_mode == IOC_OPERATE )
      {
         // change APDU to RUN
         pIOC->SetRun();
      }
      else
      {
         // no change of APDU, finish IO Base request
         pnd_pnio_user_request_iobc_done( pIOC->m_modeRQB );
         pIOC->m_modeRQB = 0;
      }
   }
}

/*----------------------------------------------------------------------------*/
/* IOC (CM-CL) callback functions                                             */
/*----------------------------------------------------------------------------*/


LSA_VOID CIOCUser::OpenChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB   != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_OPEN_CHANNEL);

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_04( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::OpenChannelDone(): Open IOC user channel done, rsp(%#x) sys_path(%#x) handle(%u) rqb(%08x)", 
        CM_RQB_GET_RESPONSE(pRQB),
        pUser->m_sysPath,
        CM_RQB_GET_HANDLE( pRQB ),
        pRQB );

    pUser->m_pnioHandle = pRQB->args.channel.open->handle;
    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

    if ( rsp == CM_OK )
    {
        if ( pUser->m_isStartup ) // automatic startup ?
        {
            // Continue with getting CM CL channel info
            pUser->GetChannelInfo();
        }
    }
    else
    {
        // stop with PNIO error
        pUser->startupDone( PNIO_ERR_INTERNAL );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::CloseChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB   != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_CLOSE_CHANNEL);

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::CloseChannelDone(): Close IOC user channel done, rsp(%#x) sys_path(%#x) RQB(0x%08x)", 
        rsp,
        pUser->m_sysPath,
        pRQB );

    pUser->m_pnioHandle = CM_INVALID_HANDLE;
    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

    if ( rsp == CM_OK )
    {
        if ( pUser->m_isShutdown ) // shutdown in progress ?
        {
            LSA_UINT32 pnioErr = ( rsp == CM_OK ) ? PNIO_OK : PNIO_ERR_INTERNAL;

            // signal shutdown complete
            pUser->shutdownDone( pnioErr );
        }
    }
    else
    {
        PND_FATAL( "CIOCUser::CloseChannelDone(): unexpected CM response received" );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::GetChannelInfoDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB  != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_CHANNEL_INFO);

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_05( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
      "CIOCUser::GetChannelInfoDone(): IOC GetChannelInfo done, rsp(%#x), EDD_hw_type(0x%08x) if_id(%u) nr_of_ports(%u) RQB(0x%08x)", 
        CM_RQB_GET_RESPONSE(pRQB),
        pRQB->args.channel.info->hardware_type,
        pRQB->args.channel.info->interface_id,
        pRQB->args.channel.info->port_cnt,
        pRQB );

    if ( rsp == CM_OK )
    {
        if ( pUser->m_isStartup ) // automatic startup ?
        {
            // Set the EDD HW properties to all devices (needed for ARCB check)
            pUser->setEddHWType( pRQB->args.channel.info->hardware_type );

            // Continue with CM CL creation, with mode is stopped
            pUser->CreateCmCl();
        }
    }
    else
    {
        // stop with PNIO error
        pUser->startupDone( PNIO_ERR_INTERNAL );
    }

    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::CreateCmClDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB  != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB)   == CM_OPC_CL_CREATE);

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::CreateCmClDone(): IOC CREATE done, rsp(%#x) RQB(0x%08x)", 
        rsp,
        pRQB );

    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

    if ( rsp == CM_OK )
    {
        pUser->m_isCreated = LSA_TRUE;

        // Provide the user event RQBs ( overloading, ...)
        pUser->m_pEventHandler->StartUserListening();

        if ( pUser->m_isStartup ) // automatic startup ?
        {
            // continue with startup each device (adding)
            PND_ASSERT( rsp == CM_OK);

            pUser->m_requestList.clear();
            LSA_INT count = pUser->getNodeList( pUser->m_deviceMap, pUser->m_requestList );

            PND_IOC_TRACE_01( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
                "CIOCUser::CreateCmClDone(): starting devices, count(%u)", 
                count );

            CIOCDevice* pDevice = pUser->getNextDevice();

            if ( pDevice != 0 ) // device avaliable ?
            {
                // create PND RQB for local Shutdown
                PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
                PND_ASSERT( pRQBLocal != 0 );

                LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

                pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
                pRQBLocal->args.pnuc_internal.data_ptr = pUser->m_responseRQB;

                // Register CBF info in RQB for local request done
                PND_RQB_SET_USER_INST_PTR( pRQBLocal, pUser );
                PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CIOCUser::StartupDone );

                // Trigger first startup and wait for complete
                // Note: received in StartupDone
                pDevice->Startup( pRQBLocal );
            }
        }
    }
    else
    {
        // stop with PNIO error
        pUser->startupDone( PNIO_ERR_INTERNAL );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::DeleteCmClDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB   != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_DELETE );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::DeleteCmClDone(): CM CL DELETE done, rsp(%#x) PNIO-IFNr(%u) RQB(0x%08x)", 
        CM_RQB_GET_RESPONSE(pRQB),
        pUser->m_pndHandle.pnio_if_nr, 
        pRQB );

    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );

    if ( rsp == CM_OK )
    {
        pUser->m_isCreated = LSA_FALSE;

        if ( pUser->m_isShutdown ) // shutdown ?
        {
            // Close the channel
            pUser->CloseChannel();
        }
    }
    else
    {
        PND_FATAL( "CIOCUser::DeleteCmClDone(): unexpected CM response received" );
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetRunDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB   != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_CONTROL );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::SetRunDone():  CM CL CONTROL Run, rsp(%#x) PNIO-IFNr(%u) RQB(0x%08x)", 
        rsp,
        pUser->m_pndHandle.pnio_if_nr, 
        pRQB );

   if ( rsp != CM_OK )
   {
      pUser->m_modeRQB->args.iob_set_mode.pnio_err = PNIO_ERR_INTERNAL;
   }

   if ( pUser->m_mode == IOC_OFFLINE ) // new state is OFFLINE
   {
      // Start with passivate all devices
      pUser->m_requestList.clear();
      LSA_INT count = pUser->getNodeList( pUser->m_deviceMap, pUser->m_requestList );

      PND_IOC_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
         "CIOCUser::SetRunDone(): starting SetMode devices, mode(%u), count(%u)", 
         pUser->m_modeRQB->args.iob_set_mode.mode, count );

      CIOCDevice* pDevice = pUser->getNextDevice();

      if ( pDevice != 0 ) // device avaliable ?
      {
         // create PND RQB for local SetMode
         PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
         PND_ASSERT( pRQBLocal != 0 );

         LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

         pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
         pRQBLocal->args.pnuc_internal.data_ptr = pUser->m_modeRQB;

         // Register CBF info in RQB for local request done
         PND_RQB_SET_USER_INST_PTR( pRQBLocal, pUser );
         PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CIOCUser::SetModeDone );

         // Trigger first setmode (passivate) and wait for complete
         // Note: received in SetModeDone
         pDevice->SetMode( pRQBLocal, LSA_FALSE );
      }
      else
      {
         // no devices to passivate finish IO Base request
         pnd_pnio_user_request_iobc_done( pUser->m_modeRQB );
         pUser->m_modeRQB = 0;
      }
   }
   else
   {
      pnd_pnio_user_request_local_done( pUser->m_modeRQB );
      pUser->m_modeRQB = 0;
   }

    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetStopDone( LSA_VOID* p_inst, LSA_VOID* p_rb)
{
    CIOCUser *pUser            = static_cast<CIOCUser*>(p_inst);
    CM_UPPER_RQB_PTR_TYPE pRQB = static_cast<CM_UPPER_RQB_PTR_TYPE>(p_rb);

    PND_ASSERT( pUser != 0);
    PND_ASSERT( pRQB   != 0);
    PND_ASSERT( CM_RQB_GET_OPCODE(pRQB) == CM_OPC_CL_CONTROL );

    LSA_RESPONSE_TYPE rsp = CM_RQB_GET_RESPONSE(pRQB);

    PND_IOC_TRACE_03( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::SetStopDone():  CM CL CONTROL Stop, rsp(%#x) PNIO-IFNr(%u) RQB(0x%08x)", 
        rsp,
        pUser->m_pndHandle.pnio_if_nr, 
        pRQB );

   if ( rsp != CM_OK )
   {
      pUser->m_modeRQB->args.iob_set_mode.pnio_err = PNIO_ERR_INTERNAL;
   }

   if ( pUser->m_mode == IOC_OFFLINE ) // new state is OFFLINE
   {
      // Start with passivate all devices
      pUser->m_requestList.clear();
      LSA_INT count = pUser->getNodeList( pUser->m_deviceMap, pUser->m_requestList );

      PND_IOC_TRACE_02( pUser->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
         "CIOCUser::SetMode(): starting SetMode devices, mode(%u), count(%u)", 
         pUser->m_modeRQB->args.iob_set_mode.mode, count );

      CIOCDevice* pDevice = pUser->getNextDevice();

      if ( pDevice != 0 ) // device avaliable ?
      {
         // create PND RQB for local SetMode
         PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
         PND_ASSERT( pRQBLocal != 0 );

         LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

         pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
         pRQBLocal->args.pnuc_internal.data_ptr = pUser->m_modeRQB;

         // Register CBF info in RQB for local request done
         PND_RQB_SET_USER_INST_PTR( pRQBLocal, pUser );
         PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CIOCUser::SetModeDone );

         // Trigger first setmode (passivate) and wait for complete
         // Note: received in SetModeDone
         pDevice->SetMode( pRQBLocal, LSA_FALSE );
      }
      else
      {
         // no devices to passivate finish IO Base request
         pnd_pnio_user_request_iobc_done( pUser->m_modeRQB );
         pUser->m_modeRQB = 0;
      }
   }
   else
   {
      pnd_pnio_user_request_local_done( pUser->m_modeRQB );
      pUser->m_modeRQB = 0;
   }

    pnd_cm_free_rqb( pRQB, &pUser->m_pndHandle );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CIOCUser::startupDone( LSA_UINT32 pnioErr )
{
    PND_RQB_PTR_TYPE pRQB = m_responseRQB;
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    // Signal startup complete
    m_isStarted   = LSA_TRUE;
    m_isStartup   = LSA_FALSE;
    m_responseRQB = 0;

    PND_IOC_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::startupDone(): IOC User startup complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
        m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

   if (pnioErr == PNIO_OK)
   {
      m_mode = IOC_CONFIGURED;
   }

    pRQB->args.pnuc_internal.pnio_err = pnioErr;

    pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::shutdownDone( LSA_UINT32 pnioErr )
{
    PND_RQB_PTR_TYPE pRQB = m_responseRQB;
    PND_ASSERT( pRQB != 0);
    PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

    m_isStarted   = LSA_FALSE;
    m_isStartup   = LSA_FALSE;
    m_isShutdown  = LSA_FALSE;
    m_responseRQB = 0;

    PND_PD_TRACE_03( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
        "CIOCUser::shutdownDone(): IOC User shutdown complete hdNr(%u) ifNr(%u) pnio_res(%#x)",
        m_pndHandle.hd_nr, m_pndHandle.if_nr, pnioErr );

    // Signal shutdown complete
    pRQB->args.pnuc_internal.pnio_err = pnioErr;

    pnd_pnio_user_request_local_done( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::setEddHWType( LSA_UINT32 hwType )
{
    UserNodeVector devList;

    // Update EDD HW type from CM of all childs (IOC devices)
    // Note: the correct EDD HW type is used till CM ARCB creation
    LSA_INT count = getNodeList( m_deviceMap, devList );

    for ( LSA_INT idx = 0; idx < count; idx++ )
    {
        CIOCDevice* pDev = dynamic_cast<CIOCDevice*>(devList[idx]);

        if ( pDev != 0 )
        {
            pDev->SetEddHwType( hwType );
        }
    }
}

/*----------------------------------------------------------------------------*/
CIOCDevice* CIOCUser::getNextDevice()
{
    CIOCDevice* pUser = 0;

    if ( !m_requestList.empty()) // Empty list ?
    {
        // Return the first node, and remove it from top of list
        pUser = dynamic_cast<CIOCDevice*>( m_requestList[0] );

        m_requestList.erase( m_requestList.begin());
    }

    return ( pUser );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CIOCUser::SetAlarmResource(PND_RQB_PTR_TYPE AlarmRqb)
{
   PND_ASSERT(AlarmRqb != 0);

   m_alarmResources.push_back(AlarmRqb);
}


/*----------------------------------------------------------------------------*/
PND_RQB_PTR_TYPE CIOCUser::GetAlarmResource()
{
   PND_RQB_PTR_TYPE pUser = 0;

   if (!m_alarmResources.empty())
   {
      pUser = *m_alarmResources.begin();

      m_alarmResources.erase(m_alarmResources.begin());
   }

   return pUser;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
