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
/*  F i l e               &F: psi_ld.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI LD instance.                                          */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      4
#define LTRC_ACT_MODUL_ID  4

#include "psi_int.h"
#include "psi_pnstack.h"
#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#define PSI_LD_IF_MAX_REQ ((PSI_MAX_PNIO_IF_NR+1)*PSI_SYS_PATH_MAX) 
/*---------------------------------------------------------------------------*/
/*  Types                                                                    */
/*---------------------------------------------------------------------------*/

typedef enum psi_ld_state_enum {
	PSI_LD_INIT_STATE = 0,  /* Reserved for invalid */
	PSI_LD_INIT_DONE,       /* INIT sequence done */
	PSI_LD_OPENING,         /* LD Open sequence in progress (startup) */
	PSI_LD_OPEN_DONE,       /* LD Open finished (ready for requests) */
	PSI_LD_CLOSING,         /* LD Close sequence in progress (shutdown) */
} PSI_LD_STATE_TYPE;

// Open/Close function adaption for SYS channel open/Close
typedef void (*PSI_LD_CHN_OPEN_FCT)( LSA_SYS_PATH_TYPE );
typedef void (*PSI_LD_CHN_CLOSE_FCT)( LSA_SYS_PATH_TYPE, LSA_HANDLE_TYPE );

typedef struct psi_channel_fct_tag {
	PSI_LD_CHN_OPEN_FCT   fct_open;
	PSI_LD_CHN_CLOSE_FCT  fct_close;
} PSI_LD_CHANNEL_FCT_TYPE, *PSI_LD_CHANNEL_FCT_PTR_TYPE;

typedef struct psi_ld_hd_if_tag { // storage for one HD IF
	LSA_HANDLE_TYPE lower_handle[PSI_SYS_PATH_MAX];  // lower handle
} PSI_LD_HD_IF_TYPE, *PSI_LD_HD_IF_PTR_TYPE;

typedef struct psi_ld_hd_tag { // PNIO IF storage (details, channels) for one HD
	LSA_UINT16        hif_lower_handle;              // HIF HD lower handle
	PSI_HIF_SYS_TYPE  sys_lower_handle;              // PSI hSysDev for HIF lower
	LSA_UINT16        hif_upper_handle;              // HIF HD upper handle
	PSI_HIF_SYS_TYPE  sys_upper_handle;              // PSI hSysDev for HIF upper
	PSI_LD_HD_IF_TYPE hd_if[PSI_CFG_MAX_HD_IF_CNT];  // all HD IF instances
} PSI_LD_HD_TYPE, *PSI_LD_HD_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct psi_ld_hd_req_entry_tag { // PSI LD request entry for one HD
	LSA_UINT16      hd_nr;                   // HD number
	PSI_HD_ID_TYPE  hd_location;             // For HIF HD Upper open
	LSA_BOOL        runs_on_level_basic;     // For HIF HD Lower open
} PSI_LD_HD_REQ_ENTRY_TYPE, *PSI_LD_HD_REQ_ENTRY_PTR_TYPE;

typedef struct psi_ld_hd_req_tag { // PSI LD request list for all HD requests
	LSA_UINT16                nr_of_hd;                    // count of HDs in list
	LSA_INT                   act_req_idx;                 // actual handled HD
	PSI_LD_HD_REQ_ENTRY_TYPE  hd_req[PSI_CFG_MAX_HD_CNT];  // HD entries
} PSI_LD_HD_REQ_STORE_TYPE, *PSI_LD_HD_REQ_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct psi_ld_if_req_entry_tag { // PSI LD IF request entry for one HD IF
	LSA_UINT16      hd_nr;           // HD number
	LSA_UINT16      if_nr;           // IF number
	LSA_UINT16      pnio_if_nr;      // PNIO IF number
	LSA_UINT16      sys_path;        // Sys-Path
} PSI_LD_IF_REQ_ENTRY_TYPE, *PSI_LD_IF_REQ_ENTRY_PTR_TYPE;

typedef struct psi_ld_if_req_tag { // PSI LD request list for all HD requests
	LSA_UINT16                nr_of_req;                     // count of Req. in list
	LSA_INT                   act_req_idx;                   // actual handled Req
	PSI_LD_IF_REQ_ENTRY_TYPE  req[PSI_LD_IF_MAX_REQ];        // IF entries
} PSI_LD_IF_REQ_STORE_TYPE, *PSI_LD_IF_REQ_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct psi_ld_store_tag { // PSI LD instance store
	LSA_HANDLE_TYPE          global_lower_handle[PSI_SYS_PATH_MAX];  // lower handle for global sys channels
	PSI_LD_CHANNEL_FCT_TYPE  global_fct[PSI_SYS_PATH_MAX];           // fct list for open/close the global sys channels 
	PSI_LD_CHANNEL_FCT_TYPE  if_fct[PSI_SYS_PATH_MAX];               // fct list for open/close the IF sys channels 
	PSI_LD_HD_REQ_STORE_TYPE hd_req;                                 // HD request storage
	PSI_LD_IF_REQ_STORE_TYPE if_req;                                 // HD IF request storage
	PSI_LD_HD_TYPE           hd[PSI_CFG_MAX_HD_CNT];                 // all HD instances
} PSI_LD_STORE_TYPE, *PSI_LD_STORE_PTR_TYPE;

typedef struct psi_ld_instance_tag { // PSI LD instance
	PSI_LD_STATE_TYPE      ld_state;         // actual PSI LD state (see PSI_LD_STATE_TYPE)
	PSI_LD_STORE_PTR_TYPE  ld_store_ptr;     // allocated instance data
	PSI_RQB_PTR_TYPE       ld_upper_rqb;     // stored upper RQB from system request
#if (PSI_CFG_USE_SOCK == 1)
	PSI_SOCK_INPUT_TYPE    ld_sock_details;  // stored sock details from system request
#endif
} PSI_LD_INSTANCE_TYPE, *PSI_LD_INSTANCE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*  Global data                                                              */
/*---------------------------------------------------------------------------*/

