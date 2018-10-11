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
/*  F i l e               &F: edds_dev.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Device and handle-management functions      */
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
/*  15.06.04    JS    removed ASRT functions                                 */
/*  10.08.04    JS    use of EDDS_FREE_CHECK macro                           */
/*  19.01.05    JS    cast to prevent compiler warning                       */
/*  28.01.05    JS    LTRC support                                           */
/*  10.03.06    JS    added diag timer rqb                                   */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  2
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_DEV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

#ifdef EDDS_CFG_LOCAL_MEM_STATIC
EDDS_STATIC EDDS_LOCAL_MEM_ATTR EDDS_HDB_TYPE g_EDDS_HDB[EDDS_CFG_MAX_CHANNELS];
#endif

/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*=====  handle-functions                                                ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleIni                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDS channel handle management.           +*/
/*+               The number of handles the edds can manage is defined by   +*/
/*+               EDDS_CFG_HANDLE_CNT. The memory for handle-management is  +*/
/*+               static or will be allocated depending on                  +*/
/*+               EDDS_CFG_LOCAL_MEM_STATIC                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleIni(LSA_VOID)
{
    LSA_UINT32              Index;
    EDDS_HDB_INFO_TYPE *    pHDBInfo;
    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
    LSA_UINT32              CntHandle;
    LSA_RESULT              Status;

    Status = EDD_STS_OK;

    pHDBInfo = &g_EDDSInfo.HDBInfo;

    CntHandle = EDDS_CFG_MAX_CHANNELS;

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleIni()");

    #ifdef EDDS_CFG_LOCAL_MEM_STATIC
    pHDB = g_EDDS_HDB;
    #else
    pHDB     = pHDBInfo->pTabHDB;
    EDDS_ALLOC_LOCAL_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pHDB, (CntHandle * sizeof(EDDS_HDB_TYPE)));
    #endif

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )  //lint !e774 JB 10/11/2014 only if EDDS_CFG_LOCAL_MEM_STATIC is defined
    {
        Status = EDD_STS_ERR_RESOURCE;
    }
    else
    {
        EDDS_MEMSET_LOCAL(pHDB,0,CntHandle * sizeof(EDDS_HDB_TYPE));

        /* initialize HDBInfo */
        pHDBInfo->pTabHDB    = pHDB;
        pHDBInfo->MaxHandle  = CntHandle;
        pHDBInfo->UsedHandle = 0;

        for (Index=0; Index < CntHandle; Index++)
        {
            pHDB[Index].InUse           = LSA_FALSE;
        }
    }

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandleIni(), Status: 0x%X",
                           Status);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleClose                            +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes EDDS-channel Handle management                     +*/
/*+               This is only possible if no open channels are present.    +*/
/*+               If there are open channel (handles are in use) we return  +*/
/*+               EDD_STS_ERR_SEQUENCE and abort.                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleClose(LSA_VOID)
{
    LSA_UINT32              Index;
    LSA_RESULT              Status;
    EDDS_HDB_INFO_TYPE *    pHDBInfo;
    EDDS_LOCAL_HDB_PTR_TYPE pHDB;

    #ifndef EDDS_CFG_LOCAL_MEM_STATIC
    LSA_UINT16              FreeStat;
    #endif


    Status = EDD_STS_OK;

    pHDBInfo = &g_EDDSInfo.HDBInfo;
    pHDB     = pHDBInfo->pTabHDB;

    EDDS_CORE_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleClose()");

    /* check if already free. if so we have nothing todo */

    if (! LSA_HOST_PTR_ARE_EQUAL(pHDB,LSA_NULL))
    {
        if (pHDBInfo->UsedHandle != 0)
        {
            Status = EDD_STS_ERR_SEQUENCE;
        }
        else
        {

            Index = 0;
            while (Index < g_EDDSInfo.HDBInfo.MaxHandle)
            {
                if (pHDB[Index].InUse)
                    EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_CLOSE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
                Index++;
            }

            #ifndef EDDS_CFG_LOCAL_MEM_STATIC

            EDDS_FREE_LOCAL_MEM(&FreeStat, pHDB);
            EDDS_FREE_CHECK(FreeStat);
            #endif

            pHDBInfo->pTabHDB = LSA_NULL;
        } /* else */
    } /* else */


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandleClose(), Status: 0x%X",
                           Status);

    return Status;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDSHandleUsesDDB                           +*/
