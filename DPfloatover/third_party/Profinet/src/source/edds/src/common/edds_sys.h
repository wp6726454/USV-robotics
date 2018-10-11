#ifndef EDDS_SYS_H                  /* ----- reinclude-protection ----- */
#define EDDS_SYS_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: edds_sys.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*  Note: All output macros are in edd_out.h                                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*                    changed function attribut of edds_LED-functions        */
/*  22.10.03    JS    added FrameFilter for DCP and LLDP                     */
/*  20.01.04    JS    added edds_SRTProviderGetCycleCounter                  */
/*  13.02.04    JS    added support for ASRT over NRT-Interface              */
/*  22.03.04    JS    "volatile" moved into structure definition.            */
/*  16.04.04    JS    removed ASRT types                                     */
/*  02.07.04    JS    framefilter-defines moved to edd_usr.h                 */
/*  05.08.04    JS    added EDDS_FATAL_ERR_TX_RESOURCES                      */
/*  04.10.04    JS    moved EDDS_MAX_PORT_CNT from edds_dev.h                */
/*  09.02.05    JS    added EDDS_FATAL_ERR_REENTRANCE                        */
/*  11.05.05    JS    EDDS_CYCLE_COUNTER_TYPE now in edd_usr.h               */
/*  05.07.05    JS    Multiportsupport                                       */
/*  14.11.05    JS    added EDDS_MAX_PORT_CNT (moved from edd_usr.h)         */
/*  12.01.06    JS    added new NRT-Filter: EDD_NRT_FRAME_MRP                */
/*  09.03.06    JS    added EDDS_FATAL_ERR_ENDIANES                          */
/*  14.03.06    JS    added PRM. removed SWI and IRT                         */
/*  29.06.06    JS    added EDDS_UPPER_CDB_PRM_PTR_TYPE to CDB               */
/*  22.11.06    JS    FrameFilters extended (see edd_usr.h)                  */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
/*  17.06.08    JS    removed parameter ExtPaddingEnable from NRT-params     */
/*  04.05.09    JS    renamed EDDS_CFG_PORT_CNT to EDD_CFG_MAX_PORT_CNT      */
/*                    moved   EDDS_HANDLE from edd_usr.h                     */
/*                    more changes because of general edd_usr.h              */
/*  26.07.10    MA    added  new edds_interrupt_poll API function            */
#endif
/*****************************************************************************/

/**@note AP01167690, CR: RTOverUDP ablehnen */
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#warning actually RToverUDP is not supported in PNIO stack - EDDS will undefine EDDS_XRT_OVER_UDP_INCLUDE!
#undef EDDS_XRT_OVER_UDP_INCLUDE
#endif

#if defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) && defined (EDDS_CFG_SRT_INCLUDE)
#error "EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD can not be used together with EDDS_CFG_SRT_INCLUDE!"
#endif

#if defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) && defined (EDDS_CFG_ISR_MODE_STANDARD)
#error "EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD can not be set together with EDDS_CFG_ISR_MODE_STANDARD!"
#endif

#if !defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) && !defined (EDDS_CFG_ISR_MODE_STANDARD) && !defined (EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#error "Either EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD, EDDS_CFG_ISR_MODE_STANDARD or EDDS_CFG_CYCLIC_SCHEDULING_MODE has to be set!"
#endif

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* FrameID ranges                                                            */
/*---------------------------------------------------------------------------*/

#define EDDS_PTCP_SYNC_START			0x0080
#define EDDS_PTCP_SYNC_END				0x0081

#define EDDS_PTCP_DELAY_START			0xFF40
#define EDDS_PTCP_DELAY_END				0xFF43

#define EDDS_PTCP_ANNO_START			0xFF00
#define EDDS_PTCP_ANNO_END				0xFF01

#define EDDS_PTCP_SYNC_FOLLOWUP_START	0x0020
#define EDDS_PTCP_SYNC_FOLLOWUP_END		0x0021

#define EDDS_PTCP_FOLLOWUP_START		0xFF20
#define EDDS_PTCP_FOLLOWUP_END			0xFF21




/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for EDDS_FatalError())                             */
/*---------------------------------------------------------------------------*/

#define EDDS_FATAL_ERROR_TYPE LSA_UINT32

#define EDDS_FATAL_ERR_HANDLE_CLOSE    (EDDS_FATAL_ERROR_TYPE)  0x01
#define EDDS_FATAL_ERR_HANDLE_REL      (EDDS_FATAL_ERROR_TYPE)  0x02
#define EDDS_FATAL_ERR_DEV_INFO_CLOSE  (EDDS_FATAL_ERROR_TYPE)  0x03
#define EDDS_FATAL_ERR_CLOSE_DDB       (EDDS_FATAL_ERROR_TYPE)  0x04
#define EDDS_FATAL_ERR_NULL_PTR        (EDDS_FATAL_ERROR_TYPE)  0x05
#define EDDS_FATAL_ERR_MEM_FREE        (EDDS_FATAL_ERROR_TYPE)  0x06
#define EDDS_FATAL_ERR_LL              (EDDS_FATAL_ERROR_TYPE)  0x07
#define EDDS_FATAL_ERR_RELEASE_PATH_INFO (EDDS_FATAL_ERROR_TYPE) 0x08
#define EDDS_FATAL_ERR_ISR             (EDDS_FATAL_ERROR_TYPE)  0x09
#define EDDS_FATAL_ERR_SEND            (EDDS_FATAL_ERROR_TYPE)  0x0A
#define EDDS_FATAL_ERR_TIMEOUT_SEND    (EDDS_FATAL_ERROR_TYPE)  0x0B
#define EDDS_FATAL_ERR_TIMER_START     (EDDS_FATAL_ERROR_TYPE)  0x0C
#define EDDS_FATAL_ERR_INCONSISTENCE   (EDDS_FATAL_ERROR_TYPE)  0x0D
#define EDDS_FATAL_ERR_CHANNEL_USE     (EDDS_FATAL_ERROR_TYPE)  0x0E
#define EDDS_FATAL_ERR_TIMER_RESOURCE  (EDDS_FATAL_ERROR_TYPE)  0x0F
#define EDDS_FATAL_ERR_TX_RESOURCES    (EDDS_FATAL_ERROR_TYPE)  0x10
#define EDDS_FATAL_ERR_REENTRANCE      (EDDS_FATAL_ERROR_TYPE)  0x11
#define EDDS_FATAL_ERR_ENDIANES        (EDDS_FATAL_ERROR_TYPE)  0x12

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    /* these defines must be set if EDDS_CFG_CYCLIC_SCHEDULING_MODE is set! */
    #define EDDS_CFG_NRT_INCLUDE
    #define LLIF_CFG_SWITCH_SUPPORT
    #if defined(LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT)
    #error  LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT is not supported in EDDS_CFG_CYCLIC_SCHEDULING_MODE
    #endif /* defined(LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT) */
	#if defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE)
	#error  EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE is not supported in EDDS_CFG_CYCLIC_SCHEDULING_MODE
	#endif /* defined(EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE) */

