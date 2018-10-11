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
/*  F i l e               &F: eps_hif_drv_if.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF Device Driver Interface                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20055
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>        /* Types / Prototypes / Fucns */
#include <eps_trc.h>        /* Tracing                    */
#include <eps_hif_drv_if.h> /* HIF-Driver Interface       */

//----------------------------------------------------------------------------------------------------

#define EPS_CFG_HIF_DRV_IF_MAX             10
#define EPS_CFG_HIF_DRV_IF_MAX_DEVICES    (PSI_CFG_MAX_HD_CNT*2+2)

typedef struct eps_hif_drv_if_store_tag
{
	struct
	{
		LSA_BOOL bUsed;
		EPS_HIF_DRV_CFG_TYPE Cfg;
		EPS_HIF_DRV_IF_TYPE g_EpsHifDrvIf;
	} sInterface[EPS_CFG_HIF_DRV_IF_MAX];
	struct
	{
		EPS_HIF_DRV_HW_PTR_TYPE pHw;
		EPS_HIF_DRV_IF_PTR_TYPE pIf;
	} sHw[EPS_CFG_HIF_DRV_IF_MAX_DEVICES];
} EPS_HIF_DRV_IF_STORE_TYPE, *EPS_HIF_DRV_IF_STORE_PTR_TYPE;

//----------------------------------------------------------------------------------------------------

/* Global definition */
EPS_HIF_DRV_IF_STORE_TYPE g_EpsHifDrvIfStore;
EPS_HIF_DRV_IF_STORE_PTR_TYPE g_pEpsHifDrvIfStore = LSA_NULL;

//----------------------------------------------------------------------------------------------------

/**
 * This function matches the capability sturcture pCfg1 against a second capability sturcture pCfg2.
 *
 * This function is used to find the right HIF driver for the EPS PN Dev Instance / EPS SHM Instance.
 *
 * The capabilities of the HIF drivers are set at the corresponding eps_hif_XXX_drv_install function.
 * The requested capabilites are set at the corresponding eps_xxx_open_hif_drv function of the
 * EPS SHM IF implementation / EPS PN Dev IF implementation.
 *
 * @see eps_hif_drv_if_open_intern and eps_hif_drv_if_open use this function.
 *
 * @param [in] pCfg1  - capability struct 1
 * @param [in] pCfg2  - capability struct 2
 * @return LSA_TRUE   - The capabilities do match
 * @return LSA_FALSE  - The capabilities do not match
 */
