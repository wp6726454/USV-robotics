
#ifndef _INTEL_REG_H                       /* ----- reinclude-protection ----- */
#define _INTEL_REG_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: intel_reg.h                            :F&     */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  INTEL register description                       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.09.11    MA    initial version.                                       */
#endif
/*****************************************************************************/

/* --------------------------------------------------------------------------*/
/* INTEL hardware description                                                */
/* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------*/
/** Host Communication.
 *  Uses these macros to access the ethernet controller memory and registers.
 */
/* --------------------------------------------------------------------------*/

#define INTEL_BITI(bitname,value)        (0 == (value)?0:(LSA_UINT32)1<<(bitname))
#define INTEL_BIT(bitname,value)         INTEL_BITI(bitname,value)

#define INTEL_BITSI(start,end,value)     (((value) & ((1<<(1+(end)-(start)))-1))<<(start))
#define INTEL_BITS(bitsname,value)       INTEL_BITSI(bitsname##_s,bitsname##_e,value)

#define INTEL_GETBITI(bitname,value)     ((value>>(bitname))&1)
#define INTEL_GETBIT(bitname,value)      INTEL_GETBITI(bitname,value)

#define INTEL_GETBITSI(start,end,value)  ((((value) >>(start))& ((1<<(1+(end)-(start)))-1)))
#define INTEL_GETBITS(bitsname,value)    INTEL_GETBITSI(bitsname##_s,bitsname##_e,value)

#define INTEL_SETBITI(lvalue,bitname)    (lvalue |= INTEL_BITI(bitname,1))
#define INTEL_SETBIT(lvalue,bitname)     INTEL_SETBITI(lvalue,bitname)

#define INTEL_CLRBITI(lvalue,bitname)    (lvalue &= ~INTEL_BITI(bitname,1))
#define INTEL_CLRBIT(lvalue,bitname)     INTEL_CLRBITI(lvalue,bitname)

#define INTEL_CLRBITSI(lvalue,start,end) (lvalue &= ~(((1<<(1+(end)-(start)))-1) << (start)))
#define INTEL_CLRBITS(lvalue,bitsname)   INTEL_CLRBITSI(lvalue,bitsname##_s,bitsname##_e)

#define INTEL_SETBITSI(lvalue,start,end,value) (lvalue = INTEL_BITSI(start,end,value)|((lvalue & (~(((1<<(1+(end)-(start)))-1) << (start))))))
#define INTEL_SETBITS(lvalue,bitsname,value)   INTEL_SETBITSI(lvalue,bitsname##_s,bitsname##_e,value)

#define intel_basereg                  	(handle->params.pRegBaseAddr)

	//@fixme: refactoring to get rid of lint warings (lsa_uint8* casted to lsa_uint64*) NEA6.0
#define intel_reg64(addr)             	((volatile LSA_UINT64*)(((LSA_UINT8*)intel_basereg)+addr))[0]
#define intel_reg32(addr)             	(((volatile LSA_UINT32*)(((LSA_UINT8*)intel_basereg)+addr))[0])


//#define intel_reg16(addr)             	((volatile LSA_UINT16*)(((LSA_UINT8*)intel_basereg)+addr))[0]
//#define intel_reg8(addr)              	((volatile LSA_UINT8*)(((LSA_UINT8*)intel_basereg)+addr))[0]

/* --------------------------------------------------------------------------*/
/* PCI Configuration Registers                                               */
/* --------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------*/
/* PCI Control & Status Registers                                            */
/* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------*/
/** INTEL register addresses and register descriptions.
 * @see 8254x_GBe_SDM.pdf (317453006EN.PDF), Table 13-2
 *
 * macro build-up convention for register base addresses:
 * INTEL_ 'register name as in developers manual'
 * e.g.: '#define INTEL_CTRL_EXT 0x00018' for CTRL_EXT register base address.
 *
 * macro build-up convention for register bit address:
 * INTEL_ 'register name as in developers manual' _ 'bit name as in developer manual' (+ 's'/'e')
 * e.g.: '#define INTEL_CTRL_EXT_PHYINT    5' for PHYINT bit in CTRL_EXT register.
 *       '#define INTEL_CTRL_EXT_GPI_EN_s  0' for first GPI_EN bit in CTRL_EXT register.
 *       '#define INTEL_CTRL_EXT_GPI_EN_e  3' for last GPI_EN bit in CTRL_EXT register.
 */
