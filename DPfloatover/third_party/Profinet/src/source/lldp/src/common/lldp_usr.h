
#ifndef LLDP_USR_H                       /* ----- reinclude-protection ----- */
#define LLDP_USR_H

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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_usr.h                                :F&  */
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
/*  Defines constants, types, macros and prototyping for LLDP                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  05.04.04    JS    initial version.                                       */
/*  22.09.04    JS    added LLDP_TLV_LLDPDU_END_SIZE                         */
/*  20.10.04    JS    added defines for optional TLVs                        */
/*  22.10.04    JS    added defines/declarations for new functions           */
/*  15.11.04    JS    added LLDP_RX_FRAME_BUFFER_LENGTH                      */
/*  22.06.05    JS    added "_" LLDP_TLV_GET_EX_PARAMS_TYPE                  */
/*  17.10.05    JS    added UNKNOWN_OPTIONAL types/defines                   */
/*  12.10.06    JS    changed LSA_RET_xxx codes to LSA_RSP_xxx codes         */
/*  14.11.06    JS    using LSA_RESPONSE_TYPE                                */
/*  01.10.07    JS    added LLDP_EVENT_LOOPBACK                              */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/

#define LLDP_OPC_OPEN_CHANNEL                (LSA_OPCODE_TYPE)   0x01
#define LLDP_OPC_CLOSE_CHANNEL               (LSA_OPCODE_TYPE)   0x02
#define LLDP_OPC_GET_CONFIG                  (LSA_OPCODE_TYPE)   0x03
#define LLDP_OPC_SET                         (LSA_OPCODE_TYPE)   0x04
#define LLDP_OPC_START                       (LSA_OPCODE_TYPE)   0x05
#define LLDP_OPC_STOP                        (LSA_OPCODE_TYPE)   0x06
#define LLDP_OPC_INDICATION_PROVIDE          (LSA_OPCODE_TYPE)   0x07
#define LLDP_OPC_GET                         (LSA_OPCODE_TYPE)   0x08
#define LLDP_OPC_STATS                       (LSA_OPCODE_TYPE)   0x09
#define LLDP_OPC_TIMER                       (LSA_OPCODE_TYPE)   0x0A
#define LLDP_OPC_SET_GLOBALS                 (LSA_OPCODE_TYPE)   0x0B

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_RSP  LSA_RESPONSE_TYPE  /* dont change */

/* LSA OK-Codes */
#define LLDP_RSP_OK                          (LLDP_RSP)    LSA_RSP_OK
/* LSA LLDP-Codes */
#define LLDP_RSP_ERR_RESOURCE                (LLDP_RSP)    LSA_RSP_ERR_RESOURCE
#define LLDP_RSP_ERR_PARAM                   (LLDP_RSP)    LSA_RSP_ERR_PARAM
#define LLDP_RSP_ERR_SEQUENCE                (LLDP_RSP)    LSA_RSP_ERR_SEQUENCE
#define LLDP_RSP_ERR_SYS_PATH                (LLDP_RSP)    LSA_RSP_ERR_SYS_PATH

/* NARE OK-Code (MaxOffset = 0x3F) */
/* LSA_RSP_OK_OFFSET = 0x40       */

#define LLDP_RSP_OK_CANCEL                   (LLDP_RSP)    (LSA_RSP_OK_OFFSET + 0x01)

/* NARE ERR-Codes (MaxOffset = 0x3E)*/
/* LSA_RSP_ERR_OFFSET = 0xC0       */

#define LLDP_RSP_ERR_OPCODE                  (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x01)
#define LLDP_RSP_ERR_CHANNEL_USE             (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x02)
#define LLDP_RSP_ERR_LL                      (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x03)
#define LLDP_RSP_ERR_TLV_MISMATCH            (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x04)
#define LLDP_RSP_ERR_TLV_UNKNOWN             (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x05)
#define LLDP_RSP_ERR_TLV_PARAM_ERROR         (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x06)
#define LLDP_RSP_ERR_NO_TIMER                (LLDP_RSP)    (LSA_RSP_ERR_OFFSET + 0x07)

/*===========================================================================*/
/*                              basic types for LLDP                         */
/*===========================================================================*/

#define LLDP_UPPER_MEM_PTR_TYPE               /* pointer to upper-memory */ \
LSA_VOID                    LLDP_UPPER_MEM_ATTR *

#define LLDP_UPPER_MEM_U8_PTR_TYPE            /* pointer to upper-memory */ \
LSA_UINT8                   LLDP_UPPER_MEM_ATTR *

#define LLDP_UPPER_MEM_U16_PTR_TYPE           /* pointer to upper-memory */ \
LSA_UINT16                  LLDP_UPPER_MEM_ATTR *

#define LLDP_UPPER_MEM_U32_PTR_TYPE          /* pointer to upper-memory */ \
LSA_UINT32                  LLDP_UPPER_MEM_ATTR *

#define LLDP_UPPER_MEM_U64_PTR_TYPE              /* pointer to device-memory */ \
LSA_UINT64                  LLDP_UPPER_MEM_ATTR *


#define LLDP_EDD_LOWER_MEM_PTR_TYPE             /* pointer to lower-memory */ \
LSA_VOID                    LLDP_EDD_LOWER_MEM_ATTR *


#define LLDP_EDD_LOWER_MEM_U8_PTR_TYPE          /* pointer to lower-memory */ \
LSA_UINT8                   LLDP_EDD_LOWER_MEM_ATTR *


#define LLDP_LOCAL_MEM_PTR_TYPE                /* pointer to local-memory */ \
LSA_VOID                    LLDP_LOCAL_MEM_ATTR *

#define LLDP_LOCAL_MEM_U8_PTR_TYPE             /* pointer to local-memory */ \
LSA_UINT8                   LLDP_LOCAL_MEM_ATTR *

#define LLDP_LOCAL_MEM_U16_PTR_TYPE            /* pointer to local-memory */ \
LSA_UINT16                  LLDP_LOCAL_MEM_ATTR *

#define LLDP_LOCAL_MEM_U32_PTR_TYPE            /* pointer to local-memory */ \
LSA_UINT32                  LLDP_LOCAL_MEM_ATTR *

#define LLDP_LOCAL_MEM_U8_CONST_PTR_TYPE       /* pointer to local-memory */ \
const LSA_UINT8             LLDP_LOCAL_MEM_ATTR *

#define LLDP_LOCAL_MEM_BOOL_PTR_TYPE            /* pointer to local-memory */ \
LSA_BOOL                    LLDP_COMMON_MEM_ATTR *


#define LLDP_UPPER_RQB_MEM_PTR_TYPE            /* pointer to upper RQB-mem*/ \
LSA_VOID                    LLDP_UPPER_RQB_ATTR *


#define LLDP_COMMON_MEM_PTR_TYPE               /* pointer to common-memory */ \
LSA_VOID                    LLDP_COMMON_MEM_ATTR *

#define LLDP_COMMON_MEM_U8_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT8                   LLDP_COMMON_MEM_ATTR *

#define LLDP_COMMON_MEM_U16_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT16                  LLDP_COMMON_MEM_ATTR *

#define LLDP_COMMON_MEM_U32_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT32                  LLDP_COMMON_MEM_ATTR *

#define LLDP_COMMON_MEM_BOOL_PTR_TYPE            /* pointer to common-memory */ \
LSA_BOOL                    LLDP_COMMON_MEM_ATTR *


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct _LLDP_RQB_TYPE     LLDP_UPPER_RQB_ATTR *    LLDP_UPPER_RQB_PTR_TYPE;

