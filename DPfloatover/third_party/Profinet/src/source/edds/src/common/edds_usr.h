
#ifndef EDDS_USR_H                        /* ----- reinclude-protection ----- */
#define EDDS_USR_H

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
/*  F i l e               &F: edds_usr.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  28.04.09    AH    initial version. LSA_EDDS_P04.02.00.00_00.01.01.01     */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                              basic types for EDDP                         */
/*===========================================================================*/

#define EDDS_LOWER_MEM_PTR_TYPE              /* pointer to lower-memory */ \
LSA_VOID                    EDDS_LOWER_MEM_ATTR *

#define EDDS_LOCAL_MEM_PTR_TYPE               /* pointer to local-memory */ \
LSA_VOID                    EDDS_LOCAL_MEM_ATTR *

#define EDDS_LOCAL_MEM_U8_PTR_TYPE            /* pointer to local-memory */ \
LSA_UINT8                   EDDS_LOCAL_MEM_ATTR *

#define EDDS_LOCAL_MEM_U16_PTR_TYPE           /* pointer to local-memory */ \
LSA_UINT16                  EDDS_LOCAL_MEM_ATTR *

#define EDDS_LOCAL_MEM_U32_PTR_TYPE           /* pointer to local-memory */ \
LSA_UINT32                  EDDS_LOCAL_MEM_ATTR *


#define EDDS_DEV_MEM_PTR_TYPE                 /* pointer to device-memory */ \
LSA_VOID                    EDDS_LOWER_MEM_ATTR *

#define EDDS_DEV_MEM_U8_PTR_TYPE              /* pointer to device-memory */ \
LSA_UINT8                  EDDS_LOWER_MEM_ATTR *

#define EDDS_DEV_MEM_U32_PTR_TYPE             /* pointer to device-memory */ \
LSA_UINT32                  EDDS_LOWER_MEM_ATTR *

#define EDDS_DEV_MEM_U64_PTR_TYPE               /* pointer to device-memory */ \
LSA_UINT64                  EDDS_LOWER_MEM_ATTR *


/*---------------------------------------------------------------------------*/
/* Service within RQB for EDD_OPC_REQUEST and EDD_OPC_REQUEST_SRT_BUFFER     */
/*                                                                           */
/* Bits 08..15 used for decoding of service by EDDS-ERTEC. Do not change!    */
/*                                                                           */
/* Range 0x8000..0x8FFF can be used for EDDS depended services!              */
/* other special service values maybe reserved within edd_usr.h              */
/*---------------------------------------------------------------------------*/

/*=====  EDD_OPC_REQUEST:  EDDS debug  (0x8000..0x80FF)  ====*/
#define EDDS_SRV_DEBUG_TYPE                             (EDD_SERVICE)   0x8000

#define EDDS_SRV_GET_DEBUG_STATS                        (EDD_SERVICE)   0x8000
#define EDDS_SRV_DEBUG_SETUP_PHY                        (EDD_SERVICE)   0x8001

