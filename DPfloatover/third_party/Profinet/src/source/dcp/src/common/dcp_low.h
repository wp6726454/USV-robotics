#ifndef DCP_LOW_H                   /* ----- reinclude-protection ----- */
#define DCP_LOW_H

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
/*  C o m p o n e n t     &C: DCP (Discovery & Configuration Protocol)  :C&  */
/*                                                                           */
/*  F i l e               &F: dcp_low.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower component interface                                                */
/*  Defines constants, types, macros and prototyping for dcp   .             */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2002-07-17  P00.00.00.00_00.01.00.00  lrg  from LSA_GLOB_P02.00.00.00_00 */
/*                                             .01.04.01                     */
/*  2002-10-31  P01.00.00.00_00.01.01.01  lrg  1.Increment:Identify Response */
/*  2002-11-14  P01.00.00.00_00.02.00.00  lrg  2.Increment: GET/SET Response */
/*                                             Macros for lower RQB access   */
/*                                             moved to dcp_cfg.txt          */
/*                                             <- dcp_request_lower_done     */
/*                                             -> dcp_request_lower_eth_done */
/*  2003-01-27  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              dcp_request_lower_done() -> dcp_request_lower_eth_done()     */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-06-04  P01.00.00.00_00.06.00.00  lrg  6.Increment:                  */
/*              Prototypes for dcp_lower_tx_mem() and dcp_lower_rx_mem()     */
/*  2003-12-15  P02.00.00.00_00.01.00.00  lrg Version 2                      */
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg Version 3                      */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg LSA_GLOB 2.1.0.0_0.4.2.1       */
/*  2007-05-31  P04.00.00.00_00.10.02.02  lrg LSA_GLOB 2.1.0.0_0.8.1.1       */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2013-03-04  P05.02.00.00_00.04.17.02 AP01503994: free upper 2 bit of XID */
/*              DCP_MAX_CHANNELS = 63, lower RQB Makros dcp_cfg.h->dcp_low.h */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/
typedef union _DCP_LOWER_MEM_PTR_TYPE
{
	DCP_ETH_MEM_PTR_TYPE	MemEthPtr;
	DCP_PB_MEM_PTR_TYPE		MemPbPtr;
	LSA_VOID_PTR_TYPE		VoidPtr;

} DCP_LOWER_MEM_PTR_TYPE;

typedef union _DCP_LOWER_TXMEM_PTR_TYPE
{
	DCP_ETH_TXMEM_PTR_TYPE	TxEthPtr;
	DCP_PB_TXMEM_PTR_TYPE	TxPbPtr;

} DCP_LOWER_TXMEM_PTR_TYPE;

typedef union _DCP_LOWER_RXMEM_PTR_TYPE
{
	DCP_ETH_RXMEM_PTR_TYPE	RxEthPtr;
	DCP_PB_RXMEM_PTR_TYPE	RxPbPtr;

} DCP_LOWER_RXMEM_PTR_TYPE;

typedef union _DCP_LOWER_RQB_PTR_TYPE
{
	DCP_ETH_RQB_PTR_TYPE	RbEthPtr;
	DCP_PB_RQB_PTR_TYPE		RbPbPtr;
	LSA_VOID_PTR_TYPE		VoidPtr;

} DCP_LOWER_RQB_PTR_TYPE;

typedef union _DCP_LOWER_RQB_TYPE
{
	DCP_ETH_RQB_TYPE		RbEth;
	DCP_PB_RQB_TYPE			RbPb;

} DCP_LOWER_RQB_TYPE;

typedef union _DCP_ETH_PAR_TYPE
{
	 EDD_RQB_MULTICAST_TYPE	 Mcst;
	 EDD_RQB_NRT_RECV_TYPE	 Recv;
	 EDD_RQB_NRT_SEND_TYPE	 Send;
	 EDD_RQB_GET_PARAMS_TYPE getParam;

} DCP_ETH_PAR_TYPE;

