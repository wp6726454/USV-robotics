
#ifndef NARE_INT_H                       /* ----- reinclude-protection ----- */
#define NARE_INT_H

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
/*  F i l e               &F: nare_int.h                                :F&  */
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
/*  NARE.                                                                    */
/*                                                                           */
/*****************************************************************************/

#include "nare_trc.h"
#include "nare_dbg.h"

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
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

/* NARE module IDs used in C-Files */

#define NARE_MODULE_ID_NARE_USR              0x0
#define NARE_MODULE_ID_NARE_SYS              0x1
#define NARE_MODULE_ID_NARE_GLB              0x3
#define NARE_MODULE_ID_NARE_DBG              0x4
#define NARE_MODULE_ID_NARE_DCP              0x5
#define NARE_MODULE_ID_NARE_SOCK             0x6
#define NARE_MODULE_ID_NARE_TIM              0x7
#define NARE_MODULE_ID_NARE_EDD              0x8

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef NARE_IP_TYPE  NARE_LOCAL_MEM_ATTR *    NARE_LOCAL_MEM_IP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Overview of Handle and Requests handled by NARE:                          */
/*                                                                           */
/* We have four types of handles:                                            */
/*                                                                           */
/* - SOCK-Systemchannel (one for all NICs)                                   */
/* - DCP-Systemchannel (one for every NIC)                                   */
/* - EDD-Systemchannel (one for every NIC)                                   */
/* - USER-Channels (multiple per NIC possible)                               */
/*                                                                           */
/* The Systemchannels have to be opened by Systemadaption bevore a user      */
/* can open a USER-Channel. User-Requests can only be done with USER-Channels*/
/*                                                                           */
/* All Requests are handled by the User-Channel. Each user Channel holds a   */
/* number of RCTRL-blocks (RequestControlBlocks) which hold all information  */
/* about the current state of the Request within NARE.                       */
/*                                                                           */
/* This User-RCTRL-Block will move through the USER, DCP, EDD and SOCK       */
/* channels depending on the Actions to perform.                             */
/*                                                                           */
/* If a User-Request needs a DCP-Call the RCTRL-Blcok is queued within the   */
/* DCP-Handle for performing a DCP-Request. Within the DCP-Handle we have    */
/* DCP-RCTRL-Blocks for handling multiple DCP-Requests simultaneously.       */
/* If such a DCP-RCTRL-Block is free the User-RCTRL-Block is asigned to      */
/* this DCP-RCTRL-Block and the DCP-Request is initiated. if not DCP-RCTRL-  */
/* block is free the User-RCTRL-Block remains queued till one gets free.     */
/*                                                                           */
/* The same handling is done with EDD and SOCK-Requests.                     */
/*                                                                           */
/* The max number of simultaneous requests are configured by                 */
/*                                                                           */
/* - NARE_CFG_MAX_RUSER : Max number of User-Requests simultaneously handled */
/*                        more requests gets queued till one RCTRL gets free */
/*                        (e.g. a user-requests finishes)                    */
/* - NARE_CFG_MAX_RDCP  : Max number of DCP-Requests simultaneously handled. */
/*                        Each DCP-RCTRL-Block is asigned to a User-RCTRL-   */
/*                        block (contains a pointer to it). Multiple User-   */
/*                        channels may use the same DCP-Channel (same NIC)   */
/* - NARE_CFG_MAX_REDD  : Max number of EDD-Requests simultaneously handled. */
/*                        Each EDD-RCTRL-Block is asigned to a User-RCTRL-   */
/*                        block (contains a pointer to it). Multiple User-   */
/*                        channels may use the same EDD-Channel (same NIC)   */
/* - NARE_CFG_MAX_RSOCK : Max number of SOCK-Requests simultaneously handled */
/*                        Each SOCK-RCTRL-Block is asigned to a User-RCTRL-  */
/*                        block (contains a pointer to it). All User-        */
/*                        channels use the same SOCK-Channel (NIC independ.) */
/*                                                                           */
/* Example:                                                                  */
/*                                                                           */
/* NARE_CFG_MAX_RUSER 10                                                     */
/* NARE_CFG_MAX_RDCP  20                                                     */
/* NARE_CFG_MAX_REDD  20                                                     */
/* NARE_CFG_MAX_RSOCK 20                                                     */
/*                                                                           */
/* We can handle at least 10 User-requests per user-channel simultaneously   */
/* This 10 requests are forwarded to DCP, EDD or SOCK. In DCP we can handle  */
/* at least 20 DCP-requests at once, in SOCK too. Alle other Requests        */
/* or blocks are queued within the handle till resources get free.           */
/*                                                                           */
/*                                                                           */
/* The RCTRL-Block managed as array of RCTRL-Blocks. So we can use the       */
/* index into the array for reference. For DCP and SOCK we use this index    */
/* as user_id within the lower RQB, so we find the proper RCTRL-Block on     */
/* confirmation (the requests are not first in first out! so we need to      */
/* find the proper RCTRL!)                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Controlblock for Request handling (User Request Control Block. RCTRL)     */
/*                                                                           */
/* This controlblock contains all information of a request currently in      */
/* progress within NARE. The block contains all information about the        */
/* current state of the request. The block will be moved/chained within      */
/* several queues/tables while it processes the states.                      */
/*                                                                           */
/* Note to pRState:                                                          */
/*                                                                           */
/* pRState points to the current state of the request.                       */
/*                                                                           */
/* Depending on the request and mode we have to perform specific actions     */
/* (States) for this request. For every mode we have a fixed State-Sequence  */
/* which we advance in. So on start pRState will point to the proper state   */
/* sequence (const-array of states). To advance we only have to increment    */
/* pRState and it points to the next state. So we need no switch/case for    */
/* determining the next state.                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct _NARE_USER_RCTRL_TYPE   NARE_LOCAL_MEM_ATTR *    NARE_USER_RCTRL_PTR_TYPE;

typedef struct _NARE_USER_RCTRL_TYPE
{
    LSA_UINT8                        BState;   /* Blockstate                         */
    LSA_UINT8                        QueueType;/* Queuetype the request is pending   */
    NARE_LOCAL_MEM_U8_CONST_PTR_TYPE pRState;  /* Pointer to State of Ctrl-Block     */
    /* States are hold within const-arrays*/
    LSA_UINT8                        DCPSetMode;  /* Mode for DCP-Set                */
    LSA_BOOL                         DoCancel; /* TRUE if Request should be cancled  */
                                               /*      pCancelRQB present.           */
    LSA_UINT16                       ThisIdx;  /* Index in RCTRL-Table of this Block */
    LSA_UINT32                       DCPStationNameState; /* Station of Device from identify */
    NARE_USER_RCTRL_PTR_TYPE         pNext;    /* used for queuing blocks            */
    NARE_USER_RCTRL_PTR_TYPE         pPrev;    /* used for queuing blocks            */
    NARE_UPPER_RQB_PTR_TYPE          pRQB;        /* Request RQB                     */
    NARE_UPPER_RQB_PTR_TYPE          pCancelRQB;  /* CANCEL-RQB if Cancel pending    */
} NARE_USER_RCTRL_TYPE;

/* defines of BState (Blockstate) */

#define NARE_RCTRL_BSTATE_FREE          0  /* Controlblock is free for use    */
#define NARE_RCTRL_BSTATE_USED          1  /* Block is in use.                */

/* defines of QueueType.            */

#define NARE_RCTRL_QTYPE_NONE            0 /* RCTRL not queued. running or    */
/* inactive.                       */
#define NARE_RCTRL_QTYPE_DCP_PENDING     1 /* RCTRL queued in DCP  handle     */
#define NARE_RCTRL_QTYPE_DCP_RUNNING     2 /* RCTRL running in DCP handle     */
#define NARE_RCTRL_QTYPE_EDD_PENDING     3 /* RCTRL queued in EDD  handle     */
#define NARE_RCTRL_QTYPE_EDD_RUNNING     4 /* RCTRL running in EDD  handle    */
#define NARE_RCTRL_QTYPE_SOCK_PENDING    5 /* RCTRL queued in SOCK handle     */
#define NARE_RCTRL_QTYPE_SOCK_RUNNING    6 /* RCTRL running in SOCK handle    */
#define NARE_RCTRL_QTYPE_EDD_OPEN        7 /* Spezial RCTRL used for EDD      */
/* Open-Channel IP-test            */

/* defines of RState (Requeststate) pRState points to.          */
/* pRState not present if QueueType = NARE_RCTRL_QTYPE_EDD_OPEN */

