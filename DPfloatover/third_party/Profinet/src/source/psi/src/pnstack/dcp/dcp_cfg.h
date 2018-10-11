#ifndef DCP_CFG_H                        /* ----- reinclude-protection ----- */
#define DCP_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: dcp_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                      Frame for file "dcp_cfg.h".                          */
/*                      ==============================                       */
/*                                                                           */
/*  Configuration for dcp:                                                   */
/*  Defines constants, types and macros for dcp.                             */
/*                                                                           */
/*  The current values are EXAMPLES - working for Win32 based module test    */ 
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */ 
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
/*              Macros for lower RQB access moved here from dcp_low.h        */
/*  2003-01-27  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              Macros for upper RQB access moved here from dcp_usr.h        */
/*  2003-02-13  P01.00.00.00_00.03.00.01  lrg  new:DCP_REQ_LOCAL_DATA        */
/*              Macros for lower CANCEL function added                       */
/*              DCP_SPH_TCPIP_STATUS added                                   */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*                                             neu:  DCP_SERVER_PBC          */
/*  2003-06-03  P01.00.00.00_00.06.00.00  lrg  6.Increment:                  */
/*              DCP_READ_TCPIP_STATUS removed (SPH defines more than 0001)   */
/*              DCP_COPY_RECV_TO_LOCAL_MEM removed                           */
/*  2003-07-01  P01.00.00.00_00.07.00.00  lrg  7.Increment:                  */
/*              DCP_MAC_ADR_SIZE and DCP_IP_ADR_SIZE moved from dcp_sys.h    */
/*  2003-07-16  P01.00.00.00_00.07.01.00  lrg  LSA_GLOB 2.1.0.0_0.2.1.1      */
/*  2003-07-16  P01.00.00.00_00.07.02.00  lrg  neu: DCP_SRV_RSP_OWN_HDR      */
/*              mit zugehoerigen Ethernet-Header Werten                      */
/*  2003-07-30  P01.00.00.00_00.08.00.00  lrg  8.Increment: Client channel   */
/*  2003-08-28  P01.00.00.00_00.09.00.00  lrg  9.Increment: Profibus DB      */
/*  2003-09-29  P01.00.00.00_00.11.00.00  lrg 11.Increment: Mehrere IDENTIFY */
/*              gleichzeitig durch Timer-Array. Anzahl konfigurierbar durch  */
/*              DCP_NUM_SERVER_IDENTIFY_TIMER und 							 */
/*              DCP_NUM_CLIENT_IDENTIFY_TIMER                                */
/*  2003-10-17  P01.00.00.00_00.11.00.01  lrg  Traces disabled               */
/*  2003-11-04  P02.00.00.00_00.01.00.00  lrg  1.Increment Version 2: 		 */
/*              PT2_MODE entfernt                                            */
/*  2003-12-15  P02.00.00.00_00.01.00.00  dsc  New version of timeout hand-  */
/*              ling introduced: #define DCP_DO_TIMEOUT_EXTERN               */
/*              Neu: #define DCP_TYPEDEF_UPPER_RQB_PTR (fuer dsc ;-)         */   
/*  2004-02-09  P02.00.00.00_00.02.00.00  lrg  DCP_BIG_ENDIAN for swapping   */
/*  2004-02-12  P02.00.00.00_00.02.00.01  lrg  Not define DCP_SERVER_MODE    */
/*              now possible. New: DCP_UPPER_RQB_USERID_GET()                */   
/*              DCP_ETH_RQB_USER_PTR_SET/GET() UserId -> user_id             */   
/*              removed: #define DCP_SPH_TCPIP_STATUS                        */
/*  2004-03-29  P02.00.00.00_00.02.00.02  lrg  DCP_MAX_CHANNELS dependend of */
/*              DCP_CLIENT_MODE, DCP_SERVER_MODE and DCP_MAX_LOW_CHANNELS    */   
/*  2004-04-16  P02.00.00.00_00.02.05.02  lrg  nested comments removed       */
/*  2004-04-28  P02.00.00.00_00.02.05.04  lrg  DCP_ETH_ERR_TIMEOUT           */
/*  2004-05-03  P02.00.00.00_00.02.05.05  lrg DCP_MAX_RSP_DATA_SIZE und      */
/*              DCP_MAX_REQ_DATA_SIZE verschoben von dcp_usr.h               */
/*  2004-06-23  P03.00.00.00_00.01.00.00  lrg Version 3 start                */
/*  2004-10-11  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*              RQB-Zugriffsmakros angepasst: DCP_UPPER_RQB_FILTER_PTR_SET   */
/*              111004lrg001 Makro zum Loeschen der Multicastadresse         */
/*  2004-12-17  P03.02.00.00_00.02.02.00  lrg  171204lrg001: New macro       */
/*              DCP_ETH_RQB_RECV_CNT_GET() to read RequestCount from RQB     */
/*              DCP_UPPER_RQB_USERID_SET(): .uvar32 entfernt                 */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  LSA_GLOB 2.1.0.0_0.4.2.1      */
/*              DCP_NUM_CLIENT_REQUEST_TIMER auf 10 und beschrieben          */
/*  2005-02-16  P03.04.00.00_00.04.00.00  lrg  LSA-Trace eingebaut           */
/*  2005-04-15  P03.04.00.00_00.04.00.00  lrg  currently not used            */
/*              DCP_INIT_SYS_PTR and DCP_SYS_PTR_IS_INITIALIZED              */
/*  2005-06-10  P03.06.00.00_00.06.00.00  lrg  EDD_RQB_GET/SET_STATUS        */
/*              ersetzt durch EDD_RQB_GET/SET_RESPONSE                       */
/*  2005-12-09  P03.09.00.00_00.01.00.00  lrg  upper RQB macros moved to     */
/*              dcp_usr.h                                                    */
/*  2006-06-02  P03.11.00.00_00.01.01.02  lrg ARTS-RQ 00291071:  mandatory   */
/*              upper RQB macros moved back to dcp_cfg.txt/h.                */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2006-11-24  P03.12.00.00_00.30.01.02  lrg  ArtsRQ AP00372097             */
/*              DCP_ETH_RQB_COMPID_GET replaced by DCP_ETH_RQB_ERR_GET       */
/*  2007-02-20  P04.00.00.00_00.01.01.02  lrg  DCP_NUM_CLIENT_HELLO_RQB      */
/*              DCP_NUM_SERVER_IDENTIFY_TIMER -> ...IDENTIFY_RESOURCES       */
/*  2007-03-29  P04.00.00.00_00.01.01.03  lrg  DCP_NUM_CLIENT_..._RESOURCES  */
/*              sizeof(ip_addr) -> EDD_IP_ADDR_SIZE                          */
/*  2007-04-24  P04.00.00.00_00.10.01.02 240407lrg001:DCP_MAX_LOW_CHANNELS=1 */
/*              DCP_NUM_CLIENT_HELLO_RQB entfernt                            */
/*  2007-06-20  P04.00.00.00_00.20.01.02  lrg  removed: DCP_SERVER_PBC,      */
/*              DCP_OPTION_PBC and DCP_OPTION_PB                             */
/*  2007-07-23  P04.00.00.00_00.20.03.02  lrg  DCP_CLIENT_DEVICE_FSU_MIN     */
/*  2007-11-14  P04.00.00.00_00.50.01.02 141107lrg001: upper RQB LSA macros  */
/*              Tabs removed from this history                               */
/*              DCP_CLIENT_DEVICE_FSU_MIN -> DCP_CLIENT_DEVICE_MIN           */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2009-01-27  P04.01.00.00_00.02.01.02 270109lrg001: Casts for C++,        */
/*              TxFrmGroup                                                   */
/*  2010-08-24  P05.00.00.00_00.01.01.02 240810lrg001: AP00999022 UPPER_DONE */
/*              AP00999104 DCP_CFG_TRACE_MODE, private trace removed         */
/*  2010-11-29  P05.00.00.00_00.02.01.02 231110lrg001: AP00988516            */
/*              DCP-Identify-Filter on confirmation                          */
/*  2011-01-18  P05.00.00.00_00.03.12.02 lrg: AP01017495 Index-Trace         */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: AP01205450:              */
/*              Struktur err aus RQB entfernen                               */
/*  2012-02-09  T05.01.00.00_00.05.26.01                                     */
/*              AP01297962: remove DCP_CLIENT_MODE and DCP_SERVER_MODE       */
/*  2012-03-08  S05.02.00.00_00.01.16.01 AP01138373: DCP_ETH_FILTER_SET      */
/*              Entfernen des DCP Identify Filter Service in DCP             */
/*  2013-03-04  P05.02.00.00_00.04.17.02 AP01503994: free upper 2 bit of XID */
/*              DCP_MAX_CHANNELS = 63, lower RQB Makros dcp_cfg.h->dcp_low.h */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01565431 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2014-05-26  P05.04.00.00_00.01.09.01 1027120 Korrektur der PSI-Konfigu-  */
/*              ration im DPC bezüglich DCP_DO_TIMEOUT_EXTERN                */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/* AP01565431 PSI: DCP channel details und Konfigurationsschalter bereinigen
 * --> move these constants to the apprpriate place in PSI files to fill DCP and OHA details
*/
#define PSI_DCP_CLIENT_RESOURCES	PSI_MAX_CONNECTS_PER_SECOND	// max. number of concurrent client services GET+SET+IDENTIFY
#define PSI_DCP_HELLO_RESOURCES		16	// max. number of concurrent server HELLO requests