#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

#define EDDS_HANDLE            LSA_VOID EDDS_LOCAL_MEM_ATTR *
/* for device-handle   */

#define EDDS_DETAIL_PTR_TYPE               /* pointer to EDDS_DETAIL */ \
struct edds_detail_tag      EDDS_SYSTEM_MEM_ATTR *

#define EDDS_FATAL_ERROR_PTR_TYPE         /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag  EDDS_LOCAL_MEM_ATTR *


/*===========================================================================*/

#define EDDS_MAX_PORT_CNT      EDD_CFG_MAX_PORT_CNT

/*---------------------------------------------------------------------------*/
/* usage of LSA_RQB_LOCAL_ERROR_TYPE                                         */
/* - lsa_component_id:                                                       */
/*    LSA_COMP_ID_EDDS                                                       */
/* - error_code_id not used. always 0                                        */
/* - error_code[12] used for following format:                               */
/*---------------------------------------------------------------------------*/

typedef struct EDDS_LOCAL_ERROR_CODE_
{
    LSA_OPCODE_TYPE Opcode;
    EDD_SERVICE     Service;
    LSA_UINT16      response;
} EDDS_LOCAL_ERROR_CODE_TYPE;

typedef struct EDDS_LOCAL_ERROR_CODE_ EDD_UPPER_RQB_ATTR *    EDDS_LOCAL_ERROR_CODE_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Service within RQB for EDD_OPC_SYSTEM                                     */
/*                                                                           */
/* Bits 08..15 used for decoding of service by EDDS. Do not change!          */
/*                                                                           */
/* Range 0x0000..0x0FFF reserved for Device-Services                         */
/*---------------------------------------------------------------------------*/

/*=====  EDD_OPC_SYSTEM:  System-Services (0x0100..0x01FF) ===*/
#define EDDSP_SRV_DEV_TYPE                              (EDD_SERVICE)   0x0100

#define EDDS_SRV_DEV_OPEN                               (EDD_SERVICE)   0x0100
#define EDDS_SRV_DEV_SETUP                              (EDD_SERVICE)   0x0101
#define EDDS_SRV_DEV_SHUTDOWN                           (EDD_SERVICE)   0x0102
#define EDDS_SRV_DEV_CLOSE                              (EDD_SERVICE)   0x0103
#define EDDS_SRV_DEV_SETUP_LINK                         (EDD_SERVICE)   0x0104
#define EDDS_SRV_DEV_COMP_NRT_INI                       (EDD_SERVICE)   0x0110
#define EDDS_SRV_DEV_COMP_NRT_REL                       (EDD_SERVICE)   0x0111
#define EDDS_SRV_DEV_COMP_SRT_INI                       (EDD_SERVICE)   0x0112
#define EDDS_SRV_DEV_COMP_SRT_REL                       (EDD_SERVICE)   0x0113
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#define EDDS_SRV_DEV_INTERRUPT                          (EDD_SERVICE)   0x0120
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
#define EDDS_SRV_DEV_TIMEOUT                            (EDD_SERVICE)   0x0121
#define EDDS_SRV_DEV_TRIGGER                            (EDD_SERVICE)   0x0122

/*---------------------------------------------------------------------------*/
/* Additional status within RQBs ?                                          */
/* ERR-Codes from Offset 0x2F..0x3E reserved for EDDS specific systemfunctions*/
/*---------------------------------------------------------------------------*/

/* EDDS OK-Code (MaxOffset = 0x3F) */
/* LSA_RSP_OK_OFFSET = 0x40       */

/* EDDS ERR-Codes (MaxOffset = 0x3E)*/
/* LSA_RSP_ERR_OFFSET = 0xC0       */


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
/*  +------------+ service-       e.g. EDDS_UPPER_DEV_OPEN_PTR_TYPE          */
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
/*======================= Device open/setup services ========================*/
/*===========================================================================*/

#ifdef LLIF_CFG_SWITCH_SUPPORT
/*---------------------------------------------------------------------------*/
/* switch- device management structure                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* MaxMRP_Instances           : Number of MRP instances supported for this (out)                        */
/*                              interface. This parameter is configured                                 */
/*                              within EDD startup.                                                     */
/*                              0: none. MRP not supported.                                             */
/*                              1.. EDD_CFG_MAX_MRP_INSTANCE_CNT                                        */
/* MRPDefaultRoleInstance0    : If MRP is supported but no MRP             (out)                        */
/*                              parameterization is present it is assumed                               */
/*                              that only one MRP instance is present.                                  */
/*                              In this case this parameter defines the MRP                             */
/*                              role for MRP instance 0.                                                */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/* MRPSupportedRole           : This parameter defines the possible MRP    (out)                        */
/*                              roles if only one MRP instance is present                               */
/*                              within parameterization (PRM).                                          */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/*                              Values can be ORed!                                                     */
/* MRPSupportedMultipleRole   : This parameter defines the possible MRP    (out)                        */
/*                              roles if multiple  MRP instance are present                             */
/*                              within parameterization (PRM).                                          */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/*                              Values can be ORed!                                                     */
/* MRPRingPort          : For each port of the interface this parameter defines if the port is          */
/*                        a ring port used with MRP and if this port is a default ring port on startup  */
/*                          -> possible defines for this parameter see <edd_usr.h>                      */
typedef struct _EDDS_DPB_SWI_TYPE
{
#ifndef EDDS_DONT_USE_MRP
    LSA_UINT8                       MaxMRP_Instances;
    LSA_UINT8                       MRPDefaultRoleInstance0;
    LSA_UINT8                       MRPSupportedRole;
    LSA_UINT8                       MRPSupportedMultipleRole;

    LSA_UINT8                       MRPRingPort[EDDS_MAX_PORT_CNT];
#else
	LSA_BOOL						DUMMY;
#endif
} EDDS_DPB_SWI_TYPE;