/* --------------------------------------------------------------------------*/

/* general registers */
#define INTEL_CTRL					0x00000
	#define INTEL_CTRL_FD				0
	/* reserved 1 */
	//#define INTEL_CTRL_GIO_MASTER_DISABLE	2	/* springville */
	#define INTEL_CTRL_LRST				3
	/* reserved 4 */
	//#define INTEL_CTRL_ASDE				5
	#define INTEL_CTRL_SLU				6
	#define INTEL_CTRL_ILOS				7
	#define INTEL_CTRL_SPEED_s			8
	#define INTEL_CTRL_SPEED_e			9
	/* reserved 10 */
	#define INTEL_CTRL_FRCSPD			11
	#define INTEL_CTRL_FRCDPLX			12
	/* reserved 17:13 */
	//#define INTEL_CTRL_SDP0_DATA		18
	//#define INTEL_CTRL_SDP1_DATA		19
	//#define INTEL_CTRL_ADVD3WUC			20
	//#define INTEL_CTRL_EN_PHY_PWR_MGMT	21
	//#define INTEL_CTRL_SDP0_IODIR		22
	//#define INTEL_CTRL_SDP1_IODIR		23
	/* reserved 25:24 */
	#define INTEL_CTRL_RST				26
	#define INTEL_CTRL_RFCE				27
	#define INTEL_CTRL_TFCE				28
	#define INTEL_CTRL_DEV_RST			29	/* springville */
	//#define INTEL_CTRL_VME				30
	#define INTEL_CTRL_PHY_RST			31

#define INTEL_STATUS				0x00008
	#define INTEL_STATUS_FD				0
	#define INTEL_STATUS_LU				1
	//#define INTEL_STATUS_FunctionID_s	2
	//#define INTEL_STATUS_FunctionID_e	3
	//#define INTEL_STATUS_TXOFF			4
	//#define INTEL_STATUS_TBIMODE		5
	#define INTEL_STATUS_SPEED_s		6
	#define INTEL_STATUS_SPEED_e		7
	//#define INTEL_STATUS_ASDV_s			8
	//#define INTEL_STATUS_ASDV_e			9
	/* reserved 10 */
	//#define INTEL_STATUS_PCI66			11
	//#define INTEL_STATUS_BUS64			12
	//#define INTEL_STATUS_PCIX_MODE		13
	//#define INTEL_STATUS_PCIXSPD_s		14
	//#define INTEL_STATUS_PCIXSPD_e		15
	#define INTEL_STATUS_PF_RST_DONE	21	/* springville */
	/* reserved 31:16 */


#define INTEL_EECD					0x00010
	#define INTEL_EDCD_AUTO_RD		9

//#define INTEL_EERD					0x00014
#define INTEL_CTRL_EXT				0x00018
//#define INTEL_FLA					0x0001C
	//#define INTEL_CTRL_EXT_GPI_EN_s		0
	//#define INTEL_CTRL_EXT_GPI_EN_e		3
	/* reserved 4 */
	//#define INTEL_CTRL_EXT_PHYINT		5
	//#define INTEL_CTRL_EXT_SDP6_DATA	6
	//#define INTEL_CTRL_EXT_SDP7_DATA	7
	/* reserved 9:8 */
	//#define INTEL_CTRL_EXT_SDP6_IODIR	10
	//#define INTEL_CTRL_EXT_SDP7_IODIR	11
	//#define INTEL_CTRL_EXT_ASDCHK		12
	//#define INTEL_CTRL_EXT_EE_RST		13
	/* reserved 14 */
	#define INTEL_CTRL_EXT_SPD_BYPS		15
	/* reserved 16*/
	//#define INTEL_CTRL_EXT_RO_DIS		17
	/* reserved 20:18 */
	//#define INTEL_CTRL_EXT_VREG_POWER_DOWN	21
	//#define INTEL_CTRL_EXT_LINK_MODE_s	22
	//#define INTEL_CTRL_EXT_LINK_MODE_e	23
	/* reserved 24:31 */
	#define INTEL_CTRL_EXT_DRV_LOAD		28

