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
/*  F i l e               &F: PnDev_DriverU.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Open PnDev_Driver
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
//	start up PnDev_Driver
//************************************************************************

ePNDEV_RESULT fnPnDev_StartUpDriver(PNDEV_CHAR*				sPathDllIn,
									const UINT32			lDimErrorIn,
									uPNDEV_DRIVER_DESC**	pHandleDriverOut,
									PNDEV_CHAR*				sErrorOut)
{
ePNDEV_RESULT		eResult = ePNDEV_FATAL;
uPNDEV_DRIVER_DESC	uDriverDescTmp;

	// preset OutputParameter
	{
		*pHandleDriverOut = NULL;

		_tcscpy_s(	sErrorOut,
					lDimErrorIn,
					_TEXT(""));
	}

		// preset
		memset(	&uDriverDescTmp,
				0,
				sizeof(uPNDEV_DRIVER_DESC));

	if	(fnPnDev_LoadDll(	sPathDllIn,
							lDimErrorIn,
							&uDriverDescTmp.uIf,
							&uDriverDescTmp.hDll,
							sErrorOut))
		// loading DLL ok
	{
	int lTest = 0;

		// test DLL
		lTest = (uDriverDescTmp.uIf.pFnTest)();

		if	(lTest != 0x1234)
			// error
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at function fnPnDev_Test()!"), _TRUNCATE);
		} 
		else
		{
		ePNDEV_RESULT eResultTmp = ePNDEV_FATAL;

			// open PnDev_Driver
			eResultTmp = (uDriverDescTmp.uIf.pFnOpenDriver)(lDimErrorIn,
															(void**) pHandleDriverOut,
															sErrorOut);

			if	(eResultTmp == ePNDEV_OK)
				// success
			{
			uPNDEV_DRIVER_DESC* hDriver = NULL;

				hDriver = *pHandleDriverOut;

				if	(hDriver == NULL)
					// error
				{
					// set ErrorString
					_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
					_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid DriverHandle returned by service StartUp!"), _TRUNCATE);
				}
				else
				{
					// update DeviceDescriptionBlock returned by service OpenDriver
					{
						*((uPNDEV_FCT_PTR*) &hDriver->uIf)	= *((uPNDEV_FCT_PTR*) &uDriverDescTmp.uIf);
						hDriver->hDll						= uDriverDescTmp.hDll;
					}

					// check version of PnDev_DriverU.h (used by application) and installed driver (Windows: DLL, SYS)
					//	- can only be done in user context because of required defines located at used PnDev_DriverU.h
					//	- can be done here because this C-file is added to application
					//		(it uses HeaderFiles of user, not of DLL)
					eResult = (*hDriver->uIf.pFnCheckVersion)(	hDriver,
																PNDEVDRV_VERSION,
																PNDEVDRV_DISTRIBUTION,
																PNDEVDRV_INCREMENT,
																PNDEVDRV_INTEGRATION_COUNTER,
																lDimErrorIn,
																sErrorOut);
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	shut down PnDev_Driver
//************************************************************************

ePNDEV_RESULT fnPnDev_ShutDownDriver(	uPNDEV_DRIVER_DESC*	hDriverIn,
										const UINT32		lDimErrorIn,
										PNDEV_CHAR*			sErrorOut)
{
ePNDEV_RESULT eResult = ePNDEV_FATAL;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	if	(hDriverIn == NULL)
		// invalid DriverHandle
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid DriverHandle!"), _TRUNCATE);
	}
	else
	{
		if	(hDriverIn->uIf.pFnCloseDriver == NULL)
			// invalid FctPtr
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid FctPtr!"), _TRUNCATE);
		}
		else
		{
		HANDLE			hDllTmp		= NULL;
		ePNDEV_RESULT	eResultTmp	= ePNDEV_FATAL;

			// store DllHandle before releasing DriverHandle!
			hDllTmp = hDriverIn->hDll;

			// shut down PnDevDriver
			eResultTmp = (*hDriverIn->uIf.pFnCloseDriver)(	hDriverIn,
															lDimErrorIn,
															sErrorOut);

			if	(eResultTmp == ePNDEV_OK)
				// success
			{
				if	(fnPnDev_UnloadDll(	hDllTmp,
										lDimErrorIn,
										sErrorOut))
					// unloading DLL ok
				{
					// success
					eResult = ePNDEV_OK;
				}
			}
		}
	}

	return(eResult);
}