typedef EDDS_DPB_SWI_TYPE   EDD_UPPER_MEM_ATTR *   EDDS_UPPER_DPB_SWI_PTR_TYPE;
#endif

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_OPEN                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DPB_TYPE
{
    /* Global parameters ---*/
    EDDS_SYS_HANDLE             hSysDev;                /* system handle        */
    LSA_UINT32                  InterfaceID;            /* Interface ID         */
    LSA_UINT32                  TraceIdx;            	/* Interface ID         */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT16                  RxBufferCnt;            /* number of Rx-Buffers */
    /* 2..63                */
    LSA_UINT8                   TxBufferCnt;            /* number of Tx-Buffers */
    /* 1..62                */
    LSA_UINT32                  RxBufferSize;           /* Size of Rx-Buffer    */
    LSA_UINT32                  RxBufferOffset;         /* Start of Frame in    */
    /* buffer               */
    LSA_UINT8                   TxToutCnt;              /* Transmit-Timeout in s*/
    /* 0: none              */
#else  /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    LSA_UINT32                  RxBufferCnt;            /* number of Rx-Buffers [In/Out] */
    LSA_UINT32                  TxBufferCnt;            /* number of Tx-Buffers [In/Out] */
    LSA_BOOL                    ZeroCopyInterface;      /* use ZeroCopyInterface */
    LSA_UINT16                  schedulerCycle;         /* scheduler reduction   */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    LSA_VOID_PTR_TYPE		pLLFunctionTable;
    LSA_VOID_PTR_TYPE 		pLLManagementData;
    /* HW-dependend --------*/
    LSA_VOID_PTR_TYPE		pLLParameter;

    /* MaxPortCntOfAllEDD :    Number of all interfaces of all EDDs         (in) */
    /*                         max: EDD_CFG_MAX_INTERFACE_CNT                    */
    LSA_UINT16                          MaxPortCntOfAllEDD;

    /* MaxInterfaceCntOfAllEDD Number of all ports of all EDDs              (in) */
    /*                         max: EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE           */
    LSA_UINT16                          MaxInterfaceCntOfAllEDD;
#ifdef LLIF_CFG_SWITCH_SUPPORT
    EDDS_DPB_SWI_TYPE			SWI;
#endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    LSA_UINT32              RxIsrMaxFrameCnt;      /* specifies the maximum amount of frames to be */
                                                   /* received before aborting the receive handler */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
} EDDS_DPB_TYPE;

typedef EDDS_DPB_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DPB_PTR_TYPE;

/*--------------------------------------------------------------------------*/
/* Optimizations for zero-copy-interface:                                   */
/*--------------------------------------------------------------------------*/
/* RxBufferSize and RxBufferOffset are used for allocating receive-buffers  */
/* inside edds. RxBufferSize is the size of the whole buffer allocated.     */
/* RxBufferOffset is the offset from start where the frame will be put.     */
/* This is also the pointer which will be returned to the user when a frame */
/* is returned! (so there is RxBufferOffset free room before this address!) */
/* Note: RxBufferSize-RxBufferOffset >= EDD_FRAME_BUFFER_LENGTH             */
/* Note: When deallocating rx-buffers you must subtract RxBufferOffset from */
/*       the frame-buffer pointer!                                          */
/* Note: The edds does not allocate or frees any transmitbuffers exchanged  */
/*       whith the user.                                                    */
/* Note: RxBufferOffset must be a multiple of the rx-buffer-alignment       */
/*       (i.e. a multiple of (EDDS_RX_FRAME_BUFFER_ALIGN+1)                 */
/*--------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_INI_TYPE
{
    EDDS_HANDLE             hDDB;
    EDDS_UPPER_DPB_PTR_TYPE pDPB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_INI_TYPE;

typedef EDDS_RQB_DDB_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_INI_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*=================  SETUP/SHUTDOWN/CLOSE DEVICE  ===========================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_SETUP                      */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DSB_TYPE
{
	LSA_UINT8           LinkSpeedMode;
	LSA_UINT8           _dummy[3];
	EDD_MAC_ADR_TYPE    MACAddress[EDD_CFG_MAX_PORT_CNT+1];
} EDDS_DSB_TYPE;

typedef struct _EDDS_DSB_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DSB_PTR_TYPE;


/* for use with EDD_SRV_SETUP_PHY         */

/* PHYPower */
#define EDDS_PHY_POWER_ON        0  /* Switch the Phy on, -> Link-Ability    */
#define EDDS_PHY_POWER_OFF       1  /* Switch the Phy off -> No Link-Ability */
#define EDDS_PHY_POWER_UNCHANGED 2  /* No Changes on Phy                     */
/* for use with EDD_SRV_SETUP_PHY        */

