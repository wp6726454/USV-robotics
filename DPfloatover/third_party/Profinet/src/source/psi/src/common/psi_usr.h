#ifndef PSI_USR_H                   /* ----- reinclude-protection ----- */
#define PSI_USR_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcodes within RQB (System requests)                                      */
/*---------------------------------------------------------------------------*/

#define PSI_OPC_LD_OPEN_DEVICE      1
#define PSI_OPC_LD_CLOSE_DEVICE     2
#define PSI_OPC_HD_OPEN_DEVICE      3
#define PSI_OPC_HD_CLOSE_DEVICE     4

/*---------------------------------------------------------------------------*/
/*  Response within RQB (Response)                                           */
/*---------------------------------------------------------------------------*/

#define PSI_OK               LSA_RSP_OK               /* 0x01       */
#define PSI_OK_CANCEL        (LSA_RSP_OK_OFFSET + 1)  /* 0x41 /  65 */

#define PSI_ERR_RESOURCE     LSA_RSP_ERR_RESOURCE     /* 0x84 / 132 */
#define PSI_ERR_PARAM        LSA_RSP_ERR_PARAM        /* 0x86 / 134 */
#define PSI_ERR_SEQUENCE     LSA_RSP_ERR_SEQUENCE     /* 0x87 / 135 */
#define PSI_ERR_SYS_PATH     LSA_RSP_ERR_SYS_PATH     /* 0x91 / 145 */

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

#define PSI_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1)

#define PSI_NRT_POOL_HANDLE_INVALID ((LSA_INT)0)

#define PSI_PROVIDER_GROUP_ID_CMCL  0   // group ID for CM-CL Provider (IOC)
#define PSI_PROVIDER_GROUP_ID_CMSV  1   // group ID for CM-SV Provider (IOD)
#define PSI_PROVIDER_GROUP_ID_CMMC  2   // group ID for CM-MC Provider (IOM)

#define PSI_MAX_STATION_NAME_LEN  255   // Max size of NoS (see oha_usr.h for OHA_MAX_STATION_NAME_LEN)

#define PSI_GLOBAL_PNIO_IF_NR       0   // PNIO IF Number for global channels
#define PSI_MAX_PNIO_IF_NR         16   // greatest PNIO IF Number for IF specific channels
                                        // note IF Nr [1..16] (see possible EDD IF Nr)

/*  Definition for external I2C HW multiplexer selection in PSI (=> EDD variable I2CMuxSelect) */
#define PSI_I2C_MUX_SELECT_NONE     0   // Default: no I2C HW multiplexer used (i.e.: no POF port)
                                        // otherwise: HwPortId is used for I2CMuxSelect for selection of an external I2C HW multiplexer

/*-----------------------------------------------------------------------------
// forward declarations / common types
//---------------------------------------------------------------------------*/

typedef struct psi_rqb_tag * PSI_UPPER_RQB_PTR_TYPE;

/*===========================================================================*/
/*                              basic types for PSI                          */
/*===========================================================================*/

#define PSI_MAC_ADDR_SIZE  6
typedef LSA_UINT8 PSI_MAC_TYPE[PSI_MAC_ADDR_SIZE];  /* MAC Type */

/*----------------------------------------------------------------------------*/
/* Types for LD params                                                        */
/*----------------------------------------------------------------------------*/

typedef enum psi_ld_runs_type_enum { 
	PSI_LD_RUNS_ON_UNKNOWN   = 0x00,  // reserved for ínvalid
	PSI_LD_RUNS_ON_LIGHT     = 0x01,  // LD runs on APP level (i.E.: pcIOX )
	PSI_LD_RUNS_ON_ADVANCED  = 0x02,  // LD runs on advanced level (i.E.: High perfomance system) 
	PSI_LD_RUNS_ON_BASIC     = 0x03,  // LD runs on basic level (i.E.: as FW on HD)
	PSI_LD_RUNS_ON_MAX

} PSI_LD_RUNS_ON_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD params                                                        */
/*----------------------------------------------------------------------------*/

