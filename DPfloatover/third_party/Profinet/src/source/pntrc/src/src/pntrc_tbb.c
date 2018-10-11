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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_tbb.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*    TBB = Two Buff Binary                                                  */
/*  LSA-Trace                                                                */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-buffer_id                          */
/*===========================================================================*/

#define PNTRC_MODULE_ID   2
#define LTRC_ACT_MODUL_ID 2

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "pntrc_inc.h"
#include "pntrc_trc.h"
#include "pntrc_int.h"

/*===========================================================================*/
/*                           sys structs/ unions                             */
/*===========================================================================*/

/*===========================================================================*/
/*                                global data                                */
/*===========================================================================*/

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/


/*===========================================================================*/
/*                              user functions                               */
/*===========================================================================*/

#define PNTRC_LITTLE_ENDIAN 0

#define PNTRC_TIMESTAMP_UNIT_NS 0

LSA_VOID pntrc_ttb_init_buffer_header(LSA_UINT32 Offset, LSA_UINT32 Id, LSA_UINT32 Size, LSA_UINT32 FirstBlock, LSA_UINT32 MaxTraceBlocks)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE pHeader;

	pHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)((LSA_UINT8*)g_pPntrcData->TraceMem+Offset);

	pHeader->MagicNumber=TRACE_BUF_MAGICNUMBER;
	pHeader->ByteWidth=4;
	pHeader->FirstBlock=FirstBlock;
	pHeader->Id=Id;
	pHeader->MaxBlockCount=MaxTraceBlocks;
	pHeader->Options=PNTRC_LITTLE_ENDIAN;
	pHeader->Size=Size;
	pHeader->TimeStampUnit=PNTRC_TIMESTAMP_UNIT_NS;
	pHeader->TimeStampWidth=8;
	pHeader->CurBlockCount=0;
	pHeader->Flags=0;
	pHeader->Reserved1=0;
}

LSA_VOID pntrc_tbb_set_mifs_value(LSA_UINT32 ofs, LSA_UINT32 value)
{
	LSA_UINT8 i;

	*(LSA_UINT32*)((LSA_UINT8*)(&g_pPntrcData->TraceMem->MasterMIF)+ofs)=value;
	for (i=0; i<PNTRC_MAX_SLAVE_MIF; i++) {
		*(LSA_UINT32*)((LSA_UINT8*)(&g_pPntrcData->TraceMem->SlaveMif[i])+ofs)=value;
	}
}

LSA_VOID pntrc_tbb_init_category(int CPUNr, int Category, LSA_UINT32 Start, LSA_UINT32 Size)
{
    LSA_UINT32 MaxTraceBlocks = 0;  /* maximum number of trace blocks in the buffer */
    LSA_UINT32 UsedSize;
	LSA_UINT32 HalfSize;
	LSA_UINT32 Offset0,Offset1;
	LSA_UINT32 AdjustedSize;
	LSA_UINT32 FirstBlock;
	LSA_UINT32 End;

	Offset0=((Start+3)/4)*4; //round up to 32bit boundary
	End=Start+Size-1;
	AdjustedSize=End-Offset0+1;
	AdjustedSize=(AdjustedSize/8)*8; //round down, adjustedSize is now a multiple of 64 bit
	HalfSize = AdjustedSize / 2; // HalfSize is now a multiple of 32 bit
	Offset1 = Offset0 + HalfSize;

    /* calculate valid trace buffer size */
	FirstBlock=sizeof(PNTRC_BUFFER_HEADER_TYPE);
    MaxTraceBlocks = (HalfSize - sizeof(PNTRC_BUFFER_HEADER_TYPE) ) / sizeof (PNTRC_BUFFER_ENTRY_TYPE);
	UsedSize = sizeof(PNTRC_BUFFER_HEADER_TYPE) + sizeof(PNTRC_BUFFER_ENTRY_TYPE) * MaxTraceBlocks;

	//set values in MIF
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].OffsetBuffer[0]),Offset0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].SizeBuffer[0]),UsedSize);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].StateBuffer[0]),PNTRC_BUFFER_STATE_FILLING);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].OffsetBuffer[1]),Offset1);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].SizeBuffer[1]),UsedSize);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].StateBuffer[1]),PNTRC_BUFFER_STATE_EMPTY);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].CurBufferNr),0);

	pntrc_ttb_init_buffer_header(Offset0, ((g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU & 1) << 9) | Category, UsedSize, FirstBlock, MaxTraceBlocks);
	pntrc_ttb_init_buffer_header(Offset1, ((g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU & 1) << 9) | Category | 0x100, UsedSize, FirstBlock, MaxTraceBlocks);

	g_pPntrcData->FirstTraceEntry[Category][0].data.header.Subsys=0; //indicate that buffers have been never used
	g_pPntrcData->FirstTraceEntry[Category][1].data.header.Subsys=0; //indicate that buffers have been never used

}

PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category, LSA_UINT8 BufferNr)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
	LSA_UINT32 Offset;

	Offset=Mif->BuffersDesc[CPUNr][Category].OffsetBuffer[BufferNr];
	Ptr=(PNTRC_BUFFER_HEADER_PTR_TYPE)((LSA_UINT8*)(Mif) + Offset);
	return Ptr;
}

PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_cur_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
	Ptr=pntrc_get_buffer_header(Mif,CPUNr,Category,(LSA_UINT8)Mif->BuffersDesc[CPUNr][Category].CurBufferNr);
	return Ptr;
}

PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_other_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
	Ptr=pntrc_get_buffer_header(Mif,CPUNr,Category,(LSA_UINT8)(1-Mif->BuffersDesc[CPUNr][Category].CurBufferNr));
	return Ptr;
}

PNTRC_BUFFER_ENTRY_PTR_TYPE pntrc_get_cur_trace_block(LSA_UINT8 Category, LSA_UINT32 FirstBlock, LSA_UINT32 CurBlockCount)
{
	PNTRC_BUFFER_ENTRY_PTR_TYPE Ptr;
	Ptr=(PNTRC_BUFFER_ENTRY_PTR_TYPE)((LSA_UINT8*)pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category)+FirstBlock+CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE));
	return Ptr;
}

