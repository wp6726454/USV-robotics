#ifndef EDDS_IOBUFFER_INT_H___           /* ----- reinclude-protection ----- */
#define EDDS_IOBUFFER_INT_H___

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
/*  F i l e               &F: edds_iobuf_int.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO Buffer - API internal structures, defines etc.*/
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

#include "edds_sys.h"       /* edds_srt_inc.h */
                            /* edds_nrt_inc.h */
	                        /* edds_dev.h     */

#include "edds_nrt_inc.h"   /* EDDS_LOCAL_HDB_COMP_NRT_PTR_TYPE */

#include "edds_srt_inc.h"   /* EDDS_CSRT_MGM_INFOBLOCK_TYPE */
	                        /* EDDS_CSRT_PROVIDER_CR_TYPE   */
                            /* EDDS_CSRT_CONSUMER_CR_TYPE   */
                            /* EDDS_CSRT_PROVIDER_TYPE_xxx  */
                            /* EDDS_CSRT_CONSUMER_TYPE_xxx  */

#include "edds_dev.h"       /* EDDS_FRAME_LEN_TYPE_WORD_OFFSET and  */
	                        /* EDDS_VLAN_TAG                        */

#include "edds_int.h"       /* LittleEndian to Host macros */

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#include "edds_pck.h"       /* header sizes for ip, udp etc. */
#endif


/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* Information like index and new flag is always stored in one byte in       */
/* the EXCHANGE variable at its base address. This applies to EDDS           */
/* and IOBuffer when accessing these variables. Access to this byte by       */
/* byte pointer does not need any consideration of endianess.                */
/*                                                                           */
/* example:                                                                  */
/* Value stored in memory for Xchange with reference to buffer 1 and         */
/* new flag is 0x8100. Index and new flag can be retrieved by accessing      */
/* Xchange with its address casted to a byte pointer independently from      */
/* host endianess.                                                           */
/*                                                                           */
/* index = (*((LSA_UINT8*)&Xchange)) & MASK                                  */
/*---------------------------------------------------------------------------*/
/*                   | NEW |     |  Idx  | <-- Meaning                       */
/*                   |  7  | ... |  1-0  | <-- Bit                           */
/* +---------+       +-----+-----+-------+                                   */
/* | User    |  -->  |  -  |     | 1/2/3 |                                   */
/* +---------+       +-----+-----+-------+                                   */
/* | Xchange |  -->  |  x  |     | 1/2/3 |                                   */
/* +---------+       +-----+-----+-------+                                   */
/* | EDDS    |  -->  |  -  |     | 1/2/3 |                                   */
/* +---------+       +-----+-----+-------+                                   */
/* | Latest  |  -->  |  -  |     | 1/2/3 | --> Provider only                 */
/* +---------+       +-----+-----+-------+                                   */
/*                                   |                                       */
/*                                   |                                       */
/* +----------+                      |                                       */
/* | Puffer 1 |   <------------------+                                       */
/* +----------+                      |                                       */
/* | Puffer 2 |   <------------------+                                       */
/* +----------+                      |                                       */
/* | Puffer 3 |   <------------------+                                       */
/* +----------+                                                              */
/*---------------------------------------------------------------------------*/

#define EDDS_IOBUFFER_MASK_INDEX          0x03
#define EDDS_IOBUFFER_MASK_NEW_FLAG       0x80


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct _EDDS_IOBUFFER_TYPE 
{	
	LSA_UINT16                       MngmId;

	LSA_UINT16                       ProviderCrCnt;
	LSA_UINT16                       ConsumerCrCnt;
	
	LSA_UINT16                       OffsetXrtWithVlan;
	LSA_UINT16                       OffsetXrtWithoutVlan;
#ifdef EDDS_XRT_OVER_UDP_INCLUDE	
	LSA_UINT16                       OffsetUdpWithVlan;
	LSA_UINT16                       OffsetUdpWithoutVlan;
#endif	
	
    EDDS_CSRT_PROVIDER_CR_TYPE_PTR   pProvBufferTable;        // address in shared RAM
    EDDS_CSRT_CONSUMER_CR_TYPE_PTR   pConsBufferTable;        // address in shared RAM
} EDDS_IOBUFFER_TYPE;

typedef struct _EDDS_IOBUFFER_TYPE   *   EDDS_IOBUFFER_PTR_TYPE;


/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

#define EDDS_IOBUFFER_HANDLE_IS_VALID(handle)   (0 != handle)

#define EDDS_IOBUFFER_MANAGEMENT_IS_VALID(handle)   (0 != handle)

#define EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE(handle) {                                  \
	if( !EDDS_IOBUFFER_HANDLE_IS_VALID(handle) )                                          \
	{                                                                                     \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_HANDLE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                     \
}

#define EDDS_IOBUFFER_VALIDATE_CR_NUMBER(cr_number,max_cr_cnt) {                             \
	if( cr_number < 1 || cr_number > max_cr_cnt )                                            \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_NUMBER, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#define EDDS_IOBUFFER_VALIDATE_PROVIDER_TYPE(type) {                                         \
	if( !( EDDS_CSRT_PROVIDER_TYPE_XRT == type ||                                            \
	       EDDS_CSRT_PROVIDER_TYPE_UDP == type ) )                                           \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_PROV_TYPE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}
#else
#define EDDS_IOBUFFER_VALIDATE_PROVIDER_TYPE(type) {                                         \
	if( !( EDDS_CSRT_PROVIDER_TYPE_XRT == type ) )                                           \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_PROV_TYPE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}
#endif

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#define EDDS_IOBUFFER_VALIDATE_CONSUMER_TYPE(type) {                                         \
	if( !( EDDS_CSRT_CONSUMER_TYPE_XRT == type ||                                            \
	       EDDS_CSRT_CONSUMER_TYPE_UDP == type ) )                                           \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CONS_TYPE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}
#else
#define EDDS_IOBUFFER_VALIDATE_CONSUMER_TYPE(type) {                                         \
	if( !( EDDS_CSRT_CONSUMER_TYPE_XRT == type ) )                                           \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CONS_TYPE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}
#endif

#define EDDS_IOBUFFER_VALIDATE_BUFFER_INDEX(index) {                                          \
	if( ( 0 > index ||                                                                        \
	      2 < index ) )                                                                       \
	{                                                                                         \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_BUFFER_IDX, EDDS_MODULE_ID, __LINE__); \
	}                                                                                         \
}

#define EDDS_IOBUFFER_VALIDATE_CR_USED(used) {                                                   \
	if( !used )                                                                                  \
	{                                                                                            \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_NOT_IN_USE, EDDS_MODULE_ID, __LINE__); \
	}                                                                                            \
}
	
#define EDDS_IOBUFFER_VALIDATE_CR_LOCKED(flag) {                                                 \
	if( !flag )                                                                                  \
	{                                                                                            \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_NOT_LOCKED, EDDS_MODULE_ID, __LINE__); \
	}                                                     	                                     \
}                                                                                                \

#define EDDS_IOBUFFER_VALIDATE_CR_UNLOCKED(flag) {                                           \
	if( flag )                                                                               \
	{                                                                                        \
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_LOCKED, EDDS_MODULE_ID, __LINE__); \
	}                                                                                        \
}


/*****************************************************************************/
/*  end of file edds_iobuf_int.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_IOBUFFER_INT_H___ */
