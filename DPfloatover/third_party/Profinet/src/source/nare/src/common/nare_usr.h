#ifndef NARE_USR_H                     /* ----- reinclude-protection ----- */
#define NARE_USR_H

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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_usr.h                                :F&  */
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

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/

#define NARE_OPC_OPEN_CHANNEL                (LSA_OPCODE_TYPE)   0x01
#define NARE_OPC_CLOSE_CHANNEL               (LSA_OPCODE_TYPE)   0x02
#define NARE_OPC_RESOLVE_CONFIG              (LSA_OPCODE_TYPE)   0x03
#define NARE_OPC_RESOLVE_CONFIG_CANCEL       (LSA_OPCODE_TYPE)   0x04
#define NARE_OPC_TEST_IP                     (LSA_OPCODE_TYPE)   0x05
#define NARE_OPC_TEST_IP_CANCEL              (LSA_OPCODE_TYPE)   0x06
#define NARE_OPC_RESOLVE_IP                  (LSA_OPCODE_TYPE)   0x07
#define NARE_OPC_TIMER                       (LSA_OPCODE_TYPE)   0x08
#define NARE_OPC_RESOLVE_CONFIG_NOTIFY       (LSA_OPCODE_TYPE)   0x09
#define NARE_OPC_HELLO                       (LSA_OPCODE_TYPE)   0x0A
#define NARE_OPC_HELLO_STOP                  (LSA_OPCODE_TYPE)   0x0B

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

#define NARE_RSP  LSA_RESPONSE_TYPE  /* dont change */

/* LSA OK-Codes */
#define NARE_RSP_OK                          (NARE_RSP)    LSA_RSP_OK
/* LSA NARE-Codes */
#define NARE_RSP_ERR_RESOURCE                (NARE_RSP)    LSA_RSP_ERR_RESOURCE
#define NARE_RSP_ERR_PARAM                   (NARE_RSP)    LSA_RSP_ERR_PARAM
#define NARE_RSP_ERR_SEQUENCE                (NARE_RSP)    LSA_RSP_ERR_SEQUENCE
#define NARE_RSP_ERR_SYS_PATH                (NARE_RSP)    LSA_RSP_ERR_SYS_PATH

/* NARE OK-Code (MaxOffset = 0x3F) */
/* LSA_RSP_OK_OFFSET = 0x40       */

#define NARE_RSP_OK_CANCEL                   (NARE_RSP)    (LSA_RSP_OK_OFFSET + 0x01)

/* NARE ERR-Codes (MaxOffset = 0x3E)*/
/* LSA_RSP_ERR_OFFSET = 0xC0       */

#define NARE_RSP_ERR_OPCODE                  (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x01)
#define NARE_RSP_ERR_CHANNEL_USE             (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x02)
#define NARE_RSP_ERR_DNS_FAILED              (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x03)
#define NARE_RSP_ERR_DCP_STATION_NOT_FOUND   (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x04)
#define NARE_RSP_ERR_DCP_MULTIPLE_STATIONS   (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x05)
#define NARE_RSP_ERR_DCP_SET_FAILED          (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x06)
#define NARE_RSP_ERR_DCP_SET_TIMEOUT         (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x07)
#define NARE_RSP_ERR_MULTIPLE_IP_USE         (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x08)
#define NARE_RSP_ERR_LL                      (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x09)
#define NARE_RSP_ERR_NO_OWN_IP               (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0A)
#define NARE_RSP_ERR_IP_IN_USE               (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0B)
#define NARE_RSP_ERR_DCP_IDENT_TLV_ERROR     (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0C)
#define NARE_RSP_ERR_IP_RESOLVE_NO_ANSWER    (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0D)
#define NARE_RSP_ERR_NO_TIMER                (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0E)
#define NARE_RSP_ERR_STATION_NAME_DIFFERENT  (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x0F)
#define NARE_RSP_ERR_INVALID_IP_PARAMS       (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x10)
#define NARE_RSP_ERR_DCP_ID_MISMATCH         (NARE_RSP)    (LSA_RSP_ERR_OFFSET + 0x11)
/*===========================================================================*/
/*                              basic types for NARE                         */
/*===========================================================================*/

