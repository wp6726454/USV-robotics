#ifndef EPS_HW_ERTEC400_H_          /* ----- reinclude-protection ----- */
#define EPS_HW_ERTEC400_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_hw_ertec400.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

// "EDDI_CFG_ERTEC_400" is only defined by "EPS_PLF_ERTEC400_ARM9", so the (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
#if ( ((PSI_CFG_USE_EDDI == 1) && (defined (EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
/*---------------------------------------------------------------------------*/

LSA_VOID    eps_init_hw_ertec400( LSA_UINT16 boardNr );
LSA_VOID    eps_undo_init_hw_ertec400( LSA_UINT16 boardNr );
LSA_RESULT  eps_set_pllport_ERTEC400( LSA_UINT16 board_nr, LSA_UINT32 irte_ptr, LSA_UINT32 location, LSA_UINT16 pllmode, LSA_UINT16 gpio );
LSA_VOID    eps_pof_set_led_ERTEC400( LSA_UINT16 portID, LSA_BOOL on, LSA_SYS_PTR_TYPE sysPtr );
LSA_VOID    eps_i2c_set_scl_low_high_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL level );
LSA_VOID    eps_i2c_set_sda_low_high_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL level );
LSA_UINT8   eps_i2c_sda_read_ERTEC400( LSA_SYS_PTR_TYPE sysPtr );
LSA_VOID    eps_i2c_set_sda_in_out_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_VOID    eps_i2c_set_scl_in_out_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_BOOL    eps_i2c_select_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT16 PortId, LSA_UINT16 I2CMuxSelect );
LSA_RESULT  eps_i2c_ll_write_offset_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT8 I2CDevAddr, LSA_UINT8 I2COffsetCnt, LSA_UINT8 I2COffset1, LSA_UINT8  I2COffset2, LSA_UINT32 Size, LSA_UINT8  * pBuf );
LSA_RESULT  eps_i2c_ll_read_offset_ERTEC400( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT8 I2CDevAddr, LSA_UINT8 I2COffsetCnt, LSA_UINT8 I2COffset1, LSA_UINT8 I2COffset2, LSA_UINT32 Size, LSA_UINT8 * pBuf );
LSA_VOID    eps_ertec400_FillNrtMemParams( LSA_VOID * pboardInfoVoid );
/*----------------------------------------------------------------------------*/
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HW_ERTEC400_H_ */
