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
/*  F i l e               &F: hif_mem_ring.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements hif ring pool memory management                               */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID      38
#define LTRC_ACT_MODUL_ID  38


#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "hif_int.h"
#include "hif_mem_ring.h"

/**********************************************************************
 * Types
 *********************************************************************/
/**
 * Structure to store information about allocated memory blocks.
 *
 * @see HIF_MEM_RING_POOL_TYPE
 */
typedef struct hif_mem_ring_block_desc
{
    LSA_UINT8 *prev_alloc_end;    ///< alloc_end of hif pool at allocation time, used for consistency check in hif_mem_ring_free_from_pool
    LSA_UINT32 size;              ///< Size of the memory block (including the block descriptor)
#ifdef HIF_CFG_MEM_RING_CHECK_OVERWRITE
    LSA_UINT32 orig_size;         ///< Original size of the allocated memory
#endif
} HIF_MEM_RING_BLOCK_DESC_TYPE, *HIF_MEM_RING_BLOCK_DESC_PTR_TYPE;

/**
 * Structure to store information about allocated memory blocks.
 *
 * @see HIF_MEM_RING_POOL_TYPE
 */
typedef struct hif_mem_ring_block_desc_external
{
    HIF_MEM_RING_BLOCK_DESC_PTR_TYPE  allocated_block;   ///< allocated memory block, used for consistency check in hif_mem_ring_free_from_pool
    LSA_UINT32                        size;              ///< Size of the memory block (including the block descriptor)
} HIF_MEM_RING_BLOCK_DESC_EXTERNAL_TYPE, *HIF_MEM_RING_BLOCK_DESC_EXTERNAL_PTR_TYPE;


/**
 * Type to encapsulate ring pool management structure
 */
typedef struct hif_mem_ring_pool
{

    LSA_UINT16 enter_exit_id;   ///< lock used to protect the memory management variables
  
    LSA_UINT8 *pool_begin;      ///< base adress of ring pool, marks beginning of ring pool
    LSA_UINT8 *pool_end;        ///< base adress of ring pool + size of ring pool, marks end of ring pool
    LSA_UINT8 *alloc_begin;     ///< marks the beginnig of the allocated/used area of the ring pool
    LSA_UINT8 *alloc_end;       ///< marks the end of the allocated/used area and also the beginning of the free area of the ring pool
    LSA_UINT32 configured_size; ///< configured size of the ring pool in bytes
    LSA_UINT32 pool_usage;      ///< current ring pool usage in bytes
    LSA_UINT32 max_pool_usage;  ///< "high water mark"; maximum ring pool usage so far in bytes
    LSA_UINT32 alloc_calls;     ///< number of successful calls to hif_mem_ring_alloc_from_pool()
    LSA_UINT32 free_calls;      ///< number of successful calls to hif_mem_ring_free_from_pool()
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    HIF_MEM_RING_BLOCK_DESC_EXTERNAL_TYPE allocated_block_desc[HIF_SHM_RB_BUFFER_LENGTH] ;
    LSA_UINT32                            allocated_block_desc_count;
    LSA_UINT32                            next_alloc_block_desc_idx;
    LSA_UINT32                            next_free_block_desc_idx;
#endif
} HIF_MEM_RING_POOL_TYPE, *HIF_MEM_RING_POOL_PTR_TYPE;

