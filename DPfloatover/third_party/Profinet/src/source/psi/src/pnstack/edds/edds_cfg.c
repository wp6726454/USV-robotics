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
/*  F i l e               &F: edds_cfg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of EDDS using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define EDDS_MODULE_ID     32016
#define LTRC_ACT_MODUL_ID  32016

#include "psi_int.h"
#include "eps_rtos.h"

#if ( PSI_CFG_USE_EDDS == 1 )

#include "edds_inc.h"
#include "edds_int.h"
#include "edds_trc.h"
#include "edds_iobuf_inc.h"

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*----------------------------------------------------------------------------*/
enum _EDDS_ENTER_EXIT
{ 
	EDDS_ENUM_HIGH,
	EDDS_ENUM_SHORT,
	EDDS_ENUM_LOCK,
	EDDS_ENUM_SET_PHASE_MASK,
	EDDS_ENUM_MIDDLE,
	EDDS_ENUM_LOCK_MAX
};

static LSA_UINT16 edds_enter_exit_id[EDDS_ENUM_LOCK_MAX];

/*=============================================================================*/
// Note: Important EDDS_ASSERT(cond) calls FATAL if condition is true !!!

void edds_psi_startstop( int start )
{
	LSA_UINT32        idx;
	LSA_RESULT        result;
	LSA_RESPONSE_TYPE rsp;

	if ( start )
	{
		// alloc critical sections
		for ( idx = 0; idx < EDDS_ENUM_LOCK_MAX; idx++ )
		{
			edds_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;

			PSI_ALLOC_REENTRANCE_LOCK( &rsp, &edds_enter_exit_id[idx] );
			EDDS_ASSERT( rsp != LSA_RET_OK );
		}

		// init EDDS
		result = edds_init();
		EDDS_ASSERT( EDD_STS_OK != result );
	}
	else
	{
		// undo init EDDS
		result = edds_undo_init();
		EDDS_ASSERT( EDD_STS_OK != result );

		// free critical sections
		for ( idx = 0; idx < EDDS_ENUM_LOCK_MAX; idx++ )
		{
			PSI_FREE_REENTRANCE_LOCK( &rsp, edds_enter_exit_id[idx] );
			EDDS_ASSERT( rsp != LSA_RET_OK );

			edds_enter_exit_id[idx] = PSI_LOCK_ID_INVALID;
		}
	}
}

/*=============================================================================*/

