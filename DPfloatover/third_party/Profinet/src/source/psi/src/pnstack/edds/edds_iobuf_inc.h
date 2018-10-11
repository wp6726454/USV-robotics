#ifndef EDDS_IOBUFFER_INC_H___           /* ----- reinclude-protection ----- */
#define EDDS_IOBUFFER_INC_H___

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for StdMac)    :C&  */
/*                                                                           */
/*  F i l e               &F: edds_iobuf_inc.h/txt                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IO BUFFER - include                              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  01.07.12    TP    initial version.                                       */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

/*---------------------- include your headers here --------------------------*/

#include "lsa_cfg.h"          /* LSA types                        */
#include "lsa_sys.h"          /* LSA types                        */
#include "lsa_usr.h"          /* LSA types                        */

/*---------------------- include your headers here --------------------------*/
#include "edd_cfg.h"          /* EDD base data types              */
#include "edd_usr.h"          /* EDD user interface               */
#include "edds_usr.h"         /* EDDS user interface              */
#include "edds_cfg.h"         /* EDDS configuration               */
#include "edds_sys.h"
#include "edds_srt_inc.h"     /* EDDS SRT                         */
#include "edds_iobuf_usr.h"   /* io buffer user api               */
#include "edds_iobuf_out.h"   /* io buffer output macros          */

/*---------------------- include your headers here --------------------------*/


/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  macros                                   */
/*===========================================================================*/


/*****************************************************************************/
/*  end of file edds_iobuf_inc.h/txt                                         */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_IOBUFFER_INC_H___ */
