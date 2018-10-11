#ifndef HIF_INT_H
#define HIF_INT_H

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_int.h                                 :F&  */
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

#include "hif_inc.h"
#include "hif_trc.h"

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define HIF_IS_NULL(ptr)			(LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define HIF_IS_NOT_NULL(ptr)		(! LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define HIF_ARE_EQUAL(ptr1,ptr2)	(LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

/*===========================================================================*/
/*                                  defines                                  */
/*===========================================================================*/

/* Constants */
#define HIF_INTERNAL_REQ_RESERVED_CH_HANDLE 0
#define HIF_LD_TIMER_ID                     1
#define HIF_HD_TIMER_ID                     2

#define HIF_MAX_INSTANCES                   (HIF_CFG_MAX_LD_INSTANCES + HIF_CFG_MAX_HD_INSTANCES)
#define HIF_MAX_CH_HANDLES                  (1 + HIF_CFG_MAX_LD_CH_HANDLES + HIF_CFG_MAX_HD_CH_HANDLES)

/* Shared Memory Ring Buffer length */
#define HIF_SHM_RB_BUFFER_LENGTH            100

/* Shared Memory State */
#define HIF_SHM_STATE_DEVICE_NOT_READY      0x11111111
#define HIF_SHM_STATE_DEVICE_OPENED         0x33333333

/* SHM START / STOP sequences */
#define HIF_SHM_CHECK_OVERWRITE_START_SEQ   0x52415453  //STAR(T) in ASCII and little endian for readability (RATS)
#define HIF_SHM_CHECK_OVERWRITE_STOP_SEQ    0x504F5453  //STOP in ASCII and little endian for readability (POTS)

/*===========================================================================*/
/*                                   types                                   */
/*===========================================================================*/

/*------------------------------------------------------------------------------
// enums
//----------------------------------------------------------------------------*/

/* Instance States */
typedef enum
{
	HIF_INST_STATE_FREE    = 1,
	HIF_INST_STATE_OPENING = 2,
	HIF_INST_STATE_OPEN    = 3,
	HIF_INST_STATE_CLOSING = 4

} HIF_INST_STATE_TYPE;

/* Pipe States */
typedef enum
{
	HIF_PIPE_STATE_CLOSED        = 1,
	HIF_PIPE_STATE_OPENING_LOWER = 2,
	HIF_PIPE_STATE_OPENED_LOWER  = 3,
	HIF_PIPE_STATE_OPENING_UPPER = 4,
	HIF_PIPE_STATE_OPENED_UPPER  = 5

} HIF_PIPE_STATE_TYPE;

/* Device Types */
typedef enum
{
	HIF_DEV_TYPE_LD_UPPER = 1,
	HIF_DEV_TYPE_LD_LOWER = 2,
	HIF_DEV_TYPE_HD_UPPER = 3,
	HIF_DEV_TYPE_HD_LOWER = 4

} HIF_DEV_TYPE_TYPE;

/*------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------*/

typedef struct hif_shm_pipe_header_tag  *HIF_SHM_PIPE_HEADER_PTR_TYPE;
typedef struct hif_pipe_tag         *HIF_PIPE_PTR_TYPE;
typedef struct hif_instance_tag     *HIF_INST_PTR_TYPE;
typedef struct hif_ch_handle_tag    *HIF_CH_HANDLE_PTR_TYPE;
typedef struct hif_rb_tag_tag       *HIF_RB_TAG_PTR_TYPE;
typedef struct hif_sys_rb_tag_tag   *HIF_SYS_RB_TAG_PTR_TYPE;
typedef struct hif_poll_timer_tag   *HIF_POLL_TIMER_PTR_TYPE;
typedef struct hif_data_tag         *HIF_DATA_PTR_TYPE;

/*------------------------------------------------------------------------------
// shared mem header
//----------------------------------------------------------------------------*/

typedef struct hif_shm_header_tag
{
	struct
	{
		LSA_UINT32              State;                /* State of the instance (HIF_SHM_STATE_*) */
		LSA_UINT32              uHeaderLen;           /* Length of the header structure          */
		LSA_UINT32              IsUpper;              /* 1 = Upper instance, 0 = Lower instance; Needed for further interpretation */
		LSA_UINT32              uRingBufferLen;       /* Length of ring buffer block */
		LSA_UINT32              uRQBBufferLen;        /* Length of RQB Data buffer */
	} common;

	struct
	{
		LSA_UINT8               uRqbRQ;               /* Indicator for requests which are ready to receive in shm */
		LSA_UINT8               uRqbRQ_Ack;           /* Acknowledge for received RQBs                            */
	} tm_control;

	union
	{
		struct
		{
			LSA_UINT32          uIfIdent;
			LSA_UINT32          uPipeHeaderLen;
			LSA_UINT32          uPnioVersionLen;
			LSA_UINT8           uaPnioVersion[26];
			LSA_UINT8           reserved1;
			LSA_UINT8           reserved2;
		} lower;
	} dev_specific;

} HIF_SHM_HEADER_TAG;


/*------------------------------------------------------------------------------
// ring_buffers for pipes
//----------------------------------------------------------------------------*/

typedef struct hif_ring_buffer_tag
{
	LSA_UINT32    ri; // READ_INDEX - last element in the ring_buffer
	LSA_UINT32    wi; // WRITE_INDEX - first element in the ring buffer
	LSA_UINT32    rqb[HIF_SHM_RB_BUFFER_LENGTH]; 
    LSA_UINT32    PipeIndex[HIF_SHM_RB_BUFFER_LENGTH]; /* PipeIndex of Channel in RQB, need to assign RGB to Pipe*/

} HIF_RING_BUFFER_TYPE, *HIF_RING_BUFFER_PTR_TYPE;

/*------------------------------------------------------------------------------
// for each queue (double linked list) of opened and closed pipes
//----------------------------------------------------------------------------*/

typedef struct hif_pipe_header_tag {
		struct hif_pipe_header_tag  *pNext;
		struct hif_pipe_header_tag  *pPrev;

} HIF_PIPE_HEADER_TYPE, *HIF_PIPE_HEADER_PTR_TYPE;
	
typedef struct hif_queuing_tag
{
    struct hif_queuing_tag   *pNext;		    /* forward link */
    struct hif_queuing_tag   *pPrev;		    /* backward link */

} HIF_QUEUING_TYPE;

typedef struct hif_pipe_shm_send_ring_tag
{
	LSA_UINT32 wi;                                         /* Local copy of write index, needed for lookup because pWiShmWriteOnly might not be located on this processor */
	LSA_UINT32 Ring[HIF_SHM_RB_BUFFER_LENGTH];             /* Local copy of RQB Ring, used for cleaning */
	LSA_BOOL   bRingElementUsed[HIF_SHM_RB_BUFFER_LENGTH]; /* Indicates if ring element is used */
	LSA_UINT32 *pRiShmReadOnly;                            /* Read Index located on this processor (Shared Memory Region), written by the other processor */
	LSA_UINT32 *pPipeIndexWriteOnly;                          /* Write PipeIndex located on this processor (Shared Memory Region), written by the other processor */
	LSA_UINT32 *pWiShmWriteOnly;                           /* Write index, probably located on the other processor, write only */
	LSA_UINT32 *pRingWriteOnly;                            /* RQB Ring, probably located on the other processor, write only */
	HIF_RING_BUFFER_PTR_TYPE pWriteOnlyRing;               /* Only for debug purposes */

} HIF_PIPE_SHM_SEND_RING_TYPE, *HIF_PIPE_SHM_SEND_RING_PTR_TYPE;

typedef struct hif_pipe_shm_receive_ring_tag
{
	LSA_UINT32 ri;                              /* Local copy of read index, needed for lookup because pRiShmReadOnly might not be located on this processor */
	LSA_UINT32 *pRiShmWriteOnly;                /* Read index, probably located on the other processor, write only */
	LSA_UINT32 *pPipeIndexReadOnly;                /* Read Pipeindex, probably located on the other processor, write only */
	LSA_UINT32 *pWiShmReadOnly;                 /* Write index, located on this processor */
	LSA_UINT32 *pRingReadOnly;                  /* RQB Ring, located on this processor */
	HIF_RING_BUFFER_PTR_TYPE pReadOnlyRing;     /* Only for debug purposes */

} HIF_PIPE_SHM_RECEIVE_RING_TYPE, *HIF_PIPE_SHM_RECEIVE_RING_PTR_TYPE;

/*------------------------------------------------------------------------------
// hif_component_support_store_tag
//----------------------------------------------------------------------------*/

typedef struct hif_component_support_store_tag
{
	struct
	{
		LSA_BOOL bUsed;
		HIF_COMPONENT_SUPPORT_STD_TYPE Comp;
	} CompStd[HIF_MAX_COMP_ID];

	struct
	{
		LSA_BOOL bUsed;
		HIF_COMPONENT_SUPPORT_SYS_TYPE Comp;
	} CompSys[HIF_MAX_COMP_ID];

} HIF_COMPONENT_SUPPORT_STORE_TYPE, *HIF_COMPONENT_SUPPORT_SOTRE_PTR_TYPE;


/*------------------------------------------------------------------------------
// hif_shm_rqb_ringlist_tag
//----------------------------------------------------------------------------*/
typedef struct hif_shm_rqb_ringlist_tag
{
	HIF_PIPE_SHM_SEND_RING_TYPE    SendRing;            
	HIF_PIPE_SHM_RECEIVE_RING_TYPE ReceiveRing;
	HIF_RQB_PTR_TYPE               pSndQueue;           //queued rqbs which will be send as soon as there is space in the send ring
	HIF_RQB_PTR_TYPE               pRcvList;            //received RQBs

} HIF_SHM_RQB_RINGLIST_TAG_TYPE, * HIF_SHM_RQB_RINGLIST_TAG_PTR_TYPE;



/*------------------------------------------------------------------------------
// hif_pipe_tag
//----------------------------------------------------------------------------*/

typedef struct hif_pipe_tag
{
	HIF_PIPE_STATE_TYPE                State;                        //closed/opening lower/opening upper/opened lower/opened upper/closing lower/closing upper
	LSA_UINT16                         PipeIndex;                    //index of this element in hif_inst array
	HIF_INST_PTR_TYPE                  pInstance;                    //quick link to a specific hif instance
	HIF_HD_ID_TYPE                     hd_id;
	HIF_IF_ID_TYPE                     if_id;
	HIF_PIPE_ID_TYPE                   pipe_id;
	LSA_COMP_ID_TYPE                   comp_id;                      //component id
	HIF_SHM_RQB_RINGLIST_TAG_PTR_TYPE  pRQBRingList;                 //quick link to RQBRingList

	union 
    {
		struct 
        {
			HIF_CH_HANDLE_PTR_TYPE  ch_handle;                      //quick link to assigned channel handle
			LSA_HANDLE_TYPE         hUpper;                         //upper handle which is used for the cnf
		} upper;

		struct 
        {
			LSA_HANDLE_TYPE         lower_orig_comp_ch_handle;      //original handle of the target component - only set on lower side

		} lower;
	} d;

	struct 
    {
		LSA_HANDLE_TYPE hLower;
	} short_circuit_data_backup;

	struct 
    {
		LSA_VOID (*RELEASE_PATH_INFO)(LSA_UINT16 *ptr_dst, LSA_SYS_PTR_TYPE sys_ptr, HIF_DETAIL_PTR_TYPE detail_ptr);
        #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
		HIF_SERIALIZE_CBF_TYPE              Serialize;
		HIF_DESERIALIZE_CBF_TYPE            Deserialize;
        #endif
		HIF_COMPONENT_SUPPORT_STD_PTR_TYPE  pCompFuncs;
	} func;

} HIF_PIPE_TYPE;

/*------------------------------------------------------------------------------
// hif_instance_tag
//----------------------------------------------------------------------------*/

typedef struct hif_instance_tag
{
	HIF_INST_STATE_TYPE            State;                         //free, opening, open, closing
	HIF_DEV_TYPE_TYPE              Type;                          //LD upper / LD lower / HD upper / HD lower
	LSA_UINT16                     Mode;                          //Shared Mem Transfer / Pass Through / Mixed
	HIF_HANDLE                     hH;                            //handle HIF - returned by device open needed again in detail_ptr (will be index of devices struct)
	LSA_BOOL                       bRcvEnabled;                   //indicator if instance is ready for receiving requests
	LSA_BOOL                       bLDDevice;                     //indicator if instance is LD type
	LSA_BOOL                       bHDDevice;                     //indicator if instance is HD type
	LSA_BOOL                       bUpperDevice;                  //indicator if instance is Upper Device
	LSA_BOOL                       bLowerDevice;                  //indicator if instance is Lower Device
	HIF_SYS_HANDLE                 hSysDev;                       //upper device handle
	HIF_RQB_PTR_TYPE               pRQBIsr;                       //RQB which is send to HIF Thread in case an IRQ arrived on target device
	HIF_RQB_PTR_TYPE               pRQBShort;                     //RQB which is send to HIF Thread in short mode
	HIF_RQB_PTR_TYPE               pRQBUpperDeviceCloseFinishRsp; //RQB 
	HIF_PIPE_TYPE                  Pipes[HIF_MAX_PIPES_INST];     //Pipe specific data

	struct
	{
		HIF_UPPER_CALLBACK_FCT_PTR_TYPE SystemDoneCbf;
		HIF_HANDLE hH;
	} backup;

	union
	{
		struct
		{
			HIF_HD_ID_TYPE hd_id;
		} hd;
	} DevSpecific;

	struct
	{
		struct
		{
			LSA_BOOL bTimerPoll;
			LSA_BOOL bShmHandling;
			LSA_BOOL bSerialization;
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			LSA_BOOL bShmSndWaitAck;
            #endif
            HIF_SEND_IRQ_MODE_TYPE SendIrqMode;  // mode for sending irq's (no_wait/read_back/timeout before send)
            LSA_UINT64 uNsWaitBeforeSendIrq;     // ns timeout before sending irq (only used if send_irq_mode is set to timeout mode)              

		} TmOptions;

		struct
		{
			volatile LSA_UINT8*            pRcvRQ;  // trigger from remote instance
			volatile LSA_UINT8*            pSndRQ;  // trigger to remote instance
            #if defined (HIF_CFG_USE_SND_WAIT_ACK)
			LSA_UINT8*                     pRcvRQAck;  // optional Ack
			LSA_UINT8*                     pSndRQAck;  
            #endif
            LSA_UINT32                     uActOpenRQBs;   //count of RQBs which haven't returned so far
            HIF_SHM_RQB_RINGLIST_TAG_TYPE  RQBRingList;

            struct
			{
				LSA_UINT8* pBase;                    /* base ptr             */
				LSA_UINT32 uSize;                    /* size of shm          */
				HIF_SHM_HEADER_TAG* pShmHeader;      /* shm header structure */
				LSA_UINT8  *pRqbBufferBase;          /* RQB Buffer Base Ptr  */
			} ShmLocal; /* Shared mem on the local processor which should only be read (because of bad pci read performance) */

			struct
			{
				LSA_INT    hPool;                    /* pool handle          */
				LSA_UINT8* pBase;                    /* base ptr             */
				LSA_UINT32 uSize;                    /* size of shm          */
				HIF_SHM_HEADER_TAG* pShmHeader;      /* shm header structure */
				LSA_UINT8  *pRqbBufferBase;          /* RQB Buffer Base Ptr  */
			} ShmRemote; /* Shared mem on the remote processor which should be written (because of bad pci read performance) */
		} SharedMem;                                  //Shared Mem Transfer Mode / Mixed Mode
		
		HIF_INST_PTR_TYPE pHifPartner;                //Pass Through Mode / Mixed Mode
	} if_pars;

	struct
	{
		LSA_VOID (*DO_REQUEST_LOCAL)(HIF_RQB_PTR_TYPE pRQB);
		LSA_VOID (*DO_INTERRUPT_REQUEST)(HIF_RQB_PTR_TYPE pRQB);
		LSA_VOID (*SEND_IRQ)(HIF_SYS_HANDLE hSysDev, HIF_HANDLE hH);
		LSA_VOID (*ACK_IRQ)(HIF_SYS_HANDLE hSysDev, HIF_HANDLE hH);
		LSA_VOID (*REQUEST_DONE)(HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,HIF_RQB_PTR_TYPE ptr_upper_rqb, LSA_SYS_PTR_TYPE sys_ptr);
		LSA_VOID (*REQUEST_LOWER)(HIF_RQB_PTR_TYPE pRQB, HIF_SYS_HANDLE hSysDev);
		LSA_VOID (*OPEN_CHANNEL_LOWER)(HIF_RQB_PTR_TYPE pRQB, HIF_SYS_HANDLE hSysDev);
		LSA_VOID (*CLOSE_CHANNEL_LOWER)(HIF_RQB_PTR_TYPE pRQB, HIF_SYS_HANDLE hSysDev);
		LSA_VOID (*CLOSE_DEVICE_LOWER)(HIF_RQB_PTR_TYPE pRQB);
	} func;
} HIF_INST_TYPE;

/*------------------------------------------------------------------------------
// hif_ch_handle_tag
//----------------------------------------------------------------------------*/

typedef struct hif_ch_handle_tag
{
	LSA_BOOL               bUsed;
	LSA_BOOL               bValid;
	LSA_HANDLE_TYPE        hHifCompCh;                 //new component handle send to the user
	LSA_SYS_PTR_TYPE       sys_ptr;                    //sys ptr of target component
	HIF_DETAIL_PTR_TYPE    pDetails;                   //quick link to the target detail type
	HIF_PIPE_PTR_TYPE      pPipe;                      //quick link to target pipe (calculated from hd/if/pipe_id mapping)
	LSA_VOID LSA_FCT_PTR( , pCbf)(HIF_RQB_PTR_TYPE  upper_rqb_ptr); //stores the original cbf from open_channel

} HIF_CH_HANDLE_TYPE;

/*------------------------------------------------------------------------------
// hif_glob_tag
//----------------------------------------------------------------------------*/

typedef struct hif_poll_timer_tag
{
	LSA_TIMER_ID_TYPE         hPollTimer;                  //Timer for polling hif shared mem buffers
	LSA_UINT16                PollUserCnt;                 //Count of hif devices using the timer
	HIF_RQB_PTR_TYPE          pRQBPoll;                    //HIF RQB used for isr polling

} HIF_POLL_TIMER_TYPE;

/*------------------------------------------------------------------------------
// hif_rb_tag
//----------------------------------------------------------------------------*/

typedef struct hif_rb_tag_tag
{
	HIF_PIPE_PTR_TYPE                pPipe;                //quick link to pipe (only planned for debug purposes)
	LSA_USER_ID_TYPE                 UserId;               //UserId backed up for RQB

} HIF_RB_TAG_TYPE;

/*------------------------------------------------------------------------------
// hif_sys_rb_tag
//----------------------------------------------------------------------------*/

typedef struct hif_sys_rb_tag_tag
{
	HIF_PIPE_PTR_TYPE                pPipe;                //quick link to pipe (only planned for debug purposes)
	LSA_USER_ID_TYPE                 UserId;               //UserId backed up for RQB
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf;                 //Callback function (only for sys requests)

} HIF_SYS_RB_TAG_TYPE;

/*------------------------------------------------------------------------------
// hif_data_tag
//----------------------------------------------------------------------------*/

typedef struct hif_data_tag
{
	HIF_INST_TYPE             Instances[HIF_MAX_INSTANCES];   //Each hif_system(OPC_OPEN) allocates one
	HIF_CH_HANDLE_TYPE        ChHandles[HIF_MAX_CH_HANDLES];  //Max 256 because LSA_HANDLE_TYPE is used as index

	HIF_COMPONENT_SUPPORT_STORE_TYPE  CompStore;

	struct
	{
		HIF_POLL_TIMER_TYPE   PollTimer;
		HIF_RQB_PTR_TYPE      pRQBPoll;
	} HD;

	struct
	{
		HIF_POLL_TIMER_TYPE   PollTimer;
		HIF_RQB_PTR_TYPE      pRQBPoll;
	} LD;

} HIF_DATA_TYPE;

/*------------------------------------------------------------------------------
// hif_usr_pipe_definition_tag
//----------------------------------------------------------------------------*/

typedef struct hif_usr_pipe_definition_tag
{
	HIF_PIPE_ID_TYPE pipe_id;
	LSA_COMP_ID_TYPE comp_id;

} HIF_USR_PIPE_DEFINITION_TYPE;

/*===========================================================================*/
/*                                data                                       */
/*===========================================================================*/

#if (HIF_MODULE_ID == 1)
	HIF_DATA_PTR_TYPE g_pHifData;
#else
	LSA_EXTERN HIF_DATA_PTR_TYPE g_pHifData;
#endif

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*-hif_sys.c------------------------------------------------------------------*/
LSA_VOID   hif_system_callback(HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response);
LSA_BOOL   hif_poll_instance(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_lower_sys_request_done(LSA_VOID* rb);
LSA_VOID   hif_lower_sys_request_done2(LSA_VOID* rb);

/*-hif_usr.c------------------------------------------------------------------*/
LSA_VOID   hif_callback(HIF_CH_HANDLE_PTR_TYPE pChHandle, HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response);
LSA_VOID   hif_lower_request_done(LSA_VOID* rb);
LSA_VOID   hif_lower_request_done2(LSA_VOID* rb);
LSA_VOID   hif_open_channel(HIF_RQB_PTR_TYPE rb, LSA_BOOL bLDRequest, LSA_BOOL bURequest);
LSA_VOID   hif_close_channel(HIF_RQB_PTR_TYPE rb, LSA_BOOL bLDRequest);
LSA_VOID   hif_request(HIF_RQB_PTR_TYPE rb, HIF_POLL_TIMER_TYPE* pPollTimer, LSA_BOOL bLDRequest);

/*-hif_comp.c-----------------------------------------------------------------*/
LSA_VOID   hif_install_integrated_comp_access_funcs(LSA_VOID);
LSA_VOID   hif_update_std_comp_access_funcs(HIF_PIPE_PTR_TYPE pPipe);

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
LSA_VOID   hif_update_serialization_funcs(HIF_PIPE_PTR_TYPE pPipe);
#endif

/*-hif_dev.c------------------------------------------------------------------*/
LSA_UINT16 hif_upper_device_close(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_device_close_proceed(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_device_close_finish_1 (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_device_close_finish_2 (HIF_INST_PTR_TYPE pHifInst);
LSA_UINT16 hif_lower_device_close(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_pipe_request(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_pipe_request_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);

LSA_VOID   hif_lower_device_close_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_send_admin_request(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_receive_admin_request(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_send_admin_request_done(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);

LSA_VOID   hif_lower_sys_request(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_sys_request_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);

LSA_UINT16 hif_upper_hd_device_open(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_hd_device_open_finish (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_UINT16 hif_lower_hd_device_open (HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_hd_device_open_proceed (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_hd_device_open_finish (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);

LSA_UINT16 hif_upper_ld_device_open(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_ld_device_open_finish (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_UINT16 hif_lower_ld_device_open (HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_ld_device_open_proceed (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_ld_device_open_finish (HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);

LSA_VOID   hif_lower_save_return_path_to_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_get_return_path_of_rqb(HIF_PIPE_PTR_TYPE* ppPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_restore_rqb_user_fields_and_free_return_path(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_save_return_path_and_cbf_to_sys_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_get_return_path_of_sys_rqb(HIF_PIPE_PTR_TYPE* ppPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_restore_user_fields_of_sys_rqb_and_free_return_path(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);

/*-hif_int.c------------------------------------------------------------------*/
LSA_UINT32 hif_get_if_ident(HIF_INST_PTR_TYPE pHifInst);
LSA_UINT16 hif_map_lsa_to_rqb_response(LSA_UINT16 lsaResponseVal);
LSA_RESULT hif_alloc_local_internal_rqb(HIF_RQB_PTR_TYPE* ppRQB, LSA_OPCODE_TYPE Opcode);
LSA_VOID   hif_free_local_internal_rqb(HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_alloc_ch_handle(HIF_PIPE_PTR_TYPE pPipe, HIF_CH_HANDLE_PTR_TYPE* ppHifChHandle);
LSA_VOID   hif_free_ch_handle(HIF_CH_HANDLE_PTR_TYPE pHifChHandle);
LSA_VOID   hif_alloc_rb_tag(HIF_RB_TAG_PTR_TYPE* ppHifRbTag, HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_free_rb_tag(HIF_RB_TAG_PTR_TYPE pHifRbTag);
LSA_VOID   hif_free_sys_rb_tag(HIF_SYS_RB_TAG_PTR_TYPE pHifSysRbTag);
LSA_VOID   hif_alloc_sys_rb_tag(HIF_SYS_RB_TAG_PTR_TYPE* ppHifSysRbTag, HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_alloc_instance(HIF_INST_PTR_TYPE* ppHifInst, LSA_OPCODE_TYPE opcode);
LSA_VOID   hif_free_instance(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_get_instance(HIF_INST_PTR_TYPE* ppHifInst, HIF_HANDLE hH);
LSA_BOOL   hif_check_for_open_instances(LSA_VOID);
LSA_BOOL   hif_is_upper_device(HIF_INST_PTR_TYPE pHifInst);
LSA_BOOL   hif_is_ld_device(HIF_INST_PTR_TYPE pHifInst);
LSA_UINT32 hif_test_rqb_queue(HIF_RQB_PTR_TYPE* rqb_queue, HIF_RQB_PTR_TYPE rqb);
LSA_UINT16 hif_add_to_rqb_queue_as_first(HIF_RQB_PTR_TYPE* rqb_queue, HIF_RQB_PTR_TYPE rqb, LSA_UINT16 PipeIndex);
LSA_UINT16 hif_add_to_rqb_queue(HIF_RQB_PTR_TYPE* rqb_queue, HIF_RQB_PTR_TYPE rqb, LSA_UINT16 PipeIndex);
LSA_UINT16 hif_get_from_rqb_queue(HIF_RQB_PTR_TYPE* rqb_queue, HIF_RQB_PTR_TYPE* rqb, LSA_UINT16 *pPipeIndex);
LSA_UINT16 hif_unlink_from_rqb_queue(HIF_RQB_PTR_TYPE* rqb_queue, HIF_RQB_PTR_TYPE rqb);
LSA_UINT32 hif_get_rqb_id(LSA_VOID* rqbShm);

/*-hif_pipe.c-----------------------------------------------------------------*/
LSA_VOID   hif_upper_open_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_upper_close_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_open_channel(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_close_channel(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_close_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_lower_open_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_UINT16 hif_verify_usr_pipe_definition(HIF_USR_PIPE_DEFINITION_TYPE* pHifUsrPipeDef);
LSA_UINT16 hif_setup_pipe_mapping(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_setup_single_pipe_params_by_index(HIF_INST_PTR_TYPE pHifInst, LSA_UINT16 index, LSA_COMP_ID_TYPE comp_id, HIF_HD_ID_TYPE hd_id, HIF_IF_ID_TYPE if_id, HIF_PIPE_ID_TYPE pipe_id);
LSA_VOID   hif_get_pipe(HIF_PIPE_PTR_TYPE* ppPipe, HIF_INST_PTR_TYPE pHifInst, HIF_HD_ID_TYPE hd_id, HIF_IF_ID_TYPE if_id, HIF_PIPE_ID_TYPE pipe_id);
LSA_VOID   hif_set_pipe_state(HIF_PIPE_PTR_TYPE pPipe, HIF_PIPE_STATE_TYPE eState);
LSA_BOOL   hif_is_upper_pipe(HIF_PIPE_PTR_TYPE pPipe);

/*-hif_sr.c-------------------------------------------------------------------*/
LSA_UINT16 hif_enable_receive(HIF_INST_PTR_TYPE pHifInst);
LSA_UINT16 hif_disable_receive(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_send(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_process_rcv_list(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_process_rcv_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_UINT16 hif_setup_transfer_mode(HIF_INST_PTR_TYPE pHifInst, HIF_GET_PAR_PTR_TYPE pPars);
LSA_UINT16 hif_finish_transfer_mode(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_short_receive_queue(HIF_PIPE_HEADER_PTR_TYPE pQueue);


LSA_UINT16 hif_shm_send_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB);
LSA_VOID   hif_shm_receive(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_shm_send(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_shm_clean_rqbs_from_instance_clean_queue(HIF_INST_PTR_TYPE pHifInst);
LSA_BOOL   hif_shm_send_rqbs_from_send_queue(HIF_PIPE_PTR_TYPE pPipe, LSA_BOOL* pbNewRqbsSent);
LSA_VOID   hif_shm_ring_buffer_send_clean_all_elements(HIF_INST_PTR_TYPE pHifInst);
LSA_VOID   hif_shm_ring_buffer_send_clean_element(HIF_PIPE_PTR_TYPE pPipe);
LSA_BOOL   hif_shm_ring_buffer_send_is_full(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing);
LSA_BOOL   hif_shm_ring_buffer_send_enque(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing, LSA_UINT32 uRqbOffset, LSA_UINT32 uRqb, LSA_UINT16 pipe_id);
LSA_BOOL   hif_shm_ring_buffer_receive_deque_start(HIF_PIPE_SHM_RECEIVE_RING_PTR_TYPE pRcvRing, LSA_UINT32 *puRqb, LSA_UINT16 *pPipeIndex);

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_CLRPC == 1) )
/*-hif_ser_clrpc.c------------------------------------------------------------*/
LSA_UINT16  hif_clrpc_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_clrpc_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_clrpc_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_clrpc_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_CM == 1) )
/*-hif_ser_cm.c---------------------------------------------------------------*/
LSA_UINT16  hif_cm_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_cm_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_cm_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_cm_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_DCP == 1))
/*-hif_ser_dcp.c--------------------------------------------------------------*/
LSA_UINT16  hif_dcp_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_dcp_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_dcp_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_dcp_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_EDD == 1) )
/*-hif_ser_edd.c--------------------------------------------------------------*/
LSA_UINT16  hif_edd_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_edd_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_edd_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_edd_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_IOH == 1) )
/*-hif_ser_ioh.c--------------------------------------------------------------*/
LSA_UINT16  hif_ioh_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_ioh_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_ioh_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_ioh_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_LLDP == 1) )
/*-hif_ser_lldp.c-------------------------------------------------------------*/
LSA_UINT16  hif_lldp_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_lldp_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_lldp_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_lldp_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_MRP == 1) )
/*-hif_ser_mrp.c--------------------------------------------------------------*/
LSA_UINT16  hif_mrp_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_mrp_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_mrp_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_mrp_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_NARE == 1) )
/*-hif_ser_nare.c-------------------------------------------------------------*/
LSA_UINT16  hif_nare_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_nare_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_nare_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_nare_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_OHA == 1) )
/*-hif_ser_oha.c--------------------------------------------------------------*/
LSA_UINT16  hif_oha_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_oha_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_oha_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_oha_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_SNMPX == 1) )
/*-hif_ser_snmpx.c------------------------------------------------------------*/
LSA_UINT16  hif_snmpx_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_snmpx_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_snmpx_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_snmpx_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_SOCK == 1) )
/*-hif_ser_sock.c-------------------------------------------------------------*/
LSA_UINT16  hif_sock_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_sock_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_sock_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_sock_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_GSY == 1) )
/*-hif_ser_sock.c-------------------------------------------------------------*/
LSA_UINT16  hif_gsy_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_gsy_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_gsy_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_gsy_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_ACP == 1) )
/*-hif_ser_pntrc.c-------------------------------------------------------------*/
LSA_UINT16  hif_acp_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_acp_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_acp_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_acp_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/*-hif_ser_hif.c-------------------------------------------------------------*/
LSA_UINT16  hif_hif_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_hif_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_hif_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_hif_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_PSI == 1) )
/*-hif_ser_cm.c---------------------------------------------------------------*/
LSA_UINT16  hif_psi_serialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_psi_serialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
LSA_UINT16  hif_psi_deserialize_lower (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
LSA_UINT16  hif_psi_deserialize_upper (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
//	HIF DE-/SERIALIZATION FUNCTIONS
//----------------------------------------------------------------------------*/

/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_8()
*  Purpose:     - copies uint8 from host to shared-mem
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#define HIF_SER_COPY_HTSHM_8(dest, src)\
{\
    **(dest) = (LSA_UINT8)(src);\
	*(dest) = *(dest)+1;\
}

/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_16()
*  Purpose:     - copies uint16 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_HTSHM_16(dest, src)\
{\
	LSA_UINT16 temp = 0;\
	temp = HIF_LTOSHM_S(src);\
	HIF_MEMCPY((LSA_UINT16*)*(dest),&temp,2);\
	*(dest) = *(dest)+1;\
}
#else
#define HIF_SER_COPY_HTSHM_16(dest, src)\
{\
    **(dest) = (LSA_UINT16)(src);\
    *(dest) = *(dest)+1;\
}
#endif

/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_32()
*  Purpose:     - copies uint32 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_HTSHM_32(dest, src)\
{\
	LSA_UINT32 temp = 0;\
	temp = HIF_LTOSHM_L(src);\
	HIF_MEMCPY((LSA_UINT32*)*(dest),&temp,4);\
	*(dest) = *(dest)+1;\
}
#else
#define HIF_SER_COPY_HTSHM_32(dest, src)\
{\
    **(dest) = (LSA_UINT32)(src);\
	*(dest) = *(dest)+1;\
}
#endif

/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_64()
*  Purpose:     - copies uint64 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 8Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_HTSHM_64(dest, src)\
{\
	LSA_UINT64 temp = 0;\
	temp = HIF_LTOSHM_LL(src);\
	HIF_MEMCPY((LSA_UINT64*)*(dest),&temp,8);\
	*(dest) = *(dest)+2;\
}
#else
#define HIF_SER_COPY_HTSHM_64(dest, src)\
{\
	HIF_MEMCPY((LSA_UINT64*)*(dest),&(src),8);\
	*(dest) = *(dest)+2;\
}
#endif
/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_BUF()
*  Purpose:     - copies data from host to shared-mem without swapping
*			    - increases help-pointer
*  Input:       src, destination address and size
*  Return:      -
****************************************************************************/
#define HIF_SER_COPY_HTSHM_BUF(dest, src, size)\
{\
	HIF_MEMCPY((LSA_UINT32*)*(dest),(src),(size));\
	*(dest) = (LSA_UINT32*)((LSA_UINT8*)(*(dest))+(HIF_SER_SIZEOF(size)));\
}

/****************************************************************************
*  Name:        HIF_SER_COPY_SHMTH_8()
*  Purpose:     - copies uint8 from host to shared-mem
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#define HIF_SER_COPY_SHMTH_8(dest, src)\
{\
    *((LSA_UINT8*)(dest)) = **((LSA_UINT8**)(src));\
    *(src) = *(src)+1;\
}

/****************************************************************************
*  Name:        HIF_SER_COPY_SHMTH_16()
*  Purpose:     - copies uint16 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_SHMTH_16(dest, src)\
{\
	LSA_UINT16 temp = 0;\
	temp = HIF_SHMTOL_S((LSA_UINT16)**(src));\
	HIF_MEMCPY((LSA_UINT16*)(dest),&temp,2);\
	*(src) = *(src)+1;\
}
#else
#define HIF_SER_COPY_SHMTH_16(dest, src)\
{\
    *((LSA_UINT8*)(dest)) = **((LSA_UINT8**)(src));\
    *((LSA_UINT8*)(dest) + 1) = *((*(LSA_UINT8**)(src)) + 1);\
	*(src) = *(src)+1;\
}
#endif

/****************************************************************************
*  Name:        HIF_SER_COPY_SHMTH_32()
*  Purpose:     - copies uint32 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 4Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_SHMTH_32(dest, src)\
{\
	LSA_UINT32 temp = 0;\
	temp = HIF_SHMTOL_L((LSA_UINT32)**(src));\
	HIF_MEMCPY((LSA_UINT32*)(dest),&temp,4);\
	*(src) = *(src)+1;\
}
#else
#if defined HIF_CFG_ALIGNMENT_SAVE_COPY_FUNCTION // attention: for arm platforms (ertec200p, ertec200, ertec400) this define has to be set! 32 bit copy does not work for misaligned 8/16 bit enums!  
#define HIF_SER_COPY_SHMTH_32(dest, src)\
{\
    *((LSA_UINT8*)(dest)) = **((LSA_UINT8**)(src));\
    *((LSA_UINT8*)(dest) + 1) = *((*(LSA_UINT8**)(src)) + 1);\
    *((LSA_UINT8*)(dest) + 2) = *((*(LSA_UINT8**)(src)) + 2);\
    *((LSA_UINT8*)(dest) + 3) = *((*(LSA_UINT8**)(src)) + 3);\
    *(src) = *(src)+1;\
}
#else
#define HIF_SER_COPY_SHMTH_32(dest, src)\
{\
    *((LSA_UINT32*)(dest)) = **((LSA_UINT32**)(src));\
    *(src) = *(src)+1;\
}
#endif
#endif
/****************************************************************************
*  Name:        HIF_SER_COPY_SHMTH_64()
*  Purpose:     - copies uint64 from host to shared-mem and takes care of swapping
*			    - increases help-pointer + 8Byte
*  Input:       src, destination address
*  Return:      -
****************************************************************************/
#if defined LSA_HOST_ENDIANESS_BIG
#define HIF_SER_COPY_SHMTH_64(dest, src)\
{\
	LSA_UINT64 temp = 0;\
	temp = HIF_SHMTOL_LL((LSA_UINT64)**(src));\
	HIF_MEMCPY((LSA_UINT64*)(dest),&temp,8);\
	*(src) = *(src)+2;\
}
#else
#define HIF_SER_COPY_SHMTH_64(dest, src)\
{\
	HIF_MEMCPY((LSA_UINT64*)(dest),*(src),8);\
	*(src) = *(src)+2;\
}
#endif


/*=============================================================================
 * function name:  HIF_SER_COPY_SHMTH_BUF
 *
 * function:       Copies an Buffer from the shared memory to host memory
 *                 and increases src pointer
 *
 * parameters:     LSA_UINT32*  dest  - Destination Ptr for the value 
 *                 LSA_UINT32** src   - Source Values which should be copied
 *                 LSA_UINT32   size  - Length of the buffer in bytes
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#define HIF_SER_COPY_SHMTH_BUF(dest, src, size)\
{\
	HIF_MEMCPY((dest),(LSA_VOID*)*(src),(size));\
	*(src) = (LSA_UINT32*)((LSA_UINT8*)*src+HIF_SER_SIZEOF(size));\
}

/*===========================================================================*/
/*                           Serialization                                   */
/*===========================================================================*/

/****************************************************************************
* Name:     HIF_SER_SIZEOF_OPCODE_TYPE
*
* Purpose:  returns the length of the "opcode" of the RQB 
*			used to calculate the rqb_size for serialization.
*			length is calculated including padding - HIF_SER_SIZEOF()
*
* Input:    - 
*
* Output:   -
* Return:   length of opcode
****************************************************************************/
#define HIF_SER_SIZEOF_OPCODE_TYPE() HIF_SER_SIZEOF(sizeof(LSA_OPCODE_TYPE))

/****************************************************************************
* Name:     HIF_SER_SIZEOF_RESPONSE_TYPE
*
* Purpose:  returns the length of the "response" of the RQB 
*			used to calculate the rqb_size for serialization.
*			length is calculated including padding - HIF_SER_SIZEOF()
*
* Input:    - 
*
* Output:   -
* Return:   length of response
****************************************************************************/
#define HIF_SER_SIZEOF_RESPONSE_TYPE() HIF_SER_SIZEOF(sizeof(LSA_RESPONSE_TYPE))


/****************************************************************************
* Name:     HIF_SER_SIZEOF_COMP_ID_TYPE
*
* Purpose:  returns the length of the "comp_id" of the RQB 
*			used to calculate the rqb_size for serialization.
*			length is calculated including padding - HIF_SER_SIZEOF()
*
* Input:    - 
*
* Output:   -
* Return:   length of comp_id
****************************************************************************/
#define HIF_SER_SIZEOF_COMP_ID_TYPE() HIF_SER_SIZEOF(sizeof(LSA_COMP_ID_TYPE))

/****************************************************************************
* Name:     HIF_SER_SIZEOF_SYS_PATH_TYPE
*
* Purpose:  returns the length of the "sys_path" of the RQB 
*			used to calculate the rqb_size for serialization.
*			length is calculated including padding - HIF_SER_SIZEOF()
*
* Input:    - 
*
* Output:   -
* Return:   length of sys_path
****************************************************************************/
#define HIF_SER_SIZEOF_SYS_PATH_TYPE() HIF_SER_SIZEOF(sizeof(LSA_SYS_PATH_TYPE))

/****************************************************************************
*  Name:        HIF_SER_OPCODE_HTSHM()
*  Purpose:     serializes the opcode 
*  Input:       dest - destination in shared_mem
*               opcode
*  Return:      -
****************************************************************************/
#define HIF_SER_OPCODE_HTSHM(dest, opcode) HIF_SER_COPY_HTSHM_8(dest,opcode)

/****************************************************************************
*  Name:        HIF_DESER_GET_OPCODE()
*  Purpose:     gets the opcode from serialized data in shared-mem
*  Input:       rqbShm ... serialized RQB in sharde-mem
*  Return:      opcode
****************************************************************************/
#define HIF_DESER_GET_OPCODE(rqbShm, opcode) HIF_MEMCPY(&opcode,(LSA_UINT32*)rqbShm+1,sizeof(LSA_OPCODE_TYPE))

/****************************************************************************
*  Name:        HIF_SER_RESPONSE_HTSHM()
*  Purpose:     - serializes the response
*  Input:       destination in shared_mem, response
*  Return:      -
****************************************************************************/
#define HIF_SER_RESPONSE_HTSHM(dest, response) HIF_SER_COPY_HTSHM_16(dest,response)

/****************************************************************************
*  Name:        HIF_SER_RESPONSE_SHMTH()
*  Purpose:     deserializes the response
*  Input:       response, src in shared_mem
*  Return:      -
****************************************************************************/
#define HIF_SER_RESPONSE_SHMTH(response, src) HIF_SER_COPY_SHMTH_16(response,src)

/****************************************************************************
*  Name:        HIF_SER_COMP_ID_HTSHM()
*  Purpose:     - serializes the comp_id
*  Input:       destination in shared_mem, comp_id
*  Return:      -
****************************************************************************/
#define HIF_SER_COMP_ID_HTSHM(dest, comp_id) HIF_SER_COPY_HTSHM_16(dest,comp_id)

/****************************************************************************
*  Name:        HIF_SER_COMP_ID_SHMTH()
*  Purpose:     - deserializes the comp_id
*  Input:       comp_id, src in shared_mem
*  Return:      -
****************************************************************************/
#define HIF_SER_COMP_ID_SHMTH(comp_id, src) HIF_SER_COPY_SHMTH_16(comp_id,src)

/****************************************************************************
*  Name:        HIF_DESER_GET_COMP_ID_FROM_SHM()
*  Purpose:     gets the comp_id from serialized data in shared-mem
*				from the begin of the serialized data
*  Input:       rqbShm ... begin of serialized RQB in sharde-mem
*  Return:      opcode
****************************************************************************/
#define HIF_DESER_GET_COMP_ID_FROM_SHM(rqbShm, comp_id) HIF_MEMCPY(&comp_id,(LSA_UINT32*)rqbShm+2,sizeof(LSA_COMP_ID_TYPE))

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_INT_H */
