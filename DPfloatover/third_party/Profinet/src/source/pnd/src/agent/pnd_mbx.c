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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_mbx.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements mailboxes for LSA-component PNDRIVER                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1113
#define PND_MODULE_ID       1113

#include "pnd_int.h"
#include "pnd_sys.h"


#define PND_QUIT_MSG 0x42

/*---------------------------------------------------------------------------*/

enum PND_MBX_MSG_PRIO
{
    PND_MBX_MSG_PRIO_LOW  = 1,
    PND_MBX_MSG_PRIO_HIGH = 2
};

typedef struct pnd_mbx_pnio_header_tag {
    LSA_RQB_HEADER(struct pnd_pnio_header*)
} PND_MBX_PNIO_HEADER_TYPE, *PND_MBX_PNIO_HEADER_PTR_TYPE;

typedef struct pnd_mbx_msg_queue_tag
{
    LSA_BOOL                      bUsed;
    PND_MBX_PNIO_HEADER_PTR_TYPE  pFirst;
} PND_MBX_MSG_QUEUE_TYPE, *PND_MBX_MSG_QUEUE_PTR_TYPE;

typedef struct pnd_mbx_thread_tag
{
    LSA_UINT16              blocking_wait;
    PND_MBX_MSG_QUEUE_TYPE  msg_queue_low;
    PND_MBX_MSG_QUEUE_TYPE  msg_queue_high;
    LSA_UINT32              uMsgQueueHighProcessedCnt;
    LSA_BOOL                bUsed;
    LSA_BOOL                bQuitThread;
    LSA_UINT32              hThread;
    struct
    {
        LSA_INT             iActMsgInTransfer;
        LSA_UINT32          uTotalMsgCount;
    } stats;
} PND_MBX_THREAD_TYPE, *PND_MBX_THREAD_PTR_TYPE;

typedef struct pnd_mbx_thread_store_tag
{
    PND_MBX_THREAD_TYPE sThreads[PND_MAX_TASK_ID];
} PND_MBX_THREAD_STORE_TYPE, *PND_MBX_THREAD_STORE_PTR_TYPE;

typedef struct pnd_mbx_mbx_tag {
    LSA_UINT16     mbox_prio;
    LSA_UINT32     pnd_threadId;
} PND_MBX_MBX_TYPE, *PND_MBX_MBX_PTR_TYPE; 

typedef struct pnd_mbx_store_tag{
    LSA_BOOL                     bInitDone;
    LSA_UINT16                   blocking_wait;
    PND_MBX_THREAD_STORE_TYPE    sThreadStore;
    PND_MBX_MBX_TYPE             sMbx[PND_MAX_MBX_ID];
} PND_MBX_STORE_TAG, *PND_MBX_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

static PND_MBX_STORE_TAG g_PndriverMbx;

/*---------------------------------------------------------------------------*/

static LSA_UINT16 pnd_enter_exit_id = PND_LOCK_ID_INVALID;

/*---------------------------------------------------------------------------*/

static LSA_VOID pnd_mbx_enqueue(PND_MBX_PNIO_HEADER_PTR_TYPE pReq, PND_MBX_MSG_QUEUE_PTR_TYPE pQueue)
{
    PND_RQB_PTR_TYPE pFirst;
    PND_RQB_PTR_TYPE pPrev;
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)pReq;

    PND_ASSERT(pQueue != LSA_NULL);
    PND_ASSERT(pRQB != LSA_NULL);

    if(pQueue->pFirst == LSA_NULL)
    {
        pQueue->pFirst = pReq;
        LSA_RQB_SET_PREV_RQB_PTR(pRQB, pRQB);
        LSA_RQB_SET_NEXT_RQB_PTR(pRQB, pRQB);
    }
    else
    {
        pFirst = (PND_RQB_PTR_TYPE)pQueue->pFirst;
        pPrev  = LSA_RQB_GET_PREV_RQB_PTR(pFirst);

        LSA_RQB_SET_PREV_RQB_PTR(pRQB, pPrev);
        LSA_RQB_SET_NEXT_RQB_PTR(pRQB, pFirst);
        LSA_RQB_SET_NEXT_RQB_PTR(pPrev,  pRQB);
        LSA_RQB_SET_PREV_RQB_PTR(pFirst, pRQB);
    }
}

