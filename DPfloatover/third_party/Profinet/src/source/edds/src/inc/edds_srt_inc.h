
#ifndef EDDS_SRT_INC_H                        /* ----- reinclude-protection ----- */
#define EDDS_SRT_INC_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_srt_inc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  03.06.02    JS    initial version.                                       */
/*  19.08.03    JS    added KnotProvMaxRatioCnt                              */
/*                    added some aSRT stuff                                  */
/*  23.09.03    JS    EDDS_SRT_VLAN_PRIO_ASRT_HIGH changed from 7 to 6       */
/*  30.09.03    JS    changed EDDS_ASRT_FRAME_LENGTH_INTERNAL from 60 to 64  */
/*  16.10.03    JS    added SkipCheckFlag                                    */
/*  01.12.03    JS    CLEAR/OPERTE renamed to STOP/RUN                       */
/*  04.12.03    JS    Added CycleCounter-Support of Consumer                 */
/*  14.12.03    JS    Changed frameformat of Error-Frames                    */
/*  21.01.04    JS    Changed CycleBaseFactor and ReductionRatio interpretion*/
/*  15.06.04    JS    removed ASRT-types/defines                             */
/*  26.06.04    JS    moved/changed some define names                        */
/*  04.08.04    JS    added xrt over udp support names                       */
/*  14.01.05    JS    we now have two consumer tables. one for XRT Class 2   */
/*                    and one for XRT and XRToverUPD (Class1/4)              */
/*  07.03.05    JS    New WatchdogCylceCnt                                   */
/*  10.03.05    JS    Made CycleCntStamp different for xrt/udp (16/32Bit)    */
/*  08.08.05    JS    added SkipCycleStamp                                   */
/*  10.11.05    JS    Added CycleCounter                                     */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
/*  04.05.09    JS    moved defines because of general edd_usr.h             */
/*  18.01.10    JS    AP00914640: Provider shall not Autostop before first   */
/*                    MISS detected (initial MISS state shall not Autostop a */
/*                    Provider! Note: The Consumer must be activated with    */
/*                    "SetToUnknown" so the first Consumer MISS can be used  */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDDS functions uses the following IDs                     */
/*                                                                           */
/*        EDDS:  0x00 .. 0x0F                                                */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*        EIB:   0x10 .. 0x1F                                                */
/*                                                                           */
/*        Each component uses the followin module IDs (defined in component) */
/*                                                                           */
/*        NRT:   0x80 .. 0x8F                                                */
/*        SRT:   0x90 .. 0x9F                                                */
/*        IRT:   0xA0 .. 0xAF                                                */
/*        SWI:   0xB0 .. 0xBF                                                */
/*                                                                           */
/* ========================================================================= */


#define EDDS_MODULE_ID_SRT_USR             0x90
#define EDDS_MODULE_ID_SRT_PROV            0x91
#define EDDS_MODULE_ID_SRT_CONS            0x92

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

#define EDDS_SRT_IDX_NOT_USED       0xffff    /* for several not used idx var*/

#define EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL  LSA_TRACE_LEVEL_ERROR

/*---------------------------------------------------------------------------*/
/* Frameformat for cyclic SRT:                                               */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ mandatory on send      */
/* 12/16 | 2 |TypeLenxRT                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 | 2 |FrameID                              |                         */
/* 16/20 |   |I/O-Data                             |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |   xRT-Data              */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+                         */
/*       | 2 |CycleCounter                         |                         */
/*       | 1 |DataStatus                           |                         */
/*       | 1 |TransferStatus                       |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if EDD_VLAN_TAG_SIZE != 4
#error "EDD_VLAN_TAG_SIZE <> 4 not supported!!"
#endif

#define EDDS_CSRT_FRAME_IO_START_WITH_VLAN              20
#define EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN           16

/* size of framecomponents before and after I/O-Data */
/* (including VLAN-Tag!)                             */

#define EDDS_CSRT_FRAME_IO_DATA_PROLOG_SIZE            (EDDS_CSRT_FRAME_IO_START_WITH_VLAN)
#define EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE             4

/* Offsets of Fields behind I/O-Data */

#define EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET            0
#define EDDS_CSRT_FRAME_DATA_STATUS_OFFSET              2
#define EDDS_CSRT_FRAME_TRANSFER_STATUS_OFFSET          3


/*---------------------------------------------------------------------------*/
/* Frameformat for cyclic SRT over UDP                                       */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ not inserted on send   */
/* 12/16 | 2 |IP-Type                              |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 |20 |IPHeader                             |  we only accept 20 Byte */
/* ------+---+-------------------------------------+-------------------------*/
/* 34/38 | 8 |UDP-Header                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 42/46 | 2 |FrameID                              |                         */
/* 44/48 |   |I/O-Data                             |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+                         */
/*       | 2 |CycleCounter                         |                         */
/*       | 1 |DataStatus                           |                         */
/*       | 1 |TransferStatus                       |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* size of framecomponents before and after I/O-Data */
/* (excluding VLAN-Tag!)                             */


/*---------------------------------------------------------------------------*/
/* VLAN - Tag fields                                                         */
/* Network-Format:                                                           */
/*                                                                           */
/*               8 7 6    5    4 3 2 1                                       */
/*              +--------------------+                                       */
/*   octet 1    | Prio | CFI |  VID  |                                       */
/*              +--------------------+                                       */
/*   octet 2    |       VID          |                                       */
/*              +--------------------+                                       */
/*                                                                           */
/*  15.13 12  11         ...         0                                       */
/*  +----+---+-----------------------+                                       */
/*  |Prio|CFI|         VID           |                                       */
/*  +----+---+-----------------------+                                       */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Bits 0..11 */

#define EDDS_SRT_VLAN_ID_MSK            0x0FFF
#define EDDS_SRT_VLAN_ID_SHIFT          0

/* Bit  12    */

#define EDDS_SRT_VLAN_CFI_MSK           0x1000
#define EDDS_SRT_VLAN_CFI_SHIFT         12

/* Bits 13..15*/

#define EDDS_SRT_VLAN_PRIO_MSK          0xE000
#define EDDS_SRT_VLAN_PRIO_SHIFT        13

#define EDDS_SRT_VLAN_PRIO_ASRT_HIGH    6
#define EDDS_SRT_VLAN_PRIO_CSRT         6
#define EDDS_SRT_VLAN_PRIO_ASRT_LOW     5

