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
/*  F i l e               &F: eps_hw_pnip.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNIP adpation                                                        */
/*                                                                           */
/*****************************************************************************/

#define EPS_MODULE_ID      20021
#define LTRC_ACT_MODUL_ID  20021

#include <eps_sys.h>          /* Types / Prototypes / Fucns */

#if ( PSI_CFG_USE_EDDP == 1 ) 
/*---------------------------------------------------------------------------*/

#include <eps_trc.h>          /* Tracing                                  */
#include <eps_register.h>     /* Register access macros                   */
#include <pnip_xrsl.h>        /* PNIP Register definitions                */
#include <eps_hw_pnip.h>      /* EPS HW Ertec200p adaption                */

/* Local defines are needed as long as there no global definitions of those vals */
#define	ERTEC200P_GPIO_REG__IOCTRL_0		          0x00
#define	ERTEC200P_GPIO_REG__PORT_MODE_0_L	          0x18
#define ERTEC200P_GPIO_REG_OUT_0                      0x04
#define ERTEC200P_GPIO_REG_OUT_SET_0                  0x08
#define ERTEC200P_GPIO_REG_OUT_CLEAR_0                0x0C
#define ERTEC200P_GPIO_REG_IN_0                       0x14

#define PNIP_REG_PLL_OUT_CONTROL_4              0x00032010
#define PNIP_REG_IRQ_CONTROL_4                  0x00032110
#define PNIP_REG_IRQ2CONTROL_MUX2               0x00042300
#define PNIP_REG_IRQ3CONTROL_MUX3               0x00043304

/* NRT / CRT Ram Sizes */
#define SDRAM_NRT_SIZE_RECOMMENDED              0x00200000   // 2048kB
#define SDRAM_CRT_SIZE_RECOMMENDED              0x00080000   // 512kB
#define SDRAM_K32_SIZE_RECOMMENDED                       0   // 0kB
#define SDRAM_SIZE_RECOMMENDED (SDRAM_NRT_SIZE_RECOMMENDED + SDRAM_CRT_SIZE_RECOMMENDED + SDRAM_K32_SIZE_RECOMMENDED)

/* GPIO */
#define EPS_HW_E200P_GPIO_INIT(base, gpio_num_0_31)    eps_e200p_reg_wr_32(base + ERTEC200P_GPIO_REG__IOCTRL_0, eps_e200p_reg_r_32(base + ERTEC200P_GPIO_REG__IOCTRL_0) & ~(1<<(gpio_num_0_31)))
#define EPS_HW_E200P_GPIO_INIT_IN(base, gpio_num_0_31) eps_e200p_reg_wr_32(base + ERTEC200P_GPIO_REG__IOCTRL_0, eps_e200p_reg_r_32(base + ERTEC200P_GPIO_REG__IOCTRL_0) | (1<<(gpio_num_0_31)))
#define EPS_HW_E200P_GPIO_SET(base, gpio_num)          eps_e200p_reg_wr_32(base + ERTEC200P_GPIO_REG_OUT_SET_0,   1<<(gpio_num))
#define EPS_HW_E200P_GPIO_CLEAR(base, gpio_num)        eps_e200p_reg_wr_32(base + ERTEC200P_GPIO_REG_OUT_CLEAR_0, 1<<(gpio_num))
#define EPS_HW_E200P_GPIO_TOGGLE(base, gpio_num)       eps_e200p_reg_wr_32(base + ERTEC200P_GPIO_REG_OUT_0, (eps_e200p_reg_r_32(base + ERTEC200P_GPIO_REG_OUT_0)^(1<<(gpio_num))))
#define EPS_HW_E200P_GPIO_GET(base, gpio_num)          (eps_e200p_reg_r_32(base + ERTEC200P_GPIO_REG_IN_0) & (1<<(gpio_num)))

#define EPS_HW_E200P_GPIO_MAP_DEBUG_1                 1
#define EPS_HW_E200P_GPIO_MAP_DEBUG_2                 6