static PND_RQB_PTR_TYPE pnd_mbx_dequeue(PND_MBX_MSG_QUEUE_PTR_TYPE pQueue)
{
    PND_RQB_PTR_TYPE pFirst = LSA_NULL;
    PND_RQB_PTR_TYPE pNewFirst;

    PND_ASSERT(pQueue != LSA_NULL);

    if(pQueue->pFirst == LSA_NULL) /* Queue Empty */
    {
        return LSA_NULL;
    }
    else
    {
        pFirst = (PND_RQB_PTR_TYPE)pQueue->pFirst;

        if(LSA_RQB_GET_NEXT_RQB_PTR(pFirst) == pFirst) /* Last Elem */
        {
            pQueue->pFirst = LSA_NULL;
        }
        else /* Further Elements */
        {
            pNewFirst = LSA_RQB_GET_NEXT_RQB_PTR(pFirst);

            LSA_RQB_SET_PREV_RQB_PTR(pNewFirst, LSA_RQB_GET_PREV_RQB_PTR(pFirst));
            LSA_RQB_SET_NEXT_RQB_PTR(LSA_RQB_GET_PREV_RQB_PTR(pFirst), pNewFirst);

            pQueue->pFirst = (PND_MBX_PNIO_HEADER_PTR_TYPE)pNewFirst;
        }
    }

    LSA_RQB_SET_PREV_RQB_PTR(pFirst, LSA_NULL);
    LSA_RQB_SET_NEXT_RQB_PTR(pFirst, LSA_NULL);

    return pFirst;
}

static PND_MBX_THREAD_PTR_TYPE pnd_mbx_get_thread(LSA_UINT32 hThread)
{
    PND_MBX_THREAD_PTR_TYPE pThread;

    PND_ASSERT(hThread < PND_MAX_TASK_ID);

    pThread = &g_PndriverMbx.sThreadStore.sThreads[hThread];

    PND_ASSERT(pThread->bUsed == LSA_TRUE);

    return pThread;
}

static LSA_VOID* pnd_mbx_get_next_msg_for_thread(PND_MBX_THREAD_PTR_TYPE pThread)
{
    PND_RQB_PTR_TYPE pRQB = LSA_NULL;

    if(pThread->uMsgQueueHighProcessedCnt == 2)
    {
        pThread->uMsgQueueHighProcessedCnt = 0;

        /* Check for Low Message as we have processed a couple of highs previously */
        pRQB = pnd_mbx_dequeue(&pThread->msg_queue_low);
    }

    if(pRQB == LSA_NULL)
    {
        /* Check for High Message */
        pRQB = pnd_mbx_dequeue(&pThread->msg_queue_high);

        if(pRQB != LSA_NULL)
        {
            pThread->uMsgQueueHighProcessedCnt++;
        }
        else
        {
            pThread->uMsgQueueHighProcessedCnt = 0;

            /* Check for Low Message */
            pRQB = pnd_mbx_dequeue(&pThread->msg_queue_low);
        }
    }

    if(pRQB != LSA_NULL)
    {
        pThread->stats.iActMsgInTransfer--;

        // PND_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "pnd_mbx_get_next_msg_for_thread(): RQB(0x%8X) hThread(0x%X) CompId(0x%x) Opc(0x%x) MsgInTransfer(%d)", pRQB, pThread->hThread, LSA_RQB_GET_COMP_ID(pRQB), LSA_RQB_GET_OPCODE(pRQB), pThread->stats.iActMsgInTransfer );
    }
    else
    {
        // PND_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_WARN, "pnd_mbx_get_next_msg_for_thread(): No Message Found! hThread(0x%X)", pThread->hThread );
    }

    if( (pThread->msg_queue_high.pFirst == LSA_NULL) &&
        (pThread->msg_queue_low.pFirst  == LSA_NULL) )
    {
        /* No More Messages in Queue => Wait */
        pnd_reset_event(pThread->blocking_wait);
    }
    else
    {
        /* More Messages in Queue => Retrigger */
        pnd_set_event(pThread->blocking_wait);
    }

    return pRQB;
}

static LSA_VOID pnd_mbx_post_msg(LSA_UINT32 hThread, pnd_postfunction pFkt , struct pnd_pnio_header *rqb, LSA_UINT16 mbox_prio)
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb;
    PND_MBX_THREAD_PTR_TYPE pThread = LSA_NULL;

    PND_ASSERT(rqb != LSA_NULL);
    PND_ASSERT(pFkt != LSA_NULL);

    pThread = pnd_mbx_get_thread(hThread);
    PND_ASSERT(pThread != LSA_NULL);

    LSA_RQB_SET_REQ_FCT_PTR(pRQB, pFkt);

    pnd_enter_critical_section(pnd_enter_exit_id);

    switch(mbox_prio)
    {
        case PND_MBX_MSG_PRIO_LOW:
            pnd_mbx_enqueue((PND_MBX_PNIO_HEADER_PTR_TYPE)pRQB, &pThread->msg_queue_low);
            break;
        case PND_MBX_MSG_PRIO_HIGH:
            pnd_mbx_enqueue((PND_MBX_PNIO_HEADER_PTR_TYPE)pRQB, &pThread->msg_queue_high);
            break;
        default:
            PND_FATAL("Mailbox Prio unknown!");
    }

    pThread->stats.uTotalMsgCount++;
    pThread->stats.iActMsgInTransfer++;

    // PND_SYSTEM_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_LOW, "pnd_mbx_post_msg(): RQB(0x%8X) hThread(0x%X) pFkt(0x%8X) prio(%d) CompId(0x%x) Opc(0x%x) MsgInTransfer(%d)", rqb, pThread->hThread, pFkt, mbox_prio, LSA_RQB_GET_COMP_ID(pRQB), LSA_RQB_GET_OPCODE(pRQB), pThread->stats.iActMsgInTransfer);

    pnd_set_event(pThread->blocking_wait);

    pnd_exit_critical_section(pnd_enter_exit_id);
}