/// Size of the block descriptor aligned to the specified alignment - Including START sequences in case of HIF check overwrite
#if (defined HIF_CFG_MEM_RING_CHECK_OVERWRITE) && (HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_1)
#define HIF_MEM_RING_BLOCK_DESC_SIZE (((sizeof(HIF_MEM_RING_BLOCK_DESC_TYPE)+sizeof(LSA_UINT32)/*one STAR-MagicValue*/+(HIF_CFG_MEM_RING_POOL_ALIGN)-1) / HIF_CFG_MEM_RING_POOL_ALIGN) * HIF_CFG_MEM_RING_POOL_ALIGN)
#elif (defined HIF_CFG_MEM_RING_CHECK_OVERWRITE) && (HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_4)
#define HIF_MEM_RING_BLOCK_DESC_SIZE (((sizeof(HIF_MEM_RING_BLOCK_DESC_TYPE)+sizeof(LSA_UINT32)*4/*four STAR-MagicValue*/+(HIF_CFG_MEM_RING_POOL_ALIGN)-1) / HIF_CFG_MEM_RING_POOL_ALIGN) * HIF_CFG_MEM_RING_POOL_ALIGN)
#else /* no START STOP check */
#define HIF_MEM_RING_BLOCK_DESC_SIZE (((sizeof(HIF_MEM_RING_BLOCK_DESC_TYPE)+(HIF_CFG_MEM_RING_POOL_ALIGN)-1) / HIF_CFG_MEM_RING_POOL_ALIGN) * HIF_CFG_MEM_RING_POOL_ALIGN)
#endif
/**********************************************************************
 * External variables
 *********************************************************************/


/**********************************************************************
 * Global variables
 *********************************************************************/
#if (HIF_CFG_MEM_RING_POOL_MAX > 0)
HIF_MEM_RING_POOL_TYPE hif_mem_ring_pools[HIF_CFG_MEM_RING_POOL_MAX] = {{0}}; // init with zeros to mark all pools as free 
#endif

/**********************************************************************
 * Functions
 *********************************************************************/

/**
 * \brief Init hif ring pool
 * 
 * @param pool_begin
 * @param pool_size
 * @param hif_mem_ring_pool_ptr
 */
LSA_VOID_PTR_TYPE hif_mem_ring_pool_init(LSA_UINT8* pool_begin, LSA_UINT32 pool_size, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr)
{
    LSA_UINT8 *begin;
    LSA_UINT32 size;
    LSA_UINT32 remainder;
    LSA_UINT16 rsp;
    HIF_MEM_RING_POOL_PTR_TYPE ring_pool_data_ptr;
    
    if(hif_mem_ring_pool_ptr != LSA_NULL)
    {
        // use given helper pointer for memory management data of pool
        ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE)hif_mem_ring_pool_ptr;
        begin = pool_begin;
        size = pool_size;
    }
    else
    {
        // create memory management data for hif ring pool at the beginning of pool
        ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) pool_begin;
        size = pool_size;
        // make sure ring_pool_data_ptr fits the requested alignment
        remainder = (LSA_UINT32)ring_pool_data_ptr % HIF_CFG_MEM_RING_POOL_ALIGN;
        if(remainder != 0)
        {
            ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) (pool_begin + HIF_CFG_MEM_RING_POOL_ALIGN - remainder); 
            size -= HIF_CFG_MEM_RING_POOL_ALIGN;
        }
        
        begin = (LSA_UINT8*)ring_pool_data_ptr + sizeof(HIF_MEM_RING_POOL_TYPE);
        size -= sizeof(HIF_MEM_RING_POOL_TYPE);
    }
    
    HIF_ALLOC_REENTRANCE_LOCK(&rsp, &ring_pool_data_ptr->enter_exit_id );
    HIF_ASSERT(rsp == LSA_RET_OK);
    
    // make sure begin and size fits the requested alignment
    remainder = (LSA_UINT32)begin % HIF_CFG_MEM_RING_POOL_ALIGN;
    if(remainder != 0)
    {
        begin += HIF_CFG_MEM_RING_POOL_ALIGN - remainder;
        size -= HIF_CFG_MEM_RING_POOL_ALIGN;
    }
    
    // Initialize pool data
    ring_pool_data_ptr->pool_begin=begin;
    ring_pool_data_ptr->pool_end=begin+size;
    ring_pool_data_ptr->alloc_begin=begin;
    ring_pool_data_ptr->alloc_end=begin;
    ring_pool_data_ptr->configured_size=size;
    ring_pool_data_ptr->pool_usage=0;
    ring_pool_data_ptr->max_pool_usage=0;
    ring_pool_data_ptr->free_calls=0;
    ring_pool_data_ptr->alloc_calls=0;
    
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    ring_pool_data_ptr->allocated_block_desc_count=0;
    ring_pool_data_ptr->next_alloc_block_desc_idx=0;
    ring_pool_data_ptr->next_free_block_desc_idx=0;
