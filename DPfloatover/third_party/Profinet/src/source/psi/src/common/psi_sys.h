#ifndef PSI_SYS_H_                  /* ----- reinclude-protection ----- */
#define PSI_SYS_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: psi_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System interface                                                         */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#define PSI_COMP_ID_LAST  LSA_COMP_ID_IOH  // note: IOH is max comp_id, see lsa_sys.h

struct psi_header {
	LSA_RQB_HEADER(struct psi_header*)
};

typedef void(*psi_postfunction)(void*);

/*===========================================================================*/
/*  PSI Sys Ptr                                                              */
/*===========================================================================*/

typedef struct psi_sys_tag
{
	LSA_UINT16      hd_nr;                  // HD number [1..N]
	LSA_UINT16      if_nr;                  // IF number [1..N]
	LSA_UINT16      comp_id;                // LSA_COMP_ID of owner
	LSA_UINT16      comp_id_lower;          // LSA_COMP_ID of lower component (i.E.: used EDDx lower apdaption)
	LSA_UINT16      comp_id_edd;            // LSA_COMP_ID of used EDDx (i.E.: used for HW adaptiom (i.E. GSY Sync)
	LSA_UINT16      pnio_if_nr;             // PNIO IF number [0..16](0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT16      rsp_mbx_id;             // MBX ID for posting responses (from Lower to upper)
	LSA_UINT32      trace_idx;              // Trace Index
	PSI_SYS_HANDLE  hSysIF;                 // hSysDev for one HD IF
	LSA_INT         hPoolNRT;               // NRT pool handle for one HD IF
	LSA_BOOL        check_arp;              // check for ARP sniffing at callback yes/no
	LSA_BOOL        runs_on_level_basic;    // set if LD/HD run on different systems
} PSI_SYS_TYPE, * PSI_SYS_PTR_TYPE;

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1))
typedef struct psi_edd_sys_tag
{
	LSA_UINT16      hd_nr;                  // HD number [1..N]
	LSA_UINT16      if_nr;                  // IF number (logical number for PN IF) [1..N]
	LSA_UINT16      pnio_if_nr;             // PNIO IF number [0..16](0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT16      edd_comp_id;            // LSA_COMP_ID of used EDDx
} PSI_EDD_SYS_TYPE, * PSI_EDD_SYS_PTR_TYPE;
#endif

// Note: Tasks see description of PNIO tasks
typedef enum {
	PSI_NRT_ORG_TASK_ID,
	PSI_NRT_LOW_TASK_ID,
	PSI_EDDI_SYS_REST_TASK_ID,
	PSI_EDDI_AUX_TASK_ID,
	PSI_EDDP_TASK_ID,
    #if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	PSI_EDDS_RQB_TASK_ID,
    #elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	PSI_EDDS_HIGH_TASK_ID,
	PSI_EDDS_MIDDLE_TASK_ID,
	PSI_EDDS_LOW_TASK_ID,
    #endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
	PSI_IP_TASK_ID,
	PSI_PN_LSA_LD_TASK_ID,
	PSI_PN_LSA_HD_TASK_ID,
	PSI_PN_LSA_LOW_TASK_ID,
	PSI_HIF_HD_TASK_ID,
	PSI_HIF_LD_TASK_ID,
	PSI_PN_TASK_ID,
	PSI_POF_TASK_ID,
	PSI_IOH_TASK_ID,
	/*-------------------------*/
	PSI_MAX_TASK_ID        /* Max TASK ID */
} PSI_TASK_ID_TYPE;

/*===========================================================================*/
/*  PSI mbox adaption                                                        */
/*===========================================================================*/

#define PSI_MBX_ID_EDDI_AUX		    PSI_EDDI_AUX_TASK_ID
#define PSI_MBX_ID_EDDI_NRT_LOW	    PSI_NRT_LOW_TASK_ID
#define PSI_MBX_ID_EDDI_REST	    PSI_EDDI_SYS_REST_TASK_ID
#define PSI_MBX_ID_EDDP_HIGH	    PSI_EDDP_TASK_ID
#define PSI_MBX_ID_EDDP_LOW		    PSI_EDDP_TASK_ID
#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#define PSI_MBX_ID_EDDS_RQB		    PSI_EDDS_RQB_TASK_ID
#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)
#define PSI_MBX_ID_EDDS_HIGH	    PSI_EDDS_HIGH_TASK_ID
#define PSI_MBX_ID_EDDS_MIDDLE      PSI_EDDS_MIDDLE_TASK_ID
#define PSI_MBX_ID_EDDS_LOW		    PSI_EDDS_LOW_TASK_ID
#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE
#define PSI_MBX_ID_HIF_HD		    PSI_HIF_HD_TASK_ID
#define PSI_MBX_ID_HIF_LD	        PSI_HIF_LD_TASK_ID
#define PSI_MBX_ID_EDDI_NRT_ORG	    PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_GSY	            PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_LLDP			    PSI_NRT_ORG_TASK_ID    
#define PSI_MBX_ID_MRP		        PSI_NRT_ORG_TASK_ID
#define PSI_MBX_ID_ACP			    PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_CM	            PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_DCP	            PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_NARE			    PSI_PN_LSA_HD_TASK_ID
#define PSI_MBX_ID_CLRPC		    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_OHA			    PSI_PN_LSA_LD_TASK_ID
#define PSI_MBX_ID_POF			    PSI_POF_TASK_ID
#define PSI_MBX_ID_SNMPX		    PSI_IP_TASK_ID
#define PSI_MBX_ID_SOCK			    PSI_IP_TASK_ID
#define PSI_MBX_ID_TCIP			    PSI_IP_TASK_ID
#define PSI_MBX_ID_IOH              PSI_IOH_TASK_ID
#define PSI_MBX_ID_PSI              PSI_PN_TASK_ID
#define PSI_MBX_ID_USER			    PSI_MAX_TASK_ID
#define PSI_MBX_ID_MAX              (PSI_MAX_TASK_ID+1)

LSA_VOID psi_mbx_sys_init(LSA_VOID);
LSA_VOID psi_mbx_sys_undo_init(LSA_VOID);

/*===========================================================================*/
/*  PSI threading                                                            */
/*===========================================================================*/

#ifndef PSI_THREAD_READY
LSA_VOID PSI_THREAD_READY( LSA_VOID_PTR_TYPE arg );
#endif

#ifndef PSI_THREAD_STOPPED
LSA_VOID PSI_THREAD_STOPPED( LSA_VOID_PTR_TYPE arg );
#endif

/*---------------------------------------------------------------------------*/
/*  SYS PATH definition for system and user channels                         */
/*---------------------------------------------------------------------------*/

