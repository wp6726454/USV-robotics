
#ifndef NARE_SYS_H                       /* ----- reinclude-protection ----- */
#define NARE_SYS_H

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
/*  F i l e               &F: nare_sys.h                                :F&  */
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
/*  Note: All output macros are in nare_out.h                                */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for NAREFatalError())                              */
/*---------------------------------------------------------------------------*/

#define NARE_FATAL_ERROR_TYPE  LSA_UINT32

#define NARE_FATAL_ERR_NULL_PTR            (NARE_FATAL_ERROR_TYPE)  0x01
#define NARE_FATAL_ERR_MEM_FREE            (NARE_FATAL_ERROR_TYPE)  0x02
#define NARE_FATAL_ERR_RELEASE_PATH_INFO   (NARE_FATAL_ERROR_TYPE)  0x03
#define NARE_FATAL_ERR_INCONSISTENZ        (NARE_FATAL_ERROR_TYPE)  0x04
#define NARE_FATAL_ERR_TIMER_RESOURCE      (NARE_FATAL_ERROR_TYPE)  0x05
#define NARE_FATAL_ERR_HANDLE_INVALID      (NARE_FATAL_ERROR_TYPE)  0x06
#define NARE_FATAL_ERR_DCP                 (NARE_FATAL_ERROR_TYPE)  0x07
#define NARE_FATAL_ERR_SOCK                (NARE_FATAL_ERROR_TYPE)  0x08
#define NARE_FATAL_ERR_EDD                 (NARE_FATAL_ERROR_TYPE)  0x09
#define NARE_FATAL_ERR_NIC_INVALID         (NARE_FATAL_ERROR_TYPE)  0x0A
#define NARE_FATAL_ERR_OPENING_SYSCHANNEL  (NARE_FATAL_ERROR_TYPE)  0x0B
#define NARE_FATAL_ERR_PATHINFO            (NARE_FATAL_ERROR_TYPE)  0x0C

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
#define NARE_DETAIL_PTR_TYPE                 /* pointer to NARE_DETAIL */ \
struct nare_detail_tag      NARE_SYSTEM_MEM_ATTR *

#define NARE_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag  NARE_LOCAL_MEM_ATTR *



/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by NARE_GET_PATH_INFO)                          */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in NARE_GET_PATH_INFO)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef union _NARE_CDB_PATH_PARAMS_TYPE
{
    /* Params for NARE_PATH_TYPE_DCP Channels */
    struct
    {
        LSA_UINT16      NICId;                  /* NIC-specific ID                    */
        LSA_UINT16      DCPIdentifyRetryCnt;    /* Number of retrys for Identify if   */
        /* no response.                       */
        LSA_UINT16      DCPSetRetryCnt;         /* Number of retrys for Set on timeout*/
        LSA_UINT16      MaxRDCP;                /* Max number of DCP-Requests simultaneously handled. */
    } Dcp;

    /* Params for NARE_PATH_TYPE_EDD Channels */
    struct
    {
        LSA_UINT16      NICId;                  /* NIC-specific ID                    */
        LSA_UINT16      ARPSniffTime;           /* Time in multiple of 100ms to sniff */
                                                /* for ARP-Responses.                 */
        LSA_UINT16      ARPRetryCnt;            /* spezifies the additional ARPs that */
                                                /* will be send if no response recv.  */
                                                /* in "NARE_OPC_TEST_IP" or           */
                                                /* in "NARE_OPC_RESOLVE_IP".          */
                                                /* 1..x.                              */
                                                /* delays are spezified in multiple of 100ms       */
        LSA_UINT16      ARPSniffTimeout1;       /* delay until the next ARP is send (odd retries)  */
        LSA_UINT16      ARPSniffTimeout2;       /* delay until the next ARP is send (even retries) */
        LSA_UINT16      MaxREDD;                /* Max number of EDD-Requests simultaneously handled. */

    } Edd;

    /* Params for NARE_PATH_TYPE_USER Channels */
    struct
    {
        LSA_UINT16      NICId;                  /* NIC-specific ID                    */
        LSA_UINT16      MaxRUser;               /* Max number of User-Requests simultaneously handled */
    } User;

    /* Params for NARA_PATH_TYPE_SOCK Channels */
    struct
    {
            LSA_UINT16      MaxRSOCK;               /* Max number of SOCK-Requests simultaneously handled */
    } Sock;

} NARE_CDB_PATH_PARAMS_TYPE;

