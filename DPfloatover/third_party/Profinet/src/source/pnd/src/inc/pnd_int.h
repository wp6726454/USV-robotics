#ifndef PND_INT_H
#define PND_INT_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

/* PSI and LSA types */
#include "psi_cfg.h"        // Base system

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "ltrc_if.h"

#include "psi_usr.h"
#include "psi_sys.h"

/* EPS framework types */
#include "eps_rtos.h"
#include "eps_sys.h"
#include "eps_timer.h"

/* IO Base */
#include "pniousrx.h"


/*===========================================================================*/
//Product specific defines
/*===========================================================================*/

#define PND_MLFB                                 "6ES7195-3AA00-0YA0"
#define PND_VERSION_REVISION_PREFIX             'V'
#define PND_VERSION_FUNCTIONAL_ENHANCEMENT       1
#define PND_VERSION_BUG_FIX                      1
#define PND_VERSION_INTERNAL_CHANGE              0

/*------------------------------------------------------------------------------
//    OPCODES
//    PND_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/


#define PND_OPC_STARTUP             1   /* PND startup IOB and PNUC  */
#define PND_OPC_SHUTDOWN            2   /* PND shutdown IOB and PNUC */

#define PND_OPC_CONTROLLER_OPEN     3
#define PND_OPC_CONTROLLER_CLOSE    4

#define PND_OPC_REC_READ            5
#define PND_OPC_REC_WRITE           6

#define PND_OPC_ALARM_PROVIDE       7   /* Provide Alarm IND RQBs         */
#define PND_OPC_ALARM_INDICATION    8   /* Indicate Alarm                 */
#define PND_OPC_ALARM_RESPONSE      9   /* Alarm response, handled by IOB */

#define PND_OPC_SET_MODE           10
#define PND_OPC_DEVICE_ACTIVATE    11

#define PND_OPC_PI_ALLOC           12
#define PND_OPC_PI_FREE            13

#define PND_OPC_INTERNAL_TIMER     14

#define PND_OPC_PNUC_INTERNAL_REQ  15   /* PNIO user core internal request */

#define PND_OPC_DIAG_REQ		   16   /* PNIO diagnostic service request */
#define PND_OPC_SET_ADDRESS        17   /* PNIO Set IP and NoS request */
#define PND_OPC_REMA_IND           18   /* Remanent Indication */
#define PND_OPC_INTERFACE_OPEN     19  //PNDriver-PDEV Interface
#define PND_OPC_INTERFACE_CLOSE    20  //PNDriver-PDEV Interface

#define PND_OPC_INTERFACE_OPEN     19
#define PND_OPC_INTERFACE_CLOSE    20

#define PND_OPC_INTERFACE_OPEN     19
#define PND_OPC_INTERFACE_CLOSE    20

#define PND_OPC_REMA_PROVIDE       21   /* Provide Rema IND RQBs         */
#define PND_OPC_REMA_RESPONSE      22   /* Rema response, handled by IOB */

#define PND_OPC_PRM_WRITE          23   /* OHA parameter write */

/*===========================================================================*/
// Additional internal ERROR_CODES to pnioerrx.h
/*===========================================================================*/

#define PNIO_OK_CANCEL      0x00000401

/*===========================================================================*/
/*                                 defies                                    */
/*===========================================================================*/

#define PNIOI_DREC_MAX_SIZE (32*1024) // Pndriver support up to 32k

#define PND_PNIO_MAX_RESOURCES  9 // Max PND Resources (set_mode / dev_activate)
#define PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES  16 // Max PND Record read&write resources

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* stringify                                                                 */
/*---------------------------------------------------------------------------*/

#define PND_STRINGIFY_(n)  # n
#define PND_STRINGIFY(n)   PND_STRINGIFY_(n) /* make it a string */

/*---------------------------------------------------------------------------*/
/* pragma message                                                            */
/* intended usage: #pragma PND_MESSAGE(...)                                  */
/*---------------------------------------------------------------------------*/