#define NARE_RCTRL_RSTATE_START           0 /* Start of statemachine       1,2 */
#define NARE_RCTRL_RSTATE_SOCK_GHBN       1 /* GetHostByName is running    1   */
#define NARE_RCTRL_RSTATE_DCP_IDENT       2 /* Running Identify            1   */
#define NARE_RCTRL_RSTATE_EDD_ARP         3 /* ARP is running              1   */
#define NARE_RCTRL_RSTATE_EDD_TEST_IP_ARP 4 /* ARP is running              2   */
#define NARE_RCTRL_RSTATE_EDD_RES_IP_ARP  5 /* ARP is running              3   */
#define NARE_RCTRL_RSTATE_DCP_SET         6 /* Running Set                 1   */
#define NARE_RCTRL_RSTATE_READY        0xFF /* ready                       1,2 */

/* Notes: */
/* 1 : for requests with opcode NARE_OPC_RESOLVE_CONFIG  */
/* 2 : for requests with opcode NARE_OPC_TEST_IP         */
/* 3 : for requests with opcode NARE_OPC_RESOLVE_IP      */

/* defines of DCPSetMode (BitMasks)  */

#define NARE_RCTRL_DCP_SET_NONE         0x0 /* No DCP-Set                      */
#define NARE_RCTRL_DCP_SET_IP           0x1 /* Set IP-Params                   */
#define NARE_RCTRL_DCP_SET_STATIONNAME  0x2 /* Set StationName                 */
#define NARE_RCTRL_DCP_SET_ALL          (NARE_RCTRL_DCP_SET_IP | NARE_RCTRL_DCP_SET_STATIONNAME)

/* defines of DCPStationNameState  */
#define NARE_RCTRL_DCP_STATION_NAME_NOT_SET       0x0 /* No Station name set          */
#define NARE_RCTRL_DCP_STATION_NAME_SET_EQUAL     0x1 /* Station name set equal       */
#define NARE_RCTRL_DCP_STATION_NAME_SET_DIFFERENT 0x2 /* Station name set not equal   */

typedef struct _NARE_USER_RCTRL_LINK_BASE_TYPE
{
    NARE_USER_RCTRL_PTR_TYPE          pBottom;
    NARE_USER_RCTRL_PTR_TYPE          pTop;
} NARE_USER_RCTRL_LINK_BASE_TYPE;


typedef struct _NARE_USER_RCTRL_LINK_BASE_TYPE  NARE_LOCAL_MEM_ATTR *    _NARE_USER_RCTRL_LINK_BASE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* This structure is  used for internal request initiated by timeout-handler.*/
/* InUse is set if the RQB is underway (from timeout to nare_system(). if    */
/* it arrives nare_system() InUse is reset to LSA_FALSE. So only one RQB is  */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef volatile struct _NARE_INTERNAL_REQUEST_STRUCT
{
    LSA_BOOL                    InUse;          /* LSA_TRUE if in rqb-queue  */
    NARE_UPPER_RQB_PTR_TYPE     pRQB;
} NARE_INTERNAL_REQUEST_TYPE;

typedef NARE_INTERNAL_REQUEST_TYPE  NARE_LOCAL_MEM_ATTR * NARE_INTERNAL_REQUEST_PTR_TYPE;


/*===========================================================================*/
/* Global Timer management                                                   */
/* This is used to asign a LSA-Timer ID to an static RQB                     */
/* This management is deviceindependend and used for all devices.            */
/*===========================================================================*/

#define NARE_CFG_MAX_TIMER 1   /* number of Timers for all  */
/* devices                   */

typedef volatile struct _NARE_TIMER_INFO_TYPE
{
    LSA_BOOL                        InUse;
    LSA_TIMER_ID_TYPE               TimerID;
    NARE_INTERNAL_REQUEST_TYPE      IntRQB;
    LSA_VOID                        LSA_FCT_PTR(NARE_LOCAL_FCT_ATTR, Cbf)
    (NARE_INTERNAL_REQUEST_PTR_TYPE   pIntRQB);
} NARE_TIMER_INFO_TYPE;

typedef volatile struct _NARE_TIMER_INFO_TYPE     NARE_LOCAL_MEM_ATTR *    NARE_TIMER_INFO_PTR_TYPE;

#define NARE_TIMER_ID_INVALID   0xFFFF

/*===========================================================================*/
/* Handle-management-structures                                              */
/*===========================================================================*/

/* forward declaration */
typedef struct _NARE_HDB_USER_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_HDB_USER_PTR_TYPE;

/* general handle parameters */
typedef struct _NARE_HANDLE_PARAMS_TYPE
{
    LSA_SYS_PATH_TYPE                   SysPath;
    LSA_SYS_PTR_TYPE                    pSys;
    NARE_DETAIL_PTR_TYPE                pDetail;
    LSA_HANDLE_TYPE                     HandleUpper;
    LSA_VOID                            LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf)(NARE_UPPER_RQB_PTR_TYPE pRQB);
} NARE_HANDLE_PARAMS_TYPE;

#ifndef NARE_CFG_NO_SOCK
/* --------------------------------------------------------------------------*/
/* for SOCK-Systemchannel (only one SOCK-Channel possible)..                 */
/*                                                                           */
/* for handling all requests to SOCK. We have a queue which holds all        */
/* user-request blocks awaiting service. i.e. need a request to SOCK.        */
/* If a free SOCK-Request nlock is present pending user-requests are         */
/* removed and assotiated with a SOCK-Request block.                         */
/*                                                                           */
/* We have only a limited number of SOCK-Request blocks (configured). This   */
/* number is the number of simultaneous active SOCK-requests.                */
/*                                                                           */
/* --------------------------------------------------------------------------*/

typedef struct _NARE_SOCK_RCTRL_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_SOCK_RCTRL_PTR_TYPE;

/* LowerLayer (SOCK) Request control block  */
typedef struct _NARE_SOCK_RCTRL_TYPE
{
    LSA_BOOL                            InUse;     /* block in use?                       */
    LSA_UINT16                          ThisIdx;   /* Index in RCTRL-Table of this Block  */
    NARE_SOCK_RCTRL_PTR_TYPE            pNext;     /* used for queuing blocks             */
    NARE_SOCK_RCTRL_PTR_TYPE            pPrev;     /* used for queuing blocks             */
    NARE_USER_RCTRL_PTR_TYPE            pRCtrl;    /* user-control block for this request */
} NARE_SOCK_RCTRL_TYPE;

typedef struct _NARE_SOCK_RCTRL_LINK_BASE_TYPE
{
    NARE_SOCK_RCTRL_PTR_TYPE          pBottom;
    NARE_SOCK_RCTRL_PTR_TYPE          pTop;
} NARE_SOCK_RCTRL_LINK_BASE_TYPE;

typedef struct _NARE_SOCK_RCTRL_LINK_BASE_TYPE  NARE_LOCAL_MEM_ATTR *    NARE_SOCK_RCTRL_LINK_BASE_PTR_TYPE;

/* declaring a pointer to an array */
typedef NARE_SOCK_RCTRL_TYPE                              NARE_SOCK_RCTRL_ARRAY_TYPE[1];
typedef NARE_SOCK_RCTRL_ARRAY_TYPE  NARE_LOCAL_MEM_ATTR * NARE_SOCK_RCTRL_ARRAY_PTR_TYPE;

typedef struct _NARE_SOCK_RCTRL_MGM_TYPE
{
    LSA_UINT32                          Cnt;       /* Number of RequestControlblocks*/
                                                   /* set to NARE_CFG_MAX_RSOCK     */
    LSA_UINT32                          Free;      /* Number of free blocks         */
    LSA_UINT32                          Used;      /* Number of used blocks         */
    NARE_SOCK_RCTRL_LINK_BASE_TYPE      pFree;     /* LinkedList of free RCtrl.     */
    NARE_SOCK_RCTRL_ARRAY_PTR_TYPE      pTable;    /* Table of RequestControlblocks */
    /* Cnt entrys.                   */
} NARE_SOCK_RCTRL_MGM_TYPE;


typedef struct _NARE_HDB_SOCK_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_HDB_SOCK_PTR_TYPE;

typedef struct _NARE_HDB_SOCK_TYPE
{
    LSA_UINT8                        State;        /* State of handle               */
    LSA_HANDLE_TYPE                  ThisHandle;   /* this handle                   */
    LSA_UINT32                       TraceIdx;     /* trace index for index trace   */
    NARE_HANDLE_PARAMS_TYPE          Params;       /* general handle parameters     */
    NARE_UPPER_RQB_PTR_TYPE          pOCReq;       /* Open or Close Request         */
    LSA_UINT16                       UserCnt;      /* Number of User-Channels using */
                                                   /* this DCP-channel.             */
    LSA_HANDLE_TYPE                  SOCKHandle;   /* If InUse. the SOCK handle     */
    LSA_UINT32                       PendRCtrlCnt; /* Number of pending user RCtrlB.*/
    NARE_USER_RCTRL_LINK_BASE_TYPE   PendRCtrl;    /* Pending User-RCtrl-Blocks     */
                                                   /* ------------------------------*/
    NARE_SOCK_RCTRL_MGM_TYPE         SOCKRCtrl;    /* Request-Control-blocks for    */
                                                   /* SockRequests.                 */
                                                   /* ------------------------------*/
  LSA_UINT16                         NoOfRCtrlCnt;    /* No. of parallel RQs           */
} NARE_HDB_SOCK_TYPE;


