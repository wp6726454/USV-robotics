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
/*  F i l e               &F: eps_wpcapdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS WPCAP Interface Adaption                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20037
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>           /* Types / Prototypes / Funcs               */

#if ( PSI_CFG_USE_EDDS == 1 ) 
/*-------------------------------------------------------------------------*/
#include <eps_trc.h>           /* Tracing                                  */
#include <eps_tasks.h>         /* EPS Thread Api                           */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_wpcapdrv.h>      /* Own Header                               */
#include <eps_plf.h>           /* EPS PCI Interface functions              */
#include <eps_mem.h>           /* EPS Memory Management                    */

#include <edd_inc.h>           /* EDD Types                                */
#include <edds_int.h>          /* EDDS Types                               */
#include <pcap.h>
#include <packet32_inc.h>      /* EDDS Lower Layer Intel Driver            */

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_ISR_MODE_POLL   2
#define EPS_ISR_MODE EPS_ISR_MODE_POLL

#define EPS_CFG_WPCAPDRV_MAX_BOARDS             50

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO 
#define EPS_WPCAPDRV_PRINTF_FOUND_BOARDS    
#endif

/* - Function Forward Declaration ----------------------------------------------------------------- */

LSA_VOID    eps_wpcapdrv_isr_cbf           (LSA_UINT32 uParam, LSA_VOID* pArgs);
LSA_VOID    eps_wpcapdrv_uninstall         (LSA_VOID);
LSA_UINT16  eps_wpcapdrv_enable_interrupt  (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16  eps_wpcapdrv_disable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt);
LSA_UINT16  eps_wpcapdrv_set_gpio          (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16  eps_wpcapdrv_clear_gpio        (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16  eps_wpcapdrv_timer_ctrl_start  (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16  eps_wpcapdrv_timer_ctrl_stop   (struct eps_pndev_hw_tag* pHwInstIn);
LSA_UINT16	eps_wpcapdrv_read_trace_data   (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
LSA_UINT16	eps_wpcapdrv_write_trace_data  (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT8* ptr, LSA_UINT32 offset, LSA_UINT32 size);
LSA_UINT16  eps_wpcapdrv_save_dump         (EPS_PNDEV_HW_PTR_TYPE pHwInstIn);

/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_wpcapdrv_store_tag;

typedef struct eps_stdmacdrv_device_info_tag
{
	LSA_CHAR* pName;
	LSA_CHAR Name[300];
	EPS_PNDEV_MAC_TYPE uMac;
} EPS_WPCAPDRV_DEVICE_INFO_TYPE, *EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE;

typedef struct eps_wpcapdrv_board_tag
{	
	LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_SYS_TYPE sysDev;
	EPS_WPCAPDRV_DEVICE_INFO_TYPE sDeviceInfo;
	struct
	{
		PACKET32_LL_HANDLE_TYPE hLL;
		EDDS_LL_TABLE_TYPE tLLFuncs;
	} EDDS_LL;
	struct eps_wpcapdrv_store_tag* pBackRef;
} EPS_WPCAPDRV_BOARD_TYPE, *EPS_WPCAPDRV_BOARD_PTR_TYPE;

typedef struct eps_wpcapdrv_poll_tag
{
	LSA_BOOL   bRunning;
	LSA_UINT32 uCntUsers;
	LSA_UINT32 hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc[EPS_CFG_WPCAPDRV_MAX_BOARDS];
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard[EPS_CFG_WPCAPDRV_MAX_BOARDS];
} EPS_WPCAPDRV_POLL_TYPE, *EPS_WPCAPDRV_POLL_PTR_TYPE;

typedef struct eps_wpcapdrv_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_WPCAPDRV_BOARD_TYPE board[EPS_CFG_WPCAPDRV_MAX_BOARDS];
	EPS_WPCAPDRV_DEVICE_INFO_TYPE foundDevices[EPS_CFG_WPCAPDRV_MAX_BOARDS];
	LSA_UINT32 uCntFoundDevices;
	struct
	{
		EPS_WPCAPDRV_POLL_TYPE PnGatheredIsr;
	} poll;
} EPS_WPCAPDRV_STORE_TYPE, *EPS_WPCAPDRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
EPS_WPCAPDRV_STORE_TYPE g_EpsWpcapDrv;
EPS_WPCAPDRV_STORE_PTR_TYPE g_pEpsWpcapDrv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#ifdef EPS_WPCAPDRV_PRINTF_FOUND_BOARDS

static LSA_VOID eps_wpcapdrv_printf_found_boards(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE pDevice;

	printf("\r\nEPS WpcapDrv Found Network Boards:\r\n");
	printf("----------------------------------------");

	for(i=0; i<g_pEpsWpcapDrv->uCntFoundDevices; i++)
	{
		pDevice = &g_pEpsWpcapDrv->foundDevices[i];

		printf("\r\n%02x:%02x:%02x:%02x:%02x:%02x  -  %s",pDevice->uMac[0],pDevice->uMac[1],pDevice->uMac[2],pDevice->uMac[3],pDevice->uMac[4],pDevice->uMac[5],pDevice->pName);
	}

	printf("\r\n----------------------------------------\r\n");
	printf("\r\n");
}
#endif

#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
static LSA_VOID eps_wpcapdrv_printf_openclose_board(LSA_BOOL bOpen, EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	if(bOpen == LSA_TRUE)
	{
		printf("\r\nEPS WpcapDrv Board Opened: %s", pBoard->sDeviceInfo.pName );
	}
	else
	{
		printf("\r\nEPS WpcapDrv Board Closed: %s", pBoard->sDeviceInfo.pName );
	}
	printf("\r\n");
}
#endif

static LSA_VOID eps_wpcapdrv_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsWpcapDrv->hEnterExit);
}

static LSA_VOID eps_wpcapdrv_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsWpcapDrv->hEnterExit, LSA_FALSE);
}

