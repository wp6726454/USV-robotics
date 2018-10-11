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
/*  F i l e               &F: eps_ipc.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Inter Processor Communication                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20000
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>                  /* Types / Prototypes / Fucns */
#include <eps_trc.h>                  /* Tracing                    */
#include <eps_locks.h>                /* EPS Locks                  */
#include <eps_adonis_ipc_drv.h>       /* Adonis EPS Ipc Driver      */
#include <eps_tasks.h>                /* EPS Tasks Api              */
#include <eps_rtos.h>                 /* Driver Interface           */
#include <eps_ipc.h>                  /* Own Header                 */

#define EPS_IPC_MAX_INSTANCES 10
#define EPS_IPC_MAGIC_NUMBER  0x12123434

typedef struct {
	LSA_UINT32 uMagic; /* EPS_IPC_MAGIC_NUMBER */
	LSA_UINT32 uTxCnt; /* Transmit Cnt         */
	LSA_UINT32 uRxCnt; /* Receive Cnt          */
	LSA_UINT32 uTriggerBaseAddr;
	LSA_UINT32 uTriggerMsg;
} EPS_IPC_SHM_TYPE, *EPS_IPC_SHM_PTR_TYPE;

typedef struct
{
	LSA_BOOL   bUsed;
	LSA_BOOL   bIsrEnabled;
	LSA_UINT32 uCntRcvTx;
	LSA_UINT32 uCntRcvIsrs;
	LSA_UINT32 uCntDataTransmitDelay;
	LSA_UINT32 hOwnHandle;
	EPS_IPC_SHM_PTR_TYPE pRemoteShm;
	EPS_IPC_SHM_PTR_TYPE pLocalShm;
	EPS_ADONIS_IPC_DRV_NOTIFY_INFO_TYPE sLocalNotifyInfo;
	EPS_ADONIS_IPC_DRV_INT_INFO_TYPE sLocalInterruptInfo;
	struct
	{
		EPS_IPC_THREAD_PROC_TYPE pCbf;
		LSA_UINT32 uParam;
		LSA_VOID* pArgs;
	} Cbf;
} EPS_IPC_RX_INST_TYPE, *EPS_IPC_RX_INST_PTR_TYPE;

typedef struct
{
	LSA_BOOL   bUsed;
	LSA_UINT32 uCntIsrTrigger;
	LSA_UINT32 uCntDataReceiveDelay;
	LSA_UINT32 hOwnHandle;
	EPS_IPC_SHM_PTR_TYPE pRemoteShm;
	EPS_IPC_SHM_PTR_TYPE pLocalShm;
	EPS_ADONIS_IPC_DRV_NOTIFY_INFO_TYPE sRemoteNotifyInfo;
} EPS_IPC_TX_INST_TYPE, *EPS_IPC_TX_INST_PTR_TYPE;

typedef struct
{
	LSA_UINT16 hEnterExit;
	LSA_BOOL   bInit;
	int fdIpcDriver;
	EPS_IPC_RX_INST_TYPE InstRx[EPS_IPC_MAX_INSTANCES];
	EPS_IPC_TX_INST_TYPE InstTx[EPS_IPC_MAX_INSTANCES];
} EPS_IPC_TYPE, *EPS_IPC_PTR_TYPE;

EPS_IPC_TYPE g_EpsIpc;
EPS_IPC_PTR_TYPE g_pEpsIpc = LSA_NULL;


/**
 * Eps ips interrupt service routine receive thread
 * 
 * Makes sure data has arrived and acknowledges the receive.
 * Executes given thread procedure after ipc receive.
 * 
 * @param uParam unused
 * @param pArgs pointer to ipc receive instance
 */