/*+  Input/Output               EDDS_LOCAL_DDB_PTR_TYPE    pDDB             +*/
/*+  Result                :    LSA_BOOL T                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         Pointer to DDB                                            +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Searches in Handle-management for handle which uses the   +*/
/*+               spezified DDB.                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_HandleSearchDDB(EDDS_LOCAL_DDB_PTR_TYPE pDDB)
{

    LSA_UINT32              Index;
    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
    EDDS_HDB_INFO_TYPE *    pHDBInfo;
    LSA_BOOL                Found;


    pHDBInfo = &g_EDDSInfo.HDBInfo;
    pHDB     = pHDBInfo->pTabHDB;

    Found = LSA_FALSE;

    Index = 0;
    while ( (Index < g_EDDSInfo.HDBInfo.MaxHandle) && ( ! Found ))
    {
        if  ((pHDB[Index].InUse) &&
             (LSA_HOST_PTR_ARE_EQUAL(pHDB[Index].pDDB,pDDB)))
        {
            Found = LSA_TRUE;
        }

        Index++;
    }

    return(Found);


}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleNew                              +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE EDDS_LOCAL_MEM_ATTR * pHandle+*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE            * ppHDB  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHandle      Pointer to address for LSA_HANDLE which will be returned. +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Creates new EDDS channel Handle                           +*/
/*+               On success an LSA_HANDLE and a pointer to the HDB will be +*/
/*+               returned.                                                 +*/
/*+               InUse and Handle is setup, the rest is initialized with 0 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,ppHDB)
//JB 10/11/2014 ppHDB will only be set
//AD_DISCUSS 20/11/2014 ppHDB could be pointing to null
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleNew(EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR *ppHDB)
{

    LSA_UINT32              Index;
    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
    LSA_RESULT              Status;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleNew(ppHDB: 0x%X)",
                           ppHDB);

    Status = EDD_STS_OK;

    if (g_EDDSInfo.HDBInfo.UsedHandle >= g_EDDSInfo.HDBInfo.MaxHandle)
    {
        Status = EDD_STS_ERR_RESOURCE;
    }
    else
    {
        Index = 0;
        pHDB = g_EDDSInfo.HDBInfo.pTabHDB;

        while ( (Index < g_EDDSInfo.HDBInfo.MaxHandle)   &&
                (pHDB[Index].InUse) )
            Index++;

        if (pHDB[Index].InUse)
        {
            Status = EDD_STS_ERR_RESOURCE;
        }
        else
        {

            g_EDDSInfo.HDBInfo.UsedHandle++;

            EDDS_MEMSET_LOCAL(&pHDB[Index],0,sizeof(EDDS_HDB_TYPE));

            pHDB[Index].InUse   = LSA_TRUE;
            pHDB[Index].Handle  = (LSA_HANDLE_TYPE) Index;

            *ppHDB   = &pHDB[Index];
        }

    }

    EDDS_CORE_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandleNew(Handle: 0x%X, *ppHDB: 0x%X), Status: 0x%X",
                           (*ppHDB)->Handle,
                           *ppHDB,
                           Status);

    return Status;
}
//lint +esym(613,ppHDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleGetHDB                           +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE                    Handle   +*/
/*+                             EDDS_LOCAL_HDB_PTR_TYPE            * ppHDB  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets pointer to handle-managment entry (HDB) for handle.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,ppHDB)
//JB 10/11/2014 ppHDB will only be set
//AD_DISCUSS 20/11/2014 ppHDB could be pointing to null
LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleGetHDB( LSA_HANDLE_TYPE const Handle,
                                                  EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppHDB)
{

    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
    LSA_RESULT              Status;


    Status = EDD_STS_OK;

    /* Handle is index in table */

    if (Handle >= g_EDDSInfo.HDBInfo.MaxHandle )
    {
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {
        pHDB    = g_EDDSInfo.HDBInfo.pTabHDB;
        *ppHDB  = &pHDB[Handle];
    }


    return Status;
}
//lint +esym(613,ppHDB)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_HandleRel                              +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE                    Handle   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees channel handle.                                     +*/
/*+                                                                         +*/
/*+               If the handle is not in use, EDD_STS_ERR_SEQUENCE is      +*/
/*+               returned.                                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_HandleRel(LSA_HANDLE_TYPE Handle)
{

    EDDS_LOCAL_HDB_PTR_TYPE pHDB;
    LSA_RESULT              Status;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_HandleRel(Handle: 0x%X)",
                           Handle);

    Status = EDD_STS_OK;

    if (Handle >= g_EDDSInfo.HDBInfo.MaxHandle)
    {
        Status = EDD_STS_ERR_PARAM;
    }
    else
    {

        pHDB = g_EDDSInfo.HDBInfo.pTabHDB;

        if (Handle != pHDB[Handle].Handle)
            EDDS_FatalError(EDDS_FATAL_ERR_HANDLE_REL,
                            EDDS_MODULE_ID,
                            __LINE__);

        if (!pHDB[Handle].InUse)
        {
            Status = EDD_STS_ERR_SEQUENCE;
        }
        else
        {
            g_EDDSInfo.HDBInfo.UsedHandle--;

            pHDB[Handle].InUse          = LSA_FALSE;
        }
    }

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_HandleRel(), Status: 0x%X",
                           Status);

    return Status;
}

/*===========================================================================*/


