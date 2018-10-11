
#ifndef OHA_USR_H                     /* ----- reinclude-protection ----- */
#define OHA_USR_H

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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_usr.h                                 :F&  */
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
/*  Defines constants, types, macros and prototyping for OHA                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

/*-----------------------------------------------------------------------------
// library
//---------------------------------------------------------------------------*/

#include "oha_lib.h"

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/

#define OHA_OPC_OPEN_CHANNEL                (LSA_OPCODE_TYPE)   0x01
#define OHA_OPC_CLOSE_CHANNEL               (LSA_OPCODE_TYPE)   0x02
#define OHA_OPC_GET_CONFIG                  (LSA_OPCODE_TYPE)   0x03
#define OHA_OPC_GET_ADDRESS                 (LSA_OPCODE_TYPE)   0x04
#define OHA_OPC_SET_ADDRESS                 (LSA_OPCODE_TYPE)   0x05

#define OHA_OPC_SET_IDENT                   (LSA_OPCODE_TYPE)   0x06

#define OHA_OPC_DCP_CONTROL                 (LSA_OPCODE_TYPE)   0x07
#define OHA_OPC_DCP_INDICATION              (LSA_OPCODE_TYPE)   0x08
#define OHA_OPC_DCP_RESPONSE                (LSA_OPCODE_TYPE)   0x09
#define OHA_OPC_TEST_LLDP_CONTROL           (LSA_OPCODE_TYPE)   0x0A
#define OHA_OPC_LLDP_INDICATION             (LSA_OPCODE_TYPE)   0x0B
#define OHA_OPC_GET_NEIGHBOUR               (LSA_OPCODE_TYPE)   0x0C

#define OHA_OPC_LLDP_CONTROL_OID            (LSA_OPCODE_TYPE)   0x0E
#define OHA_OPC_DCP_READ_ELEMENT            (LSA_OPCODE_TYPE)   0x0F
#define OHA_OPC_DCP_WRITE_ELEMENT           (LSA_OPCODE_TYPE)   0x10
/* TIA 1030928: OHA_OPC_TEST_SNMP_CONTROL removed */
#define OHA_OPC_LLDP_READ_ELEMENT           (LSA_OPCODE_TYPE)   0x12


#define OHA_OPC_WRITE_PTCP_DATA             (LSA_OPCODE_TYPE)   0x17

#define OHA_OPC_READ_QUERY_ELEMENTS         (LSA_OPCODE_TYPE)   0x18

#define OHA_OPC_TIMER                       (LSA_OPCODE_TYPE)   0x1A  /* only internal usage */

#define OHA_OPC_HELLO_INDICATION            (LSA_OPCODE_TYPE)   0x1B

#define OHA_OPC_ADDR_INFO_IND               (LSA_OPCODE_TYPE)   0x1C

#define OHA_OPC_PRM_PREPARE                 (LSA_OPCODE_TYPE)   0x21
#define OHA_OPC_PRM_WRITE                   (LSA_OPCODE_TYPE)   0x22
#define OHA_OPC_PRM_END                     (LSA_OPCODE_TYPE)   0x23
#define OHA_OPC_PRM_COMMIT                  (LSA_OPCODE_TYPE)   0x24
#define OHA_OPC_PRM_READ                    (LSA_OPCODE_TYPE)   0x25
#define OHA_OPC_PRM_INDICATION              (LSA_OPCODE_TYPE)   0x26

#define OHA_OPC_REMA_INDICATION             (LSA_OPCODE_TYPE)   0x27
#define OHA_OPC_REMA_READ                   (LSA_OPCODE_TYPE)   0x28
#define OHA_OPC_REMA_CONTROL                (LSA_OPCODE_TYPE)   0x29

#define OHA_OPC_MRP_CONTROL_OID             (LSA_OPCODE_TYPE)   0x3A

#define OHA_OPC_LLDP_WRITE_OPTION           (LSA_OPCODE_TYPE)   0x31
#define OHA_OPC_LLDP_READ_OPTIONS           (LSA_OPCODE_TYPE)   0x32
#define OHA_OPC_PORT_CONTROL                (LSA_OPCODE_TYPE)   0x33

#define OHA_OPC_TCIP_CONTROL                (LSA_OPCODE_TYPE)   0x34

#define OHA_OPC_SET_EMERGENCY               (LSA_OPCODE_TYPE)   0x36

#define OHA_OPC_LLDP_ACCESS_CONTROL         (LSA_OPCODE_TYPE)   0x40
#define OHA_OPC_LLDP_CONFIG_INDICATION      (LSA_OPCODE_TYPE)   0x41
#define OHA_OPC_LLDP_CONFIG_RESPONSE        (LSA_OPCODE_TYPE)   0x42
#define OHA_OPC_LLDP_PORT_CONFIG_INDICATION (LSA_OPCODE_TYPE)   0x43
#define OHA_OPC_LLDP_PORT_CONFIG_RESPONSE   (LSA_OPCODE_TYPE)   0x44

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

/* OHA OK-Code (MaxOffset = 0x3F)  */
/* LSA_RSP_OK_OFFSET = 0x40        */

#define OHA_OK                          LSA_RSP_OK
#define OHA_OK_CANCEL                   (LSA_RSP_OK_OFFSET + 0x01)
#define OHA_OK_ACTIVE                   (LSA_RSP_OK_OFFSET + 0x02)

#define OHA_ERR_RESOURCE                LSA_RSP_ERR_RESOURCE
#define OHA_ERR_PARAM                   LSA_RSP_ERR_PARAM
#define OHA_ERR_SEQUENCE                LSA_RSP_ERR_SEQUENCE
#define OHA_ERR_SYS_PATH                LSA_RSP_ERR_SYS_PATH

/* OHA ERR-Codes (MaxOffset = 0x3E) */
/* LSA_RSP_ERR_OFFSET = 0xC0        */

#define OHA_ERR_CHANNEL_USE             (LSA_RSP_ERR_OFFSET + 0x01)
#define OHA_ERR_SYNTAX                  (LSA_RSP_ERR_OFFSET + 0x02)
#define OHA_ERR_REF                     (LSA_RSP_ERR_OFFSET + 0x03)
#define OHA_ERR_LL                      (LSA_RSP_ERR_OFFSET + 0x04)
#define OHA_ERR_PROTOCOL                (LSA_RSP_ERR_OFFSET + 0x05)
#define OHA_ERR_NOT_IMPLEMENTED         (LSA_RSP_ERR_OFFSET + 0x06)
#define OHA_ERR_STATION_NOT_FOUND       (LSA_RSP_ERR_OFFSET + 0x07)
#define OHA_ERR_MULTIPLE_STATIONS       (LSA_RSP_ERR_OFFSET + 0x08)
#define OHA_ERR_MULTIPLE_IP             (LSA_RSP_ERR_OFFSET + 0x09)
#define OHA_ERR_FAILED                  (LSA_RSP_ERR_OFFSET + 0x0A)
#define OHA_ERR_BUFFER_OVERFLOW         (LSA_RSP_ERR_OFFSET + 0x0B)
#define OHA_ERR_USER_LOCK               (LSA_RSP_ERR_OFFSET + 0x0C)
#define OHA_ERR_DATA_INVALID            (LSA_RSP_ERR_OFFSET + 0x0D)

/* Prm Read/Write */
#define OHA_ERR_PRM_INDEX               (LSA_RSP_ERR_OFFSET + 0x10)
#define OHA_ERR_PRM_DATA                (LSA_RSP_ERR_OFFSET + 0x11)
#define OHA_ERR_PRM_BLOCK               (LSA_RSP_ERR_OFFSET + 0x12)
#define OHA_ERR_PRM_PORTID              (LSA_RSP_ERR_OFFSET + 0x13)

/* Prm End */
#define OHA_ERR_PRM_CONSISTENCY         (LSA_RSP_ERR_OFFSET + 0x14)

/*===========================================================================*/
/*                              basic types for OHA                          */
/*===========================================================================*/

#define OHA_UPPER_MEM_PTR_TYPE       LSA_VOID            OHA_UPPER_MEM_ATTR *
#define OHA_UPPER_MEM_U8_PTR_TYPE    LSA_UINT8           OHA_UPPER_MEM_ATTR *
#define OHA_UPPER_MEM_U16_PTR_TYPE   LSA_UINT16          OHA_UPPER_MEM_ATTR *
#define OHA_UPPER_MEM_U32_PTR_TYPE   LSA_UINT32          OHA_UPPER_MEM_ATTR *
#define OHA_UPPER_MEM_U64_PTR_TYPE   LSA_UINT64          OHA_UPPER_MEM_ATTR *

#define OHA_EDD_LOWER_MEM_PTR_TYPE   LSA_VOID            OHA_EDD_LOWER_MEM_ATTR *
#define OHA_EDD_LOWER_MEM_U8_PTR_TYPE   LSA_UINT8        OHA_EDD_LOWER_MEM_ATTR *

#define OHA_DCP_LOWER_MEM_PTR_TYPE      LSA_VOID         OHA_DCP_LOWER_MEM_ATTR *
#define OHA_DCP_LOWER_MEM_U8_PTR_TYPE   LSA_UINT8        OHA_DCP_LOWER_MEM_ATTR *

#define OHA_NARE_LOWER_MEM_PTR_TYPE     LSA_VOID         OHA_NARE_LOWER_MEM_ATTR *
#define OHA_NARE_LOWER_MEM_U8_PTR_TYPE  LSA_UINT8        OHA_NARE_LOWER_MEM_ATTR *

#define OHA_MRP_LOWER_MEM_PTR_TYPE      LSA_VOID         OHA_MRP_LOWER_MEM_ATTR *
#define OHA_MRP_LOWER_MEM_U8_PTR_TYPE   LSA_UINT8        OHA_MRP_LOWER_MEM_ATTR *

#define OHA_LLDP_LOWER_MEM_PTR_TYPE     LSA_VOID         OHA_LLDP_LOWER_MEM_ATTR *
#define OHA_LLDP_LOWER_MEM_U8_PTR_TYPE  LSA_UINT8        OHA_LLDP_LOWER_MEM_ATTR *

#define OHA_SOCK_LOWER_MEM_PTR_TYPE     LSA_VOID         OHA_SOCK_LOWER_MEM_ATTR *
#define OHA_SOCK_LOWER_MEM_U8_PTR_TYPE  LSA_UINT8        OHA_SOCK_LOWER_MEM_ATTR *

#define OHA_SNMPX_LOWER_MEM_PTR_TYPE    LSA_VOID         OHA_SNMPX_LOWER_MEM_ATTR *
#define OHA_SNMPX_LOWER_MEM_U8_PTR_TYPE LSA_UINT8        OHA_SNMPX_LOWER_MEM_ATTR *

#define OHA_LOCAL_MEM_PTR_TYPE          LSA_VOID         OHA_LOCAL_MEM_ATTR *
#define OHA_LOCAL_MEM_U8_PTR_TYPE       LSA_UINT8        OHA_LOCAL_MEM_ATTR *
#define OHA_LOCAL_MEM_U16_PTR_TYPE      LSA_UINT16       OHA_LOCAL_MEM_ATTR *
#define OHA_LOCAL_MEM_U32_PTR_TYPE      LSA_UINT32       OHA_LOCAL_MEM_ATTR *

#define OHA_LOCAL_MEM_U8_CONST_PTR_TYPE const LSA_UINT8  OHA_LOCAL_MEM_ATTR *
#define OHA_LOCAL_MEM_BOOL_PTR_TYPE     LSA_BOOL         OHA_COMMON_MEM_ATTR *

#define OHA_UPPER_RQB_MEM_PTR_TYPE      LSA_VOID         OHA_UPPER_RQB_ATTR *

#define OHA_COMMON_MEM_PTR_TYPE         LSA_VOID         OHA_COMMON_MEM_ATTR *
#define OHA_COMMON_MEM_U8_PTR_TYPE      LSA_UINT8        OHA_COMMON_MEM_ATTR *
#define OHA_COMMON_MEM_U16_PTR_TYPE     LSA_UINT16       OHA_COMMON_MEM_ATTR *
#define OHA_COMMON_MEM_U32_PTR_TYPE     LSA_UINT32       OHA_COMMON_MEM_ATTR *
#define OHA_COMMON_MEM_BOOL_PTR_TYPE    LSA_BOOL         OHA_COMMON_MEM_ATTR *


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct oha_rqb_tag     OHA_UPPER_RQB_ATTR *    OHA_UPPER_RQB_PTR_TYPE;

#define OHA_MAC_ADDR_SIZE  6
typedef LSA_UINT8 OHA_MAC_TYPE[OHA_MAC_ADDR_SIZE];

#define OHA_IP_ADDR_SIZE   4
#define OHA_IP_SUITE_SIZE  12      /* IpAddress + SubnetMask + RouterAddress */

typedef LSA_UINT8 OHA_IP_TYPE[OHA_IP_ADDR_SIZE];

typedef struct oha_ip_suite_tag
{
    OHA_IP_TYPE     IpAddr;                 /* IP address                    */
    OHA_IP_TYPE     NetMask;                /* associated net mask           */
    OHA_IP_TYPE     Gateway;                /* associated gateway address    */
} OHA_IP_SUITE_TYPE;

typedef struct oha_ip_suite_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_IP_SUITE_PTR_TYPE;

typedef struct oha_name_tag
{
    OHA_UPPER_MEM_U8_PTR_TYPE pName;        /* NameOfStation                 */
    LSA_UINT16                Length;       /* length of NameOfStation       */
    LSA_BOOL                  Remanent;     /* true if remanent              */
} OHA_NAME_TYPE;

typedef struct oha_name_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_NAME_PTR_TYPE;

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

#ifndef OHA_MAX_STATION_NAME_LEN
#define OHA_MAX_STATION_NAME_LEN        DCP_MAX_STATION_NAME_LEN /* 255 */
#endif
#ifndef OHA_MAX_STATION_TYPE_LEN
#define OHA_MAX_STATION_TYPE_LEN        DCP_MAX_STATION_TYPE_LEN /* 255 */
#endif

/*===========================================================================*/
/*                                 SNMP                                      */
/*===========================================================================*/

#define OHA_OID                     LSA_UINT32
#define OHA_OID_PTR_TYPE            OHA_OID      OHA_UPPER_MEM_ATTR *

#define  OHA_SNMP_SIZE            1400  /* MAX size of an snmp packet */

#define  OHA_SNMP_PORT             161  /* reseved UDP port for SNMP  */
#define  OHA_SNMP_TRAP_PORT        162


#define  OHA_MAX_OID_LEN       64    /* max length in OHA_OID's of an encoded oid */

/* snmp version */

#define  OHA_SNMP_VERSION_1     0
#define  OHA_SNMP_VERSION_2     1
#define  OHA_SNMP_VERSION_3     3

/* snmp message types */

#define  OHA_SNMP_GET_REQ       (0x0)
#define  OHA_SNMP_GETNEXT_REQ   (0x1)
#define  OHA_SNMP_GET_RSP       (0x2)
#define  OHA_SNMP_SET_REQ       (0x3)
#define  OHA_SNMP_TRP_REQ       (0x4)
#define  OHA_SNMP_GETBULK_REQ   (0x5)

/* snmp error status */

#define  OHA_SNMP_ERR_NOERROR     (0x0)
#define  OHA_SNMP_ERR_TOOBIG      (0x1)
#define  OHA_SNMP_ERR_NOSUCHNAME  (0x2)
#define  OHA_SNMP_ERR_NOSUCH      (0x2)
#define  OHA_SNMP_ERR_BADVALUE    (0x3)
#define  OHA_SNMP_ERR_READONLY    (0x4)
#define  OHA_SNMP_ERR_GENERR      (0x5)

