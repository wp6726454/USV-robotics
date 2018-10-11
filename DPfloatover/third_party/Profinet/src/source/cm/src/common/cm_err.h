#ifndef CM_ERR_H
#define CM_ERR_H

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_err.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile: PNIO error definitions                              */
/*                                                                           */
/*****************************************************************************/

/*
 * NOTE: file is included by cm_usr.h
 */

/*---------------------------------------------------------------------------*/

/*
 * the CM_PNIO_ERR_CODE is returned in the out-argument
 * 'pnio_status' of the RPC methods as 'Unsigned32'
 *
 * the macros below must be used!
 */

#define CM_PNIO_ERR_MAKE(code, decode, code_1, code_2) (LSA_UINT32)( \
	( (((LSA_UINT32)(LSA_UINT8)(code))    << 24) \
	| (((LSA_UINT32)(LSA_UINT8)(decode))  << 16) \
	| (((LSA_UINT32)(LSA_UINT8)(code_1))  <<  8) \
	| (((LSA_UINT32)(LSA_UINT8)(code_2))/*<<  0*/) \
	))

#define CM_PNIO_ERR_FETCH_CODE(err)   ( (LSA_UINT8)((err) >> 24) )

#define CM_PNIO_ERR_FETCH_DECODE(err) ( (LSA_UINT8)((err) >> 16) )

#define CM_PNIO_ERR_FETCH_CODE_1(err) ( (LSA_UINT8)((err) >>  8) )

#define CM_PNIO_ERR_FETCH_CODE_2(err) ( (LSA_UINT8)((err) >>  0) )

/***/

#define CM_PNIO_ERR_MAKE_12(code, decode, code_12) (LSA_UINT32)( \
	( (((LSA_UINT32)(LSA_UINT8)(code))       << 24) \
	| (((LSA_UINT32)(LSA_UINT8)(decode))     << 16) \
	| (((LSA_UINT32)(LSA_UINT16)(code_12))/* <<  0*/) \
	))

#define CM_PNIO_ERR_CODE_12(code_1, code_2) (LSA_UINT16)( \
	( (((LSA_UINT16)(LSA_UINT8)(code_1))   <<  8) \
	| (((LSA_UINT16)(LSA_UINT8)(code_2))/* <<  0*/) \
	))

/*---------------------------------------------------------------------------*/

/*
 * PN IO Errors (PNIO Status)
 * see IEC 61158-6
 */

/* Chapter "Coding of the field ErrorCode" */
#define CM_PNIO_ERR_CODE_PNIO			0x81	/* Used for all errors not covered elsewhere. */
#define CM_PNIO_ERR_CODE_RTA			0xCF	/* Within the ERR-RTA-PDU and UDP-RTA-PDU */
#define CM_PNIO_ERR_CODE_ALARM_ACK		0xDA	/* Within the DATA-RTA-PDU and UDP-RTA-PDU */

#define CM_PNIO_ERR_CODE_CONNECT		0xDB
#define CM_PNIO_ERR_CODE_RELEASE		0xDC
#define CM_PNIO_ERR_CODE_CONTROL		0xDD
#define CM_PNIO_ERR_CODE_READ			0xDE
#define CM_PNIO_ERR_CODE_WRITE			0xDF

/* Chapter "Coding of the field ErrorDecode" */
#define CM_PNIO_ERR_DECODE_PNIO			0x81
#define CM_PNIO_ERR_DECODE_PNIORW		0x80

/* ErrorCode1 */
#define CM_PNIO_ERR_CODE1_CTLDINA		0x3F	/*  63 / CTLDINA (successor of the NRPM) */
#define CM_PNIO_ERR_CODE1_MAC			0x44	/*  68 / see "Multiplex MAC Protocol Machine (MUX)" */
#define CM_PNIO_ERR_CODE1_RTA			0xFD	/* 253 / Used by RTA for protocol error (RTA_ERR_CLS_PROTOCOL) */


/*
 * ok, success, no error
 */

#define CM_PNIO_ERR_NONE  ((LSA_UINT32)0x00000000)  /* ok, no error, same as CM_PNIO_ERR_MAKE(0, 0, 0, 0) */

