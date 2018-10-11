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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_sys.c                                 :F&  */
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
#define HIF_MODULE_ID		2
#define LTRC_ACT_MODUL_ID	2

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 * \brief initialise the module HIF
 * 
 * must call once on startup
 *  
 * @return
 */
LSA_UINT16 hif_init (LSA_VOID)
{
	LSA_UINT16 res = LSA_RET_OK;
	LSA_UINT16 i,e,retVal;
	
	HIF_LD_ENTER();
	HIF_HD_ENTER();
	
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_init->");    

    /* check if HIF uses the same size for a BOOL like defined LSA_BOOL (in lsa_cfg.h) */
    if (sizeof(LSA_BOOL) != HIF_LSA_BOOL_SIZE)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_init(): LSA_BOOL-size is not equal HIF_LSA_BOOL_SIZE declared in HIF");
		HIF_FATAL(0);
    }

	/* init internal structures */
	HIF_ALLOC_LOCAL_MEM((void**) &g_pHifData, sizeof(*g_pHifData));

	if(HIF_IS_NULL(g_pHifData))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_init(-) can not alloc g_pHifData");
		res = LSA_RET_ERR_RESOURCE;
	}
	else
	{
		HIF_MEMSET(g_pHifData, 0, sizeof(*g_pHifData));
		
		/* Apply init values */   
		for(i=0; i<HIF_MAX_INSTANCES; i++)
		{
			g_pHifData->Instances[i].hH    = (HIF_HANDLE)i;
			g_pHifData->Instances[i].State = HIF_INST_STATE_FREE;
	
			#if ( HIF_CFG_MAX_LD_INSTANCES > 0 )
			if(i<HIF_CFG_MAX_LD_INSTANCES) /* LD Device */
			{
				g_pHifData->Instances[i].bLDDevice = LSA_TRUE;
				g_pHifData->Instances[i].bHDDevice = LSA_FALSE;
				
				g_pHifData->Instances[i].func.DO_INTERRUPT_REQUEST = HIF_LD_DO_INTERRUPT_REQUEST;
				g_pHifData->Instances[i].func.DO_REQUEST_LOCAL = HIF_LD_DO_REQUEST_LOCAL;
				g_pHifData->Instances[i].func.SEND_IRQ = HIF_LD_SEND_IRQ;
				g_pHifData->Instances[i].func.ACK_IRQ  = HIF_LD_ACK_IRQ;
				g_pHifData->Instances[i].func.REQUEST_LOWER = HIF_LD_REQUEST_LOWER;
				g_pHifData->Instances[i].func.CLOSE_CHANNEL_LOWER = HIF_LD_CLOSE_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.OPEN_CHANNEL_LOWER = HIF_LD_OPEN_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.CLOSE_DEVICE_LOWER = HIF_LD_CLOSE_DEVICE_LOWER;
			}
			#endif
			#if ( HIF_CFG_MAX_LD_INSTANCES > 0 ) && ( HIF_CFG_MAX_HD_INSTANCES > 0)
			else /* HD Device */
			#endif
			#if ( HIF_CFG_MAX_HD_INSTANCES > 0)
			{
				g_pHifData->Instances[i].bLDDevice = LSA_FALSE;
				g_pHifData->Instances[i].bHDDevice = LSA_TRUE;
				
				g_pHifData->Instances[i].func.DO_INTERRUPT_REQUEST = HIF_HD_DO_INTERRUPT_REQUEST;
				g_pHifData->Instances[i].func.DO_REQUEST_LOCAL = HIF_HD_DO_REQUEST_LOCAL;
				g_pHifData->Instances[i].func.SEND_IRQ = HIF_HD_SEND_IRQ;
				g_pHifData->Instances[i].func.ACK_IRQ  = HIF_HD_ACK_IRQ;
				g_pHifData->Instances[i].func.REQUEST_LOWER = HIF_HD_REQUEST_LOWER;
				g_pHifData->Instances[i].func.CLOSE_CHANNEL_LOWER = HIF_HD_CLOSE_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.OPEN_CHANNEL_LOWER = HIF_HD_OPEN_CHANNEL_LOWER;
				g_pHifData->Instances[i].func.CLOSE_DEVICE_LOWER = HIF_HD_CLOSE_DEVICE_LOWER;
			}
			#endif
	
			hif_setup_single_pipe_params_by_index(&g_pHifData->Instances[i], HIF_GLOB_ADMIN, LSA_COMP_ID_HIF, 0, 0, HIF_GLOB_ADMIN);

			for(e=0; e<HIF_MAX_PIPES_INST; e++)
			{
				g_pHifData->Instances[i].Pipes[e].PipeIndex       = e;
				g_pHifData->Instances[i].Pipes[e].State           = HIF_PIPE_STATE_CLOSED;
				g_pHifData->Instances[i].Pipes[e].pInstance       = &(g_pHifData->Instances[i]);				
			}

		}
		
		for(i=0; i<HIF_MAX_CH_HANDLES; i++)
		{
			g_pHifData->ChHandles[i].bUsed      = LSA_FALSE;
			g_pHifData->ChHandles[i].bValid     = LSA_FALSE;
			g_pHifData->ChHandles[i].hHifCompCh = (LSA_HANDLE_TYPE)i;
		}
		
		for(i=0; i<HIF_MAX_COMP_ID; i++)
		{
			g_pHifData->CompStore.CompStd[i].bUsed = LSA_FALSE;
			g_pHifData->CompStore.CompSys[i].bUsed = LSA_FALSE;
		}

		/* Reserve channel handle used by HIF internally */
		g_pHifData->ChHandles[HIF_INTERNAL_REQ_RESERVED_CH_HANDLE].bUsed = LSA_TRUE;
	}
	
    #if !defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
	if(res == LSA_RET_OK)
	{
		/* alloc ld poll timer */
		if(HIF_CFG_MAX_LD_INSTANCES > 0)
		{
            HIF_ALLOC_TIMER(&res, &(g_pHifData->LD.PollTimer.hPollTimer), LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_1MS);
			
			if(res != LSA_RET_OK)
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-) can not alloc ld poll timer");        
				res = LSA_RET_ERR_RESOURCE;
			}
		}
	}
    #endif

    #if !defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
	if(res == LSA_RET_OK)
	{
		/* alloc hd poll timer */
		if(HIF_CFG_MAX_HD_INSTANCES > 0)
		{
            HIF_ALLOC_TIMER(&res, &(g_pHifData->HD.PollTimer.hPollTimer), LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_1MS);

			if(res != LSA_RET_OK)
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-) can not alloc hd poll timer");        
				res = LSA_RET_ERR_RESOURCE;
			}
		}
	}
    #endif

	if( (res == LSA_RET_OK) && (HIF_CFG_MAX_LD_INSTANCES > 0) )
	{
		/* alloc ld timeout poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->LD.PollTimer.pRQBPoll), HIF_OPC_TIMEOUT_POLL);
	}
	
	if( (res == LSA_RET_OK) && (HIF_CFG_MAX_HD_INSTANCES > 0) )
	{
		/* alloc hd timeout poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->HD.PollTimer.pRQBPoll), HIF_OPC_TIMEOUT_POLL);
	}

	if( (res == LSA_RET_OK) && (HIF_CFG_MAX_LD_INSTANCES > 0) )
	{
		/* alloc ld poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->LD.pRQBPoll), HIF_OPC_POLL);
	}
	
	if( (res == LSA_RET_OK) && (HIF_CFG_MAX_HD_INSTANCES > 0) )
	{
		/* alloc hd poll rqb */
		res = hif_alloc_local_internal_rqb(&(g_pHifData->HD.pRQBPoll), HIF_OPC_POLL);
	}
	
	if(res == LSA_RET_OK)
	{
		/* alloc isr rqb */
		for(i=0; (i<HIF_MAX_INSTANCES) && (res==LSA_RET_OK); i++)
		{
			res = hif_alloc_local_internal_rqb(&(g_pHifData->Instances[i].pRQBIsr), HIF_OPC_ISR);
			g_pHifData->Instances[i].pRQBIsr->args.internal_req.receive.hHDest = g_pHifData->Instances[i].hH;
		}
		
		/* alloc short rqb */
		for(i=0; (i<HIF_MAX_INSTANCES) && (res==LSA_RET_OK); i++)
		{
			res = hif_alloc_local_internal_rqb(&(g_pHifData->Instances[i].pRQBShort), HIF_OPC_RECEIVE);
			g_pHifData->Instances[i].pRQBShort->args.internal_req.receive.hHDest = g_pHifData->Instances[i].hH;
		}
	}

	if(res == LSA_RET_OK)
	{
		hif_install_integrated_comp_access_funcs();
	}
	
	if(res != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "hif_init(-)");
		
		/* cleanup & abort */    
		if(HIF_IS_NOT_NULL(g_pHifData))
		{
			/* dealloc ld timer poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->LD.PollTimer.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->LD.PollTimer.pRQBPoll);
			}
			/* dealloc hd timer poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->HD.PollTimer.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->HD.PollTimer.pRQBPoll);
			}
			/* dealloc ld poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->LD.pRQBPoll);
			}
			/* dealloc hd poll rqb */
			if(HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll))
			{
				hif_free_local_internal_rqb(g_pHifData->HD.pRQBPoll);
			}
			/* dealloc isr/short rqbs */      
			for(i=0; i<HIF_MAX_INSTANCES; i++)
			{
				if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBIsr))
				{
					hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBIsr);
				}
				if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBShort))
				{
					hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBShort);
				}
			}
			
			/* dealloc global hif data */
			HIF_FREE_LOCAL_MEM(&retVal, g_pHifData);
			HIF_ASSERT(retVal == LSA_RET_OK);
			
			g_pHifData = (HIF_DATA_TYPE*)LSA_NULL;
		}
	}
	else
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_init(+)");
	}

	HIF_HD_EXIT();
	HIF_LD_EXIT();
	
	return res;
}

