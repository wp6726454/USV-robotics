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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_sys.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the system LSA interface                                      */
/*                                                                           */
/*****************************************************************************/
#define PNTRC_MODULE_ID      1
#define LTRC_ACT_MODUL_ID    1

#define LTRC_SET_SUBSYS_CFG_TABLE

#include "pntrc_inc.h"
#include "pntrc_int.h"
#include "pntrc_trc.h"

PNTRC_FILE_SYSTEM_EXTENSION(PNTRC_MODULE_ID)

LSA_UINT16 pntrc_init(PNTRC_INIT_PTR_TYPE pInit)
{
	LSA_UINT16 res = LSA_RET_OK;
	LSA_UINT32 i, j, k;
	LSA_UINT32 cntComponents = 0;
	LSA_USER_ID_TYPE userId;
	LSA_UINT8 iCPU;
	PNTRC_INIT_TRACE_TIMER_TYPE pInitTraceTimer;

	PNTRC_ASSERT(pInit != LSA_NULL);

	PNTRC_ALLOC_CRITICAL_SECTION();

	PNTRC_ENTER_SERVICE();

	PNTRC_INIT_TRACE_TIMER(&pInitTraceTimer);

	/* Init Global Data */
	g_pPntrcData = &g_PntrcData;
	g_pPntrcData->TraceMem=LSA_NULL;
	g_pPntrcData->hSysDev=pInit->hSysDev;
	g_pPntrcData->pCBBufferFull=pInit->pCBBufferFull;
	g_pPntrcData->SetLogLevelPending=LSA_FALSE;
	g_pPntrcData->SetLogLevel_pLogLevels=LSA_NULL;
	g_pPntrcData->forceBufferSwitch=LSA_FALSE;
	g_pPntrcData->pRQBLogLevels=0;
	g_pPntrcData->pntrc_SendSyncScaler=0;

	for (iCPU=0; iCPU<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; iCPU++) {
		g_pPntrcData->pntrc_lower_cpu_list[iCPU].Opened=LSA_FALSE;
		g_pPntrcData->pntrc_lower_cpu_list[iCPU].index=iCPU;
	}


	/* Init Binary Trace Buffers */
	pntrc_tbb_init(pInit);

	/* Switch Trace functions to binary trace */
	if(pInit->bUseLocalBinaryTrace == LSA_TRUE)
	{
		pntrc_current_fct.entry00=pntrc_tbb_memory_00;
		pntrc_current_fct.entry01=pntrc_tbb_memory_01;
		pntrc_current_fct.entry02=pntrc_tbb_memory_02;
		pntrc_current_fct.entry03=pntrc_tbb_memory_03;
		pntrc_current_fct.entry04=pntrc_tbb_memory_04;
		pntrc_current_fct.entry05=pntrc_tbb_memory_05;
		pntrc_current_fct.entry06=pntrc_tbb_memory_06;
		pntrc_current_fct.entry07=pntrc_tbb_memory_07;
		pntrc_current_fct.entry08=pntrc_tbb_memory_08;
		pntrc_current_fct.entry09=pntrc_tbb_memory_09;
		pntrc_current_fct.entry10=pntrc_tbb_memory_10;
		pntrc_current_fct.entry11=pntrc_tbb_memory_11;
		pntrc_current_fct.entry12=pntrc_tbb_memory_12;
		pntrc_current_fct.entry13=pntrc_tbb_memory_13;
		pntrc_current_fct.entry14=pntrc_tbb_memory_14;
		pntrc_current_fct.entry15=pntrc_tbb_memory_15;
		pntrc_current_fct.entry16=pntrc_tbb_memory_16;
		pntrc_current_fct.entrybytearray=pntrc_tbb_memory_bytearray;
		pntrc_current_fct.entrystring=pntrc_tbb_memory_string;
		pntrc_current_fct.entrysynctime=pntrc_tbb_memory_synctime;
	}

		/* Init Log Levels */
	if(pInit->bUseInitLogLevels == LSA_TRUE)
	{
		PNTRC_ASSERT(pInit->pInitLogLevels != LSA_NULL);
		userId.uvar8=0;
		pntrc_tbb_set_log_levels(pInit->pInitLogLevels,userId,0);
	}
	else
	{
		cntComponents = sizeof(ltrc_subsys_cfg_table)/sizeof(LTRC_ADVTEXT_INFO_TYPE);
		k=1;
		for(i=1; i<(cntComponents-1); i++)
		{
			for(j=0; j<ltrc_subsys_cfg_table[i].instance; j++)
			{
				pntrc_current_level[k] = ltrc_subsys_cfg_table[i].level;
				k++;
			}
		}
		if (k != TRACE_SUBSYS_NUM) {
			PNTRC_SYSTEM_TRACE_00(0, PNTRC_TRACE_LEVEL_FATAL, "ltrc_sub.h is inconsistent");
			PNTRC_FATAL(0);
		}
	}

	/*
	for(i=0; i<TRACE_SUBSYS_NUM; i++)
	{
		pntrc_current_level[i] = LSA_TRACE_LEVEL_CHAT;
	}
	*/

	PNTRC_EXIT_SERVICE();

    return res;
}

