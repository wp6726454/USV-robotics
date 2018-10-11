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
/*  F i l e               &F: psi_res_calc.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI resource calculation and storage based on input data  */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      6
#define LTRC_ACT_MODUL_ID  6

#include "psi_int.h"
#include "psi_res_calc.h"

#include "dcp_cfg.h"  // DCP_NUM_xx
#include "oha_cfg.h"  // OHA_CFG_MAX_SENDER
#include "cm_inc.h"   // CM header info

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#define PSI_INVALID_PORT_ID  0xFFFF
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct psi_pnio_if_entry_tag {   // Storage type for a channel PNIO IF entry (global or IF)
	LSA_BOOL                 is_used;                     // PNIO IF already in use yes/no
	LSA_UINT16               hd_nr;                       // HD NR for IF
	LSA_UINT16               if_nr;                       // IF NR for IF
	PSI_DETAIL_STORE_TYPE    details;                     // Detail storage
} PSI_PNIO_IF_ENTRY_TYPE, *PSI_PNIO_IF_ENTRY_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_pnio_if_store_tag { // PSI PNIO storage instance
	LSA_BOOL                    init_done;                         // init done yes/no
	PSI_PNIO_IF_ENTRY_PTR_TYPE  entry_ptr[PSI_MAX_PNIO_IF_NR+1];   // PNIO IF entry
} PSI_PNIO_IF_INST_TYPE, *PSI_PNIO_IF_INST_PTR_TYPE;


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static PSI_PNIO_IF_INST_TYPE psi_pnio_store = { LSA_FALSE, {LSA_NULL} };

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_get_nr_of_user_ports(
	LSA_UINT16              if_nr, 
	PSI_HD_INPUT_PTR_TYPE   p_hd_in )
{
	LSA_UINT16 idx;
	LSA_UINT16 result = 0;

	PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT((if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT));

	// lookup in HW port entries for user ports configured for this IF
	for (idx = 0; idx < p_hd_in->nr_of_ports; idx++)
	{
		if (p_hd_in->hw_port[idx].if_nr == if_nr) // IF nr matches
		{
			result++;
		}
	}

	return result;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_init_details_acp( 
	PSI_RT_DETAIL_PTR_TYPE  rt_detail_ptr )
{
	PSI_ASSERT( rt_detail_ptr != 0 );

	rt_detail_ptr->frame_id_base_class1_unicast     = 0;
	rt_detail_ptr->nr_of_consumers_class1_unicast   = 0;
	rt_detail_ptr->frame_id_base_class1_multicast   = 0;
	rt_detail_ptr->nr_of_consumers_class1_multicast = 0;

	rt_detail_ptr->frame_id_base_class2_unicast     = 0;
	rt_detail_ptr->nr_of_consumers_class2_unicast   = 0;
	rt_detail_ptr->frame_id_base_class2_multicast   = 0;
	rt_detail_ptr->nr_of_consumers_class2_multicast = 0;

	rt_detail_ptr->nr_of_providers_class1_2 = 0;
	rt_detail_ptr->nr_of_consumers_class3   = 0;
	rt_detail_ptr->nr_of_providers_class3   = 0;

	rt_detail_ptr->alarm_ref_base  = 0;
	rt_detail_ptr->alarm_nr_of_ref = 0;
	rt_detail_ptr->group_id        = 0;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmcl( 
	LSA_UINT16              if_nr, 
	PSI_RT_DETAIL_PTR_TYPE  rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE   p_hd_in )
{
	PSI_HD_IF_INPUT_PTR_TYPE p_if;

	PSI_ASSERT( rt_detail_ptr != LSA_NULL );
	PSI_ASSERT( p_hd_in  != LSA_NULL );
	PSI_ASSERT( (if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&p_hd_in->pnio_if[if_nr-1];

	// Prepare the ACP settings based on used EDDx
	switch ( p_hd_in->edd_type )
	{
	case LSA_COMP_ID_EDDP:
	case LSA_COMP_ID_EDDI:
		{
			// Combined usage of RTC 1/2
			rt_detail_ptr->frame_id_base_class1_unicast     = 0xC000;
			rt_detail_ptr->nr_of_consumers_class1_unicast   = p_if->ioc.nr_of_rt_devices;
			rt_detail_ptr->frame_id_base_class1_multicast   = 0; 
			rt_detail_ptr->nr_of_consumers_class1_multicast = 0; 

			rt_detail_ptr->frame_id_base_class2_unicast     = 0x8000;
			rt_detail_ptr->nr_of_consumers_class2_unicast   = p_if->ioc.nr_of_rt_devices; 
			rt_detail_ptr->frame_id_base_class2_multicast   = 0; 
			rt_detail_ptr->nr_of_consumers_class2_multicast = 0;

			rt_detail_ptr->nr_of_providers_class1_2 = p_if->ioc.nr_of_rt_devices;

			rt_detail_ptr->nr_of_consumers_class3 = p_if->ioc.nr_of_irt_devices;
			rt_detail_ptr->nr_of_providers_class3 = p_if->ioc.nr_of_irt_devices;;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = rt_detail_ptr->nr_of_providers_class1_2 + rt_detail_ptr->nr_of_providers_class3;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMCL;
		}
		break;
	case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			LSA_UINT16 rt_cons_iod = p_if->iod.nr_of_instances * p_if->iod.nr_of_ar;
			LSA_UINT16 rt_cons     = p_if->ioc.nr_of_rt_devices - p_if->ioc.nr_of_qv_consumer;
			LSA_UINT16 rt_prov     = p_if->ioc.nr_of_rt_devices - p_if->ioc.nr_of_qv_provider;

			rt_detail_ptr->frame_id_base_class1_unicast     = 0xF800 - rt_cons_iod - rt_cons; // before IOD-UC
			rt_detail_ptr->nr_of_consumers_class1_unicast   = rt_cons;
			rt_detail_ptr->frame_id_base_class1_multicast   = 0;
			rt_detail_ptr->nr_of_consumers_class1_multicast = 0;

			rt_detail_ptr->frame_id_base_class2_unicast     = 0xBC00 - rt_cons_iod - rt_cons; // before IOD-UC
			rt_detail_ptr->nr_of_consumers_class2_unicast   = rt_cons;
			rt_detail_ptr->frame_id_base_class2_multicast   = 0;
			rt_detail_ptr->nr_of_consumers_class2_multicast = 0;

			rt_detail_ptr->nr_of_providers_class1_2 = rt_prov + rt_prov;

			rt_detail_ptr->nr_of_consumers_class3 = 0;
			rt_detail_ptr->nr_of_providers_class3 = 0;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = rt_detail_ptr->nr_of_providers_class1_2 + rt_detail_ptr->nr_of_providers_class3;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMCL;
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_set_details_acp_cmcl(): invalid EDDx type, if_nr(%u), edd_type(%u/%#x)",
				if_nr, 
				p_hd_in->edd_type,
				p_hd_in->edd_type );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmsv(
	LSA_UINT16              if_nr, 
	PSI_RT_DETAIL_PTR_TYPE  rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE   p_hd_in )
{
	LSA_UINT16               nr_of_rt_uc;
	LSA_UINT16               nr_of_rt_mc;
	LSA_UINT16               nr_of_irt_uc;
	LSA_UINT16               nr_of_irt_mc;
	PSI_HD_IF_INPUT_PTR_TYPE p_if;

	PSI_ASSERT( rt_detail_ptr != LSA_NULL );
	PSI_ASSERT( p_hd_in  != LSA_NULL );
	PSI_ASSERT( (if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&p_hd_in->pnio_if[if_nr-1];

	// Prepare the ACP settings based on used EDDx
	// configure for each RT/IRT AR one IO-CR and M-CR
	// Note: M-CR only set if IOD-MC feature is configured
	nr_of_rt_uc = p_if->iod.nr_of_instances * p_if->iod.nr_of_ar;
	nr_of_rt_mc = p_if->iod.nr_of_instances * p_if->iod.nr_of_ar;

#if ( PSI_CFG_RT_CLASS_3_SUPPORT_ON )
	nr_of_irt_uc = nr_of_rt_uc;
	nr_of_irt_mc = nr_of_rt_mc;
#else
	nr_of_irt_uc = 0;
	nr_of_irt_mc = 0;
#endif

	switch ( p_hd_in->edd_type )
	{
	case LSA_COMP_ID_EDDP:
	case LSA_COMP_ID_EDDI:
		{
			// Combined usage of RTC 1/2
			rt_detail_ptr->frame_id_base_class1_unicast     = 0xC000;
			rt_detail_ptr->nr_of_consumers_class1_unicast   = nr_of_rt_uc; 
			rt_detail_ptr->frame_id_base_class1_multicast   = 0; 
			rt_detail_ptr->nr_of_consumers_class1_multicast = 0;

			rt_detail_ptr->frame_id_base_class2_unicast     = 0x8000;
			rt_detail_ptr->nr_of_consumers_class2_unicast   = nr_of_rt_uc; 
			rt_detail_ptr->frame_id_base_class2_multicast   = 0; 
			rt_detail_ptr->nr_of_consumers_class2_multicast = 0;

			rt_detail_ptr->nr_of_providers_class1_2 = nr_of_rt_uc;
			rt_detail_ptr->nr_of_consumers_class3   = nr_of_irt_uc;
			rt_detail_ptr->nr_of_providers_class3   = nr_of_irt_mc;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = rt_detail_ptr->nr_of_providers_class1_2 + rt_detail_ptr->nr_of_providers_class3;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMSV;
		}
		break;
	case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			rt_detail_ptr->frame_id_base_class1_unicast     = 0xF800 - nr_of_rt_uc;
			rt_detail_ptr->nr_of_consumers_class1_unicast   = nr_of_rt_uc;
			rt_detail_ptr->frame_id_base_class1_multicast   = 0xF800;
			rt_detail_ptr->nr_of_consumers_class1_multicast = nr_of_rt_mc;

			rt_detail_ptr->frame_id_base_class2_unicast     = 0xBC00 - nr_of_rt_uc;
			rt_detail_ptr->nr_of_consumers_class2_unicast   = nr_of_rt_uc;
			rt_detail_ptr->frame_id_base_class2_multicast   = 0xBC00;
			rt_detail_ptr->nr_of_consumers_class2_multicast = nr_of_rt_mc;

			rt_detail_ptr->nr_of_providers_class1_2 = (nr_of_rt_uc + nr_of_rt_mc + nr_of_rt_uc + nr_of_rt_mc);
			rt_detail_ptr->nr_of_consumers_class3   = 0;
			rt_detail_ptr->nr_of_providers_class3   = 0;

			rt_detail_ptr->alarm_ref_base  = 0x1000;
			rt_detail_ptr->alarm_nr_of_ref = rt_detail_ptr->nr_of_providers_class1_2 + rt_detail_ptr->nr_of_providers_class3;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMSV;
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_set_details_acp_cmsv(): invalid EDDx type, if_nr(%u), edd_type(%u/%#x)",
				if_nr, 
				p_hd_in->edd_type,
				p_hd_in->edd_type );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmmc(
	LSA_UINT16              if_nr, 
	PSI_RT_DETAIL_PTR_TYPE  rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE   p_hd_in )
{
	PSI_HD_IF_INPUT_PTR_TYPE p_if;

	PSI_ASSERT( rt_detail_ptr != LSA_NULL );
	PSI_ASSERT( p_hd_in  != LSA_NULL );
	PSI_ASSERT( (if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&p_hd_in->pnio_if[if_nr-1];

	// Prepare the ACP settings based on used EDDx
	switch ( p_hd_in->edd_type )
	{
	case LSA_COMP_ID_EDDP:
	case LSA_COMP_ID_EDDI:
		{
			// Combined usage of RTC 1/2
			rt_detail_ptr->frame_id_base_class1_unicast     = 0;
			rt_detail_ptr->nr_of_consumers_class1_unicast   = 0; 
			rt_detail_ptr->frame_id_base_class1_multicast   = 0xC000;
			rt_detail_ptr->nr_of_consumers_class1_multicast = 0;

			rt_detail_ptr->frame_id_base_class2_unicast     = 0;
			rt_detail_ptr->nr_of_consumers_class2_unicast   = 0; 
			rt_detail_ptr->frame_id_base_class2_multicast   = 0x8000; 
			rt_detail_ptr->nr_of_consumers_class2_multicast = 0;

			rt_detail_ptr->nr_of_providers_class1_2 = p_if->ioc.nr_of_qv_provider;

			rt_detail_ptr->nr_of_consumers_class3 = p_if->ioc.nr_of_qv_consumer;
			rt_detail_ptr->nr_of_providers_class3 = p_if->ioc.nr_of_qv_provider;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = 0;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMMC;
		}
		break;
	case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			LSA_UINT16 rt_cons_iod = p_if->iod.nr_of_instances * p_if->iod.nr_of_ar;

			rt_detail_ptr->frame_id_base_class1_unicast     = 0;
			rt_detail_ptr->nr_of_consumers_class1_unicast   = 0;
			rt_detail_ptr->frame_id_base_class1_multicast   = 0xF800 + rt_cons_iod; // after IOD-MC
			rt_detail_ptr->nr_of_consumers_class1_multicast = p_if->ioc.nr_of_qv_consumer;

			// SRT: RTclass2
			rt_detail_ptr->frame_id_base_class2_unicast     = 0;
			rt_detail_ptr->nr_of_consumers_class2_unicast   = 0;
			rt_detail_ptr->frame_id_base_class2_multicast   = 0xBC00 + rt_cons_iod; // after IOD-MC
			rt_detail_ptr->nr_of_consumers_class2_multicast = p_if->ioc.nr_of_qv_consumer;

			rt_detail_ptr->nr_of_providers_class1_2 = p_if->ioc.nr_of_qv_provider + p_if->ioc.nr_of_qv_provider;

			rt_detail_ptr->nr_of_consumers_class3 = 0;
			rt_detail_ptr->nr_of_providers_class3 = 0;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = 0;
			rt_detail_ptr->group_id        = PSI_PROVIDER_GROUP_ID_CMMC;
		}
		break;
	default:
		{
			PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
				"psi_set_details_acp_cmsv(): invalid EDDx type, if_nr(%u), edd_type(%u/%#x)",
				if_nr, 
				p_hd_in->edd_type,
				p_hd_in->edd_type );

			PSI_FATAL( 0 );
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID  psi_set_details_pof_port(	PSI_PORT_DETAIL_PTR_TYPE  const  pof_detail_ptr )
{
	PSI_ASSERT(pof_detail_ptr != LSA_NULL);

    pof_detail_ptr->b_is_pof_port = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_init_if_details( PSI_DETAIL_STORE_PTR_TYPE detail_store_ptr )
{
	LSA_UINT16  port_idx;

	PSI_ASSERT(detail_store_ptr != LSA_NULL);

	detail_store_ptr->is_created  = LSA_FALSE;
	detail_store_ptr->hSysIF      = 0;
	detail_store_ptr->hPoolNRT    = 0;
	detail_store_ptr->pnio_if_nr  = 0;
	detail_store_ptr->edd_comp_id = 0;
	detail_store_ptr->trace_idx   = 0;
	detail_store_ptr->nr_of_ports = 0;

	psi_init_details_acp( &detail_store_ptr->acp.cm_cl );
	psi_init_details_acp( &detail_store_ptr->acp.cm_sv );
	psi_init_details_acp( &detail_store_ptr->acp.cm_mc );

	detail_store_ptr->cm.cl.device_count          = 0;
	detail_store_ptr->cm.cl.max_alarm_data_length = 0;
	detail_store_ptr->cm.cl.max_read_implicit     = 0;
	detail_store_ptr->cm.cl.max_record_length     = 0;

	detail_store_ptr->cm.sv.device_count          = 0;
	detail_store_ptr->cm.sv.max_alarm_data_length = 0;
	detail_store_ptr->cm.sv.max_ar                = 0;
	detail_store_ptr->cm.sv.max_read_implicit     = 0;
	detail_store_ptr->cm.sv.max_record_length     = 0;

	detail_store_ptr->oha.allow_non_pnio_mautypes  = LSA_FALSE;
	detail_store_ptr->oha.check_interval           = 0;
	detail_store_ptr->oha.check_ip_enabled         = LSA_FALSE;
	detail_store_ptr->oha.nos_allow_upper_cases    = LSA_FALSE;
	detail_store_ptr->oha.hello_resource_count_dcp = 0;
	detail_store_ptr->oha.recv_resource_count_dcp  = 0;

	for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
	{
		detail_store_ptr->pof.usr_port[port_idx].b_is_pof_port = LSA_FALSE;
	}

	detail_store_ptr->tcip.send_resources       = 0;
	detail_store_ptr->tcip.arp_rcv_resources    = 0;
	detail_store_ptr->tcip.icmp_rcv_resources   = 0;
	detail_store_ptr->tcip.tcp_rcv_resources    = 0;
	detail_store_ptr->tcip.udp_rcv_resources    = 0;
	detail_store_ptr->tcip.multicast_support_on = LSA_FALSE;
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_res_calc_init( LSA_VOID )
{
	LSA_UINT16 idx;

	/* Init the instance vars */
	PSI_ASSERT( !psi_pnio_store.init_done );

	/* Allocate storage for PNIO IF details store (global and IF specific) */
	for ( idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++ )
	{
		psi_pnio_store.entry_ptr[idx] = (PSI_PNIO_IF_ENTRY_PTR_TYPE) psi_alloc_local_mem( sizeof( PSI_PNIO_IF_ENTRY_TYPE ));
		PSI_ASSERT( psi_pnio_store.entry_ptr[idx] != LSA_NULL );

		psi_pnio_store.entry_ptr[idx]->is_used = LSA_FALSE;
		psi_pnio_store.entry_ptr[idx]->hd_nr   = 0;
		psi_pnio_store.entry_ptr[idx]->if_nr   = 0;

		psi_init_if_details(&psi_pnio_store.entry_ptr[idx]->details);
	}

	psi_pnio_store.init_done = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_undo_init( LSA_VOID )
{
	LSA_UINT16 idx;

	/* Undo INIT the instance vars */
	PSI_ASSERT( psi_pnio_store.init_done );

	/* Free storage for PNIO IF details store (global and IF specific) */
	for ( idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++ )
	{
		PSI_ASSERT( psi_pnio_store.entry_ptr[idx] != LSA_NULL );
		PSI_ASSERT( !psi_pnio_store.entry_ptr[idx]->is_used );

		psi_free_local_mem( psi_pnio_store.entry_ptr[idx] );
	}

	psi_pnio_store.init_done = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_delete( LSA_VOID )
{
  LSA_UINT16 idx;

  PSI_ASSERT( psi_pnio_store.init_done );
  
  for ( idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++ )
  {
    PSI_ASSERT( psi_pnio_store.entry_ptr[idx] != LSA_NULL );

    psi_pnio_store.entry_ptr[idx]->is_used = LSA_FALSE;
    psi_pnio_store.entry_ptr[idx]->hd_nr   = 0;
    psi_pnio_store.entry_ptr[idx]->if_nr   = 0;

    psi_init_if_details(&psi_pnio_store.entry_ptr[idx]->details);
  }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_set_global_details( PSI_SYS_HANDLE hSysIF )
{
	PSI_DETAIL_STORE_PTR_TYPE p_details;

	PSI_ASSERT( psi_pnio_store.entry_ptr[0] != LSA_NULL );

	p_details = &psi_pnio_store.entry_ptr[0]->details;

	// ------------ Global details ------------
	p_details->hSysIF      = hSysIF;
	p_details->pnio_if_nr  = (LSA_UINT8)PSI_GLOBAL_PNIO_IF_NR;
	p_details->nr_of_ports = 0;  // don't care
	p_details->hPoolNRT    = 0;  // don't care
	p_details->edd_comp_id = 0;  // don't care
	p_details->trace_idx   = 0;  // don't care
	p_details->is_created  = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  psi_res_calc_set_if_details(
	LSA_UINT16                    if_nr,
	PSI_SYS_HANDLE                hSysIF,
	LSA_INT                       nrt_pool_handle,
	PSI_HD_INPUT_PTR_TYPE  const  p_hd_in )
{
	PSI_DETAIL_STORE_PTR_TYPE p_details;
	PSI_HD_IF_INPUT_PTR_TYPE  p_if;
	LSA_UINT16                nr_of_user_ports;
	LSA_UINT16                port_idx;

	PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT((if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT));

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&p_hd_in->pnio_if[if_nr-1];
	PSI_ASSERT(p_if != LSA_NULL);

	p_details = psi_get_detail_store(p_hd_in->hd_id, if_nr);
	PSI_ASSERT(p_details != LSA_NULL);

	// Prepares the settings for details based on input values for this HD-IF
	// Note: the settings are used during open channels for GET_PATH_INFO_xx
	nr_of_user_ports = psi_get_nr_of_user_ports(if_nr, p_hd_in);
	PSI_ASSERT(nr_of_user_ports <= PSI_CFG_MAX_PORT_CNT);

	// ------------ Global details ------------
	p_details->hSysIF               = hSysIF;
	p_details->hPoolNRT             = nrt_pool_handle;
	p_details->edd_comp_id          = p_hd_in->edd_type;
	p_details->pnio_if_nr           = (LSA_UINT8)p_if->edd_if_id;
	p_details->trace_idx            = p_if->trace_idx;
	p_details->nr_of_ports          = nr_of_user_ports;
	p_details->runs_on_level_basic  = p_hd_in->runs_on_level_basic;

	// ------------ OHA details ------------
	p_details->oha.recv_resource_count_dcp = (DCP_NUM_SERVER_IDENTIFY_RESOURCES +  // thats the server MIN value
		DCP_NUM_SERVER_SET_RESOURCES + DCP_NUM_SERVER_GET_RESOURCES); 

    #if PSI_MAX_CL_OSU_DEVICES > 0
	p_details->oha.hello_resource_count_dcp = (PSI_DCP_HELLO_RESOURCES * 2); // twice for retries
    #else
	p_details->oha.hello_resource_count_dcp = 0;
    #endif

	p_details->oha.check_interval          = p_if->oha.check_interval;
	p_details->oha.check_ip_enabled        = p_if->oha.check_ip_enabled;
	p_details->oha.nos_allow_upper_cases   = p_if->oha.nos_allow_upper_cases;
	p_details->oha.allow_non_pnio_mautypes = p_hd_in->allow_none_pnio_mautypes;

	// ------------ POF details ------------
	// init the POF details (POF ports are set later)
	for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++ )
	{
		p_details->pof.usr_port[port_idx].b_is_pof_port = LSA_FALSE;
	}

	// ------------ CM-CL/CM-MC details ------------
	if ( p_if->ioc.use_setting ) // IOC/IOM configured?
	{
		p_details->cm.cl.device_count          = p_if->ioc.nr_of_rt_devices;
		p_details->cm.cl.max_alarm_data_length = p_if->ioc.max_alarm_data_length;
		p_details->cm.cl.max_read_implicit     = p_if->ioc.max_read_implicit;

		// max_record_length is a summary of configured max_record length(=data) + protocol header length
		// note: means length of PROFINETIOServiceReqPDU but limited to 64K (see IEC)
		if ( (CM_RECORD_OFFSET + p_if->ioc.max_record_length) > 0xFFFF )  // > 64K
		{
			p_details->cm.cl.max_record_length = 0xFFFF;
		}
		else
		{
			p_details->cm.cl.max_record_length = (CM_RECORD_OFFSET + p_if->ioc.max_record_length);
		}

		// ------------ ACP details for CM-CL/CM-MC ------------
		psi_set_details_acp_cmcl( if_nr, &p_details->acp.cm_cl, p_hd_in );
		psi_set_details_acp_cmmc( if_nr, &p_details->acp.cm_mc, p_hd_in );
	}
	else
	{
		// Set to unused
		p_details->cm.cl.device_count          = 0;
		p_details->cm.cl.max_alarm_data_length = 0;
		p_details->cm.cl.max_read_implicit     = 0;
		p_details->cm.cl.max_record_length     = 0;

		psi_init_details_acp( &p_details->acp.cm_cl );
		psi_init_details_acp( &p_details->acp.cm_mc );
	}

	// ------------ CM-SV details ------------
	if ( p_if->iod.use_setting ) // IOD configured?
	{
		p_details->cm.sv.device_count          = p_if->iod.nr_of_instances;
		p_details->cm.sv.max_alarm_data_length = p_if->iod.max_alarm_data_length;
		p_details->cm.sv.max_ar                = p_if->iod.nr_of_ar;
		p_details->cm.sv.max_read_implicit     = p_if->iod.nr_of_read_implicit;

		// max_record_length is a summary of configured max_record length(=data) + protocol header length
		// note: means length of PROFINETIOServiceReqPDU but limited to 64K (see IEC)
		if ( (CM_RECORD_OFFSET + p_if->iod.max_record_length) > 0xFFFF )  // > 64K
		{
			p_details->cm.sv.max_record_length = 0xFFFF;
		}
		else
		{
			p_details->cm.sv.max_record_length = (CM_RECORD_OFFSET + p_if->iod.max_record_length);
		}

		// ------------ ACP details for CM-SV ------------
		psi_set_details_acp_cmsv( if_nr, &p_details->acp.cm_sv, p_hd_in );
	}
	else
	{
		// Set to unused
		p_details->cm.sv.device_count          = 0;
		p_details->cm.sv.max_alarm_data_length = 0;
		p_details->cm.sv.max_ar                = 0;
		p_details->cm.sv.max_read_implicit     = 0;
		p_details->cm.sv.max_record_length     = 0;

		psi_init_details_acp( &p_details->acp.cm_sv );
	}

	// ------------ TCIP details ------------
	if ( p_if->ip.use_setting ) // IP for TCIP configured?
	{
		p_details->tcip.send_resources       = p_if->ip.nr_of_send;
		p_details->tcip.arp_rcv_resources    = p_if->ip.nr_of_arp;
		p_details->tcip.icmp_rcv_resources   = p_if->ip.nr_of_icmp;
		p_details->tcip.udp_rcv_resources    = p_if->ip.nr_of_udp;
		p_details->tcip.tcp_rcv_resources    = p_if->ip.nr_of_tcp;
		p_details->tcip.multicast_support_on = p_if->ip.multicast_support_on;
	}
	else
	{
		p_details->tcip.send_resources       = 0;
		p_details->tcip.arp_rcv_resources    = 0;
		p_details->tcip.icmp_rcv_resources   = 0;
		p_details->tcip.udp_rcv_resources    = 0;
		p_details->tcip.tcp_rcv_resources    = 0;
		p_details->tcip.multicast_support_on = LSA_FALSE;
	}

	p_details->is_created = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID  psi_res_calc_set_hw_port_outputs(
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out,
	PSI_HD_STORE_PTR_TYPE   const  p_hd_store )
{
	LSA_UINT32         if_idx;
	LSA_UINT32         port_idx;
    LSA_UINT32  const  nr_of_ports = p_hd_store->nr_of_ports;

 	for (if_idx = 0; if_idx < PSI_CFG_MAX_HD_IF_CNT; if_idx++)
	{
 		for (port_idx = 0; port_idx < nr_of_ports; port_idx++)
		{
	        p_hd_out->hw_port[port_idx].media_type = p_hd_store->hd_hw.port_media_type[if_idx][port_idx+1];
        }
 		for (; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++)
		{
		    p_hd_out->hw_port[port_idx].media_type = PSI_MEDIA_TYPE_UNKNOWN;
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID  psi_res_calc_update_hw_port_outputs(
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out,
	LSA_UINT32              const  port_idx,
	PSI_PORT_MEDIA_TYPE     const  new_port_media_type )
{
    p_hd_out->hw_port[port_idx].media_type = new_port_media_type;
}

/*---------------------------------------------------------------------------*/
LSA_VOID  psi_res_calc_set_if_outputs(
	LSA_UINT16              const  if_nr,
	PSI_HD_INPUT_PTR_TYPE   const  p_hd_in,
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out )
{
	PSI_HD_IF_INPUT_PTR_TYPE   p_if_inp;
	PSI_HD_IF_OUTPUT_PTR_TYPE  p_if_out;

	PSI_ASSERT( p_hd_in  != LSA_NULL );
	PSI_ASSERT( p_hd_out != LSA_NULL );
	PSI_ASSERT( (if_nr) && (if_nr <= PSI_CFG_MAX_HD_IF_CNT) );

	/* prepare the outputs for this IF */
	/* Note: used by an application for the CM Create services */
	p_if_inp = (PSI_HD_IF_INPUT_PTR_TYPE)&p_hd_in->pnio_if[if_nr-1];
	p_if_out = (PSI_HD_IF_OUTPUT_PTR_TYPE)&p_hd_out->pnio_if[if_nr-1];

	p_if_out->nr_of_usr_ports = psi_get_nr_of_user_ports( (LSA_UINT16)p_if_inp->if_id, p_hd_in );
	PSI_ASSERT( p_if_out->nr_of_usr_ports <= PSI_CFG_MAX_PORT_CNT );

	if ( p_if_inp->ioc.use_setting )  // IOC/IOM configured?
	{
		p_if_out->ioc_max_devices = p_if_inp->ioc.nr_of_rt_devices;
		p_if_out->iom_max_devices = p_if_inp->ioc.nr_of_qv_consumer + p_if_inp->ioc.nr_of_qv_provider;
	}
	else
	{
		p_if_out->ioc_max_devices = 0;
		p_if_out->iom_max_devices = 0;
	}

	if ( p_if_inp->iod.use_setting )  // IOD configured?
	{
		p_if_out->iod_max_devices  = p_if_inp->iod.nr_of_instances;
		p_if_out->iod_max_ar       = p_if_inp->iod.nr_of_ar;
		p_if_out->iod_max_ar_DAC   = p_if_inp->iod.nr_of_read_implicit;
		p_if_out->iod_max_ar_RT    = p_if_inp->iod.nr_of_ar;
		p_if_out->iod_max_ar_IOS   = 0;   /* disabled IO supervisor ARs (see AP01034486) */

        #if ( PSI_CFG_RT_CLASS_3_SUPPORT_ON )
		// Note: CM support only 1 RTC3-AR due to AP00774491
		p_if_out->iod_max_ar_IRT  = 1;
        #else
		p_if_out->iod_max_ar_IRT  = 0;
        #endif
	}
	else
	{
		p_if_out->iod_max_devices  = 0;
		p_if_out->iod_max_ar       = 0;
		p_if_out->iod_max_ar_DAC   = 0;
		p_if_out->iod_max_ar_RT    = 0;
		p_if_out->iod_max_ar_IOS   = 0;
	}
}

/*---------------------------------------------------------------------------*/
PSI_DETAIL_STORE_PTR_TYPE psi_get_detail_store( LSA_UINT16 hd_id, LSA_UINT16 if_id )
{
	LSA_UINT16                 idx;
	PSI_DETAIL_STORE_PTR_TYPE  ret_ptr = LSA_NULL;

	PSI_ASSERT(hd_id <= PSI_CFG_MAX_HD_CNT);
	PSI_ASSERT(if_id <= PSI_CFG_MAX_HD_IF_CNT);

	// lookup in PNIO storage for matching PNIO IF entry [1..16]
	for (idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++)
	{
		PSI_ASSERT(psi_pnio_store.entry_ptr[idx] != LSA_NULL);

		if (( psi_pnio_store.entry_ptr[idx]->is_used        ) &&  // used ?
			( psi_pnio_store.entry_ptr[idx]->hd_nr == hd_id ) &&  // Entry matches ?
			( psi_pnio_store.entry_ptr[idx]->if_nr == if_id ))
		{
			ret_ptr = &psi_pnio_store.entry_ptr[idx]->details;
			break;
		}
	}

	PSI_ASSERT(ret_ptr != 0);
	PSI_ASSERT(idx == ret_ptr->pnio_if_nr);

	return ret_ptr;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_add_detail_store( LSA_UINT16 pnio_if_id, LSA_UINT16 hd_id, LSA_UINT16 if_id )
{
	PSI_ASSERT( pnio_if_id <= PSI_MAX_PNIO_IF_NR );

	if ( psi_pnio_store.entry_ptr[pnio_if_id]->is_used ) // already in use ?
	{
		PSI_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL, 
			"psi_add_detail_store(): PNIO if (%u) already in use, check config, hd_id(%u) if_id(%u)",
			pnio_if_id,
			hd_id,
			if_id );

		PSI_FATAL( 0 );
	}

	// reserve entry with PNIO IF for HD, and HD IF 
	psi_pnio_store.entry_ptr[pnio_if_id]->is_used            = LSA_TRUE;
	psi_pnio_store.entry_ptr[pnio_if_id]->hd_nr              = hd_id;
	psi_pnio_store.entry_ptr[pnio_if_id]->if_nr              = if_id;
	psi_pnio_store.entry_ptr[pnio_if_id]->details.pnio_if_nr = (LSA_UINT8)pnio_if_id;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_remove_detail_store( LSA_UINT16 pnio_if_id )
{
	PSI_ASSERT( pnio_if_id <= PSI_MAX_PNIO_IF_NR );
	PSI_ASSERT( psi_pnio_store.entry_ptr[pnio_if_id]->is_used );

	// reserve entry with PNIO IF for HD, and HD IF 
	psi_pnio_store.entry_ptr[pnio_if_id]->is_used            = LSA_FALSE;
	psi_pnio_store.entry_ptr[pnio_if_id]->hd_nr              = 0;
	psi_pnio_store.entry_ptr[pnio_if_id]->if_nr              = 0;
	psi_pnio_store.entry_ptr[pnio_if_id]->details.pnio_if_nr = 0;
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_get_max_ar_count_all_ifs( LSA_VOID )
{
	LSA_UINT16 idx;
	LSA_UINT32 max_ar = 0;
	PSI_PNIO_IF_ENTRY_PTR_TYPE pnio_store_entry;

	// lookup in PNIO storage for matching PNIO IF entry [1..16]
	for ( idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++ )
	{
		pnio_store_entry = psi_pnio_store.entry_ptr[idx];

		if (( pnio_store_entry->is_used    ) &&  // used ?
			( pnio_store_entry->hd_nr != 0 ) &&  // HD Entry (not global pnio entry)
			( pnio_store_entry->if_nr != 0 ))
		{
			if( pnio_store_entry->details.cm.sv.max_ar > max_ar )
				max_ar = pnio_store_entry->details.cm.sv.max_ar;
		}
	}

	return max_ar;
}

/*---------------------------------------------------------------------------*/
LSA_UINT8 psi_get_pnio_if_nr( LSA_UINT16 hd_id, LSA_UINT16 if_id )
{
	LSA_UINT8 result;

	if ( ( hd_id == 0 ) && ( if_id == 0 )) // global SYS channel ?
	{
		result = PSI_GLOBAL_PNIO_IF_NR;
	}
	else
	{
		PSI_DETAIL_STORE_PTR_TYPE p_ret = psi_get_detail_store( hd_id, if_id );
		PSI_ASSERT( p_ret != LSA_NULL );
		PSI_ASSERT( p_ret->is_created );

		result = p_ret->pnio_if_nr;
	}

	return result;
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_get_trace_idx_for_sys_path( LSA_SYS_PATH_TYPE sys_path )
{
	LSA_UINT32 result;

	LSA_UINT16 hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 if_nr = PSI_SYSPATH_GET_IF( sys_path );

	if ( ( hd_nr == 0 ) && ( if_nr == 0 )) // global SYS channel ?
	{
		result = 0; /* Joker value 0 for global traces */
	}
	else
	{
		PSI_DETAIL_STORE_PTR_TYPE p_ret = psi_get_detail_store( hd_nr, if_nr );
		PSI_ASSERT( p_ret != LSA_NULL );
		PSI_ASSERT( p_ret->is_created );

		result = p_ret->trace_idx;
	}

	return result;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
