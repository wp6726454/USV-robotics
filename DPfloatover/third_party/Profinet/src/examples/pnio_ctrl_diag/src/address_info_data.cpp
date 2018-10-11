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
/*  F i l e               &F: address_info_data.cpp                     :F&  */
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
#include "address_info_data.h"
#include <string.h>

AddressInfoData::AddressInfoData(void)
{
    memset(m_name, 0, sizeof(m_name));
    memset(m_typeOfStation, 0, sizeof(m_typeOfStation));
    m_ipAddr = 0;
    m_ipMask = 0;
    m_defaultRouter = 0;
}


AddressInfoData::~AddressInfoData(void)
{
}

void AddressInfoData::setName(PNIO_UINT8* name)
{
    if(name != PNIO_NULL)
    {
        memcpy(m_name, name, sizeof(m_name));
    }
}

void AddressInfoData::setTypeOfStation(PNIO_UINT8* pTypeOfStation)
{
    if(pTypeOfStation != PNIO_NULL)
    {
        memcpy(m_typeOfStation, pTypeOfStation, sizeof(m_typeOfStation));
    }
}

void AddressInfoData::setIpAddr(PNIO_UINT32 ipAddr)
{
    m_ipAddr = ipAddr;
}

void AddressInfoData::setIpMask(PNIO_UINT32 ipMask)
{
    m_ipMask = ipMask;
}

void AddressInfoData::setDefaultRouter(PNIO_UINT32 defaultRouter)
{
    m_defaultRouter = defaultRouter;
}

void AddressInfoData::getName(PNIO_UINT8* name)
{
    if(name != PNIO_NULL)
    {
        memcpy(name, m_name, sizeof(m_name));
    }
}

void AddressInfoData::getTypeOfStation(PNIO_UINT8* pTypeOfStation)
{
    if(pTypeOfStation != PNIO_NULL)
    {
        memcpy(pTypeOfStation, m_typeOfStation, sizeof(m_typeOfStation));
    }
}

PNIO_UINT32 AddressInfoData::getIpAddr()
{
    return m_ipAddr;
}

PNIO_UINT32 AddressInfoData::getIpMask()
{
    return m_ipMask;
}

PNIO_UINT32 AddressInfoData::getDefaultRouter()
{
    return m_defaultRouter;
}