typedef struct _EDDS_RQB_DDB_SETUP_TYPE
{
    EDDS_HANDLE             hDDB;
    EDDS_UPPER_DSB_PTR_TYPE pDSB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_SETUP_TYPE;

typedef struct _EDDS_RQB_DDB_SETUP_TYPE EDD_UPPER_MEM_ATTR *   EDDS_UPPER_DDB_SETUP_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_SHUTDOWN                   */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_SHUTDOWN_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_SHUTDOWN_TYPE;

typedef struct _EDDS_RQB_DDB_SHUTDOWN_TYPE EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_CLOSE                      */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_REL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_REL_TYPE;

typedef struct _EDDS_RQB_DDB_REL_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_REL_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_SETUP_LINK                 */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_SETUP_LINK_TYPE
{
    EDDS_HANDLE         hDDB;
    LSA_UINT16          PortID;
    LSA_UINT8           LinkSpeedMode;
    LSA_UINT8           PHYPower;
    LSA_VOID            LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_SETUP_LINK_TYPE;

/* defines for LinkSpeedMode see above */
/* defines for PHYPower see above      */

typedef struct _EDDS_RQB_SETUP_LINK_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_SETUP_LINK_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*=========================  INI COMPONENTS  ================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_COMP_NRT_INI               */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_CMP_NRT_INI_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_UINT32              RxBufferCntARPIP;     /* number of rx-buffers to alloc for NRT ARP/IP*/
    LSA_UINT32              RxBufferCntASRTOther; /* number of rx-buffers to alloc for ASRT other*/
    LSA_UINT32              RxBufferCntASRTAlarm; /* number of rx-buffers to alloc for ASRT alarm*/
    LSA_UINT32              RxBufferCntOther;     /* number of rx-buffers to alloc for NRT Other */
    LSA_BOOL                RxFilterUDP_Unicast;
    LSA_BOOL                RxFilterUDP_Broadcast;

    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_CMP_NRT_INI_TYPE;

typedef struct _EDDS_RQB_CMP_NRT_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CMP_NRT_INI_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_COMP_SRT_INI               */
/*---------------------------------------------------------------------------*/

/* cyclic SRT init-Structure */

/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS ON                                                        */
/*---------------------------------------------------------------------------*/
#include "pnio_pck4_on.h"

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE
{
	LSA_UINT16                ProviderCrCnt;       // amount of providers that are configured
	LSA_UINT16                ConsumerCrCnt;       // amount of consumers that are configured
	EDD_COMMON_MEM_PTR_TYPE   pProviderCrTable;    // pointer to provider Cr table
	EDD_COMMON_MEM_PTR_TYPE   pConsumerCrTable;    // pointer to consumer Cr table
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE       EDDS_CSRT_MGM_INFOBLOCK_TYPE;
typedef struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE   *   EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR;

#include "pnio_pck_off.h"
/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS OFF                                                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CSRT_INI_TYPE
{
    LSA_UINT16              CycleBaseFactor;
    LSA_UINT16              ConsumerFrameIDBaseClass2;
    LSA_UINT16              ConsumerCntClass2;
    LSA_UINT16              ConsumerFrameIDBaseClass1;
    LSA_UINT16              ConsumerCntClass1;
    LSA_UINT16              ProviderCnt;                   /* XRT+XRToverUDP */
    LSA_UINT16              ProviderCycleDropCnt;
    LSA_UINT32              ProviderCyclePhaseMaxCnt1ms;   /* maximum supported provider at 1ms */
	EDD_COMMON_MEM_PTR_TYPE hIOBufferManagement;           /* CRT IOBuffer handler mgm */
	                                                       /* in shared memory         */
} EDDS_CSRT_INI_TYPE;

typedef struct _EDDS_CSRT_INI_TYPE EDD_UPPER_MEM_ATTR *    EDDS_CSRT_INI_PTR_TYPE;

/* ConsumerFrameIDBase has to be >= EDD_SRT_FRAMEID_CSRT_START and           */
/* ConsumerFrameIDBase+ConsumerCnt has to be <= EDD_SRT_FRAMEID_CSRT_STOP    */

/* CycleBaseFactor                                                           */
/* Base factor for calculation of base cycle time (=CycleBaseFactor* 31.25us)*/
/* For calculation of Cycle-Time: 250...4000us                               */
/* We currently only support 32,64,128 (=1000us,2000us,4000us Cylce-Time)    */

/* ConsumerFrameIDBaseClass2                                                 */
/* Start of FrameID-Range valid for RT-Class 2 Consumers                     */
/* Range: EDD_SRT_FRAMEID_CSRT_START1 ..  EDD_SRT_FRAMEID_CSRT_STOP1         */
/* Note: Shall be used from Class 2 FrameIDs                                 */

/* ConsumerCntClass2                                                         */
/* Max RT-Class2 Consumers supported. (SRT)                                  */
/* Range: 1..(EDD_SRT_FRAMEID_CSRT_STOP1-ConsumerFrameIDBase1+1)             */

/* ConsumerFrameIDBaseClass1                                                 */
/* Start of FrameID-Range valid for Class1 Consumers                         */
/* Range: EDD_SRT_FRAMEID_CSRT_START2  ..  EDD_SRT_FRAMEID_CSRT_STOP2        */
/* Note: Shall be used from Class 1 FrameIDs                                 */

/* ConsumerCntClass1                                                         */
/* Max RT-Class1 Consumers supported (SRT and xRToverUDP)                    */
/* Range: 1..(EDD_SRT_FRAMEID_CSRT_STOP2-ConsumerFrameIDBase2+1)             */

/* ProviderCnt                                                               */
/* Max Provider supported.                                                   */
/* Range: 1..EDDS_SRT_PROVIDER_CNT_MAX                                       */

#define EDDS_SRT_PROVIDER_CNT_MAX               0x7FFF   /* < 0xFFFF ! */

/* ProviderCycleDropCnt                                                      */
/* Number of provider cycles to drop on machine overload (if we can not send */
/* all providers within cycle.                                               */
/* 0: no drops                                                               */

#define EDDS_SRT_PROVIDER_CYCLE_DROP_MAX        255

/* ProviderCylcePhaseMaxByteCnt 1)                                           */
/* Max number of i/o-bytes for all providers within one cycle                */
/* ProviderCylcePhaseMaxCnt 1)                                               */
/* Max number of providers within one cycle.                                 */
/* 1) used with EDD_SRV_SRT_PROVIDER_ADD                                     */
/* ProviderCylcePhaseMaxByteCntHard 2)                                       */
/* Max number of i/o-bytes for all providers within one cycle. hard-limit    */
/* ProviderCylcePhaseMaxCntHard 2)                                           */
/* Max number of providers within one cycle. hard-limit for                  */
/* 2) used with EDD_SRV_SRT_PROVIDER_CHANGE_PHASE.                           */

typedef struct _EDDS_RQB_CMP_SRT_INI_TYPE
{
    EDDS_HANDLE             hDDB;
    EDDS_CSRT_INI_TYPE      CSRT;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_CMP_SRT_INI_TYPE;

typedef struct _EDDS_RQB_CMP_SRT_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CMP_SRT_INI_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/


/*===========================================================================*/
/*=========================  REL COMPONENTS  ================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_COMP_NRT_REL               */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_CMP_NRT_REL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_CMP_NRT_REL_TYPE;

typedef struct _EDDS_RQB_CMP_NRT_REL_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CMP_NRT_REL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_COMP_SRT_REL               */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_CMP_SRT_REL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_CMP_SRT_REL_TYPE;

typedef struct _EDDS_RQB_CMP_SRT_REL_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CMP_SRT_REL_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================= INTERNAL & SERVICES   =============================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Filled and used internal!!!                                               */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_INTERRUPT                  */
/*                          Service: EDDS_SRV_DEV_TIMEOUT                    */
/*                          Service: EDDS_SRV_DEV_TRIGGER                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DEV_INTERNAL_TYPE EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE;

typedef struct _EDDS_RQB_DEV_INTERNAL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_UINT32              ID;     /* Sub-ID                      */
    LSA_UINT32              Param;  /* service-dependend parameter */
} EDDS_RQB_DEV_INTERNAL_TYPE;

/* EDDS_SRV_DEV_INTERRUPT (Param not used)  */
/* IDs:                                     */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

#define EDDS_ISR_TX         1               /* Frame transmitted            */
#define EDDS_ISR_RX_NRT     2               /* NRT frame received           */
#define EDDS_ISR_RX_cSRT    3               /* cyclic  SRT-frame received   */
#define EDDS_ISR_PHY        4               /* PHY-Interrupt                */

/* EDDS_SRV_DEV_TIMEOUT                                                   */
/* IDs:                                                                   */

#define EDDS_TIMEOUT_TX     0               /* transmit timeout occured   */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
#define EDDS_TIMEOUT_LL     1               /* LL timeout (delayed function) occured   */

/* EDDS_SRV_DEV_TRIGGER                     */
/* IDs:                                     */

#define EDDS_TRIGGER_PROV_INDICATION  0      /* trigger a provider indicat.  */
#define EDDS_TRIGGER_CONS_INDICATION  1      /* trigger a consumer indicat.  */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#define EDDS_TRIGGER_PROVIDER_CYCLE   2      /* Provider Cycle               */
#define EDDS_TRIGGER_CONSUMER_CYCLE   3      /* Consumer Check Cycle         */
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
#define EDDS_TRIGGER_NRT_CANCEL_PART2 4      /* Trigger NRT-Cancel Part 2    */
#define EDDS_TRIGGER_RX_OVER_CYCLE    5      /* Rx Overload cycle timer      */
#define EDDS_TRIGGER_RX_OVER_EVENT    6      /* RX Overload event            */
#define EDDS_TRIGGER_LED_BLINK_CYCLE  7      /* LED blink timer event        */
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#define EDDS_TRIGGER_DIAG_CYCLE       8      /* Diagnose/prm cylce timer     */
#endif


/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by EDDS_GET_PATH_INFO)                          */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* CDB- Parameter for NRT                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CDB_NRT_TYPE
{
    LSA_UINT32  FrameFilter;        /* defines NRT-Frames to be receive for  */
    /* this channel                          */
} EDDS_CDB_NRT_TYPE;

/* defines for FrameFilter. Can be ORed. see edd_usr.h, EDD_SRV_NRT_RECV */

/* predefined substitution of FrameFilter: */

#define EDDS_NRT_FRAME_IP (EDD_NRT_FRAME_IP_ICMP |    \
EDD_NRT_FRAME_IP_TCP   |    \
EDD_NRT_FRAME_IP_UDP   |    \
EDD_NRT_FRAME_ARP)


#define EDDS_NRT_FRAME_LEN      (EDD_NRT_FRAME_LEN_DCP |  EDD_NRT_FRAME_LEN_TSYNC | EDD_NRT_FRAME_LEN_OTHER )

typedef struct _EDDS_CDB_NRT_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CDB_NRT_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* CDB- Parameter for PRM                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CDB_PRM_TYPE
{
    LSA_BOOL   PortDataAdjustLesserCheckQuality;
} EDDS_CDB_PRM_TYPE;

typedef struct _EDDS_CDB_PRM_TYPE EDD_UPPER_MEM_ATTR *EDDS_UPPER_CDB_PRM_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* PortDataAdjustLesserCheckQuality:                                         */
/*                                                                           */
/* LSA_TRUE:   Do a lesser strict check (see table below)                    */
/* LSA_FALSE:  No special handling.                                          */
/*                                                                           */
/* PDPortDataAdjust checking rules:                                          */
/*                                                                           */
/* --------+--------------------+----------------------+-------------------- */
/*  PRM    |     IEC61158       |  LesserCheckQuality  |     Local-Flag      */
/* --------+--------------------+----------------------+-------------------- */
/*  WRITE  | PortState = OFF    |             PortState = OFF                */
/*         |                    |                 10 FD/HD                   */
/*         |      100 FD        |                100 FD/HD                   */
/*         |     1000 FD        |               1000 FD/HD                   */
/*         |    10000 FD        |              10000 FD/HD                   */
/*         |     Autoneg 1)     |                 Autoneg 1)                 */
/* --------+--------------------+----------------------+-------------------- */
/*  END    | At least one port has to be set to        |  no restrictions    */
/*         | Autoneg, 100 FD, 1000 FD or 10000 FD      |                     */
/* --------+-------------------------------------------+-------------------- */
/*                                                                           */
/* IEC61158           : Checking rule according to IEC61158.                 */
/* LesserCheckQuality : Rule if PortDataAdjustLesserCheckQuality is set      */
/* Local-Flag         : Rule if "Local"  flag set with PRM-request by CMPD.  */
/*                                                                           */
/*  1) Autoneg is used if no MAUType or PortState is present for this port.  */
/*     This is the default setting.                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* CDB- Parameter for IRT                                                    */
/*---------------------------------------------------------------------------*/

typedef LSA_VOID    EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CDB_IRT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* CDB- Parameter for SWI                                                    */
/*---------------------------------------------------------------------------*/

typedef LSA_VOID    EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CDB_SWI_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in EDDS_GET_PATH_INFO)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_COMP_USE_TYPE
{
    LSA_BOOL     UseNRT;          /* LSA_TRUE: NRT-Component used by channel */
    LSA_BOOL     UseCSRT;         /* LSA_TRUE:cSRT-Component used by channel */
    LSA_BOOL     UsePRM;          /* LSA_TRUE: PRM-Component used by channel */
} EDDS_COMP_USE_TYPE;

