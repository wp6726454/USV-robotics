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
/*  F i l e               &F: diagnosis_info_data.cpp                   :F&  */
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
#include "diagnosis_info_data.h"
#include <string.h>

DiagnosisInfoData::DiagnosisInfoData(void)
{
    m_mode = PNIO_DA_FALSE;
    m_errorCause = 0;
    m_reasonCode = 0;
    memset(m_additionalInfo, 0, sizeof(m_additionalInfo));
}


DiagnosisInfoData::~DiagnosisInfoData(void)
{
}


void DiagnosisInfoData::setMode(PNIO_DEV_ACT_TYPE mode)
{
    m_mode = mode;
}

void DiagnosisInfoData::setErrorCause(PNIO_UINT16 error_cause)
{
    m_errorCause = error_cause;
}

void DiagnosisInfoData::setReasonCode(PNIO_UINT8 reason_code)
{
    m_reasonCode = reason_code;
}

void DiagnosisInfoData::setAdditionalInfo(PNIO_UINT8* additional_info)
{
    if(additional_info != PNIO_NULL)
    {
        memcpy(m_additionalInfo, additional_info, sizeof(m_additionalInfo));        
    }
}

PNIO_DEV_ACT_TYPE DiagnosisInfoData::getMode()
{
    return m_mode;
}

PNIO_UINT16 DiagnosisInfoData::getErrorCause()
{
    return m_errorCause;
}

PNIO_UINT8 DiagnosisInfoData::getReasonCode()
{
    return m_reasonCode;
}

void DiagnosisInfoData::getAdditionalInfo(PNIO_UINT8* additional_info)
{
    if(additional_info != PNIO_NULL)
    {
        memcpy(additional_info, m_additionalInfo, sizeof(m_additionalInfo));        
    }
}