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
/*  F i l e               &F: pnd_UserNode.cpp                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of the common user node storage class                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3102
#define PND_MODULE_ID       3102

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

/* StdLib++ */
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_UserNode.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */


/////////////////////////////////////////////////////////////////////////////
// CUserNode
/////////////////////////////////////////////////////////////////////////////

CUserNode::CUserNode()
{
	m_pParentNode = 0;

	m_isShutdown  = LSA_FALSE;
	m_isStartup   = LSA_FALSE;
	m_isStarted   = LSA_FALSE;

	pnd_memset( &m_pndHandle, 0, sizeof( PND_HANDLE_TYPE ));
}

/*---------------------------------------------------------------------------*/
CUserNode::CUserNode( PND_HANDLE_PTR_TYPE pHandle, CUserNode* parent )
{
	PND_ASSERT( pHandle != 0 );
	PND_ASSERT( parent  != 0 );

	m_pParentNode = parent;
	m_pndHandle   = *pHandle;

	m_isShutdown  = LSA_FALSE;
	m_isStartup   = LSA_FALSE;
	m_isStarted   = LSA_FALSE;
}

/*---------------------------------------------------------------------------*/
CUserNode::~CUserNode()
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID CUserNode::AddNode( UserNodeMap &userNodeMap, CUserNode* node )
{
	PND_ASSERT( node != 0 );

	UserNodeMap::iterator mapIter;

	string key = node->GetKey();

	mapIter = userNodeMap.find( key );

	PND_ASSERT( mapIter == userNodeMap.end() );  // in MAP ?
	userNodeMap[key] = node;
}

/*---------------------------------------------------------------------------*/
CUserNode* CUserNode::RemoveNode( UserNodeMap &userNodeMap, CUserNode* node )
{
	PND_ASSERT( node != 0 );

	CUserNode* value = 0;
	UserNodeMap::iterator mapIter;

	string key = node->GetKey();

	mapIter = userNodeMap.find( key );

	if( mapIter != userNodeMap.end() )  // in MAP ?
	{
		value = mapIter->second;

		userNodeMap.erase( mapIter );
	}

	return ( value );
}

