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
/*  F i l e               &F: hif_sr.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements Hif Upper-Lower Send/Receive handling                         */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID		6
#define LTRC_ACT_MODUL_ID	6

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 */
LSA_UINT16 hif_enable_receive(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 retVal = LSA_RET_OK;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_enable_receive(+) -  pHifInst(0x%x)", pHifInst);

	/* activate Timer Poll */
	if( pHifInst->if_pars.TmOptions.bTimerPoll == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
            #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
			/* LD Device */
			if(g_pHifData->LD.PollTimer.PollUserCnt == 0)
			{
                LSA_USER_ID_TYPE uTimerHandle;
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - activate hif ld timer poll - hH(%d)",pHifInst->hH);

				uTimerHandle.uvar32 = HIF_LD_TIMER_ID;
				HIF_START_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer, uTimerHandle, HIF_CFG_POLL_INTERVAL);
			}
			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_enable_receive() - HIF Timer for LD couldn't start - hH(%d)",pHifInst->hH);
				return retVal;
			}
			g_pHifData->LD.PollTimer.PollUserCnt++;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - timer ld poll cnt++ - hH(%d)",pHifInst->hH);
            #endif
		}
		else /* HD Device */
		{
            #if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
			if(g_pHifData->HD.PollTimer.PollUserCnt == 0)
			{
                LSA_USER_ID_TYPE uTimerHandle;
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - activate hif hd timer poll - hH(%d)",pHifInst->hH);

				uTimerHandle.uvar32 = HIF_HD_TIMER_ID;
				HIF_START_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer, uTimerHandle, HIF_CFG_POLL_INTERVAL);
			}
			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_enable_receive() - HIF Timer for HD couldn't start - hH(%d)",pHifInst->hH);
				return retVal;
			}
			g_pHifData->HD.PollTimer.PollUserCnt++;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - timer hd poll cnt++ - hH(%d)",pHifInst->hH);
            #endif
		}
	}

	/* enable Interrupts */
	if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
			/* LD Device */
			HIF_LD_ENABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
		else /* HD Device */
		{
			HIF_HD_ENABLE_ISR(pHifInst->hSysDev, pHifInst->hH);	
		}
	}

	if(retVal == LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - receive enabled - hH(%d)",pHifInst->hH);

		pHifInst->bRcvEnabled = LSA_TRUE;
	}

	return retVal;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 *        - LSA_RET_ERR_SEQUENCE
 *        - retval of stopped timer
 *        
 */
