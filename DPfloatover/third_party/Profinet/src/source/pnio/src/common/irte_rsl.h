//////////////////////////////////////////////////////////////////////
//// AHB Register Include File
//// Version:           0.5
//// Version RSL-Tools: 0.6.9B
//// Erstellungsdatum:  08.11.2010
////
//////////////////////////////////////////////////////////////////////

#ifndef IRTE_RSL_H                       /* ----- reinclude-protection ----- */
#define IRTE_RSL_H

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
/*  F i l e               &F: irte_rsl.h                                :F&  */
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


#define IRTE_REG_PORT_CONTROL_P0                                                                         0x00000000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PORT_CONTROL_P0__MSK_DIAG_SYNRST                                                            0x00000800
#define IRTE_REG_PORT_CONTROL_P0_SHFT_DIAG_SYNRST                                                            11
// ACCESS__DIAG_SYNRST: r,w
// DESCR__DIAG_SYNRST: Synchroner Reset der IRT-Diag-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_SND_SYNRST                                                             0x00000400
#define IRTE_REG_PORT_CONTROL_P0_SHFT_SND_SYNRST                                                             10
// ACCESS__SND_SYNRST: r,w
// DESCR__SND_SYNRST: Synchroner Reset der Snd-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_NRTSND_SYNRST                                                          0x00000100
#define IRTE_REG_PORT_CONTROL_P0_SHFT_NRTSND_SYNRST                                                          8
// ACCESS__NRTSND_SYNRST: r,w
// DESCR__NRTSND_SYNRST: Synchroner Reset der NRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_IRT_SYNRST                                                             0x00000080
#define IRTE_REG_PORT_CONTROL_P0_SHFT_IRT_SYNRST                                                             7
// ACCESS__IRT_SYNRST: r,w
// DESCR__IRT_SYNRST: Synchroner Reset der IRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_NRTRCV_SYNRST                                                          0x00000010
#define IRTE_REG_PORT_CONTROL_P0_SHFT_NRTRCV_SYNRST                                                          4
// ACCESS__NRTRCV_SYNRST: r,w
// DESCR__NRTRCV_SYNRST: Synchroner Reset der NRT-Rcv-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_DB_SYNRST                                                              0x00000008
#define IRTE_REG_PORT_CONTROL_P0_SHFT_DB_SYNRST                                                              3
// ACCESS__DB_SYNRST: r,w
// DESCR__DB_SYNRST: Synchroner Reset der IRTE/SRT-Rcv-DB-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_CT_SYNRST                                                              0x00000004
#define IRTE_REG_PORT_CONTROL_P0_SHFT_CT_SYNRST                                                              2
// ACCESS__CT_SYNRST: r,w
// DESCR__CT_SYNRST: Synchroner Reset der IRTE-CT-Control
#define IRTE_REG_PORT_CONTROL_P0__MSK_RCV_SYNRST                                                             0x00000002
#define IRTE_REG_PORT_CONTROL_P0_SHFT_RCV_SYNRST                                                             1
// ACCESS__RCV_SYNRST: r,w
// DESCR__RCV_SYNRST: Synchroner Reset der Rcv-Control
#define IRTE_REG_PORT_CONTROL_P0_RST__VAL                              0x00000000


#define IRTE_REG_MIN_PREAMBLE_COUNT_P0                                                                   0x00000004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MIN_PREAMBLE_COUNT_P0__MSK_MIN_PREAMBLE_COUNT                                               0x000000FF
#define IRTE_REG_MIN_PREAMBLE_COUNT_P0_SHFT_MIN_PREAMBLE_COUNT                                               0
// ACCESS__MIN_PREAMBLE_COUNT: r,w
// DESCR__MIN_PREAMBLE_COUNT: Wert / = FFh:Gibt die minimale Bitanzahl der Preambel am MII / RMII an,bevor ein Telegrammempfang erlaubt ist.Wert  = FFh:Unterstützt den Loop-Back-Modus der MAC. Die Funktion ist nur in Zusammenhang mit dem gesetzten „MAC-Control“-Bit 4 aktiv.
#define IRTE_REG_MIN_PREAMBLE_COUNT_P0_RST__VAL                        0x00000000


#define IRTE_REG_DEFAULT_VLAN_TAG_P0                                                                     0x00000014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_DEFAULT_VLAN_TAG_P0__MSK_DEFAULT_VLAN_PRIORITAET                                            0x0000E000
#define IRTE_REG_DEFAULT_VLAN_TAG_P0_SHFT_DEFAULT_VLAN_PRIORITAET                                            13
// ACCESS__DEFAULT_VLAN_PRIORITAET: r,w
// DESCR__DEFAULT_VLAN_PRIORITAET: Default VLAN-Priorität
#define IRTE_REG_DEFAULT_VLAN_TAG_P0__MSK_CFI_BIT                                                            0x00001000
#define IRTE_REG_DEFAULT_VLAN_TAG_P0_SHFT_CFI_BIT                                                            12
// ACCESS__CFI_BIT: r,w
// DESCR__CFI_BIT: : = 0 für IEEE 802.3
#define IRTE_REG_DEFAULT_VLAN_TAG_P0__MSK_DEFAULT_VLAN_ADDRESS                                               0x00000FFF
#define IRTE_REG_DEFAULT_VLAN_TAG_P0_SHFT_DEFAULT_VLAN_ADDRESS                                               0
// ACCESS__DEFAULT_VLAN_ADDRESS: r,w
// DESCR__DEFAULT_VLAN_ADDRESS: Default VLAN-Adresse
#define IRTE_REG_DEFAULT_VLAN_TAG_P0_RST__VAL                          0x00000000


#define IRTE_REG_PORT_CONTROL_P1                                                                         0x00001000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PORT_CONTROL_P1__MSK_DIAG_SYNRST                                                            0x00000800
#define IRTE_REG_PORT_CONTROL_P1_SHFT_DIAG_SYNRST                                                            11
// ACCESS__DIAG_SYNRST: r,w
// DESCR__DIAG_SYNRST: Synchroner Reset der IRT-Diag-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_SND_SYNRST                                                             0x00000400
#define IRTE_REG_PORT_CONTROL_P1_SHFT_SND_SYNRST                                                             10
// ACCESS__SND_SYNRST: r,w
// DESCR__SND_SYNRST: Synchroner Reset der Snd-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_NRTSND_SYNRST                                                          0x00000100
#define IRTE_REG_PORT_CONTROL_P1_SHFT_NRTSND_SYNRST                                                          8
// ACCESS__NRTSND_SYNRST: r,w
// DESCR__NRTSND_SYNRST: Synchroner Reset der NRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_IRT_SYNRST                                                             0x00000080
#define IRTE_REG_PORT_CONTROL_P1_SHFT_IRT_SYNRST                                                             7
// ACCESS__IRT_SYNRST: r,w
// DESCR__IRT_SYNRST: Synchroner Reset der IRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_NRTRCV_SYNRST                                                          0x00000010
#define IRTE_REG_PORT_CONTROL_P1_SHFT_NRTRCV_SYNRST                                                          4
// ACCESS__NRTRCV_SYNRST: r,w
// DESCR__NRTRCV_SYNRST: Synchroner Reset der NRT-Rcv-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_DB_SYNRST                                                              0x00000008
#define IRTE_REG_PORT_CONTROL_P1_SHFT_DB_SYNRST                                                              3
// ACCESS__DB_SYNRST: r,w
// DESCR__DB_SYNRST: Synchroner Reset der IRTE/SRT-Rcv-DB-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_CT_SYNRST                                                              0x00000004
#define IRTE_REG_PORT_CONTROL_P1_SHFT_CT_SYNRST                                                              2
// ACCESS__CT_SYNRST: r,w
// DESCR__CT_SYNRST: Synchroner Reset der IRTE-CT-Control
#define IRTE_REG_PORT_CONTROL_P1__MSK_RCV_SYNRST                                                             0x00000002
#define IRTE_REG_PORT_CONTROL_P1_SHFT_RCV_SYNRST                                                             1
// ACCESS__RCV_SYNRST: r,w
// DESCR__RCV_SYNRST: Synchroner Reset der Rcv-Control
#define IRTE_REG_PORT_CONTROL_P1_RST__VAL                              0x00000000


#define IRTE_REG_MIN_PREAMBLE_COUNT_P1                                                                   0x00001004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MIN_PREAMBLE_COUNT_P1__MSK_MIN_PREAMBLE_COUNT                                               0x000000FF
#define IRTE_REG_MIN_PREAMBLE_COUNT_P1_SHFT_MIN_PREAMBLE_COUNT                                               0
// ACCESS__MIN_PREAMBLE_COUNT: r,w
// DESCR__MIN_PREAMBLE_COUNT: Wert / = FFh:Gibt die minimale Bitanzahl der Preambel am MII / RMII an,bevor ein Telegrammempfang erlaubt ist.Wert  = FFh:Unterstützt den Loop-Back-Modus der MAC. Die Funktion ist nur in Zusammenhang mit dem gesetzten „MAC-Control“-Bit 4 aktiv.
#define IRTE_REG_MIN_PREAMBLE_COUNT_P1_RST__VAL                        0x00000000


#define IRTE_REG_DEFAULT_VLAN_TAG_P1                                                                     0x00001014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_DEFAULT_VLAN_TAG_P1__MSK_DEFAULT_VLAN_PRIORITAET                                            0x0000E000
#define IRTE_REG_DEFAULT_VLAN_TAG_P1_SHFT_DEFAULT_VLAN_PRIORITAET                                            13
// ACCESS__DEFAULT_VLAN_PRIORITAET: r,w
// DESCR__DEFAULT_VLAN_PRIORITAET: Default VLAN-Priorität
#define IRTE_REG_DEFAULT_VLAN_TAG_P1__MSK_CFI_BIT                                                            0x00001000
#define IRTE_REG_DEFAULT_VLAN_TAG_P1_SHFT_CFI_BIT                                                            12
// ACCESS__CFI_BIT: r,w
// DESCR__CFI_BIT: : = 0 für IEEE 802.3
#define IRTE_REG_DEFAULT_VLAN_TAG_P1__MSK_DEFAULT_VLAN_ADDRESS                                               0x00000FFF
#define IRTE_REG_DEFAULT_VLAN_TAG_P1_SHFT_DEFAULT_VLAN_ADDRESS                                               0
// ACCESS__DEFAULT_VLAN_ADDRESS: r,w
// DESCR__DEFAULT_VLAN_ADDRESS: Default VLAN-Adresse
#define IRTE_REG_DEFAULT_VLAN_TAG_P1_RST__VAL                          0x00000000


#define IRTE_REG_PORT_CONTROL_P2                                                                         0x00002000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PORT_CONTROL_P2__MSK_DIAG_SYNRST                                                            0x00000800
#define IRTE_REG_PORT_CONTROL_P2_SHFT_DIAG_SYNRST                                                            11
// ACCESS__DIAG_SYNRST: r,w
// DESCR__DIAG_SYNRST: Synchroner Reset der IRT-Diag-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_SND_SYNRST                                                             0x00000400
#define IRTE_REG_PORT_CONTROL_P2_SHFT_SND_SYNRST                                                             10
// ACCESS__SND_SYNRST: r,w
// DESCR__SND_SYNRST: Synchroner Reset der Snd-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_NRTSND_SYNRST                                                          0x00000100
#define IRTE_REG_PORT_CONTROL_P2_SHFT_NRTSND_SYNRST                                                          8
// ACCESS__NRTSND_SYNRST: r,w
// DESCR__NRTSND_SYNRST: Synchroner Reset der NRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_IRT_SYNRST                                                             0x00000080
#define IRTE_REG_PORT_CONTROL_P2_SHFT_IRT_SYNRST                                                             7
// ACCESS__IRT_SYNRST: r,w
// DESCR__IRT_SYNRST: Synchroner Reset der IRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_NRTRCV_SYNRST                                                          0x00000010
#define IRTE_REG_PORT_CONTROL_P2_SHFT_NRTRCV_SYNRST                                                          4
// ACCESS__NRTRCV_SYNRST: r,w
// DESCR__NRTRCV_SYNRST: Synchroner Reset der NRT-Rcv-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_DB_SYNRST                                                              0x00000008
#define IRTE_REG_PORT_CONTROL_P2_SHFT_DB_SYNRST                                                              3
// ACCESS__DB_SYNRST: r,w
// DESCR__DB_SYNRST: Synchroner Reset der IRTE/SRT-Rcv-DB-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_CT_SYNRST                                                              0x00000004
#define IRTE_REG_PORT_CONTROL_P2_SHFT_CT_SYNRST                                                              2
// ACCESS__CT_SYNRST: r,w
// DESCR__CT_SYNRST: Synchroner Reset der IRTE-CT-Control
#define IRTE_REG_PORT_CONTROL_P2__MSK_RCV_SYNRST                                                             0x00000002
#define IRTE_REG_PORT_CONTROL_P2_SHFT_RCV_SYNRST                                                             1
// ACCESS__RCV_SYNRST: r,w
// DESCR__RCV_SYNRST: Synchroner Reset der Rcv-Control
#define IRTE_REG_PORT_CONTROL_P2_RST__VAL                              0x00000000


#define IRTE_REG_MIN_PREAMBLE_COUNT_P2                                                                   0x00002004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MIN_PREAMBLE_COUNT_P2__MSK_MIN_PREAMBLE_COUNT                                               0x000000FF
#define IRTE_REG_MIN_PREAMBLE_COUNT_P2_SHFT_MIN_PREAMBLE_COUNT                                               0
// ACCESS__MIN_PREAMBLE_COUNT: r,w
// DESCR__MIN_PREAMBLE_COUNT: Wert / = FFh:Gibt die minimale Bitanzahl der Preambel am MII / RMII an,bevor ein Telegrammempfang erlaubt ist.Wert  = FFh:Unterstützt den Loop-Back-Modus der MAC. Die Funktion ist nur in Zusammenhang mit dem gesetzten „MAC-Control“-Bit 4 aktiv.
#define IRTE_REG_MIN_PREAMBLE_COUNT_P2_RST__VAL                        0x00000000


#define IRTE_REG_DEFAULT_VLAN_TAG_P2                                                                     0x00002014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_DEFAULT_VLAN_TAG_P2__MSK_DEFAULT_VLAN_PRIORITAET                                            0x0000E000
#define IRTE_REG_DEFAULT_VLAN_TAG_P2_SHFT_DEFAULT_VLAN_PRIORITAET                                            13
// ACCESS__DEFAULT_VLAN_PRIORITAET: r,w
// DESCR__DEFAULT_VLAN_PRIORITAET: Default VLAN-Priorität
#define IRTE_REG_DEFAULT_VLAN_TAG_P2__MSK_CFI_BIT                                                            0x00001000
#define IRTE_REG_DEFAULT_VLAN_TAG_P2_SHFT_CFI_BIT                                                            12
// ACCESS__CFI_BIT: r,w
// DESCR__CFI_BIT: : = 0 für IEEE 802.3
#define IRTE_REG_DEFAULT_VLAN_TAG_P2__MSK_DEFAULT_VLAN_ADDRESS                                               0x00000FFF
#define IRTE_REG_DEFAULT_VLAN_TAG_P2_SHFT_DEFAULT_VLAN_ADDRESS                                               0
// ACCESS__DEFAULT_VLAN_ADDRESS: r,w
// DESCR__DEFAULT_VLAN_ADDRESS: Default VLAN-Adresse
#define IRTE_REG_DEFAULT_VLAN_TAG_P2_RST__VAL                          0x00000000


#define IRTE_REG_PORT_CONTROL_P3                                                                         0x00003000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PORT_CONTROL_P3__MSK_DIAG_SYNRST                                                            0x00000800
#define IRTE_REG_PORT_CONTROL_P3_SHFT_DIAG_SYNRST                                                            11
// ACCESS__DIAG_SYNRST: r,w
// DESCR__DIAG_SYNRST: Synchroner Reset der IRT-Diag-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_SND_SYNRST                                                             0x00000400
#define IRTE_REG_PORT_CONTROL_P3_SHFT_SND_SYNRST                                                             10
// ACCESS__SND_SYNRST: r,w
// DESCR__SND_SYNRST: Synchroner Reset der Snd-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_NRTSND_SYNRST                                                          0x00000100
#define IRTE_REG_PORT_CONTROL_P3_SHFT_NRTSND_SYNRST                                                          8
// ACCESS__NRTSND_SYNRST: r,w
// DESCR__NRTSND_SYNRST: Synchroner Reset der NRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_IRT_SYNRST                                                             0x00000080
#define IRTE_REG_PORT_CONTROL_P3_SHFT_IRT_SYNRST                                                             7
// ACCESS__IRT_SYNRST: r,w
// DESCR__IRT_SYNRST: Synchroner Reset der IRT-Snd-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_NRTRCV_SYNRST                                                          0x00000010
#define IRTE_REG_PORT_CONTROL_P3_SHFT_NRTRCV_SYNRST                                                          4
// ACCESS__NRTRCV_SYNRST: r,w
// DESCR__NRTRCV_SYNRST: Synchroner Reset der NRT-Rcv-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_DB_SYNRST                                                              0x00000008
#define IRTE_REG_PORT_CONTROL_P3_SHFT_DB_SYNRST                                                              3
// ACCESS__DB_SYNRST: r,w
// DESCR__DB_SYNRST: Synchroner Reset der IRTE/SRT-Rcv-DB-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_CT_SYNRST                                                              0x00000004
#define IRTE_REG_PORT_CONTROL_P3_SHFT_CT_SYNRST                                                              2
// ACCESS__CT_SYNRST: r,w
// DESCR__CT_SYNRST: Synchroner Reset der IRTE-CT-Control
#define IRTE_REG_PORT_CONTROL_P3__MSK_RCV_SYNRST                                                             0x00000002
#define IRTE_REG_PORT_CONTROL_P3_SHFT_RCV_SYNRST                                                             1
// ACCESS__RCV_SYNRST: r,w
// DESCR__RCV_SYNRST: Synchroner Reset der Rcv-Control
#define IRTE_REG_PORT_CONTROL_P3_RST__VAL                              0x00000000


#define IRTE_REG_MIN_PREAMBLE_COUNT_P3                                                                   0x00003004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MIN_PREAMBLE_COUNT_P3__MSK_MIN_PREAMBLE_COUNT                                               0x000000FF
#define IRTE_REG_MIN_PREAMBLE_COUNT_P3_SHFT_MIN_PREAMBLE_COUNT                                               0
// ACCESS__MIN_PREAMBLE_COUNT: r,w
// DESCR__MIN_PREAMBLE_COUNT: Wert / = FFh:Gibt die minimale Bitanzahl der Preambel am MII / RMII an,bevor ein Telegrammempfang erlaubt ist.Wert  = FFh:Unterstützt den Loop-Back-Modus der MAC. Die Funktion ist nur in Zusammenhang mit dem gesetzten „MAC-Control“-Bit 4 aktiv.
#define IRTE_REG_MIN_PREAMBLE_COUNT_P3_RST__VAL                        0x00000000


#define IRTE_REG_DEFAULT_VLAN_TAG_P3                                                                     0x00003014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_DEFAULT_VLAN_TAG_P3__MSK_DEFAULT_VLAN_PRIORITAET                                            0x0000E000
#define IRTE_REG_DEFAULT_VLAN_TAG_P3_SHFT_DEFAULT_VLAN_PRIORITAET                                            13
// ACCESS__DEFAULT_VLAN_PRIORITAET: r,w
// DESCR__DEFAULT_VLAN_PRIORITAET: Default VLAN-Priorität
#define IRTE_REG_DEFAULT_VLAN_TAG_P3__MSK_CFI_BIT                                                            0x00001000
#define IRTE_REG_DEFAULT_VLAN_TAG_P3_SHFT_CFI_BIT                                                            12
// ACCESS__CFI_BIT: r,w
// DESCR__CFI_BIT: : = 0 für IEEE 802.3
#define IRTE_REG_DEFAULT_VLAN_TAG_P3__MSK_DEFAULT_VLAN_ADDRESS                                               0x00000FFF
#define IRTE_REG_DEFAULT_VLAN_TAG_P3_SHFT_DEFAULT_VLAN_ADDRESS                                               0
// ACCESS__DEFAULT_VLAN_ADDRESS: r,w
// DESCR__DEFAULT_VLAN_ADDRESS: Default VLAN-Adresse
#define IRTE_REG_DEFAULT_VLAN_TAG_P3_RST__VAL                          0x00000000


#define IRTE_REG_CLK_CONTROL                                                                             0x00010000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT7                                                                      0x00000080
#define IRTE_REG_CLK_CONTROL_SHFT_PORT7                                                                      7
// ACCESS__PORT7: r,w
// DESCR__PORT7: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT6                                                                      0x00000040
#define IRTE_REG_CLK_CONTROL_SHFT_PORT6                                                                      6
// ACCESS__PORT6: r,w
// DESCR__PORT6: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT5                                                                      0x00000020
#define IRTE_REG_CLK_CONTROL_SHFT_PORT5                                                                      5
// ACCESS__PORT5: r,w
// DESCR__PORT5: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT4                                                                      0x00000010
#define IRTE_REG_CLK_CONTROL_SHFT_PORT4                                                                      4
// ACCESS__PORT4: r,w
// DESCR__PORT4: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT3                                                                      0x00000008
#define IRTE_REG_CLK_CONTROL_SHFT_PORT3                                                                      3
// ACCESS__PORT3: r,w
// DESCR__PORT3: 
#define IRTE_REG_CLK_CONTROL__MSK_PORT2                                                                      0x00000004
#define IRTE_REG_CLK_CONTROL_SHFT_PORT2                                                                      2
// ACCESS__PORT2: r,w
// DESCR__PORT2: ...
#define IRTE_REG_CLK_CONTROL__MSK_PORT1                                                                      0x00000002
#define IRTE_REG_CLK_CONTROL_SHFT_PORT1                                                                      1
// ACCESS__PORT1: r,w
// DESCR__PORT1: Port 1 ..
#define IRTE_REG_CLK_CONTROL__MSK_PORT0                                                                      0x00000001
#define IRTE_REG_CLK_CONTROL_SHFT_PORT0                                                                      0
// ACCESS__PORT0: r,w
// DESCR__PORT0: Port 0 : Taktversorgung schalten,   Bit 0  = 1  :  Takt Port 0 aus,   Default   = eingeschaltet
#define IRTE_REG_CLK_CONTROL_RST__VAL                                  0x00000000


#define IRTE_REG_APLCLK_DIVIDER                                                                          0x00011000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_APLCLK_DIVIDER__MSK_APLCLK_DIVIDER                                                          0x0000FFFF
#define IRTE_REG_APLCLK_DIVIDER_SHFT_APLCLK_DIVIDER                                                          0
// ACCESS__APLCLK_DIVIDER: r,w
// DESCR__APLCLK_DIVIDER: Anzahl der Clkcy-Takte pro AplClk-Taktperiode
#define IRTE_REG_APLCLK_DIVIDER_RST__VAL                               0x00000000


#define IRTE_REG_CLKCY_DELAY                                                                             0x00011004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLKCY_DELAY__MSK_CLKCY_DELAY                                                                0x0000FFFF
#define IRTE_REG_CLKCY_DELAY_SHFT_CLKCY_DELAY                                                                0
// ACCESS__CLKCY_DELAY: r,w
// DESCR__CLKCY_DELAY: Zeitliche Verzögerung der Signale Clkcy und AplClk gegenüber dem Zyklusbeginn in 100 MHz-Takten
#define IRTE_REG_CLKCY_DELAY_RST__VAL                                  0x00000000


#define IRTE_REG_CLKCY_INT_DELAY                                                                         0x00011008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLKCY_INT_DELAY__MSK_CLKCY_INT_DELAY                                                        0x0000FFFF
#define IRTE_REG_CLKCY_INT_DELAY_SHFT_CLKCY_INT_DELAY                                                        0
// ACCESS__CLKCY_INT_DELAY: r,w
// DESCR__CLKCY_INT_DELAY: Zeitliche Verzögerung des Signals Clkcy_int gegenüber dem Zyklusbeginn in 100 MHz-Takten
#define IRTE_REG_CLKCY_INT_DELAY_RST__VAL                              0x00000000


#define IRTE_REG_CLKCY_INT_DIVIDER                                                                       0x0001100C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLKCY_INT_DIVIDER__MSK_CLKCY_INT_DIVIDER                                                    0x0000FFFF
#define IRTE_REG_CLKCY_INT_DIVIDER_SHFT_CLKCY_INT_DIVIDER                                                    0
// ACCESS__CLKCY_INT_DIVIDER: r,w
// DESCR__CLKCY_INT_DIVIDER: Anzahl der Clkcy-Takte pro Clkcy_Int-Taktperiode
#define IRTE_REG_CLKCY_INT_DIVIDER_RST__VAL                            0x00000000


#define IRTE_REG_CLKCY_PERIOD                                                                            0x00011010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLKCY_PERIOD__MSK_CLKCY_PERIOD                                                              0x0000FFFF
#define IRTE_REG_CLKCY_PERIOD_SHFT_CLKCY_PERIOD                                                              0
// ACCESS__CLKCY_PERIOD: r,w
// DESCR__CLKCY_PERIOD: Periodendauer von Clkcy in 100 MHz-Takten. Dies muß eine gerade Zahl sein, da der Takt ein Tastverhältnis von 1:1 besitzen soll. Als Minimalwert kann hier „2“ eingetragen werden, der Clkcy hat dann eine Frequenz von 50 MHz.
#define IRTE_REG_CLKCY_PERIOD_RST__VAL                                 0x00000000


#define IRTE_REG_CLKGEN_COMMAND                                                                          0x00011014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CLKGEN_COMMAND__MSK_ENABLE_MULTICYCLE_APL_CLOCKS                                            0x00000004
#define IRTE_REG_CLKGEN_COMMAND_SHFT_ENABLE_MULTICYCLE_APL_CLOCKS                                            2
// ACCESS__ENABLE_MULTICYCLE_APL_CLOCKS: r,w
// DESCR__ENABLE_MULTICYCLE_APL_CLOCKS: Enable_Multicycle_Apl_Clocks  = 1: Für AplClk und Clkcy_Int sind Periodendauern länger als ein Zyklus einstellbar\nEnable_Multicycle_Apl_Clocks  = 0: AplClk- und Clkcy_Int-Zähler werden am Zyklusende immer zurückgesetzt
#define IRTE_REG_CLKGEN_COMMAND__MSK_APLCLK_START                                                            0x00000002
#define IRTE_REG_CLKGEN_COMMAND_SHFT_APLCLK_START                                                            1
// ACCESS__APLCLK_START: r,w
// DESCR__APLCLK_START: AplClk_Start: Anforderung zur Generierung des AplClk und Clkcy_Int\nAplClk_Start  = 1: Start\nAplClk_Start  = 0: Stop
#define IRTE_REG_CLKGEN_COMMAND__MSK_CLKCY_START                                                             0x00000001
#define IRTE_REG_CLKGEN_COMMAND_SHFT_CLKCY_START                                                             0
// ACCESS__CLKCY_START: r,w
// DESCR__CLKCY_START: Clkcy_Start: Anforderung zur Generierung des Clkcy\nClkcy_Start  = 1: Start\nClkcy_Start  = 0: Stop
#define IRTE_REG_CLKGEN_COMMAND_RST__VAL                               0x00000000


#define IRTE_REG_COMPARATOR_VALUE_1                                                                      0x00011018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_COMPARATOR_VALUE_1__MSK_COMPARATOR_VAL_1                                                    0xFFFFFFFF
#define IRTE_REG_COMPARATOR_VALUE_1_SHFT_COMPARATOR_VAL_1                                                    0
// ACCESS__COMPARATOR_VAL_1: r,w
// DESCR__COMPARATOR_VAL_1: 
#define IRTE_REG_COMPARATOR_VALUE_1_RST__VAL                           0x00000000


#define IRTE_REG_COMPARATOR_VALUE_2                                                                      0x0001101C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_COMPARATOR_VALUE_2__MSK_COMPARATOR_VAL_2                                                    0xFFFFFFFF
#define IRTE_REG_COMPARATOR_VALUE_2_SHFT_COMPARATOR_VAL_2                                                    0
// ACCESS__COMPARATOR_VAL_2: r,w
// DESCR__COMPARATOR_VAL_2: 
#define IRTE_REG_COMPARATOR_VALUE_2_RST__VAL                           0x00000000


#define IRTE_REG_COMPARATOR_VALUE_3                                                                      0x00011020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_COMPARATOR_VALUE_3__MSK_COMPARATOR_VAL_3                                                    0xFFFFFFFF
#define IRTE_REG_COMPARATOR_VALUE_3_SHFT_COMPARATOR_VAL_3                                                    0
// ACCESS__COMPARATOR_VAL_3: r,w
// DESCR__COMPARATOR_VAL_3: 
#define IRTE_REG_COMPARATOR_VALUE_3_RST__VAL                           0x00000000


#define IRTE_REG_COMPARATOR_VALUE_4                                                                      0x00011024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_COMPARATOR_VALUE_4__MSK_COMPARATOR_VAL_4                                                    0xFFFFFFFF
#define IRTE_REG_COMPARATOR_VALUE_4_SHFT_COMPARATOR_VAL_4                                                    0
// ACCESS__COMPARATOR_VAL_4: r,w
// DESCR__COMPARATOR_VAL_4: 
#define IRTE_REG_COMPARATOR_VALUE_4_RST__VAL                           0x00000000


#define IRTE_REG_COMPARATOR_VALUE_5                                                                      0x00011028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_COMPARATOR_VALUE_5__MSK_COMPARATOR_VAL_5                                                    0xFFFFFFFF
#define IRTE_REG_COMPARATOR_VALUE_5_SHFT_COMPARATOR_VAL_5                                                    0
// ACCESS__COMPARATOR_VAL_5: r,w
// DESCR__COMPARATOR_VAL_5: 
#define IRTE_REG_COMPARATOR_VALUE_5_RST__VAL                           0x00000000


#define IRTE_REG_CYCLE_LENGTH                                                                            0x0001102C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_CYCLE_LENGTH__MSK_CYCLE_LENGTH                                                              0xFFFFFFFF
#define IRTE_REG_CYCLE_LENGTH_SHFT_CYCLE_LENGTH                                                              0
// ACCESS__CYCLE_LENGTH: r,w
// DESCR__CYCLE_LENGTH: Länge des geregelten Isochronzyklus in Anzahl von 100 MHz Takten
#define IRTE_REG_CYCLE_LENGTH_RST__VAL                                 0x00000000


#define IRTE_REG_MUX_SELEKTOR                                                                            0x00011030
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT5                                                                 0x00F00000
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT5                                                                 20
// ACCESS__MUX_PORT5: r,w
// DESCR__MUX_PORT5: Wert Beschreibung\n0000 Fester Wert 1 (Resetzustand)\n0001 Fester Wert 0\n0010 Fester Wert 1\n0011 Fester Wert 1\n0100 Signal AplClk\n0101 Signal Clkcy\n0110 Signal Clkcy_Int\n0111 Signal PLL_EXT_OUT (entspricht AplClk ohne Delay)\n1000 Interrupt Comp1 (low-aktiver Impuls von 340ns)\n1001 Interrupt Comp2 (low-aktiver Impuls von 340ns)\n1010 Interrupt Comp3 (low-aktiver Impuls von 340ns)\n1011 Interrupt Comp4 (low-aktiver Impuls von 340ns)\n1100 Interrupt Comp5 (low-aktiver Impuls von 340ns)\n1101 Fester Wert 1\n1110 Fester Wert 1\n1111 Fester Wert 1
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT4                                                                 0x000F0000
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT4                                                                 16
// ACCESS__MUX_PORT4: r,w
// DESCR__MUX_PORT4: 
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT3                                                                 0x0000F000
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT3                                                                 12
// ACCESS__MUX_PORT3: r,w
// DESCR__MUX_PORT3: 
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT2                                                                 0x00000E00
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT2                                                                 9
// ACCESS__MUX_PORT2: r,w
// DESCR__MUX_PORT2: 
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT1                                                                 0x000000F0
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT1                                                                 4
// ACCESS__MUX_PORT1: r,w
// DESCR__MUX_PORT1: 
#define IRTE_REG_MUX_SELEKTOR__MSK_MUX_PORT0                                                                 0x0000000F
#define IRTE_REG_MUX_SELEKTOR_SHFT_MUX_PORT0                                                                 0
// ACCESS__MUX_PORT0: r,w
// DESCR__MUX_PORT0: 
#define IRTE_REG_MUX_SELEKTOR_RST__VAL                                 0x00000000


#define IRTE_REG_IRT_STARTTIME_SND_P0                                                                    0x0000B000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_SND_P0__MSK_IRT_STARTTIME_SND_P0                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_SND_P0_SHFT_IRT_STARTTIME_SND_P0                                              0
// ACCESS__IRT_STARTTIME_SND_P0: r,w
// DESCR__IRT_STARTTIME_SND_P0: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Sendeeinheit von Port 0.
#define IRTE_REG_IRT_STARTTIME_SND_P0_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_RCV_P0                                                                    0x0000B004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_RCV_P0__MSK_IRT_STARTTIME_RCV_P0                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_RCV_P0_SHFT_IRT_STARTTIME_RCV_P0                                              0
// ACCESS__IRT_STARTTIME_RCV_P0: r,w
// DESCR__IRT_STARTTIME_RCV_P0: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Empfangseinheit von Port 0.
#define IRTE_REG_IRT_STARTTIME_RCV_P0_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_SND_P1                                                                    0x0000B008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_SND_P1__MSK_IRT_STARTTIME_SND_P1                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_SND_P1_SHFT_IRT_STARTTIME_SND_P1                                              0
// ACCESS__IRT_STARTTIME_SND_P1: r,w
// DESCR__IRT_STARTTIME_SND_P1: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Sendeeinheit von Port 1.
#define IRTE_REG_IRT_STARTTIME_SND_P1_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_RCV_P1                                                                    0x0000B00C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_RCV_P1__MSK_IRT_STARTTIME_RCV_P1                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_RCV_P1_SHFT_IRT_STARTTIME_RCV_P1                                              0
// ACCESS__IRT_STARTTIME_RCV_P1: r,w
// DESCR__IRT_STARTTIME_RCV_P1: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Empfangseinheit von Port 1.
#define IRTE_REG_IRT_STARTTIME_RCV_P1_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_SND_P2                                                                    0x0000B010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_SND_P2__MSK_IRT_STARTTIME_SND_P2                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_SND_P2_SHFT_IRT_STARTTIME_SND_P2                                              0
// ACCESS__IRT_STARTTIME_SND_P2: r,w
// DESCR__IRT_STARTTIME_SND_P2: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Sendeeinheit von Port 2.
#define IRTE_REG_IRT_STARTTIME_SND_P2_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_RCV_P2                                                                    0x0000B014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_RCV_P2__MSK_IRT_STARTTIME_RCV_P2                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_RCV_P2_SHFT_IRT_STARTTIME_RCV_P2                                              0
// ACCESS__IRT_STARTTIME_RCV_P2: r,w
// DESCR__IRT_STARTTIME_RCV_P2: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Empfangseinheit von Port 2.
#define IRTE_REG_IRT_STARTTIME_RCV_P2_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_SND_P3                                                                    0x0000B018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_SND_P3__MSK_IRT_STARTTIME_SND_P3                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_SND_P3_SHFT_IRT_STARTTIME_SND_P3                                              0
// ACCESS__IRT_STARTTIME_SND_P3: r,w
// DESCR__IRT_STARTTIME_SND_P3: Vergleichwerts zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Sendeeinheit von Port 3.
#define IRTE_REG_IRT_STARTTIME_SND_P3_RST__VAL                         0x00000000


#define IRTE_REG_IRT_STARTTIME_RCV_P3                                                                    0x0000B01C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_STARTTIME_RCV_P3__MSK_IRT_STARTTIME_RCV_P3                                              0xFFFFFFFF
#define IRTE_REG_IRT_STARTTIME_RCV_P3_SHFT_IRT_STARTTIME_RCV_P3                                              0
// ACCESS__IRT_STARTTIME_RCV_P3: r,w
// DESCR__IRT_STARTTIME_RCV_P3: Vergleichswert zum Cycle-Timer. Zu diesem Wert beginnt die IRT-Kommunikation der Empfangseinheit von Port 3.
#define IRTE_REG_IRT_STARTTIME_RCV_P3_RST__VAL                         0x00000000


#define IRTE_REG_NRT_MAPPING_CHA                                                                         0x00012000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_MAPPING_CHA__MSK_NRT_MAPPING_CHA                                                        0x000003FF
#define IRTE_REG_NRT_MAPPING_CHA_SHFT_NRT_MAPPING_CHA                                                        0
// ACCESS__NRT_MAPPING_CHA: r,w
// DESCR__NRT_MAPPING_CHA: Bit-Maske für die Zuordnung der Teilwarteschlangen (Prioritäten) eines Kanals A zu eine der beiden Schnittstellen (SS0 oder SS1).\n0:  Diese Teilwarteschlange ist Schnittstelle 1 zugeordnet\n1:  Diese Teilwarteschlange ist Schnittstelle 0 zugeordnet
#define IRTE_REG_NRT_MAPPING_CHA_RST__VAL                              0x00000000


#define IRTE_REG_NRT_MAPPING_CHB                                                                         0x00012004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_MAPPING_CHB__MSK_NRT_MAPPING_CHB                                                        0x000003FF
#define IRTE_REG_NRT_MAPPING_CHB_SHFT_NRT_MAPPING_CHB                                                        0
// ACCESS__NRT_MAPPING_CHB: r,w
// DESCR__NRT_MAPPING_CHB: Bit-Maske für die Zuordnung der Teilwarteschlangen (Prioritäten) eines Kanals B zu eine der beiden Schnittstellen (SS0 oder SS1).\n0:  Diese Teilwarteschlange ist Schnittstelle 1 zugeordnet\n1:  Diese Teilwarteschlange ist Schnittstelle 0 zugeordnet
#define IRTE_REG_NRT_MAPPING_CHB_RST__VAL                              0x00000000


#define IRTE_REG_NRT_ENABLE_CHA0                                                                         0x00012400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_ENABLE_CHA0__MSK_RX_ENABLE_CHA0                                                         0x00000004
#define IRTE_REG_NRT_ENABLE_CHA0_SHFT_RX_ENABLE_CHA0                                                         2
// ACCESS__RX_ENABLE_CHA0: r,w
// DESCR__RX_ENABLE_CHA0: write:‚0‘: keine Zustandsänderung‚1‘: Freigabe / Sperren des Empfangens an Kanal A, SS0read:‚0‘: Empfangen an SS0 von Kanal A ist gesperrt‚1‘: Empfangen an SS0 von Kanal A ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHA0__MSK_TX_ENABLE_CHA0                                                         0x00000002
#define IRTE_REG_NRT_ENABLE_CHA0_SHFT_TX_ENABLE_CHA0                                                         1
// ACCESS__TX_ENABLE_CHA0: r,w
// DESCR__TX_ENABLE_CHA0: write:‚0‘: keine Zustandsänderung ‚1‘: Freigabe / Sperren des Sendens an Kanal A, SS0read:‚0‘: Senden an SS0 von Kanal A ist gesperrt‚1‘: Senden an SS0 von Kanal A ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHA0__MSK_SET_RESET                                                              0x00000001
#define IRTE_REG_NRT_ENABLE_CHA0_SHFT_SET_RESET                                                              0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten Enable-Bits werden rückgesetzt (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten Enable-Bits werden gesetzt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_NRT_ENABLE_CHA0_RST__VAL                              0x00000000