/*---------------------------------------------------------------------------*/
/*  overridable PND request methodes                                         */
/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::RecordRead( PND_RQB_PTR_TYPE p_rb )
{
	// send PNIO error response for invalid LADDR as default  RQB to IOBC
	p_rb->args.rec_read.pnio_err = PNIO_ERR_UNKNOWN_ADDR;

	pnd_pnio_user_request_iobc_done( p_rb );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::RecordWrite( PND_RQB_PTR_TYPE p_rb )
{
	// send PNIO error response for invalid LADDR as default  RQB to IOBC
	p_rb->args.rec_write.pnio_err = PNIO_ERR_UNKNOWN_ADDR;

	pnd_pnio_user_request_iobc_done( p_rb );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::DeviceActivate( PND_RQB_PTR_TYPE p_rb )
{
	// send PNIO error response for invalid LADDR as default  RQB to IOBC
	p_rb->args.iob_dev_act.pnio_err = PNIO_ERR_UNKNOWN_ADDR;

	pnd_pnio_user_request_iobc_done( p_rb );
}

/*---------------------------------------------------------------------------*/
CUserNode* CUserNode::GetParent()
{
	return ( m_pParentNode );
}

/*---------------------------------------------------------------------------*/
/* Overridable methodes                                                      */
/*---------------------------------------------------------------------------*/

LSA_BOOL CUserNode::IsStarted()
{
	return ( m_isStarted );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CUserNode::IsStopped()
{
	return ( !m_isStarted );
}

/*---------------------------------------------------------------------------*/
LSA_BOOL CUserNode::IsShutdown()
{
	return ( m_isShutdown );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::ResetBgzd()
{
	// Default implementation resets own BGZD state
	m_bgzdStore.Reset();
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::UpdateBgzd( CBgzdInfoStore& bgzdInfo )
{
	// Merge the own state with BGZD from Args
	// Note: called by parent to set the summary state of all childs
	m_bgzdStore.UpdateBgzdInfo( bgzdInfo );
}

/*---------------------------------------------------------------------------*/
LSA_HANDLE_TYPE CUserNode::GetPnioHandle()
{
	// Default implementation returns an invalid PNIO handle
	return ( PND_INVALID_PNIO_USER_HANDLE );
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::Startup( LSA_VOID* p_rb )
{
	// Default implementation should not be called, missing override of real class
	LSA_UNUSED_ARG( p_rb );
	PND_FATAL( "CUserNode::Startup(): error base class function called !");
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::Shutdown( LSA_VOID* p_rb )
{
	// Default implementation should not be called, missing override of real class
	LSA_UNUSED_ARG( p_rb );
	PND_FATAL( "CUserNode::Startup(): error base class function called !");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID CUserNode::deleteMap( UserNodeMap &userNodeMap )
{
	UserNodeMap::iterator mapIter;

	for ( mapIter = userNodeMap.begin(); mapIter != userNodeMap.end(); mapIter++ )
	{
		CUserNode *node = mapIter->second;

		delete( node );
	}

	// Cleanup the map keys (with empty values)
	userNodeMap.clear();
}

/*---------------------------------------------------------------------------*/
LSA_VOID CUserNode::deleteList( UserNodeVector &list )
{
	// Free CUserNode elements in Object list
	LSA_INT count = list.size();

	for ( LSA_INT lvi = 0; lvi < count; lvi++ )
	{
		CUserNode* obj = list[lvi];

		if ( obj != 0 )
		{
			delete( obj );
		}
	}

	// Cleanup the list (with empty values)
	list.clear();
}

/*---------------------------------------------------------------------------*/
CUserNode* CUserNode::getSubNode( UserNodeMap &userNodeMap, LSA_UINT16 key )
{
	CUserNode *node = 0;

	if ( !userNodeMap.empty()) //empty map ?
	{
		ostringstream resStr;
		UserNodeMap::iterator mapIter;

		resStr << key;

		string val = resStr.str();

		mapIter = userNodeMap.find( val );

		if ( mapIter != userNodeMap.end() )  // in MAP ?
		{
			node = mapIter->second;
		}
	}

	return ( node );
}

/*---------------------------------------------------------------------------*/
CUserNode* CUserNode::getSubNode( UserNodeMap &userNodeMap, LSA_UINT32 key )
{
	CUserNode *node = 0;

	if ( !userNodeMap.empty())  //empty map ?
	{
		ostringstream resStr;
		UserNodeMap::iterator mapIter;

		resStr << key;

		string val = resStr.str();

		mapIter = userNodeMap.find( val );

		if ( mapIter != userNodeMap.end() )  // in MAP ?
		{
			node = mapIter->second;
		}
	}

	return ( node );
}

/*---------------------------------------------------------------------------*/
CUserNode* CUserNode::getSubNode( UserNodeMap &userNodeMap, string key )
{
	CUserNode *node = 0;

	if ( !userNodeMap.empty())  //empty map ?
	{
		UserNodeMap::iterator mapIter;

		mapIter = userNodeMap.find( key );

		if ( mapIter != userNodeMap.end() )  // in MAP ?
		{
			node = mapIter->second;
		}
	}

	return ( node );
}

/*---------------------------------------------------------------------------*/
LSA_INT CUserNode::getNodeList( UserNodeMap &userNodeMap, UserNodeVector &list )
{
	UserNodeMap::iterator mapIter;

	for ( mapIter = userNodeMap.begin(); mapIter != userNodeMap.end(); mapIter++ )
	{
		CUserNode *node = mapIter->second;

		if ( node != 0 )
		{
			list.push_back( node );
		}
	}

	return ( list.size() );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
