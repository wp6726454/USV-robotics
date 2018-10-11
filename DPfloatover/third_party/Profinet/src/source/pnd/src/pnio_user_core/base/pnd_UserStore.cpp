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
/*  F i l e               &F: pnd_UserStore.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of pcIOX PNIO User storage class                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3105
#define PND_MODULE_ID       3105

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

#include "pnd_ValueHelper.h"
#include "pnd_ParamStore.h"
#include "pnd_BgzdStore.h"
#include "pnd_UserNode.h"
#include "pnd_UserStore.h"

// All supported User classes
#include "pnd_OHAUser.h"
#include "pnd_PDUser.h"
#include "pnd_IOCUser.h"


/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

/////////////////////////////////////////////////////////////////////////////
// CUserStore class
/////////////////////////////////////////////////////////////////////////////

CUserStore::CUserStore(): CUserNode()
{
	for ( LSA_INT idx = 0; idx < PNIO_USER_MAX; idx++ )
	{
		m_pUser[idx] = 0;
	}

	m_requestList.clear();
}

/*----------------------------------------------------------------------------*/
CUserStore::CUserStore( PND_HANDLE_PTR_TYPE pHandle, PND_PNUC_LADDR_PTR_TYPE laddr_ptr, CUserNode *pParent ) : CUserNode( pHandle, pParent )
{
	PND_ASSERT( laddr_ptr != 0 );

	// Create all IF specific PNIO users
	for ( LSA_INT user = OHA_USER; user < PNIO_USER_MAX; user++ )
	{
		m_pUser[user] = createUser( (PND_PNIO_USER_TYPE)user, laddr_ptr );
	}

	m_requestList.clear();
}

