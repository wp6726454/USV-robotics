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
/*  F i l e               &F: submodule_data.cpp                        :F&  */
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
#include "submodule_data.h"


SubmoduleData::SubmoduleData(void)
{
    m_dataLength = 0;
    m_api = 0;
    m_reductionFactor = 0;
    m_phase = 0;
    m_cyleTime = 0;
    m_statNo = 0;
    m_slot = 0;
    m_subslot = 0;
}


SubmoduleData::~SubmoduleData(void)
{
}


void SubmoduleData::setAddress(PNIO_ADDR& addr)
{
    m_address = addr;
}

void SubmoduleData::setDataLength(PNIO_UINT32 length)
{
    m_dataLength = length;
}

void SubmoduleData::setDataType(PNIO_DATA_TYPE data_type)
{
    m_dataType = data_type;
}

void SubmoduleData::setComType(PNIO_COM_TYPE com_type)
{
    m_comType = com_type;
}

void SubmoduleData::setApi(PNIO_UINT32 api)
{
    m_api = api;
}

void SubmoduleData::setReductionFactor(PNIO_UINT32 reduction_factor)
{
    m_reductionFactor = reduction_factor;
}

void SubmoduleData::setPhase(PNIO_UINT32 phase)
{
    m_phase = phase;
}

void SubmoduleData::setCycleTime(PNIO_UINT32 cycle_time)
{
    m_cyleTime = cycle_time;
}

void SubmoduleData::setStatNo(PNIO_UINT32 stat_no)
{
    m_statNo = stat_no;
}

void SubmoduleData::setSlot(PNIO_UINT32 slot)
{
    m_slot = slot;
}

void SubmoduleData::setSubslot(PNIO_UINT32 subslot)
{
    m_subslot = subslot;
}

PNIO_ADDR& SubmoduleData::getAddress()
{
    return m_address;
}

PNIO_UINT32 SubmoduleData::getDataLength()
{
    return m_dataLength;
}

PNIO_DATA_TYPE SubmoduleData::getDataType()
{
    return m_dataType;
}

PNIO_COM_TYPE SubmoduleData::getComType()
{
    return m_comType;
}

PNIO_UINT32 SubmoduleData::getApi()
{
    return m_api;
}

PNIO_UINT32 SubmoduleData::getReductionFactor()
{
    return m_reductionFactor;
}

PNIO_UINT32 SubmoduleData::getPhase()
{
    return m_phase;
}

PNIO_UINT32 SubmoduleData::getCyleTime()
{
    return m_cyleTime;
}

PNIO_UINT32 SubmoduleData::getStatNo()
{
    return m_statNo;
}

PNIO_UINT32 SubmoduleData::getSlot()
{
    return m_slot;
}

PNIO_UINT32 SubmoduleData::getSubslot()
{
    return m_subslot;
}
