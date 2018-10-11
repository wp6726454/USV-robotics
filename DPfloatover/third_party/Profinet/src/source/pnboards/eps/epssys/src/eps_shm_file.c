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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_shm_file.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Interface Adaption                                       */
/*                                                                           */
/*****************************************************************************/
#include <eps_sys.h>         /* Types / Prototypes / Fucns        */ 
#include <eps_trc.h>         /* Tracing                           */
#include <eps_locks.h>       /* EPS Locks                         */
#include <eps_shm_file.h>    /* EPS Shared Mem File handling      */
#include <eps_mem.h>         /* EPS Local Mem Alloc / Free        */
#include <eps_tasks.h>       /* Thread Sleep                      */

#define LTRC_ACT_MODUL_ID 20039

#define EPS_SHM_FILE_MAX_FILES 5

#ifndef EPS_SUPPRESS_PRINTF
//#define EPS_SHM_FILE_PRINTF
#define EPS_SHM_FILE_PRINTF_ERR
#endif


typedef struct
{
    LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_SHM_FILE_TRANSFER_PTR_TYPE pFileTransfer;
} EPS_SHM_FILE_GLOB_TYPE, *EPS_SHM_FILE_GLOB_PTR_TYPE;

EPS_SHM_FILE_GLOB_PTR_TYPE g_pEpsShmFile = LSA_NULL;
EPS_SHM_FILE_GLOB_TYPE g_EpsShmFile = { 0 };


static LSA_VOID eps_shm_file_undo_init_critical_section(LSA_VOID)
{
	LSA_RESPONSE_TYPE retVal;
	retVal = eps_free_critical_section(g_pEpsShmFile->hEnterExit);
	EPS_ASSERT(LSA_RET_OK == retVal);
}

static LSA_VOID eps_shm_file_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsShmFile->hEnterExit, LSA_FALSE);
}

static LSA_VOID eps_shm_file_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsShmFile->hEnterExit);
}

static LSA_VOID eps_shm_file_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsShmFile->hEnterExit);
}

