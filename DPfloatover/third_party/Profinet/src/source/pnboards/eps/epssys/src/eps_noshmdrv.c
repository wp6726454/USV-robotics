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
/*  F i l e               &F: eps_noshmdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS No Shared Mem Interface Adaption                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20027
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>           /* Types / Prototypes / Fucns        */ 
#include <eps_trc.h>           /* Tracing                           */
#include <eps_shm_if.h>        /* Shared Mem Interface              */
#include <eps_noshmdrv.h>      /* Own Header                        */
#include <eps_rtos.h>          /* OS interface                      */
#include <eps_hif_drv_if.h>    /* HIF Driver Interface              */
#include <eps_locks.h>         /* EPS Locks                         */
#include <eps_tasks.h>         /* EPS Tasks                         */

/* - Local defines -------------------------------------------------------------------------------- */

#define EPS_NOSHMDRV_CFG_FATAL_SIZE 0x00010000
#define EPS_NOSHMDRV_CFG_PNTRC_SIZE 0x00100000

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_NOSHMDRV_PRINTF
#endif

/* - Typedefs ------------------------------------------------------------------------------------- */

typedef struct eps_noshmdrv_store_tag
{
	LSA_BOOL bInit;
	LSA_BOOL bOpen;
	LSA_BOOL bFatal;
	LSA_UINT16 hEnterExit;
	EPS_SHM_HW_TYPE sHw;
	LSA_UINT8 uFatal[EPS_NOSHMDRV_CFG_FATAL_SIZE];
	LSA_UINT8 uTrace[EPS_NOSHMDRV_CFG_PNTRC_SIZE];
	EPS_NOSHMDRV_INSTALL_ARGS_TYPE sInstallArgs;
	LSA_VOID_PTR_TYPE hSys;
	EPS_HIF_DRV_HW_PTR_TYPE pHifDrvHwInstOut;
} EPS_NOSHMDRV_STORE_TYPE, *EPS_NOSHMDRV_STORE_PTR_TYPE;

/* - Prototypes ----------------------------------------------------------------------------------- */

LSA_VOID   eps_noshmdrv_uninstall              (LSA_VOID);
LSA_UINT16 eps_noshmdrv_app_ready              (EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID* pHifHandle);
LSA_UINT16 eps_noshmdrv_app_shutdown           (EPS_SHM_HW_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_noshmdrv_sig_fatal              (EPS_SHM_HW_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal);

/* - Global Data ---------------------------------------------------------------------------------- */

EPS_NOSHMDRV_STORE_TYPE g_EpsNoShmDrv;
EPS_NOSHMDRV_STORE_PTR_TYPE g_pEpsNoShmDrv;

/* - Source --------------------------------------------------------------------------------------- */

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsNoShmDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsNoShmDrv->hEnterExit, LSA_FALSE);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsNoShmDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsNoShmDrv->hEnterExit);
}

/**
 * Opens a HIF driver for the No Shared Memory driver
 *
 * The following capabilies are requested:
 *    bUseIpcInterrupts     := LSA_FALSE
 *    bUsePnCoreInterrupts  := LSA_FALSE
 *    bUseRemoteMem         := LSA_FALSE
 *    bUseDma               := LSA_FALSE
 *    bUseShortMode         := LSA_TRUE
 *    bUseSharedMemMode     := LSA_FALSE
 *
 * Hint: The HIF Short Driver offers such capabilities.
 *
 * @see eps_hif_drv_if_open         - This function is used to find a matching HIF driver.
 * @see eps_hif_short_drv_open      - The HIF Short Driver may be called by eps_hif_drv_if_open.
 *
 * @param [in] pHwInstOut           - pointer to structure containing function pointers and memory pointers.
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_open_hif_driver(EPS_SHM_HW_PTR_TYPE pHwInstOut)
{
	LSA_UINT16 retVal;
	EPS_HIF_DRV_OPENPAR_TYPE InPars;
	EPS_HIF_DRV_CFG_TYPE sHifCfg;

	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);

	eps_memset(&InPars, 0, sizeof(InPars));

	InPars.Mode = EPS_HIF_DRV_HIF_TM_SHORT;
    InPars.if_pars.SendIrqMode = HIF_SEND_IRQ_AFTER_NO_WAIT;  // don't care shortcut mode

	eps_memset(&sHifCfg, 0, sizeof(sHifCfg));
	sHifCfg.bUseShortMode        = LSA_TRUE;

	retVal = eps_hif_drv_if_open ((LSA_UINT16)pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/, &sHifCfg, &InPars, &g_pEpsNoShmDrv->pHifDrvHwInstOut);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
}

/**
 * Closes the used HIF driver using the EPS HIF Drv IF.
 *
 * @see eps_hif_drv_if_close          - used by this function
 * @see eps_hif_short_drv_close       - The HIF Short Driver may be called by eps_hif_drv_if_close.
 *
 * @param [in] pHwInstOut             - pointer to structure containing function pointers and memory pointers.
 * @return LSA_VOID
 */
