#ifndef EPS_SYS_H
#define EPS_SYS_H

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
/*  F i l e               &F: eps_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
#include "psi_inc.h"   /* Psi Interface          */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

/*****************************************************************************/
/*  Macros                                                                   */
/*****************************************************************************/

#define EPS_RQB_SET_REQ_FCT_PTR(rb_, val_)   {(rb_)->_user_func = (val_);}

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef LSA_VOID LSA_FCT_PTR(,EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(LSA_VOID* pEpsOpen, LSA_VOID* unused);

typedef LSA_VOID (*EPS_LOWER_DONE_FCT)(LSA_VOID_PTR_TYPE pInst, LSA_VOID_PTR_TYPE pRQB);

struct eps_header
{
	LSA_RQB_HEADER(struct eps_header*)
};

typedef struct eps_sys_tag
{
	LSA_UINT16 hd_nr;
	LSA_UINT16 if_nr;
	LSA_UINT16 pnio_if_nr;
	LSA_UINT16 edd_comp_id;
	LSA_BOOL   shm_if;
} EPS_SYS_TYPE, *EPS_SYS_PTR_TYPE;

typedef struct
{
	LSA_UINT8*  pBase;
	LSA_UINT32  uPhyAddr;
	LSA_UINT32  lSize;
} EPS_MEM_REGION_TYPE, *EPS_MEM_REGION_PTR_TYPE;

typedef struct
{	
	EPS_MEM_REGION_TYPE         LocalMemPool;
	EPS_MEM_REGION_TYPE         FastMemPool;

} EPS_HW_INFO_TYPE, *EPS_HW_INFO_PTR_TYPE;

typedef enum eps_runs_on_type_enum { 
	EPS_RUNS_ON_UNKNOWN      = 0x00,
    EPS_RUNS_ON_APPLICATION  = 0x01,  // Pciox App or light, basic and advanced
    EPS_RUNS_ON_ADVANCED     = 0x02,  // adonis_x32 fw
    EPS_RUNS_ON_BASIC        = 0x03,  // basic fw
	EPS_RUNS_ON_MAX
} EPS_RUNS_ON_TYPE;

typedef enum eps_mem_ram_type_enum {
    EPS_RAM_MEM_IRE         = 0,
    EPS_RAM_MEM_PNIP        = 1,
    EPS_RAM_MEM_SDRAM       = 2,
    EPS_RAM_MEM_PERIF_RAM
} EPS_MEM_RAM_TYPE;

typedef struct eps_open_tag
{
    LSA_UINT32                      hd_count;
	PSI_SOCK_INPUT_TYPE             sock_args;                    //In: Sock configuration values for LD 
    PSI_IM_INPUT_TYPE               imInput;
	PSI_HD_INPUT_TYPE               hd_args[PSI_CFG_MAX_HD_CNT];  //In: configuration of all HD
	PSI_HD_OUTPUT_TYPE              hd_out[PSI_CFG_MAX_HD_CNT];   //out: calculated config of all HD
    LSA_BOOL                        ramtest_on[PSI_CFG_MAX_HD_CNT + 1]; // LD + n HD                   
    LSA_BOOL                        write_dumpfiles_on_fatal[PSI_CFG_MAX_HD_CNT + 1]; // LD + n HD  
    LSA_BOOL                        fw_debug_activated;
	LSA_BOOL                        shm_debug_activated;
	LSA_BOOL                        shm_halt_activated;
    LSA_VOID*                       pUsrHandle;
    LSA_UINT16                      eps_proceed_event;
    PSI_LD_RUNS_ON_TYPE             ldRunsOnType;
} EPS_OPEN_TYPE;
typedef EPS_OPEN_TYPE* EPS_OPEN_PTR_TYPE;

typedef struct eps_close_tag
{
    LSA_VOID*                       pUsrHandle;
    LSA_UINT16                      eps_proceed_event;
} EPS_CLOSE_TYPE;
typedef EPS_CLOSE_TYPE* EPS_CLOSE_PTR_TYPE;

typedef struct
{
	LSA_BOOL                     bEpsInFatal;
	LSA_BOOL                     bBlockingPermitted;
	LSA_BOOL                     bDebug;
	LSA_BOOL					 bShmDebug;
	LSA_BOOL					 bShmHalt;
	LSA_BOOL                     bShutdown;
	LSA_BOOL                     bRunning;
	EPS_HW_INFO_TYPE             HwInfo;
	PSI_HIF_SYS_TYPE             SysLowerLD;
	HIF_HANDLE                   hLowerLD;
	PSI_HIF_SYS_TYPE             SysLowerHD;
	HIF_HANDLE                   hLowerHD;
	LSA_VOID_PTR_TYPE            hSysUpper;
    HIF_HANDLE                   hUpperLD;                 // HIF LD upper handle                      
    PSI_LD_RUNS_ON_TYPE          ldRunsOnType;             // PSI configuration for distributed system 
    LSA_UINT16                   hdCount;                  // number of hds                            
    PSI_HIF_SYS_TYPE             SysUpperLD;               // Handle to upper LD IF                    
    PSI_HD_INPUT_TYPE            hdInput[PSI_CFG_MAX_HD_CNT]; //store for HD information               
    PSI_IM_INPUT_TYPE            imInput;                   // Storage for IM Data                     
    EPS_OPEN_TYPE                uEpsOpen;
    EPS_CLOSE_TYPE               uEpsClose;
    LSA_UINT32                   iOpenUserChannels;

} EPS_DATA_TYPE, *EPS_DATA_PTR_TYPE;

typedef struct eps_shm_fatal_store_tag
{
	LSA_UINT32 bUsed;
	LSA_UINT32 uSize;
	LSA_UINT32 uErrCode;
	LSA_UINT32 uLine;
	LSA_CHAR   sMsg[200];
	LSA_CHAR   sFile[200];
	LSA_CHAR   sComp[200];
	LSA_CHAR   sFunc[200];
	LSA_UINT32 uLsaFatalLen;
	struct
	{
		LSA_UINT16         lsa_component_id;        /* LSA_COMP_ID_* */
		LSA_UINT16         module_id;               /* *_MODULE_ID  */
		LSA_UINT16         line;                    /* line of file */
		LSA_UINT16         error_data_length;       
		LSA_UINT32         error_code[4];
		LSA_UINT8          error_data[1000];
	} lsa_fatal;
} EPS_SHM_FATAL_STORE_TYPE, *EPS_SHM_FATAL_STORE_PTR_TYPE;

typedef struct eps_fatal_store_dump
{
    LSA_UINT8*         pDumpBuf;                /* base address of local buffer */
    LSA_UINT32         lDumpSize;               /* size of local Buffer */
    LSA_UINT32         lBoardType;
    LSA_UINT32         lAsicType;
    LSA_UINT32         lAsicTypeInfo;
    LSA_UINT32         lMemRamType;
}EPS_FATAL_STORE_DUMP_TYPE, *EPS_FATAL_STORE_DUMP_PTR_TYPE;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)
typedef LSA_VOID (*EPS_LOWER_DONE_FCT)(LSA_VOID* p_this, LSA_VOID* p_rb);