/*---------------------------------------------------------------------------*/

LSA_VOID pnd_mbx_sys_init(LSA_VOID)
{
    LSA_UINT16 i;
    LSA_UINT16 retVal;

    pnd_memset(&g_PndriverMbx, 0, sizeof(g_PndriverMbx));

    retVal = pnd_alloc_event(&g_PndriverMbx.blocking_wait);
    PND_ASSERT(retVal == LSA_RET_OK);

    pnd_alloc_critical_section(&pnd_enter_exit_id, LSA_FALSE);

    for (i = 1; i < PND_MAX_TASK_ID; i++)
    {
        retVal = pnd_alloc_event(&g_PndriverMbx.sThreadStore.sThreads[i].blocking_wait);
        PND_ASSERT(retVal == LSA_RET_OK);

        g_PndriverMbx.sThreadStore.sThreads[i].bUsed = LSA_TRUE;
        g_PndriverMbx.sThreadStore.sThreads[i].hThread = i;
    }

    // Prepare the MBX list (Mapping MBX to PNIO thread
    // Note: is done with coding, to prevent wrong assigments is case of MBX index changes

    // g_PndriverMbx.sMbx[PND_MBX_ID_USER].name_ptr            = "MBX_PNIO_USER"; // Reserved for User thread
    // g_PndriverMbx.sMbx[PND_MBX_ID_USER].mbox_prio             = 0;

    //g_PndriverMbx.sMbx[PND_MBX_ID_EDDI_NRT_ORG].name_ptr    = "IO_BASE_CORE_TASK";
    g_PndriverMbx.sMbx[PND_MBX_ID_IO_BASE_CORE].mbox_prio     = PND_MBX_MSG_PRIO_LOW;
    g_PndriverMbx.sMbx[PND_MBX_ID_IO_BASE_CORE].pnd_threadId  = PND_IO_BASE_CORE_TASK_ID;

    //g_PndriverMbx.sMbx[PND_MBX_ID_GSY].name_ptr             = "PNIO_USER_CORE_TASK";
    g_PndriverMbx.sMbx[PND_MBX_ID_PNIO_USER_CORE].mbox_prio              = PND_MBX_MSG_PRIO_LOW;
    g_PndriverMbx.sMbx[PND_MBX_ID_PNIO_USER_CORE].pnd_threadId      = PND_PNIO_USER_CORE_TASK;

    

    g_PndriverMbx.bInitDone = LSA_TRUE;

    // PND_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "pnd_mbox_sys_init(): Init Done (+)" );
}

LSA_VOID pnd_mbx_sys_undo_init(LSA_VOID)
{
    LSA_UINT16 i;
    LSA_UINT16 retVal;

    for (i = 1; i < PND_MAX_TASK_ID; i++)
    {
        g_PndriverMbx.sThreadStore.sThreads[i].bQuitThread = LSA_TRUE;
        pnd_set_event(g_PndriverMbx.sThreadStore.sThreads[i].blocking_wait);
        
        pnd_wait_event(g_PndriverMbx.blocking_wait);
        pnd_reset_event(g_PndriverMbx.blocking_wait);

        retVal = pnd_free_event(g_PndriverMbx.sThreadStore.sThreads[i].blocking_wait);
        PND_ASSERT(retVal == LSA_RET_OK);

        PND_ASSERT(g_PndriverMbx.sThreadStore.sThreads[i].bUsed == LSA_FALSE);
    }

    pnd_free_critical_section(pnd_enter_exit_id);

    retVal = pnd_free_event(g_PndriverMbx.blocking_wait);
    PND_ASSERT(retVal == LSA_RET_OK);

    g_PndriverMbx.bInitDone = LSA_FALSE;

    // PND_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "pnd_mbx_sys_undo_init(): Undo Init Done (+)" );
}

