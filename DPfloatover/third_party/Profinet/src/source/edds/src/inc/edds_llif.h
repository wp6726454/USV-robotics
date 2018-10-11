
#ifndef EDDS_LLIF_H                    /* ----- reinclude-protection ----- */
#define EDDS_LLIF_H

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
/*  F i l e               &F: edds_llif.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile for EDDS. Lower Layer Interface (LLIF)               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  01.07.04    JS    changed mem-attribute of EDDS_LL_GET_MAC_ADDR          */
/*  03.02.05    JS    #ifndefs added.                                        */
/*  05.07.05    JS    added multiportsupport                                 */
/*  14.11.05    JS    EDDS_LL_SET_LINK_STATE. Parameter "PHYPower" added     */
/*  16.11.05    JS    EDDS_LL_GET_LINK_STATE. Parameter "pPortState" added   */
/*  06.03.06    JS    added switch-functions                                 */
/*  02.05.06    JS    added PortID to EDDS_LL_GET_MAC_ADDR                   */
/*  24.05.06    JS    added EDDS_LL_CHECK_LINK_STATE                         */
/*  26.09.06    JS    added pLinkSpeedModeCapability                         */
/*  23.01.07    JS    added EDDS_LL_GET_PORT_CNT                             */
/*  04.03.07    JS    added parameter MACAddrPrio                            */
/*  12.02.08    JS    added parameter "pIsWireless"                          */
/*  30.01.09    JS    added Mautype and Mediatype to  EDDS_LL_GET_LINK_STATE */
/*                    removed EDDS_LL_CHECK_LINK_STATE                       */
/*                    added   EDDS_LL_CHECK_MAUTYPE                          */
/*  26.07.10    MA    added  new edds_interrupt_poll API function            */
/*  12.02.08    TP    added arp set filter function                          */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                             some configs for LL                           */
/*===========================================================================*/

/*===========================================================================*/
/*                   Low-Level-Handle-Type                                   */
/*                                                                           */
/* The Handle Type EDDS_LL_HANDLE_TYPE has to be defined in a header of LLIF */
/*===========================================================================*/

