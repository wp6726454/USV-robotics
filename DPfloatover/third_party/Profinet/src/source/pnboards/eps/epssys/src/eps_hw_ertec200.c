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
/*  F i l e               &F: eps_hw_ertec200.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS ERTEC200 adapation                                                   */
/*                                                                           */
/*****************************************************************************/

#define EPS_MODULE_ID      20023
#define LTRC_ACT_MODUL_ID  20023

/* EPS includes */
#include <eps_sys.h>     /* Types / Prototypes */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

// "EDDI_CFG_ERTEC_200" is only defined by "EPS_PLF_ERTEC200_ARM9", so the (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
#if ( ((PSI_CFG_USE_EDDI == 1) && (defined (EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
/*---------------------------------------------------------------------------*/

#include <eps_cp_hw.h>   /* EPS CP PSI adaption        */
#include <eps_cp_mem.h>  /* EPS CP PSI MEM adaption    */
#include <eps_isr.h>     /* EPS ISR adaption           */
#include <eps_locks.h>   /* EPS Locks                  */
#include <eps_trc.h>     /* Tracing                    */

#include <psi_hd.h>

#if !defined( EDDI_CFG_PHY_BLINK_EXTERNAL_NEC )  // must be set for EB200 PnBoards (see pnboards_psi_cfg_plf_eps.h)
#pragma error ("config error EDDI_CFG_PHY_BLINK_EXTERNAL_NEC must be set for EB200-PNBoards")
#endif

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/
#define ERTEC200_x0002500_GPIO_IOCTRL        0x00000000   // IOCTRL register offset in GPIO
#define ERTEC200_x0002504_GPIO_OUT           0x00000004   // OUT register offset in GPIO
#define ERTEC200_x0002508_GPIO_IN            0x00000008   // IN register offset in GPIO
#define ERTEC200_x000250c_GPIO_PORT_MODE_L   0x0000000C   // PORT_MODE_L register offset in GPIO
#define ERTEC200_x0002510_GPIO_PORT_MODE_H   0x00000010   // PORT_MODE_H register offset in GPIO
#define ERTEC200_x0002520_GPIO2_IOCTRL       0x00000020   // IOCTRL2 register offset in GPIO
#define ERTEC200_x0002524_GPIO2_OUT          0x00000024   // OUT2 register offset in GPIO

/*----------------------------------------------------------------------------*/
/*  Types                                                                     */
/*----------------------------------------------------------------------------*/
typedef struct eps_ertec200_hw_store_tag
{
	LSA_UINT16   init_done;                // Entry initialized yes/no
	LSA_UINT16   hw_enter_exit_id;         // Lock for LED of POF GPIO access
	LSA_UINT16   pof_selected_hw_port_id;  // HW port ID selected by POF function
	LSA_UINT16   pof_led_init_done;        // POF LED init done
	EPS_SYS_TYPE eps_sys;
} EPS_ERTEC200_HW_STORE_TYPE, * EPS_ERTEC200_HW_STORE_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/*  Variables                                                                 */
/*----------------------------------------------------------------------------*/

static EPS_ERTEC200_HW_STORE_TYPE eps_ertec200_hw_store[PSI_CFG_MAX_HD_CNT+1];

/*----------------------------------------------------------------------------*/
static LSA_UINT32 eps_get_LED_mask( LSA_UINT16 gpio_nr )
{
	LSA_UINT32 result = 0;  // Default nothing to set/reset

	if ( gpio_nr != 0xFFFF ) // Not set (known) ??
	{
		EPS_ASSERT(( gpio_nr >= 0 ) && ( gpio_nr < 64 )); // in range ?

		result = ( gpio_nr < 32 ) ? ( 1 << gpio_nr) : ( 1 << (gpio_nr - 32) );
	}

	return result;
}

