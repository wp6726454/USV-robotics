
#ifndef LSA_SYS_H                        /* ----- reinclude-protection ----- */
#define LSA_SYS_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2010 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: Layer Stack Architecture (LSA)    :P&          */
/*                                                                           */
/*  P a c k a g e         &W: LSA-Global                        :W&          */ 
/*                                                                           */
/*  C o m p o n e n t     &C: Global Definitions                :C&          */
/*                                                                           */
/*  F i l e               &F: lsa_sys.h                         :F&          */
/*                                                                           */
/*  V e r s i o n         &V: LSA_GLOB_P02.01.00.00_00.13.02.01 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2010-09-16                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface for LSA                                                 */
/*  Defines constants, types and macros for LSA.                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*  2000-11-22  P 01.00.01.03  ws   Im Kommentar: Einige deutsche Begriffe   */
/*                                  gegen englische ausgetauscht.            */
/*  2001-07-26  P02.00.01.001  ws   *_USER_ID_TYPE von lsa_cfg.h nach        */
/*                                  lsa_sys.h verlagert. Ausserdem: Statt    */
/*                                  drei User-ID-Typen gibt es nur noch eine.*/
/*  2001-07-27  P02.00.01.001  ws   __LSA_SYS_H geaendert nach LSA_SYS_H.    */
/*  2001-07-27  P02.00.01.001  ws   LSA_HOST_PTR_ARE_EQUAL() neu definiert.  */
/*  2001-07-27  P02.00.01.001  ws   LSA_FATAL_ERROR_TYPE um void-Pointer er- */
/*                                  gaenzt.                                  */
/*  2001-07-30  P02.00.01.001  ws   Schutz vor erneutem Includieren an den   */
/*                                  Dateianfang verlagert.                   */
/*  2001-09-27  P02.00.01.001  ws   LSA_TIMER_TYPE_OFFSET neu.               */
/*  2001-09-27  P02.00.01.001  ws   LSA_COMPONENT_NAME_SIZE neu.             */
/*  2001-09-28  P02.00.01.001  ws   LSA_RET_OK_HIGHEST und                   */
/*                                  LSA_RET_ERR_HIGHEST geloescht; neu:      */
/*                                  LSA_RET_OK_OFFSET und LSA_RET_ERR_OFFSET */
/*  2001-10-05  P02.00.01.001  ws   LSA_RET_ERR_SEQUENCE neu.                */
/*  2001-10-08  P02.00.01.001  ws   Kommentare ergaenzt.                     */
/*  2001-10-09  P02.00.01.001  ws   Definitionen von LSA_TIMER_TYPE_ONE_SHOT,*/
/*                                  LSA_TIMER_TYPE_CYCLIC, LSA_TIME_BASE_1MS */
/*                                  bis LSA_TIME_BASE_100S geaendert um      */
/*                                  "Null" zu vermeiden.                     */
/*  2002-01-30  P02.00.01.002  ws   Kommentar ergaenzt.                      */
/*  2002-01-31  P02.00.01.002  ws   Statt LSA_COMP_ID_S7TCP jetzt            */
/*                                  LSA_COMP_ID_STCP.                        */
/*  2002-03-08  P02.00.01.003  ws   Versionierung an die neue KM-Prozess-    */
/*                                  beschreibung angepasst (d. h.            */
/*                                  LSA_VERSION_TYPE geaendert).             */
/*  2002-03-08  P02.00.01.003  ws   LSA_COMPONENT_NAME_SIZE durch            */
/*                                  LSA_PREFIX_SIZE ersetzt.                 */
/*  2002-03-12  P02.00.01.003  ws   LSA_COMP_ID_EDD und LSA_COMP_ID_ACP neu. */
/*  2002-03-12  P02.00.01.003  ws   LSA_COMP_ID_DPV2M geaendert nach         */
/*                                  LSA_COMP_ID_DPTM.                        */
/*  2002-03-12  P02.00.01.003  ws   LSA_COMP_ID_TGSDB geaendert nach         */
/*                                  LSA_COMP_ID_TGS.                         */
/*  2002-05-17  P2.0.0.0_0.1.4.1 ws Versionierung an die neue KM-Prozess-    */
/*                                  beschreibung V0.94 vom 29.04.2002        */
/*                                  angepasst.                               */
/*  2002-05-17  P2.0.0.0_0.1.4.1 ws Timer Types und Time Base mit #ifndef    */
/*                                  und #endif "geklammert" und              */
/*                                  LSA_TIMER_TYPE_OFFSET geloescht.         */
/*  2002-05-17  P2.0.0.0_0.1.4.1 ws ARTS 99116: LSA_FATAL_ERROR_TYPE: Ein    */
/*                                  Parameter ergaenzt und ein Parameter     */
/*                                  umbenannt.                               */
/*  2002-07-02  P2.0.0.0_0.1.5.1 ws Defines LSA_COMP_ID_SOCK,                */
/*                                  LSA_COMP_ID_CD7 und LSA_COMP_ID_FWUP neu.*/ 
/*  2002-07-26  V2.0.0.0_0.1.6.1 ws ARTS 51557: LSA_HOST_PTR_ARE_EQUAL() von */
/*                                  lsa_sys.h nach lsa_cfg.h verlagert.      */
/*  2002-10-28  P2.1.0.0_0.1.1.1 ws Define LSA_COMP_ID_LTRC neu.             */
/*  2003-04-30  P2.1.0.0_0.2.1.1 ws ARTS+ AP00012220: Anzahl der Zeichen     */
/*                                  fuer PREFIX von 4 auf 10 erhoeht: Define */
/*                                  LSA_PREFIX_SIZE angepasst.               */
/*  2003-04-30  P2.1.0.0_0.2.1.1 ws Defines LSA_COMP_ID_ESD,                 */
/*                                  LSA_COMP_ID_PCSRT, LSA_COMP_ID_CM,       */
/*                                  LSA_COMP_ID_CLRPC, LSA_COMP_ID_TSL7 neu. */
/*  2003-04-30  P2.1.0.0_0.2.1.1 ws Define LSA_COMP_ID_CD7 nach              */
/*                                  LSA_COMP_ID_DCP umbenannt.               */
/*  2003-04-30  P2.1.0.0_0.2.1.1 ws ARTS+ AP00010284: Defines                */
/*                                  LSA_TIME_BASE_1US, LSA_TIME_BASE_10US,   */
/*                                  LSA_TIME_BASE_100US neu.                 */
/*  2003-08-26  P2.1.0.0_0.2.2.1 ws Define LSA_COMP_ID_NARE neu.             */
/*  2003-12-08  P2.1.0.0_0.3.1.1 ws Define LSA_COMP_ID_PCIOX neu.            */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00132068: Fuer C++-Compiler      */
/*                                  Datei mit C-Bindung gekennzeichnet.      */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws ARTS+ AP00167458: Copyright abgeaendert. */
/*  2004-06-30  P2.1.0.0_0.4.1.1 ws Define LSA_COMP_ID_PCIOX geloescht,      */
/*                                  Defines LSA_COMP_ID_NTPC und             */
/*                                  LSA_COMP_ID_OUC neu.                     */
/*  2004-11-30  P2.1.0.0_0.4.2.1 ws Defines LSA_COMP_ID_IRTM,                */
/*                                  LSA_COMP_ID_LLDP, LSA_COMP_ID_OHA,       */
/*                                  LSA_COMP_ID_GSY und LSA_COMP_ID_MRP neu. */
/*  2005-07-22  P2.1.0.0_0.5.1.1 ws Trace: Neu: Pseudo-Makros als Scanner-   */
/*                                  Kennung fuer die symbolische Anzeige von */
/*                                  Konstanten.                              */
/*  2006-03-28  P2.1.0.0_0.7.1.1 ws ARTS+ AP00268519: LSA_RQB_HEADER-        */
/*                                  Definition neu: LSA_RESPONSE_TYPE neu.   */
/*  2006-03-29  P2.1.0.0_0.7.1.1 ws Defines LSA_COMP_ID_SNMPX,               */
/*                                  LSA_COMP_ID_POF und                      */
/*                                  LSA_COMP_ID_EDD_ERTEC neu, Define        */
/*                                  LSA_COMP_ID_IRTM geloescht; Define       */
/*                                  LSA_COMP_ID_DGWT umbenannt nach          */
/*                                  LSA_COMP_ID_DSIF.                        */
/*  2007-03-22  P2.1.0.0_0.8.1.1 ws ARTS+ AP00424201: Variable priority im   */
/*                                  LSA_RQB_HEADER neu: Hier: Konstanten     */
/*                                  LSA_PRIORITY_* neu.                      */
/*  2007-03-22  P2.1.0.0_0.8.1.1 ws Define LSA_COMP_ID_TCIP neu.             */
/*  2007-07-10  P2.1.0.0_0.9.1.1 ws Define LSA_COMP_ID_REMA neu.             */
/*  2007-08-08 P2.1.0.0_0.10.1.1 ws ARTS+ AP00498913: LSA_CHAR neu. Typ von  */
/*                                  lsa_prefix in LSA_VERSION_TYPE jetzt     */
/*                                  array von LSA_CHAR.                      */
/*  2008-12-01 P2.1.0.0_0.11.2.1 ws Defines LSA_COMP_ID_L7AA,                */
/*                                  LSA_COMP_ID_S7PT, LSA_COMP_ID_PSI,       */
/*                                  LSA_COMP_ID_ACFG, LSA_COMP_ID_PNIO neu.  */
/*  2009-02-09 P2.1.0.0_0.12.1.1 ws Defines LSA_COMP_ID_EDDS,                */
/*                                  LSA_COMP_ID_EDDI, LSA_COMP_ID_EDDP,      */
/*                                  LSA_COMP_ID_PCIOX, LSA_COMP_ID_PNDEVDRV, */
/*                                  LSA_COMP_ID_PCMODTST neu.                */
/*  2010-02-05 P2.1.0.0_0.12.4.1 ws ARTS+ AP00908402: Define                 */
/*                                  LSA_COMP_ID_BPLX neu.                    */ 
/*  2010-06-04 P2.1.0.0_0.13.1.1 ws ARTS+ AP00248379: prefix_init() und      */
/*                                  prefix_undo_init() um eine asynchrone    */
/*                                  Schnittstelle erweitert: Responsewert    */
/*                                  LSA_RET_OK_ASYNC neu.                    */
/*  2009-09-16 P2.1.0.0_0.13.2.1 ws Defines LSA_COMP_ID_HIF,                 */
/*                                  LSA_COMP_ID_PNBOARDS und LSA_COMP_ID_IOH */
/*                                  neu.                                     */
/*                                                                           */
/*****************************************************************************/


                              
/*===========================================================================*/
/* basic types for lsa according ANSI C1999 Standard (ISO/IEC 9899:1999)     */
/*                                                                           */
/* See also file lsa_cfg.h/txt.                                              */
/*===========================================================================*/
#define LSA_CHAR   char          /* An object declared as type char is large */
                                 /* enough to store any member of the basic  */
                                 /* execution character set, see ANSI C1999. */