#define LLDP_IP_ADDR_SIZE   4
#define LLDP_MAC_ADDR_SIZE  6

typedef LSA_UINT8 LLDP_IP_TYPE[LLDP_IP_ADDR_SIZE];
typedef LSA_UINT8 LLDP_MAC_TYPE[LLDP_MAC_ADDR_SIZE];

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Number of max. LLDP-neighbours per Port. This defines the number of       */
/* neighbours per port, replaces old the LLDP-Detail "PORTMaxSender" !!!     */
/*---------------------------------------------------------------------------*/

#define LLDP_MAX_PORT_SENDER 4


#define LLDP_RX_FRAME_BUFFER_LENGTH     EDD_FRAME_BUFFER_LENGTH   /* RX-Framebuffer size         */

#define LLDP_MAX_TLV_DATA_SIZE              1500   /* Max TLV-Chain length.      */
#define LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END  (LLDP_MAX_TLV_DATA_SIZE - (LLDP_TL_SIZE + LLDP_TLV_LLDPDU_END_SIZE))

/*===========================================================================*/
/*                       Args for RQB (Request Block)                        */
/*===========================================================================*/

#define LLDP_PORT_ID_ALL  0xFFFF

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_OPEN_CHANNEL                                             */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_OPEN_CHANNEL_TYPE
{
    LSA_HANDLE_TYPE                 handle;
    /* req: -                                */
    /* cnf: channel-handle of LLDP           */
    /* At all later RQBs user will write it  */
    /* into handle of LLDP_RQB_HEADER        */
    LSA_HANDLE_TYPE                 handle_upper;
    /* req: channel-handle of user           */
    /* cnf: -                                */
    /* At all confirmations LLDP writes it   */
    /* into handle of LLDP_RQB_HEADER.       */
    LSA_SYS_PATH_TYPE               sys_path;
    /* req: system-path of channel           */
    /* cnf: -                                */
    LSA_VOID                        LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR,
                                                lldp_request_upper_done_ptr)
    (LLDP_UPPER_RQB_PTR_TYPE upper_rqb_ptr);
    /* req: pointer to callback-function     */
    /* cnf: -                                */
} LLDP_RQB_OPEN_CHANNEL_TYPE;

typedef struct _LLDP_RQB_OPEN_CHANNEL_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_OPEN_CHANNEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_CLOSE_CHANNEL                                            */
/*---------------------------------------------------------------------------*/

/* no args */


/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_GET_CONFIG                                               */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_GET_CONFIG_TYPE
{
    LSA_UINT16    PORTCnt;                                  /* out */
    LSA_UINT16    PORTMaxSender;                            /* out */
} LLDP_RQB_GET_CONFIG_TYPE;

typedef struct _LLDP_RQB_GET_CONFIG_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_GET_CONFIG_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Defaults for LLDP                                                         */
/*---------------------------------------------------------------------------*/

#define LLDP_DEFAULT_TX_REINIT_DELAY 	0
#define LLDP_DEFAULT_TX_DELAY		0
#define LLDP_DEFAULT_MSG_DELAY		5

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_SET                                                      */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_SET_TYPE
{
    LSA_UINT16                  PORTId;                     /* in  */
/*    LSA_UINT32                  MsgTxInterval;  will be removed  */
    LLDP_UPPER_MEM_PTR_TYPE     pTLV;                       /* in  */
    LSA_UINT32                  TLVLength;                  /* in  */
} LLDP_RQB_SET_TYPE;

typedef struct _LLDP_RQB_SET_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_SET_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_START                                                    */
/*---------------------------------------------------------------------------*/
/* defines for the start and stop modes                                      */

#define LLDP_RX_MODE    1
#define LLDP_TX_MODE    2
#define LLDP_RXTX_MODE  3

typedef struct _LLDP_RQB_START_TYPE
{
    LSA_UINT16    PORTId;                                   /* in  */
    LSA_UINT8     Mode;
} LLDP_RQB_START_TYPE;

typedef struct _LLDP_RQB_START_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_START_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_STOP                                                     */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_STOP_TYPE
{
    LSA_UINT16    PORTId;                                   /* in  */
    LSA_UINT8     Mode;
} LLDP_RQB_STOP_TYPE;

typedef struct _LLDP_RQB_STOP_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_STOP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_INDICATION_PROVIDE                                       */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_UPPER_TLV_TYPE
{
    LSA_UINT8    Data[LLDP_MAX_TLV_DATA_SIZE];
} LLDP_UPPER_TLV_TYPE;

typedef struct _LLDP_UPPER_TLV_TYPE LLDP_UPPER_MEM_ATTR * LLDP_UPPER_TLV_PTR_TYPE;

typedef struct _LLDP_RQB_INDICATION_PROVIDE_TYPE
{
    LSA_UINT16                PORTId;                       /* out */
    LSA_UINT16                SenderId;                     /* out */
    LSA_UINT16                Event;                        /* out */
    LLDP_MAC_TYPE             MACAddr;                      /* out */
    LLDP_UPPER_TLV_PTR_TYPE   pTLV;                         /* in/out */
    LSA_UINT32                TLVLength;                    /* out */
} LLDP_RQB_INDICATION_PROVIDE_TYPE;

typedef struct _LLDP_RQB_INDICATION_PROVIDE_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_INDICATION_PROVIDE_PTR_TYPE;

/* Event: */

#define LLDP_EVENT_DATA_VALID       0  /* new or changed data                  */
#define LLDP_EVENT_DATA_INVALID     1  /* data invalid because of TTL exceeded */
#define LLDP_EVENT_SHUTDOWN         2  /* shutdown received. data invalid      */
#define LLDP_EVENT_TOO_MANY_SENDER  3  /* too many sender.                     */
//#define LLDP_EVENT_LOOPBACK         4  /* loopback on port detected            */

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_GET                                                      */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_GET_TYPE
{
    LSA_UINT16                 PORTId;                      /* in  */
    LSA_UINT16                 SenderId;                    /* in  */
    LSA_BOOL                   Present;                     /* out */
    LLDP_MAC_TYPE              MACAddr;                     /* out */
    LLDP_UPPER_TLV_PTR_TYPE    pTLV;                        /* in/out */
    LSA_UINT32                 TLVLength;                   /* out */
} LLDP_RQB_GET_TYPE;

typedef struct _LLDP_RQB_GET_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_GET_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_STATS                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_UPPER_STATS_TYPE
{
    LSA_UINT32    RxInvalidDstMAC;       /* LLDP-Frame with invalid DST-MAC     */
                                         /* (not the LLDP MC address) -> no LLDP*/
    LSA_UINT32    RxFramesInTotal;       /* LLDP frames received total        1)*/
    LSA_UINT32    RxFramesDiscardedTotal;/* LLDP frames discarded total       1)*/
    LSA_UINT32    RxFramesInErrors;      /* in frames with tlv-error          1)*/
    LSA_UINT32    RxAgeoutsTotal;        /* number of times TTL expired.      1)*/
    LSA_UINT32    RxTooManyTotal;        /* number of too-many sender occured   */
    LSA_UINT32    TxFramesOutTotal;      /* frames send total                 1)*/
    LSA_UINT32    TxShutdownFailed;      /* sending shutdown frame failed       */
    LSA_UINT32    TxDataFailed;          /* sending data-frame failed           */
    LSA_UINT32    TxNoMemResources;      /* sending LLDP frame failed because   */
                                         /* no memory resources.                */
    LSA_UINT32    TxStillUnderway;       /* can not send data-frame, because    */
                                         /* last one still in progress.         */
                                         /* 1) statistics spezified in LLDP-Spec*/
} LLDP_UPPER_STATS_TYPE;