/*---------------------------------------------------------------------------*/
/* RQB- service-specific paramteter blocks, pParam points to.                */
/*      Opcode and Service specify the Paramblock                            */
/*                                                                           */
/*   RQB-Header                                                              */
/*  +------------+                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  +------------+ service-       e.g. EDDSP_UPPER_GET_STATS_PTR_TYPE        */
/*  |   pParam   | ------------->+-----------+                               */
/*  +------------+ specific      |           |                               */
/*                               | +------------+                            */
/*                               | |            |                            */
/*                               | | +-------------+                         */
/*                               | | |             |                         */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*======================= EDDS privat services ===============================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDS_SRV_GET_DEBUG_STATS                */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_GET_DEBUG_STATS_TYPE
{
    /* Global Statistics */
    struct
    {
        LSA_UINT32    RxXRToUDPIPWrongChecksum;   /* Wrong xRToverUPD Checksum */
        LSA_UINT32    RxXRToUDPWrongHeader;       /* Wrong xRToverUPD Header   */
        LSA_UINT32    TxResources;                /* Internal Tx-Resource error*/
        LSA_UINT32    RxOverloadCnt;              /* RxOverloads occured       */
    } Glob;

    /* NRT Statistics */
    struct
    {
        LSA_UINT32    RxARPIPSD;                  /* number of ARPIP-Frames discarded      */
        LSA_UINT32    RxASRTAlarmSD;              /* number of ASRT Alarm-Frames discarded */
        LSA_UINT32    RxASRTOtherSD;              /* number of ASRT Other-Frames discarded */
        LSA_UINT32    RxOtherSD;                  /* number of Other-Frames discarded      */
        LSA_UINT32    RxARPIP_Drop;               /* number of ARPIP-Frames dropped (no rqb)       */
        LSA_UINT32    RxLenDCP_Drop;              /* number of DCP-LEN   -Frames dropped (no rqb)  */
        LSA_UINT32    RxLenTSYNC_Drop;            /* number of TSYNC-LEN -Frames dropped (no rqb)  */
        LSA_UINT32    RxLenOther_Drop;            /* number of Type-Other -Frames dropped (no rqb) */
        LSA_UINT32    RxXRTDCP_Drop;              /* number of DCP-xRT   -Frames dropped (no rqb)  */
        LSA_UINT32    RxLLDP_Drop;                /* number of LLDP-Other -Frames dropped (no rqb) */
        LSA_UINT32    RxOther_Drop;               /* number of Other-Frames dropped (no rqb )      */
        LSA_UINT32    RxASRTAlarm_Drop;           /* number of ASRT-ALARM Frames dropped (no rqb ) */
        LSA_UINT32    RxUDP_ASRTAlarm_Drop;       /* number of UDP ASRT-Frames dropped (no rqb )   */
        LSA_UINT32    RxUDP_XRTDCP_Drop;          /* number of UDP DCP XRT-Frames dropped (no rqb )*/
        LSA_UINT32    RxPTP_Drop;                 /* number of PTP Frames dropped (no rqb)         */
        LSA_UINT32    RxMRP_Drop;                 /* number of MRP Frames dropped (no rqb)         */
        LSA_UINT32    RxDCPFilterDrop;            /* number of DCP-xRT  -Frames filter-drop.       */
        LSA_UINT32    RxDCPFilterInvalidFrame;    /* invalid DCP-IDENTIFY-Frame (length)           */
    } NRT;

    /* SRT Statistics */
    struct
    {
        LSA_UINT32    ProvCycleOverloadCnt;       /* Number of Overloads.      */
        LSA_UINT32    ProvCycleCounterDropCnt;    /* Number of times the Prov. */
        /* CycleCounter occured      */
        /* before the previous one   */
        /* was handled.              */
        /* (heavy overload?)         */
        LSA_UINT32    ConsCycleCounterDropCnt;    /* Number of times the Cons. */
        /* CycleCounter occured      */
        /* before the previous one   */
        /* was handled.              */
        /* (heavy overload?)         */
        LSA_UINT32    ConsCycleOverloadCnt;       /* Number of times the Cons. */
        /* Check-Cycle was overloaded*/
        LSA_UINT32    ConsCycleJitterDrop;        /* Consumer check dropped    */
        /* because of cycle-jitter.  */
        LSA_UINT32    ConsRxCycleCounterOld;      /* Number of old Frames      */
        LSA_UINT32    ConsRxWrongXRTLenOrMAC;     /* Number of XRT-Frames with */
        /* Wrong FrameLen or Src-MAC */
        LSA_UINT32    ConsRxWrongUDPLenOrIP;      /* Number of UDP-Frames with */
        /* Wrong UDP-Len or Src-IP . */
        LSA_UINT32    NoMoreKnotElements;         /* No more free Knot elements*/
    } SRT;

} EDDS_RQB_GET_DEBUG_STATS_TYPE;

typedef struct _EDDS_RQB_GET_DEBUG_STATS_TYPE EDD_UPPER_MEM_ATTR *   EDDS_UPPER_GET_DEBUG_STATS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDS_SRV_DEBUG_SETUP_PHY                */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_SETUP_PHY_TYPE
{
    LSA_UINT16          PortID;
    LSA_UINT8           LinkSpeedMode;
    LSA_UINT8           PHYPower;
} EDDS_RQB_SETUP_PHY_TYPE;

/* LinkSpeedMode: */

#if 0
/* already defined.. */
#define EDD_LINK_AUTONEG        0
#define EDD_LINK_100MBIT_HALF   1
#define EDD_LINK_100MBIT_FULL   2
#define EDD_LINK_10MBIT_HALF    3
#define EDD_LINK_10MBIT_FULL    4
#define EDD_LINK_1GBIT_HALF     5
#define EDD_LINK_1GBIT_FULL     6
#define EDD_LINK_10GBIT_HALF    7
#define EDD_LINK_10GBIT_FULL    8
#define EDD_LINK_UNCHANGED      9 /* No Changes for Speed and Mode */
#endif

#if 0
/* PHYPower */
/* already defined.. */
#define EDDS_PHY_POWER_ON        0  /* Switch the Phy on, -> Link-Ability    */
#define EDDS_PHY_POWER_OFF       1  /* Switch the Phy off -> No Link-Ability */
#define EDDS_PHY_POWER_UNCHANGED 2  /* No Changes on Phy                     */
#endif

typedef struct _EDDS_RQB_SETUP_PHY_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_SETUP_PHY_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/



#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDD_USR_H */
