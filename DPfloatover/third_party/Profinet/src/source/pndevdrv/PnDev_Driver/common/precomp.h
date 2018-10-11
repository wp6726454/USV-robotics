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
/*  F i l e               &F: precomp.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Environment specific include file required by C files of directory "common"
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __precomp_h__
#define __precomp_h__

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	#pragma once

	#ifndef UNICODE
		#define	UNICODE
		#define	_UNICODE
	#endif

	#define WIN32_LEAN_AND_MEAN													// exclude rarely-used stuff from Windows headers

	#if defined (PNDEV_TOOL_GCC_MINGW)
		#ifndef WINVER
			#define WINVER	0x502												// required for SetDllDirectory()
		#endif
	#else																		// Microsoft
		#include "pndev_targetver.h"
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <tchar.h>
	#include <windows.h>
	#include "Shlwapi.h"

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#include <Adonis_Inc.h>

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	#if defined PNDEV_DRIVER_SYS
		#include <linux/module.h>
		#include <linux/init.h>
		#include <linux/kernel.h>	/* printk() */
		#include <linux/fs.h>
	#else
		#include <stdint.h>
		#include <stddef.h>
		#include <unistd.h>       //Get Path of executable
		#include <dlfcn.h>        //dynamic loading of shared objects
    	#include <pthread.h>      //setting Thread priority
	#endif
	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

#endif
