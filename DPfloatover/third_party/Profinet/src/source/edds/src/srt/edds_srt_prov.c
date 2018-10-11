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
/*  F i l e               &F: edds_srt_prov.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDS. provider          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  06.06.02    JS    initial version.                                       */
/*                    support for max-provider-cnt per cycle and phasemasks  */
/*                    ProviderGroupCnt now a define.                         */
/*  27.11.03    JS    bugfix. used ProviderByteCnt instead of ProviderCnt    */
/*  01.12.03    JS    CLEAR_OPERATE->STOP_RUN.                               */
/*              JS    Changed DataStatus handling                            */
/*  20.01.04    JS    EDDS_RTProviderGetCycleCounter added                   */
/*                    changed SRTProviderSetPhaseMasks to EDDSRTProviderSe.. */
/*                    changed starting with CycleCounter-1 to CycleCounter,  */
/*                    so the Counter read with EDDSRTProivderGetCycleCounter */
/*                    matches better.                                        */
/*                    pBuffer-Parameter removed from consumer-add            */
/*  21.01.04    JS    Changed CycleBaseFactor and ReductionRatio interpretion*/
/*  03.02.04    JS    Changed calculation of maximum used bandwith for a     */
/*                    ratio/phase on provider add/change phase               */
/*  05.04.04    JS    changes to prevent compiler warnings                   */
/*                    Using LSA_UNUSED_ARG                                   */
/*  19.04.04    JS    Bugfix. Added updating of maxabove-values within       */
/*                    EDDS_RTProviderChangePhase()                           */
/*  31.01.05    JS    LTRC support                                           */
/*  06.07.05    JS    added multiport support                                */
/*  11.10.05    JS    changed a memory attribute to avoid compiler warning   */
/*  10.11.05    JS    Added CycleCounter to EDDS_CSRTIndicationFill          */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  03.08.06    JS    tracelevel of overload error configureable             */
/*  03.08.06    JS    tracelevel of overload error configureable             */
/*  26.07.07    JS    added EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING           */
/*                    (will skip overload handling. useful if provider timer */
/*                     is inaccurate. we just skip event an continue)        */
/*  04.05.09    JS    removed EDDS_PROV_FIRST_SEND indication                */
/*  23.09.09    JS    removed support for EDD_CSRT_ID_ALL                    */
/*                    added ProviderAutoStop and new PROVIDER_CONTROL User IF*/
/*  18.01.10    JS    AP00914640: Provider shall not Autostop before first   */
/*                    MISS detected (initial MISS state shall not Autostop a */
/*                    Provider! Note: The Consumer must be activated with    */
/*                    "SetToUnknown" so the first Consumer MISS can be used  */
/*  31.06.10    JS    added protection for cooperative environments.         */
/*                    (AP00952063)                                           */
/*  18.02.2015  RZ    added support of correct sendorder of providers        */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  145
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SRT_PROV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

#include "edds_iobuf_int.h"

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_SRT_INCLUDE

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                    Macros                                 */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderAddToSendListInsert          +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             EDDS_SRT_CHAIN_BASE_TYPE* pSendList         +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProvider       +*/
/*+                             LSA_UINT16 ProviderIdx                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp       : Pointer to SRT management                            +*/
/*+  pSendList       : Pointer to send list                                 +*/
/*+  pProvider       : Pointer to provider                                  +*/
/*+  ProviderIdx     : index of provider                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: add a provider into a send list                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderAddToSendList(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    EDDS_SRT_CHAIN_BASE_TYPE* pSendList,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProvider,
    LSA_UINT16 ProviderIdx)
{
    EDDS_IS_VALID_PTR(pCSRTComp);  
    EDDS_IS_VALID_PTR(pSendList);
    EDDS_IS_VALID_PTR(pProvider);

    /* empty list */
    if( EDDS_SRT_CHAIN_IDX_END == pSendList->BottomIdx)
    {
        pProvider->SendLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pProvider->SendLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pSendList->TopIdx = ProviderIdx;

        /* THIS must be the last action. Adding the Provider to the    */
        /* Knot-Send-List.                                             */
        /* With this sequence we dont have a conflict situation if     */
        /* we currently sending from this list. because the sender     */
        /* only use the BottomIdx and Next and so the addition here is */
        /* the only point of accessing the same memory location. but   */
        /* this doesnt matter. Maybe the sender will use this new      */
        /* provider or not.                                            */
        pSendList->BottomIdx = ProviderIdx;
    }
    else
    {
        if (EDD_CYCLE_PHASE_SEQUENCE_LAST == pProvider->CyclePhaseSequence)
        {
            LSA_UINT16 IdxTopOld;
            /* insert at the end */
            pProvider->SendLink.Next = EDDS_SRT_CHAIN_IDX_END;
            pProvider->SendLink.Prev = pSendList->TopIdx;

            /* save old TopIdx */
            IdxTopOld = pSendList->TopIdx;

            pSendList->TopIdx = ProviderIdx;

            /* THIS must be the last action. Adding the Provider to the    */
            /* Knot-Send-List.                                             */
            /* With this sequence we dont have a conflict situation if     */
            /* we currently sending from this list. because the sender     */
            /* only use the BottomIdx and Next and so the addition here is */
            /* the only point of accessing the same memory location. but   */
            /* this doesnt matter. Maybe the sender will use this new      */
            /* provider or not.                                            */
            pCSRTComp->Prov.pProvTbl[IdxTopOld].SendLink.Next = ProviderIdx;
        }
        else
        {

            EDDS_CSRT_PROVIDER_TYPE_PTR pPrevProv = &(pCSRTComp->Prov.pProvTbl[pSendList->BottomIdx]);
            LSA_UINT16  IdxPrevProv = pSendList->BottomIdx;
            /* if provider is new first element */
            if(pProvider->CyclePhaseSequence < pPrevProv->CyclePhaseSequence)
            {
                pProvider->SendLink.Next = IdxPrevProv;
                pProvider->SendLink.Prev = EDDS_SRT_CHAIN_IDX_END;

                /* previously first provider references now new provider as Prev */
                pPrevProv->SendLink.Prev = ProviderIdx;

                /* THIS must be the last action. Adding the Provider to the    */
                /* Knot-Send-List.                                             */
                /* With this sequence we dont have a conflict situation if     */
                /* we currently sending from this list. because the sender     */
                /* only use the BottomIdx and Next and so the addition here is */
                /* the only point of accessing the same memory location. but   */
                /* this doesnt matter. Maybe the sender will use this new      */
                /* provider or not.                                            */
                pSendList->BottomIdx = ProviderIdx;
            }
            else
            {
                /* search index */
                while(EDDS_SRT_CHAIN_IDX_END != pPrevProv->SendLink.Next )
                {
                    EDDS_CSRT_PROVIDER_TYPE_PTR pNextProv = &(pCSRTComp->Prov.pProvTbl[pPrevProv->SendLink.Next]);
                    if(pProvider->CyclePhaseSequence < pNextProv->CyclePhaseSequence)
                    {
                        /* found */
                        break;
                    }
                    /* save idx of next provider in list */
                    IdxPrevProv = pPrevProv->SendLink.Next;
                    /* save next provider */
                    pPrevProv = pNextProv;

                }
                /* insert behind pLastProv */
                pProvider->SendLink.Next = pPrevProv->SendLink.Next;
                /* if pProvider is not the last element */
                if(EDDS_SRT_CHAIN_IDX_END != pPrevProv->SendLink.Next)
                {
                    pCSRTComp->Prov.pProvTbl[pPrevProv->SendLink.Next].SendLink.Prev = ProviderIdx;
                }
                else
                {
                    /* new provider is last provider now */
                    pSendList->TopIdx = ProviderIdx;
                }
                pProvider->SendLink.Prev = IdxPrevProv;

                /* THIS must be the last action. Adding the Provider to the    */
                /* Knot-Send-List.                                             */
                /* With this sequence we dont have a conflict situation if     */
                /* we currently sending from this list. because the sender     */
                /* only use the BottomIdx and Next and so the addition here is */
                /* the only point of accessing the same memory location. but   */
                /* this doesnt matter. Maybe the sender will use this new      */
                /* provider or not.                                            */
                pPrevProv->SendLink.Next = ProviderIdx;
            }
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTSetupProviderFrameSrcMAC             +*/
/*+  Input                 :    EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE  pSrcMAC         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a Framebuffer for provider with the given values    +*/
/*+               (parameters not checked!)                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pSrcMAC)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pSrcMAC as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTSetupProviderFrameSrcMAC(
    EDD_UPPER_MEM_PTR_TYPE      pBuffer,
    EDDS_LOCAL_MEM_U8_PTR_TYPE  pSrcMAC)
{

    LSA_UINT32  i;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i+EDD_MAC_ADDR_SIZE]  = pSrcMAC[i]; //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    }

}
//lint +esym(613,pSrcMAC)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTSetupProviderFrameXRT                +*/
/*+  Input                 :    EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             LSA_UINT32                  DataLen         +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE  pDstMAC         +*/
/*+                             LSA_UINT16                  FrameID         +*/
/*+                             LSA_UINT16                  VLANID          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a framebuffer for provider with the given values    +*/
/*+               (parameters not checked!)                                 +*/
/*+               Transferstatus is set to 0                                +*/
/*+               VLANID is a part of the VLAN-Tag.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDstMAC)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDstMAC as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTSetupProviderFrameXRT(
    EDD_UPPER_MEM_PTR_TYPE      pBuffer,
    LSA_UINT32                  DataLen,
    EDDS_LOCAL_MEM_U8_PTR_TYPE  pDstMAC,
    LSA_UINT16                  FrameID,
    LSA_UINT16                  VLANID)
{

    LSA_UINT32  i;
    LSA_UINT16  VLANTag;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i]  = pDstMAC[i]; //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    }

    /* Setup VLAN-Tag with VLANId and Priority */

    VLANTag = (LSA_UINT16) ((( VLANID << EDDS_SRT_VLAN_ID_SHIFT ) & EDDS_SRT_VLAN_ID_MSK) |  //lint !e835 JB 18/11/2014 #def-based
                            ( EDDS_SRT_VLAN_PRIO_CSRT << EDDS_SRT_VLAN_PRIO_SHIFT));

    EDDS_SRT_FRAME_ACC_VLAN_TAG(pBuffer) = EDDS_VLAN_TAG; //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_VLAN_ID(pBuffer) = EDDS_HTONS(VLANTag); //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_TYPELEN(pBuffer) = EDDS_xRT_TAG; //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call
    EDDS_SRT_FRAME_ACC_FRAMEID(pBuffer) = EDDS_HTONS(FrameID); //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call

    /* cyclecounter and datastatus will be inserted with every send */

	/* TransferStatus */ 
    ((EDD_UPPER_MEM_U8_PTR_TYPE  )pBuffer)[EDDS_CSRT_FRAME_IO_START_WITH_VLAN+DataLen+EDDS_CSRT_FRAME_TRANSFER_STATUS_OFFSET] = 0; //lint !e613 JB 19/11/2014 *pBuffer (unknown-name) | cannot be a null ptr, checked before function call 

}
//lint -esym(613,pDstMAC)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_IPChecksum                             +*/
/*+  Input                      EDD_UPPER_MEM_U16_PTR_TYPE    pBuf          +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pBuf       : Pointer to start of 20 Bytes IP-Header                    +*/
/*+  Result     : Checksum                                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates IP-Header checksum.                            +*/
/*+               if result is 0 the checksum of the IP-Header is valid     +*/
/*+               Note: pBuffer points to a WORD aligned address            +*/
/*+               Note: There is a "optimized" checksum function within edds +*/
/*+                     (EDDS_IPChecksumAndGetHeader()) used to check Header! +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_UINT16 EDDS_IPChecksum (
    EDD_UPPER_MEM_U16_PTR_TYPE pBuf)
{
    LSA_UINT32  Sum;
    LSA_UINT32  i;
    Sum = 0;

    for ( i=0; i < (EDDS_IP_HEADER_SIZE/2); i++ )
        Sum += *pBuf++;

    /* add carrys so we get ones-complement */
    Sum  = (Sum >> 16) + (Sum & 0xffff);
    Sum += (Sum >> 16);

    return((LSA_UINT16)~Sum);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTSetupProviderFrameUDP                +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE      pBuffer         +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup a Framebuffer for provider with provider values for +*/
/*+               xRToverUDP                                                +*/
/*+               Transferstatus is set to 0                                +*/
/*+               VLANID is a part of the VLAN-Tag.                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTSetupProviderFrameUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    EDD_UPPER_MEM_PTR_TYPE      pBuffer,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv)
{

    LSA_UINT32                 i;
    LSA_UINT16                 VLANTag;
    EDDS_UDP_XRT_FRAME_PTR_TYPE pIPHeader;
    LSA_UINT16                 Len;

    for (i=0; i<EDD_MAC_ADDR_SIZE; i++)
    {
        ((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[i]  = pProv->DstMAC.MacAdr[i];
    }

    /* Setup VLAN-Tag with VLANId and Priority */

    VLANTag = (LSA_UINT16) ((( pProv->VLANID << EDDS_SRT_VLAN_ID_SHIFT ) & EDDS_SRT_VLAN_ID_MSK) |
                            ( EDDS_SRT_VLAN_PRIO_CSRT << EDDS_SRT_VLAN_PRIO_SHIFT));

    EDDS_SRT_FRAME_ACC_VLAN_TAG(pBuffer) = EDDS_VLAN_TAG;
    EDDS_SRT_FRAME_ACC_VLAN_ID(pBuffer) = EDDS_HTONS(VLANTag);
    EDDS_SRT_FRAME_ACC_TYPELEN(pBuffer) = EDDS_IP_TAG;

    pIPHeader = (EDDS_UDP_XRT_FRAME_PTR_TYPE) &((EDD_UPPER_MEM_U8_PTR_TYPE )pBuffer)[EDDS_FRAME_HEADER_SIZE_WITH_VLAN];

    Len = sizeof(pIPHeader->Ip) + sizeof(pIPHeader->Udp) + sizeof(pIPHeader->FrameID) + pProv->DataLen;

    /* insert Transfer Status */
    ((EDD_UPPER_MEM_U8_PTR_TYPE  )pBuffer)[EDDS_FRAME_HEADER_SIZE_WITH_VLAN + Len + EDDS_CSRT_FRAME_TRANSFER_STATUS_OFFSET] = 0; /* TransferStatus */

    Len += EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE; /* IP-Len */

    /* insert IP-header */
    pIPHeader->Ip.VersionIHL     = EDDS_IP_XRT_VER_IHL;
    pIPHeader->Ip.TOS            = EDDS_IP_XRT_TOS;
    pIPHeader->Ip.TotalLength    = EDDS_HTONS(Len);
    pIPHeader->Ip.Identification = 0;
    pIPHeader->Ip.FragmentOffset = EDDS_HTONS(EDDS_IP_DF);
    pIPHeader->Ip.TTL            = pCSRTComp->IPParams.TTL;
    pIPHeader->Ip.Protocol       = EDDS_IP_UDP;
    pIPHeader->Ip.Checksum       = 0;

    for (i=0; i<EDD_IP_ADDR_SIZE; i++)
    {
        pIPHeader->Ip.SrcIP.b.IP[i]  = pProv->SrcIP.b.IP[i];
        pIPHeader->Ip.DestIP.b.IP[i] = pProv->DstIP.IPAdr[i];
    }

    pIPHeader->Ip.Checksum = EDDS_IPChecksum((EDD_UPPER_MEM_U16_PTR_TYPE)&pIPHeader->Ip);

    Len -= sizeof(pIPHeader->Ip);

    /* insert UDP-header */
    pIPHeader->Udp.SrcPort       = EDDS_xRT_UDP_PORT;
    pIPHeader->Udp.DstPort       = EDDS_xRT_UDP_PORT;
    pIPHeader->Udp.Length        = EDDS_HTONS(Len);
    pIPHeader->Udp.Checksum      = 0; /* not used */

    /* insert Frame-ID */
    pIPHeader->FrameID           = EDDS_HTONS(pProv->FrameID);

    /* cyclecounter and datastatus will be inserted with every send */

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetupCycleStart              +*/
/*+  Input                 :    EDDS_LOCAL_DDB_PTR_TYPE pDDB                +*/
/*+                             EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets ProviderCycle variables to start a Provider Cycle.   +*/
/*+               After this is done a TransmitTrigger will cause providers +*/
/*+               to be send. If link is down we dont start the cycle.      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pPCtrl,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pPCtrl as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetupCycleStart(
    EDDS_LOCAL_DDB_PTR_TYPE                   pDDB,
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR     pPCtrl)
{

    EDDS_DDB_COMP_CSRT_TYPE_PTR          pCSRTComp;
    LSA_UINT32                           Mask,KnotIndex;

    /* we only start cycle if the link is up. If the link is down, we */
    /* dont send anything. This is for optimization because if the    */
    /* link is down sending lasts very long and causes overruns which */
    /* delays other handling (i.g. NRT).                              */

    pCSRTComp = &pDDB->pSRT->CSRT;

    if ( pDDB->pGlob->LinkStatus[0].Status == EDD_LINK_UP ) /* check link of AUTO -> if any port is UP */
    {

        Mask                       = pPCtrl->MinInterval-1;
        KnotIndex                  = (Mask) + ((pCSRTComp->Prov.ProviderCycle.CycleCounter) & Mask);

        /* if knot not used set ActProvIdx to SRT_CHAIN_IDX_END */
        if ( pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx == EDDS_SRT_IDX_NOT_USED )
        {
            pPCtrl->ActProvIdx     = EDDS_SRT_CHAIN_IDX_END;
        }
        else
        {
            if (pPCtrl->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
            {
                pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListXRT.BottomIdx;
            }
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            else /* udp */
            {
                pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListUDP.BottomIdx;
            }
            #endif
        }

        pPCtrl->ActInterval        = pPCtrl->MinInterval;
        pPCtrl->CycleDoSend        = LSA_TRUE;
        pPCtrl->CycleInProgress    = LSA_TRUE;
    }

    pPCtrl->CycleSendStarted   = LSA_FALSE;
}
//lint +esym(613,pPCtrl,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTGetProviderToSend                    +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR  pCSRTComp      +*/
/*+                             EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl +*/
/*+  Result                :    EDDS_CSRT_PROVIDER_TYPE_PTR                 +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp  : Pointer to CSRT Management                                +*/
/*+  pPCtrl     : Pointer to provider control structure                     +*/
/*+                                                                         +*/
/*+  Results    : LSA_NULL: No provider found to send                       +*/
/*+               else pointer to provider to send.                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Get next provider to send for the actual cycle.           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp, pPCtrl)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp or pPCtrl as null ptr's!
EDDS_STATIC EDDS_CSRT_PROVIDER_TYPE_PTR EDDS_LOCAL_FCT_ATTR EDDS_RTGetProviderToSend(
    EDDS_DDB_COMP_CSRT_TYPE_PTR               pCSRTComp,
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR     pPCtrl)
{

    LSA_BOOL                             Finished,Found;
    LSA_UINT32                           Mask,i,KnotIndex;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    EDDS_CSRT_CONSUMER_TYPE_PTR          pCons;
    EDDS_CSRT_CONSUMER_FRAME_TYPE_PTR    pConsFrame;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTGetProviderToSend(pCSRTComp: 0x%X, pPCtrl: 0x%X)",
                           pCSRTComp,
                           pPCtrl);

    pPCtrl->CycleSendStarted = LSA_TRUE;

    Found       = LSA_FALSE;
    Finished    = LSA_FALSE;
    pProv       = LSA_NULL;

    /* --------------------------------------------------------------------*/
    /* Search for next Provider to send or finished cycle.                 */
    /* --------------------------------------------------------------------*/

    while (( ! Found ) && ( ! Finished ))
    {

        /* Check the current SendList of Knot for more Provider to be send */
        /* If no more present we have to switch to the next knot..         */
        /* If one is present, we set Found to TRUE. and break the loop.    */

        if ( pPCtrl->ActProvIdx != EDDS_SRT_CHAIN_IDX_END )
        {

            pProv = &pCSRTComp->Prov.pProvTbl[pPCtrl->ActProvIdx];

            /* only if provider is active and correct type */
            if (pProv->Active)
            {
                /* must be the right provider !*/
                EDDS_ASSERT(pProv->Type != pPCtrl->Type);

                /* check if this provider has Autostop. If so we only send if */
                /* the Autostop is enabled and the Consumer is not in MISS    */

                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {
                    EDDS_ASSERT(pProv->AutoStopConsumerID >= pCSRTComp->Cons.ConsumerCnt);
                    pCons      = &pCSRTComp->Cons.pConsTbl[pProv->AutoStopConsumerID];
                    pConsFrame = &pCSRTComp->Cons.pConsFrameTbl[pProv->AutoStopConsumerID];

                    /* Autostop enabled within Consumer? (Consumer is in use!)*/
                    if ( pCons->ProviderAutoStopEnabled )
                    {
                        /* check if Consumer Active (and no MISS detected)  */
                        if (pConsFrame->State >= EDDS_SRT_CONS_STATE_ON_0)
                        {
                            Found = LSA_TRUE; /* No Provider Autostop */
                        }
                        else
                        {
                            /* Autostop. Dont send Provider if consumer not active or MISS detected */
                            EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                                       "EDDS_RTGetProviderToSend: Provider Autostop (ProvID: %d,ConsID: %d)",
                                       pPCtrl->ActProvIdx,
                                       pProv->AutoStopConsumerID);
                        }
                    }
                    else
                    {
                        Found = LSA_TRUE; /* Autostop not enabled */
                    }
                }
                else /* no autostop */
                {
                    Found = LSA_TRUE;
                }

                if ( Found )
                {
                    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                                          "EDDS_RTGetProviderToSend: Provider to send: %d",
                                          pPCtrl->ActProvIdx);
                }

            }

            pPCtrl->ActProvIdx = pProv->SendLink.Next;

        }
        else
        {
            /* Switch to next knot.                                          */
            /* If this is the last knot we finish the current cycle.         */
            /* by setting Finished to LSA_TRUE                               */
            /* Note that MaxInterval maybe < ActInterval because MaxInterval */
            /*      may have been changed while handling this. So dont use   */
            /*      a == to detect end of loop                               */

            if ( pPCtrl->ActInterval < pPCtrl->MaxInterval )
            {

                i           = pPCtrl->ActInterval;
                i           = i*2;
                Mask        = i-1;
                KnotIndex   = (Mask) + ((pCSRTComp->Prov.ProviderCycle.CycleCounter) & Mask);


                EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                                      "EDDS_RTGetProviderToSend: Switch to KnotIDX: %d, Int: %d, CyCnt: %d",
                                      KnotIndex,
                                      pPCtrl->ActInterval,
                                      pCSRTComp->Prov.ProviderCycle.CycleCounter);


                pPCtrl->ActInterval = (LSA_UINT16) i;

                /* if knot not used set ActProvIdx to SRT_CHAIN_IDX_END */
                if ( pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx == EDDS_SRT_IDX_NOT_USED )
                {
                    pPCtrl->ActProvIdx = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    if ( pPCtrl->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                        #endif
                    {
                        pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListXRT.BottomIdx;
                    }
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    else  /* udp */
                    {
                        pPCtrl->ActProvIdx = pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[KnotIndex].KnotElementIdx].ProviderSendListUDP.BottomIdx;
                    }
                    #endif
                }

            }
            else /* all done for this cycle. No sends pending */
            {
                pPCtrl->CycleDoSend = LSA_FALSE;

                /* if the counter is already 0 (maybe because no sends neccessary for   */
                /* this cycle) we reset the CycleInProgress, because no sends underway  */
                /* and we are ready. If sends are underwqy the CycleInProgress flag     */
                /* will be reset after all frames were sent (see TxReadyCyclic-function)*/

                if ( ! pPCtrl->ActTxInProgress )
                {
                    pPCtrl->CycleInProgress = LSA_FALSE;
                }
                Finished = LSA_TRUE;
            }
        }
    }

    if ( ! Found ) pProv = LSA_NULL;

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTGetProviderToSend(), pProv: 0x%X",
                           pProv);

    return(pProv);

}
//lint +esym(613,pCSRTComp, pPCtrl)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTTxQueryCyclicXRT                     +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present. (Cyclic SRT-Requests)                            +*/
/*+                                                                         +*/
/*+   Calculation of KnotTableIndex:                                        +*/
/*+                                                                         +*/
/*+   Interval     = 1;                                                     +*/
/*+   KnotIndex    = 0;                                                     +*/
/*+                                                                         +*/
/*+   while (Interval < MaxInterval)                                        +*/
/*+   {                                                                     +*/
/*+      HandleKnotSendlist(KnotIndex)                                      +*/
/*+      Interval = Interval * 2                                            +*/
/*+      Mask     = Interval -1                                             +*/
/*+      KnotIndex = (Interval-1) + ( CycleCounter & Mask)                  +*/
/*+   }                                                                     +*/
/*+                                                                         +*/
/*+   IMPORTANT:  Must be synchronized with remove-provider!! this function +*/
/*+               must not be interrupted by a remove-provider request.     +*/
/*+                                                                         +*/
/*+               ProviderRemove adjusts counter on remove!                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pLength,pBuffer,pDDB)
//JB 18/11/2014 out param - pDDB checked where called
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTTxQueryCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength)

{

    LSA_RESULT                           Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR          pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    LSA_UINT32                           i;
    LSA_UINT16                           Dummy;
    LSA_UINT8                            DataStatus;
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl;

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTTxQueryCyclicXRT(pDDB: 0x%X)",
                           pDDB);


    Status    = EDD_STS_OK_NO_DATA;
    pCSRTComp = &pDDB->pSRT->CSRT;
    pPCtrl    = &pCSRTComp->Prov.ProviderCycle.Xrt;

    if ( pPCtrl->CycleDoSend)
    {
		LSA_UINT16                     EDDS;
		EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;

        pProv = EDDS_RTGetProviderToSend(pCSRTComp,pPCtrl);

        /* --------------------------------------------------------------------*/
        /* If a provider is found to send. we determine the send buffer        */
        /* pProv points to the provider.                                       */
        /* --------------------------------------------------------------------*/

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pProv,LSA_NULL))
		{
			EDDS_EXCHANGE_TYPE Temp;

			// NOTE:
			// Information like index and new flag is always stored in one byte in
			// the EXCHANGE variables at its lowest addresses. This applies to EDDS and
			// IOBuffer when accessing these variables. Access to this byte by byte
			// pointer does not need any consideration of endianess.
			//
			// example:
			// Value stored in memory for Xchange with reference to buffer 1 and
			// new flag is 0x8100. Index and new flag can be retrieved by accessing
			// Xchange with its address casted to a byte pointer independently from
			// host endianess.
			//
			// index = (*((LSA_UINT8*)&Xchange)) & ~MASK

			pCr = &pCSRTComp->Prov.pProvCrTbl[pProv->CrNumber - 1];   // 1 .. ProviderCnt
			EDDS = *((LSA_UINT8*)&pCr->EDDS);

            /* The sendbuffer cannot be setup on provider add, because it ist */
            /* possible, that this buffer is currently sended. (very unlikely */
            /* because someone have to remove a provider and add a new one    */
            /* while the buffer is within the controller for send. So we setup*/
            /* the sendbuffer here, because we know it is not sended currently*/
            /* This setup is only done once after adding the provider.        */

            if (pProv->DoSetupBuffer )  /* must we setup the send-buffer ? */
            {
				EDDS_RTSetupProviderFrameXRT(pCr->pBuffer[EDDS],
                                             pProv->DataLen,
                                             pProv->DstMAC.MacAdr,
                                             pProv->FrameID,
                                             pProv->VLANID);

                pProv->DoSetupBuffer = LSA_FALSE;
            }

			Temp = pCr->XChange;

			if ( (*((LSA_UINT8*)&Temp)) & EDDS_IOBUFFER_MASK_NEW_FLAG )   /* new frame from user present ?   */
            {
				// NOTE: the content of Xchange may changed in the meantime, but not the new flag
				LSA_UINT8* pValue;

				Temp = pCr->EDDS;
				Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

				// reset new flag in Temp and set EDDS entry
				pValue    = ((LSA_UINT8*)&Temp);
				*pValue  &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
				pCr->EDDS = Temp;

				EDDS = *((LSA_UINT8*)&pCr->EDDS);
            }

            /* Now fill in DataStatus (Transferstatus is already set to 0)        */
            /* DataStatus consist of a global Provider State and a provider State */
            /* (will be filled even if no new buffer, because it may be changed)  */

            i = EDDS_CSRT_FRAME_IO_DATA_PROLOG_SIZE+pProv->DataLen;

            /* Insert Cycle-Counter. Based on 31.25us  */
            /* CycleCounter in Network-Byte order      */

            Dummy = ((LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter) << pCSRTComp->CycleStampAdjustShift));

            /* Tow accesses because of possible missalignment */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET]   = (LSA_UINT8) (Dummy >> 8);   /* CycleCounter   */  //lint !e835 JB 18/11/2014 def-based
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET+1] = (LSA_UINT8) (Dummy & 0xFF); /* CycleCounter  */  //lint !e835 JB 18/11/2014 def-based

            /* For calculation the proper setting for the data status we use  */
            /* the following formula.                                         */
            /*                                                                */
            /* DataStatus = ((A|B) & WinMask ) | (A&B))                       */
            /*                                                                */
            /* A: Provider specific state (set per provider)         8 Bit    */
            /* B: Provider global state   (set per provider group)   8 Bit    */
            /* WinMask : decides which value (0 or 1) wins           8 Bit    */
            /*           0: 0 wins                                            */
            /*           1: 1 wins                                            */

            DataStatus = ((( pProv->DataStatus | pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus ) & EDD_CSRT_DSTAT_WIN_MASK ) |
                          ( pProv->DataStatus & pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));

            /* DataStatus is located behind CycleCounter */

            ((EDD_UPPER_MEM_U8_PTR_TYPE )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_DATA_STATUS_OFFSET] = DataStatus;

            *pBuffer = pCr->pBuffer[EDDS];
            *pLength = i+EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;


            EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_RTTxQueryCyclicXRT: Provider to send: FrameID: 0x%X, Buffer: 0x%X, Len: %d",
                                  pProv->FrameID,
                                  *pBuffer,
                                  *pLength);

            /* Set FirstSend-Flag if not set yet and set flag for pending indic. */

            if ( ! pProv->FirstSend )
            {
                pProv->FirstSend                            = EDDS_PROV_FIRST_SEND;
                pCSRTComp->Indication.ProvFirstSendOccured  = LSA_TRUE;
                pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
            }

            #ifdef EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER
            pProv->SendCounter++;
            #endif

            pPCtrl->ActTxInProgress++;
            Status = EDD_STS_OK;


        }
    }

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTTxQueryCyclicXRT(*pBuffer: 0x%X, Length: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           Status);


    return(Status);

}
//lint +esym(613,pLength,pBuffer,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTTxReadyCyclicXRT                     +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after a provider was sent (XRT)                    +*/
/*+                                                                         +*/
/*+               We check if this was the last provider for this cycle.    +*/
/*+               if so we set CycleInProgress to FALSE.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as a null ptr!
//@fixme am2219 NEA6.0 function refactoring
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTTxReadyCyclicXRT(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_RESULT              TxStatus)
{

    LSA_RESULT                      Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRTComp;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTTxReadyCyclicXRT(pDDB: 0x%X, TxStatus: %d)",
                           pDDB,
                           TxStatus);

    Status = EDD_STS_OK;

    pCSRTComp = &pDDB->pSRT->CSRT;

    /* If Cycle in progress and no more sends pending and the */
    /* Tx-Counter reaches 0 we are ready for this cycle.      */

    if  ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress  )
    {
        pCSRTComp->Prov.ProviderCycle.Xrt.ActTxInProgress--;

        if (( ! pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend       ) &&
            ( ! pCSRTComp->Prov.ProviderCycle.Xrt.ActTxInProgress   ))
        {

            EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_RTTxReadyCyclicXRT: xRT Provider Cycle ready");

            pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress = LSA_FALSE;
        }
    }

    LSA_UNUSED_ARG(TxStatus);


    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTTxReadyCyclicXRT(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pDDB)