/*===========================================================================*/
/*=====  DDB & DDBInfo functions                                         ====*/
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DevInfoIni                             +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    ---                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDS Device Description Block (DDB)       +*/
/*+               Info structure. This structure is the anchor of           +*/
/*+               all DDB initialized later on.                             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DevInfoIni(LSA_VOID)
{
    EDDS_DDB_INFO_TYPE *    pDDBInfo;

    pDDBInfo = &g_EDDSInfo.DDBInfo;
    pDDBInfo->pBotton   = LSA_NULL;
    pDDBInfo->pTop      = LSA_NULL;
    pDDBInfo->CntDDB    = 0;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DevInfoClose                           +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes DDB-info. It can only be closed if no DDB is open. +*/
/*+               If a DDB is open LSA_RET_ERR_SEQUENCE is returned.        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_DevInfoClose(LSA_VOID)
{
    EDDS_DDB_INFO_TYPE *    pDDBInfo;

    pDDBInfo = &g_EDDSInfo.DDBInfo;

    if (pDDBInfo->CntDDB != 0)
        return EDD_STS_ERR_SEQUENCE;

    if ( (!LSA_HOST_PTR_ARE_EQUAL(pDDBInfo->pBotton, LSA_NULL))  ||
         (!LSA_HOST_PTR_ARE_EQUAL(pDDBInfo->pTop, LSA_NULL)) )
    {
        EDDS_FatalError(EDDS_FATAL_ERR_DEV_INFO_CLOSE,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    return EDD_STS_OK;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CreateDDB                              +*/