typedef enum psi_asic_type_enum { // see also _ePNDEV_ASIC in PnDev_DriverU.h 
	PSI_ASIC_TYPE_UNKNOWN   = 0x00,  // reserved for invalid
	PSI_ASIC_TYPE_ERTEC400  = 0x01,  // ERTEC400 
	PSI_ASIC_TYPE_ERTEC200  = 0x02,  // ERTEC200
	PSI_ASIC_TYPE_SOC1      = 0x03,  // SOC1
	PSI_ASIC_TYPE_ERTEC200P = 0x04,  // ERTEC200+
	PSI_ASIC_TYPE_INTEL_XX  = 0x05,  // Intel I210, Hartwell
	PSI_ASIC_TYPE_KSZ88XX   = 0x06,  // Micrel KSZ88XX (1 or 2 Ports)
	PSI_ASIC_TYPE_MAX

} PSI_ASIC_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_port_type_enum { // see also EDD_MEDIATYPE_xx in edd_usr.h 
	PSI_MEDIA_TYPE_UNKNOWN  = 0x00,  // reserved for invalid
	PSI_MEDIA_TYPE_COPPER   = 0x01,  // media type copper cable 
	PSI_MEDIA_TYPE_FIBER    = 0x02,  // media type fiber cable (POF)
	PSI_MEDIA_TYPE_WIRELESS = 0x03,  // media type wireless
	PSI_MEDIA_TYPE_MAX

} PSI_PORT_MEDIA_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_ringport_type_enum { // see also EDD_MRP_RING_xx in edd_usr.h
	PSI_NO_RING_PORT      = 0x00,  // no ring port
	PSI_RING_PORT         = 0x01,  // possible ring port but no default
	PSI_RING_PORT_DEFAULT = 0x02,  // default ring port

} PSI_RINGPORT_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum psi_mrp_role_enum {  // see also EDD_MRP_ROLE_xx in edd_usr.h
	PSI_MRP_ROLE_NONE         = 0x00,  // no MRP
	PSI_MRP_ROLE_CLIENT       = 0x01,  // MRP role client
	PSI_MRP_ROLE_MANAGER      = 0x02,  // MRP role manager
	PSI_MRP_ROLE_AUTO_MANAGER = 0x03,  // MRP role auto manager

} PSI_MRP_ROLE_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD Selection                                                     */
/*----------------------------------------------------------------------------*/

typedef enum psi_hd_select_enum  // Possible HD ID key settings
{
	PSI_HD_SELECT_WITH_UNKNOWN  = 0,  // Unknwon Select method
	PSI_HD_SELECT_WITH_LOCATION = 1,  // HD selected by PCI Lcoation (i.E. PnDevDrv boards)
	PSI_HD_SELECT_WITH_MAC      = 2   // HD selected by MAC (i.E. StdMAC boards (WPCAP)

} PSI_HD_SELECT_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_pci_location_tag // for HD addressing over PCI location
{
	LSA_UINT16 bus_nr;       // Bus number
	LSA_UINT16 device_nr;    // Device number
	LSA_UINT16 function_nr;  // Function number

} PSI_PCI_LOCATION_TYPE, * PSI_PCI_LOCATION_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_hd_id_tag // for select HD at startup
{
	PSI_HD_SELECT_TYPE        hd_selection;  // Selector for board selection
	PSI_MAC_TYPE              hd_mac;        // MAC of StdMAC board (used by pcSRT, WPCAP)
	PSI_PCI_LOCATION_TYPE     hd_location;   // Location key of PN-IP hd (PnDevDrv boards)

} PSI_HD_ID_TYPE, * PSI_HD_ID_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for I&M input                                                        */
/*----------------------------------------------------------------------------*/

#define PSI_MAX_SIZE_IM_ORDER_ID     (20 + 1)     // Max size of order ID visible string with "\0"
#define PSI_MAX_SIZE_IM_DEVICE_TYPE  (25 + 1)     // Max size of device type visible string with "\0"
#define PSI_MAX_SIZE_IM_SERIAL_NR    (16 + 1)     // Max size of serial number visible string with "\0"