typedef struct _LLDP_RQB_STATS_TYPE
{
    LSA_UINT16              PORTId;                         /* in  */
    LSA_BOOL                Reset;                          /* in  */
    LLDP_UPPER_STATS_TYPE   Cntr;                           /* out */
} LLDP_RQB_STATS_TYPE;

typedef struct _LLDP_RQB_STATS_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_STATS_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_TIMER  (for LLDP internal use only!)                     */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_RQB_TIMER_TYPE
{
    LSA_UINT16              TimerID; /* LLDP-TimerID */
    LSA_UINT32              UserID;
} LLDP_RQB_TIMER_TYPE;

typedef struct _LLDP_RQB_TIMER_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_TIMER_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: LLDP_OPC_SET_GLOBALS                                              */
/*---------------------------------------------------------------------------*/
typedef struct _LLDP_RQB_SET_GLOBALS_TYPE
{
    LSA_UINT32				TxDelay;
    LSA_UINT32				ReInitDelay;
    LSA_UINT32				MsgTxInterval;
} LLDP_RQB_SET_GLOBALS_TYPE;

typedef struct _LLDP_RQB_SET_GLOBALS_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_RQB_SET_GLOBALS_PTR_TYPE;

#define LLDP_TIMER_USER_ID_CYLIC    0   /* TTL-Check and Send base timer */

/*---------------------------------------------------------------------------*/
/*                           RQB- Header for Services                        */
/*---------------------------------------------------------------------------*/

typedef union _LLDP_RQB_ARGS_TYPE
{
    LLDP_RQB_OPEN_CHANNEL_TYPE           channel;       /* open channel             */
    LLDP_RQB_GET_CONFIG_TYPE             GetConfig;     /* get config               */
    LLDP_RQB_SET_TYPE                    Set;           /* set                      */
    LLDP_RQB_START_TYPE                  Start;         /* start                    */
    LLDP_RQB_STOP_TYPE                   Stop;          /* stop                     */
    LLDP_RQB_INDICATION_PROVIDE_TYPE     IndProvide;    /* indication provide       */
    LLDP_RQB_GET_TYPE                    Get;           /* get                      */
    LLDP_RQB_STATS_TYPE                  Stats;         /* stats                    */
    LLDP_RQB_SET_GLOBALS_TYPE            SetGlobals;    /* set global params        */
    LLDP_RQB_TIMER_TYPE                  Timer;         /* TimerEvent               */
} LLDP_RQB_ARGS_TYPE;

#if 0
/* removed due to AP01205450 */
/* usage of LSA_RQB_LOCAL_ERROR_TYPE             */
/* - lsa_component_id:                           */
/*    LSA_COMP_ID_UNUSED : no error-code present */
/*    LSA_COMP_ID_LLDP   : errorcode from LLDP   */
/*    LSA_COMP_ID_EDD    : errorcode from EDD    */
/* - error_code_id not used. always 0            */
/* - error_code[12] used for following format:   */
/*   if LSA_COMP_ID_LLDP:                        */

typedef struct LLDP_LOCAL_ERROR_CODE_
{
    LSA_OPCODE_TYPE opcode;
    LSA_UINT16      response;
} LLDP_LOCAL_ERROR_CODE_TYPE;

typedef struct LLDP_LOCAL_ERROR_CODE_     LLDP_UPPER_RQB_ATTR *    LLDP_LOCAL_ERROR_CODE_PTR_TYPE;
#endif
typedef struct _LLDP_RQB_TYPE
{
    LLDP_RQB_HEADER
    LLDP_RQB_ARGS_TYPE            args;       /* IN/OUT: Args of Request     */
    LLDP_RQB_TRAILER
} LLDP_RQB_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*                                LLPDU/TLVs                                 */
/*===========================================================================*/

#define LLDP_MAX_TLV_DATA_SIZE      1500  /* max length of all TLVs */

/*---------------------------------------------------------------------------*/
/* TLV-Format:                                                               */
/*                                                                           */
/*     7 Bits    9 Bits    0..511 octets                                     */
/*  +---------+---------+----------------+                                   */
/*  |  Type   |  Length |  Value         |                                   */
/*  +---------+---------+----------------+                                   */
/*                                                                           */
/*  | <--- 2 octets --> |                                                    */
/*                                                                           */
/*  Length: Length of Value-Field                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_TL_SIZE               2      /* Size of TL-field in octets      */
#define LLDP_LEN_MAX_SIZE          511    /* Maximum Length in octets        */

#define LLDP_TLV_TYPE_MSK          0xFE
#define LLDP_TLV_TYPE_SHIFT        1

#define LLDP_TLV_LENGTH_HIGH_MSK   0x01
#define LLDP_TLV_LENGTH_LOW_MSK    0xFF


/* TLV-Types */

#define LLDP_TLV_TYPE_LLDPDU_END          0x00
#define LLDP_TLV_TYPE_CHASSIS_ID          0x01
#define LLDP_TLV_TYPE_PORT_ID             0x02
#define LLDP_TLV_TYPE_TTL                 0x03
#define LLDP_TLV_TYPE_PORT_DESCR          0x04
#define LLDP_TLV_TYPE_SYSTEM_NAME         0x05
#define LLDP_TLV_TYPE_SYSTEM_DESCR        0x06
#define LLDP_TLV_TYPE_SYSTEM_CAP          0x07
#define LLDP_TLV_TYPE_MGM_ADDR            0x08
#define LLDP_TLV_TYPE_ORG_SPECIFIC        0x7F

/*===========================================================================*/
/* Mandatory TLVs                                                            */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_CHASSIS_ID                                                  */
/*                                                                           */
/*     1 octec      1..255 octets                                            */
/*  +---------+--------------------------+                                   */
/*  | Subtype |  chassis id string       |                                   */
/*  +---------+--------------------------+                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE       1
#define LLDP_MIN_CHASSIS_ID_STRING_SIZE        1
#define LLDP_MAX_CHASSIS_ID_STRING_SIZE        255

#define LLDP_TLV_CHASSIS_ID_MIN_SIZE           (LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE+1)

/* subtypes */

#define LLDP_TLV_CHA_SUB_CHASSIS_COMP                1
#define LLDP_TLV_CHA_SUB_CHASSIS_INTERFACE_ALIAS     2
#define LLDP_TLV_CHA_SUB_PORT                        3
#define LLDP_TLV_CHA_SUB_MAC_ADDR                    4
#define LLDP_TLV_CHA_SUB_NETWORK_ADDR                5
#define LLDP_TLV_CHA_SUB_INTERFACE_NAME              6
#define LLDP_TLV_CHA_SUB_LOCALLY_ASSIGNED            7

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_PORT_ID                                                     */
/*                                                                           */
/*     1 octec      1..255 octets                                            */
/*  +---------+--------------------------+                                   */
/*  | Subtype |  port id string          |                                   */
/*  +---------+--------------------------+                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_PORT_ID_SUBTYPE_SIZE          1
#define LLDP_MIN_PORT_ID_STRING_SIZE           1
#define LLDP_MAX_PORT_ID_STRING_SIZE           255

#define LLDP_TLV_PORT_ID_MIN_SIZE              (LLDP_TLV_PORT_ID_SUBTYPE_SIZE+1)

/* subtypes */
#define LLDP_TLV_PORT_SUB_INTERFACE_ALIAS      1
#define LLDP_TLV_PORT_SUB_PORT_COMP            2
#define LLDP_TLV_PORT_SUB_MAC_ADDR             3
#define LLDP_TLV_PORT_SUB_NETWORK_ADDR         4
#define LLDP_TLV_PORT_SUB_INTERFACE_NAME       5
#define LLDP_TLV_PORT_SUB_AGENT_CIRCUIT_ID     6
#define LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED     7

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_TTL                                                         */
/*                                                                           */
/*     2 octec                                                               */
/*  +------------+                                                           */
/*  | TimeToLive |                                                           */
/*  +------------+                                                           */
/*                                                                           */
/*   TimeToLive in Seconds (0..0xFFFF)                                       */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_TTL_SIZE 2
#define LLDP_TLV_TTL_MAX    0xFFFF

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_LLDPDU_END                                                  */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_LLDPDU_END_SIZE 0  /* size of value-field */

