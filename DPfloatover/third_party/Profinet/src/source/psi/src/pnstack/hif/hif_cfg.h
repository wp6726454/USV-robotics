#ifndef HIF_CFG_H
#define HIF_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: hif_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component HIF using PSI.                       */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "hif_inc.h"
 */

void hif_psi_startstop (int start);



/*===========================================================================*/
/*   stdLib                                                                  */
/*===========================================================================*/
#define HIF_MEMCPY  PSI_MEMCPY
#define HIF_MEMCMP  PSI_MEMCMP
#define HIF_MEMSET  PSI_MEMSET
#define HIF_STRLEN  PSI_STRLEN


/*===========================================================================*/
/*                                HIF-Settings                               */
/*===========================================================================*/

/* Settings @ psi_cfg.h */

/*------------------------------------------------------------------------------
// HIF: size of a LSA_BOOL for HIF (1 || 2 || 4 ==> 8 || 16 || 32 Bit)
//----------------------------------------------------------------------------*/
#define HIF_LSA_BOOL_SIZE   1

/*------------------------------------------------------------------------------
// HIF Context Change Output Macro on hif_lower_request_done() and hif_lower_sys_request_done() call
//----------------------------------------------------------------------------*/
#define HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO 0
#define HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO     0

/*------------------------------------------------------------------------------
// HIF Number of highest LSA_COMP_ID in system + 1
//----------------------------------------------------------------------------*/
#define HIF_MAX_COMP_ID (PSI_COMP_ID_LAST + 1)

/*------------------------------------------------------------------------------
// HIF Compile RQB serialization
//----------------------------------------------------------------------------*/
#ifndef HIF_CFG_COMPILE_SERIALIZATION
# error "HIF_CFG_COMPILE_SERIALIZATION not defined"
#endif

/*------------------------------------------------------------------------------
// HIF MEM malloc Settings
//----------------------------------------------------------------------------*/
#define HIF_CFG_MEM_MALLOC_ALIGN        32          /* 32 byte allignment     */
#define HIF_CFG_MEM_MALLOC_POOL_MAX     0           /* no global storage used */

/*------------------------------------------------------------------------------
// HIF MEM Ring Pool Settings
//----------------------------------------------------------------------------*/

#define HIF_CFG_MEM_RING_POOL_ALIGN     32          /* 32 byte allignment     */
#define HIF_CFG_MEM_RING_POOL_MAX       (HIF_CFG_MAX_LD_INSTANCES + HIF_CFG_MAX_HD_INSTANCES)
/**
 * Save block desctiption external (outside of pool), to prevent from pci read access of allocated data in shared memory 
 */
#define HIF_CFG_MEM_RING_SAVE_BLOCK_DESC_EXTERNAL

/*===========================================================================*/
/*                       HIF_CFG_MEM_RING_CHECK_OVERWRITE                    */
/*===========================================================================*/
/** 
  * This switch can be activated to check HIF serialization/deserialization overwrite in Shared Memory.
  * Possible values are:
  *
  * HIF_MEM_RING_CHECK_START_STOP_1
  * Restrictions: currently only available when using HIF_MEM_RING
  * After every serialization the current SER writepointer will be checked against the RQB SHM start pointer + RQB serialization size.
  * Will generate a START sequenze (STAR (0x)) before the allocated SER buffer and a STOP sequenze (STOP (0x)) after the buffer. These 
  * 8 Bytes (2 * 4 Bytes) will be transparent for the "user". After every de-/serialization HIF will check if the START and STOP sequences
  * are okay and not overwritten.
  *
  * HIF_MEM_RING_CHECK_START_STOP_4
  * Restrictions: currently only available when using HIF_MEM_RING
  * Includes HIF_MEM_RING_CHECK_START_STOP_1 but with 4 START sequences at the beginning and 4 STOP sequences after the allocated shared memory
  **/
//#define HIF_CFG_MEM_RING_CHECK_OVERWRITE     HIF_MEM_RING_CHECK_START_STOP_4

/*------------------------------------------------------------------------------
// HIF User defined Pipes
//----------------------------------------------------------------------------*/

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 1)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_1 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 2)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_2 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 3)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_3 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 4)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_4 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 5)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_5 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 6)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_6 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 7)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_7 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS == 8)
#define HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_8 , LSA_COMP_ID_SOCK  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8)
#error "HIF: Only 8 sock app user channels supported by HIF"
#endif