PNTRC_BUFFER_ENTRY_PTR_TYPE pntrc_get_cur_first_trace_entry_save(LSA_UINT8 Category)
{
	PNTRC_BUFFER_ENTRY_PTR_TYPE Ptr;
	Ptr=(PNTRC_BUFFER_ENTRY_PTR_TYPE)&g_pPntrcData->FirstTraceEntry[Category][g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].CurBufferNr];
	return Ptr;
}

LSA_VOID pntrc_tbb_send_sync_time(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev)
{
	PNTRC_MIF_TYPE Mif;
	LSA_UINT32 stamp_low,stamp_high;

	PNTRC_GET_TRACESTAMP(&stamp_low,&stamp_high);
	Mif.TimeSync.SyncTimeLow=stamp_low;
	Mif.TimeSync.SyncTimeHigh=stamp_high;
	Mif.TimeSync.NewSyncTime=1;
	PNTRC_WRITE_LOWER_CPU(hLowerDev,(LSA_UINT8*)(&Mif.TimeSync),PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,TimeSync),sizeof(Mif.TimeSync));
}

LSA_VOID pntrc_tbb_init(PNTRC_INIT_PTR_TYPE pInit)
{
	LSA_UINT32 i,j,k;
	LSA_UINT32 SizePerCategory;

#ifdef ZERO_TRACE_BUFFERS
	PNTRC_MEMSET_LOCAL_MEM(pInit->Ptr,0,pInit->Size);
#endif

	g_pPntrcData->TraceMem=(PNTRC_TRACE_MEM_PTR_TYPE)(pInit->Ptr);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,MagicNumber),TRACE_MEM_MAGICNUMBER);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Ready),0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,AnyBufferFull),0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,FatalOccurred),0);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.MajorVersion),1);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.MinorVersion),0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.Endiness),0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.Padding),0);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.MaxCategoryCount),PNTRC_MAX_CATEGORY_COUNT);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.MaxLowerCPUCount),PNTRC_CFG_MAX_LOWER_CPU_CNT);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.IsTopMostCPU),pInit->bIsTopMostCPU);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.TraceSubSysNum),TRACE_SUBSYS_NUM);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,ForceBufferSwitch),0);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,TimeSync.NewSyncTime),0);

	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),0);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewReadTraceLevels),0);

	g_pPntrcData->bufferSwitchInProgress=LSA_FALSE;

	SizePerCategory = (pInit->Size - (1+PNTRC_MAX_SLAVE_MIF)*sizeof(PNTRC_MIF_TYPE) )/ PNTRC_MAX_CATEGORY_COUNT/(1+PNTRC_CFG_MAX_LOWER_CPU_CNT);
	for (i=0; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		for (j=0; j<PNTRC_MAX_CATEGORY_COUNT; j++) {
			pntrc_tbb_init_category(i,j,(1+PNTRC_MAX_SLAVE_MIF)*sizeof(PNTRC_MIF_TYPE) + i*(SizePerCategory*PNTRC_MAX_CATEGORY_COUNT)+j*SizePerCategory,SizePerCategory);
		}
	}
	for (i=0; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		for (j=0; j<PNTRC_MAX_CATEGORY_COUNT; j++) {
			for (k=0; k<2; k++) {
				g_pPntrcData->pTraceBuffer[i][j][k]=0;
			}
		}
	}
	g_pPntrcData->UseInitLogLevels=pInit->bUseInitLogLevels;
}

LSA_VOID pntrc_tbb_uninit(LSA_VOID)
{
	LSA_UINT32 i,j,k;
	LSA_UINT16 rc;

	pntrc_tbb_save_all_buffers(LSA_FALSE);
	for (i=0; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		for (j=0; j<PNTRC_MAX_CATEGORY_COUNT; j++) {
			for (k=0; k<2; k++) {
				if (g_pPntrcData->pTraceBuffer[i][j][k] != 0) {
					PNTRC_FREE_LOCAL_MEM(&rc, g_pPntrcData->pTraceBuffer[i][j][k]);
					PNTRC_ASSERT(rc==LSA_RET_OK);
					g_pPntrcData->pTraceBuffer[i][j][k] = 0;
				}
			}
		}
	}
}

LSA_VOID pntrc_tbb_set_ready(LSA_VOID)
{
	PNTRC_ASSERT(g_pPntrcData->TraceMem != LSA_NULL);
	pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Ready),PNTRC_MIF_READY);
}

LSA_VOID pntrc_tbb_set_log_levels_lower_cpu(PNTRC_CPU_HANDLE_PTR_TYPE hLowerDev, LSA_UINT8* pLogLevels)
{
	PNTRC_MIF_TYPE Mif;
	LSA_UINT32 i;
	for(i=1; i<TRACE_SUBSYS_NUM; i++)
	{
		Mif.WriteTraceLevels[i] = pLogLevels[i];
	}
	Mif.NewWriteTraceLevels=1;
	PNTRC_WRITE_LOWER_CPU(hLowerDev->SysHandle,(LSA_UINT8*)(&Mif.NewWriteTraceLevels),PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),sizeof(Mif.NewWriteTraceLevels)+sizeof(Mif.WriteTraceLevels));
	hLowerDev->SetLogLevelPending=LSA_TRUE;
}

LSA_BOOL pntrc_lower_cpus_open(LSA_VOID)
{
	LSA_UINT8 i;
	for (i=0; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened == LSA_TRUE) {
			return LSA_TRUE;
		}
	}
	return LSA_FALSE;
}