/* snmp traps */

#define  OHA_SNMP_TRAP_COLDSTART       0
#define  OHA_SNMP_TRAP_WARMSTART       1
#define  OHA_SNMP_TRAP_LINKDOWN        2
#define  OHA_SNMP_TRAP_LINKUP          3
#define  OHA_SNMP_TRAP_AUTHFAIL        4
#define  OHA_SNMP_TRAP_EGPNEIGHBORLOSS 5
#define  OHA_SNMP_TRAP_ENTERPRISESPECIFIC  6

/* types of snmp objects */

/* ASN.1 primitive types */
#define  OHA_SNMP_INTEGER       (0x02)
#define  OHA_SNMP_INTEGER32     (0x02)
#define  OHA_SNMP_BITS          (0x03)
#define  OHA_SNMP_STRING        (0x04)
#define  OHA_SNMP_NULLOBJ       (0x05)
#define  OHA_SNMP_OBJID         (0x06)
/* defined types (from the SMI, RFC 1065) */
/* 0x40 ... ASN_APPLICATION */
#define  OHA_SNMP_IPADDRESS   (0x40 |  0)
#define  OHA_SNMP_COUNTER     (0x40 |  1)
#define  OHA_SNMP_COUNTER32   (0x40 |  1)
#define  OHA_SNMP_GAUGE       (0x40 |  2)
#define  OHA_SNMP_GAUGE32     (0x40 |  2)
#define  OHA_SNMP_UNSIGNED32  (0x40 |  2)
#define  OHA_SNMP_TIMETICKS   (0x40 |  3)
#define  OHA_SNMP_OPAQUE      (0x40 |  4)
/*       OHA_SNMP_NSAP        (0x40 |  5)            historic */
#define  OHA_SNMP_COUNTER64   (0x40 |  6)
/*       OHA_SNMP_UINTEGER    (0x40 |  7)            historic */
#define  OHA_SNMP_FLOAT       (0x40 |  8)
#define  OHA_SNMP_DOUBLE      (0x40 |  7)
#define  OHA_SNMP_INTEGER64   (0x40 | 10)
#define  OHA_SNMP_UNSIGNED64  (0x40 | 11)

/* for SnmpVersion OHA_SNMP_VERSION_2 und OHA_SNMP_VERSION_3 */
#define  OHA_SNMP_ASN_NSO     (0x80)  /* RFC1905:noSuchObject   */
#define  OHA_SNMP_ASN_NSI     (0x81)  /* RFC1905:noSuchInstance */
#define  OHA_SNMP_ASN_EOM     (0x82)  /* RFC1905:endOfMib       */

/*  Error codes:  */
#define  OHA_SNMP_RONLY                 0xAAAA   /* read access for everyone */
#define  OHA_SNMP_RWRITE                0xAABA   /* add write access for community private */
#define  OHA_SNMP_NOACCESS              0x0000   /* no access for anybody */
#define  OHA_SNMP_READCREATE            0xBBBB   /* read,write,create access for everyone */
#define  OHA_SNMP_WRITEONLY             0xCCCC   /* writeonly access for everyone */
#define  OHA_SNMP_ACCESSIBLE_FOR_NOTIFY 0xDDDD   /* notify access for everyone */

/*===========================================================================*/
/*                                 SNMPX                                     */
/*===========================================================================*/

#define OHA_LLDP_MIB_TREE           1,0,8802,1,1,2
#define OHA_LLDP_MIB_TREE_LEN       6

/* the oid of the PNO-SMI-MIB */
#define OHA_SNMP_PNO_SMI_OID        1,3,6,1,4,1,24686
#define OHA_SNMP_PNO_SMI_OID_LEN    7

/*===========================================================================*/
/*                               OHA_DB                                      */
/*===========================================================================*/

/* access-controls for DB elements (OHA_DB_ELEM) */

#define  OHA_READ_ONLY          0xAA   /* read access for everyone */
#define  OHA_NO_ACCESS          0x00   /* no access for anybody */
#define  OHA_READ_WRITE         0xBB   /* read,write,create access for everyone */
#define  OHA_WRITE_ONLY         0xCC   /* writeonly access for everyone */
#define  OHA_QUERY_WRITE        0xDD   /* query write-access for everyone */

/*===========================================================================*/
/*                   DB (DataBase Element)                                   */
/*===========================================================================*/

/*==============                    DCP         =============================*/
/* In V6.0 OHA_DB_ELEM_TYPE is internal. Opcodes uses OHA_DCP_ELEM_TYPE.     */

typedef struct oha_db_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_DB_ELEM_PTR_TYPE;

typedef struct oha_db_elem_tag
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  ListElem;   /* queue element */
    LSA_UINT8                   Type;       /* type of variable (option)     */
    LSA_UINT8                   SubType;    /* type of variable (suboption)  */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable;  /* variable in octets            */
    LSA_UINT16                  Length;     /* length of above in octets     */
    LSA_UINT8                   AccCtrl;    /* access control for variable   */
    LSA_BOOL                    Remanent;   /* storage of variable in memory */
    LSA_UINT16                  LockCnt[OHA_CFG_MAX_CHANNELS]; /* passed to AccCtrl by a user   */
} OHA_DB_ELEM_TYPE;


/*==============                    LLDP        =============================*/
/* In V6.0 OHA_DB_LLDP_ELEM_TYPE is internal. Opcodes uses OHA_LLDP_OPTION_TYPE. */

typedef struct oha_db_lldp_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_DB_LLDP_ELEM_PTR_TYPE;

typedef struct oha_db_lldp_elem_tag
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) ListElem;   /* queue element */
    LSA_UINT8                   Type;       /* type of variable */
    LSA_UINT8                   SubType;    /* subtype of variable */
    LSA_UINT32                  OUI;        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable;  /* variable in octets */
    LSA_UINT16                  Length;     /* length of pVariable in octets */
    LSA_BOOL                    Remanent;   /* storage of variable in memory */
} OHA_DB_LLDP_ELEM_TYPE;


/*===========================================================================*/
/* OID parameters for LLDP-SET (PortConfiguration)                           */
/*===========================================================================*/

#define OHA_LLDP_TX_ENABLE       1  /* true  */
#define OHA_LLDP_TX_DISABLE      2  /* false */

/* AdminStatus: (refer to LLDP.MIB: lldpPortConfigAdminStatus) */

#define OHA_LLDP_SEND_ONLY       1  /* enables only send of lldp-data, txOnly(1) */
#define OHA_LLDP_RECV_ONLY       2  /* enables only receive of lldp-data, rxOnly(2) */
#define OHA_LLDP_SEND_RECV       3  /* enables send and receive, txAndRx(3) */
#define OHA_LLDP_DISABLED        4  /* no send or receive LLDP frames on this port, disabled (4) */

/* TLVsTxEnable: (refer to LLDP.MIB: lldpPortConfigTLVsTxEnable) */

#define OHA_LLDP_TLVS_TX_DISABLE         0
#define OHA_LLDP_TLVS_TX_ENABLE  (OHA_LLDP_PORT_DESC_TX | OHA_LLDP_SYS_NAME_TX | OHA_LLDP_SYS_DESC_TX | OHA_LLDP_SYS_CAP_TX)

#define OHA_LLDP_PORT_DESC_TX             0x80  /* bit portDesc(0) of lldpPortConfigTLVsTxEnable  */
#define OHA_LLDP_SYS_NAME_TX              0x40  /* bit sysName(1) of lldpPortConfigTLVsTxEnable   */
#define OHA_LLDP_SYS_DESC_TX              0x20  /* bit sysDesc(2) of lldpPortConfigTLVsTxEnable   */
#define OHA_LLDP_SYS_CAP_TX               0x10  /* bit sysCap(3) of lldpPortConfigTLVsTxEnable"   */

/* Dot3TLVsTxEnable: (refer to LLDP.MIB: lldpXdot3PortConfigTLVsTxEnable) */

#define OHA_DOT3_TLVS_TX_DISABLE         0
#define OHA_DOT3_TLVS_TX_ENABLE (OHA_DOT3_MAC_PHY_CONFIG_STATUS_TX | OHA_DOT3_POWER_VIA_MDI_TX | \
								  OHA_DOT3_LINK_AGGREGATION_TX | OHA_DOT3_MAX_FRAME_SIZE_TX)

#define OHA_DOT3_MAC_PHY_CONFIG_STATUS_TX 0x80 /* bit macPhyConfigStatus(0) of lldpXdot3PortConfigTLVsTxEnable */
#define OHA_DOT3_POWER_VIA_MDI_TX         0x40 /* bit powerViaMDI(1) of lldpXdot3PortConfigTLVsTxEnable" */
#define OHA_DOT3_LINK_AGGREGATION_TX      0x20 /* bit linkAggregation(2) of lldpXdot3PortConfigTLVsTxEnable */
#define OHA_DOT3_MAX_FRAME_SIZE_TX        0x10 /* bit maxFrameSize(3) of lldpXdot3PortConfigTLVsTxEnable */


/*===========================================================================*/
/*                                  DCP                                      */
/*===========================================================================*/

/*            DCP Database IDs (wellknown Options)                  */

#define OHA_DCP_OPTION_TCP      DCP_DBID_TCP       /* TCP-IP parameter         */
#define OHA_DCP_OPTION_IDENT    DCP_DBID_IDENT     /* Station identification   */
#define OHA_DCP_OPTION_DHCP     DCP_DBID_DHCP      /* DHCP-Parameter           */
#define OHA_DCP_OPTION_LLDP     DCP_DBID_LLDP      /* DHCP-Parameter           */
#define OHA_DCP_OPTION_CTRL     DCP_DBID_CTRL      /* Control options          */
#define OHA_DCP_OPTION_PBPARAM  DCP_DBID_PBPARAM   /* Profibus parameter       */
#define OHA_DCP_OPTION_NOFILTER DCP_DBID_NOFILTER  /* IDENTIFY No filter       */

/* DCP-Control (Suboptions)                                         */

#define OHA_DCP_TRANS_START       DCP_DSID_START     /* CONTROL: Start transact. */
#define OHA_DCP_TRANS_END         DCP_DSID_END       /* CONTROL: EndOf transact. */
#define OHA_DCP_SIGNAL            DCP_DSID_SIGNAL    /* CONTROL: Signal          */
#define OHA_DCP_RESPONSE          DCP_DSID_RESPONSE  /* CONTROL: Response result */
#define OHA_DCP_RESET_TO_FACTORY  DCP_DSID_RESETFACT /* CONTROL: Reset to factory settings */
#define OHA_DCP_RESET             DCP_DSID_RESET     /* CONTROL: Reset param and/or data   */

/* DCP-Dataset IDs (Suboptions) -> see DCP_USR.H                    */

/*===========================================================================*/
/*                                LLDP                                       */
/*===========================================================================*/
#ifndef OHA_MAX_TLV_DATA_SIZE
#define OHA_MAX_TLV_DATA_SIZE           LLDP_MAX_TLV_DATA_SIZE
#endif

#ifndef OHA_MAX_CHASSIS_STRLEN
#define OHA_MAX_CHASSIS_STRLEN          LLDP_MAX_CHASSIS_ID_STRING_SIZE /* 255 */
#endif
#ifndef OHA_MAX_PORT_STRLEN
#define OHA_MAX_PORT_STRLEN             LLDP_MAX_PORT_ID_STRING_SIZE /* 255 */
#endif

/* LLDP-Types */

#define OHA_LLDP_TYPE_CHASSIS_ID          0x01
#define OHA_LLDP_TYPE_PORT_ID             0x02
#define OHA_LLDP_TYPE_TTL                 0x03
#define OHA_LLDP_TYPE_PORT_DESCR          0x04      /* ifDescr is recommended */
#define OHA_LLDP_TYPE_SYSTEM_NAME         0x05      /* sysName is recommended */
#define OHA_LLDP_TYPE_SYSTEM_DESCR        0x06      /* sysDescr is recommended */
#define OHA_LLDP_TYPE_SYSTEM_CAP          0x07
#define OHA_LLDP_TYPE_MGM_ADDR            0x08
#define OHA_LLDP_TYPE_ORG_SPECIFIC        0x7F


/* Chassis-Id  (LLDP-Type 1) */

#define OHA_CHASSIS_ID_COMP                 LLDP_TLV_CHA_SUB_CHASSIS_COMP            /* 1 */
#define OHA_CHASSIS_ID_INTERFACE_ALIAS      LLDP_TLV_CHA_SUB_CHASSIS_INTERFACE_ALIAS /* 2 */
#define OHA_CHASSIS_ID_PORT                 LLDP_TLV_CHA_SUB_PORT                    /* 3 */
#define OHA_CHASSIS_ID_MAC_ADDR             LLDP_TLV_CHA_SUB_MAC_ADDR                /* 4 */
#define OHA_CHASSIS_ID_NETWORK_ADDR         LLDP_TLV_CHA_SUB_NETWORK_ADDR            /* 5 */
#define OHA_CHASSIS_ID_INTERFACE_NAME       LLDP_TLV_CHA_SUB_INTERFACE_NAME          /* 6 */
#define OHA_CHASSIS_ID_LOCALLY_ASSIGNED     LLDP_TLV_CHA_SUB_LOCALLY_ASSIGNED        /* 7 */

/* Port-Id (LLDP-Type 2) */

#define OHA_PORT_ID_INTERFACE_ALIAS         LLDP_TLV_PORT_SUB_INTERFACE_ALIAS   /* 1 */
#define OHA_PORT_ID_PORT_COMP               LLDP_TLV_PORT_SUB_PORT_COMP         /* 2 */
#define OHA_PORT_ID_MAC_ADDR                LLDP_TLV_PORT_SUB_MAC_ADDR          /* 3 */
#define OHA_PORT_ID_NETWORK_ADDR            LLDP_TLV_PORT_SUB_NETWORK_ADDR      /* 4 */
#define OHA_PORT_ID_INTERFACE_NAME          LLDP_TLV_PORT_SUB_INTERFACE_NAME    /* 5 */
#define OHA_PORT_ID_AGENT_CIRCUIT_ID        LLDP_TLV_PORT_SUB_AGENT_CIRCUIT_ID  /* 6 */
#define OHA_PORT_ID_LOCALLY_ASSIGNED        LLDP_TLV_PORT_SUB_LOCALLY_ASSIGNED  /* 7 */


/* Management Address (LLDP-Type 8) - SubType */

#define OHA_ADDR_FAMILY_IPV4                1
#define OHA_ADDR_FAMILY_MAC_ADDR            6

/* IEEE 802.1 */
#define OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1 \
( (LSA_UINT32) ( ((LSA_UINT32)0x00 << 16) + \
((LSA_UINT32)0x80 << 8 ) + \
((LSA_UINT32)0xC2  ) ))
/* IEEE 802.3 */
#define OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3 \
( (LSA_UINT32) ( ((LSA_UINT32)0x00 << 16) + \
((LSA_UINT32)0x12 << 8 ) + \
((LSA_UINT32)0x0F  ) ))

