#ifndef EDDS_IOBUFFER_USR_H___           /* ----- reinclude-protection ----- */
#define EDDS_IOBUFFER_USR_H___

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
/*  F i l e               &F: edds_iobuf_usr.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO Buffer - API                                  */
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

typedef LSA_VOID_PTR_TYPE   EDDS_IOBUFFER_HANDLE_TYPE;


/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/

/*===========================================================================*/
/*                            function prototypes                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_IOBuffer_Init
 *
 * function:       Initialize io buffer module.
 *
 * parameters:     pIOBufferManagement   management info block from srt init
 *                 MngmId                management id for IOBuffer instance
 *                 pConsumerCRCnt        amount of consumer
 *                 pProviderCRCnt        amount of provider
 *
 * return value:   EDDS_IOBUFFER_HANDLE_TYPE as handle for initialized 
 *                 io buffer module.
 *===========================================================================*/
EDDS_IOBUFFER_HANDLE_TYPE EDDS_IOBuffer_Init ( 
    EDD_COMMON_MEM_PTR_TYPE      hIOBufferManagement,   // [IN]
	LSA_UINT16                   MngmId,                // [IN]
    EDD_COMMON_MEM_U16_PTR_TYPE  pConsumerCRCnt,        // [OUT]
	EDD_COMMON_MEM_U16_PTR_TYPE  pProviderCRCnt         // [OUT]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_Deinit
 *
 * function:       Deinitialize IO buffer module.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_Deinit ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer   // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderAlloc
 *
 * function:       Allocates an io buffer provider cr.
 *
 * parameters:     hIOBuffer     user io buffer handle from EDDS_IOBuffer_Init
 *                 DataLen       length of IO data
 *                 ProviderType  type of provider: xRT or UDP
 *
 * return value:   cr number as reference to provider cr
 *===========================================================================*/
LSA_UINT16 EDDS_IOBuffer_ProviderAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,     // [IN]
	LSA_UINT16                 DataLen,       // [IN]
    LSA_UINT16                 ProviderType   // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderFree
 *
 * function:       Frees an io buffer provider cr.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    cr number as reference to cr to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ProviderFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);


/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderLockCurrent
 *
 * function:       Lock the current provider buffer for use by user.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    provider cr number
 *
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *===========================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockCurrent ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderLockNew
 *
 * function:       Lock a provider buffer for use by user. Content undefined.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    Provider CRNumber
 *
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *===========================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockNew ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ProviderUnlock
 *
 * function:       Unlock a previous locked provider buffer.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    provider cr number
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ProviderUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerAlloc
 *
 * function:       Allocates a io buffer consumer cr.
 *
 * parameters:     hIOBuffer     user io buffer handle from EDDS_IOBuffer_Init
 *                 DataLen       length of IO data
 *                 ConsumerType  type of consumer: xRT or UDP
 *
 * return value:   cr number as reference to consumer cr
 *===========================================================================*/
LSA_UINT16 EDDS_IOBuffer_ConsumerAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,     // [IN]
	LSA_UINT16                 DataLen,       // [IN]
	LSA_UINT16                 ConsumerType   // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerFree
 *
 * function:       Frees a io buffer consumer cr.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    cr number as reference to cr to be freed
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ConsumerFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerLock
 *
 * function:       Lock the current consumer buffer for use by user.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    consumer cr number
 *
 * return value:   EDD_UPPER_MEM_U8_PTR_TYPE as pointer to I/O buffer
 *===========================================================================*/
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ConsumerLock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*=============================================================================
 * function name:  EDDS_IOBuffer_ConsumerUnlock
 *
 * function:       Unlock a previous locked consumer buffer.
 *
 * parameters:     hIOBuffer   user io buffer handle from EDDS_IOBuffer_Init
 *                 CRNumber    consumer cr number
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID EDDS_IOBuffer_ConsumerUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CRNumber     // [IN]
);

/*****************************************************************************/
/*  end of file edds_iobuf_usr.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_IOBUFFER_USR_H___ */
