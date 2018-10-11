#ifndef TCIP_CFG_H                       /* ----- reinclude-protection ----- */
#define TCIP_CFG_H

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
/*  F i l e               &F: tcip_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration module                                                     */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*=== pcIOX configuration (do not copy/paste it) ============================*/
/*===========================================================================*/

void tcip_psi_startstop(int start);

#define TCIP_DEBUG  PSI_DEBUG

/*===========================================================================*/
/*===========================================================================*/

#ifndef TCIP_CFG_TRACE_MODE
#error "TCIP_CFG_TRACE_MODE not defined"
#endif

#ifndef TCIP_FILE_SYSTEM_EXTENSION
# define TCIP_FILE_SYSTEM_EXTENSION(module_id_)  /* nothing */
#endif

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_COPY_ON_SEND   PSI_TCIP_CFG_COPY_ON_SEND //Using COPY IF for TCIP

/*===========================================================================*/
/*===========================================================================*/

#if defined(LSA_HOST_ENDIANESS_BIG)

#define TCIP_CFG_BIG_ENDIAN	 1 /* 0 == little endian, 1 == big endian */

#endif

#if defined(LSA_HOST_ENDIANESS_LITTLE)

#define TCIP_CFG_BIG_ENDIAN	 0

#endif

/*===========================================================================*/
/*===========================================================================*/

#ifndef TCIP_CFG_MAX_SOCKETS
#error "TCIP_CFG_MAX_SOCKETS not defined"
#endif

#define TCIP_CFG_MAXARPS  LSA_MAX((PSI_CFG_MAX_DIAG_TOOLS + ((PSI_MAX_CL_DEVICES + (PSI_MAX_SV_DEVICES * PSI_MAX_SV_AR)) * PSI_CFG_MAX_PNIO_IF_CNT)),32) /* maximum mumber of ARP table entries, at least 32 */

#define TCIP_CFG_SYS_SERVICES  0x4C /* 0x4C == 0b01001100 == Layer 7+4+3, see RFC3418, sysServices */

#define TCIP_CFG_NUMBIGBUFS  150 /* number of packets of size BIGBUFSIZE */
#define TCIP_CFG_NUMLILBUFS   50 /* number of packets of size LILBUFSIZE */

#define TCIP_CFG_COMPRESS_RCV_MBUFS  0 /* off, see sockbuf define SB_MBCOMP */

#define TCIP_CFG_NPDEBUG  0 /* off */

#ifndef TCIP_CFG_ENABLE_MULTICAST
# error "TCIP_CFG_ENABLE_MULTICAST not defined"
#endif

/*===========================================================================*/
/*===========================================================================*/

#define TCIP_CFG_QUEUE_CHECK_ENABLE  0 /* see macro QUEUE_CHECK */

#define TCIP_CFG_SOC_CHECK_ENABLE    0 /* see macro SOC_CHECK */

#define TCIP_CFG_SOC_CHECK_MORE      0 /* 1 enables SOC_CHECK in t_recv(), t_send(), t_select() */

/*===========================================================================*/
/*===========================================================================*/

#if TCIP_DEBUG

#define TCIP_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */

/* check if condition holds or call fatal (programming error) */
#define TCIP_ASSERT(cond_) { if (!(cond_)) { TCIP_FATAL(); } }

#else

#define TCIP_TEST_POINTER(ptr_)  (ptr_) /* don't test */

#define TCIP_ASSERT(cond_) { /* empty-block in release version */ }

#endif


/*===========================================================================*/
/* AP01358814 moved prototypes to config header                              */
/*===========================================================================*/
void  tcp_sleep(void * timeout);
void  tcp_wakeup(void * wake);

void LOCK_NET_RESOURCE(int res);
void UNLOCK_NET_RESOURCE(int res);


/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*
 * all known platforms have a "flat" memory-model
 */

#define TCIP_FCTATTR_FLAT  /* nothing */
#define TCIP_MEMATTR_FLAT  /* nothing */

/*
 * code attributes
 */

#define TCIP_UPPER_IN_FCT_ATTR        TCIP_FCTATTR_FLAT  /* tcip-user to tcip */
#define TCIP_UPPER_OUT_FCT_ATTR       TCIP_FCTATTR_FLAT  /* tcip to tcip-user */

#define TCIP_EDD_LOWER_IN_FCT_ATTR    TCIP_FCTATTR_FLAT  /* lower component to tcip */
#define TCIP_EDD_LOWER_OUT_FCT_ATTR   TCIP_FCTATTR_FLAT  /* tcip to lower component */

#define TCIP_SYSTEM_IN_FCT_ATTR       TCIP_FCTATTR_FLAT  /* system to tcip */
#define TCIP_SYSTEM_OUT_FCT_ATTR      TCIP_FCTATTR_FLAT  /* tcip to system */

#define TCIP_LOCAL_FCT_ATTR           TCIP_FCTATTR_FLAT  /* local function */

/*
 * data attributes
 */

#define TCIP_UPPER_RQB_ATTR       TCIP_MEMATTR_FLAT  /* upper-rqb data */
#define TCIP_UPPER_MEM_ATTR       TCIP_MEMATTR_FLAT  /* upper-memory data */

