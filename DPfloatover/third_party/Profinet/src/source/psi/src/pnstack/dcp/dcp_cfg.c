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
/*  F i l e               &F: dcp_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of DCP using PSI.                          */
/*                                                                           */
/*****************************************************************************/

#define DCP_MODULE_ID      32013
#define LTRC_ACT_MODUL_ID  32013

#include "psi_int.h"

#include "dcp_inc.h"
#include "dcp_trc.h"

DCP_FILE_SYSTEM_EXTENSION(DCP_MODULE_ID)

/*---------------------------------------------------------------------------*/

void dcp_psi_startstop( int start )
{
	LSA_UINT16 rc;

	if ( start ) 
	{
		rc = dcp_init();
		DCP_ASSERT( rc == LSA_RET_OK );
	}
	else 
	{
		rc = dcp_undo_init();
		DCP_ASSERT( rc == LSA_RET_OK );
	}
}

/*---------------------------------------------------------------------------*/

#ifndef DCP_GET_PATH_INFO
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_GET_PATH_INFO(
	LSA_UINT16              DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
	LSA_SYS_PTR_TYPE        DCP_LOCAL_MEM_ATTR *     sys_ptr_ptr,
	DCP_DETAIL_PTR_TYPE     DCP_LOCAL_MEM_ATTR *     detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                                path )
{
	*ret_val_ptr = psi_dcp_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_RELEASE_PATH_INFO
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_RELEASE_PATH_INFO(
	LSA_UINT16              DCP_LOCAL_MEM_ATTR *         ret_val_ptr,
	LSA_SYS_PTR_TYPE                                     sys_ptr,
	DCP_DETAIL_PTR_TYPE                                  detail_ptr )
{
	*ret_val_ptr = psi_dcp_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_FATAL_ERROR
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FATAL_ERROR(
	LSA_UINT16               length,
	DCP_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "dcp", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_OPEN_CHANNEL_LOWER
LSA_VOID DCP_LOWER_OUT_FCT_ATTR DCP_OPEN_CHANNEL_LOWER(
	DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr.RbEthPtr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr.RbEthPtr, LSA_COMP_ID_DCP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_open_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr.RbEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_CLOSE_CHANNEL_LOWER
LSA_VOID DCP_LOWER_OUT_FCT_ATTR DCP_CLOSE_CHANNEL_LOWER(
	DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr.RbEthPtr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr.RbEthPtr, LSA_COMP_ID_DCP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_close_fct( pSys->comp_id_lower  );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr.RbEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_REQUEST_LOWER
LSA_VOID DCP_LOWER_OUT_FCT_ATTR DCP_REQUEST_LOWER(
	DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE       sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"DCP_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u) svc(%#x)",
		lower_rqb_ptr.RbEthPtr,
		DCP_ETH_RQB_HANDLE_GET(lower_rqb_ptr.RbEthPtr),
		DCP_ETH_RQB_OPCODE_GET(lower_rqb_ptr.RbEthPtr),
		DCP_ETH_RQB_SERVICE_GET(lower_rqb_ptr.RbEthPtr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr.RbEthPtr, pSys->comp_id_lower );

	// get the mailbox ID (based on service for EDDI) and request function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr.RbEthPtr, LSA_COMP_ID_DCP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_request_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr.RbEthPtr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_ENTER
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ENTER( LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

#ifndef DCP_EXIT
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_EXIT( LSA_VOID ) 
{
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_ALLOC_LOWER_RXMEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_LOWER_RXMEM(
	DCP_LOWER_RXMEM_PTR_TYPE DCP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
    LSA_UNUSED_ARG( user_id );
	DCP_ASSERT( pSys );

    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
		    "DCP_ALLOC_LOWER_RXMEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        lower_mem_ptr_ptr->RxEthPtr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_rx_mem( &(lower_mem_ptr_ptr->RxEthPtr), length, sys_ptr, LSA_COMP_ID_DCP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_LOWER_RXMEM(): ptr(%08x)", lower_mem_ptr_ptr->RxEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_LOWER_RXMEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_LOWER_RXMEM(
	LSA_UINT16               DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_LOWER_RXMEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_LOWER_RXMEM(): ptr(%08x)", lower_mem_ptr.RxEthPtr );

	psi_edd_free_nrt_rx_mem( ret_val_ptr, lower_mem_ptr.RxEthPtr, sys_ptr, LSA_COMP_ID_DCP );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_ALLOC_LOWER_TXMEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_LOWER_TXMEM(
	DCP_LOWER_TXMEM_PTR_TYPE DCP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
    LSA_UNUSED_ARG( user_id );
	DCP_ASSERT( pSys );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_LOWER_TXMEM(): invalid length 0" );
        lower_mem_ptr_ptr->TxEthPtr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_tx_mem( &(lower_mem_ptr_ptr->TxEthPtr), length, sys_ptr, LSA_COMP_ID_DCP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_LOWER_TXMEM(): ptr(%08x)", lower_mem_ptr_ptr->TxEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_LOWER_TXMEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_LOWER_TXMEM(
	LSA_UINT16               DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_LOWER_TXMEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_LOWER_TXMEM(): ptr(%08x)", lower_mem_ptr.TxEthPtr );

	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr.TxEthPtr, sys_ptr, LSA_COMP_ID_DCP );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_ALLOC_LOWER_RQB
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_LOWER_RQB(
	DCP_LOWER_RQB_PTR_TYPE DCP_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                            user_id,
	LSA_UINT16                                  length,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

    if ( length != sizeof(DCP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_LOWER_RQB(): invalid length != sizeof(DCP_LOWER_RQB_TYPE)" );
        lower_rqb_ptr_ptr->RbEthPtr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&(lower_rqb_ptr_ptr->RbEthPtr), user_id, length, sys_ptr, LSA_COMP_ID_DCP, PSI_MTYPE_LOWER_RQB_EDD );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr_ptr->RbEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_LOWER_RQB
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_LOWER_RQB(
	LSA_UINT16             DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr.RbEthPtr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr.RbEthPtr, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_ALLOC_LOWER_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_LOWER_MEM(
	DCP_LOWER_MEM_PTR_TYPE DCP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                            user_id,
	LSA_UINT16                                  length,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_LOWER_MEM(): invalid length 0" );
        lower_mem_ptr_ptr->MemEthPtr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

  	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&(lower_mem_ptr_ptr->MemEthPtr), user_id, length, sys_ptr, LSA_COMP_ID_DCP, PSI_MTYPE_LOWER_MEM_EDD );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_LOWER_MEM(): ptr(%08x)", (*lower_mem_ptr_ptr).MemEthPtr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_LOWER_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_LOWER_MEM(
	LSA_UINT16             DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr.MemEthPtr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr.MemEthPtr, sys_ptr, LSA_COMP_ID_DCP, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	Upper Layer
//----------------------------------------------------------------------------*/

#ifndef DCP_REQUEST_UPPER_DONE
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_REQUEST_UPPER_DONE(
	DCP_UPPER_CALLBACK_FCT_PTR_TYPE dcp_request_upper_done_ptr,
	DCP_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)dcp_request_upper_done_ptr, (LSA_VOID_PTR_TYPE *)upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_ALLOC_UPPER_RQB
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_UPPER_RQB(
	DCP_UPPER_RQB_PTR_TYPE DCP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                            user_id,
	LSA_UINT16                                  length,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

    if ( length != sizeof(DCP_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_UPPER_RQB(): invalid length != sizeof(DCP_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_DCP, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, 
		"DCP_ALLOC_UPPER_RQB(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_UPPER_RQB
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_UPPER_RQB(
	LSA_UINT16             DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_UPPER_RQB_PTR_TYPE                      upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, 
		"DCP_FREE_UPPER_RQB(): ptr(%08x)", upper_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, sys_ptr, LSA_COMP_ID_DCP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_UPPER_RQB_LOCAL(
	DCP_UPPER_RQB_PTR_TYPE DCP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_UINT16                                  length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( length != sizeof(DCP_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_UPPER_RQB_LOCAL(): invalid length != sizeof(DCP_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_DCP, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_UPPER_RQB_LOCAL(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_UPPER_RQB_LOCAL
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_UPPER_RQB_LOCAL(
	LSA_UINT16             DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_UPPER_RQB_PTR_TYPE                      upper_rqb_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_UPPER_RQB_LOCAL(): ptr(%08x)", upper_rqb_ptr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, 0, LSA_COMP_ID_DCP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef DCP_ALLOC_LOCAL_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_LOCAL_MEM(
	DCP_LOCAL_MEM_PTR_TYPE DCP_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT16                                  length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"DCP_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "DCP_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_LOCAL_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_LOCAL_MEM(
	LSA_UINT16             DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	DCP_LOCAL_MEM_PTR_TYPE                      local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "DCP_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_DCP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_COPY_COMMON_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_COPY_COMMON_MEM(
	LSA_VOID   DCP_COMMON_MEM_ATTR * src_common_mem_ptr,
	LSA_VOID   DCP_COMMON_MEM_ATTR * dst_common_mem_ptr,
	LSA_UINT16                       length )
{
	PSI_MEMCPY( dst_common_mem_ptr, src_common_mem_ptr, length );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_COPY_UPPER_TO_LOCAL_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_COPY_UPPER_TO_LOCAL_MEM(
	DCP_UPPER_MEM_PTR_TYPE src_ptr,
	DCP_LOCAL_MEM_PTR_TYPE dst_ptr,
	LSA_UINT16             length,
	LSA_SYS_PTR_TYPE	   sys_ptr )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );
	DCP_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );
#else
	LSA_UNUSED_ARG( sys_ptr );
#endif

	PSI_MEMCPY( dst_ptr, src_ptr, length );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_COPY_LOCAL_TO_UPPER_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_COPY_LOCAL_TO_UPPER_MEM(
	DCP_LOCAL_MEM_PTR_TYPE	 		src_ptr,
	DCP_UPPER_MEM_PTR_TYPE			dst_ptr,
	LSA_UINT16              		length,
	LSA_SYS_PTR_TYPE				sys_ptr )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );
	DCP_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );
#else
	LSA_UNUSED_ARG( sys_ptr );
#endif

	PSI_MEMCPY( dst_ptr, src_ptr, length );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_COPY_RXETH_TO_LOCAL_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_COPY_RXETH_TO_LOCAL_MEM(
	DCP_ETH_RXVOID_PTR_TYPE src_ptr,
	DCP_LOCAL_MEM_PTR_TYPE  dst_ptr,
	LSA_UINT16              length,
	LSA_SYS_PTR_TYPE        sys_ptr )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );
	DCP_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );
#else
	LSA_UNUSED_ARG(sys_ptr);
#endif

	PSI_MEMCPY(dst_ptr, src_ptr, length);
}
#else
#error "by design a function!"
#endif

#ifndef DCP_UPPER_TO_ETH_MEM
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_UPPER_TO_ETH_MEM(
	DCP_ETH_MEM_PTR_TYPE   DCP_LOCAL_MEM_ATTR * eth_mem_ptr_ptr,
	DCP_UPPER_MEM_PTR_TYPE                      upper_mem_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );
	DCP_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );
#else
	LSA_UNUSED_ARG( sys_ptr );
#endif

	*eth_mem_ptr_ptr = upper_mem_ptr; 
}
#else
#error "by design a function!"
#endif

#ifndef DCP_ETH_TO_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ETH_TO_UPPER_MEM(
	DCP_UPPER_MEM_PTR_TYPE	DCP_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	DCP_ETH_MEM_PTR_TYPE						 eth_mem_ptr,
	LSA_SYS_PTR_TYPE							 sys_ptr )
{
#if PSI_DEBUG /*(LaM) needed for preventing Warnings in release Builds, SysPtr is only used in Asserts which will be empty*/
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	DCP_ASSERT( pSys );
	DCP_ASSERT( pSys->comp_id == LSA_COMP_ID_DCP );
#else
	LSA_UNUSED_ARG( sys_ptr );
#endif

	*upper_mem_ptr_ptr = eth_mem_ptr;
}
#else
#error "by design a function!"
#endif

#ifndef DCP_MEMSET_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_MEMSET_LOCAL(
	DCP_LOCAL_MEM_PTR_TYPE      pMem,
	LSA_UINT8                   Value,
	LSA_UINT32                  Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_ALLOC_TIMER
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_ALLOC_TIMER(
	LSA_UINT16 DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 DCP_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                      timer_type,
	LSA_UINT16                      time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr,  timer_id_ptr,  timer_type, time_base, dcp_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_START_TIMER
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_START_TIMER(
	LSA_UINT16 DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                      timer_id,
	LSA_USER_ID_TYPE                user_id,
	LSA_UINT16                      time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_STOP_TIMER
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_STOP_TIMER(
	LSA_UINT16 DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                      timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef DCP_FREE_TIMER
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_FREE_TIMER(
	LSA_UINT16 DCP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                      timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef DCP_DO_TIMER_REQUEST
LSA_VOID DCP_SYSTEM_OUT_FCT_ATTR DCP_DO_TIMER_REQUEST(
	DCP_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, (LSA_VOID_PTR_TYPE *)pRQB );
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
