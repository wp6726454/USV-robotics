#ifndef TCIP_PLS_H
#define TCIP_PLS_H

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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_pls.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check plausibility                                                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/

#if defined TCIP_CFG_ARP_AGEOUT_1SEC
#error TCIP_CFG_ARP_AGEOUT_1SEC is obsolete and must not be defined!
#endif

/*===========================================================================*/

#if !defined LSA_HOST_ENDIANESS_BIG && !defined LSA_HOST_ENDIANESS_LITTLE
#error "LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE is missing in file lsa_cfg.h!"
#endif

#if defined LSA_HOST_ENDIANESS_BIG && defined LSA_HOST_ENDIANESS_LITTLE
#error "Define LSA_HOST_ENDIANESS_BIG or LSA_HOST_ENDIANESS_LITTLE in file lsa_cfg.h!"
#endif

/*===========================================================================*/

#ifndef TCIP_CFG_COPY_ON_SEND
# error "TCIP_CFG_COPY_ON_SEND not defined in file tcip_cfg.h"
#endif

/*===========================================================================*/

#ifndef TCIP_CFG_TRACE_MODE
# error "TCIP_CFG_TRACE_MODE not defined in file tcip_cfg.h"
#endif


#ifdef TCIP_CFG_IP_MAX_MEMBERSHIPS
#error "TCIP_CFG_IP_MAX_MEMBERSHIPS is obsolete, please remove from tcip_cfg.h"
#endif

/*===========================================================================*/

#ifndef TCIP_DEBUG
# error "TCIP_DEBUG not defined in tcip_cfg.h!"
#endif

/*===========================================================================*/

#ifndef TCIP_ASSERT
# error "TCIP_ASSERT() not defined in tcip_cfg.h!"
#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_PLS_H */
