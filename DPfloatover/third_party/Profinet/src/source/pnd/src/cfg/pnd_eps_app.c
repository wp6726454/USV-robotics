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
/*  F i l e               &F: pnd_eps_plf_pndriver.c                    :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDriver Platform Adaption                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  20051
#define PND_MODULE_ID      20051

#include <eps_sys.h>           /* Types / Prototypes / Fucns               */
#include <eps_trc.h>           /* Tracing                                  */
#include <pntrc_inc.h>         /* Tracing                                  */
#include <eps_cp_hw.h>         /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>     /* Pn Device Drv Interface                  */
#include <eps_mem.h>           /*  EPS Local Memory                        */
#include <eps_tasks.h>

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <eps_wpcapdrv.h>      /* WPCAP Support                            */
#endif

#include <eps_stdmacdrv.h>     /* Standard Mac Driver                      */
#include <eps_shm_if.h>        /* Shared Mem Drv Interface                 */
#include <eps_hif_drv_if.h>    /* HIF Drv Interface                        */
#include <eps_noshmdrv.h>      /* Shortcut Drv                             */
#include <eps_hif_short_drv.h> /* HIF Shortcut Drv                         */
#include <eps_app.h>           /* Own Header                               */
#include <eps_rtos.h>          /* RTOS Interface                           */
#include <eps_timer.h>         /* EPS Timers                               */
#include <eps_locks.h>         /* EPS Locks                                */
#include <servusrx.h>          /* DebugSetting                             */

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include <eps_pndevdrv.h>      /* EPSPnDev                                 */
#endif

//---------------------------------------------------------------------------------------------------
LSA_UINT8 InitLogLevels[TRACE_SUBSYS_NUM];


//----------------------------------------------------------------------------------------------------

void pntrc_rqb_done(LSA_VOID* pRQB0)
{
    LSA_UINT16 rc;
    PNTRC_RQB_PTR_TYPE pRQB;
    EPS_ASSERT( pRQB0 != 0);
    pRQB=(PNTRC_RQB_PTR_TYPE)pRQB0;
    PNTRC_FREE_LOCAL_MEM( &rc, pRQB );
    EPS_ASSERT( rc == LSA_RET_OK );
}


LSA_VOID *eps_plf_trace_buffer_change_timeout(LSA_TIMER_ID_TYPE timer_id, LSA_USER_ID_TYPE user_id)
{
    PNTRC_RQB_PTR_TYPE pRQB;

    LSA_UNUSED_ARG(timer_id);
    LSA_UNUSED_ARG(user_id);

    PNTRC_ALLOC_LOCAL_MEM( (void**)&pRQB, sizeof(PNTRC_RQB_TYPE));
    PNTRC_RQB_SET_OPCODE(pRQB,PNTRC_OPC_SWITCH_BUFFERS);
    pRQB->Cbf=pntrc_rqb_done;
    pntrc_request(pRQB);

    return 0;
}

LSA_VOID eps_plf_eps_appready(EPS_SHM_HW_PTR_TYPE pHw, LSA_VOID* pHifHandle)
{
    LSA_UNUSED_ARG(pHifHandle);
    LSA_UNUSED_ARG(pHw);
}


LSA_VOID eps_plf_eps_appshutdown(EPS_SHM_HW_PTR_TYPE pHw)
{
    LSA_UNUSED_ARG(pHw);
}

LSA_VOID eps_plf_trace_buffer_full(LSA_VOID_PTR_TYPE hSys, LSA_VOID* hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength)
{
    PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = (PNIO_DEBUG_SETTINGS_PTR_TYPE)hSys;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT32                   UsedSize;

    LSA_UNUSED_ARG(uLength);

    pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)pBuffer;
    UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);

    if(LSA_NULL != DebugSetting->CbfPntrcBufferFull) //do nothing if cbf is not set
    {
      // pass current trace buffer to user
      DebugSetting->CbfPntrcBufferFull(pBuffer, UsedSize);
    }

    pntrc_buffer_full_done(hTrace, uBufferId);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  EPS_APP_ALLOC_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag) //0 = LD, 1 = HD
{
    //Load XML
    LSA_UNUSED_ARG(file_tag);
}


/*----------------------------------------------------------------------------*/
LSA_VOID  EPS_APP_FREE_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag)
{
    LSA_UNUSED_ARG(file_tag);
}