LSA_VOID pntrc_tbb_set_log_levels(LSA_UINT8* pLogLevels, LSA_USER_ID_TYPE UserID, PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE SetLogLevelCB)
{
//	char* St="Hallo";
	LSA_UINT32 i;
	PNTRC_ASSERT(pLogLevels != LSA_NULL);

	g_pPntrcData->SetLogLevelCB=SetLogLevelCB;
	g_pPntrcData->SetLogLevelUserID=UserID;

	for(i=1; i<TRACE_SUBSYS_NUM; i++)
	{
		pntrc_current_level[i] = pLogLevels[i];
	}
	
	g_pPntrcData->SetLogLevel_pLogLevels=pntrc_current_level;
	
//	PNTRC_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_WARN, "String \"%s\"",St);
//	PNTRC_SYSTEM_TRACE_BYTE_ARRAY(0, LSA_TRACE_LEVEL_WARN, "Trace Mem",St,strlen(St));
//	PNTRC_SYSTEM_TRACE_BYTE_ARRAY(0, LSA_TRACE_LEVEL_WARN, "Trace Mem",St,0);
	PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting %d log levels",TRACE_SUBSYS_NUM);
	if (pntrc_lower_cpus_open()) {
		PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting %d log levels for lower cpus",TRACE_SUBSYS_NUM);
		g_pPntrcData->SetLogLevelPending=LSA_TRUE;
		for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
			if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
				pntrc_tbb_set_log_levels_lower_cpu(&g_pPntrcData->pntrc_lower_cpu_list[i],pLogLevels);
			}
		}
	} else {
		PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: No lower cpus open, calling CB immediately");
		if (SetLogLevelCB != 0) {
			PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: CB != 0, call it");
			(*SetLogLevelCB)(g_pPntrcData->RQBSetTraceLevels);
			g_pPntrcData->RQBSetTraceLevels=LSA_NULL;
		}
	}
}

LSA_VOID pntrc_tbb_get_log_levels(LSA_UINT8* pLogLevels)
{
	int i;

	for(i=1; i<TRACE_SUBSYS_NUM; i++)
	{
		pLogLevels[i] = pntrc_current_level[i];
	}
}

LSA_UINT32 pntrc_tbb_SendBufferSystemAdaption(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID, PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader)
{
	LSA_UINT32 UsedSize;
	if (g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU) {
		UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);
		if (UsedSize>0) {
			PNTRC_BUFFER_FULL(SysHandle, BufferID, (LSA_UINT8*)pBufferHeader, UsedSize);
		}
	}
	return LSA_RET_OK;
}

LSA_VOID pntrc_tbb_set_buffer_empty(LSA_VOID* SysHandle, LSA_UINT32 IsTopMostCPU, LSA_UINT32 CPUNr, LSA_UINT32 CategoryNr, LSA_UINT32 BufferNr)
{
	PNTRC_MIF_TYPE Mif;
	if (IsTopMostCPU) {
		pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[0][CategoryNr].StateBuffer[BufferNr]),PNTRC_BUFFER_STATE_EMPTY);
	} else {
		Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr] = PNTRC_BUFFER_STATE_EMPTY;
		PNTRC_WRITE_LOWER_CPU((PNTRC_SYS_HANDLE_LOWER_PTR_TYPE)SysHandle,
			(LSA_UINT8*)(&Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]),
			PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]),
			sizeof(Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]));
	}
}


LSA_VOID pntrc_tbb_copy_lower_buffer(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, PNTRC_MIF_PTR_TYPE Mif, LSA_UINT32 CPUNr, LSA_UINT32 CategoryNr, LSA_UINT32 BufferNr, LSA_BOOL FetchFatal)
{
	LSA_UINT32 Offset,Size;
	LSA_UINT8* pTraceBuffer;
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
	LSA_UINT32 UsedSize;
	LSA_UINT32 DestOffset;
	LSA_UINT8* pDest;
	LSA_UINT32 CPUId;

	Offset=Mif->BuffersDesc[CPUNr][CategoryNr].OffsetBuffer[BufferNr];
	Size=Mif->BuffersDesc[CPUNr][CategoryNr].SizeBuffer[BufferNr];

	if (g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU) {
		if (g_pPntrcData->pTraceBuffer[CPUNr][CategoryNr][BufferNr] == 0) {
			PNTRC_ALLOC_LOCAL_MEM((void**)&g_pPntrcData->pTraceBuffer[CPUNr][CategoryNr][BufferNr], Size);
			PNTRC_ASSERT(g_pPntrcData->pTraceBuffer[CPUNr][CategoryNr][BufferNr] != 0);
		}
		pTraceBuffer=g_pPntrcData->pTraceBuffer[CPUNr][CategoryNr][BufferNr];
		PNTRC_READ_LOWER_CPU(hLowerDev,Offset,Size,pTraceBuffer);
		pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)pTraceBuffer;
		CPUId=(LowerCPUNr*10) + (pBufferHeader->Id >> 10);
		pBufferHeader->Id=pBufferHeader->Id & (~0xfc00); /*Mask out the board nr*/
		pBufferHeader->Id=pBufferHeader->Id | (CPUId << 10);
		if (FetchFatal) {
			PNTRC_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, CPU %ld, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,CPUNr,CategoryNr,BufferNr);
		}
		pntrc_tbb_SendBufferSystemAdaption(hLowerDev,pBufferHeader->Id,pBufferHeader);
	} else {
	/* We are in the middle of two CPUs. Our lower CPU has number LowerCPUNr. As currently there are only three levels, our lower level has no underlying CPUs. Therefore there can only be trace data
	   from our lower CPU itself, which are the buffers with CPUNr=0. So CPUNr can only be 0 here. We have now to transfer these buffers to the middle CPU, where they shall be copied to the buffers
	   with CPUNr=LowerCPUNr.
	*/
		/*First check, if our destination buffer is empty and already fetched by the upper CPU*/
		if (g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[LowerCPUNr][CategoryNr].StateBuffer[BufferNr] != PNTRC_BUFFER_STATE_FULL) {
			if (g_pPntrcData->pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr] == 0) {
				PNTRC_ALLOC_LOCAL_MEM((void**)&g_pPntrcData->pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr], Size);
				PNTRC_ASSERT(g_pPntrcData->pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr] != 0);
			}
			pTraceBuffer=g_pPntrcData->pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr];
			PNTRC_READ_LOWER_CPU(hLowerDev,Offset,Size,pTraceBuffer);
			pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)pTraceBuffer;
			UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);
			if (UsedSize>0) {
				if (FetchFatal) {
					PNTRC_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, CPU %ld, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,CPUNr,CategoryNr,BufferNr);
				}
				DestOffset=g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[LowerCPUNr][CategoryNr].OffsetBuffer[BufferNr];
				pDest=(LSA_UINT8*)(&g_pPntrcData->TraceMem->MasterMIF) + DestOffset;
				/*copy to upper CPU shared memory*/
				PNTRC_MEMCPY_LOCAL_MEM(pDest, g_pPntrcData->pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr], UsedSize);
				pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)pDest;
				pBufferHeader->Id=pBufferHeader->Id | (LowerCPUNr << 10);
				pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[LowerCPUNr][CategoryNr].StateBuffer[BufferNr]),PNTRC_BUFFER_STATE_FULL);
				/*free the buffer of the lower CPU*/
				/*The lower (3d level) CPU has its buffers in CPUNr=0*/
				pntrc_tbb_set_buffer_empty(hLowerDev, g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU, CPUNr, CategoryNr, BufferNr);
			}
		}
	}
}

