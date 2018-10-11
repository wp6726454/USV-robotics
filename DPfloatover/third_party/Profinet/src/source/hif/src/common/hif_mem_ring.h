#ifndef HIF_MEM_RING_H_
#define HIF_MEM_RING_H_

#include <stdint.h>
#include <stddef.h>

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
/*  F i l e               &F: hif_mem_ring.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Declarations for hif ring pool memory management                         */
/*                                                                           */
/*****************************************************************************/

/**
 * Structure used to store information about the current status of the ring pool.
 * 
 * @see #hif_mem_ring_pool_get_info 
 */
typedef struct hif_mem_ring_pool_info
{
    LSA_UINT8 *pool_begin;    ///< base adress of ring pool, marks beginning of ring pool
    LSA_UINT8 *pool_end;      ///< base adress of ring pool + size of ring pool, marks end of ring pool
    LSA_UINT8 *alloc_begin;   ///< marks the beginnig of the allocated/used area of the ring pool
    LSA_UINT8 *alloc_end;     ///< marks the end of the allocated/used area and also the beginning of the free area of the ring pool
    LSA_UINT32 size;          ///< configured size of the ring pool in bytes
    LSA_UINT32 used;          ///< current ring pool usage in bytes
    LSA_UINT32 max_used;      ///< "high water mark"; maximum ring pool usage so far in bytes
    LSA_UINT32 alloc_calls;   ///< number of successful calls to hif_mem_ring_alloc_from_pool()
    LSA_UINT32 free_calls;    ///< number of successful calls to hif_mem_ring_free_from_pool()
} HIF_MEM_RING_POOL_INFO_TYPE, *HIF_MEM_RING_POOL_INFO_PTR_TYPE;


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
extern LSA_INT           hif_mem_ring_pool_get_info(HIF_MEM_RING_POOL_INFO_PTR_TYPE pool_info, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr);


/**
 * Allocates a new memory block of the given size from ring pool.
 *
 *
 * @param size The size of the memory block to be allocated.
 * @return Upon successful completion a pointer to the allocated memory is returned.
 *    If \c size is 0, \c LSA_NULL is returned. If no memory is available, \c LSA_NULL is returned
 *    and #errno is set to #ENOMEM
 */
extern LSA_VOID_PTR_TYPE hif_mem_ring_alloc_from_pool(LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr);

/**
 * Deallocates the memory pointed to by \c ptr.
 * \c ptr has to be a pointer to a memory allocated by hif_mem_ring_alloc_from_pool().
 * If ptr is not a pointer to a previously allocated memory block, no action is taken.
 *
 * @param ptr Pointer to a memory block previously allocated by hif_mem_ring_alloc_from_pool()
 */
extern LSA_VOID          hif_mem_ring_free_from_pool(LSA_VOID_PTR_TYPE ptr, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr);

/**
 * Init and Undo Init hif ring pool.
 */
extern LSA_VOID_PTR_TYPE hif_mem_ring_pool_init(LSA_UINT8* pool_begin, LSA_UINT32 pool_size, LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr);
extern LSA_VOID          hif_mem_ring_pool_undo_init(LSA_VOID_PTR_TYPE hif_mem_ring_pool_ptr);

/**
 * Create and Delete hif ring pool.
 */
extern LSA_INT           hif_mem_ring_pool_create(LSA_UINT8* begin, LSA_UINT32 size);
extern LSA_VOID          hif_mem_ring_pool_delete(LSA_INT pool_nr);
/**
 * Allocates a new memory block of the given size from hif ring pool.
 */
extern LSA_VOID_PTR_TYPE hif_mem_ring_pool_alloc(LSA_UINT32 size, LSA_INT pool_nr);
/**
 * Deallocates the memory pointed to by \c ptr from hif ring pool.
 */
extern LSA_VOID          hif_mem_ring_pool_free(LSA_VOID_PTR_TYPE ptr, LSA_INT pool_nr);

/**
 * Checks for overwrites of START/STOP bytes written before/after allocated memory block
 */
extern LSA_UINT8 hif_mem_ring_pool_check_overwrite(LSA_UINT8* ptr, const LSA_CHAR *func);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_MEM_RING_H_ */