/*===========================================================================*/
/*                            constants for lsa                              */
/*===========================================================================*/

/*===== for lsa_component_id =====*/
PTRC_SUBLABEL_LIST_OPEN("LSA_COMP_ID")
#define LSA_COMP_ID_L2        0x0001 /* AMPRO2                               */
#define LSA_COMP_ID_DPMC      0x0002 /* DP-Modul-Container (DP-Master)       */
#define LSA_COMP_ID_S7L4      0x0003 /* S7-Transport                         */
#define LSA_COMP_ID_DPTM      0x0004 /* DP/T-M                               */
#define LSA_COMP_ID_TG        0x0005 /* Transport-Gateway                    */
#define LSA_COMP_ID_RPC7      0x0006 /* Remote Procedure Call for SPS7       */
#define LSA_COMP_ID_DSIF      0x0007 /* Datensatz-Interface                  */
#define LSA_COMP_ID_TGS       0x0008 /* Transport-Gateway-SDB-Interpreter    */
#define LSA_COMP_ID_V1SL      0x0009 /* DPV1-Slave-Paket                     */
#define LSA_COMP_ID_HV        0x000a /* Hochverfuegbare Ebene7-Verbindungen  */
#define LSA_COMP_ID_KA        0x000b /* Kanalabgleich (redundant systems     */
                                     /* need it)                             */
