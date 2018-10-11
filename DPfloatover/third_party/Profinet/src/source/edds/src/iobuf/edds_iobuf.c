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
/*  F i l e               &F: edds_iobuf.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO Buffer - API implementation                   */
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

#define LTRC_ACT_MODUL_ID  11    /* EDDS_MODULE_ID_EDD_IO */
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_iobuf_inc.h"     /* io buffer header          */
#include "edds_iobuf_int.h"     /* io buffer internal header */

#ifdef EDDS_CFG_USE_IOBUFFER

/*===========================================================================*/
/*                               global Variables                            */
/*===========================================================================*/


/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

#define EDDS_IOBUF_FATAL_ERR_ALLOC         (EDDS_FATAL_ERROR_TYPE)  0x01
#define EDDS_IOBUF_FATAL_ERR_FREE          (EDDS_FATAL_ERROR_TYPE)  0x02
#define EDDS_IOBUF_FATAL_ERR_HANDLE        (EDDS_FATAL_ERROR_TYPE)  0x03
#define EDDS_IOBUF_FATAL_ERR_PROV_TYPE     (EDDS_FATAL_ERROR_TYPE)  0x04
#define EDDS_IOBUF_FATAL_ERR_CONS_TYPE     (EDDS_FATAL_ERROR_TYPE)  0x05
#define EDDS_IOBUF_FATAL_ERR_DATA_LEN      (EDDS_FATAL_ERROR_TYPE)  0x06
#define EDDS_IOBUF_FATAL_ERR_CR_NUMBER     (EDDS_FATAL_ERROR_TYPE)  0x07
#define EDDS_IOBUF_FATAL_ERR_BUFFER_IDX    (EDDS_FATAL_ERROR_TYPE)  0x08
#define EDDS_IOBUF_FATAL_ERR_CR_NOT_IN_USE (EDDS_FATAL_ERROR_TYPE)  0x09
#define EDDS_IOBUF_FATAL_ERR_CR_IN_USE     (EDDS_FATAL_ERROR_TYPE)  0x0A
#define EDDS_IOBUF_FATAL_ERR_CR_NOT_LOCKED (EDDS_FATAL_ERROR_TYPE)  0x0B
#define EDDS_IOBUF_FATAL_ERR_CR_LOCKED     (EDDS_FATAL_ERROR_TYPE)  0x0C


/*===========================================================================*/
/*                              external functions                           */
/*===========================================================================*/

