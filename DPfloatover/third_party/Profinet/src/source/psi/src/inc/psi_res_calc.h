#ifndef PSI_RES_CALC_H
#define PSI_RES_CALC_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: psi_res_calc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#include "psi_hd.h"

#include "psi_int.h"
#include "psi_trc.h"

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

typedef struct psi_rt_detail_tag { // Storage type for ACP specific RTC consumer/providers settings
	LSA_UINT16 frame_id_base_class1_unicast;       // RTC1 settings
	LSA_UINT16 nr_of_consumers_class1_unicast; 
	LSA_UINT16 frame_id_base_class1_multicast; 
	LSA_UINT16 nr_of_consumers_class1_multicast; 
	LSA_UINT16 frame_id_base_class2_unicast;       // RTC2 settings
	LSA_UINT16 nr_of_consumers_class2_unicast; 
	LSA_UINT16 frame_id_base_class2_multicast; 
	LSA_UINT16 nr_of_consumers_class2_multicast;
	LSA_UINT16 nr_of_providers_class1_2;
	LSA_UINT16 nr_of_consumers_class3;             // RTC3 settings
	LSA_UINT16 nr_of_providers_class3; 
	LSA_UINT16 alarm_ref_base;                     // Alarms
	LSA_UINT16 alarm_nr_of_ref;
	LSA_UINT16 group_id; 
} PSI_RT_DETAIL_TYPE, *PSI_RT_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct psi_detail_acp_tag { // Storage type for ACP detail settings
	PSI_RT_DETAIL_TYPE  cm_sv;  // settings CM SV (IOD)
	PSI_RT_DETAIL_TYPE  cm_cl;  // settings CM CL (IOC)
	PSI_RT_DETAIL_TYPE  cm_mc;  // settings CM_MC (IOM)
} PSI_ACP_DETAIL_TYPE, *PSI_ACP_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct psi_cm_detail_tag { // Storage type for CM detail settings
	struct psi_cm_cl_store_tag 
	{
		LSA_UINT16  device_count; 
		LSA_UINT16  max_alarm_data_length;
		LSA_UINT32  max_record_length;
		LSA_UINT16  max_read_implicit;
	} cl; 
	struct psi_cm_sv_store_tag 
	{
		LSA_UINT16  device_count;
		LSA_UINT16  max_ar;
		LSA_UINT16  max_alarm_data_length;
		LSA_UINT32  max_record_length;
		LSA_UINT16  max_read_implicit;
	} sv; 
} PSI_CM_DETAIL_TYPE, *PSI_CM_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct psi_oha_detail_tag { // Storage type for CM detail settings
	LSA_UINT16  hello_resource_count_dcp;
	LSA_UINT16  recv_resource_count_dcp;
	LSA_UINT16  check_interval;
	LSA_BOOL    check_ip_enabled;
	LSA_BOOL    allow_non_pnio_mautypes;
	LSA_BOOL    nos_allow_upper_cases;

} PSI_OHA_DETAIL_TYPE, *PSI_OHA_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct psi_port_detail_tag { // Storage for user port mapping [1..N]
	LSA_BOOL   b_is_pof_port;
} PSI_PORT_DETAIL_TYPE, *PSI_PORT_DETAIL_PTR_TYPE;

typedef struct psi_pof_detail_tag { // Storage type for POF detail settings
	PSI_PORT_DETAIL_TYPE usr_port[PSI_CFG_MAX_PORT_CNT+1]; //index = UsrPortID, 0 = reserved
} PSI_POF_DETAIL_TYPE, *PSI_POF_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/

typedef struct psi_tcip_detail_tag { // Storage type for TCIP detail settings
	LSA_UINT32 send_resources;
	LSA_UINT32 arp_rcv_resources;
	LSA_UINT32 icmp_rcv_resources;
	LSA_UINT32 udp_rcv_resources;
	LSA_UINT32 tcp_rcv_resources;
	LSA_BOOL   multicast_support_on;
} PSI_TCIP_DETAIL_TYPE, *PSI_TCIP_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
typedef struct psi_detail_store_tag { // Setup info for each PNIO IF used for detail ptr
	LSA_BOOL               is_created;        // Settings for PNIO-IF created yes/no
	PSI_SYS_HANDLE         hSysIF;            // hSys Handle for this HD IF
	LSA_INT                hPoolNRT;          // NRT pool handle
	LSA_UINT8              pnio_if_nr;        // PNIO IF nr [0..16]
	LSA_UINT32             trace_idx;         // Trace Index assigned on open device
	LSA_UINT16             edd_comp_id;       // LSA_COMP_ID of used EDDx
	LSA_UINT16             nr_of_ports;       // used ports for this IF
	LSA_BOOL               runs_on_level_basic;  // set if LD/HD run on different systems
	PSI_ACP_DETAIL_TYPE    acp;
	PSI_CM_DETAIL_TYPE     cm;
	PSI_OHA_DETAIL_TYPE    oha;
	PSI_POF_DETAIL_TYPE    pof;
	PSI_TCIP_DETAIL_TYPE   tcip;
} PSI_DETAIL_STORE_TYPE, *PSI_DETAIL_STORE_PTR_TYPE;

/*===========================================================================*/
/*                      prototypes                                           */
/*===========================================================================*/

LSA_VOID psi_set_details_pof_port( PSI_PORT_DETAIL_PTR_TYPE const pof_detail_ptr );

LSA_VOID psi_res_calc_init( LSA_VOID );

LSA_VOID psi_res_calc_undo_init( LSA_VOID );

LSA_VOID psi_res_calc_delete( LSA_VOID );

LSA_VOID psi_add_detail_store(
	LSA_UINT16 pnio_if_id,
	LSA_UINT16 hd_id, 
	LSA_UINT16 if_id );

LSA_VOID  psi_remove_detail_store( LSA_UINT16 pnio_if_id );

PSI_DETAIL_STORE_PTR_TYPE psi_get_detail_store(
	LSA_UINT16 hd_id,
	LSA_UINT16 if_id );

LSA_VOID  psi_res_calc_set_if_details(
	LSA_UINT16                    if_nr,
	PSI_SYS_HANDLE                hSysIF,
	LSA_INT                       nrt_pool_handle,
	PSI_HD_INPUT_PTR_TYPE  const  p_hd_in ); 

LSA_VOID  psi_res_calc_set_hw_port_outputs(
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out,
	PSI_HD_STORE_PTR_TYPE   const  p_hd_store );

LSA_VOID  psi_res_calc_update_hw_port_outputs(
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out,
	LSA_UINT32              const  port_idx,
	PSI_PORT_MEDIA_TYPE     const  new_port_media_type );

LSA_VOID  psi_res_calc_set_if_outputs(
	LSA_UINT16              const  if_nr,
	PSI_HD_INPUT_PTR_TYPE   const  p_hd_in,
	PSI_HD_OUTPUT_PTR_TYPE  const  p_hd_out );

LSA_UINT32  psi_get_max_ar_count_all_ifs( 
	LSA_VOID );

LSA_UINT8  psi_get_pnio_if_nr( 
	LSA_UINT16 hd_id, 
	LSA_UINT16 if_id );

LSA_UINT32 psi_get_trace_idx_for_sys_path( LSA_SYS_PATH_TYPE sys_path );

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_RES_CALC_H */
