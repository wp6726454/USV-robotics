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
/*  F i l e               &F: pnd_plf_linux.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDriver Platform Adaption                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20051
#define PND_MODULE_ID      20051

#include <eps_sys.h>           /* Types / Prototypes / Fucns               */
#include <eps_trc.h>           /* Tracing                                  */
#include <pntrc_inc.h>         /* Tracing                                  */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* Pn Device Drv Interface                  */
#include <eps_mem.h>           /*  EPS Local Memory                        */
#include <eps_tasks.h>
#include <eps_shm_if.h>        /* Shared Mem Drv Interface                 */
#include <eps_hif_drv_if.h>    /* HIF Drv Interface                        */
#include <eps_noshmdrv.h>      /* Shortcut Drv                             */
#include <eps_hif_short_drv.h> /* HIF Shortcut Drv                         */
#include <eps_plf.h>           /* Own Header                               */
#include <eps_rtos.h>          /* RTOS Interface                           */
#include <eps_timer.h>         /* EPS Timers                               */
#include <eps_locks.h>         /* EPS Locks                                */
#include <servusrx.h>          /* DebugSetting                             */


LSA_VOID EPS_PLF_REGISTER_SHM_IF(LSA_VOID* pShmIf)
{/* function is empty for PN Driver */
    LSA_UNUSED_ARG(pShmIf);
}

LSA_VOID EPS_PLF_DISABLE_INTERRUPTS(LSA_VOID)
{
    EPS_FATAL(0); /* Not Supported */
}

LSA_VOID EPS_PLF_ENABLE_INTERRUPTS(LSA_VOID)
{
    EPS_FATAL(0); /* Not Supported */
}

long EPS_PLF_EXCHANGE_LONG(long volatile *pAddr, long lVal)
{
    return ( __sync_lock_test_and_set(pAddr, lVal) );
}

//- PCI Implementation -------------------------------------------------------------------------------

LSA_VOID EPS_PLF_PCI_GET_DEVICES(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId, EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpaceOut, LSA_UINT16 uMaxDevice, LSA_UINT16 *pFoundDevices)
{
    /* Not used with WPCAP. Need to be implemented when using Intel NIC driver (eps_stdmacdrv_install()) */

    LSA_UNUSED_ARG(uVendorId);
    LSA_UNUSED_ARG(uDeviceId);
    LSA_UNUSED_ARG(pConfigSpaceOut);
    LSA_UNUSED_ARG(uMaxDevice);
    LSA_UNUSED_ARG(pFoundDevices);

    EPS_FATAL(0); 
}

LSA_VOID EPS_PLF_PCI_ENA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace, EPS_PLF_PCI_ISR_CBF_PTR_TYPE pCbf)
{
    /* Not used with WPCAP. Need to be implemented when using Intel NIC driver (eps_stdmacdrv_install()) */

    LSA_UNUSED_ARG(pConfigSpace);
    LSA_UNUSED_ARG(pCbf);

    EPS_FATAL(0);
}

LSA_VOID EPS_PLF_PCI_DIA_INTERRUPT(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pConfigSpace)
{
    /* Not used with WPCAP. Need to be implemented when using Intel NIC driver (eps_stdmacdrv_install()) */

    LSA_UNUSED_ARG(pConfigSpace);

    EPS_FATAL(0);
}

LSA_VOID EPS_PLF_PCI_READ_DOUBLE_WORD(LSA_UINT32 uBusNum, LSA_UINT32 uDeviceNum, LSA_UINT32 uFuncNum, LSA_UINT32 uOffset, LSA_UINT32* uVal)
{
    /* Not used with WPCAP. Need to be implemented when using Intel NIC driver (eps_stdmacdrv_install()) */

    LSA_UNUSED_ARG(uBusNum);
    LSA_UNUSED_ARG(uDeviceNum);
    LSA_UNUSED_ARG(uFuncNum);
    LSA_UNUSED_ARG(uOffset);
    LSA_UNUSED_ARG(uVal);

    EPS_FATAL(0);
}

LSA_BOOL EPS_PLF_PCI_MAP_MEMORY(LSA_UINT8** pBase, LSA_UINT32 uBase, LSA_UINT32 uSize)
{
    /* Not used with WPCAP. Need to be implemented when using Intel NIC driver (eps_stdmacdrv_install()) */

    LSA_UNUSED_ARG(pBase);
    LSA_UNUSED_ARG(uBase);
    LSA_UNUSED_ARG(uSize);

    EPS_FATAL(0);

    return LSA_TRUE;
}