/* PNO */
#define OHA_LLDP_ORG_OUI_PNO_1    0x00
#define OHA_LLDP_ORG_OUI_PNO_1_1  0x01  /* do not send, only receive as valid PNIO_OUI 00-0e-cf */
#define OHA_LLDP_ORG_OUI_PNO_1_2  0x02  /*   -"-   */
#define OHA_LLDP_ORG_OUI_PNO_1_3  0x03  /*   -"-   */
#define OHA_LLDP_ORG_OUI_PNO_2    0x0E
#define OHA_LLDP_ORG_OUI_PNO_3    0xCF

#define OHA_LLDP_TYPE_ORG_OUI_PNO \
( (LSA_UINT32) ( ((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_1 << 16) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_2 << 8 ) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_3  ) ))
#define OHA_LLDP_TYPE_ORG_OUI_PNO_1 \
( (LSA_UINT32) ( ((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_1_1 << 16) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_2 << 8 ) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_3  ) ))
#define OHA_LLDP_TYPE_ORG_OUI_PNO_2 \
( (LSA_UINT32) ( ((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_1_2 << 16) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_2 << 8 ) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_3  ) ))
#define OHA_LLDP_TYPE_ORG_OUI_PNO_3 \
( (LSA_UINT32) ( ((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_1_3 << 16) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_2 << 8 ) + \
((LSA_UINT32)OHA_LLDP_ORG_OUI_PNO_3  ) ))

#define OHA_LLDP_SUB_TYPE_PNO_DELAY                 1
#define OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS           2
#define OHA_LLDP_SUB_TYPE_PNO_ALIAS                 3
#define OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP       4
#define OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR       5
#define OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS           6


/*===========================================================================*/
/*                                REMA                                       */
/*===========================================================================*/

/* RecordIndex (u32), API (u32), SlotNumber (u16), SubslotNumber (u16), */
/* RecordDataLength (u32), RemaCompID(u16), Reserved(u16)               */

#define OHA_REMA_RECORD_HEADER_LENGTH 20

/*===========================================================================*/
/*                       Args for RQB (Request Block)                        */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_OPEN_CHANNEL                                             */
/*---------------------------------------------------------------------------*/

#define OHA_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */


typedef LSA_VOID  LSA_FCT_PTR(OHA_UPPER_OUT_FCT_ATTR,OHA_UPPER_CALLBACK_FCT_PTR_TYPE)(OHA_UPPER_RQB_PTR_TYPE rb);

typedef struct oha_open_channel_tag
{
    LSA_HANDLE_TYPE                 handle;
    /* req: -                                */
    /* cnf: channel-handle of OHA            */
    /* At all later RQBs user will write it  */
    /* into handle of OHA_RQB_HEADER         */
    LSA_HANDLE_TYPE                 handle_upper;
    /* req: channel-handle of user           */
    /* cnf: -                                */
    /* At all confirmations OHA writes it    */
    /* into handle of OHA_RQB_HEADER.        */
    LSA_SYS_PATH_TYPE               sys_path;
    /* req: system-path of channel           */
    /* cnf: -                                */
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE oha_request_upper_done_ptr;
    /* req: pointer to callback-function     */
    /* cnf: -                                */
} OHA_CHANNEL_TYPE;

typedef struct oha_open_channel_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_OPEN_CHANNEL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_CLOSE_CHANNEL                                             */
/*---------------------------------------------------------------------------*/

/* no args */


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_GET_CONFIG                                                */
/*---------------------------------------------------------------------------*/

typedef struct oha_config_tag
{
    LSA_UINT16          NicId;                              /* out */
    LSA_UINT32          InterfaceID;                        /* out */
    LSA_UINT16          PortCnt;                            /* out */
    LSA_UINT16          MaxSender;                          /* out */
} OHA_CONFIG_TYPE;

typedef struct oha_config_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_GET_CONFIG_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_TIMER  (for OHA internal use only!)                       */
/*---------------------------------------------------------------------------*/

typedef struct oha_timer_tag
{
    LSA_UINT16              TimerID; /* LLDP-TimerID */
    LSA_UINT32              UserID;
} OHA_TIMER_TYPE;

typedef struct oha_timer_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_TIMER_PTR_TYPE;

#define OHA_TIMER_USER_ID_CYLIC    0   /* IpName-Check base timer */

/* timer for delaying the PTCP diagnosis: 1 ... (OhaCfgMaxChannels * OhaCfgMaxPorts) */


#define OHA_MAX_CHECK_INTERVAL     6   /* max. interval of cyclic check (NameOfStation or IP) */

#define OHA_CHECK_GRID             5   /* 5 seconds */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_SET_IDENT                                                 */
/*---------------------------------------------------------------------------*/

/* applicable option(s): OHA_CTRL_OEM_DEVICE_ID */

typedef struct oha_ident_tag
{
    LSA_UINT32          Options;
    LSA_UINT16          OEMVendorID;      /* OEM Identification       */
    LSA_UINT16          OEMDeviceID;
} OHA_IDENT_TYPE;

typedef struct oha_ident_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_IDENT_PTR_TYPE;

/*---------------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_GET_ADDRESS, OHA_OPC_SET_ADDRESS, OHA_OPC_SET_EMERGENCY         */
/*---------------------------------------------------------------------------------*/

/* OHA_OPC_SET_ADDRESS   - an existing IP-Suite is set as Expected IP              */
/* OHA_OPC_SET_EMERGENCY - an existing IP-Suite is set as Emergency IP             */

typedef struct oha_station_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_STATION_PTR_TYPE;

typedef struct oha_station_tag
{
    OHA_MAC_TYPE            LocalMacAddr;
    OHA_IP_TYPE             LocalIpAddr;
    OHA_IP_TYPE             LocalSubnetMask;
    OHA_IP_TYPE             DefaultRouterAddr;
    OHA_UPPER_MEM_PTR_TYPE  pStationType;
    LSA_UINT16              StationTypeLen;             /* OHA_MAX_STATION_TYPE_LEN */
    OHA_UPPER_MEM_PTR_TYPE  pStationName;               /* OHA_MAX_STATION_NAME_LEN */
    LSA_UINT16              StationNameLen;             /* OHA_MAX_STATION_NAME_LEN */
    LSA_UINT16              VendorId;                   /* Vendor-ID                */
    LSA_UINT16              DeviceId;                   /* Device-ID                */
    LSA_UINT16              TcpIpStatus;        /* status-word for TCP/IP parameter */
    LSA_UINT8               ProfinetRole;               /* 0=no role, 1=Profinet IO,*/
    LSA_UINT16              DeviceInstance;             /* DeviceInstanceValue      */
} OHA_STATION_TYPE;


typedef struct oha_address_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_ADDRESS_PTR_TYPE;

typedef struct oha_address_tag
{
    LSA_UINT32                  Options;
    LSA_BOOL                    CheckIpEnabled;         /* LSA_TRUE: Check IP       */
    LSA_UINT8                   AccessMode;
    LSA_BOOL                    Remanent;
    LSA_UINT32                  UserLockStatus;
    OHA_UPPER_STATION_PTR_TYPE  pStationData;
} OHA_ADDRESS_TYPE;


/*---------------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_ADDR_INFO_IND                                                   */
/*---------------------------------------------------------------------------------*/

typedef struct oha_addr_info_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_ADDR_INFO_PTR_TYPE;

typedef struct oha_addr_info_tag
{
    OHA_NAME_TYPE       NoS;                    /* NameOfStation (the interface) name    */
    OHA_IP_SUITE_TYPE   CurrentIPSuite;         /* the currently active IP suite */
    LSA_BOOL            CurrentIPSuiteIsEmergency; /* true if CurrentIPSuite = EmergencyIPSuite */
                                                   /* false: otherwise */
    LSA_BOOL            IpSuiteRemanent;        /* true if CurrentIPSuite is remanent    */
    /***/

    LSA_BOOL            CurrentIPSuiteConflictDetected; /* true if the CurrentIPSuite is conflicting with another station, CurrentIPSuite is reset to 0.0.0.0 */
    /* which IPSuite is conflicting?
     * if CurrentIPSuite_is_emergency == FALSE --> look into ExpectedIPSuite
     * if CurrentIPSuite_is_emergency == TRUE  --> look into EmergencyIPSuite
     */

    OHA_IP_SUITE_TYPE   ExpectedIPSuite;  /* expected  IP suite, may be 0.0.0.0 */
    OHA_IP_SUITE_TYPE   EmergencyIPSuite; /* emergency IP suite, may be 0.0.0.0 */

    LSA_BOOL            FactoryReset;       /* true: a reset to factory is done */
                                        /* false: if IP or NoS is set (default) */
} OHA_ADDR_INFO_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_DCP_READ_ELEMENT, OHA_OPC_DCP_WRITE_ELEMENT               */
/*---------------------------------------------------------------------------*/

typedef struct oha_dcp_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_DCP_ELEM_PTR_TYPE;

typedef struct oha_dcp_elem_tag
{
    LSA_UINT8                   Type;       /* type of variable (option)     */
    LSA_UINT8                   SubType;    /* type of variable (suboption)  */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable;  /* variable in octets            */
    LSA_UINT16                  Length;     /* length of above in octets     */
    LSA_UINT8                   AccCtrl;    /* access control for variable   */
    LSA_BOOL                    Remanent;   /* storage of variable in memory */
} OHA_DCP_ELEM_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_READ_ELEMENT                                         */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_LLDP_ELEM_PTR_TYPE;

typedef struct oha_lldp_elem_tag
{
    LSA_UINT16                  PortId;
    LSA_UINT8                   Type;       /* type of variable (option)     */
    LSA_UINT8                   SubType;    /* type of variable (suboption)  */
    LSA_UINT32                  OUI;        /* OUI, if subtype=127           */
    OHA_DB_LLDP_ELEM_PTR_TYPE   pElements;  /* Ptr to Array of LLDP elements */
    LSA_UINT16                  Count;      /* count of LLDP elements        */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVarBuffer; /* 1.5k-buffer for all LLDP variables */
    LSA_UINT8                   Status;     /* DATA_VALID, NOT_PRESENT,      */
    OHA_MAC_TYPE                SenderMacAddr;                        /* out */
} OHA_LLDP_ELEM_TYPE;

/*---------------------------------------------------------------------------*/
/*         OHA_OPC_HELLO_INDICATION                                          */
/*---------------------------------------------------------------------------*/

typedef struct oha_hello_indication_tag
{
    LSA_UINT32      FilterID; /* FilterID from EDD (hello filter) */  /* out */
    LSA_UINT16      StationNameLen;                                   /* in/out */
    OHA_UPPER_MEM_PTR_TYPE  pStationName;                             /* in/out  */
    OHA_IP_TYPE     IPAddr;   /* 0.0.0.0 if no IP found */            /* out */
    OHA_IP_TYPE     SubnetMask;                                       /* out */
    OHA_IP_TYPE     DefaultRouterAddr;                                /* out */
    OHA_MAC_TYPE    MacAddr;  /* 0.0.0.0.0.0 if no MAC found */       /* out */
} OHA_HELLO_INDICATION_TYPE;

typedef struct oha_hello_indication_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_HELLO_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_TEST_LLDP_CONTROL                                         */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_control_tag
{
    LSA_UINT8               Mode;                           /* in  */
} OHA_LLDP_CONTROL_TYPE;

typedef struct oha_lldp_control_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_LLDP_CONTROL_PTR_TYPE;

/* Mode: */

#define OHA_START_SEND           1  /* starts the cyclic sending of lldp-data */
#define OHA_STOP_SEND            2  /* stops the cyclic sending of lldp-data  */
#define OHA_RESET_SEND           3  /* updates the lldp-data in start and stop */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_ACCESS_CONTROL                                       */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_access_tag
{
    LSA_UINT16          PortId;         /* 0...all ports, 1...port1, 2...port2, ... */
    LSA_UINT8           AccessMode;     /* OHA_NO_ACCESS, OHA_READ_ONLY, OHA_READ_WRITE or OHA_QUERY_WRITE */
    LSA_UINT32          Options;		/* see below */
    LSA_UINT32          PortOptions;	/* see below */
} OHA_LLDP_ACCESS_TYPE;

typedef struct oha_lldp_access_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_LLDP_ACCESS_PTR_TYPE;

/* Options (see LLDP.MIB): */
#define OHA_CTRL_TX_INTERVAL			(LSA_UINT16) 0x0001 /* lldpConfiguration "lldpMessageTxInterval"    */
#define OHA_CTRL_TX_HOLD_MULTIPLIER		(LSA_UINT16) 0x0002 /* lldpConfiguration "lldpMessageTxHoldMultiplier" */
#define OHA_CTRL_REINIT_DELAY			(LSA_UINT16) 0x0004 /* lldpConfiguration "lldpReinitDelay"          */
#define OHA_CTRL_TX_DELAY				(LSA_UINT16) 0x0008 /* lldpConfiguration "lldpTxDelay"              */
#define OHA_CTRL_NOTIFICATION_INTERVAL	(LSA_UINT16) 0x0010 /* lldpConfiguration "lldpNotificationInterval" */

/* Port-Options (see LLDP.MIB, LLDP_PNO-MIB, LLDP_DOT1-MIB, LLDP_DOT3-MIB): */
#define OHA_CTRL_ADMIN_STATUS		(LSA_UINT32) 0x00000001 /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
#define OHA_CTRL_NOTIFICATION		(LSA_UINT32) 0x00000002 /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
#define OHA_CTRL_LLDP_TLVS 			(LSA_UINT32) 0x00000004 /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable"           */

#define OHA_CTRL_LLDP_MAN_ADDR		(LSA_UINT32) 0x00000008 /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
#define OHA_CTRL_PNO_DELAY 			(LSA_UINT32) 0x00000010 /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
#define OHA_CTRL_PNO_PORT_STATUS	(LSA_UINT32) 0x00000020 /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"     */
#define OHA_CTRL_PNO_ALIAS			(LSA_UINT32) 0x00000040 /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
#define OHA_CTRL_PNO_MRP			(LSA_UINT32) 0x00000080 /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
#define OHA_CTRL_PNO_PTCP			(LSA_UINT32) 0x00000100 /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
#define OHA_CTRL_DOT3_TLVS			(LSA_UINT32) 0x00000200 /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */

#define OHA_CTRL_DOT1_VLAN_ID		(LSA_UINT32) 0x00000400 /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
#define OHA_CTRL_DOT1_VLAN_NAME		(LSA_UINT32) 0x00000800 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
#define OHA_CTRL_DOT1_PROTO_VLAN	(LSA_UINT32) 0x00001000 /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
#define OHA_CTRL_DOT1_PROTOCOL		(LSA_UINT32) 0x00002000 /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_CONFIG_INDICATION, OHA_OPC_LLDP_CONFIG_RESPONSE      */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_if_param_tag
{
    LSA_UINT16     TxInterval;
    LSA_UINT16     TxHoldMultiplier;
    LSA_UINT16     ReinitDelay;
    LSA_UINT16     TxDelay;
    LSA_UINT16     NotificationInterval;
} OHA_LLDP_IF_PARAM_TYPE;

typedef struct oha_lldp_config_tag
{
    LSA_UINT8               Mode;		 /* Indication: OHA_EVENT_LOCAL_SET, OHA_EVENT_REMOTE_SET, OHA_EVENT_QUERY_SET */
										 /* Response:   OHA_MODE_SET, OHA_MODE_NO_SET */
    LSA_UINT16              Options;	 /* see OHA_OPC_LLDP_ACCESS_CONTROL */
    LSA_UINT16              RemaOptions; /* see OHA_OPC_LLDP_ACCESS_CONTROL */
    OHA_LLDP_IF_PARAM_TYPE  Config;
} OHA_LLDP_CONFIG_TYPE;