/*---------------------------------------------------------------------------*/
/* Macros for accessing Frame (pBuffer has to be DWORD-aligned!)             */
/*---------------------------------------------------------------------------*/

/* xSRT-Header */

#define EDDS_SRT_FRAME_ACC_VLAN_TAG(pBuffer)(((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_VLAN_TYPE_WORD_OFFSET])
#define EDDS_SRT_FRAME_ACC_VLAN_ID(pBuffer) (((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_VLAN_ID_WORD_OFFSET])
#define EDDS_SRT_FRAME_ACC_TYPELEN(pBuffer) (((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET])
#define EDDS_SRT_FRAME_ACC_FRAMEID(pBuffer) (((EDD_UPPER_MEM_U16_PTR_TYPE )pBuffer)[EDDS_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET])

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/* link-structure */

typedef struct _EDDS_SRT_LINK_BASE_TYPE
{
    EDD_UPPER_RQB_PTR_TYPE          pBottom;
    EDD_UPPER_RQB_PTR_TYPE          pTop;
} EDDS_SRT_LINK_BASE_TYPE;

typedef struct _EDDS_SRT_LINK_BASE_TYPE  EDDS_LOCAL_MEM_ATTR *   EDDS_SRT_LINK_BASE_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* for chaining of free/used consumer/provider/context                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_SRT_CHAIN_BASE_TYPE
{
    LSA_UINT16      BottomIdx;      /* index in provider/consumer table.     */
    LSA_UINT16      TopIdx;         /* index in provider/consumer table.     */
} EDDS_SRT_CHAIN_BASE_TYPE;

typedef struct _EDDS_SRT_CHAIN_TYPE
{
    LSA_UINT16      Prev;           /* prev index in provider/consumer table.*/
    LSA_UINT16      Next;           /* next index in provider/consumer table.*/
} EDDS_SRT_CHAIN_TYPE;

#define EDDS_SRT_CHAIN_IDX_END     0xFFFF      /* end of table */

/*===========================================================================*/
/* SRT handle (channel) management-structures                                */
/*===========================================================================*/


/*===========================================================================*/
/* cSRT- Device Management structure                                         */
/*===========================================================================*/

#define EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN         1  /* dont change */
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#define EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX        15
#else
#define EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX        10
#endif

#define EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN     1
#define EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX    10

#define EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MIN     1
#define EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MAX    15

/* Number of CyclesRatios */

#define EDDS_CSRT_CYCLE_RATIO_CNT   (EDDS_CSRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX)

/* some internal statistics */

typedef struct _EDDS_CSRT_STAT_TYPE
{
    #ifdef EDDS_CFG_SRT_USE_RXCNT_STATISTIC
    LSA_UINT32      RxCnt;                      /* Number of received Frames */
    #endif
    LSA_UINT32      ProvCycleOverloadCnt;       /* Number of Overloads.      */
    LSA_UINT32      ProvCycleCounterDropCnt;    /* Number of times the Prov. */
    /* CycleCounter occured      */
    /* before the previous one   */
    /* was handled.              */
    /* (heavy overload?)         */
    LSA_UINT32      ConsRxCycleCounterOld;      /* Number of old Frames      */
    LSA_UINT32      ConsRxWrongXRTLenOrMAC;     /* Number of XRT-Frames with */
    /* Wrong FrameLen or Src-MAC */
    LSA_UINT32      ConsRxWrongUDPLenOrIP;      /* Number of UDP-Frames with */
    /* Wrong UDP-Len or Src-IP . */
    LSA_UINT32      ConsCycleCounterDropCnt;    /* Number of times the Cons. */
    /* CycleCounter occured      */
    /* before the previous one   */
    /* was handled.              */
    /* (heavy overload?)         */
    LSA_UINT32      ConsCycleOverloadCnt;       /* Number of times the Cons. */
    /* Check-Cycle was overloaded*/
    LSA_UINT32      ConsCycleJitterDrop;        /* Consumer check dropped    */
    /* because of cycle-jitter.  */
    LSA_UINT32      NoMoreKnotElements;         /* No more free Knot elements*/
} EDDS_CSRT_STAT_TYPE;


/*---------------------------------------------------------------------------*/
/* for cyclic-timer                                                          */
/*---------------------------------------------------------------------------*/

typedef volatile struct _EDDS_SRT_TIMER_STRUCT
{
    LSA_BOOL        InUse;                /* LSA_TRUE: Timer in USE  */
    LSA_BOOL        InProgress;           /* LSA_TRUE: Timer running */
    LSA_TIMER_ID_TYPE  TimerID;           /* LSA Timer ID            */
    LSA_UINT16      EDDSTimerID;          /* Timer ID                */
    LSA_UINT32      BaseTimeNS;           /* Base time in ns.        */
} EDDS_SRT_TIMER_TYPE;

