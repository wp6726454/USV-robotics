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
/*  F i l e               &F: edds_isr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-interrupthandler                            */
/*                          This module is very hardwaredependend!!!         */
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
/*  27.05.02    JS    reorder of rx-handling. SRT-Frametype check fixed.     */
/*  12.11.03    JS    added EDDS_LL_DONT_USE_LL_INTERRUPT_FCT                */
/*  10.03.04    JS    added support for tasking-compiler V2.0                */
/*  15.06.04    JS    removed ASRT support                                   */
/*  21.06.04    JS    handling of ibc32 LLIF changed a bit. Now we can       */
/*                    use more then 16 rx-descriptors because we clear       */
/*                    the whole isr-fifo (within LLIF) and so we are not     */
/*                    limited by the 16 isr-fifo entrys (se receivers        */
/*                    stops if the fifo is full).                            */
/*                    -> configurable with new LLIF_CFG_RX_MODE_SINGLE define*/
/*  23.06.04    JS    added #ifdef EDDS_CFG_NRT_INCLUDE                      */
/*  25.06.04    JS    added support for xRT over UDP                         */
/*  08.12.04    JS    UDP-Header checksum dont care!                         */
/*  21.01.05    JS    added Rx-overload check support                        */
/*  25.01.05    JS    renamed EDDS_LL_DONT_USE_LL_INTERRUPT_FCT to           */
/*                    LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT.                    */
/*  28.01.05    JS    LTRC support                                           */
/*  09.02.05    JS    added optional reentrance check                        */
/*  06.07.05    JS    added multiport support                                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  31.06.10    JS    added optional protection macros  (AP00952063)         */
/*  26.07.10    MA    added  new edds_interrupt_poll API function            */
/*  25.05.11    MA    removed LLIF_CFG_RX_MODE_SINGLE macro                  */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  7
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_ISR */


