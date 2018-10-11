#ifndef PNTRC_USR_H
#define PNTRC_USR_H

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
/*  F i l e               &F: pntrc_usr.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*------------------------------------------------------------------------------
//	OPCODES
//	for PNTRC 
//----------------------------------------------------------------------------*/

/* System */
#define PNTRC_OPC_SET_TRACE_LEVELS            1
#define PNTRC_OPC_SET_TRACE_LEVELS_COMP       2
#define PNTRC_OPC_SET_TRACE_LEVELS_SUBSYS     3
#define PNTRC_OPC_SET_TRACE_LEVELS_IF         4
#define PNTRC_OPC_SWITCH_BUFFERS              5
#define PNTRC_OPC_GET_TRACE_LEVELS            6

#define PNTRC_RSP_OK                     LSA_OK   /* without errors */
#define PNTRC_RSP_ERR_BUSY               0x02     /* PNTRC is busy */  
#define PNTRC_RSP_ERR_REQUEST_PENDING    0x05    
#define PNTRC_RSP_ERR_CPUS_STILL_OPEN    0x06    

#define PNTRC_RQB_HEADER						LSA_RQB_HEADER(PNTRC_RQB_PTR_TYPE)
#define PNTRC_RQB_TRAILER						LSA_RQB_TRAILER

typedef  LSA_VOID  LSA_FCT_PTR( , PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE)(
	LSA_VOID*  rb
);

typedef struct pntrc_rqb_tag  *PNTRC_RQB_PTR_TYPE;

typedef struct pntrc_rqb_tag
{
    PNTRC_RQB_HEADER
    union
    {
        struct
        {
			LSA_UINT8* pLogLevels;				   //In: log levels
        } set_trace_levels;
        struct
        {
			LTRC_COMP_TYPE comp;
			PNTRC_LEVEL_TYPE level;
        } set_trace_levels_comp;
        struct
        {
			LTRC_SUBSYS_TYPE subsys;
			PNTRC_LEVEL_TYPE level;
        } set_trace_levels_subsys;
        struct
        {
			LTRC_SUBSYS_TYPE intf;
			PNTRC_LEVEL_TYPE level;
        } set_trace_levels_if;
//        struct
//        {
//			int i;   
//        } switch_buffers;        
        struct
        {
			LSA_UINT8* pLogLevels;				   //Out: log levels, array of size TRACE_SUBSYS_NUM must be allocated by user
        } get_trace_levels;
    } args;   
	PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE Cbf;        //In:  Callback function
    PNTRC_RQB_TRAILER
} PNTRC_RQB_TYPE;

#define PNTRC_CATEGORY_TYPE LSA_UINT32


#define PNTRC_MAX_CATEGORY_COUNT 2
#define PNTRC_MAX_SLAVE_MIF 1

typedef struct pntrc_general_tag
{	
	LSA_UINT32 MajorVersion;
	LSA_UINT32 MinorVersion;
	LSA_UINT32 Endiness;
	LSA_UINT32 Padding;
} PNTRC_MIF_GENERAL_TYPE;

typedef struct pntrc_config_tag
{	
	LSA_UINT32 MaxLowerCPUCount;
	LSA_UINT32 MaxCategoryCount;
	LSA_UINT32 IsTopMostCPU;
	LSA_UINT32 TraceSubSysNum;
} PNTRC_MIF_CONFIG_TYPE;

typedef struct pntrc_timesync_tag
{	
	LSA_UINT32 NewSyncTime;
	LSA_UINT32 SyncTimeLow;
	LSA_UINT32 SyncTimeHigh;
} PNTRC_MIF_TIMESYNC_TYPE;

typedef struct pntrc_twin_buffers_desc_tag
{	
	LSA_UINT32 CurBufferNr;
	LSA_UINT32 OffsetBuffer[2];
	LSA_UINT32 SizeBuffer[2];
	LSA_UINT32 StateBuffer[2];
} PNTRC_TWIN_BUFFERS_DESC_TYPE;
typedef PNTRC_TWIN_BUFFERS_DESC_TYPE* PNTRC_TWIN_BUFFERS_DESC_PTR_TYPE;

