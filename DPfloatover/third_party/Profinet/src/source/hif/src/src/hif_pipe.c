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
/*  F i l e               &F: hif_pipe.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements HIF Pipe/Channel handling                                     */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID		5
#define LTRC_ACT_MODUL_ID	5

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 * \brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_upper_open_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	/* Open channel done? */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_open_channel_done(+) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENED_UPPER);

		pPipe->func.pCompFuncs->ReplaceOpenChannelLowerHandle(pRQB, pPipe->d.upper.ch_handle->hHifCompCh);
	}
	else
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_open_channel_done(-) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}
	
	hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));	
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_lower_close_channel(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel() - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

	/* remember the source of the RQB */
	hif_lower_save_return_path_to_rqb(pPipe, pRQB);

	/* close channel request received from upper */
	HIF_RQB_SET_HANDLE(pRQB, pPipe->d.lower.lower_orig_comp_ch_handle);

	/* request lower component */
	pPipe->pInstance->func.CLOSE_CHANNEL_LOWER(pRQB, pPipe->pInstance->hSysDev);
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_upper_close_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	/* Close channel done? */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_close_channel_done(+) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);

		pPipe->func.RELEASE_PATH_INFO(&retVal, pPipe->d.upper.ch_handle->sys_ptr, pPipe->d.upper.ch_handle->pDetails);

		hif_free_ch_handle(pPipe->d.upper.ch_handle);
		pPipe->d.upper.ch_handle = LSA_NULL;

		if(retVal != LSA_RET_OK)
		{
			HIF_FATAL(0);
		}
	}
	else
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_close_channel_done(-) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);
		
		hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));
	}
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_lower_open_channel(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel() - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

	/* remember the source of the RQB */
	hif_lower_save_return_path_to_rqb(pPipe, pRQB);

	/* Adjust Pipe state to opening */
	hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENING_LOWER);

	/* Only open_channel requests valid */	
	retVal = pPipe->func.pCompFuncs->ReplaceOpenChannelCbf(pRQB, hif_lower_request_done);
	if(retVal != LSA_RET_OK)
	{
		/* somethings wrong with the rqb -> should not happen here */
		HIF_FATAL(0);
	}

	pPipe->pInstance->func.OPEN_CHANNEL_LOWER(pRQB, pPipe->pInstance->hSysDev);
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_lower_close_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	/* Close channel RQB! */

	/* Close pipe if close channel was successfull */
	if(HIF_RQB_GET_RESPONSE((HIF_RQB_PTR_TYPE)pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel_done(+) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}
	else
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel_done(-) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);
	}

	/* return RQB to hif upper */
	hif_send(pPipe, pRQB);
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param pRQB
 */