typedef struct oha_lldp_config_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_LLDP_CONFIG_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_PORT_CONFIG_INDICATION,                              */
/*         OHA_OPC_LLDP_PORT_CONFIG_RESPONSE                                 */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_port_param_tag
{
    LSA_UINT8     AdminStatus;			/* txOnly(1), rxOnly(2), txAndRx(3), disabled(4) */
    LSA_UINT8     NotificationEnable;	/* Enable(1), Disable(2) */
    LSA_UINT8     TLVsTxEnable; 		/* BITS: portDesc(0), SysName(1), sysDesc(2), sysCap(3) */
    LSA_UINT8     ManAddrTxEnable;		/* Enable(1), Disable(2) */
    LSA_UINT8     PnoDelayTxEnable;		/* Enable(1), Disable(2) */
    LSA_UINT8     PnoPortStatusTxEnable;/* Enable(1), Disable(2) */
    LSA_UINT8     PnoAliasTxEnable;		/* Enable(1), Disable(2) */
    LSA_UINT8     PnoMrpTxEnable;		/* Enable(1), Disable(2) */
    LSA_UINT8     PnoPtcpTxEnable;		/* Enable(1), Disable(2) */
    LSA_UINT8     Dot1VlanIdTxEnable;	/* Enable(1), Disable(2) */
    LSA_UINT8     Dot1VlanNameTxEnable;	/* Enable(1), Disable(2) */
    LSA_UINT8     Dot1PortProtoVlanTxEnable;/* Enable(1), Disable(2) */
    LSA_UINT8     Dot1ProtocolTxEnable;	/* Enable(1), Disable(2) */
    LSA_UINT8     Dot3TLVsTxEnable;		/* BITS: macPhyConfigStatus(0), powerViaMDI(1), */
} OHA_LLDP_PORT_PARAM_TYPE;				/*       linkAggregation(2),maxFrameSize(3)     */

typedef struct oha_lldp_port_config_tag
{
    LSA_UINT16                PortID;
    LSA_UINT8                 Mode;
    LSA_UINT32                PortOptions;	 /* see OHA_OPC_LLDP_ACCESS_CONTROL */
    LSA_UINT32                PortOptionsRema;
    OHA_LLDP_PORT_PARAM_TYPE  PortConfig;
} OHA_LLDP_PORT_CONFIG_TYPE;

typedef struct oha_lldp_port_config_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_LLDP_PORT_CONFIG_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_PORT_CONTROL                                              */
/*---------------------------------------------------------------------------*/

typedef struct oha_port_control_tag
{
    LSA_UINT16              Mode;                           /* in  */
    LSA_UINT16              PortId;                         /* in  */
} OHA_PORT_CONTROL_TYPE;

typedef struct oha_port_control_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_PORT_CONTROL_PTR_TYPE;

/* Mode: */

#define OHA_LLDP_SEND_ON           OHA_LLDP_SEND_RECV  /* enables the sending of lldp-data  */
#define OHA_LLDP_SEND_OFF          OHA_LLDP_RECV_ONLY  /* disables the sending of lldp-data */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_INDICATION                                           */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_indication_tag
{
    LSA_UINT8                   Status;
    LSA_UINT16                  PortId;
    OHA_MAC_TYPE                SenderMacAddr;
} OHA_LLDP_INDICATION_TYPE;

typedef struct oha_lldp_indication_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_LLDP_INDICATION_PTR_TYPE;

/* Stati: */

#define OHA_STATUS_NOT_PRESENT     10  /* shutdown received or TTL exceeded    */
#define OHA_STATUS_DATA_VALID      11  /* new or changed data                  */
#define OHA_STATUS_DATA_INVALID    12  /* present but no valid IDs/names       */
#define OHA_STATUS_MULTIPLE_SENDER 13  /* too many sender.                     */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_DCP_INDICATION                                            */
/*---------------------------------------------------------------------------*/

typedef struct oha_dcp_indication_tag
{
    LSA_UINT8                   Event;                          /* out */
    LSA_UINT8                   Reason;                         /* in, for OHA_OPC_DCP_RESPONSE only */
    LSA_UINT8                   Type;                           /* out (Dcp-option)    */
    LSA_UINT8                   SubType;                        /* out (Dcp-suboption) */
    LSA_UINT32                  Options;                        /* out, additional options added  */
    LSA_UINT32                  RemaOptions;                    /* out, former 'Remananent'       */
    OHA_MAC_TYPE                SenderMACAddr;                  /* out */
    LSA_BOOL                    CheckIpEnabled;                 /* in, for OHA_OPC_DCP_RESPONSE only */
    OHA_UPPER_STATION_PTR_TYPE  pStationData;                   /* in  */
    LSA_UINT16                  Count;                          /* out (count of elements to change) */
} OHA_DCP_INDICATION_TYPE;

typedef struct oha_dcp_indication_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_DCP_INDICATION_PTR_TYPE;

/* Events (continued): */

#define OHA_EVENT_REMOTE_SET      21  /* parameters of the database had been set by remote DCP-request */
#define OHA_EVENT_LOCAL_SET       22  /* parameters of the database had been set by any other user */
#define OHA_EVENT_QUERY_SET       23  /* ther's a DcpSet to respond by the control-user */
#define OHA_EVENT_CONTROL         24  /* DCP-Control - Options            */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_DCP_RESPONSE                                              */
/*---------------------------------------------------------------------------*/

#define OHA_MODE_SET             71  /* the options are to set in the DataBase       */
/* the remote DCP-Server is responded positive  */
/* an IP-Suite is stored as Expected IP-Suite   */
#define OHA_MODE_NO_SET          72  /* the options are not to set in the DataBase   */
/* the remote DCP-Server is responded negative  */
#define OHA_MODE_EMERGENCY_SET   73  /* like OHA_MODE_SET, an IP-suite, if existing, */
/* is stored as Emergency IP-Suite              */
#define OHA_MODE_EXTERNAL_SET    74  /* the options are NOT set in the DataBase      */
/* the remote DCP-Server is responded positive  */

/* error codes of the DCP protocol, which are sent to the client                     */
#define OHA_DCP_REASON_OK              DCP_RESULT_OK          /* No error, parameter delivered and accepted */
#define OHA_DCP_REASON_NO_DATABASE     DCP_RESULT_NO_DATABASE /* Option not supported                    */
#define OHA_DCP_REASON_NO_DATASET      DCP_RESULT_NO_DATASET  /* Suboption not supported or no dataset available */
#define OHA_DCP_REASON_DS_NOT_SET      DCP_RESULT_DS_NOT_SET  /* Default value, Suboption not set (local reasons) */
#define OHA_DCP_REASON_NO_RESOURCE     DCP_RESULT_NO_RESOURCE /* (temporary) resource error (server)     */
#define OHA_DCP_REASON_NOT_POSSIBLE    DCP_RESULT_SET_NOT_POSSIBLE   /* SET not possible (local reasons)        */
#define OHA_DCP_REASON_NOT_APPLICABLE  DCP_RESULT_SET_NOT_APPLICABLE /* SET not possible (application operation) */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_DCP_CONTROL                                               */
/*---------------------------------------------------------------------------*/

typedef struct oha_dcp_control_tag
{
    LSA_UINT8                      Mode;                           /* in  */
    LSA_UINT32                     Options;                        /* in/out */
    LSA_UINT8                      Type;                           /* in (Dcp-option)    */
    LSA_UINT8                      SubType;                        /* in (Dcp-suboption) */
	OHA_UPPER_ADDR_INFO_PTR_TYPE   pAddrInfo;                      /* out */
} OHA_DCP_CONTROL_TYPE;

typedef struct oha_dcp_control_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_DCP_CONTROL_PTR_TYPE;

/* Mode: */

#define OHA_SET_FILTER            21  /* set a frame filter         */
#define OHA_RESET_FILTER          22  /* delete a frame filter      */
#define OHA_LOCK_SET              31  /* user lock for a Set        */
#define OHA_UNLOCK_SET            32  /* user unlock for a Set      */
#define OHA_LOCK_GET              41  /* user lock for a Get        */
#define OHA_UNLOCK_GET            42  /* user unlock for a Get      */
#define OHA_SHOW_LOCATION         51  /* LED flashing               */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_READ_QUERY_ELEMENTS                                       */
/*---------------------------------------------------------------------------*/

typedef struct oha_query_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_QUERY_ELEM_PTR_TYPE;

typedef struct oha_query_elem_tag
{
    LSA_UINT16                  Count;      /* count of pElements           */
    OHA_DB_ELEM_PTR_TYPE        pElements;  /* Ptr to Array of DCP elements */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVarBuffer; /* 1.5k-buffer for all DCP variables */
} OHA_QUERY_ELEM_TYPE;

/* Options, Query Options, SetStatus and Remanence - values: */

#define OHA_CTRL_MAC_ADDR               (LSA_UINT32) 0x00000001
#define OHA_CTRL_IP_SUITE               (LSA_UINT32) 0x00000002
#define OHA_CTRL_STATION_ID             (LSA_UINT32) 0x00000004
#define OHA_CTRL_STATION_NAME           (LSA_UINT32) 0x00000008
#define OHA_CTRL_STATION_TYPE           (LSA_UINT32) 0x00000010
#define OHA_CTRL_DHCP_OPTION            (LSA_UINT32) 0x00000020
#define OHA_CTRL_PROFIBUS_COMMON        (LSA_UINT32) 0x00000040
#define OHA_CTRL_PROFIBUS_ADDITION      (LSA_UINT32) 0x00000080
                                               /* AP01520094: OrangeBox-Options (DCP) removed */
#define OHA_CTRL_TCIP_STATUS            (LSA_UINT32) 0x00000800 /*it's no DCP option */
#define OHA_CTRL_RESET_TO_FACTORY       (LSA_UINT32) 0x00001000

#define OHA_CTRL_PNIO_ROLE              (LSA_UINT32) 0x00004000

#define OHA_CTRL_ANY_OTHER_OPTION       (LSA_UINT32) 0x00008000

/* AP01534406: OHA_CTRL_TRANS_END (end transaction) removed */
/* TFS 852764: OHA_CTRL_RESPONSE removed, Suboption 凴esponse?only for coding of  DCP-ResPDU, see PNIO draft */
#define OHA_CTRL_RESET_COMMUNICATION    (LSA_UINT32) 0x00040000 /* Reset communication param. */
#define OHA_CTRL_DEVICE_INSTANCE        (LSA_UINT32) 0x00080000 /* DeviceInstanceValue        */
#define OHA_CTRL_OEM_DEVICE_ID          (LSA_UINT32) 0x00100000 /* OEM Identification         */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_GET_NEIGHBOUR                                             */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldpdu_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_LLDPDU_PTR_TYPE;

typedef struct oha_lldpdu_tag
{
    OHA_UPPER_MEM_U8_PTR_TYPE pNameOfChassis;       /* ChassisID TLV, NameOfChassis */
    LSA_UINT16              ChassisNameLen;     /* max. 255 bytes OHA_MAX_CHASSIS_STRLEN */
    LSA_UINT8               TypeOfChassis;      /* chassis ID subtype          */
    LSA_UINT16              PortNameLen;        /* max. 255 bytes OHA_MAX_PORT_STRLEN */
    LSA_UINT8               TypeOfPort;         /* port ID subtype             */
    OHA_UPPER_MEM_U8_PTR_TYPE pNameOfPort;        /* Port-ID, e.g. "Port-001"    */
    LSA_UINT16              TimeToLive;         /* TTL in seconds              */
} OHA_LLDPDU_TYPE;

typedef struct oha_neighbour_tag
{
    LSA_UINT16                  PortId;
    LSA_UINT8                   Status;
    OHA_MAC_TYPE                SenderMacAddr;
    OHA_UPPER_LLDPDU_PTR_TYPE   pLLDPDU;
} OHA_NEIGHBOUR_TYPE;

typedef struct oha_neighbour_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_GET_NEIGHBOUR_PTR_TYPE;

/* Stati: see OHA_OPC_LLDP_INDICATION */

/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_CONTROL_OID, OHA_OPC_MRP_CONTROL_OID                 */
/*---------------------------------------------------------------------------*/

typedef struct oha_oid_tag
{
    OHA_OID_PTR_TYPE            pOidName;               /* in / out(getnext) */
    LSA_UINT32                  OidNameLen;             /* in / out(getnext) */
    LSA_UINT8                   VarType;
    LSA_UINT32                  VarLength;
    OHA_UPPER_MEM_U8_PTR_TYPE   pVarValue;
    LSA_UINT16                  AccessControl;
    LSA_UINT8                   MessageType;
    LSA_UINT32                  SnmpError;
    LSA_INT32                   SnmpVersion;
} OHA_OID_TYPE;

typedef struct oha_oid_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_GET_OID_PTR_TYPE;


/* Node: */
#define OHA_LLDP_MIB_NODE            8802  /* Node: iso8802 defined in LLDP-MIB */
#define OHA_MRP_MIB_NODE            62439  /* Node: iec62439 defined in MRP-MIB */


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_WRITE_OPTION                                         */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_option_tag  OHA_COMMON_MEM_ATTR *   OHA_LLDP_OPTION_PTR_TYPE;

typedef struct oha_lldp_option_tag
{
    LSA_UINT16                  PortId;
    LSA_UINT8                   Type;       /* type of variable (option)     */
    LSA_UINT8                   SubType;    /* type of variable (suboption)  */
    LSA_UINT32                  OUI;        /* OUI, if subtype=127           */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable;  /* variable in octets            */
    LSA_UINT16                  Length;     /* length of variable octets     */
} OHA_LLDP_OPTION_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_LLDP_READ_OPTIONS                                         */
/*---------------------------------------------------------------------------*/

typedef struct oha_lldp_options_tag  OHA_COMMON_MEM_ATTR *   OHA_LLDP_OPTIONS_PTR_TYPE;

typedef struct oha_lldp_options_tag
{
    LSA_UINT16                  PortId;
    LSA_UINT8                   Type;       /* type of variable (option)     */
    LSA_UINT32                  OUI;        /* OUI, if subtype=127           */
    LSA_UINT8                   SubType;    /* type of variable (suboption)  */
    OHA_DB_LLDP_ELEM_PTR_TYPE   pElements;  /* Ptr to Array of LLDP elements */
    LSA_UINT16                  Count;      /* count of pElements            */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVarBuffer; /* 1.5k-buffer for all LLDP variables */
} OHA_LLDP_OPTIONS_TYPE;


/*===========================================================================*/
/*                 general data of TOPO (expected)                           */
/*===========================================================================*/

/* valid MAUTypes in PNIO */

#define OHA_MAU_TYPE_10BASET        LLDP_TLV_MAU_TYPE_10BASET       /* UTP MAU, duplex mode unknown      */

#define OHA_MAU_TYPE_10BASETHD      LLDP_TLV_MAU_TYPE_10BASETHD     /* 10 Mbit/s, half duplex mode (CU)  */
#define OHA_MAU_TYPE_10BASETFD      LLDP_TLV_MAU_TYPE_10BASETFD     /* 10 Mbit/s, full duplex mode (CU)  */
#define OHA_MAU_TYPE_10BASEFLHD     LLDP_TLV_MAU_TYPE_10BASEFLHD    /* async fiber 10Mbit/s, half duplex */
#define OHA_MAU_TYPE_10BASEFLFD     LLDP_TLV_MAU_TYPE_10BASEFLFD    /* async fiber10Mbit/s, full duplex  */