void dcp_psi_startstop( int start );

/* check if condition holds or call fatal (programming error) */
#if PSI_DEBUG
#define DCP_ASSERT(cond_) { if (!(cond_)) { PSI_FATAL_COMP( LSA_COMP_ID_DCP, DCP_MODULE_ID,0 ) } }
#else
#define DCP_ASSERT(cond_) { /* empty-block in release version */ }
#endif

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

/* If you have a computer system whitch stors most significant byte at the
 * lowest address of the word/doubleword: define this 
*/
#ifdef LSA_HOST_ENDIANESS_BIG
#define DCP_BIG_ENDIAN
#endif

/*=============================================================================
 *       set local_mem static or dynamic:
 * static (no use of DCP_ALLOC_LOCAL_MEM):                
 * or dynamic (use of DCP_ALLOC_LOCAL_MEM):
 *===========================================================================*/
/* DCP_ALLOC_LOCAL_MEM is used by DCP to allocate timer control blocks per channel
*/ 
#undef DCP_LOCAL_MEM_STATIC
#define DCP_LOCAL_MEM_DYNAMIC

/*=============================================================================
 *       clear global data:
 * clear (DCP_DATA_TYPE dcp_data = {0};):                                      
#define DCP_CLEAR_GLOBAL_DATA                                          
 * or no clear:                                      
 *===========================================================================*/
