#ifndef EDDS_IOBUFFER_OUT_H___           /* ----- reinclude-protection ----- */
#define EDDS_IOBUFFER_OUT_H___

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for StdMac)    :C&  */
/*                                                                           */
/*  F i l e               &F: edds_iobufout.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO Buffer - system adaptation                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  01.07.12    TP    initial version.                                       */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_IOBUFFER_EXCHANGE
 *
 * function:       Atomic read of a pointer at a memory location and write 
 *                 a new one.
 *
 * parameters:     EDDS_EXCHANGE_TYPE*   pointer adress
 *                 EDDS_EXCHANGE_TYPE    value to be exchanged
 *
 * return value:   The function returns the initial value pointed to by 
 *                 pTarget.
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_EXCHANGE 
EDDS_EXCHANGE_TYPE EDDS_IOBUFFER_EXCHANGE( 
		   EDDS_EXCHANGE_TYPE volatile * pTarget,
		   EDDS_EXCHANGE_TYPE            Value
	   );
#endif

/*=============================================================================
 * function name:  EDDS_IOBUFFER_ADDR_EDDS_TO_USER
 *
 * function:       Determining the user address of a EDDS memory pointer.
 *
 * parameters:     LSA_UINT16                  management id for IOBuffer instance
 *                 EDD_UPPER_MEM_U8_PTR_TYPE   adress to be converted   
 *
 * return value:   Pointer in user address space.
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_ADDR_EDDS_TO_USER 
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBUFFER_ADDR_EDDS_TO_USER( 
           LSA_UINT16                  MngmId, 
		   EDD_UPPER_MEM_U8_PTR_TYPE   pAddr
	   );
#endif

/*=============================================================================
 * function name:  EDDS_IOBUFFER_MEM_ALLOC
 *
 * function:       Allocate a memory for usage by io buffer module.
 *
 * parameters:     LSA_VOID_PTR_TYPE *   pointer to memory
 *                 LSA_UINT16            size to be allocated
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_MEM_ALLOC 
LSA_VOID EDDS_IOBUFFER_MEM_ALLOC(
           LSA_VOID_PTR_TYPE * ppMem,
		   LSA_UINT32          Size 
	   );
#endif

/*=============================================================================
 * function name:  EDDS_IOBUFFER_MEM_FREE
 *
 * function:       Free memory previous allocated with EDDS_IOBUFFER_MEM_ALLOC.
 *
 * parameters:     LSA_VOID_PTR_TYPE   memory pointer to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_MEM_FREE 
LSA_VOID EDDS_IOBUFFER_MEM_FREE( 
	        LSA_UINT16        EDDS_LOCAL_MEM_ATTR * ret_val_ptr,
			LSA_VOID_PTR_TYPE                       pMem
	   );
#endif

/*=============================================================================
 * function name:  EDDS_IOBUFFER_MEMCPY
 *
 * function:       Copy buffer from source to destination.
 *
 * parameters:     LSA_VOID_PTR_TYPE   destination buffer
 *                 LSA_VOID_PTR_TYPE   source buffer
 *                 LSA_UINT32          length to be copied
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_MEMCPY 
LSA_VOID EDDS_IOBUFFER_MEMCPY( 
					LSA_VOID_PTR_TYPE pDst, 
					LSA_VOID_PTR_TYPE pSrc, 
					LSA_UINT32 Length 
	   );
#endif

/*=============================================================================
 * function name:  EDDS_IOBUFFER_FATAL_ERROR
 *
 * function:       Notify a fatal error.
 *
 * parameters:     LSA_UINT16                  length of error struct
 *                 EDDS_FATAL_ERROR_PTR_TYPE   error_ptr
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef EDDS_IOBUFFER_FATAL_ERROR 
LSA_VOID EDDS_IOBUFFER_FATAL_ERROR( 
					LSA_UINT16                 length,
					EDDS_FATAL_ERROR_PTR_TYPE  error_ptr 
	   );
#endif


/*****************************************************************************/
/*  end of file edds_iobuf_out.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_IOBUFFER_OUT_H___ */
