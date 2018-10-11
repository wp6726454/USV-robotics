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
/*  F i l e               &F: PnDev_Util.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Useful routines for using PnDev_Driver
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __PnDev_Util_h__
#define __PnDev_Util_h__

	#ifdef __cplusplus		// if C++ - compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################
  //------------------------------------------------------------------------
  #if defined (PNDEV_OS_WIN) || defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#define _BLACK			0
	#define _DARK_BLUE		1
	#define _DARK_GREEN		2
	#define _DARK_CYAN		3
	#define _DARK_RED		4
	#define _DARK_MAGENTA	5
	#define _BROWN			6
	#define _GRAY			7
	#define _DARK_GRAY		8
	#define _BLUE			9
	#define _GREEN			10													// ok
	#define _CYAN			11													// ok
	#define _RED			12													// ok
	#define _MAGENTA		13													// ok
	#define _YELLOW			14													// ok
	#define _WHITE			15
	#define _BLINK			128													// ok (e.g. _RED | _BLINK)
  //---------------
  #define _STDBACKGROUND  _BLACK
  #define _STDFOREGROUND  _WHITE
  //------------------------------------------------------------------------
  #elif defined (PNDEV_OS_LINUX)
  //------------------------------------------------------------------------
	#define _BLACK        0
	#define _DARK_BLUE    
	#define _DARK_GREEN   
	#define _DARK_CYAN    
	#define _DARK_RED     
	#define _DARK_MAGENTA 
	#define _BROWN        130
	#define _GRAY         37 
	#define _DARK_GRAY    90
	#define _BLUE         34
	#define _GREEN        32
	#define _CYAN         36
	#define _RED          31                  
	#define _MAGENTA      35                          
	#define _YELLOW       33
	#define _WHITE        97
	#define _BLINK        5             //does not work on Debian
	//---------------  
	#define _STDBACKGROUND  49-10       //49 ist Standardbackgroundcolor and 10 is getting added in colorprintf-funktion
	#define _STDFOREGROUND  _BLACK
  //------------------------------------------------------------------------
  #else
  //------------------------------------------------------------------------
  //------------------------------------------------------------------------
  #endif
  //------------------------------------------------------------------------
	  
	// WinCon.h
	//#define FOREGROUND_BLUE      0x0001										// text color contains blue
	//#define FOREGROUND_GREEN     0x0002										// text color contains green
	//#define FOREGROUND_RED       0x0004										// text color contains red
	//#define FOREGROUND_INTENSITY 0x0008										// text color is intensified
	//#define BACKGROUND_BLUE      0x0010										// background color contains blue
	//#define BACKGROUND_GREEN     0x0020										// background color contains green
	//#define BACKGROUND_RED       0x0040										// background color contains red
	//#define BACKGROUND_INTENSITY 0x0080										// background color is intensified
	
	//------------------------------------------------------------------------
	#define PNDEV_SOC1_SCRB_REG__IRTE_CTRL		0x150
	#define PNDEV_SOC1_SCRB_REG__ENABLE_PN_PB	0xA00
	#define PNDEV_SOC1_GPIO_REG__PORT_MODE0_L	0x18
	#define PNDEV_SOC1_GPIO_REG__PORT_MODE0_H	0x1c
	#define PNDEV_SOC1_GPIO_REG__PORT_MODE1_L	0x38
	#define PNDEV_SOC1_GPIO_REG__PORT_MODE1_H	0x3c

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_STDIN
	{
		ePNDEV_STDIN_INVALID	= 0,
		ePNDEV_STDIN_KEYBOARD	= 1,
		ePNDEV_STDIN_RS232A		= 2,
		ePNDEV_STDIN_RS232B		= 3,
		ePNDEV_STDIN_VCON		= 4
	}
	ePNDEV_STDIN;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_STDOUT
	{
		ePNDEV_STDOUT_INVALID	= 0,
		ePNDEV_STDOUT_SCREEN	= 1,
		ePNDEV_STDOUT_RS232A	= 2,
		ePNDEV_STDOUT_RS232B	= 3,
		ePNDEV_STDOUT_VCON		= 4
	}
	ePNDEV_STDOUT;

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Function prototypes
	//########################################################################
	
	BOOLEAN fnDrvUtil_InitPhySoc1(			volatile UINT8*			pGpioIn,
											volatile UINT8*			pScrbIn,
											const BOOLEAN			bPhyClockIntIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN fnDrvUtil_CleanUp(				uPNDEV_DRIVER_DESC*		hDriverIn,
											const UINT32			lDimArrayIn,
											uPNDEV_OPEN_DEVICE_OUT	uArrayUsedDeviceIn[],
											BOOLEAN					bCloseAnywayIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN fnDrvUtil_GetFreeListIndex(		const UINT32			lDimArrayIn,
											uPNDEV_OPEN_DEVICE_OUT	uArrayUsedDeviceIn[],
											const UINT32			lDimErrorIn,
											UINT32*					pIndexOut,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN fnDrvUtil_ReleaseListIndex(		const UINT32			lIndexIn,
											const UINT32			lDimArrayIn,
											uPNDEV_OPEN_DEVICE_OUT	uArrayUsedDeviceIn[],
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN	fnDrvUtil_GetUsedListIndex(		PNDEV_CHAR*				sPciLocIn,
											const UINT32			lDimArrayIn,
											uPNDEV_OPEN_DEVICE_OUT	uArrayUsedDeviceIn[],	// array=ptr
											UINT32*					pIndexOut);
	BOOLEAN fnDrvUtil_SetCurDirToExe(		PNDEV_CHAR*				sFullPathExeIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	/*void	fnDrvUtil_ShowError(			const HANDLE			hStdoutIn,
											const ePNDEV_STDOUT		eStdoutIn,
											const ePNDEV_RESULT		eResultIn,
											const PNDEV_CHAR*		sErrorIn);	-->	2014-05-13 - UT5507 - function moved to PnDev_Test.h */
	BOOLEAN	fnDrvUtil_RegisterStdStreams(	const ePNDEV_STDIN		eStdinIn,
											const ePNDEV_STDOUT		eStdoutIn,
											const UINT32			lDimErrorIn,
											HANDLE*					pHandleStdinOut,
											HANDLE*					pHandleStdoutOut,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN	fnDrvUtil_InstallDriver(		const ePNDEV_STDIN		eStdinIn,
											const ePNDEV_STDOUT		eStdoutIn,
											const UINT32			lDimErrorIn,
											HANDLE*					pHandleStdinOut,
											HANDLE*					pHandleStdoutOut,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN fnDrvUtil_CheckInstalledDriver(	PNDEV_CHAR*				sSysFileNameIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	BOOLEAN	fnDrvUtil_UnregisterStdStreams(	const HANDLE			hStdinIn,
											const HANDLE			hStdoutIn,
											const UINT32			lDimErrorIn,
											PNDEV_CHAR*				sErrorOut);
	/*void	fnDrvUtil_PrintfColor(			const HANDLE			hStdoutIn,
											const ePNDEV_STDOUT		eStdoutIn,
											const UINT32			lColorFontIn,
											const UINT32			lColorBackIn,
											const PNDEV_CHAR*		sFormatIn, ...); -->	2014-07-02 - UT5507 - function moved to PnDev_Test.h */

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ - compiler: End of C linkage
	}
	#endif

#endif
