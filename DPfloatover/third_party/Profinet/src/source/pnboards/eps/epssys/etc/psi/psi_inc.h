#ifndef PSI_INC_H
#define PSI_INC_H

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
/*  F i l e               &F: psi_inc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  The main include file for PSI                                            */
/*                                                                           */
/*  Includes of psi and of the different systems, compilers or               */
/*  operating system.                                                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/* user specific include files */

#include "psi_cfg.h"      // Basic configuration of PSI framework

/*--- include the global system header ----------------------------------------*/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*--- include pn-stack header ------------------------------------------*/

#include "pnio_version.h"  // PNIO

#if ( PSI_CFG_USE_ACP == 1 )
#include "acp_inc.h"
#endif

#if ( PSI_CFG_USE_CLRPC == 1 )
#include "clrpc_inc.h"
#endif

#if ( PSI_CFG_USE_CM == 1 )
#include "cm_inc.h"
#endif

#if ( PSI_CFG_USE_DCP == 1 )
#include "dcp_inc.h"
#endif

#if (( PSI_CFG_USE_EDDI == 1 ) || ( PSI_CFG_USE_EDDP == 1 ) || ( PSI_CFG_USE_EDDS == 1 ) || ( PSI_CFG_USE_PCSRT == 1 ))
#include "edd_inc.h"
#endif

#if ( PSI_CFG_USE_GSY == 1 )
#include "gsy_inc.h"
#endif

#if ( PSI_CFG_USE_HIF == 1 )
#include "hif_inc.h"
#endif

#if ( PSI_CFG_USE_IOH == 1 )
#include "ioh_inc.h"
#endif

#if ( PSI_CFG_USE_LLDP == 1 )
#include "lldp_inc.h"
#endif

#if ( PSI_CFG_USE_MRP == 1 )
#include "mrp_inc.h"
#endif

#if ( PSI_CFG_USE_NARE == 1 )
#include "nare_inc.h"
#endif

#if ( PSI_CFG_USE_OHA == 1 )
#include "oha_inc.h"
#endif

#if ( PSI_CFG_USE_POF == 1 )
#include "pof_inc.h"
#endif

#if ( PSI_CFG_USE_SNMPX == 1 )
#include "snmpx_inc.h"
#endif

#if ( PSI_CFG_USE_SOCK == 1 )
#include "sock_inc.h"
#endif

#if ( PSI_CFG_USE_TCIP == 1 )
#include "tcip_inc.h"
#endif

/*- include the PSI header --------------------------------------------------*/

#include "psi_usr.h"
#include "psi_sys.h"
#include "psi_pls.h"


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif   /* of PSI_INC_H */
