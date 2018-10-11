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
/*  F i l e               &F: sock_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SOCK using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define SOCK_MODULE_ID     32023
#define LTRC_ACT_MODUL_ID  32023

#include "psi_int.h"

#include "sock_inc.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*===========================================================================*/

#if ( PSI_CFG_USE_TCIP == 1) // using interniche TCP IP
#include "snmpport.h"
#include "snmp_imp.h"
#else
#include <WinSock.h> // wegen u_char
/* note that ...\oha\test\mib2 has no header file with a protoype */
int snmp_agt_parse(u_char*, unsigned, u_char*, unsigned);
#endif

/*===========================================================================*/

static LSA_BOOL       sock_select_enabled = LSA_FALSE;
/*static*/ LSA_UINT16 sock_oneshot_TimerID;
/*static*/ LSA_VOID   sock_oneshot( LSA_UINT16 timer_id, LSA_USER_ID_TYPE user_id );

static LSA_UINT16 sock_enter_exit_id = PSI_LOCK_ID_INVALID;

/*===========================================================================*/
/*===  START/STOP  ==========================================================*/
/*===========================================================================*/

void sock_psi_startstop( int start )
{
	LSA_UINT32 result;
	LSA_RESPONSE_TYPE rsp;

	SOCK_ASSERT(SOCK_AF_INET == AF_INET); /* cross-check */
	SOCK_ASSERT(SOCK_CFG_MAX_CONNECTIONS <= FD_SETSIZE); /* cross-check */

	if ( start )
	{
		PSI_ALLOC_REENTRANCE_LOCK( &rsp, &sock_enter_exit_id );
		SOCK_ASSERT( rsp == LSA_RET_OK );

		result = sock_init();
		SOCK_ASSERT( LSA_RET_OK == result );

		sock_select_enabled = LSA_TRUE; /* enable before timer-start (one-shot!) */

#if ( PSI_CFG_USE_TCIP == 0) // NOT using interniche TCP IP
		{
			LSA_UINT16       retVal;
	        LSA_USER_ID_TYPE null_usr_id = {0};

			PSI_ALLOC_TIMER_TGROUP0( &retVal, &sock_oneshot_TimerID, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_10MS, sock_oneshot );
			SOCK_ASSERT( LSA_RET_OK == retVal );

			PSI_START_TIMER_TGROUP0( &retVal, sock_oneshot_TimerID, null_usr_id, 1 ); /* 10ms */
			SOCK_ASSERT( LSA_RET_OK == retVal /*|| LSA_RET_OK_TIMER_RESTARTED == retVal*/);
		}
#endif
	}
	else
	{
		sock_select_enabled = LSA_FALSE;

#if ( PSI_CFG_USE_TCIP == 0) // NOT using interniche TCP IP
		{
			LSA_UINT16 retVal;

			PSI_STOP_TIMER_TGROUP0( &retVal, sock_oneshot_TimerID );
			SOCK_ASSERT( LSA_RET_OK == retVal || LSA_RET_OK_TIMER_NOT_RUNNING == retVal);

			PSI_FREE_TIMER_TGROUP0( &retVal, sock_oneshot_TimerID );
			SOCK_ASSERT( LSA_RET_OK == retVal );
		}
#endif

		sock_undo_init();

		PSI_FREE_REENTRANCE_LOCK( &rsp, sock_enter_exit_id );
		SOCK_ASSERT( rsp == LSA_RET_OK );

		sock_enter_exit_id = PSI_LOCK_ID_INVALID;
	}
}