#define NARE_UPPER_MEM_PTR_TYPE               /* pointer to upper-memory */ \
LSA_VOID                    NARE_UPPER_MEM_ATTR *

#define NARE_UPPER_MEM_U8_PTR_TYPE            /* pointer to upper-memory */ \
LSA_UINT8                   NARE_UPPER_MEM_ATTR *

#define NARE_UPPER_MEM_U16_PTR_TYPE           /* pointer to upper-memory */ \
LSA_UINT16                  NARE_UPPER_MEM_ATTR *

#define NARE_UPPER_MEM_U32_PTR_TYPE          /* pointer to upper-memory */ \
LSA_UINT32                  NARE_UPPER_MEM_ATTR *

#define NARE_UPPER_MEM_U64_PTR_TYPE              /* pointer to device-memory */ \
LSA_UINT64                  NARE_UPPER_MEM_ATTR *


#define NARE_DCP_LOWER_MEM_PTR_TYPE             /* pointer to upper-memory */ \
LSA_VOID                    NARE_DCP_LOWER_MEM_ATTR *

#define NARE_DCP_LOWER_MEM_U8_PTR_TYPE          /* pointer to upper-memory */ \
LSA_UINT8                   NARE_DCP_LOWER_MEM_ATTR *

#define NARE_EDD_LOWER_MEM_PTR_TYPE             /* pointer to upper-memory */ \
LSA_VOID                    NARE_EDD_LOWER_MEM_ATTR *


#define NARE_EDD_LOWER_MEM_U8_PTR_TYPE          /* pointer to upper-memory */ \
LSA_UINT8                   NARE_EDD_LOWER_MEM_ATTR *


#define NARE_SOCK_LOWER_MEM_PTR_TYPE            /* pointer to upper-memory */ \
LSA_VOID                    NARE_SOCK_LOWER_MEM_ATTR *


#define NARE_LOCAL_MEM_PTR_TYPE                /* pointer to local-memory */ \
LSA_VOID                    NARE_LOCAL_MEM_ATTR *

#define NARE_LOCAL_MEM_U8_PTR_TYPE             /* pointer to local-memory */ \
LSA_UINT8                   NARE_LOCAL_MEM_ATTR *

#define NARE_LOCAL_MEM_U16_PTR_TYPE            /* pointer to local-memory */ \
LSA_UINT16                  NARE_LOCAL_MEM_ATTR *

#define NARE_LOCAL_MEM_U32_PTR_TYPE            /* pointer to local-memory */ \
LSA_UINT32                  NARE_LOCAL_MEM_ATTR *

#define NARE_LOCAL_MEM_U8_CONST_PTR_TYPE       /* pointer to local-memory */ \
const LSA_UINT8             NARE_LOCAL_MEM_ATTR *

#define NARE_UPPER_RQB_MEM_PTR_TYPE            /* pointer to upper RQB-mem*/ \
LSA_VOID                    NARE_UPPER_RQB_ATTR *

#define NARE_COMMON_MEM_PTR_TYPE               /* pointer to common-memory */ \
LSA_VOID                    NARE_COMMON_MEM_ATTR *

#define NARE_COMMON_MEM_U8_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT8                   NARE_COMMON_MEM_ATTR *

#define NARE_COMMON_MEM_U16_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT16                  NARE_COMMON_MEM_ATTR *

#define NARE_COMMON_MEM_U32_PTR_TYPE            /* pointer to common-memory */ \
LSA_UINT32                  NARE_COMMON_MEM_ATTR *

#define NARE_COMMON_MEM_BOOL_PTR_TYPE            /* pointer to common-memory */ \
LSA_BOOL                    NARE_COMMON_MEM_ATTR *


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct _NARE_RQB_TYPE     NARE_UPPER_RQB_ATTR *    NARE_UPPER_RQB_PTR_TYPE;

