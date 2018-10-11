#ifndef PND_PARAM_STORE_H
#define PND_PARAM_STORE_H
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
/*  F i l e               &F: pnd_ParamStore.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition for Param storage class (HW and PNIO settings)                */
/*  Note: the HW settings are fixed for StdMAC or PNIO PnBoards              */
/*        PNIO configuration is set at startup from user                     */
/*****************************************************************************/

typedef vector<PND_PARAM_PRM_RECORD_PTR_TYPE> PrmRecordVector;
typedef vector<PND_PARAM_SUBSLOT_PTR_TYPE> SubslotVector;

////////////////////////////////////////////////////////////////////////////////////
// CParamStoreImplIF  interface to implement param handlings (XML)
////////////////////////////////////////////////////////////////////////////////////

#include "servusrx.h"

class CParamStoreImplIF
{
public:
	virtual ~CParamStoreImplIF(){};

	// methodes delegated to special implementation class (XML,...)
	virtual LSA_UINT32 Create( LSA_UINT32 nrOfEntries, LSA_VOID* pParamsData, LSA_UINT32 length, LSA_VOID* pRemaData, LSA_UINT32 remaLength, PND_XML_REMA_DATA_TYPE* remaData  ) = 0;
	virtual LSA_UINT32 UpdatePnioIF( LSA_UINT16 hdNr, LSA_UINT16 ifNr, PSI_HD_IF_OUTPUT_PTR_TYPE pOutput ) = 0;
	virtual LSA_VOID UpdateRema(PND_XML_REMA_DATA_TYPE remaData, PNIO_UINT8** remaFileData, PNIO_UINT32* dataLength) = 0;
	virtual LSA_UINT32 GetParamPnioIF( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PNIO_IF_PTR_TYPE p_params ) = 0;
	virtual LSA_UINT32 GetParamOHA( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_OHA_HD_IF_PTR_TYPE p_params ) = 0;
	virtual LSA_UINT32 GetParamPD(  PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PD_HD_IF_PTR_TYPE  p_params ) = 0;
	virtual LSA_UINT32 GetParamIOC( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_IOC_HD_IF_PTR_TYPE p_params ) = 0;

	virtual LSA_UINT32 CreatePrmRecordsPD( PND_HANDLE_PTR_TYPE cpHandle, PrmRecordVector* pRecList ) = 0;
	virtual LSA_UINT32 CreateDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList ) = 0;

	virtual LSA_UINT32 CreateArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd ) = 0;
	virtual LSA_UINT32 CreateArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE p_Read ) = 0;

};

/////////////////////////////////////////////////////////////////////////////
// CParamsStore PnDriver params storage class for HW and PNIO configuration
/////////////////////////////////////////////////////////////////////////////

class CParamStore
{
	// Attributes
private:
	static CParamStore *m_instance;           // The one and only logging instance (Singleton)
	CParamStoreImplIF  *m_pParamHandlerXML;   // concrete param handling for XML configuration received from data

	LSA_BOOL               m_created;                         // storage based on user data created yes/no
	PND_PARAM_STARTUP_TYPE m_startup;                         // PND handle storage for each user CP location
	PSI_IM_INPUT_TYPE      m_imInput;                         // I&M configuration for this LD
	PSI_HD_INPUT_TYPE      m_hdInput[PSI_CFG_MAX_HD_CNT+1];   // Possible HD inputs for one LD
	PSI_HD_OUTPUT_TYPE     m_hdOutput[PSI_CFG_MAX_HD_CNT+1];  // calc. HD config for each HD (from PN-Stack)

	PND_XML_REMA_DATA_TYPE m_remaData;
    PNIO_SYSTEM_DESCR      m_systemDescription;

	// Construction/ Destruction
	CParamStore(); // Public not allowed (Singleton instance)
    CParamStore(CParamStore&); // No copy constructor
    CParamStore& operator=(const CParamStore&); // No assignment operator
public:
	virtual ~CParamStore();

