#ifndef DCP_INT_H                     /* ----- reinclude-protection ----- */
#define DCP_INT_H

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
/*  F i l e               &F: dcp_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  dcp.                                                                     */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2002-07-17  P00.00.00.00_00.01.00.00  lrg  from LSA_GLOB_P02.00.01.001   */
/*  2002-10-31  P01.00.00.00_00.01.01.01  lrg  1.Increment:Identify Response */
/*  2002-11-06  P01.00.00.00_00.02.00.00  lrg  2.Increment: GET/SET Response */
/*                                             new: DCP_MAX_TIMER_COUNT      */
/*  2003-01-27  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*                                             New function codes            */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*              Section "compiler errors" now in dcp_pls.h                   */
/*  2003-06-03  P01.00.00.00_00.06.00.00  lrg  6.Increment:                  */
/*              dcp_IdentifyFilterCheck() returns now LSA_INT                */
/*  2003-07-23  P01.00.00.00_00.07.02.00  Neuer Identblock SequenceNumber:   */
/*              dcp_IdentifyFilterCheck() neuer Parameter SeqRetPtrPtr       */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*  2003-09-29  P01.00.00.00_00.11.00.00  lrg 11.Increment: Mehrere IDENTIFY */
/*              gleichzeitig durch Timer-Array.                              */
/*  2003-12-15  P02.00.00.00_00.01.00.00  dsc  New version of timeout hand-  */
/*              ling introduced                                              */   
/*              DCP_MAX_TIMER_COUNT von dcp_int.h nach dcp_sys.h verschoben  */
/*  2003-02-09  P02.00.00.00_00.02.00.00  lrg                                */
/*              NEU: Native RQB-Interface mit Retry und Request-Queuing      */
/*  2004-02-23  P02.00.00.00_00.02.00.01  lrg  dcp_CheckReq(...,LSA_UINT8)   */
/*  2004-03-16  P02.00.00.00_00.02.00.02  lrg  QueueCancel                   */
/*              aus DCP_CLIENT_DATA entfernt                                 */
/*  2004-04-30  P02.00.00.00_00.02.05.02  lrg  DCP_TRANSACT um Response      */
/*              erweitert fuer Sendefehler/Cancel                            */
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-10-11  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*              dcp_LowerFilterSet() mit FilterCount versorgt                */
/*              111004lrg001 Prototype für dcp_CloseLowerSet()               */
/*  2004-12-17  P03.02.00.00_00.02.02.00  lrg  171204lrg001: New parameter   */
/*              RequestCount for dcp_ClientInd()                             */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  LSA_GLOB 2.1.0.0_0.4.2.1      */
/*  2005-01-12  P03.03.00.00_00.03.02.00  lrg  120105lrg001: New parameter   */
/*              RequestCount for dcp_ServerInd(). New: RecvRblClient in      */
/*              lower channel structure. Count in DCP_UPPER_QUEUE and        */
/*              DCP_LOWER_QUEUE changed from LSA_UINT8 to LSA_UINT32         */
/*  2007-03-14  P04.00.00.00_00.01.01.02  lrg  Hello, Ressourcensteuerung    */
/*  2007-07-23  P04.00.00.00_00.20.03.02  lrg  DCP_CLIENT_DEVICE_FSU_MIN     */
/*  2007-11-14  P04.00.00.00_00.50.01.02 141107lrg001: Tabs removed from this*/
/*              history, DCP_CLIENT_DEVICE_FSU_MIN -> DCP_CLIENT_DEVICE_MIN  */
/*  2007-12-05  P04.00.00.00_00.60.01.02 051207lrg001: ArtsRQ AP00552010     */
/*              Beim CloseChannel auf Sendeconfirmations warten.             */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-08-24  P05.00.00.00_00.01.01.02 280307lrg001: AP00999022 UPPER_DONE */
/*  2010-11-23  P05.00.00.00_00.02.01.02 231110lrg001: AP00988516            */
/*              DCP-Identify-Filter on confirmation                          */
/*  2011-01-18  P05.00.00.00_00.03.12.02 lrg: AP01017495 Index-Trace         */
/*  2011-04-21  S05.00.00.00_00.05.05.02 lrg: DCP_PROC_xxx removed           */
/*  2011-06-29  S05.01.00.00_00.03.03.02 lrg: AP01138397 DCP_TIMER_EXT       */
/*  2011-08-25  S05.01.00.00_00.04.03.02 AP00696784: Comments in english     */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-03-08  S05.02.00.00_00.01.16.01 AP01138373: DCP_OPC_FILTER_SET      */
/*              Entfernen des DCP Identify Filter Service in DCP             */
/*  2013-03-04  P05.02.00.00_00.04.17.02 AP01503994: free upper 2 bit of XID */
/*              Check DCP_MAX_CHANNELS <= 63                                 */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-06-17  P05.03.00.00_00.01.10.01 AP01387412 DCP Timer Optimierung    */
/*  2014-06-25  P05.03.00.00_00.03.09.01 1064327: ServerSetWait (WACK)       */
/*  2014-08-25  P05.03.00.00_00.03.18.01 1104556: Undo ServerSetWait (WACK)  */
/*  2014-09-03  P05.04.00.00_00.01.22.01 1089622: dcp_TimerTabExec() nicht   */
/*              im Timer-Kontext ausführen                                   */
/*  2014-10-01  P05.04.00.00_00.01.22.01 1097934 Umstellung auf PNIO-PACK    */
/*              Makros aus dcp_pck.h hierher verschoben                      */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
#define DCP_MODULE_ID_SYS 		1
#define DCP_MODULE_ID_USR 		2
#define DCP_MODULE_ID_LOW 		3
#define DCP_MODULE_ID_SRV 		4

