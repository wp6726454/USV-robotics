#ifndef HIF_SYS_H
#define HIF_SYS_H

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
/*  F i l e               &F: hif_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                            HIF transfer modes                             */
/*===========================================================================*/
#define HIF_TM_SHARED_MEM           1
#define HIF_TM_SHORT                2
#define HIF_TM_MIXED                3
#define HIF_TM_SHARED_MEM_NO_SER    4

/*===========================================================================*/
/*                       Max number of pipes / device                        */
/*===========================================================================*/
#define HIF_MAX_PIPES_INST 128

/*===========================================================================*/
/*                       HIF_CFG_MEM_RING_CHECK_OVERWRITE                    */
/*===========================================================================*/
/** 
  * This switch can be activated to check HIF serialization/deserialization overwrite in Shared Memory.
  * Possible values are:
  *
  * HIF_MEM_RING_CHECK_START_STOP_1
  * Restrictions: currently only available when using HIF_MEM_RING
  * After every serialization the current SER writepointer will be checked against the RQB SHM start pointer + RQB serialization size.
  * Will generate a START sequenze (STAR (0x)) before the allocated SER buffer and a STOP sequenze (STOP (0x)) after the buffer. These 
  * 8 Bytes (2 * 4 Bytes) will be transparent for the "user". After every de-/serialization HIF will check if the START and STOP sequences
  * are okay and not overwritten.
  *
  * HIF_MEM_RING_CHECK_START_STOP_4
  * Restrictions: currently only available when using HIF_MEM_RING
  * Includes HIF_MEM_RING_CHECK_START_STOP_1 but with 4 START sequences at the beginning and 4 STOP sequences after the allocated shared memory
  **/
#define HIF_MEM_RING_CHECK_START_STOP_1          1
#define HIF_MEM_RING_CHECK_START_STOP_4          4

/*===========================================================================*/
/*                       constants for detail-pointer                        */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define HIF_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag *

/*------------------------------------------------------------------------------
// Enum HIF_SEND_IRQ_MODE_TYPE (defines behavior before sending irq)
//----------------------------------------------------------------------------*/
typedef enum
{
    HIF_SEND_IRQ_MODE_INVALID    = 0,
    HIF_SEND_IRQ_AFTER_NO_WAIT   = 1, // default is no wait before sending irq!
    HIF_SEND_IRQ_AFTER_READ_BACK = 2, // read back before sending irq to make sure irq is deliverd after data!
    HIF_SEND_IRQ_AFTER_TIMEOUT   = 3  // for further performance optimation explicit timeout before sending irq can be used.
} HIF_SEND_IRQ_MODE_TYPE; 

/*------------------------------------------------------------------------------
// hif_get_par_tag
//----------------------------------------------------------------------------*/

typedef struct hif_get_par_tag
{
    HIF_HANDLE hH;                                    //Out: HIF Handle
    LSA_UINT16 Mode;                                  //In:  HIF Transfer Modes: Shared Mem Transfer / Pass Through / Mixed
    struct
    {
        struct
        {
			LSA_BOOL bUseTimerPoll;                   //In:  If true HIF starts an Timer for polling shared memory transfer activities (needed in case interrupts are not fully supported)
			HIF_SEND_IRQ_MODE_TYPE SendIrqMode;       //In:  mode for sending irq's (no_wait/read_back/timeout before send)
			LSA_UINT64 uNsWaitBeforeSendIrq;          //In:  ns timeout before sending irq (only used if send_irq_mode is set to timeout mode)     
			struct
			{
				LSA_UINT8* Base;                      //In:  Base Address of HIF Shared Mem Interface (Shared Mem Mode/Mixed Mode) (must be aligned correctly e.g. 32Bit)
				LSA_UINT32 Size;                      //In:  Size fo HIF Shared Mem in Bytes
			} Local;
			struct
			{
				LSA_UINT8* Base;                      //In:  Base Address of HIF Shared Mem Interface (Shared Mem Mode/Mixed Mode) (must be aligned correctly e.g. 32Bit)
				LSA_UINT32 Size;                      //In:  Size fo HIF Shared Mem in Bytes
			} Remote;
        } SharedMem;                                  //Shared Mem Transfer Mode / Mixed Mode
        HIF_HANDLE hif_lower_device;                  //In:  Lower HIF handle for Pass Through / Mixed Mode (Open Upper Device only)
        struct
        {
			LSA_UINT16 Mode;                          //In:  Shared Mem Mode / Pass Through Mode
        } Pipes[HIF_MAX_PIPES_INST];                  //Only used in Mixed Mode
    } if_pars;    
} HIF_GET_PAR_TYPE;

typedef struct hif_get_par_tag  *HIF_GET_PAR_PTR_TYPE;

/*------------------------------------------------------------------------------
// hif_detail_tag
//----------------------------------------------------------------------------*/

typedef struct hif_detail_tag
{
	HIF_HANDLE hH;
	HIF_HD_ID_TYPE hd_id;
	HIF_IF_ID_TYPE if_id;
	HIF_PIPE_ID_TYPE pipe_id;
} HIF_DETAIL_TYPE;

typedef struct hif_detail_tag   *HIF_DETAIL_PTR_TYPE;

/*--------------------------------------------------------------------------------------
// hif_component_support_std_tag / hif_compononet_support_sys_tag / serialization types
//--------------------------------------------------------------------------------------*/

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
typedef LSA_UINT16 (*HIF_SERIALIZE_CBF_TYPE)  (HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_INT hShmPool, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest, LSA_UINT32* pSerRqbLen);
typedef LSA_UINT16 (*HIF_DESERIALIZE_CBF_TYPE)(HIF_SYS_HANDLE hSysDev, LSA_VOID* pHifPipeHandle, LSA_VOID* pRqbSrc, LSA_VOID** ppRqbDest);
#endif

typedef struct hif_component_support_std_tag
{
	LSA_BOOL        (*IsCloseChannelRqb)            (LSA_VOID* pRqb);
	LSA_VOID        (*ReplaceOpenChannelLowerHandle)(HIF_RQB_PTR_TYPE pRQB, LSA_HANDLE_TYPE hHifChHandle);
	LSA_HANDLE_TYPE (*GetOpenChannelLowerHandle)    (HIF_RQB_PTR_TYPE pRQB);
	LSA_UINT16      (*ReplaceOpenChannelCbf)        (HIF_RQB_PTR_TYPE pRQB, HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf);
	LSA_UINT16      (*GetOpenChannelCompInfo)       (HIF_RQB_PTR_TYPE pRQB, HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,	LSA_SYS_PATH_TYPE *pPath, LSA_HANDLE_TYPE *hUpper);

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	HIF_SERIALIZE_CBF_TYPE   SerializeUpper;
	HIF_DESERIALIZE_CBF_TYPE DeserializeLower;
	HIF_SERIALIZE_CBF_TYPE   SerializeLower;
	HIF_DESERIALIZE_CBF_TYPE DeserializeUpper;
#endif
} HIF_COMPONENT_SUPPORT_STD_TYPE, *HIF_COMPONENT_SUPPORT_STD_PTR_TYPE;

