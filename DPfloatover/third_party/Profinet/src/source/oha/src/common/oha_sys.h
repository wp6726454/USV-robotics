
#ifndef OHA_SYS_H                        /* ----- reinclude-protection ----- */
#define OHA_SYS_H

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
/*  F i l e               &F: oha_sys.h                                 :F&  */
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
/*  Defines constants, types, macros and prototyping for oha                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

#ifdef OHA_CFG_DONT_USE_LINK_STATUS
# error "OHA_CFG_DONT_USE_LINK_STATUS was a temporary define. Please remove it."
#endif

/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for OHAFatalError())                               */
/*---------------------------------------------------------------------------*/

#define OHA_FATAL_ERROR_TYPE  LSA_UINT32

#define OHA_FATAL_ERR_NULL_PTR            (OHA_FATAL_ERROR_TYPE)  0x01
#define OHA_FATAL_ERR_MEM_FREE            (OHA_FATAL_ERROR_TYPE)  0x02
#define OHA_FATAL_ERR_RELEASE_PATH_INFO   (OHA_FATAL_ERROR_TYPE)  0x03
#define OHA_FATAL_ERR_INCONSISTENZ        (OHA_FATAL_ERROR_TYPE)  0x04
#define OHA_FATAL_ERR_HANDLE_INVALID      (OHA_FATAL_ERROR_TYPE)  0x06
#define OHA_FATAL_ERR_EDD                 (OHA_FATAL_ERROR_TYPE)  0x07
#define OHA_FATAL_ERR_DCP                 (OHA_FATAL_ERROR_TYPE)  0x08
#define OHA_FATAL_ERR_LLDP                (OHA_FATAL_ERROR_TYPE)  0x09
#define OHA_FATAL_ERR_NARE                (OHA_FATAL_ERROR_TYPE)  0x0A
#define OHA_FATAL_ERR_SOCK                (OHA_FATAL_ERROR_TYPE)  0x0B
#define OHA_FATAL_ERR_USER                (OHA_FATAL_ERROR_TYPE)  0x0C
#define OHA_FATAL_ERR_SNMPX               (OHA_FATAL_ERROR_TYPE)  0x0D
#define OHA_FATAL_ERR_TIMER_RESOURCE      (OHA_FATAL_ERROR_TYPE)  0x0E
#define OHA_FATAL_ERR_MRP                 (OHA_FATAL_ERROR_TYPE)  0x0F

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
#define OHA_DETAIL_PTR_TYPE                 /* pointer to OHA_DETAIL */ \
struct oha_detail_tag      OHA_SYSTEM_MEM_ATTR *

#define OHA_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag  OHA_LOCAL_MEM_ATTR *

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by OHA_GET_PATH_INFO)                           */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in OHA_GET_PATH_INFO)                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*------------  RemaStationFormat ---------------*/

/* OHA_OPC_REMA_INDICATION indicates the station record OHA_RECORD_INDEX_MULTIPLE_STATION */
#define OHA_REMA_STATION_FORMAT_STANDARD    1

/* OHA_OPC_REMA_INDICATION indicates the station records OHA_RECORD_INDEX_NAME_OF_STATION, OHA_RECORD_INDEX_IP_SUITE */
/*                         and OHA_RECORD_INDEX_IF_DHCP_PARAMS */
#define OHA_REMA_STATION_FORMAT_ADVANCED    2