#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA0                                                                0x00012404
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA0__MSK_NRT_SEND_DESCRIPTOR_CHA0                                      0xFFFFFFFF
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA0_SHFT_NRT_SEND_DESCRIPTOR_CHA0                                      0
// ACCESS__NRT_SEND_DESCRIPTOR_CHA0: r,w
// DESCR__NRT_SEND_DESCRIPTOR_CHA0: Übergabe-Register der hochprioren SS des NRT-API von Kanal A, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA0_RST__VAL                     0x00000000


#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA0                                                             0x00012408
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA0__MSK_NRT_RECEIVE_DESCRIPTOR_CHA0                                0xFFFFFFFF
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA0_SHFT_NRT_RECEIVE_DESCRIPTOR_CHA0                                0
// ACCESS__NRT_RECEIVE_DESCRIPTOR_CHA0: r,w
// DESCR__NRT_RECEIVE_DESCRIPTOR_CHA0: Übergabe-Register der SS0 des NRT-API von Kanal A, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA0_RST__VAL                  0x00000000


#define IRTE_REG_NRT_ENABLE_CHA1                                                                         0x0001240C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_ENABLE_CHA1__MSK_RX_ENABLE_CHA1                                                         0x00000004
#define IRTE_REG_NRT_ENABLE_CHA1_SHFT_RX_ENABLE_CHA1                                                         2
// ACCESS__RX_ENABLE_CHA1: r,w
// DESCR__RX_ENABLE_CHA1: write:‚0‘: keine Zustandsänderung‚1‘: Freigabe / Sperren des Empfangens an Kanal A, SS1read:‚0‘: Empfangen an SS1 von Kanal A ist gesperrt‚1‘: Empfangen an SS1 von Kanal A ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHA1__MSK_TX_ENABLE_CHA1                                                         0x00000002
#define IRTE_REG_NRT_ENABLE_CHA1_SHFT_TX_ENABLE_CHA1                                                         1
// ACCESS__TX_ENABLE_CHA1: r,w
// DESCR__TX_ENABLE_CHA1: write:‚0‘: keine Zustandsänderung ‚1‘: Freigabe / Sperren des Sendens an Kanal A, SS1read:‚0‘: Senden an SS1 von Kanal A ist gesperrt‚1‘: Senden an SS1 von Kanal A ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHA1__MSK_SET_RESET                                                              0x00000001
#define IRTE_REG_NRT_ENABLE_CHA1_SHFT_SET_RESET                                                              0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten Enable-Bits werden rückgesetzt (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten Enable-Bits werden gesetzt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_NRT_ENABLE_CHA1_RST__VAL                              0x00000000


#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA1                                                                0x00012410
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA1__MSK_NRT_SEND_DESCRIPTOR_CHA1                                      0xFFFFFFFF
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA1_SHFT_NRT_SEND_DESCRIPTOR_CHA1                                      0
// ACCESS__NRT_SEND_DESCRIPTOR_CHA1: r,w
// DESCR__NRT_SEND_DESCRIPTOR_CHA1: Übergabe-Register der niederprioren SS des NRT-API von Kanal A, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHA1_RST__VAL                     0x00000000


#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA1                                                             0x00012414
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA1__MSK_NRT_RECEIVE_DESCRIPTOR_CHA1                                0xFFFFFFFF
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA1_SHFT_NRT_RECEIVE_DESCRIPTOR_CHA1                                0
// ACCESS__NRT_RECEIVE_DESCRIPTOR_CHA1: r,w
// DESCR__NRT_RECEIVE_DESCRIPTOR_CHA1: Übergabe-Register der SS1 des NRT-API von Kanal A, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHA1_RST__VAL                  0x00000000


#define IRTE_REG_NRT_ENABLE_CHB0                                                                         0x00012418
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_ENABLE_CHB0__MSK_RX_ENABLE_CHB0                                                         0x00000004
#define IRTE_REG_NRT_ENABLE_CHB0_SHFT_RX_ENABLE_CHB0                                                         2
// ACCESS__RX_ENABLE_CHB0: r,w
// DESCR__RX_ENABLE_CHB0: write:‚0‘: keine Zustandsänderung‚1‘: Freigabe / Sperren des Empfangens an Kanal B, SS0read:‚0‘: Empfangen an SS0 von Kanal B ist gesperrt‚1‘: Empfangen an SS0 von Kanal B ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHB0__MSK_TX_ENABLE_CHB0                                                         0x00000002
#define IRTE_REG_NRT_ENABLE_CHB0_SHFT_TX_ENABLE_CHB0                                                         1
// ACCESS__TX_ENABLE_CHB0: r,w
// DESCR__TX_ENABLE_CHB0: write:‚0‘: keine Zustandsänderung ‚1‘: Freigabe / Sperren des Sendens an Kanal B, SS0read:‚0‘: Senden an SS0 von Kanal B ist gesperrt‚1‘: Senden an SS0 von Kanal B ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHB0__MSK_SET_RESET                                                              0x00000001
#define IRTE_REG_NRT_ENABLE_CHB0_SHFT_SET_RESET                                                              0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten Enable-Bits werden rückgesetzt (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten Enable-Bits werden gesetzt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_NRT_ENABLE_CHB0_RST__VAL                              0x00000000


#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB0                                                                0x0001241C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB0__MSK_NRT_SEND_DESCRIPTOR_CHB0                                      0xFFFFFFFF
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB0_SHFT_NRT_SEND_DESCRIPTOR_CHB0                                      0
// ACCESS__NRT_SEND_DESCRIPTOR_CHB0: r,w
// DESCR__NRT_SEND_DESCRIPTOR_CHB0: Übergabe-Register der hochprioren SS des NRT-API von Kanal B, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB0_RST__VAL                     0x00000000


#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB0                                                             0x00012420
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB0__MSK_NRT_RECEIVE_DESCRIPTOR_CHB0                                0xFFFFFFFF
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB0_SHFT_NRT_RECEIVE_DESCRIPTOR_CHB0                                0
// ACCESS__NRT_RECEIVE_DESCRIPTOR_CHB0: r,w
// DESCR__NRT_RECEIVE_DESCRIPTOR_CHB0: Übergabe-Register der SS0 des NRT-API von Kanal B, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB0_RST__VAL                  0x00000000


#define IRTE_REG_NRT_ENABLE_CHB1                                                                         0x00012424
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_ENABLE_CHB1__MSK_RX_ENABLE_CHB1                                                         0x00000004
#define IRTE_REG_NRT_ENABLE_CHB1_SHFT_RX_ENABLE_CHB1                                                         2
// ACCESS__RX_ENABLE_CHB1: r,w
// DESCR__RX_ENABLE_CHB1: write:‚0‘: keine Zustandsänderung‚1‘: Freigabe / Sperren des Empfangens an Kanal B, SS1read:‚0‘: Empfangen an SS1 von Kanal B ist gesperrt‚1‘: Empfangen an SS1 von Kanal B ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHB1__MSK_TX_ENABLE_CHB1                                                         0x00000002
#define IRTE_REG_NRT_ENABLE_CHB1_SHFT_TX_ENABLE_CHB1                                                         1
// ACCESS__TX_ENABLE_CHB1: r,w
// DESCR__TX_ENABLE_CHB1: write:‚0‘: keine Zustandsänderung ‚1‘: Freigabe / Sperren des Sendens an Kanal B, SS1read:‚0‘: Senden an SS1 von Kanal B ist gesperrt‚1‘: Senden an SS1 von Kanal B ist freigegeben
#define IRTE_REG_NRT_ENABLE_CHB1__MSK_SET_RESET                                                              0x00000001
#define IRTE_REG_NRT_ENABLE_CHB1_SHFT_SET_RESET                                                              0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten Enable-Bits werden rückgesetzt (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten Enable-Bits werden gesetzt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_NRT_ENABLE_CHB1_RST__VAL                              0x00000000


#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB1                                                                0x00012428
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB1__MSK_NRT_SEND_DESCRIPTOR_CHB1                                      0xFFFFFFFF
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB1_SHFT_NRT_SEND_DESCRIPTOR_CHB1                                      0
// ACCESS__NRT_SEND_DESCRIPTOR_CHB1: r,w
// DESCR__NRT_SEND_DESCRIPTOR_CHB1: Übergabe-Register der niederprioren SS des NRT-API von Kanal B, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_SEND_DESCRIPTOR_CHB1_RST__VAL                     0x00000000


#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB1                                                             0x0001242C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB1__MSK_NRT_RECEIVE_DESCRIPTOR_CHB1                                0xFFFFFFFF
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB1_SHFT_NRT_RECEIVE_DESCRIPTOR_CHB1                                0
// ACCESS__NRT_RECEIVE_DESCRIPTOR_CHB1: r,w
// DESCR__NRT_RECEIVE_DESCRIPTOR_CHB1: Übergabe-Register der SS1 des NRT-API von Kanal A, Hierüber wird der Zeiger auf die ersten DMACW-Struktur übergeben. Hinweis: Eine nicht 8-Byte aligned geschriebene Adresse wird nur 8-Byte aligned übernommen und kann nur 8-Byte aligned zurückgelesen werden.
#define IRTE_REG_NRT_RECEIVE_DESCRIPTOR_CHB1_RST__VAL                  0x00000000


#define IRTE_REG_IRT_CONTROL                                                                             0x00013000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_CONTROL__MSK_IRT_RCV_EN_PORT3                                                           0x00000080
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_RCV_EN_PORT3                                                           7
// ACCESS__IRT_RCV_EN_PORT3: r,w
// DESCR__IRT_RCV_EN_PORT3: Freigabe der IRT-Empfangsliste an Port 3
#define IRTE_REG_IRT_CONTROL__MSK_IRT_SND_EN_PORT3                                                           0x00000040
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_SND_EN_PORT3                                                           6
// ACCESS__IRT_SND_EN_PORT3: r,w
// DESCR__IRT_SND_EN_PORT3: Freigabe der IRT-Sendeliste an Port 3
#define IRTE_REG_IRT_CONTROL__MSK_IRT_RCV_EN_PORT2                                                           0x00000020
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_RCV_EN_PORT2                                                           5
// ACCESS__IRT_RCV_EN_PORT2: r,w
// DESCR__IRT_RCV_EN_PORT2: Freigabe der IRT-Empfangsliste an Port 2
#define IRTE_REG_IRT_CONTROL__MSK_IRT_SND_EN_PORT2                                                           0x00000010
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_SND_EN_PORT2                                                           4
// ACCESS__IRT_SND_EN_PORT2: r,w
// DESCR__IRT_SND_EN_PORT2: Freigabe der IRT-Sendeliste an Port 2
#define IRTE_REG_IRT_CONTROL__MSK_IRT_RCV_EN_PORT1                                                           0x00000008
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_RCV_EN_PORT1                                                           3
// ACCESS__IRT_RCV_EN_PORT1: r,w
// DESCR__IRT_RCV_EN_PORT1: Freigabe der IRT-Empfangsliste an Port 1
#define IRTE_REG_IRT_CONTROL__MSK_IRT_SND_EN_PORT1                                                           0x00000004
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_SND_EN_PORT1                                                           2
// ACCESS__IRT_SND_EN_PORT1: r,w
// DESCR__IRT_SND_EN_PORT1: Freigabe der IRT-Sendeliste an Port 1
#define IRTE_REG_IRT_CONTROL__MSK_IRT_RCV_EN_PORT0                                                           0x00000002
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_RCV_EN_PORT0                                                           1
// ACCESS__IRT_RCV_EN_PORT0: r,w
// DESCR__IRT_RCV_EN_PORT0: Freigabe der IRT-Empfangsliste an Port 0
#define IRTE_REG_IRT_CONTROL__MSK_IRT_SND_EN_PORT0                                                           0x00000001
#define IRTE_REG_IRT_CONTROL_SHFT_IRT_SND_EN_PORT0                                                           0
// ACCESS__IRT_SND_EN_PORT0: r,w
// DESCR__IRT_SND_EN_PORT0: Freigabe der IRT-Sendeliste an Port 0
#define IRTE_REG_IRT_CONTROL_RST__VAL                                  0x00000000


#define IRTE_REG_IMAGEMODE                                                                               0x00013004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IMAGEMODE__MSK_ENABLE_ASYNCDMA                                                              0x00000020
#define IRTE_REG_IMAGEMODE_SHFT_ENABLE_ASYNCDMA                                                              5
// ACCESS__ENABLE_ASYNCDMA: r,w
// DESCR__ENABLE_ASYNCDMA: ‚0’: Beim Synchron-Buffered Interface wird der DMA nicht-quittiert    gestartet. Dies ist die Standardeinstellung für synchrone     Applikationen.‚1’: Beim Synchron-Buffered Interface wird der DMA quittiert     gestartet. Diese Einstellung ist notwendig, wenn neben der     synchronen Applikation auch eine asynchrone Applikation auf    das Synchron-Buffered Interface arbeitet.
#define IRTE_REG_IMAGEMODE__MSK_ENABLE_STANDBY                                                               0x00000010
#define IRTE_REG_IMAGEMODE_SHFT_ENABLE_STANDBY                                                               4
// ACCESS__ENABLE_STANDBY: r,w
// DESCR__ENABLE_STANDBY: ‚0’: Betrieb ohne StandBy-Master Bedeutung des Bit 3 (StandBy-Bit) im Datenstatus des Telegramms ist don’t care.‚1’: Betrieb mit StandBy-Master Nur dann wenn Bit 3 im Datenstatus  = ‚0’ erfolgt der Abbild-Transfer ins K-RAM.
#define IRTE_REG_IMAGEMODE__MSK_SYCAPI_START                                                                 0x0000000C
#define IRTE_REG_IMAGEMODE_SHFT_SYCAPI_START                                                                 2
// ACCESS__SYCAPI_START: r,w
// DESCR__SYCAPI_START: Das synchrone API wird gestartet bei Ende von   ‚00‘: Minimum TimeCompare oder Ende IRT-Kommunikation mit           Transferüberwachung   ‚01‘: Minimum TimeCompare oder Ende iSRT-Kommunikation            mit Transferüberwachung   ‚10‘: TimeCompare mit Transferüberwachung   ‚11‘: TimeCompare ohne Transferüberwachung
#define IRTE_REG_IMAGEMODE__MSK_BUFFER_MODE                                                                  0x00000003
#define IRTE_REG_IMAGEMODE_SHFT_BUFFER_MODE                                                                  0
// ACCESS__BUFFER_MODE: r,w
// DESCR__BUFFER_MODE: ‚00‘: kein Mode eingestellt‚01‘: synchron ungepuffert‚10‘: synchron gepuffert‚11‘: synchron Wechselpuffer
#define IRTE_REG_IMAGEMODE_RST__VAL                                    0x00000000


#define IRTE_REG_IRT_SYNCMODE                                                                            0x00013400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_SYNCMODE__MSK_DMA_INENABLE                                                              0x00000004
#define IRTE_REG_IRT_SYNCMODE_SHFT_DMA_INENABLE                                                              2
// ACCESS__DMA_INENABLE: r,w
// DESCR__DMA_INENABLE: Dieses Bit hat nur dann eine Bedeutung, wenn im „ImageMode_Register“ das Bit Enable_AsyncDMA =1 gesetzt ist.‚0’: Der DMA-Transfer der Eingangsdaten darf nicht gestartet      werden. Die asynchrone Applikation muss noch gestoppt      werden.‚1’: Der DMA-Transfer der Eingangsdaten wird gestartet.      Die asynchrone Applikation wurde gestoppt.
#define IRTE_REG_IRT_SYNCMODE__MSK_IRT_OUTDONE                                                               0x00000002
#define IRTE_REG_IRT_SYNCMODE_SHFT_IRT_OUTDONE                                                               1
// ACCESS__IRT_OUTDONE: r,w
// DESCR__IRT_OUTDONE: Die Bearbeitung der Ausgangsdaten durch die synchrone Anwendung ist abgeschlossen
#define IRTE_REG_IRT_SYNCMODE__MSK_IRT_INDONE                                                                0x00000001
#define IRTE_REG_IRT_SYNCMODE_SHFT_IRT_INDONE                                                                0
// ACCESS__IRT_INDONE: r,w
// DESCR__IRT_INDONE: Die Bearbeitung der Eingangsdaten durch die snychrone Anwendung ist abgeschlossen
#define IRTE_REG_IRT_SYNCMODE_RST__VAL                                 0x00000000


#define IRTE_REG_DATA_UPDATE                                                                             0x00013404
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_DATA_UPDATE__MSK_ENABLE_COUNTER                                                             0x00010000
#define IRTE_REG_DATA_UPDATE_SHFT_ENABLE_COUNTER                                                             16
// ACCESS__ENABLE_COUNTER: r,w
// DESCR__ENABLE_COUNTER: write:‚1’: Freigabe des Zählers zum nächsten Zyklus‚0’: Sperren des Zählers zum nächsten Zyklusread:‚1’: Zähler ist freigegeben‚0’: Zähler ist gesperrt
#define IRTE_REG_DATA_UPDATE__MSK_COUNTER                                                                    0x0000FFFF
#define IRTE_REG_DATA_UPDATE_SHFT_COUNTER                                                                    0
// ACCESS__COUNTER: r,w
// DESCR__COUNTER: write:not implementedread:Anzahl der Updates der Eingangsdatensätze eines synchronen Abbilds
#define IRTE_REG_DATA_UPDATE_RST__VAL                                  0x00000000


#define IRTE_REG_IRT_TRANSFER_ERR                                                                        0x00013408
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_IRT_TRANSFER_ERR__MSK_IRT_TRANSFER_ERR                                                      0x000000FF
#define IRTE_REG_IRT_TRANSFER_ERR_SHFT_IRT_TRANSFER_ERR                                                      0
// ACCESS__IRT_TRANSFER_ERR: r,
// DESCR__IRT_TRANSFER_ERR: read:Anzahl der fehlerhaften Transfers für einen Datensatz dieses Knoten. Der Registerinhalt wird beim Auslesen zurückgesetzt.
#define IRTE_REG_IRT_TRANSFER_ERR_RST__VAL                             0x00000000


#define IRTE_REG_DMA_SWITCHADR                                                                           0x0001340C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_DMA_SWITCHADR__MSK_SWITCH_OUT                                                               0x00000002
#define IRTE_REG_DMA_SWITCHADR_SHFT_SWITCH_OUT                                                               1
// ACCESS__SWITCH_OUT: r,
// DESCR__SWITCH_OUT: ‚0‘: Die gültigen Ausgangsdaten werden vom Ausgangsdatenpuffer mit der Adresse im Register DMAInDestAdr0 kopiert.‚1‘: Die gültigen Ausgangsdaten werden vom Ausgangsdatenpuffer mit der Adresse im Regsiter DMAInDestAdr1 kopiert.
#define IRTE_REG_DMA_SWITCHADR__MSK_SWITCH_IN                                                                0x00000001
#define IRTE_REG_DMA_SWITCHADR_SHFT_SWITCH_IN                                                                0
// ACCESS__SWITCH_IN: r,
// DESCR__SWITCH_IN: ‚0‘: Die gültigen Eingangsdaten wurden auf den Eingangsdatenpuffer mit der Adresse im Register DMAInDestAdr0 kopiert.‚1‘: Die gültigen Eingangsdaten wurden auf den Eingangsdatenpuffer mit der Adresse im Regsiter DMAInDestAdr1 kopiert.
#define IRTE_REG_DMA_SWITCHADR_RST__VAL                                0x00000000


#define IRTE_REG_APPL_WATCHDOG                                                                           0x00013410
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_APPL_WATCHDOG__MSK_APPL_WATCHDOG                                                            0x000000FF
#define IRTE_REG_APPL_WATCHDOG_SHFT_APPL_WATCHDOG                                                            0
// ACCESS__APPL_WATCHDOG: r,w
// DESCR__APPL_WATCHDOG: Überwachung der asynchronen Applikation, läuft der Watchdog ab, werden die asynchronen Abbild-Daten als inkonsistent versendet. Der Watchdog wird mit jedem Zyklus um 1 dekrementiert. Wird der Watchdog mit FFh geladen, so unterbleibt die Überwachung
#define IRTE_REG_APPL_WATCHDOG_RST__VAL                                0x0000000F


#define IRTE_REG_PHY_CMD_P0                                                                              0x00015010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_CMD_P0__MSK_PREAMBLE_SUPPRESS                                                           0x00000040
#define IRTE_REG_PHY_CMD_P0_SHFT_PREAMBLE_SUPPRESS                                                           6
// ACCESS__PREAMBLE_SUPPRESS: r,w
// DESCR__PREAMBLE_SUPPRESS: 0: Die Präambel wird zum PHY gesendet.1: Die Präambel wird nicht zum PHY gesendet.
#define IRTE_REG_PHY_CMD_P0__MSK_ENABLE                                                                      0x00000020
#define IRTE_REG_PHY_CMD_P0_SHFT_ENABLE                                                                      5
// ACCESS__ENABLE: r,w
// DESCR__ENABLE: 0: Der Status dieses PHYs (Ports) wird nicht gepollt1: Der Status dieses PHYs (Ports) wird gepollt
#define IRTE_REG_PHY_CMD_P0__MSK_PHY_ADDRESS                                                                 0x0000001F
#define IRTE_REG_PHY_CMD_P0_SHFT_PHY_ADDRESS                                                                 0
// ACCESS__PHY_ADDRESS: r,w
// DESCR__PHY_ADDRESS: Adresse des PHYs (Ports) des Status Register gepollt werden soll
#define IRTE_REG_PHY_CMD_P0_RST__VAL                                   0x00000000


#define IRTE_REG_PHY_STAT_P0                                                                             0x00015014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_STAT_P0__MSK_100BASE_T4                                                                 0x00008000
#define IRTE_REG_PHY_STAT_P0_SHFT_100BASE_T4                                                                 15
// ACCESS__100BASE_T4: r,w
// DESCR__100BASE_T4: 0: PHY not able to perform 100BASE-T41: PHY able to perform 100BASE-T4
#define IRTE_REG_PHY_STAT_P0__MSK_100BASE_X_FULL_DUPLEX                                                      0x00004000
#define IRTE_REG_PHY_STAT_P0_SHFT_100BASE_X_FULL_DUPLEX                                                      14
// ACCESS__100BASE_X_FULL_DUPLEX: r,w
// DESCR__100BASE_X_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-X1: PHY able to perform full duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P0__MSK_100BASE_X_HALF_DUPLEX                                                      0x00002000
#define IRTE_REG_PHY_STAT_P0_SHFT_100BASE_X_HALF_DUPLEX                                                      13
// ACCESS__100BASE_X_HALF_DUPLEX: r,w
// DESCR__100BASE_X_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-X1: PHY able to perform half duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P0__MSK_10_MB_S_FULL_DUPLEX                                                        0x00001000
#define IRTE_REG_PHY_STAT_P0_SHFT_10_MB_S_FULL_DUPLEX                                                        12
// ACCESS__10_MB_S_FULL_DUPLEX: r,w
// DESCR__10_MB_S_FULL_DUPLEX: 0: PHY not able to operate at 10 Mb/s in full duplex mode1: PHY able to operate at 10 Mb/s in full duplex mode
#define IRTE_REG_PHY_STAT_P0__MSK_10_MB_S_HALF_DUPLEX                                                        0x00000800
#define IRTE_REG_PHY_STAT_P0_SHFT_10_MB_S_HALF_DUPLEX                                                        11
// ACCESS__10_MB_S_HALF_DUPLEX: r,w
// DESCR__10_MB_S_HALF_DUPLEX: 0: PHY not able to operate at 10 Mb/s in half duplex mode1: PHY able to operate at 10 Mb/s in half duplex mode
#define IRTE_REG_PHY_STAT_P0__MSK_100BASE_T2_FULL_DUPLEX                                                     0x00000400
#define IRTE_REG_PHY_STAT_P0_SHFT_100BASE_T2_FULL_DUPLEX                                                     10
// ACCESS__100BASE_T2_FULL_DUPLEX: r,w
// DESCR__100BASE_T2_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-T21: PHY able to perform full duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P0__MSK_100BASE_T2_HALF_DUPLEX                                                     0x00000200
#define IRTE_REG_PHY_STAT_P0_SHFT_100BASE_T2_HALF_DUPLEX                                                     9
// ACCESS__100BASE_T2_HALF_DUPLEX: r,w
// DESCR__100BASE_T2_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-T21: PHY able to perform half duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P0__MSK_EXTENDED_STATUS                                                            0x00000100
#define IRTE_REG_PHY_STAT_P0_SHFT_EXTENDED_STATUS                                                            8
// ACCESS__EXTENDED_STATUS: r,w
// DESCR__EXTENDED_STATUS: 0: No extended status information in Register 151: Extended status information in Register 15
#define IRTE_REG_PHY_STAT_P0__MSK_MF_PREAMBLE_SUPPRESSION                                                    0x00000040
#define IRTE_REG_PHY_STAT_P0_SHFT_MF_PREAMBLE_SUPPRESSION                                                    6
// ACCESS__MF_PREAMBLE_SUPPRESSION: r,w
// DESCR__MF_PREAMBLE_SUPPRESSION: 0: PHY will not accept management frames with preamble suppressed.1: PHY will accept management frames with preamble suppressed.
#define IRTE_REG_PHY_STAT_P0__MSK_AUTO_NEGOTIATION_COMPLETE                                                  0x00000020
#define IRTE_REG_PHY_STAT_P0_SHFT_AUTO_NEGOTIATION_COMPLETE                                                  5
// ACCESS__AUTO_NEGOTIATION_COMPLETE: r,w
// DESCR__AUTO_NEGOTIATION_COMPLETE: 0: Auto-Negotiation process not completed1: Auto-Negotiation process completed
#define IRTE_REG_PHY_STAT_P0__MSK_REMOTE_FAULT                                                               0x00000010
#define IRTE_REG_PHY_STAT_P0_SHFT_REMOTE_FAULT                                                               4
// ACCESS__REMOTE_FAULT: r,w
// DESCR__REMOTE_FAULT: 0: no remote fault condition detected1: remote fault condition detected
#define IRTE_REG_PHY_STAT_P0__MSK_AUTO_NEGOTIATION_ABILITY                                                   0x00000008
#define IRTE_REG_PHY_STAT_P0_SHFT_AUTO_NEGOTIATION_ABILITY                                                   3
// ACCESS__AUTO_NEGOTIATION_ABILITY: r,w
// DESCR__AUTO_NEGOTIATION_ABILITY: 0: PHY is not able to perform Auto-Negotiation1: PHY is able to perform Auto-Negotiation
#define IRTE_REG_PHY_STAT_P0__MSK_LINK_STATUS                                                                0x00000004
#define IRTE_REG_PHY_STAT_P0_SHFT_LINK_STATUS                                                                2
// ACCESS__LINK_STATUS: r,w
// DESCR__LINK_STATUS: 0: link is down1: link is up
#define IRTE_REG_PHY_STAT_P0__MSK_JABBER_DETECT                                                              0x00000002
#define IRTE_REG_PHY_STAT_P0_SHFT_JABBER_DETECT                                                              1
// ACCESS__JABBER_DETECT: r,w
// DESCR__JABBER_DETECT: 0: no jabber condition detected1: jabber condition detected
#define IRTE_REG_PHY_STAT_P0__MSK_EXTENDED_CAPABILITY                                                        0x00000001
#define IRTE_REG_PHY_STAT_P0_SHFT_EXTENDED_CAPABILITY                                                        0
// ACCESS__EXTENDED_CAPABILITY: r,w
// DESCR__EXTENDED_CAPABILITY: 0: basic register set capabilities only1: extended register capabilities
#define IRTE_REG_PHY_STAT_P0_RST__VAL                                  0x00000000


#define IRTE_REG_PHY_CMD_P1                                                                              0x00015018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_CMD_P1__MSK_PREAMBLE_SUPPRESS                                                           0x00000040
#define IRTE_REG_PHY_CMD_P1_SHFT_PREAMBLE_SUPPRESS                                                           6
// ACCESS__PREAMBLE_SUPPRESS: r,w
// DESCR__PREAMBLE_SUPPRESS: 0: Die Präambel wird zum PHY gesendet.1: Die Präambel wird nicht zum PHY gesendet.
#define IRTE_REG_PHY_CMD_P1__MSK_ENABLE                                                                      0x00000020
#define IRTE_REG_PHY_CMD_P1_SHFT_ENABLE                                                                      5
// ACCESS__ENABLE: r,w
// DESCR__ENABLE: 0: Der Status dieses PHYs (Ports) wird nicht gepollt1: Der Status dieses PHYs (Ports) wird gepollt
#define IRTE_REG_PHY_CMD_P1__MSK_PHY_ADDRESS                                                                 0x0000001F
#define IRTE_REG_PHY_CMD_P1_SHFT_PHY_ADDRESS                                                                 0
// ACCESS__PHY_ADDRESS: r,w
// DESCR__PHY_ADDRESS: Adresse des PHYs (Ports) des Status Register gepollt werden soll
#define IRTE_REG_PHY_CMD_P1_RST__VAL                                   0x00000000


#define IRTE_REG_PHY_STAT_P1                                                                             0x0001501C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_STAT_P1__MSK_100BASE_T4                                                                 0x00008000
#define IRTE_REG_PHY_STAT_P1_SHFT_100BASE_T4                                                                 15
// ACCESS__100BASE_T4: r,w
// DESCR__100BASE_T4: 0: PHY not able to perform 100BASE-T41: PHY able to perform 100BASE-T4
#define IRTE_REG_PHY_STAT_P1__MSK_100BASE_X_FULL_DUPLEX                                                      0x00004000
#define IRTE_REG_PHY_STAT_P1_SHFT_100BASE_X_FULL_DUPLEX                                                      14
// ACCESS__100BASE_X_FULL_DUPLEX: r,w
// DESCR__100BASE_X_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-X1: PHY able to perform full duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P1__MSK_100BASE_X_HALF_DUPLEX                                                      0x00002000
#define IRTE_REG_PHY_STAT_P1_SHFT_100BASE_X_HALF_DUPLEX                                                      13
// ACCESS__100BASE_X_HALF_DUPLEX: r,w
// DESCR__100BASE_X_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-X1: PHY able to perform half duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P1__MSK_10_MB_S_FULL_DUPLEX                                                        0x00001000
#define IRTE_REG_PHY_STAT_P1_SHFT_10_MB_S_FULL_DUPLEX                                                        12
// ACCESS__10_MB_S_FULL_DUPLEX: r,w
// DESCR__10_MB_S_FULL_DUPLEX: 0: PHY not able to operate at 10 Mb/s in full duplex mode1: PHY able to operate at 10 Mb/s in full duplex mode
#define IRTE_REG_PHY_STAT_P1__MSK_10_MB_S_HALF_DUPLEX                                                        0x00000800
#define IRTE_REG_PHY_STAT_P1_SHFT_10_MB_S_HALF_DUPLEX                                                        11
// ACCESS__10_MB_S_HALF_DUPLEX: r,w
// DESCR__10_MB_S_HALF_DUPLEX: 0: PHY not able to operate at 10 Mb/s in half duplex mode1: PHY able to operate at 10 Mb/s in half duplex mode
#define IRTE_REG_PHY_STAT_P1__MSK_100BASE_T2_FULL_DUPLEX                                                     0x00000400
#define IRTE_REG_PHY_STAT_P1_SHFT_100BASE_T2_FULL_DUPLEX                                                     10
// ACCESS__100BASE_T2_FULL_DUPLEX: r,w
// DESCR__100BASE_T2_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-T21: PHY able to perform full duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P1__MSK_100BASE_T2_HALF_DUPLEX                                                     0x00000200
#define IRTE_REG_PHY_STAT_P1_SHFT_100BASE_T2_HALF_DUPLEX                                                     9
// ACCESS__100BASE_T2_HALF_DUPLEX: r,w
// DESCR__100BASE_T2_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-T21: PHY able to perform half duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P1__MSK_EXTENDED_STATUS                                                            0x00000100
#define IRTE_REG_PHY_STAT_P1_SHFT_EXTENDED_STATUS                                                            8
// ACCESS__EXTENDED_STATUS: r,w
// DESCR__EXTENDED_STATUS: 0: No extended status information in Register 151: Extended status information in Register 15
#define IRTE_REG_PHY_STAT_P1__MSK_MF_PREAMBLE_SUPPRESSION                                                    0x00000040
#define IRTE_REG_PHY_STAT_P1_SHFT_MF_PREAMBLE_SUPPRESSION                                                    6
// ACCESS__MF_PREAMBLE_SUPPRESSION: r,w
// DESCR__MF_PREAMBLE_SUPPRESSION: 0: PHY will not accept management frames with preamble suppressed.1: PHY will accept management frames with preamble suppressed.
#define IRTE_REG_PHY_STAT_P1__MSK_AUTO_NEGOTIATION_COMPLETE                                                  0x00000020
#define IRTE_REG_PHY_STAT_P1_SHFT_AUTO_NEGOTIATION_COMPLETE                                                  5
// ACCESS__AUTO_NEGOTIATION_COMPLETE: r,w
// DESCR__AUTO_NEGOTIATION_COMPLETE: 0: Auto-Negotiation process not completed1: Auto-Negotiation process completed
#define IRTE_REG_PHY_STAT_P1__MSK_REMOTE_FAULT                                                               0x00000010
#define IRTE_REG_PHY_STAT_P1_SHFT_REMOTE_FAULT                                                               4
// ACCESS__REMOTE_FAULT: r,w
// DESCR__REMOTE_FAULT: 0: no remote fault condition detected1: remote fault condition detected
#define IRTE_REG_PHY_STAT_P1__MSK_AUTO_NEGOTIATION_ABILITY                                                   0x00000008
#define IRTE_REG_PHY_STAT_P1_SHFT_AUTO_NEGOTIATION_ABILITY                                                   3
// ACCESS__AUTO_NEGOTIATION_ABILITY: r,w
// DESCR__AUTO_NEGOTIATION_ABILITY: 0: PHY is not able to perform Auto-Negotiation1: PHY is able to perform Auto-Negotiation
#define IRTE_REG_PHY_STAT_P1__MSK_LINK_STATUS                                                                0x00000004
#define IRTE_REG_PHY_STAT_P1_SHFT_LINK_STATUS                                                                2
// ACCESS__LINK_STATUS: r,w
// DESCR__LINK_STATUS: 0: link is down1: link is up
#define IRTE_REG_PHY_STAT_P1__MSK_JABBER_DETECT                                                              0x00000002
#define IRTE_REG_PHY_STAT_P1_SHFT_JABBER_DETECT                                                              1
// ACCESS__JABBER_DETECT: r,w
// DESCR__JABBER_DETECT: 0: no jabber condition detected1: jabber condition detected
#define IRTE_REG_PHY_STAT_P1__MSK_EXTENDED_CAPABILITY                                                        0x00000001
#define IRTE_REG_PHY_STAT_P1_SHFT_EXTENDED_CAPABILITY                                                        0
// ACCESS__EXTENDED_CAPABILITY: r,w
// DESCR__EXTENDED_CAPABILITY: 0: basic register set capabilities only1: extended register capabilities
#define IRTE_REG_PHY_STAT_P1_RST__VAL                                  0x00000000


#define IRTE_REG_PHY_CMD_P2                                                                              0x00015020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_CMD_P2__MSK_PREAMBLE_SUPPRESS                                                           0x00000040
#define IRTE_REG_PHY_CMD_P2_SHFT_PREAMBLE_SUPPRESS                                                           6
// ACCESS__PREAMBLE_SUPPRESS: r,w
// DESCR__PREAMBLE_SUPPRESS: 0: Die Präambel wird zum PHY gesendet.1: Die Präambel wird nicht zum PHY gesendet.
#define IRTE_REG_PHY_CMD_P2__MSK_ENABLE                                                                      0x00000020
#define IRTE_REG_PHY_CMD_P2_SHFT_ENABLE                                                                      5
// ACCESS__ENABLE: r,w
// DESCR__ENABLE: 0: Der Status dieses PHYs (Ports) wird nicht gepollt1: Der Status dieses PHYs (Ports) wird gepollt
#define IRTE_REG_PHY_CMD_P2__MSK_PHY_ADDRESS                                                                 0x0000001F
#define IRTE_REG_PHY_CMD_P2_SHFT_PHY_ADDRESS                                                                 0
// ACCESS__PHY_ADDRESS: r,w
// DESCR__PHY_ADDRESS: Adresse des PHYs (Ports) des Status Register gepollt werden soll
#define IRTE_REG_PHY_CMD_P2_RST__VAL                                   0x00000000