typedef struct hif_compononet_support_sys_tag
{
	LSA_VOID (*GetSysRequesthH)      (HIF_HANDLE* phH, HIF_RQB_PTR_TYPE pRQB);
	LSA_VOID (*GetSysRequestCbf)     (HIF_SYS_HANDLE hSysDev, HIF_RQB_PTR_TYPE pRQB, HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE *ppCbf);
	LSA_VOID (*ReplaceSysRequestCbf) (HIF_SYS_HANDLE hSysDev, HIF_RQB_PTR_TYPE pRQB, HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf);

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	HIF_SERIALIZE_CBF_TYPE   SerializeUpper;
	HIF_DESERIALIZE_CBF_TYPE DeserializeLower;
	HIF_SERIALIZE_CBF_TYPE   SerializeLower;
	HIF_DESERIALIZE_CBF_TYPE DeserializeUpper;
#endif
} HIF_COMPONENT_SUPPORT_SYS_TYPE, *HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define HIF_SER_SIZEOF(x) (((x) + 3) & ~3) /* Returns sizeof corrected to shared memory alignment */

#define HIF_FATAL(error_code_0)	{\
	hif_fatal_error (HIF_MODULE_ID, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL\
		);\
	}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  hif_init
 *
 * function:       initialize hif
 *
 * parameters:     HIF_INIT_PTR_TYPE  init
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16 hif_init(
    LSA_VOID
);

/*=============================================================================
 * function name:  hif_undo_init
 *
 * function:       undo the initialization of hif
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16 hif_undo_init(
    LSA_VOID
);

/*=============================================================================
 * function name:  hif_register_standard_component
 *
 * function:       register a component support structure
 *
 * parameters:     HIF_COMPONENT_SUPPORT_STD_PTR_TYPE
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              LSA_RET_ERR_PARAM
 *                              or others
 *===========================================================================*/
LSA_UINT16 hif_register_standard_component(
	LSA_COMP_ID_TYPE CompId, 
	HIF_COMPONENT_SUPPORT_STD_PTR_TYPE pCompFuncs
);

/*=============================================================================
 * function name:  hif_register_sys_component
 *
 * function:       register a component support structure
 *
 * parameters:     HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              LSA_RET_ERR_PARAM
 *                              or others
 *===========================================================================*/
LSA_UINT16 hif_register_sys_component(
	LSA_COMP_ID_TYPE CompId, 
	HIF_COMPONENT_SUPPORT_SYS_PTR_TYPE pCompFuncs
);

/*=============================================================================
 * function name:  hif_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_USER_ID_TYPE  user_id:   id of hif
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID hif_timeout(
    LSA_UINT16        timer_id,
    LSA_USER_ID_TYPE  user_id
);

/*=============================================================================
 * function name:  hif_poll
 *
 * function:       hif poll service routine
 *
 * parameters:     LSA_VOID 
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID hif_poll(
	LSA_VOID
);

/*=============================================================================
 * function name:  hif_interrupt
 *
 * function:       hif interrupt service routine
 *
 * parameters:     HIF_HANDLE        hH: Target Hif Device 
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID hif_interrupt(
	HIF_HANDLE hH
);

/*====  out functions  =====*/

/*===========================================================================*/
/*                                 common                                    */
/*===========================================================================*/

/****************************************************************************
* Name:     HIF_ALLOC_TIMER
*
* Purpose:  Function allocates a new LSA timer.
* Input:    timer_type	- Pointer to address for response
*           timer_base	- Timer base:
*                           LSA_TIME_BASE_1MS
*                           LSA_TIME_BASE_10MS
*                           LSA_TIME_BASE_100MS
*                           LSA_TIME_BASE_1S
*                           LSA_TIME_BASE_10S
*                           LSA_TIME_BASE_100S
*
* Output:   ret_val_ptr	    - Pointer to address for response
*           timer_id_ptr	- Pointer to the address for the Timer ID
* Return:   -
****************************************************************************/
#ifndef HIF_ALLOC_TIMER
LSA_VOID
HIF_ALLOC_TIMER(
		LSA_UINT16  *ret_val_ptr,
		LSA_UINT16  *timer_id_ptr,
		LSA_UINT16  timer_type,
        LSA_UINT16  time_base
);
#endif

/****************************************************************************
* Name:     HIF_START_TIMER
*
* Purpose:  Starts the LSA timer, previously allocated with HIF_ALLOC_TIMER.
* Input:    timer_id    - Timer ID that was obtained at HIF_ALLOC_TIMER
*           user_id	    - User identification of the timer
*           time		- Runtime of the timer as multiple of the timer_base. >0
*
* Output:   ret_val_ptr	    - Pointer to address for response
* Return:   -
****************************************************************************/
#ifndef HIF_START_TIMER
LSA_VOID
HIF_START_TIMER(
        LSA_UINT16          *ret_val_ptr,
        LSA_TIMER_ID_TYPE   timer_id,
        LSA_USER_ID_TYPE    user_id,
        LSA_UINT16          time
);
#endif

/****************************************************************************
* Name:     HIF_STOP_TIMER
*
* Purpose:  Stops the LSA timer, previously started with HIF_START_TIMER.
* Input:    timer_id        - Timer ID that was obtained at HIF_ALLOC_TIMER
* Output:   ret_val_ptr	    - Pointer to address for response
* Return:   -
****************************************************************************/
#ifndef HIF_STOP_TIMER
LSA_VOID
HIF_STOP_TIMER(
        LSA_UINT16          *ret_val_ptr,
        LSA_TIMER_ID_TYPE   timer_id
);
#endif

/****************************************************************************
* Name:     HIF_FREE_TIMER
*
* Purpose:  Frees the LSA timer, previously allocated with HIF_ALLOC_TIMER.
* Input:    timer_id        - Timer ID that was obtained at HIF_ALLOC_TIMER
* Output:   ret_val_ptr	    - Pointer to address for response
* Return:   -
****************************************************************************/
#ifndef HIF_FREE_TIMER
LSA_VOID
HIF_FREE_TIMER(
        LSA_UINT16          *ret_val_ptr,
        LSA_TIMER_ID_TYPE   timer_id
);
#endif

/****************************************************************************
* Name:     HIF_GET_NS_TICKS
*
* Purpose:  Returns ns ticks for performance measurements.
* Input:    -
* Output:   LSA_UINT64 - current ns ticks from performance counter.
* Return:   -
****************************************************************************/
#ifndef HIF_GET_NS_TICKS
LSA_UINT64
HIF_GET_NS_TICKS( 
        LSA_VOID
);
#endif