/*----------------------------------------------------------------------------*/
/*  Initialisation                                                            */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_init_hw_ertec200( LSA_UINT16 boardNr )
{
	LSA_RESPONSE_TYPE rsp;

	EPS_ASSERT( !eps_ertec200_hw_store[boardNr].init_done );

	eps_ertec200_hw_store[boardNr].pof_selected_hw_port_id = EPS_HW_PORT_ID_UNDEFINED;
	eps_ertec200_hw_store[boardNr].hw_enter_exit_id        = EPS_LOCK_ID_INVALID;
	eps_ertec200_hw_store[boardNr].pof_led_init_done       = LSA_FALSE;

	rsp = eps_alloc_critical_section( &eps_ertec200_hw_store[boardNr].hw_enter_exit_id, LSA_FALSE );
	EPS_ASSERT( rsp == LSA_RET_OK );

	eps_ertec200_hw_store[boardNr].init_done = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_undo_init_hw_ertec200( LSA_UINT16 boardNr )
{
	LSA_RESPONSE_TYPE rsp;

	EPS_ASSERT( eps_ertec200_hw_store[boardNr].init_done );

	rsp = eps_free_critical_section( eps_ertec200_hw_store[boardNr].hw_enter_exit_id );
	EPS_ASSERT( rsp == LSA_RET_OK );

	eps_ertec200_hw_store[boardNr].pof_selected_hw_port_id = EPS_HW_PORT_ID_UNDEFINED;
	eps_ertec200_hw_store[boardNr].hw_enter_exit_id        = EPS_LOCK_ID_INVALID;
	eps_ertec200_hw_store[boardNr].pof_led_init_done       = LSA_FALSE;

	eps_ertec200_hw_store[boardNr].init_done = LSA_FALSE;	
}

/*----------------------------------------------------------------------------*/
/*  All boards                                                                */
/*----------------------------------------------------------------------------*/

void eps_ertec200_FillNrtMemParams( LSA_VOID * pBoardInfoVoid )
{
	EPS_BOARD_INFO_TYPE * pboardInfo = (EPS_BOARD_INFO_TYPE *)pBoardInfoVoid;

  // setup NRT buffer
  pboardInfo->nrt_mem.base_ptr = pboardInfo->eddi.sdram.base_ptr;
  pboardInfo->nrt_mem.phy_addr = pboardInfo->eddi.sdram.phy_addr;
  pboardInfo->nrt_mem.size     = pboardInfo->eddi.sdram.size;

  pboardInfo->eddi.fragmentation_supported = LSA_FALSE;

  pboardInfo->eddi.nrt_mem_dmacw_default.base_ptr       = 0;
  pboardInfo->eddi.nrt_mem_dmacw_default.phy_addr       = 0;
  pboardInfo->eddi.nrt_mem_dmacw_default.size           = 0;
  pboardInfo->eddi.user_mem_fcts_dmacw_default.Alloc    = eps_cp_mem_nrt_rxtx_default_alloc;
  pboardInfo->eddi.user_mem_fcts_dmacw_default.Free     = eps_cp_mem_nrt_rxtx_default_free;

  pboardInfo->eddi.nrt_mem_rx_default.base_ptr          = 0;
  pboardInfo->eddi.nrt_mem_rx_default.phy_addr          = 0;
  pboardInfo->eddi.nrt_mem_rx_default.size              = 0;
  pboardInfo->eddi.user_mem_fcts_rx_default.Alloc       = eps_cp_mem_nrt_rxtx_default_alloc;
  pboardInfo->eddi.user_mem_fcts_rx_default.Free        = eps_cp_mem_nrt_rxtx_default_free;

  pboardInfo->eddi.nrt_mem_tx_default.base_ptr          = 0;
  pboardInfo->eddi.nrt_mem_tx_default.phy_addr          = 0;
  pboardInfo->eddi.nrt_mem_tx_default.size              = 0;
  pboardInfo->eddi.user_mem_fcts_tx_default.Alloc       = eps_cp_mem_nrt_rxtx_default_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_default.Free        = eps_cp_mem_nrt_rxtx_default_free;

  pboardInfo->eddi.nrt_mem_tx_low_frag.base_ptr         = 0;
  pboardInfo->eddi.nrt_mem_tx_low_frag.phy_addr         = 0;
  pboardInfo->eddi.nrt_mem_tx_low_frag.size             = 0;
  pboardInfo->eddi.user_mem_fcts_tx_low_frag.Alloc      = eps_cp_mem_nrt_fatal_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_low_frag.Free       = eps_cp_mem_nrt_fatal_free;

  pboardInfo->eddi.nrt_mem_tx_mid_frag.base_ptr         = 0;
  pboardInfo->eddi.nrt_mem_tx_mid_frag.phy_addr         = 0;
  pboardInfo->eddi.nrt_mem_tx_mid_frag.size             = 0;
  pboardInfo->eddi.user_mem_fcts_tx_mid_frag.Alloc      = eps_cp_mem_nrt_fatal_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_mid_frag.Free       = eps_cp_mem_nrt_fatal_free;

  pboardInfo->eddi.nrt_mem_tx_high_frag.base_ptr        = 0;
  pboardInfo->eddi.nrt_mem_tx_high_frag.phy_addr        = 0;
  pboardInfo->eddi.nrt_mem_tx_high_frag.size            = 0;
  pboardInfo->eddi.user_mem_fcts_tx_high_frag.Alloc     = eps_cp_mem_nrt_fatal_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_high_frag.Free      = eps_cp_mem_nrt_fatal_free;

  pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.base_ptr     = 0;
  pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.phy_addr     = 0;
  pboardInfo->eddi.nrt_mem_tx_mgmtlow_frag.size         = 0;
  pboardInfo->eddi.user_mem_fcts_tx_mgmtlow_frag.Alloc  = eps_cp_mem_nrt_fatal_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_mgmtlow_frag.Free   = eps_cp_mem_nrt_fatal_free;

  pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.base_ptr    = 0;
  pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.phy_addr    = 0;
  pboardInfo->eddi.nrt_mem_tx_mgmthigh_frag.size        = 0;
  pboardInfo->eddi.user_mem_fcts_tx_mgmthigh_frag.Alloc = eps_cp_mem_nrt_fatal_alloc;
  pboardInfo->eddi.user_mem_fcts_tx_mgmthigh_frag.Free  = eps_cp_mem_nrt_fatal_free; 
}

/*----------------------------------------------------------------------------*/
/*  PLL setting                                                               */
/*----------------------------------------------------------------------------*/

LSA_RESULT eps_set_pllport_ERTEC200 (
	LSA_UINT16  board_nr,
	LSA_UINT32  irte_ptr,
	LSA_UINT32  location,
	LSA_UINT16  pllmode,
	LSA_UINT16  gpio )
{
	LSA_UINT32 regPortModeH;
	LSA_UINT32 regIOCtrl;
	LSA_UINT32 regGpioOut;
	LSA_UINT32 muxSel;
	IRTE_SW_MAKRO_REV6_T *pIRTE;

	LSA_UINT8  *pGPIO             = LSA_NULL;
	LSA_UINT32 *pGPIO_PORT_MODE_H = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL      = LSA_NULL;
	LSA_UINT32 *pGPIO_OUT         = LSA_NULL;

	LSA_RESULT retval = EDD_STS_OK;

	EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store( board_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	// get base pointer to GPIO and IRTE
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	pIRTE = (IRTE_SW_MAKRO_REV6_T*)pBoard->eddi.irte.base_ptr;

	EPS_ASSERT( pGPIO != 0 );
	EPS_ASSERT( pIRTE != 0 );

	// Check the params for the call
	if (( irte_ptr != (LSA_UINT32)pIRTE ) || // not matches
		( location != EDDI_LOC_ERTEC200 ) || // ERTEC200 ?
		( gpio     != 25                ))   // GPIO 25 
	{
		return EDD_STS_ERR_PARAM;
	}

	eps_enter_critical_section( eps_ertec200_hw_store[board_nr].hw_enter_exit_id );

	// get needed GPIO register
	pGPIO_PORT_MODE_H = (LSA_UINT32*)( pGPIO + ERTEC200_x0002510_GPIO_PORT_MODE_H );
	pGPIO_IOCTRL      = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );
	pGPIO_OUT         = (LSA_UINT32*)( pGPIO + ERTEC200_x0002504_GPIO_OUT );

	regPortModeH = *pGPIO_PORT_MODE_H;
	regIOCtrl    = *pGPIO_IOCTRL;
	regGpioOut   = *pGPIO_OUT;
	muxSel       = pIRTE->s11000_Zyklussync.x11030_Mux_Selektor;

	switch ( pllmode )
	{
		case EDDI_PRM_DEF_PLL_MODE_OFF:  // switch output off
		{
			// Set reset state for MUX_Selector
			muxSel = 0;

			// GPIO 23 
			regPortModeH &= ~( 1 << 14 );  // GPIO 23, driver (Bit14:15 = 0:0)
			regPortModeH &= ~( 1 << 15 );
			regIOCtrl    |=  ( 1 << 23 );  // GPIO 23, disable,  (Bit23 = 1)
			regGpioOut   &= ~( 1 << 23 );  // GPIO 23, transmist (Bit23 = 0)

			// GPIO 24
			regPortModeH &= ~( 1 << 16 );  // GPIO 24, XPLL_EXT_IN, high-impedance (Bit16:17 = 0:0)
			regPortModeH &= ~( 1 << 17 );
			regIOCtrl    |=  ( 1 << 24 );  // GPIO 24, output, disable, (Bit24 = 1)

			// GPIO 25
			regPortModeH &= ~( 1 << 19 );  // GPIO 25, XTGEN_OUT1 (Bit18:19 = 1:0)
			regPortModeH |=  ( 1 << 18 );
			regIOCtrl    |=  ( 1 << 25 );  // GPIO 25, output, disable, (Bit25 = 1)
		}
		break;
		case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT:  // set PLL-Output
		{
			// Configuraton MUX_Selector for Mux_Port0 (XTGEN_OUT1) */
			muxSel |= 0x00000007;

			// GPIO 23 
			regPortModeH &= ~( 1 << 14 );  // GPIO 23, driver (Bit14:15 = 0:0)
			regPortModeH &= ~( 1 << 15 );
			regIOCtrl    &= ~( 1 << 23 );  // GPIO 23, enable  (Bit23 = 0)
			regGpioOut   |=  ( 1 << 23 );  // GPIO 23, output  (Bit23 = 1)

			// GPIO 24
			regPortModeH &= ~( 1 << 16 );  // GPIO 24, XPLL_EXT_IN, high-impedance (Bit16:17 = 0:0)
			regPortModeH &= ~( 1 << 17 );
			regIOCtrl    |=  ( 1 << 24 );  // GPIO 24, output  (Bit24 = 1)

			// GPIO 25
			regPortModeH &= ~( 1 << 19 );  // GPIO 25, XTGEN_OUT1 (Bit18:19 = 1:0)
			regPortModeH |=  ( 1 << 18 );
			regIOCtrl    &= ~( 1 << 25 );  // GPIO 25, output  (Bit25 = 0)
		}
		break;
		case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN: // set PLL-Input
		{
			// GPIO programming for XPLL_EXT_IN (Reset Mux_Port0 (XTGEN_OUT1))
			// Note, don't set muxSel, because if XPLLOUT is selected -> leave it!

			// GPIO 23 
			regPortModeH &= ~( 1 << 14 );  // GPIO 23, driver (Bit14:15 = 0:0)
			regPortModeH &= ~( 1 << 15 );
			regIOCtrl    &= ~( 1 << 23 );  // GPIO 23, enable  (Bit23 = 0)
			regGpioOut   &= ~( 1 << 23 );  // GPIO 23, input   (Bit23 = 0)

			// GPIO 24
			regPortModeH |=  ( 1 << 16 );  // GPIO 24, XPLL_EXT_IN (Bit16:17 = 1:0)
			regPortModeH &= ~( 1 << 17 );
			regIOCtrl    |=  ( 1 << 24 );  // GPIO 24, input  (Bit24 = 1)

			// GPIO 25
			// Note: Don't set anything on GPIO 25
		}
		break;
		default:
		{
			retval = EDD_STS_ERR_PARAM;
		}
		break;
	}

	if ( retval == EDD_STS_OK )
	{
		pIRTE->s11000_Zyklussync.x11030_Mux_Selektor = muxSel;
		*pGPIO_PORT_MODE_H                           = regPortModeH;
		*pGPIO_IOCTRL                                = regIOCtrl;
		*pGPIO_OUT                                   = regGpioOut;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[board_nr].hw_enter_exit_id );

	return retval;
}

/*----------------------------------------------------------------------------*/
/*  POF LED settings                                                          */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_pof_set_led_EB200( LSA_UINT16 portID, LSA_BOOL on, LSA_SYS_PTR_TYPE sysPtr )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	LSA_UINT8  *pGPIO           = NULL;
	LSA_UINT32 *pGPIO_IOCTRL2   = NULL;
	LSA_UINT32 *pGPIO_OUT2      = NULL;
	EPS_SYS_PTR_TYPE           pSys;

	// Note: on EB200 with POF ports following GPIO nr (on GPIO2) are used for LEDs
	//       POF1_LED (=GPIO 35) POF1_LED, POF2_LED (=GPIO 38)
	LSA_UINT32 pof1Led = eps_get_LED_mask( 35 ); // POF1_LED;
	LSA_UINT32 pof2Led = eps_get_LED_mask( 38 ); // POF2_LED;

	pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );
	EPS_ASSERT(( portID >= 1 ) && ( portID <= pBoard->nr_of_ports ));

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	if ( !eps_ertec200_hw_store[pSys->hd_nr].pof_led_init_done ) // POF LED initialized ?
	{
	    if (PSI_MEDIA_TYPE_FIBER == psi_hd_get_psi_media_type(pSys->hd_nr, pSys->if_nr, pBoard->port_map[0][1].hw_port_id)) //POF port?
		{
			// Set GPIO2_xx to output lowactive, and initialize with LED OFF
			*pGPIO_IOCTRL2 &= ~pof1Led;
			*pGPIO_OUT2    |= pof1Led;
		}

	    if (PSI_MEDIA_TYPE_FIBER == psi_hd_get_psi_media_type(pSys->hd_nr, pSys->if_nr, pBoard->port_map[0][2].hw_port_id)) //POF port?
		{
			// Set GPIO2_xx to output lowactive, and initialize with LED OFF
			*pGPIO_IOCTRL2 &= ~pof2Led;
			*pGPIO_OUT2    |= pof2Led;
		}

		eps_ertec200_hw_store[pSys->hd_nr].pof_led_init_done = LSA_TRUE;
	}

	if (PSI_MEDIA_TYPE_FIBER == psi_hd_get_psi_media_type(pSys->hd_nr, pSys->if_nr, pBoard->port_map[0][portID].hw_port_id)) //POF port?
	{
		switch( pBoard->port_map[0][portID].hw_port_id ) // GPIO setting is based on HW port ID mapping
		{
		    case 1: // POF1_LED
			{
				if ( on )
				{
					*pGPIO_OUT2 &= ~pof1Led; // GPIO_xx (LED = on)
				}
				else
				{
					*pGPIO_OUT2 |= pof1Led;  // GPIO_xx (LED = off) 
				}
			}
			break;
		    case 2: // POF2_LED
			{
				if ( on )
				{
					*pGPIO_OUT2 &= ~pof2Led; // GPIO_xx (LED = on)
				}
				else
				{
					*pGPIO_OUT2 |= pof2Led;  // GPIO_xx (LED = off) 
				}
			}
			break;
		    default: break;
		}
	}
	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
