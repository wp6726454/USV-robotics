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
/*  F i l e               &F: eps_pn_drv_if.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN Device Driver Interface                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20024
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>       /* Types / Prototypes / Fucns */
#include <eps_trc.h>       /* Tracing                    */
#include <eps_cp_hw.h>     /* EPS PSI HW adaption layer  */
#include <eps_pn_drv_if.h> /* PN-Driver Interface        */

//----------------------------------------------------------------------------------------------------

#define EPS_CFG_PN_DRV_IF_MAX             2
#define EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO 50

typedef struct eps_pn_drv_if_store_tag
{
	struct
	{
		LSA_BOOL bUsed;
		EPS_PNDEV_IF_TYPE g_EpsPnDevIf;
	} sInterface[EPS_CFG_PN_DRV_IF_MAX];
	struct
	{
		EPS_PNDEV_HW_PTR_TYPE pHw;
		EPS_PNDEV_IF_PTR_TYPE pIf;
	} sHw[PSI_CFG_MAX_HD_CNT+1];
	struct
	{
		LSA_BOOL bUsed;
		EPS_PNDEV_IF_DEV_INFO_TYPE sDevInfo;
	} sRegisteredDevInfoStore[EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO];
} EPS_PN_DRV_IF_STORE_TYPE, *EPS_PN_DRV_IF_STORE_PTR_TYPE;

//----------------------------------------------------------------------------------------------------

/* Global definition */
EPS_PN_DRV_IF_STORE_TYPE g_EpsPnDevIfStore;
EPS_PN_DRV_IF_STORE_PTR_TYPE g_pEpsPnDevIfStore = LSA_NULL;

//----------------------------------------------------------------------------------------------------

/**
 * Initializes the PN Dev IF store by setting all values to 0.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_init(LSA_VOID)
{
	eps_memset(&g_EpsPnDevIfStore, 0, sizeof(g_EpsPnDevIfStore));

	g_pEpsPnDevIfStore = &g_EpsPnDevIfStore;
}

/**
 * revert the initialization of pndevdrv interfaces by calling the uninstall function of the implementation.
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndev_if_undo_init(LSA_VOID)
{
	LSA_UINT16 i;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);

	for(i=0; i<(PSI_CFG_MAX_HD_CNT+1); i++)
	{
		if( (g_pEpsPnDevIfStore->sHw[i].pHw != LSA_NULL) ||
			(g_pEpsPnDevIfStore->sHw[i].pIf != LSA_NULL) )
		{
			EPS_FATAL("All boards have to be closed by undo initialization of pndevdrv"); /* not all boards closed */
		}
	}

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX; i++)
	{
		if(g_pEpsPnDevIfStore->sInterface[i].bUsed == LSA_TRUE)
		{
			EPS_ASSERT(g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf.uninstall != LSA_NULL);

			g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf.uninstall();
			g_pEpsPnDevIfStore->sInterface[i].bUsed = LSA_FALSE;
		}
	}

	eps_memset(&g_EpsPnDevIfStore, 0, sizeof(g_EpsPnDevIfStore));
	g_pEpsPnDevIfStore = LSA_NULL;
}

//z003010au DOCU
/**
 * This function registers a PN Dev implementation into the EPS PN Dev IF.
 * 
 * Each PN Dev implementation calls this function in the install function.
 * Checks if all the function pointer are linked to actual functions by checking for NULL pointers.
 * 
 * @param [in] pPnDevIf    - structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_register(EPS_PNDEV_IF_PTR_TYPE pPnDevIf)
{
	LSA_UINT32 i;

	EPS_ASSERT(pPnDevIf != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);

	EPS_ASSERT(pPnDevIf->close != LSA_NULL);
	EPS_ASSERT(pPnDevIf->open != LSA_NULL);
	EPS_ASSERT(pPnDevIf->uninstall != LSA_NULL);

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX; i++)
	{
		if(g_pEpsPnDevIfStore->sInterface[i].bUsed == LSA_FALSE)
		{
			g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf = *pPnDevIf;
			g_pEpsPnDevIfStore->sInterface[i].bUsed = LSA_TRUE;
			return;
		}
	}

	EPS_FATAL("Not enough free interfaces to register a new one"); /* If Overflow */
}

/**
 * Registers a PNBoard into the registred devices storage.
 * 
 * @see eps_pndev_if_get_registered_devices - returns all the registred devices.
 * 
 * @param [in] pDev                         - info about the PCI Location / MAC address 
 * @return LSA_VOID
 */