static PSI_LD_INSTANCE_TYPE psi_ld_inst = { PSI_LD_INIT_STATE, LSA_NULL, LSA_NULL };

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_VOID psi_ld_init_channel_fct( PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	LSA_UINT16 idx;

	for ( idx = 0; idx < PSI_SYS_PATH_MAX; idx++ )
	{
		pFunc[idx].fct_open  = LSA_NULL;
		pFunc[idx].fct_close = LSA_NULL;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_ld_init_global_channel_fct( PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	PSI_ASSERT( pFunc != 0 );

	PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_global_channel_fct(): Setup global PSI LD system channel list" );

	// Initialize first
	psi_ld_init_channel_fct( pFunc );

	// Prepare the global SYS channels based on compile keys
#if( PSI_CFG_USE_CLRPC== 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_global_channel_fct(): set CLRPC entry(PSI_PATH_GLO_SYS_CLRPC_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_CLRPC_SOCK );

	pFunc[PSI_PATH_GLO_SYS_CLRPC_SOCK].fct_open  = psi_clrpc_open_channel;
	pFunc[PSI_PATH_GLO_SYS_CLRPC_SOCK].fct_close = psi_clrpc_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // CLRPC global

#if( PSI_CFG_USE_OHA == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_global_channel_fct(): set OHA entry(PSI_PATH_GLO_SYS_OHA_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_OHA_SOCK );

	pFunc[PSI_PATH_GLO_SYS_OHA_SOCK].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_GLO_SYS_OHA_SOCK].fct_close = psi_oha_close_channel;

#if( PSI_CFG_USE_SNMPX == 1 ) && ( PSI_CFG_USE_SNMPX_AGT == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"set OHA entry(PSI_PATH_GLO_SYS_OHA_SNMPX), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_OHA_SNMPX );

	pFunc[PSI_PATH_GLO_SYS_OHA_SNMPX].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_GLO_SYS_OHA_SNMPX].fct_close = psi_oha_close_channel;
	/*----------------------------------------------------------------------------*/
#endif
#endif // OHA global

#if( PSI_CFG_USE_SNMPX == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_global_channel_fct(): set SNMPX entry(PSI_PATH_GLO_SYS_SNMPX_SOCK), SYS-PATH(%u)", 
		PSI_PATH_GLO_SYS_SNMPX_SOCK );

	pFunc[PSI_PATH_GLO_SYS_SNMPX_SOCK].fct_open  = psi_snmpx_open_channel;
	pFunc[PSI_PATH_GLO_SYS_SNMPX_SOCK].fct_close = psi_snmpx_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // SNMPX global
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_ld_init_if_channel_fct( PSI_LD_CHANNEL_FCT_PTR_TYPE pFunc )
{
	PSI_ASSERT( pFunc != 0 );

	// Initialize first
	psi_ld_init_channel_fct( pFunc );

	// Prepare the IF specific SYS channels based on compile Keys
	/*----------------------------------------------------------------------------*/
#if( PSI_CFG_USE_ACP == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set ACP entry(PSI_PATH_IF_SYS_ACP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_ACP_EDD );

	pFunc[PSI_PATH_IF_SYS_ACP_EDD].fct_open   = psi_acp_open_channel;
	pFunc[PSI_PATH_IF_SYS_ACP_EDD].fct_close  = psi_acp_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // ACP

#if( PSI_CFG_USE_CM == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_EDD );

	pFunc[PSI_PATH_IF_SYS_CM_EDD].fct_open   = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_EDD].fct_close  = psi_cm_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_CLRPC_SOCK), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_CLRPC );

	pFunc[PSI_PATH_IF_SYS_CM_CLRPC].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_CLRPC].fct_close = psi_cm_close_channel;

#if( PSI_CFG_USE_MRP == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_MRP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_MRP );

	pFunc[PSI_PATH_IF_SYS_CM_MRP].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_MRP].fct_close = psi_cm_close_channel;
#endif

#if( PSI_CFG_USE_POF == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_POF_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_POF_EDD );

	pFunc[PSI_PATH_IF_SYS_CM_POF_EDD].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_POF_EDD].fct_close = psi_cm_close_channel;
#endif

#if( PSI_CFG_USE_GSY == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_GSY_CLOCK), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_GSY_CLOCK );

	pFunc[PSI_PATH_IF_SYS_CM_GSY_CLOCK].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_GSY_CLOCK].fct_close = psi_cm_close_channel;
#endif

#if( PSI_CFG_USE_NARE == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_NARE), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_NARE );

	pFunc[PSI_PATH_IF_SYS_CM_NARE].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_NARE].fct_close = psi_cm_close_channel;
#endif

#if( PSI_CFG_USE_OHA == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set CM entry(PSI_PATH_IF_SYS_CM_OHA), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_CM_OHA );

	pFunc[PSI_PATH_IF_SYS_CM_OHA].fct_open  = psi_cm_open_channel;
	pFunc[PSI_PATH_IF_SYS_CM_OHA].fct_close = psi_cm_close_channel;
#endif
	/*----------------------------------------------------------------------------*/
#endif  // CM

#if( PSI_CFG_USE_DCP == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set DCP entry(PSI_PATH_IF_SYS_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_DCP_EDD].fct_open  = psi_dcp_open_channel;
	pFunc[PSI_PATH_IF_SYS_DCP_EDD].fct_close = psi_dcp_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // DCP

#if( PSI_CFG_USE_GSY == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set GSY entry(PSI_PATH_IF_SYS_GSY_EDD_SYNC), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_GSY_EDD_SYNC );

	pFunc[PSI_PATH_IF_SYS_GSY_EDD_SYNC].fct_open   = psi_gsy_open_channel;
	pFunc[PSI_PATH_IF_SYS_GSY_EDD_SYNC].fct_close  = psi_gsy_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set GSY entry(PSI_PATH_IF_SYS_GSY_EDD_ANNO), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_GSY_EDD_ANNO );

	pFunc[PSI_PATH_IF_SYS_GSY_EDD_ANNO].fct_open   = psi_gsy_open_channel;
	pFunc[PSI_PATH_IF_SYS_GSY_EDD_ANNO].fct_close  = psi_gsy_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // GSY

#if( PSI_CFG_USE_MRP == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set MRP entry(PSI_PATH_IF_SYS_MRP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_MRP_EDD );

	pFunc[PSI_PATH_IF_SYS_MRP_EDD].fct_open   = psi_mrp_open_channel;
	pFunc[PSI_PATH_IF_SYS_MRP_EDD].fct_close  = psi_mrp_close_channel;
	/*----------------------------------------------------------------------------*/
#endif // MRP

#if( PSI_CFG_USE_NARE == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set NARE entry(PSI_PATH_IF_SYS_NARE_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_NARE_EDD );

	pFunc[PSI_PATH_IF_SYS_NARE_EDD].fct_open   = psi_nare_open_channel;
	pFunc[PSI_PATH_IF_SYS_NARE_EDD].fct_close  = psi_nare_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set NARE entry(PSI_PATH_IF_SYS_NARE_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_NARE_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_NARE_DCP_EDD].fct_open   = psi_nare_open_channel;
	pFunc[PSI_PATH_IF_SYS_NARE_DCP_EDD].fct_close  = psi_nare_close_channel;
	/*----------------------------------------------------------------------------*/
#endif // NARE

#if( PSI_CFG_USE_OHA == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_EDD].fct_close = psi_oha_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_LLDP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_LLDP_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_LLDP_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_LLDP_EDD].fct_close = psi_oha_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_DCP_EDD), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_DCP_EDD );

	pFunc[PSI_PATH_IF_SYS_OHA_DCP_EDD].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_DCP_EDD].fct_close = psi_oha_close_channel;

#if( PSI_CFG_USE_NARE == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_NARE), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_NARE );

	pFunc[PSI_PATH_IF_SYS_OHA_NARE].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_NARE].fct_close = psi_oha_close_channel;
#endif

#if( PSI_CFG_USE_MRP == 1 )
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set OHA entry(PSI_PATH_IF_SYS_OHA_MRP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_OHA_MRP );

	pFunc[PSI_PATH_IF_SYS_OHA_MRP].fct_open  = psi_oha_open_channel;
	pFunc[PSI_PATH_IF_SYS_OHA_MRP].fct_close = psi_oha_close_channel;
#endif
	/*----------------------------------------------------------------------------*/
#endif  // OHA

#if( PSI_CFG_USE_TCIP == 1 )
	/*----------------------------------------------------------------------------*/
	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_ARP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_ARP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ARP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ARP].fct_close  = psi_tcip_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_ICMP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_ICMP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ICMP].fct_open  = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_ICMP].fct_close = psi_tcip_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_UDP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_UDP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_UDP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_UDP].fct_close  = psi_tcip_close_channel;

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, 
		"psi_ld_init_if_channel_fct(): set TCIP entry(PSI_PATH_IF_SYS_TCIP_EDD_TCP), SYS-PATH(%u)",
		PSI_PATH_IF_SYS_TCIP_EDD_TCP );

	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_TCP].fct_open   = psi_tcip_open_channel;
	pFunc[PSI_PATH_IF_SYS_TCIP_EDD_TCP].fct_close  = psi_tcip_close_channel;
	/*----------------------------------------------------------------------------*/
