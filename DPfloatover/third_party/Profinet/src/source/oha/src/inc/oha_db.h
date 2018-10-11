
#ifndef OHA_DB_H                         /* ----- reinclude-protection ----- */
#define OHA_DB_H

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
/*  F i l e               &F: oha_db.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Database of OHA                                                          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  12.02.14    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                   DB (DataBase Element)                                   */
/*===========================================================================*/

/*==============              DCP (see oha_usr.h)       =====================*/

/*==============              LLDP (see oha_usr.h)      =====================*/


/*==============              SNMP              =============================*/

/* we treat the SNMP objects as LLDP objects */
#define oha_db_snmp_elem_tag           oha_db_lldp_elem_tag
#define OHA_DB_SNMP_ELEM_TYPE          OHA_DB_LLDP_ELEM_TYPE
#define OHA_DB_SNMP_ELEM_PTR_TYPE      OHA_DB_LLDP_ELEM_PTR_TYPE


/*==============              MIB object        =============================*/

typedef struct oha_db_oid_elem_tag  OHA_COMMON_MEM_ATTR *   OHA_DB_OID_ELEM_PTR_TYPE;

typedef struct oha_db_oid_elem_tag
{
    LSA_UINT32      Value;                  /* variable as integer           */
    LSA_UINT32      TmpSetVal;              /* temorary during a set oper.   */
    LSA_UINT8       Length;                 /* length of Variable in octets  */
    LSA_UINT8       AccCtrl;                /* access control for variable   */
    LSA_BOOL        Remanent;               /* storage of variable in memory */
} OHA_DB_OID_ELEM_TYPE;


/*===========================================================================*/
/* OID parameters for LLDP-MIB (statistics)                                  */
/*===========================================================================*/

/* lldpStatsTxPortTable, lldpStatsRxPortTable */
typedef struct oha_oid_port_stats_tag
{
    LSA_UINT32              TxPortFramesTotal;
    LSA_UINT32              RxPortFramesDiscardedTotal;
    LSA_UINT32              RxPortFramesErrors;
    LSA_UINT32              RxPortFramesTotal;
    LSA_UINT32              RxPortTLVsDiscardedTotal;
    LSA_UINT32              RxPortTLVsUnrecognizedTotal; /* Types 9 ... 126 */
    LSA_UINT32              RxPortAgeoutsTotal;
} OHA_OID_PORT_STATS_TYPE;

/* lldpStatistics */
typedef struct oha_oid_stats_tag OHA_COMMON_MEM_ATTR * OHA_OID_STATS_PTR_TYPE;

typedef struct oha_oid_stats_tag
{
    LSA_UINT32              LastChangeTime;
    LSA_UINT32              RemTabInserts;
    LSA_UINT32              RemTabDeletes;
    LSA_UINT32              RemTabDrops;
    LSA_UINT32              RemTabAgeouts;
} OHA_OID_STATS_TYPE;

/* lldpConfiguration */
typedef struct oha_oid_lldp_config_tag OHA_COMMON_MEM_ATTR * OHA_OID_LLDP_CONFIG_PTR_TYPE;

typedef struct oha_oid_lldp_config_tag
{
    OHA_DB_OID_ELEM_TYPE    TxInterval;       /* Integer32 (5..32768), DEFVAL { 30 } */
    OHA_DB_OID_ELEM_TYPE    LldpReinitDelay;  /* Integer32 (1..10 ),   DEFVAL {'0'} */
    OHA_DB_OID_ELEM_TYPE    LldpTxDelay;      /* Integer32 (1..8192 ), DEFVAL { 2 } */
    OHA_DB_OID_ELEM_TYPE    TxHoldMultiplier; /* Integer32 (2..10 ),   DEFVAL { 4 } */
    OHA_DB_OID_ELEM_TYPE    NotificationInterval; /* Integer32 (5..3600 ), DEFVAL { 5 } */
} OHA_OID_LLDP_CONFIG_TYPE;

/* lldpPortConfig */
typedef struct oha_oid_port_config_tag OHA_COMMON_MEM_ATTR * OHA_OID_PORT_CONFIG_PTR_TYPE;

typedef struct oha_oid_port_config_tag
{                                               /* see lldpPortConfigTable */
    OHA_DB_OID_ELEM_TYPE    AdminStatus;        /* txOnly(1), rxOnly(2), txAndRx(3), disabled(4) */
    OHA_DB_OID_ELEM_TYPE    NotificationEnable; /* Enable(1), Disable(2) */

	OHA_DB_OID_ELEM_TYPE	TLVsTxEnable;	    /* see lldpPortConfigTLVsTxEnable: BITS: portDesc(0), */
												/*                 sysName(1), sysDesc(2), sysCap(3)  */

    OHA_DB_OID_ELEM_TYPE    ManAddrTxEnable;    /* see lldpConfigManAddrTable */
    OHA_DB_OID_ELEM_TYPE    PnoDelayTxEnable;   /* see lldpXPnoConfigTable    */
    OHA_DB_OID_ELEM_TYPE    PnoPortStatusTxEnable;
    OHA_DB_OID_ELEM_TYPE    PnoAliasTxEnable;
    OHA_DB_OID_ELEM_TYPE    PnoMrpTxEnable;
    OHA_DB_OID_ELEM_TYPE    PnoPtcpTxEnable;

    OHA_DB_OID_ELEM_TYPE    Dot3TLVsTxEnable; /* see lldpXdot3PortConfigTable: BITS: macPhyConfigStatus(0)' */
											  /*        powerViaMDI(1), linkAggregation(2), maxFrameSize(3) */

    OHA_DB_OID_ELEM_TYPE    Dot1VlanIdTxEnable;   /* see lldpXdot1ConfigPortVlanTable */
    OHA_DB_OID_ELEM_TYPE    Dot1VlanNameTxEnable; /* see lldpXdot1ConfigVlanNameTable */
    OHA_DB_OID_ELEM_TYPE    Dot1PortProtoVlanTxEnable; /* see lldpXdot1ConfigProtoVlanTable */
    OHA_DB_OID_ELEM_TYPE    Dot1ProtocolTxEnable; /* see lldpXdot1ConfigProtocolTable */
} OHA_OID_PORT_CONFIG_TYPE;