typedef EDDS_SRT_TIMER_TYPE   EDDS_LOCAL_MEM_ATTR *     EDDS_SRT_TIMER_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* Knot-Management (Table)                                                   */
/* This table is used for managing of provider send list and consumer       */
/* check-list.                                                               */
/*                                                                           */
/* To save space the table has too parts. The KnotTable and the              */
/* KnotElementTable. The Knottable holds an index into the KnotElementTable  */
/* if the KnotTable entry is used.                                           */
/*                                                                           */
/*         KnotTable              KnotElementTable                           */
/*       +-----------+             +-----------+                             */
/*       |         -----index-->   |           |                             */
/*       +-----------+             +-----------+                             */
/*       |  unused   |             |           |                             */
/*       +-----------+             +-----------+                             */
/*       |         -----index-->   :           :                             */
/*       +-----------+                                                       */
/*       |           |                                                       */
/*       :           :                                                       */
/*                                                                           */
/*                                                                           */
/* With this table for every cycle we have send-lists (provider) and         */
/* check-lists (consumer) to be handled. This is managed within the         */
/* KnotElement.                                                              */
/*                                                                           */
/* Basics:                                                                   */
/*                                                                           */
/* The cycletime is the basecycle we send provider. Note that for consumer   */
/* we have an additional "check-factor" to reduce the checktimes. So we      */
/* may only check every 3 cycletimes for arrived consumers.                  */
/*                                                                           */
/* cycletime = 1000,2000 or 4000 us.                                         */
/*                                                                           */
/* For every provider/consumer a cycle for sending/checking is defined       */
/* by a reductionratio, as a factor based on this cycletime.                 */
/*                                                                           */
/* ReductionRatio:  1,2,4,8,16,32,64,128...                                  */
/*                                                                           */
/* A ReductionRatio of 4 means, that a provider is send every 4 Cycles.      */
/*                                                                           */
/* Provider:                                                                 */
/*                                                                           */
/* So a provider a ReductionRation of 2 has a two-times slower cycle         */
/* (interval = 2). For this provider we have two time times where we can     */
/* send the frame (=phase). For a Ratio of 4 we have four times (interval=4) */
/* and four phases.                                                          */
/*                                                                           */
/* The combination of ReductionRatio and phase is called KNOT.               */
/*                                                                           */
/* For every Knot several provider can be defined  (or calculated) to be     */
/* send (provider send list). In the Knot-table the anchor of this list      */
/* is stored (the anchor is a index into the provider-table, where the       */
/* linking is done)                                                          */
/*                                                                           */
/*                                                                           */
/* For every Knot several provider can be defined  (or calculated) to be     */
/* send (provider send list). In the Knot-table the index of a Knot-element  */
/* is stored. This Knot element holds the index into the provider-table where*/
/* the linking is done)                                                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* This table has the following format:                                      */
/*                                                                           */
/* Knot  Index   table content                                               */
/* 1.1    0      index to knot element or 0xfff if not used.                 */
/* 2.1    1                  "                                               */
/* 2.2    2                  "                                               */
/* 4.1    3                  "                                               */
/* 4.2    4                  "                                               */
/* 4.3    5                  "                                               */
/* 4.4    6                  "                                               */
/*                                                                           */
/* x.y : x= RatioNr,y:Phase within ratio                                     */
/*                                                                           */
/* ReductionRatio:   1  2  4  8  16  32  64  128                             */
/* RatioNr           1  2  3  4  5   6   7   8                               */
/* (note: within program we use RatioNr from 0..7 for easier usage)          */
/*                                                                           */
/* The entries of the table is fix and determined by the Maximum             */
/* ReductionRation allowed (1+2+4+8+16+32.. MaxReductionRatio)               */
/* For a max. Reduction Ratio of 128 we have 255 entries.                    */
/*                                                                           */
/* With every clock we have to use one knot from each ReductionRatio         */
/* (e.g. 1.1,2.1,4.1.. for first clock, 1.1,2.2,4.2.. for second clock etc.) */
/*                                                                           */
/* Consumer:                                                                 */
/*                                                                           */
/* For consumer we have to check for received frames. The check is done      */
/* with a reduced timescale (e.g. 3 times the cycletime of a consumer).      */
/* (if a consumer arrives with 1 ms cycle time we check for this frame       */
/* every 3 ms). This factor is global for all consumer. So every consumer    */
/* has a check-timer x-times the cycletime. So similar to the provider we    */
/* have the same Knots with consumers to be checked (but with a different    */
/* clock).                                                                   */
/*                                                                           */
/* The entries of the table is identically to providers.                     */
/*                                                                           */
/* With every clock we have to use one knot from each ReductioRatio          */
/* (e.g. 1.1,2.1,4.1.. for first clock, 1.1,2.2,4.2.. for second clock etc.) */
/*                                                                           */
/* As we can see, the tables for provider and consumer have the same format  */
/* and size. The knot (index in table) which has to be handled for each      */
/* clock can be calculated with the following sequence:                      */
/*                                                                           */
/* CyclCnt is a global 32-Bit Counter incremented with each clock.           */
/*                                                                           */
/* i     = 1                                                                 */
/* index = 0                                                                 */
/*                                                                           */
/* n represents the number of different reduction ratios                     */
/* n represents the reduction ratio as follows: 1=1,2=2,3=4,4=8,5=16..8=128  */
/*                                                                           */
/* for (n=1;n<=8;n++)                                                        */
/* {                                                                         */
/*   HandleKnot(Index)                                                       */
/*   i = i*2                                                                 */
/*   mask = i-1                                                              */
/*   index = (i-1) + (CyclCnt & mask);                                       */
/* }                                                                         */
/* CyclCnt++;                                                                */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_KNOT_ELEMENT_TYPE
{
    LSA_BOOL            InUse;                    /* Element in use within Knottable*/
    LSA_UINT16          ProviderCnt;              /* number of provider in KnotList */
    EDDS_SRT_CHAIN_BASE_TYPE ProviderSendListXRT;      /* Index in provider management   */
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
    SRT_CHAIN_BASE_TYPE ProviderSendListUDP;      /* Index in provider management   */
    #endif
    LSA_UINT32          ProviderByteCnt;          /* Sum of all Databytes of        */
    /* providers in send list.        */
    /* (Independent of active or not) */
    LSA_UINT16          ConsumerCnt;              /* number of consumer in KnotList */
    EDDS_SRT_CHAIN_BASE_TYPE ConsumerCheckList;        /* index in consumer management   */
    EDDS_SRT_CHAIN_TYPE      UsedFreeLink;             /* chainelement for used or       */
    /* freelist                       */
} EDDS_CSRT_KNOT_ELEMENT_TYPE;

#define EDDS_CSRT_LIST_END   0xFFFF

typedef struct _EDDS_CSRT_KNOT_ELEMENT_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR;

typedef struct _EDDS_CSRT_KNOT_TYPE
{
    LSA_UINT16          KnotElementIdx;           /* Index of Knot element.         */
    /* EDDS_SRT_IDX_NOT_USED if unused */
    LSA_UINT16          ProviderCntAboveMax;      /* max number of providers of all */
    /* ratios above this ratio        */
    LSA_UINT16          ProviderByteCntAboveMax;  /* max number of provider bytes   */
    /* of all ratios above this ratio */
} EDDS_CSRT_KNOT_TYPE;

typedef struct _EDDS_CSRT_KNOT_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_CSRT_KNOT_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* Provider-Management (Table)                                               */
/*---------------------------------------------------------------------------*/

#define EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER /* add a provider-send-counter*/
/* to management.             */