/*
 * UnknownBlock(-Type)
 */

#define CM_PNIO_FAULTY_BlockType(err_code) \
	CM_PNIO_ERR_MAKE(err_code, CM_PNIO_ERR_DECODE_PNIO, 64/*RMPM*/, 1/*Unknown Blocks*/)


/*
 * FAULTY_RPC_CALL is codes as (see part 6 Table 86 - Values of ErrorCode2 for ErrorCode1=RPC)
 * ErrorCode := depending on RPCOpNum (0xDB - 0xDF)
 * ErrorDecode := independent from RPCOpNum (0x81)
 * ErrorCode1 := RPC (69)
 * ErrorCode2 := (0 - 8)
 */

#define CM_PNIO_RPC_CODE2_REJECTED				1
#define CM_PNIO_RPC_CODE2_FAULTED				2
#define CM_PNIO_RPC_CODE2_TIMEOUT				3
#define CM_PNIO_RPC_CODE2_IN_ARGS				4
#define CM_PNIO_RPC_CODE2_OUT_ARGS				5
#define CM_PNIO_RPC_CODE2_DECODE				6
#define CM_PNIO_RPC_CODE2_PNIO_OUT_ARGS			7
#define CM_PNIO_RPC_CODE2_APPLICATION_TIMEOUT	8

#define CM_PNIO_FAULTY_RPC_CALL(err_code, code2) \
	CM_PNIO_ERR_MAKE((err_code), CM_PNIO_ERR_DECODE_PNIO, 69, (code2))

/*
 * Constants for PNIO_D_Connect()
 */

#define CM_PNIO_FAULTY_ConnectBlockReq(code1, code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_CONNECT, CM_PNIO_ERR_DECODE_PNIO, (code1), (code2))

#define CM_PNIO_FAULTY_ARBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(1, (code2))

#define CM_PNIO_FAULTY_IOCRBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(2, (code2))

#define CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(3, (code2))

#define CM_PNIO_FAULTY_AlarmCRBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(4, (code2))

#define CM_PNIO_FAULTY_PrmServerReqBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(5, (code2))

#define CM_PNIO_FAULTY_MCRBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(6, (code2))

#define CM_PNIO_FAULTY_IRInfoBlock(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(9, (code2))

#define CM_PNIO_FAULTY_SRInfoBlock(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(10, (code2))

#define CM_PNIO_FAULTY_ARFSUBlock(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(11, (code2))

#define CM_PNIO_FAULTY_ARVendorBlockReq(code2) \
	CM_PNIO_FAULTY_ConnectBlockReq(12, (code2))

/*
 * Constants for PNIO_D_Release()
 */

#define CM_PNIO_FAULTY_Release(code1, code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_RELEASE, CM_PNIO_ERR_DECODE_PNIO, (code1), (code2))

#define CM_PNIO_FAULTY_ReleaseBlock(code2) \
	CM_PNIO_FAULTY_Release(40, (code2))

/*
 * Constants for PNIO_D_Control()
 */

#define CM_PNIO_FAULTY_IODControl_Code1_Connect  20
#define CM_PNIO_FAULTY_IODControl_Code1_Plug     21
#define CM_PNIO_FAULTY_IODControl_Code1_PrmBegin 24
#define CM_PNIO_FAULTY_IODControl_Code1_SubmoduleList 25

#define CM_PNIO_FAULTY_IODControlConnectPlug(code1, code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_CONTROL, CM_PNIO_ERR_DECODE_PNIO, (code1), (code2))

