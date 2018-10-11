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
/*  F i l e               &F: os_Env.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for adaption to used environment (OS, compiler, C/C++ library)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

// Note if using VisualStudio:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#include <precomp.h>														// environment specific IncludeFiles
#include <PnDev_Driver_Inc.h>												//	- name is fixed for Driver ToolChain, include by conditional compiling not possible!
//------------------------------------------------------------------------
#else
//------------------------------------------------------------------------
#include "precomp.h"
#include "PnDev_Driver_Inc.h"												// all IncludeFiles of PnDev_Driver
//------------------------------------------------------------------------
#endif


#ifdef PNDEV_TRACE															// required for Driver ToolChain
	#include "os_Env.tmh"
#endif

#ifdef PNDEV_DRIVER_UM                          //Usermode

//************************************************************************
//  D e s c r i p t i o n :
//
//	load DLL
//************************************************************************
//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnPnDev_LoadDll(const PNDEV_CHAR*	sPathDllIn,
						const UINT32		lDimErrorIn,
						void*				pFctPtrOut,						// void* because of os_Env.h
						HANDLE*				pHandleDllOut,
						PNDEV_CHAR*			sErrorOut)
{
BOOLEAN			bResult		= FALSE;
HANDLE			hDllTmp		= NULL;
uPNDEV_FCT_PTR*	pFctPtrTmp	= NULL;

	pFctPtrTmp = (uPNDEV_FCT_PTR*) pFctPtrOut;

	// preset OutputParameter
	{
		memset(	pFctPtrOut,
				0,
				sizeof(uPNDEV_FCT_PTR));

		*pHandleDllOut = NULL;
	}

	// try to get handle of DLL
	hDllTmp = GetModuleHandle(sPathDllIn);

	if	(hDllTmp == NULL)
		// DLL not already loaded
	{
		// clear search path for DLLs
		//	- if DLL is not specified by full path then wrong DLL may be loaded
		SetDllDirectory(_TEXT(""));

		// load library
		hDllTmp = LoadLibrary(sPathDllIn);
	}

	if	(hDllTmp == NULL)
		// error
	{
	ULONG	lLastError = 0;
	_TCHAR	sLastError[PNDEV_SIZE_STRING_BUF] = {0};

		// get last error
		//	- Note: value is not cleared by read
		lLastError = GetLastError();

		// set LastErrorString
		//_ltot_s(lLastError, sLastError, _countof(sLastError), 10);
		_stprintf_s(sLastError,
					_countof(sLastError),
					_TEXT("Cannot load %s (Error=0x%x)"),
					sPathDllIn,
					lLastError);

		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): "), _TRUNCATE);
		_tcsncat_s(	sErrorOut, lDimErrorIn, sLastError,		_TRUNCATE);
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("!\n\n"),	_TRUNCATE);
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("DLL not found or runtime components of Visual C++ libraries missing\n"), _TRUNCATE);
	}
	else
	{
		// get FctPtr
		pFctPtrTmp->pFnTest				= (PNDEV_FN_TEST)				GetProcAddress(	hDllTmp, "fnPnDev_Test");
		pFctPtrTmp->pFnOpenDriver		= (PNDEV_FN_OPEN_DRIVER)		GetProcAddress(	hDllTmp, "fnPnDev_OpenDriver");
		pFctPtrTmp->pFnCloseDriver		= (PNDEV_FN_CLOSE_DRIVER)		GetProcAddress(	hDllTmp, "fnPnDev_CloseDriver");
		pFctPtrTmp->pFnGetVersion		= (PNDEV_FN_GET_VERSION)		GetProcAddress(	hDllTmp, "fnPnDev_GetVersion");
		pFctPtrTmp->pFnCheckVersion		= (PNDEV_FN_CHECK_VERSION)		GetProcAddress(	hDllTmp, "fnPnDev_CheckVersion");
		pFctPtrTmp->pFnGetDeviceInfo	= (PNDEV_FN_GET_DEVICE_INFO)	GetProcAddress(	hDllTmp, "fnPnDev_GetDeviceInfo");
		pFctPtrTmp->pFnOpenDevice		= (PNDEV_FN_OPEN_DEVICE)		GetProcAddress(	hDllTmp, "fnPnDev_OpenDevice");
		pFctPtrTmp->pFnChangeInt		= (PNDEV_FN_CHANGE_INT)			GetProcAddress(	hDllTmp, "fnPnDev_ChangeInt");
		pFctPtrTmp->pFnGetDeviceState	= (PNDEV_FN_GET_DEVICE_STATE)	GetProcAddress(	hDllTmp, "fnPnDev_GetDeviceState");
		pFctPtrTmp->pFnProgramFlash		= (PNDEV_FN_PROGRAM_FLASH)		GetProcAddress(	hDllTmp, "fnPnDev_ProgramFlash");
		pFctPtrTmp->pFnChangeXhifPage	= (PNDEV_FN_CHANGE_XHIF_PAGE)	GetProcAddress(	hDllTmp, "fnPnDev_ChangeXhifPage");
		pFctPtrTmp->pFnComparePciLoc	= (PNDEV_FN_COMPARE_PCI_LOC)	GetProcAddress(	hDllTmp, "fnPnDev_ComparePciLoc");
		pFctPtrTmp->pFnCopyData			= (PNDEV_FN_COPY_DATA)			GetProcAddress(	hDllTmp, "fnPnDev_CopyData");
		pFctPtrTmp->pFnCloseDevice		= (PNDEV_FN_CLOSE_DEVICE)		GetProcAddress(	hDllTmp, "fnPnDev_CloseDevice");
		pFctPtrTmp->pFnInitNrt			= (PNDEV_FN_INIT_NRT)			GetProcAddress(	hDllTmp, "fnPnDev_InitNrt");
		pFctPtrTmp->pFnAllocNrtRqb		= (PNDEV_FN_ALLOC_NRT_RQB)		GetProcAddress(	hDllTmp, "fnPnDev_AllocNrtRqb");
		pFctPtrTmp->pFnFreeNrtRqb		= (PNDEV_FN_FREE_NRT_RQB)		GetProcAddress(	hDllTmp, "fnPnDev_FreeNrtRqb");
		pFctPtrTmp->pFnSendNrt			= (PNDEV_FN_SEND_NRT)			GetProcAddress(	hDllTmp, "fnPnDev_SendNrt");
		pFctPtrTmp->pFnRcveNrt			= (PNDEV_FN_RCVE_NRT)			GetProcAddress(	hDllTmp, "fnPnDev_RcveNrt");
		pFctPtrTmp->pFnChangeNrt		= (PNDEV_FN_CHANGE_NRT)			GetProcAddress(	hDllTmp, "fnPnDev_ChangeNrt");
        pFctPtrTmp->pFnDoNetProfi		= (PNDEV_FN_DO_NETPROFI)		GetProcAddress(	hDllTmp, "fnPnDev_DoNetProfi");


		if	(	(pFctPtrTmp->pFnTest			== NULL)
			||	(pFctPtrTmp->pFnOpenDriver		== NULL)
			||	(pFctPtrTmp->pFnCloseDriver		== NULL)
			||	(pFctPtrTmp->pFnGetVersion		== NULL)
			||	(pFctPtrTmp->pFnCheckVersion	== NULL)
			||	(pFctPtrTmp->pFnGetDeviceInfo	== NULL)
			||	(pFctPtrTmp->pFnOpenDevice		== NULL)
			||	(pFctPtrTmp->pFnChangeInt		== NULL)
			||	(pFctPtrTmp->pFnGetDeviceState	== NULL)
			||	(pFctPtrTmp->pFnProgramFlash	== NULL)
			||	(pFctPtrTmp->pFnChangeXhifPage	== NULL)
			||	(pFctPtrTmp->pFnComparePciLoc	== NULL)
			||	(pFctPtrTmp->pFnCopyData		== NULL)
			||	(pFctPtrTmp->pFnCloseDevice		== NULL)
			||	(pFctPtrTmp->pFnInitNrt			== NULL)
			||	(pFctPtrTmp->pFnAllocNrtRqb		== NULL)
			||	(pFctPtrTmp->pFnFreeNrtRqb		== NULL)
			||	(pFctPtrTmp->pFnSendNrt			== NULL)
			||	(pFctPtrTmp->pFnRcveNrt			== NULL)
			||	(pFctPtrTmp->pFnChangeNrt		== NULL)
			||	(pFctPtrTmp->pFnDoNetProfi		== NULL))
			// error
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid FctPtr!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;

			// return
			*pHandleDllOut = hDllTmp;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnPnDev_LoadDll(const PNDEV_CHAR*	sPathDllIn,
						const UINT32		lDimErrorIn,
						void*				pFctPtrOut,
						HANDLE*				pHandleDllOut,
						PNDEV_CHAR*			sErrorOut)
{
BOOLEAN			bResult		= FALSE;
uPNDEV_FCT_PTR*	pFctPtrTmp	= NULL;

	pFctPtrTmp = (uPNDEV_FCT_PTR*) pFctPtrOut;

	// preset OutputParameter
	{
		memset(	pFctPtrOut,
				0,
				sizeof(uPNDEV_FCT_PTR));

		*pHandleDllOut = NULL;
	}

	// get FctPtr
	pFctPtrTmp->pFnTest				= (PNDEV_FN_TEST)				&fnPnDev_Test;
	pFctPtrTmp->pFnOpenDriver		= (PNDEV_FN_OPEN_DRIVER)		&fnPnDev_OpenDriver;
	pFctPtrTmp->pFnCloseDriver		= (PNDEV_FN_CLOSE_DRIVER)		&fnPnDev_CloseDriver;
	pFctPtrTmp->pFnGetVersion		= (PNDEV_FN_GET_VERSION)		&fnPnDev_GetVersion;
	pFctPtrTmp->pFnCheckVersion		= (PNDEV_FN_CHECK_VERSION)		&fnPnDev_CheckVersion;
	pFctPtrTmp->pFnGetDeviceInfo	= (PNDEV_FN_GET_DEVICE_INFO)	&fnPnDev_GetDeviceInfo;
	pFctPtrTmp->pFnOpenDevice		= (PNDEV_FN_OPEN_DEVICE)		&fnPnDev_OpenDevice;
	pFctPtrTmp->pFnChangeInt		= (PNDEV_FN_CHANGE_INT)			&fnPnDev_ChangeInt;
	pFctPtrTmp->pFnGetDeviceState	= (PNDEV_FN_GET_DEVICE_STATE)	&fnPnDev_GetDeviceState;
	pFctPtrTmp->pFnProgramFlash		= (PNDEV_FN_PROGRAM_FLASH)		&fnPnDev_ProgramFlash;
	pFctPtrTmp->pFnChangeXhifPage	= (PNDEV_FN_CHANGE_XHIF_PAGE)	&fnPnDev_ChangeXhifPage;
	pFctPtrTmp->pFnComparePciLoc	= (PNDEV_FN_COMPARE_PCI_LOC)	&fnPnDev_ComparePciLoc;
	pFctPtrTmp->pFnCopyData			= (PNDEV_FN_COPY_DATA)			&fnPnDev_CopyData;
	pFctPtrTmp->pFnCloseDevice		= (PNDEV_FN_CLOSE_DEVICE)		&fnPnDev_CloseDevice;
	pFctPtrTmp->pFnInitNrt			= (PNDEV_FN_INIT_NRT)			&fnPnDev_InitNrt;
	pFctPtrTmp->pFnAllocNrtRqb		= (PNDEV_FN_ALLOC_NRT_RQB)		&fnPnDev_AllocNrtRqb;
	pFctPtrTmp->pFnFreeNrtRqb		= (PNDEV_FN_FREE_NRT_RQB)		&fnPnDev_FreeNrtRqb;
	pFctPtrTmp->pFnSendNrt			= (PNDEV_FN_SEND_NRT)			&fnPnDev_SendNrt;
	pFctPtrTmp->pFnRcveNrt			= (PNDEV_FN_RCVE_NRT)			&fnPnDev_RcveNrt;
	pFctPtrTmp->pFnChangeNrt		= (PNDEV_FN_CHANGE_NRT)			&fnPnDev_ChangeNrt;

	if	(	(pFctPtrTmp->pFnTest			== NULL)
		||	(pFctPtrTmp->pFnOpenDriver		== NULL)
		||	(pFctPtrTmp->pFnCloseDriver		== NULL)
		||	(pFctPtrTmp->pFnGetVersion		== NULL)
		||	(pFctPtrTmp->pFnCheckVersion	== NULL)
		||	(pFctPtrTmp->pFnGetDeviceInfo	== NULL)
		||	(pFctPtrTmp->pFnOpenDevice		== NULL)
		||	(pFctPtrTmp->pFnChangeInt		== NULL)
		||	(pFctPtrTmp->pFnGetDeviceState	== NULL)
		||	(pFctPtrTmp->pFnProgramFlash	== NULL)
		||	(pFctPtrTmp->pFnChangeXhifPage	== NULL)
		||	(pFctPtrTmp->pFnComparePciLoc	== NULL)
		||	(pFctPtrTmp->pFnCopyData		== NULL)
		||	(pFctPtrTmp->pFnCloseDevice		== NULL)
		||	(pFctPtrTmp->pFnInitNrt			== NULL)
		||	(pFctPtrTmp->pFnAllocNrtRqb		== NULL)
		||	(pFctPtrTmp->pFnFreeNrtRqb		== NULL)
		||	(pFctPtrTmp->pFnSendNrt			== NULL)
		||	(pFctPtrTmp->pFnRcveNrt			== NULL)
		||	(pFctPtrTmp->pFnChangeNrt		== NULL))
		// error
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid FctPtr!"), _TRUNCATE);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnPnDev_LoadDll(const PNDEV_CHAR* 	sPathDllIn,
						const UINT32    	lDimErrorIn,
						void*      		 	pFctPtrOut,
						HANDLE*       		pHandleDllOut,
						PNDEV_CHAR*     	sErrorOut)
{
BOOLEAN     		bResult   	= FALSE;
HANDLE      		hDllTmp   	= NULL;
uPNDEV_FCT_PTR* 	pFctPtrTmp  = NULL;

    pFctPtrTmp = (uPNDEV_FCT_PTR*) pFctPtrOut;

    // preset OutputParameter
    {
		memset( pFctPtrOut,
			0,
			sizeof(uPNDEV_FCT_PTR));

		*pHandleDllOut = NULL;
    }

    // try to get handle of DLL
    hDllTmp = dlopen(	sPathDllIn,
    					RTLD_LAZY);

    if 	(hDllTmp == NULL)
    	// error
    {
    _TCHAR  sLastError[PNDEV_SIZE_STRING_BUF] = {0};

		_stprintf_s(sLastError,
					_countof(sLastError),
					_TEXT("Cannot load '%s' (Error: %s)"),
					sPathDllIn,
					dlerror());

		// set ErrorString
		_tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): "), _TRUNCATE);
		_tcsncat_s( sErrorOut, lDimErrorIn, sLastError,   _TRUNCATE);
		_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("!\n\n"), _TRUNCATE);
		_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT(".so not found\n"), _TRUNCATE);
    }
    else
    {
    	// get FctPtr
		//fn = dlsym(lib_handle, "ctest1");
		pFctPtrTmp->pFnTest       		= (PNDEV_FN_TEST)       		dlsym( hDllTmp, "fnPnDev_Test");
		pFctPtrTmp->pFnOpenDriver   	= (PNDEV_FN_OPEN_DRIVER)    	dlsym( hDllTmp, "fnPnDev_OpenDriver");
		pFctPtrTmp->pFnCloseDriver   	= (PNDEV_FN_CLOSE_DRIVER)   	dlsym( hDllTmp, "fnPnDev_CloseDriver");
		pFctPtrTmp->pFnGetVersion   	= (PNDEV_FN_GET_VERSION)    	dlsym( hDllTmp, "fnPnDev_GetVersion");
		pFctPtrTmp->pFnCheckVersion   	= (PNDEV_FN_CHECK_VERSION)  	dlsym( hDllTmp, "fnPnDev_CheckVersion");
		pFctPtrTmp->pFnGetDeviceInfo  	= (PNDEV_FN_GET_DEVICE_INFO)	dlsym( hDllTmp, "fnPnDev_GetDeviceInfo");
		pFctPtrTmp->pFnOpenDevice   	= (PNDEV_FN_OPEN_DEVICE)    	dlsym( hDllTmp, "fnPnDev_OpenDevice");
		pFctPtrTmp->pFnChangeInt    	= (PNDEV_FN_CHANGE_INT)     	dlsym( hDllTmp, "fnPnDev_ChangeInt");
		pFctPtrTmp->pFnGetDeviceState	= (PNDEV_FN_GET_DEVICE_STATE) 	dlsym( hDllTmp, "fnPnDev_GetDeviceState");
		pFctPtrTmp->pFnProgramFlash   	= (PNDEV_FN_PROGRAM_FLASH)    	dlsym( hDllTmp, "fnPnDev_ProgramFlash");
		pFctPtrTmp->pFnChangeXhifPage 	= (PNDEV_FN_CHANGE_XHIF_PAGE) 	dlsym( hDllTmp, "fnPnDev_ChangeXhifPage");
		pFctPtrTmp->pFnComparePciLoc  	= (PNDEV_FN_COMPARE_PCI_LOC)  	dlsym( hDllTmp, "fnPnDev_ComparePciLoc");
		pFctPtrTmp->pFnCopyData     	= (PNDEV_FN_COPY_DATA)      	dlsym( hDllTmp, "fnPnDev_CopyData");
		pFctPtrTmp->pFnCloseDevice    	= (PNDEV_FN_CLOSE_DEVICE)   	dlsym( hDllTmp, "fnPnDev_CloseDevice");
		pFctPtrTmp->pFnInitNrt      	= (PNDEV_FN_INIT_NRT)     		dlsym( hDllTmp, "fnPnDev_InitNrt");
		pFctPtrTmp->pFnAllocNrtRqb    	= (PNDEV_FN_ALLOC_NRT_RQB)    	dlsym( hDllTmp, "fnPnDev_AllocNrtRqb");
		pFctPtrTmp->pFnFreeNrtRqb   	= (PNDEV_FN_FREE_NRT_RQB)   	dlsym( hDllTmp, "fnPnDev_FreeNrtRqb");
		pFctPtrTmp->pFnSendNrt      	= (PNDEV_FN_SEND_NRT)     		dlsym( hDllTmp, "fnPnDev_SendNrt");
		pFctPtrTmp->pFnRcveNrt      	= (PNDEV_FN_RCVE_NRT)     		dlsym( hDllTmp, "fnPnDev_RcveNrt");
		pFctPtrTmp->pFnChangeNrt    	= (PNDEV_FN_CHANGE_NRT)     	dlsym( hDllTmp, "fnPnDev_ChangeNrt");
		pFctPtrTmp->pFnDoNetProfi   	= (PNDEV_FN_DO_NETPROFI)    	dlsym( hDllTmp, "fnPnDev_DoNetProfi");

		if	( 	(pFctPtrTmp->pFnTest      		== NULL)
			||  (pFctPtrTmp->pFnOpenDriver    	== NULL)
			||  (pFctPtrTmp->pFnCloseDriver   	== NULL)
			||  (pFctPtrTmp->pFnGetVersion   	== NULL)
			||  (pFctPtrTmp->pFnCheckVersion  	== NULL)
			||  (pFctPtrTmp->pFnGetDeviceInfo 	== NULL)
			||  (pFctPtrTmp->pFnOpenDevice    	== NULL)
			||  (pFctPtrTmp->pFnChangeInt   	== NULL)
			||  (pFctPtrTmp->pFnGetDeviceState  == NULL)
			||  (pFctPtrTmp->pFnProgramFlash  	== NULL)
			||  (pFctPtrTmp->pFnChangeXhifPage  == NULL)
			||  (pFctPtrTmp->pFnComparePciLoc 	== NULL)
			||  (pFctPtrTmp->pFnCopyData    	== NULL)
			||  (pFctPtrTmp->pFnCloseDevice   	== NULL)
			||  (pFctPtrTmp->pFnInitNrt     	== NULL)
			||  (pFctPtrTmp->pFnAllocNrtRqb  	== NULL)
			||  (pFctPtrTmp->pFnFreeNrtRqb    	== NULL)
			||  (pFctPtrTmp->pFnSendNrt     	== NULL)
			||  (pFctPtrTmp->pFnRcveNrt     	== NULL)
			||  (pFctPtrTmp->pFnChangeNrt   	== NULL)
			||  (pFctPtrTmp->pFnDoNetProfi    	== NULL))
			// error
		{
			// set ErrorString
			_tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Invalid FctPtr!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;

			// return
			*pHandleDllOut = hDllTmp;
		}
    }

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	unload DLL
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

BOOLEAN fnPnDev_UnloadDll(	HANDLE			hDllIn,
							const UINT32	lDimErrorIn,
							PNDEV_CHAR*		sErrorOut)
{
BOOLEAN	bResult	= FALSE;

	if	(!FreeLibrary(hDllIn))
		// error
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at FreeLibrary()!"), _TRUNCATE);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnPnDev_UnloadDll(	HANDLE			hDllIn,
							const UINT32	lDimErrorIn,
							PNDEV_CHAR*		sErrorOut)
{
BOOLEAN	bResult	= FALSE;

	// success
	bResult = TRUE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnPnDev_UnloadDll(  HANDLE      hDllIn,
              const UINT32  lDimErrorIn,
              PNDEV_CHAR*   sErrorOut)
{
BOOLEAN bResult = FALSE;
INT32	lTmpResult = -1;

	lTmpResult = dlclose(hDllIn);

  if  (lTmpResult != LINUX_OK)
    // error
  {
  // set ErrorString
  _TCHAR  sLastError[PNDEV_SIZE_STRING_BUF] = {0};

    _stprintf_s(sLastError,
          _countof(sLastError),
          _TEXT("Cannot unload shared object (Error: %s)"),
          dlerror());

    // set ErrorString
    _tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
    _tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Error at dlclose()!"), _TRUNCATE);
    _tcsncat_s( sErrorOut, lDimErrorIn, sLastError,   _TRUNCATE);
  }
  else
  {
    // success
    bResult = TRUE;
  }

  return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

#endif //PNDEV_DRIVER_UM

//************************************************************************
//  D e s c r i p t i o n :
//
//	set ThreadPrio
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN) && defined (PNDEV_DRIVER_UM)
//------------------------------------------------------------------------
BOOLEAN fnSetThreadPrio(const HANDLE	hThreadIn,
						const UINT32	lPrioThreadIn,
						const INT32		lPolicyIn,
						const UINT32	lDimErrorIn,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lPolicyIn);

	if	(lPrioThreadIn > 31)
		// error
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid ThreadPriority!"), _TRUNCATE);
	}
	else
	{
	int lPrioThreadTmp = 0;

		if	(lPrioThreadIn <= 19)
			// NormalPrio
		{
			lPrioThreadTmp = THREAD_PRIORITY_NORMAL;
		}
		else if	(lPrioThreadIn <= 29)
				// HighPrio
		{
			lPrioThreadTmp = THREAD_PRIORITY_HIGHEST;
		}
		else	// TimeCriticalPrio
		{
			lPrioThreadTmp = THREAD_PRIORITY_TIME_CRITICAL;
		}

		if	(!SetThreadPriority(hThreadIn,
								lPrioThreadTmp))
			// error at setting ThreadPrio
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at SetThreadPriority()!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnSetThreadPrio(const HANDLE	hThreadIn,
						const UINT32	lPrioIn,
						const INT32		lPolicyIn,
						const UINT32	lDimErrorIn,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lPolicyIn);

	if	(lPrioIn > 31)
		// error
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Invalid ThreadPriority!"), _TRUNCATE);
	}
	else
	{
	int32_t lResultTmp = ADN_ERROR;

		// set scheduling prio
		lResultTmp = pthread_setschedprio(	(pthread_t)hThreadIn,
											lPrioIn);

		if 	(lResultTmp != ADN_OK)
			// error
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_setschedprio()!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX) && defined (PNDEV_DRIVER_UM)
//------------------------------------------------------------------------
BOOLEAN fnSetThreadPrio(const HANDLE	hThreadIn,
            			const UINT32	lPrioThreadIn,
            			const INT32		lPolicyIn,
            			const UINT32	lDimErrorIn,
            			PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult 	= FALSE;
int32_t lResultTmp 	= LINUX_ERROR;
int32_t lCurrentThreadPolicy;
int32_t lChangedPolicy;
struct sched_param uCurrentSchedParam;

    // getting current thread policy for getting max thread prio
    lResultTmp = pthread_getschedparam( (pthread_t) hThreadIn,
                                        &lCurrentThreadPolicy,
                                        &uCurrentSchedParam);

    if  (lResultTmp != LINUX_OK)
      //error
    {
        // set ErrorString
        _tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
        _tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Invalid ThreadID!"), _TRUNCATE);
    }
    else
    {
    int32_t lMaxThreadPrio = 0;
    
    	lChangedPolicy = lPolicyIn;
    
    	if	(	(lPrioThreadIn == 0)
    		&&	(lPolicyIn != SCHED_OTHER))
    	{
    		// Since other operating systems (Windows, Adonis) don't need the policy change in order to change the priority
    		// currently fnPnDev_ChangeInt Service can only change only the priority of a thread, not it's policy.
    		// To prevent from errors when resetting the priority to 0 (not allowed for SCHED_RR and SCHED_FIFO)
    		// we assume that the thread was an non real-time thread (SCHED_OTHER)
    		lChangedPolicy = SCHED_OTHER;
    		
    	}

		if	(lCurrentThreadPolicy != lChangedPolicy)
		{
			uCurrentSchedParam.sched_priority 	= lPrioThreadIn;
			lCurrentThreadPolicy				= lChangedPolicy;

			lResultTmp = pthread_setschedparam( (pthread_t) hThreadIn,
												lCurrentThreadPolicy,
												&uCurrentSchedParam);

		}
		else
		{
			lResultTmp = LINUX_OK;
		}

	    if  (lResultTmp != LINUX_OK)
	      //error
	    {
	        // set ErrorString
	        _tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
	        _tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Error at switch Thread to other policy!"), _TRUNCATE);
	    }
	    else
	    {
			// get max ThreadPrio
	    	lMaxThreadPrio = sched_get_priority_max(lCurrentThreadPolicy);

			if  (lMaxThreadPrio == LINUX_ERROR)
				//error
			{
				// set ErrorString
				_tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
				_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Invalid SchedulingPolicy!"), _TRUNCATE);
			}
			else
			{
				if  (lPrioThreadIn > (UINT32) lMaxThreadPrio)
				  // error
				{
					// set ErrorString
					_tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
					_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Invalid ThreadPriority!"), _TRUNCATE);
				}
				else
				{
					// set scheduling prio
					lResultTmp = pthread_setschedprio(  (pthread_t)hThreadIn,
														lPrioThreadIn);

					if  (lResultTmp != LINUX_OK)
					  // error
					{
						// set ErrorString
						_tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
						_tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_setschedprio()!"), _TRUNCATE);
					}
					else
					{
						// success
						bResult = TRUE;
				   }
				}
			}
	    }
    }
    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get ThreadPrio
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN) && defined (PNDEV_DRIVER_UM)
//------------------------------------------------------------------------
UINT32 fnGetThreadPrio(	const HANDLE	hThreadIn, 
						INT32*			pPolicyOut)
{
	UNREFERENCED_PARAMETER(pPolicyOut);
	return(GetThreadPriority(hThreadIn));
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)  || (defined PNDEV_OS_LINUX && defined PNDEV_DRIVER_UM)
//------------------------------------------------------------------------
UINT32 fnGetThreadPrio(	const HANDLE	hThreadIn, 
						INT32* 			pPolicyOut)
{
UINT32				lResult 		= 0;
int32_t				lResultTmp		= -1;
INT32				lThreadpolicy	= 0;
struct sched_param	uThreadSchedParam;

	//preset out param
	*pPolicyOut = SCHED_OTHER;

	// get scheduling thread parameter
	lResultTmp = pthread_getschedparam(	(pthread_t) hThreadIn,
										&lThreadpolicy,
										&uThreadSchedParam);

	if 	(lResultTmp == POSIX_OK)
		// success
	{
		// return scheduling priority
		lResult = uThreadSchedParam.sched_priority;
		*pPolicyOut = lThreadpolicy;
	}

	return (lResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get current ThreadHandle
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#ifdef PNDEV_DRIVER_UM                          //Usermode

HANDLE fnGetThreadHandle(void)
{
	return(GetCurrentThread());
}
#endif

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || (defined (PNDEV_OS_LINUX) && defined (PNDEV_DRIVER_UM))
//------------------------------------------------------------------------
HANDLE fnGetThreadHandle(void)
{
	return((HANDLE) pthread_self());
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	create mutex
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	fnCreateMutex(	const UINT32	lDimErrorIn,
						void*			pMutexOut,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN				bResult = FALSE;
pthread_mutexattr_t	uMutexAttr;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	// Note:
	//	- MutexType must be recursive because CriticalSection must be nestable
	//	- PriorityInheritance:
	//		- if a low prior thread A locks a mutex and a high prior thread C wants to lock the same mutex
	//		- then a thread B with a prioity between A and C would repress the thread C   (PriorityInversion)
	//		- PriorityInheritance gives the thread A temporarily the priority of C, till thread A unlocks the mutex

	// preset
	memset(	&uMutexAttr,
			0,
			sizeof(pthread_mutexattr_t));

	if	(pthread_mutexattr_init(&uMutexAttr) != ADN_OK)
		// error at initializing MutexAttr
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_init()!"), _TRUNCATE);
	}
	else if	(pthread_mutexattr_settype(	&uMutexAttr,
										PTHREAD_MUTEX_RECURSIVE) != ADN_OK)
			// error at specifying MutexType
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_settype()!"), _TRUNCATE);
	}

#if (ADN_MTX_MODE == ADN_MTX_MODE_PRIO)
	else if	(pthread_mutexattr_setprotocol(	&uMutexAttr,
											PTHREAD_PRIO_INHERIT) != ADN_OK)
			// error at specifying PriorityInheritance
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_setprotocol()!"), _TRUNCATE);
	}
#endif

	else
	{
		if	(pthread_mutex_init(pMutexOut,
								&uMutexAttr) != ADN_OK)
			// error at initializing mutex
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutex_init()!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	fnCreateMutex(	const UINT32	lDimErrorIn,
						void*			pMutexOut,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN				bResult = FALSE;
#if defined (PNDEV_DRIVER_SYS)
	// linux kernel mutexes are initialized without mutex attributes
#else
pthread_mutexattr_t	uMutexAttr;
#endif


	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

#if defined (PNDEV_DRIVER_SYS)
	// linux kernel mutexes are initialized without mutex attributes
	// PriorityInheritance is implemented in all in-kernel semaphores in real time patched linux (CONFIG_PREEMPT_RT)
	mutex_init((struct mutex*) pMutexOut);
	// mutex_init has no return value
	bResult = TRUE;
#else
	// Note:
	//	- MutexType must be recursive because CriticalSection must be nestable
	//	- PriorityInheritance:
	//		- if a low prior thread A locks a mutex and a high prior thread C wants to lock the same mutex
	//		- then a thread B with a prioity between A and C would repress the thread C   (PriorityInversion)
	//		- PriorityInheritance gives the thread A temporarily the priority of C, till thread A unlocks the mutex

	// preset
	memset(	&uMutexAttr,
			0,
			sizeof(pthread_mutexattr_t));

	if	(pthread_mutexattr_init(&uMutexAttr) != LINUX_OK)
		// error at initializing MutexAttr
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_init()!"), _TRUNCATE);
	}
	else if	(pthread_mutexattr_settype(	&uMutexAttr,
										PTHREAD_MUTEX_RECURSIVE) != LINUX_OK)
			// error at specifying MutexType
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_settype()!"), _TRUNCATE);
	}
	else if	(pthread_mutexattr_setprotocol(	&uMutexAttr,
											PTHREAD_PRIO_INHERIT) != LINUX_OK)
			// error at specifying PriorityInheritance
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutexattr_setprotocol()!"), _TRUNCATE);
	}
	else
	{
		if	(pthread_mutex_init(pMutexOut,
								&uMutexAttr) != LINUX_OK)
			// error at initializing mutex
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutex_init()!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}
#endif
	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	delete mutex
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	fnDeleteMutex(	void*			pMutexIn,
						const UINT32	lDimErrorIn,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	if	(pthread_mutex_destroy(pMutexIn) != ADN_OK)
		// error at deleting mutex
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutex_destroy()!"), _TRUNCATE);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	fnDeleteMutex(	void*			pMutexIn,
						const UINT32	lDimErrorIn,
						PNDEV_CHAR*		sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

#if defined (PNDEV_DRIVER_SYS)
	// destroy linux kernel mutex
	mutex_destroy((struct mutex*) pMutexIn);
	// mutex_destroy has no return value
	bResult = TRUE;
#else
	if	(pthread_mutex_destroy(pMutexIn) != LINUX_OK)
		// error at deleting mutex
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_mutex_destroy()!"), _TRUNCATE);
	}
	else
	{
		// success
		bResult = TRUE;
	}
#endif

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	set ThreadPar
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || (defined (PNDEV_OS_LINUX) && defined (PNDEV_DRIVER_UM))
//------------------------------------------------------------------------
BOOLEAN	fnSetThreadPar(	const UINT32		lPrioThreadIn,
						const UINT32		lStackSizeThreadIn,
						const PNDEV_CHAR*	sNameThreadIn,
                        const UINT32		lDimErrorIn,
                        void*				pThreadParOut,
						PNDEV_CHAR*			sErrorOut)
{
BOOLEAN bResult					= FALSE;
pthread_attr_t*	pThreadParTmp	= NULL;

	UNREFERENCED_PARAMETER(sNameThreadIn);

	pThreadParTmp = (pthread_attr_t*) pThreadParOut;

	if	(pthread_attr_init(pThreadParTmp) != POSIX_OK)
		// error at initializing ThreadPar
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_init()!"), _TRUNCATE);
	}
	else
	{
		// Note:
		//	- DefaultState: PTHREAD_CREATE_JOINABLE
		//	- cannot be used because pthread_join() doesn't offer a TimeoutValue
		//	- PTHREAD_CREATE_JOINABLE causes a ZombieThread at AdonisThreadList if pthread_join() is not called by another thread

		if (pthread_attr_setdetachstate(pThreadParTmp,
										PTHREAD_CREATE_DETACHED) != POSIX_OK)
			// error at setting DetachState
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setdetachstate()!"), _TRUNCATE);
		}
		else
		{
			if	(pthread_attr_setstacksize(	pThreadParTmp,
											lStackSizeThreadIn) != POSIX_OK)
				// error at setting ThreadStackSize
			{
				// set ErrorString
				_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
				_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setstacksize()!"), _TRUNCATE);
			}
			else
			{
            #if defined (PNDEV_OS_ADONIS)
			    //Setting thread name in Linux not possible for not yet existing threads
				if	(pthread_attr_setname(	pThreadParTmp,
											sNameThreadIn) != POSIX_OK)
					// error at setting ThreadName
				{
					// set ErrorString
					_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
					_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setname()!"), _TRUNCATE);
				}
				else
            #endif
				{
					if	(pthread_attr_setinheritsched(	pThreadParTmp,
														PTHREAD_EXPLICIT_SCHED) != POSIX_OK)
						// error at setting PTHREAD_EXPLICIT_SCHED at ThreadPar
						//	- service required for the following setting of priority!
					{
						// set ErrorString
						_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
						_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setinheritsched()!"), _TRUNCATE);
					}
					else
					{
					#if defined (PNDEV_OS_LINUX)
						// Set scheduling policy to PNDEV_THREAD_POLICY
						// In Linux scheduling policy has to be set, before setting ThreadPriority
						if	(pthread_attr_setschedpolicy(	pThreadParTmp,
															PNDEV_THREAD_POLICY) != POSIX_OK)
							// error at setting scheduling policy at ThreadPar
						{
							// set ErrorString
							_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
							_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setschedpolicy()!"), _TRUNCATE);
						}
						else
					#endif
						{
						struct sched_param uSchedPar;

							// set ThreadPriority
							uSchedPar.sched_priority = lPrioThreadIn;

							if	(pthread_attr_setschedparam(pThreadParTmp,
															&uSchedPar) != POSIX_OK)
								// error at storing new SchedulingPar at ThreadPar
							{
								// set ErrorString
								_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
								_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at pthread_attr_setschedparam()!"), _TRUNCATE);
							}
							else
							{
								// success
								bResult = TRUE;
							}
						}
					}
				}
			}
		}
	}

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//########################################################################
//	INTERNAL
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert a string to LowerCase
//************************************************************************

//------------------------------------------------------------------------
#if defined (TOLOWER)
//------------------------------------------------------------------------
errno_t _tcslwr_s(	PNDEV_CHAR*	sIn,
					size_t		lCountBufIn)
{
errno_t	lResult	= -1;
size_t	lNoChar	= 0;

	// get real number of characters
	lNoChar = _tcslen(sIn);

	if	(lNoChar < lCountBufIn)
		// valid number of real characters (terminal NULL must be regarded)
	{
	UINT32 i = 0;

		// success
		lResult = 0;

		for	(i = 0; i < lNoChar; i++)
			// all characters
		{
			// convert to LowerCase
			sIn[i] = tolower(sIn[i]);
		}
	}

	return(lResult);
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert a string to UpperCase
//************************************************************************

//------------------------------------------------------------------------
#if defined (TOUPPER)
//------------------------------------------------------------------------
errno_t _tcsupr_s(	PNDEV_CHAR*	sIn,
					size_t		lCountBufIn)
{
errno_t	lResult	= -1;
size_t	lNoChar	= 0;

	// get real number of characters
	lNoChar = _tcslen(sIn);

	if	(lNoChar < lCountBufIn)
		// valid number of real characters (terminal NULL must be regarded)
	{
	UINT32 i = 0;

		// success
		lResult = 0;

		for	(i = 0; i < lNoChar; i++)
			// all characters
		{
			// convert to UpperCase
			sIn[i] = toupper(sIn[i]);
		}
	}

	return(lResult);
}
#endif

#ifdef PNDEV_DRIVER_UM                          //Usermode
//************************************************************************
//  D e s c r i p t i o n :
//
//	convert ASCII to Unicode string
//************************************************************************

void fnConvertStringCharToWchar(const char*	sIn,
								wchar_t*	sResultOut)
{
size_t i = 0;

	// copy char to wchar_t inclusive terminal NULL
	for (i = 0; i <= strlen(sIn); i++)
	{
		sResultOut[i] = (wchar_t) sIn[i];
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert ASCII to _TCHAR string
//************************************************************************

void fnConvertStringCharToTchar(const char*	sIn,
								PNDEV_CHAR*	sResultOut)
{
size_t i = 0;

	// copy char to _TCHAR inclusive terminal NULL
	for (i = 0; i <= strlen(sIn); i++)
	{
		sResultOut[i] = (PNDEV_CHAR) sIn[i];
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert Unicode to ASCII string
//************************************************************************

#if defined (PNDEV_UNICODE)

void fnConvertStringWcharToChar(const wchar_t*	sIn,
								char*			sResultOut)
{
size_t i = 0;

	// copy wchar_t to char inclusive terminal NULL
	for (i = 0; i <= wcslen(sIn); i++)
	{
		sResultOut[i] = (char) sIn[i];
	}
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert Unicode to _TCHAR string
//************************************************************************

#if defined (PNDEV_UNICODE)

void fnConvertStringWcharToTchar(const wchar_t*	sIn,
								PNDEV_CHAR*		sResultOut)
{
size_t i = 0;

	// copy wchar_t to _TCHAR inclusive terminal NULL
	for (i = 0; i <= wcslen(sIn); i++)
	{
		sResultOut[i] = (PNDEV_CHAR) sIn[i];
	}
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert _TCHAR to Unicode string
//************************************************************************

void fnConvertStringTcharToWchar(const PNDEV_CHAR*	sIn,
								wchar_t*			sResultOut)
{
size_t i = 0;

	// copy _TCHAR to wchar_t inclusive terminal NULL
	for (i = 0; i <= _tcslen(sIn); i++)
	{
		sResultOut[i] = (wchar_t) sIn[i];
	}
}

#endif //PNDEV_DRIVER_UM

//************************************************************************
//  D e s c r i p t i o n :
//
//	convert _TCHAR to ASCII string
//************************************************************************

void fnConvertStringTcharToChar(const PNDEV_CHAR*	sIn,
								char*				sResultOut)
{
size_t i = 0;

	// copy _TCHAR to char inclusive terminal NULL
	for (i = 0; i <= _tcslen(sIn); i++)
	{
		sResultOut[i] = (char) sIn[i];
	}
}
