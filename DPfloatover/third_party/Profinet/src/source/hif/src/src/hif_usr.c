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
/*  F i l e               &F: hif_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the user LSA interface                                        */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID		1
#define LTRC_ACT_MODUL_ID	1

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 * @brief hif_callback
 * 
 * 
 * 
 * @param pChHandle
 * @param pRQB
 * @param response
 */
LSA_VOID hif_callback(HIF_CH_HANDLE_PTR_TYPE pChHandle, HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_callback(+) - pChHandle(0x%x), response(0x%x)", pChHandle, response);

	HIF_ASSERT(HIF_IS_NOT_NULL(pRQB));
	HIF_ASSERT(HIF_IS_NOT_NULL(pChHandle));
	HIF_ASSERT(HIF_IS_NOT_NULL(pChHandle->pPipe));

	/* set response code */
	HIF_RQB_SET_RESPONSE(pRQB, response);

	/* set handles */
	HIF_RQB_SET_HANDLE(pRQB, pChHandle->pPipe->d.upper.hUpper);

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_callback pRQB(0x%x) response(0x%x)",pRQB,response);

	/* call output macro */
	pChHandle->pPipe->pInstance->func.REQUEST_DONE((HIF_UPPER_CALLBACK_FCT_PTR_TYPE)pChHandle->pCbf, pRQB, pChHandle->sys_ptr);
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_callback");
}

/**
 * @brief hif_open_channel
 * 
 * 
 * 
 * @param rqb
 * @param bLDRequest
 * @param bURequest
 * @return
 */