static LSA_VOID eps_e200p_reg_wr_32(LSA_UINT32 address, LSA_UINT32 val)
{
	EPS_REG32_WRITE(address, val);
}

static LSA_UINT32 eps_e200p_reg_r_32(LSA_UINT32 address)
{
	return EPS_REG32_READ(address);
}

LSA_VOID eps_hw_e200p_init( LSA_UINT8 * pApbPerif )
{
	LSA_UINT32 uGpioBase = (LSA_UINT32)(pApbPerif + 0x18000);

    EPS_ASSERT(pApbPerif != 0);

	/* Set GPIO 0 to output for PLL0 Out (Can be used on EB200P as well as FPGA) */
	/*     Set GPIO to Output */
	eps_e200p_reg_wr_32( (uGpioBase + ERTEC200P_GPIO_REG__IOCTRL_0), eps_e200p_reg_r_32(uGpioBase + ERTEC200P_GPIO_REG__IOCTRL_0) & ~1);
	/*     Configure alternate function A */
	eps_e200p_reg_wr_32( (uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L), (eps_e200p_reg_r_32(uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L) & (~0xC)) | 0x1);

	/* always enable GPIOs for I2C for auto-POF-detection */
	/* set GPIO 4...7 to alternate function C (I2C) */
	eps_e200p_reg_wr_32( (uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L), (eps_e200p_reg_r_32(uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L) ) | 0xFF00UL);
}

LSA_VOID eps_hw_e200p_enable_isochronous_interrupt( LSA_UINT8* pPnip, LSA_UINT8* pApbPerif, LSA_BOOL bPnipIsIntern )
{
	LSA_UINT32 uGpioBase = (LSA_UINT32)(pApbPerif + 0x18000);
	LSA_UINT32 uPnipBase = (LSA_UINT32)(pPnip);

	EPS_ASSERT(pPnip != 0);
	EPS_ASSERT(pApbPerif != 0);

	/* Set GPIO 4 to output for PLL4 Out (For debug of isochronous mode) */
	/*     Set GPIO to Output */
	eps_e200p_reg_wr_32( (uGpioBase + ERTEC200P_GPIO_REG__IOCTRL_0), eps_e200p_reg_r_32(uGpioBase + ERTEC200P_GPIO_REG__IOCTRL_0) & ~10);
	/*     Configure alternate function A */
	eps_e200p_reg_wr_32( (uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L), (eps_e200p_reg_r_32(uGpioBase + ERTEC200P_GPIO_REG__PORT_MODE_0_L) & (~0x300)) | 0x100);

	/* Setup PLL4_INT of PN-IP */
	eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_IRQ_CONTROL_4)    ,          8 ); /* Connect PLL4 with ApplTimer2 >= ApplCompare_2_1 */
	eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_PLL_OUT_CONTROL_4), 0x00FE0008 ); /* Connect PLL4_OUT with ApplTimer2 >= ApplCompare_2_1 */

	if (bPnipIsIntern)
	{
		/* Internal use ICU 2 */
		eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_IRQ2CONTROL_MUX3) , 71 );  /* Connect PN-IP IRQ3 with Event 71 (PLL4) */
	}
	else
	{
		/* External use ICU 3 */
		eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_IRQ3CONTROL_MUX3) , 71 );  /* Connect PN-IP IRQ3 with Event 71 (PLL4) */

		/* Perif ICU Setup is done via PnDevDrv */
	}
}

LSA_VOID eps_hw_e200p_disable_isochronous_interrupt( LSA_UINT8* pPnip, LSA_BOOL bPnipIsIntern )
{
	LSA_UINT32 uPnipBase = (LSA_UINT32)(pPnip);

	EPS_ASSERT(pPnip != 0);

	if (bPnipIsIntern)
	{
		/* Internal use ICU 2 */
		eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_IRQ2CONTROL_MUX3) , 86 );  /* Connect PN-IP IRQ3 with No Event */
	}
	else
	{
		/* External use ICU 3 */
		eps_e200p_reg_wr_32( (uPnipBase + PNIP_REG_IRQ3CONTROL_MUX3) , 86 );  /* Connect PN-IP IRQ3 with No Event */

		/* Perif ICU Setup is done via PnDevDrv */
	}
}

