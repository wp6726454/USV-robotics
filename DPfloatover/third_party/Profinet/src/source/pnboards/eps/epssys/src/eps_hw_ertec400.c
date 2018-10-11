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
/*  F i l e               &F: eps_hw_ertec400.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS ERTEC400 adapation                                                   */
/*                                                                           */
/*****************************************************************************/

#define EPS_MODULE_ID      20022
#define LTRC_ACT_MODUL_ID  20022

/* EPS includes */
#include <eps_sys.h>     /* Types / Prototypes */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

// "EDDI_CFG_ERTEC_400" is only defined by "EPS_PLF_ERTEC400_ARM9", so the (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
#if ( ((PSI_CFG_USE_EDDI == 1) && (defined (EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
/*---------------------------------------------------------------------------*/

#include <eps_cp_hw.h>   /* EPS CP PSI adaption        */
#include <eps_cp_mem.h>  /* EPS CP PSI MEM adaption    */
#include <eps_isr.h>     /* EPS ISR adaption           */
#include <eps_locks.h>   /* EPS Locks                  */
#include <eps_trc.h>     /* Tracing                    */

/*----------------------------------------------------------------------------*/
/*  Defines                                                                   */
/*----------------------------------------------------------------------------*/
#define EPS_ERTEC400_GPIO_IOCTRL            0x00000000  // IOCTRL register offset in GPIO
#define EPS_ERTEC400_GPIO_OUT               0x00000004  // OUT register offset in GPIO
#define EPS_ERTEC400_GPIO_IN                0x00000008  // IN register offset in GPIO
#define EPS_ERTEC400_GPIO_PORT_MODE_L       0x0000000C  // PORT_MODE_L register offset in GPIO
#define EPS_ERTEC400_GPIO_PORT_MODE_H       0x00000010  // PORT_MODE_H register offset in GPIO

// I2C on ERTEC400 PCIe board
#define EPS_ERTEC400_GPIO_I2C_SCL           0x08        // GPIO for I2C SCL
#define EPS_ERTEC400_GPIO_I2C_SDA           0x04        // GPIO for I2C SDA

#define EPS_ERTEC400_GPIO_I2C_MUX_S0        0x10        // GPIO for I2C Mux Select 0
#define EPS_ERTEC400_GPIO_I2C_MUX_S1        0x20        // GPIO for I2C Mux Select 1

#define EPS_I2C_SDA_SHIFT                   2           // SDA shift - adapt to SDA -> GPIO position

#define READ_REG_ULONG(offset)              ((*(volatile unsigned long *)((g_GPIO_Register) + (offset))))

#define WRITE_REG_ULONG(offset, val)        (*(volatile unsigned long *)((g_GPIO_Register) + (offset)) = (val))

/*----------------------------------------------------------------------------*/
/*  Types                                                                     */
/*----------------------------------------------------------------------------*/
typedef struct eps_ertec400_hw_store_tag
{
	LSA_UINT16      init_done;                // Entry initialized yes/no
	LSA_UINT16      hw_enter_exit_id;         // Lock for LED of POF GPIO access
	EPS_SYS_TYPE    eps_sys;
} EPS_ERTEC400_HW_STORE_TYPE, * EPS_ERTEC400_HW_STORE_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/*  Variables                                                                 */
/*----------------------------------------------------------------------------*/

static EPS_ERTEC400_HW_STORE_TYPE eps_ertec400_hw_store[PSI_CFG_MAX_HD_CNT+1];

// pointer to GPIO-register-set used by specific macros
static LSA_UINT32 g_GPIO_Register = 0;

