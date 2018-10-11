#ifndef PSI_PNSTACK_H               /* ----- reinclude-protection ----- */
#define PSI_PNSTACK_H

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
/*  F i l e               &F: psi_pnstack.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile including all PN-Stack specific types, functions, .. */
/*                                                                           */
/*****************************************************************************/

#include "psi_hd.h"

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1))
#include "edd_inc.h"  // For EDDx setup types
#endif

#if (PSI_CFG_USE_HIF == 1)
#include "hif_cfg.h"
#include "hif_usr.h"
#include "hif_sys.h"
#endif

#if ( PSI_CFG_USE_ACP == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_acp_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_acp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_acp_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_acp_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_acp_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_CLRPC == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_clrpc_get_path_info(
	LSA_SYS_PTR_TYPE            *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE           *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE           sys_path );

LSA_RESULT psi_clrpc_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_clrpc_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_clrpc_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_clrpc_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_CM == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_cm_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_cm_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_cm_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_cm_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_cm_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_DCP == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_dcp_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_dcp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_dcp_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_dcp_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_dcp_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_EDDI == 1 )
/*----------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

typedef struct psi_eddi_session_tag  // session data for EDDI open/close sequence
{
	LSA_UINT16              hd_nr;          // HD number (HD ID)
	PSI_SYS_HANDLE          hd_sys_handle;  // hSysDev for EDDI device (app handle)
	PSI_EDD_HDDB            *hDDB_ptr;      // rev to EDDI device handle (hDDB)
    PSI_HD_OUTPUT_PTR_TYPE  p_hd_out;       // pointer to hd_out
	PSI_REQUEST_FCT         req_done_func;  // PSI EDDI CBF (exchanged in request done EDDI)

} PSI_EDDI_SESSION_TYPE, * PSI_EDDI_SESSION_PTR_TYPE;

typedef struct psi_eddi_setup_tag // things for  EDDI setup
{
	PSI_EDDI_SESSION_TYPE   session;        // Session data for open/close
	EDDI_DPB_TYPE           iniDPB;         // Device ParameterBlock
	EDDI_RQB_COMP_INI_TYPE  iniCOMP;        // Comp INI Parameter block
	EDDI_DSB_TYPE           iniDSB;         // Device Setup ParameterBlock

} PSI_EDDI_SETUP_TYPE, * PSI_EDDI_SETUP_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_eddi_get_path_info(
	LSA_SYS_PTR_TYPE            *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE           *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE           sys_path );

LSA_RESULT psi_eddi_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/* Adaption function for PSI HD system (EDDI device setup) */
LSA_VOID psi_eddi_device_open( 
	PSI_EDDI_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddi_basic_setup( 
	PSI_EDDI_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddi_device_setup( 
	PSI_EDDI_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddi_device_close(
	PSI_EDDI_SESSION_PTR_TYPE p_session ); 

LSA_VOID psi_eddi_prepare_setup_data( 
	PSI_EDDI_SETUP_PTR_TYPE   p_setup, 
	PSI_HD_INPUT_PTR_TYPE     p_inp, 
	PSI_HD_STORE_PTR_TYPE     p_hd );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_EDDP == 1 )
/*----------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

typedef struct psi_eddp_session_tag  // session data for EDDP open/close sequence
{
	LSA_UINT16              hd_nr;                              // HD number (HD ID)
	LSA_UINT16              act_if_idx;                         // actual IF index (used by setup
	LSA_UINT16              nr_of_if;                           // Nr of IF to setup
	PSI_SYS_HANDLE          hd_sys_handle;                      // hSysDev for EDDP device (app handle)
	PSI_EDD_HDDB            *hDDB_ptr;                          // rev to EDDP device handle (hDDB)
	PSI_EDD_HIDB            *hIDB_ptr[PSI_CFG_MAX_HD_IF_CNT];   // rev to EDDP interface handle (hIDB)
	PSI_REQUEST_FCT         req_done_func;                      // PSI EDDP CBF (exchanged in request done EDDP)

} PSI_EDDP_SESSION_TYPE, * PSI_EDDP_SESSION_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct psi_eddp_ini_tag // things for  EDDP setup
{
	PSI_EDDP_SESSION_TYPE   session;                            // Session data for open/close
	EDDP_DPB_TYPE           iniDPB;                             // Device ParameterBlock
	EDDP_IPB_TYPE           iniIPB[PSI_CFG_MAX_HD_IF_CNT];      // Interface Parameter block
	EDDP_DSB_TYPE           iniDSB;                             // Device Setup ParameterBlock
	EDDP_ISB_TYPE           iniISB[PSI_CFG_MAX_HD_IF_CNT];      // Interface Setup ParameterBlock

} PSI_EDDP_SETUP_TYPE, * PSI_EDDP_SETUP_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_eddp_get_path_info(
	LSA_SYS_PTR_TYPE     *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE    *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE    sys_path );

LSA_RESULT psi_eddp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/* PSI HD System function for EDDP device/IF setup */
LSA_VOID psi_eddp_device_open(
	PSI_HD_EDDP_PTR_TYPE     p_hw_eddp,
	PSI_EDDP_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddp_device_setup( 
	PSI_EDDP_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddp_interface_setup( 
	PSI_EDDP_SETUP_PTR_TYPE  p_setup ); 

LSA_VOID psi_eddp_interface_shutdown( 
	PSI_EDDP_SESSION_PTR_TYPE p_session );

LSA_VOID psi_eddp_device_shutdown( 
	PSI_EDDP_SESSION_PTR_TYPE p_session );

LSA_VOID psi_eddp_device_close(
	PSI_EDDP_SESSION_PTR_TYPE p_session );

LSA_VOID psi_eddp_prepare_setup_data( 
	PSI_EDDP_SETUP_PTR_TYPE   p_setup, 
	PSI_HD_INPUT_PTR_TYPE     p_inp, 
	PSI_HD_STORE_PTR_TYPE     p_hd );

LSA_VOID psi_eddp_ertec200p_init_phy_config_reg(
    EDDP_SYS_HANDLE  hSysDev );

LSA_VOID psi_eddp_phy_get_media_type(
    EDDP_HANDLE         hDDB,
    EDDP_SYS_HANDLE     hSysDev,
    LSA_UINT32          HwPortID,
    LSA_UINT8           *pMediaType );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_EDDS == 1 )
/*----------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
//  Types
/////////////////////////////////////////////////////////////////////////////

typedef struct psi_edds_session_tag  // session data for EDDS open/close sequence
{
	LSA_UINT16       hd_nr;                 // HD number (HD ID)
	PSI_SYS_HANDLE   hd_sys_handle;         // hSysDev for EDDS device (app handle)
	PSI_EDD_HDDB     *hDDB_ptr;             // rev to EDDS device handle (hDDB)
	PSI_REQUEST_FCT  req_done_func;         // PSI EDDS CBF (exchanged in request done EDDS)
    LSA_BOOL         b_gigabit_support;     // GigabitSupport?

} PSI_EDDS_SESSION_TYPE, *PSI_EDDS_SESSION_PTR_TYPE;

typedef struct psi_edds_ini_tag // things for  EDDS setup
{
	PSI_EDDS_SESSION_TYPE      session;      // Session data for open/close
	EDDS_DPB_TYPE              iniDPB;       // device parameter block
	EDDS_DSB_TYPE              iniDSB;       // device setup parameter block
	EDDS_RQB_CMP_NRT_INI_TYPE  iniNRT;       // NRT setup parameter block
	EDDS_RQB_CMP_SRT_INI_TYPE  iniSRT;       // SRT setup parameter block

} PSI_EDDS_SETUP_TYPE, *PSI_EDDS_SETUP_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_edds_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_edds_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_edds_device_open(
	PSI_EDDS_SETUP_PTR_TYPE p_setup );

LSA_VOID psi_edds_init_nrt(
	PSI_EDDS_SETUP_PTR_TYPE p_setup );

LSA_VOID psi_edds_init_srt(
	PSI_EDDS_SETUP_PTR_TYPE p_setup );

LSA_VOID psi_edds_device_setup(
	PSI_EDDS_SETUP_PTR_TYPE p_setup );

LSA_VOID psi_edds_device_shutdown(
	PSI_EDDS_SESSION_PTR_TYPE p_session );

LSA_VOID psi_edds_release_nrt(
	PSI_EDDS_SESSION_PTR_TYPE p_session );

LSA_VOID psi_edds_release_srt(
	PSI_EDDS_SESSION_PTR_TYPE p_session );

LSA_VOID psi_edds_device_close(
	PSI_EDDS_SESSION_PTR_TYPE p_session );

LSA_VOID psi_edds_prepare_setup_data( 
	PSI_EDDS_SETUP_PTR_TYPE   p_setup, 
	PSI_HD_INPUT_PTR_TYPE     p_inp, 
	PSI_HD_STORE_PTR_TYPE     p_hd );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_GSY == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_gsy_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_gsy_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_gsy_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_gsy_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_gsy_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_HIF == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_hif_ld_upper_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_hif_hd_upper_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_hif_hd_lower_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_hif_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_IOH == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_ioh_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_ioh_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/*----------------------------------------------------------------------------*/
#endif


#if ( PSI_CFG_USE_LLDP == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_lldp_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_lldp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_MRP == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_mrp_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_mrp_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_mrp_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_mrp_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_mrp_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_NARE == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_nare_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_nare_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_nare_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_nare_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_nare_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_BOOL psi_is_rcv_arp_rsp(LSA_VOID_PTR_TYPE ptr_rqb, LSA_UINT16 pnio_if_nr); /* Check if target NRT Rcv Frame is an Arp Response */
LSA_VOID psi_request_nare_ARPResponseFrameIn( LSA_VOID_PTR_TYPE ptr_rqb ); /* Arp Sniffing over PSI (only valid in Arp Context) */

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_OHA == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_oha_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_oha_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_oha_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_oha_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_oha_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

LSA_VOID psi_oha_init_db_if( 
	LSA_UINT16                   pnio_if_nr, 
	PSI_HD_IF_OHA_INPUT_PTR_TYPE oha_input_ptr,
	LSA_UINT8*                   chassis_name_ptr );

LSA_VOID psi_oha_undo_init_db_if( 
	LSA_UINT16                   pnio_if_nr );

/*----------------------------------------------------------------------------*/
#endif

#define PSI_POF_ADR_PAGE_A0     (LSA_UINT8)0xA0     // serial bus 2 wire address for data information

#if ( PSI_CFG_USE_POF == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_pof_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_pof_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_SNMPX == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_snmpx_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_snmpx_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_snmpx_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_snmpx_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_snmpx_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_SOCK == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_sock_get_path_info(
	LSA_SYS_PTR_TYPE             *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE            *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE            sys_path );

LSA_RESULT psi_sock_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_sock_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_sock_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_sock_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

#if ( PSI_CFG_USE_TCIP == 1 )
/*----------------------------------------------------------------------------*/
/* Adaption function for PSI base system (channels) */
LSA_RESULT psi_tcip_get_path_info(
	LSA_SYS_PTR_TYPE            *sys_ptr_ptr,
	LSA_VOID_PTR_TYPE           *detail_ptr_ptr,
	LSA_SYS_PATH_TYPE           sys_path );

LSA_RESULT psi_tcip_release_path_info(
	LSA_SYS_PTR_TYPE     sys_ptr,
	LSA_VOID_PTR_TYPE    detail_ptr );

LSA_VOID psi_tcip_open_channel( LSA_SYS_PATH_TYPE sys_path );
LSA_VOID psi_tcip_close_channel( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle );
LSA_VOID psi_tcip_channel_done( LSA_VOID_PTR_TYPE rqb_ptr );

/*----------------------------------------------------------------------------*/
#endif

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_PNSTACK_H */