static LSA_VOID eps_wpcapdrv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsWpcapDrv->hEnterExit);
}

static LSA_VOID eps_wpcapdrv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsWpcapDrv->hEnterExit);
}

static EPS_WPCAPDRV_BOARD_PTR_TYPE eps_wpcapdrv_alloc_board(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	for(i=0; i<EPS_CFG_WPCAPDRV_MAX_BOARDS; i++)
	{
		pBoard = &g_pEpsWpcapDrv->board[i];

		if(pBoard->bUsed == LSA_FALSE)
		{
			pBoard->sHw.hDevice        = (LSA_VOID*)pBoard;
			pBoard->sHw.EnableIsr      = eps_wpcapdrv_enable_interrupt;
			pBoard->sHw.DisableIsr     = eps_wpcapdrv_disable_interrupt;
			pBoard->sHw.SetGpio        = eps_wpcapdrv_set_gpio;
			pBoard->sHw.ClearGpio      = eps_wpcapdrv_clear_gpio;
			pBoard->sHw.TimerCtrlStart = eps_wpcapdrv_timer_ctrl_start;
			pBoard->sHw.TimerCtrlStop  = eps_wpcapdrv_timer_ctrl_stop;
			pBoard->sHw.ReadTraceData  = eps_wpcapdrv_read_trace_data;
			pBoard->sHw.WriteTraceData = eps_wpcapdrv_write_trace_data;
            pBoard->sHw.SaveDump       = eps_wpcapdrv_save_dump;
			pBoard->pBackRef           = g_pEpsWpcapDrv;
			pBoard->bUsed              = LSA_TRUE;

			return pBoard;
		}
	}

	return pBoard;
}