LSA_VOID hif_open_channel(HIF_RQB_PTR_TYPE rqb, LSA_BOOL bLDRequest, LSA_BOOL bURequest)
{
	LSA_UINT16 retVal;
	LSA_SYS_PATH_TYPE path;
	LSA_HANDLE_TYPE hUpper;

	HIF_INST_PTR_TYPE pHifInst;
	HIF_CH_HANDLE_PTR_TYPE pChHandle;
	HIF_CH_HANDLE_TYPE chHandle;

	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_open_channel(+) - rqb(0x%x), bURequest(0x%x)", rqb, bLDRequest);
    
	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - rqb (0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) - check args",rqb);

	/* get Cbf + Path of RQB (component specific) */

	if(g_pHifData->CompStore.CompStd[HIF_RQB_GET_COMP_ID(rqb)].bUsed != LSA_TRUE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - CompStd[%d].bUsed (0x%x) is not used", HIF_RQB_GET_COMP_ID(rqb), g_pHifData->CompStore.CompStd[HIF_RQB_GET_COMP_ID(rqb)].bUsed);
		HIF_FATAL(0);
	}

	retVal = g_pHifData->CompStore.CompStd[HIF_RQB_GET_COMP_ID(rqb)].Comp.GetOpenChannelCompInfo(rqb, (HIF_UPPER_CALLBACK_FCT_PTR_TYPE*)&(chHandle.pCbf), &path, &hUpper);

	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - GetOpenChannelCompInfo(...) was not successful");
		HIF_FATAL(0); /* HIF Could not get Cbf and Path out of open channel RQB*/
	}
	
	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_open_channel - rqb(0x%x), bURequest(0x%x), pPath(0x%x)", rqb, bLDRequest, path);

	if(bLDRequest == LSA_TRUE)
	{
		/* get Detail Ptr which identifies hif instance + pipe */
		if(bURequest == LSA_TRUE)
		{
			HIF_LD_U_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &chHandle.pDetails, path);
		}
		else
		{
			HIF_LD_L_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &chHandle.pDetails, path);
		}
		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - HIF_LD_L_GET_PATH_INFO(...) was not successful");
			HIF_FATAL(0); /* HIF Could not get the details for the target path */
		}

		HIF_LD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel_ld rqb(0x%x)",rqb);
	}
	else
	{
		/* get Detail Ptr which identifies hif instance + pipe */
		if(bURequest == LSA_TRUE)
		{
			HIF_HD_U_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &chHandle.pDetails, path);
		}
		else
		{
			HIF_HD_L_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &chHandle.pDetails, path);
		}
		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - HIF_HD_L_GET_PATH_INFO(...) was not successful");
			HIF_FATAL(0); /* HIF Could not get the details for the target path */
		}

		HIF_HD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel_hd rqb(0x%x)",rqb);
	}

	/* get Hif Inst */
	hif_get_instance(&pHifInst, chHandle.pDetails->hH);

	if(HIF_IS_NULL(pHifInst))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - pHifInst(0x%x) is not valid", pHifInst);
		HIF_FATAL(0); /* hH from Details wrong */
	}

	/* check if HIF Inst OK for Open Channel */
	if(pHifInst->State != HIF_INST_STATE_OPEN)
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - pHifInst->State(0x%x) is not valid", pHifInst->State);
		HIF_FATAL(0); /* HIF Instance in wrong state */
	}

	/* get Pipe */
	hif_get_pipe(&chHandle.pPipe, pHifInst, chHandle.pDetails->hd_id, chHandle.pDetails->if_id, chHandle.pDetails->pipe_id);
	if(HIF_IS_NULL(chHandle.pPipe))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - chHandle.pPipe(0x%x) is not valid", chHandle.pPipe);
		HIF_FATAL(0); /* Detail specified pipe not found */
	}

	HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) inst(%d) pipe(%d) hd_id(%d) if_id(%d) pipe_id(%d) - forward",rqb,pHifInst->hH,chHandle.pPipe->PipeIndex,chHandle.pPipe->hd_id,chHandle.pPipe->if_id,chHandle.pPipe->pipe_id);

	if(chHandle.pPipe->State != HIF_PIPE_STATE_CLOSED)
	{
		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_open_channel.req (upper) (-) qrb(0x%x) - invalid pipe state (%d)",rqb,chHandle.pPipe->State);
		hif_callback(&chHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));
		
		if(bLDRequest == LSA_TRUE)
		{
			HIF_LD_EXIT();
		}
		else
		{
			HIF_HD_EXIT();
		}
		return;		
	}

	/* alloc new ch handle */
	hif_alloc_ch_handle(chHandle.pPipe, &pChHandle);
	if(HIF_IS_NULL(pChHandle))
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_open_channel.req (upper) (-) rqb(0x%x) - cannot get channel handle",rqb);
		hif_callback(&chHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_RESOURCE));

		if(bLDRequest == LSA_TRUE)
		{
			HIF_LD_EXIT();
		}
		else
		{
			HIF_HD_EXIT();
		}
		return;
	}

	//Copy data to real channel handle structure
	pChHandle->sys_ptr = chHandle.sys_ptr;
	pChHandle->pDetails = chHandle.pDetails;
	pChHandle->pPipe = chHandle.pPipe;
	pChHandle->pCbf = chHandle.pCbf;

	pChHandle->pPipe->d.upper.hUpper = hUpper;
	pChHandle->pPipe->d.upper.ch_handle = pChHandle;
	pChHandle->bValid = LSA_TRUE;

	/* adjust pipe state */
	hif_set_pipe_state(pChHandle->pPipe, HIF_PIPE_STATE_OPENING_UPPER);

	/* save device type specifics */
	if(bLDRequest == LSA_TRUE)
	{
		if(bURequest == LSA_TRUE)
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_LD_U_RELEASE_PATH_INFO;
		}
		else
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_LD_L_RELEASE_PATH_INFO;
		}
	}
	else
	{
		if(bURequest == LSA_TRUE)
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_HD_U_RELEASE_PATH_INFO;
		}
		else
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_HD_L_RELEASE_PATH_INFO;
		}
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) chH(%d) - pipe_state=opening_upper - forward open to hif lower",rqb,pChHandle->hHifCompCh);

	/* open_channel ok for this hif instance */

	/* forward request to lower (upper) hif layer */
	hif_send(pChHandle->pPipe, rqb);

	if(bLDRequest == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}
}

/**
 * @brief hif_ld_l_open_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_l_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_l_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_TRUE, LSA_FALSE);
}

/**
 * @brief hif_ld_u_open_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_u_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_u_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_TRUE, LSA_TRUE);
}

/**
 * @brief hif_hd_l_open_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_l_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_l_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_FALSE, LSA_FALSE);
}

/**
 * @brief hif_hd_u_open_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_u_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_u_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_FALSE, LSA_TRUE);
}

/**
 * @brief hif_close_channel
 * 
 * 
 * 
 * @param rqb
 * @param bLDRequest
 * @return
 */
