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
/*  F i l e               &F: eps_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS System Startup + Error Handling                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20001
#define EPS_MODULE_ID    LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#define EPS_SYS /* see lsa_cfg.h */


#include <psi_inc.h>

#include <eps_rtos.h>      /* RTOS Interface          */
#include <eps_sys.h>       /* Types / Prototypes      */
#include <eps_cp_hw.h>     /* CP PSI adation          */
#include <eps_cp_mem.h>    /* CP Mem Pool adaption    */
#include <eps_locks.h>     /* EPS Locks               */
#include <eps_events.h>    /* EPS Events              */
#include <eps_isr.h>       /* EPS Interrupts          */
#include <eps_mem.h>       /* Local Memory Handling   */
#include <eps_trc.h>       /* Tracing                 */
#include <eps_tasks.h>     /* EPS Threads             */
#include <eps_shm_if.h>    /* Shared Memory Interface */
#include <eps_plf.h>       /* EPS Platform Interface  */
#include <eps_app.h>       /* EPS Application Interface*/           
#include <eps_timer.h>     /* Timer System Interface  */
#include <eps_pn_drv_if.h> /* EPS PN Driver Interface */
#include <eps_hif_drv_if.h> /* EPS HIF Driver Interface */
#include <eps_pndevdrv.h>   /* EPS PNDEV Driver Interface */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <eps_pn_imcea_drv.h> /* EPS IMCEA Driver Interface */
#endif

#if ( PSI_CFG_USE_PNTRC == 1 )
#include <pntrc_inc.h>     /* PNTRC Interface         */
#endif

EPS_FILE_SYSTEM_EXTENSION(EPS_MODULE_ID)

/*------------------------------------------------------------------------------
//	Forwared decl
//----------------------------------------------------------------------------*/

typedef struct
{ 
    LSA_VOID *_user_arg;
    EPS_LOWER_DONE_FCT _user_func;

} EPS_RQB_USER_ID_TYPE, *EPS_RQB_USER_ID_PTR_TYPE;

static LSA_VOID eps_system_request_lower_done(LSA_VOID* pRQBData);
static LSA_VOID eps_system_request_lower(LSA_VOID_PTR_TYPE pRQBData, LSA_VOID_PTR_TYPE pInst, EPS_LOWER_DONE_FCT pFct, LSA_UINT16 mbox_id, PSI_REQUEST_FCT req_fct);


/*------------------------------------------------------------------------------
 //	Module Globals
 //----------------------------------------------------------------------------*/
EPS_SHM_HW_PTR_TYPE g_pEpsShmHw = LSA_NULL;

/*------------------------------------------------------------------------------
 // Advanced Reboot Globals
 //----------------------------------------------------------------------------*/
#define EPS_ADVANCED_REBOOT_STACK_SIZE 8192

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_IMCEADRV_PRINTF
#endif

// thread vars
pthread_t       lThIdAdvRebootShutdown = 0;
LSA_UINT32      lThIdAdvRebootShutdownStackAdr[EPS_ADVANCED_REBOOT_STACK_SIZE];
pthread_t       lThIdAdvRebootMain     = 0;
LSA_UINT32      lThIdAdvRebootMainStackAdr[EPS_ADVANCED_REBOOT_STACK_SIZE];

LSA_UINT32      bGlobIsEPSReboot = LSA_FALSE;

/*------------------------------------------------------------------------------
 //	Software Fatal Handling
 //----------------------------------------------------------------------------*/

LSA_VOID eps_store_fatal_info(EPS_SHM_HW_PTR_TYPE pEpsShmHw, EPS_SHM_FATAL_PAR_PTR_TYPE pFatal)
{
  EPS_SHM_FATAL_STORE_PTR_TYPE pFatalStore;
  LSA_UINT32 i;

	if( ( pEpsShmHw != LSA_NULL ) &&
		( pEpsShmHw->sFatal.pBase != LSA_NULL ) &&
		( pEpsShmHw->sFatal.uSize >= sizeof(EPS_SHM_FATAL_STORE_TYPE) ) )
  {
    if (pFatal != LSA_NULL)
    {
      pFatalStore = (EPS_SHM_FATAL_STORE_PTR_TYPE) pEpsShmHw->sFatal.pBase;

      if (pFatalStore->bUsed != 1)
      {
        pFatalStore->uErrCode = pFatal->uErrCode;
        pFatalStore->uLine = pFatal->uLine;

        if (pFatal->pFile != LSA_NULL)
          for (i = 0; (i < 200) || (pFatal->pFile[i]) != 0; i++)
            pFatalStore->sFile[i] = pFatal->pFile[i];

        if (pFatal->pMsg != LSA_NULL)
          for (i = 0; (i < 200) || (pFatal->pMsg[i]) != 0; i++)
            pFatalStore->sMsg[i] = pFatal->pMsg[i];

        pFatalStore->uLsaFatalLen = pFatal->uLsaFatalLen;

        if (pFatal->pComp != LSA_NULL)
          for (i = 0; (i < 200) || (pFatal->pComp[i]) != 0; i++)
            pFatalStore->sComp[i] = pFatal->pComp[i];
        
        if (pFatal->pFunc != LSA_NULL)
          for (i = 0; (i < 200) || (pFatal->pFunc[i]) != 0; i++)
            pFatalStore->sFunc[i] = pFatal->pFunc[i];

        if (pFatal->pLsaFatal != LSA_NULL)
        {
          pFatalStore->lsa_fatal.error_code[0] = pFatal->pLsaFatal->error_code[0];
          pFatalStore->lsa_fatal.error_code[1] = pFatal->pLsaFatal->error_code[1];
          pFatalStore->lsa_fatal.error_code[2] = pFatal->pLsaFatal->error_code[2];
          pFatalStore->lsa_fatal.error_code[3] = pFatal->pLsaFatal->error_code[3];
          pFatalStore->lsa_fatal.error_data_length = pFatal->pLsaFatal->error_data_length;
          pFatalStore->lsa_fatal.line = pFatal->pLsaFatal->line;
          pFatalStore->lsa_fatal.lsa_component_id = pFatal->pLsaFatal->lsa_component_id;
          pFatalStore->lsa_fatal.module_id = pFatal->pLsaFatal->module_id;

          if (pFatal->pComp != LSA_NULL)
            for (i = 0; (i < 1000) && (i < pFatal->pLsaFatal->error_data_length); i++)
							pFatalStore->lsa_fatal.error_data[i] = ((LSA_UINT8*)pFatal->pLsaFatal->error_data_ptr)[i];
        }

        pFatalStore->uSize = sizeof(EPS_SHM_FATAL_STORE_TYPE);
        pFatalStore->bUsed = 1;
      }
    }
    else
    {
      for (i = 0; i < sizeof(EPS_SHM_FATAL_STORE_TYPE); i++)
      {
        pEpsShmHw->sFatal.pBase[i] = 0;
      }
    }
  }
}