static LSA_BOOL eps_hif_drv_config_fits_perfect(EPS_HIF_DRV_CFG_PTR_TYPE pCfg1, EPS_HIF_DRV_CFG_PTR_TYPE pCfg2)
{
	EPS_ASSERT(pCfg1 != LSA_NULL);
	EPS_ASSERT(pCfg2 != LSA_NULL);
	EPS_ASSERT(pCfg1 != pCfg2); /* Comparing the same data doesn't make sense => check code */

	if( (pCfg1->bUseDma              == pCfg2->bUseDma) &&
		(pCfg1->bUseIpcInterrupts    == pCfg2->bUseIpcInterrupts) &&
		(pCfg1->bUsePnCoreInterrupts == pCfg2->bUsePnCoreInterrupts) &&
		(pCfg1->bUseRemoteMem        == pCfg2->bUseRemoteMem) &&
		(pCfg1->bUseShortMode        == pCfg2->bUseShortMode) &&
		(pCfg1->bUseSharedMemMode    == pCfg2->bUseSharedMemMode) )
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * Sets all capabilites to LSA_FALSE.
 * @param [inout] pCfg - pointer to the structure with the capabilities.
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_init_config(EPS_HIF_DRV_CFG_PTR_TYPE pCfg)
{
	EPS_ASSERT(pCfg != LSA_NULL);

	eps_memset(pCfg, 0, sizeof(*pCfg));

	pCfg->bUseDma              = LSA_FALSE;
	pCfg->bUseIpcInterrupts    = LSA_FALSE;
	pCfg->bUsePnCoreInterrupts = LSA_FALSE;
	pCfg->bUseRemoteMem        = LSA_FALSE;
	pCfg->bUseShortMode        = LSA_FALSE;
	pCfg->bUseSharedMemMode    = LSA_FALSE;
}

/**
 * Inits the HIF driver storage
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_if_init(LSA_VOID)
{
	eps_memset(&g_EpsHifDrvIfStore, 0, sizeof(g_EpsHifDrvIfStore));

	g_pEpsHifDrvIfStore = &g_EpsHifDrvIfStore;
}

/** 
 * calls the uninstall function for all Interfaces.
 * 
 * if all boards are closed, the Interfaces are uninstalled and marked as unused.
 * g_EpsHifDrvIfStore is set to 0 after uninstalling.
 * 
 * @param LSA_VOID
 */
LSA_VOID eps_hif_drv_if_undo_init(LSA_VOID)
{
	LSA_UINT16 i;

	EPS_ASSERT(g_pEpsHifDrvIfStore != LSA_NULL);

	for(i=0; i<EPS_CFG_HIF_DRV_IF_MAX_DEVICES; i++)
	{
		if( (g_pEpsHifDrvIfStore->sHw[i].pHw != LSA_NULL) ||
			(g_pEpsHifDrvIfStore->sHw[i].pIf != LSA_NULL) )
		{
			EPS_FATAL("not all boards closed."); /* not all boards closed */
		}
	}

	for(i=0; i<EPS_CFG_HIF_DRV_IF_MAX; i++)
	{
		if(g_pEpsHifDrvIfStore->sInterface[i].bUsed == LSA_TRUE)
		{
			EPS_ASSERT(g_pEpsHifDrvIfStore->sInterface[i].g_EpsHifDrvIf.uninstall != LSA_NULL);

			g_pEpsHifDrvIfStore->sInterface[i].g_EpsHifDrvIf.uninstall();
			g_pEpsHifDrvIfStore->sInterface[i].bUsed = LSA_FALSE;
		}
	}

	eps_memset(&g_EpsHifDrvIfStore, 0, sizeof(g_EpsHifDrvIfStore));
	g_pEpsHifDrvIfStore = LSA_NULL;
}

/** 
 * Registers a new interface
 * 
 * if none function of pHifDrvIf is LSA_NULL, an empty sInterface is searched.
 * There pHifDrvIf an pConfig is stored and it is marked as used.
 * If none is found, a FATAL occures. 
 * 
 * @param [in] pHifDrvIf - structure containing the driver controlling functions
 * @param [in] pConfig   - capability structrue with several config flags
 */
LSA_VOID eps_hif_drv_if_register(EPS_HIF_DRV_IF_PTR_TYPE pHifDrvIf, EPS_HIF_DRV_CFG_PTR_TYPE pConfig)
{
	LSA_UINT32 i;

	EPS_ASSERT(pHifDrvIf != LSA_NULL);
	EPS_ASSERT(g_pEpsHifDrvIfStore != LSA_NULL);

	EPS_ASSERT(pHifDrvIf->close         != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->open          != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->uninstall     != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->MemPoolCreate != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->MemPoolFree   != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->MemAlloc      != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->MemFree       != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->EnableIsr	    != LSA_NULL);
	EPS_ASSERT(pHifDrvIf->DisableIsr	!= LSA_NULL);
	EPS_ASSERT(pHifDrvIf->SendIsr		!= LSA_NULL);
	EPS_ASSERT(pHifDrvIf->AckIsr		!= LSA_NULL);
	EPS_ASSERT(pHifDrvIf->ReenableIsr	!= LSA_NULL);
	EPS_ASSERT(pHifDrvIf->GetParams     != LSA_NULL);

	for(i=0; i<EPS_CFG_HIF_DRV_IF_MAX; i++)
	{
		if(g_pEpsHifDrvIfStore->sInterface[i].bUsed == LSA_FALSE)
		{
			g_pEpsHifDrvIfStore->sInterface[i].g_EpsHifDrvIf = *pHifDrvIf;
			g_pEpsHifDrvIfStore->sInterface[i].Cfg = *pConfig;
			g_pEpsHifDrvIfStore->sInterface[i].bUsed = LSA_TRUE;
			return;
		}
	}

	EPS_FATAL("interface overflow: too much interfaces registered"); /* If Overflow */
}

/**
 * Internal implementation of eps_hif_drv_if_open.
 *
 * @see eps_hif_drv_if_open
 *
 * @param [in] hd_id       - 0 = LD, 1...4 = HD
 * @param [in] bUpper      - 1 = Upper, 0 = Lower
 * @param [in] pCfg        - capability strucure for the requested HIF driver
 * @param [in] pInPars     - generic in structure for HIF drivers
 * @param [out] ppHwInstOut - handle to instance
 * @return #EPS_HIF_DRV_RET_ERR   an error occured
 * @return #EPS_HIF_DRV_RET_OK    successully openend
 */
static LSA_UINT16 eps_hif_drv_if_open_intern (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_CFG_PTR_TYPE pCfg, struct eps_hif_drv_openpar_tag* pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT32 i;
	LSA_UINT16 retVal = EPS_HIF_DRV_RET_ERR;
	EPS_HIF_DRV_IF_PTR_TYPE pIf = LSA_NULL;
	LSA_UINT32 DeviceIndex;
	LSA_BOOL bFitResult = LSA_FALSE;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}
	
	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw == LSA_NULL);

	EPS_ASSERT(g_pEpsHifDrvIfStore != LSA_NULL)               /* Uninit?                         */
	EPS_ASSERT(DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES); /* hd_id out of range              */
	EPS_ASSERT(pCfg != LSA_NULL);

  // Search all installed EPS HIF Drv instances to see if any of the HIF drivers match the capability structure.
	for(i=0; i<EPS_CFG_HIF_DRV_IF_MAX; i++)
	{
    // only seach used drivers
		if(g_pEpsHifDrvIfStore->sInterface[i].bUsed == LSA_TRUE)
		{
			pIf = &g_pEpsHifDrvIfStore->sInterface[i].g_EpsHifDrvIf;
			
			EPS_ASSERT(pIf != LSA_NULL);
      // This function checks the capability structure against the capability structure of the HIF driver.
      // Note that if more than one driver matches, the first one will be used.
			bFitResult = eps_hif_drv_config_fits_perfect(pCfg, &g_pEpsHifDrvIfStore->sInterface[i].Cfg);

			if(bFitResult == LSA_TRUE)
			{
				EPS_ASSERT(pIf->open != LSA_NULL);

        // The HIF driver matches, so the open function is called.
				retVal = pIf->open(pInPars, ppHwInstOut, hd_id, bUpper);

				if(retVal == EPS_HIF_DRV_RET_OK)
				{
					g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw = *ppHwInstOut;
					g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf = pIf;
					break;
				}
			}
		}
	}

	return retVal;
}

