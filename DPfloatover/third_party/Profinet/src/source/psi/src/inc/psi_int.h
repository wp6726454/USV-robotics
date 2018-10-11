#ifndef PSI_INT_H
#define PSI_INT_H

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
/*  F i l e               &F: psi_int.h                                 :F&  */
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

#include "psi_cfg.h"        // Base system

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "ltrc_if.h"

#include "psi_usr.h"
#include "psi_sys.h"

#include "psi_trc.h"        // Tracing
#include "psi_res_calc.h"   // Resource calulation
#include "psi_pnstack.h"    // PN-Stack adaption

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* stringify                                                                 */
/*---------------------------------------------------------------------------*/

#define PSI_STRINGIFY_(n)  # n
#define PSI_STRINGIFY(n)   PSI_STRINGIFY_(n) /* make it a string */

/*---------------------------------------------------------------------------*/
/* pragma message                                                            */
/* intended usage: #pragma PSI_MESSAGE(...)                                  */
/*---------------------------------------------------------------------------*/

/*
#define PSI_MESSAGE(str_) message( __FILE__ "(" PSI_STRINGIFY(__LINE__) ") : " str_) //(LaM)PSI_MESSAGE only for developers!
*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

#define psi_is_null( ptr )        (LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define psi_is_not_null(ptr)      (!LSA_HOST_PTR_ARE_EQUAL((ptr), LSA_NULL))
#define psi_are_equal(ptr1,ptr2)  (LSA_HOST_PTR_ARE_EQUAL((ptr1), (ptr2)))

/* using for adaption of all <comp>_ASSERT, if no own ASSERT is avaliable */
#define PSI_FATAL_COMP( comp_id, mod_id, error_code_0 )	{\
	psi_fatal_error( comp_id, mod_id, (LSA_UINT16)__LINE__,\
			(LSA_UINT32)(error_code_0), 0, 0, 0, 0, LSA_NULL\
		);\
	}

/*===========================================================================*/
/*    prototypes for common used functions                                   */
/*===========================================================================*/

LSA_VOID_PTR_TYPE psi_alloc_local_mem( LSA_UINT32 length );
LSA_VOID psi_free_local_mem( LSA_VOID_PTR_TYPE  mem_ptr );

#if (PSI_CFG_USE_SOCK == 1)
PSI_SOCK_INPUT_PTR_TYPE psi_ld_get_sock_details( LSA_VOID );
#endif

LSA_BOOL psi_get_mrp_default_rports( 
	LSA_UINT16             if_nr, 
	PSI_HD_INPUT_PTR_TYPE  hd_input_ptr,
	LSA_UINT8              *rport1_ptr,
	LSA_UINT8              *rport2_ptr );

PSI_HD_PORT_INPUT_PTR_TYPE psi_get_hw_port_entry_for_user_port( 
	LSA_UINT16             if_nr,
	LSA_UINT16             usr_port_nr,
	PSI_HD_INPUT_PTR_TYPE  hd_input_ptr );

/*===========================================================================*/
/*    prototypes for conversion functions                                    */
/*===========================================================================*/

#ifndef PSI_SWAP_U16
LSA_VOID   PSI_SWAP_U16(
	LSA_UINT16  * short_ptr ); /* correct alignment! */
#endif

#ifndef   PSI_SWAP_U32
LSA_VOID PSI_SWAP_U32(
	LSA_UINT32  * long_ptr ); /* correct alignment! */
#endif

#ifndef   PSI_SWAP_U64
LSA_VOID PSI_SWAP_U64(
	LSA_UINT64  * long_long_ptr ); /* correct alignment! */
#endif

#ifndef PSI_NTOH16
	LSA_UINT16 PSI_NTOH16(LSA_UINT16 v);
#endif

#ifndef PSI_HTON16
	LSA_UINT16 PSI_HTON16(LSA_UINT16 v);
#endif

#ifndef PSI_NTOH32
	LSA_UINT32 PSI_NTOH32(LSA_UINT32 v);
#endif

#ifndef PSI_HTON32
	LSA_UINT32 PSI_HTON32(LSA_UINT32 v);
#endif

#ifndef PSI_NTOH64
	LSA_UINT64 PSI_NTOH64(LSA_UINT64 v);
#endif

#ifndef PSI_HTON64
	LSA_UINT64 PSI_HTON64(LSA_UINT64 v);
#endif

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_INT_H_ */