#define DCP_NO_CLEAR_GLOBAL_DATA

/* Place of the structure for local copy and temporary buffer
 * of DCP requests and responses for server and client
 * (sizeof DCP_ETH_FRAME plus DCP_MAX_REQ_DATA_SIZE or DCP_MAX_RSP_DATA_SIZE) 
 * if NOT defined: on stack
#define DCP_REQ_LOCAL_DATA
*/ 

/*---------------------------------------------------------------------------*/
/* Advanced protection and function handling settings.                       */
/*                                                                           */
/* DCP is designed to use the input function dcp_timeout() without           */
/* DCP_ENTER() and DCP_EXIT() even if called from a priority above standard  */
/* handling 1). The requirement for this is, that this function can not be   */
/* intercepted by standard handling.                                         */
/* If this requirement is not met, additional protection with DCP_ENTER()    */
/* and DCP_EXIT() is neccessary. If so the following defines may have to     */
/* be used to include protection within this function.                       */
/* #undef DCP_DO_TIMEOUT_EXTERN is a little special, because it              */
/* simplyfies some handing if no context-change is neccessary.               */
/* #define DCP_DO_TIMEOUT_EXTERN is the more general case, if a context      */
/* change for timeout handling is necessary.                                 */
/*                                                                           */
/* DCP_DO_TIMEOUT_EXTERN:                                                    */
/*      #undef:  dcp_timeout() calls DCP_ENTER() and DCP_EXIT() to protect   */
/*               itself against interruption because DCP directly calls the  */
/*               internal timer handling without context change.             */
/*      #define: dcp_timeout() does not call DCP_ENTER() and DCP_EXIT() be-  */
/*               cause DCP uses DCP_DO_TIMER_REQUEST within dcp_timeout() to */
/*               do a context change. Within dcp_timeout() very little is    */
/*               done, to be able to call the function from interrupt con-   */
/*               text, too.                                                  */
/*                                                                           */
/* Defaults:                                                                 */
/*                                                                           */
/*  #define DCP_DO_TIMEOUT_EXTERN                                            */
/*
#undef  DCP_DO_TIMEOUT_EXTERN
*/
#define DCP_DO_TIMEOUT_EXTERN

