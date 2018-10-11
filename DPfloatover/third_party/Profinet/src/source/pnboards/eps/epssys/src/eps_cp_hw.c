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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_cp_hw.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS is a framework to integrate the LSA-modules                          */
/*  of the PN IO runtime stack on an embedded platform.                      */
/*                                                                           */
/*  PN Board specific functions and storage used for EDDx HW adaption        */
/*****************************************************************************/

#define EPS_MODULE_ID      20002
#define LTRC_ACT_MODUL_ID  20002

#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "eps_locks.h"
#include "eps_trc.h"

#include "psi_cfg.h"
#include "psi_usr.h"
#include "psi_sys.h"
#include "eps_pn_drv_if.h"   /* PN-Driver Interface */

EPS_FILE_SYSTEM_EXTENSION(EPS_MODULE_ID)

/*----------------------------------------------------------------------------*/

static EPS_BOARD_INFO_TYPE board_list[PSI_CFG_MAX_HD_CNT+1];

/*----------------------------------------------------------------------------*/

static struct {
	LSA_UINT16               enter_exit;
	EPS_BOARD_INFO_PTR_TYPE  p_list;
} eps_boards;

/*----------------------------------------------------------------------------*/
/** 
 * Initilizes the board storage structure.
 * 
 * @see eps_init        - calls this function.
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hw_init( LSA_VOID )
{
	eps_alloc_critical_section(&eps_boards.enter_exit, LSA_FALSE);

	eps_memset(board_list, 0, sizeof(board_list));

	eps_boards.p_list = board_list;

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_hw_init(): Board HW init done, max boards(%u)",PSI_CFG_MAX_HD_CNT );
}

/*----------------------------------------------------------------------------*/
/**
 * Deletes the board storage. Sets the pointer to invalid.
 * 
 * @see eps_undo_init - calls this function
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hw_undo_init( LSA_VOID )
{
	EPS_ASSERT( eps_boards.p_list ); // Initialized?

	eps_boards.p_list = 0;

	eps_memset(board_list, 0, sizeof(board_list));

	eps_free_critical_section(eps_boards.enter_exit);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_hw_undo_init(): Board HW cleanup done, max boards(%u)",PSI_CFG_MAX_HD_CNT );
}

/*----------------------------------------------------------------------------*/
/**
 * Getter function for the edd type of the selected board.
 * 
 * @param [in] board_nr     - 1...4 = HD.
 * 
 * @return LSA_COMP_ID_EDDI - PNBoard with IRTE IP
 * @return LSA_COMP_ID_EDDP - PNBoard with PNIP
 * @return LSA_COMP_ID_EDDS - Standard Ethernet Controller
 */
LSA_UINT16 eps_hw_get_edd_type( LSA_UINT16 board_nr )
{
	LSA_UINT16 edd_type = 0;

	EPS_ASSERT(eps_boards.p_list); // Initialized?
	EPS_ASSERT((board_nr >= 1) && (board_nr <= PSI_CFG_MAX_HD_CNT));

	eps_enter_critical_section(eps_boards.enter_exit);

	edd_type = eps_boards.p_list[board_nr].edd_type;

	eps_exit_critical_section(eps_boards.enter_exit);

	return edd_type;
}

/*----------------------------------------------------------------------------*/
/**
 * Initializes the Board Port param structure.
 * 
 * @param [in] p_boardInfo            - the structure that must be initialized.
 * @return LSA_VOID
 */
