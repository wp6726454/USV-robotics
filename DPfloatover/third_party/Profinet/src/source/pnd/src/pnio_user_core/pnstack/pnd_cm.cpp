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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_cm.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  base adaption for CM                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 3601
#define PND_MODULE_ID     3601

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

#include "pnd_pnstack.h"

#include "psi_cfg.h"
#include "psi_usr.h"
#include "psi_sys.h"

#include "cm_inc.h"

/* StdLib++ */
#include <vector>
#include <string>

using namespace std;

#include "pnd_ValueHelper.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE pnd_cm_alloc_rqb( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PND_HANDLE_PTR_TYPE pnd_sys, LSA_UINT16 more_size )
{
	// Allocate an user RQB for opcode
	CValueHelper           valObj;
	CM_UPPER_RQB_PTR_TYPE  pRQB;
	LSA_UINT16             alloc_size;
	PSI_SYS_TYPE           psi_sys;
	LSA_USER_ID_TYPE dummy_user_id = {0};

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from CM to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	alloc_size = (LSA_UINT16)(sizeof( CM_ANY_ARGS_TYPE ) + more_size); // Note: for all xxx[1] RQB params
	PND_ASSERT(alloc_size < 0xFFFF );

	CM_ALLOC_UPPER_RQB( &pRQB, dummy_user_id, sizeof(*pRQB), &psi_sys );
	PND_ASSERT(pRQB);

	CM_RQB_SET_OPCODE( pRQB, opcode);
	CM_RQB_SET_HANDLE( pRQB, handle);
	LSA_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_CM );

	CM_ALLOC_UPPER_MEM(&pRQB->args.void_ptr, dummy_user_id, alloc_size, &psi_sys );
	PND_ASSERT(pRQB->args.void_ptr);

	switch ( opcode )  // alloc additional data based on opcode
	{
#if ( PSI_CFG_USE_IOC )
	case CM_OPC_CL_ALARM_SEND:
	case CM_OPC_CL_ALARM_ACK_SEND:
		{
			pRQB->args.channel.alarm->alarm_data_length = EDD_FRAME_BUFFER_LENGTH;
			CM_ACP_ALLOC_ALARM_MEM((CM_ACP_LOWER_MEM_PTR_TYPE*)&pRQB->args.channel.alarm->alarm_data, EDD_FRAME_BUFFER_LENGTH, &psi_sys);
			PND_ASSERT( pRQB->args.channel.alarm->alarm_data);
		}
		break;

#endif
#if ( PSI_CFG_USE_IOD )
	case CM_OPC_SV_AR_ALARM_SEND:
		{
			pRQB->args.channel.alarm->alarm_data_length = EDD_FRAME_BUFFER_LENGTH;
			CM_ACP_ALLOC_ALARM_MEM((CM_ACP_LOWER_MEM_PTR_TYPE*)&pRQB->args.channel.alarm->alarm_data, EDD_FRAME_BUFFER_LENGTH, &psi_sys );
			PND_ASSERT(pRQB->args.channel.alarm->alarm_data);
		}
		break;
#endif
	case CM_OPC_ALARM_IND_RES_PROVIDE:
		{
			pRQB->args.channel.alarm->alarm_data_length = 0;
			pRQB->args.channel.alarm->alarm_data        = LSA_NULL;
		}
		break;
	default:
		break;
	}

	return ( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_cm_free_rqb( LSA_VOID_PTR_TYPE p_rqb, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper valObj;
	LSA_UINT16   rc;
	PSI_SYS_TYPE psi_sys;

	CM_UPPER_RQB_PTR_TYPE pRQB = (CM_UPPER_RQB_PTR_TYPE)p_rqb;
	PND_ASSERT( pRQB != LSA_NULL );
	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from CM to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	switch( CM_RQB_GET_OPCODE(pRQB) ) // free additional data based on opcode
	{
#if ( PSI_CFG_USE_IOC )
	case CM_OPC_CL_ALARM_SEND:
	case CM_OPC_CL_ALARM_ACK_SEND:
#endif
#if ( PSI_CFG_USE_IOD )
	case CM_OPC_SV_AR_ALARM_SEND:
#endif
	case CM_OPC_ALARM_IND_RES_PROVIDE:
		{
			if ( pRQB->args.channel.alarm->alarm_data != LSA_NULL )
			{
				CM_ACP_FREE_ALARM_MEM( &rc, pRQB->args.channel.alarm->alarm_data, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	default:
		break;
	}

	if ( pRQB->args.void_ptr != LSA_NULL ) 
	{
		CM_FREE_UPPER_MEM(&rc, pRQB->args.void_ptr, &psi_sys );
		PND_ASSERT( rc == LSA_RET_OK );
	}

	CM_FREE_UPPER_RQB(&rc, pRQB, &psi_sys );
	PND_ASSERT( rc == LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
LSA_VOID_PTR_TYPE pnd_cm_alloc_upper_mem( LSA_UINT16 len, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper      valObj;
	LSA_VOID_PTR_TYPE p_ptr;
	PSI_SYS_TYPE      psi_sys;
	LSA_USER_ID_TYPE  dummy_user_id = {0};

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from CM to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	CM_ALLOC_UPPER_MEM(&p_ptr, dummy_user_id, len, &psi_sys );
	PND_ASSERT( p_ptr != 0 );

	return ( p_ptr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_cm_free_upper_mem( LSA_VOID_PTR_TYPE p_mem, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper valObj;
	LSA_UINT16   rc;
	PSI_SYS_TYPE psi_sys;

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from CM to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	CM_FREE_UPPER_MEM(&rc, p_mem, &psi_sys);
	PND_ASSERT(rc == LSA_RET_OK);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
