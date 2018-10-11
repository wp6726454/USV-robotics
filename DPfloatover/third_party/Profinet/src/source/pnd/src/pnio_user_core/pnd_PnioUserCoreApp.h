#ifndef PND_PNIO_USER_CORE_APP_H
#define PND_PNIO_USER_CORE_APP_H
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
/*  F i l e               &F: pnd_PnioUserCoreApp.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the PnDriver PNIO user core application class              */
/*                                                                           */
/*****************************************************************************/

class CUserNode;

/////////////////////////////////////////////////////////////////////////////
// CPnioUserCoreApp  PnDriver PNIO user core app class (singleton instance)
/////////////////////////////////////////////////////////////////////////////

class CPnioUserCoreApp: public CUserNode
{
	// Attributes
private:
	static CPnioUserCoreApp *m_instance;  // The one and only app instance (Singleton)

	PND_HANDLE_TYPE     m_pndPnioHandle[PSI_CFG_MAX_HD_CNT];    // UserStorage for each HD/IF
	CUserNode*          m_userStore[PSI_CFG_MAX_HD_CNT];    // UserStorage for each HD/IF
	PND_PNUC_LADDR_TYPE m_logAddrList[PSI_CFG_MAX_HD_CNT];  // LADDR list for each HD/IF
	UserNodeVector      m_requestList;                      // Request list for startup/shutdown 
	                                                        // User IF in a sequence

	// Construction/ Destruction
	CPnioUserCoreApp(); // Public not allowed (Singleton instance)
public:
	virtual ~CPnioUserCoreApp();

	// static Accessors
	static LSA_BOOL          IsCreated();
	static CPnioUserCoreApp* GetInstance();
	static LSA_VOID          DeleteInstance();

	// PND request Methodes
	void ControllerOpen( PND_RQB_PTR_TYPE pRQB );
	void ControllerClose( PND_RQB_PTR_TYPE pRQB );
	void AlarmProvide( PND_RQB_PTR_TYPE pRQB );
	void AlarmReProvide( PND_RQB_PTR_TYPE pRQB );
	void SetMode(PND_RQB_PTR_TYPE pRQB);
	void DiagnosticRequest( PND_RQB_PTR_TYPE pRQB );
	void SetIpAndNos( PND_RQB_PTR_TYPE p_rb );
	void RemaRead( PND_RQB_PTR_TYPE p_rb );
	void InterfaceOpen( PND_RQB_PTR_TYPE pRQB );
	void InterfaceClose( PND_RQB_PTR_TYPE pRQB );
	void RemaProvide( PND_RQB_PTR_TYPE pRQB );
    void RemaReProvide( PND_RQB_PTR_TYPE pRQB );
    void ParamWrite( PND_RQB_PTR_TYPE pRQB );

	// overrideable PND request Methodes
	virtual LSA_VOID RecordRead( PND_RQB_PTR_TYPE p_rb );
	virtual LSA_VOID RecordWrite( PND_RQB_PTR_TYPE p_rb );
	virtual LSA_VOID DeviceActivate( PND_RQB_PTR_TYPE p_rb );

	// overrideable Methodes
	virtual string   GetKey();
	virtual LSA_UINT GetNumber();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );

    void GetLogAddrList( LSA_VOID* p_rb );

protected:
	// Class methodes ( Used for callbacks )
	static LSA_VOID StartupDone( void* p_inst, void* p_rb );
	static LSA_VOID ShutdownDone( void* p_inst, void* p_rb );

private:
	CUserNode* getNextUserStore();

	LSA_VOID GetDiagConfigSubmoduleList( PND_RQB_PTR_TYPE pRQB );
	LSA_VOID GetDiagDeviceDiagnostic( PND_RQB_PTR_TYPE pRQB );
	LSA_VOID GetDiagConfigNameAddrInfoData( PND_RQB_PTR_TYPE pRQB );

	LSA_VOID AddSubmoduleConfigToBuffer( CUserNode *pUser, LSA_UINT8 *pBuffer );
	LSA_VOID AddDiagDeviceDiagnosticToBuffer( CUserNode *pUser, LSA_VOID *pBuffer );
	LSA_VOID AddConfNameAddrInfoToBuffer( PND_RQB_PTR_TYPE pRQB, LSA_VOID *pBuffer);

	LSA_VOID UpdateRema( PND_RQB_PTR_TYPE p_rb );
};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_PNIO_USER_CORE_APP_H */