LSA_VOID eps_fatal(LSA_CHAR *file, LSA_INT line, const LSA_CHAR *func, LSA_CHAR *str) /* Standard Fatal Err (like Assert) */
{
	if ( !g_pEpsData->bEpsInFatal )   //FATAL already handled, usecase FATAL in FATAL ?
	{
		EPS_SHM_FATAL_PAR_TYPE sFatal;
        EPS_PNDEV_HW_PTR_TYPE ppHwInst;
		LSA_UINT8 hd_id;

        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL," >> eps_fatal()");

        g_pEpsData->bEpsInFatal = LSA_TRUE;

		if (g_pEpsShmHw != LSA_NULL) /* If shared mem available, store information */
		{
			eps_memset(&sFatal, 0, sizeof(sFatal));
			sFatal.uErrCode = EPS_SHM_ERR_CODE_FATAL;
			sFatal.uLine = line;
			sFatal.pFile = file;
			sFatal.pMsg = str;
			sFatal.pFunc = func;

			eps_store_fatal_info(g_pEpsShmHw, &sFatal);

            g_pEpsShmHw->FatalError(g_pEpsShmHw, sFatal);
        }

        eps_tasks_fatal();

			for(hd_id=0; hd_id<(PSI_CFG_MAX_HD_CNT+1); hd_id++)
			{
				if((eps_pndev_if_get(hd_id, &ppHwInst) != EPS_PNDEV_RET_DEV_NOT_FOUND) && (g_pEpsData->uEpsOpen.write_dumpfiles_on_fatal[hd_id]))
				{
                EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
                  //save dump
					ppHwInst->SaveDump(ppHwInst); 
                  
                  EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_fatal - disabling interrupts for hd_id = %d", hd_id);
                  
                IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;

                ppHwInst->DisableIsr(ppHwInst,&IsrDesc);

	            if(ppHwInst->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
	            {
		            IsrDesc = EPS_PNDEV_ISR_PN_NRT;
		            ppHwInst->DisableIsr(ppHwInst, &IsrDesc);
				}
			}
		}

        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "!!!EPS FATAL!!! => Msg(%s)", str);
  		EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "!!!EPS FATAL!!! => File(%s)", file);
  		EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "!!!EPS FATAL!!! => Function(%s)", func);
  		EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"!!!EPS FATAL!!! => Line(%d)", line);
  		EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL," << eps_fatal()");

        pntrc_fatal_occurred();

        EPS_APP_FATAL_HOOK();

		EPS_APP_KILL_EPS(); /* Let EPS die here! */
	}
}

LSA_VOID eps_lsa_fatal(LSA_CHAR* comp, LSA_UINT16 length, LSA_FATAL_ERROR_TYPE *error_ptr) /* LSA Fatal Err */
{
    if ( !g_pEpsData->bEpsInFatal )   //FATAL already handled, usecase FATAL in FATAL ?
    {
        EPS_SHM_FATAL_PAR_TYPE sFatal;
        EPS_PNDEV_HW_PTR_TYPE ppHwInst;
	    LSA_UINT8 hd_id;

        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL," >> eps_lsa_fatal()");
        
        g_pEpsData->bEpsInFatal = LSA_TRUE;
        
        if (g_pEpsShmHw != LSA_NULL) /* If shared mem available, store information */
        {
            eps_memset(&sFatal, 0, sizeof(sFatal));
            sFatal.uErrCode = EPS_SHM_ERR_CODE_FATAL_LSA;
            sFatal.pComp = comp;
            sFatal.pLsaFatal = error_ptr;
            sFatal.uLsaFatalLen = length;

            eps_store_fatal_info(g_pEpsShmHw, &sFatal);

            g_pEpsShmHw->FatalError(g_pEpsShmHw, sFatal);
        }

        eps_tasks_fatal();

        for(hd_id=0; hd_id<(PSI_CFG_MAX_HD_CNT+1); hd_id++)
	    {
		    if((eps_pndev_if_get(hd_id, &ppHwInst) != EPS_PNDEV_RET_DEV_NOT_FOUND) && (g_pEpsData->uEpsOpen.write_dumpfiles_on_fatal[hd_id]))
		    {
                EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;
                // save dump
			    ppHwInst->SaveDump(ppHwInst); 
                
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_lsa_fatal - disabling interrupts for hd_id = %d", hd_id);
                IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;

                ppHwInst->DisableIsr(ppHwInst,&IsrDesc);

	            if(ppHwInst->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
	            {
		            IsrDesc = EPS_PNDEV_ISR_PN_NRT;
		            ppHwInst->DisableIsr(ppHwInst, &IsrDesc);
	            }
		    }
	    }

        EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL,"!!!EPS LSA FATAL!!! => CompId(%d) ModuleId(%d) Line(%d) ErrCode(%d)", error_ptr->lsa_component_id, error_ptr->module_id, error_ptr->line, error_ptr->error_code);
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL," << eps_lsa_fatal()");

	    pntrc_fatal_occurred(); /* From now on, no Traces will be recorded */

        EPS_APP_FATAL_HOOK();

	    EPS_APP_KILL_EPS(); /* Let EPS die here! */
    }
}

/*------------------------------------------------------------------------------
 //	Init / Startup
 //----------------------------------------------------------------------------*/

static LSA_VOID eps_startup_done(HIF_HANDLE hH)
{
	EPS_ASSERT(g_pEpsData != LSA_NULL);

	/* Signalize FW Ready and Check Driver State */
	g_pEpsShmHw->AppReady(g_pEpsShmHw, &hH);	
}

#ifdef EPS_HD_ONLY

