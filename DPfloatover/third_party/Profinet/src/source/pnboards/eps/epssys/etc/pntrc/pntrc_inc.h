#ifndef PNTRC_INC_H
#define PNTRC_INC_H
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
/*  F i l e               &F: pntrc_inc.h.txt                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file for PNTRC                                          */
/*                                                                           */
/*  Includes of pntrc and of the different systems, compilers or             */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/

/*- include the global system header ----------------------------------------*/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"


/*- include the PNTRC header --------------------------------------------------*/

#include "pntrc_cfg.h"

#define _PNTRC_QUOTEME(x) #x
#define PNTRC_QUOTEME(x) _PNTRC_QUOTEME(x) 

/* If the compile switch is not set to a specific include header, the standard pntrc_sub.h from pnio/src/common is included
 * example: -DPNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER=productXYZ.h */
#ifdef PNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER
#include PNTRC_QUOTEME(PNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER)   /* expands to #include "PNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER", e.g. #include "productXYZ_pntrc_sub.h" */
#else
/* the standard header from the delivery is used */
#include "pntrc_sub.h" /* defines subsystems */
#endif
#include "pntrc_usr.h"
#include "pntrc_sys.h"
#include "pntrc_tbb.h"
#include "pntrc_pls.h"


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_INC_H */
