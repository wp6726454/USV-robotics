#ifndef DCP_USR_H                     /* ----- reinclude-protection ----- */
#define DCP_USR_H

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
/*  F i l e               &F: dcp_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for dcp.                */
/*                                                                           */
/*****************************************************************************/
#ifdef DCP_MESSAGE /*********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2002-07-17  P00.00.00.00_00.01.00.00  lrg  from LSA_GLOB_P02.00.00.00_00 */
/*                                             .01.04.01                     */
/*  2002-10-31  P01.00.00.00_00.01.01.01  lrg  1.Increment:Identify Response */
/*  2003-01-27  P01.00.00.00_00.03.00.00  lrg  3.Increment:                  */
/*              Macros for RQB access moved to dcp_cfg.h                     */
/*              Upper request block: ParaPtr -> Params                       */
/*  2003-04-17  P01.00.00.00_00.04.00.00  lrg  4.Increment: CD7 -> DCP       */
/*  2003-06-13  P01.00.00.00_00.07.00.00  lrg  7.Increment: Client services  */
/*  2003-07-17  P01.00.00.00_00.07.01.00  lrg  LSA_GLOB 2.1.0.0_0.2.1.1      */
/*  2003-12-15  P02.00.00.00_00.01.00.00  dsc  New version of timeout hand-  */
/*              ling introduced: DCP_OPC_TIMER, DCP_RQB_TIMER_TYPE           */
/*              If defined DCP_TYPEDEF_UPPER_RQB_PTR: Use typedef for upper  */
/*              rqb ptr type                                                 */
/*  2004-02-09  P02.00.00.00_00.02.00.00  lrg                                */
/*              NEU: Native RQB Interface mit Retry und Request-Queuing      */
/*              Neue Database-IDs gemaess PNO-DCP-Spezifikation: 7->1,8->2...*/
/*  2004-02-11  P02.00.00.00_00.02.00.01  lrg PBC-Database-Erweiterung       */
/*  2004-03-03  P02.00.00.00_00.02.00.02  lrg forward declaration dcp_rqb_tag*/
/*              und nur noch Swap-Macros unter #ifdef DCP_BIG_ENDIAN         */
/*  2004-03-12  P02.00.00.00_00.02.00.03  lrg Result-Codes modififiert       */
/*              4 und 5 entfernt, DCP_RESULT_NO_RESOURCE: 6 -> 4             */
/*  2004-03-29  P02.00.00.00_00.02.00.04  lrg                                */
/*              DCP_SWAPD(...16ptr_,...) -> DCP_SWAPD(...32ptr_,...)         */
/*  2004-04-19  P02.00.00.00_00.03.00.00  lrg  Strukturen und Opcodes fuer   */
/*              Server mit RQB Interface.                                    */
/*  2004-06-24  P03.00.00.00_00.01.00.00  lrg  Version 3                     */
/*  2004-10-01  P03.02.00.00_00.02.00.00  lrg  011004lrg001: support for     */
/*              more than 1 primary IDENTIFY filter ( e.g. Alias names)      */
/*  2004-12-20  P03.02.00.00_00.02.02.01  lrg Macros DCP_HI*(), DCP_LO*()    */
/*              and DCP_SWAP*() moved from dcp_usr.h to dcp_pck.h/txt        */
/*  2004-12-22  P03.03.00.00_00.02.03.00  lrg  LSA_GLOB 2.1.0.0_0.4.2.1      */
/*              Values for response codes of DCP RQB changed                 */
/*  2005-01-12  P03.03.00.00_00.03.02.00  lrg Macros DCP_WORD(), DCP_DWORD() */
/*              moved from dcp_pck.h/txt to dcp_usr.h                        */
/*  2005-12-09  P03.09.00.00_00.01.00.00  lrg  upper RQB macros moved here   */
/*              from dcp_cfg.h/txt                                           */
/*  2006-06-02  P03.11.00.00_00.01.01.02  lrg ARTS-RQ 00291071:  mandatory   */
/*              upper RQB macros moved back to dcp_cfg.txt/h.                */
/*              New:  DCP_DSID_RESETFACT and  DCP_TYPE_CTRL_RESETFACT        */
/*              New:  DCP_DSID_IDALIAS and  DCP_TYPE_DEVICE_ALIAS            */
/*  2006-06-30  P03.12.00.00_00.01.01.02  lrg  LSA_GLOB 2.1.0.0_0.7.2.1      */
/*  2006-06-30  P03.12.00.00_00.25.01.02  lrg ARTS-RQ 00291071: M. geloescht */
/*  2007-02-26  P04.00.00.00_00.01.01.02  lrg neu: HELLO u. DeviceInitiative */
/*  2007-03-06  P04.00.00.00_00.01.01.03  lrg neu: FilterId                  */
/*  2007-05-31  P04.00.00.00_00.10.02.02  lrg LSA_GLOB 2.1.0.0_0.8.1.1       */
/*  2007-05-31  P04.00.00.00_00.20.01.02  lrg ArtsRQ AP00477812: Tolerance   */
/*  2008-06-20  P04.00.00.00_00.80.02.03 lrg: DCP_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.01.01 lrg: Package Version (PNIO)         */
/*  2010-08-24  P05.00.00.00_00.01.01.02 240810lrg001: AP00999022 UPPER_DONE */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: AP01205450:              */
/*              AP01205450 Struktur err aus RQB entfernen                    */
/*  2011-08-25  S05.01.00.00_00.04.03.02 AP00696784: Comments in english     */
/*  2011-11-21  T05.01.00.00_00.05.13.01 AP01278788: DCP_IDENTIFY_FALTUNG_MAX*/
/*  2012-03-08  S05.02.00.00_00.01.16.01 AP01138373: DCP_OPC_FILTER_SET      */
/*              Entfernen des DCP Identify Filter Service in DCP             */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01069471 DCP_REQUEST_UPPER_DONE   */
/*              and all other functions with DCP_SYSTEM_OUT_FCT_ATTR moved   */
/*              from dcp_usr.h to dcp_sys.h                                  */
/*  2013-05-16  P05.03.00.00_00.01.10.01 AP01509782 DCP channel details und  */
/*              Konfigurationsschalter bereinigen                            */
/*  2013-07-12  P05.03.00.00_00.01.10.01 AP01561748:DCP: Verhalten bzgl.     */
/*              TransactionEnd an Norm anpassen                              */
/*  2014-09-09  P05.04.00.00_00.01.20.01 1109849 DCP CM: Umsetzung des       */
/*              Features "OEM Identification"                                */
/*  2014-10-01  P05.04.00.00_00.01.22.01 1097934 Umstellung auf PNIO-PACK    */
/*              Strukturen und Konstanten aus dcp_pck.h hierher verschoben   */
/*                                                                           */
/*****************************************************************************/
#endif /* DCP_MESSAGE */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* Low- and High Byte, Word
*/
#define DCP_HIBYTE(u16_)	((LSA_UINT8)(((LSA_UINT16)(u16_)) >> 8))
#define DCP_LOBYTE(u16_)	((LSA_UINT8)(u16_))
#define DCP_HIWORD(u32_)	((LSA_UINT16)(((LSA_UINT32)(u32_)) >> 16))
#define DCP_LOWORD(u32_)	((LSA_UINT16)(u32_))