typedef enum psi_path_enum {
	PSI_SYS_PATH_INVALID = 0, /* Reserved for invalid */

	/* group of global system channels */
	PSI_PATH_GLO_SYS_CLRPC_SOCK,
	PSI_PATH_GLO_SYS_SNMPX_SOCK,
	PSI_PATH_GLO_SYS_OHA_SOCK,
	PSI_PATH_GLO_SYS_OHA_SNMPX,

	/* group of IF specific system channels */
	PSI_PATH_IF_SYS_MRP_EDD,
	PSI_PATH_IF_SYS_TCIP_EDD_ARP,
	PSI_PATH_IF_SYS_TCIP_EDD_ICMP,
	PSI_PATH_IF_SYS_TCIP_EDD_UDP,
	PSI_PATH_IF_SYS_TCIP_EDD_TCP,
	PSI_PATH_IF_SYS_DCP_EDD,
	PSI_PATH_IF_SYS_NARE_EDD,
	PSI_PATH_IF_SYS_NARE_DCP_EDD,
	PSI_PATH_IF_SYS_OHA_EDD,
	PSI_PATH_IF_SYS_OHA_LLDP_EDD,
	PSI_PATH_IF_SYS_OHA_DCP_EDD,
	PSI_PATH_IF_SYS_OHA_NARE,
	PSI_PATH_IF_SYS_OHA_MRP,
	PSI_PATH_IF_SYS_GSY_EDD_SYNC,
	PSI_PATH_IF_SYS_GSY_EDD_ANNO,
	PSI_PATH_IF_SYS_ACP_EDD,
	PSI_PATH_IF_SYS_CM_EDD,
	PSI_PATH_IF_SYS_CM_CLRPC,
	PSI_PATH_IF_SYS_CM_MRP,
	PSI_PATH_IF_SYS_CM_POF_EDD,
	PSI_PATH_IF_SYS_CM_GSY_CLOCK,
	PSI_PATH_IF_SYS_CM_NARE,
	PSI_PATH_IF_SYS_CM_OHA,

	/* group of global application user channels */
	PSI_PATH_GLO_APP_SOCK1,
	PSI_PATH_GLO_APP_SOCK2,
	PSI_PATH_GLO_APP_SOCK3,
	PSI_PATH_GLO_APP_SOCK4,
	PSI_PATH_GLO_APP_SOCK5,
	PSI_PATH_GLO_APP_SOCK6,
	PSI_PATH_GLO_APP_SOCK7,
	PSI_PATH_GLO_APP_SOCK8,
	PSI_PATH_GLO_APP_OHA,
	PSI_PATH_GLO_APP_SNMPX,
	PSI_PATH_GLO_APP_IOS_CLRPC,

	/* group of IF specific application user channels */
	PSI_PATH_IF_APP_CMPD,
	PSI_PATH_IF_APP_IOC_CMCL_ACP,
	PSI_PATH_IF_APP_IOM_CMMC_ACP,
	PSI_PATH_IF_APP_IOD_CMSV_ACP,
	PSI_PATH_IF_APP_OHA,
	PSI_PATH_IF_APP_IOH,
	PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC,
	PSI_PATH_IF_APP_NARE_IP_TEST,

	/*---------------------------------------------------------------------------*/
	PSI_SYS_PATH_MAX
} PSI_SYS_PATH_TYPE;

/*===========================================================================*/
/*     FATAL error types                                                     */
/*===========================================================================*/

#define PSI_FATAL_ERROR_PTR_TYPE          /* pointer to LSA_FATAL_ERROR */ \
	struct lsa_fatal_error_tag*

/*===========================================================================*/
/*                                FctPtr NRTMEM                              */
/*===========================================================================*/
typedef struct _PSI_USR_NRTMEM_ID_FKT_TYPE
{
	LSA_UINT8* LSA_FCT_PTR( , Alloc)( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
	LSA_VOID   LSA_FCT_PTR( , Free)( LSA_UINT8* ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id );
} PSI_USR_NRTMEM_ID_FKT_TYPE;

/*===========================================================================*/
/*                                   Macros                                  */
/*===========================================================================*/

#define PSI_FATAL(error_code_0)	{\
	psi_fatal_error( LSA_COMP_ID_PSI, PSI_MODULE_ID, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL\
		);\
	}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*  Initialization                                                           */
/*---------------------------------------------------------------------------*/

LSA_UINT16 psi_init( LSA_VOID );
LSA_UINT16 psi_undo_init( LSA_VOID );

/*---------------------------------------------------------------------------*/
/*  stdlib                                                                   */
/*---------------------------------------------------------------------------*/

#ifndef PSI_MEMSET
LSA_VOID PSI_MEMSET(
	LSA_VOID_PTR_TYPE ptr,
	LSA_INT           val,
	LSA_UINT          len );
#endif

#ifndef PSI_MEMCPY
LSA_VOID PSI_MEMCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src,
	LSA_UINT          len );
#endif

#ifndef PSI_MEMCMP
LSA_INT PSI_MEMCMP(
	LSA_VOID_PTR_TYPE pBuf1,
	LSA_VOID_PTR_TYPE pBuf2,
	LSA_UINT          Length );
#endif

#ifndef PSI_MEMMOVE
LSA_VOID PSI_MEMMOVE(
	LSA_VOID_PTR_TYPE  dst,
	LSA_VOID_PTR_TYPE  src,
	LSA_UINT           len );
#endif

#ifndef PSI_STRCPY
LSA_VOID PSI_STRCPY(
	LSA_VOID_PTR_TYPE dst,
	LSA_VOID_PTR_TYPE src );
#endif

#ifndef PSI_STRLEN
LSA_UINT PSI_STRLEN(
	LSA_VOID_PTR_TYPE str );
#endif

#ifndef PSI_SPRINTF
LSA_INT PSI_SPRINTF(
	LSA_UINT8  *str,
	LSA_CHAR   *fmt,
	... );
#endif

#ifndef PSI_SSCANF_UUID
LSA_INT PSI_SSCANF_UUID(
	LSA_UINT8  *uuid_string,
	LSA_CHAR   *fmt,
	LSA_UINT32 *time_low,
	LSA_INT    *time_mid,
	LSA_INT    *time_hi_and_version,
	LSA_INT    *clock_seq_hi_and_reserved,
	LSA_INT    *clock_seq_low,
	LSA_INT    *node0,
	LSA_INT    *node1,
	LSA_INT    *node2,
	LSA_INT    *node3,
	LSA_INT    *node4,
	LSA_INT    *node5,
	LSA_INT    *read_count );
#endif

#ifndef PSI_LOG_10
LSA_VOID PSI_LOG_10(
	LSA_UINT32  const Arg_in_ppm,
	LSA_INT32         * pResult_in_ppm );
#endif

#ifndef PSI_POW_10
LSA_VOID PSI_POW_10(
	LSA_INT16       const numerator, 
	LSA_UINT16      const denominator, 
	LSA_UINT32*     pResult );
#endif

#ifndef PSI_EXCHANGE_LONG
long PSI_EXCHANGE_LONG(
	long volatile * long_ptr,
	long            val );
#endif

/*---------------------------------------------------------------------------*/
/*  FATAL handling                                                           */
/*---------------------------------------------------------------------------*/

LSA_VOID psi_fatal_error(
	LSA_UINT16         comp_id,
	LSA_UINT16         module_id,
	LSA_UINT16         line,
	LSA_UINT32         error_code_0,
	LSA_UINT32         error_code_1,
	LSA_UINT32         error_code_2,
	LSA_UINT32         error_code_3,
	LSA_UINT16         error_data_len,
	LSA_VOID_PTR_TYPE  error_data );

#ifndef PSI_FATAL_ERROR
LSA_VOID PSI_FATAL_ERROR(
	LSA_CHAR                  *comp,
	LSA_UINT16                length,
	PSI_FATAL_ERROR_PTR_TYPE  error_ptr );
#endif

#ifndef PSI_RQB_ERROR
LSA_VOID  PSI_RQB_ERROR(
	LSA_UINT16         comp_id,
	LSA_UINT16         comp_id_lower,
	LSA_VOID_PTR_TYPE  rqb_ptr );
#endif