LSA_UINT16 hif_disable_receive(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 retVal = LSA_RET_OK;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_disable_receive(+) -  pHifInst(0x%x)", pHifInst);

	if(pHifInst->bRcvEnabled == LSA_FALSE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - pHifInst->bRcvEnabled (0x%x) is not enabled, hH(0x%x) ",pHifInst->bRcvEnabled, pHifInst->hH);
		return LSA_RET_ERR_SEQUENCE;
	}

	/* deactivate Timer Poll */
	if( pHifInst->if_pars.TmOptions.bTimerPoll == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
            #if !defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
			/* LD Device */
			g_pHifData->LD.PollTimer.PollUserCnt--;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer ld poll cnt-- - hH(%d)",pHifInst->hH);

			if(g_pHifData->LD.PollTimer.PollUserCnt == 0)
			{
				HIF_STOP_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer);

				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer ld stop - hH(%d)",pHifInst->hH);
			}

			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - HIF Timer for LD couldn't stopped, hH(0x%x)",pHifInst->hH);
				return retVal;
			}
            #endif
		}
		else /* HD Device */
		{
            #if !defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
			/* deactivate Timer Poll */
			g_pHifData->HD.PollTimer.PollUserCnt--;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer hd poll cnt-- - hH(%d)",pHifInst->hH);

			if(g_pHifData->HD.PollTimer.PollUserCnt == 0)
			{
				HIF_STOP_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer);

				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer hd stop - hH(%d)",pHifInst->hH);
			}

			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - HIF Timer for HD couldn't stopped, hH(0x%x)",pHifInst->hH);
				return retVal;
			}
            #endif
		}
	}

	/* disable Interrupts */
	if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
			/* LD Device */

			/* disable Interrupts */
			HIF_LD_DISABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
		else /* HD Device */
		{
			/* disable Interrupts */
			HIF_HD_DISABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
	}

	if(retVal == LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - receive disabled - hH(%d)",pHifInst->hH);

		pHifInst->bRcvEnabled = LSA_FALSE;
	}

	return retVal;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_send(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_RQB_PTR_TYPE pRQBTrigger;

	HIF_PROGRAM_TRACE_07(0, LSA_TRACE_LEVEL_NOTE, ">>> hif_send() hH(%d) hdId(%d) ifId(%d) pipeId(%d) opc(%d) comp_id(%d) pRQB(0x%x)",pPipe->pInstance->hH,pPipe->hd_id,pPipe->if_id,pPipe->pipe_id,HIF_RQB_GET_OPCODE(pRQB),pPipe->comp_id,pRQB);

    if(hif_is_upper_pipe(pPipe) == LSA_TRUE) /* Upper Pipe sending RQBs increases act open RQB count */
	{
        pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs++;
	}
	else /* Lower pipe returning RQB reduces act RQB count */
	{
		HIF_ASSERT(pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs > 0);
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs--;
	}

    if(pPipe->pInstance->Mode == HIF_TM_SHARED_MEM)
	{
		/* Always add send RQBs to Send Queue and then try to transmit them */
		hif_add_to_rqb_queue(&pPipe->pRQBRingList->pSndQueue, (HIF_RQB_PTR_TYPE)pRQB, pPipe->PipeIndex);

		hif_shm_send(pPipe->pInstance);
	}
	else /* pPipe->Mode == HIF_TM_SHORT */
	{
		/* backup/overwrite fields which should not be transported (handle, user_id) */
		pPipe->short_circuit_data_backup.hLower = HIF_RQB_GET_HANDLE(pRQB);
		HIF_RQB_SET_HANDLE(pRQB, 0xEE);

		/* => absolutely hif_enter critically! */

		/* add to target rcv list of target hif instance */
        hif_add_to_rqb_queue(&pPipe->pInstance->if_pars.pHifPartner->if_pars.SharedMem.RQBRingList.pRcvList, pRQB, pPipe->PipeIndex);

		if(HIF_IS_NOT_NULL(pPipe->pInstance->if_pars.pHifPartner->pRQBShort))
		{
			pRQBTrigger = pPipe->pInstance->if_pars.pHifPartner->pRQBShort;
			pPipe->pInstance->if_pars.pHifPartner->pRQBShort = LSA_NULL;

			/* Send trigger to other HIF device */
			pPipe->pInstance->func.DO_REQUEST_LOCAL(pRQBTrigger);
		}

		/* <= hif_exit */
	}
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<<< hif_send() ");
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 */
LSA_VOID   hif_process_rcv_list(HIF_INST_PTR_TYPE pHifInst)
{
	HIF_RQB_PTR_TYPE   pRQB;
    LSA_UINT16         PipeIndex;
    HIF_PIPE_PTR_TYPE  pPipe;

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_process_rcv_list() - hH(%d)", pHifInst->hH);

	hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pRcvList, &pRQB, &PipeIndex);

	while(HIF_IS_NOT_NULL(pRQB))
	{
        //Get Pipe
        pPipe = &pHifInst->Pipes[PipeIndex];

		hif_process_rcv_rqb(pPipe, pRQB);

		hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pRcvList, &pRQB, &PipeIndex);
	}
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_process_rcv_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_PROGRAM_TRACE_07(0, LSA_TRACE_LEVEL_NOTE, "hif_process_rcv_rqb() - receive hH(%d) hdId(%d) ifId(%d) pipeId(%d) opc(%d) comp_id(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, HIF_RQB_GET_OPCODE(pRQB), pPipe->comp_id,pRQB);

	if(hif_is_upper_pipe(pPipe) == LSA_TRUE) /* Upper Pipe receive reduces act open RQB count */
	{
		HIF_ASSERT(pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs > 0);
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs--;
	}
	else /* Lower Pipe receive increases act open RQB count */
	{
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs++;
	}

	if(pPipe->pipe_id == HIF_GLOB_ADMIN)
	{
		/* HIF Admin / Sys requests */
		if(HIF_RQB_GET_COMP_ID(pRQB) == LSA_COMP_ID_HIF) /* HIF Internal Admin Request */
		{
			if(hif_is_upper_device(pPipe->pInstance))
			{
				hif_send_admin_request_done(pPipe->pInstance, pRQB);
			}
			else
			{
				hif_receive_admin_request(pPipe->pInstance, pRQB);
			}
		}
		else /* Sys Request using HIF Admin Pipe */
		{
			if(hif_is_upper_device(pPipe->pInstance))
			{
				hif_upper_sys_request_done(pPipe, pRQB);
			}
			else
			{
				hif_lower_sys_request(pPipe, pRQB);
			}
		}
	}
	else
	{
		/* All other requests */

		if(pPipe->State == HIF_PIPE_STATE_CLOSED)
		{
			hif_lower_open_channel(pPipe, pRQB);
		}
		else if(hif_is_upper_pipe(pPipe) == LSA_TRUE)
		{
			/* Recover RQB in case of short curcuit mode */
			if(pPipe->pInstance->Mode == HIF_TM_SHORT)
			{
				HIF_RQB_SET_HANDLE(pRQB, pPipe->short_circuit_data_backup.hLower);
			}

			if(pPipe->State == HIF_PIPE_STATE_OPENING_UPPER)
			{
				hif_upper_open_channel_done(pPipe, pRQB);
			}
			else
			{
				HIF_ASSERT(pPipe->func.pCompFuncs != LSA_NULL);
				HIF_ASSERT(pPipe->func.pCompFuncs->IsCloseChannelRqb != LSA_NULL);
				if( pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE )
				{
					/* close channel request returns from lower  */
					hif_upper_close_channel_done(pPipe, pRQB);
				}
				else
				{
					/* request returns from lower => forward to user */
					hif_upper_pipe_request_done(pPipe, pRQB);
				}
			}
		}
		else
		{
			HIF_ASSERT(pPipe->func.pCompFuncs != LSA_NULL);
			HIF_ASSERT(pPipe->func.pCompFuncs->IsCloseChannelRqb != LSA_NULL);
			if( pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE )
			{
				hif_lower_close_channel(pPipe, pRQB);
			}
			else
			{
				/* receiving request from upper => forward to component */
				hif_lower_pipe_request(pPipe, pRQB);
			}
		}
	}	// endif pipe is HIF_GLOB_ADMIN
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @param pPars
 */
LSA_UINT16 hif_setup_transfer_mode(HIF_INST_PTR_TYPE pHifInst, HIF_GET_PAR_PTR_TYPE pPars)
{
	LSA_UINT16 i;
	LSA_VERSION_TYPE version;
	HIF_SHM_HEADER_TAG shmHeaderRemote;
	HIF_SHM_HEADER_TAG *pShmHeaderLocal = LSA_NULL;
	LSA_UINT8 lVersion[26];
	LSA_UINT32 uIfIdent;
	HIF_RING_BUFFER_PTR_TYPE pRcvRing;
	HIF_RING_BUFFER_PTR_TYPE pSndRing;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_transfer_mode() - hH(%d), pPars(0x%x)",pHifInst->hH, pPars);

	/* Basic Params Check */

    //!!!AH!!! HIF_TM_MIXED und HIF_TM_SHARED_MEM_NO_SER entfernen!

	if(pPars->Mode == HIF_TM_MIXED)
	{
		for(i=0; i<HIF_MAX_PIPES_INST; i++) /* In mixed mode each Pipe has to have a specific transfer mode */
		{
			if( (pPars->if_pars.Pipes[i].Mode != HIF_TM_SHORT) &&
				(pPars->if_pars.Pipes[i].Mode != HIF_TM_SHARED_MEM) )
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_setup_transfer_mode(-) unknown pipe transfer mode"); 
				return LSA_RET_ERR_RESOURCE;
			}
		}
	}

	/* Init TransferMode Options */
	/*
		bSerialization
			LSA_TRUE  = Use RQB De- Serialization on shared memory receive/send
		    LSA_FALSE = Only shared memory signalling is used. The RQB Address and it's Buffers need to be accessible on the receivers side
		bTimerPoll
			LSA_TRUE  = Use addtional polling for shared memory send (for queued sends) / receive. This is the fallback in case of missed interrupts.
		    LSA_FALSE = No polling => no fallback!
		bShmHandling
			LSA_TRUE  = Use shared nemory handling
		    LSA_FALSE = No shared memory handling
		bShmSndWaitAck
			LSA_TRUE  = Sends an interrupt (Acknowledge) from the receiver to the sender side when an request is received => Acknowledge. 
			            The sender doesn't send any further requests until it gets the acknowledge. On receiving the Ack the sender tries to process it's send queues.
		    LSA_FALSE = No Ack signalling => in case a send Req is queued you will need the bTimerPoll to get it processed. 
	*/

	pHifInst->Mode = pPars->Mode;
	if (pPars->Mode != HIF_TM_SHORT) // for short driver get_params are not called -> if_pars can't be used in short mode! 
	{
		pHifInst->if_pars.TmOptions.bTimerPoll = pPars->if_pars.SharedMem.bUseTimerPoll;
		pHifInst->if_pars.TmOptions.SendIrqMode = pPars->if_pars.SharedMem.SendIrqMode;
		pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq = pPars->if_pars.SharedMem.uNsWaitBeforeSendIrq;
	
		if( (pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_NO_WAIT) &&
			(pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_READ_BACK) &&    
			(pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_TIMEOUT) )
		{
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unexpected SendIrqMode=%d", pHifInst->if_pars.TmOptions.SendIrqMode);
			return LSA_RET_ERR_PARAM;
		}
	}

	switch(pHifInst->Mode)
	{
		case HIF_TM_MIXED:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_TRUE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		case HIF_TM_SHARED_MEM:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_TRUE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		case HIF_TM_SHORT:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bTimerPoll                 = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_FALSE;
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_FALSE;
            #endif
			break;
		case HIF_TM_SHARED_MEM_NO_SER:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		default:
            {
			    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unknown transfer mode"); 
			    return LSA_RET_ERR_RESOURCE;
            }
			// break; --> warning #111-D: statement is unreachable
	}

	#if (HIF_CFG_COMPILE_SERIALIZATION == 0)
	if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
	{
		HIF_FATAL(1); /* misconfiguration => serialization is not compiled but should be used */
	}
	#endif

	/* Copy / Init basic input pars */
	pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase  = LSA_NULL;
	pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = LSA_NULL;

	if( (pHifInst->Mode == HIF_TM_MIXED) ||
		(pHifInst->Mode == HIF_TM_SHARED_MEM) ||
		(pHifInst->Mode == HIF_TM_SHARED_MEM_NO_SER) )
	{
		/* Initialize parameters for local and remote shared memory */
		pHifInst->if_pars.SharedMem.ShmLocal.pBase       = pPars->if_pars.SharedMem.Local.Base;
		pHifInst->if_pars.SharedMem.ShmLocal.uSize       = pPars->if_pars.SharedMem.Local.Size;
		pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader  = (HIF_SHM_HEADER_TAG*)pHifInst->if_pars.SharedMem.ShmLocal.pBase;
		pHifInst->if_pars.SharedMem.ShmRemote.pBase      = pPars->if_pars.SharedMem.Remote.Base;
		pHifInst->if_pars.SharedMem.ShmRemote.uSize      = pPars->if_pars.SharedMem.Remote.Size;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = (HIF_SHM_HEADER_TAG*)pHifInst->if_pars.SharedMem.ShmRemote.pBase;
	}
	else
	{
		/* Set parameters for shared memory to 0 */
		pHifInst->if_pars.SharedMem.ShmLocal.pBase       = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmLocal.uSize       = 0;
		pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader  = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmRemote.pBase      = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmRemote.uSize      = 0;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = LSA_NULL;
	}

	/* Set Pipe Modes */
	if(pPars->Mode == HIF_TM_SHORT)
	{
		pHifInst->Mode = HIF_TM_SHORT;
	}
	else /* HIF_TM_SHARED_MEM, HIF_TM_SHARED_MEM_NO_SER Modes are not divided on pipe level */
	{
		pHifInst->Mode = HIF_TM_SHARED_MEM;
	}

	/* Init Short Mode */
	if( (pHifInst->Mode == HIF_TM_MIXED) ||
	    (pHifInst->Mode == HIF_TM_SHORT) )
	{
		if(hif_is_upper_device(pHifInst) == LSA_TRUE)
		{
			hif_get_instance(&pHifInst->if_pars.pHifPartner, pPars->if_pars.hif_lower_device);
			if(HIF_IS_NULL(pHifInst->if_pars.pHifPartner))
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) could not find hif short partner");
				return LSA_RET_ERR_RESOURCE;
			}
			else /* hif partner OK */
			{
				/* absolutely HIF_ENTER critically! */
                //!!!AH evtl. mutex???
				if(hif_is_ld_device(pHifInst) == LSA_TRUE)
				{
					if(pHifInst->if_pars.pHifPartner->Type != HIF_DEV_TYPE_LD_LOWER)
					{
						HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unmatching types");
						return LSA_RET_ERR_PARAM;
					}
				}
				else
				{
					if(pHifInst->if_pars.pHifPartner->Type != HIF_DEV_TYPE_HD_LOWER)
					{
                        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unmatching types");
						return LSA_RET_ERR_PARAM;
					}
				}

				pHifInst->if_pars.pHifPartner->if_pars.pHifPartner = pHifInst;
			}
		}
	}

	/* Init Shared Mem Mode */
	if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
	{
		/*
			In shared memory mode HIF uses 2 shared memory buffers to eliminate slow pci(e) reads:
			1) Local  => this memory should be located in the local system and is almost read only because it's 
			             written by the other HIF instance, exceptions to read only are the trigger vars
			2) Remote => this memory is located at the target system which is for example connected by PCI(e).
			             The remote memory is write only!

			Structure of shared mem:
			Upper => ( Header | Ring List | RQB Buffer )
			Lower => ( Header | [otpional Pipe Header] | Ring List | RQB Buffer )
		*/

		
		/* Check if shared mem base ptr are set */
		if( HIF_IS_NULL(pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader) || HIF_IS_NULL(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader) )
		{
            //AH!!! Trace!!!
			return LSA_RET_ERR_PARAM;
		}

		/* Check Min Shared Mem size */
		if(pHifInst->if_pars.SharedMem.ShmRemote.uSize < (sizeof(HIF_SHM_HEADER_TAG) + (sizeof(HIF_RING_BUFFER_TYPE))))
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) uSize (remote) (0x%x) is invalid", pHifInst->if_pars.SharedMem.ShmRemote.uSize);
			return LSA_RET_ERR_RESOURCE;
        }

		if(pHifInst->if_pars.SharedMem.ShmLocal.uSize < (sizeof(HIF_SHM_HEADER_TAG) + (sizeof(HIF_RING_BUFFER_TYPE))))
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) uSize (local) (0x%x) is invalid", pHifInst->if_pars.SharedMem.ShmLocal.uSize);
            return LSA_RET_ERR_RESOURCE;
        }

		/* Get PNIO Version */
		pnio_version(sizeof(LSA_VERSION_TYPE), &version);
		lVersion[0]  = (LSA_UINT8)(version.lsa_component_id >> 8);
		lVersion[1]  = (LSA_UINT8)(version.lsa_component_id & 0xff);
		lVersion[2]  = version.lsa_prefix[0];
		lVersion[3]  = version.lsa_prefix[1];
		lVersion[4]  = version.lsa_prefix[2];
		lVersion[5]  = version.lsa_prefix[3];
		lVersion[6]  = version.lsa_prefix[4];
		lVersion[7]  = version.lsa_prefix[5];
		lVersion[8]  = version.lsa_prefix[6];
		lVersion[9]  = version.lsa_prefix[7];
		lVersion[10] = version.lsa_prefix[8];
		lVersion[11] = version.lsa_prefix[9];
		lVersion[12] = version.lsa_prefix[10];
		lVersion[13] = version.lsa_prefix[11];
		lVersion[14] = version.lsa_prefix[12];
		lVersion[15] = version.lsa_prefix[13];
		lVersion[16] = version.lsa_prefix[14];
		lVersion[17] = version.kind;
		lVersion[18] = version.version;
		lVersion[19] = version.distribution;
		lVersion[20] = version.fix;
		lVersion[21] = version.hotfix;
		lVersion[22] = version.project_number;
		lVersion[23] = version.increment;
		lVersion[24] = version.integration_counter;
		lVersion[25] = version.gen_counter;

		/* Clear Own Shared Mem Header */
		HIF_MEMSET((LSA_VOID*)&shmHeaderRemote, 0, sizeof(HIF_SHM_HEADER_TAG));
		
		/* Init quick reference to local shared mem header */
		/* Be careful => on an upper device the local shared mem header already has valid information */
		/*               on an lower device the local shared mem header is not initialised            */
		pShmHeaderLocal  = pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader;

		/* Setup trigger vars which will be used in timer poll mechanism */
		pHifInst->if_pars.SharedMem.pSndRQ                  = &(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.uRqbRQ);
		pHifInst->if_pars.SharedMem.pRcvRQ                  = &pShmHeaderLocal->tm_control.uRqbRQ;
        
        #if defined (HIF_CFG_USE_SND_WAIT_ACK)
        pHifInst->if_pars.SharedMem.pSndRQAck               = &pShmHeaderLocal->tm_control.uRqbRQ_Ack;
		pHifInst->if_pars.SharedMem.pRcvRQAck               = &(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.uRqbRQ_Ack);
        #endif

		if(hif_is_upper_device(pHifInst) == LSA_TRUE)
		{
			/* Check Size of local shm header */
			if(pShmHeaderLocal->common.uHeaderLen != sizeof(HIF_SHM_HEADER_TAG))
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.uHeaderLen (0x%x) is invalid! Nedded uHeaderLen (0x%x)",pShmHeaderLocal->common.uHeaderLen, sizeof(HIF_SHM_HEADER_TAG));
				return LSA_RET_ERR_PARAM;
            }
			/* Check State */
			if(pShmHeaderLocal->common.State != HIF_SHM_STATE_DEVICE_OPENED)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.State (0x%x) is invalid! Nedded State (0x%x)",pShmHeaderLocal->common.State, HIF_SHM_STATE_DEVICE_OPENED);
				return LSA_RET_ERR_SEQUENCE;
            }

			/* Check Role */
			if(pShmHeaderLocal->common.IsUpper != 0)
            {
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.IsUpper is != 0");
				return LSA_RET_ERR_PARAM;
            }

			/* Check Shared Mem Header */
			uIfIdent = hif_get_if_ident(pHifInst); /* the Interface Ident contains configuration data which must match between upper and lower HIF instance */
			if(pShmHeaderLocal->dev_specific.lower.uIfIdent != uIfIdent)
			{
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - Interface Ident doesn't match (0x%x/0x%x), check for ld/hd and HIF_USE compiler switches",uIfIdent,pShmHeaderLocal->dev_specific.lower.uIfIdent);
				HIF_FATAL(0);
			}

			if(pShmHeaderLocal->dev_specific.lower.uPnioVersionLen != 26)
            {
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->dev_specific.lower.uPnioVersionLen != 26");
				return LSA_RET_ERR_RESOURCE;		
            }
			
			/* Compare PNIO Version */
			if(HIF_MEMCMP(lVersion, pShmHeaderLocal->dev_specific.lower.uaPnioVersion, 26) > 0)
			{
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - Wrong PNIO Version");
				return LSA_RET_ERR_PARAM;
			}

			/* Create Own Shared Mem Header */
			shmHeaderRemote.common.uHeaderLen     = sizeof(HIF_SHM_HEADER_TAG);
			shmHeaderRemote.common.IsUpper        = 1;
			shmHeaderRemote.common.uRingBufferLen = sizeof(HIF_RING_BUFFER_TYPE);
			shmHeaderRemote.common.uRQBBufferLen  = pHifInst->if_pars.SharedMem.ShmRemote.uSize - shmHeaderRemote.common.uHeaderLen - shmHeaderRemote.common.uRingBufferLen;

			/* Check: Ring buffer lengths should be identical */
			if(shmHeaderRemote.common.uRingBufferLen != pShmHeaderLocal->common.uRingBufferLen)
			{
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - shmHeaderRemote.common.uRingBufferLen (0x%x) and pShmHeaderLocal->common.uRingBufferLen (0x%x) mismatch!",shmHeaderRemote.common.uRingBufferLen, pShmHeaderLocal->common.uRingBufferLen);
				return LSA_RET_ERR_PARAM;
			}

			/* Find RQB data base address for mem pool creation  */
			/* Shared mem pools are only needed in case of serialization */
			if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
			{
				if(pHifInst->Mode == HIF_TM_MIXED)
				{
					pHifInst->if_pars.SharedMem.ShmRemote.hPool = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.hPool; /* Use lower pool as there is only 1 available in mixed mode */
					pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
				}
				else
				{
					pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.common.uRingBufferLen;                   
                    HIF_CREATE_SER_POOL((LSA_VOID*)pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, shmHeaderRemote.common.uRQBBufferLen, pHifInst->hSysDev, &pHifInst->if_pars.SharedMem.ShmRemote.hPool);
                    pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmLocal.pBase + pShmHeaderLocal->common.uHeaderLen + pShmHeaderLocal->common.uRingBufferLen + pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen;
				}
			}

			shmHeaderRemote.tm_control.uRqbRQ_Ack = 1; /* Set initial acknowledge */

			/* Lock SHM interface for this HIF Inst */
			shmHeaderRemote.common.State = HIF_SHM_STATE_DEVICE_OPENED;
		}
		else
		{
			/* Create Shared Mem Header */
			shmHeaderRemote.common.uHeaderLen                   = sizeof(HIF_SHM_HEADER_TAG);
			shmHeaderRemote.common.uRingBufferLen               = sizeof(HIF_RING_BUFFER_TYPE);
			shmHeaderRemote.common.IsUpper                      = 0;
			shmHeaderRemote.dev_specific.lower.uIfIdent         = hif_get_if_ident(pHifInst);
			shmHeaderRemote.dev_specific.lower.uPnioVersionLen  = 26;
			shmHeaderRemote.dev_specific.lower.uPipeHeaderLen   = 0;

			/* Store local pnio version to shared mem */
			HIF_MEMCPY(shmHeaderRemote.dev_specific.lower.uaPnioVersion, lVersion, 26);

			/* In case of serialization we need to create mem pools in the shared memory */
			if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
			{
				shmHeaderRemote.common.uRQBBufferLen = (pHifInst->if_pars.SharedMem.ShmRemote.uSize - (shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen + shmHeaderRemote.common.uRingBufferLen));

				/* Find RQB data base address for mem pool creation  */
				pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen + shmHeaderRemote.common.uRingBufferLen;

				HIF_CREATE_SER_POOL((LSA_VOID*)pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, shmHeaderRemote.common.uRQBBufferLen, pHifInst->hSysDev, &pHifInst->if_pars.SharedMem.ShmRemote.hPool);

				if(pHifInst->Mode == HIF_TM_MIXED)
				{
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
				}
				else
				{
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmLocal.pBase + sizeof(HIF_SHM_HEADER_TAG)/* (LaM) pShmHeaderLocal->common.uHeaderLen is not available here*/ + sizeof(HIF_RING_BUFFER_TYPE) /* pShmHeaderLocal->common.uRingBufferLen not available here */;
				}
			}

			/* Enable SHM interface */
			shmHeaderRemote.common.State = HIF_SHM_STATE_DEVICE_OPENED;
		}

        /* Calculate ring */
        HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList, 0, sizeof(HIF_SHM_RQB_RINGLIST_TAG_TYPE));

		if(pHifInst->Mode == HIF_TM_SHARED_MEM)
		{
			if(hif_is_upper_device(pHifInst) == LSA_TRUE)
			{
				pRcvRing = (HIF_RING_BUFFER_PTR_TYPE)(pHifInst->if_pars.SharedMem.ShmLocal.pBase + pShmHeaderLocal->common.uHeaderLen + pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen );
				pSndRing = (HIF_RING_BUFFER_PTR_TYPE)(pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen );
			}
			else
			{
				pRcvRing = (HIF_RING_BUFFER_PTR_TYPE)(pHifInst->if_pars.SharedMem.ShmLocal.pBase + sizeof(HIF_SHM_HEADER_TAG)/* (LaM) pShmHeaderLocal->common.uHeaderLen is not available here*/);
				pSndRing = (HIF_RING_BUFFER_PTR_TYPE)(pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen );

				/* Ring structures are initialized by lower device because the lower device is first available */
				HIF_MEMSET(pRcvRing, 0, sizeof(HIF_RING_BUFFER_TYPE)); /* Clear own ring structure in shared memory */
				HIF_MEMSET(pSndRing, 0, sizeof(HIF_RING_BUFFER_TYPE)); /* Clear remote ring structure in shared memory */	
			}

			/* Create local structures simplifying the access to the rings */
			HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList.SendRing, 0, sizeof(HIF_PIPE_SHM_SEND_RING_TYPE));
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pRingWriteOnly     = pSndRing->rqb;
            pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pPipeIndexWriteOnly = pSndRing->PipeIndex;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly    = &pSndRing->wi;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pRiShmReadOnly     = &pRcvRing->ri;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWriteOnlyRing     = pSndRing;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.wi = *pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly; /* (LaM) not nice but needed */
				
			HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, 0, sizeof(HIF_PIPE_SHM_RECEIVE_RING_TYPE));
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRingReadOnly   = pRcvRing->rqb;
            pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pPipeIndexReadOnly = pRcvRing->PipeIndex;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly = &pSndRing->ri;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pWiShmReadOnly  = &pRcvRing->wi;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pReadOnlyRing   = pRcvRing;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.ri = *pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly; /* (LaM) not nice but needed */

		    /* Map one ringlist to all channels of it's own interface */
		    for(i=0; i<HIF_MAX_PIPES_INST; i++)
		    {
                pHifInst->Pipes[i].pRQBRingList = &pHifInst->if_pars.SharedMem.RQBRingList;
            }
		}

		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_setup_transfer_mode() Local  Shm: hH(%d) ShmBase(0x%8x) ShmLen(0x%8x) RqbBufferBase(0x%8x(calculated))",pHifInst->hH, pShmHeaderLocal, pHifInst->if_pars.SharedMem.ShmLocal.uSize, pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase);
		HIF_PROGRAM_TRACE_10(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_setup_transfer_mode() Remote Shm: hH(%d) ShmBase(0x%8x) ShmLen(0x%8x) RqbBufferBase(0x%8x) isUpper(%d) HeaderLen(0x%8x) RingLen(0x%8x) PipeHeaderLen(0x%8x) RqbBufferLen(0x%8x) PoolHandle(0x%8x) ",pHifInst->hH, pHifInst->if_pars.SharedMem.ShmRemote.pBase, pHifInst->if_pars.SharedMem.ShmRemote.uSize, pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, pShmHeaderLocal->common.IsUpper, pShmHeaderLocal->common.uHeaderLen, pShmHeaderLocal->common.uRingBufferLen, pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen, pShmHeaderLocal->common.uRQBBufferLen, pHifInst->if_pars.SharedMem.ShmRemote.hPool);

		*pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = shmHeaderRemote; /* Write Shm Header to shared memory */
		#if defined (HIF_CFG_USE_SND_WAIT_ACK)
        *pHifInst->if_pars.SharedMem.pSndRQAck = 1; /* Set initial send acknowledge (myself) so that the processing can start */
        #endif
	}

	return LSA_RET_OK;
}