typedef struct psi_im_sw_version_input_tag
{
	LSA_UINT8 revision_prefix;          // revision prefix (i.E. T, P, ..)
	LSA_UINT8 functional_enhancement;   // revision number for functional enhancement
	LSA_UINT8 bug_fix;                  // revision number for bug fix
	LSA_UINT8 internal_change;          // revision number for internal change

} PSI_IM_SW_VERSION_TYPE, * PSI_IM_SW_VERSION_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_im_input_tag // I&M input data from INI (see PNIO spec for values)
{
	LSA_UINT16              vendor_id_high;                             // vendor ID high
	LSA_UINT16              vendor_id_low;                              // vendor ID low
	LSA_UINT16              hw_revision;                                // HW revision
	PSI_IM_SW_VERSION_TYPE  sw_version;                                 // SW rev with prefix and single rev nr.
	LSA_UINT16              revision_counter;                           // IM rev counter
	LSA_UINT16              profile_id;                                 // IM profile ID
	LSA_UINT16              profile_specific_type;                      // IM profile specific type
	LSA_UINT16              version;                                    // IM Version High and Low
	LSA_UINT16              supported;                                  // Bit field for supported I&M records
	LSA_UINT8               order_id[PSI_MAX_SIZE_IM_ORDER_ID];         // manufacturer specific visible string
	LSA_UINT8               device_type[PSI_MAX_SIZE_IM_DEVICE_TYPE];   // manufacturer specific visible string

} PSI_IM_INPUT_TYPE, * PSI_IM_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for Sock input                                                        */
/*----------------------------------------------------------------------------*/

typedef struct psi_sock_app_ch_detail_socket_option_tag
{
	LSA_BOOL SO_REUSEADDR_;      /* allow local address reuse */
	LSA_BOOL SO_DONTROUTE_;      /* just use interface addresses */
	LSA_BOOL SO_SNDBUF_;         /* send buffer size */
	LSA_BOOL SO_RCVBUF_;         /* receive buffer size */
	LSA_BOOL SO_LINGER_;         /* socket lingers on close */
	LSA_BOOL SO_TCPNODELAY_;     /* delay sending data (Nagle algorithm) */
	LSA_BOOL SO_TCPNOACKDELAY_;  /* delay sending ACKs */

} PSI_SOCK_APP_CH_DETAIL_SOCKET_OPTION_INPUT_TYPE;

typedef struct psi_sock_app_ch_detail_details_tag
{
	PSI_SOCK_APP_CH_DETAIL_SOCKET_OPTION_INPUT_TYPE socket_option;
	LSA_INT32 send_buffer_max_len;
	LSA_INT32 rec_buffer_max_len;
	LSA_INT16 sock_close_at_once;
	LSA_INT16 linger_time;

} PSI_SOCK_APP_CH_DETAIL_DETAILS_TYPE, * PSI_SOCK_APP_CH_DETAIL_DETAILS_PTR_TYPE;

typedef struct psi_sock_app_ch_detail_input_tag
{
	LSA_BOOL                            use_settings;   // Specify sock details or use default PSI settings
	PSI_SOCK_APP_CH_DETAIL_DETAILS_TYPE sock_detail;    // If not using default values fill in following details for sock app channels

} PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE, * PSI_SOCK_APP_CH_DETAIL_INPUT_PTR_TYPE;

typedef struct psi_sock_input_tag // Sock input data
{
	PSI_SOCK_APP_CH_DETAIL_INPUT_TYPE sock_app_ch_details[PSI_CFG_MAX_SOCK_APP_CHANNELS];

} PSI_SOCK_INPUT_TYPE, * PSI_SOCK_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD input                                                         */
/*----------------------------------------------------------------------------*/

