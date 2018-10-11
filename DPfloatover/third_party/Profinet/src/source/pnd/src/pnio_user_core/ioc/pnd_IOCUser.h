#ifndef PND_IOC_USER_H
#define PND_IOC_USER_H
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
/*  F i l e               &F: pnd_IOCUser.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for IOC (CM-CL) User                                           */
/*****************************************************************************/

class CUserNode;
class CIOCEventHandler;
class CIOCDevice;


typedef enum pnd_ioc_mode_enum// ioc specific mode states
{
	IOC_INITIAL,
	IOC_CONFIGURED,
	IOC_OFFLINE,
	IOC_CLEAR,
	IOC_OPERATE,
	IOC_MODE_MAX
} PND_IOC_MODE_TYPE;

typedef struct pnd_alarm_session_tag // storage for indicated PND alarms
{
	CIOCEventHandler*  pevHandler;       // IOC Device event handler for Alarm complete
	CM_UPPER_RQB_PTR_TYPE  pCmRqb;       // parked CM RQB for complete

} PND_ALARM_SESSION_TYPE, *PND_ALARM_SESSION_PTR_TYPE;


/////////////////////////////////////////////////////////////////////////////
// CIOCUser Interface for IOC (CM-CL) User
/////////////////////////////////////////////////////////////////////////////

class CIOCUser : public CUserNode
{
	// Attributes
private:
	LSA_HANDLE_TYPE           m_pnioHandle;     // CM CL handle
	LSA_SYS_PATH_TYPE         m_sysPath;        // Sys path for Open channel
	PND_PARAM_IOC_HD_IF_TYPE  m_iocParam;       // IOC setup params
	LSA_BOOL                  m_isCreated;      // CM CL create done yes/no
	CIOCEventHandler         *m_pEventHandler;  // IOC user event (indication) handling
	UserNodeVector            m_requestList;    // List for startup/shutdown all devices in a sequence
	UserNodeMap               m_deviceMap;      // Map for holding the IOC devices
	PND_RQB_PTR_TYPE          m_responseRQB;    // Stored Response RQB for startup/shutdown
	LSA_UINT16                m_nrOfAlarmRes;   // Alarm resource count over all devices
	UserRqbVector             m_alarmResources;  // Alarm Resources
	PND_IOC_MODE_TYPE         m_mode;            // IOC operating mode
	PND_RQB_PTR_TYPE          m_modeRQB;    // Stored Response RQB for set_mode


	// Construction/ Destruction
public:
	CIOCUser();
	CIOCUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent );
	virtual ~CIOCUser();
	LSA_VOID Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams );

	// overrideable methodes methodes for CUserNode IF
	virtual string   GetKey();
	virtual LSA_UINT GetNumber();
	virtual LSA_VOID Startup( LSA_VOID* p_rb );
	virtual LSA_VOID Shutdown( LSA_VOID* p_rb );
	virtual LSA_HANDLE_TYPE GetPnioHandle();

	// Overrides for special UserNode methodes
	virtual LSA_VOID StartUpdateBgzd();

	// Methodes
	CIOCDevice*            GetDevice( LSA_UINT16 deviceNr );
	LSA_VOID               SetAlarmResource( PND_RQB_PTR_TYPE AlarmRqb );
	PND_RQB_PTR_TYPE       GetAlarmResource();
//	LSA_VOID               StoreAlarmRqb(CM_UPPER_RQB_PTR_TYPE rqb);
//	CM_UPPER_RQB_PTR_TYPE  ReStoreAlarmRqb(PND_RQB_PTR_TYPE pnd_rqb);

	// PND Request
	LSA_VOID ControllerOpen( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID ControllerClose( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID AlarmProvide( PND_RQB_PTR_TYPE p_rb );
	LSA_VOID AlarmIndication( LSA_VOID * p_rb );
	LSA_VOID AlarmReProvide( PND_RQB_PTR_TYPE pRQB );
	PND_IOC_MODE_TYPE GetMode( LSA_VOID );
	LSA_VOID SetMode( PND_RQB_PTR_TYPE pRQB );

	// CM Requests
	LSA_VOID OpenChannel();
	LSA_VOID CloseChannel();
	LSA_VOID GetChannelInfo();
	LSA_VOID CreateCmCl();
	LSA_VOID DeleteCmCl();
	LSA_VOID SetRun();
	LSA_VOID SetStop();

protected:
	// Class methodes ( Used for callbacks )
	static LSA_VOID StartupDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID ShutdownDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID OpenChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID CloseChannelDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID GetChannelInfoDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
	static LSA_VOID CreateCmClDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID DeleteCmClDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID SetRunDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID SetStopDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
	static LSA_VOID SetModeDone( LSA_VOID* p_inst, LSA_VOID* p_rb );



private:
	LSA_VOID startupDone( LSA_UINT32 pnioErr );
	LSA_VOID shutdownDone( LSA_UINT32 pnioErr );
	LSA_VOID setEddHWType( LSA_UINT32 hwType );

	CIOCDevice* getNextDevice();
};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IOC_USER_H */