typedef struct edds_detail_tag
{
    EDDS_COMP_USE_TYPE          UsedComp;
    EDDS_HANDLE                 hDDB;
    LSA_BOOL                    InsertSrcMAC; /* True: edds inserts src-mac in */
    /*       send frame             */
    EDDS_UPPER_CDB_NRT_PTR_TYPE pNRT;
    EDDS_UPPER_CDB_PRM_PTR_TYPE pPRM;
} EDDS_CDB_TYPE;


/*===========================================================================*/
/*                                 SWITCH                                    */
/*===========================================================================*/

typedef struct _EDDS_RQB_SWI_SET_PORT_REDUNDANT_TYPE
{
    LSA_BOOL PortID[EDD_CFG_MAX_PORT_CNT];
} EDDS_RQB_SWI_SET_PORT_REDUNDANT_TYPE;

typedef struct _EDDS_RQB_SWI_SET_PORT_REDUNDANT_TYPE EDD_UPPER_MEM_ATTR * EDDS_UPPER_SWI_SET_PORT_REDUNDANT_PTR_TYPE;

typedef struct _EDDS_RQB_SWI_SET_BRIDGE_MODE_TYPE
{
    LSA_UINT32    Mode;
} EDDS_RQB_SWI_SET_BRIDGE_MODE_TYPE;

typedef struct _EDDS_RQB_SWI_SET_BRIDGE_MODE_TYPE EDD_UPPER_MEM_ATTR * EDDS_UPPER_SWI_SET_BRIDGE_MODE_PTR_TYPE;