#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTTxQueryCyclicUDP                     +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             EDD_UPPER_MEM_PTR_TYPE ... *pBuffer         +*/
/*+                             EDDS_LOCAL_MEM_U32_PTR_TYPE    pLength      +*/
/*+                                                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pBuffer    : Address for pointer to tx-buffer (output)                 +*/
/*+  pLength    : Address for LSA_UINT32 of buffer length (output)          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_OK_NO_DATA           No pending tx-requests       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will look for pending tx-requests for this  +*/
/*+               device and returns a pointer to the tx-buffer if one is   +*/
/*+               present. (Cyclic SRT-Requests)                            +*/
/*+                                                                         +*/
/*+   Calculation of KnotTableIndex:                                        +*/
/*+                                                                         +*/
/*+   Interval     = 1;                                                     +*/
/*+   KnotIndex    = 0;                                                     +*/
/*+                                                                         +*/
/*+   while (Interval < MaxInterval)                                        +*/
/*+   {                                                                     +*/
/*+      HandleKnotSendlist(KnotIndex)                                      +*/
/*+      Interval = Interval * 2                                            +*/
/*+      Mask     = Interval -1                                             +*/
/*+      KnotIndex = (Interval-1) + ( CycleCounter & Mask)                  +*/
/*+   }                                                                     +*/
/*+                                                                         +*/
/*+   IMPORTANT:  Must be synchronized with remove-provider!! this function +*/
/*+               must not be interrupted by a remove-provider request.     +*/
/*+                                                                         +*/
/*+               ProviderRemove adjusts counter on remove!                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTTxQueryCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE                       pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR   *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                   pLength)

{

    LSA_RESULT                           Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR          pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR          pProv;
    LSA_UINT32                           i;
    LSA_UINT16                           Dummy;
    EDD_UPPER_MEM_PTR_TYPE               pTemp;
    LSA_UINT8                            DataStatus;
    EDDS_CSRT_PROV_CYCLE_CONTROL_TYPE_PTR pPCtrl;

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTTxQueryCyclicUDP(pDDB: 0x%X)",
                           pDDB);


    Status    = EDD_STS_OK_NO_DATA;
    pCSRTComp = &pDDB->pSRT->CSRT;
    pPCtrl    = &pCSRTComp->Prov.ProviderCycle.Udp;

    if ( pPCtrl->CycleDoSend)
    {
		LSA_UINT16                     EDDS;
		EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr;

        pProv = EDDS_RTGetProviderToSend(pCSRTComp,pPCtrl);

        /* --------------------------------------------------------------------*/
        /* If a provider is found to send. we determine the send buffer        */
        /* pProv points to the provider.                                       */
        /* --------------------------------------------------------------------*/

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pProv,LSA_NULL))
        {
			EDDS_EXCHANGE_TYPE Temp;

			pCr = &pCSRTComp->Prov.pProvCrTbl[pProv->CrNumber - 1];   // 1 .. ProviderCnt
			EDDS = *((LSA_UINT8*)&pCr->EDDS);

            /* The sendbuffer cannot be setup on provider add, because it is  */
            /* possible, that this buffer is currently sended. (very unlikely */
            /* because someone have to remove a provider and add a new one    */
            /* while the buffer is within the controller for send. So we setup*/
            /* the sendbuffer here, because we know it is not sended currently*/
            /* This setup is only done once after adding the provider.        */

            if (pProv->DoSetupBuffer )  /* must we setup the send-buffer ? */
            {
                EDDS_RTSetupProviderFrameUDP(pCSRTComp,
                                             pCr->pBuffer[EDDS],
                                             pProv);

                pProv->DoSetupBuffer = LSA_FALSE;
            }

			Temp = pCr->XChange;

			if ( (*((LSA_UINT8*)&Temp)) & EDDS_IOBUFFER_MASK_NEW_FLAG )   /* new frame from user present ?   */
            {
				LSA_UINT8* pValue;

				Temp = pCr->EDDS;
				Temp = EDDS_IOBUFFER_EXCHANGE(&pCr->XChange, Temp);

				// reset new flag in Temp and set EDDS entry
				pValue    = ((LSA_UINT8*)&Temp);
				*pValue  &= ~EDDS_IOBUFFER_MASK_NEW_FLAG;
				pCr->EDDS = Temp;

				EDDS = *((LSA_UINT8*)&pCr->EDDS);
            }

            /* Now fill in DataStatus (Transferstatus is already set to 0)        */
            /* DataStatus consist of a global Provider State and a provider State */
            /* (will be filled even if no new buffer, because it may be changed)  */

            i = EDDS_FRAME_HEADER_SIZE_WITH_VLAN +
                sizeof(EDDS_IP_HEADER_TYPE)  +
                sizeof(EDDS_UDP_HEADER_TYPE) +
                sizeof(EDDS_FRAME_ID_TYPE)   +
                pProv->DataLen;  /* start of APDU-Status within frame */

            /* Insert Cycle-Counter. Based on 1ms      */
            /* CycleCounter in Network-Byte order      */

            Dummy = ((LSA_UINT16) ((pCSRTComp->Prov.ProviderCycle.CycleCounter) << (pCSRTComp->CycleStampAdjustShift - 5) ));

            /* Tow accesses because of possible missalignment */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET]   = (LSA_UINT8) (Dummy >> 8);   /* CycleCounter   */
            ((EDD_UPPER_MEM_U8_PTR_TYPE  )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_CYCLE_COUNTER_OFFSET+1] = (LSA_UINT8) (Dummy & 0xFF); /* CycleCounter  */

            /* For calculation the proper setting for the data status we use  */
            /* the following formula.                                         */
            /*                                                                */
            /* DataStatus = ((A|B) & WinMask ) | (A&B))                       */
            /*                                                                */
            /* A: Provider specific state (set per provider)         8 Bit    */
            /* B: Provider global state   (set per provider group)   8 Bit    */
            /* WinMask : decides which value (0 or 1) wins           8 Bit    */
            /*           0: 0 wins                                            */
            /*           1: 1 wins                                            */

            DataStatus = ((( pProv->DataStatus | pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus ) & EDD_CSRT_DSTAT_WIN_MASK ) |
                          ( pProv->DataStatus & pCSRTComp->Prov.pProvGroupTbl[pProv->GroupID].ProviderDataStatus));

            /* DataStatus is located behind CycleCounter */

            ((EDD_UPPER_MEM_U8_PTR_TYPE )pCr->pBuffer[EDDS])[i+EDDS_CSRT_FRAME_DATA_STATUS_OFFSET] = DataStatus;

            *pBuffer = pCr->pBuffer[EDDS];
            *pLength = i+EDDS_CSRT_FRAME_IO_DATA_EPILOG_SIZE;

            EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                                  "EDDS_RTTxQueryCyclicUDP: Provider to send: FrameID: 0x%X, Buffer: 0x%X, Len: %d",
                                  pProv->FrameID,
                                  *pBuffer,
                                  *pLength);


            /* Set FirstSend-Flag if not set yet and set flag for pending indic. */

            if ( ! pProv->FirstSend )
            {
                pProv->FirstSend                            = EDDS_PROV_FIRST_SEND;
                pCSRTComp->Indication.ProvFirstSendOccured  = LSA_TRUE;
                pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
            }

            #ifdef EDDS_CSRT_PROVIDER_INCLUDE_SEND_COUNTER
            pProv->SendCounter++;
            #endif

            pPCtrl->ActTxInProgress++;
            Status = EDD_STS_OK;


        }
    }


    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTTxQueryCyclicUDP(*pBuffer: 0x%X, Length: %d), Status: 0x%X",
                           *pBuffer,
                           *pLength,
                           Status);


    return(Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTTxReadyCyclicUDP                     +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE        pDDB         +*/
/*+                             LSA_RESULT                     TxStatus     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  TxStatus   : tx-status of "inprogress" tx. put into RQB                +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Called after a provider was sent (xRToverUDP)             +*/
/*+                                                                         +*/
/*+               We check if this was the last provider for this cycle.    +*/
/*+               if so we set CycleInProgress to FALSE.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTTxReadyCyclicUDP(
    EDDS_LOCAL_DDB_PTR_TYPE pDDB,
    LSA_RESULT              TxStatus)
{

    LSA_RESULT                      Status;
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRTComp;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTTxReadyCyclicUDP(pDDB: 0x%X, TxStatus: %d)",
                           pDDB,
                           TxStatus);


    Status = EDD_STS_OK;

    pCSRTComp = &pDDB->pSRT->CSRT;

    /* If Cycle in progress and no more sends pending and the */
    /* Tx-Counter reaches 0 we are ready for this cycle.      */

    if  ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress  )
    {
        pCSRTComp->Prov.ProviderCycle.Udp.ActTxInProgress--;

        if (( ! pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend       ) &&
            ( ! pCSRTComp->Prov.ProviderCycle.Udp.ActTxInProgress   ))
        {
            EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_RTTxReadyCyclicXRT: UDP Provider Cycle ready");

            pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress = LSA_FALSE;

        }
    }

    LSA_UNUSED_ARG(TxStatus);

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTTxReadyCyclicUDP(), Status: 0x%X",
                           Status);

    return(Status);

}
#endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTProviderTimerCbf                   +*/
/*+  Input/Output          :    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB     +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to Internal RQB used for this timer event.        +*/
/*+               (this is a pointer to ProvTimerRQB in provider mgmgent)   +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Provider Cycle-Timer Callbackfunction.                    +*/
/*+               Called after the Cycletimer elapsed.                      +*/
/*+                                                                         +*/
/*+               We do trigger the output MACRO                            +*/
/*+               EDDS_DO_PROVIDER_TIMER_REQUEST()                          +*/
/*+               if not already underway.                                  +*/
/*+                                                                         +*/
/*+               Further we check if some indications for the provider are +*/
/*+               recognized and should be indicated. if so we trigger the  +*/
/*+               EDDS_DO_PROVIDER_IND_REQUEST() macro.                     +*/
/*+                                                                         +*/
/*+               (Provider indications will only be checked cyclic)        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pIntRQB)
//JB 05/12/2014 cannot be a null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTProviderTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB)
{

    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRTComp;

    pDDB        = ((EDDS_UPPER_DEV_INTERNAL_PTR_TYPE) pIntRQB->pRQB->pParam)->hDDB;
    pCSRTComp   = &pDDB->pSRT->CSRT;

    if ( ! pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup!     */
        pIntRQB->InUse = LSA_TRUE;
        EDDS_DO_PROVIDER_TIMER_REQUEST(pIntRQB->pRQB);
    }
    else
    {
        /* we are not able to handle the timer request since last */
        /* timer occurance. we drop the event an set a statistic  */
        /* counter                                                */


        EDDS_CRT_TRACE_00(0,EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL,
                              "EDDS_CSRTProviderTimerCbf: Timer overload occured!");


        pCSRTComp->Stats.ProvCycleCounterDropCnt++;

        /* indicate only if providers active */

        #ifndef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING
        if ( pCSRTComp->Prov.ProviderActCnt)
        {
            pCSRTComp->Indication.ProvOverloadOccured   = LSA_TRUE;
            pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
        }
        #endif

    }

    /* ----------------------------------------------------------------------*/
    /* check if some provider indications pending and have to be             */
    /* indicated. if so we trigger the provider indiciation check            */
    /* if indication resources present.                                      */
    /* ----------------------------------------------------------------------*/

    if  (( pCSRTComp->Indication.ProvIndicationPending )    &&
         ( ! LSA_HOST_PTR_ARE_EQUAL(pCSRTComp->Indication.RqbQueue.pBottom,LSA_NULL)))
    {
        if ( ! pCSRTComp->ProvIndTriggerRQB.InUse )
        {
            pCSRTComp->ProvIndTriggerRQB.InUse = LSA_TRUE;
            EDDS_DO_PROVIDER_IND_REQUEST(pCSRTComp->ProvIndTriggerRQB.pRQB);
        }
    }

}
//lint +esym(613,pIntRQB)

