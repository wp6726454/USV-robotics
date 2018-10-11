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
/*  F i l e               &F: packet32_edd.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level functions for EDDS (WinPCAP/Packet32)  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.07.10    MA    initial version.                                       */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  17
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID  /* Ids 16..31 reserved for LL */


/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */  //#JB_TODO lint warning cause this file is not used, but it sets EDDS_CFG_HW_PACKET32 (windows)
#ifdef EDDS_CFG_HW_PACKET32

/*===========================================================================*/
/*                              global Variables                             */
/*===========================================================================*/


#include "packet32_inc.h"

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*===========================================================================*/
/*                           static helper functions                         */
/*===========================================================================*/

//lint -esym(613,handle)
//JB 14/11/2014 helper function, handle checked where this function is called
//WARNING: be careful when using this function, make sure not to use handle as null ptr!
static LSA_RESULT packet32_set_power_state( PACKET32_LL_HANDLE_TYPE* handle,
                                            LSA_UINT8                PHYPower )
{
  LSA_RESULT Status = EDD_STS_OK;

  if(EDDS_PHY_POWER_UNCHANGED != PHYPower)
  {
    //  PnP and PM OIDs
    #define OID_PNP_CAPABILITIES             0xFD010100
    #define OID_PNP_SET_POWER                0xFD010101
    #define OID_PNP_QUERY_POWER              0xFD010102
    #define OID_PNP_ADD_WAKE_UP_PATTERN      0xFD010103
    #define OID_PNP_REMOVE_WAKE_UP_PATTERN   0xFD010104
    #define OID_PNP_WAKE_UP_PATTERN_LIST     0xFD010105
    #define OID_PNP_ENABLE_WAKE_UP           0xFD010106

    struct
    {
      PACKET_OID_DATA pkt;
      LSA_UINT8 _more_data[sizeof(DEVICE_POWER_STATE)];
    } oid_buf;

    oid_buf.pkt.Oid = OID_PNP_SET_POWER;
    oid_buf.pkt.Length = sizeof(oid_buf._more_data);
    *((DEVICE_POWER_STATE*)oid_buf.pkt.Data) = (DEVICE_POWER_STATE) (EDDS_PHY_POWER_ON == PHYPower ? NdisDeviceStateD0 : NdisDeviceStateD2)
		; //lint !e826 JB 14711/2014 pointer cast on purpose

    if (PacketRequest(handle->pAdapter, TRUE/*set*/, &oid_buf.pkt))
    {
      Status = EDD_STS_OK;
    }
    else
    {
      Status = EDD_STS_ERR_HW;
    }
  }

  return (Status);
}
//lint +esym(613,handle)

//lint -esym(613,handle,pResult)
//JB 14/11/2014 helper function, checked where called
//WARNING: be careful when using this function, make sure not to use handle or pResult as null ptr!
static NDIS_MEDIA_STATE EDDS_LOCAL_FCT_ATTR packet32_get_connect_status(
    PACKET32_LL_HANDLE_TYPE* handle,
    LSA_RESULT* pResult )
{
  NDIS_MEDIA_STATE media_state = NdisMediaStateDisconnected;

  struct
  {
    PACKET_OID_DATA pkt;
    LSA_UINT8 _more_data[sizeof(NDIS_MEDIA_STATE)];
  } oid_buf;

  oid_buf.pkt.Oid = OID_GEN_MEDIA_CONNECT_STATUS;
  oid_buf.pkt.Length = sizeof(oid_buf._more_data);

  if (PacketRequest(handle->pAdapter, FALSE/*get*/, &oid_buf.pkt))
  {
    media_state = *((NDIS_MEDIA_STATE*)oid_buf.pkt.Data);  //lint !e826 JB 14/11/2014 pointer cast on purpose
    *pResult = EDD_STS_OK;
  }
  else
  {
    *pResult = EDD_STS_ERR_HW;
  }

  return (media_state);
}
//lint +esym(613,handle,pResult)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_OPEN                            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_HANDLE                   hDDB          +*/
/*+                             EDDS_UPPER_DPB_PTR_TYPE       pDPB          +*/
/*+                             LSA_UINT32                    TraceIdx      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  hDDB         : Handle to Device Description Block (Device-management)  +*/
/*+  pDPB         : Pointer to Device Parameter Block                       +*/
/*+  TraceIdx     : Trace index to use with this EDDS LLIF instance         +*/
/*+                                                                         +*/
/*+  Result       :         EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_ALIGN                               +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize LL Ethernetcontroller using DPB parameters     +*/
/*+                                                                         +*/
/*+               See DPB documentation.                                    +*/
/*+                                                                         +*/
/*+               global:                                                   +*/
/*+                                                                         +*/
/*+               pDPB->RxBufferCnt     : number of rx-buffers/descriptors  +*/
/*+               pDPB->TxBufferCnt     : number of tx-buffers/descriptors  +*/
/*+               pDPB->RxBufferSize    : rx-buffers size                   +*/
/*+               pDPB->RxBufferOffset  : rx-buffer offset                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_OPEN(LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_HANDLE				hDDB,
		EDDS_UPPER_DPB_PTR_TYPE pDPB,
		LSA_UINT32				TraceIdx)
{
    LSA_VOID_PTR_TYPE pSendBuffer;
    LSA_VOID_PTR_TYPE pRecvBuffer;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :PACKET32_LL_OPEN()");

	handle->pAdapter = PacketOpenAdapter(pDPB->pLLParameter);
	handle->hDDB     = hDDB;
	handle->hSysDev  = pDPB->hSysDev;

	handle->TraceIdx = TraceIdx;

	handle->RxDropCnt = 0;

	if(EDD_FRAME_BUFFER_LENGTH < (EDD_IEEE_FRAME_LENGTH_WITH_CRC+EDD_VLAN_TAG_SIZE)) //lint !e506 !e774 JB 14/11/2014 define-based behaviour
	{
	  return EDD_STS_ERR_ABORT;
	}

	if ((0 == handle->pAdapter) || (INVALID_HANDLE_VALUE == handle->pAdapter->hFile))
	{
		EDDS_LL_TRACE_00(handle->TraceIdx, LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_OPEN: cannot open adapter");
		return EDD_STS_ERR_HW;
	}

    /* allocate send packet structure */
    /**@todo avoid copy frames from pSendBuffer to edds tx buffer */
    EDDS_ALLOC_LOCAL_FAST_MEM((&pSendBuffer),LLIF_CFG_SEND_BUFFER_SIZE);

    if (0 != pSendBuffer)
    {
        EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&handle->pSendBuffer,EDD_FRAME_BUFFER_LENGTH);

        if((0 != handle->pSendBuffer) && (0 != (handle->pSendPacketStruct = PacketAllocatePacket())))
        {
            PacketInitPacket(handle->pSendPacketStruct,pSendBuffer,LLIF_CFG_SEND_BUFFER_SIZE);
            handle->pSendPacket = 0;
        }else
        {
            LSA_UINT16 retval;
            PacketFreePacket(handle->pSendPacketStruct);
            EDDS_FREE_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pSendBuffer);
            handle->pSendBuffer = 0;
            EDDS_FREE_LOCAL_FAST_MEM(&retval,pSendBuffer);
            pSendBuffer = 0;
        }
    }
    if(0 == pSendBuffer)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_SETUP: cannot allocate send packet structure.");
        return EDD_STS_ERR_RESOURCE;
    }

    /* allocate receive packet structure */
    /**@todo avoid copy frames from pRecvBuffer to edds tx buffer */
    EDDS_ALLOC_LOCAL_FAST_MEM((&pRecvBuffer),LLIF_CFG_RECV_BUFFER_SIZE);

    if (0 != pRecvBuffer)
    {
        EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&handle->pRecvBuffer,EDD_FRAME_BUFFER_LENGTH);

        if((0 != handle->pRecvBuffer) && (0 != (handle->pRecvPacketStruct = PacketAllocatePacket())))
        {
            PacketInitPacket(handle->pRecvPacketStruct,pRecvBuffer,LLIF_CFG_RECV_BUFFER_SIZE);
            handle->pRecvPacket = 0;
        }else
        {
            LSA_UINT16 retval;
            PacketFreePacket(handle->pRecvPacketStruct);
            EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pRecvBuffer);
            handle->pRecvBuffer = 0;
            EDDS_FREE_LOCAL_FAST_MEM(&retval,pRecvBuffer);
            pRecvBuffer = 0;
        }
    }
    if(0 == pRecvBuffer)
    {
        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                "PACKET32_LL_SETUP: cannot allocate receive packet structure");
        return EDD_STS_ERR_RESOURCE;
    }

    handle->pendingInterrupts = 0;
    handle->linkState         = NdisMediaStateDisconnected;
    handle->sendPackets       = 0;

	EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT:PACKET32_LL_OPEN()");

	return (EDD_STS_OK);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SETUP                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_DSB_PTR_TYPE         pDSB        +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pDSB       : Pointer to Device Setup Block                             +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                         EDD_STS_ERR_HW                                  +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup LL Ethernetcontroller and start it.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SETUP(	LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_DSB_PTR_TYPE pDSB)