#if defined (TOOL_CHAIN_MICROSOFT)  /* Microsoft, pragma-args are expanded */
#define PND_MESSAGE(str_) message( __FILE__ "(" PND_STRINGIFY(__LINE__) ") : " str_)
#undef PND_MESSAGE /* suppres compiler messages */

#elif defined  (TOOL_CHAIN_GNU)
#undef PND_MESSAGE

#elif defined  (TOOL_CHAIN_RM_GNU)
#undef PND_MESSAGE

#endif

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef PND_FILE_SYSTEM_EXTENSION
# define PND_FILE_SYSTEM_EXTENSION( module_id_ )
#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define pnd_is_null( ptr )        (LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define pnd_is_not_null(ptr)      (!LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define pnd_are_equal(ptr1,ptr2)  (LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

/*===========================================================================*/
/*   basic macros mapped to EPS framework                                    */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
#define PND_GPIO_NR_UNDEFINED  0xFFFF  // Initial value for GPIO number (not set)
#define PND_TIMER_ID_INVALID   0xFFFF  // Initial value for timer ID (not set(
#define PND_LADDR_INVALID      0xFFFF  // Invalid LADDR
#define PND_IO_OFFSET_INVALID  0xFFFF  // Invalid IO offset

/*----------------------------------------------------------------------------*/
/* Types for tasks                                                            */
/*----------------------------------------------------------------------------*/

typedef enum _PND_TASK_ID
{
    PND_IOBASE_CORE_TASK_ID,   /* IOBase core Task   */
    PND_PN_USER_CORE_TASK_ID,  /* PN user core Task */
    PND_TASK_ID_MAX      /* don't move, has to be the last entry! */
} PND_TASK_ID;

typedef void (*PND_USER_CBF)( LSA_VOID_PTR_TYPE pCbfPrm );
typedef LSA_VOID (*PND_CORE_DONE_FCT)( LSA_VOID_PTR_TYPE p_this, LSA_VOID_PTR_TYPE p_rqb );


/*----------------------------------------------------------------------------*/
/* Access to user defined LSA Header fields                                   */
/*----------------------------------------------------------------------------*/
#define PND_RQB_SET_USER_INST_PTR(rb_, val_)  {(rb_)->_user_instance = (val_);}
#define PND_RQB_GET_USER_INST_PTR(rb_)        ((rb_)->_user_instance)

#define PND_RQB_SET_USER_CBF_PTR(rb_, val_)  {(rb_)->_user_func = (val_);}
#define PND_RQB_GET_USER_CBF_PTR(rb_)        ((rb_)->_user_func)

/*---------------------------------------------------------------------------*/

#define PND_LADDR_MAX  0x8000  // 32K address space

typedef enum pnd_ind_ref_type
{
    PNIO_REF_IOC,
    PNIO_REF_IFC
} PND_IND_REF_TYPE;

typedef enum pnd_laddr_type
{
    UNKNOWN = 0,
    PDSUBSLOTINTERFACE,
	PDSUBSLOTPORT,
    IOCDEVICE,
	IOCSLOT,
    IOCSUBSLOT
}PND_LADDR_TYPE;

typedef struct rema_record_type { 
    PNIO_UINT8*  recordData;          
    PNIO_UINT32  recordLen;           
    PNIO_UINT32  recordType;
} PND_REMA_RECORD_TYPE, *PND_REMA_RECORD_PTR_TYPE;

typedef struct rema_data_type { 
    PND_REMA_RECORD_TYPE  ipSuiteRecord;          
    PND_REMA_RECORD_TYPE  nosRecord;           
    PND_REMA_RECORD_TYPE mib2Record;
} PND_XML_REMA_DATA_TYPE, *PND_XML_REMA_DATA_PTR_TYPE;