LSA_VOID hif_close_channel(HIF_RQB_PTR_TYPE rqb, LSA_BOOL bLDRequest)
{
	HIF_CH_HANDLE_PTR_TYPE pChHandle;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_close_channel(+) - rqb(0x%x), bLDRequest(0x%x)", rqb, bLDRequest);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_close_channel(-) - rqb(0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	if(bLDRequest == LSA_TRUE)
	{
		HIF_LD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_channel_ld rqb(0x%x)",rqb);
	}
	else
	{
		HIF_HD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_channel_hd rqb(0x%x)",rqb);
	}

	pChHandle = &(g_pHifData->ChHandles[HIF_RQB_GET_HANDLE(rqb)]);

	if(pChHandle->bUsed == LSA_TRUE)
	{
		if(pChHandle->bValid == LSA_TRUE)
		{
			if(pChHandle->pPipe->State == HIF_PIPE_STATE_OPENED_UPPER)
			{
				HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "hif_close_channel.req (upper) rqb(0x%x) hH(%d) pipe(%d) chH(%d) - forward close to hif lower",rqb,pChHandle->pPipe->pInstance->hH,pChHandle->pPipe->PipeIndex,pChHandle->hHifCompCh);

				/* request from upper => send to lower */
				hif_send(pChHandle->pPipe, rqb);
			}
			else
			{
				HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_UNEXP, "hif_close_channel.req (upper) (-) rqb(0x%x) hH(%d) pipe(%d) chH(%d) - invalid pipe state (%d)",rqb,pChHandle->pPipe->pInstance->hH,pChHandle->pPipe->PipeIndex,pChHandle->hHifCompCh,pChHandle->pPipe->State);

				hif_callback(pChHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));
			}
		}
		else /* invalid channel handle!!! */
		{
			HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_close_channel (upper) (-) rqb(0x%x) chH(%d) channel handle not valid!",rqb,pChHandle->hHifCompCh);
			HIF_FATAL(0);
		}
	}
	else /* invalid channel handle!!! */
	{
		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_close_channel (upper) (-) rqb(0x%x) chH(%d) invalid channel handle",rqb,pChHandle->hHifCompCh);
		HIF_FATAL(0);
	}
	
	if(bLDRequest == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}
}

/**
 * @brief hif_ld_l_close_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_l_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_cm(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_TRUE);
}

/**
 * @brief hif_ld_u_close_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_u_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_u_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_TRUE);
}

/**
 * @brief hif_hd_l_close_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_l_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_l_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_FALSE);
}

/**
 * @brief hif_hd_u_close_channel
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_u_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_u_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_FALSE);
}

/**
 * @brief hif_request
 * 
 * 
 * 
 * @param rqb
 * @param pPollTimer
 * @param bLDRequest
 * @return
 */
