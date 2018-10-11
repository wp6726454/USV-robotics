//////////////////////////////////////////////////////////////////////
//// Walter Register Include File
//// Version:           0.6
//// Version RSL-Tools: 0.6.9B
//// Erstellungsdatum:  08.11.2010
////
//////////////////////////////////////////////////////////////////////

#ifndef IRTE_REV07_RSL_H                 /* ----- reinclude-protection ----- */
#define IRTE_REV07_RSL_H
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
/*  C o m p o n e n t     &C: PNExplorer [DefExtract]                   :C&  */
/*                                                                           */
/*  F i l e               &F: irte_rev07_rsl.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef RSL_MESSAGE
/*  xx.xx.xx    xx    initial version.                                       */
/*  translated by: DefExtract, V 2.0!                                        */
#endif
/*****************************************************************************/
         
                                   
/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/


// ------------------------------------------
// Makros for access to peripheral registers
// ------------------------------------------

//////////////////////////////////////////////////////////////////////
//Registers for Module IRTE
//////////////////////////////////////////////////////////////////////




#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0                                                                  0x00000008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_WRONG_SOURCE_UDP                                                0x00200000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_WRONG_SOURCE_UDP                                                21
// ACCESS__WRONG_SOURCE_UDP: r,w
// DESCR__WRONG_SOURCE_UDP: 1: Der empfangene Source-UDP-Port eines RToverUDP-Frames \n    stimmt nicht mit dem erwarteten Source-UDP-Port überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_WRONG_SOURCE_IP                                                 0x00100000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_WRONG_SOURCE_IP                                                 20
// ACCESS__WRONG_SOURCE_IP: r,w
// DESCR__WRONG_SOURCE_IP: 1: Die empfangene Source-IP eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten Source-IP-Adresse überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_IP_CHECKSUM_ERROR                                               0x00080000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_IP_CHECKSUM_ERROR                                               19
// ACCESS__IP_CHECKSUM_ERROR: r,w
// DESCR__IP_CHECKSUM_ERROR: 1: Die empfangene IP-Header-Checksum eines RToverUDP-\n    Frames stimmt nicht mit der erwarteten IP-Checksum überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_WRONG_UDP_LENGTH                                                0x00040000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_WRONG_UDP_LENGTH                                                18
// ACCESS__WRONG_UDP_LENGTH: r,w
// DESCR__WRONG_UDP_LENGTH: 1: Die empfangene UDP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_WRONG_IP_LENGTH                                                 0x00020000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_WRONG_IP_LENGTH                                                 17
// ACCESS__WRONG_IP_LENGTH: r,w
// DESCR__WRONG_IP_LENGTH: 1: Die empfangene IP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_WRONG_SENDTIME                                           0x00010000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_WRONG_SENDTIME                                           16
// ACCESS__ENABLE_WRONG_SENDTIME: r,w
// DESCR__ENABLE_WRONG_SENDTIME: 1: Eintrag im  Trace-Buffer wenn der spezifizierte Zeitpunkt eines    RT-Telegramms ablaufen ist.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_NO_SENDDATA                                              0x00008000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_NO_SENDDATA                                              15
// ACCESS__ENABLE_NO_SENDDATA: r,w
// DESCR__ENABLE_NO_SENDDATA: 1: Zum spezifizierten Zeitpunkt ist kein RT-Telegramm zum     Versenden vorhanden sind.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_NO_SRT_ENTRY                                             0x00004000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_NO_SRT_ENTRY                                             14
// ACCESS__ENABLE_NO_SRT_ENTRY: r,w
// DESCR__ENABLE_NO_SRT_ENTRY: 1: In der NRT-Phase konnte beim Empfang eines SRT-    Telegramms kein Eintrag in der ACW-Liste gefunden werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_BUF_OVERLOAD                                             0x00002000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_BUF_OVERLOAD                                             13
// ACCESS__ENABLE_BUF_OVERLOAD: r,w
// DESCR__ENABLE_BUF_OVERLOAD: 1: Beim Telegrammempfang sind alle Datenpuffer übergelaufen
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_NO_TABLE_ENTRY                                                  0x00001000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_NO_TABLE_ENTRY                                                  12
// ACCESS__NO_TABLE_ENTRY: r,w
// DESCR__NO_TABLE_ENTRY: 1: Sourceadresse kann nicht in die MAC-Adresstabelle eingetragen    werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_NO_ACW                                                   0x00000800
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_NO_ACW                                                   11
// ACCESS__ENABLE_NO_ACW: r,w
// DESCR__ENABLE_NO_ACW: 1: Empfang eines Zeit-Telegramms im IRT-Zeitbereich und dabei     kein ACW mit der richtigen FrameID und opt. MAC-Adresse     gefunden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_WRONG_LENGTH                                             0x00000400
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_WRONG_LENGTH                                             10
// ACCESS__ENABLE_WRONG_LENGTH: r,w
// DESCR__ENABLE_WRONG_LENGTH: 1: Empfang eines RT-Telegramms mit einer falschen Länge.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_RCVBUFOVFL                                               0x00000200
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_RCVBUFOVFL                                               9
// ACCESS__ENABLE_RCVBUFOVFL: r,w
// DESCR__ENABLE_RCVBUFOVFL: 1: Puffer-Überlauf beim Empfang eines RT-Telegramms.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_CRC_ERROR                                                0x00000100
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_CRC_ERROR                                                8
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Empfang eines Telegramms mit CRC-Fehler im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_WRONG_FRAMEID                                            0x00000080
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_WRONG_FRAMEID                                            7
// ACCESS__ENABLE_WRONG_FRAMEID: r,w
// DESCR__ENABLE_WRONG_FRAMEID: 1: Empfang eines RT-Telegramms mit der falschen FrameID    und opt. MAC-Adresse im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_IRT_TEL_ERROR                                            0x00000040
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_IRT_TEL_ERROR                                            6
// ACCESS__ENABLE_IRT_TEL_ERROR: r,w
// DESCR__ENABLE_IRT_TEL_ERROR: 1: Eintrag im  Trace-Buffer, wenn ein RT-Telegramm kleiner    als 20 Byte empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_NO_IRT_TEL                                               0x00000020
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_NO_IRT_TEL                                               5
// ACCESS__ENABLE_NO_IRT_TEL: r,w
// DESCR__ENABLE_NO_IRT_TEL: 1: Eintrag im  Trace-Buffer, wenn bis zum erwarteten Zeitpunkt kein RT-Telegramm empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_REC_NRT                                                  0x00000010
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_REC_NRT                                                  4
// ACCESS__ENABLE_REC_NRT: r,w
// DESCR__ENABLE_REC_NRT: 1: Eintrag im  Trace-Buffer beim Empfang eines NRT-Telegramms \n    (kein Ether-Type) im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_OUT_OF_BOUNDS                                            0x00000008
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_OUT_OF_BOUNDS                                            3
// ACCESS__ENABLE_OUT_OF_BOUNDS: r,w
// DESCR__ENABLE_OUT_OF_BOUNDS: 1: Eintrag im  Trace-Buffer beim Empfang eines iSRT-    Frames ausserhalb der iSRT-Zeitphase.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_REC_REPL_FRAME                                           0x00000004
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_REC_REPL_FRAME                                           2
// ACCESS__ENABLE_REC_REPL_FRAME: r,w
// DESCR__ENABLE_REC_REPL_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines    Ersatz-Telegramms im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_REC_TIME_FRAME                                           0x00000002
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_REC_TIME_FRAME                                           1
// ACCESS__ENABLE_REC_TIME_FRAME: r,w
// DESCR__ENABLE_REC_TIME_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines Zeittelegramms      im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0__MSK_ENABLE_REC_IRT_FRAME                                            0x00000001
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_SHFT_ENABLE_REC_IRT_FRAME                                            0
// ACCESS__ENABLE_REC_IRT_FRAME: r,w
// DESCR__ENABLE_REC_IRT_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines RT-Frames im     IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P0_RST__VAL                       0x00000000


#define IRTE_R7_REG_LINE_DELAY_P0                                                                           0x0000000C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LINE_DELAY_P0__MSK_LINE_DELAY0                                                              0x00000001
#define IRTE_R7_REG_LINE_DELAY_P0_SHFT_LINE_DELAY0                                                              0
// ACCESS__LINE_DELAY0: r,w
// DESCR__LINE_DELAY0: Leitungsverzögerung zum Nachbar-Switch (bzw. dem angeschlossenen Port des Nachbar-Switches) in 1ns-Genauigkeit
#define IRTE_R7_REG_LINE_DELAY_P0_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_CONTROL_P0                                                                          0x00000010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_ORG_PAUSE_LEARNING_DISABLE                                              0x00008000
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_ORG_PAUSE_LEARNING_DISABLE                                              15
// ACCESS__ORG_PAUSE_LEARNING_DISABLE: r,w
// DESCR__ORG_PAUSE_LEARNING_DISABLE: 0: ORG- und Pause-Telegramme werden entsprechend Bit 4   (Learning-Enable)  bzw. Bit 5 (MC-Learning-Enable) behandelt1: ORG- und Pause-Telegramme werden nicht gelernt
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_ADD_LINEDELAY                                                           0x00004000
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_ADD_LINEDELAY                                                           14
// ACCESS__ADD_LINEDELAY: r,w
// DESCR__ADD_LINEDELAY: 0: Addition des Line-Delays beim Sender1: Addition des Line-Delays beim Empfänger
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_MONITOR_ACYCLIC                                                         0x00002000
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_MONITOR_ACYCLIC                                                         13
// ACCESS__MONITOR_ACYCLIC: r,w
// DESCR__MONITOR_ACYCLIC: 0: Der azyklische Verkehr auf diesem Port wird nicht beobachtet1: Der gesamte azyklische Verkehr auf diesem Port wird beobachtet, d.h. alle Telegramme ausser vom Typ Ether-Type mit Frame-ID 0080-FBFF.
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_SHORT_ENABLE                                                            0x00001000
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_SHORT_ENABLE                                                            12
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 0: Telegramme < 64 Byte werden beim Empfang gefiltert.1: keine Prüfung auf Mindest-Telegrammlänge beim Empfang
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_MONITOR_CYCLIC                                                          0x00000800
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_MONITOR_CYCLIC                                                          11
// ACCESS__MONITOR_CYCLIC: r,w
// DESCR__MONITOR_CYCLIC: 0: Der zyklische Verkehr auf diesem Port wird nicht beobachtet1: Der zyklische Verkehr auf diesem Port wird beobachtet
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_REDUNDANCE_ENABLE                                                       0x00000400
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_REDUNDANCE_ENABLE                                                       10
// ACCESS__REDUNDANCE_ENABLE: r,w
// DESCR__REDUNDANCE_ENABLE: 0: Es wird kein redundanter Verkehr abgewickelt1: Es wird redundanter Verkehr abgewickelt
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_ISRT_ENABLE                                                             0x00000200
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_ISRT_ENABLE                                                             9
// ACCESS__ISRT_ENABLE: r,w
// DESCR__ISRT_ENABLE: 0: Es ist kein Zeitbereich für iSRT reserviert1: Es ist ein Zeitbereich für iSRT reserviert
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_EN_CHECK_SA                                                             0x00000100
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_EN_CHECK_SA                                                             8
// ACCESS__EN_CHECK_SA: r,w
// DESCR__EN_CHECK_SA: 0: Telegramme, deren Sourceadresse in der Adresstabelle     eingetragen sind, werden weitergeleitet.1: Telegramme, deren Sourceadresse nicht in der Adresstabelle     eingetragen sind werden gefiltert.
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_INGRESS_FILTERING                                                       0x00000080
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_INGRESS_FILTERING                                                       7
// ACCESS__INGRESS_FILTERING: r,w
// DESCR__INGRESS_FILTERING: 0: Im Tag Mode werden alle tagged Telegramme an diesem Port     empfangen. 1: Im Tag Mode wird ein Telegramm nur dann empfangen,     wenn die VLAN-Adresse des Telegramms mit dem VLAN-    Netzwerk am Empfangsport übereinstimmt.
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_ENABLE_PADDING                                                          0x00000040
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_ENABLE_PADDING                                                          6
// ACCESS__ENABLE_PADDING: r,w
// DESCR__ENABLE_PADDING: 0: Port macht kein  Padding für Telegramme < 64 Bytes1: Port macht  Padding für Telegramme < 64 Bytes
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_MC_LEARNING_ENABLE                                                      0x00000020
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_MC_LEARNING_ENABLE                                                      5
// ACCESS__MC_LEARNING_ENABLE: r,w
// DESCR__MC_LEARNING_ENABLE: 0: Nur die Sourceadressen von Unicast-Telegrammen werden     zum Lernen der Unicast-Adresstabelle genutzt1: Die Sourceadressen aller Telegramme werden zum Lernen von      Einträgen der Unicast-Adresstabelle genutzt.
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_LEARNING_ENABLE                                                         0x00000010
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_LEARNING_ENABLE                                                         4
// ACCESS__LEARNING_ENABLE: r,w
// DESCR__LEARNING_ENABLE: 0: Es werden keine neuen Einträge gelernt1: Dynamische Einträge der Unicast-Adress-Tabelle können aus    den Sourceadressen von Telegrammen gelernt  werden
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_ENABLE_SRT_PRIORITAET                                                   0x00000008
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_ENABLE_SRT_PRIORITAET                                                   3
// ACCESS__ENABLE_SRT_PRIORITAET: r,w
// DESCR__ENABLE_SRT_PRIORITAET: 0: Telegramme mit einem Typ-Feld Ether-Type werden nicht besonders behandelt1: Telegramme mit den Typ-Feld Ether-Type erhalten beim Weiterleiten SRT-Priorität. Aufteilung auf zyklisch/azyklisch erfolgt durch die Frame-ID des Telegramms.
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_EN_CUT_THROUGH                                                          0x00000004
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_EN_CUT_THROUGH                                                          2
// ACCESS__EN_CUT_THROUGH: r,w
// DESCR__EN_CUT_THROUGH: 0: Die Telegramme an Port Px müssen im K-RAM zwischen-gespeichert werden (store-and-foreward) 1: Die Telegramme an Port Px müssen, wenn möglich, direkt weitergeleitet werden (cut-through)
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_FULL_DUPLEX                                                             0x00000002
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_FULL_DUPLEX                                                             1
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: 0: Halbduplex1: Vollduplex
#define IRTE_R7_REG_NRT_CONTROL_P0__MSK_BAUDRATE                                                                0x00000001
#define IRTE_R7_REG_NRT_CONTROL_P0_SHFT_BAUDRATE                                                                0
// ACCESS__BAUDRATE: r,w
// DESCR__BAUDRATE: 0: Der Port wird im 100MBit-Modus betrieben1: Der Port wird im 10MBit-Modus betrieben
#define IRTE_R7_REG_NRT_CONTROL_P0_RST__VAL                               0x00000000


#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1                                                                  0x00001008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_WRONG_SOURCE_UDP                                                0x00200000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_WRONG_SOURCE_UDP                                                21
// ACCESS__WRONG_SOURCE_UDP: r,w
// DESCR__WRONG_SOURCE_UDP: 1: Der empfangene Source-UDP-Port eines RToverUDP-Frames \n    stimmt nicht mit dem erwarteten Source-UDP-Port überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_WRONG_SOURCE_IP                                                 0x00100000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_WRONG_SOURCE_IP                                                 20
// ACCESS__WRONG_SOURCE_IP: r,w
// DESCR__WRONG_SOURCE_IP: 1: Die empfangene Source-IP eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten Source-IP-Adresse überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_IP_CHECKSUM_ERROR                                               0x00080000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_IP_CHECKSUM_ERROR                                               19
// ACCESS__IP_CHECKSUM_ERROR: r,w
// DESCR__IP_CHECKSUM_ERROR: 1: Die empfangene IP-Header-Checksum eines RToverUDP-\n    Frames stimmt nicht mit der erwarteten IP-Checksum überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_WRONG_UDP_LENGTH                                                0x00040000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_WRONG_UDP_LENGTH                                                18
// ACCESS__WRONG_UDP_LENGTH: r,w
// DESCR__WRONG_UDP_LENGTH: 1: Die empfangene UDP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_WRONG_IP_LENGTH                                                 0x00020000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_WRONG_IP_LENGTH                                                 17
// ACCESS__WRONG_IP_LENGTH: r,w
// DESCR__WRONG_IP_LENGTH: 1: Die empfangene IP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_WRONG_SENDTIME                                           0x00010000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_WRONG_SENDTIME                                           16
// ACCESS__ENABLE_WRONG_SENDTIME: r,w
// DESCR__ENABLE_WRONG_SENDTIME: 1: Eintrag im  Trace-Buffer wenn der spezifizierte Zeitpunkt eines    RT-Telegramms ablaufen ist.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_NO_SENDDATA                                              0x00008000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_NO_SENDDATA                                              15
// ACCESS__ENABLE_NO_SENDDATA: r,w
// DESCR__ENABLE_NO_SENDDATA: 1: Zum spezifizierten Zeitpunkt ist kein RT-Telegramm zum     Versenden vorhanden sind.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_NO_SRT_ENTRY                                             0x00004000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_NO_SRT_ENTRY                                             14
// ACCESS__ENABLE_NO_SRT_ENTRY: r,w
// DESCR__ENABLE_NO_SRT_ENTRY: 1: In der NRT-Phase konnte beim Empfang eines SRT-    Telegramms kein Eintrag in der ACW-Liste gefunden werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_BUF_OVERLOAD                                             0x00002000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_BUF_OVERLOAD                                             13
// ACCESS__ENABLE_BUF_OVERLOAD: r,w
// DESCR__ENABLE_BUF_OVERLOAD: 1: Beim Telegrammempfang sind alle Datenpuffer übergelaufen
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_NO_TABLE_ENTRY                                                  0x00001000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_NO_TABLE_ENTRY                                                  12
// ACCESS__NO_TABLE_ENTRY: r,w
// DESCR__NO_TABLE_ENTRY: 1: Sourceadresse kann nicht in die MAC-Adresstabelle eingetragen    werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_NO_ACW                                                   0x00000800
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_NO_ACW                                                   11
// ACCESS__ENABLE_NO_ACW: r,w
// DESCR__ENABLE_NO_ACW: 1: Empfang eines Zeit-Telegramms im IRT-Zeitbereich und dabei     kein ACW mit der richtigen FrameID und opt. MAC-Adresse     gefunden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_WRONG_LENGTH                                             0x00000400
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_WRONG_LENGTH                                             10
// ACCESS__ENABLE_WRONG_LENGTH: r,w
// DESCR__ENABLE_WRONG_LENGTH: 1: Empfang eines RT-Telegramms mit einer falschen Länge.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_RCVBUFOVFL                                               0x00000200
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_RCVBUFOVFL                                               9
// ACCESS__ENABLE_RCVBUFOVFL: r,w
// DESCR__ENABLE_RCVBUFOVFL: 1: Puffer-Überlauf beim Empfang eines RT-Telegramms.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_CRC_ERROR                                                0x00000100
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_CRC_ERROR                                                8
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Empfang eines Telegramms mit CRC-Fehler im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_WRONG_FRAMEID                                            0x00000080
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_WRONG_FRAMEID                                            7
// ACCESS__ENABLE_WRONG_FRAMEID: r,w
// DESCR__ENABLE_WRONG_FRAMEID: 1: Empfang eines RT-Telegramms mit der falschen FrameID    und opt. MAC-Adresse im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_IRT_TEL_ERROR                                            0x00000040
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_IRT_TEL_ERROR                                            6
// ACCESS__ENABLE_IRT_TEL_ERROR: r,w
// DESCR__ENABLE_IRT_TEL_ERROR: 1: Eintrag im  Trace-Buffer, wenn ein RT-Telegramm kleiner    als 20 Byte empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_NO_IRT_TEL                                               0x00000020
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_NO_IRT_TEL                                               5
// ACCESS__ENABLE_NO_IRT_TEL: r,w
// DESCR__ENABLE_NO_IRT_TEL: 1: Eintrag im  Trace-Buffer, wenn bis zum erwarteten Zeitpunkt kein RT-Telegramm empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_REC_NRT                                                  0x00000010
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_REC_NRT                                                  4
// ACCESS__ENABLE_REC_NRT: r,w
// DESCR__ENABLE_REC_NRT: 1: Eintrag im  Trace-Buffer beim Empfang eines NRT-Telegramms \n    (kein Ether-Type) im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_OUT_OF_BOUNDS                                            0x00000008
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_OUT_OF_BOUNDS                                            3
// ACCESS__ENABLE_OUT_OF_BOUNDS: r,w
// DESCR__ENABLE_OUT_OF_BOUNDS: 1: Eintrag im  Trace-Buffer beim Empfang eines iSRT-    Frames ausserhalb der iSRT-Zeitphase.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_REC_REPL_FRAME                                           0x00000004
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_REC_REPL_FRAME                                           2
// ACCESS__ENABLE_REC_REPL_FRAME: r,w
// DESCR__ENABLE_REC_REPL_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines    Ersatz-Telegramms im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_REC_TIME_FRAME                                           0x00000002
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_REC_TIME_FRAME                                           1
// ACCESS__ENABLE_REC_TIME_FRAME: r,w
// DESCR__ENABLE_REC_TIME_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines Zeittelegramms      im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1__MSK_ENABLE_REC_IRT_FRAME                                            0x00000001
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_SHFT_ENABLE_REC_IRT_FRAME                                            0
// ACCESS__ENABLE_REC_IRT_FRAME: r,w
// DESCR__ENABLE_REC_IRT_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines RT-Frames im     IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P1_RST__VAL                       0x00000000


#define IRTE_R7_REG_LINE_DELAY_P1                                                                           0x0000100C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LINE_DELAY_P1__MSK_LINE_DELAY1                                                              0x00000001
#define IRTE_R7_REG_LINE_DELAY_P1_SHFT_LINE_DELAY1                                                              0
// ACCESS__LINE_DELAY1: r,w
// DESCR__LINE_DELAY1: Leitungsverzögerung zum Nachbar-Switch (bzw. dem angeschlossenen Port des Nachbar-Switches) in 1ns-Genauigkeit
#define IRTE_R7_REG_LINE_DELAY_P1_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_CONTROL_P1                                                                          0x00001010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_ORG_PAUSE_LEARNING_DISABLE                                              0x00008000
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_ORG_PAUSE_LEARNING_DISABLE                                              15
// ACCESS__ORG_PAUSE_LEARNING_DISABLE: r,w
// DESCR__ORG_PAUSE_LEARNING_DISABLE: 0: ORG- und Pause-Telegramme werden entsprechend Bit 4   (Learning-Enable)  bzw. Bit 5 (MC-Learning-Enable) behandelt1: ORG- und Pause-Telegramme werden nicht gelernt
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_ADD_LINEDELAY                                                           0x00004000
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_ADD_LINEDELAY                                                           14
// ACCESS__ADD_LINEDELAY: r,w
// DESCR__ADD_LINEDELAY: 0: Addition des Line-Delays beim Sender1: Addition des Line-Delays beim Empfänger
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_MONITOR_ACYCLIC                                                         0x00002000
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_MONITOR_ACYCLIC                                                         13
// ACCESS__MONITOR_ACYCLIC: r,w
// DESCR__MONITOR_ACYCLIC: 0: Der azyklische Verkehr auf diesem Port wird nicht beobachtet1: Der gesamte azyklische Verkehr auf diesem Port wird beobachtet, d.h. alle Telegramme ausser vom Typ Ether-Type mit Frame-ID 0080-FBFF.
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_SHORT_ENABLE                                                            0x00001000
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_SHORT_ENABLE                                                            12
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 0: Telegramme < 64 Byte werden beim Empfang gefiltert.1: keine Prüfung auf Mindest-Telegrammlänge beim Empfang
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_MONITOR_CYCLIC                                                          0x00000800
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_MONITOR_CYCLIC                                                          11
// ACCESS__MONITOR_CYCLIC: r,w
// DESCR__MONITOR_CYCLIC: 0: Der zyklische Verkehr auf diesem Port wird nicht beobachtet1: Der zyklische Verkehr auf diesem Port wird beobachtet
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_REDUNDANCE_ENABLE                                                       0x00000400
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_REDUNDANCE_ENABLE                                                       10
// ACCESS__REDUNDANCE_ENABLE: r,w
// DESCR__REDUNDANCE_ENABLE: 0: Es wird kein redundanter Verkehr abgewickelt1: Es wird redundanter Verkehr abgewickelt
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_ISRT_ENABLE                                                             0x00000200
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_ISRT_ENABLE                                                             9
// ACCESS__ISRT_ENABLE: r,w
// DESCR__ISRT_ENABLE: 0: Es ist kein Zeitbereich für iSRT reserviert1: Es ist ein Zeitbereich für iSRT reserviert
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_EN_CHECK_SA                                                             0x00000100
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_EN_CHECK_SA                                                             8
// ACCESS__EN_CHECK_SA: r,w
// DESCR__EN_CHECK_SA: 0: Telegramme, deren Sourceadresse in der Adresstabelle     eingetragen sind, werden weitergeleitet.1: Telegramme, deren Sourceadresse nicht in der Adresstabelle     eingetragen sind werden gefiltert.
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_INGRESS_FILTERING                                                       0x00000080
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_INGRESS_FILTERING                                                       7
// ACCESS__INGRESS_FILTERING: r,w
// DESCR__INGRESS_FILTERING: 0: Im Tag Mode werden alle tagged Telegramme an diesem Port     empfangen. 1: Im Tag Mode wird ein Telegramm nur dann empfangen,     wenn die VLAN-Adresse des Telegramms mit dem VLAN-    Netzwerk am Empfangsport übereinstimmt.
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_ENABLE_PADDING                                                          0x00000040
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_ENABLE_PADDING                                                          6
// ACCESS__ENABLE_PADDING: r,w
// DESCR__ENABLE_PADDING: 0: Port macht kein  Padding für Telegramme < 64 Bytes1: Port macht  Padding für Telegramme < 64 Bytes
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_MC_LEARNING_ENABLE                                                      0x00000020
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_MC_LEARNING_ENABLE                                                      5
// ACCESS__MC_LEARNING_ENABLE: r,w
// DESCR__MC_LEARNING_ENABLE: 0: Nur die Sourceadressen von Unicast-Telegrammen werden     zum Lernen der Unicast-Adresstabelle genutzt1: Die Sourceadressen aller Telegramme werden zum Lernen von      Einträgen der Unicast-Adresstabelle genutzt.
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_LEARNING_ENABLE                                                         0x00000010
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_LEARNING_ENABLE                                                         4
// ACCESS__LEARNING_ENABLE: r,w
// DESCR__LEARNING_ENABLE: 0: Es werden keine neuen Einträge gelernt1: Dynamische Einträge der Unicast-Adress-Tabelle können aus    den Sourceadressen von Telegrammen gelernt  werden
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_ENABLE_SRT_PRIORITAET                                                   0x00000008
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_ENABLE_SRT_PRIORITAET                                                   3
// ACCESS__ENABLE_SRT_PRIORITAET: r,w
// DESCR__ENABLE_SRT_PRIORITAET: 0: Telegramme mit einem Typ-Feld Ether-Type werden nicht besonders behandelt1: Telegramme mit den Typ-Feld Ether-Type erhalten beim Weiterleiten SRT-Priorität. Aufteilung auf zyklisch/azyklisch erfolgt durch die Frame-ID des Telegramms.
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_EN_CUT_THROUGH                                                          0x00000004
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_EN_CUT_THROUGH                                                          2
// ACCESS__EN_CUT_THROUGH: r,w
// DESCR__EN_CUT_THROUGH: 0: Die Telegramme an Port Px müssen im K-RAM zwischen-gespeichert werden (store-and-foreward) 1: Die Telegramme an Port Px müssen, wenn möglich, direkt weitergeleitet werden (cut-through)
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_FULL_DUPLEX                                                             0x00000002
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_FULL_DUPLEX                                                             1
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: 0: Halbduplex1: Vollduplex
#define IRTE_R7_REG_NRT_CONTROL_P1__MSK_BAUDRATE                                                                0x00000001
#define IRTE_R7_REG_NRT_CONTROL_P1_SHFT_BAUDRATE                                                                0
// ACCESS__BAUDRATE: r,w
// DESCR__BAUDRATE: 0: Der Port wird im 100MBit-Modus betrieben1: Der Port wird im 10MBit-Modus betrieben
#define IRTE_R7_REG_NRT_CONTROL_P1_RST__VAL                               0x00000000