#define INTEL_MDIC					0x00020
#define INTEL_MDIC_DATA_s			0
#define INTEL_MDIC_DATA_e			15
#define INTEL_MDIC_REGADD_s			16
#define INTEL_MDIC_REGADD_e			20
#define INTEL_MDIC_PHYADD_s			21
#define INTEL_MDIC_PHYADD_e			25
#define INTEL_MDIC_OP_s				26
#define INTEL_MDIC_OP_e				27
	#define INTEL_MDIC_OP_MDI_WRITE		1
	#define INTEL_MDIC_OP_MDI_READ		2
	/* 0+3: reserved opcodes */
#define INTEL_MDIC_R				28
#define INTEL_MDIC_I				29
#define INTEL_MDIC_E				30
/* reserved 31 */

//#define INTEL_FCAL					0x00028
//#define INTEL_FCAH					0x0002C
//#define INTEL_FCT					0x00030
//#define INTEL_VET					0x00038
//#define INTEL_FCTTV					0x00170
//#define INTEL_TXCW					0x00178
//#define INTEL_RXCW					0x00180

/* LED control register */
#define INTEL_LEDCTL				0x00E00
  //#define INTEL_LEDCTL_LED0_MODE_s          0
  //#define INTEL_LEDCTL_LED0_MODE_e          3
  //#define INTEL_LEDCTL_LED_PCI_MODE         4
  //#define INTEL_LEDCTL_GLOBAL_BLINK_MODE    5
  //#define INTEL_LEDCTL_LED0_IVRT            6
  //#define INTEL_LEDCTL_LED0_BLINK           7
  //#define INTEL_LEDCTL_LED1_MODE_s          8
  //#define INTEL_LEDCTL_LED1_MODE_e          11
/* reserved [13:12] */
  //#define INTEL_LEDCTL_LED1_IVRT            14
  //#define INTEL_LEDCTL_LED1_BLINK           15
  //#define INTEL_LEDCTL_LED2_MODE_s      16
  //#define INTEL_LEDCTL_LED2_MODE_e      19
/* reserved [21:20] */
  //#define INTEL_LEDCTL_LED2_IVRT        22
  //#define INTEL_LEDCTL_LED2_BLINK       23
/* reserved [27:24] */
/* reserved [29:28] */
/* reserved [31:30] */

//#define INTEL_LEDCTL_DEFAULT_LEDS_WORKING   0x00078406
#define INTEL_LEDCTL_DEFAULT_LEDS_ON        0x000E0E0F
#define INTEL_LEDCTL_DEFAULT_LEDS_OFF       0x000F0F0F

/* internal phy configuration register */
#define INTEL_IPCNFG                0x00E38
  #define INTEL_IPCNFG_EEE_100M_AN    2
  #define INTEL_IPCNFG_EEE_1G_AN      3

    /* internal phy configuration register */
#define INTEL_PHPM                  0x00E14
    #define INTEL_PHPM_SPD_EN             0
    #define INTEL_PHPM_GO_LINK_DISCONNECT 5


#define INTEL_EXTCNF_CTRL           0x00F00
    /* reserved [31:28] */
    /* reserved [27:16] */
    /* reserved [15:8] */
    #define INTEL_EXTCNF_CTRL_MDIO_MNG_OWNERSHIP    7
    #define INTEL_EXTCNF_CTRL_MDIO_HW_OWNERSHIP     6
    #define INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP     5
    /* reserved [4] */
    /* reserved [3] */
    /* reserved [2] */
    /* reserved [1] */
    /* reserved [0], write 0 */

/* DMA registers */
//F#define INTEL_PBA					0x01000

