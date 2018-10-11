#ifndef PND_USER_NODE_H
#define PND_USER_NODE_H
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
/*  F i l e               &F: pnd_UserNode.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the common user node storage class                         */
/*                                                                           */
/*****************************************************************************/

class CUserNode;
class CBgzdInfoStore;

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

typedef enum pnd_pnio_user_enum // IF specific PNIO users
{
	OHA_USER     = 0x00,  // OHA (user
	PD_USER      = 0x01,  // PD (CM-PD) user
	IOC_USER     = 0x02,  // IOC (CM-CL) user
	PNIO_USER_MAX
} PND_PNIO_USER_TYPE;

typedef map<string, CUserNode*> UserNodeMap;
typedef vector<CUserNode*> UserNodeVector;
typedef vector<PND_RQB_PTR_TYPE> UserRqbVector;

/////////////////////////////////////////////////////////////////////////////
// CUserNode
/////////////////////////////////////////////////////////////////////////////

class CUserNode
{
	// Attributes
protected:
	CUserNode*      m_pParentNode;      // Link to parent user
	PND_HANDLE_TYPE m_pndHandle;        // PND handle (HD/IF,...)
	LSA_BOOL        m_isShutdown;       // Shutdown active yes/no
	LSA_BOOL        m_isStartup;        // Startup active yes/no
	LSA_BOOL        m_isStarted;        // Startup complete yes/no
	CBgzdInfoStore  m_bgzdStore;        // BGZD state of node (single or summary)

	// Construction/ Destruction
protected:
	CUserNode();
	CUserNode( PND_HANDLE_PTR_TYPE pHandle, CUserNode* parent );
public:
	virtual ~CUserNode();

	// Methodes
protected:
	LSA_VOID deleteMap( UserNodeMap &map );
	LSA_VOID deleteList( UserNodeVector &list );

	CUserNode* getSubNode( UserNodeMap &map, LSA_UINT16 key );
	CUserNode* getSubNode( UserNodeMap &map, LSA_UINT32 key );
	CUserNode* getSubNode( UserNodeMap &map, string key );
	LSA_INT    getNodeList( UserNodeMap &map, UserNodeVector &list );

public:
	// Accessors
	CUserNode* GetParent();
	LSA_VOID   AddNode( UserNodeMap &map, CUserNode* node );
	CUserNode* RemoveNode( UserNodeMap &map, CUserNode* node );

	// overridable PND request methodes
	virtual LSA_VOID RecordRead( PND_RQB_PTR_TYPE p_rb );
	virtual LSA_VOID RecordWrite( PND_RQB_PTR_TYPE p_rb );
	virtual LSA_VOID DeviceActivate( PND_RQB_PTR_TYPE p_rb );

	// overridable methodes
	virtual LSA_BOOL IsStarted();
	virtual LSA_BOOL IsStopped();
	virtual LSA_BOOL IsShutdown();
	virtual LSA_VOID ResetBgzd();
	virtual LSA_VOID UpdateBgzd( CBgzdInfoStore& bgzdInfo );
	virtual LSA_HANDLE_TYPE GetPnioHandle();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );

	// Pure virtual methodes
	virtual string   GetKey()    = 0;
	virtual LSA_UINT GetNumber() = 0;
};


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_USER_NODE_H */
