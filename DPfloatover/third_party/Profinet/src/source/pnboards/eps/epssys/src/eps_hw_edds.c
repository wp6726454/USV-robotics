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
/*  F i l e               &F: eps_hw_edds.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS EDDS Lower Layer Adaption                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20060
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */


/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Funcs               */ 
#include <eps_trc.h>           /* Tracing                                  */
#include <eps_tasks.h>         /* EPS Thread Api                           */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_plf.h>           /* EPS PCI Interface functions              */
#include <eps_mem.h>           /* EPS Memory Management                    */
#include <eps_hw_edds.h>       /* Own header*/

#if (PSI_CFG_USE_EDDS == 1) 

#if defined (EDDS_CFG_HW_INTEL)
/**
 * Get the MAC Adress for an intel board
 * 
 * @param [in] uVendorId		 Vendor ID, e.g. Intel
 * @param [in] pBase			 Basepointer to seek for a MAC address
 * @param [out] pMac			 ptr to sys device in
 * @return LSA_TRUE              a MAC was read from the board
 * @return LSA_FALSE             Not supported
 */
LSA_BOOL eps_intelLL_get_mac_addr(LSA_UINT16 uVendorId, LSA_UINT8* pBase, LSA_UINT8* pMac)
{
	LSA_UINT32 tmp;

	EPS_ASSERT(pMac != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uVendorId != 0);

	pMac[0] = 0;
	pMac[1] = 0;
	pMac[2] = 0;
	pMac[3] = 0;
	pMac[4] = 0;
	pMac[5] = 0;

	if (uVendorId == EPS_INTEL_PCI_VENDOR_ID)
	{
		tmp = eps_edds_basereg_reg32(pBase, INTEL_RAH_0_15(0));
		pMac[4] = (tmp >> 0) & 0xFF;
		pMac[5] = (tmp >> 8) & 0xFF;
		tmp = eps_edds_basereg_reg32(pBase, INTEL_RAL_0_15(0));
		pMac[0] = (tmp >> 0) & 0xFF;
		pMac[1] = (tmp >> 8) & 0xFF;
		pMac[2] = (tmp >> 16) & 0xFF;
		pMac[3] = (tmp >> 24) & 0xFF;

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/**
 * intel LL open device for edds
 * 
 * @param pEddsIn 				pointer to edds parameter structure
 * @param pHwIn					pointer to strucute with hardware configuration
 * @param pSysDevIn				ptr to sys device in
 * @param pRegBaseIn			ptr to BaseRegister
 * @param hd_idIn				index of hd
 * @param uVendorIdIn
 * @param uDeviceIdIn
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16 eps_intelLL_open(EPS_EDDS_INTEL_LL_PTR_TYPE  pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		pHwIn,
							EPS_SYS_PTR_TYPE			pSysDevIn,							
							LSA_UINT8* 					pRegBaseIn,
							LSA_UINT16 					hd_idIn,
							LSA_UINT16					uVendorIdIn,
							LSA_UINT16					uDeviceIdIn)
{
	EDDS_LL_TABLE_TYPE * pLLTable;
	LSA_UINT16           uPortMacEnd;
	LSA_UINT16           i;
	LSA_UINT16           result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pEddsIn != LSA_NULL); 
	EPS_ASSERT(pHwIn != LSA_NULL); 
	EPS_ASSERT(pSysDevIn != LSA_NULL); 
	EPS_ASSERT(pRegBaseIn != LSA_NULL); 

	if (pRegBaseIn != LSA_NULL)
	{
		/* Fill EDDS LL */

		pLLTable = &pEddsIn->tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = INTEL_LL_OPEN;
		pLLTable->setup                     = INTEL_LL_SETUP;
		pLLTable->shutdown                  = INTEL_LL_SHUTDOWN;
		pLLTable->close                     = INTEL_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = INTEL_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = INTEL_LL_SEND_STS;

		#ifdef EDDS_CFG_SEND_BLOCKED
		pLLTable->triggerSend               = INTEL_LL_SEND_TRIGGER;
		#endif

		pLLTable->reinitTX                  = INTEL_LL_REINIT_TX;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = INTEL_LL_RECV;
		pLLTable->provideReceiveBuffer      = INTEL_LL_RECV_PROVIDE;
    
		/* Information functions */
		pLLTable->getLinkStateSetup         = INTEL_LL_GET_LINK_STATE_SETUP;
		pLLTable->getLinkState              = INTEL_LL_GET_LINK_STATE;
		pLLTable->checkMAU                  = INTEL_LL_CHECK_MAUTYPE;
		pLLTable->getStatistics             = INTEL_LL_GET_STATS;
		pLLTable->getCapabilities           = INTEL_LL_GET_CAPS;
		/* MAC address management functions */
		pLLTable->getMACAddr                = INTEL_LL_GET_MAC_ADDR;
		pLLTable->enableMC                  = INTEL_LL_MC_ENABLE;
		pLLTable->disableMC                 = INTEL_LL_MC_DISABLE;

        #if defined (EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        pLLTable->recurringTask             = INTEL_LL_RECURRING_TASK;
        #elif !defined (EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		/* Interrupt functions */
		pLLTable->getPendingInterrupt       = INTEL_LL_INTERRUPT_POLL;
		pLLTable->interruptRX               = INTEL_LL_INTERRUPT_RX;
		pLLTable->interruptTX               = INTEL_LL_INTERRUPT_TX;
		pLLTable->interruptPhy              = INTEL_LL_INTERRUPT_PHY;
		#if !defined (LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT)
		pLLTable->interrupt                 = INTEL_LL_INTERRUPT;
		#endif //LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
        #endif //EDDS_CFG_CYCLIC_SCHEDULING_MODE

		/* Control functions */
		pLLTable->setLinkState              = INTEL_LL_SET_LINK_STATE;
		pLLTable->backupPortLEDMode         = INTEL_LL_LED_BACKUP_MODE;
		pLLTable->restorePortLEDMode        = INTEL_LL_LED_RESTORE_MODE;
		pLLTable->setPortLED                = INTEL_LL_LED_SET_MODE;

		#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
		pLLTable->enableRX                  = INTEL_LL_RX_OVERLOAD_RX_ON;
		pLLTable->disableRX                 = INTEL_LL_RX_OVERLOAD_RX_OFF;
		#endif

		/* Switch functions */
		#ifdef LLIF_CFG_SWITCH_SUPPORT
		pLLTable->setSwitchPortState        = LSA_NULL; //= INTEL_LL_SWITCH_SET_PORT_STATE;
		pLLTable->controlSwitchMulticastFwd = LSA_NULL; //= INTEL_LL_SWITCH_MULTICAST_FWD_CTRL;
		pLLTable->flushSwitchFilteringDB    = LSA_NULL; //= INTEL_LL_SWITCH_FLUSH_FILTERING_DB;
		pLLTable->setSwitchPortRedundant    = LSA_NULL; //= INTEL_LL_SWITCH_SET_PORT_REDUNDANT;
		pLLTable->setSwitchBridgeMode       = LSA_NULL; //= INTEL_LL_SWITCH_SET_BRIDGE_MODE;
        #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
		pLLTable->getSwitchDroppedFrames    = LSA_NULL; //= INTEL_LL_SWITCH_DROP_CNT;
        #endif
		#endif

	    // EDDS set arp filter function
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter = LSA_NULL; //(LaM) ToDo CHECK!
		#endif
		
		pLLTable->timeout           		= INTEL_LL_TIMEOUT;

		// Fill Device Info

		EPS_BOARD_INFO_PTR_TYPE pBoardInfo = &pHwIn->EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pSysDevIn->hd_nr       = hd_idIn;
		pSysDevIn->if_nr       = 1;	// don't care
		pSysDevIn->pnio_if_nr  = 0;	// don't care
		pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = pSysDevIn;

		eps_intelLL_get_mac_addr(uVendorIdIn, pRegBaseIn, pBoardInfo->if_mac[0].mac);

		// Init User to HW port mapping (used PSI GET HD PARAMS)
		eps_hw_init_board_port_param(pBoardInfo);

		// Port specific setup
		pBoardInfo->nr_of_ports = 1;

		// Port MAC - Ersatz Mac Adressen laut Norm bei Standard Hw
		for (i = 0; i < pBoardInfo->nr_of_ports; i++)
		{
			uPortMacEnd = 0x3840 + i;

			pBoardInfo->port_mac[i+1].mac[0] = 0x08;
			pBoardInfo->port_mac[i+1].mac[1] = 0x00;
			pBoardInfo->port_mac[i+1].mac[2] = 0x06;
			pBoardInfo->port_mac[i+1].mac[3] = 0x9D;
			pBoardInfo->port_mac[i+1].mac[4] = (LSA_UINT8)(uPortMacEnd>>8);
			pBoardInfo->port_mac[i+1].mac[5] = (LSA_UINT8)(uPortMacEnd&0xff);

			// setup portmapping (=1:1)
			pBoardInfo->port_map[0][i+1].hw_phy_nr  = i;
			pBoardInfo->port_map[0][i+1].hw_port_id = i+1;
		}

		// Prepare process image settings (local mem is used)
		// Note: we use the full size of KRAM, real size is calculated in PSI
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;

		pEddsIn->sIntelLLParameter.DeviceID               = (LSA_UINT16)uDeviceIdIn;
		pEddsIn->sIntelLLParameter.pRegBaseAddr           = (LSA_VOID*)pRegBaseIn; /* Register Base Address on Intel LL */
		pEddsIn->sIntelLLParameter.Disable1000MBitSupport = LSA_TRUE;
		pBoardInfo->edds.ll_params = (LSA_VOID*)(&pEddsIn->sIntelLLParameter);

		/* Fill other Params */
		pHwIn->asic_type = EPS_PNDEV_ASIC_INTEL_XX;
		pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	return result;
}
#endif

#if defined (EDDS_CFG_HW_KSZ88XX)
/**
 * Get the MAC Adress for an micrel board
 * 
 * @param [in] uVendorId		 Vendor ID, e.g. Micrel
 * @param [in] pBase			 Basepointer to seek for a MAC address
 * @param [out] pMac			 ptr to sys device in
 * @return LSA_TRUE              a MAC was read from the board
 * @return LSA_FALSE             Not supported
 */
LSA_BOOL 	eps_ksz88xx_get_mac_addr(	LSA_UINT16	uVendorId, 
										LSA_UINT8* 	pBase, 
										LSA_UINT8* 	pMac)
{
	EPS_ASSERT(pMac != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uVendorId != 0);

	pMac[0] = 0;
	pMac[1] = 0;
	pMac[2] = 0;
	pMac[3] = 0;
	pMac[4] = 0;
	pMac[5] = 0;

	if (uVendorId == EPS_KSZ88XX_PCI_VENDOR_ID) 
	{
		LSA_UINT16 tmp;	
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARL_0_15(0));
		pMac[4] = (tmp>>8)&0xFF; 
		pMac[5] = tmp & 0xFF;
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARM_0_15(0));
		pMac[2] = (tmp>>8)&0xFF; 
		pMac[3] = tmp & 0xFF;
		tmp = eps_edds_basereg_reg16(pBase, KSZ88XX_MARH_0_15(0));
		pMac[0] = (tmp>>8)&0xFF; 
		pMac[1] = tmp & 0xFF;

		return LSA_TRUE;
	}

	return LSA_FALSE;

}											