/*+  Input/Output          :    EDDS_LOCAL_DDB_PTR_TYPE * ppDDB             +*/
/*+                             LSA_UINT32                TxBufferCnt (>0)  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+  TxBufferCnt  : Number of Tx-Buffers to manage                          +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an DDB structure and initializes it. The new DDB+*/
/*+               is chained into the global DDB-info structure and a       +*/
/*+               pointer to the new DDB is returned.                       +*/
/*+                                                                         +*/
/*+               EDDS_DevInfoIni() has to be called before using this funct.+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_RESOURCE is returned, if alloc fails or no    +*/
/*+               more devices supported.                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


//lint -esym(613,pDDB)
//ppDDB will be set
//AD_DISCUSS 20/11/2014 ppDDB could be pointing to null
LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CreateDDB(
    EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB,
    EDDS_UPPER_DPB_PTR_TYPE const                 pDPB)
{

    EDDS_LOCAL_DDB_PTR_TYPE         pDDB;
    EDDS_LOCAL_DEVICE_GLOB_PTR_TYPE pGlob;
    LSA_UINT32                      Size;
    LSA_UINT16                      FreeStat;
    LSA_RESULT                      Result;


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CreateDDB(ppDDB: 0x%X)",
                           ppDDB);


    Result = EDD_STS_OK;
    pDDB   = LSA_NULL;
    pGlob  = LSA_NULL;

    /*-----------------------------------------------------------------------*/
    /* Check for Max-Devices supported                                       */
    /*-----------------------------------------------------------------------*/

    if ( g_EDDSInfo.DDBInfo.CntDDB >= EDDS_CFG_MAX_DEVICES )
    {
        Result = EDD_STS_ERR_RESOURCE;
    }

    /*-----------------------------------------------------------------------*/
    /* alloc for  DDB-type                                                   */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {

        EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pDDB), sizeof(EDDS_DDB_TYPE));

        if ( LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pDDB,0,sizeof(EDDS_DDB_TYPE));
    }

    /*-----------------------------------------------------------------------*/
    /* alloc for global DDB-type parts                                       */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {

        Size =  sizeof(EDDS_DEVICE_GLOB_TYPE);

        #ifdef EDDS_CFG_GLOB_FAST_MEM
        EDDS_ALLOC_LOCAL_FAST_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob), Size);
        #else
        EDDS_ALLOC_LOCAL_MEM((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob, Size);
        #endif

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pGlob,0,Size);
    }

    /*-----------------------------------------------------------------------*/
    /* alloc for  pTxInfo                                                    */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Size =  sizeof(EDDS_TX_INFO_TYPE)* (pDPB->TxBufferCnt);

        EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob->pTxInfo), Size);

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->pTxInfo, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else
        {
            EDDS_MEMSET_LOCAL(pGlob->pTxInfo,0,Size);
            pGlob->TxBuffCnt           = pDPB->TxBufferCnt;
        }
    }

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    /*-----------------------------------------------------------------------*/
    /* alloc ressources for CopyInterface                                    */
    /*-----------------------------------------------------------------------*/

    if ( (EDD_STS_OK == Result) && (! pDPB->ZeroCopyInterface) )
    {
        Size =  EDD_FRAME_BUFFER_LENGTH * (pDPB->TxBufferCnt);
        // we must use hSysDev from pDPB here (pDDB->hSysDev is not initialized yet)!
        EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(pDPB->hSysDev,((EDD_UPPER_MEM_PTR_TYPE *)&pGlob->pTxTransferMem), Size);
    }
    pGlob->ZeroCopyInterface   = pDPB->ZeroCopyInterface;
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    /*-----------------------------------------------------------------------*/
    /* alloc for  MC-MAC Management                                          */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Size =  sizeof(EDDS_MC_MAC_INFO_TYPE);

        EDDS_ALLOC_LOCAL_MEM(((EDDS_LOCAL_MEM_PTR_TYPE *)&pGlob->pMCInfo),Size);

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->pMCInfo, LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
        else EDDS_MEMSET_LOCAL(pGlob->pMCInfo,0,Size);
    }


    /*-----------------------------------------------------------------------*/
    /* Allocate upper RQBs for internal use                                  */
    /*-----------------------------------------------------------------------*/
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    if ( Result == EDD_STS_OK )
    {
        pGlob->RQBInt_Tx.pRQB       = EDDS_AllocInternalRQB();

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Tx.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }

    if ( Result == EDD_STS_OK )
    {
        pGlob->RQBInt_Tx_Tout.pRQB  = EDDS_AllocInternalRQB();

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Tx_Tout.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    if ( Result == EDD_STS_OK )
    {
        pGlob->RQBInt_LL_Tout.pRQB  = EDDS_AllocInternalRQB();

        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_LL_Tout.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_Rx_NRT.pRQB   = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Rx_NRT.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }

    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_Rx_cSRT.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Rx_cSRT.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }

    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_PHY.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_PHY.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

    #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_Rx_OverTimer.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Rx_OverTimer.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }

    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_Rx_OverEvent.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_Rx_OverEvent.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }
    #endif

    #ifdef EDDS_CFG_LED_BLINK_SUPPORT
    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_LED_BlinkTimer.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_LED_BlinkTimer.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }
    #endif

    #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
    if ( Result == EDD_STS_OK )
    {

        pGlob->RQBInt_DiagTimer.pRQB  = EDDS_AllocInternalRQB();
        if ( LSA_HOST_PTR_ARE_EQUAL(pGlob->RQBInt_DiagTimer.pRQB,LSA_NULL) )
        {
            Result = EDD_STS_ERR_RESOURCE;
        }
    }
    #endif

    /*-----------------------------------------------------------------------*/
    /* Allocate PRM Structures                                               */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {
        Result = EDDS_PrmAlloc(&pGlob->Prm);
    }

    /*-----------------------------------------------------------------------*/
    /* on scuccess we initialize the internal RQBs with fixed values.        */
    /*-----------------------------------------------------------------------*/

    if ( Result == EDD_STS_OK )
    {

        /*-------------------------------------------------------------------*/
        /* Init internal RQBs used by EDDS in Interrupt/Timeout-handler      */
        /*-------------------------------------------------------------------*/
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Tx,EDDS_SRV_DEV_INTERRUPT,0,pDDB,EDDS_ISR_TX,0);
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Tx_Tout,EDDS_SRV_DEV_TIMEOUT,0,pDDB,EDDS_TIMEOUT_TX,0);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
    	EDDS_SetupInternalRQBParams(&pGlob->RQBInt_LL_Tout,EDDS_SRV_DEV_TIMEOUT,0,pDDB,EDDS_TIMEOUT_LL,0);
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Rx_NRT,EDDS_SRV_DEV_INTERRUPT,0,pDDB,EDDS_ISR_RX_NRT,0);
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Rx_cSRT,EDDS_SRV_DEV_INTERRUPT,0,pDDB,EDDS_ISR_RX_cSRT,0);
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_PHY,EDDS_SRV_DEV_INTERRUPT,0,pDDB,EDDS_ISR_PHY,0);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
        #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
        pGlob->RxOverload.Status   = EDD_NO_RX_OVERLOAD;
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Rx_OverTimer,EDDS_SRV_DEV_TRIGGER,0,pDDB,EDDS_TRIGGER_RX_OVER_CYCLE,0);
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_Rx_OverEvent,EDDS_SRV_DEV_TRIGGER,0,pDDB,EDDS_TRIGGER_RX_OVER_EVENT,0);
        #endif
        #ifdef EDDS_CFG_LED_BLINK_SUPPORT
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_LED_BlinkTimer,EDDS_SRV_DEV_TRIGGER,0,pDDB,EDDS_TRIGGER_LED_BLINK_CYCLE,0);
        #endif
        #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
        EDDS_SetupInternalRQBParams(&pGlob->RQBInt_DiagTimer,EDDS_SRV_DEV_TRIGGER,0,pDDB,EDDS_TRIGGER_DIAG_CYCLE,0);
        #endif
        /*-------------------------------------------------------------------*/
        /* Init structure (all parts are already memset to 0, so no init to 0*/
        /* is required!)                                                     */
        /*-------------------------------------------------------------------*/

        pDDB->pNext         = LSA_NULL;
        pDDB->pPrev         = LSA_NULL;
        pDDB->hDDB          = (EDDS_HANDLE)pDDB;
        pDDB->pGlob         = pGlob;
        pDDB->pNRT          = LSA_NULL;  /* so NRT-Component present yet */
        pDDB->pSRT          = LSA_NULL;  /* so SRT-Component present yet */

        g_EDDSInfo.DDBInfo.CntDDB++;

        EDDS_PUT_BLOCK_TOP(g_EDDSInfo.DDBInfo.pBotton, g_EDDSInfo.DDBInfo.pTop, pDDB);

        *ppDDB = pDDB;
    }



    /*-----------------------------------------------------------------------*/
    /* on error we free all allocs                                           */
    /*-----------------------------------------------------------------------*/

    if (Result != EDD_STS_OK )
    {
        if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB, LSA_NULL) )
        {
            if ( ! LSA_HOST_PTR_ARE_EQUAL(pGlob, LSA_NULL) )
            {
                if ( ! LSA_HOST_PTR_ARE_EQUAL(pGlob->pTxInfo, LSA_NULL) )
                {
                    EDDS_FREE_LOCAL_MEM(&FreeStat,pGlob->pTxInfo);
                    EDDS_FREE_CHECK(FreeStat);
                }

                if ( ! LSA_HOST_PTR_ARE_EQUAL(pGlob->pMCInfo, LSA_NULL) )
                {
                    EDDS_FREE_LOCAL_MEM(&FreeStat,pGlob->pMCInfo);
                    EDDS_FREE_CHECK(FreeStat);
                }

                /* free local RQBs. Note that we dont need to check for NULL */
                /* pointer here, this is done inside free function.          */

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                EDDS_FreeInternalRQB(pGlob->RQBInt_Tx.pRQB);
                EDDS_FreeInternalRQB(pGlob->RQBInt_Tx_Tout.pRQB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                EDDS_FreeInternalRQB(pGlob->RQBInt_LL_Tout.pRQB);
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
                EDDS_FreeInternalRQB(pGlob->RQBInt_Rx_NRT.pRQB);
                EDDS_FreeInternalRQB(pGlob->RQBInt_Rx_cSRT.pRQB);
                EDDS_FreeInternalRQB(pGlob->RQBInt_PHY.pRQB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
                #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
                EDDS_FreeInternalRQB(pGlob->RQBInt_Rx_OverTimer.pRQB);
                EDDS_FreeInternalRQB(pGlob->RQBInt_Rx_OverEvent.pRQB);
                #endif
                #ifdef EDDS_CFG_LED_BLINK_SUPPORT
                EDDS_FreeInternalRQB(pGlob->RQBInt_LED_BlinkTimer.pRQB);
                #endif
                #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
                EDDS_FreeInternalRQB(pGlob->RQBInt_DiagTimer.pRQB);
                #endif

                EDDS_PrmFree(&pGlob->Prm);

                #ifdef EDDS_CFG_GLOB_FAST_MEM
                EDDS_FREE_LOCAL_FAST_MEM(&FreeStat,pGlob);
                #else
                EDDS_FREE_LOCAL_MEM(&FreeStat,pGlob);
                #endif
                EDDS_FREE_CHECK(FreeStat);

            }

            EDDS_FREE_LOCAL_FAST_MEM(&FreeStat,pDDB);
            EDDS_FREE_CHECK(FreeStat);
        }

    }

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CreateDDB(), Status: 0x%X",
                           Result);

    return(Result);
}
//lint +esym(613,pDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_GetDDB                                 +*/
/*+  Input/Output          :    EDDS_HANDLE               hDDB              +*/
/*+                             EDDS_LOCAL_DDB_PTR_TYPE * ppDDB             +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hDDB         : Valid DDB-Handle                                        +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets the DDB associated with the handle.                  +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM    is returned, if no DDB was found.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,ppDDB)
//JB 10/11/2014 ppDDB will be set/allocated within this function, a null ptr would lead to EDD_STS_ERR_PARAM
//AD_DISCUSS 20/11/2014 better solution define a macro to capsule the check
LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_GetDDB(EDDS_HANDLE const hDDB, EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB)
{


    #ifdef EDDS_CFG_DO_DDB_CHECK
    EDDS_GET_BLOCK_HANDLE( g_EDDSInfo.DDBInfo.pBotton,
                           g_EDDSInfo.DDBInfo.pTop,
                           (EDDS_LOCAL_DDB_PTR_TYPE)hDDB,
                           *ppDDB);

    if (LSA_HOST_PTR_ARE_EQUAL(*ppDDB, LSA_NULL))
        return EDD_STS_ERR_PARAM;

    return EDD_STS_OK;
    #else
    if (LSA_HOST_PTR_ARE_EQUAL((EDDS_LOCAL_DDB_PTR_TYPE)hDDB, LSA_NULL))
        return EDD_STS_ERR_PARAM;

    *ppDDB = (EDDS_LOCAL_DDB_PTR_TYPE) hDDB;
    return EDD_STS_OK;
    #endif
}
//lint +esym(613,ppDDB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_CloseDDB                               +*/
/*+  Input/Output               EDDS_LOCAL_DDB_PTR_TYPE    pDDB             +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB to be closed                             +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an DDB. The DDB is unqueued and the memory is freed+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM    is returned, if hDDB is invalid.     +*/
/*+                                                                         +*/
/*+               A fatal error is signaled, if no DDB is present.          +*/
/*+                                                                         +*/
/*+               Note: This function does not check for open handles for   +*/
/*+                     this device!!                                       +*/
/*+                     Does not check for open components!                 +*/
/*+                                                                         +*/
/*+               Important: No internal RQB must be in use !!! not checked.+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pDDB)
//JB 10/11/2014 pDDB will be checked in EDDS_GetDDB
//AD_DISCUSS 20/11/2014 pDDB will be checked depending on EDDS_CFG_DO_DDB_CHECK
//@fixme am2219 function refactoring, return value -> LSA_VOID
LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CloseDDB(EDDS_HANDLE hDDB)
{

    LSA_RESULT              Status;
    LSA_UINT16              FreeStat;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;
    EDDS_LOCAL_DDB_PTR_TYPE pRemDDB;

    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :EDDS_CloseDDB(hDDB: 0x%X)",
                           hDDB);

    pRemDDB = LSA_NULL;
    pDDB    = LSA_NULL;  /* to prevent compiler warning */

    Status = EDDS_GetDDB(hDDB, &pDDB);

    if ( Status == EDD_STS_OK )
    {

        if (g_EDDSInfo.DDBInfo.CntDDB == 0)
            EDDS_FatalError(EDDS_FATAL_ERR_CLOSE_DDB,
                            EDDS_MODULE_ID,
                            __LINE__);

        g_EDDSInfo.DDBInfo.CntDDB--;

        EDDS_REM_BLOCK(g_EDDSInfo.DDBInfo.pBotton, g_EDDSInfo.DDBInfo.pTop, pDDB, pRemDDB);

        if ( !LSA_HOST_PTR_ARE_EQUAL(pDDB, pRemDDB) )
            EDDS_FatalError(EDDS_FATAL_ERR_CLOSE_DDB,
                            EDDS_MODULE_ID,
                            __LINE__);

        /*-----------------------------------------------------------------------*/
        /* Free global structure within DDB                                      */
        /*-----------------------------------------------------------------------*/

        if ( !LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob, LSA_NULL) )
        {

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pTxInfo, LSA_NULL) )
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat,pDDB->pGlob->pTxInfo);
                EDDS_FREE_CHECK(FreeStat);
            }