/* Macros to assemble High- and Low bytes/words 
*/
#define DCP_WORD(l8_,h8_)		\
		((LSA_UINT16)((((LSA_UINT16)(l8_)) | (((LSA_UINT16)(h8_)) << 8))))

#define DCP_DWORD(l16_,h16_)	\
		((LSA_UINT32)((((LSA_UINT32)(l16_)) | (((LSA_UINT32)(h16_)) << 16))))

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/*=============================================================================
 *      dcp request block (== upper request block)
 *===========================================================================*/

/*=====  opcodes within request block  =====
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_OPC")
#define DCP_OPC_OPEN_CHANNEL		(LSA_OPCODE_TYPE)	1
#define DCP_OPC_CLOSE_CHANNEL		(LSA_OPCODE_TYPE)	2
#define DCP_OPC_TIMER				(LSA_OPCODE_TYPE)	3
#define DCP_OPC_RESERVED			(LSA_OPCODE_TYPE)	4
#define DCP_OPC_IDENTIFY_REQ		(LSA_OPCODE_TYPE)	5
#define DCP_OPC_NOTIFY				(LSA_OPCODE_TYPE)	6
#define DCP_OPC_SET_REQ				(LSA_OPCODE_TYPE)	7
#define DCP_OPC_GET_REQ				(LSA_OPCODE_TYPE)	8
#define DCP_OPC_RECV_IND			(LSA_OPCODE_TYPE)	9
#define DCP_OPC_SEND_RSP			(LSA_OPCODE_TYPE)	10
#define DCP_OPC_CANCEL_RECV			(LSA_OPCODE_TYPE)	11
#define DCP_OPC_HELLO_REQ			(LSA_OPCODE_TYPE)	12
PTRC_SUBLABEL_LIST_CLOSE("DCP_OPC")

/*=====  response codes within request block  =====
*/
#define DCP_RSP_OK					(LSA_UINT16) 	LSA_OK
#define DCP_RSP_ERR_RESOURCE		(LSA_UINT16)	LSA_RSP_ERR_RESOURCE
#define DCP_RSP_ERR_PARAM			(LSA_UINT16)	LSA_RSP_ERR_PARAM
#define DCP_RSP_ERR_SEQUENCE		(LSA_UINT16)	LSA_RSP_ERR_SEQUENCE
#define DCP_RSP_ERR_SYS_PATH		(LSA_UINT16)	LSA_RSP_ERR_SYS_PATH
#define DCP_RSP_OK_ACTIVE			(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  1)
#define DCP_RSP_OK_CANCEL			(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  2)
#define DCP_RSP_OK_MORE				(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  3)
#define DCP_RSP_OK_RESULT			(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  4)
#define DCP_RSP_ERR_SYNTAX			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  1)
#define DCP_RSP_ERR_REF				(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  2)
#define DCP_RSP_ERR_PROTOCOL		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  3)
#define DCP_RSP_ERR_TIMEOUT			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  4)
#define DCP_RSP_ERR_LOWER_LAYER		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  5)
#define DCP_RSP_ERR_CONFIG			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  6)
#define DCP_RSP_ERR_NO_DATABASE		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  7)
#define DCP_RSP_ERR_NO_DATASET		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  8)
#define DCP_RSP_ERR_DS_NOT_SET		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  9)
#define DCP_RSP_ERR_UNKNOWN			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 10)
#define DCP_RSP_ERR_NO_ACTION		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 11)
#define DCP_RSP_ERR_NO_RESOURCE		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 12)
#define DCP_RSP_ERR_NO_SERVICE		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 13)