/* Mode */
#define EDDS_BRIDGE_MODE_SF         0   /* store and forward mode */
#define EDDS_BRIDGE_MODE_CT         1   /* cut through mode       */


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                              external functions                           */
/*===========================================================================*/

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  edds_init
 *
 * function:       initialize EDDS
 *
 * parameters:     ....
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_init(LSA_VOID);

/*=============================================================================
 * function name:  edds_undo_init
 *
 * function:       undo the initialization of EDDS
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_undo_init(LSA_VOID);

/*=============================================================================
 * function name:  edds_system
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDDS_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_system(EDD_UPPER_RQB_PTR_TYPE upper_rqb_ptr);


/*=============================================================================
 * function name:  edds_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDD_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (EDD_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of edds
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_open_channel(EDD_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  edds_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDDS_OPC_CLOS_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_close_channel(EDD_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  edds_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDDS_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_request(EDD_UPPER_RQB_PTR_TYPE upper_rqb_ptr);


/*=============================================================================
 * function name:  edds_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDS_SYSTEM_IN_FCT_ATTR edds_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id);


#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*=============================================================================
 * function name:  edds_interrupt
 *
 * function:       Edds-ISR-Handler called in isr
 *
 * parameters:     EDDS_HANDLE          hDDB
 *                 LSA_UINT32           IntSource
 *
 * return value:   LSA_VOID
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_interrupt(
    EDDS_HANDLE const hDDB,
    LSA_UINT32 IntSource);


/*=============================================================================
 * function name:  edds_interrupt_poll
 *
 * function:       Interrupt source polling
 *                 returns LSA_TRUE if interrupt source is active
 *
 * parameters:     EDDS_HANDLE          hDDB
 *                 LSA_UINT32*			IntSource [OUT]
 *
 * return value:   LSA_UINT32 OUT parameter:
 *                 		interrupt source,
 *                 		EDDS_INTERRUPT_NONE if no interrupt is pending.
 *                 LSA_BOOL:
 *                 		LSA_TRUE if more interrupt sources are pending.
 *                 		LSA_FALSE if no other interrupt sources are pending.
 *
 *===========================================================================*/

LSA_BOOL  EDD_UPPER_IN_FCT_ATTR  edds_interrupt_poll(
    EDDS_HANDLE const hDDB, LSA_UINT32* IntSource);

#else /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*=============================================================================
 * function name:  edds_scheduler
 *
 * function:       EDDS scheduler called cyclically from system adaption (1ms)
 *
 * parameters:     EDDS_HANDLE          hDDB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_scheduler( EDDS_HANDLE const hDDB );

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*=============================================================================
 * function name:  edds_ll_start_timer
 *
 * function:       start a one shot timer for LL
 *                 returns LSA_RET_OK on success.
 *
 * parameters:     EDDS_HANDLE          hDDB
 * 				   LSU_UINT16           StandbyTime
 *
 *===========================================================================*/

LSA_VOID EDDS_LOCAL_FCT_ATTR edds_ll_start_timer(
		EDDS_HANDLE const hDDB,
		LSA_UINT16 StandbyTime
		);

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*=============================================================================
 * function name:  EDDS_TRIGGER_SW_INTERRUPT
 *
 * function:       software generation of interrupt source
 *
 * parameters:     EDDS_HANDLE          hDDB
 *                 LSA_UINT32           IntSource
 *
 * return value:   LSA_VOID
 *===========================================================================*/

LSA_VOID EDDS_TRIGGER_SW_INTERRUPT(EDDS_HANDLE const hDDB, LSA_UINT32 IntSource);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/* define for IntSource (meaning depends on hardware!) */
#define  EDDS_INTERRUPT_NONE            0    /* no interrupt. discard.       */
#define  EDDS_INTERRUPT_TX              1    /* Interrupt for Tx             */
#define  EDDS_INTERRUPT_RX              2    /* Interrupt for Rx             */
#define  EDDS_INTERRUPT_PHY             3    /* Interrupt for PHY            */

/*=============================================================================
 * function name:  edds_SRTProviderGetCycleCounter
 *
 * function:       Get SRT-provider CycleCounter (31.25us ticks)
 *
 * parameters:     EDDS_HANDLE                 hDDB
 *                 EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/

typedef struct _EDDS_CYCLE_COUNTER_TYPE
{
    LSA_UINT32      High;
    LSA_UINT32      Low;
} EDDS_CYCLE_COUNTER_TYPE;

typedef struct _EDDS_CYCLE_COUNTER_TYPE EDD_UPPER_MEM_ATTR * EDDS_CYCLE_COUNTER_PTR_TYPE;

LSA_RESULT EDDS_SYSTEM_IN_FCT_ATTR edds_SRTProviderGetCycleCounter(
    EDDS_HANDLE                     hDDB,
    EDDS_CYCLE_COUNTER_PTR_TYPE     pCycleCounter);

/*---------------------------------------------------------------------------*/
/* this structure is  used for internal request initiated by interrupt or    */
/* timeout-handler. pParam in pRQB points to EDDS_RQB_DEV_INTERNAL_TYPE      */
/* InUse is set if the RQB is underway (from Int-Handler to edds_system(). if */
/* it arrives edds_system() InUse is reset to LSA_FALSE. So only one RQB is  */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef volatile struct _EDDS_INTERNAL_REQUEST_STRUCT
{
    LSA_BOOL                    InUse;          /* LSA_TRUE if in rqb-queue  */
    EDD_UPPER_RQB_PTR_TYPE      pRQB;
} EDDS_INTERNAL_REQUEST_TYPE;

typedef EDDS_INTERNAL_REQUEST_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_INTERNAL_REQUEST_PTR_TYPE;

/*===========================================================================*/
/* Capabilities                                                              */
/*===========================================================================*/
typedef struct _EDDS_LL_CAPS_TYPE
{
    LSA_UINT32                         MaxFrameSize;
    LSA_UINT32                         HWFeatures;
    LSA_UINT32                         SWFeatures;
    LSA_UINT32                         PortCnt;
} EDDS_LL_CAPS_TYPE;