LSA_VOID eps_pndev_if_register_device(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev)
{
	LSA_UINT32 i;
	LSA_BOOL bSuccess = LSA_FALSE;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);	

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO; i++)
	{
		if(g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed == LSA_FALSE)
		{
			g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo = *pDev;

			g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed = LSA_TRUE;
			bSuccess = LSA_TRUE;
			break;
		}
	}

	EPS_ASSERT(bSuccess == LSA_TRUE);
}

/**
 * Returns a structure will all supported PNBoards. 
 * 
 * Currently eps_imcea only. T
 * 
 * UseCase: The LD Lower at Advanced uses this function to store all supported PNBoards into the shared memory.
 * @see eps_pndev_if_get_registered_devices - calls this function
 * @param [out] pDev                        - out structure with infos about the supported PN Boards
 * @param [in] uMaxDevices                  - max. number of devices that the caller expects.
 * @param [out] pCntDevices                 - actual number of devices. Will be smaller or equal to uMaxDevices
 * @return
 */
LSA_VOID eps_pndev_if_get_registered_devices(EPS_PNDEV_IF_DEV_INFO_PTR_TYPE pDev, LSA_UINT16 uMaxDevices, LSA_UINT16* pCntDevices)
{
	LSA_UINT16 i=0,e = 0;

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL);
	EPS_ASSERT(pDev != LSA_NULL);
	EPS_ASSERT(pCntDevices != LSA_NULL);

	if(uMaxDevices  > EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO)
	{
		e = EPS_CFG_PN_DRV_IF_MAX_BOARD_INFO;
	}
	else
	{
		e = uMaxDevices;
	}

	while(i<e)
	{
		if(g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].bUsed == LSA_TRUE)
		{
			pDev[i].uPciAddrValid = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciAddrValid;
			pDev[i].uPciBusNr     = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciBusNr;
			pDev[i].uPciDeviceNr  = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciDeviceNr;
			pDev[i].uPciFuncNr    = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uPciFuncNr;

			pDev[i].uMacAddrValid = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddrValid;
			pDev[i].uMacAddr[0]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[0];
			pDev[i].uMacAddr[1]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[1];
			pDev[i].uMacAddr[2]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[2];
			pDev[i].uMacAddr[3]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[3];
			pDev[i].uMacAddr[4]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[4];
			pDev[i].uMacAddr[5]   = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uMacAddr[5];

			pDev[i].uEddType     = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.uEddType;

            #if defined (EPS_CFG_USE_PNDEVDRV)
            pDev[i].eBoard       = g_pEpsPnDevIfStore->sRegisteredDevInfoStore[i].sDevInfo.eBoard;
            #endif

			i++;
		}
		else
		{
			break;
		}
	}

	*pCntDevices = i;
}

/**
 * This function searches in all installed PN Dev Instances for a PN Dev implementation that supports the given PNBoard.
 * This function is used
 *    - by the EPS LD upper that wants to download the LD+HD firmware
 *    - by the EPS HD upper that wants to download the HD firmware
 *    - by the HD to get connection to the read hardware (Basic, Advanced 2T)
 *    
 * Note that the first driver that finds a board on the given location will be used. This may be the case if two drivers support the board (eps_pndevdrv, eps_stdmacdrv).
 *    
 * The information about the PNBoard are stored in ppHwInstOut. This structure also contains function pointer to access the board
 * 
 * @see eps_hw_connect                  - calls this function 
 * @see PSI_GET_HD_PARAM                - calls eps_hw_connect. UseCase: Light, Basic FW (LD+HD, HD part), Advanced 2T (LD+HD, HD part)
 * @see PSI_HIF_HD_U_GET_PARAMS         - calls eps_hw_connect. UseCase: Light + HD FW (HD Upper). Calls it with hd_nr = 1...4
 * @see PSI_HIF_LD_U_GET_PARAMS         - calls eps_hw_connect. UseCase: Basic and Advanced (LD Upper, at application level). Calls it with hd_nr = 0 
 * 
 * @param [in] pLocation                - location descriptor (PCI Location or MAC Address)
 * @param [in] pOption                  - options (debug, download of firmware). Note that not all PN Dev implementations support this.
 * @param [in] ppHwInstOut              - out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @param [in] hd_id                    - hd number. 0 = LD, 1...4 = HD instance.
 * @return #EPS_PNDEV_RET_DEV_NOT_FOUND - board was not found by any installed PN Dev Implementation
 * @return #EPS_PNDEV_RET_OK            - board was successfully openend
 */
