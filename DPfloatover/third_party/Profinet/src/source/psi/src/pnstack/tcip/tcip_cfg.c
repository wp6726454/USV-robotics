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
/*  F i l e               &F: tcip_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of TCIP using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define TCIP_MODULE_ID     32024
#define LTRC_ACT_MODUL_ID  32024

#include "psi_int.h"

#if ( PSI_CFG_USE_HIF_HD == 1 )
#include "hif_cfg.h"
#include "hif_usr.h"
#endif

#include "tcip_inc.h" 

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*----------------------------------------------------------------------------*/

static LSA_UINT16 tcip_enter_exit_interniche_id = PSI_LOCK_ID_INVALID;

/*----------------------------------------------------------------------------*/

void tcip_psi_startstop( int start )
{
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		LSA_UINT16 rc;

		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &tcip_enter_exit_interniche_id );
		TCIP_ASSERT( rsp == LSA_RET_OK );

		rc = tcip_init();
		TCIP_ASSERT( rc == LSA_RET_OK );
	}
	else 
	{
		LSA_UINT16 rc;

		rc = tcip_undo_init();
		TCIP_ASSERT( rc == LSA_RET_OK );

		PSI_FREE_REENTRANCE_LOCK( &rsp, tcip_enter_exit_interniche_id );
		TCIP_ASSERT( rc == LSA_RET_OK );

		tcip_enter_exit_interniche_id = PSI_LOCK_ID_INVALID;
	}
}

/*----------------------------------------------------------------------------*/