static LSA_VOID eps_noshmdrv_close_hif_driver(EPS_SHM_HW_PTR_TYPE pHwInstOut)
{
	LSA_UINT16 retVal;

	retVal = eps_hif_drv_if_close((LSA_UINT16)pHwInstOut->uHdNr, LSA_FALSE /*bUpper*/);

	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
}

/**
 * Uninstalls the EPS SHM implementation by setting bOpen to LSA_FALSE and bInit to LSA_TRUE.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_noshmdrv_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsNoShmDrv->bOpen == LSA_FALSE);
	EPS_ASSERT(g_pEpsNoShmDrv->bInit == LSA_TRUE);

	g_pEpsNoShmDrv->bInit = LSA_FALSE;

	eps_noshmdrv_undo_init_critical_section();
}

#if ( PSI_CFG_USE_PNTRC == 1 )
/**
 * Calls pTraceBufferFullCbf to empty the PNTRC buffer. The framework has to implement this function.
 * 
 * @see PNTRC_BUFFER_FULL          - calls this function
 * 
 * @param [in] hTrace              - Trace handle ID from PNTRC
 * @param [in] uBufferId           - Buffer ID from PNTRC
 * @param [in] pBuffer             - Buffer to be emptied (SRC)
 * @param [in] uLength             - Length of the Buffer to be emptied (SRC)
 * @return LSA_VOID
 */
LSA_VOID eps_noshmdrv_trace_buffer_full_cbf(LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 hBuffer, LSA_UINT8* pBuffer, LSA_UINT32 uLength)
{
	g_pEpsNoShmDrv->sInstallArgs.pTraceBufferFullCbf(g_pEpsNoShmDrv->hSys, hTrace, hBuffer, pBuffer, uLength);
}
#endif