static LSA_VOID eps_hif_hd_lower_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
	LSA_UINT16 rc;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT(g_pEpsData  != LSA_NULL);
	EPS_ASSERT(pRQB        != LSA_NULL);

	EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
	EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_HD_LOWER_OPEN);

	g_pEpsData->hLowerHD = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT(rc == LSA_RET_OK);

	eps_startup_done(g_pEpsData->hLowerHD); /* System ready for open! - Send Signal to PnDevDrv */

	eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_hd_lower_close_done(LSA_VOID* arg, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 rc;
	LSA_UINT16* phDoneEvent = (LSA_UINT16*)arg;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT(pRQB     != LSA_NULL);

	EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
	EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_HD_LOWER_CLOSE);

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT(rc == LSA_RET_OK);

	eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_hd_lower_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE pRQB;
	PSI_HIF_SYS_PTR_TYPE pSysLowerHD;

	EPS_ASSERT(pEpsData != LSA_NULL);

	/* Create HIF LD Lower Device */
	pSysLowerHD = &pEpsData->SysLowerHD;

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB));
	EPS_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_HD_LOWER_OPEN );
	HIF_RQB_SET_HANDLE ( pRQB, 0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	pSysLowerHD->hd_nr         = 1;
	pSysLowerHD->shortcut_mode = LSA_FALSE;
	pSysLowerHD->is_upper      = LSA_FALSE;

	pRQB->args.dev_lower_open.hSysDev = pSysLowerHD;
	pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; /* out from HIF */
	pRQB->args.dev_lower_open.Cbf     = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

	eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_hd_lower_open_done, PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system);
}

LSA_VOID eps_hif_hd_lower_close(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE pRQB;

	EPS_ASSERT(pEpsData != LSA_NULL);

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB));
	EPS_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_HD_LOWER_CLOSE);
	HIF_RQB_SET_HANDLE ( pRQB, 0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	pRQB->args.dev_close.hH  = pEpsData->hLowerHD;
	pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

	eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_hd_lower_close_done, PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system);
}

#else //LD

LSA_VOID eps_hif_ld_upper_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
   	LSA_UINT16 rc;
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

	EPS_ASSERT( pRQB != 0 );
	EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == HIF_OK );
	EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == HIF_OPC_LD_UPPER_OPEN );

	g_pEpsData->hUpperLD = pRQB->args.dev_ld_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT( rc == LSA_RET_OK );

	// Signal done
	eps_set_event( *phDoneEvent );
}


LSA_VOID eps_hif_ld_lower_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
	LSA_UINT16 rc;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT(g_pEpsData  != LSA_NULL);
	EPS_ASSERT(pRQB        != LSA_NULL);

	EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
	EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_LD_LOWER_OPEN);

	g_pEpsData->hLowerLD = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT(rc == LSA_RET_OK);

	eps_startup_done(g_pEpsData->hLowerLD);   /* System ready for open! - Send Signal to PnDevDrv */

	eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_ld_lower_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE pRQB;
	PSI_HIF_SYS_PTR_TYPE pSysLowerLD;

	EPS_ASSERT(pEpsData != LSA_NULL);
     
	/* Create HIF LD Lower Device */
	pSysLowerLD = &pEpsData->SysLowerLD;

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB));
	EPS_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_LD_LOWER_OPEN );
	HIF_RQB_SET_HANDLE ( pRQB, 0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	pSysLowerLD->hd_nr = 0;

    if(PSI_LD_RUNS_ON_LIGHT == pEpsData->ldRunsOnType)
    {
	    pSysLowerLD->shortcut_mode = LSA_TRUE;
    }
    else
    {
        pSysLowerLD->shortcut_mode = LSA_FALSE;
    }

    pSysLowerLD->ldRunsOnType = pEpsData->ldRunsOnType;

	pSysLowerLD->is_upper     = LSA_FALSE;
	
	pRQB->args.dev_lower_open.hSysDev = pSysLowerLD;
	pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; /* out from HIF */
	pRQB->args.dev_lower_open.Cbf     = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

	eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_lower_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}

LSA_VOID eps_hif_ld_lower_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
	LSA_UINT16 rc;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT(pRQB        != LSA_NULL);

	EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
	EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_LD_LOWER_CLOSE);

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT(rc == LSA_RET_OK);

	eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_ld_lower_close(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE pRQB;

	EPS_ASSERT(pEpsData != LSA_NULL);

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB));
	EPS_ASSERT(pRQB != LSA_NULL);

	HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_LD_LOWER_CLOSE);
	HIF_RQB_SET_HANDLE ( pRQB, 0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

	pRQB->args.dev_close.hH  = pEpsData->hLowerLD;
	pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

	eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_lower_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}

#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
// Close HIF LD upper connection
LSA_VOID eps_hif_ld_upper_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
   	LSA_UINT16 rc;
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

	EPS_ASSERT( pRQB != 0 );
	EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == HIF_OK );
    EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == HIF_OPC_LD_UPPER_CLOSE );

	g_pEpsData->hUpperLD = pRQB->args.dev_close.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB );
	EPS_ASSERT( rc == LSA_RET_OK );

	// Signal done
	eps_set_event( *phDoneEvent );
}


static LSA_VOID eps_hif_ld_upper_close(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE pRQB;

	HIF_ALLOC_LOCAL_MEM((void**)&pRQB, sizeof(*pRQB) );
	EPS_ASSERT(pRQB);

	HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_LD_UPPER_CLOSE );
	HIF_RQB_SET_HANDLE( pRQB,  0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF);

	pRQB->args.dev_close.hH =  pEpsData->hUpperLD;;

	pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_upper_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}
#endif

LSA_VOID eps_ld_upper_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
    EPS_OPEN_TYPE * pEpsLdOpenOut = &g_pEpsData->uEpsOpen;

	LSA_UINT16 rc; 
	EPS_SYS_PTR_TYPE dummy_sys = {0};
    PSI_UPPER_LD_OPEN_PTR_TYPE pOpen;
    LSA_UINT32 nrOfHD;

    LSA_UINT16 hd_idx;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT( pRQB != 0 );
	EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == PSI_OK );
	EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == PSI_OPC_LD_OPEN_DEVICE );

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "< eps_ld_upper_open_done - PSI LD open done");
	
	pOpen = &((PSI_UPPER_RQB_PTR_TYPE)pRQB)->args.ld_open;

	// Update all INI stores with the calculated values for each PNIO-IF
	nrOfHD = pOpen->hd_count;

	for ( hd_idx = 0; hd_idx < nrOfHD; hd_idx++ )
	{
        pEpsLdOpenOut->hd_out[hd_idx] = pOpen->hd_out[hd_idx];
		pEpsLdOpenOut->hd_args[hd_idx] = pOpen->hd_args[hd_idx];
	}

	PSI_FREE_LOCAL_MEM(&rc, pRQB, dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
	EPS_ASSERT (rc == LSA_RET_OK);

    eps_set_event(*phDoneEvent);
}