#define OHA_MAU_TYPE_100BASETXFD    LLDP_TLV_MAU_TYPE_100BASETXFD   /* 100 MBit Full-Duplex CU (Default) */
#define OHA_MAU_TYPE_100BASEFXFD    LLDP_TLV_MAU_TYPE_100BASEFXFD   /* 100 MBit Full-Duplex Fiber        */
#define OHA_MAU_TYPE_1000BASEXFD    LLDP_TLV_MAU_TYPE_1000BASEXFD   /*   1 GBit Full-Duplex unknown      */
#define OHA_MAU_TYPE_1000BASELXFD   LLDP_TLV_MAU_TYPE_1000BASELXFD  /*   1 GBit Full-Duplex Fiber, long  */
#define OHA_MAU_TYPE_1000BASESXFD   LLDP_TLV_MAU_TYPE_1000BASESXFD  /*   1 GBit Full-Duplex Fiber, short */
#define OHA_MAU_TYPE_1000BASETFD    LLDP_TLV_MAU_TYPE_1000BASETFD   /*   1 GBit Full-Duplex UTP          */

#define OHA_MAU_TYPE_10GIGBASEFX    LLDP_TLV_MAU_TYPE_10GIGBASEX    /*  10 GBit Fiber                    */

#define OHA_MAU_TYPE_100BASETXHD    LLDP_TLV_MAU_TYPE_100BASETXHD   /* 100 MBit Half-Duplex CU           */
#define OHA_MAU_TYPE_100BASEFXHD    LLDP_TLV_MAU_TYPE_100BASEFXHD   /* 100 MBit Half-Duplex Fiber        */
#define OHA_MAU_TYPE_1000BASEXHD    LLDP_TLV_MAU_TYPE_1000BASEXHD   /*   1 GBit Half-Duplex unknown      */
#define OHA_MAU_TYPE_1000BASELXHD   LLDP_TLV_MAU_TYPE_1000BASELXHD  /*   1 GBit Half-Duplex Fiber, long  */
#define OHA_MAU_TYPE_1000BASESXHD   LLDP_TLV_MAU_TYPE_1000BASESXHD  /*   1 GBit Half-Duplex Fiber, short */
#define OHA_MAU_TYPE_1000BASETHD    LLDP_TLV_MAU_TYPE_1000BASETHD   /*   1 GBit Half-Duplex UTP          */

#define OHA_MAU_TYPE_100BASELX10    0x2E                            /* 100 MBit Full-Duplex Fiber, long  */
#define OHA_MAU_TYPE_1000BASELX10   0x31                            /*   1 GBit Full-Duplex Fiber, long  */
#define OHA_MAU_TYPE_100BASEPXFD    0x36                            /* 100 MBit Full-Duplex POF-Fiber    */


/*===========================================================================*/
/*                 TOPO functions                                            */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* helper functions for reading PDEV records                                 */
/*---------------------------------------------------------------------------*/

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay);

LSA_EXTERN LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_Peers_PdPortDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNextPeers,
    OHA_COMMON_MEM_U16_PTR_TYPE Port_Len,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPort,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis);

LSA_EXTERN LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pNumberOfPeers,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPeerRecord,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppFirstPeer,
    OHA_COMMON_MEM_U16_PTR_TYPE pMAUType,
    LSA_BOOL OHA_LOCAL_MEM_ATTR * pDelayExisting,
    OHA_COMMON_MEM_U32_PTR_TYPE pLineDelay,
    OHA_COMMON_MEM_U16_PTR_TYPE pLinkState,
    OHA_COMMON_MEM_U32_PTR_TYPE pMediaType,
    OHA_COMMON_MEM_U16_PTR_TYPE pRTClass3PortStatus);

LSA_EXTERN LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_Peers_PdPortDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNextPeers,
    OHA_COMMON_MEM_U16_PTR_TYPE Port_Len,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPort,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis);

/* MRP_DomainUUID:                                      */
/*  uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)          */
/*     00000000-0000-0000-0000-000000000000  - reserved */
/*     00000000-0000-0000-0000-000000000001 to          */
/*     FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFE  - unique   */
/*     FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF  - default  */

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PdPortMrpDataExpected(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecordData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppDomainUUID);

LSA_EXTERN LSA_BOOL OHA_UPPER_IN_FCT_ATTR OHA_Get_PdInterfaceDataReal(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE Chassis_Len,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppChassis,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppMacAddr,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr);

LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_FastStartUpData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloMode,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloInterval,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloRetry,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloDelay);

/*---------------------------------------------------------------------------*/
/* helper function for reading multiple PDEV-records                         */
/*---------------------------------------------------------------------------*/
/*  Description: gets a single PDEVRecord from a MultiplePDEV-record         */
/*               depending on the given RecordIndex                          */
/*                                                                           */
/*  Note:  Some single PDEV records can be further read with the             */
/*         following OHA helper functions depending on the RecordIndex.      */
/*                                                                           */
/*  OHA_RECORD_INDEX_PD_PORT_DATA_CHECK     OHA_Get_PdPortDataExpected()     */
/*  OHA_RECORD_INDEX_PD_PORT_DATA_REAL      OHA_Get_PdPortDataReal()	     */
/*  OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL  OHA_Get_PdPortMrpDataExpected()  */
/*  OHA_RECORD_INDEX_PD_INTERFACE_DATA_REAL OHA_Get_PdInterfaceDataReal()    */
/*  OHA_RECORD_INDEX_FAST_STARTUP           OHA_Get_FastStartUpData()        */
/*  OHA_RECORD_INDEX_LLDP_INTERFACE_DATA    OHA_Get_PrmLLDPInterfaceData()   */
/*  OHA_RECORD_INDEX_LLDP_PORT_DATA         OHA_Get_PrmLLDPPortData()        */
/*  OHA_RECORD_INDEX_MULTIPLE_BUS           OHA_Get_PrmBUSData()             */
/*  OHA_RECORD_INDEX_BUS_COMMON             OHA_Get_PrmBusDataCommon()       */
/*  OHA_RECORD_INDEX_BUS_ADDITION           OHA_Get_PrmBusDataAddition()     */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmPDEVData(
    OHA_UPPER_MEM_U8_PTR_TYPE					pRecordData,	/* IN */
    LSA_UINT32									RecordLen,		/* IN */
	LSA_UINT16									RecordIndex,	/* IN */
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppPDevData,	/* OUT */
    OHA_COMMON_MEM_U32_PTR_TYPE					pPDevLen);		/* OUT */

/*---------------------------------------------------------------------------*/
/* helper function for reading multiple Station-records                      */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE Options,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

/*---------------------------------------------------------------------------*/
/* helper function for reading multiple SNMP-records                         */
/*---------------------------------------------------------------------------*/

/*------------------------ retrieve MIB2 objects ----------------------------*/
/*   Description: get SysName, SysLocation and SysContact from a             */
/*                MultipleSNMP-record.                                       */
/*                If an object does not exist, a Null-Pointer is returned.   */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysName,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysNameLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysContact,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysContactLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSysLocation,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysLocationLen);

/*---------------------------------------------------------------------------*/
/* helper function for reading multiple BusParams-records                    */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmBUSData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pOptions,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppCommonBus,
    OHA_COMMON_MEM_U16_PTR_TYPE pCommonBusLength,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppAdditionBus,
    OHA_COMMON_MEM_U16_PTR_TYPE pAdditionBusLength);

/*------------------------ retrieve SNMP subrecords -------------------------*/
/*   Description: gets a single SNMP Record from a Multiple SNMP-record     +*/
/*                depending on the given RecordIndex                        +*/
/*---------------------------------------------------------------------------*/
/*+  Note:  Some single SNMP records can be further read with the           +*/
/*+         following OHA helper functions depending on the RecordIndex.    +*/
/*+                                                                         +*/
/*+  OHA_RECORD_INDEX_LLDP_INTERFACE_DATA OHA_Get_PrmLLDPInterfaceData()    +*/
/*+  OHA_RECORD_INDEX_LLDP_PORT_DATA      OHA_Get_PrmLLDPPortData()         +*/
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPRecord(
    OHA_UPPER_MEM_U8_PTR_TYPE					pRecordData,	/* IN */
    LSA_UINT32									RecordLen,		/* IN */
	LSA_UINT32									RecordIndex,	/* IN */
	LSA_UINT16									PortID,	        /* IN */
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppSnmpData,	/* OUT */
    OHA_COMMON_MEM_U32_PTR_TYPE					pSnmpLen);      /* OUT */

/*---------------------------------------------------------------------------*/
/* helper function for reading LLDP-interface subrecords                     */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmLLDPInterfaceData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pIfOptions,
	OHA_LLDP_IF_PARAM_TYPE OHA_COMMON_MEM_ATTR * pIfConfig);

/*---------------------------------------------------------------------------*/
/* helper function for reading LLDP-port subrecords                          */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmLLDPPortData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pPortOptions,
	OHA_LLDP_PORT_PARAM_TYPE OHA_COMMON_MEM_ATTR * pPortConfig);

/*---------------------------------------------------------------------------*/
/* helper function for reading TCPKeepAliveRecord                            */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmTCIPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pKeepAliveTime);

/*---------------------------------------------------------------------------*/
/* helper function for reading SnmpControlRecord                             */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmSNMPControlData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pSnmpControl);

/*---------------------------------------------------------------------------*/
/* helper function for reading NameOfStationRecord                           */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmNameOfStationData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppNameOfStation,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

/*---------------------------------------------------------------------------*/
/* helper function for reading IPSuiteRecord                                 */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmIpSuiteData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppIpAddr,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

/*---------------------------------------------------------------------------*/
/* helper function for reading PDInterfaceAdjust                             */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmIfAdjustData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U32_PTR_TYPE pMultipleInterfaceMode);

/*---------------------------------------------------------------------------*/
/* helper function for reading PDIdentification record                       */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmPnIdentData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U16_PTR_TYPE pVendorID,
    OHA_COMMON_MEM_U16_PTR_TYPE pDeviceID,
    OHA_COMMON_MEM_U16_PTR_TYPE pInstanceID);

/*---------------------------------------------------------------------------*/
/* helper function for reading DHCPParameters                                */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmDHCPData(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_COMMON_MEM_U8_PTR_TYPE  pOptionCode,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppClientID,
    OHA_COMMON_MEM_U8_PTR_TYPE  pClientIDLen);