/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_UINT16 hif_finish_transfer_mode(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 uRetVal=LSA_RET_OK;

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_finish_transfer_mode() - hH(%d)",pHifInst->hH);

	if(pHifInst->Mode == HIF_TM_SHARED_MEM)
	{
        hif_shm_ring_buffer_send_clean_all_elements(pHifInst); /* clean the sent rqb(s) which haven't been freed so far */
    }

	if(HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader))
	{
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->common.State = HIF_SHM_STATE_DEVICE_NOT_READY;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = LSA_NULL;
	}

	if( ( (pHifInst->Mode == HIF_TM_MIXED) &&               /* In Mixed mode the lower device kills the pool */
	      (hif_is_upper_device(pHifInst) == LSA_FALSE) ) ||
		  (pHifInst->Mode == HIF_TM_SHARED_MEM) )           /* In SHM mode both have to kill their pool */
	{
		/* Free the RQB Pool - Only one is currently supported */
		HIF_DELETE_SER_POOL(&uRetVal, pHifInst->hSysDev, pHifInst->if_pars.SharedMem.ShmRemote.hPool);
	}

	return uRetVal;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 * @return
 */
LSA_UINT16 hif_shm_send_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_VOID_PTR_TYPE pShmSerRqb = LSA_NULL;
	LSA_UINT32 shmSerRqbOffset = 0;
	LSA_UINT32 shmSerRqbLen = 0;
	LSA_UINT32 is_full = 0;
	LSA_UINT16 ret_val;

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	LSA_UINT64 start_count_ser, end_count_ser;
	#endif
	LSA_UINT64 end_count_total;
	LSA_UINT64 start_count_total = HIF_GET_NS_TICKS();
	#endif
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,">>> hif_shm_send_rqb() ");

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_cm(+) -  pPipe(0x%x), pRQB(0x%x)", pPipe, pRQB);

	hif_shm_ring_buffer_send_clean_element(pPipe);

	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	if(pPipe->pInstance->if_pars.TmOptions.bSerialization == LSA_TRUE)
	{
    	LSA_VOID_PTR_TYPE pShmSerRqbReal = LSA_NULL;

		/* copy the RQB to the shared memory and return a pointer (rqb_shm) */
		HIF_ASSERT(pPipe->func.Serialize != LSA_NULL);
		
		//Measure the serialzation performance
		#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
		start_count_ser = HIF_GET_NS_TICKS();
		ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		end_count_ser = HIF_GET_NS_TICKS();
		HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_send_rqb(): Serialization of the RQB took %d ns",(end_count_ser - start_count_ser));
		#else
		ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		#endif
		
		pShmSerRqbReal = pShmSerRqb;
		HIF_SHM_DMA_COPY(&pShmSerRqbReal, shmSerRqbLen, pPipe->pInstance->hSysDev);

		/* add rqb to the ring buffer (only rqb offset to the beginning of shared memory is stored in the ring buffer */
		shmSerRqbOffset = (LSA_UINT32)((LSA_UINT8*)pShmSerRqbReal - (LSA_UINT8*)pPipe->pInstance->if_pars.SharedMem.ShmRemote.pRqbBufferBase);
	}
	else
	#endif
	{
		shmSerRqbOffset = (LSA_UINT32)pRQB;
		pShmSerRqb = pRQB;
		ret_val = LSA_RET_OK;
	}

	if(ret_val == LSA_RET_OK) /* Serialization OK */
	{
		HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_send_rqb() rqb(%d) hH(%d) pipe(%d) comp_id(%d) ring(0x%x) ring_index(%d) shm_buff(0x%08x) shm_buff_o(0x%08x) len(%d)",pRQB, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id, pPipe->pRQBRingList->SendRing.pRingWriteOnly, pPipe->pRQBRingList->SendRing.wi, pShmSerRqb, shmSerRqbOffset, shmSerRqbLen);

		is_full = hif_shm_ring_buffer_send_enque(&pPipe->pRQBRingList->SendRing, shmSerRqbOffset, (LSA_UINT32)pShmSerRqb, pPipe->PipeIndex);
		if (is_full) /* can't be full since we checked this above */
		{
			HIF_FATAL(0);
		}
	} 
    else if(ret_val == LSA_RET_ERR_RESOURCE) /* Not enough available shared mem */
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_shm_send_rqb(-) Not enough available shared mem");	
        return LSA_RET_ERR_RESOURCE;
	}
	else /* Serialization failed for unknown reason */
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_shm_send_rqb(-) Serialization failed for unknown reason");
		HIF_FATAL(0);
	}

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_total = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_send_rqb(): The total function duration is %d ns",(end_count_total - start_count_total));
	#else
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,"<<< hif_shm_send_rqb() ");
	#endif

	return LSA_RET_OK;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_VOID   hif_shm_send(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_BOOL           bNewRqbsSent = LSA_FALSE;
	HIF_RQB_PTR_TYPE   pSndRqb       = LSA_NULL;
	HIF_PIPE_PTR_TYPE  pPipe         = LSA_NULL;
	LSA_UINT16         retVal;
    LSA_UINT16         PipeIndex;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send(+) -  pHifInst(0x%x)", pHifInst);

    #if defined (HIF_CFG_USE_SND_WAIT_ACK)
	if( pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE ) /* Use ack wait */
	{
		if(*pHifInst->if_pars.SharedMem.pSndRQAck != 1)
		{
			return; /* need to wait for Acknowledge */
		}
	}
    #endif

    while ((!hif_shm_ring_buffer_send_is_full(&pHifInst->if_pars.SharedMem.RQBRingList.SendRing)) && (HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue))) // is there an RQB in the local send queue?
	{
		//HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_send_rqbs_from_send_queue() send queued rqb hH(%d) pipe(%d) comp_id(%d)", pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id);

		/* get the first rqb in the local send queue */
		hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue, &pSndRqb, &PipeIndex);
        
        //Get Pipe        
        pPipe = &pHifInst->Pipes[PipeIndex];

		retVal = hif_shm_send_rqb (pPipe, pSndRqb);

		if(retVal == LSA_RET_OK)
		{
			bNewRqbsSent = LSA_TRUE;
		}
		else /* Send failed, readd to send queue and don't try to send any further RQBs in this cycle */
		{
			HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_send() failed - out of mem - add to shared mem send queue - rqb(0x%x) hH(%d) pipe(%d) comp_id(%d)",pSndRqb, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id);

            hif_add_to_rqb_queue_as_first(&pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue, pSndRqb, pPipe->PipeIndex);

			break;
		}
	}

    if ( pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue != LSA_NULL )
    {
        /* When we got an ack, it is most likely that there are items in the ring lists (shared memory) which can be deleted. */
        /* For optimisation we only clean the shared memory when we have new RQBs for sending */
        hif_shm_ring_buffer_send_clean_all_elements(pHifInst);
    }
	
	/* send queue not empty? */
	if(HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue))
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_send() send queue (0x%x) not empty ", pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue);
	}

	if(bNewRqbsSent == LSA_TRUE)
	{
        /* Update wi in shared memory */
		*pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly = pHifInst->if_pars.SharedMem.RQBRingList.SendRing.wi;
        
        #if defined (HIF_CFG_USE_SND_WAIT_ACK)
		if( pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE ) /* Use ack wait */
		{
			*pHifInst->if_pars.SharedMem.pSndRQAck = 0; /* reset Ack */
		}
        #endif
		
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() PCIWrite pHifInst->if_pars.SharedMem.pSndRQ = 1");
		*pHifInst->if_pars.SharedMem.pSndRQ = 1;
		
		if (pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_NO_WAIT)
		{
		    // no wait before irq -> nothing to do
		}
		else if(pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_READ_BACK)
		{
		    // read back last pci transfer before sending irq, to prevent that irq comes before data
	        volatile LSA_UINT8 tmp;
	        tmp = *pHifInst->if_pars.SharedMem.pSndRQ;
	    
	        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() PCIRead pHifInst->if_pars.SharedMem.pSndRQ = %d", tmp);
	    }
		else if (pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_TIMEOUT)
	    {
	        HIF_WAIT_NS(pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq);
	        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() SEND_IRQ after %d ns timeout", pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq);
	    }
		else
		{
		    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_send() pHifInst->if_pars.TmOptions.SendIrqMode not set correctly!");
		    HIF_FATAL(0);
		}
		

		pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
	}
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_VOID   hif_shm_receive(HIF_INST_PTR_TYPE pHifInst)
{
	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	LSA_UINT64 start_count_deser, end_count_deser;
	#endif
	LSA_UINT64 end_count_total;
	LSA_UINT64 start_count_total = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, ">>> hif_shm_receive() pHifInst (0x%x)", pHifInst);
    
    if(*pHifInst->if_pars.SharedMem.pRcvRQ != 0)
    {
        LSA_VOID_PTR_TYPE        pRcvRqb         = LSA_NULL;
        HIF_PIPE_PTR_TYPE        pPipe           = LSA_NULL;
        LSA_VOID_PTR_TYPE        pShmSerRqb      = LSA_NULL;
        LSA_UINT32               shmSerRqbOffset = 0; 
        LSA_UINT32               is_empty        = 0;
        LSA_UINT16               PipeIndex       = 0xFFFF;

        *pHifInst->if_pars.SharedMem.pRcvRQ = 0; /* Acknowledge Request on Trigger Var */

        /* 1. Get next RQB in shared mem receive ring */
        is_empty = hif_shm_ring_buffer_receive_deque_start(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, &shmSerRqbOffset, &PipeIndex);
        while (!is_empty) /* received new RQB */
        {
            pPipe = &pHifInst->Pipes[PipeIndex];

            /* 2. get pointer to serialized RQB */
            #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
            if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
            {
                LSA_UINT16  ret_val;

                pShmSerRqb = (LSA_VOID*)((LSA_UINT32)shmSerRqbOffset + pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase);

                /* deserialize */
                HIF_ASSERT(pPipe->func.Deserialize != LSA_NULL);

                //Measure the deserialization performance
                #if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
                start_count_deser = HIF_GET_NS_TICKS();
                ret_val = pPipe->func.Deserialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pShmSerRqb, &pRcvRqb);
                end_count_deser = HIF_GET_NS_TICKS();
                HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_receive_queue(): Deserialization of the RQB took %d ns",(end_count_deser - start_count_deser));
                #else
                ret_val = pPipe->func.Deserialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pShmSerRqb, &pRcvRqb);
                #endif

                HIF_ASSERT(ret_val == HIF_OK);
            }
            else
            #else
            LSA_UNUSED_ARG(pHifInst);
            #endif
            {
                pRcvRqb = (LSA_VOID*)shmSerRqbOffset;
            }

            HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_receive() pRQB(0x%x) hH(%d) pipe(%d) comp_id(%d) opc(%d) ring(0x%x) new_ring_index(%d) shm_buff(0x%x) shm_buff_o(0x%x)",pRcvRqb, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id, HIF_RQB_GET_OPCODE((HIF_RQB_PTR_TYPE)pRcvRqb), pPipe->pRQBRingList->ReceiveRing.pRingReadOnly, pPipe->pRQBRingList->ReceiveRing.ri, pShmSerRqb, shmSerRqbOffset);

            hif_process_rcv_rqb(pPipe, (HIF_RQB_PTR_TYPE)pRcvRqb);

            is_empty = hif_shm_ring_buffer_receive_deque_start(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, &shmSerRqbOffset, &PipeIndex);

            if(is_empty)
            {
                /* update the receive index on the shared memory */
                /* This might cause the other processor to reuse / free the ring elements which are already read */
                *pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly = pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.ri;
            }
        }
        #if defined (HIF_CFG_USE_SND_WAIT_ACK)
        if(pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE)
        {
            *pHifInst->if_pars.SharedMem.pRcvRQAck = 1; /* Send optional receive acknowledge */

            pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
        }
        #else
        pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
        #endif

    }

	#if (defined HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_total = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_receive(): The total function duration is %d ns",(end_count_total - start_count_total));
	#else
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_shm_receive() pHifInst (0x%x)", pHifInst);
	#endif

}