/*
 * Constants for PNIO_D_Read() and PNIO_D_Write()
 *
 * from the PNIO-Spec, Coding of ErrorCode1 with ErrorDecode PNIORW
 *
 * 10:__ = APPLICATION
 *     0 = read error
 *     1 = write error
 *     2 = module failure
 *     3 = not specified
 *     4 = not specified
 *     5 = not specified
 *     6 = not specified
 *     7 = busy
 *     8 = version conflict
 *     9 = feature not supported
 *    10 = User specific 1
 *    11 = User specific 2
 *    12 = User specific 3
 *    13 = User specific 4
 *    14 = User specific 5
 *    15 = User specific 6
 *
 * 11:__ = ACCESS
 *     0 = invalid index
 *     1 = write length error
 *     2 = invalid slot/subslot
 *     3 = type conflict
 *     4 = invalid area
 *     5 = state conflict
 *     6 = access denied
 *     7 = invalid range
 *     8 = invalid parameter
 *     9 = invalid type
 *    10 = backup
 *    11 = User specific 7
 *    12 = User specific 8
 *    13 = User specific 9
 *    14 = User specific 10
 *    15 = User specific 11
 *
 * 12:__ = RESOURCE (semantics: may work later)
 *     0 = read constrain conflict
 *     1 = write constrain conflict
 *     2 = resource busy
 *     3 = resource unavailable
 *     4 = not specified
 *     5 = not specified
 *     6 = not specified
 *     7 = not specified
 *     8 = User specific 12
 *     9 = User specific 13
 *    10 = User specific 14
 *    11 = User specific 15
 *    12 = User specific 16
 *    13 = User specific 17
 *    14 = User specific 18
 *    15 = User specific 19
 */

#define CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err) \
	(((pnio_err) & 0x00FFFFFF) | ((LSA_UINT32)CM_PNIO_ERR_CODE_WRITE << 24))

#define CM_PNIO_FAULTY_Record_PNIO(code1, code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_READ/*!*/, CM_PNIO_ERR_DECODE_PNIO, (code1), (code2))

#define CM_PNIO_FAULTY_Record_PNIORW(code1, code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_READ/*!*/, CM_PNIO_ERR_DECODE_PNIORW, (code1), (code2))


#define CM_PNIO_FAULTY_Record__RMPM_ArgsLengthInvalid() \
	CM_PNIO_FAULTY_Record_PNIO(64/*RMPM*/, 0/*ArgsLength invalid*/)

#define CM_PNIO_FAULTY_Record__RMPM_AruuidUnknown() \
	CM_PNIO_FAULTY_Record_PNIO(64/*RMPM*/, 5/*AR UUID unknown*/)

#define CM_PNIO_FAULTY_Record__RMPM_StateConflict() \
	CM_PNIO_FAULTY_Record_PNIO(64/*RMPM*/, 6/*State conflict*/)

#define CM_PNIO_FAULTY_Record__RMPM_OutOfMemory() \
	CM_PNIO_FAULTY_Record_PNIO(64/*RMPM*/, 8/*Out of Memory*/)

#define CM_PNIO_FAULTY_Record__Block(code2) \
	CM_PNIO_FAULTY_Record_PNIO(8, (code2))

/* 10:__ = APPLICATION */

#define CM_PNIO_FAULTY_Record__ReadError(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xA0, (code2))

#define CM_PNIO_FAULTY_Record__WriteError(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xA1, (code2))

#define CM_PNIO_FAULTY_Record__UnkownError(code2) /* see AP00392849 */ \
	CM_PNIO_FAULTY_Record_PNIORW(0xA3, (code2))

#define CM_PNIO_FAULTY_Record__VersionConflict(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xA8, (code2))

#define CM_PNIO_FAULTY_Record__FeatureNotSupported(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xA9, (code2))

/* 11:__ = ACCESS */

#define CM_PNIO_FAULTY_Record__InvalidIndex(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB0, (code2))

#define CM_PNIO_FAULTY_Record__WriteLengthError(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB1, (code2))

#define CM_PNIO_FAULTY_Record__InvalidSlotSubslot(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB2, (code2))

#define CM_PNIO_FAULTY_Record__InvalidArea(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB4, (code2))

#define CM_PNIO_FAULTY_Record__StateConflict(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB5, (code2))

#define CM_PNIO_FAULTY_Record__AccessDenied(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB6, (code2))

#define CM_PNIO_FAULTY_Record__InvalidRange(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB7, (code2))

#define CM_PNIO_FAULTY_Record__InvalidParameter(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB8, (code2))

#define CM_PNIO_FAULTY_Record__InvalidType(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xB9, (code2))