LSA_VOID eps_ipc_isr_rx_rcv_thread(LSA_UINT32 uParam, LSA_VOID* pArgs)
{
    LSA_UNUSED_ARG(uParam);
    
	EPS_IPC_RX_INST_PTR_TYPE pInst = (EPS_IPC_RX_INST_PTR_TYPE)pArgs;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

	if(pInst->pLocalShm != LSA_NULL)
	{
		/* This algorithm checks if data that was transmitted has already arrived, that's needed
		   because the data takes a different way then the interrupt 
		*/

		while(pInst->pLocalShm->uTxCnt == pInst->uCntRcvTx) /* If there is new data the uTxCnt must be greater than the local uCntRcvTx */
		{
			EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_WARN, "eps_ipc_isr_rx_rcv_thread(): Received IPC Isr h(0x%x) but data hasn't arrived now => sleeping", pInst->hOwnHandle );

			pInst->uCntDataTransmitDelay++; /* If it's equal, data hasn't arrived => we need to wait for the data */
			eps_tasks_usleep(2);
		}

		pInst->uCntRcvTx++; /* Increase our local uCntRcvtx => now it should be equal to uTxCnt again */
		
		/* So far this method works as long there is no interrupt lost. If one is lost the counters will be always different and we are losing incomplete data protection!
		   Setting uCntRcvTx = uTxCnt sounds better but doesn't work as well, as a couple of interrupts get buffered by the apic causing uCntRcvTx and uTxCnt to get the same value
		   which would stuck the system.

		   The solution is to implement a ping/pong mechanism. Further tx interrupts on the source processor are not allowed until rx finished.
		*/

		if(pInst->pRemoteShm != LSA_NULL)
		{
			pInst->pRemoteShm->uRxCnt = pInst->uCntRcvTx; /* If there is a feedback path in the other direction we set the actual counter there to ack the receive. */
		}
	}

	pInst->uCntRcvIsrs++;

	pInst->Cbf.pCbf(pInst->Cbf.uParam, pInst->Cbf.pArgs);
}

/**
 * Send SW interrupt when target is ready to receive new data.
 * Wait for target if previous receive hasn't been acknowledged yet.
 * 
 * @param pInst pointer to ipc transceive instance containing interrupt information
 */
static LSA_VOID eps_ipc_send_isr(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
	if(pInst->pRemoteShm != LSA_NULL)
	{
		if(pInst->pLocalShm != LSA_NULL)
		{
			while(pInst->pLocalShm->uRxCnt != pInst->uCntIsrTrigger)
			{
				EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_WARN, "eps_ipc_send_isr(): Send IPC Isr h(0x%x) but previous wasn't processed by target processor => sleeping", pInst->hOwnHandle );

				/* The previous receive hasn't been acked so far => we need to wait until done (ping/pong mechanism) */
				eps_tasks_usleep(0);

				pInst->uCntDataReceiveDelay++;
			}
		}

		pInst->uCntIsrTrigger++;
		pInst->pRemoteShm->uTxCnt = pInst->uCntIsrTrigger;
	}
	else
	{
		pInst->uCntIsrTrigger++;
	}

	*((LSA_UINT32*)pInst->sRemoteNotifyInfo.uAddress) = pInst->sRemoteNotifyInfo.uValue; /* Write value to target processor SW interrupt */
}

/**
 * Searches for a free ipc driver notification info instance and allocates it.
 * 
 * Requests an interrupt vector from pool.
 * Stores handle to interrupt vector in in notification info.
 * Stores 32 bit local address of interrupt vector in notification info
 * Stores 32 bit translated pci address in in notification info.
 * Stores trigger value of interrupt vector in in notification info.
 * 
 * @param pInst pointer to ipc receive instance
 */
static LSA_VOID eps_ipc_alloc_local_notify_info(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	retVal = ioctl(g_pEpsIpc->fdIpcDriver, EPS_ADONIS_IPC_DRV_ALLOC_LOCAL_NOTIFY_INFO, &pInst->sLocalNotifyInfo);		
	EPS_ASSERT(retVal == 0);
}

/**
 * Free allocated free ipc driver notification info instance and interrupt vector.
 * 
 * @param pInst pointer to ipc receive instance containing ipc notification info
 */
static LSA_VOID eps_ipc_free_local_notify_info(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	retVal = ioctl(g_pEpsIpc->fdIpcDriver, EPS_ADONIS_IPC_DRV_FREE_LOCAL_NOTIFY_INFO, &pInst->sLocalNotifyInfo);		
	EPS_ASSERT(retVal == 0);
}

/**
 * Registers ipc interrupt in adonis and enables it.
 * 
 * @param pInst pointer to ipc receive instance containing interrupt information
 */
static LSA_VOID eps_ipc_enable_local_ipc_interrupt(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	retVal = ioctl(g_pEpsIpc->fdIpcDriver, EPS_ADONIS_IPC_DRV_LOCAL_INT_ENABLE, (LSA_VOID*)&pInst->sLocalInterruptInfo);		
	EPS_ASSERT(retVal == 0);
}

