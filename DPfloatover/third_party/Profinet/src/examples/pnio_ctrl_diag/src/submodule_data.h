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
/*  F i l e               &F: submodule_data.h                          :F&  */
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
#include "pniousrx.h"


class SubmoduleData
{
    PNIO_ADDR m_address;
    PNIO_UINT32 m_dataLength;
    PNIO_DATA_TYPE m_dataType;
    PNIO_COM_TYPE m_comType;
    PNIO_UINT32 m_api;
    PNIO_UINT32 m_reductionFactor;
    PNIO_UINT32 m_phase;
    PNIO_UINT32 m_cyleTime;
    PNIO_UINT32 m_statNo;
    PNIO_UINT32 m_slot;
    PNIO_UINT32 m_subslot;
    

public:
    SubmoduleData(void);
    ~SubmoduleData(void);

    void setAddress(PNIO_ADDR &addr);
    void setDataLength(PNIO_UINT32 length);
    void setDataType(PNIO_DATA_TYPE dataType);
    void setComType(PNIO_COM_TYPE comType);
    void setApi(PNIO_UINT32 api);
    void setReductionFactor(PNIO_UINT32 reductionFactor);
    void setPhase(PNIO_UINT32 phase);
    void setCycleTime(PNIO_UINT32 cycleTime);
    void setStatNo(PNIO_UINT32 statNo);
    void setSlot(PNIO_UINT32 slot);
    void setSubslot(PNIO_UINT32 subslot);

    PNIO_ADDR &getAddress();
    PNIO_UINT32 getDataLength();
    PNIO_DATA_TYPE getDataType();
    PNIO_COM_TYPE getComType();
    PNIO_UINT32 getApi();
    PNIO_UINT32 getReductionFactor();
    PNIO_UINT32 getPhase();
    PNIO_UINT32 getCyleTime();
    PNIO_UINT32 getStatNo();
    PNIO_UINT32 getSlot();
    PNIO_UINT32 getSubslot();

};