{
    LSA_UINT idx;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(pDSB);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SETUP()");

	if (FALSE == PacketSetLoopbackBehavior(handle->pAdapter, NPF_DISABLE_LOOPBACK))
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_SETUP: cannot disable loopback capture.");
		return EDD_STS_ERR_HW;
	}

	if (FALSE == PacketSetHwFilter(handle->pAdapter, NDIS_PACKET_TYPE_PROMISCUOUS))
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_SETUP: cannot set promiscuous mode.");
		return EDD_STS_ERR_HW;
	}

	if (FALSE == PacketSetBuff(handle->pAdapter, LLIF_CFG_KERNEL_BUFFER_SIZE))
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_SETUP: cannot set internal adapter buffer.");
		return EDD_STS_ERR_RESOURCE;
	}

	/* do not block on read */
	if (FALSE == PacketSetReadTimeout(handle->pAdapter, INFINITE))  //lint !e569 JB 14/11/2014 max val
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_SETUP: cannot set read timeout to infinite.");
		return EDD_STS_ERR_HW;
	}

	/* each send packet is not repeated on wire */
	if (FALSE == PacketSetNumWrites(handle->pAdapter, 1))
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_ERROR,
				"PACKET32_LL_SETUP: cannot disable packet duplication.");
		return EDD_STS_ERR_HW;
	}

	/* interface mac address */
    {
        LSA_UINT32 index;

        struct
        {
            PACKET_OID_DATA pkt;
            unsigned char more[6];  //#JB_TODO check this commented out (test windows)
        } buf;

        buf.pkt.Oid = OID_802_3_CURRENT_ADDRESS;
        buf.pkt.Length = EDD_MAC_ADDR_SIZE;
        if (PacketRequest(handle->pAdapter, FALSE/*get*/, &buf.pkt)
                && buf.pkt.Length == EDD_MAC_ADDR_SIZE)
        {
            for(index=0;index<EDD_MAC_ADDR_SIZE;++index)
            {
                handle->MACAddress.MacAdr[index]  = buf.pkt.Data[index];  //lint !e661 !e662 JB 14/11/2014 intended behaviour
            }

            if( (0 == pDSB->MACAddress[0].MacAdr[0]) && (0 == pDSB->MACAddress[0].MacAdr[1]) && (0 == pDSB->MACAddress[0].MacAdr[2]) &&
                (0 == pDSB->MACAddress[0].MacAdr[3]) && (0 == pDSB->MACAddress[0].MacAdr[4]) && (0 == pDSB->MACAddress[0].MacAdr[5]) )
            {
                pDSB->MACAddress[0] = handle->MACAddress;
            }
        }
        else
        {
            return EDD_STS_ERR_HW;
        }
    }

    /* port mac addresses */
    for(idx = 1 ; idx <= EDD_CFG_MAX_PORT_CNT ; ++idx)
    {
        if( (0 != pDSB->MACAddress[idx].MacAdr[0]) || (0 != pDSB->MACAddress[idx].MacAdr[1]) || (0 != pDSB->MACAddress[idx].MacAdr[2]) ||
            (0 != pDSB->MACAddress[idx].MacAdr[3]) || (0 != pDSB->MACAddress[idx].MacAdr[4]) || (0 != pDSB->MACAddress[idx].MacAdr[5]) )
        {
            handle->MACAddressPort[idx-1] = pDSB->MACAddress[idx];
        }
        else
        {
            EDDS_MEMSET_UPPER(handle->MACAddressPort[idx-1].MacAdr,0,sizeof(handle->MACAddressPort[idx-1].MacAdr));
        }
    }

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
				"OUT:PACKET32_LL_SETUP()");

	return (EDD_STS_OK);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SHUTDOWN                        +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Shutdown LL Ethernetcontroller. Stops it                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SHUTDOWN(LSA_VOID_PTR_TYPE pLLManagement)
{
	PACKET32_LL_HANDLE_TYPE* handle;

	handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SHUTDOWN()");

	/* nothing todo */

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
				"OUT:PACKET32_LL_SHUTDOWN()");

	return (EDD_STS_OK);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_CLOSE                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes LL Ethernetcontroller. Stops it                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement)
{
    LSA_UINT16 retval = EDD_STS_OK;
    LSA_VOID_PTR_TYPE pSendBuffer;
    LSA_VOID_PTR_TYPE pRecvBuffer;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_CLOSE()");

    if(0 != handle->pRecvPacketStruct)
    {
        EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pRecvBuffer);
        pRecvBuffer = handle->pRecvPacketStruct->Buffer;
        EDDS_FREE_LOCAL_FAST_MEM(&retval, pRecvBuffer );
        PacketFreePacket(handle->pRecvPacketStruct);
        handle->pRecvPacketStruct = 0;
    }

    if(0 != handle->pSendPacketStruct)
    {
        EDDS_FREE_TX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,handle->pSendBuffer);
        pSendBuffer = handle->pSendPacketStruct->Buffer;
        EDDS_FREE_LOCAL_FAST_MEM(&retval, pSendBuffer);
        PacketFreePacket(handle->pSendPacketStruct);
        handle->pSendPacketStruct = 0;
    }

	PacketCloseAdapter(handle->pAdapter);
	handle->pAdapter = 0;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
				"OUT:PACKET32_LL_CLOSE()");

	return (EDD_STS_OK);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    PACKET32_LL_RECV                                    +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pLength       +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pPortID       +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to pointer to buffer with Frame-netto-data       +*/
