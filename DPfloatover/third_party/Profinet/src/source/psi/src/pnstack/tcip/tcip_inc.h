#ifndef TCIP_INC_H
#define TCIP_INC_H
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
/*  F i l e               &F: tcip_inc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file of TCIP                                            */
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


/*- include the lower-layer header ------------------------------------------*/

#include "edd_inc.h"


/*- include the TCIP header -------------------------------------------------*/

#include "tcip_cfg.h"

#if ((TCIP_CFG_TRACE_MODE == 1) || (TCIP_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif

#include "tcip_sys.h"
#include "tcip_trc.h"
#include "tcip_usr.h"
#include "tcip_low.h"
#include "tcip_pls.h"


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_INC_H */