#endif  /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CSRTProviderCycleRequest               +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Provider Cycle-Timer function.                            +*/
/*+               This function is called after a provider-timer-request    +*/
/*+               occured (triggered by EDDS_DO_PROVIDER_TIMER_REQUEST)     +*/
/*+                                                                         +*/
/*+               This request will start a new Provider send cycle.        +*/
/*+               We check if we are still in cycle-handling. if so         +*/
/*+               we have an overload an stop the actual cycle and set      +*/
/*+               and overload indication marker.                           +*/
/*+                                                                         +*/
/*+               On Overload we check if we should drop cycles and do it.  +*/
/*+                                                                         +*/
/*+               if no overload (and no drop sequence in progress) we will +*/
/*+               start a new provider cycle by setting a flag and trigger  +*/
/*+               transmission.                                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING be careful when using this function, make sure not to use pDDB as null ptr
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CSRTProviderCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRTComp;

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CSRTProviderCycleRequest(pDDB: 0x%X)",
                           pDDB);


    pCSRTComp = &pDDB->pSRT->CSRT;

    #ifndef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING
    pCSRTComp->Prov.ProviderCycle.CycleCounter++;
    #endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    pCSRTComp->ProvTimerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    /*---------------------------------------------------------------------------*/
    /* if we are curently in OverloadDrop we only increment the cycleCounter and */
    /* continue..                                                                */
    /*---------------------------------------------------------------------------*/

    if ( pCSRTComp->Prov.ProviderCycle.OverloadDropCycles )
    {

        EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,
                              "EDDS_CSRTProviderCycleRequest: Dropping this cycle because of overload!");

        pCSRTComp->Prov.ProviderCycle.OverloadDropCycles--;
    }
    else
    {
        /* ----------------------------------------------------------------------*/
        /* We check if Cycle is in progress. if so we have an overload situation */
        /* We cancel the actual cycle and set the overload flag for indication   */
        /* ----------------------------------------------------------------------*/

        if (   ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress )
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
               || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress )
            #endif
           )
        {

            EDDS_CRT_TRACE_01(0,EDDS_CFG_CSRT_OVERLOAD_TRACE_LEVEL,
                                  "EDDS_CSRTProviderCycleRequest: Overload! Cycle not ready. (pDDB: 0x%X)",
                                  pDDB);

            /* indicate only if providers active */

            #ifndef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING
            if ( pCSRTComp->Prov.ProviderActCnt)
            {
                pCSRTComp->Indication.ProvOverloadOccured   = LSA_TRUE;
                pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
                /* additional cycles to drop */
                pCSRTComp->Prov.ProviderCycle.OverloadDropCycles = pCSRTComp->Cfg.ProviderCycleDropCnt;
            }
            #endif


            pCSRTComp->Stats.ProvCycleOverloadCnt++; /* statistics */

        }
        else /* no overload and provider cycle not running. good situation */
        {

            #ifdef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING
            pCSRTComp->Prov.ProviderCycle.CycleCounter++;
            #endif

            EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "EDDS_CSRTProviderCycleRequest: Starting provider cycle (0x%X)",
                                  pCSRTComp->Prov.ProviderCycle.CycleCounter);


            EDDS_RTProviderSetupCycleStart(pDDB,&pCSRTComp->Prov.ProviderCycle.Xrt);
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            EDDS_RTProviderSetupCycleStart(pDDB,&pCSRTComp->Prov.ProviderCycle.Udp);
            #endif
            EDDS_TransmitTrigger(pDDB); /* Trigger a transmission for this dev.*/
            /* This cause a query for this all     */
            /* components. even for this provider. */

            #ifdef EDDS_CFG_SEND_CYCLE_BASED

            /* Do some error checks with Cycle based send mode...            */

            if (   ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress )
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                   || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress )
                #endif
               )
            {

                /* if we cant start the cycle, because tx still underway (this   */
                /* may occur, if we have no provider active but long NRT-Frames  */
                /* still underway) we have a overload situation because we cant  */
                /* start sending. But only with cycle based send-mode,           */
                /* because we only start sending at cycle start (and cant here)  */
                /* with other modes this doesnt matter, because the cycle was    */
                /* still ready and send is triggered within nrt. This situation  */
                /* may occur in 10Mbit Mode if we send 1500Byte NRT-Frames. This */
                /* lasts about 1.5ms. With this we will delay till next cycle... */

                /* Note. XRT is sendet first, so we only need to check for XRT   */
                if ( ! pCSRTComp->Prov.ProviderCycle.Xrt.CycleSendStarted )
                {



                    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                          "EDDS_CSRTProviderCycleRequest: Overload! Old tx still underway (pDDB: 0x%X)",
                                          pDDB);

                    /* stop this cycle, because we couldnt start it */
                    pCSRTComp->Prov.ProviderCycle.Xrt.CycleInProgress = LSA_FALSE;
                    pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend     = LSA_FALSE;

                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    /* stop this cycle, because we couldnt start it */
                    pCSRTComp->Prov.ProviderCycle.Udp.CycleInProgress = LSA_FALSE;
                    pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend     = LSA_FALSE;
                    #endif

                    #ifndef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING
                    /* indicate only if providers active */
                    if ( pCSRTComp->Prov.ProviderActCnt)
                    {
                        pCSRTComp->Indication.ProvOverloadOccured   = LSA_TRUE;
                        pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
                        /* additional cycles to drop */
                        pCSRTComp->Prov.ProviderCycle.OverloadDropCycles = pCSRTComp->Cfg.ProviderCycleDropCnt;
                    }
                    #endif

                    pCSRTComp->Stats.ProvCycleOverloadCnt++; /* statistics */

                }
                else
                {
                    /* if we started sending, but dont be able to send all       */
                    /* cyclic frames at once we dont have enough tx-resources in */
                    /* lower layer. This is fatal.                               */

                    if (    ( pCSRTComp->Prov.ProviderCycle.Xrt.CycleDoSend )
                        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                            || ( pCSRTComp->Prov.ProviderCycle.Udp.CycleDoSend )
                        #endif
                       )
                    {

                        EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,
                                              "EDDS_CSRTProviderCycleRequest: To less Tx-Descriptors! (pDDB: 0x%X)",
                                              pDDB);

                        EDDS_FatalError(EDDS_FATAL_ERR_TX_RESOURCES,
                                        EDDS_MODULE_ID,
                                        __LINE__);

                    }
                }
            }
            #endif

        }
    }

    EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CSRTProviderCycleRequest()");

}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderCheckIndication(pDDB)        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Provider indication check function                        +*/
/*+               This function looks for all pending provider indications  +*/
/*+               and try to indicate them to the user.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderCheckIndication(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{

    EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp;
    LSA_BOOL                                IndResourcePresent;

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderCheckIndication(pDDB: 0x%X)",
                           pDDB);

    pCSRTComp           = &pDDB->pSRT->CSRT;

    IndResourcePresent  = LSA_TRUE;

    if (pCSRTComp->Indication.ProvIndicationPending )
    {
        pCSRTComp->Indication.ProvIndicationPending = LSA_FALSE;

        /*------------------------------------------------------------------------*/
        /* Check for overload error. This is a provider-global error.             */
        /*------------------------------------------------------------------------*/

        if ( pCSRTComp->Indication.ProvOverloadOccured )
        {

            if ( !  EDDS_CSRTIndicationFill( pCSRTComp,
                                             0,
                                             EDD_CSRT_PROV_EVENT_DROP,
                                             0, /* EventStatus always zero in provider */
                                             0))
            {
                IndResourcePresent = LSA_FALSE;
            }
            else pCSRTComp->Indication.ProvOverloadOccured = LSA_FALSE;
        }

        /* --------------------------------------------------------------------------*/
        /* Finish the indication request handling. This will indicate the rqb if     */
        /* something is filled.                                                      */
        /* --------------------------------------------------------------------------*/
		 
        EDDS_CSRTIndicationFinish(pCSRTComp);

        /* if we have not indicated all status we the pending flag again. */

        if ( ! IndResourcePresent ) pCSRTComp->Indication.ProvIndicationPending = LSA_TRUE;
    }


    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderCheckIndication(Pending?: %d)",
                           pCSRTComp->Indication.ProvIndicationPending);

}
//lint +esym(613,pDDB)

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderCheckIndRequest(pDDB)        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Provider Indication Check request                         +*/
/*+               This function is called after a EDDS_SRV_DEV_TRIGGER      +*/
/*+               request with EDDS_TRIGGER_PROV_INDICATION occured         +*/
/*+               This function looks for all pending provider indications  +*/
/*+               and try to indicate them to the user.                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 18/11/2014 checked where called
//WARNING: be careful when calling this function, make sure not to use pDDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderCheckIndRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB)
{


    pDDB->pSRT->CSRT.ProvIndTriggerRQB.InUse = LSA_FALSE; /* RQB is free for further use now*/

    EDDS_RTProviderCheckIndication(pDDB);


}
//lint +esym(613,pDDB)
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname       :    EDDS_RTProviderGetMaxUsed                      +*/
/*+  Input/Output       :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp          +*/
/*+                          LSA_UINT32                  CycleReductionRatio+*/
/*+                          LSA_UINT32                  CyclePhase         +*/
/*+                          EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxByteCnt        +*/
/*+                          EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxCnt            +*/
/*+                                                                         +*/
/*+  Result             :    LSA_BOOL                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to CSRTManagment                         +*/
/*+  CycleReductionRatio : CycleReductionRatio (1,2,4,8,16..512)            +*/
/*+  CyclePhase          : CyclePhase (1..CycleReductionRatio)              +*/
/*+  pMaxByteCnt         : Maximum provider bytes already present           +*/
/*+  pMaxCnt             : Maximum providers already present                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates the already used provider byte count and       +*/
/*+               providers which are sended together with the provider     +*/
/*+               within CycleReductionRation/CyclePhase in worst case.     +*/
/*+                                                                         +*/
/*+               Because of the 2er multiple of reduction ratios, it is    +*/
/*+               fixed, which phases from other reduction ratios may be    +*/
/*+               send together with the specified ReductionRatio/Phase     +*/
/*+               at a spezified cycle                                      +*/
/*+                                                                         +*/
/*+ Example (till max ratio of 128)                                         +*/
/*+                                                                         +*/
/*+ CycleReductionRatio = 8                                                 +*/
/*+ CyclePhase          = 3                                                 +*/
/*+                                                                         +*/
/*+                    CycleReductionRatio                                  +*/
/*+               below <-   V  -> above                                    +*/
/*+                                                                         +*/
/*+     1     2      4       8        16       32       64       128   .... +*/
/*+ 1   x     x      -       -        -        -        -        -          +*/
/*+ 2         -      -       -        -        -        -        -          +*/
/*+ 3                x       x        x        x        x        x          +*/
/*+ 4                -       -        -        -        -        -          +*/
/*+ 5                        -        -        -        -        -          +*/
/*+ 6                        -        -        -        -        -          +*/
/*+ 7                        -        -        -        -        -          +*/
/*+ 8                        -        -        -        -        -          +*/
/*+ 9   \------v-------------/        -        -        -        -          +*/
/*+10                                 -        -        -        -          +*/
/*+11      always the same            x        x        x        x          +*/
/*+12        phases used              -        -        -        -          +*/
/*+13                                 -        -        -        -          +*/
/*+14                                 -        -        -        -          +*/
/*+15                                 -        -        -        -          +*/
/*+16                                 -        -        -        -          +*/
/*+17                                          -        -        -          +*/
/*+18                                          -        -        -          +*/
/*+19                                          x        x        x          +*/
/*+20                                          -        -        -          +*/
/*+21                                          -        -        -          +*/
/*+22                                          -        -        -          +*/
/*+23                                          -        -        -          +*/
/*+:                                           :        :        :          +*/
/*+128                                                                      +*/
/*+                                                                         +*/
/*+                                   \-----------------v----------------/  +*/
/*+                                           several combinations of       +*/
/*+                                               phases to test            +*/
/*+                                                                         +*/
/*+                  possible combinations of phases:                       +*/
/*+                                                                         +*/
/*+ Ratio:    1.2.3.4        8              16.32.64.128                    +*/
/*+                                                                         +*/
/*+ 1         01.01.03      03              03.03.03.03                     +*/
/*+ 2         01.01.03      03              11.11.11.11                     +*/
/*+ 3         01.01.03      03              03.19.19.19                     +*/
/*+ 4         01.01.03      03              11.27.27.27                     +*/
/*+ 5         01.01.03      03              03.03.35.35                     +*/
/*+ 6         01.01.03      03              11.11.43.43                     +*/
/*+ 7         01.01.03      03              03.19.51.51                     +*/
/*+ 8         01.01.03      03              11.27.59.59                     +*/
/*+ 9         01.01.03      03              03.03.03.67                     +*/
/*+ 10        01.01.03      03              11.11.11.75                     +*/
/*+ 11        01.01.03      03              03.19.19.83                     +*/
/*+ 12        01.01.03      03              11.27.27.91                     +*/
/*+ 13        01.01.03      03              03.03.35.99                     +*/
/*+ 14        01.01.03      03              11.11.43.107                    +*/
/*+ 15        01.01.03      03              03.19.51.115                    +*/
/*+ 16        01.01.03      03              11.27.59.123                    +*/
/*+                                                                         +*/
/*+  ^        \------------------v---------------------/                    +*/
/*+  |                Phases used for combination                           +*/
/*+  |                                                                      +*/
/*+  +----  EDDS_CSRT_Cycle_REDUCTION_RATIO_MAX/CycleReductionRatio         +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  As we can see the phases from reduction ratios below the actual one    +*/
/*+  is a fixed one and will not change. For the ratio above we have 2      +*/
/*+  possible phases with each ratio-step.                                  +*/
/*+                                                                         +*/
/*+  We use the following management with each ratio/phase (=knot)          +*/
/*+                                                                         +*/
/*+  Store                                                                  +*/
/*+                                                                         +*/
/*+  - provider byte count                                                  +*/
/*+  - provider count                                                       +*/
/*+  - provider byte count of max above this ratio 1)                       +*/
/*+  - provider count of max above this ratio      1)                       +*/
/*+                                                                         +*/
/*+  1) this is the combination of ratio/phases which represents the worst  +*/
/*+     case.                                                               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+ *Calculating the used max provider cnt/byte cnt:                        +*/
/*+                                                                         +*/
/*+  To calculate the actual max provider byte count or provider count we   +*/
/*+  have to sum up all values below the actual ratio/phase (which is       +*/
/*+  only one combination, as shown above), the actual value and the        +*/
/*+  stored max-value.                                                      +*/
/*+                                                                         +*/
/*+  For Ratio 512: We have 10* 2 adds (* 2 because of byte cnt and cnt)    +*/
/*+  For Ratio   1: We have 1 * 2 adds (* 2 because of byte cnt and cnt)    +*/
/*+                                                                         +*/
/*+  In addition we have a loop to find the phases within ratios below.     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+ *Adding a new provider and updating the values:                         +*/
/*+                                                                         +*/
/*+  If we add a new provider we only have to update the max-values of      +*/
/*+  the ratios/phases of ratios below the actual one.                      +*/
/*+                                                                         +*/
/*+  Example:                                                               +*/
/*+                                                                         +*/
/*+  If we add a provider to ratio 8 phase 3 we have to look within ratio 4 +*/
/*+  into the phase which may use phase 3 of ratio 8 if the max value       +*/
/*+  changed. So we have to look into ratio 4 phase 3 and look if the       +*/
/*+  max value changed. So we have to decide by calculating the max value   +*/
/*+  of ratio 8 phase 3 and ratio 8 phase 7 (only these two phases may be   +*/
/*+  used together with ratio 4 phase 3)                                    +*/
/*+                                                                         +*/
/*+  After updating ratio 4 / phase 3, we have to do the same procedure     +*/
/*+  with ratio 2 and so on:                                                +*/
/*+                                                                         +*/
/*+   1      2      4        8                                              +*/
/*+                                                                         +*/
/*+   x -+-> x -+-> o        -                                              +*/
/*+      +-> -  |   -        -                                              +*/
/*+             +-> x -+-->  x                                              +*/
/*+                 -  |     -                                              +*/
/*+                    |     -                                              +*/
/*+                    |     -                                              +*/
/*+                    +-->  o                                              +*/
/*+                          -                                              +*/
/*+                                                                         +*/
/*+   As we can see we have to do two decitions with every ratio till we    +*/
/*+   reached ratio 1.                                                      +*/
/*+                                                                         +*/
/*+   So we have to calculate:                                              +*/
/*+                                                                         +*/
/*+   Adding the max values of two phases (2*2 Adds) and two compares       +*/
/*+   (byte and byte count)                                                 +*/
/*+                                                                         +*/
/*+   For Ratio 512: We have 9 * 2 adds and one compare                     +*/
/*+   For Ratio   1: We have noting to do                                   +*/
/*+                                                                         +*/
/*+   NOTE: The max provider byte count and provider count may have         +*/
/*+         different combinations of ratios/phases used!                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp,pMaxByteCnt,pMaxCnt)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use any null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetMaxUsed(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    LSA_UINT32                  CycleReductionRatio,
    LSA_UINT32                  CyclePhase,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxByteCnt,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxCnt)
{


    LSA_UINT32      ActRatio0; /* Actual Ratio counting from 0 (0,1,3,7,15,31,63...) */
    LSA_UINT32      ActPhase0; /* Actual Phase counting from 0..(Ratio-1) */
    LSA_UINT32      Phase0;    /* Phase counting from 0..(Ratio-1) */
    LSA_UINT32      MaxCnt;
    LSA_UINT32      MaxByteCnt;
    LSA_BOOL        Finished;
    LSA_UINT16      Idx;
    EDDS_CSRT_KNOT_TYPE_PTR           pKnotTbl;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR   pKnotElementTbl;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetMaxUsed(Ratio: %d, Phase: %d)",
                           CycleReductionRatio,
                           CyclePhase);


    pKnotTbl         = pCSRTComp->Knot.pKnotTbl;
    pKnotElementTbl  = pCSRTComp->Knot.pKnotElementTbl;

    ActRatio0  = CycleReductionRatio-1;
    Phase0     = CyclePhase-1;

    /* --------------------------------------------------------------------------*/
    /* Get the max-values for the ratios above CycleReductionRatio               */
    /* --------------------------------------------------------------------------*/

    MaxCnt     = pKnotTbl[ActRatio0+Phase0].ProviderCntAboveMax;
    MaxByteCnt = pKnotTbl[ActRatio0+Phase0].ProviderByteCntAboveMax;

    /* --------------------------------------------------------------------------*/
    /* For all ratios from CycleReductionRatio to Ratio 1 we have only one phase */
    /* to use so we always have fixed max-values here and can calculate it ones  */
    /* --------------------------------------------------------------------------*/

    Finished   = LSA_FALSE;

    while ( ! Finished )
    {
        /* Phase to use for this Ratio. ActRatio0 is used to mask the Phase0 */
        /* for this ratio.                                                   */

        ActPhase0   = Phase0 & ActRatio0;
        Idx         = pKnotTbl[ActRatio0+ActPhase0].KnotElementIdx;

        if ( Idx != EDDS_SRT_IDX_NOT_USED )
        {
            MaxByteCnt += pKnotElementTbl[Idx].ProviderByteCnt;
            MaxCnt     += pKnotElementTbl[Idx].ProviderCnt;
        }

        if ( ActRatio0)
            ActRatio0 >>= 1; /* next ratio below */
        else
            Finished = LSA_TRUE;
    }


    *pMaxByteCnt = MaxByteCnt;
    *pMaxCnt     = MaxCnt;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                          "EDDS_RTProviderGetMaxUsed: MaxByteCnt: 0x%X, MaxCnt: 0x%X",
                          *pMaxByteCnt,
                          *pMaxCnt);

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetMaxUsed(MaxByteCnt: 0x%X, MaxCnt: 0x%X)",
                           *pMaxByteCnt,
                           *pMaxCnt);

}
//lint +esym(613,pCSRTComp,pMaxByteCnt,pMaxCnt)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname       :    EDDS_RTProviderSetMaxAbove                     +*/
/*+  Input/Output       :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp          +*/
/*+                          LSA_UINT32                  CycleReductionRatio+*/
/*+                          LSA_UINT32                  CyclePhase         +*/
/*+                          EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxByteCnt        +*/
/*+                          EDDS_LOCAL_MEM_U32_PTR_TYPE pMaxCnt            +*/
/*+                                                                         +*/
/*+  Result             :    LSA_BOOL                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to CSRTManagment                         +*/
/*+  CycleReductionRatio : CycleReductionRatio (1,2,4,8,16..512)            +*/
/*+  CyclePhase          : CyclePhase (1..CycleReductionRatio)              +*/
/*+  pMaxByteCnt         : Maximum provider bytes already present           +*/
/*+  pMaxCnt             : Maximum providers already present                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculates the max. above values of provider byte cnt and +*/
/*+               provider cnt for CycleReductionRatio/Phase.               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+   As we can see we have to do two decitions with every ratio till we    +*/
/*+   reached ratio 1.                                                      +*/
/*+                                                                         +*/
/*+   1      2      4        8    ......                                    +*/
/*+                                                                         +*/
/*+   x -+-> x -+-> o        -                                              +*/
/*+      +-> -  |   -        -                                              +*/
/*+             +-> x -+-->  x                                              +*/
/*+                 -  |     -                                              +*/
/*+                    |     -                                              +*/
/*+                    |     -                                              +*/
/*+                    +-->  o                                              +*/
/*+                          -                                              +*/
/*+                                                                         +*/
/*+   So we have to calculate:                                              +*/
/*+                                                                         +*/
/*+   Adding the max values of two phases (2*2 Adds) and two compares       +*/
/*+   (byte and byte count)                                                 +*/
/*+                                                                         +*/
/*+   For Ratio 512: We have 9 * 2 adds and one compare                     +*/
/*+   For Ratio   1: We have noting to do                                   +*/
/*+                                                                         +*/
/*+   NOTE: The max provider byte count and provider count may have         +*/
/*+         different combinations of ratios/phases used!                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMaxAbove(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    LSA_UINT32                  CycleReductionRatio,
    LSA_UINT32                  CyclePhase)
{


    LSA_UINT32      ActRatio0;   /* Actual Ratio counting from 0 (0,1,3,7,15,31,63....) */
    LSA_UINT32      ActPhase0;   /* Actual Phase counting from 0..(Ratio-1) */
    LSA_UINT32      AboveRatio0; /* Actual Ratio counting from 0 (0,1,3,7,15,31,63....) */
    LSA_UINT32      Phase0;      /* Phase counting from 0..(Ratio-1) */
    LSA_UINT32      Max1;
    LSA_UINT32      Max2;
    LSA_UINT32      HelpIdx;
    EDDS_CSRT_KNOT_TYPE_PTR           pKnotTbl;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR   pKnotElementTbl;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderSetMaxAbove(Ratio: %d, Phase: %d)",
                           CycleReductionRatio,
                           CyclePhase);


    pKnotTbl         = pCSRTComp->Knot.pKnotTbl;
    pKnotElementTbl  = pCSRTComp->Knot.pKnotElementTbl;

    ActRatio0  = CycleReductionRatio-1;
    Phase0     = CyclePhase-1;

    while ( ActRatio0 ) /* till we reached ratio 1 (ActRatio0 = 0) */
    {

        AboveRatio0  = ActRatio0;
        ActRatio0  >>= 1; /* next ratio below */

        ActPhase0   = Phase0 & ActRatio0;  /* 0 base phase within this ratio */

        HelpIdx = AboveRatio0+ActPhase0;

        /* the first phase */
        if ( pKnotTbl[HelpIdx].KnotElementIdx != EDDS_SRT_IDX_NOT_USED )
        {
            Max1 = pKnotElementTbl[pKnotTbl[HelpIdx].KnotElementIdx].ProviderByteCnt +
                   pKnotTbl[HelpIdx].ProviderByteCntAboveMax;
        }
        else
        {
            Max1 = pKnotTbl[HelpIdx].ProviderByteCntAboveMax;
        }

        /* the second phase */
        if ( pKnotTbl[HelpIdx+ActRatio0+1].KnotElementIdx != EDDS_SRT_IDX_NOT_USED )
        {
            Max2 = pKnotElementTbl[pKnotTbl[HelpIdx+ActRatio0+1].KnotElementIdx].ProviderByteCnt +
                   pKnotTbl[HelpIdx+ActRatio0+1].ProviderByteCntAboveMax;
        }
        else
        {
            Max2 = pKnotTbl[HelpIdx+ActRatio0+1].ProviderByteCntAboveMax;
        }

        if ( Max1 > Max2 )
        {
            pKnotTbl[ActRatio0+ActPhase0].ProviderByteCntAboveMax = (LSA_UINT16) Max1;
        }
        else
        {
            pKnotTbl[ActRatio0+ActPhase0].ProviderByteCntAboveMax = (LSA_UINT16) Max2;
        }


        /* the first phase */
        if ( pKnotTbl[HelpIdx].KnotElementIdx != EDDS_SRT_IDX_NOT_USED )
        {
            Max1 = pKnotElementTbl[pKnotTbl[HelpIdx].KnotElementIdx].ProviderCnt +
                   pKnotTbl[HelpIdx].ProviderCntAboveMax;
        }
        else
        {
            Max1 = pKnotTbl[HelpIdx].ProviderCntAboveMax;
        }

        /* the second phase */
        if ( pKnotTbl[HelpIdx+ActRatio0+1].KnotElementIdx != EDDS_SRT_IDX_NOT_USED )
        {
            Max2 = pKnotElementTbl[pKnotTbl[HelpIdx+ActRatio0+1].KnotElementIdx].ProviderCnt +
                   pKnotTbl[HelpIdx+ActRatio0+1].ProviderCntAboveMax;
        }
        else
        {
            Max2 = pKnotTbl[HelpIdx+ActRatio0+1].ProviderCntAboveMax;
        }

        if ( Max1 > Max2 )
        {
            pKnotTbl[ActRatio0+ActPhase0].ProviderCntAboveMax = (LSA_UINT16) Max1;
        }
        else
        {
            pKnotTbl[ActRatio0+ActPhase0].ProviderCntAboveMax = (LSA_UINT16) Max2;
        }


        EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                              "EDDS_RTProviderSetMaxAbove: New Max: Byte: %d, Cnt: %d",
                              (pKnotTbl[ActRatio0+ActPhase0].ProviderByteCntAboveMax),
                              (pKnotTbl[ActRatio0+ActPhase0].ProviderCntAboveMax));

    }


    EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderSetMaxAbove()");

}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname     :    EDDS_RTProviderGetFreeHard                       +*/
/*+  Input/Output     :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp            +*/
/*+                        LSA_UINT32                  CycleReductionRatio  +*/
/*+                        LSA_UINT32                  CyclePhase           +*/
/*+                        EDDS_LOCAL_MEM_U32_PTR_TYPE pProvBytesFree       +*/
/*+                        EDDS_LOCAL_MEM_U32_PTR_TYPE pProvCntFree         +*/
/*+                                                                         +*/
/*+  Result           :    LSA_BOOL                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to CSRTManagment                         +*/
/*+  CycleReductionRatio : CycleReductionRatio (1..)                        +*/
/*+  CyclePhase          : Cycle Phase (1..CycleReductionRatio)             +*/
/*+  pProvBytesFree      : Free Provider Bytes for this Cycle returned      +*/
/*+  pProvCntFree        : Free Provider Cnt   for this Cycle returned      +*/
/*+                                                                         +*/
/*+  Result              : LSA_TRUE if we have room                         +*/
/*+                        LSA_FALSE  if BytesFree and/or CntFree is 0      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Returns the free room for providers within the spezified  +*/
/*+               reduction ration for hard-limit.                          +*/
/*+                                                                         +*/
/*+               This does include the already used provider bytes         +*/
/*+               and count within this ratio/phase!                        +*/
/*+                                                                         +*/
/*+               If we have no limit we return 0xFFFFFFFF.                 +*/
/*+                                                                         +*/
/*+               If we have no room (free bytes or/and cnt reached 0) we   +*/
/*+               return FALSE                                              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pProvBytesFree,pProvCntFree)
//JB 18/11/2014 out params
EDDS_STATIC LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetFreeHard(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    LSA_UINT32                  CycleReductionRatio,
    LSA_UINT32                  CyclePhase,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pProvBytesFree,
    EDDS_LOCAL_MEM_U32_PTR_TYPE pProvCntFree)
{

    LSA_UINT32      MaxByteCnt,MaxCnt;
    LSA_BOOL        Status;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetFreeHard(Ratio: %d, Phase: %d)",
                           CycleReductionRatio,
                           CyclePhase);

    Status = LSA_TRUE;

    EDDS_RTProviderGetMaxUsed(pCSRTComp,
                              CycleReductionRatio,
                              CyclePhase,
                              &MaxByteCnt,
                              &MaxCnt);


    *pProvBytesFree = 0xFFFFFFFF; /* no limit */

    *pProvCntFree = 0xFFFFFFFF; /* no limit */

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                          "EDDS_RTProviderGetFreeHard: BFree: 0x%X, CFree: 0x%X",
                          *pProvBytesFree,
                          *pProvCntFree);

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetFreeHard(BFree: 0x%X, CFree: 0x%X), Status: 0x%X",
                           *pProvBytesFree,
                           *pProvCntFree,
                           Status);

    return(Status);

}
//lint +esym(613,pProvBytesFree,pProvCntFree)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :  EDDS_RTProviderGetKnotAndPhase                        +*/
/*+  Input/Output  :  EDDS_DDB_COMP_CSRT_TYPE_PTR            pCSRTComp      +*/
/*+                   EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE   pRqbParam      +*/
/*+                   LSA_UINT16                             CycleReductionNr*/
/*+                   EDDS_CSRT_KNOT_TYPE_PTR LOCAL_MEM_ATTR *pKnot         +*/
/*+                   EDDS_LOCAL_MEM_U32_PTR_TYPE            pKnotTableIdx  +*/
/*+                   EDDS_LOCAL_MEM_U16_PTR_TYPE            pCyclePhase    +*/
/*+                                                                         +*/
/*+  Result        :  LSA_RESULT                                            +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to CSRT-Managment                        +*/
/*+  pRqbParam           : Pointer to RQB-Parameter for provider add        +*/
/*+  CycleReductionNr    : Reduction Nr. (0,1,2,3..7)                       +*/
/*+  pKnot               : on return: pointer to address of knot-table entry+*/
/*+  pKnotTableIdx       : on retrun: pointer to address of knot-table idx  +*/
/*+  pCyclePhase         : on return: pointer to address of CyclePhase      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Depending on the CyclePhase and ReductionRatio            +*/
/*+               we will return a valid knot-table-index and the CyclePhase+*/
/*+               for this provider to use                                  +*/
/*+                                                                         +*/
/*+               Searches for best phase if the user wants to.             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp,pProv,pKnotElement)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB) - pKnotElement out param
//WARNING: be careful when using this function, make sure not to use pCSRTComp or pProv as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetKnotAndPhase(
    EDDS_DDB_COMP_CSRT_TYPE_PTR                  pCSRTComp,
    EDDS_CSRT_PROVIDER_TYPE_PTR                  pProv,
    LSA_UINT16                                   CycleReductionNr,
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR EDDS_LOCAL_MEM_ATTR *pKnotElement)
{


    LSA_UINT32             Interval;
//    LSA_UINT32             ProvBytesFree;
//    LSA_UINT32             ProvCntFree;
    LSA_UINT16             CyclePhase;
    LSA_RESULT             Status;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetKnotAndPhase(pCSRTComp: 0x%X, pProv: 0x%X, RatoNr: %d)",
                           pCSRTComp,
                           pProv,
                           CycleReductionNr);


    Status = EDD_STS_OK;

    /* Interval == ReductionRatio */

    Interval   = pProv->CycleReductionRatio;
    CyclePhase = pProv->CyclePhase;

    pProv->CyclePhase   = CyclePhase;

    if ( CyclePhase > Interval ) /* out of range ? */
    {
        Status = EDD_STS_ERR_PARAM;

        EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderGetKnotAndPhase: Wrong CyclePhase (%d). Max: %d",
                            CyclePhase,
                            Interval);

    }
    else
    {
        /* Get free provider bytes and cnt for this ReductionRatio and Phase. If  */
        /* no limit the values will be set to 0xFFFFFFFF which will most likely   */
        /* not be reached. So we dont have to do a special check later            */
        //ProvBytesFree = 0xFFFFFFFF;
        //ProvCntFree   = 0xFFFFFFFF;

        /* calculate Knot-Table index                           */
        /* (Interval-1) + (CyclePhase-1)                        */
        /* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

        pProv->KnotTableIdx  = Interval+CyclePhase-2;
        pProv->CyclePhase    = CyclePhase;


        /* Get Knot with filled in KnotElement */
        Status = EDDS_RTKnotGet(pCSRTComp,pProv->KnotTableIdx);

        if(EDD_STS_OK == Status)
        {
            *pKnotElement = &pCSRTComp->Knot.pKnotElementTbl[pCSRTComp->Knot.pKnotTbl[pProv->KnotTableIdx].KnotElementIdx];
        }
    }

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetKnotAndPhase(KTIdx: %d, CyPhase: %d), Status: 0x%X",
                           pProv->KnotTableIdx,
                           pProv->CyclePhase,
                           Status);

    return(Status);
}
//lint +esym(613,pCSRTComp,pProv,pKnotElement)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetMinMaxRatioUsedXRT        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ProviderByteCnt and ProviderCnt and sets the value+*/
/*+               within provider  cycle control                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMinMaxRatioUsedXRT(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp)
{

    LSA_UINT32              Max;
    LSA_UINT32              Min;
    LSA_UINT32              i;

    Min = EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX-1;
    Max = EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1;  //lint !e778 JB 18/11/2014 #def-based - should be 0

    for (i=EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1; i<EDDS_CSRT_XRT_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++)  //lint !e778 JB 18/11/2014 #def-based - should be 0
    {
        /* something present ? */
        if ( pCSRTComp->Prov.ProvRatioInfo[i].CntXRT )
        {
            if ( i < Min ) Min = i;
            if ( i > Max ) Max = i;
        }
    }

    if ( Min > Max ) /* the case if nothing present at all. set Min = Max */
    {
        Min = Max;
    }

    /* Note: We may be interrupted by cyclic send routine here. So it may   */
    /*       be possible that Max is updated, but Min not. This dont realy  */
    /*       care, because its only the border till we check for providers  */
    /*       and it dont matters if we check more or less. So we dont       */
    /*       protect this sequence.                                         */

    pCSRTComp->Prov.ProviderCycle.Xrt.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(Max);
    pCSRTComp->Prov.ProviderCycle.Xrt.MinInterval = (LSA_UINT16) EDDSGet2Potenz(Min);

}
//lint +esym(613,pCSRTComp)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetMinMaxRatioUsedUDP        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ProviderByteCnt and ProviderCnt and sets the value+*/
/*+               within provider  cycle control                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMinMaxRatioUsedUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp)
{

    LSA_UINT32              Max;
    LSA_UINT32              Min;
    LSA_UINT32              i;

    Min = EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MAX-1;
    Max = EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1;

    for (i=EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MIN-1; i<EDDS_CSRT_UDP_CYCLE_REDUCTION_RATIO_FACTOR_MAX; i++)
    {
        /* something present ? */
        if ( pCSRTComp->Prov.ProvRatioInfo[i].CntUDP )
        {
            if ( i < Min ) Min = i;
            if ( i > Max ) Max = i;
        }
    }

    if ( Min > Max ) /* the case if nothing present at all. set Min = Max */
    {
        Min = Max;
    }

    /* Note: We may be interrupted by cyclic send routine here. So it may   */
    /*       be possible that Max is updated, but Min not. This dont realy  */
    /*       care, because its only the border till we check for providers  */
    /*       and it dont matters if we check more or less. So we dont       */
    /*       protect this sequence.                                         */

    pCSRTComp->Prov.ProviderCycle.Udp.MaxInterval = (LSA_UINT16) EDDSGet2Potenz(Max);
    pCSRTComp->Prov.ProviderCycle.Udp.MinInterval = (LSA_UINT16) EDDSGet2Potenz(Min);

}
#endif