/* Index value of multi user ID
*/
#define DCP_HANDLE_UID				0
#define DCP_CLIENT_UID				1
#define DCP_RBTYPE_UID				2
#define DCP_INDEX_UID				3

/* Lower requestblock types for user ID
*/
#define DCP_LOWER_OPEN				1
#define DCP_LOWER_SEND				2
#define DCP_LOWER_RECV				3
#define DCP_LOWER_CANCEL			4
#define DCP_LOWER_FILTER			5
#define DCP_LOWER_MULTICAST			6

/* AP01503994: max. DCP_MAX_CHANNELS = 63 to free uppermost 2 bits of XID (upper channel handle)
*/
#define DCP_CLIENT_XID_PNIO		0			// uppermost two bit = 0 (1, 2, 3)
#define DCP_CLIENT_XID_MASK		0x3fffffff

#ifndef	DCP_MAX_CHANNELS
#error "DCP_MAX_CHANNELS not defined"
#else
#if	(DCP_MAX_CHANNELS > 63)
#error "DCP_MAX_CHANNELS must not exceed 63"
#endif
#endif

/* AP01509782 DCP channel details
*/
#define DCP_CLIENT_RETRY_COUNT	4			// Number of Get/Set-Request Retries on RspTimeout: 
											// default = 4 for 1s timeout and servers ClientHoldTime 3s
#define DCP_CLIENT_HOLD_TIME	3			// Multirequest lock time in seconds: default 3s
#define DCP_CLIENT_UC_TIMEOUT	1000		// Milliseconds to wait for Get/Set response = UC Client Timeout: default 1s
#define DCP_CLIENT_UC_TIMEOUT_TICKS		10	// 1s on a 100ms timer

/* In order to have always one lower RQB in DCP for a HELLO request:
*/
#define DCP_NUM_CLIENT_HELLO_RESOURCES		2
#define DCP_NUM_IDENT_RECV					2	// for one IDENTIFY request (tbd)

/* Delay time Base (t_basis) in milliseconds 
 * for Server responses to IDENTIFY Multicast Requests
 * T_delay = t_basis * ( K_addr % Faltung) 	[sec]
*/
#define DCP_MULTICAST_DELAY_BASE	10	// default: 10ms

/* Client: round up to next 100ms
*/
#define DCP_MULTICAST_RSP_TIMER_BASE	100
#define DCP_UNICAST_RSP_TIMER_BASE		100

/* Server: round up to next 100ms 
*/
#define DCP_MULTICAST_REQ_TIMER_BASE	100