typedef struct _EDDS_LL_CAPS_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE;
/*-----------------------------------------------------------------------*/
/* MaxFrameSize         Size in byte of maximum frame for send/receive   */
/*                                                                       */
/* HWFeatures                                                            */
#define EDDS_LL_CAPS_HWF_NONE                            (0)
#define EDDS_LL_CAPS_HWF_CRC_GENERATE                    (1/*<<0*/)
#define EDDS_LL_CAPS_HWF_CRC_DECODE                      (1<<1)
#define EDDS_LL_CAPS_HWF_IPV4_CHK_GENERATE               (1<<2)
#define EDDS_LL_CAPS_HWF_IPV4_CHK_DECODE                 (1<<3)
#define EDDS_LL_CAPS_HWF_TCPV4_CHK_GENERATE              (1<<4)
#define EDDS_LL_CAPS_HWF_TCPV4_CHK_DECODE                (1<<5)
#define EDDS_LL_CAPS_HWF_UDPV4_CHK_GENERATE              (1<<6)
#define EDDS_LL_CAPS_HWF_UDPV4_CHK_DECODE                (1<<7)
#define EDDS_LL_CAPS_HWF_TCPV6_CHK_GENERATE              (1<<8)
#define EDDS_LL_CAPS_HWF_TCPV6_CHK_DECODE                (1<<9)
#define EDDS_LL_CAPS_HWF_UDPV6_CHK_GENERATE              (1<<10)
#define EDDS_LL_CAPS_HWF_UDPV6_CHK_DECODE                (1<<11)
/*                                                                       */
/* SWFeatures                                                            */
#define EDDS_LL_CAPS_SWF_NONE                            (0)
#define EDDS_LL_CAPS_SWF_SRT                             (1/*<<0*/)
#define EDDS_LL_CAPS_SWF_SEND_ASYNCHRONOUS               (1<<1)
#define EDDS_LL_CAPS_SWF_SEND_BLOCK_MODE                 (1<<2)
#define EDDS_LL_CAPS_SWF_SEND_CYCLE_BASED                (1<<3)
#define EDDS_LL_CAPS_SWF_SEND_ISOCHRONOUS                (1<<4)
#define EDDS_LL_CAPS_SWF_XRT_OVER_UDP                    (1<<5)
/*                                                                       */
/* PortCnt              Number of ports on controller; if PortCnt >1,    */
/*                      controller is a switch.                          */
/*                                                                       */
/*-----------------------------------------------------------------------*/

/*===========================================================================*/
/* Statistics                                                                */
/*===========================================================================*/

typedef struct _EDDS_STATISTICS_TYPE
{
    LSA_UINT32      InOctets;
    LSA_UINT32      InUcastPkts;
    LSA_UINT32      InNUcastPkts;
    LSA_UINT32      InDiscards;
    LSA_UINT32      InErrors;
    LSA_UINT32      InUnknownProtos;
    LSA_UINT32      OutOctets;
    LSA_UINT32      OutUcastPkts;
    LSA_UINT32      OutNUcastPkts;
    LSA_UINT32      OutDiscards;
    LSA_UINT32      OutErrors;
    LSA_UINT32      OutQLen;
} EDDS_STATISTICS_TYPE;

typedef struct _EDDS_STATISTICS_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_LOCAL_STATISTICS_PTR_TYPE;


/* OBJECT IDENTITIES for MAU types */

#define EDDS_LL_MAUTYPE_AUI			1
//  DESCRIPTION "no internal MAU, view from AUI"
//  REFERENCE   "[IEEE802.3], Section 7"

#define EDDS_LL_MAUTYPE_10BASE5		2
//	DESCRIPTION "thick coax MAU"
//	REFERENCE   "[IEEE802.3], Section 7"

#define EDDS_LL_MAUTYPE_FOIRL			3
//  DESCRIPTION "FOIRL MAU"
//  REFERENCE   "[IEEE802.3], Section 9.9"

#define EDDS_LL_MAUTYPE_10BASE2		4
//  DESCRIPTION "thin coax MAU"
//  REFERENCE   "[IEEE802.3], Section 10"

#define EDDS_LL_MAUTYPE_10BASET		5
//  DESCRIPTION "UTP MAU.
//              Note that it is strongly recommended that
//              agents return either dot3MauType10BaseTHD or
//              dot3MauType10BaseTFD if the duplex mode is
//              known.  However, management applications should
//              be prepared to receive this MAU type value from
//              older agent implementations."
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASEFP 		6
//  DESCRIPTION "passive fiber MAU"
//  REFERENCE   "[IEEE802.3], Section 16"

#define EDDS_LL_MAUTYPE_10BASEFB		7
//  DESCRIPTION "sync fiber MAU"
//  REFERENCE   "[IEEE802.3], Section 17"

#define EDDS_LL_MAUTYPE_10BASEFL		8
//  DESCRIPTION "async fiber MAU.
//              Note that it is strongly recommended that
//              agents return either dot3MauType10BaseFLHD or
//              dot3MauType10BaseFLFD if the duplex mode is
//              known.  However, management applications should
//              be prepared to receive this MAU type value from
//              older agent implementations."
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_10BROAD36		9
//  DESCRIPTION "broadband DTE MAU.
//              Note that 10BROAD36 MAUs can be attached to
//              interfaces but not to repeaters."
//  REFERENCE   "[IEEE802.3], Section 11"

//------ new since RFC 1515:

#define EDDS_LL_MAUTYPE_10BASETHD		10
//  DESCRIPTION "UTP MAU, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASETFD		11
//  DESCRIPTION "UTP MAU, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASEFLHD		12
//  DESCRIPTION "async fiber MAU, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_10BASEFLFD		13
//  DESCRIPTION "async fiber MAU, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_100BASET4		14
//  DESCRIPTION "4 pair category 3 UTP"
//  REFERENCE   "[IEEE802.3], Section 23"

#define EDDS_LL_MAUTYPE_100BASETXHD	15
//  DESCRIPTION "2 pair category 5 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 25"

#define EDDS_LL_MAUTYPE_100BASETXFD	16
//  DESCRIPTION "2 pair category 5 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 25"

#define EDDS_LL_MAUTYPE_100BASEFXHD	17
//  DESCRIPTION "X fiber over PMT, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 26"

#define EDDS_LL_MAUTYPE_100BASEFXFD	18
//  DESCRIPTION "X fiber over PMT, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 26"

#define EDDS_LL_MAUTYPE_100BASET2HD	19
//  DESCRIPTION "2 pair category 3 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 32"

#define EDDS_LL_MAUTYPE_100BASET2FD	20
//  DESCRIPTION "2 pair category 3 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 32"

//------ new since RFC 2239:

#define EDDS_LL_MAUTYPE_1000BASEXHD	21
//  DESCRIPTION "PCS/PMA, unknown PMD, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 36"

#define EDDS_LL_MAUTYPE_1000BASEXFD	22
//  DESCRIPTION "PCS/PMA, unknown PMD, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 36"

