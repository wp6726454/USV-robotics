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
/*  F i l e               &F: lldp_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LLDP using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LLDP_MODULE_ID     32019
#define LTRC_ACT_MODUL_ID  32019

#include "psi_int.h"

#include "lldp_inc.h"
#include "lldp_int.h"
#include "lldp_trc.h"

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

/*===========================================================================*/

void lldp_psi_startstop( int start )
{
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{ 
		rsp = lldp_init();
		LLDP_ASSERT_FALSE( rsp == LLDP_RSP_OK );
	}
	else
	{ 
		rsp = lldp_undo_init();
		LLDP_ASSERT_FALSE( rsp == LLDP_RSP_OK );
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_GET_PATH_INFO
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_GET_PATH_INFO(
	LSA_UINT16           LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE     LLDP_LOCAL_MEM_ATTR * sys_ptr_ptr,
	LLDP_DETAIL_PTR_TYPE LLDP_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_lldp_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_RELEASE_PATH_INFO
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_RELEASE_PATH_INFO(
	LSA_UINT16           LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE                           sys_ptr,
	LLDP_DETAIL_PTR_TYPE                       detail_ptr )
{
	*ret_val_ptr = psi_lldp_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_ALLOC_TIMER
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_ALLOC_TIMER(
	LSA_UINT16 LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 LLDP_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                       timer_type,
	LSA_UINT16                       time_base )
{
	PSI_ALLOC_TIMER_TGROUP1( ret_val_ptr, timer_id_ptr, timer_type, time_base, lldp_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_START_TIMER
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_START_TIMER(
	LSA_UINT16       LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                             timer_id,
	LSA_USER_ID_TYPE                       user_id,
	LSA_UINT16                             time )
{
	PSI_START_TIMER_TGROUP1( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_STOP_TIMER
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_STOP_TIMER(
	LSA_UINT16 LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_STOP_TIMER_TGROUP1( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_FREE_TIMER
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_FREE_TIMER(
	LSA_UINT16 LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_FREE_TIMER_TGROUP1( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_FATAL_ERROR
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_FATAL_ERROR(
	LSA_UINT16                length,
	LLDP_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "lldp", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_ENTER
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_ENTER( LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EXIT
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EXIT( LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_ALLOC_LOCAL_MEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_ALLOC_LOCAL_MEM(
	LLDP_LOCAL_MEM_PTR_TYPE LLDP_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT32                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"LLDP_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_LLDP, PSI_MTYPE_LOCAL_MEM );

	LLDP_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "LLDP_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_FREE_LOCAL_MEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_FREE_LOCAL_MEM(
	LSA_UINT16              LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_LOCAL_MEM_PTR_TYPE                       local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "LLDP_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_LLDP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_RQB_ERROR
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_RQB_ERROR(
	LLDP_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_LLDP, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_MEMSET_LOCAL
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_MEMSET_LOCAL(
	LLDP_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_MEMCOPY_COMMON
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_MEMCOPY_COMMON(
	LLDP_COMMON_MEM_PTR_TYPE pDst,
	LLDP_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32               Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_MEMCMP_COMMON
LSA_BOOL LLDP_SYSTEM_OUT_FCT_ATTR LLDP_MEMCMP_COMMON(
	LLDP_COMMON_MEM_PTR_TYPE pBuf1,
	LLDP_COMMON_MEM_PTR_TYPE pBuf2,
	LSA_UINT32               Length )
{
	return ( PSI_MEMCMP( pBuf1, pBuf2, (LSA_UINT)Length ) == 0 ? LSA_TRUE : LSA_FALSE );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_REQUEST_UPPER_DONE
LSA_VOID  LLDP_SYSTEM_OUT_FCT_ATTR  LLDP_REQUEST_UPPER_DONE(
	LLDP_UPPER_CALLBACK_FCT_PTR_TYPE lldp_request_upper_done_ptr,
	LLDP_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)lldp_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_ALLOC_UPPER_RQB_LOCAL(
	LLDP_UPPER_RQB_PTR_TYPE LLDP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_UINT16                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( length != sizeof(LLDP_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"LLDP_ALLOC_UPPER_RQB_LOCAL(): invalid length != sizeof(LLDP_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_LLDP, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "LLDP_ALLOC_UPPER_RQB_LOCAL(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_FREE_UPPER_RQB_LOCAL
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_FREE_UPPER_RQB_LOCAL(
	LSA_UINT16              LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_UPPER_RQB_PTR_TYPE                       upper_rqb_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "LLDP_FREE_UPPER_RQB_LOCAL(): ptr(%08x)", upper_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, 0, LSA_COMP_ID_LLDP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_DO_TIMER_REQUEST
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_DO_TIMER_REQUEST(
	LLDP_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_OPEN_CHANNEL_LOWER
LSA_VOID LLDP_EDD_LOWER_OUT_FCT_ATTR LLDP_EDD_OPEN_CHANNEL_LOWER(
	LLDP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_LLDP, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_open_fct( pSys->comp_id_lower );

	if ( mbx_id == PSI_MBX_ID_EDDI_NRT_ORG )
	{
		psi_request_direct_start( mbx_id, p_func, lower_rqb_ptr );
	}
	else
	{
		psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID LLDP_EDD_LOWER_OUT_FCT_ATTR LLDP_EDD_CLOSE_CHANNEL_LOWER(
	LLDP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_LLDP, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_close_fct( pSys->comp_id_lower );

	if ( mbx_id == PSI_MBX_ID_EDDI_NRT_ORG )
	{
		psi_request_direct_start( mbx_id, p_func, lower_rqb_ptr );
	}
	else
	{
		psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_REQUEST_LOWER
LSA_VOID LLDP_EDD_LOWER_OUT_FCT_ATTR LLDP_EDD_REQUEST_LOWER(
	LLDP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	LLDP_LOWER_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"LLDP_EDD_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u) service(%u)",
		lower_rqb_ptr,
		LLDP_EDD_RQB_GET_HANDLE(lower_rqb_ptr),
		LLDP_EDD_RQB_GET_OPCODE(lower_rqb_ptr),
		LLDP_EDD_RQB_GET_SERVICE(lower_rqb_ptr) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and request function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_LLDP, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_request_fct( pSys->comp_id_lower );

	if ( mbx_id == PSI_MBX_ID_EDDI_NRT_ORG )
	{
		psi_request_direct_start( mbx_id, p_func, lower_rqb_ptr );
	}
	else
	{
		psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_ALLOC_LOWER_RQB
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_ALLOC_LOWER_RQB(
	LLDP_EDD_LOWER_RQB_PTR_TYPE LLDP_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

    if ( length != sizeof(LLDP_EDD_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"LLDP_EDD_ALLOC_LOWER_RQB(): invalid length != sizeof(LLDP_EDD_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }
	
	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_RQB_EDD );
	
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EDD_FREE_LOWER_RQB
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_FREE_LOWER_RQB(
	LSA_UINT16                  LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_EDD_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_ALLOC_LOWER_MEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_ALLOC_LOWER_MEM(
	LLDP_EDD_LOWER_MEM_PTR_TYPE LLDP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"LLDP_EDD_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_MEM_EDD );
	
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EDD_FREE_LOWER_MEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_FREE_LOWER_MEM(
	LSA_UINT16                  LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_ALLOC_LOWER_TXMEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_ALLOC_LOWER_TXMEM(
	LLDP_EDD_LOWER_MEM_PTR_TYPE LLDP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"LLDP_EDD_ALLOC_LOWER_TXMEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
	    LSA_UNUSED_ARG( user_id );
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }

	LLDP_ASSERT_FALSE( length <= EDD_FRAME_BUFFER_LENGTH );

	psi_edd_alloc_nrt_tx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_LLDP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_ALLOC_LOWER_TXMEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EDD_FREE_LOWER_TXMEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_FREE_LOWER_TXMEM(
	LSA_UINT16                  LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_FREE_LOWER_TXMEM(): ptr(%08x)", lower_mem_ptr );

	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_LLDP );
}
#else
#error "by design a function!"
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef LLDP_EDD_ALLOC_LOWER_RXMEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_ALLOC_LOWER_RXMEM(
	LLDP_EDD_LOWER_MEM_PTR_TYPE LLDP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"LLDP_EDD_ALLOC_LOWER_RXMEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *lower_mem_ptr_ptr = LSA_NULL;
	    LSA_UNUSED_ARG( user_id );
        PSI_FATAL_COMP( LSA_COMP_ID_LLDP, LLDP_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_rx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_LLDP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_ALLOC_LOWER_RXMEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef LLDP_EDD_FREE_LOWER_RXMEM
LSA_VOID LLDP_SYSTEM_OUT_FCT_ATTR LLDP_EDD_FREE_LOWER_RXMEM(
	LSA_UINT16                  LLDP_LOCAL_MEM_ATTR * ret_val_ptr,
	LLDP_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	LLDP_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "LLDP_EDD_FREE_LOWER_RXMEM(): ptr(%08x)", lower_mem_ptr );

	psi_edd_free_nrt_rx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_LLDP );
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
