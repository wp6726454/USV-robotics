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
/*  F i l e               &F: edds_nrt_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NRT (non real time) for EDDS.                    */
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
/*  22.10.03    JS    Optional padding of NRT-Frames to 64 Bytes.            */
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  05.04.04    JS    Using LSA_UNUSED_ARG                                   */
/*  15.06.04    JS    ASRT over NRT always used                              */
/*  28.06.04    JS    Changed variable name/Type InUse to State              */
/*  04.10.04    JS    Support for multiple DCP-Identify filters.             */
/*                    Bugfix. Deactivation of DCP-Filter within channel close*/
/*                    only with DCP-channels, not always.                    */
/*  14.01.05    JS    little changes to avoid compiler warnings              */
/*  31.01.05    JS    LTRC support                                           */
/*  17.03.05    JS    bugfix. decrement TxQueueReqCnt on cancel.             */
/*  09.08.05    JS    EDDS_NRT_FRAME_PTP renamed to EDD_NRT_FRAME_PTCP       */
/*  12.01.06    JS    added new NRT-Filter: EDD_NRT_FRAME_MRP                */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  10.10.06    JS    added NRT_TX_PRIO_3 prio                               */
/*  22.11.06    JS    added new NRT-Filters: EDD_NRT_FRAME_IP_ICMP           */
/*                                           EDD_NRT_FRAME_IP_IGMP           */
/*                                           EDD_NRT_FRAME_IP_TCP            */
/*                                           EDD_NRT_FRAME_IP_UDP            */
/*                                           EDD_NRT_FRAME_IP_OTHER          */
/*                                           EDD_NRT_FRAME_ARP               */
/*                                           EDD_NRT_FRAME_RARP              */
/*  27.02.07    JS    added new NRT-Filter : NRT_FILTER_XRT_DCP_HELLO_IDX    */
/*  22.01.08    JS    added new NRT-Filter : EDD_NRT_FRAME_LEN_STDBY         */
/*  07.05.09    JS    added new NRT-Filter : EDD_NRT_FRAME_LEN_HSR           */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  128
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_NRT_USR */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_NRT_INCLUDE

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