/**
 * Some EPS PN DEV implementations and all EPS SHM IF implementation require a HIF driver.
 *   - The EPS SHM IF implementation uses a HIF driver for the HD / LD lower interface
 *   - The EPS PN Dev implementation uses a HIF driver for the HD / LD upper interface
 * eps_hif_drv_if_open is called at the eps_xxx_open_hif_drv.
 *
 * @param [in] hd_id       - 0 = LD, 1...4 = HD
 * @param [in] bUpper      - 1 = Upper, 0 = Lower
 * @param [in] pCfg        - capability strucure for the requested HIF driver
 * @param [in] pInPars     - generic in structure for HIF drivers
 * @param [out] ppHwInstOut - handle to instance
 * @return #EPS_HIF_DRV_RET_ERR   an error occured
 * @return #EPS_HIF_DRV_RET_OK    successully openend
 */
LSA_UINT16 eps_hif_drv_if_open (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_CFG_PTR_TYPE pCfg, struct eps_hif_drv_openpar_tag* pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT16 retVal = EPS_HIF_DRV_RET_ERR;

	retVal = eps_hif_drv_if_open_intern(hd_id, bUpper, pCfg, pInPars, ppHwInstOut);

	return retVal;
}

/**
 * Returns the instance of a HIF driver.
 * @param [in] hd_id           - 0 = LD, 1...4 = HD
 * @param [in] bUpper          - 1 = Upper, 0 = Lower
 * @param [out] ppHwInstOut     - handle to instance
 * @return #EPS_HIF_DRV_RET_ERR - Instance not found
 * @return #EPS_HIF_DRV_RET_OK  - Instance found
 */
LSA_UINT16 eps_hif_drv_if_get (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT16 retVal = EPS_HIF_DRV_RET_OK;
	LSA_UINT32 DeviceIndex;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES); /* hd_id out of range              */

	*ppHwInstOut = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw;

	if(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw == LSA_NULL)
	{
		retVal = EPS_HIF_DRV_RET_ERR;
	}

	return retVal;
}

/**
 * Closes a HIF driver instance
 * @param [in] hd_id     - 0 = LD, 1...4 = HD
 * @param [in] bUpper    - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK - Driver instance was closed successfully
 */