typedef struct pnd_laddr_tag { // used for LADDR to UserNode mapping
    LSA_VOID_PTR_TYPE laddr_ref[PND_LADDR_MAX];  // LADDR array
    LSA_UINT8    laddr_type[PND_LADDR_MAX]; 
	LSA_UINT16   laddr_device_nr[PND_LADDR_MAX];  // LADDR used for Device Number mapping
	LSA_UINT16   laddr_slot_nr[PND_LADDR_MAX];    // LADDR used for Slot Number mapping
	LSA_UINT16   laddr_subslot_nr[PND_LADDR_MAX]; // LADDR used for Subslot Number mapping
} PND_PNUC_LADDR_TYPE, *PND_PNUC_LADDR_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct pnd_handle_tag { // used for mapping CP-ID to HD/IF 
    LSA_UINT16  cp_nr;           // CP number (from user)
    LSA_UINT16  hd_nr;           // HD number [1..N]
    LSA_UINT16  if_nr;           // IF number (logical number for PN IF)
    LSA_UINT16  pnio_if_nr;      // PNIO IF number [0..16](0=global, 1-16=PNIO IF)
    LSA_UINT16  edd_comp_id;     // LSA_COMP_ID of used EDDx
} PND_HANDLE_TYPE, *PND_HANDLE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef struct pnd_param_startup_tag { // used for startup PND system
    LSA_BOOL         decentral_io_system;             // check if decentral io system present in xml file
    LSA_UINT16       nr_of_cp;                        // count of CP handle entries
    PND_HANDLE_TYPE  pnd_handle[PSI_CFG_MAX_HD_CNT];  // PND handle
} PND_PARAM_STARTUP_TYPE, *PND_PARAM_STARTUP_PTR_TYPE;
    