typedef struct nare_detail_tag
{
    LSA_UINT16                PathType; /* Type of Channel                    */
    NARE_CDB_PATH_PARAMS_TYPE Params;   /* PathType specific Params.          */
} NARE_CDB_TYPE;

/* defines for PathType */

#define NARE_PATH_TYPE_USER   1   /* User-Channel            */
#define NARE_PATH_TYPE_SOCK   2   /* System-Channel for SOCK */
#define NARE_PATH_TYPE_DCP    3   /* System-Channel for DCP  */
#define NARE_PATH_TYPE_EDD    4   /* System-Channel for EDD  */

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
 * function name:  nare_init
 *
 * function:       initialize NARE
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/

LSA_UINT16  NARE_SYSTEM_IN_FCT_ATTR nare_init(LSA_VOID);

/*=============================================================================
 * function name:  nare_undo_init
 *
 * function:       undo the initialization of NARE
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/

LSA_UINT16  NARE_SYSTEM_IN_FCT_ATTR  nare_undo_init(LSA_VOID);

/*=============================================================================
 * function name:  nare_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  NARE_SYSTEM_IN_FCT_ATTR  nare_timeout(
    LSA_TIMER_ID_TYPE timer_id,
    LSA_USER_ID_TYPE  user_id);

/*============================================================================= 
 * function name:  NARE_REQUEST_UPPER_DONE 
 * 
 * function:       returns a finished request to the upper layer 
 * 
 * parameters:     NARE_UPPER_CALLBACK_FCT_PTR_TYPE  
 *                                            prefix_request_upper_done_ptr: 
 *                     pointer to prefix_request_upper_done_ptr, given by 
 *                     prefix_open_channel() 
 * 
 *                 NARE_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper- 
 *                                                            RQB 
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 * 
 * return value:   LSA_VOID 
 * 
 *===========================================================================*/
typedef  LSA_VOID  LSA_FCT_PTR( NARE_UPPER_OUT_FCT_ATTR,
                               NARE_UPPER_CALLBACK_FCT_PTR_TYPE)(    
                               NARE_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);

#ifndef NARE_REQUEST_UPPER_DONE
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_REQUEST_UPPER_DONE(    
       NARE_UPPER_CALLBACK_FCT_PTR_TYPE  nare_request_upper_done_ptr,    
       NARE_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,    
       LSA_SYS_PTR_TYPE                  sys_ptr
);
#endif

/*****************************************************************************/
/*                                                                           */
/* Output-functions are in nare_out.h                                        */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/* begin of packed Struktures                                                */
/*===========================================================================*/
/*                           ARP-Defines                                     */
/*===========================================================================*/
/* all will be 16 bit aligned                                                */

#include "pnio_pck2_on.h"

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Ethernet transmission layer                                    For IPv4   */
/*                                                              and Ethernet */
/*                                                                  Bytes    */
/*   48.bit: Ethernet address of destination                          6      */
/*   48.bit: Ethernet address of sender                               6      */
/*   16.bit: Protocol type = ether_type$ADDRESS_RESOLUTION  = 0x806   2      */
/*                                                                           */
/* Ethernet packet data:                                                     */
/*   16.bit: (ar$hrd) Hardware address space (e.g., Ethernet,         2      */
/*                    Packet Radio Net.)                                     */
/*   16.bit: (ar$pro) Protocol address space.  For Ethernet           2      */
/*                    hardware, this is from the set of type                 */
/*                    fields ether_typ$&lt;protocol&gt;.                     */
/*    8.bit: (ar$hln) byte length of each hardware address            1      */
/*    8.bit: (ar$pln) byte length of each protocol address            1      */
/*   16.bit: (ar$op)  opcode (ares_op$REQUEST | ares_op$REPLY)        2      */
/*   nbytes: (ar$sha) Hardware address of sender of this              6      */
/*	                  packet, n from the ar$hln field.                       */
/*   mbytes: (ar$spa) Protocol address of sender of this              4      */
/*                    packet, m from the ar$pln field.                       */
/*   nbytes: (ar$tha) Hardware address of target of this              6      */
/*                    packet (if known).                                     */
/*   mbytes: (ar$tpa) Protocol address of target.                     4      */
/*                                                                  -----    */
/*                                                                   42      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define NARE_ARP_TYPE             0x0806
#define NARE_VLAN_TYPE            0x8100

/* field ar$hdr */
#define NARE_ARP_HRD_ETHERNET     1