/*===========================================================================*/
/*                 general data of LLDP-PDU                                  */
/*===========================================================================*/

typedef struct oha_db_lldpdu_tag OHA_COMMON_MEM_ATTR * OHA_DB_LLDPDU_PTR_TYPE;

typedef struct oha_db_lldpdu_tag
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pChassisId;     /* Chassis ID: SubTypes: 0 ..7 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pPortId;        /* Port ID: SubTypes: 0 ..7 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pTTL;           /* Time-to-live: 2 octets   */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pPortDescr;     /* Port description: ifDescr is recommended */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSystemName;    /* System name: sysName is recommended */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSystemDescr;   /* System description: sysDescr is recommended */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSystemCap;     /* System capabilities: Bits 1 ...7 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pMgmAddr;       /* Management Address: to reach higher layer entities */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_Others;/* Organizationally-specififc: OUI: 3 Octets, SubType: 1 Octet */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_802_1; /* IEEE 802.1 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_802_3; /* IEEE 802.3 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_Pno;   /* PNO        */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pUnknown_Optionals; /* 9 ... 126 (deleted upon expiration) */
} OHA_DB_LLDPDU_TYPE;


typedef struct oha_oid_rementry_tag
{
    LSA_UINT32              RemTimeMark;
    LSA_UINT32              RemIndex;
} OHA_OID_REMENTRY_TYPE;


/* remote sender data */

typedef struct oha_lldp_sender_tag OHA_UPPER_MEM_ATTR * OHA_LLDP_SENDER_PTR_TYPE;

typedef struct oha_lldp_sender_tag
{
    LSA_UINT8                   SnmpSenderStatus;   /* status for one (multiple) neighbour - for SNMP */
    LSA_UINT16                  LldpEvent;          /* LLDP_EVENT_DATA_VALID, _TOO_MANY_SENDER, ... */
    /* LSA_UINT16               SenderId;    */     /* index + 1 to RemoteSender[] */
    OHA_MAC_TYPE                SenderMacAddr;      /*  MAC of sender               */
    OHA_OID_REMENTRY_TYPE       OidRemEntry;

    /* OHA_LLDPDU_TYPE          LLDPDU;      */
    OHA_DB_LLDPDU_TYPE          DbLLDPDU;
} OHA_LLDP_SENDER_TYPE;


/* device-specific params (NIC-independant) */

typedef struct oha_db_device_tag OHA_UPPER_MEM_ATTR * OHA_UPPER_DB_DEVICE_PTR_TYPE;

typedef struct oha_db_device_tag
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pChassisId; /* NameOfDevice (global) */
    /* max. 255 bytes OHA_MAX_CHASSIS_STRLEN */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSysName;    /* System name: sysName is recommended   */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSysDescr;   /* System description: sysDescr is recommended */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pSysCap;     /* System capabilities                   */
	/* MIB2 system-group parameters */
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) pSysContact; /* SysContact  */
	OHA_LIST_TYPED_ENTRY_TYPE(oha_db_snmp_elem_tag) pSysLocation;/* SysLocation */
} OHA_DB_DEVICE_TYPE;


typedef struct oha_db_lldp_tag OHA_COMMON_MEM_ATTR * OHA_DB_LLDP_PTR_TYPE;

typedef struct oha_db_lldp_tag
{
    LSA_UINT16              PortId;
    LSA_UINT16              LldpSenderId;               /* index + 1 to RemoteSender[] - for LLDP */
    LSA_UINT8               LldpSenderStatus;         /* common status for all senders - for LLDP */
    LSA_UINT16              MrpInstance;                /* index + 1, MRP instance of this port   */

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pPortId;        /* Port-ID, e.g. "port-001" */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pIfDescr;       /* If-Descr TLV, DescrOfSystem  */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_Others;/* Organizationally-specififc: OUI: 3 Octets, SubType: 1 Octet */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_802_1; /* IEEE 802.1 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_802_3; /* IEEE 802.3 */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pOrgSpec_Pno;   /* PNO        */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pUnknown_Optionals; /* 9 ... 126 */

	OHA_COMMON_MEM_U8_PTR_TYPE  pLldpPortID;    /* IEEE-conformal LLDP_PortID: NameOfPort + "." + NameOfStation or */
												/*                             NameOfPort + "." if nameless        */
    OHA_LLDP_SENDER_TYPE        RemoteSender[LLDP_MAX_PORT_SENDER]; /* AP01531722: all sender */

    OHA_OID_PORT_STATS_TYPE     PortStats;                          /* port statistics of LLDP-MIB */
    OHA_OID_PORT_CONFIG_TYPE    PortConfig;                         /* port config. for LLDP-SET   */
} OHA_DB_LLDP_TYPE;