typedef union _DCP_LOWER_PAR_TYPE
{
	DCP_ETH_PAR_TYPE		ParEth;
	DCP_PB_PAR_TYPE			ParPb;

} DCP_LOWER_PAR_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * Opcodes and service codes of lower RQB	(ethernet device driver)
*/
#define DCP_ETH_OPC_OPEN_CHANNEL	EDD_OPC_OPEN_CHANNEL
#define DCP_ETH_OPC_CLOSE_CHANNEL	EDD_OPC_CLOSE_CHANNEL
#define DCP_ETH_OPC_REQUEST			EDD_OPC_REQUEST
#define DCP_ETH_GET_PARAMS			EDD_SRV_GET_PARAMS
#define DCP_ETH_MULTICAST			EDD_SRV_MULTICAST
#define DCP_ETH_RECV				EDD_SRV_NRT_RECV
#define DCP_ETH_SEND				EDD_SRV_NRT_SEND
#define DCP_ETH_CANCEL				EDD_SRV_NRT_CANCEL

/* Mode for CANCEL request
*/
#define DCP_ETH_CANCEL_MODE_ALL		EDD_NRT_CANCEL_MODE_ALL
#define DCP_ETH_CANCEL_MODE_UID 	EDD_NRT_CANCEL_MODE_RX_BY_ID

/* access to elements of lower RQB	(ethernet device driver) 
*/
#define DCP_ETH_RQB_NEXT_GET			(EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR	//270109lrg001
#define DCP_ETH_RQB_PREV_GET			(EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_PREV_RQB_PTR	//270109lrg001
#define DCP_ETH_RQB_NEXT_SET			EDD_RQB_SET_NEXT_RQB_PTR
#define DCP_ETH_RQB_PREV_SET			EDD_RQB_SET_PREV_RQB_PTR
#define DCP_ETH_RQB_OPCODE_GET 			EDD_RQB_GET_OPCODE
#define DCP_ETH_RQB_OPCODE_SET 			EDD_RQB_SET_OPCODE
#define DCP_ETH_RQB_HANDLE_GET 			EDD_RQB_GET_HANDLE
#define DCP_ETH_RQB_HANDLE_SET	 		EDD_RQB_SET_HANDLE
#define DCP_ETH_RQB_USERID_UVAR32_GET 	EDD_RQB_GET_USERID_UVAR32
#define DCP_ETH_RQB_USERID_UVAR32_SET 	EDD_RQB_SET_USERID_UVAR32
#define DCP_ETH_RQB_USERID_GET	 		EDD_RQB_GET_USERID
#define DCP_ETH_RQB_COMPID_SET 			EDD_RQB_SET_COMPID
#define DCP_ETH_RQB_SERVICE_GET 		EDD_RQB_GET_SERVICE
#define DCP_ETH_RQB_SERVICE_SET 		EDD_RQB_SET_SERVICE
#define DCP_ETH_RQB_RESPONSE_GET		EDD_RQB_GET_RESPONSE
#define DCP_ETH_RQB_RESPONSE_SET		EDD_RQB_SET_RESPONSE
#define DCP_ETH_RQB_PPARAM_SET 			EDD_RQB_SET_PPARAM
#define DCP_ETH_RQB_USER_PTR_SET		EDD_RQB_SET_USERID_PTR
#define DCP_ETH_RQB_USER_PTR_GET		EDD_RQB_GET_USERID_PTR

/* Set CompID in err structure if extended error handling is supported
*/ 
#define DCP_ETH_RQB_ERR_COMPID_SET(_RbPtr, _Value)

#define DCP_ETH_RQB_PPARAM_GET(_RbPtr)	\
		((_RbPtr)->pParam)

/* Get error structure
*/ 
#define DCP_ETH_RQB_ERR_GET(_RbPtr) ((_RbPtr)->err)


/* ATTENTIONE: *** first call DCP_ETH_PPARAM_SET *** :ATTENTIONE
*/
#define DCP_ETH_RQB_HANDLE_UPPER_SET(_RbPtr, _Value)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->HandleUpper = (_Value))

#define DCP_ETH_RQB_HANDLE_LOWER_SET(_RbPtr, _Value)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->Handle = (_Value))

