
#ifndef LLDP_INT_H /* ----- reinclude-protection ----- */
#define LLDP_INT_H

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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_int.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  LLDP.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
/*  15.07.04    JS    GetParams instead of GetMACAddr                        */
/*  22.09.04    JS    changed ethertype to 0x88CC (from 0xC000)  according to*/
/*                    802.1AB D11.                                           */
/*  18.10.04    JS    added txDelayCnt                                       */
/*  15.11.04    JS    use LLDP_RX_FRAME_BUFFER_LENGTH                        */
/*  06.10.05    JS    "DoSend" added.                                        */
/*  03.05.07    JS    added use of LSA_TIMER_ID_TYPE                         */
/*  26.06.08    JS    added "volatile" to be more save against compiler      */
/*                    code optimizations (changing instruction order)        */
#endif
/*****************************************************************************/

#include "lldp_trc.h"

#ifdef __cplusplus /* If C++ - compiler: Use C linkage */
extern "C" {
#endif

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/* ========================================================================= */

/* LLDP module IDs used in C-Files */

#define LLDP_MODULE_ID_LLDP_USR 0x0
#define LLDP_MODULE_ID_LLDP_SYS 0x1
#define LLDP_MODULE_ID_LLDP_GLB 0x2
#define LLDP_MODULE_ID_LLDP_DBG 0x3
#define LLDP_MODULE_ID_LLDP_TIM 0x4
#define LLDP_MODULE_ID_LLDP_EDD 0x5
#define LLDP_MODULE_ID_LLDP_TLV 0x6

/*===========================================================================*/
/*                             IEEE-802.3 defines                            */
/*===========================================================================*/

#define LLDP_FRAME_DST_MAC_OFFSET 0
#define LLDP_FRAME_SRC_MAC_OFFSET 6
#define LLDP_FRAME_TYPE_OFFSET 12
#define LLDP_FRAME_TLV_OFFSET 14

#define LLDP_FRAME_HEADER_SIZE LLDP_FRAME_TLV_OFFSET

#define LLDP_MIN_TLV_DATA_SIZE 46 /* Min TLV-Chain length       */
/* without END-TLV            */

#define LLDP_MAX_FRAME_LENGTH (LLDP_MAX_TLV_DATA_SIZE + LLDP_FRAME_TLV_OFFSET)

#define LLDP_ETHERTYPE 0x88CC

/*===========================================================================*/
/*                   some internal configuration settings                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef LLDP_IP_TYPE LLDP_LOCAL_MEM_ATTR *LLDP_LOCAL_MEM_IP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* This structure is  used for internal request initiated by timeout-handler.*/
/* InUse is set if the RQB is underway (from timeout to LLDP_system(). if    */
/* it arrives LLDP_system() InUse is reset to LSA_FALSE. So only one RQB is  */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef volatile struct _LLDP_INTERNAL_REQUEST_STRUCT {
  LSA_BOOL InUse; /* LSA_TRUE if in rqb-queue  */
  LLDP_UPPER_RQB_PTR_TYPE pRQB;
} LLDP_INTERNAL_REQUEST_TYPE;

typedef LLDP_INTERNAL_REQUEST_TYPE LLDP_LOCAL_MEM_ATTR
    *LLDP_INTERNAL_REQUEST_PTR_TYPE;

/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

typedef volatile struct _LLDP_TIMER_INFO_TYPE {
  LSA_BOOL InUse;
  LSA_TIMER_ID_TYPE TimerID;
  LLDP_INTERNAL_REQUEST_TYPE IntRQB;
  LSA_VOID LSA_FCT_PTR(LLDP_LOCAL_FCT_ATTR,
                       Cbf)(LLDP_INTERNAL_REQUEST_PTR_TYPE pIntRQB);
} LLDP_TIMER_INFO_TYPE;

typedef volatile struct _LLDP_TIMER_INFO_TYPE LLDP_LOCAL_MEM_ATTR
    *LLDP_TIMER_INFO_PTR_TYPE;

#define LLDP_TIMER_ID_INVALID 0xFFFF

/*===========================================================================*/
/* Handle-management-structures                                              */
/*===========================================================================*/

/* general handle parameters */
typedef struct _LLDP_HANDLE_PARAMS_TYPE {
  LSA_SYS_PATH_TYPE SysPath;
  LSA_SYS_PTR_TYPE pSys;
  LLDP_DETAIL_PTR_TYPE pDetail;
  LSA_HANDLE_TYPE HandleUpper;
  LSA_VOID LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR,
                       Cbf)(LLDP_UPPER_RQB_PTR_TYPE pRQB);
} LLDP_HANDLE_PARAMS_TYPE;

/* --------------------------------------------------------------------------*/
/* For EDD Channel Port handling                                             */
/* --------------------------------------------------------------------------*/