typedef struct oha_db_dcp_tag
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pLocalMacAddr;

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pLocalIpSuite;      /* RealIp (IpAddr, Subnet, Gateway)  */
    LSA_BOOL                                    IpConflictDetected; /* IP conflict, if LSA_TRUE          */
    LSA_UINT16                                  TcpIpStatus;        /* status-word for TCP/IP parameter  */
    LSA_BOOL                                    FactoryReset;       /* LSA_TRUE, if a RtF is done        */

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pDbaseList;         /* pointer to databases              */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pControlOptions;    /* DCP Control Options               */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pDhcpOptions;       /* DHCP-Option-List                  */

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pStationType;       /* OHA_MAX_STATION_TYPE_LEN          */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pStationId;         /* VendorID, DeviceID                */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pProfinetRole;      /* 0 = no role, 1 = Profinet IO,     */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pDevInstance;       /* DeviceInstanceValue               */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pOEMDeviceID;       /* OEM Identification                */

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pPB_Common;         /* common profibus parameters        */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pPB_Addition;       /* additional profibus parameters    */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pPB_Others;         /* other PB options                  */

    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pDeviceInitiative;  /* DeviceInitiative                  */

    OHA_DB_ELEM_PTR_TYPE                        pTempIpSuite;       /* for checking the ethernet    */

    OHA_DB_ELEM_PTR_TYPE                        pExpectedIpSuite;   /* IpAddr, SubnetMask, RouterAddr    */
    OHA_DB_ELEM_PTR_TYPE                        pEmergencyIpSuite;  /* IpAddr, SubnetMask, RouterAddr    */

    OHA_DB_ELEM_PTR_TYPE                        pRemaIpSuite;       /* for remanent storage         */
} OHA_DB_DCP_TYPE;

typedef struct oha_pn_ident_tag
{
    LSA_UINT16     VendorID;
    LSA_UINT16     DeviceID;
    LSA_UINT16     InstanceID;
    LSA_BOOL       Existing;
    LSA_BOOL       Remanent;     /* true for remanent storage */
} OHA_PN_IDENT_TYPE;

typedef struct oha_db_if_tag OHA_COMMON_MEM_ATTR * OHA_DB_IF_PTR_TYPE;

typedef struct oha_db_if_tag
{
    LSA_UINT16                  NicId;
    LSA_UINT32                  InterfaceID;
    LSA_UINT16                  PortCnt;
    LSA_UINT16                  MaxMRP_Instances; /* count of MRP rings                 */
    LSA_UINT16                  PortMaxSender;  /* Number of Sender per Port          */
	LSA_UINT32                  TraceIdx;       /* needed for indexed trace           */
    LSA_UINT32                  Resolution;     /* LineDelay = LineDelay * Resolution */
    LSA_UINT32                  HardwareType;   /*  Bit 10 (EDD-Feature): Delays supported   */
    LSA_UINT32                  LengthOfPeriod; /* SendClockFactor x 32.5us (AP01528425) */
    LSA_UINT8                   MRPDefaultRole; /* No Role on Startup / Client / Manager */
    LSA_UINT8                   MRPRoleCapability; /* If none (=0), MRP is not supported */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_elem_tag)  pStationName;   /* OHA_MAX_STATION_NAME_LEN  */
    OHA_DB_DCP_TYPE             DcpData;                      /* DCP-specific part      */
    OHA_DB_LLDP_TYPE            LldpData[OHA_CFG_MAX_PORTS];  /* LLDP-specific part     */
    OHA_OID_STATS_TYPE          LldpStatistics;         /* lldpStatistics    */
    OHA_DB_ELEM_PTR_TYPE        pTempStationName;       /* for checking the ethernet    */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pTTL;       /* Time-to-live: 2 octets   */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pMgmAddr;   /* Management Address: to reach higher layer entities */
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) pIfDescr_0; /* If-Descr (port_id = 0)     */
    LSA_UINT32                  TempTxHoldMultiplier;   /* for buffering a Lldp-SetGlobals */
											/* table for max. 255 ports (one bit per port) */
	LSA_UINT8                   TempTxPortTable[32+1];  /* temorary for ManAddrTlv's    */

    LSA_UINT32                  MultipleIfModeTemp;     /* written by PrmWrite (temp)   */
    LSA_UINT32                  MultipleIfModeValid;    /* set by PrmCommit (valid)     */
	LSA_BOOL                    MultipleIfModeExisting; /* set by PrmCommit is done     */
    LSA_BOOL                    MultipleIfModeSetTemp;  /* record written (temp)   */
    LSA_BOOL                    MultipleIfModeSetValid; /* record written (valid)  */

    OHA_PN_IDENT_TYPE           PNIdentTemp;     /* PNIdentData written by PrmWrite (temp) */
    OHA_PN_IDENT_TYPE           PNIdentValid;    /* PNIdentData is valid by PrmCommit */

} OHA_DB_IF_TYPE;

typedef struct oha_db_tag OHA_COMMON_MEM_ATTR * OHA_DB_PTR_TYPE;

typedef struct oha_db_tag
{
    LSA_UINT16          NicCount;          /* count of used NICs in the system */
    OHA_DB_DEVICE_TYPE  StationData;       /* pSysName, pSysDescr, ...         */
	OHA_OID_LLDP_CONFIG_TYPE    GlobalConfig;     /* Lldp config (global)      */
    OHA_DB_IF_TYPE      IfData[OHA_CFG_MAX_NICS]; /* interface, NIC-specific   */
} OHA_DB_TYPE;


LSA_EXTERN OHA_DB_TYPE oha_database;        /* common database */

/*===========================================================================*/
/*                   database helper functions                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  OHA_DbGetTrcIdx
 *
 * function:       gets TraceIdx from the Database-Interface-Entry by NIC-Id
 *                 for the interface trace. If none is found 0 is returned.
 *
 * parameters:     LSA_UINT16  IfId
 *
 * return value:   LSA_UINT16  TraceIdx from database (got from EDD_SRV_GET_PARAMS)
 *
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DbGetTrcIdx(
    LSA_UINT16                    NICId);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DB_GET_INTERFACE                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_UPPER_DB_IF_PTR_TYPE                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a Database-Entry with NICId.              +*/
/*+  If none is found LSA_NULL is returned                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_EXTERN OHA_DB_IF_PTR_TYPE OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_GET_INTERFACE(
    LSA_UINT16 NICId);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DB_GET_NIC                              +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_UPPER_DB_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Nic-ID to a Database-Interface-Entry by IfId.        +*/