#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2                                                                  0x00002008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_WRONG_SOURCE_UDP                                                0x00200000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_WRONG_SOURCE_UDP                                                21
// ACCESS__WRONG_SOURCE_UDP: r,w
// DESCR__WRONG_SOURCE_UDP: 1: Der empfangene Source-UDP-Port eines RToverUDP-Frames \n    stimmt nicht mit dem erwarteten Source-UDP-Port überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_WRONG_SOURCE_IP                                                 0x00100000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_WRONG_SOURCE_IP                                                 20
// ACCESS__WRONG_SOURCE_IP: r,w
// DESCR__WRONG_SOURCE_IP: 1: Die empfangene Source-IP eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten Source-IP-Adresse überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_IP_CHECKSUM_ERROR                                               0x00080000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_IP_CHECKSUM_ERROR                                               19
// ACCESS__IP_CHECKSUM_ERROR: r,w
// DESCR__IP_CHECKSUM_ERROR: 1: Die empfangene IP-Header-Checksum eines RToverUDP-\n    Frames stimmt nicht mit der erwarteten IP-Checksum überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_WRONG_UDP_LENGTH                                                0x00040000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_WRONG_UDP_LENGTH                                                18
// ACCESS__WRONG_UDP_LENGTH: r,w
// DESCR__WRONG_UDP_LENGTH: 1: Die empfangene UDP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_WRONG_IP_LENGTH                                                 0x00020000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_WRONG_IP_LENGTH                                                 17
// ACCESS__WRONG_IP_LENGTH: r,w
// DESCR__WRONG_IP_LENGTH: 1: Die empfangene IP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_WRONG_SENDTIME                                           0x00010000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_WRONG_SENDTIME                                           16
// ACCESS__ENABLE_WRONG_SENDTIME: r,w
// DESCR__ENABLE_WRONG_SENDTIME: 1: Eintrag im  Trace-Buffer wenn der spezifizierte Zeitpunkt eines    RT-Telegramms ablaufen ist.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_NO_SENDDATA                                              0x00008000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_NO_SENDDATA                                              15
// ACCESS__ENABLE_NO_SENDDATA: r,w
// DESCR__ENABLE_NO_SENDDATA: 1: Zum spezifizierten Zeitpunkt ist kein RT-Telegramm zum     Versenden vorhanden sind.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_NO_SRT_ENTRY                                             0x00004000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_NO_SRT_ENTRY                                             14
// ACCESS__ENABLE_NO_SRT_ENTRY: r,w
// DESCR__ENABLE_NO_SRT_ENTRY: 1: In der NRT-Phase konnte beim Empfang eines SRT-    Telegramms kein Eintrag in der ACW-Liste gefunden werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_BUF_OVERLOAD                                             0x00002000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_BUF_OVERLOAD                                             13
// ACCESS__ENABLE_BUF_OVERLOAD: r,w
// DESCR__ENABLE_BUF_OVERLOAD: 1: Beim Telegrammempfang sind alle Datenpuffer übergelaufen
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_NO_TABLE_ENTRY                                                  0x00001000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_NO_TABLE_ENTRY                                                  12
// ACCESS__NO_TABLE_ENTRY: r,w
// DESCR__NO_TABLE_ENTRY: 1: Sourceadresse kann nicht in die MAC-Adresstabelle eingetragen    werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_NO_ACW                                                   0x00000800
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_NO_ACW                                                   11
// ACCESS__ENABLE_NO_ACW: r,w
// DESCR__ENABLE_NO_ACW: 1: Empfang eines Zeit-Telegramms im IRT-Zeitbereich und dabei     kein ACW mit der richtigen FrameID und opt. MAC-Adresse     gefunden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_WRONG_LENGTH                                             0x00000400
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_WRONG_LENGTH                                             10
// ACCESS__ENABLE_WRONG_LENGTH: r,w
// DESCR__ENABLE_WRONG_LENGTH: 1: Empfang eines RT-Telegramms mit einer falschen Länge.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_RCVBUFOVFL                                               0x00000200
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_RCVBUFOVFL                                               9
// ACCESS__ENABLE_RCVBUFOVFL: r,w
// DESCR__ENABLE_RCVBUFOVFL: 1: Puffer-Überlauf beim Empfang eines RT-Telegramms.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_CRC_ERROR                                                0x00000100
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_CRC_ERROR                                                8
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Empfang eines Telegramms mit CRC-Fehler im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_WRONG_FRAMEID                                            0x00000080
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_WRONG_FRAMEID                                            7
// ACCESS__ENABLE_WRONG_FRAMEID: r,w
// DESCR__ENABLE_WRONG_FRAMEID: 1: Empfang eines RT-Telegramms mit der falschen FrameID    und opt. MAC-Adresse im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_IRT_TEL_ERROR                                            0x00000040
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_IRT_TEL_ERROR                                            6
// ACCESS__ENABLE_IRT_TEL_ERROR: r,w
// DESCR__ENABLE_IRT_TEL_ERROR: 1: Eintrag im  Trace-Buffer, wenn ein RT-Telegramm kleiner    als 20 Byte empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_NO_IRT_TEL                                               0x00000020
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_NO_IRT_TEL                                               5
// ACCESS__ENABLE_NO_IRT_TEL: r,w
// DESCR__ENABLE_NO_IRT_TEL: 1: Eintrag im  Trace-Buffer, wenn bis zum erwarteten Zeitpunkt kein RT-Telegramm empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_REC_NRT                                                  0x00000010
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_REC_NRT                                                  4
// ACCESS__ENABLE_REC_NRT: r,w
// DESCR__ENABLE_REC_NRT: 1: Eintrag im  Trace-Buffer beim Empfang eines NRT-Telegramms \n    (kein Ether-Type) im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_OUT_OF_BOUNDS                                            0x00000008
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_OUT_OF_BOUNDS                                            3
// ACCESS__ENABLE_OUT_OF_BOUNDS: r,w
// DESCR__ENABLE_OUT_OF_BOUNDS: 1: Eintrag im  Trace-Buffer beim Empfang eines iSRT-    Frames ausserhalb der iSRT-Zeitphase.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_REC_REPL_FRAME                                           0x00000004
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_REC_REPL_FRAME                                           2
// ACCESS__ENABLE_REC_REPL_FRAME: r,w
// DESCR__ENABLE_REC_REPL_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines    Ersatz-Telegramms im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_REC_TIME_FRAME                                           0x00000002
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_REC_TIME_FRAME                                           1
// ACCESS__ENABLE_REC_TIME_FRAME: r,w
// DESCR__ENABLE_REC_TIME_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines Zeittelegramms      im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2__MSK_ENABLE_REC_IRT_FRAME                                            0x00000001
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_SHFT_ENABLE_REC_IRT_FRAME                                            0
// ACCESS__ENABLE_REC_IRT_FRAME: r,w
// DESCR__ENABLE_REC_IRT_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines RT-Frames im     IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P2_RST__VAL                       0x00000000


#define IRTE_R7_REG_LINE_DELAY_P2                                                                           0x0000200C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LINE_DELAY_P2__MSK_LINE_DELAY2                                                              0x00000001
#define IRTE_R7_REG_LINE_DELAY_P2_SHFT_LINE_DELAY2                                                              0
// ACCESS__LINE_DELAY2: r,w
// DESCR__LINE_DELAY2: Leitungsverzögerung zum Nachbar-Switch (bzw. dem angeschlossenen Port des Nachbar-Switches) in 1ns-Genauigkeit
#define IRTE_R7_REG_LINE_DELAY_P2_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_CONTROL_P2                                                                          0x00002010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_ORG_PAUSE_LEARNING_DISABLE                                              0x00008000
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_ORG_PAUSE_LEARNING_DISABLE                                              15
// ACCESS__ORG_PAUSE_LEARNING_DISABLE: r,w
// DESCR__ORG_PAUSE_LEARNING_DISABLE: 0: ORG- und Pause-Telegramme werden entsprechend Bit 4   (Learning-Enable)  bzw. Bit 5 (MC-Learning-Enable) behandelt1: ORG- und Pause-Telegramme werden nicht gelernt
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_ADD_LINEDELAY                                                           0x00004000
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_ADD_LINEDELAY                                                           14
// ACCESS__ADD_LINEDELAY: r,w
// DESCR__ADD_LINEDELAY: 0: Addition des Line-Delays beim Sender1: Addition des Line-Delays beim Empfänger
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_MONITOR_ACYCLIC                                                         0x00002000
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_MONITOR_ACYCLIC                                                         13
// ACCESS__MONITOR_ACYCLIC: r,w
// DESCR__MONITOR_ACYCLIC: 0: Der azyklische Verkehr auf diesem Port wird nicht beobachtet1: Der gesamte azyklische Verkehr auf diesem Port wird beobachtet, d.h. alle Telegramme ausser vom Typ Ether-Type mit Frame-ID 0080-FBFF.
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_SHORT_ENABLE                                                            0x00001000
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_SHORT_ENABLE                                                            12
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 0: Telegramme < 64 Byte werden beim Empfang gefiltert.1: keine Prüfung auf Mindest-Telegrammlänge beim Empfang
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_MONITOR_CYCLIC                                                          0x00000800
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_MONITOR_CYCLIC                                                          11
// ACCESS__MONITOR_CYCLIC: r,w
// DESCR__MONITOR_CYCLIC: 0: Der zyklische Verkehr auf diesem Port wird nicht beobachtet1: Der zyklische Verkehr auf diesem Port wird beobachtet
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_REDUNDANCE_ENABLE                                                       0x00000400
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_REDUNDANCE_ENABLE                                                       10
// ACCESS__REDUNDANCE_ENABLE: r,w
// DESCR__REDUNDANCE_ENABLE: 0: Es wird kein redundanter Verkehr abgewickelt1: Es wird redundanter Verkehr abgewickelt
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_ISRT_ENABLE                                                             0x00000200
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_ISRT_ENABLE                                                             9
// ACCESS__ISRT_ENABLE: r,w
// DESCR__ISRT_ENABLE: 0: Es ist kein Zeitbereich für iSRT reserviert1: Es ist ein Zeitbereich für iSRT reserviert
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_EN_CHECK_SA                                                             0x00000100
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_EN_CHECK_SA                                                             8
// ACCESS__EN_CHECK_SA: r,w
// DESCR__EN_CHECK_SA: 0: Telegramme, deren Sourceadresse in der Adresstabelle     eingetragen sind, werden weitergeleitet.1: Telegramme, deren Sourceadresse nicht in der Adresstabelle     eingetragen sind werden gefiltert.
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_INGRESS_FILTERING                                                       0x00000080
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_INGRESS_FILTERING                                                       7
// ACCESS__INGRESS_FILTERING: r,w
// DESCR__INGRESS_FILTERING: 0: Im Tag Mode werden alle tagged Telegramme an diesem Port     empfangen. 1: Im Tag Mode wird ein Telegramm nur dann empfangen,     wenn die VLAN-Adresse des Telegramms mit dem VLAN-    Netzwerk am Empfangsport übereinstimmt.
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_ENABLE_PADDING                                                          0x00000040
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_ENABLE_PADDING                                                          6
// ACCESS__ENABLE_PADDING: r,w
// DESCR__ENABLE_PADDING: 0: Port macht kein  Padding für Telegramme < 64 Bytes1: Port macht  Padding für Telegramme < 64 Bytes
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_MC_LEARNING_ENABLE                                                      0x00000020
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_MC_LEARNING_ENABLE                                                      5
// ACCESS__MC_LEARNING_ENABLE: r,w
// DESCR__MC_LEARNING_ENABLE: 0: Nur die Sourceadressen von Unicast-Telegrammen werden     zum Lernen der Unicast-Adresstabelle genutzt1: Die Sourceadressen aller Telegramme werden zum Lernen von      Einträgen der Unicast-Adresstabelle genutzt.
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_LEARNING_ENABLE                                                         0x00000010
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_LEARNING_ENABLE                                                         4
// ACCESS__LEARNING_ENABLE: r,w
// DESCR__LEARNING_ENABLE: 0: Es werden keine neuen Einträge gelernt1: Dynamische Einträge der Unicast-Adress-Tabelle können aus    den Sourceadressen von Telegrammen gelernt  werden
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_ENABLE_SRT_PRIORITAET                                                   0x00000008
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_ENABLE_SRT_PRIORITAET                                                   3
// ACCESS__ENABLE_SRT_PRIORITAET: r,w
// DESCR__ENABLE_SRT_PRIORITAET: 0: Telegramme mit einem Typ-Feld Ether-Type werden nicht besonders behandelt1: Telegramme mit den Typ-Feld Ether-Type erhalten beim Weiterleiten SRT-Priorität. Aufteilung auf zyklisch/azyklisch erfolgt durch die Frame-ID des Telegramms.
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_EN_CUT_THROUGH                                                          0x00000004
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_EN_CUT_THROUGH                                                          2
// ACCESS__EN_CUT_THROUGH: r,w
// DESCR__EN_CUT_THROUGH: 0: Die Telegramme an Port Px müssen im K-RAM zwischen-gespeichert werden (store-and-foreward) 1: Die Telegramme an Port Px müssen, wenn möglich, direkt weitergeleitet werden (cut-through)
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_FULL_DUPLEX                                                             0x00000002
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_FULL_DUPLEX                                                             1
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: 0: Halbduplex1: Vollduplex
#define IRTE_R7_REG_NRT_CONTROL_P2__MSK_BAUDRATE                                                                0x00000001
#define IRTE_R7_REG_NRT_CONTROL_P2_SHFT_BAUDRATE                                                                0
// ACCESS__BAUDRATE: r,w
// DESCR__BAUDRATE: 0: Der Port wird im 100MBit-Modus betrieben1: Der Port wird im 10MBit-Modus betrieben
#define IRTE_R7_REG_NRT_CONTROL_P2_RST__VAL                               0x00000000


#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3                                                                  0x00003008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_WRONG_SOURCE_UDP                                                0x00200000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_WRONG_SOURCE_UDP                                                21
// ACCESS__WRONG_SOURCE_UDP: r,w
// DESCR__WRONG_SOURCE_UDP: 1: Der empfangene Source-UDP-Port eines RToverUDP-Frames \n    stimmt nicht mit dem erwarteten Source-UDP-Port überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_WRONG_SOURCE_IP                                                 0x00100000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_WRONG_SOURCE_IP                                                 20
// ACCESS__WRONG_SOURCE_IP: r,w
// DESCR__WRONG_SOURCE_IP: 1: Die empfangene Source-IP eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten Source-IP-Adresse überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_IP_CHECKSUM_ERROR                                               0x00080000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_IP_CHECKSUM_ERROR                                               19
// ACCESS__IP_CHECKSUM_ERROR: r,w
// DESCR__IP_CHECKSUM_ERROR: 1: Die empfangene IP-Header-Checksum eines RToverUDP-\n    Frames stimmt nicht mit der erwarteten IP-Checksum überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_WRONG_UDP_LENGTH                                                0x00040000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_WRONG_UDP_LENGTH                                                18
// ACCESS__WRONG_UDP_LENGTH: r,w
// DESCR__WRONG_UDP_LENGTH: 1: Die empfangene UDP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_WRONG_IP_LENGTH                                                 0x00020000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_WRONG_IP_LENGTH                                                 17
// ACCESS__WRONG_IP_LENGTH: r,w
// DESCR__WRONG_IP_LENGTH: 1: Die empfangene IP-Länge eines RToverUDP-Frames stimmt \n    nicht mit der erwarteten IP-Länge überein.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_WRONG_SENDTIME                                           0x00010000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_WRONG_SENDTIME                                           16
// ACCESS__ENABLE_WRONG_SENDTIME: r,w
// DESCR__ENABLE_WRONG_SENDTIME: 1: Eintrag im  Trace-Buffer wenn der spezifizierte Zeitpunkt eines    RT-Telegramms ablaufen ist.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_NO_SENDDATA                                              0x00008000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_NO_SENDDATA                                              15
// ACCESS__ENABLE_NO_SENDDATA: r,w
// DESCR__ENABLE_NO_SENDDATA: 1: Zum spezifizierten Zeitpunkt ist kein RT-Telegramm zum     Versenden vorhanden sind.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_NO_SRT_ENTRY                                             0x00004000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_NO_SRT_ENTRY                                             14
// ACCESS__ENABLE_NO_SRT_ENTRY: r,w
// DESCR__ENABLE_NO_SRT_ENTRY: 1: In der NRT-Phase konnte beim Empfang eines SRT-    Telegramms kein Eintrag in der ACW-Liste gefunden werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_BUF_OVERLOAD                                             0x00002000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_BUF_OVERLOAD                                             13
// ACCESS__ENABLE_BUF_OVERLOAD: r,w
// DESCR__ENABLE_BUF_OVERLOAD: 1: Beim Telegrammempfang sind alle Datenpuffer übergelaufen
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_NO_TABLE_ENTRY                                                  0x00001000
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_NO_TABLE_ENTRY                                                  12
// ACCESS__NO_TABLE_ENTRY: r,w
// DESCR__NO_TABLE_ENTRY: 1: Sourceadresse kann nicht in die MAC-Adresstabelle eingetragen    werden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_NO_ACW                                                   0x00000800
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_NO_ACW                                                   11
// ACCESS__ENABLE_NO_ACW: r,w
// DESCR__ENABLE_NO_ACW: 1: Empfang eines Zeit-Telegramms im IRT-Zeitbereich und dabei     kein ACW mit der richtigen FrameID und opt. MAC-Adresse     gefunden
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_WRONG_LENGTH                                             0x00000400
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_WRONG_LENGTH                                             10
// ACCESS__ENABLE_WRONG_LENGTH: r,w
// DESCR__ENABLE_WRONG_LENGTH: 1: Empfang eines RT-Telegramms mit einer falschen Länge.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_RCVBUFOVFL                                               0x00000200
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_RCVBUFOVFL                                               9
// ACCESS__ENABLE_RCVBUFOVFL: r,w
// DESCR__ENABLE_RCVBUFOVFL: 1: Puffer-Überlauf beim Empfang eines RT-Telegramms.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_CRC_ERROR                                                0x00000100
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_CRC_ERROR                                                8
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Empfang eines Telegramms mit CRC-Fehler im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_WRONG_FRAMEID                                            0x00000080
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_WRONG_FRAMEID                                            7
// ACCESS__ENABLE_WRONG_FRAMEID: r,w
// DESCR__ENABLE_WRONG_FRAMEID: 1: Empfang eines RT-Telegramms mit der falschen FrameID    und opt. MAC-Adresse im IRT-Zeitbereich
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_IRT_TEL_ERROR                                            0x00000040
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_IRT_TEL_ERROR                                            6
// ACCESS__ENABLE_IRT_TEL_ERROR: r,w
// DESCR__ENABLE_IRT_TEL_ERROR: 1: Eintrag im  Trace-Buffer, wenn ein RT-Telegramm kleiner    als 20 Byte empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_NO_IRT_TEL                                               0x00000020
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_NO_IRT_TEL                                               5
// ACCESS__ENABLE_NO_IRT_TEL: r,w
// DESCR__ENABLE_NO_IRT_TEL: 1: Eintrag im  Trace-Buffer, wenn bis zum erwarteten Zeitpunkt kein RT-Telegramm empfangen wird.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_REC_NRT                                                  0x00000010
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_REC_NRT                                                  4
// ACCESS__ENABLE_REC_NRT: r,w
// DESCR__ENABLE_REC_NRT: 1: Eintrag im  Trace-Buffer beim Empfang eines NRT-Telegramms \n    (kein Ether-Type) im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_OUT_OF_BOUNDS                                            0x00000008
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_OUT_OF_BOUNDS                                            3
// ACCESS__ENABLE_OUT_OF_BOUNDS: r,w
// DESCR__ENABLE_OUT_OF_BOUNDS: 1: Eintrag im  Trace-Buffer beim Empfang eines iSRT-    Frames ausserhalb der iSRT-Zeitphase.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_REC_REPL_FRAME                                           0x00000004
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_REC_REPL_FRAME                                           2
// ACCESS__ENABLE_REC_REPL_FRAME: r,w
// DESCR__ENABLE_REC_REPL_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines    Ersatz-Telegramms im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_REC_TIME_FRAME                                           0x00000002
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_REC_TIME_FRAME                                           1
// ACCESS__ENABLE_REC_TIME_FRAME: r,w
// DESCR__ENABLE_REC_TIME_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines Zeittelegramms      im IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3__MSK_ENABLE_REC_IRT_FRAME                                            0x00000001
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_SHFT_ENABLE_REC_IRT_FRAME                                            0
// ACCESS__ENABLE_REC_IRT_FRAME: r,w
// DESCR__ENABLE_REC_IRT_FRAME: 1: Eintrag im  Trace-Buffer beim Empfang eines RT-Frames im     IRT-Zeitbereich.
#define IRTE_R7_REG_IRT_DIAGNOSE_ENABLE_P3_RST__VAL                       0x00000000