/*---------------------------------------------------------------------------*/
/* helper function for reading CommonBusParameters                           */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_UPPER_IN_FCT_ATTR OHA_Get_PrmBusDataCommon(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppBusData,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

/*---------------------------------------------------------------------------*/
/* helper function for reading AdditionalBusParameters                       */
/*---------------------------------------------------------------------------*/
LSA_EXTERN LSA_UINT16 OHA_Get_PrmBusDataAddition(
    OHA_UPPER_MEM_U8_PTR_TYPE   pRecData,
    LSA_UINT32                  RecordLen,
    OHA_UPPER_MEM_U8_PTR_TYPE OHA_COMMON_MEM_ATTR * ppBusData,
    OHA_COMMON_MEM_U16_PTR_TYPE pLength);

#define OHA_UUID_SIZE   16

typedef LSA_UINT8 OHA_UUID_TYPE[OHA_UUID_SIZE];
typedef OHA_UUID_TYPE OHA_UPPER_MEM_ATTR * OHA_UPPER_UUID_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_WRITE_PTCP_DATA                                           */
/*---------------------------------------------------------------------------*/

/* DoSync: */

/* LSA_TRUE  := write LLDP-TLV LLDP_PNIO_PTCPSTATUS to database and send if active */
/* LSA_FALSE := remove the LLDP-TLV from the database and update send if active */

typedef struct oha_ptcp_data_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_PTCP_DATA_PTR_TYPE;

typedef struct oha_ptcp_data_tag
{
    LSA_BOOL         DoSync;              /* IN */ /* write/remove TLV, see above */
    LSA_BOOL         IsSync;              /* IN */ /* := LSA_FALSE if not sync */
    OHA_MAC_TYPE     MasterSourceAddress; /* IN */ /* := 0 if not sync         */
} OHA_PTCP_DATA_TYPE;


/*---------------------------------------------------------------------------*/
/* Opcode: OHA_OPC_TCIP_CONTROL (see SOCK_OPC_CNTRL_TIMEPARAMS)              */
/*---------------------------------------------------------------------------*/

/* TimeOption: */
#define OHA_GET_TIME                 SOCK_GET_TIMEPARAMS
#define OHA_SET_TIME                 SOCK_SET_TIMEPARAMS

#define OHA_RESET_TIME               SOCK_SET_TIMEPARAMS_CLOSE_TCP_CONN

#define OHA_NO_CHANGE_TIME           SOCK_NO_CHANGE_TIMEPARMS
#define OHA_KEEPALIVE_OFF            SOCK_KEEPALIVE_OFF

typedef struct oha_tcip_data_tag OHA_UPPER_MEM_ATTR *  OHA_UPPER_TCIP_DATA_PTR_TYPE;

typedef struct oha_tcip_data_tag
{
	LSA_UINT16       TimeOption;        /* IN */
    LSA_UINT32       KeepAliveTime;		/* IN,OUT */ /* keep alive time in ms    */
    LSA_UINT32       ConnectionTimeout;	/* IN,OUT */ /* connection timeout in ms */
    LSA_UINT32       RetransmitTimeout;	/* IN,OUT */ /* retransmit timeout in ms */
    LSA_UINT32       RetransmitCount;	/* IN,OUT */ /* retransmit count         */
} OHA_TCIP_DATA_TYPE;


/*---------------------------------------------------------------------------
    from IEC 61156 Part 6
  ---------------------------------------------------------------------------*/

/* record index PDPortDataReal for one subslot     */
#define OHA_RECORD_INDEX_PD_PORT_DATA_REAL          0x802A

/* record index  PDPortDataCheck for one subslot   */
#define OHA_RECORD_INDEX_PD_PORT_DATA_CHECK         0x802B

#if 0
/* record index PDPortMrpDataAdjust for one subslot  */
#define OHA_RECORD_INDEX_PD_PORT_MRP_DATA_ADJUST    0x8053
#endif

/* record index PDPortMrpDataReal for one subslot   */
#define OHA_RECORD_INDEX_PD_PORT_MRP_DATA_REAL      0x8054

/* record index  PDPortDataAdjust for one subslot   */
#define OHA_RECORD_INDEX_PD_PORT_DATA_ADJUST        0x802F

/* record index PDInterfaceAdjust for one subslot */
#define OHA_RECORD_INDEX_PD_INTERFACE_ADJUST        0x8071

/* record index PDInterfaceDataReal for one subslot */
#define OHA_RECORD_INDEX_PD_INTERFACE_DATA_REAL     0x8080

/* record index PDIRData for one subslot     */
#define OHA_RECORD_INDEX_PD_IR_DATA                 0x802C

/* Expected PDSyncData with SyncID value 0 */
#define OHA_RECORD_INDEX_PD_SYNC_DATA               0x802D

/* record index 0x8090 - Expected PDInterfaceFSUDataAdjust */
#define OHA_RECORD_INDEX_FAST_STARTUP               0x8090

/* record index 0xB070 - PDInterfaceAdditionalInfo */
#define OHA_RECORD_INDEX_PD_IF_ADDITIONAL_INFO      0xB070

/* record index SnmpControl for one subslot */
#define OHA_RECORD_INDEX_SNMP_CONTROL               0xB071

/* record index 0xF8000000 MultiplePDev */
#define OHA_RECORD_INDEX_MULTIPLE_PDEV              0xF8000000

/* record index 0xF8000001 MultipleStation */
#define OHA_RECORD_INDEX_MULTIPLE_STATION           0xF8000001

/* record index 0xF8000002 MultipleSNMP */
#define OHA_RECORD_INDEX_MULTIPLE_SNMP              0xF8000002

/* record index 0xF8000003 MultipleBusParams */
#define OHA_RECORD_INDEX_MULTIPLE_BUS               0xF8000003

/* record index 0x00010100  DcpNameOfStationRecord */
#define OHA_RECORD_INDEX_DCP_NAME_OF_STATION        0x00010100

/* record index 0x00010101  DcpIPSuiteRecord */
#define OHA_RECORD_INDEX_DCP_IP_SUITE               0x00010101

/* record index 0x00023003  NameOfStationRecord */
#define OHA_RECORD_INDEX_NAME_OF_STATION            0x00023003

/* record index 0x00023000  IPSuiteRecord */
#define OHA_RECORD_INDEX_IP_SUITE                   0x00023000

/* record index 0x00023002  DHCPParametersRecord */
#define OHA_RECORD_INDEX_DHCP_PARAMS                0x00023002

/* record index 0x00011000  SNMP */
#define OHA_RECORD_INDEX_SNMP                       0x00011000

/* record index 0x00011001  SysName */
#define OHA_RECORD_INDEX_SYS_NAME                   0x00011001

/* record index 0x00011002  SysContact */
#define OHA_RECORD_INDEX_SYS_CONTACT                0x00011002

/* record index 0x00011003  SysLocation */
#define OHA_RECORD_INDEX_SYS_LOCATION               0x00011003

/* record index 0x00011004 LldpInterfaceRecord */
#define OHA_RECORD_INDEX_LLDP_INTERFACE_DATA        0x00011004

/* record index 0x00011005 LldpInterfaceDefaultRecord */
#define OHA_RECORD_INDEX_LLDP_INTERFACE_DEFAULT_DATA 0x00011005

/* record index 0x00011006 LldpPortRecord */
#define OHA_RECORD_INDEX_LLDP_PORT_DATA             0x00011006

/* record index 0x00011007 LldpPortDefaultRecord */
#define OHA_RECORD_INDEX_LLDP_PORT_DEFAULT_DATA     0x00011007

/* record index 0x00023008 TCPKeepAliveRecord */
#define OHA_RECORD_INDEX_TCP_KEEP_ALIVE             0x00023008

/* record index 0x00023100 PNIdentificationRecord */
#define OHA_RECORD_INDEX_PN_IDENTIFICATION          0x00023100

/* record index 0x00023110 CommonBusParamsRecord */
#define OHA_RECORD_INDEX_BUS_COMMON                 0x00023110

/* record index 0x00023111 AdditionalBusParamsRecord */
#define OHA_RECORD_INDEX_BUS_ADDITION               0x00023111


/* BlockType: 0x0608 - PDInterfaceFSUDataAdjust (FastStartUp)  */
#define OHA_BLOCK_TYPE_PD_IF_FSU_DATA_ADJUST        0x0608
/* BlockType: 0x0609 - ARFSUDataAdjust (FastStartUp)  */
#define OHA_BLOCK_TYPE_PD_AR_FSU_DATA_ADJUST        0x0609

/* BlockType: 0x0600 - FSHelloBlock (FastStartUp)  */
#define OHA_BLOCK_TYPE_FAST_STARTUP_HELLO           0x0600
#define OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_BEGIN  0x0603
#define OHA_BLOCK_TYPE_FAST_STARTUP_RESERVED_END    0x060F

/* BlockHeader (CheckPeers, CheckPropagationDelayFactor, CheckMAUType,              */
/*              CheckLinkState, CheckSyncDifference, CheckMAUTypeDifference):       */
/*  BlockType (u16), BlockLength (u16), BlockVersionHigh (u8), BlockVersionLow (u8) */

#define OHA_TOPO_BLOCK_HEADER_PDU_SIZE \
(sizeof(LSA_UINT16) + sizeof(LSA_UINT16) + sizeof(LSA_UINT16))

/* BlockType: Sub block for reading MRP port data (PDPortMrpDataReal)       */
#define OHA_BLOCK_TYPE_PD_PORT_MRP_DATA_REAL    0x0215

/* BlockType: Sub block for RecordDataRead/Write PDPortDataReal             */
#define OHA_BLOCK_TYPE_PD_PORT_DATA_REAL        0x020F

/* BlockType: Sub block for RecordDataRead/Write PDPortDataCheck            */
#define OHA_BLOCK_TYPE_PD_PORT_DATA_CHECK       0x0200

/* BlockType: Sub block for RecordDataRead/Write PDSyncData                 */
#define OHA_BLOCK_TYPE_PD_SYNC_DATA             0x0203

/* BlockType: Sub block for RecordDataRead PDInterfaceDataReal              */
#define OHA_BLOCK_TYPE_PD_INTERFACE_DATA_REAL   0x0240

/* BlockType: Sub block PDIRData            */
#define OHA_BLOCK_TYPE_PD_IR_DATA               0x0205

/* BlockType: Sub block PDIRGlobalData      */
#define OHA_BLOCK_TYPE_PDIR_GLOBAL_DATA         0x0206


/***** BlockTypes (LldpInterface-Record) *****/
/* BlockType: Sub block LldpTxInterval       */
#define OHA_BLOCK_TYPE_LLDP_TX_INTERVAL           0x7000
/* BlockType: Sub block LldpTxHoldMultiplier */
#define OHA_BLOCK_TYPE_LLDP_TX_HOLD_MULTIPLIER    0x7001
/* BlockType: Sub block LldpReinitDelay      */
#define OHA_BLOCK_TYPE_LLDP_REINIT_DELAY          0x7002
/* BlockType: Sub block LldpTxDelay          */
#define OHA_BLOCK_TYPE_LLDP_TX_DELAY              0x7003
/* BlockType: Sub block LldpNotificationInterval */
#define OHA_BLOCK_TYPE_LLDP_NOTIFICATION_INTERVAL 0x7004


/***** BlockTypes (LldpPort-Record) *****/
/* BlockType: Sub block AdminStatus	         */
#define OHA_BLOCK_TYPE_LLDP_ADMIN_STATUS          0x7005
/* BlockType: Sub block NotificationEnable   */
#define OHA_BLOCK_TYPE_LLDP_NOTIFICATION          0x7006
/* BlockType: Sub block TLVsTxEnable      */
#define OHA_BLOCK_TYPE_LLDP_TLVS_TX				  0x7007
/* BlockType: Sub block ManAddrTxEnable      */
#define OHA_BLOCK_TYPE_LLDP_MAN_ADDR_TX           0x7008
/* BlockType: Sub block Dot3TLVsTxEnable      */
#define OHA_BLOCK_TYPE_DOT3_TLVS_TX				  0x7009
/* BlockType: Sub block Dot1VlanIdTxEnable   */
#define OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_ID_TX       0x700A
/* BlockType: Sub block Dot1VlanNameTxEnable */
#define OHA_BLOCK_TYPE_LLDP_DOT1_VLAN_NAME_TX     0x700B
/* BlockType: Sub block Dot1PortProtoVlanTxEnable */
#define OHA_BLOCK_TYPE_LLDP_DOT1_PORT_PROTO_TX    0x700C
/* BlockType: Sub block Dot1ProtocolTxEnable */
#define OHA_BLOCK_TYPE_LLDP_DOT1_PROTOCOL_TX      0x700D

/* BlockType: Sub block PnoDelayTxEnable	 */
#define OHA_BLOCK_TYPE_LLDP_PNO_DELAY_TX          0x700E
/* BlockType: Sub block PnoPortStatusTxEnable*/
#define OHA_BLOCK_TYPE_LLDP_PNO_PORT_STATUS_TX    0x700F
/* BlockType: Sub block PnoAliasTxEnable     */
#define OHA_BLOCK_TYPE_LLDP_PNO_ALIAS_TX          0x7010
/* BlockType: Sub block PnoMrpTxEnable       */
#define OHA_BLOCK_TYPE_LLDP_PNO_MRP_TX            0x7011
/* BlockType: Sub block PnoPtcpTxEnable	     */
#define OHA_BLOCK_TYPE_LLDP_PNO_PTCP_TX           0x7012

/* BlockType: Sub block StationNameBlock	 */
#define OHA_BLOCK_TYPE_NAME_OF_STATION            0xA201

/* BlockType: Sub block IPSuiteBlock         */
#define OHA_BLOCK_TYPE_IP_SUITE                   0x3000

/* BlockType: Sub block DHCPParameters       */
#define OHA_BLOCK_TYPE_DHCP_PARAMS                0x3008

/* BlockType: Sub block KeepAliveTimeBlock	 */
#define OHA_BLOCK_TYPE_TCP_KEEP_ALIVE_TIME        0x3012

/* BlockType: Sub block PDInterfaceAdjust    */
#define OHA_BLOCK_TYPE_PD_INTERFACE_ADJUST        0x0250

/* BlockType: Sub block PNIdentification    */
#define OHA_BLOCK_TYPE_PN_IDENTIFICATION          0xF001

/* BlockType: Sub block SnmpControl        */
#define OHA_BLOCK_TYPE_SNMP_CONTROL               0xF003

/* BlockType: Sub block CommonBusParameters */
#define OHA_BLOCK_TYPE_BUS_COMMON                 0x7020

/* BlockType: Sub block AdditionalBusParameters */
#define OHA_BLOCK_TYPE_BUS_ADDITION               0x7021


/* BlockType: Sub block for checking Peers  PDPortDataCheck                 */
#define OHA_TOPO_CHECK_PEERS                0x020A
/* BlockType: Sub block for checking MAUType    PDPortDataCheck             */
#define OHA_TOPO_CHECK_MAU_TYPE             0x020C
/* BlockType: Sub block for checking LineDelayFactor    PDPortDataCheck     */
#define OHA_TOPO_CHECK_DELAY                0x020B
/* BlockType: Sub block for checking DomainBoundary PDPortDataCheck         */
#define OHA_TOPO_CHECK_DOMAIN_BOUNDARY      0x020D
/* BlockType: Sub block for checking MulticastBoundary PDPortDataCheck      */
#define OHA_TOPO_CHECK_MULTICAST_BOUNDARY   0x0211

/* BlockType: Sub block for checking LinkState PDPortDataCheck              */
#define OHA_TOPO_CHECK_LINK_STATE           0x021C

/* BlockType: Sub block for checking local and remote CableDelay            */
#define OHA_TOPO_CHECK_DELAY_DIFFERENCE     0x021E

/* BlockType: Sub block for checking local and remote MAUTypes              */
#define OHA_TOPO_CHECK_MAU_TYPE_DIFFERENCE  0x021F

/* BlockType: Sub block for RecordDataRead/Write PDPortDataAdjust           */
#define OHA_BLOCK_TYPE_PD_PORT_DATA_ADJUST      0x0202

#if 0
/* BlockType: Sub block for adjusting MRP port data (PDPortMrpDataAdjust)   */
#define OHA_BLOCK_TYPE_PD_PORT_MRP_DATA_ADJUST  0x0214
#endif
/*  Sub block for adjusting DomainBoundary (PDPortDataAdjust)               */
#define OHA_TOPO_ADJUST_DOMAIN_BOUNDARY         0x0209
/*  Sub block for adjusting MulticastBoundary (PDPortDataAdjust)            */
#define OHA_TOPO_ADJUST_MULTICAST_BOUNDARY      0x0210
/*  Sub block for adjusting MAUType (PDPortDataAdjust)                      */
#define OHA_TOPO_ADJUST_MAU_TYPE                0x020E
/*  Sub block for adjusting LinkState (PDPortDataAdjust)                    */
#define OHA_TOPO_ADJUST_LINK_STATE              0x021B

/*  Sub block for adjusting PeerToPeerBoundary (PDPortDataAdjust)           */
#define OHA_TOPO_ADJUST_PEER_TO_PEER_BOUNDARY   0x0224
/*  Sub block for adjusting DCPBoundary (PDPortDataAdjust)                  */
#define OHA_TOPO_ADJUST_DCP_BOUNDARY            0x0225

/* BlockLength: This field shall contain the number of octets without counting the fields Type and Length */
/* means  total_size - sizeof(Header.Type) - sizeof(Header.Length) */

/* BlockVersionHigh, BlockVersionLow (MSB-first) */
#define OHA_TOPO_DATA_BLOCK_VERSION         0x0100                  /* V 1.0 */
#define OHA_TOPO_DATA_BLOCK_VERSION_V11     0x0101                  /* V 1.1 */
#define OHA_TOPO_DATA_BLOCK_VERSION_V12     0x0102                  /* V 1.2 */

/* SubBlock CheckPeers:                                                      */
/*     BlockHeader, NumberOfPeers (u8), (                                    */
/*                  LengthPeerPortName (u8), PeerPortName (octet[255]),          */
/*                  LengthPeerStationName (u8), PeerStationName (octet[1..255])  */
/*                                        )*, [Padding]                      */
/* Padding: The number of padding octets shall be adapted to make the block WORD aligned. */

/* SubBlock CheckLineDelayFactor:                                            */
/*     BlockHeader, LineDelayFactor (u32)                                    */

/* SubBlock CheckMAUType:                                                    */
/*     BlockHeader, MAUType (u16), [Padding]                                 */

/* SubBlock CheckLinkState:                                                  */
/*     BlockHeader, LinkState (u16)                                          */

/* SubBlock CheckSyncDifference:                                             */
/*     BlockHeader,  CheckSyncMode(u16)                                      */

/* SubBlock CheckMAUTypeDifference:                                          */
/*     BlockHeader,  MAUTypeMode(u16)                                        */

/* Coding of the field LineDelayFactor (Unsigned32)                          */
/*    Bit 0 - 30: LineDelay.Value                                            */
/*    Bit 31:     LineDelay.FormatIndicator                                  */
/*        0 ... LineDelay.Value is coded as line delay                       */
/*        1 ... LineDelay.Value is coded as cable delay                      */

#define OHA_LINEDELAY_FORMAT_INDICATOR      0x80000000

/* Coding of the field LinkState (Unsigned16) */
/* 0x0000   reserved                            OHA_PORT_LINK_STATE_RESERVED */
/* 0x0001   up (ready to pass packets)          OHA_PORT_LINK_STATE_UP       */
/* 0x0002   down                                OHA_PORT_LINK_STATE_DOWN     */
/* 0x0003   testing (in some test mode)         OHA_PORT_LINK_STATE_TESTING  */
/* 0x0004   Unknown (status can not determined) OHA_PORT_LINK_STATE_UNKNOWN  */
/* 0x0005   Dormant                             OHA_PORT_LINK_STATE_DORMANT  */
/* 0x0006   NotPresent                          OHA_PORT_LINK_STATE_NOT_PRESENT */
/* 0x0007   LowerLayerDown                      OHA_PORT_LINK_STATE_LL_DOWN  */
/* 0x0008 - 0xFFFF  reserved                    OHA_FATAL_ERROR() is called  */
#define OHA_PORT_LINK_STATE_RESERVED        0x0000
#define OHA_PORT_LINK_STATE_UP              0x0001
#define OHA_PORT_LINK_STATE_DOWN            0x0002
#define OHA_PORT_LINK_STATE_TESTING         0x0003
#define OHA_PORT_LINK_STATE_UNKNOWN         0x0004
#define OHA_PORT_LINK_STATE_DORMANT         0x0005
#define OHA_PORT_LINK_STATE_NOT_PRESENT     0x0006
#define OHA_PORT_LINK_STATE_LL_DOWN         0x0007

/* Coding of the field MediaType */
/* 0x00000000   unknown PDPortDataReal          OHA_MEDIUM_TYPE_UNKNOWN      */
/* 0x00000001   Cu cable    PDPortDataReal      OHA_MEDIUM_TYPE_COPPER       */
/* 0x00000002   FO cable    PDPortDataReal      OHA_MEDIUM_TYPE_FIBER        */
/* 0x00000003   Radio communication PDPortDataReal OHA_MEDIUM_TYPE_RADIO_COMMUNICATION */
/* 0x00000004 - 0xFFFFFFFF  reserved            OHA_MEDIUM_TYPE_RESERVED     */
#define OHA_MEDIUM_TYPE_UNKNOWN             0x00000000
#define OHA_MEDIUM_TYPE_COPPER              0x00000001
#define OHA_MEDIUM_TYPE_FIBER               0x00000002
#define OHA_MEDIUM_TYPE_RADIO_COMMUNICATION 0x00000003
#define OHA_MEDIUM_TYPE_RESERVED            0x00000004


/* Coding of the field CheckSyncMode:             */
/*                                                */
/* Bit 0: CableDelay                              */
/*                                                */
/*     0x00 OFF No check                          */
/*     0x01 ON  Check cable delay difference      */
/*              between local and remote measured */
/*              cable delay versus 50ns.          */
/*                                                */
/* Bit 1: SyncMaster                              */
/*                                                */
/*     0x00 OFF No check                          */
/*     0x01 ON  Check PTCP_MasterSourceAddress    */
/*              between local and remote using    */
/*              LLDP_PNIO_PTCP_STATUS.            */
/*                                                */
/* Bit 2 - 15: reserved                           */
/*  This field shall be set according to 3.6.3.2. */

#define OHA_DELAY_DIFFERENCE_CHECK_OFF      0x0000
#define OHA_DELAY_DIFFERENCE_CHECK_ON       0x0001
#define OHA_SYNC_DIFFERENCE_CHECK_ON        0x0002

/* Coding of the field MAUTypeMode:               */
/*                                                */
/* Bit 0: MAUTypeMode.Check                       */
/*                                                */
/*     0x00 OFF No check                          */
/*     0x01 ON  Check MAU type difference between */
/*              local and remote detected value.  */
/*                                                */
/* Bit 1 - 15: MAUTypeMode.reserved               */
/*     This field shall be set to zero.           */

#define OHA_MAU_TYPE_DIFFERENCE_CHECK_OFF   0x0000
#define OHA_MAU_TYPE_DIFFERENCE_CHECK_ON    0x0001

/* Coding of the field DomainBoundary:            */
/*                                                */
/* Bit 0:                                         */
/* 0x00 do not block the multicast MAC address 01-0E-CF-00-04-00 */
/* 0x01 block the multicast MAC address 01-0E-CF-00-04-00        */

#define OHA_SYNC_BOUNDARY_MASK          0x00000001

#define OHA_SYNC_BOUNDARY_OFF           0x00000000 /* do not block the multicast MAC */
#define OHA_SYNC_BOUNDARY_ON            0x00000001 /* block the multicast MAC address 01-0E-CF-00-04-00 */

/* Coding of the field PeerToPeerBoundary:        */
/*                                                */
/* Bit 0:                                         */
/* 0x00 The LLDP agent shall send LLDP frames for this port. */
/* 0x01 The LLDP agent shall not send LLDP frames (egress filter). */
/* Bit 1:                                         */
/* 0x00 The PTCP ASE agent shall send PTCP_DELAY frames for this port. */
/* 0x01 The PTCP ASE agent shall not send PTCP_DELAY frames (egress filter). */

#define OHA_PTP_SEND_LLDP_MASK          0x00000001
#define OHA_PTP_SEND_DELAY_MASK         0x00000002

#define OHA_PTP_SEND_LLDP_ON            0x00000000
#define OHA_PTP_SEND_LLDP_OFF           0x00000001

#define OHA_PTP_SEND_DELAY_ON           0x00000000
#define OHA_PTP_SEND_DELAY_OFF          0x00000002

/* Coding of the field DeviceInitiativeValue:     */
/*                                                */
/* Bit 0: DeviceInitiativeValue.Hello             */
/* 0x00 OFF Device does not issue a DCP-Hello-ReqPDU after power on. */
/* 0x01 ON  Device does issue a DCP-Hello-ReqPDU after power on.     */
/*                                                */
/* Bit 1 - 15: DeviceInitiativeValue.reserved     */

#define OHA_DCP_HELLO_OFF               0
#define OHA_DCP_HELLO_ON                1
#define OHA_DCP_HELLO_ON_DELAY          2
#define OHA_DCP_HELLO_MODE_MASK         3

/* default values for device initiative (FSHelloBlock) */
#define OHA_DEF_HELLO_RETRY             3   /* 3 repeats, default     */
#define OHA_DEF_HELLO_INTERVAL         30   /* 30 ms, default         */
#define OHA_DEF_HELLO_DELAY             0   /* no delay by first send */

/*---------------------------------------------------------------------------*/
/*  Opcode: OHA_OPC_PRM_INDICATION                                           */
/*---------------------------------------------------------------------------*/

/*               ------- possible entries: -------                               */
/*  port-ID 1...n:   MAUType, Delay, Link (Status), Peer (Chassis), Peer (Port), */
/*                   Peer (MAUType), Peer (MRP Domain), Peer (None),             */
/*                   Peer (CableDelay), Peer (RT_Class3), Peer (PTCP),           */
/*                   Peer (PreambleLength), Peer (Fragmentation)                 */
/*  port-ID 0:       Interface (GateWay), Interface (NoS), Interface (IP)        */

#define OHA_TOPO_MAX_IND_ENTRIES            13

typedef struct oha_prm_diag_entry_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_PRM_DIAG_ENTRY_PTR_TYPE;

typedef struct oha_prm_diag_entry_tag
{
    LSA_UINT16 ChannelNumber;       /* 0x8000 */
    LSA_UINT16 ChannelProperties;   /* ChannelProperties.Type_Bit0_7 := 0, */
    /* ChannelProperties.Accumulative_Bit8 := 0, */
    /* ChannelProperties.MaintenanceRequired_Bit9  := Komponentenspezifisch, */
    /* ChannelProperties.MaintenanceDemanded_Bit10 := Komponentenspezifisch, */
    /* ChannelProperties.Specifier_Bit11_12 := Appearing(1) oder Disappearing(2) */
    /* ChannelProperties.Direction_Bit13_15 =: 0 */
    LSA_UINT16 ChannelErrorType;    /* 0x8002 */
    LSA_UINT16 ExtChannelErrorType; /* 0x8000..0x8003 */
    LSA_UINT32 ExtChannelAddValue;  /* 0 */
} OHA_PRM_DIAG_ENTRY_TYPE;


/* length of all Topo Indication segments: 10 x sizeof("ExtChannelDiagnosisData") */
#define OHA_TOPO_MAX_IND_DATA_SIZE          (sizeof(OHA_PRM_DIAG_ENTRY_TYPE) * OHA_TOPO_MAX_IND_ENTRIES)

typedef struct oha_prm_indication_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_PRM_INDICATION_PTR_TYPE;

typedef struct oha_prm_indication_tag
{
    LSA_UINT16                  edd_port_id;        /* 1 ... OHA_CFG_MAX_PORTS */
    LSA_UINT16                  diag_cnt;           /* 0 ... OHA_TOPO_MAX_IND_ENTRIES */
    OHA_UPPER_PRM_DIAG_ENTRY_PTR_TYPE diag_ptr;     /* pointer to arrays (OHA_TOPO_MAX_IND_DATA_SIZE) */
} OHA_PRM_INDICATION_TYPE;


/*     ChannelNumber (Unsigned16)      */

#define OHA_TOPO_CHANNEL_NUMBER                     0x8000

/*     ChannelProperties (Unsigned16)      */

#define OHA_TOPO_ERROR_APPEARS                      0x01
#define OHA_TOPO_ERROR_DISAPPEARS_AND_OKAY          0x02

/* In OHA there are no alarms with Maintenace Required or Maintenace Demanded at all, */
/* therfore no Maintenace must be set */
#define OHA_SET_DIAG_ERROR(_prop, _err) { /* PDEV diagnoses are maintenance-required */ \
_prop = _err; _prop <<= 11; /* _prop |= 0x0200; */ \
}