PNTRC_CPU_HANDLE_PTR_TYPE pntrc_get_free_lower_cpu_slot(LSA_VOID)
{
	LSA_UINT8 i;
	for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (!g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
			return &(g_pPntrcData->pntrc_lower_cpu_list[i]);
		}
	}
	return LSA_NULL;
}


PNTRC_CPU_HANDLE_PTR_TYPE pntrc_open_lower_cpu(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE SysHandle)
{
	PNTRC_CPU_HANDLE_PTR_TYPE pCPUHandle;
	PNTRC_ASSERT(SysHandle != LSA_NULL);
	PNTRC_ENTER_SERVICE();
	pCPUHandle=pntrc_get_free_lower_cpu_slot();
	if (pCPUHandle == LSA_NULL) {
		PNTRC_FATAL(0);
	}
	pCPUHandle->SetLogLevelPending=LSA_FALSE;
	pCPUHandle->SyncTimeSent=LSA_FALSE;
	if (g_pPntrcData->UseInitLogLevels) {
		pCPUHandle->InitLogLevelsSent=LSA_FALSE;
	} else {
		pCPUHandle->InitLogLevelsSent=LSA_TRUE;
	}
	pCPUHandle->FatalHandled=LSA_FALSE;
	pCPUHandle->SysHandle=SysHandle;
	pCPUHandle->Opened=LSA_TRUE;
	PNTRC_EXIT_SERVICE();
	return pCPUHandle;
}

LSA_VOID pntrc_close_lower_cpu(PNTRC_CPU_HANDLE_PTR_TYPE PntrcHandle)
{
	LSA_UINT16 rc;
	int j,k;

	PNTRC_ASSERT(PntrcHandle != LSA_NULL);
	PNTRC_ENTER_SERVICE();
	if (PntrcHandle->Opened == LSA_TRUE) {
		pntrc_tbb_save_all_lower_buffers(PntrcHandle->SysHandle,PntrcHandle->index,LSA_NULL,LSA_FALSE);  //save open trace buffers
		for (j=0; j<PNTRC_MAX_CATEGORY_COUNT; j++) {
			for (k=0; k<2; k++) {
				if (g_pPntrcData->pTraceBuffer[PntrcHandle->index][j][k] != 0) {
					PNTRC_FREE_LOCAL_MEM(&rc, g_pPntrcData->pTraceBuffer[PntrcHandle->index][j][k]);
					PNTRC_ASSERT(rc==LSA_RET_OK);
					g_pPntrcData->pTraceBuffer[PntrcHandle->index][j][k] = 0;
				}
			}
		}
		PntrcHandle->Opened=LSA_FALSE;
	}
	PNTRC_EXIT_SERVICE();
}

LSA_UINT16 pntrc_service(void)
{
	PNTRC_ENTER_SERVICE();
	pntrc_tbb_service_lower_cpus();
	if (!g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU) {
		pntrc_service_MIF();
		pntrc_service_slave_MIF();
	}
	if (g_pPntrcData->forceBufferSwitch) {
		g_pPntrcData->forceBufferSwitch=LSA_FALSE;
		pntrc_ForceBufferSwitch();
	}
	PNTRC_EXIT_SERVICE();
	return 0;
}