LSA_UINT16  pnd_get_message(LSA_VOID (**fkt)(void *), struct pnd_pnio_header **rqb )
{
    PND_MBX_PNIO_HEADER_PTR_TYPE pRQB;

    PND_MBX_THREAD_PTR_TYPE pThread;
    LSA_UINT32 uThreadId;

    uThreadId = pnd_threads_get_current_id();
    pThread = pnd_mbx_get_thread(uThreadId);

    PND_ASSERT(pThread != LSA_NULL);

    pnd_wait_event(pThread->blocking_wait);

    if (pThread->bQuitThread == LSA_TRUE)
    {
        pThread->bUsed = LSA_FALSE;
        pnd_set_event(g_PndriverMbx.blocking_wait);

        return PND_QUIT_MSG;
    }

    pnd_enter_critical_section(pnd_enter_exit_id);

    pRQB = (PND_MBX_PNIO_HEADER_PTR_TYPE)pnd_mbx_get_next_msg_for_thread(pThread);

    pnd_exit_critical_section(pnd_enter_exit_id);	

    if(pRQB == LSA_NULL)
    {
        // Noting to find in Any Queue ->wrong sequenze
        // befor pnd_get_message make pnd_post_message
        return LSA_RET_ERR_SEQUENCE;
    }

    *fkt = LSA_RQB_GET_REQ_FCT_PTR(pRQB);
    *rqb = (struct pnd_pnio_header *)pRQB;

    LSA_RQB_SET_REQ_FCT_PTR(pRQB, LSA_NULL);

    return LSA_RET_OK;
}

LSA_VOID pnd_request_start( // Common request function for all PNIO components (not the PNIO user)
    LSA_UINT16          mbx_id,          // MBX Id of receiver
    PND_REQUEST_FCT     request_func,    // request function (open, close, request, system)
    LSA_VOID_PTR_TYPE   rb )             // RQB
{
    struct pnd_header *pRQB = (struct pnd_header *)rb;

    LSA_UINT32 curId = pnd_threads_get_current_id(); // current context 

    if ( PND_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
    {
        /* a thread called pnd_fatal(), no further processing */
        return;
    }

    PND_ASSERT( ( mbx_id > 0 ) && ( mbx_id < PND_MAX_MBX_ID )); // 0 (user) not allowed
    PND_ASSERT( request_func != LSA_NULL );

    if ( curId == g_PndriverMbx.sMbx[mbx_id].pnd_threadId ) // same task context, for direct call ?
    {
        (request_func)(pRQB);
    } 
    else 
    {
        // differnt context --> post message
        // Note: in case of error this funtion does not finish
        pnd_mbx_post_msg( g_PndriverMbx.sMbx[mbx_id].pnd_threadId, (pnd_postfunction) request_func, (struct pnd_pnio_header *)(pRQB), g_PndriverMbx.sMbx[mbx_id].mbox_prio );
    }
}

LSA_VOID pnd_request_local(  // Common request local function for all PNIO components (not the PNIO user)
    LSA_UINT16            mbx_id,          // MBX Id of receiver
    PND_REQUEST_FCT       request_func,    // request function (open, close, request, system)
    LSA_VOID_PTR_TYPE     rb           )   // RQB
{
    struct pnd_header *pRQB = (struct pnd_header *)rb;

    if ( PND_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
    {
        /* a thread called pnd_fatal(), no further processing */
        return;
    }

    PND_ASSERT( ( mbx_id > 0 ) && ( mbx_id < PND_MAX_MBX_ID)); // 0 (user) not allowed
    PND_ASSERT( request_func != 0 );

    // Always post the message (direct call not allowed, because same component)
    // Note: in case of error this funtion does not finish

    pnd_mbx_post_msg( g_PndriverMbx.sMbx[mbx_id].pnd_threadId, (pnd_postfunction) request_func, (struct pnd_pnio_header *)(pRQB), g_PndriverMbx.sMbx[mbx_id].mbox_prio );
}


LSA_UINT32 pnd_core_thread_proc(LSA_VOID_PTR_TYPE arg)
{
    int result      = LSA_RET_ERR_PARAM;
    LSA_INT32 ret  = -1;

    LSA_VOID (*fkt)(void*);
    struct pnd_pnio_header *rqb;

    fkt = LSA_NULL;
    rqb = LSA_NULL;

    pnd_threads_signal_ready(arg);

    while(LSA_TRUE)
    {
        /* Get message blocking */

        result = pnd_get_message(&fkt,&rqb);

        if ( PND_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
        {
            /* a thread called pcpnio_fatal(), no further processing */
            break;
        }
        else
        {
            if (result == LSA_RET_OK)
            {
                if ((fkt != LSA_NULL) && (rqb != LSA_NULL))
                {
                    ((void(*)(void*))(fkt))((void*)rqb);
                }
            }
            else if (result != LSA_RET_ERR_RESOURCE)
            {
                /* Message System Down or Quit (PND_QUIT_MSG) => Stop Thread */
                ret = 0;
                break;
            }
        }
    }

    pnd_threads_signal_stopped(arg);

    return ret; // QUIT (0 = OK; -1 = Failure)
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.    */
/*****************************************************************************/