/**
 * \brief uninitialise the module HIF
 * 
 * 
 * 
 * @return
 *        - LSA_RET_ERR_SEQUENCE
 *        - LSA_RET_OK
 */
LSA_UINT16 hif_undo_init (LSA_VOID)
{
	LSA_UINT16 i;
	LSA_UINT16 retVal;
	
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_undo_init->");
	
	if(HIF_IS_NULL(g_pHifData))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called when g_pHifData == 0!");
		return LSA_RET_ERR_SEQUENCE;
	}
	
	/* check if there is any open hif device */
	if(hif_check_for_open_instances() == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => not all hif devices closed!");
		return LSA_RET_ERR_SEQUENCE;
	}
	
	/* check for open poll users - should not be possible */
	if(g_pHifData->LD.PollTimer.PollUserCnt != 0)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => ld poll users still registered!");
		return LSA_RET_ERR_SEQUENCE;
	}
	if(g_pHifData->HD.PollTimer.PollUserCnt != 0)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_undo_init(-) called => hd poll users still registered!");
		return LSA_RET_ERR_SEQUENCE;
	} 
	
	if(HIF_CFG_MAX_LD_INSTANCES > 0)
	{
        #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
		/* free poll timer */
		HIF_FREE_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer);
		HIF_ASSERT(retVal == LSA_RET_OK);
        #endif
		/* free timeout poll rqb */
		hif_free_local_internal_rqb(g_pHifData->LD.PollTimer.pRQBPoll);
		/* free poll rqb */
		hif_free_local_internal_rqb(g_pHifData->LD.pRQBPoll);
	}
	
	if(HIF_CFG_MAX_HD_INSTANCES > 0)
	{
        #if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
		/* free poll timer */
		HIF_FREE_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer);
		HIF_ASSERT(retVal == LSA_RET_OK);
        #endif
		/* free timeout poll rqb */
		hif_free_local_internal_rqb(g_pHifData->HD.PollTimer.pRQBPoll);
		/* free poll rqb */
		hif_free_local_internal_rqb(g_pHifData->HD.pRQBPoll);
	}
	
	/* dealloc isr/short rqbs */
	for(i=0; i<HIF_MAX_INSTANCES; i++)
	{
		if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBIsr))
		{
			hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBIsr);
		}
		if(HIF_IS_NOT_NULL(g_pHifData->Instances[i].pRQBShort))
		{
			hif_free_local_internal_rqb(g_pHifData->Instances[i].pRQBShort);
		}
	}
	
	/* free internal structures */
	HIF_FREE_LOCAL_MEM(&retVal, g_pHifData);
	HIF_ASSERT(retVal == LSA_RET_OK);
	
	g_pHifData = (HIF_DATA_TYPE*)LSA_NULL;
	
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_undo_init(+)");
	
	return LSA_RET_OK;
}

