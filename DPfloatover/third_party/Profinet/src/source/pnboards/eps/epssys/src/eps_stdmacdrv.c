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
/*  F i l e               &F: eps_stdmacdriver.c                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.30         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-05-13                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Standard Ethernet Mac Interface Adaption                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20036
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>           /* Types / Prototypes / Funcs               */ 
#include <eps_trc.h>           /* Tracing                                  */
#include <eps_tasks.h>         /* EPS Thread Api                           */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_hw_edds.h>	   /* IntelLL					               */
#include <eps_stdmacdrv.h>     /* Own Header                               */
#include <eps_mem.h>           /* EPS Memory Management                    */

#if ( (PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL) )
/* - Function Forward Declaration ----------------------------------------------------------------- */

LSA_VOID    eps_stdmacdrv_isr_cbf           (LSA_UINT32 uParam, LSA_VOID* pArgs);
LSA_VOID    eps_stdmacdrv_uninstall         (LSA_VOID);
LSA_UINT16  eps_stdmacdrv_enable_interrupt  (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16  eps_stdmacdrv_disable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt);
LSA_UINT16  eps_stdmacdrv_set_gpio          (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16  eps_stdmacdrv_clear_gpio        (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16  eps_stdmacdrv_timer_ctrl_start  (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16  eps_stdmacdrv_timer_ctrl_stop   (struct eps_pndev_hw_tag* pHwInstIn);
LSA_UINT16	eps_stdmacdrv_read_trace_data	(EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
LSA_UINT16	eps_stdmacdrv_write_trace_data	(EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT8* ptr, LSA_UINT32 offset, LSA_UINT32 size);



/* - Global Data ---------------------------------------------------------------------------------- */
EPS_STDMACDRV_STORE_TYPE g_EpsStdMacDrv;
EPS_STDMACDRV_STORE_PTR_TYPE g_pEpsStdMacDrv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#ifdef EPS_STDMACDRV_PRINTF_FOUND_BOARDS

/**
 * get mac addr of intel board for printf
 * 
 * @param pDevice	ptr to device structure
 * @param pMac		ptr where the mac address will be saved
 * @return
 */
static LSA_BOOL eps_stdmacdrv_get_mac_addr_intel_for_printf(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pDevice, LSA_UINT8* pMac)
{
	LSA_UINT8* pBase;

	EPS_ASSERT(pMac != LSA_NULL);
	EPS_ASSERT(pDevice != LSA_NULL);

	pMac[0] = 0;
	pMac[1] = 0;
	pMac[2] = 0;
	pMac[3] = 0;
	pMac[4] = 0;
	pMac[5] = 0;

	if( ( pDevice->uVendorId == EPS_INTEL_PCI_VENDOR_ID) && 
		( pDevice->info.bValid == LSA_TRUE) )
	{
		if(pDevice->info.Bar[0].BarSize != 0)
		{
			EPS_PLF_PCI_MAP_MEMORY(&pBase, (pDevice->info.Bar[0].Bar & 0xfffffff0), pDevice->info.Bar[0].BarSize);
			EPS_ASSERT(pBase != LSA_NULL);

			return eps_intelLL_get_mac_addr(pDevice->uVendorId, pBase, pMac);
		}
	}

	return LSA_FALSE;
}

/**
 * printf bar info
 * 
 * @param pDevice	ptr to device structure
 * @return
 */
static LSA_VOID eps_stdmacdrv_printf_bar_info(EPS_PLF_PCI_CONFIG_SPACE_PTR_TYPE pDevice)
{
	LSA_UINT32 uBarNr = 0;
	LSA_UINT32 uBarSize;
	LSA_UINT32 uBar;
	LSA_UINT32 bIOSpace;
	LSA_UINT32 b20Bit;
	LSA_UINT32 b32Bit;
	LSA_UINT32 b64Bit;
	LSA_UINT32 bPrefetch;

	while(uBarNr<6)
	{
		uBarSize = pDevice->info.Bar[uBarNr].BarSize;
		uBar     = pDevice->info.Bar[uBarNr].Bar;

	    bIOSpace  = 0;
        b20Bit    = 0;
        b32Bit    = 0;
        b64Bit    = 0;
        bPrefetch = 0;

		if(uBarSize != 0) 
		{
			bIOSpace  = (uBar & 1);

			if(bIOSpace == 0)
			{
				bPrefetch = (uBar >> 3) & 1;
				if( ( (uBar >> 1) & 3) == 0 ) b32Bit = 1;
				if( ( (uBar >> 1) & 3) == 1 ) b20Bit = 1;
				if( ( (uBar >> 1) & 3) == 2 ) b64Bit = 1;
			}

			printf("    Bar%d: 0x%8x Size: 0x%8x (Masked 0x%8x)", (int)uBarNr, (unsigned int)uBar, (unsigned int)uBarSize, (unsigned int)pDevice->info.Bar[uBarNr].BarMaskedVal);
			printf(" - ");
			if (bPrefetch) printf("(Prefetch) ");
			if (b20Bit)    printf("(20Bit) ");
			if (b32Bit)    printf("(32Bit) ");
			if (b64Bit)    printf("(64Bit) ");
			if (bIOSpace)  printf("(IOSpace) ");
			if (!bIOSpace) printf("(MemSpace) ");

			printf("\r\n");
		}

		if(b64Bit) uBarNr++; /* Skip next bar as address vals only */

		uBarNr++;
	}
}

/**
 * printf device list
 * 
 * @param uVendorId		Value of VendorId
 * @param uDeviceId 	Value of DeviceId
 * @return
 */
static LSA_VOID eps_stdmacdrv_printf_devices(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId)
{
	LSA_UINT32 i;
	LSA_BOOL bMacFound;
	LSA_UINT8 Mac[6];
	unsigned short cntDevices;
	EPS_PLF_PCI_CONFIG_SPACE_TYPE sDevices[EPS_CFG_STDMACDRV_MAX_BOARDS];

	eps_memset(sDevices, 0, sizeof(sDevices));
	EPS_PLF_PCI_GET_DEVICES(uVendorId, uDeviceId, &sDevices[0], EPS_CFG_STDMACDRV_MAX_BOARDS, &cntDevices);

	for(i=0; i<cntDevices; i++)
	{
		printf("\r\nPCI bus %d, device %d, function %d - ", (int)sDevices[i].uBusNr, (int)sDevices[i].uDeviceNr, (int)sDevices[i].uFunctionNr);
				
		bMacFound = eps_stdmacdrv_get_mac_addr_intel_for_printf(&sDevices[i], Mac);

		switch(uDeviceId)
		{
			//case DevId_Intel_82540em:
			//	printf("Intel 82540em                                 ");
			//	break;
			case DevId_Intel_IG82541PI:
				printf("Intel IG82541PI                               ");
				break;
			case DevId_Intel_IG82540EM:
				printf("Intel 82454em                                 ");
				break;
			case DevId_Intel_GBEPCI_GT_Adapter:
				printf("Intel GBE PCI GT Adapter                      ");
				break;
			case DevId_Intel_PCIe_CT_Adapter_82574L_Gigabit_Network:
				printf("Intel PCIe CT Adapter 82574L Gigabit Network  ");
				break;
			case DevId_Intel_Onboard_82567LM_3_Gigabit_Network:
				printf("Intel Onboard 82567LM-3 Gigabit Network       ");
				break;
			case DevId_Intel_Springville:
				printf("Intel Springville                             ");
				break;
		}

		/* Details */
		if(sDevices[i].info.bValid)
		{
			printf("\r\n");
			printf("    Ven: 0x%8x Dev: 0x%8x", uVendorId, uDeviceId);
			//printf("    Ven: 0x%8x Dev: 0x%8x\r\n", uVendorId, uDeviceId);
			//printf("    Status: 0x%8x - IRQ Pending (%d) | Supports: MSI(%d)/MSI-X(%d) | IO-Apic Line: (%d)\r\n", (unsigned int)sDevices[i].info.Status, (int)((sDevices[i].info.Status & 0x8)>>3), sDevices[i].info.bMSISupport,sDevices[i].info.bMSIXSupport, (int)sDevices[i].info.IOApicLine);
			//printf("    Command: 0x%8x - IRQ Disable (%d)\r\n", (unsigned int)sDevices[i].info.Command, (int)((sDevices[i].info.Command & 0x400)>>10));

			if(bMacFound) 
				printf("    Mac Addr: %02x:%02x:%02x:%02x:%02x:%02x\r\n",Mac[0],Mac[1],Mac[2],Mac[3],Mac[4],Mac[5]);
			//eps_stdmacdrv_printf_bar_info(&(sDevices[i]));
			//printf("\r\n");
		}

		/* Sleep a bit between the printfs to get the buffer out */
		eps_tasks_sleep(1);
	}
}

/**
 * printf found intel boards
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_stdmacdrv_printf_found_boards(LSA_VOID)
{
	printf("\r\nEPS StdMacDrv (");

	switch(EPS_STDMACDRV_ISR_MODE)
	{
		case EPS_ISR_MODE_ISR:
			printf("ISR");
			break;
		case EPS_ISR_MODE_POLL:
			printf("Poll");
			break;
		case EPS_ISR_MODE_MIXED:
			printf("Msi/Poll");
			break;
	}

	printf(" Mode) Found Network Boards:\r\n");
	printf("----------------------------------------");

	//eps_stdmacdrv_printf_devices(EPS_STDMACDRV_INTEL_PCI_VENDOR_ID, DevId_Intel_82540em);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82541PI);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82540EM);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_GBEPCI_GT_Adapter);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_PCIe_CT_Adapter_82574L_Gigabit_Network);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Onboard_82567LM_3_Gigabit_Network);
	eps_stdmacdrv_printf_devices(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Springville);

	printf("----------------------------------------\r\n");
}
#endif

#ifdef EPS_STDMACDRV_PRINTF_OPEN_CLOSE_INFO
/**
 * print information, once a board has been closed or opened
 * 
 * @param bOpen		boolean True, if open action was done
 * @param pBoard	ptr to board structure
 * @return LSA_VOID
 */
static LSA_VOID eps_stdmacdrv_printf_openclose_board(LSA_BOOL bOpen, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_PNDEV_LOCATION_PTR_TYPE pLoc = &pBoard->sDeviceInfo.sLoc;

	if(bOpen == LSA_TRUE)
	{
		printf("\r\nEPS StdMacDrv Board Opened: PCI bus %d, device %d, function %d", (int)pLoc->uBusNr, (int)pLoc->uDeviceNr, (int)pLoc->uFctNr );
	}
	else
	{
		printf("\r\nEPS StdMacDrv Board Closed: PCI bus %d, device %d, function %d", (int)pLoc->uBusNr, (int)pLoc->uDeviceNr, (int)pLoc->uFctNr );
	}

#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_POLL)
	printf(" (ISR POLL MODE)");
#elif (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
	printf(" (MSI ISR if supported)");
#else
	printf(" (ISR MODE)");
#endif

	printf("\r\n");
}
#endif

/**
 * undo init critical section
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_stdmacdrv_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsStdMacDrv->hEnterExit);
}

/**
 * init critical section
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_stdmacdrv_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsStdMacDrv->hEnterExit, LSA_FALSE);
}

/**
 * enter critical section
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_stdmacdrv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsStdMacDrv->hEnterExit);
}

/**
 * exit critical section
 * 
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_stdmacdrv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsStdMacDrv->hEnterExit);
}

/**
 * alloc board entry
 * - create new board
 * - initialize board structure 
 * 
 * @param LSA_VOID
 * @return pBoard
 */
static EPS_STDMACDRV_BOARD_PTR_TYPE eps_stdmacdrv_alloc_board(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	for(i=0; i<EPS_CFG_STDMACDRV_MAX_BOARDS; i++)
	{
		pBoard = &g_pEpsStdMacDrv->board[i];

		if(pBoard->bUsed == LSA_FALSE)
		{
			pBoard->sHw.hDevice        = (LSA_VOID*)pBoard;
			pBoard->sHw.EnableIsr      = eps_stdmacdrv_enable_interrupt;
			pBoard->sHw.DisableIsr     = eps_stdmacdrv_disable_interrupt;
			pBoard->sHw.SetGpio        = eps_stdmacdrv_set_gpio;
			pBoard->sHw.ClearGpio      = eps_stdmacdrv_clear_gpio;
			pBoard->sHw.TimerCtrlStart = eps_stdmacdrv_timer_ctrl_start;
			pBoard->sHw.TimerCtrlStop  = eps_stdmacdrv_timer_ctrl_stop;
			pBoard->sHw.ReadTraceData  = eps_stdmacdrv_read_trace_data;
			pBoard->sHw.WriteTraceData = eps_stdmacdrv_write_trace_data;
			pBoard->pBackRef           = g_pEpsStdMacDrv;
			pBoard->bUsed              = LSA_TRUE;

			return pBoard;
		}
	}

	return pBoard;
}

/**
 * register device infos 
 * 
 * @param uVendorId Value of VendorId
 * @param uDeviceId	Value of DeviceId
 * @return LSA_VOID
 */
static LSA_VOID eps_stdmacdrv_register_device_infos_2(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId)
{
	LSA_UINT32 i;
	unsigned short cntDevices;
	EPS_PLF_PCI_CONFIG_SPACE_TYPE sDevices[EPS_CFG_STDMACDRV_MAX_BOARDS];
	EPS_PNDEV_IF_DEV_INFO_TYPE sPnDevInfo;

	eps_memset(sDevices, 0, sizeof(sDevices));
	EPS_PLF_PCI_GET_DEVICES(uVendorId, uDeviceId, &sDevices[0], EPS_CFG_STDMACDRV_MAX_BOARDS, &cntDevices);

	for(i=0; i<cntDevices; i++)
	{
		sPnDevInfo.uPciAddrValid = 1;
		sPnDevInfo.uPciBusNr    = (LSA_UINT16)sDevices[i].uBusNr;
		sPnDevInfo.uPciDeviceNr = (LSA_UINT16)sDevices[i].uDeviceNr;
		sPnDevInfo.uPciFuncNr   = (LSA_UINT16)sDevices[i].uFunctionNr;
		sPnDevInfo.uEddType     = LSA_COMP_ID_EDDS;

		sPnDevInfo.uMacAddrValid = 0;

		eps_pndev_if_register_device(&sPnDevInfo);
	}
}

/**
 * register device infos 
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_stdmacdrv_register_device_infos(LSA_VOID)
{
	//eps_stdmacdrv_register_device_infos_2(EPS_STDMACDRV_INTEL_PCI_VENDOR_ID, DevId_Intel_82540em);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82541PI);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82540EM);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_GBEPCI_GT_Adapter);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_PCIe_CT_Adapter_82574L_Gigabit_Network);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Onboard_82567LM_3_Gigabit_Network);
	eps_stdmacdrv_register_device_infos_2(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Springville);
}

/**
 * get device for adress
 * 
 * @param uVendorId 		Value of VendorId
 * @param uDeviceId			Value of DeviceId
 * @param pLoc				ptr to enum with LoctionType
 * @param pDeviceInfo		ptr to device structure
 * @return LSA_TRUE
 */
static LSA_BOOL eps_stdmacdrv_get_device_for_address(LSA_UINT16 uVendorId, LSA_UINT16 uDeviceId, EPS_PNDEV_LOCATION_PTR_TYPE pLoc, EPS_STDMACDRV_DEVICE_INFO_PTR_TYPE pDeviceInfo)
{
	LSA_UINT32 i;
	LSA_UINT16 cntDevices;
    //LSA_INT retval;
	//PCI_CONFIG_SPACE devices[EPS_CFG_STDMACDRV_MAX_BOARDS];
	EPS_PLF_PCI_CONFIG_SPACE_TYPE sDevices[EPS_CFG_STDMACDRV_MAX_BOARDS];

	pDeviceInfo->pBase = LSA_NULL;

	//eps_memset(devices,0,sizeof(devices));
    //retval = RmPciSearchFunction(uVendorId, uDeviceId, &devices[0], EPS_CFG_STDMACDRV_MAX_BOARDS, &cntDevices);

	eps_memset(sDevices,0,sizeof(sDevices));
	EPS_PLF_PCI_GET_DEVICES(uVendorId, uDeviceId, &sDevices[0], EPS_CFG_STDMACDRV_MAX_BOARDS, &cntDevices);

	if(cntDevices > 0)
	{
		for(i=0; i<cntDevices; i++)
		{
			//if( (devices[i].PciBusNum == pLoc->uBusNr)       &&
			//	(devices[i].PciDeviceNum == pLoc->uDeviceNr) &&
			//	(devices[i].PciFunctionNum == pLoc->uFctNr) )
			if( (sDevices[i].uBusNr == pLoc->uBusNr)       &&
				(sDevices[i].uDeviceNr == pLoc->uDeviceNr) &&
				(sDevices[i].uFunctionNr == pLoc->uFctNr) )
			{
				/* Device Found! => Get Base Pointer of the Registers */
				//pDeviceInfo->pBase     = (LSA_UINT8*)(devices[i].BaseAdr0 & ~0xf); // (LaM)ToDo Check Virtual/Physical
				pDeviceInfo->pBase        = sDevices[i].pBase0;
				pDeviceInfo->uSize        = sDevices[i].uSize0;
				pDeviceInfo->eDeviceId    = (EPS_STDMACDRV_INTEL_PCI_DEVICE_IDS)uDeviceId;
				pDeviceInfo->sConfigSpace = sDevices[i];

				return LSA_TRUE;
			}
		}
	}

	return LSA_FALSE;
}

/**
 * look up if intel hardware exists and open it
 * 
 * @param pLoc				ptr to enum with LoctionType
 * @param pBoard			ptr to current board structure
 * @return pUserSpaceBase	ptr to UserSpace register 
 */
static LSA_UINT8* eps_stdmacdrv_open_hw(EPS_PNDEV_LOCATION_PTR_TYPE pLoc, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_BOOL bFound = LSA_FALSE;
	LSA_UINT8* pUserSpaceBase = LSA_NULL;
	LSA_BOOL bMmapResult = LSA_FALSE;

	pBoard->sDeviceInfo.sLoc = *pLoc;

	bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82540EM, pLoc, &pBoard->sDeviceInfo);
	
	if(bFound != LSA_TRUE)
	{
		bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_IG82541PI, pLoc, &pBoard->sDeviceInfo);
	}
	if(bFound != LSA_TRUE)
	{
		bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_GBEPCI_GT_Adapter, pLoc, &pBoard->sDeviceInfo);
	}
	if(bFound != LSA_TRUE)
	{
		bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_PCIe_CT_Adapter_82574L_Gigabit_Network, pLoc, &pBoard->sDeviceInfo);
	}
	if(bFound != LSA_TRUE)
	{
		bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Onboard_82567LM_3_Gigabit_Network, pLoc, &pBoard->sDeviceInfo);
	}
	if(bFound != LSA_TRUE)
	{
		bFound = eps_stdmacdrv_get_device_for_address(EPS_INTEL_PCI_VENDOR_ID, DevId_Intel_Springville, pLoc, &pBoard->sDeviceInfo);
	}
	//if(bFound != LSA_TRUE)
	//{
	//	  bFound = eps_stdmacdrv_get_device_for_address(EPS_STDMACDRV_INTEL_PCI_VENDOR_ID, DevId_Intel_82540em, pLoc, &pBoard->sDeviceInfo);
	//}

	if(bFound == LSA_TRUE)
	{
		/* Map device base address to user space memory */

		bMmapResult = EPS_PLF_PCI_MAP_MEMORY(&pUserSpaceBase, (LSA_UINT32)pBoard->sDeviceInfo.pBase, pBoard->sDeviceInfo.uSize);

		EPS_ASSERT(bMmapResult == LSA_TRUE);
	}

	return pUserSpaceBase;
}