/*----------------------------------------------------------------------------*/
/*  Source                                                                    */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_init_hw_ertec400( LSA_UINT16 boardNr )
{
	LSA_RESPONSE_TYPE rsp;

	EPS_ASSERT( !eps_ertec400_hw_store[boardNr].init_done );

	eps_ertec400_hw_store[boardNr].hw_enter_exit_id = EPS_LOCK_ID_INVALID;

	rsp = eps_alloc_critical_section( &eps_ertec400_hw_store[boardNr].hw_enter_exit_id, LSA_FALSE );
	EPS_ASSERT( rsp == LSA_RET_OK );

	eps_ertec400_hw_store[boardNr].init_done = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_undo_init_hw_ertec400( LSA_UINT16 boardNr )
{
	LSA_RESPONSE_TYPE rsp;
	
	EPS_ASSERT( eps_ertec400_hw_store[boardNr].init_done );

	rsp = eps_free_critical_section( eps_ertec400_hw_store[boardNr].hw_enter_exit_id );
	EPS_ASSERT( rsp == LSA_RET_OK );

	eps_ertec400_hw_store[boardNr].hw_enter_exit_id = EPS_LOCK_ID_INVALID;

	eps_ertec400_hw_store[boardNr].init_done = LSA_FALSE;	
}

/*----------------------------------------------------------------------------*/
/*  All boards                                                                */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_ertec400_FillNrtMemParams( LSA_VOID * pboardInfoVoid )
{
  EPS_BOARD_INFO_TYPE * pboardInfo = (EPS_BOARD_INFO_TYPE *)pboardInfoVoid;

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

LSA_RESULT eps_set_pllport_ERTEC400( LSA_UINT16 board_nr,
									 LSA_UINT32 irte_ptr,
									 LSA_UINT32 location,
									 LSA_UINT16 pllmode,
									 LSA_UINT16 gpio )
{
	LSA_UINT32        regPortModeH;
	LSA_UINT32        regIOCtrl;
	LSA_UINT32        regGpioOut;
	LSA_UINT32        muxSel;
	IRTE_SW_MAKRO_T * pIRTE;
	
	LSA_UINT8  * pGPIO;
	LSA_UINT32 * pGPIO_PORT_MODE_H = LSA_NULL;
	LSA_UINT32 * pGPIO_IOCTRL      = LSA_NULL;
	LSA_UINT32 * pGPIO_OUT         = LSA_NULL;

	LSA_RESULT retval = EDD_STS_OK;

	EPS_BOARD_INFO_PTR_TYPE pBoard = eps_hw_get_board_store( board_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );

	// get base pointer to GPIO and IRTE
	pGPIO = pBoard->eddi.apb_periph_gpio.base_ptr;
	pIRTE = (IRTE_SW_MAKRO_T*)pBoard->eddi.irte.base_ptr;

	EPS_ASSERT( pGPIO );
	EPS_ASSERT( pIRTE );

	// Check the params for the call
	if (( irte_ptr != (LSA_UINT32)pIRTE  ) || // not matches
		( location != EDDI_LOC_ERTEC400  ) || // ERTEC400 ?
		( gpio     != 25                 ))   // GPIO 25 
	{
		return EDD_STS_ERR_PARAM;
	}

	eps_enter_critical_section( eps_ertec400_hw_store[board_nr].hw_enter_exit_id );

	// get needed GPIO register
	pGPIO_PORT_MODE_H = (LSA_UINT32*)( pGPIO + EPS_ERTEC400_GPIO_PORT_MODE_H );
	pGPIO_IOCTRL      = (LSA_UINT32*)( pGPIO + EPS_ERTEC400_GPIO_IOCTRL );
	pGPIO_OUT         = (LSA_UINT32*)( pGPIO + EPS_ERTEC400_GPIO_OUT );

	// get register values by dereferencing the pointers
	regPortModeH = *pGPIO_PORT_MODE_H;
	regIOCtrl    = *pGPIO_IOCTRL;
	regGpioOut   = *pGPIO_OUT;
	muxSel       = pIRTE->s11000_Zyklussync.x11030_Mux_Selektor;

	switch( pllmode )
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
			// Configuraton MUX_Selector for Mux_Port0 (XTGEN_OUT1)
			muxSel |= 0x00000007;

			// GPIO 23 
			regPortModeH &= ~( 1 << 14 );  // GPIO 23, driver (Bit14:15 = 0:0)
			regPortModeH &= ~( 1 << 15 );
			regIOCtrl    &= ~( 1 << 23 );  // GPIO 23, enable  (Bit23 = 0)
			regGpioOut   |= ( 1 << 23 );   // GPIO 23, output  (Bit23 = 1)

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
		*pGPIO_PORT_MODE_H = regPortModeH;
		*pGPIO_IOCTRL      = regIOCtrl;
		*pGPIO_OUT         = regGpioOut;
	}

	eps_exit_critical_section( eps_ertec400_hw_store[board_nr].hw_enter_exit_id );

	return retval;
}

/*----------------------------------------------------------------------------*/
/*  POF LED settings                                                          */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_pof_set_led_ERTEC400( LSA_UINT16 portID, LSA_BOOL on, LSA_SYS_PTR_TYPE sysPtr ) //ToDo (LaM) Check if needed
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr != 0 );

	pSys = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard != 0 );
	EPS_ASSERT( pBoard->eddi.is_valid != 0 );
	EPS_ASSERT(( portID >= 1 ) && ( portID <= pBoard->nr_of_ports ));

	LSA_UNUSED_ARG( on );

    #ifdef EPS_MESSAGE
    #pragma EPS_MESSAGE("TODO: Add ERTEC400 specific POF LED setting here")
    #endif
}