#endif
    
    return ring_pool_data_ptr;
}


/**
 * \brief Undo init hif ring pool
 * 
 * @param hif_mem_ring_pool_ptr
 */
LSA_VOID hif_mem_ring_pool_undo_init(LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr)
{
    LSA_UINT16 rsp;
    HIF_MEM_RING_POOL_PTR_TYPE ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) hif_mem_ring_pool_ptr;
    
    HIF_FREE_REENTRANCE_LOCK(&rsp, ring_pool_data_ptr->enter_exit_id);
    HIF_ASSERT(rsp == LSA_RET_OK);
    HIF_MEMSET(ring_pool_data_ptr, 0, sizeof(HIF_MEM_RING_POOL_TYPE));
}

/**
 * \brief Create hif ring pool
 * 
 * @param begin
 * @param size
 * @return
 */
LSA_INT hif_mem_ring_pool_create(LSA_UINT8* begin, LSA_UINT32 size)
{
    LSA_INT pool_nr = -1;
    
#if (HIF_CFG_MEM_RING_POOL_MAX > 0)
	LSA_INT pool_idx;

    HIF_ENTER();
    
    for ( pool_idx = 0; pool_idx < HIF_CFG_MEM_RING_POOL_MAX; pool_idx++ )
    {
        if ( 0 == hif_mem_ring_pools[pool_idx].configured_size )
        {
            hif_mem_ring_pool_init(begin, size, &hif_mem_ring_pools[pool_idx]);
            pool_nr = pool_idx + 1;
            break;
        }
    }
    
    HIF_EXIT();
#else
    LSA_UNUSED_ARG( begin );
    LSA_UNUSED_ARG( size );
#endif
    
    return pool_nr;
}

/**
 * \brief Delete hif ring pool
 * 
 * @param pool_nr
 */
LSA_VOID hif_mem_ring_pool_delete(LSA_INT pool_nr)
{
    HIF_ASSERT(pool_nr > 0 && pool_nr <= HIF_CFG_MEM_RING_POOL_MAX);
#if (HIF_CFG_MEM_RING_POOL_MAX > 0)
    HIF_ENTER();
    hif_mem_ring_pool_undo_init(&hif_mem_ring_pools[pool_nr-1]);
    HIF_EXIT();
#endif
}


/**
 * Deallocates the memory pointed to by \c ptr.
 * \c ptr has to be a pointer to a memory allocated by hif_mem_ring_alloc_from_pool().
 * If ptr is not a pointer to a previously allocated memory block, no action is taken.
 *
 * @param ptr Pointer to a memory block previously allocated by hif_mem_ring_alloc_from_pool()
 */
LSA_VOID hif_mem_ring_free_from_pool(LSA_VOID_PTR_TYPE ptr, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr)
{
    HIF_MEM_RING_POOL_PTR_TYPE ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) hif_mem_ring_pool_ptr;
    LSA_UINT32 size;
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    HIF_MEM_RING_BLOCK_DESC_EXTERNAL_PTR_TYPE block_desc_ext;
#else
    HIF_MEM_RING_BLOCK_DESC_PTR_TYPE block_desc; // block desc in allocated memory pool is only used if no external block_desc is stored
#endif
    
    HIF_ASSERT(ring_pool_data_ptr != LSA_NULL);
    
    if (ptr==LSA_NULL)
    {
        return;
    }
    
    // protect memory management data structures
    HIF_ENTER_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    // the block descriptor is stored in ring_pool_data_ptr
    block_desc_ext = &ring_pool_data_ptr->allocated_block_desc[ring_pool_data_ptr->next_free_block_desc_idx];
    
    // compare with the block descriptor stored in front of each block with globally stored descriptor
    HIF_ASSERT(block_desc_ext->allocated_block == (HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)(((LSA_UINT32)ptr)-HIF_MEM_RING_BLOCK_DESC_SIZE));
    
    size = block_desc_ext->size;
    