/****************************************************************************
* Name:     HIF_WAIT_NS
*
* Purpose:  This function waits time in ns.
* Input:    uTimeNs      - time to wait in ns
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_WAIT_NS
LSA_VOID
HIF_WAIT_NS( 
        LSA_UINT64 uTimeNs
);
#endif

/*=============================================================================
 * function name:  HIF_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_ENTER
LSA_VOID   
HIF_ENTER(
    LSA_VOID
);
#endif


/*=============================================================================
 * function name:  HIF_EXIT
 *
 * function:       reset reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_EXIT
LSA_VOID
HIF_EXIT(
        LSA_VOID
);
#endif

/*=============================================================================
 * function name:  HIF_ALLOC_REENTRANCE_LOCK
 *
 * function:       alloc reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_ALLOC_REENTRANCE_LOCK
LSA_VOID 
HIF_ALLOC_REENTRANCE_LOCK( 
        LSA_UINT16  * ret_val_ptr, 
        LSA_UINT16  * lock_handle_ptr
);
#endif

/*=============================================================================
 * function name:  HIF_FREE_REENTRANCE_LOCK
 *
 * function:       free reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_FREE_REENTRANCE_LOCK
LSA_VOID
HIF_FREE_REENTRANCE_LOCK(
        LSA_UINT16   *ret_val_ptr,
        LSA_UINT16   lock_handle 
);
#endif

/*=============================================================================
 * function name:  HIF_ENTER_REENTRANCE_LOCK
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_ENTER_REENTRANCE_LOCK
LSA_VOID
HIF_ENTER_REENTRANCE_LOCK( 
        LSA_UINT16 lock_handle
);
#endif

/*=============================================================================
 * function name:  HIF_EXIT_REENTRANCE_LOCK
 *
 * function:       reset reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef HIF_EXIT_REENTRANCE_LOCK
LSA_VOID
HIF_EXIT_REENTRANCE_LOCK( 
        LSA_UINT16 lock_handle
);
#endif

/****************************************************************************
* 
* Name:     HIF_ALLOC_LOCAL_MEM
*
* Purpose:  The function allocates length bytes in the local memory, and
*           returns a pointer to this area.  The memory area has to
*           be correctly aligned for all data types (that is, 32 bit
*           aligned as a rule). If a NULL pointer is returned, it was
*           not possible to allocate memory. This macro is used 
*           internally only for allocating RQBs in the local memory
*           of HIF_UPPER.
*
* Input:    length      - Length of the memory area to be allocated 
*                             (in bytes)
*
* Output:   ptr_ptr_mem - Pointer to address for pointer to memory 
*                         space. If NULL is returned here, then no 
*                         memory could be allocated.
* Return:   -
****************************************************************************/
#ifndef HIF_ALLOC_LOCAL_MEM
LSA_VOID
HIF_ALLOC_LOCAL_MEM(
        LSA_VOID_PTR_TYPE   *ptr_ptr_mem,
        LSA_UINT32          length
);
#endif

/****************************************************************************
* Name:     HIF_FREE_LOCAL_MEM
*
* Purpose:  The function frees the memory that was previously allocated 
*           with HIF_ALLOC_LOCAL_MEM. mem_ptr is the pointer that
*           was returned by HIF_ALLOC_LOCAL_MEM.
*
* Input:    ptr_mem     - pointer to memory area that is to be freed
* Output:   ret_val_ptr - pointer to address for response
* Return:   -
****************************************************************************/
#ifndef HIF_FREE_LOCAL_MEM
LSA_VOID
HIF_FREE_LOCAL_MEM(
        LSA_UINT16          *ret_val_ptr,
        LSA_VOID_PTR_TYPE   ptr_mem
);
#endif

/****************************************************************************
* Name:     HIF_CREATE_SER_POOL
*
* Purpose:  Function creates a serialization memory pool for the RQBs.
*           In standard case serialization memory is located in the shared 
*           memory of the Logical Device. Function stores the ID of the pool 
*           in pool_id.
*
* Input:    base			- Base addres of the shared memory.
*           size			- Size of memory pool (in bytes).
*           sys_handle      - Handle of the HIF instance
*
* Output:   pool		    - ID of the memory pool
*
* Return:   -
****************************************************************************/
#ifndef HIF_CREATE_SER_POOL
LSA_VOID
HIF_CREATE_SER_POOL(
        LSA_VOID_PTR_TYPE   base,
        LSA_UINT32          size,
	    HIF_SYS_HANDLE      sys_handle,
	    LSA_INT*            pool
);
#endif

/****************************************************************************
* Name:     HIF_DELETE_SER_POOL
*
* Purpose:  Function deletes a serialization memory pool for the RQBs.
*
* Input:    pool		    - memory pool
*           sys_handle      - Handle of the HIF instance
* Output:   ret_val_ptr     - pointer to address for response
*
* Return:   -
****************************************************************************/
#ifndef HIF_DELETE_SER_POOL
LSA_VOID
HIF_DELETE_SER_POOL(
	    LSA_UINT16          *ret_val_ptr,
		HIF_SYS_HANDLE      sys_handle,
        LSA_INT             pool
);
#endif

/****************************************************************************
* Name:     HIF_ALLOC_SER_MEM
*
* Purpose:  The function allocates length bytes in serialization and returns 
*           a pointer to this area. The memory area has to be correctly 
*           aligned for all data types (that is, 32bit aligned as a rule). 
*           If a NULL pointer is returned, it was not possible to allocate 
*           memory. This macro is used for allocating RQBs in the shared 
*           memory. The function is used also in the serializing functions 
*           for copying RQBs from local memory to the shared memory.
*
* Input:    length		- Length of the memory area to be allocated in bytes
*           sys_handle  - Handle of the HIF instance
*           pool        - Memory pool
*
* Output:   ptr_ptr_mem	- Pointer to address for pointer to memory space.
*                           If NULL is returned here, then no memory could 
*                           be allocated.
* Return:   -
****************************************************************************/
#ifndef HIF_ALLOC_SER_MEM
LSA_VOID
HIF_ALLOC_SER_MEM(
        LSA_VOID_PTR_TYPE   *ptr_ptr_mem,
        LSA_UINT32          length,
        HIF_SYS_HANDLE      sys_handle,
        LSA_INT             pool
);
#endif

/****************************************************************************
* Name:     HIF_FREE_SER_MEM
*
* Purpose:  The function frees memory that was previously allocated with
*           HIF_ALLOC_SER_MEM. ptr_mem is the pointer that was returned by
*           HIF_ALLOC_SER_MEM.
*
* Input:    ptr_mem		- Pointer to memory area that is to be freed
*           sys_handle  - Handle of the HIF instance
*           pool        - Memory pool
*
* Output:   ret_val_ptr	- Pointer to address for response
* Return:   -
****************************************************************************/
#ifndef HIF_FREE_SER_MEM
LSA_VOID
HIF_FREE_SER_MEM(
        LSA_UINT16          *ret_val_ptr,
        LSA_VOID_PTR_TYPE   ptr_mem,
        HIF_SYS_HANDLE      sys_handle,
        LSA_INT             pool
);
#endif

/****************************************************************************
* Name:     HIF_SHM_DMA_COPY
*
* Purpose:  The function gives the system adaption the opportunity to transfer 
*           the serialized rqb to shared memory by using a dma transfer.
*           For using this functionality HIF_ALLOC_SER_MEM needs to return buffers which are not located in shm
*           The ptr_ptr_rqb variable points to the buffer allocated by HIF_ALLOC_SER_MEM. The system adaption
*           should replace this pointer by the target shared memory pointer which is addressed by the DMA.
*
* Input:    ptr_ptr_rqb		- In: Pointer to Pointer of serialized RQB - The address is located in the area delivered by HIF_ALLOC_SER_MEM
*                             Out: Pointer to new RQB located in the shared memory region
*           rqb_len         - Length of serialized RQB
*           sys_handle      - Sys handle of the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_SHM_DMA_COPY
LSA_VOID HIF_SHM_DMA_COPY(
		LSA_VOID** ptr_ptr_rqb,
		LSA_UINT32 rqb_len,
		HIF_SYS_HANDLE sys_handle
);
#endif

/****************************************************************************
* Name:     HIF_MEMSET
*
* Purpose:  Fills the memory with value.
*
* Input:    ptr_mem	- Pointer to the memory that is to be filled with value
*           value   - Value with which the memory is to be filled
*           length	- Memory length in bytes
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_MEMSET
LSA_VOID
HIF_MEMSET(
        LSA_VOID_PTR_TYPE   ptr_mem,
        LSA_INT             value,
	    LSA_UINT            length
);
#endif

/****************************************************************************
* Name:     HIF_MEMCPY
*
* Purpose:  Copies Memory
*
* Input:    dst	- destination pointer
*           src - source pointer
*           len - length
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_MEMCPY
LSA_VOID HIF_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT          len 
);
#endif

/****************************************************************************
* Name:     HIF_MEMCMP
*
* Purpose:  Compares Memory
*
* Input:    pBuf1	- destination pointer
*           pBuf2   - source pointer
*           Length  - length
*
* Output:   -
* Return:   LSA_INT
            < 0	the first byte that does not match in both memory blocks has a lower value in ptr1 than in ptr2 
              0	the contents of both memory blocks are equal
            > 0	the first byte that does not match in both memory blocks has a greater value in ptr1 than in ptr2 
****************************************************************************/
#ifndef HIF_MEMCMP
LSA_UINT HIF_MEMCMP(
	LSA_VOID_PTR_TYPE pBuf1,
	LSA_VOID_PTR_TYPE pBuf2,
	LSA_UINT          Length 
);
#endif