LSA_VOID eps_shm_file_free(EPS_APP_FILE_TAG_PTR_TYPE pFile)
{
    EPS_ASSERT(pFile != LSA_NULL);

	if(pFile->pFileData != LSA_NULL)
	{
		eps_mem_free(pFile->pFileData, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	}

	pFile->pFileData = LSA_NULL;
    pFile->uFileSize = 0;
    pFile->isHD	     = 0;
    pFile->FileType  = EPS_APP_FILE_INVALID;

    #if defined (EPS_CFG_USE_PNDEVDRV)
    pFile->eBoard    = ePNDEV_BOARD_INVALID;
    #endif
}


LSA_VOID eps_shm_file_init(LSA_UINT8* pBase)
{
	EPS_ASSERT(g_pEpsShmFile == LSA_NULL);
	EPS_ASSERT(pBase != LSA_NULL);

	g_pEpsShmFile = &g_EpsShmFile;
	EPS_ASSERT(g_pEpsShmFile->bInit == LSA_FALSE);

	eps_memset(g_pEpsShmFile, 0, sizeof(*g_pEpsShmFile));

	eps_shm_file_init_critical_section();

	eps_shm_file_enter();

	g_pEpsShmFile->pFileTransfer = (EPS_SHM_FILE_TRANSFER_PTR_TYPE)pBase;
	g_pEpsShmFile->pFileTransfer->req.bReqStart = 0;
	g_pEpsShmFile->pFileTransfer->req.uReqPacketRead = 0;

	g_pEpsShmFile->bInit = LSA_TRUE;
	    
	eps_shm_file_exit();
}

/** undo initialization of shared memory file
 * 
 *  if no file is in use any more, the global variable g_pEpsShmFile->bInit is set to LSA_FALSE.
 */
LSA_VOID eps_shm_file_undo_init(LSA_VOID)
{
	EPS_ASSERT(g_pEpsShmFile != LSA_NULL);

	eps_shm_file_undo_init_critical_section();
	g_pEpsShmFile->bInit = LSA_FALSE;
	g_pEpsShmFile = LSA_NULL;
}


LSA_VOID eps_shm_file_alloc(EPS_APP_FILE_TAG_PTR_TYPE pFile)
{
	LSA_UINT32 r = 0;
	LSA_UINT32 uActOffset = 0;
	LSA_BOOL bDone = LSA_FALSE;
	LSA_BOOL bFirstPacket = LSA_TRUE;
	volatile EPS_SHM_FILE_TRANSFER_PTR_TYPE pFT;
	
	EPS_ASSERT(g_pEpsShmFile != LSA_NULL);
	EPS_ASSERT(g_pEpsShmFile->bInit == LSA_TRUE);

	eps_shm_file_enter();

	pFT = g_pEpsShmFile->pFileTransfer;

    pFT->req.isHD = pFile->isHD;

    #if defined (EPS_CFG_USE_PNDEVDRV)
    pFT->req.eBoard = pFile->eBoard;
    #endif
    pFT->req.FileType = pFile->FileType;

#ifdef EPS_SHM_FILE_PRINTF
	printf("\r\nEPS SHM FILE requesting File Transfer");
	printf("\r\nTransfer (");
#endif

	pFT->req.uReqPacketRead = r;
	pFT->rsp.bRspDone = 0;
	pFT->req.bReqStart = 1;
	pFT->rsp.bTransferDone = 0;
	pFT->rsp.bTransferDoneCnf = 0;
	
	while(!bDone)
	{
		if(pFT->rsp.bRspDone == 1)
		{
			if(pFT->rsp.bRspErr == 1)
			{
				//EPS_SYSTEM_TRACE_STRING( 0, LSA_TRACE_LEVEL_ERROR, "EPS SHM FILE Transfer Failed - ");

				#ifdef EPS_SHM_FILE_PRINTF
				printf("-)");
				printf("\r\nTransfer FAILED!");
				#endif
				#ifdef EPS_SHM_FILE_PRINTF_ERR
				printf("\r\nEPS SHM FILE (%s) Transfer Failed\r\n");
				#endif

				eps_shm_file_free(pFile);
				break;
			}
			else
			{
				if(bFirstPacket == LSA_TRUE)
				{
					pFile->uFileSize = pFT->rsp.uFileSize;
					pFile->pFileData = (LSA_UINT8*)eps_mem_alloc(pFile->uFileSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
					if(pFile->pFileData == LSA_NULL)
					{
						eps_shm_file_free(pFile);
						break;
					}

					EPS_ASSERT( pFT->rsp.uActPacket == r );
					EPS_ASSERT( (uActOffset + pFT->rsp.uActPacketSize) <= pFile->uFileSize );
					eps_memcpy((LSA_VOID*)(pFile->pFileData + uActOffset), (LSA_VOID*)pFT->rsp.uData, pFT->rsp.uActPacketSize);
					uActOffset += pFT->rsp.uActPacketSize;
	
					bFirstPacket = LSA_FALSE;
				}
				else
				{
					EPS_ASSERT( pFT->rsp.uActPacket == r );
					EPS_ASSERT( (uActOffset + pFT->rsp.uActPacketSize) <= pFile->uFileSize );
					eps_memcpy((LSA_VOID*)(pFile->pFileData + uActOffset), (LSA_VOID*)pFT->rsp.uData, pFT->rsp.uActPacketSize);
					uActOffset += pFT->rsp.uActPacketSize;
				}

				#ifdef EPS_SHM_FILE_PRINTF
				printf("+");
				#endif

				if(uActOffset >= pFile->uFileSize)
				{
					bDone = LSA_TRUE;
				}
				else
				{
					r++;
					pFT->req.uReqPacketRead = r;
					pFT->rsp.bRspDone = 0;
					pFT->req.bReqStart = 1;
				}
			}
		}
		else
		{
			eps_tasks_sleep(1);
		}
	}

	pFT->rsp.bRspDone  = 0;
	pFT->req.bReqStart = 0;
	pFT->rsp.bTransferDone = 1;
	
	//wait for transfer done confirmation
	while (pFT->rsp.bTransferDoneCnf == 0)
	{
	    //do nothing
	    eps_tasks_sleep(1);
	}

	
	#ifdef EPS_SHM_FILE_PRINTF
	if(pFile != LSA_NULL)
	{
		printf(")");
		printf("\r\nTransfer Success (0x%X Bytes) Handle(%d)!", (unsigned int)uActOffset, (int)pFile->nHandle);
	}
	#endif

	eps_shm_file_exit();
}