/**
 * \brief register component interfaces
 * 
 * 
 * 
 * @param CompId
 * @param pCompFuncs
 * @return
 */
/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/
LSA_UINT16 hif_register_standard_component(LSA_COMP_ID_TYPE CompId, HIF_COMPONENT_SUPPORT_STD_PTR_TYPE pCompFuncs)
{
	HIF_ASSERT(g_pHifData != LSA_NULL);

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_register_standard_component(+) - CompId(0x%x), pCompFuncs(0x%x)", CompId, pCompFuncs);

	if(pCompFuncs == LSA_NULL)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%d/0x%x) pCompFuncs(0x%x)",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

	if( (pCompFuncs->GetOpenChannelCompInfo        == LSA_NULL) ||
		(pCompFuncs->GetOpenChannelLowerHandle     == LSA_NULL) ||
		(pCompFuncs->IsCloseChannelRqb             == LSA_NULL) ||
		(pCompFuncs->ReplaceOpenChannelCbf         == LSA_NULL) ||
		(pCompFuncs->ReplaceOpenChannelLowerHandle == LSA_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) a required function ptr is missing",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	if( (pCompFuncs->DeserializeLower == LSA_NULL) ||
		(pCompFuncs->DeserializeUpper == LSA_NULL) ||
		(pCompFuncs->SerializeLower   == LSA_NULL) ||
		(pCompFuncs->SerializeUpper   == LSA_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) a required function ptr for serialization is missing",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}
#endif

	if(g_pHifData->CompStore.CompStd[CompId].bUsed == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_standard_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) component already registered",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_SEQUENCE;
	}

	g_pHifData->CompStore.CompStd[CompId].Comp  = *pCompFuncs;
	g_pHifData->CompStore.CompStd[CompId].bUsed = LSA_TRUE;

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_register_standard_component(+) CompId(%d/0x%x) pCompFuncs(0x%x)",CompId,CompId, pCompFuncs);

	return LSA_RET_OK;
}

