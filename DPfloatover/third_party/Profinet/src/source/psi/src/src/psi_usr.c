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
/*  F i l e               &F: psi_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI LD/HD user interface (see psi_usr.h)                  */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      2
#define LTRC_ACT_MODUL_ID  2

#include "psi_inc.h"
#include "psi_int.h"
#include "psi_ld.h"
#include "psi_hd.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*---------------------------------------------------------------------------*/
/* PSI EDD User Request handling                                             */
/*                                                                           */
/* As the user doesn't know which EDD is used on the HD, PSI needs to find   */
/* the correct one and to delegate the request there.                        */
/*                                                                           */
/* The comp_id of the RQB needs to be set to LSA_COMP_ID_EDD                 */
/* Currently only 1 Path is supported (PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC)*/
/*---------------------------------------------------------------------------*/

#if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )

typedef struct
{
	LSA_BOOL                        bUsed;
	LSA_HANDLE_TYPE                 edd_handle;     /* Channel handle delivered by EDD */
	LSA_HANDLE_TYPE                 psi_handle;     /* PSI Channel handle              */
	LSA_HANDLE_TYPE                 user_handle;    /* Edd User handle backup          */
	EDD_UPPER_CALLBACK_FCT_PTR_TYPE user_cbf;       /* User callback backup            */
	LSA_UINT16                      edd_type;       /* Edd comp_id                     */
} PSI_EDD_USER_CHANNEL_TYPE, *PSI_EDD_USER_CHANNEL_PTR_TYPE;

typedef struct
{
	PSI_EDD_USER_CHANNEL_TYPE channel[PSI_CFG_MAX_PNIO_IF_CNT];
} PSI_EDD_USER_CHANNEL_STORE_TYPE, *PSI_EDD_USER_CHANNEL_STORE_PTR_TYPE;

PSI_EDD_USER_CHANNEL_STORE_TYPE g_PsiEddUserStore;

static LSA_VOID psi_hd_edd_user_channel_request_done( EDD_RQB_TYPE * rqb_ptr )
{
	EDD_RQB_TYPE *                  edd_rqb_ptr = (EDD_RQB_TYPE *)rqb_ptr;
	EDD_RQB_OPEN_CHANNEL_TYPE *     edd_open_ptr;
	PSI_EDD_USER_CHANNEL_PTR_TYPE   channel_ptr;
	PSI_SYS_TYPE                    Sys;

	PSI_ASSERT(LSA_RQB_GET_HANDLE(rqb_ptr) < PSI_CFG_MAX_PNIO_IF_CNT);

	channel_ptr = &g_PsiEddUserStore.channel[LSA_RQB_GET_HANDLE(rqb_ptr)];

	#if ( PSI_CFG_USE_HIF_HD == 1 )
	Sys.rsp_mbx_id = PSI_MBX_ID_HIF_HD;
	#else
	Sys.rsp_mbx_id = PSI_MBX_ID_USER; /* EDD user direct */
	#endif

	switch (LSA_RQB_GET_OPCODE(rqb_ptr))
	{
		case EDD_OPC_OPEN_CHANNEL:
			edd_open_ptr = (EDD_RQB_OPEN_CHANNEL_TYPE *)edd_rqb_ptr->pParam;
			edd_open_ptr->Cbf = channel_ptr->user_cbf;
			edd_open_ptr->HandleUpper = channel_ptr->user_handle;

			if (LSA_RQB_GET_RESPONSE(rqb_ptr) == LSA_OK)
			{
				channel_ptr->edd_handle = edd_open_ptr->Handle;
				edd_open_ptr->Handle    = channel_ptr->psi_handle;
			}
			else
			{
				channel_ptr->bUsed = LSA_FALSE;
			}

			break;

		case EDD_OPC_CLOSE_CHANNEL:
			if (LSA_RQB_GET_RESPONSE(rqb_ptr) == LSA_OK)
			{
				channel_ptr->bUsed = LSA_FALSE;
			}

			break;

		default:
			break;
	}

	LSA_RQB_SET_HANDLE( rqb_ptr, channel_ptr->user_handle );

	psi_request_done( (PSI_REQUEST_FCT)channel_ptr->user_cbf, rqb_ptr, &Sys );
}