typedef struct pntrc_mif_tag
{
	LSA_UINT32 MagicNumber;
	LSA_UINT32 Ready;
	LSA_UINT32 AnyBufferFull;	
	LSA_UINT32 FatalOccurred;	

	PNTRC_MIF_GENERAL_TYPE General;

	LSA_UINT32 ForceBufferSwitch;

	PNTRC_MIF_CONFIG_TYPE Config;
	PNTRC_TWIN_BUFFERS_DESC_TYPE BuffersDesc[1+PNTRC_CFG_MAX_LOWER_CPU_CNT][PNTRC_MAX_CATEGORY_COUNT];

	PNTRC_MIF_TIMESYNC_TYPE TimeSync;

	LSA_UINT32 NewWriteTraceLevels;
	LSA_UINT8 WriteTraceLevels[TRACE_SUBSYS_NUM];

	LSA_UINT32 NewReadTraceLevels;
	LSA_UINT8 ReadTraceLevels[TRACE_SUBSYS_NUM];
} PNTRC_MIF_TYPE; 
typedef PNTRC_MIF_TYPE* PNTRC_MIF_PTR_TYPE;

#define TRACE_MEM_MAGICNUMBER 0x11772288
#define TRACE_BUF_MAGICNUMBER 0x99887766

#define TRACE_BUF_FLAG_INTR_SKIP 0x01

#define LSA_ERR_SWITCH_ALREADY_IN_PROGRESS 2

typedef struct pntrc_trace_mem_tag
{	
	PNTRC_MIF_TYPE MasterMIF;
    PNTRC_MIF_TYPE SlaveMif[PNTRC_MAX_SLAVE_MIF];
    LSA_UINT8 Buffers;
} PNTRC_TRACE_MEM_TYPE;
typedef PNTRC_TRACE_MEM_TYPE *PNTRC_TRACE_MEM_PTR_TYPE;

typedef struct pntrc1_buffer_header_tag
{
	LSA_UINT32			         MagicNumber;
	LSA_UINT32					 ByteWidth;
	PNTRC_SYSTEM_ID_TYPE         Id;
	LSA_UINT32                   Size;
	LSA_UINT32                   MaxBlockCount;
	LSA_UINT32                   FirstBlock;
	LSA_UINT32                   CurBlockCount;
	LSA_UINT32                   TimeStampWidth;
	LSA_UINT32                   TimeStampUnit;
	LSA_UINT32                   Options;
	LSA_UINT32		             Flags;
	LSA_UINT32                   Reserved1;
} PNTRC_BUFFER_HEADER_TYPE;
typedef PNTRC_BUFFER_HEADER_TYPE* PNTRC_BUFFER_HEADER_PTR_TYPE;

typedef struct pntrc_buffer_entry_tag
{
	 union
	 {
		struct
		{
			LSA_UINT32 Header;
			LSA_UINT32 a1;
			LSA_UINT32 a2;
			LSA_UINT32 a3;
		} classic;
		struct
		{
			LSA_UINT8  ParCnt;
			LSA_UINT8  Flags;
			LSA_UINT16 Subsys;

			LSA_UINT16 LineNr;
			LSA_UINT16 ModuleId;
			LSA_UINT32 TimestampHigh;
			LSA_UINT32 TimestampLow;
		} header;
		struct
		{
			LSA_UINT8  ParCnt;
			LSA_UINT8  Flags;
			LSA_UINT16 ReservedZero;
			union
			{
				struct
				{
					 LSA_UINT32 par1;
					 LSA_UINT32 par2;
					 LSA_UINT32 par3;
				} pars;
				struct
				{
					 LSA_UINT8 par1;
					 LSA_UINT8 par2;
					 LSA_UINT8 par3;
					 LSA_UINT8 par4;
					 LSA_UINT8 par5;
					 LSA_UINT8 par6;
					 LSA_UINT8 par7;
					 LSA_UINT8 par8;
					 LSA_UINT8 par9;
					 LSA_UINT8 par10;
					 LSA_UINT8 par11;
					 LSA_UINT8 par12;               
				} byteArray;
			} par;
		} traceEntry;
	} data;
} PNTRC_BUFFER_ENTRY_TYPE;

typedef PNTRC_BUFFER_ENTRY_TYPE* PNTRC_BUFFER_ENTRY_PTR_TYPE;

typedef enum pntrc_buffer_state_enum {
	PNTRC_BUFFER_STATE_EMPTY		= 1,
	PNTRC_BUFFER_STATE_FILLING		= 2,
	PNTRC_BUFFER_STATE_FULL			= 3,
} BUFFER_STATE_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/


/*===========================================================================*/
/*                               user macros                                 */
/*===========================================================================*/




/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/


/*====  in functions  =====*/



/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_USR_H */
