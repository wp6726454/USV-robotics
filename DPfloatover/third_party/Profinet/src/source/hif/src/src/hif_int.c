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
/*  F i l e               &F: hif_int.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements hif internal helper functions                                 */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID		4
#define LTRC_ACT_MODUL_ID	4

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 * \brief <empty>
 * 
 * other
 * 
 * @param pHifInst
 * @return
 */
LSA_UINT32 hif_get_if_ident(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT32 uIfIdent = 0;
	LSA_UINT32 i = 1;
	LSA_BOOL bIsLdDevice = hif_is_ld_device(pHifInst);

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_if_ident(+) -  pHifInst(0x%x)", pHifInst);

	/* Interface Ident is generated out of the static/dynamic configuration */
	/* and used to check the upper against the lower Hif Device interface.  */
	/* It includes information about ld/hd device and the pipe mapping      */

	#if (HIF_CFG_USE_ACP == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_CLRPC == 1)
	uIfIdent |= i;
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_CM == 1)
	uIfIdent |= i;
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_DCP == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_EDD == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_GSY == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_IOH == 1)
	uIfIdent |= i;
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_LLDP == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_MRP == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_NARE == 1)
	if(!bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_OHA == 1)
	uIfIdent |= i;
	#endif
	i=i<<1;

	i=i<<1; /* Historical HIF_CFG_USE_PNTRC which was removed */

	#if (HIF_CFG_USE_PSI == 1)
	uIfIdent |= i;
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_SOCK == 1)
	if(bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	#if (HIF_CFG_USE_SNMPX == 1)
	if(bIsLdDevice)
	{
		uIfIdent |= i;
	}
	#endif
	i=i<<1;

	if(bIsLdDevice) /* ld device? */
	{
		uIfIdent |= i;
	}
	i=i<<1;

	if(!bIsLdDevice) /* hd device? */
	{
		uIfIdent |= i;
	}
	i=i<<1;

	#if (HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS == 1)
	#if (HIF_CFG_USE_SOCK == 1)
	if(!bIsLdDevice) /* additional usr specific sock pipes used in hd */
	{
		uIfIdent |= i;
	}
	#endif
	#endif
	i=i<<1;
	i=i<<1;

	if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE) /* Serialization used */
	{
		uIfIdent |= i;
	}
	i=i<<1;

	return uIfIdent;
}

/**
 * \brief <empty>
 * 
 * mapping
 * 
 * @param lsaResponseVal
 * @return
 */
LSA_UINT16 hif_map_lsa_to_rqb_response(LSA_UINT16 lsaResponseVal)
{

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_map_lsa_to_rqb_response(+) -  lsaResponseVal(0x%x)", lsaResponseVal);

	switch(lsaResponseVal)
	{
		case LSA_RET_OK:
			return HIF_OK;
		case LSA_RET_ERR_PARAM:
			return HIF_ERR_PARAM;
		case LSA_RET_ERR_SEQUENCE:
			return HIF_ERR_SEQUENCE;
		case LSA_RET_ERR_SYS_PATH:
			return HIF_ERR_SYS_PATH;
	}
	return HIF_ERR_PARAM;
}

/**
 * \brief <empty>
 * 
 * internal RQBs
 * 
 * @param ppRQB
 * @param Opcode
 * @return
 */
LSA_RESULT hif_alloc_local_internal_rqb(HIF_RQB_PTR_TYPE* ppRQB, LSA_OPCODE_TYPE Opcode)
{
	LSA_UINT16 RqbSize = sizeof(HIF_RQB_TYPE);

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_alloc_local_internal_rqb(+) -  ppRQB(0x%x), Opcode(0x%x)", ppRQB, Opcode);

	HIF_ASSERT(ppRQB != LSA_NULL);

	HIF_ALLOC_LOCAL_MEM((void**) ppRQB, RqbSize);

	if(HIF_IS_NULL(*ppRQB))
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_alloc_local_internal_rqb(-) cannot get memory for internal RQB opc(%d)",Opcode);
		return LSA_RET_ERR_RESOURCE;
	}

	HIF_MEMSET(*ppRQB, 0, RqbSize);

	HIF_RQB_SET_OPCODE(*ppRQB, Opcode);

	HIF_RQB_SET_HANDLE(*ppRQB, HIF_INTERNAL_REQ_RESERVED_CH_HANDLE);

	return LSA_RET_OK;
}