#ifndef PSI_FATAL_ERROR_OCCURED
LSA_BOOL PSI_FATAL_ERROR_OCCURED( LSA_VOID );
#endif

/*----------------------------------------------------------------------------*/
/* Reentrance Locks                                                           */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_REENTRANCE_LOCK
LSA_VOID PSI_ALLOC_REENTRANCE_LOCK( 
	LSA_UINT16  * ret_val_ptr, 
	LSA_UINT16  * lock_handle_ptr );
#endif

#ifndef PSI_FREE_REENTRANCE_LOCK
LSA_VOID PSI_FREE_REENTRANCE_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle );
#endif

#ifndef PSI_ENTER_REENTRANCE_LOCK
LSA_VOID PSI_ENTER_REENTRANCE_LOCK( 
	LSA_UINT16 lock_handle );
#endif

#ifndef PSI_EXIT_REENTRANCE_LOCK
LSA_VOID PSI_EXIT_REENTRANCE_LOCK( 
	LSA_UINT16 lock_handle );
#endif

/*---------------------------------------------------------------------------*/
/* Enter/exit handling (Reentrance locks) and Wait event                     */
/*---------------------------------------------------------------------------*/

/*=============================================================================
 * function name:  PSI_ENTER
 *
 * function:       set reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef PSI_ENTER
  LSA_VOID   PSI_ENTER(LSA_VOID);
#endif

/*=============================================================================
 * function name:  PSI_EXIT
 *
 * function:       cancel reentrance lock
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef PSI_EXIT
  LSA_VOID   PSI_EXIT(LSA_VOID);
#endif

/*----------------------------------------------------------------------------*/
/* IR Locks                                                                   */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_IR_LOCK
LSA_VOID PSI_ALLOC_IR_LOCK( 
	LSA_UINT16   * ret_val_ptr, 
	LSA_UINT16   * lock_handle_ptr );
#endif

#ifndef PSI_FREE_IR_LOCK
LSA_VOID PSI_FREE_IR_LOCK(
	 LSA_UINT16   *ret_val_ptr,
	 LSA_UINT16   lock_handle );
#endif

#ifndef PSI_ENTER_IR_LOCK
LSA_VOID PSI_ENTER_IR_LOCK( 
	LSA_UINT16 lock_handle );
#endif

#ifndef PSI_EXIT_IR_LOCK
LSA_VOID PSI_EXIT_IR_LOCK( 
	LSA_UINT16 lock_handle );
#endif

/*----------------------------------------------------------------------------*/
/* Blocking Waits                                                             */
/*----------------------------------------------------------------------------*/

#ifndef PSI_ALLOC_BLOCKING_WAIT
LSA_VOID  PSI_ALLOC_BLOCKING_WAIT(
	LSA_UINT16  *ret_val_ptr,
	LSA_VOID_PTR_TYPE bw_type);
#endif

#ifndef PSI_FREE_BLOCKING_WAIT
LSA_VOID  PSI_FREE_BLOCKING_WAIT( 
	LSA_UINT16  *ret_val_ptr,
	LSA_VOID_PTR_TYPE bw_type);
#endif

#ifndef PSI_SET_BLOCKING_WAIT
LSA_VOID  PSI_SET_BLOCKING_WAIT(
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_RESET_BLOCKING_WAIT
LSA_VOID  PSI_RESET_BLOCKING_WAIT( 
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_WAIT_BLOCKING_WAIT
LSA_VOID  PSI_WAIT_BLOCKING_WAIT( 
	LSA_VOID_PTR_TYPE bw_type );
#endif

#ifndef PSI_WAIT_NS
LSA_VOID PSI_WAIT_NS(
    LSA_UINT64 uTimeNs );
#endif

/*----------------------------------------------------------------------------*/
/* local memory                                                               */
/*----------------------------------------------------------------------------*/

/* Memory Types for malloc statistics */
#define PSI_MTYPE_IGNORE             0  // reserved for all user allocs with no statistics
#define PSI_MTYPE_LOCAL_MEM          1
#define PSI_MTYPE_UPPER_RQB          2
#define PSI_MTYPE_UPPER_MEM          3
#define PSI_MTYPE_NRT_MEM            4 // NRT send/receive buffer
#define PSI_MTYPE_PI_MEM             5 // Process image memory (KRAM, IOCC,...)
#define PSI_MTYPE_HIF_MEM            6

#define PSI_MTYPE_LOWER_RQB_ACP      7
#define PSI_MTYPE_LOWER_MEM_ACP      8
#define PSI_MTYPE_LOWER_RQB_CLRPC    9
#define PSI_MTYPE_LOWER_MEM_CLRPC    10
#define PSI_MTYPE_LOWER_RQB_DCP      11
#define PSI_MTYPE_LOWER_MEM_DCP      12
#define PSI_MTYPE_LOWER_RQB_EDD      13  /* use same index for all lower EDDx */
#define PSI_MTYPE_LOWER_MEM_EDD      14  /* use same index for all lower EDDx */
#define PSI_MTYPE_LOWER_RQB_GSY      15
#define PSI_MTYPE_LOWER_MEM_GSY      16
#define PSI_MTYPE_LOWER_RQB_LLDP     17
#define PSI_MTYPE_LOWER_MEM_LLDP     18
#define PSI_MTYPE_LOWER_RQB_MRP      19
#define PSI_MTYPE_LOWER_MEM_MRP      20
#define PSI_MTYPE_LOWER_RQB_NARE     21
#define PSI_MTYPE_LOWER_MEM_NARE     22
#define PSI_MTYPE_LOWER_RQB_OHA      23
#define PSI_MTYPE_LOWER_MEM_OHA      24
#define PSI_MTYPE_LOWER_RQB_POF      25
#define PSI_MTYPE_LOWER_MEM_POF      26
#define PSI_MTYPE_LOWER_RQB_SNMPX    27
#define PSI_MTYPE_LOWER_MEM_SNMPX    28
#define PSI_MTYPE_LOWER_RQB_SOCK     29
#define PSI_MTYPE_LOWER_MEM_SOCK     30
#define PSI_MTYPE_LOWER_RQB_IOH      31
#define PSI_MTYPE_LOWER_MEM_IOH      32
#define PSI_MTYPE_LOWER_RQB_PNTRC    33
#define PSI_MTYPE_LOWER_MEM_PNTRC    34
#define PSI_MTYPE_LOWER_RQB_HIF      35
#define PSI_MTYPE_LOWER_MEM_HIF      36
#define PSI_MTYPE_LOWER_RQB_PSI      37
#define PSI_MTYPE_LOWER_MEM_PSI      38

#define PSI_MTYPE_MAX    (PSI_MTYPE_LOWER_MEM_PSI+1)

#ifndef PSI_ALLOC_LOCAL_MEM
LSA_VOID PSI_ALLOC_LOCAL_MEM(
	LSA_VOID_PTR_TYPE     *mem_ptr_ptr,
	LSA_USER_ID_TYPE      user_id,
	LSA_UINT32            length,
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_UINT16            comp_id,
	LSA_UINT16            mem_type );
#endif

#ifndef PSI_FREE_LOCAL_MEM
LSA_VOID PSI_FREE_LOCAL_MEM(
	LSA_UINT16            *ret_val_ptr,
	LSA_VOID_PTR_TYPE     mem_ptr,
	LSA_SYS_PTR_TYPE      sys_ptr,
	LSA_UINT16            comp_id,
	LSA_UINT16            mem_type );
#endif

/*----------------------------------------------------------------------------*/
/* HIF memory pool                                                            */
/*----------------------------------------------------------------------------*/

#ifndef PSI_CREATE_HIF_MEM_POOL
LSA_VOID PSI_CREATE_HIF_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT *           pool_handle_ptr );
#endif

#ifndef PSI_DELETE_HIF_MEM_POOL
LSA_VOID PSI_DELETE_HIF_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT             pool_handle );
#endif