#define LSA_COMP_ID_STCP      0x000c /* s7-connections on top of TCP/IP      */
                                     /* (RFC1006)                            */
#define LSA_COMP_ID_LPW       0x000d /* Large Package Wrapper (large         */
                                     /* packages on top of PROFIBUS DP)      */
#define LSA_COMP_ID_DPMI      0x000e /* Mess-Initiator (performance          */
                                     /* diagnosis of PROFIBUS DP)            */
#define LSA_COMP_ID_CPT       0x000f /* CP-Transport (SCP-adapter for        */
                                     /* firmware), LSA-SCP-Adapter           */
#define LSA_COMP_ID_EDD       0x0010 /* Ethernet-Device-Driver for IBC32,    */
                                     /* old Software Versions                */
#define LSA_COMP_ID_ACP       0x0011 /* Alarm-Customer-Producer for          */
                                     /* real-time ethernet                   */
#define LSA_COMP_ID_SOCK      0x0012 /* LSA-Socket-Adapter                   */
#define LSA_COMP_ID_DCP       0x0013 /* Discovery and Configuration Protocol */
                                     /* (assistance tools for communication: */
                                     /* e.g. "Knotentaufe", Lifelist, ...)   */
#define LSA_COMP_ID_FWUP      0x0014 /* Firmware- (Operating System-) Update */
#define LSA_COMP_ID_LTRC      0x0015 /* LSA-Trace                            */
#define LSA_COMP_ID_ESD       0x0016 /* Ethernet SDB interpreter             */
#define LSA_COMP_ID_PCSRT     0x0017 /* Win2k driver for aSRT and SRT        */
                                     /* (IO controllers and IO devices at    */
                                     /* Ethernet on Win2k-PCs)               */