#define OHA_GET_DIAG_ERROR(_err, _prop) { \
_err = _prop; _err >>= 11; _err &= 0x03; \
}

/*     ChannelErrorType (Unsigned16)      */

/* 0x0000  reserved Unknown error         */
/* 0x8000  Data Transmission Impossible   */
/* 0x8001  Remote Mismatch                */
/* 0x8002  Media Redundancy Mismatch      */
/* 0x800D  Interface Config Mismatch      */
#define OHA_CHANNEL_ERROR_TYPE_RESERVED             0x0000
#define OHA_DATA_TRANSMISSION_IMPOSSIBLE            0x8000
#define OHA_REMOTE_MISMATCH                         0x8001
#define OHA_MEDIA_REDUNDANCY_MISMATCH               0x8002
#define OHA_MULTIPLE_INTERFACE_MISMATCH             0x800D

/*     ExtChannelErrorType (Unsigned16)   */

/* 0x0000   reserved                            */
/* 0x0001 - 0x7FFF  Manufacturer Specific       */
/* 0x9000 - 0x9FFF  reserved for profiles       */
/* 0xA000 - 0xFFFF  reserved                    */
#define OHA_EXTCHANNEL_ERROR_TYPE_RESERVED          0x0000

/*     ExtChannelErrorType for ChannelErrorType OHA_DATA_TRANSMISSION_IMPOSSIBLE */

/* 0x8000   Port State Mismatch - Link down     */
/* 0x8001   MAU Type Mismatch   alarm/diagnosis */
/* 0x8002   Line Delay mismatch alarm/diagnosis */
/* 0x8003 - 0x8FFF  reserved                    */
#define OHA_PORT_STATE_MISMATCH                     0x8000
#define OHA_MAU_TYPE_MISMATCH                       0x8001
#define OHA_LINE_DELAY_MISMATCH                     0x8002

/*     ExtChannelErrorType for ChannelErrorType OHA_REMOTE_MISMATCH */

/* 0x8000   Peer NameOfStation mismatch         */
/* 0x8001   Peer NameOfPort mismatch            */
/* 0x8002   Peer RT_CLASS_3 mismatch            */
/* 0x8003   Peer MAUType mismatch               */
/* 0x8004   Peer MRP domain mismatch            */
/* 0x8005   No peer detected                    */
/* 0x8006   Reserved                            */
/* 0x8007   Peer CableDelay mismatch            */
/* 0x8008   Peer PTCP mismatch                  */
/* 0x8009	Peer Preamble Length mismatch       */
/* 0x800A	Peer Fragmentation mismatch         */
/* 0x800B - 0x8FFF  reserved                    */
#define OHA_PEER_STATION_NAME_MISMATCH              0x8000 /* renamed OHA_PEER_CHASSIS_ID_MISMATCH */
#define OHA_PEER_PORT_NAME_MISMATCH                 0x8001 /* renamed OHA_PEER_PORT_ID_MISMATCH    */
#define OHA_PEER_RT_CLASS_3_MISMATCH                0x8002
#define OHA_PEER_MAUTYPE_MISMATCH                   0x8003
#define OHA_PEER_MRP_DOMAIN_MISMATCH                0x8004
#define OHA_PEER_NONE_DETECTED                      0x8005
#define OHA_PEER_CABLEDELAY_MISMATCH                0x8007
#define OHA_PEER_PTCP_MISMATCH                      0x8008
#define OHA_PEER_PREAMBLE_LENGTH_MISMATCH           0x8009
#define OHA_PEER_FRAGMENTATION_MISMATCH             0x800A

/*     ExtChannelErrorType for ChannelErrorType OHA_MEDIA_REDUNDANCY_MISMATCH */

/* 0x8000   Manager role fail                   */
/* 0x8001   MRP ring open                       */
/* 0x8002   Reserved                            */
/* 0x8003   Multiple mananger                   */
/* 0x8004 - 0x8FFF  reserved                    */

/*     ExtChannelErrorType for ChannelErrorType OHA_MULTIPLE_INTERFACE_MISMATCH */

/* 0x8000   StandardGateway mismatch            */
/* 0x8001   NameOfStation mismach               */
/* 0x8002   IPAddress mismach                   */
/* 0x8003   NameOfDeviceMode mismatch           */
#define OHA_STANDARD_GATEWAY_MISMATCH               0x8000
#define OHA_NAME_OF_STATION_MISMATCH                0x8001
#define OHA_IP_ADDRESS_MISMATCH                     0x8002
#define OHA_INTERFACE_MODE_MISMATCH                 0x8003

/*     default ExtChannelAddValue (Unsigned32) */

#ifndef OHA_TOPO_EXT_CHANNEL_ADD_VALUE
#define OHA_TOPO_EXT_CHANNEL_ADD_VALUE              0
#endif

#define OHA_RTCLASS2_OFF                        0   /* Not used or configured */
/* #define OHA_RTCLASS2_CONFIGURED              1   *//* Reserved     */
#define OHA_RTCLASS2_UP                         2   /* activated for transmission and reception */

#define OHA_PORT_STATUS_CLASS3_OFF              0   /* Not used or configured */
#define OHA_PORT_STATUS_CLASS3_CONFIGURED       1   /* Reserved       */
#define OHA_PORT_STATUS_CLASS3_UP               2   /* activated for transmission */
/*#define OHA_PORT_STATUS_CLASS3_DOWN           3   *//* Reserved     */
#define OHA_PORT_STATUS_CLASS3_RUNNING          4   /* activated for transmission and reception */

/* Coding of the field RTClass2_PortStatus        */
/*                                                */
/* Bit 0 - 1: RTClass2_PortStatus.RTClass2        */
/*                                                */
/* Bit 2 - 15: RTClass2_PortStatus.reserved       */
/*     This field shall be set to zero.           */

#define OHA_PORT_STATUS_CLASS2_MASK             0x0003

/* Coding of the field RTClass3_PortStatus:       */
/*                                                */
/* Bit 0 - 2: RTClass3_PortStatus.RTClass3        */
/*                                                */
/* Bit 3 - 11: RTClass3_PortStatus.reserved       */
/*     This field shall be set to zero.           */
/*                                                */
/* Bit 12: RTClass3_PortStatus.Fragmentation      */
/*                                                */
/* Bit 13: RTClass3_PortStatus.PreambleLength     */
/*                                                */
/* Bit 14: RTClass3_PortStatus.reserved           */
/*     This field shall be set to zero.           */
/*                                                */
/* Bit 15: RTClass3_PortStatus.Mode               */

#define OHA_PORT_STATUS_CLASS3_MASK             0x0007

#define OHA_PORT_STATUS_CLASS3_FRAGMENTATION_MASK  0x1000
#define OHA_PORT_STATUS_CLASS3_PREAMBLE_MASK       0x2000
#define OHA_PORT_STATUS_CLASS3_MODE_MASK           0x8000

