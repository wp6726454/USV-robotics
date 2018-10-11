#ifndef PND_PSI_CFG_PLF_WINDOWS_WPCAP_INTERNICHE_H
#define PND_PSI_CFG_PLF_WINDOWS_WPCAP_INTERNICHE_H

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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_psi_cfg_plf_windows_wpcap_interniche.h:F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS Platform defintions                                              */
/*                                                                           */
/*****************************************************************************/

/* included by "psi_cfg.h" */
#include "eps_plf_types.h"  /* for defines of EPS_PLATFORM and EPS_PLF */

/*===========================================================================*/
/*        Platform settings                                                  */
/*===========================================================================*/
/* EPS_PLATFORM_PNDRIVER and EPS_PLF_WINDOWS_X86 are defined in <eps_plf_types.h> */
#define EPS_PLATFORM    EPS_PLATFORM_PNDRIVER
#define EPS_PLF         EPS_PLF_WINDOWS_X86

/* extra warnings for compile level 3 MsDev */
#if defined(_MSC_VER)
#pragma warning(3:4032) /* formal parameter 'number' has different type when promoted */
#pragma warning(3:4130) /* 'operator ' : logical operation on address of string constant */
#pragma warning(3:4131) /* 'function' : uses old-style declarator */

#pragma warning(3:4209) /* nonstandard extension used : benign typedef redefinition */
#pragma warning(3:4727) /* conditional expression is constant */

#pragma warning(3:4189) /* 'identifier' : local variable is initialized but not referenced */

#pragma warning(3:4100) /* Unreferenced formal parameter */
#pragma warning(3:4101) /* Unreferenced local variable */
#pragma warning(3:4121) /* structure is sensitive to alignment */
#pragma warning(3:4389) /* '!=' : signed/unsigned mismatch */
#pragma warning(3:4701) /* local may be used w/o init */
// #pragma warning(3:4702) /* Unreachable code */
#pragma warning(3:4705) /* Statement has no effect */

// #pragma warning(3:4242) /* 'identifier' : conversion from 'type1' to 'type2', possible loss of data */
// #pragma warning(3:4244) /* '=/+=' : conversion from 'int' to 'uint16_t', possible loss of data */
#pragma warning(3:4254) /* 'operator' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(3:4289) /* nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope */
#pragma warning(3:4389) /* An operation involved signed and unsigned variables. This could result in a loss of data. */
#pragma warning(3:4431) /* missing type specifier - int assumed. Note: C no longer supports default-int */

#pragma warning(error:4002) /* Zu viele uebergebene Parameter für das Makro 'XXX' */
#pragma warning(error:4013) /* 'function' undefined; assuming extern returning int */
#pragma warning(error:4020) /* zu viele parameter */
#pragma warning(error:4024) /* different types for formal and actual parameter */
#pragma warning(error:4028) /* formal parameter 'number' different from declaration */
#pragma warning(error:4029) /* declared formal parameter list different from definition */
#pragma warning(error:4033) /* 'XXX' muss einen Wert zurueckgeben */
#pragma warning(error:4047) /* differs in levels of indirection  */
#pragma warning(error:4133) /* incompatible types */
#pragma warning(error:4553) /* '==' : Operator hat keine Auswirkungen; ist '=' beabsichtigt? */
#pragma warning(error:4700) /* warning C4700: uninitialized local variable 'XXX' used */
#pragma warning(error:4706) /* assignment w/i conditional expression */
#pragma warning(error:4709) /* command operator w/o index expression */

#pragma warning(disable/*once*/:4514) /* optimizer removed an inline function that is not called */

#endif /* defined(_MSC_VER) */
// #define _DEBUG_

#if defined(_DEBUG)
  #define PSI_DEBUG     1
#else
  #define PSI_DEBUG     0
#endif

#if defined(_DEBUG)
  #define EPS_DEBUG     1
#else
  #define EPS_DEBUG     0
#endif


/*---------------------------------------------------------------------------*/
/* Common defines valid for all targets                                      */
/*---------------------------------------------------------------------------*/

// suppress printf
#define EPS_SUPPRESS_PRINTF

// Do not use HIF serialisation
#define EPS_LOCAL_PN_USER


#define PSI_CFG_MAX_HD_CNT       1 /* Max number of HD(HardwareDevices) [1..N] for one LD instance */
#define PSI_CFG_MAX_HD_IF_CNT    1 /* Max number of IF(Interfaces) [1..4] for one HD instance      */
#define PSI_CFG_MAX_PNIO_IF_CNT  1 /* Maximum number of PNIO interfaces of all EDDs integrated     */
#define PSI_CFG_MAX_PORT_CNT     1 /* Maximum number of ports supported by EDD API services        */

