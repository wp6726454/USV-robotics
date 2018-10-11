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
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: eps_plf_windows.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Platform Adaption                                                    */
/*                                                                           */
/*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>  // struct timeval

#include <psi_inc.h>
#include <eps_lib.h>           /* File*,...                                */
#include <eps_sys.h>           /* Types / Prototypes / Fucns               */
#include <eps_trc.h>           /* Tracing                                  */
#include <pntrc_inc.h>         /* Tracing                                  */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* Pn Device Drv Interface                  */
#include <eps_pn_imcea_drv.h>  /* Pn Device Drv Interface                  */
#include <eps_pndevdrv.h>      /* PnDev Driver Support                     */
#include <eps_wpcapdrv.h>      /* WPCAP Support                            */
#include <eps_shm_if.h>        /* Shared Mem Drv Interface                 */
#include <eps_hif_drv_if.h>    /* HIF Drv Interface                        */
#include <eps_noshmdrv.h>      /* Shortcut Drv                             */
#include <eps_hif_short_drv.h> /* HIF Shortcut Drv                         */
#include <eps_plf.h>           /* EPS PLF prototypes                       */
#include <eps_rtos.h>          /* RTOS Interface                           */
#include <eps_timer.h>         /* EPS Timers                               */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_hif_universal_drv.h> /* EPS HIF Driver Implementation         */


/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_REGISTER_SHM_IF( LSA_VOID* pShmIf )
{
	// note: not used on pcIOX NIF
	LSA_UNUSED_ARG(pShmIf);
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_ENABLE_INTERRUPTS(LSA_VOID)
{
	// note: on windows level no enable IR is possible
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_PLF_DISABLE_INTERRUPTS(LSA_VOID)
{
	// note: on windows level no disable IR is possible
}

/*----------------------------------------------------------------------------*/

long EPS_PLF_EXCHANGE_LONG( long volatile *pAddr, long lVal)
{
	return( InterlockedExchange( pAddr, lVal) );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