/**
 * ksz88xx LL open device for edds
 * 
 * @param pEddsIn 				pointer to edds parameter structure
 * @param pHwIn					pointer to strucute with hardware configuration
 * @param pSysDevIn				ptr to sys device in
 * @param pRegBaseIn			ptr to BaseRegister
 * @param hd_idIn				index of hd
 * @param uVendorIdIn
 * @param uDeviceIdIn
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16 eps_ksz88xx_open(EPS_EDDS_KSZ88XX_LL_PTR_TYPE  pEddsIn,
							EPS_PNDEV_HW_PTR_TYPE		  pHwIn,
							EPS_SYS_PTR_TYPE			  pSysDevIn,							
							LSA_UINT8* 					  pRegBaseIn,
							LSA_UINT16 					  hd_idIn,
							LSA_UINT16					  uVendorIdIn,
							LSA_UINT16					  uDeviceIdIn)
{
	EDDS_LL_TABLE_TYPE * pLLTable;
	LSA_UINT16           uPortMacEnd;
	LSA_UINT16           i;
	LSA_UINT16           result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pEddsIn != LSA_NULL); 
	EPS_ASSERT(pHwIn != LSA_NULL); 
	EPS_ASSERT(pSysDevIn != LSA_NULL); 
	EPS_ASSERT(pRegBaseIn != LSA_NULL); 

	if (pRegBaseIn != LSA_NULL)
	{
		/* Fill EDDS LL */
		pLLTable = &pEddsIn->tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = KSZ88XX_LL_OPEN;
		pLLTable->setup                     = KSZ88XX_LL_SETUP;
		pLLTable->shutdown                  = KSZ88XX_LL_SHUTDOWN;
		pLLTable->close                     = KSZ88XX_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = KSZ88XX_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = KSZ88XX_LL_SEND_STS;

		#ifdef EDDS_CFG_SEND_BLOCKED
		pLLTable->triggerSend               = KSZ88XX_LL_SEND_TRIGGER;
		#endif

		pLLTable->reinitTX                  = KSZ88XX_LL_REINIT_TX;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = KSZ88XX_LL_RECV;
		pLLTable->provideReceiveBuffer      = KSZ88XX_LL_RECV_PROVIDE;
    
		/* Information functions */
		pLLTable->getLinkStateSetup         = KSZ88XX_LL_GET_LINK_STATE_SETUP;
		pLLTable->getLinkState              = KSZ88XX_LL_GET_LINK_STATE;
		pLLTable->checkMAU                  = KSZ88XX_LL_CHECK_MAUTYPE;
		pLLTable->getStatistics             = KSZ88XX_LL_GET_STATS;
		pLLTable->getCapabilities           = KSZ88XX_LL_GET_CAPS;
		/* MAC address management functions */
		pLLTable->getMACAddr                = KSZ88XX_LL_GET_MAC_ADDR;
		pLLTable->enableMC                  = KSZ88XX_LL_MC_ENABLE;
		pLLTable->disableMC                 = KSZ88XX_LL_MC_DISABLE;

        #if defined (EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        pLLTable->recurringTask             = KSZ88XX_LL_RECURRING_TASK;
        #elif !defined (EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		/* Interrupt functions */
		pLLTable->getPendingInterrupt       = KSZ88XX_LL_INTERRUPT_POLL;
		pLLTable->interruptRX               = KSZ88XX_LL_INTERRUPT_RX;
		pLLTable->interruptTX               = KSZ88XX_LL_INTERRUPT_TX;
		pLLTable->interruptPhy              = KSZ88XX_LL_INTERRUPT_PHY;
		#if !defined (LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT)
		pLLTable->interrupt                 = KSZ88XX_LL_INTERRUPT;
		#endif //LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
        #endif //EDDS_CFG_CYCLIC_SCHEDULING_MODE

		/* Control functions */
		pLLTable->setLinkState              = KSZ88XX_LL_SET_LINK_STATE;
		pLLTable->backupPortLEDMode         = KSZ88XX_LL_LED_BACKUP_MODE;
		pLLTable->restorePortLEDMode        = KSZ88XX_LL_LED_RESTORE_MODE;
		pLLTable->setPortLED                = KSZ88XX_LL_LED_SET_MODE;

		#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
		pLLTable->enableRX                  = KSZ88XX_LL_RX_OVERLOAD_RX_ON;
		pLLTable->disableRX                 = KSZ88XX_LL_RX_OVERLOAD_RX_OFF;
		#endif

		/* Switch functions */
		#ifdef LLIF_CFG_SWITCH_SUPPORT
		pLLTable->setSwitchPortState        = KSZ88XX_LL_SWITCH_SET_PORT_STATE;
		pLLTable->controlSwitchMulticastFwd = KSZ88XX_LL_SWITCH_MULTICAST_FWD_CTRL;
		pLLTable->flushSwitchFilteringDB    = KSZ88XX_LL_SWITCH_FLUSH_FILTERING_DB;
		pLLTable->setSwitchPortRedundant    = KSZ88XX_LL_SWITCH_SET_PORT_REDUNDANT;
		pLLTable->setSwitchBridgeMode       = KSZ88XX_LL_SWITCH_SET_BRIDGE_MODE;
        #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
		pLLTable->getSwitchDroppedFrames    = KSZ88XX_LL_SWITCH_DROP_CNT;
        #endif
		#endif

		/* EDDS set arp filter function */
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter 				= LSA_NULL; 
		#endif
		
		pLLTable->timeout           		= LSA_NULL;

		
		/* Fill Device Info */
		EPS_BOARD_INFO_PTR_TYPE pBoardInfo = &pHwIn->EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pSysDevIn->hd_nr       = hd_idIn;
		pSysDevIn->if_nr       = 1;	// don't care
		pSysDevIn->pnio_if_nr  = 0;	// don't care
		pSysDevIn->edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = pSysDevIn;

		// Get MAC Address from Hardware
		eps_ksz88xx_get_mac_addr(uVendorIdIn, pRegBaseIn, pBoardInfo->if_mac[0].mac);

		// Init User to HW port mapping (used PSI GET HD PARAMS)
		eps_hw_init_board_port_param(pBoardInfo);

		// Port specific setup
		if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841 == uDeviceIdIn)
		{
			pBoardInfo->nr_of_ports = 1; 
		}
		else if (EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842 == uDeviceIdIn)
		{
			pBoardInfo->nr_of_ports = 2; 
		}

		// Port MAC - Ersatz Mac Adressen laut Norm bei Standard Hw
		for (i = 0; i < pBoardInfo->nr_of_ports; i++)
		{
			uPortMacEnd = 0x3940 + i;

			pBoardInfo->port_mac[i+1].mac[0] = 0x08;
			pBoardInfo->port_mac[i+1].mac[1] = 0x00;
			pBoardInfo->port_mac[i+1].mac[2] = 0x06;
			pBoardInfo->port_mac[i+1].mac[3] = 0x9D;
			pBoardInfo->port_mac[i+1].mac[4] = (LSA_UINT8)(uPortMacEnd>>8);
			pBoardInfo->port_mac[i+1].mac[5] = (LSA_UINT8)(uPortMacEnd&0xff);

			// setup portmapping (=1:1)
			pBoardInfo->port_map[0][i+1].hw_phy_nr  = i;
			pBoardInfo->port_map[0][i+1].hw_port_id = i+1;
		}

		// Prepare process image settings (local mem is used)
		pBoardInfo->pi_mem.base_ptr = 0;
		pBoardInfo->pi_mem.phy_addr = 0;
		pBoardInfo->pi_mem.size     = 0;

		// setup HIF buffer
		pBoardInfo->hif_mem.base_ptr = 0;
		pBoardInfo->hif_mem.phy_addr = 0;
		pBoardInfo->hif_mem.size     = 0;

		// add the LL settings
		pBoardInfo->edds.is_valid          = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pEddsIn->hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)&pEddsIn->tLLFuncs;
		pBoardInfo->edds.ll_params 		   = (LSA_VOID*)pRegBaseIn;

		/* Fill other Params */
		pHwIn->asic_type = EPS_PNDEV_ASIC_KSZ88XX;
		pHwIn->EpsBoardInfo.board_type = EPS_PNDEV_BOARD_KSZ88XX;
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	return result;
}
#endif	

#endif //(PSI_CFG_USE_EDDS == 1)