/*------------------------------------------------------------------------------
//	HIF shared memory ring buffer
//----------------------------------------------------------------------------*/
/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @return
 */
LSA_VOID hif_shm_ring_buffer_send_clean_all_elements(HIF_INST_PTR_TYPE pHifInst)
{
	HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing = &pHifInst->if_pars.SharedMem.RQBRingList.SendRing;
	LSA_VOID_PTR_TYPE pShmSerRqb;
	LSA_UINT16 ret_val;
	LSA_UINT32 wi = pSndRing->wi;
	LSA_UINT32 ri = *pSndRing->pRiShmReadOnly;
    LSA_UINT32 ti = 0;


    if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
	{
        for(ti = 0; ti < HIF_SHM_RB_BUFFER_LENGTH; ti++)
	    {
            if (pSndRing->bRingElementUsed[ti] == LSA_TRUE)                
            {
                HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_NOTE,   "hif_shm_ring_buffer_send_clean_all_elements(SENDRINGLIST %d) - uRqbOffset:0x%x, PipeIndex:%d, uRqb:0x%x, bRingElementUsed:%d",
                                                                ti, pSndRing->pRingWriteOnly[ti], pSndRing->pPipeIndexWriteOnly[ti], pSndRing->Ring[ti], pSndRing->bRingElementUsed[ti]);
            }
	    }

		do
		{
			if (pSndRing->bRingElementUsed[wi] == LSA_TRUE)
			{
                LSA_UINT32 opcode;
                LSA_UINT32 comp_id;

				pShmSerRqb = (LSA_VOID*)(pSndRing->Ring[wi]);

                //Get opcode from RQB
                HIF_DESER_GET_OPCODE(pShmSerRqb, opcode); 

                //Get comp_id from RQB
                HIF_MEMCPY(&comp_id,(LSA_UINT32*)pShmSerRqb+2,sizeof(LSA_COMP_ID_TYPE));

				HIF_PROGRAM_TRACE_07(0, LSA_TRACE_LEVEL_NOTE, "hif_shm_ring_buffer_send_clean_all_elements() - ### Release HIF Mem, comp_id:0x%x, Opcode:0x%x, hH:%d ring:0x%x, ring_index:%d, shm_buff:0x%x, shm_buff_o:0x%x",comp_id, (opcode & 0xFF), pHifInst->hH, pSndRing->pRingWriteOnly, wi, pShmSerRqb, pSndRing->Ring[wi]);

				/* free the RQB in the shared memory - it has already been read by the receiver */
				HIF_FREE_SER_MEM (&ret_val, pShmSerRqb, pHifInst->hSysDev, pHifInst->if_pars.SharedMem.ShmRemote.hPool);

				HIF_ASSERT(ret_val == HIF_OK);

				/* mark the element in the ring-buffer as free */
				pSndRing->Ring[wi] = 0;
				pSndRing->bRingElementUsed[wi] = LSA_FALSE;
			}
			wi = (wi + 1) % HIF_SHM_RB_BUFFER_LENGTH;
		} while(wi!=ri);
	}
	else
	{
		do
		{
			if (pSndRing->bRingElementUsed[wi] == LSA_TRUE)
			{
				/* mark the element in the ring-buffer as free */
				pSndRing->Ring[wi] = 0;
				pSndRing->bRingElementUsed[wi] = LSA_FALSE;
			}
			wi = (wi + 1) % HIF_SHM_RB_BUFFER_LENGTH;
		} while(wi!=ri);
	}
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pPipe
 */