/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*****************************************************************************/
/*                                                                           */
/* Extern variables                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Extern functions                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Globlal variables                                                         */
/*                                                                           */
/*****************************************************************************/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DropUnusedUDPFrames                    +*/
/*+  Input/Output          :    LSA_UINT32  MACTyp                          +*/
/*+                             LSA_BOOL  RxFilterUDP_Broadcast             +*/
/*+                             EDD_UPPER_MEM_U8_PTR_TYPE  pFrame           +*/
/*+                             LSA_UINT32  UserDataOffset                  +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  MACTyp                : MAC Typ                                        +*/
/*+  pFrame                : Framedata                                      +*/
/*+  UserDataOffset        : Offset to IP Header                            +*/
/*+                                                                         +*/
/*+ Return Value:          LSA_TRUE:  filter UDP Frame                      +*/
/*+                        LSA_FALSE: pass to User, do not filter           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function filters the UDP frames based on UDP         +*/
/*+               Whitelist.                                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pFrame,pDDB)
//JB 11/11/2014 pFrame can not be a null ptr - pDDB is checked within EDDS_GetDDB
//WARNING: be careful when calling this function, make sure not to use any null ptr!
EDDS_STATIC LSA_BOOL  EDDS_LOCAL_FCT_ATTR  EDDS_DropUnusedUDPFrames (
	EDDS_LOCAL_DDB_PTR_TYPE      const  pDDB,
    EDD_UPPER_MEM_U8_PTR_TYPE    const  pFrame,
    LSA_UINT32                   const  UserDataOffset )
{
	LSA_BOOL dropFrame;
    EDDS_IP_HEADER_TYPE  *  const  pIPHeader = (EDDS_IP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset];
    EDDS_UDP_HEADER_TYPE *         pUDPHeader;
    LSA_UINT32                     DstPort;
    LSA_UINT8                      IHL;

    EDDS_NRT_TRACE_00(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDS_DropUnusedUDPFrames->");

    IHL = (LSA_UINT8)((pIPHeader->VersionIHL) & 0xF);

    /* first, all frames should be dropped */
	dropFrame = LSA_TRUE;

    if (IHL < 0x5) //IP Header Length is min. 5
    {
        //Drop Frame, because wrong IP Header Length
    } else
    {
        // check for fragmentation
        if ((EDDS_NTOHS(pIPHeader->FragmentOffset) & EDDS_IP_OFFSET) > 0)
        {
            dropFrame = LSA_FALSE;
        }
        else
        {        
            pUDPHeader = (EDDS_UDP_HEADER_TYPE *)(void *)&pFrame[UserDataOffset + (sizeof(LSA_UINT32) * IHL)];
            DstPort = (LSA_UINT32)EDDS_NTOHS(pUDPHeader->DstPort);
    
            if (!EDDS_IS_MC_MAC(pFrame) && (pDDB->pNRT->RxFilterUDP_Unicast))  /* Unicast Frame & should be filtered */
            {
                //Check UDP Whitelist's
                if (    (EDDS_UDP_FILTER_DST_PORT_DHCP_CLIENT == DstPort)
                     || (EDDS_UDP_FILTER_DST_PORT_SNMP == DstPort)
                     || (EDDS_UDP_FILTER_DST_PORT_SNMPTRAP == DstPort)
                     || (EDDS_UDP_FILTER_DST_PORT_PNIO_EPM == DstPort)
                     || (    (DstPort >= EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_BEGIN)
                          && (DstPort <= EDDS_UDP_FILTER_DST_PORT_IANA_FREE_PORT_END) )
                    )
                {
                    //Pass to User
                    dropFrame = LSA_FALSE;
                }
            }else if (EDDS_IS_BC_MAC(pFrame) && (pDDB->pNRT->RxFilterUDP_Broadcast)) /* Broadcast Frame & should be filtered */
            {
                if ((EDDS_UDP_FILTER_DST_PORT_DHCP_CLIENT == DstPort)
                        || (EDDS_UDP_FILTER_DST_PORT_NTP_SNTP == DstPort)
                    )
    
                {
                    //Pass to User
                    dropFrame = LSA_FALSE;
                }
            }else	/* Multicast */
            {
                /* no filtering on multicast */
                dropFrame = LSA_FALSE;
            }
        }
    }

    return dropFrame;
}
//lint +esym(613,pFrame,pDDB)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_IPChecksumAndGetHeader                 +*/
/*+  Input                      EDD_UPPER_MEM_U16_PTR_TYPE    pBuf          +*/
/*+                             LSA_UINT16                   *pIPHeader     +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuf       : Pointer to start of 20 Bytes IP-Header                    +*/
/*+  pIPHeader  : Pointer to 20 Byte filled with IP-Header data (output)    +*/
/*+  Result     : Checksum                                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates IP-Header checksum.                            +*/
/*+               if result is 0 the checksum of the IP-Header is valid     +*/
/*+               Note: pBuffer points to a WORD aligned address            +*/
/*+               Note: There is a "normal" checksum function within edds   +*/
/*+                     (EDDS_IPChecksum()) used to fill a IP-Header!       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_UINT16 EDDS_IPChecksumAndGetHeader (
    EDD_UPPER_MEM_U16_PTR_TYPE pBuf,
    LSA_UINT16                *pIPHeader)
{
    LSA_UINT32  Sum;
    LSA_UINT32  i;

    #undef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
    #ifdef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
    LSA_UINT32  Help;
    EDD_UPPER_MEM_U32_PTR_TYPE pHelp;
    #endif

    Sum = 0;

    #ifdef EDDS_CFG_IP_CHECKSUM_OPTIMIZE
    /*------------------------------------------------------- */
    /* optimized checksum calc (reduced amount of framebuffer */
    /* accesses. pBuf[1] must be DWORD aligned! (is the case  */
    /* within an Ethernetframe). We assume 20 Byte IP Header  */
    /*------------------------------------------------------- */
    Sum  = *pIPHeader++ = pBuf[0];

    pHelp = (EDD_UPPER_MEM_U32_PTR_TYPE) pBuf[1];

    for ( i=0; i< 4; i++ ) /* (4*4 Byte = 16 Byte) */
    {
        Help  = *pHelp++;
        #ifdef EDDS_CFG_BIG_ENDIAN
        Sum  += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
        Sum  += *pIPHeader++ = (LSA_UINT16) Help;
        #else
        Sum  += *pIPHeader++ = (LSA_UINT16) Help;
        Sum  += *pIPHeader++ = (LSA_UINT16) (Help >> 16);
        #endif
    }

    Sum  = *pIPHeader++ = pBuf[9];
    /*------------------------------------------------------- */
    #else
    for ( i=0; i < (EDDS_IP_HEADER_SIZE/2); i++ )
        Sum += *pIPHeader++ = *pBuf++;
    #endif

    /* add carrys so we get ones-complement */
    Sum  = (Sum >> 16) + (Sum & 0xffff);
    Sum += (Sum >> 16);

    return((LSA_UINT16)~Sum);
}

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleRxFrame                          +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ..    *ppRxBuffer    +*/
/*+                             LSA_UINT32                    RxLen         +*/
/*+                             LSA_UINT32                    PortID        +*/
/*+  Output                     EDD_UPPER_MEM_U8_PTR_TYPE     pRxBuffer     +*/
/*+  Result                :    ---                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+  pRxBuffer  : address of pointer to Receive Buffer (in/out)             +*/
/*+  RxLen      : Length of Frame                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for Frametype and calls handler for frame          +*/
/*+               As result the new framebuffer is returned (which may      +*/
/*+               be the same as pFrame)                                    +*/
/*+                                                                         +*/
/*+               First we check for VLAN-Tag. If so we must read the type  +*/
/*+               len field 4 bytes ahead. then wie analyse the type field  +*/
/*+               for the different framestypes wie have to filter.         +*/
/*+               For SRT we also have to check the FrameID-Field which is  +*/
/*+               located behind the Type/len-field.                        +*/
/*+                                                                         +*/
/*+               For xRToverUDP we have to check several more fields.      +*/
/*+                                                                         +*/
/*+               Note: This function have to be very speed optimized and   +*/
/*+                     should do as less as possible access to the frame   +*/
/*+                     buffer (uncached-memory!!)                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,ppRxBuffer,pDDB)
//JB 18/11/2014 checked where called (LLIF) - can not be a null pointer
//WARNING: be careful when using this function, make sure not to use ppRxBuffer as null ptr!
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandleRxFrame
#else /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandleRxFrame
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR  *ppRxBuffer,
    LSA_UINT32                                   RxLen,
    LSA_UINT32                                   PortID)
{

    EDDS_TYPE_LEN_FRAME_ID_TYPE     FrameData;
    #ifdef EDDS_CFG_NRT_INCLUDE
	#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pRQB;
	#endif
    #endif
    LSA_UINT16                      Dummy;
    EDD_UPPER_MEM_U8_PTR_TYPE       pFrame;
    LSA_BOOL                        VLANTagPresent;

    EDDS_ISR_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleRxFrame(pDDB: 0x%X,*ppRxBuffer: 0x%X,RxLen: %d, PortID: %d)",
                           pDDB,
                           *ppRxBuffer,
                           RxLen,
                           PortID);

    /* ---------------------------------------------------------------------------*/
    /* get bytes 12..15 from Frame                                                */
    /* Note that this is in inet-format (big endian)                              */
    /* ---------------------------------------------------------------------------*/

    pFrame = (EDD_UPPER_MEM_U8_PTR_TYPE) *ppRxBuffer;

    FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)pFrame)[3]; //lint !e826 JB 11/11/2014 correct access to pFrame

    /* ---------------------------------------------------------------------------*/
    /* check for VLAN-Tag. if so we must read the real Type/len 4 Bytes ahead     */
    /* ---------------------------------------------------------------------------*/

    if ( FrameData.w.TypeLen == EDDS_VLAN_TAG )
    {
        FrameData.LenTypeID = ((EDD_UPPER_MEM_U32_PTR_TYPE)pFrame)[4]; //lint !e826 JB 11/11/2014 correct access to pFrame
        VLANTagPresent = LSA_TRUE;
    }
    else VLANTagPresent = LSA_FALSE;

    /* ---------------------------------------------------------------------------*/
    /* Now check for the Frametype                                                */
    /* Note: we check for SRT outside the switch because we want to be sure       */
    /*       we get the fastest branch to SRT-case because SRT needs more         */
    /*       filtering.                                                           */
    /* ---------------------------------------------------------------------------*/

    /* ---------------------------------------------------------------------------*/
    /* xRT-Ethernet frame                                                         */
    /* ---------------------------------------------------------------------------*/
    if ( FrameData.w.TypeLen == EDDS_xRT_TAG )
    {

        /* here a check for cyclic and acyclic SRT                     */

        /* Frame-ID ranges:                                            */
        /*                                                             */
        /* 0000...00FF:  Timesynchronisation                           */
        /* 0100...7FFF:  IRT-Frames                                    */
        /* 8000...FBFF:  cyclic SRT                                    */
        /* FC00...FCFF:  acyclic SRT high prio                         */
        /* FC01          PN-IO Alarm high                              */
        /* FD00...FDFF:  reserved                                      */
        /* FE00...FEFC:  acyclic SRT low  prio                         */
        /* FE01          PN-IO Alarm low                               */
        /* FEFE          DCP via xRT                                   */
        /* FEFF          DCP via xRT                                   */
        /* FF00...FFFF:  reserved                                      */
        /*                                                             */

        /* first we check for cyclic SRT, then for acyclic. all other  */
        /* frames will be dropped.                                     */

        #ifndef EDDS_CFG_BIG_ENDIAN
        Dummy = (FrameData.b.FrameID_H << 8 ) + FrameData.b.FrameID_L;
        #else
        Dummy = FrameData.w.FrameID;
        #endif

        if (( Dummy >= EDD_SRT_FRAMEID_CSRT_START ) && ( Dummy <= EDD_SRT_FRAMEID_CSRT_STOP ))
        {
            #ifdef EDDS_CFG_SRT_INCLUDE
        	if((pDDB->pGlob->HWParams.Caps.SWFeatures) & EDDS_LL_CAPS_SWF_SRT)
        	{
				EDDS_RTRxIndicateCyclicXRT( pDDB,
											ppRxBuffer,
											RxLen,
											Dummy,
											VLANTagPresent);
        	}
            #endif
        }
        else
        {

            #ifdef EDDS_CFG_NRT_INCLUDE
            /*------------------------------------------------------ */
            /* a aSRT  Frame was received. We indicate the           */
            /* new frame. (the functions gives us a new              */
            /* rxbuffer which will be given to the                   */
            /* controller below.                                     */
            /* IMPORTANT: This function must be very fast            */
            /*------------------------------------------------------ */

            /* ALARM region ? */
            if ((( Dummy >= EDD_SRT_FRAMEID_ASRT_START_LOW  ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_LOW )) ||
                (( Dummy >= EDD_SRT_FRAMEID_ASRT_START_HIGH ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH )))
            {

                if ( EDDS_NRTRxIndicate( pDDB,
                                                  ppRxBuffer,
                                                  RxLen,
                                                  EDDS_NRT_BUFFER_ASRT_ALARM_IDX,
                                                  PortID,
                                                  EDDS_NRT_DSCR_STATE_IN_USE_XRT) == EDD_STS_OK )
                {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                    /*-------------------------------------------------- */
                    /* we have a frame received which needs further low  */
                    /* prior handling. signal this with output-macro.    */
                    /* this results in calling of an isr-systemrequest.  */
                    /*-------------------------------------------------- */
                    /* Note: we dont need to signal the                  */
                    /*       same request more then once if one is under-*/
                    /*       way. So we check this here                  */
                    /*-------------------------------------------------- */

                    if ( ! pRQB->InUse )
                    {
                        pRQB->InUse = LSA_TRUE;
                        EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                }
            }
            else /* no ALARM */
            {
                if ( EDDS_NRTRxIndicate( pDDB,
                                                  ppRxBuffer,
                                                  RxLen,
                                                  EDDS_NRT_BUFFER_ASRT_OTHER_IDX,
                                                  PortID,
                                                  EDDS_NRT_DSCR_STATE_IN_USE_XRT) == EDD_STS_OK )
                {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                    /*-------------------------------------------------- */
                    /* we have a frame received which needs further low  */
                    /* prior handling. signal this with output-macro.    */
                    /* this results in calling of an isr-systemrequest.  */
                    /*-------------------------------------------------- */
                    /* Note: we dont need to signal the                  */
                    /*       same request more then once if one is under-*/
                    /*       way. So we check this here                  */
                    /*-------------------------------------------------- */

                    if ( ! pRQB->InUse )
                    {
                        pRQB->InUse = LSA_TRUE;
                        EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                }
            }
            #endif
        }
    }
    /* ---------------------------------------------------------------------------*/
    /* no xRT-Ethernet frame                                                      */
    /* ---------------------------------------------------------------------------*/
    else
    {
        switch (FrameData.w.TypeLen)
        {
            case EDDS_IP_TAG: /* IP */
                /*------------------------------------------------------ */
                /* Drop unused UDP Frames                                */
                /*------------------------------------------------------ */
                if (    (pDDB->pNRT->RxFilterUDP_Broadcast)
                     || (pDDB->pNRT->RxFilterUDP_Unicast)
                   )
                {
                    LSA_UINT32             DLen;
                    EDDS_IP_HEADER_TYPE  * pIPHeader;

                    (VLANTagPresent)?(DLen = EDDS_FRAME_HEADER_SIZE_WITH_VLAN):(DLen = EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN);

                    pIPHeader = (EDDS_IP_HEADER_TYPE *) &pFrame[DLen]; //lint !e826 JB 11/11/2014 correct access to pFrame

                    if (EDDS_IP_UDP == pIPHeader->Protocol)
                    {
                        if (EDDS_DropUnusedUDPFrames(pDDB,pFrame, DLen))
                        {
                            /* UDP frame should be dropped */
                            break;
                        }
                        else
                        {
                            /* do nothing, frame should be processed */
                        }
                    }
                }
                /* no break, fall through */

                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                /*------------------------------------------------------ */
                /* check for xRT over UDP-Frames.                        */
                /*------------------------------------------------------ */
            	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_XRT_OVER_UDP)
                {
                    EDDS_UDP_XRT_FRAME_PTR_TYPE pHeader;
                    LSA_UINT32                  DLen;
                    EDDS_IP_HEADER_TYPE         IPHeader; /* IPHeader cache */

                    if ( VLANTagPresent )
                    {
                        pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pFrame[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];
                        DLen   = RxLen-EDDS_FRAME_HEADER_SIZE_WITH_VLAN;
                    }
                    else
                    {
                        pHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &pFrame[EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN];
                        DLen   = RxLen-EDDS_FRAME_HEADER_SIZE_WITHOUT_VLAN;
                    }

                    /* check if this is a xRToverUDP - IP/UDP-Header */

                    if (( FrameData.bIP.IPVersIHL == EDDS_IP_XRT_VER_IHL    ) &&
                        ( pHeader->Ip.Protocol    == EDDS_IP_UDP            ) &&
                        ( pHeader->Udp.DstPort    == EDDS_xRT_UDP_PORT      ))
                    {

                        /* check the IP-Checksum and copy header to IPHeader */
                        if ( EDDS_IPChecksumAndGetHeader((LSA_UINT16 *) pHeader,(LSA_UINT16 *)&IPHeader) == 0 )
                        {

                            LSA_UINT32 IPLen;
                            LSA_UINT32 UDPLen;

                            IPLen  = (LSA_UINT32) EDDS_NTOHS(IPHeader.TotalLength);
                            UDPLen = (LSA_UINT32) EDDS_NTOHS(pHeader->Udp.Length);

                            /* verify some more header settings..        */
                            /* Note: We dont check the UDP-Checksum      */
                            /*       must be 0, else discarded!          */
                            /* check: FragmentOffset (== 0)              */
                            /* check: UDP-Src Port                       */
                            /* check: Local IP Address (== DstIP) or MC  */
                            /* check: No UDP-Checksum (==0)              */
                            /* check: Verify length consistenz           */

                            if (((IPHeader.FragmentOffset & EDDS_IP_FLAG_OFFSET_MSK) == EDDS_IP_FLAG_OFFSET_VAL )
                                && ( pHeader->Udp.SrcPort  == EDDS_xRT_UDP_PORT )
                                &&
                                (
                                    ( IPHeader.DestIP.dw    == pDDB->pGlob->HWParams.IPAddress.dw ) ||
                                    ( (IPHeader.DestIP.b.IP[0] & 0xF0)  == 0xE0 ) /* IP-Multicast 224..239 */
                                )
                                #if 0
                                && ( pHeader->Udp.Checksum == 0        )
                                #endif
                                && ( IPLen  <= DLen                    )
                                && ( IPLen  >= EDDS_UDP_XRT_HEADER_SIZE )
                                && ( UDPLen == IPLen-EDDS_IP_HEADER_SIZE ))
                            {
                                /* we have a valid xRToverUDP-Frame */
                                Dummy = EDDS_NTOHS(pHeader->FrameID); /* get FrameID */

                                if (( Dummy >= EDD_SRT_FRAMEID_CSRT_START ) && ( Dummy <= EDD_SRT_FRAMEID_CSRT_STOP ))
                                {
                                    #ifdef EDDS_CFG_SRT_INCLUDE
                                	if(pDDB->pGlob->HWParams.Caps.SWFeatures & EDDS_LL_CAPS_SWF_SRT)
                                	{
										EDDS_RTRxIndicateCyclicUDP( pDDB,
																	ppRxBuffer,
																	&pHeader->FrameID,  /* pointer to UDP-Data start */
																	&IPHeader,
																	UDPLen-EDDS_UDP_HEADER_SIZE,
																	Dummy);/* FrameID */
                                	}
                                    #endif
                                }
                                else
                                {
                                    #ifdef EDDS_CFG_NRT_INCLUDE
                                    /* we have a asynchroneous xRToverUDP frame.      */

                                    /* ALARM region ? */
                                    if ((( Dummy >= EDD_SRT_FRAMEID_ASRT_START_LOW  ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_LOW )) ||
                                        (( Dummy >= EDD_SRT_FRAMEID_ASRT_START_HIGH ) && ( Dummy <= EDD_SRT_FRAMEID_ASRT_STOP_HIGH )))
                                    {

                                        if ( EDDS_NRTRxIndicate(
                                                 pDDB,
                                                 ppRxBuffer,
                                                 RxLen,
                                                 EDDS_NRT_BUFFER_ASRT_ALARM_IDX,
                                                 PortID,
                                                 EDDS_NRT_DSCR_STATE_IN_USE_UDP) == EDD_STS_OK )
                                        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                            pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                                            /*-------------------------------------------------- */
                                            /* we have a frame received which needs further low  */
                                            /* prior handling. signal this with output-macro.    */
                                            /* this results in calling of an isr-systemrequest.  */
                                            /*-------------------------------------------------- */
                                            /* Note: we dont need to signal the                  */
                                            /*       same request more then once if one is under-*/
                                            /*       way. So we check this here                  */
                                            /*-------------------------------------------------- */

                                            if ( ! pRQB->InUse )
                                            {
                                                pRQB->InUse = LSA_TRUE;
                                                EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                                            }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                        }
                                    }
                                    else /* no ALARM */
                                    {

                                        if ( EDDS_NRTRxIndicate(
                                                 pDDB,
                                                 ppRxBuffer,
                                                 RxLen,
                                                 EDDS_NRT_BUFFER_ASRT_OTHER_IDX,
                                                 PortID,
                                                 EDDS_NRT_DSCR_STATE_IN_USE_UDP) == EDD_STS_OK )
                                        {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                            pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                                            /*-------------------------------------------------- */
                                            /* we have a frame received which needs further low  */
                                            /* prior handling. signal this with output-macro.    */
                                            /* this results in calling of an isr-systemrequest.  */
                                            /*-------------------------------------------------- */
                                            /* Note: we dont need to signal the                  */
                                            /*       same request more then once if one is under-*/
                                            /*       way. So we check this here                  */
                                            /*-------------------------------------------------- */

                                            if ( ! pRQB->InUse )
                                            {
                                                pRQB->InUse = LSA_TRUE;
                                                EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                                            }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                        }
                                    }
                                    #endif
                                }
                                break;  /* if it is xrt over UDP we break */
                            }
                            else
                            {

                                EDDS_ISR_TRACE__00(0,LSA_TRACE_LEVEL_UNEXP,
                                                      "EDDS_HandleRxFrame(): xRToverUDP invalid IP header or wrong IP");

                                pDDB->pGlob->RxStats.RxXRToUDPWrongHeader++;
                                break; /* invalid Header or IP setting of xRToUDP-Frame. discard */
                            }
                        }
                        else
                        {
                            EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,
                                                  "EDDS_HandleRxFrame(): xRToverUDP frame with wrong IP checksum");

                            pDDB->pGlob->RxStats.RxXRToUDPIPWrongChecksum++;
                            break; /* invalid checksum. discard */
                        }

                    } /* not a xRToUDP Frame. let it handle by standard IP */
                }
                #endif

                #ifndef EDDS_CFG_NRT_INCLUDE
                break;  /* break if we dont support NRT */
                #else
                /* no break, fall through if we support NRT */
                #endif
				//lint -fallthrough
				//JB 12/11/2014 on purpose, see comment above

                #ifdef EDDS_CFG_NRT_INCLUDE

                /* no break, fall through, see above comments */
            case EDDS_ARP_TAG: /* ARP          */
            case EDDS_RARP_TAG: /* reversed ARP */

                /*------------------------------------------------------ */
                /* a NRT-ARPIP Frame was received. We indicate the       */
                /* new frame. (the functions gives us a new              */
                /* rxbuffer which will be given to the                   */
                /* controller below.                                     */
                /*------------------------------------------------------ */

                if ( EDDS_NRTRxIndicate(pDDB,
                                             ppRxBuffer,
                                             RxLen,
                                             EDDS_NRT_BUFFER_IP_IDX,
                                             PortID,
                                             EDDS_NRT_DSCR_STATE_IN_USE) == EDD_STS_OK )
                {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                    pRQB    = &pDDB->pGlob->RQBInt_Rx_NRT;

                    /*-------------------------------------------------- */
                    /* we have a frame received which needs further low  */
                    /* prior handling. signal this with output-macro.    */
                    /* this results in calling of an isr-systemrequest.  */
                    /*-------------------------------------------------- */
                    /* Note: we dont need to signal the                  */
                    /*       same request more then once if one is under-*/
                    /*       way. So we check this here                  */
                    /*-------------------------------------------------- */

                    if ( ! pRQB->InUse )
                    {
                        pRQB->InUse = LSA_TRUE;
                        EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                }
                break;
                #endif

            default:

                #ifdef EDDS_CFG_NRT_INCLUDE

                /* -------------------------------------------------------------*/
                /* convert type/len to host-format                              */
                /* -------------------------------------------------------------*/

                #ifndef EDDS_CFG_BIG_ENDIAN
                Dummy = (FrameData.b.TypeLen_H << 8 ) + FrameData.b.TypeLen_L;
                #else
                Dummy = FrameData.w.TypeLen;
                #endif

                if ( Dummy <= 1500 )
                {

                    /*------------------------------------------------------ */
                    /* a NRT-LEN Frame was received. We indicate the         */
                    /* new frame. (the functions gives us a new              */
                    /* rxbuffer which will be given to the                   */
                    /* controller below.                                     */
                    /*------------------------------------------------------ */

                    if ( EDDS_NRTRxIndicate(pDDB,
                                                 ppRxBuffer,
                                                 RxLen,
                                                 EDDS_NRT_BUFFER_OTHER_IDX,
                                                 PortID,
                                                 EDDS_NRT_DSCR_STATE_IN_USE_OTHER_LEN) == EDD_STS_OK )
                    {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                        pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                        /*-------------------------------------------------- */
                        /* we have a frame received which needs further low  */
                        /* prior handling. signal this with output-macro.    */
                        /* this results in calling of an isr-systemrequest.  */
                        /*-------------------------------------------------- */
                        /* Note: we dont need to signal the                  */
                        /*       same request more then once if one is under-*/
                        /*       way. So we check this here                  */
                        /*-------------------------------------------------- */

                        if ( ! pRQB->InUse )
                        {
                            pRQB->InUse = LSA_TRUE;
                            EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                        }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                    }
                }
                else /* NRT-Other*/
                {

                    /*------------------------------------------------------ */
                    /* a NRT-OTHER Frame was received. We indicate the       */
                    /* new frame. (the functions gives us a new              */
                    /* rxbuffer which will be given to the                   */
                    /* controller below.                                     */
                    /*------------------------------------------------------ */

                    if ( EDDS_NRTRxIndicate(pDDB,
                                                 ppRxBuffer,
                                                 RxLen,
                                                 EDDS_NRT_BUFFER_OTHER_IDX,
                                                 PortID,
                                                 EDDS_NRT_DSCR_STATE_IN_USE_OTHER_TYPE) == EDD_STS_OK )
                    {
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                        pRQB = &pDDB->pGlob->RQBInt_Rx_NRT;

                        /*-------------------------------------------------- */
                        /* we have a frame received which needs further low  */
                        /* prior handling. signal this with output-macro.    */
                        /* this results in calling of an isr-systemrequest.  */
                        /*-------------------------------------------------- */
                        /* Note: we dont need to signal the                  */
                        /*       same request more then once if one is under-*/
                        /*       way. So we check this here                  */
                        /*-------------------------------------------------- */

                        if ( ! pRQB->InUse )
                        {
                            pRQB->InUse = LSA_TRUE;
                            EDDS_DO_INT_REQUEST_RCV_NRT(pRQB->pRQB);
                        }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                    }
                }
                #endif
                break;  /* default */

        } /* switch */
    } /* else */


    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandleRxFrame(*ppRxBuffer: 0x%X)",
                           *ppRxBuffer);

}
//lint +esym(613,ppRxBuffer,pDDB)

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_InterruptPHY                           +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+                             LSA_UINT32                 IntSource        +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDDS Isr-interrupthandler for PHY-Interrupts              +*/
/*+               used for status-change of link/speed/mode.                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called (LLIF)
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDS_InterruptPHY(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{

    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pRQB;

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_InterruptPHY(pDDB: 0x%X)",
                           pDDB);

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_INTERRUPT_PHY(pDDB: 0x%X)",pDDB);

    if ( EDDS_LL_INTERRUPT_PHY(pDDB) == EDD_STS_OK)  //lint !e613 JB 19/11/2014 unknown-name (pDDB)
    {

        EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_INTERRUPT_PHY(). Status == EDD_STS_OK");

        pGlob   = pDDB->pGlob;
        pRQB    = &pGlob->RQBInt_PHY;

        if ( ! pRQB->InUse )
        {
            pRQB->InUse = LSA_TRUE;
            EDDS_DO_INT_REQUEST_PHY(pRQB->pRQB);
        }
    }
    else
    {
        EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_INTERRUPT_PHY(). Status <> EDD_STS_OK");
    }

    EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_InterruptPHY()");

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_InterruptRx                            +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDDS Isr-interrupthandler for Received frames             +*/
/*+                                                                         +*/
/*+               Checks for frames coming in. If something is present, we  +*/
/*+               check the frametype (NRT/SRT?) and call an component      +*/
/*+               dependen handler for handling the receive-frame. this     +*/
/*+               handler must be very short/fast (NRT has a second ring    +*/
/*+               so we only re-queue the buffer and need no copy!) The     +*/
/*+               whole handler must no exceed 6.7us !! to be save not to   +*/
/*+               loss SRT-Frames!!                                         +*/
/*+                                                                         +*/
/*+               If a frame was received we call the output-function       +*/
/*+               EDDS_DO_INT_REQUEST with an RQB. The systemadaption calls +*/
/*+               edds_system() with this RQB (but not in interrupt-state!!) +*/
/*+               In edds_system more, less timecritical, handling is done  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called (LLIF)
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDS_InterruptRx(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{

    LSA_RESULT                      Result;
    EDD_UPPER_MEM_PTR_TYPE          pRxBuffer;
    LSA_UINT32                      RxLen;
    LSA_UINT32                      PortID;
    #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    #endif

    #ifdef EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD
    LSA_UINT32 rcvCnt = 0;
    #endif

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_InterruptRx(pDDB: 0x%X)",
                           pDDB);

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_INTERRUPT_RX(pDDB: 0x%X)",pDDB);

    Result = EDDS_LL_INTERRUPT_RX(pDDB); //lint !e613 JB 19/11/2014 unknown-name (pDDB)

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_INTERRUPT_RX(). Status: 0x%X",Result);

    if ( Result == EDD_STS_OK)
    {
        /*-------------------------------------------------------------------*/
        /* check for received frames                                         */
        /*-------------------------------------------------------------------*/

        EDDS_ISR_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,
                            "==> EDDS_LL_RECV(pDDB: 0x%X,&pRXBuffer: 0x%X, &RxLen: 0x%X)",
                            pDDB,
                            &pRxBuffer,
                            &RxLen);

        while ( (Result = EDDS_LL_RECV(pDDB,
                                       &pRxBuffer,
                                       &RxLen,
                                       &PortID)) != EDD_STS_OK_NO_DATA )
        {


            EDDS_ISR_TRACE_04(0,LSA_TRACE_LEVEL_NOTE,
             "<== EDDS_LL_RECV(pRXBuffer: 0x%X, RxLen: 0x%X, PortID: %d), Result: 0x%X",
             pRxBuffer,
             RxLen,
             PortID,
             Result);

            /*---------------------------------------------------------------*/
            /* if data was present we only continue if the data is           */
            /* errorfree. data with error will be ignored.                   */
            /*---------------------------------------------------------------*/

            if ( Result == EDD_STS_OK )
            {
                EDDS_HandleRxFrame(pDDB,&pRxBuffer,RxLen,PortID);

            } /* if (Result == EDD_STS_OK)..*/

            /*-------------------------------------------------------------- */
            /* provide a new buffer to the controller                        */
            /*-------------------------------------------------------------- */


            EDDS_ISR_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_RECV_PROVIDE(pDDB: 0x%X,pRXBuffer: 0x%X)",
                                pDDB,
                                pRxBuffer);

            if ( EDDS_LL_RECV_PROVIDE(pDDB,pRxBuffer) != EDD_STS_OK )
            {
                EDDS_FatalError( EDDS_FATAL_ERR_ISR,
                                 EDDS_MODULE_ID,
                                 __LINE__);
            }

            EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_RECV_PROVIDE(), Result: EDD_STS_OK");

            #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
            /*-------------------------------------------------------------- */
            /* Rx Overload check.                                            */
            /* If we received too much frames we turn off the receiver. It   */
            /* will be reenabled after a time period. This is to prevent     */
            /* the CPU from heavy network traffic causing denial of service  */
            /*-------------------------------------------------------------- */

            if(EDDS_LL_AVAILABLE(pDDB,disableRX))
            {
            	pGlob  = pDDB->pGlob;

				if ( ! pGlob->RxOverload.RxOFF ) /* if not already OFF */
				{
					EDDS_INTERNAL_REQUEST_PTR_TYPE  pRQB;

					if ( ++pGlob->RxOverload.RxCnt >= EDDS_CFG_RX_OVERLOAD_RX_LIMIT )
					{
						pGlob->RxOverload.RxOFF  = LSA_TRUE;
						pGlob->RxOverload.Status = EDD_RX_OVERLOAD;
						pGlob->Errors.RxOverloadCnt++;

						EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
											  "EDDS_InterruptRx: Rx-Overload detected. Turning OFF receiver (pDDB: 0x%X)",
											  pDDB);


						EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
											"==> EDDS_LL_RX_OVERLOAD_RX_OFF(pDDB: 0x%X)",
											pDDB);

						EDDS_LL_RX_OVERLOAD_RX_OFF(pDDB);

						EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
											"<== EDDS_LL_RX_OVERLOAD_RX_OFF()");

						/* three stages of OFF time. */
						switch ( pGlob->RxOverload.OFFLevel )
						{
							case 0:
								pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_1+1; /* +1 jitter */
								pGlob->RxOverload.OFFLevel = 1;
								break;
							case 1:
								pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_2+1; /* +1 jitter */
								pGlob->RxOverload.OFFLevel = 2;
								break;
							default:
								pGlob->RxOverload.OFFCnt = EDDS_CFG_RX_OVERLOAD_RXOFF_3+1; /* +1 jitter */
								break;
						}

						pRQB = &pDDB->pGlob->RQBInt_Rx_OverEvent;

						if ( ! pRQB->InUse )
						{
							/* Note that the RQB is already filled up in startup!     */
							pRQB->InUse = LSA_TRUE;
							EDDS_DO_INT_REQUEST_RX_OVERLOAD(pRQB->pRQB);
						}

					}
				}
            }
            #endif

            #ifdef EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD
            rcvCnt++;

            if ( rcvCnt > pDDB->RxIsrMaxFrameCnt )
            {
                EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
                                    "EDDS_LL_RECV(), rcvCnt(%u)", rcvCnt);
                Result = EDD_STS_OK_NO_DATA;
                break;
            }
            #endif

            EDDS_ISR_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,
                                "==> EDDS_LL_RECV(pDDB: 0x%X,&pRXBuffer: 0x%X, &RxLen: 0x%X)",
                                pDDB,
                                &pRxBuffer,
                                &RxLen);

        }


        if ( Result == EDD_STS_OK_NO_DATA )  //lint !e774 JB 11/11/2014 False if EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD defined
        {
            EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                                "<== EDDS_LL_RECV(), Result: EDD_STS_OK_NO_DATA");
        }

    }

    EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_InterruptRx()");


}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_InterruptTx                            +*/
/*+  Input                      EDDS_LOCAL_DDB_PTR_TYPE const pDDB          +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : DDB reference                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDDS Isr-interrupthandler for Transmitinterrupt           +*/
/*+                                                                         +*/
/*+               Checks if the transmit interrupt signaled the end of a    +*/
/*+               transmission. if so we call the output function           +*/
/*+               EDDS_DO_INT_REQUEST() with an RQB which the systemadaption +*/
/*+               uses for calling the edds_system() function.              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called (LLIF)
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
EDDS_STATIC LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDS_InterruptTx(EDDS_LOCAL_DDB_PTR_TYPE const pDDB)
{

    LSA_RESULT                      Result;
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pRQB;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_InterruptTx(pDDB: 0x%X)",
                           pDDB);

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"==> EDDS_LL_INTERRUPT_TX(pDDB: 0x%X)",pDDB);

    Result = EDDS_LL_INTERRUPT_TX(pDDB); //lint !e613 JB 19/11/2014 unknown-name (pDDB)

    EDDS_ISR_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<== EDDS_LL_INTERRUPT_TX(). Status: 0x%X",Result);

    if ( Result == EDD_STS_OK )
    {

        /*-------------------------------------------------------------------*/
        /* if a transmission is ready, signal to lower prio handler.         */
        /*-------------------------------------------------------------------*/

        /*-------------------------------------------------- */
        /* we have a frame transmitted                       */
        /* signal this with output-macro.                    */
        /* this results in calling of an isr-systemrequest.  */
        /*-------------------------------------------------- */
        /* Note: the systemadaption dont need to signal the  */
        /*       same request more then once if one is under-*/
        /*       way. So we check this here                  */
        /*-------------------------------------------------- */

        pGlob = pDDB->pGlob;
        pRQB  = &pGlob->RQBInt_Tx;

        if ( ! pRQB->InUse )
        {
            pRQB->InUse = LSA_TRUE;
            EDDS_DO_INT_REQUEST_SND(pRQB->pRQB);
        }
    }


    EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_InterruptTx()");

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Function name         :    edds_interrupt                              +*/
/*+  Input                      EDDS_HANDLE                hDDB             +*/
/*+                             LSA_UINT32                 IntSource        +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hDDB       : EDDS device handle (from open device)                     +*/
/*+  IntSource  : Source of interrupt:                                      +*/
/*+                                                                         +*/
/*+               EDDS_INTERRUPT_NONE:    no interrupt event, discard.      +*/
/*+               EDDS_INTERRUPT_TX  :    Interrupt for Tx  event           +*/
/*+               EDDS_INTERRUPT_RX  :    Interrupt for Rx  event           +*/
/*+               EDDS_INTERRUPT_PHY :    Interrupt for PHY event           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: EDDS interrupt handling                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,hDDB)
//JB 18/11/2014 checked where called (pDDB -> hDDB)
//WARNING: be careful when calling this function, make sure not to use hDDB as null ptr!
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_interrupt(EDDS_HANDLE const hDDB,LSA_UINT32 IntSource)
{
	EDDS_LOCAL_DDB_PTR_TYPE const pDDB = (EDDS_LOCAL_DDB_PTR_TYPE const) hDDB;
//  EDDS_GetDDB(hDDB,&pDDB);

	EDDS_ISR_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_interrupt(hDDB: 0x%X, IntSource: 0x%X)",
                           hDDB,
                           IntSource);

#ifdef EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    EDDS_LOCK();
#endif

    EDDS_ENTER_CHECK_INTERRUPT();

    /*---------------------------------------------------------------------------*/
    /* for performance we dont use EddsGetDDB here                               */
    /* We assume the hDDB is valid                                               */
    /*---------------------------------------------------------------------------*/

