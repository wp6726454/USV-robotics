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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_test_helper.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Helper functions for the Test Application for PNDriver                   */
/*                                                                           */
/*****************************************************************************/
#include "eps_sys.h"

#if defined(EPS_CFG_USE_PNDEVDRV)
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#include <precomp.h>
#endif

#include <eps_app.h>
#include <eps_plf.h>
#include <stdio.h>
#include <stdlib.h>
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"
#include "eps_pndevdrv.h"
#include "pnd_sys.h"
#include "pnd_test.h"
#include "servusrx.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#define PND_CFG_WPCAPDRV_MAX_BOARDS 50

PNIO_DEBUG_SETTINGS_TYPE dbg;
PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = PNIO_NULL;

#define CHAR_ESC 27
#define CHAR_BACK 8