#if (HIF_CFG_USE_SNMPX == 1)
#define HIF_CPLD_GLO_APP_SNMPX_USER { HIF_CPLD_GLO_APP_SNMPX_USER_CH , LSA_COMP_ID_SNMPX },
#else
#define HIF_CPLD_GLO_APP_SNMPX_USER
#endif

/* User defined global Pipes for the HIF Logical Device */
#define HIF_CFG_LD_GLOB_USR_PIPES  {     \
		HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE                       /* User Application Channel - Sock           */ \
		HIF_CPLD_GLO_APP_SNMPX_USER                              /* User Application Channel - SNMPX          */ \
		{ HIF_CPLD_GLO_APP_OHA_USER_CH    , LSA_COMP_ID_OHA   }, /* User Application Channel - Object Handler */ \
		{ HIF_CPLD_GLO_APP_CLRPC_USER_CH  , LSA_COMP_ID_CLRPC }  /* User Application Channel - CLRPC          */ \
	}

/* User defined Interface specific Pipes for the HIF Logical Device */
#if (HIF_CFG_USE_IOH == 1)
#define HIF_CFG_LD_GLOB_USR_APP_IOH { HIF_CPLD_IF_APP_IOH_USER_CH , LSA_COMP_ID_IOH  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_IOH
#endif

#define HIF_CFG_LD_IF_USR_PIPES  {     \
		HIF_CFG_LD_GLOB_USR_APP_IOH                                   /* User Application Channel - IO-Application        */ \
		{ HIF_CPLD_IF_APP_CMPD_USER_CH          , LSA_COMP_ID_CM   }, /* User Application Channel - Physical Device       */ \
		{ HIF_CPLD_IF_APP_IOC_CMCL_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Controller         */ \
		{ HIF_CPLD_IF_APP_IOM_CMMC_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Multicast          */ \
		{ HIF_CPLD_IF_APP_IOD_CMSV_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Device             */ \
		{ HIF_CPLD_IF_APP_OHA_USER_CH           , LSA_COMP_ID_OHA  }, /* User Application Channel - Object Handler        */ \
		{ HIF_CPLD_IF_APP_EDD_SIMATIC_TIME_SYNC , LSA_COMP_ID_EDD  }, /* User Application Channel - Edd Simatic Time Sync */ \
		{ HIF_CPLD_IF_APP_NARE_IP_TEST          , LSA_COMP_ID_NARE }  /* User Application Channel - Nare Ip Test          */ \
	}

/* User defined Global Pipes for the HIF Hardware Device (not used) */
//#define HIF_CFG_HD_GLOB_USR_PIPES  {   
//		{ USR_PIPE1, LSA_COMP_ID_SOCK }, 
//		{ USR_PIPE2, LSA_COMP_ID_SOCK }  
//	}

/* User defined Interface specific Pipes for the HIF Hardware Device */
#if (HIF_CFG_USE_IOH == 1)
#define HIF_CFG_HD_GLOB_USR_APP_IOH { HIF_CPHD_IF_APP_IOH_USER_CH , LSA_COMP_ID_IOH  },
#else
#define HIF_CFG_HD_GLOB_USR_APP_IOH
#endif

#if (HIF_CFG_USE_MRP == 1)
#define HIF_CPHD_IF_SYS_MRP_EDD_ { HIF_CPHD_IF_SYS_MRP_EDD , LSA_COMP_ID_MRP },
#else
#define HIF_CPHD_IF_SYS_MRP_EDD_
#endif

#if (HIF_CFG_USE_GSY == 1)
#define HIF_CPHD_IF_SYS_GSY_EDD_SYNC_ { HIF_CPHD_IF_SYS_GSY_EDD_SYNC , LSA_COMP_ID_GSY },
#define HIF_CPHD_IF_SYS_GSY_EDD_ANNO_ { HIF_CPHD_IF_SYS_GSY_EDD_ANNO , LSA_COMP_ID_GSY },
#else
#define HIF_CPHD_IF_SYS_GSY_EDD_SYNC_
#define HIF_CPHD_IF_SYS_GSY_EDD_ANNO_
#endif

#define HIF_CFG_HD_IF_USR_PIPES  {     \
		HIF_CFG_HD_GLOB_USR_APP_IOH                                   /* User Application Channel - IO-Application        */ \
		{ HIF_CPHD_IF_APP_CMPD_USER_CH          , LSA_COMP_ID_CM   }, /* User Application Channel - Physical Device       */ \
		{ HIF_CPHD_IF_APP_IOC_CMCL_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Controller         */ \
		{ HIF_CPHD_IF_APP_IOM_CMMC_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Multicast          */ \
		{ HIF_CPHD_IF_APP_IOD_CMSV_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Device             */ \
		{ HIF_CPHD_IF_APP_EDD_SIMATIC_TIME_SYNC , LSA_COMP_ID_EDD  }, /* User Application Channel - Edd Simatic Time Sync */ \
		{ HIF_CPHD_IF_APP_NARE_IP_TEST          , LSA_COMP_ID_NARE }, /* User Application Channel - Nare Ip Test          */ \
		{ HIF_CPHD_IF_SYS_CM_CLRPC              , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_OHA                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_MRP_EDD_                                      /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_DCP_EDD               , LSA_COMP_ID_DCP  }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_NARE_EDD              , LSA_COMP_ID_NARE }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_NARE_DCP_EDD          , LSA_COMP_ID_NARE }, /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_GSY_EDD_SYNC_                                 /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_GSY_EDD_ANNO_                                 /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_ACP_EDD               , LSA_COMP_ID_ACP  }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_EDD                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_MRP                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_POF_EDD            , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_GSY_CLOCK          , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_NARE               , LSA_COMP_ID_CM   }  /* Pipe used by PSI LD Channel State Machine        */ \
	}

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define HIF_DEBUG  PSI_DEBUG

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define HIF_RQB_HEADER						LSA_RQB_HEADER(HIF_RQB_PTR_TYPE)