/* DCP Database IDs (Options)
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_DBID")
#define DCP_DBID_INVALID	0		/* unused 					*/
#define DCP_DBID_TCP		1		/* TCP-IP parameter			*/
#define DCP_DBID_IDENT		2		/* Station identification	*/
#define DCP_DBID_DHCP		3		/* DHCP-Parameter			*/
#define DCP_DBID_LLDP		4		/* DHCP-Parameter			*/
#define DCP_DBID_CTRL		5		/* Control options			*/
#define DCP_DBID_PBC		128		/* PBC-Profil (OrangeBox)	*/
#define DCP_DBID_PBPARAM	129		/* Profibus parameter		*/
#define DCP_DBID_ROUTER		130		/* Router/Gateway			*/
#define DCP_DBID_DPCLOCK	131		/* DP/time master			*/
#define DCP_DBID_DIAG		132		/* Diagnosis data			*/
#define DCP_DBID_ISO		133		/* ISO parameter			*/
#define DCP_DBID_ISOTCP		134		/* ISO/TCP parameter		*/
#define DCP_DBID_NOFILTER	255		/* IDENTIFY No filter		*/
#define DCP_DBID_DEVINIT	6		/* DeviceInitiative Option	*/
PTRC_SUBLABEL_LIST_CLOSE("DCP_DBID")

/* DCP-Dataset IDs (Suboptions)
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_DSID")
#define DCP_DSID_INVALID	0		/* used for fillbyte		*/
#define DCP_DSID_PBPROFIL	1		/* Profibus: Profile ID		*/
#define DCP_DSID_PBPARAM	2		/* Profibus: add. Parameters*/
#define DCP_DSID_PBCOMMON	3		/* Profibus: common params	*/
#define DCP_DSID_PBADDITION	4		/* Profibus: add. parameters*/
#define DCP_DSID_RGADDR		1		/* Router: glob. addr.info	*/
#define DCP_DSID_RGIDENT	2		/* Router: Identification	*/
#define DCP_DSID_MACADR		1		/* ISO/TCP: MAC address		*/
#define DCP_DSID_TCPIP		2		/* IP address...			*/
#define DCP_DSID_PBCSWITCH	1		/* PBC: Switch setting		*/
#define DCP_DSID_PBCRESP	2		/* PBC: Response setting	*/
#define DCP_DSID_PBCCONFIG	3		/* PBC: Switch configuration*/
#define DCP_DSID_IDTYPE		1		/* IDENT: station type		*/
#define DCP_DSID_IDNAME		2		/* IDENT: station name		*/
#define DCP_DSID_IDID		3		/* IDENT: DetailedType		*/
#define DCP_DSID_IDROLE		4		/* IDENT: ProfinetIO role	*/
#define DCP_DSID_IDOPTS		5		/* IDENT: Option list		*/
#define DCP_DSID_IDALIAS	6		/* IDENT: Alias name		*/
#define DCP_DSID_IDDEVINST	7		/* IDENT: DeviceInstance	*/
#define DCP_DSID_IDOEMID	8		/* IDENT: OEMDeviceID		*/
#define DCP_DSID_START		1		/* CONTROL: Start transact.	*/
#define DCP_DSID_END		2		/* CONTROL: EndOf transact.	*/
#define DCP_DSID_SIGNAL		3		/* CONTROL: Signal			*/
#define DCP_DSID_RESPONSE	4		/* CONTROL: Response result */
#define DCP_DSID_RESETFACT	5		/* CONTROL: FactoryReset    */
#define DCP_DSID_RESET		6		/* CONTROL: ResetToFactory  */
#define DCP_DSID_NOFILTER	255		/* IDENTIFY: No filter      */
#define DCP_DSID_DEVINIT	1		/* DeviceInitiative SubOpt.	*/
PTRC_SUBLABEL_LIST_CLOSE("DCP_DSID")

/* DCP type: 8 bit Option, 8 bit Suboption
*/
typedef LSA_UINT16			DCP_TYPE;
#define DCP_TYPE_SIZE		2