/* Link Status of a Port */
typedef struct _LLDP_LINK_STATUS_TYPE {
  LSA_UINT8 Status;
  LSA_UINT8 Speed; /* not used yet */
  LSA_UINT8 Mode;  /* not used yet */
} LLDP_LINK_STATUS_TYPE;

/* defines for Status/Speed/Mode see EDD_RQB_GET_LINK_STATUS_TYPE */

/* Indication Status of a Port */
typedef struct _LLDP_RX_INDICATION_TYPE {
  LSA_UINT8 StateIndicated; /* state indicated to user */
  LSA_BOOL Changed;         /* Data valid but changed  */
  /* since last indication!  */
  LSA_BOOL IndLastInvalid; /* LSA_TRUE if we have to  */
  /* indicate, that the last */
  /* sender got invalid.     */
  LLDP_MAC_TYPE LastMACAddr; /* last-indicated sender   */
                             /* to user. (may not be the*/
                             /* actual one!)            */
} LLDP_RX_INDICATION_TYPE;

/* RX-Data */
typedef struct _LLDP_RX_FRAME_INFO_TYPE {
  LSA_UINT8 State;    /* actual state            */
  LSA_UINT32 ThisIdx; /* Index of this element   */
  /* in array  = senderid    */
  LLDP_RX_INDICATION_TYPE Ind; /* indication infos        */
  LSA_UINT32 TTLCnt;           /* TTL in Seconds          */
  LSA_UINT32 ElapseCnt;        /* Counter the Data gets   */
  /* invalid if no new Data  */
  /* arrives.                */
  LSA_UINT32 FrameLen; /* whole frame length      */
  LSA_UINT8 Frame[LLDP_RX_FRAME_BUFFER_LENGTH];
  LSA_UINT32 TLVDataLength;
  LSA_UINT32 TLVDataOffset;
} LLDP_RX_FRAME_INFO_TYPE;

typedef struct _LLDP_RX_FRAME_INFO_TYPE LLDP_LOCAL_MEM_ATTR
    *LLDP_RX_FRAME_INFO_PTR_TYPE;

/* State/StateIndicated */
/* note:  if State <> DATA_VALID -> the rx-resource is free for new sender */

#define LLDP_RX_FRAME_STATE_DATA_INVALID \
  0x00 /* invalid Data because of TTL expired */
#define LLDP_RX_FRAME_STATE_SHUTDOWN \
  0x01 /* Shutdown received. Data invalid     */
#define LLDP_RX_FRAME_STATE_DATA_VALID \
  0x02 /* Valid Data present (or changed)     */

/* Tx-Data */
typedef struct _LLDP_TX_FRAME_INFO_TYPE {
  LSA_UINT8 State;  /* actual state            */
  LSA_BOOL DataNew; /* New Frame? Set to LSA_TRUE after    */
  /* changing Frame. Reset after sended  */
  /* first time and copied to pFrame     */
  LSA_BOOL Underway; /* LSA_TRUE if Tx in progress          */
  LSA_BOOL DoSend;   /* LSA_TRUE if we should send after    */
  /* confirmation of current send.       */
  LSA_UINT32 Interval;       /* SendInterval in seconds             */
  LSA_UINT32 NextSend;       /* Next Send Timer counter.            */
  LSA_UINT32 ReinitDelayCnt; /* Number of ticks to delay    */
  /* after Shutdown              */
  LSA_UINT32 TxDelayCnt; /* Number of ticks to delay    */
  /* after change of data or link*/
  LSA_UINT32 DataLen; /* Number of Bytes valid in Frame  and */
  /* pFrame if used.                     */
  LSA_UINT8 Frame[LLDP_MAX_FRAME_LENGTH];
#ifdef LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
  LLDP_EDD_LOWER_MEM_PTR_TYPE pFrame; /* Pointer to Tx-Frame memory.         */
#endif
} LLDP_TX_FRAME_INFO_TYPE;

/* State */
#define LLDP_TX_FRAME_STATE_STOP 0 /* Tx stopped no data set              */
#define LLDP_TX_FRAME_STATE_STOP_SET 1 /* Tx stopped data set */
#define LLDP_TX_FRAME_STATE_STOP_SEND_SHUTDOWN \
  2 /* Stop in progress. must send shutdown*/
#define LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN \
  3 /* Stop in progress. wait shutdown send*/
#define LLDP_TX_FRAME_STATE_STOP_SHUTDOWN_DELAY \
  4                                 /* Shutdown send wait reinit delay.    */
#define LLDP_TX_FRAME_STATE_START 5 /* Tx started (and data set)           */

/* LowerLayer (EDD) Request PORT Control (PCtrl) */
typedef struct _LLDP_EDD_PCTRL_TYPE LLDP_LOCAL_MEM_ATTR
    *LLDP_EDD_PCTRL_PTR_TYPE;