#define EPS_RQB_SET_USER_INST_PTR(rb_, val_)  {(rb_)->_user_instance = (val_);}
#define EPS_RQB_GET_USER_INST_PTR(rb_)        ((rb_)->_user_instance)
#define EPS_RQB_SET_USER_CBF_PTR(rb_, val_)  {(rb_)->_user_func = (val_);}
#define EPS_RQB_GET_USER_CBF_PTR(rb_)        ((rb_)->_user_func)

//EPS RQB Header type
typedef struct eps_rqb_header 
{  
	LSA_RQB_HEADER(struct eps_rqb_header*)
} EPS_RQB_TYPE;
#endif

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/
#ifdef EPS_SYS
	EPS_DATA_TYPE g_EpsData;
	EPS_DATA_PTR_TYPE g_pEpsData;
#else
	LSA_EXTERN EPS_DATA_TYPE g_EpsData;
	LSA_EXTERN EPS_DATA_PTR_TYPE g_pEpsData;
#endif

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* eps_sys.c       ----------------------------------------------------------*/
LSA_VOID eps_init(LSA_VOID_PTR_TYPE hSys);
LSA_VOID eps_undo_init ( LSA_VOID );

LSA_VOID eps_open(EPS_OPEN_PTR_TYPE pEpsOpen, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec); 
LSA_VOID eps_close(EPS_CLOSE_PTR_TYPE pEpsClose, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec);
LSA_VOID eps_ld_upper_open(EPS_OPEN_PTR_TYPE pEpsOpen, LSA_UINT16* phDoneEvent);
LSA_VOID eps_ld_upper_close(EPS_CLOSE_PTR_TYPE pEpsClose, LSA_UINT16* phDoneEvent);
LSA_VOID eps_ld_upper_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID eps_ld_upper_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID eps_hif_ld_upper_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent);
LSA_VOID eps_hif_ld_upper_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB);

LSA_VOID   eps_fatal ( LSA_CHAR *file, LSA_INT line, const LSA_CHAR *func, LSA_CHAR *str );
LSA_VOID eps_lsa_fatal ( LSA_CHAR* comp, LSA_UINT16 length, LSA_FATAL_ERROR_TYPE *error_ptr );

LSA_VOID eps_remove_boards( LSA_VOID );
LSA_VOID eps_advanced_init_reboot ( LSA_VOID );
LSA_UINT32 eps_advanced_reboot_is_active( LSA_VOID );

#if (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)
LSA_VOID eps_user_system_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT p_fct );
// open user channel for a PNIO comp
LSA_VOID eps_user_open_channel_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT p_fct );
// close user channel for a PNIO comp
LSA_VOID eps_user_close_channel_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT p_fct );
LSA_VOID eps_user_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT p_fct );
#endif

#ifdef EPS_HD_ONLY
LSA_VOID eps_hif_hd_lower_close(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent);
#endif

LSA_INT eps_sprintf ( LSA_UINT8 *buffer, const char *format, ... );

/*****************************************************************************/
/*  Fatal / Assert                                                           */
/*****************************************************************************/

#ifdef _TEST

#include "..\..\#tests\epsTests.h"

#define EPS_FATAL(reason_)  epsFatalCallback();\
	eps_fatal (__FILE__, __LINE__, __FUNCTION__, "fatal: " #reason_);
#else
#define EPS_FATAL(reason_) eps_fatal (__FILE__, __LINE__, __FUNCTION__, "fatal: " #reason_);
#endif

#if EPS_DEBUG
#define EPS_ASSERT(cond_) { \
	if (!(cond_)) { \
		eps_fatal (__FILE__, __LINE__, __FUNCTION__, "assert: " #cond_); \
	} \
}
#else
#define EPS_ASSERT(cond_) { \
    if (cond_){} \
}
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SYS_H */