/*----------------------------------------------------------------------------*/
CUserStore::~CUserStore()
{
	// Deletes all created PNIO users
	for ( LSA_INT idx = 0; idx < PNIO_USER_MAX; idx++ )
	{
		if ( m_pUser[idx] != 0 ) // user created ?
		{
			delete ( m_pUser[idx] );
			m_pUser[idx] = 0;
		}
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

string CUserStore::GetKey()
{
	CValueHelper valObj;

	string result = valObj.UINT16ToString( m_pndHandle.cp_nr );

	return ( result );
}

/*----------------------------------------------------------------------------*/
LSA_UINT CUserStore::GetNumber()
{
	return ( m_pndHandle.cp_nr );
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CUserStore::Startup( LSA_VOID* p_rb )
{
	CUserNode *pUser;

	m_requestList.clear();

	m_isStartup = LSA_TRUE;

	// Startup all created PNIO user
	// Note: Opens the user channels and start the user App
	for ( LSA_INT idx = 0; idx < PNIO_USER_MAX; idx++ )
	{
		pUser = m_pUser[idx];

		if ( pUser != 0 ) // created IF user ?
		{
			// Add entry to request list
			m_requestList.push_back ( pUser );
		}
	}

	PND_PNUC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CUserStore::Startup(): Starting, PNIO users, count(%u)", 
		m_requestList.size() );

	pUser = getNextUser();

	if ( pUser != 0 ) // user avaliable ?
	{
		// create PND RQB for local Startup
		PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
		PND_ASSERT( pRQBLocal != 0 );

		LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

		pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
		pRQBLocal->args.pnuc_internal.data_ptr = p_rb;

		// Register CBF info in RQB for local request done
		PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
		PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CUserStore::StartupDone );

		// Trigger first startup and wait for complete
		// Note: received in StartupDone
		pUser->Startup( pRQBLocal );
	}
	else
	{
		// startup finished
		m_isStartup = LSA_FALSE;
		m_isStarted = LSA_TRUE;

		PND_PNUC_TRACE_00( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CUserStore::Startup(): >>>> Starting, PNIO users complete" );

		// Trigger request done
		pnd_pnio_user_request_local_done( p_rb );
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID CUserStore::Shutdown( LSA_VOID* p_rb )
{
	CUserNode *pUser;

	if ( !m_isShutdown ) // Shutdown trigger already in progress ?
	{
		m_isShutdown = LSA_TRUE;

		m_requestList.clear();

		// Shutdown all created PNIO user
		// Note: Stops the user app and closes the user channels
		//       we close it in reverse open order
		for ( LSA_INT idx = PNIO_USER_MAX-1; idx >= OHA_USER; idx-- )
		{
			pUser = m_pUser[idx];

			if ( pUser != 0 ) // created IF user ?
			{
				// Add entry to request list
				m_requestList.push_back( pUser );
			}
		}

		PND_PNUC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CUserStore::Shutdown():Stopping, PNIO users, count(%u)", 
			m_requestList.size() );

		pUser = getNextUser();

		if ( pUser != 0 ) // user avaliable ?
		{
			// create PND RQB for local Shutdown
			PND_RQB_PTR_TYPE pRQBLocal = (PND_RQB_PTR_TYPE)pnd_mem_alloc( sizeof( PND_RQB_TYPE ));
			PND_ASSERT( pRQBLocal != 0 );

			LSA_RQB_SET_OPCODE( pRQBLocal, PND_OPC_PNUC_INTERNAL_REQ );

			pRQBLocal->args.pnuc_internal.pnio_err = PNIO_OK;
			pRQBLocal->args.pnuc_internal.data_ptr = p_rb;

			// Register CBF info in RQB for local request done
			PND_RQB_SET_USER_INST_PTR( pRQBLocal, this );
			PND_RQB_SET_USER_CBF_PTR( pRQBLocal, CUserStore::ShutdownDone );

			// Trigger first shutdown and wait for complete
			// Note: received in ShutdownDone
			pUser->Shutdown( pRQBLocal );
		}
		else
		{
			PND_PNUC_TRACE_00( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
				"CUserStore::Shutdown(): >>>> Stopping, PNIO users complete" );

			m_isShutdown = LSA_FALSE;
			m_isStarted  = LSA_FALSE;

			// Trigger request done
			pnd_pnio_user_request_local_done( p_rb );
		}
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID CUserStore::StartupDone( void* p_inst, void* p_rb )
{
	CUserStore *pStore    = static_cast<CUserStore*>(p_inst);
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pStore != 0);
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	CUserNode *pUser = pStore->getNextUser();

    if ( pRQB->args.pnuc_internal.pnio_err != PNIO_OK )  // internal error detected
	{
        PND_PNUC_TRACE_00( pStore->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
		"CUserStore::StartupDone(): >>>> Starting, PNIO users failed" );

		pStore->m_isStartup = LSA_FALSE;
		pStore->m_isStarted = LSA_TRUE;

		// Restore upper RQB and set PNIO result
		PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
		PND_ASSERT( pRspRQB != 0 );
		
		pRspRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;

		// Delete local RQB
		pnd_mem_free( pRQB );

		// Trigger request done
		pnd_pnio_user_request_local_done( pRspRQB );
    }
    else
    {
	    if ( pUser != 0 ) // user avaliable ?
	    {
		    // Trigger next startup and wait for complete
		    // Note: reusing received RQB for next startup
		    pUser->Startup( pRQB );
	    }
	    else
	    {
		    PND_PNUC_TRACE_00( pStore->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			    "CUserStore::StartupDone(): >>>> Starting, PNIO users complete" );

		    pStore->m_isStartup = LSA_FALSE;
		    pStore->m_isStarted = LSA_TRUE;

		    // Restore upper RQB and set PNIO result
		    PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
		    PND_ASSERT( pRspRQB != 0 );
		
		    pRspRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;

		    // Delete local RQB
		    pnd_mem_free( pRQB );

		    // Trigger request done
		    pnd_pnio_user_request_local_done( pRspRQB );
	    }
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID CUserStore::ShutdownDone( void* p_inst, void* p_rb )
{
	CUserStore *pStore    = static_cast<CUserStore*>(p_inst);
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(p_rb);

	PND_ASSERT( pStore != 0);
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	CUserNode *pUser = pStore->getNextUser();

	if ( pUser != 0 ) // user avaliable ?
	{
		// Trigger next shutdown and wait for complete
		// Note: reusing received RQB for next shutdown
		pUser->Shutdown( pRQB );
	}
	else
	{
		PND_PNUC_TRACE_00( pStore->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CUserStore::ShutdownDone(): >>>> Stopping, PNIO userscomplete" );

		pStore->m_isStarted  = LSA_FALSE;
		pStore->m_isShutdown = LSA_FALSE;

		// Restore upper RQB and set PNIO result
		PND_RQB_PTR_TYPE pRspRQB = (PND_RQB_PTR_TYPE)pRQB->args.pnuc_internal.data_ptr;
		PND_ASSERT( pRspRQB != 0 );
		
		pRspRQB->args.pnuc_internal.pnio_err = pRQB->args.pnuc_internal.pnio_err;

		// Delete local RQB
		pnd_mem_free( pRQB );

		// Trigger request done
		pnd_pnio_user_request_local_done( pRspRQB );

		PND_PNUC_TRACE_00( pStore->m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
			"CUserStore::ShutdownDone(): >>>> Stopping, PNIO userscomplete" ); 
	}
}

/*----------------------------------------------------------------------------*/
CUserNode* CUserStore::GetUser( PND_PNIO_USER_TYPE type )
{
	PND_ASSERT(( type >= OHA_USER ) && ( type < PNIO_USER_MAX ));

	CUserNode* pResult = m_pUser[type];

	return ( pResult );
}

/*----------------------------------------------------------------------------*/
LSA_VOID CUserStore::StartUpdateBgzd()
{
	// Note: in case of shutdwon, BGZD update is not done
	if ( !m_isShutdown ) // Shutdown in progress
	{
		m_bgzdStore.Reset();

		// Update the own BGZD from BGZD of all childs as summary
		for ( LSA_INT idx = 0; idx < PNIO_USER_MAX; idx++ )
		{
			if ( m_pUser[idx] != 0 ) // created IF user ?
			{
				// Add entry to request list
				m_pUser[idx]->UpdateBgzd( m_bgzdStore );
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CUserNode* CUserStore::getNextUser()
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

/*----------------------------------------------------------------------------*/

CUserNode* CUserStore::createUser( PND_PNIO_USER_TYPE userType, PND_PNUC_LADDR_PTR_TYPE laddr_ptr )
{
	CUserNode* pUser = 0;

	PND_PNUC_TRACE_02( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,
		"CUserStore::createUser(): Create PNIO user, UserID(%u) LADDR_ptr(%#x)",
		userType, laddr_ptr );

	CParamStore* pParam = CParamStore::GetInstance();
	PND_ASSERT( pParam != 0 );

	switch( userType )
	{
	case IOC_USER: // CM-CL user
		{
			PND_PARAM_IOC_HD_IF_TYPE iocParam;
			pnd_memset( &iocParam, 0, sizeof ( PND_PARAM_IOC_HD_IF_TYPE ));

			CIOCUser* pUserIOC = new CIOCUser( &m_pndHandle, this );
			PND_ASSERT( pUserIOC != 0 );

			LSA_UINT32 res = pParam->GetParamIOC( &m_pndHandle, &iocParam );
			PND_ASSERT( res == PNIO_OK );

			pUserIOC->Create( laddr_ptr, &iocParam );
			pUser = pUserIOC;
		}
		break;
	case OHA_USER: // OHA user
		{
			PND_PARAM_OHA_HD_IF_TYPE ohaParam;
			pnd_memset( &ohaParam, 0, sizeof ( PND_PARAM_OHA_HD_IF_TYPE ));

			COHAUser *pUserOHA = new COHAUser( &m_pndHandle, this );
			PND_ASSERT( pUserOHA != 0 );

			LSA_UINT32 res = pParam->GetParamOHA( &m_pndHandle, &ohaParam );
			PND_ASSERT( res == PNIO_OK );

			pUserOHA->Create( &ohaParam );
			pUser = pUserOHA;
		}
		break;

	case PD_USER: // CM-PD user
		{
			PND_PARAM_PD_HD_IF_TYPE pdParam;
			pnd_memset( &pdParam, 0, sizeof ( PND_PARAM_PD_HD_IF_TYPE ));

			CPDUser *pUserPD = new CPDUser( &m_pndHandle, this );
			PND_ASSERT( pUserPD != 0 );

			LSA_UINT32 res = pParam->GetParamPD( &m_pndHandle, &pdParam );
			PND_ASSERT( res == PNIO_OK );

			pUserPD->Create( laddr_ptr, &pdParam );
			pUser = pUserPD;
		}
		break;
	default:
		{
			PND_PNUC_TRACE_01( m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
				"CUserStore::createUser: UserStore not supports PNIO user with ID(%u)", 
				userType );

			PND_FATAL( "CUserStore::createUser(): Invalid PNIO user ID" );
		}
		break;
	}

	return ( pUser );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