static LSA_VOID eps_wpcapdrv_register_device_infos(LSA_VOID)
{
	EPS_PNDEV_IF_DEV_INFO_TYPE sPnDevInfo;

	int nResult;
	int nIdx = 0;
	pcap_if_t* alldevs;
	pcap_if_t* dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	LPADAPTER lpAdapter;

	struct 
	{
		PACKET_OID_DATA pkt;
		char more[6];
	} buf;

	g_pEpsWpcapDrv->uCntFoundDevices = 0;

	// get all available devices
	nResult = pcap_findalldevs(&alldevs, errbuf);
	EPS_ASSERT( 0 == nResult );

	// iterate through all found devices
	for( dev = alldevs ; (NULL != dev) && (g_pEpsWpcapDrv->uCntFoundDevices<EPS_CFG_WPCAPDRV_MAX_BOARDS) ; dev = dev->next )
	{
		nIdx++;

		// query the mac-address
		lpAdapter = PacketOpenAdapter(dev->name);
		EPS_ASSERT( 0 != lpAdapter );

		buf.pkt.Oid    = OID_802_3_CURRENT_ADDRESS;
		buf.pkt.Length = 6;

		if( PacketRequest(lpAdapter, LSA_FALSE, &buf.pkt) && buf.pkt.Length == 6 )
		{
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].pName = &g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].Name[0];

			eps_strcpy((LSA_CHAR*)g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].pName, (LSA_CHAR*)dev->name);

			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[0]  = buf.pkt.Data[0];
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[1]  = buf.pkt.Data[1];
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[2]  = buf.pkt.Data[2];
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[3]  = buf.pkt.Data[3];
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[4]  = buf.pkt.Data[4];
			g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[5]  = buf.pkt.Data[5];

			sPnDevInfo.uPciAddrValid = 0;
			sPnDevInfo.uMacAddrValid = 1;
			sPnDevInfo.uMacAddr[0] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[0];
			sPnDevInfo.uMacAddr[1] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[1];
			sPnDevInfo.uMacAddr[2] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[2];
			sPnDevInfo.uMacAddr[3] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[3];
			sPnDevInfo.uMacAddr[4] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[4];
			sPnDevInfo.uMacAddr[5] = g_pEpsWpcapDrv->foundDevices[g_pEpsWpcapDrv->uCntFoundDevices].uMac[5];

			sPnDevInfo.uEddType      = LSA_COMP_ID_EDDS;

			eps_pndev_if_register_device(&sPnDevInfo);

			g_pEpsWpcapDrv->uCntFoundDevices++;
		}

		PacketCloseAdapter(lpAdapter);
	}

	if(NULL != dev)
	{
		EPS_FATAL("there are more network adapters installed than defined by EPS_CFG_WPCAPDRV_MAX_BOARDS"); /* there are more network adapters installed than defined by EPS_CFG_WPCAPDRV_MAX_BOARDS => increase EPS_CFG_WPCAPDRV_MAX_BOARDS */
	}

	pcap_freealldevs(alldevs);
}

static LSA_VOID eps_wpcapdrv_free_board(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	pBoard->bUsed = LSA_FALSE;
}

