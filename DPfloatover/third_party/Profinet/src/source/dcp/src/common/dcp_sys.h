#ifndef DCP_SYS_H                     /* ----- reinclude-protection ----- */
#define DCP_SYS_H

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
/*  F i l e               &F: dcp_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for dcp.                */
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
/*  2002-11-08  P01.00.00.00_00.02.00.00  lrg  2.Increment: GET/SET Response */
/*  2003-01-27  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              DCP_TRACE_SET_REQ(): DCP_REQ_PTR -> LSA_UINT8 *              */
/*              New: DCP_TRACE_SET_REQ()                                     */
/*              DCP_MAC_ADR, DCP_IP_ADR moved here from cfg.h and pck.h      */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-05-05  P01.00.00.00_00.05.00.00  lrg  5.Increment: SHOW_LOCATION    */
/*  2003-06-04  P01.00.00.00_00.06.00.00  lrg  6.Increment:                  */
/*              Comments at DCP_DETAIL_TYPE     							 */
/*  2003-07-01  P01.00.00.00_00.07.00.00  lrg  7.Increment:                  */
/*              DCP_MAC_ADR_SIZE and DCP_IP_ADR_SIZE moved to dcp_cfg.h/txt  */
/*              DCP_COPY_RECV_TO_LOCAL_MEM() ersetzt durch DCP_COPY_RXETH_   */
/*              TO_LOCAL_MEM() und DCP_COPY_LOCAL_TO_SEND_MEM() ersetzt      */ 
/*              durch DCP_COPY_LOCAL_TO_ETHTX_MEM()                          */
/*  2003-07-09  neu: DCP_READ_PROFINET_ROLE()                                */
/*  2003-07-17  P01.00.00.00_00.07.01.00  lrg  LSA_GLOB 2.1.0.0_0.2.1.1      */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*  2003-08-28  P01.00.00.00_00.09.00.00  lrg  9.Increment: Profibus DB      */
/*  2003-09-01  P01.00.00.00_00.10.00.00  lrg  10.Increment: DHCP-           */
/*              Datensatz 03 unterstuetzen                                   */
/*  2003-11-04  P02.00.00.00_00.01.00.00  lrg  1.Increment Version 2: 		 */
/*              DCP_RAND() und DCP_SRAND() entfernt                          */
/*              neu: DCP_LOCK_BEGIN() und DCP_LOCK_END()                     */
/*              neu: DCP_FILTER_GET()                                        */
/*  2003-12-15  P02.00.00.00_00.01.00.00  dsc  New version of timeout hand-  */
/*              ling introduced                                              */   
/*              RetVal für DCP_TRACE_SET_RSP(): wenn != LSA_RET_OK wird das  */   
/*              erste Resultfeld der Response auf DCP_RESULT_DS_NOT_SET      */
/*              gesetzt.                                                     */
/*              DCP_MAX_TIMER_COUNT von dcp_int.h nach dcp_sys.h verschoben  */
/*  2004-01-28  P02.00.00.00_00.02.00.00  lrg  PBC-Erweiterung               */
/*              Neue Ausgangsfunktion: DCP_READ_PBC_SWITCH_CONFIG()          */
/*  2003-02-09  P02.00.00.00_00.02.00.00  lrg                                */
/*              NEU: Native RQB-Interface mit Retry und Request-Queuing      */
/*  2004-03-29  P02.00.00.00_00.02.00.01  lrg  DCP_MAX_TIMER_COUNT dependend */
/*              of DCP_CLIENT_MODE, DCP_SERVER_MODE and DCP_MAX_LOW_CHANNELS */   
/*  2004-03-31  P02.00.00.00_00.02.05.02  lrg  DCP_STATION_NAME_GET(...)     */
/*              #ifndef DCP_STATION_TYPE_GET -> #fndef DCP_STATION_NAME_GET  */   
/*  2004-04-19  P02.00.00.00_00.03.00.00  lrg  Details fuer Server mit RQB-  */
/*              Interface. Prototypen der DCP-Ausgangsfunktionen entfernt    */   
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-10-04  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*              neu: DCP_UPPER_TO_ETH_MEM()                                  */
/*  2004-10-13  P03.02.00.00_00.02.00.01  lrg  High- und Low-Byte der MAC-   */
/*              Adresse zur Berechnung der Faltungskonstanten vertauscht:    */
/*              DCP_MAC_RAND_IDX4 (4) wird zu DCP_MAC_RAND_IDX_LOW (5) und   */
/*              DCP_MAC_RAND_IDX5 (5) wird zu DCP_MAC_RAND_IDX_HIGH (4).     */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  LSA_GLOB 2.1.0.0_0.4.2.1      */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2007-03-16  P04.00.00.00_00.01.01.02  lrg  Hello und Ressorcensteuerung  */
/*              new: DCP_MEMSET_LOCAL, LocalMacAdr removed from Details      */
/*  2007-05-31  P04.00.00.00_00.10.02.02  lrg LSA_GLOB 2.1.0.0_0.8.1.1       */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-11-23  P05.00.00.00_00.02.01.02 231110lrg001: AP00988516            */
/*              DCP-Identify-Filter on confirmation                          */
/*  2011-06-29  S05.01.00.00_00.03.03.02 lrg: AP01138397 DCP_xxx_TIMER_COUNT */
/*  2012-01-11  T05.01.00.00_00.05.13.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01069471 DCP_REQUEST_UPPER_DONE   */
/*              and all other functions with DCP_SYSTEM_OUT_FCT_ATTR moved   */
/*              from dcp_usr.h to dcp_sys.h                                  */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-06-17  P05.03.00.00_00.01.10.01 AP01387412 DCP Timer Optimierung    */
/*  2014-05-26  P05.04.00.00_00.01.09.01 1027120 Korrektur der PSI-Konfigu-  */
/*              ration im DPC bezüglich DCP_DO_TIMEOUT_EXTERN                */
/*              --> dcp_timeout_sys() removed                                */
/*  2014-10-01  P05.04.00.00_00.01.22.01 1097934 Umstellung auf PNIO-PACK    */
/*              Strukturen und Konstanten aus dcp_pck.h hierher verschoben   */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                basic types                                */
/*     (normally you shouldn't use these definitions, use LSA_* instead)     */
/*===========================================================================*/
#define DCP_UINT8                LSA_UINT8
#define DCP_UINT16               LSA_UINT16
#define DCP_UINT32               LSA_UINT32

#define DCP_INT8                 LSA_INT8
#define DCP_INT16                LSA_INT16
#define DCP_INT32                LSA_INT32

#define DCP_INT                  LSA_INT

/*===========================================================================*/
/*                                other types                                */
/*     (normally you shouldn't use these definitions, use LSA_* instead)     */
/*===========================================================================*/
#define DCP_VOID                 LSA_VOID
#define DCP_VOID_PTR_TYPE        LSA_VOID_PTR_TYPE
#define DCP_BOOL                 LSA_BOOL
#define DCP_USER_ID_TYPE         LSA_USER_ID_TYPE

/*===========================================================================*/
/*                             basic constants                               */
/*     (normally you shouldn't use these definitions, use LSA_* instead)     */
/*===========================================================================*/
#define DCP_FALSE                LSA_FALSE
#define DCP_TRUE                 LSA_TRUE
#define DCP_NULL                 LSA_NULL
#define DCP_OK                   LSA_OK
#define DCP_EXTERN               LSA_EXTERN

/*=====  timer types for system integration  =====*/
#define DCP_TIMER_TYPE_ONE_SHOT  LSA_TIMER_TYPE_ONE_SHOT
#define DCP_TIMER_TYPE_CYCLIC    LSA_TIMER_TYPE_CYCLIC

