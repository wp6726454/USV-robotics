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
/*  F i l e               &F: clrpc_cfg.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CLRPC using PSI.                        */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    32030
#define LTRC_ACT_MODUL_ID  32030

#include "psi_int.h"

#include "clrpc_inc.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)

/*----------------------------------------------------------------------------*/

void clrpc_psi_startstop( int start )
{
	if ( start )
	{
		static CLRPC_INIT_TYPE clrpc_init_param; // not defined by LSA if necessarily static. static is safe.
		LSA_UINT16             rc;

		clrpc_init_param.oneshot_lsa_time_base   = LSA_TIME_BASE_100MS;  
		clrpc_init_param.oneshot_lsa_time_factor = 1;                    

		rc = clrpc_init( &clrpc_init_param );
		CLRPC_ASSERT( rc == LSA_RET_OK );
	}
	else 
	{
		LSA_UINT16 rc;

		rc = clrpc_undo_init();
		CLRPC_ASSERT( rc == LSA_RET_OK );
	}
}

/*-----------------------------------------------------------------------------
//	System Layer (prototypes in clrpc_sys.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_GET_PATH_INFO
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_GET_PATH_INFO(
	LSA_UINT16              CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE        CLRPC_LOCAL_MEM_ATTR * sys_ptr_ptr,
	CLRPC_DETAIL_PTR_TYPE   CLRPC_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                              path )
{
	*ret_val_ptr = psi_clrpc_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_RELEASE_PATH_INFO
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_RELEASE_PATH_INFO(
	LSA_UINT16             CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr,
	CLRPC_DETAIL_PTR_TYPE                         detail_ptr )
{
	*ret_val_ptr = psi_clrpc_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_LOCAL_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_LOCAL_MEM(
	CLRPC_LOCAL_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT16                                      length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CLRPC_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CLRPC, CLRPC_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_LOCAL_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_LOCAL_MEM(
	LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	CLRPC_LOCAL_MEM_PTR_TYPE                           local_mem_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr,0 , LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_TIMER
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_TIMER(
	LSA_UINT16 CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 CLRPC_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                        timer_type,
	LSA_UINT16                        time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr,  timer_id_ptr,  timer_type, time_base, clrpc_timeout );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_START_TIMER
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_START_TIMER(
	LSA_UINT16        CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_UINT16                                timer_id,
	LSA_USER_ID_TYPE                          user_id,
	LSA_UINT16                                time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_STOP_TIMER
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_STOP_TIMER(
	LSA_UINT16 CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                        timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_TIMER
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_TIMER(
	LSA_UINT16 CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                        timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifdef CLRPC_ENTER
#error "who defined it? not used!"
#endif

#ifdef CLRPC_EXIT
#error "who defined it? not used!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_FATAL_ERROR
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FATAL_ERROR(
	LSA_UINT16                 length,
	CLRPC_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "clrpc", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*-----------------------------------------------------------------------------
//	Upper Layer (prototypes in clrpc_usr.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_REQUEST_UPPER_DONE
LSA_VOID CLRPC_UPPER_OUT_FCT_ATTR CLRPC_REQUEST_UPPER_DONE(
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE  clrpc_request_upper_done_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                   sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)clrpc_request_upper_done_ptr, (struct psi_header*)upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_ALLOC_UPPER_RQB
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_UPPER_RQB(
	CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
    if ( length != sizeof(CLRPC_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CLRPC_ALLOC_UPPER_RQB(): invalid length != sizeof(CLRPC_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CLRPC, CLRPC_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_UPPER_RQB
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_UPPER_RQB(
	LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	CLRPC_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_UPPER_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_UPPER_MEM(
	CLRPC_UPPER_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CLRPC_ALLOC_UPPER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CLRPC, CLRPC_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_UPPER_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_UPPER_MEM(
	LSA_UINT16                 CLRPC_LOCAL_MEM_ATTR *  ret_val_ptr,
	CLRPC_UPPER_MEM_PTR_TYPE                           upper_mem_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_mem_ptr, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_UPPER_RQB_ERROR
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_UPPER_RQB_ERROR(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	PSI_RQB_ERROR( LSA_COMP_ID_CLRPC, 0, upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_REQUEST_LOCAL
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_REQUEST_LOCAL(
	CLRPC_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, (LSA_VOID_PTR_TYPE)upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*-----------------------------------------------------------------------------
//	Lower Layer (prototypes in clrpc_low.h)
//---------------------------------------------------------------------------*/

#ifndef CLRPC_OPEN_CHANNEL_LOWER
LSA_VOID CLRPC_LOWER_OUT_FCT_ATTR CLRPC_OPEN_CHANNEL_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CLRPC_ASSERT( pSys != 0 );

	CLRPC_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel,(LSA_VOID_PTR_TYPE) lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_CLOSE_CHANNEL_LOWER
LSA_VOID CLRPC_LOWER_OUT_FCT_ATTR CLRPC_CLOSE_CHANNEL_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CLRPC_ASSERT( pSys != 0 );

	CLRPC_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, (LSA_VOID_PTR_TYPE)lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_REQUEST_LOWER
LSA_VOID CLRPC_LOWER_OUT_FCT_ATTR CLRPC_REQUEST_LOWER(
	CLRPC_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CLRPC_ASSERT( pSys != 0 );

	CLRPC_ASSERT( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, (LSA_VOID_PTR_TYPE)lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_LOWER_RQB
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_LOWER_RQB(
	CLRPC_LOWER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
    if ( length != sizeof(CLRPC_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CLRPC_ALLOC_LOWER_RQB(): invalid length != sizeof(CLRPC_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CLRPC, CLRPC_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_RQB_SOCK );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_LOWER_RQB
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_LOWER_RQB(
	LSA_UINT16               CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	CLRPC_LOWER_RQB_PTR_TYPE                        lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_RQB_SOCK );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_ALLOC_LOWER_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_ALLOC_LOWER_MEM(
	CLRPC_LOWER_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                user_id,
	LSA_UINT16                                      length,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CLRPC_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CLRPC, CLRPC_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_MEM_SOCK );
}
#else
#error "by design a function!"
#endif

#ifndef CLRPC_FREE_LOWER_MEM
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_FREE_LOWER_MEM(
	LSA_UINT16               CLRPC_LOCAL_MEM_ATTR * ret_val_ptr,
	CLRPC_LOWER_MEM_PTR_TYPE                        lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOWER_MEM_SOCK );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef CLRPC_LOWER_RQB_ERROR
LSA_VOID CLRPC_SYSTEM_OUT_FCT_ATTR CLRPC_LOWER_RQB_ERROR(
	CLRPC_LOWER_RQB_PTR_TYPE lower_rqb_ptr )
{
	PSI_RQB_ERROR( LSA_COMP_ID_CLRPC, LSA_COMP_ID_SOCK, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