/*===========================================================================*/
/* Optional TLVs                                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_PORT_DESCR                                                  */
/*                                                                           */
/*             0..255 octets                                                 */
/*  +------------------------------------+                                   */
/*  | port description                   |                                   */
/*  +------------------------------------+                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_PORT_DESCR_MIN_SIZE        0
#define LLDP_TLV_PORT_DESCR_MAX_SIZE        255

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_SYSTEM_NAME                                                 */
/*                                                                           */
/*             0..255 octets                                                 */
/*  +------------------------------------+                                   */
/*  | system name                        |                                   */
/*  +------------------------------------+                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_SYSTEM_NAME_MIN_SIZE       0
#define LLDP_TLV_SYSTEM_NAME_MAX_SIZE       255


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_SYSTEM_DESCR                                                */
/*                                                                           */
/*             0..255 octets                                                 */
/*  +------------------------------------+                                   */
/*  | system description                 |                                   */
/*  +------------------------------------+                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_SYSTEM_DESCR_MIN_SIZE      0
#define LLDP_TLV_SYSTEM_DESCR_MAX_SIZE      255


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_SYSTEM_CAP                                                  */
/*                                                                           */
/*      2 octets         2 octets                                            */
/*  +--------------+-----------------+                                       */
/*  | system       |  enabled        |                                       */
/*  | capabilities.|  capabilities   |                                       */
/*  +--------------+-----------------+                                       */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_SYSTEM_CAP_SYSTEM_CAP_SIZE        2
#define LLDP_TLV_SYSTEM_CAP_ENABLED_CAB_SIZE       2

#define LLDP_TLV_SYSTEM_CAP_SIZE                   4

/* System  capabilities (Bitmap) */
/* Enabled capabilities (Bitmap) */

#define LLDP_TLV_SYSCAB_OTHER                    0x0001
#define LLDP_TLV_SYSCAB_REPEATER                 0x0002
#define LLDP_TLV_SYSCAB_BRIDGE                   0x0004
#define LLDP_TLV_SYSCAB_WLAN_ACCESS_POINT        0x0008
#define LLDP_TLV_SYSCAB_ROUTER                   0x0010
#define LLDP_TLV_SYSCAB_TELEPHONE                0x0020
#define LLDP_TLV_SYSCAB_DOCSIS_CABLE_DEVICE      0x0040
#define LLDP_TLV_SYSCAB_STATION_ONLY             0x0080

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_MGM_ADDR                                                    */
/*                                                                           */
/*    1 octet  1 octet  1-31 octets  1 octet   4 octet  1 octet 0-128 octets */
/*  +---------+-------+------------+---------+---------+------+------------+ */
/*  |mgmt addr|mgmt   | management |interface|interface|OID   | OID        | */
/*  |string   |addr   |  address   |numbering|  number |string| identifier | */
/*  |length   |subtype|            | subtype |         |length|            | */
/*  +---------+-------+------------+---------+---------+------+------------+ */
/*            |                    |                   |                   | */
/*            |< mgmt addr string >|                   |< Object ID string>| */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE    1
#define LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE          1
#define LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN        2
#define LLDP_TLV_MGM_ADDR_MAX_MGMT_ADDR_STRING_LEN       32
#define LLDP_TLV_MGM_ADDR_IFNUM_SUBTYPE_SIZE              1
#define LLDP_TLV_MGM_ADDR_IFNUM_SIZE                      4
#define LLDP_TLV_MGM_ADDR_OI_STRING_LENGTH_SIZE           1
#define LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN              0
#define LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN            128

#define LLDP_TLV_MGM_ADDR_MIN_SIZE       9
#define LLDP_TLV_MGM_ADDR_MAX_SIZE       167

/* mgmt addr subtype : see ADDRESS FAMILY NUMBERS on www.iana.org */

/* interface numbering subtype */

#define LLDP_TLV_MGMADDR_IFNUM_UNKNOWN               1
#define LLDP_TLV_MGMADDR_IFNUM_IFINDEX               2
#define LLDP_TLV_MGMADDR_IFNUM_SYSTEM_PORT_NUMBER    3


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC                                                */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | Group    |  Group defined    |                               */
/*  |         | defined  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_ORG_SPECIFIC_OUI_SIZE                        3
#define LLDP_TLV_ORG_SPECIFIC_GROUP_DEFINED_TLV_TYPE_SIZE     1
#define LLDP_TLV_ORG_SPECIFIC_MIN_GROUP_DEFINED_ISTRING_SIZE  0
#define LLDP_TLV_ORG_SPECIFIC_MAX_GROUP_DEFINED_ISTRING_SIZE  507

#define LLDP_TLV_ORG_SPECIFIC_MIN_SIZE    4
#define LLDP_TLV_ORG_SPECIFIC_MAX_SIZE    511

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | Group    |  Group defined    |                               */
/*  | 00-80-C2| defined  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* IEEE 802.1 */
#define LLDP_TLV_ORG_OUI_IEEE_802_1_1    0x00
#define LLDP_TLV_ORG_OUI_IEEE_802_1_2    0x80
#define LLDP_TLV_ORG_OUI_IEEE_802_1_3    0xC2