/*=====  time base for system integration  =====*/
#define DCP_TIME_BASE_1US        LSA_TIME_BASE_1US
#define DCP_TIME_BASE_10US       LSA_TIME_BASE_10US
#define DCP_TIME_BASE_100US      LSA_TIME_BASE_100US
#define DCP_TIME_BASE_1MS        LSA_TIME_BASE_1MS
#define DCP_TIME_BASE_10MS       LSA_TIME_BASE_10MS
#define DCP_TIME_BASE_100MS      LSA_TIME_BASE_100MS
#define DCP_TIME_BASE_1S         LSA_TIME_BASE_1S
#define DCP_TIME_BASE_10S        LSA_TIME_BASE_10S
#define DCP_TIME_BASE_100S       LSA_TIME_BASE_100S

/* Values for timer user_id
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_TIMER")
#define DCP_TIMER_IDENTIFY		1
#define DCP_TIMER_LOCK			2
#define DCP_TIMER_SHOW			3
#define DCP_TIMER_IDENTREQ		4
#define DCP_TIMER_SETREQ		5
#define DCP_TIMER_GETREQ		6
#define DCP_TIMER_HELLOREQ		7
PTRC_SUBLABEL_LIST_CLOSE("DCP_TIMER")

/*===========================================================================*/
/*                             other constants                               */
/*     (normally you shouldn't use these definitions, use LSA_* instead)     */
/*===========================================================================*/

/*=====  function return-values  =====*/
#define DCP_RET_OK                         	LSA_RET_OK
#define DCP_RET_OK_TIMER_RESTARTED         	LSA_RET_OK_TIMER_RESTARTED
#define DCP_RET_OK_TIMER_NOT_RUNNING       	LSA_RET_OK_TIMER_NOT_RUNNING

#define DCP_RET_ERR_RESOURCE               	LSA_RET_ERR_RESOURCE
#define DCP_RET_ERR_PARAM                  	LSA_RET_ERR_PARAM
#define DCP_RET_ERR_SYS_PATH               	LSA_RET_ERR_SYS_PATH
#define DCP_RET_ERR_NO_TIMER               	LSA_RET_ERR_NO_TIMER
#define DCP_RET_ERR_TIMER_IS_RUNNING       	LSA_RET_ERR_TIMER_IS_RUNNING

/* Error codes for error function and trace
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_ERR")
#define DCP_ERR_FATAL						(LSA_UINT16) 0x8000
#define DCP_ERR_BAD_HANDLE					(LSA_UINT16) 0x8001
#define DCP_ERR_NO_CBF						(LSA_UINT16) 0x8002
#define DCP_ERR_BAD_OPCODE					(LSA_UINT16) 0x8003
#define DCP_ERR_CH_FREE						(LSA_UINT16) 0x8004
#define DCP_ERR_CH_PTR						(LSA_UINT16) 0x8005
#define DCP_ERR_NO_RBU						(LSA_UINT16) 0x8006
#define DCP_ERR_ASYNC_PTR					(LSA_UINT16) 0x8007
#define DCP_ERR_BAD_SERVICE					(LSA_UINT16) 0x8008
#define DCP_ERR_DEVICE						(LSA_UINT16) 0x8009
#define DCP_ERR_FREE_LOWER_MEM				(LSA_UINT16) 0x800a
#define DCP_ERR_FREE_LOWER_RQB				(LSA_UINT16) 0x800b
#define DCP_ERR_NO_SYSTYPE 					(LSA_UINT16) 0x800c
#define DCP_ERR_RECEIVE 					(LSA_UINT16) 0x800d
#define DCP_ERR_SEND	 					(LSA_UINT16) 0x800e
#define DCP_ERR_TIMER	 					(LSA_UINT16) 0x800f
#define DCP_ERR_TIMER_STOP 					(LSA_UINT16) 0x8010
#define DCP_ERR_TIMER_FREE					(LSA_UINT16) 0x8011
#define DCP_ERR_REQLEN	 					(LSA_UINT16) 0x8012
#define DCP_ERR_RBU_FREE	 				(LSA_UINT16) 0x8013
#define DCP_ERR_REL_PATH	 				(LSA_UINT16) 0x8014
#define DCP_ERR_CONFIG		 				(LSA_UINT16) 0x8015
#define DCP_ERR_UID			 				(LSA_UINT16) 0x8016
PTRC_SUBLABEL_LIST_CLOSE("DCP_ERR")
	
/*===========================================================================*/
/*                       constants for detail pointer                        */
/*===========================================================================*/

/* Bytes in MAC-Adr to calculate random delay time for response to
 * multicast service request
*/
#define DCP_MAC_RAND_IDX_LOW	5
#define DCP_MAC_RAND_IDX_HIGH	4


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef LSA_UINT8	DCP_MAC_ADR[DCP_MAC_ADR_SIZE];
typedef LSA_UINT8	DCP_IP_ADR[DCP_IP_ADR_SIZE];

/* Profile- and busparameters for Profibus (Database 0x0001)
 * Profile id and standard busparameters (Dataset 0x01)
 * 16- and 32-bit values are in systems notification (NOT swapped)
*/
typedef struct _DCP_PBPROFIL
{
	LSA_UINT8		 	ProfileId;
	LSA_UINT8		 	NumMasterCl2Allowed;
	LSA_UINT8		 	FdlAdd;
	LSA_UINT8		 	Baudrate;
	LSA_UINT16		 	Tsl;
	LSA_UINT16		 	TsdrMin;
	LSA_UINT16		 	TsdrMax;
	LSA_UINT8		 	Tqui;
	LSA_UINT8		 	Tset;
	LSA_UINT32		 	TTR;
	LSA_UINT8		 	G;
	LSA_UINT8		 	Hsa;
	LSA_UINT8		 	MaxRetryLimit;
	LSA_UINT8		 	RetryCtrToken;
	LSA_UINT16		 	TbusCtrlInRing;
	LSA_UINT16		 	TbusCtrlOutOfRing;
	LSA_UINT16		 	TreportSend;

} DCP_PBPROFIL;

/* Profile- and busparameters for Profibus (Database 0x0001)
 * Additional parameters (Dataset 0x02) 
 * 16- and 32-bit values are in systems notification (NOT swapped)
*/
typedef struct _DCP_PBPARAM
{
	LSA_UINT32		 	Tmsi;
	LSA_UINT16		 	TmsiReserve;
	LSA_UINT32		 	TdpCyc;
	LSA_UINT16		 	AcycReqCtr;
	LSA_UINT8		 	ModeEquDis;
	LSA_UINT8		 	MasterEquDis;
	LSA_UINT8		 	CycleCtrEquDis;
	LSA_UINT8		 	SynSap;
	LSA_UINT8		 	SynSduLen;
	LSA_UINT32		 	TthEquDis;
	LSA_UINT8		 	ModeClockSync;
	LSA_UINT8		 	DelayTimeCtrClockSync;
	LSA_UINT8		 	Max_Tjitter;

} DCP_PBPARAM;

/* Pointer types
*  =============
*/

#define DCP_SYS_BYTE_PTR_TYPE                   /* pointer to system-byte */ \
    LSA_UINT8                   DCP_SYSTEM_MEM_ATTR *

#define DCP_SYS_WORD_PTR_TYPE                   /* pointer to system-word */ \
    LSA_UINT16                  DCP_SYSTEM_MEM_ATTR *

#define DCP_SYS_PBPROFIL_PTR_TYPE               /* pointer to system-struct */ \
    DCP_PBPROFIL                DCP_SYSTEM_MEM_ATTR *

#define DCP_SYS_PBPARAM_PTR_TYPE               /* pointer to system-struct */ \
    DCP_PBPARAM                 DCP_SYSTEM_MEM_ATTR *

#define DCP_UPPER_MEM_PTR_TYPE                  /* pointer to upper-memory */ \
    LSA_VOID                    DCP_UPPER_MEM_ATTR *

#define DCP_LOWER_ETH_MEM_PTR_TYPE    /* pointer to ethernet driver memory */ \
    LSA_VOID                    DCP_LOWER_ETH_MEM_ATTR *

