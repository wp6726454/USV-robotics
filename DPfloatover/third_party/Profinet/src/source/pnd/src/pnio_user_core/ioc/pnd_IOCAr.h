#ifndef PND_IOC_AR_H
#define PND_IOC_AR_H
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
/*  F i l e               &F: pnd_IOCAr.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of controller AR class                                        */
/*                                                                           */
/*****************************************************************************/

class CIOCSubslot;
class CIOCSlot;

/////////////////////////////////////////////////////////////////////////////
// CIOCAr storage class for IOC ARs
/////////////////////////////////////////////////////////////////////////////

class CIOCAr
{
	// Attributes
private:
	LSA_UINT16             m_arNr;             // AR number
	LSA_UINT16             m_sessionKey;       // Session key
	LSA_UINT16             m_type;             // AR type
	LSA_UINT32             m_properties;       // Properties of AR
	PND_HANDLE_TYPE        m_pndHandle;        // PND handle
	CLRPC_UUID_TYPE        m_UUID;             // UUID of AR
	PND_IOC_AR_STATE_TYPE  m_arState;          // Actual AR-state
	PND_AR_DS_TYPE         m_arDS;             // Actual AR data state
	PND_PNUC_IOCR_TYPE     m_consumer;         // Input CR for an IOC
	PND_PNUC_IOCR_TYPE     m_provider;         // Output CR for an IOC

	// Construction/ Destruction
public:
	CIOCAr( PND_HANDLE_PTR_TYPE pHandle, LSA_VOID* arcb_ptr );
	virtual ~CIOCAr();
	
	// Methodes
	LSA_BOOL CreateIOCR( LSA_VOID* iocr_ptr, LSA_VOID* iodu_ptr );

	// IOCR accessors
	LSA_VOID* GetProviderHandle();
	LSA_VOID* GetConsumerHandle();
	LSA_VOID  UpdateImageProperties( LSA_VOID_PTR_TYPE pEddProp );

	// Accessors Read
	LSA_UINT16            GetSessionKey();
	LSA_BOOL              IsSRD();
	LSA_BOOL              IsINDATA();
	LSA_BOOL              IsPassivate();
	LSA_BOOL              IsRecordReadAllowed();
	LSA_UINT16            GetArType();
	LSA_UINT16            GetArNumber();
	PND_IOC_AR_STATE_TYPE GetArState();
	PND_AR_DS_TYPE        GetArDataState();

	// Accessors Write
	LSA_VOID SetSessionKey( LSA_UINT16 sessionKey );
	LSA_VOID SetArState( PND_IOC_AR_STATE_TYPE state );
	LSA_VOID SetUUID( LSA_UINT8* p_uuid );
	LSA_VOID SetDataStatus( LSA_UINT16 cmDataStatus );

private:

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IOC_AR_H */
