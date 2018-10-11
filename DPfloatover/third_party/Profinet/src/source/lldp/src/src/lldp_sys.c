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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_sys.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP-system functions                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  19.04.04    JS    initial version.                                       */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  1
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_SYS */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


#ifdef LLDP_CFG_LOCAL_MEM_STATIC

struct LLDP_STATIC_TYPE_
{
    /* main-management of LLDP */
    LLDP_LOCAL_MEM_ATTR LLDP_MGM_TYPE        Mgm;

} g_LLDP_Static;

#endif

LLDP_LOCAL_MEM_ATTR LLDP_MGM_PTR_TYPE   g_pLLDPInfo;


/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_FreeMgm                                +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LLDP_RSP_OK                                 +*/
/*+                             LLDP_RSP_ERR_SEQUENCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees LLDP Management if allocated.                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_FreeMgm(LSA_VOID)
{

    LLDP_RSP Response;

    #ifndef LLDP_CFG_LOCAL_MEM_STATIC
    LSA_UINT16   FreeStat;
    #endif

    Response = LLDP_RSP_OK;

    if (!LSA_HOST_PTR_ARE_EQUAL(g_pLLDPInfo, LSA_NULL) )
    {

        /* ----------------------------------------------------------------------*/
        /* free Timer if present                                                 */
        /* ----------------------------------------------------------------------*/
        if ( g_pLLDPInfo->Glob.CyclicTimerID != LLDP_TIMER_ID_INVALID )
        {
            Response = LLDP_TimerFree(g_pLLDPInfo->Glob.CyclicTimerID);

            /* Note: We dont expect the Timer RQB to be underway ! */
            if ( Response != LLDP_RSP_OK )
            {
                LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,"LLDP_FreeMgm. Timer-free (0x%X)",Response);
                LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            }

            g_pLLDPInfo->Glob.CyclicTimerID = LLDP_TIMER_ID_INVALID;
        }
        /* ----------------------------------------------------------------------*/
        /* free global management if not static                                  */
        /* ----------------------------------------------------------------------*/
 #ifndef LLDP_CFG_LOCAL_MEM_STATIC
        LLDP_FREE_LOCAL_MEM(&FreeStat, g_pLLDPInfo);
        LLDP_MEM_FREE_FAIL_CHECK(FreeStat);
        g_pLLDPInfo = LSA_NULL;
 #endif
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_init                                   +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LLDP_RESP                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      LLDP_RSP_OK                                               +*/
/*+               LLDP_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the LLDP.                                     +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_SYSTEM_IN_FCT_ATTR lldp_init(LSA_VOID)
{
    LLDP_RSP                Status;
    LSA_UINT32              i;
#ifndef LLDP_CFG_LOCAL_MEM_STATIC
	LLDP_LOCAL_MEM_PTR_TYPE pAlloc;
#endif

    Status = LLDP_RSP_OK;
    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_init()");

    /* --------------------------------------------------------------------------*/
    /* If memory should be static we use global memory.                          */
    /* --------------------------------------------------------------------------*/
#ifdef LLDP_CFG_LOCAL_MEM_STATIC

	LLDP_MEMSET_LOCAL(&g_LLDP_Static,0,sizeof(g_LLDP_Static));
	g_pLLDPInfo           = &g_LLDP_Static.Mgm;

#else
	/* --------------------------------------------------------------------------*/
	/* If not static we allocate memory.                                         */
	/* --------------------------------------------------------------------------*/
	LLDP_ALLOC_LOCAL_MEM(&pAlloc, sizeof(LLDP_MGM_TYPE));
	g_pLLDPInfo = (LLDP_MGM_PTR_TYPE)pAlloc;

	if ( LSA_HOST_PTR_ARE_EQUAL(g_pLLDPInfo, LSA_NULL) )
	{
		Status = LLDP_RSP_ERR_RESOURCE;
		LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,"lldp_init(). Allocating memory failed.");
	}
	else
	{
		LLDP_MEMSET_LOCAL(g_pLLDPInfo,0,sizeof(LLDP_MGM_TYPE));
	}

#endif


    /* --------------------------------------------------------------------------*/
    /* Setup management                                                          */
    /* --------------------------------------------------------------------------*/

    if (Status == LLDP_RSP_OK )
    {
        g_pLLDPInfo->Glob.LLError.lsa_component_id = LSA_COMP_ID_UNUSED;

        g_pLLDPInfo->HUser.HandleCnt = LLDP_CFG_MAX_CHANNELS;
        g_pLLDPInfo->HUser.UsedCnt   = 0;

        for ( i=0; i<LLDP_CFG_MAX_CHANNELS; i++)
        {
            g_pLLDPInfo->HUser.HDB[i].State        = LLDP_HANDLE_STATE_FREE;
            g_pLLDPInfo->HUser.HDB[i].ThisHandle   = (LSA_HANDLE_TYPE) (i);
        }
        /* ----------------------------------------------------------------------*/
        /* Setup Timer-Management                                                */
        /* ----------------------------------------------------------------------*/
        LLDP_TimerMgmSetup();
        /* ----------------------------------------------------------------------*/
        /* Allocate Send/TTL-Cyclic Timer                                        */
        /* ----------------------------------------------------------------------*/
        g_pLLDPInfo->Glob.CyclicTimerID      = LLDP_TIMER_ID_INVALID;
        g_pLLDPInfo->Glob.CyclicTimerUserCnt = 0;
        Status = LLDP_TimerAlloc( LSA_TIMER_TYPE_CYCLIC,
                                  LSA_TIME_BASE_1S,
                                  LLDP_TIMER_USER_ID_CYLIC,
                                  LLDP_CylicTimerCB,
                                  &g_pLLDPInfo->Glob.CyclicTimerID);
        if ( Status != LLDP_RSP_OK )
        {
            LLDP_FreeMgm();
        }
    }
    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_init(), Status: 0x%X",Status);
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_undo_init                              +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      LLDP_RSP_OK                                               +*/
/*+               LLDP_RSP_ERR_SEQUENCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes LLDP.                                       +*/
/*+               This is only possible if no device is open                +*/
/*+               LLDP_RSP_ERR_SEQUENCE if device is open.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_SYSTEM_IN_FCT_ATTR  lldp_undo_init(LSA_VOID)
{

    LLDP_RSP  Status;

    Status = LLDP_RSP_OK;

    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_undo_init()");
    if ( LLDP_MGM_GET_HUSER_PTR()->UsedCnt  )
    {
        LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,"lldp_undo_init(). Not all channels closed yet.");
        Status = LLDP_RSP_ERR_SEQUENCE;
    }
    else
    {
        Status = LLDP_FreeMgm();
    }
    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_undo_init(), Status: 0x%X",Status);
    return(Status);
}

/*****************************************************************************/
/*  end of file LLDP_SYS.C                                                    */
/*****************************************************************************/