/* field ar$pro */
#define NARE_ARP_PRO_IP           0x800

/* field ar$op*/
#define NARE_ARP_OP_REQUEST       1
#define NARE_ARP_OP_RESPONSE      2

/* field ar$hln */
#define NARE_ARP_HLN              NARE_MAC_ADDR_SIZE /* 6 */

/* field ar$pln */
#define NARE_ARP_PLN              NARE_IP_ADDR_SIZE  /* 4 */

/* Frame offsets */
#define NARE_ARP_DST_OFFSET       (0)
#define NARE_ARP_SRC_OFFSET       (NARE_ARP_DST_OFFSET+NARE_MAC_ADDR_SIZE)
#define NARE_ARP_TYPE_OFFSET      (NARE_ARP_SRC_OFFSET+NARE_MAC_ADDR_SIZE)
#define NARE_ARP_HRD_OFFSET       (NARE_ARP_TYPE_OFFSET+2)
#define NARE_ARP_PRO_OFFSET       (NARE_ARP_HRD_OFFSET+2)
#define NARE_ARP_HLN_OFFSET       (NARE_ARP_PRO_OFFSET+2)
#define NARE_ARP_PLN_OFFSET       (NARE_ARP_HLN_OFFSET+1)
#define NARE_ARP_OP_OFFSET        (NARE_ARP_PLN_OFFSET+1)
#define NARE_ARP_SHA_OFFSET       (NARE_ARP_OP_OFFSET+2)
#define NARE_ARP_SPA_OFFSET       (NARE_ARP_OP_OFFSET+NARE_ARP_HLN)
#define NARE_ARP_THA_OFFSET       (NARE_ARP_OP_OFFSET+NARE_ARP_PLN)
#define NARE_ARP_TPA_OFFSET       (NARE_ARP_OP_OFFSET+NARE_ARP_HLN)

/* Note: with NARE_MAC_TYPE = 6 and NARE_IP_TYPE = 4 all 16-Bit */
/*       values are propably aligned at a 16-Bit boundary if    */
/*       the frame starts at a 16-Bit boundary!                 */

#define ARP_FRAME_HEADER \
    NARE_MAC_TYPE	DstMacAddr;		/* Destination MAC address */ \
    NARE_MAC_TYPE	SrcMacAddr;		/* Source MAC address      */ \
    LSA_UINT16	    Type;	  		/* Protocol Type (0x806)   */

#define ARP_FRAME_VLAN_HEADER \
    NARE_MAC_TYPE	DstMacAddr;		/* Destination MAC address */ \
    NARE_MAC_TYPE	SrcMacAddr;		/* Source MAC address      */ \
    LSA_UINT16	    VLANType; 		/* VLAN-Type               */ \
    LSA_UINT16	    VLANCtrl;		/* VLAN-Control            */ \
    LSA_UINT16	    Type;	  		/* Protocol Type (0x806)   */

#define ARP_FRAME_VLAN \
    LSA_UINT16	    VLANType; 		/* VLAN-Type               */ \
    LSA_UINT16	    VLANCtrl;		/* VLAN-Control            */  

#define ARP_FRAME_CONTENT \
    LSA_UINT16      Hrd;            /* Headertype    (1)       */ \
    LSA_UINT16      Pro;            /* Protocoltype  (0x800)   */ \
    LSA_UINT8       Hln;            /* Hardware addr. len (6)  */ \
    LSA_UINT8       Pln;            /* Protocol len (4)        */ \
    LSA_UINT16      Op;             /* Opcode (1,2)            */ \
    NARE_MAC_TYPE   Sha;            /* Sender Hardware addr.   */ \
    NARE_IP_TYPE    Spa;            /* Sender Protocol addr    */ \
    NARE_MAC_TYPE   Tha;            /* Target Hardware addr.   */ \
    NARE_IP_TYPE    Tpa;            /* Target Protocol addr.   */