/*===========================================================================*/
/*                       Args for RQB (Request Block)                        */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_OPEN_CHANNEL                                             */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_RQB_OPEN_CHANNEL_TYPE
{
    LSA_HANDLE_TYPE                 handle;
    /* req: -                                */
    /* cnf: channel-handle of NARE           */
    /* At all later RQBs user will write it  */
    /* into handle of NARE_RQB_HEADER        */
    LSA_HANDLE_TYPE                 handle_upper;
    /* req: channel-handle of user           */
    /* cnf: -                                */
    /* At all confirmations NARE writes it   */
    /* into handle of NARE_RQB_HEADER.       */
    LSA_SYS_PATH_TYPE               sys_path;
    /* req: system-path of channel           */
    /* cnf: -                                */
    LSA_VOID                        LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR,
                                                nare_request_upper_done_ptr)
    (NARE_UPPER_RQB_PTR_TYPE upper_rqb_ptr);
    /* req: pointer to callback-function     */
    /* cnf: -                                */
} NARE_RQB_OPEN_CHANNEL_TYPE;

typedef struct _NARE_RQB_OPEN_CHANNEL_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_OPEN_CHANNEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_CLOSE_CHANNEL                                            */
/*---------------------------------------------------------------------------*/

/* no args */


/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_RESOLVE_CONFIG                                           */
/*---------------------------------------------------------------------------*/

#define NARE_IP_ADDR_SIZE   4
#define NARE_MAC_ADDR_SIZE  6

typedef LSA_UINT8 NARE_MAC_TYPE[NARE_MAC_ADDR_SIZE];

typedef NARE_MAC_TYPE NARE_UPPER_RQB_ATTR *    NARE_UPPER_RQB_MAC_PTR_TYPE;

typedef LSA_UINT8 NARE_IP_TYPE[NARE_IP_ADDR_SIZE];

typedef NARE_IP_TYPE  NARE_UPPER_RQB_ATTR *    NARE_UPPER_RQB_IP_PTR_TYPE;

typedef struct _NARE_CONFIG_TYPE
{
    NARE_IP_TYPE        IPAddr;
    NARE_IP_TYPE        SubMask;
    NARE_IP_TYPE        DefRouter;
} NARE_CONFIG_TYPE;

typedef struct _NARE_RESOLVE_TYPE
{
    NARE_IP_TYPE        IPAddr;     /* 0.0.0.0 if no IP       */
    NARE_IP_TYPE        SubMask;    /* 0.0.0.0 if no SubMask  */
    NARE_IP_TYPE        DefRouter;  /* 0.0.0.0 if no DefRout. */
    NARE_MAC_TYPE       MACAddr;    /* 0.0.0.0.0.0 if no MAC  */
} NARE_RESOLVE_TYPE;


typedef struct _NARE_ALIAS_TYPE
{
    NARE_UPPER_MEM_PTR_TYPE      pName;            /* pointer to StationName          */
    LSA_UINT8                    NameLen;          /* Len in byte of Name             */
} NARE_ALIAS_TYPE;

typedef struct _NARE_ALIAS_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_ALIAS_PTR_TYPE;

/* Defines for Extended Mode */

#define NARE_EXT_MODE_LEGACY            0
#define NARE_EXT_MODE_CHECK_DEVICE_ID   1