#define PSI_TCIP_CFG_COPY_ON_SEND   1 //Using COPY IF for TCIP

/* Enable the automatic filling up to 3 bytes with 0 in NRT-Send-Buffer by EDDI     */
/* Switching on is only allowed, if PSI_TCIP_CFG_COPY_ON_SEND is defined!           */
#define PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT

#define PSI_CFG_MAX_SOCK_APP_CHANNELS           8 /* Maximum number of user application sock channels, Values (1..8) */
#define PSI_CFG_MAX_DIAG_TOOLS                  8 /* Maximum number of diagnosis tools used in the system */
#define PSI_CFG_SOCK_MULTICAST_SUPPORT          0 /* No Multicasts */

#define PSI_CFG_CM_CFG_SIMATIC_B000             1
#define PSI_CFG_CM_CFG_CL_ALLOW_DIAG_AND_PDEV   1

// Compile Keys for CM/SNMPX role specific features
#define PSI_CFG_USE_IOC        1
#define PSI_CFG_USE_IOM        0
#define PSI_CFG_USE_IOD        0
#define PSI_CFG_USE_IOD_MC     0     // don't activiate, feature not release
#define PSI_CFG_USE_SNMPX_MGR  0
#define PSI_CFG_USE_SNMPX_AGT  0     // don't activiate, feature not release

// Compile Keys PNIO component role and specific
// Note: 0 is off, 1 used on HOST, 2 used on embedded Testsystem

#define PSI_CFG_USE_ACP     1 // must be set equal to CM
#define PSI_CFG_USE_CLRPC   1
#define PSI_CFG_USE_CM      1
#define PSI_CFG_USE_DCP     1
#define PSI_CFG_USE_EDDI    0
#define PSI_CFG_USE_EDDP    0
#define PSI_CFG_USE_EDDS    1
#define PSI_CFG_USE_GSY     0
#define PSI_CFG_USE_HIF     1
#define PSI_CFG_USE_HIF_HD  1
#define PSI_CFG_USE_IOH     0
#define PSI_CFG_USE_LLDP    1
#define PSI_CFG_USE_MRP     0
#define PSI_CFG_USE_NARE    1
#define PSI_CFG_USE_OHA     1
#define PSI_CFG_USE_POF     0
#define PSI_CFG_USE_PNTRC   1
#define PSI_CFG_USE_SNMPX   0
#define PSI_CFG_USE_SOCK    1
#define PSI_CFG_USE_TCIP    1  // Interniche IP-Stack

#define PSI_ALLOW_NONE_PNIO_TYPES  1  // Allow 10MB Mau-Types

#define PSI_MAX_CL_DEVICES     16       // Max devices for create client
#define PSI_MAX_CL_DEVICE_NR   2048      // Max devic Nr [1..2048] for the client
#define PSI_MAX_CL_RECORD_SIZE (32*1024) // Max Record size = 32K
#define PSI_MAX_CL_READ_IMPL   16
#define PSI_MAX_CL_OSU_DEVICES 0

#define PSI_MAX_MC_DEVICES   0
#define PSI_MAX_MC_DEVICE_NR 0

#define PSI_MAX_SV_DEVICES     0
#define PSI_MAX_SV_READ_IMPL   0
#define PSI_MAX_SV_AR          0
#define PSI_MAX_SV_RECORD_SIZE 0


#if ( PSI_CFG_USE_HIF == 0 )
#undef  PSI_CFG_USE_HIF_HD
#define PSI_CFG_USE_HIF_HD 0
#endif

/*============================================================================
 *       Endianess:
 * If you have a computer system whitch stores the most significant byte at the
 * lowest address of the word/doubleword: Define this
 * #define LSA_HOST_ENDIANESS_BIG
 * else define this
 * #define LSA_HOST_ENDIANESS_LITTLE
 *===========================================================================*/

#undef  LSA_HOST_ENDIANESS_BIG
#define LSA_HOST_ENDIANESS_LITTLE

/*------------------------------------------------------------------------------
// interface to LSA trace
//  0 .. no LSA trace
//  1 .. LSA trace [default]
//  2 .. LSA index trace
//----------------------------------------------------------------------------*/
#define PSI_CFG_TRACE_MODE  2

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PSI_CFG_PLF_WINDOWS_WPCAP_INTERNICHE_H */