#ifndef TCIP_GET_PATH_INFO
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_GET_PATH_INFO(
	LSA_UINT16           TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE     TCIP_LOCAL_MEM_ATTR * sys_ptr_ptr,
	TCIP_DETAIL_PTR_TYPE TCIP_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_tcip_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef TCIP_RELEASE_PATH_INFO
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_RELEASE_PATH_INFO(
	LSA_UINT16           TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE                           sys_ptr,
	TCIP_DETAIL_PTR_TYPE                       detail_ptr )
{
	*ret_val_ptr = psi_tcip_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	System Layer (prototypes in tcip_sys.h)
//----------------------------------------------------------------------------*/

#ifndef TCIP_ALLOC_LOCAL_MEM
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_ALLOC_LOCAL_MEM(
	TCIP_LOCAL_MEM_PTR_TYPE TCIP_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT16                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"TCIP_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, TCIP_MODULE_ID, 0 );
        return;
}

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FREE_LOCAL_MEM
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_FREE_LOCAL_MEM(
	LSA_UINT16              TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	TCIP_LOCAL_MEM_PTR_TYPE                       local_mem_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_TCIP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMSET
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT val,
	LSA_UINT len )
{
	PSI_MEMSET( ptr, val, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMCPY
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len )
{
	PSI_MEMCPY( dst, src, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMMOVE
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_MEMMOVE(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT len )
{
	PSI_MEMMOVE( dst, src, len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_MEMCMP
LSA_INT TCIP_SYSTEM_OUT_FCT_ATTR TCIP_MEMCMP(
	LSA_VOID_PTR_TYPE buf1,
	LSA_VOID_PTR_TYPE buf2,
	LSA_UINT len )
{
	return ( PSI_MEMCMP( buf1, buf2, len ) );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_ALLOC_TIMER
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_ALLOC_TIMER(
	LSA_UINT16        TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_TIMER_ID_TYPE TCIP_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                              timer_type,
	LSA_UINT16                              time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, tcip_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_START_TIMER
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_START_TIMER(
	LSA_UINT16        TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_TIMER_ID_TYPE                       timer_id,
	LSA_USER_ID_TYPE                        user_id,
	LSA_UINT16                              time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_STOP_TIMER
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_STOP_TIMER(
	LSA_UINT16        TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_TIMER_ID_TYPE                       timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FREE_TIMER
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_FREE_TIMER(
	LSA_UINT16        TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_TIMER_ID_TYPE                       timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_ARP_RESPONSE_RECEIVED
LSA_VOID TCIP_ARP_RESPONSE_RECEIVED(
	LSA_UINT16                 group_id, /* must have the same value as NARE_CDB_PATH_PARAMS_TYPE::NICId */
	EDD_UPPER_MEM_U8_PTR_TYPE  frame_buf,
	LSA_UINT16                 frame_len )
{
	// NOTE must be empty, because the ARP sniffing is done on HD level (see nare_cfg.c)
	LSA_UNUSED_ARG( group_id );
	LSA_UNUSED_ARG( frame_buf );
	LSA_UNUSED_ARG( frame_len );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FATAL_ERROR
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_FATAL_ERROR(
	LSA_UINT16                length,
	TCIP_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "tcip", length, error_ptr );
}
#else
#pragma TCIP_MESSAGE("TCIP_FATAL_ERROR redefined by compiler switch to: " TCIP_STRINGIFY(TCIP_FATAL_ERROR))
#endif

/*------------------------------------------------------------------------------
//	Upper Layer (prototypes in tcip_usr.h)
//----------------------------------------------------------------------------*/

#ifndef TCIP_REQUEST_UPPER_DONE
LSA_VOID  TCIP_UPPER_OUT_FCT_ATTR TCIP_REQUEST_UPPER_DONE(
	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE tcip_request_upper_done_ptr,
	TCIP_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)tcip_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_ALLOC_UPPER_RQB
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_ALLOC_UPPER_RQB(
	TCIP_UPPER_RQB_PTR_TYPE TCIP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                              user_id,
	LSA_UINT16                                    length,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(TCIP_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"TCIP_ALLOC_UPPER_RQB(): invalid length != sizeof(TCIP_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, TCIP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_ALLOC_UPPER_RQB(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_FREE_UPPER_RQB
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_FREE_UPPER_RQB(
	LSA_UINT16              TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	TCIP_UPPER_RQB_PTR_TYPE                       upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                              sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_FREE_UPPER_RQB(): ptr(%08x)", upper_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef TCIP_REQUEST_LOCAL
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_REQUEST_LOCAL(
	TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_TCIP, (PSI_REQUEST_FCT)tcip_request, upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef LOCK_NET_RESOURCE
void LOCK_NET_RESOURCE( int res )
{
	LSA_UNUSED_ARG( res );
	PSI_ENTER_REENTRANCE_LOCK( tcip_enter_exit_interniche_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef UNLOCK_NET_RESOURCE
void UNLOCK_NET_RESOURCE( int res )
{
	LSA_UNUSED_ARG( res );
	PSI_EXIT_REENTRANCE_LOCK( tcip_enter_exit_interniche_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

void tcp_sleep( void * event )
{
	/* see nsref_complete.htm, chapter "Net Resource Method" */
	
	TCIP_FATAL1((LSA_UINT32)event);  /* must not be called for NBIO */
}

/*---------------------------------------------------------------------------*/

void tcp_wakeup(void * _event)
{
	/* see nsref_complete.htm, chapter "Net Resource Method" */
	LSA_UNUSED_ARG(_event);  /* is called by stack although NBIO is used */
}

/*===========================================================================*/

#ifndef TCIP_EXCHANGE_LONG
long  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EXCHANGE_LONG(
	long volatile *ptr,
	long val )
{
	return( PSI_EXCHANGE_LONG( ptr, val ) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_OPEN_CHANNEL_LOWER
LSA_VOID TCIP_EDD_LOWER_OUT_FCT_ATTR TCIP_EDD_OPEN_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and open function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_open_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID TCIP_EDD_LOWER_OUT_FCT_ATTR TCIP_EDD_CLOSE_CHANNEL_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and close function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_close_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_REQUEST_LOWER
LSA_VOID TCIP_EDD_LOWER_OUT_FCT_ATTR TCIP_EDD_REQUEST_LOWER(
	TCIP_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (EDDx is located on HD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, lower_rqb_ptr );
    #else
	{
	    // get the mailbox ID and request function for EDDx
	    LSA_UINT16      mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_TCIP, sys_ptr );
	    PSI_REQUEST_FCT p_func = edd_get_request_fct( pSys->comp_id_lower );

	    // post the message to EDDx (without HIF EDDx is located in same system)
	    psi_request_start( mbx_id, p_func, lower_rqb_ptr );
	}
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_ALLOC_LOWER_RQB
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_ALLOC_LOWER_RQB(
	TCIP_EDD_LOWER_RQB_PTR_TYPE TCIP_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(TCIP_EDD_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"TCIP_EDD_ALLOC_LOWER_RQB(): invalid length != sizeof(TCIP_EDD_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, TCIP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_RQB_EDD );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_EDD_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_FREE_LOWER_RQB
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_FREE_LOWER_RQB(
	LSA_UINT16                  TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	TCIP_EDD_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_EDD_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_ALLOC_LOWER_MEM
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_ALLOC_LOWER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE TCIP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"TCIP_EDD_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, TCIP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_MEM_EDD );
	
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_EDD_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_FREE_LOWER_MEM
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_FREE_LOWER_MEM(
	LSA_UINT16                  TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	TCIP_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "TCIP_EDD_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_ALLOC_BUFFER_MEM
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_ALLOC_BUFFER_MEM(
	TCIP_EDD_LOWER_MEM_PTR_TYPE TCIP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
    LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"TCIP_EDD_ALLOC_BUFFER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_TCIP, TCIP_MODULE_ID, 0 );
        return;
    }

    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
        PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    TCIP_ASSERT( pSys != LSA_NULL );

        if ( pSys->runs_on_level_basic )
	    {
		    PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, null_usr_id, length, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
	    }
	    else
	    {
		    EDD_ALLOC_DEV_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_TCIP );
	    }
	}
    #endif
}
#endif

/*===========================================================================*/

#ifndef TCIP_EDD_FREE_BUFFER_MEM
LSA_VOID TCIP_SYSTEM_OUT_FCT_ATTR TCIP_EDD_FREE_BUFFER_MEM(
	LSA_UINT16                  TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
	TCIP_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
    #if defined (HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined (HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
    #else
    {
	    PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	    TCIP_ASSERT( pSys != LSA_NULL );

        if ( pSys->runs_on_level_basic )
	    {
		    PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP, PSI_MTYPE_NRT_MEM );
	    }
	    else
	    {
		    EDD_FREE_DEV_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_TCIP );
	    }
	}
    #endif
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