typedef struct _LLDP_EDD_PCTRL_TYPE {
  LSA_UINT32 ThisIdx; /* Index of this element in array      */
  /* = portid                            */
  LLDP_MAC_TYPE MACAddr;            /* MAC Address of Port                 */
  LLDP_LINK_STATUS_TYPE LinkStatus; /* Current LinkStatus                  */
  LSA_UINT8 TooManyState;           /* TooMany-Sender state       */
  LLDP_RX_FRAME_INFO_PTR_TYPE pRx;  /* Rx-Data (array with PortMaxSender)  */
  LLDP_TX_FRAME_INFO_TYPE Tx;       /* Tx-Data                             */
  LLDP_UPPER_RQB_PTR_TYPE pStopReq; /* Pending STOP-Request                */
  LSA_UINT16 StopPORTId;            /* PortID from STOP-Request            */
  LLDP_UPPER_STATS_TYPE Stats;      /* Statistics                          */
  LSA_BOOL RxStoped;                /* indicates port mustn't receive      */
} LLDP_EDD_PCTRL_TYPE;

/* TooMany-State */
#define LLDP_RX_FRAME_NOT_TOO_MANY \
  0x00 /* not too many sender yet.            */
#define LLDP_RX_FRAME_TOO_MANY_OCCURED \
  0x01 /* Multiple Sender (MAC) occured       */
#define LLDP_RX_FRAME_TOO_MANY_IND \
  0x02 /* Multiple Sender (MAC) occured and   */
/* indicated to user                   */

/* --------------------------------------------------------------------------*/
/* For UserChannel                                                           */
/* --------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_LINK_BASE_TYPE {
  LLDP_UPPER_RQB_PTR_TYPE pBottom;
  LLDP_UPPER_RQB_PTR_TYPE pTop;
} LLDP_RQB_LINK_BASE_TYPE;

typedef struct _LLDP_HDB_TYPE LLDP_LOCAL_MEM_ATTR *LLDP_HDB_PTR_TYPE;

typedef struct _LLDP_HDB_TYPE {
  LSA_UINT16 State;               /* handle state                   */
  LSA_HANDLE_TYPE ThisHandle;     /* this handle                    */
  LLDP_HANDLE_PARAMS_TYPE Params; /* general handle parameters      */
  LLDP_UPPER_RQB_PTR_TYPE pOCReq; /* Open or Close Request          */
  LLDP_RSP OpenResponse;          /* Response from Open if error    */
  LSA_UINT32 IndReqPending;       /* Number of pending ind. request */
  LLDP_RQB_LINK_BASE_TYPE IndReq; /* Indication Requests            */
  /* occured while opening.        */
  LSA_HANDLE_TYPE EDDHandle; /* If Open the lower EDD handle.  */
  LLDP_MAC_TYPE MACAddr;     /* MAC Address of NIC             */
  /* One RCTRL for every port      */
  LSA_UINT32 PortCnt;             /* Number of Ports                */
  LSA_UINT32 PortMaxSender;       /* Number of Sender per Port (Rx) */
  LSA_UINT32 InStartCnt;          /* Number of active Ports (START) */
  LSA_UINT32 PortGetParamsCnt;    /* Number of Ports Params get     */
  LLDP_EDD_PCTRL_PTR_TYPE pPCtrl; /* array of Port Controls.        */
  LSA_UINT32 RxPendingCnt;        /* Number of receive requests     */
  /* pending in EDD                */
  LSA_UINT32 TxPendingCnt; /* Number of transmit requests    */
  /* pending in EDD                */
  LSA_UINT32 LinkIndPendingCnt; /* Number of Link-Ind-Req         */
  /* pending in EDD                */
  LSA_BOOL ReleaseSysPtr; /* if TRUE: the sysptr must be released after RQB is
                             returned  */
  /*          to the upper layer using LLDP_REQUEST_UPPER_DONE() */
  LSA_UINT32 TraceIdx; /* Trace index delivered by EDDx */
  LSA_UINT32 LLDP_ChannelTxDelay;
  LSA_UINT32 LLDP_ChannelReInitDelay;
  LSA_UINT32 LLDP_ChannelMsgTxInterval;
} LLDP_HDB_TYPE;

/* State: */
#define LLDP_HANDLE_STATE_FREE 0
#define LLDP_HANDLE_STATE_CLOSED 1
#define LLDP_HANDLE_STATE_OPEN_EDD_RUNNING 2
#define LLDP_HANDLE_STATE_CANCEL_EDD_RUNNING \
  3 /* cancel NRT-Requests          */
#define LLDP_HANDLE_STATE_CLOSE_EDD_RUNNING 4
#define LLDP_HANDLE_STATE_GET_MAC_EDD_RUNNING \
  5 /* get MAC of EDD-Channel       */
#define LLDP_HANDLE_STATE_GET_PORT_MAC_EDD_RUNNING \
  6 /* get PortMACs                 */
