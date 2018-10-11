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
/*  F i l e               &F: im_data.h                                 :F&  */
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

typedef struct {
    PNIO_BLOCK_HEADER blockHeader;              /* BlockType: 0x0020 */
    PNIO_UINT8        vendorIdHigh;
    PNIO_UINT8        endorIdLow;
    PNIO_UINT8        orderID[20];
    PNIO_UINT8        imSerialNumber[16];
    PNIO_UINT16       imHardwareRevision;     /* (big-endian) */
    PNIO_UINT8        imSoftwareRevision[4];
    PNIO_UINT16       imRevisionCounter;      /* (big-endian) */
    PNIO_UINT16       imProfileId;            /* (big-endian) */
    PNIO_UINT16       imProfileSpecificType; /* (big-endian) */
    PNIO_UINT8        imVersionMajor;
    PNIO_UINT8        imVersionMinor;
    PNIO_UINT16       imSupported;             /* Bitliste Bit 1 --> Supports IM1... (big-endian) */
} PNIO_IM0_TYPE;

typedef struct {
   PNIO_BLOCK_HEADER blockHeader;            /*  BlockType: 0x0021 */
   PNIO_UINT8        imTagFunction[32];
   PNIO_UINT8        imTagLocation[22];
}  PNIO_IM1_TYPE;

typedef struct {
   PNIO_BLOCK_HEADER blockHeader;           /*  BlockType: 0x0022 */
   PNIO_UINT8        imDate[16];
}  PNIO_IM2_TYPE;

typedef struct {
    PNIO_BLOCK_HEADER blockHeader;          /*  BlockType: 0x0023 */
    PNIO_UINT8        imDescriptor[54];
}  PNIO_IM3_TYPE;

typedef struct {
    PNIO_BLOCK_HEADER blockHeader;          /*  BlockType: 0x0024 */
    PNIO_UINT8        imSignature[54];
}  PNIO_IM4_TYPE;

class IMData
{
    PNIO_UINT16 m_vendorId;
    PNIO_UINT8 m_orderId[20];
    PNIO_UINT16 m_hwRevision;
    PNIO_UINT8 m_serialNumber[16];
    PNIO_UINT16 m_profileSpecificType;
    PNIO_UINT16 m_supportedIm;
    PNIO_UINT8 m_imFunction[32];
    PNIO_UINT8 m_imLocation[22];
    PNIO_UINT8 m_imDate[16];
    PNIO_UINT8 m_imDescriptor[54];
    PNIO_UINT8 m_imSignature[54];

public:
    IMData(void);
    ~IMData(void);

    void setVendorId(PNIO_UINT16 vendorId); 
    void setOrderId(PNIO_UINT8 *pOrderId);
    void setHWRevision(PNIO_UINT16 hwRevision); 
    void setSerialNumber(PNIO_UINT8 *pSerialNr); 
    void setProfileSpecificType(PNIO_UINT16 profileSpecificType); 
    void setSupportedIm(PNIO_UINT16 supportedIm); 
    void setIMFunction(PNIO_UINT8 *pImFunction); 
    void setIMLocation(PNIO_UINT8 *pImLocation); 
    void setIMDate(PNIO_UINT8 *pImDate); 
    void setIMDescriptor(PNIO_UINT8 *pImDescriptor); 
    void setIMSignature(PNIO_UINT8 *pImSignature);

    PNIO_UINT16 getVendorId();
    void getOrderId(PNIO_UINT8 *pOrderId);
    PNIO_UINT16 getHwRevision();
    void getSerialNumber(PNIO_UINT8 *pSerialNr);
    PNIO_UINT16 getProfileSpecificType();
    PNIO_UINT16 getSupportedIm();
    void getIMFunction(PNIO_UINT8 *pImFunction); 
    void getIMLocation(PNIO_UINT8 *pImlocation); 
    void getIMDate(PNIO_UINT8 *pImDate); 
    void getIMDescriptor(PNIO_UINT8 *pImDescriptor); 
    void getIMSignature(PNIO_UINT8 *pImSignature);
};