//lint -esym(613,pCSRTComp)
//JB 06/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderInsert(
		EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp,
		LSA_UINT16                              Idx)
{
    LSA_RESULT                              Status;

    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;

    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;

    LSA_UINT16                              ReductionFactor; /* 1..x */

	pProv           = &pCSRTComp->Prov.pProvTbl[Idx];

    ReductionFactor = pProv->CycleReductionNr +1;

    if(EDD_CYCLE_PHASE_UNDEFINED == ReductionFactor) {
    	ReductionFactor = EDDS_RTGetAndCheckReductionFactor(
    			pProv->CycleReductionRatio,
    			(LSA_BOOL) (pProv->Type ==  EDDS_CSRT_PROVIDER_TYPE_UDP)
    	);
	}

	if(0 == ReductionFactor)
    {
    	Status = EDD_STS_ERR_PARAM;
    }else
    {
    	pProv->CycleReductionNr     =    ReductionFactor-1;
		/*----------------------------------------------------------------- */
		/* Find and/or check for valid Phase, set it in Provider and get    */
		/* and get Knot-table Element                                       */
		/*----------------------------------------------------------------- */

		Status = EDDS_RTProviderSetKnotAndPhase(pCSRTComp,
												pProv,
												(LSA_UINT16)(ReductionFactor-1),
												&pKnotElement
												);

		if(EDD_STS_OK == Status) {

			/*------------------------------------------------------------- */
			/* Put provider to send-list in knot management(at end of list) */
			/* sequence in list currently not supported!                    */
			/*------------------------------------------------------------- */

			if ( EDDS_CSRT_PROVIDER_TYPE_UDP != pProv->Type )
			{
			    EDDS_RTProviderAddToSendList(pCSRTComp,&pKnotElement->ProviderSendListXRT,pProv,Idx);
			}
			#ifdef EDDS_XRT_OVER_UDP_INCLUDE
			else
			{
			    EDDS_RTProviderAddToSendList(pCSRTComp,&pKnotElement->ProviderSendListUDP,pProv,Idx);
			    
			}
			#endif
			pKnotElement->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */

			/* set the maximum number of ProviderBytes for this reduction ratio */

			if ( pKnotElement->ProviderByteCnt >
				 pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].MaxByteCnt)
			{
				pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].MaxByteCnt = pKnotElement->ProviderByteCnt;
			}

			pKnotElement->ProviderCnt++;

			/* set the maximum number of Provider for this reduction ratio     */

			if ( pKnotElement->ProviderCnt >
				 pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].MaxCnt)
			{
				pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].MaxCnt = pKnotElement->ProviderCnt;
			}

			if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type)
			{
				#ifdef EDDS_XRT_OVER_UDP_INCLUDE
				pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].CntUDP++;
				EDDS_RTProviderSetMinMaxRatioUsedUDP(pCSRTComp);
				#endif
			}
			else
			{
				pCSRTComp->Prov.ProvRatioInfo[pProv->CycleReductionNr].CntXRT++;
				EDDS_RTProviderSetMinMaxRatioUsedXRT(pCSRTComp);
			}

			/* update max-above - values within knot-table.                    */

			EDDS_RTProviderSetMaxAbove(pCSRTComp,
									   pProv->CycleReductionRatio,
									   pProv->CyclePhase);


		}
		else {
			EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
								"EDDS_RTProviderADD: EDDS_RTProviderGetKnotAndPhase() failed (0x%X)",
								Status);
		}
    }
    return Status;
}
//lint +esym(613,pCSRTComp)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderADD                          +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_ADD_TYPE                                         +*/
/*+                                                                         +*/
/*+  FrameID                 : FrameID for Provider.                        +*/
/*+  DstMAC                  : Destination MAC address                      +*/
/*+  DataLen                 : DataLen of I/O-Data in frame.                +*/
/*+  CycleReductionRatio     : spezifies CycleTime based on CycleBaseFactor +*/
/*+                            (1,2,4,8,16,32,64,128,256,512)               +*/
/*+  CyclePhase              : spezifies Phase within cycle.                +*/
/*+  CyclePhaseSequence      : spezifies Sequence in Phase (not supported)  +*/
/*+  VLANID                  : VLAN-ID put into frame.                      +*/
/*+  UserID                  : UserID for Event-indications.                +*/
/*+  GroupID                 : GroupID for ProviderSetState                 +*/
/*+  ProviderID              : returned ProviderID.                         +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+               EDD_STS_ERR_NO_LOCAL_IP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to add a new provider (xRT and xRTOverUDP)        +*/
/*+                                                                         +*/
/*+               First we check if free providers present. if not we       +*/
/*+               return EDD_STS_ERR_RESOURCE. Then we check for several    +*/
/*+               valid parameters.                                         +*/
/*+                                                                         +*/
/*+               Next we queue the provider within "used-chain" and put    +*/
/*+               the provider to the desired Knot-Send-Chain. (depends     +*/
/*+               on CycleReductionRation and CyclePhase).                  +*/
/*+                                                                         +*/
/*+               We also preinitialize all three send-frames with the      +*/
/*+               given values (MACs, FrameID, VLAN-ID etc.) which will not +*/
/*+               change.                                                   +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderADD(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_UINT16                              Idx;
    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE    pRqbParam;
    EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT16                              ReductionFactor = 0; /* 1..x */
    LSA_UINT16                              RtClassProp;
    LSA_BOOL                                Udp;
    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB			= pHDB->pDDB;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderADD(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    Udp             = LSA_FALSE;

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE) pRQB->pParam;
    pCSRTComp   = &pDDB->pSRT->CSRT;
    RtClassProp = pRqbParam->Properties & EDD_CSRT_PROV_PROP_RTCLASS_MASK;

    /*---------------------------------------------------------------------------*/
    /* get a free provider from the free list                                    */
    /*---------------------------------------------------------------------------*/

    Idx = pCSRTComp->Prov.ProviderFree.BottomIdx;

    /* Check if we have a free provider entry                   */
    /* this is true if the bottom-idx in the free list is valid */

    if ( Idx == EDDS_SRT_CHAIN_IDX_END )
    {

        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderADD: No more free provider!");

        Status = EDD_STS_ERR_RESOURCE;
    }

    if(EDD_STS_OK == Status)
    {
        /* ---------------------------------------------------- */
        /* check Properties for RT classes                      */
        /* ---------------------------------------------------- */
        if( RtClassProp != EDD_CSRT_PROV_PROP_RTCLASS_1 &&
            RtClassProp != EDD_CSRT_PROV_PROP_RTCLASS_2 )
        {
            Status = EDD_STS_ERR_PARAM;
        } /* check Properties for RT classes */

        /* ---------------------------------------------------- */
        /* check Properties flags                               */
        /* ---------------------------------------------------- */
        if ((pRqbParam->Properties & EDD_CSRT_PROV_PROP_SYSRED) ||
            (pRqbParam->Properties & EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP) ||
            (pRqbParam->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY))
        {
            Status = EDD_STS_ERR_PARAM;
        } /* check Properties flags */
    }


    if(EDD_STS_OK == Status)
    {
        /*-----------------------------------------------------------------------*/
        /* check for xRToverUdp or XRT Provider ADD and for DataLen              */
        /*-----------------------------------------------------------------------*/

        if ( (EDD_RQB_GET_SERVICE(pRQB) == EDD_SRV_SRT_PROVIDER_ADD) && (EDD_CSRT_PROV_PROP_RTCLASS_UDP == RtClassProp) )
        {
            #ifdef EDDS_XRT_OVER_UDP_INCLUDE
            if (( pRqbParam->DataLen  < EDD_CSRT_UDP_DATALEN_MIN ) ||
                ( pRqbParam->DataLen  > EDD_CSRT_UDP_DATALEN_MAX ))
            {

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderADD: Invalid UDP DataLen (%d)",
                                    pRqbParam->DataLen);

                Status = EDD_STS_ERR_PARAM;
            }
            else
            {
                if (pDDB->pGlob->HWParams.IPAddress.dw == 0 ) /* no IP present ? */
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: No local IP set!");

                    Status = EDD_STS_ERR_NO_LOCAL_IP;

                }
            }

            Udp = LSA_TRUE;
            #else
            Status = EDD_STS_ERR_SERVICE;   /* should not occure because already checked outside */
            #endif
        }
        else
        {
            if (( pRqbParam->DataLen  < EDD_CSRT_DATALEN_MIN ) ||
                ( pRqbParam->DataLen  > EDD_CSRT_DATALEN_MAX ))
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderADD: Invalid XRT DataLen (%d)",
                                    pRqbParam->DataLen);

                Status = EDD_STS_ERR_PARAM;
            }
            Udp = LSA_FALSE;
        }

        /*---------------------------------------------------------------------------*/
        /* get ReductionFactor(1..x) from ReductionRatio. 0 signals invalid Ratio    */
        /*---------------------------------------------------------------------------*/
        if(EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pRqbParam->CycleReductionRatio)
        {
            ReductionFactor = EDDS_RTGetAndCheckReductionFactor(pRqbParam->CycleReductionRatio,Udp);

            if(0 == ReductionFactor)
            {
                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: wrong CycleReductionRatio");
                Status = EDD_STS_ERR_PARAM;
            }
        } /* check CycleReductionRatio */

        /* ---------------------------------------------------- */
        /* check CyclePhase                                     */
        /* ---------------------------------------------------- */
        if ( (EDD_CYCLE_PHASE_UNDEFINED != pRqbParam->CyclePhase) &&
             (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED != pRqbParam->CycleReductionRatio) )
        {
            if (pRqbParam->CyclePhase > pRqbParam->CycleReductionRatio)
            {
                EDDS_UPPER_TRACE_00(pDDB->pGlob->TraceIdx,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderADD: wrong CyclePhase");
                Status = EDD_STS_ERR_PARAM;
            }
        } /* check CyclePhase */

        /* ---------------------------------------------------- */
        /* check GroupID                                        */
        /* ---------------------------------------------------- */
        if ( EDD_CFG_CSRT_MAX_PROVIDER_GROUP <= pRqbParam->GroupID )
        {
            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderADD: Invalid GroupID!");

            Status = EDD_STS_ERR_PARAM;
        } /* check GroupID */

        /*-----------------------------------------------------------------------*/
        /* check FrameID                                                         */
        /*-----------------------------------------------------------------------*/
        if( EDD_FRAME_ID_UNDEFINED != pRqbParam->FrameID)
        {
            /* ---------------------------------------------------- */
            /* check FrameID                                        */
            /* --> FrameID must match RTClass                       */
            /* --> check FrameID area (RTClass boundary/configured  */
            /* ---------------------------------------------------- */
            switch (RtClassProp)
            {
                case EDD_CSRT_PROV_PROP_RTCLASS_1:
                {
                    if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID) ||
                        (EDD_SRT_FRAMEID_CSRT_STOP2  < pRqbParam->FrameID))
                    {
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "[H:--] EDDS_RTProviderADD(): FrameID (0x%X) does not match RTClass",
                            pRqbParam->FrameID);
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                break;

                case EDD_CSRT_PROV_PROP_RTCLASS_2:
                {
                    if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID) ||
                        (EDD_SRT_FRAMEID_CSRT_STOP1  < pRqbParam->FrameID))
                    {
                        EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                            "[H:--] EDDS_RTProviderADD(): FrameID (0x%X) does not match RTClass",
                            pRqbParam->FrameID);
                        Status = EDD_STS_ERR_PARAM;
                    }
                }
                break;

                default:
                {
                    EDDS_UPPER_TRACE_01(pDDB->pGlob->TraceIdx, LSA_TRACE_LEVEL_ERROR,
                        "[H:--] EDDS_RTProviderADD(): invalid RTClassProperty (0x%X)",
                        RtClassProp);
                    Status = EDD_STS_ERR_PARAM;
                }
                break;
            } /* check FrameID */
        }

        if ( Status == EDD_STS_OK )
        {
			EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
								"EDDS_RTProviderADD: PID: %d, Ratio: %d",
								Idx,
								pRqbParam->CycleReductionRatio);




			pProv        = &pCSRTComp->Prov.pProvTbl[Idx];

			/*------------------------------------------------------------- */
			/* copy parameter to management structure                       */
			/*------------------------------------------------------------- */
			pProv->KnotTableIdx			=    0xFFFFFFFF;		/* not enqueued yet. */
			pProv->CyclePhase           =    pRqbParam->CyclePhase;		///am2219
			pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;
			pProv->CycleReductionRatio  =    pRqbParam->CycleReductionRatio;
			pProv->CycleReductionNr     =    ReductionFactor-1;
			pProv->DataLen              =    pRqbParam->DataLen;
			pProv->FrameID              =    pRqbParam->FrameID;
			pProv->UserID               =    pRqbParam->UserID;
			pProv->GroupID              =    pRqbParam->GroupID;

			pRqbParam->ProviderID = Idx;	/* Provider ID is the index within */
											/* provider table                  */

			pProv->Type                 =   Udp ? EDDS_CSRT_PROVIDER_TYPE_UDP:EDDS_CSRT_PROVIDER_TYPE_XRT;
			pProv->Properties           =   pRqbParam->Properties;
			pProv->Active               =   LSA_FALSE;
			pProv->FirstSend            =   0;

			pProv->DataStatus           =   EDD_CSRT_DSTAT_LOCAL_INIT_VALUE;
			pProv->AutoStopConsumerID   =   EDD_CONSUMER_ID_REMOVE_AS; /* none */

			pCSRTComp->Prov.ProviderUsedCnt++;

			/* setup framebuffer in EDDS_RTProviderControl */

			/*------------------------------------------------------------- */
			/* Remove Provider at bottom from free-chain list               */
			/*------------------------------------------------------------- */

			/* if this was the last free provider. set Idx to END-tag       */
			if ( pProv->UsedFreeLink.Next == EDDS_SRT_CHAIN_IDX_END )
			{
				pCSRTComp->Prov.ProviderFree.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
				pCSRTComp->Prov.ProviderFree.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
			}
			else
			{
				/* not the last provider */
				pCSRTComp->Prov.ProviderFree.BottomIdx = pProv->UsedFreeLink.Next;
				pCSRTComp->Prov.pProvTbl[pProv->UsedFreeLink.Next].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
			}

			/*------------------------------------------------------------- */
			/* Put Provider to used-chain list (end)                        */
			/*------------------------------------------------------------- */

			/* nothing in queue ? */
			if ( pCSRTComp->Prov.ProviderUsed.TopIdx == EDDS_SRT_CHAIN_IDX_END )
			{
				pCSRTComp->Prov.ProviderUsed.BottomIdx   = Idx;
				pCSRTComp->Prov.ProviderUsed.TopIdx      = Idx;
				pProv->UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
				pProv->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
			}
			else
			{
				pProv->UsedFreeLink.Prev = pCSRTComp->Prov.ProviderUsed.TopIdx;
				pProv->UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
				pCSRTComp->Prov.pProvTbl[pCSRTComp->Prov.ProviderUsed.TopIdx].UsedFreeLink.Next = Idx;
				pCSRTComp->Prov.ProviderUsed.TopIdx = Idx;
			}

	        if( !( EDD_CYCLE_PHASE_UNDEFINED == pProv->CyclePhase ||
	        	   EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == pProv->CyclePhaseSequence ||
	        	   EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pProv->CycleReductionRatio
	        	  ))
	        {
	        	Status = EDDS_RTProviderInsert(pCSRTComp,Idx);
	        }

        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderADD, Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetMax                       +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp       +*/