LSA_VOID pntrc_tbb_save_all_lower_buffers(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, PNTRC_MIF_PTR_TYPE pMif, LSA_BOOL FetchFatal)
{
	LSA_UINT32 i,j,k;
	PNTRC_MIF_TYPE Mif;

	if (pMif == LSA_NULL) {
		pMif=&Mif;
	}
	PNTRC_READ_LOWER_CPU(hLowerDev,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config),sizeof(pMif->Config)+sizeof(pMif->BuffersDesc),(LSA_UINT8*)(&pMif->Config));
	for (i=0; i< 1+pMif->Config.MaxLowerCPUCount; i++) {
		for (j=0; j<pMif->Config.MaxCategoryCount; j++) {
			for (k=0; k<2; k++) {
				if (pMif->BuffersDesc[i][j].StateBuffer[k] != PNTRC_BUFFER_STATE_EMPTY) {
					pntrc_tbb_copy_lower_buffer(hLowerDev,LowerCPUNr,pMif,i,j,k,FetchFatal);
				}
			}
		}
	}
}

LSA_VOID pntrc_tbb_service_lower_cpu(PNTRC_CPU_HANDLE_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, LSA_BOOL sendSyncTime)
{
	PNTRC_MIF_TYPE Mif;
	LSA_UINT32 i,j,k;

	PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,MagicNumber),sizeof(Mif.MagicNumber)+sizeof(Mif.Ready)+sizeof(Mif.AnyBufferFull)+sizeof(Mif.FatalOccurred),(LSA_UINT8*)(&Mif.MagicNumber));
	if (Mif.Ready != PNTRC_MIF_READY) return;
	if (Mif.FatalOccurred & !hLowerDev->FatalHandled) {
		hLowerDev->FatalHandled=LSA_TRUE;
		PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching fatal trace begin ***");
		pntrc_tbb_save_all_lower_buffers(hLowerDev->SysHandle,LowerCPUNr,&Mif,LSA_TRUE);
		PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching fatal trace end ***");
		return;
	}
	if (Mif.AnyBufferFull) {
		PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config),sizeof(Mif.Config)+sizeof(Mif.BuffersDesc),(LSA_UINT8*)(&Mif.Config));
		for (i=0; i < 1+Mif.Config.MaxLowerCPUCount; i++) {
			for (j=0; j < Mif.Config.MaxCategoryCount; j++) {
				for (k=0; k<2; k++) {
					if (Mif.BuffersDesc[i][j].StateBuffer[k] == PNTRC_BUFFER_STATE_FULL) {
						pntrc_tbb_copy_lower_buffer(hLowerDev->SysHandle,LowerCPUNr,&Mif,i,j,k,LSA_FALSE);
					}
				}
			}
		}
	}
	if (hLowerDev->InitLogLevelsSent == LSA_FALSE) {
		if (g_pPntrcData->SetLogLevel_pLogLevels != LSA_NULL) {  //log levels have been set at least once
			PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting initial log levels for lower CPU");
			pntrc_tbb_set_log_levels_lower_cpu(hLowerDev,pntrc_current_level);
		}
		hLowerDev->InitLogLevelsSent = LSA_TRUE;
	}
	if (sendSyncTime) {
		pntrc_tbb_send_sync_time(hLowerDev->SysHandle);
	}
	if (hLowerDev->SetLogLevelPending) {
		PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,
			PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),
			sizeof(Mif.NewWriteTraceLevels),
			(LSA_UINT8*)(&Mif.NewWriteTraceLevels));
		if (Mif.NewWriteTraceLevels == 0) {
			hLowerDev->SetLogLevelPending=LSA_FALSE;
		}
	}
}

LSA_VOID pntrc_tbb_service_lower_cpus(LSA_VOID)
{
	LSA_UINT8 i;
	LSA_BOOL sendSyncTime;
	LSA_BOOL anyLowerLogLevelPending;

	sendSyncTime=LSA_FALSE;
	g_pPntrcData->pntrc_SendSyncScaler++;
	if (g_pPntrcData->pntrc_SendSyncScaler == PNTRC_SEND_SYNC_SCALER) {
		sendSyncTime=LSA_TRUE;
		g_pPntrcData->pntrc_SendSyncScaler=0;
	}

	for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
			pntrc_tbb_service_lower_cpu(&g_pPntrcData->pntrc_lower_cpu_list[i],i,sendSyncTime);
		}
	}
	if (g_pPntrcData->SetLogLevelPending) {
		anyLowerLogLevelPending=LSA_FALSE;
		for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
			if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened && g_pPntrcData->pntrc_lower_cpu_list[i].SetLogLevelPending) {
				anyLowerLogLevelPending=LSA_TRUE;
			}
		}
		if (!anyLowerLogLevelPending) {
			if (g_pPntrcData->SetLogLevelCB != 0) {
				(*g_pPntrcData->SetLogLevelCB)(g_pPntrcData->RQBSetTraceLevels);
				g_pPntrcData->RQBSetTraceLevels=LSA_NULL;
			}
			g_pPntrcData->SetLogLevelPending=LSA_FALSE;
		}
	}
}

LSA_VOID pntrc_tbb_buffer_full_done(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID)
{
	LSA_UINT32 BufferNr;
	LSA_UINT32 Category;
	LSA_UINT32 IsTopMostCPU;
	LSA_UINT8 CPUId;
	LSA_UINT8 LowerCPUNr=0;

    BufferNr=(BufferID & 0x100)>>8;
	Category=BufferID & 0xFF;
	IsTopMostCPU=(BufferID & 0x200)>>9;
	CPUId=(BufferID & 0xFC00)>>10;
	if (CPUId > 10) {
		/* three CPU levels involved */
		LowerCPUNr=((BufferID & 0xFC00)>>10) % 10;
	}

	pntrc_tbb_set_buffer_empty(SysHandle,IsTopMostCPU,LowerCPUNr,Category,BufferNr);
}

