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
/*  F i l e               &F: psi_hd.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI HD instance                                           */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      3
#define LTRC_ACT_MODUL_ID  3

#include "psi_int.h"
#include "psi_hd.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*---------------------------------------------------------------------------*/
/*  Types                                                                    */
/*---------------------------------------------------------------------------*/

typedef enum psi_hd_state_enum {
	PSI_HD_INIT_STATE = 0,  // Reserved for invalid
	PSI_HD_INIT_DONE,       // INIT sequence done
	PSI_HD_OPENING,         // HD Open sequence in progress (startup)
	PSI_HD_OPEN_DONE,       // HD Open finished (ready for requests)
	PSI_HD_CLOSING          // HD Close sequence in progress (shutdown)
} PSI_HD_STATE_TYPE;

typedef struct psi_hd_instance_tag { // PSI HD instance
	PSI_HD_STATE_TYPE     hd_state;                         // current PSI HD state (see PSI_HD_STATE_TYPE)
	PSI_RQB_PTR_TYPE      hd_open_device_rqb_ptr;           // stored upper RQB from system request HD Open Device
	PSI_RQB_PTR_TYPE      hd_close_device_rqb_ptr;          // stored upper RQB from system request HD Close Device
	PSI_HD_STORE_PTR_TYPE hd_store_ptr[PSI_CFG_MAX_HD_CNT]; // allocated instance data
} PSI_HD_INSTANCE_TYPE, * PSI_HD_INSTANCE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*  Global data                                                              */
/*---------------------------------------------------------------------------*/

static PSI_HD_INSTANCE_TYPE psi_hd_inst = { PSI_HD_INIT_STATE, LSA_NULL };

#if (PSI_CFG_USE_EDDP == 1)
// reserve static memory for gshared SRD RAM used in EDDP open device and IOH 
// Note: need allocation of shared mem from upper side for LD/HD splitted
static EDDP_GSHAREDMEM_TYPE psi_eddp_srd_shmem[PSI_CFG_MAX_HD_CNT];
#endif

/*---------------------------------------------------------------------------*/
static PSI_HD_IF_PTR_TYPE psi_hd_get_if_for_pnio_if_nr( LSA_UINT16 pnio_if_nr )
{
	LSA_UINT16          hd_idx;
	LSA_UINT16          if_idx;
	PSI_HD_IF_PTR_TYPE  p_ret = LSA_NULL;

	// lookup to get the IF matching with PNIO IF nr
	// Note: necessary and used for NARE, TCIP adaption coding (ARP-Sniffer)
	/* Init the PSI HD instance vars */
	PSI_ASSERT( psi_hd_inst.hd_state != PSI_HD_INIT_STATE );
	PSI_ASSERT(( pnio_if_nr > 0 ) && ( pnio_if_nr <= PSI_MAX_PNIO_IF_NR ));

	for ( hd_idx = 0; hd_idx < PSI_CFG_MAX_HD_CNT; hd_idx++ )
	{
		PSI_HD_STORE_PTR_TYPE hd_ptr = psi_hd_inst.hd_store_ptr[hd_idx];

		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			PSI_HD_IF_PTR_TYPE if_ptr = &hd_ptr->hd_if[if_idx];

			if (( if_ptr->is_used ) && (if_ptr->pnio_if_nr == pnio_if_nr )) // Matches
			{
				p_ret = if_ptr;
			}
		}
	}

	return p_ret;
}

/*---------------------------------------------------------------------------*/
PSI_HD_STORE_PTR_TYPE  psi_hd_get_hd_store( LSA_UINT16  const  hd_nr )
{
	PSI_ASSERT((hd_nr) && (hd_nr <= PSI_CFG_MAX_HD_CNT));

	return psi_hd_inst.hd_store_ptr[hd_nr-1];
}

/*---------------------------------------------------------------------------*/
PSI_RQB_PTR_TYPE  psi_hd_get_hd_open_device_rqb_ptr( LSA_VOID )
{
	PSI_ASSERT(psi_hd_inst.hd_open_device_rqb_ptr != LSA_NULL);
	PSI_ASSERT(psi_hd_inst.hd_state == PSI_HD_OPENING);

	return psi_hd_inst.hd_open_device_rqb_ptr;
}