LSA_VOID hif_request(HIF_RQB_PTR_TYPE rqb, HIF_POLL_TIMER_TYPE* pPollTimer, LSA_BOOL bLDRequest)
{
	HIF_INST_PTR_TYPE pHifInst;
	HIF_CH_HANDLE_PTR_TYPE pChHandle;
	LSA_UINT16 i;
	LSA_UINT16 uInstStartIndex;
	LSA_UINT16 uInstEndIndex;

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count_lower_req;
	LSA_UINT64 start_count_lower_req = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_request(+) - rqb(0x%x), pPollTimer(0x%x), bLDRequest(0x%x)", rqb, pPollTimer, bLDRequest);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_request(-) - rqb(0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	if(bLDRequest == LSA_TRUE)
	{
		HIF_LD_ENTER();
		uInstStartIndex = 0;
		uInstEndIndex = HIF_CFG_MAX_LD_INSTANCES;
	}
	else
	{
		HIF_HD_ENTER();
		uInstStartIndex = HIF_CFG_MAX_LD_INSTANCES;
		uInstEndIndex = HIF_CFG_MAX_LD_INSTANCES + HIF_CFG_MAX_HD_INSTANCES;
	}

	/* internal request ? */
	if(HIF_RQB_GET_HANDLE(rqb) == HIF_INTERNAL_REQ_RESERVED_CH_HANDLE)
	{
		switch(HIF_RQB_GET_OPCODE(rqb))
		{
			case HIF_OPC_POLL:
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.poll) rqb(0x%x) HIF_OPC_POLL ld(%d)",rqb,bLDRequest);

				/* return resource */
				if(bLDRequest == LSA_TRUE)
				{
					g_pHifData->LD.pRQBPoll = rqb;
				}
				else
				{
					g_pHifData->HD.pRQBPoll = rqb;
				}

				/* check all relevant instances for received messages */
				for(i=uInstStartIndex; i<uInstEndIndex; i++)
				{
					pHifInst = &(g_pHifData->Instances[i]);

					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
						{
							hif_shm_receive(pHifInst);
							hif_shm_send(pHifInst);
							if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
							{
								hif_upper_device_close_finish_2(pHifInst);
							}
						}
					}
				}
				break;
			case HIF_OPC_TIMEOUT_POLL:
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.poll) rqb(0x%x) HIF_OPC_TIMEOUT_POLL ld(%d)",rqb,bLDRequest);

				/* return resource */
				pPollTimer->pRQBPoll = rqb;

				/* check all relevant instances for received messages */
				for(i=uInstStartIndex; i<uInstEndIndex; i++)
				{
					pHifInst = &(g_pHifData->Instances[i]);

					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
						{
							hif_shm_receive(pHifInst);
							hif_shm_send(pHifInst);
							if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
							{
								hif_upper_device_close_finish_2(pHifInst);
							}
						}
					}
				}
				break;
			case HIF_OPC_ISR:
				HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.isr) rqb(0x%x) hH(%d) HIF_OPC_ISR ld(%d)",rqb,rqb->args.internal_req.receive.hHDest,bLDRequest);

				hif_get_instance(&pHifInst, rqb->args.internal_req.receive.hHDest);

				if(HIF_IS_NOT_NULL(pHifInst))
				{
					/* return trigger resource */
					pHifInst->pRQBIsr = rqb;

					//pHifInst->func.ACK_IRQ(pHifInst->hSysDev, pHifInst->hH); /* Acknowledge Interrupt */

					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						hif_shm_receive(pHifInst);
						hif_shm_send(pHifInst);
						if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
						{
							hif_upper_device_close_finish_2(pHifInst);
						}
					}
					
					pHifInst->func.ACK_IRQ(pHifInst->hSysDev, pHifInst->hH); /* Acknowledge Interrupt after hif transfers -> hif_interrupt won't be executed until all hif recieve, send actions finished! */
                    if(pHifInst->bRcvEnabled == LSA_TRUE)
                    {
                        hif_shm_receive(pHifInst);
                    }
				}
				else
				{
					HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (internal.isr) rqb(0x%x) HIF_OPC_ISR - cannot get instance (%d)",rqb,rqb->args.internal_req.receive.hHDest);
					HIF_FATAL(0);
				}
				break;
			case HIF_OPC_RECEIVE:
				hif_get_instance(&pHifInst, rqb->args.internal_req.receive.hHDest);
				if(HIF_IS_NOT_NULL(pHifInst))
				{
					HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.receive) rqb(0x%x) hH(%d) HIF_OPC_RECEIVE ld(%d)",rqb,rqb->args.internal_req.receive.hHDest,bLDRequest);

					/* return trigger resource */
					pHifInst->pRQBShort = rqb;
					/* process received rqbs */
					
                    hif_process_rcv_list (pHifInst);
				}
				else
				{
					HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (internal.receive) rqb(0x%x) HIF_OPC_RECEIVE - cannot get instance (%d) ld(%d)",rqb,rqb->args.internal_req.receive.hHDest,bLDRequest);
					HIF_FATAL(0);
				}
				break;
			default:
				HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (internal) rqb(0x%x) invalid internal opcode (%d) ld(%d)",rqb,HIF_RQB_GET_OPCODE(rqb),bLDRequest);
				HIF_FATAL(0);
				break;
		}
	}
	else /* external request! */
	{
		pChHandle = &(g_pHifData->ChHandles[HIF_RQB_GET_HANDLE(rqb)]);

		if(pChHandle->bUsed == LSA_TRUE)
		{
			if(pChHandle->bValid == LSA_TRUE)
			{
				if(pChHandle->pPipe->State == HIF_PIPE_STATE_OPENED_UPPER)
				{
					HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (external) rqb(0x%x) hH(%d) pipe(%d) - forward request to hif lower) ld(%d)",rqb,pChHandle->pPipe->pInstance->hH,pChHandle->pPipe->PipeIndex,bLDRequest);

					/* request from upper => send to lower */
					hif_send(pChHandle->pPipe, rqb);
				}
				else
				{
					HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_UNEXP, "hif_request (external) rqb(0x%x) hH(%d) pipe(%d) - bad pipe state (%d)",rqb,pChHandle->pPipe->pInstance->hH,pChHandle->pPipe->PipeIndex,pChHandle->pPipe->State);

					hif_callback(pChHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));
				}
			}
			else /* invalid channel handle!!! */
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (external) channel handle not valid!");
				HIF_FATAL(0);
			}
		}
		else /* invalid channel handle!!! */
		{
			HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (external) invalid channel handle");
			HIF_FATAL(0);
		}
	}

	if(bLDRequest == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_lower_req = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_request(): The total function duration is %d ns",(end_count_lower_req - start_count_lower_req)); //TOD0 z002p64u
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_request");
	#endif
}