#else
    // the block descriptor is stored in front of each block
    block_desc=(HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)(((LSA_UINT32)ptr)-HIF_MEM_RING_BLOCK_DESC_SIZE);
    
    // ring puffer has to be cleaned in same order as it was allocated!
    HIF_ASSERT(ring_pool_data_ptr->alloc_begin == block_desc->prev_alloc_end);
    
    size = block_desc->size;
    
#endif
    
    if(hif_mem_ring_pool_check_overwrite(ptr, "hif_mem_ring_free_from_pool") == LSA_TRUE)
    {
        HIF_FATAL(0);
    }

    // alloc_end comes after alloc_begin (no pool_end overrun of alloc_end)
    if(ring_pool_data_ptr->alloc_begin < ring_pool_data_ptr->alloc_end)
    {
        ring_pool_data_ptr->pool_usage  -= size;
        ring_pool_data_ptr->alloc_begin += size;
    }
    // alloc_end comes before alloc_begin but current block_desc was alloced before pool_end overrun of alloc_end
    else if((LSA_UINT32)(ring_pool_data_ptr->pool_end - ring_pool_data_ptr->alloc_begin) >= size)
    {
        ring_pool_data_ptr->pool_usage  -= size;
        ring_pool_data_ptr->alloc_begin += size;
    }
    // the current block_desc did not fit at end of pool and caused pool_end overrun of alloc_end
    else
    {
        ring_pool_data_ptr->pool_usage  -= ((ring_pool_data_ptr->pool_end - ring_pool_data_ptr->alloc_begin) + size);
        ring_pool_data_ptr->alloc_begin = ring_pool_data_ptr->pool_begin + size;
    }
    
    ring_pool_data_ptr->free_calls++;
    
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    // decrement allocated block_desc_count and set next_free_block_desc to next element in ring list.
    ring_pool_data_ptr->allocated_block_desc_count--;
    ring_pool_data_ptr->next_free_block_desc_idx=(ring_pool_data_ptr->next_free_block_desc_idx + 1) % HIF_SHM_RB_BUFFER_LENGTH;
#endif
    
    HIF_EXIT_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
    return;
}

/**
 * Allocates a new memory block of the given size from ring pool.
 *
 *
 * @param size The size of the memory block to be allocated.
 * @return Upon successful completion a pointer to the allocated memory is returned.
 *    If \c size is 0, \c LSA_NULL is returned. If no memory is available, \c LSA_NULL is returned
 *    and #errno is set to #ENOMEM
 */
LSA_VOID_PTR_TYPE hif_mem_ring_alloc_from_pool(LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr)
{
    HIF_MEM_RING_BLOCK_DESC_PTR_TYPE suitable_block_desc; // holds the block that will fulfil the memory request
    LSA_UINT32 fit_size;
    LSA_UINT32 org_size=size;
    HIF_MEM_RING_POOL_PTR_TYPE ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) hif_mem_ring_pool_ptr;
    
    HIF_ASSERT(ring_pool_data_ptr != LSA_NULL);
    
    // It makes no sense to allocate memory of length 0
    if (size == 0)
    {
        return LSA_NULL;
    }
    
    // increase the block size in order to be able to store a pointer to the block descriptor and STOP/START checks
#if (defined HIF_CFG_MEM_RING_CHECK_OVERWRITE) && (HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_1)
    size+=HIF_MEM_RING_BLOCK_DESC_SIZE+sizeof(LSA_UINT32)*1; //STOP-Magickey
#elif (defined HIF_CFG_MEM_RING_CHECK_OVERWRITE) && (HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_4)
    size+=HIF_MEM_RING_BLOCK_DESC_SIZE+sizeof(LSA_UINT32)*4; //STOP-Magickey
#else /* no START STOP check */
    size+=HIF_MEM_RING_BLOCK_DESC_SIZE;