/*----------------------------------------------------------------------------*/
/*  I2C adaption                                                              */
/*----------------------------------------------------------------------------*/

LSA_VOID eps_i2c_set_scl_low_high_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL level )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	if (level)
	{
		// set SCL to high level: set SCL to input => set to Tristate => high level is set via pull up resistor
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) | EPS_ERTEC400_GPIO_I2C_SCL); // set SCL to input

		//WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) | EPS_ERTEC400_GPIO_I2C_SCL); // SCL = high
	}
	else
	{
		//set SCL to low level: switch Tristate to output => set SCL to low level
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT,    READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT)    & ~EPS_ERTEC400_GPIO_I2C_SCL); // set SCL to low level
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) & ~EPS_ERTEC400_GPIO_I2C_SCL); // set SCL to output

		//WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) & ~EPS_ERTEC400_GPIO_I2C_SCL); // SCL = low
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_low_high_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL level )
{
    // This function is for a connection with a pull up resistor on the SDA line.
    // To set SDA to low level, SDA data direction must be set to output and level must be set to 0.
    // To set SDA to high level, only data direction must be set to input. The pull up resistor sets the SDA line to high level.

	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	if (level)
	{
		// set SDA to high level: set SDA to input => set to Tristate => high level is set via pull up resistor
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) | EPS_ERTEC400_GPIO_I2C_SDA); // set SDA to input

		//WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) | EPS_ERTEC400_GPIO_I2C_SDA); // SDA = high
	}
	else
	{
		//set SDA to low level: switch Tristate to output => set SDA to low level
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT,    READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT)    & ~EPS_ERTEC400_GPIO_I2C_SDA); // set SDA to low level
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) & ~EPS_ERTEC400_GPIO_I2C_SDA); // set SDA to output

		//WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) & ~EPS_ERTEC400_GPIO_I2C_SDA); // SDA = low
	}
}

/*----------------------------------------------------------------------------*/
LSA_UINT8 eps_i2c_sda_read_ERTEC400( LSA_SYS_PTR_TYPE sysPtr )
{
	LSA_UINT8               sda_read = 0;
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	sda_read = (LSA_UINT8)(((READ_REG_ULONG(EPS_ERTEC400_GPIO_IN) >> EPS_I2C_SDA_SHIFT) & 0x00000001UL));

	return sda_read;
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_sda_in_out_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
    // Behavior of funktion eps_i2c_set_sda_low_high_ERTEC400:
    // This function is for a connection with a pull up resistor on the SDA line.
    // To set SDA to low level, SDA data direction must be set to output and level must be set to 0.
    // To set SDA to high level, only data direction must be set to input. The pull up resistor sets the SDA line to high level.
    // In this case eps_i2c_set_sda_in_out_ERTEC400 is "overwritten".

	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	if (outPut)
	{
		// set SDA to high level: set SDA to input => set to Tristate => high level is set via pull up resistor
        WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) | EPS_ERTEC400_GPIO_I2C_SDA); // set SDA to input

		//WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) & ~EPS_ERTEC400_GPIO_I2C_SDA)); // set SDA to output
	}
	else
	{
        WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) | EPS_ERTEC400_GPIO_I2C_SDA); // set SDA to input
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_i2c_set_scl_in_out_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut )
{
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	if (outPut)
	{
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) & ~EPS_ERTEC400_GPIO_I2C_SCL); // set SCL to output
	}
	else
	{
		WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) | EPS_ERTEC400_GPIO_I2C_SCL); // set SCL to input
	}
}