#define IRTE_REG_PHY_STAT_P2                                                                             0x00015024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_STAT_P2__MSK_100BASE_T4                                                                 0x00008000
#define IRTE_REG_PHY_STAT_P2_SHFT_100BASE_T4                                                                 15
// ACCESS__100BASE_T4: r,w
// DESCR__100BASE_T4: 0: PHY not able to perform 100BASE-T41: PHY able to perform 100BASE-T4
#define IRTE_REG_PHY_STAT_P2__MSK_100BASE_X_FULL_DUPLEX                                                      0x00004000
#define IRTE_REG_PHY_STAT_P2_SHFT_100BASE_X_FULL_DUPLEX                                                      14
// ACCESS__100BASE_X_FULL_DUPLEX: r,w
// DESCR__100BASE_X_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-X1: PHY able to perform full duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P2__MSK_100BASE_X_HALF_DUPLEX                                                      0x00002000
#define IRTE_REG_PHY_STAT_P2_SHFT_100BASE_X_HALF_DUPLEX                                                      13
// ACCESS__100BASE_X_HALF_DUPLEX: r,w
// DESCR__100BASE_X_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-X1: PHY able to perform half duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P2__MSK_10_MB_S_FULL_DUPLEX                                                        0x00001000
#define IRTE_REG_PHY_STAT_P2_SHFT_10_MB_S_FULL_DUPLEX                                                        12
// ACCESS__10_MB_S_FULL_DUPLEX: r,w
// DESCR__10_MB_S_FULL_DUPLEX: 0: PHY not able to operate at 10 Mb/s in full duplex mode1: PHY able to operate at 10 Mb/s in full duplex mode
#define IRTE_REG_PHY_STAT_P2__MSK_10_MB_S_HALF_DUPLEX                                                        0x00000800
#define IRTE_REG_PHY_STAT_P2_SHFT_10_MB_S_HALF_DUPLEX                                                        11
// ACCESS__10_MB_S_HALF_DUPLEX: r,w
// DESCR__10_MB_S_HALF_DUPLEX: 0: PHY not able to operate at 10 Mb/s in half duplex mode1: PHY able to operate at 10 Mb/s in half duplex mode
#define IRTE_REG_PHY_STAT_P2__MSK_100BASE_T2_FULL_DUPLEX                                                     0x00000400
#define IRTE_REG_PHY_STAT_P2_SHFT_100BASE_T2_FULL_DUPLEX                                                     10
// ACCESS__100BASE_T2_FULL_DUPLEX: r,w
// DESCR__100BASE_T2_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-T21: PHY able to perform full duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P2__MSK_100BASE_T2_HALF_DUPLEX                                                     0x00000200
#define IRTE_REG_PHY_STAT_P2_SHFT_100BASE_T2_HALF_DUPLEX                                                     9
// ACCESS__100BASE_T2_HALF_DUPLEX: r,w
// DESCR__100BASE_T2_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-T21: PHY able to perform half duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P2__MSK_EXTENDED_STATUS                                                            0x00000100
#define IRTE_REG_PHY_STAT_P2_SHFT_EXTENDED_STATUS                                                            8
// ACCESS__EXTENDED_STATUS: r,w
// DESCR__EXTENDED_STATUS: 0: No extended status information in Register 151: Extended status information in Register 15
#define IRTE_REG_PHY_STAT_P2__MSK_MF_PREAMBLE_SUPPRESSION                                                    0x00000040
#define IRTE_REG_PHY_STAT_P2_SHFT_MF_PREAMBLE_SUPPRESSION                                                    6
// ACCESS__MF_PREAMBLE_SUPPRESSION: r,w
// DESCR__MF_PREAMBLE_SUPPRESSION: 0: PHY will not accept management frames with preamble suppressed.1: PHY will accept management frames with preamble suppressed.
#define IRTE_REG_PHY_STAT_P2__MSK_AUTO_NEGOTIATION_COMPLETE                                                  0x00000020
#define IRTE_REG_PHY_STAT_P2_SHFT_AUTO_NEGOTIATION_COMPLETE                                                  5
// ACCESS__AUTO_NEGOTIATION_COMPLETE: r,w
// DESCR__AUTO_NEGOTIATION_COMPLETE: 0: Auto-Negotiation process not completed1: Auto-Negotiation process completed
#define IRTE_REG_PHY_STAT_P2__MSK_REMOTE_FAULT                                                               0x00000010
#define IRTE_REG_PHY_STAT_P2_SHFT_REMOTE_FAULT                                                               4
// ACCESS__REMOTE_FAULT: r,w
// DESCR__REMOTE_FAULT: 0: no remote fault condition detected1: remote fault condition detected
#define IRTE_REG_PHY_STAT_P2__MSK_AUTO_NEGOTIATION_ABILITY                                                   0x00000008
#define IRTE_REG_PHY_STAT_P2_SHFT_AUTO_NEGOTIATION_ABILITY                                                   3
// ACCESS__AUTO_NEGOTIATION_ABILITY: r,w
// DESCR__AUTO_NEGOTIATION_ABILITY: 0: PHY is not able to perform Auto-Negotiation1: PHY is able to perform Auto-Negotiation
#define IRTE_REG_PHY_STAT_P2__MSK_LINK_STATUS                                                                0x00000004
#define IRTE_REG_PHY_STAT_P2_SHFT_LINK_STATUS                                                                2
// ACCESS__LINK_STATUS: r,w
// DESCR__LINK_STATUS: 0: link is down1: link is up
#define IRTE_REG_PHY_STAT_P2__MSK_JABBER_DETECT                                                              0x00000002
#define IRTE_REG_PHY_STAT_P2_SHFT_JABBER_DETECT                                                              1
// ACCESS__JABBER_DETECT: r,w
// DESCR__JABBER_DETECT: 0: no jabber condition detected1: jabber condition detected
#define IRTE_REG_PHY_STAT_P2__MSK_EXTENDED_CAPABILITY                                                        0x00000001
#define IRTE_REG_PHY_STAT_P2_SHFT_EXTENDED_CAPABILITY                                                        0
// ACCESS__EXTENDED_CAPABILITY: r,w
// DESCR__EXTENDED_CAPABILITY: 0: basic register set capabilities only1: extended register capabilities
#define IRTE_REG_PHY_STAT_P2_RST__VAL                                  0x00000000


#define IRTE_REG_PHY_CMD_P3                                                                              0x00015028
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_CMD_P3__MSK_PREAMBLE_SUPPRESS                                                           0x00000040
#define IRTE_REG_PHY_CMD_P3_SHFT_PREAMBLE_SUPPRESS                                                           6
// ACCESS__PREAMBLE_SUPPRESS: r,w
// DESCR__PREAMBLE_SUPPRESS: 0: Die Präambel wird zum PHY gesendet.1: Die Präambel wird nicht zum PHY gesendet.
#define IRTE_REG_PHY_CMD_P3__MSK_ENABLE                                                                      0x00000020
#define IRTE_REG_PHY_CMD_P3_SHFT_ENABLE                                                                      5
// ACCESS__ENABLE: r,w
// DESCR__ENABLE: 0: Der Status dieses PHYs (Ports) wird nicht gepollt1: Der Status dieses PHYs (Ports) wird gepollt
#define IRTE_REG_PHY_CMD_P3__MSK_PHY_ADDRESS                                                                 0x0000001F
#define IRTE_REG_PHY_CMD_P3_SHFT_PHY_ADDRESS                                                                 0
// ACCESS__PHY_ADDRESS: r,w
// DESCR__PHY_ADDRESS: Adresse des PHYs (Ports) des Status Register gepollt werden soll
#define IRTE_REG_PHY_CMD_P3_RST__VAL                                   0x00000000


#define IRTE_REG_PHY_STAT_P3                                                                             0x0001502C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PHY_STAT_P3__MSK_100BASE_T4                                                                 0x00008000
#define IRTE_REG_PHY_STAT_P3_SHFT_100BASE_T4                                                                 15
// ACCESS__100BASE_T4: r,w
// DESCR__100BASE_T4: 0: PHY not able to perform 100BASE-T41: PHY able to perform 100BASE-T4
#define IRTE_REG_PHY_STAT_P3__MSK_100BASE_X_FULL_DUPLEX                                                      0x00004000
#define IRTE_REG_PHY_STAT_P3_SHFT_100BASE_X_FULL_DUPLEX                                                      14
// ACCESS__100BASE_X_FULL_DUPLEX: r,w
// DESCR__100BASE_X_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-X1: PHY able to perform full duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P3__MSK_100BASE_X_HALF_DUPLEX                                                      0x00002000
#define IRTE_REG_PHY_STAT_P3_SHFT_100BASE_X_HALF_DUPLEX                                                      13
// ACCESS__100BASE_X_HALF_DUPLEX: r,w
// DESCR__100BASE_X_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-X1: PHY able to perform half duplex 100BASE-X
#define IRTE_REG_PHY_STAT_P3__MSK_10_MB_S_FULL_DUPLEX                                                        0x00001000
#define IRTE_REG_PHY_STAT_P3_SHFT_10_MB_S_FULL_DUPLEX                                                        12
// ACCESS__10_MB_S_FULL_DUPLEX: r,w
// DESCR__10_MB_S_FULL_DUPLEX: 0: PHY not able to operate at 10 Mb/s in full duplex mode1: PHY able to operate at 10 Mb/s in full duplex mode
#define IRTE_REG_PHY_STAT_P3__MSK_10_MB_S_HALF_DUPLEX                                                        0x00000800
#define IRTE_REG_PHY_STAT_P3_SHFT_10_MB_S_HALF_DUPLEX                                                        11
// ACCESS__10_MB_S_HALF_DUPLEX: r,w
// DESCR__10_MB_S_HALF_DUPLEX: 0: PHY not able to operate at 10 Mb/s in half duplex mode1: PHY able to operate at 10 Mb/s in half duplex mode
#define IRTE_REG_PHY_STAT_P3__MSK_100BASE_T2_FULL_DUPLEX                                                     0x00000400
#define IRTE_REG_PHY_STAT_P3_SHFT_100BASE_T2_FULL_DUPLEX                                                     10
// ACCESS__100BASE_T2_FULL_DUPLEX: r,w
// DESCR__100BASE_T2_FULL_DUPLEX: 0: PHY not able to perform full duplex 100BASE-T21: PHY able to perform full duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P3__MSK_100BASE_T2_HALF_DUPLEX                                                     0x00000200
#define IRTE_REG_PHY_STAT_P3_SHFT_100BASE_T2_HALF_DUPLEX                                                     9
// ACCESS__100BASE_T2_HALF_DUPLEX: r,w
// DESCR__100BASE_T2_HALF_DUPLEX: 0: PHY not able to perform half duplex 100BASE-T21: PHY able to perform half duplex 100BASE-T2
#define IRTE_REG_PHY_STAT_P3__MSK_EXTENDED_STATUS                                                            0x00000100
#define IRTE_REG_PHY_STAT_P3_SHFT_EXTENDED_STATUS                                                            8
// ACCESS__EXTENDED_STATUS: r,w
// DESCR__EXTENDED_STATUS: 0: No extended status information in Register 151: Extended status information in Register 15
#define IRTE_REG_PHY_STAT_P3__MSK_MF_PREAMBLE_SUPPRESSION                                                    0x00000040
#define IRTE_REG_PHY_STAT_P3_SHFT_MF_PREAMBLE_SUPPRESSION                                                    6
// ACCESS__MF_PREAMBLE_SUPPRESSION: r,w
// DESCR__MF_PREAMBLE_SUPPRESSION: 0: PHY will not accept management frames with preamble suppressed.1: PHY will accept management frames with preamble suppressed.
#define IRTE_REG_PHY_STAT_P3__MSK_AUTO_NEGOTIATION_COMPLETE                                                  0x00000020
#define IRTE_REG_PHY_STAT_P3_SHFT_AUTO_NEGOTIATION_COMPLETE                                                  5
// ACCESS__AUTO_NEGOTIATION_COMPLETE: r,w
// DESCR__AUTO_NEGOTIATION_COMPLETE: 0: Auto-Negotiation process not completed1: Auto-Negotiation process completed
#define IRTE_REG_PHY_STAT_P3__MSK_REMOTE_FAULT                                                               0x00000010
#define IRTE_REG_PHY_STAT_P3_SHFT_REMOTE_FAULT                                                               4
// ACCESS__REMOTE_FAULT: r,w
// DESCR__REMOTE_FAULT: 0: no remote fault condition detected1: remote fault condition detected
#define IRTE_REG_PHY_STAT_P3__MSK_AUTO_NEGOTIATION_ABILITY                                                   0x00000008
#define IRTE_REG_PHY_STAT_P3_SHFT_AUTO_NEGOTIATION_ABILITY                                                   3
// ACCESS__AUTO_NEGOTIATION_ABILITY: r,w
// DESCR__AUTO_NEGOTIATION_ABILITY: 0: PHY is not able to perform Auto-Negotiation1: PHY is able to perform Auto-Negotiation
#define IRTE_REG_PHY_STAT_P3__MSK_LINK_STATUS                                                                0x00000004
#define IRTE_REG_PHY_STAT_P3_SHFT_LINK_STATUS                                                                2
// ACCESS__LINK_STATUS: r,w
// DESCR__LINK_STATUS: 0: link is down1: link is up
#define IRTE_REG_PHY_STAT_P3__MSK_JABBER_DETECT                                                              0x00000002
#define IRTE_REG_PHY_STAT_P3_SHFT_JABBER_DETECT                                                              1
// ACCESS__JABBER_DETECT: r,w
// DESCR__JABBER_DETECT: 0: no jabber condition detected1: jabber condition detected
#define IRTE_REG_PHY_STAT_P3__MSK_EXTENDED_CAPABILITY                                                        0x00000001
#define IRTE_REG_PHY_STAT_P3_SHFT_EXTENDED_CAPABILITY                                                        0
// ACCESS__EXTENDED_CAPABILITY: r,w
// DESCR__EXTENDED_CAPABILITY: 0: basic register set capabilities only1: extended register capabilities
#define IRTE_REG_PHY_STAT_P3_RST__VAL                                  0x00000000


#define IRTE_REG_MD_DATA                                                                                 0x00015000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MD_DATA__MSK_MD_DATA                                                                        0x0000FFFF
#define IRTE_REG_MD_DATA_SHFT_MD_DATA                                                                        0
// ACCESS__MD_DATA: r,w
// DESCR__MD_DATA: Schnittstellen-Register zwischen PHY und MAC.
#define IRTE_REG_MD_DATA_RST__VAL                                      0x00000000


#define IRTE_REG_MD_CA                                                                                   0x00015004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MD_CA__MSK_PREAMBLE_SUPPRESS                                                                0x00001000
#define IRTE_REG_MD_CA_SHFT_PREAMBLE_SUPPRESS                                                                12
// ACCESS__PREAMBLE_SUPPRESS: r,w
// DESCR__PREAMBLE_SUPPRESS: 1: die Preambel wird nicht zum PHY gesendet.
#define IRTE_REG_MD_CA__MSK_BUSY                                                                             0x00000800
#define IRTE_REG_MD_CA_SHFT_BUSY                                                                             11
// ACCESS__BUSY: r,w
// DESCR__BUSY: 0: wenn kein Schreib- oder Lesezugriff auf ein PHY-Register    ausgeführt wird.1: muss vor jedem Beginn eines Schreib- oder Lesezugriffs auf     ein PHY-Register gesetzt sein und anschliessend wieder    zurückgesetzt werden.
#define IRTE_REG_MD_CA__MSK_WRITE                                                                            0x00000400
#define IRTE_REG_MD_CA_SHFT_WRITE                                                                            10
// ACCESS__WRITE: r,w
// DESCR__WRITE: 0: Lesezugriff auf ein PHY-Register1: Schreibzugriff auf ein PHY-Register
#define IRTE_REG_MD_CA__MSK_PHY_ADDRESS                                                                      0x000003E0
#define IRTE_REG_MD_CA_SHFT_PHY_ADDRESS                                                                      5
// ACCESS__PHY_ADDRESS: r,w
// DESCR__PHY_ADDRESS: Adresse des PHYs, auf den zugegriffen werden soll.
#define IRTE_REG_MD_CA__MSK_ADDRESS                                                                          0x0000001F
#define IRTE_REG_MD_CA_SHFT_ADDRESS                                                                          0
// ACCESS__ADDRESS: r,w
// DESCR__ADDRESS: Adresse des PHY-Registers, das geschrieben oder gelesen werden soll.
#define IRTE_REG_MD_CA_RST__VAL                                        0x00000000


#define IRTE_REG_SMI_CONFIGURATION                                                                       0x00015008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_SMI_CONFIGURATION__MSK_SMI_BAUDRATEN                                                        0x00000003
#define IRTE_REG_SMI_CONFIGURATION_SHFT_SMI_BAUDRATEN                                                        0
// ACCESS__SMI_BAUDRATEN: r,w
// DESCR__SMI_BAUDRATEN: Einstellen der SMI-Baudrate:\n„00“:  5,56 MHz\n„01“:  3,57 MHz\n„10“:  1,67 MHz\n„11“:  1,19 MHz
#define IRTE_REG_SMI_CONFIGURATION_RST__VAL                            0x00000000


#define IRTE_REG_LINK_CHANGE                                                                             0x0001500C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_LINK_CHANGE__MSK_PORT3                                                                      0x00000008
#define IRTE_REG_LINK_CHANGE_SHFT_PORT3                                                                      3
// ACCESS__PORT3: r,
// DESCR__PORT3: ‚0‘: keine Änderung des Link-Status‚1‘: Der Link-Status an Port 3 hat sich geändert
#define IRTE_REG_LINK_CHANGE__MSK_PORT2                                                                      0x00000004
#define IRTE_REG_LINK_CHANGE_SHFT_PORT2                                                                      2
// ACCESS__PORT2: r,
// DESCR__PORT2: ‚0‘: keine Änderung des Link-Status‚1‘: Der Link-Status an Port 2 hat sich geändert
#define IRTE_REG_LINK_CHANGE__MSK_PORT1                                                                      0x00000002
#define IRTE_REG_LINK_CHANGE_SHFT_PORT1                                                                      1
// ACCESS__PORT1: r,
// DESCR__PORT1: ‚0‘: keine Änderung des Link-Status‚1‘: Der Link-Status an Port 1 hat sich geändert
#define IRTE_REG_LINK_CHANGE__MSK_PORT0                                                                      0x00000001
#define IRTE_REG_LINK_CHANGE_SHFT_PORT0                                                                      0
// ACCESS__PORT0: r,
// DESCR__PORT0: ‚0‘: keine Änderung des Link-Status‚1‘: Der Link-Status an Port 0 hat sich geändert
#define IRTE_REG_LINK_CHANGE_RST__VAL                                  0x00000000


#define IRTE_REG_MAC_CONTROL_P0                                                                          0x00015440
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MAC_CONTROL_P0__MSK_LINK_STATUS_10_MB_S                                                     0x00008000
#define IRTE_REG_MAC_CONTROL_P0_SHFT_LINK_STATUS_10_MB_S                                                     15
// ACCESS__LINK_STATUS_10_MB_S: r,w
// DESCR__LINK_STATUS_10_MB_S: Status der 10 Mb/s Verbindung
#define IRTE_REG_MAC_CONTROL_P0__MSK_ENABLE_MISSED_ROLL                                                      0x00002000
#define IRTE_REG_MAC_CONTROL_P0_SHFT_ENABLE_MISSED_ROLL                                                      13
// ACCESS__ENABLE_MISSED_ROLL: r,w
// DESCR__ENABLE_MISSED_ROLL: 1: Enable für Interrupt, wenn Missed Roll  = 1.
#define IRTE_REG_MAC_CONTROL_P0__MSK_MISSED_ROLL                                                             0x00000400
#define IRTE_REG_MAC_CONTROL_P0_SHFT_MISSED_ROLL                                                             10
// ACCESS__MISSED_ROLL: r,w
// DESCR__MISSED_ROLL: Statusbit (nur lesbar). 1: Bei einem Überlauf des Missed Error Counters in der MAC-Unit.Mit jedem Lesezugriff auf den Missed Error Counter wird dieses Bitwieder zurückgesetzt.
#define IRTE_REG_MAC_CONTROL_P0__MSK_LINK_CHANGED                                                            0x00000100
#define IRTE_REG_MAC_CONTROL_P0_SHFT_LINK_CHANGED                                                            8
// ACCESS__LINK_CHANGED: r,w
// DESCR__LINK_CHANGED: Interruptbit, das einen Link-Wechsel anzeigt. Dieses Bit wird auf denMAC-Ausgang Int_Link geführt.Ein Schreibzugriff mit Link changed : = 1 setzt dieses Link changed und den MAC-Ausgang Int_Link zurück.Ein Schreibzugriff mit Link changed : = 0 hat keine Wirkung.
#define IRTE_REG_MAC_CONTROL_P0__MSK_LOOP_10_MB_S                                                            0x00000080
#define IRTE_REG_MAC_CONTROL_P0_SHFT_LOOP_10_MB_S                                                            7
// ACCESS__LOOP_10_MB_S: r,w
// DESCR__LOOP_10_MB_S: 1: setzt den MAC-Ausgang Loop_10
#define IRTE_REG_MAC_CONTROL_P0__MSK_CONNECTION_MODE                                                         0x00000060
#define IRTE_REG_MAC_CONTROL_P0_SHFT_CONNECTION_MODE                                                         5
// ACCESS__CONNECTION_MODE: r,w
// DESCR__CONNECTION_MODE: Selektiert den Verbindungsmodus:00  = Automatic, (default)01  = Force 10-Mbit/s endec10  = Force MII (bestimmt durch MII Takt)
#define IRTE_REG_MAC_CONTROL_P0__MSK_MAC_LOOPBACK                                                            0x00000010
#define IRTE_REG_MAC_CONTROL_P0_SHFT_MAC_LOOPBACK                                                            4
// ACCESS__MAC_LOOPBACK: r,w
// DESCR__MAC_LOOPBACK: 1: Alle gesendeten Signale werden von der MAC-Empfangseinheit    wieder empfangen ohne die MAC zu verlassen.
#define IRTE_REG_MAC_CONTROL_P0__MSK_FULL_DUPLEX                                                             0x00000008
#define IRTE_REG_MAC_CONTROL_P0_SHFT_FULL_DUPLEX                                                             3
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: = 0: Halb-Duplex = 1: Voll-DuplexBit Full-Duplex ist lesbar und schreibbar.
#define IRTE_REG_MAC_CONTROL_P0__MSK_SOFTWARE_RESET                                                          0x00000004
#define IRTE_REG_MAC_CONTROL_P0_SHFT_SOFTWARE_RESET                                                          2
// ACCESS__SOFTWARE_RESET: r,w
// DESCR__SOFTWARE_RESET: 1: Reset für alle Ethernet MAC Steuerwerke und für die internen MAC-FIFOs
#define IRTE_REG_MAC_CONTROL_P0__MSK_HALT_IMMEDIATE                                                          0x00000002
#define IRTE_REG_MAC_CONTROL_P0_SHFT_HALT_IMMEDIATE                                                          1
// ACCESS__HALT_IMMEDIATE: r,w
// DESCR__HALT_IMMEDIATE: 1: Sende- und Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_MAC_CONTROL_P0__MSK_HALT_REQUEST                                                            0x00000001
#define IRTE_REG_MAC_CONTROL_P0_SHFT_HALT_REQUEST                                                            0
// ACCESS__HALT_REQUEST: r,w
// DESCR__HALT_REQUEST: 1: Sende- und Empfangseinheit abschalten.    Laufende Transfers werden noch beendet.
#define IRTE_REG_MAC_CONTROL_P0_RST__VAL                               0x00000000


#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0                                                  0x00015444
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0__MSK_COMPARE_ENABLE                                  0x00000010
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_SHFT_COMPARE_ENABLE                                  4
// ACCESS__COMPARE_ENABLE: r,w
// DESCR__COMPARE_ENABLE: 1: Die Destinationadresse mpfangener Telegramme wird mit den\n    gespeicherten Adressen im ARC-Speicher verglichen.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0__MSK_NEGATIVE_ARC                                    0x00000008
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_SHFT_NEGATIVE_ARC                                    3
// ACCESS__NEGATIVE_ARC: r,w
// DESCR__NEGATIVE_ARC: 0: akzeptiert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und filtert alle anderen Telegramme.\n1: filtert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und akzeptiert alle anderen Telegramme.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0__MSK_BROADCAST_ACCEPT                                0x00000004
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_SHFT_BROADCAST_ACCEPT                                2
// ACCESS__BROADCAST_ACCEPT: r,w
// DESCR__BROADCAST_ACCEPT: 1: Jedes Broadcast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0__MSK_GROUP_ACCEPT                                    0x00000002
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_SHFT_GROUP_ACCEPT                                    1
// ACCESS__GROUP_ACCEPT: r,w
// DESCR__GROUP_ACCEPT: 1: Jedes Multicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0__MSK_STATION_ACCEPT                                  0x00000001
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_SHFT_STATION_ACCEPT                                  0
// ACCESS__STATION_ACCEPT: r,w
// DESCR__STATION_ACCEPT: 1: Jedes Unicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P0_RST__VAL       0x00000000


#define IRTE_REG_TRANSMIT_CONTROL_P0                                                                     0x00015448
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_COMPLETION                                                  0x00004000
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_COMPLETION                                                  14
// ACCESS__ENABLE_COMPLETION: r,w
// DESCR__ENABLE_COMPLETION: 1: Interrupt-Enable, wenn die MAC-Unit ein Telegramm sendet    oder abbricht.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_TRANSMIT_PARITY                                             0x00002000
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_TRANSMIT_PARITY                                             13
// ACCESS__ENABLE_TRANSMIT_PARITY: r,w
// DESCR__ENABLE_TRANSMIT_PARITY: 1: Interrupt-Enable, wenn beim MAC-Transmit-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_LATE_COLLISION                                              0x00001000
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_LATE_COLLISION                                              12
// ACCESS__ENABLE_LATE_COLLISION: r,w
// DESCR__ENABLE_LATE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode nach 512 TBit eine    Kollision auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_EXCESSIVE_COLLISION                                         0x00000800
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_EXCESSIVE_COLLISION                                         11
// ACCESS__ENABLE_EXCESSIVE_COLLISION: r,w
// DESCR__ENABLE_EXCESSIVE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode beim selben    Telegramm mehr als 16 Kollisionen auftreten.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_LOST_CARRIER                                                0x00000400
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_LOST_CARRIER                                                10
// ACCESS__ENABLE_LOST_CARRIER: r,w
// DESCR__ENABLE_LOST_CARRIER: 1: Interrupt-Enable, wenn ein „Carrier Sense“ Fehler auftritt
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_EXCESSIVE_DEFERAL                                           0x00000200
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_EXCESSIVE_DEFERAL                                           9
// ACCESS__ENABLE_EXCESSIVE_DEFERAL: r,w
// DESCR__ENABLE_EXCESSIVE_DEFERAL: 1: Interrupt-Enable, wenn die maximale „Deferral-Time”     überschritten wird.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_ENABLE_UNDERRUN                                                    0x00000100
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_ENABLE_UNDERRUN                                                    8
// ACCESS__ENABLE_UNDERRUN: r,w
// DESCR__ENABLE_UNDERRUN: 1: Interrupt-Enable, bei einem MAC-Transmit-FIFO Underrun
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_MII_10_MB_S_MODE                                                   0x00000080
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_MII_10_MB_S_MODE                                                   7
// ACCESS__MII_10_MB_S_MODE: r,w
// DESCR__MII_10_MB_S_MODE: 1: Aktiviert SQE-Test im MII 10 Mb/s Mode (wenn MII_Conn  = 1)
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_SEND_PAUSE                                                         0x00000040
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_SEND_PAUSE                                                         6
// ACCESS__SEND_PAUSE: r,w
// DESCR__SEND_PAUSE: 1: Veranlasst das Senden eines Pause-Telegramms    Wird nach dem Versenden dieses Pause-Telegramms von der    MAC-Unit wieder zurückgesetzt.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_NO_EXCESSIVE_DEFER                                                 0x00000020
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_NO_EXCESSIVE_DEFER                                                 5
// ACCESS__NO_EXCESSIVE_DEFER: r,w
// DESCR__NO_EXCESSIVE_DEFER: 1: Keine Test auf “Excessive Deferral”
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_FAST_BACK_OFF_AND_COUNT                                            0x00000010
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_FAST_BACK_OFF_AND_COUNT                                            4
// ACCESS__FAST_BACK_OFF_AND_COUNT: r,w
// DESCR__FAST_BACK_OFF_AND_COUNT: 1: verwendet schnellere back-off Timer und Byte Counter    (nur zum Testen)
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_SUPPRESS_CRC                                                       0x00000008
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_SUPPRESS_CRC                                                       3
// ACCESS__SUPPRESS_CRC: r,w
// DESCR__SUPPRESS_CRC: 1: MAC-Unit hängt kein CRC an das Telegramm-Ende an
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_SUPPRESS_PADDING                                                   0x00000004
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_SUPPRESS_PADDING                                                   2
// ACCESS__SUPPRESS_PADDING: r,w
// DESCR__SUPPRESS_PADDING: 1: Kein Padding für Telegramme mit weniger als 64 Bytes
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_TRANSMIT_HALT_REQUEST                                              0x00000002
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_TRANSMIT_HALT_REQUEST                                              1
// ACCESS__TRANSMIT_HALT_REQUEST: r,w
// DESCR__TRANSMIT_HALT_REQUEST: 1: Sendeeinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_TRANSMIT_CONTROL_P0__MSK_TRANSMIT_ENABLE                                                    0x00000001
#define IRTE_REG_TRANSMIT_CONTROL_P0_SHFT_TRANSMIT_ENABLE                                                    0
// ACCESS__TRANSMIT_ENABLE: r,w
// DESCR__TRANSMIT_ENABLE: 0: Sendeeinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_TRANSMIT_CONTROL_P0_RST__VAL                          0x00000000


#define IRTE_REG_TRANSMIT_STATUS_P0                                                                      0x0001544C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_HOST_NOT_RESPONDING_TRANSMIT                                        0x00400000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_HOST_NOT_RESPONDING_TRANSMIT                                        22
// ACCESS__HOST_NOT_RESPONDING_TRANSMIT: r,
// DESCR__HOST_NOT_RESPONDING_TRANSMIT: 1: ?
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_PAUSE_TRANSMIT                                                      0x00200000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_PAUSE_TRANSMIT                                                      21
// ACCESS__PAUSE_TRANSMIT: r,
// DESCR__PAUSE_TRANSMIT: 1: MAC hat ein MAC Control PAUSE Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_MAC_CONTROL_TRANSMIT                                                0x00100000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_MAC_CONTROL_TRANSMIT                                                20
// ACCESS__MAC_CONTROL_TRANSMIT: r,
// DESCR__MAC_CONTROL_TRANSMIT: 1: MAC hat ein MAC Control Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_VLAN_TAGGED_TRANSMIT                                                0x00080000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_VLAN_TAGGED_TRANSMIT                                                19
// ACCESS__VLAN_TAGGED_TRANSMIT: r,
// DESCR__VLAN_TAGGED_TRANSMIT: 1: MAC hat ein VLAN tagged Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_BROADCAST_TRANSMIT                                                  0x00040000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_BROADCAST_TRANSMIT                                                  18
// ACCESS__BROADCAST_TRANSMIT: r,
// DESCR__BROADCAST_TRANSMIT: 1: MAC hat ein Broadcast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_MULTICAST_TRANSMIT                                                  0x00020000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_MULTICAST_TRANSMIT                                                  17
// ACCESS__MULTICAST_TRANSMIT: r,
// DESCR__MULTICAST_TRANSMIT: 1: MAC hat ein Multicast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_SIGNAL_QUALITY_ERROR                                                0x00010000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_SIGNAL_QUALITY_ERROR                                                16
// ACCESS__SIGNAL_QUALITY_ERROR: r,
// DESCR__SIGNAL_QUALITY_ERROR: 1: kein Empfang eines „heart beat signals“ am Telegramm-Ende
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_TRANSMISSION_HALTED                                                 0x00008000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_TRANSMISSION_HALTED                                                 15
// ACCESS__TRANSMISSION_HALTED: r,
// DESCR__TRANSMISSION_HALTED: 1: die Sendeeinheit wurde durch     „Transmit Enable : = 0“ oder „Transmit Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_COMPLETION                                                          0x00004000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_COMPLETION                                                          14
// ACCESS__COMPLETION: r,
// DESCR__COMPLETION: 1: die MAC-Unit hat ein Telegramm gesendet oder abgebrochen.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_TRANSMIT_PARITY_ERROR                                               0x00002000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_TRANSMIT_PARITY_ERROR                                               13
// ACCESS__TRANSMIT_PARITY_ERROR: r,
// DESCR__TRANSMIT_PARITY_ERROR: 1: beim MAC-Transmit-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_LATE_COLLISION                                                      0x00001000
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_LATE_COLLISION                                                      12
// ACCESS__LATE_COLLISION: r,
// DESCR__LATE_COLLISION: 1: im Halb-Duplexmode ist nach 512 TBit eine Kollision     aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_TRANSMIT_10_MB_S_STATUS                                             0x00000800
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_TRANSMIT_10_MB_S_STATUS                                             11
// ACCESS__TRANSMIT_10_MB_S_STATUS: r,
// DESCR__TRANSMIT_10_MB_S_STATUS: 1: wenn ein Telegramm über das 10-Mbit/s Interface gesendet     wird.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_LOST_CARRIER                                                        0x00000400
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_LOST_CARRIER                                                        10
// ACCESS__LOST_CARRIER: r,
// DESCR__LOST_CARRIER: 1: beim Senden ist ein „Carrier Sense“ Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_EXCESSIVE_DEFERAL                                                   0x00000200
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_EXCESSIVE_DEFERAL                                                   9
// ACCESS__EXCESSIVE_DEFERAL: r,
// DESCR__EXCESSIVE_DEFERAL: 1: die maximale „Deferral-Time” wurde überschritten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_UNDERRUN                                                            0x00000100
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_UNDERRUN                                                            8
// ACCESS__UNDERRUN: r,
// DESCR__UNDERRUN: 1: ein MAC-Transmit-FIFO Underrun ist aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_INTERRUPT_ON_TRANSMIT                                               0x00000080
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_INTERRUPT_ON_TRANSMIT                                               7
// ACCESS__INTERRUPT_ON_TRANSMIT: r,
// DESCR__INTERRUPT_ON_TRANSMIT: 1: wenn das Senden eines Telegramms einen Interrupt auslöst.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_PAUSED                                                              0x00000040
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_PAUSED                                                              6
// ACCESS__PAUSED: r,
// DESCR__PAUSED: 1: das nächste Telegramm muss die Pause-Zeit abwarten.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_TRANSMIT_DEFERRED                                                   0x00000020
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_TRANSMIT_DEFERRED                                                   5
// ACCESS__TRANSMIT_DEFERRED: r,
// DESCR__TRANSMIT_DEFERRED: 1: wenn ein zu sendendes Telegramm im Wartezustand ist, da     die Halb-Duplex Verbindung belegt ist.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_EXCESSIVE_COLLISION                                                 0x00000010
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_EXCESSIVE_COLLISION                                                 4
// ACCESS__EXCESSIVE_COLLISION: r,
// DESCR__EXCESSIVE_COLLISION: 1: wenn beim selben Telegramm mehr als 16 Kollisionen     aufgetreten sind.
#define IRTE_REG_TRANSMIT_STATUS_P0__MSK_TRANSMIT_COLLISION_COUNT                                            0x0000000F
#define IRTE_REG_TRANSMIT_STATUS_P0_SHFT_TRANSMIT_COLLISION_COUNT                                            0
// ACCESS__TRANSMIT_COLLISION_COUNT: r,
// DESCR__TRANSMIT_COLLISION_COUNT: Anzahl der Kollisionen beim Senden eines Telegramms.
#define IRTE_REG_TRANSMIT_STATUS_P0_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_CONTROL_P0                                                                      0x00015450
// ACCESS__: (r)(t)(s),(w)
// DESCR__: 
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_GOOD                                                         0x00004000
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_GOOD                                                         14
// ACCESS__ENABLE_GOOD: r,w
// DESCR__ENABLE_GOOD: 1: Interrupt-Enable, wenn ein Telegramm fehlerfrei empfangenwird.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_RECEIVE_PARITY                                               0x00002000
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_RECEIVE_PARITY                                               13
// ACCESS__ENABLE_RECEIVE_PARITY: r,w
// DESCR__ENABLE_RECEIVE_PARITY: 1: Interrupt-Enable, wenn beim MAC-Receive-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_LONG_ERROR                                                   0x00000800
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_LONG_ERROR                                                   11
// ACCESS__ENABLE_LONG_ERROR: r,w
// DESCR__ENABLE_LONG_ERROR: 1: Interrupt-Enable, beim Empfang von Telegrammen mit einer     Framelänge grösser als 1518 Bytes (bzw. 1522 Bytes für    VLAN), ausser das Bit „Long Enable“ ist gesetzt.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_OVERFLOW                                                     0x00000400
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_OVERFLOW                                                     10
// ACCESS__ENABLE_OVERFLOW: r,w
// DESCR__ENABLE_OVERFLOW: 1: Interrupt-Enable, bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_CRC_ERROR                                                    0x00000200
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_CRC_ERROR                                                    9
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Interrupt-Enable, bei einem CRC-Fehler oder wenn Rx_er : = 1vom PHY.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_ENABLE_ALIGNMENT                                                    0x00000100
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_ENABLE_ALIGNMENT                                                    8
// ACCESS__ENABLE_ALIGNMENT: r,w
// DESCR__ENABLE_ALIGNMENT: 1: Interrupt-Enable, wenn ein Telegramm mit einer Framelänge    empfangen wird, die kein vielfaches eines Bytes ist und dessen    CRC fehlerhaft ist.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_IGNORE_CRC_VALUE                                                    0x00000040
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_IGNORE_CRC_VALUE                                                    6
// ACCESS__IGNORE_CRC_VALUE: r,w
// DESCR__IGNORE_CRC_VALUE: 1: CRC-Check ist abgeschaltet.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_PASS_CONTROL                                                        0x00000020
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_PASS_CONTROL                                                        5
// ACCESS__PASS_CONTROL: packets,r
// DESCR__PASS_CONTROL: 
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_STRIP_CRC_VALUE                                                     0x00000010
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_STRIP_CRC_VALUE                                                     4
// ACCESS__STRIP_CRC_VALUE: r,w
// DESCR__STRIP_CRC_VALUE: 1: CRC-Check, aber CRC wird am MAC-Interface nicht übergeben.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_SHORT_ENABLE                                                        0x00000008
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_SHORT_ENABLE                                                        3
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    kleiner als 64 Bytes.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_LONG_ENABLE                                                         0x00000004
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_LONG_ENABLE                                                         2
// ACCESS__LONG_ENABLE: r,w
// DESCR__LONG_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_RECEIVE_HALT_REQUEST                                                0x00000002
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_RECEIVE_HALT_REQUEST                                                1
// ACCESS__RECEIVE_HALT_REQUEST: r,w
// DESCR__RECEIVE_HALT_REQUEST: 1: Empfangseinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_RECEIVE_CONTROL_P0__MSK_RECEIVE_ENABLE                                                      0x00000001
#define IRTE_REG_RECEIVE_CONTROL_P0_SHFT_RECEIVE_ENABLE                                                      0
// ACCESS__RECEIVE_ENABLE: r,w
// DESCR__RECEIVE_ENABLE: 0: Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_RECEIVE_CONTROL_P0_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_STATUS_P0                                                                       0x00015454
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_ARCENT                                                               0x3E000000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_ARCENT                                                               25
// ACCESS__ARCENT: r,
// DESCR__ARCENT: Offsetadresse im ARC-Speicher unter der die empfangene Adresse eingetragen ist.ARCEnt  = 11111, wenn empfangene Adresse nicht in der ARC-Unit gespeichert ist.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_ARCSTATUS                                                            0x01E00000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_ARCSTATUS                                                            21
// ACCESS__ARCSTATUS: r,
// DESCR__ARCSTATUS: Status der ARC-Unit
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_PAUSE_RECEIVED                                                       0x00100000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_PAUSE_RECEIVED                                                       20
// ACCESS__PAUSE_RECEIVED: r,
// DESCR__PAUSE_RECEIVED: 1: MAC hat ein MAC Control PAUSE Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_VLAN_RECEIVED                                                        0x00080000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_VLAN_RECEIVED                                                        19
// ACCESS__VLAN_RECEIVED: r,
// DESCR__VLAN_RECEIVED: 1: MAC hat ein VLAN tagged Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_BROADCAST_RECEIVED                                                   0x00040000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_BROADCAST_RECEIVED                                                   18
// ACCESS__BROADCAST_RECEIVED: r,
// DESCR__BROADCAST_RECEIVED: 1: MAC hat ein Broadcast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_MULTICAST_RECEIVED                                                   0x00020000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_MULTICAST_RECEIVED                                                   17
// ACCESS__MULTICAST_RECEIVED: r,
// DESCR__MULTICAST_RECEIVED: 1: MAC hat ein Multicast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_RECEPTION_HALTED                                                     0x00008000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_RECEPTION_HALTED                                                     15
// ACCESS__RECEPTION_HALTED: r,
// DESCR__RECEPTION_HALTED: 1: die Empfangseinheit wurde durch „Receive Enable : = 0“ oder „Receive Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_GOOD_RECEIVED                                                        0x00004000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_GOOD_RECEIVED                                                        14
// ACCESS__GOOD_RECEIVED: r,
// DESCR__GOOD_RECEIVED: 1: ein Telegramm wurde fehlerfrei empfangen.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_RECEIVE_PARITY_ERROR                                                 0x00002000
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_RECEIVE_PARITY_ERROR                                                 13
// ACCESS__RECEIVE_PARITY_ERROR: r,
// DESCR__RECEIVE_PARITY_ERROR: 1: beim MAC-Receive-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_LONG_ERROR                                                           0x00000800
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_LONG_ERROR                                                           11
// ACCESS__LONG_ERROR: r,
// DESCR__LONG_ERROR: 1: beim Empfang eines Telegramms mit einer Framelänte    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).    Long Error wird nicht gesetzt, wenn in Receive-Control das Bit    „Enable Long Error“ gesetzt ist.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_OVERFLOW_ERROR                                                       0x00000400
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_OVERFLOW_ERROR                                                       10
// ACCESS__OVERFLOW_ERROR: r,
// DESCR__OVERFLOW_ERROR: 1: bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_CRC_ERROR                                                            0x00000200
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_CRC_ERROR                                                            9
// ACCESS__CRC_ERROR: r,
// DESCR__CRC_ERROR: 1: bei einem CRC-Fehler oder wenn Rx_er : = 1 ist.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_ALIGNMENT_ERROR                                                      0x00000100
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_ALIGNMENT_ERROR                                                      8
// ACCESS__ALIGNMENT_ERROR: r,
// DESCR__ALIGNMENT_ERROR: 1: wenn ein Telegramm mit einer Framelänge empfangen wurde,    die kein vielfaches eines Bytes ist und dessen CRC fehlerhaft     ist.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_RECEIVE_10_MB_S_STATUS                                               0x00000080
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_RECEIVE_10_MB_S_STATUS                                               7
// ACCESS__RECEIVE_10_MB_S_STATUS: r,
// DESCR__RECEIVE_10_MB_S_STATUS: 1: wenn dasTelegramm über das 10-Mbit/s Interface empfangen     wurde.0: wenn das Telegramm über das MII-Interface empfangen     wurde.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_INTERRUPT_ON_RECEIVE                                                 0x00000040
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_INTERRUPT_ON_RECEIVE                                                 6
// ACCESS__INTERRUPT_ON_RECEIVE: r,
// DESCR__INTERRUPT_ON_RECEIVE: 1: wenn ein Telegramm empfangen wurde und ein Interrupt-    Enable aktiviert ist. Dies schließt das Bit „Enable Good“ ein.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_CONTROL_FRAME_RECEIVED                                               0x00000020
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_CONTROL_FRAME_RECEIVED                                               5
// ACCESS__CONTROL_FRAME_RECEIVED: r,
// DESCR__CONTROL_FRAME_RECEIVED: 1: ein MAC Control Telegramm wurde empfangen (type  = 8808H)    und ARC hat die Adresse erkannt.
#define IRTE_REG_RECEIVE_STATUS_P0__MSK_LENGTH_ERROR                                                         0x00000010
#define IRTE_REG_RECEIVE_STATUS_P0_SHFT_LENGTH_ERROR                                                         4
// ACCESS__LENGTH_ERROR: r,
// DESCR__LENGTH_ERROR: 1: Length field im Telegramm stimmt nicht mit empfangener     Framelänge überein. Wird nicht gesetzt bei     VLAN-Telegrammen, MAC Control Telegrammen,     bei einem Length field kleiner als 46 und bei einer Framelängevon 64 Bytes.
#define IRTE_REG_RECEIVE_STATUS_P0_RST__VAL                            0x00000000