#define EDDS_LL_MAUTYPE_1000BASELXHD	23
//  DESCRIPTION "Fiber over long-wavelength laser, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASELXFD	24
//  DESCRIPTION "Fiber over long-wavelength laser, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASESXHD	25
//  DESCRIPTION "Fiber over short-wavelength laser, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASESXFD	26
//  DESCRIPTION "Fiber over short-wavelength laser, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASECXHD	27
//  DESCRIPTION "Copper over 150-Ohm balanced cable, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 39"

#define EDDS_LL_MAUTYPE_1000BASECXFD	28
//  DESCRIPTION "Copper over 150-Ohm balanced cable, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 39"

#define EDDS_LL_MAUTYPE_1000BASETHD	29
//  DESCRIPTION "Four-pair Category 5 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 40"

#define EDDS_LL_MAUTYPE_1000BASETFD	30
//  DESCRIPTION "Four-pair Category 5 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 40"

//------ new since RFC 2668:

#define EDDS_LL_MAUTYPE_10GIGBASEX		31
//  DESCRIPTION "X PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 48"

#define EDDS_LL_MAUTYPE_10GIGBASELX4	32
//  DESCRIPTION "X fiber over WWDM optics"
//  REFERENCE   "[IEEE802.3], Section 53"

#define EDDS_LL_MAUTYPE_10GIGBASER		33
//  DESCRIPTION "R PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 49"

#define EDDS_LL_MAUTYPE_10GIGBASEER	34
//  DESCRIPTION "R fiber over 1550 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASELR	35
//  DESCRIPTION "R fiber over 1310 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASESR	36
//  DESCRIPTION "R fiber over 850 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASEW		37
//  DESCRIPTION "W PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 49 and 50"

#define EDDS_LL_MAUTYPE_10GIGBASEEW	38
//  DESCRIPTION "W fiber over 1550 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASELW	39
//  DESCRIPTION "W fiber over 1310 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASESW	40
//  DESCRIPTION "W fiber over 850 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

//------ new since RFC 3636:

#define EDDS_LL_MAUTYPE_10GIGBASECX4	41
//  DESCRIPTION "X copper over 8 pair 100-Ohm balanced cable"
//  REFERENCE   "[IEEE802.3], Section 54"

#define EDDS_LL_MAUTYPE_2BASETL		42
//  DESCRIPTION "Voice grade UTP copper, up to 2700m, optional PAF"
//  REFERENCE   "[IEEE802.3], Sections 61 and 63"

#define EDDS_LL_MAUTYPE_10BASETS 43
//  DESCRIPTION "Voice grade UTP copper, up to 750m, optional PAF"
//  REFERENCE   "[IEEE802.3], Sections 61 and 62"

#define EDDS_LL_MAUTYPE_100BASEBX10D	44
//  DESCRIPTION "One single-mode fiber OLT, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"


#define EDDS_LL_MAUTYPE_100BASEBX10U	45
//  DESCRIPTION "One single-mode fiber ONU, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"

#define EDDS_LL_MAUTYPE_100BASELX10	46
//  DESCRIPTION "Two single-mode fibers, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"

#define EDDS_LL_MAUTYPE_1000BASEBX10D	47
//  DESCRIPTION "One single-mode fiber OLT, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASEBX10U	48
//  DESCRIPTION "One single-mode fiber ONU, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASELX10	49
//  DESCRIPTION "Two sigle-mode fiber, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASEPX10D	50
//  DESCRIPTION "One single-mode fiber EPON OLT, 10km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX10U	51
//  DESCRIPTION "One single-mode fiber EPON ONU, 10km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX20D	52
//  DESCRIPTION "One single-mode fiber EPON OLT, 20km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX20U	53
//  DESCRIPTION "One single-mode fiber EPON ONU, 20km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_10GBASET		54
//  DESCRIPTION "Four-pair Category 6A or better, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 55"

#define EDDS_LL_MAUTYPE_10GBASELRM		55
//  DESCRIPTION "R multimode fiber over 1310 nm optics"
//  REFERENCE   "IEEE Std 802.3, Clause 68"

#define EDDS_LL_MAUTYPE_1000BASEKX		56
//  DESCRIPTION "X backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 70"

#define EDDS_LL_MAUTYPE_10GBASEKX4		57
//  DESCRIPTION "4 lane X backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 71"

#define EDDS_LL_MAUTYPE_10GBASEKR		58
//  DESCRIPTION "R backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 72"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD1	59
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting low
//               power budget (PRX10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD2	60
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting
//              medium power budget (PRX20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD3	61
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting high
//              power budget (PRX30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU1	62
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting low
//              power budget (PRX10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU2	63
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting
//              medium power budget (PRX20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU3	64
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting high
//              power budget (PRX30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD1		65
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting low
//              power budget (PR10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD2		66
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting
//              medium power budget (PR20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD3		67
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting high
//              power budget (PR30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRU1		68
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON ONU, supporting
//              low and medium power budget (PR10 and PR20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRU3		69
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON ONU, supporting high
//              power budget (PR30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_40GBASEKR4		70
//    DESCRIPTION "40GBASE-R PCS/PMA over an electrical
//                 backplane"
//    REFERENCE   "IEEE Std 802.3, Clause 84"

#define EDDS_LL_MAUTYPE_40GBASECR4		71
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 lane shielded
//                 copper balanced cable"
//    REFERENCE   "IEEE Std 802.3, Clause 85"

#define EDDS_LL_MAUTYPE_40GBASESR4		72
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 lane multimode
//                 fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 86"

#define EDDS_LL_MAUTYPE_40GBASEFR		73
//    DESCRIPTION "40GBASE-R PCS/PMA over single mode
//                 fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 89"

#define EDDS_LL_MAUTYPE_40GBASELR4		74
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 87"

#define EDDS_LL_MAUTYPE_100GBASECR10	75
//    DESCRIPTION "100GBASE-R PCS/PMA over 10 lane
//                 shielded copper balanced cable"
//    REFERENCE   "IEEE Std 802.3, Clause 85"

#define EDDS_LL_MAUTYPE_100GBASESR10	76
//    DESCRIPTION "100GBASE-R PCS/PMA over 10 lane
//                 multimode fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 86"

#define EDDS_LL_MAUTYPE_100GBASELR4		77
//    DESCRIPTION "100GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber, long reach"
//    REFERENCE   "IEEE Std 802.3, Clause 88"

#define EDDS_LL_MAUTYPE_100GBASEER4		78
//    DESCRIPTION "100GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber PMD, extended reach"
//    REFERENCE   "IEEE Std 802.3, Clause 88"


/*****************************************************************************/
/*                                                                           */
/* Output-functions are in edd_out.h                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  end of file EDDS_SYS.H                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_SYS_H */