LSA_UINT16 eps_hif_drv_if_close (LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	LSA_UINT16 retVal;
	LSA_UINT32 DeviceIndex;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}		

	EPS_ASSERT(DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES); /* hd_id out of range              */	
	
	EPS_ASSERT(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);
	EPS_ASSERT(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf != LSA_NULL);

	EPS_ASSERT(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf->close != LSA_NULL);

	retVal = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf->close(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw);
	EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);

	g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw = LSA_NULL;
	g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf = LSA_NULL;

	return retVal;
}

/**
 * This function creates a memory pool. The function calls the corresponding MemPoolCreate function of the HIF Driver.
 *
 * @see PSI_CREATE_HIF_MEM_POOL - calls this function
 *
 * @param [in] hd_id          - 0 = LD, 1...4 = HD
 * @param [in] bUpper         - 1 = Upper, 0 = Lower
 * @param [out] hPool         - handle to the pool. Equals the calculated DeviceIndex
 * @param [out] pBase         - BasePointer to the allocated mem region
 * @param [in] uLength        - requested length
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_if_mem_pool_create(LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_INT* hPool, LSA_VOID* pBase, LSA_UINT32 uLength)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);
	EPS_ASSERT( pBase != LSA_NULL );
	EPS_ASSERT( uLength != 0 );

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	pIf->MemPoolCreate(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, pBase, uLength);
	
	*hPool = (LSA_INT)DeviceIndex;
}

/**
 * This function releases a HIF pool. The function calls the corresponding MemPoolFree function of the HIF Driver.
 *
 * @param [in] hPool        - Handle to HIF driver instance
 * @param [out] pRetVal     - return value from the HIF driver instance
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_if_mem_pool_free (LSA_INT hPool, LSA_UINT16 *pRetVal)
{
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT32 DeviceIndex = (LSA_UINT32)hPool;

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	pIf->MemPoolFree(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, pRetVal);
}

/**
 * This function allocates memory for a given pool. The function calls the corresponding MemAlloc function of the HIF Driver.
 *
 * @see PSI_HIF_ALLOC_MEM   - calls this function
 * @param [in] hPool        - Handle to HIF driver intance
 * @param [out] ppMem       - pointer to the allocated memory
 * @param [in] uLength      - requested length
 * @return
 */
LSA_VOID eps_hif_drv_if_alloc_shm (LSA_INT hPool, LSA_VOID_PTR_TYPE *ppMem, LSA_UINT32 uLength)
{
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT32 DeviceIndex = (LSA_UINT32)hPool;

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	pIf->MemAlloc(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, ppMem, uLength);
}

/**
 * This function frees memory for a given pool. The function calls the corresponding MemFree function of the HIF Driver.
 * @see PSI_HIF_SHM_DMA_COPY  - calls this function
 *
 * @param [in] hPool          - Handle to HIF driver intance
 * @param [out] pRetVal       - return value from the HIF driver instanc
 * @param [in] pMem           - pointer to the memory region that shall be released.
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_if_free_shm (LSA_INT hPool, LSA_UINT16 *pRetVal, LSA_VOID_PTR_TYPE pMem)
{
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT32 DeviceIndex = (LSA_UINT32)hPool;

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	pIf->MemFree(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, pRetVal, pMem);
}

/**
 * This function copies data using a DMA or memcopy if DMA transfer is not supported
 * The function calls the corresponding DmaCpy function of the HIF Driver.
 *
 * @see PSI_HIF_SHM_DMA_COPY  - calls this function
 * @param [in] hd_id          - 0 = LD, 1...4 = HD
 * @param [in] bUpper         - 1 = Upper, 0 = Lower
 * @param [in] ppData         - pointer to the data that shall be copied
 * @param [in] uLength        - length of the data to be copied
 * @return LSA_VOID
 */
LSA_VOID eps_hif_drv_if_dma_copy (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID** ppData, LSA_UINT32 uLength)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	pIf->DmaCpy(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, ppData, uLength);
}