#define DCP_TYPE_TCP_MAC		DCP_WORD(DCP_DSID_MACADR,    DCP_DBID_TCP)
#define DCP_TYPE_TCP_IP			DCP_WORD(DCP_DSID_TCPIP,     DCP_DBID_TCP)
#define DCP_TYPE_DEVICE_TYPE	DCP_WORD(DCP_DSID_IDTYPE,    DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_NAME	DCP_WORD(DCP_DSID_IDNAME,    DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_ID		DCP_WORD(DCP_DSID_IDID,      DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_ROLE	DCP_WORD(DCP_DSID_IDROLE,    DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_OPTIONS	DCP_WORD(DCP_DSID_IDOPTS,    DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_ALIAS	DCP_WORD(DCP_DSID_IDALIAS,   DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_INST	DCP_WORD(DCP_DSID_IDDEVINST, DCP_DBID_IDENT)
#define DCP_TYPE_DEVICE_OEMID	DCP_WORD(DCP_DSID_IDOEMID,   DCP_DBID_IDENT)
#define DCP_TYPE_DHCP(subopt_)	DCP_WORD(subopt_,            DCP_DBID_DHCP)
#define DCP_TYPE_LLDP(subopt_)	DCP_WORD(subopt_,            DCP_DBID_LLDP)
#define DCP_TYPE_CTRL_START		DCP_WORD(DCP_DSID_START,     DCP_DBID_CTRL)
#define DCP_TYPE_CTRL_END		DCP_WORD(DCP_DSID_END,       DCP_DBID_CTRL)
#define DCP_TYPE_CTRL_SIGNAL	DCP_WORD(DCP_DSID_SIGNAL,    DCP_DBID_CTRL)
#define DCP_TYPE_CTRL_RESULT	DCP_WORD(DCP_DSID_RESPONSE,  DCP_DBID_CTRL)
#define DCP_TYPE_CTRL_RESETFACT DCP_WORD(DCP_DSID_RESETFACT, DCP_DBID_CTRL)
#define DCP_TYPE_CTRL_RESET     DCP_WORD(DCP_DSID_RESET,     DCP_DBID_CTRL)
#define DCP_TYPE_PBC_SWITCH		DCP_WORD(DCP_DSID_PBCSWITCH, DCP_DBID_PBC)
#define DCP_TYPE_PBC_RESP		DCP_WORD(DCP_DSID_PBCRESP,   DCP_DBID_PBC)
#define DCP_TYPE_PBC_CONFIG		DCP_WORD(DCP_DSID_PBCCONFIG, DCP_DBID_PBC)
#define DCP_TYPE_PB_PROFIL		DCP_WORD(DCP_DSID_PBPROFIL,  DCP_DBID_PBPARAM)
#define DCP_TYPE_PB_PBPARAM		DCP_WORD(DCP_DSID_PBPARAM,   DCP_DBID_PBPARAM)
#define DCP_TYPE_PB_COMMON		DCP_WORD(DCP_DSID_PBCOMMON,  DCP_DBID_PBPARAM)
#define DCP_TYPE_PB_PBADDITION	DCP_WORD(DCP_DSID_PBADDITION,DCP_DBID_PBPARAM)
#define DCP_TYPE_ISOTCP_MAC		DCP_WORD(DCP_DSID_MACADR,    DCP_DBID_ISOTCP)
#define DCP_TYPE_ALL_SELECT		DCP_WORD(DCP_DSID_NOFILTER,  DCP_DBID_NOFILTER)
#define DCP_TYPE_DEV_INITIATIVE	DCP_WORD(DCP_DSID_DEVINIT,   DCP_DBID_DEVINIT)

/* ProfinetIO role bits for DCP_TYPE_DEVICE_ROLE
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_PN")
#define  DCP_PN_DEVICE			1
#define  DCP_PN_CONTROLLER		2
#define  DCP_PN_MULTIDEVICE		4
#define  DCP_PN_SUPERVISOR		8
PTRC_SUBLABEL_LIST_CLOSE("DCP_PN")


/* Subtypes of id filter (DetailedTypeOfStation) in IDENTIFY request params
*/
#define DCP_DETAIL_FILTER_TYPE_PNIO		1
#define DCP_DETAIL_FILTER_LEN_PNIO		4

/* Status of TCP/IP parameter "IpState" at SET request
*/
#define DCP_TCP_PARA_TEMP			0		/* Valid til next power off	*/
#define DCP_TCP_PARA_REMANENT		1		/* Valid forever			*/

/* Status of TCP/IP parameter "IpState" at GET and IDENTIFY request
*/
#define DCP_STATUS_TCPIP_NONE		0x0000	/* TCP/IP Parameter not initialized	*/
#define DCP_STATUS_TCPIP_PROJ		0x0001	/* TCP/IP Parameter projektiert		*/
#define DCP_STATUS_TCPIP_DHCP		0x0002	/* TCP/IP Parameter got from DHCP 	*/
#define DCP_STATUS_TCPIP_RESERVED	0x0003	/* -0x007f: Reserviert				*/
#define DCP_STATUS_TCPIP_CONFLICT   0x0080  /* Bit7: IP addr. conflict detected */

/* Status of ResetToFactory parameter "behavior" at SET request
*/
#define DCP_STATUS_RESET_NONE			0x0000	/* Reserved	                     */
#define DCP_STATUS_RESET_APPLICATION	0x0001	/* Reset application data        */
#define DCP_STATUS_RESET_COMMUNICATION	0x0002	/* Reset communication parameter */
#define DCP_STATUS_RESET_ENGINEERING	0x0003	/* Reset engineering parameter   */
#define DCP_STATUS_RESET_ALL_DATA		0x0008	/* Reset all stored data         */
#define DCP_STATUS_RESET_RESTORE_DATA	0x0009	/* Reset and restore data        */

/* Maximum string length
*/
#define DCP_MAX_STATION_TYPE_LEN	255
#define DCP_MAX_STATION_NAME_LEN	255
#define DCP_MAX_STATION_ID_LEN		6

/* Result in Response PDU
*/
PTRC_SUBLABEL_LIST_OPEN("DCP_RESULT")
#define DCP_RESULT_OK					0
#define DCP_RESULT_NO_DATABASE			1
#define DCP_RESULT_NO_DATASET			2
#define DCP_RESULT_DS_NOT_SET			3
#define DCP_RESULT_NO_RESOURCE			4
#define DCP_RESULT_SET_NOT_POSSIBLE		5
#define DCP_RESULT_SET_NOT_APPLICABLE	6
PTRC_SUBLABEL_LIST_CLOSE("DCP_RESULT")

/* Number of indication RQBs (and IDENTIFY timers) for server requests.
 * This defines the max. number of concurrent requests of the service for the server.
 * If server is "full" the next remote request of the same service will be dropped.
 * The servers user (e.g. OHA) has to send the sum (16+8+1) indication RQBs the first
 * time to enable DCP to accept and indicate remote client requests. 
 */
