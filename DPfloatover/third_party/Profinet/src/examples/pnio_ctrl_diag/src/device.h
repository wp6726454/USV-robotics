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
/*  F i l e               &F: device.h                                  :F&  */
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
#include <vector>
#include "pniousrx.h"

class IMData;
class DiagnosisInfoData;
class SubmoduleData;

typedef std::vector<SubmoduleData*> SubmoduleVector;

class Device
{
    IMData *m_pIMData;
    DiagnosisInfoData *m_pDiagnosisInfoData;
    SubmoduleVector m_SubmoduleList;
    PNIO_UINT16 m_DeviceNr;

public:
    Device(PNIO_UINT16 deviceNr);
    ~Device(void);

    void setIMData(IMData &imData);
    void setDiagnosisInfoData(DiagnosisInfoData &diagnosisInfoData);

    IMData *getIMData();
    DiagnosisInfoData *getDiagnosisInfoData();
    PNIO_UINT16 getDeviceNr();
    SubmoduleVector &getSubmoduleList();
};