LSA_UINT16 pntrc_fatal_occurred(void)
{
	if (g_pPntrcData != LSA_NULL) {
		if (g_pPntrcData->TraceMem != LSA_NULL) {
			if (g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU) {
				pntrc_tbb_save_all_buffers(LSA_TRUE);
			} else {
				g_pPntrcData->TraceMem->MasterMIF.FatalOccurred=1;
			}
		}
	}
	return 0;
}

LSA_VOID pntrc_set_log_levels_CB(LSA_VOID* rqb0)
{
	LSA_UINT16 rc = LSA_RET_OK;
	PNTRC_RQB_PTR_TYPE rqb;
	PNTRC_ASSERT(rqb0 != 0);
	rqb=(PNTRC_RQB_PTR_TYPE)rqb0;
	PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_OK);
	if (g_pPntrcData->pRQBLogLevels != 0) {
		PNTRC_FREE_LOCAL_MEM(&rc,g_pPntrcData->pRQBLogLevels);
		g_pPntrcData->pRQBLogLevels=0;
	}
	if (rqb->Cbf != 0) {
		(*rqb->Cbf)(rqb);
	}

}

LSA_VOID pntrc_request(PNTRC_RQB_PTR_TYPE rqb)
{
	int i,j;
	switch(PNTRC_RQB_GET_OPCODE(rqb)) {
		case PNTRC_OPC_SET_TRACE_LEVELS:
			if (g_pPntrcData->RQBSetTraceLevels  != LSA_NULL) {
				PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_ERR_BUSY);
				if (rqb->Cbf != 0) {
					(*rqb->Cbf)(rqb);
				}
			} else {
				g_pPntrcData->RQBSetTraceLevels=rqb;
				pntrc_tbb_set_log_levels(rqb->args.set_trace_levels.pLogLevels,PNTRC_RQB_GET_USER_ID(rqb),pntrc_set_log_levels_CB);
			}
		break;
		case PNTRC_OPC_SET_TRACE_LEVELS_COMP:
			if (g_pPntrcData->RQBSetTraceLevels  != LSA_NULL) {
				PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_ERR_BUSY);
				if (rqb->Cbf != 0) {
					(*rqb->Cbf)(rqb);
				}
			} else {
				g_pPntrcData->RQBSetTraceLevels=rqb;
				PNTRC_ALLOC_LOCAL_MEM( (void**)&g_pPntrcData->pRQBLogLevels, TRACE_SUBSYS_NUM );
				PNTRC_ASSERT(g_pPntrcData->pRQBLogLevels != 0);
				for(i=0; i<TRACE_SUBSYS_NUM; i++)
				{
					g_pPntrcData->pRQBLogLevels[i]=pntrc_current_level[i];
				}
				i=0;
				while (ltrc_subsys_cfg_table[i].instance != 0xffff) {
					if (ltrc_subsys_cfg_table[i].comp == rqb->args.set_trace_levels_comp.comp) {
						for(j=0; j<ltrc_subsys_cfg_table[i].instance; j++) {
							g_pPntrcData->pRQBLogLevels[ltrc_subsys_cfg_table[i].subsysId + j]=rqb->args.set_trace_levels_comp.level;
						}
					}
					i=i+1;
				}
				pntrc_tbb_set_log_levels(g_pPntrcData->pRQBLogLevels,PNTRC_RQB_GET_USER_ID(rqb),pntrc_set_log_levels_CB);
			}
		break;
		case PNTRC_OPC_SET_TRACE_LEVELS_SUBSYS:
			if (g_pPntrcData->RQBSetTraceLevels  != LSA_NULL) {
				PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_ERR_BUSY);
				if (rqb->Cbf != 0) {
					(*rqb->Cbf)(rqb);
				}
			} else {
				g_pPntrcData->RQBSetTraceLevels=rqb;
				PNTRC_ALLOC_LOCAL_MEM( (void**)&g_pPntrcData->pRQBLogLevels, TRACE_SUBSYS_NUM );
				PNTRC_ASSERT(g_pPntrcData->pRQBLogLevels != 0);
				for(i=0; i<TRACE_SUBSYS_NUM; i++)
				{
					g_pPntrcData->pRQBLogLevels[i]=pntrc_current_level[i];
				}
				for(i=0; i<ltrc_subsys_cfg_table[rqb->args.set_trace_levels_subsys.subsys].instance; i++) {
					g_pPntrcData->pRQBLogLevels[rqb->args.set_trace_levels_subsys.subsys+i]=rqb->args.set_trace_levels_subsys.level;
				}
				pntrc_tbb_set_log_levels(g_pPntrcData->pRQBLogLevels,PNTRC_RQB_GET_USER_ID(rqb),pntrc_set_log_levels_CB);
			}
		break;
		case PNTRC_OPC_SET_TRACE_LEVELS_IF:
			if (g_pPntrcData->RQBSetTraceLevels  != LSA_NULL) {
				PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_ERR_BUSY);
				if (rqb->Cbf != 0) {
					(*rqb->Cbf)(rqb);
				}
			} else {
				g_pPntrcData->RQBSetTraceLevels=rqb;
				PNTRC_ALLOC_LOCAL_MEM( (void**)&g_pPntrcData->pRQBLogLevels, TRACE_SUBSYS_NUM );
				PNTRC_ASSERT(g_pPntrcData->pRQBLogLevels != 0);
				for(i=0; i<TRACE_SUBSYS_NUM; i++)
				{
					g_pPntrcData->pRQBLogLevels[i]=pntrc_current_level[i];
				}
				g_pPntrcData->pRQBLogLevels[rqb->args.set_trace_levels_if.intf]=rqb->args.set_trace_levels_if.level;
				pntrc_tbb_set_log_levels(g_pPntrcData->pRQBLogLevels,PNTRC_RQB_GET_USER_ID(rqb),pntrc_set_log_levels_CB);
			}
		break;
		case PNTRC_OPC_SWITCH_BUFFERS:
			pntrc_force_buffer_switch();
			PNTRC_RQB_SET_RESPONSE (rqb, PNTRC_RSP_OK);
			if (rqb->Cbf != 0) {
				(*rqb->Cbf)(rqb);
			}
		break;
		case PNTRC_OPC_GET_TRACE_LEVELS:
			PNTRC_ASSERT(rqb->args.get_trace_levels.pLogLevels != 0);
			pntrc_tbb_get_log_levels(rqb->args.get_trace_levels.pLogLevels);
		break;
	}
}