#define LSA_COMP_ID_CM        0x0018 /* PN IO Context Management (IO         */
                                     /* controller and IO devices at         */
                                     /* Ethernet)                            */
#define LSA_COMP_ID_CLRPC     0x0019 /* Connectionless Remote Procedure Call */
                                     /* (OSF DCE RPC for Ethernet)           */
#define LSA_COMP_ID_TSL7      0x001a /* Gateway MPI/DP-> WAN in the layer 7  */
                                     /* of teleservice adapters              */
#define LSA_COMP_ID_NARE      0x001b /* Name Resolver at Ethernet            */
#define LSA_COMP_ID_PCIOX     0x001c /* Simulator for IO Device and          */  
                                     /* IO Controller under Win2k and WinXP  */
#define LSA_COMP_ID_NTPC      0x001d /* Network Time Protocol - Client       */
#define LSA_COMP_ID_OUC       0x001e /* OpenUserCommunication                */
/*
#define LSA_COMP_ID_IRTM      0x001f    PNIO IRT Management                  */
#define LSA_COMP_ID_LLDP      0x0020 /* Link Layer Discovery Protocol        */
#define LSA_COMP_ID_OHA       0x0021 /* Option Handler                       */
#define LSA_COMP_ID_GSY       0x0022 /* Generic Sync: Clock pulse and clock  */
                                     /* time synchronization                 */
#define LSA_COMP_ID_MRP       0x0023 /* Media Redundancy Protocol            */
#define LSA_COMP_ID_SNMPX     0x0024 /* Master Agent according to the AgentX */
                                     /* concept (RFC2741)                    */
#define LSA_COMP_ID_POF       0x0025 /* Polymer Optical Fiber                */
#define LSA_COMP_ID_EDD_ERTEC 0x0026 /* Ethernet-Device-Driver for ERTEC     */
                                     /* (Classic), old Software Versions     */
#define LSA_COMP_ID_TCIP      0x0027 /* TCP/IP Stack and SNMPv1 Clients      */
#define LSA_COMP_ID_REMA      0x0028 /* Remanence                            */
#define LSA_COMP_ID_L7AA      0x0029 /* Layer 7-Application Association      */
                                     /* Establishment (=Applikationsbeziehung*/
                                     /* einrichten) for "classic world"      */
#define LSA_COMP_ID_S7PT      0x002a /* S7 Plus Transport: Application       */
                                     /* Association Handling for "plus world"*/
