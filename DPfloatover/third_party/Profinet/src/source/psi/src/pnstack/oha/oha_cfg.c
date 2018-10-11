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
/*  F i l e               &F: oha_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of OHA using PSI.                          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define OHA_MODULE_ID      32031
#define LTRC_ACT_MODUL_ID  32031

#include "psi_int.h"
#include "psi_hd.h"

#if ( PSI_CFG_USE_HIF_HD == 1 )
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "oha_inc.h"
#include "oha_trc.h"

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_UINT16 oha_enter_exit_id = PSI_LOCK_ID_INVALID;

/*----------------------------------------------------------------------------*/

void oha_psi_startstop( int start )
{
	LSA_UINT16        rc;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &oha_enter_exit_id );
		OHA_ASSERT( rsp == LSA_RET_OK );

		rc = oha_init();
		OHA_ASSERT( rc == OHA_OK );

		// Note: pcIOX uses the PNIO IF numbers as NIC (global = 0)
		rc = oha_init_interface( PSI_GLOBAL_PNIO_IF_NR, 0, 0, 0, LSA_FALSE, 0, LSA_NULL, 0, LSA_FALSE, LSA_NULL, 0, 0, 0, LSA_NULL, 0, LSA_NULL, 0 ); /* init global IF */
		OHA_ASSERT( rc == OHA_OK );
	}
	else
	{
		rc = oha_undo_init();
		OHA_ASSERT( rc == OHA_OK );

		PSI_FREE_REENTRANCE_LOCK( &rsp, oha_enter_exit_id );
		OHA_ASSERT( rsp == LSA_RET_OK );

		oha_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*----------------------------------------------------------------------------*/

#ifndef OHA_GET_PATH_INFO
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_GET_PATH_INFO(
	LSA_UINT16              OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE        OHA_LOCAL_MEM_ATTR *  sys_ptr_ptr,
	OHA_DETAIL_PTR_TYPE     OHA_LOCAL_MEM_ATTR *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                             path )
{
	*ret_val_ptr = psi_oha_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_RELEASE_PATH_INFO
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_RELEASE_PATH_INFO(
	LSA_UINT16              OHA_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	OHA_DETAIL_PTR_TYPE     detail_ptr )
{
	*ret_val_ptr = psi_oha_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ALLOC_TIMER
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_ALLOC_TIMER(
	LSA_UINT16 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 OHA_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                      timer_type,
	LSA_UINT16                      time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, oha_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_START_TIMER
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_START_TIMER(
	LSA_UINT16       OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                            timer_id,
	LSA_USER_ID_TYPE                      user_id,
	LSA_UINT16                            time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_STOP_TIMER
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_STOP_TIMER(
	LSA_UINT16 OHA_LOCAL_MEM_ATTR  * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_FREE_TIMER
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_FREE_TIMER(
	LSA_UINT16 OHA_LOCAL_MEM_ATTR  * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_DO_TIMER_REQUEST
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_DO_TIMER_REQUEST(
	OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_FATAL_ERROR
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_FATAL_ERROR(
	LSA_UINT16               length,
	OHA_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "oha", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ENTER
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_ENTER( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( oha_enter_exit_id );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EXIT
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_EXIT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( oha_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ALLOC_LOCAL_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_ALLOC_LOCAL_MEM(
	OHA_LOCAL_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT32                                  length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"OHA_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
}

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM );
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "OHA_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_FREE_LOCAL_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_FREE_LOCAL_MEM(
	LSA_UINT16             OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_LOCAL_MEM_PTR_TYPE                      local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "OHA_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_DCP_OPEN_CHANNEL_LOWER
LSA_VOID OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_OPEN_CHANNEL_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower== LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (DCP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	// post the message to DCP (without HIF SCP is located in same system)
	psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_CLOSE_CHANNEL_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (DCP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	// post the message to DCP (without HIF SCP is located in same system)
	psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_REQUEST_LOWER
LSA_VOID OHA_DCP_LOWER_OUT_FCT_ATTR  OHA_DCP_REQUEST_LOWER(
	OHA_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, 
		"OHA_DCP_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u)",
		lower_rqb_ptr, 
		OHA_DCP_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_DCP_RQB_GET_OPCODE(lower_rqb_ptr) );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (DCP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	// post the message to DCP (without HIF DCP is located in same system)
	psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_EDD_OPEN_CHANNEL_LOWER
LSA_VOID OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_OPEN_CHANNEL_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and open function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_open_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_CLOSE_CHANNEL_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and close function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_close_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_REQUEST_LOWER
LSA_VOID OHA_EDD_LOWER_OUT_FCT_ATTR  OHA_EDD_REQUEST_LOWER(
	OHA_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_EDD_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u) service(%u)",
		lower_rqb_ptr, 
		OHA_EDD_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_EDD_RQB_GET_OPCODE(lower_rqb_ptr), 
		OHA_EDD_RQB_GET_SERVICE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and request function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_OHA, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_request_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_SOCK

#ifndef OHA_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_OPEN_CHANNEL_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_CLOSE_CHANNEL_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_REQUEST_LOWER
LSA_VOID OHA_SOCK_LOWER_OUT_FCT_ATTR  OHA_SOCK_REQUEST_LOWER(
	OHA_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_SOCK_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u)",
		lower_rqb_ptr, 
		OHA_SOCK_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_SOCK_RQB_GET_OPCODE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#endif /* SOCK */

/*----------------------------------------------------------------------------*/

#ifdef OHA_CFG_USE_SNMPX

#ifndef OHA_SNMPX_OPEN_CHANNEL_LOWER
LSA_VOID OHA_SNMPX_LOWER_OUT_FCT_ATTR OHA_SNMPX_OPEN_CHANNEL_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_CLOSE_CHANNEL_LOWER
LSA_VOID OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_SNMPX_CLOSE_CHANNEL_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_REQUEST_LOWER
LSA_VOID OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_SNMPX_REQUEST_LOWER(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_SNMPX_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_SNMPX_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_SNMPX_RQB_GET_OPCODE(lower_rqb_ptr) );

	OHA_ASSERT( pSys->comp_id == LSA_COMP_ID_SNMPX );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#endif /* SNMPX */

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_NARE

#ifndef OHA_NARE_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_OPEN_CHANNEL_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (NARE is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	// post the message to NARE (without HIF NARE is located in same system)
	psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_CLOSE_CHANNEL_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (NARE is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	// post the message to NARE (without HIF NARE is located in same system)
	psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_REQUEST_LOWER
LSA_VOID  OHA_NARE_LOWER_OUT_FCT_ATTR  OHA_NARE_REQUEST_LOWER(
	OHA_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"OHA_NARE_REQUEST_LOWER(): rqb(%08x) h(%d) opc(%d)",
		lower_rqb_ptr, 
		OHA_NARE_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_NARE_RQB_GET_OPCODE(lower_rqb_ptr) );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_NARE );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (NARE is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	// post the message to NARE (without HIF NARE is located in same system)
	psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#endif  /* NARE */

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_MRP

#ifndef OHA_MRP_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_OPEN_CHANNEL_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (MRP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	// post the message to MRP (without HIF MRP is located in same system)
	psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_CLOSE_CHANNEL_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE           sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (MRP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	// post the message to MRP (without HIF MRP is located in same system)
	psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_REQUEST_LOWER
LSA_VOID  OHA_MRP_LOWER_OUT_FCT_ATTR  OHA_MRP_REQUEST_LOWER(
	OHA_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, 
		"OHA_MRP_REQUEST_LOWER(): rqb(%08x) h(%d) opc(%d)",
		lower_rqb_ptr, 
		OHA_MRP_RQB_GET_HANDLE(lower_rqb_ptr), 
		OHA_MRP_RQB_GET_OPCODE(lower_rqb_ptr) );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_MRP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (MRP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	// post the message to MRP (without HIF MRP is located in same system)
	psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#endif  /* MRP */

/*----------------------------------------------------------------------------*/

#ifndef OHA_LLDP_OPEN_CHANNEL_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_OPEN_CHANNEL_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (LLDP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	// post the message to LLDP (without HIF LLDP is located in same system)
	psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_CLOSE_CHANNEL_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_CLOSE_CHANNEL_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (LLDP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	// post the message to LLDP (without HIF LLDP is located in same system)
	psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_REQUEST_LOWER
LSA_VOID  OHA_LLDP_LOWER_OUT_FCT_ATTR  OHA_LLDP_REQUEST_LOWER(
	OHA_LLDP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, 
		"OHA_LLDP_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		OHA_LLDP_RQB_GET_HANDLE(lower_rqb_ptr),
		OHA_LLDP_RQB_GET_OPCODE(lower_rqb_ptr) );

	OHA_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_LLDP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (LLDP is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	// post the message to LLDP (without HIF LLDP is located in same system)
	psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_MEMSET
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR  OHA_MEMSET(
	OHA_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8              Value,
	LSA_UINT32             Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef OHA_REQUEST_UPPER_DONE
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR  OHA_REQUEST_UPPER_DONE(
	OHA_UPPER_CALLBACK_FCT_PTR_TYPE  oha_request_upper_done_ptr,
	OHA_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)oha_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_ALLOC_UPPER_RQB_LOCAL
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_ALLOC_UPPER_RQB_LOCAL(
	OHA_UPPER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_UINT16                                  length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( length != sizeof(OHA_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"OHA_ALLOC_UPPER_RQB_LOCAL(): invalid length != sizeof(OHA_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
}

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_RQB );
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "OHA_ALLOC_UPPER_RQB_LOCAL(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_FREE_UPPER_RQB_LOCAL
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_FREE_UPPER_RQB_LOCAL(
	LSA_UINT16             OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_UPPER_RQB_PTR_TYPE                      upper_rqb_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "OHA_FREE_UPPER_RQB_LOCAL(): ptr(%08x)", upper_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, 0, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_ALLOC_UPPER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_ALLOC_UPPER_MEM(
	OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE                            user_id,
	LSA_UINT16                                  length,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_ALLOC_UPPER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_MEM );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_ALLOC_UPPER_MEM(): ptr(%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_FREE_UPPER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_FREE_UPPER_MEM(
	LSA_UINT16             OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_UPPER_MEM_PTR_TYPE                      upper_mem_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_FREE_UPPER_MEM(): ptr(%08x)", upper_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_DCP_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_DCP_ALLOC_LOWER_RQB(
	OHA_DCP_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_DCP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_DCP_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_DCP_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DCP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_DCP_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_DCP_FREE_LOWER_RQB(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_DCP_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_DCP_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DCP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#ifndef OHA_DCP_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_DCP_ALLOC_LOWER_MEM(
	OHA_DCP_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_DCP_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DCP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_DCP_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_DCP_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_DCP_FREE_LOWER_MEM(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_DCP_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_DCP_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DCP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_LLDP_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_LLDP_ALLOC_LOWER_RQB(
	OHA_LLDP_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_LLDP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_LLDP_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_LLDP_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_LLDP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_LLDP_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_LLDP_FREE_LOWER_RQB(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_LLDP_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_LLDP_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_LLDP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_LLDP_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_LLDP_ALLOC_LOWER_MEM(
	OHA_LLDP_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_LLDP_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_LLDP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_LLDP_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_LLDP_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_LLDP_FREE_LOWER_MEM(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_LLDP_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_LLDP_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_LLDP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_NARE

#ifndef OHA_NARE_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_NARE_ALLOC_LOWER_RQB(
	OHA_NARE_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_NARE_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_NARE_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_NARE_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_NARE );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_NARE_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_NARE_FREE_LOWER_RQB(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_NARE_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_NARE_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_NARE );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_NARE_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_NARE_ALLOC_LOWER_MEM(
	OHA_NARE_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_NARE_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_NARE );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_NARE_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NARE_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_NARE_FREE_LOWER_MEM(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR  * ret_val_ptr,
	OHA_NARE_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_NARE_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_NARE );
}
#else
#error "by design a function!"
#endif

#endif /* OHA_CFG_NO_NARE */

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_MRP

#ifndef OHA_MRP_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MRP_ALLOC_LOWER_RQB(
	OHA_MRP_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_MRP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_MRP_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_MRP_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_MRP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_MRP_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MRP_FREE_LOWER_RQB(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_MRP_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_MRP_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_MRP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_MRP_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MRP_ALLOC_LOWER_MEM(
	OHA_MRP_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_MRP_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_MRP );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_MRP_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MRP_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MRP_FREE_LOWER_MEM(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_MRP_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_MRP_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_MRP );
}
#else
#error "by design a function!"
#endif

#endif /* OHA_CFG_NO_MRP */

/*----------------------------------------------------------------------------*/

#ifndef OHA_EDD_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_EDD_ALLOC_LOWER_RQB(
	OHA_EDD_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_EDD_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_EDD_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_EDD_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_EDD );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_EDD_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_EDD_FREE_LOWER_RQB(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_EDD_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_EDD_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_EDD_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_EDD_ALLOC_LOWER_MEM(
	OHA_EDD_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_EDD_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_EDD );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_EDD_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_EDD_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_EDD_FREE_LOWER_MEM(
	LSA_UINT16                 OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_EDD_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_EDD_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_CFG_NO_SOCK

#ifndef OHA_SOCK_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SOCK_ALLOC_LOWER_RQB(
	OHA_SOCK_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_SOCK_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_SOCK_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_SOCK_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SOCK );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SOCK_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SOCK_FREE_LOWER_RQB(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_SOCK_LOWER_RQB_PTR_TYPE                      lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SOCK_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SOCK );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_SOCK_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SOCK_ALLOC_LOWER_MEM(
	OHA_SOCK_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                 user_id,
	LSA_UINT16                                       length,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_SOCK_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SOCK );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SOCK_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SOCK_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR  OHA_SOCK_FREE_LOWER_MEM(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_SOCK_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SOCK_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SOCK );
}
#else
#error "by design a function!"
#endif

#endif /* SOCK */

/*----------------------------------------------------------------------------*/

#ifdef OHA_CFG_USE_SNMPX

#ifndef OHA_SNMPX_ALLOC_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SNMPX_ALLOC_LOWER_RQB(
	OHA_SNMPX_LOWER_RQB_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(OHA_SNMPX_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_SNMPX_ALLOC_LOWER_RQB(): invalid length != sizeof(OHA_SNMPX_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SNMPX );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SNMPX_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_FREE_LOWER_RQB
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SNMPX_FREE_LOWER_RQB(
	LSA_UINT16                  OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_SNMPX_LOWER_RQB_PTR_TYPE                     lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                 sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SNMPX_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SNMPX );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_SNMPX_ALLOC_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SNMPX_ALLOC_LOWER_MEM(
	OHA_SNMPX_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"OHA_SNMPX_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_OHA, OHA_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SNMPX );
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SNMPX_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SNMPX_FREE_LOWER_MEM
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_SNMPX_FREE_LOWER_MEM(
	LSA_UINT16                   OHA_LOCAL_MEM_ATTR * ret_val_ptr,
	OHA_SNMPX_LOWER_MEM_PTR_TYPE                      lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	OHA_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "OHA_SNMPX_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SNMPX );
}
#else
#error "by design a function!"
#endif

#endif /* SNMPX */

/*----------------------------------------------------------------------------*/

#ifndef OHA_RQB_ERROR
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_RQB_ERROR(
	OHA_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_OHA, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef OHA_MEMCPY
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MEMCPY(
	OHA_COMMON_MEM_PTR_TYPE pDst,
	OHA_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32              Length )
{
	PSI_MEMCPY( pDst, pSrc, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MEMMOVE
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_MEMMOVE(
	OHA_COMMON_MEM_PTR_TYPE  pDst,
	OHA_COMMON_MEM_PTR_TYPE  pSrc,
	LSA_UINT32               Length )
{
	PSI_MEMMOVE( pDst, pSrc, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_MEMCMP
LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR OHA_MEMCMP(
	OHA_COMMON_MEM_PTR_TYPE pBuf1,
	OHA_COMMON_MEM_PTR_TYPE pBuf2,
	LSA_UINT32              Length )
{
	return( PSI_MEMCMP( pBuf1, pBuf2, (LSA_UINT)Length ) == 0 ? LSA_TRUE : LSA_FALSE );
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  OHA_SWAP_U16, _U32
 *
 * function:       swap "in place"
 *                 between "little" and "big" endian or vice versa
 *
 * documentation:  LSA_CLRPC_LSA-Interface.doc
 *
 *===========================================================================*/

#ifndef OHA_SWAP_U16
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR OHA_SWAP_U16(
	LSA_UINT16 OHA_COMMON_MEM_ATTR * short_ptr ) /* correct alignment! */
{
	LSA_UINT16 temp;
	temp = *short_ptr;
	*short_ptr = (LSA_UINT16)(temp >> 8) | (LSA_UINT16)(temp << 8);
}
#else
#error "by design a function!"
#endif

#ifndef OHA_SWAP_U32
LSA_VOID  OHA_SYSTEM_OUT_FCT_ATTR OHA_SWAP_U32(
	LSA_UINT32 OHA_COMMON_MEM_ATTR * long_ptr ) /* correct alignment! */
{
	LSA_UINT32 temp;
	temp = *long_ptr;
	*long_ptr = (temp >> 24)
				| ((temp >> 8) & 0x0000ff00)
				| ((temp << 8) & 0x00ff0000)
				| (temp << 24);
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:    OHA_HTONS ... host to network byte order, short integer
 *                      _NTOHS ... network to host byte order, short integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONS
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_HTONS(
	LSA_UINT16 host_short )
{
    #ifndef OHA_BIG_ENDIAN
	OHA_SWAP_U16(&host_short);
    #endif
	return host_short;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NTOHS
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_NTOHS(
	LSA_UINT16 network_short )
{
    #ifndef OHA_BIG_ENDIAN
	OHA_SWAP_U16(&network_short);
    #endif
	return network_short;
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:    OHA_HTONL ... host to network byte order, long  integer
 *                      _NTOHL ... network to host byte order, long  integer
 *
 * function:       byte order conversion
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_HTONL
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_HTONL(
	LSA_UINT32 host_long )
{
#ifndef OHA_BIG_ENDIAN
	OHA_SWAP_U32(&host_long);
#endif
	return host_long;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_NTOHL
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_NTOHL(
	LSA_UINT32 network_long )
{
#ifndef OHA_BIG_ENDIAN
	OHA_SWAP_U32(&network_long);
#endif
	return network_long;
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//  read data from PDU / write data to PDU
//----------------------------------------------------------------------------*/

#ifndef OHA_PUT8_HTON
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_PUT8_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT8         val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr[offset] = val;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET8_NTOH
LSA_UINT8 OHA_LOCAL_FCT_ATTR OHA_GET8_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	return ( ptr[offset] );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_PUT16_HTON
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_PUT16_HTON ( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT16        val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	val = OHA_HTONS( val );
	OHA_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET16_NTOH
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_GET16_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT16                 val = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	OHA_MEMCPY( &val, ptr, sizeof(val) );
	val = OHA_NTOHS( val );
	return val;
}
#else
#error "by design a function!"
#endif

#ifndef OHA_PUT32_HTON
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_PUT32_HTON( /* put and convert from host byte order to network byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset,
	LSA_UINT32        val )
{
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	val = OHA_HTONL( val );
	OHA_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

#ifndef OHA_GET32_NTOH
LSA_UINT32 OHA_LOCAL_FCT_ATTR OHA_GET32_NTOH( /* get and convert from network byte order to host byte order */
	LSA_VOID_PTR_TYPE base,
	LSA_INT           offset )
{
	LSA_UINT32  val = 0;
	OHA_COMMON_MEM_U8_PTR_TYPE ptr = (OHA_COMMON_MEM_U8_PTR_TYPE)base;
	ptr += offset;
	OHA_MEMCPY( &val, ptr, sizeof(val) );
	val = OHA_NTOHL( val );
	return val;
}
#else
#error "by design a function!"
#endif

/*=============================================================================
 * function name:  OHA_GET_SYS_UPTIME
 *
 * function:       The OHA_GET_SYS_UPTIME function retrieves the number of 10 milliseconds
 *                 that have elapsed since the system was started.
 *
 * documentation:  LSA_OHA_RoughDesign.doc
 *
 *===========================================================================*/
#ifndef OHA_GET_SYS_UPTIME
LSA_UINT32  OHA_SYSTEM_OUT_FCT_ATTR OHA_GET_SYS_UPTIME( LSA_VOID )
{
	LSA_UINT32 dwStart = PSI_GET_TICKS_1MS();  /* in milliseconds */
	return ( dwStart/10 );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++  DCP-Server  +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_IND
 * function:       Trace incoming SET Requests
 *                 Called before calling "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 *  ReqPtr:     -> SET request data
 *                 LSA_INT      ReqLen:     Length of request
 * return value:   LSA_UINT8   Reason:      The first Result-field of the
 *                                          SET-Response will be set to this value.
 *
 *  OHA_DCP_REASON_OK (no error)        Positive response, accept a SET and deliver parameters
 *  OHA_DCP_REASON_NO_DATABASE          Option not supported
 *  OHA_DCP_REASON_NO_DATASET           uboption not supported or no dataset available
 *  OHA_DCP_REASON_DS_NOT_SET (default) Suboption not set (local reasons)
 *  OHA_DCP_REASON_NO_RESOURCE          (temporary) resource error (server)
 *  OHA_DCP_REASON_NOT_POSSIBLE         SET not possible (local reasons)
 *  OHA_DCP_REASON_NOT_APPLICABLE       SET not possible (application operation)
 *
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_IND
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR OHA_REPORT_DCP_SET_IND(
	LSA_UINT16                          NicId,
	OHA_UPPER_DCP_INDICATION_PTR_TYPE   pDcpInd,
	LSA_UINT8   OHA_LOCAL_MEM_ATTR*     ReqPtr,
	LSA_INT                             ReqLen )
{
	LSA_UNUSED_ARG( NicId );
	LSA_UNUSED_ARG( pDcpInd );
	LSA_UNUSED_ARG( ReqPtr );
	LSA_UNUSED_ARG( ReqLen );

	return OHA_DCP_REASON_OK;
}
#endif

/*=============================================================================
 * function name:  OHA_REPORT_DCP_SET_RES
 * function:       Trace End of SET Request
 *                 Called after calling all "out functions" to write datasets
 * parameters:     OHA_DCP_INDICATION_TYPE DcpInd: DCP-Indication part
 *                 (see documentation of OHA_OPC_DCP_INDICATION)
 *                 LSA_UINT8 * RspPtr:      -> SET response data
 *                 LSA_INT     RspLen:      Length of response
 * return value:   -
 *===========================================================================*/
#ifndef OHA_REPORT_DCP_SET_RES
LSA_VOID OHA_SYSTEM_OUT_FCT_ATTR OHA_REPORT_DCP_SET_RES(
	LSA_UINT16                     NicId,
	OHA_DCP_INDICATION_TYPE        DcpInd,
	LSA_UINT8 OHA_LOCAL_MEM_ATTR * RspPtr,
	LSA_INT                        RspLen )
{
	LSA_UNUSED_ARG( NicId );
	LSA_UNUSED_ARG( DcpInd );
	LSA_UNUSED_ARG( RspPtr );
	LSA_UNUSED_ARG( RspLen );
}
#endif