#define IRTE_R7_REG_LINE_DELAY_P3                                                                           0x0000300C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LINE_DELAY_P3__MSK_LINE_DELAY2                                                              0x00000001
#define IRTE_R7_REG_LINE_DELAY_P3_SHFT_LINE_DELAY2                                                              0
// ACCESS__LINE_DELAY2: r,w
// DESCR__LINE_DELAY2: Leitungsverzögerung zum Nachbar-Switch (bzw. dem angeschlossenen Port des Nachbar-Switches) in 1ns-Genauigkeit
#define IRTE_R7_REG_LINE_DELAY_P3_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_CONTROL_P3                                                                          0x00003010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_ORG_PAUSE_LEARNING_DISABLE                                              0x00008000
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_ORG_PAUSE_LEARNING_DISABLE                                              15
// ACCESS__ORG_PAUSE_LEARNING_DISABLE: r,w
// DESCR__ORG_PAUSE_LEARNING_DISABLE: 0: ORG- und Pause-Telegramme werden entsprechend Bit 4   (Learning-Enable)  bzw. Bit 5 (MC-Learning-Enable) behandelt1: ORG- und Pause-Telegramme werden nicht gelernt
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_ADD_LINEDELAY                                                           0x00004000
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_ADD_LINEDELAY                                                           14
// ACCESS__ADD_LINEDELAY: r,w
// DESCR__ADD_LINEDELAY: 0: Addition des Line-Delays beim Sender1: Addition des Line-Delays beim Empfänger
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_MONITOR_ACYCLIC                                                         0x00002000
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_MONITOR_ACYCLIC                                                         13
// ACCESS__MONITOR_ACYCLIC: r,w
// DESCR__MONITOR_ACYCLIC: 0: Der azyklische Verkehr auf diesem Port wird nicht beobachtet1: Der gesamte azyklische Verkehr auf diesem Port wird beobachtet, d.h. alle Telegramme ausser vom Typ Ether-Type mit Frame-ID 0080-FBFF.
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_SHORT_ENABLE                                                            0x00001000
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_SHORT_ENABLE                                                            12
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 0: Telegramme < 64 Byte werden beim Empfang gefiltert.1: keine Prüfung auf Mindest-Telegrammlänge beim Empfang
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_MONITOR_CYCLIC                                                          0x00000800
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_MONITOR_CYCLIC                                                          11
// ACCESS__MONITOR_CYCLIC: r,w
// DESCR__MONITOR_CYCLIC: 0: Der zyklische Verkehr auf diesem Port wird nicht beobachtet1: Der zyklische Verkehr auf diesem Port wird beobachtet
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_REDUNDANCE_ENABLE                                                       0x00000400
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_REDUNDANCE_ENABLE                                                       10
// ACCESS__REDUNDANCE_ENABLE: r,w
// DESCR__REDUNDANCE_ENABLE: 0: Es wird kein redundanter Verkehr abgewickelt1: Es wird redundanter Verkehr abgewickelt
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_ISRT_ENABLE                                                             0x00000200
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_ISRT_ENABLE                                                             9
// ACCESS__ISRT_ENABLE: r,w
// DESCR__ISRT_ENABLE: 0: Es ist kein Zeitbereich für iSRT reserviert1: Es ist ein Zeitbereich für iSRT reserviert
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_EN_CHECK_SA                                                             0x00000100
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_EN_CHECK_SA                                                             8
// ACCESS__EN_CHECK_SA: r,w
// DESCR__EN_CHECK_SA: 0: Telegramme, deren Sourceadresse in der Adresstabelle     eingetragen sind, werden weitergeleitet.1: Telegramme, deren Sourceadresse nicht in der Adresstabelle     eingetragen sind werden gefiltert.
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_INGRESS_FILTERING                                                       0x00000080
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_INGRESS_FILTERING                                                       7
// ACCESS__INGRESS_FILTERING: r,w
// DESCR__INGRESS_FILTERING: 0: Im Tag Mode werden alle tagged Telegramme an diesem Port     empfangen. 1: Im Tag Mode wird ein Telegramm nur dann empfangen,     wenn die VLAN-Adresse des Telegramms mit dem VLAN-    Netzwerk am Empfangsport übereinstimmt.
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_ENABLE_PADDING                                                          0x00000040
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_ENABLE_PADDING                                                          6
// ACCESS__ENABLE_PADDING: r,w
// DESCR__ENABLE_PADDING: 0: Port macht kein  Padding für Telegramme < 64 Bytes1: Port macht  Padding für Telegramme < 64 Bytes
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_MC_LEARNING_ENABLE                                                      0x00000020
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_MC_LEARNING_ENABLE                                                      5
// ACCESS__MC_LEARNING_ENABLE: r,w
// DESCR__MC_LEARNING_ENABLE: 0: Nur die Sourceadressen von Unicast-Telegrammen werden     zum Lernen der Unicast-Adresstabelle genutzt1: Die Sourceadressen aller Telegramme werden zum Lernen von      Einträgen der Unicast-Adresstabelle genutzt.
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_LEARNING_ENABLE                                                         0x00000010
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_LEARNING_ENABLE                                                         4
// ACCESS__LEARNING_ENABLE: r,w
// DESCR__LEARNING_ENABLE: 0: Es werden keine neuen Einträge gelernt1: Dynamische Einträge der Unicast-Adress-Tabelle können aus    den Sourceadressen von Telegrammen gelernt  werden
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_ENABLE_SRT_PRIORITAET                                                   0x00000008
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_ENABLE_SRT_PRIORITAET                                                   3
// ACCESS__ENABLE_SRT_PRIORITAET: r,w
// DESCR__ENABLE_SRT_PRIORITAET: 0: Telegramme mit einem Typ-Feld Ether-Type werden nicht besonders behandelt1: Telegramme mit den Typ-Feld Ether-Type erhalten beim Weiterleiten SRT-Priorität. Aufteilung auf zyklisch/azyklisch erfolgt durch die Frame-ID des Telegramms.
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_EN_CUT_THROUGH                                                          0x00000004
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_EN_CUT_THROUGH                                                          2
// ACCESS__EN_CUT_THROUGH: r,w
// DESCR__EN_CUT_THROUGH: 0: Die Telegramme an Port Px müssen im K-RAM zwischen-gespeichert werden (store-and-foreward) 1: Die Telegramme an Port Px müssen, wenn möglich, direkt weitergeleitet werden (cut-through)
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_FULL_DUPLEX                                                             0x00000002
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_FULL_DUPLEX                                                             1
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: 0: Halbduplex1: Vollduplex
#define IRTE_R7_REG_NRT_CONTROL_P3__MSK_BAUDRATE                                                                0x00000001
#define IRTE_R7_REG_NRT_CONTROL_P3_SHFT_BAUDRATE                                                                0
// ACCESS__BAUDRATE: r,w
// DESCR__BAUDRATE: 0: Der Port wird im 100MBit-Modus betrieben1: Der Port wird im 10MBit-Modus betrieben
#define IRTE_R7_REG_NRT_CONTROL_P3_RST__VAL                               0x00000000


#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER                                                             0x00011034
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER__MSK_I                                                          0x80000000
#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER_SHFT_I                                                          31
// ACCESS__I: r,w
// DESCR__I: Inkrement-Bit;wenn 1, wird im Korrekturzyklus bei jeder Korrektur der Cycle-Timer um 2 inkrementiert;wenn 0, wird im Korrekturzyklus bei jeder Korrektur der Cycle-Timer nicht inkrementiert;
#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER__MSK_CYCLETIMER_R                                               0x7FFFFFFF
#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER_SHFT_CYCLETIMER_R                                               0
// ACCESS__CYCLETIMER_R: r,w
// DESCR__CYCLETIMER_R: Anzahl der 100 MHz Takte, nach denen im Korrekturzyklus eine Korrektur durchgeführt wird; steht hier z.B. eine 4, so wird bei jedem 4. Takt eine Korrektur durchgeführt.Beispiel: RPhaseK  31..0  = 80000004War die Zykluslänge vor dem Aktivieren der Korrektur 30000 Takte, so beträgt sie jetzt 30000/5 * 4  = 24000 TakteBeispiel: RPhaseK  31..0  = 00000004War die Zykluslänge vor dem Aktivieren der Korrektur 30000 Takte, so beträgt sie jetzt 30000/3 * 4  = 40000 Takte
#define IRTE_R7_REG_PHASE_CORRECTION_CYCLETIMER_RST__VAL                  0x00000000


#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER                                                           0x00011038
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER__MSK_I                                                        0x80000000
#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER_SHFT_I                                                        31
// ACCESS__I: r,w
// DESCR__I: Inkrement-Bit;wenn 1, wird im Korrekturzyklus bei jeder Korrektur der Clock-Counter um +2ns inkrementiert;wenn 0, wird im Korrekturzyklus bei jeder Korrektur der Clock-Counter um -1ns inkrementiert;
#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER__MSK_CLOCKCOUNTER_R                                           0x7FFFFFFF
#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER_SHFT_CLOCKCOUNTER_R                                           0
// ACCESS__CLOCKCOUNTER_R: r,w
// DESCR__CLOCKCOUNTER_R: Anzahl der 100 MHz Takte, nach denen im Korrekturzyklus eine Korrektur durchgeführt wird; steht hier z.B. eine 4, so wird bei jedem 4. Takt eine Korrektur durchgeführt.
#define IRTE_R7_REG_PHASE_CORRECTION_CLOCKCOUNTER_RST__VAL                0x00000000


#define IRTE_R7_REG_CYCLE_CNT_ENTITY                                                                        0x0001103C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CYCLE_CNT_ENTITY__MSK_CYCLE_CNT_ENTITY                                                      0x0000FFFF
#define IRTE_R7_REG_CYCLE_CNT_ENTITY_SHFT_CYCLE_CNT_ENTITY                                                      0
// ACCESS__CYCLE_CNT_ENTITY: r,w
// DESCR__CYCLE_CNT_ENTITY: Anzahl der in einem Isochronzykus enthaltenen Basiszyklen (1 Basiszyklus  = 31,25µs).
#define IRTE_R7_REG_CYCLE_CNT_ENTITY_RST__VAL                             0x00000000


#define IRTE_R7_REG_CYCLE_FRAMEID                                                                           0x00011040
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CYCLE_FRAMEID__MSK_CYCLE_FRAMEID                                                            0x0000FFFF
#define IRTE_R7_REG_CYCLE_FRAMEID_SHFT_CYCLE_FRAMEID                                                            0
// ACCESS__CYCLE_FRAMEID: r,w
// DESCR__CYCLE_FRAMEID: Frame-ID des aktiven Zyklussynchronisations-Telegramms (vom Zyklussynchronisations-Master)
#define IRTE_R7_REG_CYCLE_FRAMEID_RST__VAL                                0x00000000


#define IRTE_R7_REG_CYCLE_CNT_VAL_31_0                                                                      0x00011400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CYCLE_CNT_VAL_31_0__MSK_CYCLE_CNT_VAL                                                       0xFFFFFFFF
#define IRTE_R7_REG_CYCLE_CNT_VAL_31_0_SHFT_CYCLE_CNT_VAL                                                       0
// ACCESS__CYCLE_CNT_VAL: r,w
// DESCR__CYCLE_CNT_VAL: Anzahl der Basistakte (1 Basistakt  = 31,25µs)
#define IRTE_R7_REG_CYCLE_CNT_VAL_31_0_RST__VAL                           0x00000000


#define IRTE_R7_REG_CYCLE_CNT_VAL_47_32                                                                     0x00011404
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CYCLE_CNT_VAL_47_32__MSK_CYCLE_CNT_VAL                                                      0x0000FFFF
#define IRTE_R7_REG_CYCLE_CNT_VAL_47_32_SHFT_CYCLE_CNT_VAL                                                      0
// ACCESS__CYCLE_CNT_VAL: r,w
// DESCR__CYCLE_CNT_VAL: Anzahl der Basistakte (1 Basistakt  = 31,25µs)
#define IRTE_R7_REG_CYCLE_CNT_VAL_47_32_RST__VAL                          0x00000000


#define IRTE_R7_REG_CYCLE_TIME_VAL                                                                          0x00011408
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CYCLE_TIME_VAL__MSK_CYCLE_TIME_VAL                                                          0xFFFFFFFF
#define IRTE_R7_REG_CYCLE_TIME_VAL_SHFT_CYCLE_TIME_VAL                                                          0
// ACCESS__CYCLE_TIME_VAL: r,w
// DESCR__CYCLE_TIME_VAL: Wert innerhalb des geregelten Isochronzyklus in Anzahl von 100 MHz Takten
#define IRTE_R7_REG_CYCLE_TIME_VAL_RST__VAL                               0x00000000


#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_EXT                                                                 0x0001140C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_EXT__MSK_CLOCK_CNT_VAL_BEGIN_EXT                                        0xFFFFFFFF
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_EXT_SHFT_CLOCK_CNT_VAL_BEGIN_EXT                                        0
// ACCESS__CLOCK_CNT_VAL_BEGIN_EXT: r,w
// DESCR__CLOCK_CNT_VAL_BEGIN_EXT: abgespeicherter Wert des Clock-Counters in 1ns-Genauigkeit; das Speichern wird durch eine fallende Flanke am externen Pin PLL_Ext_In des IRTE-Switch-Makros ausgelöst.
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_EXT_RST__VAL                      0x00000000


#define IRTE_R7_REG_APLCLK_TIMER                                                                            0x00011410
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_APLCLK_TIMER__MSK_APLCLK_TIMER                                                              0xFFFFFFFF
#define IRTE_R7_REG_APLCLK_TIMER_SHFT_APLCLK_TIMER                                                              0
// ACCESS__APLCLK_TIMER: r,
// DESCR__APLCLK_TIMER: Zeitlicher Abstand zum Aplclk-Perioden-Beginn in 100 MHz-Schritten
#define IRTE_R7_REG_APLCLK_TIMER_RST__VAL                                 0x00000000


#define IRTE_R7_REG_CLKGEN_STATUS                                                                           0x00011414
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CLKGEN_STATUS__MSK_STCLKCY_START                                                            0x00000002
#define IRTE_R7_REG_CLKGEN_STATUS_SHFT_STCLKCY_START                                                            1
// ACCESS__STCLKCY_START: r,
// DESCR__STCLKCY_START: StClkcy_Start: (verzögerte) Antwort / Quittierung der Anforderung gemäß Bit Clkcy_StartStClkcy_Start  = 1: Takterzeugung gestartet / läuftStClkcy_Start  = 0: Takterzeugung gestoppt / steht; keine Signalerzeugung
#define IRTE_R7_REG_CLKGEN_STATUS__MSK_STAPLCLK_START                                                           0x00000001
#define IRTE_R7_REG_CLKGEN_STATUS_SHFT_STAPLCLK_START                                                           0
// ACCESS__STAPLCLK_START: r,
// DESCR__STAPLCLK_START: StAplClk_Start: (verzögerte) Antwort / Quittierung der Anforderung gemäß Bit AplClk _StartStAplClk_Start  = 1: Takterzeugung gestartet / läuftStAplClk_Start  = 0: Takterzeugung gestoppt / steht; keine Signalerzeugung
#define IRTE_R7_REG_CLKGEN_STATUS_RST__VAL                                0x00000000


#define IRTE_R7_REG_CLOCK_CNT_VAL                                                                           0x00011418
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CLOCK_CNT_VAL__MSK_CLOCK_CNT_VAL                                                            0xFFFFFFFF
#define IRTE_R7_REG_CLOCK_CNT_VAL_SHFT_CLOCK_CNT_VAL                                                            0
// ACCESS__CLOCK_CNT_VAL: r,w
// DESCR__CLOCK_CNT_VAL: Wert eines freilaufenden (ungeregelten) Zählers in Anzahl von 1 GHz Takten (1ns-Genauigkeit). Die relativen Werte dieses Zählers dienen als Basis zur Regelung des Isochronzyklus
#define IRTE_R7_REG_CLOCK_CNT_VAL_RST__VAL                                0x00000000


#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN                                                                     0x0001141C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN__MSK_CLOCK_CNT_VAL_BEGIN                                                0xFFFFFFFF
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_SHFT_CLOCK_CNT_VAL_BEGIN                                                0
// ACCESS__CLOCK_CNT_VAL_BEGIN: r,w
// DESCR__CLOCK_CNT_VAL_BEGIN: abgespeicherter Wert des Clock-Counters in 1ns-Genauigkeit; das Speichern wird durch den Beginn des Isochron-Zyklus ausgelöst.
#define IRTE_R7_REG_CLOCK_CNT_VAL_BEGIN_RST__VAL                          0x00000000


#define IRTE_R7_REG_ISRT_TIME                                                                               0x00011420
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ISRT_TIME__MSK_ISRT_TIME                                                                    0x000FFFFF
#define IRTE_R7_REG_ISRT_TIME_SHFT_ISRT_TIME                                                                    0
// ACCESS__ISRT_TIME: r,w
// DESCR__ISRT_TIME: write:Ladewert des iSRT-Timersread:Aktueller Wert des iSRT-Timers und damit die minimal zur Verfügung stehende reservierte iSRT-Phase (in 100MHz)
#define IRTE_R7_REG_ISRT_TIME_RST__VAL                                    0x00000000


#define IRTE_R7_REG_MAX_ISRT_LENGTH                                                                         0x00011424
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_MAX_ISRT_LENGTH__MSK_MAX_ISRT_LENGTH                                                        0xFFFFFFFF
#define IRTE_R7_REG_MAX_ISRT_LENGTH_SHFT_MAX_ISRT_LENGTH                                                        0
// ACCESS__MAX_ISRT_LENGTH: r,
// DESCR__MAX_ISRT_LENGTH: read:Maximaler Wert des Cycle-Timers bis zu dem die iSRT-Phase verlängert worden ist (maximale iSRT-Phase über alle Zyklen hinweg). Der Registerinhalt wird mit dem Lesezugriff gelöscht.
#define IRTE_R7_REG_MAX_ISRT_LENGTH_RST__VAL                              0x00000000


#define IRTE_R7_REG_CLOCK_CNT_VAL_SND                                                                       0x00011428
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CLOCK_CNT_VAL_SND__MSK_CLOCK_CNT_VAL_SND                                                    0xFFFFFFFF
#define IRTE_R7_REG_CLOCK_CNT_VAL_SND_SHFT_CLOCK_CNT_VAL_SND                                                    0
// ACCESS__CLOCK_CNT_VAL_SND: r,
// DESCR__CLOCK_CNT_VAL_SND: read:Abgespeicherter Wert von Clock_Cnt_Val (in 1ns-Genauigkeit) beim Versenden eines im DMACW speziell markierten NRT-Telegramms (PT-Bit im DMACW gesetzt). Der Registerinhalt bleibt nach dem Auslesen erhalten.
#define IRTE_R7_REG_CLOCK_CNT_VAL_SND_RST__VAL                            0x00000000


#define IRTE_R7_REG_ISRT_OUTOFBOUND                                                                         0x0001142C
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_ISRT_OUTOFBOUND__MSK_ISRT_OUTOFBOUND                                                        0x000000FF
#define IRTE_R7_REG_ISRT_OUTOFBOUND_SHFT_ISRT_OUTOFBOUND                                                        0
// ACCESS__ISRT_OUTOFBOUND: r,
// DESCR__ISRT_OUTOFBOUND: read:Anzahl der iSRT-Telegramm, die ausserhalb der iSRT-Phase gesendet werden. Der Registerinhalt wird mit dem Lesezugriff gelöscht.
#define IRTE_R7_REG_ISRT_OUTOFBOUND_RST__VAL                              0x00000000


#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB                                                                     0x00012008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB__MSK_MC_LEARNING_DISABLE                                                0x00000400
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB_SHFT_MC_LEARNING_DISABLE                                                10
// ACCESS__MC_LEARNING_DISABLE: r,w
// DESCR__MC_LEARNING_DISABLE: nur im Back-to-Back-Mode (Bit 8  = ‚1’) für Kanal B relevant:0: Die Sourceadressen aller Telegramme werden zum Lernen von      Einträgen der Unicast-Adresstabelle genutzt.1: Nur die Sourceadressen von Unicast-Telegrammen werden     zum Lernen der Unicast-Adresstabelle genutzt
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB__MSK_LEARNING_DISABLE                                                   0x00000200
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB_SHFT_LEARNING_DISABLE                                                   9
// ACCESS__LEARNING_DISABLE: r,w
// DESCR__LEARNING_DISABLE: nur im Back-to-Back-Mode (Bit 8  = ‚1’) für Kanal B relevant:0: Dynamische Einträge der Unicast-Adress-Tabelle können aus    den Sourceadressen von Telegrammen gelernt  werden1: Es werden keine neuen Einträge gelernt
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB__MSK_BACK_TO_BACK_MODE                                                  0x00000100
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB_SHFT_BACK_TO_BACK_MODE                                                  8
// ACCESS__BACK_TO_BACK_MODE: r,w
// DESCR__BACK_TO_BACK_MODE: Polarität des Owner-Bits an den Sende-SS’en von Kanal B wird für die automatisch Kopplung zweier Makros zu einem Mehr-Port-Switch gedreht.0: Owner-Bit-Polarität  = Empfangs-SS’en von Kanal B1: Owner-Bit-Polarität / = Empfangs-SS’en von Kanal B
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB__MSK_ENABLE_PERIOD                                                      0x000000FF
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB_SHFT_ENABLE_PERIOD                                                      0
// ACCESS__ENABLE_PERIOD: r,w
// DESCR__ENABLE_PERIOD: Abstand in Einheiten von 640 ns (FPGA: 1280 ns) in dem periodisch die beiden Empfangs- und Sende-Schnittstellen des Kanals B freigegeben werden.Bei ‚0’h unterbleibt der periodische Anstoss.
#define IRTE_R7_REG_NRT_AUTO_ENABLE_CHB_RST__VAL                          0x00000000


#define IRTE_R7_REG_ARP_TABLE_BASE                                                                          0x0001200C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ARP_TABLE_BASE__MSK_IP_TABLE_BASE                                                           0x001FFFFF
#define IRTE_R7_REG_ARP_TABLE_BASE_SHFT_IP_TABLE_BASE                                                           0
// ACCESS__IP_TABLE_BASE: r,w
// DESCR__IP_TABLE_BASE: KRAM-Byte-Adresse; zeigt auf den Start der IP-Tabelle für ARP-Telegramme; muß 8-Byte-aligned sein, d.h. es muß immer gelten: IP-Base-Address 2..0  = 000.Der Eintrag „1FFFFFh“ zeigt an, dass keine IP-Tabelle vorhanden ist. Alle ARP-Telegramme werden gefiltert.
#define IRTE_R7_REG_ARP_TABLE_BASE_RST__VAL                               0x00000000


#define IRTE_R7_REG_ARP_TABLE_LENGTH                                                                        0x00012010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ARP_TABLE_LENGTH__MSK_IP_TABLE_LENGTH                                                       0x00000007
#define IRTE_R7_REG_ARP_TABLE_LENGTH_SHFT_IP_TABLE_LENGTH                                                       0
// ACCESS__IP_TABLE_LENGTH: r,w
// DESCR__IP_TABLE_LENGTH: Länge des IP-Tabelle in Anzahl der Einträge - 1
#define IRTE_R7_REG_ARP_TABLE_LENGTH_RST__VAL                             0x00000000


#define IRTE_R7_REG_DCP_TABLE_BASE                                                                          0x00012014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DCP_TABLE_BASE__MSK_FILTERTYPE_TABLE_BASE                                                   0x001FFFFF
#define IRTE_R7_REG_DCP_TABLE_BASE_SHFT_FILTERTYPE_TABLE_BASE                                                   0
// ACCESS__FILTERTYPE_TABLE_BASE: r,w
// DESCR__FILTERTYPE_TABLE_BASE: KRAM-Byte-Adresse; zeigt auf den Start der Filter-Tabelle für DCP-Telegramme; muß 8-Byte-aligned sein, d.h. es muß immer gelten: Filter-Base-Address 2..0  = 000.Der Eintrag „1FFFFFh“ zeigt an, dass keine Filter-Tabelle vorhanden ist. Die DCP-Telegramme werden nicht gefiltert.
#define IRTE_R7_REG_DCP_TABLE_BASE_RST__VAL                               0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_MASK                                                                     0x00013008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_MASK__MSK_DATENSTATUS_MASK                                                   0x000000FF
#define IRTE_R7_REG_APPLDATASTATUS_MASK_SHFT_DATENSTATUS_MASK                                                   0
// ACCESS__DATENSTATUS_MASK: r,w
// DESCR__DATENSTATUS_MASK: Bitweise Auswahl der Datenstatus-Bits:‘0’: aus Register “ApplDataStatus_1…._7‘1’: aus Datenstatus im Datenpuffer
#define IRTE_R7_REG_APPLDATASTATUS_MASK_RST__VAL                          0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_1                                                                        0x0001300C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_1__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_1_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_1__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_1_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_1__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_1_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_1__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_1_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_1_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_2                                                                        0x00013010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_2__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_2_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_2__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_2_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_2__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_2_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_2__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_2_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_2_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_3                                                                        0x00013014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_3__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_3_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_3__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_3_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_3__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_3_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_3__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_3_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_3_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_4                                                                        0x00013018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_4__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_4_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_4__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_4_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_4__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_4_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_4__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_4_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_4_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_5                                                                        0x0001301C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_5__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_5_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_5__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_5_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_5__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_5_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_5__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_5_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_5_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_6                                                                        0x00013020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_6__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_6_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_6__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_6_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_6__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_6_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_6__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_6_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_6_RST__VAL                             0x00000000


#define IRTE_R7_REG_APPLDATASTATUS_7                                                                        0x00013024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_APPLDATASTATUS_7__MSK_STATUS7_4                                                             0x000000F0
#define IRTE_R7_REG_APPLDATASTATUS_7_SHFT_STATUS7_4                                                             4
// ACCESS__STATUS7_4: r,w
// DESCR__STATUS7_4: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_7__MSK_STANDBY                                                               0x00000008
#define IRTE_R7_REG_APPLDATASTATUS_7_SHFT_STANDBY                                                               3
// ACCESS__STANDBY: r,w
// DESCR__STANDBY: ‚0’: kein Redundanz-Master‚1’: Sender ist Redundanz-Master
#define IRTE_R7_REG_APPLDATASTATUS_7__MSK_DATAVALID                                                             0x00000004
#define IRTE_R7_REG_APPLDATASTATUS_7_SHFT_DATAVALID                                                             2
// ACCESS__DATAVALID: r,w
// DESCR__DATAVALID: Ausgangsdatensatz ist konsistent und gültig. Durch die SW veränderbar.
#define IRTE_R7_REG_APPLDATASTATUS_7__MSK_STATUS1_0                                                             0x00000003
#define IRTE_R7_REG_APPLDATASTATUS_7_SHFT_STATUS1_0                                                             0
// ACCESS__STATUS1_0: r,w
// DESCR__STATUS1_0: SW-Status
#define IRTE_R7_REG_APPLDATASTATUS_7_RST__VAL                             0x00000000


#define IRTE_R7_REG_IRT_ASYNCMODE                                                                           0x00013028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_ASYNCMODE__MSK_IRT_OUTFREEZE                                                            0x00000002
#define IRTE_R7_REG_IRT_ASYNCMODE_SHFT_IRT_OUTFREEZE                                                            1
// ACCESS__IRT_OUTFREEZE: r,w
// DESCR__IRT_OUTFREEZE: Die asynchrone Anwendung verschickt die Ausgangsdaten inkonsistent.
#define IRTE_R7_REG_IRT_ASYNCMODE__MSK_IRT_INFREEZE                                                             0x00000001
#define IRTE_R7_REG_IRT_ASYNCMODE_SHFT_IRT_INFREEZE                                                             0
// ACCESS__IRT_INFREEZE: r,w
// DESCR__IRT_INFREEZE: Die asynchrone Anwendung verschickt die Eingangsdaten inkonsistent.
#define IRTE_R7_REG_IRT_ASYNCMODE_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAINDESTADR0                                                                           0x0001302C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAINDESTADR0__MSK_DMAINDESTADR0                                                            0xFFFFFFFF
#define IRTE_R7_REG_DMAINDESTADR0_SHFT_DMAINDESTADR0                                                            0
// ACCESS__DMAINDESTADR0: r,w
// DESCR__DMAINDESTADR0: UserRAM-Byte-Adresse; zeigt auf das Ziel des ersten Empfangspuffers im UserRAM für die IRT-Eingangsdaten; muß 64-Bit-aligned sein, d.h DMAInDestAdr0  2..0  = 000
#define IRTE_R7_REG_DMAINDESTADR0_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAINDESTADR1                                                                           0x00013030
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAINDESTADR1__MSK_DMAINDESTADR1                                                            0xFFFFFFFF
#define IRTE_R7_REG_DMAINDESTADR1_SHFT_DMAINDESTADR1                                                            0
// ACCESS__DMAINDESTADR1: r,w
// DESCR__DMAINDESTADR1: UserRAM-Byte-Adresse; zeigt auf das Ziel des zweiten Empfangspuffers im UserRAM für die IRT-Eingangsdaten; muß 64-Bit-aligned sein, d.h DMAInDestAdr1  2..0  = 000
#define IRTE_R7_REG_DMAINDESTADR1_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAINSRCADR                                                                             0x00013034
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAINSRCADR__MSK_DMAINSRCADR                                                                0x001FFFFF
#define IRTE_R7_REG_DMAINSRCADR_SHFT_DMAINSRCADR                                                                0
// ACCESS__DMAINSRCADR: r,w
// DESCR__DMAINSRCADR: KRAM-Byte-Adresse; zeigt auf den Start des Abbildes der Eingangsdaten im KRAM; muß 64-Bit-aligned sein, d.h DMAInSrcAdr  2..0  = 000
#define IRTE_R7_REG_DMAINSRCADR_RST__VAL                                  0x00000000