/*+                received if EDD_STS_OK.                                  +*/
/*+  pLength    :  Pointer to address with length of frame received.        +*/
/*+  pPortID    :  Pointer to address with PortID of frame received.        +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                EDD_STS_OK_NO_DATA                                       +*/
/*+                EDD_STS_ERR_RX                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions checks if a frame was received. If not it  +*/
/*+               returns EDD_STS_OK_NO_DATA. If a frame was received, the  +*/
/*+               functions returns pBufferAddr and pLength and sets        +*/
/*+               EDD_STS_OK or EDD_STS_ERR_RX. The buffer returned must    +*/
/*+               not be accessed anymore by the LLIF! The EDDS calls       +*/
/*+               PACKET32_LL_RECV_PROVIDE() to provide a new receive buffer+*/
/*+               to the LL. EDD_STS_ERR_RX is set, if the frame has a      +*/
/*+               error (e.g. wrong CRC). The EDDS discards this frame      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+ Use of this function within EDDS:                                       +*/
/*+                                                                         +*/
/*+                    Receive Interrupt                                    +*/
/*+                           |                                             +*/
/*+                           |                                             +*/
/*+       +------------------>+                                             +*/
/*+       |                   |                                             +*/
/*+       |                PACKET32_LL_RECV()                               +*/
/*+       |           +-------+------------+                                +*/
/*+       |           |                    |                                +*/
/*+       |     Frame present            No Frame                           +*/
/*+       |     (good or bad)              |                                +*/
/*+       |           |                    |                                +*/
/*+       |   handle buffer if good        |                                +*/
/*+       |           |                    |                                +*/
/*+       |    PACKET32_LL_RECV_PROVIDE()      |                            +*/
/*+       |           |                    |                                +*/
/*+       +-----------+                    |                                +*/
/*+                                       End (wait for receive interrupt)  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pBufferAddr,pLength,pPortID)
//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV(	LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID)
{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;
	LSA_RESULT Status;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_RECV(pBufferAddr: 0x%X, pLength: 0x%X)",
			pBufferAddr,
			pLength);


	*pPortID = 1; /* only one Port supported */

	if(0 == handle->pRecvPacket)
	{
		switch(WaitForSingleObject(PacketGetReadEvent(handle->pAdapter), 0))
		{
		case WAIT_OBJECT_0:  //lint !e835 JB 14/11/2014  define based (declared within WinBase.h)
			Status = EDD_STS_OK;
			break;
		case WAIT_TIMEOUT:
			Status = EDD_STS_OK_NO_DATA;
			break;
		default:
			Status = EDD_STS_ERR_HW;
			break;
		}

		if(EDD_STS_OK == Status)
		{
			if(FALSE == PacketReceivePacket(handle->pAdapter,handle->pRecvPacketStruct,TRUE))
			{
				Status = EDD_STS_ERR_HW;
			}else
			{
				// move to first bpf_hdr in receive buffer
				handle->pRecvPacket = (struct bpf_hdr*)handle->pRecvPacketStruct->Buffer;

				Status = EDD_STS_OK;
			}
		}
	}else
	{
		Status = EDD_STS_OK;
	}

	if(EDD_STS_OK == Status)
	{
		LSA_UINT	off/*=0*/;
		LSA_UINT8*	buf = (LSA_UINT8*)handle->pRecvPacket;

		// get address of frame data
		off = handle->pRecvPacket->bh_hdrlen;
		*pBufferAddr = handle->pRecvBuffer;
		if(0 != *pBufferAddr)
		{
			// length of frame data
			*pLength = handle->pRecvPacket->bh_caplen;

      if( EDD_FRAME_BUFFER_LENGTH < *pLength )
      {
        handle->RxDropCnt++;
        EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_WARN,
          "PACKET32_LL_RECV(): frame with len=%u dropped (overall=%u).",
          *pLength, handle->RxDropCnt);
        Status = EDD_STS_OK_NO_DATA;
      }
      else
      {
        EDDS_MEMCOPY_UPPER(*pBufferAddr,&buf[off],*pLength);
        handle->pRecvBuffer = 0;
      }

			// move to next bpf_hdr in receive buffer
			off = Packet_WORDALIGN(off+handle->pRecvPacket->bh_caplen);
			handle->pRecvPacket = (struct bpf_hdr*)&buf[off];  //lint !e826 JB 14/11/2014 pointer cast on purpose

			// more frames in buffer ?
			if( handle->pRecvPacketStruct->ulBytesReceived <= (DWORD)(((LSA_UINT8*)handle->pRecvPacket) - (LSA_UINT8*)handle->pRecvPacketStruct->Buffer))
			{
				handle->pRecvPacket = 0;
			}
		}else
		{
			Status = EDD_STS_ERR_RESOURCE;
		}
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_RECV(), Status: 0x%X",
			Status);

	return (Status);

}
//lint +esym(613,pBufferAddr,pLength,pPortID)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    PACKET32_LL_RECV_PROVIDE                            +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE                pBufferAddr   +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to receive buffer. The receive buffer is         +*/
/*+                EDD_FRAME_BUFFER_LENGTH in size.                         +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions provides a new receive buffer to the LL.   +*/
/*+               This function is called after the LL returned a buffer    +*/
/*+               with PACKET32_LL_RECV() to the EDDS.                      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+ Use of this function within EDDS:                                       +*/
/*+                                                                         +*/
/*+                    Receive Interrupt                                    +*/
/*+                           |                                             +*/
/*+                           |                                             +*/
/*+       +------------------>+                                             +*/
/*+       |                   |                                             +*/
/*+       |                PACKET32_LL_RECV()                               +*/
/*+       |           +-------+------------+                                +*/
/*+       |           |                    |                                +*/
/*+       |     Frame present            No Frame                           +*/
/*+       |     (good or bad)              |                                +*/
/*+       |           |                    |                                +*/
/*+       |   handle buffer if good        |                                +*/
/*+       |           |                    |                                +*/
/*+       |    PACKET32_LL_RECV_PROVIDE()      |                            +*/
/*+       |           |                    |                                +*/
/*+       +-----------+                    |                                +*/
/*+                                       End (wait for receive interrupt)  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_RECV_PROVIDE(
		LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr)
{
	LSA_RESULT Status;

	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_RECV_PROVIDE(pBufferAddr: 0x%X)",
			pBufferAddr);

	handle->pRecvBuffer = pBufferAddr;

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_RECV_PROVIDE(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND                            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_MEM_PTR pBufferAddr              +*/
/*+                             LSA_UINT32        Length                    +*/
/*+                             LSA_UINT32        PortID                    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to buffer with Frame-netto-data to send          +*/
/*+                This address will be used and put to the controller!     +*/
/*+                (The data is not copied!) So the buffer must not be      +*/
/*+                accessed until it is released!                           +*/
/*+  Length     :  Number of Bytes pBuffer points to  (max. 1514/1518 Byte) +*/
/*+  PortID     :  PortID to send                                           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start sending a Frame. The frame (pBufferAddr) is given   +*/
/*+               to  the low-level function for further handling. Length   +*/
/*+               is the length of the frame. The frame has to include      +*/
/*+               mac-addresses, type/len, and data. FCS is added be low-   +*/
/*+               level function (normally be controller)                   +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE pBufferAddr, LSA_UINT32 Length,
		LSA_UINT32 PortID)

{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;
	LPPACKET lpPacket;
	LSA_RESULT Status;

	LSA_UNUSED_ARG(PortID);

//#ifndef EDDS_CFG_SEND_BLOCKED
//	lpPacket = handle->pSendBuffer;  //happens anyways
//#else
//#endif

	lpPacket = handle->pSendBuffer;
	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SEND(pBufferAddr: 0x%X, Length: 0x%X, PortID: %d)",
			pBufferAddr,
			Length,
			PortID);

#ifndef EDDS_CFG_SEND_BLOCKED
	if (lpPacket == NULL)
	{
		Status = EDD_STS_ERR_RESOURCE;
	}
	else
	{
		PacketInitPacket(lpPacket,pBufferAddr,Length);
		if (FALSE == PacketSendPacket(handle->pAdapter, lpPacket,
				TRUE))
		{
			Status = EDD_STS_ERR_RESOURCE;
		}
		else
		{
			++handle->sendPackets;
			Status = EDD_STS_OK;
		}
	}

	/**@note For performance improvement,
	 * TX interrupt is handled directly in LL_SEND to avoid cycle drop */