/*---------------------------------------------------------------------------*/
typedef struct pnd_param_hif_ld_tag  // used to get the params for HID LD open
{
    LSA_UINT32          hd_count;                      // Count of used HD
    HIF_HD_INPUT_TYPE   hd_param[PSI_CFG_MAX_HD_CNT];  // HIF HD params
} PND_PARAM_HIF_LD_TYPE, *PND_PARAM_HIF_LD_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_psi_ld_tag  // used to get the params for PSI LD open
{
    LSA_UINT32          hd_count;                       // Count of used HD
    PSI_IM_INPUT_TYPE   im_param;                       // PSI I&M params
    PSI_HD_INPUT_TYPE   hd_param[PSI_CFG_MAX_HD_CNT];   // PSI HD params
} PND_PARAM_PSI_LD_TYPE, *PND_PARAM_PSI_LD_PTR_TYPE; 

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_pnio_if_tag // used to get the IF params for one PNIO IF
{
    LSA_UINT8   edd_if_id;              // EDD IF ID (PNIO IF nr)
    LSA_BOOL    check_ip_enabled;          // Check option for IP Suite (OHA)
    LSA_UINT16  check_interval;         // Check interval (for IP Suite)
    LSA_UINT16  vendor_id;              // Vendor ID
    LSA_UINT16  device_id;              // Device ID
    LSA_UINT16  max_alarm_data_length;  // Max alarm data size
    LSA_UINT16  send_clock;             // send clock

} PND_PARAM_PNIO_IF_TYPE, *PND_PARAM_PNIO_IF_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_param_prm_entry_tag // for PD PRM records
{
    LSA_UINT16 slot_nr;              // slot nr
    LSA_UINT16 subslot_nr;           // subslot nr
    LSA_UINT32 record_index;         // record index
    LSA_UINT32 record_data_length;   // size of record data  
    LSA_UINT8* record_data;          // record data buffer

} PND_PARAM_PRM_RECORD_TYPE, *PND_PARAM_PRM_RECORD_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_pd_sub_entry_tag  // used to get PDev subslot info
{
    LSA_UINT16 slot_nr;     // slot nr
    LSA_UINT16 subslot_nr;  // subslot nr
    LSA_UINT32 mod_ident;   // mod ident
    LSA_UINT32 sub_ident;   // submod ident
    LSA_UINT16 laddr;       // logical address
}
PND_PARAM_PD_SUB_ENTRY_TYPE, *PND_PARAM_PD_SUB_ENTRY_PTRY_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_pd_hd_if_tag  // used to get the IF params for PD user
{
    LSA_UINT16                  instance;                          // instance
    LSA_UINT16                  device_id;                         // device ID
    LSA_UINT16                  vendor_id;                         // vendor ID
    LSA_UINT16                  map_count;                         // Nr. of PDev entries for mappings
    LSA_UINT16                  pdev_validation;                   // PDEV configuration local yes/no
    PND_PARAM_PD_SUB_ENTRY_TYPE pdev_sub[PSI_CFG_MAX_PORT_CNT+1];  // PDEV subslot entries for mapping
} PND_PARAM_PD_HD_IF_TYPE, *PND_PARAM_PD_HD_IF_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_oha_hd_if_tag  // used to get the IF params for OHA user
{
    LSA_UINT16  nr_of_usr_ports;                                   // real port count
    LSA_UINT8   edd_if_id;                                         // EDD IF ID (PNIO IF nr)
    LSA_BOOL    check_ip_address;                                  // Check IP Suite yes/no
    LSA_UINT16  vendor_id;                                         // Device ID
    LSA_UINT16  device_id;                                         // Vendor ID
	LSA_BOOL    IPAddrValAllowed;                                  // IPAddressValidationLocal
    LSA_BOOL    NameOfStationValAllowed;                           // NameOfStationValidation
} PND_PARAM_OHA_HD_IF_TYPE, *PND_PARAM_OHA_HD_IF_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_ioc_hd_if_tag  // used to get the IF params for IOC user
{
    LSA_UINT16  device_count;            // Nr. of configured remote device
    LSA_UINT16  max_alarm_data_length;   // Max alarm data size
    LSA_UINT16  max_devices;             // Max devices
    LSA_UINT16  instance;                // instance
    LSA_UINT16  device_id;               // device ID
    LSA_UINT16  vendor_id;               // vendor ID
    LSA_UINT16  sr_ar_nr;                // System redundancy AR nr (used by CM-CL)
} PND_PARAM_IOC_HD_IF_TYPE, *PND_PARAM_IOC_HD_IF_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_subslot_tag  // used to get the subslot params for one device
{
    LSA_UINT32   api;                 // API
    LSA_UINT16   slot_nr;             // Slot
    LSA_UINT32   slot_ident;          // Slot ident
    LSA_UINT16   slot_laddr;          // Slot LADDR
    LSA_UINT16   subslot_nr;          // Subslot
    LSA_UINT32   subslot_ident;       // Subslot ident
    LSA_UINT16   subslot_laddr;       // Subslot LADDR
    LSA_UINT16   subslot_properties;  // Subslot properties (with all reduced bits)
    LSA_UINT16   input_length;        // Subslot input data length
    LSA_UINT32   input_base_laddr;    // iBase LADDR
    LSA_UINT16   output_length;       // Subslot output data length
    LSA_UINT32   output_base_laddr;   // qBase LADDR
    LSA_UINT32   reduction_ratio;     // Reduction ratio
    LSA_UINT32   phase;               // Phase
    LSA_UINT32   cycle_time;          // Cycle time
} PND_PARAM_SUBSLOT_TYPE, *PND_PARAM_SUBSLOT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct pnd_param_device_tag  // used to get the device params for one IOC
{
    LSA_BOOL    is_activate;       // Configured activ yes/no
    LSA_UINT16  vendor_id;         // VendorID
    LSA_UINT16  device_id;         // DeviceID
    LSA_UINT16  instance;          // Instance
    LSA_UINT16  device_laddr;      // Device LADDR
    LSA_UINT16  nr_of_subslot;     // Nr of subslots
    LSA_UINT16  device_number;     // Device Number (Station Number)
} PND_PARAM_DEVICE_TYPE, *PND_PARAM_DEVICE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* iobase core and pnio user core startup */
struct pnd_startup_tag
{
    PND_PARAM_STARTUP_TYPE startup_param;
    PND_USER_CBF           cbf;
    LSA_UINT32             pnio_err;
};

typedef struct pnd_startup_tag PND_STARTUP_TYPE;

typedef PND_STARTUP_TYPE* PND_STARTUP_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* iobase core and pnio user core shutdown */
struct pnd_shutdown_tag
{
    PND_USER_CBF           cbf;
    LSA_UINT32             pnio_err;
};

typedef struct pnd_shutdown_tag PND_SHUTDOWN_TYPE;