LSA_UINT16 eps_pndev_if_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	LSA_UINT32 i;
	LSA_UINT16 retVal = EPS_PNDEV_RET_DEV_NOT_FOUND;
	EPS_PNDEV_IF_PTR_TYPE pIf = LSA_NULL;

	EPS_ASSERT(pLocation != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pHw == LSA_NULL);

	EPS_ASSERT(g_pEpsPnDevIfStore != LSA_NULL)        /* Uninit?                         */
	EPS_ASSERT(hd_id <= PSI_CFG_MAX_HD_CNT)          /* hd_id out of range               */

	for(i=0; i<EPS_CFG_PN_DRV_IF_MAX; i++)
	{
	  // call open at all PN Dev implementations
		if(g_pEpsPnDevIfStore->sInterface[i].bUsed == LSA_TRUE)
		{
			pIf = &g_pEpsPnDevIfStore->sInterface[i].g_EpsPnDevIf;
			
			EPS_ASSERT(pIf != LSA_NULL);
			EPS_ASSERT(pIf->open != LSA_NULL);

			retVal = pIf->open(pLocation, pOption, ppHwInstOut, hd_id);

			if(retVal == EPS_PNDEV_RET_OK)
			{
				g_pEpsPnDevIfStore->sHw[hd_id].pHw = *ppHwInstOut;
				g_pEpsPnDevIfStore->sHw[hd_id].pIf = pIf;
				// PN Dev implementation supports board, success. Even if the other implementations support that board, they are not used.
				break;
			}
		}
	}

	return retVal;
}

/**
 * Returns the descriptior of the PNBoard on the given hd_id.
 * Note that at eps_pndev_if_open, a hd_id and location descriptor is given to EPS PN Dev. 
 * EPS PN Dev stores the ppHwInstOut for the PNBoard with the hd_id provided there. If the same hd_id is provided here, the same 
 * ppHwInstOut will be returned.
 * 
 * @see eps_pndev_if_open       - opens the PNBoard. Also returns EPS_PNDEV_HW_PTR_PTR_TYPE. The usecases are also described there.
 * @see eps_hw_connect          - calls this function to see if the board was already openend. 
 * 
 * @see eps_eddi_exttimer_start   - calls this function (EDDI only)
 * @see eps_eddi_exttimer_stop    - calls this function (EDDI only)
 * @see eps_enable_pnio_ir        - calls this function (interrupt mechanism)
 * @see eps_disable_pnio_ir       - calls this function (interrupt mechanism)
 * @see eps_enable_iso_interrupt  - calls this function (isochronous mode, interrupt mechanism)
 * @see eps_disable_iso_interrupt - calls this function (isochronous mode, interrupt mechanism)
 * @param [in] hd_id              - 0 = LD , 1...4 = HD. 
 * @param [out] ppHwInstOut       - out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @return
 */
LSA_UINT16 eps_pndev_if_get(LSA_UINT16 hd_id, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut)
{
	LSA_UINT16 retVal = EPS_PNDEV_RET_OK;

	EPS_ASSERT(ppHwInstOut != LSA_NULL);
	EPS_ASSERT(hd_id <= PSI_CFG_MAX_HD_CNT)      /* hd_id out of range               */

	*ppHwInstOut = g_pEpsPnDevIfStore->sHw[hd_id].pHw;

	if(g_pEpsPnDevIfStore->sHw[hd_id].pHw == LSA_NULL)
	{
		retVal = EPS_PNDEV_RET_DEV_NOT_FOUND;
	}

	return retVal;
}

/**
 * Closes the PN Dev instance by callind the close function of the implementation.
 * @param hd_id               - 0 = LD, 1...4 = HD.
 * @return #EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndev_if_close(LSA_UINT16 hd_id)
{
	LSA_UINT16 retVal;

	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pHw != LSA_NULL);
	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pIf != LSA_NULL);

	EPS_ASSERT(g_pEpsPnDevIfStore->sHw[hd_id].pIf->close != LSA_NULL);

	retVal = g_pEpsPnDevIfStore->sHw[hd_id].pIf->close(g_pEpsPnDevIfStore->sHw[hd_id].pHw);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);

	g_pEpsPnDevIfStore->sHw[hd_id].pHw = LSA_NULL;
	g_pEpsPnDevIfStore->sHw[hd_id].pIf = LSA_NULL;

	return retVal;
}

/**
 * Closes the PN Dev instance in a fatal. Does not check for return code EPS_PNDEV_RET_OK.
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_pndev_if_fatal_close(LSA_VOID)
{
	LSA_UINT32 i;

	for( i = 0; i < (PSI_CFG_MAX_HD_CNT+1); i++ )
	{
		if(g_pEpsPnDevIfStore->sHw[i].pHw != LSA_NULL)
		{
			g_pEpsPnDevIfStore->sHw[i].pIf->close(g_pEpsPnDevIfStore->sHw[i].pHw);
			g_pEpsPnDevIfStore->sHw[i].pHw = LSA_NULL;
			g_pEpsPnDevIfStore->sHw[i].pIf = LSA_NULL;
		}
	}
}

//----------------------------------------------------------------------------------------------------
