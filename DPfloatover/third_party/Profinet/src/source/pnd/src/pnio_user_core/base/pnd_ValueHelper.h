#ifndef PND_VALUE_HELPER_H
#define PND_VALUE_HELPER_H

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
/*  F i l e               &F: pnd_ValueHelper.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PnDriver Value helping class for conversions and value checking          */
/*                                                                           */
/*****************************************************************************/

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

typedef vector<string> stringVector;

/////////////////////////////////////////////////////////////////////////////
// CValueHelper: Value Helping class

class CValueHelper
{
private:
	string m_lastError;
	// Construction
public:
	CValueHelper();
	virtual ~CValueHelper();

	// Accessors
	string GetError();

	// Methodes for value check
	LSA_BOOL IsMinValid( string value, LSA_INT min );
	LSA_BOOL IsRangeValid( string value, LSA_INT min, LSA_INT max );
	LSA_BOOL IsSizeValid( string value, LSA_INT maxSize );
	LSA_BOOL IsMACValid( string value );
	LSA_BOOL IsUUIDValid( string value );
	LSA_BOOL IsIPValid( string value );
	LSA_BOOL IsIPValidNullAllowed( string value );
	LSA_BOOL IsBoardIdValid( string value );
	LSA_BOOL IsNotEmptyMAC( LSA_UINT8 *pMac );

	// Methodes for conversion string to type
	LSA_BOOL String2Int( string value, LSA_INT *pIntVal );
	LSA_BOOL String2UINT32( string value, LSA_UINT32 *pVal );
	LSA_BOOL String2UINT16( string value, LSA_UINT16 *pVal );
	LSA_BOOL String2UINT8( string value, LSA_UINT8 *pVal );
	LSA_BOOL String2BOOL( string value, LSA_BOOL *pVal );
	LSA_BOOL String2MAC( string value, LSA_UINT8 *pMac );
	LSA_BOOL String2IP( string value, LSA_UINT32* pIpVal );
	LSA_BOOL String2UUID( string value, LSA_VOID* pUUID );
	LSA_BOOL String2Location( string value, PSI_PCI_LOCATION_PTR_TYPE pLocId );
	LSA_BOOL String2BoardID( string value,  PSI_HD_ID_PTR_TYPE pCpID );
	LSA_BOOL String2EddCompID( string value, LSA_UINT16 *pEddCompID );
	LSA_BOOL String2PortMediaType( string value, PSI_PORT_MEDIA_TYPE *pMediaType );
	LSA_BOOL String2RingPortType( string value, PSI_RINGPORT_TYPE *pRingPortType );
	LSA_BOOL String2MrpRoleType( string value, PSI_MRP_ROLE_TYPE  *pRoleType );
	LSA_BOOL String2LdRunsType( string value, PSI_LD_RUNS_ON_TYPE  *pLdRunsType );
	LSA_BOOL String2EddType( string value,  LSA_UINT16 *pEddType );
	LSA_BOOL String2AsicType( string value, LSA_UINT16 *pAsicType );
	LSA_BOOL String2ImSoftwareVersion( string value, PSI_IM_SW_VERSION_PTR_TYPE pImID );
	PNIO_UINT8* HexArrToCharArr(PNIO_UINT8* src, PNIO_UINT32 dataLength);		
    LSA_VOID CharArrToHexArr(PNIO_UINT8* dst, PNIO_UINT8* src, PNIO_UINT32 datalength);

	// Methodes for conversion basic types to string
	string UINT32ToString( LSA_UINT32 val );
	string UINT16ToString( LSA_UINT16 val );
	string UINT8ToString( LSA_UINT8 val );
	string INT32ToString( LSA_INT32 val );
	string INT16ToString( LSA_INT16 val );
	string INT8ToString( LSA_INT8 val );
	string BOOLToString( LSA_BOOL val );

	// Methodes for conversion special types to string
	string MAC2String( LSA_UINT8* pMac );
	string Buffer2String( LSA_UINT8* pBuffer, LSA_UINT16 bufferSize );
	string IP2String( LSA_UINT32 ip );
	string IP2String( LSA_UINT8* pIP );

	// Methodes for conversion one type to an other one
	LSA_VOID PndHandle2SysPtrPSI( PND_HANDLE_PTR_TYPE cpHandle, LSA_VOID* psiSysPtr );

	// methodes
	LSA_BOOL   SplitObjectID( string objID, LSA_UINT16 *pInst, LSA_UINT16 *pDevId, LSA_UINT16 *pVenId );
	string     GetLeftSubString( string srcStr, LSA_CHAR seperator );
	LSA_INT    GetByteCount( string argVal );
	LSA_UINT16 GetBytes( string argVal, LSA_UINT8 **pMem );
	string     GetJoinedString( stringVector& srcList, string seperator );
    LSA_UINT32 GetDigitCount(LSA_UINT32 number);

private:

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_VALUE_HELPER_H */
