
#ifndef _INTEL_USR_H                      /* ----- reinclude-protection ----- */
#define _INTEL_USR_H

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
/*  F i l e               &F: INTEL_usr.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level user header (PCAP)                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  23.06.10    AM    initial version.                                       */
#endif
/*****************************************************************************/

/* per default use only one rx and tx queue */

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_OPEN(LSA_VOID_PTR_TYPE pLLManagement,
			EDDS_HANDLE				hDDB,
			EDDS_UPPER_DPB_PTR_TYPE pDPB,
			LSA_UINT32				TraceIdx);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SETUP(LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_DSB_PTR_TYPE pDSB);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SHUTDOWN(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV(	LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_PROVIDE(
		LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE pBufferAddr, LSA_UINT32 Length,
		LSA_UINT32 PortID);

#ifdef EDDS_CFG_SEND_BLOCKED
LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_TRIGGER(LSA_VOID_PTR_TYPE pLLManagement);
#endif

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_STS(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_REINIT_TX(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_STATS(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 PortID, EDDS_LOCAL_STATISTICS_PTR_TYPE pStats);

LSA_RESULT  EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_CAPS(
	LSA_VOID_PTR_TYPE 					pLLManagement,
    EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE	pCaps);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
	    EDDS_LOCAL_MEM_U8_PTR_TYPE  pLinkSpeedModeConfigured);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 LinkStat,
		LSA_UINT8 PHYPower);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CHECK_MAUTYPE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT16 MAUType,
		LSA_UINT8 PHYPower, EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE_SETUP(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pPHYPower,
		LSA_BOOL EDDS_LOCAL_MEM_ATTR * pIsWireless);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_ENABLE(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_MAC_ADDR(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_BACKUP_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_RESTORE_MODE(
	LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_SET_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_BOOL LEDOn);

#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RX_OVERLOAD_RX_ON(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RX_OVERLOAD_RX_OFF(
		LSA_VOID_PTR_TYPE pLLManagement);
#endif

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
  LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RECURRING_TASK(LSA_VOID_PTR_TYPE pLLManagement);
#else /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
LSA_UINT32 EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 IntSource);
#endif

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_TX(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_RX(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_BOOL EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_POLL(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32* IntSource);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_PHY(
		LSA_VOID_PTR_TYPE pLLManagement);

#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_TIMEOUT(LSA_VOID_PTR_TYPE pLLManagement);

/*---------------------------------------------------------------------------*/
/* Define the INTEL_LL_HANDLE_TYPE for use by EDDS                            */
/*---------------------------------------------------------------------------*/

/* minimum size for framebuffer to receive 1518 byte is 2048 byte */
typedef LSA_UINT8 INTEL_FRAMEBUFFER[2048];
typedef INTEL_FRAMEBUFFER INTEL_RX_FRAMEBUFFER;
//typedef INTEL_FRAMEBUFFER INTEL_TX_FRAMEBUFFER;


typedef struct _INTEL_LL_PARAMETER_TYPE
{
    LSA_VOID_PTR_TYPE 	pRegBaseAddr;
    LSA_UINT16			DeviceID;
    LSA_BOOL            Disable1000MBitSupport;   // Defines if 1000 MBit shall not be supported with fixed
                                                  // setting and with Autoneg. With autoneg, the PHY does not
                                                  // advertise 1000 MBit support if configured for autoneg.
                                                  // With fixed setting the PRM – Record (PortDataAdjust) will
                                                  // be rejected if a 1000 MBit MAUType shall be configured.
}INTEL_LL_PARAMETER_TYPE;

typedef struct _INTEL_LL_HANDLE_TYPE
{
	INTEL_LL_PARAMETER_TYPE		params;
    LSA_UINT32					TraceIdx;
    LSA_VOID_PTR_TYPE			hDDB;
    EDDS_SYS_HANDLE             hSysDev;
    LSA_UINT32					pendingIRQs;
    LSA_UINT32					receiveInterruptBitMask;
    LSA_UINT32                  receiveInterruptBitMaskBackup;
    LSA_UINT32                  transmitInterruptBitMask;

    struct INTEL_RDESC_*     	pRxDescriptors;   // start of Tx descr --> aligned address
    EDD_UPPER_MEM_PTR_TYPE      pRxDescrAlloc;    // start of allocated memory for Rx descr
    INTEL_RX_FRAMEBUFFER**      RxFrameBufferPtr;
    LSA_UINT32					RxFrameBufferCount;
    LSA_UINT32					RXDescriptorIndex;
    LSA_UINT32					RXDescriptorIndexEnd;
    //LSA_UINT32					RXDescriptorsUsed;

    union INTEL_TDESC_*			pTxDescriptors;   // start of Tx descr --> aligned address
    EDD_UPPER_MEM_PTR_TYPE      pTxDescrAlloc;    // start of allocated memory for Tx descr
    LSA_UINT32					TxFrameBufferCount;
    LSA_UINT32					TXDescriptorIndex;
    LSA_UINT32					TXDescriptorIndexEnd;
    LSA_UINT32					TXDescriptorsUsed;
    EDD_MAC_ADR_TYPE        	InterfaceMAC;
    EDD_MAC_ADR_TYPE        	PortMAC[1];
    LSA_UINT64					PortStats[0x40];
    LSA_UINT64                  InUcastPktsShadow;
    LSA_UINT64                  OutUcastPktsShadow;

    LSA_UINT32					MCRegL[16];
    LSA_UINT32					MCRegH[16];
    LSA_UINT32                  backupLEDCTL;
    LSA_UINT32                  savedLEDCTL;
    LSA_UINT32                  fakeSendCounter;
    LSA_BOOL                    isLinkUp;
    LSA_UINT8	                resetState;
    //LSA_UINT8                   stateTxFIFOFlush;
    LSA_UINT8					savedLinkStat;
    LSA_UINT8					savedPHYPower;
    LSA_UINT32					lastRXHead;

    LSA_UINT32                  AutoNegCapAdvertised;
    LSA_UINT8                   LinkSpeedModeConfigured;
} INTEL_LL_HANDLE_TYPE;

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* _INTEL_USR_H_ */