/*---------------------------------------------------------------------------*/
static  LSA_VOID  psi_hd_connect( PSI_HD_INPUT_PTR_TYPE  const  hd_ptr )
{
	LSA_UINT16            const nr_of_if    = hd_ptr->nr_of_if;
    LSA_UINT16            const nr_of_ports = hd_ptr->nr_of_ports;
	LSA_UINT16                  ret;
	LSA_UINT32                  if_idx;
    LSA_UINT32                  port_idx;
	PSI_HD_PARAM_TYPE           hw_param;
	PSI_HD_SYS_ID_TYPE          hw_sys_id;
	PSI_HD_STORE_PTR_TYPE       p_hd_store;

	PSI_ASSERT( hd_ptr != LSA_NULL );
	PSI_ASSERT( (hd_ptr->hd_id) && (hd_ptr->hd_id <= PSI_CFG_MAX_HD_CNT) );

	p_hd_store = psi_hd_inst.hd_store_ptr[hd_ptr->hd_id-1];
	PSI_ASSERT( !p_hd_store->is_used );

	PSI_MEMSET( &hw_param, 0, sizeof(hw_param) );
	PSI_MEMSET( &hw_sys_id, 0, sizeof(hw_sys_id) );

	// set the SYS address information
	hw_sys_id.hd_location = hd_ptr->hd_location;
	hw_sys_id.asic_type   = hd_ptr->asic_type;
	hw_sys_id.rev_nr      = hd_ptr->rev_nr;
	hw_sys_id.edd_type    = hd_ptr->edd_type;

	PSI_GET_HD_PARAM(  /* try connection HD and getting the HW params */
		&ret,
		hd_ptr,
		&hw_sys_id,
		&hw_param);

	if (ret != PSI_OK)
	{
		PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_connect(): getting HD params failed, hd_nr(%u) ret(%u/%#x)",
			hd_ptr->hd_id, ret, ret );
		PSI_FATAL( 0 );
	}

	// Store the HD information for this HD
	p_hd_store->hd_sys_handle  = hw_param.hd_sys_handle;
	p_hd_store->hd_hw.edd_type = hw_param.edd_type;
	p_hd_store->hd_hw.nrt_mem  = hw_param.nrt_mem;
	p_hd_store->hd_hw.pi_mem   = hw_param.pi_mem;
	p_hd_store->hd_hw.hif_mem  = hw_param.hif_mem;
	p_hd_store->hd_hw.edd      = hw_param.edd;

    #if (PSI_CFG_USE_EDDP == 1)
	if (p_hd_store->hd_hw.edd_type == LSA_COMP_ID_EDDP)
	{
		// note: todo replace global storage with shared mem from upper
		p_hd_store->hd_hw.edd.eddp.g_shared_ram_base_ptr = &psi_eddp_srd_shmem[hd_ptr->hd_id-1];
	}
    #endif

    // Preset EDD-NRT-Copy-Interface
	p_hd_store->hd_hw.b_edd_nrt_copy_if_on = LSA_FALSE;

	// Init port map
	PSI_MEMSET(p_hd_store->hd_hw.port_map, 0, sizeof(p_hd_store->hd_hw.port_map));

	for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	{
		PSI_HD_IF_PTR_TYPE  const  p_if = &p_hd_store->hd_if[if_idx];

		if (if_idx < nr_of_if)
		{
			p_if->is_used    = LSA_TRUE;
			p_if->pnio_if_nr = hd_ptr->pnio_if[if_idx].edd_if_id;
			p_if->sys_handle = hw_param.if_sys_handle[if_idx];

			// Copy IF MAC addresses from HD System adaption
			PSI_MEMCPY(p_hd_store->hd_hw.if_mac[if_idx], hw_param.if_mac[if_idx], sizeof(PSI_MAC_TYPE));
			PSI_MEMCPY(hd_ptr->if_mac[if_idx], hw_param.if_mac[if_idx], sizeof(PSI_MAC_TYPE));

			for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
			{
                LSA_UINT32  HwPortId = hw_param.port_map[if_idx][port_idx].hw_port_id;

                if // port invalid?
                   ((port_idx == 0) || (port_idx > nr_of_ports))
                {
                    HwPortId = 0; //HwPortId = 0 = reserved
                }

	            PSI_ASSERT( HwPortId <= PSI_CFG_MAX_PORT_CNT );

			    // copy Port mapping info for each IF
				p_hd_store->hd_hw.port_map[if_idx][port_idx] = hw_param.port_map[if_idx][port_idx];

			    // copy Port Media Type Selector info for each IF 1:1
				p_hd_store->hd_hw.port_media_type_select[if_idx][port_idx] = hw_param.port_media_type_select[if_idx][port_idx];

                // plausible Media Type Auto
                switch (p_hd_store->hd_hw.port_media_type_select[if_idx][port_idx])
                {
                    case PSI_MEDIA_TYPE_SEL_AUTO:
                    {
                        switch (p_hd_store->hd_hw.edd_type)
                        {
                            #if (PSI_CFG_USE_EDDI == 1)
                            case LSA_COMP_ID_EDDI:
                            {
                                if (   (EDD_HW_TYPE_USED_ERTEC_400 != p_hd_store->hd_hw.edd.eddi.device_type)
                                    && (EDD_HW_TYPE_USED_SOC       != p_hd_store->hd_hw.edd.eddi.device_type))
                                {
		                            PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			                            "psi_hd_connect(): getting HD param failed, Media Type Select Auto only allowed with EDDI in connection with ERTEC 400 and SOC1, hd_nr(%u)",
			                            hd_ptr->hd_id );
		                            PSI_FATAL( 0 );
                                }
                                break;
                            }
                            #endif
                            #if (PSI_CFG_USE_EDDP == 1)
                            case LSA_COMP_ID_EDDP: break;
                            #endif
                            default:
                            {
		                        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			                        "psi_hd_connect(): getting HD param failed, Media Type Select Auto only allowed with EDDI and EDDP, hd_nr(%u)",
			                        hd_ptr->hd_id );
		                        PSI_FATAL( 0 );
                            }
                        }
                        //POF port auto detection via dummy-I2C-access is executed later after EDD Device Open
		                p_hd_store->hd_hw.port_media_type[if_idx][port_idx]             = PSI_MEDIA_TYPE_UNKNOWN;
                        p_hd_store->hd_hw.port_media_type_by_HwPortId[if_idx][HwPortId] = PSI_MEDIA_TYPE_UNKNOWN;
                        break;
                    }
                    case PSI_MEDIA_TYPE_SEL_COPPER:
                    {
		                p_hd_store->hd_hw.port_media_type[if_idx][port_idx]             = PSI_MEDIA_TYPE_COPPER;
                        p_hd_store->hd_hw.port_media_type_by_HwPortId[if_idx][HwPortId] = PSI_MEDIA_TYPE_COPPER;
                        break;
                    }
                    case PSI_MEDIA_TYPE_SEL_FIBER:
                    {
		                p_hd_store->hd_hw.port_media_type[if_idx][port_idx]             = PSI_MEDIA_TYPE_FIBER;
                        p_hd_store->hd_hw.port_media_type_by_HwPortId[if_idx][HwPortId] = PSI_MEDIA_TYPE_FIBER;
                        break;
                    }
                    case PSI_MEDIA_TYPE_SEL_WIRELESS:
                    {
		                p_hd_store->hd_hw.port_media_type[if_idx][port_idx]             = PSI_MEDIA_TYPE_WIRELESS;
                        p_hd_store->hd_hw.port_media_type_by_HwPortId[if_idx][HwPortId] = PSI_MEDIA_TYPE_WIRELESS;
                        break;
                    }
                    default:
                    {
		                PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			                "psi_hd_connect(): getting HD param failed, Media Type Select is invalid, hd_nr(%u)",
			                hd_ptr->hd_id );
		                PSI_FATAL( 0 );
                    }
                }
			}
		}
		else
		{
			PSI_MEMSET( p_if, 0, sizeof(PSI_HD_IF_TYPE) );
			PSI_MEMSET( p_hd_store->hd_hw.if_mac[if_idx], 0, sizeof(PSI_MAC_TYPE) );
		}
	}

	for (port_idx = 0; port_idx < nr_of_ports; port_idx++)
	{
	    // Copy Port MAC addresses from HD system adaption 
	    PSI_MEMCPY( p_hd_store->hd_hw.port_mac[port_idx], hw_param.port_mac[port_idx], sizeof(PSI_MAC_TYPE) );
	    PSI_MEMCPY( hd_ptr->port_mac[port_idx], hw_param.port_mac[port_idx], sizeof(PSI_MAC_TYPE) );
	}

	p_hd_store->nr_of_all_ports = hd_ptr->nr_of_all_ports;
	p_hd_store->nr_of_all_if    = hd_ptr->nr_of_all_if;

	p_hd_store->is_used = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_pool_create( LSA_UINT16 hd_id )
{
	// Note: NRT pool is only created for EDDI, EDDP HD
	PSI_HD_STORE_PTR_TYPE p_hd;
	LSA_INT               nrt_pool_handle;

	p_hd = psi_hd_inst.hd_store_ptr[hd_id-1];

    nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
	PSI_CREATE_NRT_MEM_POOL(
		p_hd->hd_hw.nrt_mem.base_ptr,
		p_hd->hd_hw.nrt_mem.size,
		p_hd->hd_sys_handle,
		&nrt_pool_handle,
		PSI_HD_NRT_MEM );

	if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_pool_create(): creating NRT pool failed, hd_nr(%u)",
			hd_id );
		PSI_FATAL( 0 );
	}

	p_hd->nrt_pool_handle = nrt_pool_handle;

	// EDDI needs some more pools for fragmentation
	if (( p_hd->hd_hw.edd_type == LSA_COMP_ID_EDDI ) && 
		( p_hd->hd_hw.edd.eddi.fragmentation_supported ))
	{
        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_dmacw_default_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_dmacw_default_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_DMACW_DEFAULT);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_DMACW_DEFAULT failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_dmacw_default_pool_handle = nrt_pool_handle;

		if ( p_hd->hd_hw.edd.eddi.nrt_rx_default_mem.size )
		{
            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
			PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_rx_default_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_rx_default_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_RX_DEFAULT);

			if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
				    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_RX_DEFAULT failed, hd_nr(%u)",
					hd_id );
				PSI_FATAL( 0 );
			}

			p_hd->nrt_rx_default_pool_handle = nrt_pool_handle;
		}

		if ( p_hd->hd_hw.edd.eddi.nrt_tx_default_mem.size )
		{
            nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
			PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_default_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_default_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_DEFAULT);

			if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
				    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_DEFAULT failed, hd_nr(%u)",
					hd_id );
				PSI_FATAL( 0 );
			}

			p_hd->nrt_tx_default_pool_handle = nrt_pool_handle;
		}

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_low_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_low_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_LOW_FRAG);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_LOW_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_low_frag_pool_handle = nrt_pool_handle;

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mid_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mid_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_MID_FRAG);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_MID_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}
		p_hd->nrt_tx_mid_frag_pool_handle = nrt_pool_handle;

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_high_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_high_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_HIGH_FRAG);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_HIGH_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_high_frag_pool_handle = nrt_pool_handle;

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mgmtlow_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mgmtlow_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_MGMTLOW_FRAG);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_MGMTLOW_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_mgmtlow_frag_pool_handle = nrt_pool_handle;

        nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		PSI_CREATE_NRT_MEM_POOL(p_hd->hd_hw.edd.eddi.nrt_tx_mgmthigh_frag_mem.base_ptr, p_hd->hd_hw.edd.eddi.nrt_tx_mgmthigh_frag_mem.size, p_hd->hd_sys_handle, &nrt_pool_handle, PSI_HD_NRT_TX_MGMTHIGH_FRAG);

		if ( PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle )
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_create(): creating NRT pool PSI_HD_NRT_TX_MGMTHIGH_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_mgmthigh_frag_pool_handle = nrt_pool_handle;
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_pool_delete( LSA_UINT16 hd_id )
{
	// Note: NRT pool is only created for EDDI, EDDP HD
	PSI_HD_STORE_PTR_TYPE p_hd = psi_hd_inst.hd_store_ptr[hd_id-1];
	LSA_UINT16            ret;

	PSI_DELETE_NRT_MEM_POOL(
		&ret,
		p_hd->hd_sys_handle,
		p_hd->nrt_pool_handle,
		PSI_HD_NRT_MEM );

	if ( ret != LSA_RET_OK ) 
	{
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_pool_delete(): deleting NRT pool failed, hd_nr(%u)",
			hd_id );
		PSI_FATAL( 0 );
	}

	p_hd->nrt_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

	// EDDI needs some more pools for fragmentation
	if (( p_hd->hd_hw.edd_type == LSA_COMP_ID_EDDI ) && 
		( p_hd->hd_hw.edd.eddi.fragmentation_supported ))
	{
		PSI_DELETE_NRT_MEM_POOL( &ret, p_hd->hd_sys_handle, p_hd->nrt_dmacw_default_pool_handle, PSI_HD_NRT_DMACW_DEFAULT );

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_DMACW_DEFAULT failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_dmacw_default_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		if ( p_hd->hd_hw.edd.eddi.nrt_rx_default_mem.size )
		{
			PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_rx_default_pool_handle, PSI_HD_NRT_RX_DEFAULT);

			if ( ret != LSA_RET_OK ) 
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
				    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_RX_DEFAULT failed, hd_nr(%u)",
					hd_id );
				PSI_FATAL( 0 );
			}

			p_hd->nrt_rx_default_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		}

		if ( p_hd->hd_hw.edd.eddi.nrt_tx_default_mem.size )
		{
			PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_default_pool_handle, PSI_HD_NRT_TX_DEFAULT);

			if ( ret != LSA_RET_OK ) 
			{
				PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
				    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_DEFAULT failed, hd_nr(%u)",
					hd_id );
				PSI_FATAL( 0 );
			}

			p_hd->nrt_tx_default_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
		}

		PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_low_frag_pool_handle, PSI_HD_NRT_TX_LOW_FRAG);

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_LOW_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_low_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mid_frag_pool_handle, PSI_HD_NRT_TX_MID_FRAG);

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_MID_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_mid_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_high_frag_pool_handle, PSI_HD_NRT_TX_HIGH_FRAG);

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_HIGH_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_high_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mgmtlow_frag_pool_handle, PSI_HD_NRT_TX_MGMTLOW_FRAG);

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_MGMTLOW_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_mgmtlow_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;

		PSI_DELETE_NRT_MEM_POOL(&ret, p_hd->hd_sys_handle, p_hd->nrt_tx_mgmthigh_frag_pool_handle, PSI_HD_NRT_TX_MGMTHIGH_FRAG);

		if ( ret != LSA_RET_OK ) 
		{
			PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL,
			    "psi_hd_pool_delete(): deleting NRT pool PSI_HD_NRT_TX_MGMTHIGH_FRAG failed, hd_nr(%u)",
				hd_id );
			PSI_FATAL( 0 );
		}

		p_hd->nrt_tx_mgmthigh_frag_pool_handle = PSI_NRT_POOL_HANDLE_INVALID;
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_open(
	LSA_UINT16            hd_nr,
	PSI_HD_INPUT_PTR_TYPE p_hd_in,
	PSI_HD_STORE_PTR_TYPE p_hd_store )
{
	PSI_ASSERT(p_hd_in    != LSA_NULL);
	PSI_ASSERT(p_hd_store != LSA_NULL);

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_open(): EDD Open start, hd_nr(%d), edd_type(%#x)", 
        hd_nr, p_hd_in->edd_type);

	switch (p_hd_in->edd_type)
	{
        #if ( PSI_CFG_USE_EDDI == 1 )
	    case LSA_COMP_ID_EDDI:
		{
			PSI_EDDI_SETUP_PTR_TYPE p_data = (PSI_EDDI_SETUP_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_EDDI_SETUP_TYPE) );
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0 , sizeof(PSI_EDDI_SETUP_TYPE));

			// Init and prepare the setup data (using EDDI conv. functions)
			// Note: the calculated PI size is set in HD store
			psi_eddi_prepare_setup_data(p_data, p_hd_in, p_hd_store);

			// set the open session data
			p_data->session.hd_nr         = hd_nr;
			p_data->session.hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->session.hd_sys_handle = p_hd_store->hd_sys_handle;

			// start open EDDI device
			// Note: finish is done after setup sequence (see psi_eddi.c)
			psi_eddi_device_open( p_data );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDP == 1 )
	    case LSA_COMP_ID_EDDP:
		{
			LSA_UINT16  if_idx;

			PSI_EDDP_SETUP_PTR_TYPE p_data = (PSI_EDDP_SETUP_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_EDDP_SETUP_TYPE));
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0 , sizeof(PSI_EDDP_SETUP_TYPE));

			// Init and prepare the setup data (using EDDP init functions)
			psi_eddp_prepare_setup_data( p_data, p_hd_in, p_hd_store ); 

			// set the open session data (needed for async open)
			p_data->session.hd_nr         = hd_nr;
			p_data->session.act_if_idx    = 0;
			p_data->session.nr_of_if      = p_hd_in->nr_of_if;
			p_data->session.hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->session.hd_sys_handle = p_hd_store->hd_sys_handle;

			for (if_idx = 0; if_idx < p_data->session.nr_of_if; if_idx++)
			{
				p_data->session.hIDB_ptr[if_idx] = &p_hd_store->hd_if[if_idx].edd_hIDB;
			}

			// start open EDDP device
			// Note: finish is done after setup sequence (see psi_eddp.c)
			psi_eddp_device_open( &p_hd_store->hd_hw.edd.eddp, p_data );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
	    case LSA_COMP_ID_EDDS:
		{
			PSI_EDDS_SETUP_PTR_TYPE p_data = (PSI_EDDS_SETUP_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_EDDS_SETUP_TYPE));
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0, sizeof(PSI_EDDS_SETUP_TYPE) );

			// Init and prepare the setup data (using EDDI conv. functions)
			// Note: the calculated PI size is set in HD store
			psi_edds_prepare_setup_data( p_data, p_hd_in, p_hd_store );

			// set the open session data
			p_data->session.hd_nr         = hd_nr;
			p_data->session.hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->session.hd_sys_handle = p_hd_store->hd_sys_handle;

			// open EDDS device
			// Note: finish is done after setup sequence (see psi_edds.c)
			psi_edds_device_open( p_data );
		}
		break;
        #endif

	    default:
		{
			PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_hd_edd_open(): invalid EDD_Type(%u/%#x)",
				p_hd_in->edd_type, p_hd_in->edd_type );
			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_open_done( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE p_data )
{
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE       cbf_fct;
	PSI_RQB_PTR_TYPE                const pRQB = psi_hd_get_hd_open_device_rqb_ptr();

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_open_done(): EDD Open finished, hd_nr(%u)", hd_nr );

	// Free the setup data
	psi_free_local_mem( p_data );

	// HD Open Device finished --> response to Upper
	psi_hd_inst.hd_open_device_rqb_ptr = LSA_NULL;

	cbf_fct = pRQB->args.hd_open.psi_request_upper_done_ptr;

	PSI_RQB_SET_RESPONSE( pRQB, PSI_OK );
	psi_hd_inst.hd_state = PSI_HD_OPEN_DONE;

    #if (PSI_CFG_USE_HIF == 1)
	psi_system_request_done( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf_fct, pRQB );
    #else
	psi_system_request_done( PSI_MBX_ID_PSI, cbf_fct, pRQB);
    #endif
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_close(
	LSA_UINT16            hd_nr,
	PSI_HD_STORE_PTR_TYPE p_hd_store  )
{
	LSA_UINT16 edd_type;

	PSI_ASSERT( p_hd_store != LSA_NULL );

	edd_type = p_hd_store->hd_hw.edd_type;

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close(): EDD Close start, hd_nr(%d), edd_type(%#x)", hd_nr, edd_type);

	switch ( edd_type )
	{
        #if ( PSI_CFG_USE_EDDI == 1 )
	    case LSA_COMP_ID_EDDI:
		{
			PSI_EDDI_SESSION_PTR_TYPE p_data = psi_alloc_local_mem( sizeof(PSI_EDDI_SESSION_TYPE));
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0 , sizeof(PSI_EDDI_SESSION_TYPE));

			// set the close session data
			p_data->hd_nr         = hd_nr;
			p_data->hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->hd_sys_handle = p_hd_store->hd_sys_handle;

			// close EDDI device
			// Note: finish is done after close sequence (see psi_eddi.c)
			psi_eddi_device_close( p_data );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDP == 1 )
    	case LSA_COMP_ID_EDDP:
		{
			LSA_UINT16 if_idx;

			PSI_EDDP_SESSION_PTR_TYPE p_data = psi_alloc_local_mem( sizeof(PSI_EDDP_SESSION_TYPE));
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0 , sizeof(PSI_EDDP_SESSION_TYPE));

			// set the close session data (needed for async close)
			p_data->hd_nr         = hd_nr;
			p_data->act_if_idx    = 0;
			p_data->nr_of_if      = p_hd_store->nr_of_if;
			p_data->hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->hd_sys_handle = p_hd_store->hd_sys_handle;

			for ( if_idx = 0; if_idx < p_hd_store->nr_of_if; if_idx++ )
			{
				p_data->hIDB_ptr[if_idx] = &p_hd_store->hd_if[if_idx].edd_hIDB;
			}

			// close EDDP device, starting with closing the EDDP interfaces 
			// Note: finish is done after close sequence (see psi_eddp.c)
			psi_eddp_interface_shutdown( p_data );
		}
		break;
        #endif

        #if ( PSI_CFG_USE_EDDS == 1 )
	    case LSA_COMP_ID_EDDS:
		{
			PSI_EDDS_SESSION_PTR_TYPE p_data = psi_alloc_local_mem( sizeof(PSI_EDDS_SESSION_TYPE));
			PSI_ASSERT( p_data != 0 );

			PSI_MEMSET( p_data, 0 , sizeof(PSI_EDDS_SESSION_TYPE));

			// set the close session data
			p_data->hd_nr         = hd_nr;
			p_data->hDDB_ptr      = &p_hd_store->edd_hDDB;
			p_data->hd_sys_handle = p_hd_store->hd_sys_handle;

			// close EDDS device
			// Note: finish is done after close sequence (see psi_edds.c)
			psi_edds_device_shutdown( p_data );
		}
		break;
        #endif

	    default:
		{
			PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
				"psi_hd_edd_close(): invalid EDD_Type(%u/%#x)",
				edd_type, edd_type );
			PSI_FATAL( 0 );
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_edd_close_done( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE p_data )
{
	PSI_RQB_PTR_TYPE                pRQB;
	PSI_HD_STORE_PTR_TYPE           p_store_ptr;
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE cbf_fct;
	LSA_UINT16                      ret;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_close_done(): EDD Close finished, hd_nr(%u)", hd_nr );

	if ( p_data != LSA_NULL )  // Session data for shutdown allocated?
	{
		psi_free_local_mem( p_data );
	}

	p_store_ptr = psi_hd_inst.hd_store_ptr[hd_nr-1];

	if ( p_store_ptr->runs_on_level_basic ) // HD runs on LD Level?
	{
		LSA_UINT16 if_idx;

		// Deactivate all active detail entries for PNIO IF
		// Note: removing is only done if HD runs not on same level as LD
		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			if ( p_store_ptr->hd_if[if_idx].is_used ) // Activated?
			{
				psi_remove_detail_store( p_store_ptr->hd_if[if_idx].pnio_if_nr );
			}
		}
	}

	// Free the NRT pool
	psi_hd_pool_delete( hd_nr );

	PSI_FREE_HD_PARAM(  // disconnect HD
		&ret,
		hd_nr );

	if ( ret != PSI_OK )
	{
		PSI_HD_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_edd_close_done(): free HD params failed, hd_nr(%u) ret(%u/%#x)",
			hd_nr, ret, ret );
		PSI_FATAL( 0 );
	}

	// Init the HD storage
	PSI_MEMSET( p_store_ptr, 0, sizeof( PSI_HD_STORE_TYPE ));

	// HD CLOSE finished --> response to Upper
	pRQB = psi_hd_inst.hd_close_device_rqb_ptr;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;

	cbf_fct = pRQB->args.hd_close.psi_request_upper_done_ptr;

	PSI_RQB_SET_RESPONSE( pRQB, PSI_OK );
	psi_hd_inst.hd_state = PSI_HD_INIT_DONE;

    #if (PSI_CFG_USE_HIF == 1)
	psi_system_request_done( PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)cbf_fct, pRQB );
    #else
	psi_system_request_done( PSI_MBX_ID_PSI, cbf_fct, pRQB);
    #endif
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_hd_init( LSA_VOID )
{
	LSA_UINT16 hd_idx;
	LSA_UINT16 if_idx;

	/* Init the PSI HD instance vars */
	PSI_ASSERT( psi_hd_inst.hd_state == PSI_HD_INIT_STATE );

	for ( hd_idx = 0; hd_idx < PSI_CFG_MAX_HD_CNT; hd_idx++ )
	{
		PSI_HD_STORE_PTR_TYPE hd_ptr;

		hd_ptr = (PSI_HD_STORE_PTR_TYPE)psi_alloc_local_mem( sizeof(PSI_HD_STORE_TYPE) );
		PSI_ASSERT( hd_ptr != 0 );

		hd_ptr->is_used                         = LSA_FALSE;
		hd_ptr->edd_hDDB                        = 0;
		hd_ptr->hd_sys_handle                   = 0;
		hd_ptr->nrt_pool_handle                 = PSI_NRT_POOL_HANDLE_INVALID;
		hd_ptr->nrt_dmacw_default_pool_handle   = 0;
		hd_ptr->nrt_rx_default_pool_handle      = 0;
		hd_ptr->nrt_tx_default_pool_handle      = 0;
		hd_ptr->nrt_tx_low_frag_pool_handle     = 0;
		hd_ptr->nrt_tx_mid_frag_pool_handle     = 0;
		hd_ptr->nrt_tx_high_frag_pool_handle    = 0;
		hd_ptr->nrt_tx_mgmtlow_frag_pool_handle = 0;
		hd_ptr->nrt_tx_mgmthigh_frag_pool_handle= 0;

		PSI_MEMSET( &hd_ptr->hd_hw, 0, sizeof( PSI_HD_HW_TYPE ));

		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			hd_ptr->hd_if[if_idx].is_used    = LSA_FALSE;
			hd_ptr->hd_if[if_idx].sys_handle = 0;
			hd_ptr->hd_if[if_idx].edd_hIDB   = 0;
			hd_ptr->hd_if[if_idx].pnio_if_nr = 0;
			hd_ptr->hd_if[if_idx].is_arp_on  = LSA_FALSE;
		}

		psi_hd_inst.hd_store_ptr[hd_idx] = hd_ptr;
	}

    psi_hd_inst.hd_open_device_rqb_ptr  = LSA_NULL;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;
	psi_hd_inst.hd_state                = PSI_HD_INIT_DONE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_undo_init( LSA_VOID )
{
	LSA_UINT16 hd_idx;
	LSA_UINT16 if_idx;

	// Undo INIT all PSI HD instance vars
	PSI_ASSERT( psi_hd_inst.hd_state == PSI_HD_INIT_DONE );

	/* cleanup HD IF vars */
	for ( hd_idx = 0; hd_idx < PSI_CFG_MAX_HD_CNT; hd_idx++ )
	{
		PSI_HD_STORE_PTR_TYPE hd_ptr = psi_hd_inst.hd_store_ptr[hd_idx];
		
		PSI_ASSERT( !hd_ptr->is_used );

		for ( if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++ )
		{
			PSI_ASSERT( !hd_ptr->hd_if[if_idx].is_used );
		}

		psi_free_local_mem( hd_ptr );
		hd_ptr = 0;
	}

    psi_hd_inst.hd_open_device_rqb_ptr  = LSA_NULL;
	psi_hd_inst.hd_close_device_rqb_ptr = LSA_NULL;
	psi_hd_inst.hd_state                = PSI_HD_INIT_STATE;
}

/*---------------------------------------------------------------------------*/
/*  Request interface                                                        */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_hd_open_device( PSI_RQB_PTR_TYPE rqb_ptr )
{
	LSA_UINT16                 if_idx;
	PSI_UPPER_HD_OPEN_PTR_TYPE p_open;
	PSI_HD_INPUT_PTR_TYPE      p_hd_in;
	PSI_HD_OUTPUT_PTR_TYPE     p_hd_out;
	PSI_HD_STORE_PTR_TYPE      p_hd_store;

	// check params
	p_open = &rqb_ptr->args.hd_open;

	if (( p_open->hd_args.hd_id == 0 ) || ( p_open->hd_args.hd_id > PSI_CFG_MAX_HD_CNT )) // wrong HD count
	{
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_open_device(): invalid params, hd_id(%u) rqb_ptr(%#x)",
			p_open->hd_args.hd_id, rqb_ptr );
		PSI_FATAL( 0 );
	}

	if (( p_open->hd_args.nr_of_if == 0 ) || ( p_open->hd_args.nr_of_if > PSI_CFG_MAX_HD_IF_CNT )) // wrong HD count
	{
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_open_device(): invalid params, if_count(%u) rqb_ptr(%#x)",
			p_open->hd_args.nr_of_if, rqb_ptr );
		PSI_FATAL( 0 );
	}
	
	// Check if the HD environment is set to another EDDI config. 
#ifdef EDDI_CFG_SOC 
	if (   (p_open->hd_args.asic_type == PSI_ASIC_TYPE_ERTEC400) 
	    || (p_open->hd_args.asic_type == PSI_ASIC_TYPE_ERTEC200))
	{
	    // has to be SOC1, PNIP or EDDS
      PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
          "psi_hd_open_device(): invalid eddi config. This firmware is compiled to run with SOC1." );
      PSI_FATAL( 0 );
	}
#endif
#ifdef EDDI_CFG_ERTEC_400
    if (   (p_open->hd_args.asic_type == PSI_ASIC_TYPE_SOC1) 
        || (p_open->hd_args.asic_type == PSI_ASIC_TYPE_ERTEC200))
  {
      // has to be ERTEC400, PNIP or EDDS
      PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
          "psi_hd_open_device(): invalid eddi config. This firmware is compiled to run with ERTEC400." );
      PSI_FATAL( 0 );
  }
#endif 
#ifdef EDDI_CFG_ERTEC_200
    if (   (p_open->hd_args.asic_type == PSI_ASIC_TYPE_SOC1) 
        || (p_open->hd_args.asic_type == PSI_ASIC_TYPE_ERTEC400))
  {
      // has to be ERTEC200, PNIP or EDDS
      PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL,
          "psi_hd_open_device(): invalid eddi config. This firmware is compiled to run with ERTEC200." );
      PSI_FATAL( 0 );
  }