typedef struct psi_irte_input_tag // IRTE input data from HW ini
{
	LSA_BOOL   use_setting;                 // use IRTE configuration or default PSI settings (only for EDDI)
	LSA_BOOL   irt_supported;               // IRT supported yes/no
	LSA_UINT16 nr_irt_forwarder;            // nr of IRT forwarder
	LSA_UINT16 reserved_ram_for_time;       // Time [us] for which KRAM should be reserved for each port
	LSA_BOOL   support_irt_flex;            // yes: IRT Flex parametrization is accepted; no: IRT Flex parametrization is rejected
    LSA_BOOL   disable_max_port_delay_check;// yes: checks against PDIRGlobalData.MaxPortRxDelay and PDIRGlobalData.MaxPortTxData are disabled. no: enable checks

} PSI_HD_IRTE_INPUT_TYPE, * PSI_HD_IRTE_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_pnip_input_tag // PNIP input data from HW ini
{
	LSA_BOOL   use_setting;         // use PNIP configuration or default PSI settings (only for EDDP)
	LSA_UINT16 k32fw_trace_level;   // Initial K32FW trace level

} PSI_HD_PNIP_INPUT_TYPE, * PSI_HD_PNIP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_stdmac_input_tag // EDDS input data from HW ini
{
	LSA_BOOL  use_setting;          // use StdMAC configuration or default PSI settings (only for EDDS)
    LSA_BOOL    gigabit_support;        // enable Gigabit-Support
    LSA_BOOL    nrt_copy_interface;     // enable NRT-Copy-Interface

} PSI_HD_STDMAC_INPUT_TYPE, * PSI_HD_STDMAC_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_hw_port_input_tag // All HW port specific INI inputs from HW INI
{
	LSA_UINT16             if_nr;                // IF-Nr which uses this port
	LSA_UINT16             user_nr;              // User Port number for IF
	PSI_RINGPORT_TYPE      mrp_ringport_type;    // None, Possible, Default

} PSI_HD_PORT_INPUT_TYPE, * PSI_HD_PORT_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Types for HD-IF input                                                      */
/*----------------------------------------------------------------------------*/

