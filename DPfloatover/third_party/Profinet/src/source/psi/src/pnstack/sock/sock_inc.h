#ifndef SOCK_INC_H
#define SOCK_INC_H
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
/*  F i l e               &F: sock_inc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SOCK                                    */
/*                                                                           */
/*****************************************************************************/

#include "lsa_cfg.h" 
#include "lsa_sys.h"
#include "lsa_usr.h"

#include "psi_cfg.h"
/***/

#if ( PSI_CFG_USE_TCIP == 1 )
#include "tcip_inc.h"
#endif

/***/

#include "sock_cfg.h"

#if ((SOCK_CFG_TRACE_MODE == 1) || (SOCK_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif

#include "sock_usr.h"
#include "sock_sys.h"
#include "sock_trc.h"
/***/

#if 1 /* pcIOX: no ip-stack defines (FALSE, TRUE, etc.) "above" SOCK */
# if defined (SOCK_MODULE_ID)
#  include "sock_low.h" /* pulls in the ipstack header files */
# else
#  if   defined(CLRPC_INC_H)
#  elif defined(NARE_INC_H)
#  elif defined(OHA_INC_H)
#  elif defined(SNMPX_INC_H)
#  elif defined(PCIOX_PNSTACK_H)
#  elif defined(PSI_PNSTACK_H)
#  else
#   error "check this: who else needs sock?"
#  endif
#  define AF_INET 2                   // value see "ip.h", cross-checked in sock_pcpnio_startstop()
#  define SOCKIF_AF_INET AF_INET      // or else "clrpc_*.c" does not compile, see sock_usr.h
# endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif /* of SOCK_INC.H */
