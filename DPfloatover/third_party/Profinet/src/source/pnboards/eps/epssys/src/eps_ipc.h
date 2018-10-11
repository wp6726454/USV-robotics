#ifndef EPS_IPC_H_
#define EPS_IPC_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_ipc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS IPC                                                                  */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_IPC_RET_ERR 0
#define EPS_IPC_RET_OK  1

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef LSA_VOID (*EPS_IPC_THREAD_PROC_TYPE)(LSA_UINT32 uParam, LSA_VOID* pArgs);


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* Basic initialisation */
LSA_VOID   eps_ipc_install                 (LSA_VOID);
LSA_VOID   eps_ipc_uninstall               (LSA_VOID);
LSA_UINT32 eps_ipc_sizeof_shm_data         (LSA_VOID);

/* Local interrupt */
LSA_UINT16 eps_ipc_rx_alloc                (LSA_UINT32* hRxHandle, LSA_UINT32* pPhysAddress, LSA_UINT32* pVal);
LSA_VOID   eps_ipc_rx_free                 (LSA_UINT32  hRxHandle);
LSA_VOID   eps_ipc_rx_enable               (LSA_UINT32  hRxHandle, EPS_IPC_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs);
LSA_VOID   eps_ipc_rx_disable              (LSA_UINT32  hRxHandle);
LSA_VOID   eps_ipc_rx_set_remote_shm       (LSA_UINT32  hRxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize);
LSA_VOID   eps_ipc_rx_set_local_shm        (LSA_UINT32  hRxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize);

/* Remote Interrupt */
LSA_UINT16 eps_ipc_tx_alloc                (LSA_UINT32* hTxHandle, LSA_UINT32 uPhysAddress, LSA_UINT32 uMsgVal);
LSA_UINT16 eps_ipc_tx_alloc_from_local_shm (LSA_UINT32* hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize);
LSA_VOID   eps_ipc_tx_free                 (LSA_UINT32  hTxHandle);
LSA_VOID   eps_ipc_tx_set_remote_shm       (LSA_UINT32  hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize);
LSA_VOID   eps_ipc_tx_set_local_shm        (LSA_UINT32  hTxHandle, LSA_UINT8* pBase, LSA_UINT32 uSize);
LSA_VOID   eps_ipc_tx_send                 (LSA_UINT32  hTxHandle);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_IPC_H_ */