#ifndef EDDS_GET_PATH_INFO
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PATH_INFO(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE        EDDS_LOCAL_MEM_ATTR * sys_ptr_ptr,
	EDDS_DETAIL_PTR_TYPE    EDDS_LOCAL_MEM_ATTR * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                             path )
{
	*ret_val_ptr = psi_edds_get_path_info( sys_ptr_ptr, (void**)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_RELEASE_PATH_INFO
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RELEASE_PATH_INFO(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr,
	EDDS_DETAIL_PTR_TYPE    detail_ptr )
{
	*ret_val_ptr = psi_edds_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/

#ifndef EDDS_ALLOC_LOCAL_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_LOCAL_MEM(
	EDDS_LOCAL_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT32                                    length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_LOCAL_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_LOCAL_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_LOCAL_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_LOCAL_MEM(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDDS_LOCAL_MEM_PTR_TYPE                       local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_LOCAL_MEM(): ptr(%08x)", local_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_LOCAL_FAST_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_LOCAL_FAST_MEM(
	EDDS_LOCAL_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * local_mem_ptr_ptr,
	LSA_UINT32                                    length )
{
 	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_LOCAL_FAST_MEM(): invalid length 0" );
        *local_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_LOCAL_FAST_MEM(): ptr(%08x)", *local_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_LOCAL_FAST_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_LOCAL_FAST_MEM(
	LSA_UINT16              EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDDS_LOCAL_MEM_PTR_TYPE                       local_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_LOCAL_FAST_MEM(): ptr(%08x)", local_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_UINT32                                   length )
{
    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(): ptr(%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_RX_TRANSFER_BUFFER_MEM(): ptr(%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_UINT32                                   length )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(): ptr(%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_TX_TRANSFER_BUFFER_MEM(): ptr(%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        DeviceBuffer-FUNCTION                                                */
/*=============================================================================*/

#ifndef EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                              hSysDev,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
    LSA_UINT32                                   length )
{
    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_DEV_BUFFER_MEM(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_EDDS_ALLOC_DEV_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, upper_mem_ptr_ptr, length );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_DEV_BUFFER_MEM(): ptr(%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_DEV_BUFFER_MEM(
    EDDS_SYS_HANDLE                              hSysDev,
    LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
    EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_DEV_BUFFER_MEM(): ptr(%08x)", upper_mem_ptr );

	PSI_EDDS_FREE_DEV_BUFFER_MEM( (PSI_SYS_HANDLE)hSysDev, ret_val_ptr, upper_mem_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        Mau Type decoding                                                    */
/*=============================================================================*/

#ifndef EDDS_CHECK_AND_DECODE_MAUTYPE
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CHECK_AND_DECODE_MAUTYPE(
    LSA_UINT16                 EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDDS_SYS_HANDLE                                  hSysDev,
    EDDS_HANDLE                                      hDDB,
    LSA_UINT32                                       PortID,
    LSA_UINT16                                       MAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE                       pSpeedMode )
{
	LSA_UNUSED_ARG( ret_val_ptr );
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hDDB );
	LSA_UNUSED_ARG( PortID );
	LSA_UNUSED_ARG( MAUType );
	LSA_UNUSED_ARG( pSpeedMode );

	PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_GET_PHY_PARAMS
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_GET_PHY_PARAMS(
    EDDS_SYS_HANDLE                     hSysDev,
    EDDS_HANDLE                         hDDB,
    LSA_UINT32                          PortID,
    LSA_UINT8                           Speed,
    LSA_UINT8                           Mode,
    EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType )
{
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( hDDB );
	LSA_UNUSED_ARG( PortID );

	*pMediaType = EDD_MEDIATYPE_COPPER_CABLE; /* All EDDS supported HW is copper currently */
	*pMAUType   = EDD_MAUTYPE_UNKNOWN;

	switch ( Speed )
	{
		case EDD_LINK_SPEED_10:
		{
			if (EDD_LINK_MODE_HALF == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_10BASETHD;
			}
			else if (EDD_LINK_MODE_FULL == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_10BASETFD;
			}
			break;
		}
		case EDD_LINK_SPEED_100:
		{
			if (EDD_LINK_MODE_HALF == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_100BASETXHD;
			}
			else if (EDD_LINK_MODE_FULL == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_100BASETXFD;
			}
			break;
		}
		case EDD_LINK_SPEED_1000:
		{
			if (EDD_LINK_MODE_HALF == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_1000BASETHD;
			}
			else if (EDD_LINK_MODE_FULL == Mode)
			{
				*pMAUType = EDDS_LL_MAUTYPE_1000BASETFD;
			}
			break;
		}
	}
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        Wait-FUNCTION                                                        */
/*=============================================================================*/

#ifndef EDDS_WAIT_US
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_WAIT_US(
    LSA_UINT16 Count )
{
	PSI_WAIT_NS( (LSA_UINT64)1000 * Count );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_REQUEST_UPPER_DONE
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_REQUEST_UPPER_DONE(
	EDD_UPPER_CALLBACK_FCT_PTR_TYPE edds_request_upper_done_ptr,
	EDD_UPPER_RQB_PTR_TYPE          upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr )
{
	LSA_BOOL bIsArpRsp = LSA_FALSE;

	EDD_RQB_TYPE *   pRqb = (EDD_RQB_TYPE *)upper_rqb_ptr;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	EDDS_ASSERT( pSys == LSA_NULL );

	if ( pSys->check_arp ) // NARE ARP channel ?
	{
		if ( EDD_RQB_GET_SERVICE( upper_rqb_ptr ) == EDD_SRV_NRT_RECV ) // NRT ?
		{
			/* Check if it is an arp response frame and arp sniffing is on */
			bIsArpRsp = psi_is_rcv_arp_rsp( upper_rqb_ptr, pSys->pnio_if_nr );
		}
	}

	if ( bIsArpRsp )
	{
		/* Arp Response => forward it to nare context for arp sniffing, from there it will be forwarded to receiver context */
		/* We need to backup eddx_request_upper_done_ptr, pSys->rsp_mbx_id and pSys->pnio_if_nr for processing in nare context */
		pRqb->psi_cbf     = (PSI_EDD_REQUEST_DONE_CBF_TYPE)edds_request_upper_done_ptr;
		pRqb->psi_sys_ptr = (LSA_VOID *)pSys;

		psi_request_start( PSI_MBX_ID_NARE, psi_request_nare_ARPResponseFrameIn, upper_rqb_ptr );
	}
	else
	{
		psi_request_done ( (PSI_REQUEST_FCT)edds_request_upper_done_ptr, upper_rqb_ptr, sys_ptr ); /* No Arp Rsp => forward request to receiver */
	}
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_UPPER_RQB_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_UPPER_RQB_LOCAL(
	EDD_UPPER_RQB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
	LSA_UINT16                                   length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( length != sizeof(EDD_RQB_TYPE) )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_UPPER_RQB_LOCAL(): invalid length != sizeof(EDD_RQB_TYPE)" );
        *upper_rqb_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_rqb_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_RQB );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_UPPER_RQB_LOCAL(): ptr(%08x)", *upper_rqb_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_UPPER_RQB_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_UPPER_RQB_LOCAL(
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_RQB_PTR_TYPE                       upper_rqb_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_UPPER_RQB_LOCAL(): ptr(%08x)", upper_rqb_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_rqb_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_RQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ALLOC_UPPER_MEM_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_ALLOC_UPPER_MEM_LOCAL(
	EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR * upper_mem_ptr_ptr,
	LSA_UINT32                                   length )
{
	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_ALLOC_UPPER_MEM_LOCAL(): invalid length 0" );
        *upper_mem_ptr_ptr = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)upper_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_MEM );
	
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_ALLOC_UPPER_MEM_LOCAL(): ptr(%08x)", *upper_mem_ptr_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_UPPER_MEM_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FREE_UPPER_MEM_LOCAL(
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       upper_mem_ptr )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_FREE_UPPER_MEM_LOCAL(): ptr(%08x)", upper_mem_ptr );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, upper_mem_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_MEM );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR_UPPER
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR_UPPER(
	EDDS_SYS_HANDLE                     hSysDev,
	EDD_UPPER_MEM_PTR_TYPE              Ptr,
	LSA_UINT32 EDDS_LOCAL_MEM_ATTR *    pAddr )
{
	PSI_EDDS_PTR_TO_ADDR_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_PTR_TO_ADDR64_UPPER
EDDS_FAR_FCT LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PTR_TO_ADDR64_UPPER(
	EDDS_SYS_HANDLE                     hSysDev,
	EDD_UPPER_MEM_PTR_TYPE              Ptr,
	LSA_UINT64 EDDS_LOCAL_MEM_ATTR *    pAddr )
{
	PSI_EDDS_PTR_TO_ADDR64_UPPER( (PSI_SYS_HANDLE)hSysDev, Ptr, pAddr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/
/*        TIMER-FUNCTIONS                                                      */
/*=============================================================================*/

#ifndef EDDS_ALLOC_TIMER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_ALLOC_TIMER(
	LSA_RESULT  EDDS_LOCAL_MEM_ATTR        *  ret_val_ptr,
	LSA_TIMER_ID_TYPE  EDDS_LOCAL_MEM_ATTR *  timer_id_ptr,
	LSA_UINT16                                timer_type,
	LSA_UINT16                                time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, edds_timeout );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_START_TIMER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_START_TIMER(
	LSA_RESULT        EDDS_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                        timer_id,
	LSA_USER_ID_TYPE                         user_id,
	LSA_UINT16                               time )
{
	PSI_START_TIMER_TGROUP0( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_STOP_TIMER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_STOP_TIMER(
	LSA_RESULT  EDDS_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                  timer_id )
{
	PSI_STOP_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FREE_TIMER
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_FREE_TIMER(
	LSA_RESULT  EDDS_LOCAL_MEM_ATTR *  ret_val_ptr,
	LSA_TIMER_ID_TYPE                  timer_id )
{
	PSI_FREE_TIMER_TGROUP0( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMSET_UPPER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_UPPER(
	EDDS_LOCAL_MEM_PTR_TYPE        pMem,
	LSA_UINT8                      Value,
	LSA_UINT32                     Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMSET_LOCAL
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_LOCAL(
	EDDS_LOCAL_MEM_PTR_TYPE             pMem,
	LSA_UINT8                           Value,
	LSA_UINT32                          Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMSET_COMMON
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMSET_COMMON(
	EDDS_LOCAL_MEM_PTR_TYPE             pMem,
	LSA_UINT8                           Value,
	LSA_UINT32                          Length )
{
	PSI_MEMSET( pMem, Value, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_MEMCOPY_UPPER
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_MEMCOPY_UPPER(
	EDD_UPPER_MEM_PTR_TYPE              pDst,
	EDD_UPPER_MEM_PTR_TYPE              pSrc,
	LSA_UINT32                          Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_RQB_ERROR
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_RQB_ERROR(
	EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_RQB_ERROR( LSA_COMP_ID_EDDS, 0, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_FATAL_ERROR
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_FATAL_ERROR(
	LSA_UINT16                length,
	EDDS_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "edds", length, error_ptr );
}
#else
#error "by design a function!"
#endif


/*=============================================================================*/
//  Tasks:
//  Org		(Sync)		->	High (Short)
//  Nrt-Low (Com)		->	Middle
//  Rest	(Rest)		->	Low
/*=============================================================================*/

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*=============================================================================*/
/* prio:           RQB */

#ifndef EDDS_DO_LLTIMER_REQUEST
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_LLTIMER_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*=============================================================================*/
/* prio:           LOW */

#ifndef EDDS_DO_LLTIMER_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_LLTIMER_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*=============================================================================*/
/* prio:           RQB */

#ifndef EDDS_DO_TIMEOUT_REQUEST
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_TIMEOUT_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*=============================================================================*/
/* prio:           HIGH */

#ifndef EDDS_DO_TIMEOUT_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_TIMEOUT_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_HIGH, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           RQB */

#ifndef EDDS_DO_NRT_CANCEL_PART2
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_NRT_CANCEL_PART2( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           LOW */

#ifndef EDDS_DO_NRT_CANCEL_PART2
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_NRT_CANCEL_PART2( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           MIDDLE */

#ifndef EDDS_DO_CONSUMER_TIMER_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_CONSUMER_TIMER_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_MIDDLE, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           HIGH */

#ifndef EDDS_DO_PROVIDER_TIMER_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_PROVIDER_TIMER_REQUEST(EDD_UPPER_RQB_PTR_TYPE  pRQB)
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_HIGH, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           RQB */

#ifdef EDDS_CFG_LED_BLINK_SUPPORT
#ifndef EDDS_DO_LED_BLINK_TIMER_REQUEST
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_LED_BLINK_TIMER_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           LOW */

#ifdef EDDS_CFG_LED_BLINK_SUPPORT
#ifndef EDDS_DO_LED_BLINK_TIMER_REQUEST
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_LED_BLINK_TIMER_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           MIDDLE */

#ifndef EDDS_DO_PROVIDER_IND_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_PROVIDER_IND_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_EDDS_MIDDLE, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           RQB */

#ifndef EDDS_DO_CONSUMER_IND_REQUEST
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_DO_CONSUMER_IND_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           LOW */

#ifndef EDDS_DO_CONSUMER_IND_REQUEST
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_CONSUMER_IND_REQUEST( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           LOW */

#ifndef EDDS_DO_INT_REQUEST_PHY
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_INT_REQUEST_PHY(EDD_UPPER_RQB_PTR_TYPE  pRQB)
{
	EDDS_ASSERT( LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM );
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           LOW */

#ifndef EDDS_DO_INT_REQUEST_RCV_NRT
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_INT_REQUEST_RCV_NRT( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

#if !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*===========================================================================*/
/* prio:           HIGH */

#ifndef EDDS_DO_INT_REQUEST_SND
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_DO_INT_REQUEST_SND( EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	EDDS_ASSERT(LSA_RQB_GET_OPCODE(pRQB) != EDD_OPC_SYSTEM);
	psi_request_local( PSI_MBX_ID_EDDS_HIGH, (PSI_REQUEST_FCT)edds_system, pRQB );
}
#else
#error "by design a function!"
#endif
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

/*=============================================================================*/

#ifndef EDDS_ENTER_HIGH
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_ENTER_HIGH( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_HIGH] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_EXIT_HIGH
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_EXIT_HIGH( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_HIGH] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ENTER_SHORT
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_ENTER_SHORT( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_SHORT] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_EXIT_SHORT
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_EXIT_SHORT( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_SHORT] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_LOCK
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_LOCK( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_LOCK] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_UNLOCK
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_UNLOCK( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_LOCK] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ENTER_SET_PHASE_MASK
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_ENTER_SET_PHASE_MASK( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_SET_PHASE_MASK] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_EXIT_SET_PHASE_MASK
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_EXIT_SET_PHASE_MASK( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_SET_PHASE_MASK] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_ENTER_MIDDLE
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_ENTER_MIDDLE( LSA_VOID )
{
	PSI_ENTER_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_MIDDLE] );
}
#else
#error "by design a function!"
#endif

#ifndef EDDS_EXIT_MIDDLE
LSA_VOID  EDDS_SYSTEM_OUT_FCT_ATTR  EDDS_EXIT_MIDDLE( LSA_VOID )
{
	PSI_EXIT_REENTRANCE_LOCK( edds_enter_exit_id[EDDS_ENUM_MIDDLE] );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_ADDR_EDDS_TO_USER
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBUFFER_ADDR_EDDS_TO_USER(
    LSA_UINT16                MngmId,
    EDD_UPPER_MEM_U8_PTR_TYPE pAddr )
{
	LSA_UNUSED_ARG( MngmId );

	return pAddr;
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_MEM_ALLOC 
LSA_VOID EDDS_IOBUFFER_MEM_ALLOC(
           LSA_VOID_PTR_TYPE * ppMem,
		   LSA_UINT32          length )
{
 	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_IOBUFFER_MEM_ALLOC(): invalid length 0" );
        *ppMem = LSA_NULL;
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	PSI_ALLOC_LOCAL_MEM( ppMem, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_MEM_ALLOC(): ptr(%08x)", *ppMem );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_MEM_FREE
LSA_VOID EDDS_IOBUFFER_MEM_FREE(
	    LSA_UINT16        EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
		LSA_VOID_PTR_TYPE                       pMem )
{
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_MEM_FREE(): ptr(%08x)", pMem );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, pMem, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_MEMCPY 
LSA_VOID EDDS_IOBUFFER_MEMCPY(
    LSA_VOID_PTR_TYPE pDst, 
    LSA_VOID_PTR_TYPE pSrc, 
    LSA_UINT32        Length )
{
	PSI_MEMCPY( pDst, pSrc, Length );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_FATAL_ERROR 
LSA_VOID EDDS_IOBUFFER_FATAL_ERROR(
    LSA_UINT16                length,
    EDDS_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "edds", length, error_ptr );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_EXCHANGE 
EDDS_EXCHANGE_TYPE EDDS_IOBUFFER_EXCHANGE(
    EDDS_EXCHANGE_TYPE volatile * pTarget,
    EDDS_EXCHANGE_TYPE            Value )
{
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_EXCHANGE()" );

	return( PSI_EXCHANGE_LONG( pTarget, Value) );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_ALLOC_MNGM_MEM
LSA_VOID EDDS_IOBUFFER_ALLOC_MNGM_MEM(
    EDDS_SYS_HANDLE          hSysDev,
    EDD_UPPER_MEM_PTR_TYPE * pMem,
    LSA_UINT32               length )
{
 	LSA_USER_ID_TYPE null_usr_id = {0};

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"EDDS_IOBUFFER_ALLOC_MNGM_MEM(): invalid length 0" );
        *pMem = LSA_NULL;
        LSA_UNUSED_ARG( hSysDev );
        PSI_FATAL_COMP( LSA_COMP_ID_EDDS, EDDS_MODULE_ID, 0 );
        return;
    }

	// NOTE: if app and edds get splitted, we have to use shared memory!!!
	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)pMem, null_usr_id, length, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_ALLOC_MNGM_MEM(): ptr(%08x)", *pMem );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_IOBUFFER_FREE_MNGM_MEM
LSA_VOID EDDS_IOBUFFER_FREE_MNGM_MEM(
	EDDS_SYS_HANDLE                              hSysDev,
	LSA_UINT16             EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
	EDD_UPPER_MEM_PTR_TYPE                       pMem )
{
    LSA_UNUSED_ARG( hSysDev );

	// NOTE: if app and edds get splitted, we have to use shared memory!!!
	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "EDDS_IOBUFFER_FREE_MNGM_MEM(): ptr(%08x)", pMem );

	PSI_FREE_LOCAL_MEM( ret_val_ptr, pMem, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifndef EDDS_NRT_SEND_HOOK
LSA_VOID EDDS_NRT_SEND_HOOK(
    EDDS_SYS_HANDLE        hSysDev,
    EDD_UPPER_RQB_PTR_TYPE pRQB )
{
	PSI_EDD_NRT_SEND_HOOK( (PSI_SYS_HANDLE)hSysDev, pRQB );
}
#else
#error "by design a function!"
#endif

/*=============================================================================*/

#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT(
    LSA_UINT16                       timer_id,
    LSA_UINT32                       CycleTime,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pTimerValue,
    LSA_BOOL   EDDS_LOCAL_MEM_ATTR * pRun )
{
    int             RetVal;
	LSA_UINT32      AktTimeUs;
	LSA_UINT32      Delta;
	struct timespec ActualTime;

    LSA_UNUSED_ARG( timer_id );

	RetVal = clock_gettime( CLOCK_MONOTONIC, &ActualTime );
    EDDS_ASSERT( RetVal );

	AktTimeUs = ((LSA_UINT64)ActualTime.tv_nsec + ((LSA_UINT64)ActualTime.tv_sec * (LSA_UINT64)1000000000));

	Delta = (LSA_UINT32)(AktTimeUs - *pTimerValue);

	if ( Delta < (CycleTime / 3000 * 2) )
	{
		*pRun = LSA_FALSE;
	}
	else
	{
		*pTimerValue = AktTimeUs;
		*pRun 		 = LSA_TRUE;
	}
}
#endif
#endif

/*=============================================================================
 * function name:  EDDS_CSRT_CONSUMER_CYCLE_START
 *
 * function:       Called after the consumer timer is started.
 *                 The timer_id was allocated previously for this consumer
 *                 check cycle and can be used to identify the cycle time.
 *
 *                 This macro will be called  after the consumer timer
 *                 is started. The Systemadaption setup the TimerValue
 *                 with the first timer value.
 *
 * parameters:     LSA_UINT16                              timer_id
 *                 LSA_UINT32    EDDS_LOCAL_MEM_ATTR *     pTimerValue
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#ifndef EDDS_CSRT_CONSUMER_CYCLE_START
LSA_VOID EDDS_SYSTEM_OUT_FCT_ATTR EDDS_CSRT_CONSUMER_CYCLE_START(
    LSA_UINT16                       timer_id,
    LSA_UINT32 EDDS_LOCAL_MEM_ATTR * pTimerValue )
{
    int             RetVal;
	struct timespec ActualTime;

    LSA_UNUSED_ARG( timer_id );

	RetVal = clock_gettime( CLOCK_MONOTONIC, &ActualTime );
    EDDS_ASSERT( RetVal );

	*pTimerValue = ActualTime.tv_nsec;
}
#endif
#endif

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI EDDS support (PSI_CFG_USE_EDDS=" PSI_STRINGIFY(PSI_CFG_USE_EDDS) ")")
#endif
#endif // PSI_CFG_USE_EDDS

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