#if 0
	handle->pendingInterrupts |= 1<<(EDDS_INTERRUPT_TX-1);
#else
	edds_interrupt(handle->hDDB,EDDS_INTERRUPT_TX);
#endif
#else
	if(0 == handle->pSendPacket)
	{
		/* get first packet */
		handle->pSendPacket = handle->pSendPacketStruct.Buffer;
		handle->pSendPacket.caplen	= Length;
		handle->pSendPacket.len		= Length;
		handle->pSendPacket.ts		= 0;
	}
#endif
	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SEND(), Status: 0x%X",
			Status);

	return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND_TRIGGER                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts transmission of frames applied by PACKET32_LL_SEND. +*/
/*+               Only called if EDDS_CFG_SEND_BLOCKED is defined (edd_cfg.h)+*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef EDDS_CFG_SEND_BLOCKED
LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_TRIGGER(LSA_VOID_PTR_TYPE pLLManagement)
{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SEND_TRIGGER(pDDB: 0x%X)",
			pDDB);

	LSA_UNUSED_ARG(pDDB);

#ifndef EDDS_CFG_INTERRUPT_TX_DIRECT
	handle->pendingInterrupts |= 1<<(EDDS_INTERRUPT_TX-1);
#else
	edds_interrupt(pDDB,EDDS_INTERRUPT_TX);
#endif

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SEND()");

}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SEND_STS                        +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK           : frame successul transm.  +*/
/*+                         EDD_STS_OK_NO_DATA   : no frame transmitted     +*/
/*+                         ... transmit errors                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for finished transmissions. If a transmission was  +*/
/*+               finished, EDD_STS_OK is returned. If no (more) finished   +*/
/*+               EDD_STS_OK_NO_DATA is returned.                           +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SEND_STS(LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SEND_STS()");

	if(0 != handle->sendPackets)
	{
		Status = EDD_STS_OK;
		--handle->sendPackets;
	}else
	{
		Status = EDD_STS_OK_NO_DATA;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SEND_STS(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_REINIT_TX                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT  :                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reinits Transmit-unit of ethernetcontroller. Called after +*/