#ifdef EDDS_CFG_LOCAL_MEM_STATIC
EDDS_STATIC EDDS_LOCAL_MEM_ATTR EDDS_HDB_COMP_NRT_TYPE g_EDDS_HDB_NRT[EDDS_CFG_MAX_CHANNELS];
EDDS_STATIC EDD_RQB_TYPE                                g_EDDS_HDB_NRT_CANCEL_RQB[EDDS_CFG_MAX_CHANNELS];
EDDS_STATIC EDDS_RQB_DEV_INTERNAL_TYPE                g_EDDS_HDB_NRT_CANCEL_RQB_PARAM[EDDS_CFG_MAX_CHANNELS];
#endif

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NRTFreeRxDscr                               +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                             NRT_DSCR_MGM_TYPE_PTR   pNRTCompRxDscr      +*/
/*+                             LSA_UINT32              RxBufferOffset      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pNRTCompRxDscr Pointer to NRT Rx-Dscriptor managment to be freed       +*/
/*+  RxBufferOffset Offset in Receive-Buffer where frame will be stored     +*/
/*+                 (needed to caclulate the real buffer start address)     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees all memory used in Rx-Dscriptors.                   +*/
/*+               Note that the Rx-Buffers are not the same allocated inside+*/
/*+               because they maybe exchanged with buffers from user or    +*/
/*+               in eib-functions.                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pNRTCompRxDscr,pDDB)
//JB 18/11/2014 checked before function call
//WARNING: be careful when using this function, make sure not to use pNRTCompRxDscr or pDDB as null ptr!
//lint -esym(715,pDDB)
//JB 09/12/2014 system adaption (#JB_DISCUSS in code?)
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR NRTFreeRxDscr(EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
                                                    EDDS_NRT_DSCR_MGM_TYPE_PTR           pNRTCompRxDscr
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                    ,
                                                    LSA_UINT32                      RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                    )
{
    LSA_UINT16                      RetVal = LSA_RET_OK;
    LSA_UINT32                      i;
    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer;

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_NRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :NRTFreeRxDscr(pNRTCRDscr: 0x%X,RxBOff: 0x%X)",
                           pNRTCompRxDscr,
                           RxBufferOffset);
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    if ( ! LSA_HOST_PTR_ARE_EQUAL(pNRTCompRxDscr->pRxDscrStart,LSA_NULL))
    {

        /*-----------------------------------------------------------------------*/
        /* Free all Rx-Buffers. Note that this buffers maybe not the same which  */
        /* we have allocated in startup!! (because they will be exchanged with   */
        /* user)                                                                 */
        /*-----------------------------------------------------------------------*/

        for ( i = 0; i< pNRTCompRxDscr->RxDscrCnt; i++)
        {

            if (! LSA_HOST_PTR_ARE_EQUAL(pNRTCompRxDscr->pRxDscrStart[i].pBuffer,LSA_NULL))
            {

                /* calculate the pointer used when allocated (- RxBufferOffset!)   */

                pBuffer = ((EDD_UPPER_MEM_U8_PTR_TYPE)(pNRTCompRxDscr->pRxDscrStart[i].pBuffer) )
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                        - RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                        ;

                EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,(EDD_UPPER_MEM_PTR_TYPE)pBuffer);
                EDDS_FREE_CHECK(RetVal);

                pNRTCompRxDscr->pRxDscrStart[i].pBuffer = LSA_NULL;
            }
        }

        #ifdef EDDS_CFG_NRT_FAST_MEM
        EDDS_FREE_LOCAL_FAST_MEM(&RetVal,pNRTCompRxDscr->pRxDscrStart);
        #else
        EDDS_FREE_LOCAL_MEM(&RetVal,pNRTCompRxDscr->pRxDscrStart);
        #endif

        EDDS_FREE_CHECK(RetVal);

        pNRTCompRxDscr->pRxDscrStart = LSA_NULL;

    }


    EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:NRTFreeRxDscr()");


}
//lint +esym(715,pDDB)
//lint +esym(613,pNRTCompRxDscr,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NRTInitRxDscr                               +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE       pDDB          +*/
/*+                             NRT_DSCR_MGM_TYPE_PTR         pNRTCompRxDscr+*/
/*+                             LSA_UINT32                    RxBufferCnt   +*/
/*+                             LSA_UINT32                    RxBufferSize  +*/
/*+                             LSA_UINT32                    RxBufferOffset+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pNRTCompRxDscr Pointer to NRT Rx-Dscriptor managment to be filled      +*/
/*+  RxBufferCnt  : Number of Rx-Descr and receive buffers to allocate (>0) +*/
/*+  RxBufferSize : Size of one Receive-buffer                              +*/
/*+  RxBufferOffset Offset in Receive-Buffer where frame will be stored     +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+               EDD_STS_ERR_ALIGN    (allocated buffer not propably align)+*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function allocates and initializes the nrt receive   +*/
/*+               descriptor management. RxBufferSize and RxBufferOffset    +*/
/*+               is used because of zero-copy interface with user to       +*/
/*+               leave room in buffer before the frame. This function fills+*/
/*+               the pRxDscrStart with rx-buffer pointers.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pNRTCompRxDscr,pDDB)
//JB 07/11/2014 checked where called
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR NRTInitRxDscr(EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
                                                    EDDS_NRT_DSCR_MGM_TYPE_PTR       pNRTCompRxDscr,
                                                    LSA_UINT32                       RxBufferCnt,
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                    LSA_UINT32                       RxBufferSize,
                                                    LSA_UINT32                       RxBufferOffset,
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                    LSA_UINT32                       RxDscrLookUpCntMax)
{

    LSA_RESULT                      Status;
    LSA_UINT32                      i;
    LSA_UINT16                      RetVal;
    EDD_UPPER_MEM_PTR_TYPE          pBuffer;
    LSA_UINT32                      Size;
    LSA_UINT32                      Addr;


#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    EDDS_NRT_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :NRTInitRxDscr(pNRTCRDscr: 0x%X,Cnt: %d,Size: %d,Off: %d)",
                           pNRTCompRxDscr,
                           RxBufferCnt,
                           RxBufferSize,
                           RxBufferOffset);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    Status = EDD_STS_OK;

    /*-------------------------------------------------------------------------*/
    /* Allocate memory for Receive-Descriptors                                 */
    /*-------------------------------------------------------------------------*/


    pNRTCompRxDscr->RxDscrCnt   = RxBufferCnt;
    pNRTCompRxDscr->RxFillCnt   = 0;
    pNRTCompRxDscr->RxCheckCnt  = 0;

    Size = sizeof(EDDS_NRT_RX_DSCR_TYPE) * RxBufferCnt;


    #ifdef EDDS_CFG_NRT_FAST_MEM
    EDDS_ALLOC_LOCAL_FAST_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pNRTCompRxDscr->pRxDscrStart,Size);
    #else
    EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pNRTCompRxDscr->pRxDscrStart),Size);
    #endif

    if (LSA_HOST_PTR_ARE_EQUAL(pNRTCompRxDscr->pRxDscrStart,LSA_NULL))
        Status = EDD_STS_ERR_RESOURCE;
    else
        EDDS_MEMSET_LOCAL(pNRTCompRxDscr->pRxDscrStart,0,Size);

    /*-------------------------------------------------------------------------*/
    /* Allocate memory for Receive-Buffers. This memory will be allocated      */
    /* in upper transfer mem, because it may be exchanged with user.           */
    /* We must allocated each buffer seperatly because they will be exchanged  */
    /* with the user!                                                          */
    /*-------------------------------------------------------------------------*/

    if ( EDD_STS_OK == Status )
    {
        i = 0;

        while (i< RxBufferCnt)
        {

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer, RxBufferSize);
#else /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&pBuffer,EDD_FRAME_BUFFER_LENGTH);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

            if (LSA_HOST_PTR_ARE_EQUAL(pBuffer,LSA_NULL))
            {
                Status = EDD_STS_ERR_RESOURCE;
            }
            else
            {

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                EDDS_PTR_TO_ADDR_UPPER(pDDB->hSysDev,(EDD_UPPER_MEM_PTR_TYPE)pBuffer,&Addr);

                Addr += RxBufferOffset;
#else
                Addr = (LSA_UINT32)pBuffer;
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

                if ( (LSA_UINT32)((Addr)) & EDDS_RX_FRAME_BUFFER_ALIGN ) /* alignment check */
                {
                    Status = EDD_STS_ERR_ALIGN;
                    EDDS_FREE_RX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&RetVal,pBuffer);  //lint !e774 JB 09/12/2014 system adaption
                    EDDS_FREE_CHECK(RetVal);

                }
                else
                {
                    /* store pointer to buffer+offset (will be the frame-start !)*/

                    pNRTCompRxDscr->pRxDscrStart[i].State   = EDDS_NRT_DSCR_STATE_FREE;
                    pNRTCompRxDscr->pRxDscrStart[i].pBuffer = ((EDD_UPPER_MEM_U8_PTR_TYPE)pBuffer)
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                            + RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                            ;
                }
            }

            i++;

        } /* while */
    } /* if */

    pNRTCompRxDscr->RxDscrLookUpCntMax = RxDscrLookUpCntMax;

    if ( Status != EDD_STS_OK )
    {
        NRTFreeRxDscr(pDDB,pNRTCompRxDscr
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                      ,RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                      );
    }

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:NRTInitRxDscr(), Status: 0x%X",
                           Status);


    return(Status);


}
//lint +esym(613,pNRTCompRxDscr,pDDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTInitComponent                       +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_COMP_NRT_INI       +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_NRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_NRT_INI           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the NRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pDDB)
//JB 18/11/2014 pRQB checked before function call - pDDB set LSA_NULL and initiated
//WARNING: be careful when using this function, make sure not to use pRQB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTInitComponent(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_CMP_NRT_INI_PTR_TYPE pRqbCMP;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    LSA_UINT16                      RetVal;
    LSA_UINT32                      i;

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTInitComponent(pRQB: 0x%X)",
                           pRQB);


    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTInitComponent: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {

        pRqbCMP = (EDDS_UPPER_CMP_NRT_INI_PTR_TYPE)pRQB->pParam;
        pDDB    = LSA_NULL; /* to avoid compiler warning */

        /* check for valid parameters */

        if (( pRqbCMP->RxBufferCntARPIP      == 0 ) ||
            ( pRqbCMP->RxBufferCntASRTOther  == 0 ) ||
            ( pRqbCMP->RxBufferCntASRTAlarm  == 0 ) ||
            ( pRqbCMP->RxBufferCntOther      == 0 ))
        {

            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTInitComponent: Invalid RQB parameter!");
            Status = EDD_STS_ERR_PARAM;
        }
        else Status = EDDS_GetDDB(pRqbCMP->hDDB, &pDDB);

        if (Status == EDD_STS_OK)
        {
            /* check if not already initialized. If so report error */

            if ( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL) )
            {
                EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTInitComponent: NRT already initialized!");
                Status = EDD_STS_ERR_SEQUENCE;
            }
            else
            {

                #ifdef EDDS_CFG_NRT_FAST_MEM
                EDDS_ALLOC_LOCAL_FAST_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pNRTComp, sizeof(EDDS_DDB_COMP_NRT_TYPE));
                #else
                EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pNRTComp), sizeof(EDDS_DDB_COMP_NRT_TYPE));
                #endif
                if ( LSA_HOST_PTR_ARE_EQUAL(pNRTComp, LSA_NULL) )
                {
                    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTInitComponent: Allocating NRT-memory failed!");
                    Status = EDD_STS_ERR_RESOURCE;
                }
                else
                {
                    /* init with 0 */

                    EDDS_MEMSET_LOCAL(pNRTComp,0,sizeof(EDDS_DDB_COMP_NRT_TYPE));

                    pDDB->pNRT = pNRTComp;

                    pNRTComp->HandleCnt   = 0; /* currently no handles open */

                    for ( i=0; i< EDDS_NRT_DCP_FILTER_CNT; i++)
                    {
                        pNRTComp->DCPFilter[i].pDCP_TLVFilter = LSA_NULL; /* no DCP Filter   */
                    }

                    pNRTComp->ARPFilter.InUse = LSA_FALSE;

                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_0].InProgCnt = 0;
                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_1].InProgCnt = 0;
                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_2].InProgCnt = 0;

                    /* setup limits for sending */

                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_0].InProgCntMax = EDDS_CFG_NRT_MAX_SEND_AT_ONCE_LOW;
                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_1].InProgCntMax = EDDS_CFG_NRT_MAX_SEND_AT_ONCE_MEDIUM;
                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_2].InProgCntMax = EDDS_CFG_NRT_MAX_SEND_AT_ONCE_HIGH;
                    pNRTComp->Tx[EDDS_NRT_TX_PRIO_3].InProgCntMax = EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG;

                    /* setup FilterInfo */

                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_ARP_IDX].FrameFilter           = EDD_NRT_FRAME_ARP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_DCP_IDX].FrameFilter       = EDD_NRT_FRAME_LEN_DCP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_TSYNC_IDX].FrameFilter     = EDD_NRT_FRAME_LEN_TSYNC;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_OTHER_IDX].FrameFilter     = EDD_NRT_FRAME_LEN_OTHER;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_OTHER_IDX].FrameFilter         = EDD_NRT_FRAME_OTHER;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_DCP_IDX].FrameFilter       = EDD_NRT_FRAME_DCP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_OTHER_LLDP_IDX].FrameFilter    = EDD_NRT_FRAME_LLDP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_ASRT_IDX].FrameFilter      = EDD_NRT_FRAME_ASRT;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_UDP_ASRT_IDX].FrameFilter  = EDD_NRT_FRAME_UDP_ASRT;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_UDP_DCP_IDX].FrameFilter   = EDD_NRT_FRAME_UDP_DCP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_PTP_SYNC_IDX].FrameFilter  = EDD_NRT_FRAME_PTCP_SYNC;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_PTP_ANNO_IDX].FrameFilter  = EDD_NRT_FRAME_PTCP_ANNO;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_PTP_DELAY_IDX].FrameFilter = EDD_NRT_FRAME_PTCP_DELAY;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_OTHER_MRP_IDX].FrameFilter     = EDD_NRT_FRAME_MRP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_IP_ICMP_IDX].FrameFilter       = EDD_NRT_FRAME_IP_ICMP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_IP_IGMP_IDX].FrameFilter       = EDD_NRT_FRAME_IP_IGMP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_IP_TCP_IDX].FrameFilter        = EDD_NRT_FRAME_IP_TCP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_IP_UDP_IDX].FrameFilter        = EDD_NRT_FRAME_IP_UDP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_IP_OTHER_IDX].FrameFilter      = EDD_NRT_FRAME_IP_OTHER;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_RARP_IDX].FrameFilter          = EDD_NRT_FRAME_RARP;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_DCP_HELLO_IDX].FrameFilter = EDD_NRT_FRAME_DCP_HELLO;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_XRT_UDP_DCP_HELLO_IDX].FrameFilter = EDD_NRT_FRAME_UDP_DCP_HELLO;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_STDBY_IDX].FrameFilter         = EDD_NRT_FRAME_LEN_STDBY;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_HSR_IDX].FrameFilter           = EDD_NRT_FRAME_LEN_HSR;
                    pNRTComp->FilterInfo[EDDS_NRT_FILTER_LEN_SINEC_FWL_IDX].FrameFilter     = EDD_NRT_FRAME_LEN_SINEC_FWL;

                    pNRTComp->RxFilterUDP_Broadcast = pRqbCMP->RxFilterUDP_Broadcast;
                    pNRTComp->RxFilterUDP_Unicast = pRqbCMP->RxFilterUDP_Unicast;

                    /*----------------------------------------------------*/
                    /* Allocate/Init DCP-Hello Structure                  */
                    /*----------------------------------------------------*/

                    EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE*)&pNRTComp->pDCPHelloFilter), sizeof(EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_TYPE));

                    if ( LSA_HOST_PTR_ARE_EQUAL(pNRTComp->pDCPHelloFilter, LSA_NULL) )
                    {
                        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTInitComponent: Allocating NRT-memory failed!");
                        Status = EDD_STS_ERR_RESOURCE;
                    }
                    else
                    {
                        EDDS_MEMSET_LOCAL(pNRTComp->pDCPHelloFilter,0,sizeof(EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_TYPE));

                        for ( i=0; i< EDD_DCP_MAX_DCP_HELLO_FILTER; i++)
                        {
                            pNRTComp->pDCPHelloFilter->Filter[i].InUse = LSA_FALSE;    /* no DCP Filter   */
                        }
                    }

                    /*----------------------------------------------------*/
                    /* Setup Rx-Descriptors for ARP and IP-Frames         */
                    /*----------------------------------------------------*/
                    if ( Status == EDD_STS_OK )
                    {
                        Status = NRTInitRxDscr( pDDB,
                                                &pNRTComp->RxDscrMgm[EDDS_NRT_BUFFER_IP_IDX],
                                                pRqbCMP->RxBufferCntARPIP,
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                pDDB->pGlob->RxBufferSize,
                                                pDDB->pGlob->RxBufferOffset,
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                EDDS_NRT_RX_DSCR_MAX_LOOKUP_ARPIP);
                    }

                    /*----------------------------------------------------*/
                    /* Setup Rx-Descriptors for ASRT Other Frames         */
                    /*----------------------------------------------------*/
                    if ( Status == EDD_STS_OK )
                    {

                        Status = NRTInitRxDscr( pDDB,
                                                &pNRTComp->RxDscrMgm[EDDS_NRT_BUFFER_ASRT_OTHER_IDX],
                                                pRqbCMP->RxBufferCntASRTOther,
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                pDDB->pGlob->RxBufferSize,
                                                pDDB->pGlob->RxBufferOffset,
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_OTHER);
                    }

                    /*----------------------------------------------------*/
                    /* Setup Rx-Descriptors for other  TYPE-Frames        */
                    /*----------------------------------------------------*/
                    if ( Status == EDD_STS_OK )
                    {

                        Status = NRTInitRxDscr( pDDB,
                                                &pNRTComp->RxDscrMgm[EDDS_NRT_BUFFER_OTHER_IDX],
                                                pRqbCMP->RxBufferCntOther,
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                pDDB->pGlob->RxBufferSize,
                                                pDDB->pGlob->RxBufferOffset,
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                EDDS_NRT_RX_DSCR_MAX_LOOKUP_OTHER);
                    }

                    /*----------------------------------------------------*/
                    /* Setup Rx-Descriptors for ASRT Alarm -Frames        */
                    /*----------------------------------------------------*/
                    if ( Status == EDD_STS_OK )
                    {

                        Status = NRTInitRxDscr( pDDB,
                                                &pNRTComp->RxDscrMgm[EDDS_NRT_BUFFER_ASRT_ALARM_IDX],
                                                pRqbCMP->RxBufferCntASRTAlarm,
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                pDDB->pGlob->RxBufferSize,
                                                pDDB->pGlob->RxBufferOffset,
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                EDDS_NRT_RX_DSCR_MAX_LOOKUP_ASRT_ALARM);
                    }

                    /*----------------------------------------------------*/
                    /* Setup look up counters                             */
                    /*----------------------------------------------------*/
                    pNRTComp->RxDscrMaxLookUpCnt = EDDS_NRT_RX_DSCR_MAX_LOOKUP;

                    EDDS_NRTRxResetDscrLookUpCounter(pNRTComp);


                    if ( Status != EDD_STS_OK )
                    {
                        LSA_UINT16 idx;

                        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTInitComponent: NRT initalization failed! (0x%X)",Status);

                        if ( ! LSA_HOST_PTR_ARE_EQUAL(pNRTComp->pDCPHelloFilter, LSA_NULL) )
                        {
                            EDDS_FREE_LOCAL_MEM(&RetVal,pNRTComp->pDCPHelloFilter);
                            EDDS_FREE_CHECK(RetVal);
                        }

                        /* Note: we can call NRTFreeRxDscr even on not initalized*/
                        /*       parts, because the pointers are NULL which is   */
                        /*       checked.                                        */

                        for (idx = 0 ; idx < EDDS_NRT_BUFFER_MAX_IDX ; idx++)
                        {
                            NRTFreeRxDscr(pDDB,&pNRTComp->RxDscrMgm[idx]
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                                    , pDDB->pGlob->RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                                    );
                        }

                        #ifdef EDDS_CFG_NRT_FAST_MEM
                        EDDS_FREE_LOCAL_FAST_MEM(&RetVal,pNRTComp);
                        #else
                        EDDS_FREE_LOCAL_MEM(&RetVal,pNRTComp);
                        #endif
                        EDDS_FREE_CHECK(RetVal);

                        pDDB->pNRT = LSA_NULL;

                    }

                }

            }
        }
        else
        {
            EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTInitComponent: Invalid RQB parameter or wrong hDDB!");
            Status = EDD_STS_ERR_PARAM;  /* if DDB not found  or invalid params */
        }

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_COMP_NRT_INI finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbCMP->Cbf,pRQB);
    }

    EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTInitComponent()");


}
//lint +esym(613,pRQB,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRelComponent                        +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_DEV_COMP_NRT_REL       +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.      +*/
/*+                                                                         +*/
/*+  pParam points to EDDS_RQB_CMP_NRT_REL_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     EDD_OPC_SYSTEM                  +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user                      +*/
/*+     LSA_COMP_ID_TYPE        CompId:                                     +*/
/*+     EDD_SERVICE             Service:    EDDS_SRV_COMP_NRT_REL           +*/
/*+     LSA_RESULT              Status:     EDD_STS_OK                      +*/
/*+                                         EDD_STS_ERR_SEQUENCE            +*/
/*+                                         EDDS_STS_ERR_RESOURCES          +*/
/*+                                         EDD_STS_ERR_PARAM               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will release the NRT-Component management   +*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be freed.                                            +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the any channel      +*/
/*+               uses this component and the device is still setup         +*/
/*+               (device-shutdown has to be called first!)                 +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB)
//JB 18/11/2014 checked where called
//WARNING: be careful when using this function, make sure not to use pRQB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRelComponent(EDD_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_RESULT                      Status;
    EDDS_UPPER_CMP_NRT_REL_PTR_TYPE pRqbCMP;
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    LSA_UINT16                      RetVal;

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTRelComponent(pRQB: 0x%X)",
                           pRQB);


    if ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTRelComponent: RQB->pParam is NULL!");
        EDDS_RQB_SET_STATUS(pRQB, EDD_STS_ERR_PARAM);
        EDDS_RQB_ERROR(pRQB);
    }
    else
    {
        pRqbCMP = (EDDS_UPPER_CMP_NRT_REL_PTR_TYPE)pRQB->pParam;

        Status = EDDS_GetDDB(pRqbCMP->hDDB, &pDDB);

        if (Status == EDD_STS_OK)
        {
            /* check if initialized. If not return with ok */

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT, LSA_NULL) )
            {

                /* check for open channels using this component. */
                /* if open channels, return with error.          */
                /* If hardware is setup (i.e. no device-shutdown */
                /* called yet) we return with error.             */

                if (( ! pDDB->pGlob->HWIsSetup  ) &&
                    ( ! pDDB->pNRT->HandleCnt   ))
                {
                    LSA_UINT16 idx;

                    /* free DCP Hello Filter mgmt */
                    if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pNRT->pDCPHelloFilter, LSA_NULL) )
                    {
                        EDDS_FREE_LOCAL_MEM(&RetVal,pDDB->pNRT->pDCPHelloFilter);
                        EDDS_FREE_CHECK(RetVal);
                    }

                    for (idx = 0 ; idx < EDDS_NRT_BUFFER_MAX_IDX ; idx++)
                    {
                        NRTFreeRxDscr(pDDB, &pDDB->pNRT->RxDscrMgm[idx]
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                                                                   , pDDB->pGlob->RxBufferOffset
#endif /* #if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                                                                   );
                    }
                    /* we continue on error */

                    #ifdef EDDS_CFG_NRT_FAST_MEM
                    EDDS_FREE_LOCAL_FAST_MEM(&RetVal,pDDB->pNRT);
                    #else
                    EDDS_FREE_LOCAL_MEM(&RetVal,pDDB->pNRT);
                    #endif
                    EDDS_FREE_CHECK(RetVal);

                    pDDB->pNRT = LSA_NULL;

                }
                else
                {
                    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTRelComponent: Handles still open and/or hardware still setup!");
                    Status = EDD_STS_ERR_SEQUENCE;
                }

            }
            else Status = EDD_STS_OK;
        }
        else
        {
            EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTRelComponent: Invalid hDDB (0x%X)!",pRqbCMP->hDDB);
            Status = EDD_STS_ERR_PARAM;  /* if DDB not found */
        }

        EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- Request: EDDS_SRV_DEV_COMP_NRT_REL finished. Response: 0x%X",Status);

        EDDS_RQB_SET_STATUS(pRQB, Status);
        EDDS_CallCbf(pRqbCMP->Cbf,pRQB);
    }

    EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTRelComponent()");


}
//lint +esym(613,pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTOpenChannel                         +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+                             EDDS_UPPER_CDB_NRT_PTR_TYPE  pCDB           +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel (<  EDDS_CFG_MAX_CHANNELS)        +*/
/*+  pCDB       : Pointer to filled ChannelDescriptionBlock parameters      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_CHANNEL_USE Filter already in use by channel  +*/
/*+               EDDS_STS_ERR_RESOURCES  Alloc failed                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Opens Channel for NRT. The Handle will specify the Channel+*/
/*+               and pDDB the Device. pCDB has all NRT parameters to use   +*/
/*+               for this channel (e.g. Filters)                           +*/
/*+                                                                         +*/
/*+               Each Frametype (Filter) can only be used by one channel   +*/
/*+               but a channel can register for more filters (ORed)        +*/
/*+                                                                         +*/
/*+               It is possible, that the channel dont uses any filter     +*/
/*+               (in this case no receive is possible but sending)         +*/
/*+                                                                         +*/
/*+               It is not checked if the handle is already open!           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pCDB,pHDB)
//JB 07/11/2014 checked where called
//WARNING: Be careful when using this function, make sure not to use pCDB or pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTOpenChannel(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
    EDDS_UPPER_CDB_NRT_PTR_TYPE pCDB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    LSA_UINT32                      RxIdx;
    EDDS_LOCAL_HDB_COMP_NRT_PTR_TYPE pNRTHdl;
    LSA_UINT32                      i;
    #ifndef EDDS_CFG_LOCAL_MEM_STATIC
    LSA_UINT16                      FreeStat;
    #endif
    EDDS_LOCAL_DDB_PTR_TYPE     	pDDB		= pHDB->pDDB;
    LSA_HANDLE_TYPE             	Handle		= pHDB->Handle;

    EDDS_NRT_TRACE_04(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTOpenChannel(pDDB: 0x%X, pHDB: 0x%X, Hdl: %d, pCDB: 0x%X)",
                           pDDB,
                           pHDB,
                           Handle,
                           pCDB);

    pNRTComp    = pDDB->pNRT;
    Status      = EDD_STS_OK;

    /* ----------------------------------------------------------------------*/
    /* If NRT-component not used return with error                           */
    /* ----------------------------------------------------------------------*/

    if (LSA_HOST_PTR_ARE_EQUAL(pNRTComp, LSA_NULL))
    {
        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_NRTOpenChannel: pNRTComp is NULL!");

        return(EDD_STS_ERR_PARAM);
    }


    /* Check if any Filter is already in use by a channel */

    for ( i=0; i<=EDDS_NRT_MAX_FILTER_IDX; i++)
    {
        if  (( pCDB->FrameFilter & pNRTComp->FilterInfo[i].FrameFilter ) &&
             ( pNRTComp->FilterInfo[i].InUse) )
        {

            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                "EDDS_NRTOpenChannel: NRT-Filter still in use!");

            Status = EDD_STS_ERR_CHANNEL_USE;
        }
    }

    if ( Status == EDD_STS_OK)
    {
        /* alloc handle management-memory and put into global handle mgmt.  */

        #ifdef EDDS_CFG_LOCAL_MEM_STATIC
        pNRTHdl = &g_EDDS_HDB_NRT[Handle];

        EDDS_MEMSET_LOCAL(pNRTHdl,0,sizeof(EDDS_HDB_COMP_NRT_TYPE));

        pNRTHdl->CancelRQB.pRQB  = &g_EDDS_HDB_NRT_CANCEL_RQB[Handle];
        g_EDDS_HDB_NRT_CANCEL_RQB[Handle].pParam = &g_EDDS_HDB_NRT_CANCEL_RQB_PARAM[Handle];

        #else

        EDDS_ALLOC_LOCAL_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pNRTHdl, sizeof(EDDS_HDB_COMP_NRT_TYPE));

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pNRTHdl,LSA_NULL) )
        {
            EDDS_MEMSET_LOCAL(pNRTHdl,0,sizeof(EDDS_HDB_COMP_NRT_TYPE));

            pNRTHdl->CancelRQB.pRQB = EDDS_AllocInternalRQB();

            if ( LSA_HOST_PTR_ARE_EQUAL(pNRTHdl->CancelRQB.pRQB,LSA_NULL) )
            {

                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,
                                    "EDDS_NRTOpenChannel: Cannot allocate RQB-memory. No resources.");

                EDDS_FREE_LOCAL_MEM(&FreeStat,pNRTHdl);
                EDDS_FREE_CHECK(FreeStat);
                Status = EDD_STS_ERR_RESOURCE;

            }
        }
        else
        {
            EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,
                                "EDDS_NRTOpenChannel: Cannot allocate handle-memory. No resources.");

            Status = EDD_STS_ERR_RESOURCE;
        }

        if ( Status == EDD_STS_OK)
        #endif
        {

            pHDB->pNRT = pNRTHdl;

            /* Setup internal RQB for EDDS_DO_NRT_CANCEL_PART2-Macro           */

            EDDS_SetupInternalRQBParams(&pNRTHdl->CancelRQB,
                                 EDDS_SRV_DEV_TRIGGER,
                                 Handle,
                                 pDDB,
                                 EDDS_TRIGGER_NRT_CANCEL_PART2,0);

            /* Note:                                                           */
            /* we use only one receive-queue (RdIdx) for all spezified filters.*/
            /* So the RxQueueIdx is the same for all entrys.                   */

            RxIdx = EDDS_NRT_FILTER_NOT_USED;

            for ( i=0; i<=EDDS_NRT_MAX_FILTER_IDX; i++)
            {
                if  ( pCDB->FrameFilter & pNRTComp->FilterInfo[i].FrameFilter )
                {
                    if (RxIdx == EDDS_NRT_FILTER_NOT_USED ) RxIdx = i;
                    pNRTComp->FilterInfo[i].pHDB   = pHDB;
                    pNRTComp->FilterInfo[i].InUse  = LSA_TRUE;
                }
            }

            EDDS_NRT_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_LOW,
                                  "NRTOpenChannel: FrameFilter: 0x%X, RxIDx: 0x%X",
                                  pCDB->FrameFilter,
                                  RxIdx);


            pNRTHdl->RxQueueIdx       = RxIdx;  /* store the Rx-queue for this handle */
            pNRTHdl->RxQueueReqCnt    = 0;      /* no requests pending yet.           */
            pNRTHdl->TxInProgCnt      = 0;
            pNRTHdl->TxQueueReqCnt    = 0;
            pNRTHdl->CancelInProgress = LSA_FALSE;

            pNRTComp->HandleCnt++;        /* cnt of currently open handles      */
        }

    }


    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTOpenChannel(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pCDB,pHDB)