LSA_VOID hif_shm_ring_buffer_send_clean_element(HIF_PIPE_PTR_TYPE pPipe)
{
	HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing = &pPipe->pRQBRingList->SendRing;
	LSA_VOID_PTR_TYPE pShmSerRqb;
	LSA_UINT16 ret_val;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_clean_element(+) -  pPipe(0x%x)", pPipe);

	if (pSndRing->bRingElementUsed[pSndRing->wi] == LSA_TRUE)
	{
		if(pPipe->pInstance->if_pars.TmOptions.bSerialization == LSA_TRUE)
		{
            LSA_UINT32 opcode;

			pShmSerRqb = (LSA_VOID*)(pSndRing->Ring[pSndRing->wi]);

            HIF_DESER_GET_OPCODE(pShmSerRqb, opcode);

			HIF_PROGRAM_TRACE_08(0, LSA_TRACE_LEVEL_NOTE, "hif_shm_ring_buffer_send_clean_element(): ### Release HIF Mem: CompID:0x%x , Opcode:0x%x, hH:%d, pipe:%d, ring:0x%x, ring_index:%d, shm_buff:0x%x, shm_buff_o:0x%x", pPipe->comp_id, (opcode & 0xFF), pPipe->pInstance->hH, pPipe->pipe_id, pSndRing->pRingWriteOnly, pSndRing->wi, pShmSerRqb, pSndRing->Ring[pSndRing->wi]);

			/* free the RQB in the shared memory - it has already been read by the receiver */
			HIF_FREE_SER_MEM (&ret_val, pShmSerRqb, pPipe->pInstance->hSysDev, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool);
			HIF_ASSERT(ret_val == HIF_OK);
		}

		/* mark the element in the ring-buffer as free */
		pSndRing->Ring[pSndRing->wi] = 0;
		pSndRing->bRingElementUsed[pSndRing->wi] = LSA_FALSE;
	}
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pSndRing
 * @return
 *        - LSA_TRUE
 *        - LSA_FALSE
 */
LSA_BOOL hif_shm_ring_buffer_send_is_full(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing)
{
	/* This function checks if a send ring is full */
	/* Send ring is full when the write index would reach the read index */

	LSA_UINT32 ret;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_is_full(+) -  pSndRing(0x%x)", pSndRing);

	ret = (((pSndRing->wi + 1) % HIF_SHM_RB_BUFFER_LENGTH) == *pSndRing->pRiShmReadOnly);

	if (ret)
		return LSA_TRUE;
	else
		return LSA_FALSE;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pSndRing
 * @param uRqbOffset
 * @param uRqb
 * @param PipeIndex
 * @return
 *        - TRUE
 *        - FALSE
 */
LSA_BOOL hif_shm_ring_buffer_send_enque(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing, LSA_UINT32 uRqbOffset, LSA_UINT32 uRqb, LSA_UINT16 PipeIndex)
{
	/* This function enqueues a new Rqb to the send Ring */
	/* Additionally a local copy is stored which is used for cleanup */
	/* because send ring might be read only */

	LSA_BOOL is_full;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_enque(+) -  pSndRing(0x%x), uRqbOffset(0x%x), uRqb(0x%x), PipeIndex(0x%x)", pSndRing, uRqbOffset, uRqb, PipeIndex);

	is_full = hif_shm_ring_buffer_send_is_full(pSndRing);

	if(!is_full)
	{
		pSndRing->pRingWriteOnly[pSndRing->wi] = uRqbOffset;
        pSndRing->pPipeIndexWriteOnly[pSndRing->wi] = (LSA_UINT32)PipeIndex; 
        pSndRing->Ring[pSndRing->wi] = uRqb;
		pSndRing->bRingElementUsed[pSndRing->wi] = LSA_TRUE;
		pSndRing->wi = ((pSndRing->wi + 1) % HIF_SHM_RB_BUFFER_LENGTH);
		/* *pSndRing->pWiShmWriteOnly = pSndRing->wi; => moved to for optimisation */
        HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_NOTE, "hif_shm_ring_buffer_send_enque() - ### Enque HIF Mem PipeIndex:%d ring:0x%x, ring_index:%d, uRqb:0x%x, shm_buff_o:0x%x",
            PipeIndex, pSndRing->pRingWriteOnly, pSndRing->wi, pSndRing->Ring[pSndRing->wi], uRqbOffset);

	}
	return is_full;
}

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pRcvRing
 * @param puRqb
 * @param pPipeIndex
 * @return
 *        - is_empty
 */
LSA_BOOL hif_shm_ring_buffer_receive_deque_start(HIF_PIPE_SHM_RECEIVE_RING_PTR_TYPE pRcvRing, LSA_UINT32 *puRqb, LSA_UINT16 *pPipeIndex)
{
	/* This function dequeues a new RQB from the receive ring */
	/* The local receive index is increased, the shm receive isn't because the rqbs need to be processed first */  

	LSA_BOOL is_empty = LSA_FALSE;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_receive_deque_start(+) -  pRcvRing(0x%x), puRqb(0x%x), pPipeIndex(0x%x)", pRcvRing, puRqb, pPipeIndex);

	if(pRcvRing->ri == *pRcvRing->pWiShmReadOnly)
		is_empty = LSA_TRUE;

	if(!is_empty)
	{
		*puRqb = pRcvRing->pRingReadOnly[pRcvRing->ri];
        *pPipeIndex = (LSA_UINT16) pRcvRing->pPipeIndexReadOnly[pRcvRing->ri];
         
        HIF_ASSERT(*pPipeIndex < HIF_MAX_PIPES_INST);

		pRcvRing->ri = (pRcvRing->ri + 1) % HIF_SHM_RB_BUFFER_LENGTH;
	}
	return is_empty;
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
/*****************************************************************************/