typedef PND_SHUTDOWN_TYPE* PND_SHUTDOWN_PTR_TYPE;
/*----------------------------------------------------------------------------*/

/* iobase core open */
struct pnd_iob_core_open_tag
{
    PND_HANDLE_TYPE    pnd_handle;      //IN
    LSA_UINT16         nr_of_alarm_res; // out
    LSA_VOID_PTR_TYPE  laddr_list_ptr;  // LADDR list for each IF
	LSA_VOID_PTR_TYPE  dev_number_list_ptr;  // device number list for each IF
	LSA_VOID_PTR_TYPE  slot_number_list_ptr; // slot number list for each IF
	LSA_VOID_PTR_TYPE  subslot_number_list_ptr; // subslot number list for each IF
    LSA_UINT32         pnio_err;        // out
};

typedef struct pnd_iob_core_open_tag PND_IOB_CORE_OPEN_TYPE;

typedef PND_IOB_CORE_OPEN_TYPE* PND_IOB_CORE_OPEN_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* iobase core close */
struct pnd_iob_core_close_tag
{
    PND_HANDLE_TYPE  pnd_handle;      //IN
    LSA_UINT16       param;    //In: HIF LD handle for hif_sys_request
    LSA_UINT32       pnio_err;
};

typedef struct pnd_iob_core_close_tag PND_IOB_CORE_CLOSE_TYPE;

typedef PND_IOB_CORE_CLOSE_TYPE* PND_IOB_CORE_CLOSE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* record read / write */
struct pnd_rec_read_write_tag
{
    PNIO_REF          UserRef;          // in
    PNIO_ADDR         Addr;             // in
    PNIO_UINT32       RecordIndex;      // in
    PNIO_UINT32       Length;           // in/out
    LSA_VOID_PTR_TYPE pBuffer; // Buffer[PNIOI_DREC_MAX_SIZE]; // in, out
    PNIO_ERR_STAT     err;
    PND_HANDLE_TYPE   pnd_handle;      //IN
    LSA_UINT32        pnio_err;
};

typedef struct pnd_rec_read_write_tag PND_RECORD_READ_WRITE_TYPE;

typedef PND_RECORD_READ_WRITE_TYPE *PND_RECORD_READ_WRITE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct pnd_event_data
{
    PNIO_UINT8 event_opcode;  /* cm event type */
    PNIO_UINT16 device_nr;   /* device number */
    PNIO_UINT16 ar_nr;       /* ar number */
    PNIO_UINT16 session_key; /* session key, see CM_OPC_CL_AR_OFFLINE_IND or CM_OPC_SV_AR_CONNECT_IND */
}
PND_CTRL_EVENT_DATA;



/* alarm provide */
struct pnd_alarm_tag
{
    PNIO_REF               ind_ref;
    PNIO_ADDR              addr;
    PNIO_CTRL_ALARM_DATA   alarm;
    PND_CTRL_EVENT_DATA    event_ind;
    PND_HANDLE_TYPE        pnd_handle;      //IN
    LSA_VOID_PTR_TYPE      SessionData;
    LSA_UINT32             pnio_err;
};

typedef struct pnd_alarm_tag PND_ALARM_TYPE;

typedef PND_ALARM_TYPE* PND_ALARM_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* set_mode */
struct pnd_set_mode_tag
{
    PNIO_ADDR          Addr;
    PNIO_MODE_TYPE     mode;
    PND_HANDLE_TYPE   pnd_handle;      //IN
    LSA_UINT32        pnio_err;
};

typedef struct pnd_set_mode_tag PND_SET_MODE_TYPE;

typedef PND_SET_MODE_TYPE* PND_SET_MODE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* activate */
struct pnd_dev_activate_tag
{
    PNIO_ADDR          Addr;
    PNIO_DEV_ACT_TYPE  DevActMode;
    PND_HANDLE_TYPE    pnd_handle;      //IN
    LSA_UINT32         pnio_err;
};

typedef struct pnd_dev_activate_tag PND_DEV_ACTIVATE_TYPE;

typedef PND_DEV_ACTIVATE_TYPE* PND_DEV_ACTIVATE_PTR_TYPE;