void eps_ld_upper_open(EPS_OPEN_PTR_TYPE pEpsOpen, LSA_UINT16* phDoneEvent) 
{
    LSA_UINT16 iSockDetails;
    LSA_UINT16 idx;

	PSI_UPPER_RQB_PTR_TYPE     pRQB;
	PSI_UPPER_LD_OPEN_PTR_TYPE pOpen;

	LSA_USER_ID_TYPE    dummy_id  = {0};
	EPS_SYS_PTR_TYPE    dummy_sys = {0};
    g_pEpsData->uEpsOpen        = *pEpsOpen;    

    PSI_ALLOC_LOCAL_MEM((void**)&pRQB, dummy_id, sizeof(*pRQB), &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
	EPS_ASSERT(pRQB != NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_open - Calling PSI with OPC PSI_OPC_LD_OPEN_DEVICE");

	PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_LD_OPEN_DEVICE );
	PSI_RQB_SET_HANDLE( pRQB, 0 );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

	pOpen = &pRQB->args.ld_open;

	pOpen->hH       = g_pEpsData->hUpperLD;
    pOpen->hd_count = g_pEpsData->hdCount; 
    pOpen->im_args  = g_pEpsData->imInput;

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_open - hd_count = %d", pOpen->hd_count ); 

	for( iSockDetails = 0; iSockDetails < PSI_CFG_MAX_SOCK_APP_CHANNELS; iSockDetails++) 
	{
		pOpen->sock_args.sock_app_ch_details[iSockDetails] = g_pEpsData->uEpsOpen.sock_args.sock_app_ch_details[iSockDetails];
	}

	for ( idx = 0; idx < g_pEpsData->hdCount; idx++ )
	{
        pOpen->hd_args[idx]             = g_pEpsData->hdInput[idx];
	}

	pOpen->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;
    
    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}

LSA_VOID eps_close(EPS_CLOSE_PTR_TYPE pEpsClose, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec)
{
    LSA_UINT16 retVal;
    EPS_UPPER_CALLBACK_FCT_PTR_TYPE pEpsLdClose =  (EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(pCbf);
    
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close called with TimeOutSec value = %d (0=Infinite)", TimeOutSec);

    retVal = eps_alloc_event(&pEpsClose->eps_proceed_event);
	EPS_ASSERT(retVal == LSA_RET_OK);
    
	// *** 1. CLOSE LD UPPER ***************************************************************************
    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
    eps_ld_upper_close(pEpsClose, &pEpsClose->eps_proceed_event);
	retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
	eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close - EPS LD Upper close done");
    #endif

    // *** 2. CLOSE HIF LD UPPER ***********************************************************************
    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
    eps_hif_ld_upper_close(g_pEpsData, &pEpsClose->eps_proceed_event);     
    retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
    eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close - HIF LD Upper close done");
    #endif
    // *************************************************************************************************
    
    // *** 3. CLOSE HIF LD LOWER -> Disable HIF receive ************************************************
	#ifdef EPS_HD_ONLY
    eps_hif_hd_lower_close(g_pEpsData, &pEpsClose->eps_proceed_event);      /* Undo Init HIF HD Lower Device */
    #else
    eps_hif_ld_lower_close(g_pEpsData, &pEpsClose->eps_proceed_event);      /* Undo Init HIF LD Lower Device */
    #endif
	retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
	eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close - HIF LD/HD Lower close done");
    // *************************************************************************************************
    
    eps_remove_boards();
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close - removed all boards");

    retVal = eps_free_event(pEpsClose->eps_proceed_event);
	EPS_ASSERT(retVal == LSA_RET_OK);

    psi_undo_init();                /* closes Mailboxes/Stack => will end threads */

    if (pEpsLdClose) 
    {
        //Asynchron
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close - calling Application");
        pEpsLdClose(pEpsClose, g_pEpsData->uEpsClose.pUsrHandle);
    }
}


LSA_VOID eps_ld_upper_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 rc;
	EPS_SYS_PTR_TYPE dummy_sys = {0};
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

	EPS_ASSERT(phDoneEvent != LSA_NULL);
	EPS_ASSERT( pRQB != 0 );
	EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == PSI_OK );
	EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == PSI_OPC_LD_CLOSE_DEVICE );

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "< eps_ld_upper_close_done - PSI LD close done");
    
	PSI_FREE_LOCAL_MEM(&rc, pRQB, dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
	EPS_ASSERT (rc == LSA_RET_OK);

    eps_set_event(*phDoneEvent);


}

void eps_ld_upper_close(EPS_CLOSE_PTR_TYPE pEpsClose, LSA_UINT16* phDoneEvent)
{
	PSI_UPPER_RQB_PTR_TYPE      pRQB;
	PSI_UPPER_LD_CLOSE_PTR_TYPE pClose;

	LSA_USER_ID_TYPE   dummy_id  = {0};
	EPS_SYS_PTR_TYPE dummy_sys = {0};

    g_pEpsData->uEpsClose = *pEpsClose;

	PSI_ALLOC_LOCAL_MEM((void**)&pRQB, dummy_id, sizeof(*pRQB), &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
	EPS_ASSERT(pRQB);

	  EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_close - calling PSI with OPC PSI_OPC_LD_CLOSE_DEVICE");
	
	PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_LD_CLOSE_DEVICE );
	PSI_RQB_SET_HANDLE( pRQB, 0 );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

	pClose = &pRQB->args.ld_close;

	pClose->hH                         = g_pEpsData->hUpperLD; 
	pClose->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}