/* Interrupt registers */
#define INTEL_ICR					0x000C0
#define INTEL_ITR					0x000C4
#if ! (defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) || defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE))
#define INTEL_ICS                   0x000C8
#define INTEL_IMS					0x000D0
#endif
#define INTEL_IMC					0x000D8

#define INTEL_INTERRUPT_TXDW			0
#define INTEL_INTERRUPT_TXQE			1  /* only 82574 */
#define INTEL_INTERRUPT_LSC			2
//#define INTEL_INTERRUPT_RXSEQ			3
//#define INTEL_INTERRUPT_RXDMT0		4
/* reserved 5 */
//#define INTEL_INTERRUPT_RXO			6
#define INTEL_INTERRUPT_RXT0			7
/* reserved 8 */
//#define INTEL_INTERRUPT_MDAC			9
//#define INTEL_INTERRUPT_RXCFG			10
/* reserved 11 */
//#define INTEL_INTERRUPT_PHYINT		12
//#define INTEL_INTERRUPT_GPI_s			13
//#define INTEL_INTERRUPT_GPI_e			14
//#define INTEL_INTERRUPT_TXD_LOW		15
//#define INTEL_INTERRUPT_SRPD			16
/* reserved 19:17 */
#define INTEL_INTERRUPT_RXQ0			20	/* only 82574 */
#define INTEL_INTERRUPT_TXQ0            22  /* only 82574 */
/* reserved 31:21 */

  // 82574 GbE Controller
  //   --> INT_ASSERTED Bit 31: Interrupt Asserted
  //     This bit is set when the LAN port has a pending interrupt. If the
  //     interrupt is enabled in the PCI configuration space, an interrupt is
  //     asserted.
  // SPVL
  //   -->INTA 31: Interrupt Asserted.
  //     Indicates that the INT line is asserted. Can be used by the software device driver
  //     in a shared interrupt scenario to decide if the received interrupt was emitted by
  //     the I210. This bit is not valid in MSI/MSI-X environments.
  //#define INTEL_INTERRUPT_MASK    0x7FFFFFFF

/* Receive registers */
#define INTEL_RCTL					0x00100
	/* reserved 0 */
	#define INTEL_RCTL_EN			1
	//#define INTEL_RCTL_SBP			2
	#define INTEL_RCTL_UPE			3
	#define INTEL_RCTL_MPE			4
	//#define INTEL_RCTL_LPE			5
	//#define INTEL_RCTL_LBM_s		6
	//#define INTEL_RCTL_LBM_e		7
	//#define INTEL_RCTL_RDMTS_s		8
	//#define INTEL_RCTL_RDMTS_e		9
	/* reserved 11:10 */
	//#define INTEL_RCTL_MO_s			12
	//#define INTEL_RCTL_MO_e			13
	/* reserved 14 */
	#define INTEL_RCTL_BAM			15
	//#define INTEL_RCTL_BSIZE_s		16
	//#define INTEL_RCTL_BSIZE_e		17
	//#define INTEL_RCTL_VFE			18
	//#define INTEL_RCTL_CFIEN		19
	//#define INTEL_RCTL_CFI			20
	/* reserved 21 */
	//#define INTEL_RCTL_DPF			22
	//#define INTEL_RCTL_PMCF			23
	/* reserved 24 */
	//#define INTEL_RCTL_BSEX			25
	#define INTEL_RCTL_SECRC		26
	/* reserved 27:31 */

//#define INTEL_FCRTL					0x02160
//#define INTEL_FCRTH					0x02168
#define INTEL_RDBAL				    0x02800
//#define INTEL_RDBAH				    0x02804
#define INTEL_RDLEN				    0x02808
#define INTEL_RDH				    0x02810
#define INTEL_RDT				    0x02818
//#define INTEL_RDTR				    0x02820
//#define INTEL_RADV				    0x0282C
//#define INTEL_RSRPD					0x02C00
#define INTEL_MTA_0_127(number)		(0x05200+(4*(number)))
#define INTEL_RAL_0_15(number)		(0x05400+(8*(number)))
#define INTEL_RAH_0_15(number)		(0x05404+(8*(number)))
#define INTEL_RAH_AV				31
//#define INTEL_RAH_AS_s				16
//#define INTEL_RAH_AS_e				17
#define INTEL_VFTA_0_127(number)	(0x05600+(4*(number)))

