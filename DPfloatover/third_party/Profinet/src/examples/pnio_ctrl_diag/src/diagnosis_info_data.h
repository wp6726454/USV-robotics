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
/*  F i l e               &F: diagnosis_info_data.h                     :F&  */
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

class DiagnosisInfoData
{
    PNIO_DEV_ACT_TYPE m_mode;
    PNIO_UINT16 m_errorCause;
    PNIO_UINT8 m_reasonCode;
    PNIO_UINT8 m_additionalInfo[10];
public:
    DiagnosisInfoData(void);
    ~DiagnosisInfoData(void);

    void setMode(PNIO_DEV_ACT_TYPE mode);
    void setErrorCause(PNIO_UINT16 errorCause);
    void setReasonCode(PNIO_UINT8 reasonCode);
    void setAdditionalInfo(PNIO_UINT8 *pAdditionalInfo);

    PNIO_DEV_ACT_TYPE getMode();
    PNIO_UINT16 getErrorCause();
    PNIO_UINT8 getReasonCode();
    void getAdditionalInfo(PNIO_UINT8 *pAdditionalInfo);
};

