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
/*  F i l e               &F: eps_hif_short_drv.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF Short Drv                                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20057
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>               /* Types / Prototypes / Fucns */
#include <eps_trc.h>               /* Tracing                    */
#include <eps_locks.h>             /* EPS Locks                  */
#include <eps_hif_drv_if.h>        /* HIF-Driver Interface       */
#include <eps_hif_short_drv.h>     /* Own Header                 */

#define EPS_HIF_SHORT_DRV_MAX_INSTANCES 10

LSA_VOID   eps_hif_short_drv_uninstall       (LSA_VOID);
LSA_UINT16 eps_hif_short_drv_open            (struct eps_hif_drv_openpar_tag* pInPars, EPS_HIF_DRV_HW_PTR_TYPE* ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_short_drv_close           (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_short_drv_enable_isr      (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_UINT16 hHif);
LSA_UINT16 eps_hif_short_drv_disable_isr     (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_short_drv_send_hif_ir     (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_short_drv_ack_hif_ir      (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_short_drv_reenable_hif_ir (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn);
LSA_VOID   eps_hif_short_drv_dmacpy          (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_VOID** ppData, LSA_UINT32 uLength);
LSA_VOID   eps_hif_short_drv_mem_pool_create (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_VOID* pBase, LSA_UINT32 uLength);
LSA_VOID   eps_hif_short_drv_mem_pool_free   (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_UINT16 *pRetVal);
LSA_VOID   eps_hif_short_drv_alloc           (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_VOID_PTR_TYPE *ppMem, LSA_UINT32 uLength);
LSA_VOID   eps_hif_short_drv_free            (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_UINT16 *pRetVal, LSA_VOID_PTR_TYPE pMem);
LSA_VOID   eps_hif_short_drv_get_params      (EPS_HIF_DRV_HW_PTR_TYPE pShmInstIn, LSA_VOID* pPar);

typedef struct
{
	LSA_BOOL   bUsed;
	LSA_UINT32 hOwnHandle;
	EPS_HIF_DRV_OPENPAR_TYPE InPar;
	EPS_HIF_DRV_HW_TYPE sHw;
} EPS_HIF_SHORT_DRV_INST_TYPE, *EPS_HIF_SHORT_DRV_INST_PTR_TYPE;

typedef struct
{
	LSA_UINT16 hEnterExit;
	LSA_BOOL   bInit;
	EPS_HIF_SHORT_DRV_INST_TYPE Inst[EPS_HIF_SHORT_DRV_MAX_INSTANCES];
} EPS_HIF_SHORT_DRV_TYPE, *EPS_HIF_SHORT_DRV_PTR_TYPE;

EPS_HIF_SHORT_DRV_TYPE g_EpsHifShortDrv;
EPS_HIF_SHORT_DRV_PTR_TYPE g_pEpsHifShortDrv = LSA_NULL;

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_short_drv_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsHifShortDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_short_drv_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsHifShortDrv->hEnterExit, LSA_FALSE);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_short_drv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsHifShortDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_short_drv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsHifShortDrv->hEnterExit);
}

/** 
 * Allocates a instance of the HIF Short Mode Driver.
 * 
 * @param LSA_VOID
 * @return EPS_HIF_SHORT_DRV_INST_PTR_TYPE - handle to instance
 */
static EPS_HIF_SHORT_DRV_INST_PTR_TYPE eps_hif_short_drv_alloc_instance(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_HIF_SHORT_DRV_INST_PTR_TYPE pInst = LSA_NULL;

	for(i=0; i<EPS_HIF_SHORT_DRV_MAX_INSTANCES; i++)
	{
		pInst = &g_pEpsHifShortDrv->Inst[i];

		if(pInst->bUsed == LSA_FALSE)
		{
			pInst->hOwnHandle = i;

			pInst->bUsed = LSA_TRUE;

			return pInst;
		}
	}

	return pInst;
}

/**
 * Releases an instance
 * @param [in] pInst - instance to be freed
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_short_drv_free_instance(EPS_HIF_SHORT_DRV_INST_PTR_TYPE pInst)
{
	EPS_ASSERT(pInst != LSA_NULL);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	pInst->bUsed = LSA_FALSE;
}

/**
 * Returns the instance.
 * 
 * @param [in] pHwInstIn                    - handle to instance
 * @return EPS_HIF_SHORT_DRV_INST_PTR_TYPE  - the pointer the struct of the instance
 */
static EPS_HIF_SHORT_DRV_INST_PTR_TYPE eps_hif_short_drv_get_inst (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	LSA_UINT32 index = (LSA_UINT32)pHwInstIn->hHandle;
	
	return &g_pEpsHifShortDrv->Inst[index];
}

/**
 * Installs the HIF Short Mode Driver by calling eps_hif_drv_if_register. Does a function mapping from the API defined in eps_hif_drv_if.
 * 
 * The Short Mode Driver is a short circuit driver. If HIF calls GetParams, the SHORT MODE is returned to HIF. 
 *  
 * Set the HIF Driver capabilites
 *    bUseIpcInterrupts     := LSA_FALSE
 *    bUsePnCoreInterrupts  := LSA_FALSE
 *    bUseRemoteMem         := LSA_FALSE
 *    bUseDma               := LSA_FALSE
 *    bUseShortMode         := LSA_TRUE
 *    bUseSharedMemMode     := LSA_FALSE
 * 
 * @see eps_hif_drv_if_register     - this function is used to register the driver implementation to the EPS HIF Drv IF.
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_hif_short_drv_install (LSA_VOID)
{
	EPS_HIF_DRV_IF_TYPE sHifDrvIf;
	EPS_HIF_DRV_CFG_TYPE sCfg;

	//Init Internals

	eps_memset(&g_EpsHifShortDrv, 0, sizeof(g_EpsHifShortDrv));
	g_pEpsHifShortDrv = &g_EpsHifShortDrv;

	eps_hif_short_drv_init_critical_section();

	eps_memset(&sHifDrvIf, 0, sizeof(sHifDrvIf));

	//Init Interface
	sHifDrvIf.open      = eps_hif_short_drv_open;
	sHifDrvIf.close     = eps_hif_short_drv_close;
	sHifDrvIf.uninstall = eps_hif_short_drv_uninstall;

	sHifDrvIf.AckIsr        = eps_hif_short_drv_ack_hif_ir;
	sHifDrvIf.DisableIsr    = eps_hif_short_drv_disable_isr;
	sHifDrvIf.EnableIsr     = eps_hif_short_drv_enable_isr;
	sHifDrvIf.ReenableIsr   = eps_hif_short_drv_reenable_hif_ir;
	sHifDrvIf.SendIsr       = eps_hif_short_drv_send_hif_ir;
	sHifDrvIf.DmaCpy        = eps_hif_short_drv_dmacpy;
	sHifDrvIf.MemPoolCreate = eps_hif_short_drv_mem_pool_create;
	sHifDrvIf.MemPoolFree   = eps_hif_short_drv_mem_pool_free;
	sHifDrvIf.MemAlloc      = eps_hif_short_drv_alloc;
	sHifDrvIf.MemFree       = eps_hif_short_drv_free;
	sHifDrvIf.GetParams     = eps_hif_short_drv_get_params;

	g_pEpsHifShortDrv->bInit = LSA_TRUE;

	eps_hif_drv_init_config(&sCfg);

	sCfg.bUseShortMode   = LSA_TRUE;

	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
}

/**
 * Uninstalls the driver by setting bInit to LSA_FALSE and deleting the pointer.
 * Releases locks.
 * 
 * @see eps_hif_drv_if_undo_init - calls this function
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hif_short_drv_uninstall (LSA_VOID)
{
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);

	eps_hif_short_drv_undo_init_critical_section();

	g_pEpsHifShortDrv->bInit = LSA_FALSE;

	g_pEpsHifShortDrv = LSA_NULL;
}

/**
 * eps_hif_drv_if_open implementation of the EPS HIF Short Mode Drv.
 *
 * @see eps_hif_drv_if_open
 *
 * @param [in] pInPars               - generic in structure for all EPS HIF drivers
 * @param [out] ppHwInstOut          - handle to the instance
 * @param [in] hd_id                 - 0 = LD, 1...4 = HD
 * @param [in] bUpper                - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_short_drv_open (EPS_HIF_DRV_OPENPAR_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	EPS_HIF_SHORT_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(pInPars != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_short_drv_open - Using EPS HIF Short Mode Drv. hd_id=%d, isUpper=%d", hd_id, bUpper);

	eps_hif_short_drv_enter();

	pInst = eps_hif_short_drv_alloc_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	pInst->InPar       = *pInPars;
	pInst->sHw.hd_id   = hd_id;
	pInst->sHw.bUpper  = bUpper;
	pInst->sHw.hHandle = pInst->hOwnHandle;

	*ppHwInstOut = &pInst->sHw;

	eps_hif_short_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function closes the instance of the HIF Short Mode driver 
 * 
 * @see eps_hif_drv_if_close  - calls this function
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_short_drv_close (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	EPS_HIF_SHORT_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	eps_hif_short_drv_enter();

	pInst = eps_hif_short_drv_get_inst(pHwInstIn);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);

	eps_hif_short_drv_free_instance(pInst);

	eps_hif_short_drv_exit();

	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_short_drv_close - done");

	return EPS_HIF_DRV_RET_OK;
}

/**
 * enable driver in hif - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * @param [in] pHwInstIn - handle to the instance
 * @param [in] hHif      - handle to HIF
 * @return #EPS_HIF_DRV_RET_ERR
 */
LSA_UINT16 eps_hif_short_drv_enable_isr (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_UINT16 hHif)
{
	LSA_UNUSED_ARG(hHif);
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */

	return EPS_HIF_DRV_RET_ERR;
}

/**
 * disable driver in hif - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @return #EPS_HIF_DRV_RET_ERR
 */
LSA_UINT16 eps_hif_short_drv_disable_isr (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */

	return EPS_HIF_DRV_RET_ERR;
}

/**
 * send hif interrupt - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @return #EPS_HIF_DRV_RET_ERR
 */
LSA_UINT16 eps_hif_short_drv_send_hif_ir (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */

	return EPS_HIF_DRV_RET_ERR;
}

/**
 * acknowledge hif interrupt - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @return #EPS_HIF_DRV_RET_ERR
 */
LSA_UINT16 eps_hif_short_drv_ack_hif_ir (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */

	return EPS_HIF_DRV_RET_ERR;
}

/**
 * reenable hif interrupt - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * 
 * @return EPS_HIF_DRV_RET_ERR
 */
LSA_UINT16 eps_hif_short_drv_reenable_hif_ir (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */

	return EPS_HIF_DRV_RET_ERR;
}

/**
 * hif: copy with dma - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 * @return LSA_VOID
 */
LSA_VOID eps_hif_short_drv_dmacpy (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_VOID** ppData, LSA_UINT32 uLength)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(ppData);
	LSA_UNUSED_ARG(uLength);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */
}

/**
 * hif: copy with dma - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 */
LSA_VOID eps_hif_short_drv_mem_pool_create (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_VOID* pBase, LSA_UINT32 uLength)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pBase);
	LSA_UNUSED_ARG(uLength);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */
}

