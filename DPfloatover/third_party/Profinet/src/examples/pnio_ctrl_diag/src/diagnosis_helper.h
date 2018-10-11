#pragma once
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
/*  F i l e               &F: diagnosis_helper.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*****************************************************************************/
#include "pniousrx.h"
#include <vector>
#include <map>
#include "diagnosis_info_data.h"
#include "address_info_data.h"
#include "im_data.h"

class Device;
class SubmoduleData;
class IMData;
class DiagnosisInfoData;
class AddressInfoData;

typedef std::map<PNIO_UINT16, Device*> DeviceMap;
typedef std::vector<SubmoduleData*> SubmoduleList;

typedef enum {
	IM0_TYPE = 0xAFF0,
	IM1_TYPE = 0xAFF1,
    IM2_TYPE = 0xAFF2,
    IM3_TYPE = 0xAFF3,
    IM4_TYPE = 0xAFF4
} IM_INDEX_TYPE;


class DiagnosisHelper
{
    DeviceMap m_deviceMap;
    DiagnosisInfoData m_diagnosisInfoData;
    AddressInfoData m_addressInfoData;
    IMData m_iMData;
    PNIO_UINT32 m_errorCode;

public:
    DiagnosisHelper(void);
    ~DiagnosisHelper(void);

    void extractDevices(const PNIO_UINT8 *buffer, PNIO_UINT32 bufferLen );
    void extractSubmodules(SubmoduleList &submoduleList, const PNIO_UINT8 *buffer, PNIO_UINT32 bufferLen );
    void extractAddressInfo(const PNIO_UINT8 *buffer );
    void extractDiagnosisInfo(const PNIO_UINT8 *buffer );
    void extractIMData(const PNIO_UINT8 *buffer, IM_INDEX_TYPE imType );

    void printDevice();
    void printSubmoduleList();
    void printAddressInfo();
    void printDiagnosisInfo();

    PNIO_UINT32 makeDiagnosisRequest(PNIO_UINT32 handle, PNIO_CTRL_DIAG_ENUM diagService, PNIO_UINT16 diagWaitEvent, PNIO_UINT32 laddr = PNIO_NULL);
    PNIO_UINT32 getDeviceConfAndDiag(PNIO_UINT32 handle, PNIO_UINT16 diagWaitEvent, PNIO_UINT16 recReadWaitEvent);

    void setErrorCode(PNIO_UINT32 errorCode);
    PNIO_UINT32 getErrorCode();
private:
    void addNode( DeviceMap &map, PNIO_UINT16 deviceNr, Device* device );
    Device *getDevice(DeviceMap& map, PNIO_UINT16 deviceNr);
    void emptyMap( DeviceMap &map );

    void freeDeviceMap();
};