/**
 * \brief register component system interfaces
 * 
 * 
 * 
 * @param CompId
 * @param pCompFuncs
 * @return
 */
LSA_UINT16 hif_register_sys_component(LSA_COMP_ID_TYPE CompId, HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE pCompFuncs)
{
	HIF_ASSERT(g_pHifData != LSA_NULL);

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_register_sys_component(+) - CompId(0x%x), pCompFuncs(0x%x)", CompId, pCompFuncs);

	if(pCompFuncs == LSA_NULL)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%d/0x%x) pCompFuncs(0x%x)",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

	if( (pCompFuncs->GetSysRequesthH      == LSA_NULL) ||
		(pCompFuncs->GetSysRequestCbf     == LSA_NULL) ||
		(pCompFuncs->ReplaceSysRequestCbf == LSA_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) a required function ptr is missing",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	if( (pCompFuncs->DeserializeLower == LSA_NULL) ||
		(pCompFuncs->DeserializeUpper == LSA_NULL) ||
		(pCompFuncs->SerializeLower   == LSA_NULL) ||
		(pCompFuncs->SerializeUpper   == LSA_NULL) )
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) a required function ptr for serialization is missing",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_PARAM;
	}
#endif

	if(g_pHifData->CompStore.CompSys[CompId].bUsed == LSA_TRUE)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_WARN, "hif_register_sys_component(-) CompId(%d/0x%x) pCompFuncs(0x%x) component already registered",CompId,CompId, pCompFuncs);
		return LSA_RET_ERR_SEQUENCE;
	}

	g_pHifData->CompStore.CompSys[CompId].Comp  = *pCompFuncs;
	g_pHifData->CompStore.CompSys[CompId].bUsed = LSA_TRUE;

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_register_sys_component(+) CompId(%d/0x%x) pCompFuncs(0x%x)",CompId,CompId, pCompFuncs);

	return LSA_RET_OK;
}

/**
 * \brief configure the HIF module
 * 
 * 
 * 
 * @param pRQB
 * @return
 */