static LSA_VOID psi_hd_edd_request( PSI_RQB_PTR_TYPE rqb_ptr )
{
	PSI_EDD_USER_CHANNEL_PTR_TYPE channel_ptr;

	PSI_ASSERT(LSA_RQB_GET_HANDLE(rqb_ptr) < PSI_CFG_MAX_PNIO_IF_CNT);

	PSI_ENTER();

	channel_ptr = &g_PsiEddUserStore.channel[LSA_RQB_GET_HANDLE(rqb_ptr)];

	PSI_ASSERT(channel_ptr->bUsed == LSA_TRUE);

	LSA_RQB_SET_HANDLE(rqb_ptr, channel_ptr->edd_handle);

	PSI_EXIT();

	switch (channel_ptr->edd_type)
	{
        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, rqb_ptr );
			break;
        #endif
        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_request, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_request, rqb_ptr );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
			break;
        #endif
        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_request, rqb_ptr );
			break;
        #endif
		default:
		    PSI_FATAL( 0 );
	}
}

static LSA_VOID psi_hd_edd_close_channel( PSI_RQB_PTR_TYPE rqb_ptr )
{
	PSI_EDD_USER_CHANNEL_PTR_TYPE channel_ptr;

	PSI_ASSERT(LSA_RQB_GET_HANDLE(rqb_ptr) < PSI_CFG_MAX_PNIO_IF_CNT);

	PSI_ENTER();

	channel_ptr = &g_PsiEddUserStore.channel[LSA_RQB_GET_HANDLE(rqb_ptr)];

	PSI_ASSERT(channel_ptr->bUsed == LSA_TRUE);

	LSA_RQB_SET_HANDLE(rqb_ptr, channel_ptr->edd_handle);

	PSI_EXIT();

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_channel(): EDD Close channel, edd_type(%#x)", 
        channel_ptr->edd_type);

    switch (channel_ptr->edd_type)
	{
        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_close_channel, rqb_ptr );
			break;
        #endif
        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
			break;
        #endif
        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_close_channel, rqb_ptr );
			break;
        #endif
		default:
			PSI_FATAL( 0 );
	}
}

static LSA_VOID psi_hd_edd_open_channel( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* Handle an EDD service. The Edd Request needs to be routed to the correct EDDx component. */

	EDD_RQB_TYPE *                  edd_rqb_ptr = (EDD_RQB_TYPE *)rqb_ptr;
	EDD_RQB_OPEN_CHANNEL_TYPE *     edd_open_ptr;
	PSI_DETAIL_STORE_PTR_TYPE       store_ptr;
	PSI_EDD_USER_CHANNEL_PTR_TYPE   channel_ptr = LSA_NULL;
	LSA_UINT16                      hd_nr, if_nr, path, i;

	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_open_channel(): invalid RQB, rqb_ptr(%#x)", rqb_ptr );
		PSI_FATAL( 0 );
	}

    PSI_ASSERT(PSI_RQB_GET_COMP_ID( rqb_ptr ) == LSA_COMP_ID_EDD);

	edd_open_ptr = (EDD_RQB_OPEN_CHANNEL_TYPE *)edd_rqb_ptr->pParam;
	PSI_ASSERT( psi_is_not_null(edd_open_ptr) );

	hd_nr = PSI_SYSPATH_GET_HD( edd_open_ptr->SysPath );
	if_nr = PSI_SYSPATH_GET_IF( edd_open_ptr->SysPath );
	path  = PSI_SYSPATH_GET_PATH( edd_open_ptr->SysPath );

	PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_open_channel(): EDD Open channel, hd_nr(%u) if_nr(%u) path(%#x/%u) sys_path(%#x)", 
        hd_nr, if_nr, path, path, edd_open_ptr->SysPath);

    PSI_ASSERT(path == PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC);

	PSI_ENTER();

	store_ptr = psi_get_detail_store( hd_nr, if_nr );
	PSI_ASSERT( psi_is_not_null(store_ptr) );

	for(i=0; i<PSI_CFG_MAX_PNIO_IF_CNT; i++)
	{
		if(g_PsiEddUserStore.channel[i].bUsed == LSA_FALSE)
		{
			channel_ptr = &g_PsiEddUserStore.channel[i];
			channel_ptr->psi_handle = (LSA_HANDLE_TYPE)i;
			break;
		}
	}
	PSI_ASSERT(psi_is_not_null(channel_ptr));

	channel_ptr->bUsed = LSA_TRUE;

	channel_ptr->edd_type = store_ptr->edd_comp_id;

	channel_ptr->user_handle = edd_open_ptr->HandleUpper;
	channel_ptr->user_cbf = edd_open_ptr->Cbf;

	edd_open_ptr->Cbf = psi_hd_edd_user_channel_request_done;
	edd_open_ptr->HandleUpper = channel_ptr->psi_handle;

	PSI_EXIT();

	switch(channel_ptr->edd_type)
	{
        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_open_channel, rqb_ptr );
			break;
        #endif
        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
			break;
        #endif
        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_open_channel, rqb_ptr );
			break;
        #endif
		default:
			PSI_FATAL( 0 );
	}
}