//JB 07/11/2014 checked where called

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTCloseChannel                        +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+                             LSA_HANDLE_TYPE              Handle         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  Handle     : Handle for this Channel to be closed                      +*/
/*+                                                                         +*/
/*+  Results    : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE         Channel has pending requests +*/
/*+               EDD_STS_ERR_RESOURCE         Error on freeing memory      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes the Channel. If there are any pending requests     +*/
/*+               the channel can not be closed. If NRT is not used return  +*/
/*+               with ok.                                                  +*/
/*+                                                                         +*/
/*+               Note:  If EDD_STS_ERR_RESOURCE is returned the channel is +*/
/*+                      closed but we had an error while freeing memory.   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 18/11/2014 checked before function call
//WARNING: be careful when using this function, make sure not to use pHDB as null ptr!
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTCloseChannel(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                      Status;
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    LSA_UINT32                      i;
    #ifndef EDDS_CFG_LOCAL_MEM_STATIC
    LSA_UINT16                      FreeStat;
    #endif
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB	= pHDB->pDDB;
    LSA_HANDLE_TYPE             Handle	= pHDB->Handle;

    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTCloseChannel(pDDB: 0x%X, pHDB: 0x%X, Hdl: %d)",
                           pDDB,
                           pHDB,
                           Handle);

    pNRTComp    = pDDB->pNRT;
    Status      = EDD_STS_OK;

    /* ----------------------------------------------------------------------*/
    /* If NRT-component not used return with OK.                             */
    /* ----------------------------------------------------------------------*/

    if (LSA_HOST_PTR_ARE_EQUAL(pNRTComp, LSA_NULL)) return(EDD_STS_OK);

    /* ----------------------------------------------------------------------*/
    /* If handle dont uses NRT return ok.                                    */
    /* ----------------------------------------------------------------------*/

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB->pNRT, LSA_NULL)) return(EDD_STS_OK);

    /* ----------------------------------------------------------------------*/
    /* Next we check if cancel is in progress or any tx/rx-request present   */
    /* ----------------------------------------------------------------------*/

    if (( pHDB->pNRT->CancelInProgress ) ||
        ( pHDB->pNRT->RxQueueReqCnt    ) ||
        ( pHDB->pNRT->TxQueueReqCnt    ) ||
        ( pHDB->pNRT->TxInProgCnt      ))
    {

        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                            "EDDS_NRTCloseChannel: Tx/RX requests still present. Cannot close channel.");

        Status = EDD_STS_ERR_SEQUENCE;
    }

    /* ----------------------------------------------------------------------*/
    /* If all is ok we free all memory and release the filters               */
    /* ----------------------------------------------------------------------*/

    if ( Status == EDD_STS_OK )
    {

        /* release all filters which are used by the handle. */

        for ( i=0; i<=EDDS_NRT_MAX_FILTER_IDX; i++)
        {
            if (LSA_HOST_PTR_ARE_EQUAL(pHDB, pNRTComp->FilterInfo[i].pHDB) )
            {
                pNRTComp->FilterInfo[i].InUse       = LSA_FALSE;
                pNRTComp->FilterInfo[i].pHDB        = LSA_NULL;
            }
        }

        /* If this channel has set DCP-Filter we deactivate the filter */
        for ( i=0; i< EDDS_NRT_DCP_FILTER_CNT; i++)
        {
            if ((pNRTComp->DCPFilter[i].InUse) &&
                (pNRTComp->DCPFilter[i].Handle == pHDB->Handle))
            {
                pNRTComp->DCPFilter[i].InUse          = LSA_FALSE;
                pNRTComp->DCPFilter[i].Handle         = 0;
                pNRTComp->DCPFilter[i].pDCP_TLVFilter = LSA_NULL; /* no DCP Filter   */
            }
        }

        /* deactivate all DCP-HELLO Filter used by this channel */
        for ( i=0; i< EDD_DCP_MAX_DCP_HELLO_FILTER; i++)
        {
            if ((pNRTComp->pDCPHelloFilter->Filter[i].InUse) &&
                (pNRTComp->pDCPHelloFilter->Filter[i].Handle == Handle))
            {
                pNRTComp->pDCPHelloFilter->Filter[i].InUse = LSA_FALSE;    /* no DCP Filter   */
            }
        }

        /* deactivate ARP Filters if by this channel */
        if ((pNRTComp->ARPFilter.InUse) &&
            (pNRTComp->ARPFilter.Handle == Handle))
        {
            pNRTComp->ARPFilter.InUse = LSA_FALSE;    /* no ARP Filters   */
        }

        /* release all memory used by the handle.          . */

        if ( !LSA_HOST_PTR_ARE_EQUAL(pHDB->pNRT, LSA_NULL))
        {

            #ifndef EDDS_CFG_LOCAL_MEM_STATIC

            EDDS_FreeInternalRQB(pHDB->pNRT->CancelRQB.pRQB);

            EDDS_FREE_LOCAL_MEM(&FreeStat,pHDB->pNRT);
            EDDS_FREE_CHECK(FreeStat);
            #endif

            pHDB->pNRT = LSA_NULL;  /* we always release the handle         */

        }

        pNRTComp->HandleCnt--;      /* cnt of currently open handles        */

    }

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTCloseChannel(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTDCPHelloFilter                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_NRT_SET_DCP_HELLO_FILTER request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB,pHDB)
//JB 18/11/2014 pRQB and pHDB are already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pRQB or pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTDCPHelloFilter(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                      Status;
    EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE     pRqbParam;
    EDDS_NRT_DCP_HELLO_FILTER_MGM_ARRAY_PTR         pHelloFilter;
    LSA_BOOL                                        Found;
    LSA_UINT32                                      i,j;
    EDDS_LOCAL_DDB_PTR_TYPE     					pDDB			= pHDB->pDDB;

    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTDCPHelloFilter(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status        = EDD_STS_OK;
    pRqbParam     = (EDD_UPPER_NRT_SET_DCP_HELLO_FILTER_PTR_TYPE) pRQB->pParam;
    pHelloFilter  = pDDB->pNRT->pDCPHelloFilter;

    if ( LSA_HOST_PTR_ARE_EQUAL( pRqbParam, LSA_NULL))
    {
        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTDCPHelloFilter: Invalid parameter");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        switch (pRqbParam->Mode)
        {
                /*-------------------------------------------------------------------*/
            case EDD_DCP_HELLO_FILTER_OFF_ALL:
                /*-------------------------------------------------------------------*/

                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,
                                    "EDDS_NRTDCPHelloFilter: Turn of all filters.");

                /* Turn off all filters channel independent  */
                for (i=0; i<EDD_DCP_MAX_DCP_HELLO_FILTER; i++)
                {
                    pHelloFilter->Filter[i].InUse = LSA_FALSE;
                }
                break;

                /*-------------------------------------------------------------------*/
            case EDD_DCP_HELLO_FILTER_OFF:
                /*-------------------------------------------------------------------*/

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
                                    "EDDS_NRTDCPHelloFilter: Turn of filter with UserID %d",pRqbParam->UserID);

                /* search for UserID and turn off all filters with UserID */
                Found = LSA_FALSE;
                for (i=0; i<EDD_DCP_MAX_DCP_HELLO_FILTER; i++)
                {
                    if ( ( pHelloFilter->Filter[i].InUse)  &&
                         ( pRqbParam->UserID == pHelloFilter->Filter[i].UserID ) &&
                         ( pHelloFilter->Filter[i].Handle == pHDB->Handle ))
                    {
                        pHelloFilter->Filter[i].InUse = LSA_FALSE;
                        Found = LSA_TRUE;
                    }
                }

                if (!Found)
                {
                    EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_NRTDCPHelloFilter: No filter found with UserID %d",pRqbParam->UserID);

                    Status = EDD_STS_ERR_RESOURCE; /* no Filter found */
                }
                break;

                /*-------------------------------------------------------------------*/
            case EDD_DCP_HELLO_FILTER_ON:
                /*-------------------------------------------------------------------*/

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,
                                    "EDDS_NRTDCPHelloFilter: Turn on filter with UserID %d",pRqbParam->UserID);

                if ( LSA_HOST_PTR_ARE_EQUAL( pRqbParam->pNameOfStation, LSA_NULL) ||
                     (pRqbParam->NameOfStationLen == 0) ||
                     (pRqbParam->NameOfStationLen > EDD_DCP_MAX_DCP_HELLO_NAME_OF_STATION))
                {
                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTDCPHelloFilter: Invalid parameter");
                    Status = EDD_STS_ERR_PARAM;
                }
                else
                {
                    Found = LSA_FALSE;
                    i     = 0;

                    /* search a free entry */
                    while ((!Found) && ( i < EDD_DCP_MAX_DCP_HELLO_FILTER))
                    {
                        if (! pHelloFilter->Filter[i].InUse )
                            Found = LSA_TRUE;
                        else
                            i++;
                    }

                    /* if found fill free entry */
                    if (Found && (i<EDD_DCP_MAX_DCP_HELLO_FILTER))  //JB solution for lint ( i<EDD_DCP_MAX_DCP_HELLO_FILTER should not be needed)
                    {
                        j = pRqbParam->NameOfStationLen;

                        while (j)
                        {
                            j--;
                            pHelloFilter->Filter[i].StationName[j] = pRqbParam->pNameOfStation[j];
                        }

                        pHelloFilter->Filter[i].StationNameLen = pRqbParam->NameOfStationLen;
                        pHelloFilter->Filter[i].UserID         = pRqbParam->UserID;
                        pHelloFilter->Filter[i].Handle         = pHDB->Handle;
                        pHelloFilter->Filter[i].InUse          = LSA_TRUE; /* last action. activate filter */
                    }
                    else
                    {
                        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTDCPHelloFilter: No free filter entry found!");
                        Status = EDD_STS_ERR_RESOURCE;
                    }
                }
                break;
                /*-------------------------------------------------------------------*/
            default:
                /*-------------------------------------------------------------------*/
                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTDCPHelloFilter: Invalid parameter");
                Status = EDD_STS_ERR_PARAM;
                break;
        } /* switch */
    }

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTDCPHelloFilter(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTARPFilter                           +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles EDD_SRV_NRT_SET_ARP_FILTER request                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB, pHDB)
//JB 18/11/2014 pRQB and pHDB are already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pRQB or pHDB as null ptr!
EDDS_STATIC LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTARPFilter(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                                      Status;
    EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE           pRqbParam;
    EDDS_NRT_ARP_FILTER_MGM_PTR                     pARPFilter;
    LSA_UINT32                                      i;
    EDDS_LOCAL_DDB_PTR_TYPE     					pDDB			= pHDB->pDDB;

    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTARPFilter(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Status        = EDD_STS_OK;
    pRqbParam     = (EDD_UPPER_NRT_SET_ARP_FILTER_PTR_TYPE) pRQB->pParam;
    pARPFilter    = &pDDB->pNRT->ARPFilter;

    if ( LSA_HOST_PTR_ARE_EQUAL( pRqbParam, LSA_NULL))
    {
        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTARPFilter: Invalid parameter");
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        switch (pRqbParam->Mode)
        {
                /*-------------------------------------------------------------------*/
            case EDD_ARP_FILTER_OFF:
                /*-------------------------------------------------------------------*/

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"EDDS_NRTARPFilter: Turn OFF ARP filters (Handle: %d)",pHDB->Handle);

                /* traceinfo if multiple channels setting ARP-Filters (doesnt make sense) */
                if (( pARPFilter->InUse ) &&
                    ( pARPFilter->Handle != pHDB->Handle ))
                {
                    EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_HIGH,"EDDS_NRTARPFilter: Turn OFF ARP filters from different user-channel! (Handle1: 0x%X, Handle2: 0x%X)",pARPFilter->Handle,pHDB->Handle);
                }

                /* reset filters */
                for ( i=0; i < EDD_MAX_ARP_FILTER; i++)
                {
                    EDDS_MEMSET_LOCAL(&pARPFilter->ARPFilter.IPAddr[i], 0, EDD_IP_ADDR_SIZE);
                }

                pARPFilter->ARPFilter.FilterCnt = 0;
                pARPFilter->Handle              = pHDB->Handle; /* channel set this filters */
                pARPFilter->InUse               = LSA_FALSE;

                #ifdef LLIF_CFG_USE_LL_ARP_FILTER
                /* reset filter at LL when function pointer is properly set */
                if( EDDS_LL_AVAILABLE(pDDB,setArpFilter) )
                {
                    Status = EDDS_LL_SET_ARP_FILTER(pDDB, &pARPFilter->ARPFilter);
                }
                #endif

                break;

                /*-------------------------------------------------------------------*/
            case EDD_ARP_FILTER_ON:
                /*-------------------------------------------------------------------*/

                EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"EDDS_NRTARPFilter: Turn ON ARP filters (Handle: %d)",pHDB->Handle);

                if (( pRqbParam->FilterCnt > EDD_MAX_ARP_FILTER ) || (pRqbParam->FilterCnt == 0 ))
                {
                    EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTARPFilter: Invalid FilterCnt (%d)",pRqbParam->FilterCnt);
                    Status = EDD_STS_ERR_PARAM;
                }
                else
                {
                    /* traceinfo if multiple channels setting ARP-Filters (doesnt make sense) */
                    if (( pARPFilter->InUse ) &&
                        ( pARPFilter->Handle != pHDB->Handle ))
                    {
                        EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE_HIGH,"EDDS_NRTARPFilter: Replacing ARP filters from different user-channel! (Handle1: 0x%X, Handle2: 0x%X)",pARPFilter->Handle,pHDB->Handle);
                    }

                    pARPFilter->InUse = LSA_FALSE; /* temporary disable while updating. only for sure. */
                    /* filtering is done in same prio but to be sure..  */

                    /* copy filters to management */
                    for ( i=0; i < pRqbParam->FilterCnt; i++)
                    {
                        pARPFilter->ARPFilter.IPAddr[i] = pRqbParam->IPAddr[i];
                    }

                    pARPFilter->ARPFilter.FilterCnt = pRqbParam->FilterCnt;
                    pARPFilter->Handle              = pHDB->Handle; /* channel set this filters */

                    #ifdef LLIF_CFG_USE_LL_ARP_FILTER
                    /* set filter at LL when function pointer is properly set */
                    if( EDDS_LL_AVAILABLE(pDDB,setArpFilter) )
                    {
                        Status = EDDS_LL_SET_ARP_FILTER(pDDB, &pARPFilter->ARPFilter); /* EDDS filter is disabled */
                    }
					else
                    {
                        pARPFilter->InUse  = LSA_TRUE; /* EDDS filter is enabled */
                    }
                    #else
                    pARPFilter->InUse  = LSA_TRUE; /* EDDS filter is enabled */
                    #endif

                }

                break;
                /*-------------------------------------------------------------------*/
            default:
                /*-------------------------------------------------------------------*/
                EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,"EDDS_NRTARPFilter: Invalid parameter");
                Status = EDD_STS_ERR_PARAM;
                break;
        } /* switch */
    }

    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTARPFilter(), Status: 0x%X",
                           Status);

    return(Status);

}
//lint +esym(613,pRQB, pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTCancelPart1                         +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancels NRT-Requests Part1                                +*/
/*+               This part has to be done in high-prio context (in the same+*/
/*+               context send-requests are handled) or higher.             +*/
/*+                                                                         +*/
/*+               We unqueue any send-requests for this channel and queue   +*/
/*+               it for cancel. Then we check if no more send-requests     +*/
/*+               in progress. if so we trigger part2 of cancel. if requests+*/
/*+               in progress triggering will be done when all in progress  +*/
/*+               requests are finished.                                    +*/
/*+                                                                         +*/
/*+               We set the "CancelInProgress" flag to signal that we are  +*/
/*+               in cancel-state.                                          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 18/11/2014 pHDB is already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pHDB as null ptr!
EDDS_STATIC LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTCancelPart1(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQBDummy;
    EDD_UPPER_RQB_PTR_TYPE          pRQBDummy2;
    LSA_UINT32                      i;
    EDDS_LOCAL_DDB_PTR_TYPE     	pDDB		= pHDB->pDDB;

    EDDS_NRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTCancelPart1(pDDB: 0x%X, pHDB: 0x%X)",
                           pDDB,
                           pHDB);

    pNRTComp = pDDB->pNRT;

    /* --------------------------------------------------------------*/
    /* set CancelInProgress, because Cancel is done in two stages    */
    /* CancelInProgress flag is checked within tx-ready-handling     */
    /* --------------------------------------------------------------*/

    pHDB->pNRT->CancelInProgress = LSA_TRUE;

    /* --------------------------------------------------------------*/
    /* Remove all pending but not in progress tx-requests and queue  */
    /* them in cancel-queue. Search within all priorities.           */
    /* --------------------------------------------------------------*/

    for ( i = 0; i < EDDS_NRT_TX_PRIO_CNT; i++)
    {
        pRQBDummy = pNRTComp->Tx[i].Req.pBottom;

        while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
        {
            EDDS_RQB_SEARCH_QUEUE_HANDLE(pRQBDummy,pHDB->Handle);

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
            {

                #if 1
                EDDS_RQB_REM_VALID_BLOCK( pNRTComp->Tx[i].Req.pBottom,
                                          pNRTComp->Tx[i].Req.pTop,
                                          pRQBDummy);

                pRQBDummy2 = pRQBDummy;
                #else
                EDDS_RQB_REM_BLOCK( pNRTComp->Tx[i].Req.pBottom,
                                    pNRTComp->Tx[i].Req.pTop,
                                    pRQBDummy,
                                    pRQBDummy2);
                #endif

                pRQBDummy = EDD_RQB_GET_NEXT_RQB_PTR(pRQBDummy);

                EDDS_RQB_PUT_BLOCK_TOP( pHDB->pNRT->TxCancel.pBottom,
                                        pHDB->pNRT->TxCancel.pTop,
                                        pRQBDummy2);
            }

        }
    } /* for */


    /* --------------------------------------------------------------*/
    /* Now check if we have any pending tx-requests in progress. If  */
    /* not we trigger the low-prio cancel handling.                  */
    /* --------------------------------------------------------------*/

    if (!  pHDB->pNRT->TxInProgCnt )
    {
        /* Note that the RQB is already filled up in startup!     */

        if ( ! pHDB->pNRT->CancelRQB.InUse )
        {
            pHDB->pNRT->CancelRQB.InUse = LSA_TRUE;
            EDDS_DO_NRT_CANCEL_PART2(pHDB->pNRT->CancelRQB.pRQB);
        }
    }

    EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTCancelPart1()");

}
//lint +esym(613,pHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTCancelPart2                         +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE      pDDB           +*/
/*+                        :    EDDS_LOCAL_HDB_PTR_TYPE      pHDB           +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Cancels NRT-Requests Part2                                +*/
/*+               This part has to be done in low -prio context(in the same +*/
/*+               context with read-requests)                               +*/
/*+                                                                         +*/
/*+               We finish all rx and tx- requests with cancel-status.     +*/
/*+                                                                         +*/
/*+               This function has to be called after Part 1 of Cancel     +*/
/*+               handling                                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pHDB)
//JB 18/11/2014 pHDB is already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTCancelPart2(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp;
    EDD_UPPER_RQB_PTR_TYPE          pRQBDummy;
    EDD_UPPER_RQB_PTR_TYPE          pRQBDummy2;
    LSA_UINT32                      RxIdx;
    EDD_UPPER_NRT_RECV_PTR_TYPE     pRQBRcv;
    LSA_UINT8                       Mode;
    LSA_UINT32                      RequestID;
    EDDS_LOCAL_DDB_PTR_TYPE     	pDDB		= pHDB->pDDB;

    EDDS_NRT_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTCancelPart2(pDDB: 0x%X, pHDB: 0x%X)",
                           pDDB,
                           pHDB);


    pNRTComp = pDDB->pNRT;
    pHDB->pNRT->CancelRQB.InUse = LSA_FALSE;

    /* should only be called if CancelRQB present */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDB->pNRT->pUserCancelRQB,LSA_NULL) ) return;

    /* --------------------------------------------------------------*/
    /* First we cancel pending rx-requests                           */
    /* --------------------------------------------------------------*/

    Mode      = ((EDD_UPPER_NRT_CANCEL_PTR_TYPE) pHDB->pNRT->pUserCancelRQB->pParam)->Mode;

    if  (( Mode == EDD_NRT_CANCEL_MODE_ALL      ) ||
         ( Mode == EDD_NRT_CANCEL_MODE_RX_ALL   ) ||
         ( Mode == EDD_NRT_CANCEL_MODE_RX_BY_ID ))
    {

        RxIdx = pHDB->pNRT->RxQueueIdx;  /* Get rx-queue idx from handle */

        /* check if a rx-queue is used */

        if (( RxIdx != EDDS_NRT_FILTER_NOT_USED ) &&
            ( RxIdx <= EDDS_NRT_MAX_FILTER_IDX  ))
        {

            RequestID = ((EDD_UPPER_NRT_CANCEL_PTR_TYPE) pHDB->pNRT->pUserCancelRQB->pParam)->RequestID;

            pRQBDummy  = pNRTComp->RxReq[RxIdx].pBottom;

            while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL) ) /* search all */
            {
                if ( ( Mode != EDD_NRT_CANCEL_MODE_RX_BY_ID ) ||
                     (((EDD_UPPER_NRT_RECV_PTR_TYPE)pRQBDummy->pParam)->RequestID == RequestID ))
                {

                    #if 1
                    EDDS_RQB_REM_VALID_BLOCK( pNRTComp->RxReq[RxIdx].pBottom,
                                              pNRTComp->RxReq[RxIdx].pTop,
                                              pRQBDummy);

                    pRQBDummy2 = pRQBDummy;

                    #else
                    EDDS_RQB_REM_BLOCK(pNRTComp->RxReq[RxIdx].pBottom,
                                       pNRTComp->RxReq[RxIdx].pTop,
                                       pRQBDummy,
                                       pRQBDummy2);

                    /* pRQBDummy2 has to be the same value as pRQBDummy and <> 0 */

                    if ( LSA_HOST_PTR_ARE_EQUAL(pRQBDummy2, LSA_NULL) )
                    {
                        EDDS_FatalError(EDDS_FATAL_ERR_INCONSISTENCE,
                                        EDDS_MODULE_ID,
                                        __LINE__);
                    }
                    #endif

                    pHDB->pNRT->RxQueueReqCnt--;  /*  requests in queue  */
                    /* set number of remaining rx-requests in RQB */

                    pRQBRcv  = (EDD_UPPER_NRT_RECV_PTR_TYPE) pRQBDummy->pParam;
                    pRQBRcv->RequestCnt = pHDB->pNRT->RxQueueReqCnt;


                    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,
                                          "EDDS_NRTCancelPart2: Cancel Rx-RQB: Hdl: 0x%X, pRQB:0x%X, RQBCnt: %d",
                                          pHDB->Handle,
                                          pRQBDummy2,
                                          pRQBRcv->RequestCnt);

                    pRQBDummy = EDD_RQB_GET_NEXT_RQB_PTR(pRQBDummy);

                    EDDS_RequestFinish(pHDB,pRQBDummy2,EDD_STS_OK_CANCEL);

                }
                else
                {
                    pRQBDummy = EDD_RQB_GET_NEXT_RQB_PTR(pRQBDummy);
                }
            }
        }
    }


    /* --------------------------------------------------------------*/
    /* Remove all tx-requests within cancel-queue.                   */
    /* --------------------------------------------------------------*/

    do
    {
        EDDS_RQB_REM_BLOCK_BOTTOM(pHDB->pNRT->TxCancel.pBottom,
                                  pHDB->pNRT->TxCancel.pTop,
                                  pRQBDummy);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL))
        {

            EDDS_NRT_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,
                                  "EDDS_NRTCancelPart2: Cancel Tx-RQB: Hdl: 0x%X, pRQB: 0x%X",
                                  pHDB->Handle,
                                  pRQBDummy);

            /* decrement number of TxRequests queued within EDDS */
            pHDB->pNRT->TxQueueReqCnt--;
            EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK_CANCEL);
        }


    }
    while ( ! LSA_HOST_PTR_ARE_EQUAL(pRQBDummy, LSA_NULL));


    /* --------------------------------------------------------------*/
    /* Finish the CANCEL-Request                                     */
    /* --------------------------------------------------------------*/

    pHDB->pNRT->CancelInProgress = LSA_FALSE;

    // EDD_SRV_NRT_CANCEL is finished with calling callback from HDB.
    // After this callback a possible call to close this NRT channel
    // from another context may reset the reference in pHDB->pNRT,
    // so dont use it after callback.
    pRQBDummy = pHDB->pNRT->pUserCancelRQB;
    pHDB->pNRT->pUserCancelRQB = LSA_NULL;

    EDDS_RequestFinish(pHDB,pRQBDummy,EDD_STS_OK);

    EDDS_NRT_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTCancelPart2()");
}
//lint +esym(613,pHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_NRTRequest                             +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pDDB       : Pointer to DeviceDescriptionBlock                         +*/
/*+  pHDB       : Pointer to HandleDescriptionBlock                         +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST                 +*/
/*+     LSA_HANDLE_TYPE         Handle:                                     +*/
/*+     LSA_USER_ID_TYPE        UserId:     ID of user (not used)           +*/
/*+     LSA_COMP_ID_TYPE        CompId:     Cmponent-ID                     +*/
/*+     EDD_SERVICE             Service:    EDD_SRV_NRT_RECV                +*/
/*+                                         EDD_SRV_NRT_SEND                +*/
/*+                                         EDD_SRV_NRT_CANCEL              +*/
/*+     LSA_RESULT              Status:     Return status                   +*/
/*+     EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on service               +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function depends on service             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function handles all NRT-Requests with OPCODE-Type   +*/
/*+               EDD_OPC_REQUEST and service EDDS_SRV_NRT_xxxx.            +*/
/*+                                                                         +*/
/*+               If we have a parameter error we will set EDD_STS_ERR_PARAM+*/
/*+               and call the handle-callbackfunction and abort the request+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SERVICE is set if we get an invalid Service.  +*/
/*+               EDD_STS_ERR_CHANNEL_USE is set if the service is invalid  +*/
/*+               for this channel (i.e. not used by this channel)          +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_ALIGN is set if the buffer dont matches the   +*/
/*+               required alignmentrequirements.                           +*/
/*+                                                                         +*/
/*+               If the request is valid, it is queued or executed.        +*/
/*+                                                                         +*/
/*+               Note: Opcode not checked!                                 +*/
/*+                     NRT-Component already initialized (not checked)     +*/
/*+                     Channel uses this component (not checked)           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pRQB, pHDB)
//JB 18/11/2014 pRQB and pHDB are already checked when this function is called
//WARNING: be careful when using this function, make sure not to call it with pRQB or pHDB as null ptr!
LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB)
{

    LSA_RESULT                              Status;
    LSA_BOOL                                Indicate;
    EDD_UPPER_NRT_CANCEL_PTR_TYPE           pRQBCancel;
    EDD_UPPER_NRT_SET_DCP_FILTER_PTR_TYPE   pRQBFilter;
    LSA_UINT32                              i;
    EDDS_LOCAL_DDB_PTR_TYPE     			pDDB		= pHDB->pDDB;


    EDDS_NRT_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_NRTRequest(pRQB: 0x%X, pDDB: 0x%X, pHDB: 0x%X)",
                           pRQB,
                           pDDB,
                           pHDB);

    Indicate = LSA_TRUE;
    Status   = EDD_STS_OK;

    /*---------------------------------------------------------------------------*/
    /* Check if management initialized for handle. Should be because we are only */
    /* called if request is valid for this handle...but for sure..               */
    /*---------------------------------------------------------------------------*/

    if  ( LSA_HOST_PTR_ARE_EQUAL(pHDB->pNRT, LSA_NULL) )
    {
        EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_UNEXP,"EDDS_NRTRequest: NRT not initialized yet");
        Status = EDD_STS_ERR_CHANNEL_USE;
    }

    /*---------------------------------------------------------------------------*/
    /* Alle requests need an param-ptr, so check it.                             */
    /*---------------------------------------------------------------------------*/

    if (Status == EDD_STS_OK)
    {
        if  ( LSA_HOST_PTR_ARE_EQUAL(pRQB->pParam, LSA_NULL) )
        {
            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,">>> Request: Service: 0x%X, RQB->pParam is NULL",EDD_RQB_GET_SERVICE(pRQB));
            Status = EDD_STS_ERR_PARAM;
        }
    }


    if (Status == EDD_STS_OK)
    {

        switch (EDD_RQB_GET_SERVICE(pRQB))
        {
                /*-----------------------------------------------------------------------*/
                /* Receive-Request. Queue the Request within Rx-Queue used by handle.    */
                /* Service : EDD_SRV_NRT_RECV                                            */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_RECV:

                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_RECV (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));

                Status = EDDS_NRTRecv(pRQB,pHDB);
                if ( Status == EDD_STS_OK)  Indicate = LSA_FALSE; /* Request is queued */
                break;

                /*-----------------------------------------------------------------------*/
                /* Send   -Request. Queue the Request within Tx-Queue and trigger send.  */
                /* Service : EDD_SRV_NRT_SEND                                            */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_SEND:

                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_SEND (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));

                /* tx-requests only allowed if no tx-cancel currently running */

                if ( ! pHDB->pNRT->CancelInProgress )  /* a tx-cancel running */
                {
                    Status = EDDS_NRTSend(pRQB,pHDB);
                }
                else
                {

                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_NRTRequest: EDD_SRV_NRT_SEND: Cancel in progress");

                    Status = EDD_STS_ERR_SEQUENCE;
                }

                if ( Status == EDD_STS_OK)  Indicate = LSA_FALSE; /* Request is queued */

                break;

                /*-----------------------------------------------------------------------*/
                /* Cancel -Request. Cancels requests depending on mode.                  */
                /* Service : EDD_SRV_NRT_CANCEL                                          */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_CANCEL:

                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_CANCEL (pRQB: 0x%X, Handle: 0x%X)",pRQB, EDD_RQB_GET_HANDLE(pRQB));

                if ( ! pHDB->pNRT->CancelInProgress ) /* no cancel already running   */
                {
                    pRQBCancel = (EDD_UPPER_NRT_CANCEL_PTR_TYPE) pRQB->pParam;

                    switch (pRQBCancel->Mode)
                    {
                        case EDD_NRT_CANCEL_MODE_ALL:
                        case EDD_NRT_CANCEL_MODE_TX_ALL:

                            pHDB->pNRT->pUserCancelRQB   = pRQB;  /* save RQB-Ptr for later use */

                            /* Because this sequence is done in low-prio context an must not be */
                            /* interrupted by high-prio context, we have to use EDDS_ENTER_HIGH()*/

                            EDDS_ENTER_HIGH();

                            EDDS_NRTCancelPart1(pHDB);

                            EDDS_EXIT_HIGH();

                            Indicate = LSA_FALSE;
                            break;

                        case EDD_NRT_CANCEL_MODE_RX_ALL:
                        case EDD_NRT_CANCEL_MODE_RX_BY_ID:

                            /* if we only have to cancel Rx-Requests, we need only to do        */
                            /* part2 of cancel. We dont set CancelInProgress flag, because we   */
                            /* do all parts of cancel synchron and tx must not be canceled.     */

                            pHDB->pNRT->pUserCancelRQB   = pRQB;  /* save RQB-Ptr for later use */

                            EDDS_NRTCancelPart2(pHDB);

                            Indicate = LSA_FALSE;

                            break;
                        default:

                            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_NRTRequest: EDD_SRV_NRT_CANCEL: Invalid Mode (0x%X)",
                                                pRQBCancel->Mode);

                            Status = EDD_STS_ERR_PARAM;
                            break;
                    }
                }
                else
                {

                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                        "EDDS_NRTRequest: EDD_SRV_NRT_CANCEL: Cancel already running");

                    Status = EDD_STS_ERR_SEQUENCE;
                }

                break;
                /*-----------------------------------------------------------------------*/
                /* DCP-Filter-Request.                                                   */
                /* Service : EDD_SRV_NRT_SET_DCP_FILTER                                  */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_SET_DCP_FILTER:

                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_SET_DCP_FILTER (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));

                pRQBFilter = (EDD_UPPER_NRT_SET_DCP_FILTER_PTR_TYPE) pRQB->pParam;

                switch ( pRQBFilter->Mode )
                {
                    case EDD_DCP_FILTER_ON:
                        /* check parameters */
                        if (( pRQBFilter->FilterTLVCnt <= EDDS_NRT_DCP_FILTER_CNT ) && ( pRQBFilter->FilterTLVCnt ))
                        {
                            /* check parameters */
							//lint -esym(613,pTLV)
							//JB 19/11/2014 Status = EDD_STS_ERR_PARAM, no null ptr access possible
                            for ( i=0; i< pRQBFilter->FilterTLVCnt; i++)
                            {
                                LSA_UINT8* pTLV = (LSA_UINT8*) pRQBFilter->FilterTLV[i].pTLV;

                                if ( LSA_HOST_PTR_ARE_EQUAL(pTLV, LSA_NULL))
                                {
                                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_NRTRequest: EDD_SRV_NRT_SET_DCP_FILTER: pTLV is NULL");

                                    Status = EDD_STS_ERR_PARAM;
                                }

                                if( Status == EDD_STS_OK )
                                {
                                  LSA_UINT16 ValueLength = (pTLV[EDDS_DCP_TLV_LENGTH_OFFSET] << 8) +
                                                            pTLV[EDDS_DCP_TLV_LENGTH_OFFSET+1];

								                  /* length-field within the TLV shall be equal to (TLVLen-4) */
                                  if( ValueLength != pRQBFilter->FilterTLV[i].TLVLen - EDDS_DCP_TLV_TLSIZE  ||
                                      ValueLength > (EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN - EDDS_DCP_TLV_TLSIZE) )
                                  {
                                    EDDS_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_ERROR,
                                                        "EDDS_NRTRequest: EDD_SRV_NRT_SET_DCP_FILTER: wrong TLV length");

                                    Status = EDD_STS_ERR_PARAM;
                                  }
                                }
                            }

                            if ( Status == EDD_STS_OK )
                            {
                                for ( i=0; i< EDDS_NRT_DCP_FILTER_CNT; i++)
                                {
                                	if ( i < pRQBFilter->FilterTLVCnt )
                                    {
                                        pDDB->pNRT->DCPFilter[i].InUse  = LSA_TRUE;
                                        pDDB->pNRT->DCPFilter[i].Handle = pHDB->Handle;

                                        /* clear old and insert new TLV */
                                        EDDS_MEMSET_UPPER(pDDB->pNRT->DCPFilter[i].DCP_TLVFilter,
                                                         0,
                                                         EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN);

                                        EDDS_MEMCOPY_UPPER(pDDB->pNRT->DCPFilter[i].DCP_TLVFilter,
                                                          pRQBFilter->FilterTLV[i].pTLV,
                                                          pRQBFilter->FilterTLV[i].TLVLen);

                                        pDDB->pNRT->DCPFilter[i].pDCP_TLVFilter      = &pDDB->pNRT->DCPFilter[i].DCP_TLVFilter[0];

                                        pDDB->pNRT->DCPFilter[i].Type                = (pDDB->pNRT->DCPFilter[i].DCP_TLVFilter[EDDS_DCP_TLV_TYPE_OFFSET] << 8) +
                                                                                       pDDB->pNRT->DCPFilter[i].DCP_TLVFilter[EDDS_DCP_TLV_TYPE_OFFSET+1];  //lint !e835 JB 18/11/2014 define-based behaviour
                                        pDDB->pNRT->DCPFilter[i].ValueLength         = (LSA_UINT16) (pRQBFilter->FilterTLV[i].TLVLen - EDDS_DCP_TLV_TLSIZE);
                                    }
                                    else
                                    {
                                        /* clear unused TLV */
                                        pDDB->pNRT->DCPFilter[i].InUse  = LSA_FALSE;
                                        pDDB->pNRT->DCPFilter[i].Handle = 0;

                                        EDDS_MEMSET_UPPER(pDDB->pNRT->DCPFilter[i].DCP_TLVFilter, 0, EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN);
										                    pDDB->pNRT->DCPFilter[i].pDCP_TLVFilter = LSA_NULL;
                                        pDDB->pNRT->DCPFilter[i].Type           = 0;
                                        pDDB->pNRT->DCPFilter[i].ValueLength    = 0;
                                    }
                                } /* for */
                            }
                        }
                        else
                        {

                            EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                                "EDDS_NRTRequest: EDD_SRV_NRT_SET_DCP_FILTER: Invalid FilterTLVCnt (0x%X)",
                                                pRQBFilter->FilterTLVCnt);

                            Status = EDD_STS_ERR_PARAM;
                        }
                        break;
                    case EDD_DCP_FILTER_OFF:
						{
							for ( i=0; i< EDDS_NRT_DCP_FILTER_CNT; i++)
							{
								/* clear unused TLV */
								pDDB->pNRT->DCPFilter[i].InUse  = LSA_FALSE;
								pDDB->pNRT->DCPFilter[i].Handle = 0;

								EDDS_MEMSET_UPPER(pDDB->pNRT->DCPFilter[i].DCP_TLVFilter, 0, EDD_DCP_MAX_SET_DCP_FILTER_TLV_LEN);
								pDDB->pNRT->DCPFilter[i].pDCP_TLVFilter = LSA_NULL;
								pDDB->pNRT->DCPFilter[i].Type           = 0;
								pDDB->pNRT->DCPFilter[i].ValueLength    = 0;
							} /* for */
						}
                        break;
                    default:
                        EDDS_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,
                                            "EDDS_NRTRequest: EDD_SRV_NRT_SET_DCP_FILTER: Invalid Mode (0x%X)",
                                            pRQBFilter->Mode);

                        Status = EDD_STS_ERR_PARAM;
                        break;
                }
                break;
                /*-----------------------------------------------------------------------*/
                /* DCP-Filter-Request.                                                   */
                /* Service : EDD_SRV_NRT_SET_DCP_HELLO_FILTER                            */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_SET_DCP_HELLO_FILTER:
                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_SET_DCP_HELLO_FILTER (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
                Status = EDDS_NRTDCPHelloFilter(pRQB,pHDB);
                break;
                /*-----------------------------------------------------------------------*/
                /* APR-Filter-Request.                                                   */
                /* Service : EDD_SRV_NRT_SET_ARP_FILTER                                  */
                /*-----------------------------------------------------------------------*/
            case EDD_SRV_NRT_SET_ARP_FILTER:
                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_NOTE,">>> Request: EDD_SRV_NRT_SET_ARP_FILTER (pRQB: 0x%X, Handle: 0x%X)",pRQB,EDD_RQB_GET_HANDLE(pRQB));
                Status = EDDS_NRTARPFilter(pRQB,pHDB);
                break;
                /*-----------------------------------------------------------------------*/
                /* Unknown-Request.                                                      */
                /*-----------------------------------------------------------------------*/
            default:

                EDDS_UPPER_TRACE_02(0,LSA_TRACE_LEVEL_ERROR,">>> EDDS_NRTRequest: Invalid Service (pRQB: 0x%X, Service: 0x%X)",pRQB,EDD_RQB_GET_SERVICE(pRQB));

                Status = EDD_STS_ERR_SERVICE;
                break;

        } /* switch */
    } /* if */


    if ( Indicate )
    {
        EDDS_RequestFinish(pHDB,pRQB,Status);
    }


    EDDS_NRT_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_NRTRequest(), Status: 0x%X",
                           EDD_STS_OK);
}
//lint +esym(613,pRQB,pHDB,pTLV)



#endif

/*****************************************************************************/
/*  end of file NRT_USR.C                                                    */
/*****************************************************************************/



