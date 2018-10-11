#ifndef CM_INC_H
#define CM_INC_H
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
/*  F i l e               &F: cm_inc.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file of CM                                              */
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

#include "pnio_version.h"

/*- include the lower-layer header ------------------------------------------*/

#include "cm_cfg.h"

#if (( CM_CFG_TRACE_MODE == 1) || (CM_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif

#include "edd_inc.h"

#if CM_CFG_USE_MRP
#include "mrp_inc.h"
#endif

#if CM_CFG_USE_GSY
#include "gsy_inc.h"
#endif

#if CM_CFG_USE_POF
#include "pof_inc.h"
#endif

#if CM_CFG_USE_NARE
#include "nare_inc.h"
#endif

#include "oha_inc.h"

#include "clrpc_inc.h"

#include "acp_inc.h" /* note: the "minimal PDEV" needs the ACP headers too (alarm things) */


/*- include the CM header ---------------------------------------------------*/

#include "cm_sys.h"
#include "cm_trc.h"
#include "cm_usr.h" /* needs the pointers with attributes */
#include "cm_low.h"
#include "cm_pls.h"


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_INC_H */