/* we have only one SOCK channel!  */

typedef struct _NARE_HDB_SOCK_INFO_TYPE
{
    LSA_UINT16                    HandleCnt;       /* Number of handles supported   */
                                                   /* set to 1.                     */
    LSA_UINT16                    UsedCnt;         /* currently used channels       */
    NARE_HDB_SOCK_TYPE            HDB[1];          /* to array of MaxHandle         */
                                                   /* NARE_HDB_SOCK_TYPE types.     */
} NARE_HDB_SOCK_INFO_TYPE;


#endif
/* --------------------------------------------------------------------------*/
/* struct for holding the additional IDs                                     */
/* --------------------------------------------------------------------------*/
typedef struct _NARE_ID_STORAGE_TYPE
{
    LSA_UINT32                    Foundstate;  /*reflects found ids      */
    LSA_UINT16                    VendorID;
    LSA_UINT16                    DeviceID;
    LSA_UINT16                    InstanceID;
} NARE_ID_STORAGE_TYPE;


/* --------------------------------------------------------------------------*/
/* opening Channel states                                                    */
/* --------------------------------------------------------------------------*/
#define NOT_FINISHED        0
#define FINISHED_OK         1
#define FINISHED_REL_SYSPTR 2

/* State: */

#define NARE_HANDLE_STATE_FREE                      0
#define NARE_HANDLE_STATE_CLOSED                    1
#define NARE_HANDLE_STATE_OPEN_DCP_RUNNING          2
#define NARE_HANDLE_STATE_CLOSE_DCP_RUNNING         3
#define NARE_HANDLE_STATE_OPEN_SOCK_RUNNING         4
#define NARE_HANDLE_STATE_CLOSE_SOCK_RUNNING        5
#define NARE_HANDLE_STATE_OPEN_EDD_RUNNING          6
#define NARE_HANDLE_STATE_CLOSE_EDD_RUNNING         7
#define NARE_HANDLE_STATE_GET_MAC_EDD_RUNNING       8   /* get MAC of EDD-Channel                  */
#define NARE_HANDLE_STATE_SNIFF_OWN_IP_EDD_RUNNING  9   /* running ARP-Sniffer on OPEN-EDD-Channel */
#define NARE_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING  10
#define NARE_HANDLE_STATE_CLOSE_ERROR_DCP_RUNNING  11
#define NARE_HANDLE_STATE_OPEN                     15

/* --------------------------------------------------------------------------*/
/* For DCP Channel                                                           */
/*                                                                           */
/* For handling all DCP-Request from NARE to DCP                             */
/* --------------------------------------------------------------------------*/

typedef struct _NARE_DCP_SET_INFO_TYPE
{
    LSA_UINT16                          OptionCnt;     /* Number of Options within set    */
    /* 1 or 2                          */
    LSA_UINT16                          OptionType[2]; /* Type of Option awaited in resp. */
    /* (network-format!)               */
} NARE_DCP_SET_INFO_TYPE;

/* LowerLayer (DCP) Request management type */
typedef struct _NARE_DCP_RCTRL_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_DCP_RCTRL_PTR_TYPE;

typedef struct _NARE_DCP_RCTRL_TYPE
{
    LSA_BOOL                            InUse;     /* InUse ?                             */
    LSA_UINT16                          ThisIdx;   /* Index in RCTRL-Table of this Block  */
    NARE_DCP_RCTRL_PTR_TYPE             pNext;     /* used for queuing blocks             */
    NARE_DCP_RCTRL_PTR_TYPE             pPrev;     /* used for queuing blocks             */
    NARE_USER_RCTRL_PTR_TYPE            pRCtrl;    /* user-control block for this request */
    LSA_UINT16                          RetryCnt;  /* Retrys left. 0 if none              */
    LSA_UINT16                          NotifyCnt; /* Number of Notifys occured           */
    LSA_UINT16                          AliasUsed; /* Alias-name used. 0 = none.          */
    NARE_DCP_SET_INFO_TYPE              SetOptions;/* Infos about Set-Options used        */
} NARE_DCP_RCTRL_TYPE;

typedef struct _NARE_DCP_RCTRL_LINK_BASE_TYPE
{
    NARE_DCP_RCTRL_PTR_TYPE          pBottom;
    NARE_DCP_RCTRL_PTR_TYPE          pTop;
} NARE_DCP_RCTRL_LINK_BASE_TYPE;

typedef struct _NARE_DCP_RCTRL_LINK_BASE_TYPE  NARE_LOCAL_MEM_ATTR *    NARE_DCP_RCTRL_LINK_BASE_PTR_TYPE;

/* declaring a pointer to an array */
typedef NARE_DCP_RCTRL_TYPE                              NARE_DCP_RCTRL_ARRAY_TYPE[1];
typedef NARE_DCP_RCTRL_ARRAY_TYPE  NARE_LOCAL_MEM_ATTR * NARE_DCP_RCTRL_ARRAY_PTR_TYPE;

/* RCTRLs used with identify/set requests. We support several requests at once */

typedef struct _NARE_DCP_RCTRL_MGM_TYPE
{
    LSA_UINT32                          Cnt;       /* Number of RequestControlblocks*/
    /* set to NARE_CFG_MAX_RDCP      */
    LSA_UINT32                          Free;      /* Number of free blocks         */
    LSA_UINT32                          Used;      /* Number of used blocks         */
    NARE_DCP_RCTRL_LINK_BASE_TYPE       pFree;     /* LinkedList of free RCtrl.     */
    NARE_DCP_RCTRL_ARRAY_PTR_TYPE       pTable;    /* Pointer to Array with RCTRL   */
    /* Cnt entrys.                   */
} NARE_DCP_RCTRL_MGM_TYPE;

/* HELLO Management. HELLO will be done with own resources. */

typedef struct _NARE_DCP_HELLO_MGM_TYPE
{
    LSA_UINT32                          Status;    /* Status                        */
    NARE_DCP_LOWER_RQB_PTR_TYPE         pRQB;      /* RQB used for HELLO/HELLO_STOP */
    NARE_DCP_LOWER_MEM_PTR_TYPE         pTLV;      /* TLV memory help pointer.      */
    NARE_HDB_USER_PTR_TYPE              pHDB;      /* HDB of user if <> IDLE        */
} NARE_DCP_HELLO_MGM_TYPE;

typedef struct _NARE_DCP_HELLO_MGM_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_DCP_HELLO_MGM_PTR_TYPE;

/* Status */
#define NARE_DCP_HELLO_IDLE              0
#define NARE_DCP_HELLO_IN_PROGRESS       1

typedef struct _NARE_HDB_DCP_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_HDB_DCP_PTR_TYPE;

typedef struct _NARE_HDB_DCP_TYPE
{
    LSA_UINT16                       State;           /* handle state                  */
    LSA_HANDLE_TYPE                  ThisHandle;      /* this handle                   */
    LSA_UINT32                       TraceIdx;        /* trace index for index trace   */
    NARE_HANDLE_PARAMS_TYPE          Params;          /* general handle parameters     */
    NARE_UPPER_RQB_PTR_TYPE          pOCReq;          /* Open or Close Request running */
    NARE_RSP                         OpenResponse;    /* Response from Open if error   */
                                                      /* occured while opening.        */
    LSA_UINT16                       UserCnt;         /* Number of User-Channels using */
                                                      /* this DCP-channel.             */
    LSA_HANDLE_TYPE                  DCPHandle;       /* If Open the lower DCP handle. */
    LSA_UINT32                       PendRCtrlCnt;    /* Number of pending RCtrlB.     */
    NARE_USER_RCTRL_LINK_BASE_TYPE   PendRCtrl;       /* Pending UserCtrl-Blocks       */
    NARE_DCP_RCTRL_MGM_TYPE          DCPRCtrl;        /* Request-Control-blocks for    */
                                                      /* DCPRequests.                  */
    LSA_UINT32                       NotifyDCPCnt;    /* Number of Notify RQB within   */
                                                      /* DCP.                          */
    NARE_DCP_HELLO_MGM_TYPE          Hello;           /* Hello Management              */
                                                      /* ------------------------------*/
  LSA_UINT16                         NoOfRCtrlCnt;    /* No. of parallel RQs           */
} NARE_HDB_DCP_TYPE;

/* State: */

/* #define NARE_HANDLE_STATE_CLOSED                   0  */
/* #define NARE_HANDLE_STATE_OPEN_DCP_RUNNING         1  */
/* #define NARE_HANDLE_STATE_CLOSE_DCP_RUNNING        2  */
/* #define NARE_HANDLE_STATE_OPEN                     5  */