/* Coding of the field MultipleInterfaceMode:     */
/*                                                */
/* Bit 0: MultipleInterfaceMode.NameOfDevice      */
/* Bit 1 - 15: MultipleInterfaceMode.reserved_1   */
/* Bit 16 - 31: MultipleInterfaceMode.reserved_2  */

#define OHA_NAME_OF_DEVICE_MASK                    0x00000001

/* Coding of the field SnmpControl:               */
/*                                                */
/* Bit 0: SnmpControl.Activation                  */
/* Bit 1  SnmpControl.Deactivation                */

#define OHA_CTRL_SNMP_ACTIVATE                   1  /* activate the SNMP-agents   */
#define OHA_CTRL_SNMP_DEACTIVATE                 0  /* deactivate the SNMP-agents */

#define OHA_SNMP_CONTROL_MASK                    0x00000001

/*---------------------------------------------------------------------------*/
/*  Opcode: OHA_OPC_PRM_PREPARE, OHA_OPC_PRM_END, OHA_OPC_PRM_COMMIT         */
/*---------------------------------------------------------------------------*/

typedef struct oha_prm_control_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_PRM_CONTROL_PTR_TYPE;

typedef struct oha_prm_control_tag    /* params only for OHA_OPC_PRM_COMMIT: */
{                                     /* LSA_FALSE: remote request ("cable") */
    LSA_BOOL                is_local; /* LSA_TRUE:  local request (by appl.) */
                                      /* AP01520453: do_rtf flag removed     */
} OHA_PRM_CONTROL_TYPE;

/*---------------------------------------------------------------------------*/
/*  Opcode: OHA_OPC_PRM_WRITE, OHA_OPC_PRM_READ                              */
/*---------------------------------------------------------------------------*/

typedef struct oha_prm_data_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_PRM_DATA_PTR_TYPE;

typedef struct oha_prm_data_tag
{
    LSA_UINT16                  slot_number;
    LSA_UINT16                  subslot_number;
    LSA_UINT16                  edd_port_id;
    LSA_UINT32                  record_index;
    LSA_UINT32                  record_data_length;
    OHA_UPPER_MEM_U8_PTR_TYPE   record_data;
    LSA_BOOL                    mk_rema;  /* LSA_TRUE:  write data to REMA   */
									      /* LSA_FALSE: no write to REMA     */
} OHA_PRM_DATA_TYPE;

/*---------------------------------------------------------------------------*/
/*   Opcode: OHA_OPC_REMA_INDICATION, OHA_OPC_REMA_READ                      */
/*---------------------------------------------------------------------------*/

/* data_tag: */

#define OHA_PDEV_DATA           0x0001 /* Bit 0: PDEV data */
#define OHA_STATION_DATA        0x0002 /* Bit 1: STATION data (IP, NoS) */
#define OHA_SNMP_DATA           0x0004 /* Bit 2: SNMP data (SysName, SysContact, SysLocation) */
#define OHA_BUS_DATA            0x0008 /* Bit 3: BUS data (CommonBusParams, AdditionalBusParams) */

typedef struct oha_rema_data_tag
{
    LSA_UINT32                  index;              /* REMA_READ: IN(0xF8000000, 0xF8000001) */
													/* REMA_IND: OUT(0xF8000000, 0xF8000001) */
    LSA_UINT32                  data_length;        /* IN/OUT */
    OHA_UPPER_MEM_U8_PTR_TYPE   data;               /* OUT */
    LSA_UINT16                  data_tag;           /* OUT */
} OHA_REMA_DATA_TYPE;

typedef struct oha_rema_data_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_REMA_DATA_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/*   Opcode: OHA_OPC_REMA_CONTROL                                            */
/*---------------------------------------------------------------------------*/

/* Mode: */

#define OHA_HOLD_REMA_IND     1  /* lock rema indications   */
#define OHA_UNHOLD_REMA_IND   2  /* unlock rema indications */

typedef struct oha_rema_control_tag
{
    LSA_UINT8                 Mode;                             /* IN  */
} OHA_REMA_CONTROL_TYPE;

typedef struct oha_rema_control_tag   OHA_UPPER_MEM_ATTR *    OHA_UPPER_REMA_CONTROL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*                           RQB- Header for Services                        */
/*---------------------------------------------------------------------------*/

typedef union oha_rqb_args_tag
{
    OHA_CHANNEL_TYPE          channel;           /* open/close channel        */
    OHA_CONFIG_TYPE           Config;            /* get config                */
    OHA_IDENT_TYPE            Ident;             /* OEM Identification        */
    OHA_ADDRESS_TYPE          Address;           /* write/read database       */
    OHA_ADDR_INFO_TYPE        AddrInfo;          /* address indication        */
    OHA_DCP_INDICATION_TYPE   DcpInd;            /* indication provide (dcp)  */
    OHA_HELLO_INDICATION_TYPE HelloInd;          /* indication provide (hello)*/
    OHA_LLDP_INDICATION_TYPE  LldpInd;           /* indication provide (lldp) */
    OHA_DCP_CONTROL_TYPE      DcpControl;        /* enable/disable dcp set    */
    OHA_LLDP_CONTROL_TYPE     LldpControl;       /* start/stop/send localdata */
    OHA_NEIGHBOUR_TYPE        Neighbour;         /* get neighbour-data        */
    OHA_OID_TYPE              SnmpOid;           /* get snmp-oid              */
    OHA_DCP_ELEM_TYPE         DcpElem;           /* read/write Dcp-Option     */
    OHA_LLDP_ELEM_TYPE        LldpElem;          /* read remote Lldp-Option   */
    OHA_PORT_CONTROL_TYPE     PortControl;       /* enablse/disables LldpSend.*/

    OHA_PTCP_DATA_TYPE        PtcpData;          /* write the PTCP-STATUS     */
    OHA_TCIP_DATA_TYPE        TcipData;          /* TcIP params (keep alive)  */
    OHA_QUERY_ELEM_TYPE       QueryElem;         /* read query Dcp-Options    */

    OHA_LLDP_OPTION_TYPE      LldpOption;        /* write local Lldp-Option   */
    OHA_LLDP_OPTIONS_TYPE     LldpOptions;       /* read local Lldp-Options   */

    OHA_PRM_CONTROL_TYPE      PrmControl;        /* prepare, end, commit (prm)*/
    OHA_PRM_DATA_TYPE         PrmData;           /* read / write (prm)        */
    OHA_PRM_INDICATION_TYPE   PrmInd;            /* indication provide (prm)  */
    OHA_REMA_DATA_TYPE        RemaData;          /* REMA read / indication    */
    OHA_REMA_CONTROL_TYPE     RemaControl;       /* hold/unhold/sending ind.  */
    OHA_LLDP_ACCESS_TYPE      LldpAccess;        /* lldp.mib access           */
	OHA_LLDP_CONFIG_TYPE      LldpConfig;        /* config ind. provide(lldp) */
    OHA_LLDP_PORT_CONFIG_TYPE LldpPortConfig;    /* port config ind. (lldp)   */

    OHA_TIMER_TYPE            Timer;             /* TimerEvent                */
} OHA_RQB_ARGS_TYPE;


typedef struct oha_rqb_tag
{
    OHA_RQB_HEADER
    OHA_RQB_ARGS_TYPE             args;       /* IN/OUT: Args of Request     */
    OHA_RQB_TRAILER
} OHA_RQB_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/


/*=============================================================================
 * function name:  oha_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         OHA_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_UINT16              Response:       -
 *      OHA_RQB_ERROR_TYPE      Error:          -
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(OHA_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (OHA_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of nare
 *      LSA_UINT16              Response:       OHA_OK
 *                                              OHA_ERR_PARAM
 *                                              OHA_ERR_SEQUENCE
 *                                              or others
 *      OHA_RQB_ERROR_TYPE      Error:          Additional errorcodes from
 *                                              lower layer
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_open_channel(OHA_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  oha_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         OHA_OPC_CLOSE_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_UINT16              Response:       -
 *      OHA_RQB_ERROR_TYPE      Error:          -
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LSA_UINT16              Response:       OHA_OK
 *                                              OHA_ERR_PARAM
 *                                              OHA_ERR_SEQUENCE
 *                                              or others
 *      OHA_RQB_ERROR_TYPE      Error:          Additional errorcodes from
 *                                              lower layer
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_close_channel(OHA_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  oha_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     OHA_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of nare
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_UINT16              Response:   -
 *      OHA_RQB_ERROR_TYPE      Error:      -
 *      OHA_ARGS_TYPE           Args:       Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     OHA_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_UINT16              Response:   OHA_OK
 *                                          OHA_ERR_PARAM
 *                                          OHA_ERR_SEQUENCE
 *                                          or others
 *      OHA_RQB_ERROR_TYPE      Error:      Additional errorcodes from lower
 *                                          layer
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_request(OHA_UPPER_RQB_PTR_TYPE  upper_rqb_ptr);


/* check naming conventions: see FDIS 61158-6-10 Version 2.3 Ed. 2 from 2014-03
---------------------------

The following syntax is reserved by PNIO:
 i) remote SET (name_interpertation = LSA_FALSE): default (draft convention), no further interpretation
ii) local  SET (name_interpertation = LSA_TRUE):  station-name is valid if
    the last (rightmost) label match with "--ab-cd-ef-gh-ij-kl"  with a ...l consisting of [a-f0-9-]
-------------------------------  */
LSA_BOOL  OHA_LOCAL_FCT_ATTR  oha_is_valid_station_name(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name,
    LSA_BOOL allow_upper_case,
    LSA_BOOL name_interpretation
);

/*------------------------------------------------------------------------------
   validate port-name
     *
     * syntax: see PNIO draft
     *
  ----------------------------------------------------------------------------*/
LSA_BOOL  OHA_LOCAL_FCT_ATTR oha_is_valid_port_name(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name
);

/*------------------------------------------------------------------------------
// validate mac-string
     *
     * syntax:
     *
     * the label match with "ab-cd-ef-gh-ij-kl" with a...l consists of [a-f,0-9]
     *
//----------------------------------------------------------------------------*/
LSA_BOOL OHA_LOCAL_FCT_ATTR oha_is_valid_mac_string(
    LSA_UINT16 length,
    OHA_COMMON_MEM_U8_PTR_TYPE name,
    LSA_BOOL allow_upper_case);

/*------------------------------------------------------------------------------
  validate ip-address, subnet-mask and gateway-ip
    *
    * checking rules see SPH IP Configuration
    *
  ----------------------------------------------------------------------------*/
LSA_BOOL oha_is_valid_ip_address (
	LSA_UINT32 ip_addr,        /* ip-address in host byte order */
	LSA_UINT32 subnet_mask,    /* subnet-mask in host byte order */
	LSA_UINT32 gateway,        /* ip-address of the gateway in host byte order */
	LSA_BOOL ip_interpretation /* LSA_TRUE ... ip_address 0.0.0.0 is an error too */
);

/*----------------------------------------------------------------------------*/
/* OHA_OID to ascii() */
/* Return Value   */
/* oha_oid2a returns the number of characters written, not including the terminating null character, */
/* or a negative value if an output error occurs. A negative value is also returned if oidlen or more */
/* wide characters are requested to be written. */
/*----------------------------------------------------------------------------*/
LSA_INT oha_oid2a (
    OHA_OID_PTR_TYPE oid, LSA_UINT16 oidlen,
    OHA_COMMON_MEM_U8_PTR_TYPE s_oid, LSA_UINT16 s_oidlen
);


/*----------------------------------------------------------------------------*/
/* UUID to ascii() */
/* Return Value    */
/* oha_oid2a returns the number of characters written, not including the terminating null character, */
/* or a negative value if an output error occurs. A negative value is also returned if uuidlen or more */
/* wide characters are requested to be written. */
/*----------------------------------------------------------------------------*/
LSA_INT  OHA_UPPER_IN_FCT_ATTR  oha_uuid2a
(
    OHA_COMMON_MEM_U8_PTR_TYPE uuid,
    OHA_COMMON_MEM_U8_PTR_TYPE uuid_string,
    LSA_UINT16 len
);

/*----------------------------------------------------------------------------*/
/* MAC to ascii() */
/* Return Value    */
/* oha_mac2a returns the number of characters written, not including the terminating null character, */
/* or a negative value if an output error occurs. A negative value is also returned if MacLen or more */
/* wide characters are requested to be written. */
/*----------------------------------------------------------------------------*/
LSA_UINT16 oha_mac2a (
    OHA_COMMON_MEM_U8_PTR_TYPE mac,
    OHA_COMMON_MEM_U8_PTR_TYPE s_mac);

/*----------------------------------------------------------------------------*/
/* IP to ascii() */
/* Return Value    */
/* oha_ip2a returns the number of characters written, not including the terminating null character, */
/* or a negative value if an output error occurs. A negative value is also returned if IpLen or more */
/* wide characters are requested to be written. */
/*----------------------------------------------------------------------------*/
LSA_UINT16 oha_ip2a (
    OHA_COMMON_MEM_U8_PTR_TYPE ip,
    OHA_COMMON_MEM_U8_PTR_TYPE s_ip);

/* make the staion name valid:
------------------------------
The field dest_name will be coded as data type OctetString with 1 to 240 octets.
The following syntax is applied:
-       1 or more labels, separated by [.]
-       total length is 1-240
-       label-length is 1-63, truncated if longer
-       labels consisting of [A-Z] are converted to [a-z]
-       labels not consisting of [A-Za-z0-9-] are converted to [x]
-       starting labels with [-] are converted to [x]
-       ending labels with [-] are converted to [x]
EXAMPLE 1: "device-1.vendor_1" is converted to "device-1.vendorx1"
EXAMPLE 2: "device-1.b鰃eholz" is converted to "device-1.bxgeholz"

-       if mac_address is not a null pointer then it holds the mac address
        which is added by a further label as a string
EXAMPLE 3: "device-1.press1" and mac_address is converted to "device-1.press1.00-0a-5e-02-9b-8d"

 -       if mac_prefix is True the mac address is added with the prefix "--"
EXAMPLE 4: "device-1.press1" and mac_address with mac_prefix=True is converted to "device-1.press1.--00-0a-5e-02-9b-8d"
-------------------------------  */
/* Return Values:   */
/*   OHA_ERR_PARAM              invalid (input)length or no buffer  */
/*   OHA_ERR_FAILED             could not convert (MAC address,...) */
/*   OHA_ERR_BUFFER_OVERFLOW    output buffer too small             */
/*   OHA_ERR_DATA_INVALID       label without characters or string is a PortId like "port-001" */
LSA_UINT16  OHA_LOCAL_FCT_ATTR  oha_make_valid_station_name(
    LSA_UINT16                  src_name_len,   /* IN */
    OHA_COMMON_MEM_U8_PTR_TYPE  src_name,       /* IN */
    OHA_COMMON_MEM_U16_PTR_TYPE dest_name_len,  /* IN: length of buffer, OUT: length of data written */
    OHA_COMMON_MEM_U8_PTR_TYPE  dest_name,      /* IN, OUT */
    OHA_MAC_TYPE                mac_address,    /* IN */
    LSA_BOOL                    mac_prefix      /* IN */
);


LSA_VOID oha_prm_write_header(OHA_UPPER_MEM_U8_PTR_TYPE pRecordData, LSA_UINT16 block_type,
							  LSA_UINT32 block_length, LSA_UINT16 block_version,
							  LSA_BOOL block_alignment);

/*====  out functions  =====*/


/*****************************************************************************/
/*  end of file oha_usr.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_USR_H */