/*+               a transmit-timeout occured                                +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_REINIT_TX(LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_REINIT_TX()");

	LSA_UNUSED_ARG(handle);
	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_REINIT_TX(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_GET_STATS                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+                             EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                           :                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_STATS(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 PortID, EDDS_LOCAL_STATISTICS_PTR_TYPE pStats)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_GET_STATS(PortID: 0x%X, pStats: 0x%X)",
			PortID,
			pStats);

	LSA_UNUSED_ARG(handle);
	if((1 < PortID)||(0==pStats))
	{
		/* PortID not expected */
		Status = EDD_STS_ERR_PARAM;
	}else
	{
/**@todo count send/receive bytes and other statistc data
		if(0!=PacketGetStatsEx(handle->pAdapter,&stats))
		{
			Status = EDD_STS_OK;
		}else
		{
			Status = EDD_STS_ERR_HW;
		}
*/
		pStats->InOctets = 0;
		pStats->InUcastPkts = 0;
		pStats->InNUcastPkts = 0;
		pStats->InDiscards = 0;
		pStats->InErrors = 0;
		pStats->InUnknownProtos = 0;
		pStats->OutOctets = 0;
		pStats->OutUcastPkts = 0;
		pStats->OutNUcastPkts = 0;
		pStats->OutDiscards = 0;
		pStats->OutErrors = 0;
		pStats->OutQLen = 0;

		Status = EDD_STS_OK;
	}
	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_GET_STATS(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :  PACKET32_LL_GET_CAPS                          +*/
/*+  Input/Output          :  LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                           EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE pCaps [out]  +*/
/*+  Result                :  LSA_RESULT                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                           :                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT  EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_CAPS(
	LSA_VOID_PTR_TYPE 					pLLManagement,
    EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE	pCaps)
{
	LSA_RESULT Status = EDD_STS_OK;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_GET_CAPS()");

	LSA_UNUSED_ARG(handle);
	pCaps->HWFeatures = 0;
	pCaps->SWFeatures = EDDS_LL_CAPS_SWF_SRT | EDDS_LL_CAPS_SWF_SEND_ASYNCHRONOUS;
	pCaps->PortCnt = 1;
	pCaps->MaxFrameSize = 1536;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_GET_CAPS(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      : PACKET32_LL_GET_LINK_STATE                         +*/
/*+  Input/Output      : LSA_VOID_PTR_TYPE                  pLLManagement   +*/
/*+                      EDD_UPPER_GET_LINK_STATUS_PTR_TYPE   pLinkStat,    +*/
/*+                      EDDS_LOCAL_MEM_U16_PTR_TYPE  pMAUType,             +*/
/*+                      EDDS_LOCAL_MEM_U8_PTR_TYPE   pMediaType,           +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pPortStatus,          +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pAutonegCapAdvertised)+*/
/*+                                                                         +*/
/*+  Result             :    LSA_RESULT                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to get link status from  (1..x)                      +*/
/*+  pLinkStat  : Pointer to LinkStatus to be filled                        +*/
/*+                                                                         +*/
/*+  pMAUType   : Pointer to address for actual MAUType                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0) if unknown. Else IEC MAUType     +*/
/*+                                                                         +*/
/*+  pMediaType : Pointer to address for actual MediaType                   +*/
/*+               EDD_MEDIATYPE_UNKNOWN (=0) if unknown. Else IEC MediaType +*/
/*+                                                                         +*/
/*+  pPortStatus: Pointer to address for actual PortStatus (see edd_usr.h)  +*/
/*+                                                                         +*/
/*+  pAutonegCapAdvertised:    Pointer to address for actual autoneg        +*/
/*+                            advertised capabilities.                     +*/
/*+                            Note: 0 if no autoneg supported.             +*/
/*+                            EDDS_AUTONEG_CAP_xxx see edd_usr.h           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+             :           EDD_STS_ERR_PARAM                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets LinkStatus from Low-Level-functions                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pMAUType,pMediaType,pPortStatus,pAutonegCapAdvertised)
//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE  pLinkSpeedModeConfigured)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_GET_LINK_STATE(PortID: %d)",
			PortID);

	pLinkStat->Status = (NdisMediaStateDisconnected == handle->linkState) ? EDD_LINK_DOWN : EDD_LINK_UP;
	pLinkStat->Mode   = EDD_LINK_UNKNOWN;
	pLinkStat->Speed  = EDD_LINK_UNKNOWN;

	*pMAUType              = EDD_MAUTYPE_UNKNOWN;
	*pMediaType            = EDD_MEDIATYPE_UNKNOWN;
	*pPortStatus           = EDD_PORT_NOT_PRESENT;
	*pAutonegCapAdvertised = 0; // when not supported or known set to 0

	Status = EDD_STS_OK;

	/* only one Port supported */
	if (PortID != 1)
	{
		/* PortID not expected */
		Status = EDD_STS_ERR_PARAM;
	}
	else
	{
		pLinkStat->PortID = (LSA_UINT16)PortID; // set port id

		*pLinkSpeedModeConfigured = EDD_LINK_AUTONEG;

		/* only when link is UP */
		if( EDD_LINK_UP == pLinkStat->Status )
		{
		  struct
		  {
		    PACKET_OID_DATA pkt;
		    LSA_UINT8 _more_data[sizeof(NDIS_CO_LINK_SPEED)];
		  } oid_buf;

		  /**@todo implement NDIS requests to get real capabilities */
		  oid_buf.pkt.Oid    = OID_GEN_LINK_SPEED;
		  oid_buf.pkt.Length = sizeof(oid_buf._more_data);

		  if( PacketRequest(handle->pAdapter, FALSE/*get*/, &oid_buf.pkt) )
		  {
		    NDIS_CO_LINK_SPEED* speed = (NDIS_CO_LINK_SPEED*)oid_buf.pkt.Data;  //lint !e826

		    *pMediaType            = EDD_MEDIATYPE_COPPER_CABLE;
		    *pPortStatus           = EDD_PORT_PRESENT;
		    *pAutonegCapAdvertised = EDD_AUTONEG_CAP_10BASET     |
                                 EDD_AUTONEG_CAP_10BASETFD   |
                                 EDD_AUTONEG_CAP_100BASETX   |
                                 EDD_AUTONEG_CAP_100BASETXFD |
                                 EDD_AUTONEG_CAP_1000BASETFD;

		    switch(speed->Outbound)
		    {
		      case 100000 :
		        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
		        pLinkStat->Speed = EDD_LINK_SPEED_10;
		        *pMAUType        = EDDS_LL_MAUTYPE_10BASETFD;
		        break;
		      case 1000000 :
		        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
		        pLinkStat->Speed = EDD_LINK_SPEED_100;
		        *pMAUType        = EDDS_LL_MAUTYPE_100BASETXFD;
		        break;
		      case 10000000 :
		        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
		        pLinkStat->Speed = EDD_LINK_SPEED_1000;
		        *pMAUType        = EDDS_LL_MAUTYPE_1000BASETFD;
		        break;
		      case 100000000 :
		        pLinkStat->Mode  = EDD_LINK_MODE_FULL;
		        pLinkStat->Speed = EDD_LINK_SPEED_10000;
		        *pMAUType        = EDDS_LL_MAUTYPE_10GBASET;
		        break;
			  default:
				  Status = EDD_STS_ERR_RESOURCE;
		    }
		    //Status = EDD_STS_OK;
		  }
		  else
		  {
		    Status = EDD_STS_ERR_HW;
		  }
		}
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_GET_LINK_STATE(), Status: 0x%X",
			Status);

	return (Status);

}
//lint +esym(613,pMAUType,pMediaType,pPortStatus,pAutonegCapAdvertised)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SET_LINK_STATE                  +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             LSA_UINT8                           LinkStat+*/
/*+                             LSA_UINT8                           PHYPower+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  LinkStat   : LinkStatus to be setup                                    +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+               EDD_LINK_UNCHANGED     (do not change LinkStat)           +*/
/*+  PHYPower   : Power of PHY                                              +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not change power)             +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Link to given values (only if hardware is setup)      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 LinkStat,
		LSA_UINT8 PHYPower)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(PortID);
	LSA_UNUSED_ARG(LinkStat);

	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X)",
			PortID,
			LinkStat,
			PHYPower);

	// set phy power (do nothing for EDDS_PHY_POWER_UNCHANGED)
	Status = packet32_set_power_state(handle, PHYPower);

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SET_LINK_STATE(), Status: 0x%X",
			Status);

	return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_CHECK_MAUTYPE                   +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE             pDDB    +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             LSA_UINT16                          MAUType +*/
/*+                             LSA_UINT8                           PHYPower+*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pLinkStat+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to Device Description Block (Device-management)   +*/
/*+  PortID     : Port to check LinkStat for (1..EDDS_MAX_PORT_CNT)         +*/
/*+  MAUType    : MAUType to be checked                                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0)                                  +*/
/*+               else IEEE MAUType (1..x)                                  +*/
/*+  PHYPower   : Power of PHY to be checked                                +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not check)                    +*/
/*+  pLinkStat  : LinkState  setup                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check if the MAUType/PHYPower can be set for Port         +*/
/*+                                                                         +*/
/*+               If a MAUType is present (<> EDD_MAUTYPE_UNKNOWN) check if +*/
/*+               it is supported and return pLinkStat with Speed/Mode for  +*/
/*+               MAUType.                                                  +*/
/*+                                                                         +*/
/*+               If PHYPower is <> EDDS_PHY_POWER_UNCHANGED check if PHY   +*/
/*+               power is supported.                                       +*/
/*+                                                                         +*/
/*+               pDDB->pGlob->LLHandle contains LLIF device structure!     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_CHECK_MAUTYPE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT16 MAUType,
		LSA_UINT8 PHYPower, EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_CHECK_MAUTYPE(PortID: %d, MAUType: %d, Power: 0x%X)",
			PortID,
			MAUType,
			PHYPower);

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(MAUType);
	LSA_UNUSED_ARG(PHYPower);
	LSA_UNUSED_ARG(PortID);
	LSA_UNUSED_ARG(pLinkStat);

	/**@todo implement with NDIS requests */

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_CHECK_MAUTYPE(), Status: 0x%X",
			Status);

	return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_GET_LINK_STATE_SETUP            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pLinkStat+*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pPHYPower+*/