/***************************************************
 * If defined DCP_TYPEDEF_UPPER_RQB_PTR: 
 *    DCP uses "typedef..." for upper rqb ptr type, 
 * else "#define..."
 *
*/
#define DCP_TYPEDEF_UPPER_RQB_PTR

/***********************************************************
 * Ethernet header:
 * +------+------+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+
 * | Dmac | Smac |12|13|14|15|16|17|18|19| DCP-PDU.......|
 * +------+------+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+
 * If any other value shall be used define it here 
*/
#if 0

#define DCP_ETH_VLANTAG_0		0x81	/* 12: VLAN 	*/
#define DCP_ETH_VLANTAG_1		0x00	/* 13: VLAN 	*/
#define DCP_ETH_VLANTAG_2		0x00	/* 14: VLAN 	*/
#define DCP_ETH_VLANTAG_3		0x00	/* 15: VLAN 	*/
#define DCP_ETH_ETHTYPE_0		0x88	/* 16: EtheType	*/
#define DCP_ETH_ETHTYPE_1		0x92	/* 17: EtheType	*/
#define DCP_ETH_FRAMEID_0		0xfe	/* 18: FrameID	*/
#define DCP_ETH_FRAMEID_MRQ		0xfe	/* 19: FrameID	*/
#define DCP_ETH_FRAMEID_MRS		0xff	/* 19: FrameID	*/
#define DCP_ETH_FRAMEID_URR		0xfd	/* 19: FrameID	*/
#define DCP_ETH_FRAMEID_HLO		0xfc	/* 19: FrameID	*/

#endif

/* Max. size of data part in DCP-Request/Response-PDU = max len of VLAN tagged
 * ethernet frame (1496) - ethernet header (20) - DCP-Header (10)
*/
#define DCP_MAX_RSP_DATA_SIZE	1466	
#define DCP_MAX_REQ_DATA_SIZE	1466

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/
#define DCP_ATTR_NEAR   
#define DCP_ATTR_FAR    
#define DCP_ATTR_SHUGE  
#define DCP_ATTR_HUGE   

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/
#define DCP_UPPER_IN_FCT_ATTR    DCP_ATTR_HUGE  /* dcp-user to dcp */
#define DCP_UPPER_OUT_FCT_ATTR   DCP_ATTR_HUGE  /* dcp to dcp-user */
#define DCP_LOWER_IN_FCT_ATTR    DCP_ATTR_HUGE  /* lower component to dcp */
#define DCP_LOWER_OUT_FCT_ATTR   DCP_ATTR_HUGE  /* dcp to lower component */
#define DCP_SYSTEM_IN_FCT_ATTR   DCP_ATTR_FAR   /* system to dcp */
#define DCP_SYSTEM_OUT_FCT_ATTR  DCP_ATTR_FAR   /* dcp to system */
#define DCP_LOCAL_FCT_ATTR       DCP_ATTR_NEAR  /* local function */