#define IRTE_R7_REG_DMAINLENGTH                                                                             0x00013038
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAINLENGTH__MSK_DMAINLENGTH                                                                0x0000FFFF
#define IRTE_R7_REG_DMAINLENGTH_SHFT_DMAINLENGTH                                                                0
// ACCESS__DMAINLENGTH: r,w
// DESCR__DMAINLENGTH: Ist die Anzahl der Bytes für die Größe des IRT-Eingangspuffers für die IRT-Eingangsdaten.
#define IRTE_R7_REG_DMAINLENGTH_RST__VAL                                  0x00000000


#define IRTE_R7_REG_DMAOUTSRCADR0                                                                           0x0001303C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAOUTSRCADR0__MSK_DMAOUTSRCADR0                                                            0xFFFFFFFF
#define IRTE_R7_REG_DMAOUTSRCADR0_SHFT_DMAOUTSRCADR0                                                            0
// ACCESS__DMAOUTSRCADR0: r,w
// DESCR__DMAOUTSRCADR0: UserRAM-Byte-Adresse; zeigt auf den Start des ersten Ausgangspuffers im UserRAM für die IRT-Ausgangsdaten; muß 64-Bit-aligned sein, d.h DMAOutSrcAdr0  2..0  = 000
#define IRTE_R7_REG_DMAOUTSRCADR0_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAOUTSRCADR1                                                                           0x00013040
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAOUTSRCADR1__MSK_DMAOUTSRCADR1                                                            0xFFFFFFFF
#define IRTE_R7_REG_DMAOUTSRCADR1_SHFT_DMAOUTSRCADR1                                                            0
// ACCESS__DMAOUTSRCADR1: r,w
// DESCR__DMAOUTSRCADR1: UserRAM-Byte-Adresse; zeigt auf den Start des zweiten Ausgangspuffers im UserRAM für die IRT-Ausgangsdaten; muß 64-Bit-aligned sein, d.h DMAOutSrcAdr1  2..0  = 000
#define IRTE_R7_REG_DMAOUTSRCADR1_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAOUTDESTADR                                                                           0x00013044
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAOUTDESTADR__MSK_DMAOUTDESTADR                                                            0x001FFFFF
#define IRTE_R7_REG_DMAOUTDESTADR_SHFT_DMAOUTDESTADR                                                            0
// ACCESS__DMAOUTDESTADR: r,w
// DESCR__DMAOUTDESTADR: KRAM-Byte-Adresse; zeigt auf das Ziel des Abbildes der Ausgangsdaten im KRAM; muß 64-Bit-aligned sein, d.h DMAOutDestAdr 2..0  = 000
#define IRTE_R7_REG_DMAOUTDESTADR_RST__VAL                                0x00000000


#define IRTE_R7_REG_DMAOUTLENGTH                                                                            0x00013048
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_DMAOUTLENGTH__MSK_DMAOUTLENGTH                                                              0x0000FFFF
#define IRTE_R7_REG_DMAOUTLENGTH_SHFT_DMAOUTLENGTH                                                              0
// ACCESS__DMAOUTLENGTH: r,w
// DESCR__DMAOUTLENGTH: Ist die Anzahl der Bytes für die Größe des IRT-Ausgangspuffers für die IRT-Ausgangsdaten.
#define IRTE_R7_REG_DMAOUTLENGTH_RST__VAL                                 0x00000000


#define IRTE_R7_REG_IRT_TIMECOMPARE                                                                         0x0001304C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_TIMECOMPARE__MSK_IRT_TIMECOMPARE                                                        0xFFFFFFFF
#define IRTE_R7_REG_IRT_TIMECOMPARE_SHFT_IRT_TIMECOMPARE                                                        0
// ACCESS__IRT_TIMECOMPARE: r,w
// DESCR__IRT_TIMECOMPARE: Vergleichswert zur Bildung eines Interrupt und Start der Anwendung
#define IRTE_R7_REG_IRT_TIMECOMPARE_RST__VAL                              0x00000000


#define IRTE_R7_REG_MAX_IRT_FRAME_ID                                                                        0x00013050
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAX_IRT_FRAME_ID__MSK_MAX_IRT_FRAME_ID                                                      0x0000FFFF
#define IRTE_R7_REG_MAX_IRT_FRAME_ID_SHFT_MAX_IRT_FRAME_ID                                                      0
// ACCESS__MAX_IRT_FRAME_ID: r,w
// DESCR__MAX_IRT_FRAME_ID: Wert der max. möglichen Frame-ID, die für IRT-Kommunikation zugelassen wird.Wert  = 0000h: die Frame-ID-Überwachung ist abgeschaltet
#define IRTE_R7_REG_MAX_IRT_FRAME_ID_RST__VAL                             0x00000000


#define IRTE_R7_REG_NRT_FCW_UPPER_LIMIT                                                                     0x00016000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_FCW_UPPER_LIMIT__MSK_NRT_FCW_UPPER_LIMIT                                                0x00000FFF
#define IRTE_R7_REG_NRT_FCW_UPPER_LIMIT_SHFT_NRT_FCW_UPPER_LIMIT                                                0
// ACCESS__NRT_FCW_UPPER_LIMIT: r,w
// DESCR__NRT_FCW_UPPER_LIMIT: Obere Schwelle der in der NRT-FCW-Leerliste mindestens vorzuhaltenden FCWs. Wird „FFFh“ eingetragen, ist die Hysterese abgeschaltet und es gilt lediglich „NRT_FCW_Lower_Limit“.
#define IRTE_R7_REG_NRT_FCW_UPPER_LIMIT_RST__VAL                          0x00000000


#define IRTE_R7_REG_NRT_FCW_LOWER_LIMIT                                                                     0x00016004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_FCW_LOWER_LIMIT__MSK_NRT_FCW_LOWER_LIMIT                                                0x00000FFF
#define IRTE_R7_REG_NRT_FCW_LOWER_LIMIT_SHFT_NRT_FCW_LOWER_LIMIT                                                0
// ACCESS__NRT_FCW_LOWER_LIMIT: r,w
// DESCR__NRT_FCW_LOWER_LIMIT: Untere Schwelle der in der NRT-FCW-Leerliste mindestens vorzuhaltenden FCWs.
#define IRTE_R7_REG_NRT_FCW_LOWER_LIMIT_RST__VAL                          0x00000000


#define IRTE_R7_REG_NRT_DB_UPPER_LIMIT                                                                      0x00016008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_DB_UPPER_LIMIT__MSK_NRT_DB_UPPER_LIMIT                                                  0x00000FFF
#define IRTE_R7_REG_NRT_DB_UPPER_LIMIT_SHFT_NRT_DB_UPPER_LIMIT                                                  0
// ACCESS__NRT_DB_UPPER_LIMIT: r,w
// DESCR__NRT_DB_UPPER_LIMIT: Obere Schwelle der in der NRT-DB-Leerliste mindestens vorzuhaltenden Daten-puffer. Wird „FFFh“ eingetragen, ist die Hysterese abgeschaltet und es gilt lediglich „NRT_DB_Lower_Limit“.
#define IRTE_R7_REG_NRT_DB_UPPER_LIMIT_RST__VAL                           0x00000000


#define IRTE_R7_REG_NRT_DB_LOWER_LIMIT                                                                      0x0001600C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_DB_LOWER_LIMIT__MSK_NRT_DB_LOWER_LIMIT                                                  0x00000FFF
#define IRTE_R7_REG_NRT_DB_LOWER_LIMIT_SHFT_NRT_DB_LOWER_LIMIT                                                  0
// ACCESS__NRT_DB_LOWER_LIMIT: r,w
// DESCR__NRT_DB_LOWER_LIMIT: Untere Schwelle der in der NRT-DB-Leerliste mindestens vorzuhaltenden Datenpuffer.
#define IRTE_R7_REG_NRT_DB_LOWER_LIMIT_RST__VAL                           0x00000000


#define IRTE_R7_REG_HOL_CH_UPPER_LIMIT                                                                      0x00016010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_CH_UPPER_LIMIT__MSK_HOL_CH_UPPER_LIMIT                                                  0x00000FFF
#define IRTE_R7_REG_HOL_CH_UPPER_LIMIT_SHFT_HOL_CH_UPPER_LIMIT                                                  0
// ACCESS__HOL_CH_UPPER_LIMIT: r,w
// DESCR__HOL_CH_UPPER_LIMIT: Obere Schwelle der für eine lokale Schnittstelle (CHA, CHB bzw. SS0/SS1) maximal zulässige Datenpufferanzahl
#define IRTE_R7_REG_HOL_CH_UPPER_LIMIT_RST__VAL                           0x00000000


#define IRTE_R7_REG_HOL_CH_LOWER_LIMIT                                                                      0x00016014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_CH_LOWER_LIMIT__MSK_HOL_CH_LOWER_LIMIT                                                  0x00000FFF
#define IRTE_R7_REG_HOL_CH_LOWER_LIMIT_SHFT_HOL_CH_LOWER_LIMIT                                                  0
// ACCESS__HOL_CH_LOWER_LIMIT: r,w
// DESCR__HOL_CH_LOWER_LIMIT: Obere Schwelle der für eine lokale Schnittstelle (CHA, CHB bzw. SS0/SS1) maximal zulässige Datenpufferanzahl. Wird „FFFh“ eingetragen, ist die Hysterese abgeschaltet und es gilt lediglich „HOL_CH_Upper_Limit“.
#define IRTE_R7_REG_HOL_CH_LOWER_LIMIT_RST__VAL                           0x00000000


#define IRTE_R7_REG_HOL_PORT_UPPER_LIMIT                                                                    0x00016018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_PORT_UPPER_LIMIT__MSK_HOL_PORT_UPPER_LIMIT                                              0x00000FFF
#define IRTE_R7_REG_HOL_PORT_UPPER_LIMIT_SHFT_HOL_PORT_UPPER_LIMIT                                              0
// ACCESS__HOL_PORT_UPPER_LIMIT: r,w
// DESCR__HOL_PORT_UPPER_LIMIT: Obere Schwelle der für einen Port maximal zulässigen Datenpufferanzahl.
#define IRTE_R7_REG_HOL_PORT_UPPER_LIMIT_RST__VAL                         0x00000000


#define IRTE_R7_REG_HOL_PORT_LOWER_LIMIT                                                                    0x0001601C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_PORT_LOWER_LIMIT__MSK_HOL_PORT_LOWER_LIMIT                                              0x00000FFF
#define IRTE_R7_REG_HOL_PORT_LOWER_LIMIT_SHFT_HOL_PORT_LOWER_LIMIT                                              0
// ACCESS__HOL_PORT_LOWER_LIMIT: r,w
// DESCR__HOL_PORT_LOWER_LIMIT: Untere Schwelle der für einen Port maximal zulässigen Datenpufferanzahl. Wird „FFFh“ eingetragen, ist die Hysterese abgeschaltet und es gilt lediglich „HOL_Port_Upper_Limit“.
#define IRTE_R7_REG_HOL_PORT_LOWER_LIMIT_RST__VAL                         0x00000000


#define IRTE_R7_REG_UC_TABLE_BASE                                                                           0x00016020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UC_TABLE_BASE__MSK_UC_TABLE_BASE                                                            0x001FFFFF
#define IRTE_R7_REG_UC_TABLE_BASE_SHFT_UC_TABLE_BASE                                                            0
// ACCESS__UC_TABLE_BASE: r,w
// DESCR__UC_TABLE_BASE: KRAM-Byte-Adresse; zeigt auf den Start der Unicast-Adresstabelle;muß 8-Byte-aligned sein, d.h. es muß immer gelten: UC-Base-Address 2..0  = 000.Der Eintrag „1FFFFFh“ zeigt an, dass keine UC-Adresstabelle vorhanden ist. Die Zielfindung erfolgt über UC-Default.
#define IRTE_R7_REG_UC_TABLE_BASE_RST__VAL                                0x00000000


#define IRTE_R7_REG_UC_TABLE_LENGTH                                                                         0x00016024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UC_TABLE_LENGTH__MSK_UC_TABLE_LENGTH                                                        0x00000FFF
#define IRTE_R7_REG_UC_TABLE_LENGTH_SHFT_UC_TABLE_LENGTH                                                        0
// ACCESS__UC_TABLE_LENGTH: r,w
// DESCR__UC_TABLE_LENGTH: Zahl der Unicast-Adresstabellen-Einträge – 1.Beachte: Die Zahl der Einträge muß eine Zweierpotenz sein.
#define IRTE_R7_REG_UC_TABLE_LENGTH_RST__VAL                              0x00000000


#define IRTE_R7_REG_UC_DEFAULT_CTRL                                                                         0x00016028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_DESTINATION_PORT3                                                      0x00000020
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_DESTINATION_PORT3                                                      5
// ACCESS__DESTINATION_PORT3: r,w
// DESCR__DESTINATION_PORT3: NRT-Telegramm an Port 3 weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_DESTINATION_PORT2                                                      0x00000010
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_DESTINATION_PORT2                                                      4
// ACCESS__DESTINATION_PORT2: r,w
// DESCR__DESTINATION_PORT2: NRT-Telegramm an Port 2 weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_DESTINATION_PORT1                                                      0x00000008
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_DESTINATION_PORT1                                                      3
// ACCESS__DESTINATION_PORT1: r,w
// DESCR__DESTINATION_PORT1: NRT-Telegramm an Port 1 weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_DESTINATION_PORT0                                                      0x00000004
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_DESTINATION_PORT0                                                      2
// ACCESS__DESTINATION_PORT0: r,w
// DESCR__DESTINATION_PORT0: NRT-Telegramm an Port 0 weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_CHB                                                                    0x00000002
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_CHB                                                                    1
// ACCESS__CHB: r,w
// DESCR__CHB: NRT-Telegramm an Kanal B weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL__MSK_CHA                                                                    0x00000001
#define IRTE_R7_REG_UC_DEFAULT_CTRL_SHFT_CHA                                                                    0
// ACCESS__CHA: r,w
// DESCR__CHA: NRT-Telegramm an Kanal A weiterleiten
#define IRTE_R7_REG_UC_DEFAULT_CTRL_RST__VAL                              0x00000000


#define IRTE_R7_REG_MC_TABLE_BASE                                                                           0x0001602C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MC_TABLE_BASE__MSK_MC_TABLE_BASE                                                            0x001FFFFF
#define IRTE_R7_REG_MC_TABLE_BASE_SHFT_MC_TABLE_BASE                                                            0
// ACCESS__MC_TABLE_BASE: r,w
// DESCR__MC_TABLE_BASE: KRAM-Byte-Adresse; zeigt auf den Start der Multicast-Adresstabelle;muß 8-Byte-aligned sein, d.h. es muß immer gelten: MC-Base-Address 2..0  = 000.Der Eintrag „1FFFFFh“ zeigt an, dass keine MC-Adresstabelle vorhanden ist. Die Zielfindung erfolgt über die UC-Adresstabelle. Ist auch dort kein Eintrag vorhanden, dann über MC-Default.
#define IRTE_R7_REG_MC_TABLE_BASE_RST__VAL                                0x00000000


#define IRTE_R7_REG_MC_TABLE_LENGTH                                                                         0x00016030
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MC_TABLE_LENGTH__MSK_MC_TABLE_LENGTH                                                        0x000000FF
#define IRTE_R7_REG_MC_TABLE_LENGTH_SHFT_MC_TABLE_LENGTH                                                        0
// ACCESS__MC_TABLE_LENGTH: r,w
// DESCR__MC_TABLE_LENGTH: Anzahl der Einträge – 1 der MC-Control-Arrays innerhalb der Multicast-Adresstabelle. Beachte: Die Anzahl der Einträge muß eine Zweierpotenz sein.
#define IRTE_R7_REG_MC_TABLE_LENGTH_RST__VAL                              0x00000000


#define IRTE_R7_REG_MC_DEFAULT_CTRL                                                                         0x00016034
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_DESTINATION_PORT3                                                      0x00000020
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_DESTINATION_PORT3                                                      5
// ACCESS__DESTINATION_PORT3: r,w
// DESCR__DESTINATION_PORT3: NRT-Telegramm an Port 3 weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_DESTINATION_PORT2                                                      0x00000010
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_DESTINATION_PORT2                                                      4
// ACCESS__DESTINATION_PORT2: r,w
// DESCR__DESTINATION_PORT2: NRT-Telegramm an Port 2 weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_DESTINATION_PORT1                                                      0x00000008
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_DESTINATION_PORT1                                                      3
// ACCESS__DESTINATION_PORT1: r,w
// DESCR__DESTINATION_PORT1: NRT-Telegramm an Port 1 weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_DESTINATION_PORT0                                                      0x00000004
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_DESTINATION_PORT0                                                      2
// ACCESS__DESTINATION_PORT0: r,w
// DESCR__DESTINATION_PORT0: NRT-Telegramm an Port 0 weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_CHB                                                                    0x00000002
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_CHB                                                                    1
// ACCESS__CHB: r,w
// DESCR__CHB: NRT-Telegramm an Kanal B weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL__MSK_CHA                                                                    0x00000001
#define IRTE_R7_REG_MC_DEFAULT_CTRL_SHFT_CHA                                                                    0
// ACCESS__CHA: r,w
// DESCR__CHA: NRT-Telegramm an Kanal A weiterleiten
#define IRTE_R7_REG_MC_DEFAULT_CTRL_RST__VAL                              0x00000000


#define IRTE_R7_REG_UCMC_LFSR_MASK                                                                          0x00016038
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UCMC_LFSR_MASK__MSK_UCMC_LFSR_CTRL                                                          0x00000FFF
#define IRTE_R7_REG_UCMC_LFSR_MASK_SHFT_UCMC_LFSR_CTRL                                                          0
// ACCESS__UCMC_LFSR_CTRL: r,w
// DESCR__UCMC_LFSR_CTRL: Maskierung der Rückkopplungspfade des LFSR. Ein gesetztes Bit gibt den entsprechenden Rückkopplungspfad frei.
#define IRTE_R7_REG_UCMC_LFSR_MASK_RST__VAL                               0x00000000


#define IRTE_R7_REG_UC_TABLE_RANGE                                                                          0x0001603C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UC_TABLE_RANGE__MSK_UC_TABLE_RANGE                                                          0x0000000F
#define IRTE_R7_REG_UC_TABLE_RANGE_SHFT_UC_TABLE_RANGE                                                          0
// ACCESS__UC_TABLE_RANGE: r,w
// DESCR__UC_TABLE_RANGE: Größe des Nachbarschaftsbereichs - beginnend ab Einsprung-Offset (der Offset wird durch das LFSR bestimmt).
#define IRTE_R7_REG_UC_TABLE_RANGE_RST__VAL                               0x00000000


#define IRTE_R7_REG_VLAN_TABLE_BASE                                                                         0x00016040
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_VLAN_TABLE_BASE__MSK_VLAN_TABLE_BASE                                                        0x001FFFFF
#define IRTE_R7_REG_VLAN_TABLE_BASE_SHFT_VLAN_TABLE_BASE                                                        0
// ACCESS__VLAN_TABLE_BASE: r,w
// DESCR__VLAN_TABLE_BASE: KRAM-Byte-Adresse; zeigt auf den Start der VLAN-Adresstabelle;muß 8-Byte-aligned sein, d.h. es muß immer gelten: VLAN-Base-Address 2..0  = 000.Der Eintrag „1FFFFFh“ zeigt an, dass keine VLAN-Adresstabelle vorhanden ist. Alle Telegramme werden gefiltert.
#define IRTE_R7_REG_VLAN_TABLE_BASE_RST__VAL                              0x00000000


#define IRTE_R7_REG_VLAN_TABLE_LENGTH                                                                       0x00016044
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_VLAN_TABLE_LENGTH__MSK_VLAN_TABLE_LENGTH                                                    0x000000FF
#define IRTE_R7_REG_VLAN_TABLE_LENGTH_SHFT_VLAN_TABLE_LENGTH                                                    0
// ACCESS__VLAN_TABLE_LENGTH: r,w
// DESCR__VLAN_TABLE_LENGTH: Länge der VLAN-Adresstabelle in Anzahl der Einträge - 1
#define IRTE_R7_REG_VLAN_TABLE_LENGTH_RST__VAL                            0x00000000


#define IRTE_R7_REG_VLAN_LFSR_MASK                                                                          0x00016048
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_VLAN_LFSR_MASK__MSK_VLAN_LFSR_CTRL                                                          0x000000FF
#define IRTE_R7_REG_VLAN_LFSR_MASK_SHFT_VLAN_LFSR_CTRL                                                          0
// ACCESS__VLAN_LFSR_CTRL: r,w
// DESCR__VLAN_LFSR_CTRL: Maskierung der Rückkopplungspfade des LFSR. Ein gesetztes Bit gibt den entsprechenden Rückkopplungspfad frei.
#define IRTE_R7_REG_VLAN_LFSR_MASK_RST__VAL                               0x00000000


#define IRTE_R7_REG_GROUP_NUMBER                                                                            0x0001604C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_GROUP_NUMBER__MSK_PORT3_GROUP                                                               0x00000020
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_PORT3_GROUP                                                               5
// ACCESS__PORT3_GROUP: r,w
// DESCR__PORT3_GROUP: Gruppenzugehörigkeit von Port 3
#define IRTE_R7_REG_GROUP_NUMBER__MSK_PORT2_GROUP                                                               0x00000010
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_PORT2_GROUP                                                               4
// ACCESS__PORT2_GROUP: r,w
// DESCR__PORT2_GROUP: Gruppenzugehörigkeit von Port 2
#define IRTE_R7_REG_GROUP_NUMBER__MSK_PORT1_GROUP                                                               0x00000008
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_PORT1_GROUP                                                               3
// ACCESS__PORT1_GROUP: r,w
// DESCR__PORT1_GROUP: Gruppenzugehörigkeit von Port 1
#define IRTE_R7_REG_GROUP_NUMBER__MSK_PORT0_GROUP                                                               0x00000004
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_PORT0_GROUP                                                               2
// ACCESS__PORT0_GROUP: r,w
// DESCR__PORT0_GROUP: Gruppenzugehörigkeit von Port 0
#define IRTE_R7_REG_GROUP_NUMBER__MSK_CHB_GROUP                                                                 0x00000002
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_CHB_GROUP                                                                 1
// ACCESS__CHB_GROUP: r,w
// DESCR__CHB_GROUP: Gruppenzugehörigkeit von Kanal B
#define IRTE_R7_REG_GROUP_NUMBER__MSK_CHA_GROUP                                                                 0x00000001
#define IRTE_R7_REG_GROUP_NUMBER_SHFT_CHA_GROUP                                                                 0
// ACCESS__CHA_GROUP: r,w
// DESCR__CHA_GROUP: Gruppenzugehörigkeit von Kanal A
#define IRTE_R7_REG_GROUP_NUMBER_RST__VAL                                 0x00000000


#define IRTE_R7_REG_TIMER_SCOREBOARD_BASE                                                                   0x00016050
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TIMER_SCOREBOARD_BASE__MSK_TIMER_SCOREBOARD_BASE                                            0x001FFFFF
#define IRTE_R7_REG_TIMER_SCOREBOARD_BASE_SHFT_TIMER_SCOREBOARD_BASE                                            0
// ACCESS__TIMER_SCOREBOARD_BASE: r,w
// DESCR__TIMER_SCOREBOARD_BASE: Zeiger auf das Timer-Scoreboard .Die Zeiger ist ein 8-Byte granularer Offset in den IRT-Switch Adressraum.
#define IRTE_R7_REG_TIMER_SCOREBOARD_BASE_RST__VAL                        0x00000000


#define IRTE_R7_REG_TIMER_SCOREBOARD_ENTRIES                                                                0x00016054
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TIMER_SCOREBOARD_ENTRIES__MSK_TIMER_SCOREBOARD_ENTRIES                                      0x000003FF
#define IRTE_R7_REG_TIMER_SCOREBOARD_ENTRIES_SHFT_TIMER_SCOREBOARD_ENTRIES                                      0
// ACCESS__TIMER_SCOREBOARD_ENTRIES: r,w
// DESCR__TIMER_SCOREBOARD_ENTRIES: Gibt die Gesamtanzahl der möglichen Entries im Timer-Scoreboard an (muss immer ein Vielfaches von 2 sein).
#define IRTE_R7_REG_TIMER_SCOREBOARD_ENTRIES_RST__VAL                     0x00000000


#define IRTE_R7_REG_PS_SCOREBOARD_0_BASE                                                                    0x00016058
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_PS_SCOREBOARD_0_BASE__MSK_PS_SCOREBOARD_0_BASE                                              0x001FFFFF
#define IRTE_R7_REG_PS_SCOREBOARD_0_BASE_SHFT_PS_SCOREBOARD_0_BASE                                              0
// ACCESS__PS_SCOREBOARD_0_BASE: r,w
// DESCR__PS_SCOREBOARD_0_BASE: Zeiger auf das Producer-State-Scoreboard 0. Die Zeiger ist ein 8-Byte granularer Offset in den IRT-Switch Adressraum.
#define IRTE_R7_REG_PS_SCOREBOARD_0_BASE_RST__VAL                         0x00000000