/**
 * Disables ipc interrupt and unregisters it in adonis.
 * 
 * @param pInst pointer to ipc receive instance containing interrupt information
 */
static LSA_VOID eps_ipc_disable_local_ipc_interrupt(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	retVal = ioctl(g_pEpsIpc->fdIpcDriver, EPS_ADONIS_IPC_DRV_LOCAL_INT_DISABLE, (LSA_VOID*)&pInst->sLocalInterruptInfo);		
	EPS_ASSERT(retVal == 0);
}

/**
 * Adds logical address from uPciAddress in remote notification info to adonis memory mapping
 * and stores virtual address into uAddress in remote notification info.
 * 
 * @param pInst pointer to ipc receive instance containing remote notification info
 */
static LSA_VOID eps_ipc_translate_remote_notify_info(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	retVal = ioctl(g_pEpsIpc->fdIpcDriver, EPS_ADONIS_IPC_DRV_TRANSLATE_NOTIFY_INFO, &pInst->sRemoteNotifyInfo);		
	EPS_ASSERT(retVal == 0);
}

/**
 * Destroys locks for eps critical sections.
 */
static LSA_VOID eps_ipc_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsIpc->hEnterExit);
}

/**
 * Inits locks for eps critical sections.
 */
static LSA_VOID eps_ipc_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsIpc->hEnterExit, LSA_FALSE);
}

/**
 * Enter function for critical sections in eps ipc
 * 
 * Protects from concurrent access in following critical section.
 */
static LSA_VOID eps_ipc_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsIpc->hEnterExit);
}

/**
 * Exit function for critical sections in eps ipc
 * 
 * Marks the end of critical section in eps ipc.
 */
static LSA_VOID eps_ipc_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsIpc->hEnterExit);
}

/**
 * Searches for a free ipc receive instance, allocates and inits it.
 * 
 * @return pointer to allocated instance or LSA_NULL if no free instance was found
 */
static EPS_IPC_RX_INST_PTR_TYPE eps_ipc_alloc_rx_instance(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_IPC_RX_INST_PTR_TYPE pInst = LSA_NULL;

	for(i=0; i<EPS_IPC_MAX_INSTANCES; i++)
	{
		pInst = &g_pEpsIpc->InstRx[i];

		if	(pInst->bUsed == LSA_FALSE)
			// found free instance
		{
			eps_memset(pInst, 0, sizeof(*pInst));
			pInst->hOwnHandle       = i;
			pInst->bUsed = LSA_TRUE;
			break;
		}
		else
			// instance already used
		{
			pInst = LSA_NULL;
		}
	}

	return pInst;
}

/**
 * Searches for a free ipc transceive instance, allocates and inits it.
 * 
 * @return pointer to allocated instance or LSA_NULL if no free instance was found
 */
static EPS_IPC_TX_INST_PTR_TYPE eps_ipc_alloc_tx_instance(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_IPC_TX_INST_PTR_TYPE pInst = LSA_NULL;

	for(i=0; i<EPS_IPC_MAX_INSTANCES; i++)
	{
		pInst = &g_pEpsIpc->InstTx[i];

		if	(pInst->bUsed == LSA_FALSE)
			// found free instance
		{
			eps_memset(pInst, 0, sizeof(*pInst));
			pInst->hOwnHandle       = i;
			pInst->bUsed = LSA_TRUE;
			break;
		}
		else
			// instance already used
		{
			pInst = LSA_NULL;
		}
	}

	return pInst;
}

/**
 * Marks used ipc receive instance as free.
 */
static LSA_VOID eps_ipc_free_rx_instance(EPS_IPC_RX_INST_PTR_TYPE pInst)
{
	EPS_ASSERT(pInst != LSA_NULL);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	pInst->uCntRcvIsrs = 0;
	pInst->uCntRcvTx = 0;
	pInst->uCntDataTransmitDelay = 0;
	pInst->bUsed = LSA_FALSE;
}

/**
 * Marks used ipc transceive instance as free.
 */
static LSA_VOID eps_ipc_free_tx_instance(EPS_IPC_TX_INST_PTR_TYPE pInst)
{
	EPS_ASSERT(pInst != LSA_NULL);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	pInst->uCntIsrTrigger = 0;
	pInst->uCntDataReceiveDelay = 0;
	pInst->bUsed = LSA_FALSE;
}