typedef union oha_cbd_path_params_tag
{
    /* Params for OHA_PATH_TYPE_USER Channel */
    struct
    {										/* NIC-specific ID (global or if-specific):    */
        LSA_UINT16      NICId;				/* global: NICId of OHA_PATH_TYPE_SOCK channel */
											/* if-specific: NICId of the other system channels */ 
											/* LSA_FALSE: every dcp indication is sent to the user */
        LSA_UINT32      DcpIndicationFilter;/* 0: all dcp indications are sent to the user */
                                            /* <>0: bitcoded, see OHA_CTRL_IP_SUITE, OHA_CTRL_STATION_NAME,...  */
        LSA_UINT16      CheckInterval;      /* 0 ... no cyclic check (IP),                 */
											/* 1 ... check every 5sec, 2...10sec, 6...30sec (max.) */
        LSA_BOOL        CheckIpEnabled;     /* only for OHA_OPC_PRM_WRITE of a station-record: */
                                            /* LSA_TRUE: Check IP, LSA_FALSE: don't check IP */
        LSA_BOOL        allow_non_pnio_mautypes; /* true: 10Mbps and half-duplex (for all speeds) are allowed */

											/* 0 ... no REMA user channel */
											/* <>0 ..for OHA_OPC_REMA_INDICATION(data_tag=OHA_STATION_DATA) confirmation: */
        LSA_UINT16      RemaStationFormat; 	/* 1 ... index=OHA_RECORD_INDEX_MULTIPLE_STATION will be indicated */
											/* 2 ... index=OHA_RECORD_INDEX_NAME_OF_STATION, OHA_RECORD_INDEX_IP_SUITE */

        LSA_BOOL        CheckTopology;      /* LSA_FALSE: no topology check       */
                                            /* LSA_TRUE:  topology check on       */
	} User;

    /* Params for OHA_PATH_TYPE_EDD Channel */
    struct
    {
        LSA_UINT16      NICId;               /* NIC-specific ID                    */

											/* LSA_FALSE: The NameOfStation is strictly checked for PNIO standard conventions. */
        LSA_BOOL        NoS_allow_upper_cases; /* LSA_TRUE:  Upper cases are converted to lower cases before checking. */

    } Edd;

    /* Params for OHA_PATH_TYPE_DCP Channel */
    struct
    {
        LSA_UINT16      NICId;               /* NIC-specific ID                    */
        LSA_UINT16      NumOfRecv;           /* receive-resources for DCP-Server   */

        LSA_UINT16      NumOfRecvFSU;        /* receive-resources for FastStartUp  */
        /* for establishing AR's at the same time (min.16) */
    } Dcp;

    /* Params for OHA_PATH_TYPE_LLDP Channel */
    struct
    {
        LSA_UINT16      NICId;               /* NIC-specific ID                    */
        LSA_UINT16      NumOfRecv;           /* receive-resources for LLDP         */
    } Lldp;

    /* Params for OHA_PATH_TYPE_SOCK Channel */
    struct
    {
        LSA_UINT16      NICId;               /* NIC-specific ID                    */
    } Sock;

    /* Params for OHA_PATH_TYPE_SNMPX Channel */
    struct
    {
        LSA_UINT16      NICId;               /* NIC-specific ID                    */
    } Snmpx;

    /* Params for OHA_PATH_TYPE_NARE Channel */
    struct
    {
        LSA_UINT16      NICId;             /* NIC-specific ID                      */
    } Nare;

    /* Params for OHA_PATH_TYPE_MRP Channel */
    struct
    {
        LSA_UINT16      NICId;             /* NIC-specific ID                      */
    } Mrp;

} OHA_CDB_PATH_PARAMS_TYPE;


typedef struct oha_detail_tag
{
    LSA_UINT16                PathType; /* Type of Channel                    */
    OHA_CDB_PATH_PARAMS_TYPE  Params;   /* PathType specific Params.          */
} OHA_CDB_TYPE;

/* defines for PathType */

#define OHA_PATH_TYPE_USER   1   /* User-Channel            */
#define OHA_PATH_TYPE_SOCK   2   /* System-Channel for SOCK */
#define OHA_PATH_TYPE_DCP    3   /* System-Channel for DCP  */
#define OHA_PATH_TYPE_EDD    4   /* System-Channel for EDD  */
#define OHA_PATH_TYPE_LLDP   5   /* System-Channel for LLDP */
#define OHA_PATH_TYPE_NARE   6   /* System-Channel for NARE */
#define OHA_PATH_TYPE_SNMPX  7   /* System-Channel for SNMPX */
#define OHA_PATH_TYPE_MRP    8   /* System-Channel for MRP  */