#define DCP_ETH_TXBYTE_PTR_TYPE    /* pointer to ethernet driver tx memory */ \
    LSA_UINT8					DCP_LOWER_ETH_TXMEM_ATTR *

#define DCP_ETH_RXBYTE_PTR_TYPE    /* pointer to ethernet driver rx memory */ \
    LSA_UINT8					DCP_LOWER_ETH_RXMEM_ATTR *

#define DCP_ETH_RXVOID_PTR_TYPE    /* pointer to ethernet driver rx memory */ \
    LSA_VOID					DCP_LOWER_ETH_RXMEM_ATTR *

#define DCP_ETH_TXVOID_PTR_TYPE    /* pointer to ethernet driver tx memory */ \
    LSA_VOID					DCP_LOWER_ETH_TXMEM_ATTR *

#define DCP_LOWER_PB_MEM_PTR_TYPE     /* pointer to profibus driver memory */ \
    LSA_VOID                    DCP_LOWER_PB_MEM_ATTR *

#define DCP_LOCAL_MEM_PTR_TYPE                  /* pointer to local-memory */ \
    LSA_VOID                    DCP_LOCAL_MEM_ATTR *

#define DCP_DETAIL_PTR_TYPE                       /* pointer to DCP_DETAIL */ \
    struct dcp_detail_tag       DCP_SYSTEM_MEM_ATTR *
                               
#define DCP_FATAL_ERROR_PTR_TYPE             /* pointer to LSA_FATAL_ERROR */ \
    struct lsa_fatal_error_tag  DCP_LOCAL_MEM_ATTR *

/* dcp_fatal_error
*  ===============
*      (normally you shouldn't use this definition, use LSA_* instead)
*/
#define DCP_FATAL_ERROR_TYPE             LSA_FATAL_ERROR_TYPE

/* dcp_detail
*  ==========
*/
typedef struct dcp_detail_tag
{
	LSA_UINT16	LowerId;  				/* Lower Channel Identification:  must be the same	*/
										/* value at server, FSU and client channel on the	*/
 										/* same lower device.								*/
	LSA_UINT8	DevId;  				/* Standard Ethernet or Fast Start Up (see below)	*/
										/* DCP_DEV_FSU has to be opened by system before 	*/
										/* Hello indications can be received on server chan.*/
	LSA_BOOL	ClientCh;				/* TRUE: Detail extension for client channel exists	*/
										/* FALSE: This is a server channel or DCP_DEV_FSU	*/
	LSA_BOOL	AllocOnRequest;			/* FALSE: Allocate all lower RBs at open_channel	*/
										/* ------------------------------------------------ */
	LSA_UINT16	ServerHelloResources;	/* FSU channel: Number of receive RQBs in lower  	*/
										/* layer for incoming HELLO requests				*/
										/* ------------------------------------------------ */
	LSA_UINT8	ClientResources;		/* Client channel: Number of RQBs and timers for	*/
										/* outgoing client requests							*/
	LSA_BOOL	HelloEnable;			/* Client channel: Use 10ms HELLO timer				*/
} DCP_DETAIL_TYPE;

/* Values for DevId	in Details
*/
#define DCP_DEV_ETH		0		/* Standard server/client user channel */
#define DCP_DEV_FSU		1		/* FSU server system channel for Hello */

/* start of packed types
*/
#include "pnio_pck2_on.h"

/* Datasets
***********
*/