LSA_UINT16 hif_ld_system (HIF_RQB_PTR_TYPE pRQB)
{
	HIF_HANDLE hH;
	HIF_INST_PTR_TYPE pHifInst;
	LSA_UINT16 retVal = LSA_RET_ERR_PARAM;
	LSA_COMP_ID_TYPE comp_id;
	
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_system(+) -  pRQB(0x%x)", pRQB);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(pRQB))
	{
		HIF_FATAL(0);
	}
	
	HIF_LD_ENTER();

	comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	if(comp_id != LSA_COMP_ID_HIF)
	{
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH != LSA_NULL);

		g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH(&hH, pRQB);

		hif_get_instance(&pHifInst, hH);
		if(HIF_IS_NULL(pHifInst))
		{
			HIF_FATAL(0); /* invalid handle */
		}

		HIF_ASSERT(hif_is_upper_device(pHifInst) == LSA_TRUE);
		HIF_ASSERT(hif_is_ld_device(pHifInst) == LSA_TRUE);

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_system(+) external sys req => pRQB(0x%x) hH(%d)", pRQB, hH);

		hif_send_admin_request(pHifInst, pRQB);
	}
	else
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_LD_UPPER_OPEN:
			{
				retVal = hif_upper_ld_device_open(pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_OPEN:
			{
				retVal = hif_lower_ld_device_open(pRQB);
				break;
			}
			case HIF_OPC_LD_UPPER_CLOSE:
			{
				retVal = hif_upper_device_close(pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_CLOSE:
			{
				retVal = hif_lower_device_close(pRQB);
				break;
			}
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_ld_system(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				retVal = LSA_RET_ERR_PARAM;
			}
		}

		if(retVal != LSA_RET_OK_ASYNC)
		{
			hif_system_callback(pRQB, retVal);
		}
	}

	HIF_LD_EXIT();
	
	return retVal;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pRQB
 * @return
 */
LSA_UINT16 hif_hd_system (HIF_RQB_PTR_TYPE pRQB)
{
	HIF_HANDLE hH;
	HIF_INST_PTR_TYPE pHifInst;
	LSA_UINT16 retVal = LSA_RET_ERR_PARAM;
	LSA_COMP_ID_TYPE comp_id;
	
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_system(+) -  pRQB(0x%x)", pRQB);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(pRQB))
	{
		HIF_FATAL(0);
	}
	
	HIF_HD_ENTER();
	
	comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	if(comp_id != LSA_COMP_ID_HIF)
	{
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
		HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH != LSA_NULL);

		g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequesthH(&hH, pRQB);

		hif_get_instance(&pHifInst, hH);
		if(HIF_IS_NULL(pHifInst))
		{
			HIF_FATAL(0); /* invalid handle */
		}

		HIF_ASSERT(hif_is_upper_device(pHifInst) == LSA_TRUE);
		HIF_ASSERT(hif_is_ld_device(pHifInst) == LSA_FALSE);

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_system(+) external sys req => pRQB(0x%x) hH(%d)", pRQB, hH);

		hif_send_admin_request(pHifInst, pRQB);
	}
	else
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_HD_UPPER_OPEN:
			{
				retVal = hif_upper_hd_device_open(pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_OPEN:
			{
				retVal = hif_lower_hd_device_open(pRQB);
				break;
			}
			case HIF_OPC_HD_UPPER_CLOSE:
			{
				retVal = hif_upper_device_close(pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_CLOSE:
			{
				retVal = hif_lower_device_close(pRQB);
				break;
			}
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_hd_system(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				retVal = LSA_RET_ERR_PARAM;
			}
		}
	
		if(retVal != LSA_RET_OK_ASYNC)
		{
			hif_system_callback(pRQB, retVal);
		}
	}
	
	HIF_HD_EXIT();
	
	return retVal;
}

/**
 * \brief finish an HIF system request
 * 
 * 
 * 
 * @param pRQB
 * @return
 */
LSA_VOID hif_open_device_lower_done(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_HANDLE hH = (HIF_HANDLE)0xFF;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_open_device_lower_done(+) -  pRQB(0x%x)", pRQB);

	switch(HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
			hH = pRQB->args.dev_ld_open.hH;
			break;
		case HIF_OPC_HD_UPPER_OPEN:
			hH = pRQB->args.dev_hd_open.hH;
			break;
		default:
			HIF_FATAL(0);
	}

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NULL(pHifInst))
	{
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_device_lower_done pRQB(0x%x) hH(%d)",pRQB,hH);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		HIF_LD_ENTER();

		/* restore original system_done Cbf (only relevant for short mode) */
		pRQB->args.dev_ld_open.Cbf = pHifInst->backup.SystemDoneCbf;
		pRQB->args.dev_ld_open.hH  = pHifInst->backup.hH;

		/* proceed device open process */
		hif_lower_ld_device_open_finish(pHifInst, pRQB);

		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_ENTER();

		/* restore original system_done Cbf (only relevant for short mode) */
		pRQB->args.dev_hd_open.Cbf = pHifInst->backup.SystemDoneCbf;
		pRQB->args.dev_hd_open.hH  = pHifInst->backup.hH;

		/* proceed device open process */
		hif_lower_hd_device_open_finish(pHifInst, pRQB);

		HIF_HD_EXIT();
	}
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pRQB
 * @return
 */
LSA_VOID hif_close_device_lower_done(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_HANDLE hH = pRQB->args.dev_close.hH;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_close_device_lower_done(+) -  pRQB(0x%x)", pRQB);

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NULL(pHifInst))
	{
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_device_lower_done pRQB(0x%x) hH(%d)",pRQB,hH);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		/* LD Device */
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	/* restore original system_done Cbf (only relevant for short mode) */
	pRQB->args.dev_close.Cbf = pHifInst->backup.SystemDoneCbf;
	pRQB->args.dev_close.hH  = pHifInst->backup.hH;

	/* proceed with device close process */
	hif_lower_device_close_finish(pHifInst, pRQB);

	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		/* LD Device */
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}
}

/**
 * \brief finish a Common system request
 * 
 * 
 * 
 * @param rqb
 */
LSA_VOID   hif_lower_sys_request_done(LSA_VOID* rqb)
{
	/* (LaM)Real lower_sys_request_done handling is done in hif_lower_sys_request_done2() */
	/* hif_lower_sys_request_done2() has to be called in the correct hif context (HD/LD)! */
	/* This function gives the system adaption the possibility to change contexts         */

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done(+) -  rqb(0x%x)", rqb);

#if ( HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	/* Sys requests returned from lower */

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done() - pRQB(0x%x)",pRQB);

	/* Restore Cbf */
	hif_get_return_path_of_sys_rqb(&pPipe, pRQB);

	/* (LaM) in principle accessing pPipe structure is enter/exit critical!                 */
	/* But for performance reasons we don't use enter/exit here expecting that there is no	*/
	/* HIF user closing the channel/device while there are open requests in the system.		*/

	//if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	//{
	//	HIF_LD_ENTER();
	//}
	//else /* HD Device */
	//{
	//	HIF_HD_ENTER();
	//}

	HIF_SYS_REQUEST_LOWER_DONE(hif_lower_sys_request_done2, pRQB, pPipe->pInstance->hSysDev);

	//if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	//{
	//	HIF_LD_EXIT();
	//}
	//else /* HD Device */
	//{
	//	HIF_HD_EXIT();
	//}
#else
	hif_lower_sys_request_done2(rqb);
#endif
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param rqb
 */
LSA_VOID   hif_lower_sys_request_done2(LSA_VOID* rqb)
{
	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	/* Sys requests returned from lower */

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request_done2() - pRQB(0x%x)",pRQB);

	/* Restore Cbf */
	hif_get_return_path_of_sys_rqb(&pPipe, pRQB);

	hif_restore_user_fields_of_sys_rqb_and_free_return_path(pPipe, pRQB);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	hif_send(pPipe, pRQB); /* Send RQB to HIF Upper */

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}
}

/**
 * \brief HIF Timer timeout
 * 
 * 
 * 
 * @param timer_id
 * @param user_id
 */
LSA_VOID hif_timeout(LSA_UINT16 timer_id, LSA_USER_ID_TYPE  user_id)
{
	HIF_RQB_PTR_TYPE pRQBPoll;
	LSA_UINT32 i;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_timeout(+)");

	LSA_UNUSED_ARG(timer_id);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(user_id.uvar32 == HIF_LD_TIMER_ID) /* LD Poll Timer timeout */
	{
		HIF_ASSERT(g_pHifData->LD.PollTimer.PollUserCnt != 0);

		#if HIF_CFG_MAX_LD_INSTANCES > 0
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->LD.PollTimer.pRQBPoll))
		{
			for(i=0; i<HIF_CFG_MAX_LD_INSTANCES; i++)
			{
				/* send IRQ poll if there is something to receive or there is something in the send queue*/
				if( (g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) && (g_pHifData->Instances[i].if_pars.TmOptions.bTimerPoll == LSA_TRUE)) 
				{
					pRQBPoll = g_pHifData->LD.PollTimer.pRQBPoll;
					g_pHifData->LD.PollTimer.pRQBPoll = LSA_NULL;
					HIF_LD_DO_TIMEOUT_REQUEST(pRQBPoll);
					break;
				}
			}
		}
		#else
		HIF_FATAL(0); /* Couldn't be */
		#endif

	}
	else if(user_id.uvar32 == HIF_HD_TIMER_ID) /* HD Poll Timer timeout */
	{
		HIF_ASSERT(g_pHifData->HD.PollTimer.PollUserCnt != 0);

		#if (HIF_CFG_MAX_HD_INSTANCES > 0)
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->HD.PollTimer.pRQBPoll))
		{
			for(i=HIF_CFG_MAX_LD_INSTANCES; i<HIF_MAX_INSTANCES; i++)
			{
				/* send IRQ poll if there is something to receive */
				if((g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) && (g_pHifData->Instances[i].if_pars.TmOptions.bTimerPoll == LSA_TRUE))
				{
					pRQBPoll = g_pHifData->HD.PollTimer.pRQBPoll;
					g_pHifData->HD.PollTimer.pRQBPoll = LSA_NULL;
					HIF_HD_DO_TIMEOUT_REQUEST(pRQBPoll);
					break;
				}
			}
		}
		#else
		HIF_FATAL(0); /* Couldn't be */
		#endif
	}
	else /* Unknown Timer Id ! */
	{
		HIF_FATAL(0);
	}
}