#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT

	if(EDDS_LL_AVAILABLE(pDDB,interrupt))
	{
		EDDS_ISR_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
							"==> EDDS_LL_INTERRUPT(pDDB: 0x%X, IntSource: %d)",
							pDDB,
							IntSource);
		IntSource = EDDS_LL_INTERRUPT(pDDB,IntSource);
		EDDS_ISR_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
			"<== EDDS_LL_INTERRUPT(pDDB: 0x%X): IntSource: %d",
							pDDB,
							IntSource);
	}
#endif
    switch ( IntSource)
    {

            /*---------------------------------------------------------------*/
        case EDDS_INTERRUPT_TX:
            /*---------------------------------------------------------------*/
            EDDS_InterruptTx(pDDB);
            break;

            /*---------------------------------------------------------------*/
        case EDDS_INTERRUPT_RX:
            /*---------------------------------------------------------------*/
            EDDS_InterruptRx(pDDB);
            break;

            /*---------------------------------------------------------------*/
        case EDDS_INTERRUPT_PHY:
            /*---------------------------------------------------------------*/
            EDDS_InterruptPHY(pDDB);
            break;

            /*---------------------------------------------------------------*/
        case EDDS_INTERRUPT_NONE:
            /*---------------------------------------------------------------*/
            break;
            /*---------------------------------------------------------------*/
        default:
            /*---------------------------------------------------------------*/
            EDDS_FatalError(EDDS_FATAL_ERR_ISR,
                            EDDS_MODULE_ID,
                            __LINE__);
            break;
    } /* switch */

    EDDS_EXIT_CHECK_INTERRUPT();

