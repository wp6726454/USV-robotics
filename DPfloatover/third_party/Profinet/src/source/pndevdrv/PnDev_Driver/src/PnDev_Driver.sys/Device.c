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
/*  F i l e               &F: Device.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Entry points of device
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Device.tmh"
#endif

#ifdef ALLOC_PRAGMA
	#pragma alloc_text (PAGE, fnAddDevice)
	#pragma alloc_text (PAGE, fnPrepareHw)
	#pragma alloc_text (PAGE, fnReleaseHw)
	#pragma alloc_text (PAGE, fnShutdown)
	#pragma alloc_text (PAGE, fnGetPciLocShort)
//	#pragma alloc_text (PAGE, fnCheckSharedHostMemUM)
//	#pragma alloc_text (PAGE, fnMapSharedHostMemUM)
//	#pragma alloc_text (PAGE, fnReleaseSharedHostMemUM)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  AddDevice
//************************************************************************

BOOLEAN fnAddDevice(DEVICE_EXTENSION*		pDevExtIn,
					HANDLE					hOsParIn,
					const BOOLEAN			bOsDpcSupportIn,
					const ePNDEV_BOARD		eBoardIn,
					const uPNDEV_PCI_LOC*	pPciLocIn,
					const UINT32			lDimErrorIn,
					_TCHAR*					sErrorOut)
{
BOOLEAN	bResult = FALSE;

	PAGED_CODE();

	// initialize AppList
	fnInitList(&pDevExtIn->uListApp);

	// preset TimeScaleFactor
	//	- Windows: cannot be passed by OpenDriver because of Plug&Play
	//	- overwritten at first OpenDevice
	pDevExtIn->lTimeScaleFactor = 1;

	// store parameters
	pDevExtIn->hDeviceOs		= hOsParIn;
	pDevExtIn->eBoard			= eBoardIn;
	pDevExtIn->bOsDpcSupport	= bOsDpcSupportIn;

	// update PciLoc
	{
		*((uPNDEV_PCI_LOC*) &pDevExtIn->uPciLoc) = *pPciLocIn;

		if	(_tcscmp(pDevExtIn->uPciLoc.sString, _TEXT("")) != 0)
			// PciLoc as string
		{
			// get PciLocShort
			fnGetPciLocShort(pDevExtIn);
		}
		else
		{
			// set PciLoc
			_RtlStringCchPrintf(pDevExtIn->uPciLoc.sString,
								_countof(pDevExtIn->uPciLoc.sString),
								_TEXT("%u,%u,%u"),
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);

			// set PciLocShort
			_RtlStringCchPrintf(pDevExtIn->sPciLocShort,
								_countof(pDevExtIn->sPciLocShort),
								_TEXT("%u,%u,%u"),
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);

			// set PciLocShortAscii
			RtlStringCchPrintfA(pDevExtIn->sPciLocShortAscii,
								_countof(pDevExtIn->sPciLocShortAscii),
								"%u,%u,%u",
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);
		}
	}

	if	(fnStartUpLockRes(	pDevExtIn,
							lDimErrorIn,
							sErrorOut))
		// starting up resources for locking ok
	{
	uPNDEV_CONFIG_SPACE	uConfigSpace;

		// Note:
		//	- at startup a LinkTraining is done
		//	- these tests may cause PCIe errors.
		//	  -> they must be cleared after startup

		if	(fnGetInfoConfigSpace(	pDevExtIn,
									lDimErrorIn,
									&uConfigSpace,
									sErrorOut))
			// getting ConfigSpaceInfo ok
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  prepare hardware
//************************************************************************

BOOLEAN fnPrepareHw(DEVICE_EXTENSION*	        pDevExtIn,
					HANDLE				        hOsParIn,
                    const ePNDEV_RESET_ACTION	eResetActionIn,
					const UINT32		        lDimErrorIn,
					_TCHAR*				        sErrorOut)
{
BOOLEAN	bResult = FALSE;

	PAGED_CODE();

	if	(fnBoard_GetHwResKm(pDevExtIn,
							hOsParIn,
							lDimErrorIn,
							sErrorOut))
		// getting HwRes of board ok
	{
	BOOLEAN	bResultMap = TRUE;
	UINT32	i = 0;

		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			if	(!fnMapMemPhyToVirtual(	pDevExtIn,
										&pDevExtIn->uHwResKm.uBar[i],
										i,
										lDimErrorIn,
										sErrorOut))
				// error at mapping a range of physical addresses to non-paged virtual memory
			{
				bResultMap = FALSE;

				// leave loop
				break;
			}
			else
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: BAR%u: PhyAdr=0x%llx, VirtualAdr=0x%p, size=0x%x",
									pDevExtIn->sPciLocShortAscii,
									i,
									pDevExtIn->uHwResKm.uBar[i].uPhyAdr.QuadPart,
									pDevExtIn->uHwResKm.uBar[i].pVirtualAdr,
									pDevExtIn->uHwResKm.uBar[i].lSize);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}
		}

		if	(bResultMap)
			// success
		{
			if	(fnBoard_SetPtrHwResKm(	pDevExtIn,
										lDimErrorIn,
										sErrorOut))
				// setting KernelModePtr to HwRes ok
			{
				// KernelModePtr to HwRes ok
				pDevExtIn->bPtrHwResKmOk = TRUE;

				// Note:
				//	- HwReset is done by PowerOn or fnReleaseHw()
				//	- for connecting interrupts (fnConnectInt) HW must be initialized (IntPolarity!)

				if	((*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
                                                                        eResetActionIn,
																		lDimErrorIn,
																		sErrorOut))
					// resetting board ok
				{
					// success
					bResult = TRUE;
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  release hardware
//************************************************************************

BOOLEAN fnReleaseHw(DEVICE_EXTENSION*	pDevExtIn,
					const UINT32		lDimErrorIn,
					_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bResultClose	= FALSE;
UINT32	i = 0;

	PAGED_CODE();

	// Note:
	//	- function only called by Windows if there are no more Apps independant if service CloseDevice was called correctly
	//	- SysThread may not be terminated
	//		-> exception if virtual memory is released afterwards
	//		-> cleanup required!

	// preset
	bResultClose = TRUE;

	if	(pDevExtIn->lCtrApp != 0)
		// App exist
	{
	uLIST_HEADER*	pList	= NULL;
	uPNDEV_APP*		pApp	= NULL;

		pList = &pDevExtIn->uListApp;

		while	(!fnListEmpty(pList))
				// AppList not empty
		{
			// get first block of AppList
			pApp = (uPNDEV_APP*) pList->pNext;

			if	(!fnCloseDevice(pDevExtIn,
								pApp,
								lDimErrorIn,
								sErrorOut))
				// error at closing device
				//	- remove block
			{
				bResultClose = FALSE;
			}
		}
	}

	if	(bResultClose)
		// success
	{
	BOOLEAN	bResultReset = TRUE;

		if	(pDevExtIn->bPtrHwResKmOk)
			// KernelModePtr to HwRes ok
		{
			if	(!(*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																	ePNDEV_RESET_ACTION_RELEASE_HW,
																	lDimErrorIn,
																	sErrorOut))
				// error at resetting board
			{
				bResultReset = FALSE;
			}
		}

		if	(bResultReset)
			// success
		{
			if	(fnDisconnectInt(	pDevExtIn,
									lDimErrorIn,
									sErrorOut))
				// disconnecting interrupt ok
			{
				if	(fnShutDownLockRes(	pDevExtIn,
										lDimErrorIn,
										sErrorOut))
					// shutting down resources for locking ok
				{
					// success
					bResult = TRUE;
				}
			}
		}

		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			// release virtual memory
			fnReleaseMemVirtual(pDevExtIn,
								&pDevExtIn->uHwResKm.uBar[i]);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down
//	- called from D0Exit when the device is torn down or when the system is shutdown
//	- called at PASSIVE_LEVEL
//************************************************************************

void fnShutdown(DEVICE_EXTENSION* pDevExtIn)
{
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]	= {0};

	// Note:
	//	- Reset the device to put the device in a known initial state
	//	- WDF has already called out EvtDisable callback to disable the interrupt

	PAGED_CODE();

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: ---- %s",
						pDevExtIn->sPciLocShortAscii,
						__FUNCTION__);

	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				FALSE,						// logging
				sTraceDetail);

	// WdfInterrupt is already disabled so issue a full reset

	// function called before fnEvtDeviceReleaseHw() -> no ResetDevice necessary
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get PciLocShort
//
//	Note:
//	- don't return an error because PciLocShort is only used for trace
//	- creating PciLocShort may fail e.g. in china if there is no comma
//************************************************************************

void fnGetPciLocShort(DEVICE_EXTENSION* pDevExtIn)
{
_TCHAR*	sComma1	= NULL;
_TCHAR*	sComma2	= NULL;
char	sPciLocAscii[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]		= {0};

	PAGED_CODE();

	// convert _TCHAR to ASCII string
	fnConvertStringTcharToChar(	pDevExtIn->uPciLoc.sString,
								sPciLocAscii);

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[x,x,x]: PciLoc=%s",
						sPciLocAscii);

	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				TRUE,						// logging
				sTraceDetail);

	// Note:
	//	- the nomenclature of PciLoc depends on used language settings
	//	- examples:
	//		- "PCI bus 5, device 10, function 0"
	//		- "PCI Slot 3 (PCI bus 5, device 10, function 0)"
	//		- "PCI-Bus 5, Ger�t 10, Funktion 0"
	//		- "PCI-Steckplatz 4 (PCI-Bus 5, Ger�t 10, Funktion 0)"
/*
	// debug purpose:
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI bus 5, device 10, function 0"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI Slot 3 (PCI bus 5, device 10, function 0)"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI-Bus 5, Ger�t 10, Funktion 0"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI-Steckplatz 4 (PCI-Bus 5, Ger�t 10, Funktion 0)"));
*/
	// search for first comma
	sComma1 = _tcsstr(pDevExtIn->uPciLoc.sString, _TEXT(","));

	if	(sComma1 == NULL)
		// error
	{
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ########## First comma at PciLocation not found!");

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
		// search for second comma
		sComma2 = _tcsstr((sComma1 + 1), _TEXT(","));

		if	(sComma2 == NULL)
			// error
		{
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[x,x,x]: ########## Second comma at PciLocation not found!");

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
		_TCHAR*	sBlankBeforeComma1 = NULL;
		UINT32	i = 0;

			// search for blank before comma1
			{
				// temporary: terminate string at comma1
				*sComma1 = _TEXT('\0');

				// search for last blank
				//	- search backward
				sBlankBeforeComma1 = _tcsrchr(pDevExtIn->uPciLoc.sString, _TEXT(' '));

				// undo change at comma1
				*sComma1 = _TEXT(',');
			}

			if	(sBlankBeforeComma1 == NULL)
				// error
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: ########## Blank before comma1 at PciLocation not found!");

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
			UINT32	lLen	= 0;
			_TCHAR	cChar	= _TEXT('\0');
			UINT32	k		= 0;

				lLen = _tcslen(sBlankBeforeComma1);

				for	(i = 0; i < lLen; i++)
					// all characters of string 'blank before comma1'
				{
					cChar = sBlankBeforeComma1[i];

					if	(	(	(cChar >= _TEXT('0'))
							&&	(cChar <= _TEXT('9')))
						||	(cChar == _TEXT(',')))
						//		0..9
						// OR	comma
					{
						// add character to TargetString
						pDevExtIn->sPciLocShort[k] = cChar;
						k++;
					}
				}

				// add terminating null
				pDevExtIn->sPciLocShort[k] = _TEXT('\0');

				// convert _TCHAR to ASCII string
				fnConvertStringTcharToChar(	pDevExtIn->sPciLocShort,
											pDevExtIn->sPciLocShortAscii);

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: PciLocShort computed",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							FALSE,						// logging
							sTraceDetail);
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	get Memory for Device
//************************************************************************

BOOLEAN fnCheckSharedHostMemUM(	DEVICE_EXTENSION*	pDevExtIn,
								BOOLEAN				bReqHostMem,
								BOOLEAN				bMasterApp,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

    if	(bReqHostMem)
    	// HostMem required
    {
        if	(pDevExtIn->eValidHostMemPool != ePNDEV_VAL_HOST_MEM_INVALID)
			// valid BoardType
		{
		BOOLEAN	bHostMem	= FALSE;
		INT32 	i 			= 0;

			if	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_ALL)
				// all pools are valid
			{
				for	(i=3; i>=0; i--)
					// valid HostMemIndex
				{
					if	(	(!g_uMemSys.uHostMem[i].bInUse)
						&& 	(g_uMemSys.uHostMem[i].pAdrVirt != NULL))
						// memory is available
						// && memory pointer invalid - DriverEntry was not able to allocate memory
					{
						// success
						bHostMem = TRUE;

						break;
					}
				}
			}

			if	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_FIRST)
				// only first pool valid
			{
				if	(	(!g_uMemSys.uHostMem[0].bInUse)
					&& 	(g_uMemSys.uHostMem[0].pAdrVirt != NULL))
					// memory0 is available
					// && memory pointer invalid - DriverEntry was not able to allocate memory
				{
					// success
					bHostMem = TRUE;
				}
			}

			if 	(bHostMem)
				// got HostMemory
			{
					pDevExtIn->bUsesHostMem = TRUE;
					bResult 				= TRUE;
			}
			else
			{
				// invalid board type
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Can not provide contiguous shared host memory for device: The corresponding resources are already in use or Memory allocation failed at DriverEntry!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
		}
		else
		{
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Can not provide contiguous shared host memory for device: Board type not supported!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
    }
    else
    {
    	// success
        bResult = TRUE;
    }

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	Map SharedHostMem UserMode
//************************************************************************

BOOLEAN fnMapSharedHostMemUM(	DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
								uPNDEV_SHARED_HOST_SDRAM*			pSharedHostSdram,
								_TCHAR*								sErrorOut)
{
BOOLEAN bResult 		= FALSE;
INT32	lFoundHostMem 	= -1;

    if	(pDevExtIn->bUsesHostMem)
    	// HostMem available
    {
    INT32 i = 0;

    	for	(i = 3; i >= 0; i--)
    		// valid HostMemIndex
    	{
    		if (!g_uMemSys.uHostMem[i].bInUse)
    			// not InUse
    		{
	            if	(	((pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_FIRST) && (i == 0))	// only HostMemory0 valid
	            	||	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_ALL))					// all HostMemories valid
	            	// valid HostMemory
	            {
	            	if 	(pDevExtIn->lUsesHostMemIndex == i)
	            		// right Index
	            	{
						if	(pInternalIn->uHostMemUm.uAdrUser.pPtr != NULL)
							// valid ptr
						{
							g_uMemSys.uHostMem[i].uHwResUserSpace.uAdrUser.pPtr	= pInternalIn->uHostMemUm.uAdrUser.pPtr;
							g_uMemSys.uHostMem[i].uHwResUserSpace.lSize 		= pInternalIn->uHostMemUm.lSize;
							g_uMemSys.uHostMem[i].uHwResUserSpace.uMdl.pPtr		= pInternalIn->uHostMemUm.uMdl.pPtr;

							// success
							lFoundHostMem = i;

							break;
						}
						else
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): No valid ptr from MappingVirtualToUM!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

							break;
						}
	            	}
	            	else
	            	{
	            		// set ErrorString
	            		fnBuildString(	_TEXT(__FUNCTION__),
	            						_TEXT("(): No valid lUsesHostMemIndex!"),
	            						NULL,
	            						lDimErrorIn,
	            						sErrorOut);
	            	}
	            }
    		}
    	}

    	if	(lFoundHostMem > -1)
    		// success
    	{
    		if 	(g_uMemSys.uHostMem[lFoundHostMem].pAdrVirt != NULL)
    			// ptr valid
    		{
    			// set UsedFlag
    			g_uMemSys.uHostMem[lFoundHostMem].bInUse = TRUE;

    			// set SharedHostSdram
    			pSharedHostSdram->uBase.pPtr   	=  g_uMemSys.uHostMem[lFoundHostMem].uHwResUserSpace.uAdrUser.pPtr;
    			pSharedHostSdram->lPhyAdr      	=  g_uMemSys.uHostMem[lFoundHostMem].lPhyAdr;
    			pSharedHostSdram->lSize        	=  g_uMemSys.uHostMem[lFoundHostMem].uHwResUserSpace.lSize;
    			pDevExtIn->lUsesHostMemIndex	=  lFoundHostMem;

    			if 	(pDevExtIn->eAsic == ePNDEV_ASIC_AM5728)
    				// TI Omap
    			{   		
    			UINT32 lTmpGmacVersion = 0;
    			
    				lTmpGmacVersion = *((UINT32*)pDevExtIn->uHwResKm.uBoard.as.uOmap.uGmacSw.pPtr);
    				
    				if (fnBoardOmap_ConfigPciMaster(pDevExtIn,
    												pSharedHostSdram->lPhyAdr,
    												pSharedHostSdram->lSize,
    												lDimErrorIn,
    												sErrorOut))
    					// ConfigPciMaster
    				{
    					// success
    					bResult = TRUE;
    				}
    			}
    			else
    			{
    				// success
    				bResult = TRUE;	
    				
    			}
    		}
    	}
    }
    else
    {
    	// success
    	bResult = TRUE;
    }

    return (bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	Release SharedHostMem UserMode
//************************************************************************

void fnReleaseSharedHostMemUM(DEVICE_EXTENSION*	pDevExtIn)
{

    if	(	(pDevExtIn->eAsic == ePNDEV_ASIC_ERTEC400)
    	||	(pDevExtIn->eAsic == ePNDEV_ASIC_SOC1)
    	||	(pDevExtIn->eAsic == ePNDEV_ASIC_I210)
		||	(pDevExtIn->eAsic == ePNDEV_ASIC_I82574)
    	||	(pDevExtIn->eAsic == ePNDEV_ASIC_KSZ8841)
    	||	(pDevExtIn->eAsic == ePNDEV_ASIC_KSZ8842))
		// valid AsicType
	{


    // mk5656 ToDo:
    /*UINT32	lDimError = 0;
    char 	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};


		// wait
		LARGE_INTEGER lValueTimer;
		lValueTimer.QuadPart = -20000;

		if	(KeDelayExecutionThread(KernelMode, FALSE, &lValueTimer) != STATUS_SUCCESS)
			// error
		{
		}
		*/

		/*if	(!fnDelayThread(pDevExtIn,
							1,
							lDimError,
							sTraceDetail))
			// error at starting WaitTime of current thread ok
		{
			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_BASIC,
						FALSE,						// logging
						sTraceDetail);
		}
		else
		{*/
			// success
			if(pDevExtIn->bUsesHostMem)
			{
				fnReleaseMemUser(	pDevExtIn,       // devExt not used
									&(g_uMemSys.uHostMem[pDevExtIn->lUsesHostMemIndex].uHwResUserSpace));

				// reset ptr
				g_uMemSys.uHostMem[pDevExtIn->lUsesHostMemIndex].bInUse	= FALSE;
				pDevExtIn->bUsesHostMem									= FALSE;
				pDevExtIn->lUsesHostMemIndex 							= 0;

			}
		//}
	}
}