#define TCIP_EDD_LOWER_RQB_ATTR   TCIP_MEMATTR_FLAT  /* lower-rqb data */
#define TCIP_EDD_LOWER_MEM_ATTR   TCIP_MEMATTR_FLAT  /* lower-memory data */

#define TCIP_SYSTEM_MEM_ATTR      TCIP_MEMATTR_FLAT  /* system-memory data */

#define TCIP_LOCAL_MEM_ATTR       TCIP_MEMATTR_FLAT  /* local data */

#define TCIP_COMMON_MEM_ATTR      TCIP_MEMATTR_FLAT  /* greatest common data attribute */

/*===========================================================================*/
/*===========================================================================*/

/*
 * TCIP-HEADER and TCIP-TRAILER
 */

#if ! defined(LSA_RQB_HEADER) || ! defined(LSA_RQB_TRAILER)
# error "LSA_RQB_HEADER or LSA_RQB_TRAILER not defined in lsa_cfg.h"
#endif


#define TCIP_RQB_HEADER \
	LSA_RQB_HEADER(TCIP_UPPER_RQB_PTR_TYPE) /* standard LSA-header comes first */ \
	/* end of TCIP_RQB_HEADER */


#define TCIP_RQB_TRAILER \
	LSA_RQB_TRAILER /* standard LSA-trailer comes last */ \
	/* end of TCIP_RQB_TRAILER */


#define TCIP_RQB_SET_NEXT_RQB_PTR(rb_, val_)	LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define TCIP_RQB_GET_NEXT_RQB_PTR(rb_)			LSA_RQB_GET_NEXT_RQB_PTR(rb_)

#define TCIP_RQB_SET_PREV_RQB_PTR(rb_, val_)	LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define TCIP_RQB_GET_PREV_RQB_PTR(rb_)			LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define TCIP_RQB_SET_OPCODE(rb_, val_)			LSA_RQB_SET_OPCODE(rb_, val_)
#define TCIP_RQB_GET_OPCODE(rb_)				LSA_RQB_GET_OPCODE(rb_)

#define TCIP_RQB_SET_HANDLE(rb_, val_)			LSA_RQB_SET_HANDLE(rb_, val_)
#define TCIP_RQB_GET_HANDLE(rb_)				LSA_RQB_GET_HANDLE(rb_)

#define	TCIP_RQB_SET_USER_ID(rb_, val_)			LSA_RQB_SET_USER_ID(rb_, val_)
#define	TCIP_RQB_GET_USER_ID(rb_)				LSA_RQB_GET_USER_ID(rb_)

#define TCIP_RQB_SET_USER_ID_PTR(rb_, val_)		LSA_RQB_SET_USER_ID_PTR(rb_, val_)
#define TCIP_RQB_GET_USER_ID_PTR(rb_)			LSA_RQB_GET_USER_ID_PTR(rb_)

#define TCIP_RQB_SET_RESPONSE(rqb_, v_)			LSA_RQB_SET_RESPONSE(rqb_, v_)
#define TCIP_RQB_GET_RESPONSE(rqb_)				LSA_RQB_GET_RESPONSE(rqb_)

/*===========================================================================*/
/*===========================================================================*/

/*
 * lower layer macros for EDD
 */

#define TCIP_EDD_INVALID_HANDLE	0xFF /* EDD doesn't export one */

#define TCIP_EDD_LOWER_RQB_TYPE	EDD_RQB_TYPE

#define TCIP_EDD_SET_NEXT_RQB_PTR(rb_, val_)	EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define TCIP_EDD_SET_PREV_RQB_PTR(rb_, val_)	EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define TCIP_EDD_SET_OPCODE(rb_, opc_, svc_)	{ EDD_RQB_SET_OPCODE(rb_, opc_); EDD_RQB_SET_SERVICE(rb_, svc_); }
#define TCIP_EDD_SET_HANDLE(rb_, val_)			EDD_RQB_SET_HANDLE(rb_, val_)
#define TCIP_EDD_SET_USER_ID_PTR(rb_, val_)		EDD_RQB_SET_USERID_PTR(rb_, val_)
#define TCIP_EDD_SET_RESPONSE(rb_, val_)		EDD_RQB_SET_RESPONSE(rb_, val_)

#define TCIP_EDD_GET_NEXT_RQB_PTR(rb_)			EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define TCIP_EDD_GET_PREV_RQB_PTR(rb_)			EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define TCIP_EDD_GET_OPCODE(rb_)				EDD_RQB_GET_OPCODE(rb_)
#define TCIP_EDD_GET_SERVICE(rb_)				EDD_RQB_GET_SERVICE(rb_)
#define TCIP_EDD_GET_HANDLE(rb_)				EDD_RQB_GET_HANDLE(rb_)
#define TCIP_EDD_GET_USER_ID_PTR(rb_)			EDD_RQB_GET_USERID_PTR(rb_)
#define TCIP_EDD_GET_RESPONSE(rb_)				EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_CFG_H (reinclude protection) */
