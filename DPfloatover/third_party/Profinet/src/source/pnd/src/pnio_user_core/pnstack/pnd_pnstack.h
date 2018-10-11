#ifndef PND_PNSTACK_H
#define PND_PNSTACK_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_pnstack.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Prototypes for all PN stack component adaption functions (ie PATH)       */
/*                                                                           */
/*****************************************************************************/

#define PND_INVALID_PNIO_USER_HANDLE ((LSA_HANDLE_TYPE)-1)  // Invalid PNIO User Handle

#define PND_IOC_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMCL   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP
#define PND_IOD_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMSV   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP
#define PND_IOM_GROUP_ID  PSI_PROVIDER_GROUP_ID_CMMC   // 0 <= group_id < EDD_CFG_CSRT_MAX_PROVIDER_GROUP

//---------------------------------------------------------------------------
typedef enum pnd_sync_state_type_enum // Possible SYNC state (LED states)
{
	PND_SYNC_STATE_NOT_CONFIGURED = 0,   // Not configured (posible a RT device without SYNC)
	PND_SYNC_STATE_NOT_SYNC       = 1,   // Configured but not in state SYNC
	PND_SYNC_STATE_SYNC           = 2,   // Configured and in state SYNC
	PND_SYNC_STATE_MAX                   // Enum max value
} PND_SYNC_STATE_TYPE;

/*----------------------------------------------------------------------------*/
typedef enum pnd_ioc_ar_state_enum { // AR states IOC/IOM
	PND_IOC_AR_STATE_UNKNOWN  = 0x00,  // Initial state (added only)
	PND_IOC_AR_STATE_OFFLINE  = 0x01,  // Offline state
	PND_IOC_AR_STATE_START    = 0x02,  // AR connecting till IN-DATA
	PND_IOC_AR_STATE_PREDATA  = 0x03,  // AR IN-DATA follows, Alarm.ind and Record-RW enabled
	PND_IOC_AR_STATE_INDATA   = 0x04,  // AR IN-DATA
	PND_IOC_AR_STATE_PASSIVE  = 0x05,  // AR passivated
	PND_IOC_AR_STATE_MAX
} PND_IOC_AR_STATE_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_ar_ds_state_tag {  // see also cm_event_apdu_status_enum in cm_usr.h
	LSA_BOOL is_run;          // state run(=LSA_TRUE) or Stop(=LSA_FALSE)
	LSA_BOOL is_station_ok;   // state station OK(=LSA_TRUE) or probelm(=LSA_FALSE)
	                          // following state only for SRD ARs
	LSA_BOOL is_primary;      // state primary(=LSA_TRUE) or backup(=LSA_FALSE)
	LSA_BOOL is_sysred_ok;    // state sysred OK(=LSA_TRUE) or problem(=LSA_FALSE)
} PND_AR_DS_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_pi_prop_tag {  // basic PI buffer properties (from IOCR config)
	LSA_BOOL    is_provider;           // provider buffer yes/no
	LSA_BOOL    is_dfp;                // DFP buffer yes/no
	LSA_BOOL    is_rtc3;               // RTC3 buffer yes/no
	LSA_UINT32  partial_length;        // size of PI data (not alligned) in bytes
	LSA_UINT32  apdu_status_length;    // size of APDU state in byte
	LSA_UINT16  group_id;              // ACP Group-ID (CL,MC,SV-Group)
	LSA_UINT8   prov_type;             // Provider Type see EDD_PROVADD_TYPE_xx
	LSA_UINT16  frame_id;              // image FrameID
	LSA_UINT32  subframe_offset;       // DFP subframe offset
	LSA_UINT32  subframe_data;         // DFP subframe data (contains position and subframe length)

} PND_PI_PROP_TYPE, *PND_PI_PROP_PTR_TYPE;

/*----------------------------------------------------------------------------*/
typedef struct pnd_pnuc_iocr_tag {  // holds IOCR config data for one input/output PI buffer
	LSA_UINT16         iocr_ref_nr;       // CR number
	LSA_VOID_PTR_TYPE  arcb_iocr_ptr;     // link to IOCR ptr in ARCB (for update after PI ALLOC)
	LSA_VOID_PTR_TYPE  iodu_handle;       // logical buffer handle from IODU
	PND_PI_PROP_TYPE   m_piProperties;    // PI properties (from IOCR configuration)

} PND_PNUC_IOCR_TYPE, *PND_PNUC_IOCR_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/* Adaption functions for PNIO base system (PNIO user to PNIO stack component)*/

LSA_VOID_PTR_TYPE pnd_cm_alloc_rqb( 
	LSA_OPCODE_TYPE     opcode, 
	LSA_HANDLE_TYPE     handle, 
	PND_HANDLE_PTR_TYPE handle_ptr, 
	LSA_UINT16          more_size );

LSA_VOID pnd_cm_free_rqb( 
	LSA_VOID_PTR_TYPE   rqb_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_cm_alloc_upper_mem( 
	LSA_UINT16          len, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID pnd_cm_free_upper_mem( 
	LSA_VOID_PTR_TYPE   mem_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_oha_alloc_rqb( 
	LSA_OPCODE_TYPE     opcode, 
	LSA_HANDLE_TYPE     handle, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID pnd_oha_free_rqb( 
	LSA_VOID_PTR_TYPE   rqb_ptr, 
	PND_HANDLE_PTR_TYPE handle_ptr );

LSA_VOID_PTR_TYPE pnd_oha_alloc_upper_mem( 
	LSA_UINT16          len, 
	PND_HANDLE_PTR_TYPE handle_ptr );
	
LSA_VOID pnd_oha_free_upper_mem( 
	LSA_VOID_PTR_TYPE   mem_ptr,
	PND_HANDLE_PTR_TYPE handle_ptr );

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PNSTACK_H */