#ifndef PSI_HIF_ALLOC_MEM
LSA_VOID PSI_HIF_ALLOC_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	PSI_SYS_HANDLE      sys_handle,
	LSA_INT             pool_handle );
#endif

#ifndef PSI_HIF_FREE_MEM
LSA_VOID PSI_HIF_FREE_MEM(
	LSA_UINT16 *         ret_val_ptr,
	LSA_VOID_PTR_TYPE    mem_ptr,
	PSI_SYS_HANDLE       sys_handle,
	LSA_INT              pool_handle );
#endif

#ifndef PSI_HIF_SHM_DMA_COPY
LSA_VOID PSI_HIF_SHM_DMA_COPY(
    LSA_VOID * *        ptr_ptr_rqb,
	LSA_UINT32          rqb_len,
	PSI_SYS_HANDLE      sys_handle );
#endif

/*----------------------------------------------------------------------------*/
/* NRT memory pool (for one HD instance)                                      */
/*----------------------------------------------------------------------------*/

// NRT pool types available. Only used for create/delete
// no enum to ease PSI function definitions
// as long as the upper layers do not consequently use RX_DEFAULT and TX_DEFAULT, 
// RX_DEFAULT, TX_DEFAULT and NRT_POOL have to share the same pool!
#define PSI_HD_NRT_MEM                 0   /* NRT (RXTX) memory (ShardMem, SDRam) */
#define PSI_HD_NRT_DMACW_DEFAULT       1
#define PSI_HD_NRT_RX_DEFAULT          PSI_HD_NRT_MEM
#define PSI_HD_NRT_TX_DEFAULT          PSI_HD_NRT_MEM
#define PSI_HD_NRT_TX_LOW_FRAG         4      
#define PSI_HD_NRT_TX_MID_FRAG         5 
#define PSI_HD_NRT_TX_HIGH_FRAG        6
#define PSI_HD_NRT_TX_MGMTLOW_FRAG     7
#define PSI_HD_NRT_TX_MGMTHIGH_FRAG    8
#define PSI_HD_NRT_MAX_MEMPOOLS        9

#ifndef PSI_CREATE_NRT_MEM_POOL
LSA_VOID PSI_CREATE_NRT_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT *           pool_handle_ptr,
	LSA_UINT8           cp_mem_nrt );
#endif

#ifndef PSI_DELETE_NRT_MEM_POOL
LSA_VOID PSI_DELETE_NRT_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT             pool_handle,
	LSA_UINT8           cp_mem_nrt);
#endif

/* marked for DELETION */
#ifndef PSI_NRT_ALLOC_MEM
LSA_VOID PSI_NRT_ALLOC_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_ALLOC_TX_MEM
LSA_VOID PSI_NRT_ALLOC_TX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_ALLOC_RX_MEM
LSA_VOID PSI_NRT_ALLOC_RX_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

/* marked for DELETION */
#ifndef PSI_NRT_FREE_MEM
LSA_VOID PSI_NRT_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_FREE_TX_MEM
LSA_VOID PSI_NRT_FREE_TX_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

#ifndef PSI_NRT_FREE_RX_MEM
LSA_VOID PSI_NRT_FREE_RX_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle,
	LSA_UINT16          comp_id );
#endif

/*----------------------------------------------------------------------------*/
/* PI memory pool                                                             */
/*----------------------------------------------------------------------------*/

#ifndef PSI_CREATE_PI_MEM_POOL
LSA_VOID PSI_CREATE_PI_MEM_POOL( 
	LSA_VOID_PTR_TYPE   basePtr, 
	LSA_UINT32          length, 
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT *           pool_handle_ptr );
#endif

#ifndef PSI_DELETE_PI_MEM_POOL
LSA_VOID PSI_DELETE_PI_MEM_POOL( 
	LSA_UINT16 *        ret_val_ptr,
	PSI_SYS_HANDLE      sys_handle, 
	LSA_INT             pool_handle );
#endif

#ifndef PSI_PI_ALLOC_MEM
LSA_VOID PSI_PI_ALLOC_MEM( 
	LSA_VOID_PTR_TYPE * mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_INT             pool_handle,
	LSA_BOOL            is_provider );
#endif

#ifndef PSI_PI_FREE_MEM
LSA_VOID PSI_PI_FREE_MEM(
	LSA_UINT16 *        ret_val_ptr,
	LSA_VOID_PTR_TYPE   mem_ptr,
	LSA_INT             pool_handle );
#endif

/*----------------------------------------------------------------------------*/
/* Timer                                                                      */
/*----------------------------------------------------------------------------*/

typedef LSA_VOID (*PSI_TIMEOUT_CBF_PTR_TYPE)(LSA_TIMER_ID_TYPE, LSA_USER_ID_TYPE);

#ifndef PSI_ALLOC_TIMER
LSA_VOID PSI_ALLOC_TIMER_TGROUP0(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout );

LSA_VOID PSI_ALLOC_TIMER_TGROUP1(
	LSA_UINT16               * ret_val_ptr,
	LSA_TIMER_ID_TYPE        * timer_id_ptr,
	LSA_UINT16                 timer_type,
	LSA_UINT16                 time_base,
	PSI_TIMEOUT_CBF_PTR_TYPE   psi_timeout );
#endif

#ifndef PSI_START_TIMER
LSA_VOID  PSI_START_TIMER_TGROUP0(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time );

LSA_VOID  PSI_START_TIMER_TGROUP1(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id,
	LSA_USER_ID_TYPE    user_id,
	LSA_UINT16          time );
#endif

#ifndef PSI_STOP_TIMER
LSA_VOID  PSI_STOP_TIMER_TGROUP0(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );

LSA_VOID  PSI_STOP_TIMER_TGROUP1(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );
#endif

#ifndef PSI_FREE_TIMER
LSA_VOID  PSI_FREE_TIMER_TGROUP0(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );

LSA_VOID  PSI_FREE_TIMER_TGROUP1(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE   timer_id );
#endif

/*----------------------------------------------------------------------------*/
/*  Ticks count                                                               */
/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_TICKS_1MS
LSA_UINT32 PSI_GET_TICKS_1MS( LSA_VOID );
#endif

#ifndef PSI_GET_TICKS_100NS
LSA_UINT64 PSI_GET_TICKS_100NS( LSA_VOID );
#endif

/*----------------------------------------------------------------------------*/
/*  LD params                                                                 */
/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_IM_SERIAL_NUMBER
LSA_VOID  PSI_GET_IM_SERIAL_NUMBER(
	LSA_UINT16 *    ret_val_ptr,
	LSA_UINT8 *     im_serial_nr_ptr,
	LSA_UINT16      im_serial_nr_size );
#endif

/*----------------------------------------------------------------------------*/
/*  HD HW params                                                              */
/*----------------------------------------------------------------------------*/