#define LSA_COMP_ID_PSI       0x002b /* Profinet Stack Interface             */
#define LSA_COMP_ID_ACFG      0x002c /* Autoconfiguration Manager for        */
                                     /* PROFINET IO                          */
#define LSA_COMP_ID_PNIO      0x002d /* PROFINET IO Basisstack               */
#define LSA_COMP_ID_EDDS      0x002e /* Ethernet-Device-Driver for IBC32,    */
                                     /* new Software Versions                */
#define LSA_COMP_ID_EDDI      0x002f /* Ethernet-Device-Driver for ERTEC     */
                                     /* (Classic), new Software Versions     */
#define LSA_COMP_ID_EDDP      0x0030 /* Ethernet-Device-Driver for ERTEC+    */
#define LSA_COMP_ID_PNDEVDRV  0x0031 /* PROFINET IO Development Driver for   */
                                     /* PC based Test Applications           */
#define LSA_COMP_ID_PCMODTST  0x0032 /* Module Test Application for          */
                                     /* PROFINET IO LSA Components           */
#define LSA_COMP_ID_BPLX      0x0033 /* S7+ Back Plane Interface Layer 2/4   */
#define LSA_COMP_ID_HIF       0x0034 /* Hostinterface: Interface to PNIO     */
                                     /* Stack for multiprocessing            */
#define LSA_COMP_ID_PNBOARDS  0x0035 /* Firmware for PNBoards                */
#define LSA_COMP_ID_IOH       0x0036 /* IO-Handler                           */
#define LSA_COMP_ID_UNKNOWN   0xfffe
#define LSA_COMP_ID_UNUSED    0xffff
PTRC_SUBLABEL_LIST_CLOSE("LSA_COMP_ID")

/*===== for lsa_prefix =====*/
#define LSA_PREFIX_SIZE                 15    /* including '\0', see *_ver.h */

/*=====  function return-values  =====*/
PTRC_SUBLABEL_LIST_OPEN("LSA_RET")
#define LSA_RET_OK                      LSA_OK /* without errors */
#define LSA_RET_OK_ASYNC                0x04   /* Without errors, the        */
                                               /* functionality will end     */
                                               /* asynchronously. The        */
                                               /* component will call an out */
                                               /* function to end the        */
                                               /* functionality.             */
#define LSA_RET_OK_TIMER_RESTARTED      0x0a   /* timer has been restarted */
#define LSA_RET_OK_TIMER_NOT_RUNNING    0x0b   /* timer was not running; */
                                               /* timer is stopped       */
#define LSA_RET_ERR_RESOURCE            0x84   /* no resources */
#define LSA_RET_ERR_PARAM               0x86   /* parameter is wrong */
#define LSA_RET_ERR_SEQUENCE            0x87   /* wrong sequence */
#define LSA_RET_ERR_SYS_PATH            0x91   /* wrong system-path */
#define LSA_RET_ERR_NO_TIMER            0xa0   /* no timer has been allocated*/
#define LSA_RET_ERR_TIMER_IS_RUNNING    0xa1   /* because timer is running */
                                               /* timer has not been       */
                                               /* deallocated              */
                         /* note: other numbers are used by *_RSP_* for RQBs */

#define LSA_RET_OK_OFFSET               0x40
          /* lsa-components may use 0x40 - 0x7F for own function ok-codes */
#define LSA_RET_ERR_OFFSET              0xc0
          /* lsa-components may use 0xc0 - 0xfe for own function error-codes */
          /* 0xff is reserved for *_RSP_UNUSED */
PTRC_SUBLABEL_LIST_CLOSE("LSA_RET")
                                          

/*=====  priority  =====*/
PTRC_SUBLABEL_LIST_OPEN("LSA_PRIORITY")
#define LSA_PRIORITY_DEFAULT               0    /* priority: Default,        */
                                                /* equivalent to priority Low*/
#define LSA_PRIORITY_LOWEST                1    /* priority: Lowest */
#define LSA_PRIORITY_LOWER                 2    /* priority: Lower */
#define LSA_PRIORITY_LOW                   3    /* priority: Low */
#define LSA_PRIORITY_MIDDLE                4    /* priority: Middle */
#define LSA_PRIORITY_HIGH                  5    /* priority: High */
#define LSA_PRIORITY_HIGHER                6    /* priority: Higher */
#define LSA_PRIORITY_HIGHEST               7    /* priority: Highest */
PTRC_SUBLABEL_LIST_CLOSE("LSA_PRIORITY")