/*===========================================================================*/
/*                   Low-Level ethernetcontroller interface                  */
/*                                                                           */
/* This will map the functions used in edds to hardware specific ones        */
/*                                                                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_OPEN_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement,
  EDDS_HANDLE				hDDB,
  EDDS_UPPER_DPB_PTR_TYPE pDPB,
  LSA_UINT32				TraceIdx);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SETUP_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement,
    EDDS_UPPER_DSB_PTR_TYPE pDSB);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SHUTDOWN_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_CLOSE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE  pBufferAddr,
    LSA_UINT32              Length,
    LSA_UINT32              PortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_STS_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_VOID    EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SEND_TRIGGER_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECV_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 							pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE  EDDS_LOCAL_MEM_ATTR *pBufferAddr,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                 pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                 pPortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECV_PROVIDE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    EDD_UPPER_MEM_PTR_TYPE          pBufferAddr);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_REINIT_TX_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_CAPS_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE	pCaps);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_STATS_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_LINK_STATE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    LSA_UINT32                          PortID,
    EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStat,
    EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortStatus,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pAutonegCapAdvertised,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkSpeedModeConfigured);

//@fixme am2219 ret val to LSA_VOID
typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_LINK_STATE_SETUP_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    LSA_UINT32                          PortID,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkStat,
    EDDS_LOCAL_MEM_U8_PTR_TYPE          pPHYPower,
    LSA_BOOL EDDS_LOCAL_MEM_ATTR     *  pIsWireless);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SET_LINK_STATE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    LSA_UINT8                       LinkStat,
    LSA_UINT8                       PHYPower);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_CHECK_MAUTYPE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    LSA_UINT16                      MAUType,
    LSA_UINT8                       PHYPower,
    EDDS_LOCAL_MEM_U8_PTR_TYPE      pLinkStat);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_MC_ENABLE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 						pLLManagement,
    EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR		*pMCAddr);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_MC_DISABLE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 						pLLManagement,
    LSA_BOOL                             	DisableAll,
    EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR 	*pMCAddr);


#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

typedef LSA_VOID  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RECURRING_TASK_PTR)(
    LSA_VOID_PTR_TYPE       pLLManagement);

#else /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_INTERRUPT_TX_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_INTERRUPT_RX_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_BOOL  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_INTERRUPT_POLL_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement,
  LSA_UINT32* 			IntSource);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_INTERRUPT_PHY_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_UINT32  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_INTERRUPT_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement,
    LSA_UINT32              IntSource);

#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

//@fixme am2219 ret val to LSA_VOID
typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_GET_MAC_ADDR_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 						pLLManagement,
    LSA_UINT32                           	PortID,
    EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR 	*pMCAddr);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_BACKUP_MODE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_RESTORE_MODE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_LED_SET_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 				pLLManagement,
    LSA_UINT32                      PortID,
    LSA_BOOL						On);

typedef LSA_VOID EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RX_OVERLOAD_RX_OFF_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_VOID EDDS_LOCAL_FCT_ATTR (*EDDS_LL_RX_OVERLOAD_RX_ON_FUNC_PTR)(
    LSA_VOID_PTR_TYPE 		pLLManagement);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_MULTICAST_FWD_CTRL_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    EDD_UPPER_PORTID_MODE_PTR_TYPE      pPortIDModeArray,
    LSA_UINT16                          PortIDModeCnt,
    LSA_UINT16                          MACAddrPrio,
    LSA_UINT16                          MACAddrGroup,
    LSA_UINT32                          MACAddrLow);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_SET_PORT_STATE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 						pLLManagement,
    EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE   pPortStates);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_FLUSH_FILTERING_DB_FUNC_PTR)(
LSA_VOID_PTR_TYPE 		                  pLLManagement,
EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE pFlushFilterDB);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_SET_PORT_REDUNDANT_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 							pLLManagement,
    EDDS_UPPER_SWI_SET_PORT_REDUNDANT_PTR_TYPE  pPortRed);

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_SET_BRIDGE_MODE_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    LSA_UINT32                          Mode);

#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SWITCH_DROP_CNT_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 					pLLManagement,
    EDDS_LOCAL_MEM_U32_PTR_TYPE         pDropCnt);
#endif /* LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT */

typedef LSA_RESULT EDDS_LOCAL_FCT_ATTR (*EDDS_LL_SET_ARP_FILTER_FUNC_PTR)(
    LSA_VOID_PTR_TYPE 		            pLLManagement,
    EDDS_ARP_FRAME_FILTER_PTR           pArpFilter);

typedef LSA_RESULT  EDDS_LOCAL_FCT_ATTR (*EDDS_LL_TIMEOUT_FUNC_PTR)(
  LSA_VOID_PTR_TYPE 		pLLManagement);