#define IRTE_R7_REG_PS_SCOREBOARD_1_BASE                                                                    0x0001605C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_PS_SCOREBOARD_1_BASE__MSK_PS_SCOREBOARD_1_BASE                                              0x001FFFFF
#define IRTE_R7_REG_PS_SCOREBOARD_1_BASE_SHFT_PS_SCOREBOARD_1_BASE                                              0
// ACCESS__PS_SCOREBOARD_1_BASE: r,w
// DESCR__PS_SCOREBOARD_1_BASE: Zeiger auf das Producer-State-Scoreboard 1. Die Zeiger ist ein 8-Byte granularer Offset in den IRT-Switch Adressraum.
#define IRTE_R7_REG_PS_SCOREBOARD_1_BASE_RST__VAL                         0x00000000


#define IRTE_R7_REG_TMLIST_BASE                                                                             0x00016060
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TMLIST_BASE__MSK_TMLIST_BASE                                                                0x001FFFFF
#define IRTE_R7_REG_TMLIST_BASE_SHFT_TMLIST_BASE                                                                0
// ACCESS__TMLIST_BASE: r,w
// DESCR__TMLIST_BASE: Zeiger auf die TimeMaster-ListeDer Zeiger ist ein 8-Byte granularer Offset in den IRT-Switch Adressraum.
#define IRTE_R7_REG_TMLIST_BASE_RST__VAL                                  0x00000000


#define IRTE_R7_REG_TMLIST_LENGTH                                                                           0x00016064
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TMLIST_LENGTH__MSK_TMLIST_LENGTH                                                            0x00000003
#define IRTE_R7_REG_TMLIST_LENGTH_SHFT_TMLIST_LENGTH                                                            0
// ACCESS__TMLIST_LENGTH: r,w
// DESCR__TMLIST_LENGTH: Anzahl der Einträge - 1 in der TimeMaster-List pro Protokollgruppe. Dabei sind die Einträge 1(Wert =0), 2(Wert =1) und 4(Wert =3) zulässig
#define IRTE_R7_REG_TMLIST_LENGTH_RST__VAL                                0x00000000


#define IRTE_R7_REG_FC_MASK                                                                                 0x00016068
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FC_MASK__MSK_FC_MASK                                                                        0x000003FF
#define IRTE_R7_REG_FC_MASK_SHFT_FC_MASK                                                                        0
// ACCESS__FC_MASK: r,w
// DESCR__FC_MASK: Bitmaske für das Empfangen von Telegrammen bei unterschreiten der Low-Water-Mark für NRT-FCWs oder NRT-DB an den Ports und am NRT-API.Bit n entspricht dabei der Priorität n.‚0‘: Telegramme mit dieser Priorität werden nicht angenommen.‚1‘: Telegramme mit dieser Priorität werden angenommen.
#define IRTE_R7_REG_FC_MASK_RST__VAL                                      0x00000000


#define IRTE_R7_REG_HOL_MASK_P0                                                                             0x0001606C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_MASK_P0__MSK_HOL_MASK_P0                                                                0x000003FF
#define IRTE_R7_REG_HOL_MASK_P0_SHFT_HOL_MASK_P0                                                                0
// ACCESS__HOL_MASK_P0: r,w
// DESCR__HOL_MASK_P0: Bitmaske für das Annehmen von Sendeaufträgen bei erreichtem HOL-Limit an Port 0. Bit n entspricht dabei der Priorität n.‚0‘: Sendeaufträge mit dieser Priorität werden nicht angenommen.‚1‘: Sendeaufträge mit dieser Priorität werden angenommen.
#define IRTE_R7_REG_HOL_MASK_P0_RST__VAL                                  0x00000000


#define IRTE_R7_REG_HOL_MASK_P1                                                                             0x00016070
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_MASK_P1__MSK_HOL_MASK_P1                                                                0x000003FF
#define IRTE_R7_REG_HOL_MASK_P1_SHFT_HOL_MASK_P1                                                                0
// ACCESS__HOL_MASK_P1: r,w
// DESCR__HOL_MASK_P1: Bitmaske für das Annehmen von Sendeaufträgen bei erreichtem HOL-Limit an Port 1. Bit n entspricht dabei der Priorität n.‚0‘: Sendeaufträge mit dieser Priorität werden nicht angenommen.‚1‘: Sendeaufträge mit dieser Priorität werden angenommen.
#define IRTE_R7_REG_HOL_MASK_P1_RST__VAL                                  0x00000000


#define IRTE_R7_REG_HOL_MASK_P2                                                                             0x00016074
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_MASK_P2__MSK_HOL_MASK_P2                                                                0x000003FF
#define IRTE_R7_REG_HOL_MASK_P2_SHFT_HOL_MASK_P2                                                                0
// ACCESS__HOL_MASK_P2: r,w
// DESCR__HOL_MASK_P2: Bitmaske für das Annehmen von Sendeaufträgen bei erreichtem HOL-Limit an Port 2. Bit n entspricht dabei der Priorität n.‚0‘: Sendeaufträge mit dieser Priorität werden nicht angenommen.‚1‘: Sendeaufträge mit dieser Priorität werden angenommen.
#define IRTE_R7_REG_HOL_MASK_P2_RST__VAL                                  0x00000000


#define IRTE_R7_REG_HOL_MASK_P3                                                                             0x00016078
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_MASK_P3__MSK_HOL_MASK_P3                                                                0x000003FF
#define IRTE_R7_REG_HOL_MASK_P3_SHFT_HOL_MASK_P3                                                                0
// ACCESS__HOL_MASK_P3: r,w
// DESCR__HOL_MASK_P3: Bitmaske für das Annehmen von Sendeaufträgen bei erreichtem HOL-Limit an Port 2. Bit n entspricht dabei der Priorität n.‚0‘: Sendeaufträge mit dieser Priorität werden nicht angenommen.‚1‘: Sendeaufträge mit dieser Priorität werden angenommen.
#define IRTE_R7_REG_HOL_MASK_P3_RST__VAL                                  0x00000000


#define IRTE_R7_REG_LOW_WATER_STATUS                                                                        0x00016420
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_LOW_WATER_STATUS__MSK_DB_STATE                                                              0x00000002
#define IRTE_R7_REG_LOW_WATER_STATUS_SHFT_DB_STATE                                                              1
// ACCESS__DB_STATE: r,
// DESCR__DB_STATE: 0: kein DB-Mangel1: Low-Water-Mark aufgrund von DB-Mangel aktiv
#define IRTE_R7_REG_LOW_WATER_STATUS__MSK_FCW_STATE                                                             0x00000001
#define IRTE_R7_REG_LOW_WATER_STATUS_SHFT_FCW_STATE                                                             0
// ACCESS__FCW_STATE: r,
// DESCR__FCW_STATE: 0: kein FCW-Mangel1: Low-Water-Mark aufgrund von FCW-Mangel aktiv
#define IRTE_R7_REG_LOW_WATER_STATUS_RST__VAL                             0x00000000


#define IRTE_R7_REG_NRT_FCW_COUNT                                                                           0x00016424
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_FCW_COUNT__MSK_NRT_FCW_COUNT                                                            0x00000FFF
#define IRTE_R7_REG_NRT_FCW_COUNT_SHFT_NRT_FCW_COUNT                                                            0
// ACCESS__NRT_FCW_COUNT: r,w
// DESCR__NRT_FCW_COUNT: Die Anzahl der in der NRT-FCW-Leerliste noch verfügbaren FCWs
#define IRTE_R7_REG_NRT_FCW_COUNT_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_DB_COUNT                                                                            0x00016428
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_DB_COUNT__MSK_NRT_DB_COUNT                                                              0x00000FFF
#define IRTE_R7_REG_NRT_DB_COUNT_SHFT_NRT_DB_COUNT                                                              0
// ACCESS__NRT_DB_COUNT: r,w
// DESCR__NRT_DB_COUNT: Die Anzahl der in der NRT-DB-Leerliste noch verfügbaren Datenpuffer
#define IRTE_R7_REG_NRT_DB_COUNT_RST__VAL                                 0x00000000


#define IRTE_R7_REG_SRT_FCW_COUNT                                                                           0x0001642C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_SRT_FCW_COUNT__MSK_SRT_FCW_COUNT                                                            0x00000FFF
#define IRTE_R7_REG_SRT_FCW_COUNT_SHFT_SRT_FCW_COUNT                                                            0
// ACCESS__SRT_FCW_COUNT: r,w
// DESCR__SRT_FCW_COUNT: Die Anzahl der in der SRT-FCW-Leerliste noch verfügbaren FCWs
#define IRTE_R7_REG_SRT_FCW_COUNT_RST__VAL                                0x00000000


#define IRTE_R7_REG_HOL_COUNT_CHA_SS0                                                                       0x00016430
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_CHA_SS0__MSK_HOL_CNT_CHA_SS0                                                      0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_CHA_SS0_SHFT_HOL_CNT_CHA_SS0                                                      0
// ACCESS__HOL_CNT_CHA_SS0: r,w
// DESCR__HOL_CNT_CHA_SS0: Die Anzahl der für das NRT-API CHA an SS0 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_CHA_SS0_RST__VAL                            0x00000000


#define IRTE_R7_REG_HOL_COUNT_CHA_SS1                                                                       0x00016434
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_CHA_SS1__MSK_HOL_CNT_CHA_SS1                                                      0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_CHA_SS1_SHFT_HOL_CNT_CHA_SS1                                                      0
// ACCESS__HOL_CNT_CHA_SS1: r,w
// DESCR__HOL_CNT_CHA_SS1: Die Anzahl der für das NRT-API CHA an SS1 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_CHA_SS1_RST__VAL                            0x00000000


#define IRTE_R7_REG_LIST_STATUS_CHA                                                                         0x00016438
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_CHA__MSK_LIST_STATUS                                                            0x000003FF
#define IRTE_R7_REG_LIST_STATUS_CHA_SHFT_LIST_STATUS                                                            0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am NRT-API-CHA. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_CHA_RST__VAL                              0x00000000


#define IRTE_R7_REG_HOL_COUNT_CHB_SS0                                                                       0x0001643C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_CHB_SS0__MSK_HOL_CNT_CHB_SS0                                                      0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_CHB_SS0_SHFT_HOL_CNT_CHB_SS0                                                      0
// ACCESS__HOL_CNT_CHB_SS0: r,w
// DESCR__HOL_CNT_CHB_SS0: Die Anzahl der für das NRT-API CHB an SS0 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_CHB_SS0_RST__VAL                            0x00000000


#define IRTE_R7_REG_HOL_COUNT_CHB_SS1                                                                       0x00016440
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_CHB_SS1__MSK_HOL_CNT_CHB_SS1                                                      0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_CHB_SS1_SHFT_HOL_CNT_CHB_SS1                                                      0
// ACCESS__HOL_CNT_CHB_SS1: r,w
// DESCR__HOL_CNT_CHB_SS1: Die Anzahl der für das NRT-API CHB an SS1 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_CHB_SS1_RST__VAL                            0x00000000


#define IRTE_R7_REG_LIST_STATUS_CHB                                                                         0x00016444
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_CHB__MSK_LIST_STATUS                                                            0x000003FF
#define IRTE_R7_REG_LIST_STATUS_CHB_SHFT_LIST_STATUS                                                            0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am NRT-API-CHB. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_CHB_RST__VAL                              0x00000000


#define IRTE_R7_REG_HOL_COUNT_P0                                                                            0x00016448
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_P0__MSK_HOL_CNT_                                                                  0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_P0_SHFT_HOL_CNT_                                                                  0
// ACCESS__HOL_CNT_: r,w
// DESCR__HOL_CNT_: Die Anzahl der für Port 0 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_P0_RST__VAL                                 0x00000000


#define IRTE_R7_REG_LIST_STATUS_P0                                                                          0x0001644C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_P0__MSK_LIST_STATUS                                                             0x000003FF
#define IRTE_R7_REG_LIST_STATUS_P0_SHFT_LIST_STATUS                                                             0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am Port 0. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_P0_RST__VAL                               0x00000000


#define IRTE_R7_REG_HOL_COUNT_P1                                                                            0x00016450
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_P1__MSK_HOL_CNT_                                                                  0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_P1_SHFT_HOL_CNT_                                                                  0
// ACCESS__HOL_CNT_: r,w
// DESCR__HOL_CNT_: Die Anzahl der für Port 1 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_P1_RST__VAL                                 0x00000000


#define IRTE_R7_REG_LIST_STATUS_P1                                                                          0x00016454
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_P1__MSK_LIST_STATUS                                                             0x000003FF
#define IRTE_R7_REG_LIST_STATUS_P1_SHFT_LIST_STATUS                                                             0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am Port 1. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_P1_RST__VAL                               0x00000000


#define IRTE_R7_REG_HOL_COUNT_P2                                                                            0x00016458
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_P2__MSK_HOL_CNT_                                                                  0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_P2_SHFT_HOL_CNT_                                                                  0
// ACCESS__HOL_CNT_: r,w
// DESCR__HOL_CNT_: Die Anzahl der für Port 2 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_P2_RST__VAL                                 0x00000000


#define IRTE_R7_REG_LIST_STATUS_P2                                                                          0x0001645C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_P2__MSK_LIST_STATUS                                                             0x000003FF
#define IRTE_R7_REG_LIST_STATUS_P2_SHFT_LIST_STATUS                                                             0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am Port 2. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_P2_RST__VAL                               0x00000000


#define IRTE_R7_REG_HOL_COUNT_P3                                                                            0x00016460
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_HOL_COUNT_P3__MSK_HOL_CNT_                                                                  0x00000FFF
#define IRTE_R7_REG_HOL_COUNT_P3_SHFT_HOL_CNT_                                                                  0
// ACCESS__HOL_CNT_: r,w
// DESCR__HOL_CNT_: Die Anzahl der für Port 3 in Gebrauch befindlichen Datenpuffer
#define IRTE_R7_REG_HOL_COUNT_P3_RST__VAL                                 0x00000000


#define IRTE_R7_REG_LIST_STATUS_P3                                                                          0x00016464
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_LIST_STATUS_P3__MSK_LIST_STATUS                                                             0x000003FF
#define IRTE_R7_REG_LIST_STATUS_P3_SHFT_LIST_STATUS                                                             0
// ACCESS__LIST_STATUS: r,w
// DESCR__LIST_STATUS: Füllstand der NRT-Sendeliste am Port 3. Jedem Bit ist dabei eine Priorität der Liste zugeordnet.‚0‘: keine Eintrag in der jeweiligen Warteschlange‚1‘: Eintrag / Einträge in der jeweiligen Warteschlange
#define IRTE_R7_REG_LIST_STATUS_P3_RST__VAL                               0x00000000


#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST                                                     0x0001741C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_NO_TABLE_ENTRY                                 0x00400000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_NO_TABLE_ENTRY                                 22
// ACCESS__INT_NO_TABLE_ENTRY: r,w
// DESCR__INT_NO_TABLE_ENTRY: Die Sourceadresse konnte in die Adresstabelle nicht eingetragen werden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_NRT_DB_EMPTY                                       0x00200000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_NRT_DB_EMPTY                                       21
// ACCESS__NRT_DB_EMPTY: r,w
// DESCR__NRT_DB_EMPTY: Es sind keine Elemente in der NRT-DB-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_NRT_FCW_EMPTY                                      0x00100000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_NRT_FCW_EMPTY                                      20
// ACCESS__NRT_FCW_EMPTY: r,w
// DESCR__NRT_FCW_EMPTY: Es sind keine Elemente in der NRT-FCW-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_SRT_FCW_EMPTY                                      0x00080000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_SRT_FCW_EMPTY                                      19
// ACCESS__SRT_FCW_EMPTY: r,w
// DESCR__SRT_FCW_EMPTY: Es sind keine Elemente in der SRT-FCW-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_UNDERFLOW_NRT                                      0x00040000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_UNDERFLOW_NRT                                      18
// ACCESS__UNDERFLOW_NRT: r,w
// DESCR__UNDERFLOW_NRT: Die Elemente der NRT-FCW- oder NRT-DB-Leerliste fallen unter die parametrierte Schwelle
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_OVERFLOW_HOL                                       0x00020000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_OVERFLOW_HOL                                       17
// ACCESS__OVERFLOW_HOL: r,w
// DESCR__OVERFLOW_HOL: Die HOL-Schwelle eines Ports bzw. Kanals hat zugeschlagen.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INSTRUCTION_DONE                                   0x00010000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INSTRUCTION_DONE                                   16
// ACCESS__INSTRUCTION_DONE: r,w
// DESCR__INSTRUCTION_DONE: Der Auftrag an der Kommando-Schnittstelle wurde ausgeführt.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_TIME_STAMP                                     0x00008000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_TIME_STAMP                                     15
// ACCESS__INT_TIME_STAMP: r,w
// DESCR__INT_TIME_STAMP: Abspeichern des Sendezeitpunktes beim Versenden
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_HOST_ACCESS_ERROR                                  0x00004000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_HOST_ACCESS_ERROR                                  14
// ACCESS__HOST_ACCESS_ERROR: r,w
// DESCR__HOST_ACCESS_ERROR: Zugriffsverletzung am AHB / APB
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_SP_INT                                             0x00002000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_SP_INT                                             13
// ACCESS__SP_INT: r,w
// DESCR__SP_INT: Dieser Interrupt wird durch einen Schreibzugriff auf das Register Activate-SP-Interrupt ausgelöst.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_HP_INT                                             0x00001000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_HP_INT                                             12
// ACCESS__HP_INT: r,w
// DESCR__HP_INT: Dieser Interrupt wird durch einen Schreibzugriff auf das Register Activate-HP-Interrupt ausgelöst.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_CLOCKCNT_WRAP                                  0x00000800
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_CLOCKCNT_WRAP                                  11
// ACCESS__INT_CLOCKCNT_WRAP: r,w
// DESCR__INT_CLOCKCNT_WRAP: Interrupt beim Umbruch/Überlauf des Clock-Counters
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_TRIGGER_UNIT                                   0x00000400
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_TRIGGER_UNIT                                   10
// ACCESS__INT_TRIGGER_UNIT: r,w
// DESCR__INT_TRIGGER_UNIT: Monitor-Betrieb: bei K-Bus- / SC-Bus-FehlerTrigger-Betrieb: Triggerbedingung erfüllt
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_LINK_STATUS_CHANGE                                 0x00000200
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_LINK_STATUS_CHANGE                                 9
// ACCESS__LINK_STATUS_CHANGE: r,w
// DESCR__LINK_STATUS_CHANGE: An einem Port hat sich der Link Status geändert
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_FATAL_NRTLIST_ERR                              0x00000100
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_FATAL_NRTLIST_ERR                              8
// ACCESS__INT_FATAL_NRTLIST_ERR: r,w
// DESCR__INT_FATAL_NRTLIST_ERR: In einem DMACW oder einer NRT-Listenstruktur ist ein fundamentaler Parametrierungsfehler (Länge, Adresse) aufgetreten
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_RCV_CHB1                                       0x00000080
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_RCV_CHB1                                       7
// ACCESS__INT_RCV_CHB1: r,w
// DESCR__INT_RCV_CHB1: Die Rcv-DMACW-Kette von CHB SS1 ist fertig bearbeitet bzw. die Sendeliste von CHB SS1 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_SND_CHB1                                       0x00000040
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_SND_CHB1                                       6
// ACCESS__INT_SND_CHB1: r,w
// DESCR__INT_SND_CHB1: Ein Snd-DMACW von CHB SS1 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_RCV_CHB0                                       0x00000020
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_RCV_CHB0                                       5
// ACCESS__INT_RCV_CHB0: r,w
// DESCR__INT_RCV_CHB0: Die Rcv-DMACW-Kette von CHB SS0 ist fertig bearbeitet bzw. die Sendeliste von CHB SS0 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_SND_CHB0                                       0x00000010
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_SND_CHB0                                       4
// ACCESS__INT_SND_CHB0: r,w
// DESCR__INT_SND_CHB0: Ein Snd-DMACW von CHB SS0 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_RCV_CHA1                                       0x00000008
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_RCV_CHA1                                       3
// ACCESS__INT_RCV_CHA1: r,w
// DESCR__INT_RCV_CHA1: Die Rcv-DMACW-Kette von CHA SS1 ist fertig bearbeitet bzw. die Sendeliste von CHA SS1 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_SND_CHA1                                       0x00000004
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_SND_CHA1                                       2
// ACCESS__INT_SND_CHA1: r,w
// DESCR__INT_SND_CHA1: Ein Snd-DMACW von CHA SS1 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_RCV_CHA0                                       0x00000002
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_RCV_CHA0                                       1
// ACCESS__INT_RCV_CHA0: r,w
// DESCR__INT_RCV_CHA0: Die Rcv-DMACW-Kette von CHA SS0 ist fertig bearbeitet bzw. die Sendeliste von CHA SS0 ist leer,
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST__MSK_INT_SND_CHA0                                       0x00000001
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_INT_SND_CHA0                                       0
// ACCESS__INT_SND_CHA0: r,w
// DESCR__INT_SND_CHA0: Ein Snd-DMACW von CHA SS0 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_RST__VAL          0x00000000


#define IRTE_R7_REG_EOI_IRQ0_REGISTER_HOST                                                                  0x00017420
// ACCESS__: ,w
// DESCR__: 
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_HOST__MSK_W                                                               0x0000FFFF
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_HOST_SHFT_W                                                               0
// ACCESS__W: ,w
// DESCR__W: Legt die Dauer der Deaktivierung des Interruptausgangs beim Schreiben auf dieses EOI-Register als Anzahl von 25MHz/50MHz Takten (FPGA/ASIC) fest. Muß zur Deaktivierung jedes Interrupts geschrieben werden. Inaktivzeit  = (w+1)*20 ns
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_HOST_RST__VAL                       0x00000000


#define IRTE_R7_REG_EOI_IRQ1_REGISTER_HOST                                                                  0x00017424
// ACCESS__: ,w
// DESCR__: 
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_HOST__MSK_W                                                               0x0000FFFF
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_HOST_SHFT_W                                                               0
// ACCESS__W: ,w
// DESCR__W: Legt die Dauer der Deaktivierung des Interruptausgangs beim Schreiben auf dieses EOI-Register als Anzahl von 25MHz/50MHz Takten (FPGA/ASIC) fest. Muß zur Deaktivierung jedes Interrupts geschrieben werden. Inaktivzeit  = (w+1)*20 ns
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_HOST_RST__VAL                       0x00000000


#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH                                                   0x00017444
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_NO_TABLE_ENTRY                               0x00400000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_NO_TABLE_ENTRY                               22
// ACCESS__INT_NO_TABLE_ENTRY: r,w
// DESCR__INT_NO_TABLE_ENTRY: Die Sourceadresse konnte in die Adresstabelle nicht eingetragen werden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_NRT_DB_EMPTY                                     0x00200000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_NRT_DB_EMPTY                                     21
// ACCESS__NRT_DB_EMPTY: r,w
// DESCR__NRT_DB_EMPTY: Es sind keine Elemente in der NRT-DB-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_NRT_FCW_EMPTY                                    0x00100000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_NRT_FCW_EMPTY                                    20
// ACCESS__NRT_FCW_EMPTY: r,w
// DESCR__NRT_FCW_EMPTY: Es sind keine Elemente in der NRT-FCW-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_SRT_FCW_EMPTY                                    0x00080000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_SRT_FCW_EMPTY                                    19
// ACCESS__SRT_FCW_EMPTY: r,w
// DESCR__SRT_FCW_EMPTY: Es sind keine Elemente in der SRT-FCW-Leerliste mehr vorhanden.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_UNDERFLOW_NRT                                    0x00040000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_UNDERFLOW_NRT                                    18
// ACCESS__UNDERFLOW_NRT: r,w
// DESCR__UNDERFLOW_NRT: Die Elemente der NRT-FCW- oder NRT-DB-Leerliste fallen unter die parametrierte Schwelle
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_OVERFLOW_HOL                                     0x00020000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_OVERFLOW_HOL                                     17
// ACCESS__OVERFLOW_HOL: r,w
// DESCR__OVERFLOW_HOL: Die HOL-Schwelle eines Ports bzw. Kanals hat zugeschlagen.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INSTRUCTION_DONE                                 0x00010000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INSTRUCTION_DONE                                 16
// ACCESS__INSTRUCTION_DONE: r,w
// DESCR__INSTRUCTION_DONE: Der Auftrag an der Kommando-Schnittstelle wurde ausgeführt.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_TIME_STAMP                                   0x00008000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_TIME_STAMP                                   15
// ACCESS__INT_TIME_STAMP: r,w
// DESCR__INT_TIME_STAMP: Abspeichern des Sendezeitpunktes beim Versenden
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_NOT                                              0x00004000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_NOT                                              14
// ACCESS__NOT: r,w
// DESCR__NOT: 
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_SP_INT                                           0x00002000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_SP_INT                                           13
// ACCESS__SP_INT: r,w
// DESCR__SP_INT: Dieser Interrupt wird durch einen Schreibzugriff auf das Register Activate-SP-Interrupt ausgelöst.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_HP_INT                                           0x00001000
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_HP_INT                                           12
// ACCESS__HP_INT: r,w
// DESCR__HP_INT: Dieser Interrupt wird durch einen Schreibzugriff auf das Register Activate-HP-Interrupt ausgelöst.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_CLOCKCNT_WRAP                                0x00000800
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_CLOCKCNT_WRAP                                11
// ACCESS__INT_CLOCKCNT_WRAP: r,w
// DESCR__INT_CLOCKCNT_WRAP: Interrupt beim Umbruch/Überlauf des Clock-Counters
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_TRIGGER_UNIT                                 0x00000400
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_TRIGGER_UNIT                                 10
// ACCESS__INT_TRIGGER_UNIT: r,w
// DESCR__INT_TRIGGER_UNIT: Monitor-Betrieb: bei K-Bus- / SC-Bus-FehlerTrigger-Betrieb: Triggerbedingung erfüllt
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_LINK_STATUS_CHANGE                               0x00000200
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_LINK_STATUS_CHANGE                               9
// ACCESS__LINK_STATUS_CHANGE: r,w
// DESCR__LINK_STATUS_CHANGE: An einem Port hat sich der Link Status geändert
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_FATAL_NRTLIST_ERR                            0x00000100
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_FATAL_NRTLIST_ERR                            8
// ACCESS__INT_FATAL_NRTLIST_ERR: r,w
// DESCR__INT_FATAL_NRTLIST_ERR: In einem DMACW oder einer NRT-Listenstruktur ist ein fundamentaler Parametrierungsfehler (Länge, Adresse) aufgetreten
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_RCV_CHB1                                     0x00000080
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_RCV_CHB1                                     7
// ACCESS__INT_RCV_CHB1: r,w
// DESCR__INT_RCV_CHB1: Die Rcv-DMACW-Kette von CHB SS1 ist fertig bearbeitet bzw. die Sendeliste von CHB SS1 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_SND_CHB1                                     0x00000040
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_SND_CHB1                                     6
// ACCESS__INT_SND_CHB1: r,w
// DESCR__INT_SND_CHB1: Ein Snd-DMACW von CHB SS1 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_RCV_CHB0                                     0x00000020
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_RCV_CHB0                                     5
// ACCESS__INT_RCV_CHB0: r,w
// DESCR__INT_RCV_CHB0: Die Rcv-DMACW-Kette von CHB SS0 ist fertig bearbeitet bzw. die Sendeliste von CHB SS0 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_SND_CHB0                                     0x00000010
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_SND_CHB0                                     4
// ACCESS__INT_SND_CHB0: r,w
// DESCR__INT_SND_CHB0: Ein Snd-DMACW von CHB SS0 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_RCV_CHA1                                     0x00000008
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_RCV_CHA1                                     3
// ACCESS__INT_RCV_CHA1: r,w
// DESCR__INT_RCV_CHA1: Die Rcv-DMACW-Kette von CHA SS1 ist fertig bearbeitet bzw. die Sendeliste von CHA SS1 ist leer.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_SND_CHA1                                     0x00000004
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_SND_CHA1                                     2
// ACCESS__INT_SND_CHA1: r,w
// DESCR__INT_SND_CHA1: Ein Snd-DMACW von CHA SS1 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_RCV_CHA0                                     0x00000002
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_RCV_CHA0                                     1
// ACCESS__INT_RCV_CHA0: r,w
// DESCR__INT_RCV_CHA0: Die Rcv-DMACW-Kette von CHA SS0 ist fertig bearbeitet bzw. die Sendeliste von CHA SS0 ist leer,
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH__MSK_INT_SND_CHA0                                     0x00000001
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_INT_SND_CHA0                                     0
// ACCESS__INT_SND_CHA0: r,w
// DESCR__INT_SND_CHA0: Ein Snd-DMACW von CHA SS0 ist fertig bearbeitet.
#define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_RST__VAL        0x00000000