#endif

    if (org_size>size) // overflow?
    {
        errno = ENOMEM;
        return LSA_NULL;
    }
    
    // make sure the size fits the requested alignment
    if ((size % HIF_CFG_MEM_RING_POOL_ALIGN) != 0)
    {
        fit_size = (size + HIF_CFG_MEM_RING_POOL_ALIGN) - (size % HIF_CFG_MEM_RING_POOL_ALIGN);
        
        // detect overflow
        if (fit_size < size)
        {
            errno = ENOMEM;
            return LSA_NULL;
        }
        size = fit_size;
    }
    
    // searching / manipulating the memory management data structures has to be protected
    HIF_ENTER_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
    // find a block of which has at least the requested size
    suitable_block_desc=LSA_NULL;
    // alloc_end comes after alloc_begin (no pool_end overrun of alloc_end)
    if(ring_pool_data_ptr->alloc_begin <= ring_pool_data_ptr->alloc_end)
    {
        // is there enough space after alloc_end until pool_end?
        if((LSA_UINT32)(ring_pool_data_ptr->pool_end - ring_pool_data_ptr->alloc_end) >= size)
        {
            ring_pool_data_ptr->pool_usage += size;
            suitable_block_desc = (HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)ring_pool_data_ptr->alloc_end;
            suitable_block_desc->prev_alloc_end = ring_pool_data_ptr->alloc_end;
            ring_pool_data_ptr->alloc_end += size;
        }
        // is there enough space from pool_begin until alloc_begin?
        else if((LSA_UINT32)(ring_pool_data_ptr->alloc_begin - ring_pool_data_ptr->pool_begin) >= size)
        {
            // alloc_end overruns pool_end -> space after alloc_end until pool_end becomes waste
            ring_pool_data_ptr->pool_usage += ((ring_pool_data_ptr->pool_end - ring_pool_data_ptr->alloc_end)+size);
            suitable_block_desc = (HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)ring_pool_data_ptr->pool_begin;
            suitable_block_desc->prev_alloc_end = ring_pool_data_ptr->alloc_end;
            ring_pool_data_ptr->alloc_end = ring_pool_data_ptr->pool_begin + size;
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"hif_mem_ring_alloc_from_pool: Pool break around. Allocate memory at the beginning");
        }
    }
    // alloc_end comes before alloc_begin (pool_end overrun of alloc_end)
    else if((LSA_UINT32)(ring_pool_data_ptr->alloc_begin - ring_pool_data_ptr->alloc_end) >= size)
    {
        ring_pool_data_ptr->pool_usage += size;
        suitable_block_desc = (HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)ring_pool_data_ptr->alloc_end;
        suitable_block_desc->prev_alloc_end = ring_pool_data_ptr->alloc_end;
        ring_pool_data_ptr->alloc_end += size;
    }
    
    // no free memory was found
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    if (suitable_block_desc == LSA_NULL || ring_pool_data_ptr->allocated_block_desc_count >= HIF_SHM_RB_BUFFER_LENGTH)
    {
        HIF_EXIT_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
        
        //trace-point: "Error from alloc: No free memory was found."
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"Error from hif_mem_ring_alloc_from_pool: No free memory was found. Needed size is 0x%x bytes, allocated_block_desc_count is %d", size, ring_pool_data_ptr->allocated_block_desc_count);
        
        errno = ENOMEM;
        return LSA_NULL;
    }
#else
    if (suitable_block_desc == LSA_NULL)
    {
        HIF_EXIT_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
        
        //trace-point: "Error from alloc: No free memory was found."
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Error from hif_mem_ring_alloc_from_pool: No free memory was found. Needed size is 0x%x bytes", size);
        
        errno = ENOMEM;
        return LSA_NULL;
    }
#endif
    
    
    // the found suitable block will get the requested size
    suitable_block_desc->size = size;
    
#ifdef HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL
    // save block desc in ring_pool_data_ptr
    ring_pool_data_ptr->allocated_block_desc[ring_pool_data_ptr->next_alloc_block_desc_idx].allocated_block = suitable_block_desc;
    ring_pool_data_ptr->allocated_block_desc[ring_pool_data_ptr->next_alloc_block_desc_idx].size = size;
    
    // increment allocated block_desc_count and set next_alloc_block_desc to next element in ring list.
    ring_pool_data_ptr->allocated_block_desc_count++;
    ring_pool_data_ptr->next_alloc_block_desc_idx=(ring_pool_data_ptr->next_alloc_block_desc_idx + 1) % HIF_SHM_RB_BUFFER_LENGTH;