#define DCP_NUM_SERVER_IDENTIFY_RESOURCES	16
#define DCP_NUM_SERVER_GET_RESOURCES		8
#define DCP_NUM_SERVER_SET_RESOURCES		1

/* Service ID in DCP-PDU-Header
*/
#define DCP_NOREQUEST	(LSA_UINT8)		0xff
#define DCP_NOSERVICE	(LSA_UINT8)		0
#define DCP_GET			(LSA_UINT8)		3
#define DCP_SET			(LSA_UINT8)		4
#define DCP_IDENTIFY	(LSA_UINT8)		5
#define DCP_HELLO		(LSA_UINT8)		6

/* Service type in DCP-PDU-Header
*/
#define DCP_ROLE_MASK			3
#define DCP_ROLE_MSG			2
#define DCP_ROLE_RSP			1
#define DCP_ROLE_REQ			0

/* Value for reserved fields in DCP-Pdu
*/
#define DCP_RESERVED			0

/* SHOW_LOCATION Modifier in DCP-PDU
 * AP01373755: Remove DCP_FLASH_ONCE_TIME (3 seconds) and DCP_FLASH_ONCE_FREQ (2 Hz)
*/
#define DCP_FLASH_ONCE			1	/* The signaling shall be started 			*/
									/* using the Link LED or a similar element.	*/

/* Result in Response PDU
*/
#define DCP_RESULT_RESERVED		0

/* Result in Response Header
*/
#define DCP_RESULT_NO_SERVICE	0x04

/* Reserved byte in GET-Response PDU
*/
#define DCP_DS_RESERVED			0

/* Bit 0 in Low Byte of Status at Request 
 * controls the remanence of the data set:
*/
#define DCP_MASK_DS_REMANENT	1


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/* pointers to user memory 
*/
#define DCP_USR_BYTE_PTR	LSA_UINT8		DCP_UPPER_MEM_ATTR *
#define DCP_USR_WORD_PTR	LSA_UINT16		DCP_UPPER_MEM_ATTR *
#define DCP_USR_DWORD_PTR	LSA_UINT32		DCP_UPPER_MEM_ATTR *

/*=============================================================================
 *      dcp request block (== upper request block)
 *===========================================================================*/
/*************************************************************************** 
 * Structure for one node in the client's IDENTIFY response buffer
*/
typedef struct DCP_NODE_IDENTIFY_
{
	LSA_INT		TlvCnt;							/* Count of TLVs in TlvChain*/
	LSA_UINT8	NodeAddr[DCP_MAC_ADR_SIZE];		/* Server station address	*/
	LSA_UINT8	TlvChain[DCP_MAX_RSP_DATA_SIZE];/* Response TLV chain buffer*/

} DCP_NODE_IDENTIFY;
typedef struct DCP_NODE_IDENTIFY_	DCP_UPPER_MEM_ATTR *	DCP_USR_NODE_PTR;

/* Client REQUEST Identify args *************************************************
*/
typedef struct DCP_RQB_ARGS_IDENTIFY_REQ_
{
	DCP_USR_BYTE_PTR	AddrPtr;	/* Dst address: 01-0E-CF-00-00-00 is	 	*/
									/* used if LSA_NULL. On return: unchanged	*/
	DCP_USR_BYTE_PTR	FilterPtr;	/* Pointer to Filter TLV chain				*/
									/* On return: unchanged						*/
	DCP_USR_NODE_PTR	NodePtr;	/* Pointer to first node struct DCP_NODE_	*/
									/* IDENTIFY in user buffer. 				*/
									/* On return: unchanged						*/
	LSA_INT				FilterCnt;	/* Count of filter TLVs at FilterPtr		*/
									/* On return: unchanged						*/
	LSA_INT				NodeCnt;	/* Max. count of node structs in user buffer*/
									/* On return: actual count of node entrys	*/
	LSA_UINT16			Faltung;	/* "Faltungsfaktor" to send in request -	*/
									/* used also to calc time to wait:			*/
									/* DCP_IDENTIFY_FALTUNG_DEFAULT is used if 0*/
									/* On return: used "Faltungsfaktor"			*/
	LSA_BOOL			Notify;		/* Callback with Notify-RB (if one is in	*/
									/* queue) on every entry of a new node		*/
									/* On return: unchanged				  		*/
	LSA_UINT32			NotifyId;	/* Will be returned with NotifyIndication	*/
									/* if Notify is LSA_TRUE.					*/
									/* On return: unchanged				  		*/
	LSA_UINT16			Tolerance;	/* Time in Milliseconds to be added to the	*/
									/* timeout value calculated from Faltung.	*/  
									/* The default value of 1000 is used if 0	*/
									/* On return: unchanged				  		*/
} DCP_RQB_ARGS_IDENTIFY_REQ;

// Maximum/default values for identify parameter "Faltung"
#define DCP_IDENTIFY_FALTUNG_MAX		6400	// IEC61158-6 V2.3: 0x1900
#define DCP_IDENTIFY_FALTUNG_DEFAULT	1024	// Faltungsfaktor for client Identify req.
												// Timeout = 10,23 s + 1s = 11,23 seconds / request

/* Client REQUEST Notify args ***************************************************
*/
typedef struct DCP_RQB_ARGS_NOTIFY_
{
	DCP_USR_NODE_PTR	NodePtr;	/* Pointer to node struct DCP_NODE_IDENTIFY	*/
									/* in user buffer. On return: unchanged		*/
	LSA_INT				NotifyCnt;	/* Only return: 							*/
									/* Count of Notify-RBs waiting in DCP queue	*/
	LSA_UINT32			NotifyId;	/* Only return: NotifyId from IDENTIFY.req	*/

} DCP_RQB_ARGS_NOTIFY;

