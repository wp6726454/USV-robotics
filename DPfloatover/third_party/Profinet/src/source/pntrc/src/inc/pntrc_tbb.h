#ifndef PNTRC_TBB_H                      /* ----- reinclude-protection ----- */
#define PNTRC_TBB_H

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
/*  F i l e               &F: pntrc_trc.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the system LSA interface                                      */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

#define LTRC_TBB_ADD_INFO_LEVEL                   0x0F
#define LTRC_TBB_ADD_INFO_TRACESTAMP              0x10
#define LTRC_TBB_ADD_INFO_BYTEARRAY               0x20
#define LTRC_TBB_ADD_INFO_RESERVED_B              0x40
#define LTRC_TBB_ADD_INFO_RESERVED_C              0x80

#define LTRC_LINE_COUNT_TBB_TSON_00                  1
#define LTRC_LINE_COUNT_TBB_TSON_01                  2
#define LTRC_LINE_COUNT_TBB_TSON_02                  2
#define LTRC_LINE_COUNT_TBB_TSON_03                  2
#define LTRC_LINE_COUNT_TBB_TSON_04                  3
#define LTRC_LINE_COUNT_TBB_TSON_05                  3
#define LTRC_LINE_COUNT_TBB_TSON_06                  3
#define LTRC_LINE_COUNT_TBB_TSON_07                  4
#define LTRC_LINE_COUNT_TBB_TSON_08                  4
#define LTRC_LINE_COUNT_TBB_TSON_09                  4
#define LTRC_LINE_COUNT_TBB_TSON_10                  5
#define LTRC_LINE_COUNT_TBB_TSON_11                  5
#define LTRC_LINE_COUNT_TBB_TSON_12                  5
#define LTRC_LINE_COUNT_TBB_TSON_13                  6
#define LTRC_LINE_COUNT_TBB_TSON_14                  6
#define LTRC_LINE_COUNT_TBB_TSON_15                  6
#define LTRC_LINE_COUNT_TBB_TSON_16                  7

/*===========================================================================*/
/*                           sys structs/ unions                             */
/*===========================================================================*/


/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                         prototyping memory                                */
/*===========================================================================*/

#define PNTRC_MIF_READY 0x431707   //magic number

LSA_EXTERN LSA_VOID pntrc_tbb_init(PNTRC_INIT_PTR_TYPE pInit);
LSA_EXTERN LSA_VOID	pntrc_tbb_uninit(LSA_VOID);
LSA_EXTERN LSA_VOID pntrc_tbb_set_ready(LSA_VOID);
LSA_EXTERN LSA_VOID pntrc_tbb_buffer_full_done(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID);

LSA_EXTERN LSA_VOID pntrc_tbb_set_buffer_to_free(LSA_UINT32 Offset);
LSA_EXTERN LSA_VOID pntrc_TimerSwitchesBuffer(LSA_VOID);
LSA_EXTERN LSA_VOID pntrc_tbb_set_log_levels(LSA_UINT8* pLogLevels, LSA_USER_ID_TYPE UserID, PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE SetLogLevelCB);

LSA_EXTERN LSA_VOID pntrc_tbb_get_log_levels(LSA_UINT8* pLogLevels);

LSA_EXTERN LSA_VOID pntrc_tbb_service_lower_cpus(LSA_VOID);
LSA_EXTERN LSA_VOID pntrc_service_MIF(LSA_VOID);
LSA_EXTERN LSA_VOID pntrc_service_slave_MIF(LSA_VOID);

LSA_VOID pntrc_ForceBufferSwitch(LSA_VOID);

LSA_VOID pntrc_tbb_save_all_buffers(LSA_BOOL FetchFatal);
LSA_VOID pntrc_tbb_save_all_lower_buffers(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, PNTRC_MIF_PTR_TYPE pMif, LSA_BOOL FetchFatal);

//LSA_EXTERN LSA_VOID  LTRC_LOCAL_FCT_ATTR  ltrc_tbb_undo_init (LSA_VOID);
//LTRC_BUFFER_ADMIN_TYPE* LTRC_LOCAL_FCT_ATTR ltrc_tbb_buffer_init(LSA_UINT base_address, LSA_UINT32 buff_size);

/*=============================================================================
 * function name:  pntrc_tbb_memory_00
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_00 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_01
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       a1:       argument 1
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_01 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1
);

/*=============================================================================
 * function name:  pntrc_tbb_memory_02
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_02 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_03
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_03 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_04
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       a1:       argument 1
 *                 LSA_UINT32       a2:       argument 2
 *                 LSA_UINT32       a3:       argument 3
 *                 LSA_UINT32       a4:       argument 4
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_04 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_05
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_05 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_06
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_06 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_07
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_07 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_08
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_08 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_09
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_09 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_10
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_10 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_11
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_11 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_12
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_12 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_13
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_13 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_14
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_14 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_15
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_15 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_16
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
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
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_16 (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
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
);

/*=============================================================================
 * function name:  pntrc_tbb_memory_bytearray
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       ltrc_tracepoint_ref: tracepoint reference
 *                 LSA_UINT8 *      DataPtr:  datapointer
 *                 LSA_UINT16       DataLen:  datalen
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_bytearray (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_UINT8 *            DataPtr,
    LSA_UINT16             DataLen
);


/*=============================================================================
 * function name:  pntrc_tbb_memory_string
 *
 * function:       Print binary trace entry into the memory buffer
 *
 * parameters:     LTRC_SUBSYS_TYPE subsys:   subsys number
 *                 PNTRC_LEVEL_TYPE  level:    level number
 *                 LSA_UINT32       ltrc_tracepoint_ref: tracepoint reference
 *                 LSA_UINT8 *      DataPtr:  datapointer
 *                 LSA_UINT16       DataLen:  datalen
 *
 * return value:   LSA_BOOL         LSA_TRUE  trace entry is     printed
 *                                  LSA_FALSE trace entry is not printed
 *
 *===========================================================================*/
LSA_EXTERN
LSA_BOOL pntrc_tbb_memory_string (
    LTRC_SUBSYS_TYPE       subsys,
    PNTRC_LEVEL_TYPE       level,
	LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
	LSA_UINT32             ModulId,
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
    LSA_UINT32             ltrc_tracepoint_ref,
    LSA_CHAR *             st
);


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
    );

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/


/*****************************************************************************/
/*  end of file LTRC_TBB.H                                                   */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
#endif  /* of LTRC_TBB_H */