#define CM_PNIO_FAULTY_Record__Backup(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xBA, (code2))

#define CM_PNIO_FAULTY_Record__BufferTooSmall(code2) \
	CM_PNIO_FAULTY_Record__InvalidType((code2)) /* a special case: see part 5, "Read Partial Access" */

/* 12:__ = RESOURCE */

#define CM_PNIO_FAULTY_Record__ResourceBusy(code2) \
	CM_PNIO_FAULTY_Record_PNIORW(0xC2, (code2))

/*
 * Constants for IODConnectRes
 */

#define CM_PNIO_FAULTY_IODConnectRes(code12) \
	CM_PNIO_ERR_MAKE_12(CM_PNIO_ERR_CODE_CONNECT, CM_PNIO_ERR_DECODE_PNIO, code12)

#define CM_PNIO_ERR_ModuleDiffBlock(code2) \
	CM_PNIO_ERR_CODE_12(53/*ModuleDiffBlock*/, (code2))

#define CM_PNIO_ERR_ARVendorBlockRes(code2) \
	CM_PNIO_ERR_CODE_12(56/*ARVendorBlockRes*/, (code2))

/*
 * Constants for IOXControlRes
 */

#define CM_PNIO_FAULTY_IOXControlRes(code12) \
	CM_PNIO_ERR_MAKE_12(CM_PNIO_ERR_CODE_CONTROL, CM_PNIO_ERR_DECODE_PNIO, code12)

#define CM_PNIO_ERR_ControlBlockConnect(code2) \
	CM_PNIO_ERR_CODE_12(22/*ControlBlockConnect*/, (code2))

#define CM_PNIO_ERR_ControlBlockPlug(code2) \
	CM_PNIO_ERR_CODE_12(23/*ControlBlockPlug*/, (code2))

/*
 * CM generates AlarmAcks internally for Alarms that are marked
 * as 'unsupported' by the user. See CreateServer / CreateClient.
 *
 * 60 = AlarmAck, Error Code 0 = Alarm Type Not Supported
 *                           1 = Wrong Submodule State
 *                           2 = IOCARSR Backup - Alarm not executed
 */

#define CM_PNIO_ERR_AlarmTypeNotSupported \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_ALARM_ACK, CM_PNIO_ERR_DECODE_PNIO, 60, 0)

#define CM_PNIO_ERR_AlarmAndWrongSubmoduleState \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_ALARM_ACK, CM_PNIO_ERR_DECODE_PNIO, 60, 1)

#define CM_PNIO_ERR_AlarmAck_IOCARSR_backup \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_ALARM_ACK, CM_PNIO_ERR_DECODE_PNIO, 60, 2) /* TIA 209760 */

/*----------------------------------------------------------------------------*/

/* same as ACP_PNIO_STATUS_REASON_TEMPLATE for ERR-RTA-PDU */

#define CM_PNIO_ERR_RTA_PROTOCOL(code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_RTA, CM_PNIO_ERR_DECODE_PNIO, CM_PNIO_ERR_CODE1_RTA, (code2))

/*----------------------------------------------------------------------------*/

#define CM_PNIO_FAULTY_RTA(code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_PNIO, CM_PNIO_ERR_DECODE_PNIO, CM_PNIO_ERR_CODE1_RTA, (code2))

/*----------------------------------------------------------------------------*/

#define CM_PNIO_FAULTY_CTLDINA(code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_PNIO, CM_PNIO_ERR_DECODE_PNIO, CM_PNIO_ERR_CODE1_CTLDINA, (code2))

/*----------------------------------------------------------------------------*/

#define CM_PNIO_FAULTY_MAC(code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_PNIO, CM_PNIO_ERR_DECODE_PNIO, CM_PNIO_ERR_CODE1_MAC, (code2))

/*----------------------------------------------------------------------------*/

#define CM_PNIO_FAULTY_PDInstanceTailor(code2) \
	CM_PNIO_ERR_MAKE(CM_PNIO_ERR_CODE_WRITE, CM_PNIO_ERR_DECODE_PNIORW, 0xAA/*MachineTailorError*/, (code2)/*see SPH*/)

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ERR_H */