/*----------------------------------------------------------------------------*/

/* ------------- IODU services PI_ALLOC / PI_FREE  ---------------------------*/
typedef struct {               
  LSA_UINT32          IO_base_addr;        //IN don't care if length_IO_data 0
  LSA_UINT32          offset_IO_data;      //IN offset of IO data in frame buffer
  LSA_UINT32          length_IO_data;      //IN 0 indicates a dataless submodule (TODO: was ist bei unterdrückten IO daten?)
  LSA_UINT32          offset_IOCS;         //IN offset of IOCS data in output frame buffer (controller view)

} PND_IO_ADDR_TYPE;
typedef PND_IO_ADDR_TYPE * PND_IO_ADDR_PTR_TYPE;

typedef struct {
  LSA_BOOL        is_provider;          //IN
  LSA_UINT32      partial_length;       //IN 
  LSA_UINT32      apdu_status_length;   //IN
  LSA_BOOL        is_dfp;               //IN
  
  LSA_VOID_PTR_TYPE   buffer_handle;        //OUT
  LSA_UINT32          edd_cr_number;        //OUT
  LSA_UINT32          edd_properties;       //OUT
  LSA_UINT32          edd_data_offset;      //OUT
  LSA_UINT32          edd_forward_mode;     //OUT
  LSA_UINT32          edd_ct_outdatastart;  //OUT

  LSA_UINT32              number_of_IO_addr;    //IN
  PND_IO_ADDR_PTR_TYPE    IO_addr_array;        //IN array of number_of_IO_addr elements

} PND_BUFFER_PROPERTIES_TYPE;
typedef PND_BUFFER_PROPERTIES_TYPE * PND_BUFFER_PROPERTIES_PTR_TYPE;


typedef struct {
  PND_HANDLE_TYPE               pnd_handle;         //IN
  LSA_UINT32                    device_nr;          //IN
  PND_BUFFER_PROPERTIES_TYPE    consumer_buffer;    //IN/OUT
  PND_BUFFER_PROPERTIES_TYPE    provider_buffer;    //IN/OUT

  LSA_UINT32                    pnio_err;           //OUT

}PND_PI_ALLOC_TYPE;
typedef PND_PI_ALLOC_TYPE * PND_PI_ALLOC_PTR_TYPE;


typedef struct {
  PND_HANDLE_TYPE       pnd_handle;                 //IN
  LSA_VOID_PTR_TYPE     consumer_buffer_handle;     //IN
  LSA_VOID_PTR_TYPE     provider_buffer_handle;     //IN

  LSA_UINT32            pnio_err;                   //OUT

}PND_PI_FREE_TYPE;
typedef PND_PI_FREE_TYPE * PND_PI_FREE_PTR_TYPE;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/* ----- PNIO user core internal requests (i.E. startup/shutdown subsys)----- */
struct pnd_pnuc_internal_tag
{
    LSA_UINT32        pnio_err;
    LSA_UINT16        nr_of_alarm_ind;
    LSA_VOID_PTR_TYPE data_ptr;
};

typedef struct pnd_pnuc_internal_tag PND_PNUC_INTERNAL_TYPE;
typedef PND_PNUC_INTERNAL_TYPE* PND_PNUC_INTERNAL_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* PNIO diagnostic service request */
typedef struct pnd_diagnostic_request_tag
{
    PNIO_CTRL_DIAG*             DiagReq;
	PND_HANDLE_TYPE             pnd_handle;
	PNIO_UINT32                 DiagDataBufferLen;
    PNIO_UINT8 *                pDiagDataBuffer;    /* cast to PNIO_CTRL_DIAG_CONFIG_SUBMODULE[...] or
                                                    to PNIO_CTRL_DIAG_DEVICE[1] */
    LSA_UINT32                  pnio_err;
}PND_DIAG_REQ_TYPE;

typedef PND_DIAG_REQ_TYPE* PND_DIAG_REQ_PTR_TYPE;