#endif  // TCIP
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static LSA_VOID psi_ld_set_sys_handle( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	// Register sys channel handle for SYS PATH
	LSA_UINT16 hd_nr       = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr       = PSI_SYSPATH_GET_IF( sys_path );
	LSA_SYS_PATH_TYPE path = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_ASSERT( psi_ld_inst.ld_state == PSI_LD_OPENING );
	PSI_ASSERT( path < PSI_SYS_PATH_MAX );

	if (( hd_nr == 0 ) && ( if_nr == 0 )) // Global channel ?
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = handle;
	}
	else
	{
		PSI_LD_HD_PTR_TYPE hd_ptr;

		PSI_ASSERT(( hd_nr > 0 ) && ( hd_nr <= PSI_CFG_MAX_HD_CNT ));
		PSI_ASSERT(( if_nr > 0 ) && ( if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
		hd_ptr->hd_if[if_nr-1].lower_handle[path] = handle;
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_reset_sys_handle( LSA_SYS_PATH_TYPE sys_path )
{
	// Register sys channel handle for SYS PATH
	LSA_UINT16 hd_nr       = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr       = PSI_SYSPATH_GET_IF( sys_path );
	LSA_SYS_PATH_TYPE path = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_ASSERT( psi_ld_inst.ld_state == PSI_LD_CLOSING );
	PSI_ASSERT( path < PSI_SYS_PATH_MAX );

	if (( hd_nr == 0 ) && ( if_nr == 0 )) // Global channel ?
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = PSI_INVALID_HANDLE;
	}
	else
	{
		PSI_LD_HD_PTR_TYPE hd_ptr;

		PSI_ASSERT(( hd_nr > 0 ) && ( hd_nr <= PSI_CFG_MAX_HD_CNT ));
		PSI_ASSERT(( if_nr > 0 ) && ( if_nr <= PSI_CFG_MAX_HD_IF_CNT ));

		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
		hd_ptr->hd_if[if_nr-1].lower_handle[path] = PSI_INVALID_HANDLE;
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_prepare_req_list( PSI_UPPER_LD_OPEN_PTR_TYPE p_open )
{
	LSA_UINT16 hd_idx;
	LSA_UINT16 if_idx;
	LSA_UINT16 path;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

	LSA_UINT16 add_idx = 0;

	PSI_ASSERT( p_open != LSA_NULL );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	// First clean the list
	PSI_MEMSET( p_hd_req, 0, sizeof ( PSI_LD_HD_REQ_STORE_TYPE ));
	PSI_MEMSET( p_if_req, 0, sizeof ( PSI_LD_IF_REQ_STORE_TYPE ));

	// First add global sys channel to IF req list
	for ( path = PSI_SYS_PATH_INVALID; path < PSI_SYS_PATH_MAX; path++ )
	{
		if ( psi_ld_inst.ld_store_ptr->global_fct[path].fct_open != LSA_NULL )
		{
			p_if_req->req[add_idx].hd_nr      = 0;
			p_if_req->req[add_idx].if_nr      = 0;
			p_if_req->req[add_idx].pnio_if_nr = PSI_GLOBAL_PNIO_IF_NR;
			p_if_req->req[add_idx].sys_path   = path;
			add_idx++;
		}
	}

	// Initialize hd req lists
	p_hd_req->nr_of_hd    = (LSA_UINT16)p_open->hd_count;
	p_hd_req->act_req_idx = 0;

	for ( hd_idx = 0; hd_idx < p_open->hd_count; hd_idx++ )
	{
		PSI_HD_INPUT_PTR_TYPE   p_hd_in  = &p_open->hd_args[hd_idx];

		/* add HDs to HD request list (for open/close HDs) */
		p_hd_req->hd_req[hd_idx].hd_nr               = p_hd_in->hd_id;
		p_hd_req->hd_req[hd_idx].hd_location         = p_hd_in->hd_location;
		p_hd_req->hd_req[hd_idx].runs_on_level_basic = p_hd_in->runs_on_level_basic;

		for ( if_idx = 0; if_idx < p_hd_in->nr_of_if; if_idx++ )
		{
			PSI_HD_IF_INPUT_PTR_TYPE p_if = &p_hd_in->pnio_if[if_idx];
			LSA_UINT16               if_nr = (LSA_UINT16)p_if->if_id;

			/* add HD IF sys channels to request list */
			for ( path = PSI_SYS_PATH_INVALID; path < PSI_SYS_PATH_MAX; path++ )
			{
				if ( psi_ld_inst.ld_store_ptr->if_fct[path].fct_open != LSA_NULL )
				{
					p_if_req->req[add_idx].hd_nr      = p_hd_in->hd_id;
					p_if_req->req[add_idx].if_nr      = if_nr;
					p_if_req->req[add_idx].pnio_if_nr = p_if->edd_if_id;
					p_if_req->req[add_idx].sys_path   = path;
					add_idx++;
				}
			}

			// Add the detail store for this PNIO IF
			// Note: if a duplicate PNIO IF ID is configured, add fails with FATAL
			psi_add_detail_store( p_if->edd_if_id, p_hd_in->hd_id, if_nr );
		}
	}

	// complete LD IF list
	p_if_req->nr_of_req   = add_idx;
	p_if_req->act_req_idx = 0;
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_clean_req_list( LSA_VOID )
{
	LSA_UINT16                   idx;
	LSA_UINT16                   max_if;
	LSA_UINT16                   last_pnio_if_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	// Unregister all PNIO-IF in Detailstore
	max_if = p_if_req->nr_of_req;

	last_pnio_if_nr = 0xFF;

	for ( idx = 0; idx < max_if; idx++ )
	{
		LSA_UINT16 pnio_if_nr = p_if_req->req[idx].pnio_if_nr;

		if ( pnio_if_nr != last_pnio_if_nr ) // already removed ?
		{
			#if ( PSI_CFG_USE_OHA == 1 )
			if(pnio_if_nr != PSI_GLOBAL_PNIO_IF_NR)
			{
				psi_oha_undo_init_db_if(pnio_if_nr); /* Undo init all pnio ifs from oha database / opposit to psi_oha_init_db_if() */
			}
			#endif

			// remove entry for PNIO IF
			psi_remove_detail_store( pnio_if_nr );

			last_pnio_if_nr = pnio_if_nr;
		}
	}

	// clean the lists
	PSI_MEMSET( p_hd_req, 0, sizeof ( PSI_LD_HD_REQ_STORE_TYPE ));
	PSI_MEMSET( p_if_req, 0, sizeof ( PSI_LD_IF_REQ_STORE_TYPE ));
}

/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_OHA == 1 )
static LSA_VOID psi_ld_get_lldp_chassis_id( LSA_UINT8* p_data )
{
	LSA_UINT16 retVal;
	LSA_UINT8  im_serial_nr[PSI_MAX_SIZE_IM_SERIAL_NR];

	PSI_RQB_PTR_TYPE p_rqb = psi_ld_inst.ld_upper_rqb;
	PSI_ASSERT( p_rqb != LSA_NULL );

	// prepare the LLDP chassis ID for OHA from LD I&M data and register it
	// Note: IM Serial Number not part of LD params, it is requested by function
	PSI_MEMSET( im_serial_nr, 0, sizeof(im_serial_nr));

	PSI_GET_IM_SERIAL_NUMBER( &retVal, im_serial_nr, PSI_MAX_SIZE_IM_SERIAL_NR );
	PSI_ASSERT( retVal == PSI_OK );

	PSI_SPRINTF( p_data, "%-25.25s %-20.20s %-16.16s %5u %c%3u%3u%3u",
		p_rqb->args.ld_open.im_args.device_type,
		p_rqb->args.ld_open.im_args.order_id,
		im_serial_nr,
		p_rqb->args.ld_open.im_args.hw_revision,
		p_rqb->args.ld_open.im_args.sw_version.revision_prefix,
		p_rqb->args.ld_open.im_args.sw_version.functional_enhancement,
		p_rqb->args.ld_open.im_args.sw_version.bug_fix,
		p_rqb->args.ld_open.im_args.sw_version.internal_change );
}
#endif // ( PSI_CFG_USE_OHA == 1 ) RQ-ID: AP01382158

/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_SOCK == 1 )
PSI_SOCK_INPUT_PTR_TYPE psi_ld_get_sock_details( LSA_VOID )
{
	return &psi_ld_inst.ld_sock_details;
}
#endif // ( PSI_CFG_USE_SOCK == 1 )

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_ld_close_next_hd( PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req )
{
	PSI_ASSERT( p_hd_req != LSA_NULL );

	// Check if all HIF HD are closed
	p_hd_req->act_req_idx -= 1;

	if ( p_hd_req->act_req_idx >= 0 ) // Next HD ?
	{
		// Close next PSI HD (shutdown) FW system
		psi_ld_close_hd( p_hd_req->act_req_idx );
	}
	else
	{
		// All HDs closed, finish close Request
		PSI_UPPER_CALLBACK_FCT_PTR_TYPE cbf_fct;

		PSI_RQB_PTR_TYPE pRQB = psi_ld_inst.ld_upper_rqb;

        PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_next_hd(): LD Close finished, hd_count(%u)", p_hd_req->nr_of_hd );

        // Reset the request list and the detail storage
		psi_ld_clean_req_list();

		// Close fínished --> response to Upper
		psi_ld_inst.ld_upper_rqb = LSA_NULL;

		cbf_fct = pRQB->args.ld_close.psi_request_upper_done_ptr;

		PSI_RQB_SET_RESPONSE( pRQB, PSI_OK );
		psi_ld_inst.ld_state = PSI_LD_INIT_DONE;  // set state to not open

#if ( PSI_CFG_USE_HIF_HD == 1 )
		//  Finish Request using HIF LD
		psi_system_request_done( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf_fct, pRQB );
#else
		// Finish Request
		psi_system_request_done( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)cbf_fct, pRQB);
#endif
	}
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_init( LSA_VOID )
{
	LSA_UINT16   path;
	LSA_UINT16   hd_idx;
	LSA_UINT16   if_idx;

	/* Init the PSI LD instance vars */
	PSI_ASSERT( psi_ld_inst.ld_state == PSI_LD_INIT_STATE );

	psi_ld_inst.ld_store_ptr = (PSI_LD_STORE_PTR_TYPE) psi_alloc_local_mem( sizeof(PSI_LD_STORE_TYPE) );
	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );

	psi_ld_inst.ld_upper_rqb = LSA_NULL;

	/* setup the fct list for global and if specific sys channels */
	psi_ld_init_global_channel_fct( psi_ld_inst.ld_store_ptr->global_fct );
	psi_ld_init_if_channel_fct( psi_ld_inst.ld_store_ptr->if_fct );

	/* Init global IF vars */
	for ( path = 0; path < PSI_SYS_PATH_MAX; path++ )
	{
		psi_ld_inst.ld_store_ptr->global_lower_handle[path] = PSI_INVALID_HANDLE;
	}

	/* Init HD IF vars */
	for ( hd_idx = 0; hd_idx < PSI_CFG_MAX_HD_CNT; hd_idx++ )
	{
		PSI_LD_HD_PTR_TYPE hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_idx];

		hd_ptr->hif_lower_handle = PSI_INVALID_HANDLE;
		hd_ptr->hif_upper_handle = PSI_INVALID_HANDLE;
		hd_ptr->sys_lower_handle.shortcut_mode = LSA_FALSE;
		hd_ptr->sys_lower_handle.is_upper = LSA_FALSE;

		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			PSI_LD_HD_IF_PTR_TYPE if_ptr = &hd_ptr->hd_if[if_idx];

			for ( path = 0; path < PSI_SYS_PATH_MAX; path++ )
			{
				if_ptr->lower_handle[path] = PSI_INVALID_HANDLE;
			}
		}
	}

	/* Init req. list */
	PSI_MEMSET( &psi_ld_inst.ld_store_ptr->hd_req, 0, sizeof ( PSI_LD_HD_REQ_STORE_TYPE ));
	PSI_MEMSET( &psi_ld_inst.ld_store_ptr->if_req, 0, sizeof ( PSI_LD_IF_REQ_STORE_TYPE ));

	psi_ld_inst.ld_state = PSI_LD_INIT_DONE;
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_undo_init( LSA_VOID )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	LSA_UINT16  path;
	LSA_UINT16  hd_idx;
	LSA_UINT16  if_idx;

	/* Undo INIT all PSI LD instance vars */
	PSI_ASSERT( psi_ld_inst.ld_state == PSI_LD_INIT_DONE );

	for ( path = 0; path < PSI_SYS_PATH_MAX; path++ ) /* check for closed all globals channels */
	{
		LSA_HANDLE_TYPE lower_handle = psi_ld_inst.ld_store_ptr->global_lower_handle[path];
		PSI_ASSERT( lower_handle == PSI_INVALID_HANDLE );
	}

	/* cleanup HD IF vars */
	for ( hd_idx = 0; hd_idx < PSI_CFG_MAX_HD_CNT; hd_idx++ )
	{
		PSI_LD_HD_PTR_TYPE hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_idx];

		PSI_ASSERT( hd_ptr->hif_lower_handle == PSI_INVALID_HANDLE );
		PSI_ASSERT( hd_ptr->hif_upper_handle == PSI_INVALID_HANDLE );

		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			PSI_LD_HD_IF_PTR_TYPE if_ptr = &hd_ptr->hd_if[if_idx];

			for ( path = 0; path < PSI_SYS_PATH_MAX; path++ )
			{
				PSI_ASSERT( if_ptr->lower_handle[path] == PSI_INVALID_HANDLE );
			}
		}
	}