LSA_VOID eps_hw_e200p_ack_isochronous_interrupt( LSA_UINT8* pPnip, LSA_BOOL bPnipIsIntern )
{
	//reset EOI IRQ3Control_MUX3
	if (!bPnipIsIntern)
	{
		eps_e200p_reg_wr_32((LSA_UINT32)pPnip + PNIP_REG_IRQ3CONTROL_MUX3, eps_e200p_reg_r_32((LSA_UINT32)pPnip + PNIP_REG_IRQ3CONTROL_MUX3) & 0xffff);
	}
}
	
LSA_VOID eps_hw_e200p_calculateSizeInSDRAM( LSA_UINT32 size, LSA_UINT32* sizeNRT, LSA_UINT32* sizeCRT )
{
	// calculate SDRAM mem ranges for CRT and NRT area based on avaliable SDRAM

	EPS_ASSERT( sizeNRT != LSA_NULL );
	EPS_ASSERT( sizeCRT != LSA_NULL );
	EPS_ASSERT( 0 == (size % 4) );  // 4 byte alligned ?

	// check for required ram size
	if ( SDRAM_SIZE_RECOMMENDED <= size )  // SDRAM size allows full range ?
	{
		*sizeNRT = SDRAM_NRT_SIZE_RECOMMENDED;
		*sizeCRT = SDRAM_CRT_SIZE_RECOMMENDED;
    }
	else
	{
		if ( SDRAM_K32_SIZE_RECOMMENDED != 0 ) // use K32 DLL
        {
			// set the ranges with appended K32 memory
			EPS_ASSERT( SDRAM_K32_SIZE_RECOMMENDED < (size / 4) );

			*sizeCRT = ( size / 4 ) - SDRAM_K32_SIZE_RECOMMENDED;
			*sizeNRT = size - *sizeCRT - SDRAM_K32_SIZE_RECOMMENDED;
		}
		else
		{
			// set the ranges without K32 memory
			*sizeCRT = ( size / 4 ); // reserve 25% for CRT
			*sizeNRT = size - *sizeCRT;
		}
	}

	EPS_ASSERT( size >= *sizeNRT + *sizeCRT );
}

LSA_VOID eps_hw_e200p_set_gpio(LSA_UINT8* pApbPerif, EPS_HW_E200P_GPIO_DESC_TYPE eGpio)
{
	LSA_UINT32 uGpioBase = (LSA_UINT32)(pApbPerif + 0x18000);

	switch (eGpio)
	{
		case EPS_HW_E200P_DEBUG_GPIO_1:
			EPS_HW_E200P_GPIO_SET(uGpioBase, EPS_HW_E200P_GPIO_MAP_DEBUG_1);
			break;
		case EPS_HW_E200P_DEBUG_GPIO_2:
			EPS_HW_E200P_GPIO_SET(uGpioBase, EPS_HW_E200P_GPIO_MAP_DEBUG_2);
			break;
	}
}

LSA_VOID eps_hw_e200p_clear_gpio(LSA_UINT8* pApbPerif, EPS_HW_E200P_GPIO_DESC_TYPE eGpio)
{
	LSA_UINT32 uGpioBase = (LSA_UINT32)(pApbPerif + 0x18000);

	switch (eGpio)
	{
		case EPS_HW_E200P_DEBUG_GPIO_1:
			EPS_HW_E200P_GPIO_CLEAR(uGpioBase, EPS_HW_E200P_GPIO_MAP_DEBUG_1);
			break;
		case EPS_HW_E200P_DEBUG_GPIO_2:
			EPS_HW_E200P_GPIO_CLEAR(uGpioBase, EPS_HW_E200P_GPIO_MAP_DEBUG_2);
			break;
	}
}

/*----------------------------------------------------------------------------*/
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