/*+                             LSA_UINT32              CycleReductionRatio +*/
/*+                             LSA_UINT16              CycleReductionNr    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRTComp           : Pointer to start CSRT-management                 +*/
/*+  CycleReductionRatio : CycleReductionRatio to find maxbytecnt for       +*/
/*+                        (1.2.4.8.16..512)                                +*/
/*+  CycleReductionNr    : Reduction Nr. (0,1,2,3..10)                      +*/
/*+                                                                         +*/
/*+  Result     : void                                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searchs KnotTable for the CycleReductionRatio for the     +*/
/*+               maximum ProviderByteCnt and ProviderCnt and sets the value+*/
/*+               within ProvRatioInfo                                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRTComp)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRTComp as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetMax(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRTComp,
    LSA_UINT32                  CycleReductionRatio,
    LSA_UINT16                  CycleReductionNr )
{

    EDDS_CSRT_KNOT_TYPE_PTR         pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR pKnotElement;
    LSA_UINT32                      Max;
    LSA_UINT32                      MaxCnt;
    LSA_UINT32                      i;

    Max      = 0;
    MaxCnt   = 0;

    /* get interval for this CycleReductionRatio     */
    /* (is also the number of phases for this ratio) */

    i = CycleReductionRatio; /* 1,2,4,8..512 */

    /* Startindex in Knot-Table for this Ratio       */

    pKnot        = &pCSRTComp->Knot.pKnotTbl[i-1];
    pKnotElement = pCSRTComp->Knot.pKnotElementTbl;

    while (i--)
    {
        if ( pKnot->KnotElementIdx != EDDS_SRT_IDX_NOT_USED )
        {
            if ( pKnotElement[pKnot->KnotElementIdx].ProviderByteCnt > Max    ) Max    = pKnotElement[pKnot->KnotElementIdx].ProviderByteCnt;
            if ( pKnotElement[pKnot->KnotElementIdx].ProviderCnt     > MaxCnt ) MaxCnt = pKnotElement[pKnot->KnotElementIdx].ProviderCnt;
        }
        pKnot++;
    }

    if( EDDS_CSRT_CYCLE_RATIO_CNT <= CycleReductionNr )
    { //fatal
        EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,
                              "EDDS_RTProviderSetMax: out-of-bounds (CycleReductionNr: 0x%X)",
                              CycleReductionNr);

        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }
    else
    {
		pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].MaxByteCnt  = Max;
		pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].MaxCnt      = MaxCnt;
    }


}
//lint +esym(613,pCSRTComp)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTProviderChangeSendListXRT                +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                             LSA_UINT32                  KnotTableIdxNew +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT           : Pointer to CSRT-Management                           +*/
/*+  ProviderID      : ID of provider to change phase                       +*/
/*+  pProv           : Pointer to provider magm to change phase             +*/
/*+  KnotTableIdxNew : New KnottableIndex for this provider                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes provider from on knot-table send-list and puts    +*/
/*+               him to another. this is done with protection against      +*/
/*+               high prio context of edds.                                +*/
/*+                                                                         +*/
/*+               Note: For both phases a knot element must be present.     +*/
/*+                     After change knot we try to free the old knot elem. +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT,pProv)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT or pProv as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTProviderChangeSendListXRT(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv,
    LSA_UINT32                  KnotTableIdxNew)
{


    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_TYPE_PTR                 pKnotNew;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElementNew;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    KnotTableIdx    = pProv->KnotTableIdx;
    pKnot           = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
    EDDS_ASSERT(pKnot->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
    pKnotElement    = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];
    pKnotNew        = &pCSRT->Knot.pKnotTbl[KnotTableIdxNew];
    EDDS_ASSERT(pKnotNew->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
    pKnotElementNew = &pCSRT->Knot.pKnotElementTbl[pKnotNew->KnotElementIdx];

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    EDDS_ENTER_HIGH();

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */
                pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pKnotElement->ProviderSendListXRT.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
                pKnotElement->ProviderSendListXRT.BottomIdx = Next;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pKnotElement->ProviderSendListXRT.TopIdx = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
        pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx  = Next;
    }

    /*---------------------------------------------------------------------------*/
    /* Now we put the provider to the new knot list                              */
    /*---------------------------------------------------------------------------*/

    pProv->KnotTableIdx     =  KnotTableIdxNew;
    
    EDDS_RTProviderAddToSendList(pCSRT,&pKnotElementNew->ProviderSendListXRT,pProv,ProviderID);

    EDDS_EXIT_HIGH();

    /*---------------------------------------------------------------------------*/
    /* Actualize the limits                                                      */
    /*---------------------------------------------------------------------------*/

    pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
    pKnotElement->ProviderCnt--;

    pKnotElementNew->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */
    pKnotElementNew->ProviderCnt++;

    /* free old Knot Element if not used anymore */
    EDDS_RTKnotFree(pCSRT,KnotTableIdx);

}
//lint +esym(613,pCSRT,pProv)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTProviderChangeSendListUDP                +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR pProv           +*/
/*+                             LSA_UINT32                  KnotTableIdxNew +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT           : Pointer to CSRT-Management                           +*/
/*+  ProviderID      : ID of provider to change phase                       +*/
/*+  pProv           : Pointer to provider magm to change phase             +*/
/*+  KnotTableIdxNew : New KnottableIndex for this provider                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes provider from on knot-table send-list and puts    +*/
/*+               him to another. this is done with protection against      +*/
/*+               high prio context of edds.                                +*/
/*+                                                                         +*/
/*+               Note: For both phases a knot element must be present.     +*/
/*+                     After change knot we try to free the old knot elem. +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTProviderChangeSendListUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    EDDS_CSRT_PROVIDER_TYPE_PTR pProv,
    LSA_UINT32                  KnotTableIdxNew)
{


    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_TYPE_PTR                 pKnotNew;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElementNew;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    KnotTableIdx    = pProv->KnotTableIdx;
    pKnot           = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
    EDDS_ASSERT(pKnot->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
    pKnotElement    = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];
    pKnotNew        = &pCSRT->Knot.pKnotTbl[KnotTableIdxNew];
    EDDS_ASSERT(pKnotNew->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
    pKnotElementNew = &pCSRT->Knot.pKnotElementTbl[pKnotNew->KnotElementIdx];

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    EDDS_ENTER_HIGH();

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */
                pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pKnotElement->ProviderSendListUDP.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                pKnotElement->ProviderSendListUDP.BottomIdx = Next;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next    = Next;
            pKnotElement->ProviderSendListUDP.TopIdx    = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
        pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
    }

    if ( pCSRT->Prov.ProviderCycle.Udp.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Udp.ActProvIdx  = Next;
    }

    /*---------------------------------------------------------------------------*/
    /* Now we put the provider to the new knot list                              */
    /*---------------------------------------------------------------------------*/

    pProv->KnotTableIdx     =  KnotTableIdxNew;
    
    EDDS_RTProviderAddToSendList(pCSRT,&pKnotElementNew->ProviderSendListUDP,pProv,ProviderID);

    EDDS_EXIT_HIGH();

    /*---------------------------------------------------------------------------*/
    /* Actualize the limits                                                      */
    /*---------------------------------------------------------------------------*/

    pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
    pKnotElement->ProviderCnt--;

    pKnotElementNew->ProviderByteCnt += pProv->DataLen; /* sum of all datalen  */
    pKnotElementNew->ProviderCnt++;

    /* free old Knot Element if not used anymore */
    EDDS_RTKnotFree(pCSRT,KnotTableIdx);

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderChangePhase                  +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_ADD_TYPE                    +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_CHANGE_PHASE_TYPE                                +*/
/*+                                                                         +*/
/*+  ProviderID              : ProviderID                                   +*/
/*+  CyclePhase              : spezifies Phase within cycle. (1..x)         +*/
/*+  CyclePhaseSequence      : spezifies Sequence in Phase (not supported)  +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_LIMIT_REACHED                                 +*/
/*+               EDD_STS_ERR_PHASE_MASKED                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to change phase of a provider.                    +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB,pProv)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pProv set null -> init
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderChangePhase(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE pRqbParam;
    EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT32                              KnotTableIdx;
    LSA_UINT16                              OldCyclePhase;
    LSA_UINT32                              Interval;
    LSA_UINT32                              ProvBytesFree;
    LSA_UINT32                              ProvCntFree;
    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB			= pHDB->pDDB;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderChangePhase(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pProv           = LSA_NULL; /* to prevent compiler bug */
    KnotTableIdx    = 0;        /* to prevent compiler bug */
    Interval        = 0;        /* to prevent compiler bug */

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_CHANGE_PHASE_PTR_TYPE) pRQB->pParam;
    pCSRTComp   = &pDDB->pSRT->CSRT;

    /*---------------------------------------------------------------------------*/
    /* check parameter                                                           */
    /*---------------------------------------------------------------------------*/

    if ( pRqbParam->ProviderID >=pCSRTComp->Cfg.ProviderCnt )
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderChangePhase: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ProviderID];

        if ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pProv->Type )
        {
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderChangePhase: Provider not in use (%d)!",
                                pRqbParam->ProviderID);

            Status = EDD_STS_ERR_SEQUENCE;
        }
        else
        {

            Interval    = pProv->CycleReductionRatio; /* 1,2,4,8...512 */

            /* Check if Phase is valid */
            if (( pRqbParam->CyclePhase == 0       )  ||
                ( pRqbParam->CyclePhase > Interval ))   /* invalid Phase  */
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderChangePhase: Invalid Phase (%d)!",
                                    pRqbParam->CyclePhase);

                Status = EDD_STS_ERR_PARAM;
            }
        }
    }

    /*---------------------------------------------------------------------------*/
    /* if parameters ok check if we stay within limits..                         */
    /*---------------------------------------------------------------------------*/

    if ( Status == EDD_STS_OK )
    {
        /* is this a new phase? if not nothing is to be done..*/

        if ( pProv->CyclePhase != pRqbParam->CyclePhase )
        {
            /* calculate Knot-Table index                           */
            /* (Interval-1) + (CyclePhase-1)                        */
            /* example for Interval = 4 and CyclePhase =2 : Idx = 4 */

            KnotTableIdx    = Interval+pRqbParam->CyclePhase-2;

            /* check if we have room.  if so we look if the provider dont*/
            /* exceeds the limits for this phase                         */

            if ( EDDS_RTProviderGetFreeHard(pCSRTComp,
                                            Interval, /* == CycleReductionRatio */
                                            pRqbParam->CyclePhase,
                                            &ProvBytesFree,
                                            &ProvCntFree))
            {
                /* save CyclePhase of old KnotTable Entry               */
                OldCyclePhase               =    pProv->CyclePhase;

                pProv->CyclePhase           =    pRqbParam->CyclePhase;
                pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;

                if ( EDDS_RTKnotGet(pCSRTComp,KnotTableIdx) == EDD_STS_OK )
                {
                    EDDS_UPPER_TRACE_03(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                        "EDDS_RTProviderChangePhase: PID: %d, NewPhase: %d, OldPhase: %d",
                                        pRqbParam->ProviderID,
                                        pProv->CyclePhase,
                                        OldCyclePhase);

                    /* change the providers send list (phase) atomar */
                    if ( pProv->Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                    {
                        SRTProviderChangeSendListXRT(pCSRTComp,
                                                     pRqbParam->ProviderID,
                                                     pProv,
                                                     KnotTableIdx);
                    }
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    else
                    {
                        SRTProviderChangeSendListUDP(pCSRTComp,
                                                     pRqbParam->ProviderID,
                                                     pProv,
                                                     KnotTableIdx);
                    }
                    #endif

                    /* calculate and set new maximum provider byte cnt for this */
                    /* reduction ratio.                                         */
                    EDDS_RTProviderSetMax(pCSRTComp,
                                          pProv->CycleReductionRatio,
                                          pProv->CycleReductionNr);

                    /* update max-above - values within knot-table.             */
                    EDDS_RTProviderSetMaxAbove(pCSRTComp,
                                               pProv->CycleReductionRatio,
                                               pProv->CyclePhase);

                    /* update max-above - values within knot-table for the old CyclePhase  */
                    EDDS_RTProviderSetMaxAbove(pCSRTComp,
                                               pProv->CycleReductionRatio,
                                               OldCyclePhase);
                }
                else
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,
                                        "EDDS_RTProviderChangePhase: EDDS_RTKnotGet() failed");
                }
            }
            else Status = EDD_STS_ERR_LIMIT_REACHED;
        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderChangePhase(), Status: 0x%X",
                           Status);

    return(Status);
}
//lint +esym(613,pRQB,pHDB,pProv)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTProviderRemoveXRT                        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ProviderID   : ID of provider to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where provider is queued in sendlist +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a provider from Knot-Sendlist and set it to       +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Removement is syncronized with sending in progress in     +*/
/*+               higher prio state (task). We adjust the sender management +*/
/*+               if we remove a provider in the sendlist.                  +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTProviderRemoveXRT(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    LSA_UINT32                  KnotTableIdx)
{

    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;


    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :SRTProviderRemoveXRT(pCSRT: 0x%X, PID: %d, KTIdx: %d)",
                           pCSRT,
                           ProviderID,
                           KnotTableIdx);

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    /* Use ENTER_HIGH instead of SHORT. This is a little optimization to         */
    /* not use SHORT protection with HIGH. May always be used instead of SHORT   */
    /* but to be compatible with older EDDS version we only use it if            */
    /* EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT is defined               */

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_ENTER_HIGH();
#endif

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    /* with priority based protection:                                          */
    /* Use short protection because of "ActProvIdx" access by HIGH-Prio context */
    /* in addition to chain elements, so we must asure consistense.             */
    /* EDDS_ENTER_HIGH may also be used instead, but this is faster.            */