LSA_VOID eps_hif_ld_upper_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
	HIF_RQB_PTR_TYPE    pRQB;

	LSA_UINT16          hdCount = pEpsData->hdCount; 
    LSA_BOOL            shortcutMode;
    LSA_UINT16          idx;
    LSA_UINT16          ifIdx;

	EPS_ASSERT((hdCount > 0 ) && ( hdCount <= PSI_CFG_MAX_HD_CNT ));

	HIF_ALLOC_LOCAL_MEM((void**)&pRQB, sizeof(*pRQB) );
	EPS_ASSERT(pRQB);

	HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_LD_UPPER_OPEN );
	HIF_RQB_SET_HANDLE( pRQB,  0 );
	HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF);

	// Prepare the hSysDev for HIF LD upper
    if(PSI_LD_RUNS_ON_LIGHT == g_pEpsData->ldRunsOnType)
    {
        shortcutMode = LSA_TRUE;
    }
    else // the LD is located on a firmware, e.g. Advanced or Basic
    {
        shortcutMode = LSA_FALSE;
    }

	// Prepare the upper hSysDev
	pEpsData->SysUpperLD.hd_nr = 0;
    pEpsData->SysUpperLD.is_upper   = LSA_TRUE; 

	if ( shortcutMode )
	{
		// Note: for upper no location information is necessary
		pEpsData->SysUpperLD.shortcut_mode    = shortcutMode;
        pEpsData->SysUpperLD.hif_lower_handle = pEpsData->hLowerLD;
	}
	else
	{
		// LD upper connection must be opened over driver, so location info must be added
        pEpsData->SysUpperLD.hd_sys_id.hd_location = g_pEpsData->hdInput[0].hd_location;
	}

    pEpsData->SysUpperLD.ldRunsOnType = g_pEpsData->ldRunsOnType;

	pRQB->args.dev_ld_open.hSysDev  = &pEpsData->SysUpperLD;
    pRQB->args.dev_ld_open.hd_count = pEpsData->hdCount;
	pRQB->args.dev_ld_open.hH       = PSI_INVALID_HANDLE; // out from HIF

	for ( idx = 0; idx < pEpsData->hdCount; idx++ )
	{
		PSI_HD_INPUT_TYPE hdInp = pEpsData->hdInput[idx]; // m_pHwIniStore->GetHdEntry( idx+1 );

		pRQB->args.dev_ld_open.hd_args[idx].hd_id    = hdInp.hd_id;
		pRQB->args.dev_ld_open.hd_args[idx].nr_of_if = hdInp.nr_of_if;

		for ( ifIdx = 0; ifIdx < hdInp.nr_of_if; ifIdx++ )
		{
			pRQB->args.dev_ld_open.hd_args[idx].pnio_if[ifIdx].if_id = hdInp.pnio_if[ifIdx].if_id;
		}
	}

	pRQB->args.dev_ld_open.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;
    
    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_upper_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}

#endif  //ENDIF !EPS_HD_ONLY


static LSA_VOID eps_system_request_lower_done(LSA_VOID* pRQBData)
{
	struct eps_header *pRQB = (struct eps_header*)pRQBData;
	EPS_RQB_USER_ID_PTR_TYPE pEpsUserId;

	EPS_ASSERT(pRQB != LSA_NULL);

	pEpsUserId = (EPS_RQB_USER_ID_PTR_TYPE) LSA_RQB_GET_USER_ID_PTR(pRQB);

	EPS_ASSERT(pEpsUserId != LSA_NULL);
	EPS_ASSERT(pEpsUserId->_user_func != LSA_NULL);

	pEpsUserId->_user_func(pEpsUserId->_user_arg, pRQBData);

	eps_mem_free(pEpsUserId, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
}


static LSA_VOID eps_system_request_lower(LSA_VOID_PTR_TYPE pRQBData, LSA_VOID_PTR_TYPE pArg, EPS_LOWER_DONE_FCT pFct, LSA_UINT16 mbox_id, PSI_REQUEST_FCT req_fct)
{
	struct eps_header *pRQB = (struct eps_header*)pRQBData;
	EPS_RQB_USER_ID_PTR_TYPE pEpsUserId;

	/* Send Request to HIF System*/

	EPS_ASSERT(pRQB != LSA_NULL);
	EPS_ASSERT(pFct != LSA_NULL);

	pEpsUserId = (EPS_RQB_USER_ID_PTR_TYPE) eps_mem_alloc(sizeof(*pEpsUserId), LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);

	EPS_ASSERT(pEpsUserId != LSA_NULL);

	LSA_RQB_SET_USER_ID_PTR(pRQB, pEpsUserId);

	pEpsUserId->_user_arg  = pArg;
	pEpsUserId->_user_func = pFct;

	psi_request_start( mbox_id, req_fct, pRQBData );
}


/*----------------------------------------------------------------------------*/
LSA_VOID eps_remove_boards( LSA_VOID )
{
	LSA_INT board;

    // Remove all added PN boards
	for ( board = PSI_CFG_MAX_HD_CNT; board >= 0; board-- )
	{
        eps_hw_close_hd((LSA_UINT16)board);
	}
}


/**
 * initialize the EPS Framework. Calls the OUT-Macro EPS_APP_INIT that the application must provide.
 * @param hSys SystemHandle, not used right now in adonis system
*/
LSA_VOID eps_init(LSA_VOID_PTR_TYPE hSys) 
{
    #if ( PSI_CFG_USE_PNTRC == 1 ) /* Setup PSI PNTRC Init Data*/
 	PNTRC_INIT_TYPE pntrc_init_struct;
    #endif

	EPS_SHM_OPEN_OPTION_TYPE sShmOpenOption;
	LSA_UINT16 retVal;

	g_pEpsData = &g_EpsData;

    eps_memset(g_pEpsData, 0, sizeof(EPS_DATA_TYPE));        

	g_pEpsData->bEpsInFatal = LSA_FALSE;
	g_pEpsData->bBlockingPermitted = LSA_TRUE;
	g_pEpsData->bShutdown   = LSA_FALSE;
	g_pEpsData->bRunning    = LSA_TRUE;
	g_pEpsData->hSysUpper   = hSys;

	eps_locks_init();                          /* Init Eps Critical Section Module */

	eps_hif_drv_if_init();                     /* Init HIF Driver Interface */

	eps_pndev_if_init();                       /* Init Eps Network Card Driver Interface */

	eps_hw_init();                          /* Init HW boards (handles reachable comm boards) */
	EPS_APP_INIT(hSys, &g_pEpsData->HwInfo);   /* Call the App to initialize the SHM IF Driver and HIF Drivers */

	retVal = eps_timer_init_tgroup0();                 /* Init LSA Timer System Thread Group 0 */
	EPS_ASSERT(retVal == LSA_RSP_OK);
	retVal = eps_timer_init_tgroup1();                 /* Init LSA Timer System Thread Group 1 */
	EPS_ASSERT(retVal == LSA_RSP_OK);

	/* Open Shared Memory Interface */
	eps_memset(&sShmOpenOption, 0, sizeof(sShmOpenOption));
	sShmOpenOption.bEnableWatchdog = LSA_TRUE;
	#ifdef EPS_HD_ONLY
	sShmOpenOption.uHdNr = 1;	
	#else
	sShmOpenOption.uHdNr = 0;
	#endif	

	retVal = eps_shm_if_open(hSys, &sShmOpenOption, &g_pEpsShmHw); /* Open the EPS SHM IF implementation instance using the EPS SHM API*/
	EPS_ASSERT(retVal == EPS_SHM_RET_OK);
	EPS_ASSERT(g_pEpsShmHw != LSA_NULL);
	EPS_PLF_REGISTER_SHM_IF(g_pEpsShmHw);
	
	eps_store_fatal_info(g_pEpsShmHw, LSA_NULL); /* Init Fatal Buffer */
	
	if(g_pEpsShmHw->bShmHalt)
	{
		// use fatal hook to halt on startup when shared memory debugging boards
	    EPS_APP_HALT();
	}

	/* Init PNTRC */
#if ( PSI_CFG_USE_PNTRC == 1 ) /* Setup PSI PNTRC Init Data*/
	pntrc_init_struct.bIsTopMostCPU       = g_pEpsShmHw->sPntrcParam.bIsTopMostCPU;
	pntrc_init_struct.bUseLocalBinaryTrace= LSA_TRUE; // g_pEpsShmHw->sPntrcParam.bUseLocalBinaryTrace; TODO !!!AH
	pntrc_init_struct.bUseInitLogLevels   = g_pEpsShmHw->sPntrcParam.bUseInitLogLevels;
	pntrc_init_struct.pInitLogLevels      = g_pEpsShmHw->sPntrcParam.pInitLogLevels;
	pntrc_init_struct.Size                = g_pEpsShmHw->sPntrc1.uSize;
	pntrc_init_struct.Ptr		          = (LSA_UINT8*)g_pEpsShmHw->sPntrc1.pBase;

	pntrc_init(&pntrc_init_struct);
#endif

	
	/* Init Local Memory Management with standard and fast mem pools */
	retVal = eps_mem_init((LSA_UINT32*)g_pEpsData->HwInfo.LocalMemPool.pBase, g_pEpsData->HwInfo.LocalMemPool.lSize, (LSA_UINT32*)g_pEpsData->HwInfo.FastMemPool.pBase, g_pEpsData->HwInfo.FastMemPool.lSize);
	EPS_ASSERT(retVal == LSA_RSP_OK);

    eps_events_init();                      /* Init Events */

    eps_tasks_init();                       /* Init PNIO threads and Thread System */

#if (PSI_CFG_USE_PNTRC == 1)
	pntrc_startstop( 1 , g_pEpsShmHw );
#endif
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_init() - trace system is now active for this instance");
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "< eps_init()");
}

