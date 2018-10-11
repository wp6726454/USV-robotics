#ifndef CM_PRM_H
#define CM_PRM_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_prm.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Prm-Interface                                                            */
/*                                                                           */
/*****************************************************************************/


/*------------------------------------------------------------------------------
// EDD PRM-INTERFACE
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_prm_prepare(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_prm_write(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_BOOL				is_local,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_prm_end(
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  is_local
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_prm_commit(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_edd_prm_read(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_edd_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
);


/*------------------------------------------------------------------------------
// MRP PRM-INTERFACE
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_MRP

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_prepare(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_write(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_end(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_prm_commit(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mrp_prm_read(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_mrp_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
);

#endif

/*------------------------------------------------------------------------------
// GSY PRM-INTERFACE
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_prepare(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 sync_id
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_write(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT8				sync_id,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_end(
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  sync_id
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_prm_commit(
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  sync_id
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_gsy_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT8				sync_id,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_gsy_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
);

#endif

/*------------------------------------------------------------------------------
// POF PRM-INTERFACE
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_POF

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_prm_prepare(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_prm_write(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_prm_end(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_prm_commit(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pof_prm_read(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_pof_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
);

#endif


/*------------------------------------------------------------------------------
// OHA PRM-INTERFACE
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_prepare (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_BOOL				mk_rema,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_end (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_prm_commit (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_BOOL  is_local
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oha_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
);

CM_PORT_UINT  CM_LOCAL_FCT_ATTR
cm_oha_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
);


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_PRM_H */