#endif

	/* init the fct list for global and if specific sys channels */
	psi_ld_init_channel_fct( psi_ld_inst.ld_store_ptr->global_fct );
	psi_ld_init_channel_fct( psi_ld_inst.ld_store_ptr->if_fct );

	psi_free_local_mem( psi_ld_inst.ld_store_ptr );

	psi_ld_inst.ld_store_ptr = LSA_NULL;
	psi_ld_inst.ld_upper_rqb = LSA_NULL;
	psi_ld_inst.ld_state     = PSI_LD_INIT_STATE;
}

/*---------------------------------------------------------------------------*/
/*  Request interface                                                        */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_open_device( PSI_RQB_PTR_TYPE rqb_ptr )
{
	LSA_INT first_idx;
	PSI_UPPER_LD_OPEN_PTR_TYPE p_open = &rqb_ptr->args.ld_open;

	if (( p_open->hd_count == 0 ) || ( p_open->hd_count > PSI_CFG_MAX_HD_CNT )) // wrong HD count
	{
		PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_ld_open_device(): invalid params, hd_count(%u), rqb_ptr(%#x)",
			p_open->hd_count,
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_device(): LD Open start, hd_count(%u)", p_open->hd_count );

	// Start opening a LD device
	psi_ld_inst.ld_upper_rqb = rqb_ptr;
	psi_ld_inst.ld_state     = PSI_LD_OPENING;

	// Store Sock details if available
#if ( PSI_CFG_USE_SOCK == 1)
	psi_ld_inst.ld_sock_details = rqb_ptr->args.ld_open.sock_args;
#endif

	// delete resource calculation values
	psi_res_calc_delete ();

	// Prepare the global IFs (before open all HDs)
	// Note: the HD sets specifics like (hSysDev,...) are not possible on LD side
	psi_add_detail_store( PSI_GLOBAL_PNIO_IF_NR, 0, 0 );

	// Prepare HD request list for HD open
	// Note: the request list is used for HIF HD and PSI HD open/close
	psi_ld_prepare_req_list( p_open );

	first_idx = (LSA_UINT16)psi_ld_inst.ld_store_ptr->hd_req.act_req_idx;
	PSI_ASSERT( first_idx == 0 );

#if ( PSI_CFG_USE_HIF_HD == 1 )
	// First start creating HIF HD lower connection before open the devices
	// Note: check for HIF HD lower open (shortcut mode)
	psi_ld_open_hif_hd_lower( first_idx );
#else
	// Open all HD devices without HIF HD (normal order)
	psi_ld_open_hd( first_idx );
#endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_device( PSI_RQB_PTR_TYPE rqb_ptr  )
{
	PSI_LD_IF_REQ_STORE_PTR_TYPE    p_if_req;
    PSI_LD_HD_REQ_STORE_PTR_TYPE    p_hd_req;

	PSI_ASSERT( rqb_ptr != 0 );

    p_if_req = &psi_ld_inst.ld_store_ptr->if_req;
    p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

    PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_device(): LD Close start, hd_count(%u)", p_hd_req->nr_of_hd );

	// Start closing a LD device
	psi_ld_inst.ld_upper_rqb = rqb_ptr;
	psi_ld_inst.ld_state     = PSI_LD_CLOSING;

	// start with closing all PSI LD system channels (reverse order)
	p_if_req->act_req_idx = p_if_req->nr_of_req - 1;

	psi_ld_close_channels( p_if_req->act_req_idx );
}

#if ( PSI_CFG_USE_HIF_HD == 1 )
/*---------------------------------------------------------------------------*/
/* HIF HD open/close request                                                 */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_open_hif_hd_lower( LSA_INT hd_req_idx )
{
	PSI_HD_INPUT_PTR_TYPE        p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	PSI_ASSERT( psi_ld_inst.ld_upper_rqb != LSA_NULL );

	p_hd = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_req_idx];

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( hd_req_idx == p_hd_req->act_req_idx );

	if ( !p_hd->runs_on_level_basic )  // HD runs on LD level ?
	{
		HIF_RQB_PTR_TYPE   pRQB;
		PSI_LD_HD_PTR_TYPE hd_ptr;

		LSA_UINT16 hd_nr = p_hd_req->hd_req[hd_req_idx].hd_nr;

		hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

		// Prepare the SysPtr
		// Note: in shortcut mode all location fields are don't care
		PSI_MEMSET( &hd_ptr->sys_lower_handle, 0, sizeof( PSI_SYS_PTR_TYPE ));

		hd_ptr->sys_lower_handle.hd_nr                 = hd_nr;
		hd_ptr->sys_lower_handle.hd_sys_id.edd_type    = p_hd->edd_type;
		hd_ptr->sys_lower_handle.hd_sys_id.rev_nr      = p_hd->rev_nr;
		hd_ptr->sys_lower_handle.hd_sys_id.asic_type   = p_hd->asic_type;
		hd_ptr->sys_lower_handle.hd_sys_id.hd_location = p_hd->hd_location;
		hd_ptr->sys_lower_handle.shortcut_mode         = LSA_TRUE;
		hd_ptr->sys_lower_handle.is_upper              = LSA_FALSE;

		// Open HIF HD lower in shortcut mode
		HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
		PSI_ASSERT(pRQB);

		HIF_RQB_SET_HANDLE( pRQB,  PSI_INVALID_HANDLE   );
		HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_HD_LOWER_OPEN );
		HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

		pRQB->args.dev_lower_open.hSysDev = &hd_ptr->sys_lower_handle;
		pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; // out from HIF

		pRQB->args.dev_lower_open.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)psi_ld_open_hif_hd_lower_done;

		PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
			"psi_ld_open_hif_hd_lower(): pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
			pRQB,
			hd_req_idx,
			p_hd_req->nr_of_hd,
			p_hd_req->hd_req[hd_req_idx].hd_nr );

		/* post the message to HIF HD */
		psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
	}
	else
	{
		// HIF HD lower open is part of FW system, continue with Open HIF HD Upper
		// Note: In this case the driver connection is done during HIF_HD_U_GET_PARAMS
		psi_ld_open_hif_hd_upper( hd_req_idx );
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_lower_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                   rc;
	LSA_UINT16                   hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;
	HIF_RQB_PTR_TYPE             pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( pRQB != 0 );
	PSI_ASSERT( HIF_RQB_GET_RESPONSE( pRQB ) == HIF_OK );
	PSI_ASSERT( HIF_RQB_GET_OPCODE( pRQB )   == HIF_OPC_HD_LOWER_OPEN );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	hd_nr    = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr   = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_hif_hd_lower_done(): rsp(%u/%#x) pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

    hd_ptr->hif_lower_handle = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	PSI_ASSERT( rc == LSA_RET_OK );

	// Continue with HIF HD Upper Open
	psi_ld_open_hif_hd_upper( p_hd_req->act_req_idx );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_upper( LSA_INT hd_req_idx )
{
	LSA_UINT16                   hd_nr;
	LSA_UINT16                   if_idx;
	HIF_RQB_PTR_TYPE             pRQB;
	PSI_HD_INPUT_PTR_TYPE        p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	/* Create the HIF HD RQB and copy the HD section from LD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	PSI_ASSERT( psi_ld_inst.ld_upper_rqb != LSA_NULL );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( hd_req_idx == p_hd_req->act_req_idx );

	HIF_RQB_SET_HANDLE( pRQB,  PSI_INVALID_HANDLE   );
	HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_HD_UPPER_OPEN );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	hd_nr  = p_hd_req->hd_req[hd_req_idx].hd_nr;
	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	p_hd   = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_nr-1];

	// Prepare the SysPtr
	// Note: the location information for HD is important in none shortcut mode
	PSI_MEMSET( &hd_ptr->sys_upper_handle, 0, sizeof( PSI_SYS_PTR_TYPE ));

	hd_ptr->sys_upper_handle.hd_nr                 = hd_nr;
	hd_ptr->sys_upper_handle.hd_sys_id.edd_type    = p_hd->edd_type;
	hd_ptr->sys_upper_handle.hd_sys_id.rev_nr      = p_hd->rev_nr;
	hd_ptr->sys_upper_handle.hd_sys_id.asic_type   = p_hd->asic_type;
	hd_ptr->sys_upper_handle.hd_sys_id.hd_location = p_hd->hd_location;
	hd_ptr->sys_upper_handle.is_upper              = LSA_TRUE;

	if ( !p_hd->runs_on_level_basic )  // HD runs on LD level ?
	{
		// open HIF HD upper in shortcut mode
		// Note: all location params are don't care 
		hd_ptr->sys_upper_handle.shortcut_mode    = LSA_TRUE;
		hd_ptr->sys_upper_handle.hif_lower_handle = hd_ptr->hif_lower_handle;
	}
	else
	{
		// open HIF HD upper
		// Note: the location params are used for real open the HD
		hd_ptr->sys_upper_handle.shortcut_mode    = LSA_FALSE;
		hd_ptr->sys_upper_handle.hif_lower_handle = PSI_INVALID_HANDLE;
	}

	// Setup HIF args for open HD upper
	pRQB->args.dev_hd_open.hSysDev = &hd_ptr->sys_upper_handle;
	pRQB->args.dev_hd_open.hH      = PSI_INVALID_HANDLE; // out from HIF
	pRQB->args.dev_hd_open.Cbf     = psi_ld_open_hif_hd_upper_done;

	pRQB->args.dev_hd_open.hd_args.hd_id    = p_hd->hd_id;
	pRQB->args.dev_hd_open.hd_args.nr_of_if = p_hd->nr_of_if;

	for ( if_idx = 0; if_idx < pRQB->args.dev_hd_open.hd_args.nr_of_if; if_idx++ )
	{
		pRQB->args.dev_hd_open.hd_args.pnio_if[if_idx].if_id = p_hd->pnio_if[if_idx].if_id;
	}

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_hif_hd_upper(): pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		pRQB,
		hd_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hif_hd_upper_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                   rc;
	LSA_UINT16                   hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr     != LSA_NULL );
	PSI_ASSERT( pRQB                         != LSA_NULL );
	PSI_ASSERT( HIF_RQB_GET_OPCODE( pRQB )   == HIF_OPC_HD_UPPER_OPEN );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	hd_nr    = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr   = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_hif_hd_upper_done(): rsp(%u/%#x) pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if( HIF_RQB_GET_RESPONSE( pRQB ) != HIF_OK )
	{
		PSI_FATAL( 0 );
	}

	hd_ptr->hif_upper_handle = pRQB->args.dev_hd_open.hH;

	// Free the RQB
	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	PSI_ASSERT( rc == LSA_RET_OK );

	// Continue with PSI HD Open (setup the HD system)
	psi_ld_open_hd( p_hd_req->act_req_idx );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_upper( LSA_INT hd_req_idx )
{
	LSA_UINT16                   hd_nr;
	HIF_RQB_PTR_TYPE             pRQB;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	/* Create the HIF HD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	HIF_RQB_SET_HANDLE( pRQB,  PSI_INVALID_HANDLE   );
	HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_HD_UPPER_CLOSE );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( p_hd_req->act_req_idx == hd_req_idx );

	hd_nr  = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	pRQB->args.dev_close.hH  = hd_ptr->hif_upper_handle;
	pRQB->args.dev_close.Cbf = psi_ld_close_hif_hd_upper_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hif_hd_upper(): pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_upper_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                   rc;
	LSA_UINT16                   hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT( pRQB != LSA_NULL );
	PSI_ASSERT( HIF_RQB_GET_OPCODE(pRQB) == HIF_OPC_HD_UPPER_CLOSE );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hif_hd_upper_done(): rsp(%u/%#x) pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if( PSI_RQB_GET_RESPONSE( pRQB ) != PSI_OK )
	{
		PSI_FATAL( 0 );
	}

	hd_nr  = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	hd_ptr->hif_upper_handle = PSI_INVALID_HANDLE;

	// Free the RQB
	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	PSI_ASSERT( rc == LSA_RET_OK );

	if ( !p_hd_req->hd_req[p_hd_req->act_req_idx].runs_on_level_basic ) // HD lower closing ?
	{
		// Close lower HIF HD
		psi_ld_close_hif_hd_lower( p_hd_req->act_req_idx );
	}
	else
	{
		PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);

		// Close (shutdown) next HD or finish the request
		psi_ld_close_next_hd( p_hd_req );
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_lower( LSA_INT hd_req_idx )
{
	LSA_UINT16                   hd_nr;
	HIF_RQB_PTR_TYPE             pRQB;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	/* Create the HIF HD RQB */
	HIF_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
	PSI_ASSERT(pRQB);

	HIF_RQB_SET_HANDLE( pRQB,  PSI_INVALID_HANDLE   );
	HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_HD_LOWER_CLOSE );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( p_hd_req->act_req_idx == hd_req_idx );

	hd_nr  = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	pRQB->args.dev_close.hH  = hd_ptr->hif_lower_handle;
	pRQB->args.dev_close.Cbf = psi_ld_close_hif_hd_lower_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hif_hd_lower(): pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[hd_req_idx].hd_nr );

	/* post the message to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hif_hd_lower_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                   rc;
	LSA_UINT16                   hd_nr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_LD_HD_PTR_TYPE           hd_ptr;

	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT( pRQB != LSA_NULL );
	PSI_ASSERT( HIF_RQB_GET_OPCODE(pRQB) == HIF_OPC_HD_LOWER_CLOSE );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hif_hd_lower_done(): rsp(%u/%#x) pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if( PSI_RQB_GET_RESPONSE( pRQB ) != PSI_OK )
	{
		PSI_FATAL( 0 );
	}

	hd_nr  = p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr;
	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];
	hd_ptr->hif_lower_handle = PSI_INVALID_HANDLE;

	// Free the RQB
	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);

	// Close (shutdown) next HD or finish the request
	psi_ld_close_next_hd( p_hd_req );
}
#endif

/*---------------------------------------------------------------------------*/
/* PSI HD open/close request                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hd( LSA_INT hd_req_idx )
{
	LSA_UINT16                   hdNr;
	LSA_UINT16                   hd_edd_nr_of_all_ports       = 0;      // parameter necessary for TCP
	LSA_UINT16                   hd_edd_nr_of_all_interfaces  = 0;      // parameter necessary for TCP
	LSA_UINT16                   idx;
	PSI_UPPER_HD_OPEN_PTR_TYPE   p_open;
	PSI_HD_INPUT_PTR_TYPE        p_hd;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	/* Create the PSI HD RQB and copy the HD section for LD RQB */
	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_RQB_TYPE));
	PSI_ASSERT( pRQB != LSA_NULL );

	PSI_ASSERT( psi_ld_inst.ld_upper_rqb != LSA_NULL );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( hd_req_idx == p_hd_req->act_req_idx );

	PSI_RQB_SET_HANDLE( pRQB, PSI_INVALID_HANDLE );
	PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_HD_OPEN_DEVICE );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

	hdNr   = p_hd_req->hd_req[hd_req_idx].hd_nr;
	p_hd   = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[hd_req_idx];
	p_open = &pRQB->args.hd_open;

	PSI_MEMCPY( &p_open->hd_args, p_hd, sizeof(PSI_HD_INPUT_TYPE));

	// patch entries for nr_of_all_ports and nur_of_all_interfaces
	for( idx = 0; idx < psi_ld_inst.ld_upper_rqb->args.ld_open.hd_count; idx++ )
	{
	  PSI_ASSERT(psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports <= PSI_CFG_MAX_PORT_CNT );
	  PSI_ASSERT(psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports > 0);
	  
	  hd_edd_nr_of_all_ports        += psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[idx].nr_of_ports;
	  hd_edd_nr_of_all_interfaces++;
	}
	
	p_open->hd_args.nr_of_all_ports = hd_edd_nr_of_all_ports;
	p_open->hd_args.nr_of_all_if    = hd_edd_nr_of_all_interfaces; 
		  
	p_open->hH = psi_ld_get_hif_hd_upper_handle( hdNr );
	p_open->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)psi_ld_open_hd_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_hd(): HD Open start, pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		pRQB,
		hd_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[hd_req_idx].hd_nr );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	/* post the message to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
    #else
	/* post the message to PSI HD */
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_system, pRQB );
    #endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_hd_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	LSA_UINT16                   if_idx;
	LSA_UINT16                   port_idx;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;
	PSI_HD_INPUT_PTR_TYPE        p_hd_in;
	PSI_HD_OUTPUT_PTR_TYPE       p_hd_out;
	PSI_HD_IF_OHA_INPUT_PTR_TYPE p_oha_inp;
    #if ( PSI_CFG_USE_OHA == 1 )
	LSA_UINT8 chassis_name[PSI_MAX_STATION_NAME_LEN + 1];
    #endif

	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT( pRQB                     != LSA_NULL );
	PSI_ASSERT( PSI_RQB_GET_OPCODE(pRQB) == PSI_OPC_HD_OPEN_DEVICE );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_hd_done(): HD Open finished, rsp(%u/%#x), pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if( PSI_RQB_GET_RESPONSE( pRQB ) != PSI_OK )
	{
		PSI_FATAL( 0 );
	}

	// Add the calculated HD ouptuts to LD upper RQB
	p_hd_out = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_out[p_hd_req->act_req_idx];
	PSI_MEMCPY( p_hd_out, &pRQB->args.hd_open.hd_out, sizeof( PSI_HD_OUTPUT_TYPE));

	// Add the inout HD params to LD upper RQB
	p_hd_in = &psi_ld_inst.ld_upper_rqb->args.ld_open.hd_args[p_hd_req->act_req_idx];

	for( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
	{
		PSI_MEMCPY( p_hd_in->if_mac[if_idx], pRQB->args.hd_open.hd_args.if_mac[if_idx], sizeof( PSI_MAC_TYPE) );
	}
	
	for( port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++)
	{
		PSI_MEMCPY( p_hd_in->port_mac[port_idx], pRQB->args.hd_open.hd_args.port_mac[port_idx], sizeof( PSI_MAC_TYPE) );
	}

	// Free the RQB
	psi_free_local_mem( pRQB );

#if ( PSI_CFG_USE_OHA == 1 )
	// Prepare LLDP chassis ID (using for all PNIO IF)
	PSI_MEMSET( chassis_name, 0, sizeof(chassis_name));
	psi_ld_get_lldp_chassis_id( chassis_name );
#endif
	// Prepare the Interfaces for this HD (before open the Sys-Channels
	for ( if_idx = 0; if_idx < p_hd_in->nr_of_if; if_idx++ )
	{
    #if ( PSI_CFG_USE_OHA == 1 )
		LSA_UINT16 pnio_if_nr = p_hd_in->pnio_if[if_idx].edd_if_id;
    #endif
		if ( p_hd_in->runs_on_level_basic )  // HD not runs on LD level ?
		{
			// Calculate the details for this interface (inputs)
			// Note: the HD sets specifics like (hSysDev,...) are not possible on LD side
			psi_res_calc_set_if_details( (LSA_UINT16)p_hd_in->pnio_if[if_idx].if_id, 0, 0, p_hd_in );
		}

		// register OHA interface (before opening all IF channels
		p_oha_inp = &p_hd_in->pnio_if[if_idx].oha;
		#if ( PSI_CFG_USE_OHA == 1 )
		psi_oha_init_db_if( pnio_if_nr, p_oha_inp, chassis_name );
		#endif
	}

	p_hd_req->act_req_idx += 1;

	if ( p_hd_req->act_req_idx < p_hd_req->nr_of_hd ) // Next HD ?
	{
#if ( PSI_CFG_USE_HIF_HD == 1 )
		// Open next HD using HIF HD
		psi_ld_open_hif_hd_lower( p_hd_req->act_req_idx );
#else
		// Open next HD without HIF HID
		psi_ld_open_hd( p_hd_req->act_req_idx );
#endif
	}
	else
	{
		// All HD open
		PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

		// continue with open the PSI LD sys channels (global and IF)
		p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

		p_if_req->act_req_idx = 0;

		psi_ld_open_channels( p_if_req->act_req_idx );
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hd( LSA_INT hd_req_idx )
{
	LSA_UINT16                   hdNr;
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	/* Create the PSI HD RQB and copy the HD section for LD RQB */
	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_RQB_TYPE));
	PSI_ASSERT( pRQB != LSA_NULL );

	PSI_RQB_SET_HANDLE( pRQB, PSI_INVALID_HANDLE   );
	PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_HD_CLOSE_DEVICE );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;
	PSI_ASSERT( p_hd_req->act_req_idx == hd_req_idx );

	hdNr = p_hd_req->hd_req[hd_req_idx].hd_nr;
	pRQB->args.hd_close.hd_id                      = hdNr;
	pRQB->args.hd_close.hH                         = psi_ld_get_hif_hd_upper_handle( hdNr );
	pRQB->args.hd_close.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)psi_ld_close_hd_done;

	PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hd(): HD Close start, pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[hd_req_idx].hd_nr );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	/* post the message to HIF HD */
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system, pRQB );
    #else
	/* post the message to PSI HD */
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_system, pRQB );
    #endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_hd_done( LSA_VOID_PTR_TYPE rb_ptr )
{
	PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)rb_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT( pRQB != LSA_NULL );
	PSI_ASSERT( PSI_RQB_GET_OPCODE(pRQB) == PSI_OPC_HD_CLOSE_DEVICE );

	p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_close_hd_done(): HD Close finished, rsp(%u/%#x), pRQB(%#x) req_idx(%u) hd_count(%u) hd_id(%u)",
		PSI_RQB_GET_RESPONSE( pRQB ),
		PSI_RQB_GET_RESPONSE( pRQB ),
		pRQB,
		p_hd_req->act_req_idx,
		p_hd_req->nr_of_hd,
		p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr );

	if( PSI_RQB_GET_RESPONSE( pRQB ) != PSI_OK )
	{
		PSI_FATAL( 0 );
	}

	// Free the RQB
	psi_free_local_mem( pRQB );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// Close HIF HD upper
	psi_ld_close_hif_hd_upper( p_hd_req->act_req_idx );
    #else
	PSI_LD_CLOSED_HD(p_hd_req->hd_req[p_hd_req->act_req_idx].hd_nr);

	// Close (shutdown) next HD or finish the request
	psi_ld_close_next_hd( p_hd_req );
    #endif
}