/* Constants for Hello.req
*/
#define DCP_HELLO_TIMER_BASE			LSA_TIME_BASE_10MS
#define DCP_HELLO_MC_ADDR				{0x01,0x0E,0xCF,0x00,0x00,0x01}
#define DCP_IDENTIFY_MC_ADDR			{0x01,0x0E,0xCF,0x00,0x00,0x00}
					  
/* Ethernet header (if values defined: from dcp_cfg.h)
*/
#ifdef	DCP_ETH_VLANTAG_0
#define DCP_VLANTAG_0	(LSA_UINT8)		DCP_ETH_VLANTAG_0
#else									/* VLANtag[0] in ethernet header DCP_ETHDR */
#define DCP_VLANTAG_0	(LSA_UINT8)		0x81
#endif
#ifdef	DCP_ETH_VLANTAG_1
#define DCP_VLANTAG_1	(LSA_UINT8)		DCP_ETH_VLANTAG_1
#else									/* VLANtag[1] in ethernet header DCP_ETHDR */
#define DCP_VLANTAG_1	(LSA_UINT8)		0x00
#endif
#ifdef	DCP_ETH_VLANTAG_2
#define DCP_VLANTAG_2	(LSA_UINT8)		DCP_ETH_VLANTAG_2
#else									/* VLANtag[2] in ethernet header DCP_ETHDR */
#define DCP_VLANTAG_2	(LSA_UINT8)		0x00
#endif
#ifdef	DCP_ETH_VLANTAG_3
#define DCP_VLANTAG_3	(LSA_UINT8)		DCP_ETH_VLANTAG_3
#else									/* VLANtag[3] in ethernet header DCP_ETHDR */
#define DCP_VLANTAG_3	(LSA_UINT8)		0x00
#endif
#ifdef	DCP_ETH_ETHTYPE_0
#define DCP_ETHTYPE_0	(LSA_UINT8)		DCP_ETH_ETHTYPE_0
#else									/* Ethertype[0] in ethernet header DCP_ETHDR */
#define DCP_ETHTYPE_0	(LSA_UINT8)		0x88
#endif
#ifdef	DCP_ETH_ETHTYPE_1
#define DCP_ETHTYPE_1	(LSA_UINT8)		DCP_ETH_ETHTYPE_1
#else									/* Ethertype[1] in ethernet header DCP_ETHDR */
#define DCP_ETHTYPE_1	(LSA_UINT8)		0x92
#endif
#ifdef	DCP_ETH_FRAMEID_0
#define DCP_FRAMEID_0	(LSA_UINT8)		DCP_ETH_FRAMEID_0
#else									/* FrameId[0] in ethernet header DCP_ETHDR */
#define DCP_FRAMEID_0	(LSA_UINT8)		0xfe
#endif
#ifdef	DCP_ETH_FRAMEID_MRQ
#define DCP_FRAMEID_MRQ	(LSA_UINT8)		DCP_ETH_FRAMEID_MRQ
#else									/* FrameId[1] for multicast request */
#define DCP_FRAMEID_MRQ	(LSA_UINT8)		0xfe
#endif
#ifdef	DCP_ETH_FRAMEID_MRS
#define DCP_FRAMEID_MRS	(LSA_UINT8)		DCP_ETH_FRAMEID_MRS
#else									/* FrameId[1] for multicast response */
#define DCP_FRAMEID_MRS	(LSA_UINT8)		0xff
#endif
#ifdef	DCP_ETH_FRAMEID_URR
#define DCP_FRAMEID_URR	(LSA_UINT8)		DCP_ETH_FRAMEID_URR
#else									/* FrameId[1] for unicast request/response */
#define DCP_FRAMEID_URR	(LSA_UINT8)		0xfd
#endif
#ifdef	DCP_ETH_FRAMEID_HLO
#define DCP_FRAMEID_HLO	(LSA_UINT8)		DCP_ETH_FRAMEID_HLO
#else									/* FrameId[1] for hello request */
#define DCP_FRAMEID_HLO	(LSA_UINT8)		0xfc
#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#ifndef DCP_BIG_ENDIAN

/* Macros to swap the high and low bytes of a 16 bit word
*/
#define DCP_SWAP16(u16_)	\
		((LSA_UINT16)(((((LSA_UINT16)(u16_)) >> 8) | (((LSA_UINT16)(u16_)) << 8))))