LSA_VOID pntrc_set_ready(LSA_VOID)
{
	pntrc_tbb_set_ready();
}

LSA_VOID pntrc_buffer_full_done(LSA_VOID* hSysDev, PNTRC_BUFFER_ID_TYPE BufferID)
{
	PNTRC_ASSERT(hSysDev != LSA_NULL);
	pntrc_tbb_buffer_full_done(hSysDev,BufferID);
}
LSA_VOID pntrc_force_buffer_switch(LSA_VOID)
{
	g_pPntrcData->forceBufferSwitch=LSA_TRUE;
}

LSA_RESPONSE_TYPE pntrc_set_log_levels_from_components(LSA_UINT8* pLogLevels, LSA_UINT8* pCompLevels, LSA_UINT16 CompCount)
{
	LSA_UINT16 i,k,l;
	//map component trace levels to all subsys tracelevels
	for(i=0; i<CompCount; i++) {
		for(k=0; ltrc_subsys_cfg_table[k].instance != 0xffff; k++) {
			if(ltrc_subsys_cfg_table[k].comp == (LTRC_COMP_TYPE)i) {
				for(l=0; l<ltrc_subsys_cfg_table[k].instance; l++) {
					pLogLevels[ltrc_subsys_cfg_table[k].subsysId + l] = pCompLevels[i];
				}
			}
		}
	}
	return LSA_RET_OK;
}

LSA_BOOL pntrc_all_cpus_closed(LSA_VOID)
{
	LSA_UINT8 i;
	for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
			return LSA_FALSE;
		}
	}
	return LSA_TRUE;
}