/**
 * \brief HIF Poll
 * 
 * 
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID hif_poll(LSA_VOID)
{
	LSA_UINT32 i;
	LSA_BOOL bInstTriggered = LSA_FALSE;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_poll(+)");

	#if ( HIF_CFG_MAX_LD_INSTANCES > 0 )
	for(i=0; i<HIF_CFG_MAX_LD_INSTANCES; i++) /* go through LD Devices */
	{
		if( HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll) &&	(g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) )
		{
			bInstTriggered = hif_poll_instance(&g_pHifData->Instances[i]);
			if(bInstTriggered == LSA_TRUE)
			{
				break; /* Only 1 Poll RQB available for all LD Devices, so break Loop if already sent */
			}
		}
	}

	bInstTriggered = LSA_FALSE;
	#endif

	#if ( HIF_CFG_MAX_HD_INSTANCES > 0 )
	for(i=HIF_CFG_MAX_LD_INSTANCES; i<HIF_MAX_INSTANCES; i++) /* go through HD Devices */
	{
		if( HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll) &&	(g_pHifData->Instances[i].bRcvEnabled == LSA_TRUE) )
		{
			bInstTriggered = hif_poll_instance(&g_pHifData->Instances[i]);
			if(bInstTriggered == LSA_TRUE)
			{
				break; /* Only 1 Poll RQB available for all HD Devices, so break Loop if already sent */
			}
		}
	}
	#endif
}