/* Wakeup registers */
//#define INTEL_WUC					0x05800
//#define INTEL_WUFC					0x05808
//#define INTEL_WUS					0x05810
//#define INTEL_IPAV					0x05838
//#define INTEL_IP4AT0				0x05840
//#define INTEL_IP4AT1				0x05844
//#define INTEL_IP4AT2				0x05848
//#define INTEL_IP4AT3				0x0584C
//#define INTEL_IP6AT_mem32x4			0x05880
//#define INTEL_WUPL					0x05900
//#define INTEL_WUPM_mem8x128			0x05A00
//#define INTEL_FFLT_mem64x4			0x05F00
//#define INTEL_FFMT_mem64x128		0x09000
//#define INTEL_FFVT_mem64x128		0x09800

/* Manageability registers */
#define INTEL_MANC					0x05820
#define INTEL_MANC_KEEP_PHY_LINK_UP     18
    #define INTEL_MANC_ARP_EN	        0x00002000 /* Enable ARP Request Filtering */
    #define INTEL_MANC_EN_MNG2HOST		0x00200000 /* Enable MNG packets to host memory */

#define INTEL_MANC2H				0x05860

    /* Semaphore registers */
    #define INTEL_SWSM        0x5B50
      /* reserved 0 */
      #define INTEL_SWSM_SWESMBI 1
    /* reserved 30:2 */
    /* reserved 31 */

    #define INTEL_FWSM        0x5B54
      #define INTEL_FWSM_EEP_FW_Semaphore   0
      #define INTEL_FWSM_FW_Mode_s          1
      #define INTEL_FWSM_FW_Mode_e          3
      /* reserved 5:4 */
      #define INTEL_FWSM_EEP_Reload_Ind     6
      /* reserved 14:7 */
      #define INTEL_FWSM_FW_Val_Bit         15
      #define INTEL_FWSM_Reset_Cnt_s        16
      #define INTEL_FWSM_Reset_Cnt_e        18
      #define INTEL_FWSM_Ext_Err_Ind_s      19
      #define INTEL_FWSM_Ext_Err_Ind_e      24
      #define INTEL_FWSM_PCIe_Config_Err_Ind  25
      #define INTEL_FWSM_PHY_SERDES0_Config_Err_Ind 26
      /* reserved 27:30 */
      #define INTEL_FWSM_Factory_MAC_address_restored 31

    #define INTEL_SW_FW_SYNC  0x5B5C
      #define INTEL_SW_FW_SYNC_SW_FLASH_SM 0
      #define INTEL_SW_FW_SYNC_SW_PHY_SM 1
      #define INTEL_SW_FW_SYNC_SW_I2C_SM 2
      #define INTEL_SW_FW_SYNC_SW_MAC_CSR_SM 3
      /* Reserved 6:4 */
      #define INTEL_SW_FW_SYNC_SW_SVR_SM 7
      #define INTEL_SW_FW_SYNC_SW_MB_SM 8
      /* Reserved 9 */
      #define INTEL_SW_FW_SYNC_SW_MNG_SM 10
      /* Reserved 15:11 */
      #define INTEL_SW_FW_SYNC_FW_FLASH_SM 16
      #define INTEL_SW_FW_SYNC_FW_PHY_SM 17
      #define INTEL_SW_FW_SYNC_FW_I2C_SM 18
      #define INTEL_SW_FW_SYNC_FW_MAC_CSR_SM 19
      /* Reserved 22:20 */
      #define INTEL_SW_FW_SYNC_FW_SVR_SM 23
      /* Reserved 31:24*/

/* Transmit registers */
#define INTEL_TCTL					0x00400
	/* reserved 0 */
	#define INTEL_TCTL_EN			1
	/* reserved 2 */
	#define INTEL_TCTL_PSP			3
	#define INTEL_TCTL_CT_s			4
	#define INTEL_TCTL_CT_e			11
	#define INTEL_TCTL_COLD_s		12
	#define INTEL_TCTL_COLD_e		21
	//#define INTEL_TCTL_SWXOFF		22
	/* reserved 23 */
	//#define INTEL_TCTL_RTLC			24
	/* reserved 25 */
	/* reserved 31:26 */