#define LLDP_HANDLE_STATE_SET_MC_EDD_RUNNING \
  7 /* Set Multicast of EDD running */
#define LLDP_HANDLE_STATE_DISABLE_MC_EDD_RUNNING \
  8 /* Disable MC while closing cha */
#define LLDP_HANDLE_STATE_DISABLE_MC_ERROR_EDD_RUNNING \
  9 /* Disable MC because of error  */
#define LLDP_HANDLE_STATE_CANCEL_ERROR_EDD_RUNNING \
  10 /* cancel because of open error */
#define LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING \
  11                                          /* close because of open error  */
#define LLDP_HANDLE_STATE_CLOSE_WAIT_TIMER 12 /* wait for timer while closing \
                                                 */
#define LLDP_HANDLE_STATE_OPEN 15

typedef struct _LLDP_HDB_INFO_TYPE {
  LSA_UINT16 HandleCnt; /* Number of handles supported        */
  /* currently only 1                   */
  LSA_UINT16 UsedCnt; /* currently used user-channels       */
  /* currently only 1                   */
  LLDP_HDB_TYPE HDB[LLDP_CFG_MAX_CHANNELS];
  /* to array of MaxHandle              */
  /* LLDP_HDB_TYPE types.               */
} LLDP_HDB_INFO_TYPE;

/* --------------------------------------------------------------------------*/
/* global LLDP-management (handle independend)                               */
/* --------------------------------------------------------------------------*/

#define LLDP_CFG_MAX_TIMER 1 /* number of Timers for all  */
/* devices                   */

typedef struct _LLDP_GLOB_TYPE {
  LLDP_TIMER_INFO_TYPE Timer[LLDP_CFG_MAX_TIMER]; /* Timermanagement     */
  LSA_UINT16 CyclicTimerID;                       /* Timer_ID of cylcic  */
  /* 1s timer.           */
  LSA_UINT32 CyclicTimerCnt;     /* TimerCounter        */
  LSA_UINT32 CyclicTimerUserCnt; /* number of user of   */
  /* timer               */
  LSA_UINT8 Temp[LLDP_MAX_FRAME_LENGTH]; /* Temp-TLV            */
  LSA_RQB_LOCAL_ERROR_TYPE LLError;      /* last LL-Error       */
} LLDP_GLOB_TYPE;

typedef struct _LLDP_GLOB_TYPE LLDP_LOCAL_MEM_ATTR *LLDP_GLOB_PTR_TYPE;

/* --------------------------------------------------------------------------*/
/* global LLDP-management anchor                                             */
/* --------------------------------------------------------------------------*/

typedef struct _LLDP_MGM_TYPE LLDP_LOCAL_MEM_ATTR *LLDP_MGM_PTR_TYPE;

typedef struct _LLDP_MGM_TYPE {
  LLDP_GLOB_TYPE Glob;      /* Global Mgm.        */
  LLDP_HDB_INFO_TYPE HUser; /* User-Handle Mgm    */
} LLDP_MGM_TYPE;

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

LSA_EXTERN LLDP_MGM_PTR_TYPE g_pLLDPInfo; /* global LLDP-INFO-Structure */

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

#define LLDP_MGM_GET_GLOB_PTR() (&(g_pLLDPInfo->Glob))
#define LLDP_MGM_GET_HUSER_PTR() (&(g_pLLDPInfo->HUser))

#if 0
/*===========================================================================*/
/*                           Access to network bytes                         */
/*===========================================================================*/

#ifndef LLDP_CFG_BIG_ENDIAN
/* flips a byte pattern of 0102 to 0201 */
#define LLDP_FLIP16(C) (LSA_UINT16)((((C) >> 8) & 0xFF) | (((C) << 8) & 0xFF00))
/* flips a byte pattern of 01020304 to 04030201 */
#define LLDP_FLIP32(C)                                \
  ((LSA_UINT32)(((((LSA_UINT32)C) >> 24) & 0xFF) |    \
                ((((LSA_UINT32)C) >> 8) & 0xFF00) |   \
                ((((LSA_UINT32)C) << 8) & 0xFF0000) | \
                ((((LSA_UINT32)C) << 24) & 0xFF000000)))
#else
#define LLDP_FLIP16(C) (C)
#define LLDP_FLIP32(C) (C)
#endif

#ifndef LLDP_NTOHS
#define LLDP_NTOHS(Value) (LLDP_FLIP16(Value)) /* Network TO Host Short */
#endif

#ifndef LLDP_NTOHL
#define LLDP_NTOHL(Value) (LLDP_FLIP32(Value)) /* Network TO Host Long  */
#endif

#ifndef LLDP_HTONS
#define LLDP_HTONS(Value) (LLDP_FLIP16(Value)) /* Host To Network Short */
#endif