/* Profile- and Bus parameter for Profibus (Option 129)
 * Profile id and general bus parameters (Dataset 1)
*/
 PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBPROFIL_
{
	LSA_UINT8		 	ProfileId;
	LSA_UINT8		 	NumMasterCl2Allowed;
	LSA_UINT8		 	FdlAdd;
	LSA_UINT8		 	Baudrate;
	LSA_UINT8		 	Tsl[2];
	LSA_UINT8		 	TsdrMin[2];
	LSA_UINT8		 	TsdrMax[2];
	LSA_UINT8		 	Tqui;
	LSA_UINT8		 	Tset;
	LSA_UINT8		 	TTR[4];
	LSA_UINT8		 	G;
	LSA_UINT8		 	Hsa;
	LSA_UINT8		 	MaxRetryLimit;
	LSA_UINT8		 	RetryCtrToken;
	LSA_UINT8		 	TbusCtrlInRing[2];
	LSA_UINT8		 	TbusCtrlOutOfRing[2];
	LSA_UINT8		 	TreportSend[2];

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_PBPROFIL_ DCP_DS_PBPROFIL;
#define DCP_DS_PBPROFIL_SIZE	26

/* Profile- and Bus parameter for Profibus (Option 129)
 * Additional Parameters (Dataset 2) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBPARAM_
{
	LSA_UINT8		 	Tmsi[4];
	LSA_UINT8		 	TmsiReserve[2];
	LSA_UINT8		 	TdpCyc[4];
	LSA_UINT8		 	AcycReqCtr[2];
	LSA_UINT8		 	ModeEquDis;
	LSA_UINT8		 	MasterEquDis;
	LSA_UINT8		 	CycleCtrEquDis;
	LSA_UINT8		 	SynSap;
	LSA_UINT8		 	SynSduLen;
	LSA_UINT8		 	TthEquDis[4];
	LSA_UINT8		 	ModeClockSync;
	LSA_UINT8		 	DelayTimeCtrClockSync;
	LSA_UINT8		 	Max_Tjitter;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_PBPARAM_ DCP_DS_PBPARAM;
#define DCP_DS_PBPARAM_SIZE		24

/* Profile- and Bus parameter for Profibus (Option 129)
 * CommonBusParameters (Dataset 3)
*/
 PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBCOMMON_
{
	LSA_UINT8		 	Syntax_Id[2];
	LSA_UINT8		 	Profile_Id;
	LSA_UINT8		 	Number_Master_Cl2_Allowed;
	LSA_UINT8		 	TS;
	LSA_UINT8		 	Data_rate;
	LSA_UINT8		 	TSL[2];
	LSA_UINT8		 	minTSRD[2];
	LSA_UINT8		 	maxTSDR[2];
	LSA_UINT8		 	TQUI;
	LSA_UINT8		 	TSET;
	LSA_UINT8		 	TTR[4];
	LSA_UINT8		 	G;
	LSA_UINT8		 	HSA;
	LSA_UINT8		 	max_retry_limit;
	LSA_UINT8		 	Retry_Ctr_Token;
	LSA_UINT8		 	Tbus_Control_In_Ring[2];
	LSA_UINT8		 	Tbus_Control_Out_Of_Ring[2];
	LSA_UINT8		 	Treport_send[2];

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_PBCOMMON_ DCP_DS_PBCOMMON;
#define DCP_DS_PBCOMMON_SIZE	28

/* Profile- and Bus parameter for Profibus (Option 129)
 * AdditionalBusParameters (Dataset 4)
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBADDITION_
{
	LSA_UINT8		 	Syntax_Id[2];
	LSA_UINT8		 	Tmsi[4];
	LSA_UINT8		 	Tmsi_Reserve[2];
	LSA_UINT8		 	Tdq_Cyc[4];
	LSA_UINT8		 	Acyc_Req_Ctr[2];
	LSA_UINT8		 	Mode_Equ_Dis;
	LSA_UINT8		 	Master_Equ_Dis;
	LSA_UINT8		 	Cycle_Ctr_Equ_Dis;
	LSA_UINT8		 	Syn_Sap;
	LSA_UINT8		 	Syn_Sdu_Len;
	LSA_UINT8		 	Tth_Equ_Dis[4];
	LSA_UINT8		 	Mode_Clock_Sync;
	LSA_UINT8		 	Delay_Time_Ctr_Clock_Sync;
	LSA_UINT8		 	Max_Tjitter;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_PBADDITION_ DCP_DS_PBADDITION;
#define DCP_DS_PBADDITION_SIZE	26

/* Properties of Router/Gateway (Option 130)
 * Global address informations (Dataset 1) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_RGADDR_
{
	LSA_UINT8		 	Segment[2];		/* Number of physical segment from planning	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_RGADDR_ DCP_DS_RGADDR;
#define DCP_DS_RGADDR_SIZE		2

/* Properties of Router/Gateway (Option 130)
 * Identification of Router/Gateway (Dataset 2) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_RGIDENT_
{
	LSA_UINT8		 	Stamp[2];		/* Counter for changes of the routing table */
	LSA_UINT8		 	Type;			/* Type of net address (-> Length)	 		*/
	LSA_UINT8		 	Reserved;		/*  										*/
	DCP_MAC_ADR		 	Addr;			/* Adress of the router						*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_RGIDENT_SIZE	(4+DCP_MAC_ADR_SIZE)
typedef struct DCP_DS_RGIDENT_ DCP_DS_RGIDENT;

/* Properties of PBC node (Option 128)
 * Switch adjustment (Dataset 1) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBCSWITCH_
{
	LSA_UINT8		 	Filter;			/* Selection of telegram types	*/
	LSA_UINT8		 	Len;			/* Length of Mask Array			*/
	LSA_UINT8		 	Mask[2];		/* Port mask					*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_PBCSWITCH_SIZE	4
typedef struct DCP_DS_PBCSWITCH_  DCP_DS_PBCSWITCH;

/* Properties of PBC node (Option 128)
 * Response behaviour on Identify (Dataset 2) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBCRESP_
{
	LSA_UINT8		 	NdResp;			/* Response option for Identify(PBC_ND)	*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_PBCRESP_SIZE		1
typedef struct DCP_DS_PBCRESP_ DCP_DS_PBCRESP;

/* Properties of PBC node (Option 128)
 * Port properties of Switch (Dataset 3) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBCCONFIG_
{
	LSA_UINT8		 	Len;			/* Length of Port Array		*/
	LSA_UINT8		 	Mask[2];		/* Port mask				*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_PBCCONFIG_SIZE	3
typedef struct DCP_DS_PBCCONFIG_ DCP_DS_PBCCONFIG;

/* Properties ISO/ISO&TCP/TCP node (Option 1)
 * MAC Address (Dataset 1) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_MACADDR_
{
	DCP_MAC_ADR	 	Addr;

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_MACADDR_SIZE		DCP_MAC_ADR_SIZE
typedef struct DCP_DS_MACADDR_ DCP_DS_MACADDR;

/* Properties ISO/ISO&TCP/TCP node (Option 1)
 * TCP/IP Parameter (Dataset 2) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_TCPIP_
{
	DCP_IP_ADR		 	IpAddr;			/* IP Address	  	*/
	DCP_IP_ADR		 	SubnetMask;		/* Subnet mask	  	*/
	DCP_IP_ADR		 	DefaultRouter;	/* Standard Router	*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_TCPIP_SIZE		12
typedef struct DCP_DS_TCPIP_ DCP_DS_TCPIP;

/* DHCP Parameters (Option 3) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_DHCP_
{
	LSA_UINT8			OptionCode;		/* ID of the first option of list		*/
	LSA_UINT8			OptionLen;		/* Length of the first option of list	*/
	LSA_UINT8			OptionVal[1];	/* Value of the first option of list	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_DHCP_ DCP_DS_DHCP;

/* LLDP Parameter (Option 4) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_LLDP_
{
	LSA_UINT8		 	Todo;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_LLDP_ DCP_DS_LLDP;

/* General properties (Option 2)
 * Station type (Dataset 1) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_IDTYPE_
{
	LSA_UINT8		 	String[2];		/* Station type (variable length)	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_IDTYPE_ DCP_DS_IDTYPE;

/* General properties (Option 2)
 * Station name (Dataset 2) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_IDNAME_
{
	LSA_UINT8		 	String[2];		/* Station name (variable length)	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_IDNAME_ DCP_DS_IDNAME;

/* General properties (Option 2)
 * Extended Station type (Dataset 3) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_IDID_
{
	LSA_UINT8		 	Value[2];	   	/* Station identificator (variable length)	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_IDID_ DCP_DS_IDID;

/* General properties (Option 2)
 * ProfinetIO Role (Dataset 4) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_IDROLE_
{
	LSA_UINT8		 	Role;		   	/* Station role (PROFInet IO)		*/
	LSA_UINT8		 	Reserved;		/* Bit field for one or more roles	*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_IDROLE_SIZE		2
typedef struct DCP_DS_IDROLE_ DCP_DS_IDROLE;

/* General properties (Option 2)
 * Device types (Dataset 5) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_IDOPTS_
{
	LSA_UINT16		 	Type[2];		/* Supported Options/Suboptions	*/
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_IDOPTS_ DCP_DS_IDOPTS;

/* Control (Option 5)
 * Signal (Dataset 3) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_SIGNAL_
{
  	LSA_UINT8		ModifierHigh;		/* Modifier (high byte) */
  	LSA_UINT8		ModifierLow; 		/* Modifier (low byte) 	*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_SIGNAL_SIZE		2
typedef struct DCP_DS_SIGNAL_ DCP_DS_SIGNAL;

/* Control (Option 5)
 * Response (Dataset 4) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_RESULT_
{
  	LSA_UINT8		Code;				/* Result code		 		*/
  	LSA_UINT8		AddOn;	 			/* Optional additional data	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_RESULT_ DCP_DS_RESULT;

/* Device Initiative (Option 6)
 * DeviceInitiativeValue (Dataset 1) 
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_DEVINIT_
{
	LSA_UINT16		 	Value;			/* Bit0: Device sends HELLO	*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_DEVINIT_SIZE		2
typedef struct DCP_DS_DEVINIT_ DCP_DS_DEVINIT;

/* ManufacturerSpecificParameterType for CommonBusParameters and    */
/* AdditionalBusParameters                                          */
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_PBMANUFACT_
{
  	LSA_UINT8			OUI[3];			/* ManufacturerOUI			*/
	LSA_UINT8		 	String[1];		/* ManufacturerSpecificString */
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DS_PBMANUFACT_ DCP_DS_PBMANUFACT;

PNIO_PACKED_ATTRIBUTE_PRE union DCP_DS_DATA_
{
	DCP_DS_PBPROFIL		PbProfil;		/* Profibus: Profile ID		*/
	DCP_DS_PBPARAM		PbParam;		/* Profibus: add. parameters*/
	DCP_DS_RGADDR		RgAddr;			/* Router: glob. addr.info	*/
	DCP_DS_RGIDENT		RgIdent;		/* Router: Identification	*/
	DCP_DS_PBCSWITCH	PbcSwitch;		/* PBC 01: Switch			*/
	DCP_DS_PBCRESP		PbcResp;		/* PBC 02: Response			*/
	DCP_DS_PBCCONFIG	PbcConfig;		/* PBC 03: Configuration	*/
	DCP_DS_MACADDR		MacAddr; 		/* ISO/TCP: MAC address		*/
	DCP_DS_TCPIP		TcpIp;			/* ISO/TCP: IP address...	*/
	DCP_DS_DHCP			Dhcp;			/* DHCP option				*/
	DCP_DS_LLDP			Lldp;			/* LLDP option				*/
	DCP_DS_IDTYPE		IdentType;		/* IDENT: station type		*/
	DCP_DS_IDNAME		IdentName;		/* IDENT: station name		*/
	DCP_DS_IDID			IdentId;		/* IDENT: st. identificator */
	DCP_DS_IDROLE		IdentRole;		/* IDENT: role		 		*/
	DCP_DS_IDOPTS		IdentOpts;		/* IDENT: Options/Subopts	*/
	DCP_DS_SIGNAL		Signal;			/* CTRL: Show Location 		*/
	DCP_DS_RESULT		Result;			/* CTRL: Result dataset		*/
	DCP_DS_DEVINIT		DevInit;		/* DeviceInitiative			*/
	DCP_DS_PBMANUFACT	PbManufact;		/* ManufacturerSpecificParameterType */

} PNIO_PACKED_ATTRIBUTE_POST;
typedef union DCP_DS_DATA_ DCP_DS_DATA;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DATASET_IDENT_	
                                        /* without Status!			*/
{
	LSA_UINT8			Opt;			/* Option (Database ID)		*/
	LSA_UINT8			Id;				/* Suboption (Dataset ID)	*/
	LSA_UINT16			Len;			/* Length of dataset 		*/
	DCP_DS_DATA			Data;			/* Variable data part		*/

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_IDENT_HDR_SIZE	4		/* Only Opt, Id, Len		*/
typedef struct DCP_DATASET_IDENT_	DCP_DATASET_IDENT;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DS_HDR_
{
	LSA_UINT8			Opt;			/* Option (Database ID)		*/
	LSA_UINT8			Id;				/* Suboption (Dataset ID)	*/
	LSA_UINT16			Len;			/* Length of dataset 		*/
										/* + Status           		*/
} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_DS_HDR_SIZE	4
#define DCP_DS_RESULT_MIN_SIZE	(DCP_DS_HDR_SIZE + 4)
typedef struct DCP_DS_HDR_ DCP_DS_HDR;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_DATASET_
{
	DCP_DS_HDR			Hdr;			/* Dataset header	  		*/
	LSA_UINT16			Status;			/* Status of dataset  		*/
	DCP_DS_DATA			Data;			/* Variable data part		*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_DATASET_ DCP_DATASET;

/* DCP PDU structure
********************
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_HEADER_
{
	LSA_UINT8		 	ServiceId;		/* Identify, Get, Set...	*/
	LSA_UINT8		 	ServiceType;	/* UC-Req, UC-Rsp, MC-Req	*/
	LSA_UINT16		 	XidHigh;		/* Transaction ID			*/
	LSA_UINT16		 	XidLow;
	LSA_UINT8		 	RspDelayHigh;	/* Response delay (MC-Req)	*/
	LSA_UINT8		 	RspDelayLow;
	LSA_UINT16		 	Length;			/* of data behind header	*/
} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_HEADER_SIZE	10
typedef struct DCP_HEADER_ DCP_HEADER;

/* DCP Request PDU
******************
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_IDENT_REQ_	/* IDENTIFY Request **/
{
	DCP_DATASET_IDENT	Filter;			/* Option/Dataset available	*/
										/* on server (1...n)		*/
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_IDENT_REQ_	DCP_IDENT_REQ;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_GET_REQ_ /* GET Request ********/
{
	LSA_UINT16		Type[2];			/* List of Options/Datasets	*/
										/*  requested (1...n)		*/
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_GET_REQ_	DCP_GET_REQ;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_SET_REQ_ /* SET Request ********/
{
	DCP_DATASET		Dataset;			/* List of Datasets to set	*/
										/* (variable length)		*/
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_SET_REQ_	DCP_SET_REQ;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_HELLO_REQ_	/* HELLO Request ****/
{
	DCP_DATASET		Dataset;			/* Hello dataset list		*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_HELLO_REQ_ DCP_HELLO_REQ;

PNIO_PACKED_ATTRIBUTE_PRE union DCP_REQ_DATA_
{
	LSA_UINT8		Byte[DCP_MAX_REQ_DATA_SIZE];
	DCP_IDENT_REQ	Identify;
	DCP_GET_REQ		Get;
	DCP_SET_REQ		Set;
	DCP_HELLO_REQ	Hello;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef union DCP_REQ_DATA_ DCP_REQ_DATA;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_REQ_
{
	DCP_HEADER		Hdr;
	DCP_REQ_DATA	Data;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_REQ_ DCP_REQ;

/* DCP Response PDU
*******************
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_IDENT_RSP_
{
	DCP_DATASET		IdentDs;			/* Identify dataset list	*/

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_IDENT_RSP_ DCP_IDENT_RSP;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_GET_RSP_
{
	DCP_DATASET		Dataset;			/* Datasets requested		*/
										/* List: variable length	*/ 
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_GET_RSP_ DCP_GET_RSP;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_SET_RSP_
{
	DCP_DATASET		Result;				/* Result list for Error/	*/
										/* Success indication		*/
										/* (variable length)		*/
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_SET_RSP_ DCP_SET_RSP;

PNIO_PACKED_ATTRIBUTE_PRE union DCP_RSP_DATA_
{
	LSA_UINT8		Byte[DCP_MAX_RSP_DATA_SIZE];
	DCP_IDENT_RSP	Identify;
	DCP_GET_RSP		Get;
	DCP_SET_RSP		Set;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef union DCP_RSP_DATA_ DCP_RSP_DATA;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_RSP_
{
	DCP_HEADER		Hdr;
	DCP_RSP_DATA	Data;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_RSP_ DCP_RSP;

PNIO_PACKED_ATTRIBUTE_PRE union DCP_PDU_
{
	DCP_REQ		 	Req;
	DCP_RSP		 	Rsp;

} PNIO_PACKED_ATTRIBUTE_POST;
typedef union DCP_PDU_ DCP_PDU;

/* DCP Ethernet frame structure
*******************************
*/
PNIO_PACKED_ATTRIBUTE_PRE struct DCP_ETHDR_
{
	DCP_MAC_ADR		DstMacAdr;		/* Destination MAC address */
	DCP_MAC_ADR		SrcMacAdr;		/* Source MAC address */
	LSA_UINT8		VlanTag[4];		/* VLAN-Tag */
	LSA_UINT8		EthType[2];		/* Ethertype */
	LSA_UINT8		FrameId[2];		/* Frame-ID */

} PNIO_PACKED_ATTRIBUTE_POST;
#define DCP_ETHDR_SIZE	20
typedef struct DCP_ETHDR_ DCP_ETHDR;

PNIO_PACKED_ATTRIBUTE_PRE struct DCP_ETH_FRAME_
{
	DCP_ETHDR		Hdr;			/* Ethernet Header */
	DCP_PDU  		Pdu;			/* DCP-PDU */

} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct DCP_ETH_FRAME_ DCP_ETH_FRAME;

typedef DCP_ETH_FRAME		DCP_LOWER_ETH_RXMEM_ATTR *	DCP_ETH_RECV_PTR;
typedef DCP_ETH_FRAME		DCP_LOWER_ETH_TXMEM_ATTR *	DCP_ETH_SEND_PTR;
typedef DCP_REQ 			DCP_LOWER_ETH_RXMEM_ATTR *	DCP_ETH_REQ_PTR;
typedef DCP_DATASET			DCP_UPPER_MEM_ATTR 		*	DCP_USR_DATASET_PTR;
typedef DCP_DATASET_IDENT	DCP_UPPER_MEM_ATTR 		*	DCP_USR_DSIDENT_PTR;
typedef DCP_REQ_DATA		DCP_UPPER_MEM_ATTR 		*	DCP_USR_REQ_DATA_PTR;
typedef DCP_RSP_DATA		DCP_UPPER_MEM_ATTR 		*	DCP_USR_RSP_DATA_PTR;

typedef DCP_REQ *		DCP_REQ_PTR;
typedef DCP_RSP *		DCP_RSP_PTR;
typedef DCP_PDU *		DCP_PDU_PTR;

/* end of packed types
*/

#include "pnio_pck_off.h"


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/


/*=============================================================================
 * function name:  dcp_init
 *
 * function:       initialize dcp
 *
 * parameters:     ....
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  DCP_SYSTEM_IN_FCT_ATTR  dcp_init(
    LSA_VOID
    /* TYPE1  var1,
     * TYPE2  var2,
     * and so on
     */
);

/*=============================================================================
 * function name:  dcp_undo_init
 *
 * function:       undo the initialization of dcp
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_UINT16  DCP_SYSTEM_IN_FCT_ATTR  dcp_undo_init(
    LSA_VOID
);

/*=============================================================================
 * function name:  dcp_timeout
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_UINT16        timer_id:  id of timer
 *                 LSA_USER_ID_TYPE  user_id:   id of dcp
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_timeout(
    LSA_UINT16        timer_id,
    LSA_USER_ID_TYPE  user_id
);

/*=============================================================================
 * function name:  dcp_upper_rqb
 *
 * function:       asynchronous allocation of an upper RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of RQB
 *                 DCP_UPPER_RQB_PTR_TYPE     upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_upper_rqb(
    LSA_USER_ID_TYPE                            user_id,
    LSA_UINT16                                  length,
    DCP_UPPER_RQB_PTR_TYPE                      upper_rqb_ptr
);

/*=============================================================================
 * function name:  dcp_upper_mem
 *
 * function:       asynchronous allocation of an upper-memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of dcp
 *                 LSA_UINT16                 length:         length of memory
 *                 DCP_UPPER_MEM_PTR_TYPE     upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  DCP_SYSTEM_IN_FCT_ATTR  dcp_upper_mem(
    LSA_USER_ID_TYPE                            user_id,
    LSA_UINT16                                  length,
    DCP_UPPER_MEM_PTR_TYPE                      upper_mem_ptr
);

/*====  out functions  =====*/

/*=============================================================================
 * function name:  DCP_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     DCP_UPPER_CALLBACK_FCT_PTR_TYPE 
 *                                            dcp_request_upper_done_ptr:
 *                     pointer to dcp_request_upper_done_ptr, given by
 *                     dcp_open_channel()
 *
 *                 DCP_UPPER_RQB_PTR_TYPE     upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef DCP_REQUEST_UPPER_DONE
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR    DCP_REQUEST_UPPER_DONE(
    DCP_UPPER_CALLBACK_FCT_PTR_TYPE  dcp_request_upper_done_ptr,
    DCP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                 sys_ptr
);
#endif

#ifdef DCP_DO_TIMEOUT_EXTERN
/*=============================================================================
 * function name:  DCP_DO_TIMER_REQUEST     
 *
 * function:       Does a dcp_system() request with the spezified RQB. This
 *                 function is called within DCP timer handler to signal
 *                 a Timer event to the main context of DCP.  
 *                 Systemadaption has to call dcp_system() with the specified
 *                 RQB. Before calling, the systemadaption should change to 
 *                 taskstate. 
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE   pRQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_DO_TIMER_REQUEST
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_DO_TIMER_REQUEST(
    DCP_UPPER_RQB_PTR_TYPE  pRQB
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper RQB for local use 
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE  ...  *  upper_rqb_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory 
 *                                                 available
 *                 LSA_UINT16                      length:   length of RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_UPPER_RQB_LOCAL(
    DCP_UPPER_RQB_PTR_TYPE  DCP_LOCAL_MEM_ATTR *  upper_rqb_ptr_ptr,
    LSA_UINT16                                    length
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_UPPER_RQB_LOCAL
 *
 * function:       free an upper RQB allocated with DCP_ALLOC_UPPER_RQB_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper RQB or
 *                 DCP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_UPPER_RQB_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *  ret_val_ptr,
    DCP_UPPER_RQB_PTR_TYPE                           upper_rqb_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_UPPER_MEM_LOCAL
 *
 * function:       allocate an upper memory block for local use 
 *
 * parameters:     DCP_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory 
 *                                                 available
 *                 LSA_UINT16                      length:   length of memory block
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_UPPER_MEM_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_UPPER_MEM_LOCAL(
    DCP_UPPER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *  upper_mem_ptr_ptr,
    LSA_UINT16                                    length
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_UPPER_MEM_LOCAL
 *
 * function:       free an upper memory block allocated with DCP_ALLOC_UPPER_MEM_LOCAL
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper memory block
 *                 DCP_UPPER_MEM_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                         memory block
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_UPPER_MEM_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_UPPER_MEM_LOCAL(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *  ret_val_ptr,
    DCP_UPPER_MEM_PTR_TYPE                           upper_mem_ptr
);
#endif
#endif /* DCP_DO_TIMEOUT_EXTERN */

/*=============================================================================
 * function name:  DCP_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper RQB
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE  ...  *     upper_rqb_ptr_ptr:
 *                                   return value: pointer to upper RQB pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_upper_rqb().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of RQB
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_UPPER_RQB
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_UPPER_RQB(
    DCP_UPPER_RQB_PTR_TYPE  DCP_LOCAL_MEM_ATTR *       upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                                   user_id,
    LSA_UINT16                                         length,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_UPPER_RQB
 *
 * function:       free an upper RQB
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper RQB or
 *                                                      wrong system pointer
 *                 DCP_UPPER_RQB_PTR_TYPE     upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_UPPER_RQB
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_UPPER_RQB(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *   ret_val_ptr,
    DCP_UPPER_RQB_PTR_TYPE                            upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_UPPER_MEM
 *
 * function:       allocate an upper-memory
 *
 * parameters:     DCP_UPPER_MEM_PTR_TYPE  ...  *     upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: The allocation
 *                                                 will be asynchronous, see
 *                                                 dcp_upper_mem().
 *                 LSA_USER_ID_TYPE                   user_id:  id of dcp
 *                 LSA_UINT16                         length:   length of
 *                                                              memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_UPPER_MEM(
    DCP_UPPER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *       upper_mem_ptr_ptr,
    LSA_USER_ID_TYPE                                   user_id,
    LSA_UINT16                                         length,
    LSA_SYS_PTR_TYPE                                   sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_UPPER_MEM
 *
 * function:       free an upper-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      upper-memory or
 *                                                      wrong system pointer
 *                 DCP_UPPER_MEM_PTR_TYPE     upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *                 LSA_SYS_PTR_TYPE           sys_ptr:        system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_UPPER_MEM(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *   ret_val_ptr,
    DCP_UPPER_MEM_PTR_TYPE                            upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_INIT_SYS_PTR
 *
 * function:       initialize system pointer
 *
 *                 Attention:
 *                 The LSA component only may call this macro when the 
 *                 component hasn't called DCP_GET_PATH_INFO() yet.
 *                 The real value assignment then must be made by the call of
 *                 DCP_GET_PATH_INFO().
 *
 * parameters:     LSA_SYS_PTR_TYPE  ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_INIT_SYS_PTR
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_INIT_SYS_PTR(
    LSA_SYS_PTR_TYPE  DCP_LOCAL_MEM_ATTR *  sys_ptr_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_SYS_PTR_IS_INITIALIZED
 *
 * function:       Is the system pointer initialized
 *                 (by DCP_INIT_SYS_PTR())?
 *
 * parameters:     LSA_BOOL  ...  *  ret_val_ptr:
 *                                        return value: LSA_TRUE   yes
 *                                                      LSA_FALSE  no
 *                 LSA_SYS_PTR_TYPE  sys_ptr:   system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_SYS_PTR_IS_INITIALIZED
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_SYS_PTR_IS_INITIALIZED(
    LSA_BOOL          DCP_LOCAL_MEM_ATTR *  ret_val_ptr,
    LSA_SYS_PTR_TYPE                           sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_GET_PATH_INFO
 *
 * function:       get system pointer and detail pointer of a communication 
 *                 channel
 *
 * parameters:     LSA_UINT16              ...  *  ret_val_ptr:
 *                                      return value: LSA_RET_OK
 *                                                    LSA_RET_ERR_SYS_PATH
 *                 LSA_SYS_PTR_TYPE        ...  *  sys_ptr_ptr:
 *                                      return value: pointer to system pointer
 *                 DCP_DETAIL_PTR_TYPE     ...  *  detail_ptr_ptr:
 *                                      return value: pointer to detail pointer
 *                 LSA_SYS_PATH_TYPE               path: path of a
 *                                                       communication channel  
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_GET_PATH_INFO
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_GET_PATH_INFO(
    LSA_UINT16              DCP_LOCAL_MEM_ATTR *     ret_val_ptr,
    LSA_SYS_PTR_TYPE        DCP_LOCAL_MEM_ATTR *     sys_ptr_ptr,
    DCP_DETAIL_PTR_TYPE     DCP_LOCAL_MEM_ATTR *     detail_ptr_ptr,
    LSA_SYS_PATH_TYPE                                path
);
#endif

/*=============================================================================
 * function name:  DCP_RELEASE_PATH_INFO
 *
 * function:       release system pointer and detail pointer of a communication 
 *                 channel
 *
 * parameters:     LSA_UINT16      ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no release because of
 *                                                      wrong system pointer or
 *                                                      wrong detail pointer
 *                 LSA_SYS_PTR_TYPE        sys_ptr:     system pointer
 *                 DCP_DETAIL_PTR_TYPE  detail_ptr:     detail pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_RELEASE_PATH_INFO
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_RELEASE_PATH_INFO(
    LSA_UINT16              DCP_LOCAL_MEM_ATTR *         ret_val_ptr,
    LSA_SYS_PTR_TYPE                                     sys_ptr,
    DCP_DETAIL_PTR_TYPE                                  detail_ptr
);
#endif

#ifdef DCP_LOCAL_MEM_DYNAMIC
/*=============================================================================
 * function name:  DCP_ALLOC_LOCAL_MEM
 *
 * function:       allocate a local-memory
 *
 * parameters:     DCP_LOCAL_MEM_PTR_TYPE  ...  *  local_mem_ptr_ptr:
 *                                   return value: pointer to local-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: No memory 
 *                                                 available
 *                 LSA_UINT16                         length: length of memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_LOCAL_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_LOCAL_MEM(
    DCP_LOCAL_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *        local_mem_ptr_ptr,
    LSA_UINT16                                          length
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_LOCAL_MEM
 *
 * function:       free a local-memory
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                     return value: LSA_RET_OK         ok
 *                                   LSA_RET_ERR_PARAM  no deallocation because
 *                                                      of wrong pointer to
 *                                                      local-memory
 *                 DCP_LOCAL_MEM_PTR_TYPE  local_mem_ptr:     pointer to local-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_LOCAL_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_LOCAL_MEM(
    LSA_UINT16                 DCP_LOCAL_MEM_ATTR *   ret_val_ptr,
    DCP_LOCAL_MEM_PTR_TYPE                            local_mem_ptr
);
#endif
#endif

/*=============================================================================
 * function name:  DCP_MEMSET_LOCAL
 *
 * function:       fills memory with value
 *
 * parameters:      DCP_LOCAL_MEM_PTR_TYPE         pMem
 *                  LSA_UINT8                      Value
 *                  LSA_UINT32                     Length
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_MEMSET_LOCAL
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_MEMSET_LOCAL(
    DCP_LOCAL_MEM_PTR_TYPE              pMem,                       
    LSA_UINT8                           Value,
    LSA_UINT32                          Length
);
#endif

/*=============================================================================
 * function name:  DCP_LOWER_TO_UPPER_MEM
 *
 * function:       convert lower-memory to upper-memory
 *
 * parameters:     DCP_UPPER_MEM_PTR_TYPE  ...  *     upper_mem_ptr_ptr:
 *                                   return value: pointer to upper-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 DCP_LOWER_MEM_PTR_TYPE             lower_mem_ptr: 
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_LOWER_TO_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_LOWER_TO_UPPER_MEM(
    DCP_UPPER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *          upper_mem_ptr_ptr,
    DCP_LOWER_MEM_PTR_TYPE                                lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_UPPER_TO_LOWER_MEM
 *
 * function:       convert upper-memory to lower-memory
 *
 * parameters:     DCP_LOWER_MEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 DCP_UPPER_MEM_PTR_TYPE             upper_mem_ptr:  
 *                                                 pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                   sys_ptr:  system pointer
 *                                                              
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_UPPER_TO_LOWER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_UPPER_TO_LOWER_MEM(
    DCP_LOWER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *          lower_mem_ptr_ptr,
    DCP_UPPER_MEM_PTR_TYPE                                upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_UPPER_TO_ETH_MEM
 *
 * function:       convert upper-memory to lower-ethernet-memory
 *
 * parameters:     DCP_ETH_MEM_PTR_TYPE  ...  *    eth_mem_ptr_ptr:
 *                                   return value:   pointer to lower-ethernet-
 *                                                   memory pointer
 *                                                   or LSA_NULL: conversion
 *                                                   wasn't possible 
 *                 DCP_UPPER_MEM_PTR_TYPE          upper_mem_ptr:  
 *                                                   pointer to upper-memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system pointer
 *                                                              
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_UPPER_TO_ETH_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_UPPER_TO_ETH_MEM(
    DCP_ETH_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *            eth_mem_ptr_ptr,
    DCP_UPPER_MEM_PTR_TYPE                                upper_mem_ptr,
    LSA_SYS_PTR_TYPE                                      sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ETH_TO_UPPER_MEM
 *
 * function:       convert lower ethernet memory to upper memory
 *
 * parameters:     DCP_UPPER_MEM_PTR_TYPE  ...  *  upper_mem_ptr_ptr:
 *                                   return value:   pointer to upper
 *                                                   memory pointer
 *                                                   or LSA_NULL: conversion
 *                                                   wasn't possible 
 *                 DCP_UPPER_MEM_PTR_TYPE          eth_mem_ptr:  
 *                                                   pointer to lower ethernet 
 *                                                   memory
 *                 LSA_SYS_PTR_TYPE                sys_ptr:  system pointer
 *                                                              
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ETH_TO_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ETH_TO_UPPER_MEM(
    DCP_UPPER_MEM_PTR_TYPE	DCP_LOCAL_MEM_ATTR *		upper_mem_ptr_ptr,
    DCP_ETH_MEM_PTR_TYPE								eth_mem_ptr,
    LSA_SYS_PTR_TYPE									sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_LOWER_TO_LOWER_MEM
 *
 * function:       convert lower-memory to lower-memory
 *
 * parameters:     DCP_LOWER_MEM_PTR_TYPE  ...  *     lower_mem_ptr_ptr:
 *                                   return value: pointer to lower-memory-
 *                                                 pointer
 *                                                 or LSA_NULL: conversion
 *                                                 wasn't possible 
 *                 DCP_LOWER_MEM_PTR_TYPE             lower_mem_ptr: 
 *                                                 pointer to lower-memory
 *                 LSA_SYS_PTR_TYPE                   src_sys_ptr: 
 *                                                 system pointer of
 *                                                 source-memory
 *                 LSA_SYS_PTR_TYPE                   dst_sys_ptr:
 *                                                 system pointer of
 *                                                 destination-memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_LOWER_TO_LOWER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_LOWER_TO_LOWER_MEM(
    DCP_LOWER_MEM_PTR_TYPE  DCP_LOCAL_MEM_ATTR *          lower_mem_ptr_ptr,
    DCP_LOWER_MEM_PTR_TYPE                                lower_mem_ptr,
    LSA_SYS_PTR_TYPE                                      src_sys_ptr,
    LSA_SYS_PTR_TYPE                                      dst_sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_COPY_COMMON_MEM
 *
 * function:       copy common memory
 *                 The memory areas of source and destination mustn't overlap.
 *
 * parameters:     LSA_VOID  DCP_COMMON_MEM_ATTR *     src_common_mem_ptr:
 *                                                 void pointer to source-
 *                                                 common-memory
 *                 LSA_VOID  DCP_COMMON_MEM_ATTR *     dst_common_mem_ptr:
 *                                                 void pointer to destination-
 *                                                 common-memory
 *                 LSA_UINT16                          length: number of bytes
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_COPY_COMMON_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_COPY_COMMON_MEM(
    LSA_VOID  DCP_COMMON_MEM_ATTR *                    src_common_mem_ptr,
    LSA_VOID  DCP_COMMON_MEM_ATTR *                    dst_common_mem_ptr,
    LSA_UINT16                                         length
);
#endif

/*=============================================================================
 * function name:  DCP_COPY_UPPER_TO_LOCAL_MEM
 *
 * function:       copy upper data to local memory
 *
 * parameters:     DCP_UPPER_MEM_PTR_TYPE	src_ptr:	pointer to upper memory
 *                 DCP_LOCAL_MEM_PTR_TYPE	dst_ptr:	pointer to local memory
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_COPY_UPPER_TO_LOCAL_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR	DCP_COPY_UPPER_TO_LOCAL_MEM(
    DCP_UPPER_MEM_PTR_TYPE	 		src_ptr,
    DCP_LOCAL_MEM_PTR_TYPE			dst_ptr,
    LSA_UINT16              		length,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_COPY_LOCAL_TO_UPPER_MEM
 *
 * function:       copy local data to upper memory
 *
 * parameters:     DCP_LOCAL_MEM_PTR_TYPE	src_ptr:	pointer to local memory
 *                 DCP_UPPER_MEM_PTR_TYPE	dst_ptr:	pointer to upper memory
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_COPY_LOCAL_TO_UPPER_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR	DCP_COPY_LOCAL_TO_UPPER_MEM(
    DCP_LOCAL_MEM_PTR_TYPE	 		src_ptr,
    DCP_UPPER_MEM_PTR_TYPE			dst_ptr,
    LSA_UINT16              		length,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_COPY_RXETH_TO_LOCAL_MEM
 *
 * function:       copy lower PDU data to local memory
 *
 * parameters:     DCP_ETH_RXVOID_PTR_TYPE	src_ptr:	pointer to lower
 *                                                  	receive PDU memory
 *                 DCP_LOCAL_MEM_PTR_TYPE	dst_ptr:	pointer to local memory
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_COPY_RXETH_TO_LOCAL_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR	DCP_COPY_RXETH_TO_LOCAL_MEM(
    DCP_ETH_RXVOID_PTR_TYPE		 	src_ptr,
    DCP_LOCAL_MEM_PTR_TYPE			dst_ptr,
    LSA_UINT16              		length,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_COPY_LOCAL_TO_ETHTX_MEM
 *
 * function:       copy local memory data to lower PDU
 *
 * parameters:     DCP_LOCAL_MEM_PTR_TYPE	src_ptr:	pointer to local memory
 *                 DCP_ETH_TXVOID_PTR_TYPE	dst_ptr:	pointer to lower
 *                                                  	send PDU memory
 *                 
 *                 LSA_UINT16               length: 	number of bytes to copy
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_COPY_LOCAL_TO_ETHTX_MEM
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR	DCP_COPY_LOCAL_TO_ETHTX_MEM(
    DCP_LOCAL_MEM_PTR_TYPE			src_ptr,
    DCP_ETH_TXVOID_PTR_TYPE		 	dst_ptr,
    LSA_UINT16              		length,
    LSA_SYS_PTR_TYPE				sys_ptr
);
#endif

/*=============================================================================
 * function name:  DCP_ALLOC_TIMER
 *
 * function:       allocate a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                           return value: LSA_RET_OK            timer has been
 *                                                               allocated 
 *                                         LSA_RET_ERR_NO_TIMER  no timer has 
 *                                                               been allocated 
 *                 LSA_UINT16  ...  *  timer_id_ptr:
 *                           return value: pointer to id of timer
 *                 LSA_UINT16          timer_type:  LSA_TIMER_TYPE_ONE_SHOT or
 *                                                  LSA_TIMER_TYPE_CYCLIC
 *                 LSA_UINT16          time_base:   LSA_TIME_BASE_1US,
 *                                                  LSA_TIME_BASE_10US,
 *                                                  LSA_TIME_BASE_100US,
 *                                                  LSA_TIME_BASE_1MS,
 *                                                  LSA_TIME_BASE_10MS,
 *                                                  LSA_TIME_BASE_100MS,
 *                                                  LSA_TIME_BASE_1S,
 *                                                  LSA_TIME_BASE_10S or
 *                                                  LSA_TIME_BASE_100S
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ALLOC_TIMER
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ALLOC_TIMER(
    LSA_UINT16  DCP_LOCAL_MEM_ATTR *               ret_val_ptr,
    LSA_UINT16  DCP_LOCAL_MEM_ATTR *               timer_id_ptr,
    LSA_UINT16                                     timer_type,
    LSA_UINT16                                     time_base
);
#endif

/*=============================================================================
 * function name:  DCP_START_TIMER
 *
 * function:       start a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                  timer has been
 *                                                             started
 *                                 LSA_RET_OK_TIMER_RESTARTED  timer has been
 *                                                             restarted
 *                                 LSA_RET_ERR_PARAM           timer hasn't
 *                                                             been started
 *                                                             because of wrong
 *                                                             timer-id
 *                                 After the expiration of the running time 
 *                                 system will call dcp_timeout().
 *                 LSA_UINT16          timer_id:  id of timer
 *                 LSA_USER_ID_TYPE    user_id:   id of dcp
 *                 LSA_UINT16          time:      running time
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_START_TIMER
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_START_TIMER(
    LSA_UINT16        DCP_LOCAL_MEM_ATTR *         ret_val_ptr,
    LSA_UINT16                                     timer_id,
    LSA_USER_ID_TYPE                               user_id,
    LSA_UINT16                                     time
);
#endif

/*=============================================================================
 * function name:  DCP_STOP_TIMER
 *
 * function:       stop a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                   return value: LSA_RET_OK                    timer has been
 *                                                               stopped
 *                                 LSA_RET_OK_TIMER_NOT_RUNNING  timer was not
 *                                                               running; timer
 *                                                               is stopped
 *                                 LSA_RET_ERR_PARAM             timer hasn't
 *                                                               stopped
 *                                                               because
 *                                                               of wrong 
 *                                                               timer-id
 *                 LSA_UINT16          timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_STOP_TIMER
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_STOP_TIMER(
    LSA_UINT16  DCP_LOCAL_MEM_ATTR *              ret_val_ptr,
    LSA_UINT16                                    timer_id
);
#endif

/*=============================================================================
 * function name:  DCP_FREE_TIMER
 *
 * function:       free a timer
 *
 * parameters:     LSA_UINT16  ...  *  ret_val_ptr:
 *                return value: LSA_RET_OK                    timer has been
 *                                                            deallocated
 *                              LSA_RET_ERR_TIMER_IS_RUNNING  because timer is
 *                                                            running timer has
 *                                                            not been
 *                                                            deallocated
 *                              LSA_RET_ERR_PARAM             no deallocation
 *                                                            because of wrong
 *                                                            timer-id
 *                 LSA_UINT16          timer_id:  id of timer
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FREE_TIMER
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FREE_TIMER(
    LSA_UINT16  DCP_LOCAL_MEM_ATTR *              ret_val_ptr,
    LSA_UINT16                                    timer_id
);
#endif

/*=============================================================================
 * function name:  DCP_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_ENTER
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_ENTER(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  DCP_EXIT
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_EXIT
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_EXIT(
    LSA_VOID
);
#endif

/*=============================================================================
 * function name:  DCP_FATAL_ERROR
 *
 * function:       notify a fatal-error
 *
 * parameters:     LSA_UINT16                   length:     number of bytes
 *                 DCP_FATAL_ERROR_PTR_TYPE  error_ptr:  pointer to
 *                                                          LSA_FATAL_ERROR
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef DCP_FATAL_ERROR
LSA_VOID  DCP_SYSTEM_OUT_FCT_ATTR  DCP_FATAL_ERROR(
    LSA_UINT16                                     length,
    DCP_FATAL_ERROR_PTR_TYPE                       error_ptr
);
#endif

/*****************************************************************************/
/*  end of file DCP_SYS.H                                                    */
/*****************************************************************************/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of DCP_SYS_H */