#define IRTE_R7_REG_EOI_IRQ0_REGISTER_SWITCH                                                                0x00017448
// ACCESS__: ,w
// DESCR__: 
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_SWITCH__MSK_W                                                             0x0000FFFF
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_SWITCH_SHFT_W                                                             0
// ACCESS__W: ,w
// DESCR__W: Legt die Dauer der Deaktivierung des Interruptausgangs beim Schreiben auf dieses EOI-Register als Anzahl von 25MHz/50MHz Takten (FPGA/ASIC) fest. Muß zur Deaktivierung jedes Interrupts geschrieben werden. Inaktivzeit  = (w+1)*20 ns
#define IRTE_R7_REG_EOI_IRQ0_REGISTER_SWITCH_RST__VAL                     0x00000000


#define IRTE_R7_REG_EOI_IRQ1_REGISTER_SWITCH                                                                0x0001744C
// ACCESS__: ,w
// DESCR__: 
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_SWITCH__MSK_W                                                             0x0000FFFF
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_SWITCH_SHFT_W                                                             0
// ACCESS__W: ,w
// DESCR__W: Legt die Dauer der Deaktivierung des Interruptausgangs beim Schreiben auf dieses EOI-Register als Anzahl von 25MHz/50MHz Takten (FPGA/ASIC) fest. Muß zur Deaktivierung jedes Interrupts geschrieben werden. Inaktivzeit  = (w+1)*20 ns
#define IRTE_R7_REG_EOI_IRQ1_REGISTER_SWITCH_RST__VAL                     0x00000000


#define IRTE_R7_REG_IRT_FATAL_ERROR                                                                         0x00017458
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_FATAL_ERROR__MSK_P3                                                                     0x000F0000
#define IRTE_R7_REG_IRT_FATAL_ERROR_SHFT_P3                                                                     16
// ACCESS__P3: r,w
// DESCR__P3: dto.
#define IRTE_R7_REG_IRT_FATAL_ERROR__MSK_P2                                                                     0x0000F000
#define IRTE_R7_REG_IRT_FATAL_ERROR_SHFT_P2                                                                     12
// ACCESS__P2: r,w
// DESCR__P2: dto.
#define IRTE_R7_REG_IRT_FATAL_ERROR__MSK_P1                                                                     0x00000F00
#define IRTE_R7_REG_IRT_FATAL_ERROR_SHFT_P1                                                                     8
// ACCESS__P1: r,w
// DESCR__P1: dto.
#define IRTE_R7_REG_IRT_FATAL_ERROR__MSK_P0                                                                     0x000000F0
#define IRTE_R7_REG_IRT_FATAL_ERROR_SHFT_P0                                                                     4
// ACCESS__P0: r,w
// DESCR__P0: Bit 4: IRT-Sende-FCW-Elementen oder den zugehörigen Datenpuffer-Strukturen\nBit 5: IRT-Empfangs-FCW-Elementen oder den zugehörigen Datenpuffer-Strukturen\nBit 6: ACW-Empfangs-Elementen oder den zugehörigen Datenpuffer-Strukturen\nBit 7: Remote-/Producer-Para
#define IRTE_R7_REG_IRT_FATAL_ERROR__MSK_ALLGEMEIN                                                              0x0000000F
#define IRTE_R7_REG_IRT_FATAL_ERROR_SHFT_ALLGEMEIN                                                              0
// ACCESS__ALLGEMEIN: r,w
// DESCR__ALLGEMEIN: Bit 3: Strukturen der zyklischen gepufferten und Wechselpuffer-Schnittstelle\nBit 2: ACW-Sende-Elementen bzw. den zugehörigen Datenpuffer-Strukturen oder SRT-FCW-Strukturen\nBit 1: Strukturen der Remote-/Producer-Überwachung\nBit 0: Trace-Strukturen
#define IRTE_R7_REG_IRT_FATAL_ERROR_RST__VAL                              0x00000000


#define IRTE_R7_REG_NRT_FATAL_ERROR                                                                         0x0001745C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_FATAL_ERROR__MSK_P3                                                                     0x00000080
#define IRTE_R7_REG_NRT_FATAL_ERROR_SHFT_P3                                                                     7
// ACCESS__P3: r,w
// DESCR__P3: dto.
#define IRTE_R7_REG_NRT_FATAL_ERROR__MSK_P2                                                                     0x00000040
#define IRTE_R7_REG_NRT_FATAL_ERROR_SHFT_P2                                                                     6
// ACCESS__P2: r,w
// DESCR__P2: dto.
#define IRTE_R7_REG_NRT_FATAL_ERROR__MSK_P1                                                                     0x00000020
#define IRTE_R7_REG_NRT_FATAL_ERROR_SHFT_P1                                                                     5
// ACCESS__P1: r,w
// DESCR__P1: dto.
#define IRTE_R7_REG_NRT_FATAL_ERROR__MSK_P0                                                                     0x00000010
#define IRTE_R7_REG_NRT_FATAL_ERROR_SHFT_P0                                                                     4
// ACCESS__P0: r,w
// DESCR__P0: NRT-FCW- bzw. NRT-DB-Strukturen (Leerlisten, Filterliste, Sendelisten, TM-Liste)
#define IRTE_R7_REG_NRT_FATAL_ERROR__MSK_ALLGEMEIN                                                              0x0000000F
#define IRTE_R7_REG_NRT_FATAL_ERROR_SHFT_ALLGEMEIN                                                              0
// ACCESS__ALLGEMEIN: r,w
// DESCR__ALLGEMEIN: Bit 3: ARP- bzw. DCP-Filter-Strukturen\nBit 2: NRT-FCW- bzw. NRT-DB-Strukturen am NRT-API (Leerlisten, Sendelisten, TM-Liste)\nBit 1: DMACW-Strukturen des Sende-APIs\nBit 0: DMACW-Strukturen des Empfangs-APIs
#define IRTE_R7_REG_NRT_FATAL_ERROR_RST__VAL                              0x00000000


#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_31_0                                                                 0x0001800C
// ACCESS__: ,
// DESCR__: 
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_31_0__MSK_DEBUG_ADDRESS_VALUE_1                                          0xFFFFFFFF
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_31_0_SHFT_DEBUG_ADDRESS_VALUE_1                                          0
// ACCESS__DEBUG_ADDRESS_VALUE_1: ,
// DESCR__DEBUG_ADDRESS_VALUE_1: SA bzw. DA auf die verglichen werden soll
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_31_0_RST__VAL                      0x00000000


#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_47_32                                                                0x00018010
// ACCESS__: ,
// DESCR__: 
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_47_32__MSK_DEBUG_ADDRESS_VALUE_1                                         0x0000FFFF
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_47_32_SHFT_DEBUG_ADDRESS_VALUE_1                                         0
// ACCESS__DEBUG_ADDRESS_VALUE_1: ,
// DESCR__DEBUG_ADDRESS_VALUE_1: SA bzw. DA auf die verglichen werden soll
#define IRTE_R7_REG_FM_ADDRESS_VALUE_1_47_32_RST__VAL                     0x00000000


#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_31_0                                                                 0x00018014
// ACCESS__: ,
// DESCR__: 
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_31_0__MSK_DEBUG_ADDRESS_VALUE_2                                          0xFFFFFFFF
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_31_0_SHFT_DEBUG_ADDRESS_VALUE_2                                          0
// ACCESS__DEBUG_ADDRESS_VALUE_2: ,
// DESCR__DEBUG_ADDRESS_VALUE_2: SA bzw. DA auf die verglichen werden soll
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_31_0_RST__VAL                      0x00000000


#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_47_32                                                                0x00018018
// ACCESS__: ,
// DESCR__: 
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_47_32__MSK_DEBUG_ADDRESS_VALUE_2                                         0x0000FFFF
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_47_32_SHFT_DEBUG_ADDRESS_VALUE_2                                         0
// ACCESS__DEBUG_ADDRESS_VALUE_2: ,
// DESCR__DEBUG_ADDRESS_VALUE_2: SA bzw. DA auf die verglichen werden soll
#define IRTE_R7_REG_FM_ADDRESS_VALUE_2_47_32_RST__VAL                     0x00000000


#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_1                                                                   0x0001801C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_1__MSK_DEBUG_OFFSET_DATAFIELD_1                                         0x000007FF
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_1_SHFT_DEBUG_OFFSET_DATAFIELD_1                                         0
// ACCESS__DEBUG_OFFSET_DATAFIELD_1: r,w
// DESCR__DEBUG_OFFSET_DATAFIELD_1: Anzahl an Datenbytes, ab der ein Datenfeld-Vergleich startet
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_1_RST__VAL                        0x00000000


#define IRTE_R7_REG_FM_MASK_DATAFIELD_1                                                                     0x00018020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_MASK_DATAFIELD_1__MSK_DEBUG_MASK_DATAFIELD_1                                             0xFFFFFFFF
#define IRTE_R7_REG_FM_MASK_DATAFIELD_1_SHFT_DEBUG_MASK_DATAFIELD_1                                             0
// ACCESS__DEBUG_MASK_DATAFIELD_1: r,w
// DESCR__DEBUG_MASK_DATAFIELD_1: Vergleichsmaske für den Datenfeld-Vergleich:Bitposition  = ‚0’: Bit ist nicht für den Vergleich relevantBitposition  = ‚1’: Bit ist für den Vergleich relevant
#define IRTE_R7_REG_FM_MASK_DATAFIELD_1_RST__VAL                          0x00000000


#define IRTE_R7_REG_FM_DATAFIELD_1                                                                          0x00018024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_DATAFIELD_1__MSK_DEBUG_DATAFIELD_1                                                       0xFFFFFFFF
#define IRTE_R7_REG_FM_DATAFIELD_1_SHFT_DEBUG_DATAFIELD_1                                                       0
// ACCESS__DEBUG_DATAFIELD_1: r,w
// DESCR__DEBUG_DATAFIELD_1: Vergleichswert für den Datenfeld-Vergleich
#define IRTE_R7_REG_FM_DATAFIELD_1_RST__VAL                               0x00000000


#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_2                                                                   0x00018028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_2__MSK_DEBUG_OFFSET_DATAFIELD_2                                         0x000007FF
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_2_SHFT_DEBUG_OFFSET_DATAFIELD_2                                         0
// ACCESS__DEBUG_OFFSET_DATAFIELD_2: r,w
// DESCR__DEBUG_OFFSET_DATAFIELD_2: Anzahl an Datenbytes, ab der ein Datenfeld-Vergleich startet
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_2_RST__VAL                        0x00000000


#define IRTE_R7_REG_FM_MASK_DATAFIELD_2                                                                     0x0001802C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_MASK_DATAFIELD_2__MSK_DEBUG_MASK_DATAFIELD_2                                             0xFFFFFFFF
#define IRTE_R7_REG_FM_MASK_DATAFIELD_2_SHFT_DEBUG_MASK_DATAFIELD_2                                             0
// ACCESS__DEBUG_MASK_DATAFIELD_2: r,w
// DESCR__DEBUG_MASK_DATAFIELD_2: Vergleichsmaske für den Datenfeld-Vergleich:Bitposition  = ‚0’: Bit ist nicht für den Vergleich relevantBitposition  = ‚1’: Bit ist für den Vergleich relevant
#define IRTE_R7_REG_FM_MASK_DATAFIELD_2_RST__VAL                          0x00000000


#define IRTE_R7_REG_FM_DATAFIELD_2                                                                          0x00018030
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_DATAFIELD_2__MSK_DEBUG_DATAFIELD_2                                                       0xFFFFFFFF
#define IRTE_R7_REG_FM_DATAFIELD_2_SHFT_DEBUG_DATAFIELD_2                                                       0
// ACCESS__DEBUG_DATAFIELD_2: r,w
// DESCR__DEBUG_DATAFIELD_2: Vergleichswert für den Datenfeld-Vergleich
#define IRTE_R7_REG_FM_DATAFIELD_2_RST__VAL                               0x00000000


#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_3                                                                   0x00018034
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_3__MSK_DEBUG_OFFSET_DATAFIELD_3                                         0x000007FF
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_3_SHFT_DEBUG_OFFSET_DATAFIELD_3                                         0
// ACCESS__DEBUG_OFFSET_DATAFIELD_3: r,w
// DESCR__DEBUG_OFFSET_DATAFIELD_3: Anzahl an Datenbytes, ab der ein Datenfeld-Vergleich startet
#define IRTE_R7_REG_FM_OFFSET_DATAFIELD_3_RST__VAL                        0x00000000


#define IRTE_R7_REG_FM_MASK_DATAFIELD_3                                                                     0x00018038
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_MASK_DATAFIELD_3__MSK_DEBUG_MASK_DATAFIELD_3                                             0xFFFFFFFF
#define IRTE_R7_REG_FM_MASK_DATAFIELD_3_SHFT_DEBUG_MASK_DATAFIELD_3                                             0
// ACCESS__DEBUG_MASK_DATAFIELD_3: r,w
// DESCR__DEBUG_MASK_DATAFIELD_3: Vergleichsmaske für den Datenfeld-Vergleich:Bitposition  = ‚0’: Bit ist nicht für den Vergleich relevantBitposition  = ‚1’: Bit ist für den Vergleich relevant
#define IRTE_R7_REG_FM_MASK_DATAFIELD_3_RST__VAL                          0x00000000


#define IRTE_R7_REG_FM_DATAFIELD_3                                                                          0x0001803C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_DATAFIELD_3__MSK_DEBUG_DATAFIELD_3                                                       0xFFFFFFFF
#define IRTE_R7_REG_FM_DATAFIELD_3_SHFT_DEBUG_DATAFIELD_3                                                       0
// ACCESS__DEBUG_DATAFIELD_3: r,w
// DESCR__DEBUG_DATAFIELD_3: Vergleichswert für den Datenfeld-Vergleich
#define IRTE_R7_REG_FM_DATAFIELD_3_RST__VAL                               0x00000000


#define IRTE_R7_REG_FM_COUNT                                                                                0x00018040
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_COUNT__MSK_D_T                                                                           0x000000C0
#define IRTE_R7_REG_FM_COUNT_SHFT_D_T                                                                           6
// ACCESS__D_T: r,w
// DESCR__D_T: Datafield-Trigger-Count: = 00: negativer Trigger, Adress-Triggerung, wenn die empfangenen Datenfelder ungleich den relevanten Datafield-Komparatoren sind.? 00: Adress-Triggerung, wenn die Trefferanzahl beim Vergleich der empfangenen Datenfelder mit den relevanten Datafield-Komparatoren größer gleich dem Wert in Datafield_Trigger_Count ist.
#define IRTE_R7_REG_FM_COUNT__MSK_A_T                                                                           0x00000030
#define IRTE_R7_REG_FM_COUNT_SHFT_A_T                                                                           4
// ACCESS__A_T: r,w
// DESCR__A_T: Address-Trigger-Count:00: negativer Trigger, Adress-Triggerung, wenn die empfangene DA und SA ungleich den relevanten Adress-Komparatoren ist.(01 OR 10): Adress-Triggerung, wenn die Trefferanzahl beim Vergleich der und SA mit den relevanten Adress-Komparatoren größer gleich dem Wert in Address_Trigger_Count ist.11: Kein Monitoring, bei selektierten Adress-Komparatoren.
#define IRTE_R7_REG_FM_COUNT__MSK_D_S                                                                           0x0000000C
#define IRTE_R7_REG_FM_COUNT_SHFT_D_S                                                                           2
// ACCESS__D_S: r,w
// DESCR__D_S: Datafield-Selector-Count: = 00: negativer Selektor, Monitoring, wenn die empfangenen Datenfelder ungleich den selektierten Datafield-Komparatoren sind.? 00: Monitoring, wenn die Trefferanzahl beim Vergleich der empfangenen Datenfelder mit den selektierten Datafield-Komparatoren größer gleich dem Wert in Datafield_Selector_Count ist.
#define IRTE_R7_REG_FM_COUNT__MSK_A_S                                                                           0x00000003
#define IRTE_R7_REG_FM_COUNT_SHFT_A_S                                                                           0
// ACCESS__A_S: r,w
// DESCR__A_S: Address-Selector-Count:00:  negativer Selektor, Monitoring, wenn die empfangene DA und SA  ungleich den selektierten Adress-Komparatoren ist. (01 OR 10):  Monitoring, wenn die Treffer-anzahl beim Vergleich der empfangenen DA und SA mit den selektierten Adress-Komparatoren größer gleich dem Wert in Address_Selector_Count ist.11: Kein Monitoring, bei selektierten Adress-Komparatoren.
#define IRTE_R7_REG_FM_COUNT_RST__VAL                                     0x00000000


#define IRTE_R7_REG_FM_DELTA_TIME                                                                           0x00018044
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_DELTA_TIME__MSK_DEBUG_DELTA_TIME                                                         0x001FFFFF
#define IRTE_R7_REG_FM_DELTA_TIME_SHFT_DEBUG_DELTA_TIME                                                         0
// ACCESS__DEBUG_DELTA_TIME: r,w
// DESCR__DEBUG_DELTA_TIME: Max. Zeitfenster zweier aufeinander folgender Telegramme, die den Filter-Trigger passieren. Der Wert ist in 2,048µs Granularität einstellbar.
#define IRTE_R7_REG_FM_DELTA_TIME_RST__VAL                                0x00000000


#define IRTE_R7_REG_FM_SELECTOR_ENABLE                                                                      0x00018048
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_SELECTOR_ENABLE__MSK_DATAFIELD_3                                                         0x00000010
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_SHFT_DATAFIELD_3                                                         4
// ACCESS__DATAFIELD_3: r,w
// DESCR__DATAFIELD_3: Freigabe von Datafield_3 für die Selektor-Auswertung
#define IRTE_R7_REG_FM_SELECTOR_ENABLE__MSK_DATAFIELD_2                                                         0x00000008
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_SHFT_DATAFIELD_2                                                         3
// ACCESS__DATAFIELD_2: r,w
// DESCR__DATAFIELD_2: Freigabe von Datafield_2 für die Selektor-Auswertung
#define IRTE_R7_REG_FM_SELECTOR_ENABLE__MSK_DATAFIELD_1                                                         0x00000004
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_SHFT_DATAFIELD_1                                                         2
// ACCESS__DATAFIELD_1: r,w
// DESCR__DATAFIELD_1: Freigabe von Datafield_1 für die Selektor-Auswertung
#define IRTE_R7_REG_FM_SELECTOR_ENABLE__MSK_ADRESS_VALUE_2                                                      0x00000002
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_SHFT_ADRESS_VALUE_2                                                      1
// ACCESS__ADRESS_VALUE_2: r,w
// DESCR__ADRESS_VALUE_2: Freigabe von Adress_Value_2 für die Selektor-Auswertung
#define IRTE_R7_REG_FM_SELECTOR_ENABLE__MSK_ADRESS_VALUE_1                                                      0x00000001
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_SHFT_ADRESS_VALUE_1                                                      0
// ACCESS__ADRESS_VALUE_1: r,w
// DESCR__ADRESS_VALUE_1: Freigabe von Adress_Value_1 für die Selektor-Auswertung
#define IRTE_R7_REG_FM_SELECTOR_ENABLE_RST__VAL                           0x00000000


#define IRTE_R7_REG_FM_TRIGGER_ENABLE                                                                       0x0001804C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_TRIGGER_ENABLE__MSK_DATAFIELD_3                                                          0x00000010
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_SHFT_DATAFIELD_3                                                          4
// ACCESS__DATAFIELD_3: r,w
// DESCR__DATAFIELD_3: Freigabe von Datafield_3 für die Trigger-Auswertung
#define IRTE_R7_REG_FM_TRIGGER_ENABLE__MSK_DATAFIELD_2                                                          0x00000008
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_SHFT_DATAFIELD_2                                                          3
// ACCESS__DATAFIELD_2: r,w
// DESCR__DATAFIELD_2: Freigabe von Datafield_2 für die Trigger-Auswertung
#define IRTE_R7_REG_FM_TRIGGER_ENABLE__MSK_DATAFIELD_1                                                          0x00000004
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_SHFT_DATAFIELD_1                                                          2
// ACCESS__DATAFIELD_1: r,w
// DESCR__DATAFIELD_1: Freigabe von Datafield_1 für die Trigger-Auswertung
#define IRTE_R7_REG_FM_TRIGGER_ENABLE__MSK_ADRESS_VALUE_2                                                       0x00000002
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_SHFT_ADRESS_VALUE_2                                                       1
// ACCESS__ADRESS_VALUE_2: r,w
// DESCR__ADRESS_VALUE_2: Freigabe von Adress_Value_2 für die Trigger-Auswertung
#define IRTE_R7_REG_FM_TRIGGER_ENABLE__MSK_ADRESS_VALUE_1                                                       0x00000001
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_SHFT_ADRESS_VALUE_1                                                       0
// ACCESS__ADRESS_VALUE_1: r,w
// DESCR__ADRESS_VALUE_1: Freigabe von Adress_Value_1 für die Trigger-Auswertung
#define IRTE_R7_REG_FM_TRIGGER_ENABLE_RST__VAL                            0x00000000


#define IRTE_R7_REG_FM_MONITOR_COUNT                                                                        0x00018050
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_MONITOR_COUNT__MSK_DEBUG_MONITOR_COUNT                                                   0x0000FFFF
#define IRTE_R7_REG_FM_MONITOR_COUNT_SHFT_DEBUG_MONITOR_COUNT                                                   0
// ACCESS__DEBUG_MONITOR_COUNT: r,w
// DESCR__DEBUG_MONITOR_COUNT: Anzahl der Telegramme, die nach einem aufgetretenen Trigger in der Betriebsart Post-Trigger noch aufgezeichnet werden.
#define IRTE_R7_REG_FM_MONITOR_COUNT_RST__VAL                             0x00000000


#define IRTE_R7_REG_FM_CONTROL                                                                              0x00018054
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FM_CONTROL__MSK_ENABLE_DIAG_PX                                                              0x000000F0
#define IRTE_R7_REG_FM_CONTROL_SHFT_ENABLE_DIAG_PX                                                              4
// ACCESS__ENABLE_DIAG_PX: r,w
// DESCR__ENABLE_DIAG_PX: Freigabe des Debugging an den jeweiligen Ports
#define IRTE_R7_REG_FM_CONTROL__MSK_ENABLE_TIME_CTRL                                                            0x00000008
#define IRTE_R7_REG_FM_CONTROL_SHFT_ENABLE_TIME_CTRL                                                            3
// ACCESS__ENABLE_TIME_CTRL: r,w
// DESCR__ENABLE_TIME_CTRL: 0: Ein Trigger-Treffer liegt vor, wenn ein die Trigger-Filter(4..0) passiert hat.1: Es wird jeweils der zeitliche Abstand letzten beiden Telegramme gemessen, die die Trigger-Filter(4..0) passiert haben. Ist dieser zeitliche Abstand größer gleich als die in Delta_Time vorgegebene Zeit liegt ein Trigger-Treffer vor.
#define IRTE_R7_REG_FM_CONTROL__MSK_IGNORE_CRC_CHECK                                                            0x00000004
#define IRTE_R7_REG_FM_CONTROL_SHFT_IGNORE_CRC_CHECK                                                            2
// ACCESS__IGNORE_CRC_CHECK: r,w
// DESCR__IGNORE_CRC_CHECK: 0: Der Selektor-Filter und der Trigger-Filter lassen jeweils nur Telegramme ohne CRC-Fehler passieren.1: Der Selektor-Filter und der Trigger-Filter lassen jeweils auch Telegramme mit CRC-Fehler passieren.
#define IRTE_R7_REG_FM_CONTROL__MSK_AWARE_TAG                                                                   0x00000002
#define IRTE_R7_REG_FM_CONTROL_SHFT_AWARE_TAG                                                                   1
// ACCESS__AWARE_TAG: r,w
// DESCR__AWARE_TAG: 0: Der Offset-Nullpunkt von einem zeigt immer auf 1. Byte nach der SA. Dies gilt sowohl für ungetaggte als auch für getaggte Telegramme.1: Der Offset-Nullpunkt von einem Datafield zeigt auf        - das 1. Byte nach der SA, bei ungetaggten Telegramm        - das 1. Byte nach dem Tag, bei einem getaggten Telegramm
#define IRTE_R7_REG_FM_CONTROL__MSK_DIAG_MODE                                                                   0x00000001
#define IRTE_R7_REG_FM_CONTROL_SHFT_DIAG_MODE                                                                   0
// ACCESS__DIAG_MODE: r,w
// DESCR__DIAG_MODE: 0: BetriebsartPost-Trigger1: Betriebsart Pre-Trigger
#define IRTE_R7_REG_FM_CONTROL_RST__VAL                                   0x00000000


