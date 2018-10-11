#ifndef EDD_INC_H           //reinclude-protection
#define EDD_INC_H

#if defined __cplusplus     //If C++ - compiler: Use C linkage
extern "C" {
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
/*  F i l e               &F: edd_inc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Include-File:                                                            */
/*  Includes of EDDx and of the different systems, compilers or              */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/

#if (( PSI_CFG_USE_EDDI == 1) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 ))

#if (PSI_CFG_USE_EDDI == 1)

/* add EDDI interface */
#include "eddi_inc.h"
#endif

#if (PSI_CFG_USE_EDDP == 1)

/* add EDDP interface */
#include "eddp_inc.h"
#endif

#if (PSI_CFG_USE_EDDS == 1)

/* add EDDS interface */
#include "edds_inc.h"
#endif

#else

 #error "no valid EDDx configurion?"

#endif

#if defined __cplusplus
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif //EDD_INC_H