/*---------------------------------------------------------------------------*/
/* type for managing provider CR's                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS ON                                                        */
/*---------------------------------------------------------------------------*/
#include "pnio_pck4_on.h"

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDS_CSRT_PROVIDER_CR_TYPE
{
	EDDS_EXCHANGE_TYPE      User;         // holds reference to user buffer
	EDDS_EXCHANGE_TYPE      XChange;      // holds reference to XChange buffer
	EDDS_EXCHANGE_TYPE      EDDS;         // holds reference to EDDS buffer
	EDDS_EXCHANGE_TYPE      Latest;       // holds reference to the actual buffer (last changed by user)

	EDD_UPPER_MEM_PTR_TYPE  pBuffer[3];   // 3 buffer

	LSA_UINT16              DataLen;      // length of IO data

	LSA_UINT8               Type;         // type of provider/consumer --> xRT or UDP

    LSA_BOOL                IsLocked;     // locked by user
	                                      // no special locking mechanism needed; calls to lock/unlock a provider
		                                  // have to be from one context

	LSA_BOOL                CrUsed;       // is this Cr used or not
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDS_CSRT_PROVIDER_CR_TYPE EDDS_LOCAL_MEM_ATTR    EDDS_CSRT_PROVIDER_CR_TYPE;
typedef struct _EDDS_CSRT_PROVIDER_CR_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_CSRT_PROVIDER_CR_TYPE_PTR;

#include "pnio_pck_off.h"
/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS OFF                                                       */
/*---------------------------------------------------------------------------*/


typedef struct _EDDS_CSRT_PROVIDER_TYPE
{
    LSA_UINT8                   Type;           /* Type of Providers         */
    LSA_UINT16                  Properties;     /* properties from ADD       */
    LSA_BOOL                    Active;         /* provider active?          */
	LSA_UINT16                  CrNumber;       /* CrNumber from activate    */
    /* --------------------------*/
    LSA_UINT32                  KnotTableIdx;   /* Idx to Knot-table where   */
    /* Provider is chained.      */
    /* User-Parameters ----------*/
    LSA_UINT16                  FrameID;        /* FrameID to insert in Frame*/
    EDD_MAC_ADR_TYPE            DstMAC;         /* Dst-MAC-Address           */
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
    EDD_IP_ADR_TYPE             DstIP;          /* Dst-IP-Address            */
    EDDS_IP_TYPE                SrcIP;          /* Src-IP-Address            */
    #endif
    LSA_UINT16                  DataLen;        /* Length of I/O-Data. Not   */
    /* FrameLength!              */
    LSA_UINT16                  CycleReductionRatio;   /* 1,2,4,8,..128.. */
    LSA_UINT16                  CycleReductionNr;      /* 0,1,2,3...7..   */
    LSA_UINT16                  CyclePhase;
    LSA_UINT32                  CyclePhaseSequence;
    LSA_UINT16                  VLANID;
    LSA_UINT32                  UserID;
    LSA_UINT16                  GroupID;        /* Provider Group from Add   */

    LSA_UINT16                  AutoStopConsumerID;
                                                /* 0..ConsumerID,            */
                                                /* EDD_CONSUMER_ID_REMOVE_AS */
                                                /* if not used               */

    LSA_UINT8                   DataStatus;     /* Bits 0..3 of DataStatus   */
    LSA_BOOL                    DoSetupBuffer;  /* LSA_TRUE tells the        */
    /* sender to setup FrameBuf. */
    LSA_UINT8                   FirstSend;      /* First Send Flags          */

    #ifdef EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER
    LSA_UINT32                  SendCounter;    /* Incremented after each    */
    /* send. (for statistics)    */
    #endif
    /* Chains..------------------*/
    EDDS_SRT_CHAIN_TYPE         SendLink;       /* chainelement for sendlist */
    EDDS_SRT_CHAIN_TYPE         UsedFreeLink;   /* chainelement for used or  */
    /* freelist                  */


} EDDS_CSRT_PROVIDER_TYPE;

typedef struct _EDDS_CSRT_PROVIDER_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_CSRT_PROVIDER_TYPE_PTR;

/* Provider types */
#define EDDS_CSRT_PROVIDER_TYPE_NOT_USED	0
#define EDDS_CSRT_PROVIDER_TYPE_XRT         1
#define EDDS_CSRT_PROVIDER_TYPE_UDP         2

/* FirstSend : */

#define EDDS_PROV_FIRST_SEND                0x1 /* set after first send               */
#define EDDS_PROV_FIRST_SEND_INDICATED      0x2 /* set after first send was indicated */

/*---------------------------------------------------------------------------*/
/* Type for managing provider groups                                         */
/* Every provider is asigned to a group. Each group holds groupspecific      */
/* Providersettings                                                          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_PROV_GROUP_TYPE
{
    LSA_UINT8   ProviderDataStatus;             /* global Provider State     */
} EDDS_CSRT_PROV_GROUP_TYPE;

typedef struct _EDDS_CSRT_PROV_GROUP_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_CSRT_PROV_GROUP_TYPE_PTR;

/*---------------------------------------------------------------------------*/
/* type for managing provider ratio infos                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_PROV_RATIO_INFO_TYPE
{
    /* Holds the maximum Provider*/
    /* Framebytecount in a phase */
    /* of a ratio                */
    LSA_UINT32   MaxByteCnt;
    /* Holds the maximum Provider*/
    /* count in a phase of a rati*/
    LSA_UINT32   MaxCnt;                        /* (xrt+udp)                 */

    /* Holds the number of       */
    /* Provider within ratio     */
    LSA_UINT32   CntXRT;                        /* for  xrt                  */
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
    /* Holds the number of       */
    /* Provider within ratio     */
    LSA_UINT32   CntUDP;                        /* for udp                   */
    #endif
} EDDS_CSRT_PROV_RATIO_INFO_TYPE;

/*---------------------------------------------------------------------------*/
/* type for managing provider cycle                                          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE
{
    LSA_UINT8       Type;                   /* Type of Providers to control  */
    /* EDDS_CSRT_PROVIDER_TYPE_XRT   */
    /* EDDS_CSRT_PROVIDER_TYPE_UDP   */
    LSA_BOOL        CycleInProgress;        /* TRUE if provider Cycle runnig */
    /* Set when Cycle starts. reset  */
    /* after last frame was sent.    */
    LSA_BOOL        CycleSendStarted;       /* TRUE if first tx-query of a   */
    /* provider cycle occured.       */
    LSA_BOOL        CycleDoSend;            /* TRUE if provider should send. */
    /* (reset on overload or if no   */
    /*  more provider in sendlist)   */

    /* actual Min/Max Intervals used */
    LSA_UINT16      MinInterval;            /* 1,2,4,8,16...                 */
    LSA_UINT16      MaxInterval;            /* 1,2,4,8,16...                 */

    /* if CycleInProgress:           */
    LSA_UINT16      ActInterval;            /* 1,2,4,8,16...                 */
    LSA_UINT16      ActProvIdx;             /*                               */
    LSA_UINT16      ActTxInProgress;        /* Number of transmits in progr. */
} EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE;