LSA_VOID EPS_APP_INSTALL_DRV_OPEN(PSI_LD_RUNS_ON_TYPE ldRunsOnType)
{
    LSA_UNUSED_ARG(ldRunsOnType);
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    //----- PORTING HINT -------------------------------------------------------------
    // When using the Intel NIC driver implementation you need to un-comment this line:
    // eps_stdmacdrv_install();           /* Install EPS StdMac-Driver  NOTE: Define EDDS_CFG_HW_INTEL in preprocessor settings    */
    // and comment this line:
    eps_wpcapdrv_install();              /* Install EPS WPCAP-Driver  NOTE: Define EDDS_CFG_HW_PACKET32 in preprocessor settings   */
    #endif
}

LSA_VOID EPS_APP_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw)
{
    EPS_NOSHMDRV_INSTALL_ARGS_TYPE NoShmDrvArgs;
    PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = (PNIO_DEBUG_SETTINGS_PTR_TYPE)hSys;

    NoShmDrvArgs.pAppReadyCbf       = eps_plf_eps_appready;
    NoShmDrvArgs.pAppShutdownCbf    = eps_plf_eps_appshutdown;

    if(DebugSetting->CbfPntrcBufferFull != LSA_NULL)
    {
        pntrc_set_log_levels_from_components(InitLogLevels, DebugSetting->TraceLevels, PNIO_TRACE_COMP_NUM);

        NoShmDrvArgs.sPntrcParam.pInitLogLevels       = &InitLogLevels[0];
        NoShmDrvArgs.sPntrcParam.bUseInitLogLevels    = LSA_TRUE;
    }
    else
    {
        NoShmDrvArgs.sPntrcParam.bUseInitLogLevels    = LSA_FALSE;
        NoShmDrvArgs.sPntrcParam.pInitLogLevels       = LSA_NULL;
    }

    NoShmDrvArgs.pTraceBufferFullCbf              = eps_plf_trace_buffer_full;
    NoShmDrvArgs.sPntrcParam.bIsTopMostCPU        = LSA_TRUE;
    NoShmDrvArgs.sPntrcParam.bUseLocalBinaryTrace = LSA_TRUE;

    /* Install drivers */
    eps_noshmdrv_install(&NoShmDrvArgs); /* Install EPS Shortcut driver DO NOT REMOVE */

    eps_hif_short_drv_install();         /* Install HIF Shortcut driver DO NOT REMOVE */

    EPS_ASSERT(pEpsHw != LSA_NULL);

    /* Init Hw structure*/
    eps_memset(pEpsHw, 0, sizeof(*pEpsHw));

    /* Local Memory */
    //----- PORTING HINT --------------------------------------------------------------
    pEpsHw->LocalMemPool.lSize                          = 0x3400000; //Size of local mem used by PN Driver
    pEpsHw->LocalMemPool.pBase                          = (LSA_UINT8*)malloc(pEpsHw->LocalMemPool.lSize);
    // When using eps_stdmacdrv_install() you need to provide the physical address of the local mem
    pEpsHw->LocalMemPool.uPhyAddr                       = (LSA_UINT32)pEpsHw->LocalMemPool.pBase;
    //---------------------------------------------------------------------------------

    /* Fast Memory */
    pEpsHw->FastMemPool.uPhyAddr                        = 0x00000000; /* No Specialized Fast Mem Used */
    pEpsHw->FastMemPool.pBase                           = 0x00000000; /* Take Fast Mem From Local Mem */
    pEpsHw->FastMemPool.lSize                           = 0x10000;

    //TODO Hack
    #if (EPS_PLF == EPS_PLF_LINUX_X86)
    //PNDriver Linux
    eps_pndevdrv_install();
    #endif
}

LSA_VOID EPS_APP_UNDO_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw)
{
    LSA_UNUSED_ARG(hSys);

    free(pEpsHw->LocalMemPool.pBase);
}

LSA_VOID EPS_APP_KILL_EPS(LSA_VOID)
{
    EPS_APP_FATAL_HOOK();

    if(EPS_APP_KILL_EPS != 0) // This code is here to prevent warnings
    {
        exit(255); /* c-lib exit! */
    }    
}

LSA_VOID EPS_APP_FATAL_HOOK(LSA_VOID)
{
	// HINT for developer: You can set your breakpoint in this function. If PND_DEBUG is set and an internal error occurs, this function is called. 
	// You can get get the callstack to see where the FATAL error occured.
	// If a debugger is attached, you can set a breakpoint using this implementation (that means the breakpoint is set every time).

	//__asm{int 3}; /* debugger "user-breakpoint" */

	// Note: You should only use an implementation like this if a debugger is attached. We strongly recommend not to call __asm{int 3}; 
	// in a software used for productive use cases.
}

/*----------------------------------------------------------------------------*/
LSA_VOID EPS_APP_SAVE_DUMP_FILE(EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo)
{
	LSA_UNUSED_ARG(pDumpStoreInfo);
}

LSA_VOID EPS_APP_HALT(LSA_VOID)
{

}