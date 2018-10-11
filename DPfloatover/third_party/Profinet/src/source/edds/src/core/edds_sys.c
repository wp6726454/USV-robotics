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
/*  F i l e               &F: edds_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-system input-functions                      */
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
/*  26.09.03    JS    bugfix in edds_version. changed implementation.        */
/*                    (was wrong if we have fill-bytes within version-struct)*/
/*  20.01.04    JS    edds_SRTProviderGetCycleCounter added                  */
/*  28.01.05    JS    LTRC support                                           */
/*                    use LSA_UINT16 instead of LSA_RESULT with edds_version()*/
/*  06.07.05    JS    added multiport support                                */
/*  04.10.05    JS    added LED-blink support                                */
/*  09.03.06    JS    added endianes check                                   */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
/*  23.01.07    JS    added  dynamic PortCnt                                 */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_EDD_SYS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */

EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

EDDS_INFO_TYPE             g_EDDSInfo;

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_init                                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the EDDS.                                     +*/
/*+               The number of handles the edds can manage is defined by   +*/
/*+               EDDS_CFG_HANDLE_CNT. The memory for handle-management is  +*/
/*+               static or will be allocated depending on                  +*/
/*+               EDDS_LOCAL_MEM_STATIC.                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_init(LSA_VOID)
{
    LSA_RESULT              Status;
    LSA_UINT16              i;

    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_init()");

    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"--> edds_init()");

    #ifdef EDDS_CFG_REENTRANCE_CHECK
    EDDS_ReentranceCheckInit();
    #endif

    /* check for correct endianess define */

    i = 0x1234;
    #ifndef EDDS_CFG_BIG_ENDIAN
    if ( *(LSA_UINT8*)&i != 0x34 )
    {
        #else
    if ( *(LSA_UINT8*)&i != 0x12 )
    {
        #endif
        EDDS_FatalError(EDDS_FATAL_ERR_ENDIANES,
                        EDDS_MODULE_ID,
                        __LINE__);
    }

    Status = EDDS_HandleIni();

    if (Status != EDD_STS_OK) Status = EDD_STS_ERR_RESOURCE;
    else
    {
        Status = EDDS_TimerInfoIni();

        if ( Status != EDD_STS_OK )
        {
            Status = EDDS_HandleClose();
        }
        else
        {
            EDDS_DevInfoIni();
        }
    }

    EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<-- edds_init(). Response: 0x%X",Status);

    EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_init(), Status: 0x%X",
                           Status);

    return Status;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_undo_init                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes EDDS.                                       +*/
/*+               This is only possible if no device is open                +*/
/*+               EDD_STS_ERR_SEQUENCE if device is open.                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_undo_init(LSA_VOID)
{

    LSA_RESULT  Status;

    EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_undo_init()");

    Status = EDDS_DevInfoClose(); /* only possible if no device is open */

    if (Status == EDD_STS_OK)
    {
        Status = EDDS_HandleClose();

        if (Status == EDD_STS_OK)
        {
            Status = EDDS_TimerInfoFree();
        }
    }

    EDDS_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_undo_init(), Status: 0x%X",
                           Status);

    return Status;

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    edds_SRTProviderGetCycleCounter             +*/
/*+  Input                      EDDS_HANDLE    hDDB                         +*/
/*+                             EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter   +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to Device Description Block (Device-management) +*/
/*+  pCycleCounter: Pointer to address for CycleCounter                     +*/
/*+                                                                         +*/
/*+  Result                :    EDD_STS_OK                                  +*/
/*+                             EDD_STS_ERR_SEQUENCE                        +*/
/*+                             EDD_STS_ERR_PARAM                           +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description:  Gets actual provider CycleCounter of cyclic SRT.         +*/
/*+                                                                         +*/
/*+                The CycleCounter is based on 31.25us and wraps around    +*/
/*+                if 32Bit is reached. The function can be alled anytime   +*/
/*+                except for startup/shutdown of device/srt-component      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef EDDS_CFG_SRT_INCLUDE

LSA_RESULT EDDS_SYSTEM_IN_FCT_ATTR edds_SRTProviderGetCycleCounter(
    EDDS_HANDLE                 hDDB,
    EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter)
{

    LSA_RESULT              Status;
    EDDS_LOCAL_DDB_PTR_TYPE pDDB;

    EDDS_SYSTEM_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,
                           "IN :edds_SRTProviderGetCycleCounter(hDDB: 0x%X,pCycleCounter: 0x%X)",
                           hDDB,
                           pCycleCounter);

    if (LSA_HOST_PTR_ARE_EQUAL(pCycleCounter, LSA_NULL))
    {
        EDDS_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,
                           "edds_SRTProviderGetCycleCounter, pCycleCounter is NULL_PTR");
        return EDD_STS_ERR_EXCP;
    }

    Status = EDD_STS_OK;

    pDDB = (EDDS_LOCAL_DDB_PTR_TYPE)hDDB;

    if ( (!LSA_HOST_PTR_ARE_EQUAL(pDDB,LSA_NULL)) && (!LSA_HOST_PTR_ARE_EQUAL(pDDB->pSRT,LSA_NULL)) )
    {
        Status = EDDS_RTProviderGetCycleCounter(&pDDB->pSRT->CSRT,pCycleCounter);
    }
    else
	{
		Status = EDD_STS_ERR_SEQUENCE;
	}

    EDDS_SYSTEM_TRACE_03(0,LSA_TRACE_LEVEL_CHAT,
                           "OUT:edds_SRTProviderGetCycleCounter(High: 0x%X, Low: 0x%X), Status: 0x%X",
                           pCycleCounter->High,
                           pCycleCounter->Low,
                           Status);

    return(Status);

}

#endif

/*****************************************************************************/
/*  end of file EDDS_SYS.C                                                   */
/*****************************************************************************/