/*----------------------------------------------------------------------------*/
LSA_BOOL eps_i2c_select_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT16 PortId, LSA_UINT16 I2CMuxSelect ) // implementation for EB400 PCIe board
{
	LSA_UINT32              hw_port_id;
	EPS_BOARD_INFO_PTR_TYPE pBoard;
	EPS_SYS_PTR_TYPE        pSys;
	EPS_ASSERT( sysPtr );

	pSys   = (EPS_SYS_PTR_TYPE)sysPtr;
	pBoard = eps_hw_get_board_store( pSys->hd_nr );
	EPS_ASSERT( pBoard );
	EPS_ASSERT( pBoard->eddi.is_valid );
	EPS_ASSERT( PortId && (PortId <= pBoard->nr_of_ports) );
	EPS_ASSERT( I2CMuxSelect && (I2CMuxSelect <= PSI_CFG_MAX_PORT_CNT) );
	EPS_ASSERT( pBoard->eddi.apb_periph_gpio.base_ptr );

	hw_port_id = pBoard->port_map[0][PortId].hw_port_id;

    // The I2C device must be selected via a HW multiplexer controlled by signals I2C_MUX_S0 and I2C_MUX_S1.
    //
    //  I2C_MUX_S0  I2C_MUX_S1  selection
    //  ---------------------------------------------------
    //  0	        0		    Transceiver Port 1 (HwPortId)
    //  1	        0		    Transceiver Port 2 (HwPortId)
    //  0	        1		    Transceiver Port 3 (HwPortId)
    //  1	        1		    Transceiver Port 4 (HwPortId)

	g_GPIO_Register = (LSA_UINT32)pBoard->eddi.apb_periph_gpio.base_ptr;

	eps_enter_critical_section( eps_ertec400_hw_store[pSys->hd_nr].hw_enter_exit_id );

	// set Multiplexer I2C_MUX_S0 and I2C_MUX_S1 to output
	WRITE_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL, READ_REG_ULONG(EPS_ERTEC400_GPIO_IOCTRL) & ~(EPS_ERTEC400_GPIO_I2C_MUX_S0 | EPS_ERTEC400_GPIO_I2C_MUX_S1));

	switch (hw_port_id)
	{
		case 1: // = HwPortId 1
            // I2C_MUX_S0 = 0, I2C_MUX_S1 = 0
		    WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) & ~(EPS_ERTEC400_GPIO_I2C_MUX_S0 | EPS_ERTEC400_GPIO_I2C_MUX_S1));
			break;
		case 2: // = HwPortId 2
            // I2C_MUX_S0 = 1, I2C_MUX_S1 = 0
            WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, (READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) & ~EPS_ERTEC400_GPIO_I2C_MUX_S1) | EPS_ERTEC400_GPIO_I2C_MUX_S0);
			break;
		case 3: // = HwPortId 3
            // I2C_MUX_S0 = 0, I2C_MUX_S1 = 1
            WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, (READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) & ~EPS_ERTEC400_GPIO_I2C_MUX_S0) | EPS_ERTEC400_GPIO_I2C_MUX_S1);
			break;
		case 4: // = HwPortId 4
            // I2C_MUX_S0 = 1, I2C_MUX_S1 = 1
            WRITE_REG_ULONG(EPS_ERTEC400_GPIO_OUT, READ_REG_ULONG(EPS_ERTEC400_GPIO_OUT) | (EPS_ERTEC400_GPIO_I2C_MUX_S0 | EPS_ERTEC400_GPIO_I2C_MUX_S1));
			break;
	}

	eps_exit_critical_section( eps_ertec400_hw_store[pSys->hd_nr].hw_enter_exit_id );

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_write_offset_ERTEC400( 
	LSA_SYS_PTR_TYPE   sysPtr,
	LSA_UINT8          I2CDevAddr,
	LSA_UINT8          I2COffsetCnt,
	LSA_UINT8          I2COffset1,
	LSA_UINT8          I2COffset2,
	LSA_UINT32         Size,
	LSA_UINT8        * pBuf )
{
	LSA_UNUSED_ARG(sysPtr);
	LSA_UNUSED_ARG(I2CDevAddr);
	LSA_UNUSED_ARG(I2COffsetCnt);
	LSA_UNUSED_ARG(I2COffset1);
	LSA_UNUSED_ARG(I2COffset2);
	LSA_UNUSED_ARG(Size);
	LSA_UNUSED_ARG(pBuf);

	EPS_FATAL("not expected call for an ERTEC400 board");

	return EDD_STS_ERR_NOT_ALLOWED;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT eps_i2c_ll_read_offset_ERTEC400( 
	LSA_SYS_PTR_TYPE   sysPtr,
	LSA_UINT8          I2CDevAddr,
	LSA_UINT8          I2COffsetCnt,
	LSA_UINT8          I2COffset1,
	LSA_UINT8          I2COffset2,
	LSA_UINT32         Size,
	LSA_UINT8        * pBuf )
{
	LSA_UNUSED_ARG(sysPtr);
	LSA_UNUSED_ARG(I2CDevAddr);
	LSA_UNUSED_ARG(I2COffsetCnt);
	LSA_UNUSED_ARG(I2COffset1);
	LSA_UNUSED_ARG(I2COffset2);
	LSA_UNUSED_ARG(Size);
	LSA_UNUSED_ARG(pBuf);

	EPS_FATAL("not expected call for an ERTEC400 board");

	return EDD_STS_ERR_NOT_ALLOWED;
}

/*----------------------------------------------------------------------------*/
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