/*=====  data attributes  =====*/
#define DCP_UPPER_RQB_ATTR       DCP_ATTR_HUGE  	/* upper-rqb data */
#define DCP_UPPER_MEM_ATTR       DCP_ATTR_NEAR  	/* upper-memory data */
#define DCP_SYSTEM_MEM_ATTR      DCP_ATTR_HUGE  	/* system-memory data */
#define DCP_LOCAL_MEM_ATTR       DCP_ATTR_SHUGE 	/* local data */
#define DCP_COMMON_MEM_ATTR      DCP_ATTR_SHUGE 	/* greatest common */
                                                	/* data attribute  */
#define DCP_LOWER_RQB_ATTR
#define DCP_LOWER_MEM_ATTR
#define DCP_LOWER_ETH_RQB_ATTR   EDD_UPPER_RQB_ATTR /* ethernet driver rqb */
#define DCP_LOWER_ETH_MEM_ATTR   EDD_UPPER_MEM_ATTR	/* ethernet driver mem */
#define DCP_LOWER_ETH_TXMEM_ATTR EDD_UPPER_MEM_ATTR	/* ethernet send mem */
#define DCP_LOWER_ETH_RXMEM_ATTR EDD_UPPER_MEM_ATTR	/* ethernet recv mem */
#define DCP_LOWER_PB_RQB_ATTR    DCP_ATTR_HUGE  	/* lower-rqb data */
#define DCP_LOWER_PB_MEM_ATTR    DCP_ATTR_NEAR  	/* lower-memory data */
#define DCP_LOWER_PB_TXMEM_ATTR  DCP_ATTR_NEAR  	/* lower-memory send data */
#define DCP_LOWER_PB_RXMEM_ATTR  DCP_ATTR_NEAR  	/* lower-memory recv data */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/* Ethernet
*/
#define DCP_ETH_SERVICE_TYPE	EDD_SERVICE
#define DCP_ETH_MEM_MAX_LEN		EDD_FRAME_BUFFER_LENGTH
#define DCP_ETH_RQB_PTR_TYPE  	EDD_UPPER_RQB_PTR_TYPE
#define DCP_ETH_MEM_PTR_TYPE 	EDD_UPPER_MEM_PTR_TYPE
#define DCP_ETH_TXMEM_PTR_TYPE 	EDD_UPPER_MEM_PTR_TYPE
#define DCP_ETH_RXMEM_PTR_TYPE 	EDD_UPPER_MEM_PTR_TYPE
#define DCP_ETH_RQB_TYPE  		EDD_RQB_TYPE
#define DCP_ETH_RSP_OK  		EDD_STS_OK
#define DCP_ETH_RSP_CANCEL 		EDD_STS_OK_CANCEL
#define DCP_ETH_RSP_TIMEOUT		EDD_STS_ERR_TIMEOUT

/* Profibus
*/
#define DCP_PB_MEM_PTR_TYPE 	LSA_VOID *
#define DCP_PB_TXMEM_PTR_TYPE 	LSA_VOID *
#define DCP_PB_RXMEM_PTR_TYPE 	LSA_VOID *
#define DCP_PB_RQB_PTR_TYPE  	LSA_VOID *
#define DCP_PB_RQB_TYPE			LSA_UINT8		/* pcIOX: no Profibus */
#define DCP_PB_PAR_TYPE			LSA_UINT8		/* pcIOX: no Profibus */
#define DCP_PB_RSP_OK  			LSA_OK

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* DCP MAC- and IP-Addess
*  ======================
*/
#define DCP_MAC_ADR_SIZE		EDD_MAC_ADDR_SIZE
#define DCP_IP_ADR_SIZE			EDD_IP_ADDR_SIZE

/* Maximum number of lower channels 240407lrg001: without FSU channels
*/
#ifndef DCP_MAX_LOW_CHANNELS
# error "DCP_MAX_LOW_CHANNELS not defined"
#endif

