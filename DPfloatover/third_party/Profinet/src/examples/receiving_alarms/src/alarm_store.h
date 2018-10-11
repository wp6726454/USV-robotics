#ifndef ALARM_STORE_H
#define ALARM_STORE_H

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
/*  F i l e               &F: alarm_store.h                             :F&  */
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
#include <pnd_int.h>
#include "alarm_item.h"

class CAlarmStore
{
public:
    CAlarmStore();
    void init();
    void addNewAlarm(PNIO_CBE_PRM_ALARM_IND& item);
    unsigned int getFirstAlarmIndex();
    unsigned int getLastAlarmIndex();
    bool isEmpty();
    const AlarmItem_s* getAlarmByIndex(unsigned int index);
    unsigned int calculateNextIndex(unsigned int currentIndex);

private:
    enum {MAX_NUMBER_OF_ALARM_ITEMS = 128};       
    unsigned int m_head;
    unsigned int m_tail;
    unsigned int m_alarmCounter; 
    AlarmItem_s m_alarmItems[MAX_NUMBER_OF_ALARM_ITEMS];  
    bool m_bFull;
    bool m_bEmpty;    
};

#endif