//#define INTEL_TIPG					0x00410
//	#define INTEL_TIPG_IPGT_s		0
//	#define INTEL_TIPG_IPGT_e		9
//	#define INTEL_TIPG_IPGR1_s		10
//	#define INTEL_TIPG_IPGR1_e		19
//	#define INTEL_TIPG_IPGR2_s		20
//	#define INTEL_TIPG_IPGR2_e		29
/* reserved 31:30 */

//#define INTEL_AIFS						0x00458
#define INTEL_TDBAL					    0x03800
//#define INTEL_TDBAH					    0x03804
#define INTEL_TDLEN					    0x03808
#define INTEL_TDH					    0x03810
#define INTEL_TDT					    0x03818
//#define INTEL_TIDV					    0x03820

/* TX DMA registers */
//#define INTEL_TXDMAC				0x03000
#define INTEL_TXDCTL				0x03828
	//#define INTEL_TXDCTL_WTHRESH_s	16F
	//#define INTEL_TXDCTL_WTHRESH_e	20
	#define INTEL_TXDCTL_ENABLE		25
//#define INTEL_TADV					0x0282C
//#define INTEL_TSPMT					0x03830

/* RX DMA registers */
#define INTEL_RXDCTL				0x02828
	//#define INTEL_RXDCTL_WTHRESH_s	16
	//#define INTEL_RXDCTL_WTHRESH_e	20
	#define INTEL_RXDCTL_ENABLE		25
//#define INTEL_RXCSUM				0x05000

/* Statistic registers */
#define INTEL_CRCERRS				0x04000
//#define INTEL_ALGNERRC				0x04004
//#define INTEL_SYMERRS				0x04008
#define INTEL_RXERRC				0x0400C
#define INTEL_MPC					0x04010
//#define INTEL_SCC					0x04014
//#define INTEL_ECOL					0x04018
//#define INTEL_MCC					0x0401C
//#define INTEL_LATECOL				0x04020
//#define INTEL_COLC					0x04028
//#define INTEL_DC					0x04030
//#define INTEL_TNCRS					0x04034
//#define INTEL_SEC					0x04038
//#define INTEL_CEXTERR				0x0403C
#define INTEL_RLEC					0x04040
//#define INTEL_XONRXC				0x04048
//#define INTEL_XONTXC				0x0404C
//#define INTEL_XOFFRXC				0x04050
//#define INTEL_XOFFTXC				0x04054
//#define INTEL_FCRUC					0x04058
//#define INTEL_PRC64					0x0405C
//#define INTEL_PRC127				0x04060
//#define INTEL_PRC255				0x04064
//#define INTEL_PRC511				0x04068
//#define INTEL_PRC1023				0x0406C
//#define INTEL_PRC1522				0x04070
#define INTEL_GPRC					0x04074
#define INTEL_BPRC					0x04078
#define INTEL_MPRC					0x0407C
#define INTEL_GPTC					0x04080
//#define INTEL_GORC_reg64			0x04088
#define INTEL_GORCL					0x04088
//#define INTEL_GORCH					0x0408C
//#define INTEL_GOTC_reg64			0x04090
#define INTEL_GOTCL					0x04090
//#define INTEL_GOTCH					0x04094
//#define INTEL_RNBC					0x040A0
//#define INTEL_RUC					0x040A4
//#define INTEL_RFC					0x040A8
//#define INTEL_ROC					0x040AC
//#define INTEL_RJC					0x040B0
//#define INTEL_MGTPRC				0x040B4
//#define INTEL_MGTPDC				0x040B8
//#define INTEL_MGTPTC				0x040BC
//#define INTEL_TOR_reg64				0x040C0
//#define INTEL_TORL					0x040C0
//#define INTEL_TORH					0x040C4
//#define INTEL_TOT_reg64				0x040C8
//#define INTEL_TOTL					0x040C8
//#define INTEL_TOTH					0x040CC
//#define INTEL_TPR					0x040D0
//#define INTEL_TPT					0x040D4
//#define INTEL_PTC64					0x040D8
//#define INTEL_PTC127				0x040DC
//#define INTEL_PTC255				0x040E0
//#define INTEL_PTC511				0x040E4
//#define INTEL_PTC1023				0x040E8
//#define INTEL_PTC1522				0x040EC
#define INTEL_MPTC					0x040F0
#define INTEL_BPTC					0x040F4
//#define INTEL_TSCTC					0x040F8
//#define INTEL_TSCTFC				0x040FC

