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
/*  F i l e               &F: edd_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of EDDx using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32029
#define LTRC_ACT_MODUL_ID  32029

#include "psi_int.h"

#if (( PSI_CFG_USE_EDDI == 1) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 ))
/*----------------------------------------------------------------------------*/

#include "edd_inc.h"

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/

/* marked for DELETION */
LSA_VOID EDD_ALLOC_DEV_MEM(
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       alloc_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

    // The length-0-check is already executed in the calling function!

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user allocates the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		alloc_comp_id = pSys->comp_id_lower;
	}
	else
	{
		alloc_comp_id = lsa_comp_id;
	}

	switch ( alloc_comp_id ) // EDDx variant ?
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
			// allocate NRT buffer from NRT memory pool
			PSI_NRT_ALLOC_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"EDD_ALLOC_DEV_MEM(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
	            LSA_USER_ID_TYPE null_usr_id = {0};
			    // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, null_usr_id, length, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
    			// allocate NRT buffer from NRT memory pool
	    		PSI_NRT_ALLOC_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );
            }

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"EDD_ALLOC_DEV_MEM(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"EDD_ALLOC_DEV_MEM(): invalid alloc_comp_id = EDDx-Type(%u)",
				alloc_comp_id );
            *lower_mem_ptr_ptr = LSA_NULL;
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
/* marked for DELETION */
LSA_VOID EDD_FREE_DEV_MEM(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       free_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user free the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		free_comp_id = pSys->comp_id_lower;
	}
	else
	{
		free_comp_id = lsa_comp_id;
	}

	switch ( free_comp_id ) // EDDx variant ?
	{
	    case LSA_COMP_ID_EDDI:
	    case LSA_COMP_ID_EDDP:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"EDD_FREE_DEV_MEM: ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

			// free NRT buffer to NRT memory pool
			PSI_NRT_FREE_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"EDD_FREE_DEV_MEM: ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
			    // free NRT buffer to local memory pool
	            PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // free NRT buffer to NRT memory pool
			    PSI_NRT_FREE_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
            }
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"EDD_FREE_DEV_MEM(): invalid free_comp_id = EDDx-Type(%u)",
				free_comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edd_alloc_nrt_tx_mem(
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       alloc_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

    // The length-0-check is already executed in the calling function!

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user allocates the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		alloc_comp_id = pSys->comp_id_lower;
	}
	else
	{
		alloc_comp_id = lsa_comp_id;
	}

	switch ( alloc_comp_id ) // EDDx variant ?
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
            // allocate NRT buffer from NRT memory pool
			PSI_NRT_ALLOC_TX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_alloc_nrt_tx_mem(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
	            LSA_USER_ID_TYPE null_usr_id = {0};
			    // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, null_usr_id, length, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
                // allocate NRT buffer from NRT memory pool
			    PSI_NRT_ALLOC_TX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );
            }

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_alloc_nrt_tx_mem(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"psi_edd_alloc_nrt_tx_mem(): invalid alloc_comp_id = EDDx-Type(%u)",
				alloc_comp_id );
            *lower_mem_ptr_ptr = LSA_NULL;
			PSI_FATAL( 0 );
		}
	}
}

LSA_VOID psi_edd_alloc_nrt_rx_mem(
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       alloc_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

    // The length-0-check is already executed in the calling function!

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user allocates the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		alloc_comp_id = pSys->comp_id_lower;
	}
	else
	{
		alloc_comp_id = lsa_comp_id;
	}

	switch ( alloc_comp_id ) // EDDx variant ?
	{
	    case LSA_COMP_ID_EDDI:
	    case LSA_COMP_ID_EDDP:
		{
			// allocate NRT buffer from NRT memory pool
			PSI_NRT_ALLOC_RX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_alloc_nrt_rx_mem(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
	            LSA_USER_ID_TYPE null_usr_id = {0};
			    // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, null_usr_id, length, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // allocate NRT buffer from NRT memory pool
			    PSI_NRT_ALLOC_RX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNRT, lsa_comp_id );
            }

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_alloc_nrt_rx_mem(): ptr(%08x) lsa_comp_id(%u)",
				*lower_mem_ptr_ptr, lsa_comp_id );
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"psi_edd_alloc_nrt_rx_mem(): invalid alloc_comp_id = EDDx-Type(%u)",
				alloc_comp_id );
            *lower_mem_ptr_ptr = LSA_NULL;
			PSI_FATAL( 0 );
		}
	}
}

LSA_VOID psi_edd_free_nrt_tx_mem(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       free_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user free the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		free_comp_id = pSys->comp_id_lower;
	}
	else
	{
		free_comp_id = lsa_comp_id;
	}

	switch ( free_comp_id ) // EDDx variant ?
	{
	    case LSA_COMP_ID_EDDI:
	    case LSA_COMP_ID_EDDP:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_free_nrt_tx_mem(): ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

			// free NRT buffer to NRT memory pool
			PSI_NRT_FREE_TX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_free_nrt_tx_mem(): ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // free NRT buffer to NRT memory pool
			    PSI_NRT_FREE_TX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
            }
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"psi_edd_free_nrt_tx_mem(): invalid free_comp_id = EDDx-Type(%u)",
				free_comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
			PSI_FATAL( 0 );
		}
	}
}

