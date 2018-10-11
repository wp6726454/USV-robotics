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
/*  F i l e               &F: address_info_data.h                       :F&  */
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

#include "pniobase.h"

class AddressInfoData
{
    PNIO_UINT8    m_name[256];
    PNIO_UINT8    m_typeOfStation[256];
    PNIO_UINT32   m_ipAddr;
    PNIO_UINT32   m_ipMask;
    PNIO_UINT32   m_defaultRouter;

public:
    AddressInfoData(void);
    ~AddressInfoData(void);

    void setName(PNIO_UINT8* name);
    void setTypeOfStation(PNIO_UINT8* typeOfStation);
    void setIpAddr(PNIO_UINT32 ipAddr);
    void setIpMask(PNIO_UINT32 ipMask);
    void setDefaultRouter(PNIO_UINT32 defaultRouter);

    void getName(PNIO_UINT8* name);
    void getTypeOfStation(PNIO_UINT8* typeOfStation);
    PNIO_UINT32 getIpAddr();
    PNIO_UINT32 getIpMask();
    PNIO_UINT32 getDefaultRouter();
};