/****************************************************************************
* Name:     HIF_LTOSHM_S
*
* Purpose:  Converts the byte order of the local address space into byte
*           order of the shared memory address space. Used for the short
*           unsigned integer variables.
*
* Input:    local_short - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_LTOSHM_S
LSA_UINT16
HIF_LTOSHM_S(
	    LSA_UINT16  local_short );
#endif

/****************************************************************************
* Name:     HIF_LTOSHM_L
*
* Purpose:  Converts the byte order of the local address space into byte
*           order of the shared memory address space. Used for the long
*           unsigned integer variables.
*
* Input:    local_long - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_LTOSHM_L
LSA_UINT32
HIF_LTOSHM_L(
	    LSA_UINT32  local_long );
#endif

/****************************************************************************
* Name:     HIF_LTOSHM_LL
*
* Purpose:  Converts the byte order of the local address space into byte
*           order of the shared memory address space. Used for the double long
*           unsigned integer variables (64Bit).
*
* Input:    local_dlong - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_LTOSHM_LL
LSA_UINT64
HIF_LTOSHM_LL(
	    LSA_UINT64  local_dlong );
#endif

/****************************************************************************
* Name:     HIF_SHMTOL_S
*
* Purpose:  Converts the byte order of the shared memory address space into
*           byte order of the local address space. Used for the short
*           unsigned integer variables.
*
* Input:    shm_short - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_SHMTOL_S
LSA_UINT16
HIF_SHMTOL_S(
	    LSA_UINT16  shm_short );
#endif

/****************************************************************************
* Name:     HIF_SHMTOL_L
*
* Purpose:  Converts the byte order of the shared memory address space into
*           byte order of the local address space. Used for the long
*           unsigned integer variables.
*
* Input:    shm_long - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_SHMTOL_L
LSA_UINT32
HIF_SHMTOL_L(
	    LSA_UINT32  shm_long );
#endif

/****************************************************************************
* Name:     HIF_SHMTOL_LL
*
* Purpose:  Converts the byte order of the shared memory address space into
*           byte order of the local address space. Used for the long
*           unsigned integer variables.
*
* Input:    shm_dlong - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_SHMTOL_LL
LSA_UINT64
HIF_SHMTOL_LL(
	    LSA_UINT64  shm_dlong );
#endif

/****************************************************************************
* Name:     HIF_NTOH_S
*
* Purpose:  Converts the byte order of the network into
*           byte order of the local address space. 
*
* Input:    value - Variable to be converted
*
* Output:   -
* Return:   Variable in converted byte order
****************************************************************************/
#ifndef HIF_NTOH_S
LSA_UINT16 HIF_NTOH_S(
	LSA_UINT16 value );
#endif

/****************************************************************************
* Name:     HIF_FATAL_ERROR
*
* Purpose:  Function writes information to the administration buffer 
*           (exception handling)
* Input:    length      - LSA_COMP_ID
*           error_ptr   - pointer to error description
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_FATAL_ERROR
LSA_VOID
HIF_FATAL_ERROR (
        LSA_UINT16                  length,
        HIF_FATAL_ERROR_PTR_TYPE    error_ptr
);
#endif

/****************************************************************************
* Name:     HIF_STRLEN
*
* Purpose:  returns the length of a zero-terminated string 
*
* Input:    str - ptr to string
*
* Output:   -
* Return:   length of string
****************************************************************************/
#ifndef HIF_STRLEN
LSA_UINT 
HIF_STRLEN(LSA_VOID_PTR_TYPE str);
#endif

/****************************************************************************
*  Name:        HIF_SER_SYS_PATH_HTSHM()
*  Purpose:     - serializes the sys_path
*  Input:       destination in shared_mem, sys_path
*  Return:      -
****************************************************************************/
#ifndef HIF_SER_SYS_PATH_HTSHM
LSA_VOID HIF_SER_SYS_PATH_HTSHM(
		LSA_UINT32** dest,
		LSA_SYS_PATH_TYPE sys_path  );
#else
# error "by design a function!"
#endif

/****************************************************************************
*  Name:        HIF_SER_SYS_PATH_SHMTH()
*  Purpose:     - deserializes the sys_path
*  Input:       sys_path, src in shared_mem
*  Return:      -
****************************************************************************/
#ifndef HIF_SER_SYS_PATH_SHMTH
LSA_VOID HIF_SER_SYS_PATH_SHMTH(
		LSA_SYS_PATH_TYPE* sys_path,
		LSA_UINT32** src);
#else
# error "by design a function!"
#endif


/****************************************************************************
*  Name:        HIF_NRT_ALLOC_TX_MEM() / HIF_NRT_ALLOC_RX_MEM()
*  Purpose:     used during de-/serializing to alloc EDD NRT SEND/RECV Frames
*  Input:       mem_ptr_ptr ... pointer to new allocated buffer
*			    length ... length of allocated buffer
*               hd_id ... hd_id of the pipe requesting memory
*  Return:      -
****************************************************************************/
#ifndef HIF_NRT_ALLOC_TX_MEM
LSA_VOID HIF_NRT_ALLOC_TX_MEM( 
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id);
#else
# error "by design a function!"
#endif

#ifndef HIF_NRT_ALLOC_RX_MEM
LSA_VOID HIF_NRT_ALLOC_RX_MEM( 
	LSA_VOID_PTR_TYPE   *mem_ptr_ptr,
	LSA_UINT32          length,
	HIF_HD_ID_TYPE      hd_id);
#else
# error "by design a function!"
#endif

/****************************************************************************
*  Name:        HIF_NRT_FREE_TX_MEM() / HIF_NRT_FREE_TX_MEM()
*  Purpose:     used during de-/serializing to free EDD NRT SEND/RECV Frames
*  Input:       ret_val_ptr ... pointer to free result
*	   		    mem_ptr ... pointer of nrt mem to be freed 
*               hd_id ... hd_id of the pipe freeing memory
*  Return:      -
****************************************************************************/
#ifndef HIF_NRT_FREE_TX_MEM
LSA_VOID HIF_NRT_FREE_TX_MEM(
	LSA_UINT16           *ret_val_ptr,
	LSA_VOID_PTR_TYPE    mem_ptr,
	HIF_HD_ID_TYPE      hd_id);
#else
# error "by design a function!"
#endif

#ifndef HIF_NRT_FREE_RX_MEM
LSA_VOID HIF_NRT_FREE_RX_MEM(
	LSA_UINT16           *ret_val_ptr,
	LSA_VOID_PTR_TYPE    mem_ptr,
	HIF_HD_ID_TYPE      hd_id);
#else
# error "by design a function!"
#endif

