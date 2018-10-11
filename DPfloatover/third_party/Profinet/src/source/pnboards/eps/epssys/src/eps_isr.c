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
/*  F i l e               &F: eps_isr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Interrupt adaption                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20007
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "psi_usr.h"
#include "hif_inc.h"
#include "eps_trc.h"

/* EPS Driver If includes */
#include "eps_pn_drv_if.h" /* Pn Device Driver Interface */
#include "eps_shm_if.h"    /* Shared Memory Interface    */

EPS_SHM_HW_PTR_TYPE g_pEpsIsrShmHw = LSA_NULL;

LSA_VOID eps_isr_init(LSA_VOID* pShmHw)
{
	/* Init pointer to shared memory interface */
	g_pEpsIsrShmHw = (EPS_SHM_HW_PTR_TYPE)pShmHw; 
}

/* General PN Gathered Interrupt Callback Function */
LSA_VOID eps_pn_interrupt(LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)pArgs;
	LSA_UINT32 uIntSrc = uParam;

	/* Call psi interrupt for hd_id with interrupt source */
	psi_hd_interrupt(pSys->hd_nr, uIntSrc);
}

/* General EDDS Interrupt Poll Callback Function */
#if ( PSI_CFG_USE_EDDS )
LSA_VOID eps_pn_interrupt_edds_poll(LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)pArgs;
	
	LSA_UNUSED_ARG(uParam);

	/* Call psi interrupt for hd_id with interrupt source */
	psi_hd_isr_poll(pSys->hd_nr);
}
#endif

/* General EDDI Exttimer Callback Function */
LSA_VOID eps_eddi_exttimer_interrupt(LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)pArgs;

	LSA_UNUSED_ARG(uParam);

	/* Call psi interrupt for hd_id with interrupt source */
	psi_eddi_exttimer_interrupt(pSys->hd_nr);
}

/**
 * Starts the Exttimer Mode
 * 
 * A timer is started, calling a call-back-function, when it expired.
 * 
 * @param hSysDev
 */
LSA_VOID eps_eddi_exttimer_start(PSI_SYS_HANDLE hSysDev)
{
#ifdef EDDI_CFG_SII_EXTTIMER_MODE_ON
	EPS_PNDEV_CALLBACK_TYPE Cbf;
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDI);
	EPS_ASSERT(pBoard->TimerCtrlStart != LSA_NULL);

	Cbf.pArgs  = (LSA_VOID*)pSys;
	Cbf.uParam = 0;
	Cbf.pCbf   = eps_eddi_exttimer_interrupt;
	retVal = pBoard->TimerCtrlStart(pBoard, &Cbf);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
#else
	LSA_UNUSED_ARG(hSysDev);
	EPS_FATAL("Exttimer Mode not supported in this configuration.");
#endif
}

/**
 * Stopping the Exttimer Mode
 *  
 * @param hSysDev
 */
LSA_VOID eps_eddi_exttimer_stop(PSI_SYS_HANDLE hSysDev)
{
#ifdef EDDI_CFG_SII_EXTTIMER_MODE_ON
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->TimerCtrlStop != LSA_NULL);

	retVal = pBoard->TimerCtrlStop(pBoard);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
#else
	LSA_UNUSED_ARG(hSysDev);
	EPS_FATAL("Exttimer Mode not supported in this configuration.");
#endif
}

/**
 * Enables Interrupt for specified board.
 * 
 * For a HD (pSys->hd_nr) an interrupt is enabled via Systemadaption. 
 * 
 * @param hSysDev
 * @return
 */
LSA_VOID eps_enable_pnio_ir(PSI_SYS_HANDLE hSysDev)
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	EPS_PNDEV_CALLBACK_TYPE Cbf;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

	if(pSys->edd_comp_id != LSA_COMP_ID_EDDS)
	{
	Cbf.pArgs  = (LSA_VOID*)pSys;
	Cbf.uParam = 0;
	Cbf.pCbf   = eps_pn_interrupt;
	IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
	retVal = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
	}
	else
	{
		#if ( PSI_CFG_USE_EDDS )
		Cbf.pArgs  = (LSA_VOID*)pSys;
		Cbf.uParam = 0;
		Cbf.pCbf   = eps_pn_interrupt_edds_poll;
		IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
		retVal = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
		EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
		#else
		EPS_FATAL("EDDS expected but not configurated for Interrupts.");
		#endif
	}

	if(pBoard->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
	{
		Cbf.pArgs  = (LSA_VOID*)pSys;
		Cbf.uParam = 2;
		Cbf.pCbf   = eps_pn_interrupt;
		IsrDesc    = EPS_PNDEV_ISR_PN_NRT;
		retVal = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
		EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
	}
}

LSA_VOID eps_disable_pnio_ir(PSI_SYS_HANDLE hSysDev)
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

	IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
	retVal = pBoard->DisableIsr(pBoard, &IsrDesc);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);

	if(pBoard->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
	{
		IsrDesc = EPS_PNDEV_ISR_PN_NRT;
		retVal = pBoard->DisableIsr(pBoard, &IsrDesc);
		EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
	}
}

LSA_VOID eps_enable_iso_interrupt(PSI_SYS_HANDLE hSysDev, void (*func_ptr)(int param, void* args), int param, void* args)
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	EPS_PNDEV_CALLBACK_TYPE Cbf;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

	Cbf.pArgs  = args;
	Cbf.uParam = param;
	Cbf.pCbf   = (EPS_PNDEV_ISR_CBF)func_ptr;
	IsrDesc    = EPS_PNDEV_ISR_ISOCHRONOUS;
	retVal = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
}

LSA_VOID eps_disable_iso_interrupt(PSI_SYS_HANDLE hSysDev)
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
	LSA_UINT16 retVal;
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;

	EPS_ASSERT( hSysDev != 0 );

	eps_pndev_if_get(pSys->hd_nr, &pBoard);
	EPS_ASSERT(pBoard != LSA_NULL);
	EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

	IsrDesc = EPS_PNDEV_ISR_ISOCHRONOUS;
	retVal = pBoard->DisableIsr(pBoard, &IsrDesc);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
}