#ifndef LLDP_NTONL
#define LLDP_HTONL(Value) (LLDP_FLIP32(Value)) /* Host TO Network Long  */
#endif
#endif
#ifndef LLDP_COPY_MAC_ADDR_LOC_LER
#define LLDP_COPY_MAC_ADDR_LOC_LER(_DstPtr, _SrcPtr)               \
  {                                                                \
    int i;                                                         \
    for (i = 0; i < LLDP_MAC_ADDR_SIZE; i++)                       \
      *(((LSA_UINT8 LLDP_LOCAL_MEM_ATTR *)(_DstPtr)) + i) =        \
          *(((LSA_UINT8 LLDP_EDD_LOWER_MEM_ATTR *)(_SrcPtr)) + i); \
  }
#endif

#ifndef LLDP_COPY_MAC_ADDR_COMMON
#define LLDP_COPY_MAC_ADDR_COMMON(_DstPtr, _SrcPtr)             \
  {                                                             \
    int i;                                                      \
    for (i = 0; i < LLDP_MAC_ADDR_SIZE; i++)                    \
      *(((LSA_UINT8 LLDP_COMMON_MEM_ATTR *)(_DstPtr)) + i) =    \
          *(((LSA_UINT8 LLDP_COMMON_MEM_ATTR *)(_SrcPtr)) + i); \
  }
#endif

/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  LLDP_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define LLDP_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block) \
  {                                                   \
    LLDP_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);      \
    LLDP_RQB_SET_PREV_RQB_PTR(_Block, _Top);          \
    if (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) {   \
      _Top = _Block;                                  \
      _Bottom = _Block;                               \
    } else {                                          \
      LLDP_RQB_SET_NEXT_RQB_PTR(_Top, _Block);        \
      _Top = _Block;                                  \
    }                                                 \
  }

/*=============================================================================
 * function name:  LLDP_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define LLDP_RQB_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block) \
  {                                                      \
    LLDP_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);          \
    LLDP_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);         \
    if (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) {   \
      _Bottom = _Block;                                  \
      _Top = _Block;                                     \
    } else {                                             \
      LLDP_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);        \
      _Bottom = _Block;                                  \
    }                                                    \
  }

/*=============================================================================
 * function name:   LLDP_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block) \
  {                                                   \
    _Block = _Top;                                    \
    if (!LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) {  \
      _Top = LLDP_RQB_GET_PREV_RQB_PTR(_Top);         \
      if (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))   \
        _Bottom = LSA_NULL;                           \
      else                                            \
        LLDP_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);    \
    }                                                 \
  }

/*=============================================================================
 * function name:   LLDP_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block) \
  {                                                      \
    _Block = _Bottom;                                    \
                                                         \
    if (!LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) {  \
      _Bottom = LLDP_RQB_GET_NEXT_RQB_PTR((_Bottom));    \
      if (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   \
        _Top = LSA_NULL;                                 \
      else                                               \
        LLDP_RQB_SET_PREV_RQB_PTR((_Bottom), LSA_NULL);  \
    }                                                    \
  }

/*=============================================================================
 * function name:   LLDP_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)                \
  {                                                                       \
    if ((LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) ||                  \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) ||                     \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)))                    \
      _Block = LSA_NULL;                                                  \
    else {                                                                \
      _Block = _Bottom;                                                   \
      while ((!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Handle))) &&              \
             (!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Top))))                   \
        _Block = LLDP_RQB_GET_NEXT_RQB_PTR(_Block);                       \
                                                                          \
      if (!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Handle)))                     \
        _Block = LSA_NULL;                                                \
      else {                                                              \
        if (LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom)) {                  \
          LLDP_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);               \
        } else {                                                          \
          if (LSA_HOST_PTR_ARE_EQUAL((_Block), _Top)) {                   \
            LLDP_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);                \
          } else {                                                        \
            LLDP_RQB_SET_NEXT_RQB_PTR(LLDP_RQB_GET_PREV_RQB_PTR(_Block),  \
                                      LLDP_RQB_GET_NEXT_RQB_PTR(_Block)); \
            LLDP_RQB_SET_PREV_RQB_PTR(LLDP_RQB_GET_NEXT_RQB_PTR(_Block),  \
                                      LLDP_RQB_GET_PREV_RQB_PTR(_Block)); \
          }                                                               \
        }                                                                 \
      }                                                                   \
    }                                                                     \
  }

/*=============================================================================
 * function name:   LLDP_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)          \
  {                                                                        \
    if ((LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) ||                   \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) || ((_Handle) == 0))    \
      _Block = LSA_NULL;                                                   \
    else {                                                                 \
      _Block = _Bottom;                                                    \
                                                                           \
      while ((!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&             \
             (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))))                  \
        _Block = LLDP_RQB_GET_NEXT_RQB_PTR(_Block);                        \
                                                                           \
      if (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) _Block = LSA_NULL; \
    }                                                                      \
  }

/*=============================================================================
 * function name:   LLDP_RQB_SEARCH_QUEUE_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      LLDP_UPPER_RQB_PTR_TYPE         pRQB
 *                  LSA_HANDLE_TYPE                Handle
 * return value:    LSA_VOID
 *
 * pRQB             points to start of RQB-queue (maybe NULL)
 * Handle           Handle to search in RQBs
 *
 * Description:     On return pRQB points to NULL if no entry was found, else
 *                  to the RQB with the spezified Handle. On entry pRQB may
 *                  already be NULL.
 *===========================================================================*/