/*===========================================================================*/
/*                 Check Shared Memory for corruption                        */
/*===========================================================================*/
#ifndef HIF_SHM_CHECK_OVERWRITE_SERIALIZE
LSA_VOID HIF_SHM_CHECK_OVERWRITE_SERIALIZE(
    LSA_UINT32          opcode,
    LSA_VOID          * ptr_ptr_rqb_dest,
    LSA_UINT32          rqb_size,
    LSA_UINT8         * help_ptr,
    const LSA_CHAR    * func);
#else
# error "by design a function!"
#endif

#ifndef HIF_SHM_CHECK_OVERWRITE_DESERIALIZE
LSA_VOID HIF_SHM_CHECK_OVERWRITE_DESERIALIZE(
    LSA_VOID          * ptr_rqb_src,
    LSA_UINT32          opcode,
    LSA_UINT8         * help_ptr,
    const LSA_CHAR    * func);
#else
# error "by design a function!"
#endif

/*===========================================================================*/
/*                           Common RQB                                      */
/*===========================================================================*/
#if ( HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
#ifndef HIF_REQUEST_LOWER_DONE
LSA_VOID HIF_REQUEST_LOWER_DONE(
    HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE ptr_lower_rqb, 
    LSA_SYS_PTR_TYPE hSysDev );
#else
# error "by design a function"
#endif
#endif

#if ( HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1 )
#ifndef HIF_SYS_REQUEST_LOWER_DONE
LSA_VOID HIF_SYS_REQUEST_LOWER_DONE(
    HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_request_lower_done_ptr,
    HIF_RQB_PTR_TYPE ptr_lower_rqb, 
    LSA_SYS_PTR_TYPE hSysDev );
#else
# error "by design a function"
#endif
#endif


/*===========================================================================*/
/*                           Logical Device (LD)                             */
/*===========================================================================*/

/****************************************************************************
* Name:     HIF_LD_ENTER
*
* Purpose:  Function sets reentrance lock
* Input:    -
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_ENTER
LSA_VOID
HIF_LD_ENTER(LSA_VOID);
#endif


/****************************************************************************
* Name:     HIF_LD_EXIT
*
* Purpose:  Function cancels reentrance lock
* Input:    -
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_EXIT
LSA_VOID
HIF_LD_EXIT(LSA_VOID);
#endif

/****************************************************************************
* Name:     HIF_LD_DO_POLL_REQUEST
*
* Purpose:  This function is called from within the function hif_poll().
*           System Adaptation must then call the hif_internal_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_DO_POLL_REQUEST
LSA_VOID
HIF_LD_DO_POLL_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_LD_DO_TIMEOUT_REQUEST
*
* Purpose:  This function is called from within the function hif_timeout().
*           System Adaptation must then call the hif_internal_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_DO_TIMEOUT_REQUEST
LSA_VOID
HIF_LD_DO_TIMEOUT_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_LD_DO_REQUEST_LOCAL
*
* Purpose:  This function is called from within the function hif_ld_request().
*           System Adaptation must then call the hif_ld_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_DO_REQUEST_LOCAL
LSA_VOID
HIF_LD_DO_REQUEST_LOCAL(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_LD_U_GET_PATH_INFO
*
* Purpose:  Function obtains channel path information. Function is called
*           in the Open Channel service in order to get the information from
*           system adaptation regarding the channel. If system adaptation 
*           recognizes an invalid path, it returns the error code 
*           LSA_RET_ERR_SYS_PATH.
*
* Input:    path	        - LSA path info as specified for Open Channel
*
* Output:   ret_val_ptr	    - Pointer to address for the response
*           sys_ptr_ptr	    - Pointer to address for LSA Sys Ptr
*           detail_ptr_ptr	- Pointer to address for Detail Ptr
* Return:   -
****************************************************************************/
#ifndef HIF_LD_U_GET_PATH_INFO
LSA_VOID
HIF_LD_U_GET_PATH_INFO(
        LSA_UINT16              *ret_val_ptr,
        LSA_SYS_PTR_TYPE	    *sys_ptr_ptr,
        HIF_DETAIL_PTR_TYPE	    *detail_ptr_ptr,
        LSA_SYS_PATH_TYPE       path
);
#endif

/****************************************************************************
* Name:     HIF_LD_L_GET_PATH_INFO
*
* Purpose:  Function obtains channel path information. Function is called
*           in the Open Channel service in order to get the information from
*           system adaptation regarding the channel. If system adaptation 
*           recognizes an invalid path, it returns the error code 
*           LSA_RET_ERR_SYS_PATH.
*
* Input:    path	        - LSA path info as specified for Open Channel
*
* Output:   ret_val_ptr	    - Pointer to address for the response
*           sys_ptr_ptr	    - Pointer to address for LSA Sys Ptr
*           detail_ptr_ptr	- Pointer to address for Detail Ptr
* Return:   -
****************************************************************************/
#ifndef HIF_LD_L_GET_PATH_INFO
LSA_VOID
HIF_LD_L_GET_PATH_INFO(
        LSA_UINT16              *ret_val_ptr,
        LSA_SYS_PTR_TYPE	    *sys_ptr_ptr,
        HIF_DETAIL_PTR_TYPE	    *detail_ptr_ptr,
        LSA_SYS_PATH_TYPE       path
);
#endif

