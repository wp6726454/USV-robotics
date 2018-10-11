#ifndef PND_IOC_DEVICE_H
#define PND_IOC_DEVICE_H
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
/*  F i l e               &F: pnd_IOCDevice.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the IOC (CM CL) Device class                               */
/*                                                                           */
/*****************************************************************************/

class CIOCAr;
class CIOCSubslot;
class CIOCSlot;
class CIOCEventHandler;

/////////////////////////////////////////////////////////////////////////////
// PND Defines
/////////////////////////////////////////////////////////////////////////////

// Diagnostic device states
typedef struct  
{
	PNIO_UINT16			ErrorCause;
	PNIO_UINT8			ReasonCode;
	PNIO_UINT8			AdditionalInfo[10];
} ATTR_PACKED DIAG_DEVICE_STATE;
// CIOCDevice storage class for an IOC device
/////////////////////////////////////////////////////////////////////////////

class CIOCDevice : public CUserNode
{
    // Attributes
private:
    LSA_UINT16        m_deviceNr;         // Device Number (KEY value of node)
    LSA_UINT16        m_vendorID;         // VendorID
    LSA_UINT16        m_deviceID;         // DeviceID
    LSA_UINT16        m_instance;         // Instance
    LSA_UINT16        m_laddr;            // device LADDR
    LSA_BOOL          m_activate;         // Device is configured activate yes/no
    LSA_BOOL          m_isAdded;          // Device added (CM-CL)
    LSA_BOOL          m_isAllocateDone;   // PI Buffer allocated (IODU) yes/no
    LSA_UINT32        m_eddHwType;        // EDD HW Type (from CM-CL channel info)
    UserNodeMap       m_subslotMap;       // MAP to shadow real cfg subslots (exp. Config)
    CIOCAr            *m_pAR;             // Device AR
    CIOCEventHandler *m_pEventHandler;    // IOC device event (indication) handling
    PND_RQB_PTR_TYPE  m_responseRQB;      // Stored Response RQB for startup/shutdown
    LSA_UINT16        m_nrOfAlarmRes;     // Alarm resource count Alarms + IND from CM-CL
	DIAG_DEVICE_STATE m_DiagDeviceState;  // Error cause, reason code and additional info
    LSA_BOOL          m_initialStateActivate; // Initial activation state of device

    // Construction/ Destruction
public:
    CIOCDevice( LSA_UINT16 devNr, PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent );
    virtual ~CIOCDevice();
    LSA_VOID Create( PND_PNUC_LADDR_PTR_TYPE laddr_ptr, LSA_VOID_PTR_TYPE pParams, SubslotVector* pSubList );

    //CIOCUser* CIOCDevice::getIoc(); // get base controller
    CIOCUser* getIoc(); // get base controller

    // Accesors for tree
    CIOCAr*      GetAr();
    CIOCSubslot* GetSubslot( LSA_UINT32 apiNr, LSA_UINT16 slotNr, LSA_UINT16 subNr );
    LSA_INT      GetSubslots( UserNodeVector &list );
    LSA_VOID     SetEddHwType( LSA_UINT32 hwType );
    LSA_VOID     ResetSubslots( LSA_UINT16 state );
	LSA_BOOL     GetDeviceState();
    LSA_VOID     SetInitialDeviceState();
	LSA_VOID     GetDeviceDiagState( LSA_UINT16 *error_cause, LSA_UINT8 *reason_code, LSA_UINT8 *additional_info );
	LSA_VOID     SetDeviceDiagState( LSA_UINT16 error_cause, LSA_UINT8 reason_code, LSA_UINT8 *additional_info );

    // Methodes for BGZD updates from CM-CL indications
    LSA_VOID UpdateFromDiffList( LSA_VOID_PTR_TYPE diffList );
    LSA_VOID UpdateForPullModul( LSA_UINT32 api, LSA_UINT16 slot );
    LSA_VOID UpdateFromDiagAlarm( LSA_VOID_PTR_TYPE pAlarm );
    LSA_VOID UpdateForSubmodAlarm( LSA_VOID_PTR_TYPE pAlarm );
    
    LSA_VOID updateArState( PND_IOC_AR_STATE_TYPE state );

    // Overrides to implement UserNode methodes
    virtual string   GetKey();
    virtual LSA_UINT GetNumber();
    virtual LSA_VOID Startup( LSA_VOID* p_rb );
    virtual LSA_VOID Shutdown( LSA_VOID* p_rb );
    virtual LSA_HANDLE_TYPE GetPnioHandle();
    LSA_UINT16 GetVendorID();
    LSA_UINT16 GetDeviceID();
    LSA_UINT16 GetInstanceID();
    LSA_VOID SetStatusActivate();
    LSA_VOID SetStatusPassivate();


    // Overrides for special UserNode methodes
    virtual LSA_VOID ResetBgzd();
    virtual LSA_VOID UpdateBgzd( CBgzdInfoStore& bgzdInfo );
    virtual LSA_VOID StartUpdateBgzd();

    // Methodes
    LSA_VOID SetMode( PND_RQB_PTR_TYPE p_rb, LSA_BOOL activate );

    // Requests to IODU (IO-Base core subsys)
    LSA_VOID AllocateBuffer();
    LSA_VOID FreeBuffer();

    // Overrides to implement UserNode PND request methodes (CM-Requests)
    virtual LSA_VOID DeviceActivate( PND_RQB_PTR_TYPE p_rb );

    // Requests to CM/CL
    LSA_VOID Add( LSA_VOID* p_rb );
    LSA_VOID Remove();
    LSA_VOID Activate( PND_RQB_PTR_TYPE p_rb );
    LSA_VOID Passivate( PND_RQB_PTR_TYPE p_rb );

protected:
    // Class methodes ( Used for callbacks from PNIO )
    static LSA_VOID AddDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
    static LSA_VOID RemoveDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
    static LSA_VOID ActivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
    static LSA_VOID PassivateDone( LSA_VOID* p_inst, LSA_VOID* p_rb);
    static LSA_VOID SetModeDone( LSA_VOID* p_inst, LSA_VOID* p_rb);


    // Class methodes ( Used for callbacks from IODU )
    static LSA_VOID AllocateBufferDone( LSA_VOID* p_inst, LSA_VOID* p_rb );
    static LSA_VOID FreeBufferDone( LSA_VOID* p_inst, LSA_VOID* p_rb );

private:
    LSA_VOID startupDone( LSA_UINT32 pnioErr );
    LSA_VOID shutdownDone( LSA_UINT32 pnioErr );

    CIOCAr* createArFromARCB( LSA_VOID* p_cm, LSA_VOID* p_iodu, UserNodeVector& prov, UserNodeVector& cons );
    PND_IO_ADDR_PTR_TYPE createAddressList( UserNodeVector &sublist, LSA_BOOL isProvider );

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  // !defined(PND_IOC_DEVICE_H)