/**
 * Does nothing but needed
 * 
 * @return EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_update_device_list(LSA_VOID)
{
    //Do nothing
    return EPS_SHM_RET_OK;
}


/**
 * Registers the EPS NO Shm Drv into the EPS SHM IF.
 * 
 * The EPS NoShm Drv is a EPS SHM implementation for the LD Lower IF. 
 * 
 * UseCases: 
 *     EPS Light
 *     PNDriver
 *     
 * Restrictions:
 *     None
 *  
 * Concept:
 *    For the NoShmDrv, a local memory is provided. There is no interprocessor communication with another CPU via shared memory.
 * 
 * HIF Integration:
 *    The EPS NoShmDrv Drv uses the EPS Short Driver as HIF Driver. This driver will set HIF to HIF_TM_SHORT. 
 *    
 * PNTRC Integration:
 *    The trace is written into the local memory.
 * 
 * @see eps_shm_if_register           - this function is used to register the EPS SHM implementation into the EPS SHM IF.
 * @see eps_hif_short_drv_install - used HIF driver
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_noshmdrv_install (EPS_NOSHMDRV_INSTALL_ARGS_PTR_TYPE pInstallArgs)
{
	EPS_SHM_IF_TYPE sNoShmDrvIf;

	g_pEpsNoShmDrv = &g_EpsNoShmDrv;

	eps_memset(g_pEpsNoShmDrv, 0, sizeof(*g_pEpsNoShmDrv));

	eps_noshmdrv_init_critical_section();
	
	g_pEpsNoShmDrv->sInstallArgs            = *pInstallArgs;

	g_pEpsNoShmDrv->sHw.AppReady            = eps_noshmdrv_app_ready;
	g_pEpsNoShmDrv->sHw.AppShutdown         = eps_noshmdrv_app_shutdown;
	g_pEpsNoShmDrv->sHw.FatalError          = eps_noshmdrv_sig_fatal;
	g_pEpsNoShmDrv->sHw.Close               = eps_noshmdrv_close;
	
#if ( PSI_CFG_USE_PNTRC == 1 )
	g_pEpsNoShmDrv->sHw.TraceBufferFull     = eps_noshmdrv_trace_buffer_full_cbf;
	g_pEpsNoShmDrv->sHw.sPntrcParam         = pInstallArgs->sPntrcParam;
#endif

	g_pEpsNoShmDrv->bInit = LSA_TRUE;

	sNoShmDrvIf.open               = eps_noshmdrv_open;
	sNoShmDrvIf.close              = eps_noshmdrv_close;
	sNoShmDrvIf.uninstall          = eps_noshmdrv_uninstall;
	sNoShmDrvIf.update_device_list = eps_noshmdrv_update_device_list;

	eps_shm_if_register(&sNoShmDrvIf);
}

/**
 * Opens the LD lower IF.
 * This functions opens the HIF short driver.
 * 
 * @see eps_noshm_open_hif_driver - used by this function to open the HIF driver.
 * @see eps_shm_if_open           - calls this function to open the LD Lower IF.
 * 
 * @param [in] hSys               - sys handle
 * @param [in] pOption            - EPS SHM decriptor (hd_nr: 0 = LD, 1...4 = HD, Debug options). 1...4 NOT SUPPORTED!!! This is a LD Lower Driver.
 * @param [out] ppHwInstOut       - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_open (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT16 retVal = EPS_SHM_RET_OK;

	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_noshmdrv_enter();

	g_pEpsNoShmDrv->hSys                = hSys;

	g_pEpsNoShmDrv->sHw.bDebug          = LSA_FALSE;

	g_pEpsNoShmDrv->sHw.sHif.pBase      = 0;
	g_pEpsNoShmDrv->sHw.sHif.uOffset    = 0;
	g_pEpsNoShmDrv->sHw.sHif.uSize      = 0;
	g_pEpsNoShmDrv->sHw.sFatal.pBase    = &g_pEpsNoShmDrv->uFatal[0];
	g_pEpsNoShmDrv->sHw.sFatal.uOffset  = 0;
	g_pEpsNoShmDrv->sHw.sFatal.uSize    = EPS_NOSHMDRV_CFG_FATAL_SIZE;
	g_pEpsNoShmDrv->sHw.sPntrc1.pBase   = &g_pEpsNoShmDrv->uTrace[0];
	g_pEpsNoShmDrv->sHw.sPntrc1.uOffset = 0;
	g_pEpsNoShmDrv->sHw.sPntrc1.uSize   = EPS_NOSHMDRV_CFG_PNTRC_SIZE;
	g_pEpsNoShmDrv->sHw.sPntrc2.pBase   = 0;
	g_pEpsNoShmDrv->sHw.sPntrc2.uOffset = 0;
	g_pEpsNoShmDrv->sHw.sPntrc2.uSize   = 0;

	if(pOption != LSA_NULL)
	{
		g_pEpsNoShmDrv->sHw.uHdNr = pOption->uHdNr;
	}

	g_pEpsNoShmDrv->bOpen = LSA_TRUE;

	eps_noshmdrv_open_hif_driver(&g_pEpsNoShmDrv->sHw);

	*ppHwInstOut = &g_pEpsNoShmDrv->sHw;
	
	eps_noshmdrv_exit();

	return retVal;
}

/**
 * Closes the EPS SHM implementation
 * 
 * @see eps_shm_if_close            - calls this function
 * @see eps_noshm_close_hif_driver  - used by this function to close the HIF driver instance
 * @see eps_hif_short_drv_close     - may be used by this function
 * 
 * @param [in] pHwInstIn            - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_close (EPS_SHM_HW_PTR_TYPE pHwInstIn)
{
	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	eps_noshmdrv_enter();

	EPS_ASSERT(g_pEpsNoShmDrv->bOpen == LSA_TRUE);

	eps_noshmdrv_close_hif_driver(pHwInstIn);

	g_pEpsNoShmDrv->bOpen = LSA_FALSE;

	eps_noshmdrv_exit();

	return EPS_SHM_RET_OK;
}

/**
 * Calls pAppReadyCbf. The framework has to provide this function. 
 * 
 * @see eps_startup_done - calls this function
 * 
 * @param [in] pHwInstIn            - pointer to structure containing function pointers and memory pointers.
 * @param [in] pHifHandle           - handle for HIF
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_app_ready (EPS_SHM_HW_PTR_TYPE pHwInstIn, LSA_VOID* pHifHandle)
{
	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	eps_noshmdrv_enter();

	EPS_ASSERT(g_pEpsNoShmDrv->bOpen == LSA_TRUE);

	if(g_pEpsNoShmDrv->sInstallArgs.pAppReadyCbf != LSA_NULL)
	{
		g_pEpsNoShmDrv->sInstallArgs.pAppReadyCbf(pHwInstIn, pHifHandle);
	}

	eps_noshmdrv_exit();

	return EPS_SHM_RET_OK;
}

/**
 * Calls pAppShutdownCbf. The framework has to provide this function.
 * 
 * @see eps_undo_init     - calls this function
 * 
 * @param [in] pHwInstIn  - pointer to structure containing function pointers and memory pointers.
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_app_shutdown (EPS_SHM_HW_PTR_TYPE pHwInstIn)
{
	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	eps_noshmdrv_enter();

	EPS_ASSERT(g_pEpsNoShmDrv->bOpen == LSA_TRUE);

	/* AP01517865 */
	if(g_pEpsNoShmDrv->sInstallArgs.pAppShutdownCbf != LSA_NULL)
	{
		g_pEpsNoShmDrv->sInstallArgs.pAppShutdownCbf(pHwInstIn);
	}

	eps_noshmdrv_exit();

	return EPS_SHM_RET_OK;
}

