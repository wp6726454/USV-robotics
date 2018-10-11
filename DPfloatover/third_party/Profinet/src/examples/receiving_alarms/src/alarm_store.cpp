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
/*  F i l e               &F: alarm_store.cpp                           :F&  */
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
#include "alarm_store.h"
#include <stdio.h>

CAlarmStore::CAlarmStore()
{
    init();
}

void CAlarmStore::init()
{
    m_head = 0;
    m_tail = 0;
    m_alarmCounter = 0;
    m_bEmpty = true;
    m_bFull = false;
}

void CAlarmStore::addNewAlarm(PNIO_CBE_PRM_ALARM_IND& item)
{   
    m_bEmpty = false;

    m_alarmItems[m_tail].id = m_alarmCounter;    
    m_alarmItems[m_tail].address = *(item.pAddr);
    m_alarmItems[m_tail].alarmData = *(item.pAlarmData);
    
    m_alarmCounter++;

    m_tail = calculateNextIndex(m_tail);
    if(m_bFull == true)
    {
        m_head = m_tail;
    }
    else if(m_head == m_tail)
    {
        m_bFull = true;        
    }
}

unsigned int CAlarmStore::getFirstAlarmIndex()
{
    return m_head;
}

unsigned int CAlarmStore::getLastAlarmIndex()
{
    unsigned int retVal;

    if(m_tail > 0)
    {
        retVal = m_tail-1;
    }
    else
    {
        retVal = MAX_NUMBER_OF_ALARM_ITEMS-1;
    }

    return retVal;
}

unsigned CAlarmStore::calculateNextIndex(unsigned int currentIndex)
{
    return (currentIndex + 1) % MAX_NUMBER_OF_ALARM_ITEMS;
}

bool CAlarmStore::isEmpty()
{
    return m_bEmpty;
}

const AlarmItem_s* CAlarmStore::getAlarmByIndex(unsigned int index)
{
    if(index >= MAX_NUMBER_OF_ALARM_ITEMS)
    {
        return NULL;
    }
    else
    {
        return &(m_alarmItems[index]);
    }        
}

