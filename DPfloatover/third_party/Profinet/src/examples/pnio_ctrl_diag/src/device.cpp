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
/*  F i l e               &F: device.cpp                                :F&  */
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
#include "device.h"
#include "submodule_data.h"
#include "diagnosis_info_data.h"
#include "im_data.h"


Device::Device(PNIO_UINT16 deviceNr)
{
    m_DeviceNr = deviceNr;
    m_pIMData = new IMData();
    m_pDiagnosisInfoData = new DiagnosisInfoData();
}


Device::~Device(void)
{
    delete m_pIMData;
    delete m_pDiagnosisInfoData;

    for(unsigned i = 0; i<m_SubmoduleList.size(); i++)
    {
        delete m_SubmoduleList.at(i);
    }

    m_SubmoduleList.clear();
}

void Device::setIMData(IMData& imData)
{
    *m_pIMData = imData;
}

void Device::setDiagnosisInfoData(DiagnosisInfoData& diagnosisInfoData)
{
    *m_pDiagnosisInfoData = diagnosisInfoData;
}

IMData* Device::getIMData()
{
    return m_pIMData;
}

DiagnosisInfoData* Device::getDiagnosisInfoData()
{
    return m_pDiagnosisInfoData;
}

PNIO_UINT16 Device::getDeviceNr()
{
    return m_DeviceNr;
}

SubmoduleVector& Device::getSubmoduleList()
{
    return m_SubmoduleList;
}