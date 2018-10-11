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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cfg_oha.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using PSI.                           */
/*                                                                           */
/*****************************************************************************/

#define CM_MODULE_ID       32055
#define LTRC_ACT_MODUL_ID  32055

#include "psi_int.h"
#include "psi_ld.h"

#if ( PSI_CFG_USE_HIF_HD == 1 )
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "cm_inc.h"
#include "cm_trc.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*------------------------------------------------------------------------------
//	Lower Layer
//----------------------------------------------------------------------------*/

#ifndef CM_OHA_OPEN_CHANNEL_LOWER
LSA_VOID CM_OHA_LOWER_OUT_FCT_ATTR CM_OHA_OPEN_CHANNEL_LOWER(
	CM_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_OPEN_CHANNEL_LOWER()" );

	CM_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER(OHA is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_open_channel, lower_rqb_ptr );
    #else
	// post the message to OHA (without HIF OHA is located in same system)
	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_CLOSE_CHANNEL_LOWER
LSA_VOID CM_OHA_LOWER_OUT_FCT_ATTR CM_OHA_CLOSE_CHANNEL_LOWER(
	CM_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_CLOSE_CHANNEL_LOWER()" );

	CM_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER (OHA is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_close_channel, lower_rqb_ptr );
    #else
	// post the message to OHA (without HIF OHA is located in same system)
	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_REQUEST_LOWER
LSA_VOID CM_OHA_LOWER_OUT_FCT_ATTR CM_OHA_REQUEST_LOWER(
	CM_OHA_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"CM_OHA_REQUEST_LOWER(): rqb(%08x) h(%d) opc(%d)",
		lower_rqb_ptr,
		CM_OHA_GET_HANDLE(lower_rqb_ptr),
		CM_OHA_GET_OPCODE(lower_rqb_ptr));

	CM_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_OHA );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD LOWER (OHA is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_l_request, lower_rqb_ptr );
    #else
	// post the message to OHA (without HIF OHA is located in same system)
	psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_ALLOC_LOWER_RQB
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_OHA_ALLOC_LOWER_RQB(
	CM_OHA_LOWER_RQB_PTR_TYPE CM_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );
	
    if ( length != sizeof(CM_OHA_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"CM_OHA_ALLOC_LOWER_RQB(): invalid length != sizeof(CM_OHA_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, CM_MODULE_ID, 0 );
        return;
    }

    PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr , user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_RQB_OHA );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_ALLOC_LOWER_RQB: ptr(%08x)", *lower_rqb_ptr_ptr);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_FREE_LOWER_RQB
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_OHA_FREE_LOWER_RQB(
	LSA_UINT16                CM_LOCAL_MEM_ATTR * ret_val_ptr,
	CM_OHA_LOWER_RQB_PTR_TYPE                     lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr);

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_RQB_OHA );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_ALLOC_LOWER_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_OHA_ALLOC_LOWER_MEM(
	CM_OHA_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"CM_OHA_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, CM_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_OHA );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_FREE_LOWER_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_OHA_FREE_LOWER_MEM(
	LSA_UINT16                CM_LOCAL_MEM_ATTR * ret_val_ptr,
	CM_OHA_LOWER_MEM_PTR_TYPE                     lower_mem_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_OHA_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr);

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_OHA );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_OHA_LOWER_RQB_ERROR
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_OHA_LOWER_RQB_ERROR(
	CM_OHA_LOWER_RQB_PTR_TYPE oha_lower_rqb_ptr )
{
	PSI_RQB_ERROR( LSA_COMP_ID_CM, LSA_COMP_ID_OHA, oha_lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