/**
 * free board 
 * 
 * @param pBoard	ptr to current board structure
 * @return
 */
static LSA_VOID eps_stdmacdrv_free_board(EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	pBoard->bUsed = LSA_FALSE;
}

#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_POLL) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
/**
 * register board for interrupt polling
 * 
 * @param pPollIsr			ptr to PollIsr parameter
 * @param pBoard 			ptr to corresponding board structure
 * @param uIntSrc			enum with IntSrc information, (here EPS_PNDEV_ISR_PN_GATHERED or EPS_PNDEV_ISR_PN_NRT)
 * @param bCreateThread		True if Thread should be created
 * @return 					EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_stdmacdrv_register_board_for_polling(EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{
	LSA_UINT32 i;

	*bCreateThread = LSA_FALSE;

	if(pPollIsr->uCntUsers >= EPS_CFG_STDMACDRV_MAX_BOARDS)
	{
		return EPS_PNDEV_RET_ERR; /* No More Resources */
	}

	for(i=0; i<EPS_CFG_STDMACDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
		}
	}

	for(i=0; i<EPS_CFG_STDMACDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == LSA_NULL)
		{
			pPollIsr->pBoard[i]  = pBoard;  /* Register Board for polling */
			pPollIsr->uIntSrc[i] = uIntSrc;
			break;
		}
	}

	pPollIsr->uCntUsers++;

	if(pPollIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}
	 