#define DCP_SWAPW(src16ptr_, dst16ptr_) \
                {*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+1); \
                 *((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+0);}

/* Macros to swap the high and low words and the contained bytes
 * of a unsigned 32 bit double word: 0x44332211 -> 0x11223344
*/
#define DCP_SWAP32(u32_)	\
		((LSA_UINT32)(DCP_SWAP16(DCP_HIWORD(u32_)) | (DCP_SWAP16(DCP_LOWORD(u32_)) << 16)))

#define DCP_SWAPD(src32ptr_, dst32ptr_) \
                {*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+3); \
                 *((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+2); \
                 *((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+1); \
                 *((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+0);}

#else	/* DCP_BIG_ENDIAN defined: */

/* No swapping nessesary for DCP send/receive
*/
#define DCP_SWAP16(u16_)	(u16_)
#define DCP_SWAP32(u32_)	(u32_)

#define DCP_SWAPW(src16ptr_, dst16ptr_) \
                {*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+0); \
                 *((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+1);}

#define DCP_SWAPD(src32ptr_, dst32ptr_) \
                {*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+0); \
                 *((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+1); \
                 *((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+2); \
                 *((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+3);}

#endif	/* DCP_BIG_ENDIAN defined */



/* Macro for decoding of the DCP PDU Type
*/
#define DCP_IS_REQUEST(PduPtr) \
		(((PduPtr)->Req.Hdr.ServiceType & DCP_ROLE_MASK) == DCP_ROLE_REQ)

/* Macro for decoding of the sequence id from XID
*/

#define DCP_SEQ_GET(PduPtr, SeqNoPtr) *SeqNoPtr = DCP_SWAP16(PduPtr->Pdu.Req.Hdr.XidLow);

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/* DCP reqest block queue structures
*/
typedef struct _DCP_UPPER_QUEUE
{
	LSA_UINT32				Count;
	DCP_UPPER_RQB_PTR_TYPE	FirstPtr;
	DCP_UPPER_RQB_PTR_TYPE	LastPtr;

} DCP_UPPER_QUEUE;

typedef struct _DCP_LOWER_QUEUE
{
	LSA_UINT32				Count;
	DCP_LOWER_RQB_PTR_TYPE	FirstPu;
	DCP_LOWER_RQB_PTR_TYPE	LastPu;

} DCP_LOWER_QUEUE;

/* Forward declaration ot timer block ***********************************
*/
struct _DCP_TIMER;

/* Pointer to timer block ***********************************************
*/ 
typedef struct _DCP_TIMER DCP_LOCAL_MEM_ATTR * DCP_TIMER_PTR_TYPE;

typedef struct _DCP_TIMER_EXT
{
	LSA_BOOL				InUse;
	LSA_BOOL				Cyclic;
	LSA_UINT32				CountAct;
	LSA_UINT32				CountStart;
	LSA_UINT32				Factor;

} DCP_TIMER_EXT;

typedef struct _DCP_TRANSACT
{
	DCP_LOWER_TXMEM_PTR_TYPE	TxPu;
	DCP_UPPER_RQB_PTR_TYPE		RbuPtr;
	LSA_UINT16					Xid;
	LSA_UINT16					Response;
	LSA_INT						Count;
	LSA_INT						Idx;

} DCP_TRANSACT;

typedef struct _DCP_IDENT
{
	LSA_UINT32					TransactId;
 	DCP_MAC_ADR					ClientAdr;

} DCP_IDENT;

typedef union _DCP_TRACT
{
	DCP_IDENT					IdentifyData;
	DCP_TRANSACT				Txs;

} DCP_TRACT;

typedef struct _DCP_TIMER
{
	DCP_TIMER_PTR_TYPE		NextPtr;
	DCP_TIMER_PTR_TYPE		PrevPtr;
	DCP_TIMER_EXT			Ext;
	DCP_TRACT				Tract;
	LSA_HANDLE_TYPE			Handle;
	LSA_BOOL				Running;
	LSA_BOOL				Single;
	LSA_UINT8				CallId;
	LSA_UINT8				Id;
	LSA_UINT16				Base;
	LSA_UINT16				Time;
	LSA_INT					SendLen;
	DCP_LOWER_RQB_PTR_TYPE	RbPu;
	DCP_UPPER_RQB_PTR_TYPE	RbPtr;

} DCP_TIMER;

typedef struct _DCP_TIMER_QUEUE
{
	LSA_UINT32				Count;
	DCP_TIMER_PTR_TYPE		FirstPtr;
	DCP_TIMER_PTR_TYPE		LastPtr;

} DCP_TIMER_QUEUE;

typedef struct _DCP_TIMER_TAB
{
	LSA_UINT16				Id;
	LSA_UINT16				State;
	DCP_TIMER_QUEUE			Queue;
    DCP_UPPER_RQB_PTR_TYPE	RbuPtr; 
	LSA_UINT8				RbuInUse;

} DCP_TIMER_TAB;

typedef struct _DCP_CLIENT_DATA
{
	DCP_UPPER_QUEUE				QueueUpper;
	DCP_UPPER_QUEUE				QueueNotify;
	DCP_LOWER_QUEUE				QueueSend;
	DCP_TIMER_QUEUE				TimerQueue;
	LSA_UINT16					Xid;
	LSA_UINT16					RetryCount;
	LSA_BOOL					Active;

} DCP_CLIENT_DATA;



#define DCP_CHA_PTR struct _DCP_CHANNEL *
#define DCP_CHL_PTR struct _DCP_CHANNEL_LOWER *

/* DCP lower channel structure
*/
typedef struct _DCP_CHANNEL_LOWER
{
	LSA_UINT8					State;
	LSA_UINT8					ChCount;
	LSA_HANDLE_TYPE				Handle;
	LSA_HANDLE_TYPE				HandleLower;
	LSA_INT						RecvRblAct;
	LSA_INT						RecvRblClient;
	DCP_LOWER_QUEUE				QueueRecv;
	DCP_LOWER_RQB_PTR_TYPE		OpenPu;
	DCP_CHA_PTR					ChServerPtr;
	DCP_CHA_PTR					ChClientPtr;
	LSA_UINT16					K_addr;
	DCP_MAC_ADR					LocalMacAdr;
	LSA_UINT32					TraceIdx;

} DCP_CHANNEL_LOWER;

/* DCP upper channel structure
*/
typedef struct _DCP_CHANNEL
{
	LSA_UINT8										State;
	LSA_HANDLE_TYPE 								Handle;
	LSA_HANDLE_TYPE 								HandleUpper;
	LSA_UINT8										TimerCount;
	LSA_UINT32										TraceIdx;
	LSA_SYS_PATH_TYPE								SysPath;
	DCP_TIMER_PTR_TYPE								TimerPtr;
	DCP_TIMER_QUEUE									TimerQueue;
	DCP_UPPER_QUEUE									QueueUpper;
	DCP_UPPER_QUEUE									QueueRetry;
	DCP_UPPER_QUEUE									QueueInd;
	DCP_UPPER_QUEUE									QueueGet;
	DCP_UPPER_QUEUE									QueueSet;
	DCP_UPPER_QUEUE									QueueHello;
	DCP_UPPER_QUEUE									QueueIdentify;
	DCP_UPPER_QUEUE									QueueRsp;
	DCP_LOWER_QUEUE									QueueLower;
	DCP_LOWER_QUEUE									QueueCancel;
	DCP_UPPER_RQB_PTR_TYPE							CancelRbu;
	DCP_DETAIL_PTR_TYPE								DetailPtr;
	LSA_SYS_PTR_TYPE								SysPtr;
	LSA_VOID LSA_FCT_PTR(DCP_UPPER_OUT_FCT_ATTR, 	CbfPtr)
						(DCP_UPPER_RQB_PTR_TYPE  RbuPtr);
	DCP_CHL_PTR										ChLowPtr;
	DCP_CHA_PTR										ChNextPtr;
 	DCP_MAC_ADR										ClientHoldAddr;
	LSA_INT											QueueRecvCnt;
	LSA_INT											SendReqCount;			//051207lrg001
	LSA_BOOL										WaitSendCnf;			//051207lrg001
	LSA_BOOL										ServerIndReady;
	DCP_CLIENT_DATA									Identify;
	DCP_CLIENT_DATA									Set;
	DCP_CLIENT_DATA									Get;
	DCP_CLIENT_DATA									Hello;
} DCP_CHANNEL;

/* Values for channel.State	(to be seen well at memory dump)
*/
#define DCP_CHA_STATE_ERROR		0xfe
#define DCP_CHA_STATE_FREE		0xff
#define DCP_CHA_STATE_OPENING	0xf0
#define DCP_CHA_STATE_OPEN		0xf1
#define DCP_CHA_STATE_CLOSING	0xf2
#define DCP_CHA_STATE_CLOSED	0xf3
#define DCP_CHA_STATE_CANCEL	0xf4

/*-----------------------------------------------------------------------------
* DCP local data structure
*/
typedef struct dcp_data_tag
{
	DCP_CHANNEL 		*	ChListPtr;
	DCP_CHANNEL_LOWER	*	ChLowListPtr;
	DCP_CHANNEL				ChList[DCP_MAX_CHANNELS];
	DCP_CHANNEL_LOWER		ChLowList[DCP_MAX_LOW_CHANNELS*2];	// *2 because of 240407lrg001: FSU-Channel
	LSA_FATAL_ERROR_TYPE	Error;
	DCP_TIMER_TAB			Timer100ms;
	DCP_TIMER_TAB			Timer10ms;
	
} DCP_DATA_TYPE;
 
/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/
#if (DCP_MODULE_ID == DCP_MODULE_ID_SYS)

#ifdef DCP_CLEAR_GLOBAL_DATA
DCP_DATA_TYPE  DCP_LOCAL_MEM_ATTR  dcp_data = {0};
#else
DCP_DATA_TYPE  DCP_LOCAL_MEM_ATTR  dcp_data;
#endif

#else
LSA_EXTERN DCP_DATA_TYPE  DCP_LOCAL_MEM_ATTR  dcp_data;
#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* Convert Caps
*/
#define DCP_LOWER_CASE(C, P1) if(((C) >= 'A')&&((C) <= 'Z')) *(P1)= (C)+' ';else *(P1) = (C);

#define DCP_DECAPS(P, L) {LSA_INT I; \
							for (I = 0; I < (L); I++) \
								DCP_LOWER_CASE(*(((LSA_UINT8*)(P)+I)), ((LSA_UINT8*)(P))+I)}

#ifdef PT2_MODE
#pragma error ("***DCP_INT.H: PT2_MODE not longer supported!")
#endif

/*********************************************************** 
 * Macro to copy a MAC- or IP Address
*/
#define DCP_DS_SET_ADDR(_DstPtr, _SrcPtr, len)		\
		{int i; for (i = 0; i < len; i++)		\
		*(((LSA_UINT8 *)(_DstPtr))+i) = *(((LSA_UINT8 *)(_SrcPtr))+i);}

/****************************************************** 
 * Macro to compare MAC Addresses
*/
#define DCP_CMP_MAC_ADDR(_DstAdr, _SrcAdr)		\
		(_DstAdr[0] == _SrcAdr[0]) && (_DstAdr[1] == _SrcAdr[1]) && \
		(_DstAdr[2] == _SrcAdr[2]) && (_DstAdr[3] == _SrcAdr[3]) && \
		(_DstAdr[4] == _SrcAdr[4]) && (_DstAdr[5] == _SrcAdr[5])

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* System and internal functions
*/
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_CallbackRbu(LSA_SYS_PTR_TYPE,DCP_UPPER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_RequestDone(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE, LSA_UINT16);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_DequeUpper(DCP_UPPER_RQB_PTR_TYPE);
DCP_CHA_PTR	DCP_LOCAL_FCT_ATTR dcp_EnqueUpper(DCP_UPPER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_GetUpperQ(DCP_UPPER_QUEUE *, DCP_UPPER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_PutUpperQ(DCP_UPPER_QUEUE *, DCP_UPPER_RQB_PTR_TYPE);
LSA_BOOL	DCP_LOCAL_FCT_ATTR dcp_FindUpper(DCP_UPPER_QUEUE *, DCP_UPPER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_DequeLower(DCP_LOWER_QUEUE *, DCP_LOWER_RQB_PTR_TYPE, DCP_CHA_PTR);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_EnqueLower(DCP_LOWER_QUEUE *, DCP_LOWER_RQB_PTR_TYPE, DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ChJoin(DCP_CHA_PTR);
DCP_CHA_PTR	DCP_LOCAL_FCT_ATTR dcp_ChAlloc(DCP_UPPER_RQB_PTR_TYPE, LSA_SYS_PTR_TYPE, DCP_DETAIL_PTR_TYPE);
DCP_CHA_PTR	DCP_LOCAL_FCT_ATTR dcp_ChGet(LSA_HANDLE_TYPE);
DCP_CHL_PTR	DCP_LOCAL_FCT_ATTR dcp_ChLowGet(LSA_HANDLE_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ChFree(DCP_CHA_PTR);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ChClean(DCP_CHA_PTR);
#ifdef DCP_DO_TIMEOUT_EXTERN
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_InitTimerRqb(DCP_UPPER_RQB_PTR_TYPE, LSA_HANDLE_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_TimeoutRequest(DCP_UPPER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_TimerSendRQB(LSA_UINT16,LSA_USER_ID_TYPE);
#endif
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_timeout_int(DCP_TIMER_PTR_TYPE);
DCP_TIMER_PTR_TYPE	DCP_LOCAL_FCT_ATTR dcp_TimerAlloc(DCP_CHA_PTR, LSA_UINT16, LSA_UINT16);
DCP_TIMER_PTR_TYPE	DCP_LOCAL_FCT_ATTR dcp_TimerGet(DCP_CHA_PTR, LSA_UINT8, LSA_UINT16, DCP_TIMER_QUEUE*, DCP_LOWER_RQB_PTR_TYPE, DCP_UPPER_RQB_PTR_TYPE);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerPut(DCP_CHA_PTR,DCP_TIMER_PTR_TYPE,DCP_TIMER_QUEUE*);
DCP_TIMER_PTR_TYPE  DCP_LOCAL_FCT_ATTR dcp_TimerFind(DCP_CHA_PTR,LSA_UINT8,DCP_TIMER_QUEUE*);
LSA_UINT16			DCP_LOCAL_FCT_ATTR dcp_TimerStart(DCP_CHA_PTR, DCP_TIMER_PTR_TYPE, LSA_UINT8, DCP_TIMER_QUEUE*, LSA_UINT16, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerStop(DCP_CHA_PTR, DCP_TIMER_PTR_TYPE, LSA_UINT8, DCP_TIMER_QUEUE*);
LSA_VOID 			DCP_LOCAL_FCT_ATTR dcp_TimerCalc(DCP_CHA_PTR);
LSA_VOID 			DCP_LOCAL_FCT_ATTR dcp_ErrorUser(DCP_INT, DCP_INT, DCP_INT, LSA_VOID *, LSA_UINT16);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_ErrorLower(LSA_INT, LSA_INT,	LSA_INT, LSA_VOID *, LSA_UINT16);
LSA_VOID 			DCP_LOCAL_FCT_ATTR dcp_ErrorInternal(DCP_INT, DCP_INT, DCP_INT, LSA_VOID *, LSA_UINT32);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_ErrorLower(LSA_INT, LSA_INT,	LSA_INT, LSA_VOID *, LSA_UINT16);
LSA_UINT16			DCP_LOCAL_FCT_ATTR dcp_TimerTabInit(LSA_VOID);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerTabExit(LSA_VOID);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerTabFree(LSA_UINT16*,LSA_UINT16);
LSA_UINT16			DCP_LOCAL_FCT_ATTR dcp_TimerTabStart(DCP_TIMER_PTR_TYPE);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerTabStop(DCP_CHA_PTR,DCP_TIMER_PTR_TYPE,LSA_UINT16*);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerTabExec(LSA_UINT16,DCP_TIMER_TAB*);
DCP_TIMER_PTR_TYPE	DCP_LOCAL_FCT_ATTR dcp_TimerTabFindAddr(DCP_CHA_PTR,LSA_UINT32,DCP_MAC_ADR);
LSA_BOOL			DCP_LOCAL_FCT_ATTR dcp_TimerDeque(DCP_TIMER_QUEUE*,DCP_TIMER_PTR_TYPE);
LSA_VOID			DCP_LOCAL_FCT_ATTR dcp_TimerEnque(DCP_TIMER_QUEUE*,DCP_TIMER_PTR_TYPE);

/* Lower layer functions
*/
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_OpenLowerStart(DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_OpenLowerMem(DCP_CHA_PTR);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_OpenLowerCh(DCP_CHA_PTR);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_LowerFilterSet(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT16);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_OpenLowerGet(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_OpenLowerSet(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_OpenLowerFin(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_AllocEthSend(DCP_CHA_PTR, LSA_UINT8, LSA_UINT8);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_AllocEthRecv(DCP_CHA_PTR, LSA_UINT8, LSA_UINT8, DCP_ETH_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_AllocLower(DCP_CHA_PTR, LSA_UINT8, LSA_UINT8, LSA_BOOL);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_OpenLowerError(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT16);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_CancelLower(DCP_CHA_PTR, LSA_BOOL, DCP_TIMER_PTR_TYPE, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_CloseLowerSet(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_CloseLower(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_CloseLowerFin(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_DataRecv(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT8, LSA_UINT8);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_DataSend(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_INT);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_FreeLowerRb(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_BOOL	DCP_LOCAL_FCT_ATTR dcp_LowerRqbComplete(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT8);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_CancelRecv(DCP_CHA_PTR, LSA_UINT8, LSA_UINT8);


/* DCP Server functions
*/
LSA_INT		DCP_LOCAL_FCT_ATTR dcp_RspHdrSet(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, DCP_USR_BYTE_PTR, LSA_UINT8, LSA_UINT32, DCP_USR_BYTE_PTR, LSA_INT);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ServerRequest(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_RecvIndReq(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_HelloIndReq(DCP_CHA_PTR,DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_SendRspReq(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_CancelRecvReq(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_Identify(DCP_REQ_PTR, LSA_INT, DCP_ETHDR *, DCP_CHA_PTR);
LSA_UINT8	DCP_LOCAL_FCT_ATTR dcp_CheckReq(DCP_REQ_PTR, LSA_UINT16, LSA_UINT8, LSA_UINT8, DCP_CHA_PTR);
LSA_INT		DCP_LOCAL_FCT_ATTR dcp_QueueCancel(DCP_CHA_PTR,DCP_UPPER_QUEUE*,LSA_INT,LSA_INT);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ServerCancel(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ServerConf(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ServerInd(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT32);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_Get(DCP_REQ_PTR, LSA_INT, DCP_ETHDR *, DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_Set(DCP_REQ_PTR, LSA_INT, DCP_ETHDR *, DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_Hello(DCP_REQ_PTR, LSA_INT, DCP_ETHDR *, LSA_UINT32, DCP_CHA_PTR);

/* DCP Client functions
*/
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientOpen(DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientRqb(DCP_CHA_PTR);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientRequest(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientIdentify(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE, LSA_INT);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientSet(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE, LSA_INT);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientGet(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE, LSA_INT);
LSA_UINT16	DCP_LOCAL_FCT_ATTR dcp_ClientHello(DCP_CHA_PTR,DCP_UPPER_RQB_PTR_TYPE,LSA_INT);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ClientDone(DCP_CHA_PTR, DCP_UPPER_RQB_PTR_TYPE, LSA_UINT16, LSA_UINT8, LSA_INT);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ClientConf(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT16, LSA_UINT32, LSA_UINT8);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ClientInd(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT16, LSA_UINT8, LSA_UINT32, LSA_UINT32);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ClientCancel(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, LSA_UINT16, LSA_UINT8, LSA_UINT8);
LSA_VOID	DCP_LOCAL_FCT_ATTR dcp_ClientTimeout(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, DCP_UPPER_RQB_PTR_TYPE, DCP_TIMER_PTR_TYPE, LSA_UINT8);
LSA_INT		DCP_LOCAL_FCT_ATTR dcp_CheckRsp(DCP_CHA_PTR, DCP_LOWER_RQB_PTR_TYPE, DCP_ETH_FRAME *, DCP_RSP **, LSA_UINT8 **);

DCP_CLIENT_DATA	* DCP_LOCAL_FCT_ATTR dcp_ClientData(DCP_CHA_PTR, LSA_UINT8);


/*****************************************************************************/
/*  end of file DCP_INT.H                                                    */
/*****************************************************************************/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of DCP_INT_H */
