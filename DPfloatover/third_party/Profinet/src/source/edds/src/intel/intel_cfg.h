
#ifndef INTEL_CFG_H                      /* ----- reinclude-protection ----- */
#define INTEL_CFG_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: INTEL_CFG.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n: Global-Config-file for INTEL Low Level          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  25.05.11    AM    initial version.                                       */
#endif
/*****************************************************************************/

#define INTEL_MAX_TX_DESCRIPTORS	32768	/* maximum allowed value in TDLEN := 0x80000 == 32K Descriptors */
#define INTEL_MAX_RX_DESCRIPTORS	32768	/* maximum allowed value in RDLEN := 0x80000 == 32K Descriptors */

/* throttling intervall can be between 0 and  16776960ns (with a granularity of 256);
 * From "Driver Programing Interface—82574 GbE Controller" Manual:
 * [..]The optimal performance setting for this register is very system and configuration
 * specific. An initial suggested range is should be between 166656 and 1428480 Nanoseconds.[..]
 */
#define INTEL_CFG_INTERRUPT_THROTTLING_NANOSECONDS      0

#endif  /* INTEL_CFG_H_ */