//media type selector
#define PSI_MEDIA_TYPE_SEL_UNKNOWN  0x00    // reserved for invalid
#define PSI_MEDIA_TYPE_SEL_COPPER   0x01    // media type select copper cable
#define PSI_MEDIA_TYPE_SEL_FIBER    0x02    // media type select fiber cable (POF)
#define PSI_MEDIA_TYPE_SEL_WIRELESS 0x03    // media type select wireless
#define PSI_MEDIA_TYPE_SEL_AUTO     0x04    // automatic detection of media type: fiber cable (POF) or copper cable

typedef enum psi_eddp_board_type_enum { // enumeration for eddp board types
	PSI_EDDP_BOARD_TYPE_UNKNOWN                = 0x00, // unknown board
	PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1  = 0x01, // FPGA1-Ertec200P board Rev1
	PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2  = 0x02, // FPGA1-Ertec200P board Rev2
	PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1 = 0x03, // EB200P board Rev1
	PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2 = 0x04, // EB200P board Rev2
	PSI_EDDP_BOARD_MAX
} PSI_EDDP_BOARD_TYPE;

typedef struct psi_hd_mem_tag {  // Memory block information 
	LSA_UINT8* base_ptr;            // base address
	LSA_UINT32 size;                // Size of memory block
	LSA_UINT32 phy_addr;            // Phy. address 
} PSI_HD_MEM_TYPE, * PSI_HD_MEM_PTR_TYPE;

typedef struct psi_hd_eddi_tag { // HW info for EDDI HDs (IRTE-IP boards)
	LSA_UINT16                  device_type;                    // EDD_HW_TYPE_USED_SOC, EDD_HW_TYPE_USED_ERTEC_400, EDD_HW_TYPE_USED_ERTEC_200
	PSI_HD_MEM_TYPE				irte;							// IRTE memory
	PSI_HD_MEM_TYPE				kram;							// KRAM memory (part of it used for PI)
	PSI_HD_MEM_TYPE				iocc;							// IOCC memory (used for PI on SOC)
	PSI_HD_MEM_TYPE				paea;							// PAEA memory (located at offset in IOCC used for PI)
	PSI_HD_MEM_TYPE				sdram;							// SDRAM memory (used for NRT)
	PSI_HD_MEM_TYPE				shared_mem;						// Shared memory (used for NRT)
	PSI_HD_MEM_TYPE				apb_periph_scrb;				// APB-Periphericals SCRB
	PSI_HD_MEM_TYPE				apb_periph_timer;				// APB-Periphericals TIMER
    LSA_BOOL					has_ext_pll;					// EXT_PLL IF avaliable yes/no

    //SII configuration parameters
	LSA_UINT32					SII_IrqSelector;				// EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
	LSA_UINT32					SII_IrqNumber;					// EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
	LSA_UINT32					SII_ExtTimerInterval;			// 0, 250, 500, 1000 in us

    LSA_BOOL					fragmentation_supported;

	PSI_HD_MEM_TYPE             nrt_dmacw_default_mem;          // NRT memory used for NRT Buffer (DMACWs)
	PSI_HD_MEM_TYPE             nrt_tx_default_mem;             // NRT memory used for NRT Buffer (TX)
	PSI_HD_MEM_TYPE             nrt_rx_default_mem;             // NRT memory used for NRT Buffer (RX)
	PSI_HD_MEM_TYPE             nrt_tx_low_frag_mem;            // NRT memory used for NRT Buffer (TX fragmentation prio low, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mid_frag_mem;            // NRT memory used for NRT Buffer (TX fragmentation prio mid, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_high_frag_mem;           // NRT memory used for NRT Buffer (TX fragmentation prio high, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mgmtlow_frag_mem;        // NRT memory used for NRT Buffer (TX fragmentation prio MGMTLow, used only by EDDI SOC)
	PSI_HD_MEM_TYPE             nrt_tx_mgmthigh_frag_mem;       // NRT memory used for NRT Buffer (TX fragmentation prio MGMTHigh, used only by EDDI SOC)
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_dmacw_default;    // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_rx_default;       // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_default;       // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_low_frag;      // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mid_frag;      // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_high_frag;     // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmtlow_frag;  // NRT Alloc/Free 
	PSI_USR_NRTMEM_ID_FKT_TYPE  user_mem_fcts_tx_mgmthigh_frag; // NRT Alloc/Free 
} PSI_HD_EDDI_TYPE, * PSI_HD_EDDI_PTR_TYPE;

typedef struct psi_hd_eddp_tag { // HW info for EDDP HDs (PNIP boards)
	LSA_UINT16          icu_location;           // ICU IF for EDDP setup
	LSA_UINT16          hw_type;                // EDDP HW-type for EDDP setup
	LSA_UINT8           appl_timer_mode;        // EDDP IPB CRT Param ApplTimerMode
	LSA_UINT16          appl_timer_reduction_ratio;     // EDDP IPB CRT Param ApplTimerReductionRatio
	LSA_BOOL            is_transfer_end_correction_pos; // EDDP IPB CRT Param isTransferEndCorrectionPos
	LSA_UINT32          transfer_end_correction_value;  // EDDP IPB CRT Param TransferEndCorrectionValue
	PSI_EDDP_BOARD_TYPE board_type;             // type of board
	PSI_HD_MEM_TYPE     pnip;                   // PNIP memory
	PSI_HD_MEM_TYPE     sdram_NRT;              // SDRAM memory NRT range
	PSI_HD_MEM_TYPE     sdram_CRT;              // SDRAM memory CRT range (for IOBUFFER in SDRAM memory)
	PSI_HD_MEM_TYPE     perif_ram;              // PERIF memory (for IO-Buffer in PERIF memory) 
	PSI_HD_MEM_TYPE     k32_tcm;                // K32 memory
	PSI_HD_MEM_TYPE     apb_periph_scrb;        // APB-Periphericals SCRB
	PSI_HD_MEM_TYPE     apb_periph_perif;       // APB-Periphericals PERIF
	LSA_VOID_PTR_TYPE   g_shared_ram_base_ptr;  // Shared mem base ptr for SRD
} PSI_HD_EDDP_TYPE, * PSI_HD_EDDP_PTR_TYPE;

typedef struct psi_hd_edds_tag { // HW info for EDDS HDs
	LSA_VOID_PTR_TYPE  ll_handle;            // LL handle (depends on LL adaption)
	LSA_VOID_PTR_TYPE  ll_function_table;    // LL function table (depends on LL adaption)
	LSA_VOID_PTR_TYPE  ll_parameter;         // LL parameter (depends on LL adaption)
	LSA_VOID_PTR_TYPE  io_buffer_handle;     // IO buffer handle for this HD
} PSI_HD_EDDS_TYPE, * PSI_HD_EDDS_PTR_TYPE;

typedef union psi_hd_edd_type { // HD information for this PN board (EDD specific)
	PSI_HD_EDDI_TYPE   eddi;             // EDDI
	PSI_HD_EDDP_TYPE   eddp;             // EDDP
	PSI_HD_EDDS_TYPE   edds;             // EDDS
} PSI_HD_EDD_TYPE, * PSI_HD_EDD_PTR_TYPE;

typedef struct psi_hd_usr_port_map_tag { // User to HW port mapping information
	LSA_UINT16         hw_port_id;       // HW port number
	LSA_UINT16         hw_phy_nr;        // PHY nr for HW port
} PSI_HD_PORT_MAP_TYPE, * PSI_HD_PORT_MAP_PTR_TYPE;