/*  I2C adaption                                                              */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_i2c_set_scl_low_high_EB200( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL Level )
{
	LSA_UINT8  *pGPIO        = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL = LSA_NULL;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	// get needed GPIO register
	pGPIO_IOCTRL = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	// Note: on EB200 with POF ports following GPIO nr (on GPIO1) are used for SCL
	//       POF1_SCL (=GPIO 15) POF2_SCL(=GPIO 03)

	// use actual selected POF port for set (set over EDDI_I2C_PORTID)
	switch( eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id )
	{
	    case 1: // HW port 1
		{
			if ( Level ) // Set to input ?
			{
				*pGPIO_IOCTRL |= ( 1 << 15 );  // GPIO 15 (Bit15 = 1)   --> POF1_SCL_INPUT
			}
			else
			{
				*pGPIO_IOCTRL &= ~( 1 << 15 ); // GPIO 15 (Bit15 = 0)   --> POF1_SCL_OUTPUT
			}
		}
		break;
	    case 2: // HW port 2
		{
			if ( Level ) // Set to input ?
			{
				*pGPIO_IOCTRL |= ( 1 << 3 );  // GPIO 3 (Bit3 = 1)   --> POF2_SCL_INPUT
			}
			else
			{
				*pGPIO_IOCTRL &= ~( 1 << 3 ); // GPIO 3 (Bit3 = 0)   --> POF2_SCL_OUTPUT
			}
		}
		break;
	    default: break;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_low_high_EB200( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL level )
{
	LSA_UINT8  *pGPIO        = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL = LSA_NULL;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	// get needed GPIO register
	pGPIO_IOCTRL = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	// Note: on EB200 with POF ports following GPIO nr (on GPIO1) are used for SDA
	//       POF1_SDA (=GPIO 13) POF2_SDA(=GPIO 02)

	// use actual selected POF port for set (set over EDDI_I2C_PORTID)
	switch( eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id )
	{
	    case 1: // HW Port 1
		{
			if ( level ) // Set to input ?
			{
				*pGPIO_IOCTRL |= ( 1 << 13 );  // GPIO 13 (Bit13 = 1)   --> POF1_SDA_INPUT
			}
			else
			{
				*pGPIO_IOCTRL &= ~( 1 << 13 ); // GPIO 13 (Bit13 = 0)   --> POF1_SDA_OUTPUT
			}
		}
		break;
	    case 2: // HW Port 2
		{
			if ( level ) // Set to input ?
			{
				*pGPIO_IOCTRL |= ( 1 << 2 );  // GPIO 2 (Bit2 = 1)   --> POF2_SDA_INPUT
			}
			else
			{
				*pGPIO_IOCTRL &= ~( 1 << 2 ); // GPIO 2 (Bit2 = 0)   --> POF2_SDA_OUTPUT
			}
		}
		break;
	    default: break;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_UINT8 eps_i2c_sda_read_EB200( LSA_SYS_PTR_TYPE sysPtr )
{
	LSA_UINT8  * pGPIO    = LSA_NULL;
	LSA_UINT32 * pGPIO_IN = LSA_NULL;
	LSA_UINT8    result   = 0;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	// get needed register
	pGPIO_IN = (LSA_UINT32*)( pGPIO + ERTEC200_x0002508_GPIO_IN );

	// Note: on EB200 with POF ports following GPIO nr (on GPIO1) are used for SDA
	//       POF1_SDA (=GPIO 13) POF2_SDA(=GPIO 02)

	// use actual selected POF port for set (set over EDDI_I2C_PORTID)
	switch( eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id )
	{
	    case 1:  // HW port 1
		{
			LSA_UINT32 readMask = ( 1 << 13 );
			
			result = (( *pGPIO_IN & readMask ) == readMask) ? 1 : 0;
		}
		break;
	    case 2:  // HW port 2
		{
			LSA_UINT32 readMask = ( 1 << 2 );
			
			result = (( *pGPIO_IN & readMask ) == readMask) ? 1 : 0;
		}
		break;
	    default: break;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_in_out_EB200( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UINT8  *pGPIO        = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL = LSA_NULL;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	// get needed GPIO register
	pGPIO_IOCTRL = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	// Note: on EB200 with POF ports following GPIO nr (on GPIO1) are used for SDA
	//       POF1_SDA (=GPIO 13) POF2_SDA(=GPIO 02)

	// use actual selected POF port for set (set over EDDI_I2C_PORTID)
	switch( eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id )
	{
	    case 1:  // HW port 1
		{
			if ( outPut ) // Set to input ?
			{
				*pGPIO_IOCTRL &= ~( 1 << 13 ); // GPIO 13 (Bit13 = 0)   --> POF1_SDA_OUTPUT
			}
			else
			{
				*pGPIO_IOCTRL |= ( 1 << 13 );  // GPIO 13 (Bit13 = 1)   --> POF1_SDA_INPUT
			}
		}
		break;
	    case 2:  // HW port 1
		{
			if ( outPut ) // Set to input ?
			{
				*pGPIO_IOCTRL &= ~( 1 << 2 ); // GPIO 2 (Bit2 = 0)   --> POF2_SDA_OUTPUT
			}
			else
			{
				*pGPIO_IOCTRL |= ( 1 << 2 );  // GPIO 2 (Bit2 = 1)   --> POF2_SDA_INPUT
			}
		}
		break;
	    default: break;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_scl_in_out_EB200( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	LSA_UINT8  *pGPIO        = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL = LSA_NULL;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	// get needed GPIO register
	pGPIO_IOCTRL = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	// Note: on EB200 with POF ports following GPIO nr (on GPIO1) are used for SCL
	//       POF1_SCL (=GPIO 15) POF2_SCL(=GPIO 03)

	// use actual selected POF port for set (set over EDDI_I2C_PORTID)
	switch( eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id )
	{
	    case 1: // HW port 1
		{
			if ( outPut ) // Set to output ?
			{
				*pGPIO_IOCTRL &= ~( 1 << 15 ); // GPIO 15 (Bit15 = 0)   --> POF1_SCL_OUTPUT
			}
			else
			{
				*pGPIO_IOCTRL |= ( 1 << 15 );  // GPIO 15 (Bit15 = 1)   --> POF1_SCL_INPUT
			}
		}
		break;
	    case 2: // HW port 1
		{
			if ( outPut ) // Set to output ?
			{
				*pGPIO_IOCTRL &= ~( 1 << 3 ); // GPIO 3 (Bit3 = 0)   --> POF2_SCL_OUTPUT
			}
			else
			{
				*pGPIO_IOCTRL |= ( 1 << 3 );  // GPIO 3 (Bit3 = 1)   --> POF2_SCL_INPUT
			}
		}
		break;
	    default: break;
	}

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_BOOL eps_i2c_select_EB200( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT16 PortId, LSA_UINT16 I2CMuxSelect )
{
	LSA_BOOL result = LSA_FALSE;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );
	EPS_ASSERT( (PortId) && (PortId <= pBoard->nr_of_ports) );
	EPS_ASSERT( (I2CMuxSelect) && (I2CMuxSelect <= PSI_CFG_MAX_PORT_CNT) );

	switch( PortId ) // User port ID (from 1..N)
	{
	    case 1:
	    case 2:
		{
			LSA_UINT32 hw_port_id = pBoard->port_map[0][PortId].hw_port_id;

			if (PSI_MEDIA_TYPE_FIBER == psi_hd_get_psi_media_type(pSys->hd_nr, pSys->if_nr, hw_port_id)) //POF port?
			{
				eps_ertec200_hw_store[pSys->hd_nr].pof_selected_hw_port_id = (LSA_UINT16)hw_port_id;
				result = LSA_TRUE;
			}
			break;
		}
	    default: break;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_write_offset_EB200( 
	LSA_SYS_PTR_TYPE sysPtr,
	LSA_UINT8        I2CDevAddr,
	LSA_UINT8        I2COffsetCnt,
	LSA_UINT8        I2COffset1,
	LSA_UINT8        I2COffset2,
	LSA_UINT32       Size,
	LSA_UINT8       *pBuf )
{
	LSA_UNUSED_ARG(sysPtr);
	LSA_UNUSED_ARG(I2CDevAddr);
	LSA_UNUSED_ARG(I2COffsetCnt);
	LSA_UNUSED_ARG(I2COffset1);
	LSA_UNUSED_ARG(I2COffset2);
	LSA_UNUSED_ARG(Size);
	LSA_UNUSED_ARG(pBuf);

	EPS_FATAL("not expected call for an EB200 board");

	return EDD_STS_ERR_NOT_ALLOWED;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_read_offset_EB200( 
	LSA_SYS_PTR_TYPE sysPtr,
	LSA_UINT8        I2CDevAddr,
	LSA_UINT8        I2COffsetCnt,
	LSA_UINT8        I2COffset1,
	LSA_UINT8        I2COffset2,
	LSA_UINT32       Size,
	LSA_UINT8       *pBuf )
{
	LSA_UNUSED_ARG(sysPtr);
	LSA_UNUSED_ARG(I2CDevAddr);
	LSA_UNUSED_ARG(I2COffsetCnt);
	LSA_UNUSED_ARG(I2COffset1);
	LSA_UNUSED_ARG(I2COffset2);
	LSA_UNUSED_ARG(Size);
	LSA_UNUSED_ARG(pBuf);

	EPS_FATAL("not expected call for an EB200 board");

	return EDD_STS_ERR_NOT_ALLOWED;
}

/*----------------------------------------------------------------------------*/
/*   EB200                                                                    */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*        PHY LEDs                                                            */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_phy_led_blink_begin_EB200( 	
    EDDI_SYS_HANDLE hSysDev,
	LSA_UINT32 HwPortIndex,
	LSA_UINT32 PhyTransceiver)
{
	// Disable PHY LED standard function for user ON/OFF setting
	EPS_BOARD_INFO_PTR_TYPE  pBoard;
	LSA_UINT32 regPortModeL;
	LSA_UINT32 regIOCtrl;

	LSA_UINT8  *pGPIO             = LSA_NULL;
	LSA_UINT32 *pGPIO_PORT_MODE_L = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL      = LSA_NULL;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT( pSys != 0 );
	
	pBoard = eps_hw_get_board_store(pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	EPS_ASSERT( PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC );      // only NEC expected 

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get needed GPIO register
	pGPIO_PORT_MODE_L = (LSA_UINT32*)( pGPIO + ERTEC200_x000250c_GPIO_PORT_MODE_L );
	pGPIO_IOCTRL      = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	regPortModeL = *pGPIO_PORT_MODE_L;
	regIOCtrl    = *pGPIO_IOCTRL;

	switch (HwPortIndex)
	{
	    case 0: // Port 1
		{ 
			regPortModeL &= ~( 1 << 9 );   // GPIO 4 (Bit8:9 = 0:0)   --> P1-XLINK-LED, Function 2
			regPortModeL &= ~( 1 << 8 );
			regIOCtrl    &= ~( 1 << 4 );   // GPIO 4, output  (Bit4 = 0)

			regPortModeL &= ~( 1 << 12 );  // GPIO 6 (Bit12:13 = 0:0) --> P1-XACTIVITY-LED, Function 4
			regPortModeL &= ~( 1 << 13 );
			regIOCtrl    &= ~( 1 << 6  );  // GPIO 6, output  (Bit6 = 0)

		}
		break;
	    case 1: // Port 2
		{
			regPortModeL &= ~( 1 << 11 );  // GPIO 5 (Bit10:11 = 0:0) --> P2-XLINK-LED, Function 2
			regPortModeL &= ~( 1 << 10 );
			regIOCtrl    &= ~( 1 << 5  );  // GPIO 5, output  (Bit5 = 0)


			regPortModeL &= ~( 1 << 14 );  // GPIO 7 (Bit14:15 = 0:0) --> P2-XACTIVITY-LED, Function 4
			regPortModeL &= ~( 1 << 15 );
			regIOCtrl    &= ~( 1 << 7  );  // GPIO 7, output  (Bit7 = 0)
		}
		break;
	    default: break;
	}

	*pGPIO_PORT_MODE_L = regPortModeL;
	*pGPIO_IOCTRL      = regIOCtrl;

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_phy_led_blink_end_EB200( 
	EDDI_SYS_HANDLE  hSysDev,
	LSA_UINT32       HwPortIndex,
	LSA_UINT32       PhyTransceiver )
{
	// Enable PHY LED standard function for user ON/OFF setting
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	LSA_UINT32 regPortModeL;
	LSA_UINT32 regIOCtrl;

	LSA_UINT8  *pGPIO             = LSA_NULL;
	LSA_UINT32 *pGPIO_PORT_MODE_L = LSA_NULL;
	LSA_UINT32 *pGPIO_IOCTRL      = LSA_NULL;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	EPS_ASSERT( PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC );      // only NEC expected 

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get needed GPIO register
	pGPIO_PORT_MODE_L = (LSA_UINT32*)( pGPIO + ERTEC200_x000250c_GPIO_PORT_MODE_L );
	pGPIO_IOCTRL      = (LSA_UINT32*)( pGPIO + ERTEC200_x0002500_GPIO_IOCTRL );

	regPortModeL = *pGPIO_PORT_MODE_L;
	regIOCtrl    = *pGPIO_IOCTRL;

	switch (HwPortIndex)
	{
	    case 0: // Port 1
		{ 
			regPortModeL &= ~( 1 << 9 );   // GPIO 4 (Bit8:9 = 1:0)   --> P1-XLINK-LED, Function 2
			regPortModeL |=  ( 1 << 8 );
			regIOCtrl    &= ~( 1 << 4 );   // GPIO 4, output  (Bit4 = 0)

			regPortModeL |=  ( 1 << 12 );  // GPIO 6 (Bit12:13 = 1:1) --> P1-XACTIVITY-LED, Function 4
			regPortModeL |=  ( 1 << 13 );
			regIOCtrl    &= ~( 1 << 6  );  // GPIO 6, output  (Bit6 = 0)
		}
		break;
	    case 1: // Port 2
		{
			regPortModeL &= ~( 1 << 11 );  // GPIO 5 (Bit10:11 = 1:0) --> P2-XLINK-LED, Function 2
			regPortModeL |=  ( 1 << 10 );
			regIOCtrl    &= ~( 1 << 5  );  // GPIO 5, output  (Bit5 = 0)

			regPortModeL |=  ( 1 << 14 );  // GPIO 7 (Bit14:15 = 1:1) --> P2-XACTIVITY-LED, Function 4
			regPortModeL |=  ( 1 << 15 );
			regIOCtrl    &= ~( 1 << 7  );  // GPIO 7, output  (Bit7 = 0)
		}
		break;
	    default: break;
	}

	*pGPIO_PORT_MODE_L = regPortModeL;
	*pGPIO_IOCTRL      = regIOCtrl;

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_phy_led_blink_set_mode_EB200( 
	EDDI_SYS_HANDLE  hSysDev, 
	LSA_UINT32       HwPortIndex, 
	LSA_UINT32       PhyTransceiver,
	LSA_UINT16       mode )
{
	// Set PHY LEDS to ON/OFF triggered by user (for show location)
	LSA_UINT32  regGpioOut;
	LSA_UINT8  *pGPIO     = LSA_NULL;
	LSA_UINT32 *pGPIO_OUT = LSA_NULL;

	EPS_BOARD_INFO_PTR_TYPE pBoard;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_ASSERT( pSys != 0 );

	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );

	// get base pointer to GPIO
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	EPS_ASSERT( pGPIO != 0 );

	EPS_ASSERT( PhyTransceiver == EDDI_PHY_TRANSCEIVER_NEC );      // only NEC expected 

	eps_enter_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// get needed GPIO register
	pGPIO_OUT  = (LSA_UINT32*)( pGPIO + ERTEC200_x0002504_GPIO_OUT );
	regGpioOut = *pGPIO_OUT;

	switch (HwPortIndex)
	{
	    case 0: // Port 1
		{ 
			if ( mode == EDDI_LED_MODE_ON )
			{
				regGpioOut &= ~( 1 << 4 );   // GPIO 4 (Bit4 = 0)   --> P1-XLINK-LED on
				regGpioOut &= ~( 1 << 6 );   // GPIO 6 (Bit6 = 0)   --> P1-XACTIVITY-LED on
			}
			else
			{
				regGpioOut |= ( 1 << 4 );   // GPIO 4 (Bit4 = 1)   --> P1-XLINK-LED off
				regGpioOut |= ( 1 << 6 );   // GPIO 6 (Bit6 = 1)   --> P1-XACTIVITY-LED off
			}
		}
		break;
	    case 1: // Port 2
		{
			if ( mode == EDDI_LED_MODE_ON )
			{
				regGpioOut &= ~( 1 << 5 );   // GPIO 5 (Bit5 = 0)   --> P2-XLINK-LED on
				regGpioOut &= ~( 1 << 7 );   // GPIO 7 (Bit7 = 0)   --> P2-XACTIVITY-LED on
			}
			else
			{
				regGpioOut |= ( 1 << 5 );   // GPIO 5 (Bit5 = 1)   --> P2-XLINK-LED off
				regGpioOut |= ( 1 << 7 );   // GPIO 6 (Bit7 = 1)   --> P2-XACTIVITY-LED off
			}
		}
		break;
	    default: break;
	}

	*pGPIO_OUT = regGpioOut;

	eps_exit_critical_section( eps_ertec200_hw_store[pSys->hd_nr].hw_enter_exit_id );
}

/*----------------------------------------------------------------------------*/
#endif


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