#define HIF_RQB_TRAILER						LSA_RQB_TRAILER

#define HIF_RQB_SET_NEXT_RQB_PTR(rb,v)		LSA_RQB_SET_NEXT_RQB_PTR(rb,v)
#define HIF_RQB_SET_PREV_RQB_PTR(rb,v)		LSA_RQB_SET_PREV_RQB_PTR(rb,v)
#define HIF_RQB_SET_OPCODE(rb,v)			LSA_RQB_SET_OPCODE(rb,v)
#define HIF_RQB_SET_HANDLE(rb,v)			LSA_RQB_SET_HANDLE(rb,v)
#define HIF_RQB_SET_RESPONSE(rb,v)			LSA_RQB_SET_RESPONSE(rb,v)
#define HIF_RQB_SET_USER_ID_PTR(rb,v)		LSA_RQB_SET_USER_ID_PTR(rb,v)
#define HIF_RQB_SET_USER_ID(rb,v)			LSA_RQB_SET_USER_ID(rb,v)
#define HIF_RQB_SET_COMP_ID(rb,v)			LSA_RQB_SET_COMP_ID(rb,v)

#define HIF_RQB_GET_NEXT_RQB_PTR(rb)		LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define HIF_RQB_GET_PREV_RQB_PTR(rb)		LSA_RQB_GET_PREV_RQB_PTR(rb)
#define HIF_RQB_GET_OPCODE(rb)				LSA_RQB_GET_OPCODE(rb)
#define HIF_RQB_GET_HANDLE(rb)				LSA_RQB_GET_HANDLE(rb)
#define HIF_RQB_GET_RESPONSE(rb)			LSA_RQB_GET_RESPONSE(rb)
#define HIF_RQB_GET_USER_ID_PTR(rb)			LSA_RQB_GET_USER_ID_PTR(rb)
#define HIF_RQB_GET_USER_ID(rb)				LSA_RQB_GET_USER_ID(rb)
#define HIF_RQB_GET_COMP_ID(rb)             LSA_RQB_GET_COMP_ID(rb)

/*------------------------------------------------------------------------------
// HIF System Handle definition
//----------------------------------------------------------------------------*/

#define HIF_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

/*------------------------------------------------------------------------------
// HD/IF Id (Hardware Device ID / Interface ID)
//----------------------------------------------------------------------------*/

#define HIF_HD_ID_TYPE   LSA_UINT16
#define HIF_IF_ID_TYPE   LSA_UINT32

/*------------------------------------------------------------------------------
// maximum number of HIF Instances
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_MAX_LD_INSTANCES
# error "HIF_CFG_MAX_LD_INSTANCES not defined"
#endif

#ifndef HIF_CFG_MAX_HD_INSTANCES
# error "HIF_CFG_MAX_HD_INSTANCES not defined"
#endif

/*------------------------------------------------------------------------------
// maximum number of HIF Channel handles
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_MAX_LD_CH_HANDLES
# error "HIF_CFG_MAX_LD_CH_HANDLES not defined"
#endif

#ifndef HIF_CFG_MAX_HD_CH_HANDLES
# error "HIF_CFG_MAX_HD_CH_HANDLES not defined"
#endif

/*------------------------------------------------------------------------------
// usage of App specific HD Sock channels not defined
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS
# error "HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS"
#endif

/*------------------------------------------------------------------------------
// maximum number of RB Tags
//----------------------------------------------------------------------------*/