/**
 * Returns pointer to ipc receive instance for given ipc receive handle.
 * 
 * @param hHandle of ipc receive instance
 * 
 * @return pointer to ipce receive instance
 */
static EPS_IPC_RX_INST_PTR_TYPE eps_ipc_get_rx_inst (LSA_UINT32 hHandle)
{
	EPS_ASSERT(hHandle <= EPS_IPC_MAX_INSTANCES);
	EPS_ASSERT(g_pEpsIpc->InstRx[hHandle].bUsed == LSA_TRUE);
	return &g_pEpsIpc->InstRx[hHandle];
}

/**
 * Returns pointer to ipc transceive instance for given ipc transceive handle.
 * 
 * @param hHandle of ipc transceive instance
 * 
 * @return pointer to ipce transceive instance
 */
static EPS_IPC_TX_INST_PTR_TYPE eps_ipc_get_tx_inst (LSA_UINT32 hHandle)
{
	EPS_ASSERT(hHandle <= EPS_IPC_MAX_INSTANCES);
	EPS_ASSERT(g_pEpsIpc->InstTx[hHandle].bUsed == LSA_TRUE);
	return &g_pEpsIpc->InstTx[hHandle];
}

/**
 * Installation function of eps inter processor communication
 * 
 * Inits global eps ipc structure "g_EpsIpc" and stores it address in "g_pEpsIpc".
 * Inits locks for eps critical sections.
 * Installs eps adonis ipc driver and opens it.
 * Upon successful installation file descriptor to ipc driver is stored in global eps ipc structure.
 */
LSA_VOID eps_ipc_install(LSA_VOID)
{
	LSA_INT retVal;

	eps_memset(&g_EpsIpc, 0, sizeof(g_EpsIpc));
	g_pEpsIpc = &g_EpsIpc;

	eps_ipc_init_critical_section();

	retVal = eps_adonis_ipc_drv_install();
	EPS_ASSERT(retVal == 0);

	g_pEpsIpc->fdIpcDriver = open(EPS_ADONIS_IPC_DRV_NAME, O_RDWR);
	EPS_ASSERT(g_pEpsIpc->fdIpcDriver != -1);

	g_pEpsIpc->bInit = LSA_TRUE;
}

/**
 * Deinstallation function of eps inter processor communication
 * 
 * Closes eps adonis ipc driver and deinstalls it.
 * Destroys locks for eps critical sections.
 * Sets pointer to global eps ipc structure "g_pEpsIpc" to LSA_NULL.
 */
LSA_VOID eps_ipc_uninstall(LSA_VOID)
{
	LSA_INT retVal;

	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE)

	close(g_pEpsIpc->fdIpcDriver);

	retVal = eps_adonis_ipc_drv_uninstall();
	EPS_ASSERT(retVal == 0);

	g_pEpsIpc->bInit = LSA_FALSE;

	eps_ipc_undo_init_critical_section();

	g_pEpsIpc = LSA_NULL;
}

/**
 * Returns size of eps ipc shared memory data structure.
 * 
 * @return sizeof(EPS_IPC_SHM_TYPE)
 */
LSA_UINT32 eps_ipc_sizeof_shm_data(LSA_VOID)
{
	return sizeof(EPS_IPC_SHM_TYPE);
}

/**
 * Allocates an ipc receive instance and ipc notification info instance. 
 * Requests an interrupt vector from pool and gets and stores ipc notification info.
 * 
 * @param hRxHandle is set to handle of allocated ipc receive instance
 * @param pPhysAddress is set to 32 bit pcie address (@see eps_ipc_alloc_local_notify_info())
 * @param pVal is set to ipc interrupt trigger value (@see eps_ipc_alloc_local_notify_info())
 * 
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_rx_alloc (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal)
{
	EPS_IPC_RX_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(pPhysAddress != LSA_NULL);
	EPS_ASSERT(pVal != LSA_NULL);
	EPS_ASSERT(hRxHandle != LSA_NULL);

	eps_ipc_enter();

	pInst = eps_ipc_alloc_rx_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	eps_ipc_alloc_local_notify_info(pInst);

	*pVal = pInst->sLocalNotifyInfo.uValue;
	*pPhysAddress = pInst->sLocalNotifyInfo.uPciAddress;

	*hRxHandle = pInst->hOwnHandle;

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_rx_alloc(): h(0x%x), hInt(0x%x), TrigAddr(0x%8x), TrigPciAddr(0x%8x), Value(0x%8x)", pInst->hOwnHandle, pInst->sLocalNotifyInfo.hIntHandle, pInst->sLocalNotifyInfo.uAddress, pInst->sLocalNotifyInfo.uPciAddress, pInst->sLocalNotifyInfo.uValue );

	eps_ipc_exit();

	return EPS_IPC_RET_OK;
}

/**
 * Free allocated ipc receive instance, ipc notification info instance
 * and used interrupt vector.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 */