PNIO_PACKED_ATTRIBUTE_PRE struct _ARP_FRAME_HEADER_TYPE
{
    ARP_FRAME_HEADER
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _ARP_FRAME_HEADER_TYPE NARE_ARP_FRAME_HEADER_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE  struct _ARP_FRAME_HEADER_VLAN_TYPE
{
    ARP_FRAME_VLAN_HEADER
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _ARP_FRAME_HEADER_VLAN_TYPE NARE_ARP_FRAME_HEADER_VLAN_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _ARP_FRAME_CONTENT_TYPE
{
    ARP_FRAME_CONTENT
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _ARP_FRAME_CONTENT_TYPE NARE_ARP_FRAME_CONTENT_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _ARP_FRAME_TYPE
{
    ARP_FRAME_HEADER
    ARP_FRAME_CONTENT
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _ARP_FRAME_TYPE NARE_ARP_FRAME_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _ARP_FRAME_VLAN_TYPE
{
    ARP_FRAME_VLAN_HEADER
    ARP_FRAME_CONTENT
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _ARP_FRAME_VLAN_TYPE NARE_ARP_FRAME_VLAN_TYPE;


typedef NARE_ARP_FRAME_HEADER_VLAN_TYPE   NARE_EDD_LOWER_MEM_ATTR * NARE_ARP_FRAME_HEADER_VLAN_PTR_TYPE;
typedef NARE_ARP_FRAME_CONTENT_TYPE       NARE_EDD_LOWER_MEM_ATTR * NARE_ARP_FRAME_CONTENT_PTR_TYPE;
typedef NARE_ARP_FRAME_TYPE               NARE_EDD_LOWER_MEM_ATTR * NARE_ARP_FRAME_PTR_TYPE;

#define NARE_ARP_TX_FRAME_LEN          sizeof(NARE_ARP_FRAME_TYPE)
#define NARE_ARP_TX_FRAME_VLAN_LEN     sizeof(NARE_ARP_FRAME_VLAN_TYPE)

#if NARE_MAC_ADDR_SIZE != 6
#error "IP-Addr size != 6 not supported! "
#endif

PNIO_PACKED_ATTRIBUTE_PRE union _NARE_UNION_MAC_TYPE
{
    struct
    {
        LSA_UINT32  MAC1234;
        LSA_UINT16  MAC56;
    } w;

    struct
    {
        LSA_UINT8   MACAddr[NARE_MAC_ADDR_SIZE];
    } b;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef union _NARE_UNION_MAC_TYPE NARE_UNION_MAC_TYPE;

#if NARE_IP_ADDR_SIZE != 4
#error "IP-Addr size != 4 not supported! "
#endif

PNIO_PACKED_ATTRIBUTE_PRE union _NARE_UNION_IP_TYPE
{
    struct
    {
        LSA_UINT32  IP1234;
    } w;

    struct
    {
        LSA_UINT8   IPAddr[NARE_IP_ADDR_SIZE];
    } b;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef union _NARE_UNION_IP_TYPE NARE_UNION_IP_TYPE;

/*===========================================================================*/
/*                           DCP-Defines                                     */
/*===========================================================================*/

#define NARE_DCP_IP_TYPE    NARE_IP_TYPE

/* --------------------------------------------------------------------------*/
/* DataSets used with Identify or Set (needed/used with NARE)                */
/* --------------------------------------------------------------------------*/

/* Dataset for Type: DCP_TYPE_TCP_IP */
PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_DS_TCPIP_TYPE
{
    NARE_DCP_IP_TYPE 	IpAddr;
    NARE_DCP_IP_TYPE 	SubnetMask;
    NARE_DCP_IP_TYPE 	DefaultRouter;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_DS_TCPIP_TYPE NARE_DCP_DS_TCPIP_TYPE;

#define NARE_DCP_DS_TCPIP_SIZE	12

/* Dataset for Type: DCP_TYPE_DEVICE_NAME */
PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_DS_DEVICE_NAME_TYPE
{
    LSA_UINT8   Name[2];    /* Name. variable length, only low-letters! */
                            /*       not 0 terminated.                  */
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_DS_DEVICE_NAME_TYPE NARE_DCP_DS_DEVICE_NAME_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_DS_DEVICE_VENDORID_TYPE
{
    LSA_UINT16  VendorID;
    LSA_UINT16  DeviceID;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_DS_DEVICE_VENDORID_TYPE NARE_DCP_DS_DEVICE_VENDORID_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_DS_DEVICE_INSTANCE_TYPE
{
    LSA_UINT16  InstanceID;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_DS_DEVICE_INSTANCE_TYPE NARE_DCP_DS_DEVICE_INSTANCE_TYPE;


/* DataSet-Union */
/* Data (part of V-field). Depends on "T"-Field (Type) */

PNIO_PACKED_ATTRIBUTE_PRE union NARE_DCP_DS_DATA_
{                                                    /* used with Type (T):      */
    NARE_DCP_DS_TCPIP_TYPE	         TcpIp;          /* DCP_TYPE_TCP_IP       	*/
    NARE_DCP_DS_DEVICE_NAME_TYPE     NameOfStation;  /* DCP_TYPE_DEVICE_NAME  	*/
    NARE_DCP_DS_DEVICE_VENDORID_TYPE DevAndVendorID;
    NARE_DCP_DS_DEVICE_INSTANCE_TYPE InstanceId;
} PNIO_PACKED_ATTRIBUTE_POST;

typedef union NARE_DCP_DS_DATA_ NARE_DCP_DS_DATA;


/* --------------------------------------------------------------------------*/
/* TLVs                                                                      */
/* --------------------------------------------------------------------------*/
/* Important: Every TLV has to be even in size, so every chained TLV starts  */
/*            on a even address. So after a TLV a padding byte may be added. */
/*            This padding byte is not counted in "Len". So if Len has a odd */
/*            value we have one padding-byte to add (req) or take care of    */
/*            (rsp).                                                         */
/*                                                                           */
/*            With this Type/Len/Status and start of Data is always WORD     */
/*            aligned!                                                       */
/*                                                                           */
/* 1) Swapping:  The Type and all other 16/32-Bit values have to be swapped  */
/*               with NARE_HTONS() or NARE_NTOS() on writing/reading.        */
/* --------------------------------------------------------------------------*/

#define NARE_DCP_TYPE_LEN \
    LSA_UINT16          Type;           /* Option + Suboption 		*/ \
    LSA_UINT16          Len;            /* Length of "V"            */ 

#define NARE_DCP_STATUS \
    LSA_UINT16          Status;


PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_TL_TYPE
{
    NARE_DCP_TYPE_LEN
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_TL_TYPE NARE_DCP_TL_TYPE;


PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_TLS_TYPE
{
    NARE_DCP_TYPE_LEN
    NARE_DCP_STATUS
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_TLS_TYPE NARE_DCP_TLS_TYPE;

/* --------------------------------------------------------------------------*/
/* TLV for DCP-Identify-Request/Response (=Filter-TLV)                       */
/* --------------------------------------------------------------------------*/

/* Request:                                 */
/* Note: T = Type (Option + Subotion)       */
/*       L = Len                            */
/*       V = Data                           */

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_IDENT_REQ_TLV_TYPE	/* without Status!		*/
{
    NARE_DCP_TYPE_LEN
    NARE_DCP_DS_DATA    Data;			/* Variable data part		*/

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_IDENT_REQ_TLV_TYPE NARE_DCP_IDENT_REQ_TLV_TYPE;

typedef NARE_DCP_IDENT_REQ_TLV_TYPE   NARE_DCP_LOWER_MEM_ATTR * NARE_DCP_IDENT_REQ_TLV_PTR_TYPE;

/* Type   : e.g. DCP_TYPE_DEVICE_NAME used as filter.              */

/* Response:                                */
/* Note: T = Type (Option + Subotion)       */
/*       L = Len                            */
/*       V = Status+Data                    */

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_IDENT_RSP_TLV_TYPE
{
    NARE_DCP_TYPE_LEN
    NARE_DCP_STATUS
    NARE_DCP_DS_DATA	Data;           /* Variable data part       */
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_IDENT_RSP_TLV_TYPE NARE_DCP_IDENT_RSP_TLV_TYPE;

typedef NARE_DCP_IDENT_RSP_TLV_TYPE   NARE_DCP_LOWER_MEM_ATTR * NARE_DCP_IDENT_RSP_TLV_PTR_TYPE;

/* Type   : Databasis returned (in addition to filter-types)       */
/*          Identify always returns the following Datasets:        */
/*          DCP_TYPE_TCP_IP            1)                          */
/*          DCP_TYPE_DEVICE_NAME       1)                          */
/*          DCP_TYPE_DEVICE_ID         1)                          */
/*          DCP_TYPE_DEVICE_ROLE       1)                          */
/*          DCP_TYPE_DEVICE_OPTIONS    1)                          */
/* Status : depends on Type                                        */

/* STATUS (e.g. BlockInfo) of DCP_TYPE_TCP_IP */
#define NARE_DCP_IDENTIFY_STATUS_IP_ADDR_CONFLICT   0x0080  /* Bit 7 */

/* --------------------------------------------------------------------------*/
/* TLV for DCP-Set-Request/Response                                          */
/* --------------------------------------------------------------------------*/

/* Request:                                 */
/* Note: T = Type (Option + Subotion)       */
/*       L = Len                            */
/*       V = Status+Data                    */

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_SET_REQ_TLV_TYPE
{
    NARE_DCP_TYPE_LEN
    NARE_DCP_STATUS
    NARE_DCP_DS_DATA	Data;			/* Variable data part		*/
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_SET_REQ_TLV_TYPE NARE_DCP_SET_REQ_TLV_TYPE;

typedef NARE_DCP_SET_REQ_TLV_TYPE   NARE_DCP_LOWER_MEM_ATTR * NARE_DCP_SET_REQ_TLV_PTR_TYPE;

/* Type   : e.g. DCP_TYPE_TCP_IP used to set IP-parameters          */
/* Status : remanent or not remanent storage of parameters          */

#define NARE_DCP_STATUS_NOT_REMANENT    0x0 /* save values not remanent */
#define NARE_DCP_STATUS_REMANENT        0x1 /* save values remanent     */


/* Response:                                */
/* Note: T = Type (Option + Subotion)       */
/*       L = Len                            */
/*       V = Status+Result+AddOn            */

PNIO_PACKED_ATTRIBUTE_PRE struct _NARE_DCP_SET_RSP_TLV_TYPE
{
    NARE_DCP_TYPE_LEN
    NARE_DCP_STATUS
    LSA_UINT8           Result;         /* Result dataset	    	*/
    LSA_UINT8           AddOn[2];       /* Variable AddOn, depends  */
                                        /* on Len                   */
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _NARE_DCP_SET_RSP_TLV_TYPE NARE_DCP_SET_RSP_TLV_TYPE;


typedef NARE_DCP_SET_RSP_TLV_TYPE   NARE_DCP_LOWER_MEM_ATTR * NARE_DCP_SET_RSP_TLV_PTR_TYPE;

/* Type   : shall be: DCP_TYPE_CTRL_RESULT  1)                     */
/* Status : shall be  option+suboption. eg. DCP_TYPE_TCP_IP  1)    */
/* Result : shall be:                                              */
/*          DCP_RESULT_OK			0                              */
/*          DCP_RESULT_NO_DATABASE	1                              */
/*          DCP_RESULT_NO_DATASET	2                              */
/*          DCP_RESULT_DS_NOT_SET	3                              */
/*          DCP_RESULT_UNKNOWN		4                              */
/*          DCP_RESULT_NO_ACTION	5                              */
/*          DCP_RESULT_NO_RESOURCE	6                              */
/* AddOn  : Dont care.                                             */

/* --------------------------------------------------------------------------*/

#include "pnio_pck_off.h"

/*****************************************************************************/
/*  end of packed structures                                                 */
/*****************************************************************************/

/*****************************************************************************/
/*  end of file nare_sys.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_SYS_H */