#define DCP_ETH_RQB_HANDLE_LOWER_GET(_RbPtr)	\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->Handle)

#define DCP_ETH_RQB_SYS_PATH_SET(_RbPtr, _Value)		\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->SysPath = (_Value))

#define DCP_ETH_RQB_CBF_PTR_SET(_RbPtr, _Value)			\
		(((EDD_UPPER_OPEN_CHANNEL_PTR_TYPE)((_RbPtr)->pParam))->Cbf = (_Value))

#define DCP_ETH_RQB_MULTICAST_ENABLE(_RbPtr, _AdrPtr)		 \
		{LSA_INT _i; for (_i = 0; _i < EDD_MAC_ADDR_SIZE; _i++)		\
			  ((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i] = *((_AdrPtr) + _i);	\
		((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->Mode = EDD_MULTICAST_ENABLE;}

/* 111004lrg001 
*/
#define DCP_ETH_RQB_MULTICAST_DISABLE(_RbPtr, _AdrPtr)		 \
		{LSA_INT _i; for (_i = 0; _i < EDD_MAC_ADDR_SIZE; _i++)		\
			  ((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i] = *((_AdrPtr) + _i);	\
		((EDD_RQB_MULTICAST_TYPE *)((_RbPtr)->pParam))->Mode = EDD_MULTICAST_DISABLE;}

/* 280307lrg001 
*/
#define DCP_ETH_RQB_GETPARAMS_MACADDR_GET(_RbPtr, _AdrPtr)		 \
		{LSA_INT _i; for (_i = 0; _i < EDD_MAC_ADDR_SIZE; _i++)		\
			*((_AdrPtr) + _i) = ((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->MACAddr.MacAdr[_i];}

#define DCP_ETH_RQB_GETPARAMS_TRACEIDX_GET(_RbPtr)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TraceIdx)

#define DCP_ETH_RQB_GETPARAMS_TRACEIDX_SET(_RbPtr, _Value)	 \
		(((EDD_RQB_GET_PARAMS_TYPE *)((_RbPtr)->pParam))->TraceIdx = (_Value))
						  
/* 270109lrg001: EDD_UPPER_MEM_U8_PTR_TYPE, TxFrmGroup
*/
#define DCP_ETH_RQB_SEND_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen);		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Priority = EDD_NRT_SEND_PRIO_0;		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->TxFrmGroup = EDD_NRT_TX_GRP_DCP;		\
		((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->PortID = EDD_PORT_ID_AUTO

#define DCP_ETH_RQB_SEND_LEN_SET(_RbPtr, _DataLen)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length = (LSA_UINT32)(_DataLen))

#define DCP_ETH_RQB_SEND_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->Length)

#define DCP_ETH_RQB_SEND_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_SEND_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define DCP_ETH_RQB_RECV_PTR_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer)

#define DCP_ETH_RQB_RECV_PTR_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_Value))	//270109lrg001

#define DCP_ETH_RQB_RECV_LEN_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount)

#define DCP_ETH_RQB_RECV_DATA_OFFSET_GET(_RbPtr)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataOffset)

#define DCP_ETH_RQB_RECV_DATA_LEN_GET(_RbPtr)	 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->UserDataLength)

#define DCP_ETH_RQB_RECV_LEN_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_Value))

#define DCP_ETH_RQB_RECV_ID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestID = (_Value))

/* 210307lrg001
*/
#define DCP_ETH_RQB_RECV_FFINFO_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->FrameFilterInfo)

#define DCP_ETH_RQB_RECV_FFINFO_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->FrameFilterInfo = (_Value))

/* 171204lrg001
*/
#define DCP_ETH_RQB_RECV_CNT_GET(_RbPtr)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt)

#define DCP_ETH_RQB_RECV_CNT_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt = (_Value))

/* 270109lrg001: EDD_UPPER_MEM_U8_PTR_TYPE
*/
#define DCP_ETH_RQB_RECV_DATA_SET(_RbPtr, _DataPtr, _DataLen)		 \
		((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->pBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(_DataPtr);	\
		((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->RequestCnt = 0;		\
		((EDD_RQB_NRT_RECV_TYPE *)((_RbPtr)->pParam))->IOCount = (_DataLen)

#define DCP_ETH_RQB_CANCEL_MODE_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->Mode = (_Value))

#define DCP_ETH_RQB_CANCEL_ID_SET(_RbPtr, _Value)		 \
		(((EDD_RQB_NRT_CANCEL_TYPE *)((_RbPtr)->pParam))->RequestID = (_Value))

#define DCP_ETH_FRAME_MACADR_SET(_DstPtr, _SrcPtr)			\
  		{int i; for (i = 0; i < DCP_MAC_ADR_SIZE; i++)	\
			*(((LSA_UINT8 *)(_DstPtr))+i) = *(((LSA_UINT8 *)(_SrcPtr))+i);}

#define DCP_ETH_FRAME_MACADR_GET(_DstPtr, _SrcPtr)			\
  		DCP_ETH_FRAME_MACADR_SET(_SrcPtr, _DstPtr)

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  dcp_request_lower_eth_done
 *
 * function:       callback-function
 *                 confirmation of open_channel(),
 *                 close_channel() and request()
 *
 * parameters:     DCP_ETH_RQB_PTR_TYPE     eth_rqb_ptr:  pointer to ethernet
 *                                                        RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_LOWER_IN_FCT_ATTR  dcp_request_lower_eth_done(
		  DCP_ETH_RQB_PTR_TYPE                        eth_rqb_ptr
);

/*=============================================================================
 * function name:  dcp_lower_rqb
 *
 * function:       asynchronous allocation of a lower RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of RQB
 *                 DCP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr:  pointer to lower 
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_rqb(
    LSA_USER_ID_TYPE           user_id,
    LSA_UINT16                 length,
    DCP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr
);

/*=============================================================================
 * function name:  dcp_lower_mem
 *
 * function:       asynchronous allocation of a lower memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 DCP_LOWER_MEM_PTR_TYPE     lower_mem_ptr:  pointer to lower 
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_mem(
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    DCP_LOWER_MEM_PTR_TYPE                              lower_mem_ptr
);


/*=============================================================================
 * function name:  dcp_lower_tx_mem
 *
 * function:       asynchronous allocation of a lower transmit memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 DCP_LOWER_TXMEM_PTR_TYPE   lower_txmem_ptr:pointer to lower 
 *                                                            transmit memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_tx_mem(
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    DCP_LOWER_TXMEM_PTR_TYPE                            lower_txmem_ptr
);

/*=============================================================================
 * function name:  dcp_lower_rx_mem
 *
 * function:       asynchronous allocation of a lower receive memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 DCP_LOWER_RXMEM_PTR_TYPE   lower_rxmem_ptr:pointer to lower 
 *                                                            receive memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_lower_rx_mem(
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    DCP_LOWER_RXMEM_PTR_TYPE                            lower_rxmem_ptr
);

/*====  out functions  =====*/

/*=============================================================================
 * function name:  DCP_OPEN_CHANNEL_LOWER
 *
 * function:       open a communication channel to lower layer
 *
 * parameters:     DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:  pointer to lower 
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of dcp
 *     LSA_USER_ID_TYPE   user-id:       id of dcp
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of dcp
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel                 
 *     LSA_VOID  LSA_FCT_PTR(DCP_LOWER_IN_FCT_ATTR, dcp_request_lower_done_ptr)
 *                                    (DCP_LOWER_RQB_PTR_TYPE lower_rqb_ptr)
 *                                       pointer to callback-function
 *
 * RQB return values via callback-function:
 *     LSA_UINT16         response:      *_RSP_OK	if no error
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of lower layer
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
#ifndef DCP_OPEN_CHANNEL_LOWER
LSA_VOID  DCP_LOWER_OUT_FCT_ATTR  DCP_OPEN_CHANNEL_LOWER(
    DCP_LOWER_RQB_PTR_TYPE                              lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_CLOSE_CHANNEL_LOWER
 *
 * function:       close a communication channel to lower layer
 *
 * parameters:     DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:     pointer to lower 
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:  id of dcp
 *
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of dcp
 *     LSA_UINT16        response:  *_RSP_OK	if no error
 *                                  or others
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
#ifndef DCP_CLOSE_CHANNEL_LOWER
LSA_VOID  DCP_LOWER_OUT_FCT_ATTR  DCP_CLOSE_CHANNEL_LOWER(
    DCP_LOWER_RQB_PTR_TYPE                              lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_REQUEST_LOWER
 *
 * function:       send a request to a communication channel to lower layer
 *
 * parameters:     DCP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr:     pointer to lower 
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of lower layer
 *     LSA_USER_ID_TYPE  user-id:   id of dcp
 *     RQB args:                    Depend on kind of request.
 *     
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of dcp
 *     LSA_USER_ID_TYPE  user-id:   id of dcp
 *     LSA_UINT16        response:  *_RSP_OK	if no error
 *     
 *     All other RQB parameters depend on kind of request.
 *===========================================================================*/
#ifndef DCP_REQUEST_LOWER
LSA_VOID  DCP_LOWER_OUT_FCT_ATTR  DCP_REQUEST_LOWER(
    DCP_LOWER_RQB_PTR_TYPE                              lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_LOWER_RQB
 *
 * function:       allocate a lower RQB
 *
 * parameters:     DCP_LOWER_RQB_PTR_TYPE  ...  *     lower_rqb_ptr_ptr:
 *                                   return value: pointer to lower RQB pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_lower_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_LOWER_RQB
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_LOWER_RQB(
    DCP_LOWER_RQB_PTR_TYPE  DCP_LOCAL_MEM_ATTR *        lower_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_LOWER_RQB
 *
 * function:       free a lower RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower RQB or
 *                                                      wrong system pointer
 *                 DCP_LOWER_RQB_PTR_TYPE     lower_rqb_ptr:  pointer to lower 
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_LOWER_RQB
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_LOWER_RQB(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
    DCP_LOWER_RQB_PTR_TYPE                              lower_rqb_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_LOWER_MEM
 *
 * function:       allocate a lower memory
 *
 * parameters:     DCP_LOWER_MEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_LOWER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_LOWER_MEM(
    DCP_LOWER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *        lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_LOWER_MEM
 *
 * function:       free a lower memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower memory or
 *                                                      wrong system pointer
 *                 DCP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:     pointer to lower 
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_LOWER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_LOWER_MEM(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
    DCP_LOWER_MEM_PTR_TYPE                              lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_LOWER_TXMEM
 *
 * function:       allocate a lower memory for send PDUs
 *
 * parameters:     DCP_LOWER_TXMEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_LOWER_TXMEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_LOWER_TXMEM(
    DCP_LOWER_TXMEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *      lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_LOWER_TXMEM
 *
 * function:       free a lower memory for send PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower memory or
 *                                                      wrong system pointer
 *                 DCP_LOWER_MEM_PTR_TYPE  lower_mem_ptr:     pointer to lower 
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_LOWER_TXMEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_LOWER_TXMEM(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
    DCP_LOWER_TXMEM_PTR_TYPE                            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_LOWER_RXMEM
 *
 * function:       allocate a lower memory for receive PDUs
 *
 * parameters:     DCP_LOWER_RXMEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_lower_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE  sys_ptr:                   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_LOWER_RXMEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_LOWER_RXMEM(
    DCP_LOWER_RXMEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *      lower_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                    user_id,
    LSA_UINT16                                          length,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_LOWER_RXMEM
 *
 * function:       free a lower memory for receive PDUs
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      lower memory or
 *                                                      wrong system pointer
 *                 DCP_LOWER_RXMEM_PTR_TYPE  lower_mem_ptr:     pointer to lower 
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_LOWER_RXMEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_LOWER_RXMEM(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
    DCP_LOWER_RXMEM_PTR_TYPE                            lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                    sys_ptr
);
#endif

/*****************************************************************************/
/*  end of file DCP_LOW.H                                                    */
/*****************************************************************************/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of DCP_LOW_H */