/*------------------------------------------------------------------------------
//	uninitialise the module PNTRC
//----------------------------------------------------------------------------*/
LSA_UINT16
pntrc_undo_init (
    LSA_VOID
	)
{
	LSA_UINT16 rc = LSA_RET_OK;

	PNTRC_ENTER_SERVICE();

	if (g_pPntrcData->RQBSetTraceLevels  != LSA_NULL) {
		rc = PNTRC_RSP_ERR_REQUEST_PENDING;  /* request is still pending */
	} else if (!pntrc_all_cpus_closed()) {
		rc = PNTRC_RSP_ERR_CPUS_STILL_OPEN;
	} else {
		/* Global structure valid? */
		if(is_not_null(g_pPntrcData))
		{
			pntrc_tbb_uninit();

			/* Switch Trace functions to out functions */
			pntrc_current_fct.entry00=PNTRC_TRACE_OUT_00;
			pntrc_current_fct.entry01=PNTRC_TRACE_OUT_01;
			pntrc_current_fct.entry02=PNTRC_TRACE_OUT_02;
			pntrc_current_fct.entry03=PNTRC_TRACE_OUT_03;
			pntrc_current_fct.entry04=PNTRC_TRACE_OUT_04;
			pntrc_current_fct.entry05=PNTRC_TRACE_OUT_05;
			pntrc_current_fct.entry06=PNTRC_TRACE_OUT_06;
			pntrc_current_fct.entry07=PNTRC_TRACE_OUT_07;
			pntrc_current_fct.entry08=PNTRC_TRACE_OUT_08;
			pntrc_current_fct.entry09=PNTRC_TRACE_OUT_09;
			pntrc_current_fct.entry10=PNTRC_TRACE_OUT_10;
			pntrc_current_fct.entry11=PNTRC_TRACE_OUT_11;
			pntrc_current_fct.entry12=PNTRC_TRACE_OUT_12;
			pntrc_current_fct.entry13=PNTRC_TRACE_OUT_13;
			pntrc_current_fct.entry14=PNTRC_TRACE_OUT_14;
			pntrc_current_fct.entry15=PNTRC_TRACE_OUT_15;
			pntrc_current_fct.entry16=PNTRC_TRACE_OUT_16;
			pntrc_current_fct.entrybytearray=PNTRC_TRACE_OUT_BYTE_ARRAY;
			pntrc_current_fct.entrystring=PNTRC_TRACE_OUT_STRING;
			pntrc_current_fct.entrysynctime=PNTRC_TRACE_OUT_SYNC_TIME;

		}
		else
		{
			rc = LSA_RET_ERR_SEQUENCE;
		}
	}

	PNTRC_EXIT_SERVICE();

	PNTRC_FREE_CRITICAL_SECTION();

	return rc;
}

/*------------------------------------------------------------------------------
//	fills the global fatal-error structure and call PNTRC_FATAL_ERROR
//----------------------------------------------------------------------------*/
LSA_FATAL_ERROR_TYPE  pntrc_glob_fatal_error;

LSA_VOID
pntrc_fatal_error (
	LSA_UINT16  module_id,
	LSA_UINT16  line,
	LSA_UINT32	error_code_0,
	LSA_UINT32	error_code_1,
	LSA_UINT32	error_code_2,
	LSA_UINT32	error_code_3,
	LSA_UINT16  error_data_len,
	LSA_VOID_PTR_TYPE  error_data
	)
{
	pntrc_glob_fatal_error.lsa_component_id	= LSA_COMP_ID_LTRC;
	pntrc_glob_fatal_error.module_id			= module_id;
	pntrc_glob_fatal_error.line				= line;

	pntrc_glob_fatal_error.error_code[0]		= error_code_0;
	pntrc_glob_fatal_error.error_code[1]		= error_code_1;
	pntrc_glob_fatal_error.error_code[2]		= error_code_2;
	pntrc_glob_fatal_error.error_code[3]		= error_code_3;

	pntrc_glob_fatal_error.error_data_length	= error_data_len;
	pntrc_glob_fatal_error.error_data_ptr		= error_data;

	PNTRC_FATAL_ERROR ((LSA_UINT16)(sizeof(pntrc_glob_fatal_error)), &pntrc_glob_fatal_error);
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