static LSA_UINT16 eps_wpcapdrv_register_board_for_polling(EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr, EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{
	LSA_UINT32 i;

	*bCreateThread = LSA_FALSE;

	if(pPollIsr->uCntUsers >= EPS_CFG_WPCAPDRV_MAX_BOARDS)
	{
		return EPS_PNDEV_RET_ERR; /* No More Resources */
	}

	for(i=0; i<EPS_CFG_WPCAPDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
		}
	}

	for(i=0; i<EPS_CFG_WPCAPDRV_MAX_BOARDS; i++)
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
	 
static LSA_UINT16 eps_wpcapdrv_unregister_board_for_polling(EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr, EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	LSA_UINT32 i;

	*bKillThread = LSA_FALSE;

	if(pPollIsr->uCntUsers == 0)
	{
		return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
	}

	for(i=0; i<EPS_CFG_WPCAPDRV_MAX_BOARDS; i++)
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

LSA_VOID eps_wpcapdrv_pn_poll_isr_thread(LSA_UINT32 uParam, void *arg)
{
	EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr = (EPS_WPCAPDRV_POLL_PTR_TYPE)arg;
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT32 i;

	LSA_UNUSED_ARG(uParam);

	/* Enter polling loop */

	for(i=0; i<EPS_CFG_WPCAPDRV_MAX_BOARDS; i++)
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

	return;
}

static LSA_UINT16 eps_wpcapdrv_create_poll_thread(LSA_CHAR* cName, EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr)
{
    pPollIsr->hThread = eps_tasks_start_us_poll_thread( cName, EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE, eSchedFifo, 32000, eps_wpcapdrv_pn_poll_isr_thread, 0, (LSA_VOID *)pPollIsr );

	EPS_ASSERT(pPollIsr->hThread != 0);

	pPollIsr->bRunning = LSA_TRUE;
#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_add ( pPollIsr->hThread, EPS_TGROUP_USER );
#endif
	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_wpcapdrv_kill_poll_thread(EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr)
{
	pPollIsr->bRunning = LSA_FALSE;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_remove ( pPollIsr->hThread );
#endif

	eps_tasks_stop_poll_thread(pPollIsr->hThread);

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_wpcapdrv_create_pn_gathered_polling_thread(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsWpcapDrv->poll.PnGatheredIsr;

	retVal = eps_wpcapdrv_register_board_for_polling(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_wpcapdrv_create_poll_thread("WPCAP_G_POLL_ISR", pPollIsr);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_wpcapdrv_unregister_board_for_polling(pPollIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_wpcapdrv_kill_pn_gathered_polling_thread(EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_WPCAPDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsWpcapDrv->poll.PnGatheredIsr;

	retVal = eps_wpcapdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_wpcapdrv_kill_poll_thread(pPollIsr);
	}

	return retVal;
}

static LSA_UINT16 eps_wpcapdrv_enable_interrupt_intern (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	#if (EPS_ISR_MODE == EPS_ISR_MODE_ISR) || (EPS_ISR_MODE == EPS_ISR_MODE_MIXED)
	EPS_PLF_PCI_ISR_CBF_TYPE PciCbf;
	#endif

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

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

			#if (EPS_ISR_MODE == EPS_ISR_MODE_POLL)
			result = eps_wpcapdrv_create_pn_gathered_polling_thread(pBoard);
			#else
			EPS_FATAL("function can only be used in EPS_ISR_MODE_POLL");
			#endif

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			break;
		case EPS_PNDEV_ISR_PN_NRT:
		  EPS_FATAL("enabling NRT interrupts is not supported in wpcap driver"); /* not supported */
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
 * disable interrupt in wpcap driver
 * 
 * @param pHwInstIn
 * @param pInterrupt
 * @return
 */
static LSA_UINT16 eps_wpcapdrv_disable_interrupt_intern (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

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

			#if (EPS_ISR_MODE == EPS_ISR_MODE_POLL) || (EPS_ISR_MODE == EPS_ISR_MODE_MIXED)
			result = eps_wpcapdrv_kill_pn_gathered_polling_thread(pBoard);
			#else
			EPS_FATAL("function can only be used in EPS_ISR_MODE_POLL");
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			break;
		case EPS_PNDEV_ISR_PN_NRT:
		  EPS_FATAL("disabling NRT interrupts is not supported in wpcap driver"); /* not supported */
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

LSA_VOID eps_wpcapdrv_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsWpcapDrv->bInit == LSA_TRUE);

	eps_wpcapdrv_undo_init_critical_section();

	g_pEpsWpcapDrv->bInit = LSA_FALSE;

	g_pEpsWpcapDrv = LSA_NULL;
}

LSA_VOID eps_wpcapdrv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnWpcapDrvIf;

	eps_memset(&g_EpsWpcapDrv, 0, sizeof(g_EpsWpcapDrv));
	g_pEpsWpcapDrv = &g_EpsWpcapDrv;

	g_pEpsWpcapDrv->bInit = LSA_TRUE;

	eps_wpcapdrv_init_critical_section();

	eps_wpcapdrv_register_device_infos();

	#ifdef EPS_WPCAPDRV_PRINTF_FOUND_BOARDS
	eps_wpcapdrv_printf_found_boards();
	#endif

	//Init Interface
	sPnWpcapDrvIf.open      = eps_wpcapdrv_open;
	sPnWpcapDrvIf.close     = eps_wpcapdrv_close;
	sPnWpcapDrvIf.uninstall = eps_wpcapdrv_uninstall;

	eps_pndev_if_register(&sPnWpcapDrvIf);
}

LSA_VOID eps_wpcapdrv_isr_cbf (LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pArgs;
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
			EPS_FATAL("unknown interrupt source");
			break;
	}
}

LSA_UINT16 eps_wpcapdrv_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE         pBoard;
	EPS_WPCAPDRV_DEVICE_INFO_PTR_TYPE   pDeviceInfo = LSA_NULL;
	EDDS_LL_TABLE_TYPE *                pLLTable;
	EPS_BOARD_INFO_PTR_TYPE             pBoardInfo;
	LSA_UINT16                          uPortMacEnd;
	LSA_UINT16                          i;
	LSA_UINT16                          result = EPS_PNDEV_RET_OK;

	LSA_UNUSED_ARG(pOption);

	EPS_ASSERT(g_pEpsWpcapDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_wpcapdrv_enter();

	pBoard = eps_wpcapdrv_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

	/* Connect Board HW / Map Memory */

	if (pLocation->eLocationSelector == EPS_PNDEV_LOCATION_MAC)
	{
		for (i=0; i<g_pEpsWpcapDrv->uCntFoundDevices; i++)
		{
			pDeviceInfo = &g_pEpsWpcapDrv->foundDevices[i];
			if ( (pDeviceInfo->uMac[0] == pLocation->Mac[0]) &&
			     (pDeviceInfo->uMac[1] == pLocation->Mac[1]) &&
				 (pDeviceInfo->uMac[2] == pLocation->Mac[2]) &&
				 (pDeviceInfo->uMac[3] == pLocation->Mac[3]) &&
				 (pDeviceInfo->uMac[4] == pLocation->Mac[4]) &&
				 (pDeviceInfo->uMac[5] == pLocation->Mac[5]) )
			{
				break;
			}
			else
			{
				pDeviceInfo = LSA_NULL;
			}
		}
	}

	if (pDeviceInfo != LSA_NULL)
	{
		/* Fill EDDS LL */
		pBoard->sDeviceInfo = *pDeviceInfo;

		pLLTable = &pBoard->EDDS_LL.tLLFuncs;

		/* EDDS init/setup/shutdown/close functions */
		pLLTable->open                      = PACKET32_LL_OPEN;
		pLLTable->setup                     = PACKET32_LL_SETUP;
		pLLTable->shutdown                  = PACKET32_LL_SHUTDOWN;
		pLLTable->close                     = PACKET32_LL_CLOSE;

		/* Transmit buffer handling functions */
		pLLTable->enqueueSendBuffer         = PACKET32_LL_SEND;
		pLLTable->getNextFinishedSendBuffer = PACKET32_LL_SEND_STS;

		#ifdef EDDS_CFG_SEND_BLOCKED
		pLLTable->triggerSend               = PACKET32_LL_SEND_TRIGGER;
		#endif

		pLLTable->reinitTX                  = PACKET32_LL_REINIT_TX;

		/* Receive buffer handling functions */
		pLLTable->getNextReceivedBuffer     = PACKET32_LL_RECV;
		pLLTable->provideReceiveBuffer      = PACKET32_LL_RECV_PROVIDE;

		/* Information functions */
		pLLTable->getLinkStateSetup         = PACKET32_LL_GET_LINK_STATE_SETUP;
		pLLTable->getLinkState              = PACKET32_LL_GET_LINK_STATE;
		pLLTable->checkMAU                  = PACKET32_LL_CHECK_MAUTYPE;
		pLLTable->getStatistics             = PACKET32_LL_GET_STATS;
		pLLTable->getCapabilities           = PACKET32_LL_GET_CAPS;
		/* MAC address management functions */
		pLLTable->getMACAddr                = PACKET32_LL_GET_MAC_ADDR;
		pLLTable->enableMC                  = PACKET32_LL_MC_ENABLE;
		pLLTable->disableMC                 = PACKET32_LL_MC_DISABLE;
		/* Interrupt functions */
		pLLTable->getPendingInterrupt       = PACKET32_LL_INTERRUPT_POLL;
		pLLTable->interruptRX               = PACKET32_LL_INTERRUPT_RX;
		pLLTable->interruptTX               = PACKET32_LL_INTERRUPT_TX;
		pLLTable->interruptPhy              = PACKET32_LL_INTERRUPT_PHY;
		#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
		pLLTable->interrupt                 = PACKET32_LL_INTERRUPT;
		#endif
		/* Control functions */
		pLLTable->setLinkState              = PACKET32_LL_SET_LINK_STATE;
		pLLTable->backupPortLEDMode         = PACKET32_LL_LED_BACKUP_MODE;
		pLLTable->restorePortLEDMode        = PACKET32_LL_LED_RESTORE_MODE;
		pLLTable->setPortLED                = PACKET32_LL_LED_SET_MODE;

		#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
		pLLTable->enableRX                  = PACKET32_LL_RX_OVERLOAD_RX_ON;
		pLLTable->disableRX                 = PACKET32_LL_RX_OVERLOAD_RX_OFF;
		#endif
		/* Switch functions */
        #ifdef LLIF_CFG_SWITCH_SUPPORT
		pLLTable->setSwitchPortState        = PACKET32_LL_SWITCH_SET_PORT_STATE;
		pLLTable->controlSwitchMulticastFwd = PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL;
		pLLTable->flushSwitchFilteringDB    = PACKET32_LL_SWITCH_FLUSH_FILTERING_DB;
		pLLTable->setSwitchPortRedundant    = PACKET32_LL_SWITCH_SET_PORT_REDUNDANT;
		pLLTable->setSwitchBridgeMode       = PACKET32_LL_SWITCH_SET_BRIDGE_MODE;
		pLLTable->getSwitchDroppedFrames    = PACKET32_LL_SWITCH_DROP_CNT;
		#endif

	    /* EDDS set arp filter function */
		#ifdef LLIF_CFG_USE_LL_ARP_FILTER
		pLLTable->setArpFilter = LSA_NULL; //(LaM) ToDo CHECK!
		#endif

		/* Fill Device Info */

		pBoardInfo = &pBoard->sHw.EpsBoardInfo;
		eps_memset(pBoardInfo,0, sizeof(*pBoardInfo));

		pBoard->sysDev.hd_nr       = hd_id;
		pBoard->sysDev.if_nr       = 1;	// don't care
		pBoard->sysDev.pnio_if_nr  = 0;	// don't care
		pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDS;

		// Common HD settings
		pBoardInfo->edd_type      = LSA_COMP_ID_EDDS;
		pBoardInfo->hd_sys_handle = &pBoard->sysDev;

		// Init User to HW port mapping (used PSI GET HD PARAMS)
		eps_hw_init_board_port_param( pBoardInfo );

		// Port specific setup
		pBoardInfo->nr_of_ports = 1;

		// Interface Mac
		pBoardInfo->if_mac[0].mac[0] = pBoard->sDeviceInfo.uMac[0];
		pBoardInfo->if_mac[0].mac[1] = pBoard->sDeviceInfo.uMac[1];
		pBoardInfo->if_mac[0].mac[2] = pBoard->sDeviceInfo.uMac[2];
		pBoardInfo->if_mac[0].mac[3] = pBoard->sDeviceInfo.uMac[3];
		pBoardInfo->if_mac[0].mac[4] = pBoard->sDeviceInfo.uMac[4];
		pBoardInfo->if_mac[0].mac[5] = pBoard->sDeviceInfo.uMac[5];

		// Port MAC - Ersatz Mac Adressen laut Norm bei Standard Hw
		for (i = 0; i < pBoardInfo->nr_of_ports; i++ )
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

		// setup NRT buffer (local mem is used)
		pBoardInfo->nrt_mem.size     = 0x2000000;
		pBoardInfo->nrt_mem.base_ptr = (LSA_UINT8*)eps_mem_alloc(pBoardInfo->nrt_mem.size, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM);
		pBoardInfo->nrt_mem.phy_addr = (LSA_UINT32) (pBoardInfo->nrt_mem.base_ptr - ( (LSA_UINT32) g_pEpsData->HwInfo.LocalMemPool.pBase ) + g_pEpsData->HwInfo.LocalMemPool.uPhyAddr);

		EPS_ASSERT(pBoardInfo->nrt_mem.base_ptr != LSA_NULL);

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
		pBoardInfo->edds.is_valid = LSA_TRUE;
		pBoardInfo->edds.ll_handle         = (LSA_VOID_PTR_TYPE)&pBoard->EDDS_LL.hLL;
		pBoardInfo->edds.ll_function_table = (LSA_VOID_PTR_TYPE)pLLTable;

		pBoardInfo->edds.ll_params = (LSA_VOID*)pDeviceInfo->pName;

		/* Fill other Params */
		pBoard->sHw.asic_type = EPS_PNDEV_ASIC_INTEL_XX;
		pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_INTEL_XX;
	}
	else
	{
		result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
		eps_wpcapdrv_printf_openclose_board(LSA_TRUE, pBoard);
		#endif

		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
		eps_wpcapdrv_free_board(pBoard);
	}

	eps_wpcapdrv_exit();

	return result;
}

LSA_UINT16 eps_wpcapdrv_close (EPS_PNDEV_HW_PTR_TYPE pHwInstIn)
{
	EPS_WPCAPDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	pBoard = (EPS_WPCAPDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

	if(pBoard->uCountIsrEnabled != 0)
	{
		if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
		{
			IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
			eps_wpcapdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
		}
	}

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
	eps_mem_free(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_NRT_MEM); /* Free Nrt Mem Pool */

	//(LaM) ToDo - Is there something to be reset?

	eps_wpcapdrv_free_board(pBoard);

	#ifdef EPS_WPCAPDRV_PRINTF_OPEN_CLOSE_INFO
	eps_wpcapdrv_printf_openclose_board(LSA_FALSE, pBoard);
	#endif

	eps_wpcapdrv_exit();

	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_enable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	result = eps_wpcapdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

	eps_wpcapdrv_exit();

	return result;
}

LSA_UINT16 eps_wpcapdrv_disable_interrupt (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_wpcapdrv_enter();

	result = eps_wpcapdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

	eps_wpcapdrv_exit();

	return result;
}

LSA_UINT16 eps_wpcapdrv_set_gpio (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);
	/* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_clear_gpio (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(gpio);
	/* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_timer_ctrl_start (struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pCbf);
	EPS_FATAL("function not supported"); /* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_timer_ctrl_stop (struct eps_pndev_hw_tag* pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);
	EPS_FATAL("function not supported"); /* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_read_trace_data (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("function not supported"); /* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_write_trace_data (EPS_PNDEV_HW_PTR_TYPE pHwInstIn, LSA_UINT8* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("function not supported"); /* Not Supported */
	return EPS_PNDEV_RET_OK;
}

LSA_UINT16 eps_wpcapdrv_save_dump (EPS_PNDEV_HW_PTR_TYPE pHwInstIn)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	EPS_FATAL("function not supported"); /* Not Supported */
	return EPS_PNDEV_RET_OK;
}

/*----------------------------------------------------------------------------*/
#endif   

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