typedef struct pnd_set_ip_and_nos_tag
{
	PNIO_SET_IP_NOS_MODE_TYPE mode;
	PNIO_IPv4                 IpV4;
	PNIO_NOS                  NoS;
	PNIO_UINT32               Options;
	PND_HANDLE_TYPE           pnd_handle;
	LSA_UINT32                pnio_err;
}PND_SET_IP_AND_NOS_TYPE;

typedef struct pnd_rema_data_tag
{
	PND_HANDLE_TYPE             pnd_handle;         /* IN */
    PNIO_UINT32                 index;              /* REMA_READ: IN(0xF8000000, 0xF8000001) */
													/* REMA_IND: OUT(0xF8000000, 0xF8000001) */
    PNIO_UINT32                 data_length;        /* IN/OUT */
    PNIO_UINT8 *                data;               /* OUT */
    PNIO_UINT16                 data_tag;           /* OUT */
	PNIO_UINT8 *                remaXMLbuffer;
	PNIO_UINT32                 remaXMLbufferLength;
	LSA_UINT32                  pnio_err;
} PND_REMA_DATA_TYPE;

typedef PND_REMA_DATA_TYPE* PND_REMA_DATA_PTR_TYPE;

typedef struct pnd_iosystem_reconfig_tag
{
	PNIO_ADDR          Addr;
    PNIO_DEV_ACT_TYPE  Mode;
    PNIO_UINT32        pnio_err;
} PND_IOSYS_RECONFIG_TYPE;

typedef PND_IOSYS_RECONFIG_TYPE* PND_IOSYS_RECONFIG_PTR_TYPE;

typedef struct pnd_prm_write_data_tag
{
    PND_HANDLE_TYPE             pnd_handle;
    PNIO_UINT16                 slot_number;
    PNIO_UINT16                 subslot_number;
    PNIO_UINT16                 edd_port_id;
    PNIO_UINT32                 record_index;
    PNIO_UINT8                  mk_rema;  /* LSA_TRUE:  write data to REMA   */
									      /* LSA_FALSE: no write to REMA     */
} PND_PRM_WRITE_DATA_TYPE;

typedef union pnd_rqb_args_tag
{
    PND_STARTUP_TYPE            startup;
    PND_SHUTDOWN_TYPE           shutdown;
    PND_IOB_CORE_OPEN_TYPE      iob_open;
    PND_IOB_CORE_CLOSE_TYPE     iob_close;
    PND_ALARM_TYPE              iob_alarm;
    PND_SET_MODE_TYPE           iob_set_mode;
    PND_DEV_ACTIVATE_TYPE       iob_dev_act;
    PND_RECORD_READ_WRITE_TYPE  rec_read;        /* record read             */
    PND_RECORD_READ_WRITE_TYPE  rec_write;       /* record write            */
    PND_PI_ALLOC_TYPE           pi_alloc;        /* PI alloc                */
    PND_PI_FREE_TYPE            pi_free;         /* PI free                 */
    PND_PNUC_INTERNAL_TYPE      pnuc_internal;   /* PNIO user core internal */
	PND_DIAG_REQ_TYPE			diag_req;        /* PNIO diagnostic request	*/
	PND_SET_IP_AND_NOS_TYPE     set_ip_and_nos;  /* set ip and nos service  */
	PND_REMA_DATA_TYPE          rema_data;       /* REMA read / indication  */
    PND_IOSYS_RECONFIG_TYPE     iosys_recon;     /* IOSystem reconfig */
    PND_PRM_WRITE_DATA_TYPE     prm_write;       /* OHA parameter write */

} PND_RQB_ARGS_TYPE;

typedef struct pnd_rqb_tag *PND_RQB_PTR_TYPE;

/*----------------------------------------------------------------------------*/

#define PND_RQB_HEADER    LSA_RQB_HEADER(PND_RQB_PTR_TYPE)
#define PND_RQB_TRAILER   LSA_RQB_TRAILER

struct pnd_rqb_tag
{
    PND_RQB_HEADER
    PND_RQB_ARGS_TYPE args;         /* IN/OUT: Args of Request     */
    PND_RQB_TRAILER
};

typedef struct pnd_rqb_tag            PND_RQB_TYPE;

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PND_INT_H */