/**
 * unregister board for interrupt polling
 * 
 * @param pPollIsr			ptr to PollIsr parameter
 * @param pBoard 			ptr to corresponding board structure
 * @param bKillThread		True if Thread should be killed
 * @return 					EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_stdmacdrv_unregister_board_for_polling(EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	LSA_UINT32 i;

	*bKillThread = LSA_FALSE;

	if(pPollIsr->uCntUsers == 0)
	{
		return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
	}

	for(i=0; i<EPS_CFG_STDMACDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			pPollIsr->pBoard[i] = LSA_NULL;

			pPollIsr->uCntUsers--;
			if(pPollIsr->uCntUsers == 0)
			{
				*bKillThread = LSA_TRUE;
			}

			return EPS_PNDEV_RET_OK; /* Board polling already enabled */
		}
	}

	return EPS_PNDEV_RET_ERR; /* Board not found */
}

/**
 * polling isr, which will be called periodically
 * 
 * @param uParam	Parameterstructure (unused)
 * @param arg 		ptr to corresponding structure, containing board and isr parameter
 * @return 					
 */
LSA_VOID eps_stdmacdrv_pn_poll_isr_thread(LSA_UINT32 uParam, void *arg)
{
	EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr = (EPS_STDMACDRV_POLL_PTR_TYPE)arg;
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT32 i;
		
	LSA_UNUSED_ARG(uParam);

	for(i=0; i<EPS_CFG_STDMACDRV_MAX_BOARDS; i++)
	{
		pBoard = pPollIsr->pBoard[i];

		if(pBoard != LSA_NULL)
		{
			switch(pPollIsr->uIntSrc[i])
			{
				case EPS_PNDEV_ISR_PN_GATHERED:
					if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
					}
					else
					{
						EPS_FATAL("No callback function set for gathered interrupt");  /* Unexpected */
					}
					break;
				default: EPS_FATAL("unknown interrupt source"); /* Unexpected */
					break;
			}
		}
	}
}