/* Maximum number of client channels on one lower channel (61)
 */
#ifndef DCP_MAX_CLIENT_CHANNELS
# error "DCP_MAX_CLIENT_CHANNELS not defined"
#endif

/* Maximum number of upper client and server channels:
 * Max. one server channel (plus FSU channel) and 61 client channels are based 
 * on one lower channel. 120307lrg001: FSU-Channel
 * AP01503994: max. DCP_MAX_CHANNELS = 63 to free uppermost 2 bits of XID (upper channel handle)
*/
#define DCP_MAX_CHANNELS  		((DCP_MAX_CLIENT_CHANNELS + 2) * DCP_MAX_LOW_CHANNELS)

/* Enable Client HELLO requests: a 10ms LSA timer is allocated by dcp_init()
*/
#define DCP_CLIENT_HELLO_ENABLE

/*=============================================================================
 *      dcp request block (== upper request block)
 *===========================================================================*/

/*====  Definition of dcp-RQB-HEADER:  =====*/
#define DCP_RQB_HEADER           LSA_RQB_HEADER(DCP_UPPER_RQB_PTR_TYPE)

/*====  Definition of dcp-RQB-TRAILER:  =====*/
/* If necessary during system integration, additional parameters can be 
 * added!
 */
#define DCP_RQB_TRAILER          LSA_RQB_TRAILER


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * Access to upper RQB header
 * 141107lrg001: for tap: set to LSA macros if defined (in lsa_cfg.h)
*/
#ifdef  LSA_RQB_GET_HANDLE
#define DCP_UPPER_RQB_HANDLE_GET(_RbuP)  					LSA_RQB_GET_HANDLE(_RbuP)
#define DCP_UPPER_RQB_HANDLE_SET(_RbuP, _Val)  				LSA_RQB_SET_HANDLE(_RbuP, _Val)
#define DCP_UPPER_RQB_OPCODE_GET(_RbuP)  					LSA_RQB_GET_OPCODE(_RbuP)
#define DCP_UPPER_RQB_OPCODE_SET(_RbuP, _Val)				LSA_RQB_SET_OPCODE(_RbuP, _Val)
#define DCP_UPPER_RQB_RESPONSE_GET(_RbuP)		  			LSA_RQB_GET_RESPONSE(_RbuP)
#define DCP_UPPER_RQB_RESPONSE_SET(_RbuP, _Val)  			LSA_RQB_SET_RESPONSE(_RbuP, _Val)
#define DCP_UPPER_RQB_NEXT_GET(_RbuP)  						LSA_RQB_GET_NEXT_RQB_PTR(_RbuP)
#define DCP_UPPER_RQB_NEXT_SET(_RbuP, _Val)  				LSA_RQB_SET_NEXT_RQB_PTR(_RbuP, _Val)
#define DCP_UPPER_RQB_PREV_GET(_RbuP)  						LSA_RQB_GET_PREV_RQB_PTR(_RbuP)
#define DCP_UPPER_RQB_PREV_SET(_RbuP, _Val)  				LSA_RQB_SET_PREV_RQB_PTR(_RbuP, _Val)
#define DCP_UPPER_RQB_USERID_GET(_RbuP)						LSA_RQB_GET_USER_ID(_RbuP)
#define DCP_UPPER_RQB_USERID_SET(_RbuP, _Val)				LSA_RQB_SET_USER_ID(_RbuP, _Val)
#define DCP_UPPER_RQB_USER32_GET(_RbuP)						LSA_RQB_GET_USER_ID_UVAR32(_RbuP)
#define DCP_UPPER_RQB_USER32_SET(_RbuP, _Val)				LSA_RQB_SET_USER_ID_UVAR32(_RbuP, _Val)
#else
#define DCP_UPPER_RQB_HANDLE_GET(_RbuP)  					((_RbuP)->handle)
#define DCP_UPPER_RQB_HANDLE_SET(_RbuP, _Val)  				((_RbuP)->handle = (_Val))
#define DCP_UPPER_RQB_OPCODE_GET(_RbuP)  					((_RbuP)->opcode)
#define DCP_UPPER_RQB_OPCODE_SET(_RbuP, _Val)				((_RbuP)->opcode = (_Val))
#define DCP_UPPER_RQB_RESPONSE_GET(_RbuP)		  			((_RbuP)->response)
#define DCP_UPPER_RQB_RESPONSE_SET(_RbuP, _Val)  			((_RbuP)->response = (_Val))
#define DCP_UPPER_RQB_NEXT_GET(_RbuP)  						((_RbuP)->next_rqb_ptr)
#define DCP_UPPER_RQB_NEXT_SET(_RbuP, _Val)  				((_RbuP)->next_rqb_ptr = (_Val))
#define DCP_UPPER_RQB_PREV_GET(_RbuP)  						((_RbuP)->prev_rqb_ptr)
#define DCP_UPPER_RQB_PREV_SET(_RbuP, _Val)  				((_RbuP)->prev_rqb_ptr = (_Val))
#define DCP_UPPER_RQB_COMPID_SET(_RbuP, _Val)				((_RbuP)->comp_id = (_Val))
#define DCP_UPPER_RQB_USERID_GET(_RbuP)						((_RbuP)->user_id)
#define DCP_UPPER_RQB_USERID_SET(_RbuP, _Val)				((_RbuP)->user_id = (_Val))
#define DCP_UPPER_RQB_USER32_GET(_RbuP)						((_RbuP)->user_id.uvar32)
#define DCP_UPPER_RQB_USER32_SET(_RbuP, _Val)				((_RbuP)->user_id.uvar32 = (_Val))
#endif