/*+                             LSA_BOOL EDDS_LOCAL_MEM_ATTR *     pIsWirele+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  pLinkStat  : LinkStatus setup                                          +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+  pPHYPower  : PHY Power setup                                           +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+  pIsWireless: Is this port a wireless port?                             +*/
/*+               LSA_FALSE  : no                                           +*/
/*+               LSA_TRUE   : yes                                          +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the actual Setup of LinkStat and PHYPower           +*/
/*+               (Note: This are the Setup-values!)                        +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pLinkStat,pPHYPower,pIsWireless)
//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_LINK_STATE_SETUP(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pPHYPower,
		LSA_BOOL EDDS_LOCAL_MEM_ATTR * pIsWireless)
{

	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_GET_LINK_STATE_SETUP()");

	/**@todo implement with NDIS requests */
	/* depends on setup and changes with PACKET32_LL_SET_LINK_STATE! */
	LSA_UNUSED_ARG(handle);
	*pLinkStat = EDD_LINK_AUTONEG;
	*pPHYPower = EDDS_PHY_POWER_ON;
	*pIsWireless = LSA_FALSE;

	LSA_UNUSED_ARG(PortID);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
				"OUT:PACKET32_LL_GET_LINK_STATE_SETUP()");

	return (EDD_STS_OK);

}
//lint +esym(613,pLinkStat,pPHYPower,pIsWireless)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_MC_ENABLE                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Enable              +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE      : no more free MC entrys        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Enables Multicastaddress                                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_ENABLE(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_MC_ENABLE(pMCAddr: 0x%X)",
			pMCAddr);

	LSA_UNUSED_ARG(pMCAddr);

	LSA_UNUSED_ARG(handle);
	/**@todo implement with Packet32 filters */
	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_MC_ENABLE(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_MC_DISABLE                      +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                        DisableAll  +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  DisableAll  :LSA_TRUE: frees all Multicast addresses. pMCAddr          +*/
/*+                         not used.                                       +*/
/*+               LSA_FALSE:free pMACAddr only                              +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Disable if          +*/
/*+               DisableAll = LSA_FALSE                                    +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Disables Multicastaddress                                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
		LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_MC_DISABLE(DisableAll: 0x%X, pMCAddr: 0x%X)",
			DisableAll,
			pMCAddr);

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(DisableAll);
	LSA_UNUSED_ARG(pMCAddr);

	/**@todo implement with Packet32 filters */
	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_MC_DISABLE(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   PACKET32_LL_GET_MAC_ADDR                     +*/
/*+  Input/Output          :   LSA_VOID_PTR_TYPE             pLLManagement  +*/
/*+                            LSA_UINT32                       PortID      +*/
/*+                            EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr+*/
/*+  Result                :   LSA_RESULT                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : 0    : MAC Address from Station (Interface)               +*/
/*+               1..EDDS_MAX_PORT_CNT : Port MAC Address                   +*/
/*+  pMCAddr    : Pointer to 6 Byte MAC                                     +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets station or port MAC-address of Ethernetcontroller    +*/
/*+               If MAC address is 0, LLIF should get a valid MAC from     +*/
/*+               e.g. EEPROM in ethernet controller or generate an error   +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pMACAddr)
//JB 17/11/2014 will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_GET_MAC_ADDR(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_GET_MAC_ADDR(PortID: %d, pMACAddr: 0x%X)",
			PortID,
			pMACAddr);

	Status = EDD_STS_OK;

	/* Station MAC == Port MAC (we have only one Port!) */
	if(1<PortID)
	{
		/* PortID not expected */
		Status = EDD_STS_ERR_PARAM;
	}else
	{
        if(0 == PortID)
        {
            *pMACAddr = handle->MACAddress;
        }
        else
        {
            *pMACAddr = handle->MACAddressPort[PortID-1];
        }
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_GET_MAC_ADDR(), Status: 0x%X",
			Status);

	return (Status);

}
//lint +esym(613,pMACAddr)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_BACKUP_MODE                 +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Backup LED-Mode for PHY LEDs                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_BACKUP_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_LED_BACKUP_MODE(PortID: %d)",
			PortID);

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(PortID);

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_LED_BACKUP_MODE(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_RESTORE_MODE                +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: RESTORE LED-Mode for PHY LEDs                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_RESTORE_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_LED_RESTORE_MODE(PortID: %d)",
			PortID);

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(PortID);

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_LED_RESTORE_MODE(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_LED_SET_MODE                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+                             LSA_BOOL                        LEDOn       +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+  LEDOn      : Mode of LED to set (LSA_TRUE/LSA_FALSE)                   +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets LED-Mode for PHY LEDs                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_LED_SET_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_BOOL LEDOn)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_LED_SET_MODE(PortID: %d = %d)",
			PortID,
			LEDOn);

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(LEDOn);
	LSA_UNUSED_ARG(PortID);

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_LED_SET_MODE(), Status: 0x%X",
			Status);

	return (Status);

}