#define IRTE_REG_MAC_CONTROL_P1                                                                          0x000154C0
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MAC_CONTROL_P1__MSK_LINK_STATUS_10_MB_S                                                     0x00008000
#define IRTE_REG_MAC_CONTROL_P1_SHFT_LINK_STATUS_10_MB_S                                                     15
// ACCESS__LINK_STATUS_10_MB_S: r,w
// DESCR__LINK_STATUS_10_MB_S: Status der 10 Mb/s Verbindung
#define IRTE_REG_MAC_CONTROL_P1__MSK_ENABLE_MISSED_ROLL                                                      0x00002000
#define IRTE_REG_MAC_CONTROL_P1_SHFT_ENABLE_MISSED_ROLL                                                      13
// ACCESS__ENABLE_MISSED_ROLL: r,w
// DESCR__ENABLE_MISSED_ROLL: 1: Enable für Interrupt, wenn Missed Roll  = 1.
#define IRTE_REG_MAC_CONTROL_P1__MSK_MISSED_ROLL                                                             0x00000400
#define IRTE_REG_MAC_CONTROL_P1_SHFT_MISSED_ROLL                                                             10
// ACCESS__MISSED_ROLL: r,w
// DESCR__MISSED_ROLL: Statusbit (nur lesbar). 1: Bei einem Überlauf des Missed Error Counters in der MAC-Unit.Mit jedem Lesezugriff auf den Missed Error Counter wird dieses Bitwieder zurückgesetzt.
#define IRTE_REG_MAC_CONTROL_P1__MSK_LINK_CHANGED                                                            0x00000100
#define IRTE_REG_MAC_CONTROL_P1_SHFT_LINK_CHANGED                                                            8
// ACCESS__LINK_CHANGED: r,w
// DESCR__LINK_CHANGED: Interruptbit, das einen Link-Wechsel anzeigt. Dieses Bit wird auf denMAC-Ausgang Int_Link geführt.Ein Schreibzugriff mit Link changed : = 1 setzt dieses Link changed und den MAC-Ausgang Int_Link zurück.Ein Schreibzugriff mit Link changed : = 0 hat keine Wirkung.
#define IRTE_REG_MAC_CONTROL_P1__MSK_LOOP_10_MB_S                                                            0x00000080
#define IRTE_REG_MAC_CONTROL_P1_SHFT_LOOP_10_MB_S                                                            7
// ACCESS__LOOP_10_MB_S: r,w
// DESCR__LOOP_10_MB_S: 1: setzt den MAC-Ausgang Loop_10
#define IRTE_REG_MAC_CONTROL_P1__MSK_CONNECTION_MODE                                                         0x00000060
#define IRTE_REG_MAC_CONTROL_P1_SHFT_CONNECTION_MODE                                                         5
// ACCESS__CONNECTION_MODE: r,w
// DESCR__CONNECTION_MODE: Selektiert den Verbindungsmodus:00  = Automatic, (default)01  = Force 10-Mbit/s endec10  = Force MII (bestimmt durch MII Takt)
#define IRTE_REG_MAC_CONTROL_P1__MSK_MAC_LOOPBACK                                                            0x00000010
#define IRTE_REG_MAC_CONTROL_P1_SHFT_MAC_LOOPBACK                                                            4
// ACCESS__MAC_LOOPBACK: r,w
// DESCR__MAC_LOOPBACK: 1: Alle gesendeten Signale werden von der MAC-Empfangseinheit    wieder empfangen ohne die MAC zu verlassen.
#define IRTE_REG_MAC_CONTROL_P1__MSK_FULL_DUPLEX                                                             0x00000008
#define IRTE_REG_MAC_CONTROL_P1_SHFT_FULL_DUPLEX                                                             3
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: = 0: Halb-Duplex = 1: Voll-DuplexBit Full-Duplex ist lesbar und schreibbar.
#define IRTE_REG_MAC_CONTROL_P1__MSK_SOFTWARE_RESET                                                          0x00000004
#define IRTE_REG_MAC_CONTROL_P1_SHFT_SOFTWARE_RESET                                                          2
// ACCESS__SOFTWARE_RESET: r,w
// DESCR__SOFTWARE_RESET: 1: Reset für alle Ethernet MAC Steuerwerke und für die internen MAC-FIFOs
#define IRTE_REG_MAC_CONTROL_P1__MSK_HALT_IMMEDIATE                                                          0x00000002
#define IRTE_REG_MAC_CONTROL_P1_SHFT_HALT_IMMEDIATE                                                          1
// ACCESS__HALT_IMMEDIATE: r,w
// DESCR__HALT_IMMEDIATE: 1: Sende- und Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_MAC_CONTROL_P1__MSK_HALT_REQUEST                                                            0x00000001
#define IRTE_REG_MAC_CONTROL_P1_SHFT_HALT_REQUEST                                                            0
// ACCESS__HALT_REQUEST: r,w
// DESCR__HALT_REQUEST: 1: Sende- und Empfangseinheit abschalten.    Laufende Transfers werden noch beendet.
#define IRTE_REG_MAC_CONTROL_P1_RST__VAL                               0x00000000


#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1                                                  0x000154C4
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1__MSK_COMPARE_ENABLE                                  0x00000010
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_SHFT_COMPARE_ENABLE                                  4
// ACCESS__COMPARE_ENABLE: r,w
// DESCR__COMPARE_ENABLE: 1: Die Destinationadresse mpfangener Telegramme wird mit den\n    gespeicherten Adressen im ARC-Speicher verglichen.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1__MSK_NEGATIVE_ARC                                    0x00000008
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_SHFT_NEGATIVE_ARC                                    3
// ACCESS__NEGATIVE_ARC: r,w
// DESCR__NEGATIVE_ARC: 0: akzeptiert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und filtert alle anderen Telegramme.\n1: filtert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und akzeptiert alle anderen Telegramme.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1__MSK_BROADCAST_ACCEPT                                0x00000004
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_SHFT_BROADCAST_ACCEPT                                2
// ACCESS__BROADCAST_ACCEPT: r,w
// DESCR__BROADCAST_ACCEPT: 1: Jedes Broadcast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1__MSK_GROUP_ACCEPT                                    0x00000002
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_SHFT_GROUP_ACCEPT                                    1
// ACCESS__GROUP_ACCEPT: r,w
// DESCR__GROUP_ACCEPT: 1: Jedes Multicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1__MSK_STATION_ACCEPT                                  0x00000001
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_SHFT_STATION_ACCEPT                                  0
// ACCESS__STATION_ACCEPT: r,w
// DESCR__STATION_ACCEPT: 1: Jedes Unicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P1_RST__VAL       0x00000000


#define IRTE_REG_TRANSMIT_CONTROL_P1                                                                     0x000154C8
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_COMPLETION                                                  0x00004000
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_COMPLETION                                                  14
// ACCESS__ENABLE_COMPLETION: r,w
// DESCR__ENABLE_COMPLETION: 1: Interrupt-Enable, wenn die MAC-Unit ein Telegramm sendet    oder abbricht.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_TRANSMIT_PARITY                                             0x00002000
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_TRANSMIT_PARITY                                             13
// ACCESS__ENABLE_TRANSMIT_PARITY: r,w
// DESCR__ENABLE_TRANSMIT_PARITY: 1: Interrupt-Enable, wenn beim MAC-Transmit-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_LATE_COLLISION                                              0x00001000
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_LATE_COLLISION                                              12
// ACCESS__ENABLE_LATE_COLLISION: r,w
// DESCR__ENABLE_LATE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode nach 512 TBit eine    Kollision auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_EXCESSIVE_COLLISION                                         0x00000800
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_EXCESSIVE_COLLISION                                         11
// ACCESS__ENABLE_EXCESSIVE_COLLISION: r,w
// DESCR__ENABLE_EXCESSIVE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode beim selben    Telegramm mehr als 16 Kollisionen auftreten.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_LOST_CARRIER                                                0x00000400
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_LOST_CARRIER                                                10
// ACCESS__ENABLE_LOST_CARRIER: r,w
// DESCR__ENABLE_LOST_CARRIER: 1: Interrupt-Enable, wenn ein „Carrier Sense“ Fehler auftritt
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_EXCESSIVE_DEFERAL                                           0x00000200
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_EXCESSIVE_DEFERAL                                           9
// ACCESS__ENABLE_EXCESSIVE_DEFERAL: r,w
// DESCR__ENABLE_EXCESSIVE_DEFERAL: 1: Interrupt-Enable, wenn die maximale „Deferral-Time”     überschritten wird.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_ENABLE_UNDERRUN                                                    0x00000100
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_ENABLE_UNDERRUN                                                    8
// ACCESS__ENABLE_UNDERRUN: r,w
// DESCR__ENABLE_UNDERRUN: 1: Interrupt-Enable, bei einem MAC-Transmit-FIFO Underrun
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_MII_10_MB_S_MODE                                                   0x00000080
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_MII_10_MB_S_MODE                                                   7
// ACCESS__MII_10_MB_S_MODE: r,w
// DESCR__MII_10_MB_S_MODE: 1: Aktiviert SQE-Test im MII 10 Mb/s Mode (wenn MII_Conn  = 1)
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_SEND_PAUSE                                                         0x00000040
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_SEND_PAUSE                                                         6
// ACCESS__SEND_PAUSE: r,w
// DESCR__SEND_PAUSE: 1: Veranlasst das Senden eines Pause-Telegramms    Wird nach dem Versenden dieses Pause-Telegramms von der    MAC-Unit wieder zurückgesetzt.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_NO_EXCESSIVE_DEFER                                                 0x00000020
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_NO_EXCESSIVE_DEFER                                                 5
// ACCESS__NO_EXCESSIVE_DEFER: r,w
// DESCR__NO_EXCESSIVE_DEFER: 1: Keine Test auf “Excessive Deferral”
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_FAST_BACK_OFF_AND_COUNT                                            0x00000010
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_FAST_BACK_OFF_AND_COUNT                                            4
// ACCESS__FAST_BACK_OFF_AND_COUNT: r,w
// DESCR__FAST_BACK_OFF_AND_COUNT: 1: verwendet schnellere back-off Timer und Byte Counter    (nur zum Testen)
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_SUPPRESS_CRC                                                       0x00000008
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_SUPPRESS_CRC                                                       3
// ACCESS__SUPPRESS_CRC: r,w
// DESCR__SUPPRESS_CRC: 1: MAC-Unit hängt kein CRC an das Telegramm-Ende an
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_SUPPRESS_PADDING                                                   0x00000004
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_SUPPRESS_PADDING                                                   2
// ACCESS__SUPPRESS_PADDING: r,w
// DESCR__SUPPRESS_PADDING: 1: Kein Padding für Telegramme mit weniger als 64 Bytes
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_TRANSMIT_HALT_REQUEST                                              0x00000002
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_TRANSMIT_HALT_REQUEST                                              1
// ACCESS__TRANSMIT_HALT_REQUEST: r,w
// DESCR__TRANSMIT_HALT_REQUEST: 1: Sendeeinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_TRANSMIT_CONTROL_P1__MSK_TRANSMIT_ENABLE                                                    0x00000001
#define IRTE_REG_TRANSMIT_CONTROL_P1_SHFT_TRANSMIT_ENABLE                                                    0
// ACCESS__TRANSMIT_ENABLE: r,w
// DESCR__TRANSMIT_ENABLE: 0: Sendeeinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_TRANSMIT_CONTROL_P1_RST__VAL                          0x00000000


#define IRTE_REG_TRANSMIT_STATUS_P1                                                                      0x000154CC
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_HOST_NOT_RESPONDING_TRANSMIT                                        0x00400000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_HOST_NOT_RESPONDING_TRANSMIT                                        22
// ACCESS__HOST_NOT_RESPONDING_TRANSMIT: r,
// DESCR__HOST_NOT_RESPONDING_TRANSMIT: 1: ?
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_PAUSE_TRANSMIT                                                      0x00200000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_PAUSE_TRANSMIT                                                      21
// ACCESS__PAUSE_TRANSMIT: r,
// DESCR__PAUSE_TRANSMIT: 1: MAC hat ein MAC Control PAUSE Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_MAC_CONTROL_TRANSMIT                                                0x00100000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_MAC_CONTROL_TRANSMIT                                                20
// ACCESS__MAC_CONTROL_TRANSMIT: r,
// DESCR__MAC_CONTROL_TRANSMIT: 1: MAC hat ein MAC Control Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_VLAN_TAGGED_TRANSMIT                                                0x00080000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_VLAN_TAGGED_TRANSMIT                                                19
// ACCESS__VLAN_TAGGED_TRANSMIT: r,
// DESCR__VLAN_TAGGED_TRANSMIT: 1: MAC hat ein VLAN tagged Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_BROADCAST_TRANSMIT                                                  0x00040000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_BROADCAST_TRANSMIT                                                  18
// ACCESS__BROADCAST_TRANSMIT: r,
// DESCR__BROADCAST_TRANSMIT: 1: MAC hat ein Broadcast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_MULTICAST_TRANSMIT                                                  0x00020000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_MULTICAST_TRANSMIT                                                  17
// ACCESS__MULTICAST_TRANSMIT: r,
// DESCR__MULTICAST_TRANSMIT: 1: MAC hat ein Multicast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_SIGNAL_QUALITY_ERROR                                                0x00010000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_SIGNAL_QUALITY_ERROR                                                16
// ACCESS__SIGNAL_QUALITY_ERROR: r,
// DESCR__SIGNAL_QUALITY_ERROR: 1: kein Empfang eines „heart beat signals“ am Telegramm-Ende
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_TRANSMISSION_HALTED                                                 0x00008000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_TRANSMISSION_HALTED                                                 15
// ACCESS__TRANSMISSION_HALTED: r,
// DESCR__TRANSMISSION_HALTED: 1: die Sendeeinheit wurde durch     „Transmit Enable : = 0“ oder „Transmit Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_COMPLETION                                                          0x00004000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_COMPLETION                                                          14
// ACCESS__COMPLETION: r,
// DESCR__COMPLETION: 1: die MAC-Unit hat ein Telegramm gesendet oder abgebrochen.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_TRANSMIT_PARITY_ERROR                                               0x00002000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_TRANSMIT_PARITY_ERROR                                               13
// ACCESS__TRANSMIT_PARITY_ERROR: r,
// DESCR__TRANSMIT_PARITY_ERROR: 1: beim MAC-Transmit-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_LATE_COLLISION                                                      0x00001000
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_LATE_COLLISION                                                      12
// ACCESS__LATE_COLLISION: r,
// DESCR__LATE_COLLISION: 1: im Halb-Duplexmode ist nach 512 TBit eine Kollision     aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_TRANSMIT_10_MB_S_STATUS                                             0x00000800
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_TRANSMIT_10_MB_S_STATUS                                             11
// ACCESS__TRANSMIT_10_MB_S_STATUS: r,
// DESCR__TRANSMIT_10_MB_S_STATUS: 1: wenn ein Telegramm über das 10-Mbit/s Interface gesendet     wird.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_LOST_CARRIER                                                        0x00000400
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_LOST_CARRIER                                                        10
// ACCESS__LOST_CARRIER: r,
// DESCR__LOST_CARRIER: 1: beim Senden ist ein „Carrier Sense“ Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_EXCESSIVE_DEFERAL                                                   0x00000200
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_EXCESSIVE_DEFERAL                                                   9
// ACCESS__EXCESSIVE_DEFERAL: r,
// DESCR__EXCESSIVE_DEFERAL: 1: die maximale „Deferral-Time” wurde überschritten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_UNDERRUN                                                            0x00000100
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_UNDERRUN                                                            8
// ACCESS__UNDERRUN: r,
// DESCR__UNDERRUN: 1: ein MAC-Transmit-FIFO Underrun ist aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_INTERRUPT_ON_TRANSMIT                                               0x00000080
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_INTERRUPT_ON_TRANSMIT                                               7
// ACCESS__INTERRUPT_ON_TRANSMIT: r,
// DESCR__INTERRUPT_ON_TRANSMIT: 1: wenn das Senden eines Telegramms einen Interrupt auslöst.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_PAUSED                                                              0x00000040
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_PAUSED                                                              6
// ACCESS__PAUSED: r,
// DESCR__PAUSED: 1: das nächste Telegramm muss die Pause-Zeit abwarten.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_TRANSMIT_DEFERRED                                                   0x00000020
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_TRANSMIT_DEFERRED                                                   5
// ACCESS__TRANSMIT_DEFERRED: r,
// DESCR__TRANSMIT_DEFERRED: 1: wenn ein zu sendendes Telegramm im Wartezustand ist, da     die Halb-Duplex Verbindung belegt ist.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_EXCESSIVE_COLLISION                                                 0x00000010
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_EXCESSIVE_COLLISION                                                 4
// ACCESS__EXCESSIVE_COLLISION: r,
// DESCR__EXCESSIVE_COLLISION: 1: wenn beim selben Telegramm mehr als 16 Kollisionen     aufgetreten sind.
#define IRTE_REG_TRANSMIT_STATUS_P1__MSK_TRANSMIT_COLLISION_COUNT                                            0x0000000F
#define IRTE_REG_TRANSMIT_STATUS_P1_SHFT_TRANSMIT_COLLISION_COUNT                                            0
// ACCESS__TRANSMIT_COLLISION_COUNT: r,
// DESCR__TRANSMIT_COLLISION_COUNT: Anzahl der Kollisionen beim Senden eines Telegramms.
#define IRTE_REG_TRANSMIT_STATUS_P1_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_CONTROL_P1                                                                      0x000154D0
// ACCESS__: (r)(t)(s),(w)
// DESCR__: 
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_GOOD                                                         0x00004000
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_GOOD                                                         14
// ACCESS__ENABLE_GOOD: r,w
// DESCR__ENABLE_GOOD: 1: Interrupt-Enable, wenn ein Telegramm fehlerfrei empfangenwird.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_RECEIVE_PARITY                                               0x00002000
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_RECEIVE_PARITY                                               13
// ACCESS__ENABLE_RECEIVE_PARITY: r,w
// DESCR__ENABLE_RECEIVE_PARITY: 1: Interrupt-Enable, wenn beim MAC-Receive-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_LONG_ERROR                                                   0x00000800
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_LONG_ERROR                                                   11
// ACCESS__ENABLE_LONG_ERROR: r,w
// DESCR__ENABLE_LONG_ERROR: 1: Interrupt-Enable, beim Empfang von Telegrammen mit einer     Framelänge grösser als 1518 Bytes (bzw. 1522 Bytes für    VLAN), ausser das Bit „Long Enable“ ist gesetzt.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_OVERFLOW                                                     0x00000400
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_OVERFLOW                                                     10
// ACCESS__ENABLE_OVERFLOW: r,w
// DESCR__ENABLE_OVERFLOW: 1: Interrupt-Enable, bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_CRC_ERROR                                                    0x00000200
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_CRC_ERROR                                                    9
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Interrupt-Enable, bei einem CRC-Fehler oder wenn Rx_er : = 1vom PHY.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_ENABLE_ALIGNMENT                                                    0x00000100
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_ENABLE_ALIGNMENT                                                    8
// ACCESS__ENABLE_ALIGNMENT: r,w
// DESCR__ENABLE_ALIGNMENT: 1: Interrupt-Enable, wenn ein Telegramm mit einer Framelänge    empfangen wird, die kein vielfaches eines Bytes ist und dessen    CRC fehlerhaft ist.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_IGNORE_CRC_VALUE                                                    0x00000040
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_IGNORE_CRC_VALUE                                                    6
// ACCESS__IGNORE_CRC_VALUE: r,w
// DESCR__IGNORE_CRC_VALUE: 1: CRC-Check ist abgeschaltet.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_PASS_CONTROL                                                        0x00000020
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_PASS_CONTROL                                                        5
// ACCESS__PASS_CONTROL: packets,r
// DESCR__PASS_CONTROL: 
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_STRIP_CRC_VALUE                                                     0x00000010
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_STRIP_CRC_VALUE                                                     4
// ACCESS__STRIP_CRC_VALUE: r,w
// DESCR__STRIP_CRC_VALUE: 1: CRC-Check, aber CRC wird am MAC-Interface nicht übergeben.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_SHORT_ENABLE                                                        0x00000008
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_SHORT_ENABLE                                                        3
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    kleiner als 64 Bytes.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_LONG_ENABLE                                                         0x00000004
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_LONG_ENABLE                                                         2
// ACCESS__LONG_ENABLE: r,w
// DESCR__LONG_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_RECEIVE_HALT_REQUEST                                                0x00000002
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_RECEIVE_HALT_REQUEST                                                1
// ACCESS__RECEIVE_HALT_REQUEST: r,w
// DESCR__RECEIVE_HALT_REQUEST: 1: Empfangseinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_RECEIVE_CONTROL_P1__MSK_RECEIVE_ENABLE                                                      0x00000001
#define IRTE_REG_RECEIVE_CONTROL_P1_SHFT_RECEIVE_ENABLE                                                      0
// ACCESS__RECEIVE_ENABLE: r,w
// DESCR__RECEIVE_ENABLE: 0: Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_RECEIVE_CONTROL_P1_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_STATUS_P1                                                                       0x000154D4
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_ARCENT                                                               0x3E000000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_ARCENT                                                               25
// ACCESS__ARCENT: r,
// DESCR__ARCENT: Offsetadresse im ARC-Speicher unter der die empfangene Adresse eingetragen ist.ARCEnt  = 11111, wenn empfangene Adresse nicht in der ARC-Unit gespeichert ist.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_ARCSTATUS                                                            0x01E00000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_ARCSTATUS                                                            21
// ACCESS__ARCSTATUS: r,
// DESCR__ARCSTATUS: Status der ARC-Unit
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_PAUSE_RECEIVED                                                       0x00100000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_PAUSE_RECEIVED                                                       20
// ACCESS__PAUSE_RECEIVED: r,
// DESCR__PAUSE_RECEIVED: 1: MAC hat ein MAC Control PAUSE Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_VLAN_RECEIVED                                                        0x00080000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_VLAN_RECEIVED                                                        19
// ACCESS__VLAN_RECEIVED: r,
// DESCR__VLAN_RECEIVED: 1: MAC hat ein VLAN tagged Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_BROADCAST_RECEIVED                                                   0x00040000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_BROADCAST_RECEIVED                                                   18
// ACCESS__BROADCAST_RECEIVED: r,
// DESCR__BROADCAST_RECEIVED: 1: MAC hat ein Broadcast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_MULTICAST_RECEIVED                                                   0x00020000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_MULTICAST_RECEIVED                                                   17
// ACCESS__MULTICAST_RECEIVED: r,
// DESCR__MULTICAST_RECEIVED: 1: MAC hat ein Multicast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_RECEPTION_HALTED                                                     0x00008000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_RECEPTION_HALTED                                                     15
// ACCESS__RECEPTION_HALTED: r,
// DESCR__RECEPTION_HALTED: 1: die Empfangseinheit wurde durch „Receive Enable : = 0“ oder „Receive Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_GOOD_RECEIVED                                                        0x00004000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_GOOD_RECEIVED                                                        14
// ACCESS__GOOD_RECEIVED: r,
// DESCR__GOOD_RECEIVED: 1: ein Telegramm wurde fehlerfrei empfangen.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_RECEIVE_PARITY_ERROR                                                 0x00002000
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_RECEIVE_PARITY_ERROR                                                 13
// ACCESS__RECEIVE_PARITY_ERROR: r,
// DESCR__RECEIVE_PARITY_ERROR: 1: beim MAC-Receive-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_LONG_ERROR                                                           0x00000800
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_LONG_ERROR                                                           11
// ACCESS__LONG_ERROR: r,
// DESCR__LONG_ERROR: 1: beim Empfang eines Telegramms mit einer Framelänte    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).    Long Error wird nicht gesetzt, wenn in Receive-Control das Bit    „Enable Long Error“ gesetzt ist.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_OVERFLOW_ERROR                                                       0x00000400
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_OVERFLOW_ERROR                                                       10
// ACCESS__OVERFLOW_ERROR: r,
// DESCR__OVERFLOW_ERROR: 1: bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_CRC_ERROR                                                            0x00000200
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_CRC_ERROR                                                            9
// ACCESS__CRC_ERROR: r,
// DESCR__CRC_ERROR: 1: bei einem CRC-Fehler oder wenn Rx_er : = 1 ist.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_ALIGNMENT_ERROR                                                      0x00000100
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_ALIGNMENT_ERROR                                                      8
// ACCESS__ALIGNMENT_ERROR: r,
// DESCR__ALIGNMENT_ERROR: 1: wenn ein Telegramm mit einer Framelänge empfangen wurde,    die kein vielfaches eines Bytes ist und dessen CRC fehlerhaft     ist.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_RECEIVE_10_MB_S_STATUS                                               0x00000080
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_RECEIVE_10_MB_S_STATUS                                               7
// ACCESS__RECEIVE_10_MB_S_STATUS: r,
// DESCR__RECEIVE_10_MB_S_STATUS: 1: wenn dasTelegramm über das 10-Mbit/s Interface empfangen     wurde.0: wenn das Telegramm über das MII-Interface empfangen     wurde.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_INTERRUPT_ON_RECEIVE                                                 0x00000040
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_INTERRUPT_ON_RECEIVE                                                 6
// ACCESS__INTERRUPT_ON_RECEIVE: r,
// DESCR__INTERRUPT_ON_RECEIVE: 1: wenn ein Telegramm empfangen wurde und ein Interrupt-    Enable aktiviert ist. Dies schließt das Bit „Enable Good“ ein.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_CONTROL_FRAME_RECEIVED                                               0x00000020
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_CONTROL_FRAME_RECEIVED                                               5
// ACCESS__CONTROL_FRAME_RECEIVED: r,
// DESCR__CONTROL_FRAME_RECEIVED: 1: ein MAC Control Telegramm wurde empfangen (type  = 8808H)    und ARC hat die Adresse erkannt.
#define IRTE_REG_RECEIVE_STATUS_P1__MSK_LENGTH_ERROR                                                         0x00000010
#define IRTE_REG_RECEIVE_STATUS_P1_SHFT_LENGTH_ERROR                                                         4
// ACCESS__LENGTH_ERROR: r,
// DESCR__LENGTH_ERROR: 1: Length field im Telegramm stimmt nicht mit empfangener     Framelänge überein. Wird nicht gesetzt bei     VLAN-Telegrammen, MAC Control Telegrammen,     bei einem Length field kleiner als 46 und bei einer Framelängevon 64 Bytes.
#define IRTE_REG_RECEIVE_STATUS_P1_RST__VAL                            0x00000000


#define IRTE_REG_MAC_CONTROL_P2                                                                          0x00015540
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MAC_CONTROL_P2__MSK_LINK_STATUS_10_MB_S                                                     0x00008000
#define IRTE_REG_MAC_CONTROL_P2_SHFT_LINK_STATUS_10_MB_S                                                     15
// ACCESS__LINK_STATUS_10_MB_S: r,w
// DESCR__LINK_STATUS_10_MB_S: Status der 10 Mb/s Verbindung
#define IRTE_REG_MAC_CONTROL_P2__MSK_ENABLE_MISSED_ROLL                                                      0x00002000
#define IRTE_REG_MAC_CONTROL_P2_SHFT_ENABLE_MISSED_ROLL                                                      13
// ACCESS__ENABLE_MISSED_ROLL: r,w
// DESCR__ENABLE_MISSED_ROLL: 1: Enable für Interrupt, wenn Missed Roll  = 1.
#define IRTE_REG_MAC_CONTROL_P2__MSK_MISSED_ROLL                                                             0x00000400
#define IRTE_REG_MAC_CONTROL_P2_SHFT_MISSED_ROLL                                                             10
// ACCESS__MISSED_ROLL: r,w
// DESCR__MISSED_ROLL: Statusbit (nur lesbar). 1: Bei einem Überlauf des Missed Error Counters in der MAC-Unit.Mit jedem Lesezugriff auf den Missed Error Counter wird dieses Bitwieder zurückgesetzt.
#define IRTE_REG_MAC_CONTROL_P2__MSK_LINK_CHANGED                                                            0x00000100
#define IRTE_REG_MAC_CONTROL_P2_SHFT_LINK_CHANGED                                                            8
// ACCESS__LINK_CHANGED: r,w
// DESCR__LINK_CHANGED: Interruptbit, das einen Link-Wechsel anzeigt. Dieses Bit wird auf denMAC-Ausgang Int_Link geführt.Ein Schreibzugriff mit Link changed : = 1 setzt dieses Link changed und den MAC-Ausgang Int_Link zurück.Ein Schreibzugriff mit Link changed : = 0 hat keine Wirkung.
#define IRTE_REG_MAC_CONTROL_P2__MSK_LOOP_10_MB_S                                                            0x00000080
#define IRTE_REG_MAC_CONTROL_P2_SHFT_LOOP_10_MB_S                                                            7
// ACCESS__LOOP_10_MB_S: r,w
// DESCR__LOOP_10_MB_S: 1: setzt den MAC-Ausgang Loop_10
#define IRTE_REG_MAC_CONTROL_P2__MSK_CONNECTION_MODE                                                         0x00000060
#define IRTE_REG_MAC_CONTROL_P2_SHFT_CONNECTION_MODE                                                         5
// ACCESS__CONNECTION_MODE: r,w
// DESCR__CONNECTION_MODE: Selektiert den Verbindungsmodus:00  = Automatic, (default)01  = Force 10-Mbit/s endec10  = Force MII (bestimmt durch MII Takt)
#define IRTE_REG_MAC_CONTROL_P2__MSK_MAC_LOOPBACK                                                            0x00000010
#define IRTE_REG_MAC_CONTROL_P2_SHFT_MAC_LOOPBACK                                                            4
// ACCESS__MAC_LOOPBACK: r,w
// DESCR__MAC_LOOPBACK: 1: Alle gesendeten Signale werden von der MAC-Empfangseinheit    wieder empfangen ohne die MAC zu verlassen.
#define IRTE_REG_MAC_CONTROL_P2__MSK_FULL_DUPLEX                                                             0x00000008
#define IRTE_REG_MAC_CONTROL_P2_SHFT_FULL_DUPLEX                                                             3
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: = 0: Halb-Duplex = 1: Voll-DuplexBit Full-Duplex ist lesbar und schreibbar.
#define IRTE_REG_MAC_CONTROL_P2__MSK_SOFTWARE_RESET                                                          0x00000004
#define IRTE_REG_MAC_CONTROL_P2_SHFT_SOFTWARE_RESET                                                          2
// ACCESS__SOFTWARE_RESET: r,w
// DESCR__SOFTWARE_RESET: 1: Reset für alle Ethernet MAC Steuerwerke und für die internen MAC-FIFOs
#define IRTE_REG_MAC_CONTROL_P2__MSK_HALT_IMMEDIATE                                                          0x00000002
#define IRTE_REG_MAC_CONTROL_P2_SHFT_HALT_IMMEDIATE                                                          1
// ACCESS__HALT_IMMEDIATE: r,w
// DESCR__HALT_IMMEDIATE: 1: Sende- und Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_MAC_CONTROL_P2__MSK_HALT_REQUEST                                                            0x00000001
#define IRTE_REG_MAC_CONTROL_P2_SHFT_HALT_REQUEST                                                            0
// ACCESS__HALT_REQUEST: r,w
// DESCR__HALT_REQUEST: 1: Sende- und Empfangseinheit abschalten.    Laufende Transfers werden noch beendet.
#define IRTE_REG_MAC_CONTROL_P2_RST__VAL                               0x00000000


#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2                                                  0x00015544
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2__MSK_COMPARE_ENABLE                                  0x00000010
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_SHFT_COMPARE_ENABLE                                  4
// ACCESS__COMPARE_ENABLE: r,w
// DESCR__COMPARE_ENABLE: 1: Die Destinationadresse mpfangener Telegramme wird mit den\n    gespeicherten Adressen im ARC-Speicher verglichen.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2__MSK_NEGATIVE_ARC                                    0x00000008
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_SHFT_NEGATIVE_ARC                                    3
// ACCESS__NEGATIVE_ARC: r,w
// DESCR__NEGATIVE_ARC: 0: akzeptiert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und filtert alle anderen Telegramme.\n1: filtert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und akzeptiert alle anderen Telegramme.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2__MSK_BROADCAST_ACCEPT                                0x00000004
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_SHFT_BROADCAST_ACCEPT                                2
// ACCESS__BROADCAST_ACCEPT: r,w
// DESCR__BROADCAST_ACCEPT: 1: Jedes Broadcast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2__MSK_GROUP_ACCEPT                                    0x00000002
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_SHFT_GROUP_ACCEPT                                    1
// ACCESS__GROUP_ACCEPT: r,w
// DESCR__GROUP_ACCEPT: 1: Jedes Multicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2__MSK_STATION_ACCEPT                                  0x00000001
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_SHFT_STATION_ACCEPT                                  0
// ACCESS__STATION_ACCEPT: r,w
// DESCR__STATION_ACCEPT: 1: Jedes Unicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P2_RST__VAL       0x00000000


#define IRTE_REG_TRANSMIT_CONTROL_P2                                                                     0x00015548
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_COMPLETION                                                  0x00004000
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_COMPLETION                                                  14
// ACCESS__ENABLE_COMPLETION: r,w
// DESCR__ENABLE_COMPLETION: 1: Interrupt-Enable, wenn die MAC-Unit ein Telegramm sendet    oder abbricht.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_TRANSMIT_PARITY                                             0x00002000
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_TRANSMIT_PARITY                                             13
// ACCESS__ENABLE_TRANSMIT_PARITY: r,w
// DESCR__ENABLE_TRANSMIT_PARITY: 1: Interrupt-Enable, wenn beim MAC-Transmit-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_LATE_COLLISION                                              0x00001000
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_LATE_COLLISION                                              12
// ACCESS__ENABLE_LATE_COLLISION: r,w
// DESCR__ENABLE_LATE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode nach 512 TBit eine    Kollision auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_EXCESSIVE_COLLISION                                         0x00000800
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_EXCESSIVE_COLLISION                                         11
// ACCESS__ENABLE_EXCESSIVE_COLLISION: r,w
// DESCR__ENABLE_EXCESSIVE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode beim selben    Telegramm mehr als 16 Kollisionen auftreten.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_LOST_CARRIER                                                0x00000400
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_LOST_CARRIER                                                10
// ACCESS__ENABLE_LOST_CARRIER: r,w
// DESCR__ENABLE_LOST_CARRIER: 1: Interrupt-Enable, wenn ein „Carrier Sense“ Fehler auftritt
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_EXCESSIVE_DEFERAL                                           0x00000200
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_EXCESSIVE_DEFERAL                                           9
// ACCESS__ENABLE_EXCESSIVE_DEFERAL: r,w
// DESCR__ENABLE_EXCESSIVE_DEFERAL: 1: Interrupt-Enable, wenn die maximale „Deferral-Time”     überschritten wird.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_ENABLE_UNDERRUN                                                    0x00000100
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_ENABLE_UNDERRUN                                                    8
// ACCESS__ENABLE_UNDERRUN: r,w
// DESCR__ENABLE_UNDERRUN: 1: Interrupt-Enable, bei einem MAC-Transmit-FIFO Underrun
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_MII_10_MB_S_MODE                                                   0x00000080
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_MII_10_MB_S_MODE                                                   7
// ACCESS__MII_10_MB_S_MODE: r,w
// DESCR__MII_10_MB_S_MODE: 1: Aktiviert SQE-Test im MII 10 Mb/s Mode (wenn MII_Conn  = 1)
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_SEND_PAUSE                                                         0x00000040
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_SEND_PAUSE                                                         6
// ACCESS__SEND_PAUSE: r,w
// DESCR__SEND_PAUSE: 1: Veranlasst das Senden eines Pause-Telegramms    Wird nach dem Versenden dieses Pause-Telegramms von der    MAC-Unit wieder zurückgesetzt.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_NO_EXCESSIVE_DEFER                                                 0x00000020
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_NO_EXCESSIVE_DEFER                                                 5
// ACCESS__NO_EXCESSIVE_DEFER: r,w
// DESCR__NO_EXCESSIVE_DEFER: 1: Keine Test auf “Excessive Deferral”
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_FAST_BACK_OFF_AND_COUNT                                            0x00000010
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_FAST_BACK_OFF_AND_COUNT                                            4
// ACCESS__FAST_BACK_OFF_AND_COUNT: r,w
// DESCR__FAST_BACK_OFF_AND_COUNT: 1: verwendet schnellere back-off Timer und Byte Counter    (nur zum Testen)
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_SUPPRESS_CRC                                                       0x00000008
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_SUPPRESS_CRC                                                       3
// ACCESS__SUPPRESS_CRC: r,w
// DESCR__SUPPRESS_CRC: 1: MAC-Unit hängt kein CRC an das Telegramm-Ende an
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_SUPPRESS_PADDING                                                   0x00000004
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_SUPPRESS_PADDING                                                   2
// ACCESS__SUPPRESS_PADDING: r,w
// DESCR__SUPPRESS_PADDING: 1: Kein Padding für Telegramme mit weniger als 64 Bytes
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_TRANSMIT_HALT_REQUEST                                              0x00000002
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_TRANSMIT_HALT_REQUEST                                              1
// ACCESS__TRANSMIT_HALT_REQUEST: r,w
// DESCR__TRANSMIT_HALT_REQUEST: 1: Sendeeinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_TRANSMIT_CONTROL_P2__MSK_TRANSMIT_ENABLE                                                    0x00000001
#define IRTE_REG_TRANSMIT_CONTROL_P2_SHFT_TRANSMIT_ENABLE                                                    0
// ACCESS__TRANSMIT_ENABLE: r,w
// DESCR__TRANSMIT_ENABLE: 0: Sendeeinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_TRANSMIT_CONTROL_P2_RST__VAL                          0x00000000


