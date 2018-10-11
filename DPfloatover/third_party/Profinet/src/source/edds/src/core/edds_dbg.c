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
/*  F i l e               &F: edds_dbg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Debugging support functions                 */
/*                          (system dependend)                               */
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
/*  23.10.03    JS    added additional parameters of requests.               */
/*                    added ifdef EDDS_CFG_HW_IBC32                          */
/*  15.06.04    JS    removed ASRT functions                                 */
/*  27.01.05    JS    ported to LTRC                                         */
/*  20.10.05    JS    added  EDDS_TRIGGER_LED_BLINK_CYCLE                    */
/*  27.06.06    JS    added  EDDS_FILE_SYSTEM_EXTENSION                      */
#endif
/*****************************************************************************/


/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  4
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_DBG */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */


EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

#ifdef EDDS_CFG_REENTRANCE_CHECK

#include "edds_int.h"            /* internal header */

EDDS_STATIC LSA_UINT32 EDDS_LowPrioFlag;
EDDS_STATIC LSA_UINT32 EDDS_MiddlePrioFlag;
EDDS_STATIC LSA_UINT32 EDDS_HighPrioFlag;
EDDS_STATIC LSA_UINT32 EDDS_TimerPrioFlag;
EDDS_STATIC LSA_UINT32 EDDS_IntPrioFlag;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ReentranceCheckInit                     +*/
/*+  Input/Output               LSA_VOID                                    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Init reentrance check.                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ReentranceCheckInit(LSA_VOID)
{

    EDDS_LowPrioFlag  = 0;
    EDDS_MiddlePrioFlag = 0;
    EDDS_HighPrioFlag = 0;
    EDDS_TimerPrioFlag = 0;
    EDDS_IntPrioFlag  = 0;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EnterCheckRequest                       +*/
