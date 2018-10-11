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
/*  F i l e               &F: nare_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of NARE using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define NARE_MODULE_ID     32021
#define LTRC_ACT_MODUL_ID  32021

#include "psi_int.h"
#include "psi_hd.h"

#include "nare_inc.h"
#include "nare_int.h"  // needed for NARE_ASSERT_xx

#include "nare_trc.h"
#include "nare_dbg.h"  // NARE_ASSERT_xx

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

/*----------------------------------------------------------------------------*/

static LSA_INT16  nare_enter = 0;

/*----------------------------------------------------------------------------*/

void nare_psi_startstop( int start )
{
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{ 	
		rsp = nare_init();
		NARE_ASSERT_FALSE( rsp == NARE_RSP_OK );
	}
	else
	{ 
		rsp = nare_undo_init();
		NARE_ASSERT_FALSE( rsp == NARE_RSP_OK );
	}
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_GET_PATH_INFO
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_GET_PATH_INFO(
	LSA_UINT16           NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE     NARE_LOCAL_MEM_ATTR * sys_ptr_ptr,
	NARE_DETAIL_PTR_TYPE NARE_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_nare_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef NARE_RELEASE_PATH_INFO
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_RELEASE_PATH_INFO(
	LSA_UINT16           NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE                           sys_ptr,
	NARE_DETAIL_PTR_TYPE                       detail_ptr )
{
	*ret_val_ptr = psi_nare_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ALLOC_TIMER
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_ALLOC_TIMER(
	LSA_UINT16 NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 NARE_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                       timer_type,
	LSA_UINT16                       time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, nare_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_START_TIMER
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_START_TIMER(
	LSA_UINT16       NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                             timer_id,
	LSA_USER_ID_TYPE                       user_id,
	LSA_UINT16                             time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_STOP_TIMER
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_STOP_TIMER(
	LSA_UINT16 NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FREE_TIMER
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_FREE_TIMER(
	LSA_UINT16 NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FATAL_ERROR
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_FATAL_ERROR(
	LSA_UINT16                length,
	NARE_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "nare", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_RQB_ERROR
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_RQB_ERROR(
	NARE_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_NARE, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ENTER
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_ENTER( LSA_VOID )
{
	if ( nare_enter != 0 ) 
	{
		NARE_FATAL( 0 );
	}
	nare_enter += 1;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EXIT
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EXIT( LSA_VOID )
{
	nare_enter -= 1;

	if ( nare_enter != 0 ) 
	{
		NARE_FATAL( 0 );
	}
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ALLOC_LOCAL_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_ALLOC_LOCAL_MEM(
	NARE_LOCAL_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT32                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"NARE_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_NARE, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "NARE_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FREE_LOCAL_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_FREE_LOCAL_MEM(
	LSA_UINT16              NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_LOCAL_MEM_PTR_TYPE                       local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "NARE_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_NARE, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_UPPER_RQB_TO_LOWER_MEM(
	NARE_DCP_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	NARE_UPPER_RQB_MEM_PTR_TYPE                       upper_rqb_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	*lower_mem_ptr_ptr = upper_rqb_mem_ptr;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_UPPER_TO_LOWER_MEM(
	NARE_DCP_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	NARE_UPPER_MEM_PTR_TYPE                           upper_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	LSA_UNUSED_ARG( sys_ptr );
	*lower_mem_ptr_ptr = upper_mem_ptr;
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_MEMSET_LOCAL
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_MEMSET_LOCAL(
	NARE_LOCAL_MEM_PTR_TYPE pMem,
	LSA_UINT8               Value,
	LSA_UINT32              Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_MEMCOPY_COMMON
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_MEMCOPY_COMMON(
	NARE_COMMON_MEM_PTR_TYPE pDst,
	NARE_COMMON_MEM_PTR_TYPE pSrc,
	LSA_UINT32               Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_ALLOC_UPPER_RQB_LOCAL
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_ALLOC_UPPER_RQB_LOCAL(
	NARE_UPPER_RQB_PTR_TYPE NARE_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_UINT16                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( length != sizeof(NARE_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"NARE_ALLOC_UPPER_RQB_LOCAL(): invalid length != sizeof(NARE_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_NARE, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "NARE_ALLOC_UPPER_RQB_LOCAL(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_FREE_UPPER_RQB_LOCAL
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_FREE_UPPER_RQB_LOCAL(
	LSA_UINT16              NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_UPPER_RQB_PTR_TYPE                       upper_rqb_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "NARE_FREE_UPPER_RQB_LOCAL(): ptr(%08x)", upper_rqb_ptr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, 0, LSA_COMP_ID_NARE, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_REQUEST_UPPER_DONE
LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR  NARE_REQUEST_UPPER_DONE(
	NARE_UPPER_CALLBACK_FCT_PTR_TYPE nare_request_upper_done_ptr,
	NARE_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)nare_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DO_TIMER_REQUEST
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_DO_TIMER_REQUEST(
	NARE_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR NARE_ARP_RESPONSE_FRAME_CONTROL(
	LSA_UINT16 NICId,
	LSA_BOOL   On )
{
	/* InterfaceID is uses as NICId [1..16], where value=0 is not allowed (is the global IF) */
	NARE_ASSERT_FALSE( ( NICId > PSI_GLOBAL_PNIO_IF_NR ) && ( NICId <= PSI_MAX_PNIO_IF_NR ) );

	psi_hd_set_arp_on_state( NICId, On );
}

/*----------------------------------------------------------------------------*/

LSA_VOID  NARE_SYSTEM_OUT_FCT_ATTR NARE_ARP_RESPONSE_FRAME_OUT(
	LSA_UINT16                  NICId,
	NARE_EDD_LOWER_MEM_PTR_TYPE pFrame,
	LSA_UINT16                  FrameLen )
{
	/* no need to make a copy of the frame at sniffing */
	/* so NARE_ARP_RESPONSE_FRAME_OUT() is empty */
	LSA_UNUSED_ARG( NICId );
	LSA_UNUSED_ARG( pFrame );
	LSA_UNUSED_ARG( FrameLen );
}

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_OPEN_CHANNEL_LOWER
LSA_VOID NARE_EDD_LOWER_OUT_FCT_ATTR NARE_DCP_OPEN_CHANNEL_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_CLOSE_CHANNEL_LOWER
LSA_VOID NARE_DCP_LOWER_OUT_FCT_ATTR NARE_DCP_CLOSE_CHANNEL_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_REQUEST_LOWER
LSA_VOID NARE_DCP_LOWER_OUT_FCT_ATTR NARE_DCP_REQUEST_LOWER(
	NARE_DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_03( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"NARE_DCP_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u)",
		lower_rqb_ptr,
		NARE_DCP_RQB_GET_HANDLE( lower_rqb_ptr ),
		NARE_DCP_RQB_GET_OPCODE( lower_rqb_ptr ) );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_DCP );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	psi_request_direct_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_ALLOC_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_ALLOC_LOWER_RQB(
	NARE_DCP_LOWER_RQB_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( length != sizeof(NARE_DCP_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_DCP_ALLOC_LOWER_RQB(): invalid length != sizeof(NARE_DCP_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_DCP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_DCP_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_FREE_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_FREE_LOWER_RQB(
	LSA_UINT16                  NARE_LOCAL_MEM_ATTR *  ret_val_ptr,
	NARE_DCP_LOWER_RQB_PTR_TYPE                        lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_DCP_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_DCP );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_ALLOC_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_ALLOC_LOWER_MEM(
	NARE_DCP_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_DCP_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_DCP );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_DCP_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_DCP_FREE_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_DCP_FREE_LOWER_MEM(
	LSA_UINT16                  NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_DCP_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_DCP_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_DCP );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_EDD_OPEN_CHANNEL_LOWER
LSA_VOID NARE_EDD_LOWER_OUT_FCT_ATTR NARE_EDD_OPEN_CHANNEL_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and open function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_open_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_CLOSE_CHANNEL_LOWER
LSA_VOID NARE_EDD_LOWER_OUT_FCT_ATTR NARE_EDD_CLOSE_CHANNEL_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and close function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_close_fct( pSys->comp_id_lower);

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_REQUEST_LOWER
LSA_VOID NARE_EDD_LOWER_OUT_FCT_ATTR NARE_EDD_REQUEST_LOWER(
	NARE_EDD_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE            sys_ptr )
{
	LSA_UINT16      mbx_id;
	PSI_REQUEST_FCT p_func;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_04( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"NARE_EDD_REQUEST_LOWER(): rqb(%08x) h(%u) opc(%u) service(%#x)",
		lower_rqb_ptr,
		NARE_EDD_RQB_GET_HANDLE( lower_rqb_ptr ),
		NARE_EDD_RQB_GET_OPCODE( lower_rqb_ptr ),
		NARE_EDD_RQB_GET_SERVICE( lower_rqb_ptr ) );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

	// get the mailbox ID and request function for EDDx
	mbx_id = edd_get_mailbox_id( lower_rqb_ptr, LSA_COMP_ID_NARE, sys_ptr );
	p_func = (PSI_REQUEST_FCT) edd_get_request_fct( pSys->comp_id_lower );

	psi_request_start( mbx_id, p_func, lower_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_ALLOC_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_ALLOC_LOWER_RQB(
	NARE_EDD_LOWER_RQB_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( length != sizeof(NARE_EDD_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_EDD_ALLOC_LOWER_RQB(): invalid length != sizeof(NARE_EDD_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_EDD );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_FREE_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_FREE_LOWER_RQB(
	LSA_UINT16                  NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_EDD_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_ALLOC_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_ALLOC_LOWER_MEM(
	NARE_EDD_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_EDD_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_EDD );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_FREE_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_FREE_LOWER_MEM(
	LSA_UINT16                  NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );
	
	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef NARE_EDD_ALLOC_LOWER_TXMEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_ALLOC_LOWER_TXMEM(
	NARE_EDD_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                  user_id,
	LSA_UINT16                                        length,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_EDD_ALLOC_LOWER_TXMEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
	    LSA_UNUSED_ARG( user_id );
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	psi_edd_alloc_nrt_tx_mem( lower_mem_ptr_ptr, length, sys_ptr, LSA_COMP_ID_NARE );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_ALLOC_LOWER_TXMEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_EDD_FREE_LOWER_TXMEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_EDD_FREE_LOWER_TXMEM(
	LSA_UINT16                  NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_EDD_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                  sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_EDD_FREE_LOWER_TXMEM(): ptr(%08x)", lower_mem_ptr );

	psi_edd_free_nrt_tx_mem( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_NARE );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#if 0 /* SOCK not yet */

/* NOTE: if activated, the open must be connected over HIF HD or PSI LD */
/*       because SOCK is located PSI LD side */
#ifndef NARE_SOCK_OPEN_CHANNEL_LOWER
LSA_VOID NARE_SOCK_LOWER_OUT_FCT_ATTR NARE_SOCK_OPEN_CHANNEL_LOWER(
	NARE_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (SOCK is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_open_channel, lower_rqb_ptr );
    #else
	// post the message to SOCK (without HIF SOCK is located in same system)
	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_SOCK_CLOSE_CHANNEL_LOWER
LSA_VOID NARE_SOCK_LOWER_OUT_FCT_ATTR NARE_SOCK_CLOSE_CHANNEL_LOWER(
	NARE_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (SOCK is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_close_channel, lower_rqb_ptr );
    #else
	// post the message to SOCK (without HIF SOCK is located in same system)
	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_SOCK_REQUEST_LOWER
LSA_VOID NARE_SOCK_LOWER_OUT_FCT_ATTR NARE_SOCK_REQUEST_LOWER(
	NARE_SOCK_LOWER_RQB_PTR_TYPE lower_rqb_ptr,
	LSA_SYS_PTR_TYPE             sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	NARE_ASSERT_FALSE( pSys->comp_id_lower == LSA_COMP_ID_SOCK );

	// Add the comp-id to RQB for lower addressing
	PSI_RQB_SET_COMP_ID( lower_rqb_ptr, pSys->comp_id_lower );

    #if ( PSI_CFG_USE_HIF_HD == 1 )
	// post the message to HIF HD (SOCK is located on LD side)
	psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_request, lower_rqb_ptr );
    #else
	// post the message to SOCK (without HIF SOCK is located in same system)
	psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, lower_rqb_ptr );
    #endif
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_SOCK_ALLOC_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_SOCK_ALLOC_LOWER_RQB(
	NARE_SOCK_LOWER_RQB_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                                   user_id,
	LSA_UINT16                                         length,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( length != sizeof(NARE_SOCK_LOWER_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_SOCK_ALLOC_LOWER_RQB(): invalid length != sizeof(NARE_SOCK_LOWER_RQB_TYPE)" );
        *lower_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_SOCK );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_SOCK_ALLOC_LOWER_RQB(): ptr(%08x)", *lower_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef NARE_SOCK_FREE_LOWER_RQB
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_SOCK_FREE_LOWER_RQB(
	LSA_UINT16                   NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_SOCK_LOWER_RQB_PTR_TYPE                       lower_rqb_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_SOCK_FREE_LOWER_RQB(): ptr(%08x)", lower_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_rqb_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_SOCK );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef NARE_SOCK_ALLOC_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_SOCK_ALLOC_LOWER_MEM(
	NARE_SOCK_LOWER_MEM_PTR_TYPE NARE_LOCAL_MEM_ATTR * lower_mem_ptr_ptr,
	LSA_USER_ID_TYPE                                   user_id,
	LSA_UINT16                                         length,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"NARE_SOCK_ALLOC_LOWER_MEM(): invalid length 0" );
        *lower_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_NARE, NARE_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)lower_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_SOCK );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_SOCK_ALLOC_LOWER_MEM(): ptr(%08x)", *lower_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef NARE_SOCK_FREE_LOWER_MEM
LSA_VOID NARE_SYSTEM_OUT_FCT_ATTR NARE_SOCK_FREE_LOWER_MEM(
	LSA_UINT16                   NARE_LOCAL_MEM_ATTR * ret_val_ptr,
	NARE_SOCK_LOWER_MEM_PTR_TYPE                       lower_mem_ptr,
	LSA_SYS_PTR_TYPE                                   sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	NARE_ASSERT_FALSE( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "NARE_SOCK_FREE_LOWER_MEM(): ptr(%08x)", lower_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, sys_ptr, LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_SOCK );
}
#else
#error "by design a function!"
#endif

#endif /* SOCK */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