LSA_VOID hif_free_local_internal_rqb(HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16  retVal;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_free_local_internal_rqb(+) -  pRQB(0x%x)", pRQB);

	HIF_ASSERT(pRQB != LSA_NULL);

	/* currently there are no opcode specific arguments */
	HIF_FREE_LOCAL_MEM(&retVal,pRQB);

	HIF_ASSERT(retVal == LSA_RET_OK);
}

/**
 * \brief <empty>
 * 
 * HIF channel handles
 * 
 * @param pPipe
 * @param ppHifChHandle
 * @return
 */
LSA_VOID hif_alloc_ch_handle(HIF_PIPE_PTR_TYPE pPipe, HIF_CH_HANDLE_PTR_TYPE* ppHifChHandle)
{
	LSA_UINT16 i;
	HIF_CH_HANDLE_PTR_TYPE pHifChHandle;
	LSA_UINT16 loop_start=0;
	LSA_UINT16 loop_end=0;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_alloc_ch_handle(+) -  pPipe(0x%x), ppHifChHandle(0x%x)", pPipe, ppHifChHandle);

	*ppHifChHandle = LSA_NULL;

	/* Estimate resource pool */
	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		/* LD Device */
		loop_start = 1;
		loop_end = 1+HIF_CFG_MAX_LD_CH_HANDLES;
	}
	else /* HD Device */
	{
		loop_start = 1+HIF_CFG_MAX_LD_CH_HANDLES;
		loop_end = HIF_MAX_CH_HANDLES;
	}

	/* Find free resource */
	for(i=loop_start; i<loop_end; i++)
	{
		pHifChHandle = &(g_pHifData->ChHandles[i]);

		if(pHifChHandle->bUsed == LSA_FALSE)
		{
			pHifChHandle->bUsed = LSA_TRUE;

			*ppHifChHandle = pHifChHandle;
			return;
		}
	}

	return;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifChHandle
 * @return
 */
LSA_VOID hif_free_ch_handle(HIF_CH_HANDLE_PTR_TYPE pHifChHandle)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_free_ch_handle(+) -  pHifChHandle(0x%x)", pHifChHandle);

	HIF_ASSERT(HIF_IS_NOT_NULL(pHifChHandle));
	HIF_ASSERT(pHifChHandle->bUsed != LSA_FALSE);

	pHifChHandle->bValid = LSA_FALSE;
	pHifChHandle->bUsed  = LSA_FALSE;
}

/**
 * \brief <empty>
 * 
 * HIF rb tags
 * 
 * @param ppHifRbTag
 * @param pPipe
 * @param pRQB
 * @return
 */
LSA_VOID hif_alloc_rb_tag(HIF_RB_TAG_PTR_TYPE* ppHifRbTag, HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UNUSED_ARG(pRQB);

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_alloc_rb_tag(+) - ppHifRbTag(0x%x), pPipe(0x%x), pRQB(0x%x)", ppHifRbTag, pPipe, pRQB);

	HIF_ALLOC_LOCAL_MEM((LSA_VOID**)ppHifRbTag, sizeof(HIF_RB_TAG_TYPE));

	if(HIF_IS_NULL(*ppHifRbTag)) /* Out of resources */
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_alloc_rb_tag(-) ppHifRbTag(0x%x) is not valid", ppHifRbTag);
		HIF_FATAL(0);
	}

	(*ppHifRbTag)->pPipe = pPipe;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifRbTag
 * @return
 */