/**
 * \brief HIF Poll Instance
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_BOOL hif_poll_instance(HIF_INST_PTR_TYPE pHifInst)
{
	HIF_RQB_PTR_TYPE pRQBPoll;
	LSA_BOOL bIsLdDevice;
	LSA_BOOL bRQBSent = LSA_FALSE;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_poll_instance(+) - pHifInst(0x%x)", pHifInst);

	HIF_ASSERT(HIF_IS_NOT_NULL(pHifInst));

    // In HSM-variant there is a race condition, because the function's hif_send() and hif_receive() 
    // trigger asynchronously this fucntion. In normal Polling-Mode there is no race condition
    #if (defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)  || (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    HIF_ENTER();
    #endif

	bIsLdDevice = hif_is_ld_device(pHifInst);

	if(bIsLdDevice) /* LD Poll */
	{
		/* send IRQ poll if there is something to receive or there is something in the send queue*/
		if((pHifInst->bRcvEnabled == LSA_TRUE) && (HIF_IS_NOT_NULL(g_pHifData->LD.pRQBPoll)) && (pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE ))
		{
			pRQBPoll = g_pHifData->LD.pRQBPoll;
			g_pHifData->LD.pRQBPoll = LSA_NULL;
			HIF_LD_DO_POLL_REQUEST(pRQBPoll);

			bRQBSent = LSA_TRUE;
		}
	}
	else /* HD Poll */
	{
		/* Poll resource available? */
		if(HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll))
		{
			/* send IRQ poll if there is something to receive */
			if((pHifInst->bRcvEnabled == LSA_TRUE) && (HIF_IS_NOT_NULL(g_pHifData->HD.pRQBPoll)) && ( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE ))
			{
				pRQBPoll = g_pHifData->HD.pRQBPoll;
				g_pHifData->HD.pRQBPoll = LSA_NULL;
				HIF_HD_DO_POLL_REQUEST(pRQBPoll);

				bRQBSent = LSA_TRUE;
			}
		}
	}
    #if (defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)  || (defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    HIF_EXIT();
    #endif
	return bRQBSent;
}

/**
 * \brief HIF Interrupt
 * 
 * 
 * 
 * @param hH
 * @return
 */
