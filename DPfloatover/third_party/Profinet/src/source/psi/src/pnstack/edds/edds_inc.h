#ifndef EDDS_INC_H                     /* ----- reinclude-protection ----- */
#define EDDS_INC_H

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
/*  F i l e               &F: edds_inc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Include-File:                                                            */
/*  Includes of Edd and of the different systems, compilers or               */
/*  operating system.                                                        */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some includes depend on the different system, compiler or                */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  13.08.03    JS    ported to LSA_GLOB_P02.01.00.00_00.02.01.01            */
/*  10.02.05    JS    added ltrc support.                                    */
#endif
/*****************************************************************************/

/* the following LSA-headers or headers including these headers have to      */
/* be included here.                                                         */

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "lsa_usr.h"

/*---------------------------------------------------------------------------*/

#include "edd_cfg.h"
#include "edd_usr.h"
#include "edds_cfg.h"
#include "edds_usr.h"
#include "edds_sys.h"
#include "edds_pck.h"

#if ((EDDS_CFG_TRACE_MODE == 1) || (EDDS_CFG_TRACE_MODE == 2))
#include "ltrc_if.h"
#endif


/*-include your output-macro-header here ------------------------------------*/

#include "edds_trc.h"


/*---------------------------------------------------------------------------*/

#include "edds_out.h"


/*****************************************************************************/
/*  end of file EDDS_INC.H                                                    */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/

#endif  /* of EDDS_INC_H */