#define IRTE_R7_REG_MAC_DA31_0                                                                              0x00019014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAC_DA31_0__MSK_MAC_DEST_ADDRESS                                                            0xFFFFFFFF
#define IRTE_R7_REG_MAC_DA31_0_SHFT_MAC_DEST_ADDRESS                                                            0
// ACCESS__MAC_DEST_ADDRESS: r,w
// DESCR__MAC_DEST_ADDRESS: MAC-Ziel-Adresse, die als Default-Adresse beim Versenden eines Ersatztelegramms (infolge eines ausgebliebenen IRT-Telegramms) eingesetzt wird (meist eine MC-Adresse).
#define IRTE_R7_REG_MAC_DA31_0_RST__VAL                                   0x00000000


#define IRTE_R7_REG_MAC_DA47_32                                                                             0x00019018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAC_DA47_32__MSK_MAC_DEST_ADDRESS                                                           0x0000FFFF
#define IRTE_R7_REG_MAC_DA47_32_SHFT_MAC_DEST_ADDRESS                                                           0
// ACCESS__MAC_DEST_ADDRESS: r,w
// DESCR__MAC_DEST_ADDRESS: MAC-Ziel-Adresse, die als Default-Adresse beim Versenden eines Ersatztelegramms (infolge eines ausgebliebenen IRT-Telegramms) eingesetzt wird (meist eine MC-Adresse).
#define IRTE_R7_REG_MAC_DA47_32_RST__VAL                                  0x00000000


#define IRTE_R7_REG_MAC_SA31_0                                                                              0x0001901C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAC_SA31_0__MSK_SA                                                                          0xFFFFFFFF
#define IRTE_R7_REG_MAC_SA31_0_SHFT_SA                                                                          0
// ACCESS__SA: r,w
// DESCR__SA: Eigene MAC-Stationsadresse
#define IRTE_R7_REG_MAC_SA31_0_RST__VAL                                   0x00000000


#define IRTE_R7_REG_MAC_SA47_32                                                                             0x00019020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAC_SA47_32__MSK_SA                                                                         0x0000FFFF
#define IRTE_R7_REG_MAC_SA47_32_SHFT_SA                                                                         0
// ACCESS__SA: r,w
// DESCR__SA: Eigene MAC-Stationsadresse
#define IRTE_R7_REG_MAC_SA47_32_RST__VAL                                  0x00000000


#define IRTE_R7_REG_IP_ADDRESS0                                                                             0x00019024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IP_ADDRESS0__MSK_ID_ADRESSE                                                                 0xFFFFFFFF
#define IRTE_R7_REG_IP_ADDRESS0_SHFT_ID_ADRESSE                                                                 0
// ACCESS__ID_ADRESSE: r,w
// DESCR__ID_ADRESSE: IP-Adresse unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_IP_ADDRESS0_RST__VAL                                  0x00000000


#define IRTE_R7_REG_IP_ADDRESS1                                                                             0x00019028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IP_ADDRESS1__MSK_ID_ADRESSE                                                                 0xFFFFFFFF
#define IRTE_R7_REG_IP_ADDRESS1_SHFT_ID_ADRESSE                                                                 0
// ACCESS__ID_ADRESSE: r,w
// DESCR__ID_ADRESSE: IP-Adresse unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_IP_ADDRESS1_RST__VAL                                  0x00000000


#define IRTE_R7_REG_IP_ADDRESS2                                                                             0x0001902C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IP_ADDRESS2__MSK_ID_ADRESSE                                                                 0xFFFFFFFF
#define IRTE_R7_REG_IP_ADDRESS2_SHFT_ID_ADRESSE                                                                 0
// ACCESS__ID_ADRESSE: r,w
// DESCR__ID_ADRESSE: IP-Adresse unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_IP_ADDRESS2_RST__VAL                                  0x00000000


#define IRTE_R7_REG_IP_ADDRESS3                                                                             0x00019030
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IP_ADDRESS3__MSK_ID_ADRESSE                                                                 0xFFFFFFFF
#define IRTE_R7_REG_IP_ADDRESS3_SHFT_ID_ADRESSE                                                                 0
// ACCESS__ID_ADRESSE: r,w
// DESCR__ID_ADRESSE: IP-Adresse unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_IP_ADDRESS3_RST__VAL                                  0x00000000


#define IRTE_R7_REG_IP_TTL                                                                                  0x00019034
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IP_TTL__MSK_IP_TIME_TO_LIFE                                                                 0x000000FF
#define IRTE_R7_REG_IP_TTL_SHFT_IP_TIME_TO_LIFE                                                                 0
// ACCESS__IP_TIME_TO_LIFE: r,w
// DESCR__IP_TIME_TO_LIFE: Lifetime-Zähler zur Generierung eines IP-Headers
#define IRTE_R7_REG_IP_TTL_RST__VAL                                       0x00000000


#define IRTE_R7_REG_UDP_PNIO_PORT0                                                                          0x00019038
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UDP_PNIO_PORT0__MSK_UDP_PNIO_PORT                                                           0x0000FFFF
#define IRTE_R7_REG_UDP_PNIO_PORT0_SHFT_UDP_PNIO_PORT                                                           0
// ACCESS__UDP_PNIO_PORT: r,w
// DESCR__UDP_PNIO_PORT: PNIO-Port im UDP-Header (Konstante für PN-IO) unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_UDP_PNIO_PORT0_RST__VAL                               0x00000000


#define IRTE_R7_REG_UDP_PNIO_PORT1                                                                          0x0001903C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UDP_PNIO_PORT1__MSK_UDP_PNIO_PORT                                                           0x0000FFFF
#define IRTE_R7_REG_UDP_PNIO_PORT1_SHFT_UDP_PNIO_PORT                                                           0
// ACCESS__UDP_PNIO_PORT: r,w
// DESCR__UDP_PNIO_PORT: PNIO-Port im UDP-Header (Konstante für PN-IO) unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_UDP_PNIO_PORT1_RST__VAL                               0x00000000


#define IRTE_R7_REG_UDP_PNIO_PORT2                                                                          0x00019040
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UDP_PNIO_PORT2__MSK_UDP_PNIO_PORT                                                           0x0000FFFF
#define IRTE_R7_REG_UDP_PNIO_PORT2_SHFT_UDP_PNIO_PORT                                                           0
// ACCESS__UDP_PNIO_PORT: r,w
// DESCR__UDP_PNIO_PORT: PNIO-Port im UDP-Header (Konstante für PN-IO) unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_UDP_PNIO_PORT2_RST__VAL                               0x00000000


#define IRTE_R7_REG_UDP_PNIO_PORT3                                                                          0x00019044
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_UDP_PNIO_PORT3__MSK_UDP_PNIO_PORT                                                           0x0000FFFF
#define IRTE_R7_REG_UDP_PNIO_PORT3_SHFT_UDP_PNIO_PORT                                                           0
// ACCESS__UDP_PNIO_PORT: r,w
// DESCR__UDP_PNIO_PORT: PNIO-Port im UDP-Header (Konstante für PN-IO) unter der das lokale SRT-API adressiert werden kann bzw. mit der vom lokalen SRT-API eingespeist werden kann
#define IRTE_R7_REG_UDP_PNIO_PORT3_RST__VAL                               0x00000000


#define IRTE_R7_REG_IRT_CTRL_BASE                                                                           0x00019048
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IRT_CTRL_BASE__MSK_IRT_CTRL_BASE                                                            0x001FFFFF
#define IRTE_R7_REG_IRT_CTRL_BASE_SHFT_IRT_CTRL_BASE                                                            0
// ACCESS__IRT_CTRL_BASE: r,w
// DESCR__IRT_CTRL_BASE: Adresse, die im K-RAM auf den Speicherbereich zeigt, im dem die Zeiger auf das erste zu sendenden bzw. zu empfangenden IRT-FCW abgelegt sind.(zum Aufbau diese Struktur im K-RAM, siehe „IRT-Listen“)
#define IRTE_R7_REG_IRT_CTRL_BASE_RST__VAL                                0x00000000


#define IRTE_R7_REG_NRT_CTRL_BASE                                                                           0x0001904C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_CTRL_BASE__MSK_NRT_CTRL_BASIC_PTR                                                       0x001FFFFF
#define IRTE_R7_REG_NRT_CTRL_BASE_SHFT_NRT_CTRL_BASIC_PTR                                                       0
// ACCESS__NRT_CTRL_BASIC_PTR: r,w
// DESCR__NRT_CTRL_BASIC_PTR: Zeiger ins K-RAM, auf einen Bereich, in dem die Port- und NRT-API-spezifischen Zeiger auf den Beginn der priorisierten Sendeliste enthalten sind.
#define IRTE_R7_REG_NRT_CTRL_BASE_RST__VAL                                0x00000000


#define IRTE_R7_REG_FREE_CTRL_BASE                                                                          0x00019050
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FREE_CTRL_BASE__MSK_FREE_CTRL_BASIC_PTR                                                     0x001FFFFF
#define IRTE_R7_REG_FREE_CTRL_BASE_SHFT_FREE_CTRL_BASIC_PTR                                                     0
// ACCESS__FREE_CTRL_BASIC_PTR: r,w
// DESCR__FREE_CTRL_BASIC_PTR: Zeiger ins K-RAM, auf einen Bereich, in dem die Zeiger auf den Beginn der NRT-FCW-, SRT-FCW und NRT-DB-Leerlisten enthalten sind.
#define IRTE_R7_REG_FREE_CTRL_BASE_RST__VAL                               0x00000000


#define IRTE_R7_REG_ACW_LIST_BASE                                                                           0x00019054
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ACW_LIST_BASE__MSK_ACW_LIST__BASE                                                           0x001FFFFF
#define IRTE_R7_REG_ACW_LIST_BASE_SHFT_ACW_LIST__BASE                                                           0
// ACCESS__ACW_LIST__BASE: r,w
// DESCR__ACW_LIST__BASE: Basisadresse auf den Beginn der ACW-Liste
#define IRTE_R7_REG_ACW_LIST_BASE_RST__VAL                                0x00000000


#define IRTE_R7_REG_EN_ACW_OFFSET                                                                           0x00019058
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_EN_ACW_OFFSET__MSK_EN_ACW_OFFSET                                                            0x0000FFFF
#define IRTE_R7_REG_EN_ACW_OFFSET_SHFT_EN_ACW_OFFSET                                                            0
// ACCESS__EN_ACW_OFFSET: r,w
// DESCR__EN_ACW_OFFSET: Auswahl der Bits innerhalb der Frame-ID, zur Bestimmung der Offset-Adresse auf eine ACW-Empfangs-Liste.Achtung: Die Bits müssen zusammenhängend ab Bit 0 im Register liegen.
#define IRTE_R7_REG_EN_ACW_OFFSET_RST__VAL                                0x00000000


#define IRTE_R7_REG_SWITCH_CONTROL                                                                          0x0001905C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_PORT3_ENABLE                                                            0x00008000
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_PORT3_ENABLE                                                            15
// ACCESS__PORT3_ENABLE: r,w
// DESCR__PORT3_ENABLE: Freigabe von Port 3
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_PORT2_ENABLE                                                            0x00004000
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_PORT2_ENABLE                                                            14
// ACCESS__PORT2_ENABLE: r,w
// DESCR__PORT2_ENABLE: Freigabe von Port 2
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_PORT1_ENABLE                                                            0x00002000
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_PORT1_ENABLE                                                            13
// ACCESS__PORT1_ENABLE: r,w
// DESCR__PORT1_ENABLE: Freigabe von Port 1
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_PORT0_ENABLE                                                            0x00001000
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_PORT0_ENABLE                                                            12
// ACCESS__PORT0_ENABLE: r,w
// DESCR__PORT0_ENABLE: Freigabe von Port 0
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_CYCLESYNC_ENABLE                                                        0x00000800
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_CYCLESYNC_ENABLE                                                        11
// ACCESS__CYCLESYNC_ENABLE: r,w
// DESCR__CYCLESYNC_ENABLE: Freigabe der Zyklussynchronisation
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_INTRCTRL_ENABLE                                                         0x00000400
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_INTRCTRL_ENABLE                                                         10
// ACCESS__INTRCTRL_ENABLE: r,w
// DESCR__INTRCTRL_ENABLE: Freigabe des Interrupt-Controllers
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_TRIGGERUNIT_ENABLE                                                      0x00000200
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_TRIGGERUNIT_ENABLE                                                      9
// ACCESS__TRIGGERUNIT_ENABLE: r,w
// DESCR__TRIGGERUNIT_ENABLE: Freigabe der Trigger-Unit
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_NRTCTRL_ENABLE                                                          0x00000100
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_NRTCTRL_ENABLE                                                          8
// ACCESS__NRTCTRL_ENABLE: r,w
// DESCR__NRTCTRL_ENABLE: Freigabe der NRT-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_IRTCTRL_ENABLE                                                          0x00000080
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_IRTCTRL_ENABLE                                                          7
// ACCESS__IRTCTRL_ENABLE: r,w
// DESCR__IRTCTRL_ENABLE: Freigabe der IRT-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_SRTCTRL_ENABLE                                                          0x00000040
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_SRTCTRL_ENABLE                                                          6
// ACCESS__SRTCTRL_ENABLE: r,w
// DESCR__SRTCTRL_ENABLE: Freigabe der SRT-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_KRAMCTRL_ENABLE                                                         0x00000020
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_KRAMCTRL_ENABLE                                                         5
// ACCESS__KRAMCTRL_ENABLE: r,w
// DESCR__KRAMCTRL_ENABLE: Freigabe der K-RAM-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_TRCCTRL_ENABLE                                                          0x00000010
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_TRCCTRL_ENABLE                                                          4
// ACCESS__TRCCTRL_ENABLE: r,w
// DESCR__TRCCTRL_ENABLE: Freigabe der IRT-Trace-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_STATCTRL_ENABLE                                                         0x00000008
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_STATCTRL_ENABLE                                                         3
// ACCESS__STATCTRL_ENABLE: r,w
// DESCR__STATCTRL_ENABLE: Freigabe der Statistik-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_SMICTRL_ENABLE                                                          0x00000004
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_SMICTRL_ENABLE                                                          2
// ACCESS__SMICTRL_ENABLE: r,w
// DESCR__SMICTRL_ENABLE: Freigabe der PHY-/SMI-Control
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_SOFT_RESET                                                              0x00000002
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_SOFT_RESET                                                              1
// ACCESS__SOFT_RESET: r,w
// DESCR__SOFT_RESET: ‚0‘: Bei der Umschaltung in den Konfigurationsmodus (Bit 0)      werden die Parameter-Register nicht zurückgesetzt.‚1‘: Bei der Umschaltung in den Konfigurationsmodus (Bit 0)      werden alle Parameter-Register zurückgesetzt.
#define IRTE_R7_REG_SWITCH_CONTROL__MSK_SWITCH_ENABLE                                                           0x00000001
#define IRTE_R7_REG_SWITCH_CONTROL_SHFT_SWITCH_ENABLE                                                           0
// ACCESS__SWITCH_ENABLE: r,w
// DESCR__SWITCH_ENABLE: ‚0’: Switch-Makro befindet sich im Konfigurationsmodus‚1’: Switch-Makro befindet sich im Betriebsmodus
#define IRTE_R7_REG_SWITCH_CONTROL_RST__VAL                               0x00000000


#define IRTE_R7_REG_MIN_IDLE_TIME                                                                           0x00019060
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MIN_IDLE_TIME__MSK_MIN_IDLE_TIME                                                            0xFFFFFFFF
#define IRTE_R7_REG_MIN_IDLE_TIME_SHFT_MIN_IDLE_TIME                                                            0
// ACCESS__MIN_IDLE_TIME: r,w
// DESCR__MIN_IDLE_TIME: Mindestwarte-Zeit in Systemtakten, nach der das nächste Telegramm gesendet bzw. empfangen werden darf. Alle IRT-Telegramme, die innerhalb der Mindestwarte-Zeit versendet werden sollen, gehen verloren. Achtung: Bei einer Einstellung von kleineren Werten ist die Überwachung der Sendezeit bei IRT-Kommunikation nicht mehr gewährleistet25 MHz: 100MBit   Wert  = 08H10MBit   Wert  = C4H50 MHz: 100MBit   Wert  = 1DH10MBit   Wert  = 196H
#define IRTE_R7_REG_MIN_IDLE_TIME_RST__VAL                                0x00000000


#define IRTE_R7_REG_MAC_EMPTY_COUNT                                                                         0x00019064
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MAC_EMPTY_COUNT__MSK_MAC_EMPTY_COUNT                                                        0x0000FFFF
#define IRTE_R7_REG_MAC_EMPTY_COUNT_SHFT_MAC_EMPTY_COUNT                                                        0
// ACCESS__MAC_EMPTY_COUNT: r,w
// DESCR__MAC_EMPTY_COUNT: Anzahl der Systemtakte, die maximal gewartet werden müssen nachdem die MAC „Fifo-Halffull“ meldet, um selbständig durch die Sende-HW der MAC Dummy-Telegramm-daten zu übergeben. Dadurch wird ein Abriss des Telegrammstroms verhindert.25 MHz: 100MBit   Wert  = 8H10MBit   Wert  = xH50 MHz: 100MBit   Wert  = 10H10MBit   Wert  = xH
#define IRTE_R7_REG_MAC_EMPTY_COUNT_RST__VAL                              0x00000000


#define IRTE_R7_REG_NRT_TRANSFER_CONTROL                                                                    0x00019068
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_EN_NON_RRT_REDUNDANZ                                              0x00000400
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_EN_NON_RRT_REDUNDANZ                                              10
// ACCESS__EN_NON_RRT_REDUNDANZ: r,w
// DESCR__EN_NON_RRT_REDUNDANZ: ‚0‘: Redundanz nur für RRT-Telegramme‚1‘: Redundanz für alle Telegramme (NRT und SRT)
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_2_PORT_CT_MODE                                                    0x00000200
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_2_PORT_CT_MODE                                                    9
// ACCESS__2_PORT_CT_MODE: r,w
// DESCR__2_PORT_CT_MODE: ‚0‘: Einspeisung von Telegrammen in einen redundanten Ringüber API oder Ports, ERTEC-Kopplung erlaubt.‚1‘: Einspeisung von Telegrammen in einen redundanten Ring ausschließlich über API, keine ERTEC-Kopplung erlaubt.
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_ENABLE_DCP_FILTER                                                 0x00000100
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_ENABLE_DCP_FILTER                                                 8
// ACCESS__ENABLE_DCP_FILTER: r,w
// DESCR__ENABLE_DCP_FILTER: ‚0‘: DCP-Request-Filter ist ausgeschaltet‚1‘: DCP-Request-Filter ist eingeschaltet
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_ENABLE_ARP_FILTER                                                 0x00000080
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_ENABLE_ARP_FILTER                                                 7
// ACCESS__ENABLE_ARP_FILTER: r,w
// DESCR__ENABLE_ARP_FILTER: ‚0‘: ARP-Request-Filter ist ausgeschaltet‚1‘: ARP-Request-Filter ist eingeschaltet
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_NRT_CT_DELAY                                                      0x0000007E
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_NRT_CT_DELAY                                                      1
// ACCESS__NRT_CT_DELAY: r,w
// DESCR__NRT_CT_DELAY: Anzahl der Einträge in den NRT-Datenpuffern, bevor die Daten den NRT-Sendern übergeben werden.(1 Eintrag  = 8 Byte)
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL__MSK_TAG_MODE                                                          0x00000001
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_SHFT_TAG_MODE                                                          0
// ACCESS__TAG_MODE: r,w
// DESCR__TAG_MODE: ‚0‘: beim Empfang Auswerten des Tags, ungetaggte Telegramme werden entsprechend der Adresstabelle verwaltet.‚1‘: ungetaggte empfangene Telegramme werden mit dem Default-Tag des Empfangsports versehen.
#define IRTE_R7_REG_NRT_TRANSFER_CONTROL_RST__VAL                         0x00000000


#define IRTE_R7_REG_NRT_SAFETYMARGIN                                                                        0x0001906C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_NRT_SAFETYMARGIN__MSK_NRT_SAFETYMARGIN                                                      0xFFFFFFFF
#define IRTE_R7_REG_NRT_SAFETYMARGIN_SHFT_NRT_SAFETYMARGIN                                                      0
// ACCESS__NRT_SAFETYMARGIN: r,w
// DESCR__NRT_SAFETYMARGIN: Aufschlag, der für die Berechnung der Sendezeit der NRT-/SRT-Telegramme für den aktuellen Zyklus bei 10/100 MBit-Übertragung mitberücksichtigt werden muss (beinhaltet: Idle-Time, Preambel, MAC-Durchlaufzeit und Steuerwerk-Bearbeitungszeit).Der Wert ist in 10ns Cycle-Time-Intervallen anzugeben:100MBit   Wert  = A0H  10MBit   Wert  = 640H
#define IRTE_R7_REG_NRT_SAFETYMARGIN_RST__VAL                             0x00000000


#define IRTE_R7_REG_MONITOR_CONTROL                                                                         0x00019070
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_MONITOR_CONTROL__MSK_DESTINATION_PORTS                                                      0x0000003F
#define IRTE_R7_REG_MONITOR_CONTROL_SHFT_DESTINATION_PORTS                                                      0
// ACCESS__DESTINATION_PORTS: r,w
// DESCR__DESTINATION_PORTS: Spezifikation des Beobachterports. Destination-Port  = Aufbau wie in der UCMC-Adresstabelle0,1   : lokalen Kanäle A und B2...5 : Ethernet-Ports
#define IRTE_R7_REG_MONITOR_CONTROL_RST__VAL                              0x00000000


#define IRTE_R7_REG_STATISTIC_CTRL_BASE                                                                     0x00019074
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_STATISTIC_CTRL_BASE__MSK_STATISTIC                                                          0x001FFFFF
#define IRTE_R7_REG_STATISTIC_CTRL_BASE_SHFT_STATISTIC                                                          0
// ACCESS__STATISTIC: r,w
// DESCR__STATISTIC: Zeiger ins K-RAM, auf einen Bereich, ab dem die Port-spezifischen Statistikzähler abgelegt sind.
#define IRTE_R7_REG_STATISTIC_CTRL_BASE_RST__VAL                          0x00000000