/*---------------------------------------------------------------------------*/
/*  Channel open/close interface                                             */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_open_channels( LSA_INT if_req_idx )
{
	LSA_UINT16                   hd_nr;
	LSA_UINT16                   if_nr;
	LSA_UINT16                   path;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;
	PSI_LD_CHN_OPEN_FCT          fct_open;

	LSA_SYS_PATH_TYPE sys_path = 0;

	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	hd_nr = p_if_req->req[if_req_idx].hd_nr;
	if_nr = p_if_req->req[if_req_idx].if_nr;
	path  = p_if_req->req[if_req_idx].sys_path;

	PSI_ASSERT(path < PSI_SYS_PATH_MAX);

	// Prepare the sys_path value
	PSI_SYSPATH_SET_HD( sys_path, hd_nr );
	PSI_SYSPATH_SET_IF( sys_path, if_nr );
	PSI_SYSPATH_SET_PATH( sys_path, path );

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channels(): hd_nr(%u) if_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
        hd_nr, if_nr, path, path, sys_path, if_req_idx );

    // init "fct_open"
	if (( hd_nr == 0 ) && ( if_nr == 0 ))
	{
		fct_open = psi_ld_inst.ld_store_ptr->global_fct[path].fct_open; // global channels
	}
	else
	{
		fct_open = psi_ld_inst.ld_store_ptr->if_fct[path].fct_open; // if channels
	}

	// Open registered system channels from LD IF req list
	PSI_ASSERT( fct_open != LSA_NULL );

	// Start opening the system channel
	(fct_open)(sys_path);
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_channels_done( LSA_SYS_PATH_TYPE sys_path, LSA_HANDLE_TYPE handle )
{
	LSA_INT                      act_idx;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

	// Register sys channel handle for SYS PATH
	psi_ld_set_sys_handle( sys_path, handle );

	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	PSI_LD_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
		"psi_ld_open_channels_done(): hd_nr(%u) if_nr(%u) path(%#x/%u) sys_path(%#x) handle(%#x) req_idx(%u)",
		PSI_SYSPATH_GET_HD( sys_path ),
		PSI_SYSPATH_GET_IF( sys_path ),
		PSI_SYSPATH_GET_PATH( sys_path ),
		PSI_SYSPATH_GET_PATH( sys_path ),
		sys_path,
		handle,
		p_if_req->act_req_idx );

	act_idx = p_if_req->act_req_idx +1;
		
	if ( act_idx < p_if_req->nr_of_req ) // Next sys channel ?
	{
		/* open next sys channels from req list */
		p_if_req->act_req_idx = act_idx;

		psi_ld_open_channels( act_idx );
	}
	else
	{
		/* all channels done --> respond the system request */
		PSI_UPPER_CALLBACK_FCT_PTR_TYPE cbf_fct;

		PSI_RQB_PTR_TYPE pRQB = psi_ld_inst.ld_upper_rqb;

        // Open finished --> response to Upper
		psi_ld_inst.ld_upper_rqb = LSA_NULL;

		cbf_fct = pRQB->args.ld_open.psi_request_upper_done_ptr;

		PSI_RQB_SET_RESPONSE( pRQB, PSI_OK );
		psi_ld_inst.ld_state = PSI_LD_OPEN_DONE;

	    PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channels_done(): LD Open finished, hd_count(%u)", 
            pRQB->args.ld_open.hd_count );