/* we have NARE_CFG_MAX_DCP_CHANNELS DCP-channels. */

typedef struct _NARE_HDB_DCP_INFO_TYPE
{
    LSA_UINT16                    HandleCnt;       /* Number of handles supported       */
                                                   /* set to NARE_CFG_MAX_DCP_CHANNELS  */
    LSA_UINT16                    UsedCnt;         /* currently used user-channels      */
    NARE_HDB_DCP_TYPE             HDB[NARE_CFG_MAX_DCP_CHANNELS];
                                                   /* to array of MaxHandle             */
                                                   /* NARE_HDB_USER_TYPE types.         */
} NARE_HDB_DCP_INFO_TYPE;


/* --------------------------------------------------------------------------*/
/* For EDD Channel                                                           */
/*                                                                           */
/* For handling all EDD-Request from NARE to EDD                             */
/* --------------------------------------------------------------------------*/

typedef volatile struct _NARE_EDD_ARP_SNIFFER_TYPE
{
    LSA_BOOL                            DoSniff;       /* LSA_TRUE if this RCTRL has to be*/
    /* used for sniffing.              */
    LSA_UINT8                           Mode;          /* Mode of Sniffing                */
    LSA_BOOL                            MultipleRsp;   /* Multiple Responses occured      */
    LSA_UINT16                          TimerEndCounter;/* Time we end with sniffing      */
                                                       /* ARP-Responses                   */
    NARE_UNION_IP_TYPE                  DestIP;        /* IP we resolve.                  */
    NARE_UNION_IP_TYPE                  LocalIP;       /* local IP address                */
    LSA_BOOL                            CurMACValid;   /* TRUE if CurMAC valid.         1)*/
    NARE_UNION_MAC_TYPE                 CurMAC;        /* Current MAC tested for        1)*/
                                                       /* If we know the MAC we expect as */
                                                       /* MAC response this value is setup*/
                                                       /* with this MAC. If we don know it*/
                                                       /* it will get the MAC of the first*/
                                                       /* response.                       */
                                                       /* 1) only used with mode          */
                                                       /* NARE_EDD_ARP_SNIFF_MODE_MULTIPLE*/
} NARE_EDD_ARP_SNIFFER_TYPE;

/* Mode: */

#define NARE_EDD_ARP_SNIFF_MODE_MULTIPLE  0   /* test if more then one uses Dest IP */
#define NARE_EDD_ARP_SNIFF_MODE_SINGLE    1   /* test if at least one uses Dest IP  */

/* LowerLayer (EDD) Request management type */
typedef struct _NARE_EDD_RCTRL_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_EDD_RCTRL_PTR_TYPE;

typedef struct _NARE_HDB_EDD_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_HDB_EDD_PTR_TYPE;
typedef struct _NARE_EDD_RCTRL_TYPE
{
    volatile LSA_BOOL                   InUse;     /* InUse ?                             */
    LSA_UINT16                          ThisIdx;   /* Index in RCTRL-Table of this Block  */
    NARE_EDD_RCTRL_PTR_TYPE             pNext;     /* used for queuing blocks             */
    NARE_EDD_RCTRL_PTR_TYPE             pPrev;     /* used for queuing blocks             */
    NARE_EDD_LOWER_RQB_PTR_TYPE         pARP_RQB;  /* RQB for sending next ARP over EDD   */
    NARE_HDB_EDD_PTR_TYPE               pHDBEDD;   /* associated HDB                      */
    NARE_USER_RCTRL_PTR_TYPE            pRCtrl;    /* user-control block for this request */
    LSA_UINT32                          BurstCnt;  /* Number of ARP-Frames (burst) to be  */
                                                   /* send.                               */
    LSA_UINT32                          SentARPs;
    LSA_BOOL                            ARP_IO_DONE;
    LSA_UINT32                          RandomTime; /* time calculated on the least sig.  */
                                                    /* bytes of the MAC 0..1 sec in msec  */
    LSA_UINT16                          ResendTime;
    NARE_EDD_ARP_SNIFFER_TYPE           Sniffer;   /* ARP-Sniffer.                        */
#ifdef NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN
    NARE_EDD_LOWER_MEM_PTR_TYPE         pTxFrame;  /* Pointer to Tx-Frame memory.         */
                                                   /* Room for NARE_ARP_TX_FRAME_LEN      */
                                                   /* bytes.                              */
#endif
} NARE_EDD_RCTRL_TYPE;

typedef struct _NARE_EDD_RCTRL_LINK_BASE_TYPE
{
    NARE_EDD_RCTRL_PTR_TYPE          pBottom;
    NARE_EDD_RCTRL_PTR_TYPE          pTop;
} NARE_EDD_RCTRL_LINK_BASE_TYPE;

typedef struct _NARE_EDD_RCTRL_LINK_BASE_TYPE  NARE_LOCAL_MEM_ATTR *    NARE_EDD_RCTRL_LINK_BASE_PTR_TYPE;

/* declaring a pointer to an array */
typedef NARE_EDD_RCTRL_TYPE                              NARE_EDD_RCTRL_ARRAY_TYPE[1];
typedef NARE_EDD_RCTRL_ARRAY_TYPE  NARE_LOCAL_MEM_ATTR * NARE_EDD_RCTRL_ARRAY_PTR_TYPE;

typedef struct _NARE_EDD_RCTRL_MGM_TYPE
{
    LSA_UINT32                          Cnt;       /* Number of RequestControlblocks*/
    /* set to NARE_CFG_MAX_REDD      */
    LSA_UINT32                          Free;      /* Number of free blocks         */
    LSA_UINT32                          Used;      /* Number of used blocks         */
    NARE_EDD_RCTRL_LINK_BASE_TYPE       pFree;     /* LinkedList of free RCtrl.     */
    NARE_EDD_RCTRL_ARRAY_PTR_TYPE       pTable;    /* Pointer to Array with RCTRL   */
    /* Cnt entrys.                   */
} NARE_EDD_RCTRL_MGM_TYPE;


typedef struct _NARE_HDB_EDD_TYPE
{
    LSA_UINT16                       State;           /* handle state                  */
    LSA_HANDLE_TYPE                  ThisHandle;      /* this handle                   */
    LSA_UINT32                       TraceIdx;        /* trace index for index trace   */
    NARE_HANDLE_PARAMS_TYPE          Params;          /* general handle parameters     */
    NARE_UPPER_RQB_PTR_TYPE          pOCReq;          /* Open or Close Request running */
    NARE_RSP                         OpenResponse;    /* Response from Open if error   */
                                                      /* occured while opening.        */
    LSA_UINT16                       UserCnt;         /* Number of User-Channels using */
                                                      /* this EDD-channel.             */
    LSA_HANDLE_TYPE                  EDDHandle;       /* If Open the lower EDD handle. */
    LSA_UINT32                       PendRCtrlCnt;    /* Number of pending RCtrlB.     */
    NARE_USER_RCTRL_LINK_BASE_TYPE   PendRCtrl;       /* Pending UserCtrl-Blocks       */
    NARE_MAC_TYPE                    MACAddr;         /* MAC Address of NIC            */
    LSA_UINT32                       ARPSnifferCnt;   /* Number of RCTRL Sniffing      */
    NARE_EDD_RCTRL_MGM_TYPE          EDDRCtrl;        /* Request-Control-blocks for    */
                                                      /* EDDRequests.                  */
    NARE_USER_RCTRL_TYPE             UserRCtrl;       /* User-RCTRL used for ARP-Sniff */
                                                      /* on OpenChannel of EDD.        */
                                                      /* ------------------------------*/
    LSA_UINT16                       SizeOfEDDRCtrlQue;
} NARE_HDB_EDD_TYPE;

/* State: */

/* #define NARE_HANDLE_STATE_CLOSED                   0  */
/* #define NARE_HANDLE_STATE_OPEN_EDD_RUNNING         1  */
/* #define NARE_HANDLE_STATE_CLOSE_EDD_RUNNING        2  */
/* #define NARE_HANDLE_STATE_OPEN                     5  */

/* we have NARE_CFG_MAX_EDD_CHANNELS EDD-channels. */

typedef struct _NARE_ARP_TIMER_TYPE
{
    LSA_UINT16                    ID;      /* NARE Timer-ID for ARP-Timer       */
    LSA_UINT16                    UserCnt; /* Number of Users of ARP-Timer      */
                                           /* if >0 timer is running.           */
    LSA_UINT16                    Counter; /* Actual Timer Counter. Incremented */
                                           /* on each timer event.              */
} NARE_ARP_TIMER_TYPE;

