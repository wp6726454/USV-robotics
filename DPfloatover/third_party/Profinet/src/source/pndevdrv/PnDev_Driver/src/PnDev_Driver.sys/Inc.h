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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Inc.h                                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common include file of SYS
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __Inc_h__
#define __Inc_h__

	#include <PnDev_Driver_Inc.h>
	//------------------------------------------------------------------------
	#include "Basic.h"															// must be before os.h / os_SysDll.h
	#include "os.h"
	#include "os_SysDll.h"
	#include "SysDll.h"															// compiler switches -> must be before Asic_xx.h/Board_xx.h
	#include "eddi_irte.h"
	#include "Asic_Irte.h"														// must be before Private.h
	#include "Asic_Pnip.h"														// must be before Private.h
	#include "Board_Irte.h"														// must be before Private.h
	#include "Board_IrtePnip.h"													// must be before Private.h
	#include "Board_Pnip.h"														// must be before Private.h
	#include "Board_Intel.h"													// must be before Private.h
	#include "Board_Micrel.h"													// must be before Private.h
	#include "Board_Omap.h"													// must be before Private.h
	#include "Flash.h"															// must be before Private.h
 
	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------                                       
	                  
    #include "ErtecReg.h"                                                       // must be before Online.h     
    #include "netProfiKernelStructs.h"

	#endif

	#include "Private.h"

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------                                       
    
	#include "netProfiKernel.h" 
	#include "trace.h"     

	//------------------------------------------------------------------------
	#else
	//------------------------------------------------------------------------
	#undef PNDEV_TRACE

	#endif

#endif