/**
 * This function enables the interrupt service routine if supported. Polling mode drivers will do nothing.
 * The function calls the corresponding EnableIsr function of the HIF Driver.
 * 
 * @param [in] hd_id        - 0 = LD, 1...4 = HD
 * @param [in] bUpper       - 1 = Upper, 0 = Lower
 * @param [in] hHif         - handle for HIF.
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_drv_if_enable_isr (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_UINT16 hHif)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT16 retVal;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	retVal = pIf->EnableIsr(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, hHif);

	return retVal;
}

/** 
 * This function disables the interrupt service routine if supported. Polling mode drivers will do nothing.
 * The function calls the corresponding DisableIsr function of the HIF Driver.
 * 
 * @param [in] hd_id        - 0 = LD, 1...4 = HD
 * @param [in] bUpper       - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_drv_if_disable_isr (LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT16 retVal;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	retVal = pIf->DisableIsr(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw);

	return retVal;
}

/**
 * This function sends an interrupt to the Lower/Upper HIF driver running on the other CPU / Asic system. 
 * Polling mode drivers will do nothing.
 * The function calls the corresponding SendIsr function of the HIF Driver.
 * 
 * @param [in] hd_id        - 0 = LD, 1...4 = HD
 * @param [in] bUpper       - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_drv_if_send_isr (LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT16 retVal;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	retVal = pIf->SendIsr(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw);

	return retVal;
}

/**
 * This function acknowledges an interrupt. 
 * Polling mode drivers will do nothing.
 * The function calls the corresponding AckIsr function of the HIF Driver.
 * 
 * @see PSI_HIF_LD_ACK_IRQ  - calls this function
 * @see PSI_HIF_HD_ACK_IRQ  - calls this function
 * @param [in] hd_id        - 0 = LD, 1...4 = HD
 * @param [in] bUpper       - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_drv_if_ack_isr (LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT16 retVal;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	retVal = pIf->AckIsr(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw);

	return retVal;
}

/**
 * This function sends an interrupt to the Lower/Upper HIF driver running on the other CPU / Asic system. 
 * Polling mode drivers will do nothing.
 * The function calls the corresponding SendIsr function of the HIF Driver.
 * 
 * @param [in] hd_id        - 0 = LD, 1...4 = HD
 * @param [in] bUpper       - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_drv_if_reenable_isr (LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;
	LSA_UINT16 retVal;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

	retVal = pIf->ReenableIsr(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw);

	return retVal;
}

/**
 * EPS HIF DRV GetParams is used by PSI to implement the HIF OUT-Macros PSI_HIF_LD_U_GET_PARAMS, PSI_HIF_LD_L_GET_PARAMS, PSI_HIF_HD_U_GET_PARAMS, PSI_HIF_HD_L_GET_PARAMS
 *
 * Depending on which EPS HIF driver is used, the driver will return the params structure requested by HIF.
 *
 * @see PSI_HIF_LD_U_GET_PARAMS - calls this function 
 * @see PSI_HIF_LD_L_GET_PARAMS - calls this function
 * @see PSI_HIF_HD_U_GET_PARAMS - calls this function
 * @see PSI_HIF_HD_L_GET_PARAMS - calls this function
 *
 * @param [in] hd_id  - 0 = LD, 1...4 = HD
 * @param [in] bUpper - 1 = Upper, 0 = Lower
 * @param [in] pPar   - void* that is casted to HIF_GET_PAR_PTR_TYPE in EPS HIF DRV implementation. This variable is returned to HIF.
 */
LSA_VOID eps_hif_drv_if_get_params (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID* pPar)
{
	LSA_UINT32 DeviceIndex;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;

	if(bUpper)
	{
		DeviceIndex = 2 * hd_id + 1;
	}
	else
	{
		DeviceIndex = 2 * hd_id;
	}	

	EPS_ASSERT( DeviceIndex < EPS_CFG_HIF_DRV_IF_MAX_DEVICES ); /* hd_id out of range              */
	EPS_ASSERT( g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw != LSA_NULL);

	pIf = g_pEpsHifDrvIfStore->sHw[DeviceIndex].pIf;

  EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_drv_if_get_params - Calling GetParams for HIF Drv instance. hd_id=%d, isUpper=%d", hd_id, bUpper);

  // Use the installed EPS HIF DRV instances to get the params structure.
	pIf->GetParams(g_pEpsHifDrvIfStore->sHw[DeviceIndex].pHw, pPar);
}

LSA_VOID eps_hif_drv_if_check_reenable_isr(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_HIF_DRV_IF_PTR_TYPE pIf;

	for(i=0; i<EPS_CFG_HIF_DRV_IF_MAX_DEVICES; i++)
	{
		if(g_pEpsHifDrvIfStore->sHw[i].pHw != LSA_NULL)
		{
			pIf = g_pEpsHifDrvIfStore->sHw[i].pIf;

			pIf->ReenableIsr(g_pEpsHifDrvIfStore->sHw[i].pHw);
		}
	}
}

//----------------------------------------------------------------------------------------------------