typedef struct _NARE_HDB_EDD_INFO_TYPE
{
    LSA_UINT16                    HandleCnt;       /* Number of handles supported       */
                                                   /* set to NARE_CFG_MAX_EDD_CHANNELS  */
    LSA_UINT16                    UsedCnt;         /* currently used user-channels      */
    NARE_ARP_TIMER_TYPE           ARPTimer;        /* ARP-Timer management              */
    NARE_HDB_EDD_TYPE             HDB[NARE_CFG_MAX_EDD_CHANNELS];
                                                   /* to array of MaxHandle             */
                                                   /* NARE_EDD_USER_TYPE types.         */
} NARE_HDB_EDD_INFO_TYPE;

typedef struct _NARE_HDB_EDD_INFO_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_HDB_EDD_INFO_PTR_TYPE;

/* --------------------------------------------------------------------------*/
/* For UserChannel                                                           */
/*                                                                           */
/* For handling all User-Requests to NARE. We have a queue which holds all   */
/* user-request till resources are free for further processing (UserReq).    */
/*                                                                           */
/* A resource is a User Request Control Block (UserRCtrl). This is a         */
/* block holding all information about the actual request (i.g. the actual   */
/* state or the pointer to the RQB). We only have a limit number of blocks   */
/* (configurable). So if no more blocks are free, we must wait and queue the */
/* RQB.                                                                      */
/*                                                                           */
/* Every UserChannel uses one assoziated DCP-channel selected by NICId in    */
/* CDB (detail-ptr). Every UserChannel uses one SOCK-Channel (currently      */
/* there is only one SOCK-Channel for all User-Channels, because the SOCK    */
/* Channel is NIC independent)                                               */
/* --------------------------------------------------------------------------*/

/* declaring a pointer to an array */
typedef NARE_USER_RCTRL_TYPE                              NARE_USER_RCTRL_ARRAY_TYPE[1];
typedef NARE_USER_RCTRL_ARRAY_TYPE  NARE_LOCAL_MEM_ATTR * NARE_USER_RCTRL_ARRAY_PTR_TYPE;

typedef struct _NARE_USER_RCTRL_MGM_TYPE
{
    LSA_UINT32                          Cnt;       /* Number of RequestControlblocks*/
                                                   /* set to NARE_CFG_MAX_RUSER     */
    LSA_UINT32                          Free;      /* Number of free blocks         */
    LSA_UINT32                          Used;      /* Number of used blocks         */
    NARE_USER_RCTRL_LINK_BASE_TYPE      pFree;     /* LinkedList of free RCtrl.     */
    NARE_USER_RCTRL_ARRAY_PTR_TYPE      pTable;    /* Table of RequestControlblocks */
                                                   /* Cnt entrys.                   */
} NARE_USER_RCTRL_MGM_TYPE;


typedef struct _NARE_RQB_LINK_BASE_TYPE
{
    NARE_UPPER_RQB_PTR_TYPE          pBottom;
    NARE_UPPER_RQB_PTR_TYPE          pTop;
} NARE_RQB_LINK_BASE_TYPE;


typedef struct _NARE_USR_HELLO_MGM_TYPE
{
    LSA_UINT32                          Status;    /* Status                        */
    NARE_UPPER_RQB_PTR_TYPE             pRQB;      /* RQB used for HELLO/HELLO_STOP */
} NARE_USR_HELLO_MGM_TYPE;

typedef struct _NARE_USR_HELLO_MGM_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_USR_HELLO_MGM_PTR_TYPE;

/* Status */
#define NARE_USR_HELLO_IDLE              0
#define NARE_USR_HELLO_IN_PROGRESS       1   /* request in progress */

typedef struct _NARE_HDB_USER_TYPE
{
    LSA_UINT16                       State;           /* handle state                  */
    LSA_HANDLE_TYPE                  ThisHandle;      /* this handle                   */
    NARE_HANDLE_PARAMS_TYPE          Params;          /* general handle parameters     */
    NARE_HDB_DCP_PTR_TYPE            pHDBDcp;         /* Pointer to assoziated DCP-HDB */
    NARE_HDB_EDD_PTR_TYPE            pHDBEdd;         /* Pointer to assoziated EDD-HDB */
#ifndef NARE_CFG_NO_SOCK
    NARE_HDB_SOCK_PTR_TYPE           pHDBSock;        /* Pointer to assoziated SOCK-HDB*/
#endif
    LSA_UINT32                       TraceIdx;        /* trace index for index trace   */
                                                      /*-------------------------------*/
                                                      /* identfy/set requests:         */
    LSA_UINT32                       UserReqPendCnt;  /* Number of User-Requests       */
                                                      /* currently pending within      */
                                                      /* UserReq-queue (only ident/set)*/
    NARE_RQB_LINK_BASE_TYPE          UserReq;         /* user-request-queue with       */
                                                      /* requests not serviced yet     */
                                                      /* because no free RCtrl-blocks  */
    NARE_USER_RCTRL_MGM_TYPE         UserRCtrl;       /* User Request Control block mgm*/
    /*-------------------------------*/
    NARE_RQB_LINK_BASE_TYPE          NotifyReq;       /* queue with notify-requests    */
    /*-------------------------------*/
    NARE_USR_HELLO_MGM_TYPE          Hello;           /* HELLO management              */
    /*-------------------------------*/

} NARE_HDB_USER_TYPE;

/* State: */

/* #define NARE_HANDLE_STATE_CLOSED                   0  */
/* #define NARE_HANDLE_STATE_OPEN                     5  */


/* we have NARE_CFG_MAX_USER_CHANNELS user-channels. */

typedef struct _NARE_HDB_USER_INFO_TYPE
{
    LSA_UINT16                    HandleCnt;       /* Number of handles supported        */
                                                   /* set to NARE_CFG_MAX_USER_CHANNELS  */
    LSA_UINT16                    UsedCnt;         /* currently used user-channels       */
    NARE_HDB_USER_TYPE            HDB[NARE_CFG_MAX_USER_CHANNELS];
                                                   /* to array of MaxHandle              */
                                                   /* NARE_HDB_USER_TYPE types.          */
} NARE_HDB_USER_INFO_TYPE;


/* --------------------------------------------------------------------------*/
/* global NARE-management (handle independend)                               */
/* --------------------------------------------------------------------------*/

typedef struct _NARE_NDB_TYPE
{
    NARE_TIMER_INFO_TYPE     Timer[NARE_CFG_MAX_TIMER];   /* Timermanagment      */
    LSA_RQB_LOCAL_ERROR_TYPE LLError;                     /* last LL-Error       */
} NARE_NDB_TYPE;

typedef struct _NARE_NDB_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_NDB_PTR_TYPE;


/* --------------------------------------------------------------------------*/
/* global NARE-management anchor                                             */
/* --------------------------------------------------------------------------*/

typedef struct _NARE_MGM_TYPE  NARE_LOCAL_MEM_ATTR *   NARE_MGM_PTR_TYPE;

typedef struct _NARE_MGM_TYPE
{
    NARE_NDB_TYPE                               NDB;        /* Global Mgm.        */
    #ifndef NARE_CFG_NO_SOCK
    NARE_HDB_SOCK_INFO_TYPE                     HSock;      /* SOCK-Handle Mgm    */
    #endif
    NARE_HDB_EDD_INFO_TYPE                      HEdd;       /* EDD -Handle Mgm    */
    NARE_HDB_DCP_INFO_TYPE                      HDcp;       /* DCP -Handle Mgm    */
    NARE_HDB_USER_INFO_TYPE                     HUser;      /* User-Handle Mgm    */
} NARE_MGM_TYPE;

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

LSA_EXTERN NARE_MGM_PTR_TYPE    g_pNAREInfo;   /* global NARE-INFO-Structure */

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

#define NARE_MGM_GET_NDB_PTR()     (&(g_pNAREInfo->NDB))
#define NARE_MGM_GET_HSOCK_PTR()   (&(g_pNAREInfo->HSock))
#define NARE_MGM_GET_HDCP_PTR()    (&(g_pNAREInfo->HDcp))
#define NARE_MGM_GET_HUSER_PTR()   (&(g_pNAREInfo->HUser))
#define NARE_MGM_GET_HEDD_PTR()    (&(g_pNAREInfo->HEdd))


/*===========================================================================*/
/* Handle-handling                                                           */
/*===========================================================================*/

/* Coding of Handle-Variable:                                        */
/* we have four types of handle. A SOCK, DCP, EDD and a USER-Handle. */

#define NARE_HANDLE_TYPE_MSK   0xC0     /* Bit 6..7 selects the Type of handle  */
#define NARE_HANDLE_VALUE_MSK  0x3F     /* Bit 0..5 the handle value            */

#define NARE_HANDLE_TYPE_USER  0x00
#define NARE_HANDLE_TYPE_DCP   0x40
#define NARE_HANDLE_TYPE_SOCK  0x80
#define NARE_HANDLE_TYPE_EDD   0xC0

#define NARE_HANDLE_GET_TYPE(Handle)    ((Handle) & NARE_HANDLE_TYPE_MSK)