typedef struct psi_ioc_input_tag    // CM-CL/CM-MC (IOC/IOM) input data for one HD IF
{
	LSA_BOOL   use_setting;             // use IOC configuration or default PSI settings
	LSA_UINT16 nr_of_rt_devices;        // nr of RT devices
	LSA_UINT16 nr_of_irt_devices;       // nr of IRT devices
	LSA_UINT16 nr_of_qv_provider;       // nr of QV provider
	LSA_UINT16 nr_of_qv_consumer;       // nr of QV consumer
	LSA_UINT16 logical_addr_space;      // logical address space
	LSA_UINT16 nr_of_submod;            // nr of submodules
	LSA_UINT16 max_iocr_data_size;      // max IO-CR data size
	LSA_UINT16 max_mcr_data_size;       // max MC-R data size
	/* Input settings from PNIO configuration */
	LSA_UINT32 max_record_length;       // max record length (min = 4k)
	LSA_UINT16 max_alarm_data_length;   // max alarm data length
	LSA_UINT16 max_read_implicit;       // max read implict 

} PSI_HD_IF_IOC_INPUT_TYPE, * PSI_HD_IF_IOC_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_iod_input_tag     // CM-SV (IOD) input data for one HD IF
{
	LSA_BOOL   use_setting;        // use IOD configuration or default PSI settings
	LSA_UINT16 nr_of_instances;    // nr of parallel device instances
	LSA_UINT16 nr_of_ar;           // nr of parallel AR for each instance
	LSA_UINT16 max_icr_data_size;  // max I-CR data size for each AR
	LSA_UINT16 max_ocr_data_size;  // max O-CR data size for each AR
	LSA_UINT16 max_mcr_data_size;  // max M-CR data size for each AR
	/* Input settings from PNIO configuration */
	LSA_UINT32 max_record_length;       // max record length (min = 4k)
	LSA_UINT16 max_alarm_data_length;   // max alarm data length
	LSA_UINT16 nr_of_read_implicit;     // max read implict for each instance

} PSI_HD_IF_IOD_INPUT_TYPE, * PSI_HD_IF_IOD_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_cba_input_tag    // CBA input data for one HD IF
{
	LSA_BOOL   use_setting;          // use CBA configuration or default PSI settings
	LSA_UINT16 nr_of_provider_con;   // nr of CBA provider connections
	LSA_UINT16 nr_of_consumer_con;   // nr of CBA consumer connections
	LSA_UINT16 nr_of_rt_provider;    // nr of CBA RT provider
	LSA_UINT16 nr_of_rt_consumer;    // nr of CBA RT consumer
	LSA_UINT16 data_volume_size;     // Data volume size

} PSI_HD_IF_CBA_INPUT_TYPE, * PSI_HD_IF_CBA_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_ip_input_tag    // IP input data for one HD IF
{
	LSA_BOOL   use_setting;           // use IP configuration or default PSI settings
	LSA_UINT16 nr_of_send;            // nr of Send resources
	LSA_UINT16 nr_of_arp;             // nr of ARP receive resources
	LSA_UINT16 nr_of_icmp;            // nr of ICMP receive resources
	LSA_UINT16 nr_of_udp;             // nr of UDP receive resources
	LSA_UINT16 nr_of_tcp;             // nr of TCP receive resources
	LSA_BOOL   multicast_support_on;  // MC support turned on yes/no

} PSI_HD_IF_IP_INPUT_TYPE, * PSI_HD_IF_IP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_oha_input_tag  // OHA input data for one HD IF
{
	LSA_BOOL   use_setting;                                  // use OHA configuration or default PSI settings
	/* Input settings from PNIO configuration */
	LSA_UINT16 vendor_id;                                    // Device ID
	LSA_UINT16 device_id;                                    // Vendor ID
    LSA_UINT8  sys_descr[256];                                // System Description
    LSA_UINT8  if_descr[256];                                 // Interface Description
	LSA_UINT32 instance_opt;                                 // Device Instance (0..0xFFFF, 0xFFFFFFFF=not-valid)
	LSA_UINT16 check_interval;                               // Interval for IP check (see oha_sys.h)
	LSA_BOOL   check_ip_enabled;                             // Enable IP check
	LSA_BOOL   nos_allow_upper_cases;                        // Enable/Disable conversion of capital letters in NoS

} PSI_HD_IF_OHA_INPUT_TYPE, * PSI_HD_IF_OHA_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_mrp_input_tag // MRP input data for one HD IF
{
	LSA_BOOL          use_setting;             // use MRP configuration or default PSI settings
	LSA_UINT8         max_instances;           // max instances [1..N]
	LSA_UINT8         supported_role;          // Supported MRP roles for instance0
	LSA_UINT8         supported_multiple_role; // Supported MRP roles for instanceN (!= 0)
	PSI_MRP_ROLE_TYPE default_role_instance0;  // default-role for instance0

} PSI_HD_IF_MRP_INPUT_TYPE, * PSI_HD_IF_MRP_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_nrt_input_tag
{
	LSA_BOOL          RxFilterUDP_Unicast;   // Unicast UDP filtering, refer to EDDx Spec
	LSA_BOOL          RxFilterUDP_Broadcast; // Multicast UDP filtering, refer to EDDx Spec

} PSI_HD_IF_NRT_INPUT_TYPE, * PSI_HD_IF_NRT_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_hd_if_input_tag // All PNIO IF specific inputs for one HD IF
{
	LSA_UINT32                if_id;              // identifier for this interface
	LSA_UINT16                edd_if_id;          // EDD IF ID (=PNIO IF ID for PSI)
	LSA_UINT32                trace_idx;          // Trace Index used for traces of this interface
	PSI_HD_IF_IOC_INPUT_TYPE  ioc;                // CMCL (IOC) input data
	PSI_HD_IF_IOD_INPUT_TYPE  iod;                // CMSV (IOD) input data
	PSI_HD_IF_CBA_INPUT_TYPE  cba;                // CBA input data
	PSI_HD_IF_IP_INPUT_TYPE   ip;                 // TCIP input data
	PSI_HD_IF_OHA_INPUT_TYPE  oha;                // OHA input data
	PSI_HD_IF_MRP_INPUT_TYPE  mrp;                // MRP input data
	PSI_HD_IF_NRT_INPUT_TYPE  nrt;

} PSI_HD_IF_INPUT_TYPE, * PSI_HD_IF_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_hd_input_tag // HD input configuration for one HD
{
	LSA_UINT16      hd_id;                          // HD ID
	PSI_HD_ID_TYPE  hd_location;                    // HD address information (PCI, MAC)
	LSA_BOOL        runs_on_level_basic;            // false : run_level = LD; true : run_level = HD
	LSA_UINT16      nr_of_ports;                    // nr of ports (over all HD IF)
	LSA_UINT16      nr_of_if;                       // nr of interfaces for this HD
    LSA_UINT16      nr_of_all_ports;                // nr of ports over all IFs,  PSI LD sets this value
    LSA_UINT16      nr_of_all_if;                   // nr of all IFs,             PSI LD sets this value
	LSA_UINT16      edd_type;                       // EDDx type (LSA-COMP-ID) for this HD
	LSA_UINT16      asic_type;                      // ASIC type for this HD
	LSA_UINT16      rev_nr;                         // Rev number for this HD
	LSA_BOOL        allow_none_pnio_mautypes;       // 10Mbps and half-duplex (for all speeds) are allowed yes no
    LSA_UINT16      send_clock_factor;              // SendClockFactor = CycleBaseFactor
	PSI_MAC_TYPE    if_mac[PSI_CFG_MAX_HD_IF_CNT];  // interface mac
	PSI_MAC_TYPE    port_mac[PSI_CFG_MAX_PORT_CNT]; // port mac
	
	PSI_HD_IRTE_INPUT_TYPE   irte;                            // IRTE input data (only for EDDI)
	PSI_HD_PNIP_INPUT_TYPE   pnip;                            // PNIP input data (only for EDDP)
	PSI_HD_STDMAC_INPUT_TYPE stdmac;                          // StdMAC input data (only for EDDS)
	PSI_HD_PORT_INPUT_TYPE   hw_port[PSI_CFG_MAX_PORT_CNT];   // HW port settings (index = PortIndex)
	PSI_HD_IF_INPUT_TYPE     pnio_if[PSI_CFG_MAX_HD_IF_CNT];  // PNIO Interface setting

} PSI_HD_INPUT_TYPE, * PSI_HD_INPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_hw_port_output_tag // All HW port specific outputs
{
	PSI_PORT_MEDIA_TYPE    media_type;           // Media type (copper, fiber, wireless,..)

} PSI_HD_PORT_OUTPUT_TYPE, * PSI_HD_PORT_OUTPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_hd_if_output_tag // All PNIO IF specific calculated outputs
{
	LSA_UINT16   nr_of_usr_ports;     // Used ports for IF (used by CM-PD)
	LSA_UINT16   ioc_max_devices;     // Max Nr. of devices for CM-CL
	LSA_UINT16   iom_max_devices;     // Max Nr. of devices for CM-MC
	LSA_UINT16   iod_max_devices;     // Max Nr. of devices for CM-SV
	LSA_UINT16   iod_max_ar;          // Max Nr. of AR for one device
	LSA_UINT16   iod_max_ar_RT;       // Max Nr. of RT AR for one device
	LSA_UINT16   iod_max_ar_IRT;      // Max Nr. of IRT AR for one device
	LSA_UINT16   iod_max_ar_DAC;      // MAx Nr of Device access AR for one device
	LSA_UINT16   iod_max_ar_IOS;      // Max. Nr of IO Supervisor AR for one device

} PSI_HD_IF_OUTPUT_TYPE, * PSI_HD_IF_OUTPUT_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_hd_output_tag // HD output configuration for all HD
{
	PSI_HD_PORT_OUTPUT_TYPE hw_port[PSI_CFG_MAX_PORT_CNT];    // HW port settings
	PSI_HD_IF_OUTPUT_TYPE   pnio_if[PSI_CFG_MAX_HD_IF_CNT];   // PNIO Interface setting

} PSI_HD_OUTPUT_TYPE, * PSI_HD_OUTPUT_PTR_TYPE;