#define OHA_PORT_CNT_MAX  0xFF   /* max port cnt supported (255) = PortID 1..255*/

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
 * function name:  oha_init
 *
 * function:       initialize OHA
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_IN_FCT_ATTR oha_init(LSA_VOID);

/*=============================================================================
 * function name:  oha_undo_init
 *
 * function:       undo the initialization of OHA
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_IN_FCT_ATTR  oha_undo_init(LSA_VOID);

/*=============================================================================
 * function name:  oha_init_interface
 *
 * function:       initialization of an interface
 *
 * parameters:     ...
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_init_interface(
    LSA_UINT16 NICId,
    LSA_UINT16 VendorID,
    LSA_UINT16 DeviceID,
    LSA_UINT16 InstanceID,       /* used if InstanceIDValid = True */
    LSA_BOOL   InstanceIDValid,  /* True: DCP suboption InstanceID will be created, False: no suboption */ 
    LSA_UINT8  ProfinetRole,     /* DCP_PN_DEVICE, DCP_PN_CONTROLLER, etc. 0 = no role, see IEC 61158-6 DeviceRoleDetails */
    OHA_COMMON_MEM_U8_PTR_TYPE DeviceVendorValue, /* see IEC 61158-6, (former StationType) */
    LSA_UINT16 DeviceVendorLen,
	LSA_BOOL   rtf_nos_ip_read_only, /* True: initial attributes are READ_ONLY (usecase Controller), False: READ_WRITE (usecase Device) */
	OHA_COMMON_MEM_U8_PTR_TYPE NameOfDevice, /* see IEC 61158-6 MultipleInterfaceMode.NameOfDevice */
    LSA_UINT16 NameOfDeviceLen,
	LSA_UINT16 SysCapSupported, /* see IEEE 802.1AB-2009, LLDP-Type 7 "System Capabilities" (LLDP_TLV_SYSCAB_OTHER, etc.) */
	LSA_UINT16 SysCapEnabled,   /* see IEEE 802.1AB-2009, LLDP-Type 7 "System Capabilities" (LLDP_TLV_SYSCAB_OTHER, etc.) */
	OHA_COMMON_MEM_U8_PTR_TYPE SysDescr, /* System Description (MIB2 sysDescr) */
    LSA_UINT16 SysDescrLen,
	OHA_COMMON_MEM_U8_PTR_TYPE IfDescr, /* Interface Description (MIB2 ifDescr of the interface) */
    LSA_UINT16 IfDescrLen
	);

/*=============================================================================
 * function name:  oha_init_port
 *
 * function:       initialization of a port
 *
 * parameters:     ...
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_init_port(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,   /* 1 ... OHA_CFG_MAX_PORTS */
    OHA_COMMON_MEM_U8_PTR_TYPE NameOfPort, /* see IEC 61158-6 */
    LSA_UINT16 NameOfPortLen,
    OHA_COMMON_MEM_U8_PTR_TYPE IfDescr, /* Interface Description (MIB2 ifDescr of the port) */
    LSA_UINT16 IfDescrLen
    );

/*=============================================================================
 * function name:  oha_undo_init_interface
 *
 * function:       undo initialization of an interface
 *
 * parameters:     -
 *
 * return value:   -
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_undo_init_interface(
    LSA_UINT16 NICId);

/*=============================================================================
 * function name:  oha_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_UINT16        timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  OHA_SYSTEM_IN_FCT_ATTR  oha_timeout(
    LSA_UINT16        timer_id,
    LSA_USER_ID_TYPE  user_id);


/*****************************************************************************/
/*                                                                           */
/* Output-functions/macros                                                   */
/*                                                                           */
/*****************************************************************************/