#define NARE_HANDLE_IDX_USER(Handle)    ((Handle) & NARE_HANDLE_VALUE_MSK)
#define NARE_HANDLE_IDX_SOCK(Handle)    ((Handle) & NARE_HANDLE_VALUE_MSK)
#define NARE_HANDLE_IDX_DCP(Handle)     ((Handle) & NARE_HANDLE_VALUE_MSK)
#define NARE_HANDLE_IDX_EDD(Handle)     ((Handle) & NARE_HANDLE_VALUE_MSK)

#define NARE_HANDLE_IS_USER(Handle)     ((NARE_HANDLE_GET_TYPE(Handle) == NARE_HANDLE_TYPE_USER) ? LSA_TRUE:LSA_FALSE)
#define NARE_HANDLE_IS_SOCK(Handle)     ((NARE_HANDLE_GET_TYPE(Handle) == NARE_HANDLE_TYPE_SOCK) ? LSA_TRUE:LSA_FALSE)
#define NARE_HANDLE_IS_DCP(Handle)      ((NARE_HANDLE_GET_TYPE(Handle) == NARE_HANDLE_TYPE_DCP)  ? LSA_TRUE:LSA_FALSE)
#define NARE_HANDLE_IS_EDD(Handle)      ((NARE_HANDLE_GET_TYPE(Handle) == NARE_HANDLE_TYPE_EDD)  ? LSA_TRUE:LSA_FALSE)


/*===========================================================================*/
/*                           Access to network bytes                         */
/*===========================================================================*/

#ifndef NARE_CFG_BIG_ENDIAN
/* flips a byte pattern of 0102 to 0201 */
#define NARE_FLIP16(C)      (LSA_UINT16)(((((LSA_UINT16)(C))>>8)&0xFF)|((((LSA_UINT16)(C))<<8)&0xFF00))
/* flips a byte pattern of 01020304 to 04030201 */
#define NARE_FLIP32(C)      ((LSA_UINT32)(((((LSA_UINT32)(C))>>24)&0xFF)|((((LSA_UINT32)(C))>>8)&0xFF00)|((((LSA_UINT32)(C))<<8)&0xFF0000)|((((LSA_UINT32)(C))<<24)&0xFF000000)))
#else
#define NARE_FLIP16(C)      (C)
#define NARE_FLIP32(C)      (C)
#endif

#ifndef NARE_NTOHS
#define NARE_NTOHS(Value) ( NARE_FLIP16(Value) )/* Network TO Host Short */
#endif

#ifndef NARE_NTOHL
#define NARE_NTOHL(Value) ( NARE_FLIP32(Value) )/* Network TO Host Long  */
#endif

#ifndef NARE_HTONS
#define NARE_HTONS(Value) ( NARE_FLIP16(Value) )/* Host To Network Short */
#endif

#ifndef NARE_NTONL
#define NARE_HTONL(Value) ( NARE_FLIP32(Value) )/* Host TO Network Long  */
#endif

/* copy MAC-Addresses    */

#ifndef NARE_COPY_MAC_ADDR_UR_LDR
#define NARE_COPY_MAC_ADDR_UR_LDR(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR *)(_DstPtr))+i) = *(((LSA_UINT8 NARE_DCP_LOWER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_UR_LDM
#define NARE_COPY_MAC_ADDR_UR_LDM(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR *)(_DstPtr))+i) = *(((LSA_UINT8 NARE_DCP_LOWER_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_ST_LEM
#define NARE_COPY_MAC_ADDR_ST_LEM(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 *)(_DstPtr))+i) = *(((LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_LOC_LOC
#define NARE_COPY_MAC_ADDR_LOC_LOC(_DstPtr, _SrcPtr)                \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_LOC_LER
#define NARE_COPY_MAC_ADDR_LOC_LER(_DstPtr, _SrcPtr)                \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_EDD_LOWER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_LOC_UR
#define NARE_COPY_MAC_ADDR_LOC_UR(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_UR_LOC
#define NARE_COPY_MAC_ADDR_UR_LOC(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_LEM_GLB
#define NARE_COPY_MAC_ADDR_LEM_GLB(_DstPtr, _SrcPtr)                \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 *)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_MAC_ADDR_LEM_LOC
#define NARE_COPY_MAC_ADDR_LEM_LOC(_DstPtr, _SrcPtr)                \
{int i; for (i = 0; i < NARE_MAC_ADDR_SIZE; i++)    \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif


/* copy IP-Addresses     */

#ifndef NARE_COPY_IP_ADDR_UR_LDR
#define NARE_COPY_IP_ADDR_UR_LDR(_DstPtr, _SrcPtr)              \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_DCP_LOWER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_UR_LDM
#define NARE_COPY_IP_ADDR_UR_LDM(_DstPtr, _SrcPtr)              \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_DCP_LOWER_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_LOC_UR
#define NARE_COPY_IP_ADDR_LOC_UR(_DstPtr, _SrcPtr)              \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_UR_LOC
#define NARE_COPY_IP_ADDR_UR_LOC(_DstPtr, _SrcPtr)              \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_LOC_LOC
#define NARE_COPY_IP_ADDR_LOC_LOC(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_ST_LEM
#define NARE_COPY_IP_ADDR_ST_LEM(_DstPtr, _SrcPtr)              \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 *)(_DstPtr))+i) = *(((LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_LEM_LOC
#define NARE_COPY_IP_ADDR_LEM_LOC(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_LOCAL_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_LEM_SYS
#define NARE_COPY_IP_ADDR_LEM_SYS(_DstPtr, _SrcPtr)             \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_SYSTEM_MEM_ATTR*)(_SrcPtr))+i);}
#endif

#ifndef NARE_COPY_IP_ADDR_UR_UR
#define NARE_COPY_IP_ADDR_UR_UR(_DstPtr, _SrcPtr)               \
{int i; for (i = 0; i < NARE_IP_ADDR_SIZE; i++) \
*(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))+i) = *(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))+i);}
#endif

/* compares IP-Addresses */

#ifndef NARE_IP_ADDR_EQUAL_UR_UR
#define NARE_IP_ADDR_EQUAL_UR_UR(_DstPtr, _SrcPtr)              \
((((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))[0] ==  ((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))[0]) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))[1] ==  ((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))[1]) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))[2] ==  ((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))[2]) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_DstPtr))[3] ==  ((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_SrcPtr))[3]))
#endif

#ifndef NARE_IP_ADDR_ZERO_UR
#define NARE_IP_ADDR_ZERO_UR(_Ptr)              \
((((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_Ptr))[0] ==  0) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_Ptr))[1] ==  0) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_Ptr))[2] ==  0) && \
(((LSA_UINT8 NARE_UPPER_RQB_ATTR*)(_Ptr))[3] ==  0))
#endif

/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  NARE_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define NARE_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block)   \
{                                                       \
    NARE_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);        \
    NARE_RQB_SET_PREV_RQB_PTR(_Block, _Top);            \
                                                        \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))         \
    {                                                   \
        _Top    = _Block;                               \
        _Bottom = _Block;                               \
    }                                                   \
    else                                                \
    {                                                   \
        NARE_RQB_SET_NEXT_RQB_PTR(_Top, _Block);        \
        _Top        = _Block;                           \
    }                                                   \
}