/* Diagnostic  Registers */
//#define INTEL_RDFH					0x02410
//#define INTEL_RDFT					0x02418
//#define INTEL_RDFHS					0x02420
//#define INTEL_RDFTS					0x02428
//#define INTEL_RDFPC					0x02430
//#define INTEL_TDFH					0x03410
//#define INTEL_TDFT					0x03418
//#define INTEL_TDFHS					0x03420
//#define INTEL_TDFTS					0x03428
//#define INTEL_TDFPC					0x03430
//#define INTEL_PBM_mem32x16k			0x10000
//#define INTEL_PBM_mem64x8k			0x10000


/* --------------------------------------------------------------------------*/
/* EEPROM Interface                                                          */
/* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------*/
/* PHY Interface                                                             */
/* --------------------------------------------------------------------------*/
/* MDI access register see INTEL_MDIC */
#define INTEL_PHY_PCTRL				0
//#define INTEL_PHY_PSTATUS			1
//#define INTEL_PHY_PID				2
//#define INTEL_PHY_EPID				3

#define INTEL_PHY_ANA				4
#define INTEL_PHY_ANA_Selector_s	0
#define INTEL_PHY_ANA_Selector_e	4
#define INTEL_PHY_ANA_10BASETXHD	5
#define INTEL_PHY_ANA_10BASETXFD	6
#define INTEL_PHY_ANA_100BASETXHD	7
#define INTEL_PHY_ANA_100BASETXFD	8


//#define INTEL_PHY_LPA				5
//#define INTEL_PHY_ANE				6
//#define INTEL_PHY_NPT				7
//#define INTEL_PHY_LPN				8

#define INTEL_PHY_GCON				9
/* reserved 7:0 */
#define INTEL_PHY_GCON_1000BASETHD	8
#define INTEL_PHY_GCON_1000BASETFD	9


//#define INTEL_PHY_GSTATUS			10
/* reserved 11..14 */
//#define INTEL_PHY_EPSTATUS			15
#define INTEL_PHY_PSCON				16
    #define INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_s 12
    #define INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_e 14
    #define INTEL_PHY_PSCON_DOWNSHIFT_ENABLE    11
//#define INTEL_PHY_PSSTAT			17
//#define INTEL_PHY_PINTE				18
//#define INTEL_PHY_PPCONT			18
//#define INTEL_PHY_PINTS				19
//#define INTEL_PHY_EPSCON1			20
//#define INTEL_PHY_PREC				21
/* reserved 22 */
//#define INTEL_PHY_PGSTAT			23
//#define INTEL_PHY_PLED				24
/* reserved 25 */
//#define INTEL_PHY_EPSCON2			26
//#define INTEL_PHY_EPSSTAT			27
/* reserved 28 */
//#define INTEL_PHY_R30PS				29
//#define INTEL_PHY_R30AW				30
//#define INTEL_PHY_PPAGE				31

/* Flash-Security Registers */
#define INTEL_EEMNGCTL              0x12030
    /* reserved 17:0 */
    #define INTEL_EEMNGCTL_CFG_DONE   18
    /* reserved 31:19 */

/* --------------------------------------------------------------------------*/
/* Supported DEVICE IDs                                                      */
/* --------------------------------------------------------------------------*/
/* All supported device IDs via Porgramming Interface, PCI, PCIe, USB...
 * @note only supported IDs should be here.
 */