#ifndef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_ENTER_SHORT();
#endif

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    if(0xFFFFFFFF != KnotTableIdx)
    {
        pKnot        = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
        EDDS_ASSERT(pKnot->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
        pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
			( Next == EDDS_SRT_CHAIN_IDX_END ))
        {
            if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
            {
                if ( Next == EDDS_SRT_CHAIN_IDX_END )
                {
                    /* only one in list */
                    pKnotElement->ProviderSendListXRT.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                    pKnotElement->ProviderSendListXRT.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    /* first one in list, but not only one */
                    pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                    pKnotElement->ProviderSendListXRT.BottomIdx = Next;
                }
            }
            else
            {
                /* last one in list but not only one */
                pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
                pKnotElement->ProviderSendListXRT.TopIdx = Prev;
            }
        }
        else
        {
            /* in middle of list */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
        }

        pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
        pKnotElement->ProviderCnt--;

        EDDS_RTKnotFree(pCSRT,KnotTableIdx); /* remove KnotElement if unused yet */
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Xrt.ActProvIdx  = Next;
    }

#ifndef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_EXIT_SHORT();
#endif

    /*---------------------------------------------------------------------------*/
    /* Now we can remove  the provider from the InUse-chain.                     */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */

                pCSRT->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRT->Prov.ProviderUsed.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.ProviderUsed.BottomIdx = Next;
                pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next = Next;
            pCSRT->Prov.ProviderUsed.TopIdx              = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next  = Next;
        pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* Put this provider to the end of the free-chain..                          */
    /*---------------------------------------------------------------------------*/

    pCSRT->Prov.pProvTbl[ProviderID].Type = EDDS_CSRT_PROVIDER_TYPE_NOT_USED; /* this provider is no more in use */
    pCSRT->Prov.ProviderUsedCnt--;

    if ( pCSRT->Prov.ProviderFree.TopIdx == EDDS_SRT_CHAIN_IDX_END ) /* nothing in chain ? */
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.ProviderFree.BottomIdx   = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx      = ProviderID;
    }
    else
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = pCSRT->Prov.ProviderFree.TopIdx;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[pCSRT->Prov.ProviderFree.TopIdx].UsedFreeLink.Next = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx = ProviderID;
    }

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_EXIT_HIGH();
#endif

    EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:SRTProviderRemoveXRT()");

}
//lint +esym(613,pCSRT)