/* Client REQUEST Set and Get args **********************************************
 * Parameter structures for option datasets to 
 * send by SET request or receive by GET request
*/
typedef struct DCP_RQB_ARGS_SET_REQ_
{
	DCP_USR_BYTE_PTR	AddrPtr;	/* Destination station address				*/
									/* On return: unchanged						*/
	DCP_USR_BYTE_PTR	ReqPtr;		/* Pointer to TLV chain to send in request	*/
									/* On return: unchanged						*/
	DCP_USR_BYTE_PTR	RspPtr;		/* Pointer to buffer for response TLVs		*/
									/* On return: unchanged						*/
	LSA_INT				OptCnt;		/* Length of the TLV chain to send			*/
									/* On return: count of response TLVs copied	*/
									/*            to RspPtr buffer				*/
} DCP_RQB_ARGS_SET_REQ;

/* Upper RQB parameter structure for client GET request *************************
*/
typedef struct DCP_RQB_ARGS_GET_REQ_
{
	DCP_USR_BYTE_PTR	AddrPtr;	/* Destination station address				*/
									/* On return: unchanged						*/
	DCP_USR_BYTE_PTR	ReqPtr;		/* Pointer to DCP-Types array to send		*/
									/* On return: unchanged						*/
	DCP_USR_BYTE_PTR	RspPtr;		/* Pointer to buffer for response TLVs		*/
									/* On return: unchanged						*/
	LSA_INT				OptCnt;		/* Length of DCP-Types array				*/
									/* On return: count of response TLVs copied	*/
									/*            to RspPtr buffer				*/
} DCP_RQB_ARGS_GET_REQ;

/* Upper RQB parameter structure for client HELLO request ***********************
*/
typedef struct DCP_RQB_ARGS_HELLO_REQ_
{
	DCP_USR_BYTE_PTR	AddrPtr;	/* Destination station address or LSA_NULL	*/
									/* for default addr. On return: unchanged	*/
	DCP_USR_BYTE_PTR	ReqPtr;		/* Pointer to TLV chain to send in request	*/
									/* On return: unchanged						*/
	LSA_INT				OptCnt;		/* Count of TLVs in TLV chain				*/
									/* On return: unchanged						*/
	LSA_INT				Time;		/* Time interval in 10 milliseconds to		*/
									/* send the Hello.req. (0 < Time < 65536)	*/
									/* On return: unchanged						*/
	LSA_INT				SendCnt;	/* Number of DCP-Hello.req PDUs to send.	*/
									/* -n: run forever, 0: stop a running HELLO	*/
									/* On return: unchanged						*/
	LSA_INT				Delay;		/* Time interval in 10 milliseconds to		*/
									/* wait before sending the first Hello.req.	*/
									/* (0 < Time < 65536) On return: unchanged	*/
} DCP_RQB_ARGS_HELLO_REQ;

/* Upper RQB parameter structure for Server Receive Indication request **********
*/
typedef struct DCP_RQB_ARGS_RECV_IND_
{									/* Buffers filled with data on callback:	*/
	DCP_USR_BYTE_PTR	AddrPtr;	/* Ptr to buffer for clients MAC address	*/
	DCP_USR_BYTE_PTR	DataPtr;	/* Pointer to buffer for request options or	*/
									/* TLVs: min size = DCP_MAX_REQ_DATA_SIZE	*/
									/* Values on callback:						*/
	LSA_UINT32 			TransactId;	/* Transaction ID of the clients request	*/
	LSA_INT				ServiceId;	/* Service ID of the clients request		*/
	LSA_INT				OptCnt;		/* Count of request Options copied			*/
									/* to DataPtr buffer						*/
	LSA_UINT32			FilterId;	/* only at HELLO.ind: FilterID from EDD		*/

} DCP_RQB_ARGS_RECV_IND;

/* Upper RQB parameter structure for Server Send Response request ***************
*/
typedef struct DCP_RQB_ARGS_SEND_RESP_
{
	DCP_USR_BYTE_PTR	AddrPtr;	/* Ptr to buffer with clients MAC address	*/
									/* (destination of this response)			*/
	DCP_USR_BYTE_PTR	DataPtr;	/* Ptr to buffer with response data TLVs	*/
									/* with max. size = DCP_MAX_RSP_DATA_SIZE	*/
	LSA_UINT32 			TransactId;	/* Transaction ID of the clients request	*/
	LSA_INT				ServiceId;	/* Service ID of the clients request		*/
	LSA_INT				OptCnt;		/* Count of response TLVs in DataPtr buffer	*/

} DCP_RQB_ARGS_SEND_RESP;

/* Upper RQB parameter structure for Server Cancel Receive request **************
*/
typedef struct DCP_RQB_ARGS_CANCEL_RECV_
{
	LSA_INT				Count;		/* Count of Receive Indication requests	to	*/
									/* remove from DCP (-1 = remove all)		*/
									/* On return: number of remaining RQBs		*/
} DCP_RQB_ARGS_CANCEL_RECV;

/*=====  forward declaration  =====*/
struct dcp_rqb_tag;

/* Pointer to upper request block ***********************************************
*/ 
#ifdef DCP_TYPEDEF_UPPER_RQB_PTR
typedef struct dcp_rqb_tag DCP_UPPER_RQB_ATTR * DCP_UPPER_RQB_PTR_TYPE;
#else
#define DCP_UPPER_RQB_PTR_TYPE struct dcp_rqb_tag DCP_UPPER_RQB_ATTR *
#endif