/*===========================================================================*/
/*                           internal helper functions                       */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_IOBuffer_FatalError                    +*/
/*+  Input/Output               EDDS_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16           ModuleID               +*/
/*+                             LSA_UINT16           Line                   +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : EDDS-fatal-errorcode                            +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via EDDS_IOBUFFER_FATAL_ERROR macro.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC EDDS_FAR_FCT LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_IOBuffer_FatalError (
    EDDS_FATAL_ERROR_TYPE  Error,      // [IN]
    LSA_UINT16             ModuleID,   // [IN]
    LSA_UINT32             Line )      // [IN]
{
    LSA_FATAL_ERROR_TYPE LSAError;

    LSAError.lsa_component_id  = LSA_COMP_ID_EDDS;
    LSAError.module_id         = ModuleID;
    LSAError.line              = (LSA_UINT16) Line;
    LSAError.error_code[0]     = (LSA_UINT32) Error;
    LSAError.error_code[1]     = 0;
    LSAError.error_code[2]     = 0;
    LSAError.error_code[3]     = 0;
    LSAError.error_data_length = 0;
    LSAError.error_data_ptr    = LSA_NULL; /* currently no message */

    EDDS_IOBUFFER_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
}


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_Init                                 +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDD_COMMON_MEM_PTR_TYPE       hIOBufferManagement  +*/
/*+                      LSA_UINT16                    MngmId               +*/
/*+                 :    EDD_COMMON_MEM_U16_PTR_TYPE   pConsumerCRCnt       +*/
/*+                 :    EDD_COMMON_MEM_U16_PTR_TYPE   pProviderCRCnt       +*/
/*+                                                                         +*/
/*+  Result         :    EDDS_IOBUFFER_HANDLE_TYPE                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBufferManagement   :  Pointer to IOBufferManagement returned on     +*/
/*+                           initialization of edds srt component          +*/ 
/*+  MngmId                :  system id of IOBuffer instance                +*/
/*+  pConsumerCRCnt        :  Pointer to number of configured consumer cr's +*/
/*+  pProviderCRCnt        :  Pointer to number of configured provider cr's +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Handle for initialized io buffer module.                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize io buffer module for usage. On error           +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf,pConsumerCRCnt,pProviderCRCnt)
//JB 12/11/2014 IOBuf is set in this function (and allocated within a macro)
//pConsumerCRCnt and pProviderCRCnt will only be set (out params)
EDDS_IOBUFFER_HANDLE_TYPE EDDS_IOBuffer_Init ( 
    EDD_COMMON_MEM_PTR_TYPE      hIOBufferManagement,   // [IN]
	LSA_UINT16                   MngmId,                // [IN]
    EDD_COMMON_MEM_U16_PTR_TYPE  pConsumerCRCnt,        // [OUT]
	EDD_COMMON_MEM_U16_PTR_TYPE  pProviderCRCnt )       // [OUT]
{
	EDDS_IOBUFFER_PTR_TYPE           pIOBuf;
	EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR pIOBufMgmInfoBlock;
	
	// allocate memory for instance persistence
	EDDS_IOBUFFER_MEM_ALLOC( (LSA_VOID**)&pIOBuf, sizeof(EDDS_IOBUFFER_TYPE) );
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	// check memory allocation for persistence
	if( !EDDS_IOBUFFER_HANDLE_IS_VALID(pIOBuf) )
	{
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_ALLOC, EDDS_MODULE_ID, __LINE__);
	}
	
	// check for valid management info block
	if( !EDDS_IOBUFFER_MANAGEMENT_IS_VALID( hIOBufferManagement ) )
	{
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_HANDLE, EDDS_MODULE_ID, __LINE__);
	}	
	
	
	/*-----------------------------------------------------------------------*/
	/* Set up instance persistence.                                          */
	/*                                                                       */
	/* NOTE:                                                                 */
	/* IOBuffer awaits LittleEndian. Therefore conversion from LittleEndian  */
	/* to Host endianess is necessary.                                       */
	/*                                                                       */
	/* Host endianess is defined in edds_cfg.h. Regard one core or multi     */
	/* core solution. On a multi core solution edds_cfg.h has to be provided */
	/* for that core on which IOBuffer is running   .                        */
	/*-----------------------------------------------------------------------*/

	pIOBuf->MngmId = MngmId;   // save management id to this IOBuffer instance

	// get management info block in user space
	pIOBufMgmInfoBlock = (EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR) EDDS_IOBUFFER_ADDR_EDDS_TO_USER( 
		                                                    pIOBuf->MngmId, 
							                                (EDD_UPPER_MEM_U8_PTR_TYPE)hIOBufferManagement );	//lint !e826 JB 12/11/2014 cast on purpose
    //@fixme adpath2 NEA6.0 eventually change ret val of EDDS_IOBUFFER_ADDR_EDDS_TO_USER (void*) to avoid e826 - review similar lint comments
	
	// save provider and consumer Cr count --> endianess
	*pProviderCRCnt = pIOBuf->ProviderCrCnt = LE_TO_H_S(pIOBufMgmInfoBlock->ProviderCrCnt);
	*pConsumerCRCnt = pIOBuf->ConsumerCrCnt = LE_TO_H_S(pIOBufMgmInfoBlock->ConsumerCrCnt);

	// set provider and consumer Cr table
	pIOBuf->pProvBufferTable = (EDDS_CSRT_PROVIDER_CR_TYPE_PTR)EDDS_IOBUFFER_ADDR_EDDS_TO_USER( 
		                                                       pIOBuf->MngmId,
															   (EDD_UPPER_MEM_U8_PTR_TYPE)pIOBufMgmInfoBlock->pProviderCrTable ); //lint !e826 JB 12/11/2014 cast on purpose
    //@fixme adpath2 NEA6.0 eventually change ret val of EDDS_IOBUFFER_ADDR_EDDS_TO_USER (void*) to avoid e826
	pIOBuf->pConsBufferTable = (EDDS_CSRT_CONSUMER_CR_TYPE_PTR)EDDS_IOBUFFER_ADDR_EDDS_TO_USER( 
		                                                       pIOBuf->MngmId,  
															   (EDD_UPPER_MEM_U8_PTR_TYPE)pIOBufMgmInfoBlock->pConsumerCrTable );	//lint !e826 JB 12/11/2014 cast on purpose
    //@fixme adpath2 NEA6.0 eventually change ret val of EDDS_IOBUFFER_ADDR_EDDS_TO_USER (void*) to avoid e826

								  
	/*-----------------------------------------------------------------------*/
	/* calculate io data buffer offsets                                      */
	/*-----------------------------------------------------------------------*/
	
	/*-----------------------------------------------------------------------*/
	/* Frameformat for cyclic xRT:                                           */
	/*                                                                       */
	/* 0  6  12        14     16/12     18/14   20/16                        */
	/* +--+--+---------+------+---------+-------+--...--+------+             */
	/* |DA|SA| TypeLen | VLAN | TypeLen | Frame | I/O-  | APDU |             */
	/* |  |  | VLAN    | ID   | xRT     |  ID   | Data  |      |             */
	/* +--+--+---------+------+---------+-------+--...--+------+             */
	/*       |    optional    |                                              */
	/*                                                                       */
	/*-----------------------------------------------------------------------*/
	
	pIOBuf->OffsetXrtWithoutVlan = EDDS_CSRT_FRAME_IO_START_WITHOUT_VLAN;
	pIOBuf->OffsetXrtWithVlan    = EDDS_CSRT_FRAME_IO_START_WITH_VLAN;
	
	#ifdef EDDS_XRT_OVER_UDP_INCLUDE
	/*-----------------------------------------------------------------------*/
	/* Frameformat cyclic xRT over UDP:                                      */
	/*                                                                       */
	/* 0  6  12        14     16/12  18/14    38/34    46/42   48/44         */
	/* +--+--+---------+------+------+--------+--------+-------+--...--+...  */
	/* |DA|SA| TypeLen | VLAN |  IP  |   IP   |   UDP  | Frame | I/O-  |     */
	/* |  |  | VLAN    | ID   | Type | Header | Header |   ID  | Data  |     */
	/* +--+--+---------+------+------+--------+--------+-------+--...--+...  */
	/*       |    optional    |                                              */
	/*-----------------------------------------------------------------------*/	
	
	pIOBuf->OffsetUdpWithoutVlan = EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN + 
						           sizeof(EDDS_IP_HEADER_TYPE)         +
						           sizeof(EDDS_UDP_HEADER_TYPE)        + 
						           sizeof(EDDS_FRAME_ID_TYPE);			
	pIOBuf->OffsetUdpWithVlan    = pIOBuf->OffsetUdpWithoutVlan + 
	                               EDD_VLAN_TAG_SIZE;
	#endif
		
	return( (EDDS_IOBUFFER_HANDLE_TYPE) pIOBuf );
}
//lint +esym(613,pIOBuf,pConsumerCRCnt,pProviderCRCnt)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_Deinit                               +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                                                                         +*/
/*+  Result         :    LSA_VOID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+                                                                         +*/
/*+  Results    :  LSA_VOID                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Deinitialize io buffer module. On error                   +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_VOID EDDS_IOBuffer_Deinit ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer )   // [IN]
{
	LSA_UINT16 idx;
	LSA_UINT16 result;
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pProviderCr;
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR pConsumerCr;
	EDDS_IOBUFFER_PTR_TYPE pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );
	
	pProviderCr = pIOBuf->pProvBufferTable;
	pConsumerCr = pIOBuf->pConsBufferTable;
	

	/*-----------------------------------------------------------------------*/
	/* Check for still allocated provider Cr's.                              */
	/*-----------------------------------------------------------------------*/
	
	for( idx = 0 ; idx < pIOBuf->ProviderCrCnt ; idx++ )
	{
		if( pProviderCr->CrUsed )
		{
			EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_IN_USE, EDDS_MODULE_ID, __LINE__);
		}
		pProviderCr++;
	}


	/*-----------------------------------------------------------------------*/
	/* Check for still allocated consumer Cr's.                              */
	/*-----------------------------------------------------------------------*/

	for( idx = 0 ; idx < pIOBuf->ConsumerCrCnt ; idx++ )
	{
		if( pConsumerCr->CrUsed )
		{
		    EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_CR_IN_USE, EDDS_MODULE_ID, __LINE__);
		}
		pConsumerCr++;
	}
	

	/*-----------------------------------------------------------------------*/
	/* Free persistence memory.                                              */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_MEM_FREE( &result, pIOBuf );

	if( EDD_STS_OK != result )
	{
		EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_FREE, EDDS_MODULE_ID, __LINE__);
	}
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ProviderAlloc                        +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  DataLen                +*/
/*+                      LSA_UINT16                  ProviderType           +*/
/*+                                                                         +*/
/*+  Result         :    LSA_UINT16                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer    :  io buffer handle from a previous call to               +*/
/*+                  EDDS_IOBuffer_Init                                     +*/
/*+  DataLen      :  length of IO data                                      +*/
/*+  ProviderType :  type of provider (xRT or UDP)                          +*/
/*+                     - EDDS_CSRT_PROVIDER_TYPE_XRT                       +*/
/*+                     - EDDS_CSRT_PROVIDER_TYPE_UDP                       +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Provider Cr number.                                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an io buffer provider Cr. If no more Cr's are   +*/
/*                present EDD_CRNUMBER_UNDEFINED is returned.               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_UINT16 EDDS_IOBuffer_ProviderAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,       // [IN]
	LSA_UINT16                 DataLen,         // [IN]
    LSA_UINT16                 ProviderType )   // [IN]
{	
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;
	LSA_UINT16                     idx;
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	LSA_UINT16                     CrNumber = EDD_CRNUMBER_UNDEFINED;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );       // check handle
	EDDS_IOBUFFER_VALIDATE_PROVIDER_TYPE( ProviderType );   // check provider type: xRT or UDP
	
	#ifdef EDDS_XRT_OVER_UDP_INCLUDE
	if( EDDS_CSRT_PROVIDER_TYPE_UDP == ProviderType )
	{
		if (( DataLen  < EDD_CSRT_UDP_DATALEN_MIN ) ||
			( DataLen  > EDD_CSRT_UDP_DATALEN_MAX ))
		{
			EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_DATA_LEN, EDDS_MODULE_ID, __LINE__);
		}
	}
	else
	#endif
	{
		if (( DataLen  < EDD_CSRT_DATALEN_MIN ) ||
			( DataLen  > EDD_CSRT_DATALEN_MAX ))
		{
			EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_DATA_LEN, EDDS_MODULE_ID, __LINE__);
		}
	}

	// get first provider Cr entry in table
	pCr = pIOBuf->pProvBufferTable;
	
	
	/*-----------------------------------------------------------------------*/
	/* Lookup for first free Cr entry and reserve it. Return its index.      */
	/* Return EDD_CRNUMBER_UNDEFINED when no free Cr exists.                 */
	/*-----------------------------------------------------------------------*/
		
	// Cr number valid in range 1..ProviderCnt
	for( idx = 1 ; idx <= pIOBuf->ProviderCrCnt ; idx++ )
	{
		// Cr used?
		if( !pCr->CrUsed )
		{
		    pCr->CrUsed  = LSA_TRUE;
			pCr->Type    = (LSA_UINT8)ProviderType;
			pCr->DataLen = H_TO_LE_S(DataLen);   // exchange over little endian
			                                     // use LE_TO_H_S in EDDS
			CrNumber = idx;
			break;
		}
		pCr++;
	}	
	
	return CrNumber;	
}
//lint +esym(613,pIOBuf)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ProviderFree                         +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    LSA_VOID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  provider Cr number                                       +*/
/*+                                                                         +*/
/*+  Results    :  LSA_VOID                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees an io buffer provider Cr. On error                  +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_VOID EDDS_IOBuffer_ProviderFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;
	
	
	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ProviderCrCnt );   // check Cr range

	// get provider Cr
	pCr = &pIOBuf->pProvBufferTable[CrNumber - 1];   // CrNumber --> 1..ProviderCrCnt
	
	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );         // Cr must be used
	EDDS_IOBUFFER_VALIDATE_CR_UNLOCKED( pCr->IsLocked );   // Cr must not be locked
	
	
	/*-----------------------------------------------------------------------*/
	/* Free provider Cr.                                                     */
	/*-----------------------------------------------------------------------*/
	
	pCr->CrUsed = LSA_FALSE;
	pCr->Type   = EDDS_CSRT_PROVIDER_TYPE_NOT_USED;
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ProviderLockNew                      +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    EDD_UPPER_MEM_U8_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  provider Cr number                                       +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Pointer to the start of i/o buffer of a provider.                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Lock a provider buffer for usage. The content of the user +*/
/*+               buffer is undefined.                                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockNew ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]											  
{	
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf      = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	EDD_UPPER_MEM_U8_PTR_TYPE      pUserBuffer = LSA_NULL;
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;	
	LSA_UINT32                     UserIdx;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ProviderCrCnt );   // check Cr range
		
	// get provider Cr
	pCr = &pIOBuf->pProvBufferTable[CrNumber - 1];   // CrNumber --> 1..ProviderCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );  // Cr must be in use
		
	
	/*-----------------------------------------------------------------------*/
	/* Lock this provider. Retrieve user buffer and convert it's address     */
	/* from EDDS to user space.                                              */
	/*                                                                       */
	/* NOTE:                                                                 */
	/* Information like index and new flag is always stored in one byte in   */
	/* the EXCHANGE variable at its base address. This applies to EDDS       */
	/* and IOBuffer when accessing these variables. Access to this byte by   */
	/* byte pointer does not need any consideration of endianess.            */
	/*                                                                       */
	/* example:                                                              */
	/* Value stored in memory for Xchange with reference to buffer 1 and     */
	/* new flag is 0x8100. Index and new flag can be retrieved by accessing  */
	/* Xchange with its address casted to a byte pointer independently from  */
	/* host endianess.                                                       */
	/*                                                                       */
	/* index = (*((LSA_UINT8*)&Xchange)) & MASK                              */
	/*-----------------------------------------------------------------------*/
	
	// is provider not locked?
	if( !pCr->IsLocked )
	{		
		// no special locking mechanism needed; call sequence to lock/unlock a provider
		// has to be protected by system adaption from being locked from another 
		// context; just set locked flag
		pCr->IsLocked = LSA_TRUE;
		
		// get user index
		UserIdx = (*((LSA_UINT8*)&pCr->User)) & EDDS_IOBUFFER_MASK_INDEX;

		// check buffer index (0..2) 
		EDDS_IOBUFFER_VALIDATE_BUFFER_INDEX( UserIdx ); //lint !e685 !e568 JB 12/11/2014 (define specific, macro specific)
		
		//convert buffer pointer from EDDS space to user space	
		pUserBuffer = EDDS_IOBUFFER_ADDR_EDDS_TO_USER( pIOBuf->MngmId, (EDD_UPPER_MEM_U8_PTR_TYPE) pCr->pBuffer[UserIdx] );  //lint !e661 JB 12/11/2014 fatal before if out of bounds
		

		/*-----------------------------------------------------------------------*/
		/* Set offset to I/O data area in buffer depending on type of provider.  */
		/*                                                                       */
		/* NOTE: provider frame always sent with VLAN-Tag                        */
		/*-----------------------------------------------------------------------*/

		#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		if( EDDS_CSRT_PROVIDER_TYPE_UDP == pCr->Type )
			pUserBuffer += pIOBuf->OffsetUdpWithVlan;
		else
		#endif
			pUserBuffer += pIOBuf->OffsetXrtWithVlan;
	}
	
	return pUserBuffer;	
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ProviderLockCurrent                  +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    EDD_UPPER_MEM_U8_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  provider Cr number                                       +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Pointer to the start of i/o buffer of a provider.                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Lock a provider buffer for usage. The user buffer         +*/
/*+               returned contains the most recent i/o data.               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ProviderLockCurrent ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{	
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;	
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;
	LSA_UINT32                     UserIdx;
	LSA_UINT32                     LatestIdx;
	EDD_UPPER_MEM_U8_PTR_TYPE      pUserBuffer = LSA_NULL;
	EDD_UPPER_MEM_U8_PTR_TYPE      pSrcBuffer;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ProviderCrCnt );   // check Cr range
	
	// get provider Cr
	pCr = &pIOBuf->pProvBufferTable[CrNumber - 1];   // CrNumber --> 1..ProviderCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );  // Cr must be in use
	
	
	/*-----------------------------------------------------------------------*/
	/* Lock this provider. Retrieve user buffer and the buffer that is       */
	/* referenced in latest. This buffer contains the most recent data from  */
	/* last unlock. Convert addresses from EDDS to user space. Copy data to  */
	/* user buffer.                                                          */
	/*-----------------------------------------------------------------------*/	
	
	// provider not locked?
	if( !pCr->IsLocked )
	{
		LSA_UINT16 DataLen = LE_TO_H_S(pCr->DataLen);

		// no special locking mechanism needed; call sequence to lock/unlock a provider
		// has to be protected by system adaption from being locked from another  
		// context; just set locked flag
		pCr->IsLocked = LSA_TRUE;
		
		// get user and latest index
		UserIdx   = (*((LSA_UINT8*)&pCr->User))   & EDDS_IOBUFFER_MASK_INDEX;
		LatestIdx = (*((LSA_UINT8*)&pCr->Latest)) & EDDS_IOBUFFER_MASK_INDEX;
		
		// check buffer index (0..2)
		EDDS_IOBUFFER_VALIDATE_BUFFER_INDEX( UserIdx );    //lint !e685 !e568 JB 12/11/2014 (define specific, macro specific)
		EDDS_IOBUFFER_VALIDATE_BUFFER_INDEX( LatestIdx );  //lint !e685 !e568 JB 12/11/2014 (define specific, macro specific)
		
		pUserBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE) pCr->pBuffer[UserIdx];	//lint !e661 JB 12/11/2014 fatal before if out of bounds
		pSrcBuffer  = (EDD_UPPER_MEM_U8_PTR_TYPE) pCr->pBuffer[LatestIdx]; //lint !e661 JB 12/11/2014 fatal before if out of bounds
		
		//convert buffer pointer from EDDS space to user space
		pUserBuffer = EDDS_IOBUFFER_ADDR_EDDS_TO_USER( pIOBuf->MngmId, pUserBuffer );
		pSrcBuffer  = EDDS_IOBUFFER_ADDR_EDDS_TO_USER( pIOBuf->MngmId, pSrcBuffer );


		/*-----------------------------------------------------------------------*/
		/* Set offset to I/O data area in buffer depending on type of provider.  */
		/*                                                                       */
		/* NOTE: provider frame always sent with VLAN-Tag                        */
		/*-----------------------------------------------------------------------*/

		#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		if( EDDS_CSRT_PROVIDER_TYPE_UDP == pCr->Type )
		{
			pUserBuffer += pIOBuf->OffsetUdpWithVlan;
			pSrcBuffer  += pIOBuf->OffsetUdpWithVlan;
		}
		else
		#endif
		{
			pUserBuffer += pIOBuf->OffsetXrtWithVlan;
			pSrcBuffer  += pIOBuf->OffsetXrtWithVlan;
		}
		
		// copy latest buffer to user buffer with length given on alloc
		EDDS_IOBUFFER_MEMCPY( pUserBuffer, pSrcBuffer, DataLen );
	}
	
	return pUserBuffer;	
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ProviderUnlock                       +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    LSA_VOID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  provider Cr number                                       +*/
/*+                                                                         +*/
/*+  Results    :  LSA_VOID                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Unlock a previous locked provider buffer. On error        +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_VOID EDDS_IOBuffer_ProviderUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{	
	LSA_UINT8*                     pValue;
	EDDS_EXCHANGE_TYPE             Temp;
	EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ProviderCrCnt );
	
	// get provider Cr
	pCr = &pIOBuf->pProvBufferTable[CrNumber - 1];   // CrNumber --> 1..ProviderCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );       // Cr must be in use
	EDDS_IOBUFFER_VALIDATE_CR_LOCKED( pCr->IsLocked );   // Cr must be locked
	

	/*-----------------------------------------------------------------------*/
	/* Save reference to user buffer in latest. So that at any time the      */
	/* reference to the latest buffer can be retrieved. Do this first        */
	/* because the user entry is exchanged afterwards.                       */
	/* Mask user entry with new flag and exchange it. On a new provider send */
	/* cycle this buffer will be send. Finally unlock this provider.         */
	/*-----------------------------------------------------------------------*/

	// save reference to most current buffer in latest
	pCr->Latest = pCr->User;   // copy user entry to latest
	Temp        = pCr->User;   // copy user entry to Temp for exchange

	// set new flag in Temp
	pValue   = ((LSA_UINT8*)&Temp);
	*pValue |= EDDS_IOBUFFER_MASK_NEW_FLAG;

	pCr->User = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );

	// reset new flag in User
	pValue   = ((LSA_UINT8*)&pCr->User);
	*pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
	
	// unlock provider
	pCr->IsLocked = LSA_FALSE;
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ConsumerAlloc                        +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  DataLen                +*/
/*+                      LSA_UINT16                  ConsumerType           +*/
/*+                                                                         +*/
/*+  Result         :    LSA_UINT16                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer    :  io buffer handle from a previous call to               +*/
/*+                  EDDS_IOBuffer_Init                                     +*/
/*+  DataLen      :  length of IO data                                      +*/
/*+  ConsumerType :  type of consumer (xRT or UDP)                          +*/
/*+                     - EDDS_CSRT_CONSUMER_TYPE_XRT                       +*/
/*+                     - EDDS_CSRT_CONSUMER_TYPE_UDP                       +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Consumer Cr number.                                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an io buffer consumer Cr. If no more Cr's are   +*/
/*                present EDD_CRNUMBER_UNDEFINED is returned.                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_UINT16 EDDS_IOBuffer_ConsumerAlloc ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,       // [IN]
	LSA_UINT16                 DataLen,         // [IN]
    LSA_UINT16                 ConsumerType )   // [IN]
{
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
	LSA_UINT16                     idx;
	LSA_UINT16                     CrNumber = EDD_CRNUMBER_UNDEFINED;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );       // check handle
	EDDS_IOBUFFER_VALIDATE_CONSUMER_TYPE( ConsumerType );   // check consumer type: xRT or UDP
		
	#ifdef EDDS_XRT_OVER_UDP_INCLUDE
	if( EDDS_CSRT_CONSUMER_TYPE_UDP == ProviderType )
	{
		if(( DataLen  < EDD_CSRT_UDP_DATALEN_MIN ) ||
		   ( DataLen  > EDD_CSRT_UDP_DATALEN_MAX ))
		{
			EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_DATA_LEN, EDDS_MODULE_ID, __LINE__);
		}
	}
	else
	#endif
	{
		if(( DataLen  < EDD_CSRT_DATALEN_MIN ) ||
		   ( DataLen  > EDD_CSRT_DATALEN_MAX ))
		{
			EDDS_IOBuffer_FatalError( EDDS_IOBUF_FATAL_ERR_DATA_LEN, EDDS_MODULE_ID, __LINE__);
		}
	}

	// get first consumer Cr entry in table
	pCr = pIOBuf->pConsBufferTable;
	
	/*-----------------------------------------------------------------------*/
	/* Lookup for free Cr entry and reserve it. Return its index. Return     */
	/* EDD_CRNUMBER_UNDEFINED when no free Cr exists.                        */
	/*-----------------------------------------------------------------------*/
		
	// Cr number valid in range 1..ConsumerCrCnt
	for( idx = 1 ; idx <= pIOBuf->ConsumerCrCnt ; idx++ )
	{
		if( !pCr->CrUsed )
		{
		    pCr->CrUsed  = LSA_TRUE;
			pCr->Type    = (LSA_UINT8)ConsumerType;
			pCr->DataLen = H_TO_LE_S(DataLen);   // exchange over little endian
			                                     // use LE_TO_H_S in EDDS
			CrNumber = idx;
			break;
		}
		pCr++;
	}	
	
	return CrNumber;	
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ConsumerFree                         +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    LSA_VOID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  consumer Cr number                                       +*/
/*+                                                                         +*/
/*+  Results    :  LSA_VOID                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees an io buffer consumer Cr. On error                  +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_VOID EDDS_IOBuffer_ConsumerFree ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ConsumerCrCnt );   // check Cr range
		
	// get consumer Cr
	pCr = &pIOBuf->pConsBufferTable[CrNumber - 1];   // CrNumber --> 1..ConsumerCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );         // Cr must be in use
	EDDS_IOBUFFER_VALIDATE_CR_UNLOCKED( pCr->IsLocked );   // Cr must be unlocked
		

	/*-----------------------------------------------------------------------*/
	/* Free consumer Cr.                                                     */
	/*-----------------------------------------------------------------------*/		
	
	pCr->CrUsed = LSA_FALSE;
	pCr->Type   = EDDS_CSRT_CONSUMER_TYPE_NOT_USED;
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ConsumerLock                         +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    EDD_UPPER_MEM_U8_PTR_TYPE                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  consumer Cr number                                       +*/
/*+                                                                         +*/
/*+  Results:                                                               +*/
/*+  Pointer to the start of i/o buffer of a consumer.                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Lock a consumer buffer for usage. The content of the      +*/
/*+               user buffer is undefined till the activation of the       +*/
/*+               consumer.                                                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
EDD_UPPER_MEM_U8_PTR_TYPE EDDS_IOBuffer_ConsumerLock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{
	LSA_UINT8*                     pValue;
	EDDS_EXCHANGE_TYPE             Temp;
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;	
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
	LSA_UINT32                     UserIdx;
	EDD_UPPER_MEM_U8_PTR_TYPE      pUserBuffer = LSA_NULL;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ConsumerCrCnt );   // check Cr range
		
	// get consumer Cr
	pCr = &pIOBuf->pConsBufferTable[CrNumber - 1];   // CrNumber --> 1..ConsumerCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );  // Cr must be in use
	
	
	/*-----------------------------------------------------------------------*/
	/* Lock this consumer. Check whether new data exists. In that case       */
	/* exchange Xchange with user entry. If no newer data exist use old User */
	/* entry.                                                                */
	/*-----------------------------------------------------------------------*/
	
	// is consumer not locked?
	if( !pCr->IsLocked )
	{
		// no special locking mechanism needed; call sequence to lock/unlock a consumer
		// has to be protected by system adaption from being locked from another 
		// context; just set locked flag
		pCr->IsLocked = LSA_TRUE;
		
		Temp = pCr->XChange;
		
		// new buffer present in XChange, use it
		if( (*((LSA_UINT8*)&Temp)) & EDDS_IOBUFFER_MASK_NEW_FLAG )
		{
			// NOTE: the content of Xchange may changed in the meantime, but not the new flag
			Temp      = pCr->User;
			pCr->User = EDDS_IOBUFFER_EXCHANGE( &pCr->XChange, Temp );
			
			// reset new flag in User
			pValue   = ((LSA_UINT8*)&pCr->User);
			*pValue &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
		}
		
		UserIdx = (*((LSA_UINT8*)&pCr->User)) & EDDS_IOBUFFER_MASK_INDEX;
	    // check buffer index (0..2)
		EDDS_IOBUFFER_VALIDATE_BUFFER_INDEX( UserIdx );   //lint !e685 !e568 JB 12/11/2014 (define specific, macro specific)
		
		//convert buffer pointer from EDDS space to User space	
		pUserBuffer = EDDS_IOBUFFER_ADDR_EDDS_TO_USER( pIOBuf->MngmId, (EDD_UPPER_MEM_U8_PTR_TYPE) pCr->pBuffer[UserIdx] );  //lint !e661 JB 12/11/2014 fatal before if out of bounds
		

		/*-----------------------------------------------------------------------*/
		/* Calculate offset to I/O buffer. This depends on type of consumer.     */
		/*                                                                       */
		/* NOTE:                                                                 */
		/* Network endianess is always BigEndian. Vlan-Tag in buffer therefore   */
		/* always is 0x8100.                                                     */
		/*                                                                       */
		/* IOBuffer running on ...                                               */
		/* ... LittleEndian --> compare with 0x0081                              */
		/* ... BigEndian    --> compare with 0x8100                              */
		/*                                                                       */
		/* For definition of EDDS_VLAN_TAG see edds_dev.h. Depends on endianess  */
		/* configuration in edds_cfg.h for IOBuffer (regarding single core or    */
		/* multi core solution.                                                  */
		/*-----------------------------------------------------------------------*/

		#ifdef EDDS_XRT_OVER_UDP_INCLUDE
		if( EDDS_CSRT_CONSUMER_TYPE_UDP == pCr->Type )
		{
			if( ((EDD_UPPER_MEM_U16_PTR_TYPE )pUserBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET] == EDDS_VLAN_TAG )  //lint !e826 JB 12/11/2014 cast on purpose
				pUserBuffer += pIOBuf->OffsetUdpWithVlan;
			else
				pUserBuffer += pIOBuf->OffsetUdpWithoutVlan;
		}
		else
		#endif
		{
			if( ((EDD_UPPER_MEM_U16_PTR_TYPE )pUserBuffer)[EDDS_FRAME_LEN_TYPE_WORD_OFFSET] == EDDS_VLAN_TAG )  //lint !e826 JB 12/11/2014 cast on purpose
				pUserBuffer += pIOBuf->OffsetXrtWithVlan;
			else
				pUserBuffer += pIOBuf->OffsetXrtWithoutVlan;
		}
	}
	
	return pUserBuffer;		
}
//lint +esym(613,pIOBuf)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    EDDS_IOBuffer_ConsumerUnlock                       +*/
/*+                                                                         +*/
/*+  Input/Output   :    EDDS_IOBUFFER_HANDLE_TYPE   hIOBuffer              +*/
/*+                      LSA_UINT16                  CrNumber               +*/
/*+                                                                         +*/
/*+  Result         :    LSA_VOID                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hIOBuffer  :  io buffer handle from a previous call to                 +*/
/*+                EDDS_IOBuffer_Init                                       +*/
/*+  CrNumber   :  consumer Cr number                                       +*/
/*+                                                                         +*/
/*+  Results    :  LSA_VOID                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Unlock a previous locked consumer buffer. On error        +*/
/*+               EDDS_IOBUFFER_FATAL_ERROR is called.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIOBuf)
//JB 12/11/2014 pIOBuf = hIOBUffer; hIOBuffer is checked within EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE
LSA_VOID EDDS_IOBuffer_ConsumerUnlock ( 
    EDDS_IOBUFFER_HANDLE_TYPE  hIOBuffer,   // [IN]
    LSA_UINT16                 CrNumber )   // [IN]
{
	EDDS_IOBUFFER_PTR_TYPE         pIOBuf = (EDDS_IOBUFFER_PTR_TYPE) hIOBuffer;
	EDDS_CSRT_CONSUMER_CR_TYPE_PTR pCr;
	

	/*-----------------------------------------------------------------------*/
	/* Validate parameter.                                                   */
	/*-----------------------------------------------------------------------*/
	
	EDDS_IOBUFFER_VALIDATE_IOBUFFER_HANDLE( pIOBuf );                      // check handle
	EDDS_IOBUFFER_VALIDATE_CR_NUMBER( CrNumber, pIOBuf->ConsumerCrCnt );   // check Cr range
		
	// get consumer Cr
	pCr = &pIOBuf->pConsBufferTable[CrNumber - 1];   // CrNumber --> 1..ConsumerCrCnt

	EDDS_IOBUFFER_VALIDATE_CR_USED( pCr->CrUsed );       // Cr must be in use
	EDDS_IOBUFFER_VALIDATE_CR_LOCKED( pCr->IsLocked );   // Cr must be locked
	

	/*-----------------------------------------------------------------------*/
	/* Unlock the consumer.                                                  */
	/*-----------------------------------------------------------------------*/	
	
	pCr->IsLocked = LSA_FALSE;
}
//lint +esym(613,pIOBuf)

#endif

/*****************************************************************************/
/*  end of file edds_iobuf.c                                                 */
/*****************************************************************************/