LSA_VOID eps_ipc_rx_free (LSA_UINT32 hRxHandle)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_get_rx_inst(hRxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_rx_free(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_free_local_notify_info(pInst);

	eps_ipc_free_rx_instance(pInst);

	eps_ipc_exit();
}

/**
 * Enable ipc receive
 * Creates and starts ipc event thread (@see eps_tasks_thread_proc_internal) and enables ipc interrupt.
 * 
 * Ipc event thread waits for signal that is send when an interrupt has arrived.
 * After signal for interrupt has arrived ipc isr receive thread routine (@see eps_ipc_isr_rx_rcv_thread) is executed. 
 * Where the given thread procedure "pCbf" is executed after receive is confirmed and acknowledged.
 * After starting ipc event thread, an ipc interrupt is registered in adonis and enabled.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pCbf pointer to thread procedure to be executed after receive
 * @param uParam parameter of thread procedure
 * @param pArgs pointer to arguments of thread procedure
 */
LSA_VOID eps_ipc_rx_enable (LSA_UINT32 hRxHandle, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_get_rx_inst(hRxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

	pInst->Cbf.pCbf    = pCbf;
	pInst->Cbf.uParam  = uParam;
	pInst->Cbf.pArgs   = pArgs;
	pInst->bIsrEnabled = LSA_TRUE;

	pInst->sLocalInterruptInfo.hThread = eps_tasks_start_event_thread("EPS_IPC_ISR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, eps_ipc_isr_rx_rcv_thread, 0, pInst);
	pInst->sLocalInterruptInfo.hIntHandle = pInst->sLocalNotifyInfo.hIntHandle; 

	eps_ipc_enable_local_ipc_interrupt(pInst);

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_rx_enable(): h(0x%x), hThread(0x%x), hIntHandle(0x%x)", pInst->hOwnHandle, pInst->sLocalInterruptInfo.hThread, pInst->sLocalInterruptInfo.hIntHandle );

	eps_ipc_exit();
}

/**
 * Disables ipc receive for given ipc receive handle.
 * 
 * Disables ipc receive interrupt and unregisters it in adonis.
 * Sends termination signal to ipc event thread (@see eps_tasks_thread_proc_internal),
 * since receive interrupts are disabled ipc event thread will terminate upon termination signal.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 */
LSA_VOID eps_ipc_rx_disable (LSA_UINT32 hRxHandle)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_get_rx_inst(hRxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_rx_disable(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_disable_local_ipc_interrupt(pInst);

	eps_tasks_stop_event_thread(pInst->sLocalInterruptInfo.hThread);

	pInst->bIsrEnabled = LSA_FALSE;

	eps_ipc_exit();
}

/**
 * Stores local shared memory address and size in ipc receive instance.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pBase base address of local shared memory
 * @param uSize size of shared memory
 */
LSA_VOID eps_ipc_rx_set_remote_shm (LSA_UINT32 hRxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_get_rx_inst(hRxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pRemoteShm = (EPS_IPC_SHM_PTR_TYPE) pBase;

	pInst->pRemoteShm->uMagic = EPS_IPC_MAGIC_NUMBER;
	pInst->pRemoteShm->uTriggerBaseAddr = pInst->sLocalNotifyInfo.uPciAddress;
	pInst->pRemoteShm->uTriggerMsg = pInst->sLocalNotifyInfo.uValue;
	pInst->pRemoteShm->uRxCnt = 0;

	eps_ipc_exit();
}

/**
 * Stores local shared memory address and size in ipc receive instance.
 * 
 * @param hRxHandle handle of allocated ipc receive instance
 * @param pBase base address of local shared memory
 * @param uSize size of shared memory
 */
LSA_VOID eps_ipc_rx_set_local_shm (LSA_UINT32 hRxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_RX_INST_PTR_TYPE pInst = eps_ipc_get_rx_inst(hRxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pLocalShm = (EPS_IPC_SHM_PTR_TYPE) pBase;

	eps_ipc_exit();
}

/**
 * Allocates an ipc transceive instance.
 * Stores target pci address, the virtual address from memory mapping (@see eps_ipc_translate_remote_notify_info())
 * and interrupt trigger value in remote notification info.
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param uPhysAddress target pci address
 * @param uMsgVal target interrupt trigger value
 * 
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_tx_alloc (LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal)
{
	EPS_IPC_TX_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(hTxHandle != LSA_NULL);

	eps_ipc_enter();

	pInst = eps_ipc_alloc_tx_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	pInst->sRemoteNotifyInfo.uPciAddress = uPhysAddress;
	pInst->sRemoteNotifyInfo.uValue = uMsgVal;

	eps_ipc_translate_remote_notify_info(pInst);

	EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_tx_alloc(): h(0x%x), TrigAddr(0x%8x), TrigPciAddr(0x%8x), Value(0x%8x)", pInst->hOwnHandle, pInst->sRemoteNotifyInfo.uAddress, pInst->sRemoteNotifyInfo.uPciAddress, pInst->sRemoteNotifyInfo.uValue );

	*hTxHandle = pInst->hOwnHandle;

	eps_ipc_exit();

	return EPS_IPC_RET_OK;
}

/**
 * Allocates an ipc transceive instance out of information stored in shared memory.
 * 
 * @param hTxHandle is set to handle of allocated ipc transceive instance
 * @param pBase base address of shared memory
 * @param uSize size of shared memory
 * 
 * @return EPS_IPC_RET_OK
 */
LSA_UINT16 eps_ipc_tx_alloc_from_local_shm (LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize)
{
	EPS_IPC_SHM_PTR_TYPE pShmLocalHeader = (EPS_IPC_SHM_PTR_TYPE) pBase;

	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(hTxHandle != LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	return eps_ipc_tx_alloc(hTxHandle, pShmLocalHeader->uTriggerBaseAddr, pShmLocalHeader->uTriggerMsg);
}

/**
 * Free allocated ipc transceive instance instance.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
LSA_VOID eps_ipc_tx_free (LSA_UINT32 hTxHandle)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_get_tx_inst(hTxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_ipc_tx_free(): h(0x%x)", pInst->hOwnHandle );

	eps_ipc_free_tx_instance(pInst);

	eps_ipc_exit();
}

/**
 * Stores remote shared memory address and size in ipc transceive instance.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 * @param pBase base address of remote shared memory
 * @param uSize size of shared memory
 */
LSA_VOID eps_ipc_tx_set_remote_shm (LSA_UINT32 hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_get_tx_inst(hTxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pRemoteShm = (EPS_IPC_SHM_PTR_TYPE) pBase;

	eps_ipc_exit();
}

/**
 * Stores local shared memory address and size in ipc transceive instance.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 * @param pBase base address of local shared memory
 * @param uSize size of shared memory
 */
LSA_VOID eps_ipc_tx_set_local_shm (LSA_UINT32 hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_get_tx_inst(hTxHandle);

	eps_ipc_enter();

	EPS_ASSERT(pBase != LSA_NULL);
	EPS_ASSERT(uSize <= sizeof(EPS_IPC_SHM_TYPE));

	pInst->pLocalShm = (EPS_IPC_SHM_PTR_TYPE) pBase;

	eps_ipc_exit();
}

/**
 * Send ipc SW interrupt to target.
 * 
 * @param hTxHandle handle of allocated ipc transceive instance
 */
LSA_VOID eps_ipc_tx_send (LSA_UINT32 hTxHandle)
{
	EPS_ASSERT(g_pEpsIpc != LSA_NULL);
	EPS_ASSERT(g_pEpsIpc->bInit == LSA_TRUE);

	EPS_IPC_TX_INST_PTR_TYPE pInst = eps_ipc_get_tx_inst(hTxHandle);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);

	eps_ipc_send_isr(pInst);
}

