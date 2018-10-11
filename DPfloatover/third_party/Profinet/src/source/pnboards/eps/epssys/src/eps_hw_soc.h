#ifndef EPS_HW_SOC1_H_
#define EPS_HW_SOC1_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_hw_soc.h                              :F&  */
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

#if ((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_SOC)))
/*---------------------------------------------------------------------------*/

LSA_VOID   eps_init_hw_SOC ( LSA_UINT16 boardNr );
LSA_VOID   eps_undo_init_hw_SOC ( LSA_UINT16 boardNr );
LSA_VOID   eps_soc_FillNrtMemParams ( LSA_VOID * pBoardInfoVoid );
LSA_RESULT eps_set_pllport_SOC ( LSA_UINT16 board_nr, LSA_UINT32 irte_ptr, LSA_UINT32 location, LSA_UINT16 pllmode, LSA_UINT16 gpio );
LSA_VOID   eps_pof_set_led_SOC ( LSA_UINT16 portID, LSA_BOOL on, LSA_SYS_PTR_TYPE sysPtr );
LSA_VOID   eps_i2c_set_scl_low_high_SOC ( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_VOID   eps_i2c_set_sda_low_high_SOC ( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_UINT8  eps_i2c_sda_read_SOC ( LSA_SYS_PTR_TYPE sysPtr );
LSA_VOID   eps_i2c_set_sda_in_out_SOC ( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_VOID   eps_i2c_set_scl_in_out_SOC ( LSA_SYS_PTR_TYPE sysPtr, LSA_BOOL outPut );
LSA_BOOL   eps_i2c_select_SOC ( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT16 PortId, LSA_UINT16 I2CMuxSelect );
LSA_RESULT eps_i2c_ll_write_offset_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT8 I2CDevAddr, LSA_UINT8 I2COffsetCnt, LSA_UINT8 I2COffset1, LSA_UINT8  I2COffset2, LSA_UINT32 Size, LSA_UINT8  *pBuf );
LSA_RESULT eps_i2c_ll_read_offset_SOC( LSA_SYS_PTR_TYPE sysPtr, LSA_UINT8 I2CDevAddr, LSA_UINT8 I2COffsetCnt, LSA_UINT8 I2COffset1, LSA_UINT8 I2COffset2, LSA_UINT32 Size, LSA_UINT8 *pBuf );
LSA_VOID   eps_FillNrtMemParams ( EPS_BOARD_INFO_TYPE * pboardInfo);

/*----------------------------------------------------------------------------*/
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HW_SOC1_H_ */
