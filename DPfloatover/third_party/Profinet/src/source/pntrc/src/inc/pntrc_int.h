#ifndef PNTRC_INT_H
#define PNTRC_INT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: pntrc_int.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

#if (PNTRC_MODULE_ID == 1)
	LSA_UINT8 ltrc_current_level[TRACE_SUBSYS_NUM] = {0};  /*legacy mode for adonis.c - to be removed*/
	LSA_UINT8 pntrc_current_level[TRACE_SUBSYS_NUM] = {0};

	struct pntrc_entry_fcts pntrc_current_fct= {
	PNTRC_TRACE_OUT_00,
	PNTRC_TRACE_OUT_01,
	PNTRC_TRACE_OUT_02,
	PNTRC_TRACE_OUT_03,
	PNTRC_TRACE_OUT_04,
	PNTRC_TRACE_OUT_05,
	PNTRC_TRACE_OUT_06,
	PNTRC_TRACE_OUT_07,
	PNTRC_TRACE_OUT_08,
	PNTRC_TRACE_OUT_09,
	PNTRC_TRACE_OUT_10,
	PNTRC_TRACE_OUT_11,
	PNTRC_TRACE_OUT_12,
	PNTRC_TRACE_OUT_13,
	PNTRC_TRACE_OUT_14,
	PNTRC_TRACE_OUT_15,
	PNTRC_TRACE_OUT_16,
	PNTRC_TRACE_OUT_BYTE_ARRAY,
};

//	struct pntrc_entry_fcts pntrc_current_fct= {
//	pntrc_tbb_memory_00,
//	pntrc_tbb_memory_01,
//	pntrc_tbb_memory_02,
//	pntrc_tbb_memory_03,
//	pntrc_tbb_memory_04,
//	pntrc_tbb_memory_05,
//	pntrc_tbb_memory_06,
//	pntrc_tbb_memory_07,
//	pntrc_tbb_memory_08,
//	pntrc_tbb_memory_09,
//	pntrc_tbb_memory_10,
//	pntrc_tbb_memory_11,
//	pntrc_tbb_memory_12,
//	pntrc_tbb_memory_13,
//	pntrc_tbb_memory_14,
//	pntrc_tbb_memory_15,
//	pntrc_tbb_memory_16,
//	pntrc_tbb_memory_bytearray,
//};
#endif

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define is_null(ptr)			(LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define is_not_null(ptr)		(! LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define are_equal(ptr1,ptr2)	(LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

#define PNTRC_FATAL(error_code_0)	{\
	pntrc_fatal_error (PNTRC_MODULE_ID, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL\
		);\
	}

/*------------------------------------------------------------------------------
// enums
//----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
// structs
//----------------------------------------------------------------------------*/

typedef struct pntrc_data_tag
{
	PNTRC_TRACE_MEM_PTR_TYPE TraceMem;
	PNTRC_BUFFER_ENTRY_TYPE FirstTraceEntry[PNTRC_MAX_CATEGORY_COUNT][2];    //storage for one local trace entry per buffer, needed when buffer is overwritten
	PNTRC_SYS_HANDLE_TYPE hSysDev;
	PNTRC_CB_BUFFER_FULL pCBBufferFull;
	PNTRC_CPU_HANDLE_TYPE pntrc_lower_cpu_list[1+PNTRC_CFG_MAX_LOWER_CPU_CNT];
	LSA_BOOL SetLogLevelPending;
	PNTRC_RQB_PTR_TYPE RQBSetTraceLevels;
	PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE SetLogLevelCB;
	LSA_USER_ID_TYPE SetLogLevelUserID;
	LSA_UINT8* SetLogLevel_pLogLevels;
	LSA_BOOL UseInitLogLevels;
	LSA_BOOL forceBufferSwitch;
	LSA_UINT8* pRQBLogLevels;
	LSA_UINT8* pTraceBuffer[1+PNTRC_CFG_MAX_LOWER_CPU_CNT][PNTRC_MAX_CATEGORY_COUNT][2];
	LSA_BOOL bufferSwitchInProgress;
	LSA_UINT32 pntrc_SendSyncScaler; 
} PNTRC_DATA_TYPE;

typedef PNTRC_DATA_TYPE* PNTRC_DATA_PTR_TYPE;

/*===========================================================================*/
/*                                data                                       */
/*===========================================================================*/
#if (PNTRC_MODULE_ID == 1)

 PNTRC_DATA_TYPE g_PntrcData;
 PNTRC_DATA_PTR_TYPE g_pPntrcData=LSA_NULL;

#else

 LSA_EXTERN PNTRC_DATA_TYPE g_PntrcData;
 LSA_EXTERN PNTRC_DATA_PTR_TYPE g_pPntrcData;

#endif

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*-pntrc_sys.c------------------------------------------------------------------*/


/*-pntrc_usr.c------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_INT_H */