/*------------------------------------------------------------------------------
// PSI_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

typedef LSA_VOID LSA_FCT_PTR(,PSI_UPPER_CALLBACK_FCT_PTR_TYPE)(PSI_UPPER_RQB_PTR_TYPE pRQB);

struct psi_ld_open_tag
{
	LSA_UINT16                      hH;
	LSA_UINT32                      hd_count;                     //In: used element count of hd_args[]
	PSI_SOCK_INPUT_TYPE             sock_args;                    //In: Sock configuration values for LD
	PSI_IM_INPUT_TYPE               im_args;                      //In: I&M configuration values for LD
	PSI_HD_INPUT_TYPE               hd_args[PSI_CFG_MAX_HD_CNT];  //In: configuration of all HD
	PSI_HD_OUTPUT_TYPE              hd_out[PSI_CFG_MAX_HD_CNT];   //out: calculated config of all HD
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE psi_request_upper_done_ptr;   //In: Callback function
};

typedef struct psi_ld_open_tag PSI_LD_OPEN_TYPE;

typedef PSI_LD_OPEN_TYPE * PSI_UPPER_LD_OPEN_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_ld_close_tag
{
	LSA_UINT16                      hH;                           //In: HIF LD handle for hif_sys_request
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE psi_request_upper_done_ptr;   //In: Callback function
};

typedef struct psi_ld_close_tag PSI_LD_CLOSE_TYPE;

typedef PSI_LD_CLOSE_TYPE * PSI_UPPER_LD_CLOSE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_hd_open_tag
{
	LSA_UINT16                      hH;                              //In: HIF HD handle for hif_sys_request
	PSI_HD_INPUT_TYPE               hd_args;                         //In: identifier for one HD
	PSI_HD_OUTPUT_TYPE              hd_out;                          //out: calculated config of one HD
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE psi_request_upper_done_ptr;      //In: Callback function
};

typedef struct psi_hd_open_tag  PSI_HD_OPEN_TYPE;

typedef PSI_HD_OPEN_TYPE * PSI_UPPER_HD_OPEN_PTR_TYPE;

/*----------------------------------------------------------------------------*/