#define IRTE_R7_REG_SWITCH_SETUP                                                                            0x00019078
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_SWITCH_SETUP__MSK_EXTENDED_DELAY                                                            0x00002000
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_EXTENDED_DELAY                                                            13
// ACCESS__EXTENDED_DELAY: r,w
// DESCR__EXTENDED_DELAY: In einem Zeitframe, wird die Erweiterung des Delay-Feldes (1ns-Auflösung)    ‚0’: nicht behandelt    ‚1’: behandelt
#define IRTE_R7_REG_SWITCH_SETUP__MSK_CONFIRMATION_WR_DONE                                                      0x00001000
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_CONFIRMATION_WR_DONE                                                      12
// ACCESS__CONFIRMATION_WR_DONE: r,w
// DESCR__CONFIRMATION_WR_DONE: Der letzte Schreibzugriff in ein externes RAM (PCI-RAM) vor Auslösen eines Interrupts, wird auf Ausführung    ‚0’: nicht geprüft    ‚1’: geprüft (durch einen Lesezugriff)
#define IRTE_R7_REG_SWITCH_SETUP__MSK_RTOVERUDP_ENABLE                                                          0x00000400
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_RTOVERUDP_ENABLE                                                          10
// ACCESS__RTOVERUDP_ENABLE: r,w
// DESCR__RTOVERUDP_ENABLE: Filterung der RToverUDP-Frames zum SRT-API ist    ‚0’: gesperrt    ‚1’: freigegeben
#define IRTE_R7_REG_SWITCH_SETUP__MSK_IGNORE_SUBSTITUTE_FRAME                                                   0x00000200
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_IGNORE_SUBSTITUTE_FRAME                                                   9
// ACCESS__IGNORE_SUBSTITUTE_FRAME: r,w
// DESCR__IGNORE_SUBSTITUTE_FRAME: Bei IRT-Telegrammausfall bzw. bei Erkennung von ungültigen IRT-Telegrammen vor dem Weiterleitungszeitpunkt, werden:‚0’: Ersatz-Telegramme erzeugt‚1’: Ersatz-Telegramme nicht erzeugt
#define IRTE_R7_REG_SWITCH_SETUP__MSK_MAC_TXSTAT_VALUATION                                                      0x00000100
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_MAC_TXSTAT_VALUATION                                                      8
// ACCESS__MAC_TXSTAT_VALUATION: r,w
// DESCR__MAC_TXSTAT_VALUATION: Zur Kollisionserkennung werden die Informationen des MAC-Sendestatus (vor dem Telegrammende):‚0’: nicht ausgewertet‚1’: ausgewertet
#define IRTE_R7_REG_SWITCH_SETUP__MSK_CONFIRMATION_SNDTIME                                                      0x00000080
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_CONFIRMATION_SNDTIME                                                      7
// ACCESS__CONFIRMATION_SNDTIME: r,w
// DESCR__CONFIRMATION_SNDTIME: Im DMACW wird der Sendezeitpunkt des NRT-/SRT-Telegramms beim gesetztem „Precision-Time“ im DMACW:‚0’: nicht eingetragen‚1’: eingetragen
#define IRTE_R7_REG_SWITCH_SETUP__MSK_SRT_API_CHARGE_MODE                                                       0x00000040
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_SRT_API_CHARGE_MODE                                                       6
// ACCESS__SRT_API_CHARGE_MODE: r,w
// DESCR__SRT_API_CHARGE_MODE: Sind keine SRT-FCW-Elemente mehr vorhanden, dann:‚0’: die nachfolgenden ACW-Elemente verwerfen‚1’: warten bis genügend SRT-FCWs vorhanden sind, jedoch       längstens bis zum Zyklusende
#define IRTE_R7_REG_SWITCH_SETUP__MSK_NRT_RCVBUFFER_DISCHARGE_MODE                                              0x00000020
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_NRT_RCVBUFFER_DISCHARGE_MODE                                              5
// ACCESS__NRT_RCVBUFFER_DISCHARGE_MODE: r,w
// DESCR__NRT_RCVBUFFER_DISCHARGE_MODE: ‚0’: Der Receive-Buffer wird bereits während des NRT-Transfers      freigegeben.‚1’: Der Receive-Buffer wird erst nach dem NRT-Transfer       freigegeben.
#define IRTE_R7_REG_SWITCH_SETUP__MSK_NRT_RCV_API_BUFFER_MODE                                                   0x00000010
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_NRT_RCV_API_BUFFER_MODE                                                   4
// ACCESS__NRT_RCV_API_BUFFER_MODE: r,w
// DESCR__NRT_RCV_API_BUFFER_MODE: Vorgehensweise bei zu kleinen bereitgestellten DMACW-Empfangs-Datenpuffern:‚0’: „Transfer-only“ solange die DMACW-Kette durchsuchen, bis ein Datenpuffer     passender Länge gefunden wird. Alle anderen Puffer mit Result-     Status „DMACW-Datenpuffer zu klein“ der SW zurückgeben.‚1’: „Transfer-always“ so viele Telegrammdaten wie möglich in den Datenpuffer ein      tragen, die restlichen Daten verwerfen und den DMACW-      Datenpuffer mit Result-Status „DMACW-Datenpuffer zu klein“      der SW zurückgeben.
#define IRTE_R7_REG_SWITCH_SETUP__MSK_NRT_SND_API_LW_REACTION                                                   0x00000008
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_NRT_SND_API_LW_REACTION                                                   3
// ACCESS__NRT_SND_API_LW_REACTION: r,w
// DESCR__NRT_SND_API_LW_REACTION: Handhabung von NRT-Aufträgen bei erreichter Low-Water-Mark am NRT-Sende-API:‚0’: wiederholen, wenn Low-Water-Mark unterschritten‚1’: verwerfen solange Low-Water-Mark unterschritten
#define IRTE_R7_REG_SWITCH_SETUP__MSK_IRT_DMA_ACCESS                                                            0x00000004
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_IRT_DMA_ACCESS                                                            2
// ACCESS__IRT_DMA_ACCESS: r,w
// DESCR__IRT_DMA_ACCESS: DMA-Transfer der synchronen API-Daten über:‚0’: K-Bus‚1’: IDA-Bus
#define IRTE_R7_REG_SWITCH_SETUP__MSK_MAC_CHARGE_MODE                                                           0x00000002
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_MAC_CHARGE_MODE                                                           1
// ACCESS__MAC_CHARGE_MODE: r,w
// DESCR__MAC_CHARGE_MODE: Lade-Modus der MAC:‚0’: Slow-Mode („MAC-Half-Full“ wird beachtet)‚1’: Quick-Mode („MAC-Half-Full“ wird nicht beachtet)
#define IRTE_R7_REG_SWITCH_SETUP__MSK_NRT_API_ACCESS                                                            0x00000001
#define IRTE_R7_REG_SWITCH_SETUP_SHFT_NRT_API_ACCESS                                                            0
// ACCESS__NRT_API_ACCESS: r,w
// DESCR__NRT_API_ACCESS: ‚0‘: DMA-Sende- und DMA-Empfangs-Transfers arbeiten      überlappend.‚1‘: DMA-Sende- und DMA-Empfangs-Transfers arbeiten      separat (nacheinander).
#define IRTE_R7_REG_SWITCH_SETUP_RST__VAL                                 0x00000000


#define IRTE_R7_REG_FILTER_CTRL_BASE                                                                        0x0001907C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_FILTER_CTRL_BASE__MSK_FILTER_CTRL_BASIC_PTR                                                 0x001FFFFF
#define IRTE_R7_REG_FILTER_CTRL_BASE_SHFT_FILTER_CTRL_BASIC_PTR                                                 0
// ACCESS__FILTER_CTRL_BASIC_PTR: r,w
// DESCR__FILTER_CTRL_BASIC_PTR: Zeiger ins K-RAM auf einen Bereich, an dem die Filter-Liste zur ARP- und DCP-Filterung vorhanden ist.
#define IRTE_R7_REG_FILTER_CTRL_BASE_RST__VAL                             0x00000000


#define IRTE_R7_REG_SWITCH_CT_DELAY                                                                         0x00019080
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_SWITCH_CT_DELAY__MSK_SWITCH_CT_DELAY                                                        0x0000FFFF
#define IRTE_R7_REG_SWITCH_CT_DELAY_SHFT_SWITCH_CT_DELAY                                                        0
// ACCESS__SWITCH_CT_DELAY: r,w
// DESCR__SWITCH_CT_DELAY: Minimale Durchleitezeit für iSRT-Frames bei Cut-Through. Dabei ist in 1ns-Einheiten einzustellen:100 Mbit  Wert  = 1540H (6.6 µs)  10 Mbit  Wert  = 6B80H (36.8 µs)  Disable  Wert  = 0000H
#define IRTE_R7_REG_SWITCH_CT_DELAY_RST__VAL                              0x00000000


#define IRTE_R7_REG_IO_CONTROL_DELAY                                                                        0x00019084
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_IO_CONTROL_DELAY__MSK_IO_CONTROL_DELAY                                                      0x0000003F
#define IRTE_R7_REG_IO_CONTROL_DELAY_SHFT_IO_CONTROL_DELAY                                                      0
// ACCESS__IO_CONTROL_DELAY: r,w
// DESCR__IO_CONTROL_DELAY: Verzögerungszeit, die abgewartet werden muss bevor ein, über IO-Control zu transferierender SRT-Frame, versendet werden darf (Wert  = 2CH).
#define IRTE_R7_REG_IO_CONTROL_DELAY_RST__VAL                             0x00000000


#define IRTE_R7_REG_ROUTER_MAC_ADDRESS31_0                                                                  0x0001940C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS31_0__MSK_ROUTER_MAC_ADDRESS                                              0xFFFFFFFF
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS31_0_SHFT_ROUTER_MAC_ADDRESS                                              0
// ACCESS__ROUTER_MAC_ADDRESS: r,w
// DESCR__ROUTER_MAC_ADDRESS: MAC-Adresse des nächsten angeschlossenen Routers
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS31_0_RST__VAL                       0x00000000


#define IRTE_R7_REG_ROUTER_MAC_ADDRESS47_32                                                                 0x00019410
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS47_32__MSK_ROUTER_MAC_ADDRESS                                             0x0000FFFF
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS47_32_SHFT_ROUTER_MAC_ADDRESS                                             0
// ACCESS__ROUTER_MAC_ADDRESS: r,w
// DESCR__ROUTER_MAC_ADDRESS: MAC-Adresse des nächsten angeschlossenen Routers
#define IRTE_R7_REG_ROUTER_MAC_ADDRESS47_32_RST__VAL                      0x00000000


#define IRTE_R7_REG_TRIGGER_CONTROL                                                                         0x0001A000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_SELECT_BUSGROUP                                                        0x00000800
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_SELECT_BUSGROUP                                                        11
// ACCESS__SELECT_BUSGROUP: r,w
// DESCR__SELECT_BUSGROUP: 0 : triggern auf einen der internen Busse    :  KBUS oder IDA 1 : triggern auf einen der ‚externen’ Busse : SC-Bus oder MC-Buswelcher Bus selektiert wird bestimmt dann Bit 1 (s.o.)
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_ENABLE_EXTERNAL_INTERRUPT                                              0x00000600
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_ENABLE_EXTERNAL_INTERRUPT                                              9
// ACCESS__ENABLE_EXTERNAL_INTERRUPT: r,w
// DESCR__ENABLE_EXTERNAL_INTERRUPT: 80ns Trigger-Impuls:00 : des Frame-Monitors bei erkannten Trigger-Event01 : der Trace-Unit bei jedem Trace-Eintrag10 : der Trigger-Unit bei Bus-Triggerung11 : des Frame-Monitors bei Frames mit CRC-Fehler
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_BYTEENABLE                                                  0x00000100
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_BYTEENABLE                                                  8
// ACCESS__TRIGGER_ON_BYTEENABLE: r,w
// DESCR__TRIGGER_ON_BYTEENABLE: Byteenablemuster als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_LOCK_CLASS                                                  0x00000080
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_LOCK_CLASS                                                  7
// ACCESS__TRIGGER_ON_LOCK_CLASS: r,w
// DESCR__TRIGGER_ON_LOCK_CLASS: Lock-Klasse als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_WRITE                                                       0x00000040
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_WRITE                                                       6
// ACCESS__TRIGGER_ON_WRITE: r,w
// DESCR__TRIGGER_ON_WRITE: Schreibvorgang als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_READ                                                        0x00000020
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_READ                                                        5
// ACCESS__TRIGGER_ON_READ: r,w
// DESCR__TRIGGER_ON_READ: Lesevorgang als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_MASTER                                                      0x00000010
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_MASTER                                                      4
// ACCESS__TRIGGER_ON_MASTER: r,w
// DESCR__TRIGGER_ON_MASTER: Busteilnehmer als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_WRITEDATA                                                   0x00000008
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_WRITEDATA                                                   3
// ACCESS__TRIGGER_ON_WRITEDATA: r,w
// DESCR__TRIGGER_ON_WRITEDATA: Datenvergleich als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_TRIGGER_ON_ADDRESS                                                     0x00000004
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_TRIGGER_ON_ADDRESS                                                     2
// ACCESS__TRIGGER_ON_ADDRESS: r,w
// DESCR__TRIGGER_ON_ADDRESS: Adressvergleich als Triggerbedingung selektiert
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_BUS_SELECT                                                             0x00000002
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_BUS_SELECT                                                             1
// ACCESS__BUS_SELECT: r,w
// DESCR__BUS_SELECT: triggern auf K-Bus / SC-Bus, 1 : triggert auf IDA-Bus / MC-Bus
#define IRTE_R7_REG_TRIGGER_CONTROL__MSK_MODE                                                                   0x00000001
#define IRTE_R7_REG_TRIGGER_CONTROL_SHFT_MODE                                                                   0
// ACCESS__MODE: r,w
// DESCR__MODE: 0 : Monitor-Betrieb : meldet K-Bus und SC-Bus-Adressfehler1 : Trigger-Betrieb  : meldet Eintreten der Triggerbedingungen
#define IRTE_R7_REG_TRIGGER_CONTROL_RST__VAL                              0x00000000


#define IRTE_R7_REG_TRIGGER_ADDRESS_MASK                                                                    0x0001A004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_ADDRESS_MASK__MSK_ADDRESS_MASK                                                      0xFFFFFFFF
#define IRTE_R7_REG_TRIGGER_ADDRESS_MASK_SHFT_ADDRESS_MASK                                                      0
// ACCESS__ADDRESS_MASK: r,w
// DESCR__ADDRESS_MASK: Maske, mit der Busadresse und Vergleichswert und-verknüpft werden
#define IRTE_R7_REG_TRIGGER_ADDRESS_MASK_RST__VAL                         0x00000000


#define IRTE_R7_REG_TRIGGER_UPPER_DATA_MASK                                                                 0x0001A00C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_UPPER_DATA_MASK__MSK_UPPER_DATA_MASK                                                0xFFFFFFFF
#define IRTE_R7_REG_TRIGGER_UPPER_DATA_MASK_SHFT_UPPER_DATA_MASK                                                0
// ACCESS__UPPER_DATA_MASK: r,w
// DESCR__UPPER_DATA_MASK: Maske, mit der Busdaten und Vergleichswert und-verknüpft werden
#define IRTE_R7_REG_TRIGGER_UPPER_DATA_MASK_RST__VAL                      0x00000000


#define IRTE_R7_REG_TRIGGER_ADDRESS                                                                         0x0001A400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_ADDRESS__MSK_TRIGGER_ADDRESS                                                        0xFFFFFFFF
#define IRTE_R7_REG_TRIGGER_ADDRESS_SHFT_TRIGGER_ADDRESS                                                        0
// ACCESS__TRIGGER_ADDRESS: r,w
// DESCR__TRIGGER_ADDRESS: Enthält Adresse des den Trigger auslösenden Buszugriffs bzw. als Triggerbedingung den Vergleichswert (8-Byte-aligned).
#define IRTE_R7_REG_TRIGGER_ADDRESS_RST__VAL                              0x00000000


#define IRTE_R7_REG_TRIGGER_UPPER_DATA                                                                      0x0001A408
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_UPPER_DATA__MSK_TRIGGER_UPPER_DATA                                                  0xFFFFFFFF
#define IRTE_R7_REG_TRIGGER_UPPER_DATA_SHFT_TRIGGER_UPPER_DATA                                                  0
// ACCESS__TRIGGER_UPPER_DATA: r,w
// DESCR__TRIGGER_UPPER_DATA: Enthält Daten des den Trigger auslösenden Buszugriffs bzw. als Triggerbedingung den Vergleichswert (nur Write-Daten)
#define IRTE_R7_REG_TRIGGER_UPPER_DATA_RST__VAL                           0x00000000


#define IRTE_R7_REG_TRIGGER_INFO                                                                            0x0001A40C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_INFO__MSK_ADRESSFEHLERHERKUNFT                                                      0x00300000
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_ADRESSFEHLERHERKUNFT                                                      20
// ACCESS__ADRESSFEHLERHERKUNFT: r,w
// DESCR__ADRESSFEHLERHERKUNFT: nur im Monitor-Betrieb relevant00 : ./.01 : K-Bus-Adressfehler10 : SC-Bus Adressfehler11 : IDA-Bus Adressfehler
#define IRTE_R7_REG_TRIGGER_INFO__MSK_BYTEENABLE                                                                0x000FF000
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_BYTEENABLE                                                                12
// ACCESS__BYTEENABLE: r,w
// DESCR__BYTEENABLE: Byteenable
#define IRTE_R7_REG_TRIGGER_INFO__MSK_LOCK_CLASS                                                                0x00000F00
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_LOCK_CLASS                                                                8
// ACCESS__LOCK_CLASS: r,w
// DESCR__LOCK_CLASS: LockklasseLock ist nur relevant bei IDA-Bus, K-BUS und SC-Bus (dort nur eine Lock-Klasse)
#define IRTE_R7_REG_TRIGGER_INFO__MSK_WRITE                                                                     0x00000080
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_WRITE                                                                     7
// ACCESS__WRITE: r,w
// DESCR__WRITE: Schreibzugriff
#define IRTE_R7_REG_TRIGGER_INFO__MSK_READ                                                                      0x00000040
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_READ                                                                      6
// ACCESS__READ: r,w
// DESCR__READ: Lesezugriff
#define IRTE_R7_REG_TRIGGER_INFO__MSK_BUSMASTER                                                                 0x0000003F
#define IRTE_R7_REG_TRIGGER_INFO_SHFT_BUSMASTER                                                                 0
// ACCESS__BUSMASTER: r,w
// DESCR__BUSMASTER: Nummer des den Zugriff auslösenden BusmastersIDA-Bus selektiert :0  :  KRAM_control1  :  IRT-Control: IRT_dma_int_control2  :  IRT-Control: IRT_dma_ext_control3  :  Ethernet-Port 0 4  :  Ethernet-Port 15  :  Ethernet-Port 26  :  Ethernet-Port 3K-Bus selektiert oder K-Bus Fehler (im Monitor-Betrieb):0 : Zyklussynchronisation  1 : KRAM_control  2 : IRT_tracecontrol  3 : Statistikcontrol  4 : SC_Buskoppler  5 : SRT-Control: SRT_rcv_control  6 : SRT-Control: SRT_api_control  7 : NRT-Control: NRT_rcv_api_control  8 : NRT-Control: NRT_rcv_db_control  9 : NRT-Control: NRT_api_control (Rcv-Modul)10 : NRT-Control: NRT_api_control (Snd-Modul)11 : NRT-Control: NRT_snd_api_control12 : NRT-Control: NRT_filter_control13 : IRT-Control: IRT_dma_int_control14 : IRT-Control: IRT_dma_ext_control15 : Port 0 : Rcv_control16 : Port 0 : NRT_rcv_control17 : Port 0 : NRT_rcv_db_control18 : Port 0 : IRT_diag_control19 : Port 0 : IRT_snd_control20 : Port 0 : NRT_snd_control21 : Port 1 : Rcv_control22 : Port 1 : NRT_rcv_control23 : Port 1 : NRT_rcv_db_control24 : Port 1 : IRT_diag_control25 : Port 1 : IRT_snd_control26 : Port 1 : NRT_snd_control27 : Port 2 : Rcv_control28 : Port 2 : NRT_rcv_control29 : Port 2 : NRT_rcv_db_control30 : Port 2 : IRT_diag_control31 : Port 2 : IRT_snd_control32 : Port 2 : NRT_snd_control33 : Port 3 : Rcv_control34 : Port 3 : NRT_rcv_control35 : Port 3 : NRT_rcv_db_control36 : Port 3 : IRT_diag_control37 : Port 3 : IRT_snd_control38 : Port 3 : NRT_snd_controlMC-Bus selektiert :0 : DMA-control              1 : NRT-Send-control              2 : IRT-control              3 : NRT-Receive-controlSC-Bus selektiert : 0 : Slave-Businterface
#define IRTE_R7_REG_TRIGGER_INFO_RST__VAL                                 0x00000000


#define IRTE_R7_REG_TRIGGER_RESET                                                                           0x0001A410
// ACCESS__: r,w
// DESCR__: 
#define IRTE_R7_REG_TRIGGER_RESET__MSK_TRIGGER_UNIT_LOCKED                                                      0x00000001
#define IRTE_R7_REG_TRIGGER_RESET_SHFT_TRIGGER_UNIT_LOCKED                                                      0
// ACCESS__TRIGGER_UNIT_LOCKED: r,w
// DESCR__TRIGGER_UNIT_LOCKED: write:‚0’: die Trigger/Monitor-Funktion ist frei‚1’: die Trigger/Monitor-Funktion ist gesperrtread:‚0’: die Trigger-Unit ist freigegeben ‚1’: die Trigger-Unit ist nach Auftreteneiner Triggerbedingung eines Adressfehlers oder nach Schreiben einer 1 an diese Stelle       gegen erneutes Auslösen gesperrt.
#define IRTE_R7_REG_TRIGGER_RESET_RST__VAL                                0x00000000


#define IRTE_R7_REG_CC_DIFF_SUMMARY_P0                                                                      0x0001B070
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P0__MSK_CC_DIFF_SUMMARY                                                     0xFFFFFFFF
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P0_SHFT_CC_DIFF_SUMMARY                                                     0
// ACCESS__CC_DIFF_SUMMARY: r,
// DESCR__CC_DIFF_SUMMARY: Summe der Differenzen alle Empfangszeiten der aktiven Zyklussynchronisations-Telegramme zum Clock-Counter-Wert am Beginn des Isochronzyklus.
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P0_RST__VAL                           0x00000000


#define IRTE_R7_REG_CC_DELAY_SUMMARY_P0                                                                     0x0001B074
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P0__MSK_CC_DELAY_SUMMARY                                                   0xFFFFFFFF
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P0_SHFT_CC_DELAY_SUMMARY                                                   0
// ACCESS__CC_DELAY_SUMMARY: r,
// DESCR__CC_DELAY_SUMMARY: Summe alle Verzögerungszeiten der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P0_RST__VAL                          0x00000000


#define IRTE_R7_REG_CC_COUNT_P0                                                                             0x0001B078
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_COUNT_P0__MSK_CC_COUNT                                                                   0x0000FFFF
#define IRTE_R7_REG_CC_COUNT_P0_SHFT_CC_COUNT                                                                   0
// ACCESS__CC_COUNT: r,
// DESCR__CC_COUNT: Anzahl der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_COUNT_P0_RST__VAL                                  0x00000000


#define IRTE_R7_REG_CC_DIFF_SUMMARY_P1                                                                      0x0001B170
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P1__MSK_CC_DIFF_SUMMARY                                                     0xFFFFFFFF
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P1_SHFT_CC_DIFF_SUMMARY                                                     0
// ACCESS__CC_DIFF_SUMMARY: r,
// DESCR__CC_DIFF_SUMMARY: Summe der Differenzen alle Empfangszeiten der aktiven Zyklussynchronisations-Telegramme zum Clock-Counter-Wert am Beginn des Isochronzyklus.
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P1_RST__VAL                           0x00000000


#define IRTE_R7_REG_CC_DELAY_SUMMARY_P1                                                                     0x0001B174
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P1__MSK_CC_DELAY_SUMMARY                                                   0xFFFFFFFF
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P1_SHFT_CC_DELAY_SUMMARY                                                   0
// ACCESS__CC_DELAY_SUMMARY: r,
// DESCR__CC_DELAY_SUMMARY: Summe alle Verzögerungszeiten der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P1_RST__VAL                          0x00000000


#define IRTE_R7_REG_CC_COUNT_P1                                                                             0x0001B178
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_COUNT_P1__MSK_CC_COUNT                                                                   0x0000FFFF
#define IRTE_R7_REG_CC_COUNT_P1_SHFT_CC_COUNT                                                                   0
// ACCESS__CC_COUNT: r,
// DESCR__CC_COUNT: Anzahl der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_COUNT_P1_RST__VAL                                  0x00000000


#define IRTE_R7_REG_CC_DIFF_SUMMARY_P2                                                                      0x0001B270
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P2__MSK_CC_DIFF_SUMMARY                                                     0xFFFFFFFF
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P2_SHFT_CC_DIFF_SUMMARY                                                     0
// ACCESS__CC_DIFF_SUMMARY: r,
// DESCR__CC_DIFF_SUMMARY: Summe der Differenzen alle Empfangszeiten der aktiven Zyklussynchronisations-Telegramme zum Clock-Counter-Wert am Beginn des Isochronzyklus.
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P2_RST__VAL                           0x00000000


#define IRTE_R7_REG_CC_DELAY_SUMMARY_P2                                                                     0x0001B274
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P2__MSK_CC_DELAY_SUMMARY                                                   0xFFFFFFFF
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P2_SHFT_CC_DELAY_SUMMARY                                                   0
// ACCESS__CC_DELAY_SUMMARY: r,
// DESCR__CC_DELAY_SUMMARY: Summe alle Verzögerungszeiten der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P2_RST__VAL                          0x00000000


#define IRTE_R7_REG_CC_COUNT_P2                                                                             0x0001B278
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_COUNT_P2__MSK_CC_COUNT                                                                   0x0000FFFF
#define IRTE_R7_REG_CC_COUNT_P2_SHFT_CC_COUNT                                                                   0
// ACCESS__CC_COUNT: r,
// DESCR__CC_COUNT: Anzahl der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_COUNT_P2_RST__VAL                                  0x00000000


#define IRTE_R7_REG_CC_DIFF_SUMMARY_P3                                                                      0x0001B370
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P3__MSK_CC_DIFF_SUMMARY                                                     0xFFFFFFFF
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P3_SHFT_CC_DIFF_SUMMARY                                                     0
// ACCESS__CC_DIFF_SUMMARY: r,
// DESCR__CC_DIFF_SUMMARY: Summe der Differenzen alle Empfangszeiten der aktiven Zyklussynchronisations-Telegramme zum Clock-Counter-Wert am Beginn des Isochronzyklus.
#define IRTE_R7_REG_CC_DIFF_SUMMARY_P3_RST__VAL                           0x00000000


#define IRTE_R7_REG_CC_DELAY_SUMMARY_P3                                                                     0x0001B374
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P3__MSK_CC_DELAY_SUMMARY                                                   0xFFFFFFFF
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P3_SHFT_CC_DELAY_SUMMARY                                                   0
// ACCESS__CC_DELAY_SUMMARY: r,
// DESCR__CC_DELAY_SUMMARY: Summe alle Verzögerungszeiten der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_DELAY_SUMMARY_P3_RST__VAL                          0x00000000


#define IRTE_R7_REG_CC_COUNT_P3                                                                             0x0001B378
// ACCESS__: r,
// DESCR__: 
#define IRTE_R7_REG_CC_COUNT_P3__MSK_CC_COUNT                                                                   0x0000FFFF
#define IRTE_R7_REG_CC_COUNT_P3_SHFT_CC_COUNT                                                                   0
// ACCESS__CC_COUNT: r,
// DESCR__CC_COUNT: Anzahl der aktiven Zyklussynchronisations-Telegramme
#define IRTE_R7_REG_CC_COUNT_P3_RST__VAL                                  0x00000000

////////////////////////////////////////////////////////////////////////////////////
// Submodules for Module IRTE
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// Memories for Module IRTE
////////////////////////////////////////////////////////////////////////////////////
#define IRTE_R7_RAM_K_START 0x00100000
#define IRTE_R7_RAM_K_END   0x0012BFFF
////////////////////////////////////////////////////////////////////////////////////





/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/


/*****************************************************************************/
/*  end of file irte_rev07_rsl.h                                             */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  // of IRTE_REV07_RSL_H