/*---------------------------------------------------------------------------*
 * Access to upper RQB
*/
/* 040811lrg
#define DCP_UPPER_RQB_ERR_COMPID_GET(_RbuP)					((_RbuP)->err.lsa_component_id)
#define DCP_UPPER_RQB_ERR_COMPID_SET(_RbuP, _Val)			((_RbuP)->err.lsa_component_id = (_Val))
*/
#define DCP_UPPER_RQB_ERR_COMPID_GET(_RbuP)					(LSA_COMP_ID_UNUSED)
#define DCP_UPPER_RQB_ERR_COMPID_SET(_RbuP, _Val)
#define DCP_UPPER_RQB_SYSPATH_GET(_RbuP)					((_RbuP)->args.channel.sys_path)
#define DCP_UPPER_RQB_SYSPATH_SET(_RbuP, _Val)				((_RbuP)->args.channel.sys_path = (_Val))
#define DCP_UPPER_RQB_HANDLE_LOWER_GET(_RbuP)				((_RbuP)->args.channel.handle)
#define DCP_UPPER_RQB_HANDLE_LOWER_SET(_RbuP, _Val)			((_RbuP)->args.channel.handle = (_Val))
#define DCP_UPPER_RQB_HANDLE_UPPER_GET(_RbuP)				((_RbuP)->args.channel.handle_upper)
#define DCP_UPPER_RQB_HANDLE_UPPER_SET(_RbuP, _Val)			((_RbuP)->args.channel.handle_upper = (_Val))
#define DCP_UPPER_RQB_CBF_GET(_RbuP)						((_RbuP)->args.channel.dcp_request_upper_done_ptr)
#define DCP_UPPER_RQB_CBF_SET(_RbuP, _Val)					((_RbuP)->args.channel.dcp_request_upper_done_ptr = (_Val))
#define DCP_UPPER_RQB_FSU_SET(_RbuP, _Val)					// Will be set to 1 on HELLO request indication. Else to 0

/*=============================================================================
 * macro name:    DCP_FILE_SYSTEM_EXTENSION(module_id_)
 *
 * macro:         System integrators can use the macro to smuggle something 
 *                into the component source files. By default it's empty.
 *
 *                The system integrators are responsible for the contents of 
 *                that macro.
 *
 *                Internal data of LSA components may not be changed!
 *
 * parameter:     Module-id
 *
 *                There's no prototype for that macro!
 *===========================================================================*/