#define LLDP_RQB_SEARCH_QUEUE_HANDLE(pRQB, Handle)      \
  {                                                     \
    while ((!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) && \
           (LLDP_RQB_GET_HANDLE(pRQB) != Handle)) {     \
      pRQB = LLDP_RQB_GET_NEXT_RQB_PTR(pRQB);           \
    }                                                   \
  }

/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  LLDP_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define LLDP_PUT_BLOCK_TOP(_Bottom, _Top, _Block)   \
  {                                                 \
    _Block->pNext = LSA_NULL;                       \
    _Block->pPrev = _Top;                           \
    if (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) { \
      _Top = _Block;                                \
      _Bottom = _Block;                             \
    } else {                                        \
      _Top->pNext = _Block;                         \
      _Top = _Block;                                \
    }                                               \
  }

/*=============================================================================
 * function name:  LLDP_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define LLDP_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)   \
  {                                                    \
    _Block->pNext = _Bottom;                           \
    _Block->pPrev = LSA_NULL;                          \
    if (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) { \
      _Bottom = _Block;                                \
      _Top = _Block;                                   \
    } else {                                           \
      _Bottom->pPrev = _Block;                         \
      _Bottom = _Block;                                \
    }                                                  \
  }

/*=============================================================================
 * function name:   LLDP_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_REM_BLOCK_TOP(_Bottom, _Top, _Block)    \
  {                                                  \
    _Block = _Top;                                   \
    if (!LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) { \
      _Top = _Top->pPrev;                            \
      if (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))  \
        _Bottom = LSA_NULL;                          \
      else                                           \
        _Top->pNext = LSA_NULL;                      \
    }                                                \
  }

/*=============================================================================
 * function name:   LLDP_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)    \
  {                                                     \
    _Block = _Bottom;                                   \
    if (!LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) { \
      _Bottom = _Bottom->pNext;                         \
      if (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))  \
        _Top = LSA_NULL;                                \
      else                                              \
        _Bottom->pPrev = LSA_NULL;                      \
    }                                                   \
  }

/*=============================================================================
 * function name:   LLDP_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_REM_BLOCK(_Bottom, _Top, _Handle, _Block)       \
  {                                                          \
    if ((LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) ||     \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) ||        \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)))       \
      _Block = LSA_NULL;                                     \
    else {                                                   \
      _Block = _Bottom;                                      \
      while ((!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Handle))) && \
             (!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Top))))      \
        _Block = _Block->pNext;                              \
      if (!LSA_HOST_PTR_ARE_EQUAL(_Block, (_Handle)))        \
        _Block = LSA_NULL;                                   \
      else {                                                 \
        if (LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom)) {     \
          LLDP_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);      \
        } else {                                             \
          if (LSA_HOST_PTR_ARE_EQUAL((_Block), _Top)) {      \
            LLDP_REM_BLOCK_TOP(_Bottom, _Top, _Block);       \
          } else {                                           \
            _Block->pPrev->pNext = _Block->pNext;            \
            _Block->pNext->pPrev = _Block->pPrev;            \
          }                                                  \
        }                                                    \
      }                                                      \
    }                                                        \
  }

/*=============================================================================
 * function name:   LLDP_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define LLDP_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)              \
  {                                                                        \
    if ((LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL)) ||                   \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL)) || ((_Handle) == 0))    \
      _Block = LSA_NULL;                                                   \
    else {                                                                 \
      _Block = _Bottom;                                                    \
      while ((!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&             \
             (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))))                  \
        _Block = (_Block)->pNext;                                          \
      if (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) _Block = LSA_NULL; \
    }                                                                      \
  }

#define LLDP_RQBSetResponse(pRQB, Response) \
  LLDP_RQB_SET_RESPONSE(pRQB, Response)

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* LLDP_glb.c */

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_FatalError(
    LLDP_FATAL_ERROR_TYPE Error, LSA_UINT16 ModuleID, LSA_UINT32 Line);

LSA_EXTERN LSA_VOID LLDP_RQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE LLDP_UPPER_RQB_ATTR *pLLError);