typedef struct _EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR;

typedef struct _EDDS_CSRT_PROV_CYCLE_TYPE
{
    LSA_UINT32      CycleCounter;           /* ProviderCycleCounter          */
    /* wraps around! used for        */
    /* calculating knot-table-idx.   */
    LSA_UINT16      OverloadDropCycles;     /* if <> 0 remaining number of   */
    /* cycles to drop after overload.*/
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE Xrt;  /* xRT-Frames                    */
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE Udp;  /* xRToverUDP-Frames             */
    #endif
} EDDS_CSRT_PROV_CYCLE_TYPE;


/*---------------------------------------------------------------------------*/
/* type for managing consumer ratio infos                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_CONS_RATIO_INFO_TYPE
{
    /* Holds the mnumber of      */
    /* consumer within ratio     */
    LSA_UINT32   Cnt;                           /* for  xrt and udp          */
} EDDS_CSRT_CONS_RATIO_INFO_TYPE;

/*---------------------------------------------------------------------------*/
/* type for managing consumer cycle                                          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_CONS_CYCLE_TYPE
{
    LSA_BOOL        CycleInProgress;        /* TRUE if consumer Cycle runnig */
    /* Set when Cycle starts. reset  */
    /* after last frame was sent.    */
    LSA_UINT32      CycleCounter;           /* ConsumerCycleCounter          */
    /* wraps around! used for        */
    /* calculating knot-table-idx.   */
    /* actual Min/Max Intervals used */
    LSA_UINT16      MinInterval;            /* 1,2,4,8,16...                 */
    LSA_UINT16      MaxInterval;            /* 1,2,4,8,16...                 */

    LSA_UINT32      TimerValue;             /* Stored TimerValue for Jitter  */
    /* detection                     */
} EDDS_CSRT_CONS_CYCLE_TYPE;