/*=============================================================================
 * function name:  OHA_GET_PATH_INFO
 *
 * function:       get system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system-pointer
 *                 OHA_DETAIL_PTR_TYPE  ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail-pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_GET_PATH_INFO
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_GET_PATH_INFO(
    LSA_UINT16              OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        OHA_LOCAL_MEM_ATTR *  sys_ptr_ptr,
    OHA_DETAIL_PTR_TYPE     OHA_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                              path
);
#endif

/*=============================================================================
 * function name:  OHA_RELEASE_PATH_INFO
 *
 * function:       release system-pointer and detail-pointer of a communication
 *                 channel
 *
 * parameters:     LSA_UINT16      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system-pointer or
 *                                                      wrong detail-pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system-pointer
 *                 OHA_DETAIL_PTR_TYPE  detail_ptr:  detail-pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_RELEASE_PATH_INFO
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_RELEASE_PATH_INFO(
    LSA_UINT16              OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr,
    OHA_DETAIL_PTR_TYPE     detail_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated
 *                                         LSA_RET_ERR_NO_TIMER  no timer has
 *                                                               been allocated
 *                 LSA_UINT16  ...  *  timer_id_ptr:
 *                           return value: pointer to id of timer
 *                 LSA_UINT16          timer_type:  LSA_TIMER_TYPE_ONE_SHOT or
 *                                                  LSA_TIMER_TYPE_CYCLIC
 *                 LSA_UINT16          time_base:   LSA_TIME_BASE_1MS,
 *                                                  LSA_TIME_BASE_10MS,
 *                                                  LSA_TIME_BASE_100MS,
 *                                                  LSA_TIME_BASE_1S,
 *                                                  LSA_TIME_BASE_10S or
 *                                                  LSA_TIME_BASE_100S
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_ALLOC_TIMER
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_ALLOC_TIMER(
    LSA_UINT16  OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_UINT16  OHA_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                        timer_type,
    LSA_UINT16                        time_base
);
#endif

/*=============================================================================
 * function name:  OHA_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn´t
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time
 *                                 system will call OHA_timeout().
 *                 LSA_UINT16          timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of prefix
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_START_TIMER
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_START_TIMER(
    LSA_UINT16        OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_UINT16                              timer_id,
    LSA_USER_ID_TYPE                        user_id,
    LSA_UINT16                              time
);
#endif

/*=============================================================================
 * function name:  OHA_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                    timer has been
 *                                                               stopped
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped
 *                                 LSA_RET_ERR_PARAM             timer hasn´t
 *                                                               stopped
 *                                                               because
 *                                                               of wrong
 *                                                               timer-id
 *                 LSA_UINT16          timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_STOP_TIMER
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_STOP_TIMER(
    LSA_UINT16  OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_UINT16                        timer_id
);
#endif

/*=============================================================================
 * function name:  OHA_FREE_TIMER
 *
 * function:       free a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_UINT16          timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_FREE_TIMER
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_FREE_TIMER(
    LSA_UINT16  OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_UINT16                        timer_id
);
#endif

/*=============================================================================
 * function name:  OHA_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 OHA_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_FATAL_ERROR
/*@noreturn@*/ LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_FATAL_ERROR(
    LSA_UINT16                 length,
    OHA_FATAL_ERROR_PTR_TYPE   error_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_ENTER
 *
 * function:       Protect sequence within OHA from beeing interrupted.
 *                 (reentrance protection)
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_ENTER
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  OHA_EXIT
 *
 * function:       Finishes OHA_ENTER-Sequence.
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_EXIT
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  OHA_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     OHA_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT32                      length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_ALLOC_LOCAL_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_ALLOC_LOCAL_MEM(
    OHA_LOCAL_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  local_mem_ptr_ptr,
    LSA_UINT32                                      length
);
#endif

/*=============================================================================
 * function name:  OHA_FREE_LOCAL_MEM
 *
 * function:       frees local -memory
 *
 * parameters:              ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 OHA_LOCAL_MEM_PTR_TYPE  local_mem_ptr:  pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_FREE_LOCAL_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_FREE_LOCAL_MEM(
    LSA_UINT16                 OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_LOCAL_MEM_PTR_TYPE                           local_mem_ptr
);
#endif

/*=============================================================================
 * function name:  OHA_RQB_ERROR
 *
 * function:       notify a RQB-error in a user/system-request.
 *                 this function is called if there is a error (i.e. param)
 *                 within RQB which prevents notification of the caller
 *                 (i.e. missing call-back-function). the error-code is set
 *                 in "Status" in RQB. this error is typically a implementation
 *                 error.
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_RQB_ERROR
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_RQB_ERROR(
    OHA_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

LSA_EXTERN /*@noreturn@*/ LSA_VOID OHA_LOCAL_FCT_ATTR OHA_FatalError(
    OHA_FATAL_ERROR_TYPE   Error,
    LSA_UINT16             ModuleID,
    LSA_UINT32             Line);


/*=============================================================================
 * function name:  OHA_MEMSET
 *
 * function:       fills memory with value
 *
 * parameters:      OHA_LOCAL_MEM_PTR_TYPE        pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_MEMSET
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MEMSET(
    OHA_LOCAL_MEM_PTR_TYPE              pMem,
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  OHA_MEMCPY
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      OHA_COMMON_MEM_PTR_TYPE        pDst
 *                  OHA_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_MEMCPY
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MEMCPY(
    OHA_COMMON_MEM_PTR_TYPE            pDst,
    OHA_COMMON_MEM_PTR_TYPE            pSrc,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  OHA_MEMMOVE
 *
 * function:       copy Length bytes memory from pSrc to pDst.
 *
 * parameters:      OHA_COMMON_MEM_PTR_TYPE        pDst
 *                  OHA_COMMON_MEM_PTR_TYPE        pSrc
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_MEMMOVE
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_MEMMOVE(
    OHA_COMMON_MEM_PTR_TYPE            pDst,
    OHA_COMMON_MEM_PTR_TYPE            pSrc,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  OHA_MEMCMP
 *
 * function:       compares Length bytes of memory from pBuf1 with pBuf2
 *
 * parameters:      OHA_COMMON_MEM_PTR_TYPE        pBuf1
 *                  OHA_COMMON_MEM_PTR_TYPE        pBuf2
 *                  LSA_UINT32                      Length
 *
 * return value:   LSA_TRUE:  buffers are the same
 *                 LSA_FALSE: buffers differ
 *===========================================================================*/
#ifndef OHA_MEMCMP
LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR  OHA_MEMCMP(
    OHA_COMMON_MEM_PTR_TYPE            pBuf1,
    OHA_COMMON_MEM_PTR_TYPE            pBuf2,
    LSA_UINT32                         Length
);
#endif

/*=============================================================================
 * function name:  OHA_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper-RQB for local use
 *                 the memory has to be initialized with 0
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_ALLOC_UPPER_RQB_LOCAL(
    OHA_UPPER_RQB_PTR_TYPE  OHA_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif

/*=============================================================================
 * function name:  OHA_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper-RQB allocated with OHA_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-RQB or
 *                 OHA_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef OHA_FREE_UPPER_RQB_LOCAL
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
    OHA_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  OHA_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_ALLOC_UPPER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_ALLOC_UPPER_MEM(
    OHA_UPPER_MEM_PTR_TYPE  OHA_LOCAL_MEM_ATTR   * upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT16                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  OHA_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_FREE_UPPER_MEM
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_FREE_UPPER_MEM(
    LSA_UINT16             OHA_LOCAL_MEM_ATTR   * ret_val_ptr,
    OHA_UPPER_MEM_PTR_TYPE                        upper_mem_ptr,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  OHA_GET_SYS_UPTIME
 *
 * function:       The GetTickCount function retrieves the number of 10 milliseconds
 *                 that have elapsed since the system was started.
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_GET_SYS_UPTIME
LSA_UINT32  OHA_SYSTEM_OUT_FCT_ATTR
OHA_GET_SYS_UPTIME (LSA_VOID);
#endif


/*=============================================================================
 * function name:    OHA_HTONS ... host to network byte order, short integer
 *                      _NTOHS ... network to host byte order, short integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONS
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR  OHA_HTONS(
    LSA_UINT16 host_short
);
#endif

#ifndef OHA_NTOHS
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR  OHA_NTOHS(
    LSA_UINT16 network_short
);
#endif

/*=============================================================================
 * function name:    OHA_HTONL ... host to network byte order, long  integer
 *                      _NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONL
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR  OHA_HTONL(
    LSA_UINT32 host_long
);
#endif

#ifndef OHA_NTOHL
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR  OHA_NTOHL(
    LSA_UINT32 network_long
);
#endif


/*=============================================================================
 * function name:  OHA_PUT 8,16,32 and OHA_GET 8,16,32
 *
 * function:       macros for memory access without alignment requirements
 *
 * important:      WE WILL IMPLEMENT variants for mem-attribs
 *                 when needed. All known platforms are "flat".
 *
 *===========================================================================*/

#ifndef OHA_PUT8_HTON
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_PUT8_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT8 val
);
#endif