/**
 * create periodically poll thread
 * 
 * todo: changed to high permance poll thread, see eps_pndevdrv
 * 
 * @param cName			internal thread name
 * @param pPollIsr 		ptr to corresponding structure for isr parameter
 * @param pBoard 		ptr to corresponding board structure
 * @return EPS_PNDEV_RET_OK					
 */
static LSA_UINT16 eps_stdmacdrv_create_poll_thread(LSA_CHAR* cName, EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UNUSED_ARG(pBoard);

	pPollIsr->hThread = eps_tasks_start_poll_thread(cName, EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedRR /*todo eSchedFIFO*/, 1, eps_stdmacdrv_pn_poll_isr_thread, 0, (LSA_VOID*)pPollIsr); 
	EPS_ASSERT(pPollIsr->hThread != 0);

	pPollIsr->bRunning = LSA_TRUE;

	eps_tasks_group_thread_add ( pPollIsr->hThread, EPS_TGROUP_USER );

	return EPS_PNDEV_RET_OK;
}

/**
 * kill periodically poll thread
 * 
 * @param cName			internal thread name
 * @param pPollIsr 		ptr to corresponding structure for isr parameter
 * @param pBoard 		ptr to corresponding board structure
 * @return EPS_PNDEV_RET_OK					
 */