/**
 * @brief hif_ld_l_request
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_l_request(HIF_RQB_PTR_TYPE  rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_ld_l_request(+) - rqb(0x%x), g_pHifData(0x%x) ",rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->LD.PollTimer, LSA_TRUE);
}

/**
 * @brief hif_ld_u_request
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_ld_u_request(HIF_RQB_PTR_TYPE  rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_ld_u_request(+) - rqb(0x%x), g_pHifData(0x%x) ",rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->LD.PollTimer, LSA_TRUE);
}

/**
 * @brief hif_hd_l_request
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_l_request(HIF_RQB_PTR_TYPE  rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_hd_l_request(+) - rqb(0x%x), g_pHifData(0x%x) ",rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->HD.PollTimer, LSA_FALSE);
}

/**
 * @brief hif_hd_u_request
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_hd_u_request(HIF_RQB_PTR_TYPE  rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_hd_u_request(+) - rqb(0x%x), g_pHifData(0x%x) ",rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->HD.PollTimer, LSA_FALSE);
}

/**
 * @brief hif_lower_request_done
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_lower_request_done (LSA_VOID*  rqb)
{
	/* (LaM)Real lower_request_done handling is done in hif_lower_request_done2()    */
	/* hif_lower_request_done2() has to be called in the correct hif context (HD/LD) */
	/* This function gives the system adaption the possibility to change contexts    */

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_request_done(+) - rqb(0x%x)", rqb);

#if ( HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	if(HIF_IS_NULL(pRQB))
	{
		HIF_FATAL(0);
	}

	/* find source of request */
	hif_get_return_path_of_rqb(&pPipe, pRQB);
	HIF_ASSERT(HIF_IS_NOT_NULL(pPipe));

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

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_lower_request_done rqb(0x%x) hH(%d) pipe(%d) - return request to hif upper",rqb,pPipe->pInstance->hH,pPipe->Index);

	HIF_REQUEST_LOWER_DONE(hif_lower_request_done2, pRQB, pPipe->pInstance->hSysDev);

	//if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	//{
	//	HIF_LD_EXIT();
	//}
	//else /* HD Device */
	//{
	//	HIF_HD_EXIT();
	//}
#else
	hif_lower_request_done2(rqb);
#endif
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_lower_request_done");

}

/**
 * @brief hif_lower_request_done2
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_lower_request_done2 (LSA_VOID*  rqb)
{
	HIF_RQB_PTR_TYPE pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count_lower_req;
	LSA_UINT64 start_count_lower_req = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_request_done2(+) - rqb(0x%x)", rqb);

	if(HIF_IS_NULL(pRQB))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_request_done2(-) - pRQB(0x%x) is not valid", pRQB);
		HIF_FATAL(0);
	}

	/* find source of request */
	hif_get_return_path_of_rqb(&pPipe, pRQB);
	HIF_ASSERT(HIF_IS_NOT_NULL(pPipe));

	hif_restore_rqb_user_fields_and_free_return_path(pRQB);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	if(pPipe->State == HIF_PIPE_STATE_OPENING_LOWER)
	{
		hif_lower_open_channel_done(pPipe, pRQB);
	}	
	else
	{
		HIF_ASSERT(pPipe->func.pCompFuncs != LSA_NULL);
		HIF_ASSERT(pPipe->func.pCompFuncs->IsCloseChannelRqb != LSA_NULL);
		if(pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE)
		{
			hif_lower_close_channel_done(pPipe, pRQB);
		}
		else
		{
			HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_lower_request_done2 rqb(0x%x) hH(%d) pipe(%d) - return request to hif upper",rqb,pPipe->pInstance->hH,pPipe->PipeIndex);

			/* Other RQBs => return response to HIF Upper */
			hif_send(pPipe, pRQB);
		}
	}

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_lower_req = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_lower_request_done2(): The total function duration is %d ns",(end_count_lower_req - start_count_lower_req));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_lower_request_done2");
	#endif
}


/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