#define IRTE_REG_TRANSMIT_STATUS_P2                                                                      0x0001554C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_HOST_NOT_RESPONDING_TRANSMIT                                        0x00400000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_HOST_NOT_RESPONDING_TRANSMIT                                        22
// ACCESS__HOST_NOT_RESPONDING_TRANSMIT: r,
// DESCR__HOST_NOT_RESPONDING_TRANSMIT: 1: ?
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_PAUSE_TRANSMIT                                                      0x00200000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_PAUSE_TRANSMIT                                                      21
// ACCESS__PAUSE_TRANSMIT: r,
// DESCR__PAUSE_TRANSMIT: 1: MAC hat ein MAC Control PAUSE Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_MAC_CONTROL_TRANSMIT                                                0x00100000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_MAC_CONTROL_TRANSMIT                                                20
// ACCESS__MAC_CONTROL_TRANSMIT: r,
// DESCR__MAC_CONTROL_TRANSMIT: 1: MAC hat ein MAC Control Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_VLAN_TAGGED_TRANSMIT                                                0x00080000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_VLAN_TAGGED_TRANSMIT                                                19
// ACCESS__VLAN_TAGGED_TRANSMIT: r,
// DESCR__VLAN_TAGGED_TRANSMIT: 1: MAC hat ein VLAN tagged Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_BROADCAST_TRANSMIT                                                  0x00040000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_BROADCAST_TRANSMIT                                                  18
// ACCESS__BROADCAST_TRANSMIT: r,
// DESCR__BROADCAST_TRANSMIT: 1: MAC hat ein Broadcast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_MULTICAST_TRANSMIT                                                  0x00020000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_MULTICAST_TRANSMIT                                                  17
// ACCESS__MULTICAST_TRANSMIT: r,
// DESCR__MULTICAST_TRANSMIT: 1: MAC hat ein Multicast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_SIGNAL_QUALITY_ERROR                                                0x00010000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_SIGNAL_QUALITY_ERROR                                                16
// ACCESS__SIGNAL_QUALITY_ERROR: r,
// DESCR__SIGNAL_QUALITY_ERROR: 1: kein Empfang eines „heart beat signals“ am Telegramm-Ende
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_TRANSMISSION_HALTED                                                 0x00008000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_TRANSMISSION_HALTED                                                 15
// ACCESS__TRANSMISSION_HALTED: r,
// DESCR__TRANSMISSION_HALTED: 1: die Sendeeinheit wurde durch     „Transmit Enable : = 0“ oder „Transmit Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_COMPLETION                                                          0x00004000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_COMPLETION                                                          14
// ACCESS__COMPLETION: r,
// DESCR__COMPLETION: 1: die MAC-Unit hat ein Telegramm gesendet oder abgebrochen.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_TRANSMIT_PARITY_ERROR                                               0x00002000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_TRANSMIT_PARITY_ERROR                                               13
// ACCESS__TRANSMIT_PARITY_ERROR: r,
// DESCR__TRANSMIT_PARITY_ERROR: 1: beim MAC-Transmit-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_LATE_COLLISION                                                      0x00001000
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_LATE_COLLISION                                                      12
// ACCESS__LATE_COLLISION: r,
// DESCR__LATE_COLLISION: 1: im Halb-Duplexmode ist nach 512 TBit eine Kollision     aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_TRANSMIT_10_MB_S_STATUS                                             0x00000800
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_TRANSMIT_10_MB_S_STATUS                                             11
// ACCESS__TRANSMIT_10_MB_S_STATUS: r,
// DESCR__TRANSMIT_10_MB_S_STATUS: 1: wenn ein Telegramm über das 10-Mbit/s Interface gesendet     wird.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_LOST_CARRIER                                                        0x00000400
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_LOST_CARRIER                                                        10
// ACCESS__LOST_CARRIER: r,
// DESCR__LOST_CARRIER: 1: beim Senden ist ein „Carrier Sense“ Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_EXCESSIVE_DEFERAL                                                   0x00000200
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_EXCESSIVE_DEFERAL                                                   9
// ACCESS__EXCESSIVE_DEFERAL: r,
// DESCR__EXCESSIVE_DEFERAL: 1: die maximale „Deferral-Time” wurde überschritten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_UNDERRUN                                                            0x00000100
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_UNDERRUN                                                            8
// ACCESS__UNDERRUN: r,
// DESCR__UNDERRUN: 1: ein MAC-Transmit-FIFO Underrun ist aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_INTERRUPT_ON_TRANSMIT                                               0x00000080
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_INTERRUPT_ON_TRANSMIT                                               7
// ACCESS__INTERRUPT_ON_TRANSMIT: r,
// DESCR__INTERRUPT_ON_TRANSMIT: 1: wenn das Senden eines Telegramms einen Interrupt auslöst.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_PAUSED                                                              0x00000040
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_PAUSED                                                              6
// ACCESS__PAUSED: r,
// DESCR__PAUSED: 1: das nächste Telegramm muss die Pause-Zeit abwarten.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_TRANSMIT_DEFERRED                                                   0x00000020
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_TRANSMIT_DEFERRED                                                   5
// ACCESS__TRANSMIT_DEFERRED: r,
// DESCR__TRANSMIT_DEFERRED: 1: wenn ein zu sendendes Telegramm im Wartezustand ist, da     die Halb-Duplex Verbindung belegt ist.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_EXCESSIVE_COLLISION                                                 0x00000010
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_EXCESSIVE_COLLISION                                                 4
// ACCESS__EXCESSIVE_COLLISION: r,
// DESCR__EXCESSIVE_COLLISION: 1: wenn beim selben Telegramm mehr als 16 Kollisionen     aufgetreten sind.
#define IRTE_REG_TRANSMIT_STATUS_P2__MSK_TRANSMIT_COLLISION_COUNT                                            0x0000000F
#define IRTE_REG_TRANSMIT_STATUS_P2_SHFT_TRANSMIT_COLLISION_COUNT                                            0
// ACCESS__TRANSMIT_COLLISION_COUNT: r,
// DESCR__TRANSMIT_COLLISION_COUNT: Anzahl der Kollisionen beim Senden eines Telegramms.
#define IRTE_REG_TRANSMIT_STATUS_P2_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_CONTROL_P2                                                                      0x00015550
// ACCESS__: (r)(t)(s),(w)
// DESCR__: 
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_GOOD                                                         0x00004000
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_GOOD                                                         14
// ACCESS__ENABLE_GOOD: r,w
// DESCR__ENABLE_GOOD: 1: Interrupt-Enable, wenn ein Telegramm fehlerfrei empfangenwird.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_RECEIVE_PARITY                                               0x00002000
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_RECEIVE_PARITY                                               13
// ACCESS__ENABLE_RECEIVE_PARITY: r,w
// DESCR__ENABLE_RECEIVE_PARITY: 1: Interrupt-Enable, wenn beim MAC-Receive-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_LONG_ERROR                                                   0x00000800
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_LONG_ERROR                                                   11
// ACCESS__ENABLE_LONG_ERROR: r,w
// DESCR__ENABLE_LONG_ERROR: 1: Interrupt-Enable, beim Empfang von Telegrammen mit einer     Framelänge grösser als 1518 Bytes (bzw. 1522 Bytes für    VLAN), ausser das Bit „Long Enable“ ist gesetzt.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_OVERFLOW                                                     0x00000400
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_OVERFLOW                                                     10
// ACCESS__ENABLE_OVERFLOW: r,w
// DESCR__ENABLE_OVERFLOW: 1: Interrupt-Enable, bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_CRC_ERROR                                                    0x00000200
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_CRC_ERROR                                                    9
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Interrupt-Enable, bei einem CRC-Fehler oder wenn Rx_er : = 1vom PHY.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_ENABLE_ALIGNMENT                                                    0x00000100
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_ENABLE_ALIGNMENT                                                    8
// ACCESS__ENABLE_ALIGNMENT: r,w
// DESCR__ENABLE_ALIGNMENT: 1: Interrupt-Enable, wenn ein Telegramm mit einer Framelänge    empfangen wird, die kein vielfaches eines Bytes ist und dessen    CRC fehlerhaft ist.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_IGNORE_CRC_VALUE                                                    0x00000040
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_IGNORE_CRC_VALUE                                                    6
// ACCESS__IGNORE_CRC_VALUE: r,w
// DESCR__IGNORE_CRC_VALUE: 1: CRC-Check ist abgeschaltet.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_PASS_CONTROL                                                        0x00000020
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_PASS_CONTROL                                                        5
// ACCESS__PASS_CONTROL: packets,r
// DESCR__PASS_CONTROL: 
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_STRIP_CRC_VALUE                                                     0x00000010
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_STRIP_CRC_VALUE                                                     4
// ACCESS__STRIP_CRC_VALUE: r,w
// DESCR__STRIP_CRC_VALUE: 1: CRC-Check, aber CRC wird am MAC-Interface nicht übergeben.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_SHORT_ENABLE                                                        0x00000008
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_SHORT_ENABLE                                                        3
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    kleiner als 64 Bytes.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_LONG_ENABLE                                                         0x00000004
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_LONG_ENABLE                                                         2
// ACCESS__LONG_ENABLE: r,w
// DESCR__LONG_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_RECEIVE_HALT_REQUEST                                                0x00000002
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_RECEIVE_HALT_REQUEST                                                1
// ACCESS__RECEIVE_HALT_REQUEST: r,w
// DESCR__RECEIVE_HALT_REQUEST: 1: Empfangseinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_RECEIVE_CONTROL_P2__MSK_RECEIVE_ENABLE                                                      0x00000001
#define IRTE_REG_RECEIVE_CONTROL_P2_SHFT_RECEIVE_ENABLE                                                      0
// ACCESS__RECEIVE_ENABLE: r,w
// DESCR__RECEIVE_ENABLE: 0: Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_RECEIVE_CONTROL_P2_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_STATUS_P2                                                                       0x00015554
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_ARCENT                                                               0x3E000000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_ARCENT                                                               25
// ACCESS__ARCENT: r,
// DESCR__ARCENT: Offsetadresse im ARC-Speicher unter der die empfangene Adresse eingetragen ist.ARCEnt  = 11111, wenn empfangene Adresse nicht in der ARC-Unit gespeichert ist.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_ARCSTATUS                                                            0x01E00000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_ARCSTATUS                                                            21
// ACCESS__ARCSTATUS: r,
// DESCR__ARCSTATUS: Status der ARC-Unit
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_PAUSE_RECEIVED                                                       0x00100000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_PAUSE_RECEIVED                                                       20
// ACCESS__PAUSE_RECEIVED: r,
// DESCR__PAUSE_RECEIVED: 1: MAC hat ein MAC Control PAUSE Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_VLAN_RECEIVED                                                        0x00080000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_VLAN_RECEIVED                                                        19
// ACCESS__VLAN_RECEIVED: r,
// DESCR__VLAN_RECEIVED: 1: MAC hat ein VLAN tagged Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_BROADCAST_RECEIVED                                                   0x00040000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_BROADCAST_RECEIVED                                                   18
// ACCESS__BROADCAST_RECEIVED: r,
// DESCR__BROADCAST_RECEIVED: 1: MAC hat ein Broadcast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_MULTICAST_RECEIVED                                                   0x00020000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_MULTICAST_RECEIVED                                                   17
// ACCESS__MULTICAST_RECEIVED: r,
// DESCR__MULTICAST_RECEIVED: 1: MAC hat ein Multicast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_RECEPTION_HALTED                                                     0x00008000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_RECEPTION_HALTED                                                     15
// ACCESS__RECEPTION_HALTED: r,
// DESCR__RECEPTION_HALTED: 1: die Empfangseinheit wurde durch „Receive Enable : = 0“ oder „Receive Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_GOOD_RECEIVED                                                        0x00004000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_GOOD_RECEIVED                                                        14
// ACCESS__GOOD_RECEIVED: r,
// DESCR__GOOD_RECEIVED: 1: ein Telegramm wurde fehlerfrei empfangen.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_RECEIVE_PARITY_ERROR                                                 0x00002000
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_RECEIVE_PARITY_ERROR                                                 13
// ACCESS__RECEIVE_PARITY_ERROR: r,
// DESCR__RECEIVE_PARITY_ERROR: 1: beim MAC-Receive-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_LONG_ERROR                                                           0x00000800
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_LONG_ERROR                                                           11
// ACCESS__LONG_ERROR: r,
// DESCR__LONG_ERROR: 1: beim Empfang eines Telegramms mit einer Framelänte    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).    Long Error wird nicht gesetzt, wenn in Receive-Control das Bit    „Enable Long Error“ gesetzt ist.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_OVERFLOW_ERROR                                                       0x00000400
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_OVERFLOW_ERROR                                                       10
// ACCESS__OVERFLOW_ERROR: r,
// DESCR__OVERFLOW_ERROR: 1: bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_CRC_ERROR                                                            0x00000200
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_CRC_ERROR                                                            9
// ACCESS__CRC_ERROR: r,
// DESCR__CRC_ERROR: 1: bei einem CRC-Fehler oder wenn Rx_er : = 1 ist.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_ALIGNMENT_ERROR                                                      0x00000100
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_ALIGNMENT_ERROR                                                      8
// ACCESS__ALIGNMENT_ERROR: r,
// DESCR__ALIGNMENT_ERROR: 1: wenn ein Telegramm mit einer Framelänge empfangen wurde,    die kein vielfaches eines Bytes ist und dessen CRC fehlerhaft     ist.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_RECEIVE_10_MB_S_STATUS                                               0x00000080
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_RECEIVE_10_MB_S_STATUS                                               7
// ACCESS__RECEIVE_10_MB_S_STATUS: r,
// DESCR__RECEIVE_10_MB_S_STATUS: 1: wenn dasTelegramm über das 10-Mbit/s Interface empfangen     wurde.0: wenn das Telegramm über das MII-Interface empfangen     wurde.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_INTERRUPT_ON_RECEIVE                                                 0x00000040
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_INTERRUPT_ON_RECEIVE                                                 6
// ACCESS__INTERRUPT_ON_RECEIVE: r,
// DESCR__INTERRUPT_ON_RECEIVE: 1: wenn ein Telegramm empfangen wurde und ein Interrupt-    Enable aktiviert ist. Dies schließt das Bit „Enable Good“ ein.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_CONTROL_FRAME_RECEIVED                                               0x00000020
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_CONTROL_FRAME_RECEIVED                                               5
// ACCESS__CONTROL_FRAME_RECEIVED: r,
// DESCR__CONTROL_FRAME_RECEIVED: 1: ein MAC Control Telegramm wurde empfangen (type  = 8808H)    und ARC hat die Adresse erkannt.
#define IRTE_REG_RECEIVE_STATUS_P2__MSK_LENGTH_ERROR                                                         0x00000010
#define IRTE_REG_RECEIVE_STATUS_P2_SHFT_LENGTH_ERROR                                                         4
// ACCESS__LENGTH_ERROR: r,
// DESCR__LENGTH_ERROR: 1: Length field im Telegramm stimmt nicht mit empfangener     Framelänge überein. Wird nicht gesetzt bei     VLAN-Telegrammen, MAC Control Telegrammen,     bei einem Length field kleiner als 46 und bei einer Framelängevon 64 Bytes.
#define IRTE_REG_RECEIVE_STATUS_P2_RST__VAL                            0x00000000


#define IRTE_REG_MAC_CONTROL_P3                                                                          0x000155C0
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_MAC_CONTROL_P3__MSK_LINK_STATUS_10_MB_S                                                     0x00008000
#define IRTE_REG_MAC_CONTROL_P3_SHFT_LINK_STATUS_10_MB_S                                                     15
// ACCESS__LINK_STATUS_10_MB_S: r,w
// DESCR__LINK_STATUS_10_MB_S: Status der 10 Mb/s Verbindung
#define IRTE_REG_MAC_CONTROL_P3__MSK_ENABLE_MISSED_ROLL                                                      0x00002000
#define IRTE_REG_MAC_CONTROL_P3_SHFT_ENABLE_MISSED_ROLL                                                      13
// ACCESS__ENABLE_MISSED_ROLL: r,w
// DESCR__ENABLE_MISSED_ROLL: 1: Enable für Interrupt, wenn Missed Roll  = 1.
#define IRTE_REG_MAC_CONTROL_P3__MSK_MISSED_ROLL                                                             0x00000400
#define IRTE_REG_MAC_CONTROL_P3_SHFT_MISSED_ROLL                                                             10
// ACCESS__MISSED_ROLL: r,w
// DESCR__MISSED_ROLL: Statusbit (nur lesbar). 1: Bei einem Überlauf des Missed Error Counters in der MAC-Unit.Mit jedem Lesezugriff auf den Missed Error Counter wird dieses Bitwieder zurückgesetzt.
#define IRTE_REG_MAC_CONTROL_P3__MSK_LINK_CHANGED                                                            0x00000100
#define IRTE_REG_MAC_CONTROL_P3_SHFT_LINK_CHANGED                                                            8
// ACCESS__LINK_CHANGED: r,w
// DESCR__LINK_CHANGED: Interruptbit, das einen Link-Wechsel anzeigt. Dieses Bit wird auf denMAC-Ausgang Int_Link geführt.Ein Schreibzugriff mit Link changed : = 1 setzt dieses Link changed und den MAC-Ausgang Int_Link zurück.Ein Schreibzugriff mit Link changed : = 0 hat keine Wirkung.
#define IRTE_REG_MAC_CONTROL_P3__MSK_LOOP_10_MB_S                                                            0x00000080
#define IRTE_REG_MAC_CONTROL_P3_SHFT_LOOP_10_MB_S                                                            7
// ACCESS__LOOP_10_MB_S: r,w
// DESCR__LOOP_10_MB_S: 1: setzt den MAC-Ausgang Loop_10
#define IRTE_REG_MAC_CONTROL_P3__MSK_CONNECTION_MODE                                                         0x00000060
#define IRTE_REG_MAC_CONTROL_P3_SHFT_CONNECTION_MODE                                                         5
// ACCESS__CONNECTION_MODE: r,w
// DESCR__CONNECTION_MODE: Selektiert den Verbindungsmodus:00  = Automatic, (default)01  = Force 10-Mbit/s endec10  = Force MII (bestimmt durch MII Takt)
#define IRTE_REG_MAC_CONTROL_P3__MSK_MAC_LOOPBACK                                                            0x00000010
#define IRTE_REG_MAC_CONTROL_P3_SHFT_MAC_LOOPBACK                                                            4
// ACCESS__MAC_LOOPBACK: r,w
// DESCR__MAC_LOOPBACK: 1: Alle gesendeten Signale werden von der MAC-Empfangseinheit    wieder empfangen ohne die MAC zu verlassen.
#define IRTE_REG_MAC_CONTROL_P3__MSK_FULL_DUPLEX                                                             0x00000008
#define IRTE_REG_MAC_CONTROL_P3_SHFT_FULL_DUPLEX                                                             3
// ACCESS__FULL_DUPLEX: r,w
// DESCR__FULL_DUPLEX: = 0: Halb-Duplex = 1: Voll-DuplexBit Full-Duplex ist lesbar und schreibbar.
#define IRTE_REG_MAC_CONTROL_P3__MSK_SOFTWARE_RESET                                                          0x00000004
#define IRTE_REG_MAC_CONTROL_P3_SHFT_SOFTWARE_RESET                                                          2
// ACCESS__SOFTWARE_RESET: r,w
// DESCR__SOFTWARE_RESET: 1: Reset für alle Ethernet MAC Steuerwerke und für die internen MAC-FIFOs
#define IRTE_REG_MAC_CONTROL_P3__MSK_HALT_IMMEDIATE                                                          0x00000002
#define IRTE_REG_MAC_CONTROL_P3_SHFT_HALT_IMMEDIATE                                                          1
// ACCESS__HALT_IMMEDIATE: r,w
// DESCR__HALT_IMMEDIATE: 1: Sende- und Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_MAC_CONTROL_P3__MSK_HALT_REQUEST                                                            0x00000001
#define IRTE_REG_MAC_CONTROL_P3_SHFT_HALT_REQUEST                                                            0
// ACCESS__HALT_REQUEST: r,w
// DESCR__HALT_REQUEST: 1: Sende- und Empfangseinheit abschalten.    Laufende Transfers werden noch beendet.
#define IRTE_REG_MAC_CONTROL_P3_RST__VAL                               0x00000000


#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3                                                  0x000155C4
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3__MSK_COMPARE_ENABLE                                  0x00000010
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_SHFT_COMPARE_ENABLE                                  4
// ACCESS__COMPARE_ENABLE: r,w
// DESCR__COMPARE_ENABLE: 1: Die Destinationadresse mpfangener Telegramme wird mit den\n    gespeicherten Adressen im ARC-Speicher verglichen.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3__MSK_NEGATIVE_ARC                                    0x00000008
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_SHFT_NEGATIVE_ARC                                    3
// ACCESS__NEGATIVE_ARC: r,w
// DESCR__NEGATIVE_ARC: 0: akzeptiert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und filtert alle anderen Telegramme.\n1: filtert nur Telegramme, deren Destinationadresse von der \n    ARC-Unit erkannt wird und akzeptiert alle anderen Telegramme.
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3__MSK_BROADCAST_ACCEPT                                0x00000004
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_SHFT_BROADCAST_ACCEPT                                2
// ACCESS__BROADCAST_ACCEPT: r,w
// DESCR__BROADCAST_ACCEPT: 1: Jedes Broadcast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3__MSK_GROUP_ACCEPT                                    0x00000002
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_SHFT_GROUP_ACCEPT                                    1
// ACCESS__GROUP_ACCEPT: r,w
// DESCR__GROUP_ACCEPT: 1: Jedes Multicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3__MSK_STATION_ACCEPT                                  0x00000001
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_SHFT_STATION_ACCEPT                                  0
// ACCESS__STATION_ACCEPT: r,w
// DESCR__STATION_ACCEPT: 1: Jedes Unicast-Telegramm wird empfangen
#define IRTE_REG_ADDRESS_RECOGNITION_CIRCUIT_CONTROL_P3_RST__VAL       0x00000000


#define IRTE_REG_TRANSMIT_CONTROL_P3                                                                     0x000155C8
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_COMPLETION                                                  0x00004000
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_COMPLETION                                                  14
// ACCESS__ENABLE_COMPLETION: r,w
// DESCR__ENABLE_COMPLETION: 1: Interrupt-Enable, wenn die MAC-Unit ein Telegramm sendet    oder abbricht.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_TRANSMIT_PARITY                                             0x00002000
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_TRANSMIT_PARITY                                             13
// ACCESS__ENABLE_TRANSMIT_PARITY: r,w
// DESCR__ENABLE_TRANSMIT_PARITY: 1: Interrupt-Enable, wenn beim MAC-Transmit-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_LATE_COLLISION                                              0x00001000
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_LATE_COLLISION                                              12
// ACCESS__ENABLE_LATE_COLLISION: r,w
// DESCR__ENABLE_LATE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode nach 512 TBit eine    Kollision auftritt.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_EXCESSIVE_COLLISION                                         0x00000800
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_EXCESSIVE_COLLISION                                         11
// ACCESS__ENABLE_EXCESSIVE_COLLISION: r,w
// DESCR__ENABLE_EXCESSIVE_COLLISION: 1: Interrupt-Enable, wenn im Halb-Duplexmode beim selben    Telegramm mehr als 16 Kollisionen auftreten.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_LOST_CARRIER                                                0x00000400
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_LOST_CARRIER                                                10
// ACCESS__ENABLE_LOST_CARRIER: r,w
// DESCR__ENABLE_LOST_CARRIER: 1: Interrupt-Enable, wenn ein „Carrier Sense“ Fehler auftritt
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_EXCESSIVE_DEFERAL                                           0x00000200
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_EXCESSIVE_DEFERAL                                           9
// ACCESS__ENABLE_EXCESSIVE_DEFERAL: r,w
// DESCR__ENABLE_EXCESSIVE_DEFERAL: 1: Interrupt-Enable, wenn die maximale „Deferral-Time”     überschritten wird.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_ENABLE_UNDERRUN                                                    0x00000100
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_ENABLE_UNDERRUN                                                    8
// ACCESS__ENABLE_UNDERRUN: r,w
// DESCR__ENABLE_UNDERRUN: 1: Interrupt-Enable, bei einem MAC-Transmit-FIFO Underrun
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_MII_10_MB_S_MODE                                                   0x00000080
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_MII_10_MB_S_MODE                                                   7
// ACCESS__MII_10_MB_S_MODE: r,w
// DESCR__MII_10_MB_S_MODE: 1: Aktiviert SQE-Test im MII 10 Mb/s Mode (wenn MII_Conn  = 1)
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_SEND_PAUSE                                                         0x00000040
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_SEND_PAUSE                                                         6
// ACCESS__SEND_PAUSE: r,w
// DESCR__SEND_PAUSE: 1: Veranlasst das Senden eines Pause-Telegramms    Wird nach dem Versenden dieses Pause-Telegramms von der    MAC-Unit wieder zurückgesetzt.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_NO_EXCESSIVE_DEFER                                                 0x00000020
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_NO_EXCESSIVE_DEFER                                                 5
// ACCESS__NO_EXCESSIVE_DEFER: r,w
// DESCR__NO_EXCESSIVE_DEFER: 1: Keine Test auf “Excessive Deferral”
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_FAST_BACK_OFF_AND_COUNT                                            0x00000010
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_FAST_BACK_OFF_AND_COUNT                                            4
// ACCESS__FAST_BACK_OFF_AND_COUNT: r,w
// DESCR__FAST_BACK_OFF_AND_COUNT: 1: verwendet schnellere back-off Timer und Byte Counter    (nur zum Testen)
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_SUPPRESS_CRC                                                       0x00000008
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_SUPPRESS_CRC                                                       3
// ACCESS__SUPPRESS_CRC: r,w
// DESCR__SUPPRESS_CRC: 1: MAC-Unit hängt kein CRC an das Telegramm-Ende an
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_SUPPRESS_PADDING                                                   0x00000004
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_SUPPRESS_PADDING                                                   2
// ACCESS__SUPPRESS_PADDING: r,w
// DESCR__SUPPRESS_PADDING: 1: Kein Padding für Telegramme mit weniger als 64 Bytes
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_TRANSMIT_HALT_REQUEST                                              0x00000002
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_TRANSMIT_HALT_REQUEST                                              1
// ACCESS__TRANSMIT_HALT_REQUEST: r,w
// DESCR__TRANSMIT_HALT_REQUEST: 1: Sendeeinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_TRANSMIT_CONTROL_P3__MSK_TRANSMIT_ENABLE                                                    0x00000001
#define IRTE_REG_TRANSMIT_CONTROL_P3_SHFT_TRANSMIT_ENABLE                                                    0
// ACCESS__TRANSMIT_ENABLE: r,w
// DESCR__TRANSMIT_ENABLE: 0: Sendeeinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_TRANSMIT_CONTROL_P3_RST__VAL                          0x00000000


#define IRTE_REG_TRANSMIT_STATUS_P3                                                                      0x000155CC
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_HOST_NOT_RESPONDING_TRANSMIT                                        0x00400000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_HOST_NOT_RESPONDING_TRANSMIT                                        22
// ACCESS__HOST_NOT_RESPONDING_TRANSMIT: r,
// DESCR__HOST_NOT_RESPONDING_TRANSMIT: 1: ?
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_PAUSE_TRANSMIT                                                      0x00200000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_PAUSE_TRANSMIT                                                      21
// ACCESS__PAUSE_TRANSMIT: r,
// DESCR__PAUSE_TRANSMIT: 1: MAC hat ein MAC Control PAUSE Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_MAC_CONTROL_TRANSMIT                                                0x00100000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_MAC_CONTROL_TRANSMIT                                                20
// ACCESS__MAC_CONTROL_TRANSMIT: r,
// DESCR__MAC_CONTROL_TRANSMIT: 1: MAC hat ein MAC Control Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_VLAN_TAGGED_TRANSMIT                                                0x00080000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_VLAN_TAGGED_TRANSMIT                                                19
// ACCESS__VLAN_TAGGED_TRANSMIT: r,
// DESCR__VLAN_TAGGED_TRANSMIT: 1: MAC hat ein VLAN tagged Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_BROADCAST_TRANSMIT                                                  0x00040000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_BROADCAST_TRANSMIT                                                  18
// ACCESS__BROADCAST_TRANSMIT: r,
// DESCR__BROADCAST_TRANSMIT: 1: MAC hat ein Broadcast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_MULTICAST_TRANSMIT                                                  0x00020000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_MULTICAST_TRANSMIT                                                  17
// ACCESS__MULTICAST_TRANSMIT: r,
// DESCR__MULTICAST_TRANSMIT: 1: MAC hat ein Multicast-Telegramm gesendet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_SIGNAL_QUALITY_ERROR                                                0x00010000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_SIGNAL_QUALITY_ERROR                                                16
// ACCESS__SIGNAL_QUALITY_ERROR: r,
// DESCR__SIGNAL_QUALITY_ERROR: 1: kein Empfang eines „heart beat signals“ am Telegramm-Ende
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_TRANSMISSION_HALTED                                                 0x00008000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_TRANSMISSION_HALTED                                                 15
// ACCESS__TRANSMISSION_HALTED: r,
// DESCR__TRANSMISSION_HALTED: 1: die Sendeeinheit wurde durch     „Transmit Enable : = 0“ oder „Transmit Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_COMPLETION                                                          0x00004000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_COMPLETION                                                          14
// ACCESS__COMPLETION: r,
// DESCR__COMPLETION: 1: die MAC-Unit hat ein Telegramm gesendet oder abgebrochen.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_TRANSMIT_PARITY_ERROR                                               0x00002000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_TRANSMIT_PARITY_ERROR                                               13
// ACCESS__TRANSMIT_PARITY_ERROR: r,
// DESCR__TRANSMIT_PARITY_ERROR: 1: beim MAC-Transmit-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_LATE_COLLISION                                                      0x00001000
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_LATE_COLLISION                                                      12
// ACCESS__LATE_COLLISION: r,
// DESCR__LATE_COLLISION: 1: im Halb-Duplexmode ist nach 512 TBit eine Kollision     aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_TRANSMIT_10_MB_S_STATUS                                             0x00000800
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_TRANSMIT_10_MB_S_STATUS                                             11
// ACCESS__TRANSMIT_10_MB_S_STATUS: r,
// DESCR__TRANSMIT_10_MB_S_STATUS: 1: wenn ein Telegramm über das 10-Mbit/s Interface gesendet     wird.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_LOST_CARRIER                                                        0x00000400
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_LOST_CARRIER                                                        10
// ACCESS__LOST_CARRIER: r,
// DESCR__LOST_CARRIER: 1: beim Senden ist ein „Carrier Sense“ Fehler aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_EXCESSIVE_DEFERAL                                                   0x00000200
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_EXCESSIVE_DEFERAL                                                   9
// ACCESS__EXCESSIVE_DEFERAL: r,
// DESCR__EXCESSIVE_DEFERAL: 1: die maximale „Deferral-Time” wurde überschritten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_UNDERRUN                                                            0x00000100
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_UNDERRUN                                                            8
// ACCESS__UNDERRUN: r,
// DESCR__UNDERRUN: 1: ein MAC-Transmit-FIFO Underrun ist aufgetreten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_INTERRUPT_ON_TRANSMIT                                               0x00000080
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_INTERRUPT_ON_TRANSMIT                                               7
// ACCESS__INTERRUPT_ON_TRANSMIT: r,
// DESCR__INTERRUPT_ON_TRANSMIT: 1: wenn das Senden eines Telegramms einen Interrupt auslöst.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_PAUSED                                                              0x00000040
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_PAUSED                                                              6
// ACCESS__PAUSED: r,
// DESCR__PAUSED: 1: das nächste Telegramm muss die Pause-Zeit abwarten.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_TRANSMIT_DEFERRED                                                   0x00000020
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_TRANSMIT_DEFERRED                                                   5
// ACCESS__TRANSMIT_DEFERRED: r,
// DESCR__TRANSMIT_DEFERRED: 1: wenn ein zu sendendes Telegramm im Wartezustand ist, da     die Halb-Duplex Verbindung belegt ist.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_EXCESSIVE_COLLISION                                                 0x00000010
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_EXCESSIVE_COLLISION                                                 4
// ACCESS__EXCESSIVE_COLLISION: r,
// DESCR__EXCESSIVE_COLLISION: 1: wenn beim selben Telegramm mehr als 16 Kollisionen     aufgetreten sind.
#define IRTE_REG_TRANSMIT_STATUS_P3__MSK_TRANSMIT_COLLISION_COUNT                                            0x0000000F
#define IRTE_REG_TRANSMIT_STATUS_P3_SHFT_TRANSMIT_COLLISION_COUNT                                            0
// ACCESS__TRANSMIT_COLLISION_COUNT: r,
// DESCR__TRANSMIT_COLLISION_COUNT: Anzahl der Kollisionen beim Senden eines Telegramms.
#define IRTE_REG_TRANSMIT_STATUS_P3_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_CONTROL_P3                                                                      0x000155D0
// ACCESS__: (r)(t)(s),(w)
// DESCR__: 
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_GOOD                                                         0x00004000
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_GOOD                                                         14
// ACCESS__ENABLE_GOOD: r,w
// DESCR__ENABLE_GOOD: 1: Interrupt-Enable, wenn ein Telegramm fehlerfrei empfangenwird.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_RECEIVE_PARITY                                               0x00002000
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_RECEIVE_PARITY                                               13
// ACCESS__ENABLE_RECEIVE_PARITY: r,w
// DESCR__ENABLE_RECEIVE_PARITY: 1: Interrupt-Enable, wenn beim MAC-Receive-FIFO ein     Parity-Fehler auftritt.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_LONG_ERROR                                                   0x00000800
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_LONG_ERROR                                                   11
// ACCESS__ENABLE_LONG_ERROR: r,w
// DESCR__ENABLE_LONG_ERROR: 1: Interrupt-Enable, beim Empfang von Telegrammen mit einer     Framelänge grösser als 1518 Bytes (bzw. 1522 Bytes für    VLAN), ausser das Bit „Long Enable“ ist gesetzt.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_OVERFLOW                                                     0x00000400
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_OVERFLOW                                                     10
// ACCESS__ENABLE_OVERFLOW: r,w
// DESCR__ENABLE_OVERFLOW: 1: Interrupt-Enable, bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_CRC_ERROR                                                    0x00000200
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_CRC_ERROR                                                    9
// ACCESS__ENABLE_CRC_ERROR: r,w
// DESCR__ENABLE_CRC_ERROR: 1: Interrupt-Enable, bei einem CRC-Fehler oder wenn Rx_er : = 1vom PHY.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_ENABLE_ALIGNMENT                                                    0x00000100
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_ENABLE_ALIGNMENT                                                    8
// ACCESS__ENABLE_ALIGNMENT: r,w
// DESCR__ENABLE_ALIGNMENT: 1: Interrupt-Enable, wenn ein Telegramm mit einer Framelänge    empfangen wird, die kein vielfaches eines Bytes ist und dessen    CRC fehlerhaft ist.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_IGNORE_CRC_VALUE                                                    0x00000040
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_IGNORE_CRC_VALUE                                                    6
// ACCESS__IGNORE_CRC_VALUE: r,w
// DESCR__IGNORE_CRC_VALUE: 1: CRC-Check ist abgeschaltet.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_PASS_CONTROL                                                        0x00000020
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_PASS_CONTROL                                                        5
// ACCESS__PASS_CONTROL: packets,r
// DESCR__PASS_CONTROL: 
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_STRIP_CRC_VALUE                                                     0x00000010
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_STRIP_CRC_VALUE                                                     4
// ACCESS__STRIP_CRC_VALUE: r,w
// DESCR__STRIP_CRC_VALUE: 1: CRC-Check, aber CRC wird am MAC-Interface nicht übergeben.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_SHORT_ENABLE                                                        0x00000008
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_SHORT_ENABLE                                                        3
// ACCESS__SHORT_ENABLE: r,w
// DESCR__SHORT_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    kleiner als 64 Bytes.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_LONG_ENABLE                                                         0x00000004
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_LONG_ENABLE                                                         2
// ACCESS__LONG_ENABLE: r,w
// DESCR__LONG_ENABLE: 1: erlaubt den Empfang von Telegrammen mit einer Framelänge    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_RECEIVE_HALT_REQUEST                                                0x00000002
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_RECEIVE_HALT_REQUEST                                                1
// ACCESS__RECEIVE_HALT_REQUEST: r,w
// DESCR__RECEIVE_HALT_REQUEST: 1: Empfangseinheit wird abgeschaltet.     Laufende Transfers werden noch beendet.
#define IRTE_REG_RECEIVE_CONTROL_P3__MSK_RECEIVE_ENABLE                                                      0x00000001
#define IRTE_REG_RECEIVE_CONTROL_P3_SHFT_RECEIVE_ENABLE                                                      0
// ACCESS__RECEIVE_ENABLE: r,w
// DESCR__RECEIVE_ENABLE: 0: Empfangseinheit sofort abschalten.    Laufende Transfers werden abgebrochen.
#define IRTE_REG_RECEIVE_CONTROL_P3_RST__VAL                           0x00000000