static LSA_UINT16 eps_stdmacdrv_kill_poll_thread(LSA_CHAR* cName, EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr, EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UNUSED_ARG(pBoard);
	LSA_UNUSED_ARG(cName);

	pPollIsr->bRunning = LSA_FALSE;

	eps_tasks_group_thread_remove ( pPollIsr->hThread );

	eps_tasks_stop_poll_thread(pPollIsr->hThread);

	return EPS_PNDEV_RET_OK;
}

/**
 * create periodically poll thread for gathered method
 * 
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution		
 */
static LSA_UINT16 eps_stdmacdrv_create_pn_gathered_polling_thread(EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsStdMacDrv->poll.PnGatheredIsr;

	retVal = eps_stdmacdrv_register_board_for_polling(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_stdmacdrv_create_poll_thread("EPS_STDMACGPISR", pPollIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_stdmacdrv_unregister_board_for_polling(pPollIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

/**
 * kill periodically poll thread for gathered method
 * 
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution		
 */
static LSA_UINT16 eps_stdmacdrv_kill_pn_gathered_polling_thread(EPS_STDMACDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_STDMACDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsStdMacDrv->poll.PnGatheredIsr;

	retVal = eps_stdmacdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_stdmacdrv_kill_poll_thread("EPS_STDMACGATPOISR", pPollIsr, pBoard);
	}

	return retVal;
}

#endif

/**
 * enable mentioned interrupt method for current board
 * 
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pInterrupt		pointer to enum with method type
 * @param pCbf				pointer to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK, upon successful execution or EPS_PNDEV_RET_ERR
 */
static LSA_UINT16 eps_stdmacdrv_enable_interrupt_intern (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_ISR) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
	EPS_PLF_PCI_ISR_CBF_TYPE PciCbf;
	#endif

	pBoard = (EPS_STDMACDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			//(LaM) ToDo
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			if( (pBoard->sDeviceInfo.sConfigSpace.info.bValid == LSA_TRUE) &&
				( (pBoard->sDeviceInfo.sConfigSpace.info.bMSISupport == LSA_TRUE) ||
				  (pBoard->sDeviceInfo.sConfigSpace.info.bMSIXSupport == LSA_TRUE) ) )
			{
			#endif
				#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_ISR) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
				PciCbf.pArgs  = (LSA_VOID*)pBoard;
				PciCbf.uParam = (LSA_UINT32)*pInterrupt;
				PciCbf.pCbf   = eps_stdmacdrv_isr_cbf;
				EPS_PLF_PCI_ENA_INTERRUPT(&pBoard->sDeviceInfo.sConfigSpace, &PciCbf);
				#endif
			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			}
			else
			{
			#endif
				#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_POLL) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
				result = eps_stdmacdrv_create_pn_gathered_polling_thread(pBoard);
				#endif
			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			}
			#endif

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			break;
		case EPS_PNDEV_ISR_PN_NRT:
			EPS_FATAL("enabling NRT interrupts is not supported in stdmacdrv"); /* not supported */
			break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	return result;
}

/**
 * disable mentioned interrupt method for current board
 * 
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK, upon successful execution or EPS_PNDEV_RET_ERR
 */
static LSA_UINT16 eps_stdmacdrv_disable_interrupt_intern (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt)
{
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_STDMACDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			//(LaM) ToDo
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			if( (pBoard->sDeviceInfo.sConfigSpace.info.bValid == LSA_TRUE) &&
				( (pBoard->sDeviceInfo.sConfigSpace.info.bMSISupport == LSA_TRUE) ||
				  (pBoard->sDeviceInfo.sConfigSpace.info.bMSIXSupport == LSA_TRUE) ) )
			{
			#endif
				#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_ISR) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
				EPS_PLF_PCI_DIA_INTERRUPT(&pBoard->sDeviceInfo.sConfigSpace);
				#endif
			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			}
			else
			{
			#endif
				#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_POLL) || (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
				result = eps_stdmacdrv_kill_pn_gathered_polling_thread(pBoard);
				#endif
			#if (EPS_STDMACDRV_ISR_MODE == EPS_ISR_MODE_MIXED)
			}
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			break;
		case EPS_PNDEV_ISR_PN_NRT:
		  EPS_FATAL("disabling NRT interrupts is not supported in stdmacdrv"); /* not supported */
			break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	return result;
}

/**
 * uninstall StdMacDrv
 * 
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
LSA_VOID eps_stdmacdrv_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsStdMacDrv->bInit == LSA_TRUE);

	eps_stdmacdrv_undo_init_critical_section();

	g_pEpsStdMacDrv->bInit = LSA_FALSE;

	g_pEpsStdMacDrv = LSA_NULL;
}

/**
 * install StdMacDrv
 * 
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
LSA_VOID eps_stdmacdrv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnStdMacDrvIf;

	eps_memset(&g_EpsStdMacDrv, 0, sizeof(g_EpsStdMacDrv));
	g_pEpsStdMacDrv = &g_EpsStdMacDrv;

	g_pEpsStdMacDrv->bInit = LSA_TRUE;

	eps_stdmacdrv_init_critical_section();

	#ifdef EPS_STDMACDRV_PRINTF_FOUND_BOARDS
	eps_stdmacdrv_printf_found_boards();
	#endif

	eps_stdmacdrv_register_device_infos();

	//Init Interface
	sPnStdMacDrvIf.open      = eps_stdmacdrv_open;
	sPnStdMacDrvIf.close     = eps_stdmacdrv_close;
	sPnStdMacDrvIf.uninstall = eps_stdmacdrv_uninstall;

	eps_pndev_if_register(&sPnStdMacDrvIf);
}

/**
 * StdMacDrv Isr callbackfunction
 * 
 * @param	uParam		enum with Isr method
 * @param 	pArgs		ptr to current board structure
 * @return 	LSA_VOID
 */
LSA_VOID eps_stdmacdrv_isr_cbf (LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard = (EPS_STDMACDRV_BOARD_PTR_TYPE)pArgs;
	EPS_PNDEV_INTERRUPT_DESC_TYPE Isr = (EPS_PNDEV_INTERRUPT_DESC_TYPE)uParam;

	EPS_ASSERT(pBoard != LSA_NULL);

	switch(Isr)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
			}
			break;
		default:
			EPS_FATAL("not supported interrupt type for stdmacdrv");
			break;
	}
}

/**
 * open device with StdMacDrv
 * 
 * @param	pLocation 		ptr including PciLocation
 * @param	pOption			ptr to options, if FirmwareDownload and Debugging will be executed
 * @param	ppHwInstOut		pointer to ptr of strucute with hardware configuration
 * @param 	hd_id			index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard;
	EDDS_LL_TABLE_TYPE* pLLTable;
	LSA_UINT8* pRegBase = LSA_NULL;
	LSA_UINT16 uPortMacEnd;
	LSA_UINT16 i;

	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(g_pEpsStdMacDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_stdmacdrv_enter();

	pBoard = eps_stdmacdrv_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

	/* Connect Board HW / Map Memory */
	if(pLocation->eLocationSelector == EPS_PNDEV_LOCATION_PCI)
	{
	    pRegBase = eps_stdmacdrv_open_hw(pLocation, pBoard);
	}
	
	EPS_ASSERT(g_pEpsStdMacDrv->bInit == LSA_TRUE);
	
	result = eps_intelLL_open(	&pBoard->uEddsIntel,
								&pBoard->sHw,
								&pBoard->sysDev,
							 	pRegBase,
								hd_id,
								pBoard->sDeviceInfo.sConfigSpace.uVendorId,
								pBoard->sDeviceInfo.sConfigSpace.uDeviceId);
	
	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->sHw.EpsBoardInfo.nrt_mem.size     = 0x600000;
		pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr = (LSA_UINT8*)eps_mem_alloc(pBoard->sHw.EpsBoardInfo.nrt_mem.size, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM);
		pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr = (LSA_UINT32) (pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr - ( (LSA_UINT32) g_pEpsData->HwInfo.LocalMemPool.pBase ) + g_pEpsData->HwInfo.LocalMemPool.uPhyAddr);
		EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
		
		#ifdef EPS_STDMACDRV_PRINTF_OPEN_CLOSE_INFO
		eps_stdmacdrv_printf_openclose_board(LSA_TRUE, pBoard);
		#endif
		
		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
		eps_stdmacdrv_free_board(pBoard);
	}

	eps_stdmacdrv_exit();

	return result;
}