/* AP01539304 needs a typedef */
typedef LSA_VOID(LldpUpperCbf)(LLDP_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_CallCbf(
    LLDP_HDB_PTR_TYPE pHDB, LldpUpperCbf Cbf, LLDP_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_UserRequestFinish(
    LLDP_HDB_PTR_TYPE pHDB, LLDP_UPPER_RQB_PTR_TYPE pRQB, LLDP_RSP Status);

LSA_EXTERN LLDP_HDB_PTR_TYPE LLDP_LOCAL_FCT_ATTR
LLDP_GetUserHDBFromHandle(LSA_HANDLE_TYPE Handle);

/* LLDP_usr.c */

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserCloseChannelFinish(LLDP_HDB_PTR_TYPE pHDBUser);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserReleaseHDB(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserChannelStateMachine(LLDP_HDB_PTR_TYPE pHDB, LLDP_RSP Response);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserHandleTxDone(LLDP_HDB_PTR_TYPE pHDB, LLDP_EDD_PCTRL_PTR_TYPE pPCtrl);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserHandleLinkInd(LLDP_HDB_PTR_TYPE pHDB, LLDP_EDD_PCTRL_PTR_TYPE pPCtrl);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_UserHandleRxFrame(LLDP_HDB_PTR_TYPE pHDB, LLDP_EDD_PCTRL_PTR_TYPE pPCtrl,
                       LLDP_COMMON_MEM_U8_PTR_TYPE pFrame,
                       LSA_UINT32 DataOffset, LSA_UINT32 DataLen);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_RequestTimer(LLDP_UPPER_RQB_PTR_TYPE pRQB);

/* LLDP_edd.c */

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDCloseChannel(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDNrtCancel(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDOpenChannel(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDInitPortMgm(
    LLDP_HDB_PTR_TYPE pHDB, LSA_UINT32 PortCnt, LSA_UINT32 PortMaxSender);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_EDDFreePortMgm(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_EDDSendLLDPFrame(
    LLDP_HDB_PTR_TYPE pHDB, LLDP_EDD_PCTRL_PTR_TYPE pPCtrl, LSA_BOOL Shutdown);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDGetParams(LLDP_HDB_PTR_TYPE pHDB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDGetPortParams(LLDP_HDB_PTR_TYPE pHDB, LSA_UINT32 PortID);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDRecvProvide(LLDP_HDB_PTR_TYPE pHDB, LSA_UINT32 Cnt);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDLinkIndProvide(LLDP_HDB_PTR_TYPE pHDB, LSA_UINT32 PortCnt);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_EDDSetMulticast(LLDP_HDB_PTR_TYPE pHDB, LSA_UINT8 Mode);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_ConstructFrame(
    LLDP_COMMON_MEM_U8_PTR_TYPE pFrame, LLDP_COMMON_MEM_U8_PTR_TYPE pTLV,
    LSA_UINT32 TLVLength, LLDP_COMMON_MEM_U8_PTR_TYPE pSrcMAC);

LSA_EXTERN LSA_BOOL LLDP_LOCAL_FCT_ATTR
LLDP_CheckTLVEqual(LLDP_COMMON_MEM_U8_PTR_TYPE pFrame,
                   LLDP_COMMON_MEM_U8_PTR_TYPE pTLV, LSA_UINT32 TLVLength);

/* lldp_tim.c */

/* AP01539304 Compiler needs a typedef for the CallBackFunktion              */

typedef LSA_VOID(lldpTimerCbf)(LLDP_INTERNAL_REQUEST_PTR_TYPE pIntRQB);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_TimerAlloc(LSA_UINT16 TimerType, LSA_UINT16 TimerBase, LSA_UINT32 UserID,
                lldpTimerCbf Cbf, LLDP_LOCAL_MEM_U16_PTR_TYPE pLLDPTimerID);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TimerFree(LSA_UINT16 LLDPTimerID);
LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TimerMgmSetup(LSA_VOID);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TimerStop(LLDP_HDB_PTR_TYPE pHDB,
                                                       LSA_UINT16 LLDPTimerID);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TimerStart(LLDP_HDB_PTR_TYPE pHDB,
                                                        LSA_UINT16 LLDPTimerID,
                                                        LSA_UINT16 Time);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_TimerRQBFree(LSA_UINT16 LLDPTimerID);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR
LLDP_TimerRQBCheckFree(LSA_UINT16 LLDPTimerID);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_CylicTimerCB(LLDP_INTERNAL_REQUEST_PTR_TYPE pIntRQB);

/* lldp_tlv.c */

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVValidateChain(
    LLDP_COMMON_MEM_U8_PTR_TYPE pTLV, LSA_UINT32 TLength,
    LLDP_COMMON_MEM_U32_PTR_TYPE pTTL, LLDP_COMMON_MEM_U32_PTR_TYPE pNLength,
    LLDP_COMMON_MEM_BOOL_PTR_TYPE pEndTLV);

LSA_EXTERN LSA_VOID LLDP_LOCAL_FCT_ATTR
LLDP_TLVSetCommon(LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *ppTLV,
                  LLDP_COMMON_MEM_U32_PTR_TYPE pTLength, LSA_UINT32 Type,
                  LSA_UINT32 Length, LLDP_COMMON_MEM_PTR_TYPE pValue);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVGetCommon(
    LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *ppTLV,
    LLDP_COMMON_MEM_U32_PTR_TYPE pTLength, LLDP_COMMON_MEM_U32_PTR_TYPE pType,
    LLDP_COMMON_MEM_U32_PTR_TYPE pLength,
    LLDP_COMMON_MEM_PTR_TYPE LLDP_COMMON_MEM_ATTR *ppValue);

/*===========================================================================*/
/*                                   macros                                  */
/*===========================================================================*/

#define LLDP_FATAL(Error) \
  { LLDP_FatalError(Error, LLDP_MODULE_ID, __LINE__); }

#define LLDP_ALLOC_FAIL_FATAL(pPtr)               \
  {                                               \
    if (LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL)) { \
      LLDP_FATAL(LLDP_FATAL_ERR_NULL_PTR);        \
    }                                             \
  }

#define LLDP_MEM_FREE_FAIL_CHECK(RetVal)   \
  {                                        \
    if (RetVal != LSA_RET_OK) {            \
      LLDP_FATAL(LLDP_FATAL_ERR_MEM_FREE); \
    }                                      \
  }

/*===========================================================================*/
/*                 additional assert macros                                  */
/*===========================================================================*/

#ifdef LLDP_CFG_DEBUG_ASSERT
#define LLDP_ASSERT_NULL_PTR(pPtr)                                        \
  {                                                                       \
    if (LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL)) {                         \
      LLDP_FatalError(LLDP_FATAL_ERR_NULL_PTR, LLDP_MODULE_ID, __LINE__); \
    }                                                                     \
  }
#else
#define LLDP_ASSERT_NULL_PTR(pPtr) \
  {}
#endif

#ifdef LLDP_CFG_DEBUG_ASSERT
#define LLDP_ASSERT_FALSE(Cond_)                                              \
  {                                                                           \
    if (!(Cond_)) {                                                           \
      LLDP_FatalError(LLDP_FATAL_ERR_INCONSISTENZ, LLDP_MODULE_ID, __LINE__); \
    }                                                                         \
  }
#else
#define LLDP_ASSERT_FALSE(Condition) \
  {}
#endif

#ifdef LLDP_CFG_DEBUG_ASSERT
#define LLDP_ASSERT(Cond_)                                                    \
  {                                                                           \
    if ((Cond_)) {                                                            \
      LLDP_FatalError(LLDP_FATAL_ERR_INCONSISTENZ, LLDP_MODULE_ID, __LINE__); \
    }                                                                         \
  }
#else
#define LLDP_ASSERT(Condition) \
  {}
#endif

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined LLDP_UPPER_IN_FCT_ATTR
#error "LLDP_UPPER_IN_FCT_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_UPPER_OUT_FCT_ATTR
#error "LLDP_UPPER_OUT_FCT_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_SYSTEM_IN_FCT_ATTR
#error "LLDP_SYSTEM_IN_FCT_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_SYSTEM_OUT_FCT_ATTR
#error "LLDP_SYSTEM_OUT_FCT_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_LOCAL_FCT_ATTR
#error "LLDP_LOCAL_FCT_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_UPPER_RQB_ATTR
#error "LLDP_UPPER_RQB_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_UPPER_MEM_ATTR
#error "LLDP_UPPER_MEM_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_SYSTEM_MEM_ATTR
#error "LLDP_SYSTEM_MEM_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LLDP_LOCAL_MEM_ATTR
#error "LLDP_LOCAL_MEM_ATTR not defined in file lldp_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

/*===========================================================================*/
/*              removed CFG defines                                          */
/*===========================================================================*/

#if defined LLDP_CFG_FREE_ERROR_IS_FATAL
#error "LLDP_CFG_FREE_ERROR_IS_FATAL not used anymore (always on) "
#endif
#if defined LLDP_CFG_DONT_USE_LINK_STATUS
#error "LLDP_CFG_DONT_USE_LINK_STATUS not used anymore"
#endif
#if defined LLDP_CFG_USE_STOP_DELAY
#error "LLDP_CFG_USE_STOP_DELAY not used anymore"
#endif
#if defined LLDP_CFG_TX_REINIT_DELAY
#error "LLDP_CFG_TX_REINIT_DELAY not used anymore"
#endif
#if defined LLDP_CFG_TX_DELAY
#error "LLDP_CFG_TX_DELAY not used anymore"
#endif
#if defined LLDP_CFG_NRT_SEND_PRIO
#error "LLDP_CFG_NRT_SEND_PRIO not used anymore"
#endif
#ifdef LLDP_CFG_MAX_PORT_SENDER
#error "LLDP_CFG_MAX_PORT_SENDER must not be defined any more"
#endif
/*****************************************************************************/
/*  end of file LLDP_INT.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif /* of LLDP_INT_H */