// Note: the adaption HW info is used for the EDDx setup and adaption functions
typedef struct psi_get_hd_param_tag { // HW information (from system) for one HD
	PSI_SYS_HANDLE              hd_sys_handle;                          // hSysDev for HD
	PSI_SYS_HANDLE              if_sys_handle[PSI_CFG_MAX_HD_IF_CNT];   // hSysDev for HD IF
	PSI_MAC_TYPE                if_mac[PSI_CFG_MAX_HD_IF_CNT];          // all IF MAC from system
	PSI_MAC_TYPE                port_mac[PSI_CFG_MAX_PORT_CNT];         // all port MACs from system
	PSI_HD_PORT_MAP_TYPE        port_map[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // User to HW port mapping (port index 0 is reserved) for each IF
	LSA_UINT16                  port_media_type_select[PSI_CFG_MAX_HD_IF_CNT][PSI_CFG_MAX_PORT_CNT+1];  // media type selector for all ports (port index 0 is reserved) for each IF: PSI_MEDIA_TYPE_SEL_xxx
	LSA_UINT16                  edd_type;                               // EDD HW info belongs to EDD type (Comp-ID)
	PSI_HD_MEM_TYPE             hif_mem;                                // HIF memory
	PSI_HD_MEM_TYPE             pi_mem;                                 // PI memory used for CRT (SDRam, KRAM, IOCC)
	PSI_HD_MEM_TYPE             nrt_mem;                                // NRT memory used for NRT Buffer (RXTX Default) (SDRam, SHM)
	PSI_HD_EDD_TYPE             edd;                                    // HD information for EDDx HD
} PSI_HD_PARAM_TYPE, * PSI_HD_PARAM_PTR_TYPE;

typedef struct psi_get_hd_sys_id_tag { // HD information for SYS adaption 
	PSI_HD_ID_TYPE      hd_location;       // HD address information (PCI, MAC)
	LSA_UINT16          asic_type;         // Expected asic type
	LSA_UINT16          rev_nr;            // Expected rev nr
	LSA_UINT16          edd_type;          // Expected type of EDDx
} PSI_HD_SYS_ID_TYPE, * PSI_HD_SYS_ID_PTR_TYPE;

/*----------------------------------------------------------------------------*/

#ifndef PSI_GET_HD_PARAM
LSA_VOID  PSI_GET_HD_PARAM(
	LSA_UINT16               *ret_val_ptr,        // Getting state
	PSI_HD_INPUT_PTR_TYPE    hd_ptr,              // HD input configuration for one HD
	PSI_HD_SYS_ID_PTR_TYPE   hd_sys_id_ptr,       // HD address params for SYS adaption
	PSI_HD_PARAM_PTR_TYPE    hd_param_ptr         // HD HW params from system
);
#endif

#ifndef PSI_FREE_HD_PARAM
LSA_VOID  PSI_FREE_HD_PARAM(
	LSA_UINT16               *ret_val_ptr,      // Getting state
	LSA_UINT16               hd_id              // HD ID (logical)
);
#endif

#ifndef PSI_LD_CLOSED_HD
LSA_VOID PSI_LD_CLOSED_HD(
	LSA_UINT16 hd_id                            // HD ID (logical)
);
#endif

/*----------------------------------------------------------------------------*/

typedef struct psi_hif_sys_ptr_tag {
	LSA_UINT16           hd_nr;             // HD number [1..N]
	LSA_BOOL             is_upper;          // Upper/Lower Hif instance
	LSA_BOOL             shortcut_mode;     // Upper/Lower in shortcut mode
    PSI_LD_RUNS_ON_TYPE  ldRunsOnType;      // Upper/Lower LD Runs on HW
	LSA_UINT16           hif_lower_handle;  // HIF lower handle (only used for Upper OPEN)
	PSI_HD_SYS_ID_TYPE   hd_sys_id;         // HD location information (for real Open HD)
} PSI_HIF_SYS_TYPE, * PSI_HIF_SYS_PTR_TYPE;

/*----------------------------------------------------------------------------*/
/*  HD IR adaption (PNIO IR)                                                  */
/*----------------------------------------------------------------------------*/

#ifndef PSI_HD_ENABLE_IR
LSA_VOID PSI_HD_ENABLE_IR( 
	PSI_SYS_HANDLE sys_handle
);
#endif

#ifndef PSI_HD_DISABLE_IR
LSA_VOID PSI_HD_DISABLE_IR( 
	PSI_SYS_HANDLE sys_handle
);
#endif

#if ( ( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 ) )
/*----------------------------------------------------------------------------*/
/*  PSI EDDx Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDD_NRT_SEND_HOOK
LSA_VOID PSI_EDD_NRT_SEND_HOOK(
	PSI_SYS_HANDLE          hSysDev,
	LSA_VOID_PTR_TYPE       ptr_upper_rqb );
#endif

#endif

#if ( PSI_CFG_USE_EDDI == 1 ) // EDDI used 
/*----------------------------------------------------------------------------*/
/*  PSI EDDI Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_XPLL_SETPLLMODE_OUT
LSA_VOID  PSI_EDDI_LL_XPLL_SETPLLMODE_OUT(
	LSA_RESULT            * result,
	PSI_SYS_HANDLE          hSysDev,
	LSA_UINT32              pIRTE,
	LSA_UINT32              location,
	LSA_UINT16              pllmode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_BEGIN
LSA_VOID PSI_EDDI_LL_LED_BLINK_BEGIN(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_END
LSA_VOID PSI_EDDI_LL_LED_BLINK_END(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_SET_MODE
LSA_VOID PSI_EDDI_LL_LED_BLINK_SET_MODE(
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT32      HwPortIndex,
	LSA_UINT32      PhyTransceiver,
	LSA_UINT16      LEDMode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID           *  const  p,
	LSA_UINT32         const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC( 
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID          *  const  p,
	LSA_UINT32        const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID             * const  p,
	LSA_UINT32           const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_OFFSET
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_OFFSET( 
	PSI_SYS_HANDLE    hSysDev,
	LSA_VOID          *  const  p,
	LSA_UINT32        const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(
	PSI_SYS_HANDLE     hSysDev,
	LSA_UINT32         const  p,
	LSA_UINT32         const  location );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE( 
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          CycleBaseFactor,
	LSA_UINT8           Mode );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED
LSA_VOID PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( 
	PSI_EDD_HDDB        hDDB );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SCL_LOW_HIGH
LSA_VOID PSI_EDDI_I2C_SCL_LOW_HIGH( 
	PSI_SYS_HANDLE   hSysDev,
	LSA_BOOL         const  Level );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_LOW_HIGH
LSA_VOID PSI_EDDI_I2C_SDA_LOW_HIGH( 
	PSI_SYS_HANDLE    hSysDev,
	LSA_BOOL          const  Level );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_READ
LSA_VOID PSI_EDDI_I2C_SDA_READ( 
	PSI_SYS_HANDLE    hSysDev ,
	LSA_UINT8         *const  value_ptr );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_IN_OUT
LSA_VOID PSI_EDDI_I2C_SDA_IN_OUT( 
	PSI_SYS_HANDLE    hSysDev ,
	LSA_BOOL          const  OutPut );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SCL_IN_OUT
LSA_VOID PSI_EDDI_I2C_SCL_IN_OUT( 
	PSI_SYS_HANDLE    hSysDev ,
	LSA_BOOL          const  OutPut );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SELECT
LSA_VOID PSI_EDDI_I2C_SELECT( 
	LSA_BOOL        * const  ret_val_ptr,
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      const  PortId,
	LSA_UINT16      const  I2CMuxSelect );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC
LSA_VOID PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC( 
	LSA_RESULT       * const  ret_val_ptr,
	PSI_SYS_HANDLE   hSysDev,
	LSA_UINT8        const  I2CDevAddr,
	LSA_UINT8        const  I2COffsetCnt,
	LSA_UINT8        const  I2COffset1,
	LSA_UINT8        const  I2COffset2,
	LSA_UINT32       const  Size,
	LSA_UINT8       *const  pBuf );
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_READ_OFFSET_SOC
LSA_VOID PSI_EDDI_LL_I2C_READ_OFFSET_SOC( 
	LSA_RESULT       * const  ret_val_ptr,
	PSI_SYS_HANDLE   hSysDev,
	LSA_UINT8        const  I2CDevAddr,
	LSA_UINT8        const  I2COffsetCnt,
	LSA_UINT8        const  I2COffset1,
	LSA_UINT8        const  I2COffset2,
	LSA_UINT32       const  Size,
	LSA_UINT8       *const  pBuf );
#endif

/*----------------------------------------------------------------------------*/

//ExtTimer commands
#define PSI_EDDI_EXTTIMER_CMD_INIT_START    1
#define PSI_EDDI_EXTTIMER_CMD_STOP          2

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_EXT_TIMER_CMD
LSA_VOID PSI_EDDI_EXT_TIMER_CMD(
	PSI_SYS_HANDLE  const hSysDev, 
	LSA_UINT8       const Cmd,
	PSI_EDD_HDDB    const hDDB );
#endif

#endif  // EDDI HW adaption

#if ( PSI_CFG_USE_EDDP == 1 ) // EDDP used 
/*----------------------------------------------------------------------------*/
/*  PSI EDDP Interface                                                        */
/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDP_ALLOC_UPPER_RX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_RX_MEM(
	PSI_SYS_HANDLE     hSysIF,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_RX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysIF,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_ALLOC_UPPER_TX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_TX_MEM(
	PSI_SYS_HANDLE     hSysIF,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_TX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysIF,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_ALLOC_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_DEV_MEM(
	PSI_SYS_HANDLE     hSysDev,
	LSA_VOID_PTR_TYPE  *upper_mem_ptr_ptr,
	LSA_UINT32         length );
#endif

#ifndef PSI_EDDP_FREE_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif

#ifndef PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID  PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysIF,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysIF,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysDev,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysIF,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE              hSysIF,
	LSA_VOID_PTR_TYPE           Ptr,
	LSA_UINT32                  * pAddr );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_TEST_WRITE_REG
LSA_VOID PSI_EDDP_TEST_WRITE_REG(
	PSI_SYS_HANDLE     hSysDev,
	LSA_UINT32         Reg,
	LSA_UINT32         Value );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_K32_RESET
LSA_VOID PSI_EDDP_K32_RESET(
	PSI_SYS_HANDLE      hSysDev,
	LSA_BOOL            on );
#endif

/*----------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NEC_MAU_TYPE_INTERNAL
LSA_VOID  PSI_EDDP_PHY_NEC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16              *pMauType );

/*---------------------------------------------------------------------------*/
LSA_VOID  PSI_EDDP_PHY_NEC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32              *pSpeed,
    LSA_UINT32              *pDuplexity );
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NEC_LED_BLINK_INTERNAL
LSA_VOID  PSI_EDDP_PHY_NEC_LED_BlinkBegin(
	PSI_EDD_HDDB 	    hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID );

/*---------------------------------------------------------------------------*/
LSA_VOID  PSI_EDDP_PHY_NEC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode );

