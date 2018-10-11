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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_edd.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDD HW system adaption                             */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      25408
#define LTRC_ACT_MODUL_ID  25408

#include "eps_sys.h"
#include "psi_inc.h"
#include "psi_trc.h"

#if ( ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDS == 1 ) )
/*----------------------------------------------------------------------------*/

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

LSA_VOID PSI_EDD_NRT_SEND_HOOK(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID_PTR_TYPE  ptr_upper_rqb )
{
	/* ptr_upper_rqb is of type EDD_UPPER_RQB_PTR_TYPE */

	LSA_UNUSED_ARG(hSysDev);
	LSA_UNUSED_ARG(ptr_upper_rqb);

	/* Send hook not used in EPS */
}

/*----------------------------------------------------------------------------*/
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