#endif
    
    //trace-point: "Allocates memory block at with size."
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "Allocates memory block at 0x%x with size %d.",((LSA_UINT32)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE),size);
    
    ring_pool_data_ptr->alloc_calls++;
    
    if (ring_pool_data_ptr->pool_usage>ring_pool_data_ptr->max_pool_usage)
    {
        ring_pool_data_ptr->max_pool_usage=ring_pool_data_ptr->pool_usage;
    }
    
#ifdef HIF_CFG_MEM_RING_CHECK_OVERWRITE
    // Save original size in block_desc for overwrite check
    suitable_block_desc->orig_size = org_size;
    // Write START and STOP sequences at the beginning of the memory
#if HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_1
    {   //HIF_MEM_RING_CHECK_START_STOP_1
        LSA_UINT32* tmp;

        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE-sizeof(LSA_UINT32)); /* four bytes before user memory starts */
        *tmp = HIF_SHM_CHECK_OVERWRITE_START_SEQ; 
        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE+org_size); /* That's the end of the allocated user memory */
	    *tmp = HIF_SHM_CHECK_OVERWRITE_STOP_SEQ; 
    }
#elif HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_4
    { //HIF_MEM_RING_CHECK_START_STOP_4
        LSA_UINT32* tmp;

        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE-sizeof(LSA_UINT32)*4);
	    *tmp = HIF_SHM_CHECK_OVERWRITE_START_SEQ; 
        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE-sizeof(LSA_UINT32)*3);
	    *tmp = HIF_SHM_CHECK_OVERWRITE_START_SEQ; 
	    tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE-sizeof(LSA_UINT32)*2);
	    *tmp = HIF_SHM_CHECK_OVERWRITE_START_SEQ; 
	    tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE-sizeof(LSA_UINT32));
	    *tmp = HIF_SHM_CHECK_OVERWRITE_START_SEQ; 

        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE+org_size); /* That's the end of the allocated user memory */
	    *tmp = HIF_SHM_CHECK_OVERWRITE_STOP_SEQ;
        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE+org_size+sizeof(LSA_UINT32));
	    *tmp = HIF_SHM_CHECK_OVERWRITE_STOP_SEQ;
        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE+org_size+sizeof(LSA_UINT32)*2);
	    *tmp = HIF_SHM_CHECK_OVERWRITE_STOP_SEQ;
        tmp = (LSA_UINT32*)((LSA_UINT8*)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE+org_size+sizeof(LSA_UINT32)*3);
	    *tmp = HIF_SHM_CHECK_OVERWRITE_STOP_SEQ;
    }
#endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE = HIF_MEM_RING_CHECK_START_STOP_4
#endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE

    // release the spinlock protecting the memory management data structures
    HIF_EXIT_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
    // return the block increased by the space needed for the pointer to the block descriptor
    return (LSA_VOID_PTR_TYPE)((LSA_UINT32)suitable_block_desc+HIF_MEM_RING_BLOCK_DESC_SIZE);
}


/**
 * Allocates a new memory block of the given size from hif ring pool.
 */
LSA_VOID_PTR_TYPE hif_mem_ring_pool_alloc(LSA_UINT32 size, LSA_INT pool)
{
    LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr = LSA_NULL;
#if (HIF_CFG_MEM_RING_POOL_MAX > 0)
    if(pool > 0 && pool <= HIF_CFG_MEM_RING_POOL_MAX)
    {
        hif_mem_ring_pool_ptr = &hif_mem_ring_pools[pool-1];
    }
#else
    LSA_UNUSED_ARG( pool );
#endif
    return hif_mem_ring_alloc_from_pool(size, hif_mem_ring_pool_ptr);
}
/**
 * Deallocates the memory pointed to by \c ptr from hif ring pool.
 */