typedef struct _NARE_RQB_RESOLVE_CONFIG_TYPE
{
    NARE_IP_TYPE                 LocalIPAddr;      /* loacal IP-Address                */
    NARE_UPPER_MEM_PTR_TYPE      pStationName;     /* pointer to StationName           */
    LSA_UINT8                    StationNameLen;   /* Len in byte of Name              */
    LSA_UINT8                    Mode;             /* mode of operation                */
    LSA_BOOL                     CheckIP;          /* CheckIP if IP is known           */
    LSA_BOOL                     SetIPRemanent;    /* Set IP-Parameters remanent with  */
                                                   /* DCP-Set?                         */
    LSA_UINT16                   Tolerance;        /* Time in Milliseconds to be used  */
                                                   /* as timeout with DCP-Identify.    */
                                                   /* the default value of 1000 is     */
                                                   /* used if 0                        */
    LSA_UINT32                   RequestID;        /* RequestID used for Cancel        */
    LSA_BOOL                     Notify;           /* Notify DCP-Identify responses    */
    LSA_UINT32                   NotifyID;         /* NotifyID used for Notify         */
    NARE_UPPER_ALIAS_PTR_TYPE    pStationNameAlias;/* pointer to array of Alias names  */
    LSA_UINT16                   AliasCnt;         /* Number of Alias names      (in)  */
    LSA_UINT16                   AliasUsed;        /* Number of Alias name used (out)  */
                                                   /* 0: none, 1..AliasCnt             */
    LSA_BOOL                     StationNameSetEqual;                        /* (out)  */
                                                    /* TRUE:                           */
                                                    /* The StationName within the      */
                                                    /* identify response was equal to  */
                                                    /* the one from the request.       */
                                                    /* FALSE:                          */
                                                    /* The StationName within the      */
                                                    /* identify response was not       */
                                                    /* present or different            */
    LSA_UINT8                    ExtendedMode;      /* spez. whether VendorId DeviceID */
                                                    /* and InstanceID must be checked  */
    LSA_BOOL             AllowStationNameOverwrite; /* if TRUE: Stationname will be    */
                                                    /* overwritten                     */
    LSA_UINT16                   DeviceID;          /* DeviceID                        */
    LSA_UINT16                   VendorID;          /* VendorID                        */
    LSA_UINT16                   InstanceID;        /* InstanceID                      */
    NARE_CONFIG_TYPE             Cfg;               /* config-parameters               */
    NARE_RESOLVE_TYPE            Res;               /* resolve-parameters              */
} NARE_RQB_RESOLVE_CONFIG_TYPE;

typedef struct _NARE_RQB_RESOLVE_CONFIG_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_RESOLVE_CONFIG_PTR_TYPE;

/* Mode: */

#define NARE_MODE_CONFIG_IP_FIXED   0  /* resolve and config. if and nos fixed   */
#define NARE_MODE_CONFIG_IP_DNS     1  /* resolve and config. ip per DNS */
#define NARE_MODE_RESOLVE_DCP       2  /* resolve only with DCP          */
#define NARE_MODE_RESOLVE_DNS       3  /* resolve only with DNS          */
#define NARE_MODE_CONFIG_NOS_FIXED  4  /* resolve and config. nos fixed          */
/* nos: name of station */
/* CheckIP: */
/* LSA_TRUE:   Always check the IP with ARP if the IP address is known.      */
/* LSA_FALSE:  Only check the IP with ARP if the IP shall/needs to be SET    */

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_RESOLVE_CONFIG_CANCEL                                    */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_OPC_RESOLVE_CONFIG_CANCEL_TYPE
{
    LSA_UINT32              RequestID;   /* Request ID from Resolve-Config   */
    /* to cancel                        */
} NARE_OPC_RESOLVE_CONFIG_CANCEL_TYPE;

typedef struct _NARE_OPC_RESOLVE_CONFIG_CANCEL_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_RESOLVE_CONFIG_CANCEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_TEST_IP                                                  */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_OPC_TEST_IP_TYPE
{
    NARE_IP_TYPE        LocalIPAddr;/* loacal IP-Address          (in) */
    NARE_IP_TYPE        IPAddr;     /* IP-Address to test         (in) */
    NARE_MAC_TYPE       MACAddr;    /* MAC if IP found           (out) */
    LSA_UINT32          RequestID;  /* RequestID used for Cancel  (in) */
    LSA_BOOL            UseDelay;   /* if true an initial delay is used (in) */ 
} NARE_OPC_TEST_IP_TYPE;