LSA_VOID hif_interrupt(HIF_HANDLE hH)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_RQB_PTR_TYPE pRQBIsr;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_interrupt(+) - hH(0x%x)", hH);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	hif_get_instance(&pHifInst, hH);

	if(HIF_IS_NOT_NULL(pHifInst))
	{
		/* ISR resource available? */
		if(HIF_IS_NOT_NULL(pHifInst->pRQBIsr))
		{
			pRQBIsr = pHifInst->pRQBIsr;
			pHifInst->pRQBIsr = LSA_NULL;
			pHifInst->func.DO_INTERRUPT_REQUEST(pRQBIsr);
		}
	}
}

/**
 * \brief finish an HIF system request
 * 
 * 
 * 
 * @param pRQB
 * @param response
 * @return
 */
LSA_VOID hif_system_callback(HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response)
{
	LSA_UINT16 rqbResponse;
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf = LSA_NULL;
	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_system_callback(+) - pRQB(0x%x), response(0x%x)", pRQB, response);

	switch (HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
		{
			pCbf = pRQB->args.dev_ld_open.Cbf;
			break;
		}
		case HIF_OPC_HD_UPPER_OPEN:
		{
			pCbf = pRQB->args.dev_hd_open.Cbf;
			break;
		}
		case HIF_OPC_LD_LOWER_OPEN:
		case HIF_OPC_HD_LOWER_OPEN:
		{	    
			pCbf = pRQB->args.dev_lower_open.Cbf;
			break;
		}
		case HIF_OPC_LD_UPPER_CLOSE:
		case HIF_OPC_LD_LOWER_CLOSE:
		case HIF_OPC_HD_UPPER_CLOSE:
		case HIF_OPC_HD_LOWER_CLOSE:
		{
			pCbf = pRQB->args.dev_close.Cbf;
			break;
		}      
		default:
		{
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_system_callback(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
			HIF_FATAL(0);
		}
	}	
	
	rqbResponse = hif_map_lsa_to_rqb_response(response);
	
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_system_callback pRQB(0x%x) response(0x%x)",pRQB,rqbResponse);
	
	HIF_RQB_SET_RESPONSE(pRQB, rqbResponse);
	
	if(HIF_IS_NOT_NULL(pCbf))
	{
		switch (HIF_RQB_GET_OPCODE(pRQB))
		{
			case HIF_OPC_LD_UPPER_OPEN:
			case HIF_OPC_LD_UPPER_CLOSE:
			{
				HIF_LD_U_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_HD_UPPER_OPEN:
			case HIF_OPC_HD_UPPER_CLOSE:
			{
				HIF_HD_U_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_LD_LOWER_OPEN:
			case HIF_OPC_LD_LOWER_CLOSE:
			{	    
				HIF_LD_L_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}
			case HIF_OPC_HD_LOWER_OPEN:
			case HIF_OPC_HD_LOWER_CLOSE: 
			{	    
				HIF_HD_L_SYSTEM_REQUEST_DONE(pCbf, pRQB);
				break;
			}  
			default:
			{
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_system_callback(-) wrong opcode(%d)",HIF_RQB_GET_OPCODE(pRQB));
				HIF_FATAL(0);
			}
		}
	}
}

LSA_FATAL_ERROR_TYPE  hif_glob_fatal_error; /**< global fatal-error structure */
/**
 * \brief fills the global fatal-error structure and call HIF_FATAL_ERROR
 * 
 * 
 * 
 * 
 * @param module_id
 * @param line
 * @param error_code_0
 * @param error_code_1
 * @param error_code_2
 * @param error_code_3
 * @param error_data_len
 * @param error_data
 * @return
 */
LSA_VOID hif_fatal_error(
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
	hif_glob_fatal_error.lsa_component_id	= LSA_COMP_ID_HIF;
	hif_glob_fatal_error.module_id			= module_id;
	hif_glob_fatal_error.line				= line;

	hif_glob_fatal_error.error_code[0]		= error_code_0;
	hif_glob_fatal_error.error_code[1]		= error_code_1;
	hif_glob_fatal_error.error_code[2]		= error_code_2;
	hif_glob_fatal_error.error_code[3]		= error_code_3;

	hif_glob_fatal_error.error_data_length	= error_data_len;
	hif_glob_fatal_error.error_data_ptr		= error_data;

	HIF_FATAL_ERROR ((LSA_UINT16)(sizeof(hif_glob_fatal_error)), &hif_glob_fatal_error);
}


/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