#define IRTE_REG_RECEIVE_STATUS_P3                                                                       0x000155D4
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_ARCENT                                                               0x3E000000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_ARCENT                                                               25
// ACCESS__ARCENT: r,
// DESCR__ARCENT: Offsetadresse im ARC-Speicher unter der die empfangene Adresse eingetragen ist.ARCEnt  = 11111, wenn empfangene Adresse nicht in der ARC-Unit gespeichert ist.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_ARCSTATUS                                                            0x01E00000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_ARCSTATUS                                                            21
// ACCESS__ARCSTATUS: r,
// DESCR__ARCSTATUS: Status der ARC-Unit
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_PAUSE_RECEIVED                                                       0x00100000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_PAUSE_RECEIVED                                                       20
// ACCESS__PAUSE_RECEIVED: r,
// DESCR__PAUSE_RECEIVED: 1: MAC hat ein MAC Control PAUSE Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_VLAN_RECEIVED                                                        0x00080000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_VLAN_RECEIVED                                                        19
// ACCESS__VLAN_RECEIVED: r,
// DESCR__VLAN_RECEIVED: 1: MAC hat ein VLAN tagged Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_BROADCAST_RECEIVED                                                   0x00040000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_BROADCAST_RECEIVED                                                   18
// ACCESS__BROADCAST_RECEIVED: r,
// DESCR__BROADCAST_RECEIVED: 1: MAC hat ein Broadcast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_MULTICAST_RECEIVED                                                   0x00020000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_MULTICAST_RECEIVED                                                   17
// ACCESS__MULTICAST_RECEIVED: r,
// DESCR__MULTICAST_RECEIVED: 1: MAC hat ein Multicast-Telegramm empfangen.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_RECEPTION_HALTED                                                     0x00008000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_RECEPTION_HALTED                                                     15
// ACCESS__RECEPTION_HALTED: r,
// DESCR__RECEPTION_HALTED: 1: die Empfangseinheit wurde durch „Receive Enable : = 0“ oder „Receive Halt Request : = 1“    abgeschaltet.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_GOOD_RECEIVED                                                        0x00004000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_GOOD_RECEIVED                                                        14
// ACCESS__GOOD_RECEIVED: r,
// DESCR__GOOD_RECEIVED: 1: ein Telegramm wurde fehlerfrei empfangen.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_RECEIVE_PARITY_ERROR                                                 0x00002000
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_RECEIVE_PARITY_ERROR                                                 13
// ACCESS__RECEIVE_PARITY_ERROR: r,
// DESCR__RECEIVE_PARITY_ERROR: 1: beim MAC-Receive-FIFO ist ein Parity-Fehler aufgetreten.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_LONG_ERROR                                                           0x00000800
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_LONG_ERROR                                                           11
// ACCESS__LONG_ERROR: r,
// DESCR__LONG_ERROR: 1: beim Empfang eines Telegramms mit einer Framelänte    grösser als 1518 Bytes (bzw. 1522 Bytes für VLAN).    Long Error wird nicht gesetzt, wenn in Receive-Control das Bit    „Enable Long Error“ gesetzt ist.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_OVERFLOW_ERROR                                                       0x00000400
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_OVERFLOW_ERROR                                                       10
// ACCESS__OVERFLOW_ERROR: r,
// DESCR__OVERFLOW_ERROR: 1: bei einem MAC-Receive-FIFO Overflow
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_CRC_ERROR                                                            0x00000200
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_CRC_ERROR                                                            9
// ACCESS__CRC_ERROR: r,
// DESCR__CRC_ERROR: 1: bei einem CRC-Fehler oder wenn Rx_er : = 1 ist.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_ALIGNMENT_ERROR                                                      0x00000100
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_ALIGNMENT_ERROR                                                      8
// ACCESS__ALIGNMENT_ERROR: r,
// DESCR__ALIGNMENT_ERROR: 1: wenn ein Telegramm mit einer Framelänge empfangen wurde,    die kein vielfaches eines Bytes ist und dessen CRC fehlerhaft     ist.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_RECEIVE_10_MB_S_STATUS                                               0x00000080
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_RECEIVE_10_MB_S_STATUS                                               7
// ACCESS__RECEIVE_10_MB_S_STATUS: r,
// DESCR__RECEIVE_10_MB_S_STATUS: 1: wenn dasTelegramm über das 10-Mbit/s Interface empfangen     wurde.0: wenn das Telegramm über das MII-Interface empfangen     wurde.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_INTERRUPT_ON_RECEIVE                                                 0x00000040
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_INTERRUPT_ON_RECEIVE                                                 6
// ACCESS__INTERRUPT_ON_RECEIVE: r,
// DESCR__INTERRUPT_ON_RECEIVE: 1: wenn ein Telegramm empfangen wurde und ein Interrupt-    Enable aktiviert ist. Dies schließt das Bit „Enable Good“ ein.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_CONTROL_FRAME_RECEIVED                                               0x00000020
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_CONTROL_FRAME_RECEIVED                                               5
// ACCESS__CONTROL_FRAME_RECEIVED: r,
// DESCR__CONTROL_FRAME_RECEIVED: 1: ein MAC Control Telegramm wurde empfangen (type  = 8808H)    und ARC hat die Adresse erkannt.
#define IRTE_REG_RECEIVE_STATUS_P3__MSK_LENGTH_ERROR                                                         0x00000010
#define IRTE_REG_RECEIVE_STATUS_P3_SHFT_LENGTH_ERROR                                                         4
// ACCESS__LENGTH_ERROR: r,
// DESCR__LENGTH_ERROR: 1: Length field im Telegramm stimmt nicht mit empfangener     Framelänge überein. Wird nicht gesetzt bei     VLAN-Telegrammen, MAC Control Telegrammen,     bei einem Length field kleiner als 46 und bei einer Framelängevon 64 Bytes.
#define IRTE_REG_RECEIVE_STATUS_P3_RST__VAL                            0x00000000


#define IRTE_REG_PRIM_COMMAND                                                                            0x00016400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PRIM_COMMAND__MSK_ACTIVE                                                                    0x80000000
#define IRTE_REG_PRIM_COMMAND_SHFT_ACTIVE                                                                    31
// ACCESS__ACTIVE: r,w
// DESCR__ACTIVE: 0: Kommando wird nicht mehr ausgeführt1: Kommando wird noch ausgeführt
#define IRTE_REG_PRIM_COMMAND__MSK_F_CODE                                                                    0x3E000000
#define IRTE_REG_PRIM_COMMAND_SHFT_F_CODE                                                                    25
// ACCESS__F_CODE: r,w
// DESCR__F_CODE: Function-Code:„00000“: Enable ACW-Receive-List„00001“: Disable ACW-Receive-List„00010“: Enable ACW-Send-List„00011“: Disable ACW-Send-List„00100“: Insert-Send-ACW„00101“: Remove-Send-ACW„00110“: Insert-Receive-ACW„00111“: Remove-Receive-ACW„01000“: Alter-Address-Table„01001“: Reserve-Address-Entry„01010“: Reset-Send-Queue„01011“: Clear-Address-Table“01100”: Clear-Address-Entry“01101”: Enable-Data-Image“01110”: Disable-Data-Image“01111”: Insert-Send-FCW“10000”: Remove-Send-FCW“10001”: Insert-Receive-FCW“10010”: Remove-Receive-FCW
#define IRTE_REG_PRIM_COMMAND__MSK_USER_ID                                                                   0x01C00000
#define IRTE_REG_PRIM_COMMAND_SHFT_USER_ID                                                                   22
// ACCESS__USER_ID: r,w
// DESCR__USER_ID: Kennung der Software-Instanz, wird ausschließlich von der SW verwaltet
#define IRTE_REG_PRIM_COMMAND__MSK_PRIM_PAR                                                                  0x003FFFFF
#define IRTE_REG_PRIM_COMMAND_SHFT_PRIM_PAR                                                                  0
// ACCESS__PRIM_PAR: r,w
// DESCR__PRIM_PAR: Primärer Kommandoparameter. Bedeutung abhängig von Funktion.„00010“: Start-Maske (Bit: 14...0)„00100“: Adresse des Vorgänger-ACW (Bit: 20...0)              Position des Vorgänger-ACW (Bit 21)               (‚0’: ACW / ‚1’:Listenkopf)„00101“: siehe Insert-Send-ACW („00100“)„00110“: siehe Insert-Send-ACW („00100“) „00111“: siehe Insert-Send-ACW („00100“)„01001“: Offset-Adresse in UCMC-Adresstabelle (Bit: 14...0)„01010“: Queue-Spezifikation (Bit: 18...0)„01100“: Offset-Adresse in UCMC-Adresstabelle (Bit: 14...0)„01101“: Adresse des freizugebenden Datenstatus im Abbild„01110“: Adresse des zu sperrenden Datenstatus im Abbild„01111“: Adresse des Vorgänger-FCW (Bit: 20...0)              Position des Vorgänger-FCW (Bit 21)               (‚0’: FCW / ‚1’:Listenkopf)„10000“: siehe Insert-Send-FCW („01111“)„10001“: siehe Insert-Send-FCW („01111“) „10010“: siehe Insert-Send-FCW („01111“)
#define IRTE_REG_PRIM_COMMAND_RST__VAL                                 0x00000000


#define IRTE_REG_SEC_COMMAND                                                                             0x00016404
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_SEC_COMMAND__MSK_ACTIVE                                                                     0x80000000
#define IRTE_REG_SEC_COMMAND_SHFT_ACTIVE                                                                     31
// ACCESS__ACTIVE: r,w
// DESCR__ACTIVE: 0: Kommando wird nicht mehr ausgeführt1: Kommando wird noch ausgeführt
#define IRTE_REG_SEC_COMMAND__MSK_SEC_PAR                                                                    0x7FFFFFFF
#define IRTE_REG_SEC_COMMAND_SHFT_SEC_PAR                                                                    0
// ACCESS__SEC_PAR: r,w
// DESCR__SEC_PAR: Sekundärer Kommandoparameter. Bedeutung abhängig von Funktion.„00100“: Adresse des einzukettenden ACW (Bit: 20...0)„00110“: Adresse des einzukettenden ACW (Bit: 20...0)„01111“: Adresse des einzukettenden FCW (Bit: 20...0)„10001“: Adresse des einzukettenden FCW (Bit: 20...0)
#define IRTE_REG_SEC_COMMAND_RST__VAL                                  0x00000000


#define IRTE_REG_COMMAND_RESULT                                                                          0x00016408
// ACCESS__: dr,
// DESCR__: 
#define IRTE_REG_COMMAND_RESULT__MSK_ERROR                                                                   0x80000000
#define IRTE_REG_COMMAND_RESULT_SHFT_ERROR                                                                   31
// ACCESS__ERROR: dr,
// DESCR__ERROR: 0: Kommandobearbeitung ok1: Fehler bei der Kommandoübergabe oder -bearbeitung
#define IRTE_REG_COMMAND_RESULT__MSK_CONFIRM                                                                 0x40000000
#define IRTE_REG_COMMAND_RESULT_SHFT_CONFIRM                                                                 30
// ACCESS__CONFIRM: dr,
// DESCR__CONFIRM: 0: Kommando ist in Bearbeitung1: Kommando ist fertig bearbeitet.
#define IRTE_REG_COMMAND_RESULT__MSK_F_CODE                                                                  0x3E000000
#define IRTE_REG_COMMAND_RESULT_SHFT_F_CODE                                                                  25
// ACCESS__F_CODE: dr,
// DESCR__F_CODE: Function-Code:„00000“: Enable ACW-Receive-List„00001“: Disable ACW-Receive-List„00010“: Enable ACW-Send-List„00011“: Disable ACW-Send-List„00100“: Insert-Send-ACW„00101“: Remove-Send-ACW„00110“: Insert-Receive-ACW„00111“: Remove-Receive-ACW„01000“: Alter-Address-Table„01001“: Reserve-Address-Entry„01010“: Reset-Send-Queue„01011“: Clear-Address-Table“01100”: Clear-Address-Entry“01101”: Enable-Data-Image“01110”: Disable-Data-Image“01111”: Insert-Send-FCW“10000”: Remove-Send-FCW“10001”: Insert-Receive-FCW“10010”: Remove-Receive-FCW
#define IRTE_REG_COMMAND_RESULT__MSK_USER_ID                                                                 0x01C00000
#define IRTE_REG_COMMAND_RESULT_SHFT_USER_ID                                                                 22
// ACCESS__USER_ID: dr,
// DESCR__USER_ID: Kennung der Software-Instanz, wird ausschließlich von der SW verwaltet
#define IRTE_REG_COMMAND_RESULT__MSK_RETURN_VALUE                                                            0x003FFFFF
#define IRTE_REG_COMMAND_RESULT_SHFT_RETURN_VALUE                                                            0
// ACCESS__RETURN_VALUE: dr,
// DESCR__RETURN_VALUE: Rückgabewert des abgeschlossenen Kommandos. Bedeutung abhängig von Funktion.„01001“: absolute K-RAM Adresse des reservierten statischen              Eintrags (Bit: 20...0)Alle anderen Kommandos liefern „00...00“ zurück
#define IRTE_REG_COMMAND_RESULT_RST__VAL                               0x00000000


#define IRTE_REG_PORT_CONTROL                                                                            0x0001640C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PORT_CONTROL__MSK_PORT3                                                                     0x0000F000
#define IRTE_REG_PORT_CONTROL_SHFT_PORT3                                                                     12
// ACCESS__PORT3: r,w
// DESCR__PORT3: dto.
#define IRTE_REG_PORT_CONTROL__MSK_PORT2                                                                     0x00000F00
#define IRTE_REG_PORT_CONTROL_SHFT_PORT2                                                                     8
// ACCESS__PORT2: r,w
// DESCR__PORT2: dto.
#define IRTE_REG_PORT_CONTROL__MSK_PORT1                                                                     0x000000F0
#define IRTE_REG_PORT_CONTROL_SHFT_PORT1                                                                     4
// ACCESS__PORT1: r,w
// DESCR__PORT1: dto.
#define IRTE_REG_PORT_CONTROL__MSK_PORT0                                                                     0x0000000F
#define IRTE_REG_PORT_CONTROL_SHFT_PORT0                                                                     0
// ACCESS__PORT0: r,w
// DESCR__PORT0: Bit 0: Disable Port 0Bit 1: Close Port 0Bit 2: freiBit 3: frei
#define IRTE_REG_PORT_CONTROL_RST__VAL                                 0x00000000


#define IRTE_REG_FLOW_CONTROL                                                                            0x00016410
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_FLOW_CONTROL__MSK_PAUSE_PORT3                                                               0x00000010
#define IRTE_REG_FLOW_CONTROL_SHFT_PAUSE_PORT3                                                               4
// ACCESS__PAUSE_PORT3: r,w
// DESCR__PAUSE_PORT3: dto.
#define IRTE_REG_FLOW_CONTROL__MSK_PAUSE_PORT2                                                               0x00000008
#define IRTE_REG_FLOW_CONTROL_SHFT_PAUSE_PORT2                                                               3
// ACCESS__PAUSE_PORT2: r,w
// DESCR__PAUSE_PORT2: dto.
#define IRTE_REG_FLOW_CONTROL__MSK_PAUSE_PORT1                                                               0x00000004
#define IRTE_REG_FLOW_CONTROL_SHFT_PAUSE_PORT1                                                               2
// ACCESS__PAUSE_PORT1: r,w
// DESCR__PAUSE_PORT1: dto.
#define IRTE_REG_FLOW_CONTROL__MSK_PAUSE_PORT0                                                               0x00000002
#define IRTE_REG_FLOW_CONTROL_SHFT_PAUSE_PORT0                                                               1
// ACCESS__PAUSE_PORT0: r,w
// DESCR__PAUSE_PORT0: write:‚0‘: keine Zustandsänderung ‚1‘: Setzen / Rücksetzen des Pause-Bits von Port 0read:‚0‘: not pausing‚1‘: pausing
#define IRTE_REG_FLOW_CONTROL__MSK_SET_RESET                                                                 0x00000001
#define IRTE_REG_FLOW_CONTROL_SHFT_SET_RESET                                                                 0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten Pause-Bits werden rückgesetzt (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten Pause-Bits werden gesetzt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_FLOW_CONTROL_RST__VAL                                 0x00000000


#define IRTE_REG_PRODUCER_CONTROL                                                                        0x00016414
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_PRODUCER_CONTROL__MSK_PS_SCOREBOARD_SELECT                                                  0x00000010
#define IRTE_REG_PRODUCER_CONTROL_SHFT_PS_SCOREBOARD_SELECT                                                  4
// ACCESS__PS_SCOREBOARD_SELECT: r,w
// DESCR__PS_SCOREBOARD_SELECT: write:not implementedread:‚0’: Die Applikation verwendet das PS-Scoreboard 0 (default).‚1’: Die Applikation verwendet das PS-Scoreboard 1.
#define IRTE_REG_PRODUCER_CONTROL__MSK_START_PS_AGING                                                        0x00000008
#define IRTE_REG_PRODUCER_CONTROL_SHFT_START_PS_AGING                                                        3
// ACCESS__START_PS_AGING: r,w
// DESCR__START_PS_AGING: ‚0’: Die Alterung wird am Zyklusanfang gestartet‚1’: Die Alterung wird mit dem synchronen API gestartet.
#define IRTE_REG_PRODUCER_CONTROL__MSK_ENABLE_SCOREBOARD_CHANGE                                              0x00000004
#define IRTE_REG_PRODUCER_CONTROL_SHFT_ENABLE_SCOREBOARD_CHANGE                                              2
// ACCESS__ENABLE_SCOREBOARD_CHANGE: r,w
// DESCR__ENABLE_SCOREBOARD_CHANGE: ‚0’: Das Producer-State-Scoreboard darf nicht gewechselt werden     (default).‚1’: Das Producer-State-Scoreboard darf gewechselt werden.
#define IRTE_REG_PRODUCER_CONTROL__MSK_ENABLE_SRT                                                            0x00000002
#define IRTE_REG_PRODUCER_CONTROL_SHFT_ENABLE_SRT                                                            1
// ACCESS__ENABLE_SRT: r,w
// DESCR__ENABLE_SRT: ‚0’: Die Producer-Überwachung für SRT-Daten ist disabled (default).‚1’: Die Producer-Überwachung für SRT-Daten ist enabled.
#define IRTE_REG_PRODUCER_CONTROL__MSK_ENABLE_IRT                                                            0x00000001
#define IRTE_REG_PRODUCER_CONTROL_SHFT_ENABLE_IRT                                                            0
// ACCESS__ENABLE_IRT: r,w
// DESCR__ENABLE_IRT: ‚0’: Die Producer-Überwachung für IRT-Daten ist disabled (default).‚1’: Die Producer-Überwachung für IRT-Daten ist enabled.
#define IRTE_REG_PRODUCER_CONTROL_RST__VAL                             0x00000000


#define IRTE_REG_SS_QUEUE_DISABLE                                                                        0x00016418
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_SS_QUEUE_DISABLE__MSK_SS_CHB1                                                               0x00000010
#define IRTE_REG_SS_QUEUE_DISABLE_SHFT_SS_CHB1                                                               4
// ACCESS__SS_CHB1: r,w
// DESCR__SS_CHB1: dto.
#define IRTE_REG_SS_QUEUE_DISABLE__MSK_SS_CHB0                                                               0x00000008
#define IRTE_REG_SS_QUEUE_DISABLE_SHFT_SS_CHB0                                                               3
// ACCESS__SS_CHB0: r,w
// DESCR__SS_CHB0: dto.
#define IRTE_REG_SS_QUEUE_DISABLE__MSK_SS_CHA1                                                               0x00000004
#define IRTE_REG_SS_QUEUE_DISABLE_SHFT_SS_CHA1                                                               2
// ACCESS__SS_CHA1: r,w
// DESCR__SS_CHA1: dto.
#define IRTE_REG_SS_QUEUE_DISABLE__MSK_SS_CHA0                                                               0x00000002
#define IRTE_REG_SS_QUEUE_DISABLE_SHFT_SS_CHA0                                                               1
// ACCESS__SS_CHA0: r,w
// DESCR__SS_CHA0: write:‚0‘: keine Zustandsänderung ‚1‘: Freigabe / Sperren der SS0 von Kanal Aread:‚0‘: Die SS0 von Kanal A ist freigegeben‚1‘: Die SS0 von Kanal A ist gesperrt
#define IRTE_REG_SS_QUEUE_DISABLE__MSK_SET_RESET                                                             0x00000001
#define IRTE_REG_SS_QUEUE_DISABLE_SHFT_SET_RESET                                                             0
// ACCESS__SET_RESET: r,w
// DESCR__SET_RESET: write:‚0‘: Die aktivierten SS-Bits werden freigegeben (alle anderen       Bits bleiben erhalten)‚1‘: Die aktivierten SS-Bits werden gesperrt (zusätzlich zu den       schon gesetzten Bits)read:immer ‚0‘: keine Bedeutung
#define IRTE_REG_SS_QUEUE_DISABLE_RST__VAL                             0x00000000


#define IRTE_REG_ACW_LIST_STATUS                                                                         0x0001641C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_ACW_LIST_STATUS__MSK_RECEIVE_STATE                                                          0x00000002
#define IRTE_REG_ACW_LIST_STATUS_SHFT_RECEIVE_STATE                                                          1
// ACCESS__RECEIVE_STATE: r,
// DESCR__RECEIVE_STATE: 0: Die ACW-Empfangsliste ist nicht empfangsbereit1: Die ACW-Empfangsliste ist empfangsbereit
#define IRTE_REG_ACW_LIST_STATUS__MSK_SEND_STATE                                                             0x00000001
#define IRTE_REG_ACW_LIST_STATUS_SHFT_SEND_STATE                                                             0
// ACCESS__SEND_STATE: r,
// DESCR__SEND_STATE: 0: Die ACW-Sendeliste wird (noch) nicht zyklisch aktiviert1: Die ACW-Sendeliste wird zyklisch aktiviert
#define IRTE_REG_ACW_LIST_STATUS_RST__VAL                              0x00000000


#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST                                                            0x00017000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST__MSK_L1                                                        0x00000008
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST_SHFT_L1                                                        3
// ACCESS__L1: r,w
// DESCR__L1: Pegel des aktiven Interrupts IRQ1_HP; L1  = 1  => IRQ1_HP ist einsaktiv
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST__MSK_L0                                                        0x00000004
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST_SHFT_L0                                                        2
// ACCESS__L0: r,w
// DESCR__L0: Pegel des aktiven Interrupts IRQ0_HP; L0  = 1  => IRQ0_HP ist einsaktiv
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST__MSK_R1                                                        0x00000002
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST_SHFT_R1                                                        1
// ACCESS__R1: r,w
// DESCR__R1: Read-Only-Mode, d.h. schon beim Lesen des IR1 werden die Interrupts des Host-Prozessors gelöscht
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST__MSK_R0                                                        0x00000001
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST_SHFT_R0                                                        0
// ACCESS__R0: r,w
// DESCR__R0: Read-Only-Mode, d.h. schon beim Lesen des IR0 werden die Interrupts des Host-Prozessors gelöscht
#define IRTE_REG_INTERRUPT_MODE_REGISTER_HOST_RST__VAL                 0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_HOST                                                    0x00017004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_HOST__MSK_IMR_IRT0_HP                                       0xFFFFFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_HOST_SHFT_IMR_IRT0_HP                                       0
// ACCESS__IMR_IRT0_HP: r,w
// DESCR__IMR_IRT0_HP: Interrupt-Mask-Register 0 für den HP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_HOST_RST__VAL         0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_HOST                                                    0x00017008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_HOST__MSK_IMR_NRT0_HP                                       0x007FFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_HOST_SHFT_IMR_NRT0_HP                                       0
// ACCESS__IMR_NRT0_HP: r,w
// DESCR__IMR_NRT0_HP: Interrupt-Mask-Register 0 für den HP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_HOST_RST__VAL         0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_HOST                                                    0x0001700C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_HOST__MSK_IMR_IRT1_HP                                       0xFFFFFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_HOST_SHFT_IMR_IRT1_HP                                       0
// ACCESS__IMR_IRT1_HP: r,w
// DESCR__IMR_IRT1_HP: Interrupt-Mask-Register 1 für den HP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_HOST_RST__VAL         0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_HOST                                                    0x00017010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_HOST__MSK_IMR_NRT1_HP                                       0x007FFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_HOST_SHFT_IMR_NRT1_HP                                       0
// ACCESS__IMR_NRT1_HP: r,w
// DESCR__IMR_NRT1_HP: Interrupt-Mask-Register 1 für den HP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_HOST_RST__VAL         0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_HOST                                                        0x00017400
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_HOST__MSK_IR_IRT0_HP                                            0xFFFFFFFF
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_HOST_SHFT_IR_IRT0_HP                                            0
// ACCESS__IR_IRT0_HP: r,
// DESCR__IR_IRT0_HP: Enthält alle für den IRQ0 relevanten Interruptbits des Host-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_HOST                                                        0x00017404
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_HOST__MSK_IR_NRT0_HP                                            0x007FFFFF
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_HOST_SHFT_IR_NRT0_HP                                            0
// ACCESS__IR_NRT0_HP: r,
// DESCR__IR_NRT0_HP: Enthält alle für den IRQ0 relevanten Interruptbits des Host-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_HOST                                                        0x00017408
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_HOST__MSK_IR_IRT1_HP                                            0xFFFFFFFF
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_HOST_SHFT_IR_IRT1_HP                                            0
// ACCESS__IR_IRT1_HP: r,
// DESCR__IR_IRT1_HP: Enthält alle für den IRQ1 relevanten Interruptbits des Host-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_HOST                                                        0x0001740C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_HOST__MSK_IR_NRT1_HP                                            0x007FFFFF
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_HOST_SHFT_IR_NRT1_HP                                            0
// ACCESS__IR_NRT1_HP: r,
// DESCR__IR_NRT1_HP: Enthält alle für den IRQ1 relevanten Interruptbits des Host-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_HOST                                                        0x00017410
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_HOST__MSK_IAR_IRT_HP                                            0xFFFFFFFF
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_HOST_SHFT_IAR_IRT_HP                                            0
// ACCESS__IAR_IRT_HP: ,w
// DESCR__IAR_IRT_HP: löscht die Bits im IRR des Host-Prozessors, die im IAR-Datenwort gesetzt sind
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_HOST                                                        0x00017414
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_HOST__MSK_IAR_NRT_HP                                            0x007FFFFF
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_HOST_SHFT_IAR_NRT_HP                                            0
// ACCESS__IAR_NRT_HP: ,w
// DESCR__IAR_NRT_HP: löscht die Bits im IRR des Host-Prozessors, die im IAR-Datenwort gesetzt sind
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_HOST_RST__VAL             0x00000000


#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST                                                     0x00017418
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_PS_SCOREBOARD_CHANGED                          0x80000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_PS_SCOREBOARD_CHANGED                          31
// ACCESS__INT_PS_SCOREBOARD_CHANGED: r,w
// DESCR__INT_PS_SCOREBOARD_CHANGED: Dieser Interrupt wird nur generiert, wenn im Producer_Control_Register die Enable-Bits =1 gesetzt sind und die Producer-Überwachung zugeschlagen hat.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_DMAOUTDONE                                     0x40000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_DMAOUTDONE                                     30
// ACCESS__INT_DMAOUTDONE: r,w
// DESCR__INT_DMAOUTDONE: Dieser Interrupt wird nur generiert, wenn im ImageMode_Register das Bit Enable_AsyncDMA =1 gesetzt ist. Der DMA-Transfer der Ausgangsdaten ist abgeschlossen und die asynchrone Applikation kann weiterlaufen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_READYFORDMAIN                                  0x20000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_READYFORDMAIN                                  29
// ACCESS__INT_READYFORDMAIN: r,w
// DESCR__INT_READYFORDMAIN: Dieser Interrupt wird nur generiert, wenn im ImageMode_Register das Bit Enable_AsyncDMA =1 gesetzt ist. Das Synchon-Buffered interface ist bereit die Eingangsdaten per DMA zu transferieren. Die asynchrone Applikation muss angehalten und danach im IRT_asyncMode_Register das Bit DMA_InEnable =1 gesetzt werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_SRT_CYCLE_SKIPPED                                  0x10000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_SRT_CYCLE_SKIPPED                                  28
// ACCESS__SRT_CYCLE_SKIPPED: r,w
// DESCR__SRT_CYCLE_SKIPPED: Zu Beginn der neuen iSRT-Phase sind nicht alle SRT-FCWs versendet. Die ACW-Liste wird ohne Transfer-Auftrag durchlaufen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_PLL_EXT_IN                                         0x08000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_PLL_EXT_IN                                         27
// ACCESS__PLL_EXT_IN: r,w
// DESCR__PLL_EXT_IN: Ein fallende Flanke am „PLL-Ext-In“-Eingang wurde erkannt
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_PS_SCOREBOARD_SKIPPED                          0x04000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_PS_SCOREBOARD_SKIPPED                          26
// ACCESS__INT_PS_SCOREBOARD_SKIPPED: r,w
// DESCR__INT_PS_SCOREBOARD_SKIPPED: Ein erneuter Start der Producer-Überwachung kann nicht ausgeführt werden, da die vorherige Überwachung noch nicht abgeschlossen ist.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_OUT_OF_SYNC_SND                                    0x02000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_OUT_OF_SYNC_SND                                    25
// ACCESS__OUT_OF_SYNC_SND: r,w
// DESCR__OUT_OF_SYNC_SND: Ein Datensatz für ein synchrones Abbild sollte ausserhalb der Transferphase versendet werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_OUT_OF_SYNC_RCV                                    0x01000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_OUT_OF_SYNC_RCV                                    24
// ACCESS__OUT_OF_SYNC_RCV: r,w
// DESCR__OUT_OF_SYNC_RCV: Ein Telegramm für ein synchrones Abbild wird ausserhalb der Transferphase empfangen und nicht ins Abbild eingetragen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_ISRT_DURATION_FAULT                                0x00800000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_ISRT_DURATION_FAULT                                23
// ACCESS__ISRT_DURATION_FAULT: r,w
// DESCR__ISRT_DURATION_FAULT: Die iSRT-Phase hat einen Wert erreicht, der keinen NRT-Verkehr mehr zulässt.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_ACWLIST_TERMINATION                            0x00400000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_ACWLIST_TERMINATION                            22
// ACCESS__INT_ACWLIST_TERMINATION: r,w
// DESCR__INT_ACWLIST_TERMINATION: Die Bearbeitung der ACW-Sendeliste dauert länger als der Isochronzyklus
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_COMP_4_0                                           0x003E0000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_COMP_4_0                                           17
// ACCESS__COMP_4_0: r,w
// DESCR__COMP_4_0: Komparator-Interrupts der Komparatoren des AplClk
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_TCW_DONE                                       0x00010000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_TCW_DONE                                       16
// ACCESS__INT_TCW_DONE: r,w
// DESCR__INT_TCW_DONE: Ein bereitgestelltes TCW, wurde aufgrund des TCW-Schwellen-Überlaufes oder durch SW-Anstoß ausgeführt
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_FIFODATA_LOST                                  0x00008000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_FIFODATA_LOST                                  15
// ACCESS__INT_FIFODATA_LOST: r,w
// DESCR__INT_FIFODATA_LOST: Ein Trace-Eintrag ging aufgrund des Fifo-Überlaufs in der Trace-Control verloren
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_CLKCY                                              0x00004000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_CLKCY                                              14
// ACCESS__CLKCY: r,w
// DESCR__CLKCY: eine aktive Flanke des Clkcy ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_CLKCY_INT                                          0x00002000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_CLKCY_INT                                          13
// ACCESS__CLKCY_INT: r,w
// DESCR__CLKCY_INT: eine aktive Flanke des Clkcy-Int ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_APLCLK                                             0x00001000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_APLCLK                                             12
// ACCESS__APLCLK: r,w
// DESCR__APLCLK: eine aktive Flanke des AplClk ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_FATAL_IRT_SRTLIST_ERR                          0x00000800
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_FATAL_IRT_SRTLIST_ERR                          11
// ACCESS__INT_FATAL_IRT_SRTLIST_ERR: r,w
// DESCR__INT_FATAL_IRT_SRTLIST_ERR: In einem FCW, TCW oder einer IRT-Listenstruktur ist ein fundamentaler Parametrierungsfehler (Opcode, Länge, Adresse) aufgetreten. Der DMA transferiert bei gepuffertem Abbild in einen illegalen Speicherbereich.In einem ACW oder einer zugeordneten Buffer-Strukur im K-RAM ist ein fundamentaler Parametrierungsfehler (Länge, Adressbereich) aufgetreten.Eines der parametrierten Scoreboards (Producer-Überwachung) führt zu einem illegalen Speicherzugriff.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT2_SND_IRT_SRT_TEL                               0x00000400
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT2_SND_IRT_SRT_TEL                               10
// ACCESS__INT2_SND_IRT_SRT_TEL: r,w
// DESCR__INT2_SND_IRT_SRT_TEL: Senden eines IRT-Telegramms, dessen IRT2-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT1_SND_IRT_SRT_TEL                               0x00000200
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT1_SND_IRT_SRT_TEL                               9
// ACCESS__INT1_SND_IRT_SRT_TEL: r,w
// DESCR__INT1_SND_IRT_SRT_TEL: Senden eines IRT-Telegramms, dessen IRT1-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT2_RCV_IRT_SRT_TEL                               0x00000100
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT2_RCV_IRT_SRT_TEL                               8
// ACCESS__INT2_RCV_IRT_SRT_TEL: r,w
// DESCR__INT2_RCV_IRT_SRT_TEL: Empfang eines IRT-Telegramms, dessen IRT2-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT1_RCV_IRT_SRT_TEL                               0x00000080
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT1_RCV_IRT_SRT_TEL                               7
// ACCESS__INT1_RCV_IRT_SRT_TEL: r,w
// DESCR__INT1_RCV_IRT_SRT_TEL: Empfang eines IRT-Telegramms, dessen IRT1-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_OUTFAULT                                       0x00000040
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_OUTFAULT                                       6
// ACCESS__INT_OUTFAULT: r,w
// DESCR__INT_OUTFAULT: Die synchrone Anwendung konnte die Bearbeitung der Ausgangsdaten bis zum Beginn des neuen Isochronzyklus nicht abschließen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_INFAULT                                        0x00000020
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_INFAULT                                        5
// ACCESS__INT_INFAULT: r,w
// DESCR__INT_INFAULT: Die synchrone Anwendung konnte die Bearbeitung der Eingangsdaten bis zum Beginn des neuen Isochronzyklus nicht abschließen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_DMAOUT                                         0x00000010
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_DMAOUT                                         4
// ACCESS__INT_DMAOUT: r,w
// DESCR__INT_DMAOUT: Der DMA-Transfer der Ausgangsdaten konnte im synchron gepufferten und Wechselpuffer-Betrieb bis zum Beginn des neuen Isochronzyklus nicht abgeschlossen werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_DMAIN                                          0x00000008
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_DMAIN                                          3
// ACCESS__INT_DMAIN: r,w
// DESCR__INT_DMAIN: Der DMA-Transfer der Eingangsdaten konnte im synchron gepufferten und Wechselpuffer-Betrieb bis zum Beginn des neuen Isochronzyklus nicht abgeschlossen werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_IRT_TRANSFEND                                  0x00000004
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_IRT_TRANSFEND                                  2
// ACCESS__INT_IRT_TRANSFEND: r,w
// DESCR__INT_IRT_TRANSFEND: Der IRT-Transfer ist abgeschlossen. Der Interrupt wird entweder durch den Übergang in den NRT-Zeitbereich gebildet oder aufgrund eines Vergleichswertes zur Cycle-Time in der IRT-Control
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_STARTOP                                        0x00000002
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_STARTOP                                        1
// ACCESS__INT_STARTOP: r,w
// DESCR__INT_STARTOP: Startsignal für die synchrone Anwendung
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_NEWCYCLE                                       0x00000001
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_SHFT_INT_NEWCYCLE                                       0
// ACCESS__INT_NEWCYCLE: r,w
// DESCR__INT_NEWCYCLE: Beginn eines neuen Isochronzyklus
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST_RST__VAL          0x00000000


#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH                                                          0x00017014
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH__MSK_L1                                                      0x00000008
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH_SHFT_L1                                                      3
// ACCESS__L1: r,w
// DESCR__L1: Pegel des aktiven Interrupts IRQ1_HP; L1  = 1  => IRQ1_HP ist einsaktiv
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH__MSK_L0                                                      0x00000004
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH_SHFT_L0                                                      2
// ACCESS__L0: r,w
// DESCR__L0: Pegel des aktiven Interrupts IRQ0_HP; L0  = 1  => IRQ0_HP ist einsaktiv
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH__MSK_R1                                                      0x00000002
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH_SHFT_R1                                                      1
// ACCESS__R1: r,w
// DESCR__R1: Read-Only-Mode, d.h. schon beim Lesen des IR1 werden die Interrupts des Host-Prozessors gelöscht
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH__MSK_R0                                                      0x00000001
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH_SHFT_R0                                                      0
// ACCESS__R0: r,w
// DESCR__R0: Read-Only-Mode, d.h. schon beim Lesen des IR0 werden die Interrupts des Host-Prozessors gelöscht
#define IRTE_REG_INTERRUPT_MODE_REGISTER_SWITCH_RST__VAL               0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_SWITCH                                                  0x00017018
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_SWITCH__MSK_IMR_IRT0_SP                                     0xFFFFFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_SWITCH_SHFT_IMR_IRT0_SP                                     0
// ACCESS__IMR_IRT0_SP: r,w
// DESCR__IMR_IRT0_SP: Interrupt-Mask-Register 0 für den SP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ0_SWITCH_RST__VAL       0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_SWITCH                                                  0x0001701C
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_SWITCH__MSK_IMR_NRT0_SP                                     0x007FFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_SWITCH_SHFT_IMR_NRT0_SP                                     0
// ACCESS__IMR_NRT0_SP: r,w
// DESCR__IMR_NRT0_SP: Interrupt-Mask-Register 0 für den SP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ0_SWITCH_RST__VAL       0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_SWITCH                                                  0x00017020
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_SWITCH__MSK_IMR_IRT1_SP                                     0xFFFFFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_SWITCH_SHFT_IMR_IRT1_SP                                     0
// ACCESS__IMR_IRT1_SP: r,w
// DESCR__IMR_IRT1_SP: Interrupt-Mask-Register 1 für den SP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_IRT_IRQ1_SWITCH_RST__VAL       0x00000000


#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_SWITCH                                                  0x00017024
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_SWITCH__MSK_IMR_NRT1_SP                                     0x007FFFFF
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_SWITCH_SHFT_IMR_NRT1_SP                                     0
// ACCESS__IMR_NRT1_SP: r,w
// DESCR__IMR_NRT1_SP: Interrupt-Mask-Register 1 für den SP; wenn ein Bit eine 0 trägt, ist der entspre-chende Interrupt wegmaskiert; nach Reset sind alle Interrupts wegmaskiert;
#define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_SWITCH_RST__VAL       0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_SWITCH                                                      0x00017428
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_SWITCH__MSK_IR_IRT0_SP                                          0xFFFFFFFF
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_SWITCH_SHFT_IR_IRT0_SP                                          0
// ACCESS__IR_IRT0_SP: r,
// DESCR__IR_IRT0_SP: Enthält alle für den IRQ0 relevanten Interruptbits des Switch-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ0_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_SWITCH                                                      0x0001742C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_SWITCH__MSK_IR_NRT0_SP                                          0x007FFFFF
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_SWITCH_SHFT_IR_NRT0_SP                                          0
// ACCESS__IR_NRT0_SP: r,
// DESCR__IR_NRT0_SP: Enthält alle für den IRQ0 relevanten Interruptbits des Switch-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ0_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_SWITCH                                                      0x00017430
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_SWITCH__MSK_IR_IRT1_SP                                          0xFFFFFFFF
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_SWITCH_SHFT_IR_IRT1_SP                                          0
// ACCESS__IR_IRT1_SP: r,
// DESCR__IR_IRT1_SP: Enthält alle für den IRQ1 relevanten Interruptbits des Switch-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_IRT_IRQ1_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_SWITCH                                                      0x00017434
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_SWITCH__MSK_IR_NRT1_SP                                          0x007FFFFF
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_SWITCH_SHFT_IR_NRT1_SP                                          0
// ACCESS__IR_NRT1_SP: r,
// DESCR__IR_NRT1_SP: Enthält alle für den IRQ1 relevanten Interruptbits des Switch-Prozessors, die nicht durch das zugehörige Interrupt-Mask-Register gefiltert worden sind
#define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_SWITCH                                                      0x00017438
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_SWITCH__MSK_IAR_IRT_SP                                          0xFFFFFFFF
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_SWITCH_SHFT_IAR_IRT_SP                                          0
// ACCESS__IAR_IRT_SP: ,w
// DESCR__IAR_IRT_SP: löscht die Bits im IRR des Switch-Prozessors, die im IAR-Datenwort gesetzt sind
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_IRT_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_SWITCH                                                      0x0001743C
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_SWITCH__MSK_IAR_NRT_SP                                          0x007FFFFF
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_SWITCH_SHFT_IAR_NRT_SP                                          0
// ACCESS__IAR_NRT_SP: ,w
// DESCR__IAR_NRT_SP: löscht die Bits im IRR des Switch-Prozessors, die im IAR-Datenwort gesetzt sind
#define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_SWITCH_RST__VAL           0x00000000