#endif

/*---------------------------------------------------------------------------*/
/* PSI LD user interface                                                     */
/*                                                                           */
/* hif_*d_open_channel/close_channel/request can be called directly          */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_ld_system( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr))  // invalid RQB ?
	{
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_ld_system(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_system(): rqb_ptr(%#x) opcode(%#x)", rqb_ptr, PSI_RQB_GET_OPCODE(rqb_ptr) );

	switch ( PSI_RQB_GET_OPCODE( rqb_ptr ) ) // Allowed Service ?
	{
	    case PSI_OPC_LD_OPEN_DEVICE:
		{
			PSI_UPPER_LD_OPEN_PTR_TYPE p_open = &rqb_ptr->args.ld_open;

			if( psi_is_null( p_open->psi_request_upper_done_ptr )) // no callback 
			{
				PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
					"psi_ld_system(): no CBF defined for open device, upper_rqb_ptr(%#x)",
					rqb_ptr );

				PSI_FATAL( 0 );
			}
			else
			{
				// Start Open Device handling
				psi_ld_open_device( rqb_ptr );
			}
		}
		break;

        case PSI_OPC_LD_CLOSE_DEVICE:
		{
			PSI_UPPER_LD_CLOSE_PTR_TYPE p_close = &rqb_ptr->args.ld_close;

			if( psi_is_null( p_close->psi_request_upper_done_ptr )) // no callback 
			{
				PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
					"psi_ld_system(): no CBF defined for close device, upper_rqb_ptr(%#x)",
					rqb_ptr );

				PSI_FATAL( 0 );
			}
			else
			{
				// Start Close Device handling
				psi_ld_close_device( rqb_ptr );
			}
		}
		break;

        default:
		{
			PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_ld_system(): invalid opcode received, rqb_ptr(%#x) opc(%u/%#x)",
				rqb_ptr,
				PSI_RQB_GET_OPCODE( rqb_ptr ),
				PSI_RQB_GET_OPCODE( rqb_ptr ) );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_channel( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_ld_open_channel(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channel(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );
	
	// Handle an open request from upper (HIF LD or user)
	// Note: the open request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected
	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request to HIF HD or
			// direct to IOH if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_open_channel, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SNMPX == 1 )
		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SOCK == 1 )
		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr );
			#else
			psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_open_channel, rqb_ptr );
			#endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr );
			#else
			psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, rqb_ptr );
			#endif
		}
		break;
        #endif

		default:
		{
			PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_ld_open_channel(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_channel (PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_ld_close_channel(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channel(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );

	// Handle a close request from upper (HIF LD or user)
	// Note: the close request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected

	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request to HIF HD or
			// direct to IOH if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_close_channel, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SNMPX == 1 )
		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SOCK == 1 )
		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
				psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr );
			#else
				psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_close_channel, rqb_ptr );
			#endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
				psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr );
			#else
				psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, rqb_ptr );
			#endif
		}
		break;
        #endif

		default:
		{
			PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_ld_close_channel(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_request( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_ld_request(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_ld_request(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );

	// Handle a service request from upper (HIF LD or user)
	// Note: the service request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected

	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request to HIF HD or
			// direct to IOH if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr );
            #else
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_request, rqb_ptr );
            #endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SNMPX == 1 )
		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start( PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_SOCK == 1 )
		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start( PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr );
			#else
			psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_request, rqb_ptr );
			#endif
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if ( PSI_CFG_USE_HIF_HD == 1 )
			psi_request_start( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr );
			#else
			psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, rqb_ptr );
			#endif
		}
		break;
        #endif

		default:
		{
			PSI_LD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_ld_request(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
/* PSI HD user interface                                                     */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_hd_system( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null(rqb_ptr) )  // invalid RQB ?
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_hd_system(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_system(): rqb_ptr(%#x) opcode(%#x)", rqb_ptr, PSI_RQB_GET_OPCODE(rqb_ptr) );

	switch ( PSI_RQB_GET_OPCODE( rqb_ptr ) )
	{
	    case PSI_OPC_HD_OPEN_DEVICE:
		{
			PSI_UPPER_HD_OPEN_PTR_TYPE p_open = &rqb_ptr->args.hd_open;

			if( psi_is_null( p_open->psi_request_upper_done_ptr )) // no callback 
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
					"psi_hd_system(): no CBF defined for open device, upper_rqb_ptr(%#x)",
					rqb_ptr );

				PSI_FATAL( 0 );
			}
			else
			{
				// Start Open Device handling
				psi_hd_open_device( rqb_ptr );
			}
		}
		break;

        case PSI_OPC_HD_CLOSE_DEVICE:
		{
			PSI_UPPER_HD_CLOSE_PTR_TYPE p_close = &rqb_ptr->args.hd_close;

			if( psi_is_null( p_close->psi_request_upper_done_ptr )) // no callback 
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
					"psi_hd_system(): no CBF defined for close device, upper_rqb_ptr(%#x)",
					rqb_ptr );

				PSI_FATAL( 0 );
			}
			else
			{
				// Start Close Device handling
				psi_hd_close_device( rqb_ptr );
			}
		}
		break;

        default:
		{
			PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_hd_system(): invalid opcode received, rqb_ptr(%#x) opc(%u/%#x)",
				rqb_ptr,
				PSI_RQB_GET_OPCODE( rqb_ptr ),
				PSI_RQB_GET_OPCODE( rqb_ptr ) );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_open_channel( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_hd_open_channel(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_channel(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );

	// Handle an open request from upper (HIF HD)
	// Note: the open request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_ACP == 1 )
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start( PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_DCP == 1 )
		case LSA_COMP_ID_DCP:
		{
			// DCP runs on HD level, delegate request direct to DCP
			psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr );
        	#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
		}
		break;
        #endif

        #if ( PSI_CFG_USE_GSY == 1 )
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start( PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request direct to IOH
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_LLDP == 1 )
		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_MRP == 1 )
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_POF == 1 )
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start( PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_open_channel, rqb_ptr );
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_hd_open_channel(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_close_channel( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_hd_close_channel(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_close_channel(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );

	// Handle a close request from upper (HIF HD)
	// Note: the close request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_ACP == 1 )
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start( PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_DCP == 1 )
		case LSA_COMP_ID_DCP:
		{
			// DCP runs on HD level, delegate request direct to DCP
			psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
		}
		break;
        #endif

        #if ( PSI_CFG_USE_GSY == 1 )
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start( PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request direct to IOH
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_LLDP == 1 )
		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_MRP == 1 )
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_POF == 1 )
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start( PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_close_channel, rqb_ptr );
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_hd_close_channel(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_request( PSI_RQB_PTR_TYPE rqb_ptr )
{
	/* check the params */
	if( psi_is_null( rqb_ptr ))   // invalid RQB ?
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_hd_request(): invalid RQB, rqb_ptr(%#x)",
			rqb_ptr );

		PSI_FATAL( 0 );
	}

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_request(): rqb_ptr(%#x) comp_id(%#x)", rqb_ptr, PSI_RQB_GET_COMP_ID(rqb_ptr) );

	// Handle an service request from upper (HIF HD)
	// Note: the service request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch ( PSI_RQB_GET_COMP_ID( rqb_ptr ) )
	{
        #if ( PSI_CFG_USE_ACP == 1 )
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start( PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CLRPC == 1 )
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_CM == 1 )
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_DCP == 1 )
		case LSA_COMP_ID_DCP:
	    {
		    // DCP runs on HD level, delegate request direct to DCP
		    psi_request_start( PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, rqb_ptr );
	    }
	    break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDI == 1 )
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start( PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDP == 1 )
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start( PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_RQB, (PSI_REQUEST_FCT)edds_request, rqb_ptr );
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			psi_request_start( PSI_MBX_ID_EDDS_LOW, (PSI_REQUEST_FCT)edds_request, rqb_ptr );
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
		}
		break;
        #endif

        #if ( PSI_CFG_USE_GSY == 1 )
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start( PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_IOH == 1 )
		case LSA_COMP_ID_IOH:
		{
			// IOH runs on HD level, delegate request direct to IOH
			psi_request_start( PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_LLDP == 1 )
		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start( PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_MRP == 1 )
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start( PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_NARE == 1 )
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start( PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_OHA == 1 )
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start( PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, rqb_ptr );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_POF == 1 )
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start( PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_request, rqb_ptr );
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_hd_request(): unexpected comp_id(%u/%#x), rqb_ptr(%#x)",
				PSI_RQB_GET_COMP_ID( rqb_ptr ),
				PSI_RQB_GET_COMP_ID( rqb_ptr ),					
				rqb_ptr );

			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_hd_get_edd_handle( LSA_UINT16 hd_nr, PSI_EDD_HDDB * edd_handle_ptr )
{
	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );

	*edd_handle_ptr = psi_hd_get_hDDB( hd_nr );
}