/****************************************************************************
* Name:     HIF_LD_U_RELEASE_PATH_INFO
*
* Purpose:  Function releases channel path information. This function is 
*           called when a channel is closed (Close Channel). In the process,
*           the two pointers sys_ptr and detail_ptr are returned to system
*           adaptation.
*           If the pointers are invalid, the output macro returns 
*           LSA_RET_ERR_PARAM.
*
* Input:    detail_ptr	- Detail Ptr that was transferred to the HIF when 
*                           HIF_LD_GET_PATH_INFO was called.
*           sys_ptr	    - Sys Ptr that was transferred to the HIF when 
*                           HIF_LD_GET_PATH_INFO was called
*
* Output:   ret_val_ptr	    - Pointer to address for the response
* Return:   -
****************************************************************************/
#ifndef HIF_LD_U_RELEASE_PATH_INFO
LSA_VOID
HIF_LD_U_RELEASE_PATH_INFO(
		LSA_UINT16	            *ptr_dst,
		LSA_SYS_PTR_TYPE        sys_ptr,
		HIF_DETAIL_PTR_TYPE     detail_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_L_RELEASE_PATH_INFO
*
* Purpose:  Function releases channel path information. This function is 
*           called when a channel is closed (Close Channel). In the process,
*           the two pointers sys_ptr and detail_ptr are returned to system
*           adaptation.
*           If the pointers are invalid, the output macro returns 
*           LSA_RET_ERR_PARAM.
*
* Input:    detail_ptr	- Detail Ptr that was transferred to the HIF when 
*                           HIF_LD_GET_PATH_INFO was called.
*           sys_ptr	    - Sys Ptr that was transferred to the HIF when 
*                           HIF_LD_GET_PATH_INFO was called
*
* Output:   ret_val_ptr	    - Pointer to address for the response
* Return:   -
****************************************************************************/
#ifndef HIF_LD_L_RELEASE_PATH_INFO
LSA_VOID
HIF_LD_L_RELEASE_PATH_INFO(
		LSA_UINT16	            *ptr_dst,
		LSA_SYS_PTR_TYPE        sys_ptr,
		HIF_DETAIL_PTR_TYPE     detail_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_U_GET_PARAMS
*
* Purpose:  Function is called during the opening sequence of HIF. 
*           Function retrieves parameters, required for the setup of the 
*           upper layer of Logical Device.
*
* Input:    hSysDev	- Device handle   
*
* Output:   ret_val_ptr	- Pointer to address for response
*           pPars	- Pointer to the data structure of type HIF_GET_PAR_TYPE
* Return:   -
****************************************************************************/
#ifndef HIF_LD_U_GET_PARAMS
LSA_VOID
HIF_LD_U_GET_PARAMS(
        LSA_UINT16              *ret_val_ptr,
	    HIF_SYS_HANDLE          hSysDev,
	    HIF_GET_PAR_PTR_TYPE    pPars 
);
#endif

/****************************************************************************
* Name:     HIF_LD_L_GET_PARAMS
*
* Purpose:  Function is called during the opening sequence of HIF. 
*           Function retrieves parameters, required for the setup of the 
*           lower layer of Logical Device.
*
* Input:    hSysDev	- Device handle   
*
* Output:   ret_val_ptr	- Pointer to address for response
*           pPars	- Pointer to the data structure of type HIF_GET_PAR_TYPE
* Return:   -
****************************************************************************/
#ifndef HIF_LD_L_GET_PARAMS
LSA_VOID
HIF_LD_L_GET_PARAMS(
        LSA_UINT16              *ret_val_ptr,
	    HIF_SYS_HANDLE          hSysDev,
	    HIF_GET_PAR_PTR_TYPE    pPars 
);
#endif

/****************************************************************************
* Name:     HIF_LD_DO_INTERRUPT_REQUEST
*
* Purpose:  This function is called from within the function hif_interrupt().
*           System Adaptation must then call the hifl_request() function with
*           the specified RQB. System Adaptation must execute a transition
*           in the context of HIF, if necessary. However, in a typical
*           implementation System Adaptation sends a mail containing the RQB
*           to the HIF task.
*
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*                           to hif_ request()
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_DO_INTERRUPT_REQUEST
LSA_VOID
HIF_LD_DO_INTERRUPT_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_LD_ENABLE_ISR
*
* Purpose:  Function enables interrupts to the other layer of HIF LD.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_ENABLE_ISR
LSA_VOID
HIF_LD_ENABLE_ISR(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_LD_DISABLE_ISR
*
* Purpose:  Function disables interrupts to the other layer of HIF LD.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_DISABLE_ISR
LSA_VOID
HIF_LD_DISABLE_ISR(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_LD_SEND_IRQ
*
* Purpose:  This macro shall trigger the interrupt to the receiver side.
*           Macro is called by hif_request() and hif_return_rqb() after the 
*           RQB is serialized into the shared memory.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_SEND_IRQ
LSA_VOID
HIF_LD_SEND_IRQ(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_LD_ACK_IRQ
*
* Purpose:  This macro shall acknowledge the interrupt on initator side.
*           Macro is called by hif_request() in front of access to the shared memory.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_ACK_IRQ
LSA_VOID
HIF_LD_ACK_IRQ(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_LD_OPEN_DEVICE_LOWER 
*
* Purpose:  Transfers the Open Device RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open device request
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_OPEN_DEVICE_LOWER 
LSA_VOID
HIF_LD_OPEN_DEVICE_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_CLOSE_DEVICE_LOWER 
*
* Purpose:  Transfers the Close Device RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open channel request
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_CLOSE_DEVICE_LOWER 
LSA_VOID
HIF_LD_CLOSE_DEVICE_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_SYSTEM_REQUEST_LOWER 
*
* Purpose:  Transfers the System RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB)
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_SYSTEM_REQUEST_LOWER 
LSA_VOID
HIF_LD_SYSTEM_REQUEST_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_OPEN_CHANNEL_LOWER
*
* Purpose:  Opens a channel to the lower LSA layer of the Logical Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open channel request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_OPEN_CHANNEL_LOWER 
LSA_VOID
HIF_LD_OPEN_CHANNEL_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_LD_CLOSE_CHANNEL_LOWER
*
* Purpose:  Closes a channel to the lower LSA layer of the Logical Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           close channel request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_CLOSE_CHANNEL_LOWER 
LSA_VOID
HIF_LD_CLOSE_CHANNEL_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_LD_REQUEST_LOWER
*
* Purpose:  This function transfers asynchronous requests to the lower
*           LSA layer of the Logical Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for LSA request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_REQUEST_LOWER 
LSA_VOID
HIF_LD_REQUEST_LOWER (
        HIF_RQB_PTR_TYPE  lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_LD_REQUEST_UPPER_DONE
*
* Purpose:  This macro is called by HIF when a user channel request (RQB)
*           from calling hif_open_channel(), hif_request() or 
*           hif_close_channel() is finished.
*           The system adaptation has to call the upper callback function
*           to finish the request.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for LSA request
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_REQUEST_UPPER_DONE 
LSA_VOID
HIF_LD_REQUEST_UPPER_DONE (
        HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb, 
        LSA_SYS_PTR_TYPE sys_ptr
);
#endif

/****************************************************************************
* Name:     HIF_LD_L_SYSTEM_REQUEST_DONE
*
* Purpose:  This function is called by HIF LD Lower when a request (RQB)
*           from calling hif_system() is finished.
*           The system adaptation has to call the callback function to
*           finish the request.
*
* Input:    hif_system_request_done_ptr	- pointer to the callback function
*           ptr_upper_rqb				- pointer to upper RQB
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_L_SYSTEM_REQUEST_DONE 
LSA_VOID
HIF_LD_L_SYSTEM_REQUEST_DONE (
        HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_LD_U_SYSTEM_REQUEST_DONE
*
* Purpose:  This function is called by HIF LD Upper when a request (RQB)
*           from calling hif_system() is finished.
*           The system adaptation has to call the callback function to
*           finish the request.
*
* Input:    hif_system_request_done_ptr	- pointer to the callback function
*           ptr_upper_rqb				- pointer to upper RQB
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_LD_U_SYSTEM_REQUEST_DONE 
LSA_VOID
HIF_LD_U_SYSTEM_REQUEST_DONE (
        HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif


/*===========================================================================*/
/*                           Hardware Device (HD)                            */
/*===========================================================================*/

/****************************************************************************
* Name:     HIF_HD_ENTER
*
* Purpose:  Function sets reentrance lock
* Input:    -
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_ENTER
LSA_VOID
HIF_HD_ENTER(LSA_VOID);
#endif


/****************************************************************************
* Name:     HIF_HD_EXIT
*
* Purpose:  Function cancels reentrance lock
* Input:    -
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_EXIT
LSA_VOID
HIF_HD_EXIT(LSA_VOID);
#endif

/****************************************************************************
* Name:     HIF_HD_DO_POLL_REQUEST
*
* Purpose:  This function is called from within the function hif_poll().
*           System Adaptation must then call the hif_internal_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_DO_POLL_REQUEST
LSA_VOID
HIF_HD_DO_POLL_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_HD_DO_TIMEOUT_REQUEST
*
* Purpose:  This function is called from within the function hif_timeout().
*           System Adaptation must then call the hif_internal_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_DO_TIMEOUT_REQUEST
LSA_VOID
HIF_HD_DO_TIMEOUT_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_HD_DO_REQUEST_LOCAL
*
* Purpose:  This function is called from within the function hif_hd_request().
*           System Adaptation must then call the hif_hd_request() 
*           function with the specified RQB. System Adaptation must execute 
*           a transition in the context of HIF, if necessary. However, in a 
*           typical implementation System Adaptation sends a mail containing 
*           the RQB to the HIF task.
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*           to hif_request()
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_DO_REQUEST_LOCAL
LSA_VOID
HIF_HD_DO_REQUEST_LOCAL(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_HD_U_GET_PATH_INFO
*
* Purpose:  Function obtains channel path information. Function is called
*           in the Open Channel service in order to get the information from
*           system adaptation regarding the channel. If system adaptation 
*           recognizes an invalid path, it returns the error code 
*           LSA_RET_ERR_SYS_PATH.
*
* Input:    path	        - LSA path info as specified for Open Channel
*
* Output:   ret_val_ptr	    - Pointer to address for the response
*           sys_ptr_ptr	    - Pointer to address for LSA Sys Ptr
*           detail_ptr_ptr	- Pointer to address for Detail Ptr
* Return:   -
****************************************************************************/
#ifndef HIF_HD_U_GET_PATH_INFO
LSA_VOID
HIF_HD_U_GET_PATH_INFO(
        LSA_UINT16              *ret_val_ptr,
        LSA_SYS_PTR_TYPE	    *sys_ptr_ptr,
        HIF_DETAIL_PTR_TYPE	    *detail_ptr_ptr,
        LSA_SYS_PATH_TYPE       path
);
#endif

/****************************************************************************
* Name:     HIF_HD_L_GET_PATH_INFO
*
* Purpose:  Function obtains channel path information. Function is called
*           in the Open Channel service in order to get the information from
*           system adaptation regarding the channel. If system adaptation 
*           recognizes an invalid path, it returns the error code 
*           LSA_RET_ERR_SYS_PATH.
*
* Input:    path	        - LSA path info as specified for Open Channel
*
* Output:   ret_val_ptr	    - Pointer to address for the response
*           sys_ptr_ptr	    - Pointer to address for LSA Sys Ptr
*           detail_ptr_ptr	- Pointer to address for Detail Ptr
* Return:   -
****************************************************************************/
#ifndef HIF_HD_L_GET_PATH_INFO
LSA_VOID
HIF_HD_L_GET_PATH_INFO(
        LSA_UINT16              *ret_val_ptr,
        LSA_SYS_PTR_TYPE	    *sys_ptr_ptr,
        HIF_DETAIL_PTR_TYPE	    *detail_ptr_ptr,
        LSA_SYS_PATH_TYPE       path
);
#endif

/****************************************************************************
* Name:     HIF_HD_U_RELEASE_PATH_INFO
*
* Purpose:  Function releases channel path information. This function is 
*           called when a channel is closed (Close Channel). In the process,
*           the two pointers sys_ptr and detail_ptr are returned to system
*           adaptation.
*           If the pointers are invalid, the output macro returns 
*           LSA_RET_ERR_PARAM.
*
* Input:    detail_ptr	- Detail Ptr that was transferred to the HIF when 
*                           HIF_HD_GET_PATH_INFO was called.
*           sys_ptr	    - Sys Ptr that was transferred to the HIF when 
*                           HIF_HD_GET_PATH_INFO was called
*
* Output:   ret_val_ptr	    - Pointer to address for the response
* Return:   -
****************************************************************************/
#ifndef HIF_HD_U_RELEASE_PATH_INFO
LSA_VOID
HIF_HD_U_RELEASE_PATH_INFO(
		LSA_UINT16	        *ptr_dst,
		LSA_SYS_PTR_TYPE    sys_ptr,
		HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_L_RELEASE_PATH_INFO
*
* Purpose:  Function releases channel path information. This function is 
*           called when a channel is closed (Close Channel). In the process,
*           the two pointers sys_ptr and detail_ptr are returned to system
*           adaptation.
*           If the pointers are invalid, the output macro returns 
*           LSA_RET_ERR_PARAM.
*
* Input:    detail_ptr	- Detail Ptr that was transferred to the HIF when 
*                           HIF_HD_GET_PATH_INFO was called.
*           sys_ptr	    - Sys Ptr that was transferred to the HIF when 
*                           HIF_HD_GET_PATH_INFO was called
*
* Output:   ret_val_ptr	    - Pointer to address for the response
* Return:   -
****************************************************************************/
#ifndef HIF_HD_L_RELEASE_PATH_INFO
LSA_VOID
HIF_HD_L_RELEASE_PATH_INFO(
		LSA_UINT16	        *ptr_dst,
		LSA_SYS_PTR_TYPE    sys_ptr,
		HIF_DETAIL_PTR_TYPE detail_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_U_GET_PARAMS
*
* Purpose:  Function is called during the opening sequence of HIF. 
*           Function retrieves parameters, required for the setup of the 
*           upper layer of Hardware Device.
*
* Input:    hSysDev	- Device handle   
*
* Output:   ret_val_ptr	- Pointer to address for response
*           pPars	- Pointer to the data structure of type HIF_GET_PAR_TYPE
* Return:   -
****************************************************************************/
#ifndef HIF_HD_U_GET_PARAMS
LSA_VOID
HIF_HD_U_GET_PARAMS(
        LSA_UINT16              *ret_val_ptr,
	    HIF_SYS_HANDLE          hSysDev,
	    HIF_GET_PAR_PTR_TYPE    pPars 
);
#endif

/****************************************************************************
* Name:     HIF_HD_L_GET_PARAMS
*
* Purpose:  Function is called during the opening sequence of HIF. 
*           Function retrieves parameters, required for the setup of the 
*           lower layer of Hardware Device.
*
* Input:    hSysDev	- Device handle   
*
* Output:   ret_val_ptr	- Pointer to address for response
*           pPars	- Pointer to the data structure of type HIF_GET_PAR_TYPE
* Return:   -
****************************************************************************/
#ifndef HIF_HD_L_GET_PARAMS
LSA_VOID
HIF_HD_L_GET_PARAMS(
        LSA_UINT16              *ret_val_ptr,
	    HIF_SYS_HANDLE          hSysDev,
	    HIF_GET_PAR_PTR_TYPE    pPars 
);
#endif

/****************************************************************************
* Name:     HIF_HD_DO_INTERRUPT_REQUEST
*
* Purpose:  This function is called from within the function hif_interrupt().
*           System Adaptation must then call the hifl_request() function with
*           the specified RQB. System Adaptation must execute a transition
*           in the context of HIF, if necessary. However, in a typical
*           implementation System Adaptation sends a mail containing the RQB
*           to the HIF task.
*
* Input:    ptr_upper_rqb	- Pointer to the RQB that is to be transferred 
*                           to hif_ request()
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_DO_INTERRUPT_REQUEST
LSA_VOID
HIF_HD_DO_INTERRUPT_REQUEST(
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_HD_ENABLE_ISR
*
* Purpose:  Function enables interrupts to the other layer of HIF HD.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_ENABLE_ISR
LSA_VOID
HIF_HD_ENABLE_ISR(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_HD_DISABLE_ISR
*
* Purpose:  Function disables interrupts to the other layer of HIF HD.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_DISABLE_ISR
LSA_VOID
HIF_HD_DISABLE_ISR(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_HD_SEND_IRQ
*
* Purpose:  This macro shall trigger the interrupt to the receiver side.
*           Macro is called by hif_request() and hif_return_rqb() after the 
*           RQB is serialized into the shared memory.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_SEND_IRQ
LSA_VOID
HIF_HD_SEND_IRQ(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_HD_ACK_IRQ
*
* Purpose:  This macro shall acknowledge the interrupt on initator side.
*           Macro is called by hif_request() in front of access to the shared memory.
*
* Input:    hSysDev	- Device handle
*           hH 		- Reference to the HIF instance
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_ACK_IRQ
LSA_VOID
HIF_HD_ACK_IRQ(
        HIF_SYS_HANDLE hSysDev,
        HIF_HANDLE hH
);
#endif

/****************************************************************************
* Name:     HIF_HD_OPEN_DEVICE_LOWER 
*
* Purpose:  Transfers the Open Device RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open device request
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_OPEN_DEVICE_LOWER 
LSA_VOID
HIF_HD_OPEN_DEVICE_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_CLOSE_DEVICE_LOWER 
*
* Purpose:  Transfers the Close Device RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open channel request
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_CLOSE_DEVICE_LOWER 
LSA_VOID
HIF_HD_CLOSE_DEVICE_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_SYSTEM_REQUEST_LOWER 
*
* Purpose:  Transfers the System RQB to the lower LSA layer.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB)
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_SYSTEM_REQUEST_LOWER 
LSA_VOID
HIF_HD_SYSTEM_REQUEST_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_OPEN_CHANNEL_LOWER
*
* Purpose:  Opens a channel to the lower LSA layer of the Hardware Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           open channel request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_OPEN_CHANNEL_LOWER 
LSA_VOID
HIF_HD_OPEN_CHANNEL_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_HD_CLOSE_CHANNEL_LOWER
*
* Purpose:  Closes a channel to the lower LSA layer of the Hardware Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for 
*                           close channel request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_CLOSE_CHANNEL_LOWER 
LSA_VOID
HIF_HD_CLOSE_CHANNEL_LOWER (
        HIF_RQB_PTR_TYPE lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_HD_REQUEST_LOWER
*
* Purpose:  This function transfers asynchronous requests to the lower
*           LSA layer of the Hardware Device.
*
* Input:    lower_rqb_ptr	- Pointer to request block (RQB) for LSA request
*           hSysDev         - Sys Device Ptr
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_REQUEST_LOWER 
LSA_VOID
HIF_HD_REQUEST_LOWER (
        HIF_RQB_PTR_TYPE  lower_rqb_ptr,
		HIF_SYS_HANDLE hSysDev
);
#endif

/****************************************************************************
* Name:     HIF_HD_REQUEST_UPPER_DONE
*
* Purpose:  This macro is called by HIF when a user channel request (RQB)
*           from calling hif_open_channel(), hif_request() or 
*           hif_close_channel() is finished.
*           The system adaptation has to call the upper callback function
*           to finish the request.
*
* Input:    ptr_upper_rqb	- Pointer to request block (RQB) for LSA request
*           hif_request_upper_done_ptr	- pointer to the callback function
*           sys_ptr     - System pointer
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_REQUEST_UPPER_DONE 
LSA_VOID
HIF_HD_REQUEST_UPPER_DONE (
        HIF_UPPER_CALLBACK_FCT_PTR_TYPE hif_request_upper_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb, 
        LSA_SYS_PTR_TYPE sys_ptr
);
#endif

/****************************************************************************
* Name:     HIF_HD_L_SYSTEM_REQUEST_DONE
*
* Purpose:  This function is called by HIF LD Lower when a request (RQB)
*           from calling hif_system() is finished.
*           The system adaptation has to call the callback function to
*           finish the request.
*
* Input:    hif_system_request_done_ptr	- pointer to the callback function
*           ptr_upper_rqb				- pointer to upper RQB
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_L_SYSTEM_REQUEST_DONE 
LSA_VOID
HIF_HD_L_SYSTEM_REQUEST_DONE (
        HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/****************************************************************************
* Name:     HIF_HD_U_SYSTEM_REQUEST_DONE
*
* Purpose:  This function is called by HIF LD Upper when a request (RQB)
*           from calling hif_system() is finished.
*           The system adaptation has to call the callback function to
*           finish the request.
*
* Input:    hif_system_request_done_ptr	- pointer to the callback function
*           ptr_upper_rqb				- pointer to upper RQB
*
* Output:   -
* Return:   -
****************************************************************************/
#ifndef HIF_HD_U_SYSTEM_REQUEST_DONE 
LSA_VOID
HIF_HD_U_SYSTEM_REQUEST_DONE (
        HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE hif_system_request_done_ptr,
        HIF_RQB_PTR_TYPE ptr_upper_rqb
);
#endif

/*===========================================================================*/
/* non-LSA-predefined             prototyping                                */
/*===========================================================================*/


/*=============================================================================
 * function name:  hif_fatal_error
 *
 * function:       helper to fill
 *                 LSA_FATAL_ERROR_TYPE  hif_glob_fatal_error
 *                 and then calls HIF_FATAL_ERROR
 *
 *===========================================================================*/

LSA_VOID
hif_fatal_error (
	LSA_UINT16  module_id,
	LSA_UINT16  line,
	LSA_UINT32	eror_code_0,
	LSA_UINT32	eror_code_1,
	LSA_UINT32	eror_code_2,
	LSA_UINT32	eror_code_3,
	LSA_UINT16  error_data_len,
	LSA_VOID_PTR_TYPE  error_data
	);

/*=============================================================================
 * function name:  hif_ld_system
 *
 * function:       send a system request
 *
 * parameters:     HIF_RQB_PTR_TYPE  pRQB:  pointer to system-
 *                                                     RQB
 *
 * return value:   LSA_RET_OK, LSA_RET_ERR_PARAM, LSA_RET_ERR_SEQUENCE
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_UINT16  hif_ld_system(
    HIF_RQB_PTR_TYPE  pRQB
);

/*=============================================================================
 * function name:  hif_hd_system
 *
 * function:       send a system request
 *
 * parameters:     HIF_RQB_PTR_TYPE  pRQB:  pointer to system-
 *                                                     RQB
 *
 * return value:   LSA_RET_OK, LSA_RET_ERR_PARAM, LSA_RET_ERR_SEQUENCE
 *
 * RQB-parameters:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:                    Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *     RQB-header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB-args:         response:  *_RSP_OK
 *                                  *_RSP_ERR_*
 *                                  or others
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_UINT16  hif_hd_system(
    HIF_RQB_PTR_TYPE  pRQB
);

/*=============================================================================
 * function name:  hif_open_device_lower_done
 *
 * function:       finish a HIF_XX_UPPER_DEVICE_OPEN request
 *
 * parameters:     HIF_RQB_PTR_TYPE pRQB  - Request from open device
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID hif_open_device_lower_done(
	HIF_RQB_PTR_TYPE pRQB
);

/*=============================================================================
 * function name:  hif_close_device_lower_done
 *
 * function:       finish a HIF_XX_UPPER_DEVICE_CLOSE request
 *
 * parameters:     HIF_RQB_PTR_TYPE pRQB  - Request from open device
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID hif_close_device_lower_done(
	HIF_RQB_PTR_TYPE pRQB
);

/*=============================================================================
 * function name:  HIF_SER_COPY_SHMTH_LSA_BOOL
 * function name:  HIF_SER_COPY_HTSHM_LSA_BOOL
 *
 * function:       resolves the COPY-function to the correct LSA_BOOL-type
 *
 *===========================================================================*/

#if (HIF_LSA_BOOL_SIZE == 1)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_8
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_8
#elif(HIF_LSA_BOOL_SIZE == 2)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_16
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_16
#elif(HIF_LSA_BOOL_SIZE == 4)
    #define HIF_SER_COPY_HTSHM_LSA_BOOL     HIF_SER_COPY_HTSHM_32
    #define HIF_SER_COPY_SHMTH_LSA_BOOL     HIF_SER_COPY_SHMTH_32
#else
    #error "Invalid HIF_LSA_BOOL_SIZE (!=1 und !=2 und !=4) defined in HIF_CFG.H!"
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_SYS_H */