/**
 * close device with StdMacDrv
 * 
 * @param	pHwInstIn		pointer of strucute with hardware configuration
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_close (EPS_PNDEV_HW_PTR_TYPE pHwInstIn)
{
	EPS_STDMACDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_stdmacdrv_enter();

	pBoard = (EPS_STDMACDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

	if(pBoard->uCountIsrEnabled != 0)
	{
		if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
			eps_stdmacdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
		}
	}

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
	eps_mem_free(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM); /* Free Nrt Mem Pool */

	//(LaM) ToDo - Is there something to be reset?

	eps_stdmacdrv_free_board(pBoard);

	#ifdef EPS_STDMACDRV_PRINTF_OPEN_CLOSE_INFO
	eps_stdmacdrv_printf_openclose_board(LSA_FALSE, pBoard);
	#endif

	eps_stdmacdrv_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * enable interrupt for board
 * 
 * @param	pHwInstIn		pointer of strucute with hardware configuration
 * @param	pInterrupt		pointer to enum with method type
 * @param	pCbf			ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_enable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_stdmacdrv_enter();

	result = eps_stdmacdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

	eps_stdmacdrv_exit();

	return result;
}

/**
 * disable interrupt for board
 * 
 * @param	pHwInstIn		pointer of strucute with hardware configuration
 * @param	pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_disable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_stdmacdrv_enter();

	result = eps_stdmacdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

	eps_stdmacdrv_exit();

	return result;
}

/**
 * set gpio
 * 
 * @param	pHwInstIn		pointer of strucute with hardware configuration
 * @param	gpio			enum with gpios
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_set_gpio (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);

	/* Not Supported */
	return EPS_PNDEV_RET_OK;
}