/**
   eps open()

   pEpsOpen

   pCbf:        If a function ptr was given -> asynchron, else it was waiting -> synchron

   EPS variant:
    * PSI_LD_RUNS_ON_LIGHT        - PCIOX_Light, PCIOX with HD@FW, PNDriver. Used to set short circuit mode 
    * PSI_LD_RUNS_ON_ADVANCED     - PCIOX_Advanced, Used to open a LD Upper IF to the Advanced FW using IMCEA
    * PSI_LD_RUNS_ON_BASIC        - PCIOX_Basic, Used to open an LD Upper IF to the Basic FW
*/

LSA_VOID eps_open(EPS_OPEN_PTR_TYPE pEpsOpen, EPS_UPPER_CALLBACK_FCT_PTR_TYPE pCbf, LSA_UINT16 TimeOutSec)
{
    LSA_UINT16 retVal;
    LSA_UINT16 eps_startup_proceed_event;    

    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
    LSA_UINT16 idx;

    EPS_ASSERT(pEpsOpen->ldRunsOnType != PSI_LD_RUNS_ON_UNKNOWN);
    EPS_ASSERT(pEpsOpen->ldRunsOnType < PSI_LD_RUNS_ON_MAX);

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - ldRunsOnType=%d (1-Light, 2-Advanced, 3-Basic)", pEpsOpen->ldRunsOnType);

    #if (defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)  || (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    if (pEpsOpen->ldRunsOnType != PSI_LD_RUNS_ON_LIGHT)
    {
        EPS_FATAL("eps_open - In HSM is only light (ldRunsOnType = PSI_LD_RUNS_ON_LIGHT) allowed!!!");
    }
    #endif

    g_pEpsData->ldRunsOnType    = pEpsOpen->ldRunsOnType;
    g_pEpsData->bDebug          = pEpsOpen->fw_debug_activated;
    g_pEpsData->bShmDebug       = pEpsOpen->shm_debug_activated;
    g_pEpsData->bShmHalt	      = pEpsOpen->shm_halt_activated;
    
    EPS_ASSERT((pEpsOpen->hd_count > 0 ) && ( pEpsOpen->hd_count <= PSI_CFG_MAX_HD_CNT ));
    g_pEpsData->hdCount = (LSA_UINT16)pEpsOpen->hd_count;

    for ( idx = 0; idx < g_pEpsData->hdCount; idx++ )
	{
        #if (defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)  || (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
        if (pEpsOpen->hd_args[idx].runs_on_level_basic)
        {
            EPS_FATAL("eps_open - In HSM runs_on_level_basic == 1 is NOT allowed!!!");
        }
        #endif

        g_pEpsData->hdInput[idx] = pEpsOpen->hd_args[idx]; // save HD input
	}

    g_pEpsData->imInput = pEpsOpen->imInput;
    #endif

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open called with TimeOutSec value = %d (0=Infinite)", TimeOutSec);

    //Install drivers -> Call to EPS APP IF (e.g. PCIOX, PNDriver, TestApp)
    EPS_APP_INSTALL_DRV_OPEN(pEpsOpen->ldRunsOnType);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - EPS_APP_INSTALL_DRV_OPEN done");

    //Get all Devices
    eps_shm_update_device_list();

  
	eps_isr_init(g_pEpsShmHw);              /* Init Interrupt Handler */
	eps_cp_mem_init();                      /* Init Communication (HIF-/NRT-/PI-) Memory Management */

	/* Init PSI */
	psi_init(); 
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - psi_init done");

    eps_tasks_start_psi_threads();

	retVal = eps_alloc_event(&eps_startup_proceed_event);
	EPS_ASSERT(retVal == LSA_RET_OK);

   // *** 1. OPEN HIF LD LOWER -> Enable HIF receive ****************************************************
    #ifdef EPS_HD_ONLY
	eps_hif_hd_lower_open(g_pEpsData, &eps_startup_proceed_event);      /* Init HIF HD Lower Device */
    #else
	eps_hif_ld_lower_open(g_pEpsData, &eps_startup_proceed_event);      /* Init HIF LD Lower Device */
    #endif
	retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
	eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - HIF HD/LD Lower open done");
    // *************************************************************************************************

    // *** 2. OPEN HIF LD UPPER ************************************************************************
    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
    eps_hif_ld_upper_open(g_pEpsData, &eps_startup_proceed_event);
    retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
	eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - HIF LD Upper open done");
    #endif
    // **************************************************************************************************
 
    // *** 3. OPEN LD UPPER *****************************************************************************
    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
    eps_ld_upper_open(pEpsOpen, &eps_startup_proceed_event);
    retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
    eps_memcpy(pEpsOpen, &g_pEpsData->uEpsOpen, sizeof(EPS_OPEN_TYPE));
    eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - LD Upper open done");
    #endif

	retVal = eps_free_event(eps_startup_proceed_event);
	EPS_ASSERT(retVal == LSA_RET_OK);

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86)
    if (pCbf) //If pEpsLdOpen == LSA_NULL then do nothing
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open - calling application");
        //Asynchron
        ((EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(pCbf))(&g_pEpsData->uEpsOpen,  g_pEpsData->uEpsOpen.pUsrHandle);
    }
    #else
    LSA_UNUSED_ARG(pCbf);
    #endif
    
}