#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH                                                   0x00017440
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_PS_SCOREBOARD_CHANGED                        0x80000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_PS_SCOREBOARD_CHANGED                        31
// ACCESS__INT_PS_SCOREBOARD_CHANGED: r,w
// DESCR__INT_PS_SCOREBOARD_CHANGED: Dieser Interrupt wird nur generiert, wenn im Producer_Control_Register die Enable-Bits =1 gesetzt sind und die Producer-Überwachung zugeschlagen hat.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_DMAOUTDONE                                   0x40000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_DMAOUTDONE                                   30
// ACCESS__INT_DMAOUTDONE: r,w
// DESCR__INT_DMAOUTDONE: Dieser Interrupt wird nur generiert, wenn im ImageMode_Register das Bit Enable_AsyncDMA =1 gesetzt ist. Der DMA-Transfer der Ausgangsdaten ist abgeschlossen und die asynchrone Applikation kann weiterlaufen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_READYFORDMAIN                                0x20000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_READYFORDMAIN                                29
// ACCESS__INT_READYFORDMAIN: r,w
// DESCR__INT_READYFORDMAIN: Dieser Interrupt wird nur generiert, wenn im ImageMode_Register das Bit Enable_AsyncDMA =1 gesetzt ist. Das Synchon-Buffered interface ist bereit die Eingangsdaten per DMA zu transferieren. Die asynchrone Applikation muss angehalten und danach im IRT_asyncMode_Register das Bit DMA_InEnable =1 gesetzt werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_SRT_CYCLE_SKIPPED                                0x10000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_SRT_CYCLE_SKIPPED                                28
// ACCESS__SRT_CYCLE_SKIPPED: r,w
// DESCR__SRT_CYCLE_SKIPPED: Zu Beginn der neuen iSRT-Phase sind nicht alle SRT-FCWs versendet. Die ACW-Liste wird ohne Transfer-Auftrag durchlaufen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_PLL_EXT_IN                                       0x08000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_PLL_EXT_IN                                       27
// ACCESS__PLL_EXT_IN: r,w
// DESCR__PLL_EXT_IN: Ein fallende Flanke am „PLL-Ext-In“-Eingang wurde erkannt
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_PS_SCOREBOARD_SKIPPED                        0x04000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_PS_SCOREBOARD_SKIPPED                        26
// ACCESS__INT_PS_SCOREBOARD_SKIPPED: r,w
// DESCR__INT_PS_SCOREBOARD_SKIPPED: Ein erneuter Start der Producer-Überwachung kann nicht ausgeführt werden, da die vorherige Überwachung noch nicht abgeschlossen ist.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_OUT_OF_SYNC_SND                                  0x02000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_OUT_OF_SYNC_SND                                  25
// ACCESS__OUT_OF_SYNC_SND: r,w
// DESCR__OUT_OF_SYNC_SND: Ein Datensatz für ein synchrones Abbild sollte ausserhalb der Transferphase versendet werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_OUT_OF_SYNC_RCV                                  0x01000000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_OUT_OF_SYNC_RCV                                  24
// ACCESS__OUT_OF_SYNC_RCV: r,w
// DESCR__OUT_OF_SYNC_RCV: Ein Telegramm für ein synchrones Abbild wird ausserhalb der Transferphase empfangen und nicht ins Abbild eingetragen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_ISRT_DURATION_FAULT                              0x00800000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_ISRT_DURATION_FAULT                              23
// ACCESS__ISRT_DURATION_FAULT: r,w
// DESCR__ISRT_DURATION_FAULT: Die iSRT-Phase hat einen Wert erreicht, der keinen NRT-Verkehr mehr zulässt.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_ACWLIST_TERMINATION                          0x00400000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_ACWLIST_TERMINATION                          22
// ACCESS__INT_ACWLIST_TERMINATION: r,w
// DESCR__INT_ACWLIST_TERMINATION: Die Bearbeitung der ACW-Sendeliste dauert länger als der Isochronzyklus
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_COMP_4_0                                         0x003E0000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_COMP_4_0                                         17
// ACCESS__COMP_4_0: r,w
// DESCR__COMP_4_0: Komparator-Interrupts der Komparatoren des AplClk
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_TCW_DONE                                     0x00010000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_TCW_DONE                                     16
// ACCESS__INT_TCW_DONE: r,w
// DESCR__INT_TCW_DONE: Ein bereitgestelltes TCW, wurde aufgrund des TCW-Schwellen-Überlaufes oder durch SW-Anstoß ausgeführt
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_FIFODATA_LOST                                0x00008000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_FIFODATA_LOST                                15
// ACCESS__INT_FIFODATA_LOST: r,w
// DESCR__INT_FIFODATA_LOST: Ein Trace-Eintrag ging aufgrund des Fifo-Überlaufs in der Trace-Control verloren
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_CLKCY                                            0x00004000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_CLKCY                                            14
// ACCESS__CLKCY: r,w
// DESCR__CLKCY: eine aktive Flanke des Clkcy ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_CLKCY_INT                                        0x00002000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_CLKCY_INT                                        13
// ACCESS__CLKCY_INT: r,w
// DESCR__CLKCY_INT: eine aktive Flanke des Clkcy-Int ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_APLCLK                                           0x00001000
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_APLCLK                                           12
// ACCESS__APLCLK: r,w
// DESCR__APLCLK: eine aktive Flanke des AplClk ist aufgetreten
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_FATAL_IRT_SRTLIST_ERR                        0x00000800
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_FATAL_IRT_SRTLIST_ERR                        11
// ACCESS__INT_FATAL_IRT_SRTLIST_ERR: r,w
// DESCR__INT_FATAL_IRT_SRTLIST_ERR: In einem FCW, TCW oder einer IRT-Listenstruktur ist ein fundamentaler Parametrierungsfehler (Opcode, Länge, Adresse) aufgetreten. Der DMA transferiert bei gepuffertem Abbild in einen illegalen Speicherbereich.In einem ACW oder einer zugeordneten Buffer-Strukur im K-RAM ist ein fundamentaler Parametrierungsfehler (Länge, Adressbereich) aufgetreten.Eines der parametrierten Scoreboards (Producer-Überwachung) führt zu einem illegalen Speicherzugriff.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT2_SND_IRT_SRT_TEL                             0x00000400
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT2_SND_IRT_SRT_TEL                             10
// ACCESS__INT2_SND_IRT_SRT_TEL: r,w
// DESCR__INT2_SND_IRT_SRT_TEL: Senden eines IRT-Telegramms, dessen IRT2-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT1_SND_IRT_SRT_TEL                             0x00000200
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT1_SND_IRT_SRT_TEL                             9
// ACCESS__INT1_SND_IRT_SRT_TEL: r,w
// DESCR__INT1_SND_IRT_SRT_TEL: Senden eines IRT-Telegramms, dessen IRT1-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT2_RCV_IRT_SRT_TEL                             0x00000100
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT2_RCV_IRT_SRT_TEL                             8
// ACCESS__INT2_RCV_IRT_SRT_TEL: r,w
// DESCR__INT2_RCV_IRT_SRT_TEL: Empfang eines IRT-Telegramms, dessen IRT2-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT1_RCV_IRT_SRT_TEL                             0x00000080
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT1_RCV_IRT_SRT_TEL                             7
// ACCESS__INT1_RCV_IRT_SRT_TEL: r,w
// DESCR__INT1_RCV_IRT_SRT_TEL: Empfang eines IRT-Telegramms, dessen IRT1-Bit im zugehörigen FCW gesetzt ist
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_OUTFAULT                                     0x00000040
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_OUTFAULT                                     6
// ACCESS__INT_OUTFAULT: r,w
// DESCR__INT_OUTFAULT: Die synchrone Anwendung konnte die Bearbeitung der Ausgangsdaten bis zum Beginn des neuen Isochronzyklus nicht abschließen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_INFAULT                                      0x00000020
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_INFAULT                                      5
// ACCESS__INT_INFAULT: r,w
// DESCR__INT_INFAULT: Die synchrone Anwendung konnte die Bearbeitung der Eingangsdaten bis zum Beginn des neuen Isochronzyklus nicht abschließen.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_DMAOUT                                       0x00000010
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_DMAOUT                                       4
// ACCESS__INT_DMAOUT: r,w
// DESCR__INT_DMAOUT: Der DMA-Transfer der Ausgangsdaten konnte im synchron gepufferten und Wechselpuffer-Betrieb bis zum Beginn des neuen Isochronzyklus nicht abgeschlossen werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_DMAIN                                        0x00000008
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_DMAIN                                        3
// ACCESS__INT_DMAIN: r,w
// DESCR__INT_DMAIN: Der DMA-Transfer der Eingangsdaten konnte im synchron gepufferten und Wechselpuffer-Betrieb bis zum Beginn des neuen Isochronzyklus nicht abgeschlossen werden.
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_IRT_TRANSFEND                                0x00000004
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_IRT_TRANSFEND                                2
// ACCESS__INT_IRT_TRANSFEND: r,w
// DESCR__INT_IRT_TRANSFEND: Der IRT-Transfer ist abgeschlossen. Der Interrupt wird entweder durch den Übergang in den NRT-Zeitbereich gebildet oder aufgrund eines Vergleichswertes zur Cycle-Time in der IRT-Control
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_STARTOP                                      0x00000002
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_STARTOP                                      1
// ACCESS__INT_STARTOP: r,w
// DESCR__INT_STARTOP: Startsignal für die synchrone Anwendung
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH__MSK_INT_NEWCYCLE                                     0x00000001
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_SHFT_INT_NEWCYCLE                                     0
// ACCESS__INT_NEWCYCLE: r,w
// DESCR__INT_NEWCYCLE: Beginn eines neuen Isochronzyklus
#define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_SWITCH_RST__VAL        0x00000000


#define IRTE_REG_ACTIVATE_HP_INTERRUPT                                                                   0x00017450
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_ACTIVATE_HP_INTERRUPT__MSK_ACTIVATE_HP_INTERRUPT                                            0xFFFFFFFF
#define IRTE_REG_ACTIVATE_HP_INTERRUPT_SHFT_ACTIVATE_HP_INTERRUPT                                            0
// ACCESS__ACTIVATE_HP_INTERRUPT: ,w
// DESCR__ACTIVATE_HP_INTERRUPT: write:Ein Schreibvorgang auf dieses Register löst den HP-Int im Switch- und im Host-Interrupt-Request-Register aus.\nread:not implemented
#define IRTE_REG_ACTIVATE_HP_INTERRUPT_RST__VAL                        0x00000000


#define IRTE_REG_ACTIVATE_SP_INTERRUPT                                                                   0x00017454
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_ACTIVATE_SP_INTERRUPT__MSK_ACTIVATE_SP_INTERRUPT                                            0xFFFFFFFF
#define IRTE_REG_ACTIVATE_SP_INTERRUPT_SHFT_ACTIVATE_SP_INTERRUPT                                            0
// ACCESS__ACTIVATE_SP_INTERRUPT: ,w
// DESCR__ACTIVATE_SP_INTERRUPT: write:Ein Schreibvorgang auf dieses Register löst den SP-Int im Switch- und im Host-Interrupt-Request-Register aus.\nread:not implemented
#define IRTE_REG_ACTIVATE_SP_INTERRUPT_RST__VAL                        0x00000000


#define IRTE_REG_TRACE_BASE_ADDRESS                                                                      0x00018000
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRACE_BASE_ADDRESS__MSK_IRT_TRACE_UNIT_BASE_ADDRESS                                         0x001FFFFF
#define IRTE_REG_TRACE_BASE_ADDRESS_SHFT_IRT_TRACE_UNIT_BASE_ADDRESS                                         0
// ACCESS__IRT_TRACE_UNIT_BASE_ADDRESS: r,w
// DESCR__IRT_TRACE_UNIT_BASE_ADDRESS: Basis-Adresse der Trace-Unit im K-RAM (die Basis-Adresse liegt immer Wort (64-Bit) aligned, d.h. die untersten drei Adressbits werden ignoriert)
#define IRTE_REG_TRACE_BASE_ADDRESS_RST__VAL                           0x00000000


#define IRTE_REG_TRACE_BUFFER_LENGTH                                                                     0x00018004
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRACE_BUFFER_LENGTH__MSK_IRT_TRACE_BUFFER_LENGTH                                            0x0000FFFF
#define IRTE_REG_TRACE_BUFFER_LENGTH_SHFT_IRT_TRACE_BUFFER_LENGTH                                            0
// ACCESS__IRT_TRACE_BUFFER_LENGTH: r,w
// DESCR__IRT_TRACE_BUFFER_LENGTH: Anzahl der Diagnoseeinträge die der Trace-Buffer aufnehmen kann. Ein Diagnoseeintrag umfasst 128-Bit. Wenn der IRT-Trace-Buffer Entry-Counter diesen Wert erreicht hat, unterbleibt ein Eintrag in den Trace-Buffer, es wird nur der IRT-Trace-Buffer Fault-Counter inkrementiert.
#define IRTE_REG_TRACE_BUFFER_LENGTH_RST__VAL                          0x00000000


#define IRTE_REG_TRACE_TRANSFER_LIMIT                                                                    0x00018008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRACE_TRANSFER_LIMIT__MSK_IRT_TRACE_UNIT_INTERRUPT_SCHWELLE                                 0x0000FFFF
#define IRTE_REG_TRACE_TRANSFER_LIMIT_SHFT_IRT_TRACE_UNIT_INTERRUPT_SCHWELLE                                 0
// ACCESS__IRT_TRACE_UNIT_INTERRUPT_SCHWELLE: r,w
// DESCR__IRT_TRACE_UNIT_INTERRUPT_SCHWELLE: Immer wenn diese Anzahl der Einträge in der Trace-Unit überschritten ist, wird ein Interrupt erzeugt
#define IRTE_REG_TRACE_TRANSFER_LIMIT_RST__VAL                         0x00000000


#define IRTE_REG_TRACE_COMMAND_REGISTER                                                                  0x00018400
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRACE_COMMAND_REGISTER__MSK_COMMAND                                                         0xFFFFFFFF
#define IRTE_REG_TRACE_COMMAND_REGISTER_SHFT_COMMAND                                                         0
// ACCESS__COMMAND: r,w
// DESCR__COMMAND: Zeiger auf ein TCW im User-RAM, in dem der auszuführende Befehl kodiert ist, Byteadresse, Wort aligned, d.h. wenn das User-Ram im K-Ram liegt: Wort  = 64-Bit; wenn das User-Ram im Ext-Ram liegt: Wort  = 32-Bit;
#define IRTE_REG_TRACE_COMMAND_REGISTER_RST__VAL                       0x00000000


#define IRTE_REG_TRACE_MODE                                                                              0x00018404
// ACCESS__: ,w
// DESCR__: 
#define IRTE_REG_TRACE_MODE__MSK_EXECUTE_TCW                                                                 0x00000100
#define IRTE_REG_TRACE_MODE_SHFT_EXECUTE_TCW                                                                 8
// ACCESS__EXECUTE_TCW: ,w
// DESCR__EXECUTE_TCW: Abarbeitung des TCW anstossen
#define IRTE_REG_TRACE_MODE__MSK_BUFFER_MODE                                                                 0x00000008
#define IRTE_REG_TRACE_MODE_SHFT_BUFFER_MODE                                                                 3
// ACCESS__BUFFER_MODE: ,w
// DESCR__BUFFER_MODE: 0: Wenn Trace-Buffer voll, kein weiterer Eintrag1: Wenn Trace-Buffer voll, wird das erste Element überschriebenBei beiden Betriebsarten:IRT-Trace-Buffer Entry-Counter bleibt unverändertIRT-Trace-Buffer Fault-Counter wird inkrementiertIm jeweilig letzten Eintrag wird LO-Bit (Diagnose-Eintrag verloren) gesetzt
#define IRTE_REG_TRACE_MODE__MSK_START_TRACE                                                                 0x00000004
#define IRTE_REG_TRACE_MODE_SHFT_START_TRACE                                                                 2
// ACCESS__START_TRACE: ,w
// DESCR__START_TRACE: Starten/Fortsetzen der Aufzeichnung
#define IRTE_REG_TRACE_MODE__MSK_STOP_TRACE                                                                  0x00000002
#define IRTE_REG_TRACE_MODE_SHFT_STOP_TRACE                                                                  1
// ACCESS__STOP_TRACE: ,w
// DESCR__STOP_TRACE: Anhalten der Aufzeichnung
#define IRTE_REG_TRACE_MODE__MSK_RESET_TRACE                                                                 0x00000001
#define IRTE_REG_TRACE_MODE_SHFT_RESET_TRACE                                                                 0
// ACCESS__RESET_TRACE: ,w
// DESCR__RESET_TRACE: Rücksetzen der Aufzeichnung, d.h. IRT-Trace-Buffer Entry-Counter, IRT-Trace-Buffer Fault-Counter und die Schreib-Lese-Steuerung werden auf Null zurückgesetzt.
#define IRTE_REG_TRACE_MODE_RST__VAL                                   0x00000000


#define IRTE_REG_TRACE_ENTRY_COUNTER                                                                     0x00018408
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRACE_ENTRY_COUNTER__MSK_IRT_TRACE_UNIT_ENTRY_COUNTER                                       0x0000FFFF
#define IRTE_REG_TRACE_ENTRY_COUNTER_SHFT_IRT_TRACE_UNIT_ENTRY_COUNTER                                       0
// ACCESS__IRT_TRACE_UNIT_ENTRY_COUNTER: r,
// DESCR__IRT_TRACE_UNIT_ENTRY_COUNTER: Zähler, der bei jedem Eintrag in die Trace-Unit inkrementiert und bei jedem Austrag aus der Trace-Unit dekrementiert wird.
#define IRTE_REG_TRACE_ENTRY_COUNTER_RST__VAL                          0x00000000


#define IRTE_REG_TRACE_FAULT_COUNTER                                                                     0x0001840C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TRACE_FAULT_COUNTER__MSK_IRT_TRACE_UNIT_FAULT_COUNTER                                       0x0000FFFF
#define IRTE_REG_TRACE_FAULT_COUNTER_SHFT_IRT_TRACE_UNIT_FAULT_COUNTER                                       0
// ACCESS__IRT_TRACE_UNIT_FAULT_COUNTER: r,
// DESCR__IRT_TRACE_UNIT_FAULT_COUNTER: Zähler, der bei jedem nicht erfolgreichen Eintrag oder beim Überschreiben eines alten Eintrags in die Trace-Unit inkrementiert wird.
#define IRTE_REG_TRACE_FAULT_COUNTER_RST__VAL                          0x00000000


#define IRTE_REG_ETHER_RT_TYPE0                                                                          0x00019000
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_ETHER_RT_TYPE0__MSK_RT_TYPEID                                                               0x0000FFFF
#define IRTE_REG_ETHER_RT_TYPE0_SHFT_RT_TYPEID                                                               0
// ACCESS__RT_TYPEID: ,
// DESCR__RT_TYPEID: Typ-Kennung eines RT-Telegramms
#define IRTE_REG_ETHER_RT_TYPE0_RST__VAL                               0x00000000


#define IRTE_REG_ETHER_RT_TYPE1                                                                          0x00019004
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_ETHER_RT_TYPE1__MSK_RT_TYPEID                                                               0x0000FFFF
#define IRTE_REG_ETHER_RT_TYPE1_SHFT_RT_TYPEID                                                               0
// ACCESS__RT_TYPEID: ,
// DESCR__RT_TYPEID: Typ-Kennung eines RT-Telegramms
#define IRTE_REG_ETHER_RT_TYPE1_RST__VAL                               0x00000000


#define IRTE_REG_ETHER_RT_TYPE2                                                                          0x00019008
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_ETHER_RT_TYPE2__MSK_RT_TYPEID                                                               0x0000FFFF
#define IRTE_REG_ETHER_RT_TYPE2_SHFT_RT_TYPEID                                                               0
// ACCESS__RT_TYPEID: ,
// DESCR__RT_TYPEID: Typ-Kennung eines RT-Telegramms
#define IRTE_REG_ETHER_RT_TYPE2_RST__VAL                               0x00000000


#define IRTE_REG_ETHER_RT_TYPE3                                                                          0x0001900C
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_ETHER_RT_TYPE3__MSK_RT_TYPEID                                                               0x0000FFFF
#define IRTE_REG_ETHER_RT_TYPE3_SHFT_RT_TYPEID                                                               0
// ACCESS__RT_TYPEID: ,
// DESCR__RT_TYPEID: Typ-Kennung eines RT-Telegramms
#define IRTE_REG_ETHER_RT_TYPE3_RST__VAL                               0x00000000


#define IRTE_REG_IRT_CURRENT_CYCLE_ID                                                                    0x00019010
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_IRT_CURRENT_CYCLE_ID__MSK_KOMPARATOR_MASKE                                                  0x00003E00
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_SHFT_KOMPARATOR_MASKE                                                  9
// ACCESS__KOMPARATOR_MASKE: r,w
// DESCR__KOMPARATOR_MASKE: Maskierung der Interrupt-Signale, die durch die Komparatoren in der Zyklussynchronisation innerhalb eines Applikationszyklusses erzeugt werden.
#define IRTE_REG_IRT_CURRENT_CYCLE_ID__MSK_NOT_USABLE                                                        0x00000100
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_SHFT_NOT_USABLE                                                        8
// ACCESS__NOT_USABLE: r,w
// DESCR__NOT_USABLE: muss fest auf ‚1‘ gesetzt werden
#define IRTE_REG_IRT_CURRENT_CYCLE_ID__MSK_START_APPLIKATION                                                 0x00000080
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_SHFT_START_APPLIKATION                                                 7
// ACCESS__START_APPLIKATION: r,w
// DESCR__START_APPLIKATION: Die Anwendung teilt, den Beginn ihres Applikationszyklusses mit
#define IRTE_REG_IRT_CURRENT_CYCLE_ID__MSK_END_APPLIKATION                                                   0x00000040
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_SHFT_END_APPLIKATION                                                   6
// ACCESS__END_APPLIKATION: r,w
// DESCR__END_APPLIKATION: Die Anwendung teilt, das Ende ihres Applikationszyklusses mit
#define IRTE_REG_IRT_CURRENT_CYCLE_ID__MSK_IRT_CURRENTCYCLEID                                                0x0000003F
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_SHFT_IRT_CURRENTCYCLEID                                                0
// ACCESS__IRT_CURRENTCYCLEID: r,w
// DESCR__IRT_CURRENTCYCLEID: wird mit der CycleID eines EOC der IRT-Empfangs- und Sende-liste verglichen; bei Übereinstimmung erfolgt Umschaltung der Empfangs- und Sendeeinheit von IRT nach NRT.
#define IRTE_REG_IRT_CURRENT_CYCLE_ID_RST__VAL                         0x00000000


#define IRTE_REG_VERSION_NUMBER                                                                          0x00019400
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_VERSION_NUMBER__MSK_LOCATION                                                                0xF0000000
#define IRTE_REG_VERSION_NUMBER_SHFT_LOCATION                                                                28
// ACCESS__LOCATION: r,
// DESCR__LOCATION: 0xh: FPGA XC2V60001xh: FPGA XC2V80002xh: ERTEC400 (SER10)
#define IRTE_REG_VERSION_NUMBER__MSK_VARIANTE                                                                0x0F000000
#define IRTE_REG_VERSION_NUMBER_SHFT_VARIANTE                                                                24
// ACCESS__VARIANTE: r,
// DESCR__VARIANTE: x0h: 4-Port IRT/NRT/SRT (Vollausbau)x1h: 3-Port IRT (ohne gepufferten API)x2h: 2-Port NRTx3h: 3-Port NRT/SRTx4h: 2-Port NRT/SRTx5h: 1-Port IRT/NRT/SRTx6h: 2-Port IRT/NRT/SRT mit 100MBd / RMIIx7h: 4-Port IRTx8h: 2-Port IRTx9h: 3-Port IRT (mit gepufferten API)xAh: 2-Port IRT/NRT/SRT mit 10MBd  / RMIIxBh: 2-Port IRT/NRT/SRT mit MII
#define IRTE_REG_VERSION_NUMBER__MSK_MACRO_REVISION                                                          0x00FF0000
#define IRTE_REG_VERSION_NUMBER_SHFT_MACRO_REVISION                                                          16
// ACCESS__MACRO_REVISION: r,
// DESCR__MACRO_REVISION: 01h: Revision 102h: Revision 203h: Revision 3.104h: Revision 3.205h: Revision 3.3
#define IRTE_REG_VERSION_NUMBER__MSK_VERSION                                                                 0x0000FF00
#define IRTE_REG_VERSION_NUMBER_SHFT_VERSION                                                                 8
// ACCESS__VERSION: r,
// DESCR__VERSION: Version der Revision (für Fehlerkorrekturen)
#define IRTE_REG_VERSION_NUMBER__MSK_DEBUG_VERSION                                                           0x000000FF
#define IRTE_REG_VERSION_NUMBER_SHFT_DEBUG_VERSION                                                           0
// ACCESS__DEBUG_VERSION: r,
// DESCR__DEBUG_VERSION: Version der Debug-Unterstützung
#define IRTE_REG_VERSION_NUMBER_RST__VAL                               0xFFFFFFFFnone


#define IRTE_REG_SWITCH_STATUS                                                                           0x00019404
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_SWITCH_STATUS__MSK_PORT3_STOPPED                                                            0x00008000
#define IRTE_REG_SWITCH_STATUS_SHFT_PORT3_STOPPED                                                            15
// ACCESS__PORT3_STOPPED: r,
// DESCR__PORT3_STOPPED: Port 3 arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_PORT2_STOPPED                                                            0x00004000
#define IRTE_REG_SWITCH_STATUS_SHFT_PORT2_STOPPED                                                            14
// ACCESS__PORT2_STOPPED: r,
// DESCR__PORT2_STOPPED: Port 2 arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_PORT1_STOPPED                                                            0x00002000
#define IRTE_REG_SWITCH_STATUS_SHFT_PORT1_STOPPED                                                            13
// ACCESS__PORT1_STOPPED: r,
// DESCR__PORT1_STOPPED: Port 1 arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_PORT0_STOPPED                                                            0x00001000
#define IRTE_REG_SWITCH_STATUS_SHFT_PORT0_STOPPED                                                            12
// ACCESS__PORT0_STOPPED: r,
// DESCR__PORT0_STOPPED: Port 0 arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_CYCLESYNC_STOPPED                                                        0x00000800
#define IRTE_REG_SWITCH_STATUS_SHFT_CYCLESYNC_STOPPED                                                        11
// ACCESS__CYCLESYNC_STOPPED: r,
// DESCR__CYCLESYNC_STOPPED: Die Zyklussynchronisation arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_INTRCTRL_STOPPED                                                         0x00000400
#define IRTE_REG_SWITCH_STATUS_SHFT_INTRCTRL_STOPPED                                                         10
// ACCESS__INTRCTRL_STOPPED: r,
// DESCR__INTRCTRL_STOPPED: Der Interrupt-Controller arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_TRIGGERUNIT_STOPPED                                                      0x00000200
#define IRTE_REG_SWITCH_STATUS_SHFT_TRIGGERUNIT_STOPPED                                                      9
// ACCESS__TRIGGERUNIT_STOPPED: r,
// DESCR__TRIGGERUNIT_STOPPED: Die Trigger-Unit arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_NRTCTRL_STOPPED                                                          0x00000100
#define IRTE_REG_SWITCH_STATUS_SHFT_NRTCTRL_STOPPED                                                          8
// ACCESS__NRTCTRL_STOPPED: r,
// DESCR__NRTCTRL_STOPPED: Die NRT-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_IRTCTRL_STOPPED                                                          0x00000080
#define IRTE_REG_SWITCH_STATUS_SHFT_IRTCTRL_STOPPED                                                          7
// ACCESS__IRTCTRL_STOPPED: r,
// DESCR__IRTCTRL_STOPPED: Die IRT-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_SRTCTRL_STOPPED                                                          0x00000040
#define IRTE_REG_SWITCH_STATUS_SHFT_SRTCTRL_STOPPED                                                          6
// ACCESS__SRTCTRL_STOPPED: r,
// DESCR__SRTCTRL_STOPPED: Die SRT-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_KRAMCTRL_STOPPED                                                         0x00000020
#define IRTE_REG_SWITCH_STATUS_SHFT_KRAMCTRL_STOPPED                                                         5
// ACCESS__KRAMCTRL_STOPPED: r,
// DESCR__KRAMCTRL_STOPPED: Die K-RAM-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_TRCCTRL_STOPPED                                                          0x00000010
#define IRTE_REG_SWITCH_STATUS_SHFT_TRCCTRL_STOPPED                                                          4
// ACCESS__TRCCTRL_STOPPED: r,
// DESCR__TRCCTRL_STOPPED: Die IRT-Trace-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_STATCTRL_STOPPED                                                         0x00000008
#define IRTE_REG_SWITCH_STATUS_SHFT_STATCTRL_STOPPED                                                         3
// ACCESS__STATCTRL_STOPPED: r,
// DESCR__STATCTRL_STOPPED: Die Statistik-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_SMICTRL_STOPPED                                                          0x00000004
#define IRTE_REG_SWITCH_STATUS_SHFT_SMICTRL_STOPPED                                                          2
// ACCESS__SMICTRL_STOPPED: r,
// DESCR__SMICTRL_STOPPED: Die PHY-/SMI-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_KONSCTRL_STOPPED                                                         0x00000002
#define IRTE_REG_SWITCH_STATUS_SHFT_KONSCTRL_STOPPED                                                         1
// ACCESS__KONSCTRL_STOPPED: r,
// DESCR__KONSCTRL_STOPPED: Die Konsistenz-Control arbeitet nicht mehr
#define IRTE_REG_SWITCH_STATUS__MSK_SWITCH_STOPPED                                                           0x00000001
#define IRTE_REG_SWITCH_STATUS_SHFT_SWITCH_STOPPED                                                           0
// ACCESS__SWITCH_STOPPED: r,
// DESCR__SWITCH_STOPPED: Der gesamte Ethernet-Switch ist angehalten
#define IRTE_REG_SWITCH_STATUS_RST__VAL                                0x00000000


#define IRTE_REG_ACTIVITY_STATUS                                                                         0x00019408
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_ACTIVITY_STATUS__MSK_PORT3_ACTIV                                                            0x00000008
#define IRTE_REG_ACTIVITY_STATUS_SHFT_PORT3_ACTIV                                                            3
// ACCESS__PORT3_ACTIV: r,
// DESCR__PORT3_ACTIV: dto.
#define IRTE_REG_ACTIVITY_STATUS__MSK_PORT2_ACTIV                                                            0x00000004
#define IRTE_REG_ACTIVITY_STATUS_SHFT_PORT2_ACTIV                                                            2
// ACCESS__PORT2_ACTIV: r,
// DESCR__PORT2_ACTIV: dto.
#define IRTE_REG_ACTIVITY_STATUS__MSK_PORT1_ACTIV                                                            0x00000002
#define IRTE_REG_ACTIVITY_STATUS_SHFT_PORT1_ACTIV                                                            1
// ACCESS__PORT1_ACTIV: r,
// DESCR__PORT1_ACTIV: dto.
#define IRTE_REG_ACTIVITY_STATUS__MSK_PORT0_ACTIV                                                            0x00000001
#define IRTE_REG_ACTIVITY_STATUS_SHFT_PORT0_ACTIV                                                            0
// ACCESS__PORT0_ACTIV: r,
// DESCR__PORT0_ACTIV: Auslesen des Registerinhaltes mit gleichzeitigem Rücksetzen‚0‘: keine Aktivität an Port 0‚1‘: An Port 0 herrscht auf Sende- oder Empfangseite Aktivität
#define IRTE_REG_ACTIVITY_STATUS_RST__VAL                              0x00000000


#define IRTE_REG_TRIGGER_LOWER_DATA_MASK                                                                 0x0001A008
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRIGGER_LOWER_DATA_MASK__MSK_LOWER_DATA_MASK                                                0xFFFFFFFF
#define IRTE_REG_TRIGGER_LOWER_DATA_MASK_SHFT_LOWER_DATA_MASK                                                0
// ACCESS__LOWER_DATA_MASK: r,w
// DESCR__LOWER_DATA_MASK: Maske, mit der Busdaten und Vergleichswert und-verknüpft werden
#define IRTE_REG_TRIGGER_LOWER_DATA_MASK_RST__VAL                      0x00000000


#define IRTE_REG_TRIGGER_LOWER_DATA                                                                      0x0001A404
// ACCESS__: r,w
// DESCR__: 
#define IRTE_REG_TRIGGER_LOWER_DATA__MSK_TRIGGER_LOWER_DATA                                                  0xFFFFFFFF
#define IRTE_REG_TRIGGER_LOWER_DATA_SHFT_TRIGGER_LOWER_DATA                                                  0
// ACCESS__TRIGGER_LOWER_DATA: r,w
// DESCR__TRIGGER_LOWER_DATA: Enthält Daten des den Trigger auslösenden Buszugriffs bzw. als Triggerbedingung den Vergleichswert (nur Write-Daten)
#define IRTE_REG_TRIGGER_LOWER_DATA_RST__VAL                           0x00000000


#define IRTE_REG_GOOD_BYTES_RECEIVED_P0                                                                  0x0001B000
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P0__MSK_GOOD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_GOOD_BYTES_RECEIVED_P0_SHFT_GOOD_BYTES_RECEIVED                                             0
// ACCESS__GOOD_BYTES_RECEIVED: r,
// DESCR__GOOD_BYTES_RECEIVED: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P0_RST__VAL                       0x00000000


#define IRTE_REG_TOTAL_BYTES_RECEIVED_P0                                                                 0x0001B004
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P0__MSK_BAD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P0_SHFT_BAD_BYTES_RECEIVED                                             0
// ACCESS__BAD_BYTES_RECEIVED: r,
// DESCR__BAD_BYTES_RECEIVED: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P0_RST__VAL                      0x00000000


#define IRTE_REG_BYTES_SENT_P0                                                                           0x0001B008
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BYTES_SENT_P0__MSK_BYTES_SENT                                                               0xFFFFFFFF
#define IRTE_REG_BYTES_SENT_P0_SHFT_BYTES_SENT                                                               0
// ACCESS__BYTES_SENT: r,
// DESCR__BYTES_SENT: 
#define IRTE_REG_BYTES_SENT_P0_RST__VAL                                0x00000000


#define IRTE_REG_FRAMES_SENT_P0                                                                          0x0001B00C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_SENT_P0__MSK_FRAMES_SENT                                                             0xFFFFFFFF
#define IRTE_REG_FRAMES_SENT_P0_SHFT_FRAMES_SENT                                                             0
// ACCESS__FRAMES_SENT: r,
// DESCR__FRAMES_SENT: 
#define IRTE_REG_FRAMES_SENT_P0_RST__VAL                               0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P0                                                              0x0001B010
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P0__MSK_UNICAST_FRAMES_RECEIVED                                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P0_SHFT_UNICAST_FRAMES_RECEIVED                                     0
// ACCESS__UNICAST_FRAMES_RECEIVED: r,
// DESCR__UNICAST_FRAMES_RECEIVED: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P0_RST__VAL                   0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P0                                                      0x0001B014
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P0__MSK_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P0_SHFT_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0
// ACCESS__UNICAST_FRAMES_RECEIVED_UNKNOWN: r,
// DESCR__UNICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P0_RST__VAL           0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P0                                                            0x0001B018
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P0__MSK_MULTICAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P0_SHFT_MULTICAST_FRAMES_RECEIVED                                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED: r,
// DESCR__MULTICAST_FRAMES_RECEIVED: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P0_RST__VAL                 0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P0                                                    0x0001B01C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P0__MSK_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P0_SHFT_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED_UNKNOWN: r,
// DESCR__MULTICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P0_RST__VAL         0x00000000


#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P0                                                            0x0001B020
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P0__MSK_BROADCAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P0_SHFT_BROADCAST_FRAMES_RECEIVED                                 0
// ACCESS__BROADCAST_FRAMES_RECEIVED: r,
// DESCR__BROADCAST_FRAMES_RECEIVED: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P0_RST__VAL                 0x00000000


#define IRTE_REG_UNICAST_FRAMES_SENT_P0                                                                  0x0001B024
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P0__MSK_UNICAST_FRAMES_SENT                                             0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_SENT_P0_SHFT_UNICAST_FRAMES_SENT                                             0
// ACCESS__UNICAST_FRAMES_SENT: r,
// DESCR__UNICAST_FRAMES_SENT: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P0_RST__VAL                       0x00000000


#define IRTE_REG_MULTICAST_FRAMES_SENT_P0                                                                0x0001B028
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P0__MSK_MULTICAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_SENT_P0_SHFT_MULTICAST_FRAMES_SENT                                         0
// ACCESS__MULTICAST_FRAMES_SENT: r,
// DESCR__MULTICAST_FRAMES_SENT: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P0_RST__VAL                     0x00000000


#define IRTE_REG_BROADCAST_FRAMES_SENT_P0                                                                0x0001B02C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P0__MSK_BROADCAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_SENT_P0_SHFT_BROADCAST_FRAMES_SENT                                         0
// ACCESS__BROADCAST_FRAMES_SENT: r,
// DESCR__BROADCAST_FRAMES_SENT: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P0_RST__VAL                     0x00000000


#define IRTE_REG_FRAMES_UNDER_64BYTES_P0                                                                 0x0001B030
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P0__MSK_FRAMES_UNDER_64BYTES                                           0xFFFFFFFF
#define IRTE_REG_FRAMES_UNDER_64BYTES_P0_SHFT_FRAMES_UNDER_64BYTES                                           0
// ACCESS__FRAMES_UNDER_64BYTES: r,
// DESCR__FRAMES_UNDER_64BYTES: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P0_RST__VAL                      0x00000000


#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P0                                                        0x0001B034
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P0__MSK_FRAMES_WITH_LENGTH_OF_64BYTES                         0xFFFFFFFF
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P0_SHFT_FRAMES_WITH_LENGTH_OF_64BYTES                         0
// ACCESS__FRAMES_WITH_LENGTH_OF_64BYTES: r,
// DESCR__FRAMES_WITH_LENGTH_OF_64BYTES: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P0_RST__VAL             0x00000000


