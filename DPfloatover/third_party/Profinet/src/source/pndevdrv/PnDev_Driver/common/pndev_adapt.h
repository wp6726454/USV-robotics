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
/*  F i l e               &F: pndev_adapt.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for user specific adaptions
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __pndev_adapt_h__
#define __pndev_adapt_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif
	
	//########################################################################
	//  Defines
	//########################################################################
	
	typedef enum _ePNDEV_TRACE_LEVEL
	{
		ePNDEV_TRACE_LEVEL_INVALID	= 0,
		ePNDEV_TRACE_LEVEL_ERROR	= 1,
		ePNDEV_TRACE_LEVEL_INFO		= 2
	}
	ePNDEV_TRACE_LEVEL;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_TRACE_CONTEXT
	{
		ePNDEV_TRACE_CONTEXT_INVALID	= 0,
		ePNDEV_TRACE_CONTEXT_BASIC		= 1,
		ePNDEV_TRACE_CONTEXT_DPC		= 2,
		ePNDEV_TRACE_CONTEXT_ISR		= 3
	}
	ePNDEV_TRACE_CONTEXT;

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Function prototypes
	//########################################################################
	
	//------------------------------------------------------------------------
	// pndev_adapt_dll_1.c
	//------------------------------------------------------------------------
	void fnSetTraceDll(const PNDEV_CHAR* sTraceIn);
	
	//------------------------------------------------------------------------
	// pndev_adapt_sys.c
	//------------------------------------------------------------------------
	void fnSetTraceSys(	const ePNDEV_TRACE_LEVEL	eLevelIn,
						const ePNDEV_TRACE_CONTEXT	eContextIn,
						const BOOLEAN				bLogIn,
						char*						sTraceIn);
							
	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
