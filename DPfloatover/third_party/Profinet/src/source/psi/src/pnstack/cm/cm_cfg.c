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
/*  F i l e               &F: cm_cfg.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using the PSI framework.             */
/*                                                                           */
/*****************************************************************************/

#define CM_MODULE_ID       32012
#define LTRC_ACT_MODUL_ID  32012

#include "psi_int.h"
#include "psi_hd.h"

#include "cm_inc.h"
#include "cm_trc.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*----------------------------------------------------------------------------*/

static CM_INIT_TYPE cm_init_param  = {0,0};

/*----------------------------------------------------------------------------*/

void cm_psi_startstop( int start )
{
    LSA_UINT16 rc;

	if ( start )
	{
		cm_init_param.oneshot_lsa_time_base   = LSA_TIME_BASE_10MS;
		cm_init_param.oneshot_lsa_time_factor = 1;

		rc = cm_init( &cm_init_param );
		CM_ASSERT(rc == LSA_RET_OK);
	}
	else 
	{
		rc = cm_undo_init();
		CM_ASSERT( rc == LSA_RET_OK );
	}
}

/*------------------------------------------------------------------------------
//	System Layer (prototypes in cm_sys.h)
//----------------------------------------------------------------------------*/

#ifndef CM_GET_PATH_INFO
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_GET_PATH_INFO(
	LSA_UINT16          CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE    CM_LOCAL_MEM_ATTR *  sys_ptr_ptr,
	CM_DETAIL_PTR_TYPE  CM_LOCAL_MEM_ATTR *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                        path )
{
	*ret_val_ptr = psi_cm_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RELEASE_PATH_INFO
LSA_VOID  CM_SYSTEM_OUT_FCT_ATTR CM_RELEASE_PATH_INFO(
	LSA_UINT16             CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE                            sys_ptr,
	CM_DETAIL_PTR_TYPE                          detail_ptr )
{
	*ret_val_ptr = psi_cm_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_ALLOC_LOCAL_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_ALLOC_LOCAL_MEM(
	CM_LOCAL_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT16                                length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"CM_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, CM_MODULE_ID, 0 );
        return;
}

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_LOCAL_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_FREE_LOCAL_MEM(
	LSA_UINT16            CM_LOCAL_MEM_ATTR * ret_val_ptr,
	CM_LOCAL_MEM_PTR_TYPE                     local_mem_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_ALLOC_TIMER
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_ALLOC_TIMER(
	LSA_UINT16  CM_LOCAL_MEM_ATTR        * ret_val_ptr,
	LSA_TIMER_ID_TYPE  CM_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                             timer_type,
	LSA_UINT16                             time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, cm_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_START_TIMER
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_START_TIMER(
	LSA_UINT16        CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                      timer_id,
	LSA_USER_ID_TYPE                       user_id,
	LSA_UINT16                             time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_STOP_TIMER
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_STOP_TIMER(
	LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                timer_id)
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_TIMER
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_FREE_TIMER(
	LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                timer_id)
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FATAL_ERROR
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_FATAL_ERROR (
	LSA_UINT16              length,
	CM_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "cm", length, error_ptr );
}
#else
#pragma CM_MESSAGE("CM_FATAL_ERROR redefined by compiler switch to: " CM_STRINGIFY(CM_FATAL_ERROR))
#endif

/*------------------------------------------------------------------------------
//	Upper Layer (prototypes in cm_usr.h)
//----------------------------------------------------------------------------*/

#ifndef CM_REQUEST_UPPER_DONE
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_REQUEST_UPPER_DONE(
	CM_UPPER_CALLBACK_FCT_PTR_TYPE  cm_request_upper_done_ptr,
	CM_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)cm_request_upper_done_ptr, (struct psi_header *)upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

#ifndef CM_ALLOC_UPPER_RQB
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_ALLOC_UPPER_RQB(
	CM_UPPER_RQB_PTR_TYPE CM_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE                          user_id,
	LSA_UINT16                                length,
	LSA_SYS_PTR_TYPE                          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

    if ( length != sizeof(CM_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"CM_ALLOC_UPPER_RQB(): invalid length != sizeof(CM_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, CM_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_ALLOC_UPPER_RQB(): ptr(%08x)", *upper_rqb_ptr_ptr);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_UPPER_RQB
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_FREE_UPPER_RQB(
	LSA_UINT16            CM_LOCAL_MEM_ATTR * ret_val_ptr,
	CM_UPPER_RQB_PTR_TYPE                     upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_FREE_UPPER_RQB(): ptr(%08x)", upper_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_ALLOC_UPPER_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_ALLOC_UPPER_MEM(
	CM_UPPER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE                          user_id,
	LSA_UINT16                                length,
	LSA_SYS_PTR_TYPE                          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
			"CM_ALLOC_UPPER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_CM, CM_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_UPPER_MEM );
	
	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_ALLOC_UPPER_MEM(): ptr(%08x)", *upper_mem_ptr_ptr);
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_UPPER_MEM
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_FREE_UPPER_MEM(
	LSA_UINT16            CM_LOCAL_MEM_ATTR * ret_val_ptr,
	CM_UPPER_MEM_PTR_TYPE                     upper_mem_ptr,
	LSA_SYS_PTR_TYPE                          sys_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "CM_FREE_UPPER_MEM(): ptr(%08x)", upper_mem_ptr);

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_mem_ptr, sys_ptr, LSA_COMP_ID_CM, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_UPPER_RQB_ERROR_
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR  CM_UPPER_RQB_ERROR(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	PSI_RQB_ERROR( LSA_COMP_ID_CM, 0, upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_REQUEST_LOCAL
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_REQUEST_LOCAL(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, (LSA_VOID_PTR_TYPE *)upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("@@@@ TODO complete CM_GET_CYCLE_COUNTER for EDDS if known")
#endif

#ifndef CM_GET_CYCLE_COUNTER
LSA_VOID CM_SYSTEM_OUT_FCT_ATTR CM_GET_CYCLE_COUNTER(
	LSA_UINT16  CM_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr,
	LSA_UINT64  CM_LOCAL_MEM_ATTR *  cycle_counter_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	switch ( pSys->comp_id_edd ) // EDDx variant ?
	{
#if ( PSI_CFG_USE_EDDI == 1 )
	case LSA_COMP_ID_EDDI:
		{
			LSA_VOID_PTR_TYPE       hDDB;
			EDDI_CYCLE_COUNTER_TYPE tmp;

			hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddi_SRTProviderGetCycleCounter( hDDB, &tmp) != EDD_STS_OK )
			{
				CM_FATAL();
			}

			*cycle_counter_ptr = ((LSA_UINT64)tmp.High << 32) | tmp.Low;
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDP == 1 )
	case LSA_COMP_ID_EDDP:
		{
			LSA_VOID_PTR_TYPE       hIDB;
			EDDP_CYCLE_COUNTER_TYPE tmp;

			PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
			CM_ASSERT( pSys != LSA_NULL );

			hIDB = psi_hd_get_hIDB( pSys->hd_nr, pSys->if_nr );

			if ( eddp_SRTProviderGetCycleCounter( hIDB, &tmp) != EDD_STS_OK )
			{
				CM_FATAL();
			}

			*cycle_counter_ptr = ((LSA_UINT64)tmp.High << 32) | tmp.Low;
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDS == 1 )
	case LSA_COMP_ID_EDDS:
		{
		static unsigned cyc = 0;

		cyc++; /* dummy cycle only */

		*cycle_counter_ptr = cyc;
		}
		break;
#endif
	default:
		{
			// invalid EDDx
			CM_FATAL();
		}
	}

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif


#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE(" @@@@@@ TODO complete CM_GET_PRIMARY_AR for EDDP/EDDS if known" )
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_GET_PRIMARY_AR
LSA_UINT16 CM_SYSTEM_OUT_FCT_ATTR CM_GET_PRIMARY_AR(
	LSA_UINT16       arset_nr,
	LSA_SYS_PTR_TYPE sys_ptr )
{
	LSA_UINT16 session_key = 0; // default no SRD

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	CM_ASSERT( pSys != LSA_NULL );

	LSA_UNUSED_ARG( arset_nr );

	switch ( pSys->comp_id_edd ) // EDDx variant ?
	{
#if ( PSI_CFG_USE_EDDI == 1 )
	case LSA_COMP_ID_EDDI:
		{
			LSA_VOID_PTR_TYPE hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddi_ProviderGetPrimaryAR( hDDB, arset_nr, &session_key) != EDD_STS_OK ) 
			{
				CM_FATAL();
			}
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDP == 1 )
	case LSA_COMP_ID_EDDP:
		{
			LSA_VOID_PTR_TYPE hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddp_ProviderGetPrimaryAR( hDDB, arset_nr, &session_key) != EDD_STS_OK ) 
			{
				CM_FATAL();
			}
		}
		break;
#endif
	default:
		{
			// invalid EDDx or not supported EDDx
			CM_FATAL();
		}
	}

	return session_key;
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