/*=============================================================================
 * function name:  ltrc_tbb_undo_init
 *
 * function:       undo initialize binary memory
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID ltrc_tbb_undo_init (
    LSA_VOID)
{
}

LSA_VOID pntrc_tbb_WriteTraceHeader(PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock,
					  LTRC_SUBSYS_TYPE SubSys,
					  PNTRC_LEVEL_TYPE Level,
					  LSA_UINT32 TracepointRef,
					  LSA_UINT8 ParCount,
					  LSA_UINT32   TimeStampHigh, LSA_UINT32   TimeStampLow)
{
	pTraceBlock->data.header.Subsys = SubSys;
	pTraceBlock->data.header.Flags = Level;
	pTraceBlock->data.header.ParCnt = ParCount;
	pTraceBlock->data.classic.a1 = TracepointRef;

	pTraceBlock->data.header.TimestampLow = TimeStampLow;
	pTraceBlock->data.header.TimestampHigh = TimeStampHigh;
}

LSA_VOID pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount, LSA_UINT32 TimeStampHigh, LSA_UINT32 TimeStampLow)
{
	LSA_UINT32 CurBlockCount;
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
	PNTRC_BUFFER_ENTRY_PTR_TYPE pFirstTraceEntrySave;
	PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock;

	pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
	CurBlockCount=pBufferHeader->CurBlockCount;
	if (CurBlockCount > pBufferHeader->MaxBlockCount - 1) {
		CurBlockCount=CurBlockCount; //PNTRC_FATAL (0);
	}
	pTraceBlock=pntrc_get_cur_trace_block(Category,pBufferHeader->FirstBlock,CurBlockCount);

	pntrc_tbb_WriteTraceHeader(pTraceBlock,SubSys,Level,TracepointRef,ParCount,TimeStampHigh,TimeStampLow);
	if (CurBlockCount == 0) {
		pFirstTraceEntrySave=pntrc_get_cur_first_trace_entry_save(Category);
		if (pFirstTraceEntrySave->data.header.Subsys == 0) {  /* SubSys=0 indicates this is the first entry of a not overwritten buffer */
			/*Store 1st trace block. It is needed, when the buffer gets overwritten */
			pntrc_tbb_WriteTraceHeader(pFirstTraceEntrySave,SubSys,Level,TracepointRef,ParCount,TimeStampHigh,TimeStampLow);
		}
	}

	pBufferHeader->CurBlockCount = pBufferHeader->CurBlockCount + 1;
}

LSA_VOID pntrc_tbb_WriteHeaderTraceBlock(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount)
{
	LSA_UINT32   TimeStampHigh;
	LSA_UINT32   TimeStampLow;

	PNTRC_GET_TRACESTAMP (&TimeStampLow, &TimeStampHigh);

	pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp(Category, SubSys, Level, TracepointRef, ParCount, TimeStampHigh, TimeStampLow);
}

LSA_VOID pntrc_tbb_WriteParTraceBlock(LSA_UINT8 Category, LSA_UINT8 ParCount, LSA_UINT32 Par1, LSA_UINT32 Par2, LSA_UINT32 Par3)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
	PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock;

	pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
	if (pBufferHeader->CurBlockCount > pBufferHeader->MaxBlockCount - 1) {
		Par1=Par1; //PNTRC_FATAL (0);
	}
	pTraceBlock=pntrc_get_cur_trace_block(Category,pBufferHeader->FirstBlock,pBufferHeader->CurBlockCount);

	pTraceBlock->data.header.Subsys = 0;
	pTraceBlock->data.header.Flags = 0;

	pTraceBlock->data.traceEntry.ParCnt = ParCount;
	pTraceBlock->data.traceEntry.par.pars.par1 = Par1;
	pTraceBlock->data.traceEntry.par.pars.par2 = Par2;
	pTraceBlock->data.traceEntry.par.pars.par3 = Par3;

	pBufferHeader->CurBlockCount = pBufferHeader->CurBlockCount + 1;
}


LSA_UINT32 pntrc_tbb_SwitchBuffer(LSA_UINT8 Category)
{
	PNTRC_BUFFER_HEADER_PTR_TYPE pNewBufferHeader;
	PNTRC_BUFFER_HEADER_PTR_TYPE pCurBufferHeader;
	PNTRC_BUFFER_ENTRY_PTR_TYPE pFirstTraceEntrySave;
	LSA_UINT32* pCurBufferNr;
	LSA_UINT32 NewBufferNr;
	LSA_UINT32* pCurBufferState;
	LSA_UINT32* pNewBufferState;
#ifdef ZERO_TRACE_BUFFERS
	LSA_VOID* Ptr;
#endif

	if (g_pPntrcData->bufferSwitchInProgress) {
		pCurBufferHeader = pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
		pCurBufferHeader->Flags=pCurBufferHeader->Flags | TRACE_BUF_FLAG_INTR_SKIP;
		return LSA_ERR_SWITCH_ALREADY_IN_PROGRESS;
	}
	g_pPntrcData->bufferSwitchInProgress=LSA_TRUE;

	pCurBufferHeader = pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
	/* BUFFER SWITCH algorithm */
	pNewBufferHeader = pntrc_get_other_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
//	NewBufferNr = pntrc_GetOtherBufferNr(pCurTrcBuffer);
	/* WRAP AROUND algorithm */
	pCurBufferNr=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].CurBufferNr;
	pCurBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].StateBuffer[*pCurBufferNr];
	NewBufferNr=1-*pCurBufferNr;
	pNewBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].StateBuffer[NewBufferNr];
	if ((*pNewBufferState == PNTRC_BUFFER_STATE_FULL)   /*Nothing fetched yet, so stay in 2nd buffer with wrap around*/
		|| (*pNewBufferState != PNTRC_BUFFER_STATE_EMPTY)) {      /*User blocks other buffer, so stay in buffer with wrap around*/
		/*if other buffer is in use, so wrap around the current one*/

		//Now prepare buffer ptr, because we want to write traces immediately
		*pCurBufferState=PNTRC_BUFFER_STATE_EMPTY;
		pCurBufferHeader->CurBlockCount = 0;
		/*Indicate the wrap around. The original first entry of the buffer is retrieved and the associated time stamp is traced as the new first entry.
		  So it can be reconstructed between which time stamps trace information is lost.*/
		pFirstTraceEntrySave=pntrc_get_cur_first_trace_entry_save(Category);
		pntrc_tbb_WriteHeaderTraceBlock(Category, pFirstTraceEntrySave->data.header.Subsys,PNTRC_OVERRUN_MASK,0,2);
		pntrc_tbb_WriteParTraceBlock(Category,2,
			pFirstTraceEntrySave->data.header.TimestampHigh,
			pFirstTraceEntrySave->data.header.TimestampLow,
			0);
	} else {
		pNewBufferHeader->CurBlockCount=0;
		*pNewBufferState=PNTRC_BUFFER_STATE_FILLING;
		g_pPntrcData->FirstTraceEntry[Category][NewBufferNr].data.header.Subsys=0; //indicate that buffers is new
		*pCurBufferNr=1-*pCurBufferNr;  //switch the buffers
#ifdef ZERO_TRACE_BUFFERS
		Ptr=(LSA_VOID*)((LSA_UINT8*)pNewBufferHeader+pNewBufferHeader->FirstBlock);
		PNTRC_MEMSET_LOCAL_MEM(Ptr,0,pNewBufferHeader->MaxBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE));