#define DCP_FILE_SYSTEM_EXTENSION(module_id_)

/*---------------------------------------------------------------------------*/
/* upper component interface output functions:
 */
/* Call of dcp_request_upper_done_ptr

#define DCP_REQUEST_UPPER_DONE(dcp_request_upper_done_ptr_, upper_rqb_ptr_, sys_ptr_) \
    (dcp_request_upper_done_ptr_)((upper_rqb_ptr_))
*/


/*---------------------------------------------------------------------------*/
/* system interface output macros
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the dcp.
 * A function can be nevertheless invoked within the macro.
 * Attention: DCP mustn't cast the parameters at the call.
 */

/* Remove Prototyping of Functions (in dcp_sys.h) 
 * currently not used:
*/ 
#define DCP_UPPER_TO_LOWER_MEM
#define DCP_LOWER_TO_UPPER_MEM
#define DCP_LOWER_TO_LOWER_MEM
#define DCP_COPY_LOCAL_TO_ETHTX_MEM
#define DCP_INIT_SYS_PTR
#define DCP_SYS_PTR_IS_INITIALIZED

/*====  for LSA-trace:  =====*/

/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/

#ifndef DCP_CFG_TRACE_MODE
# error "DCP_CFG_TRACE_MODE not defined"

/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then 
 * the dcp trace macros can be defined here.
 *
 * See also files dcp_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/
#endif

/*=============================================================================
 * macro names:   DCP_PUT_INFO_X  (X = 0, 1, 2, 3, ..., 16)
 *
 * macro:         dcp informs the system. 
 *                Some target systems need information of some LSA-components 
 *                for diagnosis purposes.
 *                Example of information: Internal state changes
 *
 * parameters:    info_id_: kind if info
 *                para1_:   info 1
 *                para2_:   info 2
 *                 ...        ...
 *                para16_:  info 16
 *                          Examples for info: handle (of channel),
 *                                             internal state,
 *                                             error-code,
 *                                             rqb-pointer
 * return value:  LSA_VOID
 *===========================================================================*/
#define DCP_PUT_INFO_0(info_id_)
#define DCP_PUT_INFO_1(info_id_, para1_)
#define DCP_PUT_INFO_2(info_id_, para1_, para2_)
#define DCP_PUT_INFO_3(info_id_, para1_, para2_, para3_)
#define DCP_PUT_INFO_4(info_id_, para1_, para2_, para3_, para4_)
#define DCP_PUT_INFO_5(info_id_, para1_, para2_, para3_, para4_, para5_)
#define DCP_PUT_INFO_6(info_id_, para1_, para2_, para3_, para4_, para5_, \
                          para6_)
#define DCP_PUT_INFO_7(info_id_, para1_, para2_, para3_, para4_, para5_, \
                          para6_, para7_)
#define DCP_PUT_INFO_8(info_id_, para1_, para2_, para3_, para4_, para5_, \
                          para6_, para7_, para8_)
#define DCP_PUT_INFO_9(info_id_, para1_, para2_, para3_, para4_, para5_, \
                          para6_, para7_, para8_, para9_)
#define DCP_PUT_INFO_10(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_)
#define DCP_PUT_INFO_11(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_)
#define DCP_PUT_INFO_12(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_, \
                           para12_)
#define DCP_PUT_INFO_13(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_, \
                           para12_, para13_)
#define DCP_PUT_INFO_14(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_, \
                           para12_, para13_, para14_)
#define DCP_PUT_INFO_15(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_, \
                           para12_, para13_, para14_, para15_)
#define DCP_PUT_INFO_16(info_id_, para1_, para2_, para3_, para4_, para5_, \
                           para6_, para7_, para8_, para9_, para10_, para11_, \
                           para12_, para13_, para14_, para15_, para16_)

/*****************************************************************************/
/*  end of file DCP_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of DCP_CFG_H */
