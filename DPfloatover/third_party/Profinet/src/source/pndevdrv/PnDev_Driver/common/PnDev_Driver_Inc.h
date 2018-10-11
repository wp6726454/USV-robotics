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
/*  F i l e               &F: PnDev_Driver_Inc.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Include files for user interface of PnDev_Driver
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __PnDev_Driver_Inc_h__
#define __PnDev_Driver_Inc_h__

	#include <PnCore_Inc.h>
	
	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
		#ifndef UNICODE															// if CompilerSwitch for Unicode not set
			#define UNICODE
			#define _UNICODE
			
			#define	PNDEV_CHANGED_SETTINGS
		#endif
	#endif

    #include <pndev_np_common.h>												// basic types
	#include <PnDev_DriverU.h>
	#include <pndev_adapt.h>
	#include <pndev_sbl_s7p.h>
	
	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
		#ifdef PNDEV_CHANGED_SETTINGS											// if settings changed above
			#undef UNICODE
			#undef _UNICODE
			
			#undef	PNDEV_CHANGED_SETTINGS
		#endif
	#endif
#endif