#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_RX_OVERLOAD_RX_ON               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is used if Rx-Overload check is enabled     +*/
/*+               within EDDS.                                              +*/
/*+               After the receiver was turned off with                    +*/
/*+               PACKET32_LL_RX_OVERLOAD_CTRL_OFF it will be turned on     +*/
/*+               after an idle time with this function.                    +*/
/*+                                                                         +*/
/*+               This function is called within EDDS low prio context.     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RX_OVERLOAD_RX_ON(
		LSA_VOID_PTR_TYPE pLLManagement)
{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_RX_OVERLOAD_RX_ON()");

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_RX_OVERLOAD_RX_ON()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_RX_OVERLOAD_RX_OFF              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is used if Rx-Overload check is enabled     +*/
/*+               within EDDS.                                              +*/
/*+               If the the EDDS detects an Rx-Overload, it calls this     +*/
/*+               function to turn the receiver OFF.                        +*/
/*+                                                                         +*/
/*+               This function is called within EDDS edds_interrupt()      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR PACKET32_LL_RX_OVERLOAD_RX_OFF(
		LSA_VOID_PTR_TYPE pLLManagement)
{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_RX_OVERLOAD_RX_OFF()");

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_RX_OVERLOAD_RX_OFF()");

}
#endif


#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_INTERRUPT                        +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                       IntSource  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  IntSource  : Interrupt Source                                          +*/
/*+                                                                         +*/
/*+               EDDS_INTERRUPT_TX                                         +*/
/*+               EDDS_INTERRUPT_RX                                         +*/
/*+               EDDS_INTERRUPT_PHY                                        +*/
/*+               others (see edd_usr.h)                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Return       EDDS_INTERRUPT_TX                                         +*/
/*+               EDDS_INTERRUPT_RX                                         +*/
/*+               EDDS_INTERRUPT_PHY                                        +*/
/*+               EDDS_INTERRUPT_NONE                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called by EDDS on edds_interrupt() entry. +*/
/*+               The LL can handle specific interrupts not supported by    +*/
/*+               EDDS.                                                     +*/
/*+                                                                         +*/
/*+               The EDDS itself handles TX,RX and PHY interrupts.         +*/
/*+               The LL has to return these interrupt sources. All other   +*/
/*+               interrupts have to be handled here (return NONE)          +*/
/*+                                                                         +*/
/*+               After this the EDDS calls                                 +*/
/*+                                                                         +*/
/*+               PACKET32_LL_INTERRUPT_TX()  if IntSource is EDDS_INTERRUPT_TX +*/
/*+               PACKET32_LL_INTERRUPT_RX()  if IntSource is EDDS_INTERRUPT_RX +*/
/*+               PACKET32_LL_INTERRUPT_PHY() if IntSource is EDDS_INTERRUPT_PHY+*/
/*+                                                                         +*/
/*+               Note: If the LL needs no special handling it can define   +*/
/*+                     LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT32 EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 IntSource)
{
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_INTERRUPT(IntSource: 0x%X)",
			IntSource);

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_INTERRUPT(), IntSource: 0x%X",
			IntSource);

	return(IntSource);
}
#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_INTERRUPT_TX                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a TX-Interrupt occured (EDDS_INTERRUPT_TX).            +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_TX(
		LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_INTERRUPT_TX()");

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_INTERRUPT_TX(), Status: 0x%X",
			Status);
	handle->pendingInterrupts &= ~(1<<(EDDS_INTERRUPT_TX-1));  //lint !e778 !e845 JB 14/11/2014 define-based + readability

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_INTERRUPT_RX                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a RX-Interrupt occured (EDDS_INTERRUPT_RX).            +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_RX(
		LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_INTERRUPT_RX()");

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_INTERRUPT_RX(), Status: 0x%X",
			Status);

	handle->pendingInterrupts &= ~(1<<(EDDS_INTERRUPT_RX-1));

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_INTERRUPT_POLL                  +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       LSA_TRUE     if interrupt more Interrupts are pending     +*/
/*+               LSA_FALSE    if interrupt no other Interrupts are pending +*/
/*+               IntSource [OUT]  (highest) pending interrupt 			    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions returns the highest pending  interrupt     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,IntSource)
//JB 17/11/2014 will be set within this function
LSA_BOOL EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_POLL(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32* IntSource)
{
	DWORD eventNumber;
	LSA_UINT32	eddsinterrupt;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_INTERRUPT_POLL(Source: %d)",IntSource);

	eddsinterrupt = EDDS_INTERRUPT_NONE;

	/* first, check RX interrupt */
	if(handle->pendingInterrupts & (1<<(EDDS_INTERRUPT_RX-1)))
	{
		eddsinterrupt = EDDS_INTERRUPT_RX;
	}else
	{
		/* check for EDDS_INTERRUPT_RX */
		switch((eventNumber = WaitForSingleObject(PacketGetReadEvent(handle->pAdapter), LLIF_CFG_POLL_TIMEOUT_RX)))
		{
		case WAIT_OBJECT_0: //lint !e835 JB 14/11/2014  define-based (declared within WinBase.h)
			eddsinterrupt = EDDS_INTERRUPT_RX;
			handle->pendingInterrupts = (1<<(EDDS_INTERRUPT_RX-1));
			break;
		case WAIT_TIMEOUT:
			eddsinterrupt = EDDS_INTERRUPT_NONE;
			break;
		default:
			EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_FATAL,
						"OUT:PACKET32_LL_INTERRUPT_POLL(), unknown WaitEvent: 0x%X",
						eventNumber);
			break;
		}
	}

	if(EDDS_INTERRUPT_NONE == eddsinterrupt)
	{
/**@note For performance improvement,
 * TX interrupt is handled directly in LL_SEND to avoid cycle drop */
#if 0
		if(handle->pendingInterrupts & (1<<(EDDS_INTERRUPT_TX-1)))
		{
			eddsinterrupt = EDDS_INTERRUPT_TX;
		}else
#endif
		if(handle->pendingInterrupts & (1<<(EDDS_INTERRUPT_PHY-1)))
		{
			eddsinterrupt = EDDS_INTERRUPT_PHY;
			handle->pendingInterrupts = (1<<(EDDS_INTERRUPT_PHY-1));
		}
	}

	if(EDDS_INTERRUPT_NONE == eddsinterrupt)
	{
	  LSA_RESULT result;
	  NDIS_MEDIA_STATE State = packet32_get_connect_status(handle, &result);

		if( EDD_STS_OK == result )
		{
			if(handle->linkState != State)
			{
				// state is set in interrupt routine (PACKET32_LL_INTERRUPT_PHY)
				eddsinterrupt = EDDS_INTERRUPT_PHY;
				handle->pendingInterrupts = (1<<(EDDS_INTERRUPT_PHY-1));
			}
		}
		else
		{
			EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_FATAL,
						"PACKET32_LL_INTERRUPT_POLL(), get PHY state failed.");
		}
	}

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
				"OUT:PACKET32_LL_INTERRUPT_POLL(IntSource = %d) : pendingInterrupts = %d ",eddsinterrupt,handle->pendingInterrupts);

	*IntSource = eddsinterrupt;
	return (0 != handle->pendingInterrupts);
}
//lint +esym(613,IntSource)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_INTERRUPT_PHY                   +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a PHY-Interrupt occured (EDDS_INTERRUPT_PHY).          +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*LSA_EXTERN*/ LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_INTERRUPT_PHY(
		LSA_VOID_PTR_TYPE pLLManagement)
{
  NDIS_MEDIA_STATE media_state;
  LSA_RESULT Status;
  PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_INTERRUPT_PHY()");

	media_state = packet32_get_connect_status(handle, &Status);

	if( EDD_STS_OK == Status )
		handle->linkState = media_state;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_INTERRUPT_PHY(), Status: 0x%X",
			Status);

	handle->pendingInterrupts &= ~(1<<(EDDS_INTERRUPT_PHY-1));

	return (Status);
}