LSA_VOID eps_hw_init_board_port_param( EPS_BOARD_INFO_PTR_TYPE p_boardInfo )
{
	LSA_UINT32 if_idx;
	LSA_UINT32 port_idx;

	// get the detail setting for this PN Board
	EPS_ASSERT(eps_boards.p_list); // Initialized?

	eps_enter_critical_section(eps_boards.enter_exit);

	for ( if_idx = 0 ; if_idx < PSI_CFG_MAX_HD_IF_CNT ; if_idx++ )
	{
		for ( port_idx = 0; port_idx < (PSI_CFG_MAX_PORT_CNT + 1) ; port_idx++)
		{
			p_boardInfo->port_map[if_idx][port_idx].hw_port_id = EPS_HW_PORT_ID_UNDEFINED;
			p_boardInfo->port_map[if_idx][port_idx].hw_phy_nr  = EPS_HW_PHY_NR_UNDEFINED;
		}
	}

	eps_exit_critical_section(eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------*/
/**
 * Getter function for the board store for a given board.
 * 
 * @param [in] board_nr             - 0 = LD, 1...4 = HD.
 * @return EPS_BOARD_INFO_PTR_TYPE  - Pointer to board storage for the given board_nr.
 */
EPS_BOARD_INFO_PTR_TYPE eps_hw_get_board_store( LSA_UINT16 board_nr )
{
	EPS_BOARD_INFO_PTR_TYPE p_ret = 0;

	// get the detail setting for this PN Board
	EPS_ASSERT(eps_boards.p_list); // Initialized?
	EPS_ASSERT((board_nr >= 1) && (board_nr <= PSI_CFG_MAX_HD_CNT));

	eps_enter_critical_section(eps_boards.enter_exit);

	p_ret = &eps_boards.p_list[board_nr];

	eps_exit_critical_section(eps_boards.enter_exit);

	return p_ret;
}

/*----------------------------------------------------------------------------*/
/**
 * Overwrites the board storage with the given structure for the given board_nr.
 * 
 * @param [in] board_nr - 1...4 = HD
 * @param [in] p_store  - structure with the board storage info.
 * @return LSA_VOID
 */
LSA_VOID eps_hw_set_board_store( LSA_UINT16 board_nr, EPS_BOARD_INFO_PTR_TYPE p_store )
{
	// store the detail setting for this PN Board
	EPS_ASSERT(eps_boards.p_list); // Initialized?
	EPS_ASSERT(board_nr <= PSI_CFG_MAX_HD_CNT);

	eps_enter_critical_section(eps_boards.enter_exit);

	if (p_store == LSA_NULL) // Reset?
	{
		eps_memset(&eps_boards.p_list[board_nr], 0, sizeof(EPS_BOARD_INFO_TYPE));
	}
	else
	{
		eps_boards.p_list[board_nr] = *p_store;
	}

	eps_exit_critical_section(eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------*/
/**
 * Set hd_params for given board_nr
 * 
 * @param [in] board_nr       - 0 = LD, 1...4 = HD.
 * @param [in] p_param        - pointer to the structure with the hd_params. Casted to PSI_HD_PARAM_PTR_TYPE
 * @return LSA_VOID
 */
LSA_VOID eps_hw_set_hd_param( LSA_UINT16 board_nr, LSA_VOID_PTR_TYPE p_param )
{
	LSA_UINT16              if_idx;
	LSA_UINT16              port_idx;
	EPS_BOARD_INFO_PTR_TYPE p_store;
	PSI_HD_PARAM_PTR_TYPE   p_hd_param = (PSI_HD_PARAM_PTR_TYPE)p_param;

	EPS_ASSERT(eps_boards.p_list); // Initialized?
	EPS_ASSERT((board_nr >= 1) && (board_nr <= PSI_CFG_MAX_HD_CNT));

	eps_enter_critical_section(eps_boards.enter_exit);

	p_store = &eps_boards.p_list[board_nr];

	p_hd_param->hd_sys_handle = p_store->hd_sys_handle;

	for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	{
		p_hd_param->if_sys_handle[if_idx] = p_store->if_sys_handle[if_idx];
		eps_memcpy( p_hd_param->if_mac[if_idx], p_store->if_mac[if_idx].mac, PSI_MAC_ADDR_SIZE );
	}

	for (port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++)
	{
		eps_memcpy(p_hd_param->port_mac[port_idx], p_store->port_mac[port_idx+1].mac, PSI_MAC_ADDR_SIZE);
	}

	// set the common used HD settings
	p_hd_param->edd_type         = p_store->edd_type;
	p_hd_param->nrt_mem.base_ptr = p_store->nrt_mem.base_ptr;
	p_hd_param->nrt_mem.phy_addr = p_store->nrt_mem.phy_addr;
	p_hd_param->nrt_mem.size     = p_store->nrt_mem.size;

	p_hd_param->pi_mem.base_ptr  = p_store->pi_mem.base_ptr;
	p_hd_param->pi_mem.phy_addr  = p_store->pi_mem.phy_addr;
	p_hd_param->pi_mem.size      = p_store->pi_mem.size;

	p_hd_param->hif_mem.base_ptr = p_store->hif_mem.base_ptr;
	p_hd_param->hif_mem.phy_addr = p_store->hif_mem.phy_addr;
	p_hd_param->hif_mem.size     = p_store->hif_mem.size;

	switch (p_store->edd_type)
	{
	    case LSA_COMP_ID_EDDI:
		{
			EPS_ASSERT(p_store->eddi.is_valid);

			// set the HD specific params for EDDI HD
			p_hd_param->edd.eddi.device_type                          = p_store->eddi.device_type;
			p_hd_param->edd.eddi.irte.base_ptr                        = p_store->eddi.irte.base_ptr;
			p_hd_param->edd.eddi.irte.phy_addr                        = p_store->eddi.irte.phy_addr;
			p_hd_param->edd.eddi.irte.size                            = p_store->eddi.irte.size;

			p_hd_param->edd.eddi.kram.base_ptr                        = p_store->eddi.kram.base_ptr;
			p_hd_param->edd.eddi.kram.phy_addr                        = p_store->eddi.kram.phy_addr;
			p_hd_param->edd.eddi.kram.size                            = p_store->eddi.kram.size;

			p_hd_param->edd.eddi.iocc.base_ptr                        = p_store->eddi.iocc.base_ptr;
			p_hd_param->edd.eddi.iocc.phy_addr                        = p_store->eddi.iocc.phy_addr;
			p_hd_param->edd.eddi.iocc.size                            = p_store->eddi.iocc.size;
			
			p_hd_param->edd.eddi.paea.base_ptr                        = p_store->eddi.paea.base_ptr;
			p_hd_param->edd.eddi.paea.phy_addr                        = p_store->eddi.paea.phy_addr;
			p_hd_param->edd.eddi.paea.size                            = p_store->eddi.paea.size;			

			p_hd_param->edd.eddi.sdram.base_ptr                       = p_store->eddi.sdram.base_ptr;
			p_hd_param->edd.eddi.sdram.phy_addr                       = p_store->eddi.sdram.phy_addr;
			p_hd_param->edd.eddi.sdram.size                           = p_store->eddi.sdram.size;

			p_hd_param->edd.eddi.shared_mem.base_ptr                  = p_store->eddi.shared_mem.base_ptr;
			p_hd_param->edd.eddi.shared_mem.phy_addr                  = p_store->eddi.shared_mem.phy_addr;
			p_hd_param->edd.eddi.shared_mem.size                      = p_store->eddi.shared_mem.size;

			p_hd_param->edd.eddi.apb_periph_scrb.base_ptr             = p_store->eddi.apb_periph_scrb.base_ptr;
			p_hd_param->edd.eddi.apb_periph_scrb.phy_addr             = p_store->eddi.apb_periph_scrb.phy_addr;
			p_hd_param->edd.eddi.apb_periph_scrb.size                 = p_store->eddi.apb_periph_scrb.size;

			p_hd_param->edd.eddi.apb_periph_timer.base_ptr            = p_store->eddi.apb_periph_timer.base_ptr;
			p_hd_param->edd.eddi.apb_periph_timer.phy_addr            = p_store->eddi.apb_periph_timer.phy_addr;
			p_hd_param->edd.eddi.apb_periph_timer.size                = p_store->eddi.apb_periph_timer.size;

            // SII configuration parameters
			p_hd_param->edd.eddi.SII_IrqSelector			          = p_store->eddi.SII_IrqSelector;
			p_hd_param->edd.eddi.SII_IrqNumber			              = p_store->eddi.SII_IrqNumber;
			p_hd_param->edd.eddi.SII_ExtTimerInterval		          = p_store->eddi.SII_ExtTimerInterval;

            p_hd_param->edd.eddi.fragmentation_supported              = p_store->eddi.fragmentation_supported;

            p_hd_param->edd.eddi.nrt_dmacw_default_mem.base_ptr       = p_store->eddi.nrt_mem_dmacw_default.base_ptr;
            p_hd_param->edd.eddi.nrt_dmacw_default_mem.phy_addr       = p_store->eddi.nrt_mem_dmacw_default.phy_addr;
            p_hd_param->edd.eddi.nrt_dmacw_default_mem.size           = p_store->eddi.nrt_mem_dmacw_default.size;
            p_hd_param->edd.eddi.nrt_rx_default_mem.base_ptr          = p_store->eddi.nrt_mem_rx_default.base_ptr;
            p_hd_param->edd.eddi.nrt_rx_default_mem.phy_addr          = p_store->eddi.nrt_mem_rx_default.phy_addr;
            p_hd_param->edd.eddi.nrt_rx_default_mem.size              = p_store->eddi.nrt_mem_rx_default.size;
            p_hd_param->edd.eddi.nrt_tx_default_mem.base_ptr          = p_store->eddi.nrt_mem_tx_default.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_default_mem.phy_addr          = p_store->eddi.nrt_mem_tx_default.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_default_mem.size              = p_store->eddi.nrt_mem_tx_default.size;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.base_ptr         = p_store->eddi.nrt_mem_tx_low_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.phy_addr         = p_store->eddi.nrt_mem_tx_low_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.size             = p_store->eddi.nrt_mem_tx_low_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.base_ptr         = p_store->eddi.nrt_mem_tx_mid_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.phy_addr         = p_store->eddi.nrt_mem_tx_mid_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.size             = p_store->eddi.nrt_mem_tx_mid_frag.size;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.base_ptr        = p_store->eddi.nrt_mem_tx_high_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.phy_addr        = p_store->eddi.nrt_mem_tx_high_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.size            = p_store->eddi.nrt_mem_tx_high_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.base_ptr     = p_store->eddi.nrt_mem_tx_mgmtlow_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.phy_addr     = p_store->eddi.nrt_mem_tx_mgmtlow_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.size         = p_store->eddi.nrt_mem_tx_mgmtlow_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.base_ptr    = p_store->eddi.nrt_mem_tx_mgmthigh_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.phy_addr    = p_store->eddi.nrt_mem_tx_mgmthigh_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.size        = p_store->eddi.nrt_mem_tx_mgmthigh_frag.size;
            p_hd_param->edd.eddi.user_mem_fcts_dmacw_default          = p_store->eddi.user_mem_fcts_dmacw_default;
            p_hd_param->edd.eddi.user_mem_fcts_rx_default             = p_store->eddi.user_mem_fcts_rx_default;
            p_hd_param->edd.eddi.user_mem_fcts_tx_default             = p_store->eddi.user_mem_fcts_tx_default;
            p_hd_param->edd.eddi.user_mem_fcts_tx_low_frag            = p_store->eddi.user_mem_fcts_tx_low_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mid_frag            = p_store->eddi.user_mem_fcts_tx_mid_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_high_frag           = p_store->eddi.user_mem_fcts_tx_high_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mgmtlow_frag        = p_store->eddi.user_mem_fcts_tx_mgmtlow_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mgmthigh_frag       = p_store->eddi.user_mem_fcts_tx_mgmthigh_frag;

            p_hd_param->edd.eddi.has_ext_pll = p_store->eddi.has_ext_pll;

	        // Port mapping for all IF
 	        for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	        {
 		        for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		        {
			        p_hd_param->port_map[if_idx][port_idx].hw_phy_nr  = p_store->port_map[if_idx][port_idx].hw_phy_nr;
			        p_hd_param->port_map[if_idx][port_idx].hw_port_id = p_store->port_map[if_idx][port_idx].hw_port_id;

                    p_hd_param->port_media_type_select[if_idx][port_idx] = PSI_MEDIA_TYPE_SEL_COPPER;

                    if (   (p_store->board_type == EPS_PNDEV_BOARD_SOC1_PCIE)
                        || (p_store->board_type == EPS_PNDEV_BOARD_EB400_PCIE))
                        {
                        p_hd_param->port_media_type_select[if_idx][port_idx] = PSI_MEDIA_TYPE_SEL_AUTO; // automatic POF port detection
                    }
		        }
	        }
		}
		break;

	    case LSA_COMP_ID_EDDP:
		{
			EPS_ASSERT(p_store->eddp.is_valid);

			// set the specific HD params for EDDP HD
			p_hd_param->edd.eddp.icu_location = p_store->eddp.icu_location;
			p_hd_param->edd.eddp.hw_type      = p_store->eddp.hw_type;
			p_hd_param->edd.eddp.board_type   = p_store->eddp.board_type;

			p_hd_param->edd.eddp.appl_timer_mode                = p_store->eddp.appl_timer_mode;
			p_hd_param->edd.eddp.appl_timer_reduction_ratio     = p_store->eddp.appl_timer_reduction_ratio;
			p_hd_param->edd.eddp.is_transfer_end_correction_pos = p_store->eddp.is_transfer_end_correction_pos;
			p_hd_param->edd.eddp.transfer_end_correction_value  = p_store->eddp.transfer_end_correction_value;

			p_hd_param->edd.eddp.pnip.base_ptr = p_store->eddp.pnip.base_ptr;
			p_hd_param->edd.eddp.pnip.phy_addr = p_store->eddp.pnip.phy_addr;
			p_hd_param->edd.eddp.pnip.size     = p_store->eddp.pnip.size;

			p_hd_param->edd.eddp.sdram_NRT.base_ptr = p_store->eddp.sdram_NRT.base_ptr;
			p_hd_param->edd.eddp.sdram_NRT.phy_addr = p_store->eddp.sdram_NRT.phy_addr;
			p_hd_param->edd.eddp.sdram_NRT.size     = p_store->eddp.sdram_NRT.size;

			p_hd_param->edd.eddp.sdram_CRT.base_ptr = p_store->eddp.sdram_CRT.base_ptr;
			p_hd_param->edd.eddp.sdram_CRT.phy_addr = p_store->eddp.sdram_CRT.phy_addr;
			p_hd_param->edd.eddp.sdram_CRT.size     = p_store->eddp.sdram_CRT.size;

			p_hd_param->edd.eddp.perif_ram.base_ptr = p_store->eddp.perif_ram.base_ptr;
			p_hd_param->edd.eddp.perif_ram.phy_addr = p_store->eddp.perif_ram.phy_addr;
			p_hd_param->edd.eddp.perif_ram.size     = p_store->eddp.perif_ram.size;

			p_hd_param->edd.eddp.k32_tcm.base_ptr = p_store->eddp.k32_tcm.base_ptr;
			p_hd_param->edd.eddp.k32_tcm.phy_addr = p_store->eddp.k32_tcm.phy_addr;
			p_hd_param->edd.eddp.k32_tcm.size     = p_store->eddp.k32_tcm.size;

			p_hd_param->edd.eddp.apb_periph_scrb.base_ptr = p_store->eddp.apb_periph_scrb.base_ptr;
			p_hd_param->edd.eddp.apb_periph_scrb.phy_addr = p_store->eddp.apb_periph_scrb.phy_addr;
			p_hd_param->edd.eddp.apb_periph_scrb.size     = p_store->eddp.apb_periph_scrb.size;

			p_hd_param->edd.eddp.apb_periph_perif.base_ptr = p_store->eddp.apb_periph_perif.base_ptr;
			p_hd_param->edd.eddp.apb_periph_perif.phy_addr = p_store->eddp.apb_periph_perif.phy_addr;
			p_hd_param->edd.eddp.apb_periph_perif.size     = p_store->eddp.apb_periph_perif.size;
			p_hd_param->edd.eddp.g_shared_ram_base_ptr     = 0; // TODO set pointer to allocated mem or base from driver

	        // Port mapping for all IF
 	        for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	        {
 		        for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		        {
			        p_hd_param->port_map[if_idx][port_idx].hw_phy_nr	 = p_store->port_map[if_idx][port_idx].hw_phy_nr;
			        p_hd_param->port_map[if_idx][port_idx].hw_port_id	 = p_store->port_map[if_idx][port_idx].hw_port_id;
                    p_hd_param->port_media_type_select[if_idx][port_idx] = PSI_MEDIA_TYPE_SEL_AUTO; // automatic POF port detection
		        }
	        }
		}
		break;

	    case LSA_COMP_ID_EDDS:
		{
			EPS_ASSERT(p_store->edds.is_valid);

			// set the HD specific params for EDDS HD
			// note: the LL adaption settings are used
			p_hd_param->edd.edds.ll_handle         = p_store->edds.ll_handle;
			p_hd_param->edd.edds.ll_function_table = p_store->edds.ll_function_table;
			p_hd_param->edd.edds.ll_parameter      = p_store->edds.ll_params;

	        // Port mapping for all IF
 	        for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	        {
 		        for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		        {
			        p_hd_param->port_map[if_idx][port_idx].hw_phy_nr     = p_store->port_map[if_idx][port_idx].hw_phy_nr;
			        p_hd_param->port_map[if_idx][port_idx].hw_port_id    = p_store->port_map[if_idx][port_idx].hw_port_id;
                    p_hd_param->port_media_type_select[if_idx][port_idx] = PSI_MEDIA_TYPE_SEL_COPPER;
		        }
	        }
		}
		break;

	    default:
		{			
			EPS_FATAL("invalid EDDx type");
		}
		break;
	}

	eps_exit_critical_section(eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------
ld_upper_get_params	    FW download
= ld_runs_on_type    
1			            0
2			            0
3			            1

hd_upper_get_params	    FW download
= hd_runs_on_type	
0			            0
1			            1
*/
LSA_VOID eps_hw_connect( LSA_UINT16 hd_id, LSA_BOOL bFWDownload, PSI_HD_SYS_ID_PTR_TYPE hd_sys_id_ptr )
{
    EPS_BOARD_INFO_TYPE BoardInfo;

	EPS_PNDEV_OPEN_OPTION_TYPE Option;
	EPS_PNDEV_LOCATION_TYPE Location;
	EPS_PNDEV_HW_PTR_TYPE pHwInstOut;
	LSA_UINT16 retVal;

	EPS_PNDEV_HW_PTR_TYPE pBoard;
	LSA_UINT16 ret_val;
	
    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect() - hd_id = %d, bFWDownload = %d", hd_id, (LSA_UINT8) bFWDownload);
	
	ret_val = eps_pndev_if_get(hd_id, &pBoard);
    if(ret_val == EPS_PNDEV_RET_OK)
    {
	      EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect() - already connected");
		return;
    }
	Option.bDebug             = g_pEpsData->bDebug;
	Option.bShmDebug          = g_pEpsData->bShmDebug;
	Option.bShmHaltOnStartup  = g_pEpsData->bShmHalt;

	Option.bLoadFw  = bFWDownload;
	
	Location.uBusNr = hd_sys_id_ptr->hd_location.hd_location.bus_nr;
	Location.uDeviceNr = hd_sys_id_ptr->hd_location.hd_location.device_nr;
	Location.uFctNr = hd_sys_id_ptr->hd_location.hd_location.function_nr;

    eps_memcpy(Location.Mac, hd_sys_id_ptr->hd_location.hd_mac, sizeof(EPS_PNDEV_MAC_TYPE));

    if (hd_sys_id_ptr->hd_location.hd_selection == PSI_HD_SELECT_WITH_LOCATION)
	{
		Location.eLocationSelector = EPS_PNDEV_LOCATION_PCI;
        EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect() - PCI location = (%d,%d,%d)", Location.uBusNr, Location.uDeviceNr, Location.uFctNr);
	}
	else
	{
		Location.eLocationSelector = EPS_PNDEV_LOCATION_MAC;
        EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect() - MAC Address  = %d:%d:%d:%d:%d:%d", Location.Mac[0], Location.Mac[1], Location.Mac[2],Location.Mac[3],Location.Mac[4],Location.Mac[5] );
	}

	retVal = eps_pndev_if_open(&Location, &Option, &pHwInstOut, hd_id);
    if(EPS_PNDEV_RET_OK == retVal)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect() - eps_pndev_open returned with EPS_PNDEV_RET_OK");
    }
    else
    {
        EPS_FATAL("eps_hw_connect() - eps_pndev_open returned with EPS_PNDEV_RET_DEV_NOT_FOUND. Please check PCI/MAC address and configuration")
    }
	EPS_ASSERT (pHwInstOut != LSA_NULL);
	BoardInfo = pHwInstOut->EpsBoardInfo;	

	eps_hw_set_board_store(hd_id, &BoardInfo);  /* Register Settings at Board Store */
}

/*----------------------------------------------------------------------------*/

LSA_VOID eps_hw_get_hd_params( LSA_UINT16 hd_id, LSA_BOOL runs_on_level_basic, PSI_HD_SYS_ID_PTR_TYPE hd_sys_id_ptr, PSI_HD_PARAM_PTR_TYPE hd_param_ptr )
{
    eps_hw_connect(hd_id, runs_on_level_basic, hd_sys_id_ptr);
	eps_hw_set_hd_param(hd_id, (LSA_VOID_PTR_TYPE) hd_param_ptr);                   /* Fill in HD Params needed by PSI */
}

LSA_VOID eps_hw_free_hd_params( LSA_UINT16 hd_id )
{
	LSA_UINT16 retVal;
	
	retVal = eps_pndev_if_close(hd_id);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
}

LSA_VOID eps_hw_close_hd( LSA_UINT16 hd_id )
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	LSA_UINT16 retVal;
	
	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_close_hd() - closing hd_id = %d", hd_id );
	retVal = eps_pndev_if_get(hd_id, &pBoard);
	if (retVal == EPS_PNDEV_RET_OK)
	{
		retVal = eps_pndev_if_close(hd_id);
		EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
	}
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