/*=============================================================================
 * function name:  NARE_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define NARE_RQB_PUT_BLOCK_BOTTOM(_Bottom,_Top,_Block)  \
{                                                       \
NARE_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);             \
NARE_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);            \
                                                        \
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
{                                                       \
    _Bottom = _Block;                                   \
    _Top    = _Block;                                   \
}                                                       \
    else                                                \
{                                                       \
    NARE_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);         \
    _Bottom         = _Block;                           \
}                                                       \
}

/*=============================================================================
 * function name:   NARE_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block)   \
{                                                       \
    _Block = _Top;                                      \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )     \
    {                                                   \
        _Top = NARE_RQB_GET_PREV_RQB_PTR(_Top);         \
                                                        \
    if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )      \
        _Bottom = LSA_NULL;                             \
    else                                                \
        NARE_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);      \
    }                                                   \
}

/*=============================================================================
 * function name:   NARE_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)\
{                                                       \
    _Block = _Bottom;                                   \
                                                        \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )  \
    {                                                   \
        _Bottom = NARE_RQB_GET_NEXT_RQB_PTR((_Bottom)); \
                                                        \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))\
            _Top = LSA_NULL;                            \
        else                                            \
            NARE_RQB_SET_PREV_RQB_PTR((_Bottom),LSA_NULL);\
    }                                                   \
}


/*=============================================================================
 * function name:   NARE_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)      \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
    (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
    (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )             \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
        (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))          \
        _Block = NARE_RQB_GET_NEXT_RQB_PTR(_Block);             \
                                                                \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                NARE_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);\
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    NARE_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);\
                }                                               \
                else                                            \
                {                                               \
                    NARE_RQB_SET_NEXT_RQB_PTR(NARE_RQB_GET_PREV_RQB_PTR(_Block),NARE_RQB_GET_NEXT_RQB_PTR(_Block)); \
                    NARE_RQB_SET_PREV_RQB_PTR(NARE_RQB_GET_NEXT_RQB_PTR(_Block),NARE_RQB_GET_PREV_RQB_PTR(_Block)); \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   NARE_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)   \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = NARE_RQB_GET_NEXT_RQB_PTR(_Block);             \
                                                                    \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}

/*=============================================================================
 * function name:   NARE_RQB_SEARCH_QUEUE_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      NARE_UPPER_RQB_PTR_TYPE        pRQB
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

#define NARE_RQB_SEARCH_QUEUE_HANDLE(pRQB,Handle)           \
{                                                           \
    while   ((! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&  \
            ( NARE_RQB_GET_HANDLE(pRQB) != Handle ))        \
    {                                                       \
        pRQB = NARE_RQB_GET_NEXT_RQB_PTR(pRQB);             \
    }                                                       \
}

/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  NARE_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define NARE_PUT_BLOCK_TOP(_Bottom, _Top, _Block) \
{                                                 \
    _Block->pNext = LSA_NULL;                     \
    _Block->pPrev = _Top;                         \
    if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))   \
    {                                             \
        _Top    = _Block;                         \
        _Bottom = _Block;                         \
    }                                             \
    else                                          \
    {                                             \
        _Top->pNext = _Block;                     \
        _Top        = _Block;                     \
    }                                             \
}

/*=============================================================================
 * function name:  NARE_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define NARE_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
    _Block->pNext = _Bottom;                                    \
    _Block->pPrev = LSA_NULL;                                   \
    if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
    {                                                           \
        _Bottom = _Block;                                       \
        _Top    = _Block;                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        _Bottom->pPrev  = _Block;                               \
        _Bottom         = _Block;                               \
    }                                                           \
}

/*=============================================================================
 * function name:   NARE_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/
#define NARE_REM_BLOCK_TOP(_Bottom, _Top, _Block)               \
{                                                               \
    _Block = _Top;                                              \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
    {                                                           \
        _Top = _Top->pPrev;                                     \
        if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
            _Bottom = LSA_NULL;                                 \
        else                                                    \
            _Top->pNext = LSA_NULL;                             \
    }                                                           \
}                                                               \

/*=============================================================================
 * function name:   NARE_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/
#define NARE_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
    _Block = _Bottom;                                           \
    if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
    {                                                           \
        _Bottom = _Bottom->pNext;                               \
        if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
            _Top = LSA_NULL;                                    \
        else                                                    \
            _Bottom->pPrev = LSA_NULL;                          \
    }                                                           \
}                                                               \


/*=============================================================================
 * function name:   NARE_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_REM_BLOCK(_Bottom, _Top, _Handle, _Block)          \
{                                                               \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
        (LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
        _Block = LSA_NULL;                                      \
    else                                                        \
    {                                                           \
        _Block = _Bottom;                                       \
        while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
              (! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
            _Block = _Block->pNext;                             \
        if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
            _Block = LSA_NULL;                                  \
        else                                                    \
        {                                                       \
            if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
            {                                                   \
                NARE_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
            }                                                   \
            else                                                \
            {                                                   \
                if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
                {                                               \
                    NARE_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
                }                                               \
                else                                            \
                {                                               \
                    _Block->pPrev->pNext = _Block->pNext;       \
                    _Block->pNext->pPrev = _Block->pPrev;       \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
}

/*=============================================================================
 * function name:   NARE_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define NARE_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)       \
{                                                                   \
    if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
        (LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
        ((_Handle) == 0) )                                          \
        _Block = LSA_NULL;                                          \
    else                                                            \
    {                                                               \
        _Block = _Bottom;                                           \
        while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
               (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
            _Block = (_Block)->pNext;                               \
        if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
            _Block = LSA_NULL;                                      \
    }                                                               \
}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* nare_glb.c */

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NAREFatalError(
    NARE_FATAL_ERROR_TYPE  Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line);

LSA_EXTERN LSA_VOID NARERQBCopyLLErrorLocal(
    LSA_RQB_LOCAL_ERROR_TYPE  NARE_UPPER_RQB_ATTR * pLLError);

LSA_EXTERN LSA_VOID NARERQBSetError(
    NARE_UPPER_RQB_PTR_TYPE   pRQB,
    LSA_UINT16                LSACompID,
    LSA_OPCODE_TYPE           Opcode,
    LSA_UINT16                Response);

/* AP01539314 needs a typedef */
typedef  LSA_VOID  (NareUpperCbf)(NARE_UPPER_RQB_PTR_TYPE pRQB);


LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARECallCbf(
//    LSA_VOID LSA_FCT_PTR(NARE_UPPER_OUT_FCT_ATTR, Cbf)(NARE_UPPER_RQB_PTR_TYPE pRQB),
    NareUpperCbf                 Cbf,
    NARE_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE             pSys);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NAREUserRequestFinish(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_UPPER_RQB_PTR_TYPE     pRQB,
    NARE_RSP                    Status);

LSA_EXTERN NARE_HDB_USER_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetUserHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN NARE_HDB_DCP_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetDcpHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

LSA_EXTERN NARE_HDB_EDD_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetEddHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);

#ifndef NARE_CFG_NO_SOCK
LSA_EXTERN NARE_HDB_SOCK_PTR_TYPE  NARE_LOCAL_FCT_ATTR NAREGetSockHDBFromHandle(
    LSA_HANDLE_TYPE     Handle);
#endif

/* nare_usr.c */

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_UserChannelDone(
    NARE_HDB_USER_PTR_TYPE       pHDB,
    NARE_RSP                     Response);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_DCPChannelDone(
    NARE_HDB_DCP_PTR_TYPE       pHDB,
    NARE_RSP                    Response);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserCheckPending(
    NARE_HDB_USER_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_UserReady(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrlUser,
    NARE_RSP                    Response);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserStateMachine(
    NARE_USER_RCTRL_PTR_TYPE         pRCtrlUser,
    NARE_RSP                         Response,
    LSA_BOOL                         CheckPending);

LSA_EXTERN LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_CheckIpSuiteIdentically(
    NARE_CONFIG_TYPE          NARE_UPPER_MEM_ATTR * pConfig,
    NARE_RESOLVE_TYPE         NARE_UPPER_MEM_ATTR * pResolve);

/* nare_dcp.c */

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);


LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN NARE_RSP  NARE_LOCAL_FCT_ATTR NARE_DCPTrigger(
    NARE_HDB_DCP_PTR_TYPE     pHDB,
    NARE_DCP_RCTRL_PTR_TYPE   pRCtrl);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPCheckAndStartPending(
    NARE_HDB_DCP_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPApplyRCtrl(
    NARE_HDB_DCP_PTR_TYPE     pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_DCP_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser,
    LSA_SYS_PTR_TYPE                           pSys);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPReleaseHDB(
    NARE_HDB_DCP_PTR_TYPE pHDBUser);

LSA_EXTERN NARE_HDB_DCP_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_DCPGetHandleByNIC(
    LSA_UINT16                   NICId);

LSA_EXTERN NARE_DCP_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_DCPGetFreeRCTRL(
    NARE_HDB_DCP_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPReleaseRCTRL(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPIdentify(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl,
    NARE_UPPER_MEM_PTR_TYPE      pStationName,
    LSA_UINT8                    StationNameLen);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPIdentifyRQBSetup(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_LOWER_RQB_PTR_TYPE  pRQB,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT16                   StationNameLen,
    LSA_BOOL                     Alias,
    LSA_BOOL                     Notify,
    LSA_UINT32                   NotifyID,
    LSA_UINT16                   Tolerance);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPSet(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_DCP_RCTRL_PTR_TYPE      pRCtrl,
    NARE_UPPER_RQB_MAC_PTR_TYPE  pMAC,
    NARE_UPPER_RQB_IP_PTR_TYPE   pIPAddr,
    NARE_UPPER_RQB_IP_PTR_TYPE   pSubnetMask,
    NARE_UPPER_RQB_IP_PTR_TYPE   pDefRouter,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT32                   StationNameLen,
    LSA_BOOL                     SetIPRemanent);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_DCPRemoveRCtrl(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_USER_RCTRL_PTR_TYPE     pRCtrlUser);

LSA_EXTERN LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_DCPCheckNICUsed(
    LSA_UINT16 NICId);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPHello(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_HDB_USER_PTR_TYPE       pHDBUser,
    LSA_UINT32                   SendInterval,
    LSA_UINT32                   SendCnt,
    LSA_UINT32                   Delay,
    NARE_UPPER_MEM_PTR_TYPE      pTLV,
    LSA_UINT32                   OptCnt);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_DCPHelloStop(
    NARE_HDB_DCP_PTR_TYPE        pHDB,
    NARE_HDB_USER_PTR_TYPE       pHDBUser);

/* nare_edd.c */

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDOpenChannel(
    LSA_HANDLE_TYPE              NAREHandle,
    LSA_SYS_PATH_TYPE            SysPath,
    LSA_SYS_PTR_TYPE             pSys);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDCloseChannel(
    LSA_HANDLE_TYPE              EDDHandle,
    LSA_SYS_PTR_TYPE             pSys);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDReleaseHDB(
    NARE_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN NARE_HDB_EDD_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_EDDGetHandleByNIC(
    LSA_UINT16                   NICId);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_EDD_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBEdd,
    LSA_SYS_PTR_TYPE             pSys);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDSendARP(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_EDD_RCTRL_PTR_TYPE      pRCtrl,
    NARE_LOCAL_MEM_IP_PTR_TYPE   pIPAddr,
    NARE_LOCAL_MEM_IP_PTR_TYPE   pLocalIPAddr);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDApplyRCtrl(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_USER_RCTRL_PTR_TYPE     pRCtrlUser);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDCheckAndStartPending(
    NARE_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDTimerEventARP(LSA_VOID);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDGetMACAddr(
    NARE_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDTimerCB(
    NARE_INTERNAL_REQUEST_PTR_TYPE   pIntRQB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDRemoveRCtrl(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_USER_RCTRL_PTR_TYPE     pRCtrlUser);

LSA_EXTERN LSA_BOOL NARE_LOCAL_FCT_ATTR NARE_EDDCheckNICUsed(
    LSA_UINT16                   NICId);

LSA_EXTERN NARE_EDD_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_EDDGetFreeRCTRL(
    NARE_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_EDDStartSniffer(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_EDD_RCTRL_PTR_TYPE      pRCtrl,
    LSA_UINT8                    Mode);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDChannelDone(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_RSP                     Response);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARE_EDDAbortSniffing(
    NARE_HDB_EDD_PTR_TYPE        pHDB);

LSA_EXTERN LSA_BOOL  NARE_LOCAL_FCT_ATTR NARE_EDDFindAndFinishSniff(
    NARE_HDB_EDD_PTR_TYPE        pHDB,
    NARE_USER_RCTRL_PTR_TYPE     pRCtrl);


#ifndef NARE_CFG_NO_SOCK
/* nare_sock.c */
LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKOpenChannel(
    LSA_HANDLE_TYPE       NAREHandle,
    LSA_SYS_PATH_TYPE     SysPath,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKCloseChannel(
    LSA_HANDLE_TYPE       DCPHandle,
    LSA_SYS_PTR_TYPE      pSys);

LSA_EXTERN NARE_RSP  NARE_LOCAL_FCT_ATTR NARE_SOCKTrigger(
    NARE_HDB_SOCK_PTR_TYPE    pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE  pRCtrl);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKCheckAndStartPending(
    NARE_HDB_SOCK_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKApplyRCtrl(
    NARE_HDB_SOCK_PTR_TYPE    pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_SOCK_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKReleaseHDB(
    NARE_HDB_SOCK_PTR_TYPE pHDBUser);

LSA_EXTERN NARE_HDB_SOCK_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_SOCKGetHandleByNIC(
    LSA_UINT16 NICId);

LSA_EXTERN NARE_SOCK_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_SOCKGetFreeRCTRL(
    NARE_HDB_SOCK_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKReleaseRCTRL(
    NARE_HDB_SOCK_PTR_TYPE      pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE    pRCtrl);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_SOCKRemoveRCtrl(
    NARE_HDB_SOCK_PTR_TYPE    pHDB,
    NARE_USER_RCTRL_PTR_TYPE  pRCtrlUser);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_SOCKGetHostByName(
    NARE_HDB_SOCK_PTR_TYPE       pHDB,
    NARE_SOCK_RCTRL_PTR_TYPE     pRCtrl,
    NARE_UPPER_MEM_U8_PTR_TYPE   pStationName,
    LSA_UINT32                   StationNameLen);

#endif

/* nare_sys.c */

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARE_UserGetFreeHDB(
    LSA_UINT16                                 NoOfParallelRQBs,
    NARE_HDB_USER_PTR_TYPE NARE_LOCAL_MEM_ATTR *pHDBUser);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserReleaseHDB(
    NARE_HDB_USER_PTR_TYPE pHDBUser);

LSA_EXTERN NARE_USER_RCTRL_PTR_TYPE NARE_LOCAL_FCT_ATTR NARE_UserGetFreeRCTRL(
    NARE_HDB_USER_PTR_TYPE pHDB);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_UserReleaseRCTRL(
    NARE_HDB_USER_PTR_TYPE      pHDB,
    NARE_USER_RCTRL_PTR_TYPE    pRCtrl);

LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARE_EDDReleaseRCTRL(
    NARE_HDB_EDD_PTR_TYPE      pHDB,
    NARE_EDD_RCTRL_PTR_TYPE    pRCtrl);

/* nare_tim.c */
typedef  LSA_VOID  (NareTimerCbf)(NARE_INTERNAL_REQUEST_PTR_TYPE pIntRQB);

LSA_EXTERN NARE_RSP  NARE_LOCAL_FCT_ATTR NARETimerAlloc(
    LSA_UINT16                      TimerType,
    LSA_UINT16                      TimerBase,
    LSA_UINT32                      UserID,
    NareTimerCbf                    Cbf,
    NARE_LOCAL_MEM_U16_PTR_TYPE     pNARETimerID);

LSA_EXTERN NARE_RSP NARE_LOCAL_FCT_ATTR NARETimerFree(LSA_UINT16 NARETimerID);
LSA_EXTERN LSA_VOID NARE_LOCAL_FCT_ATTR NARETimerMgmSetup(LSA_VOID);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerStop(
    LSA_UINT16 NARETimerID);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerStart(
    LSA_UINT16 NARETimerID,
    LSA_UINT16 Time);

LSA_EXTERN LSA_VOID  NARE_LOCAL_FCT_ATTR NARETimerRQBFree(
    LSA_UINT16 NARETimerID);

/*===========================================================================*/
/*                                   macros                                  */
/*===========================================================================*/

#define NARE_FATAL(Error) \
{                         \
NAREFatalError( Error,    \
NARE_MODULE_ID,           \
__LINE__);                \
}


#define NARE_ALLOC_FAIL_FATAL(pPtr)             \
{                                               \
    if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL)) \
    {                                           \
        NARE_FATAL(NARE_FATAL_ERR_NULL_PTR);    \
    }                                           \
}

#ifdef NARE_CFG_FREE_ERROR_IS_FATAL
#define NARE_MEM_FREE_FAIL_CHECK(RetVal)    \
{                                           \
    if( RetVal != LSA_RET_OK )              \
    {                                       \
        NARE_FATAL(NARE_FATAL_ERR_MEM_FREE);\
    }                                       \
}
#else
#define NARE_MEM_FREE_FAIL_CHECK(pPtr) {}
#endif

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined NARE_UPPER_IN_FCT_ATTR
#error "NARE_UPPER_IN_FCT_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_UPPER_OUT_FCT_ATTR
#error "NARE_UPPER_OUT_FCT_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_SYSTEM_IN_FCT_ATTR
#error "NARE_SYSTEM_IN_FCT_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_SYSTEM_OUT_FCT_ATTR
#error "NARE_SYSTEM_OUT_FCT_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_LOCAL_FCT_ATTR
#error "NARE_LOCAL_FCT_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_UPPER_RQB_ATTR
#error "NARE_UPPER_RQB_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_UPPER_MEM_ATTR
#error "NARE_UPPER_MEM_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_SYSTEM_MEM_ATTR
#error "NARE_SYSTEM_MEM_ATTR not defined in file nare_cfg.h"
#endif

#if !defined NARE_LOCAL_MEM_ATTR
#error "NARE_LOCAL_MEM_ATTR not defined in file nare_cfg.h"
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
#if defined  NARE_CFG_ARP_SNIFFER_CHECK_ARP_RESPONSE_TYPE
#error "NARE_CFG_ARP_SNIFFER_CHECK_ARP_RESPONSE_TYPE not used anymore"
#endif
#if defined  NARE_CFG_ARP_SNIFFER_VLAN_SUPPORT
#error "NARE_CFG_ARP_SNIFFER_VLAN_SUPPORT not used anymore (always used)"
#endif
#if defined  NARE_CFG_ARP_NRT_SEND_ERROR_IGNORE
#error "NARE_CFG_ARP_NRT_SEND_ERROR_IGNORE not used anymore (always used)"
#endif
#if defined NARE_CFG_LOCAL_MEM_STATIC
#error "NARE_CFG_LOCAL_MEM_STATIC not supported anymore"
#endif

/*****************************************************************************/
/*  end of file NARE_INT.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_INT_H */