#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P0                                                           0x0001B038
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P0__MSK_FRAMES_BETWEEN_65_127BYTES                               0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P0_SHFT_FRAMES_BETWEEN_65_127BYTES                               0
// ACCESS__FRAMES_BETWEEN_65_127BYTES: r,
// DESCR__FRAMES_BETWEEN_65_127BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P0_RST__VAL                0x00000000


#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P0                                                          0x0001B03C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P0__MSK_FRAMES_BETWEEN_128_255BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P0_SHFT_FRAMES_BETWEEN_128_255BYTES                             0
// ACCESS__FRAMES_BETWEEN_128_255BYTES: r,
// DESCR__FRAMES_BETWEEN_128_255BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P0_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P0                                                          0x0001B040
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P0__MSK_FRAMES_BETWEEN_256_511BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P0_SHFT_FRAMES_BETWEEN_256_511BYTES                             0
// ACCESS__FRAMES_BETWEEN_256_511BYTES: r,
// DESCR__FRAMES_BETWEEN_256_511BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P0_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P0                                                         0x0001B044
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P0__MSK_FRAMES_BETWEEN_512_1023BYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P0_SHFT_FRAMES_BETWEEN_512_1023BYTES                           0
// ACCESS__FRAMES_BETWEEN_512_1023BYTES: r,
// DESCR__FRAMES_BETWEEN_512_1023BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P0_RST__VAL              0x00000000


#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P0                                                         0x0001B048
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P0__MSK_FRAMES_BETWEEN_1024_MAXBYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P0_SHFT_FRAMES_BETWEEN_1024_MAXBYTES                           0
// ACCESS__FRAMES_BETWEEN_1024_MAXBYTES: r,
// DESCR__FRAMES_BETWEEN_1024_MAXBYTES: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P0_RST__VAL              0x00000000


#define IRTE_REG_GOOD_FRAMES_RECEIVED_P0                                                                 0x0001B04C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P0__MSK_GOOD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P0_SHFT_GOOD_FRAMES_RECEIVED                                           0
// ACCESS__GOOD_FRAMES_RECEIVED: r,
// DESCR__GOOD_FRAMES_RECEIVED: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P0_RST__VAL                      0x00000000


#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P0                                                                0x0001B050
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P0__MSK_BAD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P0_SHFT_BAD_FRAMES_RECEIVED                                           0
// ACCESS__BAD_FRAMES_RECEIVED: r,
// DESCR__BAD_FRAMES_RECEIVED: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P0_RST__VAL                     0x00000000


#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P0                                            0x0001B054
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P0__MSK_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0xFFFFFFFF
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P0_SHFT_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0
// ACCESS__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: r,
// DESCR__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P0_RST__VAL 0x00000000


#define IRTE_REG_RX_ERRORS_P0                                                                            0x0001B058
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RX_ERRORS_P0__MSK_RX_ERRORS                                                                 0xFFFFFFFF
#define IRTE_REG_RX_ERRORS_P0_SHFT_RX_ERRORS                                                                 0
// ACCESS__RX_ERRORS: r,
// DESCR__RX_ERRORS: 
#define IRTE_REG_RX_ERRORS_P0_RST__VAL                                 0x00000000


#define IRTE_REG_OVERSIZE_FRAMES_P0                                                                      0x0001B05C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_OVERSIZE_FRAMES_P0__MSK_OVERSIZE_FRAMES                                                     0xFFFFFFFF
#define IRTE_REG_OVERSIZE_FRAMES_P0_SHFT_OVERSIZE_FRAMES                                                     0
// ACCESS__OVERSIZE_FRAMES: r,
// DESCR__OVERSIZE_FRAMES: 
#define IRTE_REG_OVERSIZE_FRAMES_P0_RST__VAL                           0x00000000


#define IRTE_REG_CRC_ALIGNMENT_ERROR_P0                                                                  0x0001B060
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P0__MSK_CRC_ALIGNMENT_ERROR                                             0xFFFFFFFF
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P0_SHFT_CRC_ALIGNMENT_ERROR                                             0
// ACCESS__CRC_ALIGNMENT_ERROR: r,
// DESCR__CRC_ALIGNMENT_ERROR: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P0_RST__VAL                       0x00000000


#define IRTE_REG_FRAMES_DROPPED_P0                                                                       0x0001B064
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_DROPPED_P0__MSK_FRAMES_DROPPED                                                       0xFFFFFFFF
#define IRTE_REG_FRAMES_DROPPED_P0_SHFT_FRAMES_DROPPED                                                       0
// ACCESS__FRAMES_DROPPED: r,
// DESCR__FRAMES_DROPPED: 
#define IRTE_REG_FRAMES_DROPPED_P0_RST__VAL                            0x00000000


#define IRTE_REG_COLLISION_P0                                                                            0x0001B068
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_COLLISION_P0__MSK_COLLISION                                                                 0xFFFFFFFF
#define IRTE_REG_COLLISION_P0_SHFT_COLLISION                                                                 0
// ACCESS__COLLISION: r,
// DESCR__COLLISION: 
#define IRTE_REG_COLLISION_P0_RST__VAL                                 0x00000000


#define IRTE_REG_HOL_BOUNDARY_CROSSED_P0                                                                 0x0001B06C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P0__MSK_HOL_BOUNDARY_CROSSED                                           0xFFFFFFFF
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P0_SHFT_HOL_BOUNDARY_CROSSED                                           0
// ACCESS__HOL_BOUNDARY_CROSSED: r,
// DESCR__HOL_BOUNDARY_CROSSED: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P0_RST__VAL                      0x00000000


#define IRTE_REG_GOOD_BYTES_RECEIVED_P1                                                                  0x0001B100
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P1__MSK_GOOD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_GOOD_BYTES_RECEIVED_P1_SHFT_GOOD_BYTES_RECEIVED                                             0
// ACCESS__GOOD_BYTES_RECEIVED: r,
// DESCR__GOOD_BYTES_RECEIVED: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P1_RST__VAL                       0x00000000


#define IRTE_REG_TOTAL_BYTES_RECEIVED_P1                                                                 0x0001B104
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P1__MSK_BAD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P1_SHFT_BAD_BYTES_RECEIVED                                             0
// ACCESS__BAD_BYTES_RECEIVED: r,
// DESCR__BAD_BYTES_RECEIVED: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P1_RST__VAL                      0x00000000


#define IRTE_REG_BYTES_SENT_P1                                                                           0x0001B108
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BYTES_SENT_P1__MSK_BYTES_SENT                                                               0xFFFFFFFF
#define IRTE_REG_BYTES_SENT_P1_SHFT_BYTES_SENT                                                               0
// ACCESS__BYTES_SENT: r,
// DESCR__BYTES_SENT: 
#define IRTE_REG_BYTES_SENT_P1_RST__VAL                                0x00000000


#define IRTE_REG_FRAMES_SENT_P1                                                                          0x0001B10C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_SENT_P1__MSK_FRAMES_SENT                                                             0xFFFFFFFF
#define IRTE_REG_FRAMES_SENT_P1_SHFT_FRAMES_SENT                                                             0
// ACCESS__FRAMES_SENT: r,
// DESCR__FRAMES_SENT: 
#define IRTE_REG_FRAMES_SENT_P1_RST__VAL                               0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P1                                                              0x0001B110
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P1__MSK_UNICAST_FRAMES_RECEIVED                                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P1_SHFT_UNICAST_FRAMES_RECEIVED                                     0
// ACCESS__UNICAST_FRAMES_RECEIVED: r,
// DESCR__UNICAST_FRAMES_RECEIVED: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P1_RST__VAL                   0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P1                                                      0x0001B114
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P1__MSK_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P1_SHFT_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0
// ACCESS__UNICAST_FRAMES_RECEIVED_UNKNOWN: r,
// DESCR__UNICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P1_RST__VAL           0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P1                                                            0x0001B118
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P1__MSK_MULTICAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P1_SHFT_MULTICAST_FRAMES_RECEIVED                                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED: r,
// DESCR__MULTICAST_FRAMES_RECEIVED: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P1_RST__VAL                 0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P1                                                    0x0001B11C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P1__MSK_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P1_SHFT_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED_UNKNOWN: r,
// DESCR__MULTICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P1_RST__VAL         0x00000000


#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P1                                                            0x0001B120
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P1__MSK_BROADCAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P1_SHFT_BROADCAST_FRAMES_RECEIVED                                 0
// ACCESS__BROADCAST_FRAMES_RECEIVED: r,
// DESCR__BROADCAST_FRAMES_RECEIVED: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P1_RST__VAL                 0x00000000


#define IRTE_REG_UNICAST_FRAMES_SENT_P1                                                                  0x0001B124
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P1__MSK_UNICAST_FRAMES_SENT                                             0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_SENT_P1_SHFT_UNICAST_FRAMES_SENT                                             0
// ACCESS__UNICAST_FRAMES_SENT: r,
// DESCR__UNICAST_FRAMES_SENT: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P1_RST__VAL                       0x00000000


#define IRTE_REG_MULTICAST_FRAMES_SENT_P1                                                                0x0001B128
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P1__MSK_MULTICAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_SENT_P1_SHFT_MULTICAST_FRAMES_SENT                                         0
// ACCESS__MULTICAST_FRAMES_SENT: r,
// DESCR__MULTICAST_FRAMES_SENT: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P1_RST__VAL                     0x00000000


#define IRTE_REG_BROADCAST_FRAMES_SENT_P1                                                                0x0001B12C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P1__MSK_BROADCAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_SENT_P1_SHFT_BROADCAST_FRAMES_SENT                                         0
// ACCESS__BROADCAST_FRAMES_SENT: r,
// DESCR__BROADCAST_FRAMES_SENT: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P1_RST__VAL                     0x00000000


#define IRTE_REG_FRAMES_UNDER_64BYTES_P1                                                                 0x0001B130
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P1__MSK_FRAMES_UNDER_64BYTES                                           0xFFFFFFFF
#define IRTE_REG_FRAMES_UNDER_64BYTES_P1_SHFT_FRAMES_UNDER_64BYTES                                           0
// ACCESS__FRAMES_UNDER_64BYTES: r,
// DESCR__FRAMES_UNDER_64BYTES: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P1_RST__VAL                      0x00000000


#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P1                                                        0x0001B134
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P1__MSK_FRAMES_WITH_LENGTH_OF_64BYTES                         0xFFFFFFFF
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P1_SHFT_FRAMES_WITH_LENGTH_OF_64BYTES                         0
// ACCESS__FRAMES_WITH_LENGTH_OF_64BYTES: r,
// DESCR__FRAMES_WITH_LENGTH_OF_64BYTES: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P1_RST__VAL             0x00000000


#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P1                                                           0x0001B138
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P1__MSK_FRAMES_BETWEEN_65_127BYTES                               0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P1_SHFT_FRAMES_BETWEEN_65_127BYTES                               0
// ACCESS__FRAMES_BETWEEN_65_127BYTES: r,
// DESCR__FRAMES_BETWEEN_65_127BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P1_RST__VAL                0x00000000


#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P1                                                          0x0001B13C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P1__MSK_FRAMES_BETWEEN_128_255BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P1_SHFT_FRAMES_BETWEEN_128_255BYTES                             0
// ACCESS__FRAMES_BETWEEN_128_255BYTES: r,
// DESCR__FRAMES_BETWEEN_128_255BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P1_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P1                                                          0x0001B140
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P1__MSK_FRAMES_BETWEEN_256_511BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P1_SHFT_FRAMES_BETWEEN_256_511BYTES                             0
// ACCESS__FRAMES_BETWEEN_256_511BYTES: r,
// DESCR__FRAMES_BETWEEN_256_511BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P1_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P1                                                         0x0001B144
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P1__MSK_FRAMES_BETWEEN_512_1023BYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P1_SHFT_FRAMES_BETWEEN_512_1023BYTES                           0
// ACCESS__FRAMES_BETWEEN_512_1023BYTES: r,
// DESCR__FRAMES_BETWEEN_512_1023BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P1_RST__VAL              0x00000000


#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P1                                                         0x0001B148
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P1__MSK_FRAMES_BETWEEN_1024_MAXBYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P1_SHFT_FRAMES_BETWEEN_1024_MAXBYTES                           0
// ACCESS__FRAMES_BETWEEN_1024_MAXBYTES: r,
// DESCR__FRAMES_BETWEEN_1024_MAXBYTES: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P1_RST__VAL              0x00000000


#define IRTE_REG_GOOD_FRAMES_RECEIVED_P1                                                                 0x0001B14C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P1__MSK_GOOD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P1_SHFT_GOOD_FRAMES_RECEIVED                                           0
// ACCESS__GOOD_FRAMES_RECEIVED: r,
// DESCR__GOOD_FRAMES_RECEIVED: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P1_RST__VAL                      0x00000000


#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P1                                                                0x0001B150
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P1__MSK_BAD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P1_SHFT_BAD_FRAMES_RECEIVED                                           0
// ACCESS__BAD_FRAMES_RECEIVED: r,
// DESCR__BAD_FRAMES_RECEIVED: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P1_RST__VAL                     0x00000000


#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P1                                            0x0001B154
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P1__MSK_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0xFFFFFFFF
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P1_SHFT_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0
// ACCESS__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: r,
// DESCR__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P1_RST__VAL 0x00000000


#define IRTE_REG_RX_ERRORS_P1                                                                            0x0001B158
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RX_ERRORS_P1__MSK_RX_ERRORS                                                                 0xFFFFFFFF
#define IRTE_REG_RX_ERRORS_P1_SHFT_RX_ERRORS                                                                 0
// ACCESS__RX_ERRORS: r,
// DESCR__RX_ERRORS: 
#define IRTE_REG_RX_ERRORS_P1_RST__VAL                                 0x00000000


#define IRTE_REG_OVERSIZE_FRAMES_P1                                                                      0x0001B15C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_OVERSIZE_FRAMES_P1__MSK_OVERSIZE_FRAMES                                                     0xFFFFFFFF
#define IRTE_REG_OVERSIZE_FRAMES_P1_SHFT_OVERSIZE_FRAMES                                                     0
// ACCESS__OVERSIZE_FRAMES: r,
// DESCR__OVERSIZE_FRAMES: 
#define IRTE_REG_OVERSIZE_FRAMES_P1_RST__VAL                           0x00000000


#define IRTE_REG_CRC_ALIGNMENT_ERROR_P1                                                                  0x0001B160
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P1__MSK_CRC_ALIGNMENT_ERROR                                             0xFFFFFFFF
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P1_SHFT_CRC_ALIGNMENT_ERROR                                             0
// ACCESS__CRC_ALIGNMENT_ERROR: r,
// DESCR__CRC_ALIGNMENT_ERROR: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P1_RST__VAL                       0x00000000


#define IRTE_REG_FRAMES_DROPPED_P1                                                                       0x0001B164
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_DROPPED_P1__MSK_FRAMES_DROPPED                                                       0xFFFFFFFF
#define IRTE_REG_FRAMES_DROPPED_P1_SHFT_FRAMES_DROPPED                                                       0
// ACCESS__FRAMES_DROPPED: r,
// DESCR__FRAMES_DROPPED: 
#define IRTE_REG_FRAMES_DROPPED_P1_RST__VAL                            0x00000000


#define IRTE_REG_COLLISION_P1                                                                            0x0001B168
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_COLLISION_P1__MSK_COLLISION                                                                 0xFFFFFFFF
#define IRTE_REG_COLLISION_P1_SHFT_COLLISION                                                                 0
// ACCESS__COLLISION: r,
// DESCR__COLLISION: 
#define IRTE_REG_COLLISION_P1_RST__VAL                                 0x00000000


#define IRTE_REG_HOL_BOUNDARY_CROSSED_P1                                                                 0x0001B16C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P1__MSK_HOL_BOUNDARY_CROSSED                                           0xFFFFFFFF
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P1_SHFT_HOL_BOUNDARY_CROSSED                                           0
// ACCESS__HOL_BOUNDARY_CROSSED: r,
// DESCR__HOL_BOUNDARY_CROSSED: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P1_RST__VAL                      0x00000000


#define IRTE_REG_GOOD_BYTES_RECEIVED_P2                                                                  0x0001B200
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P2__MSK_GOOD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_GOOD_BYTES_RECEIVED_P2_SHFT_GOOD_BYTES_RECEIVED                                             0
// ACCESS__GOOD_BYTES_RECEIVED: r,
// DESCR__GOOD_BYTES_RECEIVED: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P2_RST__VAL                       0x00000000


#define IRTE_REG_TOTAL_BYTES_RECEIVED_P2                                                                 0x0001B204
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P2__MSK_BAD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P2_SHFT_BAD_BYTES_RECEIVED                                             0
// ACCESS__BAD_BYTES_RECEIVED: r,
// DESCR__BAD_BYTES_RECEIVED: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P2_RST__VAL                      0x00000000


#define IRTE_REG_BYTES_SENT_P2                                                                           0x0001B208
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BYTES_SENT_P2__MSK_BYTES_SENT                                                               0xFFFFFFFF
#define IRTE_REG_BYTES_SENT_P2_SHFT_BYTES_SENT                                                               0
// ACCESS__BYTES_SENT: r,
// DESCR__BYTES_SENT: 
#define IRTE_REG_BYTES_SENT_P2_RST__VAL                                0x00000000


#define IRTE_REG_FRAMES_SENT_P2                                                                          0x0001B20C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_SENT_P2__MSK_FRAMES_SENT                                                             0xFFFFFFFF
#define IRTE_REG_FRAMES_SENT_P2_SHFT_FRAMES_SENT                                                             0
// ACCESS__FRAMES_SENT: r,
// DESCR__FRAMES_SENT: 
#define IRTE_REG_FRAMES_SENT_P2_RST__VAL                               0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P2                                                              0x0001B210
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P2__MSK_UNICAST_FRAMES_RECEIVED                                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P2_SHFT_UNICAST_FRAMES_RECEIVED                                     0
// ACCESS__UNICAST_FRAMES_RECEIVED: r,
// DESCR__UNICAST_FRAMES_RECEIVED: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P2_RST__VAL                   0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P2                                                      0x0001B214
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P2__MSK_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P2_SHFT_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0
// ACCESS__UNICAST_FRAMES_RECEIVED_UNKNOWN: ,
// DESCR__UNICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P2_RST__VAL           0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P2                                                            0x0001B218
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P2__MSK_MULTICAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P2_SHFT_MULTICAST_FRAMES_RECEIVED                                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED: r,
// DESCR__MULTICAST_FRAMES_RECEIVED: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P2_RST__VAL                 0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P2                                                    0x0001B21C
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P2__MSK_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P2_SHFT_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED_UNKNOWN: ,
// DESCR__MULTICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P2_RST__VAL         0x00000000


#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P2                                                            0x0001B220
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P2__MSK_BROADCAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P2_SHFT_BROADCAST_FRAMES_RECEIVED                                 0
// ACCESS__BROADCAST_FRAMES_RECEIVED: r,
// DESCR__BROADCAST_FRAMES_RECEIVED: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P2_RST__VAL                 0x00000000


#define IRTE_REG_UNICAST_FRAMES_SENT_P2                                                                  0x0001B224
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P2__MSK_UNICAST_FRAMES_SENT                                             0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_SENT_P2_SHFT_UNICAST_FRAMES_SENT                                             0
// ACCESS__UNICAST_FRAMES_SENT: r,
// DESCR__UNICAST_FRAMES_SENT: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P2_RST__VAL                       0x00000000


#define IRTE_REG_MULTICAST_FRAMES_SENT_P2                                                                0x0001B228
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P2__MSK_MULTICAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_SENT_P2_SHFT_MULTICAST_FRAMES_SENT                                         0
// ACCESS__MULTICAST_FRAMES_SENT: r,
// DESCR__MULTICAST_FRAMES_SENT: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P2_RST__VAL                     0x00000000


#define IRTE_REG_BROADCAST_FRAMES_SENT_P2                                                                0x0001B22C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P2__MSK_BROADCAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_SENT_P2_SHFT_BROADCAST_FRAMES_SENT                                         0
// ACCESS__BROADCAST_FRAMES_SENT: r,
// DESCR__BROADCAST_FRAMES_SENT: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P2_RST__VAL                     0x00000000


#define IRTE_REG_FRAMES_UNDER_64BYTES_P2                                                                 0x0001B230
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P2__MSK_FRAMES_UNDER_64BYTES                                           0xFFFFFFFF
#define IRTE_REG_FRAMES_UNDER_64BYTES_P2_SHFT_FRAMES_UNDER_64BYTES                                           0
// ACCESS__FRAMES_UNDER_64BYTES: r,
// DESCR__FRAMES_UNDER_64BYTES: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P2_RST__VAL                      0x00000000


#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P2                                                        0x0001B234
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P2__MSK_FRAMES_WITH_LENGTH_OF_64BYTES                         0xFFFFFFFF
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P2_SHFT_FRAMES_WITH_LENGTH_OF_64BYTES                         0
// ACCESS__FRAMES_WITH_LENGTH_OF_64BYTES: r,
// DESCR__FRAMES_WITH_LENGTH_OF_64BYTES: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P2_RST__VAL             0x00000000


#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P2                                                           0x0001B238
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P2__MSK_FRAMES_BETWEEN_65_127BYTES                               0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P2_SHFT_FRAMES_BETWEEN_65_127BYTES                               0
// ACCESS__FRAMES_BETWEEN_65_127BYTES: r,
// DESCR__FRAMES_BETWEEN_65_127BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P2_RST__VAL                0x00000000


#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P2                                                          0x0001B23C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P2__MSK_FRAMES_BETWEEN_128_255BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P2_SHFT_FRAMES_BETWEEN_128_255BYTES                             0
// ACCESS__FRAMES_BETWEEN_128_255BYTES: r,
// DESCR__FRAMES_BETWEEN_128_255BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P2_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P2                                                          0x0001B240
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P2__MSK_FRAMES_BETWEEN_256_511BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P2_SHFT_FRAMES_BETWEEN_256_511BYTES                             0
// ACCESS__FRAMES_BETWEEN_256_511BYTES: r,
// DESCR__FRAMES_BETWEEN_256_511BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P2_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P2                                                         0x0001B244
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P2__MSK_FRAMES_BETWEEN_512_1023BYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P2_SHFT_FRAMES_BETWEEN_512_1023BYTES                           0
// ACCESS__FRAMES_BETWEEN_512_1023BYTES: r,
// DESCR__FRAMES_BETWEEN_512_1023BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P2_RST__VAL              0x00000000


#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P2                                                         0x0001B248
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P2__MSK_FRAMES_BETWEEN_1024_MAXBYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P2_SHFT_FRAMES_BETWEEN_1024_MAXBYTES                           0
// ACCESS__FRAMES_BETWEEN_1024_MAXBYTES: r,
// DESCR__FRAMES_BETWEEN_1024_MAXBYTES: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P2_RST__VAL              0x00000000


#define IRTE_REG_GOOD_FRAMES_RECEIVED_P2                                                                 0x0001B24C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P2__MSK_GOOD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P2_SHFT_GOOD_FRAMES_RECEIVED                                           0
// ACCESS__GOOD_FRAMES_RECEIVED: r,
// DESCR__GOOD_FRAMES_RECEIVED: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P2_RST__VAL                      0x00000000


#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P2                                                                0x0001B250
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P2__MSK_BAD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P2_SHFT_BAD_FRAMES_RECEIVED                                           0
// ACCESS__BAD_FRAMES_RECEIVED: r,
// DESCR__BAD_FRAMES_RECEIVED: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P2_RST__VAL                     0x00000000


#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P2                                            0x0001B254
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P2__MSK_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0xFFFFFFFF
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P2_SHFT_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0
// ACCESS__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: ,
// DESCR__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P2_RST__VAL 0x00000000


#define IRTE_REG_RX_ERRORS_P2                                                                            0x0001B258
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RX_ERRORS_P2__MSK_RX_ERRORS                                                                 0xFFFFFFFF
#define IRTE_REG_RX_ERRORS_P2_SHFT_RX_ERRORS                                                                 0
// ACCESS__RX_ERRORS: r,
// DESCR__RX_ERRORS: 
#define IRTE_REG_RX_ERRORS_P2_RST__VAL                                 0x00000000


#define IRTE_REG_OVERSIZE_FRAMES_P2                                                                      0x0001B25C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_OVERSIZE_FRAMES_P2__MSK_OVERSIZE_FRAMES                                                     0xFFFFFFFF
#define IRTE_REG_OVERSIZE_FRAMES_P2_SHFT_OVERSIZE_FRAMES                                                     0
// ACCESS__OVERSIZE_FRAMES: r,
// DESCR__OVERSIZE_FRAMES: 
#define IRTE_REG_OVERSIZE_FRAMES_P2_RST__VAL                           0x00000000


#define IRTE_REG_CRC_ALIGNMENT_ERROR_P2                                                                  0x0001B260
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P2__MSK_CRC_ALIGNMENT_ERROR                                             0xFFFFFFFF
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P2_SHFT_CRC_ALIGNMENT_ERROR                                             0
// ACCESS__CRC_ALIGNMENT_ERROR: r,
// DESCR__CRC_ALIGNMENT_ERROR: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P2_RST__VAL                       0x00000000


#define IRTE_REG_FRAMES_DROPPED_P2                                                                       0x0001B264
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_DROPPED_P2__MSK_FRAMES_DROPPED                                                       0xFFFFFFFF
#define IRTE_REG_FRAMES_DROPPED_P2_SHFT_FRAMES_DROPPED                                                       0
// ACCESS__FRAMES_DROPPED: r,
// DESCR__FRAMES_DROPPED: 
#define IRTE_REG_FRAMES_DROPPED_P2_RST__VAL                            0x00000000


#define IRTE_REG_COLLISION_P2                                                                            0x0001B268
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_COLLISION_P2__MSK_COLLISION                                                                 0xFFFFFFFF
#define IRTE_REG_COLLISION_P2_SHFT_COLLISION                                                                 0
// ACCESS__COLLISION: r,
// DESCR__COLLISION: 
#define IRTE_REG_COLLISION_P2_RST__VAL                                 0x00000000


#define IRTE_REG_HOL_BOUNDARY_CROSSED_P2                                                                 0x0001B26C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P2__MSK_HOL_BOUNDARY_CROSSED                                           0xFFFFFFFF
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P2_SHFT_HOL_BOUNDARY_CROSSED                                           0
// ACCESS__HOL_BOUNDARY_CROSSED: r,
// DESCR__HOL_BOUNDARY_CROSSED: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P2_RST__VAL                      0x00000000


#define IRTE_REG_GOOD_BYTES_RECEIVED_P3                                                                  0x0001B300
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P3__MSK_GOOD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_GOOD_BYTES_RECEIVED_P3_SHFT_GOOD_BYTES_RECEIVED                                             0
// ACCESS__GOOD_BYTES_RECEIVED: r,
// DESCR__GOOD_BYTES_RECEIVED: 
#define IRTE_REG_GOOD_BYTES_RECEIVED_P3_RST__VAL                       0x00000000


#define IRTE_REG_TOTAL_BYTES_RECEIVED_P3                                                                 0x0001B304
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P3__MSK_BAD_BYTES_RECEIVED                                             0xFFFFFFFF
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P3_SHFT_BAD_BYTES_RECEIVED                                             0
// ACCESS__BAD_BYTES_RECEIVED: r,
// DESCR__BAD_BYTES_RECEIVED: 
#define IRTE_REG_TOTAL_BYTES_RECEIVED_P3_RST__VAL                      0x00000000


#define IRTE_REG_BYTES_SENT_P3                                                                           0x0001B308
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BYTES_SENT_P3__MSK_BYTES_SENT                                                               0xFFFFFFFF
#define IRTE_REG_BYTES_SENT_P3_SHFT_BYTES_SENT                                                               0
// ACCESS__BYTES_SENT: r,
// DESCR__BYTES_SENT: 
#define IRTE_REG_BYTES_SENT_P3_RST__VAL                                0x00000000


#define IRTE_REG_FRAMES_SENT_P3                                                                          0x0001B30C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_SENT_P3__MSK_FRAMES_SENT                                                             0xFFFFFFFF
#define IRTE_REG_FRAMES_SENT_P3_SHFT_FRAMES_SENT                                                             0
// ACCESS__FRAMES_SENT: r,
// DESCR__FRAMES_SENT: 
#define IRTE_REG_FRAMES_SENT_P3_RST__VAL                               0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P3                                                              0x0001B310
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P3__MSK_UNICAST_FRAMES_RECEIVED                                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P3_SHFT_UNICAST_FRAMES_RECEIVED                                     0
// ACCESS__UNICAST_FRAMES_RECEIVED: r,
// DESCR__UNICAST_FRAMES_RECEIVED: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_P3_RST__VAL                   0x00000000


#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P3                                                      0x0001B314
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P3__MSK_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P3_SHFT_UNICAST_FRAMES_RECEIVED_UNKNOWN                     0
// ACCESS__UNICAST_FRAMES_RECEIVED_UNKNOWN: ,
// DESCR__UNICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_UNICAST_FRAMES_RECEIVED_UNKNOWN_P3_RST__VAL           0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P3                                                            0x0001B318
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P3__MSK_MULTICAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P3_SHFT_MULTICAST_FRAMES_RECEIVED                                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED: r,
// DESCR__MULTICAST_FRAMES_RECEIVED: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_P3_RST__VAL                 0x00000000


#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P3                                                    0x0001B31C
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P3__MSK_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P3_SHFT_MULTICAST_FRAMES_RECEIVED_UNKNOWN                 0
// ACCESS__MULTICAST_FRAMES_RECEIVED_UNKNOWN: ,
// DESCR__MULTICAST_FRAMES_RECEIVED_UNKNOWN: 
#define IRTE_REG_MULTICAST_FRAMES_RECEIVED_UNKNOWN_P3_RST__VAL         0x00000000


#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P3                                                            0x0001B320
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P3__MSK_BROADCAST_FRAMES_RECEIVED                                 0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P3_SHFT_BROADCAST_FRAMES_RECEIVED                                 0
// ACCESS__BROADCAST_FRAMES_RECEIVED: r,
// DESCR__BROADCAST_FRAMES_RECEIVED: 
#define IRTE_REG_BROADCAST_FRAMES_RECEIVED_P3_RST__VAL                 0x00000000


#define IRTE_REG_UNICAST_FRAMES_SENT_P3                                                                  0x0001B324
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P3__MSK_UNICAST_FRAMES_SENT                                             0xFFFFFFFF
#define IRTE_REG_UNICAST_FRAMES_SENT_P3_SHFT_UNICAST_FRAMES_SENT                                             0
// ACCESS__UNICAST_FRAMES_SENT: r,
// DESCR__UNICAST_FRAMES_SENT: 
#define IRTE_REG_UNICAST_FRAMES_SENT_P3_RST__VAL                       0x00000000


#define IRTE_REG_MULTICAST_FRAMES_SENT_P3                                                                0x0001B328
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P3__MSK_MULTICAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_MULTICAST_FRAMES_SENT_P3_SHFT_MULTICAST_FRAMES_SENT                                         0
// ACCESS__MULTICAST_FRAMES_SENT: r,
// DESCR__MULTICAST_FRAMES_SENT: 
#define IRTE_REG_MULTICAST_FRAMES_SENT_P3_RST__VAL                     0x00000000


#define IRTE_REG_BROADCAST_FRAMES_SENT_P3                                                                0x0001B32C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P3__MSK_BROADCAST_FRAMES_SENT                                         0xFFFFFFFF
#define IRTE_REG_BROADCAST_FRAMES_SENT_P3_SHFT_BROADCAST_FRAMES_SENT                                         0
// ACCESS__BROADCAST_FRAMES_SENT: r,
// DESCR__BROADCAST_FRAMES_SENT: 
#define IRTE_REG_BROADCAST_FRAMES_SENT_P3_RST__VAL                     0x00000000


#define IRTE_REG_FRAMES_UNDER_64BYTES_P3                                                                 0x0001B330
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P3__MSK_FRAMES_UNDER_64BYTES                                           0xFFFFFFFF
#define IRTE_REG_FRAMES_UNDER_64BYTES_P3_SHFT_FRAMES_UNDER_64BYTES                                           0
// ACCESS__FRAMES_UNDER_64BYTES: r,
// DESCR__FRAMES_UNDER_64BYTES: 
#define IRTE_REG_FRAMES_UNDER_64BYTES_P3_RST__VAL                      0x00000000


#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P3                                                        0x0001B334
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P3__MSK_FRAMES_WITH_LENGTH_OF_64BYTES                         0xFFFFFFFF
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P3_SHFT_FRAMES_WITH_LENGTH_OF_64BYTES                         0
// ACCESS__FRAMES_WITH_LENGTH_OF_64BYTES: r,
// DESCR__FRAMES_WITH_LENGTH_OF_64BYTES: 
#define IRTE_REG_FRAMES_WITH_LENGTH_OF_64BYTES_P3_RST__VAL             0x00000000


#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P3                                                           0x0001B338
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P3__MSK_FRAMES_BETWEEN_65_127BYTES                               0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P3_SHFT_FRAMES_BETWEEN_65_127BYTES                               0
// ACCESS__FRAMES_BETWEEN_65_127BYTES: r,
// DESCR__FRAMES_BETWEEN_65_127BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_65_127BYTES_P3_RST__VAL                0x00000000


#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P3                                                          0x0001B33C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P3__MSK_FRAMES_BETWEEN_128_255BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P3_SHFT_FRAMES_BETWEEN_128_255BYTES                             0
// ACCESS__FRAMES_BETWEEN_128_255BYTES: r,
// DESCR__FRAMES_BETWEEN_128_255BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_128_255BYTES_P3_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P3                                                          0x0001B340
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P3__MSK_FRAMES_BETWEEN_256_511BYTES                             0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P3_SHFT_FRAMES_BETWEEN_256_511BYTES                             0
// ACCESS__FRAMES_BETWEEN_256_511BYTES: r,
// DESCR__FRAMES_BETWEEN_256_511BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_256_511BYTES_P3_RST__VAL               0x00000000


#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P3                                                         0x0001B344
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P3__MSK_FRAMES_BETWEEN_512_1023BYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P3_SHFT_FRAMES_BETWEEN_512_1023BYTES                           0
// ACCESS__FRAMES_BETWEEN_512_1023BYTES: r,
// DESCR__FRAMES_BETWEEN_512_1023BYTES: 
#define IRTE_REG_FRAMES_BETWEEN_512_1023BYTES_P3_RST__VAL              0x00000000


#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P3                                                         0x0001B348
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P3__MSK_FRAMES_BETWEEN_1024_MAXBYTES                           0xFFFFFFFF
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P3_SHFT_FRAMES_BETWEEN_1024_MAXBYTES                           0
// ACCESS__FRAMES_BETWEEN_1024_MAXBYTES: r,
// DESCR__FRAMES_BETWEEN_1024_MAXBYTES: 
#define IRTE_REG_FRAMES_BETWEEN_1024_MAXBYTES_P3_RST__VAL              0x00000000


#define IRTE_REG_GOOD_FRAMES_RECEIVED_P3                                                                 0x0001B34C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P3__MSK_GOOD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P3_SHFT_GOOD_FRAMES_RECEIVED                                           0
// ACCESS__GOOD_FRAMES_RECEIVED: r,
// DESCR__GOOD_FRAMES_RECEIVED: 
#define IRTE_REG_GOOD_FRAMES_RECEIVED_P3_RST__VAL                      0x00000000


#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P3                                                                0x0001B350
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P3__MSK_BAD_FRAMES_RECEIVED                                           0xFFFFFFFF
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P3_SHFT_BAD_FRAMES_RECEIVED                                           0
// ACCESS__BAD_FRAMES_RECEIVED: r,
// DESCR__BAD_FRAMES_RECEIVED: 
#define IRTE_REG_TOTAL_FRAMES_RECEIVED_P3_RST__VAL                     0x00000000


#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P3                                            0x0001B354
// ACCESS__: ,
// DESCR__: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P3__MSK_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0xFFFFFFFF
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P3_SHFT_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC 0
// ACCESS__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: ,
// DESCR__RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC: 
#define IRTE_REG_RECEIVED_FRAMES_UNDER_64BYTE_WITH_BAD_CRC_P3_RST__VAL 0x00000000


#define IRTE_REG_RX_ERRORS_P3                                                                            0x0001B358
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_RX_ERRORS_P3__MSK_RX_ERRORS                                                                 0xFFFFFFFF
#define IRTE_REG_RX_ERRORS_P3_SHFT_RX_ERRORS                                                                 0
// ACCESS__RX_ERRORS: r,
// DESCR__RX_ERRORS: 
#define IRTE_REG_RX_ERRORS_P3_RST__VAL                                 0x00000000


#define IRTE_REG_OVERSIZE_FRAMES_P3                                                                      0x0001B35C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_OVERSIZE_FRAMES_P3__MSK_OVERSIZE_FRAMES                                                     0xFFFFFFFF
#define IRTE_REG_OVERSIZE_FRAMES_P3_SHFT_OVERSIZE_FRAMES                                                     0
// ACCESS__OVERSIZE_FRAMES: r,
// DESCR__OVERSIZE_FRAMES: 
#define IRTE_REG_OVERSIZE_FRAMES_P3_RST__VAL                           0x00000000


#define IRTE_REG_CRC_ALIGNMENT_ERROR_P3                                                                  0x0001B360
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P3__MSK_CRC_ALIGNMENT_ERROR                                             0xFFFFFFFF
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P3_SHFT_CRC_ALIGNMENT_ERROR                                             0
// ACCESS__CRC_ALIGNMENT_ERROR: r,
// DESCR__CRC_ALIGNMENT_ERROR: 
#define IRTE_REG_CRC_ALIGNMENT_ERROR_P3_RST__VAL                       0x00000000


#define IRTE_REG_FRAMES_DROPPED_P3                                                                       0x0001B364
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_FRAMES_DROPPED_P3__MSK_FRAMES_DROPPED                                                       0xFFFFFFFF
#define IRTE_REG_FRAMES_DROPPED_P3_SHFT_FRAMES_DROPPED                                                       0
// ACCESS__FRAMES_DROPPED: r,
// DESCR__FRAMES_DROPPED: 
#define IRTE_REG_FRAMES_DROPPED_P3_RST__VAL                            0x00000000


#define IRTE_REG_COLLISION_P3                                                                            0x0001B368
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_COLLISION_P3__MSK_COLLISION                                                                 0xFFFFFFFF
#define IRTE_REG_COLLISION_P3_SHFT_COLLISION                                                                 0
// ACCESS__COLLISION: r,
// DESCR__COLLISION: 
#define IRTE_REG_COLLISION_P3_RST__VAL                                 0x00000000


#define IRTE_REG_HOL_BOUNDARY_CROSSED_P3                                                                 0x0001B36C
// ACCESS__: r,
// DESCR__: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P3__MSK_HOL_BOUNDARY_CROSSED                                           0xFFFFFFFF
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P3_SHFT_HOL_BOUNDARY_CROSSED                                           0
// ACCESS__HOL_BOUNDARY_CROSSED: r,
// DESCR__HOL_BOUNDARY_CROSSED: 
#define IRTE_REG_HOL_BOUNDARY_CROSSED_P3_RST__VAL                      0x00000000





/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/


/*****************************************************************************/
/*  end of file irte_rsl.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  // of IRTE_RSL_H