#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            if ( (! pDDB->pGlob->ZeroCopyInterface) && (! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pTxTransferMem, LSA_NULL)) )
            {
                EDDS_FREE_TX_TRANSFER_BUFFER_MEM(pDDB->hSysDev,&FreeStat,pDDB->pGlob->pTxTransferMem);
                EDDS_FREE_CHECK(FreeStat);
            }
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

            if ( ! LSA_HOST_PTR_ARE_EQUAL(pDDB->pGlob->pMCInfo, LSA_NULL) )
            {
                EDDS_FREE_LOCAL_MEM(&FreeStat,pDDB->pGlob->pMCInfo);
                EDDS_FREE_CHECK(FreeStat);
            }

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Tx.pRQB);
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Tx_Tout.pRQB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_LL_Tout.pRQB);
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Rx_NRT.pRQB);
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Rx_cSRT.pRQB);
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_PHY.pRQB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
            #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Rx_OverTimer.pRQB);
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_Rx_OverEvent.pRQB);
            #endif
            #ifdef EDDS_CFG_LED_BLINK_SUPPORT
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_LED_BlinkTimer.pRQB);
            #endif
            #ifdef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT
            EDDS_FreeInternalRQB(pDDB->pGlob->RQBInt_DiagTimer.pRQB);
            #endif

            EDDS_PrmFree(&pDDB->pGlob->Prm);

            #ifdef EDDS_CFG_GLOB_FAST_MEM
            EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pDDB->pGlob);
            #else
            EDDS_FREE_LOCAL_MEM(&FreeStat, pDDB->pGlob);
            #endif

            pDDB->pGlob = LSA_NULL;

            /* -----------------------------------------------------------------*/
            /* what can we do if free fails ?? Its fatal, because it should not */
            /* occur, but we also could ignore it, because its not a problem for*/
            /* us. Currently we signal an error and continue                    */
            /* -----------------------------------------------------------------*/

            EDDS_FREE_CHECK(FreeStat);
        }


        EDDS_FREE_LOCAL_FAST_MEM(&FreeStat, pDDB);

        /* ---------------------------------------------------------------------*/
        /* what can we do if free fails ?? Its fatal, because it should not     */
        /* occur, but we also could ignore it, because its not a problem for us */
        /* Currently we signal an error and continue                            */
        /* ---------------------------------------------------------------------*/

        EDDS_FREE_CHECK(FreeStat);

    }


    EDDS_CORE_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:EDDS_CloseDDB(), Status: 0x%X",
                           Status);


    return (Status);

}
//lint +esym(613,pDDB)

/*****************************************************************************/
/*  end of file edds_dev.c                                                   */
/*****************************************************************************/