LSA_VOID eps_undo_init(LSA_VOID)
{
	EPS_ASSERT(g_pEpsData != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_undo_init - stopping system");
	g_pEpsData->bShutdown = LSA_TRUE;

	EPS_ASSERT(g_pEpsShmHw != LSA_NULL);
	g_pEpsShmHw->AppShutdown(g_pEpsShmHw);
	
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_undo_init - trace system is now inactive for this instance");
    #if ( PSI_CFG_USE_PNTRC == 1 )
	pntrc_startstop( 0 , LSA_NULL);
    #endif

	eps_tasks_undo_init();        /* check if all threads are down?             */
	eps_cp_mem_undo_init();         /* close cp memory pools                      */

    #if ( PSI_CFG_USE_PNTRC == 1 )
	pntrc_undo_init();
    #endif

	/* Close drivers => needs to be done now as they might use timers/events/locks/allocs */  
    eps_shm_if_close(g_pEpsShmHw);
	eps_shm_if_undo_init();
	eps_pndev_if_undo_init();
	eps_hif_drv_if_undo_init();
    
    eps_events_undo_init();         /* No more events   */	
	eps_hw_undo_init();            /* No more allocs   */
	eps_timer_undo_init_tgroup0();          /* No more timers Thread Group 0  */
	eps_timer_undo_init_tgroup1();          /* No more timers Thread Group 1  */
	eps_mem_undo_init();            /* No more allocs   */

	EPS_APP_UNDO_INIT(g_pEpsData->hSysUpper, &g_pEpsData->HwInfo);

	eps_locks_undo_init();          /* No more locks    */

	g_pEpsData->bRunning = LSA_FALSE;

	g_pEpsData = LSA_NULL;
}

#if (EPS_PLF == EPS_PLF_ADONIS_X86)
/**
 * creates a new thread in adonis
 * uses directly the POSIX API, no EPS API available !
 * 
 * @param lStackSizeIn
 * @param lPriorityIn
 * @param sThreadNameIn
 * @param sThreadNumberArgument
 * @param pThreadFunction
 * @param lStackAdrIn
 * @return
 */
static pthread_t eps_advanced_reboot_create_thread  ( LSA_UINT32        lStackSizeIn,
                                                      LSA_UINT32        lPriorityIn,
                                                      const _TCHAR*     sThreadNameIn,
                                                      _TCHAR            sThreadNumberArgument,
                                                      void*             pThreadFunction,
                                                      LSA_UINT32*       lStackAdrIn)
{
// thread var
pthread_attr_t      uThAttributes;
struct sched_param  uThParams;
pthread_t           lThId         = 0;
pthread_t           lResult       = 0;
UINT32              lThResult     = 0;
_TCHAR              sThArgument   = { 0 };

    // set attributes to default
    lThResult = EPS_POSIX_PTHREAD_ATTR_INIT(&uThAttributes);
    
    if  (0 != lThResult)
    // default is set
    {
        // error
    }
    else
    {
        // enable setting individual params
        EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &uThAttributes, PTHREAD_EXPLICIT_SCHED);
        
        // stack
        lThResult = EPS_POSIX_PTHREAD_ATTR_SETSTACK (&uThAttributes, (void*) lStackAdrIn, lStackSizeIn*4); // 32k
    
        if  (0 != lThResult)
        // default is set
        {
            // error
        }
        else
        {
            // set prio
            uThParams.sched_priority = lPriorityIn;
            
            // set attributes and params
            lThResult = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &uThAttributes, &uThParams);
            
            if  (0 != lThResult)
            // default is set
            {
            // error
            }
            else
            {
                // set scheduling policy to RoundRobin
                lThResult = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(&uThAttributes, SCHED_RR);
                
                if  (0 != lThResult)
                // default is set
                {
                    // error
                }
                else
                {
                    // set thread name
                    lThResult = pthread_attr_setname( &uThAttributes, sThreadNameIn);
                    
                    if  (0 != lThResult)
                    // default is set
                    {
                        // error
                    }
                    else
                    {
                        // set detach state
                        lThResult = pthread_attr_setdetachstate(&uThAttributes, PTHREAD_INHERIT_SCHED);
                        
                        if  (0 != lThResult)
                        // default is set
                        {
                            // error
                        }
                        else
                        {
                            // set start argument
                            sThArgument = sThreadNumberArgument;
                            
                            // create thread
                            lThResult = EPS_POSIX_PTHREAD_CREATE( &lThId, &uThAttributes, pThreadFunction, &sThArgument);
                            
                            if  (0 != lThResult)
                            // default is set
                            {
                                // error
                                #ifdef EPS_IMCEADRV_PRINTF
                                printf("\nERROR - Thread not created: %s\n", sThreadNameIn);
                                #endif
                            }
                            else
                            {
                                #ifdef EPS_IMCEADRV_PRINTF
                                printf("\nThread created: %s", sThreadNameIn);
                                #endif
                                
                                lResult = lThId;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return lResult;
}

/**
 * starts the eps
 */
static LSA_VOID eps_advanced_reboot_startup( LSA_VOID )
{
    EPS_OPEN_TYPE uOpen;
    
    EPS_ASSERT(LSA_TRUE == bGlobIsEPSReboot);

    PSI_MEMSET(&uOpen, 0, sizeof(EPS_OPEN_TYPE));

    uOpen.hd_count      = 1;
    uOpen.pUsrHandle    = LSA_NULL;
    uOpen.ldRunsOnType  = PSI_LD_RUNS_ON_UNKNOWN;

    /* EPS entry point */
    eps_init(LSA_NULL ); /* hSys and ldRunsOnType are not used in Adonis Targets*/
    eps_open(&uOpen, LSA_NULL, PSI_EVENT_NO_TIMEOUT); /*pCbf are not used in Adonis Targets*/
    
    // reset the reboot flag    
    bGlobIsEPSReboot = LSA_FALSE;
}

/**
 * second part of the shutdown - is called by eps_advanced_reboot_shutdown()
 * 
 * does the eps_undo_init()
 * afterwards create a new thread for startup
 */
static LSA_VOID eps_advanced_reboot_shutdown_cbf( LSA_VOID )
{
    EPS_OPEN_TYPE uOpen;

    #ifdef EPS_IMCEADRV_PRINTF
    printf("\neps_advanced_reboot_cbf() - eps_close() done");
    #endif
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_advanced_reboot_cbf() - eps_close() done");

    eps_undo_init();
    #ifdef EPS_IMCEADRV_PRINTF
    printf("\neps_advanced_reboot_cbf() - eps_undo_init() done");
    #endif
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_advanced_reboot_cbf() - eps_undo_init() done");
    
    // starting the main thread again to start the EPS
    lThIdAdvRebootMain = eps_advanced_reboot_create_thread(     EPS_ADVANCED_REBOOT_STACK_SIZE,
                                                                RT_P1,
                                                                "EPS_REBOOT_UP",
                                                                '0',
                                                                eps_advanced_reboot_startup,
                                                                lThIdAdvRebootMainStackAdr);
    
    #ifdef EPS_IMCEADRV_PRINTF
    printf("\n\n########################## EPS ADN REBOOT - DONE ##########################");
    printf("\n###########################################################################\n");
    #endif
}

/**
 * first part of the eps shutdown - eps_close()
 */
static LSA_VOID eps_advanced_reboot_shutdown( LSA_VOID )
{
    EPS_CLOSE_TYPE uClose;
    eps_close( &uClose, (EPS_UPPER_CALLBACK_FCT_PTR_TYPE)eps_advanced_reboot_shutdown_cbf, PSI_EVENT_NO_TIMEOUT);
}

/**
 * initialize the reboot of Adonis FW
 */
LSA_VOID eps_advanced_init_reboot ( LSA_VOID )
{
    // set the reboot flag
    EPS_ASSERT(LSA_FALSE == bGlobIsEPSReboot);
    bGlobIsEPSReboot = LSA_TRUE;
    
    // memset the stacks
    eps_memset(lThIdAdvRebootShutdownStackAdr,  0, EPS_ADVANCED_REBOOT_STACK_SIZE);
    eps_memset(lThIdAdvRebootMainStackAdr,      0, EPS_ADVANCED_REBOOT_STACK_SIZE);
    
    #ifdef EPS_IMCEADRV_PRINTF
    printf("\n###########################################################################");
    printf("\n############################# EPS ADN REBOOT ##############################\n");
    #endif
    
    // starting the thread to shutdown the EPS
    lThIdAdvRebootShutdown = eps_advanced_reboot_create_thread( EPS_ADVANCED_REBOOT_STACK_SIZE,
                                                                RT_P1,
                                                                "EPS_REBOOT_DOWN",
                                                                '0',
                                                                eps_advanced_reboot_shutdown,
                                                                lThIdAdvRebootShutdownStackAdr);
}

/**
 * returns the global variable bGlobIsEPSReboot to check if reboot is active
 * 
 * @return bGlobIsEPSReboot
 */
LSA_UINT32 eps_advanced_reboot_is_active( LSA_VOID )
{
    return bGlobIsEPSReboot;
}
#endif

#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86))
/*---------------------------------------------------------------------------*/
/* Methodes from EPS user core to PN Stack messaging                         */
/*---------------------------------------------------------------------------*/

LSA_VOID eps_user_system_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct )
{
    /* add the user instance and CBF in extended RQB Header */
	/* note: information is used for addressing response receiver */
	EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE*)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
	EPS_ASSERT( p_inst  != 0 );
	EPS_ASSERT( p_fct   != 0 );

	EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

#if (PSI_CFG_USE_HIF == 1)
	// Post message to HIF LD
	psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system, rqb_ptr );
#else
	// Post message to PSI LD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_system, rqb_ptr );
#endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_open_channel_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct  )
{
	/* add the user instance and CBF in extended RQB Header */
	/* note: information is used for addressing response receiver */
	EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;	
    
    EPS_ASSERT( rqb_ptr != 0 );
	EPS_ASSERT( p_inst  != 0 );
	EPS_ASSERT( p_fct   != 0 );

    g_pEpsData->iOpenUserChannels++;

	EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

#if (PSI_CFG_USE_HIF == 1)
	// Post message to HIF LD
	psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_open_channel, rqb_ptr );
#else
	// Post message to PSI LD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_open_channel, rqb_ptr );
#endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_close_channel_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct )
{
	/* add the user instance and CBF in extended RQB Header */
	/* note: information is used for addressing response receiver */
	EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;
    
    EPS_ASSERT( rqb_ptr != 0 );
	EPS_ASSERT( p_inst  != 0 );
	EPS_ASSERT( p_fct   != 0 );

    EPS_ASSERT(g_pEpsData->iOpenUserChannels != 0);
    g_pEpsData->iOpenUserChannels--;    

	EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

#if (PSI_CFG_USE_HIF == 1)
	// Post message to HIF LD
	psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_close_channel, rqb_ptr );
#else
	// Post message to PSI LD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_close_channel, rqb_ptr );
#endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr,  LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct )
{
	/* add the user instance and CBF in extended RQB Header */
	/* note: information is used for addressing response receiver */
	EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
	EPS_ASSERT( p_inst  != 0 );
	EPS_ASSERT( p_fct   != 0 );

	EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
	EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_user_request_lower pRQB(0x%08x) p_inst(0x%08x) p_fct(0x%08x)", rqb_ptr, p_inst, p_fct);

#if (PSI_CFG_USE_HIF == 1)
	// Post message to HIF LD
	psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, rqb_ptr );
#else
	// Post message to PSI LD
	psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_request, rqb_ptr );
#endif
}
#endif

/*------------------------------------------------------------------------------
//	stdlib
//----------------------------------------------------------------------------*/
LSA_INT  eps_sprintf (LSA_UINT8 *buffer, const char *format, ...)
{
	LSA_INT rc = -1;
	va_list argptr;

	va_start (argptr, format);
	rc = vsprintf ((char*)buffer, format, argptr);
	va_end (argptr);
	return rc;
}

LSA_INT  eps_vsprintf (LSA_UINT8 *buffer, const char *format, va_list argptr)
{
	LSA_INT rc;
	rc = vsprintf ((char*)buffer, format, argptr);
	return rc;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
