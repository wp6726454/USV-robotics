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
/*  F i l e               &F: pndev_adapt_dll_1.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for user specific adaptions of DLL (language C)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

// Note:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

#include <precomp.h>														// environment specific IncludeFiles
																			//	- name is fixed for Driver ToolChain, include by conditional compiling not possible!
#include <PnDev_Driver_Inc.h>												// all IncludeFiles of PnDev_Driver

//************************************************************************
//  D e s c r i p t i o n :
//
//	set trace at DLL
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnSetTraceDll(const PNDEV_CHAR* sTraceIn)
{
//	_tprintf(sTraceIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