/*---------------------------------------------------------------------------*/
LSA_VOID  PSI_EDDP_PHY_NEC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID );
#endif

/*----------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NSC_MAU_TYPE_INTERNAL
LSA_VOID  PSI_EDDP_PHY_NSC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16              *pMauType );

LSA_VOID  PSI_EDDP_PHY_NSC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32            * pSpeed,
    LSA_UINT32            * pDuplexity );
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDP_CFG_PHY_NSC_LED_BLINK_INTERNAL
LSA_VOID  PSI_EDDP_PHY_NSC_LED_BlinkBegin(
    PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID );

/*---------------------------------------------------------------------------*/
LSA_VOID  PSI_EDDP_PHY_NSC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode );

/*---------------------------------------------------------------------------*/
LSA_VOID  PSI_EDDP_PHY_NSC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE( 
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          CycleBaseFactor,
	LSA_UINT8           Mode );
#endif

/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDP_I2C_SELECT
LSA_VOID PSI_EDDP_I2C_SELECT(
	LSA_BOOL       * const ret_val_ptr,
	PSI_SYS_HANDLE         hSysDev,
	LSA_UINT16       const PortId,
	LSA_UINT16       const I2CMuxSelect );
#endif

/*----------------------------------------------------------------------------*/
#endif  // EDDP HW adaption

#if ( PSI_CFG_USE_EDDS == 1 ) //EDDS Used
/*----------------------------------------------------------------------------*/
/*  PSI EDDS Interface                                                        */
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR_UPPER(
	PSI_SYS_HANDLE                       hSysDev,
	LSA_VOID_PTR_TYPE                    Ptr,
	LSA_UINT32                         * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_PTR_TO_ADDR64_UPPER
LSA_VOID PSI_EDDS_PTR_TO_ADDR64_UPPER(
	PSI_SYS_HANDLE                       hSysDev,
	LSA_VOID_PTR_TYPE                    Ptr,
	LSA_UINT64                         * pAddr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM
LSA_VOID  PSI_EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#else
#error "by design a function!"
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM
LSA_VOID  PSI_EDDS_FREE_RX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM
LSA_VOID  PSI_EDDS_ALLOC_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM
LSA_VOID  PSI_EDDS_FREE_TX_TRANSFER_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_ALLOC_DEV_BUFFER_MEM
LSA_VOID  PSI_EDDS_ALLOC_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length );
#endif
/*----------------------------------------------------------------------------*/
#ifndef PSI_EDDS_FREE_DEV_BUFFER_MEM
LSA_VOID  PSI_EDDS_FREE_DEV_BUFFER_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr );
#endif
/*----------------------------------------------------------------------------*/

#endif // PSI_CFG_USE_EDDS

#if ( PSI_CFG_USE_POF == 1 ) // POF used 
/*----------------------------------------------------------------------------*/
/*  PSI POF Interface                                                         */
/*----------------------------------------------------------------------------*/

#ifndef PSI_POF_FO_LED
LSA_VOID PSI_POF_FO_LED(
	LSA_UINT16        const PortID,
	LSA_BOOL          const ON,
	PSI_SYS_HANDLE    hSysDev );
#endif

/*----------------------------------------------------------------------------*/
#endif  // POF HW adaption

#if ( PSI_CFG_USE_HIF == 1 ) // HIF used 
/*----------------------------------------------------------------------------*/
/*  PSI HIF Interface                                                         */
/*----------------------------------------------------------------------------*/

/* HIF LD */

#ifndef PSI_HIF_GET_LD_UPPER_HANDLE
LSA_VOID PSI_HIF_GET_LD_UPPER_HANDLE( 
	LSA_UINT16  *hif_handle_ptr );
#endif

