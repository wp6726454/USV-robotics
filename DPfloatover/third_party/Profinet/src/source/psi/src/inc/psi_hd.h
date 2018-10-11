#ifndef PSI_HD_H                /* ----- reinclude-protection ----- */
#define PSI_HD_H

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: psi_hd.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile for PSI HD instance                                  */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
// Note: the adaption HW info is used for the EDDx setup and adaption functions
typedef struct psi_hd_hw_tag { // HW information (from system) for one HD
	PSI_MAC_TYPE         if_mac[PSI_CFG_MAX_HD_IF_CNT];     // IF macs from system
	PSI_MAC_TYPE         port_mac[PSI_CFG_MAX_PORT_CNT];    // port MACs from system
	PSI_HD_PORT_MAP_TYPE port_map[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // User to HW port mapping (port index 0 is reserved) for each IF
	LSA_UINT16           port_media_type_select[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // media type selector for all ports for each IF: PSI_MEDIA_TYPE_SEL_xxx; port index = PortId (port index 0 is reserved)
	PSI_PORT_MEDIA_TYPE  port_media_type[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // media type for all ports for each IF: PSI_MEDIA_TYPE_xxx; port index = PortId (port index 0 is reserved)
	PSI_PORT_MEDIA_TYPE  port_media_type_by_HwPortId[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // media type for all ports for each IF: PSI_MEDIA_TYPE_xxx; port index = HwPortId (port index 0 is reserved)
    LSA_BOOL             b_edd_nrt_copy_if_on;              // EDDx NRT Copy Interface activation (with active EDDx NRT Copy Interface applications can use local memory instead of NRT memory)
	LSA_UINT16           edd_type;                          // EDDx type
	PSI_HD_MEM_TYPE      hif_mem;                           // HIF memory
	PSI_HD_MEM_TYPE      pi_mem;                            // PI memory used for CRT (SDRam, KRAM, IOCC)
	PSI_HD_MEM_TYPE      nrt_mem;                           // NTR memory  used for NRT Buffer (SDRam, SHM)
	PSI_HD_EDD_TYPE      edd;                               // EDDx HD information
} PSI_HD_HW_TYPE, *PSI_HD_HW_PTR_TYPE;

typedef struct psi_hd_if_tag { // Storage type for a HD IF 
	LSA_BOOL        is_used;           // HD IF used yes/no
	PSI_SYS_HANDLE  sys_handle;        // hSysIF from System for this IF
	PSI_EDD_HDDB    edd_hIDB;          // EDD HIDB from Interface OPEN
	LSA_UINT16      pnio_if_nr;        // PNIO IF nr for this IF
	LSA_BOOL        is_arp_on;         // ARP sniffing is turned on yes/no
} PSI_HD_IF_TYPE, *PSI_HD_IF_PTR_TYPE;

typedef struct psi_hd_store_tag {  // Storage type for a HD instance
	LSA_BOOL        is_used;                          // HD used yes/no
	LSA_UINT16      nr_of_if;                         // nr of used IF
    LSA_UINT32      nr_of_ports;                      // nr of used ports
    LSA_UINT16      nr_of_all_if;                     // nr of all IFs,             PSI LD sets this value
    LSA_UINT16      nr_of_all_ports;                  // nr of ports over all IFs,  PSI LD sets this value 
	LSA_BOOL        runs_on_level_basic;              // HD run location
	PSI_SYS_HANDLE  hd_sys_handle;                    // hSysDev from System for this HD
	PSI_EDD_HDDB    edd_hDDB;                         // EDD HDDB from Device OPEN
	LSA_INT         nrt_pool_handle;                  // NRT pool handle (RXTX pool, currently used for all upper layers)
    LSA_INT         nrt_dmacw_default_pool_handle;    // NRT pool handle (DMACW pool, EDDI)
    LSA_INT         nrt_rx_default_pool_handle;       // NRT pool handle (RX pool, EDDI)
    LSA_INT         nrt_tx_default_pool_handle;       // NRT pool handle (TX pool, EDDI)
    LSA_INT         nrt_tx_low_frag_pool_handle;      // NRT pool handle (TX_LOW fragments, EDDI)
    LSA_INT         nrt_tx_mid_frag_pool_handle;      // NRT pool handle (TX_MID fragments, EDDI)
    LSA_INT         nrt_tx_high_frag_pool_handle;     // NRT pool handle (TX_HIGH fragments, EDDI)
    LSA_INT         nrt_tx_mgmtlow_frag_pool_handle;  // NRT pool handle (TX_MGMTLOW fragments, EDDI)
    LSA_INT         nrt_tx_mgmthigh_frag_pool_handle; // NRT pool handle (TX_MGMTHIGH fragments, EDDI)
	PSI_HD_HW_TYPE  hd_hw;                            // HW information from system
	PSI_HD_IF_TYPE  hd_if[PSI_CFG_MAX_HD_IF_CNT];     // all IF instances [1..n]
} PSI_HD_STORE_TYPE, *PSI_HD_STORE_PTR_TYPE;

/*===========================================================================*/
/*                              prototypes                                   */
/*===========================================================================*/

LSA_VOID psi_hd_init( LSA_VOID );
LSA_VOID psi_hd_undo_init( LSA_VOID );

LSA_VOID psi_hd_open_device( PSI_RQB_PTR_TYPE rqb_ptr );
LSA_VOID psi_hd_close_device( PSI_RQB_PTR_TYPE rqb_ptr );

PSI_EDD_HDDB psi_hd_get_hDDB( LSA_UINT16 const hd_nr );
PSI_EDD_HIDB psi_hd_get_hIDB( LSA_UINT16 const hd_nr, LSA_UINT16 const if_nr );

PSI_HD_STORE_PTR_TYPE psi_hd_get_hd_store( LSA_UINT16 const hd_nr );

PSI_RQB_PTR_TYPE psi_hd_get_hd_open_device_rqb_ptr( LSA_VOID );

LSA_VOID psi_hd_set_arp_on_state( LSA_UINT16 pnio_if_nr, LSA_BOOL state );
LSA_BOOL psi_hd_is_arp_on_state( LSA_UINT16 pnio_if_nr );

LSA_VOID psi_hd_edd_open_done( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE p_data );
LSA_VOID psi_hd_edd_close_done( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE p_data );

LSA_VOID psi_hd_get_if_mac(
	LSA_UINT16 hd_nr,
	LSA_UINT16 if_nr,
	LSA_UINT32 * if_ptr,
	PSI_MAC_TYPE * mac_ptr );

LSA_UINT32 psi_hd_get_pi_size( LSA_UINT16 const hd_nr );

PSI_PORT_MEDIA_TYPE psi_hd_get_psi_media_type(
    LSA_UINT16 const hd_nr,
	LSA_UINT32 const if_nr,
	LSA_UINT32 const HwPortId );

LSA_INT psi_hd_get_nrt_pool_handle( LSA_UINT16 const hd_nr );

LSA_BOOL psi_hd_is_edd_nrt_copy_if_on( LSA_UINT16 const hd_nr );

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_HD_H */