/**
 * hif: copy with dma - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 */
LSA_VOID eps_hif_short_drv_mem_pool_free (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_UINT16 *pRetVal)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pRetVal);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pRetVal != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */
}

/**
 * hif: copy with dma - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 */
LSA_VOID eps_hif_short_drv_alloc (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_VOID_PTR_TYPE *ppMem, LSA_UINT32 uLength)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(ppMem);
	LSA_UNUSED_ARG(uLength);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(ppMem != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */
}

/**
 * hif: copy with dma - short mode function  
 * 
 * exists for reason of specification - Throws EPS_FATAL
 */
LSA_VOID eps_hif_short_drv_free (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_UINT16 *pRetVal, LSA_VOID_PTR_TYPE pMem)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pRetVal);
	LSA_UNUSED_ARG(pMem);

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pRetVal != LSA_NULL);

	EPS_FATAL("Function should not be called in short mode"); /* Should not happen in short mode */
}

/**
 * GetParams implementation of the EPS HIF Short Mode Drv.
 * 
 * The EPS HIF short mode driver is a short mode driver.
 *  
 * @see eps_hif_drv_if_get_params - calls this function
 *
 * @param [in] pHwInstIn          - handle to instance
 * @param [out] pPar              - casted to #HIF_GET_PAR_PTR_TYPE
 * @return LSA_VOID
 */