#ifndef PSI_HIF_LD_U_GET_PARAMS
LSA_VOID PSI_HIF_LD_U_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_LD_L_GET_PARAMS
LSA_VOID PSI_HIF_LD_L_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_LD_ENABLE_ISR
LSA_VOID PSI_HIF_LD_ENABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_DISABLE_ISR
LSA_VOID PSI_HIF_LD_DISABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_SEND_IRQ
LSA_VOID PSI_HIF_LD_SEND_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef  PSI_HIF_LD_ACK_IRQ
LSA_VOID PSI_HIF_LD_ACK_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_LD_REQUEST_UPPER_DONE
LSA_VOID PSI_HIF_LD_REQUEST_UPPER_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_request_upper_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb, 
	LSA_SYS_PTR_TYPE                 sys_ptr );
#endif

#ifndef PSI_HIF_LD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_L_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb );
#endif

#ifndef PSI_HIF_LD_U_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_LD_U_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE   hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                 ptr_upper_rqb );
#endif

/* HIF HD */

#ifndef PSI_HIF_GET_HD_LOWER_HANDLE
LSA_VOID PSI_HIF_GET_HD_LOWER_HANDLE( 
	LSA_UINT16      *hif_handle_ptr,
	LSA_UINT16      hd_id );
#endif

#ifndef PSI_HIF_HD_U_GET_PARAMS
LSA_VOID PSI_HIF_HD_U_GET_PARAMS( 
	LSA_UINT16            *ret_val_ptr,
	PSI_SYS_HANDLE        hSysDev,
	LSA_VOID_PTR_TYPE     pPars );
#endif

#ifndef PSI_HIF_HD_L_GET_PARAMS
LSA_VOID PSI_HIF_HD_L_GET_PARAMS( 
	LSA_UINT16           *ret_val_ptr,
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID_PTR_TYPE    pPars );
#endif

#ifndef PSI_HIF_HD_ENABLE_ISR
LSA_VOID PSI_HIF_HD_ENABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_DISABLE_ISR
LSA_VOID PSI_HIF_HD_DISABLE_ISR( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_SEND_IRQ
LSA_VOID PSI_HIF_HD_SEND_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef  PSI_HIF_HD_ACK_IRQ
LSA_VOID PSI_HIF_HD_ACK_IRQ( 
	PSI_SYS_HANDLE  hSysDev,
	LSA_UINT16      hH );
#endif

#ifndef PSI_HIF_HD_L_SYSTEM_REQUEST_DONE
LSA_VOID PSI_HIF_HD_L_SYSTEM_REQUEST_DONE( 
	PSI_UPPER_CALLBACK_FCT_PTR_TYPE  hif_system_request_done_ptr,
	LSA_VOID_PTR_TYPE                ptr_upper_rqb );
#endif

/*----------------------------------------------------------------------------*/
#endif  // HIF adaption (Application or FW System)

#if (( PSI_CFG_USE_SOCK == 1 ) && ( PSI_CFG_USE_TCIP == 0 )) // SOCK without TCIP used 
/*----------------------------------------------------------------------------*/
/*  PSI SOCK not TCIP Interface                                               */
/*----------------------------------------------------------------------------*/

typedef struct psi_sock_name_tag  /* Note see sock_int.h for SOCK Type */
{
	LSA_UINT8   *pName; 
	LSA_UINT16   Length; 
	LSA_BOOL     Remanent;
} PSI_SOCK_NAME_TYPE;

#ifndef PSI_SOCK_SET_IPSUITE
LSA_BOOL PSI_SOCK_SET_IPSUITE(
	LSA_UINT32  IfID,
	LSA_UINT32  ip_addr,
	LSA_UINT32  net_mask,
	LSA_UINT32  gateway ); 
#endif

#ifndef PSI_SOCK_SET_MIB2_IFGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_IFGROUP_DATA(
	LSA_UINT32          IfID,
	LSA_UINT16          PortId, 
	PSI_SOCK_NAME_TYPE  IfDescr );
#endif

#ifndef PSI_SOCK_SET_MIB2_SYSGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_SYSGROUP_DATA( 
	PSI_SOCK_NAME_TYPE SysName,
	PSI_SOCK_NAME_TYPE SysDescr,
	PSI_SOCK_NAME_TYPE SysContact,
	PSI_SOCK_NAME_TYPE SysLocation );
#endif

#ifndef PSI_SOCK_GET_MIB2_IFINDEX
LSA_BOOL PSI_SOCK_GET_MIB2_IFINDEX(
	LSA_UINT32    IfID, 
	LSA_UINT16    PortID, 
	LSA_UINT32*   p_IfIndex );
#endif

#ifndef PSI_SOCK_TCPIP_SET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_SET_TIMEPARAMS(
	LSA_UINT32  KeepAliveTime,
	LSA_UINT32  ConnectionTimeout,
	LSA_UINT32  RetransTimeout,
	LSA_UINT32  RetransCount );
#endif

#ifndef PSI_SOCK_TCPIP_GET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_GET_TIMEPARAMS(
	LSA_UINT32  *KeepAliveTime,
	LSA_UINT32  *ConnectionTimeout,
	LSA_UINT32  *RetransTimeout,
	LSA_UINT32  *RetransCount );
#endif

/*----------------------------------------------------------------------------*/
#endif  // SOCK no TCIP adaption (Application or FW System)

#if ( PSI_CFG_USE_IOH == 1 ) // IOH used 
/*----------------------------------------------------------------------------*/
/*  PSI IOH Interface                                                         */
/*----------------------------------------------------------------------------*/

#ifndef PSI_IOH_FREE_FAST_MEM
LSA_VOID  PSI_IOH_FREE_FAST_MEM(
	LSA_UINT16      *ret_val_ptr,
	LSA_VOID        *local_mem_ptr );
#endif

#ifndef PSI_IOH_ALLOC_FAST_MEM
LSA_VOID  PSI_IOH_ALLOC_FAST_MEM(
	LSA_VOID      **local_mem_ptr_ptr,
	LSA_UINT32      length );
#endif

#ifndef PSI_IOH_FREE_NRT_MEM
LSA_VOID  PSI_IOH_FREE_NRT_MEM(
	LSA_SYS_PTR_TYPE       hSysDev,
    LSA_UINT16      * ret_val_ptr,
    LSA_VOID        * local_mem_ptr );
#endif

#ifndef PSI_IOH_ALLOC_NRT_MEM
LSA_VOID  PSI_IOH_ALLOC_NRT_MEM(
	LSA_SYS_PTR_TYPE       hSysDev,
    LSA_VOID      ** local_mem_ptr_ptr,
    LSA_UINT32       length );
#endif

#ifndef PSI_IOH_ENABLE_ISOCHRONOUS
LSA_VOID PSI_IOH_ENABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE  hSysDev,
	LSA_VOID (*func_ptr)(LSA_INT params, LSA_VOID* args),
	LSA_INT32 param,
	LSA_VOID* args );
#endif

#ifndef PSI_IOH_DISABLE_ISOCHRONOUS
LSA_VOID PSI_IOH_DISABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE      hSysDev );
#endif

#ifndef PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE       hSysDev,       //device info
	LSA_VOID             * const p,     //virtual address
  	LSA_BOOL             bShared );     //LSA_TRUE: calculated for SharedMEM, LSA_FALSE: calculated for LocalMEM
#endif

#ifndef PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY
LSA_BOOL PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY(
    PSI_SYS_HANDLE      hSysDev,        //device info
    LSA_UINT32          iocc_base_addr );
#endif

/*----------------------------------------------------------------------------*/
#endif  // IOH adaption (Application or FW System)

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PSI_SYS_H_ */