#ifdef LLIF_CFG_SWITCH_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL              +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+                      EDD_UPPER_PORTID_MODE_PTR_TYPE  pPortIDModeArray   +*/
/*+                      LSA_UINT16                      PortIDModeCnt      +*/
/*+                      LSA_UINT16                      MACAddrPrio        +*/
/*+                      LSA_UINT16                      MACAddrGroup       +*/
/*+                      LSA_UINT16                      MACAddrLow         +*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pPortIDModeArray   :   Pointer to array with Port ids/modes            +*/
/*+  PortIDModeCnt      :   Number of arrayelements within pPortIDModeArr.  +*/
/*+  MACAddrPrio        :   MAC Address priority                            +*/
/*+  MACAddrGroup       :   MAC Address group                               +*/
/*+  MACAddrLow         :   MAC Address low bits. Depends on Group          +*/
/*+                                                                         +*/
/*+  Details to parameters see edds-service EDD_SRV_MULTICAST_FWD_CTRL      +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Controls forwarding of multicastadresses.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_PORTID_MODE_PTR_TYPE pPortIDModeArray,
		LSA_UINT16 PortIDModeCnt,
		LSA_UINT16 MACAddrPrio,
		LSA_UINT16 MACAddrGroup,
		LSA_UINT32 MACAddrLow)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);

	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL(PortIDModeCnt: 0%d, MACAddrPrio: %d, MACAddrGroup: 0x%X)",
			PortIDModeCnt,
			MACAddrPrio,
			MACAddrGroup);

	LSA_UNUSED_ARG(pPortIDModeArray);
	LSA_UNUSED_ARG(PortIDModeCnt);
	LSA_UNUSED_ARG(MACAddrPrio);
	LSA_UNUSED_ARG(MACAddrGroup);
	LSA_UNUSED_ARG(MACAddrLow);

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_MULTICAST_FWD_CTRL(), Status: 0x%X",
			Status);

	return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_SET_PORT_STATE                  +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+                      EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE  pPortStates +*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pPortStates   : Pointer to array with Port ids/modes                   +*/
/*+                                                                         +*/
/*+  possible PortStates:                                                   +*/
/*+                                                                         +*/
/*+         EDD_PORT_STATE_DISABLE                                          +*/
/*+         EDD_PORT_STATE_BLOCKING                                         +*/
/*+         EDD_PORT_STATE_LISTENING                                        +*/
/*+         EDD_PORT_STATE_LEARNING                                         +*/
/*+         EDD_PORT_STATE_FORWARDING                                       +*/
/*+         EDD_PORT_STATE_UNCHANGED                                        +*/
/*+                                                                         +*/
/*+  Details to parameters see edds-service EDD_SRV_SWITCH_SET_PORT_STATE   +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Controls port mode for switching                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_PORT_STATE(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_SWI_SET_PORT_STATE_PTR_TYPE pPortStates)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_SET_PORT_STATE(pPortStates: 0x%X)",
			pPortStates);

	LSA_UNUSED_ARG(pPortStates);

	Status = EDD_STS_ERR_NOT_SUPPORTED;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_SET_PORT_STATE(), Status: 0x%X",
			Status);

	return(Status);

}


//*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_FLUSH_FILTERING_DB              +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Flushs filtering data base and tx-queues of forwarding    +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_FLUSH_FILTERING_DB(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_SWI_FLUSH_FILTERING_DB_PTR_TYPE pFlushFilterDB)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);
	LSA_UNUSED_ARG(pFlushFilterDB);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_FLUSH_FILTERING_DB()");

	Status = EDD_STS_ERR_NOT_SUPPORTED;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_FLUSH_FILTERING_DB(), Status: 0x%X",
			Status);

	return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_SET_PORT_REDUNDANT              +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+                      EDDS_UPPER_SWI_SET_PORT_REDUNDANT_PTR_TYPE pPortRed+*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pPortRed      : Pointer to array of all ports.                         +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set redundancy bit (RT-Bit) of port. Only two port can    +*/
/*+               have the RT-Bit at once.                                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_PORT_REDUNDANT(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_SWI_SET_PORT_REDUNDANT_PTR_TYPE pPortRed)
{
	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_SET_PORT_REDUNDANT(pPortRed: 0x%X)",
			pPortRed);

	LSA_UNUSED_ARG(pPortRed);

	Status = EDD_STS_ERR_NOT_SUPPORTED;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_SET_PORT_REDUNDANT(), Status: 0x%X",
			Status);

	return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_SET_BRIDGE_MODE                 +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+                      LSA_UINT32                                 Mode    +*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  Mode          : Pointer to array of all ports.                         +*/
/*+                   EDDS_BRIDGE_MODE_SF                                   +*/
/*+                   EDDS_BRIDGE_MODE_CT                                   +*/
/*+                                                                         +*/
/*+  Details to parameters see edds-service EDDS_SRV_SWITCH_SET_BRIDGE_MODE +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_NOT_SUPPORTED                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set bridge mode. SF (Store and forward) or CT (CutThrough)+*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_SET_BRIDGE_MODE(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 Mode)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_SET_BRIDGE_MODE(Mode: 0x%X)",
			Mode);

	LSA_UNUSED_ARG(Mode);

	Status = EDD_STS_ERR_NOT_SUPPORTED;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_SET_BRIDGE_MODE(), Status: 0x%X",
			Status);

	return(Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname   :    PACKET32_LL_SWITCH_GET_DEFAULT_MRP_R_PORTS         +*/
/*+  Input/Output   :    LSA_VOID_PTR_TYPE             pLLManagement        +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE                pPortID1+*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE                pPortID2+*/
/*+  Result         :    LSA_RESULT                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pPortID1      : Pointer to UINT32 for PortID of R-Port 1  (1..x)       +*/
/*+  pPortID2      : Pointer to UINT32 for PortID of R-Port 2  (1..x)       +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get Default MRP-R-Ports                                   +*/
/*+                                                                         +*/
/*+               If MRP is not used within system at least on PortID       +*/
/*+               shall be set to 0.  MRP is deactivated                    +*/
/*+                                                                         +*/
/*+               If MRP is supported set the two PortIDs of R-Ports used   +*/
/*+               as default R-Ports. MRP is activated                      +*/
/*+                                                                         +*/
/*+               Note: The Portstate of MRP-R-Ports shall be set to        +*/
/*+                     BLOCKING state at startup all other Ports to        +*/
/*+                     FORWARDING!                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_GET_DEFAULT_MRP_R_PORTS(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID1,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID2)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	LSA_UNUSED_ARG(handle);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_GET_DEFAULT_MRP_R_PORTS()");

	*pPortID1 = 0; /* no R-Port */
	*pPortID2 = 0; /* no R-Port */

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_GET_DEFAULT_MRP_R_PORTS(), Status: 0x%X",
			Status);

	return(Status);

}


#ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    PACKET32_LL_SWITCH_DROP_CNT                 +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDD_UPPER_MEM_U32_PTR_TYPE     pDropCnt     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to get LED mode for                                  +*/
/*+  pDropCnt   : Pointer to address for Dropped frames since last call     +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets dropped frames (not forwarded) since last call.      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR PACKET32_LL_SWITCH_DROP_CNT(
		LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_U32_PTR_TYPE pDropCnt)
{

	LSA_RESULT Status;
	PACKET32_LL_HANDLE_TYPE* handle = (PACKET32_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :PACKET32_LL_SWITCH_DROP_CNT(pDropCnt: 0x%X)",
			pDropCnt);

	Status = EDD_STS_OK;

	*pDropCnt = 0;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:PACKET32_LL_SWITCH_DROP_CNT() Status: 0x%X",
			Status);

	return(Status);

}
#endif
#endif


#endif

/*****************************************************************************/
/*  end of file EDDS_PACKET32.C                                              */
/*****************************************************************************/