typedef struct _NARE_OPC_TEST_IP_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_TEST_IP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_TEST_IP_CANCEL                                           */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_OPC_TEST_IP_CANCEL_TYPE
{
    LSA_UINT32              RequestID;   /* RequestID for Cancel             */
} NARE_OPC_TEST_IP_CANCEL_TYPE;

typedef struct _NARE_OPC_TEST_IP_CANCEL_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_TEST_IP_CANCEL_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_RESOLVE_IP                                               */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_OPC_RESOLVE_IP_TYPE
{
    NARE_IP_TYPE    LocalIPAddr;    /* loacal IP-Address      (in)  */
    NARE_IP_TYPE    IPAddr;         /* IP-Address to resolve  (in)  */
    NARE_MAC_TYPE   MACAddr;        /* MAC if IP found        (out) */
} NARE_OPC_RESOLVE_IP_TYPE;

typedef struct _NARE_OPC_RESOLVE_IP_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_RESOLVE_IP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_TIMER  (for NARE internal use only!)                     */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_RQB_TIMER_TYPE
{
    LSA_UINT16              TimerID; /* NARE-TimerID */
    LSA_UINT32              UserID;
} NARE_RQB_TIMER_TYPE;

typedef struct _NARE_RQB_TIMER_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_TIMER_PTR_TYPE;

#define NARE_TIMER_USER_ID_ARP   0

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_RESOLVE_CONFIG_NOTIFY                                    */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_RQB_RESOLVE_CONFIG_NOTIFY_TYPE
{
    LSA_UINT32                   NotifyID;         /* NotifyID used for Notify        */
    NARE_RESOLVE_TYPE            Res;              /* resolve-parameters              */
} NARE_RQB_RESOLVE_CONFIG_NOTIFY_TYPE;

typedef struct _NARE_RQB_RESOLVE_CONFIG_NOTIFY_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_RESOLVE_CONFIG_NOTIFY_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_HELLO                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_RQB_HELLO_TYPE
{
    LSA_UINT32                   SendInterval;     /* SendInterval in 10ms period     */
    LSA_UINT32                   Delay;            /* Delay first send(0..x) in 10ms  */
    LSA_UINT32                   SendCnt;          /* Number of sends (1..x)          */
    NARE_UPPER_MEM_PTR_TYPE      pTLV;             /* pointer HELLO DCP TLV chain.    */
	LSA_UINT32                   TLVLength;        /* length of the TLV chain         */
    LSA_UINT32                   OptCnt;           /* number of TLV options in chain  */
} NARE_RQB_HELLO_TYPE;

typedef struct _NARE_RQB_HELLO_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_HELLO_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: NARE_OPC_HELLO_STOP                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_RQB_HELLO_STOP_TYPE
{
    LSA_UINT32                   RequestID;        /* not used yet. dont care         */
} NARE_RQB_HELLO_STOP_TYPE;

typedef struct _NARE_RQB_HELLO_STOP_TYPE   NARE_UPPER_MEM_ATTR *    NARE_UPPER_HELLO_STOP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*                           RQB- Header for Services                        */
/*---------------------------------------------------------------------------*/

typedef union _NARE_RQB_ARGS_TYPE
{
    NARE_RQB_OPEN_CHANNEL_TYPE          channel;       /* open channel             */
    NARE_RQB_RESOLVE_CONFIG_TYPE        ResConf;       /* resolve and config       */
    NARE_OPC_RESOLVE_CONFIG_CANCEL_TYPE ResConfCancel; /* cancel resconf request   */
    NARE_OPC_TEST_IP_TYPE               TestIP;        /* Test IP request          */
    NARE_OPC_TEST_IP_CANCEL_TYPE        TestIPCancel;  /* Test IP cancel request   */
    NARE_OPC_RESOLVE_IP_TYPE            ResolveIP;     /* Resolve IP request       */
    NARE_RQB_TIMER_TYPE                 Timer;         /* TimerEvent. internal use.*/
    NARE_RQB_RESOLVE_CONFIG_NOTIFY_TYPE Notify;        /* Provide Notify rqb       */
    NARE_RQB_HELLO_TYPE                 Hello;         /* start Hello sends        */
    NARE_RQB_HELLO_STOP_TYPE            HelloStop;     /* stop Hello sends         */
} NARE_RQB_ARGS_TYPE;