	// Accessors
	static CParamStore* GetInstance();
	static LSA_VOID DeleteInstance();

	// Methodes
	LSA_UINT32 Create( LSA_UINT32 nrOfEntries, LSA_VOID* pLocationData, LSA_VOID* pParamsData, LSA_UINT32 length, LSA_VOID* pRemaData, LSA_UINT32 remaLength, PNIO_SYSTEM_DESCR* pSysDescr );
	LSA_UINT32 Create( LSA_UINT32 nrOfEntries, LSA_VOID* pLocationData, LSA_VOID* pParamsData, LSA_UINT32 length );

	// Methodes for getting/update the LD/HD settings
	LSA_UINT32 GetParamStartup( PND_PARAM_STARTUP_PTR_TYPE p_params );
	LSA_UINT32 GetParamHifLD( PND_PARAM_HIF_LD_PTR_TYPE p_params );
	LSA_UINT32 GetParamPsiLD( PND_PARAM_PSI_LD_PTR_TYPE p_params );
	LSA_UINT32 SetParamPsiHD( LSA_UINT16 hdNr, PSI_HD_OUTPUT_PTR_TYPE p_output );
	
	// Methodes for getting the HD/IF user settings (for one PNIO-IF)
	LSA_UINT32 GetParamOHA( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_OHA_HD_IF_PTR_TYPE p_params );
	LSA_UINT32 GetParamPD(  PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_PD_HD_IF_PTR_TYPE  p_params );
	LSA_UINT32 GetParamIOC( PND_HANDLE_PTR_TYPE cpHandle, PND_PARAM_IOC_HD_IF_PTR_TYPE p_params );

	// Methodes for creating the local PRM records and annotation from params
	LSA_UINT32 CreatePrmRecordsPD( PND_HANDLE_PTR_TYPE cpHandle, PrmRecordVector* pRecList );
	LSA_UINT32 CreateAnnotation( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT8 *pEntry );
	LSA_UINT32 CreateDeviceParam( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, PND_PARAM_DEVICE_PTR_TYPE pDev, SubslotVector* pSubList );

	// Methodes for creating/deleting the CM CL ARCB params
	LSA_UINT32 CreateArcbAddCL( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, LSA_UINT32 hwType, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE pAdd );
	LSA_UINT32 CreateArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, LSA_UINT16 devNr, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE p_Read );

	// cleanup methodes
	LSA_VOID DeleteArcb( PND_HANDLE_PTR_TYPE cpHandle, CM_UPPER_CL_DEVICE_ADD_PTR_TYPE p_Add );
	LSA_VOID DeleteArcbReadImpl( PND_HANDLE_PTR_TYPE cpHandle, CM_UPPER_CL_RECORD_READ_IMPLICIT_PTR_TYPE p_Read );
	LSA_VOID DeletePrmRecordsPD( PrmRecordVector* pRecList );
	LSA_VOID DeleteDeviceParam( SubslotVector* pSubList );
	LSA_VOID UpdateRema(PNIO_UINT8* data, PNIO_UINT32 dataLength, PNIO_UINT32 index, PNIO_UINT8** remaData, PNIO_UINT32* bufferLength);
	LSA_VOID SetRemaData(PNIO_UINT32 index, PNIO_UINT8* data);
	
    LSA_VOID_PTR_TYPE GetParamMultipleSNMPRecord();
    LSA_UINT32 IsSystemDescriptionValid(PNIO_SYSTEM_DESCR* pSysDescr);
    LSA_UINT32 IsIfDescriptionValid(PNIO_SYSTEM_DESCR* pSysDescr);
    LSA_UINT32 GetSysDescr(LSA_UINT8* pSysDescr);
    LSA_UINT32 GetIfDescr(LSA_UINT8* pIfDescr);
private:
	LSA_VOID prepareParamStartup( LSA_UINT32 nrOfEntries, LSA_VOID* pLocationData, PNIO_SYSTEM_DESCR* sysDescr );

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PARAM_STORE_H */