struct psi_hd_close_tag
{
	LSA_UINT16                      hH;                           //In: HIF HD handle for hif_sys_request
	LSA_UINT32                      hd_id;                        //In:  identifier for this hd
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE psi_request_upper_done_ptr;   //In: Callback function
};

typedef struct psi_hd_close_tag PSI_HD_CLOSE_TYPE;

typedef PSI_HD_CLOSE_TYPE * PSI_UPPER_HD_CLOSE_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef union psi_rqb_args_tag
{
	PSI_LD_OPEN_TYPE   ld_open;           /* LD open        */
	PSI_LD_CLOSE_TYPE  ld_close;          /* LD close        */
	PSI_HD_OPEN_TYPE   hd_open;           /* HD open        */
	PSI_HD_CLOSE_TYPE  hd_close;          /* HD close        */

} PSI_RQB_ARGS_TYPE;

typedef struct psi_rqb_tag
{
	PSI_RQB_HEADER
	PSI_RQB_ARGS_TYPE args;         /* IN/OUT: Args of Request     */
	PSI_RQB_TRAILER

} PSI_RQB_TYPE, * PSI_RQB_PTR_TYPE;

/*-----------------------------------
  Mailbox User functions
------------------------------------*/
typedef LSA_VOID (*PSI_REQUEST_FCT)( LSA_VOID_PTR_TYPE );

