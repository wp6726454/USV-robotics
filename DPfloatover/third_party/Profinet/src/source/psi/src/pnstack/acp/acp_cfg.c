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
/*  F i l e               &F: acp_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component ACP                       */
/*                                                                           */
/*****************************************************************************/

#define ACP_MODULE_ID      32001
#define LTRC_ACT_MODUL_ID  32001

#include "psi_int.h"

#include "acp_int.h"

ACP_FILE_SYSTEM_EXTENSION(ACP_MODULE_ID)

/*----------------------------------------------------------------------------*/

int acp_semicolon_missing = 0;

/*----------------------------------------------------------------------------*/

LSA_VOID acp_psi_startstop( LSA_INT start )
{
	LSA_UINT16 rc;

	if ( start )
	{
		ACP_INIT_TYPE init;

		init.rta_time_base   = LSA_TIME_BASE_10MS;
		init.rta_time_factor = 1;

		rc = acp_init( &init );
		ACP_ASSERT( rc == LSA_RET_OK );
	}
	else
	{
		rc = acp_undo_init();
		ACP_ASSERT( rc == LSA_RET_OK );
	}
}

/*------------------------------------------------------------------------------
//	Upper Layer
//----------------------------------------------------------------------------*/

#ifndef ACP_GET_PATH_INFO
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_GET_PATH_INFO(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *           ret_val_ptr,
    LSA_SYS_PTR_TYPE  ACP_LOCAL_MEM_ATTR *     sys_ptr_ptr,
    ACP_DETAIL_PTR_TYPE  ACP_LOCAL_MEM_ATTR *  detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_acp_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_RELEASE_PATH_INFO
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_RELEASE_PATH_INFO(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr,
    ACP_DETAIL_PTR_TYPE              detail_ptr )
{
	*ret_val_ptr = psi_acp_release_path_info( sys_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_UPPER_RQB
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_UPPER_RQB(
    ACP_UPPER_RQB_PTR_TYPE  ACP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                             user_id,
    LSA_UINT32                                   length,
    LSA_SYS_PTR_TYPE                             sys_ptr )
{
    if ( length != sizeof(ACP_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_UPPER_RQB(): invalid length != sizeof(ACP_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_UPPER_RQB
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_UPPER_RQB(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    ACP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, &sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_UPPER_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_UPPER_MEM(
    ACP_UPPER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                            user_id,
    LSA_UINT32                                  length,
    LSA_SYS_PTR_TYPE                            sys_ptr )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_UPPER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_UPPER_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_UPPER_MEM(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    ACP_UPPER_MEM_PTR_TYPE           upper_mem_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr ) 
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_mem_ptr, &sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_REQUEST_LOCAL
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_REQUEST_LOCAL(
    ACP_UPPER_RQB_PTR_TYPE upper_rqb_ptr ) 
{
	psi_request_local (PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_request, (LSA_VOID_PTR_TYPE *)upper_rqb_ptr);
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	Lower Layer
//----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_LOWER_RQB
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_LOWER_RQB(
    ACP_LOWER_RQB_PTR_TYPE ACP_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                            user_id,
    LSA_UINT32                                  length,
    LSA_SYS_PTR_TYPE                            sys_ptr )
{
    if ( length != sizeof(ACP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_LOWER_RQB(): invalid length != sizeof(ACP_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_LOWER_RQB
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_LOWER_RQB(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_RQB_PTR_TYPE            lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_LOWER_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_LOWER_MEM(
    ACP_LOWER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                            user_id,
    LSA_UINT32                                  length,
    LSA_SYS_PTR_TYPE                            sys_ptr )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_LOWER_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_LOWER_MEM(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, &sys_ptr, LSA_COMP_ID_ACP, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif


/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_NRT_SEND_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_NRT_SEND_MEM(
    ACP_LOWER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    LSA_SYS_PTR_TYPE                            sys_ptr )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_NRT_SEND_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_tx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_ACP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_NRT_SEND_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_NRT_SEND_MEM(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE           lower_mem_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_ACP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_NRT_RECV_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_NRT_RECV_MEM(
    ACP_LOWER_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    LSA_SYS_PTR_TYPE                            sys_ptr )
{
    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_NRT_RECV_MEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_rx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_ACP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_NRT_RECV_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_NRT_RECV_MEM(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    ACP_LOWER_MEM_PTR_TYPE            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                  sys_ptr )
{
	psi_edd_free_nrt_rx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_ACP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_OPEN_CHANNEL_LOWER

LSA_VOID ACP_LOWER_OUT_FCT_ATTR ACP_OPEN_CHANNEL_LOWER(
    ACP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE       sys_ptr )
{
	LSA_UINT16       mbx_id;
	PSI_REQUEST_FCT  p_func;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	ACP_ASSERT( pSys != 0 );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_ACP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_open_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_CLOSE_CHANNEL_LOWER
LSA_VOID ACP_LOWER_OUT_FCT_ATTR ACP_CLOSE_CHANNEL_LOWER(
    ACP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
    LSA_SYS_PTR_TYPE       sys_ptr )
{
	LSA_UINT16       mbx_id;
	PSI_REQUEST_FCT  p_func;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	ACP_ASSERT( pSys != 0 );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_ACP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_close_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_REQUEST_LOWER
LSA_VOID ACP_LOWER_OUT_FCT_ATTR ACP_REQUEST_LOWER(
    ACP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr,
    LSA_SYS_PTR_TYPE        sys_ptr )
{
	LSA_UINT16       mbx_id;
	PSI_REQUEST_FCT  p_func;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	ACP_ASSERT( pSys != 0 );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and request function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_ACP, sys_ptr );
	p_func = (PSI_REQUEST_FCT)edd_get_request_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*------------------------------------------------------------------------------
//	System Layer
//----------------------------------------------------------------------------*/

#ifndef ACP_REQUEST_UPPER_DONE
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_REQUEST_UPPER_DONE(
    ACP_UPPER_CALLBACK_FCT_PTR_TYPE  acp_request_upper_done_ptr,
    ACP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)acp_request_upper_done_ptr, upper_rqb_ptr, (LSA_VOID_PTR_TYPE *)sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_LOCAL_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_LOCAL_MEM(
    ACP_LOCAL_MEM_PTR_TYPE ACP_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
    LSA_UINT32                                  length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"ACP_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_ACP, ACP_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_LOCAL_MEM
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_LOCAL_MEM(
    LSA_UINT16 ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    ACP_LOCAL_MEM_PTR_TYPE          local_mem_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_ACP, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_MEMCPY
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT          length )
{
	PSI_MEMCPY( dst, src, length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ALLOC_TIMER
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ALLOC_TIMER(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *         ret_val_ptr,
    LSA_TIMER_ID_TYPE  ACP_LOCAL_MEM_ATTR *  timer_id_ptr,
    LSA_UINT16                               timer_type,
    LSA_UINT16                               time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, acp_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_START_TIMER
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_START_TIMER(
    LSA_UINT16  ACP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_TIMER_ID_TYPE                 timer_id,
    LSA_USER_ID_TYPE                  user_id,
    LSA_UINT16                        time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_STOP_TIMER
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_STOP_TIMER(
    LSA_UINT16 ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_TIMER_ID_TYPE               timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FREE_TIMER
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FREE_TIMER(
    LSA_UINT16 ACP_LOCAL_MEM_ATTR * ret_val_ptr,
    LSA_TIMER_ID_TYPE               timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_ENTER
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_ENTER(
    LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_EXIT
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_EXIT(
    LSA_VOID )
{
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_FATAL_ERROR
LSA_VOID ACP_SYSTEM_OUT_FCT_ATTR ACP_FATAL_ERROR(
    LSA_UINT16               length,
    ACP_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "acp", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_PUT8_HTON
LSA_VOID ACP_LOCAL_FCT_ATTR ACP_PUT8_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset,
    LSA_UINT8         val )
{
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;
	ptr[offset] = val;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_GET8_NTOH
LSA_UINT8 ACP_LOCAL_FCT_ATTR ACP_GET8_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset )
{
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;
	return( ptr[offset] );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_PUT16_HTON
LSA_VOID ACP_LOCAL_FCT_ATTR ACP_PUT16_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset,
    LSA_UINT16        val )
{
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;

	ptr += offset;
	val = PSI_HTON16( val );
	ACP_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_GET16_NTOH
LSA_UINT16 ACP_LOCAL_FCT_ATTR ACP_GET16_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset )
{
	LSA_UINT16                      val;
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;

	ptr += offset;
	ACP_MEMCPY( &val, ptr, sizeof(val) );
	val = PSI_NTOH16( val );
	return val;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_PUT32_HTON
LSA_VOID ACP_LOCAL_FCT_ATTR ACP_PUT32_HTON( /* put and convert from host byte order to network byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset,
	LSA_UINT32        val )
{
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;

	ptr += offset;
	val = PSI_HTON32( val );
	ACP_MEMCPY( ptr, &val, sizeof(val) );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef ACP_GET32_NTOH
LSA_UINT32 ACP_LOCAL_FCT_ATTR ACP_GET32_NTOH( /* get and convert from network byte order to host byte order */
    LSA_VOID_PTR_TYPE base,
    LSA_INT           offset )
{
	LSA_UINT32                      val;
	LSA_UINT8 ACP_COMMON_MEM_ATTR * ptr = (LSA_UINT8 *)base;

	ptr += offset;
	ACP_MEMCPY( &val, ptr, sizeof(val) );
	val = PSI_NTOH32( val );
	return val;
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