LSA_VOID  hif_mem_ring_pool_free(LSA_VOID_PTR_TYPE ptr, LSA_INT pool)
{
    LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr = LSA_NULL;
#if (HIF_CFG_MEM_RING_POOL_MAX > 0)
    if(pool > 0 && pool <= HIF_CFG_MEM_RING_POOL_MAX)
    {
        hif_mem_ring_pool_ptr = &hif_mem_ring_pools[pool-1];
    }
#else
    LSA_UNUSED_ARG( pool );
#endif
    hif_mem_ring_free_from_pool(ptr, hif_mem_ring_pool_ptr);
}

/**
 * Stores in the location of \c pool_info the information about the
 * current state of the pool.
 * Information includes
 *  - the configured pool size in bytes
 *  - the current pool usage in bytes
 *  - the maximum pool usage so far in bytes
 *  - number of calls to hif_mem_ring_alloc_from_pool()
 *  - number of calls to hif_mem_ring_free_from_pool()
 *
 * @param pool_info pointer to #HIF_MEM_RING_POOL_INFO_TYPE where the current pool status is stored to
 *
 * @return 0 upon successful completion
 */
LSA_INT hif_mem_ring_pool_get_info(HIF_MEM_RING_POOL_INFO_PTR_TYPE pool_info, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr)
{    
    HIF_MEM_RING_POOL_PTR_TYPE ring_pool_data_ptr = (HIF_MEM_RING_POOL_PTR_TYPE) hif_mem_ring_pool_ptr;
    
    
    // lock pool in order to get consistent information
    HIF_ENTER_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
    pool_info->max_used        = ring_pool_data_ptr->max_pool_usage;
    pool_info->size            = ring_pool_data_ptr->configured_size;
    pool_info->used            = ring_pool_data_ptr->pool_usage;
    pool_info->alloc_calls     = ring_pool_data_ptr->alloc_calls;
    pool_info->free_calls      = ring_pool_data_ptr->free_calls;
    pool_info->pool_begin      = ring_pool_data_ptr->pool_begin;
    pool_info->pool_end        = ring_pool_data_ptr->pool_end;
    pool_info->alloc_begin     = ring_pool_data_ptr->alloc_begin;
    pool_info->alloc_end       = ring_pool_data_ptr->alloc_end;
    
    HIF_EXIT_REENTRANCE_LOCK(ring_pool_data_ptr->enter_exit_id);
    
    return 0;
}

/**
 * Checks for overwrites of START/STOP bytes written before/after allocated memory block
 * @param ptr to allocated memory block
 * @param func from wich overwrite check is called
 * @return LSA_TRUE upon successful overwrite detection, LSA_FALSE otherwise.
 */
LSA_BOOL hif_mem_ring_pool_check_overwrite(LSA_UINT8* ptr, const LSA_CHAR *func)
{
#ifdef HIF_CFG_MEM_RING_CHECK_OVERWRITE
    HIF_MEM_RING_BLOCK_DESC_PTR_TYPE block_desc = (HIF_MEM_RING_BLOCK_DESC_PTR_TYPE)(((LSA_UINT8*)ptr)-HIF_MEM_RING_BLOCK_DESC_SIZE);
    // Check START and STOP sequences at the beginning of the memory
#if HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_1
    {   //HIF_MEM_RING_CHECK_START_STOP_1
        volatile LSA_UINT32 *startval;
        volatile LSA_UINT32 *stopval;

        startval   = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE - sizeof(LSA_UINT32));
        stopval    = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE + block_desc->orig_size);
        
        HIF_PROGRAM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_mem_ring_pool_check_overwrite => Function(%s)", func);

        if((*startval != HIF_SHM_CHECK_OVERWRITE_START_SEQ /*STAR*/) || (*stopval != HIF_SHM_CHECK_OVERWRITE_STOP_SEQ /*STOP*/))
        {
            LSA_UINT8* i = 0;
            
            HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR,"hif_mem_ring_pool_check_overwrite(): startval(%08x), or stopval(%08x) don't match at ptr(0x%08x)",
                                    *startval,
                                    *stopval,
                                    ptr);

            for(i = (LSA_UINT8*)block_desc; i < (LSA_UINT8*)(block_desc + block_desc->orig_size + 4); i += 4)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"hif_mem_ring_pool_check_overwrite(): ptr(0x%08x), val(0x%08x)",
                                    i, *(LSA_UINT32*)i);
            }

            return LSA_TRUE;
        }
        else
        {
            HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH,"hif_mem_ring_pool_check_overwrite() is okay: startval(%08x) or stopval(%08x) at ptr(0x%08x)",
                                    *startval,
                                    *stopval,
                                    ptr);
            return LSA_FALSE;
        }
    }