typedef struct _NARE_RQB_TYPE
{
    NARE_RQB_HEADER
    NARE_RQB_ARGS_TYPE            args;       /* IN/OUT: Args of Request     */
    NARE_RQB_TRAILER
} NARE_RQB_TYPE;

/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*==== set and get parameter of the nare-part of the rqb ===*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/


/*=============================================================================
 * function name:  nare_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         NARE_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      NARE_RSP                Response:       -
 *      NARE_RQB_ERROR_TYPE     Error:          -
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (NARE_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of nare
 *      NARE_RSP                Response:       NARE_RSP_OK
 *                                              NARE_RSP_ERR_PARAM
 *                                              NARE_RSP_ERR_SEQUENCE
 *                                              or others
 *      NARE_RQB_ERROR_TYPE     Error:          Additional errorcodes from
 *                                              lower layer
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  NARE_UPPER_IN_FCT_ATTR  nare_open_channel(NARE_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  nare_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         NARE_OPC_CLOSE_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      NARE_RSP                Response:       -
 *      NARE_RQB_ERROR_TYPE     Error:          -
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      NARE_RSP                Response:       NARE_RSP_OK
 *                                              NARE_RSP_ERR_PARAM
 *                                              NARE_RSP_ERR_SEQUENCE
 *                                              or others
 *      NARE_RQB_ERROR_TYPE     Error:          Additional errorcodes from
 *                                              lower layer
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  NARE_UPPER_IN_FCT_ATTR  nare_close_channel(NARE_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  nare_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     NARE_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     NARE_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of nare
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      NARE_RSP                Response:   -
 *      NARE_RQB_ERROR_TYPE     Error:      -
 *      NARE_ARGS_TYPE          Args:       Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     NARE_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      NARE_RSP                Response:   NARE_RSP_OK
 *                                          NARE_RSP_ERR_PARAM
 *                                          NARE_RSP_ERR_SEQUENCE
 *                                          or others
 *      NARE_RQB_ERROR_TYPE     Error:      Additional errorcodes from lower
 *                                          layer
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  NARE_UPPER_IN_FCT_ATTR  nare_request(NARE_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);


/*=============================================================================
 * function name:  nare_ARPResponseFrameIn
 *
 * function:       Siffer for ARP-Response frames
 *
 *                 This function has to be called by systemadaption if ARP-
 *                 frame sniffing is activated via output-macro
 *                 NARE_ARP_RESPONSE_FRAME_CONTROL().
 *
 *                 This function checks the ARP-frame. After checking it calls
 *                 NARE_ARP_RESPONSE_FRAME_OUT() then the function returns.
 *                 NARE_ARP_RESPONSE_FRAME_OUT()  may be used if needed but
 *                 returning from this function does also signal readyness.
 *
 *
 * parameters:     LSA_UINT16                   NICId
 *                 NARE_EDD_LOWER_MEM_PTR_TYPE  pFrame
 *                 LSA_UINT16                   FrameLen
 *
 *                 NICId     : NIC-ID from EDD-Systemhandle this frame was
 *                             received from.
 *
 *                 pFrame    : Pointer to ARP-response frame.
 *
 *                             It can be configured if this function should
 *                             check for valid ARP-response frames and ignore
 *                             all others or if it expect valid ARP-frames.
 *
 *                 FrameLen    Length of Frame. used to validate ARP-Frame
 *
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/

LSA_VOID NARE_SYSTEM_IN_FCT_ATTR nare_ARPResponseFrameIn(
    LSA_UINT16                      NICId,
    NARE_EDD_LOWER_MEM_PTR_TYPE     pFrame,
    LSA_UINT16                      FrameLen);


/*****************************************************************************/
/*                                                                           */
/* Output-functions are in nare_out.h                                        */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*  end of file nare_usr.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_USR_H */