#ifdef HIF_CFG_MAX_LD_RB_TAGS
# error "HIF_CFG_MAX_LD_RB_TAGS defined! - Not Supported any More!"
#endif

#ifdef HIF_CFG_MAX_HD_RB_TAGS
# error "HIF_CFG_MAX_HD_RB_TAGS defined! - Not Supported any More!"
#endif

/*------------------------------------------------------------------------------
// HIF ISR Poll Interval
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_POLL_INTERVAL
# error "HIF_CFG_POLL_INTERVAL not defined"
#endif


/*------------------------------------------------------------------------------
// maximum number of HIF hardware devices per logical device
//----------------------------------------------------------------------------*/

#ifndef HIF_LD_MAX_HD
# error "HIF_LD_MAX_HD not defined"
#endif

/*------------------------------------------------------------------------------
// maximum number of interfaces per logical device
//----------------------------------------------------------------------------*/

#ifndef HIF_LD_MAX_IF
# error "HIF_LD_MAX_IF not defined"
#endif

/*------------------------------------------------------------------------------
// maximum number of interfaces per hardware device
//----------------------------------------------------------------------------*/

#ifndef HIF_HD_MAX_IF
# error "HIF_HD_MAX_IF not defined"
#endif

/*------------------------------------------------------------------------------
// HIF Options
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_COMPILE_SERIALIZATION
# error "HIF_CFG_COMPILE_SERIALIZATION not defined"
#endif

/*------------------------------------------------------------------------------
// HIF Supported components
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_USE_ACP
# error "HIF_CFG_USE_ACP not defined"
#endif
#ifndef HIF_CFG_USE_CLRPC
# error "HIF_CFG_USE_CLRPC not defined"
#endif
#ifndef HIF_CFG_USE_CM
# error "HIF_CFG_USE_CM not defined"
#endif
#ifndef HIF_CFG_USE_DCP
# error "HIF_CFG_USE_DCP not defined"
#endif
#ifndef HIF_CFG_USE_EDD
# error "HIF_CFG_USE_EDD not defined"
#endif
#ifndef HIF_CFG_USE_GSY
# error "HIF_CFG_USE_GSY not defined"
#endif
#ifndef HIF_CFG_USE_IOH
# error "HIF_CFG_USE_IOH not defined"
#endif
#ifndef HIF_CFG_USE_LLDP
# error "HIF_CFG_USE_LLDP not defined"
#endif
#ifndef HIF_CFG_USE_MRP
# error "HIF_CFG_USE_MRP not defined"
#endif
#ifndef HIF_CFG_USE_NARE
# error "HIF_CFG_USE_NARE not defined"
#endif
#ifndef HIF_CFG_USE_OHA
# error "HIF_CFG_USE_OHA not defined"
#endif
#ifndef HIF_CFG_USE_PNTRC
# error "HIF_CFG_USE_PNTRC not defined"
#endif
#ifndef HIF_CFG_USE_PSI
# error "HIF_CFG_USE_PSI not defined"
#endif
#ifndef HIF_CFG_USE_SOCK
# error "HIF_CFG_USE_SOCK not defined"
#endif
#ifndef HIF_CFG_USE_SNMPX
# error "HIF_CFG_USE_SNMPX not defined"
#endif

/*------------------------------------------------------------------------------
// enable / disable cast-warnings
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_CAST_WARN
# define HIF_CFG_CAST_WARN  0
#endif


/*------------------------------------------------------------------------------
// enable / disable HIF_ASSERT
//----------------------------------------------------------------------------*/

#if HIF_DEBUG

extern int hif_semicolon_missing; /* see hif_cfg.c */

	/* check for condition: internal programming error */
# define HIF_ASSERT(cond) \
	if (! (cond)) { HIF_FATAL (0); } else hif_semicolon_missing = 1

#else

	/* no more programming error exists :-) */
# define HIF_ASSERT(cond)

#endif


/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef HIF_FILE_SYSTEM_EXTENSION
# define HIF_FILE_SYSTEM_EXTENSION(module_id_)
#endif


/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace [default]
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_TRACE_MODE
# error "HIF_CFG_TRACE_MODE not defined" 
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif



/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of HIF_CFG_H */