LSA_VOID hif_free_rb_tag(HIF_RB_TAG_PTR_TYPE pHifRbTag)
{
	LSA_UINT16 retVal;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_free_rb_tag(+) - pHifRbTag(0x%x)", pHifRbTag);

	HIF_ASSERT(HIF_IS_NOT_NULL(pHifRbTag));

	HIF_FREE_LOCAL_MEM(&retVal, pHifRbTag);
	HIF_ASSERT(retVal == LSA_RET_OK);
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param ppHifSysRbTag
 * @param pPipe
 * @param pRQB
 * @return
 */
LSA_VOID hif_alloc_sys_rb_tag(HIF_SYS_RB_TAG_PTR_TYPE* ppHifSysRbTag, HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_alloc_sys_rb_tag(+) - ppHifSysRbTag(0x%x), pPipe(0x%x), pRQB(0x%x)",  ppHifSysRbTag, pPipe, pRQB);

    LSA_UNUSED_ARG(pRQB);

	HIF_ASSERT(HIF_IS_NOT_NULL(ppHifSysRbTag));
	HIF_ALLOC_LOCAL_MEM((LSA_VOID**)ppHifSysRbTag, sizeof(HIF_SYS_RB_TAG_TYPE));

	if(HIF_IS_NULL(*ppHifSysRbTag))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_alloc_sys_rb_tag(-) ppHifSysRbTag(0x%x) is not valid", ppHifSysRbTag);
		HIF_FATAL(0);
	}

	(*ppHifSysRbTag)->pPipe = pPipe;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifSysRbTag
 * @return
 */
LSA_VOID hif_free_sys_rb_tag(HIF_SYS_RB_TAG_PTR_TYPE pHifSysRbTag)
{
	LSA_UINT16 retVal;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_free_sys_rb_tag(+) - pHifSysRbTag(0x%x)",  pHifSysRbTag);

	HIF_ASSERT(HIF_IS_NOT_NULL(pHifSysRbTag));

	HIF_FREE_LOCAL_MEM(&retVal, pHifSysRbTag);
	HIF_ASSERT(retVal == LSA_RET_OK);
}

/**
 * \brief <empty>
 * 
 * HIF instance
 * 
 * @param ppHifInst
 * @param opcode
 * @return
 */
LSA_VOID hif_alloc_instance(HIF_INST_PTR_TYPE* ppHifInst, LSA_OPCODE_TYPE opcode)
{
	LSA_UINT16 i;
	LSA_UINT16 loop_start=0;
	LSA_UINT16 loop_end=0;
	HIF_INST_PTR_TYPE pHifInst;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_alloc_instance(+) -  ppHifInst(0x%x), opcode(0x%x)", ppHifInst, opcode);

	/* Estimate resource pool */
	if( (opcode == HIF_OPC_LD_UPPER_OPEN) ||
		(opcode == HIF_OPC_LD_LOWER_OPEN) )
	{
		/* LD Device */
		loop_start = 0;
		loop_end = HIF_CFG_MAX_LD_INSTANCES;
	}
	else /* HD Device */
	{
		loop_start = HIF_CFG_MAX_LD_INSTANCES;
		loop_end = HIF_MAX_INSTANCES;
	}

	*ppHifInst = LSA_NULL;

	/* Find free resource */
	for(i=loop_start; i<loop_end; i++)
	{
		pHifInst = &(g_pHifData->Instances[i]);

		if(pHifInst->State == HIF_INST_STATE_FREE)
		{
			switch(opcode)
			{
				case HIF_OPC_LD_UPPER_OPEN:
					pHifInst->Type = HIF_DEV_TYPE_LD_UPPER;
					pHifInst->bUpperDevice = LSA_TRUE;
					pHifInst->bLowerDevice = LSA_FALSE;
					pHifInst->func.REQUEST_DONE = HIF_LD_REQUEST_UPPER_DONE;
				break;
				case HIF_OPC_LD_LOWER_OPEN:
					pHifInst->Type = HIF_DEV_TYPE_LD_LOWER;
					pHifInst->bUpperDevice = LSA_FALSE;
					pHifInst->bLowerDevice = LSA_TRUE;
					pHifInst->func.REQUEST_DONE = HIF_LD_REQUEST_UPPER_DONE; //ToDo Check if UPPER is correct here?
				break;
				case HIF_OPC_HD_UPPER_OPEN:
					pHifInst->Type = HIF_DEV_TYPE_HD_UPPER;
					pHifInst->bUpperDevice = LSA_TRUE;
					pHifInst->bLowerDevice = LSA_FALSE;
					pHifInst->func.REQUEST_DONE = HIF_HD_REQUEST_UPPER_DONE;
				break;
				case HIF_OPC_HD_LOWER_OPEN:
					pHifInst->Type = HIF_DEV_TYPE_HD_LOWER;
					pHifInst->bUpperDevice = LSA_FALSE;
					pHifInst->bLowerDevice = LSA_TRUE;
					pHifInst->func.REQUEST_DONE = HIF_HD_REQUEST_UPPER_DONE; //ToDo Check if UPPER is correct here?
				break;
				default:
				{
					HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_alloc_instance(-) cannot alloc instance with opcode(%d)", opcode);
					return;
				}
			}

			pHifInst->State = HIF_INST_STATE_OPENING;

			hif_set_pipe_state(&g_pHifData->Instances[i].Pipes[HIF_GLOB_ADMIN], HIF_PIPE_STATE_CLOSED);

			*ppHifInst = pHifInst;
			return;
		}
	}

	return;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_VOID hif_free_instance(HIF_INST_PTR_TYPE pHifInst)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_free_instance(+) -  pHifInst(0x%x)", pHifInst);

	HIF_ASSERT(pHifInst != LSA_NULL);
	HIF_ASSERT(pHifInst->State != HIF_INST_STATE_FREE);

	pHifInst->State = HIF_INST_STATE_FREE;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param ppHifInst
 * @param hH
 * @return
 */
LSA_VOID hif_get_instance(HIF_INST_PTR_TYPE* ppHifInst, HIF_HANDLE hH)
{
	*ppHifInst = LSA_NULL;

	if(hH < HIF_MAX_INSTANCES)
	{
		if(g_pHifData->Instances[hH].State != HIF_INST_STATE_FREE)
		{
			*ppHifInst = &(g_pHifData->Instances[hH]);
		}
	}

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_get_instance(-) -  ppHifInst(0x%x), hH(0x%x)", ppHifInst, hH);
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @return
 */
LSA_BOOL hif_check_for_open_instances()
{
	LSA_UINT16 i;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_check_for_open_instances(+)");

	for(i=0; i<HIF_MAX_INSTANCES; i++)
	{
		if(g_pHifData->Instances[i].State != HIF_INST_STATE_FREE)
		{
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_BOOL hif_is_upper_device(HIF_INST_PTR_TYPE pHifInst)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_cm(+) - pHifInst(0x%x)", pHifInst);

	return pHifInst->bUpperDevice;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_BOOL hif_is_ld_device(HIF_INST_PTR_TYPE pHifInst)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_ld_device(+) -  pHifInst(0x%x)", pHifInst);

	return pHifInst->bLDDevice;
}

/**
 * \brief <empty>
 * 
 * HIF RQB Queues
 * 
 * @param rqb_queue   first element of the local send queue
 * @param rqb         pointer to the RQB, stored in the host’s local memory
 * @return
 */
LSA_UINT32    hif_test_rqb_queue (
                HIF_RQB_PTR_TYPE  *rqb_queue,
                HIF_RQB_PTR_TYPE  rqb)
{
	HIF_RQB_PTR_TYPE  p;
	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_test_rqb_queue(+) -  rqb_queue(0x%x), rqb(0x%x)", rqb_queue, rqb);    
    
    HIF_ASSERT (HIF_IS_NOT_NULL (rqb));

	p = *rqb_queue;
	if (HIF_IS_NULL(p))
	{
		return 0;
	}
	if (HIF_ARE_EQUAL (p, rqb))
	{
		return 1;
	}

	p = HIF_RQB_GET_NEXT_RQB_PTR(p);
	while  (HIF_IS_NOT_NULL (p))
	{
		if (HIF_ARE_EQUAL (p, rqb))
		{
			return 1;
		}
		p = HIF_RQB_GET_NEXT_RQB_PTR(p);
	}

	return 0;
}

/**
 * \brief Adds the RQB to the queue
 * 
 * 
 * @param rqb_queue   first element of the local send queue
 * @param rqb         pointer to the RQB, stored in the host’s local memory
 * @param PipeIndex
 * @return
 *        - 1: first element in the queue
 *        - 0: else
 */
LSA_UINT16    hif_add_to_rqb_queue_as_first ( HIF_RQB_PTR_TYPE  *rqb_queue,
                                              HIF_RQB_PTR_TYPE  rqb,
                                              LSA_UINT16        PipeIndex)
{
	// SEND queue - first element of the queue
	LSA_UINT32  ret_val;
	HIF_RQB_PTR_TYPE   first, last;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_add_to_rqb_queue_as_first(+) -  rqb_queue(0x%x), rqb(0x%x), PipeIndex(0x%x)", rqb_queue, rqb, PipeIndex);    

    HIF_ASSERT (HIF_IS_NOT_NULL (rqb_queue));
	HIF_ASSERT (HIF_IS_NOT_NULL (rqb));
    HIF_ASSERT(PipeIndex < HIF_MAX_PIPES_INST);

	HIF_RQB_SET_NEXT_RQB_PTR (rqb, LSA_NULL);
	HIF_RQB_SET_PREV_RQB_PTR (rqb, LSA_NULL);

	ret_val = hif_test_rqb_queue (rqb_queue, rqb);
	if (ret_val)
	{
		HIF_FATAL(0);
	}

    //Set PipeIndex to the second byte of comp id, the comp id itself does not touched
    // Byte   0     |   1  
    //      comp id | PipeIndex
    HIF_RQB_SET_COMP_ID(rqb, HIF_RQB_GET_COMP_ID(rqb) | (PipeIndex<<8));

	// check if send queue is not empty
	if (HIF_IS_NULL(*rqb_queue))
	{
		HIF_RQB_SET_PREV_RQB_PTR (rqb, rqb);
		*rqb_queue = rqb;
		return 1;
	}

	first = *rqb_queue;
	last = (HIF_RQB_PTR_TYPE)HIF_RQB_GET_PREV_RQB_PTR(first);
	HIF_ASSERT (HIF_IS_NOT_NULL (last));

	HIF_RQB_SET_NEXT_RQB_PTR (rqb, first);
	HIF_RQB_SET_PREV_RQB_PTR (rqb, last);
	HIF_RQB_SET_PREV_RQB_PTR (first, LSA_NULL);
	*rqb_queue = rqb;

	return 0;
}

/**
 * \brief Adds the RQB to the queue
 * 
 * 
 * 
 * @param rqb_queue   first element of the local send queue
 * @param rqb         pointer to the RQB, stored in the host’s local memory
 * @param PipeIndex
 * @return
 *        - 1: first element in the queue
 *        - 0: else
 */
LSA_UINT16    hif_add_to_rqb_queue ( HIF_RQB_PTR_TYPE  *rqb_queue,
                                     HIF_RQB_PTR_TYPE  rqb,
                                     LSA_UINT16        PipeIndex )
{
    // SEND queue - first element of the queue
	LSA_UINT32        ret_val;
	HIF_RQB_PTR_TYPE  first, last;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_add_to_rqb_queue(+) -  rqb_queue(0x%x), rqb(0x%x), PipeIndex(0x%x)", rqb_queue, rqb, PipeIndex);

    HIF_ASSERT (HIF_IS_NOT_NULL (rqb_queue));
	HIF_ASSERT (HIF_IS_NOT_NULL (rqb));
    HIF_ASSERT(PipeIndex < HIF_MAX_PIPES_INST);

	HIF_RQB_SET_NEXT_RQB_PTR (rqb, LSA_NULL);
	HIF_RQB_SET_PREV_RQB_PTR (rqb, LSA_NULL);
    
    //Set PipeIndex to the second byte of comp id, the comp id itself does not touched
    // Byte   0     |   1  
    //      comp id | PipeIndex
    HIF_RQB_SET_COMP_ID(rqb, HIF_RQB_GET_COMP_ID(rqb) | (PipeIndex<<8));

	ret_val = hif_test_rqb_queue (rqb_queue, rqb);
	if (ret_val)
	{
		HIF_FATAL(0);
	}

	// check if send queue is not empty
	if (HIF_IS_NULL(*rqb_queue))
	{
		HIF_RQB_SET_PREV_RQB_PTR (rqb, rqb);
		*rqb_queue = rqb;
		return 1;
	}

	first = *rqb_queue;
	last = (HIF_RQB_PTR_TYPE)HIF_RQB_GET_PREV_RQB_PTR(first);
	HIF_ASSERT (HIF_IS_NOT_NULL (last));

	HIF_RQB_SET_PREV_RQB_PTR (first, rqb);
	HIF_RQB_SET_NEXT_RQB_PTR (last, rqb);

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_add_to_rqb_queue(-) -  rqb_queue(0x%x), rqb(0x%x), PipeIndex(0x%x)", rqb_queue, rqb, PipeIndex);    

	return 0;
}

/**
 * \brief Returns the first RQB and removes it from the queue
 * 
 * 
 * 
 * @param rqb_queue   first element of the local send queue
 * @param rqb         pointer to the RQB, stored in the host’s local memory
 * @param pPipeIndex
 * @return
 *        - 1: first element in the queue
 *        - 0: else
 */
LSA_UINT16    hif_get_from_rqb_queue ( HIF_RQB_PTR_TYPE  *rqb_queue,
                                       HIF_RQB_PTR_TYPE  *rqb,
                                       LSA_UINT16        *pPipeIndex)
{	
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_get_from_rqb_queue(+) -  rqb_queue(0x%x), rqb(0x%x), pPipeIndex(0x%x)", rqb_queue, rqb, pPipeIndex);    
    
    HIF_ASSERT (HIF_IS_NOT_NULL (rqb_queue));
	HIF_ASSERT (HIF_IS_NOT_NULL (rqb));

	*rqb = LSA_NULL;
	if (HIF_IS_NULL (*rqb_queue))
	{
		return 0;
	}

	*rqb = (*rqb_queue);

    //Get the PipeIndex from the second byte of comp id
    // Byte   0     |   1  
    //      comp id | PipeIndex        
    *pPipeIndex = (HIF_RQB_GET_COMP_ID(*rqb) >> 8) & 0xFF;

    HIF_ASSERT(*pPipeIndex < HIF_MAX_PIPES_INST);

    //Restore comp id 
    HIF_RQB_SET_COMP_ID(*rqb, HIF_RQB_GET_COMP_ID(*rqb) & 0xFF); 

	*rqb_queue = (HIF_RQB_PTR_TYPE) HIF_RQB_GET_NEXT_RQB_PTR (*rqb_queue);

	if (HIF_IS_NOT_NULL (*rqb_queue))
	{
		HIF_ASSERT (HIF_IS_NOT_NULL (HIF_RQB_GET_PREV_RQB_PTR (*rqb)));
		HIF_RQB_SET_PREV_RQB_PTR (*rqb_queue, HIF_RQB_GET_PREV_RQB_PTR (*rqb));
	}


    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_get_from_rqb_queue(-) -  rqb_queue(0x%x), rqb(0x%x), pPipeIndex(0x%x)", rqb_queue, rqb, pPipeIndex);    

	return 1;
}

/**
 * \brief Returns the first RQB and removes it from the queue
 * 
 * 
 * 
 * @param rqb_queue   first element of the queue
 * @param rqb         pointer to the RQB, stored in the host’s local memory.
 * @return
 *        - 1: first element in the queue
 *        - 0: else
 */
LSA_UINT16    hif_unlink_from_rqb_queue (
                HIF_RQB_PTR_TYPE  *rqb_queue,
                HIF_RQB_PTR_TYPE  rqb)
{
	HIF_RQB_PTR_TYPE  old, p;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_unlink_from_rqb_queue(+) -  rqb_queue(0x%x), rqb(0x%x)", rqb_queue, rqb);    

	HIF_ASSERT (HIF_IS_NOT_NULL (rqb_queue));
	HIF_ASSERT (HIF_IS_NOT_NULL (rqb));

	old = LSA_NULL;
	p = *rqb_queue;

	while  (HIF_IS_NOT_NULL (p))
	{
		if (HIF_ARE_EQUAL (p, rqb))
		{
			/* if first */
			if (HIF_IS_NULL (old))
			{
				*rqb_queue = HIF_RQB_GET_NEXT_RQB_PTR (p);
				if (HIF_IS_NOT_NULL (*rqb_queue))
				{
					HIF_RQB_SET_PREV_RQB_PTR (*rqb_queue, HIF_RQB_GET_PREV_RQB_PTR (p));
				}
			}
			/* else 2 .. n.te element */
			else
			{
				HIF_RQB_SET_NEXT_RQB_PTR (old, HIF_RQB_GET_NEXT_RQB_PTR (p));
				/* if last element in the list */
				if ( HIF_IS_NULL (HIF_RQB_GET_NEXT_RQB_PTR (old)))
				{
					/* link the _prev pointer of rqb_queue to the end of the list */
					HIF_RQB_SET_PREV_RQB_PTR (*rqb_queue, old);
				}
			}
			return 1;
		}
		old = p;
		p = (HIF_RQB_PTR_TYPE) HIF_RQB_GET_NEXT_RQB_PTR (p);
	}

    return 0;
}


/*------------------------------------------------------------------------------
//	HIF DE-/SERIALIZATION FUNCTIONS
//----------------------------------------------------------------------------*/

/**
 * \brief Get original upper rqb-pointer
 * 
 * gets rqb-id (original upper rqb-pointer) which is stored in shared-mem before the serialized rqb.
 * 
 * @param rqbShm
 * @return
 *          - rqb_id (original upper rqb-pointer)
 */
LSA_UINT32 hif_get_rqb_id(LSA_VOID* rqbShm)
{
	LSA_UINT32 tempID = 0;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_unlink_from_rqb_queue(+) -  rqbShm(0x%x)", rqbShm);    

	tempID = HIF_SHMTOL_L(*((LSA_UINT32*)(rqbShm)));

	return tempID;
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
