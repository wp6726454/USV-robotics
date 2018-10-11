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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_mbx.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements mailboxes for LSA-component PSI.                              */
/*                                                                           */
/*****************************************************************************/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#define LTRC_ACT_MODUL_ID  1
#define PSI_MODULE_ID      LTRC_ACT_MODUL_ID

#include "psi_inc.h"
#include "psi_int.h"
#include "psi_sys.h"
#include "psi_usr.h"

#define PSI_MSG_BOX_PRIOS 0

#define PSI_QUIT_MSG 0x42

/*---------------------------------------------------------------------------*/

enum PSI_MBX_MSG_PRIO
{
#if PSI_MSG_BOX_PRIOS == 1
    PSI_MBX_MSG_PRIO_HIGH = 1,
#endif
	PSI_MBX_MSG_PRIO_LOW  = 2
};

typedef struct psi_mbx_pnio_header_tag {
	LSA_RQB_HEADER(struct psi_pnio_header*)
} PSI_MBX_PNIO_HEADER_TYPE, *PSI_MBX_PNIO_HEADER_PTR_TYPE;

typedef struct psi_mbx_msg_queue_tag
{
	PSI_MBX_PNIO_HEADER_PTR_TYPE  pFirst;
} PSI_MBX_MSG_QUEUE_TYPE, *PSI_MBX_MSG_QUEUE_PTR_TYPE;

typedef struct psi_mbx_thread_tag
{
	LSA_UINT32              blocking_wait;
	PSI_MBX_MSG_QUEUE_TYPE  msg_queue_low;
#if PSI_MSG_BOX_PRIOS == 1
	PSI_MBX_MSG_QUEUE_TYPE  msg_queue_high;
	LSA_UINT32              uMsgQueueHighProcessedCnt;
#endif
	LSA_BOOL                bUsed;
	LSA_BOOL                bQuitThread;
	LSA_UINT32              hThread;
	struct
	{
		LSA_INT             iActMsgInTransfer;
		LSA_UINT32          uTotalMsgCount;
	} stats;
} PSI_MBX_THREAD_TYPE, *PSI_MBX_THREAD_PTR_TYPE;

typedef struct psi_mbx_thread_store_tag
{
	PSI_MBX_THREAD_TYPE sThreads[PSI_MAX_TASK_ID];
} PSI_MBX_THREAD_STORE_TYPE, *PSI_MBX_THREAD_STORE_PTR_TYPE;

typedef struct psi_mbx_store_tag{
	LSA_BOOL                     bInitDone;
	LSA_UINT32                   blocking_wait;
	PSI_MBX_THREAD_STORE_TYPE    sThreadStore;
} PSI_MBX_STORE_TAG, *PSI_MBX_STORE_PTR_TYPE;

/*---------------------------------------------------------------------------*/

static PSI_MBX_STORE_TAG g_PsiMbx;

/*---------------------------------------------------------------------------*/

static LSA_VOID psi_mbx_enqueue(PSI_MBX_PNIO_HEADER_PTR_TYPE pReq, PSI_MBX_MSG_QUEUE_PTR_TYPE pQueue)
{
	PSI_RQB_PTR_TYPE pFirst;
	PSI_RQB_PTR_TYPE pPrev;
	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)pReq;

	PSI_ASSERT(pQueue != LSA_NULL);
	PSI_ASSERT(pRQB != LSA_NULL);

	if(pQueue->pFirst == LSA_NULL)
	{
		pQueue->pFirst = pReq;
		LSA_RQB_SET_PREV_RQB_PTR(pRQB, pRQB);
		LSA_RQB_SET_NEXT_RQB_PTR(pRQB, pRQB);
	}
	else
	{
		pFirst = (PSI_RQB_PTR_TYPE)pQueue->pFirst;
		pPrev  = LSA_RQB_GET_PREV_RQB_PTR(pFirst);

		LSA_RQB_SET_PREV_RQB_PTR(pRQB, pPrev);
		LSA_RQB_SET_NEXT_RQB_PTR(pRQB, pFirst);
		LSA_RQB_SET_NEXT_RQB_PTR(pPrev,  pRQB);
		LSA_RQB_SET_PREV_RQB_PTR(pFirst, pRQB);
	}
}