LSA_VOID psi_edd_free_nrt_rx_mem(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16       free_comp_id;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys );

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user free the memory ?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		free_comp_id = pSys->comp_id_lower;
	}
	else
	{
		free_comp_id = lsa_comp_id;
	}

	switch ( free_comp_id ) // EDDx variant ?
	{
	    case LSA_COMP_ID_EDDI:
	    case LSA_COMP_ID_EDDP:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_free_nrt_rx_mem(): ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

			// free NRT buffer to NRT memory pool
			PSI_NRT_FREE_RX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
		}
		break;
	    case LSA_COMP_ID_EDDS:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"psi_edd_free_nrt_rx_mem(): ptr(%08x) lsa_comp_id(%u)",
				lower_mem_ptr, lsa_comp_id );

            if ( psi_hd_is_edd_nrt_copy_if_on( pSys->hd_nr ) )
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // free NRT buffer to NRT memory pool
			    PSI_NRT_FREE_RX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNRT, lsa_comp_id );
            }
		}
		break;
	    default:
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"psi_edd_free_nrt_rx_mem(): invalid free_comp_id = EDDx-Type(%u)",
				free_comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
			PSI_FATAL( 0 );
		}
	}
}

/*=============================================================================*/
/*   Requests                                                                  */
/*=============================================================================*/

LSA_UINT16 edd_get_mailbox_id( LSA_VOID_PTR_TYPE rqb_ptr,
							   LSA_UINT16        comp_id_upper,
							   LSA_SYS_PTR_TYPE  sys_ptr )
{
	LSA_UINT16              result  = PSI_MBX_ID_MAX;
	EDD_UPPER_RQB_PTR_TYPE  p_rqb   = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_SYS_PTR_TYPE        pSys    = (PSI_SYS_PTR_TYPE)sys_ptr;
	LSA_UINT32              service = EDD_RQB_GET_SERVICE( p_rqb );

	PSI_ASSERT( pSys );

	// lookup for MBX-ID for EDDx lower request
	switch ( pSys->comp_id_lower )
	{
		case LSA_COMP_ID_EDDS:
		{
			// Note: In first step all request are mapped to one mailbox
            #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			result = PSI_MBX_ID_EDDS_RQB;
            #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			result = PSI_MBX_ID_EDDS_LOW;
            #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
		}
		break;
		case LSA_COMP_ID_EDDP:
		{
			// Note: In first step all request are mapped to one mailbox
			result = PSI_MBX_ID_EDDP_HIGH;
		}
		break;
		case LSA_COMP_ID_EDDI:
		{
			result = PSI_MBX_ID_EDDI_REST;  // default for all lower request

			// The mailbox (tasks) depends on service opcode
			if (( service == EDD_SRV_NRT_RECV ) || 
				( service == EDD_SRV_NRT_SEND ) || 
				( service == EDD_SRV_NRT_CANCEL))
			{
				result = PSI_MBX_ID_EDDI_NRT_LOW;

				if (( comp_id_upper == LSA_COMP_ID_GSY  ) || // ORG Level ?
					( comp_id_upper == LSA_COMP_ID_MRP  ) ||
					( comp_id_upper == LSA_COMP_ID_LLDP ))
				{
					result = PSI_MBX_ID_EDDI_NRT_ORG;
				}
			}

			if ( service == EDD_SRV_NRT_SET_DCP_FILTER )
			{
				result = PSI_MBX_ID_EDDI_NRT_LOW;
			}

			if (( service == EDD_SRV_NRT_SEND_TIMESTAMP ) || 
				( service == EDD_SRV_SYNC_RECV   ) || 
				( service == EDD_SRV_SYNC_SEND ) || 
				( service == EDD_SRV_SYNC_CANCEL ) || 
				( service == EDD_SRV_SYNC_SET_RATE ))
			{
				result = PSI_MBX_ID_EDDI_NRT_ORG; 
			}
		}
		break;
		default: break;
	}

	PSI_ASSERT( result != PSI_MBX_ID_MAX );

	return result;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT edd_get_open_fct( LSA_UINT16 comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch ( comp_id_edd )
	{
		case LSA_COMP_ID_EDDS:
		{
            #if ( PSI_CFG_USE_EDDS == 1 )
			p_func = (EDD_REQUEST_FCT)edds_open_channel;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDP:
		{
            #if ( PSI_CFG_USE_EDDP == 1 )
			p_func = (EDD_REQUEST_FCT)eddp_open_channel;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDI:
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
			p_func = (EDD_REQUEST_FCT)eddi_open_channel;
            #endif
		}
		break;
		default: break;
	}

	PSI_ASSERT( p_func != LSA_NULL );

	return p_func;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT edd_get_close_fct( LSA_UINT16 comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch ( comp_id_edd )
	{
		case LSA_COMP_ID_EDDS:
		{
            #if ( PSI_CFG_USE_EDDS == 1 )
			p_func = (EDD_REQUEST_FCT)edds_close_channel;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDP:
		{
            #if ( PSI_CFG_USE_EDDP == 1 )
			p_func = (EDD_REQUEST_FCT)eddp_close_channel;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDI:
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
			p_func = (EDD_REQUEST_FCT)eddi_close_channel;
            #endif
		}
		break;
		default: break;
	}

	PSI_ASSERT( p_func != LSA_NULL );

	return p_func;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT edd_get_request_fct( LSA_UINT16 comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch ( comp_id_edd )
	{
		case LSA_COMP_ID_EDDS:
		{
            #if ( PSI_CFG_USE_EDDS == 1 )
			p_func = (EDD_REQUEST_FCT)edds_request;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDP:
		{
            #if ( PSI_CFG_USE_EDDP == 1 )
			p_func = (EDD_REQUEST_FCT)eddp_request;
            #endif
		}
		break;
		case LSA_COMP_ID_EDDI:
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
			p_func = (EDD_REQUEST_FCT)eddi_request;
            #endif
		}
		break;
		default: break;
	}

	PSI_ASSERT( p_func != LSA_NULL );

	return p_func;
}

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI EDDx adaption support")
#endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