#ifdef EDDS_XRT_OVER_UDP_INCLUDE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    SRTProviderRemoveUDP                        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             LSA_UINT16                  ProviderID      +*/
/*+                             LSA_UINT32                  KnotTableIdx    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  ProviderID   : ID of provider to remove                                +*/
/*+  KnotTableIdx : Index in Knottable where provider is queued in sendlist +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Removes a provider from Knot-Sendlist and set it to       +*/
/*+               "not" InUse.                                              +*/
/*+                                                                         +*/
/*+               Removement is syncronized with sending in progress in     +*/
/*+               higher prio state (task). We adjust the sender management +*/
/*+               if we remove a provider in the sendlist.                  +*/
/*+                                                                         +*/
/*+               Ids and Idx must be valid ! not checked.                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR SRTProviderRemoveUDP(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    LSA_UINT16                  ProviderID,
    LSA_UINT32                  KnotTableIdx)
{

    EDDS_CSRT_KNOT_TYPE_PTR                 pKnot;
    EDDS_CSRT_KNOT_ELEMENT_TYPE_PTR         pKnotElement;
    LSA_UINT16                              Prev;
    LSA_UINT16                              Next;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :SRTProviderRemoveUDP(pCSRT: 0x%X, PID: %d, KTIdx: %d)",
                           pCSRT,
                           ProviderID,
                           KnotTableIdx);

    /*---------------------------------------------------------------------------*/
    /* First we remove the provider from the Knotmanagment send-list.            */
    /*---------------------------------------------------------------------------*/

    /* Use ENTER_HIGH instead of SHORT. This is a little optimization to         */
    /* not use SHORT protection with HIGH. May always be used instead of SHORT   */
    /* but to be compatible with older EDDS version we only use it if            */
    /* EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT is defined               */

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_ENTER_HIGH();
#endif

    Prev = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].SendLink.Next;

    /* with priority based protection:                                          */
    /* Use short protection because of "ActProvIdx" access by HIGH-Prio context */
    /* in addition to chain elemtents, so we must asure consistense.            */
    /* EDDS_ENTER_HIGH may also be used instead.        */

#ifndef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_ENTER_SHORT();
#endif

    /* four  cases:                                                              */
    /* provider is the first in list (but not the only one)                      */
    /* provider is the last in list (but not the only one)                       */
    /* provider is the only one in list                                          */
    /* provider is inside linked list                                            */

    /* check if first or last or only in list */

    if(0xFFFFFFFF != KnotTableIdx)
    {
        pKnot        = &pCSRT->Knot.pKnotTbl[KnotTableIdx];
        EDDS_ASSERT(pKnot->KnotElementIdx >= pCSRT->Knot.KnotElementCnt);
        pKnotElement = &pCSRT->Knot.pKnotElementTbl[pKnot->KnotElementIdx];

        if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
            ( Next == EDDS_SRT_CHAIN_IDX_END ))
        {
            if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
            {
                if ( Next == EDDS_SRT_CHAIN_IDX_END )
                {
                    /* only one in list */
                    pKnotElement->ProviderSendListUDP.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                    pKnotElement->ProviderSendListUDP.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
                }
                else
                {
                    /* first one in list, but not only one */
                    pCSRT->Prov.pProvTbl[Next].SendLink.Prev    = Prev;
                    pKnotElement->ProviderSendListUDP.BottomIdx = Next;
                }
            }
            else
            {
                /* last one in list but not only one */
                pCSRT->Prov.pProvTbl[Prev].SendLink.Next    = Next;
                pKnotElement->ProviderSendListUDP.TopIdx    = Prev;
            }
        }
        else
        {
            /* in middle of list */
            pCSRT->Prov.pProvTbl[Prev].SendLink.Next = Next;
            pCSRT->Prov.pProvTbl[Next].SendLink.Prev = Prev;
        }

        pKnotElement->ProviderByteCnt -= pCSRT->Prov.pProvTbl[ProviderID].DataLen;
        pKnotElement->ProviderCnt--;

        EDDS_RTKnotFree(pCSRT,KnotTableIdx); /* remove KnotElement if unused yet */
    }

    /*---------------------------------------------------------------------------*/
    /* correct ActProvIdx for sender.. may only be neccessary                    */
    /* if sending NOT in cyclic blockmode, where sending is                      */
    /* done not in one part.                                                     */
    /*---------------------------------------------------------------------------*/

    if ( pCSRT->Prov.ProviderCycle.Udp.ActProvIdx == ProviderID )
    {
        pCSRT->Prov.ProviderCycle.Udp.ActProvIdx  = Next;
    }

#ifndef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_EXIT_SHORT();
#endif

    /*---------------------------------------------------------------------------*/
    /* Now we can remove  the provider from the InUse-chain.                     */
    /*---------------------------------------------------------------------------*/

    Prev = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev;
    Next = pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next;

    if (( Prev == EDDS_SRT_CHAIN_IDX_END ) ||
        ( Next == EDDS_SRT_CHAIN_IDX_END ))

    {
        if  ( Prev == EDDS_SRT_CHAIN_IDX_END )
        {
            if ( Next == EDDS_SRT_CHAIN_IDX_END )
            {
                /* only one in list */

                pCSRT->Prov.ProviderUsed.BottomIdx = EDDS_SRT_CHAIN_IDX_END;
                pCSRT->Prov.ProviderUsed.TopIdx    = EDDS_SRT_CHAIN_IDX_END;
            }
            else
            {
                /* first one in list, but not only one */
                pCSRT->Prov.ProviderUsed.BottomIdx = Next;
                pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
            }
        }
        else
        {
            /* last one in list but not only one */
            pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next = Next;
            pCSRT->Prov.ProviderUsed.TopIdx              = Prev;
        }
    }
    else
    {
        /* in middle of list */
        pCSRT->Prov.pProvTbl[Prev].UsedFreeLink.Next  = Next;
        pCSRT->Prov.pProvTbl[Next].UsedFreeLink.Prev  = Prev;
    }

    /*---------------------------------------------------------------------------*/
    /* Put this provider to the end of the free-chain..                          */
    /*---------------------------------------------------------------------------*/

    pCSRT->Prov.pProvTbl[ProviderID].Type = EDDS_CSRT_PROVIDER_TYPE_NOT_USED; /* this provider is no more in use */
    pCSRT->Prov.ProviderUsedCnt--;

    if ( pCSRT->Prov.ProviderFree.TopIdx == EDDS_SRT_CHAIN_IDX_END ) /* nothing in chain ? */
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.ProviderFree.BottomIdx   = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx      = ProviderID;
    }
    else
    {
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Prev = pCSRT->Prov.ProviderFree.TopIdx;
        pCSRT->Prov.pProvTbl[ProviderID].UsedFreeLink.Next = EDDS_SRT_CHAIN_IDX_END;
        pCSRT->Prov.pProvTbl[pCSRT->Prov.ProviderFree.TopIdx].UsedFreeLink.Next = ProviderID;
        pCSRT->Prov.ProviderFree.TopIdx = ProviderID;
    }

#ifdef EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT
    EDDS_EXIT_HIGH();