#endif

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_device(): hd_id(%u) if_count(%u)", 
		p_open->hd_args.hd_id, p_open->hd_args.nr_of_if );

	// Start HD Open Device
	psi_hd_inst.hd_open_device_rqb_ptr = rqb_ptr;
	psi_hd_inst.hd_state               = PSI_HD_OPENING;

	p_hd_in    = &p_open->hd_args;
	p_hd_out   = &p_open->hd_out;
	p_hd_store = psi_hd_inst.hd_store_ptr[p_hd_in->hd_id -1];

	// get the HD HW params
	psi_hd_connect(p_hd_in);

	// create the NRT pool for this HD
	psi_hd_pool_create(p_hd_in->hd_id);

	// store the HD level
	p_hd_store->runs_on_level_basic = p_hd_in->runs_on_level_basic;
	p_hd_store->nr_of_if            = p_open->hd_args.nr_of_if;
    p_hd_store->nr_of_ports         = p_hd_in->nr_of_ports;

	// preset hw_port outputs like media_type
	psi_res_calc_set_hw_port_outputs( p_hd_out, p_hd_store );

	// Calculate the channel details for all HD IFs from the inputs
	// Note: this information is needed for XXX_GET_PATH_INFO during open_channel_xx
	for (if_idx = 0; if_idx < p_open->hd_args.nr_of_if; if_idx++)
	{
		LSA_UINT16 pnio_if_nr = p_hd_in->pnio_if[if_idx].edd_if_id;
		LSA_UINT16 if_nr      = (LSA_UINT16)p_hd_in->pnio_if[if_idx].if_id;

		if (p_hd_store->runs_on_level_basic) // Runs on HD level?
		{
			// Activate entry for PNIO IF
			// Note: adding is only done if HD runs not on same level as LD
			psi_add_detail_store( pnio_if_nr, p_hd_in->hd_id, if_nr );
		}

		// Calculate the details for this interface
		psi_res_calc_set_if_details( 
			if_nr, 
			p_hd_store->hd_if[if_idx].sys_handle,
			p_hd_store->nrt_pool_handle,
			p_hd_in);

		// Calculate the output details for this interface
		psi_res_calc_set_if_outputs( if_nr, p_hd_in, p_hd_out );
	}

	// Start open EDDx Device
	psi_hd_edd_open( p_hd_in->hd_id, p_hd_in, p_hd_store );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_close_device( PSI_RQB_PTR_TYPE rqb_ptr )
{
	PSI_HD_STORE_PTR_TYPE p_store_ptr;

	// check the params
	PSI_UPPER_HD_CLOSE_PTR_TYPE p_close = &rqb_ptr->args.hd_close;

	if (( p_close->hd_id == 0 ) || ( p_close->hd_id > PSI_CFG_MAX_HD_CNT )) // wrong HD ID
	{
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL,
			"psi_hd_close_device(): invalid params, hd_id(%u) rqb_ptr(%#x)",
			p_close->hd_id, rqb_ptr );
		PSI_FATAL( 0 );
	}

	PSI_LD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_close_device(): hd_id(%u)", p_close->hd_id );

    // Start HD Close Device
	psi_hd_inst.hd_close_device_rqb_ptr = rqb_ptr;
	psi_hd_inst.hd_state                = PSI_HD_CLOSING;

	p_store_ptr = psi_hd_inst.hd_store_ptr[p_close->hd_id -1];

	// Close EDD device first
	psi_hd_edd_close( (LSA_UINT16)p_close->hd_id, p_store_ptr );
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_set_arp_on_state( LSA_UINT16 pnio_if_nr, LSA_BOOL state )
{
	PSI_HD_IF_PTR_TYPE p_if;

	PSI_ASSERT( pnio_if_nr && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR) );  // valid range?

	p_if = psi_hd_get_if_for_pnio_if_nr( pnio_if_nr );

	if ( p_if != LSA_NULL )
	{
		p_if->is_arp_on = state;
	}
}

