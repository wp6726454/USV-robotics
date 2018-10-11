
#ifndef _PACKET32_USR_H                      /* ----- reinclude-protection ----- */
#define _PACKET32_USR_H

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
/*  F i l e               &F: packet32_usr.h                            :F&  */
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


LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_OPEN(LSA_VOID_PTR_TYPE pLLManagement,
	EDDS_HANDLE				hDDB,
	EDDS_UPPER_DPB_PTR_TYPE pDPB,
	LSA_UINT32				TraceIdx);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SETUP(	LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_DSB_PTR_TYPE pDSB);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SHUTDOWN(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV(	LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_PROVIDE(
		LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE pBufferAddr, LSA_UINT32 Length,
		LSA_UINT32 PortID);

#ifdef EDDS_CFG_SEND_BLOCKED
LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_TRIGGER(LSA_VOID_PTR_TYPE pLLManagement);
#endif

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_STS(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_REINIT_TX(LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_STATS(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 PortID, EDDS_LOCAL_STATISTICS_PTR_TYPE pStats);

LSA_RESULT  EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_CAPS(
	LSA_VOID_PTR_TYPE 					pLLManagement,
    EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE	pCaps);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE  pLinkSpeedModeConfigured);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 LinkStat,
		LSA_UINT8 PHYPower);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CHECK_MAUTYPE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT16 MAUType,
		LSA_UINT8 PHYPower, EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE_SETUP(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pPHYPower,
		LSA_BOOL EDDS_LOCAL_MEM_ATTR * pIsWireless);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_ENABLE(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_MAC_ADDR(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_BACKUP_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_RESTORE_MODE(
	LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_SET_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_BOOL LEDOn);

#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RX_OVERLOAD_RX_ON(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RX_OVERLOAD_RX_OFF(
		LSA_VOID_PTR_TYPE pLLManagement);
#endif

#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
LSA_UINT32 EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 IntSource);
#endif

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_TX(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_RX(
		LSA_VOID_PTR_TYPE pLLManagement);

LSA_BOOL EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_POLL(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32* IntSource);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_PHY(
		LSA_VOID_PTR_TYPE pLLManagement);

#ifdef LLIF_CFG_SWITCH_SUPPORT
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_PORTID_MODE_PTR_TYPE pPortIDModeArray,
		LSA_UINT16 PortIDModeCnt,
		LSA_UINT16 MACAddrPrio,
		LSA_UINT16 MACAddrGroup,
		LSA_UINT32 MACAddrLow);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_PORT_STATE(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE pPortStates);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_FLUSH_FILTERING_DB(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE pFlushFilterDB);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_PORT_REDUNDANT(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_SWI_SET_PORT_REDUNDANT_PTR_TYPE pPortRed);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_BRIDGE_MODE(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 Mode);

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_GET_DEFAULT_MRP_R_PORTS(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID1,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID2);

#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_DROP_CNT(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_U32_PTR_TYPE pDropCnt);
#endif
#endif

/*---------------------------------------------------------------------------*/
/* Define the PACKET32_LL_HANDLE_TYPE for use by EDDS                        */
/*---------------------------------------------------------------------------*/

typedef struct _PACKET32_LL_HANDLE_TYPE
{
    LPADAPTER 			pAdapter;
    LSA_UINT32			TraceIdx;
    EDDS_HANDLE			hDDB;
    EDDS_SYS_HANDLE     hSysDev;         /* system handle of device */
    LSA_VOID_PTR_TYPE	pSendBuffer;		/* 1x EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM()*/
    LSA_VOID_PTR_TYPE	pRecvBuffer;		/* 1x EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM()*/
    LPPACKET  pSendPacketStruct;	/* pointer to packet32 user send buffer */
    LPPACKET  pRecvPacketStruct;	/* pointer to packet32 user send buffer */
    struct dump_bpf_hdr* pSendPacket;	/* pointer to actual send bpf_hdr */
    LSA_UINT32	sendPackets;
    struct bpf_hdr* pRecvPacket;	/* pointer to actual recv bpf_hdr */
    NDIS_MEDIA_STATE linkState;		/* saved link state */
    LSA_UINT32	pendingInterrupts;
    LSA_UINT32  RxDropCnt;          /* counts received but dropped frames, e.g. oversized frames */
    EDD_MAC_ADR_TYPE    MACAddress;
    EDD_MAC_ADR_TYPE    MACAddressPort[EDD_CFG_MAX_PORT_CNT]; /* port mac addresses */
} PACKET32_LL_HANDLE_TYPE;

/*---------------------------------------------------------------------------*/
/* Packet32 LL specific defines                                              */
/*---------------------------------------------------------------------------*/
#define LLIF_CFG_KERNEL_BUFFER_SIZE		(512*1024)
#define	LLIF_CFG_SEND_BUFFER_SIZE		(128*1024)
#define	LLIF_CFG_RECV_BUFFER_SIZE		(128*1024)
#define LLIF_CFG_POLL_TIMEOUT_RX		(1)
#ifdef EDDS_CFG_SEND_BLOCKED
#define EDDS_CFG_INTERRUPT_TX_DIRECT
#endif /* EDDS_CFG_SEND_BLOCKED */

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* _PACKET32_USR_H_ */