/**
 * FATAL Handling implementation of the EPS NOSHM driver.
 * 
 * Some error info is printed on the COM-Port. This may take some time. 
 * 
 * @see eps_lsa_fatal     - calls this function to signal a FATAL caused by PSI.
 * @see eps_fatal         - calls this function to signal a FATAL caused by EPS.
 * 
 * @param [in] pHwInstIn  - pointer to structure containing function pointers and memory pointers.
 * @param [in] sFatal     - structure with FATAL info
 * @return #EPS_SHM_RET_OK
 */
LSA_UINT16 eps_noshmdrv_sig_fatal (EPS_SHM_HW_PTR_TYPE pHwInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal)
{
	EPS_ASSERT(g_pEpsNoShmDrv != LSA_NULL);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	g_pEpsNoShmDrv->bFatal = LSA_TRUE;
	g_pEpsNoShmDrv->bOpen  = LSA_FALSE;

	#ifdef EPS_NOSHMDRV_PRINTF
	/*
	 * Blocking function calls, like printf or sleep are not permitted while holding a spinlock!
	 * Check before printing error information
	 */
	if(g_pEpsData->bBlockingPermitted == LSA_TRUE)
	{
		printf("\r\n\r\n");
	
		if(sFatal.uErrCode == EPS_SHM_ERR_CODE_FATAL)
		{
			printf("!!!EPS FATAL!!! => File(%s) Function(%s) Line(%d) Msg(%s)", sFatal.pFile, sFatal.pFunc, (int)sFatal.uLine, sFatal.pMsg);
		}
	
		if(sFatal.uErrCode == EPS_SHM_ERR_CODE_FATAL_LSA)
		{
			printf("!!!EPS LSA FATAL!!! => Comp(%s) CompId(%d) ModuleId(%d) Line(%d) ErrCode(%d)",sFatal.pComp, (int)sFatal.pLsaFatal->lsa_component_id, (int)sFatal.pLsaFatal->module_id, (int)sFatal.pLsaFatal->line, (int)sFatal.pLsaFatal->error_code);
		}
	
		printf("\r\n\r\n");
		eps_tasks_sleep(20); /* Give it some time to print out */
	}
	#else
	LSA_UNUSED_ARG(sFatal);
	#endif

	return EPS_SHM_RET_OK;
}