typedef struct _EDDS_LL_TABLE_TYPE
{
  /* EDDS init/setup/shutdown/close functions --> */
  EDDS_LL_OPEN_FUNC_PTR						    open;						/* low */
  EDDS_LL_SETUP_FUNC_PTR						setup;						/* low */
  EDDS_LL_SHUTDOWN_FUNC_PTR					    shutdown;					/* low */
  EDDS_LL_CLOSE_FUNC_PTR						close;						/* low */
  /* <-- EDDS init/setup/shutdown/close functions */
  /* Transmit buffer handling functions --> */
  EDDS_LL_SEND_FUNC_PTR						    enqueueSendBuffer;			/*HIGH!*/
  EDDS_LL_SEND_STS_FUNC_PTR					    getNextFinishedSendBuffer;	/*HIGH!*/
#ifdef EDDS_CFG_SEND_BLOCKED
  EDDS_LL_SEND_TRIGGER_FUNC_PTR				    triggerSend;				/*HIGH!*/
#endif
  EDDS_LL_REINIT_TX_FUNC_PTR					reinitTX;					/*HIGH!*/
  /* <-- Transmit buffer handling functions */
  /* Receive buffer handling functions --> */
  EDDS_LL_RECV_FUNC_PTR						    getNextReceivedBuffer;		/*!ISR!*/
  EDDS_LL_RECV_PROVIDE_FUNC_PTR				    provideReceiveBuffer;		/*!ISR!*/
  /* <-- Receive buffer handling functions */
  /* Information functions --> */
  EDDS_LL_GET_LINK_STATE_SETUP_FUNC_PTR		    getLinkStateSetup;			/* low */
  EDDS_LL_GET_LINK_STATE_FUNC_PTR				getLinkState;				/* low */
  EDDS_LL_CHECK_MAUTYPE_FUNC_PTR				checkMAU;					/* low */
  EDDS_LL_GET_STATS_FUNC_PTR					getStatistics;				/* low */
  EDDS_LL_GET_CAPS_FUNC_PTR					    getCapabilities;			/* low */
  /* <-- Information functions */
  /* MAC address management functions --> */
  EDDS_LL_GET_MAC_ADDR_FUNC_PTR				    getMACAddr;					/* low */
  EDDS_LL_MC_ENABLE_FUNC_PTR					enableMC;					/* low */
  EDDS_LL_MC_DISABLE_FUNC_PTR					disableMC;					/* low */
  /* <-- MAC address management functions */
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
  EDDS_LL_RECURRING_TASK_PTR                    recurringTask;              /* scheduler */
#else /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
  /* Interrupt functions --> */
  EDDS_LL_INTERRUPT_POLL_FUNC_PTR				getPendingInterrupt;		/*!ANY!*/
  EDDS_LL_INTERRUPT_RX_FUNC_PTR				    interruptRX;				/*!ISR!*/
  EDDS_LL_INTERRUPT_TX_FUNC_PTR				    interruptTX;				/*!ISR!*/
  EDDS_LL_INTERRUPT_PHY_FUNC_PTR				interruptPhy;				/*!ISR!*/
#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
  EDDS_LL_INTERRUPT_FUNC_PTR					interrupt;					/*!ISR!*/
#endif
  /* <-- Interrupt functions */
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
  /* Control functions --> */
  EDDS_LL_SET_LINK_STATE_FUNC_PTR				setLinkState;				/* low */
  EDDS_LL_LED_BACKUP_MODE_FUNC_PTR			    backupPortLEDMode;			/*!ANY!*/
  EDDS_LL_LED_RESTORE_MODE_FUNC_PTR			    restorePortLEDMode;			/*!ANY!*/
  EDDS_LL_LED_SET_FUNC_PTR					    setPortLED;					/*!ANY!*/
#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
  EDDS_LL_RX_OVERLOAD_RX_ON_FUNC_PTR			enableRX;					/* low */
  EDDS_LL_RX_OVERLOAD_RX_OFF_FUNC_PTR			disableRX;					/*!ISR!*/
#endif
  /* <-- Control functions */
  /* Switch functions --> */
#ifdef LLIF_CFG_SWITCH_SUPPORT
  EDDS_LL_SWITCH_SET_PORT_STATE_FUNC_PTR		setSwitchPortState;			/* low */
  EDDS_LL_SWITCH_MULTICAST_FWD_CTRL_FUNC_PTR	controlSwitchMulticastFwd;	/* low */
  EDDS_LL_SWITCH_FLUSH_FILTERING_DB_FUNC_PTR    flushSwitchFilteringDB;		/* low */
  EDDS_LL_SWITCH_SET_PORT_REDUNDANT_FUNC_PTR	setSwitchPortRedundant;		/* low */
  EDDS_LL_SWITCH_SET_BRIDGE_MODE_FUNC_PTR		setSwitchBridgeMode;		/* low */
#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
  EDDS_LL_SWITCH_DROP_CNT_FUNC_PTR			    getSwitchDroppedFrames;		/* low */
#endif
#endif
  /* <-- Switch functions */
  /* EDDS set arp filter function --> */
#ifdef LLIF_CFG_USE_LL_ARP_FILTER
  EDDS_LL_SET_ARP_FILTER_FUNC_PTR               setArpFilter;               /* low */
#endif
  /* <-- EDDS set arp filter function */
  EDDS_LL_TIMEOUT_FUNC_PTR					    timeout;					/* low */

} EDDS_LL_TABLE_TYPE;