/*+  Input/Output               EDD_UPPER_RQB_PTR_TYPE pRQB                 +*/
/*+                             LSA_UINT32            *pReentrPrio          +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pReentrPrio: Pointer to LSA_UINT32 for Prio (out):                     +*/
/*+               EDDS_RENTR_PRIO_NONE                                      +*/
/*+               EDDS_RENTR_PRIO_LOW                                       +*/
/*+               EDDS_RENTR_PRIO_MIDDLE                                    +*/
/*+               EDDS_RENTR_PRIO_HIGH                                      +*/
/*+               EDDS_RENTR_PRIO_TIMER  ( not set yet )                    +*/
/*+               EDDS_RENTR_PRIO_INT    ( not set yet )                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function entry of        +*/
/*+                                                                         +*/
/*+               edds_system()                                             +*/
/*+               edds_request()                                            +*/
/*+               edds_open_channel()                                       +*/
/*+               edds_close_channel()                                      +*/
/*+                                                                         +*/
/*+               Note: This function dont detect all possible conflicts    +*/
/*+                     situations but most.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckRequest(
    EDD_UPPER_RQB_PTR_TYPE pRQB,
    LSA_UINT32            *pReentrPrio)

{

    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE    pParam;

    *pReentrPrio    = EDDS_RENTR_PRIO_NONE;

    switch (EDD_RQB_GET_OPCODE(pRQB))
    {
        case EDD_OPC_SYSTEM:
            switch ( EDD_RQB_GET_SERVICE(pRQB) )
            {
                case EDDS_SRV_DEV_INTERRUPT:
                    pParam = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;
                    EDDS_ASSERT_NULL_PTR(pParam);
                    switch (pParam->ID)
                    {
                        case EDDS_ISR_TX:
                            /* high prio context */
                            if ( ( EDDS_IntPrioFlag ) ||
                                 ( EDDS_TimerPrioFlag ) ||
                                 ( EDDS_HighPrioFlag ) )
                            {
                                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
                            }
                            else
                            {
                                EDDS_HighPrioFlag = 1;
                                *pReentrPrio    = EDDS_RENTR_PRIO_HIGH;
                            }
                            break;

                        case EDDS_ISR_RX_NRT:
                        case EDDS_ISR_PHY:
                            /* low prio context  */
                            if ( ( EDDS_IntPrioFlag  ) ||
                                 ( EDDS_TimerPrioFlag ) ||
                                 ( EDDS_HighPrioFlag ) ||
                                 ( EDDS_MiddlePrioFlag ) ||
                                 ( EDDS_LowPrioFlag  ))
                            {
                                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
                            }
                            else
                            {
                                EDDS_LowPrioFlag = 1;
                                *pReentrPrio    = EDDS_RENTR_PRIO_LOW;
                            }
                            break;

                        default:
                            /* unknown. ignore   */
                            break;
                    }
                    break;

                case EDDS_SRV_DEV_TIMEOUT:
                    /* high prio context */
                    if ( ( EDDS_IntPrioFlag  ) ||
                         ( EDDS_TimerPrioFlag ) ||
                         ( EDDS_HighPrioFlag ))
                    {
                        EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                            EDDS_MODULE_ID,
                                            __LINE__);
                    }
                    else
                    {
                        EDDS_HighPrioFlag = 1;
                        *pReentrPrio    = EDDS_RENTR_PRIO_HIGH;
                    }

                    break;

                case EDDS_SRV_DEV_TRIGGER:
                    pParam = (EDDS_UPPER_DEV_INTERNAL_PTR_TYPE)pRQB->pParam;
                    EDDS_ASSERT_NULL_PTR(pParam);

                    switch (pParam->ID)
                    {
                        case EDDS_TRIGGER_PROVIDER_CYCLE:
                            /* high prio context */
                            if ( ( EDDS_IntPrioFlag  ) ||
                                 ( EDDS_TimerPrioFlag ) ||
                                 ( EDDS_HighPrioFlag ))
                            {
                                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
                            }
                            else
                            {
                                EDDS_HighPrioFlag = 1;
                                *pReentrPrio    = EDDS_RENTR_PRIO_HIGH;
                            }
                            break;

                        case EDDS_TRIGGER_CONSUMER_CYCLE:
                        case EDDS_TRIGGER_PROV_INDICATION:
                            /* middle or low prio context */
                            if ( ( EDDS_IntPrioFlag  ) ||
                                 ( EDDS_TimerPrioFlag ) ||
                                 ( EDDS_HighPrioFlag ) ||
                                 ( EDDS_MiddlePrioFlag ))

                            {
                                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
                            }
                            else
                            {
                                EDDS_MiddlePrioFlag = 1;
                                *pReentrPrio      = EDDS_RENTR_PRIO_MIDDLE;
                            }
                            break;

                        case EDDS_TRIGGER_NRT_CANCEL_PART2:
                        case EDDS_TRIGGER_RX_OVER_CYCLE:
                        case EDDS_TRIGGER_RX_OVER_EVENT:
                        case EDDS_TRIGGER_LED_BLINK_CYCLE:
                            /* low prio context */
                            if ( ( EDDS_IntPrioFlag  ) ||
                                 ( EDDS_TimerPrioFlag ) ||
                                 ( EDDS_HighPrioFlag ) ||
                                 ( EDDS_MiddlePrioFlag ) ||
                                 ( EDDS_LowPrioFlag  ))
                            {
                                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                                    EDDS_MODULE_ID,
                                                    __LINE__);
                            }
                            else
                            {
                                EDDS_LowPrioFlag = 1;
                                *pReentrPrio   = EDDS_RENTR_PRIO_LOW;
                            }
                            break;

                        default:
                            /* unknown. ignore  */
                            break;
                    }
                    break;
                default:
                    /* unknown. ignore  */
                    break;
            }
            break;

        case EDD_OPC_REQUEST:
        case EDD_OPC_OPEN_CHANNEL:
        case EDD_OPC_CLOSE_CHANNEL:
            /* all handled within low-prio context */
            if ( ( EDDS_IntPrioFlag  ) ||
                 ( EDDS_TimerPrioFlag ) ||
                 ( EDDS_HighPrioFlag ) ||
                 ( EDDS_MiddlePrioFlag ) ||
                 ( EDDS_LowPrioFlag  ))
            {
                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
            }
            else
            {
                EDDS_LowPrioFlag = 1;
                *pReentrPrio   = EDDS_RENTR_PRIO_LOW;
            }
            break;

        case EDD_OPC_REQUEST_SRT_BUFFER:
            /* no context requirement */
            break;

        default:
            /* unknown opcode. ignore */
            break;
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ExitCheckRequest                        +*/
/*+  Input/Output               EDD_UPPER_RQB_PTR_TYPE pRQB                 +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ReentrPrio:  current Reentrance Prio                                   +*/
/*+               EDDS_RENTR_PRIO_NONE                                      +*/
/*+               EDDS_RENTR_PRIO_LOW                                       +*/
/*+               EDDS_RENTR_PRIO_MIDDLE                                    +*/
/*+               EDDS_RENTR_PRIO_HIGH                                      +*/
/*+               EDDS_RENTR_PRIO_TIMER                                     +*/
/*+               EDDS_RENTR_PRIO_INT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function exit of         +*/
/*+                                                                         +*/
/*+               edds_system()                                             +*/
/*+               edds_request()                                            +*/
/*+               edds_open_channel()                                       +*/
/*+               edds_close_channel()                                      +*/
/*+                                                                         +*/
/*+               Note: This function dont detect all possible conflicts    +*/
/*+                     situations but most.                                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckRequest(
    LSA_UINT32  ReentrPrio)

{

    switch (ReentrPrio)
    {
        case  EDDS_RENTR_PRIO_NONE :
            /* nothing to do/check */
            break;
        case EDDS_RENTR_PRIO_LOW  :
            /* low prio context  */
            if ( ( EDDS_IntPrioFlag  ) ||
                 ( EDDS_TimerPrioFlag ) ||
                 ( EDDS_HighPrioFlag ) ||
                 ( EDDS_MiddlePrioFlag ) ||
                 ( ! EDDS_LowPrioFlag ))
            {
                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
            }
            else
            {
                EDDS_LowPrioFlag = 0;
            }
            break;
        case EDDS_RENTR_PRIO_MIDDLE:
            /* middle prio context */
            if ( ( EDDS_IntPrioFlag    ) ||
                 ( EDDS_TimerPrioFlag  ) ||
                 ( EDDS_HighPrioFlag   ) ||
                 ( ! EDDS_MiddlePrioFlag ))
            {
                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
            }
            else
            {
                EDDS_MiddlePrioFlag = 0;
            }
            break;
        case EDDS_RENTR_PRIO_HIGH :
            /* high prio context */
            if ( ( EDDS_IntPrioFlag  ) ||
                 ( EDDS_TimerPrioFlag ) ||
                 (! EDDS_HighPrioFlag ))
            {
                EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                    EDDS_MODULE_ID,
                                    __LINE__);
            }
            else
            {
                EDDS_HighPrioFlag = 0;
            }
            break;
        case EDDS_RENTR_PRIO_TIMER :
        case EDDS_RENTR_PRIO_INT  :
        default:
            /* invalid Prio */
            EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                                EDDS_MODULE_ID,
                                __LINE__);
            break;
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EnterCheckISR                           +*/
/*+  Input/Output               LSA_VOID                                    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function entry of        +*/
/*+                                                                         +*/
/*+               edds_interrupt()                                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckISR(LSA_VOID)
{

    /* Note: We should check for individuell int-sources. We currently only */
    /*       set a flag for ISR at all. We only use this to see if requests */
    /*       are lower prio as ISR not to check reentrance of ISR.          */

    EDDS_IntPrioFlag++;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ExitCheckISR                            +*/
