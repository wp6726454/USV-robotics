#ifndef PND_PD_USER_H
#define PND_PD_USER_H
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
/*  F i l e               &F: pndPDUser.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for PD (CM-PD) User                                            */
/*****************************************************************************/

class CUserNode;
class CPDSubslot;
class CPDEventHandler;

typedef struct pnd_pd_alarm_session_tag // storage for indicated PND alarms
{
	CPDEventHandler*  pevHandler;       // IOC Device event handler for Alarm complete
	CM_UPPER_RQB_PTR_TYPE  pCmRqb;       // parked CM RQB for complete

} PND_PD_ALARM_SESSION_TYPE, *PND_PD_ALARM_SESSION_PTR_TYPE;


/////////////////////////////////////////////////////////////////////////////
// CPDUser Interface for PD (CM-PD) User
/////////////////////////////////////////////////////////////////////////////

class CPDUser : public CUserNode
{
	// Attributes
private:
	LSA_SYS_PATH_TYPE     m_sysPath;         // Sys path for Open channel
	LSA_HANDLE_TYPE       m_pnioHandle;      // PNIO component handle (CM PD)
	LSA_BOOL              m_isCreated;       // CM PD create done yes/no
	UserNodeMap           m_subslotMap;      // Map for holding the PDEV subslots
	CPDEventHandler      *m_pEventHandler;   // PD event (indication) handling
	PrmRecordVector       m_prmRecList;      // PRM record list for PRM sequence
	LSA_UINT16            m_actPrmRecIdx;    // Actual PRM record index
	PND_SYNC_STATE_TYPE   m_syncState;       // Actual Sync state of IF (from IF submod)
	PND_RQB_PTR_TYPE      m_responseRQB;     // Stored Response RQB for startup/shutdown
	UserRqbVector         m_alarmResources;  // Alarm Resources
    PND_PARAM_PD_HD_IF_TYPE  m_pdParam;      // PD setup params

	// Construction/ Destruction
public:
	CPDUser();
	CPDUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent );
	virtual ~CPDUser();
	LSA_VOID Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams );
	
	// overrideable methodes methodes for CUserNode IF
	virtual string   GetKey();
	virtual LSA_UINT GetNumber();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );
	virtual LSA_HANDLE_TYPE GetPnioHandle();

	// Methodes
	CPDSubslot* GetMappedSubslot( LSA_UINT16 subslotNr );
	CPDSubslot* GetSubslot( LSA_UINT16 portID );
	LSA_VOID    StartUpdateBgzd();

	LSA_VOID          SetAlarmResource( PND_RQB_PTR_TYPE AlarmRqb );
	PND_RQB_PTR_TYPE  GetAlarmResource();

    LSA_UINT16 GetVendorId();
    LSA_UINT16 GetDeviceId();
    LSA_UINT16 GetInstanceId();

	LSA_VOID UpdateFromDiffList( LSA_VOID* pDiffList );
	LSA_VOID ResetBgzdState();
	LSA_VOID SetARP();
	LSA_VOID PrmStartup();
	LSA_VOID PrmFinished( LSA_UINT32 pnioErr );
	LSA_VOID CheckLinkState();

	// Requests (to CM-PD)
	LSA_VOID OpenChannel();
	LSA_VOID CloseChannel();
	LSA_VOID CreateCmPd();
	LSA_VOID DeleteCmPd();
	LSA_VOID PrmBegin();
	LSA_VOID PrmWrite( PND_PARAM_PRM_RECORD_PTR_TYPE pEntry );
	LSA_VOID PrmEnd();
	LSA_VOID InterfaceOpen( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID InterfaceClose( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID AlarmProvide( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID AlarmIndication( LSA_VOID * p_rb );
	LSA_VOID AlarmReProvide( PND_RQB_PTR_TYPE pRQB );

protected:
	// Class methodes ( Used for callbacks from PNIO )
	static LSA_VOID OpenChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID CloseChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID CreateCmPdDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID DeleteCmPdDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID PrmBeginDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID PrmWriteDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID PrmEndDone( LSA_VOID* p_inst, LSA_VOID* p_rb);

private:
	LSA_VOID startupDone( LSA_UINT32 pnioErr );
	LSA_VOID shutdownDone( LSA_UINT32 pnioErr );

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PD_USER_H */
