#ifndef EPS_SHM_IF_H_
#define EPS_SHM_IF_H_

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
/*  F i l e               &F: eps_shm_if.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Shared Memory Interface Driver Manager                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_SHM_RET_ERR             0
#define EPS_SHM_RET_OK              1
#define EPS_SHM_RET_DEV_NOT_FOUND   2
#define EPS_SHM_RET_DEV_OPEN_FAILED 3
#define EPS_SHM_RET_UNSUPPORTED     4

#define EPS_SHM_ERR_CODE_FATAL      0xa0
#define EPS_SHM_ERR_CODE_FATAL_LSA  0xa1
#define EPS_SHM_ERR_CODE_FATAL_EXC  0xa2

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

struct eps_shm_open_option_tag;
struct eps_shm_hw_tag;
struct eps_shm_app_ready_par_tag;

typedef struct eps_shm_open_option_tag
{
	LSA_BOOL bEnableWatchdog;
	LSA_BOOL uHdNr;
} EPS_SHM_OPEN_OPTION_TYPE, *EPS_SHM_OPEN_OPTION_PTR_TYPE;

typedef struct eps_shm_fatal_par_tag
{
	LSA_UINT32 uErrCode;
	LSA_UINT32 uLine;
	LSA_CHAR*  pMsg;
	LSA_CHAR*  pFile;
	LSA_CHAR*  pComp;
	const LSA_CHAR* pFunc;
	LSA_UINT32 uLsaFatalLen;
	LSA_FATAL_ERROR_TYPE* pLsaFatal;
} EPS_SHM_FATAL_PAR_TYPE, *EPS_SHM_FATAL_PAR_PTR_TYPE;

typedef void (*EPS_SHM_ISR_CBF)(LSA_UINT32 uParam, LSA_VOID* pArgs);

typedef struct eps_shm_callback_tag
{
	EPS_SHM_ISR_CBF pCbf;
	LSA_UINT32 uParam;
	LSA_VOID*  pArgs;
} EPS_SHM_CALLBACK_TYPE, *EPS_SHM_CALLBACK_PTR_TYPE;

typedef LSA_UINT16 (*EPS_SHM_OPEN_FCT)                   (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_PTR_TYPE pOption, struct eps_shm_hw_tag ** ppShmInstOut);
typedef LSA_UINT16 (*EPS_SHM_CLOSE_FCT)                  (struct eps_shm_hw_tag* pShmInstIn);
typedef LSA_VOID   (*EPS_SHM_UNINSTALL_FCT)              (LSA_VOID);
typedef LSA_UINT16 (*EPS_SHM_UPDATE_DEVICE_LIST_FCT)     (LSA_VOID);

typedef LSA_UINT16 (*EPS_SHM_APP_READY_FCT)              (struct eps_shm_hw_tag* pShmInstIn, LSA_VOID* pHifHandle);
typedef LSA_UINT16 (*EPS_SHM_APP_SHUTD_FCT)              (struct eps_shm_hw_tag* pShmInstIn);
typedef LSA_UINT16 (*EPS_SHM_SIG_FATAL_FCT)              (struct eps_shm_hw_tag* pShmInstIn, EPS_SHM_FATAL_PAR_TYPE sFatal);

#if ( PSI_CFG_USE_PNTRC == 1 )
typedef LSA_VOID   (*EPS_SHM_TRACE_BUFFER_FULL_CBF)      (LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength);
#endif

typedef struct
{
	EPS_SHM_OPEN_FCT  open;
	EPS_SHM_CLOSE_FCT close;
	EPS_SHM_UNINSTALL_FCT uninstall;
    EPS_SHM_UPDATE_DEVICE_LIST_FCT update_device_list;

}  EPS_SHM_IF_TYPE, *EPS_SHM_IF_PTR_TYPE;

typedef struct eps_shm_mem_area_tag
{
	LSA_UINT8* pBase;
	LSA_UINT32 uOffset;
	LSA_UINT32 uSize;
} EPS_SHM_MEM_AREA_TYPE, *EPS_SHM_AREA_PTR_TYPE;

typedef struct eps_shm_trace_param_tag
{
	LSA_BOOL   bIsTopMostCPU;
	LSA_BOOL   bUseLocalBinaryTrace;
	LSA_BOOL   bUseInitLogLevels;
	LSA_UINT8* pInitLogLevels;
} EPS_SHM_PNTRC_PARAM_TYPE, *EPS_SHM_PNTRC_PARAM_PTR_TYPE;

typedef struct eps_shm_hw_tag
{
	EPS_SHM_APP_READY_FCT              AppReady;
	EPS_SHM_APP_SHUTD_FCT              AppShutdown;
	EPS_SHM_SIG_FATAL_FCT              FatalError;
	EPS_SHM_CLOSE_FCT                  Close;	
#if ( PSI_CFG_USE_PNTRC == 1 )
	EPS_SHM_TRACE_BUFFER_FULL_CBF      TraceBufferFull;
	EPS_SHM_PNTRC_PARAM_TYPE           sPntrcParam;
#endif
	LSA_UINT32                         uHdNr;
	LSA_UINT32                         uBaseAddr;
	LSA_UINT8*                         pBase;
	LSA_BOOL                           bDebug;
	LSA_BOOL                           bShmHalt;
	EPS_SHM_MEM_AREA_TYPE              sHif;
	EPS_SHM_MEM_AREA_TYPE              sFatal;
	EPS_SHM_MEM_AREA_TYPE              sPntrc1;
	EPS_SHM_MEM_AREA_TYPE              sPntrc2;
} EPS_SHM_HW_TYPE, *EPS_SHM_HW_PTR_TYPE, **EPS_SHM_HW_PTR_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_shm_if_register (EPS_SHM_IF_PTR_TYPE pPnDevIf);
LSA_UINT16 eps_shm_if_open     (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppShmInstOut);
LSA_UINT16 eps_shm_if_close    (EPS_SHM_HW_PTR_TYPE pShmInstIn);
LSA_VOID   eps_shm_if_undo_init(LSA_VOID);
LSA_UINT16 eps_shm_update_device_list(LSA_VOID);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SHM_IF_H_ */