/*+  If none is found OHA_INVALID_NIC_ID is returned                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_GET_NIC(
    LSA_UINT32 IfId);

/*=============================================================================
 * function name:  OHA_DB_READ_ACCESS_CONTROL
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pAccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_DB_READ_ACCESS_CONTROL(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_UINT8 *                 pAccCtrl    /* access control for variable */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_ACCESS_CONTROL
 * function:       writes the parameter AccessControl to the database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_DB_WRITE_ACCESS_CONTROL(
    LSA_UINT16      NicId,
    LSA_UINT8       Type,       /* type of variable */
    LSA_UINT8       SubType,    /* type of variable */
    LSA_UINT8       AccCtrl    /* access control for variable */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LOCK_STATUS
 * function:       reads the lock status of a database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
* return value:    LSA_FALSE: no user lock (a Dcp-Set is allowed)
                   LSA_TRUE:  locked by any user (a Dcp-Set is not allowed)
*===========================================================================*/
LSA_EXTERN LSA_BOOL OHA_DB_READ_LOCK_STATUS(
    LSA_UINT16      NicId,
    LSA_UINT8       Type,
    LSA_UINT8       SubType
);

/*=============================================================================
 * function name:  OHA_DB_RESET_LOCK_STATUS
 * function:       deletes all user locks of a database-element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
* return value:    LSA_RET_OK, LSA_RET_ERR_PARAM or LSA_RET_ERR_RESOURCE
*===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_DB_RESET_LOCK_STATUS(
    LSA_UINT16      NicId,
    LSA_UINT8       Type,
    LSA_UINT8       SubType
);

/*=============================================================================
 * function name:  OHA_CONTROL_DCP_OPTIONS
 * function:       Lock or Unlock  Dcp-Options
 * parameters:     AccessControl :
                   OHA_READ_ONLY, OHA_READ_WRITE, OHA_NO_ACCESS
 *                 Options (bit-coded):
                   OHA_CHANGE_OF_MAC_ADDR,  OHA_CHANGE_OF_IP_SUITE,
                   OHA_CHANGE_OF_DHCP_OPTION, OHA_CHANGE_OF_STATION_ID,
                   OHA_CHANGE_OF_STATION_NAME, OHA_CHANGE_OF_STATION_TYPE,
                   OHA_CHANGE_OF_PROFIBUS_PARAM, OHA_CHANGE_OF_PROFIBUS_PROFIL,
                   OHA_CHANGE_OF_PBC_SWITCH, OHA_CHANGE_OF_PBC_RESPONSE,

 * return value:   LSA_UINT16:      LSA_RET_OK or Error
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_CONTROL_DCP_OPTIONS (
    LSA_UINT16      NicId,
    LSA_UINT8       AccessControl,
    LSA_UINT32      Options
);

/*=============================================================================
 * function name:  OHA_DB_READ_DCP_ELEMENT
 * function:       reades the parameters of the database element
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
                   Remanent (storage of variable in memory)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_DCP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,        /* type of variable, option */
    LSA_UINT8                   SubType,     /* subtype of variable, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,     /* length of variable in octets */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl,    /* access control for variable */
    LSA_BOOL                  * pRemanent    /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_DCP_ELEMENT
 * function:       writes the parameters to the database
 * parameters:     NicId
                   Type (type of variable, DCP-option)
                   SubType (DCP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
                   AccessControl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                                  OHA_READ_WRITE)
                   Remanent (storage of variable in memory)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DCP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_UINT8                   AccCtrl,    /* access control for variable */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_READ_SUPPORTED_DATABASE
 * function:       Get pointer and length of list of databases supported
 * parameters:     OHA_SYS_WORD_PTR_TYPE * DbasePtrPtr: Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_SUPPORTED_DATABASE(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U16_PTR_TYPE     * DbasePtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_PROFIBUS_COMMON
 * function:       Get profibus common parameters (database 1, dataset 3)
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE*  ParamPtrPtr:  Ptr to structure ptr
 * return value:   LSA_UINT16  Error:      != LSA_RET_OK if parameters
 *                                         not existing or length not 28 byte
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_PROFIBUS_COMMON(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* ParamPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_PROFIBUS_ADDITION
 * function:       Get profibus additional parameters (database 1, dataset 4)
 * parameters:     DCP_PBPROFIL **  ParamPtrPtr:    Ptr to structure ptr
 * return value:   LSA_UINT16   Error:   != LSA_RET_OK parameters
 *                                       not existing or length not 26 byte
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_PROFIBUS_ADDITION(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* ParamPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_TCPIP_STATUS
 * function:       Get Status word for TCP/IP parameter (dataset 2)
 * parameters:     LSA_VOID
 * return value:   LSA_UINT16:                          Status
 *===========================================================================*/
LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_TCPIP_STATUS(
    LSA_UINT16  NicId
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_TCPIP_STATUS
 * function:       Get Status word for TCP/IP parameter (dataset 2)
 * parameters:  LSA_UINT16:                         Status
 *              DCP_STATUS_TCPIP_NONE - TCP/IP Parameter not initialized
 *              DCP_STATUS_TCPIP_PROJ - TCP/IP Parameter projektiert
 *              DCP_STATUS_TCPIP_DHCP - TCP/IP Parameter got from DHCP
 *              DCP_STATUS_TCPIP_RESERVED - 0x0003-0x00ff: Reserviert
 * return value:   LSA_UINT16       !LSA_RET_OK
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_TCPIP_STATUS(
    LSA_UINT16  NicId,
    LSA_UINT16  Status
);

/*=============================================================================
 * function name:  OHA_DB_READ_MAC_ADDR
 * function:       Get pointer and length of the local MAC address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * AddrPtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_MAC_ADDR(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * AddrPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_IP_SUITE
 * function:       Get pointer and length of the local IP suite
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   OHA_SYS_WORD_PTR_TYPE * SubnetMaskPtrPtr: Pointer for subnet mask
                   OHA_SYS_WORD_PTR_TYPE * RouterAddrPtrPtr: Pointer for default router
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_IP_SUITE(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpAddrPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * SubnetMaskPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * RouterAddrPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_DHCP_OPTION
 * function:       Read DHCP option from system
 * parameters:     LSA_UINT8    **  OptionPtrPtr:   Ptr to RetPtr
 *                 LSA_UINT8        OptionId:       ID of DHCP-Option to read
 * return value:   LSA_UINT16       !LSA_RET_OK:    DHCP Option not exists
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_DHCP_OPTION(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* OptionPtrPtr,
    LSA_UINT8                   OptionId
);

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_TYPE
 * function:       Get pointer and length of station type string
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE * TypePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_TYPE(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* TypePtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_NAME
 * function:       Get pointer and length of station name string
 * parameters:     OHA_SYS_BYTE_PTR_TYPE * NamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_NAME(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * NamePtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_ID
 * function:       Get ptr, len and type  of station identificator
 * parameters:     LSA_UINT8    **  IdPtr:      Ptr to RetPtr (Identificator)
 * return value:   LSA_UINT8        Length:     Length of identificator or 0
 *                                              if not present in station
 *===========================================================================*/
LSA_EXTERN LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_ID(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * IdPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_OEM_DEVICE_ID
 * function:       Get ptr, len and type  of station identificator
 * parameters:     LSA_UINT8    **  IdPtr:      Ptr to Identificator
 * return value:   LSA_UINT8        Length:     Length of identificator or 0
 *                                              if not present
 *===========================================================================*/
LSA_EXTERN LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_OEM_DEVICE_ID(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * IdPtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_PROFINET_ROLE
 * function:       Get 32 bit field with stations ProfinetIO role(s)
 * parameters:     LSA_UINT16       NicId       indentifies interface
 * return value:   LSA_UINT8        role        ProfinetIO role of station
 *                                              0 if no role
 *===========================================================================*/
LSA_EXTERN LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_PROFINET_ROLE(
    LSA_UINT16  NicId
);

/*=============================================================================
 * function name:  OHA_DB_READ_DEVICE_INSTANCE
 * function:       Get device instance (option 2, suboption 7)
 * parameters:     LSA_UINT8    **  InstPtr:    Ptr to RetPtr (Identificator)
 * return value:   LSA_UINT8        Length:     Length of identificator (2) 
 *                                              or 0 if not present in station
  * DeviceInstanceValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------------+---------------------------------------------------+
 * + DeviceInstanceValue | InstanceHigh (U8), InstanceLow (U8)               +
 * +---------------------+---------------------------------------------------+
 *===========================================================================*/
LSA_EXTERN LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_DEVICE_INSTANCE(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * InstPtr
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFINET_ROLE
 * function:       Set 32 bit field with stations ProfinetIO role(s)
 * parameters:     LSA_UINT8        role        ProfinetIO role of station
 *                                              0 if no role
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station type
 *                                              cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_PROFINET_ROLE(
    LSA_UINT16      NicId,
    LSA_UINT8       role
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_MAC_ADDR
 * function:       Set local MAC address
 * parameters:     OHA_MAC_ADDR AddrPtr:                Pointer to MAC address
 *                 LSA_INT     AddrLen:                 Length of address
 *                 LSA_UINT8   AccessControl:           Read, Write, Query,...
 *                 LSA_BOOL   Remanent:                 Store value permanently
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_WRITE_MAC_ADDR(
    LSA_UINT16                          NicId,
    OHA_MAC_TYPE  OHA_LOCAL_MEM_ATTR    AddrPtr,
    LSA_INT                             AddrLen,
    LSA_UINT8                           AccessControl,
    LSA_BOOL                            Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_IP_SUITE
 * function:       Set local IP address
 * parameters:     OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 * parameters:     OHA_IP_ADDR SubnetMaskPtr:           Pointer to subnet mask
 * parameters:     OHA_IP_ADDR RouterAddrPtr:           Pointer to router address
 *                 LSA_INT    AddrLen:                  Length of address
 *                 LSA_UINT8  AccessControl:            Read, Write, Query,...
 *                 LSA_BOOL   Remanent:                 Store value permanently
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_IP_SUITE(
    LSA_UINT16                    NicId,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR IpAddrPtr,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR SubnetMaskPtr,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR RouterAddrPtr,
    LSA_INT                       AddrLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_DHCP_OPTION
 * function:       Write DHCP option to system
 *                 LSA_UINT8        OptionId:       ID of DHCP-Option
 * parameters:     LSA_UINT8    *   OptionPtr:      Ptr to Option
 * parameters:     LSA_UINT16       Length:         Length of Option
 * return value:   LSA_UINT16       !LSA_RET_OK:    invalid DHCP Option
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DHCP_OPTION(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* OptionPtr
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_DEVICE_INSTANCE
 * function:       Set station identificator (option 2, suboption 7)
 * parameters:     LSA_UINT16       DeviceInstance
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if identificator
 *                                              cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DEVICE_INSTANCE(
    LSA_UINT16                    NicId,
    LSA_UINT16                    Instance,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_ID
 * function:       Set station identificator (option 2, suboption 3)
 * parameters:     LSA_UINT8    *   IdPtr:      Ptr to identificator (DeviceIDValue)
 *                 LSA_UINT8        IdLen:      Length of identificator
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if identificator
 *                                              cannot be changed
 *
 * DeviceIDValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------+------------------------------------------------------+
 * + DeviceIDValue | VendorIDHigh, VendorIDLow, DeviceIDHigh, DeviceIDLow +
 * +---------------+------------------------------------------------------+
 *
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_STATION_ID(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* IdPtr,
    LSA_INT                       IdLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_OEM_DEVICE_ID
 * function:       Set station identificator (option 2, suboption 8)
 * parameters:     LSA_UINT8    *   IdPtr:      Ptr to identificator (DeviceIDValue)
 *                 LSA_UINT8        IdLen:      Length of identificator
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if identificator
 *                                              cannot be changed
 *
 * DeviceIDValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------+------------------------------------------------------+
 * + DeviceIDValue | VendorIDHigh, VendorIDLow, DeviceIDHigh, DeviceIDLow +
 * +---------------+------------------------------------------------------+
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_OEM_DEVICE_ID(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* IdPtr,
    LSA_INT                       IdLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFIBUS_ADDITION
 * function:       Set stations profibus parameters (database 1, dataset 4)
 * parameters:     DCP_PBPROFIL *   ParamPtr:   Ptr to structure
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if additional
 *                                              parameters cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_PROFIBUS_ADDITION(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE      ParamPtr,
	LSA_BOOL                        IsRema
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFIBUS_COMMON
 * function:       Set stations profibus profile (database 1, dataset 3)
 * parameters:     DCP_PBPROFIL *   ProfilePtr: Ptr to structure
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if common
 *                                          parameters cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_PROFIBUS_COMMON(
    LSA_UINT16                       NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE       ParamPtr,
	LSA_BOOL                         IsRema
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_NAME
 * function:       Set station name (database 8, dataset 2)
 * parameters:     LSA_UINT8    *   NamePtr:    Ptr to name string
 *                 LSA_UINT16       NameLen:    Length of name string
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_STATION_NAME(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_TYPE
 * function:       Set station type (database 8, dataset 1)
 * parameters:     LSA_UINT8    *   TypePtr:    Ptr to type string
 *                 LSA_UINT16       TypeLen:    Length of type string
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station type
 *                                              cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_STATION_TYPE(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    TypePtr,
    LSA_INT                       TypeLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_DEVICE_INITIATIVE
 * function:       Set 32 bit field with DeviceInitiativeValue
 * parameters:     LSA_UINT32   HelloMode
 *                  0x00    OFF Device does not issue a DCP-Hello-ReqPDU after power on.
 *                  0x01    ON  Device does issue a DCP-Hello-ReqPDU after power on.
 *                  0x02    ON  DCP-Hello-ReqPDU after HelloDelay.
 *                 LSA_UINT32   HelloRetry  Number of retransmission of the DCP-Hello-ReqPDU (1..x)
 *                 LSA_UINT32   HelloInterval  SendInterval (in ms)
 *                 LSA_UINT32   HelloDelay  wait this time before first DCP-Hello-ReqPDU (in ms)
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if DeviceInitiative
 *                                          cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DEVICE_INITIATIVE(
    LSA_UINT16      NicId,
    LSA_UINT32      HelloMode,
    LSA_UINT32      HelloRetry,
    LSA_UINT32      HelloInterval,
    LSA_UINT32      HelloDelay
);

/*=============================================================================
 * function name:  OHA_DB_READ_DEVICE_INITIATIVE
 * function:       Get 32 bit field with DeviceInitiativeValue
 * parameters:     LSA_UINT32 * pHelloMode
 *                              Bit 0: DeviceInitiativeValue.Hello
 *                  0x00    OFF Device does not issue a DCP-Hello-ReqPDU after power on.
 *                  0x01    ON  Device does issue a DCP-Hello-ReqPDU after power on.
 *                  0x02    ON  DCP-Hello-ReqPDU after HelloDelay.
 *                 LSA_UINT32 * pHelloRetry
 *                 LSA_UINT32   HelloInterval  SendInterval (in ms)
 *                 LSA_UINT32 * pHelloDelay
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if DeviceInitiative
 *                                          cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_DEVICE_INITIATIVE(
    LSA_UINT16      NicId,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloMode,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloRetry,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloInterval,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloDelay
);

/*----------------------------------------------------------------------------*/
/*    F U N C T I O N - Description:                                          */
/*    Compare station type string with local station type.                    */
/*  > input parameter:                                                        */
/*      - TypePtr:  Pointer to station type                                   */
/*      - TypeLen:  Length of station type                                    */
/*  > output parameter:                                                       */
/*      - result of comparison (TRUE/FALSE)                                   */
/*                                                                            */
/*  if it is another station type. Additional for PBC:                        */
/*  Send an answer, if the asked station type = PBC_ND and                    */
/*  the response flag indicates "send answer"                                 */
/*----------------------------------------------------------------------------*/
LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_STATION_TYPE_CHECK(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR * TypePtr,
    LSA_INT                      TypeLen);

/*=============================================================================
 * function name:  OHA_DB_ALIAS_NAME_CHECK
 * function:       Compare alias name byte array with all the alias names
 * parameters:     LSA_UINT8  *     TypePtr:    Pointer to first byte of
 *                                              station type byte array
 *                 LSA_INT:         TypeLen:    Length of station type array
 * return value:   LSA_TRUE         if alias name exists
 *===========================================================================*/
LSA_EXTERN LSA_BOOL OHA_DB_ALIAS_NAME_CHECK(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* AliasPtr,
    LSA_INT                       AliasLen);

/*----------------------------------------------------------------------------*/

/* oha_db_l.c */

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_ELEMENT
 * function:       writes the local parameters to the database
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length      /* length of pVariable in octets */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_ELEMENT
 * function:       reades the parameters of the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,        /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_CHASSIS_NAME  (renamed OHA_DB_WRITE_DEVICE_NAME)
 * function:       Set global device name
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE   NamePtr:    Ptr to NameOfDevice in octets
 *                 LSA_UINT16       NameLen:    Length of NameOfDevice in octets
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if NameOfDevice
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_CHASSIS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen
);

/*=============================================================================
 * function name:  OHA_DB_READ_CHASSIS_NAME
 * function:       Reads the NameOfDevice string from the database (for LLDP-MIB)
 * parameters:     ppVariable (NameOfDevice in octets)
                   pLength (length of NameOfDevice in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_CHASSIS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppDeviceName,     /* NameOfDevice in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE  pDeviceNameLength /* length of NameOfDevice in octets */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_PORT_ID
 * function:       Get pointer and length of the LLDP_PortID string
 *                 IfMode := 0 LLDP_PortID = NameOfPort, e.g. "port-001"
 *                 IfMode := 1 LLDP_PortID = NameOfPort.NameOfStation
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE * AliasNamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_UINT16:                             Length
 *===========================================================================*/
LSA_EXTERN LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_LLDP_PORT_ID(
    LSA_UINT16  NicId,
    LSA_UINT16  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE * NamePtrPtr
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_CHASSIS_ID
 * function:       Get pointer and length of the LLDP_ChassisID string
 * parameters:     NicId, PortId
                   SubType (LLDP-suboption)
                   NamePtrPtr (LLDP_ChassisID in octets)
 * return value:   LSA_UINT16 Length (length of name in octets)
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_CHASSIS_ID(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * NamePtrPtr, /* LLDP_ChassisID in octets */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType     /* subtype of variable, suboption */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_PORT_NAME
 * function:       Set chassis name
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE   NamePtr:    Ptr to name string
 *                 LSA_UINT16       PortId:     1...OHA_CFG_MAX_PORTS
 *                 LSA_UINT16       NameLen:    Length of port string
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_PORT_NAME(
    LSA_UINT16                    NicId,
    LSA_UINT16                    PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen,
    LSA_BOOL                      Remanent
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_NAME
 * function:        Writes system name string to the database (from MIB II for LLDP-MIB)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysNamePtr: Ptr to name string
 *                  LSA_UINT16      SysNameLen: Length of name string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE    SysNamePtr,
    LSA_UINT16                    SysNameLen,
    LSA_BOOL                      IsRema
);

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_NAME
 * function:       Reads the system name string from the database (for MIB II, LLDP-MIB)
 * parameters:     ppVariable (SysName in octets)
                   pLength (length of SysName in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppSysName,     /* SysName in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysNameLength,  /* length of SysName in octets */
    LSA_BOOL                   * pIsRema
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_DESCR
 * function:        Writes system description string to the database (from MIB II for LLDP-MIB)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysDescrPtr:    Ptr to name string
 *                  LSA_UINT16      SysDescrLen:    Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_DESCR (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysDescrPtr,
    LSA_UINT16                    SysDescrLen
);

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_DESCR
 * function:       Reads the system description string from the database (for MIB II, LLDP-MIB)
 * parameters:     ppVariable (SysDescr in octets)
                   pLength (length of SysDescr in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_DESCR(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppSysDescr,
    OHA_COMMON_MEM_U16_PTR_TYPE pSysDescrLength
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_CONTACT
 * function:        Writes system contact string to the database (for MIB II)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysContactPtr:  Ptr to name string
 *                  LSA_UINT16      SysContactLen:    Length of contact string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if contact cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_CONTACT (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysContactPtr,
    LSA_UINT16                    SysContactLen,
	LSA_BOOL                      IsRema
);

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_CONTACT
 * function:       reads SysContact from the database
 * parameters:     NicId
                   pVariable (SysContact in octets)
                   Length (length of SysContact in octets)
 *                 pIsRema:     storage of SysContact
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_CONTACT(
    OHA_COMMON_MEM_U8_PTR_TYPE * pSysContactPtr, /* SysContact in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysContactLen,  /* length of SysContact in octets */
	LSA_BOOL                   * pIsRema
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_LOCATION
 * function:        Writes system location string to the database (for MIB II)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysLocationPtr:  Ptr to name string
 *                  LSA_UINT16      SysLocationLen:    Length of location string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if location cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_LOCATION (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysLocationPtr,
    LSA_UINT16                    SysLocationLen,
	LSA_BOOL                      IsRema
);

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_LOCATION
 * function:       reads SysLocation from the database
 * parameters:     NicId
                   pVariable (SysLocation in octets)
                   Length (length of SysLocation in octets)
 *                 pIsRema:     storage of SysLocation
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_LOCATION(
    OHA_COMMON_MEM_U8_PTR_TYPE * pSysLocationPtr, /* Location in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysLocationLen,  /* length of Location in octets */
	LSA_BOOL                   * pIsRema
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_CAP
 * function:        Writes system capabilities string to the database (from MIB II for LLDP-MIB)
 * parameters:      LSA_UINT16    SysCapSupported
 *                  LSA_UINT16    SysCapEnabled
 * return value:    LSA_UINT16    Error: != LSA_RET_OK if capabilities cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_CAP (
    LSA_UINT16      SysCapSupported,
    LSA_UINT16      SysCapEnabled
);

/*=============================================================================
 * function name:   OHA_DB_WRITE_IF_DESCR
 * function:        Writes interface description string to the database (from MIB II for LLDP-MIB)
 * parameters:      LSA_UINT16      NicId:      Interface-Id
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr: Ptr to name string
 *                  LSA_UINT16      IfDescrLen: Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_IF_DESCR (
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr,
    LSA_UINT16                  IfDescrLen
);

/*=============================================================================
 * function name:   OHA_DB_READ_IF_DESCR
 * function:        Reads interface description string from the database (from MIB II for LLDP-MIB)
 * parameters:      LSA_UINT16      NicId:      Interface-Id
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr: Ptr to name string
 *                  LSA_UINT16      IfDescrLen: Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be read
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_IF_DESCR (
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE * pIfDescrPtr, /* IfDescr in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pIfDescrLen   /* length of IfDescr in octets */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_VALUE
 * function:       reades the global parameters of an LLDP mib object
 * parameters:     Options (LLDP.MIB option flags)
                   pValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_VALUE(
    LSA_UINT32                  Options,    /* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE pValue,     /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,    /* length of variable in octets */
    LSA_BOOL                  * pRemanent   /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_ACCESS
 * function:       reades access parameter of a global LLDP mib object
 * parameters:     Options (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_ACCESS(
    LSA_UINT32                  Options,    /* type of variable   */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl    /* access control for variable */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_PORT_VALUE
 * function:       reades a port parameter of the lldp-mib object
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   pValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_PORT_VALUE(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE pValue,     /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,    /* length of variable in octets */
    LSA_BOOL                  * pRema       /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_PORT_ACCESS
 * function:       reades the access parameter of the lldp-mib port object
 * parameters:     NicId
                   PortId
                   PortOptions (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_PORT_ACCESS(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,    /* type of variable   */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl    /* access control for variable */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_VALUE
 * function:       writes the global parameters for mib value to the database
 * parameters:     Options (LLDP.MIB option flags)
                   Value (variable)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_VALUE(
    LSA_UINT32                  Options,    /* type of variable   */
    LSA_UINT32					Value,      /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_ACCESS
 * function:       writes the parameters for mib access to the database
 * parameters:     Options (LLDP.MIB option flags)
                   AccCtrl (OHA_READ_ONLY, OHA_WRITE_ONLY,
                            OHA_NO_ACCESS, OHA_QUERY_ACCESS,
                            OHA_READ_WRITE)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_ACCESS(
    LSA_UINT32                  Options,    /* type of variable   */
    LSA_UINT8                   AccCtrl    /* access control for variable */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_PORT_VALUE
 * function:       writes the parameters for LLDP.MIB value to the database
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   Variable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    LSA_UINT32					Value,      /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
);

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS
 * function:       writes the parameters for LLDP.MIB access to the database
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_EXTERN LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    LSA_UINT8                   AccCtrl    /* access control for variable */
);

/* FUNCTION: oha_control_snmp_oid()
 *
 * When a OID is found,
 * that is lexicographicly preceded by the input string, the function
 * for that entry is called to find the method of access of the
 * instance of the named variable. If that variable is not found,
 * NULL is returned. If it is found, the function
 * returns a character pointer and a length.
 *
 * Checks the version field. If the version is SNMPv1, then
 * restrict the use of 64-bit datatypes
 * - return NULL on a GET for 64-bit datatype
 * - skip 64-bit objects when checking for GETNEXT
 *
 * RETURNS: snmp error code
 */
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR  oha_control_snmp_oid(
    OHA_OID                        mib_node,       /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    OHA_OID_PTR_TYPE               oid_name,       /* IN - name of var, OUT - name matched */
    LSA_UINT32 OHA_COMMON_MEM_ATTR * oid_len,      /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    OHA_UPPER_MEM_U8_PTR_TYPE      var_val,        /* IN/OUT - buffer of variable */
    OHA_COMMON_MEM_U8_PTR_TYPE     var_type,       /* OUT - type of matched variable */
    LSA_UINT32 OHA_COMMON_MEM_ATTR * var_len,      /* OUT - length of matched variable */
    OHA_COMMON_MEM_U16_PTR_TYPE    access_control, /* OUT - access control */
    LSA_UINT8                      msg_type,       /* IN - message type */
    LSA_INT32                      version
);


LSA_VOID oha_prm_write_header(OHA_UPPER_MEM_U8_PTR_TYPE pRecordData, LSA_UINT16 block_type,
							  LSA_UINT32 block_length, LSA_UINT16 block_version,
							  LSA_BOOL block_alignment);

/*=============================================================================
 * function name:  OHA_TOLOWER
 *
 * function:    convert uppercase letter to lowercase letter
 *
 * description: OhaToLower will return the ASCII value for the lowercase equivalent
 *              of the ASCII character chr, if it is a letter. If the character
 *              is already lowercase, it remains lowercase
 *
 *===========================================================================*/
LSA_INT OHA_TOLOWER( LSA_INT ch );

/*=============================================================================
 * function name:  OHA_STRNTOLOWER
 *
 * function:    make a string lowercase
 *
 * description: OhaStrnToLower makes string with all alphabetic characters converted to lowercase.
 *
 *===========================================================================*/
LSA_VOID OHA_STRNTOLOWER(OHA_COMMON_MEM_U8_PTR_TYPE pStr, LSA_INT Len);

/*---------------------------------------------------------------------------*/
/* function name: OHA_UPPER_CASES_ALLOWED                                    */
/*                                                                           */
/* description: Get validity of NameOfStation and PortID.                    */
/*                                                                           */
/* false: The NameOfStation is strictly checked for PNIO standard conventions*/
/* true:  Upper cases are converted to lower cases before the NameOfStation  */
/*        is checked for PNIO standard conventions.                          */
/*                                                                           */
/* Note:  If the NameOfStation is valid, the NameOfStation is only stored    */
/*        with (converted) lower cases.                                      */
/*        If the NameOfStation is invalid                                    */
/*           a) a local Set or remote DCP-Set is rejected,                   */
/*           b) a remote station (LLDP) is invalid.                          */
/*---------------------------------------------------------------------------*/
LSA_BOOL  OHA_SYSTEM_OUT_FCT_ATTR  OHA_UPPER_CASES_ALLOWED(LSA_UINT16  NicId);

/*****************************************************************************/
/*  end of file oha_db.h                                                     */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_DB_H */