static PSI_RQB_PTR_TYPE psi_mbx_dequeue(PSI_MBX_MSG_QUEUE_PTR_TYPE pQueue)
{
	PSI_RQB_PTR_TYPE pFirst = LSA_NULL;
	PSI_RQB_PTR_TYPE pNewFirst;

	PSI_ASSERT(pQueue != LSA_NULL);

	if(pQueue->pFirst == LSA_NULL) /* Queue Empty */
	{
		return LSA_NULL;
	}
	else
	{
		pFirst = (PSI_RQB_PTR_TYPE)pQueue->pFirst;

		if(LSA_RQB_GET_NEXT_RQB_PTR(pFirst) == pFirst) /* Last Elem */
		{
			pQueue->pFirst = LSA_NULL;
		}
		else /* Further Elements */
		{
			pNewFirst = LSA_RQB_GET_NEXT_RQB_PTR(pFirst);

			LSA_RQB_SET_PREV_RQB_PTR(pNewFirst, LSA_RQB_GET_PREV_RQB_PTR(pFirst));
			LSA_RQB_SET_NEXT_RQB_PTR(LSA_RQB_GET_PREV_RQB_PTR(pFirst), pNewFirst);

			pQueue->pFirst = (PSI_MBX_PNIO_HEADER_PTR_TYPE)pNewFirst;
		}
	}

	LSA_RQB_SET_PREV_RQB_PTR(pFirst, LSA_NULL);
	LSA_RQB_SET_NEXT_RQB_PTR(pFirst, LSA_NULL);

	return pFirst;
}

static PSI_MBX_THREAD_PTR_TYPE psi_mbx_get_thread(LSA_UINT32 hThread)
{
	PSI_MBX_THREAD_PTR_TYPE pThread;

	PSI_ASSERT(hThread < PSI_MAX_TASK_ID);

	pThread = &g_PsiMbx.sThreadStore.sThreads[hThread];

	PSI_ASSERT(pThread->bUsed == LSA_TRUE);

	return pThread;
}

static LSA_VOID* psi_mbx_get_next_msg_for_thread(PSI_MBX_THREAD_PTR_TYPE pThread)
{
	PSI_RQB_PTR_TYPE pRQB = LSA_NULL;

#if PSI_MSG_BOX_PRIOS == 1
	if(pThread->uMsgQueueHighProcessedCnt == 2)
	{
		pThread->uMsgQueueHighProcessedCnt = 0;

		/* Check for Low Message as we have processed a couple of highs previously */
		pRQB = psi_mbx_dequeue(&pThread->msg_queue_low);
	}

	if(pRQB == LSA_NULL)
	{
		/* Check for High Message */
		pRQB = psi_mbx_dequeue(&pThread->msg_queue_high);

		if(pRQB != LSA_NULL)
		{
			pThread->uMsgQueueHighProcessedCnt++;
		}
		else
		{
			pThread->uMsgQueueHighProcessedCnt = 0;

			/* Check for Low Message */
			pRQB = psi_mbx_dequeue(&pThread->msg_queue_low);
		}
	}
#else
	/* Check for Low Message */
	pRQB = psi_mbx_dequeue(&pThread->msg_queue_low);
#endif

	if(pRQB != LSA_NULL)
	{
		pThread->stats.iActMsgInTransfer--;

		PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_mbx_get_next_msg_for_thread(): RQB(0x%08X) hThread(0x%X) CompId(0x%x) Opc(0x%x) MsgInTransfer(%d)", pRQB, pThread->hThread, LSA_RQB_GET_COMP_ID(pRQB), LSA_RQB_GET_OPCODE(pRQB), pThread->stats.iActMsgInTransfer );
	}
	else
	{
		PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_WARN, "psi_mbx_get_next_msg_for_thread(): No Message Found! hThread(0x%X)", pThread->hThread );
	}

#if PSI_MSG_BOX_PRIOS == 1
	if( (pThread->msg_queue_high.pFirst == LSA_NULL) &&
		(pThread->msg_queue_low.pFirst  == LSA_NULL) )
#else
	if( pThread->msg_queue_low.pFirst  == LSA_NULL )