/*---------------------------------------------------------------------------*/

LSA_UINT32 psi_hd_get_real_pi_size( LSA_UINT16 hd_nr )
{
	LSA_UINT32  pi_size;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );

	pi_size = psi_hd_get_pi_size( hd_nr );

	return pi_size;
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_usr_alloc_nrt_tx_mem( LSA_VOID_PTR_TYPE * mem_ptr_ptr,
                                   LSA_UINT32		   length,
                                   LSA_UINT16          hd_nr,
                                   LSA_UINT16		   comp_id )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                         "psi_usr_alloc_nrt_tx_mem(): hd_nr(%u) mem_ptr_ptr(%08x) comp_id(%u)",
                         hd_nr, mem_ptr_ptr, comp_id );

    if ( 0 == length )
    {
		PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_usr_alloc_nrt_tx_mem(): invalid length 0" );
        *mem_ptr_ptr = LSA_NULL;
		PSI_FATAL( 0 );
        return;
    }

    switch ( comp_id )
    {
        case LSA_COMP_ID_EDDI:
        {
            #if defined (PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
            if ( length < 64UL )
            {
                // padding to 4-byte-aligned length (EDDI specific bug)
                if ( length & 3UL )
                {
                    length = (LSA_UINT32)(length & (~3UL)) + 4UL;
                }
            }
            #endif
        }
        // lint -fallthrough
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
            // allocate NRT buffer from NRT memory pool
            PSI_NRT_ALLOC_TX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );
        }
        break;
        case LSA_COMP_ID_EDDS:
        {
            if ( psi_hd_is_edd_nrt_copy_if_on( hd_nr ) )
            {
	            LSA_USER_ID_TYPE null_usr_id = {0};
			    // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
                // allocate NRT buffer from NRT memory pool
                PSI_NRT_ALLOC_TX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );
            }
        }
        break;
        default:
        {
		    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_usr_alloc_nrt_tx_mem(): invalid comp_id(%u)",
			     comp_id );
            *mem_ptr_ptr = LSA_NULL;
		    PSI_FATAL( 0 );
        }
    }
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_usr_alloc_nrt_rx_mem( LSA_VOID_PTR_TYPE * mem_ptr_ptr, 
                                   LSA_UINT32		   length,
                                   LSA_UINT16          hd_nr,
                                   LSA_UINT16		   comp_id )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                         "psi_usr_alloc_nrt_rx_mem(): hd_nr(%u) mem_ptr_ptr(%08x) comp_id(%u)",
                         hd_nr, mem_ptr_ptr, comp_id );

    if ( EDD_FRAME_BUFFER_LENGTH != length )
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_usr_alloc_nrt_rx_mem(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *mem_ptr_ptr = LSA_NULL;
		PSI_FATAL( 0 );
        return;
    }

    switch ( comp_id )
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
            // allocate NRT buffer from NRT memory pool
            PSI_NRT_ALLOC_RX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );
        }
        break;
        case LSA_COMP_ID_EDDS:
        {
            if ( psi_hd_is_edd_nrt_copy_if_on( hd_nr ) )
            {
	            LSA_USER_ID_TYPE null_usr_id = {0};
			    // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
                // allocate NRT buffer from NRT memory pool
                PSI_NRT_ALLOC_RX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );
            }
        }
        break;
        default:
        {
		    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_usr_alloc_nrt_rx_mem(): invalid comp_id(%u)",
			     comp_id );
            *mem_ptr_ptr = LSA_NULL;
		    PSI_FATAL( 0 );
        }
    }
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_usr_free_nrt_tx_mem( LSA_UINT16	    * ret_val_ptr,
                                  LSA_VOID_PTR_TYPE	  mem_ptr,
                                  LSA_UINT16          hd_nr,
                                  LSA_UINT16		  comp_id )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                         "psi_usr_free_nrt_tx_mem(): hd_nr(%u) mem_ptr(%08x) comp_id(%u)",
                         hd_nr, mem_ptr, comp_id );

    switch ( comp_id )
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
			// free NRT buffer to NRT memory pool
            PSI_NRT_FREE_TX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
        }
        break;
        case LSA_COMP_ID_EDDS:
        {
            if ( psi_hd_is_edd_nrt_copy_if_on( hd_nr ) )
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
			    // free NRT buffer to NRT memory pool
                PSI_NRT_FREE_TX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
            }
        }
        break;
        default:
        {
		    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_usr_free_nrt_tx_mem(): invalid comp_id(%u)",
			     comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
		    PSI_FATAL( 0 );
        }
    }
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_usr_free_nrt_rx_mem( LSA_UINT16	    * ret_val_ptr,
                                  LSA_VOID_PTR_TYPE	  mem_ptr,
                                  LSA_UINT16          hd_nr,
                                  LSA_UINT16		  comp_id )
{
    PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_CHAT,
                         "psi_usr_free_nrt_rx_mem(): hd_nr(%u) mem_ptr(%08x) comp_id(%u)",
                         hd_nr, mem_ptr, comp_id );

    switch ( comp_id )
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
			// free NRT buffer to NRT memory pool
            PSI_NRT_FREE_RX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
        }
        break;
        case LSA_COMP_ID_EDDS:
        {
            if ( psi_hd_is_edd_nrt_copy_if_on( hd_nr ) )
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_pool_handle( hd_nr );
			    // free NRT buffer to NRT memory pool
                PSI_NRT_FREE_RX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
            }
        }
        break;
        default:
        {
		    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_usr_free_nrt_rx_mem(): invalid comp_id(%u)",
			     comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
		    PSI_FATAL( 0 );
        }
    }
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