#if ( PSI_CFG_USE_HIF == 1 )
		psi_system_request_done( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)cbf_fct, pRQB );
#else
		psi_system_request_done( PSI_MBX_ID_PSI, cbf_fct, pRQB);
#endif
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_close_channels( LSA_INT if_req_idx )
{
	LSA_UINT8                    handle;
	LSA_UINT16                   hd_nr;
	LSA_UINT16                   if_nr;
	LSA_UINT16                   path;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;
	PSI_LD_CHN_CLOSE_FCT         fct_close;

	LSA_SYS_PATH_TYPE sys_path = 0;

	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	hd_nr  = p_if_req->req[if_req_idx].hd_nr;
	if_nr  = p_if_req->req[if_req_idx].if_nr;
	path   = p_if_req->req[if_req_idx].sys_path;

	if (( hd_nr == 0 ) && ( if_nr == 0 )) // global channels ?
	{
		fct_close = psi_ld_inst.ld_store_ptr->global_fct[path].fct_close;
		handle = psi_ld_inst.ld_store_ptr->global_lower_handle[path];;
	}
	else
	{
		fct_close = psi_ld_inst.ld_store_ptr->if_fct[path].fct_close;
		handle = psi_ld_inst.ld_store_ptr->hd[hd_nr-1].hd_if[if_nr-1].lower_handle[path];
	}

	// Close registered system channels from LD IF req list
	PSI_ASSERT( fct_close != LSA_NULL );

	// Prepare the sys_path value
	PSI_SYSPATH_SET_HD( sys_path, hd_nr );
	PSI_SYSPATH_SET_IF( sys_path, if_nr );
	PSI_SYSPATH_SET_PATH( sys_path, path );

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channels(): hd_nr(%u) if_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
        hd_nr, if_nr, path, path, sys_path, if_req_idx );

	// Start closing the system channel
	(fct_close)(sys_path, handle );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_channels_done( LSA_SYS_PATH_TYPE sys_path )
{
	LSA_INT                      act_idx;
	PSI_LD_IF_REQ_STORE_PTR_TYPE p_if_req;

	// Unregister sys channel handle for SYS PATH
	psi_ld_reset_sys_handle( sys_path );

	p_if_req = &psi_ld_inst.ld_store_ptr->if_req;

	PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channels_done(): hd_nr(%u) if_nr(%u) path(%#x/%u) sys_path(%#x) req_idx(%u)",
		PSI_SYSPATH_GET_HD( sys_path ),
		PSI_SYSPATH_GET_IF( sys_path ),
		PSI_SYSPATH_GET_PATH( sys_path ),
		PSI_SYSPATH_GET_PATH( sys_path ),
		sys_path,
		p_if_req->act_req_idx );

	act_idx = p_if_req->act_req_idx - 1;

	if ( act_idx >= 0 ) // Next channel ?
	{
		/* close next sys channels from req list */
		p_if_req->act_req_idx = act_idx;

		psi_ld_close_channels( act_idx );
	}
	else
	{
		/* Start closing all HD */
		PSI_LD_HD_REQ_STORE_PTR_TYPE p_hd_req;

		p_hd_req = &psi_ld_inst.ld_store_ptr->hd_req;

		p_hd_req->act_req_idx = p_hd_req->nr_of_hd - 1;

		// close all HD devices
		psi_ld_close_hd( p_hd_req->act_req_idx );
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

LSA_UINT16 psi_ld_get_hif_hd_upper_handle( LSA_UINT16 hd_nr )
{
	LSA_UINT16         hif_handle;
	PSI_LD_HD_PTR_TYPE hd_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT(( hd_nr > 0 ) && ( hd_nr <= PSI_CFG_MAX_HD_CNT ));

	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	hif_handle = hd_ptr->hif_upper_handle;
    #if ( PSI_CFG_USE_HIF_HD == 1 )
	PSI_ASSERT( hif_handle != PSI_INVALID_HANDLE );
    #endif

	return ( hif_handle );
}

/*---------------------------------------------------------------------------*/

LSA_UINT16 psi_ld_get_hif_hd_lower_handle( LSA_UINT16 hd_nr )
{
	LSA_UINT16         hif_handle;
	PSI_LD_HD_PTR_TYPE hd_ptr;

	PSI_ASSERT( psi_ld_inst.ld_store_ptr != LSA_NULL );
	PSI_ASSERT(( hd_nr > 0 ) && ( hd_nr <= PSI_CFG_MAX_HD_CNT ));

	hd_ptr = &psi_ld_inst.ld_store_ptr->hd[hd_nr-1];

	if ( hd_ptr->sys_lower_handle.shortcut_mode ) // UPPER/LOWER shortcut mode ?
	{
		hif_handle = hd_ptr->hif_lower_handle;
		PSI_ASSERT( hif_handle != PSI_INVALID_HANDLE );
	}
	else
	{
		hif_handle = PSI_INVALID_HANDLE;
        #if ( PSI_CFG_USE_HIF_HD == 1 )
		// lower HIF HD is opened by FW system --> try to get the handle
		// Note: On a HD FW system hd_nr is don't care
		PSI_HIF_GET_HD_LOWER_HANDLE( &hif_handle, hd_nr );
        #else
		PSI_FATAL( 0 );
        #endif
	}

	return ( hif_handle );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