/*=====  timer types  =====*/
#ifndef LSA_TIMER_TYPE_DEFINED
PTRC_SUBLABEL_LIST_OPEN("LSA_TIMER_TYPE")
#define LSA_TIMER_TYPE_ONE_SHOT            1    /* one shot timer */
#define LSA_TIMER_TYPE_CYCLIC              2    /* cyclic timer */
PTRC_SUBLABEL_LIST_CLOSE("LSA_TIMER_TYPE")
#endif

/*=====  time base  =====*/
#ifndef LSA_TIME_BASE_DEFINED
PTRC_SUBLABEL_LIST_OPEN("LSA_TIME_BASE")
#define LSA_TIME_BASE_1US                  1    /* time base: 1 usec */
#define LSA_TIME_BASE_10US                 2    /* time base: 10 usec */
#define LSA_TIME_BASE_100US                3    /* time base: 100 usec */
#define LSA_TIME_BASE_1MS                  4    /* time base: 1 msec */
#define LSA_TIME_BASE_10MS                 5    /* time base: 10 msec */
#define LSA_TIME_BASE_100MS                6    /* time base: 100 msec */
#define LSA_TIME_BASE_1S                   7    /* time base: 1 sec */
#define LSA_TIME_BASE_10S                  8    /* time base: 10 sec */
#define LSA_TIME_BASE_100S                 9    /* time base: 100 sec */
PTRC_SUBLABEL_LIST_CLOSE("LSA_TIME_BASE")
#endif



/*===========================================================================*/
/*                              types for lsa                                */
/*===========================================================================*/

/*=====  lsa common request block type: definition of parameter type  =====*/
#define LSA_RESPONSE_TYPE             LSA_UINT16    /* for response */
#define LSA_PRIORITY_TYPE             LSA_UINT8     /* for priority */


/*=====  version according to "KM-Prozessbeschreibung"  =====*/

/* see *_ver.h
 */
typedef struct lsa_version_tag
{
    LSA_UINT16  lsa_component_id;               /* LSA_COMP_ID_* */
    LSA_CHAR    lsa_prefix[LSA_PREFIX_SIZE];
    LSA_UINT8   kind;
    LSA_UINT8   version;
    LSA_UINT8   distribution;
    LSA_UINT8   fix;
    LSA_UINT8   hotfix;
    LSA_UINT8   project_number;
    LSA_UINT8   increment;
    LSA_UINT8   integration_counter;
    LSA_UINT8   gen_counter;
} LSA_VERSION_TYPE;


/*=====  fatal-error  =====*/
typedef struct  lsa_fatal_error_tag
{
    LSA_UINT16         lsa_component_id;        /* LSA_COMP_ID_* */
    LSA_UINT16         module_id;               /* *_MODULE_ID  */
    LSA_UINT16         line;                    /* line of file */
    LSA_UINT16         error_data_length;       /* length in bytes of the    */
                                                /* memory area addressed by  */
                                                /* error_data_ptr            */
    LSA_UINT32         error_code[4];
    LSA_VOID_PTR_TYPE  error_data_ptr;
} LSA_FATAL_ERROR_TYPE;


/*=====  user_id for RQBs, rqb- , memory- and timer-functions  =====*/     
typedef union lsa_user_id_tag
{
    LSA_UINT8          uvar8;
    LSA_UINT16         uvar16;
    LSA_UINT32         uvar32;
    LSA_INT8           svar8;
    LSA_INT16          svar16;
    LSA_INT32          svar32;
    LSA_UINT8          uvar8_array[4];
    LSA_UINT16         uvar16_array[2];
    LSA_UINT32         uvar32_array[1];
    LSA_INT8           svar8_array[4];
    LSA_INT16          svar16_array[2];
    LSA_INT32          svar32_array[1];
    LSA_VOID_PTR_TYPE  void_ptr;
} LSA_USER_ID_TYPE;



/*===========================================================================*/
/*                             macros for lsa                                */
/*===========================================================================*/



/*****************************************************************************/
/*  end of file LSA_SYS.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LSA_SYS_H */