LSA_VOID psi_request_direct_start(  /* starting a PNIO request */
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      request_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_VOID psi_request_start(  /* starting a PNIO request */
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      request_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_VOID psi_request_local(  /* start local PNIO request */
	LSA_UINT16            mbx_id,
	PSI_REQUEST_FCT       req_func,
	LSA_VOID_PTR_TYPE     pRQB );

LSA_VOID psi_request_done(  /* handle PNIO callbacks (respones) */
	PSI_REQUEST_FCT      req_done_func,
	LSA_VOID_PTR_TYPE    pRQB,
	LSA_SYS_PTR_TYPE     sys_ptr );

LSA_VOID psi_system_request_done(  /* handle PNIO callbacks (system requests) */
	LSA_UINT16           mbx_id,
	PSI_REQUEST_FCT      req_done_func,
	LSA_VOID_PTR_TYPE    pRQB );

LSA_INT32 psi_thread_proc( LSA_VOID_PTR_TYPE arg, LSA_UINT32 psi_thread_id );

/*-----------------------------------
  PSI SYS Path Macros
------------------------------------*/

#define PSI_SYSPATH_SET_PATH(sys_path, val)  { sys_path |= ( val & 0x00FF );}
#define PSI_SYSPATH_SET_IF(sys_path, val)    { sys_path |= ( val << 8  ); }
#define PSI_SYSPATH_SET_HD(sys_path, val)    { sys_path |= ( val << 12 ); }

#define PSI_SYSPATH_GET_PATH(sys_path)       ( sys_path & 0x00FF )
#define PSI_SYSPATH_GET_IF(sys_path)         ( sys_path >> 8 & 0x000F )
#define PSI_SYSPATH_GET_HD(sys_path)         ( sys_path >> 12 )

/*  0x2345
      |||
      ||+---- 45 sys_path  
      |+-----  3 interface id
      |+-----  2 hardware device id 
*/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions PSI-LD  =====*/
/* System requests for Open-/Close-Device requests */
LSA_VOID psi_ld_system( PSI_RQB_PTR_TYPE rqb_ptr );

/* PNIO requests for delegating to PNIO component */
LSA_VOID psi_ld_open_channel (PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_ld_close_channel (PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_ld_request( PSI_RQB_PTR_TYPE rqb_ptr );

/*====  in functions PSI-HD  =====*/
/* System requests for Open-/Close-Device requests */
LSA_VOID psi_hd_system( PSI_RQB_PTR_TYPE rqb_ptr );

/* PNIO requests for delegating to PNIO component */
LSA_VOID psi_hd_open_channel( PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_hd_close_channel( PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_hd_request( PSI_RQB_PTR_TYPE rqb_ptr );

/* PNIO IR and HD functions */
LSA_VOID psi_hd_interrupt( LSA_UINT16 hd_nr, LSA_UINT32 int_src );
LSA_BOOL psi_hd_isr_poll( LSA_UINT16 hd_nr );
LSA_VOID psi_eddi_exttimer_interrupt( LSA_UINT16 hd_nr);

LSA_VOID psi_hd_get_edd_handle( LSA_UINT16 hd_nr, PSI_EDD_HDDB *edd_handle_ptr );
LSA_VOID_PTR_TYPE psi_hd_get_edd_io_handle( LSA_UINT16 hd_nr );
LSA_UINT32 psi_hd_get_real_pi_size( LSA_UINT16 hd_nr );

/* Alloc & Free functions */
LSA_VOID psi_usr_alloc_nrt_tx_mem( LSA_VOID_PTR_TYPE * mem_ptr_ptr, LSA_UINT32 length, LSA_UINT16 hd_nr, LSA_UINT16 comp_id );
LSA_VOID psi_usr_alloc_nrt_rx_mem( LSA_VOID_PTR_TYPE * mem_ptr_ptr, LSA_UINT32 length, LSA_UINT16 hd_nr, LSA_UINT16 comp_id );
LSA_VOID psi_usr_free_nrt_tx_mem( LSA_UINT16 * ret_val_ptr, LSA_VOID_PTR_TYPE mem_ptr, LSA_UINT16 hd_nr, LSA_UINT16 comp_id );
LSA_VOID psi_usr_free_nrt_rx_mem( LSA_UINT16 * ret_val_ptr, LSA_VOID_PTR_TYPE mem_ptr, LSA_UINT16 hd_nr, LSA_UINT16 comp_id );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of PSI_USR_H */