LSA_VOID   hif_lower_open_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	/* Open Channel RQB! */

	/* Check if Pipe Open was successfull */
	if(HIF_RQB_GET_RESPONSE((HIF_RQB_PTR_TYPE)pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel_done(+) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		pPipe->d.lower.lower_orig_comp_ch_handle = pPipe->func.pCompFuncs->GetOpenChannelLowerHandle(pRQB);
		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENED_LOWER);
	}
	else
	{
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel_done(-) - hH(%d) hdId(%d) ifId(%d) pipeId(%d) pRQB(0x%x)",pPipe->pInstance->hH, pPipe->hd_id, pPipe->if_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}

	/* return RQB to hif upper */
	hif_send(pPipe, pRQB);
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @param pRQB
 * @return
 *        - LSA_RET_ERR_RESOURCE
 *        - LSA_RET_ERR_PARAM
 *        - LSA_RET_OK
 */
LSA_UINT16 hif_setup_pipe_mapping(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 e=1;
	LSA_UINT16 i,f;

	#if (defined HIF_CFG_LD_GLOB_USR_PIPES) || (defined HIF_CFG_LD_IF_USR_PIPES) || (defined HIF_CFG_HD_GLOB_USR_PIPES) || (defined HIF_CFG_HD_IF_USR_PIPES)
	LSA_UINT16 g;
	#endif

	#ifdef HIF_CFG_LD_GLOB_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aLdGlobUsrPipes[] = HIF_CFG_LD_GLOB_USR_PIPES;
	#endif
	#ifdef HIF_CFG_LD_IF_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aLdIfUsrPipes[] = HIF_CFG_LD_IF_USR_PIPES;
	#endif
	#ifdef HIF_CFG_HD_GLOB_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aHdGlobUsrPipes[] = HIF_CFG_HD_GLOB_USR_PIPES;
	#endif
	#ifdef HIF_CFG_HD_IF_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aHdIfUsrPipes[] = HIF_CFG_HD_IF_USR_PIPES;
	#endif

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_pipe_mapping(+) -  pHifInst->hH(0x%x), pRQB(0x%x)", pHifInst->hH, pRQB);

	switch(pHifInst->Type)
	{
		case HIF_DEV_TYPE_LD_LOWER:
		case HIF_DEV_TYPE_LD_UPPER:
        {
			/* Global LD Usr Pipes */
			#ifdef HIF_CFG_LD_GLOB_USR_PIPES
			for(g=0; g<(sizeof(aLdGlobUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
			{
				hif_setup_single_pipe_params_by_index(pHifInst, e++, aLdGlobUsrPipes[g].comp_id, 0, 0, aLdGlobUsrPipes[g].pipe_id);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
			}
			#endif
		
			/* IF specific pipes */
			for(f=0; f<pRQB->args.dev_ld_open.hd_count; f++)
			{
				for(i=0; i<pRQB->args.dev_ld_open.hd_args[f].nr_of_if; i++)
				{
					/* IF specific LD Usr Pipes */
					#ifdef HIF_CFG_LD_IF_USR_PIPES
					for(g=0; g<(sizeof(aLdIfUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
					{
						hif_setup_single_pipe_params_by_index(pHifInst, e++, aLdIfUsrPipes[g].comp_id, pRQB->args.dev_ld_open.hd_args[f].hd_id, pRQB->args.dev_ld_open.hd_args[f].pnio_if[i].if_id, aLdIfUsrPipes[g].pipe_id);
						
                        if (e>=HIF_MAX_PIPES_INST)
                        {
                            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                            return LSA_RET_ERR_RESOURCE;
                        }
					}
					#endif
				}
			}

			break;
        }
		case HIF_DEV_TYPE_HD_LOWER:
		case HIF_DEV_TYPE_HD_UPPER:
        {
			/* Global HD Usr Pipes */
			#ifdef HIF_CFG_HD_GLOB_USR_PIPES
			for (g=0; g<(sizeof(aHdGlobUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
			{
				hif_setup_single_pipe_params_by_index(pHifInst, e++, aHdGlobUsrPipes[g].comp_id, 0, 0, aHdGlobUsrPipes[g].pipe_id);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
			}
			#endif

			/* Unique Pipes */

			#if (HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS == 1)
			#if (HIF_CFG_USE_SOCK == 1)

			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_SOCK,  0, 0, HIF_CPHD_GLO_APP_SOCK_USER_CH_1);
			
            if (e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }

			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_SOCK,  0, 0, HIF_CPHD_GLO_APP_SOCK_USER_CH_2);
			
            if (e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif
			#endif

			/* IF specific pipes */
			for(i=0; i<pRQB->args.dev_hd_open.hd_args.nr_of_if; i++)
			{
				/* IF specific HD Usr Pipes */
				#ifdef HIF_CFG_HD_IF_USR_PIPES
				for(g=0; g<(sizeof(aHdIfUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
				{
					hif_setup_single_pipe_params_by_index(pHifInst, e++, aHdIfUsrPipes[g].comp_id, pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, aHdIfUsrPipes[g].pipe_id);
					
                    if (e>=HIF_MAX_PIPES_INST)
                    {
                        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                        return LSA_RET_ERR_RESOURCE;
                    }
				}
				#endif

				#if (HIF_CFG_USE_EDD == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_TCIP_EDD_ARP);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				
                hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_TCIP_EDD_ICMP);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				
                hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_TCIP_EDD_UDP);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				
                hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_TCIP_EDD_TCP);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif

				#if (HIF_CFG_USE_LLDP == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_LLDP, pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_OHA_LLDP_EDD);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif

				#if (HIF_CFG_USE_MRP == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_MRP,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_OHA_MRP);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif

				#if (HIF_CFG_USE_EDD == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_OHA_EDD);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				
                hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_DCP,  pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_OHA_DCP_EDD);
				
                if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif

				#if (HIF_CFG_USE_NARE == 1)				
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_NARE, pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_IF_SYS_OHA_NARE);
				if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif

				#if (HIF_CFG_USE_CLRPC == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_CLRPC, pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_L_IF_SYS_CM_CLRPC);
				if (e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
				#endif
				#if (HIF_CFG_USE_OHA == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_OHA, pRQB->args.dev_hd_open.hd_args.hd_id, pRQB->args.dev_hd_open.hd_args.pnio_if[i].if_id, HIF_CPHD_L_IF_SYS_CM_OHA);
				#endif
			}
			break;
        }
		default:
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) invalid pHifInst->Type (0x%x)", pHifInst->Type);
            return LSA_RET_ERR_PARAM;
        }
	}

	for(i = 1; i < e; i++)
	{
		hif_update_std_comp_access_funcs(&pHifInst->Pipes[i]); /* Assign Matching RQB Access Funcs (Open Channel / Serialization / ...) */
		hif_set_pipe_state(&pHifInst->Pipes[i], HIF_PIPE_STATE_CLOSED);
	}

	return LSA_RET_OK;
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pHifInst
 * @param index
 * @param comp_id
 * @param hd_id
 * @param if_id
 * @param pipe_id
 */
LSA_VOID   hif_setup_single_pipe_params_by_index(HIF_INST_PTR_TYPE pHifInst, LSA_UINT16 index, LSA_COMP_ID_TYPE comp_id, HIF_HD_ID_TYPE hd_id, HIF_IF_ID_TYPE if_id, HIF_PIPE_ID_TYPE pipe_id)
{
    HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_single_pipe_params_by_index(+) -  pHifInst(0x%x), index(0x%x), comp_id(0x%x), hd_id(0x%x), if_id(0x%x), pipe_id(0x%x)", pHifInst, index, comp_id, hd_id, if_id, pipe_id);

	pHifInst->Pipes[index].comp_id = comp_id;
	pHifInst->Pipes[index].hd_id   = hd_id;
	pHifInst->Pipes[index].if_id   = if_id;
	pHifInst->Pipes[index].pipe_id = pipe_id;
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param ppPipe
 * @param pHifInst
 * @param hd_id
 * @param if_id
 * @param pipe_id
 */
LSA_VOID   hif_get_pipe(HIF_PIPE_PTR_TYPE* ppPipe, HIF_INST_PTR_TYPE pHifInst, HIF_HD_ID_TYPE hd_id, HIF_IF_ID_TYPE if_id, HIF_PIPE_ID_TYPE pipe_id)
{
	LSA_UINT16 i;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_single_pipe_params_by_index(+) -  ppPipe(0x%x), hd_id(0x%x), if_id(0x%x), pipe_id(0x%x)", ppPipe, hd_id, if_id, pipe_id);

	*ppPipe = LSA_NULL;

	for(i=0; i<HIF_MAX_PIPES_INST; i++)
	{
		if( (pHifInst->Pipes[i].hd_id == hd_id) &&
			(pHifInst->Pipes[i].if_id == if_id) &&
			(pHifInst->Pipes[i].pipe_id == pipe_id) )
		{
			*ppPipe = &(pHifInst->Pipes[i]);
			return;
		}
	}
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @param eState
 */
LSA_VOID   hif_set_pipe_state(HIF_PIPE_PTR_TYPE pPipe, HIF_PIPE_STATE_TYPE eState)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_set_pipe_state(+) -  pPipe(0x%x), eState(0x%x)", pPipe, eState);

	pPipe->State = eState;

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	hif_update_serialization_funcs(pPipe);
#endif
}

/**
 * @brief <empty>
 * 
 * 
 * 
 * @param pPipe
 * @return
 *        - LSA_FALSE
 *        - LSA_TRUE
 */
LSA_BOOL   hif_is_upper_pipe(HIF_PIPE_PTR_TYPE pPipe)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_upper_pipe(+) -  pPipe(0x%x)", pPipe);

	switch(pPipe->State) /* Get Pipe Type by State */
	{
		case HIF_PIPE_STATE_OPENING_LOWER:
		case HIF_PIPE_STATE_OPENED_LOWER:
        {
			return LSA_FALSE;
        }
        case HIF_PIPE_STATE_OPENING_UPPER:
		case HIF_PIPE_STATE_OPENED_UPPER:
        {
            return LSA_TRUE;
        }
        default:
        {
			break;
        }
	}

	/* HIF_PIPE_STATE_CLOSED */

	if(pPipe->PipeIndex == HIF_GLOB_ADMIN) /* Admin Pipe? => Pipe direction is derived by instance type */
	{
		if(HIF_IS_NOT_NULL(pPipe->pInstance) == LSA_TRUE)
		{
			if(hif_is_upper_device(pPipe->pInstance) == LSA_TRUE)
			{
				return LSA_TRUE;
			}
			else
			{
				return LSA_FALSE;
			}
		}
	}

	/* HIF_PIPE_STATE_CLOSED - No Admin Pipe */	
	return LSA_FALSE; /* We assume that it is a lower pipe */
}
/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