#ifdef EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    EDDS_UNLOCK();
#endif

    EDDS_ISR_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_interrupt()");


}
//lint +esym(613,hDDB)

//lint -esym(613,pDDB)
//JB 18/11/2014 will be checked within EDDS_LL_INTERRUPT_POLL
LSA_BOOL EDD_UPPER_IN_FCT_ATTR  edds_interrupt_poll(EDDS_HANDLE const hDDB, LSA_UINT32* IntSource)
{
    EDDS_LOCAL_DDB_PTR_TYPE const pDDB = (EDDS_LOCAL_DDB_PTR_TYPE const) hDDB;
    //EDDS_GetDDB(hDDB,&pDDB);
    if(pDDB->pGlob->HWIsSetup)
    {
	return EDDS_LL_INTERRUPT_POLL(pDDB, IntSource);
    }
    else
    {
        *IntSource = EDDS_INTERRUPT_NONE;
    	return LSA_FALSE;
    }
}
//lint +esym(613,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Function name         :    EDDS_TRIGGER_SW_INTERRUPT                   +*/
/*+  Input                      EDDS_HANDLE                hDDB             +*/
/*+                             LSA_UINT32                 IntSource        +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hDDB       : EDDS device handle (from open device)                     +*/
/*+  IntSource  : Source of interrupt:                                      +*/
/*+                                                                         +*/
/*+               EDDS_INTERRUPT_NONE:    no interrupt event, discard.      +*/
/*+               EDDS_INTERRUPT_TX  :    Interrupt for Tx  event           +*/
/*+               EDDS_INTERRUPT_RX  :    Interrupt for Rx  event           +*/
/*+               EDDS_INTERRUPT_PHY :    Interrupt for PHY event           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: call edds_interrupt from edds context.                    +*/
/*+               This function allows edds/llif software to signal an      +*/
/*+               interrupt event independently to edds_system, without the +*/
/*+               cooperation of the hardware.                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_TRIGGER_SW_INTERRUPT(EDDS_HANDLE const hDDB, LSA_UINT32 IntSource)
{
#if !defined(EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT)
	EDDS_LOCK();
#endif	/* !defined(EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT) */

	edds_interrupt(hDDB, IntSource);

#if !defined(EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT)
	EDDS_UNLOCK();
#endif	/* !defined(EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT) */
}

#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