#endif
		*pCurBufferState=PNTRC_BUFFER_STATE_FULL;
		pntrc_tbb_SendBufferSystemAdaption(&g_pPntrcData->hSysDev,pCurBufferHeader->Id,pCurBufferHeader);
	}

	g_pPntrcData->bufferSwitchInProgress=LSA_FALSE;
	return LSA_OK;
}

LSA_VOID pntrc_tbb_save_all_buffers(LSA_BOOL FetchFatal)
{
	LSA_UINT8 i,j;
	PNTRC_TWIN_BUFFERS_DESC_PTR_TYPE BuffersDesc;
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;

	BuffersDesc=g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0]	;

	if (FetchFatal) {
		PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching buffers");
	}
	for (i=0; i<PNTRC_MAX_CATEGORY_COUNT; i++) {
		for (j=0; j<2; j++) {
			if (BuffersDesc[i].StateBuffer[j] != PNTRC_BUFFER_STATE_EMPTY) {
				pBufferHeader = pntrc_get_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,i,j);
				if (pBufferHeader->CurBlockCount > 0) {
					if (FetchFatal) {
						PNTRC_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,i,j);
					}
					pntrc_tbb_SendBufferSystemAdaption(&g_pPntrcData->hSysDev,pBufferHeader->Id,pBufferHeader);
				}
			}
		}
	}
	for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
			pntrc_tbb_save_all_lower_buffers(g_pPntrcData->pntrc_lower_cpu_list[i].SysHandle, i, LSA_NULL, FetchFatal);
		}
	}
}

LSA_UINT32 pntrc_tbb_CheckAndSwitchBuffer(LSA_UINT8 Category, LSA_UINT8 ParCount)
{
	LSA_UINT16 TraceBlockCount;
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
	LSA_UINT32 Ret=LSA_OK;

	TraceBlockCount = (ParCount+2)/3 + 1;  //+2 = round up to 3

	pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
	if (pBufferHeader->CurBlockCount + TraceBlockCount -1 > pBufferHeader->MaxBlockCount - 1) {
		Ret=pntrc_tbb_SwitchBuffer(Category);
	}
	return Ret;
}

LSA_UINT32 pntrc_enter_trace(LSA_UINT8 Category)
{
	LSA_UINT32 ret;

	ret = PNTRC_ENTER_TRACE(Category);
	return ret;

}

LSA_VOID pntrc_exit_trace(LSA_UINT8 Category)
{
	PNTRC_EXIT_TRACE(Category);
}

LSA_VOID pntrc_ForceBufferSwitch(LSA_VOID)
{
	LSA_UINT32* pCurBufferNr;
	LSA_UINT32 NewBufferNr;
	LSA_UINT32* pCurBufferState;
	LSA_UINT32* pNewBufferState;
	PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
	LSA_UINT16 i;
	LSA_UINT8 j;
	PNTRC_MIF_TYPE Mif;

	for (j=0; j<PNTRC_MAX_CATEGORY_COUNT; j++) {
		pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,j);
		if (pBufferHeader->CurBlockCount > 0) {
			pCurBufferNr=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][j].CurBufferNr;
			pCurBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][j].StateBuffer[*pCurBufferNr];
			NewBufferNr=1-*pCurBufferNr;
			pNewBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][j].StateBuffer[NewBufferNr];

			if (*pNewBufferState == PNTRC_BUFFER_STATE_EMPTY) {      /*only switch buffer, if the other one is free*/
				if (pntrc_enter_trace(j) == 0) {  //avoid reentrancy, if a trace entry wants to switch the buffers
					pntrc_tbb_SwitchBuffer(j);
				}
				pntrc_exit_trace(j);
			}
		}
	}
	for (i=1; i<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; i++) {
		if (g_pPntrcData->pntrc_lower_cpu_list[i].Opened) {
			Mif.ForceBufferSwitch=1;
			PNTRC_WRITE_LOWER_CPU(g_pPntrcData->pntrc_lower_cpu_list[i].SysHandle,
				(LSA_UINT8*)(&Mif.ForceBufferSwitch),
				PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,ForceBufferSwitch),
				sizeof(Mif.ForceBufferSwitch));
		}
	}
}

LSA_UINT8 CheckIfAnyBufferFull(PNTRC_MIF_PTR_TYPE Mif)
{
	LSA_UINT16 i,j,k;
	LSA_UINT8 Full=0;

	for (i=0; i<1+Mif->Config.MaxLowerCPUCount; i++) {
		for (j=0; j<Mif->Config.MaxCategoryCount; j++) {
			for (k=0; k<2; k++) {
				if (Mif->BuffersDesc[i][j].StateBuffer[k] == PNTRC_BUFFER_STATE_FULL) {
					Full=1;
				}
			}
		}
	}
	return Full;
}