#elif HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_4
    {   //HIF_MEM_RING_CHECK_START_STOP_4
        volatile LSA_UINT32 *startval1, *startval2, *startval3, *startval4;
        volatile LSA_UINT32 *stopval1, *stopval2, *stopval3, *stopval4;
    
         startval1   = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE - sizeof(LSA_UINT32)*4);
         startval2   = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE - sizeof(LSA_UINT32)*3);
         startval3   = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE - sizeof(LSA_UINT32)*2);
         startval4   = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE - sizeof(LSA_UINT32));
         stopval1    = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE + block_desc->orig_size);
         stopval2    = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE + block_desc->orig_size + sizeof(LSA_UINT32));
         stopval3    = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE + block_desc->orig_size + sizeof(LSA_UINT32)*2);
         stopval4    = (LSA_UINT32*)((LSA_UINT8*)block_desc + HIF_MEM_RING_BLOCK_DESC_SIZE + block_desc->orig_size + sizeof(LSA_UINT32)*3);
    
         HIF_PROGRAM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_mem_ring_pool_check_overwrite => Function(%s)", func);
    
         if((*startval1 != HIF_SHM_CHECK_OVERWRITE_START_SEQ /*STAR*/) || (*startval2 != HIF_SHM_CHECK_OVERWRITE_START_SEQ /*STAR*/) || (*startval3 != HIF_SHM_CHECK_OVERWRITE_START_SEQ /*STAR*/) || (*startval4 != HIF_SHM_CHECK_OVERWRITE_START_SEQ /*STAR*/) ||
             (*stopval1 != HIF_SHM_CHECK_OVERWRITE_STOP_SEQ /*STOP*/) || (*stopval2 != HIF_SHM_CHECK_OVERWRITE_STOP_SEQ /*STOP*/) || (*stopval3 != HIF_SHM_CHECK_OVERWRITE_STOP_SEQ /*STOP*/) || (*stopval4 != HIF_SHM_CHECK_OVERWRITE_STOP_SEQ /*STOP*/))
         {
             LSA_UINT8* i = 0;
             
             HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_ERROR,"hif_mem_ring_pool_check_overwrite(): startval1(%08x), startval2(%08x), startval3(%08x), startval4(%08x) or stopval1(%08x), stopval2(%08x), stopval3(%08x), stopval4(%08x) don't match at ptr(0x%08x)",
                                     *startval1, *startval2, *startval3, *startval4,
                                     *stopval1, *stopval2, *stopval3, *stopval4,
                                     ptr);
    
             for(i = (LSA_UINT8*)block_desc; i < (LSA_UINT8*)(block_desc + block_desc->orig_size + 4*4); i += 4)
             {
                 HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"hif_mem_ring_pool_check_overwrite(): ptr(0x%08x), val(0x%08x)",
                                     i, *(LSA_UINT32*)i);
             }
    
             return LSA_TRUE;
         }
         else
         {
             HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE_HIGH,"hif_mem_ring_pool_check_overwrite() is okay: startval1(%08x), startval2(%08x), startval3(%08x), startval4(%08x) or stopval1(%08x), stopval2(%08x), stopval3(%08x), stopval4(%08x) at ptr(0x%08x)",
                                     *startval1, *startval2, *startval3, *startval4,
                                     *stopval1, *stopval2, *stopval3, *stopval4,
                                     ptr);
             return LSA_FALSE;
         }
    }
#endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE == HIF_MEM_RING_CHECK_START_STOP_4
#else
    {
        LSA_UNUSED_ARG( ptr );
        LSA_UNUSED_ARG( func );
        return LSA_FALSE;
    }
#endif //HIF_CFG_MEM_RING_CHECK_OVERWRITE
}