/*=====  arguments of prefix request block  =====
 *
 * for DCP_OPC_OPEN_CHANNEL, see dcp_open_channel() 
*/
typedef  LSA_VOID  LSA_FCT_PTR(DCP_UPPER_OUT_FCT_ATTR, 
								DCP_UPPER_CALLBACK_FCT_PTR_TYPE)(
		DCP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

typedef struct dcp_rqb_args_channel_tag
{
    LSA_HANDLE_TYPE                 handle;
                                    /* req: -                                */
                                    /* cnf: channel-handle of dcp            */
                                    /* At all later RQBs user will write it  */
                                    /* into handle of DCP_RQB_HEADER.        */
    LSA_HANDLE_TYPE                 handle_upper;
                                    /* req: channel-handle of user           */
                                    /* cnf: -                                */
                                    /* At all confirmations dcp writes it    */
                                    /* into handle of DCP_RQB_HEADER.        */
    LSA_SYS_PATH_TYPE               sys_path;
                                    /* req: system-path of channel           */
                                    /* cnf: -                                */
    DCP_UPPER_CALLBACK_FCT_PTR_TYPE dcp_request_upper_done_ptr;
                                    /* req: pointer to callback-function     */
                                    /* cnf: -                                */
} DCP_RQB_ARGS_CHANNEL_TYPE;

/* for DCP_OPC_CLOSE_CHANNEL, see dpp_close_channel().
 * DCP_RQB_ARGS_TYPE isn't needed.
 */

#ifdef DCP_DO_TIMEOUT_EXTERN
/*---------------------------------------------------------------------------*/
/* Opcode: DCP_OPC_TIMER  (for DCP internal use only!)                       */
/*---------------------------------------------------------------------------*/
typedef struct DCP_RQB_TIMER_TYPE_        
{
	LSA_UINT16					TimerID;	/* DCP-TimerID */
	LSA_USER_ID_TYPE			UserID;
} DCP_RQB_TIMER_TYPE;

typedef struct DCP_RQB_TIMER_TYPE_ DCP_UPPER_MEM_ATTR * DCP_UPPER_TIMER_PTR_TYPE;
#endif

/*=====  union of arguments of dcp request block  =====
*/
typedef union dcp_rqb_args_tag
{
	DCP_RQB_ARGS_CHANNEL_TYPE	channel;	/* see							*/
											/* DCP_RQB_ARGS_CHANNEL_TYPE	*/
	DCP_RQB_ARGS_IDENTIFY_REQ	Identify;	/* Client: Identify				*/
	DCP_RQB_ARGS_NOTIFY			Notify;		/* Client: Notify				*/
	DCP_RQB_ARGS_SET_REQ		Set;		/* Client: Set					*/
	DCP_RQB_ARGS_GET_REQ		Get;		/* Client: Get					*/
	DCP_RQB_ARGS_HELLO_REQ		Hello;		/* Client: Hello				*/
	DCP_RQB_ARGS_RECV_IND		RecvInd;	/* Server: Receive Indication	*/
	DCP_RQB_ARGS_SEND_RESP		SendRsp;	/* Server: Send Response		*/
	DCP_RQB_ARGS_CANCEL_RECV	Cancel;		/* Server: Cancel Receive		*/
#ifdef DCP_DO_TIMEOUT_EXTERN
	DCP_RQB_TIMER_TYPE			Timer;		/* TimerEvent. internal use		*/
#endif

} DCP_RQB_ARGS_TYPE;


/*=====  prefix request block definition  =====
*/
typedef struct dcp_rqb_tag
{
    DCP_RQB_HEADER                           /* DCP RQB header, see file     */
                                             /* dcp_cfg.h                    */
/* 040811lrg
	LSA_RQB_LOCAL_ERROR_TYPE        err;     // req: -
                                             // cnf: error from lower-layer
                                             // see LSA_RQB_LOCAL_ERROR_TYPE
*/
    DCP_RQB_ARGS_TYPE               args;    /* arguments corresponding to   */
                                             /* the opcode, see              */
                                             /* DCP_RQB_ARGS_TYPE            */
    DCP_RQB_TRAILER                          /* additional RQB trailer, see  */
                                             /* file dcp_cfg.h               */
} DCP_RQB_TYPE;

/*---------------------------------------------------------------------------*
 * Access to upper RQB
*/
#define DCP_UPPER_RQB_RECV_IND_FILTER_ID_GET(_RbuP)			((_RbuP)->args.RecvInd.FilterId)
#define DCP_UPPER_RQB_RECV_IND_FILTER_ID_SET(_RbuP, _Val)	((_RbuP)->args.RecvInd.FilterId = (_Val))
#define DCP_UPPER_RQB_RECV_IND_SERVICE_ID_GET(_RbuP)		((_RbuP)->args.RecvInd.ServiceId)
#define DCP_UPPER_RQB_RECV_IND_SERVICE_ID_SET(_RbuP, _Val)	((_RbuP)->args.RecvInd.ServiceId = (_Val))
#define DCP_UPPER_RQB_RECV_IND_TRANS_ID_GET(_RbuP)			((_RbuP)->args.RecvInd.TransactId)
#define DCP_UPPER_RQB_RECV_IND_TRANS_ID_SET(_RbuP, _Val)	((_RbuP)->args.RecvInd.TransactId = (_Val))
#define DCP_UPPER_RQB_RECV_IND_OPT_CNT_GET(_RbuP)			((_RbuP)->args.RecvInd.OptCnt)
#define DCP_UPPER_RQB_RECV_IND_OPT_CNT_SET(_RbuP, _Val)		((_RbuP)->args.RecvInd.OptCnt = (_Val))
#define DCP_UPPER_RQB_RECV_IND_ADDR_PTR_GET(_RbuP)			((_RbuP)->args.RecvInd.AddrPtr)
#define DCP_UPPER_RQB_RECV_IND_ADDR_PTR_SET(_RbuP, _Val)	((_RbuP)->args.RecvInd.AddrPtr = (_Val))
#define DCP_UPPER_RQB_RECV_IND_DATA_PTR_GET(_RbuP)			((_RbuP)->args.RecvInd.DataPtr)
#define DCP_UPPER_RQB_RECV_IND_DATA_PTR_SET(_RbuP, _Val)	((_RbuP)->args.RecvInd.DataPtr = (_Val))
#define DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_GET(_RbuP)		((_RbuP)->args.SendRsp.ServiceId)
#define DCP_UPPER_RQB_RESP_REQ_SERVICE_ID_SET(_RbuP, _Val)	((_RbuP)->args.SendRsp.ServiceId = (_Val))
#define DCP_UPPER_RQB_RESP_REQ_TRANS_ID_GET(_RbuP)			((_RbuP)->args.SendRsp.TransactId)
#define DCP_UPPER_RQB_RESP_REQ_TRANS_ID_SET(_RbuP, _Val)	((_RbuP)->args.SendRsp.TransactId = (_Val))
#define DCP_UPPER_RQB_RESP_REQ_OPT_CNT_GET(_RbuP)			((_RbuP)->args.SendRsp.OptCnt)
#define DCP_UPPER_RQB_RESP_REQ_OPT_CNT_SET(_RbuP, _Val)		((_RbuP)->args.SendRsp.OptCnt = (_Val))
#define DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_GET(_RbuP)			((_RbuP)->args.SendRsp.AddrPtr)
#define DCP_UPPER_RQB_RESP_REQ_ADDR_PTR_SET(_RbuP, _Val)	((_RbuP)->args.SendRsp.AddrPtr = (_Val))
#define DCP_UPPER_RQB_RESP_REQ_DATA_PTR_GET(_RbuP)			((_RbuP)->args.SendRsp.DataPtr)
#define DCP_UPPER_RQB_RESP_REQ_DATA_PTR_SET(_RbuP, _Val)	((_RbuP)->args.SendRsp.DataPtr = (_Val))
#define DCP_UPPER_RQB_CANCEL_RECV_COUNT_SET(_RbuP, _Val)	((_RbuP)->args.Cancel.Count = (_Val))

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  dcp_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE    opcode:        *_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel                 
 *     LSA_VOID  LSA_FCT_PTR(*_UPPER_OUT_FCT_ATTR, *_request_upper_done_ptr)
 *                                    (DCP_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       pointer to callback-function
 *
 * RQB return values via callback-function:
 *     LSA_UINT16         response:      *_RSP_OK
 *                                       *_RSP_ERR_SYNTAX
 *                                       *_RSP_ERR_RESOURCE
 *                                       *_RSP_ERR_SYS_PATH
 *                                       possibly:
 *                                       *_RSP_ERR_PARAM
 *                                       *_RSP_ERR_LOWER_LAYER
 *                                       or others
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of dcp
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
LSA_VOID  DCP_UPPER_IN_FCT_ATTR  dcp_open_channel(
    DCP_UPPER_RQB_PTR_TYPE                        upper_rqb_ptr
);

/*=============================================================================
 * function name:  dcp_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:   *_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of dcp
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_UINT16        response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  or others
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
LSA_VOID  DCP_UPPER_IN_FCT_ATTR  dcp_close_channel(
    DCP_UPPER_RQB_PTR_TYPE                         upper_rqb_ptr
);

/*=============================================================================
 * function name:  dcp_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     DCP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of dcp
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB args:                    Depend on kind of request.
 *     
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    *_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     LSA_UINT16        response:  *_RSP_OK
 *                                  *_RSP_ERR_SYNTAX
 *                                  *_RSP_ERR_REF
 *                                  *_RSP_ERR_SEQUENCE
 *                                  possibly:
 *                                  *_RSP_OK_EOM
 *                                  *_RSP_ERR_RESOURCE
 *                                  *_RSP_ERR_PARAM
 *                                  *_RSP_ERR_LOC_ABORT
 *                                  *_RSP_ERR_REM_ABORT
 *                                  *_RSP_ERR_LOC_NET
 *                                  *_RSP_ERR_REM_NET
 *                                  *_RSP_ERR_PROTOCOL
 *                                  *_RSP_ERR_TIMEOUT
 *                                  *_RSP_ERR_DEV
 *                                  *_RSP_ERR_ROUTE
 *                                  *_RSP_ERR_OWN_ADDR
 *                                  *_RSP_ERR_LOWER_LAYER
 *                                  *_RSP_ERR_WITHDRAW
 *                                  or others
 *     All other RQB parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  DCP_UPPER_IN_FCT_ATTR  dcp_request(
    DCP_UPPER_RQB_PTR_TYPE                   upper_rqb_ptr
);

/*====  out functions  =====*/

/*****************************************************************************/
/*  end of file dcp_usr.h                                                 */
/*****************************************************************************/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of DCP_USR_H */