#define LLDP_TLV_ORG_OUI_IEEE_802_1 \
( (LSA_UINT32) ( ((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_1_1 << 16) + \
((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_1_2 << 8 ) + \
((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_1_3      ) ))


/* Group defined TLV type */
#define LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID            1
#define LLDP_TLV_IEEE_802_1_TYPE_PPVID                   2
#define LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME               3
#define LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY       4


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID                                    */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*      2 octets                                                             */
/*  +-------------+                                                          */
/*  |  Port       |                                                          */
/*  |  VLAN       |                                                          */
/*  |  identifier |                                                          */
/*  +-------------+                                                          */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_1_PORT_VLAN_IDENTIFIER_SIZE     2

#define LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE   (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +2)

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_1_TYPE_PPVID                                           */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*    1 octet   2 octets                                                     */
/*  +---------+----------+                                                   */
/*  |  Flags  | PPVID    |                                                   */
/*  |         | reference|                                                   */
/*  |         | number   |                                                   */
/*  +---------+----------|                                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_1_PPVID_FLAGS_SIZE                 1
#define LLDP_TLV_IEEE_802_1_PPVID_REFERENCE_NUMBER_SIZE      2

#define LLDP_TLV_IEEE_802_1_PPVID_SIZE         (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +3)

/* Flags (Bitmap): */

#define LLDP_TLV_PPVID_CAPABLE          0x01
#define LLDP_TLV_PPVID_NOT_CAPABLE      0x00
#define LLDP_TLV_PPVID_ENABLED          0x02
#define LLDP_TLV_PPVID_NOT_ENABLED      0x00


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME                                       */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*    2 octets  1 octet    1-32 octets                                       */
/*  +---------+----------+------------+                                      */
/*  | VLAN ID | VLAN     | VLAN       |                                      */
/*  |         | name     | name       |                                      */
/*  |         | length   |            |                                      */
/*  +---------+----------+------------+                                      */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE         2
#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_NAME_LEN_SIZE   1

#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN    1
#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN   32


#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_SIZE       (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +4)
#define LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_SIZE       (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +35)


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY                               */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*    1 octet      <= 255                                                    */
/*  +---------+-----------------------+                                      */
/*  | protocol| protocol              |                                      */
/*  | identity| identity              |                                      */
/*  | length  |                       |                                      */
/*  +---------+-----------------------+                                      */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_LENGTH_SIZE    1

#define LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN        0
#define LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN        255

#define LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_SIZE       (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +1)
#define LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_SIZE       (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +256)



/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.3                                   */
/*                                                                           */
/*    3 octet   1 octet       0..507 octets                                  */
/*  +---------+----------+-------------------+                               */
/*  |   OUI   | Group    |  Group defined    |                               */
/*  | 00-12-0F| defined  |  information      |                               */
/*  |         | TLV type |  string           |                               */
/*  +---------+----------|-------------------+                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* IEEE 802.3 */
#define LLDP_TLV_ORG_OUI_IEEE_802_3_1    0x00
#define LLDP_TLV_ORG_OUI_IEEE_802_3_2    0x12
#define LLDP_TLV_ORG_OUI_IEEE_802_3_3    0x0F

#define LLDP_TLV_ORG_OUI_IEEE_802_3 \
( (LSA_UINT32) ( ((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_3_1 << 16) + \
((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_3_2 << 8 ) + \
((LSA_UINT32)LLDP_TLV_ORG_OUI_IEEE_802_3_3      ) ))

/* Group defined TLV type */
#define LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS      1
#define LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI           2
#define LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION        3
#define LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE          4


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS                              */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*         1 octet          2 octets           2 octets                      */
/*  +-----------------+--------------------+-------------+                   */
/*  | autonegotiation | PMD autonegotiation| Operational |                   */
/*  | support/status  | advertised         | MAU type    |                   */
/*  |                 | capability         |             |                   */
/*  +-----------------+--------------------+-------------+                   */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_SS_SIZE     1
#define LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_ANEG_AC_SIZE     2
#define LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_MAUT_TYPE_SIZE   2

#define LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE   (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +5)


/* Autoneg support status (Bitmap) */

#define LLDP_TLV_MAC_PHY_AUTONEG_SUPPORTED          0x01
#define LLDP_TLV_MAC_PHY_AUTONEG_NOT_SUPPORTED      0x00
#define LLDP_TLV_MAC_PHY_AUTONEG_ENABLED            0x02
#define LLDP_TLV_MAC_PHY_AUTONEG_DISABLED           0x00

/* PMD autoneg advertised capability */

#define LLDP_TLV_MAU_AUTONEG_CAP_OTHER                  0
#define LLDP_TLV_MAU_AUTONEG_CAP_10BASET                1
#define LLDP_TLV_MAU_AUTONEG_CAP_10BASETFD              2
#define LLDP_TLV_MAU_AUTONEG_CAP_100BASET4              3
#define LLDP_TLV_MAU_AUTONEG_CAP_100BASETX              4
#define LLDP_TLV_MAU_AUTONEG_CAP_100BASETXFD            5
#define LLDP_TLV_MAU_AUTONEG_CAP_100BASET2              6
#define LLDP_TLV_MAU_AUTONEG_CAP_100BASET2FD            7
#define LLDP_TLV_MAU_AUTONEG_CAP_FDXPAUSE               8
#define LLDP_TLV_MAU_AUTONEG_CAP_FDXAPAUSE              9
#define LLDP_TLV_MAU_AUTONEG_CAP_FDXSPAUSE              10
#define LLDP_TLV_MAU_AUTONEG_CAP_FDXBPAUSE              11
#define LLDP_TLV_MAU_AUTONEG_CAP_1000BASEX              12
#define LLDP_TLV_MAU_AUTONEG_CAP_1000BASEXFD            13
#define LLDP_TLV_MAU_AUTONEG_CAP_1000BASET              14
#define LLDP_TLV_MAU_AUTONEG_CAP_1000BASETFD            15

/* Operational MAU type */

#define LLDP_TLV_MAU_TYPE_AUI                            1
#define LLDP_TLV_MAU_TYPE_10BASE5                        2
#define LLDP_TLV_MAU_TYPE_FOIRL                          3
#define LLDP_TLV_MAU_TYPE_10BASE2                        4
#define LLDP_TLV_MAU_TYPE_10BASET                        5
#define LLDP_TLV_MAU_TYPE_10BASEFP                       6
#define LLDP_TLV_MAU_TYPE_10BASEFB                       7
#define LLDP_TLV_MAU_TYPE_10BASEFL                       8
#define LLDP_TLV_MAU_TYPE_10BROAD36                      9
#define LLDP_TLV_MAU_TYPE_10BASETHD                     10
#define LLDP_TLV_MAU_TYPE_10BASETFD                     11
#define LLDP_TLV_MAU_TYPE_10BASEFLHD                    12
#define LLDP_TLV_MAU_TYPE_10BASEFLFD                    13
#define LLDP_TLV_MAU_TYPE_100BASET4                     14
#define LLDP_TLV_MAU_TYPE_100BASETXHD                   15
#define LLDP_TLV_MAU_TYPE_100BASETXFD                   16
#define LLDP_TLV_MAU_TYPE_100BASEFXHD                   17
#define LLDP_TLV_MAU_TYPE_100BASEFXFD                   18
#define LLDP_TLV_MAU_TYPE_100BASET2HD                   19
#define LLDP_TLV_MAU_TYPE_100BASET2FD                   20
#define LLDP_TLV_MAU_TYPE_1000BASEXHD                   21
#define LLDP_TLV_MAU_TYPE_1000BASEXFD                   22
#define LLDP_TLV_MAU_TYPE_1000BASELXHD                  23
#define LLDP_TLV_MAU_TYPE_1000BASELXFD                  24
#define LLDP_TLV_MAU_TYPE_1000BASESXHD                  25
#define LLDP_TLV_MAU_TYPE_1000BASESXFD                  26
#define LLDP_TLV_MAU_TYPE_1000BASECXHD                  27
#define LLDP_TLV_MAU_TYPE_1000BASECXFD                  28
#define LLDP_TLV_MAU_TYPE_1000BASETHD                   29
#define LLDP_TLV_MAU_TYPE_1000BASETFD                   30
#define LLDP_TLV_MAU_TYPE_10GIGBASEX                    31
#define LLDP_TLV_MAU_TYPE_10GIGBASELX4                  32
#define LLDP_TLV_MAU_TYPE_10GIGBASER                    33
#define LLDP_TLV_MAU_TYPE_10GIGBASEER                   34
#define LLDP_TLV_MAU_TYPE_10GIGBASELR                   35
#define LLDP_TLV_MAU_TYPE_10GIGBASESR                   36
#define LLDP_TLV_MAU_TYPE_10GIGBASEW                    37
#define LLDP_TLV_MAU_TYPE_10GIGBASEEW                   38
#define LLDP_TLV_MAU_TYPE_10GIGBASELW                   39
#define LLDP_TLV_MAU_TYPE_10GIGBASESW                   40

/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI                                   */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*         1 octet            1 octet             1 octet                    */
/*  +-----------------+--------------------+------------------+              */
/*  | MDI power       | Power pair         | Power class      |              */
/*  | support         |                    |                  |              */
/*  |                 |                    |                  |              */
/*  +-----------------+--------------------+------------------+              */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_SUPPORT_SIZE   1
#define LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_PAIR_SIZE      1
#define LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_POWER_CLASS_SIZE     1

#define LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE   (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +3)

/* MDI Power Support (Bitmap) */
#define LLDP_TLV_MDI_POWER_PORT_CLASS_PSE              0x01
#define LLDP_TLV_MDI_POWER_PORT_CLASS_FD               0x00
#define LLDP_TLV_MDI_POWER_SUPPORTED                   0x02
#define LLDP_TLV_MDI_POWER_NOT_SUPPORTED               0x00
#define LLDP_TLV_MDI_POWER_ENABLED                     0x04
#define LLDP_TLV_MDI_POWER_DISABLED                    0x00
#define LLDP_TLV_MDI_POWER_CONTROLABLE                 0x08
#define LLDP_TLV_MDI_POWER_NOT_CONTROLABLE             0x00

/* Power Pair */
#define LLDP_TLV_MDI_POWER_PAIR_SIGNAL                 1
#define LLDP_TLV_MDI_POWER_PAIR_SPARE                  2

/* Power Class*/
#define LLDP_TLV_MDI_POWER_CLASS0                      1
#define LLDP_TLV_MDI_POWER_CLASS1                      2
#define LLDP_TLV_MDI_POWER_CLASS2                      3
#define LLDP_TLV_MDI_POWER_CLASS3                      4
#define LLDP_TLV_MDI_POWER_CLASS4                      5


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION                                */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*         1 octet           4  octets                                       */
/*  +-----------------+--------------------+                                 */
/*  | Link aggregation| aggregated Port ID |                                 */
/*  | status          |                    |                                 */
/*  |                 |                    |                                 */
/*  +-----------------+--------------------+                                 */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_STATUS_SIZE   1
#define LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_PORT_ID_SIZE  4

#define LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +5)

/* Link aggregation status */

#define LLDP_LINK_AGGREGATION_CAPABLE              0x01
#define LLDP_LINK_AGGREGATION_NOT_CAPABLE          0x00
#define LLDP_LINK_AGGREGATION_IN_PROGRESS          0x02
#define LLDP_LINK_AGGREGATION_NOT_IN_PROGRESS      0x00


/*---------------------------------------------------------------------------*/
/* LLDP_TLV_TYPE_ORG_SPECIFIC / IEEE 802.1                                   */
/*                                                                           */
/*  LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE                                  */
/*                                                                           */
/*  Group defined information string:                                        */
/*                                                                           */
/*         2 octets                                                          */
/*  +-----------------+                                                      */
/*  | Maximum 802.3   |                                                      */
/*  | frame size      |                                                      */
/*  |                 |                                                      */
/*  +-----------------+                                                      */
/*---------------------------------------------------------------------------*/

#define LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_FRAME_SIZE_SIZE  2

#define LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE   (LLDP_TLV_ORG_SPECIFIC_MIN_SIZE +2)

/*===========================================================================*/
/*                               TLV-functions                               */
/*===========================================================================*/

typedef struct LLDP_TLV_GET_INFO_TYPE_
{
    LSA_UINT8       ChassisIDSubtype;
    LSA_UINT8       ChassisIDString[LLDP_MAX_CHASSIS_ID_STRING_SIZE];
    LSA_UINT16      ChassisIDLen;               /* string length in bytes */
    LSA_UINT8       PortIDSubtype;
    LSA_UINT8       PortIDString[LLDP_MAX_PORT_ID_STRING_SIZE];
    LSA_UINT16      PortIDLen;                  /* string length in bytes */
    LSA_UINT16      TimeToLive;
} LLDP_TLV_GET_INFO_TYPE;

typedef struct LLDP_TLV_SET_INFO_TYPE_
{
    LSA_UINT8                     ChassisIDSubtype;
    LLDP_UPPER_MEM_PTR_TYPE       pChassisIDString;
    LSA_UINT16                    ChassisIDLen; /* string length in bytes */
    LSA_UINT8                     PortIDSubtype;
    LLDP_UPPER_MEM_PTR_TYPE       pPortIDString;
    LSA_UINT16                    PortIDLen;    /* string length in bytes */
    LSA_UINT16                    TimeToLive;
} LLDP_TLV_SET_INFO_TYPE;

/* string len max LLDP_MAX_CHASSIS_ID_STRING_SIZE bytes ! */

LSA_EXTERN LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVSetChassisPortTTL(
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_SET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo);

LSA_EXTERN LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVGetChassisPortTTL(
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_GET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVGet(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pType,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pLength,
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppValue);

LSA_EXTERN LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVSet(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LSA_UINT32                                       Type,
    LSA_UINT32                                       Length,
    LLDP_UPPER_MEM_PTR_TYPE                          pValue);

/*===========================================================================*/
/* Structures/Types for utility-functions to get/set TLVs within a LLDPDU    */
/*                                                                           */
/* For parameter values see TLV description.                                 */
/*                                                                           */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_CHASSIS_ID                                                */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_CHASSIS_ID_TYPE
{
    LSA_UINT8       ChassisIDSubtype;
    LSA_UINT8       ChassisIDString[LLDP_MAX_CHASSIS_ID_STRING_SIZE];
    LSA_UINT16      ChassisIDLen; /* string length of IDString in bytes */
} LLDP_PARAMS_CHASSIS_ID_TYPE;

typedef struct _LLDP_PARAMS_CHASSIS_ID_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_CHASSIS_ID_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_PORT_ID                                                   */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_PORT_ID_TYPE
{
    LSA_UINT8       PortIDSubtype;
    LSA_UINT8       PortIDString[LLDP_MAX_PORT_ID_STRING_SIZE];
    LSA_UINT16      PortIDLen;   /* string length of PortIDString in bytes */
} LLDP_PARAMS_PORT_ID_TYPE;

typedef struct _LLDP_PARAMS_PORT_ID_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_PORT_ID_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_TTL                                                       */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_TTL_TYPE
{
    LSA_UINT16      TimeToLive;
} LLDP_PARAMS_TTL_TYPE;

typedef struct _LLDP_PARAMS_TTL_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_TTL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_LLDPU_END                                                 */
/*---------------------------------------------------------------------------*/

/* no params present */

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_PORT_DESCR                                                */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_PORT_DESCR_TYPE
{
    LSA_UINT8       PortDescription[LLDP_TLV_PORT_DESCR_MAX_SIZE];
    LSA_UINT16      PortDescriptionLen; /* length in bytes */
} LLDP_PARAMS_PORT_DESCR_TYPE;

typedef struct _LLDP_PARAMS_PORT_DESCR_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_PORT_DESCR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_SYSTEM_NAME                                               */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_SYSTEM_NAME_TYPE
{
    LSA_UINT8       SystemName[LLDP_TLV_SYSTEM_NAME_MAX_SIZE];
    LSA_UINT16      SystemNameLen; /* length in bytes */
} LLDP_PARAMS_SYSTEM_NAME_TYPE;

typedef struct _LLDP_PARAMS_SYSTEM_NAME_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_SYSTEM_NAME_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_SYSTEM_DESCR                                              */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_SYSTEM_DESCR_TYPE
{
    LSA_UINT8       SystemDescr[LLDP_TLV_SYSTEM_DESCR_MAX_SIZE];
    LSA_UINT16      SystemDescrLen; /* length in bytes */
} LLDP_PARAMS_SYSTEM_DESCR_TYPE;

typedef struct _LLDP_PARAMS_SYSTEM_DESCR_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_SYSTEM_DESCR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_SYSTEM_CAP                                                */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_SYSTEM_CAP_TYPE
{
    LSA_UINT16      SystemCapabilities;
    LSA_UINT16      EnabledCapabilities;
} LLDP_PARAMS_SYSTEM_CAP_TYPE;

typedef struct _LLDP_PARAMS_SYSTEM_CAP_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_SYSTEM_CAP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_MGM_ADDR                                                  */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_MGM_ADDR_TYPE
{
    LSA_UINT8       MgmtAddrStringLen; /* Subtype + Address ! */
    LSA_UINT8       MgmtAddrSubtype;
    LSA_UINT8       MgmtAddress[LLDP_TLV_MGM_ADDR_MAX_MGMT_ADDR_STRING_LEN];
    LSA_UINT8       InterfaceNumberingSubtype;
    LSA_UINT32      InterfaceNumber;
    LSA_UINT8       OIDStringLen;
    LSA_UINT8       OIDIdentifier[LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN];
} LLDP_PARAMS_MGM_ADDR_TYPE;

typedef struct _LLDP_PARAMS_MGM_ADDR_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_ORG_SPEC                                                  */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_ORG_SPEC_TYPE
{
    LSA_UINT8       OUI[LLDP_TLV_ORG_SPECIFIC_OUI_SIZE];
    LSA_UINT8       Subtype;
    LSA_UINT8       GroupDefInfoString[LLDP_TLV_ORG_SPECIFIC_MAX_GROUP_DEFINED_ISTRING_SIZE];
    LSA_UINT16      GroupDefInfoStringLen;
} LLDP_PARAMS_ORG_SPEC_TYPE;

typedef struct _LLDP_PARAMS_ORG_SPEC_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_ORG_SPEC_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_UNKNOWN_OPTIONAL                                          */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_UNKNOWN_OPTIONAL_TYPE
{
    LSA_UINT8       Type;        /* TLV-Type */
    LSA_UINT8       Value[LLDP_LEN_MAX_SIZE];
    LSA_UINT16      ValueLen;
} LLDP_PARAMS_UNKNOWN_OPTIONAL_TYPE;

typedef struct _LLDP_PARAMS_UNKNOWN_OPTIONAL_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_UNKNOWN_OPTIONAL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_1_PORT_VLAN_ID                                   */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_PORT_VLAN_ID_TYPE
{
    LSA_UINT16      PortVLANIdentifier;
} LLDP_PARAMS_PORT_VLAN_ID_TYPE;

typedef struct _LLDP_PARAMS_PORT_VLAN_ID_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_PORT_VLAN_ID_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_1_PPVID                                          */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_PPVID_TYPE
{
    LSA_UINT8       Flags;
    LSA_UINT16      ReferenceNumber;
} LLDP_PARAMS_PPVID_TYPE;

typedef struct _LLDP_PARAMS_PPVID_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_PPVID_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_1_VLAN_NAME                                      */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_VLAN_NAME_TYPE
{
    LSA_UINT16      VLANID;
    LSA_UINT8       VLANNameLength;
    LSA_UINT8       VLANName[LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN];
} LLDP_PARAMS_VLAN_NAME_TYPE;

typedef struct _LLDP_PARAMS_VLAN_NAME_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_VLAN_NAME_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_1_PROTOCOL_IDENTITY                              */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_PROTOCOL_IDENTITY_TYPE
{
    LSA_UINT8       Length;
    LSA_UINT8       Identity[LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN];
} LLDP_PARAMS_PROTOCOL_IDENTITY_TYPE;

typedef struct _LLDP_PARAMS_PROTOCOL_IDENTITY_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_PROTOCOL_IDENTITY_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_3_MAC_PHY_CFG_STATUS                             */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE
{
    LSA_UINT8       AutonegSupportStatus;
    LSA_UINT16      PMDAutonegAdvertisedCap;
    LSA_UINT16      OperationalMAUType;
} LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE;

typedef struct _LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_MAC_PHY_CONFIG_STATUS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_3_POWER_VIA_MDI                                  */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_POWER_VIA_MDI_TYPE
{
    LSA_UINT8       MDIPowerSupport;
    LSA_UINT8       PowerPair;
    LSA_UINT8       PowerClass;
} LLDP_PARAMS_POWER_VIA_MDI_TYPE;

typedef struct _LLDP_PARAMS_POWER_VIA_MDI_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_POWER_VIA_MDI_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_3_LINK_AGGREGATION                               */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_LINK_AGGREGATION_TYPE
{
    LSA_UINT8       LinkAggregationStatus;
    LSA_UINT32      AggregatedPortID;
} LLDP_PARAMS_LINK_AGGREGATION_TYPE;

typedef struct _LLDP_PARAMS_LINK_AGGREGATION_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_LINK_AGGREGATION_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* LLDP_PARAM_TYPE_IEEE_802_3_MAX_FRAME_SIZE                                 */
/*---------------------------------------------------------------------------*/

typedef struct _LLDP_PARAMS_MAX_FRAME_SIZE_TYPE
{
    LSA_UINT16      MaxFrameSize;
} LLDP_PARAMS_MAX_FRAME_SIZE_TYPE;

typedef struct _LLDP_PARAMS_MAX_FRAME_SIZE_TYPE   LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_PARAMS_MAX_FRAME_SIZE_PTR_TYPE;

/*===========================================================================*/

typedef union _LLDP_GET_EX_PARAMS_TYPE
{

    LLDP_PARAMS_CHASSIS_ID_TYPE             ChassisID;          /* LLDP_PARAM_TYPE_CHASSIS_ID    */
    LLDP_PARAMS_PORT_ID_TYPE                PortID;             /* LLDP_PARAM_TYPE_PORT_ID       */
    LLDP_PARAMS_TTL_TYPE                    TTL;                /* LLDP_PARAM_TYPE_TTL           */

    LLDP_PARAMS_PORT_DESCR_TYPE             PortDescr;          /* LLDP_PARAM_TYPE_PORT_DESCR    */
    LLDP_PARAMS_SYSTEM_NAME_TYPE            SystemName;         /* LLDP_PARAM_TYPE_SYSTEM_NAME   */
    LLDP_PARAMS_SYSTEM_DESCR_TYPE           SystemDescr;        /* LLDP_PARAM_TYPE_SYSTEM_DESCR  */
    LLDP_PARAMS_SYSTEM_CAP_TYPE             SystemCap;          /* LLDP_PARAM_TYPE_SYSTEM_CAP    */
    LLDP_PARAMS_MGM_ADDR_TYPE               MgmAddr;            /* LLDP_PARAM_TYPE_MGM_ADDR      */
    LLDP_PARAMS_ORG_SPEC_TYPE               OrgUnknown;         /* LLDP_PARAM_TYPE_ORG_SPECIFIC  */
    LLDP_PARAMS_UNKNOWN_OPTIONAL_TYPE       UnknownOptional;    /* LLDP_PARAM_TYPE_UNKNOWN_OPTIONAL */

    /* ORG-Specific. IEEE 802.1 */
    LLDP_PARAMS_PORT_VLAN_ID_TYPE           PortVLANID;         /* LLDP_PARAM_TYPE_IEEE_802_1_PORT_VLAN_ID       */
    LLDP_PARAMS_PPVID_TYPE                  PPVID;              /* LLDP_PARAM_TYPE_IEEE_802_1_PPVID              */
    LLDP_PARAMS_VLAN_NAME_TYPE              VLANName;           /* LLDP_PARAM_TYPE_IEEE_802_1_VLAN_NAME          */
    LLDP_PARAMS_PROTOCOL_IDENTITY_TYPE      ProtIdentity;       /* LLDP_PARAM_TYPE_IEEE_802_1_PROTOCOL_IDENTITY  */

    /* ORG-Specific. IEEE 802.3 */
    LLDP_PARAMS_MAC_PHY_CONFIG_STATUS_TYPE  MACPHYConfigStatus; /* LLDP_PARAM_TYPE_IEEE_802_3_MAC_PHY_CFG_STATUS */
    LLDP_PARAMS_POWER_VIA_MDI_TYPE          PowerViaMDI;        /* LLDP_PARAM_TYPE_IEEE_802_3_POWER_VIA_MDI      */
    LLDP_PARAMS_LINK_AGGREGATION_TYPE       LinkAgg;            /* LLDP_PARAM_TYPE_IEEE_802_3_LINK_AGGREGATION   */
    LLDP_PARAMS_MAX_FRAME_SIZE_TYPE         MaxFrameSize;       /* LLDP_PARAM_TYPE_IEEE_802_3_MAX_FRAME_SIZE     */

} LLDP_EX_PARAMS_TYPE;

typedef struct _LLDP_TLV_GET_EX_PARAMS_TYPE
{
    LSA_UINT32                    Type;       /* Params-Type (not TLV-Type from PDU!) */
    LLDP_EX_PARAMS_TYPE           Params;     /* Params depending on Type             */
} LLDP_TLV_GET_EX_PARAMS_TYPE;

typedef struct _LLDP_TLV_GET_EX_PARAMS_TYPE  LLDP_UPPER_MEM_ATTR *    LLDP_UPPER_TLV_GET_EX_PARAMS_PTR_TYPE;

/* Type */
#define LLDP_PARAM_TYPE_LLDPU_END                        0  /* LLDP_TLV_TYPE_LLDPDU_END                   */
#define LLDP_PARAM_TYPE_CHASSIS_ID                       1  /* LLDP_TLV_TYPE_CHASSIS_ID                   */
#define LLDP_PARAM_TYPE_PORT_ID                          2  /* LLDP_TLV_TYPE_PORT_ID                      */
#define LLDP_PARAM_TYPE_TTL                              3  /* LLDP_TLV_TYPE_TTL                          */

#define LLDP_PARAM_TYPE_PORT_DESCR                       4  /* LLDP_TLV_TYPE_PORT_DESCR                   */
#define LLDP_PARAM_TYPE_SYSTEM_NAME                      5  /* LLDP_TLV_TYPE_SYSTEM_NAME                  */
#define LLDP_PARAM_TYPE_SYSTEM_DESCR                     6  /* LLDP_TLV_TYPE_SYSTEM_DESCR                 */
#define LLDP_PARAM_TYPE_SYSTEM_CAP                       7  /* LLDP_TLV_TYPE_SYSTEM_CAP                   */
#define LLDP_PARAM_TYPE_MGM_ADDR                         8  /* LLDP_TLV_TYPE_MGM_ADDR                     */
#define LLDP_PARAM_TYPE_ORG_SPECIFIC                     9  /* LLDP_TLV_TYPE_ORG_SPECIFIC (if unknown)    */

#define LLDP_PARAM_TYPE_UNKNOWN_OPTIONAL                10  /* Unknown optional TLV                       */

/* Known ORG-Specifc TLVs..                   */

/* IEEE 802.1 */
#define LLDP_PARAM_TYPE_IEEE_802_1_PORT_VLAN_ID         11  /* LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID      */
#define LLDP_PARAM_TYPE_IEEE_802_1_PPVID                12  /* LLDP_TLV_IEEE_802_1_TYPE_PPVID             */
#define LLDP_PARAM_TYPE_IEEE_802_1_VLAN_NAME            13  /* LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME         */
#define LLDP_PARAM_TYPE_IEEE_802_1_PROTOCOL_IDENTITY    14  /* LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY */
/* IEEE 802.3 */
#define LLDP_PARAM_TYPE_IEEE_802_3_MAC_PHY_CFG_STATUS   15  /* LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS*/
#define LLDP_PARAM_TYPE_IEEE_802_3_POWER_VIA_MDI        16  /* LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI     */
#define LLDP_PARAM_TYPE_IEEE_802_3_LINK_AGGREGATION     17  /* LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION  */
#define LLDP_PARAM_TYPE_IEEE_802_3_MAX_FRAME_SIZE       18  /* LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE    */

/*===========================================================================*/

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetEnd(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetChassisID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_CHASSIS_ID_PTR_TYPE            pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_ID_PTR_TYPE               pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetTTL(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_TTL_PTR_TYPE                   pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortDescr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_DESCR_PTR_TYPE            pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemName(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_NAME_PTR_TYPE           pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemDescr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_DESCR_PTR_TYPE          pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemCap(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_CAP_PTR_TYPE            pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMgmAddr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE              pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortVLANID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_VLAN_ID_PTR_TYPE          pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPPVID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PPVID_PTR_TYPE                 pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetVLANName(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_VLAN_NAME_PTR_TYPE             pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetProtIdentity(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PROTOCOL_IDENTITY_TYPE         pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMACPHYConfigStatus(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_MAC_PHY_CONFIG_STATUS_PTR_TYPE  pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMACPHYConfigStatus(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_MAC_PHY_CONFIG_STATUS_PTR_TYPE  pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPowerViaMDI(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_POWER_VIA_MDI_PTR_TYPE          pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetLinkAgg(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_LINK_AGGREGATION_PTR_TYPE       pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMaxFrameSize(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_MAX_FRAME_SIZE_PTR_TYPE         pTLVParams);

LSA_EXTERN LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVGetParams(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_TLV_GET_EX_PARAMS_PTR_TYPE            pTLVParams);

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*==== set and get parameter of the nare-part of the rqb ===*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/


/*=============================================================================
 * function name:  lldp_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         LLDP_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LLDP_RSP                Response:       -
 *      LLDP_RQB_ERROR_TYPE     Error:          -
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (LLDP_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of nare
 *      LLDP_RSP                Response:       LLDP_RSP_OK
 *                                              LLDP_RSP_ERR_PARAM
 *                                              LLDP_RSP_ERR_SEQUENCE
 *                                              or others
 *      LLDP_RQB_ERROR_TYPE     Error:          Additional errorcodes from
 *                                              lower layer
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  LLDP_UPPER_IN_FCT_ATTR  lldp_open_channel(LLDP_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  lldp_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         LLDP_OPC_CLOSE_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LLDP_RSP                Response:       -
 *      LLDP_RQB_ERROR_TYPE     Error:          -
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LLDP_RSP                Response:       LLDP_RSP_OK
 *                                              LLDP_RSP_ERR_PARAM
 *                                              LLDP_RSP_ERR_SEQUENCE
 *                                              or others
 *      LLDP_RQB_ERROR_TYPE     Error:          Additional errorcodes from
 *                                              lower layer
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  LLDP_UPPER_IN_FCT_ATTR  lldp_close_channel(LLDP_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  lldp_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     LLDP_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     LLDP_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of nare
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LLDP_RSP                Response:   -
 *      LLDP_RQB_ERROR_TYPE     Error:      -
 *      LLDP_ARGS_TYPE          Args:       Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     LLDP_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LLDP_RSP                Response:   LLDP_RSP_OK
 *                                          LLDP_RSP_ERR_PARAM
 *                                          LLDP_RSP_ERR_SEQUENCE
 *                                          or others
 *      LLDP_RQB_ERROR_TYPE     Error:      Additional errorcodes from lower
 *                                          layer
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  LLDP_UPPER_IN_FCT_ATTR  lldp_request(LLDP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);



/*****************************************************************************/
/*                                                                           */
/* Output-functions are in lldp_out.h                                        */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*  end of file lldp_usr.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LLDP_USR_H */