#endif

    EDDS_CRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:SRTProviderRemoveUDP()");

}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderRemove                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE                 +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_REMOVE_TYPE                                      +*/
/*+                                                                         +*/
/*+  ProviderID : Valid ProviderID                                          +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to remove a  provider.                            +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+               A provider  has to be in use and no buffers               +*/
/*+               locked (EDD_STS_ERR_SEQUENCE if not)                      +*/
/*+                                                                         +*/
/*+               Providers may be active on remove. will be deactivated.   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderRemove(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE pRqbParam;
    EDDS_DDB_COMP_CSRT_TYPE_PTR             pCSRTComp;
    EDDS_CSRT_PROVIDER_TYPE_PTR             pProv;
    LSA_UINT16                              CycleReductionRatio;
    LSA_UINT16                              CycleReductionNr;
    LSA_UINT16                              CyclePhase;
    LSA_UINT8                               Type;
    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB			= pHDB->pDDB;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderRemove(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRqbParam = (EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE) pRQB->pParam;

    /* check for valid ID-range. ProviderID is idx within table */

    if ( pRqbParam->ProviderID >= pDDB->pSRT->CSRT.Cfg.ProviderCnt )
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderRemove: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /*-----------------------------------------------------------------------*/
        /* check if provider is in use, not active and no buffer locked.         */
        /* for ALL we first check if we can remove all provider                  */
        /*-----------------------------------------------------------------------*/

        pCSRTComp = &pDDB->pSRT->CSRT;

        /*-----------------------------------------------------------------------*/
        /* Only one provider                                                     */
        /*-----------------------------------------------------------------------*/
        {
            pProv = &pCSRTComp->Prov.pProvTbl[pRqbParam->ProviderID];

            /* provider has to be in use, not active and not locked to remove ! */

            if  ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pProv->Type )
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                    "EDDS_RTProviderRemove: Provider (%d) not in use!",
                                    pRqbParam->ProviderID);

                Status = EDD_STS_ERR_PARAM;
            }

            if ( Status == EDD_STS_OK )
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderRemove: PID: %d",
                                    pRqbParam->ProviderID);

                CycleReductionRatio = pProv->CycleReductionRatio;
                CycleReductionNr    = pProv->CycleReductionNr;
                CyclePhase          = pProv->CyclePhase;
                Type                = pProv->Type;

                /* we deactivate the provider (may already be stopped) */
                EDDS_RTProviderStop(pCSRTComp,
                                    pProv);

                /* if a Autostop-Consumer is referenced, decrement Counter */
                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {
                    EDDS_ASSERT(pProv->AutoStopConsumerID >= pCSRTComp->Cons.ConsumerCnt);

                    /* decrement number of Providers using this consumer with Autostop */
                    pCSRTComp->Cons.pConsTbl[pProv->AutoStopConsumerID].ProviderAutoStopCnt--;
                }

                if ( Type == EDDS_CSRT_PROVIDER_TYPE_XRT )
                {
                    SRTProviderRemoveXRT(pCSRTComp,
                                         pRqbParam->ProviderID,
                                         pProv->KnotTableIdx);

                    /* actualize min/max ratio used by XRT */
                    if( 0xFFFF != CycleReductionNr )
                    {
                        pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].CntXRT--;
                    }

                    EDDS_RTProviderSetMinMaxRatioUsedXRT(pCSRTComp);
                }
                #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                else
                {
                    SRTProviderRemoveUDP(pCSRTComp,
                                         pRqbParam->ProviderID,
                                         pProv->KnotTableIdx);

                    /* actualize min/max ratio used by UDP */
                    if( 0xFFFF != CycleReductionNr )
                    {
                        pCSRTComp->Prov.ProvRatioInfo[CycleReductionNr].CntUDP--;
                    }

                    EDDS_RTProviderSetMinMaxRatioUsedUDP(pCSRTComp);
                }
                #endif

                /* calculate and set new maximum provider byte cnt for this */
                /* reduction ratio.                                         */

                if(0xFFFFFFFF != pProv->KnotTableIdx)
                {
                    EDDS_RTProviderSetMax(pCSRTComp,
                                          CycleReductionRatio,
                                          CycleReductionNr);

                    /* update max-above - values within knot-table.             */
                    EDDS_RTProviderSetMaxAbove(pCSRTComp,
                                               CycleReductionRatio,
                                               CyclePhase);
                }
            }
        }

    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderRemove(), Status: 0x%X",
                           Status);


    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderStart                        +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT       +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR     pProv       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pProv      : a provider in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start a Provider                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT, pProv)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT or pProv as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderStart(
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT,
    EDDS_CSRT_PROVIDER_TYPE_PTR     pProv)
{
    /* only if not already active */
    if ( ! pProv->Active)
    {
         pCSRT->Prov.ProviderActCnt++;
        /* sequence is important, because this can be interrupted */
        /* by sender.                                             */
        pProv->FirstSend    = 0;
        pProv->Active       = LSA_TRUE;
    }

}
//lint +esym(613,pCSRT, pProv)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderStop                         +*/
/*+  Input/Output          :    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT       +*/
/*+                             EDDS_CSRT_PROVIDER_TYPE_PTR     pProv       +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT      : Pointer to CSRT-Management                                +*/
/*+  pProv      : a provider in use                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Stops a Provider                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT, pProv)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when calling this function, make sure not to use pCSRT or pProv as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RTProviderStop(
    EDDS_DDB_COMP_CSRT_TYPE_PTR     pCSRT,
    EDDS_CSRT_PROVIDER_TYPE_PTR     pProv)
{
    if ( pProv->Active)
    {
        pCSRT->Prov.ProviderActCnt--;
        /* sequence is important, because this can be interrupted */
        /* by sender.                                             */
        pProv->Active       = LSA_FALSE;
        pProv->FirstSend    = 0;
    }
}
//lint +esym(613,pCSRT, pProv)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_SET_DEBUG_INFO_PROVIDER                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Helper function.                                                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets DebugInfo in EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRqbParam)
//JB 18/11/2014 previously taken from RQB
//WARNING: be careful when using this function, make sure not to use pRqbParam as null ptr!
EDDS_STATIC void EDDS_SET_DEBUG_INFO_PROVIDER(
		EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE pRqbParam,
		LSA_RESULT Status,
		LSA_UINT16 line
	)
{
    pRqbParam->DebugInfo[0] = EDD_DEBUGINFO_CONTROL_IDENT_EDDS;
    pRqbParam->DebugInfo[1] = 0x0;
    Status = EDD_STS_ERR_PARAM;
    *((LSA_UINT16*)&pRqbParam->DebugInfo[2]) = Status;
    *((LSA_UINT16*)&pRqbParam->DebugInfo[4]) = EDDS_MODULE_ID;
    *((LSA_UINT16*)&pRqbParam->DebugInfo[6]) = line;
    pRqbParam->DebugInfo[8] = 0x0;
    pRqbParam->DebugInfo[9] = 0x0;
    pRqbParam->DebugInfo[10] = 0x0;
    pRqbParam->DebugInfo[11] = 0x0;
    pRqbParam->DebugInfo[12] = 0x0;
    pRqbParam->DebugInfo[13] = 0x0;
    pRqbParam->DebugInfo[14] = 0x0;
    pRqbParam->DebugInfo[15] = 0x0;
}
//lint +esym(613,pRqbParam)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderControl                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE                +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_CONTROL_TYPE                                     +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control a  provider.                           +*/
/*+                                                                         +*/
/*+               We activate or deactivate a provider and store the        +*/
/*+               DataValid-Bit for sending.                                +*/
/*+                                                                         +*/
/*+               If only one provider is spezified we test for the provider+*/
/*+               is InUse. If not we set EDD_STS_ERR_PARAM.                +*/
/*+                                                                         +*/
/*+               If neccessary we start the cycle-timer or stop it if no   +*/
/*+               more provider and consumer are active.                    +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB,pCr)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB - pCR set null -> init
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderControl(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{


    LSA_RESULT                                  Status;
    EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE    pRqbParam;
    EDDS_DDB_COMP_CSRT_TYPE_PTR                 pCSRT;
    EDDS_CSRT_PROVIDER_TYPE_PTR                 pProv;
    LSA_UINT8                                   Mask;
    LSA_UINT8                                   Stat;
    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB			= pHDB->pDDB;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderControl(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

	Status = EDD_STS_OK;

    pRqbParam   = (EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE) pRQB->pParam;
    pCSRT       = &pDDB->pSRT->CSRT;

    if (( pRqbParam->ProviderID >= pCSRT->Cfg.ProviderCnt     ) ||
        ( EDDS_CSRT_PROVIDER_TYPE_NOT_USED == pCSRT->Prov.pProvTbl[pRqbParam->ProviderID].Type ))
    {
        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderControl: Invalid Provider ID (%d)!",
                            pRqbParam->ProviderID);

		Status = EDD_STS_ERR_PARAM;
        EDDS_SET_DEBUG_INFO_PROVIDER(pRqbParam, Status, (LSA_UINT16)__LINE__);
    }
    else
    {
		pProv = &pCSRT->Prov.pProvTbl[pRqbParam->ProviderID];

        /* ------------------------------------------------------------------*/
        /* Activate/Deactivate check                                         */
        /* ------------------------------------------------------------------*/
        if (( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE ) &&
            ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_PASSIVATE ))
        {
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_RTProviderControl: Invalid Mode: (0x%X))!",
                                pRqbParam->Mode);

            Status = EDD_STS_ERR_PARAM;

            EDDS_SET_DEBUG_INFO_PROVIDER(pRqbParam, Status, (LSA_UINT16)__LINE__);
        }
		else if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE )
        {
			LSA_UINT16                     CrNumber;
			LSA_BOOL                       doInsert = LSA_FALSE;
			EDDS_CSRT_PROVIDER_CR_TYPE_PTR pCr      = LSA_NULL;

			CrNumber = pRqbParam->IOParams.CRNumber;

			if( EDD_CRNUMBER_NOT_USED  == CrNumber ||
				pCSRT->Cfg.ProviderCnt <  CrNumber )
			{
				EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: Invalid cr number!");

				Status = EDD_STS_ERR_PARAM;
			}
			else
			{
				LSA_UINT16 DataLen;

				pCr = &pCSRT->Prov.pProvCrTbl[CrNumber - 1];   // 1 .. ProviderCnt

				DataLen = LE_TO_H_S(pCr->DataLen);   // ensure correct endianess!

				if(  DataLen   != pProv->DataLen ||   // data length of io data must match
					 pCr->Type != pProv->Type    ||   // matching provider type --> from allocate at IOBuffer
					!pCr->CrUsed )                    // Cr must be allocated previously
				{
					EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR, "EDDS_RTProviderControl: Invalid input parameter!");

					Status = EDD_STS_ERR_PARAM;
				}
			}

			if( Status == EDD_STS_OK )
			{
				LSA_UINT16 User;
				LSA_UINT16 XChange;
				LSA_UINT16 EDDS;

				// enter CrNumber
				pProv->CrNumber = CrNumber;

				User    = *((LSA_UINT8*)&pCr->User)    & ~EDDS_IOBUFFER_MASK_NEW_FLAG;
				XChange = *((LSA_UINT8*)&pCr->XChange) & ~EDDS_IOBUFFER_MASK_NEW_FLAG;
				EDDS    = *((LSA_UINT8*)&pCr->EDDS)    & ~EDDS_IOBUFFER_MASK_NEW_FLAG;

				if( EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED == pProv->CyclePhaseSequence ) {
					pProv->CyclePhaseSequence   =    pRqbParam->CyclePhaseSequence;
					doInsert = LSA_TRUE;
				}

	            /* check CycleReductionRatio */
	            if( (EDD_CYCLE_REDUCTION_RATIO_UNDEFINED == pProv->CycleReductionRatio) )
	            {
	                LSA_BOOL IsUdp = (pProv->Type & EDDS_CSRT_PROVIDER_TYPE_XRT) != EDDS_CSRT_PROVIDER_TYPE_XRT;

	                /* CycleReductionRatio still undefined on activation */
	                if (0 == EDDS_RTGetAndCheckReductionFactor(pRqbParam->CycleReductionRatio,IsUdp))
	                {
	                    Status = EDD_STS_ERR_PARAM;
	                }
	                else
	                {
	                    pProv->CycleReductionRatio = pRqbParam->CycleReductionRatio;
	                    doInsert = LSA_TRUE;
	                }
	            }

	            /* check CyclePhase */
	            if( (EDD_STS_OK == Status) &&  (EDD_CYCLE_PHASE_UNDEFINED == pProv->CyclePhase))
	            {
	                LSA_UINT16 CyclePhase;
	                CyclePhase = pRqbParam->CyclePhase;

	                /* CyclePhase still undefined on activation or out of boundary*/
	                if ( (EDD_CYCLE_PHASE_UNDEFINED == CyclePhase) ||
	                     (CyclePhase > pProv->CycleReductionRatio) )
	                {
	                    Status = EDD_STS_ERR_PARAM;
	                }
	                else
	                {
	                    pProv->CyclePhase = CyclePhase;
	                    doInsert = LSA_TRUE;
	                }
	            }

	            // with Activate we always need a valid FrameID
	            if(EDD_STS_OK == Status)
	            {
	                if(EDD_FRAME_ID_UNDEFINED == pRqbParam->FrameID)
	                {
	                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
	                                        "EDDS_RTProviderControl: FrameID still undefined on activation!");
	                    Status = EDD_STS_ERR_PARAM;
	                }
	                else
	                {
	                    LSA_UINT16 RtClassProp = pProv->Properties & EDD_CSRT_CONS_PROP_RTCLASS_MASK;

	                    /* ---------------------------------------------------- */
	                    /* check FrameID                                        */
	                    /* --> FrameID must match RTClass                       */
	                    /* NOTE: RT class property already checked on ADD       */
	                    /* ---------------------------------------------------- */
	                    if(EDD_CSRT_CONS_PROP_RTCLASS_1 == RtClassProp)
	                    {
	                        if ((EDD_SRT_FRAMEID_CSRT_START2 > pRqbParam->FrameID) ||
	                            (EDD_SRT_FRAMEID_CSRT_STOP2  < pRqbParam->FrameID))
	                        {
	                            Status = EDD_STS_ERR_PARAM;
	                        }
	                    }
	                    else if (EDD_CSRT_CONS_PROP_RTCLASS_2 == RtClassProp)
	                    {
	                        if ((EDD_SRT_FRAMEID_CSRT_START1 > pRqbParam->FrameID) ||
	                            (EDD_SRT_FRAMEID_CSRT_STOP1  < pRqbParam->FrameID))
	                        {
	                            Status = EDD_STS_ERR_PARAM;
	                        }
	                    } /* check FrameID */

	                    if(EDD_STS_OK == Status)
	                    {
	                        pProv->FrameID = pRqbParam->FrameID;
	                    }
	                }
	            }

	            if(EDD_STS_OK == Status)
	            {
                    /*------------------------------------------------------------- */
                    /* Because it is possible that the sendbuffer is still in send  */
                    /* (from previous use of this provider) we can not fill the     */
                    /* send buffer here! So we must fill the send framebuffer       */
                    /* later when we first send this frame. So we set the flag      */
                    /* ProviderDoSetupBuffer for the sender to do it later.         */
                    /*------------------------------------------------------------- */

                    pProv->DstMAC	= pRqbParam->DstMAC;
                    #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    pProv->DstIP	= pRqbParam->DstIP;
                    ///note remove pProv->SrcIP ?
                    #endif

                    /* setup Sendbuffer Src MAC */

                    EDDS_RTSetupProviderFrameSrcMAC( pCr->pBuffer[EDDS],
                                                     pDDB->pGlob->HWParams.MACAddress.MacAdr);

                    /* the rest will be setup later ! */

                    pProv->DoSetupBuffer = LSA_TRUE;

                    /* setup Xchangebuffer */

                    EDDS_RTSetupProviderFrameSrcMAC( pCr->pBuffer[XChange],
                                                     pDDB->pGlob->HWParams.MACAddress.MacAdr);

        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type )
                    {
                        EDDS_RTSetupProviderFrameUDP(pCSRT,
                                                     pCr->pBuffer[XChange],
                                                     pProv);
                    }
                    else
        #endif
                    {
                        EDDS_RTSetupProviderFrameXRT(pCr->pBuffer[XChange],
                                                     pProv->DataLen,
                                                     pRqbParam->DstMAC.MacAdr,
                                                     pProv->FrameID,
                                                     pProv->VLANID);
                    }

                    /* setup Userbuffer */

                    EDDS_RTSetupProviderFrameSrcMAC( pCr->pBuffer[User],
                                                     pDDB->pGlob->HWParams.MACAddress.MacAdr);

        #ifdef EDDS_XRT_OVER_UDP_INCLUDE
                    if ( EDDS_CSRT_PROVIDER_TYPE_UDP == pProv->Type )
                    {
                        EDDS_RTSetupProviderFrameUDP(pCSRT,
                                                     pCr->pBuffer[User],
                                                     pProv);
                    }
                    else
        #endif
                    {
                        EDDS_RTSetupProviderFrameXRT(pCr->pBuffer[User],
                                                     pProv->DataLen,
                                                     pProv->DstMAC.MacAdr,
                                                     pProv->FrameID,
                                                     pProv->VLANID);
                    }




                    if(doInsert) {
                        if ( EDD_STS_OK != (Status = EDDS_RTProviderInsert(pCSRT,pRqbParam->ProviderID)))
                        {
                            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_RTProviderControl: EDDS_RTProviderInsert with ProviderID 0x%X failed!",
                                                pRqbParam->ProviderID);
                            EDDS_SET_DEBUG_INFO_PROVIDER(pRqbParam, Status, (LSA_UINT16)__LINE__);
                        }
                    }
	            }
			}
		}
        /* ------------------------------------------------------------------*/
        /* Autostop parameter check                                          */
        /* ------------------------------------------------------------------*/
        if ( Status == EDD_STS_OK )
        if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP )
        {
            if ( pRqbParam->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
            {
                if ( pRqbParam->AutoStopConsumerID >= pCSRT->Cons.ConsumerCnt )
                {
                    EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_RTProviderControl: Invalid AutoStopConsumerID: (0x%X))!",
                                        pRqbParam->AutoStopConsumerID);

                    Status = EDD_STS_ERR_PARAM;

                    EDDS_SET_DEBUG_INFO_PROVIDER(pRqbParam, Status, (LSA_UINT16)__LINE__);
                }
                else
                {
                    if ( ! pCSRT->Cons.pConsTbl[pRqbParam->AutoStopConsumerID].Type )
                    {
                        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                            "EDDS_RTProviderControl: Consumer not in use for AutoStopConsumerID: (0x%X))!",
                                            pRqbParam->AutoStopConsumerID);

                        Status = EDD_STS_ERR_PARAM;

                        EDDS_SET_DEBUG_INFO_PROVIDER(pRqbParam, Status, (LSA_UINT16)__LINE__);
                    }
                }
            }
        }

        /* ------------------------------------------------------------------*/
        /* All parameters ok.                                                */
        /* ------------------------------------------------------------------*/

        if ( Status == EDD_STS_OK )
        {

            EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                        "EDDS_RTProviderControl:  ProvID: %d, Mode: 0x%X",
                        pRqbParam->ProviderID,
                        pRqbParam->Mode);

            /* if we should stop, do it first */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_PASSIVATE )
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderControl: Deactivate Provider %d",
                                    pRqbParam-> ProviderID);

                EDDS_RTProviderStop(pCSRT,pProv);
            }

            /* Set AutoStop */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_AUTOSTOP )
            {
                /* currently Autostop used ? */
                if ( pProv->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS )
                {

                    EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                        "EDDS_RTProviderControl: Autostop disabled ProvId: %d",
                                        pRqbParam-> ProviderID);


                    /* decrement number of Providers using this consumer with Autostop */
                    pCSRT->Cons.pConsTbl[pProv->AutoStopConsumerID].ProviderAutoStopCnt--;
                }

                /* new Autostop set ? */
                if ( pRqbParam->AutoStopConsumerID != EDD_CONSUMER_ID_REMOVE_AS)
                {
                    EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                        "EDDS_RTProviderControl: Autostop Set ProvId: %d, ConsID: %d",
                                        pRqbParam-> ProviderID,
                                        pRqbParam-> AutoStopConsumerID);

                    /* increment number of Providers using this consumer with Autostop */
                    pCSRT->Cons.pConsTbl[pRqbParam->AutoStopConsumerID].ProviderAutoStopCnt++;
                }

                /* save new Autostop setting */
                pProv->AutoStopConsumerID = pRqbParam->AutoStopConsumerID;
            }

            /* Set DataStatus */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_STATUS )
            {

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                     "EDDS_RTProviderControl: Set DataStatus for ProvId: %d",
                                     pRqbParam-> ProviderID);

                Mask = pRqbParam->Mask;
                Stat = pRqbParam->Status;

                Mask &= EDD_CSRT_DSTAT_LOCAL_MASK_ALL;
                Stat &= EDD_CSRT_DSTAT_LOCAL_MASK_ALL;

                pProv->DataStatus = ( pProv->DataStatus & ~Mask) | (Stat & Mask);

            }

            /* if we should start, do it after all is setup */
            if ( pRqbParam->Mode & EDD_PROV_CONTROL_MODE_ACTIVATE )
            {
                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                    "EDDS_RTProviderControl: Activate Provider %d.",
                                    pRqbParam->ProviderID);

                EDDS_RTProviderStart(pCSRT,pProv);
            }

        }
    }

    LSA_UNUSED_ARG(pHDB);

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderControl(), Status: 0x%X",
                           Status);

    return(Status);


}
//lint +esym(613,pCr,pHDB,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderSetState                     +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+               pParam: EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE              +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  EDD_RQB_CSRT_PROVIDER_SET_STATE_TYPE                                   +*/
/*+                                                                         +*/
/*+  GroupID    : Providergroup to set status for                           +*/
/*+  Status     : Status with bitvalues                                     +*/
/*+               EDD_CSRT_DSTAT_BIT_STOP_RUN                               +*/
/*+               EDD_CSRT_DSTAT_BIT_STATION_FAILURE                        +*/
/*+  Mask       : Mask for Status (spezifies bits to change)                +*/
/*+               EDD_CSRT_DSTAT_BIT_STOP_RUN                               +*/
/*+               EDD_CSRT_DSTAT_BIT_STATION_FAILURE                        +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to set gobal provider state .                     +*/
/*+                                                                         +*/
/*+               This will set Bits of DataStatus for all providers        +*/
/*+               within the same providergroup.                            +*/
/*+                                                                         +*/
/*+               NewState = (AktState & NOT Mask) OR ( Status & Mask)      +*/
/*+                                                                         +*/
/*+               The RQB has to be valid (opcode/request) not checked!.    +*/
/*+               pParam has to be present (<> LSA_NULL)                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 pRQB checked within EDDS_Request - pHDB checked within EDDS_HandleGetHDB
//WARNING: be careful when using this function, make sure not to use pRGB/pHDB as null ptr!s
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderSetState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                  Status;
    EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE  pRqbParam;
    LSA_UINT8                                   n;
    LSA_UINT8                                   Stat;
    LSA_UINT8                                   Mask;
    EDDS_LOCAL_DDB_PTR_TYPE     				pDDB			= pHDB->pDDB;

    EDDS_CRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderSetState(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status = EDD_STS_OK;

    pRqbParam = (EDD_UPPER_CSRT_PROVIDER_SET_STATE_PTR_TYPE) pRQB->pParam;

    if ( pRqbParam->GroupID >=  EDD_CFG_CSRT_MAX_PROVIDER_GROUP )
    {

        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_RTProviderSetState: Invalid GroupID (%d)!",
                            pRqbParam->GroupID);

        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        /* mask to valid bits */

        Mask = pRqbParam->Mask   & EDD_CSRT_DSTAT_GLOBAL_MASK_ALL;
        Stat = pRqbParam->Status & EDD_CSRT_DSTAT_GLOBAL_MASK_ALL;

        n = pDDB->pSRT->CSRT.Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus;

        /* this will not conflict with current sender, because one access only */

        pDDB->pSRT->CSRT.Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus = ( n & ~Mask) | (Stat & Mask);

        EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
                            "EDDS_RTProviderSetState: ProviderGroup %d. DataStatus: 0x%X",
                            pRqbParam->GroupID,
                            pDDB->pSRT->CSRT.Prov.pProvGroupTbl[pRqbParam->GroupID].ProviderDataStatus);

    }

    LSA_UNUSED_ARG(pHDB);


    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderSetState(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_RTProviderGetCycleCounter              +*/
/*+  Input                 :    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT           +*/
/*+                             EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pCSRT        : Pointer to CSRT-Management                              +*/
/*+  pCycleCounter: Pointer to address for CycleCounter                     +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:  Gets actual provider CycleCounter of cyclic SRT.         +*/
/*+                                                                         +*/
/*+                The CycleCounter is based on 31.25us and wraps around    +*/
/*+                if 32Bit is reached. The function can be called anytime  +*/
/*+                except for startup/shutdown of srt-component             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCSRT)
//JB 18/11/2014 can not be a null pointer (previously taken from DDB)
//WARNING: be careful when using this function, make sure not to use pCSRT as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RTProviderGetCycleCounter(
    EDDS_DDB_COMP_CSRT_TYPE_PTR pCSRT,
    EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter)
{


    LSA_RESULT Status;

    EDDS_CRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_RTProviderGetCycleCounter(pCSRT: 0x%X, pCycleCounter: 0x%X)",
                           pCSRT,
                           pCycleCounter);

    Status = EDD_STS_OK;

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pCycleCounter,LSA_NULL))
    {
        pCycleCounter->Low  = ((pCSRT->Prov.ProviderCycle.CycleCounter) << pCSRT->CycleStampAdjustShift);
        pCycleCounter->High = 0;
    }
    else
    {
        Status = EDD_STS_ERR_PARAM;
    }

    EDDS_CRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_RTProviderGetCycleCounter(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pCSRT)


#endif

/*****************************************************************************/
/*  end of file SRT_PROV.C                                                   */
/*****************************************************************************/