/*---------------------------------------------------------------------------*/
/* type for managing consumer CR's                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS ON                                                        */
/*---------------------------------------------------------------------------*/
#include "pnio_pck4_on.h"

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDS_CSRT_CONSUMER_CR_TYPE
{
	EDDS_EXCHANGE_TYPE      User;         // holds reference to user buffer
	EDDS_EXCHANGE_TYPE      XChange;      // holds reference to XChange buffer
	EDDS_EXCHANGE_TYPE      EDDS;         // holds reference to EDDS buffer

	EDD_UPPER_MEM_PTR_TYPE  pBuffer[3];   // 3 buffer

	LSA_UINT16              DataLen;      // length of IO data

	LSA_UINT8               Type;         // type of provider/consumer --> xRT or UDP

	LSA_BOOL                IsLocked;     // locked by user
	                                      // no special locking mechanism needed; calls to lock/unlock a provider
		                                  // have to be from one context

	LSA_BOOL                CrUsed;       // is this Cr used or not
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDS_CSRT_CONSUMER_CR_TYPE EDDS_LOCAL_MEM_ATTR    EDDS_CSRT_CONSUMER_CR_TYPE;
typedef struct _EDDS_CSRT_CONSUMER_CR_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_CSRT_CONSUMER_CR_TYPE_PTR;

#include "pnio_pck_off.h"
/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS OFF                                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Management for consumer frames (Consumer-Frames). The management is a     */
/* a table with ConsumerCnt (xRT+xRToverUDP) entrys and the ConsumerID as    */
/* index. First come all xRT then all xRToverUDP-Consumers. The table is     */
/* broken into two parts and referenced by pConsTbl and pConsFrameTbl .      */
/*                                                                           */
/* pConsTbl ->    +----------------+ +                                       */
/*                |                | |                                       */
/*                | all xRT        | >  ConsumerCnt    (xRT Consumer)        */
/*                |                | |                                       */
/* Index =        +----------------+ +                                       */
/* ConsumerID     |                | |                                       */
/*                | all xRToverUDP | >  ConsumerCntUDP (xRToverUDP-Consumer) */
/*                |                | |                                       */
/*                +----------------+ +                                       */
/*                                                                           */
/* same with pConsFrameTbl.                                                  */
/*                                                                           */
/* Every Consumer has a fixed assigned FrameID based on the Index within the */
/* table! With ConsumerType (xRT/xRToverUDP) and FrameID the index can be    */
/* calculated (= ConsumerID)                                                 */
/*                                                                           */
/* Note: We use two seperate tables for consumer because we have to put      */
/*       some parts in fast memory. and the fast memory is limited!!         */
/*       All parts accessed within isr-handler put into fast memory          */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_CONSUMER_FRAME_TYPE
{
    volatile LSA_UINT8              State;      /* consumer active?          */
    LSA_UINT8                       Type;       /* UDP/XRT, ClearOnMiss      */
    /* since last consumer-check */
    LSA_UINT16                      FrameLength;/* Framelength including VLAN*/
    /* with xRToverUDP this is   */
    /* the UDP-Data len only!    */
    /* Framebuffer handling------*/

    union
    {
        struct
        {
            LSA_UINT16              CycleCntStamp; /* CycleStamp on receive. */
            /* based on actual prov.  */
            /* Cyclecounter.          */
            /* Src-MAC ---------------*/
            EDD_MAC_ADR_TYPE		SrcMAC;
        } xrt;

        struct
        {
            LSA_UINT32              CycleCntStamp; /* CycleStamp on receive. */
            /* based on actual prov.  */
            /* Cyclecounter.          */
            EDD_IP_ADR_TYPE			SrcIP;
        } udp;
    } StampAddr;

    LSA_UINT8                       DataStatus; /* Last DataStatus from Frame*/



    LSA_UINT16                      CycleCounter;  /* CycleCounter of        */
    /* last received frame    */
    /* if transferStatus is ok*/
    /* and frame was newer    */
    /* then last one.         */
    /* (not neccessary the    */
    /* counter for pX! because*/
    /* pX is only updated if  */
    /* we have a valid-frame) */

} EDDS_CSRT_CONSUMER_FRAME_TYPE;

/* defines for State */

/*-----------------------------------------------------------------------------------------*/
/* We need ascending numbers for state (for compares ==, >=, <= and !=),                   */
/* also we need this order:                                                                */
/*                                                                                         */
/* OFF -->  OFF-fire-event -->                                                             */
/*     -->  ON-no-frames   -->  ON-no-frames-fire-event -->                                */
/*     -->  ON-frames      --> ON-frames-fire-event     -->                                */
/*                                                                                         */
/* For details, see SetToUnknown.VSD                                                       */
/*-----------------------------------------------------------------------------------------*/
#define EDDS_SRT_CONS_STATE_INIT      EDDS_SRT_CONS_STATE_OFF_0
/*-----------------------------------------------------------------------------------------*/
#define EDDS_SRT_CONS_STATE_OFF_0     0     /* consumer initialized, not active            */
#define EDDS_SRT_CONS_STATE_OFF_MNS   1     /* consumer off, fire MISS_NOT_STOPPED         */
#define EDDS_SRT_CONS_STATE_OFF_1     2     /* consumer off (from autostop)                */
#define EDDS_SRT_CONS_STATE_OFF_MISS  3     /* DHT expired, (fire MISS, do autostop)       */
/*-----------------------------------------------------------------------------------------*/
											/* consumer active, ...                        */
#define EDDS_SRT_CONS_STATE_ON_0      4     /* ... no frame received yet                   */
#define EDDS_SRT_CONS_STATE_ON_1      5     /* ... no frame received yet (set_to_unknown)  */
#define EDDS_SRT_CONS_STATE_ON_2      6     /* ... no frame received yet.                  */
#define EDDS_SRT_CONS_STATE_ON_MNS    7     /* ... no frame received yet, DHT expired      */
											/*                     (fire MISS_NOT_STOPPED) */
/*-----------------------------------------------------------------------------------------*/
#define EDDS_SRT_CONS_STATE_ON_AGAIN  8     /* ... valid frame arrived (fire AGAIN).       */
#define EDDS_SRT_CONS_STATE_ON_3      9     /* valid frame(s) arrived                      */
#define EDDS_SRT_CONS_STATE_ON_4     10     /* valid frame(s) arrived (wait for AGAIN/MISS)*/
/*-----------------------------------------------------------------------------------------*/

#define EDDS_CSRT_CONSUMER_NEW_FRAME_FALSE		0
#define EDDS_CSRT_CONSUMER_NEW_FRAME_TRUE		1
#define EDDS_CSRT_CONSUMER_NEW_FRAME_CLEARED	2

typedef struct _EDDS_CSRT_CONSUMER_FRAME_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR;

typedef struct _EDDS_CSRT_CONSUMER_TYPE
{
	LSA_UINT16                  CrNumber;       /* CrNumber from activation */
    LSA_UINT8                   Type;           /* Type of Consumer (zero if not in use) */
    /* Prevents further locking. */
    /* if TRUE. Is TRUE if       */
    /* Consumer not in use or if */
    /* someone  trys to lock the */
    /* buffer.                   */
    //LSA_BOOL                    LockProtected;
    LSA_UINT16                  DataLen;
    LSA_UINT16                  CycleReductionRatio;    /* 1,2,4,8,..128.... */
    LSA_UINT16                  CycleReductionNr;       /* 0,1,2,3...7....   */
    LSA_UINT16                  CyclePhase;
    LSA_UINT32                  KnotTableIdx;   /* Idx to Knot-table where   */
    /* Consumer is chained.      */
    LSA_UINT32                  UserID;
    LSA_UINT16                  WatchdogFactor; /* Factor for receive check  */
    /* 1,2,3,4..                 */
    LSA_UINT16                  WatchdogCnt;    /* actual counter            */
    LSA_UINT32                  WatchdogCycleCnt; /* Number of watchdog prov */
    /* cylces for ConsumerCheck*/
    LSA_UINT16                  DataHoldFactor; /* Factor for data-hold check*/
    /* 1,2,3,4..                 */
    LSA_UINT16                  DataHoldCnt;    /* actual counter            */

                                                /* Number of Providers using */
                                                /* this consumer for AutoStop*/
    LSA_UINT32                  ProviderAutoStopCnt;

                                                /* ProviderAutostop Enabled? */
    LSA_BOOL                    ProviderAutoStopEnabled;

    //LSA_BOOL                    BufferLocked;
    /* Prevents further locking. */
    /* if TRUE. Is TRUE if       */
    /* Provider not in USE or if */
    /* someone  trys to lock the */
    /* buffer.                   */

    LSA_UINT8                   ActStatusFlags; /* Actual Status Flags       */
    LSA_UINT8                   IndStatusFlags; /* Indicated Status Flags    */
    /* state within cycle-check. */
    LSA_UINT32                  SkipCycleStamp; /* Cycle Stamp for Skip check*/
    LSA_UINT32                  CycleCntStamp;  /* CycleStamp on receive.    */
    /* based on actual prov.     */
    /* Cyclecounter.             */
    /* Chains..------------------*/
    EDDS_SRT_CHAIN_TYPE         CheckLink;      /* chainelement for checklist*/
    EDDS_SRT_CHAIN_TYPE         UsedFreeLink;   /* chainelement for used or  */
    /* freelist                  */
} EDDS_CSRT_CONSUMER_TYPE;

typedef struct _EDDS_CSRT_CONSUMER_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_CSRT_CONSUMER_TYPE_PTR;

/* Consumer types */
#define EDDS_CSRT_CONSUMER_TYPE_NOT_USED    		0
#define EDDS_CSRT_CONSUMER_TYPE_XRT         		1
#define EDDS_CSRT_CONSUMER_TYPE_UDP         		2
#define EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS		0x80
#define EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS_MSK	(~EDDS_CSRT_CONSUMER_TYPE_CLEAR_ON_MISS)

#if 0 /*am2219*/
/* Consumer Modes */
#define EDDS_CSRT_CONSUMER_MODE_NORMAL          0
#define EDDS_CSRT_CONSUMER_MODE_ACCEPT_INVALID  1  /* not supported anymore ! */

/**@note we should refactore this indication/datastatus bit mixture. */
/* defines (masks) for ActStatusFlags and IndStatusFlags             */
/* for optimization we use the same bit locations as datastatus      */
/* bit 2 not used for datastatus, we use bit 2 for AGAIN/MISS(_NOT_STOPPED) */

#define EDDS_CSRT_CONS_STAT_MISS_AGAIN_MSK   0x4
#define EDDS_CSRT_CONS_STAT_AGAIN   	 	 0x4
#define EDDS_CSRT_CONS_STAT_MISS		   	 0x0
#endif /*am2219*/

/* STATE:            0: BACKUP  1: PRIMARY */
/* DIAG:             0: ERROR   1: OK      */
/* STOP_RUN          0: STOP    1: RUN     */

#define EDDS_CSRT_CONS_STAT_DEFAULT         ( EDD_CSRT_DSTAT_BIT_STATE         | \
EDD_CSRT_DSTAT_BIT_STOP_RUN      | \
EDD_CSRT_DSTAT_BIT_STATION_FAILURE )