#define EDDS_LL_FUNC(pDDB,func) (pDDB)->pGlob->pLLFunctionTable->func((pDDB)->pGlob->pLLManagementData



#define EDDS_LL_OPEN(pDDB,hDDB,pDPB,TraceIdx) \
    EDDS_LL_FUNC(pDDB,open),hDDB,pDPB,TraceIdx)

#define EDDS_LL_SETUP(pDDB,pDSB) \
    EDDS_LL_FUNC(pDDB,setup),pDSB)

#define EDDS_LL_SHUTDOWN(pDDB) \
    EDDS_LL_FUNC(pDDB,shutdown))

#define EDDS_LL_CLOSE(pDDB) \
    EDDS_LL_FUNC(pDDB,close))

#define EDDS_LL_SEND(pDDB,pBufferAddr,Length,PortID) \
    EDDS_LL_FUNC(pDDB,enqueueSendBuffer),pBufferAddr,Length,PortID)

#define EDDS_LL_SEND_STS(pDDB) \
    EDDS_LL_FUNC(pDDB,getNextFinishedSendBuffer))

#define EDDS_LL_SEND_TRIGGER(pDDB) \
    EDDS_LL_FUNC(pDDB,triggerSend))

#define EDDS_LL_RECV(pDDB,pBufferAddr,pLength,pPortID) \
    EDDS_LL_FUNC(pDDB,getNextReceivedBuffer),pBufferAddr,pLength,pPortID)

#define EDDS_LL_RECV_PROVIDE(pDDB,pBufferAddr) \
    EDDS_LL_FUNC(pDDB,provideReceiveBuffer),pBufferAddr)

#define EDDS_LL_REINIT_TX(pDDB) \
    EDDS_LL_FUNC(pDDB,reinitTX))

#define EDDS_LL_GET_CAPS(pDDB,pCaps) \
    EDDS_LL_FUNC(pDDB,getCapabilities),pCaps)

#define EDDS_LL_GET_STATS(pDDB,PortID,pStats) \
    EDDS_LL_FUNC(pDDB,getStatistics),PortID,pStats)

#define EDDS_LL_GET_LINK_STATE(pDDB,PortID,pLinkStat,pMAUType,pMediaType,pPortStatus,pAutonegCapAdvertised,pLinkSpeedModeConfigured) \
		EDDS_LL_FUNC(pDDB,getLinkState),PortID,pLinkStat,pMAUType,pMediaType,pPortStatus,pAutonegCapAdvertised,pLinkSpeedModeConfigured)

#define EDDS_LL_GET_LINK_STATE_SETUP(pDDB,PortID,pLinkStat,pPHYPower,pIsWireless) \
    EDDS_LL_FUNC(pDDB,getLinkStateSetup),PortID,pLinkStat,pPHYPower,pIsWireless)

#define EDDS_LL_SET_LINK_STATE(pDDB,PortID,LinkStat,PHYPower) \
    EDDS_LL_FUNC(pDDB,setLinkState),PortID,LinkStat,PHYPower)

#define EDDS_LL_CHECK_MAUTYPE(pDDB,PortID,MAUType,PHYPower,pLinkStat) \
    EDDS_LL_FUNC(pDDB,checkMAU),PortID,MAUType,PHYPower,pLinkStat)

#define EDDS_LL_MC_ENABLE(pDDB,pMCAddr) \
    EDDS_LL_FUNC(pDDB,enableMC),pMCAddr)

#define EDDS_LL_MC_DISABLE(pDDB,DisableAll,pMCAddr) \
    EDDS_LL_FUNC(pDDB,disableMC),DisableAll,pMCAddr)

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