/*+  Input/Output               LSA_VOID                                    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function exit of         +*/
/*+                                                                         +*/
/*+               edds_interrupt()                                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckISR(LSA_VOID)
{

    EDDS_IntPrioFlag--;

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_EnterCheckTimer                         +*/
/*+  Input/Output               LSA_VOID                                    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function entry of        +*/
/*+                                                                         +*/
/*+               edds_timeout()                                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckTimer(LSA_VOID)
{

    if ( EDDS_TimerPrioFlag  )
    {
        EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                            EDDS_MODULE_ID,
                            __LINE__);
    }
    else
    {
        EDDS_TimerPrioFlag = 1;
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_ExitCheckTimer                          +*/
/*+  Input/Output               LSA_VOID                                    +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for reentrance / prio conflict                     +*/
/*+               If a reentrance problem is detected a fatal error is set  +*/
/*+               This function shall be called on function exit of         +*/
/*+                                                                         +*/
/*+               edds_timeout()                                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckTimer(LSA_VOID)
{

    if ( ! EDDS_TimerPrioFlag  )
    {
        EDDS_FatalError(    EDDS_FATAL_ERR_REENTRANCE,
                            EDDS_MODULE_ID,
                            __LINE__);
    }
    else
    {
        EDDS_TimerPrioFlag = 0;
    }

}

#endif


#ifdef EDDS_CFG_DEBUG_ENABLE


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDS_DbgDumpFrame                            +*/
/*+  Input                 :    LSA_INT8  EDDS_LOCAL_MEM_ATTR * pString     +*/
/*+                             LSA_UINT8 EDDS_LOWER_MEM_ATTR * pFrame      +*/
/*+                             LSA_UINT32                     Len          +*/
/*+  Result                :    ----                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Dump Frame to screen. hex/char output.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DbgDumpFrame(
    LSA_INT8  EDDS_LOCAL_MEM_ATTR    * pString,
    LSA_UINT8 EDDS_LOWER_MEM_ATTR    * pFrame,
    LSA_UINT32                         Len)
{

    LSA_UINT32  i,j;
    LSA_UINT8   Chars[17];

    i=0;
    while (i<Len)
    {
        printf(pString);
        printf("[%4.4lX]: ",i);
        for (j=0; (j<16) && (i<Len) ; j++)
        {
            printf("%2.2X ",pFrame[i]);
            Chars[j] = pFrame[i];
            if ( Chars[j] < 0x20) Chars[j] = '.';
            i++;
        }
        Chars[j] = 0;
        while (j++<16) printf("   ");
        printf((LSA_INT8 EDDS_LOCAL_MEM_ATTR *)Chars);
        printf("\n");
    }

}

#endif