/*===========================================================================*/
#ifndef SOCK_GET_PATH_INFO
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_GET_PATH_INFO(
	LSA_UINT16           SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE     SOCK_LOCAL_MEM_ATTR * sys_ptr_ptr,
	SOCK_DETAIL_PTR_TYPE SOCK_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                          path )
{
	*ret_val_ptr = psi_sock_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef SOCK_RELEASE_PATH_INFO
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_RELEASE_PATH_INFO(
	LSA_UINT16           SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE                           sys_ptr,
	SOCK_DETAIL_PTR_TYPE                       detail_ptr )
{
	*ret_val_ptr = psi_sock_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_REQUEST_UPPER_DONE
LSA_VOID SOCK_UPPER_OUT_FCT_ATTR SOCK_REQUEST_UPPER_DONE(
	SOCK_UPPER_CALLBACK_FCT_PTR_TYPE sock_request_upper_done_ptr,
	SOCK_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)sock_request_upper_done_ptr, upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#if ( PSI_CFG_USE_TCIP == 0) // NOT using interniche TCP IP

// poll-timer for not TCIP platforms (i.E.: using WIN32 IP-Stack)

LSA_VOID sock_oneshot( LSA_UINT16 timer_id, LSA_USER_ID_TYPE user_id )
{
	SOCK_ASSERT( timer_id == sock_oneshot_TimerID );

	// running in timer thread context

	if( sock_select_enabled )
	{
		LSA_UINT16 retVal;

		PSI_START_TIMER_TGROUP0( &retVal, timer_id, user_id, 1 ); // 10ms
		SOCK_ASSERT( LSA_RET_OK == retVal || LSA_RET_OK_TIMER_RESTARTED == retVal );

		SOCK_START_SELECT();
	}
}

#endif

/*===========================================================================*/

#ifndef SOCK_START_SELECT
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_START_SELECT( LSA_VOID )
{
	if( sock_select_enabled )
	{
		static SOCK_RQB_TYPE rqb = {0};

		// Post message to sock context
		psi_request_local( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_doselect, &rqb );
	}
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_ALLOC_LOCAL_MEM
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_ALLOC_LOCAL_MEM(
	LSA_VOID SOCK_LOCAL_MEM_PTR_TYPE SOCK_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT16                                             length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"SOCK_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_SOCK, SOCK_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "SOCK_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_FREE_LOCAL_MEM
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_FREE_LOCAL_MEM(
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR     * ret_val_ptr,
	LSA_VOID   SOCK_LOCAL_MEM_PTR_TYPE   local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "SOCK_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_ALLOC_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_ALLOC_TIMER(
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR * timer_id_ptr,
	LSA_UINT16                       timer_type,
	LSA_UINT16                       time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, sock_timeout );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_START_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_START_TIMER(
	LSA_UINT16       SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                             timer_id,
	LSA_USER_ID_TYPE                       user_id,
	LSA_UINT16                             time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_STOP_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_STOP_TIMER(
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_FREE_TIMER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_FREE_TIMER(
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_UINT16                       timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_ENTER
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_ENTER( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( sock_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_EXIT
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_EXIT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( sock_enter_exit_id );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_DO_TIMER_REQUEST
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_DO_TIMER_REQUEST(
	SOCK_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, pRQB );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_INTERLOCKED_EXCHANGE
SOCK_EXCHANGE_TYPE SOCK_INTERLOCKED_EXCHANGE(
	SOCK_EXCHANGE_TYPE volatile *ptr,
	SOCK_EXCHANGE_TYPE val )
{
	return( PSI_EXCHANGE_LONG( ptr, val ) );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_FATAL_ERROR
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_FATAL_ERROR(
	LSA_UINT16                length,
	SOCK_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "sock", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_REQUEST_LOCAL
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_REQUEST_LOCAL(
	SOCK_UPPER_RQB_PTR_TYPE rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMSET
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_MEMSET(
	LSA_VOID   * pMem,
	LSA_UINT8    Value,
	LSA_UINT32   Length )
{
	PSI_MEMSET( pMem, Value, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMCPY
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_MEMCPY(
	LSA_VOID   * pDst,
	LSA_VOID   * pSrc,
	LSA_UINT32   Length )
{
	PSI_MEMCPY( pDst, pSrc, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMMOVE
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_MEMMOVE(
	LSA_VOID         *  pDst,
	LSA_VOID         *  pSrc,
	LSA_UINT32       Length )
{
	PSI_MEMMOVE( pDst, pSrc, (LSA_UINT)Length );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_MEMCMP
LSA_BOOL SOCK_SYSTEM_OUT_FCT_ATTR SOCK_MEMCMP(
	LSA_VOID        *  pBuf1,
	LSA_VOID        *  pBuf2,
	LSA_UINT32         Length )
{
	// Note: In case of differ LSA_TRUE as result is expected, not LSA_FALSE (see sock_sys.h)
	return ( PSI_MEMCMP(pBuf1, pBuf2, (LSA_UINT)Length) == 0 ? LSA_FALSE : LSA_TRUE );
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_STRLEN
LSA_UINT32 SOCK_SYSTEM_OUT_FCT_ATTR SOCK_STRLEN(
	const LSA_CHAR * str )
{
	return ( (LSA_UINT32)PSI_STRLEN( (LSA_VOID_PTR_TYPE)str) );
}
#else
#error "by design a function!"
#endif


#ifdef SOCK_DNS_CLIENT
/*===========================================================================*/

#ifndef SOCK_DNS_LOOKUP
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_DNS_LOOKUP(
	LSA_UINT16   * retval, 
	LSA_UINT32   * ip, 
	LSA_CHAR     * name )
{
#error "who has activated SOCK_DNS_LOOKUP ?"
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_DNS_QUERY
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_DNS_QUERY(
	LSA_UINT16   * retval, 
	LSA_CHAR     * name, 
	LSA_UINT32 * ip )
{
#error "who has activated SOCK_DNS_QUERY ?"
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef SOCK_DNS_SERVERS
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_DNS_SERVERS(
	LSA_UINT16 * retval )
{
#error "who has activated SOCK_DNS_SERVERS ?"
}
#else
#error "by design a function!"
#endif
/*===========================================================================*/
#endif // SOCK_DNS_CLIENT


/*===========================================================================*/

#ifndef SOCK_SNMP_AGT_PARSE
LSA_VOID SOCK_SYSTEM_OUT_FCT_ATTR SOCK_SNMP_AGT_PARSE(
	LSA_UINT8 *                      precv_buffer,
	LSA_UINT                         len,
	LSA_UINT8 *                      psnmp_buffer,
	LSA_UINT                         snmp_len,
	LSA_UINT16 SOCK_LOCAL_MEM_ATTR * pret_val )
{
	PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT,
		"SOCK_SNMP_AGT_PARSE(): RecBuffer=(%p) Size:(%u) SNMP-Buffer=(%p) Size:(%u)",
		precv_buffer, len, psnmp_buffer, snmp_len );

	*pret_val = (LSA_UINT16)snmp_agt_parse( precv_buffer, len, psnmp_buffer, snmp_len );
}
#else
#error "by design a function!"
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