LSA_VOID pntrc_service_MIF_set_log_levels_CB(LSA_VOID* rqb0)
{
	LSA_UNUSED_ARG(rqb0);

	PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: pntrc_service_MIF_set_log_levels_CB - NewWriteTraceLevels=0");
	g_pPntrcData->TraceMem->MasterMIF.NewWriteTraceLevels=0;
}

LSA_VOID pntrc_service_MIF(LSA_VOID)
{
	LSA_USER_ID_TYPE UserID;

	UserID.void_ptr=LSA_NULL;  /*avoid compiler warning*/
	if (g_pPntrcData->TraceMem->MasterMIF.TimeSync.NewSyncTime) {
		PNTRC_WRITE_SYNC_TIME(g_pPntrcData->TraceMem->MasterMIF.TimeSync.SyncTimeLow,g_pPntrcData->TraceMem->MasterMIF.TimeSync.SyncTimeHigh);
		g_pPntrcData->TraceMem->MasterMIF.TimeSync.NewSyncTime=0;
	}
	if (g_pPntrcData->TraceMem->MasterMIF.ForceBufferSwitch) {
		PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "MIF service: pntrc force buffer switch");
		pntrc_ForceBufferSwitch();
		g_pPntrcData->TraceMem->MasterMIF.ForceBufferSwitch=0;
	}
	if (!g_pPntrcData->SetLogLevelPending && g_pPntrcData->TraceMem->MasterMIF.NewWriteTraceLevels) {
  		PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Lower: setting %d log levels from MIF",TRACE_SUBSYS_NUM);
		pntrc_tbb_set_log_levels(g_pPntrcData->TraceMem->MasterMIF.WriteTraceLevels, UserID, pntrc_service_MIF_set_log_levels_CB);
	}
	g_pPntrcData->TraceMem->MasterMIF.AnyBufferFull=CheckIfAnyBufferFull(&g_pPntrcData->TraceMem->MasterMIF);
}

LSA_VOID pntrc_service_slave_MIF_set_log_levels_CB(LSA_VOID* rqb0)
{
	LSA_UNUSED_ARG(rqb0);

	g_pPntrcData->TraceMem->SlaveMif[0].NewWriteTraceLevels=0;
}


LSA_VOID pntrc_service_slave_MIF(LSA_VOID)
{
	LSA_USER_ID_TYPE UserID;
	LSA_UINT8 i;

	UserID.void_ptr=LSA_NULL;  /*avoid compiler warning*/
	for (i=0; i<PNTRC_MAX_SLAVE_MIF; i++) {
		if (!g_pPntrcData->SetLogLevelPending && g_pPntrcData->TraceMem->SlaveMif[i].NewWriteTraceLevels) {
  			PNTRC_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Lower: setting %d log levels from slave MIF %d",TRACE_SUBSYS_NUM,i);
			pntrc_tbb_set_log_levels(g_pPntrcData->TraceMem->SlaveMif[i].WriteTraceLevels, UserID, pntrc_service_slave_MIF_set_log_levels_CB);
		}
	}
}

LSA_UINT32 pntrc_tbb_CheckAndWriteHeader(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount)
{
	LSA_UINT32 Ret;

	Ret=pntrc_tbb_CheckAndSwitchBuffer(Category,ParCount);
	if (Ret == LSA_RET_OK) {
		pntrc_tbb_WriteHeaderTraceBlock(Category,SubSys, Level, TracepointRef, ParCount);
	}
	return Ret;
}