enum INTEL_DEVICE_ID {
	IG82574L = 0x10D3,
	I210 = 0x1533
};

/* --------------------------------------------------------------------------*/
/* Intel hardware structures                                                 */
/* --------------------------------------------------------------------------*/

#define INTEL_TDESC_ARRAY_GRANULARITY   8
#define INTEL_RDESC_ARRAY_GRANULARITY   8

/** RDESC.
 *  Receive descriptor with fields.
 */
typedef struct INTEL_RDESC_{
	LSA_UINT64	BufferAddress;
	LSA_UINT16	Length;
	LSA_UINT16	packetChecksum;
	LSA_UINT8	Status;
	LSA_UINT8	Errors;
	LSA_UINT16	Special;
} INTEL_RDESC/*, *INTEL_RDESC_PTR*/;

//#define INTEL_RDESC_STATUS_PIF		7
//#define INTEL_RDESC_STATUS_IPCS		6
//#define INTEL_RDESC_STATUS_TCPCS	5
//#define INTEL_RDESC_STATUS_RSV		4
//#define INTEL_RDESC_STATUS_VP		3
//#define INTEL_RDESC_STATUS_IXSM		2
//#define INTEL_RDESC_STATUS_EOP		1
#define INTEL_RDESC_STATUS_DD		0

//#define INTEL_RDESC_ERRORS_RXE		7
//#define INTEL_RDESC_ERRORS_IPE		6
//#define INTEL_RDESC_ERRORS_TCPE		5
/* reserved bit 4 */
//#define INTEL_RDESC_ERRORS_CXE		4 	/* only IG82544GC/EI */
/* reserved bit 3 */
//#define INTEL_RDESC_ERRORS_SEQ		2
/* reserved bit 2: only IG82547GI/EI & IG82540EP/EM */
//#define INTEL_RDESC_ERRORS_SE		1
/* reserved bit 1: only IG82547GI/EI & IG82540EP/EM */
//#define INTEL_RDESC_ERRORS_CE		0

//#define INTEL_RDESC_SPECIAL_VLAN_s	0
//#define INTEL_RDESC_SPECIAL_VLAN_e	11
//#define INTEL_RDESC_SPECIAL_CFI		12
//#define INTEL_RDESC_SPECIAL_PRI_s	13
//#define INTEL_RDESC_SPECIAL_PRI_e	15

//#define INTEL_EERD_EEWR_MAX_COUNT	512 /* buffered EEPROM words rw */


/** TDESC.
 *  Transmit descriptor with fields.
 *  @note currently only legacy TDESC layout type is used.
 */
typedef union INTEL_TDESC_{
	struct INTEL_TDESC_LEGACY {
		LSA_UINT64	BufferAddress;
		LSA_UINT16	Length;
		LSA_UINT8	CSO;
		LSA_UINT8	CMD;
		LSA_UINT8	STA_RSV;
		LSA_UINT8	CSS;
		LSA_UINT16	Special;
	} legacy;
	//struct INTEL_TDESC_TYPE0000 {
	//	LSA_UINT8	IPCSS;
	//	LSA_UINT8	IPCSO;
	//	LSA_UINT16	IPCSE;
	//	LSA_UINT8	TUCSS;
	//	LSA_UINT8	TUCSO;
	//	LSA_UINT16	TUCSE;
	//	LSA_UINT32	PAYLEN_DTYP_TUCMD;
	//	LSA_UINT8	STA_RSV;
	//	LSA_UINT8	HDRLEN;
	//	LSA_UINT16	MSS;
	//} type0000;
	//struct INTEL_TDESC_TYPE0001 {
	//	LSA_UINT64	BufferAddress;
	//	LSA_UINT32	DTALEN_DTYP_DCMD;
	//	LSA_UINT8	STA_RSV;
	//	LSA_UINT8	POPTS;
	//	LSA_UINT16	Special;
	//} type0001;
} INTEL_TDESC/*, *INTEL_TDESC_PTR*/;

#define INTEL_TDESC_STA_RSV_DD       0


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* _INTEL_REG_H */