#ifndef OHA_GET8_NTOH
LSA_UINT8  OHA_SYSTEM_OUT_FCT_ATTR  OHA_GET8_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

#ifndef OHA_PUT16_HTON
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_PUT16_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT16 val
);
#endif

#ifndef OHA_GET16_NTOH
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_GET16_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

#ifndef OHA_PUT32_HTON
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_PUT32_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset,
    LSA_UINT32 val
);
#endif

#ifndef OHA_GET32_NTOH
LSA_UINT32  OHA_SYSTEM_OUT_FCT_ATTR  OHA_GET32_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT offset
);
#endif

/*****************************************************************************/
/*                                                                           */
/* DCP-Server macros                                                         */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_IND
 * function:       Trace incoming SET Requests
 *                 Called before calling "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 *  ReqPtr:     -> SET request data
 *                 LSA_INT      ReqLen:     Length of request
 * return value:   LSA_UINT8   Reason:      The first Result-field of the
 *                                          SET-Response will be set to this value.
 *
 *  OHA_DCP_REASON_OK (no error)        Positive response, accept a SET and deliver parameters
 *  OHA_DCP_REASON_NO_DATABASE          Option not supported
 *  OHA_DCP_REASON_NO_DATASET           uboption not supported or no dataset available
 *  OHA_DCP_REASON_DS_NOT_SET (default) Suboption not set (local reasons)
 *  OHA_DCP_REASON_NO_RESOURCE          (temporary) resource error (server)
 *  OHA_DCP_REASON_NOT_POSSIBLE         SET not possible (local reasons)
 *  OHA_DCP_REASON_NOT_APPLICABLE       SET not possible (application operation)
 *
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_IND
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR OHA_REPORT_DCP_SET_IND(
    LSA_UINT16                          NicId,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE   pDcpInd,
    LSA_UINT8   OHA_LOCAL_MEM_ATTR*     ReqPtr,
    LSA_INT                             ReqLen
);
#endif

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_RES
 * function:       Trace End of SET Request
 *                 Called after calling all "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 * RspPtr:      -> SET response data
 *                 LSA_INT     RspLen:      Length of response
 * return value:   -
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_RES
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_REPORT_DCP_SET_RES(
    LSA_UINT16                    NicId,
    OHA_DCP_INDICATION_TYPE       DcpInd,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* RspPtr,
    LSA_INT                       RspLen
);
#endif

/*=============================================================================
 * function name:  OHA_DO_TIMER_REQUEST
 *
 * function:       Does a OHA_system() request with the spezified RQB. This
 *                 function is called within OHA timer handler to signal
 *                 a Timer event to the main context of OHA.
 *                 Systemadaption has to call OHA_system() with the specified
 *                 RQB. Before calling, the systemadaption should change to
 *                 taskstate.
 *
 * parameters:     OHA_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/

#ifndef OHA_DO_TIMER_REQUEST
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DO_TIMER_REQUEST(
    OHA_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  OHA_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     OHA_UPPER_CALLBACK_FCT_PTR_TYPE 
 *                                         oha_request_upper_done_ptr:
 *                     pointer to oha_request_upper_done_ptr, given by
 *                     oha_open_channel()
 *
 *                 OHA_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE        sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef OHA_REQUEST_UPPER_DONE
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_REQUEST_UPPER_DONE(
    OHA_UPPER_CALLBACK_FCT_PTR_TYPE  oha_request_upper_done_ptr,
    OHA_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file OHA_SYS.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_SYS_H */