/*=============================================================================
 * function name:  pntrc_tbb_memory_00
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_00 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 0);
		}
		pntrc_exit_trace(Category);
	}
    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_01
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_01 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 1) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 1, a1, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_02
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_02 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 2) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

/*=============================================================================
 * function name:  pntrc_tbb_memory_03
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_03 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 3) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 3, a1, a2, a3);
			}
		}
		pntrc_exit_trace(Category);
	}
    return Result;
}

/*=============================================================================
 * function name:  pntrc_tbb_memory_04
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_04 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 4) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 4, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 1, a4, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_05
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_05 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 5) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 5, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 2, a4, a5, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

/*=============================================================================
 * function name:  pntrc_tbb_memory_06
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_06 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 6) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 6, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 3, a4, a5, a6);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_07
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_07 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 7) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 7, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 4, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 1, a7, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_08
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_08 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 8) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 8, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 5, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 2, a7, a8, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_09
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_09 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 9) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 9, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 6, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 3, a7, a8, a9);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_10
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_10 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 10) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 10, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 7, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 4, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 1, a10, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_11
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_11 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 11) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 11, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 8, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 5, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 2, a10, a11, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_12
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *                 LSA_UINT32       a12:      argument 12
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_12 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 12) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 12, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 9, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 6, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 3, a10, a11, a12);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_13
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *                 LSA_UINT32       a12:      argument 12
 *                 LSA_UINT32       a13:      argument 13
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_13 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 13) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 13, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 10, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 7, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 4, a10, a11, a12);
				pntrc_tbb_WriteParTraceBlock(Category, 1, a13, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_14
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *                 LSA_UINT32       a12:      argument 12
 *                 LSA_UINT32       a13:      argument 13
 *                 LSA_UINT32       a14:      argument 14
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_14 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 14) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 14, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 11, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 8, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 5, a10, a11, a12);
				pntrc_tbb_WriteParTraceBlock(Category, 2, a13, a14, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}


/*=============================================================================
 * function name:  pntrc_tbb_memory_15
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *                 LSA_UINT32       a12:      argument 12
 *                 LSA_UINT32       a13:      argument 13
 *                 LSA_UINT32       a14:      argument 14
 *                 LSA_UINT32       a15:      argument 15
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_15 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14,
    LSA_UINT32             a15
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 15) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 15, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 12, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 9, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 6, a10, a11, a12);
				pntrc_tbb_WriteParTraceBlock(Category, 3, a13, a14, a15);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}




/*=============================================================================
 * function name:  pntrc_tbb_memory_16
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE SubSys:   SubSys number
 *                 PNTRC_LEVEL_TYPE  Level:    Level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *                 LSA_UINT32       a5:       argument 5
 *                 LSA_UINT32       a6:       argument 6
 *                 LSA_UINT32       a7:       argument 7
 *                 LSA_UINT32       a8:       argument 8
 *                 LSA_UINT32       a9:       argument 9
 *                 LSA_UINT32       a10:      argument 10
 *                 LSA_UINT32       a11:      argument 11
 *                 LSA_UINT32       a12:      argument 12
 *                 LSA_UINT32       a13:      argument 13
 *                 LSA_UINT32       a14:      argument 14
 *                 LSA_UINT32       a15:      argument 15
 *                 LSA_UINT32       a16:      argument 16
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_16 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14,
    LSA_UINT32             a15,
    LSA_UINT32             a16
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 16) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 16, a1, a2, a3);
				pntrc_tbb_WriteParTraceBlock(Category, 13, a4, a5, a6);
				pntrc_tbb_WriteParTraceBlock(Category, 10, a7, a8, a9);
				pntrc_tbb_WriteParTraceBlock(Category, 7, a10, a11, a12);
				pntrc_tbb_WriteParTraceBlock(Category, 4, a13, a14, a15);
				pntrc_tbb_WriteParTraceBlock(Category, 1, a16, 0, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

LSA_UINT16 pntrc_IntMin(LSA_UINT16 a, LSA_UINT16 b)
{
	if (a<b) return a;
	else return b;
}

LSA_VOID pntrc_tbb_trace_byte_array(LSA_UINT8 Category,
									LTRC_SUBSYS_TYPE SubSys,
									PNTRC_LEVEL_TYPE Level,
									LSA_UINT16 Flags,
									LSA_UINT32 TracePointRef,
									LSA_UINT8 *DataPtr,
									LSA_UINT16 DataLen)
{
	LSA_UINT16 BytesWritten=0;
    LSA_UINT8 RemainingParams;
    LSA_UINT8 RemainingParamsStore;
    LSA_UINT16 ReadPos=0;
	LSA_UINT16 FetchCount;
	LSA_UINT32 a[3];
	LSA_UINT16 i;
	LSA_UINT16 BytesToWrite;
	LSA_UINT8 ParamCount;

	if (DataLen == 0) ParamCount=0;
	else ParamCount = (LSA_UINT8)((DataLen-1) / 4) + 1;  /*number of parameters from dataparameter*/
	RemainingParams = ParamCount;
	if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level | Flags, TracePointRef, ParamCount) == LSA_RET_OK) {
		while (BytesWritten<DataLen) {
		    RemainingParamsStore=RemainingParams;
			BytesToWrite=pntrc_IntMin(12,DataLen-BytesWritten);
			for (i=0; i<3; i++) a[i]=0;
			i=0;
			while (BytesToWrite > 0) {
				FetchCount=pntrc_IntMin(4,DataLen-BytesWritten);
				PNTRC_MEMCPY_LOCAL_MEM(&a[i], &(DataPtr[ReadPos]), (LSA_UINT32)FetchCount);
				i=i+1;
				ReadPos=ReadPos+FetchCount;
				BytesWritten=BytesWritten+FetchCount;
				RemainingParams=RemainingParams-1;
				BytesToWrite=BytesToWrite-FetchCount;
			}
			pntrc_tbb_WriteParTraceBlock(Category, RemainingParamsStore, a[0], a[1], a[2]);
		}
	}
}

/*=============================================================================
* function name:  pntrc_tbb_memory_bytearray
*
* function:       Print binary trace entry into the memory buffer
*
* parameters:     LTRC_SUBSYS_TYPE SubSys:              SubSys number
*                 PNTRC_LEVEL_TYPE  Level:               Level number
*                 LSA_CHAR  *      msg:                 formatted string
*                 LSA_UINT32       TracePointRef: tracepoint reference
*                 LSA_UINT8 *      DataPtr:             datapointer
*                 LSA_UINT16       DataLen:             datalength
*
* return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
*                                  LSA_FALSE trace entry is not printed
*
*===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_bytearray (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_UINT8 *            DataPtr,
    LSA_UINT16             DataLen
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			pntrc_tbb_trace_byte_array(Category, SubSys,Level,PNTRC_BYTE_ARRAY_MASK,TracePointRef,DataPtr,DataLen);
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

/*=============================================================================
* function name:  pntrc_tbb_memory_string
*
* function:       Print binary trace entry into the memory buffer
*
* parameters:     LTRC_SUBSYS_TYPE SubSys:              SubSys number
*                 PNTRC_LEVEL_TYPE  Level:               Level number
*                 LSA_CHAR  *      msg:                 formatted string
*                 LSA_UINT32       TracePointRef: tracepoint reference
*                 LSA_UINT8 *      DataPtr:             datapointer
*                 LSA_UINT16       DataLen:             datalength
*
* return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
*                                  LSA_FALSE trace entry is not printed
*
*===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_string (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracePointRef,
    LSA_CHAR*              st
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			pntrc_tbb_trace_byte_array(Category, SubSys,Level,PNTRC_STRING_MASK,TracePointRef,(LSA_UINT8*)st,(LSA_UINT16)PNTRC_STRLEN(st)+1);
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

/*=============================================================================
 * function name:  pntrc_tbb_memory_timesync
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_CHAR  *      msg:      formatted string
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_BOOL  pntrc_tbb_memory_synctime (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UINT32             Line,
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_CHAR*              Msg,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
	LSA_UINT8 Category;

	LSA_UNUSED_ARG(Level);
	LSA_UNUSED_ARG(Idx);
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UNUSED_ARG(ModuleId);
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
	LSA_UNUSED_ARG(File);
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
	LSA_UNUSED_ARG(Line);
#endif
#if (PNTRC_CFG_COMPILE_TEXT == 1)
	LSA_UNUSED_ARG(Msg);
#endif

	Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
	if (Category<PNTRC_MAX_CATEGORY_COUNT) {
		if (pntrc_enter_trace(Category) == 0) {
			if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level | PNTRC_SYNC_TIME_MASK, TracepointRef, 2) == LSA_RET_OK) {
				pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
			}
		}
		pntrc_exit_trace(Category);
	}

    return Result;
}

/*****************************************************************************/
/*  end of file LTRC_TBB.C                                                   */
/*****************************************************************************/