/*---------------------------------------------------------------------------*/
LSA_BOOL psi_hd_is_arp_on_state( LSA_UINT16 pnio_if_nr )
{
	PSI_HD_IF_PTR_TYPE p_if;
	LSA_BOOL           b_result;

	PSI_ASSERT( pnio_if_nr && (pnio_if_nr <= PSI_MAX_PNIO_IF_NR) ); // valid range?

	p_if = psi_hd_get_if_for_pnio_if_nr( pnio_if_nr );

	b_result = ( p_if != LSA_NULL ) ? p_if->is_arp_on : LSA_FALSE;

	return b_result;
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_hd_get_pi_size( LSA_UINT16 const hd_nr )
{
	LSA_UINT32 pi_size;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	// Note: in case of EDDI, size is reduced by conv. functions
	pi_size = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.pi_mem.size;

	return pi_size;
}

/*---------------------------------------------------------------------------*/
PSI_PORT_MEDIA_TYPE psi_hd_get_psi_media_type(
    LSA_UINT16 const hd_nr,
	LSA_UINT32 const if_nr,
	LSA_UINT32 const HwPortId )
{
	PSI_PORT_MEDIA_TYPE psi_media_type;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( if_nr && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );
	PSI_ASSERT( HwPortId && (HwPortId <= PSI_CFG_MAX_PORT_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

    psi_media_type = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.port_media_type_by_HwPortId[if_nr-1][HwPortId];

	return psi_media_type;
}

/*---------------------------------------------------------------------------*/
LSA_INT psi_hd_get_nrt_pool_handle( LSA_UINT16 const hd_nr )
{
	LSA_INT nrt_pool_handle;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	// Return HD NRT Pool Handle (needed for HIF_NRT_ALLOC_xxx(), HIF_NRT_FREE_xxx())
	nrt_pool_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->nrt_pool_handle;

	return nrt_pool_handle;
}

/*---------------------------------------------------------------------------*/
LSA_BOOL psi_hd_is_edd_nrt_copy_if_on( LSA_UINT16 const hd_nr )
{
	LSA_BOOL b_edd_nrt_copy_if_on;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	// Return boolean with HD EDD NRT Copy Interface activation (needed for HIF_NRT_ALLOC_xxx(), HIF_NRT_FREE_xxx(), ...)
    // (with active EDD NRT Copy Interface applications can use local memory instead of NRT memory)
    b_edd_nrt_copy_if_on = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.b_edd_nrt_copy_if_on;

	return b_edd_nrt_copy_if_on;
}

/*---------------------------------------------------------------------------*/
/*  Access to EDDx                                                           */
/*---------------------------------------------------------------------------*/

PSI_EDD_HDDB psi_hd_get_hDDB( LSA_UINT16 const hd_nr )
{
	PSI_EDD_HDDB edd_handle;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	edd_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->edd_hDDB;

	return edd_handle;
}

/*---------------------------------------------------------------------------*/
PSI_EDD_HIDB psi_hd_get_hIDB( LSA_UINT16 const hd_nr, LSA_UINT16 const if_nr )
{
	PSI_EDD_HIDB edd_handle;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( if_nr && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	edd_handle = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_if[if_nr-1].edd_hIDB;

	return edd_handle;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_get_if_mac( 
	LSA_UINT16     hd_nr,
	LSA_UINT16     if_nr,
	LSA_UINT32 *   if_ptr,
	PSI_MAC_TYPE * mac_ptr )
{
	PSI_HD_STORE_PTR_TYPE hd_ptr;
	PSI_HD_IF_PTR_TYPE    hd_if_ptr;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( if_nr && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	hd_ptr = psi_hd_inst.hd_store_ptr[hd_nr-1];
	PSI_ASSERT( hd_ptr->is_used );
	
	hd_if_ptr = &hd_ptr->hd_if[if_nr-1];
	PSI_ASSERT( hd_if_ptr->is_used );

	PSI_MEMCPY( mac_ptr, hd_ptr->hd_hw.if_mac[0], sizeof (PSI_MAC_TYPE) );

	*if_ptr = hd_if_ptr->pnio_if_nr;
}

/*---------------------------------------------------------------------------*/
/*  Access to EDDx IR-Handling                                               */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_hd_interrupt( LSA_UINT16 hd_nr, LSA_UINT32 int_src )
{
	PSI_HD_STORE_PTR_TYPE p_hd;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];

    PSI_HD_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_interrupt(): hd_nr(%u), int_src(%#x), edd_type(%#x), edd_hDDB(0x%X)", 
        hd_nr, int_src, p_hd->hd_hw.edd_type, p_hd->edd_hDDB);

    if (p_hd->edd_hDDB != 0)
	{
		switch ( p_hd->hd_hw.edd_type ) /* Type of EDDx */
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
		    case LSA_COMP_ID_EDDI:
			{
                /* only allowed after EDDx_SRV_DEV_OPEN and before EDDx_SRV_DEV_CLOSE */
                if (psi_hd_inst.hd_state != PSI_HD_CLOSING)
                {
				    eddi_interrupt( p_hd->edd_hDDB );
                    #if ( defined(EDDI_CFG_SOC) || defined(EDDI_CFG_SII_USE_IRQ_SP) ) && !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
				    eddi_interrupt_set_eoi( p_hd->edd_hDDB, 0x0F );  // external ICU
                    #endif
                }
			}
			break;
            #endif
            #if ( PSI_CFG_USE_EDDP == 1 )
		    case LSA_COMP_ID_EDDP:
			{
				eddp_interrupt( p_hd->edd_hDDB, int_src );
			}
			break;
            #endif
            #if ( PSI_CFG_USE_EDDS == 1 )
		    case LSA_COMP_ID_EDDS:
			{
                #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                edds_scheduler( p_hd->edd_hDDB );
                #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
				edds_interrupt( p_hd->edd_hDDB, int_src );
                #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
			}
			break;
            #endif
		    default:
			{
				LSA_UNUSED_ARG( int_src );
			}
		}
	}
}

/*-------------------------------------------------------------------------------------*/
/* int from external timer occured (replaces NEWCYCLE-Int) */
LSA_VOID psi_eddi_exttimer_interrupt( LSA_UINT16 hd_nr)
{
	PSI_HD_STORE_PTR_TYPE p_hd;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );

	p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];

    PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_eddi_exttimer_interrupt(): hd_nr(%u), p_hd->edd_hDDB(0x%X)", hd_nr, p_hd->edd_hDDB);

    /* only allowed after EDDx_SRV_DEV_OPEN and before EDDx_SRV_DEV_CLOSE */
	if ( (p_hd->edd_hDDB != 0) && (psi_hd_inst.hd_state != PSI_HD_CLOSING) )
	{
		switch ( p_hd->hd_hw.edd_type ) /* Type of EDDx */
		{
            #if ( PSI_CFG_USE_EDDI == 1 )
			case LSA_COMP_ID_EDDI:
			{
				eddi_ExtTimerInterrupt( p_hd->edd_hDDB );
                #if !(defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_USE_SPECIAL_EOI)) // EOI must not be set for polling mode or sepcial EOI mode
				eddi_interrupt_set_eoi( p_hd->edd_hDDB, 0x0F );  // external ICU
                #endif
			}
			break;
            #endif
			default: break;
		}
	}
}

#if ( PSI_CFG_USE_EDDS == 1 )

/*-------------------------------------------------------------------------------------*/
/* polling EDDS interrupts (using LL adaption)                                         */
/*-------------------------------------------------------------------------------------*/

LSA_BOOL psi_hd_isr_poll( LSA_UINT16 hd_nr )
{
    PSI_HD_STORE_PTR_TYPE p_hd;
    LSA_BOOL              b_more_irq = LSA_FALSE;

    PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
    p_hd = psi_hd_inst.hd_store_ptr[hd_nr-1];

    PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_isr_poll(): hd_nr(%u), p_hd->edd_hDDB(0x%X)", hd_nr, p_hd->edd_hDDB);

    // only allowed after EDDx_SRV_DEV_OPEN and before EDDx_SRV_DEV_CLOSE
    if ( (p_hd->edd_hDDB != 0) && (psi_hd_inst.hd_state != PSI_HD_CLOSING) )
    {
        #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        edds_scheduler( p_hd->edd_hDDB );
        #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        {
            LSA_UINT32 int_src;

            do
            {
                b_more_irq = edds_interrupt_poll( p_hd->edd_hDDB, &int_src );
                if ( EDDS_INTERRUPT_NONE != int_src ) // interrupt present?
                {
                    edds_interrupt( p_hd->edd_hDDB, int_src );
                }
                else
                {
                    break;
                }
            } while (b_more_irq);
        }
        #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
    }

    return b_more_irq;
}

/*-------------------------------------------------------------------------------------*/
/* access ot EDDS PI memory managment (see edds setup)                                 */
/*-------------------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE psi_hd_get_edd_io_handle( LSA_UINT16 hd_nr )
{
	LSA_VOID_PTR_TYPE pIO = LSA_NULL;

	PSI_ASSERT( hd_nr && (hd_nr <= PSI_CFG_MAX_HD_CNT) );
	PSI_ASSERT( psi_hd_inst.hd_store_ptr[hd_nr-1] != LSA_NULL );

	pIO = psi_hd_inst.hd_store_ptr[hd_nr-1]->hd_hw.edd.edds.io_buffer_handle;

	return pIO;
}

#endif //PSI_CFG_USE_EDDS

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
