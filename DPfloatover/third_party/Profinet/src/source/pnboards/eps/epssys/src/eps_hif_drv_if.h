#ifndef EPS_HIF_DRV_IF_H_
#define EPS_HIF_DRV_IF_H_

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
/*  F i l e               &F: eps_hif_drv_if.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF Hardware Manager Interface                                       */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_HIF_DRV_RET_ERR             0
#define EPS_HIF_DRV_RET_OK              1

#define EPS_HIF_DRV_HIF_TM_SHARED_MEM        1
#define EPS_HIF_DRV_HIF_TM_SHORT             2
#define EPS_HIF_DRV_HIF_TM_MIXED             3
#define EPS_HIF_DRV_HIF_TM_SHARED_MEM_NO_SER 4

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

struct eps_hif_drv_hw_tag;
struct eps_hif_drv_openpar_tag;

typedef LSA_VOID   (*EPS_HIF_DRV_UNINSTALL_FCT)   (LSA_VOID);
typedef LSA_UINT16 (*EPS_HIF_DRV_OPEN_FCT)        (struct eps_hif_drv_openpar_tag* pInPars, struct eps_hif_drv_hw_tag** ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
typedef LSA_UINT16 (*EPS_HIF_DRV_CLOSE_FCT)       (struct eps_hif_drv_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_ENABLE_ISR_FCT)  (struct eps_hif_drv_hw_tag* pHwInstIn, LSA_UINT16 hHif);
typedef LSA_UINT16 (*EPS_HIF_DRV_DISABLE_ISR_FCT) (struct eps_hif_drv_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_SEND_HIF_IR)     (struct eps_hif_drv_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_ACK_HIF_IR)      (struct eps_hif_drv_hw_tag* pHwInstIn);
typedef LSA_UINT16 (*EPS_HIF_DRV_REENABLE_HIF_IR) (struct eps_hif_drv_hw_tag* pHwInstIn);
typedef LSA_VOID   (*EPS_HIF_DRV_HIF_DMACPY_FCT)  (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_VOID** ppData, LSA_UINT32 uLength);
typedef LSA_VOID   (*EPS_HIF_DRV_MEM_POOL_CREATE) (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_VOID* pBase, LSA_UINT32 uLength);
typedef LSA_VOID   (*EPS_HIF_DRV_MEM_POOL_FREE)   (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_UINT16 *pRetVal);
typedef LSA_VOID   (*EPS_HIF_DRV_ALLOC_SHM)       (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_VOID_PTR_TYPE *ppMem, LSA_UINT32 uLength);
typedef LSA_VOID   (*EPS_HIF_DRV_FREE_SHM)        (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_UINT16 *pRetVal, LSA_VOID_PTR_TYPE pMem);
typedef LSA_VOID   (*EPS_HIF_DRV_GET_PARAMS)      (struct eps_hif_drv_hw_tag* pShmInstIn, LSA_VOID* pPar);

typedef struct eps_hif_drv_hw_tag
{	
	LSA_UINT32 hHandle;
	LSA_UINT16 hd_id;
	LSA_BOOL   bUpper;
}  EPS_HIF_DRV_HW_TYPE, *EPS_HIF_DRV_HW_PTR_TYPE, **EPS_HIF_DRV_HW_PTR_PTR_TYPE;

typedef struct
{
	EPS_HIF_DRV_OPEN_FCT         open;
	EPS_HIF_DRV_CLOSE_FCT        close;
	EPS_HIF_DRV_UNINSTALL_FCT    uninstall;
	EPS_HIF_DRV_MEM_POOL_CREATE  MemPoolCreate;
	EPS_HIF_DRV_MEM_POOL_FREE    MemPoolFree;
	EPS_HIF_DRV_ALLOC_SHM        MemAlloc;
	EPS_HIF_DRV_FREE_SHM         MemFree;	
	EPS_HIF_DRV_HIF_DMACPY_FCT   DmaCpy;
	EPS_HIF_DRV_ENABLE_ISR_FCT   EnableIsr;
	EPS_HIF_DRV_DISABLE_ISR_FCT  DisableIsr;
	EPS_HIF_DRV_SEND_HIF_IR      SendIsr;
	EPS_HIF_DRV_ACK_HIF_IR       AckIsr;
	EPS_HIF_DRV_REENABLE_HIF_IR  ReenableIsr;
	EPS_HIF_DRV_GET_PARAMS       GetParams;
}  EPS_HIF_DRV_IF_TYPE, *EPS_HIF_DRV_IF_PTR_TYPE;

typedef struct eps_hif_drv_cfg_tag
{
	LSA_BOOL bUseIpcInterrupts;
	LSA_BOOL bUsePnCoreInterrupts;
	LSA_BOOL bUseRemoteMem;
	LSA_BOOL bUseDma;
	LSA_BOOL bUseShortMode;
	LSA_BOOL bUseSharedMemMode;
	LSA_BOOL bReserved1; /* padding */
	LSA_BOOL bReserved2; /* padding */
} EPS_HIF_DRV_CFG_TYPE, *EPS_HIF_DRV_CFG_PTR_TYPE;

typedef struct
{
	LSA_UINT32 bValid; /* If not valid, default values will be used => Poll On, No Isr, Shared Mem (Buffer Split) */
	EPS_HIF_DRV_CFG_TYPE HifCfg;
	struct
	{
		LSA_UINT32 uMsgValue;             /* writing this value        */			
		LSA_UINT32 uPhysAddress;          /* to this pci address       */
	} IPC;                                /* IPC configuration for HIF */
	struct
	{
		LSA_UINT32 uPhyAddress;           /* PCI Address                           */
		LSA_UINT32 uSize;                 /* Size in Bytes                         */
	} HostRam;                            /* Host Memory Support => HIF Split Pool */
} EPS_HIF_DRV_SHM_UPPER_CONFIG_TYPE, *EPS_HIF_DRV_SHM_UPPER_CONFIG_PTR_TYPE;

typedef struct eps_hif_drv_openpar_tag
{
    LSA_UINT16 Mode;
    struct
    {
        struct
        {
			struct
			{
				LSA_UINT8* Base;
				LSA_UINT32 Size;
			} Local;
			struct
			{
				LSA_UINT8* Base;
				LSA_UINT32 Size;
			} Remote;
        } SharedMem;
        HIF_HANDLE hif_lower_device;
        HIF_SEND_IRQ_MODE_TYPE SendIrqMode;
        LSA_UINT64 uNsWaitBeforeSendIrq;
  
    } if_pars;
	struct
	{
		struct
		{
			struct
			{
				LSA_UINT8* pBase;
				LSA_UINT32 PhysBaseAhb;
				LSA_UINT32 RemoteShmPhysBaseAhb;
				LSA_UINT8* pShadowMem;
				LSA_BOOL   bUseDma;
			} siegdma;
			struct
			{
				LSA_BOOL   bUsehIpcRx;
				LSA_UINT32 hIpcRx;
				LSA_BOOL   bUsehIpcTx;
				LSA_UINT32 hIpcTx;
			} ipc;
		} pars;
	} drv_type_specific;
} EPS_HIF_DRV_OPENPAR_TYPE, *EPS_HIF_DRV_OPENPAR_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_hif_drv_if_init              (LSA_VOID);
LSA_VOID   eps_hif_drv_if_undo_init         (LSA_VOID);
LSA_VOID   eps_hif_drv_init_config          (EPS_HIF_DRV_CFG_PTR_TYPE pCfg);
LSA_VOID   eps_hif_drv_if_register          (EPS_HIF_DRV_IF_PTR_TYPE pHifDevIf, EPS_HIF_DRV_CFG_PTR_TYPE pConfig);
LSA_UINT16 eps_hif_drv_if_open              (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_CFG_PTR_TYPE pCfg, struct eps_hif_drv_openpar_tag* pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_UINT16 eps_hif_drv_if_close             (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_get               (LSA_UINT16 hd_id, LSA_BOOL bUpper, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_VOID   eps_hif_drv_if_mem_pool_create   (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_INT* hPool, LSA_VOID* pBase, LSA_UINT32 uLength);
LSA_VOID   eps_hif_drv_if_mem_pool_free     (LSA_INT hPool, LSA_UINT16 *pRetVal);
LSA_VOID   eps_hif_drv_if_alloc_shm         (LSA_INT hPool, LSA_VOID_PTR_TYPE *ppMem, LSA_UINT32 uLength);
LSA_VOID   eps_hif_drv_if_free_shm          (LSA_INT hPool, LSA_UINT16 *pRetVal, LSA_VOID_PTR_TYPE pMem);
LSA_VOID   eps_hif_drv_if_dma_copy          (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID** ppData, LSA_UINT32 uLength);
LSA_UINT16 eps_hif_drv_if_enable_isr        (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_UINT16 hHif);
LSA_UINT16 eps_hif_drv_if_disable_isr       (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_send_isr          (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_ack_isr           (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_drv_if_reenable_isr      (LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_VOID   eps_hif_drv_if_get_params        (LSA_UINT16 hd_id, LSA_BOOL bUpper, LSA_VOID* pPar);
LSA_VOID   eps_hif_drv_if_check_reenable_isr(LSA_VOID);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_HIF_DRV_IF_H_ */
