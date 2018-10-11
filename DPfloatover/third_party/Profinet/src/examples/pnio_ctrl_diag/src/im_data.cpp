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
/*  F i l e               &F: im_data.cpp                               :F&  */
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
#include "im_data.h"
#include <string.h> 

IMData::IMData(void)
{
    m_vendorId = 0;
    m_hwRevision = 0;
    m_profileSpecificType = 0;
    m_supportedIm = 0;
    memset(m_imDate, 0, sizeof(m_imDate));
    memset(m_serialNumber, 0, sizeof(m_serialNumber));
    memset(m_imFunction, 0, sizeof(m_imFunction));
    memset(m_imLocation, 0, sizeof(m_imLocation));
    memset(m_orderId, 0, sizeof(m_orderId));
    memset(m_imDescriptor, 0, sizeof(m_imDescriptor));
    memset(m_imSignature, 0, sizeof(m_imSignature));
}


IMData::~IMData(void)
{
}

void IMData::setVendorId(PNIO_UINT16 vendorId)
{
    m_vendorId = vendorId;
}

void IMData::setOrderId(PNIO_UINT8* orderId)
{
    if(orderId != PNIO_NULL)
    {
        memcpy(m_orderId, orderId, sizeof(m_orderId));
    } 
}

void IMData::setHWRevision(PNIO_UINT16 hwRevision)
{
    m_hwRevision = hwRevision;
}

void IMData::setSerialNumber(PNIO_UINT8* serialNr)
{
    if(serialNr != PNIO_NULL)
    {
        memcpy(m_serialNumber, serialNr, sizeof(m_serialNumber));
    }
}

void IMData::setProfileSpecificType(PNIO_UINT16 profileSpecificType)
{
    m_profileSpecificType = profileSpecificType;
}

void IMData::setSupportedIm(PNIO_UINT16 supportedIm)
{
    m_supportedIm = supportedIm;
}

void IMData::setIMFunction(PNIO_UINT8* imFunction)
{
    if(imFunction != PNIO_NULL)
    {
        memcpy(m_imFunction, imFunction, sizeof(m_imFunction));
    }
}

void IMData::setIMLocation(PNIO_UINT8* imLocation)
{
    if(imLocation != PNIO_NULL)
    {
        memcpy(m_imLocation, imLocation, sizeof(m_imLocation));
    }
}

void IMData::setIMDate(PNIO_UINT8* imDate)
{
    if(imDate != PNIO_NULL)
    {
        memcpy(m_imDate, imDate, sizeof(m_imDate));
    }
}

void IMData::setIMDescriptor(PNIO_UINT8* imDescriptor)
{
    if(imDescriptor != PNIO_NULL)
    {
        memcpy(m_imDescriptor, imDescriptor, sizeof(m_imDescriptor));
    }
}

void IMData::setIMSignature(PNIO_UINT8* imSignature)
{
    if(imSignature != PNIO_NULL)
    {
        memcpy(m_imSignature, imSignature, sizeof(m_imSignature));
    }
}

PNIO_UINT16 IMData::getVendorId()
{
    return m_vendorId;
}

void IMData::getOrderId(PNIO_UINT8* orderId)
{
    if(orderId != PNIO_NULL)
    {
        memcpy(orderId, m_orderId, sizeof(m_orderId));
    }
}

PNIO_UINT16 IMData::getHwRevision()
{
    return m_hwRevision;
}

void IMData::getSerialNumber(PNIO_UINT8* serialNr)
{
    if(serialNr != PNIO_NULL)
    {
        memcpy(serialNr, m_serialNumber, sizeof(m_serialNumber));
    }
}

PNIO_UINT16 IMData::getProfileSpecificType()
{
    return m_profileSpecificType;
}

PNIO_UINT16 IMData::getSupportedIm()
{
    return m_supportedIm;
}

void IMData::getIMFunction(PNIO_UINT8* imFunction)
{
    if(imFunction != PNIO_NULL)
    {
        memcpy(imFunction, m_imFunction, sizeof(m_imFunction));
    }
}

void IMData::getIMLocation(PNIO_UINT8* imLocation)
{
    if(imLocation != PNIO_NULL)
    {
        memcpy(imLocation, m_imLocation, sizeof(m_imLocation));
    }
}

void IMData::getIMDate(PNIO_UINT8* imDate)
{
    if(imDate != PNIO_NULL)
    {
        memcpy(imDate, m_imDate, sizeof(m_imDate));
    }
}

void IMData::getIMDescriptor(PNIO_UINT8* imDescriptor)
{
    if(imDescriptor != PNIO_NULL)
    {
        memcpy(imDescriptor, m_imDescriptor, sizeof(m_imDescriptor));
    }
}

void IMData::getIMSignature(PNIO_UINT8* imSignature)
{
    if(imSignature != PNIO_NULL)
    {
        memcpy(imSignature, m_imSignature, sizeof(m_imSignature));
    }
}