#define EDDS_CSRT_CONS_STAT_DSTAT_MSK       ( EDD_CSRT_DSTAT_BIT_STATE         | \
EDD_CSRT_DSTAT_BIT_STOP_RUN      | \
EDD_CSRT_DSTAT_BIT_STATION_FAILURE  )

/* defines for SkipCheckFlag                                                 */
/*                                                                           */
/* This flag is used to handle the ability to signal MISS or AGAIN after the */
/* consumer was set to UNKNOWN-State within consumer control. We need to     */
/* signal MISS or AGAIN for the next consumer frame coming in a f t e r      */
/* setting the state to UNKNOWN. To do this we have to skip the next check   */
/* cycle (because this may do a check with a frame received before setting   */
/* the state to UNKNOWN!). In the next cycle after the skipped one, we have  */
/* to indication MISS or AGAIN independend of IndStatusFlag.                 */
/*                                                                           */
/* SKIP_STATE2 is set within Consumer-Control if SetStateToUnknown is set.   */
/*                                                                           */
/* Within check-cycle we do..                                                */
/*                                                                           */
/* SKIP_STATE2:  Set flag to STATE1. Now two cycles finished.                */
/* SKIP_STATE1:  Indicate MISS or AGAIN independend of actual IndStatusFlags */
/*               and set flag to NONE if indication was delivered to user.   */
/* SKIP_NONE:    No special handling within check cycle.                     */

#define EDDS_CSRT_CONS_SKIP_NONE    0
#define EDDS_CSRT_CONS_SKIP_STATE1  1
#define EDDS_CSRT_CONS_SKIP_STATE2  2

/*---------------------------------------------------------------------------*/
/* Management for indications.                                               */
/* All  RQBs with indication resources are queued here.                      */
/* If no indications are present an something is to be indicated,            */
/* ProvIndicationPending and/or ConsIndicationPending will be set to TRUE.   */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_INDICATION_TYPE
{
    LSA_BOOL                    ProvIndicationPending;  /* TRUE if provider  */
    /* indications pend. */
    LSA_BOOL                    ProvOverloadOccured;    /* Global provider   */
    /* overload occured. */
    LSA_BOOL                    ProvFirstSendOccured;   /* First Send Occured*/
    LSA_BOOL                    ConsIndicationPending;  /* TRUE if consumer  */
    /* indications pend. */
    LSA_BOOL                    ConsOverloadOccured;    /* Global consumer   */
    /* check overload.   */
    LSA_BOOL                    ConsCheckIndications;   /* Set when timer    */
    /* occured, reset    */
    /* after checks are  */
    /* done.             */
    EDDS_SRT_LINK_BASE_TYPE     RqbQueue;               /* Requestqueue      */
} EDDS_CSRT_INDICATION_TYPE;

/* ProvOverload is detected, if we got a new timerevent, but the old timer or */
/* Cycle is not finished.                                                     */
/* ConsOverload is detected, if we got a new timerevent, but the old timer or */
/* checking is not finished.                                                  */

typedef struct _EDDS_CSRT_INDICATION_TYPE EDDS_LOCAL_MEM_ATTR *    EDDS_CSRT_INDICATION_TYPE_PTR;

/*===========================================================================*/

typedef struct _EDDS_CSRT_IP_PARAMS_TYPE
{
    LSA_UINT8                           TTL;            /* TTL-Value                 */
} EDDS_CSRT_IP_PARAMS_TYPE;

typedef struct _EDDS_CSRT_CONS_MGM_TYPE
{
    LSA_UINT32                          ConsumerCnt;    /* Sum of all consumers      */
    LSA_UINT32                          ConsumerUsedCnt;/* Number of used consumers  */
    LSA_UINT32                          ConsumerActCnt; /* Number of active consumers*/
    EDDS_CSRT_CONS_CYCLE_TYPE           ConsumerCycle;  /* Management for Cycle.     */
    EDDS_SRT_CHAIN_BASE_TYPE            ConsumerFree;   /* LinkList of free consumer */
    EDDS_SRT_CHAIN_BASE_TYPE            ConsumerUsed;   /* LinkList of used consumer */
    LSA_UINT16                          ConsumerFrameIDBase1;   /* Table1: XRT Class 2, 0 if not used */
    LSA_UINT16                          ConsumerFrameIDTop1;    /* Table1: XRT Class 2, 0 if not used  */
    LSA_UINT16                          ConsumerFrameIDBase2;   /* Table2: XRT Class 1 /XRTOverUPD Class 4, 0 if not used */
    LSA_UINT16                          ConsumerFrameIDTop2;    /* Table2: XRT Class 1 /XRTOverUDP Class 4, 0 if not used */
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR   pConsFrameTbl;  /* Pointer to Consumer Table (all) start with Class 2  */
    EDDS_CSRT_CONSUMER_TYPE_PTR         pConsTbl;       /* Pointer to Consumer Table (all) start with Class 2  */
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR   pConsFrameTbl2; /* Help-Pointer to start of second table (Class1/4)    */
    /* within pConsFrameTbl                                */
    EDDS_CSRT_CONSUMER_TYPE_PTR         pConsTbl2;      /* Help-Pointer to start of second table (Class1/4)    */
    /* within pConsTbl                                     */
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR      pConsCrTbl;     /* Pointer to Consumer CR Table */
	/* clear on MISS buffer */
	EDD_UPPER_MEM_PTR_TYPE              pClearOnMissBuffer; /* buffer for ClearOnMiss */
    /* Holds the Ratio Infos    */
    EDDS_CSRT_CONS_RATIO_INFO_TYPE      ConsRatioInfo[EDDS_CSRT_CYCLE_RATIO_CNT];
} EDDS_CSRT_CONS_MGM_TYPE;


