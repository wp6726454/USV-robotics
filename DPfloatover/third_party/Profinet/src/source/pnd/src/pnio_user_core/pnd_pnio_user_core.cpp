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
/*  F i l e               &F: pnd_pnio_user_core.cpp                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver PNIO user adaption                               */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3001
#define PND_MODULE_ID       3001

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"

/* StdLib++ */
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

#include "pnd_iob_core.h"
#include "pnd_pnio_user_core.h"

#include "pnd_BgzdStore.h"
#include "pnd_UserNode.h"
#include "pnd_PnioUserCoreApp.h"

PND_FILE_SYSTEM_EXTENSION(PNDRIVER_MODULE_ID)

/*---------------------------------------------------------------------------*/
/* PNIO user core construction and destruction C-IF                          */
/*---------------------------------------------------------------------------*/

LSA_UINT32 pnd_pnio_user_core_init( LSA_VOID )
{
	LSA_UINT32 result = PNIO_OK;

	if ( CPnioUserCoreApp::IsCreated() ) // init already done ?
	{
		// invalid state
		result = PNIO_ERR_SEQUENCE;
	}
	else
	{
		// create the singleton instance
		CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();

		if ( pApp == 0 )
		{
			result = PNIO_ERR_CREATE_INSTANCE;
		}
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 pnd_pnio_user_core_undo_init( LSA_VOID )
{
	LSA_UINT32 result = PNIO_OK;

	if ( CPnioUserCoreApp::IsCreated() ) // init already done ?
	{
		CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
		PND_ASSERT( pApp != 0 );

		if ( !pApp->IsStopped()) // shutdown complete ?
		{
			// invalid state
			result = PNIO_ERR_SEQUENCE;
		}
		else
		{
			// delete the singleton instance
			CPnioUserCoreApp::DeleteInstance();
		}
	}

	return ( result );
}

/*---------------------------------------------------------------------------*/
/* PNIO user core request C-IF                                               */
/*---------------------------------------------------------------------------*/
LSA_VOID pnd_pnio_user_core_request( PND_RQB_PTR_TYPE  rqb_ptr )
{
	PND_ASSERT( rqb_ptr != 0 );

	LSA_OPCODE_TYPE opc = LSA_RQB_GET_OPCODE(rqb_ptr);

	PND_PNUC_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
		"pnd_pnio_user_core_request(): opcode(%d/%#x) pRQB(0x%x)", 
		opc, opc, rqb_ptr );

	switch ( opc )  // opcode to handle
	{
	case PND_OPC_STARTUP: // startup PNIO user core
		{
			if ( CPnioUserCoreApp::IsCreated() ) // init done ?
			{
				CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
				PND_ASSERT( pApp != 0 );

				pApp->Startup( rqb_ptr );
			}
			else
			{
				// invalid sequence --> no init done before
				rqb_ptr->args.startup.pnio_err = PNIO_ERR_SEQUENCE;
				rqb_ptr->args.startup.cbf( rqb_ptr );
			}
		}
		break;
	case PND_OPC_SHUTDOWN: // shutdown PNIO user core
		{
			if ( CPnioUserCoreApp::IsCreated() ) // init done ?
			{
				CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
				PND_ASSERT( pApp != 0 );

				pApp->Shutdown( rqb_ptr );
			}
			else
			{
				// invalid sequence --> no init done before
				rqb_ptr->args.startup.pnio_err = PNIO_ERR_SEQUENCE;
				rqb_ptr->args.startup.cbf( rqb_ptr );
			}
		}
		break;
	case PND_OPC_CONTROLLER_OPEN: // controller open
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->GetLogAddrList( rqb_ptr );

			pApp->ControllerOpen( rqb_ptr );
		}
		break;
	case PND_OPC_CONTROLLER_CLOSE: // controller close
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->ControllerClose( rqb_ptr );
		}
		break;
	case PND_OPC_ALARM_PROVIDE: // getting Alarm IND resources from IOB
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->AlarmProvide( rqb_ptr );
		}
		break;
	case PND_OPC_ALARM_RESPONSE: // getting Alarm response for an Indication from IOB
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->AlarmReProvide( rqb_ptr );
		}
		break;
	case PND_OPC_REC_READ: // reading record
		{

			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->RecordRead( rqb_ptr );
		}
		break;
	case PND_OPC_REC_WRITE: // writing record
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->RecordWrite( rqb_ptr );
		}
		break;
	case PND_OPC_SET_MODE: // setting controller mode
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->SetMode( rqb_ptr );
		}
		break;
	case PND_OPC_DEVICE_ACTIVATE: // activating/deactivating device
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			
			pApp->DeviceActivate( rqb_ptr );
		}
		break;
	case PND_OPC_DIAG_REQ: // preparing diagnostic service request
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->DiagnosticRequest( rqb_ptr );
		}			
		break;
	case PND_OPC_SET_ADDRESS: // setting addresses (IpSuite and NoS)
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->SetIpAndNos( rqb_ptr );
		}			
		break;
	case PND_OPC_REMA_IND: // rema data read 
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->RemaRead( rqb_ptr );
		}
		break;
	case PND_OPC_INTERFACE_OPEN: // intrerface open
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

            pApp->GetLogAddrList( rqb_ptr );

			pApp->InterfaceOpen( rqb_ptr );
		}
		break;
	case PND_OPC_INTERFACE_CLOSE: // intrerface close
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->InterfaceClose( rqb_ptr );
		}
		break;
	case PND_OPC_REMA_PROVIDE:  // getting Rema IND resources from IOB
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );
			                             
			pApp->RemaProvide( rqb_ptr );
		}
		break;
    case PND_OPC_REMA_RESPONSE: // getting Rema response for an Indication from IOB
        {
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->RemaReProvide( rqb_ptr );
		}
		break;
    case PND_OPC_PRM_WRITE: // parameter write 
		{
			CPnioUserCoreApp *pApp = CPnioUserCoreApp::GetInstance();
			PND_ASSERT( pApp != 0 );

			pApp->ParamWrite( rqb_ptr );
		}
		break;
	default:
		{
			PND_FATAL("pnd_pnio_user_core_request(): not supported opcode received !");
		}
		break;
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_pnio_user_core_request_done( PND_RQB_PTR_TYPE rqb_ptr )
{
	PND_ASSERT( rqb_ptr != 0 );

	/* main CBF function PNIO user core, called in context of PNIO user core task */
	/* note: addressed instance is stored in extended RQB Header, for callback    */
	LSA_VOID_PTR_TYPE p_inst = PND_RQB_GET_USER_INST_PTR( rqb_ptr );
	PND_CORE_DONE_FCT p_fct  = PND_RQB_GET_USER_CBF_PTR( rqb_ptr );

	// execute the CBF for instance
	p_fct( p_inst, rqb_ptr );
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_pnio_user_request_local(  // local request to PNIO User task
	LSA_VOID_PTR_TYPE  rqb_ptr,
	LSA_VOID_PTR_TYPE  p_inst,
	PND_CORE_DONE_FCT  p_fct )
{
	/* add the user instance and CBF in extended RQB Header, and call request IF */
	/* note: information is used for addressing response receiver */
	PND_ASSERT( rqb_ptr != 0 );
	PND_ASSERT( p_inst  != 0 );
	PND_ASSERT( p_fct   != 0 );

	PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;

	PND_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	PND_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

	pnd_request_local( PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, rqb_ptr );
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_pnio_user_request_local_done( LSA_VOID_PTR_TYPE p_rqb )
{
	pnd_request_local( PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, p_rqb );
}

/*---------------------------------------------------------------------------*/
/* main CBF for PN-Stack user channels set at open channel                   */
LSA_VOID pnd_pnio_user_request_lower_done( LSA_VOID_PTR_TYPE p_rqb )
{
	/* CBF from PNIO system to PNIO user core system (CBF of PNIO channel) */
	/* note: RQB is posted to PNIO user core thread                        */
	pnd_request_start( PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, p_rqb );
}

/*---------------------------------------------------------------------------*/
/* Methodes for PND PNIO user core to IO-Base core messaging                 */
/*---------------------------------------------------------------------------*/

LSA_VOID pnd_pnio_user_request_iobc(  // request to IO-Base core task
	LSA_VOID_PTR_TYPE  rqb_ptr,
	LSA_VOID_PTR_TYPE  p_inst,
	PND_CORE_DONE_FCT  p_fct )
{
	/* add the user instance and CBF in extended RQB Header, and call request IF */
	/* note: information is used for addressing response receiver */
	PND_ASSERT( rqb_ptr != 0 );
	PND_ASSERT( p_inst  != 0 );
	PND_ASSERT( p_fct   != 0 );

	PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;

	PND_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	PND_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

	pnd_request_start( PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, rqb_ptr );
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_pnio_user_request_iobc_done( LSA_VOID_PTR_TYPE rqb_ptr )
{
	pnd_request_start( PND_MBX_ID_IO_BASE_CORE,(PND_REQUEST_FCT)pnd_iob_core_request_done, rqb_ptr);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
