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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_arsm2.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  AR state-machines                                                        */
/*  - m-provider state-machine                                               */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  72
#define CM_MODULE_ID       72

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== M-PROVIDER ============================================================*/
/*===========================================================================*/

/*
 * m-providers are added with CM_AR_CR_IDLE
 *
 * after CM_ARCQ_OPC_MPROV_ADD the cr_state == CM_AR_CR_PASSIVE
 *
 * reason: the user can initialize the buffer
 * and activate the m-provider afterwards
 */

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_start (
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_arsm_mprov_start")

	if( cm_link_status_is_up(argr->ar_com.channel) )
	{
		cm_arsm_mprov_link_up (argr);
	}
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_link_up(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_arsm_mprov_link_up")

	if (cm_ar_com_is_down(argr))
	{
		CM_AR_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) ar(%u): ignore link-up, ar is down(%u)"
			, argr->device_nr, argr->ar_nr, argr->ar_com.down
			);
	}
	else
	{
		cm_arcq_request (argr, CM_ARCQ_OPC_MPROV_ACTIVATE, 0);
	}
}

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
) {
	CM_LOG_FCT("cm_arsm_mprov_stop")

	LSA_UNUSED_ARG (argr);

	/* will be done with CM_ARCQ_OPC_FRAMES_PASSIVATE */
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