#endif
	{
		/* No More Messages in Queue => Wait */
		PSI_SET_BLOCKING_WAIT(&pThread->blocking_wait);
	}
	else
	{
		/* More Messages in Queue => Retrigger */
		PSI_RESET_BLOCKING_WAIT(&pThread->blocking_wait);
	}

	return pRQB;
}

static LSA_VOID psi_mbx_post_msg(LSA_UINT32 hThread, psi_postfunction pFkt , struct psi_pnio_header *rqb, LSA_UINT16 mbox_prio)
{
	PSI_RQB_PTR_TYPE pRQB = (PSI_RQB_PTR_TYPE)rqb;
	PSI_MBX_THREAD_PTR_TYPE pThread = LSA_NULL;

	PSI_ASSERT(rqb != LSA_NULL);
	PSI_ASSERT(pFkt != LSA_NULL);

	pThread = psi_mbx_get_thread(hThread);
	PSI_ASSERT(pThread != LSA_NULL);

	LSA_RQB_SET_REQ_FCT_PTR(pRQB, pFkt);

	PSI_ENTER();

#if PSI_MSG_BOX_PRIOS == 1
	switch(mbox_prio)
	{
		case PSI_MBX_MSG_PRIO_LOW:
			psi_mbx_enqueue((PSI_MBX_PNIO_HEADER_PTR_TYPE)pRQB, &pThread->msg_queue_low);
			break;
		case PSI_MBX_MSG_PRIO_HIGH:
			psi_mbx_enqueue((PSI_MBX_PNIO_HEADER_PTR_TYPE)pRQB, &pThread->msg_queue_high);
			break;
		default:
			PSI_FATAL(0);
	}
#else
	LSA_UNUSED_ARG(mbox_prio);
	psi_mbx_enqueue((PSI_MBX_PNIO_HEADER_PTR_TYPE)pRQB, &pThread->msg_queue_low);
#endif

	pThread->stats.uTotalMsgCount++;
	pThread->stats.iActMsgInTransfer++;

	PSI_SYSTEM_TRACE_07( 0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_mbx_post_msg(): RQB(0x%08X) hThread(0x%X) pFkt(0x%08X) prio(%d) CompId(0x%x) Opc(0x%x) MsgInTransfer(%d)", rqb, pThread->hThread, pFkt, mbox_prio, LSA_RQB_GET_COMP_ID(pRQB), LSA_RQB_GET_OPCODE(pRQB), pThread->stats.iActMsgInTransfer);

	PSI_RESET_BLOCKING_WAIT(&pThread->blocking_wait);

	PSI_EXIT();
}

static LSA_UINT16 psi_get_message_for_thread(LSA_VOID (**fkt)(void *), struct psi_pnio_header **rqb, PSI_MBX_THREAD_PTR_TYPE pThread )
{
	PSI_MBX_PNIO_HEADER_PTR_TYPE pRQB;

	PSI_WAIT_BLOCKING_WAIT(&pThread->blocking_wait);

	if (pThread->bQuitThread == LSA_TRUE)
	{
		pThread->bUsed = LSA_FALSE;
		PSI_RESET_BLOCKING_WAIT(&g_PsiMbx.blocking_wait);

		return PSI_QUIT_MSG;
	 }

	PSI_ENTER();

	pRQB = (PSI_MBX_PNIO_HEADER_PTR_TYPE)psi_mbx_get_next_msg_for_thread(pThread);

	PSI_EXIT();	

	if(pRQB == LSA_NULL)
	{
		// Noting to find in Any Queue ->wrong sequenze
		// before psi_get_message make psi_post_message
		return LSA_RET_ERR_SEQUENCE;
	}

	*fkt = LSA_RQB_GET_REQ_FCT_PTR(pRQB);
	*rqb = (struct psi_pnio_header *)pRQB;

	LSA_RQB_SET_REQ_FCT_PTR(pRQB, LSA_NULL);

	return LSA_RET_OK;
}

/*---------------------------------------------------------------------------*/

LSA_VOID psi_mbx_sys_init(LSA_VOID)
{
	LSA_UINT16 i;
	LSA_UINT16 retVal;

	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_mbx_sys_init()" );

    PSI_MEMSET(&g_PsiMbx, 0, sizeof(g_PsiMbx));

	PSI_ALLOC_BLOCKING_WAIT(&retVal, &g_PsiMbx.blocking_wait);
	PSI_ASSERT(retVal == LSA_RET_OK);

	for (i = 0; i < PSI_MAX_TASK_ID; i++)
	{
		PSI_ALLOC_BLOCKING_WAIT(&retVal, &g_PsiMbx.sThreadStore.sThreads[i].blocking_wait);
		PSI_ASSERT(retVal == LSA_RET_OK);

		g_PsiMbx.sThreadStore.sThreads[i].bUsed = LSA_TRUE;
		g_PsiMbx.sThreadStore.sThreads[i].hThread = i;
	}

	g_PsiMbx.bInitDone = LSA_TRUE;
}

LSA_VOID psi_mbx_sys_undo_init(LSA_VOID)
{
	LSA_UINT16 i;
	LSA_UINT16 retVal;

	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_mbx_sys_undo_init()" );

    for (i = 0; i < PSI_MAX_TASK_ID; i++)
	{
		g_PsiMbx.sThreadStore.sThreads[i].bQuitThread = LSA_TRUE;
		PSI_RESET_BLOCKING_WAIT(&g_PsiMbx.sThreadStore.sThreads[i].blocking_wait);
		
		PSI_WAIT_BLOCKING_WAIT(&g_PsiMbx.blocking_wait);
		PSI_SET_BLOCKING_WAIT(&g_PsiMbx.blocking_wait);

		PSI_FREE_BLOCKING_WAIT(&retVal, &g_PsiMbx.sThreadStore.sThreads[i].blocking_wait);
		PSI_ASSERT(retVal == LSA_RET_OK);

		PSI_ASSERT(g_PsiMbx.sThreadStore.sThreads[i].bUsed == LSA_FALSE);
	}

	PSI_FREE_BLOCKING_WAIT(&retVal, &g_PsiMbx.blocking_wait);
	PSI_ASSERT(retVal == LSA_RET_OK);

	g_PsiMbx.bInitDone = LSA_FALSE;
}

LSA_VOID psi_request_direct_start( // Common request function for all PNIO components (not the PNIO user)
	LSA_UINT16          mbx_id,          // MBX Id of receiver
	PSI_REQUEST_FCT     request_func,    // request function (open, close, request, system)
	LSA_VOID_PTR_TYPE   rb )             // RQB
{
	struct psi_header *pRQB = (struct psi_header *)rb;

	if ( PSI_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
	{
		/* a thread called psi_fatal(), no further processing */
		return;
	 }

#if PSI_DEBUG
	PSI_ASSERT( mbx_id < PSI_MBX_ID_USER );// user not allowed
#else
	LSA_UNUSED_ARG(mbx_id);
#endif

	PSI_ASSERT( request_func != LSA_NULL );

	//PSI_ASSERT ( PSI_GET_CURRENT_THREAD_ID() == g_PsiMbx.sMbx[mbx_id].psi_threadId ); // same task context, for direct call ?

	(request_func)(pRQB);
}

LSA_VOID psi_request_start( // Common request function for all PNIO components (not the PNIO user)
	LSA_UINT16          mbx_id,          // MBX Id of receiver
	PSI_REQUEST_FCT     request_func,    // request function (open, close, request, system)
	LSA_VOID_PTR_TYPE   rb )             // RQB
{
	struct psi_header *pRQB = (struct psi_header *)rb;

	if ( PSI_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
	{
		/* a thread called psi_fatal(), no further processing */
		return;
	}

	PSI_ASSERT( mbx_id < PSI_MBX_ID_USER );// user not allowed
	PSI_ASSERT( request_func != LSA_NULL );

		// differnt context --> post message
		// Note: in case of error this funtion does not finish
	psi_mbx_post_msg( mbx_id, (psi_postfunction) request_func, (struct psi_pnio_header *)(pRQB), PSI_MBX_MSG_PRIO_LOW );
}

LSA_VOID psi_request_local(  // Common request local function for all PNIO components (not the PNIO user)
	LSA_UINT16            mbx_id,          // MBX Id of receiver
	PSI_REQUEST_FCT       request_func,    // request function (open, close, request, system)
	LSA_VOID_PTR_TYPE     rb           )   // RQB
{
	struct psi_header *pRQB = (struct psi_header *)rb;

	if ( PSI_FATAL_ERROR_OCCURED()  ) // request (posting) allowed ?
	{
		/* a thread called psi_fatal(), no further processing */
		return;
	}
	PSI_ASSERT( mbx_id < PSI_MBX_ID_USER );//(user) not allowed
	PSI_ASSERT( request_func != 0 );

	// Always post the message (direct call not allowed, because same component)
	// Note: in case of error this funtion does not finish

	psi_mbx_post_msg( mbx_id, (psi_postfunction) request_func, (struct psi_pnio_header *)(pRQB), PSI_MBX_MSG_PRIO_LOW );
}

LSA_VOID psi_system_request_done( // Common system request callback function
	LSA_UINT16            mbx_id,          // MBX Id of receiver
	PSI_REQUEST_FCT       request_func,    // request function (open, close, request, system)
	LSA_VOID_PTR_TYPE     rb           )   // RQB
{
	// Note: impl. for new PSI MBX interface
	//       we use the request local function to prevent direct calls for this respone
	psi_request_local( mbx_id, request_func, rb );
}

LSA_VOID psi_request_done( // Common callback function for all PNIO components (not the PNIO user)
	PSI_REQUEST_FCT     request_done_func,
	LSA_VOID_PTR_TYPE   rb,
	LSA_SYS_PTR_TYPE    sys_ptr )
{
	struct psi_header *pRQB = (struct psi_header *)rb;
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT( pSys != 0 );

	if ( PSI_FATAL_ERROR_OCCURED() ) // posting allowed ?
	{
		/* a thread called psi_fatal(), no further processing */
		return;
	}

	PSI_ASSERT( request_done_func != 0  );
	PSI_ASSERT( pSys->rsp_mbx_id < PSI_MBX_ID_MAX );

	if ( pSys->rsp_mbx_id == PSI_MBX_ID_USER ) // PNIO User ?
	{
		PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
			"psi_request_done(): Delegate user request upper message, MBX id(%u) RQB(0x%08x)",
			pSys->rsp_mbx_id,
			pRQB );

		// Delegate the posting to User (to prevent duplicate posting)
		// Note: posting is done in registered request_done_func
		(request_done_func)(pRQB);
	}
	else
	{
		// always post response message to requestor (never call it direct)
		// Note: in case of error this funtion does not finish

		psi_mbx_post_msg( pSys->rsp_mbx_id, (psi_postfunction)request_done_func , (struct psi_pnio_header *)(pRQB), PSI_MBX_MSG_PRIO_LOW );
	}
}

LSA_INT32 psi_thread_proc(LSA_VOID_PTR_TYPE arg, LSA_UINT32 psi_thread_id)
{
	LSA_UINT16 result = LSA_RET_ERR_PARAM;
	LSA_INT32 ret = -1;

	PSI_MBX_THREAD_PTR_TYPE pThread;

	LSA_VOID (*fkt)(LSA_VOID*);
	struct psi_pnio_header *rqb;

	fkt = LSA_NULL;
	rqb = LSA_NULL;

	PSI_THREAD_READY(arg);

	pThread = psi_mbx_get_thread(psi_thread_id);
	PSI_ASSERT(pThread != LSA_NULL);

	PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_thread_proc(): PSI Thread entering message loop (psi_thread_id=0x%x)", psi_thread_id );

	while(LSA_TRUE)
	{
		/* Get message blocking */

		result = psi_get_message_for_thread(&fkt,&rqb,pThread);

		if ( PSI_FATAL_ERROR_OCCURED() ) // request (posting) allowed ?
		{
			/* a thread called the fatal error handler, no further processing */
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
				/* Message System Down or Quit (PSI_QUIT_MSG) => Stop Thread */
				ret = 0;
				break;
			}
		}
	}

	PSI_THREAD_STOPPED(arg);

	PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_thread_proc(): PSI Thread exiting message loop (psi_thread_id=0x%x, result=0x%x)", psi_thread_id, result );

	return ret; // QUIT (0 = OK; -1 = Failure)
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