LSA_VOID eps_hif_short_drv_get_params (EPS_HIF_DRV_HW_PTR_TYPE pHwInstIn, LSA_VOID* pPar)
{
	EPS_HIF_SHORT_DRV_INST_PTR_TYPE pInst;
	HIF_GET_PAR_PTR_TYPE pHifParams = (HIF_GET_PAR_PTR_TYPE) pPar;

	eps_hif_short_drv_enter();

	EPS_ASSERT(g_pEpsHifShortDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifShortDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHifParams != LSA_NULL);

	pInst = eps_hif_short_drv_get_inst(pHwInstIn);

	eps_memset(pHifParams, 0, sizeof(*pHifParams));

	EPS_ASSERT(pInst->InPar.Mode == EPS_HIF_DRV_HIF_TM_SHORT); /* Only short mode is supported */

	pHifParams->Mode = pInst->InPar.Mode;

	pHifParams->if_pars.hif_lower_device = pInst->InPar.if_pars.hif_lower_device;

	pHifParams->if_pars.SharedMem.bUseTimerPoll = LSA_FALSE;
    pHifParams->if_pars.SharedMem.SendIrqMode = pInst->InPar.if_pars.SendIrqMode;
    pHifParams->if_pars.SharedMem.uNsWaitBeforeSendIrq = pInst->InPar.if_pars.uNsWaitBeforeSendIrq;
    
	pHifParams->if_pars.SharedMem.Local.Base = pInst->InPar.if_pars.SharedMem.Local.Base;   
    pHifParams->if_pars.SharedMem.Local.Size = pInst->InPar.if_pars.SharedMem.Local.Size;
	pHifParams->if_pars.SharedMem.Remote.Base = pInst->InPar.if_pars.SharedMem.Remote.Base;
	pHifParams->if_pars.SharedMem.Remote.Size = pInst->InPar.if_pars.SharedMem.Remote.Size;

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_short_drv_get_params - Using EPS HIF Short Drv. hd_id=%d, isUpper=%d", pInst->sHw.hd_id, pInst->sHw.bUpper); 
	  
	eps_hif_short_drv_exit();
}