/**
 * clear gpio
 * 
 * @param	pHwInstIn		pointer of strucute with hardware configuration
 * @param	gpio			enum with gpios
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_stdmacdrv_clear_gpio (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);

	/* Not Supported */
	return EPS_PNDEV_RET_OK;
}

/**
 * start timer
 * 
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pCbf				ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16  eps_stdmacdrv_timer_ctrl_start (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pCbf);

	EPS_FATAL("start timer not supported"); /* Not Supported */

	return EPS_PNDEV_RET_OK;
}

/**
 * stop timer
 * 
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16  eps_stdmacdrv_timer_ctrl_stop (struct eps_pndev_hw_tag* pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);

	EPS_FATAL("stop timer not supported"); /* Not Supported */

	return EPS_PNDEV_RET_OK;
}

/**
 * read trace data
 * 
 * @param pHwInstIn 			pointer to strucute with hardware configuration
 * @param offset				offset where should be read from
 * @param ptr					destination pointer for the data
 * @param size					size of data
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16 eps_stdmacdrv_read_trace_data (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("read trace data not supported");
	return EPS_PNDEV_RET_OK;
}

/**
 * read trace data
 * 
 * @param pHwInstIn 			pointer to strucute with hardware configuration
 * @param ptr					source pointer for the data
 * @param offset				offset where should be write to
 * @param size					size of data
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16 eps_stdmacdrv_write_trace_data (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT8* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("write trace data not supported");
	return EPS_PNDEV_RET_OK;
}

#else

LSA_VOID eps_stdmacdrv_install(LSA_VOID)
{
	/* Do nothing because INTEL_LLIF / EDDS not included in build*/
}

#endif /* (PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL) */