typedef struct _EDDS_CSRT_PROV_MGM_TYPE
{
    LSA_UINT32                          ProviderUsedCnt;/* Number of used providers  */
    LSA_UINT32                          ProviderActCnt; /* Number of active providers*/
    EDDS_CSRT_PROV_CYCLE_TYPE           ProviderCycle;  /* Management for Cycle.     */
    EDDS_CSRT_PROV_GROUP_TYPE_PTR       pProvGroupTbl;  /* Pointer to Provider Group */
    /* parameters.(array)        */

    EDDS_SRT_CHAIN_BASE_TYPE            ProviderFree;   /* LinkList of free provider    */
    EDDS_SRT_CHAIN_BASE_TYPE            ProviderUsed;   /* LinkList of used provider    */
    EDDS_CSRT_PROVIDER_TYPE_PTR         pProvTbl;       /* Pointer to Provider Table    */
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR      pProvCrTbl;     /* Pointer to Provider CR Table */
    /* Holds the Ratio Infos     */
    EDDS_CSRT_PROV_RATIO_INFO_TYPE     ProvRatioInfo[EDDS_CSRT_CYCLE_RATIO_CNT];
} EDDS_CSRT_PROV_MGM_TYPE;

typedef struct _EDDS_CSRT_KNOT_MGM_TYPE
{
    LSA_UINT32                          KnotCnt;        /* Number of Knots           */
    EDDS_CSRT_KNOT_TYPE_PTR             pKnotTbl;       /* Pointer to Knot-Table.    */
    LSA_UINT32                          KnotElementCnt; /* Number of Knot elements   */
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR     pKnotElementTbl;/* Pointer to Knot-Element-T.*/
    EDDS_SRT_CHAIN_BASE_TYPE            KnotElementFree;/* LinkList of free knot elem*/
    EDDS_SRT_CHAIN_BASE_TYPE            KnotElementUsed;/* LinkList of used knot elem*/
} EDDS_CSRT_KNOT_MGM_TYPE;

typedef struct _EDDS_DDB_COMP_CSRT_TYPE
{
    LSA_UINT32                          HandleCnt;      /* Number of handles using   */
    /* cSRT. (only one allowed)  */
    /* --------------------------*/
    /* Configvalues -------------*/
    /* --------------------------*/
    EDDS_CSRT_INI_TYPE                  Cfg;            /* from SRT-Init             */
    /* CycleBaseTime calculated  */
    /* from BaseFactor in ns     */
    /* --------------------------*/
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_SRT_TIMER_TYPE                 ProvCycleTimer; /* Provider Cycle Timer      */
    EDDS_SRT_TIMER_TYPE                 ConsCycleTimer; /* Consumer Check Cycle Timer*/
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    LSA_UINT16                          CycleStampAdjustShift;  /* number of digits  */
    /* to shift CycleTime*/
    /* to get 31.25us    */
    /* based time-stamp  */
    /* 5,6 or 7          */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_INTERNAL_REQUEST_TYPE          ProvTimerRQB;   /* Provider Timer-RQB        */
    EDDS_INTERNAL_REQUEST_TYPE          ConsTimerRQB;   /* Consumer Timer-RQB        */
    EDDS_INTERNAL_REQUEST_TYPE          ProvIndTriggerRQB;   /* Provider Indication  */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    EDDS_INTERNAL_REQUEST_TYPE          ConsIndTriggerRQB;   /* Consumer Indication  */
    /* Trigger RQB.         */
    /* --------------------------*/
    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
    /* --------------------------*/
    EDDS_CSRT_IP_PARAMS_TYPE            IPParams;       /* IP/UDP Params             */
    /* --------------------------*/
    #endif
	/* --------------------------*/
	EDD_COMMON_MEM_PTR_TYPE             pIOBufferManagement; /* CRT IOBuffer handler mgm*/
	                                                         /* in shared memory*/
    /* --------------------------*/
    EDDS_CSRT_CONS_MGM_TYPE             Cons;           /* Consumer parts------------*/
    /* --------------------------*/
    EDDS_CSRT_PROV_MGM_TYPE             Prov;           /* Provider parts------------*/
    /* --------------------------*/
    EDDS_CSRT_KNOT_MGM_TYPE             Knot;           /* Knots --------------------*/
    /* --------------------------*/
    EDDS_CSRT_INDICATION_TYPE           Indication;     /* Indications --------------*/
    /* --------------------------*/
    EDDS_CSRT_STAT_TYPE                 Stats;          /* Some internal statistics  */
    /* --------------------------*/

    /* *********************** */
    /* reduction for scheduler */
    /* *********************** */
    
    /* Internal CycleBaseFactor for scheduler : unit in ms   */
    LSA_UINT16 schedulerModuloFactor;
    
    /* internal counter for scheduler cycle  */
    LSA_UINT64 schedulerCounter;
    
} EDDS_DDB_COMP_CSRT_TYPE;

typedef struct _EDDS_DDB_COMP_CSRT_TYPE EDDS_LOCAL_MEM_ATTR *  EDDS_DDB_COMP_CSRT_TYPE_PTR;


/* ProviderDataStatus: Global Provider DataStatus put into ProviderFrames. */
/* set with EDD_SRV_SRT_PROVIDER_SET_STATE                                 */
/* For defines see requestblock. only Bits 4..7 of DataStatus!             */

/* CycleBaseFactor: Defines Cycletime in multiple of 31.25us.              */
/* we currently only support 32,64 and 128                                 */

#define EDDS_CSRT_CYCLE_BASE_TIME_NS (31250) /* basetime in ns */

/*===========================================================================*/
/* SRT Device management-structures                                          */
/*===========================================================================*/

typedef struct _EDDS_DDB_COMP_SRT_TYPE
{
    EDDS_DDB_COMP_CSRT_TYPE     CSRT;           /* cSRT management          */
} EDDS_DDB_COMP_SRT_TYPE;

typedef struct _EDDS_DDB_COMP_SRT_TYPE    EDDS_LOCAL_MEM_ATTR *    EDDS_LOCAL_DDB_COMP_SRT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
/*  end of file srt_inc.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SRT_INC_H */