#define EDDS_LL_RECURRING_TASK(pDDB) \
        EDDS_LL_FUNC(pDDB,recurringTask))

#else /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

#define EDDS_LL_INTERRUPT_TX(pDDB) \
    EDDS_LL_FUNC((EDDS_LOCAL_DDB_PTR_TYPE)pDDB,interruptTX))

#define EDDS_LL_INTERRUPT_RX(pDDB) \
    EDDS_LL_FUNC((EDDS_LOCAL_DDB_PTR_TYPE)pDDB,interruptRX))

#define EDDS_LL_INTERRUPT_POLL(pDDB,IntSource)	\
    EDDS_LL_FUNC(pDDB,getPendingInterrupt),IntSource)

#define EDDS_LL_INTERRUPT_PHY(pDDB) \
    EDDS_LL_FUNC((EDDS_LOCAL_DDB_PTR_TYPE)pDDB,interruptPhy))

#define EDDS_LL_INTERRUPT(pDDB,IntSource) \
    EDDS_LL_FUNC(pDDB,interrupt),IntSource)

#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

#define EDDS_LL_GET_MAC_ADDR(pDDB,PortID,pMCAddr) \
    EDDS_LL_FUNC(pDDB,getMACAddr),PortID,pMCAddr)

#define EDDS_LL_LED_BACKUP_MODE(pDDB,PortID) \
    EDDS_LL_FUNC(pDDB,backupPortLEDMode),PortID)

#define EDDS_LL_LED_RESTORE_MODE(pDDB,PortID) \
    EDDS_LL_FUNC(pDDB,restorePortLEDMode),PortID)

#define EDDS_LL_LED_SET_MODE(pDDB,PortID,On) \
    EDDS_LL_FUNC(pDDB,setPortLED),PortID,On)

#define EDDS_LL_RX_OVERLOAD_RX_OFF(pDDB) \
    EDDS_LL_FUNC(pDDB,disableRX))

#define EDDS_LL_RX_OVERLOAD_RX_ON(pDDB) \
    EDDS_LL_FUNC(pDDB,enableRX))

#define EDDS_LL_SWITCH_MULTICAST_FWD_CTRL(pDDB,pPortIDModeArray,PortIDModeCnt,MACAddrPrio,MACAddrGroup,MACAddrLow) \
    EDDS_LL_FUNC(pDDB,controlSwitchMulticastFwd),pPortIDModeArray,PortIDModeCnt,MACAddrPrio,MACAddrGroup,MACAddrLow)

#define EDDS_LL_SWITCH_SET_PORT_STATE(pDDB,pPortStates) \
    EDDS_LL_FUNC(pDDB,setSwitchPortState),pPortStates)

#define EDDS_LL_SWITCH_FLUSH_FILTERING_DB(pDDB,pFlushFilterDB) \
    EDDS_LL_FUNC(pDDB,flushSwitchFilteringDB),pFlushFilterDB)

#define EDDS_LL_SWITCH_SET_PORT_REDUNDANT(pDDB,pPortRed) \
    EDDS_LL_FUNC(pDDB,setSwitchPortRedundant),pPortRed)

#define EDDS_LL_SWITCH_SET_BRIDGE_MODE(pDDB,Mode) \
    EDDS_LL_FUNC(pDDB,setSwitchBridgeMode),Mode)

#define EDDS_LL_SWITCH_DROP_CNT(pDDB,pDropCnt) \
    EDDS_LL_FUNC(pDDB,getSwitchDroppedFrames),pDropCnt)

#define EDDS_LL_SET_ARP_FILTER(pDDB,pArpFilter) \
    EDDS_LL_FUNC(pDDB,setArpFilter),pArpFilter)

#define EDDS_LL_TIMEOUT(pDDB) \
    EDDS_LL_FUNC(pDDB,timeout))

#define EDDS_LL_AVAILABLE(pDDB,func) (LSA_NULL != ((pDDB)->pGlob->pLLFunctionTable->func))


/*****************************************************************************/
/*  end of file EDDS_LLIF.H                                                  */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_LLIF_H */
